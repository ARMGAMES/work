#ifndef dbmstmtolap_h_included
#define	dbmstmtolap_h_included

#include "dbmstmt.h"
#include "OlapCommon.h"
#include "logmessage.h"


class DbmStatementWithLog : public DbmStatement
{
private:
	bool				replicateOnly;
protected:
	DatabaseManagerCommonWithOlap&	logMan;
	LogMessage			logMessage;
	bool				logActive;		// send data to OLAP
	DbmStatement::execDirect;
	DbmStatement::execDirectUpdateDelete;
	DbmStatement::execute;
	DbmStatement::executeRet;
	DbmStatement::execUpdateDelete;
	DbmStatement::execConditionalInsert;
	void replicateToOlap();
	void clearLogMessage();
	void initLog();
	virtual void parseOlapMessage(CommMsgParser& parser);
	virtual UINT32 getMsgCode() const; 
	UINT32 setOlapLicenseMaskByUserIntId(UINT32 userIntId);	// PYR-25464
	void setOlapLicenseMaskFromLicenseId(UINT32 licenseId_);

public:
	// PYR-91675 - execution mode of the statement descendant from olap statement base class (DbmStatementWithLog)
	enum eExecutionMode
	{
		eExecuteAndReplicate	= 0, // logActive=true,replicateOnly=false (statement is executed and replicated to OLAP)
		eExecuteOnly			= 1, // logActive=false,replicateOnly=false (statement is executed and NOT replicated to OLAP)
		eReplicateOnly			= 2, // logActive=true,replicateOnly=true (statement is not executed but replicated to OLAP)
		// logActive=false and replicateOnly=true - is invalid combination
	};
	DbmStatementWithLog( DatabaseManagerCommonWithOlap& manager_, OlapMsgSysInfo::eOlapDataGroup dataGroup, const eExecutionMode mode /*= eExecuteAndReplicate*/ /*PYR-91675*/);
	// !!!DO NOT USE!!! - obsolete. MG TODO: remove after no longer being used anywhere in the dbm code.
	DbmStatementWithLog( DatabaseManagerCommonWithOlap& manager_, OlapMsgSysInfo::eOlapDataGroup dataGroup, const bool logActive_ = true );
	virtual ~DbmStatementWithLog() {}

	void		executeLog();
	void		setLogActive(const bool logActive_) {logActive = logActive_;}
	void		setReplicateOnly(const bool replicateOnly_) {replicateOnly = replicateOnly_;}
	bool		isReplicateOnly() const { return replicateOnly; }
	bool		execLogUpdateDelete();
	bool		execLogConditionalInsert(); // to be used only with LZ or AR approval
	bool		execLogInsertIgnoreDuplicate(); // PYR-37710 to be used only with LZ or AR approval

	/* Obsolete by #22086
	void initFromOlapMessage(const CommMsgBody& olapMsgBody);
	*/
	void initFromOlapMessage(
		CommMsgParser& olapMsgParser 		// OLAP message parser processed by OlapMsgHandler::extractMsgData()
		);
	//
	// Support for SqlValue-derived classes
	//
	void composeLog(const PSqlValue& value);

	void setExclusiveHostId(UINT32 hostId)
	{
		logMessage.sysInfo.setExclusiveHostId(hostId);
	}

	void setIndexWithinGroup(UINT32 idx)		//	PYR-29174
	{
		logMessage.sysInfo.setIndexWithinGroup(idx);
	}

	void clearOlapMsgDynamicData()		//	PYR-29174
	{
		logMessage.sysInfo.clearDynamicData();
	}
};

#endif // dbmstmtolap_h_included
