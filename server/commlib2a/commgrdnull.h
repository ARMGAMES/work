#ifndef commgrdnull_h
#define commgrdnull_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"

#define _COMM_NULL_GUARD_NAME "NULL"

class CommClientNullGuard : public CommClientGuard
{
	CommMsgBody extra;
public:
	CommClientNullGuard(const CommMsgBody& extra_);

public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_NULL_GUARD_NAME );
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
};

class CommClientNullGuardFactory : public CommClientGuardFactory
{
	CommMsgBody extra;
public:
	void setAuth(const CommMsgBody & extra_);
	/* new */ CommClientGuardAuth* auth() override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override;
	/* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* ) override;
};

//****************************************************************************

class CommServerNullGuard : public CommServerGuard
{
public:
	CommServerNullGuard();

public://overridden
	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;
	void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;
};

class CommServerNullGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
