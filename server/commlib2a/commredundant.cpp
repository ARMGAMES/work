#include "commredundant.h"

#define TIMER_CHECK_CLIENT_CONNECTIONS 11
#define TIMER_CHECK_SERVER_CONNECTIONS 12
#define TIMER_CLIENT_CONNECT 13
#define TIMER_PRINT_STAT 14



CommServerNullGuardFactory CommRedundantConnectionsObject::guardFactory;
CommClientNullGuardFactory CommRedundantConnectionsObject::guardFactoryC;


void CommRedundantConnectionsObject::init(CommMsgBody& initMsg)
{

	CommMsgParser parser(initMsg);
	const char * parameters;
	parser.parseString(parameters);
	while (parameters && *parameters)
	{
		if (!strnicmp(parameters,"timeout=",8))
		{
			parameters+=8;
			timeOut = atoi(parameters);
			PTRACE3("CommRedundantConnectionsObject::init - timeOut = %d",timeOut);
		}
		else if (!strnicmp(parameters,"connecttimeout=",15))
		{
			parameters+=15;
			connectTimeOut = atoi(parameters);
			PTRACE3("CommRedundantConnectionsObject::init - connection timeOut = %d",connectTimeOut);
		}
		else if (!strnicmp(parameters,"printstat=",10))
		{
			parameters+=10;
			printStatTime = atoi(parameters);
			PTRACE3("CommRedundantConnectionsObject::init - printStatTimet = %d",printStatTime);
		}
		else if (!strnicmp(parameters,"type=",5))
		{
			parameters+=5;
			int flowType_ = atoi(parameters);
			switch(flowType_)
			{
			case 0:flowType = neverDropRedundancy;break;
			case 1:flowType = dropRedundancy;break;
			case 2:flowType = nonRedundant;break;
			default:PTRACE3("CommRedundantConnectionsObject::init - wrong flowType parameter = %d",flowType_);
			}
			PTRACE3("CommRedundantConnectionsObject::init - flowType = %d",flowType_);
		}
		else if (!strnicmp(parameters,"conntreshold=",13))
		{
			parameters+=13;
			perConnectionThreshold = atoi(parameters);
			PTRACE3("CommRedundantConnectionsObject::init - perConnectionThreshold = %d",perConnectionThreshold);
		}
		else if (!strnicmp(parameters,"treshold=",9))
		{
			parameters+=9;
			globalThreshold = atoi(parameters);
			PTRACE3("CommRedundantConnectionsObject::init - globalThreshold = %d",globalThreshold);
		}
		else if (!strnicmp(parameters,"treshold=",9))
		{
			parameters+=9;
			globalThreshold = atoi(parameters);
			PTRACE3("CommRedundantConnectionsObject::init - globalThreshold = %d",globalThreshold);
		}
		else if (!strnicmp(parameters,"address=",8))
		{
			outgoingConnection = true;
			parameters+=8;
			const char * addresses = strchr(parameters,'/');
			PASSERT3(addresses);
			serverObject.assign(parameters,addresses);
			addresses++;
			int num=0;
			while(addresses)
			{
				PString address;
				const char * p1 = strchr(addresses,'/');
				if (p1)
				{
					address.assign(addresses,p1);
					addresses=p1+1;
				}
				else
				{
					p1 = strchr(addresses,';');
					if (p1)
					{
						address.assign(addresses,p1);
					}
					else
					{
						address.assign(addresses);
					}
					addresses=0;
				}
				PTRACE3("CommRedundantConnectionsObject::init - new client connection: %d, address=%s serverObject=%s", num, address.c_str(),serverObject.c_str());
				clientConnections.insert(clientConnections.end(),new CommRedundantClientConnection(*this,num++,guardFactoryC,address,serverObject));
			}
			PASSERT3(clientConnections.size() && clientConnections.size()<=32);
			uniqueClientPhysConnId = nextUniqueId();
			uniqueServerPhysConnId = 0;
		}
		parameters=strchr(parameters,';');
		if (parameters)
			parameters++;
	}
	if (!clientConnections.size())
	{
		PTRACE3("CommRedundantConnectionsObject::init - object is configured for incoming server connections");
		outgoingConnection = false;
		srvPool.registerFactory( "redundant", serverConnectionFactory );
		uniqueClientPhysConnId = 0;
		uniqueServerPhysConnId = nextUniqueId();
	}
	if (printStatTime)
	{
		CommMsgBody dummy;
		postTimerMessage(TIMER_PRINT_STAT, dummy, printStatTime);
	}


}

void CommRedundantServerConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processServerMessage( msgId, body, this );
}

void CommRedundantServerConnection::connected()
{
	obj->connectedServer(this);
}

void CommRedundantServerConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	obj->disconnectedServer(this);
}


void CommRedundantClientConnection::processMessage(  UINT32 msgId, const CommMsgBody& body)
{
	obj->processClientMessage( msgId, body, this );
}

void CommRedundantClientConnection::connected()
{
	established=true;
	obj->connectedClient(this);
}

bool CommRedundantClientConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	established=false;
	obj->disconnectedClient(this);
	return false; // always is trying to restore connection
}


CommRedundantServerConnection::CommRedundantServerConnection( CommRedundantConnectionsObject* obj_ )
: CommServerConnection()
{
	obj = obj_;
	srvId=obj->nextUniqueId();
	clnId=0;
	clnNum = -1;
	lastSent = 0;
	lastReceived = 0;
	lastAck = 0;
	outstandingNumber = 0;
	outstandingSize = 0;
	lastIteratorValid = false;

}

CommRedundantClientConnection::CommRedundantClientConnection( CommRedundantConnectionsObject& obj_, int num_, CommClientGuardFactory& guardFactory_, const char * serverAddress_, const char * serverObject_ )
: CommClientConnection( guardFactory_ )
{
	established = false;
	obj = &obj_;
	clnNum=num_;
	srvId=0;
	clnId=obj->nextUniqueId();
	serverAddress.assign(serverAddress_);
	serverObject.assign(serverObject_);
	lastSent = 0;
	lastReceived = 0;
	lastAck = 0;
	outstandingNumber = 0;
	outstandingSize = 0;
	lastIteratorValid = false;

}
void CommRedundantClientConnection::reinit()
{
	established = false;
	srvId=0;
	clnId=obj->nextUniqueId();
	// just in case:
	lastSent = 0;
	lastReceived = 0;
	lastAck = 0;
	outstandingNumber = 0;
	outstandingSize = 0;
	lastIteratorValid = false;

}

void CommRedundantClientConnection::postCloseConnection()
{
	PTRACE4("CommRedundantClientConnection::postCloseConnection, clnId=%d",clnId);
	CommMsgBody body;
	body.composeUINT32(obj->uniqueClientPhysConnId)
		.composeUINT32(obj->uniqueServerPhysConnId)
		.composeUINT32(clnId)
		.composeUINT32(srvId)
		.composeINT32(clnNum);
	post(COMMMCOMPOSITE_CLOSE_MSG,body);
	srvId=0;
}

