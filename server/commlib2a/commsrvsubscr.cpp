#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commsubscr.h"

CommServerPublisher::CommServerPublisher()
{
	ida = PID();
	idb = (UINT32) time( 0 );
	idc = RANDOMTIME();
	idd = 0;
	_setHdr( ida, idb, idc, idd );

	pool = 0;
	//	child = 0;
	rev = 1;
	priority = _COMM_MSGPRIORITY_INITSUBSCR;
	numberOfSubscribers=0;
	countingOn = false;
}

void CommServerPublisher::_nextGeneration()
{
	//+++ - create new generation: increment idd, re-assign ids/revs for the whole tree, send onlineUpdate command 'H' w/new header
	PASSERT3( 0 );
}

inline UINT32 CommServerPublisher::_nextRev( CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 ret = rev++;
	if( ret == 0 )
	{
		_nextGeneration();
		if( updateMsg )
			updateMsg->newGen = true;
		ret = rev++;
	}
	return ret;
}

inline CommServerPublisher::_Node* CommServerPublisher::_findNode( const int* path, int pathLen, UINT32 rev )
{
	PASSERT3( pathLen >= 0 && pathLen < 65535 );
	root.rev = rev;//!!!
	_Node* node = &root;
	for( int i=0; i < pathLen ; ++i )
	{
		PASSERT3( !node->isLeaf );
		int idx = path[ i ];
		PASSERT3( idx >= 0 && idx < node->children.size() );
		node = node->children[ idx ];
		node->rev = rev;//!!!
	}
	return node;
}

inline CommServerPublisher::_Node* CommServerPublisher::_findNodeNoChanges( const int* path, int pathLen )
{
	PASSERT3( pathLen >= 0 && pathLen < 65535 );
	_Node* node = &root;
	for( int i=0; i < pathLen ; ++i )
	{
		PASSERT3( !node->isLeaf );
		int idx = path[ i ];
		PASSERT3( idx >= 0 && idx < node->children.size() );
		node = node->children[ idx ];
	}
	return node;
}

void CommServerPublisher::startUpdateMsg( CommServerPublisher::UpdateMsg& msg ) const
{
	if (children.size())
	{
		msg.active = true;
		msg.empty = true;
		msg.newGen = false;
		CommMsgBody& body = msg.msg;
		body.clear();
		body.composeUINT32( 0 ).composeUINT32( 0 );
		body.composeUINT32( root.rev );
	}
	else
		msg.active = false;
}

void CommServerPublisher::finishUpdateMsg( CommServerPublisher::UpdateMsg& msg ) const
{
	if( msg.active )
	{
		if( msg.newGen )
		{
			//+++
		}
		CommMsgBody& body = msg.msg;
		body.composeBYTE( '0' ).composeUINT32( root.rev );
	}
}

static void _writePath( CommMsgBody& msg, const int* path, int pathLen )
{
	PASSERT3( pathLen >= 0 && pathLen < 65535 );
	msg.composeUINT16( (UINT16)pathLen );
	for( int i=0; i < pathLen ; ++i )
	{
		PASSERT3( path[ i ] >= 0 && path[ i ] < 65535 );
		msg.composeUINT16( (UINT16)path[ i ] );
	}
}

int CommServerPublisher::appendNode( const int* path, int pathLen, CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 rev = _nextRev( updateMsg );
	_Node* node = _findNode( path, pathLen, rev );
	PASSERT3( !node->isLeaf );
	PASSERT3( node->children.size() < 65534 );
	_Node* newNode = new _Node();
	newNode->isLeaf = 0x00;
	newNode->id = newNode->rev = rev;
	node->children.push_back( newNode );

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& msg = updateMsg->msg;
		msg.composeBYTE( 'A' );
		_writePath( msg, path, pathLen );
	}
	return node->children.size() - 1;
}

#if 0
void CommServerPublisher::insertNode( const int* path, int pathLen, int pos, CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 rev = _nextRev( updateMsg );
	_Node* node = _findNode( path, pathLen, rev );
	PASSERT3( !node->isLeaf );
	PASSERT3( node->children.size() < 65534 );
	PASSERT3( pos >= 0 && pos <= node->children.size() );
	_Node* newNode = new _Node();
	newNode->isLeaf = 0x00;
	newNode->id = newNode->rev = rev;
	node->children.insert( node->children.begin() + pos, newNode );

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& msg = updateMsg->msg;
		msg.composeBYTE( 'I' );
		_writePath( msg, path, pathLen );
		msg.composeUINT16( (UINT16)pos );
	}
}
#endif

