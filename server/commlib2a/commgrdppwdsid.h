#ifndef commgrdppwdsid_h
#define commgrdppwdsid_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commgrdppwd.h"

#define _COMM_PPWDSID_GUARD_NAME "PPWDSID"

class _CommClientPlainTextPasswordSidGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	PString pwd;
	CommMsgBody extra;
	PString sid;
	CommClientAuthCallback * callback;

public:
	_CommClientPlainTextPasswordSidGuardAuth()
	{
		callback = 0;
	}

	/* virtual */ ~_CommClientPlainTextPasswordSidGuardAuth() {}
};


class CommClientPlainTextPasswordSidGuard : public CommClientGuard
{
private:
	PString user;
	PString pwd;
	PString sid;
	CommMsgBody extra;
	CommClientAuthCallback * callback;
	bool useSid;

public:
	CommClientPlainTextPasswordSidGuard( const char* user_, const char* pwd_, const char* sid_, const CommMsgBody& extra_, bool useSid);
	CommClientPlainTextPasswordSidGuard( CommClientAuthCallback & callback_, bool useSid_);

public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_PPWDSID_GUARD_NAME );
		msgBody.composeString( _COMM_PPWD_GUARD_NAME );//sic! - supports both
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
};





class CommClientPlainTextPasswordSidGuardFactory : public CommClientGuardFactory
{
private:
	bool _hasAuth;
	PString user;
	PString pwd;
	PString sid;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	CommClientPlainTextPasswordSidGuardFactory();
	CommClientPlainTextPasswordSidGuardFactory( const char* user_, const char* pwd_, const char* sid_ );
	CommClientPlainTextPasswordSidGuardFactory( CommClientAuthCallback & callback_);
	void setAuth( const char* user_, const char* pwd_, const char* sid_, const CommMsgBody* extra_ = 0 );
	void setAuth( CommClientAuthCallback & callback_);
	void setUser( const char* user_ )
	{
		PASSERT3(!callback);
		user = user_;
	}
	void setPasswd( const char* pwd_ )
	{
		PASSERT3(!callback);
		pwd  = pwd_;
	}
	void setExtra( const CommMsgBody* extra_ )
	{
		PASSERT3(!callback);

		if( extra_ )
			extra.copyFrom( *extra_ );
		else
			extra.clear();
	}

	void resetAuth();
	bool hasAuth() const;

public:
	/* new */ CommClientGuardAuth* auth() override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override;
	/* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* auth ) override;
};



//****************************************************************************

class CommServerPlainTextPasswordSidGuard : public CommServerGuard
{
public:
	enum { AuthStateNotYet = 0, AuthStateOk = 1, AuthStatePwdFailed = 101, AuthStatePwdOkNoSid = 102, AuthStatePwdOkSidFailed = 103, AuthStatePwdOkNoSidInDatabase = 104 };

private:
	PString pwdReceived;
	PString sidReceived;
	PString user;
	CommMsgBody clientExtra;
	bool emulatePPWD;
	int authState;

public:
	CommServerPlainTextPasswordSidGuard( bool emulatePPWD_ );

public://overridden
	const char* currentName() const override
	{
		return emulatePPWD ? _COMM_PPWD_GUARD_NAME : _COMM_PPWDSID_GUARD_NAME;
	}
	bool isSidUsed() const
	{
		return sidReceived.length() != 0;
	}
	void getPwdHash(PString &ret) const
	{
		CommSSLPasswordHash::generateHash(ret,pwdReceived);
	}

	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;
	bool checkUserAuth( CommMsgParser* parser ) const override;
	int getAuthState() const { return authState; }

	const char* getPlainTextPassword_useWithExtremeCaution() const override
	{
		return pwdReceived;
	}

public://static - for root
	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user,  const char* encrPassword, const char* sid, INT16 sidErrorCode = 0);
};

class CommServerPlainTextPasswordSidGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
