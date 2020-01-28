#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pudtpacket.h"
#include "pudtsocket.h"
#include "pplogfile.h"

const int PUDTPacket::hdrSize = 16;
const int PUDTHandShake::contentSize = 48;

int PUDTPacket::getLength() const
{
   return packetVector[1].len;
}

void PUDTPacket::setLength(int len)
{
   packetVector[1].len = len;
}

PUDTHandShake::PUDTHandShake():
version(0),
type(0),
ISN(0),
MSS(0),
flightFlagSize(0),
reqType(0),
id(0),
cookie(0)
{
	for (int i = 0; i < 4; ++ i)
		peerIP[i] = 0;
}

int PUDTHandShake::serialize(char* buf, int& size)
{
   if (size < contentSize)
      return -1;

   INT32* p = (INT32*)buf;
   *p++ = version;
   *p++ = type;
   *p++ = ISN;
   *p++ = MSS;
   *p++ = flightFlagSize;
   *p++ = reqType;
   *p++ = id;
   *p++ = cookie;
   for (int i = 0; i < 4; ++ i)
      *p++ = peerIP[i];

   size = contentSize;

   return 0;
}

int PUDTHandShake::deserialize(const char* buf, int size)
{
	if (size < contentSize)
		return -1;

	INT32* p = (INT32*)buf;
	version = *p++;
	type = *p++;
	ISN = *p++;
	MSS = *p++;
	flightFlagSize = *p++;
	reqType = *p++;
	id = *p++;
	cookie = *p++;
	for (int i = 0; i < 4; ++ i)
		peerIP[i] = *p++;

    return 0;
}



void PUDTPacket::pack(int pkttype, void* lparam, void* rparam, int size)
{
	PTRACE5("PUDTPacket::pack(%d, size=%d)", pkttype ,size);
   // Set (bit-0 = 1) and (bit-1~15 = type)
   header[0] = 0x80000000 | (pkttype << 16);

   // Set additional information and control information field
   switch (pkttype)
   {
   case 2: //0010 - Acknowledgement (ACK)
      // ACK packet seq. no.
      if (NULL != lparam)
         header[1] = *(INT32 *)lparam;

      // data ACK seq. no. 
      // optional: RTT (microsends), RTT variance (microseconds) advertised flow window size (packets), and estimated link capacity (packets per second)
      packetVector[1].buf = (char *)rparam;
      packetVector[1].len = size;

      break;

   case 6: //0110 - Acknowledgement of Acknowledgement (ACK-2)
      // ACK packet seq. no.
      header[1] = *(INT32 *)lparam;

      // control info field should be none
      // but "writev" does not allow this
      packetVector[1].buf = (char *)&__pad; //NULL;
      packetVector[1].len = 4; //0;

      break;

   case 3: //0011 - Loss Report (NAK)
      // loss list
      packetVector[1].buf = (char *)rparam;
      packetVector[1].len = size;

      break;

   case 4: //0100 - Congestion Warning
      // control info field should be none
      // but "writev" does not allow this
      packetVector[1].buf = (char *)&__pad; //NULL;
      packetVector[1].len = 4; //0;
  
      break;

   case 1: //0001 - Keep-alive
      // control info field should be none
      // but "writev" does not allow this
      packetVector[1].buf = (char *)&__pad; //NULL;
      packetVector[1].len = 4; //0;

      break;

   case 0: //0000 - Handshake
      // control info filed is handshake info
      packetVector[1].buf = (char *)rparam;
      packetVector[1].len = size; //sizeof(CHandShake);

      break;

   case 5: //0101 - Shutdown
      // control info field should be none
      // but "writev" does not allow this
      packetVector[1].buf = (char *)&__pad; //NULL;
      packetVector[1].len = 4; //0;

      break;

   case 7: //0111 - Message Drop Request
      // msg id 
      header[1] = *(INT32 *)lparam;

      //first seq no, last seq no
      packetVector[1].buf = (char *)rparam;
      packetVector[1].len = size;

      break;

   case 8: //1000 - Error Signal from the Peer Side
      // Error type
      header[1] = *(INT32 *)lparam;

      // control info field should be none
      // but "writev" does not allow this
      packetVector[1].buf = (char *)&__pad; //NULL;
      packetVector[1].len = 4; //0;

      break;

   case 32767: //0x7FFF - Reserved for user defined control packets
      // for extended control packet
      // "lparam" contains the extended type information for bit 16 - 31
      // "rparam" is the control information
      header[0] |= *(INT32 *)lparam;

      if (NULL != rparam)
      {
         packetVector[1].buf = (char *)rparam;
         packetVector[1].len = size;
      }
      else
      {
         packetVector[1].buf = (char *)&__pad;
         packetVector[1].len = 4;
      }

      break;

   default:
      break;
   }
}

PIOBUF* PUDTPacket::getPacketVector()
{
   return packetVector;
}

int PUDTPacket::getFlag() const
{
   // read bit 0
   return header[0] >> 31;
}

int PUDTPacket::getType() const
{
   // read bit 1~15
   return (header[0] >> 16) & 0x00007FFF;
}

int PUDTPacket::getExtendedType() const
{
   // read bit 16~31
   return header[0] & 0x0000FFFF;
}

INT32 PUDTPacket::getAckSeqNo() const
{
   // read additional information field
   return header[1];
}

int PUDTPacket::getMsgBoundary() const
{
   // read [1] bit 0~1
   return header[1] >> 30;
}

bool PUDTPacket::getMsgOrderFlag() const
{
   // read [1] bit 2
   return (1 == ((header[1] >> 29) & 1));
}

INT32 PUDTPacket::getMsgSeq() const
{
   // read [1] bit 3~31
   return header[1] & 0x1FFFFFFF;
}

PUDTPacket* PUDTPacket::clone() const
{
   PUDTPacket* pkt = new PUDTPacket;
   memcpy(pkt->header, header, hdrSize);
   pkt->data = new char[packetVector[1].len];
   memcpy(pkt->data, data, packetVector[1].len);
   pkt->packetVector[1].len = packetVector[1].len;
   return pkt;
}

PUDTPacket* PUDTPacketStorage::get()
{
	while (!m_freeBuffersStack.empty())
	{
		auto bufferIt = m_freeBuffersStack.top();
		auto circularBufferPtr = (*bufferIt);
		auto packet = (circularBufferPtr ? circularBufferPtr->get() : nullptr);
		if (packet)
		{
			return packet;
		}
		bool expected = false;
		if (m_stackLock.compare_exchange_strong(expected, true, std::memory_order_release, std::memory_order_relaxed))
		{
			PUDTCircularBuffer* emptyBuffer = nullptr;
			m_freeBuffersStack.pop(emptyBuffer);
			m_stackLock.store(false, std::memory_order_release);
		}
	}
	bool expected = false;
	if (m_bufferLock.compare_exchange_strong(expected, true, std::memory_order_release, std::memory_order_relaxed))
	{
		if (m_buffers.size() < m_container->getNumConnections())
		{
			m_buffers.emplace_back(m_mss, m_count);
			auto& buf = m_buffers.back();
			m_freeBuffersStack.push(&buf);
			auto packet = buf.get();
			m_bufferLock.store(false, std::memory_order_release);
			PASSERT3(packet);
			return packet;
		}
		else
		{
			return nullptr;
		}
	}
	return get();
}

#endif