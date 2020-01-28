#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commconn.h"
#include "commcli.h"

CommClientGuard::CommClientGuard( const char* name_ )
{
	_name = name_;
}

CommClientConnection::CommClientConnection( CommClientGuardFactory& guardFactory_)
{
	version = versionBase;
	guardFactory = &guardFactory_;
	guard = 0;
	guardAuth = 0;
	pool = 0;
	priority = _COMM_MSGPRIORITY_INITCONNECT;
	qMonitor = false;
	closePending = false;
	_quality=100;
	_qualityRequested=false;
}

CommClientConnectionEx::CommClientConnectionEx( CommClientGuardFactory& guardFactory_) :
CommClientConnection(guardFactory_)
{
	version = versionExtended;
	currentNumber=0;
	receivedNumber=0;
	serverThisId=0;
	inTransDisconnectState=false;
	disconnectStarted = 0;
	confirmedNumber = 0;
	disconnectTimeOut = CONN_EX_DISCONNECT_TIME_OUT;
	pollTime = CONN_EX_POLL_TIME;
	timerHandle = 0;
}

/* virtual */ CommClientConnection::~CommClientConnection()
{
	if( pool )
		PLog( "CommClientConnection::~CommClientConnection(): destroying non-disconnected connection" );
	delete guardAuth;
	delete guard;
}

inline bool CommClientConnection::_createGuard( const char* guardName )
{
	delete guard;
	guard = 0;
	guard = guardFactory->createGuard( guardName, guardAuth );
	return guard != 0;
}

void CommClientConnection::monitorQuality()
{
	PASSERT3( pool );
	qMonitor = true;
	if( state == _CommConnection::_StateConnOk )
		_postQMonitorRequest();
}

void CommClientConnection::_postQMonitorRequest()
{
	PASSERT3( pool );
	_CommMsg msg( _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR, priority );
	msg.body.composeUINT32( peerId ).composeUINT32( thisId );
	pool->_post( msg );
}

void CommClientConnection::post( UINT32 msgId, CommMsgBody& body )
{
	PASSERT3( pool );
	if (body._size() > 512000000 )
	{
		PLog( "CommClientConnection(thisId=%X): Message id=%d,  XLPOST sz=%u MB", thisId, msgId, (unsigned int)(body._size()/1000000));
	}
	if( state != _CommConnection::_StateConnOk )
	{
		PLog( "CommClientConnection: Message id=%d to disconnected logical connection (%s/%s/%s)- ignored",msgId,server.c_str(), serverObject.c_str(),serverConnType.c_str());
		return;
	}

	PASSERT3( guard );
	_CommConnection::_wrapUserMsg( body, msgId );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority );
	if (guard->isSimple())
	{
		size_t sz = body._size();
		BYTE* p = body._createPrefix( 4 * sizeof(UINT32) );
		CommMsgBody::writeUINT32( p, peerId );
		p += 4;
		CommMsgBody::writeUINT32( p, thisId );
		p += 4;
		CommMsgBody::writeUINT32( p, sessionId );
		p += 4;
		CommMsgBody::writeUINT32( p, sz );
		msg.body.moveFrom(body);
	}
	else
	{
		msg.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT32( sessionId );
		guard->processOutBlock( msg.body, body );
	}
	pool->_post( msg );
}

void CommClientConnectionEx::_postWaitingRequests(UINT32 serverReceived)
{
	PASSERT3( pool );
	PTRACE5("CommClientConnectionEx::_postWaitingRequests: thisId=%X, serverReceived=%u",thisId,serverReceived);
	if( state != _CommConnection::_StateConnOk)
	{
		PLog( "CommClientConnectionEx::_postWaitingRequests Message to disconnected logical connection - ignored, thisId=%X",thisId );
		return;
	}
	_List::iterator it = msgList.begin();
	while( it != msgList.end() )
	{
		_Item* item = (*it);
		if ((INT32)(item->currentNumber - serverReceived)<=0)
		{
			PTRACE5("CommClientConnectionEx::_postWaitingRequests: thisId=%X, number=%u confirmed, deleted",thisId,item->currentNumber);
			delete item;
			msgList.erase(it);
			it = msgList.begin();
		}
		else
			break;
	}
	bool sent=false;

	for (it = msgList.begin(); it != msgList.end(); ++it)
	{
		_Item* item = (*it);
		_CommMsg msg( _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority );
		msg.body.copyFrom( item->body );
		BYTE* p = msg.body._writePtr();
		CommMsgBody::writeUINT32( p, peerId );
		p += 4*6; //4 - sizeof(INT32), 6 - position of 'receivedNumber'
		CommMsgBody::writeUINT32( p, receivedNumber );
		pool->_post( msg );
		sent=true;
		confirmedNumber = receivedNumber;
		PTRACE5("CommClientConnectionEx::_postWaitingRequests: thisId=%X, number=%u resent, %u confirmed",thisId,item->currentNumber,confirmedNumber);
	}
	if (sent == false)
	{
		_CommMsg msg( _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority );
		msg.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT32( sessionId )
			.composeUINT32( thisId ).composeUINT32( serverThisId )
			.composeUINT32( currentNumber ).composeUINT32( receivedNumber ).composeBYTE(0);
		pool->_post( msg );
		confirmedNumber = receivedNumber;
		PTRACE5("CommClientConnectionEx::_postWaitingRequests: thisId=%X, nothing to resend, %u confirmed",thisId,confirmedNumber);
	}
}

