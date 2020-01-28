#include "commtmrwudp.h"
#include "commproto.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commtmrw.h"
#include "commtmrwplain.h"
#include "commoption.h"
#include "commmonitorlogger.h"

#ifdef PWIN32

//****************************************************************************


/* virtual */ bool CommRWUDPThreadManager::handles( const char* address ) const
{
	return CommUdp0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWUDPThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommUdp0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWUDPThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommUdp0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}

#endif

#ifdef UDT_DEVELOPMENT
/* virtual */ _CommRWThreadBase* CommRWThreadManagerUDT::_createNewThread( int threadIdx )
{
	_CommRWThread< _CommRWPlain<PMultiSocketRWUDT>,PMultiSocketRWUDT > *thread = new _CommRWThread< _CommRWPlain<PMultiSocketRWUDT>,PMultiSocketRWUDT >( *this, threadIdx, muxEnabled );
	thread->rw.setSocketContainer(&pudtContainer);
	return thread;
}

/* virtual */ bool CommRWThreadManagerUDT::handles( const char* address ) const
{
	return CommUdt0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManagerUDT::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommUdt0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWThreadManagerUDT::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommUdt0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}

void CommRWThreadManagerUDT::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	CommRWThreadManagerBase::loggerInformation(body);

	if (isGatherStatistic())
	{
		PString s;
		s.assign("Max queue size(").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qSize);

		s.assign("Max time in queue (").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qTime);
		qSize=0;
		qTime=0;
		if (isGatherStatistic()>1)
		{
			s.assign("Max slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotSize);
			s.assign("Max time in slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotTime);
			qSlotSize=0;
			qSlotTime=0;
            
            list<PUDTConnectionData> connDataList = pudtContainer.getConnectionData();
            for_each(connDataList.cbegin(), connDataList.cend(),
                     [&body](const PUDTConnectionData& connData)
                     {
                         PString flowWindowSize("UDTConnection(");
                         flowWindowSize.append(connData.address).append(")").append(".flowWindowSize");
                         PString congestionWindowSize("UDTConnection(");
                         congestionWindowSize.append(connData.address).append(")").append(".congestionWindowSize");
                         PString interPacketInterval("UDTConnection(");
                         interPacketInterval.append(connData.address).append(")").append(".interPacketInterval");
                         PString sendLostListSize("UDTConnection(");
                         sendLostListSize.append(connData.address).append(")").append(".sendLostListSize");
                         PString totalRetransmit("UDTConnection(");
                         totalRetransmit.append(connData.address).append(")").append(".totalRetransmit");
                         PString totalPacketsSent("UDTConnection(");
                         totalPacketsSent.append(connData.address).append(")").append(".totalPacketsSent");
                         PString sendPacketLoss("UDTConnection(");
                         sendPacketLoss.append(connData.address).append(")").append(".sendPacketLoss");
                         PString recvLostListSize("UDTConnection(");
                         recvLostListSize.append(connData.address).append(")").append(".recvLostListSize");
                         PString totalRecvPacketLoss("UDTConnection(");
                         totalRecvPacketLoss.append(connData.address).append(")").append(".totalRecvPacketLoss");
                         PString totalPacketsRecv("UDTConnection(");
                         totalPacketsRecv.append(connData.address).append(")").append(".totalPacketsRecv");
                         PString totalRecvPacketDropped("UDTConnection(");
                         totalRecvPacketDropped.append(connData.address).append(")").append(".totalRecvPacketDropped");
                         PString totalDupPacketsRecvd("UDTConnection(");
                         totalDupPacketsRecvd.append(connData.address).append(")").append(".totalDupPacketsRecvd");
                         PString recvPacketLoss("UDTConnection(");
                         recvPacketLoss.append(connData.address).append(")").append(".recvPacketLoss");
                         
						 composeLoggerStat(body, flowWindowSize.c_str(), connData.flowWindowSize);
						 composeLoggerStat(body, congestionWindowSize.c_str(), connData.congestionWindowSize);
						 composeLoggerStat(body, interPacketInterval.c_str(), connData.interPacketInterval);
						 composeLoggerStat(body, sendLostListSize.c_str(), connData.sendLostListSize);
						 composeLoggerStat(body, totalRetransmit.c_str(), connData.totalRetransmit);
						 composeLoggerStat(body, totalPacketsSent.c_str(), connData.totalPacketsSent);
						 composeLoggerStat(body, sendPacketLoss.c_str(), connData.sendPacketLoss.c_str());
						 composeLoggerStat(body, recvLostListSize.c_str(), connData.recvLostListSize);
						 composeLoggerStat(body, totalRecvPacketLoss.c_str(), connData.totalRecvPacketLoss);
						 composeLoggerStat(body, totalPacketsRecv.c_str(), connData.totalPacketsRecv);
						 composeLoggerStat(body, totalRecvPacketDropped.c_str(), connData.totalRecvPacketDropped);
						 composeLoggerStat(body, totalDupPacketsRecvd.c_str(), connData.totalDupPacketsRecvd);
						 composeLoggerStat(body, recvPacketLoss.c_str(), connData.recvPacketLoss.c_str());
                     });
		}
	}
}
void CommRWThreadManagerUDT::shutdown()
{
	pudtContainer.shutdown();
	CommRWThreadManagerBaseIP::shutdown();
}






