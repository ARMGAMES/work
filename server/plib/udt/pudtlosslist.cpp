#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pudtlosslist.h"
#include "pudtcommon.h"

////////////////////////////////////////////////////////////////////////////////

PUDTSendLossList::PUDTSendLossList(int size) :
	lossList(NULL),
	firstIndex(0),
    lossCount(0),
    capacity(size),
	lastInsertionIndex(0)
{
	lossList = new Interval[capacity];

	// -1 means there is no data in the node
	for (INT32 i = 0; i < capacity; ++ i)
	{
		lossList[i].start = -1;
		lossList[i].end = -1;
	}
}

PUDTSendLossList::~PUDTSendLossList()
{
	delete [] lossList;
}

INT32 PUDTSendLossList::insert(INT32 start, INT32 end)
{
	PLock lock(cs);

	if (0 == lossCount)
	{
		INT32 sequenceLength = PUDTSequenceNumber::seqLength(start, end);
		// If the sequence length is greater than the loss list capacity, we cannot continue with the insert.
		if( sequenceLength > capacity)
		{
			return -1;
		}

		// insert data into an empty list
		firstIndex = 0;
		lossList[firstIndex].start = start;
		if (end != start)
			lossList[firstIndex].end = end;

		lossList[firstIndex].nextIndex = -1;
		lastInsertionIndex = firstIndex;

		lossCount = sequenceLength;

		return lossCount;
	}

	// otherwise find the position where the data can be inserted
	INT32 origlen = lossCount;
	INT32 offset = PUDTSequenceNumber::seqOffset(lossList[firstIndex].start, start);
	INT32 size = PUDTSequenceNumber::seqLength(start, end);

	// If the total spread is greater than the loss list capacity, we cannot continue with the insert.
	if( offset + size > capacity)
	{
		return -1;
	}

	INT32 loc = (firstIndex + offset + capacity) % capacity;

	if (offset < 0)
	{
		// Insert data prior to the head pointer

		lossList[loc].start = start;
		if (end != start)
			lossList[loc].end = end;

		// new node becomes head
		lossList[loc].nextIndex = firstIndex;
		firstIndex = loc;
		lastInsertionIndex = loc;

		lossCount += size;
	}
	else if (offset > 0)
	{
		if (start == lossList[loc].start)
		{
			lastInsertionIndex = loc;

			// first seqno is equivalent, compare the second
			if (-1 == lossList[loc].end)
			{
				if (end != start)
				{
					lossCount += size - 1;
					lossList[loc].end = end;
				}
			}
			else if (PUDTSequenceNumber::seqCmp(end, lossList[loc].end) > 0)
			{
				// new seq pair is longer than old pair, e.g., insert [3, 7] to [3, 5], becomes [3, 7]
				lossCount += PUDTSequenceNumber::seqLength(lossList[loc].end, end) - 1;
				lossList[loc].end = end;
			}
			else
				// Do nothing if it is already there
				return 0;
		}
		else
		{
			// searching the prior node
			INT32 i;
			if ((-1 != lastInsertionIndex) && (PUDTSequenceNumber::seqCmp(lossList[lastInsertionIndex].start, start) < 0))
				i = lastInsertionIndex;
			else
				i = firstIndex;

			while ((-1 != lossList[i].nextIndex) && (PUDTSequenceNumber::seqCmp(lossList[lossList[i].nextIndex].start, start) < 0))
				i = lossList[i].nextIndex;

			if ((-1 == lossList[i].end) || (PUDTSequenceNumber::seqCmp(lossList[i].end, start) < 0))
			{
				lastInsertionIndex = loc;

				// no overlap, create new node
				lossList[loc].start = start;
				if (end != start)
					lossList[loc].end = end;

				lossList[loc].nextIndex = lossList[i].nextIndex;
				lossList[i].nextIndex = loc;

				lossCount += size;
			}
			else
			{
				lastInsertionIndex = i;

				// overlap, coalesce with prior node, insert(3, 7) to [2, 5], ... becomes [2, 7]
				if (PUDTSequenceNumber::seqCmp(lossList[i].end, end) < 0)
				{
					lossCount += PUDTSequenceNumber::seqLength(lossList[i].end, end) - 1;
					lossList[i].end = end;

					loc = i;
				}
				else
					return 0;
			}
		}
	}
	else
	{
		lastInsertionIndex = firstIndex;

		// insert to head node
		if (end != start)
		{
			if (-1 == lossList[loc].end)
			{
				lossCount += size - 1;
				lossList[loc].end = end;
			}
			else if (PUDTSequenceNumber::seqCmp(end, lossList[loc].end) > 0)
			{
				lossCount += PUDTSequenceNumber::seqLength(lossList[loc].end, end) - 1;
				lossList[loc].end = end;
			}
			else 
				return 0;
		}
		else
			return 0;
	}

	// coalesce with next node. E.g., [3, 7], ..., [6, 9] becomes [3, 9] 
	while ((-1 != lossList[loc].nextIndex) && (-1 != lossList[loc].end))
	{
		INT32 i = lossList[loc].nextIndex;

		if (PUDTSequenceNumber::seqCmp(lossList[i].start, PUDTSequenceNumber::incrementSeqNum(lossList[loc].end)) <= 0)
		{
			// coalesce if there is overlap
			if (-1 != lossList[i].end)
			{
				if (PUDTSequenceNumber::seqCmp(lossList[i].end, lossList[loc].end) > 0)
				{
					if (PUDTSequenceNumber::seqCmp(lossList[loc].end, lossList[i].start) >= 0)
						lossCount -= PUDTSequenceNumber::seqLength(lossList[i].start, lossList[loc].end);

					lossList[loc].end = lossList[i].end;
				}
				else
					lossCount -= PUDTSequenceNumber::seqLength(lossList[i].start, lossList[i].end);
			}
			else
			{
				if (lossList[i].start == PUDTSequenceNumber::incrementSeqNum(lossList[loc].end))
					lossList[loc].end = lossList[i].start;
				else
					lossCount --;
			}

			lossList[i].start = -1;
			lossList[i].end = -1;
			lossList[loc].nextIndex = lossList[i].nextIndex;
		}
		else
			break;
	}

	return lossCount - origlen;
}