void CommClientConnectionEx::post( UINT32 msgId, CommMsgBody& body )
{
	PASSERT3( pool );
	if( !( state == _CommConnection::_StateConnOk || inTransDisconnectState))
	{
		PLog( "CommClientConnectionEx: Message to disconnected logical connection - ignored" );
		return;
	}

	PASSERT3( guard );
	_CommConnection::_wrapUserMsg( body, msgId );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority );

	currentNumber++;
	msg.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT32( sessionId )
		.composeUINT32( thisId ).composeUINT32( serverThisId )
		.composeUINT32( currentNumber ).composeUINT32( receivedNumber ).composeBYTE(1);
	PTRACE5("CommClientConnectionEx::post: thisId=%X,curentNumer=%u", thisId,currentNumber);
	guard->processOutBlock( msg.body, body );

	_Item* item = new _Item();
	_List::iterator it = msgList.insert( msgList.end(), item );
	item->body.copyFrom( msg.body );
	item->currentNumber = currentNumber;
	if (!inTransDisconnectState)
	{
		pool->_post( msg );
		confirmedNumber = receivedNumber;
		PTRACE5("CommClientConnectionEx::post: thisId=%X,confirmedNumber=%u", thisId,confirmedNumber);
	}
	else
	{
		PTRACE5("CommClientConnectionEx::post: in TransDisconnect state, thisId=%X",thisId);
	}
}

bool CommClientConnectionEx::setPollTime(INT32 pollTime_)
{
	if (isConnected()) return false;
	pollTime = pollTime_;
	return true;
}

bool CommClientConnectionEx::processTimer()
{
	timerHandle = 0;
	PASSERT3( pool );

	if (inTransDisconnectState)
	{
		PTRACE5("CommClientConnectionEx::processTimer - timeout, close connection,thisId=%X",thisId);
		_clearAll();
		return false;
	}

	if (pollTime > 0)
	{
		if (confirmedNumber != receivedNumber)
		{
			_CommMsg msg(_COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority);
			msg.body.composeUINT32(peerId).composeUINT32(thisId).composeUINT32(sessionId)
				.composeUINT32(thisId).composeUINT32(serverThisId)
				.composeUINT32(currentNumber).composeUINT32(receivedNumber).composeBYTE(0);
			pool->_post(msg);
			confirmedNumber = receivedNumber;
			PTRACE5("CommClientConnectionEx::processTimer: sent confirmation, thisId=%X,confirmedNumber=%u", thisId, confirmedNumber);
		}
		_CommMsg timerMsg(_COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX, 0);
		timerMsg.body.composeUINT32(thisId);
		timerHandle = pool->inter->_postClientTimer(timerMsg, pollTime);
	}
	return true;
}

CommClientConnectionEx::~CommClientConnectionEx()
{
	CommClientConnectionEx::_clearAll();
}

void CommClientConnectionEx::_clearAll()
{
	PTRACE5("CommClientConnectionEx::_clearAll called, thisId=%X",thisId);
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = (*it);
		delete item;
	}
	msgList.clear();
	currentNumber=0;
	receivedNumber=0;
	serverThisId=0;
	disconnectStarted = 0;
	confirmedNumber = 0;
	inTransDisconnectState=false;
}

void CommClientConnectionEx::_processErrors(UINT16 errCode,const char * errMsg)
{
	pool->_postClientError( *this, errCode, errMsg );
	state = _CommConnection::_StateConnClientFatalError;
	PLog( "CommClientConnectionEx: _processUserMessageError: thisId=%X, %02X, %s", thisId,errCode, errMsg );
	_clearAll();
	bool ack = _safeClosed( errCode, errMsg );
	if( !ack )
		pool->_scheduleReconnect( *this );
}

