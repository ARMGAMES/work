#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commconn.h"
#include "commcli.h"


inline CommClientGConnectionEx::_Item::_Item( UINT32 _msgId, CommMsgBody& _body, AsyncCall* _call )
: msgId( _msgId )
, call( _call )
{
	patchedBody.moveFrom( _body );
	if(call) 
		call->addRef();
}

inline CommClientGConnectionEx::_Item::~_Item()
{
	if(call)
		call->release();
}

CommClientGConnectionEx::_ConnHandler::_ConnHandler( CommClientGuardFactory& guardFactory_ )
: CommClientConnectionEx( guardFactory_)
{
	gConn = 0;
}

void CommClientGConnectionEx::_ConnHandler::_init( CommClientGConnectionEx* gConn_ )
{
	gConn = gConn_;
}

/* virtual */ void CommClientGConnectionEx::_ConnHandler::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	gConn->_gProcessMessage( msgId, body );
}

/* virtual */ void CommClientGConnectionEx::_ConnHandler::connected()
{
	gConn->_gConnected();
}

///* virtual */ void CommClientGConnectionEx::_ConnHandler::disconnected()
//	{
//	gConn->_safeDisconnected();
//	}

/* virtual */ void CommClientGConnectionEx::_ConnHandler::qualityUpdated( int quality )
{
	gConn->_safeQualityUpdated( quality );
}

///* virtual */ bool CommClientGConnectionEx::_ConnHandler::closed( int errCode, const char* errMsg )
//	{
//	return gConn->_safeClosed( errCode, errMsg );
//	}

/* virtual */ bool CommClientGConnectionEx::_ConnHandler::closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
{
	return gConn->_safeClosed( errCode, errMsg, errMsgBody );
}

//****************************************************************************

void CommClientGConnectionEx::_safeProcessGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, CommClientGConnectionEx::AsyncCall* call )
{
	try
	{
		PSIGRETURN()
			processGMessage( requestId, msgId, body, call );
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnectionEx: exception in processGMessage (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientGConnectionEx: unknown exception in processGMessage (msgId=%d=0x%X) caught", (int)msgId, (int)msgId );
	}
}

void CommClientGConnectionEx::_safeConnected()
{
	try
	{
		PSIGRETURN()
			connected();
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnectionEx: exception in connected() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientGConnectionEx: unknown exception in connected() caught" );
	}
}


void CommClientGConnectionEx::_safeQualityUpdated( int quality )
{
	try
	{
		PSIGRETURN()
			qualityUpdated( quality );
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnectionEx: exception in qualityUpdated() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientGConnectionEx: unknown exception in qualityUpdated() caught" );
	}
}

bool CommClientGConnectionEx::_safeClosed( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
{
	try
	{
		PSIGRETURN()
			return closedOrDisconnected(errCode,errMsg,errMsgBody );
	}
	catch( PError& err )
	{
		PLog( "CommClientGConnectionEx: exception in closed() caught: %s", err.why() );
		return true;
	}
	catch( ... )
	{
		PLog( "CommClientGConnectionEx: unknown exception in closed() caught" );
		return true;
	}
}

/*
void CommClientGConnectionEx::_safeProcessReply( AsyncCall* call, UINT32 requestId, UINT32 msgId, const CommMsgBody& body )
{
try
{
call->processReply( requestId, msgId, body );
}
catch( PError& err )
{
PLog( "CommClientGConnectionEx::AsyncCall: exception in processReply (msgId=%d=0x%X) caught: %s", (int)msgId, (int)msgId, err.why() );
}
catch( ... )
{
PLog( "CommClientGConnectionEx::AsyncCall: unknown exception in processReply (msgId=%d=0x%X) caught", (int)msgId, (int)msgId );
}
}
*/
CommClientGConnectionEx::CommClientGConnectionEx( CommClientGuardFactory& guardFactory_)
: conn( guardFactory_)
{
	conn._init( this );
}

/* virtual */ CommClientGConnectionEx::~CommClientGConnectionEx()
{
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = *it;
		delete item;
	}
}

void CommClientGConnectionEx::_clearAll()
{
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = *it;
		delete item;
	}
	msgList.clear();
	msgMap.clear();
}

UINT32 CommClientGConnectionEx::post( UINT32 msgId, /*non-const!*/ CommMsgBody& body, /* new */ AsyncCall* call )
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

void CommClientGConnectionEx::cancelPost( UINT32 requestId )
{
	_List::iterator* found = msgMap.find( requestId );
	PASSERT3( found != 0 );

	_List::iterator it = *found;
	_Item* item = *it;
	delete item;
	msgList.erase( it );
	msgMap.remove( requestId );
}

void CommClientGConnectionEx::_gConnected()
{
	for( _List::iterator it = msgList.begin(); it != msgList.end() ; ++it )
	{
		_Item* item = *it;
		CommMsgBody copy;
		copy.copyFrom( item->patchedBody );
		conn.post( item->msgId, copy );
	}
	_safeConnected();
}

void CommClientGConnectionEx::_gProcessMessage( UINT32 msgId, const CommMsgBody& body )
{
	CommMsgParser parser( body );
	UINT32 requestId;
	parser.parseUINT32( requestId );
	if( requestId != 0 )//requestId == 0 means unsolicited server->client message
	{
		_List::iterator* found = msgMap.find( requestId );
		if( found == 0 )
		{
			PLog( "CommClientGConnectionEx: unexpected msg with requestId=%08X (msgId=%08X) - ignored", requestId, msgId );
			return;
		}

		_List::iterator it = *found;
		_Item* item = *it;

		CommMsgBody patchedBody;
		patchedBody.copyFrom( body );
		patchedBody._skipPrefix( 4 );
		_safeProcessGMessage( requestId, msgId, patchedBody, item->call );
		//			if( item->call->_preserveAfterProcessing )
		//				item->call = 0;//effectively prevents deleting of item->call

		delete item;
		msgList.erase( it );
		msgMap.remove( requestId );
	}
	else
	{
		CommMsgBody patchedBody;
		patchedBody.copyFrom( body );
		patchedBody._skipPrefix( 4 );
		_safeProcessGMessage( requestId, msgId, patchedBody, 0 );
	}
}

/* static */ UINT32 CommClientGConnectionEx::_getRequestId( const_iterator it )
{
	const CommMsgBody& patchedBody = (*it)->patchedBody;
	PASSERT3( patchedBody._size() >= 4 );
	return CommMsgBody::readUINT32( patchedBody._readPtr() );
}

/* static */ void CommClientGConnectionEx::_getMsgBody( const_iterator it, CommMsgBody& ret )
{
	ret.copyFrom( (*it)->patchedBody );
	ret._skipPrefix( 4 );
}
