#include "commproxy.h"
#include <exception>

// PYR-38736 #oleksandrb "Moved" from arjel folder

void CommProxyClientConnection::_safeProcessConnectRequestGranted(const char * guardName)
{
	try
	{
		PTRACE4( "CommProxyClientConnection: _safeProcessConnectRequestGranted thisId=%08X", thisId );
		PSIGRETURN()
		// Must create guard!!
		_processConnectRequestGranted(guardName);
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _safeProcessConnectRequestGranted(%08X) caught: %s", thisId, err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _safeProcessConnectRequestGranted(%08X) caught",thisId );
	}
}

void CommProxyClientConnection::_safeProcessGuardAddResponse(CommMsgParser & parser)
{
	try
	{
		PTRACE4( "CommProxyClientConnection: _safeProcessGuardAddResponse thisId=%08X", thisId );
		PSIGRETURN()
		_processGuardAddResponse(parser);
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _safeProcessGuardAddResponse(%08X) caught: %s", thisId,err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _safeProcessGuardAddResponse(%08X) caught", thisId);
	}
}

void CommProxyClientConnection::_safeProcessGuardGranted(CommMsgParser & parser)
{
	try
	{
		PTRACE4( "CommProxyClientConnection: _safeProcessGuardGranted thisId=%08X", thisId );
		PSIGRETURN()
		_processGuardGranted(parser);
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _safeProcessGuardGranted(%08X) caught: %s",thisId, err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _safeProcessGuardGranted(%08X) caught",thisId );
	}
}

void CommProxyClientConnection::_safeError(UINT32 code, UINT16 errCode, const char *errMsg, const CommMsgBody * body)
{
	try
	{
		PTRACE4( "CommProxyClientConnection: _safeError thisId=%08X", thisId );
		PSIGRETURN()
		_processError(code, errCode,errMsg, body);
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _safeError(%08X) caught: %s", thisId,err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _safeError(%08X) caught",thisId );
	}
}


void CommProxyClientConnection::_safeServerHighPriorityTransdisconnect()
{
	try
	{
		PTRACE4( "CommProxyClientConnection: _safeServerHighPriorityTransdisconnect thisId=%08X", thisId );
		PSIGRETURN()
		_processServerHighPriorityTransdisconnect();
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _safeServerHighPriorityTransdisconnect(%08X) caught: %s", thisId,err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _safeServerHighPriorityTransdisconnect(%08X) caught",thisId );
	}
}


void CommProxyClientConnection::_safeProcessMessage( CommMsgBody& body )
{
	try
	{
		PSIGRETURN()
		_processMessage( body );
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in processMessage(%08X) caught: %s", thisId,  err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommProxyClientConnection: STD exception in processMessage(%08X)  caught: %s", thisId, exc.what() );
	}

	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in processMessage(%08X)  caught",thisId);
	}
}

void CommProxyClientConnection::_safeQualityServerMonitor()
{
	try
	{
		PSIGRETURN()
		_processQualityServerMonitor(  );
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _processQualityServerMonitor(%08X) caught: %s", thisId,  err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommProxyClientConnection: STD exception in _processQualityServerMonitor(%08X)  caught: %s", thisId,  exc.what() );
	}

	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _processQualityServerMonitor(%08X)   caught",thisId);
	}
}


void CommProxyClientConnection::_safeRequestIsAlive()
{
	try
	{
		PSIGRETURN()
		_processRequestIsAlive( );
	}
	catch( PError& err )
	{
		PLog( "CommProxyClientConnection: exception in _processRequestIsAlive(%08X) caught: %s", thisId,  err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommProxyClientConnection: STD exception in _processRequestIsAlive(%08X)  caught: %s", thisId, exc.what() );
	}

	catch( ... )
	{
		PLog( "CommProxyClientConnection: unknown exception in _processRequestIsAlive(%08X)   caught",thisId);
	}
}

void CommProxyClientConnection::post( UINT32 msgId, CommMsgBody& body )
{
	if( state != _CommConnection::_StateConnOk )
	{
		PTRACE3( "CommProxyClientConnection: Message to disconnected logical connection thisId=%08X - ignored", thisId );
		return;
	}

	_CommConnection::_wrapUserMsg( body, msgId );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG, priority );
// for eNoneAuth, ePwd, ePwdSid guard is simple
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
	((CommProxyClientConnectionPool *)pool)->_post( msg );
}


