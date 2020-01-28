#ifndef commextmultithreadspool_h_included
#define commextmultithreadspool_h_included

#include "ppinclude.h"
#include "ppblock.h"
#include "ppthread.h"
#include "ppipc.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commlib.h"
#include "commsrv.h"

 

class CommExtMultithreadQueue
{
public:

private:
	PCriticalSection cs;
	PNamedEvent _event;
	CommPriorityQueue< _CommMsgQueueItem >  _queue;
	
	bool _shutdown;

public:
	CommExtMultithreadQueue()
	{
		_shutdown = false;
		_event.create(true,0);
	}
public:
	void push( _CommMsg& msg )
	{
		_CommMsgQueueItem item;
		new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
		PLock lock( cs );
		if( _shutdown )
			return;
		_queue.push( item );
		_event.set();
	}

	int waitPop(  _CommMsg& ret )
	{
		for(;;)
		{
			PLock lock( cs );
			if( _shutdown )
			{
				_event.set();
				return PNamedEvent::waitFailed;
			}

			if( _queue.size() == 0 )
			{
				lock.unlock();
				int result = _event.wait(-1);
				if( result == PNamedEvent::waitFailed )
					return PNamedEvent::waitFailed;
			}
			else
			{
				_CommMsgQueueItem item;
				item = _queue.peek();
				_queue.pop();
				ret.moveFrom( *item.msg() );
				return PNamedEvent::waitOk;
			}
		}
	}


	void shutdown()
	{
		PLock lock( cs );
		_shutdown = true;
		_event.set();

	}

};


class CommExtMultithreadsPool;



class CommExtMultithreadsContent
{
public:
	virtual ~CommExtMultithreadsContent() {}
};


class CommExtMultithreadsThread : public PThread
{

private:
	CommExtMultithreadsPool & pool;
	CommExtMultithreadsContent * content;

public:
	CommExtMultithreadsThread(CommExtMultithreadsPool & pool_, CommExtMultithreadsContent * content_): pool(pool_)
	{
		content = content_;
	}

	~CommExtMultithreadsThread()
	{
		if (content)
			delete content;
	}

	void run() override;

};



class CommExtMultithreadsPool 
{

private:
	CommExtMultithreadQueue q;
	_CommInterface & baseInterface;
	CommServerConnectionPool & srvConnectionPool;
	vector<CommExtMultithreadsThread*> threads;


public:

	CommExtMultithreadsPool(_CommInterface & baseInterface_, CommServerConnectionPool & srvConnectionPool_) :
	        baseInterface(baseInterface_),srvConnectionPool(srvConnectionPool_)
    {
	}
	void start(CommExtMultithreadsContent * content)
	{
		threads.push_back(new CommExtMultithreadsThread(*this,content));
		threads[threads.size()-1]->start();
	}
	~CommExtMultithreadsPool()
    {
		q.shutdown();
		for( int i=0;i<threads.size(); i++)
		{
			threads[i]->join();
		}
		for( int i=0;i<threads.size(); i++)
		{
			delete threads[i];
		}

	}

	bool _processMessage( _CommMsg& msg )
	{
		if (msg.type != _COMM_MSGTYPE_INTERNAL_COMMUNICATION)
			return false;
		CommServerConnection *conn = srvConnectionPool.findConnection( msg.internal.patch0);
		if (conn)
		{
			conn->post(msg.internal.patch1,msg.body);
		}
		return true;
	}
	void pustToThread(UINT32 msgId, const CommMsgBody &body, UINT32 connId); // call from the main thread to push message to the first available thread
	virtual void processMessage(UINT32 msgId, CommMsgBody &body, UINT32 connId, CommExtMultithreadsContent * content ) = 0; // called by framework in thread context
	void postReply(UINT32 msgId, CommMsgBody & reply, UINT32 connId); // post reply message from thread


friend class CommExtMultithreadsThread;


};







#endif
