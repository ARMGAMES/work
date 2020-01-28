#ifndef pptime_h_included
#define pptime_h_included

#include "ppincludebase.h"
#include "pperror.h"


struct PTime
{
public:
	int sec;//similar to time( 0 );
	int msec;

public:
	PTime() : sec(0), msec(0) {}

	static PTime now()
	{
		PTime ret;
		ret.sec = LONGSYSTEMTICKS( &ret.msec );
		return ret;
	}

	void add( int msec_ )
	{
		msec += msec_;
		sec += msec / 1000; // signed division is rounded toward zero: -1999/1000 = -1
		msec %= 1000; // within +/- 999

		if( msec < 0 )
		{
			sec--;
			msec += 1000;
		}

		PASSERT5( msec >= 0 );
		PASSERT5( msec < 1000 );
	}

	int diff( const PTime& other ) const//returns difference in milliseconds
	{
		PASSERT5( sec - other.sec < INT_MAX / 1000 );
		return msec - other.msec + ( sec - other.sec ) * 1000;
	}

	int compare( const PTime& other ) const
	{
		int ret = sec - other.sec;
		if( ret )
			return ret;
		return msec - other.msec;
	}
};


time_t plib_time(time_t * sec);
UINT32 plib_timeticks(UINT32 * sec);

bool plib_localtime_ext_bias(time_t sec, struct tm * timetm, INT16 * bias);
bool plib_localtime_ext(time_t sec, struct tm * timetm);
struct tm * plib_localtime(const time_t * sec);
time_t plib_mktime(struct tm * timetm); 
time_t plib_mkUTCtime(struct tm * timetm);
bool plib_addDays(struct tm * tm, int day);
bool plib_timetToStructTm(time_t sec, struct tm * timetm); // prohibited for business logic - returns struct tm in system time

#endif
