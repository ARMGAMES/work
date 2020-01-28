#ifndef commgrdrsasimple_h
#define commgrdrsasimple_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"

#define _COMM_RSA_SIMPLE_GUARD_NAME "RSASIMPLE"

class _CommClientRsaSimpleGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	PString pwd;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	/* virtual */ ~_CommClientRsaSimpleGuardAuth() {}
};


class CommClientRsaSimpleGuard : public CommClientGuard
{
private:
	enum {_version=1};
	PString user;
	PString pwd;
	CommMsgBody extra;
	CommClientAuthCallback * callback;
	bool firstConnection;
	PBlock securityToken_Random;

public:
	CommClientRsaSimpleGuard( const char* user_, const char * pwd_,  const CommMsgBody& extra_);
	CommClientRsaSimpleGuard( CommClientAuthCallback & callback_);
	CommClientRsaSimpleGuard( const char* user_, const PBlock & securityToken_Random_, const CommMsgBody& extra_);
	CommClientRsaSimpleGuard( CommClientAuthCallback & callback_,  const PBlock & securityToken_Random_);
	~CommClientRsaSimpleGuard();
public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_RSA_SIMPLE_GUARD_NAME );
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool isSimple() const override
	{
		return true;
	}
	static INT32 getVersion()
	{
		return _version;
	}
};

class CommClientRsaSimpleGuardFactory : public CommClientGuardFactory
{
private:
	bool _hasAuth;
	PString user;
	PString pwd;
	CommClientAuthCallback * callback;
	CommMsgBody extra;
	PBlock securityToken_Random;


public:
	CommClientRsaSimpleGuardFactory();
	void setAuth( CommClientAuthCallback & callback_);
	void setAuth( const char* user_, const char* pwd_, const CommMsgBody* extra_ = 0 );
	void setUser( const char* user_ )
	{
		PASSERT3(!callback);
		user = user_;
	}
	void setExtra( const CommMsgBody* extra_ )
	{
		PASSERT3(!callback);
		if( extra_ )
			extra.copyFrom( *extra_ );
		else
			extra.clear();
	}

	void setAuthSecurityToken( CommMsgParser& parser);
	void resetAuth();
	bool hasAuth() const;

public:
	/* new */ CommClientGuardAuth* auth() override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override;
	/* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* auth ) override;
};

//****************************************************************************

class CommServerRsaSimpleGuard : public CommServerGuard
{

private:
	enum {_version=1};

	PString user;
	INT32 peerVersion;

	bool firstConnection;

	PString pwdReceived;
	PBlock  securityToken;

	CommMsgBody clientExtra;

public:
	CommServerRsaSimpleGuard(  );
	~CommServerRsaSimpleGuard(  );

public://overridden
	const char* currentName() const override
	{
		return _COMM_RSA_SIMPLE_GUARD_NAME;
	}

	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool checkUserAuth( CommMsgParser* parser ) const override;

	bool isSimple() const override
	{
		return true;
	}
	bool isFirstConnect() const
	{
		return firstConnection;
	}

public://static - for root

	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser,
		const BYTE * randomToken, int randomTokenSize,
		const BYTE * randomToken2, int randomTokenSize2);
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword );
	static void composeSecurityToken( CommMsgBody& ret, const BYTE * randomToken, int randomTokenSize);
};

class CommServerRsaSimpleGuardFactory : public CommServerGuardFactory
{
private:
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
