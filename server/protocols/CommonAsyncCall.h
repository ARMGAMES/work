#ifndef CommonAsyncCall_h_included
#define CommonAsyncCall_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//lint -save -e1942

#include "commlib.h"

////////////////////////////////////////////////////////////////////////////////
// Utility and helper functions
////////////////////////////////////////////////////////////////////////////////

// Prepends a request Id to a message body for forwarding it as a request

inline void insertReqId(CommMsgBody& body, UINT32 reqId)
{
	if (reqId)
		CommMsgBody::writeUINT32(body._createPrefix(sizeof (UINT32)), reqId);
}

////////////////////////////////////////////////////////////////////////////////

// Creates and posts a reply message with a given error code and description.
// A correct ReqId needs to be given for a G connection, zero otherwise.

inline void postResultReply(CommServerConnection* pConn, UINT32 reqId, UINT32 replyMsgId,
							INT16 errCode = 0, char const* errDesc = "")
{
	if (pConn)
	{
		CommMsgBody reply;

		if (reqId)
			reply.composeUINT32(reqId);

		reply.composeINT16(errCode);

		if (errCode)
			reply.composeString(errDesc);

		pConn->post(replyMsgId, reply);
	}
}

////////////////////////////////////////////////////////////////////////////////

// Parses the error-code and checks if it is non-zero.
// In case of an error, parses the error description and logs it with a message.
// Assumes that the parser is initialized to the correct position.
// outErr is set to the actual error code.

inline bool checkForError(CommMsgParser& parser, char const* errMessage, INT16& outErr)
{
	// Check if the reply message contains a non-zero error code
	// If it does, log the error and return true, otherwise return false
	INT16 errCode;
	parser.parseINT16(errCode);

	outErr = errCode;

	if (errCode)
	{
		char const* errDescr;
		parser.parseString(errDescr);
		PLog("%s: %s", errMessage, errDescr);
		return true;
	}

	return false;
}


// Same as above in case the return error is not needed.
// The error message may be omitted.

inline bool checkForError(CommMsgParser& parser, char const* errMessage = "Error")
{
	INT16 outErr;
	return checkForError(parser, errMessage, outErr);
}

////////////////////////////////////////////////////////////////////////////////

// For forwarding messages that came from a G-connection

inline UINT32 forwardMessage(UINT32 msgId, CommMsgBody const& body,
							 CommClientGConnection& connection,
							 CommClientGConnection::AsyncCall* pCall = 0)
{
	// Make a copy of the message, skipping the request id, and send it to the connection
	CommMsgBody duplicate;
	duplicate.copyFromSkipReqId(body);
	return connection.post(msgId, duplicate, pCall);
}


template<typename T>
inline UINT32 forwardMessage(UINT32 msgId, CommMsgBody const& body,
							 CommClientGConnection& connection, PAutoPtr<T>& pCall)
{
	return forwardMessage(msgId, body, connection, pCall.extract());
}


// For forwarding messages that came from a regular (non-G) connection

inline UINT32 forwardClientMessage(UINT32 msgId, CommMsgBody const& body,
								   CommClientGConnection& connection,
								   CommClientGConnection::AsyncCall* pCall = 0)
{
	// Make a copy of the message and send it to the connection
	CommMsgBody duplicate;
	duplicate.copyFrom(body);
	return connection.post(msgId, duplicate, pCall);
}


template<typename T>
inline UINT32 forwardClientMessage(UINT32 msgId, CommMsgBody const& body,
								   CommClientGConnection& connection, PAutoPtr<T>& pCall)
{
	return forwardClientMessage(msgId, body, connection, pCall.extract());
}

////////////////////////////////////////////////////////////////////////////////

// For posting a message using a smart pointer AsyncCall
// The AsyncCall ownership is transferred from the smart pointer to the framework.

template<typename T>
inline UINT32 postMessage(UINT32 msgId, CommMsgBody& body, CommClientGConnection& connection,
						  PAutoPtr<T>& pCall)
{
	return connection.post(msgId, body, pCall.extract());
}

////////////////////////////////////////////////////////////////////////////////
// Common AsyncCall classes
////////////////////////////////////////////////////////////////////////////////

// An abstract base class for the CommonAsyncCall classes.
// Stores the connection id of the incoming message and a pointer to the server object.
//
// ServerObject must publicly derive from CommServerObject and must implement a
// CommServerConnection* findConnection(UINT32) method,
// usually implemented as: { return srvPool.findConnection(connId); }

