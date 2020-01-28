#ifndef _commextthreadmanager_h_included
#define _commextthreadmanager_h_included

#include "ppinclude.h"
#include "commlib.h"

class CommExtThreadManager;
class _CommExtManagableThread;
#if !defined( COMM_CLIENT_ONLY )
class _CommExtAcceptThread;
#endif
class _CommExtConnectThread;

class _CommExtPhysConnection
{
private:
//	PBaseSocket::Descriptor sockDescr;
	UINT32 connId;
	CommExtThreadManager *_mgr;

public:
	PString address;
	int slot;
	int threadId;
	UINT32 channelId;

protected:
	_CommExtPhysConnection()
	{
		threadId = -1;
		slot = -1;
		channelId = 0;
		connId = 0;
		_mgr = 0;
		
	}
	virtual ~_CommExtPhysConnection()
	{
	}
	CommExtThreadManager * mgr() const
	{
		return _mgr;
	}
public:
	UINT32 id() const
	{
		return connId;
	}

public: // sync calls
	virtual void syncConnecting() {}
	virtual void syncConnected() {}

friend class CommExtThreadManager;
friend class _CommExtManagableThread;
#if !defined( COMM_CLIENT_ONLY )
friend class _CommExtAcceptThread;
#endif
friend class _CommExtConnectThread;
};



class _CommExtManagableThread : public PThread
{

private:
	bool exited;
protected:
enum { _KeepAlivePoll = 1000 };
	CommExtThreadManager& mgr;
	CommMsgQueue q;
	int mgrIdx;
	UINT32 _nextChannelId();
	 
public:
	_CommExtManagableThread(CommExtThreadManager& mgr_, int mgrIdx_):mgr(mgr_)
	{
		mgrIdx = mgrIdx_;
		exited = false;
	}

public:
	void push( /*non-const!*/ _CommMsg& msg )
	{
		q.push( msg );
	}
	void exit()
	{
		exited = true;
	}

	void shutdown()
	{
		q.shutdown();
	}
	bool inStateOfExit() const
	{
		return exited;
	}
	void _pushConnection( CommMsgBody &body)
	{
		_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INITCONNECT,body );
		m.internal.slot = 0xFFFF;
		q.push( m );
	}

	void _pushDisconnect( const _CommExtPhysConnection & physConn )
	{
		_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		m.internal.slot = static_cast< UINT16 >( physConn.slot );
		m.internal.channelId = physConn.channelId;
		q.push( m );
	}
	bool _freeSocket( int idx );
};




class CommExtThreadManager
{
private:
	struct Thread
	{
		_CommExtManagableThread* thread;
		int maxSlots;
		int slotsLeft;
	};

	PCriticalSection csThread;
	PCriticalSection csPhysConn;
	vector< Thread > threads;
	UINT32 channelIdGen;
	CommIdMap<_CommExtPhysConnection *> physConnMap;
	
	_CommExtConnectThread* connThread;
#if !defined( COMM_CLIENT_ONLY )
	vector< /* new */ _CommExtAcceptThread* > acceptThreads;
#endif
	UINT32 connectTimeout;
	UINT32 maxNumberOfConnectionsPerThread;
	
protected:
	bool incoming;

public:

	UINT32 handshakeTimeout;
	CommExtThreadManager(bool incoming_)
	{
		incoming=incoming_;
		connThread=0;
		channelIdGen = RANDOMTIME();
		connectTimeout = 0;
		handshakeTimeout = 0;
		maxNumberOfConnectionsPerThread=0;
	}
	virtual ~CommExtThreadManager();

#if !defined( COMM_CLIENT_ONLY )
	void listenPort( int port );
#endif
	void setConnectTimeout(UINT32 connectTimeOut_)
	{
		connectTimeout = connectTimeOut_;
	}
	void setHandshakeTimeout(UINT32 handshakeTimeout_)
	{
		handshakeTimeout = handshakeTimeout_;
	}
	void setConnectionsPerThread(UINT32 maxNumberOfConnectionsPerThread_)
	{
		maxNumberOfConnectionsPerThread = maxNumberOfConnectionsPerThread_;
	}

	_CommExtPhysConnection * findPhysConn(UINT32 connId)
	{
		PLock lock(csPhysConn);
		_CommExtPhysConnection **r = physConnMap.find(connId);
		if (r)
		{
			return *r;
		}
		else 
			return 0;
	}
	UINT32 addPhysConn(_CommExtPhysConnection * conn)
	{
		PLock lock(csPhysConn);
		PASSERT3(conn->connId == 0);
		PASSERT3(conn->_mgr == 0);
		UINT32 connId = physConnMap.add(conn);
		conn->connId = connId;
		conn->_mgr = this;
		return connId;
	}
	_CommExtPhysConnection * removePhysConn(UINT32 connId)
	{
		PLock lock(csPhysConn);
		_CommExtPhysConnection **r = physConnMap.find(connId);
		if (r)
		{
			physConnMap.remove(connId);
			(*r)->connId=0;
			(*r)->_mgr=0;
			return *r;
		}
		else
			return 0;
	}



	void push( _CommExtPhysConnection * physConn,CommMsgBody & body )
	{
		PLock lock( csThread );
		PASSERT3(physConn->threadId >= 0 && physConn->threadId < static_cast< int >( threads.size() ));
		_CommExtManagableThread* thread = threads[ physConn->threadId ].thread;
		if( !thread )
		{
			PLog( "CommExtThreadManager: Thread #%d is already deleted - msg ignored", physConn->threadId );
			return;
		}
		_CommMsg m( _COMM_MSGTYPE_INTERNAL_COMMUNICATION, _COMM_MSGPRIORITY_INITCONNECT,body );
		m.internal.slot = static_cast< UINT16 >( physConn->slot );
		m.internal.channelId = physConn->channelId;
		thread->push( m );
	}