void CommProxyClientConnectionPool::_postGuardRequest(CommProxyClientConnection& conn, CommMsgBody & body )
{
	if( conn.state != _CommConnection::_StateConnPeerAssigned )
	{
		PLog( "CommProxyClientConnectionPool: _postGuardRequest for connId=%08X in state=%d - ignored", conn.thisId, conn.state );
		return;
	}

	PTRACE4( "CommProxyClientConnectionPool: Connection thisId=%08X post guard request", conn.thisId );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_REQUEST, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId ).composeUINT32( conn.sessionId );
	m.body.merge(body);
	conn.state = _CommConnection::_StateConnGuardNegotiation;
	inter->_post( m );
}

void CommProxyClientConnectionPool::_clientSideAuthFailed( CommProxyClientConnection& conn )
{
	UINT16 errCode = _COMM_ERROR_AUTHFAILED;
	const char* errMsg = "Client-side authentication failed";
	_postClientError( conn, errCode, errMsg );

	conn.state = _CommConnection::_StateConnClientFatalError;
	PTRACE3( "CommProxyClientConnectionPool: ClientFatalError: %02X, %s", errCode, errMsg );
	conn._clearAll();
	conn._safeClosed( errCode, errMsg );
}

void CommProxyClientConnectionPool::_postGuardAddRequest(CommProxyClientConnection& conn, CommMsgBody & body )
{
	if( conn.state != _CommConnection::_StateConnGuardNegotiation )
	{
		PLog( "CommProxyClientConnectionPool: _postGuardAddRequest for connId=%08X in state=%d - ignored", conn.thisId, conn.state );
		return;
	}

	PTRACE4( "CommProxyClientConnectionPool: Connection thisId=%08X post guard add request ", conn.thisId );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId ).composeUINT32( conn.sessionId );
	m.body.merge(body);
	inter->_post( m );
}

void CommProxyClientConnectionPool::_processGuardGranted(CommProxyClientConnection& conn)
{
	if( conn.state != _CommConnection::_StateConnGuardNegotiation )
	{
		PLog( "CommProxyClientConnectionPool: _processGuardGranted for connId=%08X in state=%d - ignored", conn.thisId, conn.state );
		return;
	}

	PTRACE4( "CommProxyClientConnectionPool: Connection thisId=%08X post guard granted",  conn.thisId );
	conn.state = _CommConnection::_StateConnOk;
//	conn._safeConnected();
}

void CommProxyClientConnectionPool::_postError( CommProxyClientConnection& conn, UINT32 code, UINT16 errCode, const char* errMsg )
{
	_CommMsg m( code, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId );
	if (code != _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT)
		m.body.composeUINT16( errCode ).composeString( errMsg );
	inter->_post( m );
}

void CommProxyClientConnectionPool::_postClientDisconnect( CommProxyClientConnection& conn)
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId ).composeUINT32( conn.sessionId );
	inter->_post( m );
}

void CommProxyClientConnectionPool::_postHighPriorityTransDisconnect( CommProxyClientConnection& conn)
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId );
	inter->_post( m );
}

void CommProxyClientConnectionPool::_postSrvQualityUpdate( CommProxyClientConnection & conn, int quality )
{
	_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId ).composeBYTE( quality );
	inter->_post( m );
}

void CommProxyClientConnectionPool::_postSrvIsAlive( CommProxyClientConnection & conn)
{
	_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE, conn.priority );
	m.body.composeUINT32( conn.peerId ).composeUINT32( conn.thisId );
	inter->_post( m );
}


void CommProxyClientConnectionPool::_removeConnection(CommClientConnection& conn)
{
	if (conn.pool)
	{
		connMap.remove( conn.thisId );
		delete conn.guardAuth;
		conn.guardAuth = 0;
		delete conn.guard;
		conn.guard = 0;
		conn.server = "";
		conn.serverObject = "";
		conn.serverConnType = "";
		conn.pool = 0;
	}
}