template<typename ServerObject>
class BaseAsyncCallT: public CommClientGConnection::AsyncCall
{
protected: // Disable instantiation of base class
	BaseAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn) :
		 pObj(pObject), connId(pConn ? pConn->id() : 0)
	{}
	BaseAsyncCallT(ServerObject* pObject, UINT32 connId_) :
		pObj(pObject), connId(connId_)
	{}

	virtual ~BaseAsyncCallT() {}

private: // Disable copying and assignment
	BaseAsyncCallT(BaseAsyncCallT const&);
	BaseAsyncCallT& operator=(BaseAsyncCallT const&);

public:
	CommServerConnection* findConnection() const
	{
		CommServerConnection* pConnection = pObj->findConnection(connId);
		if (!pConnection)
			PLog("AsyncCall connection id %08X not found", connId);

		return pConnection;
	}

	ServerObject const* getObj() const { return pObj; }

	UINT32 getConnId() const { return connId; }

protected:
	ServerObject* pObj;
	UINT32 connId;
};

////////////////////////////////////////////////////////////////////////////////

// An AsyncCall for forwarding unparsed replies to another connection
// optionally substitutes the original reply ID to a given one

template<typename ServerObject>
class ForwardingAsyncCallT : public BaseAsyncCallT<ServerObject>
{
	typedef BaseAsyncCallT<ServerObject> Parent;
public:
	using Parent::findConnection;  // to resolve C3861 error when std conformance is enabled
	ForwardingAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn, UINT32 requestId,
						 UINT32 replyMsgId = 0) :
		BaseAsyncCallT<ServerObject>(pObject, pConn), reqId(requestId), replyId(replyMsgId)
	{}
	ForwardingAsyncCallT(ServerObject* pObject, UINT32 connId, UINT32 requestId,
						 UINT32 replyMsgId = 0) :
		BaseAsyncCallT<ServerObject>(pObject, connId), reqId(requestId), replyId(replyMsgId)
	{}

	void processReply(UINT32 requestId, UINT32 msgId, CommMsgBody const& body) override
	{
		forwardReplyToOrigin(body);
	}

	void forwardReplyToOrigin(CommMsgBody const& body) const
	{
		CommServerConnection* pConnection = findConnection();
		if (pConnection)
		{
			// Make a copy of the reply message
			CommMsgBody duplicate;
			duplicate.copyFrom(body);
			doReply(pConnection, duplicate);
		}
	}

	void postReply(CommMsgBody& body) const
	{
		CommServerConnection* pConnection = findConnection();
		if (pConnection)
			doReply(pConnection, body);
	}

	void postReply(INT16 errCode, char const* errDesc = "") const
	{
		CommServerConnection* pConnection = findConnection();
		if (pConnection)
			postResultReply(pConnection, reqId, replyId, errCode, errDesc);
	}

	UINT32 getReqId() const { return reqId; }
	UINT32 getReplyId() const { return replyId; }

protected:
	void doReply(CommServerConnection* pConnection, CommMsgBody& body) const
	{
		insertReqId(body, reqId);
		pConnection->post(replyId, body);
	}

protected:
	UINT32 reqId;
	UINT32 replyId;
};

////////////////////////////////////////////////////////////////////////////////

// An abstract class that serves as a base for AsyncCalls that handle responses.
// If HandleErrors is true, it will parse the response for errors and, if found,
// log them and forward the response to the originating connection.
// If the error code is zero, the rest of the response is passed to the handler.
// If HandleErrors is false, the unmodified response is passed to the handler.

template<typename ServerObject, bool HandleErrors>
class CheckedAsyncCallT : public ForwardingAsyncCallT<ServerObject>
{
	typedef ForwardingAsyncCallT<ServerObject> Parent; //lint !e1516 Member decl. hides inherited member
public:
	using Parent::forwardReplyToOrigin;  // to resolve C3861 error when std conformance is enabled
	CheckedAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn, UINT32 requestId,
					  UINT32 replyMsgId) :
		ForwardingAsyncCallT<ServerObject>(pObject, pConn, requestId, replyMsgId)
	{}
	CheckedAsyncCallT(ServerObject* pObject, UINT32 connId, UINT32 requestId,
					  UINT32 replyMsgId) :
		ForwardingAsyncCallT<ServerObject>(pObject, connId, requestId, replyMsgId)
	{}

	void processReply(UINT32 requestId, UINT32 msgId, CommMsgBody const& body) override
	{
		CommMsgParser parser(body);

		if (HandleErrors) // Expression is constant
		{
			// Abort processing on failures and return error to original caller
			INT16 errCode;
			parser.parseINT16(errCode);
			if (errCode)
			{
				char const* errDesc;
				parser.parseString(errDesc);
				processError(msgId, body, errCode, errDesc);
				return;
			}
		}

		invokeHandler(parser); // invoke handler
	}

