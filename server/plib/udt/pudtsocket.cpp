
#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pudtsocket.h"
#include "pudtcommon.h"
#include "pplogfile.h"

#define UDP_HEADER_SIZE 8
#define IP_HEADER_SIZE 20


bool PUDTConnection::processData(const PUDPSocket::Descriptor & addr, PUDTPacket *packet)
{
#if PTRACELVL >= 5
	{
		PString a;
		PUDPSocket::address(a,addr);
		char buf[100];
        PTRACE5("%8.8x PUDTConnection::processData(Packet(%s)), from %s ",socketId,packet->print(buf),a.c_str());
//		const BYTE * p = (const BYTE *) packet->data;
//		PTRACE3("%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9]);
	}
#endif

	// Reset expiration counter.
	expirationCounter = 1;

	UINT64 currentTime = PUDTTimerQueue::getTimestamp();
	lastRespTime = currentTime;

   ccc->onPktReceived(packet);
   ++packetCounter;
   // update time information
   recvTimeWindow->onPktArrival(currentTime);

   // check if it is probing packet pair
   if (0 == (packet->seqNo & 0xF))
      recvTimeWindow->probe1Arrival(packet->seqNo,currentTime);
   else if (1 == (packet->seqNo & 0xF))
      recvTimeWindow->probe2Arrival(packet->seqNo,currentTime);

   ++totalPacketsRecv;
   ++totalPacketsRecvReportingInterval;

   INT32 offset = PUDTSequenceNumber::seqOffset(lastAckSent, packet->seqNo);
   if (offset < 0)
   {
	   PTRACE5("%8.8x Get duplicate packet %x",socketId,packet->seqNo);
       ++totalDupPacketsRecvd;
	   ++totalDupPacketsRecvdReportingInterval;
       return false;
   }
   if (offset >= recvBuffer->getAvailableSlots())
   {
	   PTRACE5("%8.8x offset %d > available slots, packet %x",socketId,offset, recvBuffer->getAvailableSlots(), packet->seqNo);
       ++totalRecvPacketDropped;
	   ++totalRecvPacketDroppedReportingInterval;
       return false;
   }

   if (!recvBuffer->addPacket(packet, offset) )
   {
		PTRACE5("%8.8x Slot taken %x",socketId, packet->seqNo);
		++totalDup2PacketsRecvd;
		++totalDup2PacketsRecvdReportingInterval;
		return false;
   }

   // Loss detection.
   if (PUDTSequenceNumber::seqCmp(packet->seqNo, PUDTSequenceNumber::incrementSeqNum(currentRecvSeqNumber)) > 0)
   {
      // If loss found, insert them to the receiver loss list
      recvLostList->insert(PUDTSequenceNumber::incrementSeqNum(currentRecvSeqNumber), PUDTSequenceNumber::decrementSeqNum(packet->seqNo));
	  PTRACE4("%8.8x Lost packets %x - %x",socketId,PUDTSequenceNumber::incrementSeqNum(currentRecvSeqNumber),PUDTSequenceNumber::decrementSeqNum(packet->seqNo));
      // pack loss list for NAK
      INT32 lossdata[2];
      lossdata[0] = PUDTSequenceNumber::incrementSeqNum(currentRecvSeqNumber) | 0x80000000;
	  lossdata[1] = PUDTSequenceNumber::decrementSeqNum(packet->seqNo);

      // Generate loss report immediately.
	  sendCtrl(3, NULL, lossdata, (PUDTSequenceNumber::incrementSeqNum(currentRecvSeqNumber) == PUDTSequenceNumber::decrementSeqNum(packet->seqNo)) ? 1 : 2);

	  int loss = PUDTSequenceNumber::seqLength(currentRecvSeqNumber, packet->seqNo) - 2;
      totalRecvPacketLoss += loss;
	  totalRecvPacketLossReportingInterval += loss;
   }

   // This is not a regular fixed size packet...   
   //an irregular sized packet usually indicates the end of a message, so send an ACK immediately   
   if (packet->getLength() != maxPayloadSize)   
   {
	   // send next ack right now
	   nextAckTime = currentTime;
   }

   // Update the current largest sequence number that has been received.
   // Or it is a retransmitted packet, remove it from receiver loss list.
   if (PUDTSequenceNumber::seqCmp(packet->seqNo, currentRecvSeqNumber) > 0)
	   currentRecvSeqNumber = packet->seqNo;
   else
	   recvLostList->remove(packet->seqNo);

   return true;
}


