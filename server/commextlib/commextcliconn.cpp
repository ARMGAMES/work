#include <pplib.h>
#include <commlib.h>
#include "commextcliconn.h"
#include <numeric>
#include <algorithm>

CommExtProxyCliConn::~CommExtProxyCliConn()
{
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		CommExtProxyAsyncCall * asyncCall = *it;
		delete asyncCall;
	}
}

void CommExtProxyCliConn::post( CommExtProxyAsyncCall *asyncCall )
{
	PASSERT3(asyncCall);
	_List::iterator it = msgList.insert( msgList.end(), asyncCall );
	asyncCall->reqId = msgMap.add( it );
	asyncCall->sentTicks = SYSTEMTICKS();

	CommMsgBody copy;
	copy.copyFrom( asyncCall->requestBody );
	BYTE* p = copy._createPrefix( 4 );
	CommMsgBody::writeUINT32( p, asyncCall->reqId );
	CommClientConnection::post( asyncCall->msgId, copy );
	++numOutstanding;
}

void CommExtProxyCliConn::postBroadcast( UINT32 msgId, const CommMsgBody & request)
{
	CommMsgBody copy;
	copy.copyFrom(request );
	BYTE* p = copy._createPrefix( 4 );
	CommMsgBody::writeUINT32( p, 0);
	CommClientConnection::post(msgId, copy );
}


void CommExtProxyCliConn::processMessage( UINT32 msgId, const CommMsgBody& body  )
{
	
	CommMsgParser parser( body );
	UINT32 requestId;
	parser.parseUINT32( requestId );
	if (requestId == 0)
	{
		// it was broadcast, ignore reply;
		return;
	}
	_List::iterator* found = msgMap.find( requestId );
	if( found == 0 )
	{
		PLog( "CommExtProxyCliConn: unexpected msg with requestId=%08X (msgId=%08X) - ignored", requestId, msgId );
		if (msgMap.size() < maxOutstanding)
			obj->_getFirstMessage(this);
	}
	else
	{
		_List::iterator it = *found;
		CommExtProxyAsyncCall* asyncCall = *it;
		PASSERT3(asyncCall->reqId == requestId);
		obj->_calculateProcessingTime(asyncCall->sentTicks,asyncCall->putInQueueTicks);
		msgList.erase( it );
		msgMap.remove( requestId );

		if (msgMap.size() < maxOutstanding)
			obj->_getFirstMessage(this);

		CommMsgBody copyBody;
		copyBody._moveFromConstBody(body);
		copyBody._skipPrefix( 4 );
		asyncCall->connId = id();
		asyncCall->processReply( msgId, copyBody);
		--numOutstanding;
		delete asyncCall;
	}
}


void CommExtProxyCliConn::connected()
{
	bConnected = true;
	enable();
	PTRACE3("%s connected: server='%s/%s/%s'", obj->getName(), getServer(),getServerObject(),getServerConnType());
	obj->_connected();
}
bool CommExtProxyCliConn::closedOrDisconnected( int errCode, const char* errMsg )
{
	bConnected = false;
	PTRACE3("%s closed: server='%s/%s/%s', (%d) %s",obj->getName(), getServer(),getServerObject(),getServerConnType(),errCode, errMsg);
	disable();
	obj->_reassignRequests(this);
	return false;
}

void CommExtProxyCliConn::forEachMessage(const std::function<void(const CommExtProxyAsyncCall*)>& callback) const
{
	if (!callback)
	{
		return;
	}
	std::for_each(msgList.cbegin(), msgList.cend(), [&callback](const CommExtProxyAsyncCall* asyncCall){callback(asyncCall);});
}

void CommExtCliConnPool::_push_message(CommExtProxyAsyncCall * asyncCall)
{
	asyncCall->sentTicks = 0;
	asyncCall->reqId = 0;
	messages.push_back(asyncCall);
	messageListSize++;
	maxQueueSize = maxQueueSize >messageListSize ? maxQueueSize : messageListSize;
}

