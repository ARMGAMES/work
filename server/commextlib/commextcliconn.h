#ifndef commextcliconn_h_included
#define commextcliconn_h_included

#include "pplib.h"
#include "commlib.h"
#include <functional>

class CommExtCliConnPool;
class CommExtProxyAsyncCall;

class CommExtProxyCliConn : public CommClientConnection
{
private:
	CommExtCliConnPool* obj;
	bool bConnected;
	bool bEnabled;
	int maxOutstanding;
	typedef list< /* new */ CommExtProxyAsyncCall* > _List;
	typedef CommIdMap< _List::iterator > _Map;

	_List msgList;
	_Map msgMap;
	UINT32 numOutstanding;

public:
	CommExtProxyCliConn( CommExtCliConnPool& obj_, CommClientGuardFactory& guardFactory_ , int maxOutstanding_ ) : CommClientConnection( guardFactory_ )
	{
		obj = &obj_;
		bConnected = false;
		bEnabled = false;
		maxOutstanding = maxOutstanding_;
		numOutstanding = 0;
	}
	~CommExtProxyCliConn();

	void post( CommExtProxyAsyncCall *asyncCall);
	void postBroadcast( UINT32 msgId, const CommMsgBody & request);

	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	UINT32 _size() const 
	{
		return msgMap.size();
	}
	UINT32 getNumOutstandingMsgs() const
	{
		return numOutstanding;
	}
	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override;
	bool isConnected() const
	{
		return bConnected;
	}
	bool isEnabled() const
	{
		return bEnabled;
	}
	void enable()
	{
		bEnabled = true;
	}
	void disable()
	{
		bEnabled = false;
	}
	bool isTheSame(const char * server,const char * serverObject, const char * connectionType) const
	{
		return (!strcmp(getServer(),server) &&
			    !strcmp(getServerObject(),serverObject) &&
				!strcmp(getServerConnType(),connectionType)
				);
	}

	void forEachMessage(const std::function<void(const CommExtProxyAsyncCall*)>& callback) const;

	friend class CommExtCliConnPool;
};

class CommExtCliConnPool
{
protected:
	CommClientConnectionPool & cliPool;
	CommClientGuardFactory & guardFactory;
	UINT32 sendingTimeForLoad;
	UINT32 sendingTime;
	UINT32 totalProcessingTime;
	UINT32 maxProcessingTime;
	UINT32 lastStatTime;
	bool lastStatTimeInit;
	UINT32 processingNumber;
	UINT32 maxQueueSize;
	typedef vector <CommExtProxyCliConn *> ConnectionsList;
	ConnectionsList conns;
	typedef list <CommExtProxyAsyncCall *> MessagesList; 
	MessagesList messages;
	UINT32       messageListSize;
//	PString name;
	int maxOutstanding;
	bool poolConnected;
	UINT64 lastSelectedConnectionIdx;

	virtual CommExtProxyCliConn* createConnection();
	CommExtProxyCliConn* findConnection( UINT32 connId );

private:
	void _calculateProcessingTime(UINT32 sent,UINT32 total);
	void _reassignRequests(CommExtProxyCliConn * failedConn);
	CommExtProxyCliConn * _findConnection();
	bool _getFirstMessage(CommExtProxyCliConn * clientConn);
	void _connected();
	void _push_message(CommExtProxyAsyncCall * asyncCall);
	CommExtProxyAsyncCall * _pop_message()
	{
		MessagesList::iterator m = messages.begin();
		if (m != messages.end())
		{
			CommExtProxyAsyncCall *asyncCall = (*m);
			messages.pop_front();
			if (messageListSize)
				messageListSize--;
			return asyncCall;
		}
		else
		{
			return 0;
		}
	}
	void _calculateProcessingTimeOnMeasure(int & numActiveConnections, UINT32 & sendingAdjust, UINT32 & totalAdjust, int & numInConn, int & numInQueue);

public:
	CommExtCliConnPool(CommClientConnectionPool & cliPool_,CommClientGuardFactory & guardFactory_, int maxOutstanding_) : cliPool(cliPool_), guardFactory(guardFactory_)
	{
		sendingTime = 0;
		totalProcessingTime = 0;
		maxProcessingTime = 0;
		processingNumber = 0;
		maxOutstanding = maxOutstanding_;
		poolConnected = false;
		maxQueueSize = 0;
		messageListSize = 0;
		sendingTimeForLoad = 0;
		sendingTime = 0;
		totalProcessingTime = 0;
		lastStatTime = 0;
		lastStatTimeInit = false;
		lastSelectedConnectionIdx = 0;
	}
	virtual ~CommExtCliConnPool();
	void postTo(CommExtProxyAsyncCall *asyncCall);
	void postBroadcast(UINT32 msgId, const CommMsgBody& request);
	void readInit(const PIniFile::Section* sec);
	void getStat(UINT32 & averageSendingTime, UINT32 & averageProcessingTime, UINT32 & maxProcessingTime_, UINT32 &maxQueueSize_, UINT32 & numberOfMessages, UINT32 & loadPercent);
	virtual const char * getName()=0;
	virtual void connected() {}
	virtual void closedOrDisconnected() {}
	bool isEnabled() const { return !conns.empty(); }
	void connect(const char * server, const char * serverObject, const char * serverType, int maxOutstanding_=0);
	void disconnect();
	UINT32 getNumOutstandingMsgs() const;
	void forEachConnection(const std::function<void(const CommExtProxyCliConn*)>& callback) const;

	friend class CommExtProxyCliConn;
};

class CommExtProxyAsyncCall 
{
private:
	UINT32 putInQueueTicks;
	UINT32 sentTicks;
	UINT32 reqId;
	UINT32 connId;

public:
	UINT32 msgId;
	CommMsgBody requestBody;


public:
	virtual void processReply(UINT32 msgId, CommMsgBody& body )
	{
	}

	CommExtProxyAsyncCall(UINT32 msgId_, CommMsgBody &requestBody_ )
	{
		msgId = msgId_;
		reqId = 0;
		putInQueueTicks = 0;
		sentTicks = 0;
		requestBody.moveFrom(requestBody_);
		connId = 0;
	}
	virtual ~CommExtProxyAsyncCall()
	{
	}
	virtual void composeStat(CommMsgBody & reply)
	{
	}

	UINT32 getPutInQueueTicks() const {return putInQueueTicks;}
	UINT32 getSentTicks() const {return sentTicks;}
	UINT32 getReqId() const {return reqId;}
	UINT32 getConnId() const { return connId; }

	friend class CommExtCliConnPool;
	friend class CommExtProxyCliConn;

};

#endif