void PUDTConnection::processCtrl(const PUDPSocket::Descriptor & addr,PUDTPacket *packet)
{
#if PTRACELVL >= 5
	{
		PString a;
		PUDPSocket::address(a,addr);
		char buf[100];
        PTRACE5("%8.8x PUDTConnection::processCtrl(Packet(%s)), from %s ",socketId,packet->print(buf),a.c_str());
	}
#endif

   // Reset expiration counter.
   expirationCounter = 1;

       UINT64 currentTime = PUDTTimerQueue::getTimestamp();
	lastRespTime = currentTime;

	switch (packet->getType())
	{
		case 2: //010 - Acknowledgement
			{
			    lastAckRecTime = currentTime;
				ackReceived = true;
				INT32 ack;
				// process a lite ACK
				if (4 == packet->getLength())
				{
					ack = *(INT32 *)packet->data;
					if (PUDTSequenceNumber::seqCmp(ack, lastAckRecv) >= 0)
					{
						flowWindowSize -= PUDTSequenceNumber::seqOffset(lastAckRecv, ack); 
						lastAckRecv = ack;
					}
					PTRACE5("%8.8x PUDTConnection::processCtrl(2), light ack=%8.8x",socketId,ack);
					break;
				}

				// read ACK seq. no.
				INT32 ackSeqNo = packet->getAckSeqNo();

				// send ACK acknowledgement
				// number of ACK2 can be much less than number of ACK
				if ((currentTime - lastAck2SentTime > (UINT64)PUDT_SYNInterval) || (ackSeqNo == lastAck2Sent))
				{
					sendCtrl(6, &ackSeqNo);
					lastAck2Sent = ackSeqNo;
					lastAck2SentTime = currentTime;
				}

				// Got data ACK
				ack = *(INT32 *)packet->data;
			    PTRACE5("%8.8x PUDTConnection::processCtrl(2), ack=%8.8x, lastAck=%8.8x, lastDataAckRecv=%8.8x",socketId,ack,lastAckRecv,lastDataAckRecv);
				// check the validation of the ack
				if (PUDTSequenceNumber::seqCmp(ack, PUDTSequenceNumber::incrementSeqNum(currentSendSeqNumber)) > 0)
				{
					//this should not happen: attack or bug
					throw PError("UDT: Invalid ack");
				}

				if (PUDTSequenceNumber::seqCmp(ack, lastAckRecv) >= 0)
				{
					// Update Flow Window Size, must update before and together with lastAckRecv
					flowWindowSize = *((INT32 *)packet->data + 3);
					lastAckRecv = ack;
				}

				// protect packet retransmission
				{
					PLock lock(ackLock);
					int offset = PUDTSequenceNumber::seqOffset(lastDataAckRecv, ack);
					if (offset <= 0)
					{
						PTRACE5("%8.8x PUDTConnection::processCtrl(2) offset<0ack=%8.8x, lastDataAckRecv=%8.8x",socketId,ack,lastDataAckRecv);
						break;
					}

					// acknowledge the sending buffer
					sendBuffer->ackPackets(offset);

/*
					// record total time used for sending
					m_llSndDuration += currtime - m_llSndDurationCounter;
					m_llSndDurationTotal += currtime - m_llSndDurationCounter;
					m_llSndDurationCounter = currtime;
*/

					// update sending variables
					lastDataAckRecv = ack;
					if(sendLostList->removeUpTo(PUDTSequenceNumber::decrementSeqNum(lastDataAckRecv)))
						PTRACE5("%8.8x PUDTConnection::processCtrl(2), lost list cleaned up to =%8.8x",lastDataAckRecv);
				}
				PTRACE5("%8.8x PUDTConnection::processCtrl(2), ack=%8.8x",socketId,ack);
				_sendEvent.setEvent();
				// insert this socket to snd list if it is not on the list yet
				cont.sendTimerQueue.addTimer(socketId,1,false);

				// Update RTT
				//m_iRTT = *((int32_t *)ctrlpkt.m_pcData + 1);
				//m_iRTTVar = *((int32_t *)ctrlpkt.m_pcData + 2);
				int rtt = *((INT32 *)packet->data + 1);
				RTTVar = (RTTVar * 3 + abs(rtt - RTT)) >> 2;
				RTT = (RTT * 7 + rtt) >> 3;
			//printf("Ctrl m_iRTT %d\n", m_iRTT);
				ccc->setRTT(RTT);

				if (packet->getLength() > 16)
				{
					// Update Estimated Bandwidth and packet delivery rate
					if (*((INT32 *)packet->data + 4) > 0)
						deliveryRate = (deliveryRate * 7 + *((INT32 *)packet->data + 4)) >> 3;

					if (*((INT32 *)packet->data + 5) > 0)
						estimatedBandwidth = (estimatedBandwidth * 7 + *((INT32 *)packet->data + 5)) >> 3;

					ccc->setRcvRate(deliveryRate);
					ccc->setBandwidth(estimatedBandwidth);
				}

				ccc->onACK(ack);
				CCUpdate();

				++totalAckRecv;

				break;
			}

		case 6: //110 - Acknowledgement of Acknowledgement
			{
				INT32 ack;
				int rtt = -1;

				// update RTT
				rtt = ackWindow->acknowledge(packet->getAckSeqNo(), ack, currentTime);
				if (rtt <= 0)
					break;

				//if increasing delay detected...
				//   sendCtrl(4);

				// RTT EWMA
				RTTVar = (RTTVar * 3 + abs(rtt - RTT)) >> 2;
				RTT = (RTT * 7 + rtt) >> 3;
			//	  printf("Ack m_iRTT %d\n", m_iRTT);

				ccc->setRTT(RTT);

				// update last ACK that has been received by the sender
				if (PUDTSequenceNumber::seqCmp(ack, lastAcknowledgedAckSent) > 0)
					lastAcknowledgedAckSent = ack;

				break;
			}

		case 3: //011 - Loss Report
			{
				INT32* losslist = (INT32 *)(packet->data);

				ccc->onLoss(losslist, packet->getLength() / 4);
				CCUpdate();
				PTRACE4("%8.8x Lost list %x - %x, size=%d", socketId, losslist[0] & ~0x80000000, ((packet->getLength() == 4) ? losslist[0] & ~0x80000000 : losslist[1]), packet->getLength() / 4);
				bool secure = true;

				// decode loss list message and insert loss into the sender loss list
				for (int i = 0, n = (int)(packet->getLength() / 4); i < n; ++ i)
				{
					if (0 != (losslist[i] & 0x80000000))
					{
						if ((PUDTSequenceNumber::seqCmp(losslist[i] & 0x7FFFFFFF, losslist[i + 1]) > 0) || (PUDTSequenceNumber::seqCmp(losslist[i + 1], currentSendSeqNumber) > 0))
						{
							// seq_a must not be greater than seq_b; seq_b must not be greater than the most recent sent seq
							secure = false;
							break;
						}

						int num = 0;
						if (PUDTSequenceNumber::seqCmp(losslist[i] & 0x7FFFFFFF, lastAckRecv) >= 0)
							num = sendLostList->insert(losslist[i] & 0x7FFFFFFF, losslist[i + 1]);
						else if (PUDTSequenceNumber::seqCmp(losslist[i + 1], lastAckRecv) >= 0)
							num = sendLostList->insert(lastAckRecv, losslist[i + 1]);

						totalSendPacketLoss += num;

						++i;
					}
					else if (PUDTSequenceNumber::seqCmp(losslist[i], lastAckRecv) >= 0)
					{
						if (PUDTSequenceNumber::seqCmp(losslist[i], currentSendSeqNumber) > 0)
						{
							//seq_a must not be greater than the most recent sent seq
							secure = false;
							break;
						}

						int num = sendLostList->insert(losslist[i], losslist[i]);
					
						totalSendPacketLoss += num;
					}
				}

				if (!secure)
				{
					//this should not happen: attack or bug
					throw PError("UDT: Invalid loss report");
					break;
				}

				// the lost packet (retransmission) should be sent out immediately
				cont.sendTimerQueue.addTimer(socketId,1,true);

				++totalNackRecv;

				break;
			}

		case 4: //100 - Delay Warning
				// One way packet delay is increasing, so decrease the sending rate
				interPacketInterval = (UINT64) ceil(interPacketInterval * 1.125);
				break;

			case 1: //001 - Keep-alive
				// The only purpose of keep-alive packet is to tell that the peer is still alive
				// nothing needs to be done.

				break;

			case 0: //000 - Handshake
				{
				PUDTHandShake req;
				req.deserialize(packet->data, packet->getLength());
				if (req.reqType > 0)
				{
					// The peer side has not received the handshake message, so it keeps querying
					// resend the handshake packet

					PUDTHandShake initdata;
					initdata.ISN = initialSeqNumber;
					initdata.MSS = MSS;
					initdata.flightFlagSize = flightFlagSize;
					initdata.reqType = -1;
					initdata.id = socketId;

					char* hs = new char [maxPayloadSize];
					int hs_size = maxPayloadSize;
					initdata.serialize(hs, hs_size);
					sendCtrl(0, NULL, hs, hs_size);
					delete [] hs;
				}

				break;
				}

			case 5: //101 - Shutdown
				status = CLOSED;
				cont.removeConnection(this);
				releaseSynch();
				break;

			case 7: //111 - Msg drop request
				// do not support
				throw PError("Message drop request is not supported");

			case 8: // 1000 - An error has happened to the peer side
				//int err_type = packet.getAddInfo();

				// currently only this error is signalled from the peer side
				// if recvfile() failes (e.g., due to disk fail), blcoked sendfile/send should return immediately
				// giving the app a chance to fix the issue
				status = BROKEN;
				cont.removeConnection(this);
				releaseSynch();
				break;

			case 32767: //0x7FFF - reserved and user defined messages
				ccc->processCustomMsg(packet);
				CCUpdate();

				break;

			default:
				break;
	}
}



void PUDTConnection::sendCtrl(int pkttype, void* lparam, void* rparam, int size)
{
	PUDTPacket ctrlpkt;
    PTRACE5("%8.8x PUDTConnection::sendCtrl(pkttype %d), size=%d",socketId,pkttype,size);

	switch (pkttype)
	{
		case 2: //010 - Acknowledgement
			{
				INT32 ack;

				// If there is no loss, the ACK is the current largest sequence number plus 1;
				// Otherwise it is the smallest sequence number in the receiver loss list.
				ack = recvLostList->getFirstLostSeq();
				if (-1 == ack) 
					ack = PUDTSequenceNumber::incrementSeqNum(currentRecvSeqNumber);

				if (ack == lastAcknowledgedAckSent)
					break;

				PTRACE5("%8.8x PUDTConnection::sendCtrl(2), ack=%8.8x, lastackack=%8.8x )",socketId,ack,lastAcknowledgedAckSent);

				// send out a lite ACK
				// to save time on buffer processing and bandwidth/AS measurement, a lite ACK only feeds back an ACK number
				if (4 == size)
				{
					ctrlpkt.pack(pkttype, NULL, &ack, size);
					ctrlpkt.id = peerId;
					cont.sendto(ctrlpkt, peerAddress);

					break;
				}
				UINT64 currentTime = PUDTTimerQueue::getTimestamp();

				// There are new received packets to acknowledge, update related information.
				if (PUDTSequenceNumber::seqCmp(ack, lastAckSent) > 0)
				{
					int acksize = PUDTSequenceNumber::seqOffset(lastAckSent, ack);

					lastAckSent = ack;
					recvBuffer->ackPackets(acksize);
					PTRACE5("%8.8x PUDTConnection::sendCtrl(last ack=%8.8x), %d blocks of data is ready to read",socketId,ack,acksize);
					_recvEvent.setEvent();


				}
				else if (ack == lastAckSent)
				{
					if ((currentTime - lastAckSentTime) < (UINT64)(RTT + 4 * RTTVar))
					{
						break;
					}
				}
				else
				{
					break;
				}

				// Send out the ACK only if has not been received by the sender before
				if (PUDTSequenceNumber::seqCmp(lastAckSent, lastAcknowledgedAckSent) > 0)
				{
					INT32 data[6];

					lastAckSeqNo = PUDTAckNo::incack(lastAckSeqNo);
					data[0] = lastAckSent;
					data[1] = RTT;
					data[2] = RTTVar;
					data[3] = recvBuffer->getAvailableSlots();
					// a minimum flow window of 2 is used, even if buffer is full, to break potential deadlock
					if (data[3] < 2)
						data[3] = 2;

					if (currentTime - lastAckSentTime > SYNInt)
					{
						data[4] = recvTimeWindow->getPktRcvSpeed();
						data[5] = recvTimeWindow->getBandwidth();
						ctrlpkt.pack(pkttype, &lastAckSeqNo, data, 24);
						lastAckSentTime = currentTime;
					}
					else
					{
						ctrlpkt.pack(pkttype, &lastAckSeqNo, data, 16);
					}

					ctrlpkt.id = peerId;
					cont.sendto(ctrlpkt, peerAddress);
					
					ackWindow->store(lastAckSeqNo, lastAckSent, currentTime);

					++totalAckSent;
				}
				break;
			}

		case 6: //110 - Acknowledgement of Acknowledgement
			ctrlpkt.pack(pkttype, lparam);
			PTRACE5("%8.8x PUDTConnection::sendCtrl(Ack on Ack=%8.8x)",socketId,*(int *)lparam);
			ctrlpkt.id = peerId;
			cont.sendto(ctrlpkt, peerAddress);
			break;

		case 3: //011 - Loss Report
			{
			if (NULL != rparam)
			{
				if (1 == size)
				{
				// only 1 loss packet
					ctrlpkt.pack(pkttype, NULL, (INT32 *)rparam + 1, 4);
				}
				else
				{
				// more than 1 loss packets
					ctrlpkt.pack(pkttype, NULL, rparam, 8);
				}

				ctrlpkt.id = peerId;
				cont.sendto(ctrlpkt, peerAddress);

				++totalNackSent;
			}
			else if (recvLostList->getLossCount() > 0)
			{
				// this is periodically NAK report; make sure NAK cannot be sent back too often

				// read loss list from the local receiver loss list
				INT32* data = new INT32[maxPayloadSize / 4];
				int losslen = recvLostList->getLossArray(data,  maxPayloadSize / 4);

				if (0 < losslen)
				{
					ctrlpkt.pack(pkttype, NULL, data, losslen * 4);
					ctrlpkt.id = peerId;
					cont.sendto(ctrlpkt, peerAddress);
					++totalNackSent;
				}

				delete [] data;
			}
			NACKInt = (RTT + 4 * RTTVar) + PUDT_SYNInterval;
			int rcv_speed = recvTimeWindow->getPktRcvSpeed();
			if (rcv_speed > 0)
				NACKInt += (recvLostList->getLossCount() * 1000000L) / rcv_speed;
			if (NACKInt < minNakInt)
				NACKInt = minNakInt;
			break;
			}

		case 4: //100 - Congestion Warning
			ctrlpkt.pack(pkttype);
			ctrlpkt.id = peerId;
			cont.sendto(ctrlpkt, peerAddress);
			break;

		case 1: //001 - Keep-alive
			ctrlpkt.pack(pkttype);
			ctrlpkt.id = peerId;
			cont.sendto(ctrlpkt, peerAddress);
 
			break;

		case 0: //000 - Handshake
			ctrlpkt.pack(pkttype, NULL, rparam, sizeof(PUDTHandShake));
			ctrlpkt.id = peerId;
			cont.sendto(ctrlpkt, peerAddress);

			break;

		case 5: //101 - Shutdown
			ctrlpkt.pack(pkttype);
			ctrlpkt.id = peerId;
			cont.sendto(ctrlpkt, peerAddress);

			break;

		case 7: //111 - Msg drop request
			throw PError("Message drop request is not supported");
			break;

		case 8: //1000 - acknowledge the peer side a special error
			ctrlpkt.pack(pkttype, lparam);
			ctrlpkt.id = peerId;
			cont.sendto(ctrlpkt, peerAddress);

			break;

		case 32767: //0x7FFF - Resevered for future use
			break;

		default:
			break;
		}
}


