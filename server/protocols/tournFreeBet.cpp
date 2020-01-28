#include "tournFreeBet.h"
#include "stringroutines.h"

const char* FreeBetPropData::toString( PString& str ) const
{
	str.appendUint64( base.campaignId ).append( ":" ).appendInt( base.stake ).append( ":" ).append( base.currency ).append( ":" ).appendInt( awarded.amount ).append( ":" ).append( awarded.currency );
	return str.c_str();
}

bool FreeBetPropData::fromString( const char* str ) 
{
	vector<PString> items;
	splitString( str, items, ':', true, true );

	if( items.size() < 3 )
		return false;

	if( items.size() >= 3 ) //-V547 note: always true
	{
		char* dummy;
		base.campaignId = strtoull( items[0], &dummy, 10 );
		base.stake = atoi( items[1] );
		base.currency = items[2];
	}

	if( items.size() >= 5 )
	{
		awarded.amount = atoi( items[3] );
		awarded.currency = items[4];
	}
	return true;
}