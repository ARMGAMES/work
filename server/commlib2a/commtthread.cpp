#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commtthread.h"

UINT32 _CommTimerThread::push( /*non-const!*/ _CommMsg& msg, int msec )
{
	msg.internal.patch1 = msec;
	
	PLock lock(cs);
	UINT32 handle = commTimer.addMsg(msg, msec);
	dummyQueue.push(0 /*dummy number*/);
	return handle;
}

void _CommTimerThread::shutdown()
{
	dummyQueue.shutdown();
}

/* virtual */ void _CommTimerThread::run()
{
	try
	{
		for(;;)
		{
			int timeout;
			for(;;)
			{
				timeout = commTimer.msecUntilFirst();
				if( timeout == 0 )
				{
					_CommMsg msg;
					commTimer.pop( msg );
					timer( msg );
				}
				else
					break;
			}

			int w;
			int dummy;
			if( timeout < 0 )
				w = dummyQueue.waitPop(dummy);
			else
				w = dummyQueue.waitPop(dummy, timeout + 1 );

			switch( w )
			{
			case CommMsgQueue::waitPopOk:
				//do nothing - message was already added to timer
				break;

			case CommMsgQueue::waitPopShutdown:
				PTRACE3( "_CommTimerThread shutdown - exiting" );
				//				_exit();
				return;

			case CommMsgQueue::waitPopTimeout:
				//do nothing - action will be done in the beginning of the forever cycle
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "_CommTimerThread exception: %s", err.why() );
	}
}