void CommRedundantServerConnection::postCloseConnection()
{
	PTRACE4("CommRedundantServerConnection::postCloseConnection, srvId=%08X",srvId);
	CommMsgBody body;
	body.composeUINT32(obj->uniqueClientPhysConnId)
		.composeUINT32(obj->uniqueServerPhysConnId)
		.composeUINT32(clnId)
		.composeUINT32(srvId)
		.composeINT32(clnNum);
	post(COMMMCOMPOSITE_CLOSE_MSG,body);
	clnId=0;
}

void CommRedundantConnectionsObject::postServerError(CommRedundantServerConnection *conn, UINT32 physId, UINT32 cliId, UINT32 errCode)
{
	PTRACE4("CommRedundantConnectionsObject::postServerError, physId=%08X, cliId=%08X, err=%u",physId,cliId,errCode);
	CommMsgBody body;
	body.composeUINT32(physId)
		.composeUINT32(cliId)
		.composeUINT32(errCode);
	conn->post(COMMMCOMPOSITE_ERROR_MSG,body);
}



// Instead of using post() function in connections, we are going to use our special functions - 
// postRedundant(). Because msgId and format already composed in message body we don't need to 
// compose them again using 	_CommConnection::_wrapUserMsg( body, msgId )
// It allows us to have CommMsgBody constant.
// !!!! CommServerConnection, CommClientConnection and the both pools now have
// new friend classes - redundant server and client connection correspondingly.
// Later consider moving these function to server and client connection
void CommRedundantServerConnection::postRedundant(const CommMsgBody& body )
{
	PASSERT3( pool );
	if( state != _CommConnection::_StateConnOk )
	{
		return;
	}

	PASSERT3( guard );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_SERVER_USERMSG, priority );
	msg.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT32( sessionId );
	// actually, processOutBlock doesn't change source body. It has non-constant parameter just because 
	// in original post() function CommMsgBody is non-constatnt because of _CommConnection::_wrapUserMsg( body, msgId )
	guard->processOutBlock( msg.body, (CommMsgBody&) body ); //processOutBlock doesn't change src body, 
	pool->_post( msg );
}

void CommRedundantClientConnection::postRedundant(const CommMsgBody& body )
{
	PASSERT3( pool );
	if( state != _CommConnection::_StateConnOk )
	{
		return;
	}

	PASSERT3( guard );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority);
	msg.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT32( sessionId );
	guard->processOutBlock( msg.body,(CommMsgBody&) body ); //processOutBlock doesn't change src body
	pool->_post( msg );
}


void CommRedundantConnectionsObject::closePhysicalConnection(CommRedundantServerConnection * excludedServerConnection)
{
	// parameter excludedServerConnection is almost always equal 0, so we will close all connections.
	// One exception when we get new incoming server connection - we want close everything excluding this new one.

	PTRACE3("CommRedundantConnectionsObject::closePhysicalConnection, excluded = %08X", (excludedServerConnection?excludedServerConnection->id():0));
	if (physConnectionExist)
	{
		disconnectPhysConnection();
	}
	physConnectionEstablished = false;
	physConnectionExist = false;
	reinitObject(excludedServerConnection);
}


void CommRedundantConnectionsObject::initNewServerConnection(CommMsgParser & parser,CommRedundantServerConnection * conn)
{
	PASSERT3(!outgoingConnection);

	UINT32 clientPhysConnId;
	UINT32 clientLogConnId;
	UINT32 serverPhysConnId;
	INT32  clientNum;
	parser.parseUINT32(clientPhysConnId)
		.parseUINT32(serverPhysConnId)
		.parseUINT32(clientLogConnId)
		.parseINT32(clientNum);
	PTRACE3("CommRedundantConnectionsObject::initNewServerConnection clientPhysConn=%08X serverPhysConnId=%08X clientLogConn=%08X clientNum=%d",clientPhysConnId,serverPhysConnId,clientLogConnId,clientNum);

	if (serverPhysConnId)
	{
		// Client thinks that he is reestablish existing phys connection
		if (serverPhysConnId != uniqueServerPhysConnId)
		{
			// he is wrong! Close this logical connection
			postServerError(conn,clientPhysConnId,clientLogConnId,COMMCOMPOSITE_ERROR_WRONG_SERVER_PHYSCONNECTION);
			srvPool.disconnect(conn);
			return;
		}

	}

	if (!uniqueClientPhysConnId)
	{
		// this is first connection for this physical connection.
		// At this moment phys connection on this side shouldn't exist
		PASSERT3(!physConnectionExist && !physConnectionEstablished);

		uniqueClientPhysConnId = clientPhysConnId;
		conn->clnId = clientLogConnId;
		conn->clnNum = clientNum;
		createIncomingPhysConnection("pyrlocal://");
		physConnectionExist = true;
		physConnectionConnected(1);
		physConnectionEstablished = true;
	}
	else
	{
		PTRACE3("CommRedundantConnectionsObject::initNewServerConnection old client phys connection exists = %08X",uniqueClientPhysConnId);
		// phys connection exists, or at least server is thinking that it exists
		if (clientPhysConnId != uniqueClientPhysConnId)
		{
			// Two possibilities: 
			// 1. Client lost connections, and recreate phys connection but server 
			//    didn't know about that.
			// 2. This is delayed connection from previous sessions

			// Check if we have any other server connections:
			if (isAnyServerConnectionsExist()) // this new connection will not be counted because it has zero clnId
			{
				// we have working connection, client will solve this problem
				postServerError(conn,clientPhysConnId,clientLogConnId,COMMCOMPOSITE_ERROR_WRONG_CLIENT_PHYSCONNECTION);
				srvPool.disconnect(conn);
				return;
			}
			else
			{
				// reinit object
				closePhysicalConnection(conn); // we don't want to close this new incoming connection, 
				uniqueClientPhysConnId = clientPhysConnId;
				conn->clnId = clientLogConnId;
				conn->clnNum = clientNum;
				createIncomingPhysConnection("pyrlocal://");
				physConnectionExist = true;
				physConnectionConnected(1);
				physConnectionEstablished = true;
			}
		}
		else
		{
			// check that we don't have other server connection with the same client
			CommServerConnectionPool::ConnectionIterator it;
			CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(it); // safe because there are no other types of connection by design
			while (srvConn)
			{
				if (srvConn->clnNum ==clientNum )
				{
					break;
				}
				srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(it); // safe because there are no other types of connection by design
			}
			if (srvConn)
			{
				// drop old connection
				srvPool.disconnect(srvConn);
			}
			conn->clnId = clientLogConnId;
			conn->clnNum = clientNum;
		}
	}

	conn->lastSent = 0;
	conn->lastReceived = 0;
	conn->lastAck = 0;
	conn->outstandingNumber = 0;
	conn->outstandingSize = 0;
	conn->lastIteratorValid = false;

	CommMsgBody body;
	body.composeUINT32(uniqueClientPhysConnId)
		.composeUINT32(uniqueServerPhysConnId)
		.composeUINT32(conn->clnId)
		.composeUINT32(conn->srvId)
		.composeINT32(conn->clnNum);
	conn->post(COMMMCOMPOSITE_INIT_REPLY_MSG,body);

	postQueue(conn, false);
}

