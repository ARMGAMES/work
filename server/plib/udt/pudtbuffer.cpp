#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pudtpacket.h"
#include "pudtbuffer.h"


PUDTSendBuffer::PUDTSendBuffer(INT32 size/* = 32*/, INT32 packetSize/* = 1500*/):
	cs(),
	firstPacket(0),
	currentPacket(0),
	firstFreePacket(0),
	nextMsgNumber(1),
	capacity(size),
	maxPacketSize(packetSize)
{
	// Create physical buffer.
	buffer = new char [capacity * maxPacketSize];

	// Circular vector of packets
	packetList = new Packet[capacity];
}

PUDTSendBuffer::~PUDTSendBuffer()
{
	// Destroy packet vector.
	delete[] packetList;

	// Destroy buffer.
	delete[] buffer;
}

bool PUDTSendBuffer::addMessage(const char* msg, INT32 length, bool inOrder/* = false*/)
{
	INT32 numberOfPackets = length / maxPacketSize;
	if ((length % maxPacketSize) != 0)
	{
		++numberOfPackets;
	}

	// Check if we have enough room in the buffer.
	if(numberOfPackets + getPacketCount() >= capacity)
	{
			return false;
	}

//	const BYTE * p = (const BYTE *) msg;
//	PTRACE3("add %u (%u) - %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x",firstFreePacket,length,p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9]);

	INT32 inOrderMask = inOrder;
	inOrderMask <<= 29;

	// Copy the data into the buffer.
	INT32 copySize = length;
	if(firstFreePacket + numberOfPackets > capacity)
	{
		copySize = (capacity - firstFreePacket) * maxPacketSize;
	}
	memcpy(buffer + (firstFreePacket * maxPacketSize), msg, copySize);
	if(length > copySize)
	{
		// Copy the rest of the message onto the beginning of the buffer.
		memcpy(buffer, msg + copySize, length - copySize);
	}

	// Fill details on the packets.
    INT32 cumulativeLen = 0;
	for (int i = 0; i < numberOfPackets; ++i)
	{
		packetList[firstFreePacket].length = ((i == numberOfPackets - 1) ? length - cumulativeLen : maxPacketSize);
		packetList[firstFreePacket].msgNumber = nextMsgNumber | inOrderMask;
        cumulativeLen += maxPacketSize;
		// Mark first and last packets.
		if (i == 0)
			packetList[firstFreePacket].msgNumber |= 0x80000000;
		if (i == numberOfPackets - 1)
			packetList[firstFreePacket].msgNumber |= 0x40000000;

		++firstFreePacket;
		if(firstFreePacket == capacity)
		{
			firstFreePacket = 0;
		}
	}

	++nextMsgNumber;
	if (nextMsgNumber == MaxMsgNumber)
		nextMsgNumber = 1;

	return true;
}

int PUDTSendBuffer::getPacket(char** payload, INT32& msgNumber)
{
	// No data to read
	if (getPacketCount() == 0)
		return 0;
	if (currentPacket == firstFreePacket)
		return 0;
	*payload = buffer + (currentPacket * maxPacketSize);
	INT32 readlen = packetList[currentPacket].length;
	msgNumber = packetList[currentPacket].msgNumber;

//	const BYTE * p = (const BYTE *) *payload;
//	PTRACE3("%u - %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x",currentPacket,p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9]);

	++currentPacket;
	if(currentPacket == capacity)
	{
		currentPacket = 0;
	}

	return readlen;
}

int PUDTSendBuffer::getPacket(INT32 offset, char** payload, INT32& msgNumber)
{
	if (offset >= getPacketCount())
	{
		// No data to read.
		return 0;
	}

	INT32 packetIndex = (firstPacket + offset) % capacity;

	*payload = buffer + (packetIndex * maxPacketSize);
	msgNumber = packetList[packetIndex].msgNumber;

	return packetList[packetIndex].length;
}

