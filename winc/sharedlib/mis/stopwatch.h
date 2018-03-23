
#ifndef STOP_WATCH_H
#define STOP_WATCH_H

#include <Windows.h>

namespace winlib
{
	class CStopwatch 
	{
	public:
		CStopwatch() { QueryPerformanceFrequency(&m_liPerfFreq); Start(); }
		void Start() { QueryPerformanceCounter(&m_liPerfStart); }

		// Returns # of milliseconds 
		__int64 NowInMilliseconds() const 
		{ 
			LARGE_INTEGER liPerfNow;
			QueryPerformanceCounter(&liPerfNow);
			return(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000)
				/ m_liPerfFreq.QuadPart);
		}

		// Returns # of microseconds
		__int64 NowInMicroseconds() const 
		{ 
			LARGE_INTEGER liPerfNow;
			QueryPerformanceCounter(&liPerfNow);
			return(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000000)
				/ m_liPerfFreq.QuadPart);
		}
	private:
		LARGE_INTEGER m_liPerfFreq; // Counts per second
		LARGE_INTEGER m_liPerfStart; // Starting count
	};

}

#endif