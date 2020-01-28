#ifndef commgrdencryptedpwd_h
#define commgrdencryptedpwd_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"
#include "commgrdencryption.h"

#define _COMM_ENCRYPTEDPASSWORD_GUARD_NAME "PWDEX"
#define _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME "PWDSIDEX"


class CommClientEncryptedPwdGuard : public CommClientGuard
{
private:

	enum {_version=1};
	PString user;
	PString pwd;
	PString sid;
	CommMsgBody extra;
	CommClientAuthCallback * callback;
	bool useSid;
	CommClientGuardEncryption encr;

public:
	CommClientEncryptedPwdGuard( const char* user_, const char * pwd_, const char* sid_, const CommMsgBody& extra_,  bool useSid);
	CommClientEncryptedPwdGuard( CommClientAuthCallback & callback_,  bool useSid);
	~CommClientEncryptedPwdGuard();
public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME );
		msgBody.composeString( _COMM_ENCRYPTEDPASSWORD_GUARD_NAME );
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
	void encryptMsg(const  CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

};

class CommClientEncryptedPwdGuardFactory : public CommClientGuardFactory
{
private:
	bool _hasAuth;
	PString user;
	PString pwd;
	PString sid;
	CommClientAuthCallback * callback;
	CommMsgBody extra;


public:
	CommClientEncryptedPwdGuardFactory();
	void setAuth( CommClientAuthCallback & callback_);
	void setAuth( const char* user_, const char* pwd_, const char* sid_, const CommMsgBody* extra_ = 0 );
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

class CommServerEncryptedPwdGuard : public CommServerGuard
{

private:
	enum {_version=1};
	PString user;
	INT32 peerVersion;
	bool useSid;
	PString pwdReceived;
	PString sidReceived;
	CommMsgBody clientExtra;
	CommServerGuardEncryption encr;
	

public:
	CommServerEncryptedPwdGuard( bool useSid_ );
	~CommServerEncryptedPwdGuard(  );

public://overridden
	const char* currentName() const override
	{
		return useSid ? _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME: _COMM_ENCRYPTEDPASSWORD_GUARD_NAME;
	}

	const char* getPlainTextPassword_useWithExtremeCaution() const override
	{
		return pwdReceived;
	}

	bool isSidUsed() const
	{
		return sidReceived.length() != 0;
	}

	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;
	bool checkUserAuth( CommMsgParser* parser ) const override;
	bool isSimple() const override
	{
		return true;
	}
	void getPwdHash(PString &ret) const
	{
		CommSSLPasswordHash::generateHash(ret,pwdReceived);
	}
	void encryptMsg(const CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

public://static - for root

	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword, const char* sid, INT16 sidErrorCode = 0 );
};

class CommServerEncryptedPwdGuardFactory : public CommServerGuardFactory
{
private:

public:
	CommServerEncryptedPwdGuardFactory()
	{
	}

	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