void CommRedundantConnectionsObject::replyOnInitClientConnection(CommMsgParser & parser,CommRedundantClientConnection * conn)
{
	PASSERT3(outgoingConnection);

	UINT32 clientPhysConnId;
	UINT32 clientLogConnId;
	UINT32 serverPhysConnId;
	UINT32 serverLogConnId;
	INT32  clientNum;
	parser.parseUINT32(clientPhysConnId)
		.parseUINT32(serverPhysConnId)
		.parseUINT32(clientLogConnId)
		.parseUINT32(serverLogConnId)
		.parseINT32(clientNum);

	PTRACE3("CommRedundantConnectionsObject::replyOnInitClientConnection clientPhysConn=%08X serverPhysConnId=%08X clientLogConn=%08X serverLogConnId =%08X clientNum=%d",clientPhysConnId,serverPhysConnId,clientLogConnId,serverLogConnId,clientNum);

	if (clientPhysConnId == uniqueClientPhysConnId &&
		( !uniqueServerPhysConnId || serverPhysConnId == uniqueServerPhysConnId ) &&
		clientLogConnId == conn->clnId &&
		clientNum == conn->clnNum)
	{
		// ok
		PASSERT3(physConnectionExist);
		conn->srvId = serverLogConnId;
		if  (!uniqueServerPhysConnId)
		{
			uniqueServerPhysConnId = serverPhysConnId;
		}
		if (!physConnectionEstablished)
		{
			physConnectionConnected(1);
			physConnectionEstablished = true;
		}
		conn->lastSent = 0;
		conn->lastReceived = 0;
		conn->lastAck = 0;
		conn->outstandingNumber = 0;
		conn->outstandingSize = 0;
		conn->lastIteratorValid = false;

		postQueue(conn,false);
	}
	else
	{
		PTRACE3("CommRedundantConnectionsObject::replyOnInitClientConnection - wrong IDs: uniqueClientPhysConnId=%08X uniqueServerPhysConnId=%08X conn->clnId=%08X conn->clnNum=%d",uniqueClientPhysConnId,uniqueServerPhysConnId,conn->clnId,conn->clnNum);
		cliPool.disconnect(*conn);
		conn->reinit();

	}
}

void CommRedundantConnectionsObject::moveIterator(OutQueueListIterator it)
{
	if (outgoingConnection)
	{
		for (int i=0;i<clientConnections.size();i++)
		{
			if (clientConnections[i]->srvId && clientConnections[i]->lastIteratorValid && clientConnections[i]->itr != outQueue.end())
			{
				if (it == clientConnections[i]->itr)
					++clientConnections[i]->itr;
			}
		}
	}
	else
	{
		CommServerConnectionPool::ConnectionIterator its;
		CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(its); // safe because there are no other types of connection by design
		while (srvConn)
		{
			if (srvConn->clnId && srvConn->lastIteratorValid && srvConn->itr != outQueue.end())
			{
				if (it == srvConn->itr)
					++srvConn->itr;
			}
			srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(its); // safe because there are no other types of connection by design
		}

	}
}

void CommRedundantConnectionsObject::setNewIterator(OutQueueListIterator it)
{
	struct _RedundantItem &item = (*it);
	if (outgoingConnection)
	{
		for (int i=0;i<clientConnections.size();i++)
		{
			if (clientConnections[i]->srvId && clientConnections[i]->lastIteratorValid)
			{
				UINT32 mask = (1<<clientConnections[i]->clnNum);
				int del;
				if (clientConnections[i]->itr == outQueue.end() || (del = item.orderNum - (*clientConnections[i]->itr).orderNum) < 0)
				{
					clientConnections[i]->itr = it;
					while(clientConnections[i]->itr != outQueue.end())
					{
						if ((*clientConnections[i]->itr).connectionMask & mask)
							++clientConnections[i]->itr;
						else
							break;
					}

				}
			}
		}
	}
	else
	{
		CommServerConnectionPool::ConnectionIterator its;
		CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(its); // safe because there are no other types of connection by design
		while (srvConn)
		{
			if (srvConn->clnId && srvConn->lastIteratorValid)
			{

				UINT32 mask = (1<<srvConn->clnNum);
				int del = item.orderNum - (*srvConn->itr).orderNum;
				if (del<0)
				{
					srvConn->itr = it;
					while(srvConn->itr != outQueue.end())
					{
						if ((*srvConn->itr).connectionMask & mask)
							++srvConn->itr;
						else
							break;
					}

				}
			}
			srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(its); // safe because there are no other types of connection by design
		}

	}
}
void CommRedundantConnectionsObject::calculateStatistic(const struct _RedundantItem &item)
{
	UINT32 tm = SYSTEMTICKS();
	UINT32 deltaLive = tm - item.ticksCreated;
	UINT32 deltaRoundTrip = tm - item.ticksSent;
	stat_maxRoundTrip = deltaRoundTrip > stat_maxRoundTrip ? deltaRoundTrip : stat_maxRoundTrip;
	stat_maxLive = deltaLive > stat_maxLive ? deltaLive : stat_maxLive;
	stat_averageRoundTrip += deltaRoundTrip;
	stat_averageLive += deltaLive;
	UINT32 tmp = deltaRoundTrip/50;
	if (tmp >= (sizeof(stat_distribution)/sizeof(stat_distribution[0])))
		tmp=sizeof(stat_distribution)/sizeof(stat_distribution[0]) - 1 ;
	stat_distribution[tmp]++;
	stat_numMessages++;
}

bool CommRedundantConnectionsObject::clearQueue(UINT32 counter)
{
	PTRACE5("CommRedundantConnectionsObject::clearQueue currentMsgSend=%d, counter=%d",currentMsgSend,counter);
	int delta = currentMsgSend - counter;
	if (delta<0)
	{
		PTRACE3("CommRedundantConnectionsObject::clearQueue currentMsgSend=%d, counter=%d, close everything",currentMsgSend,counter);
		// impossible!
		closePhysicalConnection();
		return false;
	}
	OutQueueListIterator it;
	it = outQueue.begin();
	if (it == outQueue.end())
		return false;

	if (flowType == neverDropRedundancy)
	{
		struct _RedundantItem &item = (*it);
		int del = counter - item.orderNum;
		if (del > 0)
		{
			// it should never happen, close connection
			PTRACE3("CommRedundantConnectionsObject::clearQueue - first orderNum==%d, counter=%d, close everything",item.orderNum,counter);
			closePhysicalConnection();
			return false;
		}
		if (del == 0)
		{
			PTRACE5("CommRedundantConnectionsObject::clearQueue delete %d",item.orderNum);
			if (printStatTime)
				calculateStatistic(item);
			totalSize -= item.body._size();
			moveIterator(it);
			outQueue.erase(it);
			queueSize--;
		}
		return true;
	}

	// in this case it is possible to receive confirmation for any message in outgoing queue

	while(it !=outQueue.end())
	{
		struct _RedundantItem &item = (*it);
		if (item.orderNum == counter)
		{
			PTRACE5("CommRedundantConnectionsObject::clearQueue delete %d",item.orderNum);
			if (printStatTime)
				calculateStatistic(item);
			totalSize -= item.body._size();
			moveIterator(it);
			outQueue.erase(it);
			queueSize--;
			break;
		}
		else 
		{
			int del = counter - item.orderNum;
			if (del<0)
				break;
		}
		++it;
	}
	return true;
}

