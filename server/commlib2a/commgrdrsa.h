#ifndef commgrdrsa_h
#define commgrdrsa_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"

#define USE_RSA_VERSION_2
#define _COMM_RSA_GUARD_NAME "RSA"

class _CommClientRsaGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	PString pwd;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	/* virtual */ ~_CommClientRsaGuardAuth() {}
};


class CommClientRsaGuard : public CommClientGuard
{
private:
#ifdef USE_RSA_VERSION_2
	enum {_version=2};
#else
	enum {_version=1};
#endif
	PString user;
	PString pwd;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

	bool firstConnection;

	PBlock securityToken_Random;
	CommSSLEncryptAES * encrypt;
	CommSSLDecryptAES * decrypt;


public:
	CommClientRsaGuard( const char* user_, const char * pwd_,  const CommMsgBody& extra_);
	CommClientRsaGuard( CommClientAuthCallback & callback_);
	CommClientRsaGuard( const char* user_, const PBlock & securityToken_Password_, const PBlock & securityToken_Random_, const CommMsgBody& extra_);
	CommClientRsaGuard( CommClientAuthCallback & callback_, const PBlock & securityToken_Password_, const PBlock & securityToken_Random_);
	~CommClientRsaGuard();
public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_RSA_GUARD_NAME );
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool isSimple() const override
	{
		return false;
	}
	static INT32 getVersion()
	{
		return _version;
	}
};

class CommClientRsaGuardFactory : public CommClientGuardFactory
{
private:
	bool _hasAuth;
	PString user;
	PString pwd;
	CommClientAuthCallback * callback;
	CommMsgBody extra;
	PBlock securityToken_Password;
	PBlock securityToken_Random;


public:
	CommClientRsaGuardFactory();
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

class CommServerRsaGuard : public CommServerGuard
{

private:
	enum {_version=2};

	PString user;
	INT32 peerVersion;

	bool firstConnection;
	bool connectionFailed;

	CommSSLDH		  * dh;
	CommSSLEncryptAES * encrypt;
	CommSSLDecryptAES * decrypt;

	PString _hashPassword;
	PBlock encryptedToken;

	CommMsgBody clientExtra;
	CommMsgBody dhParameters;

public:
	CommServerRsaGuard( const CommMsgBody & dhParameters_ );
	~CommServerRsaGuard(  );

public://overridden
	const char* currentName() const override
	{
		return _COMM_RSA_GUARD_NAME;
	}

	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool gotGuardUserInfo( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool checkUserAuth( CommMsgParser* parser ) const override;

	bool isSimple() const override
	{
		return false;
	}
	bool isFirstConnect() const
	{
		return firstConnection;
	}

public://static - for root

	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser,
		const BYTE * password, int passwordSize,const BYTE * randomToken, int randomTokenSize,
		const BYTE * password2, int passwordSize2,const BYTE * randomToken2, int randomTokenSize2);
	static void processUserInfo( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword  );
	static void composeSecurityToken( CommMsgBody& ret, const BYTE * password, int passwordSize,const BYTE * randomToken, int randomTokenSize);
};

class CommServerRsaGuardFactory : public CommServerGuardFactory
{
private:
	CommMsgBody dhParameters;
public:
	void setDHParameters(const char * fileName)
	{
		CommSSLDH * dh = new CommSSLDH();
		dh->readParameters(fileName);
		dh->composeParameters( dhParameters );
		delete dh;
	}

	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