int CommServerPublisher::appendLeaf( const int* path, int pathLen, /*non-const!*/ CommMsgBody& msg, CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 rev = _nextRev( updateMsg );
	_Node* node = _findNode( path, pathLen, rev );
	PASSERT3( !node->isLeaf );
	PASSERT3( node->children.size() < 65534 );
	_Node* newNode = new _Node();
	newNode->isLeaf = 0xFF;
	newNode->id = newNode->rev = rev;
	newNode->leaf.moveFrom( msg );
	node->children.push_back( newNode );

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& tmpMsg = updateMsg->msg;
        tmpMsg.composeBYTE( 'a' );
		_writePath(tmpMsg, path, pathLen );
        tmpMsg.composeMsgBody( newNode->leaf );
	}
	return node->children.size() - 1;
}

#if 0
void CommServerPublisher::insertLeaf( const int* path, int pathLen, int pos, /*non-const!*/ CommMsgBody& msg, CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 rev = _nextRev( updateMsg );
	_Node* node = _findNode( path, pathLen, rev );
	PASSERT3( !node->isLeaf );
	PASSERT3( node->children.size() < 65534 );
	PASSERT3( pos >= 0 && pos <= node->children.size() );
	_Node* newNode = new _Node();
	newNode->isLeaf = 0xFF;
	newNode->id = newNode->rev = rev;
	newNode->leaf.moveFrom( msg );
	node->children.insert( node->children.begin() + pos, newNode );

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& msg = updateMsg->msg;
		msg.composeBYTE( 'i' );
		_writePath( msg, path, pathLen );
		msg.composeUINT16( (UINT16)pos ).composeMsgBody( newNode->leaf );
	}
}
#endif

void CommServerPublisher::updateLeaf( const int* path, int pathLen, /*non-const!*/ CommMsgBody& msg, CommServerPublisher::UpdateMsg* updateMsg )
{
	_Node* node = _findNodeNoChanges( path, pathLen );
	PASSERT3( node->isLeaf );
	if( node->leaf._size() == msg._size() && memcmp( node->leaf._readPtr(), msg._readPtr(), msg._size() ) == 0 )
		return;

	UINT32 rev = _nextRev( updateMsg );
	/* _Node* */ node = _findNode( path, pathLen, rev );
	PASSERT3( node->isLeaf );
	node->leaf.moveFrom( msg );
	//no node->rev update needed as it is done by _findNode

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& tmpMsg = updateMsg->msg;
        tmpMsg.composeBYTE( 'u' );
		_writePath(tmpMsg, path, pathLen );
        tmpMsg.composeMsgBody( node->leaf );
	}
}

void CommServerPublisher::removeLeaf( const int* path, int pathLen, CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 rev = _nextRev( updateMsg );
	_Node* node = _findNode( path, pathLen - 1, rev );
	PASSERT3( !node->isLeaf );
	int idx = path[ pathLen - 1 ];
	PASSERT3( idx >= 0 && idx < node->children.size() );
	_Node* child = node->children[ idx ];
	PASSERT3( child->isLeaf );
	delete child;
	node->children.erase( node->children.begin() + idx );

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& msg = updateMsg->msg;
		msg.composeBYTE( 'd' );
		_writePath( msg, path, pathLen - 1 );
		msg.composeUINT16( (UINT16)idx );
	}
}

void CommServerPublisher::removeNode( const int* path, int pathLen, CommServerPublisher::UpdateMsg* updateMsg )
{
	UINT32 rev = _nextRev( updateMsg );
	_Node* node = _findNode( path, pathLen - 1, rev );
	PASSERT3( !node->isLeaf );
	int idx = path[ pathLen - 1 ];
	PASSERT3( idx >= 0 && idx < node->children.size() );
	_Node* child = node->children[ idx ];
	PASSERT3( !child->isLeaf );
	delete child;
	node->children.erase( node->children.begin() + idx );

	if( updateMsg && updateMsg->active )
	{
		updateMsg->empty = false;
		CommMsgBody& msg = updateMsg->msg;
		msg.composeBYTE( 'D' );
		_writePath( msg, path, pathLen - 1 );
		msg.composeUINT16( (UINT16)idx );
	}
}

