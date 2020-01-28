#include "commproxy.h"

// PYR-38736 #oleksandrb "Moved" from arjel folder


CommProxyServerConnection::CommProxyServerConnection( CommProxyConnection & root_)
{
	version = CommServerConnection::versionProxy;
	root = &root_;
}



bool CommProxyServerConnection::_processConnectRequest()
{
	_print("_processConnectRequest()");
	return root->obj._serverProcessConnectRequest(root);
}

void CommProxyServerConnection::_processMessage(CommMsgBody & body)
{
	_print("_processMessage");
	root->obj._processServerMessage(root,body);
}

void CommProxyServerConnection::_processError(UINT32 code, UINT16 errCode, const char *errMsg)
{
	_print("_processError");
	root->obj._serverProcessError(root, code, errCode, errMsg);
}
void CommProxyServerConnection::_processClientHighPriorityTransdisconnect()
{
	_print("_processClientHighPriorityTransdisconnect");
	root->obj._serverProcessServerHighPriorityTransdisconnect(root);
}

void CommProxyServerConnection::_processQualityClientMonitor()
{
	_print("_processQualityClientMonitor");
	root->obj._processQualityClientMonitor(root);
}

void CommProxyServerConnection::qualityUpdated( int quality )
{
	_print("qualityUpdated");
	root->obj._serverQualityUpdated(root,quality);
}

void CommProxyServerConnection::isAlive(  )
{
	_print("isAlive");
	root->obj._isAlive(root);
}



CommProxyClientConnection::CommProxyClientConnection(CommProxyConnection & root_, CommClientGuardFactory & guardFactory)
	: CommClientConnection(guardFactory)
{
	version = CommClientConnection::versionProxy;
	root = &root_;
	priority = root->serverConn->getPriority();
}

void CommProxyClientConnection::_processConnectRequestGranted(const char * guardName)
{
	_print("_processConnectRequestGranted");
	root->obj._clientProcessConnectRequestGranted(root,guardName);
}

void CommProxyClientConnection::_processGuardAddResponse(CommMsgParser & parser)
{
	_print("_processGuardAddResponse");
	root->obj._clientProcessGuardAddResponse(root,parser);
}

void CommProxyClientConnection::_processMessage(CommMsgBody & body)
{
	_print("_processMessage");
	root->obj._processClientMessage(root,body);
}

void CommProxyClientConnection::_processQualityServerMonitor()
{
	_print("_processQualityServerMonitor");
	root->obj._processQualityServerMonitor(root);
}

void CommProxyClientConnection::_processRequestIsAlive()
{
	_print("_processRequestIsAlive");
	root->obj._processRequestIsAlive(root);
}


void CommProxyClientConnection::_processGuardGranted(CommMsgParser & parser)
{
	_print("_processGuardGranted");
	root->obj._clientProcessGuardGranted(root,parser);
}

void CommProxyClientConnection::_processError(UINT32 code, UINT16 errCode, const char *errMsg, const CommMsgBody * errBody)
{
	_print("_processError");
	root->obj._clientProcessError(root, code, errCode, errMsg, errBody);
}
void CommProxyClientConnection::_processServerHighPriorityTransdisconnect()
{
	_print("_processServerHighPriorityTransdisconnect");
	root->obj._clientProcessServerHighPriorityTransdisconnect(root);
}

void CommProxyClientConnection::qualityUpdated( int quality )
{
	_print("qualityUpdated");
	root->obj._clientQualityUpdated(root,quality);
}






CommProxyConnection::CommProxyConnection(CommProxyConnectionObject & obj_ )
	: obj(obj_)
{
	rootId = 0;
	serverConn = 0;
	clientConn = 0;
	userData = 0;
	proxyClientGuardFactory.rootConn=this;
	rootId = obj.connMap.add(this);
	authenticatedByUserIntId = false; // PYR-42014
}

CommProxyConnectionFactory::CommProxyConnectionFactory(CommServerGuardFactory & guardFactory )
: CommServerConnectionFactory( guardFactory )
{
	obj = 0;
}

CommProxyServerConnection*  CommProxyConnectionFactory::createConnection() 
{
	CommProxyConnection * c = new CommProxyConnection(*obj);
	c->serverConn = new CommProxyServerConnection(*c);
	return c->serverConn;
}

