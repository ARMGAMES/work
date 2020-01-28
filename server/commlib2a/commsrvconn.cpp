#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commcli.h"
#include "commconn.h"

CommServerGuard::CommServerGuard( const char* name_ )
{
	conn = 0;
	_name = name_;
}

void CommServerGuard::_setConnection( CommServerConnection* conn_ )
{
	conn = conn_;
}

void CommServerGuard::_postGuardError( UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	conn->pool->_postGuardError( conn, errCode, errMsg, errMsgBody );
}

void CommServerGuard::_postGuardAddResponse( const CommMsgBody& body )
{
	conn->pool->_postGuardAddResponse( conn, body );
}

void CommServerGuard::_postGuardRequestGranted( const CommMsgBody& body, const CommMsgBody* infoMsgBody  )
{
	conn->pool->_postGuardRequestGranted( conn, body, infoMsgBody );
}

//*****************************************************************************

CommServerConnectionFactory::CommServerConnectionFactory( CommServerGuardFactory& guardFactory_ )
{
	guardFactories.push_back(&guardFactory_);
	authPriority = _COMM_MSGPRIORITY_INITCONNECT;
	normalPriority = _COMM_MSGPRIORITY_INITCONNECT;
	clientPriorityAllowed = false;
}

void CommServerConnectionFactory::addGuardFactory( CommServerGuardFactory& guardFactory_ )
{
	guardFactories.push_back(&guardFactory_);
}

CommServerGuard * CommServerConnectionFactory::createGuard(const vector< const char* >& guardNames )
{
	CommServerGuard * f = 0;
	for (int i=0;i<guardFactories.size();i++)
	{
		f = guardFactories[i]->createGuard(guardNames);
		if (f)
			return f;
	}
	return 0;
}

//*****************************************************************************

CommServerConnection::CommServerConnection()
{
	version = versionBase;
	guard = 0;
	pool = 0;
	qMonitor = false;
//	zombieStatus = 0;
	inPreConnectedMap = false;
	closePending = false;
}


CommServerConnectionEx::CommServerConnectionEx() :
CommServerConnection()
{
	version = versionExtended;
	currentNumber=0;
	receivedNumber=0;
	clientThisId=0;
	inTransDisconnectState=false;
	disconnectStarted = 0;
	confirmedNumber = 0;
	disconnectTimeOut = CONN_EX_DISCONNECT_TIME_OUT;
	pollTime = CONN_EX_POLL_TIME;
	timerHandle = 0;
}

/* virtual */ CommServerConnection::~CommServerConnection()
{
	delete guard;
}

CommServerConnectionEx::~CommServerConnectionEx()
{
	if( timerHandle )
		pool->inter->_delServerTimer( timerHandle );
	_clearAll();
}

void CommServerConnectionEx::_clearAll()
{
	PTRACE5("CommServerConnectionEx::_clearAll called, thisId=%X",thisId);
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = (*it);
		delete item;
	}
	msgList.clear();
	currentNumber=0;
	receivedNumber=0;
	clientThisId=0;
	disconnectStarted = 0;
	confirmedNumber = 0;
	inTransDisconnectState=false;
}

void CommServerConnectionEx::_processErrors(UINT16 errCode,const char * errMsg)
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_ERROR, priority );
	m.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT16( errCode ).composeString( errMsg );
	pool->inter->_post( m );
	state = _CommConnection::_StateConnClientFatalError;
	PLog( "CommServerConnectionEx: _processErrors: thisId=%X, %02X, %s", thisId, errCode, errMsg );
	_safeClosed(errCode,errMsg);
}

void CommServerConnectionEx::post( UINT32 msgId, CommMsgBody& body )
{
//	if( zombieStatus == 2 )
//	{//MUST be before assert( pool ) as zombies don't have pool
//		PLog( "CommServerConnectionPool: Message to zombie connection - ignored" );
//		return;
//	}

	PASSERT3( pool );
	if( !( state == _CommConnection::_StateConnOk || inTransDisconnectState) )
	{
		PLog( "CommServerConnectionPool: Message to disconnected logical connection - ignored, thisId=%X",thisId );
		return;
	}

	PASSERT3( guard );
	_CommConnection::_wrapUserMsg( body, msgId );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_SERVER_USERMSG, priority );

	currentNumber++;
	msg.body.composeUINT32( peerId ).composeUINT32( thisId ).composeUINT32( sessionId )
		.composeUINT32( thisId ).composeUINT32( clientThisId )
		.composeUINT32( currentNumber ).composeUINT32( receivedNumber ).composeBYTE(1);
	PTRACE5("CommServerConnectionEx::post: thisId=%X,curentNumer=%u", thisId,currentNumber);
	guard->processOutBlock( msg.body, body );
	_Item* item = new _Item();
	_List::iterator it = msgList.insert( msgList.end(), item );
	item->body.copyFrom( msg.body );
	item->currentNumber = currentNumber;
	if (!inTransDisconnectState)
	{
		pool->_post( msg );
		confirmedNumber = receivedNumber;
		PTRACE5("CommServerConnectionEx::post: thisId=%X,confirmedNumber=%u", thisId,confirmedNumber);
	}
	else
	{
		PTRACE5("CommServerConnectionEx::post: in TransDisconnect state, thisId=%X",thisId);
	}
}

bool CommServerConnectionEx::setPollTime(INT32 pollTime_)
{
	if (isConnected()) return false;
	pollTime = pollTime_;
	return true;
}