void PUDTConnection::releaseSynch()
{
     PTRACE5("%8.8x PUDTConnection::releaseSynch", socketId);
	_sendEvent.setEvent();
	_recvEvent.setEvent();

//      SetEvent(m_SendBlockCond);
//      WaitForSingleObject(m_SendLock, INFINITE);
//      ReleaseMutex(m_SendLock);
//      SetEvent(m_RecvDataCond);
//      WaitForSingleObject(m_RecvLock, INFINITE);
//      ReleaseMutex(m_RecvLock);
}


void PUDTConnection::checkTimers()
{
   // update CC parameters
   CCUpdate();
   //uint64_t minint = (uint64_t)(m_ullCPUFrequency * m_pSndTimeWindow->getMinPktSndInt() * 0.9);
   //if (m_ullInterval < minint)
   //   m_ullInterval = minint;

   UINT64 currentTime = PUDTTimerQueue::getTimestamp();

   if ((currentTime > nextAckTime) || ((ccc->packetsPerACKInernal > 0) && (ccc->packetsPerACKInernal <= packetCounter)))
   {
      // ACK timer expired or ACK interval is reached

//	   PTRACE3("%8.8x PUDTConnection::checkTimers - sending ack",socketId);
      sendCtrl(2);
      if (ccc->ACKPeriod > 0)
         nextAckTime = currentTime + ccc->ACKPeriod;
      else
         nextAckTime = currentTime + ACKInt;

      packetCounter = 0;
      lightACKCounter = 1;
   }
   else if (PUDT_SelfClockInterval * lightACKCounter <= packetCounter)
   {
      //send a "light" ACK
  	  PTRACE5("%8.8x PUDTConnection::checkTimers - sending light ack", socketId);
      sendCtrl(2, NULL, NULL, 4);
      ++ lightACKCounter;
   }

   // we are not sending back repeated NAK anymore and rely on the sender's EXP for retransmission
   if (recvLostList->getLossCount() > 0)
   {
	   if (currentTime > nextNackTime)
	   {
		   // NAK timer expired, and there is loss to be reported.
		   sendCtrl(3);
		   nextNackTime = currentTime + NACKInt;
		   PTRACE3("%8.8x PUDTConnection::checkTimers - sending NACK list: NACKInt=%" PRIu64 ",nextNackTime=%" PRIu64 ",currentTime=%" PRIu64, socketId,NACKInt,nextNackTime,currentTime);
	   }
   }
   else
	   nextNackTime = currentTime + NACKInt;


   if (ackReceived && (currentTime - lastAckRecTime) > (UINT64)((RTT + 4 * RTTVar) + PUDT_SYNInterval)*2)
	   
   {
	   if (sendBuffer->getPacketCount() > 0)
	   {
		   if ((PUDTSequenceNumber::incrementSeqNum(currentSendSeqNumber) != lastAckRecv) && (sendLostList->getLossCount() == 0))
		   {
			   // resend all unacknowledged packets on timeout, but only if there is no packet in the loss list
			   INT32 csn = currentSendSeqNumber;
			   int num = sendLostList->insert(lastAckRecv, csn);
			   totalSendPacketLoss += num;
			   PTRACE3("%8.8x checkTimers timeout, add to lost list %x - %x, ret=%d", socketId, lastAckRecv, csn, num);
		   }

		   ccc->onTimeout();
		   CCUpdate();

		   // immediately restart transmission
		   cont.sendTimerQueue.addTimer(socketId, 1, true);
	   }
	   lastAckRecTime = currentTime;
   }


   UINT64 next_exp_time;
   if (ccc->userDefinedRTO)
      next_exp_time = lastRespTime + ccc->RTO;
   else
   {
      UINT64 exp_int = expirationCounter * (RTT + 4 * RTTVar) + PUDT_SYNInterval; 
      if (exp_int < expirationCounter * minExpTimeout)
         exp_int = expirationCounter * minExpTimeout;
      next_exp_time = lastRespTime + exp_int;
   }

   if (currentTime > next_exp_time)
   {
      // Haven't receive any information from the peer, is it dead?!
      // timeout: at least 16 expirations and must be greater than 10 seconds
      if ((expirationCounter > 16) && (currentTime - lastRespTime > 10000000))
      {
         //
         // Connection is broken. 
         // UDT does not signal any information about this instead of to stop quietly.
         // Application will detect this when it calls any UDT methods next time.
         //
		 PTRACE5("%8.8x PUDTConnection::checkTimers - expired",socketId);
		 reset();
         return;
      }

      // sender: Insert all the packets sent after last received acknowledgement into the sender loss list.
      // recver: Send out a keep-alive packet
	 PTRACE5("%8.8x PUDTConnection::checkTimers - sending keep alive",socketId);
     sendCtrl(1);

	  ++expirationCounter;
      // Reset last response time since we just sent a heart-beat.
      lastRespTime = currentTime;
   }
}

int PUDTConnection::packData(PUDTPacket& packet, UINT64& ts)
{

   int payload = 0;
   bool probe = false;

   UINT64 entertime = PUDTTimerQueue::getTimestamp();

   if ((0 != nextPacketTargetTime) && (entertime > nextPacketTargetTime))
      interPacketTimeDiff += entertime - nextPacketTargetTime;

   // Loss retransmission always has higher priority.
   if ((packet.seqNo = sendLostList->popFirstLostSeq()) >= 0)
   {
      // protect m_iSndLastDataAck from updating by ACK processing
      PLock lock(ackLock);

      int offset = PUDTSequenceNumber::seqOffset(lastDataAckRecv, packet.seqNo);
      if (offset < 0)
	  {
		  PTRACE5("%8.8x PUDTConnection::packData lossSeq=%8.8x, offset=%d", socketId,packet.seqNo,offset);
         return 0;
	  }
	  payload = sendBuffer->getPacket(offset,&(packet.data), packet.msgNo);

      if (0 == payload)
	  {
		  PTRACE5("%8.8x PUDTConnection::packData lossSeq=%8.8x, offset=%d payload=0", socketId,packet.seqNo,offset);
         return 0;
	  }
	  PTRACE5("%8.8x retransmitting lossSeq=%8.8x", socketId,packet.seqNo);
	  ++totalRetransmit;
	  ++totalRetransmitReportingInterval;
	  lastAckRecTime = entertime;
   }
   else
   {
      // If no loss, pack a new packet.

      // check congestion/flow window limit
      int cwnd = (flowWindowSize < (int)congestionWindowSize) ? flowWindowSize : (int)congestionWindowSize;
      if (cwnd >= PUDTSequenceNumber::seqLength(lastAckRecv, PUDTSequenceNumber::incrementSeqNum(currentSendSeqNumber)))
      {
         if (0 != (payload = sendBuffer->getPacket(&(packet.data), packet.msgNo)))
         {
            currentSendSeqNumber = PUDTSequenceNumber::incrementSeqNum(currentSendSeqNumber);
            ccc->setSndCurrSeqNo(currentSendSeqNumber);

            packet.seqNo = currentSendSeqNumber;

            // every 16 (0xF) packets, a packet pair is sent
            if (0 == (packet.seqNo & 0xF))
               probe = true;
         }
         else
         {
            nextPacketTargetTime = 0;
            interPacketTimeDiff = 0;
            ts = 0;
			PTRACE5("%8.8x PUDTConnection::packData seg=%8.8x payload=0", socketId,packet.seqNo);
            return 0;
         }
      }
      else
      {
         nextPacketTargetTime = 0;
         interPacketTimeDiff = 0;
         ts = 0;
		 PTRACE5("%8.8x PUDTConnection::packData cwn=%d  < currentSendSeqNumber=%d - lastAckRecv=%d", socketId,cwnd,currentSendSeqNumber,lastAckRecv);
         return 0;
      }
   }

   packet.timeStamp = (int)(entertime&0xffffffff);
   packet.id = peerId;
   packet.setLength(payload);


   ccc->onPktSent(&packet);
   //m_pSndTimeWindow->onPktSent(packet.m_iTimeStamp);

   ++totalPacketsSent;
   ++totalPacketsSentReportingInterval;

   if (probe)
   {
      // sends out probing packet pair
      ts = entertime;
   }
   else
   {
         if (interPacketTimeDiff >= interPacketInterval)
         {
            ts = entertime;
            interPacketTimeDiff -= interPacketInterval;
         }
         else
         {
            ts = entertime + interPacketInterval - interPacketTimeDiff;
            interPacketTimeDiff = 0;
         }

   }

   nextPacketTargetTime = ts;
	{
		char buf[100];
        UNUSED(buf);
        PTRACE5("%8.8x PUDTConnection::packData(Packet(%s), next in %d usec)",socketId,packet.print(buf), (ts - entertime));
	}

   return payload;
}


