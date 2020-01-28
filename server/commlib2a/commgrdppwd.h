#ifndef commgrdppwd_h
#define commgrdppwd_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"

#define _COMM_PPWD_GUARD_NAME "PPWD"

class CommClientPlainTextPasswordGuard : public CommClientGuard
{
private:
	PString user;
	PString pwd;
	CommMsgBody extra;

public:
	CommClientPlainTextPasswordGuard( const char* user_, const char* pwd_, const CommMsgBody& extra_ );

public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_PPWD_GUARD_NAME );
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
};

class CommClientPlainTextPasswordGuardFactory : public CommClientGuardFactory
{
private:
	bool _hasAuth;
	PString user;
	PString pwd;
	CommMsgBody extra;

public:
	CommClientPlainTextPasswordGuardFactory();
	CommClientPlainTextPasswordGuardFactory( const char* user_, const char* pwd_ );
	void setAuth( const char* user_, const char* pwd_, const CommMsgBody* extra_ = 0 );
	void setUser( const char* user_ )
	{
		user = user_;
	}

	void resetAuth();
	bool hasAuth() const;

public:
	/* new */ CommClientGuardAuth* auth() override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override;
	/* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* auth ) override;
};

//****************************************************************************

class CommServerPlainTextPasswordGuard : public CommServerGuard
{
private:
	PString pwdReceived;
	PString user;
	CommMsgBody clientExtra;

public:
	CommServerPlainTextPasswordGuard();

public://overridden
	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;

	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool checkUserAuth( CommMsgParser* parser ) const override;

	void getPwdHash(PString &ret) const
	{
		CommSSLPasswordHash::generateHash(ret,pwdReceived);
	}

public://static - for root
	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword);
};

class CommServerPlainTextPasswordGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