void CommClientConnectionEx::_processUserMessage( CommMsgParser & parser)
{
	PASSERT3( guard );
	PASSERT3( pool );
	UINT32 serverThisId_;
	UINT32 clientThisId_;
	UINT32 receivedServerNumber_;
	UINT32 receivedClientNumber_;
	BYTE flags;
	parser.parseUINT32(serverThisId_).parseUINT32(clientThisId_).parseUINT32(receivedServerNumber_).parseUINT32(receivedClientNumber_).parseBYTE(flags);

	if  ( (serverThisId_ != serverThisId) || 
		(clientThisId_ != thisId) ||
		( (INT32)(receivedServerNumber_ - receivedNumber) > 1) || 
		( (INT32)(receivedClientNumber_ - currentNumber) > 0) 
		)
	{
		PTRACE3("CommClientConnectionEx::_processUserMessage: thisId=%X, receivedClientId=%X,"
			"serverId=%X, receivedServerId=%X, currentNumber=%u, receivedNumber=%u, "
			"receivedServerNumber_=%u,receivedClientNumber_=%u",
			thisId,clientThisId_, serverThisId,serverThisId_,currentNumber,
			receivedNumber,receivedServerNumber_,receivedClientNumber_);
		_processErrors(_COMM_ERROR_INCORRECT_PARAMETERS,"Incorrect parameters of extended protocol");
		return;
	}
	PTRACE5("CommClientConnectionEx::_processUserMessage: thisId=%X, receivedClientId=%X,"
		"serverId=%X, receivedServerId=%X, currentNumber=%u, receivedNumber=%u, "
		"receivedServerNumber_=%u,receivedClientNumber_=%u",
		thisId,clientThisId_, serverThisId,serverThisId_,currentNumber,
		receivedNumber,receivedServerNumber_,receivedClientNumber_);

	_List::iterator it = msgList.begin();
	while( it != msgList.end() )
	{
		_Item* item = (*it);
		if ((INT32)(item->currentNumber - receivedClientNumber_)<=0)
		{
			PTRACE5("CommClientConnectionEx::_processUserMessage: thisId=%X, message number=%u confirmed, deleted",thisId,item->currentNumber);
			delete item;
			msgList.erase(it);
			it = msgList.begin();
		}
		else
			break;
	}

	if (!flags)
		return;
	if ((receivedServerNumber_ - receivedNumber) != 1)
	{
		// Already processed, ignore
		PTRACE5("CommClientConnectionEx::_processUserMessage: thisId=%X, message already processed",thisId);
		return;
	}
	receivedNumber = receivedServerNumber_;
	CommMsgBody body;
	guard->processInBlock( body, parser );
	UINT32 msgId;
	_CommConnection::_unwrapUserMsg( msgId, body );
	_safeProcessMessage( msgId, body );
}

/* virtual */ bool CommClientConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	if( errCode == _COMM_ERROR_NOLOCALOBJECT || errCode == _COMM_ERROR_INBOUNDCONNCLOSED || errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED || errCode == _COMM_ERROR_NOCONNTYPE)
		return false;
	return true;
}

void CommClientConnection::_safeProcessMessage( UINT32 msgId,  const CommMsgBody& body )
{
	try
	{
		PSIGRETURN()
		processMessage( msgId, body );
	}
	catch( PError& err )
	{
		PLog( "CommClientConnection: exception in processMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommClientConnection: STD exception in processMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, exc.what() );
	}

	catch( ... )
	{
		PLog( "CommClientConnection: unknown exception in processMessage (msgId=%d=0x%X) caught", (int)msgId, (int)msgId );
	}
}

void CommClientConnection::_safeConnected(CommMsgBody * infoBody)
{
	try
	{
		PSIGRETURN()
		connectedWithInfo(infoBody);
	}
	catch( PError& err )
	{
		PLog( "CommClientConnection: exception in connected() caught: %s", err.why() );
	}
	catch( const exception& exc  )
	{
		PLog( "CommClientConnection: STD exception caught in connected(): '%s'", exc.what() );
	}

	catch( ... )
	{
		PLog( "CommClientConnection: unknown exception in connected() caught" );
	}
}

void CommClientConnection::_safeQualityUpdated( int quality )
{
	try
	{
		PSIGRETURN()
			qualityUpdated( quality );
	}
	catch( PError& err )
	{
		PLog( "CommClientConnection: exception in qualityUpdated() caught: %s", err.why() );
	}
	catch( const exception& exc  )
	{
		PLog( "CommClientConnection: STD exception caught in qualityUpdated(): '%s'", exc.what() );
	}
	catch( ... )
	{
		PLog( "CommClientConnection: unknown exception in qualityUpdated() caught" );
	}
}


void CommClientConnection::processQualityRequest()
{
	_postQualityUpdate(_quality);
}
void CommClientConnection::postQualityUpdate(BYTE quality_)
{
	PASSERT3( pool );
	if (quality_ > 100)
	{
		if (_qualityRequested)
			_postQualityUpdate(quality_);
	}
	else
	{
		if (_quality != quality_)
		{
			_quality = quality_;
			if (_qualityRequested)
				_postQualityUpdate(_quality);
		}
	}
}
void CommClientConnection::_postQualityUpdate(BYTE quality_)
{
	PASSERT3( pool );
	if ( state == _CommConnection::_StateConnOk )
	{
		_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE, priority );
		m.body.composeUINT32( peerId ).composeUINT32( thisId ).composeBYTE( quality_ );
		pool->_post( m );
	}
}
void CommClientConnection::_safeProcessQualityRequest()
{
	try
	{
		PSIGRETURN()
	    if (_qualityRequested)
		{
			_postQualityUpdate(_quality);
		}
		else
		{
			_qualityRequested=true;
			_postQualityUpdate( _TRANSPORT_KEEPALIVE_CONNECT );
			processQualityRequest();
		}
	}
	catch( PError& err )
	{
		PLog( "CommClientConnection: exception in processQualityRequest() caught: %s", err.why() );
	}
	catch( const exception& exc  )
	{
		PLog( "CommClientConnection: STD exception caught in processQualityRequest(): '%s'", exc.what() );
	}
	catch( ... )
	{
		PLog( "CommClientConnection: unknown exception in processQualityRequest() caught" );
	}
}
void CommClientConnection::processKeepAliveRequest()
{
	_postKeepAlive();
}
void CommClientConnection::postKeepAlive()
{
	PASSERT3( pool );
	_postKeepAlive();
}
void CommClientConnection::_postKeepAlive()
{
	PASSERT3( pool );
	if( state == _CommConnection::_StateConnOk )
	{
		_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE, priority );
		m.body.composeUINT32( peerId ).composeUINT32( thisId );
		pool->_post( m );
	}
}
void CommClientConnection::_safeProcessKeepAliveRequest()
{
	try
	{
		PSIGRETURN()
			processKeepAliveRequest();
	}
	catch( PError& err )
	{
		PLog( "CommClientConnection: exception in processKeepAliveRequest() caught: %s", err.why() );
	}
	catch( const exception& exc  )
	{
		PLog( "CommClientConnection: STD exception caught in processKeepAliveRequest(): '%s'", exc.what() );
	}
	catch( ... )
	{
		PLog( "CommClientConnection: unknown exception in processKeepAliveRequest() caught" );
	}
}


