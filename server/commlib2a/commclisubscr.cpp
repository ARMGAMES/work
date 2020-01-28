#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commsubscr.h"
#include "commcli.h"

CommClientSubscriber::CommClientSubscriber()
{
	pool = 0;
	priority = 0;
}

CommClientSubscriber::~CommClientSubscriber()
{
	if( pool )
	{
		// shouldn't  this subscription be removed from IdMap?
		// Vadim, Please discuss this with Dmitry L. & Victor G.
		PLog( "Error: CommClientSubscriber::~CommClientSubscriber(): destroying non-unsubscribed subscription" );
	}
}

/* virtual */ bool CommClientSubscriber::closed( int errCode, const char* errMsg )
{
	if( errCode == _COMM_ERROR_NOLOCALOBJECT || errCode == _COMM_ERROR_INBOUNDCONNCLOSED || errCode == _COMM_ERROR_NOPUBLISHER || errCode == _COMM_ERROR_SUBSCRIPTION_UNPUBLISHED )
		return false;
	return true;
}

void CommClientSubscriber::_safeBeginTrans()
{
	try
	{
		PSIGRETURN()
			beginTrans();
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriber: exception in beginTrans() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientSubscriber: unknown exception in beginTrans() caught" );
	}
}

void CommClientSubscriber::_safeCommitTrans()
{
	try
	{
		PSIGRETURN()
			commitTrans();
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriber: exception in commitTrans() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientSubscriber: unknown exception in commitTrans() caught" );
	}
}

void CommClientSubscriber::_safeSynchronized()
{
	try
	{
		PSIGRETURN()
			synchronized();
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriber: exception in synchronized() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientSubscriber: unknown exception in synchronized() caught" );
	}
}

void CommClientSubscriber::_safeDesynchronized()
{
	try
	{
		PSIGRETURN()
			desynchronized();
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriber: exception in desynchronized() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientSubscriber: unknown exception in desynchronized() caught" );
	}
}

bool CommClientSubscriber::_safeClosed( int errCode, const char* errMsg )
{
	try
	{
		PSIGRETURN()
			return closed( errCode, errMsg );
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriber: exception in closed() caught: %s", err.why() );
		return true;
	}
	catch( ... )
	{
		PLog( "CommClientSubscriber: unknown exception in closed() caught" );
		return true;
	}
}

//***************************************************************************/

CommClientSubscriberPool::CommClientSubscriberPool( _CommInterface& inter_ )
{
	inter = &inter_;
	connected = false;
}

CommClientSubscriberPool::~CommClientSubscriberPool()
{
	//+++ disconnect
}

CommClientSubscriber* CommClientSubscriberPool::_getSubscr( UINT32 thisId, BYTE msgType )
{
	CommClientSubscriber** subscrPtr = subscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommClientSubscriberPool: msg type=%02X for unknown subscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	CommClientSubscriber* subscr = *subscrPtr;
	PASSERT3(!subscr || subscr->thisId == thisId ); // catch 'Assertion failed: , file ..\commlib2a\commclisubscr.cpp, line 139'
	// Vadim, Please discuss this with Dmitry L. & Victor G.
	return subscr;
}

CommClientSubscriber* CommClientSubscriberPool::_getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	CommClientSubscriber* subscr = _getSubscr( thisId, msgType );
	if( subscr == 0 )
		return 0;
	if( peerId != subscr->peerId )
	{
		PLog( "CommClientSubscriberPool: msg type=%02X for subcrId=%08X with msg.peerId=%08X != subscr.peerId=%08X - ignored", msgType, thisId, peerId, subscr->peerId );
		return 0;
	}
	return subscr;
}

void CommClientSubscriberPool::_subscribe( CommClientSubscriber& subscr )
{
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_REQUEST, subscr.priority );
	m.body.composeUINT32( subscr.thisId ).composeString( subscr.server ).composeString( subscr.serverObject ).composeString( subscr.serverChannel );
	subscr.genSyncRequest( m.body );
	inter->_post( m );

	subscr.state = CommSubscription::_StateSubscrRequestSent;
	subscr.peerId = 0;//for special error cases
}