void PUDTConnection::listen(const PUDPSocket::Descriptor & addr,PUDTPacket *p)
{
	PString address;
	PUDPSocket::address(address,addr);
	{
		char buf[100];
		PTRACE3("%8.8x PUDTConnection::listen(Packet(%s)), accept from %s ",socketId,p->print(buf),address.c_str());
	}

	if (status != LISTENING)
		return;
    
   if (p->getLength() != PUDTHandShake::contentSize)
        return;

   PUDTHandShake hs;
   hs.deserialize(p->data, p->getLength());

   // SYN cookie
	UINT32 cookie = 0x12345678;
	const UINT32 * ptr = (const UINT32 *) address.c_str();
	int mod = address.length() % 4;
	if (mod)
		address.append("123",mod);
	for (int i=0; i<address.length()/4; i++)
	{
		cookie ^= *ptr++;
	}
	cookie ^= PUDPSocket::port(addr);

	UINT32 timeStamp =(UINT32) (( PUDTTimerQueue::getTimestamp() - startTime)/60000000);
   if (1 == hs.reqType)
   {
      hs.cookie = cookie ^ timeStamp;
      p->id = hs.id;
      int size = p->getLength();
      hs.serialize(p->data, size);
	  cont.sendto(*p,addr);
	  return;
   }
   else
   {
      if (hs.cookie != (cookie ^ timeStamp) )
      {
		  timeStamp--;
		  if (hs.cookie != (cookie ^ timeStamp))
			return;
      }
   }

   INT32 id = hs.id;

   // When a peer side connects in...
   if ((1 == p->getFlag()) && (0 == p->getType()))
   {
	   if ((hs.version != PUDT_Version) || (hs.type != PUDT_SocketType))
      {
         // mismatch, reject the request
		  hs.reqType = 1002;
		  int size = PUDTHandShake::contentSize;
          hs.serialize(p->data, size);
          p->id = id;
          cont.sendto(*p,addr);
      }
      else
      {
		  PUDTConnectionLock connLock;
		  PUDTConnection * c = cont.findConnection(addr,hs.id,connLock);
		  if (c && c->status == PUDTConnection::CONNECTED)
		  {
				hs.flightFlagSize = (c->maxRecvBufferSize < c->flightFlagSize)? c->maxRecvBufferSize : c->flightFlagSize;
				hs.MSS = c->MSS;
				hs.ISN = c->initialSeqNumber;
				hs.id = c->socketId;
				hs.reqType = -1;
		  }
		  else
		  {
			  PUDTConnectionLock connLockForNew;
			  PUDTConnection * cNew = new PUDTConnection(*this);
			  cNew->peerAddress = addr;
			  cNew->open();
			  cNew->socketId = cont.addConnection(cNew,connLockForNew);
			  cNew->setFromListen(addr,hs);
			  waitingForAccept.push_back(cNew->socketId);
			  PTRACE3("%8.8x PUDTConnection::listen - connection from %s accepted, id=%8.8x ",socketId, address.c_str(),cNew->socketId);
			  _recvEvent.setEvent();
		  }
		  int size = PUDTHandShake::contentSize;
		  hs.serialize(p->data, size);
		  p->id = id;
		  cont.sendto(*p,addr);
	   }
   }
   return; 
}

void PUDTConnection::setFromListen(const PUDPSocket::Descriptor & addr, PUDTHandShake& hs)
{

	{
		PString a;
		PUDPSocket::address(a,addr);
        PTRACE3("%8.8x PUDTConnection::setFromListen, from %s ",socketId,a.c_str());
	}

   // Uses the smaller MSS between the peers        
   if (hs.MSS > MSS)
      hs.MSS = MSS;
   else
      MSS = hs.MSS;

   // exchange info for maximum flow window size
   flowWindowSize = hs.flightFlagSize;
   hs.flightFlagSize = (maxRecvBufferSize < flightFlagSize)? maxRecvBufferSize : flightFlagSize;

   lastAckSent = hs.ISN;
   lastAcknowledgedAckSent = hs.ISN;
   currentRecvSeqNumber = hs.ISN - 1;

   peerId = hs.id;
   hs.id = socketId;

   // use peer's ISN and send it back for security check
   initialSeqNumber = hs.ISN;

   lastAckRecv = initialSeqNumber;
   lastDataAckRecv = initialSeqNumber;
   currentSendSeqNumber = initialSeqNumber - 1;
   lastAck2Sent = initialSeqNumber;
   lastAck2SentTime = PUDTTimerQueue::getTimestamp();

   // this is a reponse handshake
   hs.reqType = -1;

   // get local IP address and send the peer its IP address (because UDP cannot get local IP address)
// !!!!!!!!
//    memcpy(m_piSelfIP, hs->m_piPeerIP, 16);
//   /CIPAddress::ntop(peer, hs->m_piPeerIP, m_iIPversion);
  
   maxPayloadSize = MSS - UDP_HEADER_SIZE - IP_HEADER_SIZE - PUDTPacket::hdrSize;

   // Prepare all structures
	sendBuffer = new PUDTSendBuffer(maxSendBufferSize, maxPayloadSize);
	recvBuffer = new PUDTReceiveBuffer(&cont.packetStorage, maxRecvBufferSize);
	sendLostList = new PUDTSendLossList(flowWindowSize * 2);
	recvLostList = new PUDTReceiveLossList(flightFlagSize);
	ackWindow = new PUDTACKWindow(1024);
	recvTimeWindow = new PUDTTimeWindow(16, 64);
	sendTimeWindow = new PUDTTimeWindow();


/* !!!!!!!!	
	
	CInfoBlock ib;
   ib.m_iIPversion = m_iIPversion;
   CInfoBlock::convert(peer, m_iIPversion, ib.m_piIP);
   if (m_pCache->lookup(&ib) >= 0)
   {
      m_iRTT = ib.m_iRTT;
      m_iBandwidth = ib.m_iBandwidth;
   }
*/


   ccc = new PUDTCCC();
   ccc->setMSS(MSS);
   ccc->setMaxCWndSize(flowWindowSize);
   ccc->setSndCurrSeqNo(currentSendSeqNumber);
   ccc->setRcvRate(deliveryRate);
   ccc->setRTT(RTT);
   ccc->setBandwidth(estimatedBandwidth);
   ccc->init();

   interPacketInterval = (UINT64)ccc->packetSendingPeriod;
   congestionWindowSize = ccc->congestionWidowSize;
   peerAddress = addr;
   status=CONNECTED;

   //send the response to the peer, see listen() for more discussions about this
   PUDTPacket response;
   int size = PUDTHandShake::contentSize;
   char* buffer = new char[size];
   hs.serialize(buffer, size);
   response.pack(0, NULL, buffer, size);
   response.id = peerId;
   cont.sendto(response,addr);
   delete [] buffer;
}


