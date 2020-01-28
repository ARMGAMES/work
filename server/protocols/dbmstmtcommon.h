#ifndef dbmstmtcommon_h_included
#define	dbmstmtcommon_h_included

#include "dbmstmt.h"
#include "initdatetime.h"
#include "protocolstruct.h"
#include "dbmstmtolap.h"

class GetPropertyStmt : public DbmStatement
{
	void prepareStmt()
	{
		prepare("SELECT PROPERTYINT, PROPERTYSTR, PROPERTYWHEN FROM PROPERTIES WHERE PROPERTYID = ?");
		cbId = SQL_NTS;
		bindStrParam( 1, PROP_ID_LEN, propertyId, &cbId );
		bindIntCol( 1, &propertyInt, &cbInt );
		bindStrCol( 2, propertyStr, PROP_STR_LEN, &cbStr );
		bindTimeStampCol( 3, &propertyWhen, &cbWhen );
	}
public:
	SQLINTEGER				propertyInt;
	SQLCHAR					propertyId[PROP_ID_LEN];
	SQLCHAR					propertyStr[PROP_STR_LEN];
	SQL_TIMESTAMP_STRUCT	propertyWhen;
	SQLLEN				cbId, cbInt, cbStr, cbWhen;

	GetPropertyStmt(DatabaseManagerCommon& man) :DbmStatement(man) 
	{
		prepareStmt();
	}
	bool isWhenSet() const
	{ 
		return cbWhen != SQL_NULL_DATA; 
	}
	void setPropertyId( const char* id )
	{
		strCopy((char*)propertyId, sizeof(propertyId), id );
	}
	void getWhen(SrvTime& when) const
	{
		initSrvTime(when, &propertyWhen); 
	}
	const char* getPropId() const
	{
		return (const char*) propertyId;
	}
	const char* getPropStr() const
	{
		return (const char*) propertyStr;
	}
	int getPropInt() const
	{
		return propertyInt;
	}
	void getData(Property& prop) const // #10125
	{
		prop.propInt  = propertyInt;
		prop.propStr  = getPropStr();
		getWhen(prop.propWhen);
		prop.isWhenSet = isWhenSet();
	}
};

class GetPropertyLikeStmt : public GetPropertyStmt
{
	SQLCHAR patternFilter[PROP_ID_LEN];
	SQLLEN cbPattern;
	void prepareStmt()
	{
		// #PYR-12547 - added "WITH UR"
		prepare("SELECT PROPERTYINT, PROPERTYSTR, PROPERTYWHEN, PROPERTYID FROM PROPERTIES WHERE PROPERTYID LIKE ? WITH UR");
		cbId = SQL_NTS;
		bindStrParam( 1, sizeof(patternFilter), patternFilter, &cbPattern );
		bindIntCol( 1, &propertyInt, &cbInt );
		bindStrCol( 2, propertyStr, sizeof(propertyStr), &cbStr );
		bindTimeStampCol( 3, &propertyWhen, &cbWhen );
		bindStrCol( 4, propertyId, sizeof(propertyId), &cbId );
	}
public:
	GetPropertyLikeStmt(DatabaseManagerCommon& man, const char* pattern) : GetPropertyStmt(man)
	{
		strCopy((char*)patternFilter, sizeof(patternFilter), pattern);
		prepareStmt();
	}
	const char* getPropertyId() const { return (const char*) propertyId; }
};

class DeletePropertyStmt : public DbmStatementWithLog // PYR-117171
{
	PSqlString<PROP_ID_LEN> propertyId;
	void prepareStmt()
	{
		// #DBA_REVIEWED #PYR-117171 HaiweiY 2019.Build.01
		prepare("DELETE FROM PROPERTIES WHERE PROPERTYID = ?");
		bindFirstParam(propertyId);
	}
public:
	DeletePropertyStmt(DatabaseManagerCommon& man) : DbmStatementWithLog(man, OlapMsgSysInfo::eOlapGroupDefault)
	{
		setExclusiveHostId(man.getHostId()); // PYR-27616
		prepareStmt();
	}
	void init(const char* propertyId_)
	{
		propertyId = propertyId_;

		initLog();
		composeLog(propertyId);
	}
	void parseOlapMessage(CommMsgParser& parser) override
	{
		propertyId.parse(parser);
	}
	UINT32 getMsgCode() const override
	{
		return DBM_Q_OLAP_DELETE_PROPERTY;
	}
};