void CommProxyConnectionObject::remove(UINT32 rootId)
{
	CommProxyConnection **cPtr = connMap.find(rootId);
	if (cPtr)
	{
		connMap.remove(rootId);
		delete *cPtr;
	}
	else
	{
		PTRACE3("CommProxyConnectionObject::remove: rootId %x not found", rootId);
	}

}

CommProxyConnectionObject::CommProxyConnectionObject(_CommInterface& inter)
		: CommServerObject( inter ),
		  connProxyFactory(proxyServerGuardFactory),
		  srvPool( inter ),
		  cliPool( inter)
{
	connProxyFactory.obj = this;
	srvPool.setDefaultFactory( connProxyFactory );
}






bool CommProxyConnectionObject::_serverProcessConnectRequest(CommProxyConnection * proxyConn)
{
	PASSERT3(proxyConn->clientConn == 0);
	proxyConn->proxyClientGuardFactory.setServerGuard((CommProxyServerGuard *) proxyConn->serverConn->getGuard());
	proxyConn->clientConn = new CommProxyClientConnection(*proxyConn,proxyConn->proxyClientGuardFactory);
	cliPool.connect(*proxyConn->clientConn,proxyConn->getServer(),proxyConn->getServerObject(),proxyConn->getServerConnType());
	PTRACE3("New proxy connection (%s->%s/%s/%s), id=%08X, clientId=%08X, serverId=%08X",
		(proxyConn->serverConn->connPathLen() > 0 ? proxyConn->serverConn->connPathPoint(0) : "Unknown"),
		 proxyConn->getServer(),proxyConn->getServerObject(),proxyConn->getServerConnType(), proxyConn->id(), proxyConn->clientConn->id(),proxyConn->serverConn->id());
	return true;
}


void CommProxyConnectionObject::_clientProcessConnectRequestGranted(CommProxyConnection * proxyConn, const char * guardName)
{
	bool ok = ((CommProxyServerGuard*) proxyConn->serverConn->getGuard())->checkGuard(guardName);
	if (ok)
	{
		proxyConn->serverConn->_setPriority(proxyConn->clientConn->_getPriority());
		srvPool._postRequestGranted(proxyConn->serverConn);
	}
	else
	{
		srvPool._postNoAcceptableGuard(proxyConn->serverConn);
		cliPool.disconnect(*proxyConn->clientConn);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		remove(proxyConn->id());
	}
}

void CommProxyConnectionObject::_clientProcessGuardAddResponse(CommProxyConnection * proxyConn, CommMsgParser & parser)
{
	CommMsgBody body;
	body.copyMsgBodyFromParser(parser);
	srvPool._postGuardAddResponse(proxyConn->serverConn, body);
}

void CommProxyConnectionObject::_clientProcessGuardGranted(CommProxyConnection * proxyConn, CommMsgParser & parser)
{

	CommMsgBody body;
	// parse and recompose encryptedUser for encrypted psw guards
	const CommProxyServerGuard * guard = (const CommProxyServerGuard*) proxyConn->serverConn->getGuard();
	const PString guardName = guard->currentName();
	if (guardName.equals(_COMM_ENCRYPTEDPASSWORD_GUARD_NAME) ||
		guardName.equals(_COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME))
	{
		const BYTE* p;
		size_t sz;
		parser._parseVarBlock(p, sz);
		body._composeVarBlock(p, sz);
	}
	else if (guardName.equals(_COMM_WEBTOKEN_GUARD_NAME) ||
			guardName.equals(_COMM_TOKEN_GUARD_NAME) ||
			guardName.equals(_COMM_WEBTOKENINT_GUARD_NAME))
	{
		if (guard->version >= 2)
		{
			INT32 peerVersion;
			parser.parseINT32(peerVersion);
			body.composeINT32(peerVersion);
			const BYTE * p;
			size_t len;
			parser._parseVarBlock(p,len);
			body._composeVarBlock(p,len);
		}
	}

	CommMsgParser infoParser = parser;
	// for eNoneAuth, ePwd, ePwdSid client athentication is always OK
	if (!infoParser.parseEnded())
	{
		CommMsgBody infoMsgBody;
		infoParser.parseMsgBody(infoMsgBody);
		srvPool._postGuardRequestGranted(proxyConn->serverConn,body, &infoMsgBody);
	}
	else
	{
		srvPool._postGuardRequestGranted(proxyConn->serverConn,body, 0 /* infoMsgBody */);
	}
	
	cliPool._processGuardGranted(*proxyConn->clientConn);
	processClientConnected(proxyConn, parser);
}