OutQueueListIterator CommRedundantConnectionsObject::cleanMask(UINT32 clnNum)
{
	UINT32 mask = (1<<clnNum);
	PTRACE4("CommRedundantConnectionsObject::cleanMask clnNum=%d",clnNum);
	OutQueueListIterator it;
	OutQueueListIterator first;
	bool firstSet=false;
	it = outQueue.begin();
	while(it != outQueue.end())
	{
		struct _RedundantItem &item = (*it);
		if (item.connectionMask & mask)
		{
			if (!firstSet)
			{
				first = it;
				firstSet = true;
			}
			item.connectionMask &=~mask;
		}
		++it;
	}
	if (firstSet)
		return first;
	else
		return outQueue.end();
}		




void CommRedundantConnectionsObject::postQueue(CommRedundantServerConnection * conn, bool upTreshold)
{

	if (!conn->lastIteratorValid)
	{
		conn->itr = outQueue.begin();
		conn->lastIteratorValid = true;
	}

	UINT32 mask = (1<<conn->clnNum);
	while(conn->itr != outQueue.end())
	{
		struct _RedundantItem & item = (*conn->itr);
		bool toSend = false;
		if (! (item.connectionMask & mask))
		{
			if (flowType == nonRedundant || (upTreshold && flowType == dropRedundancy))
			{
				if (!item.connectionMask)
				{
					toSend = true;
				}
			}
			else
				toSend=true;
		}
		if (toSend)
		{
			if (!perConnectionThreshold || perConnectionThreshold >= conn->outstandingSize)
			{
				UINT32 size=item.body._size();
				conn->outstandingSize += size;
				conn->lastSent = item.orderNum;
				conn->outstandingNumber++;
				if (!item.connectionMask)
				{
					//first post of this message
					int del = currentMsgSend - item.orderNum;
					if (del <0)
						currentMsgSend = item.orderNum;
					item.ticksSent = SYSTEMTICKS();
				}
				item.connectionMask |= (1<<conn->clnNum);
				conn->postRedundant(item.body);
				PTRACE5("CommRedundantConnectionsObject::postQueue(s) post %d",item.orderNum);
			}
			else
				break;
		}
		++conn->itr;
	}
}


void CommRedundantConnectionsObject::postQueue(CommRedundantClientConnection * conn, bool upTreshold)
{
	if (!conn->lastIteratorValid)
	{
		conn->itr = outQueue.begin();
		conn->lastIteratorValid = true;
	}
	UINT32 mask = (1<<conn->clnNum);
	while(conn->itr != outQueue.end())
	{
		struct _RedundantItem & item = (*conn->itr);
		bool toSend = false;
		if (! (item.connectionMask & mask))
		{
			if (flowType == nonRedundant || (upTreshold && flowType == dropRedundancy))
			{
				if (!item.connectionMask)
				{
					toSend = true;
				}
			}
			else
				toSend=true;
		}
		if (toSend)
		{
			if (!perConnectionThreshold || perConnectionThreshold >= conn->outstandingSize)
			{
				UINT32 size=item.body._size();
				conn->outstandingSize += size;
				conn->lastSent = item.orderNum;
				conn->outstandingNumber++;
				if (!item.connectionMask)
				{
					//first post of this message
					int del = currentMsgSend - item.orderNum;
					if (del <0)
						currentMsgSend = item.orderNum;
					item.ticksSent = SYSTEMTICKS();
				}
				item.connectionMask |= (1<<conn->clnNum);
				conn->postRedundant(item.body);
				PTRACE5("CommRedundantConnectionsObject::postQueue(c) post %d",item.orderNum);
			}
			else
				break;
		}
		++conn->itr;
	}
}

void CommRedundantConnectionsObject::checkPhysQueue()
{
	InPhysQueueListIterator it;
	it = physQueue.begin();
	while(it != physQueue.end())
	{
		struct _RedundantIncomingItem & item = (*it);
		if (item.orderNum == currentMsgReceived)
		{
			CommMsgParser parser(item.body);
			UINT32 counter;
			parser.parseUINT32(counter);
			while(!parser.parseEnded())
			{
				UINT32 msgSize;
				_CommMsg msg;
				parser.parseBYTE(msg.priority)
					.parseUINT16(msg.flags)
					.parseBYTE(msg.type)
					.parseUINT32(msgSize);
				BYTE * data = msg.body._append(msgSize);
				const BYTE *dataRead;
				parser._parseFixedBlock(dataRead,msgSize);
				memcpy(data,dataRead,msgSize);
				postToPhysConnection(msg,0,0);
			}

			PTRACE5("CommRedundantConnectionsObject::checkPhysQueue, currentMsgReceived=%d, counter=%d",currentMsgReceived,counter);
			currentMsgReceived++;
			physQueue.erase(it);
			it = physQueue.begin();
		}
		else
		{
			PTRACE5("CommRedundantConnectionsObject::checkPhysQueue currentMsgReceived=%d, item.orderNum=%d",currentMsgReceived,item.orderNum);
			break;
		}
	}
}