int PUDTConnection::connect(const PUDPSocket::Descriptor & addr)
{

	PString a;
	{
		PUDPSocket::address(a,addr);
        PTRACE3("%8.8x PUDTConnection::connect to %s ",socketId,a.c_str());
	}

	if (status != OPENED)
		return -1; 

	peerAddress = addr;
   // This is my current configurations
	memset(&connResponse,0,sizeof(connResponse));
	PUDTHandShake connRequest;
	connRequest.version = PUDT_Version;
	connRequest.type = PUDT_SocketType;
	connRequest.MSS = MSS;
	connRequest.flightFlagSize = (maxRecvBufferSize < flightFlagSize)? maxRecvBufferSize : flightFlagSize;
	connRequest.reqType = 1;
	connRequest.id = socketId;
	connRequest.peerIP[0]= ((sockaddr_in*)&addr)->sin_addr.s_addr;
   // Random Initial Sequence Number
    srand((unsigned int)PUDTTimerQueue::getTimestamp());
	initialSeqNumber = connRequest.ISN = (INT32)(PUDTSequenceNumber::MaxSequenceNumber * (double(rand()) / RAND_MAX));
	lastAckRecv = initialSeqNumber;
	lastDataAckRecv = initialSeqNumber;
	currentSendSeqNumber = initialSeqNumber - 1;
	lastAck2Sent = initialSeqNumber;
	lastAck2SentTime = PUDTTimerQueue::getTimestamp();


  // Inform the server my configurations.
	status=CONNECTING;

    PUDTPacket request;
	int hs_size = PUDTHandShake::contentSize;
    char* reqdata = new char [hs_size];
    request.pack(0, NULL, reqdata, hs_size);
   // ID = 0, connection request
    request.id = 0;
    connRequest.serialize(reqdata, hs_size);
    cont.sendto(request,addr);

	UINT32 lastReqTime = SYSTEMTICKS();
	
   while (status==CONNECTING)
   {
	   DWORD res = ::WaitForSingleObject(_sendEvent.getHandle(),250);
	   if (status == CONNECTING && res == WAIT_TIMEOUT)
	   {
		   if ((SYSTEMTICKS() - lastReqTime) > 6000 )
		   {
			   // timeout
			   status = OPENED;
			   break;

		   }
			connRequest.serialize(reqdata, hs_size);
			request.setLength(hs_size);

   			cont.sendto(request,addr);
	   }
	   if (status == CONNECTED)
	   {
		   break;
	   }
	   else if (status != CONNECTING)
	   {
		   PTRACE3("%8.8x connect loop: res = %d, status=%d",socketId, res, status);
		   break;
	   }
   }
   delete [] reqdata;
   if(status == CONNECTED)
   {
        PTRACE3("%8.8x PUDTConnection::connect to %s established",socketId,a.c_str());
	    return  1;
   }
   return -1;
}

int PUDTConnection::processConnection(const PUDPSocket::Descriptor & addr, PUDTPacket * response) 
{

	PString a;
	{
		PUDPSocket::address(a,addr);
		char buf[100];
        PTRACE3("%8.8x PUDTConnection::processConnection(Packet(%s)), to %s ",socketId,response->print(buf),a.c_str());
	}

   // this is the 2nd half of a connection request. If the connection is setup successfully this returns 0.
   // returning -1 means there is an error.
   // returning 1 or 2 means the connection is in process and needs more handshake
	if(status != CONNECTING)
		return -1;

   if ((1 != response->getFlag()) || (0 != response->getType()))
      return -1;

   connResponse.deserialize(response->data, response->getLength());

      // set cookie
   if (1 == connResponse.reqType)
    {
		// response, we get our own request back with coockies.
		PUDTPacket request;
		int hs_size = PUDTHandShake::contentSize;
		char* reqdata = new char [hs_size];
		request.pack(0, NULL, reqdata, hs_size);
		request.id = 0;
		PUDTHandShake connRequest;
		connRequest = connResponse;
		connRequest.reqType = -1;
		connRequest.serialize(reqdata, hs_size);
		cont.sendto(request,peerAddress);
		delete [] reqdata;
        return 1;
    }

   // Re-configure according to the negotiated values.

   MSS=connResponse.MSS;
   flowWindowSize = connResponse.flightFlagSize;
	maxPayloadSize = MSS - UDP_HEADER_SIZE - IP_HEADER_SIZE - PUDTPacket::hdrSize;
	lastAckSent = connResponse.ISN;
   lastAcknowledgedAckSent = connResponse.ISN;
   currentRecvSeqNumber = connResponse.ISN - 1;
   peerId = connResponse.id;


   initialSeqNumber = connResponse.ISN;
   lastAckRecv = initialSeqNumber;
   lastDataAckRecv = initialSeqNumber;
   currentSendSeqNumber = initialSeqNumber - 1;
   lastAck2Sent = initialSeqNumber;
   lastAck2SentTime = PUDTTimerQueue::getTimestamp();
   
	sendBuffer = new PUDTSendBuffer(maxSendBufferSize, maxPayloadSize);
	recvBuffer = new PUDTReceiveBuffer(&cont.packetStorage, maxRecvBufferSize);
	sendLostList = new PUDTSendLossList(flowWindowSize * 2);
	recvLostList = new PUDTReceiveLossList(flightFlagSize);
	ackWindow = new PUDTACKWindow(1024);
	recvTimeWindow = new PUDTTimeWindow(16, 64);
	sendTimeWindow = new PUDTTimeWindow();

/*	
	CInfoBlock ib;
   ib.m_iIPversion = m_iIPversion;
   CInfoBlock::convert(m_pPeerAddr, m_iIPversion, ib.m_piIP);
   if (m_pCache->lookup(&ib) >= 0)
   {
      m_iRTT = ib.m_iRTT;
      m_iBandwidth = ib.m_iBandwidth;
   }
*/

	ccc = new PUDTCCC();
   ccc->setMSS(MSS);
   ccc->setMaxCWndSize(flowWindowSize);
   ccc->setSndCurrSeqNo(currentSendSeqNumber);
   ccc->setRcvRate(deliveryRate);
   ccc->setRTT(RTT);
   ccc->setBandwidth(estimatedBandwidth);
   ccc->init();


   interPacketInterval = (UINT64)ccc->packetSendingPeriod;
   congestionWindowSize = ccc->congestionWidowSize;
   status=CONNECTED;
	PTRACE3("%8.8x PUDTConnection::processConnection to %s established",socketId,a.c_str());
	_sendEvent.setEvent();
   return 0;
}


void PUDTConnection::CCUpdate()
{
   interPacketInterval = (UINT64)ccc->packetSendingPeriod;
   congestionWindowSize = ccc->congestionWidowSize;

   if (maxTransferRate <= 0)
      return;
   const double minSP = 1000000.0 / (double(maxTransferRate) / MSS); 
   if (interPacketInterval < minSP)
       interPacketInterval = (UINT64) minSP;
}



int PUDTConnection::send(const char* data, int len)
{
	if(status != CONNECTED)
		return -2;
    if (len <= 0)
       return 0;

	if (sendBuffer->getPacketCount() == 0)
   {
      // delay the EXP timer to avoid mis-fired timeout
	  lastRespTime = PUDTTimerQueue::getTimestamp();
   }

   if (maxSendBufferSize <= sendBuffer->getPacketCount())
   {
	   return 0;
   }

   int size = (maxSendBufferSize - sendBuffer->getPacketCount()) * maxPayloadSize;
   if (size > len)
      size = len;
   
   // insert the user buffer into the sening list
   if (sendBuffer->addMessage(data, size))
   {
		cont.sendTimerQueue.addTimer(socketId,1,false);
		return size;
   }
   else
	   return 0;
}

int PUDTConnection::recv(char* data, int len)
{
   if(status != CONNECTED)
		return -2;
   if (len <= 0)
       return 0;
   if (0 == recvBuffer->getReadyPacketCount())
	  return 0;
   int res = recvBuffer->readBuffer(data, len);
   return res;
}



