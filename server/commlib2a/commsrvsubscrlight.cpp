#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commsrvsubscrlight.h"


CommServerPublisherLight::CommServerPublisherLight()
{
	pool = 0;
	//	child = 0;
	priority = _COMM_MSGPRIORITY_INITSUBSCR;
	numberOfSubscribers=0;
	countingOn = false;
}

void CommServerPublisherLight::postUpdate( CommMsgBody& update )
{
	pool->_postUpdate( *this, update );
}

void CommServerPublisherLight::startCountNumberOfSubscribers()
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

void CommServerPublisherLight::_startCountNumberOfSubscribers()
{
	for( _SubscrSet::iterator iter = children.begin() ; iter != children.end() ; ++iter )
	{
		CommMsgBody body;
		_Subscr * child = (*iter);
		body.composeUINT32( child->peerId ).composeUINT32( child->thisId ).composeBYTE(1);
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST, priority, body );
		pool->inter->_post( m );
	}
}

void CommServerPublisherLight::stopCountNumberOfSubscribers()
{
	if(countingOn)
	{
		countingOn = false;
		for( _SubscrSet::iterator iter = children.begin() ; iter != children.end() ; ++iter )
		{
			CommMsgBody body;
			_Subscr * child = (*iter);
			body.composeUINT32( child->peerId ).composeUINT32( child->thisId ).composeBYTE(0);
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST, priority, body );
			pool->inter->_post( m );
		}
	}
}

//****************************************************************************

CommServerPublisherLightPool::CommServerPublisherLightPool( _CommInterface& inter_ )
{
	inter = &inter_;
	connected = false;
}

void CommServerPublisherLightPool::publish( const char* name, CommServerPublisherLight& publisher )
{
	PASSERT3( publisher.pool == 0 );

	_PublisherMap::iterator found = publisherMap.find( name );
	PASSERT3( found == publisherMap.end() );

	publisher.pool = this;
	publisher.name = name;
	publisherMap.insert( _PublisherMap::value_type( name, &publisher ) );
}

void CommServerPublisherLightPool::unpublish( const char* name )
{
	_PublisherMap::iterator found = publisherMap.find( name );
	if (found == publisherMap.end())
	{
		PLog( "CommServerPublisherPool::unpublish: subscription '%s' not found", name);
		return;
	}
	CommServerPublisherLight* publisher = (*found).second;
	PASSERT3( publisher->pool == this );
	publisher->pool = 0;
	publisher->name = "";
	while(publisher->children.size())
	{
		CommServerPublisherLight::_SubscrSet::iterator iter = publisher->children.begin();
		CommServerPublisherLight::_Subscr * child = (*iter);
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, publisher->priority );
		m.body.composeUINT32( child->peerId ).composeUINT32( child->thisId ).composeUINT16( _COMM_ERROR_SUBSCRIPTION_UNPUBLISHED ).composeString( "Subscription no longer exists" );
		inter->_post( m );
		_delSubscr( child );
	}
	publisherMap.erase( found );
}

CommServerPublisherLight::_Subscr* CommServerPublisherLightPool::_getSubscr( UINT32 thisId, BYTE msgType )
{
	CommServerPublisherLight::_Subscr** subscrPtr = subscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommServerPublisherLightPool: msg type=%02X for unknown subscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	CommServerPublisherLight::_Subscr* subscr = *subscrPtr;
	PASSERT3( subscr->thisId == thisId );
	return subscr;
}

CommServerPublisherLight::_Subscr* CommServerPublisherLightPool::_getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	CommServerPublisherLight::_Subscr* subscr = _getSubscr( thisId, msgType );
	if( subscr == 0 )
		return 0;
	if( peerId != subscr->peerId )
	{
		PLog( "CommServerPublisherLightPool: msg type=%02X for subscrId=%08X with msg.peerId=%08X != subscr.peerId=%08X - ignored", msgType, thisId, peerId, subscr->peerId );
		return 0;
	}
	return subscr;
}

void CommServerPublisherLightPool::_delSubscr( CommServerPublisherLight::_Subscr* subscr )
{
	CommServerPublisherLight::_SubscrSet::iterator iter = subscr->publisher->children.find(subscr);
	PASSERT3(iter != subscr->publisher->children.end());
	if (subscr->publisher->countingOn)
	{
		subscr->publisher->numberOfSubscribers = 0;
	}
	subscr->publisher->children.erase(iter);
	subscrMap.remove( subscr->thisId );
	delete subscr;
}

CommServerPublisherLight::_Subscr* CommServerPublisherLightPool::_lookForSubscr( UINT32 cliId )
{
	int i;
	for( CommServerPublisherLight::_Subscr** s = subscrMap.firstIndex( i ); s ; s = subscrMap.nextIndex( i ) )
		if( (*s)->peerId == cliId )
			return *s;
	return 0;
}

