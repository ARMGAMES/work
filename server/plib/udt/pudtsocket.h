#ifndef pudtsocket_h_included
#define pudtsocket_h_included
#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT

#include "ppsocket.h"
#include "pudtdefine.h"
#include "pudtpacket.h"
#include "pudtbuffer.h"
#include "pudtlosslist.h"
#include "pudtwindow.h"
#include "pudttimer.h"
#include "pudtccc.h"
#include "ppidmap.h"



class PUDTSocketContainer;

class PUDTRecvThread : public PThread
{
private:
	PUDTSocketContainer & cont;
public:
	PUDTRecvThread(PUDTSocketContainer & cont_):cont(cont_)
	{}
	void run() override;

};


class PUDTSendThread : public PThread
{
private:
	PUDTSocketContainer & cont;
public:
	PUDTSendThread(PUDTSocketContainer & cont_):cont(cont_)
	{}
	void run() override;

};


struct PUDTConnectionData
{
    PUDTConnectionData() : 
        flowWindowSize(0),
        congestionWindowSize(0),
        interPacketInterval(0),
        sendLostListSize(0),
        totalRetransmit(0),
        totalPacketsSent(0),
        recvLostListSize(0),
        totalRecvPacketLoss(0),
        totalPacketsRecv(0),
        totalRecvPacketDropped(0),
        totalDupPacketsRecvd(0) {}

    PString address;
    UINT32 flowWindowSize;
    UINT32 congestionWindowSize;
    UINT32 interPacketInterval;
    UINT32 sendLostListSize;
    UINT32 totalRetransmit;
    UINT32 totalPacketsSent;
    PString sendPacketLoss;
    UINT32 recvLostListSize;
    UINT32 totalRecvPacketLoss;
    UINT32 totalPacketsRecv;
    UINT32 totalRecvPacketDropped;
    UINT32 totalDupPacketsRecvd;
    PString recvPacketLoss;
};

class PUDTConnection
{
private:
	PUDTSocketContainer & cont;
	PCriticalSection ackLock;    // m_AckLock



	_PWin32Event _sendEvent;
	_PWin32Event _recvEvent;

public:
	PInterlocked     accessed;
	PUDTReceiveBuffer		* recvBuffer;
	PUDTSendBuffer			* sendBuffer;
	PUDTCCC					* ccc;
	PUDTReceiveLossList		* recvLostList;
	PUDTSendLossList		* sendLostList;
	PUDTACKWindow			* ackWindow;
	PUDTTimeWindow			* recvTimeWindow;
	PUDTTimeWindow			* sendTimeWindow;

    enum PUDTSOCKETSTATUS {INIT = 1, OPENED, LISTENING, CONNECTING, CONNECTED, BROKEN, CLOSING, CLOSED, NONEXIST};

	volatile PUDTSOCKETSTATUS status; // enum UDTSTATUS {INIT = 1, OPENED, LISTENING, CONNECTING, CONNECTED, BROKEN, CLOSING, CLOSED, NONEXIST};
	
	UINT64 startTime;                   // timestamp when the UDT entity is started                                               m_StartTime      
	volatile UINT64 lastRespTime;       // time stamp of last response from the peer                                              m_ullLastRspTime 
	volatile UINT64 lastAckRecTime;       // time stamp of the last received ACK
	bool ackReceived;
	
	UINT64 nextAckTime;			        // next time to send ack                                                                  m_ullNextACKTime
	UINT64 nextNackTime;			    // next time to send nack                                                                  m_ullNextNAKTime
	UINT64 lastAck2SentTime;	        // last time ACK2 was sent to peer                                                        m_ullSndLastAck2Time
	
	INT32 lastAck2Sent;					// last ACK2 we sent to peer                                                               m_iSndLastAck2
	INT32 lastAcknowledgedAckSent;      // last ack we sent to peer that has been acnowledged                                      m_iRcvLastAckAck
	volatile INT32  lastDataAckRecv;    // The real last ACK we received from peer that updates the sender buffer and loss list    m_iSndLastDataAck
	INT32  lastAckSeqNo;                // Last ACK sequence number                                                                m_iAckSeqNo
	
	UINT32 packetCounter;           //   m_iPktCount
	UINT32 lightACKCounter;         //  light ACK counter     m_iLightACKCount

