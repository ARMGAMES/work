#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pudtwindow.h"
#include "pudttimer.h"




PUDTACKWindow::PUDTACKWindow(int size_)
{
	size = size_;
	ACKSeqNo = new INT32[size];
	dataSeqNo = new INT32[size];
	timeStamp = new UINT64[size];
	head=0;
	tail=0;
	dataSeqNo[0] = -1;
}

PUDTACKWindow::~PUDTACKWindow()
{
	delete [] ACKSeqNo;
	delete [] dataSeqNo;
	delete [] timeStamp;
}

void PUDTACKWindow::store(INT32 seq, INT32 ack, UINT64 t)
{
	ACKSeqNo[head] = seq;
	dataSeqNo[head] = ack;
	timeStamp[head] = t;

	head = (head + 1) % size;
	if (head == tail)
		tail = (tail + 1) % size;
}

int PUDTACKWindow::acknowledge(INT32 seq, INT32& ack,  UINT64 t)
{
   if (head >= tail)
   {
      // Head has not exceeded the physical boundary of the window

      for (int i = tail; i < head; i++)
      {
         // looking for indentical ACK Seq. No.
         if (seq == ACKSeqNo[i])
         {
            // return the Data ACK it carried
            ack = dataSeqNo[i];

            // calculate RTT
            int rtt = int(t - timeStamp[i]);

            if (i + 1 == head)
            {
               tail = head = 0;
               dataSeqNo[0] = -1;
            }
            else
               tail = (i + 1) % size;

            return rtt;
         }
      }

      // Bad input, the ACK node has been overwritten
      return -1;
   }

   // Head has exceeded the physical window boundary, so it is behind tail
   for (int j = tail, n = head + size; j < n; j++)
   {
      // looking for indentical ACK seq. no.
      if (seq == ACKSeqNo[j % size])
      {
         // return Data ACK
         j %= size;
         ack = dataSeqNo[j];

         // calculate RTT
         int rtt = int(t - timeStamp[j]);

         if (j == head)
         {
            tail = head = 0;
            dataSeqNo[0] = -1;
         }
         else
            tail = (j + 1) % size;

         return rtt;
      }
   }

   // bad input, the ACK node has been overwritten
   return -1;
}



PUDTTimeWindow::PUDTTimeWindow(int asize, int psize)
{
	
	packetWindowSize = asize;
	probeWindowSize = psize;
	probeWindowPos = 0;
	packetWindowPos = 0;
	arrivalWindowFull = false;
	probeWindowFull = false;
	lastArrivelTime = PUDTTimerQueue::getTimestamp();
	probeArrivalTime = 0;
	probe1Packet = 0;
	packetWindow = new int[packetWindowSize];
	packetWindowCopy = new int[packetWindowSize];
	probeWindow = new int[probeWindowSize];
	probeWindowCopy = new int[probeWindowSize];
}

PUDTTimeWindow::~PUDTTimeWindow()
{
	delete [] packetWindow;
    delete [] packetWindowCopy;
    delete [] probeWindow;
    delete [] probeWindowCopy;
}


int PUDTTimeWindow::getPktRcvSpeed() const
{
   // get median value, but cannot change the original value order in the window
	int pos;
	if (arrivalWindowFull)
		pos = packetWindowSize - 1;
	else
		pos = packetWindowPos;
	std::copy(packetWindow, packetWindow + pos, packetWindowCopy);
	std::nth_element(packetWindowCopy, packetWindowCopy + ((pos + 1) / 2), packetWindowCopy + pos);
	int median = packetWindowCopy[(pos + 1) / 2];

	int count = 0;
	int sum = 0;
	int upper = median << 3;
	int lower = median >> 3;

	// median filtering
	int* p = packetWindow;
	for (int i = 0; i <= pos; i++, p++)
	{
		if ((*p < upper) && (*p > lower))
		{
			count++;
			sum += *p;
		}
	}

	// claculate speed, or return 0 if not enough valid value
	if (count > (pos + 1) / 2 )
		return (int)ceil(1000000.0 / ((double)sum / (double)count));
	else
		return 0;
}

int PUDTTimeWindow::getBandwidth() const
{
   // get median value, but cannot change the original value order in the window
	int pos;
	if (probeWindowFull)
		pos = probeWindowSize - 1;
	else
		pos = probeWindowPos;
	std::copy(probeWindow, probeWindow + pos, probeWindowCopy);
	std::nth_element(probeWindowCopy, probeWindowCopy + ((pos + 1) / 2), probeWindowCopy + pos);
	int median = probeWindowCopy[(pos + 1) / 2];

	int count = 0;
	int sum = 0;
	int upper = median << 3;
	int lower = median >> 3;

	// median filtering
	int* p = probeWindow;
	for (int i = 0; i <= pos; i++, p++)
	{
		if ((*p < upper) && (*p > lower))
		{
			count++;
			sum += *p;
		}
	}

    return (int)ceil(1000000.0 / ((double)sum / (double)count));
}


void PUDTTimeWindow::onPktArrival(UINT64 t)
{
	// record the packet interval between the current and the last one
	*(packetWindow + packetWindowPos) = int(t - lastArrivelTime);

	if (++packetWindowPos == packetWindowSize)
		packetWindowPos = 0;

	// remember last packet arrival time
	lastArrivelTime = t;
}

void PUDTTimeWindow::probe1Arrival(INT32 pNo, UINT64 t)
{
	probeArrivalTime = t;
	probe1Packet = pNo;
}

void PUDTTimeWindow::probe2Arrival(INT32 pNo, UINT64 t)
{
	if ((pNo - probe1Packet) != 1)
		return;
	// record the probing packets interval
	*(probeWindow + probeWindowPos) = int(t - probeArrivalTime);
	// the window is logically circular
	if (++probeWindowPos == probeWindowSize)
		probeWindowPos = 0;
}
#endif