void CommRedundantConnectionsObject::processServerMessage( UINT32 msgId, const CommMsgBody& body, CommRedundantServerConnection* conn )
{
	try
	{
		switch(msgId)
		{
		case COMMMCOMPOSITE_INIT_MSG:
			{
				PASSERT3(!outgoingConnection);
				CommMsgParser parser(body);
				initNewServerConnection(parser,conn);
				break;

			}
		case COMMMCOMPOSITE_CLOSE_MSG:
			{
				// Client ask us to close everything
				CommMsgParser parser(body);
				UINT32 clientPhysConnId;
				UINT32 clientLogConnId;
				UINT32 serverPhysConnId;
				UINT32 serverLogConnId;

				INT32  clientNum;
				parser.parseUINT32(clientPhysConnId)
					.parseUINT32(serverPhysConnId)  
					.parseUINT32(clientLogConnId)
					.parseUINT32(serverLogConnId)
					.parseINT32(clientNum);
				PTRACE4("CommRedundantConnectionsObject::processServerMessage - message COMMMCOMPOSITE_CLOSE_MSG");
				if (clientPhysConnId == uniqueClientPhysConnId &&
					serverPhysConnId == uniqueServerPhysConnId &&
					clientLogConnId == conn->clnId &&
					serverLogConnId == conn->srvId &&
					clientNum == conn->clnNum)
				{
					// yes, it is correct channel
					PTRACE4("CommRedundantConnectionsObject::processServerMessage -  COMMMCOMPOSITE_CLOSE_MSG - close ");
					conn->clnId = 0; // don't need to send CLOSE message to this connection (see reinitObject())
					closePhysicalConnection();
				}
				else
				{
					PTRACE4("CommRedundantConnectionsObject::processServerMessage -  COMMMCOMPOSITE_CLOSE_MSG - wrong channel");

				}
				break;
			}
		case COMMMCOMPOSITE_DATA_MSG:
			{

				UINT32 counter;
				UINT32 size = body._size() + 5;
				PASSERT3(!outgoingConnection);
				CommMsgParser parser(body);
				parser.parseUINT32(counter);
				PTRACE5("CommRedundantConnectionsObject::processServerMessage currentMsgReceived=%d, counter=%d",currentMsgReceived,counter);
				if (physConnectionExist && physConnectionEstablished)
				{
					if (currentMsgReceived == counter)
					{
						// this is first message we are expecting
						PTRACE5("CommRedundantConnectionsObject::processServerMessage post to phys connection, currentMsgReceived = %d",currentMsgReceived);
						while(!parser.parseEnded())
						{
							UINT32 msgSize;
							_CommMsg msg;
							parser.parseBYTE(msg.priority)
								.parseUINT16(msg.flags)
								.parseBYTE(msg.type)
								.parseUINT32(msgSize);
							BYTE * data = msg.body._append(msgSize);
							const BYTE *dataRead;
							parser._parseFixedBlock(dataRead,msgSize);
							memcpy(data,dataRead,msgSize);
							postToPhysConnection(msg,0,0);
						}
						currentMsgReceived++;
						checkPhysQueue();
					}
					else
					{
						int del = counter - currentMsgReceived;
						PTRACE5("CommRedundantConnectionsObject::processServerMessage counter=%d, currentMsgReceived=%d",counter,currentMsgReceived);
						if (del > 0)
						{
							bool toInsert=true;
							InPhysQueueListIterator it;
							it = physQueue.begin();
							while(it != physQueue.end())
							{
								struct _RedundantIncomingItem & item = (*it);
								int tmp = item.orderNum - counter;
								if (!tmp)
								{
									//Already in queue
									toInsert=false;
									break;
								}
								else if (tmp>0)
								{
									break;
								}
								++it;
							}
							if (toInsert)
							{
								_RedundantIncomingItem & itnew = *physQueue.insert(it, _RedundantIncomingItem());
								itnew.orderNum = counter;
								itnew.body.moveFrom((CommMsgBody &) body);
							}
						}
						else
						{
							PTRACE5("CommRedundantConnectionsObject::processServerMessage - message already processed, ignored");
						}
					}
					CommMsgBody reply;
					reply.composeUINT32(counter)
						.composeUINT32(size);
					conn->post(COMMMCOMPOSITE_DATA_ACK_MSG,reply);
					conn->lastReceived = counter;

				}
				else
				{
					PTRACE3("CommRedundantConnectionsObject::processServerMessage - phys connection doesn't exist, ignore");			
				}

			}
			break;
		case COMMMCOMPOSITE_DATA_ACK_MSG:
			{

				if (physConnectionExist && physConnectionEstablished)
				{
					UINT32 counter;
					UINT32 size;
					CommMsgParser parser(body);
					parser.parseUINT32(counter)
						.parseUINT32(size);
					conn->lastAck = counter;
					conn->outstandingNumber--;
					bool upTreshold = perConnectionThreshold && perConnectionThreshold < conn->outstandingSize;
					conn->outstandingSize -= size;
					if (clearQueue(counter))
					{
						postQueue(conn,upTreshold);
					}
				}
			}
			break;

		default:
			PLog(" CommRedundantConnectionsObject::processServerMessage: Unknown msgId =%d",msgId);

		}
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processServerMessage ( %02X ): %s", msgId, err.why() );
		closePhysicalConnection();
	}
	catch(  exception& exc  )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processServerMessage ( %02X ): STD exception catch: %s", msgId, exc.what() );
		closePhysicalConnection();
	}
	catch( ... )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processServerMessage ( %02X ): ... catched", msgId );
		closePhysicalConnection();
	}



}

