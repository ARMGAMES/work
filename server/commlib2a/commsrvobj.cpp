#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commconn.h"

CommServerObject::CommServerObject( _CommInterface& inter_ )
{
	//slot = -1;
	inter = &inter_;
	//	timer = 0;
	_exited = false;
}

#if 0
void CommServerObject::_init( /* int slot_, UINT32 channelId_,*/ _CommTimerInterface& timer_ )
{
	PASSERT3( timer == 0 /*&& slot < 0*/ );
	//slot = slot_;
	//channelId = channelId_;
	timer = &timer_;
}
#endif

int CommServerObject::getInQueueSize() const
{
	return inter->_getInQueueSize();
}
UINT64 CommServerObject::getWaitInQueueTime(UINT64 *readTime , UINT64 *writeTime )
{
	return inter->_getWaitInQueueTime(readTime,writeTime);
}

UINT32 CommServerObject::postTimerMessage( UINT32 msgId, CommMsgBody& body, int msec )
{
	//	PASSERT3( timer && slot >= 0 );
	//	_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INVALID );
	//	msg.body.moveFrom( body );
	//	msg.internal.slot = slot;
	//	msg.internal.channelId = channelId;
	//	msg.internal.patch0 = msgId;
	//	return timer->addMsg( msg, msec );

	//	return timer->_addTimer( msgId, body, msec );
	return inter->_addServerTimer( msgId, body, msec );
}

bool CommServerObject::deleteTimerMessage( UINT32 handle )
{
	//	return timer->_delTimer( handle );
	return inter->_delServerTimer( handle );
}

void CommServerObject::exit()
{
	_exited = true;
}

bool CommServerObject::_safeInit( CommMsgBody& body )
{
	try
	{
		PSIGRETURN()
			init( body );
	}
	catch( PError& err )
	{
		PLog( "PError caught/object will be destroyed in ServerObject::init(): %s", err.why() );
		return true;
	}
	catch( ... )
	{
		PLog( "... caught/object will be destroyed in ServerObject::init()" );
		return true;
	}
	return exited();
}

void CommServerObject::_safeShutdown()
{
	try
	{
		PSIGRETURN()
			shutdown();
	}
	catch( PError& err )
	{
		PLog( "PError caught in ServerObject::shutdown(): %s", err.why() );
		return;
	}
	catch( ... )
	{
		PLog( "... caught in ServerObject::shutdown()" );
		return;
	}
}

int CommServerObject::_safeTerminated()
{
	try
	{
		PSIGRETURN()
			return terminated();
	}
	catch( PError& err )
	{
		PLog( "PError caught in ServerObject::terminated(): %s", err.why() );
		return -1;
	}
	catch( ... )
	{
		PLog( "... caught in ServerObject::terminated()" );
		return -1;
	}
}

//static PCriticalSection _cs;
bool CommServerObject::_safeProcessMessage( _CommMsg& msg )
{
	try
	{
		//PLock lock( _cs );
		//PASSERT( _CrtCheckMemory() );
		PSIGRETURN()
			processMessage( msg );
		//PASSERT( _CrtCheckMemory() );
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in ServerObject::processMessage( %02X ): %s", msg.type, err.why() );
	}
#ifdef _DEBUG
	catch( const exception& exc )
	{
		PLog( "STD exception caught/ignored in ServerObject::processMessage( %02X ): %s", msg.type, exc.what() );
	}
#endif
	catch( ... )
	{
		PLog( "... caught/ignored in ServerObject::processMessage( %02X )", msg.type );
	}
	return exited();
}

bool CommServerObject::_safeProcessTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body )
{
	try
	{
		PSIGRETURN()
			processTimerMessage( handle, msgId, body );
	}
	catch( PError& err )
	{
		PLog( "PError caught/ignored in ServerObject::processTimerMessage( %02X ): %s", msgId, err.why() );
	}
#ifdef _DEBUG
	catch( const exception& exc )
	{
		PLog( "STD exception caught/ignored in ServerObject::processTimerMessage( %02X ): %s", msgId, exc.what() );
	}
#endif
	catch( ... )
	{
		PLog( "... caught/ignored in ServerObject::processTimerMessage( %02X )", msgId );
	}
	return exited();
}
