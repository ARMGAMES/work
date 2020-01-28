#include "_commextthreadmanager.h"



#if !defined( COMM_CLIENT_ONLY )

class _CommExtAcceptThread : public PThread
{
private:
	//	bool exited;
	CommExtThreadManager& mgr;
	PIPListeningSocket sock;
	bool shutdownFlag;
	bool exitFlag;

public:
	_CommExtAcceptThread( CommExtThreadManager& mgr_, int port );
	void run() override;
	void shutdown();
};



//****************************************************************************

_CommExtAcceptThread::_CommExtAcceptThread( CommExtThreadManager& mgr_, int port )
: mgr( mgr_ )
{
	shutdownFlag=false;
	exitFlag= false;
	sock.bind( port );
}

void _CommExtAcceptThread::run()
{
	try
	{

		PTRACE3( "_CommExtAcceptThread: listen( 5 )");

		sock.listen( 200 );
		for(;;)
		{
			PIPSocket::Descriptor accepted;
			sock.accept( accepted );
			if (shutdownFlag)
			{
				PTRACE3( "_CommExtAcceptThread: thread shutdown");
				break;
			}
			PString addr;
			PIPSocket::address( addr, accepted );
			int port = PIPSocket::port( accepted );
			PString addr2;
			mgr.composeSocketAddr( addr2, addr, port);
			PLog( "Incoming connection from '%s'", addr2.c_str() );
			mgr._pushInSocket( accepted );
		}
	}
	catch( PError& err )
	{
		PLog( "_CommExtAcceptThread exception: %s", err.why() );
	}
	catch(  exception& exc  )
	{
		PLog( "_CommExtAcceptThread  %s",  exc.what() );
	}
	catch( ... )
	{
		PLog( "_CommExtAcceptThread ...");
	}

	exitFlag = true;
	//	exited = true;
}

void _CommExtAcceptThread::shutdown()
{
	if (shutdownFlag == false)
	{
		sock.close();
	}
	shutdownFlag=true;
}

#endif	// !defined( COMM_CLIENT_ONLY )

//****************************************************************************


class _CommExtConnectThread : public PThread
{
private:
	CommExtThreadManager& mgr;
	bool shutdownFlag;
	bool exitFlag;
	struct Item
	{
		PString address;
		UINT32 connId;
		void * ssl;
		Item()
		{
			connId = 0;
			ssl = 0;
		}
		Item(UINT32 connId_, const char* address_, void * ssl_)
		{
			connId = connId_;
			address = address_;
			ssl = ssl_;
		}
	};
	typedef PQueue< PQDeque<Item> > Queue;
	Queue q;

public:
	_CommExtConnectThread( CommExtThreadManager& mgr_ );
	void pushConnection( UINT32 connId, const char *address, void * ssl_)
	{
		q.push(Item(connId,address,ssl_));
	}
	void shutdown()
	{
		q.shutdown();
	}
	void run() override;
private:
	void _connect( Item & it);
};



_CommExtConnectThread::_CommExtConnectThread( CommExtThreadManager& mgr_ )
: mgr( mgr_ )
{
	shutdownFlag=false;
	exitFlag= false;
}