void CommProxyClientConnectionPool::_connect( CommClientConnection& conn_ )
{
	if (conn_.version != CommClientConnection::versionProxy)
		return CommClientConnectionPool::_connect( conn_ );

	CommProxyClientConnection & conn = (CommProxyClientConnection & ) conn_; 
	_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST, conn.priority );
	m.body.composeUINT32( conn.thisId ).composeString( conn.server ).composeString( conn.serverObject ).composeString( conn.serverConnType );
	conn.guardFactory->composeGuardNameList( m.body );
	m.body.composeString( "" );//end of guard name list
	conn.root->_composeConnectionList( m.body );
	m.body.composeString( "" );//empty physical connection list
	conn.state = _CommConnection::_StateConnRequestSent;
	conn.peerId = 0;//for special error cases
	conn.closePending = false;
	inter->_post( m );
}



bool CommProxyClientConnectionPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( msg.type == _COMM_MSGTYPE_CONNPOOL_TIMER )
	{
		return CommClientConnectionPool::_processMessage(msg);
	}
	if( msg.type == _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX )
	{
		return CommClientConnectionPool::_processMessage(msg);
	}

	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PTRACE5( "CommProxyClientConnectionPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	switch( msg.type )
	{
		case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
		{
			return CommClientConnectionPool::_processMessage(msg);
		}
		case _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE4( "CommClientConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* _conn = _getConn( thisId, msg.type );
			if( _conn )
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;

				if( conn->state != _CommConnection::_StateConnRequestSent )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->peerId = peerId;
					conn->state = _CommConnection::_StateConnPeerAssigned;
				}
			}
			return true;
		}


		case _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE priority;
			const char* guardName;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority ).parseString( guardName );
			PTRACE3( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED: thisId=%08X peerId=%08X priority=%d", thisId, peerId, priority );

			CommClientConnection* _conn =  _getConn( thisId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;

// For solving problem with priority:
				if (conn->state == _CommConnection::_StateConnRequestSent)
				{
					conn->peerId = peerId;
					conn->state = _CommConnection::_StateConnPeerAssigned;
				}
				if( peerId != conn->peerId )
				{
					PLog( "CommProxyClientConnectionPool: msg type=%02X for connId=%08X with msg.peerId=%08X != conn.peerId=%08X - ignored", msg.type, thisId, peerId, conn->peerId );
					return true;
				}
// end
				if( conn->state != _CommConnection::_StateConnPeerAssigned )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					if( priority >= _COMM_MSGPRIORITIES )
					{
						PLog( "CommProxyClientConnectionPool: invalid priority=%d in _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
					}
					else
						conn->priority = priority;
					++conn->sessionId;
					conn->_safeProcessConnectRequestGranted(guardName);
				}
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE4( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommClientConnection* _conn =  _getConn( thisId, peerId, sessionId, msg.type);
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnGuardNegotiation )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->_safeProcessGuardAddResponse(parser);
				}
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			BYTE priority;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId ).parseBYTE( priority );
			PTRACE4( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED: thisId=%08X peerId=%08X sessionId=%08X priority=%d", thisId, peerId, sessionId, priority );

			CommClientConnection* _conn =  _getConn( thisId, peerId, sessionId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnGuardNegotiation )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					if( priority >= _COMM_MSGPRIORITIES )
					{
						PLog( "CommProxyClientConnectionPool: invalid priority=%d in _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
					}
					else
						conn->priority = priority;
					conn->_safeProcessGuardGranted(parser);
				}
			}
			return true;
		}

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
			PTRACE3( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommClientConnection* _conn =  _getConn( thisId, peerId, sessionId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->state = _CommConnection::_StateConnServerClosed;
					conn->_clearAll();
					conn->_safeError( _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT, errCode, errMsg,0);
				}
			}
			return true;
		}

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

			PTRACE3( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );

			CommClientConnection* _conn = 0;
			if( peerId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
				_conn = _getConn( thisId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				conn->state = _CommConnection::_StateConnServerFatalError;
				conn->_clearAll();
				conn->_safeError( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, errCode, errMsg, (hasErrBody ? &errMsgBody : 0) );
			}
			return true;
		}


		case _COMM_MSGTYPE_CONNECT_SERVER_ERROR:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			PTRACE3( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_ERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );
			
			CommClientConnection* _conn = 0;
			if( peerId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
				_conn = _getConn( thisId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);

				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				conn->state = _CommConnection::_StateConnDisc;
				conn->_clearAll();
				conn->_safeError(_COMM_MSGTYPE_CONNECT_SERVER_ERROR,errCode,errMsg,0);
			}
			return true;
		}


		case _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* _conn = 0;
			if( peerId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
				_conn = _getConn( thisId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				conn->state = _CommConnection::_StateConnPeerAssigned;
				conn->_clearAll();
				conn->_safeError(_COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT,0,"",0);
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE4( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* _conn = 0;
			if( peerId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
				_conn = _getConn( thisId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				PTRACE5( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X set true", thisId, peerId );
				conn->_safeServerHighPriorityTransdisconnect();
				conn->closePending=true;
			}
			return true;
		}
		case _COMM_MSGTYPE_CONNECT_SERVER_USERMSG:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			//no trace

			CommClientConnection* _conn =  _getConn( thisId, peerId, sessionId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_SERVER_USERMSG msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->_safeProcessMessage( msg.body );
				}
			}
			return true;
		}
		case _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR:	
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE4( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* _conn = 0;
			_conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				PTRACE5( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR: thisId=%08X peerId=%08X set true", thisId, peerId );
				conn->_safeQualityServerMonitor();
			}
			return true;
		}
		case _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE quality;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( quality );

			CommClientConnection* _conn =  _getConn( thisId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyClientConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeQualityUpdated( quality );
			}
			return true;
		}
		case _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE4( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientConnection* _conn = 0;
			_conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommClientConnection::versionProxy)
					return CommClientConnectionPool::_processMessage(msg);
				CommProxyClientConnection* conn = (CommProxyClientConnection*) _conn;
				PTRACE5( "CommProxyClientConnectionPool: _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE: thisId=%08X peerId=%08X set true", thisId, peerId );
				conn->_safeRequestIsAlive();
			}
			return true;
		}



	}//switch
	return false;
}