void PUDTRecvThread::run()
{
	try
	{
		while(!cont.closing)
		{
			PUDTPacket *p = cont.packetStorage.get();
			if (!p)
			{
				
				// drop packet
				static PUDTPacket temp;
				if (temp.data == 0)
				{
					temp.data=new char[cont.udpMSS - UDP_HEADER_SIZE - IP_HEADER_SIZE - PUDTPacket::hdrSize];
				}
                temp.setLength(cont.udpMSS - UDP_HEADER_SIZE - IP_HEADER_SIZE - PUDTPacket::hdrSize);
				PUDPSocket::Descriptor addr;  
				((sockaddr_in *)&addr)->sin_addr.s_addr = 0;
				int ret = cont.recvfrom(temp,addr);
				if (cont.closing)
					break;
				if (ret < 0)
				{
					if (((sockaddr_in *)&addr)->sin_addr.s_addr != 0)
					{
						while(1)
						{
							PUDTConnectionLock connLock;
							PUDTConnection * conn=cont.findConnection(addr, 0, connLock);
							if (conn)
							{
								PTRACE3("%8.8x. Read thread, connection reset", conn->socketId);
								if (conn->status == PUDTConnection::CONNECTED) 
								{
									conn->reset();
								}
							}
							else
								break;
						}
					}
				}
				else
				{
					PTRACE3("Read thread: drop packet %x due to lack of buffers",temp.seqNo);
				}
			}
			else
			{
				PUDPSocket::Descriptor addr;
				((sockaddr_in *)&addr)->sin_addr.s_addr=0;
				((sockaddr_in *)&addr)->sin_port=0;
				bool keepPacket = false;
				int ret = cont.recvfrom(*p,addr);
				if (cont.closing)
					break;
				if (ret > 0)
				{
					if (!p->id)
					{
						if(cont.listener)
						{
							PUDTConnectionLock connLock;
							PUDTConnection * l = cont.findConnection(cont.listener,connLock);
							if (l && l->status == PUDTConnection::LISTENING)
							{
								l->listen(addr,p);
							}
						}
					}
					else // p->id > 0
					{
						PUDTConnectionLock connLock;
						PUDTConnection * conn = cont.findConnection(p->id,connLock);
						if (conn)
						{
							if (conn->status == PUDTConnection::CONNECTING)
								conn->processConnection(addr,p);
							else if (conn->status == PUDTConnection::CONNECTED)
							{
								if(p->getFlag() == 0)
								{
									if (conn->processData(addr,p))
										keepPacket=true;
								}
								else
								{
									conn->processCtrl(addr,p);
								}
							}
						}
					}
				}
				else if (ret < 0)
				{
					if (((sockaddr_in *)&addr)->sin_addr.s_addr!=0)
					{
						while(1)
						{
							PUDTConnectionLock connLock;
							PUDTConnection * conn;
							conn=cont.findConnection(addr,0,connLock);
							if (conn)
							{
								PTRACE3("%8.8x. Read thread, connection reset", conn->socketId);
								if (conn->status == PUDTConnection::CONNECTED) 
								{
									conn->reset();
								}
							}
							else
								break;
						}
					}
				}
				if(!keepPacket)
				{
					cont.packetStorage.return_back(p);
				}

			}
			//
			{
				vector<UINT32> cnum;
				cont.getConnections(cnum);
				for (int i=0; i < cnum.size(); i++)
				{
					PUDTConnectionLock connLock;
					PUDTConnection * conn;
					conn=cont.findConnection(cnum[i],connLock);
					if (conn && conn->status == PUDTConnection::CONNECTED)
						conn->checkTimers();
				}
		
			}
		}
	}
	catch(PError &er)
	{
		PTRACE3("Error send %s", er.why());
	}

}

void PUDTSendThread::run()
{
	try
	{
		while (!cont.closing)
		{

			UINT64 ts;
			UINT32 connId;
			int code = cont.sendTimerQueue.waitPop(connId,ts,INFINITE);

			if (code == PUDTTimerQueueMap::waitPopShutdown)
				break;
			if (code == PUDTTimerQueueMap::waitPopTimer)
			{
				PUDTPacket pkt;
				PUDTConnectionLock conLock;
				PUDTConnection * conn = cont.findConnection(connId,conLock);
				if(conn && conn->status==PUDTConnection::CONNECTED)
				{
					UINT64 nextTs = 0;
					bool ok =  false;
					do 
					{
						ok = conn->packData(pkt,nextTs) > 0;
						if (ok)
						{
							int size = cont.sendto(pkt,conn->peerAddress);
							if (size < 0)
							{
								PTRACE3("%8.8x Send thread, connection reset", conn->socketId);
								if (conn->status == PUDTConnection::CONNECTED) 
								{
									conn->reset();
								}
							}
						}
						else
							break;
					}
					while(conn->status == PUDTConnection::CONNECTED && nextTs<=PUDTTimerQueueMap::getTimestamp());
					if (conn->status == PUDTConnection::CONNECTED && ok)
						cont.sendTimerQueue.addTimer(connId,nextTs,true);
				}
			}
		}
	}
	catch(PError &er)
	{
		PTRACE3("Error send %s", er.why());
	}
}


int PUDTSocketContainer::sendto(PUDTPacket &pkt,const PUDPSocket::Descriptor & peerAddress) const
{
   // convert control information into network order
   if (pkt.getFlag())
      for (int i = 0, n = pkt.getLength() / 4; i < n; ++ i)
         *((UINT32 *)pkt.data + i) = htonl(*((UINT32 *)pkt.data + i));

   // convert packet header into network order
   //for (int j = 0; j < 4; ++ j)
   //   packet.m_nHeader[j] = htonl(packet.m_nHeader[j]);
   UINT32* p = pkt.header;
   for (int j = 0; j < 4; ++ j)
   {
      *p = htonl(*p);
      ++ p;
   }

   int ret;
   try
   {
	   ret = socket->sendnoexc(pkt.packetVector,2,PUDTPacket::hdrSize + pkt.getLength(),&peerAddress);
   }
	catch( PError &er)
	{
		PString a;
		PUDPSocket::address(a,peerAddress);
		PTRACE3("socket.sendnoexc(%s): error: %s", a.c_str(),er.why());
		ret = -1;
	}

   // convert back into local host order
   //for (int k = 0; k < 4; ++ k)
   //   packet.m_nHeader[k] = ntohl(packet.m_nHeader[k]);
   p = pkt.header;
   for (int k = 0; k < 4; ++ k)
   {
      *p = ntohl(*p);
       ++ p;
   }

   if (pkt.getFlag())
      for (int i = 0, n = pkt.getLength() / 4; i < n; ++ i)
         *((UINT32 *)pkt.data + i) = htonl(*((UINT32 *)pkt.data + i));

   if (ret <= 0)
	   return ret;
   else 
	   return pkt.getLength();
}

int PUDTSocketContainer::recvfrom(PUDTPacket &pkt, PUDPSocket::Descriptor & peerAddress) const
{
	int  size;
	try
	{
		size = socket->recvnoexc(pkt.packetVector,2,&peerAddress);
	}
	catch( PError &er)
	{
		PString a;
		PUDPSocket::address(a,peerAddress);
		PTRACE3("socket.recvnoexc(%s): error: %s", a.c_str(),er.why());
		return -1;
	}
	if (size  <= 0)
		return size;
	if (size < PUDTPacket::hdrSize)
		return 0;
	pkt.setLength(size - PUDTPacket::hdrSize);

   UINT32* p = pkt.header;
   for (int i = 0; i < 4; ++ i)
   {
      *p = ntohl(*p);
      ++ p;
   }

   if (pkt.getFlag())
      for (int i = 0, n = pkt.getLength() / 4; i < n; ++ i)
         *((UINT32 *)pkt.data + i) = htonl(*((UINT32 *)pkt.data + i));

   return pkt.getLength();
}


PUDTConnection::PUDTConnection(PUDTSocketContainer & cont_):cont(cont_),_sendEvent( FALSE, FALSE ),_recvEvent( FALSE, FALSE ),accessed(0)
{
    maxTransferRate = -1;			
	MSS = cont.udpMSS;
	flightFlagSize = cont.udpFlightFlagSize;
	maxSendBufferSize = cont.udpMaxSendBufferSize;
    maxRecvBufferSize = cont.udpMaxRecvBufferSize;
	RTT = cont.udpRTT;             
	estimatedBandwidth = cont.udpEstimatedBandwidth;

	status = INIT;
	recvBuffer = 0;
	sendBuffer = 0;
	ccc = 0;
	recvLostList = 0;
	sendLostList = 0;
	ackWindow = 0;
	recvTimeWindow = 0;
	sendTimeWindow = 0;

	lastRespTime = 0;
	lastAckRecTime = 0;
	ackReceived = false;
	nextAckTime = 0;	
	lastAck2SentTime = 0;
	lastAck2Sent = 0;
	lastAcknowledgedAckSent = 0;
	lastDataAckRecv = 0;
	lastAckSeqNo = 0;
	packetCounter = 0; 
	lightACKCounter = 0;
	RTTVar = 0;
	
	
	deliveryRate = 0;      
	expirationCounter = 0; 
	totalPacketsRecv = totalPacketsRecvReportingInterval = 0;
	totalPacketsSent = totalPacketsSentReportingInterval = 0;
	totalAckRecv = 0;
	totalAckSent = 0;          
	totalRecvPacketLoss = totalRecvPacketLossReportingInterval = 0;
    totalRecvPacketDropped = totalRecvPacketDroppedReportingInterval = 0;
    totalDupPacketsRecvd = totalDupPacketsRecvdReportingInterval = 0;
	totalDup2PacketsRecvd = totalDup2PacketsRecvdReportingInterval = 0;
	totalSendPacketLoss = 0;   
	totalNackRecv = 0;         
	totalNackSent = 0;         
	totalRetransmit = totalRetransmitReportingInterval = 0;
	currentRecvSeqNumber = 0; 
	lastAckSent = 0;
	lastAckSentTime = 0; 
	lastAckRecv = 0;
	maxPayloadSize = 0; 
	currentSendSeqNumber = 0;
	interPacketInterval = 0; 
	nextPacketTargetTime = 0;
	interPacketTimeDiff = 0; 
	SYNInt = 0; 
    ACKInt = 0; 
	NACKInt = 0;
	minExpTimeout = 0;
	minNakInt = 0;
    flowWindowSize = 0;       
    congestionWindowSize = 0; 
	initialSeqNumber = 0;  
	socketId = 0;
	peerId = 0;
}


