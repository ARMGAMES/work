#ifndef commgrdtoken_h
#define commgrdtoken_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commgrdencryption.h"

#define _COMM_TOKEN_GUARD_NAME "TOKEN"

class CommClientTokenGuard : public CommClientGuard
{
private:
	enum {_version=2};
	PString user;
	PBlock  token;
	CommMsgBody extra;
	CommClientAuthCallback * callback;
	CommClientGuardEncryption encr;

public:
	CommClientTokenGuard( const char* user_, const PBlock & token_,  const CommMsgBody& extra_ );
	CommClientTokenGuard(  CommClientAuthCallback & callback_, const PBlock & token_);

public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_TOKEN_GUARD_NAME );
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	static INT32 getVersion()
	{
		return _version;
	}
	void encryptMsg(const  CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

};

class CommClientTokenGuardFactory : public CommClientGuardFactory
{
private:
	PString user;
	PBlock  token;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	CommClientTokenGuardFactory();
	void setAuth( const char* user_, const CommMsgBody* extra_ = 0 );
	void setAuth( CommClientAuthCallback & callback_);
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

class CommServerTokenGuard : public CommServerGuard
{
private:
	enum {_version=2};
	PBlock tokenReceived;
	INT32 peerVersion;
	CommServerGuardEncryption encr;

public:
	CommServerTokenGuard();

public://overridden
	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool checkUserAuth( CommMsgParser* parser ) const override;
	void encryptMsg(const CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

public://static - for root
	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const BYTE * token, int tokenSize, const BYTE * token2, int tokenSize2 );
	static void composeSecurityToken( CommMsgBody& ret, const BYTE * token, int tokenSize);
};

class CommServerTokenGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
