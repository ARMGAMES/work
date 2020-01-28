#ifndef commgrdpftppwd_h
#define commgrdpftppwd_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"

#define _COMM_PFTPPWD_GUARD_NAME "PFTPPWD"

class CommClientFtpPasswordGuard : public CommClientGuard
{
private:
	PString user;
	PString pwd;
	CommMsgBody extra;

public:
	CommClientFtpPasswordGuard( const char* user_, const char* pwd_, const CommMsgBody& extra_ );

public:
	void composeFirstRequest( CommMsgBody& msgBody ) override;
	bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) override;
	bool processGranted( CommMsgParser& parser ) override;
	void composeGuardNameList( CommMsgBody& msgBody ) const override
	{
		msgBody.composeString( _COMM_PFTPPWD_GUARD_NAME );
	}


	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;
};

class CommClientFtpPasswordGuardFactory : public CommClientGuardFactory
{
private:
	bool _hasAuth;
	PString user;
	PString pwd;
	CommMsgBody extra;

public:
	CommClientFtpPasswordGuardFactory();
	CommClientFtpPasswordGuardFactory( const char* user_, const char* pwd_ );
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

class CommServerFtpPasswordGuard : public CommServerGuard
{
private:
	PString pwdReceived;
	PString user;
	CommMsgBody clientExtra;

public:
	CommServerFtpPasswordGuard();

public://overridden
	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;

	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override;
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override;

	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override;//called only in response to conn->getUserAuth(), parser == 0 means 'not found'
	bool checkUserAuth( CommMsgParser* parser ) const override;

	static void encodePassword(const char * pwd, UINT32 salt, PString & pwdOut);
public://static - for root
	static const char* userAuthHandled();
	static void processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword, UINT32 salt);
//private:
	static bool _checkPassword(const char * pwd, const char * db_pwd, UINT32 salt);
	static void _crPassword(PString & outPwd,const char * pwd, UINT32 salt);
	static int  BinaryToX64(const unsigned char* input, int len, char* output);
};

class CommServerFtpPasswordGuardFactory : public CommServerGuardFactory
{
public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;
};

#endif