bool PUDTSendLossList::removeUpTo(INT32 seqno)
{
	PLock lock(cs);

	if (0 == lossCount)
		return false;
	INT32 lossCountBak = lossCount;

	// Remove all from the head pointer to a node with a larger seq. no. or the list is empty
	INT32 offset = PUDTSequenceNumber::seqOffset(lossList[firstIndex].start, seqno);
	INT32 loc = (firstIndex + offset + capacity) % capacity;

	if (0 == offset)
	{
		// It is the head. Remove the head and point to the next node
		loc = (loc + 1) % capacity;

		if (-1 == lossList[firstIndex].end)
			loc = lossList[firstIndex].nextIndex;
		else
		{
			lossList[loc].start = PUDTSequenceNumber::incrementSeqNum(seqno);
			if (PUDTSequenceNumber::seqCmp(lossList[firstIndex].end, lossList[loc].start) > 0)
				lossList[loc].end = lossList[firstIndex].end;

			lossList[firstIndex].end = -1;

			lossList[loc].nextIndex = lossList[firstIndex].nextIndex;
		}

		lossList[firstIndex].start = -1;

		if (lastInsertionIndex == firstIndex)
			lastInsertionIndex = -1;

		firstIndex = loc;

		--lossCount;
	}
	else if (offset > 0)
	{
		INT32 h = firstIndex;

		if (seqno == lossList[loc].start)
		{
			// target node is not empty, remove part/all of the seqno in the node.
			INT32 temp = loc;
			loc = (loc + 1) % capacity;

			if (-1 == lossList[temp].end)
				firstIndex = lossList[temp].nextIndex;
			else
			{
				// remove part, e.g., [3, 7] becomes [], [4, 7] after remove(3)
				lossList[loc].start = PUDTSequenceNumber::incrementSeqNum(seqno);
				if (PUDTSequenceNumber::seqCmp(lossList[temp].end, lossList[loc].start) > 0)
					lossList[loc].end = lossList[temp].end;
				firstIndex = loc;
				lossList[loc].nextIndex = lossList[temp].nextIndex;
				lossList[temp].nextIndex = loc;
				lossList[temp].end = -1;
			}
		}
		else
		{
			// target node is empty, check prior node
			INT32 i = firstIndex;
			while ((-1 != lossList[i].nextIndex) && (PUDTSequenceNumber::seqCmp(lossList[lossList[i].nextIndex].start, seqno) < 0))
				i = lossList[i].nextIndex;

			loc = (loc + 1) % capacity;

			if (-1 == lossList[i].end)
				firstIndex = lossList[i].nextIndex;
			else if (PUDTSequenceNumber::seqCmp(lossList[i].end, seqno) > 0)
			{
				// remove part/all seqno in the prior node
				lossList[loc].start = PUDTSequenceNumber::incrementSeqNum(seqno);
				if (PUDTSequenceNumber::seqCmp(lossList[i].end, lossList[loc].start) > 0)
					lossList[loc].end = lossList[i].end;

				lossList[i].end = seqno;

				lossList[loc].nextIndex = lossList[i].nextIndex;
				lossList[i].nextIndex = loc;

				firstIndex = loc;
			}
			else
				firstIndex = lossList[i].nextIndex;
		}

		// Remove all nodes prior to the new head
		while (h != firstIndex)
		{
			if (lossList[h].end != -1)
			{
				lossCount -= PUDTSequenceNumber::seqLength(lossList[h].start, lossList[h].end);
				lossList[h].end = -1;
			}
			else
				lossCount --;

			lossList[h].start = -1;

			if (lastInsertionIndex == h)
				lastInsertionIndex = -1;

			h = lossList[h].nextIndex;
		}
	}
	return lossCount != lossCountBak;
}

