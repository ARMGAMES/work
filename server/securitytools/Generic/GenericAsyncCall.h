////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericAsyncCall_h_included
#define GenericAsyncCall_h_included

// Heavily plagiarized from AlexO's CommonAsyncCall.h
// Adapted to fit the need of parallel async calls
// i.e. Server makes more than one call out, 
// and a single CommClientGConnection::AsyncCall will receive all the replies.
// That AsyncCall should know the replyId so it knows how to parse the
// call-specific response.

////////////////////////////////////////////////////////////////////////////////

#include "commcli.h"

////////////////////////////////////////////////////////////////////////////////

// A placeholder for Async Call data if no data needs to be saved
struct Void {};

////////////////////////////////////////////////////////////////////////////////

template<typename ServerObject, typename Data>
struct NoReplyAsyncCall : public CommClientGConnection::AsyncCall
{
	using PHandler = function<void(ServerObject& /*obj*/, UINT32 /*replyId*/, CommMsgParser& /*parser*/, NoReplyAsyncCall& /*pCall*/)>;

	NoReplyAsyncCall(ServerObject& obj_, PHandler replyHandler_, const Data& data_)
		: obj(obj_)
		, replyHandler(replyHandler_)
		, data(data_)
	{ }
	virtual ~NoReplyAsyncCall() { }

	virtual void processReply(UINT32 /*reqId*/, UINT32 replyId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		replyHandler(obj, replyId, parser, *this);
	}
	void set(PHandler handler) { replyHandler = handler; }

	ServerObject& obj;
	Data data;

protected:
	PHandler replyHandler;
};

template<typename ServerObject, typename Data>
struct NoReplyAsyncCall<ServerObject, Data*> : public CommClientGConnection::AsyncCall
{
	using PHandler = function<void(ServerObject& /*obj*/, UINT32 /*replyId*/, CommMsgParser& /*parser*/, NoReplyAsyncCall& /*pCall*/)>;

	NoReplyAsyncCall(ServerObject& obj_, PHandler replyHandler_, Data* data_)
		: obj(obj_)
		, replyHandler(replyHandler_)
		, data(data_)
	{ }
	virtual ~NoReplyAsyncCall() { }

	virtual void processReply(UINT32 /*reqId*/, UINT32 replyId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		replyHandler(obj, replyId, parser, *this);
	}
	void set(PHandler handler) { replyHandler = handler; }

	ServerObject& obj;
	unique_ptr<Data> data; //lint !e1925

protected:
	PHandler replyHandler;
};

////////////////////////////////////////////////////////////////////////////////

template<typename ServerObject, typename Data>
struct GenericAsyncCall : public CommClientGConnection::AsyncCall
{
	using PHandler = function<void(ServerObject& /*obj*/, UINT32 /*replyId*/, CommMsgParser& /*parser*/, GenericAsyncCall& /*pCall*/)>;

	GenericAsyncCall(ServerObject& obj_, UINT32 reqId_, UINT32 connId_, PHandler replyHandler_, const Data& data_)
		: obj(obj_)
		, reqId(reqId_)
		, connId(connId_)
		, replyHandler(replyHandler_)
		, data(data_)
	{ }
	virtual ~GenericAsyncCall() { }

	virtual void processReply(UINT32 /*reqId*/, UINT32 replyId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		replyHandler(obj, replyId, parser, *this);
	}
	void set(PHandler handler) { replyHandler = handler; }

	UINT32 reqId;
	UINT32 connId;
	ServerObject& obj;
	Data data;

protected:
	PHandler replyHandler;
};

template<typename ServerObject, typename Data>
struct GenericAsyncCall<ServerObject, Data*> : public CommClientGConnection::AsyncCall
{
	using PHandler = function<void(ServerObject& /*obj*/, UINT32 /*replyId*/, CommMsgParser& /*parser*/, GenericAsyncCall& /*pCall*/)>;

	GenericAsyncCall(ServerObject& obj_, UINT32 reqId_, UINT32 connId_, PHandler replyHandler_, Data* data_)
		: obj(obj_)
		, reqId(reqId_)
		, connId(connId_)
		, replyHandler(replyHandler_)
		, data(data_)
	{ }
	virtual ~GenericAsyncCall() { }

	virtual void processReply(UINT32 /*reqId*/, UINT32 replyId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		replyHandler(obj, replyId, parser, *this);
	}
	void set(PHandler handler) { replyHandler = handler; }

	UINT32 reqId;
	UINT32 connId;
	ServerObject& obj;
	unique_ptr<Data> data; //lint !e1925

protected:
	PHandler replyHandler;
};

////////////////////////////////////////////////////////////////////////////////

#endif // GenericAsyncCall_h_included
