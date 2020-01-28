#ifndef pudtpacket_h_included
#define pudtpacket_h_included

#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "ppsocket.h"
#include "pplogfile.h"
#include "concurrent/concurrent_stack.h"

class PUDTCircularBuffer;

class PUDTPacket
{
public:
   INT32 & seqNo;                   // alias: sequence number
   INT32 & msgNo;                   // alias: message number
   INT32 & timeStamp;               // alias: timestamp
   INT32 & id;					   // alias: socket ID
   char* & data;                    // alias: data/control information
   PUDTCircularBuffer* parentCircularBuffer;

   static const int hdrSize;	    // packet header size

public:

	const char * print(char *s)
	{
		sprintf(s,"seq=%8.8x msg=%8.8x id=%8.8x size=%lu",  seqNo, msgNo, id,packetVector[1].len);
		return s;
	}

   PUDTPacket() :
	   seqNo((INT32&)(header[0])),
	   msgNo((INT32&)(header[1])),
	   timeStamp((INT32&)(header[2])),
	   id((INT32&)(header[3])),
	   data((char*&)(packetVector[1].buf)),
	   parentCircularBuffer(nullptr),
       __pad()
   {
	   for (int i = 0; i < 4; ++ i)
		  header[i] = 0;
	   packetVector[0].buf = (char *)header;
	   packetVector[0].len = PUDTPacket::hdrSize;
	   packetVector[1].buf = NULL;
	   packetVector[1].len = 0;
   }
   ~PUDTPacket() {}

      // Functionality:
      //    Get the payload or the control information field length.
      // Parameters:
      //    None.
      // Returned value:
      //    the payload or the control information field length.

   int getLength() const;

      // Functionality:
      //    Set the payload or the control information field length.
      // Parameters:
      //    0) [in] len: the payload or the control information field length.
      // Returned value:
      //    None.

   void setLength(int len);

      // Functionality:
      //    Pack a Control packet.
      // Parameters:
      //    0) [in] pkttype: packet type filed.
      //    1) [in] lparam: pointer to the first data structure, explained by the packet type.
      //    2) [in] rparam: pointer to the second data structure, explained by the packet type.
      //    3) [in] size: size of rparam, in number of bytes;
      // Returned value:
      //    None.

   void pack(int pkttype, void* lparam = NULL, void* rparam = NULL, int size = 0);

      // Functionality:
      //    Read the packet vector.
      // Parameters:
      //    None.
      // Returned value:
      //    Pointer to the packet vector.

   PIOBUF* getPacketVector();

      // Functionality:
      //    Read the packet flag.
      // Parameters:
      //    None.
      // Returned value:
      //    packet flag (0 or 1).

   int getFlag() const;

      // Functionality:
      //    Read the packet type.
      // Parameters:
      //    None.
      // Returned value:
      //    packet type filed (000 ~ 111).

   int getType() const;

      // Functionality:
      //    Read the extended packet type.
      // Parameters:
      //    None.
      // Returned value:
      //    extended packet type filed (0x000 ~ 0xFFF).

   int getExtendedType() const;

      // Functionality:
      //    Read the ACK-2 seq. no.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field (bit 16~31).

   INT32 getAckSeqNo() const;

      // Functionality:
      //    Read the message boundary flag bit.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field [1] (bit 0~1).

   int getMsgBoundary() const;

      // Functionality:
      //    Read the message inorder delivery flag bit.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field [1] (bit 2).

   bool getMsgOrderFlag() const;

      // Functionality:
      //    Read the message sequence number.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field [1] (bit 3~31).

   INT32 getMsgSeq() const;

      // Functionality:
      //    Clone this packet.
      // Parameters:
      //    None.
      // Returned value:
      //    Pointer to the new packet.

   PUDTPacket* clone() const;

   UINT32 header[4];               // The 128-bit header field
   PIOBUF packetVector[2];      // The 2-demension vector of UDT packet [header, data]