void CommRedundantConnectionsObject::processClientMessage( UINT32 msgId, const CommMsgBody& body, CommRedundantClientConnection* conn  )
{
	try
	{
		switch(msgId)
		{
		case COMMMCOMPOSITE_INIT_REPLY_MSG:
			{
				PASSERT3(outgoingConnection);
				CommMsgParser parser(body);
				replyOnInitClientConnection(parser,conn);
				break;

			}
		case COMMMCOMPOSITE_CLOSE_MSG:
			{
				// Server ask us to close everything
				CommMsgParser parser(body);
				UINT32 clientPhysConnId;
				UINT32 clientLogConnId;
				UINT32 serverPhysConnId;
				UINT32 serverLogConnId;

				INT32  clientNum;
				parser.parseUINT32(clientPhysConnId)
					.parseUINT32(serverPhysConnId)  
					.parseUINT32(clientLogConnId)
					.parseUINT32(serverLogConnId)
					.parseINT32(clientNum);
				PTRACE4("CommRedundantConnectionsObject::processClientMessage - message COMMMCOMPOSITE_CLOSE_MSG");
				if (clientPhysConnId == uniqueClientPhysConnId &&
					serverPhysConnId == uniqueServerPhysConnId &&
					clientLogConnId == conn->clnId &&
					serverLogConnId == conn->srvId &&
					clientNum == conn->clnNum)
				{
					// yes, it is correct channel
					conn->srvId = 0; //don't need to send CLOSE to this connection
					closePhysicalConnection();
				}
				break;
			}

		case COMMMCOMPOSITE_ERROR_MSG:
			{
				UINT32 clientPhysConnId;
				UINT32 clientLogConnId;
				UINT32 errCode;
				CommMsgParser parser(body);
				parser.parseUINT32(clientPhysConnId)
					.parseUINT32(clientLogConnId)
					.parseUINT32(errCode);
				PTRACE3("CommRedundantConnectionsObject::processClientMessage error code=%d clientPhysConnId=%08X, clientLogConnId=%08X",errCode, clientPhysConnId, clientLogConnId);
				if (clientLogConnId == conn->clnId &&
					clientPhysConnId == uniqueClientPhysConnId)
				{
					conn->srvId = 0; //don't need to send CLOSE message
					closePhysicalConnection();
				}
				else
				{
					PTRACE3("CommRedundantConnectionsObject::processClientMessage error - wrong message clientPhysConnId=%08X, clientLogConnId=%08X", uniqueClientPhysConnId, conn->clnId);
					cliPool.disconnect(*conn);
					conn->reinit();
					cliPool.connect( *conn, conn->serverAddress,conn->serverObject, "redundant" );
				}
				break;
			}

		case COMMMCOMPOSITE_DATA_MSG:
			{

				UINT32 counter;
				UINT32 size = body._size() + 5;
				PASSERT3(outgoingConnection);
				CommMsgParser parser(body);
				parser.parseUINT32(counter);
				PTRACE5("CommRedundantConnectionsObject::processClientMessage currentMsgReceived=%d, counter=%d",currentMsgReceived,counter);
				if (physConnectionExist && physConnectionEstablished)
				{
					if (currentMsgReceived == counter)
					{
						// this is the first message we are expecting
						PTRACE5("CommRedundantConnectionsObject::processClientMessage post to phys connection");
						while(!parser.parseEnded())
						{
							UINT32 msgSize;
							_CommMsg msg;
							parser.parseBYTE(msg.priority)
								.parseUINT16(msg.flags)
								.parseBYTE(msg.type)
								.parseUINT32(msgSize);
							BYTE * data = msg.body._append(msgSize);
							const BYTE *dataRead;
							parser._parseFixedBlock(dataRead,msgSize);
							memcpy(data,dataRead,msgSize);
							postToPhysConnection(msg,0,0);
						}
						currentMsgReceived++;
						checkPhysQueue();
					}
					else
					{
						int del = counter - currentMsgReceived;
						if (del > 0)
						{
							bool toInsert=true;
							InPhysQueueListIterator it;
							it = physQueue.begin();
							while(it != physQueue.end())
							{
								struct _RedundantIncomingItem & item = (*it);
								int tmp = item.orderNum - counter;
								if (!tmp)
								{
									//Already in queue
									toInsert=false;
									break;
								}
								else if (tmp>0)
								{
									break;
								}
								++it;
							}
							if (toInsert)
							{
								_RedundantIncomingItem & itnew = *physQueue.insert(it, _RedundantIncomingItem());
								itnew.orderNum = counter;
								itnew.body.moveFrom((CommMsgBody &)body);
							}
						}

					}
					CommMsgBody reply;
					reply.composeUINT32(counter)
						.composeUINT32(size);
					conn->post(COMMMCOMPOSITE_DATA_ACK_MSG,reply);
					conn->lastReceived = counter;

				}
				else
				{
					PTRACE3("CommRedundantConnectionsObject::processClientMessage - phys connection doesn't exist, ignore");			
				}

			}
			break;
		case COMMMCOMPOSITE_DATA_ACK_MSG:
			{
				if (physConnectionExist && physConnectionEstablished)
				{
					UINT32 counter;
					UINT32 size;
					CommMsgParser parser(body);
					parser.parseUINT32(counter)
						.parseUINT32(size);
					conn->lastAck = counter;
					conn->outstandingNumber--;
					bool upTreshold = perConnectionThreshold && perConnectionThreshold < conn->outstandingSize;
					conn->outstandingSize -= size;
					if (clearQueue(counter))
					{
						postQueue(conn,upTreshold);
					}
				}
			}
			break;


		default:
			PLog(" CommRedundantConnectionsObject::processClientMessage: Unknown msgId =%d",msgId);

		}
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processClientMessage ( %02X ): %s", msgId, err.why() );
		closePhysicalConnection();
	}
	catch(  exception& exc  )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processClientMessage ( %02X ): STD exception catch: %s", msgId, exc.what() );
		closePhysicalConnection();
	}
	catch( ... )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processClientMessage ( %02X ): ... catched", msgId );
		closePhysicalConnection();
	}



}

void CommRedundantConnectionsObject::reinitObject(CommRedundantServerConnection * excludedServerConnection)
{
	PTRACE3("CommRedundantConnectionsObject::reinitObject, exclude connection %08X",(excludedServerConnection?excludedServerConnection->id():0));
	outQueue.clear();
	physQueue.clear();
	queueSize=0;
	currentMsgReceived=0;
	currentMsgSend=0;
	currentPhysConnMsg=0;
	totalSize = 0;
	if (outgoingConnection)
	{
		// client connections;
		for (int i=0;i<clientConnections.size();i++)
		{
			if (clientConnections[i]->srvId)
			{
				clientConnections[i]->postCloseConnection();
			}
			if (clientConnections[i]->isConnected())
			{
				cliPool.disconnect(*clientConnections[i]);
			}
			clientConnections[i]->reinit();
		}
		uniqueClientPhysConnId = nextUniqueId();
		uniqueServerPhysConnId = 0;
	}
	else
	{
		CommServerConnectionPool::ConnectionIterator it;
		CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(it); // safe because there are no other types of connection by design
		while (srvConn)
		{
			if (excludedServerConnection != srvConn)
			{
				if (srvConn->clnId)
				{
					srvConn->postCloseConnection();
				}
				if (srvConn->isConnected())
				{
					srvPool.disconnect(srvConn);
				}
				srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(it); // safe because there are no other types of connection by design
			}
			else
			{
				srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(it); // safe because there are no other types of connection by design
			}
		}
		uniqueClientPhysConnId = 0;
		uniqueServerPhysConnId = nextUniqueId();
	}
}



void CommRedundantConnectionsObject::processOutgoingPhysConnection (const PString & address)
{
	PTRACE3("CommRedundantConnectionsObject::processOutgoingPhysConnection -  %s",address.c_str());
	if (!outgoingConnection)
	{
		PLog("Server object supports only incoming phys connections. rejected");
		disconnectPhysConnection();
		return;
	}
	physConnectionExist = true;
	reinitObject();
	for (int i=0;i<clientConnections.size();i++)
	{
		cliPool.connect( *clientConnections[i], clientConnections[i]->serverAddress,clientConnections[i]->serverObject, "redundant" );
	}

	CommMsgBody dummy;
	postTimerMessage(TIMER_CLIENT_CONNECT, dummy, connectTimeOut);
}


void CommRedundantConnectionsObject::processPhysDisconnect () 
{
	PTRACE3("CommRedundantConnectionsObject::processPhysDisconnect");
	physConnectionEstablished = false;
	physConnectionExist = false;
	reinitObject();
}