void CommExtCliConnPool::_calculateProcessingTime(UINT32 sent,UINT32 total)
{
	UINT32 currentTime = SYSTEMTICKS();
	sendingTime += currentTime - sent;
	UINT32 t = SYSTEMTICKS() - total;
	totalProcessingTime += t;
	maxProcessingTime = maxProcessingTime < t ? t : maxProcessingTime;
	processingNumber++;
	if (lastStatTimeInit)
	{
		if ((int)(lastStatTime - sent) > 0)
		{
			sendingTimeForLoad += currentTime - lastStatTime;
		}
		else
		{
			sendingTimeForLoad += currentTime - sent;
		}
	}
	else
	{
		sendingTimeForLoad += currentTime - sent;
	}
}
void CommExtCliConnPool::_calculateProcessingTimeOnMeasure(int & numActiveConnections, UINT32 & sendingAdjust, UINT32 & totalAdjust, int & numInConn, int & numInQueue)
{
	numActiveConnections = 0;
	sendingAdjust = 0;
	totalAdjust = 0;
	numInConn = 0;
	numInQueue = 0;
	ConnectionsList::iterator l;
	UINT32 currentTime = SYSTEMTICKS();
	for ( l = conns.begin(); l != conns.end(); ++l)
	{
		if ( (*l)->isEnabled() && (*l)->isConnected())
		{
			numActiveConnections++;
			CommExtProxyCliConn::_List::iterator it;
			for (it = (*l)->msgList.begin(); it != (*l)->msgList.end(); ++it)
			{
				CommExtProxyAsyncCall * asyncCall = (*it);
				numInConn++;
				sendingAdjust += currentTime - asyncCall->sentTicks;
				totalAdjust += currentTime - asyncCall->putInQueueTicks;
				if (lastStatTimeInit)
				{
					if ((int)(lastStatTime - asyncCall->sentTicks) > 0)
					{
						sendingTimeForLoad += currentTime - lastStatTime;
					}
					else
					{
						sendingTimeForLoad += currentTime - asyncCall->sentTicks;
					}
				}
				else
				{
					sendingTimeForLoad += currentTime - asyncCall->sentTicks;
				}
			}
		}
	}
	MessagesList::iterator m;
	for ( m = messages.begin(); m != messages.end(); ++m)
	{
		CommExtProxyAsyncCall *asyncCall = (*m);
		numInQueue++;
		totalAdjust += currentTime - asyncCall->putInQueueTicks;
	}
}

void CommExtCliConnPool::getStat(UINT32 & averageSendingTime, UINT32 & averageProcessingTime, UINT32 & maxProcessingTime_, UINT32 &maxQueueSize_, UINT32 & numberOfMessages, UINT32 & loadPercent)
{
	UINT32 sendingAdjust, totalAdjust;
	int numActiveConnections, numInQueue, numInConn;
	_calculateProcessingTimeOnMeasure(numActiveConnections,sendingAdjust,totalAdjust,numInConn,numInQueue);

	UINT32 averageTime1 = processingNumber ? (sendingTime / processingNumber) : 0;
	UINT32 averageTime2 = (processingNumber + numInConn ) ? ( (sendingTime + sendingAdjust) / (processingNumber + numInConn ) ) : 0;
	averageSendingTime = averageTime1 > averageTime2 ? averageTime1 : averageTime2;

	averageTime1 = processingNumber ? (totalProcessingTime / processingNumber) : 0;
	averageTime2 = (processingNumber + numInConn + numInQueue ) ? ( (totalProcessingTime + totalAdjust) / (processingNumber + numInConn + numInQueue ) ) : 0;
	averageProcessingTime = averageTime1 > averageTime2 ? averageTime1 : averageTime2;

	maxQueueSize_ = maxQueueSize;
	numberOfMessages = processingNumber;
	maxProcessingTime_ = maxProcessingTime;
	if (lastStatTimeInit)
	{
		UINT32 currentTime = SYSTEMTICKS();
		UINT32 period = (currentTime - lastStatTime) * numActiveConnections;
		loadPercent = period  ? ((sendingTimeForLoad * 100 ) / period ) : 0;
	}
	else
	{
		lastStatTimeInit = true;
		loadPercent = 0;
	}
	lastStatTime = SYSTEMTICKS();
	sendingTimeForLoad = 0;
	processingNumber = 0;
	sendingTime = 0;
	totalProcessingTime = 0;
	maxQueueSize = 0;
	maxProcessingTime = 0;
}