void _CommExtConnectThread::run()
{
	try
	{
		for(;;)
		{
			Item it;
			int ok = q.waitPop( it );
			switch( ok )
			{
			case Queue::waitPopOk:
				_connect( it );
				break;
			case Queue::waitPopShutdown:
				//					exited = true;
				return;
			default:
				PASSERT3( 0 );
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "_CommExtConnectThread exception: %s", err.why() );
	}
	catch(  exception& exc  )
	{
		PLog( "_CommExtConnectThread  %s",  exc.what() );
	}
	catch( ... )
	{
		PLog( "_CommExtConnectThread ...");
	}


	//	exited = true;
}

void _CommExtConnectThread::_connect(Item & it)
{
	try
	{
		PString addr;
		int port;
		mgr.parseSocketAddr( addr, port, it.address );

		mgr.physConnConnecting( it.connId );

		PIPSocket sock;
		if (mgr.connectTimeout > 0)
		{
			if (!sock.connect( addr, port, mgr.connectTimeout*1000 ))
			{
				throw PError( "Unable to connect: connection timeout");
			}
		}
		else
		{
			sock.connect( addr, port);
		}

		mgr.physConnConnected( it.connId );

		PBaseSocket::Descriptor sockDescr;
		sock.extractDescriptor(sockDescr );
		mgr._pushOutSocket(it.connId, sockDescr, it.ssl );
	} 
	catch( PError& err )
	{

		PLog( "_CommExtConnectThread to '%s' failed: %s", it.address.c_str(), err.why() );
		mgr.disconnected( it.connId , -1, err.why());
	}
	catch(  exception& exc  )
	{
		PLog( "_CommExtConnectThread to %s: %s", it.address.c_str(),  exc.what() );
		mgr.disconnected( it.connId , -1,exc.what());
	}
	catch( ... )
	{
		PLog( "_CommExtConnectThread to '%s' failed: ...", it.address.c_str());
		mgr.disconnected( it.connId , -1, "...");
	}

}

bool _CommExtManagableThread::_freeSocket( int idx ) //to solve access problem for _RWThread
{
	return mgr._freeSocket( idx );
}

CommExtThreadManager::~CommExtThreadManager()
{
	shutdown();
	while(terminated() == false)
		PThread::sleep(100);
	{
		PLock lock( csThread );
		int n = threads.size();
		for( int i=0; i < n ; ++i )
			delete threads[ i ].thread;
#if !defined( COMM_CLIENT_ONLY )
		n = acceptThreads.size();
		for( int i=0; i < n ; ++i )
			delete acceptThreads[ i ];
#endif
		if (connThread)
			delete connThread;
	}
	if (incoming)
	{
		PLock lock( csPhysConn );
		int i;
		for( _CommExtPhysConnection** c = physConnMap.firstIndex( i ); c ; c = physConnMap.nextIndex( i ) )
		{
			_CommExtPhysConnection* conn = *c;
			delete conn;
		}
		physConnMap.clear();
	}
}

#if !defined( COMM_CLIENT_ONLY )
void CommExtThreadManager::listenPort( int port )
{
	PASSERT3(incoming);
	PLock lock( csThread );
	_CommExtAcceptThread* acceptThread = new _CommExtAcceptThread( *this, port );
	acceptThreads.push_back( acceptThread );
}
#endif
void CommExtThreadManager::start()
{
	if (incoming == false)
	{
		connThread = new _CommExtConnectThread( *this );
		connThread->start();
	}
#if !defined( COMM_CLIENT_ONLY )
	else
	{
		int n = acceptThreads.size();
		for( int i=0; i < n ; ++i )
			acceptThreads[ i ]->start();
	}
#endif
}
void CommExtThreadManager::shutdown()
{
	PLock lock( csThread );
	if (connThread)
		connThread->shutdown();

#if !defined( COMM_CLIENT_ONLY )
	for( auto&& iter : acceptThreads )
		iter->shutdown();
#endif

	for( auto&& iter : threads )
	{
		if( iter.thread != 0 )
			iter.thread->shutdown();
	}
}
bool CommExtThreadManager::terminated()
{
	PLock lock( csThread );
	if( connThread && !connThread->terminated() )
	{
		return false;
	}

#if !defined( COMM_CLIENT_ONLY )
	for( auto&& iter : acceptThreads )
	{
		if( !iter->terminated() )
			return false;
	}
#endif

	for( auto&& iter : threads )
	{
		if( iter.thread != 0 && !iter.thread->terminated() )
			return false;
	}
	return true;
}

void CommExtThreadManager::connect( _CommExtPhysConnection * conn, const char * address, void * ssl_)
{
	connThread->pushConnection( conn->connId, address, ssl_ );
}

UINT32 _CommExtManagableThread::_nextChannelId()
{
	return mgr._nextChannelId();
}