void CommClientSubscriberPool::_postDefrost( CommClientSubscriber& subscr )
{
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_DEFROST, subscr.priority );
	m.body.composeUINT32( subscr.peerId ).composeUINT32( subscr.thisId );
	subscr.genSyncRequest( m.body );
	inter->_post( m );
}
void CommClientSubscriberPool::_scheduleResubscribe( CommClientSubscriber& subscr )
{
	PASSERT3( subscr.state == CommSubscription::_StateSubscrServerFatalError );

	_CommMsg msg( _COMM_MSGTYPE_SUBSCRPOOL_TIMER, 0 );
	msg.body.composeUINT32( subscr.thisId );
	inter->_postClientTimer( msg, _ResubscribeTimeout );
}

void CommClientSubscriberPool::_resubscribe( CommClientSubscriber& subscr )
{
	subscrMap.remove( subscr.thisId );
	subscr.thisId = subscrMap.add( &subscr );
	_subscribe( subscr );
}

void CommClientSubscriberPool::_postUnsubscribe( CommClientSubscriber& subscr )
{
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE, subscr.priority );
	m.body.composeUINT32( subscr.peerId ).composeUINT32( subscr.thisId );
	inter->_post( m );
}

void CommClientSubscriberPool::_postFreeze( CommClientSubscriber& subscr )
{
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FREEZE, subscr.priority );
	m.body.composeUINT32( subscr.peerId ).composeUINT32( subscr.thisId );
	inter->_post( m );
}

void CommClientSubscriberPool::subscribe( CommClientSubscriber& subscr, const char* server, const char* serverObject, const char* serverChannel )
{
	PASSERT3( subscr.pool == 0 );
	subscr.pool = this;
	subscr.server = server;
	subscr.serverObject = serverObject;
	subscr.serverChannel = serverChannel;
	subscr.priority = _COMM_MSGPRIORITY_INITSUBSCR;
	subscr.peerId = 0;//invalid value
	subscr.thisId = subscrMap.add( &subscr );

	if( connected )
		_subscribe( subscr );
	else
		subscr.state = CommSubscription::_StateSubscrDisc;
}

void CommClientSubscriberPool::subscribeFrozen( CommClientSubscriber& subscr, const char* server, const char* serverObject, const char* serverChannel )
{
	PASSERT3( subscr.pool == 0 );
	subscr.pool = this;
	subscr.server = server;
	subscr.serverObject = serverObject;
	subscr.serverChannel = serverChannel;
	subscr.priority = _COMM_MSGPRIORITY_INITSUBSCR;
	subscr.peerId = 0;//invalid value
	subscr.thisId = subscrMap.add( &subscr );
	subscr.state = CommSubscription::_StateSubscrFrozenDisc;
}

void CommClientSubscriberPool::unsubscribe( CommClientSubscriber& subscr )
{
	PASSERT3( subscr.pool == this );
	switch( subscr.state )
	{
	case CommSubscription::_StateSubscrDisc:
	case CommSubscription::_StateSubscrServerFatalError:
		//do nothing
		break;

	case CommSubscription::_StateSubscrFrozenDisc:
		// General idea - if state _StateSubscrFrozenDisc it means that
		// there is no corresponding leaf in routing table.
		// We don't need to do anything.
		break;

	case CommSubscription::_StateSubscrRequestSent:
	case CommSubscription::_StateSubscrPeerAssigned:
	case CommSubscription::_StateSubscrSync:
	case CommSubscription::_StateSubscrDesync:
	case CommSubscription::_StateSubscrFrozenDesync:
		_postUnsubscribe( subscr );
		break;

	default:
		PASSERT3( 0 );
		break;
	}
	subscrMap.remove( subscr.thisId );
	subscr.clear();
	subscr.server = "";
	subscr.serverObject = "";
	subscr.serverChannel = "";
	subscr.pool = 0;
}



void CommClientSubscriberPool::freeze( CommClientSubscriber& subscr )
{
	PASSERT3( subscr.pool == this );
	switch( subscr.state )
	{
	case CommSubscription::_StateSubscrFrozenDisc:
	case CommSubscription::_StateSubscrFrozenDesync:
		// nothing to do
		break;

	case CommSubscription::_StateSubscrDisc:
	case CommSubscription::_StateSubscrServerFatalError:
		// the both these cases mean that there is no corresponding leaf in routing table
		subscr.state = CommSubscription::_StateSubscrFrozenDisc;
		break;

	case CommSubscription::_StateSubscrRequestSent:
		// Drop process of subscription and set disconnect frozen state 
		subscr.state = CommSubscription::_StateSubscrFrozenDisc;
		_postFreeze( subscr );
		break;
	case CommSubscription::_StateSubscrSync:    // normal situation
	case CommSubscription::_StateSubscrDesync:  // transdisconnect, will be fixed by the routing tables anyway.
	case CommSubscription::_StateSubscrPeerAssigned: // Peer assigned. Consider this case like handshake is over.
		// Set desync frozen state
		subscr.state = CommSubscription::_StateSubscrFrozenDesync;
		_postFreeze( subscr );
		break;
	default:
		PASSERT3( 0 );
		break;
	}
}