void CommServerConnectionEx::_postWaitingRequests()
{
	PASSERT3( pool );
	if( state != _CommConnection::_StateConnOk)
	{
		PLog( "CommServerConnectionPool: Message to disconnected logical connection - ignored, thisId=%X",thisId );
		return;
	}
	_List::iterator it;
	for (it = msgList.begin(); it != msgList.end(); ++it)
	{
		_Item* item = (*it);
		_CommMsg msg( _COMM_MSGTYPE_CONNECT_SERVER_USERMSG, priority );
		msg.body.copyFrom( item->body );
		BYTE* p = msg.body._writePtr();
		CommMsgBody::writeUINT32( p, peerId );
		p += 4*6; //4 - sizeof(INT32), 6 - position of 'receivedNumber'
		CommMsgBody::writeUINT32( p, receivedNumber );
		pool->_post( msg );
		confirmedNumber = receivedNumber;
		PTRACE5("CommServerConnectionEx::_postWaitingRequests: thisId=%X, confirmedNumber=%u, currentNumber=%u",thisId,confirmedNumber,item->currentNumber);
	}
}
bool CommServerConnectionEx::processTimer()
{
	timerHandle = 0;
	PASSERT3( pool );

	if (inTransDisconnectState)
	{
		PTRACE5("CommServerConnectionEx::processTimer - timeout, close connection,thisId=%X",thisId);
		_clearAll();
		return false;
	}

	if (pollTime > 0)
	{
		if (confirmedNumber != receivedNumber)
		{
			_CommMsg msg(_COMM_MSGTYPE_CONNECT_SERVER_USERMSG, priority);
			msg.body.composeUINT32(peerId).composeUINT32(thisId).composeUINT32(sessionId)
				.composeUINT32(thisId).composeUINT32(clientThisId)
				.composeUINT32(currentNumber).composeUINT32(receivedNumber).composeBYTE(0);
			pool->_post(msg);
			confirmedNumber = receivedNumber;
			PTRACE5("CommServerConnectionEx::processTimer: sent confirmation, thisId=%X,confirmedNumber=%u", thisId, confirmedNumber);
		}
		_CommMsg msg(_COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX, 0);
		msg.body.composeUINT32(thisId);
		timerHandle = pool->inter->_postClientTimer(msg, pollTime);
	}
	return true;
}


bool CommServerConnectionEx::_processUserMessage( CommMsgParser & parser)
{
	PASSERT3( guard );
	PASSERT3( pool );
	UINT32 serverThisId_;
	UINT32 clientThisId_;
	UINT32 receivedServerNumber_;
	UINT32 currentClientNumber_;
	BYTE flags;
	parser.parseUINT32(clientThisId_).parseUINT32(serverThisId_).parseUINT32(currentClientNumber_).parseUINT32(receivedServerNumber_).parseBYTE(flags);

	if  ( (serverThisId_ != thisId) || 
		(clientThisId_ != clientThisId) ||
		( (INT32)(currentClientNumber_ - receivedNumber) > 1) || 
		( (INT32)(receivedServerNumber_ - currentNumber) > 0) 
		)
	{
		PTRACE3("CommServerConnectionEx::_processUserMessage: thisId=%X, receivedServerId=%X,"
			"clientId=%X, receivedClientId=%X, currentNumber=%u, receivedNumber=%u, "
			"currentClientNumber_=%u,receivedServerNumber_=%u",
			thisId,serverThisId_, clientThisId,clientThisId_,currentNumber,
			receivedNumber,currentClientNumber_,receivedServerNumber_);
		_processErrors(_COMM_ERROR_INCORRECT_PARAMETERS,"Incorrect parameters of extended protocol");
		return false;
	}
	PTRACE5("CommServerConnectionEx::_processUserMessage: thisId=%X, receivedServerId=%X,"
		"clientId=%X, receivedClientId=%X, currentNumber=%u, receivedNumber=%u, "
		"currentClientNumber_=%u,receivedServerNumber_=%u",
		thisId,serverThisId_, clientThisId,clientThisId_,currentNumber,
		receivedNumber,currentClientNumber_,receivedServerNumber_);

	_List::iterator it = msgList.begin();
	while( it != msgList.end() )
	{
		_Item* item = (*it);
		if ((INT32)(item->currentNumber - receivedServerNumber_)<=0)
		{
			PTRACE5("CommServerConnectionEx::_processUserMessage: thisId=%X, message number=%u confirmed, deleted",thisId,item->currentNumber);
			delete item;
			msgList.erase(it);
			it = msgList.begin();
		}
		else
			break;
	}

	if (!flags)
		return true; // no data
	if ((currentClientNumber_ - receivedNumber) != 1)
	{
		// Already processed, ignore
		PTRACE5("CommServerConnectionEx::_processUserMessage: thisId=%X, message already processed",thisId);
		return true;
	}
	receivedNumber = currentClientNumber_;
	CommMsgBody body;
	guard->processInBlock( body, parser );
	UINT32 msgId;
	_CommConnection::_unwrapUserMsg( msgId, body );
	_safeProcessMessage( msgId, body );
	return true;
}


void CommServerConnection::monitorQuality()
{
	qMonitor = true;
	if( pool && state == _CommConnection::_StateConnOk )
		_postQMonitorRequest();
}

