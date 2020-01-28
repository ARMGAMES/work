#ifndef commgrdaesencrypted_h
#define commgrdaesencrypted_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"

#define _COMM_AES_GUARD_NAME "AES_ENCRYPTED"



class CommClientAesEncryptedGuard : public CommClientGuard
{
private:
	enum {_version=1};
	PString user;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

	CommSSLEncryptAES * encrypt;
	CommSSLDecryptAES * decrypt;


public:
	CommClientAesEncryptedGuard( const char * user_,PBlock &securityToken_Password, const CommMsgBody& extra_);
	CommClientAesEncryptedGuard( CommClientAuthCallback & callback_,PBlock &securityToken_Password);
	~CommClientAesEncryptedGuard();
public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_AES_GUARD_NAME );
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

class CommClientAesEncryptedGuardFactory : public CommClientGuardFactory
{
private:
	PString user;
	CommMsgBody extra;
	PBlock securityToken_Password;
	CommClientAuthCallback * callback;

public:
	CommClientAesEncryptedGuardFactory();
	void setAuth( const char* user_, const CommMsgBody* extra_ = 0 );
	void setAuth( CommClientAuthCallback & callback_);
	void setAuthSecurityToken( CommMsgParser& parser );
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

	void resetAuth();
	bool hasAuth() const;

public:
	/* new */ CommClientGuardAuth* auth() override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override;
	/* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* auth ) override;
};

//****************************************************************************

class CommServerAesEncryptedGuard : public CommServerGuard
{
private:
	enum {_version=1};

	PString user;
	PBlock encrypted;

	INT32 peerVersion;
	CommSSLEncryptAES * encrypt;
	CommSSLDecryptAES * decrypt;

public:
	CommServerAesEncryptedGuard();
	~CommServerAesEncryptedGuard(  );

public://overridden
	const char* currentName() const override
	{
		return _COMM_AES_GUARD_NAME;
	}

	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;
	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;

	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool checkUserAuth( CommMsgParser* parser ) const override;
	bool isSimple() const override
	{
		return false;
	}
public://static - for root
	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const BYTE * encryptionKey, int sizeEncryptionKey, const BYTE * encryptionKey2, int sizeEncryptionKey2 );
	static void composeSecurityToken( CommMsgBody& ret, const BYTE * encryptionKey, int sizeEncryptionKey);
};

class CommServerAesEncryptedGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
