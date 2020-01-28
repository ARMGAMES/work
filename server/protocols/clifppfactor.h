//	clifppfactor.h

#ifndef	clifppfactor_h_included
#define	clifppfactor_h_included

#include "pyrtime.h"

struct LobbyListSubscriber_FppFactor // PYR-24398
{
	UINT16 fppFactor;
	UINT32 fppFactorSiteMask;
	SrvTime fppFactorEndTime;

	LobbyListSubscriber_FppFactor( UINT16 fppFactor_, UINT32 fppFactorSiteMask_, const SrvTime& fppFactorEndTime_ )
		: fppFactor( fppFactor_ ), fppFactorSiteMask( fppFactorSiteMask_ ), fppFactorEndTime( fppFactorEndTime_ )
	{
	}

	static void parseFppFactorBody( CommMsgParser& parser, vector< LobbyListSubscriber_FppFactor >& fppFactorVector );
	static void parseFppFactorVector( CommMsgParser& parser, vector< LobbyListSubscriber_FppFactor >& fppFactor );
};

#endif // clifppfactor_h_included