   INT32 __pad;

friend class PUDTRecvThread;
friend class PUDTSendThread;
};

class PUDTHandShake
{
public:
   PUDTHandShake();

   int serialize(char* buf, int& size);
   int deserialize(const char* buf, int size);

public:
   static const int contentSize;	// Size of hand shake data

public:
   INT32 version;          // UDT version
   INT32 type;             // UDT socket type
   INT32 ISN;              // random initial sequence number
   INT32 MSS;              // maximum segment size
   INT32 flightFlagSize;   // flow control window size
   INT32 reqType;          // connection request type: 1: regular connection request, 0: rendezvous connection request, -1/-2: response
   INT32 id;		// socket ID
   INT32 cookie;		// cookie
   INT32 peerIP[4];	// The IP address that the peer's UDP port is bound to
};

class PUDTCircularBuffer
{
	private:
	PUDTPacket *packets;
	int *available;
	int startPos;
	int endPos;
	int count;
	PCriticalSection cs;
	int mss;

	PUDTCircularBuffer(const PUDTCircularBuffer& other);
	PUDTCircularBuffer& operator=(const PUDTCircularBuffer& other);

public:
    PUDTCircularBuffer() : packets(nullptr), available(nullptr), startPos(0), endPos(0), count(0), mss(0) {}
	PUDTCircularBuffer(int mss_, int count_) : packets(new PUDTPacket[count_]), available(new int[count_]), startPos(0), endPos(count_ - 1), count(count_), mss(mss_)
	{
		char* t = new char [mss * count];
		for (int i = 0; i < count; ++i)
		{
			packets[i].data = t;
			t += mss;
			available[i] = i;
			packets[i].setLength(mss);
			packets[i].parentCircularBuffer = this;
		}
	}

	~PUDTCircularBuffer()
	{
		delete packets[0].data;
		delete [] packets;
		delete [] available;
	}
	
	PUDTPacket * get()
	{
		if(startPos == endPos)
			return 0;
		int pos = available[startPos];
		PASSERT3(pos >= 0 && pos < count);
		available[startPos] = -1;
		startPos = (startPos+1) % count;
		return &packets[pos];
	}

	void return_back(const PUDTPacket * p) 
	{
		PLock lock(cs); // different threads can return buffer 
		int pos = p - packets;
		PASSERT3(pos >= 0 && pos < count);
		packets[pos].setLength(mss);
		int newEndPos = (endPos+1) % count;
		PASSERT3(newEndPos != startPos);
		PASSERT3(available[newEndPos] == -1);
		available[newEndPos] = pos;
		endPos = newEndPos;
	}
};

class PUDTSocketContainer;

class PUDTPacketStorage
{
public:
	PUDTPacketStorage() : m_mss(0), m_count(0), m_container(nullptr)
	{
		m_bufferLock.store(false);
		m_stackLock.store(false);
	}

	void init(int mss, int count, const PUDTSocketContainer* container)
	{
		m_mss = mss;
		m_count = count;
		m_container = container;
		m_buffers.emplace_back(m_mss, m_count);
		m_freeBuffersStack.push(&m_buffers.back());
	}

	PUDTPacket* get();

	void return_back(const PUDTPacket* p) 
	{
		p->parentCircularBuffer->return_back(p);
		auto it = m_freeBuffersStack.top();
		if (it == m_freeBuffersStack.rend() || (*it) != p->parentCircularBuffer)
		{
			m_freeBuffersStack.push(p->parentCircularBuffer);
		}
	}

private:
	typedef concurrent_stack<PUDTCircularBuffer*> CircularBufferStack;

	int m_mss;
	int m_count;
	const PUDTSocketContainer* m_container;
	std::list<PUDTCircularBuffer> m_buffers;
	CircularBufferStack m_freeBuffersStack;
	std::atomic_bool m_bufferLock;
	std::atomic_bool m_stackLock;
};

#endif
#endif