void CommServerConnection::_postQMonitorRequest()
{
	PASSERT3( pool );
	_CommMsg msg( _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR, priority );
	msg.body.composeUINT32( peerId ).composeUINT32( thisId );
	pool->_post( msg );
}

void CommServerConnection::monitorIsAlive()
{
	if( pool && state == _CommConnection::_StateConnOk )
	{
		_CommMsg msg( _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE, priority );
		msg.body.composeUINT32( peerId ).composeUINT32( thisId );
		pool->_post( msg );
	}
}



int CommServerConnection::connPathLen() const
{
	return physAddresses.size();
}

const char* CommServerConnection::connPathPoint( int i ) const
{
	PASSERT3( i >= 0 && i < physAddresses.size() );
	return physAddresses[ i ];
}

void CommServerConnection::post( UINT32 msgId, CommMsgBody& body )
{
//	if( zombieStatus == 2 )
//	{//MUST be before assert( pool ) as zombies don't have pool
//		PLog( "CommServerConnectionPool: Message to zombie connection - ignored" );
//		return;
//	}

	PASSERT3( pool );

	if (body._size() > 512000000 )
	{
		PLog( "CommServerConnection(thisId=%X): Message id=%d,  XLPOST sz=%u MB", thisId, msgId, (unsigned int)(body._size()/1000000));
	}

	if( state != _CommConnection::_StateConnOk )
	{
		PLog( "CommServerConnectionPool: Message to disconnected logical connection - ignored" );
		return;
	}

	PASSERT3( guard );
	_CommConnection::_wrapUserMsg( body, msgId );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_SERVER_USERMSG, priority );
	if (guard->isSimple())
	{
		size_t sz = body._size();
		BYTE* p = body._createPrefix( 4 * sizeof(UINT32));
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

void CommServerConnection::_safeProcessMessage( UINT32 msgId, const CommMsgBody& body )
{
	try
	{
		PSIGRETURN()
		processMessage( msgId, body );
	}
	catch( PError& err )
	{
		PLog( "CommServerConnection: exception in processMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommServerConnection: STD exception in processMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, exc.what() );
	}
	catch( ... )
	{
		PLog( "CommServerConnection: unknown exception in processMessage (msgId=%d=0x%X) caught", (int)msgId, (int)msgId );
	}
}

void CommServerConnection::_safeConnected()
{
	try
	{
		PSIGRETURN()
			connected();
	}
	catch( PError& err )
	{
		PLog( "CommServerConnection: exception in connected() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommServerConnection: unknown exception in connected() caught" );
	}
}


void CommServerConnection::_safeQualityUpdated( int quality )
{
	try
	{
		PSIGRETURN()
			qualityUpdated( quality );
	}
	catch( PError& err )
	{
		PLog( "CommServerConnection: exception in qualityUpdated() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommServerConnection: unknown exception in qualityUpdated() caught" );
	}
}

void CommServerConnection::_safeIsAlive()
{
	try
	{
		PSIGRETURN()
			isAlive();
	}
	catch( PError& err )
	{
		PLog( "CommServerConnection: exception in isAlive() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommServerConnection: unknown exception in isAlive() caught" );
	}
}

void CommServerConnection::_safeClosed( int errCode, const char* errMsg )
{
	try
	{
		PSIGRETURN()
			closedOrDisconnected(errCode,errMsg );
	}
	catch( PError& err )
	{
		PLog( "CommServerConnection: exception in closed() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommServerConnection: unknown exception in closed() caught" );
	}
}

//*****************************************************************************

CommServerConnectionPool::CommServerConnectionPool( _CommInterface& inter_ )
{
	inter = &inter_;
	connected = false;
	_initReplay = false;
	_lastReplayRecord = 0;
	_typeOfMessageTime = normalTime;
	_currentMessageTime = plib_time(0);
	plib_localtime_ext_bias(_currentMessageTime,&_currentLocalTime,&_currentBias);

}

void CommServerConnectionPool::registerFactory( const char* name, CommServerConnectionFactory& factory )
{
	factoryMap.insert( _FactoryMap::value_type( name, &factory ) );
}

void CommServerConnectionPool::unregisterFactory( const char* name )
{
	_FactoryMap::iterator found = factoryMap.find( name );
	if ( found != factoryMap.end())
	{
		factoryMap.erase(found);
	}
	else
	{
		PTRACE3("CommServerConnectionPool::unregisterFactory - factory '%s' bot foun d, ignore",name);
	}
}


CommServerConnection* CommServerConnectionPool::findConnection( UINT32 connId )
{
	CommServerConnection** connPtr = connMap.find( connId );
	if( connPtr == 0 )
		return 0;
	CommServerConnection* conn = *connPtr;
	PASSERT3( conn->thisId == connId );
	return conn;
}

const CommServerConnection* CommServerConnectionPool::findConnection( UINT32 connId ) const 
{
	const CommServerConnection * const * connPtr = connMap.find( connId );
	if( connPtr == 0 )
		return 0;
	const CommServerConnection* conn = *connPtr;
	PASSERT3( conn->thisId == connId );
	return conn;
}

CommServerConnection* CommServerConnectionPool::firstConnection( ConnectionIterator& i )
{
	CommServerConnection** connPtr = connMap.firstIndex( i.i );
	return connPtr ? *connPtr : 0;
}

CommServerConnection* CommServerConnectionPool::nextConnection( ConnectionIterator& i )
{
	CommServerConnection** connPtr = connMap.nextIndex( i.i );
	return connPtr ? *connPtr : 0;
}

CommServerConnection* CommServerConnectionPool::_getConn( UINT32 thisId, BYTE msgType )
{
	CommServerConnection** connPtr = connMap.find( thisId );
	if( connPtr == 0 )
	{
		PLog( "CommServerConnectionPool: msg type=%02X for unknown connId=%08X - ignored", msgType, thisId );
		return 0;
	}
	CommServerConnection* conn = *connPtr;
	PASSERT3( conn->thisId == thisId );

	return conn;
}

CommServerConnection* CommServerConnectionPool::_getConn( UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	CommServerConnection* conn = _getConn( thisId, msgType );
	if( conn == 0 )
		return 0;
	if( peerId != conn->peerId )
	{
		PLog( "CommServerConnectionPool: msg type=%02X for connId=%08X with msg.peerId=%08X != conn.peerId=%08X - ignored", msgType, thisId, peerId, conn->peerId );
		return 0;
	}
	if (conn->inPreConnectedMap)
	{//received peerId for this connection for the first time - need to remove conn from preConn list
		_PreConnMap::iterator found = preConns.find(peerId);
		if ( found == preConns.end())
		{
			PLog( "CommServerConnectionPool:_getConn connection not found in pre-connection map: peerId=%08X, size=%d", peerId,preConns.size());
		}
		else
		{
			preConns.erase( found );
			PTRACE5("CommServerConnectionPool:_getConn peerId=%08X, thisId=%08X erased from pre-connection map (%d)",peerId, thisId,preConns.size());
		}
		conn->inPreConnectedMap = false;
	}
	return conn;
}

CommServerConnection* CommServerConnectionPool::_getConn( UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType )
{
	CommServerConnection* conn = _getConn( thisId, peerId, msgType );
	if( conn == 0 )
		return 0;
	if( sessionId != conn->sessionId )
	{
		PLog( "CommServerConnectionPool: msg type=%02X for connId=%08X with msg.sessionId=%08X != conn.sessionId=%08X - ignored", msgType, thisId, sessionId, conn->sessionId );
		return 0;
	}
	return conn;
}

void CommServerConnectionPool::_delConn( CommServerConnection* conn )
{
	if (conn->inPreConnectedMap)
	{//received peerId for this connection for the first time - need to remove conn from preConn list
		_PreConnMap::iterator found = preConns.find(conn->peerId);
		if ( found == preConns.end())
		{
			PLog( "CommServerConnectionPool:_delConn connection not found in pre-connection map: peerId=%08X, size=%d", conn->peerId,preConns.size());
		}
		else
		{
			preConns.erase( found );
			PTRACE5("CommServerConnectionPool:_delConn peerId=%08X, thisId=%08X erased from pre-connection map (%d)",conn->peerId, conn->thisId, preConns.size());
		}
		conn->inPreConnectedMap = false;
	}
	connMap.remove( conn->thisId );
//	if( conn->zombieStatus == 1 )
//	{
//		conn->pool = 0;//just in case
//		conn->zombieStatus = 2;
//		PTRACE3( "CommServerConnectionPool: conn %X converted to zombie", conn->thisId );
//	}
//	else
		delete conn;
}

void CommServerConnectionPool::_postGuardError( CommServerConnection* conn, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	PASSERT3( conn->state == _CommConnection::_StateConnGuardNegotiation );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT16( errCode ).composeString( errMsg );
	if ( errMsgBody )
	{
		m.body.composeMsgBody(*errMsgBody);
	}
	inter->_post( m );
	conn->_safeClosed( errCode, errMsg );
	_delConn( conn );
}

void CommServerConnectionPool::_postGuardAddResponse( CommServerConnection* conn, const CommMsgBody& body )
{
	PASSERT3( conn->state == _CommConnection::_StateConnGuardNegotiation );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId );
	m.body.merge( body );
	inter->_post( m );
}

void CommServerConnectionPool::_postGuardRequestGranted( CommServerConnection* conn, const CommMsgBody& body, const CommMsgBody* infoMsgBody  )
{
	PASSERT3( conn->state == _CommConnection::_StateConnGuardNegotiation );
	conn->state = _CommConnection::_StateConnOk;
	//	conn->_safeConnected();
	conn->priority = conn->normalPriority;
	_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId ).composeBYTE( conn->priority );
	if (conn->version == CommServerConnection::versionExtended)
	{
		CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
		m.body.composeUINT32(connEx->thisId).composeUINT32(connEx->clientThisId);
		m.flags |= _COMM_MSGFLAGS_CONNECTION_EX;
	}
	m.body.merge( body );
	if ( infoMsgBody )
	{
		m.body.composeMsgBody(*infoMsgBody);
	}

	inter->_post( m );

	conn->_safeConnected();
	if( conn->qMonitor )
		conn->_postQMonitorRequest();
}