class InsertPropertyStmt : public DbmStatementWithLog
{
	SQLINTEGER				propertyInt;
	SQLCHAR					propertyId[PROP_ID_LEN];
	SQLCHAR					propertyStr[PROP_STR_LEN];
	SQL_TIMESTAMP_STRUCT	propertyWhen;
	SQLLEN				cbId, cbInt, cbStr, cbWhen;
	void prepareStmt()
	{
		prepare("INSERT INTO PROPERTIES (PROPERTYID, PROPERTYINT, PROPERTYSTR, PROPERTYWHEN) VALUES (?, ?, ?, ?)");
		cbStr = cbId = SQL_NTS;
		cbWhen = cbInt = 0;
		bindStrParam( 1, PROP_ID_LEN, propertyId, &cbId );
		bindIntParam( 2, &propertyInt, &cbInt );
		bindStrParam( 3, PROP_STR_LEN, propertyStr, &cbStr );
		bindTimeParam( 4, &propertyWhen, &cbWhen );
	}
private:
	DbmStatementWithLog::executeLog;
public:
	InsertPropertyStmt(DatabaseManagerCommonWithOlap& man) : DbmStatementWithLog(man, OlapMsgSysInfo::eOlapGroupDefault) 
	{
		setExclusiveHostId(man.getHostId()); // PYR-27616
		prepareStmt();
	}
	void setPropertyId( const char* id )
	{
		strCopy((char*)propertyId, sizeof(propertyId), id );
	}
	void setPropertyInt( int Int )
	{
		propertyInt = Int;
	}
	void setStrProperty( const char* str )
	{
		strCopy((char*)propertyStr, sizeof(propertyStr), str );
	}
	void setWhen(const SrvTime& when)
	{
		initTimeStruct(propertyWhen, when );
		cbWhen = 0;
	}
	void setWhenNull()
	{
		cbWhen = SQL_NULL_DATA;
	}
	void executeComposeLog()
	{
		BYTE setNull = (SQL_NULL_DATA == cbWhen) ? 1:0;
	
	    logMessage.msgId = DBM_Q_OLAP_INSERT_PROPERTY;
		logMessage.msgBody.clear();
		logMessage.msgBody
			.composeUINT32( propertyInt)
			.composeString((char*) propertyId)
			.composeString((char*) propertyStr)
			.composeBYTE(setNull)
			;
		if (!setNull)
		{
			SrvTime tmp;
			initSrvTime(tmp, &propertyWhen );
			tmp.compose(logMessage.msgBody);
		}
		executeLog();
	}

};

class UpdatePropertyStmt : public DbmStatementWithLog
{
	SQLINTEGER				propertyInt;
	SQLCHAR					propertyId[PROP_ID_LEN];
	SQLCHAR					propertyStr[PROP_STR_LEN];
	SQL_TIMESTAMP_STRUCT	propertyWhen;
	SQLLEN				cbId, cbInt, cbStr, cbWhen;
	void prepareStmt()
	{
		prepare("UPDATE PROPERTIES SET PROPERTYINT = ?, PROPERTYSTR = ?, PROPERTYWHEN = ? WHERE PROPERTYID = ?");
		cbStr = cbId = SQL_NTS;
		cbWhen = cbInt = 0;
		bindIntParam( 1, &propertyInt, &cbInt );
		bindStrParam( 2, PROP_STR_LEN, propertyStr, &cbStr );
		bindTimeParam( 3, &propertyWhen, &cbWhen );
		bindStrParam( 4, PROP_ID_LEN, propertyId, &cbId );
	}
private:
	DbmStatementWithLog::executeLog;
	DbmStatementWithLog::execLogUpdateDelete;
public:
	UpdatePropertyStmt(DatabaseManagerCommonWithOlap& man) : DbmStatementWithLog(man, OlapMsgSysInfo::eOlapGroupDefault) 
	{
		setExclusiveHostId(man.getHostId()); // PYR-27616
		prepareStmt();
	}
	void setPropertyId( const char* id )
	{
		strCopy((char*)propertyId, sizeof(propertyId), id );
	}
	void setStrProperty( const char* str )
	{
		strCopy((char*)propertyStr, sizeof(propertyStr), str );
	}
	
	void setPropertyInt ( int propInt )
	{
		propertyInt = propInt;
	}
	
	void setWhen(const SrvTime& when)
	{
		initTimeStruct(propertyWhen, when );
		cbWhen = 0;
	}
	
	void setWhenNull()
	{
		cbWhen = SQL_NULL_DATA;
	}
	bool execUpdateComposeLog()
	{
		BYTE setNull = (SQL_NULL_DATA == cbWhen) ? 1:0;
	
	    logMessage.msgId = DBM_Q_OLAP_UPDATE_PROPERTY;
		logMessage.msgBody.clear();
		logMessage.msgBody
			.composeUINT32( propertyInt)
			.composeString((char*) propertyId)
			.composeString((char*) propertyStr)
			.composeBYTE(setNull)
			;
		if (!setNull)
		{
			SrvTime tmp;
			initSrvTime(tmp, &propertyWhen );
			tmp.compose(logMessage.msgBody);
		}
		return execLogUpdateDelete();
	}
};

// PYR-96119
class GetCfgLicenseHostStmt : public DbmStatement
{
	PSqlInt licenseId;
	PSqlInt	hostId;

	void prepareStmt()
	{
		// #DBA_REVIEWED #PYR-96119 VictoriaV 2019.Main.04
		prepare("SELECT LICENSEID, HOSTID FROM CFG_LICENSEHOST");
		bindFirstCol(licenseId);
		bindNextCol(hostId);
	}

public:
	GetCfgLicenseHostStmt(DatabaseManagerCommon& man) : DbmStatement(man)
	{
		prepareStmt();
	}
	UINT32 getLicenseId() const { return licenseId.value; }
	UINT32 getHostId() const { return hostId.value; }
};

#endif // dbmstmtcommon_h_included