void CommClientSubscriberPool::defrost( CommClientSubscriber& subscr )
{
	PASSERT3( subscr.pool == this );
	switch( subscr.state )
	{
	case CommSubscription::_StateSubscrFrozenDisc:
		if( connected )
		{
			_resubscribe( subscr );
		}
		else
			subscr.state = CommSubscription::_StateSubscrDisc;
		break;
	case CommSubscription::_StateSubscrFrozenDesync:
		subscr.state = CommSubscription::_StateSubscrDesync;
		_postDefrost( subscr ); 
		break;
	case CommSubscription::_StateSubscrDisc:
	case CommSubscription::_StateSubscrServerFatalError:
	case CommSubscription::_StateSubscrRequestSent:
	case CommSubscription::_StateSubscrPeerAssigned:
	case CommSubscription::_StateSubscrSync:
	case CommSubscription::_StateSubscrDesync:
		// nothing to do
		break;

	default:
		PASSERT3( 0 );
		break;
	}
}

bool CommClientSubscriberPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( msg.type == _COMM_MSGTYPE_SUBSCRPOOL_TIMER )
	{
		CommMsgParser parser( msg.body );
		UINT32 subscrId;
		parser.parseUINT32( subscrId );

		CommClientSubscriber** subscrPtr = subscrMap.find( subscrId );
		if( subscrPtr == 0 )
			PTRACE3( "CommClientSubscriberPool: timer msg for unknown subscrId=%08X - ignored", subscrId );
		else
		{
			CommClientSubscriber* subscr = *subscrPtr;
			if (subscr->state != CommSubscription::_StateSubscrFrozenDisc)
				_resubscribe( *subscr );
		}

		return true;
	}

	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PLog( "CommClientSubscriberPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	switch( msg.type )
	{
	case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		{
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_PHYSICAL_CONNECT" );
			if( connected )
			{
				PLog( "CommClientSubscriberPool: _COMM_MSGTYPE_PHYSICAL_CONNECT in connected state - ignored" );
				return false;
			}

			int n = static_cast< int >( subscrMap.size() );
			if( n )
			{
				int i, subscrI = 0;
				PArrayAutoPtr< CommClientSubscriber* > subscrs( new CommClientSubscriber*[ n ] );//!cannot call _resubscribe within subscrMap iteration as it changes subscrMap
				for( CommClientSubscriber** subscrPtr = subscrMap.firstIndex( i ); subscrPtr ; subscrPtr = subscrMap.nextIndex( i ) )
				{
					CommClientSubscriber* subscr = *subscrPtr;
					PASSERT3( subscr->state == CommSubscription::_StateSubscrDisc || subscr->state == CommSubscription::_StateSubscrFrozenDisc);
					PASSERT3( subscrI < n );
					if (subscr->state == CommSubscription::_StateSubscrDisc)
						subscrs[ subscrI++ ] = subscr;
				}
				//				PASSERT3( subscrI == n );
				for( i=0; i < subscrI ; ++i )
				{
					_resubscribe( *subscrs[ i ] );
				}
			}
			connected = true;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
		{
			PASSERT3( connected );
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_PHYSICAL_DISCONNECT" );
			int i;
			for( CommClientSubscriber** subscrPtr = subscrMap.firstIndex( i ); subscrPtr ; subscrPtr = subscrMap.nextIndex( i ) )
			{
				CommClientSubscriber* subscr = *subscrPtr;
				if (subscr->state == CommSubscription::_StateSubscrFrozenDesync  || 
					subscr->state == CommSubscription::_StateSubscrFrozenDisc )
				{
					// The both frozen cases here. Don't infrom upper level, do nothing,
					// just change (or confirm) the state
					subscr->state = CommSubscription::_StateSubscrFrozenDisc;
				}
				else
				{
					subscr->_safeDesynchronized(); 
					subscr->state = CommSubscription::_StateSubscrDisc;
				}
			}
			connected = false;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientSubscriber* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state != CommSubscription::_StateSubscrRequestSent )
				{
					// If we are in frozen state also ignore this message for now.
					// BTW, we should be in FrozenDisc
					if (subscr->state == CommSubscription::_StateSubscrFrozenDisc)
					{
						// Subscription was frozen at state of handshake. 
						// OK, let it be!
						subscr->peerId = peerId;
						subscr->state = CommSubscription::_StateSubscrFrozenDesync;
					}
					else
					{
						PLog( "CommClientSubscriberPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
				}
				else
				{
					subscr->peerId = peerId;
					subscr->state = CommSubscription::_StateSubscrPeerAssigned;
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE priority;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority );
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE: thisId=%08X peerId=%08X priority=%d size=%d", thisId, peerId, priority, msg.body._size() );

			CommClientSubscriber* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				// To solve the problem with priorities
				if( subscr->state == CommSubscription::_StateSubscrRequestSent )
				{
					subscr->peerId = peerId;
					subscr->state = CommSubscription::_StateSubscrPeerAssigned;
				}
				// end
				if( subscr->state != CommSubscription::_StateSubscrPeerAssigned )
				{
					if (subscr->state == CommSubscription::_StateSubscrFrozenDesync)
					{
						// freeze was sent at the middle of handshake.
						// Set priority only.
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommClientSubscriberPool: invalid priority=%d in _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE for subscrId=%08X - ignored", priority, thisId );
						}
						else
							subscr->priority = priority;
					}
					else
					{
						PLog( "CommClientSubscriberPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
				}
				else
				{
					if( priority >= _COMM_MSGPRIORITIES )
					{
						PLog( "CommClientSubscriberPool: invalid priority=%d in _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE for subscrId=%08X - ignored", priority, thisId );
					}
					else
						subscr->priority = priority;

					subscr->_safeBeginTrans();
					subscr->processSyncResponse( parser );
					subscr->_safeCommitTrans();
					subscr->_safeSynchronized(); 
					subscr->state = CommSubscription::_StateSubscrSync;
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			//no trace

			CommClientSubscriber* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state != CommSubscription::_StateSubscrSync )
				{
					PLog( "CommClientSubscriberPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
				}
				else
				{ // It could happen that we will get update for frozen subscription.
					// Ignore this message silently, don't want to disturb user.
					subscr->_safeBeginTrans();
					subscr->processOnlineUpdate( parser );
					subscr->_safeCommitTrans();
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );

			CommClientSubscriber* subscr = 0;
			if( peerId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
				subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state == CommSubscription::_StateSubscrSync )
					subscr->_safeDesynchronized();

				if (subscr->state == CommSubscription::_StateSubscrFrozenDesync ||
					subscr->state == CommSubscription::_StateSubscrFrozenDisc)
				{
					// Don't call closed(). User will get closed when defrost.
					subscr->state = CommSubscription::_StateSubscrFrozenDisc;
				}
				else
				{
					subscr->state = CommSubscription::_StateSubscrServerFatalError;
					bool ack = subscr->_safeClosed( errCode, errMsg ); 
					if( !ack )
						_scheduleResubscribe( *subscr );
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientSubscriber* subscr = 0;
			if( peerId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
				subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state == CommSubscription::_StateSubscrSync )
				{
					subscr->_safeDesynchronized();
					subscr->state = CommSubscription::_StateSubscrDesync;
				}
				//else state should not be changed
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_RESYNC:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientSubscriberPool: _COMM_MSGTYPE_SUBSCRIPTION_RESYNC: thisId=%08X peerId=%08X size=%d", thisId, peerId, msg.body._size() );

			CommClientSubscriber* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state != CommSubscription::_StateSubscrDesync )
				{
					PLog( "CommClientSubscriberPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_RESYNC msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
				}
				else
				{
					subscr->_safeBeginTrans();
					subscr->processSyncResponse( parser );
					subscr->_safeCommitTrans();
					subscr->_safeSynchronized();
					subscr->state = CommSubscription::_StateSubscrSync;
				}
			}
		}
		return true;
	}//switch
	return false;
}

/* static */ void CommClientSubscriberPool::parseMessageOnTheFly( CommSubscription::OnTheFlyCallback& callback, const _CommMsg& msg, UINT32 subscrId )
{
	if( msg.type != _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE )
		return;

	CommMsgParser parser( msg.body );
	UINT32 thisId, peerId;
	parser.parseUINT32( thisId ).parseUINT32( peerId );

	if( subscrId && thisId != subscrId )
		return;

	CommSubscription::parseMessageOnTheFly( callback, parser );
}