bool CommProxyServerConnection::_safeProcessConnectRequest()
{
	try
	{
		PTRACE4( "CommProxyServerConnection: _safeProcessConnectRequest thisId=%08X", thisId );
		PSIGRETURN()
		// MUST CREATE GUARD
		return _processConnectRequest();
	}
	catch( PError& err )
	{
		PLog( "CommProxyServerConnection: exception in processConnectRequest(%08X) caught: %s", thisId,err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyServerConnection: unknown exception in processConnectRequest(%08X) caught",thisId );
	}
	return false;
}


void CommProxyServerConnection::_safeError(UINT32 code, UINT16 errCode, const char *errMsg)
{
	try
	{
		PTRACE4( "CommProxyServerConnection: _safeError thisId=%08X", thisId );
		PSIGRETURN()
		_processError(code, errCode,errMsg);
	}
	catch( PError& err )
	{
		PLog( "CommProxyServerConnection: exception in _safeError(%08X) caught: %s", thisId,err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyServerConnection: unknown exception in _safeError(%08X) caught",thisId );
	}
}


void CommProxyServerConnection::_safeClientHighPriorityTransdisconnect()
{
	try
	{
		PTRACE4( "CommProxyServerConnection: _safeClientHighPriorityTransdisconnect thisId=%08X", thisId );
		PSIGRETURN()
		_processClientHighPriorityTransdisconnect();
	}
	catch( PError& err )
	{
		PLog( "CommProxyServerConnection: exception in _safeClientHighPriorityTransdisconnect(%08X) caught: %s", thisId, err.why() );
	}
	catch( ... )
	{
		PLog( "CommProxyServerConnection: unknown exception in _safeClientHighPriorityTransdisconnect(%08X) caught",thisId );
	}
}

void CommProxyServerConnection::_safeProcessMessage( CommMsgBody& body )
{
	try
	{
		PSIGRETURN()
		_processMessage( body );
	}
	catch( PError& err )
	{
		PLog( "CommProxyServerConnection: exception in processMessage(%08X) caught: %s", thisId,  err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommProxyServerConnection: STD exception in processMessage(%08X) caught: %s", thisId,  exc.what() );
	}

	catch( ... )
	{
		PLog( "CommProxyServerConnection: unknown exception in processMessage(%08X) caught",thisId);
	}
}

void CommProxyServerConnection::_safeQualityClientMonitor()
{
	try
	{
		PSIGRETURN()
		_processQualityClientMonitor(  );
	}
	catch( PError& err )
	{
		PLog( "CommProxyServerConnection: exception in _processQualityClientMonitor(%08X) caught: %s", thisId,  err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommProxyServerConnection: STD exception in _processQualityClientMonitor(%08X) caught: %s",  thisId, exc.what() );
	}

	catch( ... )
	{
		PLog( "CommProxyServerConnection: unknown exception in _processQualityClientMonitor(%08X) caught",thisId);
	}
}


void CommProxyServerConnection::post( UINT32 msgId, CommMsgBody& body )
{
	PASSERT3( pool );
	if( state != _CommConnection::_StateConnOk )
	{
		PLog( "CommServerConnectionPool: Message to disconnected logical connection thisId=%08X - ignored", thisId );
		return;
	}
// for eNoneAuth, ePwd, ePwdSid guard is simple
	_CommConnection::_wrapUserMsg( body, msgId );
	_CommMsg msg( _COMM_MSGTYPE_CONNECT_SERVER_USERMSG, priority );
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
	((CommProxyServerConnectionPool *)pool)->_post( msg );
}



void CommProxyServerConnectionPool::_postRequestGranted(CommProxyServerConnection* conn )
{
	if( conn->state != _CommConnection::_StateConnPeerAssigned )
	{
		PLog( "CommProxyServerConnectionPool: _postRequestGranted for connId=%08X in state=%d - ignored",conn->thisId, conn->state );
		return;
	}

	conn->state = _CommConnection::_StateConnGuardNegotiationNoSession;
	PTRACE4( "CommProxyServerConnectionPool: Connection thisId=%08X granted ", conn->thisId );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeBYTE( conn->priority ).composeString( conn->guard->currentName() );
	inter->_post( m );
}

void CommProxyServerConnectionPool::_postNoAcceptableGuard(CommProxyServerConnection* conn )
{
	if( conn->state != _CommConnection::_StateConnPeerAssigned )
	{
		PLog( "CommProxyServerConnectionPool: _postNoAcceptableGuard for connId=%08X  in state=%d - ignored", conn->id(), conn->state );
		return;
	}

	PLog( "CommProxyServerConnectionPool: Connection thisId=%08X denied (no acceptable guard)",  conn->thisId  );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT16( _COMM_ERROR_NOACCEPTABLEGUARD ).composeString( "No acceptable guard" );
	inter->_post( m );
}


void CommProxyServerConnectionPool::_postGuardAddResponse( CommProxyServerConnection* conn, const CommMsgBody& body )
{
	if( conn->state != _CommConnection::_StateConnGuardNegotiation )
	{
		PLog( "CommProxyServerConnectionPool: _postGuardAddResponse for connId=%08X in state=%d - ignored", conn->thisId, conn->state );
		return;
	}
	_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId );
	m.body.merge( body );
	inter->_post( m );
}

void CommProxyServerConnectionPool::_postGuardRequestGranted( CommProxyServerConnection* conn, const CommMsgBody& body, const CommMsgBody* infoMsgBody /* PYR-27345 */)
{
	if( conn->state != _CommConnection::_StateConnGuardNegotiation )
	{
		PLog( "CommProxyServerConnectionPool: _postGuardRequestGranted for connId=%08X in state=%d - ignored", conn->thisId, conn->state );
		return;
	}

	conn->state = _CommConnection::_StateConnOk;
//	conn->_safeConnected();
	conn->priority = conn->normalPriority;
	_CommMsg m( _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId ).composeBYTE( conn->priority );
	m.body.merge( body );
	if( infoMsgBody )
	{
		m.body.composeMsgBody( *infoMsgBody );	// PYR-27345
	}
	inter->_post( m );
}

void CommProxyServerConnectionPool::_postError( CommProxyServerConnection* conn, UINT32 code, UINT16 errCode, const char* errMsg, const CommMsgBody * body )
{
	_CommMsg m( code, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId );
	if (code != _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT)
	{
		m.body.composeUINT16( errCode ).composeString( errMsg );
		if (body != 0)
			m.body.composeMsgBody(*body);
	}
	inter->_post( m );
}

void CommProxyServerConnectionPool::_postServerDisconnect( CommProxyServerConnection* conn, UINT32 code, UINT16 errCode, const char* errMsg)
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeUINT32( conn->sessionId );
	if (errCode && errMsg )
	{
		m.body.composeUINT16(errCode).composeString(errMsg);
	}
	inter->_post( m );
}

void CommProxyServerConnectionPool::_postHighPriorityTransDisconnect( CommProxyServerConnection * conn)
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId );
	inter->_post( m );
}

