#ifndef pudtccc_h_included
#define pudtccc_h_included

#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT

#include "pudtdefine.h"



class PUDTPacket;

class PUDTCCC
{

public:
   PUDTCCC();
      
   virtual ~PUDTCCC()
   {}

private:
   PUDTCCC(const PUDTCCC&);
   PUDTCCC& operator=(const PUDTCCC&) {return *this;}

private:


public:

      // Functionality:
      //    Callback function to be called (only) at the start of a UDT connection.
      //    note that this is different from CCC(), which is always called.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   virtual void init();

      // Functionality:
      //    Callback function to be called when a UDT connection is closed.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   virtual void close() {}

      // Functionality:
      //    Callback function to be called when an ACK packet is received.
      // Parameters:
      //    0) [in] ackno: the data sequence number acknowledged by this ACK.
      // Returned value:
      //    None.

   virtual void onACK(INT32);

      // Functionality:
      //    Callback function to be called when a loss report is received.
      // Parameters:
      //    0) [in] losslist: list of sequence number of packets, in the format describled in packet.cpp.
      //    1) [in] size: length of the loss list.
      // Returned value:
      //    None.

   virtual void onLoss(const INT32*, int);

      // Functionality:
      //    Callback function to be called when a timeout event occurs.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   virtual void onTimeout();

      // Functionality:
      //    Callback function to be called when a data is sent.
      // Parameters:
      //    0) [in] seqno: the data sequence number.
      //    1) [in] size: the payload size.
      // Returned value:
      //    None.

   virtual void onPktSent(const PUDTPacket*) {}

      // Functionality:
      //    Callback function to be called when a data is received.
      // Parameters:
      //    0) [in] seqno: the data sequence number.
      //    1) [in] size: the payload size.
      // Returned value:
      //    None.

   virtual void onPktReceived(const PUDTPacket*) {}

      // Functionality:
      //    Callback function to Process a user defined packet.
      // Parameters:
      //    0) [in] pkt: the user defined packet.
      // Returned value:
      //    None.

   virtual void processCustomMsg(const PUDTPacket*) {}


	void  setRTT(int rtt)
	{
		RTT = rtt;
	}
	void setRcvRate(int rcvrate)
	{
	   recvRate = rcvrate;
	}
	void setBandwidth(int bandwidth)
	{
	   estimatedBandwidth = bandwidth;
	}
	void setSndCurrSeqNo(INT32 seqno)
	{
	   currentSendSeqNumber = seqno;
	}
	void setMSS(UINT32 mss_)
	{
		MSS=mss_;
	}
	void setMaxCWndSize(int cwnd)
	{
	   maxCWndSize = cwnd;
	}
	void setACKTimer(int msINT)
	{
	   ACKPeriod = msINT > PUDT_SYNInterval ? PUDT_SYNInterval : msINT;
	}



   int packetsPerACKInernal;                  // How many packets to send one ACK, in packets      m_iACKInterval
   int ACKPeriod;                            // Periodical timer to send an ACK, in milliseconds   m_iACKPeriod
   bool userDefinedRTO;                      // if the RTO value is defined by users               m_bUserDefinedRTO
   int RTO;                                  // RTO value, microseconds                            m_iRTO
   INT32 currentSendSeqNumber;               // largest sent seq number to peer                    m_iSndCurrSeqNo
	INT32 RTT;                               // current estimated RTT, microsecond                 m_iRTT
	INT32  estimatedBandwidth;               // estimated bandwidth, packets per second            m_iBandwidth
	INT32  recvRate;                         // packet arrive rate at receiver side, packets per second    m_iRcvRate
   double packetSendingPeriod;               // Packet sending period, in microseconds                     m_dPktSndPeriod
   double congestionWidowSize;               // Congestion window size, in packets                         m_dCWndSize
   double maxCWndSize;                       // maximum cwnd size, in packets                              m_dMaxCWndSize
	int MSS;				                 // Maximum Packet Size, including all packet headers          m_iMSS;

	int    rateControlInterval;              // UDT Rate control interval                           m_iRCInterval
     UINT64 lastRateIncreaseTime;          	// last rate increase time                              m_LastRCTime
	 UINT64 lastRateDecreaseTime;          	// last rate decrease time
     bool slowStart;			            // if in slow start phase                               m_bSlowStart
     INT32 lastAckSeqNo;  			       // last ACKed seq no                                    m_iLastAck
     bool  lossDetected;			        // if loss happened since last rate increase            m_bLoss
     INT32 lastDecSeqNo;     		        // max pkt seq no sent out when last decrease happened      m_iLastDecSeq
     double lastDecPeriod;		            // value of pktsndperiod when last decrease happened       m_dLastDecPeriod
     int NAKCounter;                     // NAK counter                                               m_iNAKCount
     int decreaseRandom;                     // random threshold on decrease by number of loss events     m_iDecRandom
     int averNAKPerCongestion;              // average number of NAKs per congestion                 m_iAvgNAKNum 
     int decreaseCounter;         			// number of decreases in a congestion epoch             m_iDecCount


};

#endif
#endif