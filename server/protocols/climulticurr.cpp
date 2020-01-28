//	climulticurr.cpp

#include "climulticurr.h"

const char* _parseCurrency( CommMsgParser& parser )
{
	const char* curr;
	parser.parseString( curr );
	return curr && *curr ? curr : "USD";
}

PString& _parseCurrency( CommMsgParser& parser, PString& curr )
{
	curr.assign( _parseCurrency( parser ) );
	return curr;
}

PString& _fixCurrency( PString& curr )
{
	if( !curr.length() )
	{
		curr.assign( "USD" );
	}

	return curr;
}
