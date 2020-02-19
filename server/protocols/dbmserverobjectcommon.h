// ===========================================================
#pragma

#include "commgrdnull.h"
#include "dbcommon.h"

class CommMonitorLoggerObject;


class DbmServerObjectCommon : public CommServerObject
{
private:
	typedef CommServerObject Parent;

	DatabaseManagerCommon* dbManagerCommon;

protected:
	DbmServerObjectCommon( _CommInterface& inter_ )
		: CommServerObject( inter_ )
		, dbManagerCommon( 0 )
	{
	}

	virtual ~DbmServerObjectCommon();

	// Must be called once from static init of server object after dbManager initialization
	void initDBMServerObject(
			DatabaseManagerCommon* dbManagerCommon_
			, const PIniFile& iniFile
			);

	virtual void reportToHeLogger( const CommMonitorLoggerObject& heLogger, CommMsgBody& reportBody );

public:
	virtual void dynamicInit( const PIniFile& iniFile );
	virtual bool registerAllFactories();
};