protected:
	virtual void invokeHandler(CommMsgParser& parser) = 0;

	virtual void processError(UINT32 msgId, CommMsgBody const& body, INT16 errCode, char const* errDesc)
	{
		PLog("Message %u reply with error %d (%s)", msgId, (int)errCode, errDesc);
		forwardReplyToOrigin(body);
	}
};

////////////////////////////////////////////////////////////////////////////////

// A placeholder for the GenericAsyncCallT if no data needs to be saved
struct Void {};

////////////////////////////////////////////////////////////////////////////////

// An AsyncCall that handles the reply using a handler function and holds state data.
// Initialized with a member function of ServerObject that serves as a handler.

template<typename ServerObject, typename Data, bool HandleErrors = true>
class GenericAsyncCallT : public CheckedAsyncCallT<ServerObject, HandleErrors>
{
	typedef CheckedAsyncCallT<ServerObject, HandleErrors> Parent; //lint !e1516 Member decl. hides inherited member
protected:
	using Parent::pObj;  // to resolve C3861 error when std conformance is enabled
public:
	typedef void (ServerObject::*PHandler)(CommMsgParser&, GenericAsyncCallT*);

	GenericAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn, UINT32 requestId,
					  UINT32 replyMsgId, PHandler handler) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, pConn, requestId, replyMsgId),
		pHandler(handler)
	{}
	GenericAsyncCallT(ServerObject* pObject, UINT32 connId, UINT32 requestId,
					  UINT32 replyMsgId, PHandler handler) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, connId, requestId, replyMsgId),
			pHandler(handler)
	{}

	GenericAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn, UINT32 requestId,
					  UINT32 replyMsgId, PHandler handler, Data const& data_) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, pConn, requestId, replyMsgId), pHandler(handler), data(data_)
	{}
	GenericAsyncCallT(ServerObject* pObject, UINT32 connId, UINT32 requestId,
					  UINT32 replyMsgId, PHandler handler, Data const& data_) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, connId, requestId, replyMsgId), pHandler(handler), data(data_)
	{}

	GenericAsyncCallT& set(PHandler handler) { pHandler = handler; return *this; }

protected:
	void invokeHandler(CommMsgParser& parser) override { (pObj->*pHandler)(parser, this); }

protected:
	PHandler pHandler;

public:
	Data data; //lint !e1925
};


// Partial specialization of GenericAsyncCallT where the data is an owned pointer.
// The constructor accepts a pointer to the data and the destructor will delete it.

template<typename ServerObject, typename Data, bool HandleErrors>
class GenericAsyncCallT<ServerObject, Data*, HandleErrors> : public CheckedAsyncCallT<ServerObject, HandleErrors>
{
	typedef CheckedAsyncCallT<ServerObject, HandleErrors> Parent; //lint !e1516 Member decl. hides inherited member
protected:
	using Parent::pObj;  // to resolve C3861 error when std conformance is enabled
public:
	typedef void (ServerObject::*PHandler)(CommMsgParser&, GenericAsyncCallT*);

	//lint -sem(GenericAsyncCallT::GenericAsyncCallT, custodial(6))
	GenericAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn, UINT32 requestId,
					  UINT32 replyMsgId, PHandler handler, Data* pData = 0) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, pConn, requestId, replyMsgId), pHandler(handler), data(pData)
	{}

	GenericAsyncCallT(ServerObject* pObject, CommServerConnection const* pConn, UINT32 requestId,
					  UINT32 replyMsgId, PHandler handler, PAutoPtr<Data>& pData) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, pConn, requestId, replyMsgId), pHandler(handler), data(pData.extract())
	{}

	virtual ~GenericAsyncCallT() /*override*/ { delete data; }

	GenericAsyncCallT& set(PHandler handler) { pHandler = handler; return *this; }

protected:
	void invokeHandler(CommMsgParser& parser) override { (pObj->*pHandler)(parser, this); }

protected:
	PHandler pHandler;

public:
	Data* data; //lint !e1925
};

