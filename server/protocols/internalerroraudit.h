// internalerroraudit.h

#ifndef internalerroraudit_h_defined
#define internalerroraudit_h_defined

#include	"pplib.h"
#include	"commcli.h"
#include	"connutils.h"

#define INTERNAL_ERROR_AUDIT_SECTION "DYNAMICCONNECT"

//////////////////////////////////////////////////////////////////////////
class AuditServerConnector
{
	virtual CommClientGConnection& getConn() const = 0;

public:

	const UINT32 msgId;
	const UINT32 moduleId;

	AuditServerConnector( UINT32 msgId_, const UINT32 moduleId_ ) : msgId(msgId_), moduleId(moduleId_)
	{
	}
	void sendAuditMessage( CommMsgBody& body, CommClientGConnection::AsyncCall* call = NULL ) const
	{
		UINT32 reqId = getConn().post( msgId, body, call );
		PLog(">%s %u %X msgId=%d", "TC", msgId, reqId, msgId);
	}
	void sendAuditMessage( CommMsgBody& body, UINT32 msgIdOverride, CommClientGConnection::AsyncCall* call ) const
	{
		UINT32 reqId = getConn().post( msgIdOverride, body, call );
		PLog(">%s %u %X msgId=%d", "TC", msgId, reqId, msgIdOverride);
	}
	bool isConnected() const
	{
		return getConn().underlyingConnection().isConnected();
	}
	virtual ~AuditServerConnector()
	{
	}
};
//////////////////////////////////////////////////////////////////////////
class AuditServerConnectorUsingConn : public AuditServerConnector
{
	CommClientGConnection& conn;
	CommClientGConnection& getConn() const override { return conn; }

public:
	AuditServerConnectorUsingConn( CommClientGConnection& conn_, UINT32 msgId_, const UINT32 moduleId_ ) 
		: AuditServerConnector( msgId_, moduleId_ ), conn(conn_)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
class AuditServerConnection : public CommClientGConnection
{
public:
	AuditServerConnection(CommClientGuardFactory& guardFactoryClient) 
		: CommClientGConnection(guardFactoryClient)
	{
	}
	void connected() override
	{
		PLog("TCcn AuditServerConnection connected");
		CommClientGConnection::connected();
	}
	bool closedOrDisconnected(int errCode, const char* errMsg) override
	{
		PLog("TCdc AuditServerConnection disconnected");
		return CommClientGConnection::closedOrDisconnected(errCode, errMsg);
	}
	void processGMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* pCall) override
	{
		PLog("{%s %u %X", "TC", msgId, requestId);
		if( pCall )
		{
			pCall->processReply(requestId, msgId, body);
		}
		PLog("}");
	}
};

//////////////////////////////////////////////////////////////////////////
class AuditServerConnectorOwnConn : public AuditServerConnector
{
	mutable AuditServerConnection conn;
	CommClientConnectionPool& cliPool;
	
public:

	AuditServerConnection& getConn() const override { return conn; }
	
	AuditServerConnectorOwnConn( CommClientGuardFactory& guardFactoryClient, CommClientConnectionPool& cliPool_, UINT32 msgId_, const UINT32 moduleId_ ) 
		: AuditServerConnector( msgId_, moduleId_ ), conn(guardFactoryClient), cliPool(cliPool_)
	{
	}
	void connect( const PIniFile& ini, const char* sectionName )
	{
		ConnUtils::createClientConnection(ini, sectionName, cliPool, conn, 
			"internalErrAuditServer", "internalErrAuditObject", "internalErrAuditType");
	}
	~AuditServerConnectorOwnConn()
	{
		if( conn.underlyingConnection().isConnected())
		{
			cliPool.disconnect(conn.underlyingConnection());
		}
	}
};
//////////////////////////////////////////////////////////////////////////

class IFormatErrorAction
{
public:
	virtual void doAction(UINT32 localeId,  INT16 commonErrorCode, UINT32 errorLocationMark) const = 0;
	virtual void doAction(UINT32 localeId,  INT16 commonErrorCode, UINT32 errorLocationMark, UINT32 errorNamespace) const = 0;
	virtual ~IFormatErrorAction(){};
};
//////////////////////////////////////////////////////////////////////////
// PYR-25641
class NotifyAuditServerFormatActionQuietCall : public CommClientGConnection::AsyncCall
{
public:

	void processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body ) override
	{
		PLog("{NASFAC: %X %u", requestId, msgId);
		INT16 errCode;
		CommMsgParser parser( body );
		parser.parseINT16( errCode );
		if( errCode )
		{
			const char* errDescr;
			parser.parseString( errDescr );
			PLog( "NotifyAuditServerFormatActionCall: error %d '%s'", errCode, errDescr );
		}
		PLog("}");
	}
};
//////////////////////////////////////////////////////////////////////////