	INT32  RTTVar;                  //   m_iRTTVar
	INT32  RTT;                     //   m_iRTT
	INT32  estimatedBandwidth;      //   m_iBandwidth
	INT32  deliveryRate;            //   m_iDeliveryRate
	INT32  expirationCounter;       // Expiration counter    m_iEXPCount


	INT64 totalPacketsRecv;        //  total number of received packets                m_llRecvTotal
	INT64 totalPacketsRecvReportingInterval;        //  total number of received packets for current reporting interval                m_llRecvTotal
	INT64 totalPacketsSent;        //  total number of sent packets                    m_llSentTotal
	INT64 totalPacketsSentReportingInterval;        //  total number of sent packets for current reporting interval
	UINT64 totalAckRecv;            //  total number of received ACK packets            m_iRecvACKTotal
	UINT64 totalAckSent;            // total number of sent ACK packets                 m_iSentACKTotal
	INT32 totalRecvPacketLoss;     //  total number of lost packets (receiver side)    m_iRcvLossTotal
	INT32 totalRecvPacketLossReportingInterval;     //  total number of lost packets (receiver side) for current reporting interval    m_iRcvLossTotal
    INT32 totalRecvPacketDropped;  //  total number of packets dropped due to full buffer (receiver side)
	INT32 totalRecvPacketDroppedReportingInterval;  //  total number of packets dropped due to full buffer (receiver side) for current reporting interval
    INT32 totalDupPacketsRecvd;    //  total number of duplicate packets received (receiver side)
	INT32 totalDupPacketsRecvdReportingInterval;    //  total number of duplicate packets received (receiver side) for current reporting interval
	UINT32 totalSendPacketLoss;     //  total number of lost packets (sender side)      m_iSndLossTotal 
	UINT32 totalNackRecv;           //  total number of received NAK packets            m_iRecvNAKTotal
	UINT32 totalNackSent;           // total number of sent NAK packets                 m_iSentNAKTotal
	INT32 totalRetransmit;        // total number of retransmitted packets             m_iRetransTotal
	INT32 totalRetransmitReportingInterval;        // total number of retransmitted packets for current reporting interval             m_iRetransTotal
	INT32 totalDup2PacketsRecvd;    //  total number of duplicate packets received (receiver side)
	INT32 totalDup2PacketsRecvdReportingInterval;    //  total number of duplicate packets received (receiver side) for current reporting interval


	volatile UINT32 currentRecvSeqNumber;    // largest received seq number from peer   m_iRcvCurrSeqNo
	INT32  lastAckSent;                      //   last ack we sent to peer -            m_iRcvLastAck
	UINT64 lastAckSentTime;                  // timestamp of last ack we sent to peer   m_ullLastAckTime
	volatile INT32  lastAckRecv;             // last ack we received from peer          m_iSndLastAck
	INT32  maxPayloadSize;                   // maximum payload size                    m_iPayloadSize
	volatile UINT32 currentSendSeqNumber;    // largest sent seq number to peer         m_iSndCurrSeqNo
	

	volatile UINT64 interPacketInterval;     // Inter-packet time, in usec              m_ullInterval
	UINT64 nextPacketTargetTime; 			 // scheduled time of next packet sending     m_ullTargetTime
	UINT64 interPacketTimeDiff;              // aggregate difference in inter-packet time m_ullTimeDiff
	
	
	volatile UINT64 SYNInt;                  // SYN interval                            m_ullSYNInt
    volatile UINT64 ACKInt; 	         	 // ACK interval                            m_ullACKInt 
	volatile UINT64 NACKInt;                  // NACK interval                          m_ullNACKInt 
	UINT64   minExpTimeout;			       // timeout lower bound threshold: too small timeout can cause problem               m_ullMinExpInt  
	UINT64   minNakInt;                    // min timeout for NACK


   volatile INT32 flowWindowSize;           // Flow control window size                m_iFlowWindowSize
   volatile double congestionWindowSize;    // congestion window size                  m_dCongestionWindow

    INT64   maxTransferRate;				// maximum data transfer rate (threshold)   m_llMaxBW