bool CommClientConnection::_safeClosed( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
{
	try
	{
		PSIGRETURN()
		return closedOrDisconnected(errCode,errMsg,errMsgBody );
	}
	catch( PError& err )
	{
		PLog( "CommClientConnection: exception in closed() caught: %s", err.why() );
		return true;
	}
	catch( const exception& exc  )
	{
		PLog( "CommClientConnection: STD exception caught in closed(): '%s'", exc.what() );
		return true;
	}
	catch( ... )
	{
		PLog( "CommClientConnection: unknown exception in closed() caught" );
		return true;
	}
}

//*****************************************************************************

CommClientConnectionPool::CommClientConnectionPool( _CommInterface& inter_ )
{
	inter = &inter_;
	connected = false;
}

CommClientConnection* CommClientConnectionPool::firstConnection( ConnectionIterator& i )
{
	CommClientConnection** connPtr = connMap.firstIndex( i.i );
	return connPtr ? *connPtr : 0;
}

CommClientConnection* CommClientConnectionPool::nextConnection( ConnectionIterator& i )
{
	CommClientConnection** connPtr = connMap.nextIndex( i.i );
	return connPtr ? *connPtr : 0;
}

CommClientConnection* CommClientConnectionPool::findConnection( UINT32 connId )
{
	CommClientConnection** connPtr = connMap.find( connId );
	if ( connPtr == 0 )
		return 0;
	CommClientConnection* conn = *connPtr;
	PASSERT3( conn->thisId == connId );
	return conn;
}

CommClientConnectionPool::~CommClientConnectionPool()
{
	//+++ disconnect
}

CommClientConnection* CommClientConnectionPool::_getConn( UINT32 thisId, BYTE msgType )
{
	CommClientConnection** connPtr = connMap.find( thisId );
	if( connPtr == 0 )
	{
		PLog( "CommClientConnectionPool: msg type=%02X for unknown connId=%08X - ignored", msgType, thisId );
		return 0;
	}
	CommClientConnection* conn = *connPtr;
#ifndef COMM_CLIENT_ONLY
	if (conn->thisId != thisId)
	{
		PLog("CommClientConnectionPool::_getConn(%08X,%02X), thisId != conn->thisId(%08X). Connection %s/%s/%s, peerId=%08X, state=%d, priority=%d, sessionId=%08X",
			thisId,msgType,conn->thisId,conn->server.c_str(),conn->serverObject.c_str(),conn->serverConnType.c_str(),conn->peerId,conn->state,conn->priority,conn->sessionId);
	}
#endif
	PASSERT3( conn->thisId == thisId );
	return conn;
}

CommClientConnection* CommClientConnectionPool::_getConn( UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	CommClientConnection* conn = _getConn( thisId, msgType );
	if( conn == 0 )
		return 0;
	if( peerId != conn->peerId )
	{
		PLog( "CommClientConnectionPool: msg type=%02X for connId=%08X with msg.peerId=%08X != conn.peerId=%08X - ignored", msgType, thisId, peerId, conn->peerId );
		return 0;
	}
	return conn;
}

CommClientConnection* CommClientConnectionPool::_getConn( UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType )
{
	CommClientConnection* conn = _getConn( thisId, peerId, msgType );
	if( conn == 0 )
		return 0;
	if( sessionId != conn->sessionId )
	{
		PLog( "CommClientConnectionPool: msg type=%02X for connId=%08X with msg.sessionId=%08X != conn.sessionId=%08X - ignored", msgType, thisId, sessionId, conn->sessionId );
		return 0;
	}
	return conn;
}

void CommClientConnectionPool::_connect( CommClientConnection& conn )
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST, conn.priority );
	m.body.composeUINT32( conn.thisId ).composeString( conn.server ).composeString( conn.serverObject ).composeString( conn.serverConnType );
	conn.guardFactory->composeGuardNameList( m.body );
	m.body.composeString( "" );//end of guard name list
	m.body.composeString( "" );//empty physical connection list
	if (conn.version == CommClientConnection::versionExtended)
	{
		conn._clearAll();
		m.flags |= _COMM_MSGFLAGS_CONNECTION_EX;
		m.body.composeUINT32( conn.thisId ).composeUINT32( 0 );
		CommClientConnectionEx & connEx = static_cast<CommClientConnectionEx &>(conn);
		if (connEx.timerHandle == 0)
		{
			_CommMsg timerMsg(_COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX, 0);
			timerMsg.body.composeUINT32( connEx.thisId );
			connEx.timerHandle = inter->_postClientTimer( timerMsg, connEx.disconnectTimeOut);
		}
	}
	conn.state = _CommConnection::_StateConnRequestSent;
	conn.peerId = 0;//for special error cases
	conn.closePending = false;
	inter->_post( m );
}

