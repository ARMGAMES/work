// protocols/baseconnection.h
#ifndef protocols_baseconnection_h_included
#define protocols_baseconnection_h_included

#include "commsrv.h"
#include "commgrdnull.h"
#include "commgrdppwd.h"
#include "ppinclude.h"
#include "tracemarkers.h"

template<typename ServerObject>
class BaseIncomingConnection : public CommServerConnection
{
	bool logConnId;
public:
	ServerObject* obj;
	const PString traceMarker;

	const char* getIpAddr_() const { return (connPathLen() ? connPathPoint(0) : ""); }
	BaseIncomingConnection(ServerObject* obj_, const char* traceMarker_)
		: CommServerConnection(), logConnId(false), obj(obj_), traceMarker(traceMarker_)
	{
		PLog("%sct th=%p", traceMarker.c_str(), this);
	}
	virtual const char* getUser() const { return ""; }
	virtual void processRequest(UINT32 msgId, const CommMsgBody& body) = 0;
	void setLogConnId( bool val ) { logConnId = val; }

	void processMessage(UINT32 msgId, const CommMsgBody& body) override
	{
		PString userAndConn;
		if (getUser() && *getUser())
		{
			userAndConn.assign(" '").append(getUser()).append("' ").appendHexInt(id());
		}
		else if( logConnId )
		{
			userAndConn.assign(" ").appendHexInt(id());
		}
		PLog("{%s %u%s", traceMarker.c_str(), msgId, userAndConn.c_str());
		processRequest(msgId, body); // virtual
		PLog("}");
	}
	void post( UINT32 msgId, CommMsgBody& body ) override
	{
		PString userAndConn;
		if (getUser() && *getUser())
		{
			userAndConn.assign(" '").append(getUser()).append("' ").appendHexInt(id());
		}
		else if( logConnId )
		{
			userAndConn.assign(" ").appendHexInt(id());
		}
		PLog(">%s %u%s", traceMarker.c_str(), msgId, userAndConn.c_str());
		CommServerConnection::post(msgId, body);
	}
	void connected() override 
	{
		PLog("%scn '%s' %X th=%p ip=%s", traceMarker.c_str(), getUser(), id(), this, getIpAddr_());
	}
	void closedOrDisconnected(int errCode, const char* errMsg) override
	{
		PString errStr;
		if (errCode || (errMsg && *errMsg))
		{
			errStr.append(' ').appendInt(errCode).append(' ').append(errMsg);
		}
		PLog("%sdc '%s' %X th=%p%s", traceMarker.c_str(), getUser(), id(), this, errStr.c_str());
	}

	void replyError(UINT32 replyMsgId, UINT32 reqId, INT16 errCode, const char* errDescr)
	{
		if (!replyMsgId)
			return;
		CommMsgBody reply;
		if (reqId)
		{
			reply.composeUINT32(reqId);
		}
		reply
			.composeINT16 (errCode)
			.composeString(errDescr);
		PLog("e=%d", errCode);
		PTRACE5("DEBUG errDescr=%s", errDescr);
		post(replyMsgId, reply);
	}
};

template<typename ServerObject>
class BaseIncomingConnectionFactory : public CommServerConnectionFactory
{
public:
	const PString traceMarker;
	ServerObject* obj;
	BaseIncomingConnectionFactory( CommServerGuardFactory& guardFactory, const char* traceMarker_ ) 
		: CommServerConnectionFactory( guardFactory ), traceMarker(traceMarker_ ), obj(NULL) {}
	void init(ServerObject* obj_)
	{
		obj = obj_;
	}
	virtual ~BaseIncomingConnectionFactory() {} //lint !e1540 (BaseIncomingConnectionFactory should not free obj)
};

class OutgoingGConnWithTrace : public CommClientGConnection
{
private:
	CommClientGConnection::post; // prohibited
protected:
	const PString traceMarker;
	const PString readableName; // PYR-49304
	bool actuallyConnected; // PYR-32341
public:
	OutgoingGConnWithTrace(CommClientGuardFactory& guardFactoryClient, const char* traceMarker_, const char* readableName_ = "") :
		CommClientGConnection(guardFactoryClient), traceMarker(traceMarker_), readableName(readableName_), actuallyConnected(false) {}
	void processGMessage(UINT32 reqId, UINT32 msgId, CommMsgBody const& body, AsyncCall* call) override
	{
		PLog("{%s %u %X", traceMarker.c_str(), msgId, reqId);
		if( call )
		{
			call->processReply(reqId, msgId, body);
		}
		else
		{
			// replies without dedicated async callback AND notifications
			processReply0(reqId, msgId, body);
		}
		PLog("}");
	}
	virtual void processReply0(UINT32 reqId, UINT32 msgId, const CommMsgBody& body)
	{
		PLog("unhandled msgId=%u", msgId);
	}
	UINT32 postX(UINT32 msgId, CommMsgBody& body, AsyncCall* call = 0)
	{
		if (!underlyingConnection().isConnected())
		{
			PLog("!conn(%s) '%s' %u", traceMarker.c_str(), readableName.c_str(), msgId);
			return 0;
		}
		UINT32 reqId = CommClientGConnection::post(msgId, body, call);
		PLog(">%s %u %X", traceMarker.c_str(), msgId, reqId);
		return reqId;
	}
	void connected() override
	{
		const CommClientConnection& conn_ = underlyingConnection();
		PLog("%scn G-connected to server(%s/%s/%s) id=%X th=%p ms=%zu rName='%s'",
			traceMarker.c_str(), conn_.getServer(), conn_.getServerObject(), conn_.getServerConnType(), conn_.id(), this, _size(), readableName.c_str());
		actuallyConnected = true;
	}
	bool closedOrDisconnected( int errCode, const char* errMsg ) override
	{
		actuallyConnected = false;
		const CommClientConnection& conn_ = underlyingConnection();
		PLog("%sdc G-disconnected from server(%s/%s/%s) id=%X th=%p ms=%zu rName='%s' %d '%s'",
			traceMarker.c_str(), conn_.getServer(), conn_.getServerObject(), conn_.getServerConnType(), conn_.id(), this, _size(), readableName.c_str(), errCode, errMsg);
		return CommClientGConnection::closedOrDisconnected(errCode, errMsg);
	}
	bool isActuallyConnected() const // PYR-32341
	{
		return underlyingConnection().isConnected() && actuallyConnected;
	}
	const char* getTrace() const { return traceMarker.c_str(); }
	const char* getReadableName() const { return readableName.c_str(); } // PYR-49304
};