INT32 PUDTSendLossList::popFirstLostSeq()
{
	if (0 == lossCount)
		return -1;

	PLock lock(cs);

	if (0 == lossCount)
		return -1;

	if (lastInsertionIndex == firstIndex)
		lastInsertionIndex = -1;

	// return the first loss seq. no.
	INT32 seqno = lossList[firstIndex].start;

	// head moves to the next node
	if (-1 == lossList[firstIndex].end)
	{
		//[3, -1] becomes [], and head moves to next node in the list
		lossList[firstIndex].start = -1;
		firstIndex = lossList[firstIndex].nextIndex;
	}
	else
	{
		// shift to next node, e.g., [3, 7] becomes [], [4, 7]
		INT32 loc = (firstIndex + 1) % capacity;

		lossList[loc].start = PUDTSequenceNumber::incrementSeqNum(seqno);
		if (PUDTSequenceNumber::seqCmp(lossList[firstIndex].end, lossList[loc].start) > 0)
			lossList[loc].end = lossList[firstIndex].end;

		lossList[firstIndex].start = -1;
		lossList[firstIndex].end = -1;

		lossList[loc].nextIndex = lossList[firstIndex].nextIndex;
		firstIndex = loc;
	}

	--lossCount;

	return seqno;
}

INT32 PUDTSendLossList::getLossCount()
{
	return lossCount;
}

////////////////////////////////////////////////////////////////////////////////

PUDTReceiveLossList::PUDTReceiveLossList(int size):
	lossList(NULL),
	firstIndex(-1),
	lastIndex(-1),
	lossCount(0),
	capacity(size)
{
	lossList = new Interval [capacity];

	// -1 means there is no data in the node
	for (INT32 i = 0; i < size; ++ i)
	{
		lossList[i].start = -1;
		lossList[i].end = -1;
	}
}

PUDTReceiveLossList::~PUDTReceiveLossList()
{
	delete [] lossList;
}

bool PUDTReceiveLossList::insert(INT32 start, INT32 end)
{
	// Data to be inserted must be larger than all those in the list
	// guaranteed by the UDT receiver

	if (0 == lossCount)
	{
		INT32 sequenceLength = PUDTSequenceNumber::seqLength(start, end);
		// If the sequence length is greater than the loss list capacity, we cannot continue with the insert.
		if( sequenceLength > capacity)
		{
			return false;
		}

		// insert data into an empty list
		firstIndex = 0;
		lastIndex = 0;
		lossList[firstIndex].start = start;
		if (end != start)
			lossList[firstIndex].end = end;

		lossList[firstIndex].nextIndex = -1;
		lossList[firstIndex].previousIndex = -1;
		lossCount = sequenceLength; 

		return true;
	}

	// otherwise searching for the position where the node should be
	INT32 offset = PUDTSequenceNumber::seqOffset(lossList[firstIndex].start, start);
	INT32 loc = (firstIndex + offset) % capacity;
	INT32 size = PUDTSequenceNumber::seqLength(start, end);

	// If the resulting spread is greater than the loss list capacity, we cannot continue with the insert.
	if( offset + size > capacity)
	{
		return false;
	}

	if ((-1 != lossList[lastIndex].end) && (PUDTSequenceNumber::incrementSeqNum(lossList[lastIndex].end) == start))
	{
		// coalesce with prior node, e.g., [2, 5], [6, 7] becomes [2, 7]
		loc = lastIndex;
		lossList[loc].end = end;
	}
	else
	{
		// create new node
		lossList[loc].start = start;

		if (end != start)
			lossList[loc].end = end;

		lossList[lastIndex].nextIndex = loc;
		lossList[loc].previousIndex = lastIndex;
		lossList[loc].nextIndex = -1;
		lastIndex = loc;
	}

	lossCount += size;

	return true;
}