void CommRedundantConnectionsObject::processPhysMessage (_CommMsg & msg) 
{
	try
	{
		PTRACE5("CommRedundantConnectionsObject::processPhysMessage msgId=%d",msg.type);
		if (!physConnectionExist || !physConnectionEstablished)
		{
			PTRACE4("CommRedundantConnectionsObject::processPhysMessage - physConnection exist=%s , established=%s, msg.type=%02X dropped", (physConnectionExist?"true":"false"),(physConnectionEstablished?"true":"false"),msg.type);
			return;
		}
		if (globalThreshold && globalThreshold < totalSize)
		{
			PTRACE3("CommRedundantConnectionsObject::processPhysMessage - total size of unconfirmed queue %d exceeded theshold %d, close phys connection", totalSize, globalThreshold);
			closePhysicalConnection();
			return;
		}

		OutQueueListIterator iter = outQueue.insert(outQueue.end(), _RedundantItem());
		queueSize++;
		_RedundantItem & it = *iter;

		it.orderNum = currentPhysConnMsg;
		it.connectionMask = 0;
		it.ticksCreated = msg.internal.patch1;
		it.body.moveFrom(msg.body);

		// When we push message to the queue on commtmcomposite thread manager we
		// prepare space for msg id, format and counter. Now we can set values. 
		BYTE * data = it.body._writePtr();
		CommMsgBody::writeUINT32( data, COMMMCOMPOSITE_DATA_MSG );
		data += 4;
		*data++ = CommMsgBody::_NoFormatChar;
		CommMsgBody::writeUINT32( data, currentPhysConnMsg );
		currentPhysConnMsg++;
		UINT32 size = it.body._size();
		totalSize += size;

		if (outgoingConnection)
		{
			for (int i=0;i<clientConnections.size();i++)
			{
				if (clientConnections[i]->srvId)
				{
					if (clientConnections[i]->lastIteratorValid)
					{
						if (clientConnections[i]->itr == outQueue.end())
							clientConnections[i]->itr = iter;
					}
					else
					{
						clientConnections[i]->itr = iter;
						clientConnections[i]->lastIteratorValid=true;
					}
				}
			}

			if (flowType == nonRedundant)
			{
				// special case, sent message to one connection on only
				CommRedundantClientConnection * conn = 0;
				UINT32 tmpSize = 0xffffffff;
				for (int i=0;i<clientConnections.size();i++)
				{
					if (clientConnections[i]->srvId)
					{
						if (!perConnectionThreshold || perConnectionThreshold >= clientConnections[i]->outstandingSize)
						{
							if (tmpSize > clientConnections[i]->outstandingSize)
							{
								tmpSize = clientConnections[i]->outstandingSize;
								conn = clientConnections[i];
							}
						}
					}
				}
				if (conn)
				{
					postQueue(conn,false);
				}
			}
			else
			{
				for (int i=0;i<clientConnections.size();i++)
				{
					if (clientConnections[i]->srvId)
					{
						if (!perConnectionThreshold || perConnectionThreshold >= clientConnections[i]->outstandingSize)
						{
							postQueue(clientConnections[i],false);
						}
					}
				}
			}
		}
		else
		{
			CommServerConnectionPool::ConnectionIterator its;
			CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(its); // safe because there are no other types of connection by design
			while (srvConn)
			{
				if (srvConn->clnId)
				{
					if (srvConn->lastIteratorValid)
					{
						if (srvConn->itr == outQueue.end())
							srvConn->itr = iter;
					}
					else
					{
						srvConn->itr = iter;
						srvConn->lastIteratorValid=true;
					}
				}
				srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(its); // safe because there are no other types of connection by design
			}
			srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(its); // safe because there are no other types of connection by design
			if (flowType == nonRedundant)
			{
				// special case, sent message to one connection on only
				CommRedundantServerConnection * conn = 0;
				UINT32 tmpSize = 0xffffffff;

				while (srvConn)
				{
					if (srvConn->clnId)
					{
						if (!perConnectionThreshold || perConnectionThreshold >= srvConn->outstandingSize)
						{
							if (tmpSize > srvConn->outstandingSize)
							{
								tmpSize = srvConn->outstandingSize;
								conn = srvConn;
							}
						}
					}
					srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(its); // safe because there are no other types of connection by design
				}
				if (conn)
				{
					postQueue(conn,false);
				}
			}
			else
			{
				while (srvConn)
				{
					if (srvConn->clnId)
					{
						if (!perConnectionThreshold || perConnectionThreshold >= srvConn->outstandingSize)
						{
							postQueue(srvConn,false);						
						}

					}
					srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(its); // safe because there are no other types of connection by design
				}
			}

		}
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processPhysMessage ( %02X ): %s", msg.type, err.why() );
		closePhysicalConnection();
	}
	catch(  exception& exc  )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processPhysMessage ( %02X ): STD exception catch: %s", msg.type, exc.what() );
		closePhysicalConnection();
	}
	catch( ... )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processPhysMessage ( %02X ): ... catched", msg.type );
		closePhysicalConnection();
	}



}

void CommRedundantConnectionsObject::connectedClient(CommRedundantClientConnection * conn)
{
	PTRACE3("CommRedundantConnectionsObject::connectedClient, num=%d, id=%X",conn->clnNum,conn->id());
	CommMsgBody body;
	body.composeUINT32(uniqueClientPhysConnId)
		.composeUINT32(uniqueServerPhysConnId)
		.composeUINT32(conn->clnId)
		.composeINT32(conn->clnNum);
	conn->post(COMMMCOMPOSITE_INIT_MSG,body);
}

void CommRedundantConnectionsObject::disconnectedClient(CommRedundantClientConnection * conn)
{
	PTRACE3("CommRedundantConnectionsObject::disconnectedClient, num = %d, id=%X",conn->clnNum,conn->id());
	OutQueueListIterator it = outQueue.end();
	if (conn->srvId)
	{
		it = cleanMask(conn->clnNum);
	}
	conn->reinit();
	if (it != outQueue.end())
		setNewIterator(it);
	if ( !isAnyClientConnectionsExist() )
	{
		PTRACE3("CommRedundantConnectionsObject::disconnectedClient, no more client connections, sending timer");
		CommMsgBody dummy;
		postTimerMessage(TIMER_CHECK_CLIENT_CONNECTIONS, dummy, timeOut);
	}
	PTRACE3("CommRedundantConnectionsObject::disconnectedClient, 4");
}

bool CommRedundantConnectionsObject::isAnyClientConnectionsExist()
{

	if (outgoingConnection)
	{
		for (int i=0;i<clientConnections.size();i++)
		{
			if (clientConnections[i]->srvId)
			{
				return true;
			}

		}
	}
	return false;
}

bool CommRedundantConnectionsObject::isAnyServerConnectionsExist()
{

	if (!outgoingConnection)
	{
		CommServerConnectionPool::ConnectionIterator it;
		CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(it); // safe because there are no other types of connection by design
		while (srvConn)
		{
			if (srvConn->clnId)
			{
				return true;
			}
			srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(it); // safe because there are no other types of connection by design
		}
	}
	return false;
}


