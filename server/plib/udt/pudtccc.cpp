#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT

#include "pudtccc.h"
#include "pudttimer.h"
#include "pudtcommon.h"

namespace
{

bool initRand()
{
	srand(PUDTTimerQueue::getTimestamp());
	return true;
}

} // unnamed namespace


PUDTCCC::PUDTCCC()
{

	packetSendingPeriod = 1.;

	congestionWidowSize = 13000.0;
	estimatedBandwidth = 0;
	maxCWndSize = 0;
	MSS=0;
	currentSendSeqNumber = 0;
	recvRate = 0;
	RTT= 0;
	ACKPeriod = 0;
	packetsPerACKInernal = 0;
	userDefinedRTO = 0;
	RTO = -1;

	rateControlInterval = 0;
    lastRateIncreaseTime =0;
	lastRateDecreaseTime = 0;
    slowStart=true;	
    lastAckSeqNo=0;
    lossDetected=false;
    lastDecSeqNo = 0;
    lastDecPeriod = 0;
    NAKCounter = 0; 
    decreaseRandom = 0; 
    averNAKPerCongestion = 0;
    decreaseCounter = 0;


}



void PUDTCCC::init()
{
   rateControlInterval = PUDT_SYNInterval;
   lastRateIncreaseTime = PUDTTimerQueue::getTimestamp();
   setACKTimer(rateControlInterval);

   slowStart = true;
   lastAckSeqNo = currentSendSeqNumber;
   lossDetected = false;
   lastDecSeqNo = PUDTSequenceNumber::decrementSeqNum(lastAckSeqNo);
   lastDecPeriod = 1;
   averNAKPerCongestion = 0;
   NAKCounter = 0;
   decreaseRandom = 1;

//   m_dCWndSize = 16;
   congestionWidowSize = 13000.0;
//   m_dPktSndPeriod = 1;
   packetSendingPeriod = 6;
}

void PUDTCCC::onACK(INT32 ack)
{
   INT64 B = 0;
   double inc = 0;
   // Note: 1/24/2012
   // The minimum increase parameter is increased from "1.0 / m_iMSS" to 0.01
   // because the original was too small and caused sending rate to stay at low level
   // for long time.
   const double min_inc = 0.01;

   UINT64 currtime = PUDTTimerQueue::getTimestamp();
   if (currtime - lastRateIncreaseTime < (UINT64)rateControlInterval)
      return;

   lastRateIncreaseTime = currtime;

   if (slowStart)
   {
	   congestionWidowSize += PUDTSequenceNumber::seqLength(lastAckSeqNo, ack);
      lastAckSeqNo = ack;

      if (congestionWidowSize > maxCWndSize)
      {
         slowStart = false;
         if (recvRate > 0)
            packetSendingPeriod = 1000000.0 / recvRate;
         else
            packetSendingPeriod = (RTT + rateControlInterval) / congestionWidowSize;
      }
   }
   else
   {
        congestionWidowSize = recvRate / 1000000.0 * (RTT + rateControlInterval) + 16;
   }


   // During Slow Start, no rate increase
   if (slowStart)
      return;
/*
   if (lossDetected)
   {
      lossDetected = false;
	  PTRACE3("onAck: end congestion, packetSendingPeriod=%u, decreaseRandom = %d, averNAKPerCongestion =%d",(int)packetSendingPeriod, decreaseRandom, averNAKPerCongestion);
      return;
   }
*/
   B = (INT64)(estimatedBandwidth - 1000000.0 / packetSendingPeriod);
   if ((packetSendingPeriod > lastDecPeriod) && ((estimatedBandwidth / 9) < B))
      B = estimatedBandwidth / 9;
   if (B <= 0)
      inc = min_inc;
   else
   {
      // inc = max(10 ^ ceil(log10( B * MSS * 8 ) * Beta / MSS, 1/MSS)
      // Beta = 1.5 * 10^(-6)

	   inc = pow(10.0, ceil(log10(B * MSS * 8.0))) * 0.0000015 / MSS;

      if (inc < min_inc)
         inc = min_inc;
   }
   if (lossDetected)
   {
	   lossDetected = false;
	   double tmpPacketSendingPeriod = (packetSendingPeriod * rateControlInterval) / (packetSendingPeriod * inc + rateControlInterval);
	   PTRACE4("onAck: end congestion, would be packetSendingPeriod=%u, estimatedBandwidth=%d, B=%" PRId64 " inc=%f origPacketSendingPeriod=%u",
	    	   (unsigned int)tmpPacketSendingPeriod, estimatedBandwidth, B, inc, (unsigned int)packetSendingPeriod);
	   return;
   }
   packetSendingPeriod = (packetSendingPeriod * rateControlInterval) / (packetSendingPeriod * inc + rateControlInterval);
}

