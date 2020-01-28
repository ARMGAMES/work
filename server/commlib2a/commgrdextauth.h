#ifndef commgrdextauth_h
#define commgrdextauth_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"
#include "commgrdencryption.h"

#define _COMM_EXTERN_AUTH_GUARD_NAME "EXTERNAUTH"
#define _COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME "EXTERNAUTH_OPERATOR_TOKEN"

class CommClientExternAuthGuard : public CommClientGuard
{
private:
    enum {_version = 1};
    PString user;
    PString pwd;
	CommClientAuthCallback* callback;
    CommMsgBody extra;
	CommClientGuardEncryption encr;

protected:
	CommClientExternAuthGuard(const char* guardName_, const char* user_, const char* pwd_, const CommMsgBody& extra_);
	CommClientExternAuthGuard(const char* guardName_, CommClientAuthCallback& callback_);

public:
	CommClientExternAuthGuard(const char* user_, const char* pwd_, const CommMsgBody& extra_) : CommClientGuard(_COMM_EXTERN_AUTH_GUARD_NAME), user(user_), pwd(pwd_), callback(nullptr)
	{
		extra.copyFrom(extra_);
	}

	CommClientExternAuthGuard(CommClientAuthCallback& callback_) : CommClientGuard(_COMM_EXTERN_AUTH_GUARD_NAME), callback(&callback_) {}

public:
    void composeFirstRequest( CommMsgBody& msgBody ) override;
    bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
    bool processGranted( CommMsgParser& parser ) override;
    void composeGuardNameList( CommMsgBody& msgBody ) const override;
    void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
    void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
	void encryptMsg(const  CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

    static INT32 getVersion()
    {
        return _version;
    }
};

class CommClientExternAuthOperatorTokenGuard : public CommClientExternAuthGuard
{
public:
	CommClientExternAuthOperatorTokenGuard(const char* user_, const char* token_, const CommMsgBody& extra_) : CommClientExternAuthGuard(_COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME, user_, token_, extra_) {}
	CommClientExternAuthOperatorTokenGuard(CommClientAuthCallback& callback_) : CommClientExternAuthGuard(_COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME, callback_) {}

public:
	void composeGuardNameList(CommMsgBody& msgBody) const override
	{
		msgBody.composeString(_COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME);
	}

	static INT32 getVersion()
	{
		return CommClientExternAuthGuard::getVersion();
	}
};

class CommClientExternAuthGuardFactory : public CommClientGuardFactory
{
	virtual const char* whoAmI() const { return "CommClientExternAuthGuardFactory"; }

protected:
    bool _hasAuth;
    PString user;
    PString pwd;
	CommClientAuthCallback* callback;
    CommMsgBody extra;

public:
	CommClientExternAuthGuardFactory();
	CommClientExternAuthGuardFactory( const char* user_, const char* pwd_ );

	void setAuth( CommClientAuthCallback& callback_ );
    void setAuth( const char* user_, const char* pwd_, const CommMsgBody* extra_ = 0 );
    void setUser( const char* user_ )
    {
		PASSERT3( !callback );
        user = user_;
    }
	void setExtra(const CommMsgBody* extra_)
	{
		PASSERT3(!callback);
		if (extra_)
			extra.copyFrom(*extra_);
		else
			extra.clear();
	}

    void resetAuth();
    bool hasAuth() const {return _hasAuth;}

public:
    /* new */ CommClientGuardAuth* auth() override;
    void composeGuardNameList( CommMsgBody& msgBody ) const override;
    /* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* auth_ ) override;
};

class CommClientExternAuthOperatorTokenGuardFactory : public CommClientExternAuthGuardFactory
{
	const char* whoAmI() const override { return "CommClientExternAuthOperatorTokenGuardFactory"; }

public:
	CommClientExternAuthOperatorTokenGuardFactory() : CommClientExternAuthGuardFactory() {}
	CommClientExternAuthOperatorTokenGuardFactory(const char* user_, const char* token_) : CommClientExternAuthGuardFactory(user_, token_) {}

public:
	/* new */ CommClientGuardAuth * auth() override;
	void composeGuardNameList(CommMsgBody& msgBody) const override;
	/* new */ CommClientGuard* createGuard(const char* guardName, CommClientGuardAuth* auth_) override;
};

class CommServerExternAuthGuard : public CommServerGuard
{
private:
    INT32 peerVersion;
    PString pwdReceived;
    PString user;
    CommMsgBody clientExtra;
	CommServerGuardEncryption encr;

	virtual const char* whoAmI() const { return "CommServerExternAuthGuard"; }

protected:
	enum { _version = 1 };

	CommServerExternAuthGuard(const char* guardName) : CommServerGuard(guardName), peerVersion(0) {}

public:
    CommServerExternAuthGuard() : CommServerGuard(_COMM_EXTERN_AUTH_GUARD_NAME), peerVersion(0) {}

    const PString& getPwd() const {return pwdReceived;}

public://overridden
    void processFirstRequest( CommMsgParser& parser ) override;
    void processAddRequest( CommMsgParser& parser ) override;
    void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
    void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
    bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override; //called only in response to conn->getUserAuth(), parser == 0 means 'not found'
    void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override; //called only in response to conn->getUserAuth(), parser == 0 means 'not found'
    bool checkUserAuth( CommMsgParser* parser ) const override;
	void encryptMsg(const CommMsgBody& src, PBlock& dst) const override;
	void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const override;
	bool isEncryptionInit() const override
	{
		return encr.isInit();
	}

public://static - for root
    static const char* userAuthHandled();
    static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser);
};

class CommServerExternAuthOperatorTokenGuard : public CommServerExternAuthGuard
{
	const char* whoAmI() const override { return "CommServerExternAuthOperatorTokenGuard"; }

public:
	CommServerExternAuthOperatorTokenGuard() : CommServerExternAuthGuard(_COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME) {}

#if !defined( _MSC_VER ) || _MSC_VER >= 1910 // VS 2017
	const PString& getPwd() const = delete;
#endif

	const PString& getToken() const { return CommServerExternAuthGuard::getPwd(); }
	
public://static - for root
	static const char* userAuthHandled();
	static void processUserAuth(CommMsgBody& ret, CommMsgParser& parser);
};

class CommServerExternAuthGuardFactory : public CommServerGuardFactory
{
public:
    /* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

class CommServerExternAuthOperatorTokenGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard * createGuard(const vector< const char* >& guardNames) override;
};

#endif // commgrdextauth_h