void CommProxyConnection::_composeConnectionList(CommMsgBody & body)
{
	for (int i=0; i < serverConn->connPathLen() ;i++)
	{
		body.composeString(serverConn->connPathPoint(i));
	}
}


void CommProxyConnectionObject::_clientPostGuardRequest(CommProxyConnection * proxyConn, CommMsgBody & body)
{
	cliPool._postGuardRequest(*proxyConn->clientConn,body);
}

void CommProxyConnectionObject::_clientPostGuardAddRequest(CommProxyConnection * proxyConn, CommMsgBody & body)
{
	cliPool._postGuardAddRequest(*proxyConn->clientConn,body);
}

void CommProxyConnectionObject::_processClientMessage(CommProxyConnection * proxyConn, CommMsgBody & body)
{
	// for eNoneAuth, ePwd, ePwdSid guard is simple
	body._skipPrefix(4 * sizeof(UINT32));
	UINT32 msgId;
	_CommConnection::_unwrapUserMsg( msgId, body );
	processClientMessage( msgId, body, proxyConn );
}


void CommProxyConnectionObject::_processQualityServerMonitor(CommProxyConnection * proxyConn)
{
	proxyConn->serverConn->postQMonitorRequest();
}

void CommProxyConnectionObject::_processQualityClientMonitor(CommProxyConnection * proxyConn)
{
	proxyConn->clientConn->postQMonitorRequest();
}

void CommProxyConnectionObject::_processRequestIsAlive(CommProxyConnection * proxyConn)
{
	proxyConn->serverConn->monitorIsAlive();
}



void CommProxyConnectionObject::_processServerMessage(CommProxyConnection * proxyConn, CommMsgBody & body)
{
	// for eNoneAuth, ePwd, ePwdSid guard is simple.
	body._skipPrefix(4 * sizeof(UINT32));
	UINT32 msgId;
	_CommConnection::_unwrapUserMsg( msgId, body );
	processServerMessage( msgId, body, proxyConn );
}



CommProxyConnection::~CommProxyConnection()
{
	if (clientConn)
	{
		PLog(" rootId %08X has valid client pointer!", rootId);
		delete clientConn;
	}
	if (serverConn)
	{
		PLog(" rootId %08X has valid client pointer!", rootId);
		delete serverConn;
	}

	if (userData)
		delete userData;
}

void CommProxyConnectionObject::_clientProcessError(CommProxyConnection * proxyConn, UINT32 code, UINT16 errCode, const char *errMsg, const CommMsgBody * errBody)
{
	if (code == _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT)
	{
		srvPool._postServerDisconnect(proxyConn->serverConn,code,errCode,errMsg);
		processClientDisconnected(proxyConn,errCode,errMsg);
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		cliPool._removeConnection(*proxyConn->clientConn);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		remove(proxyConn->id());
		return;
	}
	if (code == _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR)
	{
		srvPool._postError(proxyConn->serverConn,code,errCode,errMsg,errBody);
		processClientDisconnected(proxyConn,errCode,errMsg);
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		cliPool._removeConnection(*proxyConn->clientConn);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		remove(proxyConn->id());

		return;
	}
	if (code == _COMM_MSGTYPE_CONNECT_SERVER_ERROR)
	{
		srvPool._postError(proxyConn->serverConn,code,errCode,errMsg,errBody);
		processClientDisconnected(proxyConn,errCode,errMsg);
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		cliPool._removeConnection(*proxyConn->clientConn);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		remove(proxyConn->id());
		return;
	}
	if (code == _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT)
	{
		srvPool._postError(proxyConn->serverConn,code,errCode,errMsg,errBody);
		proxyConn->serverConn->_setState(_CommConnection::_StateConnPeerAssigned);
		proxyConn->clientConn->_setState(_CommConnection::_StateConnPeerAssigned);
		processClientDisconnected(proxyConn,_COMM_ERROR_PHYSICAL_DISCONNECTED,"");
		return;
	}
	else
		PASSERT3(0);
}