void CommExtCliConnPool::_reassignRequests(CommExtProxyCliConn * failedConn)
{
	CommExtProxyCliConn::_List::iterator it;
	for (it = failedConn->msgList.begin(); it != failedConn->msgList.end(); ++it)
	{
		CommExtProxyAsyncCall * asyncCall = (*it);
		CommExtProxyCliConn * newClientConn = _findConnection();
		if (newClientConn)
		{
			PTRACE5("CommExtCliConnPool::_reassignRequests: reassign request - clientConn=%08X, size=%d", newClientConn->id(),newClientConn->_size());
			newClientConn->post(asyncCall);
		}
		else
		{
			PTRACE5("CommExtCliConnPool::_reassignRequests: - moved to message queue");
			_push_message(asyncCall);
		}
	}
	failedConn->msgList.clear();
	failedConn->msgMap.clear();
	
	ConnectionsList::iterator l;
	for ( l = conns.begin(); l != conns.end(); ++l)
	{
		if ( (*l)->isEnabled() && (*l)->isConnected())
		{
			break;
		}
	}
	if (poolConnected && l == conns.end())
	{
		poolConnected = false;
		closedOrDisconnected();

	}



}

void CommExtCliConnPool::_connected()
{
	while(messages.size())
	{
		CommExtProxyCliConn * clientConn = _findConnection();
		if (clientConn)
		{
			if (!_getFirstMessage(clientConn))
			{
				break;
			}
		}
		else
			break;
	}
	if (poolConnected == false)
	{
		poolConnected = true;
		connected();
	}
}


CommExtProxyCliConn * CommExtCliConnPool::_findConnection()
{
	CommExtProxyCliConn *conn = nullptr;
	for (size_t i = 0; i < conns.size(); ++i)
	{
		UINT64 idx = lastSelectedConnectionIdx++ % conns.size();
		CommExtProxyCliConn* c = conns[idx];
		if (c->isEnabled() && c->isConnected() && c->_size() < maxOutstanding)
		{
			conn = c;
			break;
		}
	}
	if (conn)
	{
		PTRACE5("CommExtCliConnPool::findConnection: clientConn=%08X, size=%u, numOutstanding=%u", conn->id(), conn->_size(), conn->getNumOutstandingMsgs());
		return conn;
	}
	PTRACE5("CommExtCliConnPool::findConnection: connection not found");
	return 0;
}


void CommExtCliConnPool::postTo(CommExtProxyAsyncCall *asyncCall)
{
	if (lastStatTimeInit == false)
	{
		lastStatTimeInit = true;
		lastStatTime = SYSTEMTICKS();
	}

	asyncCall->putInQueueTicks = SYSTEMTICKS();
	CommExtProxyCliConn * clientConn = _findConnection();
	if (clientConn)
	{
		PTRACE5("CommExtCliConnPool::postTo - clientConn=%08X, size=%d", clientConn->id(),clientConn->_size());
		clientConn->post(asyncCall);
	}
	else
	{
		_push_message(asyncCall);
	}
}

void CommExtCliConnPool::postBroadcast(UINT32 msgId, const CommMsgBody& request)
{
	ConnectionsList::iterator l;

	for ( l = conns.begin(); l != conns.end(); ++l)
	{
		if ( (*l)->isEnabled() && (*l)->isConnected())
		{
				PTRACE5("CommExtCliConnPool::postBroadcast(%d_) to clientConn=%08X", msgId,(*l)->id());
				(*l)->postBroadcast(msgId, request);
		}

	}
}

bool CommExtCliConnPool::_getFirstMessage(CommExtProxyCliConn * clientConn)
{
	CommExtProxyAsyncCall *asyncCall = _pop_message();
	if (asyncCall)
	{
		clientConn->post(asyncCall);
		PTRACE5("CommExtCliConnPool::getFirstMessage - message found, size = %d",  messages.size());
		return true;
	}
	else
	{
		PTRACE5("CommExtCliConnPool::getFirstMessage - message not found");
		return false;
	}
}