void CommServerPublisher::attachMsg( UINT32 attachId, const CommMsgBody& attachment, CommServerPublisher::UpdateMsg* updateMsg )
{
	if( updateMsg )
	{
		CommMsgBody& msg = updateMsg->msg;
		msg.composeBYTE( '!' ).composeUINT32( attachId ).composeMsgBody( attachment );
	}
}

void CommServerPublisher::postUpdate( CommServerPublisher::UpdateMsg& update )
{
	pool->_postUpdate( *this, update );
}

void CommServerPublisher::startCountNumberOfSubscribers()
{
	if (countingOn == false)
	{
		countingOn=true;
		if (children.size())
		{
			_startCountNumberOfSubscribers();
		}
	}
}

void CommServerPublisher::_startCountNumberOfSubscribers()
{
	for( _SubscrSet::iterator iter = children.begin() ; iter != children.end() ; ++iter )
	{
		CommMsgBody body;
		_Subscr * child = (*iter);
		body.composeUINT32( child->peerId ).composeUINT32( child->thisId ).composeBYTE(1);
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST, priority, body );
		pool->inter->_post( m );
	}
}

void CommServerPublisher::stopCountNumberOfSubscribers()
{
	if(countingOn)
	{
		countingOn = false;
		for( _SubscrSet::iterator iter = children.begin() ; iter != children.end() ; ++iter )
		{
			CommMsgBody body;
			_Subscr * child = (*iter);
			body.composeUINT32( child->peerId ).composeUINT32( child->thisId ).composeBYTE(0);
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST, priority, body );
			pool->inter->_post( m );
		}
	}
}

//****************************************************************************

CommServerPublisherPool::CommServerPublisherPool( _CommInterface& inter_ )
{
	inter = &inter_;
	connected = false;
}

void CommServerPublisherPool::publish( const char* name, CommServerPublisher& publisher )
{
	PASSERT3( publisher.pool == 0 );

	_PublisherMap::iterator found = publisherMap.find( name );
	PASSERT3( found == publisherMap.end() );

	publisher.pool = this;
	publisher.name = name;
	publisherMap.insert( _PublisherMap::value_type( name, &publisher ) );
}

void CommServerPublisherPool::unpublish( const char* name )
{
	_PublisherMap::iterator found = publisherMap.find( name );
	if (found == publisherMap.end())
	{
		PLog( "CommServerPublisherPool::unpublish: subscription '%s' not found", name);
		return;
	}
	CommServerPublisher* publisher = (*found).second;
	PASSERT3( publisher->pool == this );
	publisher->pool = 0;
	publisher->name = "";
	while(publisher->children.size())
	{
		CommServerPublisher::_SubscrSet::iterator iter = publisher->children.begin();
		CommServerPublisher::_Subscr * child = (*iter);
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, publisher->priority );
		m.body.composeUINT32( child->peerId ).composeUINT32( child->thisId ).composeUINT16( _COMM_ERROR_SUBSCRIPTION_UNPUBLISHED ).composeString( "Subscription no longer exists" );
		inter->_post( m );
		_delSubscr( child );
	}
	publisherMap.erase( found );
}

CommServerPublisher::_Subscr* CommServerPublisherPool::_getSubscr( UINT32 thisId, BYTE msgType )
{
	CommServerPublisher::_Subscr** subscrPtr = subscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommServerPublisherPool: msg type=%02X for unknown subscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	CommServerPublisher::_Subscr* subscr = *subscrPtr;
	PASSERT3( subscr->thisId == thisId );
	return subscr;
}

CommServerPublisher::_Subscr* CommServerPublisherPool::_getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	CommServerPublisher::_Subscr* subscr = _getSubscr( thisId, msgType );
	if( subscr == 0 )
		return 0;
	if( peerId != subscr->peerId )
	{
		PLog( "CommServerPublisherPool: msg type=%02X for subscrId=%08X with msg.peerId=%08X != subscr.peerId=%08X - ignored", msgType, thisId, peerId, subscr->peerId );
		return 0;
	}
	return subscr;
}

void CommServerPublisherPool::_delSubscr( CommServerPublisher::_Subscr* subscr )
{
	CommServerPublisher::_SubscrSet::iterator iter = subscr->publisher->children.find(subscr);
	PASSERT3(iter != subscr->publisher->children.end());
	if (subscr->publisher->countingOn)
	{
		subscr->publisher->numberOfSubscribers = 0;
	}
	subscr->publisher->children.erase(iter);
	subscrMap.remove( subscr->thisId );
	delete subscr;

}