PUDTConnection::PUDTConnection(const PUDTConnection & c ):cont(c.cont),_sendEvent( FALSE, FALSE ),_recvEvent( FALSE, FALSE ),accessed(0)
{
    maxTransferRate = c.maxTransferRate;		
	MSS = c.MSS;            
	flightFlagSize = c.flightFlagSize;
	maxSendBufferSize = c.maxSendBufferSize;
    maxRecvBufferSize = c.maxRecvBufferSize;
	RTT = cont.udpRTT;             
	estimatedBandwidth = cont.udpEstimatedBandwidth;

	status = INIT;

	recvBuffer = 0;
	sendBuffer = 0;
	ccc = 0;
	recvLostList = 0;
	sendLostList = 0;
	ackWindow = 0;
	recvTimeWindow = 0;
	sendTimeWindow = 0;

	lastRespTime = 0;
	lastAckRecTime = 0;
	ackReceived = false;

	nextAckTime = 0;	
	lastAck2SentTime = 0;
	lastAck2Sent = 0;
	lastAcknowledgedAckSent = 0;
	lastDataAckRecv = 0;
	lastAckSeqNo = 0;
	packetCounter = 0; 
	lightACKCounter = 0;
	RTTVar = 0;
	deliveryRate = 0;      
	expirationCounter = 0; 
	totalPacketsRecv = totalPacketsRecvReportingInterval = 0;
	totalPacketsSent = totalPacketsSentReportingInterval = 0;
	totalAckRecv = 0;
	totalAckSent = 0;          
	totalRecvPacketLoss = totalRecvPacketLossReportingInterval = 0;
    totalRecvPacketDropped = totalRecvPacketDroppedReportingInterval = 0;
    totalDupPacketsRecvd = totalDupPacketsRecvdReportingInterval = 0;
	totalDup2PacketsRecvd = totalDup2PacketsRecvdReportingInterval = 0;
	totalSendPacketLoss = 0;   
	totalNackRecv = 0;         
	totalNackSent = 0;         
	totalRetransmit = totalRetransmitReportingInterval = 0;
	currentRecvSeqNumber = 0; 
	lastAckSent = 0;
	lastAckSentTime = 0; 
	lastAckRecv = 0;
	maxPayloadSize = 0; 
	currentSendSeqNumber = 0;
	interPacketInterval = 0; 
	nextPacketTargetTime = 0;
	interPacketTimeDiff = 0; 
	SYNInt = 0; 
    ACKInt = 0; 
	NACKInt = 0;
	minExpTimeout = 0;
	minNakInt = 0;
    flowWindowSize = 0;       
    congestionWindowSize = 0; 
	initialSeqNumber = 0;  
	socketId = 0;
	peerId = 0;
}


PUDTConnection::~PUDTConnection()
{
	delete recvBuffer;
	delete sendBuffer;
	delete ccc;
	delete recvLostList;
	delete sendLostList;
	delete ackWindow;
	delete recvTimeWindow;
	delete sendTimeWindow;
	PTRACE3("%8.8x Connection deleted", socketId);
}

void PUDTConnection::open()
{
	status = OPENED;
	maxPayloadSize = MSS - UDP_HEADER_SIZE - IP_HEADER_SIZE - PUDTPacket::hdrSize;
	RTT = cont.udpRTT;             
	estimatedBandwidth = cont.udpEstimatedBandwidth;
	deliveryRate = 16;
	expirationCounter = 1;
	RTTVar = RTT >> 1;
	SYNInt = PUDT_SYNInterval;
    minExpTimeout = 300000;
	minNakInt = 300000;
	ACKInt = SYNInt;
	NACKInt = minNakInt;
	startTime = PUDTTimerQueue::getTimestamp();
    lastRespTime = startTime;
	lastAckRecTime = startTime;
	ackReceived = false;
    nextAckTime = startTime + SYNInt;
	nextNackTime = startTime + NACKInt;
    packetCounter = 0;
    lightACKCounter = 1;

    nextPacketTargetTime = 0;
    interPacketTimeDiff = 0;
}



void PUDTConnection::reset()
{
	status = BROKEN;
	cont.removeConnection(this);
	releaseSynch();
}



PUDTConnectionLock::PUDTConnectionLock()
{
	connection = 0;
}
void PUDTConnectionLock::set( PUDTConnection* conn_ )
{
	PASSERT3(conn_);
	PASSERT3(!connection);
	connection = conn_;
}
void PUDTConnectionLock::lock( PUDTConnection* conn_ )
{
	PASSERT3(conn_);
	PASSERT3(!connection);
	connection = conn_;
	connection->accessed.increment();
}
PUDTConnectionLock::~PUDTConnectionLock()
{
	if (connection)
	{
		bool c = connection->accessed.decrement();
		if (connection->status == PUDTConnection::CLOSED || connection->status == PUDTConnection::BROKEN)
		{
			if (c)
			{
				delete connection;
			}
		}
	}
}

PUDTSocketContainer::PUDTSocketContainer() :
#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable:4355)
#endif
	sndThread(*this), rcvThread(*this)
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
{
	listener = 0;
	closing = false;
	udpMSS = 1500;
	udpFlightFlagSize = 25600;
	udpMaxSendBufferSize = 8192;
    udpMaxRecvBufferSize = 8192;
	udpRTT = 1000;
	udpEstimatedBandwidth = 1;

	socket = new PUDPSocket();
}

PUDTSocketContainer::~PUDTSocketContainer()
{
	if(socket != 0)
		delete socket;
}

void PUDTSocketContainer::start()
{
	packetStorage.init(udpMSS - UDP_HEADER_SIZE - IP_HEADER_SIZE - PUDTPacket::hdrSize, udpMaxRecvBufferSize, this);

	socket->settimeout(1);
	sndThread.start();
	rcvThread.start();
}
void PUDTSocketContainer::shutdown()
{
	closing=true;
	sendTimerQueue.shutdown();
	socket->close();
	PLock lock(connectionsLock);
	int i;
	PUDTConnection ** c;
	while((c = connections.firstIndex(i)) != nullptr)
	{
		if ((*c)->status != PUDTConnection::CLOSED && (*c)->status != PUDTConnection::BROKEN)
		{
			(*c)->status = PUDTConnection::CLOSED;
			connections.remove((*c)->socketId);
			(*c)->releaseSynch();
		}
	}
}

PUDTConnection * PUDTSocketContainer::findConnection(UINT32 socketId, PUDTConnectionLock& acc )
{
	PLock lock(connectionsLock);
	PUDTConnection ** c_ =  connections.find(socketId);
	if (!c_)
		return 0;
	PUDTConnection * c = *c_;
	c->accessed.increment();
	if (c->status == PUDTConnection::CLOSED || c->status ==  PUDTConnection::BROKEN)
	{
		// actually it should not happen
		PTRACE3("%8.8x Connection in CLOSED/BROKEN state still in container", socketId);
		connections.remove(socketId);
		bool ref = c->accessed.decrement();
		if (ref)
		{
			delete c;
		}
		return 0;
	}
	acc.set(c);
	return c;
}
PUDTConnection * PUDTSocketContainer::findConnection(const PUDPSocket::Descriptor & peerAddress,UINT32 peerId, PUDTConnectionLock& acc)
{
	int i;
	PUDTConnection ** c;
	PLock lock(connectionsLock);
	for ( c = connections.firstIndex(i);c;c=connections.nextIndex(i))
	{
		if (  ((sockaddr_in *)&(*c)->peerAddress)->sin_addr.s_addr ==  ((sockaddr_in *)(&peerAddress))->sin_addr.s_addr && 
				((sockaddr_in *)&(*c)->peerAddress)->sin_port ==  ((sockaddr_in *)(&peerAddress))->sin_port &&
				(peerId == 0 || (*c)->peerId == peerId)
			)
		{
			acc.lock(*c);
			return (*c);
		}
	}
	return 0;
}
void PUDTSocketContainer::getConnections(vector<UINT32>& v)
{
	v.clear();
	v.reserve(connections.size());
	int i;
	PUDTConnection ** c;
	PLock lock(connectionsLock);

	for ( c = connections.firstIndex(i);c;c=connections.nextIndex(i))
	{
		v.push_back((*c)->socketId);
	}
}
UINT32 PUDTSocketContainer::addConnection(PUDTConnection * c, PUDTConnectionLock& acc)
{
	PLock lock(connectionsLock);
	UINT32 handle = connections.add(c);
	acc.lock(c);
	c->socketId = handle;
	return handle;
}

void PUDTSocketContainer::removeConnection(PUDTConnection * c)
{
	PLock lock(connectionsLock);
	PUDTConnection ** c_ =  connections.find(c->socketId);
	if (c_)
		connections.remove(c->socketId);
}

size_t PUDTSocketContainer::getNumConnections() const
{
	PLock lock(connectionsLock);
	return connections.size();
}

UINT32 PUDTSocketContainer::create()
{
	PUDTConnection * c = new PUDTConnection(*this);
	c->open();
	PUDTConnectionLock acc;
	UINT32 handle = addConnection(c,acc);
	return handle;
}

int  PUDTSocketContainer::listen(UINT32 socket_)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if (!c)
		return -1;
	if(c->status != PUDTConnection::OPENED)
		return -1;
	if(listener)
		return -1;
	listener = socket_;
	c->status = PUDTConnection::LISTENING;
	return 0;
}