bool PUDTReceiveLossList::remove(INT32 seqno)
{
	if (0 == lossCount)
		return false; 

	// locate the position of "seqno" in the list
	INT32 offset = PUDTSequenceNumber::seqOffset(lossList[firstIndex].start, seqno);
	if (offset < 0)
		return false;

	INT32 loc = (firstIndex + offset) % capacity;

	if (seqno == lossList[loc].start)
	{
		// This is a seq. no. that starts the loss sequence

		if (-1 == lossList[loc].end)
		{
			// there is only 1 loss in the sequence, delete it from the node
			if (firstIndex == loc)
			{
				firstIndex = lossList[firstIndex].nextIndex;
				if (-1 != firstIndex)
					lossList[firstIndex].previousIndex = -1;
			}
			else
			{
				lossList[lossList[loc].previousIndex].nextIndex = lossList[loc].nextIndex;
				if (-1 != lossList[loc].nextIndex)
					lossList[lossList[loc].nextIndex].previousIndex = lossList[loc].previousIndex;
				else
					lastIndex = lossList[loc].previousIndex;
			}

			lossList[loc].start = -1;
		}
		else
		{
			// there are more than 1 loss in the sequence
			// move the node to the next and update the starter as the next loss inSeqNo(seqno)

			// find next node
			INT32 i = (loc + 1) % capacity;

			// remove the "seqno" and change the starter as next seq. no.
			lossList[i].start = PUDTSequenceNumber::incrementSeqNum(lossList[loc].start);

			// process the sequence end
			if (PUDTSequenceNumber::seqCmp(lossList[loc].end, PUDTSequenceNumber::incrementSeqNum(lossList[loc].start)) > 0)
				lossList[i].end = lossList[loc].end;

			// remove the current node
			lossList[loc].start = -1;
			lossList[loc].end = -1;

			// update list pointer
			lossList[i].nextIndex = lossList[loc].nextIndex;
			lossList[i].previousIndex = lossList[loc].previousIndex;

			if (firstIndex == loc)
				firstIndex = i;
			else
				lossList[lossList[i].previousIndex].nextIndex = i;

			if (lastIndex == loc)
				lastIndex = i;
			else
				lossList[lossList[i].nextIndex].previousIndex = i;
		}

		--lossCount;

		return true;
	}

	// There is no loss sequence in the current position
	// the "seqno" may be contained in a previous node

	// searching previous node
	INT32 i = (loc - 1 + capacity) % capacity;
	while (-1 == lossList[i].start)
		i = (i - 1 + capacity) % capacity;

	// not contained in this node, return
	if ((-1 == lossList[i].end) || (PUDTSequenceNumber::seqCmp(seqno, lossList[i].end) > 0))
		return false;

	if (seqno == lossList[i].end)
	{
		// it is the sequence end

		if (seqno == PUDTSequenceNumber::incrementSeqNum(lossList[i].start))
			lossList[i].end = -1;
		else
			lossList[i].end = PUDTSequenceNumber::decrementSeqNum(seqno);
	}
	else
	{
		// split the sequence

		// construct the second sequence from CSeqNo::incseq(seqno) to the original sequence end
		// located at "loc + 1"
		loc = (loc + 1) % capacity;

		lossList[loc].start = PUDTSequenceNumber::incrementSeqNum(seqno);
		if (PUDTSequenceNumber::seqCmp(lossList[i].end, lossList[loc].start) > 0)      
			lossList[loc].end = lossList[i].end;

		// the first (original) sequence is between the original sequence start to CSeqNo::decseq(seqno)
		if (seqno == PUDTSequenceNumber::incrementSeqNum(lossList[i].start))
			lossList[i].end = -1;
		else
			lossList[i].end = PUDTSequenceNumber::decrementSeqNum(seqno);

		// update the list pointer
		lossList[loc].nextIndex = lossList[i].nextIndex;
		lossList[i].nextIndex = loc;
		lossList[loc].previousIndex = i;

		if (lastIndex == i)
			lastIndex = loc;
		else
			lossList[lossList[loc].nextIndex].previousIndex = loc;
	}

	--lossCount;

	return true;
}

INT32 PUDTReceiveLossList::getFirstLostSeq() const
{
	if (0 == lossCount)
		return -1;

	return lossList[firstIndex].start;
}

INT32 PUDTReceiveLossList::getLossArray(INT32* array, INT32 limit)
{
	INT32 len = 0;

	INT32 i = firstIndex;

	while ((len < limit - 1) && (-1 != i))
	{
		array[len] = lossList[i].start;
		if (-1 != lossList[i].end)
		{
			// there are more than 1 loss in the sequence
			array[len] |= 0x80000000;
			++ len;
			array[len] = lossList[i].end;
		}

		++ len;

		i = lossList[i].nextIndex;
	}

	return len;
}

INT32 PUDTReceiveLossList::getLossCount()
{
	return lossCount;
}

#endif //UDT_DEVELOPMENT