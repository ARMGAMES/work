//	clifppfactor.cpp

#include "clifppfactor.h"

void LobbyListSubscriber_FppFactor::parseFppFactorBody( CommMsgParser& parser, vector< LobbyListSubscriber_FppFactor >& fppFactorVector ) // PYR-24398
{
	PASSERT4( !fppFactorVector.size() );
	fppFactorVector.clear();

	CommMsgBody fppFactorBody;
	parser.parseMsgBody( fppFactorBody );
	CommMsgParser fppFactorParser( fppFactorBody );

	if( !fppFactorParser.parseEnded() )
		parseFppFactorVector( fppFactorParser, fppFactorVector );
}

void LobbyListSubscriber_FppFactor::parseFppFactorVector( CommMsgParser& fppFactorParser, vector< LobbyListSubscriber_FppFactor >& fppFactorVector ) // PYR-24398
{
	PASSERT4( !fppFactorVector.size() );
	fppFactorVector.clear();

	UINT16 fppFactorSize;
	fppFactorParser.parseUINT16( fppFactorSize );
	fppFactorVector.reserve( fppFactorSize );

	for( UINT16 i = 0; i < fppFactorSize; ++i )
	{
		UINT16 fppFactor;
		UINT32 fppFactorSiteMask;
		SrvTime fppFactorEndTime;
		fppFactorParser.parseUINT16( fppFactor ).
			parseUINT32( fppFactorSiteMask ).
			parseSrvTime( fppFactorEndTime );
		fppFactorVector.push_back( LobbyListSubscriber_FppFactor( fppFactor, fppFactorSiteMask, fppFactorEndTime ) );
	}
}