void CommProxyServerConnectionPool::_postCliQualityUpdate( CommProxyServerConnection * conn, int quality )
{
	_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE, conn->priority );
	m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId ).composeBYTE( quality );
	inter->_post( m );
}


bool CommProxyServerConnectionPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PTRACE3( "CommProxyServerConnectionPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	if( msg.type == _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX )
	{
		return CommServerConnectionPool::_processMessage(msg);
	}
	switch( msg.type )
	{
		case _COMM_MSGTYPE_PHYSICAL_CONNECT:
			return CommServerConnectionPool::_processMessage(msg);
		case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
			return CommServerConnectionPool::_processMessage(msg);

		case _COMM_MSGTYPE_CONNECT_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 peerId;
			const char* server;
			const char* serverObject;
			const char* connType;
			parser.parseUINT32( peerId ).parseString( server ).parseString( serverObject ).parseString( connType );
			PTRACE3( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: peerId=%08X server='%s' serverObject='%s' connType='%s'", peerId, server, serverObject, connType );
			

			_FactoryMap::iterator found = factoryMap.find( connType );
			CommProxyConnectionFactory* factory = 0;
			if (found != factoryMap.end())
			{
				return CommServerConnectionPool::_processMessage(msg);
			}
			else
			{
				if (defaultFactory)
					factory = defaultFactory;
				else
				{
					PLog( "CommProxyServerConnectionPool: Connection ('%s'/'%s'/'%s') denied (unknown connType)", server, serverObject, connType );
					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOCONNTYPE ).composeString( "Unknown connection type" );
					inter->_post( m );
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

			PString logStr;
			for (int i = 0; i < guardNames.size(); ++i)
			{
				logStr.append(i > 0 ? "," : "").append(guardNames[i]);
			}
			PTRACE5("CommProxyServerConnectionPool: guards[#%u]:%s", guardNames.size(), logStr.c_str());
			CommServerGuard* guard = factory->createGuard( guardNames );
			if( guard == 0 )
			{
				PLog( "CommProxyServerConnectionPool: Connection ('%s'/'%s'/'%s') denied (no acceptable guard)", server, serverObject, connType );
				_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
				m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOACCEPTABLEGUARD ).composeString( "No acceptable guard" );
				inter->_post( m );
				return true;
			}
			CommProxyServerConnection* conn = factory->createConnection();
			conn->state = _CommConnection::_StateConnPeerAssigned;
			if (msg.priority != _COMM_MSGPRIORITY_INITCONNECT)
			{
				// Client requests priority different from default.
				// Pass this priority to the real server
				conn->priority = msg.priority;
				conn->normalPriority = msg.priority;
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
			PTRACE4( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_REQUEST: peerId=%08X added to pre-connection map", peerId);

			for(;;)
			{
				const char* physAddr;
				parser.parseString( physAddr );
				if( *physAddr == 0 )
					break;
				PTRACE5( "CommProxyServerConnectionPool: Incoming connection via '%s'", physAddr );
				conn->physAddresses.push_back( physAddr );
			}

			{
			_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED, conn->priority );
			m.body.composeUINT32( conn->peerId ).composeUINT32( conn->thisId );
			inter->_post( m );
			}
			PTRACE4( "CommProxyServerConnectionPool: Connection ('%s'/'%s'/'%s') accepted (thisId=%08X)", server, serverObject, connType, conn->thisId );
			if (conn->_safeProcessConnectRequest() == false)
			{
				_delConn( conn );
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNECT_GUARD_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE4( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnGuardNegotiationNoSession )
				{
					PLog( "CommProxyServerConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->sessionId = sessionId;
					conn->state = _CommConnection::_StateConnGuardNegotiation;
					conn->guard->processFirstRequest( parser );//IMPORTANT: may destroy conn
				}
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE4( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnGuardNegotiation )
				{
					PLog( "CommProxyServerConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->guard->processAddRequest( parser );//IMPORTANT: may destroy conn
				}
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG:
		{
			UINT32 thisId, peerId, sessionId;
			CommMsgParser parser( msg.body );
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			//no trace

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyServerConnectionPool: unexpected _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
				{
					conn->_safeProcessMessage( msg.body );
				}
			}
			return true;
		}
	

		case _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId, sessionId;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
			PTRACE3( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;

				conn->_safeError(_COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT,0,"");
			}
			return true;		
		}
		
		case _COMM_MSGTYPE_CONNECT_CLIENT_ERROR:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			PTRACE3( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_ERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );

			CommServerConnection* _conn = 0;
			if( thisId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
			{//special case
				_conn = _lookForConn( peerId );
				if( !_conn )
				{
					PLog( "CommProxyServerConnectionPool: Unable to find conn for _COMM_MSGTYPE_CONNECT_CLIENT_ERROR with cliId=%08X", peerId );
				}
			}

			if( _conn )
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				conn->_safeError(_COMM_MSGTYPE_CONNECT_CLIENT_ERROR, errCode, errMsg );
			}
			return true;
		}
	
		case _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommServerConnection* _conn = 0;
			if( thisId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
			{//special case
				_conn = _lookForConn( peerId );
				if( !_conn )
				{
					PLog( "CommProxyServerConnectionPool: Unable to find conn for _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT with cliId=%08X", peerId );
				}
			}

			if( _conn )
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);

				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				conn->_safeError(_COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT,0,"");
			}
			return true;
		}
		case _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE5( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommServerConnection* _conn = 0;
			if( thisId )
				_conn = _getConn( thisId, peerId, msg.type );
			else
			{//special case
				_conn = _lookForConn( peerId );
				if( !_conn )
				{
					PLog( "CommProxyServerConnectionPool: Unable to find conn for _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT with cliId=%08X", peerId );
				}
			}

			if( _conn )
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				conn->_safeClientHighPriorityTransdisconnect();
				PTRACE5( "CommProxyServerConnectionPool: _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X set true", thisId, peerId );
				conn->closePending = true;
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE quality;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( quality );

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;

				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyServerConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeQualityUpdated( quality );
			}
			return true;
		}
		case _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyServerConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeIsAlive();
			}
			return true;
		}

		case _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR:	
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );

			CommServerConnection* _conn = _getConn( thisId, peerId, msg.type );
			if (_conn)
			{
				if (_conn->version != CommServerConnection::versionProxy)
					return CommServerConnectionPool::_processMessage(msg);
				CommProxyServerConnection* conn = (CommProxyServerConnection*) _conn;
				if( conn->state != _CommConnection::_StateConnOk )
				{
					PLog( "CommProxyServerConnectionPool: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
				}
				else
					conn->_safeQualityClientMonitor();
			}
			return true;

		}
	}//switch
	return false;
}