void CommRWThreadManagerUDT::setOption( const char* optName, const char* optValue )
{
	if( strcmp( optName, COMM_OPTION_INUDPBUFFER ) == 0 )
		inUdpBuffer = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_OUTUDPBUFFER ) == 0 )
		outUdpBuffer = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDPMSS ) == 0 )
		udpMSS = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDT_INFLIGHTSIZE ) == 0 )
		udpFlightFlagSize = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDT_SENDBUFFER ) == 0 )
		udpMaxSendBufferSize = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDT_RECVBUFFER ) == 0 )
		udpMaxRecvBufferSize = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDT_RTT ) == 0 )
		udpRTT = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDT_BANDWIDTH ) == 0 )
		udpEstimatedBandwidth = atoi( optValue );
	else
		CommRWThreadManagerBaseIP::setOption( optName, optValue );
}

class _CommRWAcceptThreadUDT : public _CommRWAcceptThread
{
private:
	//	bool exited;
	CommRWThreadManagerUDT& mgr;
	PUDTSOCKET serv;
	bool shutdownFlag;
	bool exitFlag;


public:
	_CommRWAcceptThreadUDT( CommRWThreadManagerUDT& mgr_, int port );
	void run() override;
	void shutdown() override;
	void threadState(CommMsgBody &body) override
	{
	}

};



//****************************************************************************

_CommRWAcceptThreadUDT::_CommRWAcceptThreadUDT( CommRWThreadManagerUDT& mgr_, int port )
: mgr( mgr_ )
{

	shutdownFlag=false;
	exitFlag= false;
#ifdef UDT_DLL
   addrinfo hints;
   addrinfo* res;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   //hints.ai_socktype = SOCK_DGRAM;

   PString service;
   service.appendInt(port);
   if (0 != getaddrinfo(NULL, service.c_str(), &hints, &res))
   {
	   PASSERT3(0);
   }

    serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   // UDT Options
   //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
   //UDT::setsockopt(serv, 0, UDT_MSS, new int(9000), sizeof(int));
   UDT::setsockopt(serv, 0, UDT_MSS, new int(1052), sizeof(int));
   UDT::setsockopt(serv, 0, UDT_FC, new int(100000), sizeof(int));
   UDT::setsockopt(serv, 0, UDT_RCVBUF, new int(100000000), sizeof(int));
   UDT::setsockopt(serv, 0, UDP_RCVBUF, new int(100000000), sizeof(int));
   UDT::setsockopt(serv, 0, UDT_SNDBUF, new int(100000000), sizeof(int));
   UDT::setsockopt(serv, 0, UDP_SNDBUF, new int(100000000), sizeof(int));



   if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
   {
	   PASSERT3(0);
   }

   freeaddrinfo(res);
#else
	serv = mgr.pudtContainer.create();
#endif
}