void CommProxyConnectionObject::_clientProcessServerHighPriorityTransdisconnect(CommProxyConnection * proxyConn)
{
	srvPool._postHighPriorityTransDisconnect(proxyConn->serverConn);
}

void CommProxyConnectionObject::_serverProcessServerHighPriorityTransdisconnect(CommProxyConnection * proxyConn)
{
	cliPool._postHighPriorityTransDisconnect(*proxyConn->clientConn);
}

void CommProxyConnectionObject::_clientQualityUpdated(CommProxyConnection * proxyConn,int quality)
{
	srvPool._postCliQualityUpdate(proxyConn->serverConn,quality);
}

void CommProxyConnectionObject::_serverQualityUpdated(CommProxyConnection * proxyConn,int quality)
{
	cliPool._postSrvQualityUpdate(*proxyConn->clientConn,quality);
}

void CommProxyConnectionObject::_isAlive(CommProxyConnection * proxyConn)
{
	cliPool._postSrvIsAlive(*proxyConn->clientConn);
}


void CommProxyConnectionObject::_serverProcessError(CommProxyConnection * proxyConn, UINT32 code, UINT16 errCode, const char *errMsg)
{
	if (code == _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT)
	{
		cliPool._postClientDisconnect(*proxyConn->clientConn);
		cliPool._removeConnection(*proxyConn->clientConn);
		processClientDisconnected(proxyConn,errCode,errMsg);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		remove(proxyConn->id());
		return;
	}
	if (code == _COMM_MSGTYPE_CONNECT_CLIENT_ERROR)
	{
		cliPool._postError(*proxyConn->clientConn,code,errCode,errMsg);
		cliPool._removeConnection(*proxyConn->clientConn);
		processClientDisconnected(proxyConn,errCode,errMsg);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		remove(proxyConn->id());
		return;
	}
	if (code == _COMM_MSGTYPE_CONNECT_SERVER_ERROR)
	{
		cliPool._postError(*proxyConn->clientConn,code,errCode,errMsg);
		cliPool._removeConnection(*proxyConn->clientConn);
		processClientDisconnected(proxyConn,errCode,errMsg);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		remove(proxyConn->id());
		return;
	}
	if (code == _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT)
	{
		cliPool._postError(*proxyConn->clientConn,code,errCode,errMsg);
		cliPool._removeConnection(*proxyConn->clientConn);
		processClientDisconnected(proxyConn,errCode,errMsg);
		delete proxyConn->clientConn;
		proxyConn->clientConn = 0;
		srvPool._delConn(proxyConn->serverConn);
		proxyConn->serverConn = 0;
		remove(proxyConn->id());
		return;
	}
}


CommProxyServerConnection::~CommProxyServerConnection(  )
{
}


CommProxyClientConnection::~CommProxyClientConnection()
{
}


// TODO: rename @ cleanup
// post via client connection to server - rename function to postFromClientConnToServer
void CommProxyConnection::postToClient(UINT32 msgId, CommMsgBody & body)
{
	serverConn->_print("postToClient");
	PTRACE5("Messsage %d posted", msgId);
	clientConn->post(msgId,body);
}

// post via server connection to client - rename function to postFromServerConnToClient
void CommProxyConnection::postToServer(UINT32 msgId, CommMsgBody & body)
{
	serverConn->_print("postToServer");
	PTRACE5("Messsage %d posted", msgId);
	serverConn->post(msgId,body);
}

void CommProxyConnectionObject::processClientConnected( CommProxyConnection* conn, CommMsgParser& /*parser*/ )
{
		PTRACE3("processClientConnected id=%08X: %s->%s/%s/%s", conn->id(), 
			(conn->serverConn->connPathLen() > 0 ? conn->serverConn->connPathPoint(0) : "Unknown"),
			conn->getServer(),conn->getServerObject(),conn->getServerConnType());
}

void CommProxyConnectionObject::processClientDisconnected( CommProxyConnection* conn ,INT16 errCode, const char * errMsg) 
{
	PTRACE3("processClientDisconnected id=%08X: %s->%s/%s/%s", conn->id(), 
		(conn->serverConn->connPathLen() > 0 ? conn->serverConn->connPathPoint(0) : "Unknown"),
		conn->getServer(),conn->getServerObject(),conn->getServerConnType());
	if (errCode)
		PTRACE3(" - %d %s", errCode,errMsg);
}