void CommExtCliConnPool::readInit(const PIniFile::Section* sec)
{
	if( sec )
	{
		int maxOutstanding_ = sec->getIntProperty( "maxoutstanding",0);
		if (maxOutstanding_>0)
		{
			maxOutstanding = maxOutstanding_;
		}
		ConnectionsList tmpConns;
		ConnectionsList::iterator it;
		for (it=conns.begin();it != conns.end(); ++it)
		{
			tmpConns.push_back((*it));
		}
		conns.clear();

		int n = sec->items.size();
		for( int i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = sec->items[ i ];
			if( item.name.equals( "connect" ) )
			{
				PString server;
				PString serverObject;
				PString serverType;
				const char *p = item.value.c_str();
				const char *p1 = strchr(p,'/');
				if (!p1)
				{
					PTRACE3("%s - invalid format",p);
					continue;
				}
				const char *p2 = strchr(p1+1,'/');
				server.assign(p,p1);
				if (p2)
				{
					serverObject.assign(p1+1,p2);
					serverType.assign(p2+1);
				}
				else
					serverObject.assign(p1+1);
			
				PTRACE3("%s: server=%s, object=%s, type=%s",p,server.c_str(),serverObject.c_str(),serverType.c_str());
				bool found=false;
				for (it=tmpConns.begin();it != tmpConns.end(); ++it)
				{
					if ((*it)->isTheSame(server,serverObject,serverType))
					{
						PTRACE3("Dynamic init %s: found server='%s' serverObject='%s', type='%s'",getName(), server.c_str(), serverObject.c_str(), serverType.c_str());
						conns.push_back((*it));
						tmpConns.erase(it);
						found=true;
						break;
					}
				}
				if (!found)
				{
					CommExtProxyCliConn *conn = createConnection();
					cliPool.connect( *conn, server.c_str(), serverObject.c_str(), serverType.c_str());
					conn->enable();
					conns.push_back(conn);
					PTRACE3("Dynamic init %s: new created:  server='%s' serverObject='%s', type='%s'",getName(), server.c_str(), serverObject.c_str(), serverType.c_str());
				}
			}

		}

		for (it=tmpConns.begin();it != tmpConns.end(); ++it)
		{
			PTRACE3("Dynamic init %s: deleted:  server='%s' serverObject='%s', type='%s'",getName(), (*it)->getServer(),(*it)->getServerObject(),(*it)->getServerConnType());
			(*it)->closedOrDisconnected(0,"");
			cliPool.disconnect(*(*it));
			delete (*it);
		}
	}
}

CommExtProxyCliConn* CommExtCliConnPool::createConnection()
{
	return new CommExtProxyCliConn(*this, guardFactory, maxOutstanding);
}

CommExtProxyCliConn* CommExtCliConnPool::findConnection( UINT32 connId )
{
	return static_cast< CommExtProxyCliConn* >( cliPool.findConnection( connId ) );
}

void CommExtCliConnPool::connect(const char * server, const char * serverObject, const char * serverType, int maxOutstanding_)
{
	int maxOut = maxOutstanding_ > 0 ? maxOutstanding_ : maxOutstanding;
	CommExtProxyCliConn *conn = new CommExtProxyCliConn( *this, guardFactory, maxOut);
	cliPool.connect( *conn, server, serverObject, serverType);
	conn->enable();
	conns.push_back(conn);
	PTRACE3("Connection added %s: server='%s' serverObject='%s', type='%s'",getName(), server, serverObject, serverType);
}

CommExtCliConnPool::~CommExtCliConnPool()
{
	for(auto conn : conns)
	{
		delete conn;
	}
	for(auto message : messages)
	{
		delete message;
	}
}

void CommExtCliConnPool::disconnect()
{
	for (auto conn : conns)
	{
		cliPool.disconnect(*conn);
		delete conn;
	}
	conns.clear();
	for (auto message : messages)
	{
		delete message;
	}
	messages.clear();
	poolConnected = false;
}

UINT32 CommExtCliConnPool::getNumOutstandingMsgs() const
{
	return std::accumulate(conns.cbegin(), conns.cend(), 0, [](UINT32 s, const CommExtProxyCliConn* conn){return s + (conn ? conn->getNumOutstandingMsgs() : 0);});
}

void CommExtCliConnPool::forEachConnection(const std::function<void(const CommExtProxyCliConn*)>& callback) const
{
	if (!callback)
	{
		return;
	}
	std::for_each(conns.cbegin(), conns.cend(), [&callback](const CommExtProxyCliConn* conn){callback(conn);});
}