CommServerPublisher::_Subscr* CommServerPublisherPool::_lookForSubscr( UINT32 cliId )
{
	int i;
	for( CommServerPublisher::_Subscr** s = subscrMap.firstIndex( i ); s ; s = subscrMap.nextIndex( i ) )
		if( (*s)->peerId == cliId )
			return *s;
	return 0;
}

void CommServerPublisherPool::processSubscrRequest(CommMsgBody & body)
{
	CommMsgParser parser(body );
	UINT32 peerId;
	const char* server;
	const char* serverObject;
	const char* subscrChannel;
	parser.parseUINT32( peerId ).parseString( server ).parseString( serverObject ).parseString( subscrChannel );
	PTRACE4( "CommServerPublisherPool::processSubscrRequest: peerId=%08X server='%s' serverObject='%s' subscrChannel='%s'", peerId, server, serverObject, subscrChannel );
			
	_PublisherMap::iterator found = publisherMap.find( subscrChannel );
	if( found == publisherMap.end() )
	{
		PTRACE3( "CommServerPublisherPool::processSubscrRequest: Subscription ('%s'/'%s'/'%s') denied (unknown channel)", server, serverObject, subscrChannel );
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
		m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOPUBLISHER ).composeString( "Unknown publisher channel" );
		inter->_post( m );
		return;
	}

	CommServerPublisher::_Subscr* subscr = new CommServerPublisher::_Subscr();
	subscr->publisher = (*found).second;
	subscr->thisId = subscrMap.add( subscr );
	subscr->peerId = peerId;
	subscr->publisher->children.insert( subscr );

	{
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED, subscr->publisher->priority );
		m.body.composeUINT32( subscr->peerId ).composeUINT32( subscr->thisId );
		inter->_post( m );
	}

	{
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE, subscr->publisher->priority );
		m.body.composeUINT32( subscr->peerId ).composeUINT32( subscr->thisId ).composeBYTE( subscr->publisher->priority );
		subscr->publisher->processSyncRequest( m.body, parser );
		inter->_post( m );
	}

	if (subscr->publisher->countingOn)
	{
		subscr->publisher->_startCountNumberOfSubscribers();
	}

	PTRACE3( "CommServerPublisherPool::processSubscrRequest: Subscription ('%s'/'%s'/'%s') accepted (thisId=%08X) and granted", server, serverObject, subscrChannel, subscr->thisId );
}



