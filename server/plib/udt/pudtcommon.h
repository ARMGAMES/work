#ifndef udtcommon_h_included
#define udtcommon_h_included

#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
////////////////////////////////////////////////////////////////////////////////

// UDT Sequence Number 0 - (2^31 - 1)
class PUDTSequenceNumber
{
public:
	/// Sequence number comparison.
	/// Compare two sequence numbers, taking into consideration a possible wrap-over.
	/// @param[in] seq1 First sequence number to compare.
	/// @param[in] seq2 Second sequence number to compare.
	/// @return A negative value if the first sequence number is less than the second, zero if they are the same, and a positive number if the first sequence number is greater than the second.
	inline static INT32 seqCmp(INT32 seq1, INT32 seq2)
	{return (abs(seq1 - seq2) < SequenceNumberThreshold) ? (seq1 - seq2) : (seq2 - seq1);}

	/// Sequence number length.
	/// Determines the length between the two given sequence numbers, including both.
	/// @param[in] seq1 First sequence number.
	/// @param[in] seq2 Last sequence number.
	/// @return The length of the sequence between both numbers, including themselves.
	inline static INT32 seqLength(INT32 seq1, INT32 seq2)
	{return (seq1 <= seq2) ? (seq2 - seq1 + 1) : (seq2 - seq1 + MaxSequenceNumber + 2);}

	/// Sequence offset.
	/// Determines the offset from the first given sequence number to the second.
	/// @param[in] seq1 First sequence number.
	/// @param[in] seq2 Second sequence number.
	/// @return The offset from the first number to the second.
	inline static INT32 seqOffset(INT32 seq1, INT32 seq2)
	{
		if (abs(seq1 - seq2) < SequenceNumberThreshold)
			return seq2 - seq1;

		if (seq1 < seq2)
			return seq2 - seq1 - MaxSequenceNumber - 1;

		return seq2 - seq1 + MaxSequenceNumber + 1;
	}

	/// Increment a sequence number.
	/// Increments the given sequence number by the given amount.
	/// @param[in] seqNumber Sequence number to increment.
	/// @param[in] increment Amount to increment sequence number by.
	/// @return The incremented number, taking into consideration a possible wrap-around.
	inline static INT32 incrementSeqNum(INT32 seqNumber, INT32 increment = 1)
	{return (MaxSequenceNumber - seqNumber >= increment) ? seqNumber + increment : seqNumber - MaxSequenceNumber + increment - 1;}

	/// Decrement a sequence number.
	/// Decrements the given sequence number by 1.
	/// @param[in] seqNumber Sequence number to decrement.
	/// @return The decremented number, taking into consideration a possible wrap-around.
	inline static INT32 decrementSeqNum(INT32 seq)
	{return (seq == 0) ? MaxSequenceNumber : seq - 1;}

public:
	static const INT32 SequenceNumberThreshold = 0x3FFFFFFF;
	static const INT32 MaxSequenceNumber = 0x7FFFFFFF;
};


class PUDTAckNo
{
public:
   inline static INT32 incack(INT32 ackno)
   {return (ackno == maxAckSeqNo) ? 0 : ackno + 1;}

public:
   static const INT32 maxAckSeqNo = 0x7FFFFFFF;         // maximum ACK sub-sequence number used in UDT
};



#endif //udtcommon_h_included

#endif //UDT_DEVELOPMENT