bool CommServerPublisherLightPool::_processMessage( _CommMsg& msg )
{
	PASSERT3( inter );
	if( !connected && msg.type != _COMM_MSGTYPE_PHYSICAL_CONNECT )//just a precaution
	{
		PLog( "CommServerPublisherLightPool: msgType = %02X when !connected - ignored", msg.type );
		return false;
	}

	switch( msg.type )
	{
	case _COMM_MSGTYPE_PHYSICAL_CONNECT:
		connected = true;
		return false;//not the only message consumer

	case _COMM_MSGTYPE_PHYSICAL_DISCONNECT:
		{
			PTRACE3( "CommServerPublisherLightPool: _COMM_MSGTYPE_PHYSICAL_CONNECT" );
			if( connected )
			{
				PLog( "CommServerPublisherLightPool: _COMM_MSGTYPE_PHYSICAL_CONNECT in connected state - ignored" );
				return false;
			}

			int n = subscrMap.size();
			if( n )
			{
				int i, subscrI = 0;
				PArrayAutoPtr< CommServerPublisherLight::_Subscr* > subscrs( new CommServerPublisherLight::_Subscr*[ n ] );//although it seems safe to call _delSubscr within subscrMap iteration, but it changes subscrMap, so we will move it out of the loop - just in case
				for( CommServerPublisherLight::_Subscr** subscrPtr = subscrMap.firstIndex( i ); subscrPtr ; subscrPtr = subscrMap.nextIndex( i ) )
				{
					CommServerPublisherLight::_Subscr* subscr = *subscrPtr;
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

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST:
		{
			CommMsgParser parser( msg.body );
			UINT32 peerId;
			const char* server;
			const char* serverObject;
			const char* subscrChannel;
			parser.parseUINT32( peerId ).parseString( server ).parseString( serverObject ).parseString( subscrChannel );
			PTRACE4( "CommServerPublisherLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST: peerId=%08X server='%s' serverObject='%s' subscrChannel='%s'", peerId, server, serverObject, subscrChannel );

			_PublisherMap::iterator found = publisherMap.find( subscrChannel );
			if( found == publisherMap.end() )
			{
				PTRACE3( "CommServerPublisherLightPool: Subscription ('%s'/'%s'/'%s') denied (unknown channel)", server, serverObject, subscrChannel );
				_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
				m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_NOPUBLISHER ).composeString( "Unknown publisher channel" );
				inter->_post( m );
				return true;
			}

			CommServerPublisherLight::_Subscr* subscr = new CommServerPublisherLight::_Subscr();
			subscr->publisher = (*found).second;
			subscr->thisId = subscrMap.add( subscr );
			subscr->peerId = peerId;
			subscr->publisher->children.insert( subscr );

			{
				_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED, subscr->publisher->priority );
				m.body.composeUINT32( subscr->peerId ).composeUINT32( subscr->thisId );
				inter->_post( m );
			}

			{
				_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE, subscr->publisher->priority );
				m.body.composeUINT32( subscr->peerId ).composeUINT32( subscr->thisId ).composeBYTE( subscr->publisher->priority );
//				subscr->publisher->processSyncRequest( m.body, parser );
				inter->_post( m );
			}

			if (subscr->publisher->countingOn)
			{
				subscr->publisher->_startCountNumberOfSubscribers();
			}

			PTRACE3( "CommServerPublisherLightPool: Subscription ('%s'/'%s'/'%s') accepted (thisId=%08X) and granted", server, serverObject, subscrChannel, subscr->thisId );
		}
		return true;

	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			parser.parseUINT32( thisId ).parseUINT32( peerId );
			PTRACE3( "CommServerPublisherLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE: thisId=%08X peerId=%08X", thisId, peerId );

			CommServerPublisherLight::_Subscr* subscr = 0;
			if( thisId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
			{//special case
				subscr = _lookForSubscr( peerId );
				if( !subscr )
				{
					PLog( "CommServerPublisherLightPool: Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE with cliId=%08X", peerId );
				}
			}

			if( subscr )
				_delSubscr( subscr );
		}
		return true;
	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE:
		{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId,count;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32(count);
			PTRACE4( "CommServerPublisherLightPool: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE: thisId=%08X peerId=%08X count=%d", thisId, peerId,count );
			CommServerPublisherLight::_Subscr* subscr = 0;
			if( thisId )
				subscr = _getSubscr( thisId, peerId, msg.type );
			else
			{//special case
				PLog( "CommServerPublisherLightPool: Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE with cliId=%08X", peerId );
			}
			if( subscr )
			{
				PASSERT3( subscr->publisher->children.find(subscr) != subscr->publisher->children.end() );
				subscr->publisher->numberOfSubscribers = count;
			}
		}
		return true;
	}
	return false;
}

void CommServerPublisherLightPool::_postUpdate( CommServerPublisherLight& publisher, CommMsgBody& update )
{
	if (publisher.children.size() > 1)
	{
		for( CommServerPublisherLight::_SubscrSet::iterator iter = publisher.children.begin() ; iter != publisher.children.end() ; ++iter )
		{
			CommMsgBody body;
			CommServerPublisherLight::_Subscr * child = (*iter);
			body.copyFrom(update);
			body._createPrefix(8);
			BYTE* p = body._writePtr();
			CommMsgBody::writeUINT32( p, child->peerId );
			p += 4;
			CommMsgBody::writeUINT32( p, child->thisId );
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE, publisher.priority, body );
			inter->_post( m );
		}
	}
	else if (publisher.children.size() == 1)
	{
		CommServerPublisherLight::_Subscr * child = *publisher.children.begin();
		update._createPrefix(8);
		BYTE* p = update._writePtr();
		CommMsgBody::writeUINT32( p, child->peerId );
		p += 4;
		CommMsgBody::writeUINT32( p, child->thisId );
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE, publisher.priority, update );
		inter->_post( m );
	}
}
//****************************************************************************


