#ifndef udtbuffer_h_included
#define udtbuffer_h_included
#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT


/// Send buffer for UDT protocol.
/// This buffer keeps the packets being sent until acknowledged by the other side.
class PUDTSendBuffer
{
private:
	/// Critical section to control access to the buffer.
	PCriticalSection cs;

	/// Structure representing a packet.
	struct Packet
	{
		INT32 length;		// Length of the packet.
		INT32 msgNumber;	// Message number.
	};

	/// The packet list is a circular vector.
	Packet *packetList;

	/// Index of the first stored packet.
	INT32 firstPacket;

	/// Index of the current packet being sent.
	INT32 currentPacket;

	/// Index of the first free packet slot.
	INT32 firstFreePacket;

	/// Buffer containing actual packet payloads.
	char* buffer;

	/// Next sequential message number.
	INT32 nextMsgNumber;

	/// Total capacity in number of packets, determined at construction.
	INT32 capacity;

	/// Maximum size of a packet, determined at construction.
	INT32 maxPacketSize;

	/// Maximum message number, counting must begin from 1 again after this.
	static const INT32 MaxMsgNumber = 0x1FFFFFFF;

private:
	PUDTSendBuffer(const PUDTSendBuffer&);
	PUDTSendBuffer& operator=(const PUDTSendBuffer&);

public:
	/// Constructor
	/// @param[in] size Buffer size in packet count.
	/// @param[in] packetSize Maximum size of a packet.
	PUDTSendBuffer(INT32 size = 32, INT32 packetSize = 1500);
	~PUDTSendBuffer();

	/// Add message to send queue.
	/// The message is added and broken into packets as per the maximum packet size specified during construction.
	/// @param[in] msg The message to be stored. The contents will be copied into the internal buffer.
	/// @param[in] length The length of the message being stored.
	/// @param[in] inOrder Flag to specify whether the message must be received only after all previous ones or not.
	/// @return True if the message was added, false if the operation failed (buffer has no room for the message).
	bool addMessage(const char* msg, INT32 length, bool inOrder = false);

	/// Gets the next not transmitted packet.
	/// @param[out] payload Pointer to the packet payload to be transmitted.
	/// @param[out] msgNumber The message sequence number.
	/// @return Size of the packet (0 means no packet available).
	INT32 getPacket(char** payload, INT32& msgNumber);

	/// Gets a specific packet for retransmission.
	/// Gets a packet skipping a given offset from the first non-acknowledged packet.
	/// @param[in] offset The offset of the desired packet in relation to the last acknowledged packet.
	/// @param[out] payload Pointer to the packet content to be retransmitted.
	/// @param[out] msgNumber The message sequence number.
	/// @return Size of the packet (0 means no packet found).
	INT32 getPacket(INT32 offset, char** payload, INT32& msgNumber);

	/// Acknowledge packets.
	/// Acknowledges the given number of packets starting on the first non-acknowledged.
	/// @param[in] offset Number of packets to acknowledge.
	void ackPackets(INT32 offset);

	/// Get packet count.
	/// Get current number of packets in the buffer.
	/// @return The current number of packets in the buffer.
	INT32 getPacketCount() const;
};


class PUDTPacket;
class PUDTPacketStorage;

////////////////////////////////////////////////////////////////////////////////
class PUDTReceiveBuffer
{
private:
	/// Critical section to control access to the buffer.
	PCriticalSection cs;

	PUDTPacketStorage * packetStorage;
	/// The packet list is a circular vector.
	PUDTPacket **packetList;

	/// Flag to indicate an empty buffer, in order to differentiate it from a full buffer. 
	bool empty;

	/// Index of the first stored packet.
	INT32 firstPacket;

	/// Position in the current packet where next read should start.
	INT32 notch;

	/// Index of the first not-acknowledged packet.
	INT32 notAckPacket;

	/// Total capacity in number of packets, determined at construction.
	INT32 capacity;

private:
	PUDTReceiveBuffer(const PUDTReceiveBuffer&);
	PUDTReceiveBuffer& operator=(const PUDTReceiveBuffer&);

public:
	PUDTReceiveBuffer(PUDTPacketStorage * packetStorage_, INT32 bufsize = 65536);
	~PUDTReceiveBuffer();

	/// Add a packet to the buffer.
	/// @param[in] payload Pointer to the buffer where the payload resides. The contents will not be copied over, it is the responsibility of the caller to ensure the buffer will be kept until the message is read.
	/// @param[in] size Size of the payload in bytes.
	/// @param[in] offset Offset of the packet in relation to the last acked packet.
	/// @return True if successful, false if either the packet was already received or is too far ahead and does not fit in the buffer.
	bool addPacket(PUDTPacket * packet, INT32 offset);

	/// Read data from the buffer.
	/// This method ignores message boundaries, and reads payloads as a stream. Only acked packets are considered.
	/// @param[in] data Pointer to the user buffer to write on.
	/// @param[in] length Length of the buffer passed in the previous parameter.
	/// @return Actual length of data copied over.
	INT32 readBuffer(char* data, int length);

	/// Mark a number of packets as acknowledged.
	/// Only contiguously stored packets are ack'ed. If a free packet slot is found, the last ack'ed packet will be the one before it.
	/// @param[in] count The number of packets to acknowledge, starting from the one immediately after the last ack'ed one.
	/// @return the actual number of packets acknowledged.
	INT32 ackPackets(INT32 count);

	/// Get the number of available (non-acked) packet slots.
	/// @return The number of available packet slots.
	INT32 getAvailableSlots() const;

	/// Get the number of valid continuous packets available for reading.
	/// @return The number of packets available for reading.
	INT32 getReadyPacketCount() const;
};

#endif //udtbuffer_h_included

#endif //UDT_DEVELOPMENT