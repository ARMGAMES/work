#include "../../../unittest-cpp/UnitTest++/UnitTest++.h"
#include "ppthread.h"
#include "udt/pudtbuffer.h"
#include "udt/pudtpacket.h"
#include "udt/pudtlosslist.h"
#include "udt/pudtcommon.h"

TEST(UDTSendBuffer)
{
	// Create a buffer with 4 slots with 10 bytes each.
	PUDTSendBuffer* sendBuffer = new PUDTSendBuffer(5, 10);

	// Check that the buffer is empty.
	CHECK_EQUAL(0, sendBuffer->getPacketCount());

	// Try to get a packet from the empty buffer.
	char* payload = 0;
	INT32 msgNumber = 0;
	CHECK_EQUAL(0, sendBuffer->getPacket(&payload, msgNumber));
	CHECK_EQUAL(0, sendBuffer->getPacket(2, &payload, msgNumber));

	// Try to acknowledge packets on the empty buffer.
	sendBuffer->ackPackets(2);
	CHECK_EQUAL(0, sendBuffer->getPacketCount());

	// Create a message with 26 characters.
	const char* msg = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	// Add the message to the buffer.
	CHECK_EQUAL(true, sendBuffer->addMessage(msg, 26));

	// Check if the message was split into 3 packets.
	CHECK_EQUAL(3, sendBuffer->getPacketCount());

	// Get the first packet.
	INT32 size = sendBuffer->getPacket(&payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(10, size);
	CHECK_EQUAL(0x80000001, msgNumber);
	CHECK_ARRAY_EQUAL("ABCDEFGHIJ", payload, 10);

	// Get the next packet.
	size = sendBuffer->getPacket(&payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(10, size);
	CHECK_EQUAL(0x00000001, msgNumber);
	CHECK_ARRAY_EQUAL("KLMNOPQRST", payload, 10);

	// Get the next packet.
	size = sendBuffer->getPacket(&payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(6, size);
	CHECK_EQUAL(0x40000001, msgNumber);
	CHECK_ARRAY_EQUAL("UVWXYZ", payload, 6);

	// Check that the packet count did not change.
	CHECK_EQUAL(3, sendBuffer->getPacketCount());

	// Try to get the next packet, which does not exist.
	CHECK_EQUAL(0, sendBuffer->getPacket(&payload, msgNumber));

	// Try to add a message that's too large.
	CHECK_EQUAL(false, sendBuffer->addMessage(msg, 11));

	// Make sure the message was not added.
	CHECK_EQUAL(3, sendBuffer->getPacketCount());

	// Get the second package for retransmission.
	size = sendBuffer->getPacket(1, &payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(10, size);
	CHECK_EQUAL(0x00000001, msgNumber);
	CHECK_ARRAY_EQUAL("KLMNOPQRST", payload, 10);

	// Get an inexistent package for retransmission.
	CHECK_EQUAL(0, sendBuffer->getPacket(3, &payload, msgNumber));

	// Acknowledge 2 packets.
	sendBuffer->ackPackets(2);

	// Make sure the slots were freed.
	CHECK_EQUAL(1, sendBuffer->getPacketCount());

	// Get an inexistent package for retransmission.
	CHECK_EQUAL(0, sendBuffer->getPacket(1, &payload, msgNumber));

	// Get the only package left for retransmission.
	size = sendBuffer->getPacket(0, &payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(6, size);
	CHECK_EQUAL(0x40000001, msgNumber);
	CHECK_ARRAY_EQUAL("UVWXYZ", payload, 6);

	// Add a message that causes the buffer to fill (and wrap around the buffer).
	CHECK_EQUAL(true, sendBuffer->addMessage(msg, 26));

	// Make sure the message was added.
	CHECK_EQUAL(4, sendBuffer->getPacketCount());

	// Try to add another message.
	CHECK_EQUAL(false, sendBuffer->addMessage(msg, 1));

	// Make sure the message was not added.
	CHECK_EQUAL(4, sendBuffer->getPacketCount());

	// Check the number of available packets.
	CHECK_EQUAL(4, sendBuffer->getPacketCount());

	// Get the first packet.
	size = sendBuffer->getPacket(&payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(10, size);
	CHECK_EQUAL(0x80000002, msgNumber);
	CHECK_ARRAY_EQUAL("ABCDEFGHIJ", payload, 10);

	// Get the next packet.
	size = sendBuffer->getPacket(&payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(10, size);
	CHECK_EQUAL(0x00000002, msgNumber);
	CHECK_ARRAY_EQUAL("KLMNOPQRST", payload, 10);

	// Get the next packet.
	size = sendBuffer->getPacket(&payload, msgNumber);

	// Check the size, message number and payload.
	CHECK_EQUAL(6, size);
	CHECK_EQUAL(0x40000002, msgNumber);
	CHECK_ARRAY_EQUAL("UVWXYZ", payload, 6);

	delete sendBuffer;
}

TEST(UDTReceiveBuffer)
{
	char* payload1 = (char*)malloc(sizeof(char)* 10);
	memcpy(payload1, "ABCDEFGHIJ", 10);
	char* payload2 = (char*)malloc(sizeof(char)* 10);
	memcpy(payload2, "KLMNOPQRST", 10);
	char* payload3 = (char*)malloc(sizeof(char)* 6);
	memcpy(payload3, "UVWXYZ", 6);
	char data[40];
	PUDTPacketStorage storage;
	storage.init( 10, 10 );

	// Create a buffer with 4 slots.
	PUDTReceiveBuffer* receiveBuffer = new PUDTReceiveBuffer(&storage, 4);

	// Check that it's empty.
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	// Ack packets on the empty buffer.
	CHECK_EQUAL(0, receiveBuffer->ackPackets(2));
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	// Try to get data from the empty buffer.
	CHECK_EQUAL(0, receiveBuffer->readBuffer(data, 40));

	PUDTPacket* packet1 = storage.get();
	packet1->data = payload1;
	packet1->setLength(10);

	// Add a packet.
	CHECK_EQUAL(true, receiveBuffer->addPacket(packet1, 0));
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	PUDTPacket* packet3 = storage.get();
	packet3->data = payload3;
	packet3->setLength(6);

	// Add another packet skipping one slot.
	CHECK_EQUAL(true, receiveBuffer->addPacket(packet3, 2));
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	// Try to acknowledge both packets.
	CHECK_EQUAL(1, receiveBuffer->ackPackets(3));
	CHECK_EQUAL(1, receiveBuffer->getReadyPacketCount());

	PUDTPacket* packet2 = storage.get();
	packet2->data = payload2;
	packet2->setLength(10);

	// Now add the skipped packet.
	CHECK_EQUAL(true, receiveBuffer->addPacket(packet2, 0));
	CHECK_EQUAL(1, receiveBuffer->getReadyPacketCount());

	// Try to add the next packet again.
	CHECK_EQUAL(false, receiveBuffer->addPacket(packet3, 1));
	CHECK_EQUAL(1, receiveBuffer->getReadyPacketCount());

	// Acknowledge the packets.
	CHECK_EQUAL(2, receiveBuffer->ackPackets(3));
	CHECK_EQUAL(3, receiveBuffer->getReadyPacketCount());

	// Read part of the buffer.
	CHECK_EQUAL(9, receiveBuffer->readBuffer(data, 9));
	CHECK_ARRAY_EQUAL("ABCDEFGHI", data, 9);
	CHECK_EQUAL(3, receiveBuffer->getReadyPacketCount());

	// Try to read more than what is stored.
	CHECK_EQUAL(17, receiveBuffer->readBuffer(data, 40));
	CHECK_ARRAY_EQUAL("JKLMNOPQRSTUVWXYZ", data, 17);
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	// Store the packets again, now wrapping over to the beginning of the buffer.
	packet1 = storage.get();
	packet1->data = payload1;
	packet1->setLength(10);
	packet2 = storage.get();
	packet2->data = payload2;
	packet2->setLength(10);
	packet3 = storage.get();
	packet3->data = payload3;
	packet3->setLength(6);
	CHECK_EQUAL(true, receiveBuffer->addPacket(packet3, 2));
	CHECK_EQUAL(true, receiveBuffer->addPacket(packet2, 1));
	CHECK_EQUAL(true, receiveBuffer->addPacket(packet1, 0));
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	// Acknowledge all packets.
	CHECK_EQUAL(3, receiveBuffer->ackPackets(3));
	CHECK_EQUAL(3, receiveBuffer->getReadyPacketCount());

	// Read the buffer.
	CHECK_EQUAL(26, receiveBuffer->readBuffer(data, 40));
	CHECK_ARRAY_EQUAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ", data, 26);
	CHECK_EQUAL(0, receiveBuffer->getReadyPacketCount());

	delete receiveBuffer;
}

TEST(SendLossList)
{
	// Create a loss list with 4 slots.
	PUDTSendLossList* sendLossList = new PUDTSendLossList(4);

	// Check that it's empty.
	CHECK_EQUAL(0, sendLossList->getLossCount());

	// Try to get the first lost sequence number.
	CHECK_EQUAL(-1, sendLossList->popFirstLostSeq());

	// Try to remove from the empty list.
	sendLossList->removeUpTo(1000);
	CHECK_EQUAL(0, sendLossList->getLossCount());

	// Try to add five sequence numbers.
	CHECK_EQUAL(-1, sendLossList->insert(1,5));

	// Add four numbers at a time.
	CHECK_EQUAL(4, sendLossList->insert(1,4));

	// Check that the numbers were inserted correctly.
	CHECK_EQUAL(4, sendLossList->getLossCount());
	CHECK_EQUAL(1, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(3, sendLossList->getLossCount());
	CHECK_EQUAL(2, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(2, sendLossList->getLossCount());
	CHECK_EQUAL(3, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(1, sendLossList->getLossCount());
	CHECK_EQUAL(4, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(0, sendLossList->getLossCount());

	// Add numbers in two overlapping intervals.
	CHECK_EQUAL(3, sendLossList->insert(2,4));
	CHECK_EQUAL(1, sendLossList->insert(1,3));

	// Check that the numbers were inserted correctly.
	CHECK_EQUAL(4, sendLossList->getLossCount());
	CHECK_EQUAL(1, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(2, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(3, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(4, sendLossList->popFirstLostSeq());

	// Add numbers wrapping over the maximum.
	CHECK_EQUAL(4, sendLossList->insert(PUDTSequenceNumber::MaxSequenceNumber - 1, 1));

	// Check that the numbers were inserted correctly.
	CHECK_EQUAL(4, sendLossList->getLossCount());
	CHECK_EQUAL(PUDTSequenceNumber::MaxSequenceNumber - 1, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(PUDTSequenceNumber::MaxSequenceNumber, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(0, sendLossList->popFirstLostSeq());
	CHECK_EQUAL(1, sendLossList->popFirstLostSeq());

	// Add numbers wrapping over the maximum (again).
	CHECK_EQUAL(4, sendLossList->insert(PUDTSequenceNumber::MaxSequenceNumber - 1, 1));

	// Try to remove nonexistent numbers.
	sendLossList->removeUpTo(PUDTSequenceNumber::MaxSequenceNumber - 2);
	CHECK_EQUAL(4, sendLossList->getLossCount());

	// Remove some of the numbers.
	sendLossList->removeUpTo(0);
	CHECK_EQUAL(1, sendLossList->getLossCount());
	CHECK_EQUAL(1, sendLossList->popFirstLostSeq());

	delete sendLossList;
}

TEST(ReceiveLossList)
{
	PUDTReceiveLossList* receiveLossList = new PUDTReceiveLossList(4);

	INT32 lossArray[4];
	INT32 array1[2] = {0x80000001,2};
	INT32 array2[2] = {0x80000001,4};
	INT32 array3[3] = {1,0x80000003,4};
	INT32 array4[2] = {0x80000003,4};
	INT32 array5[2] = {0x80000000 | (PUDTSequenceNumber::MaxSequenceNumber - 1),1};

	// Check that it's empty.
	CHECK_EQUAL(0, receiveLossList->getLossArray(lossArray, 4));

	// Try to get the first lost sequence number.
	CHECK_EQUAL(-1, receiveLossList->getFirstLostSeq());

	// Try to remove from the empty list.
	CHECK_EQUAL(false, receiveLossList->remove(1000));

	// Try to add five sequence numbers.
	CHECK_EQUAL(false, receiveLossList->insert(1,5));

	// Add two numbers at a time.
	CHECK_EQUAL(true, receiveLossList->insert(1,2));

	// Check that the numbers were inserted correctly.
	CHECK_EQUAL(2, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array1, lossArray, 2);

	// Try to add 2 more numbers, but overflowing the list.
	CHECK_EQUAL(false, receiveLossList->insert(4,5));
	CHECK_EQUAL(2, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array1, lossArray, 2);

	// Add two more numbers.
	CHECK_EQUAL(true, receiveLossList->insert(3,4));
	CHECK_EQUAL(2, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array2, lossArray, 2);

	// Remove a number from the middle.
	CHECK_EQUAL(true, receiveLossList->remove(2));
	CHECK_EQUAL(3, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array3, lossArray, 3);

	// Check that the first number is correct.
	CHECK_EQUAL(1, receiveLossList->getFirstLostSeq());

	// Remove the first number.
	CHECK_EQUAL(true, receiveLossList->remove(receiveLossList->getFirstLostSeq()));
	CHECK_EQUAL(3, receiveLossList->getFirstLostSeq());
	CHECK_EQUAL(2, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array4, lossArray, 2);

	// Remove the remaining numbers.
	CHECK_EQUAL(true, receiveLossList->remove(3));
	CHECK_EQUAL(true, receiveLossList->remove(4));
	CHECK_EQUAL(0, receiveLossList->getLossArray(lossArray, 4));

	// Add numbers wrapping over the maximum.
	CHECK_EQUAL(true, receiveLossList->insert(PUDTSequenceNumber::MaxSequenceNumber - 1, 1));

	// Check that the numbers were inserted correctly.
	CHECK_EQUAL(2, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array5, lossArray, 2);

	// Try to remove nonexistent numbers.
	CHECK_EQUAL(false, receiveLossList->remove(PUDTSequenceNumber::MaxSequenceNumber - 2));
	CHECK_EQUAL(2, receiveLossList->getLossArray(lossArray, 4));
	CHECK_ARRAY_EQUAL(array5, lossArray, 2);

	delete receiveLossList;
}

int main(int argc, char** argv)
{
	//return UnitTest::RunAllTests();
	int result = UnitTest::RunAllTests();

	return result;
}