#include "commextmultithreadspool.h"


void CommExtMultithreadsThread::run()
{
	PTRACE3( "CommExtMultithreadsThread: thread started");
	for(;;)
	{
		try
		{
			 _CommMsg  ret;
			 if (pool.q.waitPop(ret) == PNamedEvent::waitFailed)
			 {
				 PTRACE3( "CommExtMultithreadsThread: thread finished");
				 return;
			 }
			 pool.processMessage(ret.internal.patch1,ret.body,ret.internal.patch0, content);
		}
		catch( PError& err )
		{
			PLog( "PError caught/ignored in CommExtMultithreadsThread::run() : %s", err.why() );
		}
		catch( const exception& exc )
		{
			PLog( "STD exception caught/ignored in CommExtMultithreadsThread::run(): %s", exc.what() );
		}
		catch( ... )
		{
			PLog( "... caught/ignored in CommExtMultithreadsThread::run()");
		}
	}

}


void CommExtMultithreadsPool::pustToThread(UINT32 msgId, const CommMsgBody &body, UINT32 connId)
{
	_CommMsg msg( _COMM_MSGTYPE_INTERNAL_COMMUNICATION, _COMM_MSGPRIORITY_INITCONNECT );
	msg.body._moveFromConstBody( body );
	msg.internal.patch0 = connId;
	msg.internal.patch1 = msgId;
	q.push( msg );
}

void CommExtMultithreadsPool::postReply(UINT32 msgId, CommMsgBody & reply, UINT32 connId)
{
	_CommMsg msg( _COMM_MSGTYPE_INTERNAL_COMMUNICATION, _COMM_MSGPRIORITY_INITCONNECT );
	msg.body.moveFrom( reply );
	msg.internal.patch0 = connId;
	msg.internal.patch1 = msgId;
	return baseInterface._internalPost(msg);
}

