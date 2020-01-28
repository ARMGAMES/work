// PlayChipsScaler.h 

#ifndef PlayChipsScaler_h 
#define PlayChipsScaler_h 


#include "pplib.h"
#include "pplogfile.h"
#include "dbm.h"


inline INT32 MAX_PLAYCHIPS32( INT64 val )
{
	if( val > MAX_BANKROLL_SIZE )
		return MAX_BANKROLL_SIZE;
	if( val < -MAX_BANKROLL_SIZE )
		return -MAX_BANKROLL_SIZE;

	return (INT32)(val);
}

class PlayChipsScaler
{
public:
	// PYR-40177. UINT32 holds approx 4 * 10^9. This must be size of prize pool.
	// defaultMaxNumberPlayersTourn = 2000. This is number of entrants.
	// so max unscaled buyin must be 2 * 10^6 (reduced to 1*10^6)
	static const UINT64 MAX_UNSCALED_BUYIN = 1000000;

	static INT64 getExpandedTo64( INT32 playChips32, INT32 scalePM )
	{
		if( !scalePM )
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": getExpandedTo64 0 scalePM");
			PASSERT5(0);
			scalePM = 1;
		}

		return (INT64)playChips32 * scalePM;
	}
	static INT32 getCompactedTo32( INT64 playChips64, INT32 scalePM )
	{
		if( !scalePM )
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": getCompactedTo32 0 scalePM");
			PASSERT5(0);
			scalePM = 1;
		}
		return MAX_PLAYCHIPS32( playChips64 / scalePM );
	}
	static INT32 getScaleFromString( const char* str, const char* logDebug = "" )
	{
		INT32 scalePM = atoi(str);
		if( scalePM < 1 || scalePM > 1000000000 )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": getStringToScale: '%s' invalid scale %d orig '%s'", logDebug, scalePM, str );
			PASSERT5(0);
			scalePM = 1;
		}
		return scalePM; // casting will be ok because scalePM here is between 1 and 1e+9
	}
	static INT32 convertPowerToScale( INT32 pow )
	{
		if( pow < 0 )
			throw PError( "Parameter 'Scale=' may not be negative !" );
		if( pow > 9 )
			throw PError( "Parameter 'Scale=' may not exceed '9' !" );

		INT32 scalePM = 1;
		while( pow-- )
		{
			scalePM *= 10;
		}
		return scalePM; // casting will be ok because scalePM here is between 1 and 1e+9
	}
	// PYR-40177 Convenience functions
	static void calculateScale( UINT64& buyIn, INT64& scaleFactor )
	{
		// calculate appropriate scale factor
		scaleFactor = 1LL;

		while( buyIn > MAX_UNSCALED_BUYIN )
		{
			buyIn /= 10LL;
			scaleFactor *= 10LL;
		}

		if( scaleFactor <= 0LL )
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "Computed scale factor not > 0: buyIn = %" PRIu64 " scaleFactor = %" PRId64 "", buyIn, scaleFactor);
		}
	}
	static bool applyAndTestScale(const UINT64 preScale, const INT64 scaleFactor, UINT64& postScale)
	{
		postScale = preScale / scaleFactor;
		return postScale * scaleFactor == preScale;
	}
};

#endif //PlayChipsScaler_h 
