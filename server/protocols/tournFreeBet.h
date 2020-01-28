#ifndef	tournfreebet_h_included
#define	tournfreebet_h_included

#include "commmsgbody.h"

// PYR-59874
struct FreeBetDataBase
{
	UINT64 campaignId;
	INT32 stake;
	PString currency;

	FreeBetDataBase() 
		: campaignId( 0 )
		, stake( 0 )
	{}

	bool isNull() const { return stake == 0; }

	void parse( CommMsgParser& parser )
	{
		parser
			.parseUINT64(campaignId)
			.parseINT32( stake )
			.parseStringP( currency )
			;
	}

	void compose( CommMsgBody& body ) const
	{
		body
			.composeUINT64( campaignId )
			.composeINT32( stake )
			.composeString( currency )
			;
	}
};


struct FreeBetAwardedInfo
{
	INT32 amount;
	PString currency;

	FreeBetAwardedInfo() : amount( 0 ){}

	void parse( CommMsgParser& parser )
	{
		parser
			.parseINT32( amount )
			.parseStringP( currency )
			;
	}

	void compose( CommMsgBody& body ) const
	{
		body
			.composeINT32( amount )
			.composeString( currency )
			;
	}
};

struct FreeBetWinnerInfo
{
	INT32 userIntId;
	PString currency;
	PString userId;

	FreeBetWinnerInfo() : userIntId( 0 ){}

	void parse( CommMsgParser& parser )
	{
		parser
			.parseINT32( userIntId )
			.parseStringP( currency )
			.parseStringP( userId )
			;
	}

	void compose( CommMsgBody& body ) const
	{
		body
			.composeINT32( userIntId )
			.composeString( currency )
			.composeString( userId )
			;
	}
};


struct FreeBetPropData
{
	FreeBetDataBase base;
	FreeBetAwardedInfo awarded;

	void parse( CommMsgParser& parser )
	{
		base.parse( parser );
		awarded.parse( parser );
	}

	void compose( CommMsgBody& body ) const
	{
		base.compose( body );
		awarded.compose( body );
	}

	const char* toString( PString& str ) const;
	bool fromString( const char* str );

};

#endif //tournfreebet_h_included