#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commconn.h"
#include "commcli.h"


inline CommClientGConnection::_Item::_Item( UINT32 _msgId, CommMsgBody& _body, AsyncCall* _call )
: msgId( _msgId )
, call( _call )
{
	patchedBody.moveFrom( _body );
	if(call) 
		call->addRef();
}

inline CommClientGConnection::_Item::~_Item()
{
	if(call)
		call->release();
}

CommClientGConnection::_ConnHandler::_ConnHandler( CommClientGuardFactory& guardFactory_ )
: CommClientConnection( guardFactory_)
{
	gConn = 0;
}

void CommClientGConnection::_ConnHandler::_init( CommClientGConnection* gConn_ )
{
	gConn = gConn_;
}

/* virtual */ void CommClientGConnection::_ConnHandler::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	gConn->_gProcessMessage( msgId, body );
}

/* virtual */ void CommClientGConnection::_ConnHandler::connected()
{
	gConn->_gConnected();
}

///* virtual */ void CommClientGConnection::_ConnHandler::disconnected()
//	{
//	gConn->_safeDisconnected();
//	}

/* virtual */ void CommClientGConnection::_ConnHandler::qualityUpdated( int quality )
{
	gConn->_safeQualityUpdated( quality );
}

///* virtual */ bool CommClientGConnection::_ConnHandler::closed( int errCode, const char* errMsg )
//	{
//	return gConn->_safeClosed( errCode, errMsg );
//	}

/* virtual */ bool CommClientGConnection::_ConnHandler::closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
{
	return gConn->_safeClosed( errCode, errMsg, errMsgBody );
}

//****************************************************************************

void CommClientGConnection::_safeProcessGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* call )
{
	try
	{
		PSIGRETURN()
		processGMessage( requestId, msgId, body, call );
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnection: exception in processGMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, err.why() );
	}
	catch( exception& exc )
	{
		PLog( "CommClientGConnection: STD exception in processGMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, exc.what() );
	}

	catch( ... )
	{
		PLog( "CommClientGConnection: unknown exception in processGMessage (msgId=%d=0x%X) caught", (int)msgId, (int)msgId );
	}
}

void CommClientGConnection::_safeConnected()
{
	try
	{
		PSIGRETURN()
			connected();
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnection: exception in connected() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientGConnection: unknown exception in connected() caught" );
	}
}


void CommClientGConnection::_safeQualityUpdated( int quality )
{
	try
	{
		PSIGRETURN()
			qualityUpdated( quality );
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnection: exception in qualityUpdated() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientGConnection: unknown exception in qualityUpdated() caught" );
	}
}

bool CommClientGConnection::_safeClosed( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
{
	try
	{
		PSIGRETURN()
			return closedOrDisconnected(errCode,errMsg,errMsgBody );
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnection: exception in closed() caught: %s", err.why() );
		return true;
	}
	catch( ... )
	{
		PLog( "CommClientGConnection: unknown exception in closed() caught" );
		return true;
	}
}

/*
void CommClientGConnection::_safeProcessReply( AsyncCall* call, UINT32 requestId, UINT32 msgId, const CommMsgBody& body )
{
try
{
call->processReply( requestId, msgId, body );
}
catch( PError& err )
{
PLog( "CommClientGConnection::AsyncCall: exception in processReply (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, err.why() );
}
catch( ... )
{
PLog( "CommClientGConnection::AsyncCall: unknown exception in processReply (msgId=%d=0x%X) caught", (int)msgId, (int)msgId );
}
}
*/
CommClientGConnection::CommClientGConnection( CommClientGuardFactory& guardFactory_)
: conn( guardFactory_)
{
	conn._init( this );
}

/* virtual */ CommClientGConnection::~CommClientGConnection()
{
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = *it;
		delete item;
	}
}

void CommClientGConnection::_clearAll()
{
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = *it;
		delete item;
	}
	msgList.clear();
	msgMap.clear();
}

UINT32 CommClientGConnection::post( UINT32 msgId, /*non-const!*/ CommMsgBody& body, /* new */ AsyncCall* call )
{
	_Item* item = new _Item( msgId, body, call );
	_List::iterator it = msgList.insert( msgList.end(), item );
	UINT32 requestId = msgMap.add( it );
	BYTE* p = item->patchedBody._createPrefix( 4 );
	CommMsgBody::writeUINT32( p, requestId );
	//p += 4;
	CommMsgBody copy;
	copy.copyFrom( item->patchedBody );
	conn.post( item->msgId, copy );
	return requestId;
}

void CommClientGConnection::cancelPost( UINT32 requestId )
{
	_List::iterator* found = msgMap.find( requestId );
	PASSERT3( found != 0 );

	_List::iterator it = *found;
	_Item* item = *it;
	delete item;
	msgList.erase( it );
	msgMap.remove( requestId );
}

void CommClientGConnection::_gConnected()
{
	if (msgList.size() != 0)
	{
		PTRACE3("CommClientGConnection::_gConnected msgList.size=%u", msgList.size());
	}
	_List::iterator it = msgList.begin();
	while(it != msgList.end())
	{
		_Item* item = *it;
		CommMsgBody copy;
		copy.copyFrom( item->patchedBody );
		if (beforeGConnectedPost(item->msgId, copy))
		{
			conn.post(item->msgId, copy);
			++it;
		}
		else
		{
			CommMsgParser parser( item->patchedBody );
			UINT32 requestId;
			parser.parseUINT32( requestId );
			delete item;
			it = msgList.erase( it );
			msgMap.remove( requestId );
		}
	}
	_safeConnected();
}

void CommClientGConnection::_gProcessMessage( UINT32 msgId, const CommMsgBody& body )
{
	CommMsgParser parser( body );
	UINT32 requestId;
	parser.parseUINT32( requestId );
	if( requestId != 0 )//requestId == 0 means unsolicited server->client message
	{
		_List::iterator* found = msgMap.find( requestId );
		if( found == 0 )
		{
			PLog( "CommClientGConnection: unexpected msg with requestId=%08X (msgId=%08X) - ignored", requestId, msgId );
			return;
		}

		_List::iterator it = *found;
		_Item* item = *it;

		CommMsgBody bodyCopy;
		bodyCopy._moveFromConstBody(body);
		bodyCopy._skipPrefix( 4 );
		_safeProcessGMessage( requestId, msgId, bodyCopy, item->call );
		//			if( item->call->_preserveAfterProcessing )
		//				item->call = 0;//effectively prevents deleting of item->call

		// to fix a problem when _clearAll was called during the processing of AsyncCall
		// once _clearAll was called for connection msgList and msgMap will be empty and
		// items that used to reside in msgMap will no longer exist
		if( msgMap.size() != 0 )
		{
			delete item;
			msgList.erase( it );
			msgMap.remove( requestId );
		}
	}
	else
	{
		CommMsgBody bodyCopy;
		bodyCopy._moveFromConstBody(body);
		bodyCopy._skipPrefix( 4 );
		_safeProcessGMessage( requestId, msgId, bodyCopy, 0 );
	}
}

/* static */ UINT32 CommClientGConnection::_getRequestId( const_iterator it )
{
	const CommMsgBody& patchedBody = (*it)->patchedBody;
	PASSERT3( patchedBody._size() >= 4 );
	return CommMsgBody::readUINT32( patchedBody._readPtr() );
}

/* static */ void CommClientGConnection::_getMsgBody( const_iterator it, CommMsgBody& ret )
{
	ret.copyFrom( (*it)->patchedBody );
	ret._skipPrefix( 4 );
}
