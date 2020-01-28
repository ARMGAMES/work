#include "dbm.h"
#include "dbmstmt.h"
#include "cropdate.h"
#include "initdatetime.h"

class GetNextCropDateStmt : public DbmStatement
{
	SQLINTEGER			cropType;
	SQL_DATE_STRUCT		cropDate;
	SQLLEN			cbType, cbCropDate;
	void prepareStmt(const char* daemonLogTableName)
	{
		PString stmt("SELECT MAX(CROPDATE) + 1 DAY AS NEXTDATE FROM ");
		stmt.append(daemonLogTableName).append(" WHERE CROPTYPE = ?");
		prepare(stmt);
		bindIntParam(1, &cropType, &cbType);
		bindDateCol(1, &cropDate, &cbCropDate);
	}
public:
	GetNextCropDateStmt( DatabaseManagerCommon& man, UINT32 type, const char* daemonLogTableName )
		: DbmStatement(man)
	{
		cropType = type;
		prepareStmt(daemonLogTableName);
	}
	bool lastCropFound() const { return SQL_NULL_DATA != cbCropDate; }
	const SQL_DATE_STRUCT* getCropDate() const { return &cropDate; }
};

class SaveLastCroppedStmt : public DbmStatement
{
	SQLINTEGER			cropType;
	SQL_DATE_STRUCT		lastCropped;
	SQL_TIMESTAMP_STRUCT finished;
	SQLLEN			cbType, cbLastCropped, cbFinished, cbHostId;
	SQLINTEGER	hostId;	// PYR-28221
	void prepareStmt(const char* daemonLogTableName)
	{
		PString stmt("INSERT INTO ");
		stmt.append(daemonLogTableName).append(" (CROPTYPE, CROPDATE, FINISHED, HOSTID) VALUES (?, ?, ?, ?)");
		prepare(stmt);
		bindIntParam(1, &cropType, &cbType);
		bindDateParam(2, &lastCropped, &cbLastCropped);
		bindTimeParam(3, &finished, &cbFinished);
		bindIntParam(4, &hostId, &cbHostId);
	}
public:
	SaveLastCroppedStmt( DatabaseManagerCommon& man, UINT32 type, const SrvDate& srvCropped, const SrvTime&, const char* daemonLogTableName, UINT32 hostId_ );
};

SaveLastCroppedStmt::SaveLastCroppedStmt( DatabaseManagerCommon& man, UINT32 type, const SrvDate& srvCropped, const SrvTime& srvFinished, const char* daemonLogTableName, UINT32 hostId_ )
		: DbmStatement(man)
{
	cropType = type;
	initDateStruct( lastCropped, srvCropped );
	initTimeStruct( finished, srvFinished );
	hostId = hostId_;
	prepareStmt(daemonLogTableName);
}

INT16 CropDate::getNextCropDate(UINT32 cropType, SrvDate& lastCropped, PString& sqlErr)
{
	try
	{
		GetNextCropDateStmt stmt(dbmObj, cropType, daemonLogTableName);
		stmt.execute();
		bool ok = stmt.fetch(); //only one row
		stmt.closeCursor();
		if(!ok)
		{
			sqlErr = "NO FETCH";
			PLog("%s", sqlErr.c_str());
			return DBM_INTERNAL_ERROR;
		}
		if(!stmt.lastCropFound())
		{
			sqlErr = "NO CROP FOUND OF SPECIFIED TYPE ";
			sqlErr.appendInt(cropType);
			PLog("%s", sqlErr.c_str());
			return INTERNAL_LOGICAL_ERROR;
		}
		initSrvDate(lastCropped, stmt.getCropDate());
		return DBM_NO_ERROR;
		
	}
	catch(const PSqlError& er)
	{
		PLog("getNextCropDate: #%d '%s'", er.code(), er.why());
		sqlErr = er.why();
		return er.code();
	}
}

INT16 CropDate::saveCropDate(UINT32 cropType, const SrvDate& cropDate, PString& sqlErr)
{
	try
	{
		SrvTime now;
		//currentLocalTime used, only server time is interesting here, no actual message time
		now.currentLocalTime();
		//dbmObj->getMsgTime(now);
		SaveLastCroppedStmt stmt(dbmObj, cropType, cropDate, now, daemonLogTableName, dbmObj.getHostId());
		stmt.execute();
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er)
	{
		PLog("saveCropDate: #%d '%s'", er.code(), er.why());
		sqlErr = er.why();
		return er.code();
	}
}

//////////////////////////////////////////////////////////////////////////
// copied handlers from rdcroptime for PYR-15607 Home Games
//
class GetMaxCropTimeStmt : public DbmStatement
{
	PSqlInt	 cropType;
	PSqlTimestamp cropTime;

	void prepareStmt()
	{
		PString stmt("SELECT MAX(CROPTIME) FROM DAEMONTIMELOG WHERE CROPTYPE = ?");

		prepare(stmt);

		bindFirstParam(cropType);
		bindFirstCol(cropTime);
	}
public:
	GetMaxCropTimeStmt( DatabaseManagerCommon& man, UINT32 type)
		: DbmStatement(man) 
	{
		cropType = type;
		prepareStmt();
	}	
	bool lastCropNotFound() const { return cropTime.isNull(); }
	void getCropTime(SrvTime& inTime) const { cropTime.getValue(inTime); }
	const PSqlTimestamp* getCropTime() const { return &cropTime; }
};

//////////////////////////////////////////////////////////////////////////

INT16 CropTime::getNextCropTime(UINT32 cropType, SrvTime& lastCropped, PString& sqlErr)
{
	try
	{
		GetMaxCropTimeStmt stmt(dbmObj, cropType);
		stmt.execute();
		bool ok = stmt.fetch();
		stmt.closeCursor();
		if(!ok)
		{
			sqlErr = "NO FETCH";
			PLog("%s", sqlErr.c_str());
			return DBM_INTERNAL_ERROR;
		}
		if(stmt.lastCropNotFound())
		{
			sqlErr = "NO CROP TIMESTAMP FOUND OF SPECIFIED TYPE ";
			sqlErr.appendInt(cropType);
			PLog("%s", sqlErr.c_str());
			return INTERNAL_LOGICAL_ERROR;
		}
		stmt.getCropTime()->getValue(lastCropped);
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er)
	{
		PLog("getNextCropTime: #%d '%s'", er.code(), er.why());
		sqlErr = er.why();
		return er.code();
	}
}

INT16 CropTime::saveCropTime(UINT32 cropType, const SrvTime& cropTime, PString& sqlErr)
{
	try
	{
		SrvTime maxTime;
		GetMaxCropTimeStmt maxTimeStmt(dbmObj, cropType);
		maxTimeStmt.execute();

		if (maxTimeStmt.fetch())
			maxTimeStmt.getCropTime(maxTime);

		if (!cropTime.equals(maxTime))
		{
			SrvTime now;
			now.currentLocalTime();		
			SaveLastCroppedTimeStmt stmt(dbmObj, cropType, cropTime, now, dbmObj.getHostId());
			stmt.execUpdateDelete();
		}
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er)
	{
		PLog("saveCropTime: #%d '%s'", er.code(), er.why());
		sqlErr = er.why();
		return er.code();
	}
}