void CommClientConnectionPool::_reconnect( CommClientConnection& conn )
{
	connMap.remove( conn.thisId );
	conn.thisId = connMap.add( &conn );
	_connect( conn );
}

void CommClientConnectionPool::_scheduleReconnect( CommClientConnection& conn )
{
	PASSERT3(	conn.state == _CommConnection::_StateConnClientFatalError ||
		conn.state == _CommConnection::_StateConnServerClosed ||
		conn.state == _CommConnection::_StateConnServerFatalError ||
		conn.state == _CommConnection::_StateConnDisc );

	_CommMsg msg( _COMM_MSGTYPE_CONNPOOL_TIMER, 0 );
	msg.body.composeUINT32( conn.thisId );
	inter->_postClientTimer( msg, _ReconnectTimeout );
}

void CommClientConnectionPool::_postClientError( CommClientConnection& conn, UINT16 errCode, const char* errMsg )
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_CLIENT_ERROR, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId ).composeUINT16( errCode ).composeString( errMsg );
	inter->_post( m );
}

void CommClientConnectionPool::_clientSideAuthFailed( CommClientConnection& conn )
{
	UINT16 errCode = _COMM_ERROR_AUTHFAILED;
	const char* errMsg = "Client-side authentication failed";
	_postClientError( conn, errCode, errMsg );

	conn.state = _CommConnection::_StateConnClientFatalError;
	PLog( "CommClientConnectionPool: ClientFatalError: %02X, %s", errCode, errMsg );
	conn._clearAll();
	bool ack = conn._safeClosed( errCode, errMsg );
	if( !ack )
		_scheduleReconnect( conn );
}


void CommClientConnectionPool::connect( CommClientConnection& conn, const char* server, const char* serverObject, const char* serverConnType )
{
	PASSERT3( conn.pool == 0 );
	conn.pool = this;
	conn.server = server;
	conn.serverObject = serverObject;
	conn.serverConnType = serverConnType;
	//	conn.priority = _COMM_MSGPRIORITY_INITCONNECT;
	conn.peerId = 0;//invalid value
	conn.sessionId = RANDOMTIME();
	conn.thisId = connMap.add( &conn );
	conn.closePending=false;
	delete conn.guardAuth;
	conn.guardAuth = conn.guardFactory->auth();
	if( connected )
		_connect( conn );
	else
		conn.state = _CommConnection::_StateConnDisc;
}

void CommClientConnectionPool::disconnect( CommClientConnection& conn )
{
	if (conn.pool == 0)
	{
		PLog( "CommClientConnectionPool::disconnect: connection not found");
		return;
	}
	PASSERT3( conn.pool == this );
	switch( conn.state )
	{
	case _CommConnection::_StateConnDisc:
	case _CommConnection::_StateConnServerClosed:
	case _CommConnection::_StateConnServerFatalError:
	case _CommConnection::_StateConnClientFatalError:
		//do nothing
		break;

	case _CommConnection::_StateConnRequestSent:
	case _CommConnection::_StateConnPeerAssigned:
	case _CommConnection::_StateConnGuardNegotiation:
		_postClientError( conn, _COMM_ERROR_CLOSEDBYPEER, "Connection aborted by client side software during negotiation phase" );
		break;

	case _CommConnection::_StateConnOk:
		{
			_CommMsg m( _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT, conn.priority );
			m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId ).composeUINT32( conn.sessionId );
			inter->_post( m );
		}
		break;

	default:
		PASSERT3( 0 );
		break;
	}
	connMap.remove( conn.thisId );
	delete conn.guardAuth;
	conn.guardAuth = 0;
	delete conn.guard;
	conn.guard = 0;
	conn.server = "";
	conn.serverObject = "";
	conn.serverConnType = "";
	if (conn.version == CommClientConnection::versionExtended)
	{
		CommClientConnectionEx & connEx = static_cast<CommClientConnectionEx &>(conn);
		connEx._clearAll();
	}
	conn.pool = 0;
}