CommServerConnection* CommServerConnectionPool::_lookForConn( UINT32 cliId )
{
	_PreConnMap::iterator it = preConns.find(cliId);
	if (it != preConns.end())
		return (*it).second;

	else
	{
		//		int i;
		//		PTRACE3("_lookForConn cliId=%08X falled back to old algorithm, (%d)",cliId,preConns.size());
		//		for( CommServerConnection** c = connMap.firstIndex( i ); c ; c = connMap.nextIndex( i ) )
		//			if( (*c)->peerId == cliId )
		//				return *c;
		return 0;
	}
}

void CommServerConnectionPool::initRecordReplay(const char * name)
{
	if ( _recordReplayFile.isOpened() == false)
	{
		_recordReplayFile.openFile(name);
		_initReplay = false;
		setTypeOfTime(recordingType);
	}
}

void CommServerConnectionPool::closeRecordReplay()
{
	if (_recordReplayFile.isOpened())
	{
		int tt = -1;
		_recordReplayFile.writeFile((const BYTE *) &tt,sizeof(int));
		_recordReplayFile.closeFile();
		setTypeOfTime(normalTime);
	}
}

void CommServerConnectionPool::setTypeOfTime(eTimeType type)
{
	_typeOfMessageTime = type;
	if (_typeOfMessageTime == recordingType)
	{
		_currentMessageTime = plib_time(0);
		plib_localtime_ext_bias(_currentMessageTime,&_currentLocalTime,&_currentBias);
	}
	else if (_typeOfMessageTime == replayingType)
	{
		_currentMessageTime = 0;
	}
}

