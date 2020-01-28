#ifndef cropdate_h_included
#define cropdate_h_included

#include "pyrtime.h"
#include "dbcommon.h"
#include "dbmstmt.h"

class CropDate
{
	PString daemonLogTableName;
public:
	CropDate(DatabaseManagerCommon& man, const char* daemonLogTableName_ = "DAEMONLOG")
		: daemonLogTableName(daemonLogTableName_), dbmObj(man)
	{}

	INT16 getNextCropDate(UINT32 cropType, SrvDate& lastCropped, PString& sqlErr);
	INT16 saveCropDate(UINT32 cropType, const SrvDate& cropDate, PString& sqlErr);

private:
	DatabaseManagerCommon& dbmObj;
};

//////////////////////////////////////////////////////////////////////////
// PYR-15607 Home Games
class CropTime
{
public:
	CropTime(DatabaseManagerCommon& man) : dbmObj(man) {};
	INT16 getNextCropTime(UINT32 cropType, SrvTime& lastCropped, PString& sqlErr);
	INT16 saveCropTime(UINT32 cropType, const SrvTime& cropTime, PString& sqlErr);
private:
	DatabaseManagerCommon& dbmObj;	
};
//////////////////////////////////////////////////////////////////////////
// moved from cropdate.cpp
class SaveLastCroppedTimeStmt : public DbmStatement
{
	PSqlInt			cropType;
	PSqlTimestamp	lastCropped;
	PSqlTimestamp   finished;
	PSqlInt			hostId;	// PYR-28221

	void prepareStmt()
	{		
		PString stmt("INSERT INTO DAEMONTIMELOG (CROPTYPE, CROPTIME, FINISHED, HOSTID) VALUES (?, ?, ?, ?)");

		prepare(stmt);
		bindFirstParam(cropType);
		bindNextParam(lastCropped);
		bindNextParam(finished);
		bindNextParam(hostId);
	}
public:
	SaveLastCroppedTimeStmt(DatabaseManagerCommon& man, 
		UINT32 type, 
		const SrvTime& srvCropped, 
		const SrvTime& srvFinished,
		UINT32 hostId_) : DbmStatement(man)
	{
		cropType = type;
		lastCropped.init(srvCropped);
		finished.init(srvFinished);
		hostId = hostId_;
		prepareStmt();
	}
};
//////////////////////////////////////////////////////////////////////////
// moved from daemondbmanager.cpp
class SaveLastCroppedTimeStmtWithLicenseId : public DbmStatement
{
	PSqlInt			cropType;
	PSqlTimestamp	lastCropped;
	PSqlTimestamp   finished;
	PSqlInt			licenseId;
	PSqlInt			hostId;	// PYR-28221

	void prepareStmt()
	{		
		PString stmt("INSERT INTO DAEMONTIMELOG (CROPTYPE, CROPTIME, FINISHED, LICENSEID, HOSTID) VALUES (?, ?, ?, ?, ?)");

		prepare(stmt);
		bindFirstParam(cropType);
		bindNextParam(lastCropped);
		bindNextParam(finished);
		bindNextParam(licenseId);
		bindNextParam(hostId);
	}
public:
	SaveLastCroppedTimeStmtWithLicenseId(	DatabaseManagerCommon& man, 
		UINT32 cropType_, 
		const SrvTime& srvCropped_, 
		const SrvTime& srvFinished_,
		UINT32 hostId_) : DbmStatement(man)
	{
		cropType = cropType_;
		lastCropped.init(srvCropped_);
		finished.init(srvFinished_);
		licenseId = eLicenceDefault;
		hostId = hostId_;
		prepareStmt();
	}
	void init(const UINT32 licenseId_)
	{
		licenseId = licenseId_;
	}
};

class SelectCropRecordStmtByTimeAndLicenseId : public DbmStatement	// PYR-33749
{
	PSqlInt			cropType;
	PSqlTimestamp	cropTime;
	PSqlInt			licenseId;
	PSqlInt			records;
	
	void prepareStmt()
	{		
		PString stmt("SELECT count(*) FROM DAEMONTIMELOG WHERE CROPTYPE = ? AND CROPTIME = ? AND LICENSEID = ?");

		prepare(stmt);
		bindFirstCol(records);

		bindFirstParam(cropType);
		bindNextParam(cropTime);
		bindNextParam(licenseId);
	}
public:
	SelectCropRecordStmtByTimeAndLicenseId(DatabaseManagerCommon& man) : DbmStatement(man)
	{
		prepareStmt();
	}
	void init(const INT32 cropType_, const SrvTime& cropTime_, const UINT32 licenseId_)
	{
		cropType = cropType_;
		cropTime = cropTime_;
		licenseId = licenseId_;
	}
	bool isRecordExist() const
	{
		return (records.value != 0);
	}
};

//////////////////////////////////////////////////////////////////////////

#endif	//cropdate_h_included