/* virtual */ void _CommRWAcceptThreadUDT::run()
{
	try
	{
#ifdef UDT_DLL
	   if (PUDTERROR == UDT::listen(serv, mgr.listenBacklog))
	   {
		   PASSERT3(0);
	   }
		for(;;)
		{
		   sockaddr clientaddr;
		   int addrlen = sizeof(clientaddr);

		   UDTSOCKET recver;

           recver = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen);
		   if (recver == UDT::INVALID_SOCK)
			   PASSERT3(0);
 
			if (shutdownFlag)
			{
				PTRACE3( "_CommRWAcceptThreadUDT: thread shutdown");
				break;
			}
			if (mgr._allowNewConnection())
			{
				PIPSocket::Descriptor accepted;
				accepted.sock = recver;
				accepted.addr = clientaddr;
				PString addr;
				PIPSocket::address( addr, accepted );
				int port = PIPSocket::port( accepted );

				PString addr2;
				mgr.composeSocketAddr( addr2, addr, port );
				PLog( "Incoming connection from '%s'", addr2.c_str() );
				mgr._pushInSocket( accepted, addr2 );
			}
			else
			{
				PTRACE3( "_CommRWAcceptThreadUDT: number of connections exceeds limit" );
				UDT::close(recver);
			}
		}
#else
		if (mgr.pudtContainer.listen(serv) < 0)
	    {
		   PASSERT3(0);
	    }
		for(;;)
		{

			PUDPSocket::Descriptor accepted;
			int ret = mgr.pudtContainer.accept(serv,accepted);
			if (ret < 0)
			{
				PTRACE3( "_CommRWAcceptThreadUDT: socket closed");
				break;
			}
			if (shutdownFlag)
			{
				PTRACE3( "_CommRWAcceptThreadUDT: thread shutdown");
				break;
			}
			if (mgr._allowNewConnection())
			{

				PString addr;
				PUDPSocket::address( addr, accepted );
				int port = PUDPSocket::port( accepted );

				PString addr2;
				mgr.composeSocketAddr( addr2, addr, port );
				PLog( "Incoming connection from '%s'", addr2.c_str() );
				mgr._pushInSocket( accepted, addr2 );
			}
			else
			{
				PTRACE3( "_CommRWAcceptThreadUDT: number of connections exceeds limit" );
				mgr.pudtContainer.close(accepted.sock);
			}
		}

#endif
	}
	catch( PError& err )
	{
		PLog( "RWAcceptThreadUDT exception: %s", err.why() );
	}
	exitFlag = true;
	//	exited = true;
}

void _CommRWAcceptThreadUDT::shutdown()
{
	if (shutdownFlag == false)
	{
#ifdef UDT_DLL
		UDT::close(serv);
#else
		mgr.pudtContainer.close(serv);
#endif
	}
	shutdownFlag=true;
}

//****************************************************************************

class _CommRWConnectThreadUDT : public _CommRWConnectThread
{
private:
	CommRWThreadManagerUDT& mgr;

public:
	_CommRWConnectThreadUDT( CommRWThreadManagerUDT& mgr_ );
public:
	void run() override;
private:
	void _connect( _CommPhysicalConnection* conn );
};



_CommRWConnectThreadUDT::_CommRWConnectThreadUDT( CommRWThreadManagerUDT& mgr_ )
: mgr( mgr_ )
{
	//	exited = false;
}