time_t CommServerConnectionPool::messageTime() const
{
	return _currentMessageTime;
	/*
	if (_typeOfMessageTime == normalTime)
	{
	return plib_time( NULL );
	}
	else 
	{
	return _currentMessageTime;
	}
	*/
}

void CommServerConnectionPool::messageSrvLocalTime(CommSrvTime & t) const
{
	t.assign(_currentLocalTime);
	/*
	if (_typeOfMessageTime == normalTime)
	{
	t.currentLocalTime();
	}
	else 
	{
	t.assign(_currentLocalTime);
	}
	*/
}


bool CommServerConnectionPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PLog( "CommServerConnectionPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	if( msg.type == _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX )
	{
		CommMsgParser parser( msg.body );
		UINT32 connId;
		parser.parseUINT32( connId );

		CommServerConnection** connPtr = connMap.find( connId );
		if( connPtr == 0 )
			PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX msg for unknown connId=%08X - ignored", connId );
		else
		{
			CommServerConnection* conn = *connPtr;
			PASSERT3(conn->version == CommServerConnection::versionExtended);
			CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
			if (connEx->processTimer()== false)
			{
				connEx->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
				_delConn( conn );
			}
		}
		return true;
	}


	if (_typeOfMessageTime != replayingType)
	{
		time_t t = plib_time(0);
		if (t != _currentMessageTime)
		{
			_currentMessageTime = t;
			plib_localtime_ext_bias(_currentMessageTime,&_currentLocalTime,&_currentBias);
		}
	}


	if (msg.type == _COMM_MSGTYPE_CONNECT_REQUEST ||
		msg.type == _COMM_MSGTYPE_CONNECT_GUARD_REQUEST ||
		msg.type == _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST ||
		msg.type == _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG ||
		msg.type == _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT ||
		msg.type == _COMM_MSGTYPE_CONNECT_CLIENT_ERROR ||
		msg.type == _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT 
		)
	{
		if (_recordReplayFile.isOpened())
		{
			if (_initReplay == true)
			{
				UINT32 t = SYSTEMTICKS();
				int tt = t - _lastReplayRecord;
				_lastReplayRecord = t;

				_recordReplayFile.writeFile((const BYTE *) &tt,sizeof(tt));
			}
			else
			{
				_lastReplayRecord = SYSTEMTICKS();
				_initReplay = true;
			}

			_recordReplayFile.writeFile((const BYTE *) &msg.type,sizeof(msg.type));
			_recordReplayFile.writeFile((const BYTE *) &msg.priority,sizeof(msg.priority));
			UINT32 size=msg.body._size();
			_recordReplayFile.writeFile((const BYTE *) &size,sizeof(UINT32));
			_recordReplayFile.writeFile( msg.body._readPtr(),size);

			if (_typeOfMessageTime == recordingType)
			{
				_recordReplayFile.writeFile((const BYTE *) &_currentMessageTime,sizeof(_currentMessageTime));
				_recordReplayFile.writeFile((const BYTE *) &_currentBias,sizeof(_currentBias));
			}
		}
		else if (_typeOfMessageTime == replayingType)
		{
			if (msg.type == _COMM_MSGTYPE_CONNECT_REQUEST)
			{
			}
			else
			{
				size_t size =  msg.body._size() - 6;
				if (size >= 8)
				{
					CommMsgBody b;
					b.moveFrom(msg.body);
					const BYTE* p = b._readPtr() + size;
					UINT32 cTime = CommMsgBody::readUINT32( p );
					p += 4;
					_currentBias = (INT16) CommMsgBody::readUINT16( p );
					if (_currentMessageTime != cTime)
					{
						_currentMessageTime = cTime;
						plib_timetToStructTm(_currentMessageTime-_currentBias*60,&_currentLocalTime);
					}
					BYTE * pw = msg.body._alloc(size);
					memcpy(pw,b._readPtr(),size);
				}
			}
		}
	}

	switch( msg.type )
	{
	case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_PHYSICAL_CONNECT" );
		if( connected )
		{
			PLog( "CommServerConnectionPool: _COMM_MSGTYPE_PHYSICAL_CONNECT in connected state - ignored" );
			return false;
		}
		connected = true;
		return false;//not the only message consumer

	case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
		{
			PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_PHYSICAL_DISCONNECT" );
			int n = connMap.size();
			if( n )
			{
				int i, connI = 0;
				PArrayAutoPtr< CommServerConnection* > conns( new CommServerConnection*[ n ] );//although it seems safe to call _delConn within connMap iteration, but it changes connMap, so we will move it out of the loop - just in case
				for( CommServerConnection** connPtr = connMap.firstIndex( i ); connPtr ; connPtr = connMap.nextIndex( i ) )
				{
					CommServerConnection* conn = *connPtr;
					PASSERT3( connI < n );
					conns[ connI++ ] = conn;
				}
				PASSERT3( connI == n );
				for( i=0; i < n ; ++i )
				{
					conns[ i ]->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
					_delConn( conns[ i ] );
				}
			}
			connected = false;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_CONNECT_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 peerId;
			const char* server;
			const char* serverObject;
			const char* connType;
			parser.parseUINT32( peerId ).parseString( server ).parseString( serverObject ).parseString( connType );
			PTRACE4( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: peerId=%08X server='%s' serverObject='%s' connType='%s'", peerId, server, serverObject, connType );

			if (msg.flags &_COMM_MSGFLAGS_CONNECTION_EX)
			{
				if (msg.flags &_COMM_MSGFLAGS_CONNECTION_EX_RECONNECT)
				{
					PTRACE4( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: reinstate connection");
					for(;;)
					{
						const char* guardName;
						parser.parseString( guardName );
						if( *guardName == 0 )
							break;//forever
					}
					for(;;)
					{
						const char* physAddr;
						parser.parseString( physAddr );
						if( *physAddr == 0 )
							break;
					}
					UINT32 clientId;
					UINT32 serverId;
					if (parser.parseEnded())
					{
						PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error reinstate connection: peerId=%08X server='%s' serverObject='%s' connType='%s' - parse ended", peerId, server, serverObject, connType);
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
						inter->_post( m );
						return true;
					}
					parser.parseUINT32(clientId).parseUINT32(serverId);
					if (!serverId)
					{
						PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error reinstate connection: peerId=%08X server='%s' serverObject='%s' connType='%s', clientId=%X - zero serverId", peerId, server, serverObject, connType, clientId);
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
						inter->_post( m );
						return true;
					}
					CommServerConnection* conn = _getConn( serverId, msg.type );
					if (!conn)
					{
						PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error reinstate connection: peerId=%08X server='%s' serverObject='%s' connType='%s', clientId=%X, serverId=%x - connecton not found", peerId, server, serverObject, connType, clientId, serverId);
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
						inter->_post( m );
						return true;
					}
					if (conn->version != CommServerConnectionEx::versionExtended)
					{
						PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error reinstate connection: peerId=%08X server='%s' serverObject='%s' connType='%s', clientId=%X, serverId=%x - incompatible protocol versions", peerId, server, serverObject, connType, clientId, serverId);
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
						inter->_post( m );
						return true;
					}
					CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
					if (connEx->clientThisId != clientId)
					{
						PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error reinstate connection: peerId=%08X server='%s' serverObject='%s' connType='%s', clientId=%X, serverId=%x - incorrect clientId (%x)", peerId, server, serverObject, connType, clientId,serverId,connEx->clientThisId);
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
						inter->_post( m );
						return true;
					}
					if (connEx->inTransDisconnectState == false)
					{
						PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: reinstate connection: peerId=%08X server='%s' serverObject='%s' connType='%s', clientId=%X, serverId=%x - not in transdisconnected state", peerId, server, serverObject, connType, clientId,serverId);
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
						inter->_post( m );
						return true;
					}
					connEx->peerId = peerId; // peerId is changed
					connEx->inTransDisconnectState = false;
					{
						// just in case
						_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED, connEx->priority );
						m.body.composeUINT32( connEx->peerId ).composeUINT32( connEx->thisId );
						inter->_post( m );
					}
					connEx->state = _CommConnection::_StateConnOk;
					{
						_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED, connEx->priority );
						m.flags = (_COMM_MSGFLAGS_CONNECTION_EX|_COMM_MSGFLAGS_CONNECTION_EX_RECONNECT);
						m.body.composeUINT32( connEx->peerId ).composeUINT32( connEx->thisId ).composeUINT32( connEx->sessionId ).composeBYTE( connEx->priority )
							.composeUINT32( connEx->thisId ).composeUINT32( connEx->clientThisId)
							.composeUINT32( connEx->receivedNumber );
						inter->_post( m );
					}
					if( connEx->qMonitor )
						connEx->_postQMonitorRequest();
					connEx->_postWaitingRequests();
					return true;
				}
			}

			vector< const char* > guardNames;
			for(;;)
			{
				const char* guardName;
				parser.parseString( guardName );
				if( *guardName == 0 )
					break;//forever
				guardNames.push_back( guardName );
			}
			vector< const char* > physAddrs;
			for(;;)
			{
				const char* physAddr;
				parser.parseString( physAddr );
				if( *physAddr == 0 )
					break;
				PTRACE3( "CommServerConnectionPool: Incoming connection via '%s'", physAddr );
				physAddrs.push_back(physAddr);
			}

			_FactoryMap::iterator found = factoryMap.find( connType );
			CommServerConnectionFactory* factory = 0;
			if (found != factoryMap.end())
			{
				factory = (*found).second;
			}
			else
			{
				if (_aboutToConnect(msg.body,connType) && (found = factoryMap.find( connType )) != factoryMap.end())
				{
					factory = (*found).second;
				}
				else
				{
					PLog( "CommServerConnectionPool: Connection ('%s'/'%s'/'%s') denied (unknown connType)", server, serverObject, connType );				

					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOCONNTYPE ).composeString( "Unknown connection type" );
					inter->_post( m );

					if (_recordReplayFile.isOpened() )
					{
						UINT32 zero=0;
						_recordReplayFile.writeFile((const BYTE *) &zero,sizeof(UINT32));
					}
					return true;
				}
			}

			CommServerGuard* guard = factory->createGuard( guardNames );
			if( guard == 0 )
			{
				PLog( "CommServerConnectionPool: Connection ('%s'/'%s'/'%s') denied (no acceptable guard)", server, serverObject, connType );
				_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
				m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOACCEPTABLEGUARD ).composeString( "No acceptable guard" );
				inter->_post( m );

				if (_recordReplayFile.isOpened() )
				{
					UINT32 zero=0;
					_recordReplayFile.writeFile((const BYTE *) &zero,sizeof(UINT32));
				}

				return true;
			}

			CommServerConnection* conn = factory->createConnection();
			conn->state = _CommConnection::_StateConnGuardNegotiationNoSession;
			if (msg.priority != _COMM_MSGPRIORITY_INITCONNECT)
			{
				// Client requests priority different from default.
				if (factory->clientPriorityAllowed)
				{
					conn->priority = msg.priority;
					conn->normalPriority = msg.priority;
				}
				else
				{
					conn->priority = factory->authPriority;
					conn->normalPriority = factory->normalPriority;
				}
			}
			else
			{
				conn->priority = factory->authPriority;
				conn->normalPriority = factory->normalPriority;
			}
			conn->thisId = connMap.add( conn );
			conn->peerId = peerId;
			conn->sessionId = 0;
			conn->pool = this;
			conn->guard = guard;
			conn->factoryType = connType;
			conn->server = server;
			conn->serverObject = serverObject;
			guard->_setConnection( conn );
			conn->inPreConnectedMap=true;
			preConns.insert(_PreConnMap::value_type(peerId,conn));
			PTRACE4( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: peerId=%08X added to pre-connection map", peerId);

			if (_recordReplayFile.isOpened())
			{
				_recordReplayFile.writeFile((const BYTE *) &conn->thisId,sizeof(UINT32));
			}

			for(int i=0; i<physAddrs.size(); i++)
			{
				conn->physAddresses.push_back( physAddrs[i] );
			}

			if (msg.flags &_COMM_MSGFLAGS_CONNECTION_EX)
			{
				UINT32 clientId;
				UINT32 serverId;
				if (parser.parseEnded())
				{
					PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error install connection: peerId=%08X server='%s' serverObject='%s' connType='%s' - parse ended", peerId, server, serverObject, connType);
					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
					inter->_post( m );
					_delConn( conn );
					return true;
				}
				parser.parseUINT32(clientId).parseUINT32(serverId);
				if (serverId)
				{
					PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error install connection: peerId=%08X server='%s' serverObject='%s' connType='%s' clientId=%X, serverId=%X not zero", peerId, server, serverObject, connType, clientId, serverId);
					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
					inter->_post( m );
					_delConn( conn );
					return true;
				}
				if (conn->version != CommServerConnectionEx::versionExtended)
				{
					PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: error install connection: peerId=%08X server='%s' serverObject='%s' connType='%s', clientId=%X, serverId=%x - incompatible protocol versions", peerId, server, serverObject, connType, clientId, serverId);
					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect parameters of extended protocol" );
					inter->_post( m );
					_delConn( conn );
					return true;
				}
				CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
				connEx->clientThisId = clientId;
			}
			else
			{
				if (conn->version != CommServerConnectionEx::versionBase)
				{
					PLog( "CommServerConnectionPool: Connection ('%s'/'%s'/'%s') denied (incorrect type of server connection)", server, serverObject, connType );
					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_INCORRECT_PARAMETERS ).composeString( "Incorrect type of serverconnection" );
					inter->_post( m );
					_delConn( conn );
					return true;
				}
			}

			{
				_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED, conn->priority );
				m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId );
				inter->_post( m );
			}
			{
				_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED, conn->priority );
				m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeBYTE( conn->priority ).composeString( guard->currentName() );
				inter->_post( m );
			}

			if (msg.flags &_COMM_MSGFLAGS_CONNECTION_EX)
			{
				CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
				if (connEx->timerHandle)
				{
					inter->_delServerTimer(connEx->timerHandle);
					connEx->timerHandle = 0;
				}
				if (connEx->pollTime > 0)
				{
					_CommMsg tmpMsg(_COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX, 0);
					tmpMsg.body.composeUINT32(conn->thisId).composeUINT32(conn->sessionId);
					connEx->timerHandle = inter->_postClientTimer(tmpMsg, connEx->pollTime);
				}
			}

			PTRACE4( "CommServerConnectionPool: Connection ('%s'/'%s'/'%s') accepted (thisId=%08X) and granted", server, serverObject, connType, conn->thisId );
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_GUARD_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE4( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommServerConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnGuardNegotiationNoSession )
				{
					PLog( "CommServerConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					PASSERT3( conn->guard );
					conn->sessionId = sessionId;
					conn->state = _CommConnection::_StateConnGuardNegotiation;
					conn->guard->processFirstRequest( parser );//IMPORTANT: may destroy conn
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE4( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommServerConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnGuardNegotiation )
				{
					PLog( "CommServerConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					PASSERT3( conn->guard );
					conn->guard->processAddRequest( parser );//IMPORTANT: may destroy conn
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG:
		{
			UINT32 thisId, peerId, sessionId;
			CommMsgParser parser( msg.body );
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			//no trace

			CommServerConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommServerConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					if (conn->version == CommServerConnection::versionExtended)
					{
						CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
						if (connEx->_processUserMessage(parser) == false )
						{
							_delConn( conn );
						}
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

	case _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE4( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommServerConnection* conn = _getConn( thisId, peerId, sessionId, msg.type );
			if( conn )
			{
				conn->_safeClosed( 0, "Connection closed by client side" );
				_delConn( conn );
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_CLIENT_ERROR:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_ERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );

			CommServerConnection* conn = 0;
			if( thisId )
				conn = _getConn( thisId, peerId, msg.type );
			else
			{//special case
				conn = _lookForConn( peerId );
				if( !conn )
				{
					PLog( "CommServerConnectionPool: Unable to find conn for _COMM_MSGTYPE_CONNECT_CLIENT_ERROR with cliId=%08X", peerId );
				}
			}

			if( conn )
			{
				conn->_safeClosed( errCode, errMsg );
				_delConn( conn );
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommServerConnection* conn = 0;
			if( thisId )
				conn = _getConn( thisId, peerId, msg.type );
			else
			{//special case
				conn = _lookForConn( peerId );
				if( !conn )
				{
					PLog( "CommServerConnectionPool: Unable to find conn for _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT with cliId=%08X", peerId );
				}
			}

			if( conn )
			{
				if (conn->version == CommServerConnection::versionExtended && conn->state == _CommConnection::_StateConnOk)
				{
					PTRACE5( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT: extended version");
					CommServerConnectionEx * connEx = static_cast<CommServerConnectionEx *>(conn);
					connEx->inTransDisconnectState = true;
					connEx->disconnectStarted=SYSTEMTICKS();
					if (connEx->timerHandle)
					{
						inter->_delServerTimer(connEx->timerHandle);
						connEx->timerHandle = 0;
					}
					_CommMsg timerMsg(_COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX, 0);
					timerMsg.body.composeUINT32(connEx->thisId);
					connEx->timerHandle = inter->_postClientTimer(timerMsg, connEx->disconnectTimeOut);
					return true;
				}
				conn->_safeClosed(_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
				_delConn( conn );
			}
		}
		return true;
	case _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommServerConnection* conn = 0;
			if( thisId )
				conn = _getConn( thisId, peerId, msg.type );
			else
			{//special case
				conn = _lookForConn( peerId );
				if( !conn )
				{
					PLog( "CommServerConnectionPool: Unable to find conn for _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT with cliId=%08X", peerId );
				}
			}

			if( conn )
			{
				PTRACE5( "CommServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X set true", thisId, peerId );
				conn->closePending = true;
			}
		}
		return true;


	case _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE quality;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( quality );

			CommServerConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommServerConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeQualityUpdated( quality );
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );

			CommServerConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommServerConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeIsAlive();
			}
		}
		return true;

	case _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR:	//should not normally happen, but...
		//means that no 'far' connections were found on the path
		//we will emulate perfect (100) connection with no quality updates
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );

			CommServerConnection* conn = _getConn( thisId, peerId, msg.type );
			if( conn )
			{
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommServerConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE, conn->priority );
					m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeBYTE( 100 );
					inter->_post( m );
				}
			}
		}
		return true;

	}//switch
	return false;
}

