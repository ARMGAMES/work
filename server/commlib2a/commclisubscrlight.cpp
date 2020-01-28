#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commclisubscrlight.h"

CommClientSubscriberLight::CommClientSubscriberLight()
{
	pool = 0;
	priority = 0;
}

CommClientSubscriberLight::~CommClientSubscriberLight()
{
	if( pool )
	{
		// shouldn't  this subscription be removed from IdMap?
		// Vadim, Please discuss this with Dmitry L. & Victor G.
		PLog( "Error: CommClientSubscriberLight::~CommClientSubscriberLight(): destroying non-unsubscribed subscription" );
	}
}

/* virtual */ bool CommClientSubscriberLight::closed( int errCode, const char* errMsg )
{
	if( errCode == _COMM_ERROR_NOLOCALOBJECT || errCode == _COMM_ERROR_INBOUNDCONNCLOSED || errCode == _COMM_ERROR_NOPUBLISHER || errCode == _COMM_ERROR_SUBSCRIPTION_UNPUBLISHED )
		return false;
	return true;
}

void CommClientSubscriberLight::_safeProcessUpdate(const CommMsgBody & body)
{
	try
	{
		PSIGRETURN()
			processUpdate(body);
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriberLight: exception in processUpdate() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommClientSubscriberLight: unknown exception in processUpdate() caught" );
	}
}


bool CommClientSubscriberLight::_safeClosed( int errCode, const char* errMsg )
{
	try
	{
		PSIGRETURN()
			return closed( errCode, errMsg );
	}
	catch( PError& err )
	{
		PLog( "CommClientSubscriberLight: exception in closed() caught: %s", err.why() );
		return true;
	}
	catch( ... )
	{
		PLog( "CommClientSubscriberLight: unknown exception in closed() caught" );
		return true;
	}
}

//***************************************************************************/

CommClientSubscriberLightPool::CommClientSubscriberLightPool( _CommInterface& inter_ )
{
	inter = &inter_;
	connected = false;
}

CommClientSubscriberLightPool::~CommClientSubscriberLightPool()
{
	//+++ disconnect
}

CommClientSubscriberLight* CommClientSubscriberLightPool::_getSubscr( UINT32 thisId, BYTE msgType )
{
	CommClientSubscriberLight** subscrPtr = subscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommClientSubscriberLightPool: msg type=%02X for unknown subscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	CommClientSubscriberLight* subscr = *subscrPtr;
	PASSERT3( subscr->thisId == thisId ); // catch 'Assertion failed: , file ..\commlib2a\commclisubscr.cpp, line 139'
	// Vadim, Please discuss this with Dmitry L. & Victor G.
	return subscr;
}

CommClientSubscriberLight* CommClientSubscriberLightPool::_getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	CommClientSubscriberLight* subscr = _getSubscr( thisId, msgType );
	if( subscr == 0 )
		return 0;
	if( peerId != subscr->peerId )
	{
		PLog( "CommClientSubscriberLightPool: msg type=%02X for subcrId=%08X with msg.peerId=%08X != subscr.peerId=%08X - ignored", msgType, thisId, peerId, subscr->peerId );
		return 0;
	}
	return subscr;
}

void CommClientSubscriberLightPool::_subscribe( CommClientSubscriberLight& subscr )
{
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST, subscr.priority );
	m.body.composeUINT32( subscr.thisId ).composeString( subscr.server ).composeString( subscr.serverObject ).composeString( subscr.serverChannel );
//	subscr.genSyncRequest( m.body );
	inter->_post( m );

	subscr.state = CommClientSubscriberLight::_StateSubscrRequestSent;
	subscr.peerId = 0;//for special error cases
}

void CommClientSubscriberLightPool::_scheduleResubscribe( CommClientSubscriberLight& subscr )
{
	PASSERT3( subscr.state == CommClientSubscriberLight::_StateSubscrServerFatalError );

	_CommMsg msg( _COMM_MSGTYPE_SUBSCRLIGHTPOOL_TIMER, 0 );
	msg.body.composeUINT32( subscr.thisId );
	inter->_postClientTimer( msg, _ResubscribeTimeout );
}

void CommClientSubscriberLightPool::_resubscribe( CommClientSubscriberLight& subscr )
{
	subscrMap.remove( subscr.thisId );
	subscr.thisId = subscrMap.add( &subscr );
	_subscribe( subscr );
}

void CommClientSubscriberLightPool::_postUnsubscribe( CommClientSubscriberLight& subscr )
{
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE, subscr.priority );
	m.body.composeUINT32( subscr.peerId ).composeUINT32( subscr.thisId );
	inter->_post( m );
}


void CommClientSubscriberLightPool::subscribe( CommClientSubscriberLight& subscr, const char* server, const char* serverObject, const char* serverChannel )
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
		subscr.state = CommClientSubscriberLight::_StateSubscrDisc;
}

void CommClientSubscriberLightPool::unsubscribe( CommClientSubscriberLight& subscr )
{
	PASSERT3( subscr.pool == this );
	switch( subscr.state )
	{
	case CommClientSubscriberLight::_StateSubscrDisc:
	case CommClientSubscriberLight::_StateSubscrServerFatalError:
		//do nothing
		break;

	
	case CommClientSubscriberLight::_StateSubscrRequestSent:
	case CommClientSubscriberLight::_StateSubscrPeerAssigned:
	case CommClientSubscriberLight::_StateSubscrSync:
	case CommClientSubscriberLight::_StateSubscrDesync:
		_postUnsubscribe( subscr );
		break;

	default:
		PASSERT3( 0 );
		break;
	}
	subscrMap.remove( subscr.thisId );
	subscr.server = "";
	subscr.serverObject = "";
	subscr.serverChannel = "";
	subscr.pool = 0;
}