int PUDTSocketContainer::accept(UINT32 socket_, PUDPSocket::Descriptor& ret)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if (!c)
		return -1;
	while(1)
	{
		if(c->status != PUDTConnection::LISTENING)
			return -1;
		if (c->waitingForAccept.size() == 0)
		{
			DWORD wret = ::WaitForSingleObject(c->_recvEvent.getHandle(),INFINITE);
			if (wret != WAIT_OBJECT_0 || c->waitingForAccept.size() == 0)
				return -1;
			if(c->status != PUDTConnection::LISTENING)
				return -1;
		}
		UINT32 s = *c->waitingForAccept.begin();
		c->waitingForAccept.pop_front();
		PUDTConnectionLock accNew;
		PUDTConnection * cNew =  findConnection(s,accNew);
		if (cNew) 
		{
			if(cNew->status == PUDTConnection::CONNECTED)
			{
				ret = cNew->peerAddress;
				ret.sock = s;
				return 0;
			}
			if(cNew->status != PUDTConnection::CLOSED || cNew->status != PUDTConnection::BROKEN)
			{
				PTRACE3("%8.8x Acccept: new socket has status %d", s, cNew->status);
			}
		}
	}
}

int  PUDTSocketContainer::connect(UINT32 socket_,const char * peer, int port, PUDPSocket::Descriptor& peerAddress)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if(!c || c->status != PUDTConnection::OPENED)
		return -1;
	PString addrS;
	PIPSocket::hostByName(addrS,peer);
	PIPSocket::composeDescriptor(peerAddress,port,addrS);
	return c->connect(peerAddress);
}


int PUDTSocketContainer::recvnonblocking(UINT32 socket_, char * buf, int size)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if(!c)
		return -1;
	return c->recv(buf,size);
}

int  PUDTSocketContainer::sendnonblocking(UINT32 socket_, char * buf, int size)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if(!c)
		return -1;
	return c->send(buf,size);
}

HANDLE PUDTSocketContainer::getsockreadevent(UINT32 socket_)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if(!c || c->status != PUDTConnection::CONNECTED)
		return 0;
	return c->_recvEvent.getHandle();
}

HANDLE PUDTSocketContainer::getsockwriteevent(UINT32 socket_)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if(!c || c->status != PUDTConnection::CONNECTED)
		return 0;
	return c->_sendEvent.getHandle();
}

void PUDTSocketContainer::close(UINT32 socket_)
{
	PUDTConnectionLock acc;
	PUDTConnection * c =  findConnection(socket_,acc);
	if (!c || c->status == PUDTConnection::CLOSING || c->status == PUDTConnection::CLOSED || c->status == PUDTConnection::BROKEN)
		return;
	if(c->status == PUDTConnection::CONNECTED || c->status == PUDTConnection::CONNECTING)
	{
		c->status = PUDTConnection::CLOSING;
		c->sendCtrl(5);
	}
	else if (c->status == PUDTConnection::LISTENING)
	{
		if (listener == socket_)
		{
			listener = 0;
		}
		else
		{
			PTRACE3("%8.8x Close() - listener %d is not equal to listening socket ", socket_,listener);
		}
	}
	c->status = PUDTConnection::CLOSED;
	removeConnection(c);
	c->releaseSynch();
}

void  PUDTSocketContainer::printCounters()
{
	vector<UINT32> v;
	getConnections(v);
	for (int  i =0;i<v.size();i++)
	{
		PUDTConnectionLock acc;
		PUDTConnection * c =  findConnection(v[i],acc);
		if (c)
			c->printCounters();
	}
}

void PUDTConnection::printCounters()
{
    PString a;
    PUDPSocket::address(a,peerAddress);

	PLog("%8.8x %s: RTT=%d, estB=%d devileryRate=%d,",socketId,a.c_str(),RTT,estimatedBandwidth,deliveryRate); 
	PLog("   flowWindowSize=%d congestWindow=%.2lf interPacketInterval=%I64u",flowWindowSize,congestionWindowSize,interPacketInterval);
	if(status == CONNECTED || status == CONNECTING || status == CLOSING)
	{
		PLog("   currentSendSeqNumber=%8.8x, lastAckSent=%8.8x, lastDataAckRecv=%8.8x, sendLostList size=%d, totalRetransmit=%u, totalPacketsSent=% " PRIu64,
			currentSendSeqNumber, lastAckSent, lastDataAckRecv, sendLostList->getLossCount(), totalRetransmit, totalPacketsSent);
		PLog("   totalAckSent=%" PRIu64 ", totalNackSent=%u, interPacketInterval=%" PRIu64 ", interPacketTimeDiff=%" PRIu64 " ploss=%f", totalAckSent, totalNackSent,
			interPacketInterval, interPacketTimeDiff, (totalPacketsSent - totalRetransmit ? 100.0 * totalRetransmit / (totalPacketsSent - totalRetransmit) : 0));
		PLog("   currentRecvSeqNumber=%8.8x, lastAckRecv=%8.8x, lastAcknowledgedAckSent=%8.8x, recvLostList size=%d, totalRecvPacketLoss=%u, totalPacketsRecv=%" PRIu64 ,
			currentRecvSeqNumber, lastAckRecv, lastAcknowledgedAckSent, recvLostList->getLossCount(), totalRecvPacketLoss, totalPacketsRecv);
		PLog("   totalRecvPacketDropped=%u, totalDupPacketsRecvd=%u, totalDup2PacketsRecvd=%u, totalAckRecv=%" PRIu64 ", totalNackRecv=%u, ploss=%f",
			   totalRecvPacketDropped, totalDupPacketsRecvd, totalDup2PacketsRecvd, totalAckRecv, totalNackRecv, (totalPacketsRecv - totalRecvPacketLoss - totalRecvPacketDropped - totalDupPacketsRecvd ? 100.0 * (totalRecvPacketLoss + totalRecvPacketDropped + totalDupPacketsRecvd) / (totalPacketsRecv - totalRecvPacketLoss - totalRecvPacketDropped - totalDupPacketsRecvd) : 0));

		if (ccc)
			PLog("          ccc->slowStart=%s,congestionWidowSize=%.2lf,RTT=%d,packetSendingPeriod=%.2lf,estimatedBandwidth=%d,recvRate=%d",(ccc->slowStart?"y":"n"),ccc->congestionWidowSize,ccc->RTT,ccc->packetSendingPeriod,ccc->estimatedBandwidth,ccc->recvRate);
	}
}

list<PUDTConnectionData> PUDTSocketContainer::getConnectionData()
{
    list<PUDTConnectionData> res;
    vector<UINT32> v;
    getConnections(v);
    for (size_t i = 0; i < v.size(); ++i)
    {
        PUDTConnectionLock acc;
        PUDTConnection* c =  findConnection(v[i], acc);
        if (c)
        {
            res.emplace_back();
            c->getConnectionData(res.back());
        }
    }
    return res;
}

void PUDTConnection::getConnectionData(PUDTConnectionData& connData)
{
    PUDPSocket::address(connData.address, peerAddress);
    connData.address.append(":");
    connData.address.appendInt(PUDPSocket::port(peerAddress));

    connData.flowWindowSize = flowWindowSize;
    connData.congestionWindowSize = congestionWindowSize;
    connData.interPacketInterval = interPacketInterval;
    connData.sendLostListSize = (sendLostList ? sendLostList->getLossCount() : 0);
    connData.totalRetransmit = totalRetransmitReportingInterval;
    connData.totalPacketsSent = totalPacketsSentReportingInterval;
    
    double sendPacketLoss = (totalPacketsSentReportingInterval - totalRetransmitReportingInterval ? 100.0 * totalRetransmitReportingInterval / (totalPacketsSentReportingInterval - totalRetransmitReportingInterval) : 0);
    connData.sendPacketLoss.appendDouble(sendPacketLoss);
    
    connData.recvLostListSize = (recvLostList ? recvLostList->getLossCount() : 0);
    connData.totalRecvPacketLoss = totalRecvPacketLossReportingInterval;
    connData.totalPacketsRecv = totalPacketsRecvReportingInterval;
    connData.totalRecvPacketDropped = totalRecvPacketDroppedReportingInterval;
    connData.totalDupPacketsRecvd = totalDupPacketsRecvdReportingInterval;
    
    double recvPacketLoss = (totalPacketsRecvReportingInterval - totalRecvPacketLossReportingInterval - totalRecvPacketDroppedReportingInterval - totalDupPacketsRecvdReportingInterval ? 100.0 * (totalRecvPacketLossReportingInterval + totalRecvPacketDroppedReportingInterval + totalDupPacketsRecvdReportingInterval) / (totalPacketsRecvReportingInterval - totalRecvPacketLossReportingInterval - totalRecvPacketDroppedReportingInterval - totalDupPacketsRecvdReportingInterval) : 0);
    connData.recvPacketLoss.appendDouble(recvPacketLoss);

	totalPacketsSentReportingInterval = totalPacketsRecvReportingInterval = totalRetransmitReportingInterval = totalRecvPacketLossReportingInterval = totalRecvPacketDroppedReportingInterval = totalDupPacketsRecvdReportingInterval = 0;
}

#endif
