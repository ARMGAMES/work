#ifndef pudtwindow_h_included
#define pudtwindow_h_included

#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT


class PUDTACKWindow
{
public:
	PUDTACKWindow(int size = 1024);
	~PUDTACKWindow();

		// Functionality:
		//    Write an ACK record into the window.
		// Parameters:
		//    0) [in] seq: ACK seq. no.
		//    1) [in] ack: DATA ACK no.
		// Returned value:
		//    None.

	void store(INT32 seq, INT32 ack, UINT64 t);

		// Functionality:
		//    Search the ACK-2 "seq" in the window, find out the DATA "ack" and caluclate RTT .
		// Parameters:
		//    0) [in] seq: ACK-2 seq. no.
		//    1) [out] ack: the DATA ACK no. that matches the ACK-2 no.
		// Returned value:
		//    RTT.

	int acknowledge(INT32 seq, INT32& ack, UINT64 t);

private:
	INT32* ACKSeqNo;       // Seq. No. for the ACK packet
	INT32* dataSeqNo;            // Data Seq. No. carried by the ACK packet
	UINT64* timeStamp;      // The timestamp when the ACK was sent

	int size;
	int head;
	int tail;

private:
	PUDTACKWindow(const PUDTACKWindow&);
	PUDTACKWindow& operator=(const PUDTACKWindow&);
};




class PUDTTimeWindow
{
public:
	PUDTTimeWindow(int asize = 16, int psize = 16);
	~PUDTTimeWindow();

		// Functionality:
		//    Calculate the packes arrival speed.
		// Parameters:
		//    None.
		// Returned value:
		//    Packet arrival speed (packets per second).

	int getPktRcvSpeed() const;

		// Functionality:
		//    Estimate the bandwidth.
		// Parameters:
		//    None.
		// Returned value:
		//    Estimated bandwidth (packets per second).

	int getBandwidth() const;

  
		// Functionality:
		//    Record time information of an arrived packet.
		// Parameters:
		//    None.
		// Returned value:
		//    None.

	void onPktArrival(UINT64 t);

		// Functionality:
		//    Record the arrival time of the first probing packet.
		// Parameters:
		//    None.
		// Returned value:
		//    None.

	void probe1Arrival(INT32 pNo, UINT64 t);

		// Functionality:
		//    Record the arrival time of the second probing packet and the interval between packet pairs.
		// Parameters:
		//    None.
		// Returned value:
		//    None.

	void probe2Arrival(INT32 pNo, UINT64 t);

private:
	int packetWindowSize;    
	int* packetWindow;        
	int* packetWindowCopy;
	int packetWindowPos;      

	int probeWindowSize;       
	int* probeWindow;         
	int* probeWindowCopy;
	int probeWindowPos;       


	UINT64 lastArrivelTime;
	UINT64 probeArrivalTime;        // arrival time of the first probing packet
	INT32  probe1Packet;
	bool arrivalWindowFull;
	bool probeWindowFull;

private:
	PUDTTimeWindow(const PUDTTimeWindow&);
	PUDTTimeWindow &operator=(const PUDTTimeWindow&);
};
#endif
#endif