bool CommClientSubscriberLightPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( msg.type == _COMM_MSGTYPE_SUBSCRLIGHTPOOL_TIMER )
	{
		CommMsgParser parser( msg.body );
		UINT32 subscrId;
		parser.parseUINT32( subscrId );

		CommClientSubscriberLight** subscrPtr = subscrMap.find( subscrId );
		if( subscrPtr == 0 )
			PTRACE3( "CommClientSubscriberLightPool: timer msg for unknown subscrId=%08X - ignored", subscrId );
		else
		{
			CommClientSubscriberLight* subscr = *subscrPtr;
			_resubscribe( *subscr );
		}

		return true;
	}

	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PLog( "CommClientSubscriberLightPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	switch( msg.type )
	{
	case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		{
			PTRACE3( "CommClientSubscriberLightPool: _COMM_MSGTYPE_PHYSICAL_CONNECT" );
			if( connected )
			{
				PLog( "CommClientSubscriberLightPool: _COMM_MSGTYPE_PHYSICAL_CONNECT in connected state - ignored" );
				return false;
			}

			int n = static_cast< int >( subscrMap.size() );
			if( n )
			{
				int i, subscrI = 0;
				PArrayAutoPtr< CommClientSubscriberLight* > subscrs( new CommClientSubscriberLight*[ n ] );//!cannot call _resubscribe within subscrMap iteration as it changes subscrMap
				for( CommClientSubscriberLight** subscrPtr = subscrMap.firstIndex( i ); subscrPtr ; subscrPtr = subscrMap.nextIndex( i ) )
				{
					CommClientSubscriberLight* subscr = *subscrPtr;
					PASSERT3( subscr->state == CommClientSubscriberLight::_StateSubscrDisc );
					PASSERT3( subscrI < n );
					if (subscr->state == CommClientSubscriberLight::_StateSubscrDisc)
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
			PTRACE3( "CommClientSubscriberLightPool: _COMM_MSGTYPE_PHYSICAL_DISCONNECT" );
			int i;
			for( CommClientSubscriberLight** subscrPtr = subscrMap.firstIndex( i ); subscrPtr ; subscrPtr = subscrMap.nextIndex( i ) )
			{
				CommClientSubscriberLight* subscr = *subscrPtr;
				subscr->state = CommClientSubscriberLight::_StateSubscrDisc;
			}
			connected = false;
		}
		return false;//not the only message consumer

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientSubscriberLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientSubscriberLight* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state != CommClientSubscriberLight::_StateSubscrRequestSent )
				{
					PLog( "CommClientSubscriberLightPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
				}
				else
				{
					subscr->peerId = peerId;
					subscr->state = CommClientSubscriberLight::_StateSubscrPeerAssigned;
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			BYTE priority;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority );
			PTRACE3( "CommClientSubscriberLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE: thisId=%08X peerId=%08X priority=%d size=%d", thisId, peerId, priority, msg.body._size() );

			CommClientSubscriberLight* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				// To solve the problem with priorities
				if( subscr->state == CommClientSubscriberLight::_StateSubscrRequestSent )
				{
					subscr->peerId = peerId;
					subscr->state = CommClientSubscriberLight::_StateSubscrPeerAssigned;
				}
				// end
				if( subscr->state != CommClientSubscriberLight::_StateSubscrPeerAssigned &&
					subscr->state != CommClientSubscriberLight::_StateSubscrDesync)
				{
					PLog( "CommClientSubscriberPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
				}
				else
				{
					if( priority >= _COMM_MSGPRIORITIES )
					{
						PLog( "CommClientSubscriberLightPool: invalid priority=%d in _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE for subscrId=%08X - ignored", priority, thisId );
					}
					else
						subscr->priority = priority;
					subscr->state = CommClientSubscriberLight::_StateSubscrSync;
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			//no trace

			CommClientSubscriberLight* subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state != CommClientSubscriberLight::_StateSubscrSync )
				{
					PLog( "CommClientSubscriberLightPool: unexpected _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
				}
				else
				{
					msg.body._shift(8);
					subscr->_safeProcessUpdate( msg.body );
				}
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			UINT16 errCode;
			const char* errMsg;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT16( errCode ).parseString( errMsg );
			PTRACE3( "CommClientSubscriberLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR: thisId=%08X peerId=%08X errCode=%02X errMsg='%s'", thisId, peerId, (int)errCode, errMsg );

			CommClientSubscriberLight* subscr = 0;
			if( peerId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
				subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				subscr->state = CommClientSubscriberLight::_StateSubscrServerFatalError;
				bool ack = subscr->_safeClosed( errCode, errMsg ); 
				if( !ack )
					_scheduleResubscribe( *subscr );
			}
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT:
		{
			PASSERT3( connected );
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommClientSubscriberLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

			CommClientSubscriberLight* subscr = 0;
			if( peerId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
				subscr = _getSubscr( thisId, msg.type );
			if( subscr )
			{
				if( subscr->state == CommClientSubscriberLight::_StateSubscrSync )
				{
					subscr->state = CommClientSubscriberLight::_StateSubscrDesync;
				}
				//else state should not be changed
			}
		}
		return true;
	}//switch
	return false;
}

