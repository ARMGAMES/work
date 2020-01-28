//protocolstruct.h
#ifndef	protocolstruct_h_included
#define	protocolstruct_h_included

#include "pyrtime.h"
#include "ppstring.h"

// MOVED from dbmstruct.h after BUILD1075
struct Property
{
	int		propInt;
	PString propStr;
	SrvTime	propWhen;
	bool	isWhenSet;
	Property()
	{
		propInt = 0;
		propWhen.setNull();
		isWhenSet = false;
	}
	void setWhen(const SrvTime& srvTime)
	{
		isWhenSet = true;
		propWhen = srvTime;
	}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseINT32(propInt)
			.parseStringP(propStr)
			.parseBOOL(isWhenSet)
			.parseSrvTime(propWhen);
	}
	void compose(CommMsgBody& body) const
	{
		body
			.composeINT32(propInt)
			.composeString(propStr)
			.composeBOOL(isWhenSet)
			.composeSrvTime(propWhen);
	}
};


#endif // protocolstruct_h_included