void CommRedundantConnectionsObject::processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body )
{
	try
	{
		switch(msgId)
		{
		case TIMER_PRINT_STAT:
			{
				printStatistic();
				CommMsgBody dummy;
				postTimerMessage(TIMER_PRINT_STAT, dummy, printStatTime);
			}
			break;

		case TIMER_CHECK_CLIENT_CONNECTIONS:
			{
				PTRACE3("CommRedundantConnectionsObject::processTimerMessage (TIMER_CHECK_CLIENT_CONNECTIONS)");
				if ( !isAnyClientConnectionsExist() )
				{
					PTRACE3("CommRedundantConnectionsObject::processTimerMessage , client connections not found, closing phys connection");
					closePhysicalConnection();
				}
			}
			break;
		case TIMER_CHECK_SERVER_CONNECTIONS:
			{
				PTRACE3("CommRedundantConnectionsObject::processTimerMessage (TIMER_CHECK_SERVER_CONNECTIONS)");
				if ( !isAnyServerConnectionsExist() )
				{
					PTRACE3("CommRedundantConnectionsObject::processTimerMessage , server connections not found, closing phys connection");
					closePhysicalConnection();
				}
			}
			break;
		case TIMER_CLIENT_CONNECT:
			{
				PTRACE3("CommRedundantConnectionsObject::processTimerMessage (TIMER_CLIENT_CONNECT)");
				PASSERT3(outgoingConnection);
				if ( !physConnectionEstablished )
				{
					PTRACE3("CommRedundantConnectionsObject::processTimerMessage , connection timeout, closing phys connection");
					closePhysicalConnection();
				}
			}
			break;

		default:
			PTRACE3("CommRedundantConnectionsObject::processTimerMessage - unknown msgId=%d",msgId);
			break;

		}
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processTimerMessage ( %02X ): %s", msgId, err.why() );
		closePhysicalConnection();
	}
	catch(  exception& exc  )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processTimerMessage ( %02X ): STD exception catch: %s", msgId, exc.what() );
		closePhysicalConnection();
	}
	catch( ... )
	{
		PLog( "PError caught/ignored in CommRedundantConnectionsObject::processTimerMessage ( %02X ): ... catched", msgId );
		closePhysicalConnection();
	}

}

void CommRedundantConnectionsObject::connectedServer(CommRedundantServerConnection* conn)
{
	PTRACE3("CommRedundantConnectionsObject::connectedServer, id=%X",conn->id());
}


void CommRedundantConnectionsObject::disconnectedServer(CommRedundantServerConnection* conn)
{
	PTRACE3("CommRedundantConnectionsObject::disconnectedServer, id=%X",conn->id());
	OutQueueListIterator it = outQueue.end();
	if (conn->clnId)
	{
		it = cleanMask(conn->clnNum);
	}
	conn->clnId = 0;;
	if (it != outQueue.end())
		setNewIterator(it);

	if (!isAnyServerConnectionsExist() )
	{
		PTRACE3("CommRedundantConnectionsObject::disconnectedServer, no more server connections, sending timer");
		CommMsgBody dummy;
		postTimerMessage(TIMER_CHECK_SERVER_CONNECTIONS, dummy, timeOut);
	}
}

void CommRedundantConnectionsObject::printStatistic()
{

	PLog("RDSTAT: PhysConn %s, %s, sent=%u, phys=%u, rec=%u",
		(physConnectionExist?"exists":"not exists"),
		(physConnectionEstablished?"established":"not established"),
		currentMsgSend,
		currentPhysConnMsg,
		currentMsgReceived);

	OutQueueListIterator it = outQueue.begin();
	if (it != outQueue.end())
	{
		OutgoingQueueList::reverse_iterator rit=outQueue.rbegin();
		PLog("RDSTAT: Queue size=%u, data size=%u, queue head=%u, tail=%u",
			queueSize,
			totalSize,
			(*it).orderNum,
			(*rit).orderNum);

	}
	else
	{
		PLog("RDSTAT: Queue size=%u, data size=%u, queue empty",
			queueSize,
			totalSize);
	}
	PLog("RDSTAT: Number=%u, Live time: max=%u, avg=%u, Round trip: max=%u, avg=%u",
		stat_numMessages,
		stat_maxLive,
		(stat_numMessages ? (stat_averageLive/stat_numMessages) : 0),
		stat_maxRoundTrip,
		(stat_numMessages? (stat_averageRoundTrip/stat_numMessages) : 0));
	PString out;
	out.append("RDSTAT: ");
	for (int i=0; i<sizeof(stat_distribution)/sizeof(stat_distribution[0]);i++)
	{
		out.appendInt(i).append("=").appendUint(stat_distribution[i]).append(",");
	}
	PLog("%s", out.c_str());
	stat_maxRoundTrip = 0;
	stat_maxLive = 0;
	stat_averageRoundTrip = 0;
	stat_averageLive = 0;
	stat_numMessages = 0;
	memset(stat_distribution,0,sizeof(stat_distribution));


	if (outgoingConnection)
	{
		for (int i=0;i<clientConnections.size();i++)
		{
			PString out;
			out.append("RDSTAT: ").appendInt(clientConnections[i]->clnNum).append(":");
			if (clientConnections[i]->srvId)
			{
				out.append("id=").appendHexInt(clientConnections[i]->id());
				out.append(",lastSent=").appendUint(clientConnections[i]->lastSent);
				out.append(",lastReceived=").appendUint(clientConnections[i]->lastReceived);
				out.append(",lastAck=").appendUint(clientConnections[i]->lastAck);
				out.append(",outstandingNumber=").appendUint(clientConnections[i]->outstandingNumber);
				out.append(",outstandingSize=").appendUint(clientConnections[i]->outstandingSize);

				if (clientConnections[i]->lastIteratorValid && clientConnections[i]->itr != outQueue.end())
				{
					out.append(",Itr=").appendUint((*clientConnections[i]->itr).orderNum);
				}
				PLog("%s", out.c_str());
			}
			else
				PLog("%s", out.c_str());

		}
	}
	else
	{
		CommServerConnectionPool::ConnectionIterator its;
		CommRedundantServerConnection *srvConn = (CommRedundantServerConnection *)srvPool.firstConnection(its); // safe because there are no other types of connection by design
		while (srvConn)
		{
			if (srvConn->clnId)
			{
				PString out;
				out.append("RDSTAT: ").appendInt(srvConn->clnNum).append(":");
				out.append("id=").appendHexInt(srvConn->id());
				out.append(",lastSent=").appendUint(srvConn->lastSent);
				out.append(",lastReceived=").appendUint(srvConn->lastReceived);
				out.append(",lastAck=").appendUint(srvConn->lastAck);
				out.append(",outstandingNumber=").appendUint(srvConn->outstandingNumber);
				out.append(",outstandingSize=").appendUint(srvConn->outstandingSize);
				if (srvConn->lastIteratorValid && srvConn->itr != outQueue.end())
				{
					out.append(",Itr=").appendUint((*srvConn->itr).orderNum);
				}
				PLog("%s", out.c_str());
			}
			srvConn = (CommRedundantServerConnection *)srvPool.nextConnection(its); // safe because there are no other types of connection by design
		}

	}

}