void PUDTSendBuffer::ackPackets(int offset)
{
	if (offset > getPacketCount())
	{
		// Acknowledge at most the number of stored packets.
		offset = getPacketCount();
	}

	firstPacket = (firstPacket + offset) % capacity;
}

int PUDTSendBuffer::getPacketCount() const
{
	return (firstFreePacket - firstPacket + capacity) % capacity;
}

////////////////////////////////////////////////////////////////////////////////
PUDTReceiveBuffer::PUDTReceiveBuffer(PUDTPacketStorage * packetStorage_, INT32 bufsize):
	cs(),
	packetStorage(packetStorage_),
    empty(true),
    firstPacket(0),
    notch(0),
    notAckPacket(0),
	capacity(bufsize)
{
	packetList = new PUDTPacket*[capacity];
	for (int i=0;i<capacity; i++)
		packetList[i]=0;
}

PUDTReceiveBuffer::~PUDTReceiveBuffer()
{
    for (int i = 0; i < capacity; ++i)
    {
        if (packetList[i])
        {
			try
			{
				packetStorage->return_back(packetList[i]);
			}
			catch (...)
			{
			}
        }
    }
	delete[] packetList;
}

bool PUDTReceiveBuffer::addPacket(PUDTPacket * packet, INT32 offset)
{
	// Check if we have available space.
	if(getReadyPacketCount() + offset > capacity)
	{
		return false;
	}

	INT32 position = (notAckPacket + offset) % capacity;

	// Check if the desired position is used.
	if(packetList[position])
	{
		return false;
	}

	// Store the packet.
	packetList[position] = packet;
	{
		PLock lock(cs);
		empty = false;
	}

	return true;
}

INT32 PUDTReceiveBuffer::readBuffer(char* data, int length)
{
	INT32 position = firstPacket;
	INT32 notAck = notAckPacket;
	INT32 readSize = 0;
	INT32 packetsCompleted = 0;

	while ((position != notAck) && (readSize < length))
	{
		// Get the amount of data available for reading on the current packet.
		INT32 packetSize = packetList[position]->getLength() - notch;

		// Ensure the given buffer does not overflow.
		if (packetSize > length - readSize)
		{
			packetSize = length - readSize;
		}

		memcpy(data, packetList[position]->data + notch, packetSize);
		data += packetSize;

		if (packetSize + notch == packetList[position]->getLength())
		{
			// If we finished reading a packet, flag its slot as free.
			PUDTPacket *p = packetList[position];
			packetList[position] = 0;
			packetStorage->return_back(p);

			// Move to next packet.
			++position;
			if (position == capacity)
			{
				position = 0;
			}

			// Reset the notch.
			notch = 0;

			++packetsCompleted;
		}
		else
		{
			// We still have data available in the current packet, update the notch.
			notch += packetSize;
		}

		readSize += packetSize;
	}

	// Check if the buffer is empty.
	if(position == notAck)
	{
		PLock lock(cs);
		empty = true;
	}

	firstPacket = position;

	return readSize;
}

INT32 PUDTReceiveBuffer::ackPackets(INT32 count)
{
	if (count > getAvailableSlots())
	{
		// Acknowledge at most the number of unack'ed packets.
		count = getAvailableSlots();
	}

	// Make sure all acked packets were stored.
	INT32 acked = 0;
	for(INT32 i = 0; i < count; ++i)
	{
		if(!packetList[(notAckPacket + i) % capacity])
		{
			break;
		}

		++acked;
	}

	notAckPacket = (notAckPacket + acked) % capacity;

	return acked;
}

INT32 PUDTReceiveBuffer::getAvailableSlots() const
{
	return capacity - getReadyPacketCount();
}

INT32 PUDTReceiveBuffer::getReadyPacketCount() const
{
	INT32 packetCount = notAckPacket - firstPacket;
	if( packetCount < 0)
	{
		packetCount += capacity;
	}

	return packetCount;
}

#endif //UDT_DEVELOPMENT