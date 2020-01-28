#ifndef udtlosslist_h_included
#define udtlosslist_h_included
#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT

#include "ppthread.h"

////////////////////////////////////////////////////////////////////////////////

class PUDTSendLossList
{
private:
	/// Critical section to control access to the list.
	PCriticalSection cs;

	struct Interval
	{
		INT32 start;
		INT32 end;
		INT32 nextIndex;
	};

	/// Array of sequence number intervals.
	Interval* lossList;

	/// Index of first loss interval.
	INT32 firstIndex;

	/// Number of sequence numbers in loss list.
	INT32 lossCount;

	/// Total capacity of the loss list in number of intervals.
	INT32 capacity;

	/// Index of the last insertion.
	INT32 lastInsertionIndex;

private:
	PUDTSendLossList(const PUDTSendLossList&);
	PUDTSendLossList& operator=(const PUDTSendLossList&);

public:
	PUDTSendLossList(int size = 1024);
	~PUDTSendLossList();

	/// Insert a sequence number interval.
	/// Inserts a new interval in the loss list, starting with the first given sequence number, and ending with the second.
	/// If both values are equal, only one sequence number is added.
	/// @param[in] start First sequence number in the new interval.
	/// @param[in] end Last sequence number in the interval.
	/// @return The number of values that were not previously in the list, or -1 if insertion was not possible.
	INT32 insert(INT32 start, INT32 end);

	/// Remove all sequence numbers from the start to the given one.
	/// @param[in] seqno Sequence number to be the last to remove.
	bool removeUpTo(INT32 seqno);

	/// Get the count of elements in the list.
	/// @return loss list count.
	INT32 getLossCount();

	/// Get fist lost sequence number.
	/// Gets the first sequence number in the loss list and removes it from the list.
	/// @return The first sequence number in the loss list, or -1 if the list is empty.
	INT32 popFirstLostSeq();
};

////////////////////////////////////////////////////////////////////////////////

class PUDTReceiveLossList
{
private:
	struct Interval
	{
		INT32 start;
		INT32 end;
		INT32 nextIndex;
		INT32 previousIndex;
	};

	/// Array of sequence number intervals.
	Interval* lossList;

	/// Index of first loss interval.
	INT32 firstIndex;

	/// Index of last loss interval.
	INT32 lastIndex;

	/// Number of sequence numbers in loss list.
	INT32 lossCount;

	/// Total capacity of the loss list in number of intervals.
	INT32 capacity;

private:
	PUDTReceiveLossList(const PUDTReceiveLossList&);
	PUDTReceiveLossList& operator=(const PUDTReceiveLossList&);

public:
	PUDTReceiveLossList(int size = 1024);
	~PUDTReceiveLossList();

	/// Insert a sequence number interval.
	/// Inserts a new interval in the loss list, starting with the first given sequence number, and ending with the second.
	/// If both values are equal, only one sequence number is added.
	/// @param[in] start First sequence number in the new interval.
	/// @param[in] end Last sequence number in the interval.
	/// @return True if inserted, false if no space available.
	/// @note This list expects values to be strictly larger than the currently stored values, due to the workings of UDT protocol. Enforcing this is NOT
	///       the responsibility of this object. The caller must ensure this constraint.
	bool insert(INT32 start, INT32 end);

	/// Remove a given sequence number from the loss list.
	/// @param[in] seqno The sequence number to remove.
	/// @return True if the number was removed, false if there was no such sequence number in the list.
	bool remove(INT32 seqno);

	/// Get the first lost sequence number.
	/// @return First sequence number, or -1 if the list is empty.
	INT32 getFirstLostSeq() const;

	/// Fill an array with the loss list for a NAK packet.
	/// @param[out] array The array to be filled and sent.
	/// @param[in] limit The maximum length of the array.
	/// @return The length of the resulting array.
	INT32 getLossArray(INT32* array, INT32 limit);
	/// Get the count of elements in the list.
	/// @return loss list count.
	INT32 getLossCount();

};

#endif //udtlosslist_h_included

#endif //UDT_DEVELOPMENT