////////////////////////////////////////////////////////////////////////////////
// [AO 2010-06-23] 

// An AsyncCall similar to GenericAsyncCallT that does not post replies to the
// connection of the original incoming message.  Some methods are disabled.
//
// ServerObject must still implement a CommServerConnection* findConnection(UINT32)
// method (because of template funkiness), although it will not be called.

template<typename ServerObject, typename Data, bool HandleErrors = true>
class NoReplyAsyncCallT : public CheckedAsyncCallT<ServerObject, HandleErrors>
{
public:
	typedef void (ServerObject::*PHandler)(CommMsgParser&, NoReplyAsyncCallT*);

	NoReplyAsyncCallT(ServerObject* pObject, PHandler handler) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, (UINT32)0, 0, 0), pHandler(handler)
	{}

	NoReplyAsyncCallT(ServerObject* pObject, PHandler handler, Data const& data_) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, (UINT32)0, 0, 0), pHandler(handler), data(data_)
	{}

	NoReplyAsyncCallT& set(PHandler handler) { pHandler = handler; return *this; }

private: // Not supported
	typedef CheckedAsyncCallT<ServerObject, HandleErrors> Parent; //lint !e1516 Member decl. hides inherited member
	using Parent::findConnection;
	using Parent::forwardReplyToOrigin;
	using Parent::postReply;
	using Parent::getReqId;
	using Parent::getReplyId;
protected:
	using Parent::pObj;  // to resolve C3861 error when std conformance is enabled

protected:
	void processError(UINT32 msgId, CommMsgBody const& body, INT16 errCode, char const* errDesc) override
	{
		PLog("Message %u reply with error %d (%s), aborting.", msgId, errCode, errDesc);
	}

	void invokeHandler(CommMsgParser& parser) override { (pObj->*pHandler)(parser, this); }

protected:
	PHandler pHandler;

public:
	Data data; //lint !e1925
};


// Partial specialization of NoReplyAsyncCallT where the data is an owned pointer.
// The constructor accepts a pointer to the data and the destructor will delete it.

template<typename ServerObject, typename Data, bool HandleErrors>
class NoReplyAsyncCallT<ServerObject, Data*, HandleErrors> : public CheckedAsyncCallT<ServerObject, HandleErrors>
{
public:
	typedef void (ServerObject::*PHandler)(CommMsgParser&, NoReplyAsyncCallT*);

	NoReplyAsyncCallT(ServerObject* pObject, PHandler handler, Data* pData = 0) :
		CheckedAsyncCallT<ServerObject, HandleErrors>(pObject, (UINT32)0, 0, 0), data(pData), pHandler(handler)
	{}

	virtual ~NoReplyAsyncCallT() /*override*/ { delete data; }

	NoReplyAsyncCallT& set(PHandler handler) { pHandler = handler; return *this; }

private: // Not supported
	typedef CheckedAsyncCallT<ServerObject, HandleErrors> Parent; //lint !e1516 Member decl. hides inherited member
	using Parent::findConnection;
	using Parent::forwardReplyToOrigin;
	using Parent::postReply;
	using Parent::getReqId;
	using Parent::getReplyId;
protected:
	using Parent::pObj;  // to resolve C3861 error when std conformance is enabled

protected:
	void processError(UINT32 msgId, CommMsgBody const& body, INT16 errCode, char const* errDesc) override
	{
		PLog("Message %u reply with error %d (%s), aborting.", msgId, errCode, errDesc);
	}

	void invokeHandler(CommMsgParser& parser) override { (pObj->*pHandler)(parser, this); }

public:
	Data* data; //lint !e1925

protected:
	PHandler pHandler;
};

// Generic async call for parsing and logging errors only
// The main usage is for handling messages from/to servers that
// might be potentially located behind OlapSaver
class GenericErrorLoggingAsyncCall : public CommClientGConnection::AsyncCall
{
	PString errPrefix;
public:
	void processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		INT16 errCode;
		parser.parseINT16(errCode);
		if (errCode)
		{
			const char* errMsg;
			parser.parseString(errMsg);
			PLog("%s(msg=%u,rqs=%X): %d '%s'", errPrefix.c_str(), msgId, requestId, errCode, errMsg);
		}
	}
	GenericErrorLoggingAsyncCall(const char* _errPrefix = "ERROR") : errPrefix(_errPrefix) {}
};

////////////////////////////////////////////////////////////////////////////////

//lint -restore

#endif // CommonAsyncCall_h_included