void PUDTCCC::onLoss(const INT32* losslist, int)
{
//   UINT64 currtime = PUDTTimerQueue::getTimestamp();
//   if (currtime - lastRateDecreaseTime < 2 * (UINT64)rateControlInterval)
//   {
//      return;
//   }

//   lastRateDecreaseTime = currtime;

   //Slow Start stopped, if it hasn't yet
   if (slowStart)
   {
      slowStart = false;
      if (recvRate > 0)
      {
         // Set the sending rate to the receiving rate.
         packetSendingPeriod = 1000000.0 / recvRate;
         return;
      }
      // If no receiving rate is observed, we have to compute the sending
      // rate according to the current window size, and decrease it
      // using the method below.
      packetSendingPeriod = congestionWidowSize / (RTT + rateControlInterval);
   }

   lossDetected = true;

   if (PUDTSequenceNumber::seqCmp(losslist[0] & 0x7FFFFFFF, lastDecSeqNo) > 0)
   {
      lastDecPeriod = packetSendingPeriod;
      packetSendingPeriod = ceil(packetSendingPeriod * 1.05);

      averNAKPerCongestion = (int)ceil(averNAKPerCongestion * 0.875 + NAKCounter * 0.125);
      NAKCounter = 1;
      decreaseCounter = 1;

      lastDecSeqNo = currentSendSeqNumber;

      // remove global synchronization using randomization
	  static const bool randInited = initRand();
	  (void)randInited;
      decreaseRandom = (int)ceil(averNAKPerCongestion * (double(rand()) / RAND_MAX));
      if (decreaseRandom < 1)
         decreaseRandom = 1;
	  PTRACE3("onLoss: start congestion, packetSendingPeriod=%u, decreaseRandom = %d, averNAKPerCongestion =%d",(int)packetSendingPeriod, decreaseRandom, averNAKPerCongestion);
   }
   else if ((decreaseCounter ++ < 5) && (0 == (++ NAKCounter % decreaseRandom)))
   {
      // 0.875^5 = 0.51, rate should not be decreased by more than half within a congestion period
      packetSendingPeriod = ceil(packetSendingPeriod * 1.125);
      lastDecSeqNo = currentSendSeqNumber;
	  PTRACE4("onLoss: decreaseCounter=%d, NAKCounter=%d, packetSendingPeriod=%u", decreaseCounter, NAKCounter, (int)packetSendingPeriod);
   }
}

void PUDTCCC::onTimeout()
{
   if (slowStart)
   {
      slowStart = false;
      if (recvRate > 0)
         packetSendingPeriod = 1000000.0 / recvRate;
      else
         packetSendingPeriod = congestionWidowSize / (RTT + rateControlInterval);
   }
   else
   {
      /*
      m_dLastDecPeriod = m_dPktSndPeriod;
      m_dPktSndPeriod = ceil(m_dPktSndPeriod * 2);
      m_iLastDecSeq = m_iLastAck;
      */
   }
}
#endif