template <typename ServerObject>
class BaseOutgoingGConnection : public OutgoingGConnWithTrace
{
protected:
	ServerObject* obj;
public:
	BaseOutgoingGConnection(CommClientGuardFactory& guardFactoryClient, const char* traceMarker_, const char* readableName_ = "") :
		OutgoingGConnWithTrace(guardFactoryClient, traceMarker_, readableName_), obj(NULL) {}
	void init( ServerObject* obj_ )
    {
        obj = obj_;
    }
};

// PYR-51017
template <typename ServerObject>
class BaseOutgoingGConnectionWithReadableName : public OutgoingGConnWithTrace
{
protected:
	ServerObject* obj;

	void insertToServerObjectGConnMaps()
	{
		PLog("insertToServerObjectGConnMaps: traceMarker='%s', readableName='%s'", traceMarker.c_str(), readableName.c_str());

		if (traceMarker.length() == 0)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": insertToServerObjectGConnMaps: traceMarker is empty readableName='%s'", readableName.c_str());
		}
		if (!(obj->traceToGConnMap.insert(make_pair(traceMarker, this)).second))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": insertToServerObjectGConnMaps: redundant traceMarker='%s'.   readableName='%s'"
				, traceMarker.c_str(), readableName.c_str());
		} 
		if (readableName.length() == 0)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": insertToServerObjectGConnMaps: readableName is empty traceMarker='%s'", traceMarker.c_str());
		}
		if (!(obj->outConnReadableNames.insert(readableName).second))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": insertToServerObjectGConnMaps: redundant readableName='%s'.   traceMarker='%s'"
				, readableName.c_str(), traceMarker.c_str());
		}
	}

public:
	BaseOutgoingGConnectionWithReadableName(CommClientGuardFactory& guardFactoryClient, const char* traceMarker_, const char* readableName_) :
		OutgoingGConnWithTrace(guardFactoryClient, traceMarker_, readableName_), obj(NULL) {}

    // PYR-52379
    BaseOutgoingGConnectionWithReadableName(CommClientGuardFactory& guardFactoryClient, const char* traceMarker_, const char* readableName_, ServerObject* obj_) :
        OutgoingGConnWithTrace(guardFactoryClient, traceMarker_, readableName_), obj(obj_) 
    {
        insertToServerObjectGConnMaps();
    }

	void init( ServerObject* obj_ )
	{
        if( !obj ) // protect against double initialization
        {
		    obj = obj_;
		    insertToServerObjectGConnMaps();
        }
	}

	bool beforeGConnectedPost(UINT32 msgId, CommMsgBody& body) override // PYR-76565
	{
		UINT32 requestId = 0;
		try
		{
			if (body._size() >= sizeof(requestId)) // should be always true
			{
				CommMsgParser parser(body);
				parser.parseUINT32(requestId);
			}
		}
		catch (const PError&) {} // Not only PParserError to be extra safe //-V565
		PLog(">g %u %X", msgId, requestId);
		return OutgoingGConnWithTrace::beforeGConnectedPost(msgId, body);
	}
};

class SrvObjectConnUtils
{
public:
	static void connectToServer(CommClientConnectionPool& cliPool, CommClientConnection& conn,
		const char* server, const char* serverObject, const char* serverConnType)
	{
		PLog("connectToServer(%s/%s/%s)", server, serverObject, serverConnType);
		if (!server || !*server || !serverObject || !*serverObject)
		{
			PLog("-incomplete server params");
			return;
		}
		if (conn.isConnected() &&
			(!PString::equals(server, conn.getServer()) || !PString::equals(serverObject, conn.getServerObject())) )
		{
			conn.closedOrDisconnected(0/*errCode*/, "");
			cliPool.disconnect(conn);
		}
		if (!conn.isConnected())
		{
			cliPool.connect(conn, server, serverObject, serverConnType);
		}
	}
};

#endif // protocols_baseconnection_h_included
