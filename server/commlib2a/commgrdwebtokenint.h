#ifndef commgrdwebtokenint_h
#define commgrdwebtokenint_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commgrdencryption.h"

#define _COMM_WEBTOKENINT_GUARD_NAME "WEBTOKENINT"

class CommClientWebTokenIntGuard : public CommClientGuard
{
private:
	enum {_version=2};
	UINT32   userIntId;
	PString  token;
	CommMsgBody extra;
	CommClientAuthCallback * callback;
	CommClientGuardEncryption encr;

public:
	CommClientWebTokenIntGuard( UINT32 userIntId_, const char * token_,  const CommMsgBody& extra_ );
	CommClientWebTokenIntGuard( CommClientAuthCallback & callback_);

public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_WEBTOKENINT_GUARD_NAME );
	}
	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	static INT32 getVersion()
	{
		return _version;
	}
	void encryptMsg(const CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

};

class CommClientWebTokenIntGuardFactory : public CommClientGuardFactory
{
private:
	PString  token;
	UINT32   userIntId;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	CommClientWebTokenIntGuardFactory();
	void setAuth( UINT32 userIntId_, const char * token_, const CommMsgBody* extra_ = 0 );
	void setAuth(  CommClientAuthCallback & callback_);
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

class CommServerWebTokenIntGuard : public CommServerGuard
{
private:
	enum {_version=2};
	INT32 peerVersion;
	CommServerGuardEncryption encr;

public:
	CommServerWebTokenIntGuard();

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
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser );
};

class CommServerWebTokenIntGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