	void start();
	void shutdown();
	bool terminated();

	void connect( _CommExtPhysConnection * conn, const char * address, void * ssl_ );
	void forceDisconnect(const _CommExtPhysConnection & physConn )
	{
		PLock lock( csThread );
		if (physConn.threadId >= 0)
		{
			PASSERT3(physConn.threadId < static_cast< int >( threads.size() ));
			Thread& thread = threads[ physConn.threadId ];
			thread.thread->_pushDisconnect( physConn );
		}
	}

	virtual _CommExtManagableThread* _createNewThread(int threadIdx)
	{
		return new _CommExtManagableThread(*this,threadIdx);
	}
	virtual _CommExtPhysConnection* _createConnection()
	{
		PASSERT3(0);

	}
	// call backs
	virtual void readDone(UINT32 connId, CommMsgBody &body)
	{
	}
	virtual void sentDone(UINT32 connId)
	{
	}
	virtual void connected(UINT32 connId)
	{
	}
	virtual void disconnected(UINT32 connId, int errCode, const char * errMsg)
	{
	}
	virtual void connecting(UINT32 connId, const char * address, UINT32 threadId, UINT32 slot, UINT32 channelId)
	{
	}

	// sync call back from _CommExtConnectThread::_connect()
	virtual void physConnConnecting( UINT32 connId )
	{
	}
	virtual void physConnConnected( UINT32 connId )
	{
	}

private:

	UINT32 _nextChannelId()
	{
		PLock lock( csThread );
		if( ++channelIdGen == 0 )
			channelIdGen = 1;
		return channelIdGen;
	}

	void _checkExitingThreads()
	{
		int n = threads.size();
		for( int i=0; i < n ; ++i )
		{
			Thread& thread = threads[ i ];
			if( thread.thread != 0 && thread.thread->inStateOfExit() == true && thread.thread->terminated())
			{
				delete thread.thread;
				thread.thread = 0;
			}
		}
	}

	int _findSlot()//returns threadIdx. MUST be locked before call
	{
		_checkExitingThreads();
		int n = threads.size();
		for( int i=0; i < n ; ++i )
		{
			Thread& thread = threads[ i ];
			if( thread.thread != 0 && thread.thread->inStateOfExit() == false)
			{ 
				if(maxNumberOfConnectionsPerThread)
				{
					if (thread.slotsLeft > (thread.maxSlots - static_cast< int >( maxNumberOfConnectionsPerThread )) )
						return i;
				}
				else
				{
					if (thread.slotsLeft > 0)
						return i;
				}
			}

		}
		return -1;
	}

	int _allocThread()//returns threadIdx. MUST be locked before call
	{
		int n = threads.size();
		for( int i=0; i < n ; ++i )
		{
			Thread& thread = threads[ i ];
			if( thread.thread == 0)
				return i;
		}

		threads.push_back( Thread() );
		return threads.size() - 1;
	}
	void _pushConnection( CommMsgBody & body)
	{
		PLock lock( csThread );
		int threadIdx = _findSlot();
		if( threadIdx >= 0 )
		{
			PASSERT3(threadIdx < static_cast< int >( threads.size() ));
			Thread& thread = threads[ threadIdx ];
			thread.thread->_pushConnection(body);
			--thread.slotsLeft;
			return;
		}

		threadIdx = _allocThread();
		Thread& thread = threads[ threadIdx ];
		thread.thread = _createNewThread( threadIdx );
		thread.slotsLeft = thread.maxSlots = PMultiSocketRW::MaxSockets;
		thread.thread->start();
		thread.thread->_pushConnection(body);
		--thread.slotsLeft;
	}

	bool _freeSocket( int threadIdx )
	{
		PLock lock( csThread );
		PASSERT3(threadIdx >= 0 && threadIdx < static_cast< int >( threads.size() ));
		Thread& thread = threads[ threadIdx ];
		if( ++thread.slotsLeft != thread.maxSlots || threadIdx == 0 )
			return false;
		thread.thread->exit();
		return true;
	}

	void _pushInSocket(PIPSocket::Descriptor & accepted)
	{
		_CommExtPhysConnection *conn = _createConnection();
		addPhysConn(conn);
		CommMsgBody body;
		body.composeUINT32(conn->id());
		body._composeFixedBlock((const BYTE *)&accepted, sizeof(PBaseSocket::Descriptor));
		body.composeBOOL(true);
		_pushConnection(body);
	}
	void _pushOutSocket(UINT32 connId, PIPSocket::Descriptor & connected, void * ssl)
	{
		CommMsgBody body;
		body.composeUINT32(connId);
		body._composeFixedBlock((const BYTE *)&connected, sizeof(PBaseSocket::Descriptor));
		body.composeBOOL(false);
		body._composeFixedBlock((const BYTE *)&ssl, sizeof(void *));
		_pushConnection(body);
	}
	friend class _CommExtAcceptThread;
	friend class _CommExtConnectThread;
	friend class _CommExtManagableThread;


public:
	static void parseSocketAddr( PString& retAddr, int& port, const char* address )
	{
		const char* col = strchr( address, ':' );
		if( col )
		{
			retAddr.assign( address, col );
			port = atoi( col + 1 );
		}
		else
			retAddr = address;
	}
	static void composeSocketAddr( PString& retAddr, const char* address, int port )
	{
		retAddr.append( address ).append( ':' ).appendInt( port );
	}


};







#endif
