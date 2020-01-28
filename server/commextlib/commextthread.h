#ifndef commextthread_h_included
#define commextthread_h_included

#include "ppinclude.h"
#include "ppblock.h"
#include "ppthread.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commidmap.h"
#include "commmsgbody.h"
#include "commqueue.h"

class CommExtThreadPool;

class CommExtThread : public PThread
{
protected:
	CommMsgTimeControlQueue q;

private:
	CommExtThreadPool * pool;
	bool exited;
	UINT32 thisId;

	void _safeProcessMessageFromThread( UINT32 msgId, CommMsgBody& body)
	{
		try
		{
			processMessageFromThread( msgId, body);
		}
		catch( PError& err )
		{
			PLog( "PError caught/ignored in CommExtThread::_safeProcessMessageFromThread( %02X ): %s", msgId, err.why() );
		}
		catch( const exception& exc )
		{
			PLog( "STD exception caught/ignored in CommExtThread::_safeProcessMessageFromThread( %02X ): %s", msgId, exc.what() );
		}
		catch( ... )
		{
			PLog( "... caught/ignored in CommExtThread::_safeProcessMessageFromThread( %02X )", msgId );
		}
	}

	void _finished();

public:
	CommExtThread()
	{
		pool = 0;
		thisId = 0;
		exited=false;
	}
	void postToParent( UINT32 msgId, CommMsgBody& body);

	void postToThread( UINT32 msgId, CommMsgBody& body, void* ptr, BYTE priority = _COMM_MSGPRIORITY_INITCONNECT )
	{
		static_assert( sizeof( UINT64 ) >= sizeof( void* ), "Cannot use UINT64 to store a pointer!" );
		const UINT64 ptrAsUint64 = (uintptr_t)ptr;
		UINT32 param0 = ptrAsUint64 >> 32;
		UINT32 param1 = ptrAsUint64 & 0xFFFFFFFF;
		postToThread( msgId, body, param0, param1, priority );
	}
	void postToThread( UINT32 msgId, CommMsgBody& body, UINT32 param0 = 0, UINT32 param1 = 0, BYTE priority = _COMM_MSGPRIORITY_INITCONNECT )
	{
		PASSERT3(!exited);
		_CommMsg msg( _COMM_MSGTYPE_INTERNAL_COMMUNICATION, priority );
		msg.body.moveFrom( body );
		msg.internal.channelId = param0;
		msg.internal.muxChannelId = param1;
		msg.internal.patch0 = thisId;
		msg.internal.patch1 = msgId;
		q.push( msg );
	}
	virtual void extRun() = 0;
	int wait( int timeout, UINT32& msgId, CommMsgBody& body, void*& ptr )
	{
		// check that UINT64 is enough to store a pointer
		static_assert( sizeof( UINT64 ) >= sizeof( void* ), "Cannot use UINT64 to store a pointer!" );
		UINT32 param0 = 0, param1 = 0;
		int retCode = wait( timeout, msgId, body, param0, param1 );
		ptr = (void*)( (UINT64)param0 << 32 | (UINT64)param1 );
		return retCode;
	}
	int wait(int timeout, UINT32 & msgId, CommMsgBody & body, UINT32 & param0, UINT32 & param1 )
	{
		int w;
		_CommMsg msg;
		if( timeout < 0 )
			w = q.waitPop( msg );
		else
			w = q.waitPop( msg, timeout );
		switch( w )
		{
			case CommMsgQueue::waitPopOk:
				{
					PASSERT3(msg.internal.patch0 == thisId );
					param0 = msg.internal.channelId;
					param1 = msg.internal.muxChannelId;
					msgId = msg.internal.patch1;
					body.moveFrom(msg.body);
					return w;
				}
			case CommMsgQueue::waitPopShutdown:
				{
					return w;
				}
			case CommMsgQueue::waitPopTimeout:
				{
					return w;
				}
			default:
				PASSERT3(0);
		}
	}
	bool pop(UINT32 & msgId, CommMsgBody & body, UINT32 & param0, UINT32 & param1 )
	{
		_CommMsg msg;
		if(q.pop(msg))
		{
			PASSERT3(msg.internal.patch0 == thisId );
			param0 = msg.internal.channelId;
			param1 = msg.internal.muxChannelId;
			msgId = msg.internal.patch1;
			body.moveFrom(msg.body);
			return true;
		}
		else
			return false;
	}


	void run() override
	{
		try
		{
			extRun();
			exited=true;
			_finished();
		}
		catch( PError& err )
		{
			PLog( "PError caught/ignored in CommExtThread::run() : %s", err.why() );
		}
		catch( const exception& exc )
		{
			PLog( "STD exception caught/ignored in CommExtThread::run(): %s", exc.what() );
		}
		catch( ... )
		{
			PLog( "... caught/ignored in CommExtThread::run()");
		}

	}

	virtual void processMessageFromThread( UINT32 msgId, CommMsgBody& body) = 0;

friend class CommExtThreadPool;
};



class CommExtThreadPool
{
	_CommInterface & baseInterface;
private:
	CommIdMap< CommExtThread* > connMap;

public:
	CommExtThreadPool(_CommInterface & baseInterface_) :
	  baseInterface(baseInterface_)
	  {
	  }
	int _getInQueueSize() const
	{
		return baseInterface._getInQueueSize();
	}

	virtual void threadFinished(UINT32 id, CommExtThread * p){}

    void _postToParent(_CommMsg& msg)
	{
		baseInterface._internalPost(msg);
	}
	void connect(CommExtThread & thread)
	{
		PASSERT3(thread.pool == 0);
		thread.pool = this;
		thread.thisId = connMap.add( &thread );
	}

	bool _processMessage( _CommMsg& msg )
	{
		if (msg.type == _COMM_MSGTYPE_INTERNAL_COMMUNICATION_CLOSE)
		{
			UINT32 id = msg.internal.patch0;
			CommExtThread **thread = connMap.find(id);
			if (thread == 0)
			{
				PLog("CommExtThreadPool::_processMessage: 'close' to thread %08X: thread not found",msg.internal.patch0);
				return true;
			}
			connMap.remove(id);
			threadFinished(id,*thread);
			return true;
		}


		if (msg.type != _COMM_MSGTYPE_INTERNAL_COMMUNICATION)
			return false;
		UINT32 id = msg.internal.patch0;
		CommExtThread **thread = connMap.find(id);
		if (thread == 0)
		{
			PLog("CommExtThreadPool::_processMessage: message %d to thread %08X: thread not found",msg.internal.patch1,msg.internal.patch0);
			return true;
		}
		(*thread)->_safeProcessMessageFromThread(msg.internal.patch1,msg.body);
		return true;
	}





};

#endif