class NotifyAuditServerFormatAction : public IFormatErrorAction
{
	const AuditServerConnector& auditServerConnector;
	const PString userId;
	const bool useQuietCall; // PYR-25641

public:

	NotifyAuditServerFormatAction( const AuditServerConnector& c, const char* userId_, bool useQuietCall_ = false ) 
		: auditServerConnector(c), userId(userId_), useQuietCall(useQuietCall_)
	{
	}
	void doAction(UINT32 localeId,  INT16 commonErrorCode, UINT32 errLocationMark) const override
	{
		if( !auditServerConnector.isConnected() )
		{
			return;
		}
		SrvTime when;
		when.currentLocalTime();

		CommMsgBody body;
		body.composeString(userId)
			.composeUINT32(localeId)
			.composeINT16(commonErrorCode)
			.composeUINT32(errLocationMark)
			.composeUINT32(auditServerConnector.moduleId)
			.composeSrvTime(when)
			.composeUINT32(0/*errNamespace*/) // PYR-35048
			;
		// PYR-25641
		CommClientGConnection::AsyncCall* call = NULL;
		if( useQuietCall )
		{
			call = new NotifyAuditServerFormatActionQuietCall();
		}
		auditServerConnector.sendAuditMessage(body, call);
	}
	// PYR-35048
	void doAction(UINT32 localeId,  INT16 commonErrorCode, UINT32 errLocationMark, UINT32 errNamespace) const override
	{
		if( !auditServerConnector.isConnected() )
		{
			return;
		}
		SrvTime when;
		when.currentLocalTime();

		CommMsgBody body;
		body.composeString(userId)
			.composeUINT32(localeId)
			.composeINT16(commonErrorCode)
			.composeUINT32(errLocationMark)
			.composeUINT32(auditServerConnector.moduleId)
			.composeSrvTime(when)
			.composeUINT32(errNamespace)
			;
		// PYR-25641
		CommClientGConnection::AsyncCall* call = NULL;
		if( useQuietCall )
		{
			call = new NotifyAuditServerFormatActionQuietCall();
		}
		auditServerConnector.sendAuditMessage(body, call);
	}
};

//////////////////////////////////////////////////////////////////////////
class TranslateServerFormatAction : public IFormatErrorAction
{
	const AuditServerConnector& auditServerConnector;
	const PString userId;
	CommClientGConnection::AsyncCall* call; // weak reference
	UINT32 msgId;

public:

	TranslateServerFormatAction( const AuditServerConnector& c, const char* userId_, UINT32 msgId_, CommClientGConnection::AsyncCall* call_ ) 
		: auditServerConnector(c), userId(userId_), call(call_), msgId(msgId_)
	{
	}
	void doAction(UINT32 localeId,  INT16 commonErrorCode, UINT32 errLocationMark) const override
	{
		if( !auditServerConnector.isConnected() )
		{
			return;
		}
		SrvTime when;
		when.currentLocalTime();

		CommMsgBody body;
		body.composeString(userId)
			.composeUINT32(localeId)
			.composeINT16(commonErrorCode)
			.composeUINT32(errLocationMark)
			.composeUINT32(auditServerConnector.moduleId)
			.composeSrvTime(when)
			.composeUINT32(0/*errNamespace*/) // PYR-35048
			;
		auditServerConnector.sendAuditMessage(body, msgId, call);
	}
	// PYR-35048
	void doAction(UINT32 localeId,  INT16 commonErrorCode, UINT32 errLocationMark, UINT32 errNamespace) const override
	{
		if( !auditServerConnector.isConnected() )
		{
			return;
		}
		SrvTime when;
		when.currentLocalTime();

		CommMsgBody body;
		body.composeString(userId)
			.composeUINT32(localeId)
			.composeINT16(commonErrorCode)
			.composeUINT32(errLocationMark)
			.composeUINT32(auditServerConnector.moduleId)
			.composeSrvTime(when)
			.composeUINT32(errNamespace)
			;
		auditServerConnector.sendAuditMessage(body, msgId, call);
	}
};

//////////////////////////////////////////////////////////////////////////
enum AuditModuleIds
{
	eAuditModuleIdUnknown	= 0,
	eAuditModuleIdLobby		= 1,
	eAuditModuleIdMtLobby	= 2,
	eAuditModuleIdCashier	= 3,
	eAuditModuleIdRegulator	= 4,
	eAuditModuleIdBlitzLobby= 5,
	eAuditModuleIdLobbyEtc	= 6,

	eAuditModuleId_Last
};

#endif // internalerroraudit_h_defined