void CommServerConnectionPool::disconnect( CommServerConnection* conn, UINT16 errCode, const char * errMessage )
{
	if( conn->state != _CommConnection::_StateConnOk )
	{
		_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_ERROR, conn->priority );
		m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT16( _COMM_ERROR_CLOSEDBYPEER ).composeString( "Connection aborted by server side software during negotiation phase" );
		inter->_post( m );
	}
	else
	{
		_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT, conn->priority );
		m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId );
		if (errCode && errMessage)
			m.body.composeUINT16(errCode).composeString(errMessage);
		inter->_post( m );
	}
	_delConn( conn );
}

CommServerConnectionPool::~CommServerConnectionPool()
{
	deleteAllConnections();
}

void CommServerConnectionPool::deleteAllConnections()
{
	int i;
	/* +++
	for( CommServerConnection** c = connMap.firstIndex( i ); c ; c = connMap.nextIndex( i ) )
	{
	CommServerConnection* conn = *c;
	_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_ERROR, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT16( _COMM_ERROR_SERVERSHUTDOWN ).composeString( "Server shutting down" );
	inter->_post( m );
	}
	*/
	for( CommServerConnection** c = connMap.firstIndex( i ); c ; c = connMap.nextIndex( i ) )
	{
		CommServerConnection* conn = *c;
//		if( conn->zombieStatus == 1 )
//		{
//			conn->pool = 0;
//			conn->zombieStatus = 2;
//		}
//		else
			delete conn;
	}
	connMap.clear();
	preConns.clear();
}


//****************************************************************************