bool CommClientConnectionPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( msg.type == _COMM_MSGTYPE_CONNPOOL_TIMER )
	{
		CommMsgParser parser( msg.body );
		UINT32 connId;
		parser.parseUINT32( connId );

		CommClientConnection** connPtr = connMap.find( connId );
		if( connPtr == 0 )
			PTRACE3( "CommClientConnectionPool: timer msg for unknown connId=%08X - ignored", connId );
		else
		{
			CommClientConnection* conn = *connPtr;
			_reconnect( *conn );
		}

		return true;
	}
	if( msg.type == _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX )
	{
		CommMsgParser parser( msg.body );
		UINT32 connId;
		parser.parseUINT32( connId );

		CommClientConnection** connPtr = connMap.find( connId );
		if( connPtr == 0 )
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX msg for unknown connId=%08X - ignored", connId );
		else
		{
			CommClientConnection* conn = *connPtr;
			PASSERT3(conn->version == CommClientConnection::versionExtended);
			CommClientConnectionEx * connEx = static_cast<CommClientConnectionEx *>(conn);
			if (connEx->processTimer()== false)
			{
				connEx->state = _CommConnection::_StateConnPeerAssigned;
				connEx->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
			}
		}
		return true;
	}

	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PLog( "CommClientConnectionPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	switch( msg.type )
	{
	case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		{
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_PHYSICAL_CONNECT" );
			if( connected )
			{
				PLog( "CommClientConnectionPool: _COMM_MSGTYPE_PHYSICAL_CONNECT in connected state - ignored" );
				return false;
			}

			int n = connMap.size();
			if( n )
			{
				int i, connI = 0;
				PArrayAutoPtr< CommClientConnection* > conns( new CommClientConnection*[ n ] );//!cannot call _reconnect within connMap iteration as it changes connMap
				for( CommClientConnection** connPtr = connMap.firstIndex( i ); connPtr ; connPtr = connMap.nextIndex( i ) )
				{
					CommClientConnection* conn = *connPtr;
					PASSERT3( conn->state == _CommConnection::_StateConnDisc );
					PASSERT3( connI < n );
					conns[ connI++ ] = conn;
				}
				PASSERT3( connI == n );
				for( i=0; i < n ; ++i )
					_reconnect( *conns[ i ] );
			}
			connected = true;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
		{
			PASSERT3( connected );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_PHYSICAL_DISCONNECT" );
			int i;
			for( CommClientConnection** connPtr = connMap.firstIndex( i ); connPtr ; connPtr = connMap.nextIndex( i ) )
			{
				CommClientConnection* conn = *connPtr;
				conn->state = _CommConnection::_StateConnDisc;
				conn->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
			}
			connected = false;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* conn = _getConn( thisId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnRequestSent )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->peerId = peerId;
					conn->state = _CommConnection::_StateConnPeerAssigned;
				}
			}
		}
		return true;

		/*
		case _COMM_MSGTYPE_CONNECT_REQUEST_PENDING:
		{
		PASSERT3( connected );
		CommMsgParser parser( msg.body );
		UINT32 thisId, peerId;
		const char* failedAddress;
		parser.parseUINT32( thisId ).parseUINT32( peerId ).parseString( failedAddress );
		PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST_PENDING: thisId=%08X peerId=%08X, failedAddress='%s'", thisId, peerId, failedAddress );

		CommClientConnection* conn = _getConn( thisId, peerId, msg.type );
		if( conn )
		{
		if( conn->state != _CommConnection::_StateConnPeerAssigned )
		{
		PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_PENDING msg for connId=%08X in state=%d - ignored", thisId, conn->state );
		}
		else
		conn->connectPending( failedAddress );
		}
		}
		return true;
		*/

	case _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE priority;
			const char* guardName;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority ).parseString( guardName );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED: thisId=%08X peerId=%08X priority=%d", thisId, peerId, priority );

			CommClientConnection* conn = _getConn( thisId, msg.type );
			if( conn )
			{
				// For solving problem with priority:
				if (conn->state == _CommConnection::_StateConnRequestSent)
				{
					conn->peerId = peerId;
					conn->state = _CommConnection::_StateConnPeerAssigned;
				}
				if( peerId != conn->peerId )
				{
					PLog( "CommClientConnectionPool: msg type=%02X for connId=%08X with msg.peerId=%08X != conn.peerId=%08X - ignored", msg.type, thisId, peerId, conn->peerId );
					return true;
				}
				// end
				if( conn->state != _CommConnection::_StateConnPeerAssigned )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					// check if this is extended connection in transDisconnect state
					if (conn->version == CommClientConnection::versionExtended)
					{
						CommClientConnectionEx * connEx = static_cast<CommClientConnectionEx *>(conn);
						if (connEx->inTransDisconnectState)
						{
							// we are in trans disconnect state, but server already closed connection.
							// imitating disconnect and continue
							connEx->_clearAll();
							connEx->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
						}
					}
					bool Ok = conn->_createGuard( guardName );
					if( !Ok )
					{
						PLog( "CommClientConnectionPool: unknown guard name='%s' for connId=%08X - ignored", guardName, thisId );
					}
					else
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommClientConnectionPool: invalid priority=%d in _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
						}
						else
							conn->priority = priority;
						++conn->sessionId;
						_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_REQUEST, conn->priority );
						m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId );
						conn->guard->composeFirstRequest( m.body );
						conn->state = _CommConnection::_StateConnGuardNegotiation;
						inter->_post( m );
					}
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommClientConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnGuardNegotiation )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					PASSERT3( conn->guard );
					_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST, conn->priority );
					m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId );
					bool Ok = conn->guard->processResponse( m.body, parser );
					if( Ok )
						inter->_post( m );
					else
						_clientSideAuthFailed( *conn );
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			BYTE priority;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId ).parseBYTE( priority );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED: thisId=%08X peerId=%08X sessionId=%08X priority=%d", thisId, peerId, sessionId, priority );
			if (msg.flags & _COMM_MSGFLAGS_CONNECTION_EX_RECONNECT)
			{
				// reconnect
				CommClientConnection* conn = _getConn( thisId, msg.type  );
				if (conn)
				{
					if (conn->version != CommClientConnection::versionExtended)
					{
						PLog( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X , incorrect version of connection - ignored", thisId );
						return true;
					}
					CommClientConnectionEx * connEx = static_cast<CommClientConnectionEx *>(conn);
					UINT32 clientId;
					UINT32 serverId;
					UINT32 receivedFromClient;
					parser.parseUINT32(serverId).parseUINT32(clientId).parseUINT32(receivedFromClient);
					if (connEx->inTransDisconnectState == false)
					{
						// we've already closed this connection, but it was reinstated by framework.
						// 
						connEx->state = _CommConnection::_StateConnClientFatalError;
						_postClientError( *conn, _COMM_ERROR_INCORRECT_PARAMETERS, "Client has already closed connection");
						_scheduleReconnect( *conn );
						return true;
					}

					connEx->serverThisId = serverId;
					if( connEx->state == _CommConnection::_StateConnRequestSent )
					{
						connEx->peerId = peerId;
						connEx->state = _CommConnection::_StateConnPeerAssigned;
					}
					if( peerId != connEx->peerId )
					{
						PLog( "CommClientConnectionPool: msg type=%02X for connId=%08X with msg.peerId=%08X != conn.peerId=%08X - ignored", msg.type, thisId, peerId, connEx->peerId );
						return true;
					}
					if( connEx->state != _CommConnection::_StateConnPeerAssigned )
					{
						PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, connEx->state );
					}
					else
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommClientConnectionPool: invalid priority=%d in _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
						}
						else
							connEx->priority = priority;
						connEx->state = _CommConnection::_StateConnOk;
						connEx->inTransDisconnectState = false;
						if( connEx->qMonitor )
							connEx->_postQMonitorRequest();
						connEx->_postWaitingRequests(receivedFromClient);

					}
				}
				return true;
			}

			CommClientConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnGuardNegotiation )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					PASSERT3( conn->guard );
					bool Ok = conn->guard->processGranted( parser );
					if( Ok )
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommClientConnectionPool: invalid priority=%d in _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
						}
						else
							conn->priority = priority;
						if (msg.flags & _COMM_MSGFLAGS_CONNECTION_EX)
						{
							if (conn->version != CommClientConnection::versionExtended)
							{
								PLog( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X , incorrect version of connection - ignored", thisId );
								return true;
							}
							CommClientConnectionEx * connEx = static_cast<CommClientConnectionEx *>(conn);
							UINT32 clientId;
							UINT32 serverId;
							parser.parseUINT32(serverId).parseUINT32(clientId);
							PASSERT3(clientId == thisId);
							connEx->serverThisId = serverId;
							
							if (connEx->timerHandle)
							{
								inter->_delServerTimer(connEx->timerHandle);
								connEx->timerHandle = 0;
							}
							if (connEx->pollTime > 0)
							{
								_CommMsg timerMsg(_COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX, 0);
								timerMsg.body.composeUINT32(connEx->thisId);
								connEx->timerHandle = inter->_postClientTimer(timerMsg, connEx->pollTime);
							}
						}

						CommMsgBody infoMsgBody;
						bool hasInfoBody = false;
						if (!parser.parseEnded())
						{
							hasInfoBody = true;
							parser.parseMsgBody(infoMsgBody);
						}
						conn->state = _CommConnection::_StateConnOk;
						conn->_safeConnected(hasInfoBody ? &infoMsgBody : 0);
						if( conn->qMonitor )
							conn->_postQMonitorRequest();
					}
					else
						_clientSideAuthFailed( *conn );
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			UINT16 errCode=0;
			const char * errMsg = "Connection closed by server side";
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			if (!parser.parseEnded())
			{
				parser.parseUINT16(errCode).parseString(errMsg);
			}
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommClientConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->state = _CommConnection::_StateConnServerClosed;
					conn->_clearAll();
					bool ack = conn->_safeClosed( errCode, errMsg);
					if( !ack )
						_scheduleReconnect( *conn );
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			CommMsgBody errMsgBody;
			bool hasErrBody = false;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			if (!parser.parseEnded())
			{
				hasErrBody = true;
				parser.parseMsgBody(errMsgBody);
			}

			CommClientConnection* conn = 0;
			if( peerId )
				conn = _getConn( thisId, peerId, msg.type );
			else
				conn = _getConn( thisId, msg.type );
			if( conn )
			{
				PTRACE3("CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s' (server='%s' serverObject='%s' serverConnType='%s')", thisId, peerId, (int)errCode, errMsg, conn->server.c_str(), conn->serverObject.c_str(), conn->serverConnType.c_str());
				conn->state = _CommConnection::_StateConnServerFatalError;
				PLog( "CommClientConnectionPool: ServerFatalError: %02X, %s", errCode, errMsg );
				conn->_clearAll();
				bool ack = conn->_safeClosed( errCode, errMsg, (hasErrBody ? &errMsgBody : 0) );
				if( !ack )
					_scheduleReconnect( *conn );
			}
			else
			{
				PTRACE3("CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg);
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_SERVER_ERROR:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_ERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );

			CommClientConnection* conn = 0;
			if( peerId )
				conn = _getConn( thisId, peerId, msg.type );
			else
				conn = _getConn( thisId, msg.type );
			if( conn )
			{
				conn->state = _CommConnection::_StateConnDisc;
				conn->_clearAll();
				conn->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");

				if( connected )
					_scheduleReconnect( *conn );
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* conn = 0;
			if( peerId )
				conn = _getConn( thisId, peerId, msg.type );
			else
				conn = _getConn( thisId, msg.type );
			if( conn )
			{

				if (conn->version == CommClientConnection::versionExtended && conn->state == _CommConnection::_StateConnOk)
				{
					CommClientConnectionEx * connEx = static_cast<CommClientConnectionEx *>(conn);
					connEx->inTransDisconnectState = true;
					conn->state = _CommConnection::_StateConnPeerAssigned;
					connEx->disconnectStarted=SYSTEMTICKS();
					if (connEx->timerHandle)
					{
						inter->_delServerTimer(connEx->timerHandle);
						connEx->timerHandle = 0;
					}
					_CommMsg timerMsg(_COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX, 0);
					timerMsg.body.composeUINT32(connEx->thisId);
					connEx->timerHandle = inter->_postClientTimer(timerMsg, connEx->disconnectTimeOut);
					return true;
				}
				conn->state = _CommConnection::_StateConnPeerAssigned;
				conn->_clearAll();
				conn->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* conn = 0;
			if( peerId )
				conn = _getConn( thisId, peerId, msg.type );
			else
				conn = _getConn( thisId, msg.type );
			if( conn )
			{
				PTRACE5( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X set true", thisId, peerId );
				conn->closePending=true;
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_SERVER_USERMSG:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			//no trace

			CommClientConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_SERVER_USERMSG msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					if (conn->version == CommClientConnection::versionExtended)
					{
						CommClientConnectionEx * connEx = static_cast<CommClientConnectionEx *>(conn);
						connEx->_processUserMessage(parser);
					}
					else
					{
						PASSERT3( conn->guard );
						if (conn->guard->isSimple())
						{
							msg.body._skipPrefix(4 * sizeof(UINT32));
							UINT32 msgId;
							_CommConnection::_unwrapUserMsg( msgId, msg.body );
							conn->_safeProcessMessage( msgId, msg.body );
						}
						else
						{
							CommMsgBody body;
							conn->guard->processInBlock( body, parser );
							UINT32 msgId;
							_CommConnection::_unwrapUserMsg( msgId, body );
							conn->_safeProcessMessage( msgId, body );
						}
					}
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE quality;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( quality );

			CommClientConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeQualityUpdated( quality );
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR:	//should not normally happen, but...
		//means that no 'far' connections were found on the path
		//we will emulate perfect (100) connection with no quality updates
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );

			CommClientConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->_safeProcessQualityRequest();
				}
			}
		}
		return true;
	case _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE5( "CommClientConnectionPool: _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE: thisId=%08X, peerId=%08X", thisId, peerId );
			CommClientConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommClientConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->_safeProcessKeepAliveRequest();
				}
			}
		}
		 return true;




	}//switch
	return false;
}


/* static */ void CommClientConnectionPool::_getEmbeddedMessageOnTheFly( const _CommMsg& msg, UINT32 subscrMsgId, CommEmbeddedMessageOnTheFlyCallback & callback )
{
	if( msg.type == _COMM_MSGTYPE_CONNECT_SERVER_USERMSG )
    {
		PASSERT3(msg.body._size() >= 5 * sizeof(UINT32) + 2);
        UINT32 msgId;
		const BYTE * p = msg.body._readPtr();
		msgId = CommMsgBody::readUINT32( p + sizeof(UINT32) * 4 );
        if( msgId == subscrMsgId )
        {
            BYTE type = *(p+sizeof(UINT32) * 5 + 1);
            _CommMsg m( type, _COMM_MSGPRIORITY_INVALID );
            m.body.moveFrom( ((_CommMsg&)msg).body );
			m.body._skipPrefix(5 * sizeof(UINT32) + 2);
            callback.callback( m );
			m.body._createPrefix(5 * sizeof(UINT32) + 2);
			((_CommMsg&)msg).body.moveFrom(m.body );
        }
    }
}
//****************************************************************************