bool CommServerPublisherPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PLog( "CommServerPublisherPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	switch( msg.type )
	{
	case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		connected = true;
		return false;//not the only message consumer

	case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
		{
			PTRACE3( "CommServerPublisherPool: _COMM_MSGTYPE_PHYSICAL_CONNECT" );
			if( connected )
			{
				PLog( "CommServerPublisherPool: _COMM_MSGTYPE_PHYSICAL_CONNECT in connected state - ignored" );
				return false;
			}

			int n = subscrMap.size();
			if( n )
			{
				int i, subscrI = 0;
				PArrayAutoPtr< CommServerPublisher::_Subscr* > subscrs( new CommServerPublisher::_Subscr*[ n ] );//although it seems safe to call _delSubscr within subscrMap iteration, but it changes subscrMap, so we will move it out of the loop - just in case
				for( CommServerPublisher::_Subscr** subscrPtr = subscrMap.firstIndex( i ); subscrPtr ; subscrPtr = subscrMap.nextIndex( i ) )
				{
					CommServerPublisher::_Subscr* subscr = *subscrPtr;
					PASSERT3( subscrI < n );
					subscrs[ subscrI++ ] = subscr;
				}
				PASSERT3( subscrI == n );
				for( i=0; i < n ; ++i )
					_delSubscr( subscrs[ i ] );
			}
			connected = false;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 peerId;
			const char* server;
			const char* serverObject;
			const char* subscrChannel;
			parser.parseUINT32( peerId ).parseString( server ).parseString( serverObject ).parseString( subscrChannel );
			PTRACE4( "CommServerPublisherPool: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST: peerId=%08X server='%s' serverObject='%s' subscrChannel='%s'", peerId, server, serverObject, subscrChannel );
			
			_PublisherMap::iterator found = publisherMap.find( subscrChannel );
			if( found == publisherMap.end() )
			{
				if (!_aboutToSubscribe(msg.body,subscrChannel))
					return true;
				found = publisherMap.find( subscrChannel );
				if( found == publisherMap.end() )
				{
					PTRACE3( "CommServerPublisherPool: Subscription ('%s'/'%s'/'%s') denied (unknown channel)", server, serverObject, subscrChannel );
					_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
					m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOPUBLISHER ).composeString( "Unknown publisher channel" );
					inter->_post( m );
					return true;
				}
			}

			CommServerPublisher::_Subscr* subscr = new CommServerPublisher::_Subscr();
			subscr->publisher = (*found).second;
			subscr->thisId = subscrMap.add( subscr );
			subscr->peerId = peerId;
			subscr->publisher->children.insert( subscr );

			{
				_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED, subscr->publisher->priority );
				m.body.composeUINT32( subscr->peerId ).composeUINT32( subscr->thisId );
				inter->_post( m );
			}

			{
				_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE, subscr->publisher->priority );
				m.body.composeUINT32( subscr->peerId ).composeUINT32( subscr->thisId ).composeBYTE( subscr->publisher->priority );
				subscr->publisher->processSyncRequest( m.body, parser );
				inter->_post( m );
			}

			if (subscr->publisher->countingOn)
			{
				subscr->publisher->_startCountNumberOfSubscribers();
			}

			PTRACE3( "CommServerPublisherPool: Subscription ('%s'/'%s'/'%s') accepted (thisId=%08X) and granted", server, serverObject, subscrChannel, subscr->thisId );
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommServerPublisherPool: _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE: thisId=%08X peerId=%08X", thisId, peerId );

			CommServerPublisher::_Subscr* subscr = 0;
			if( thisId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
			{//special case
				subscr = _lookForSubscr( peerId );
				if( !subscr )
				{
					PLog( "CommServerPublisherPool: Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE with cliId=%08X", peerId );
				}
			}

			if( subscr )
			{
				CommServerPublisher * publisher = subscr->publisher;
				_delSubscr( subscr );
				if (publisher->children.size() == 0 )
				{
					_afterUnsubscribe(publisher->name);
				}
			}
		}
		return true;
	case _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId,count;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32(count);
			PTRACE4( "CommServerPublisherPool: _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE: thisId=%08X peerId=%08X count=%d", thisId, peerId,count );
			CommServerPublisher::_Subscr* subscr = 0;
			if( thisId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
			{//special case
				PLog( "CommServerPublisherPool: Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE with cliId=%08X", peerId );
			}
			if( subscr )
			{
				PASSERT3( subscr->publisher->children.find(subscr) != subscr->publisher->children.end() );
				subscr->publisher->numberOfSubscribers = count;
			}
		}
		return true;
	case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1:
		{
			CommMsgParser parser( msg.body );
			UINT32 peerId;
			const char* server;
			const char* serverObject;
			const char* subscrChannel;
			parser.parseUINT32( peerId ).parseString( server ).parseString( serverObject ).parseString( subscrChannel );
			PTRACE4( "CommServerPublisherPool: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1: peerId=%08X server='%s' serverObject='%s' subscrChannel='%s', protocol is not supported", peerId, server, serverObject, subscrChannel );
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
			m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_PROTOCOLNOTSUPPORTED ).composeString( "Protocol not supported" );
			inter->_post( m );
			return true;
		}
	}
	return false;
}

void CommServerPublisherPool::_postUpdate( const CommServerPublisher& publisher, CommServerPublisher::UpdateMsg& update )
{
	if( update.shouldBePosted() )
	{
		if (publisher.children.size() > 1)
		{
			for( CommServerPublisher::_SubscrSet::const_iterator iter = publisher.children.begin(); iter != publisher.children.end(); ++iter )
			{
				CommMsgBody body;
				const CommServerPublisher::_Subscr * child = (*iter);
				body.copyFrom(update.msg);
				BYTE* p = body._writePtr();
				CommMsgBody::writeUINT32( p, child->peerId );
				p += 4;
				CommMsgBody::writeUINT32( p, child->thisId );
				_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE, publisher.priority, body );
				inter->_post( m );
			}
		}
		else if (publisher.children.size() == 1)
		{
			const CommServerPublisher::_Subscr * child = *publisher.children.begin();
			BYTE* p = update.msg._writePtr();
			CommMsgBody::writeUINT32( p, child->peerId );
			p += 4;
			CommMsgBody::writeUINT32( p, child->thisId );
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE, publisher.priority, update.msg );
			inter->_post( m );
		}
	}
}
//****************************************************************************