/* virtual */ void _CommRWConnectThreadUDT::run()
{
	try
	{
		for(;;)
		{
			_CommPhysicalConnection* conn;
			int ok = q.waitPop( conn );
			switch( ok )
			{
			case Queue::waitPopOk:
				_connect( conn );
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
		PLog( "RWConnectThread exception: %s", err.why() );
	}
	//	exited = true;
}

void _CommRWConnectThreadUDT::_connect( _CommPhysicalConnection* conn )
{
	if (mgr.connectIsOutOfProcess())
	{
		PASSERT3(0);
	}
	else
	{
		try
		{
			//resolve conn->address
			PString addr;
			int port;
			mgr.parseSocketAddr( addr, port, conn->address );
#ifdef UDT_DLL			
			struct addrinfo hints, *local, *peer;
			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_flags = AI_PASSIVE;
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;

			if (0 != getaddrinfo(NULL, "9000", &hints, &local))
			{
				PASSERT3(0);
			}

			UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);

			// UDT Options
			//UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
			//UDT::setsockopt(client, 0, UDT_MSS, new int(9000), sizeof(int));
			UDT::setsockopt(client, 0, UDT_MSS, new int(1052), sizeof(int));
			UDT::setsockopt(client, 0, UDT_FC, new int(100000), sizeof(int));
			UDT::setsockopt(client, 0, UDT_SNDBUF, new int(100000000), sizeof(int));
			UDT::setsockopt(client, 0, UDP_SNDBUF, new int(100000000), sizeof(int));
			UDT::setsockopt(client, 0, UDT_RCVBUF, new int(100000000), sizeof(int));
			UDT::setsockopt(client, 0, UDP_RCVBUF, new int(100000000), sizeof(int));
			freeaddrinfo(local);
			PString portS;
			portS.appendInt(port);

			if (0 != getaddrinfo(addr.c_str(), portS.c_str(), &hints, &peer))
			{
				PASSERT3(0);
			}

			if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
			{
				PASSERT3(0);
			}
	

			CommRWThreadManagerBase::Socket rwSock;
			rwSock.sockDescr.addr = *(peer->ai_addr);
			freeaddrinfo(peer);
			rwSock.sockDescr.sock = client;
			rwSock.physConn = conn;
			mgr._pushOutSocket( rwSock );
#else
			PUDTSOCKET sock =  mgr.pudtContainer.create();
			PUDPSocket::Descriptor descr;
			if ( mgr.pudtContainer.connect(sock, addr, port, descr ) <=0 )
			{
				 mgr.pudtContainer.close(sock);
				throw PError("UDT connection can not connect");
			}
			CommRWThreadManagerBase::Socket rwSock;
			rwSock.sockDescr = descr;
			rwSock.sockDescr.sock = sock;
			rwSock.physConn = conn;
			mgr._pushOutSocket( rwSock );
#endif
		}
		catch( PError& err )
		{
			PLog( "_CommRWConnectThreadUDT to '%s' failed: %s", conn->address.c_str(), err.why() );
			mgr.routingTable.physicalConnectFailed( conn );
		}
	}
}


void CommRWThreadManagerUDT::listenPort( int port )
{
	PWLock lock( cs );
	if (listenPorts.size() > 0)
	{
		PLog("CommRWThreadManagerUDT::listenPort only one UDP port is allowed");
		return;
	}
	listenPorts.push_back(port);
}

void CommRWThreadManagerUDT::start()
{
	if (listenPorts.size() > 0)
	{
		pudtContainer.socket->bind(listenPorts[0]);
	}
	else
	{
		pudtContainer.socket->bind(0);
	}
	if(inUdpBuffer)
		pudtContainer.socket->udpSetRecvBuffer(inUdpBuffer);
	if(outUdpBuffer)
		pudtContainer.socket->udpSetSendBuffer(outUdpBuffer);

	if (udpMSS)
		pudtContainer.udpMSS = udpMSS;
	if (udpFlightFlagSize)
		pudtContainer.udpFlightFlagSize = udpFlightFlagSize;
	if (udpMaxSendBufferSize)
		pudtContainer.udpMaxSendBufferSize = udpMaxSendBufferSize;
	if (udpMaxRecvBufferSize)
		pudtContainer.udpMaxRecvBufferSize = udpMaxRecvBufferSize;
	if (udpRTT)
		pudtContainer.udpRTT = udpRTT;
	if (udpEstimatedBandwidth)
		pudtContainer.udpEstimatedBandwidth = udpEstimatedBandwidth;

	pudtContainer.start();
	if (listenPorts.size() > 0)
	{
		_CommRWAcceptThread* acceptThread = new _CommRWAcceptThreadUDT( *this, listenPorts[0] );
		acceptThreads.push_back( acceptThread );
	}

	CommRWThreadManagerBaseIP::start();
}


CommRWThreadManagerUDT::CommRWThreadManagerUDT( CommRoutingTable& routingTable_, bool enableMux )
		: CommRWThreadManagerBaseIP( routingTable_, enableMux )
{
	inUdpBuffer = 0;
	outUdpBuffer = 0;
	udpMSS = 0;
	udpFlightFlagSize = 0;
	udpMaxSendBufferSize = 0;
    udpMaxRecvBufferSize = 0;
	udpRTT = 0;
	udpEstimatedBandwidth = 0;

#ifdef UDT_DLL
	UDT::startup();
#endif
	connThread = new _CommRWConnectThreadUDT(*this);
}

CommRWThreadManagerSSLUDT::CommRWThreadManagerSSLUDT( CommRoutingTable& routingTable_, bool enableMux )
		: CommRWThreadManagerUDT( routingTable_, enableMux )
{
}


#endif