	INT32 initialSeqNumber;                  // Initial Sequence Number                                m_iISN
	int   MSS;                               // Maximum Segment Size, in bytes                         m_iMSS
	int   flightFlagSize;                   // Maximum number of packets in flight from the peer side  m_iFlightFlagSize
    int   maxSendBufferSize;                // Maximum UDT sender buffer size                          m_iSndBufSize
    int   maxRecvBufferSize;                // Maximum UDT receiver buffer size                        m_iRcvBufSize

	UINT32 socketId;                        // UDT socket number                                       m_SocketID
	UINT32 peerId;                           // peer id, for multiplexer                               m_PeerID
	PUDPSocket::Descriptor peerAddress;      // peer address                                            m_pPeerAddr

// for listener
	deque<UINT32> waitingForAccept;
// for connection 
	PUDTHandShake connResponse;
	
	PUDTConnection(PUDTSocketContainer & cont_);
	PUDTConnection(const PUDTConnection & c);
	~PUDTConnection();

	void open();
	void listen(const PUDPSocket::Descriptor & addr,PUDTPacket *p);
	int connect(const PUDPSocket::Descriptor & addr);
	void setFromListen(const PUDPSocket::Descriptor & addr, PUDTHandShake & hs);
	int processConnection(const PUDPSocket::Descriptor & addr,PUDTPacket* response); 
	int send(const char* data, int len);
	int recv(char* data, int len);


	bool processData(const PUDPSocket::Descriptor & addr,PUDTPacket *p);
	int  packData(PUDTPacket& packet, UINT64& ts);
	void processCtrl(const PUDPSocket::Descriptor & addr,PUDTPacket *p);
	void sendCtrl(int pkttype, void* lparam=0, void* rparam=0, int size=0);
	void checkTimers();
	void releaseSynch();
	void CCUpdate();
	void reset();
	void printCounters();
    void getConnectionData(PUDTConnectionData& connData);

friend class PUDTSocketContainer;
};




class PUDTConnectionLock
{
private:
	PUDTConnection * connection;
	
public:
	PUDTConnectionLock();
	void set( PUDTConnection* conn_ );
	void lock( PUDTConnection* conn_ );
	~PUDTConnectionLock();

private://prohibited
	PUDTConnectionLock( const PUDTConnectionLock& );
	PUDTConnectionLock& operator =( const PUDTConnectionLock& );


};



// One per thread manager
class PUDTSocketContainer
{
private:
	mutable PCriticalSection connectionsLock;
	PlibIdMap<PUDTConnection *> connections;

public:
	int udpMSS;
	int udpFlightFlagSize;
	int udpMaxSendBufferSize;
    int udpMaxRecvBufferSize;
	int udpRTT;
	int udpEstimatedBandwidth;
	bool closing;
	PUDPSocket* socket;
	PUDTSendThread sndThread;
	PUDTRecvThread rcvThread; 
	PUDTPacketStorage packetStorage;
	UINT32 listener;



public:
	PUDTSocketContainer();
	~PUDTSocketContainer();
	void start();
	void shutdown();
	PUDTTimerQueueMap sendTimerQueue;
	int sendto(PUDTPacket &pkt,const PUDPSocket::Descriptor & peerAddress) const;
	int recvfrom(PUDTPacket &pkt,PUDPSocket::Descriptor & peerAddress) const;
	PUDTConnection * findConnection(UINT32 socketId, PUDTConnectionLock& acc );
	PUDTConnection * findConnection(const PUDPSocket::Descriptor & peerAddress,UINT32 peerId, PUDTConnectionLock& acc);
	void getConnections(vector<UINT32>& v);
	UINT32 addConnection(PUDTConnection * c, PUDTConnectionLock& acc);
	void removeConnection(PUDTConnection * c);
	size_t getNumConnections() const;
// API
public: 
	UINT32 create();
	int listen(UINT32 socket_);
	int accept(UINT32 socket_,PUDPSocket::Descriptor& ret);
	int connect(UINT32 socket_,const char * peer, int port, PUDPSocket::Descriptor& peerAddress);
	int recvnonblocking(UINT32 socket_, char * buf, int size);
	int sendnonblocking(UINT32 socket_, char * buf, int size);
	HANDLE getsockreadevent(UINT32 socket_);
	HANDLE getsockwriteevent(UINT32 socket_);
	void close(UINT32 socket_);
	void printCounters();
    std::list<PUDTConnectionData> getConnectionData();
};

#endif
#endif
