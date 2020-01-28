#ifndef AltBounty_h_included
#define AltBounty_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commmsgbody.h"

// 	PYR-31658
struct AltBounty
{
	//WARNING: IMPORTANT! any change to this struct must also change the ATF protocol definition file srvshared/table/tableserverprot.txt P_AltBounty struct
	UINT64 tournId;
	UINT32 ticketTypeId;
	INT32 tChips;
	INT32 tournBuyInPlusRake;
	PString admission;

	AltBounty(): tournId(0), ticketTypeId(0), tChips(0), tournBuyInPlusRake(0)
	{}

	bool operator==(const AltBounty& other) const
	{
		return (this->tournId == other.tournId && 
			this->ticketTypeId == other.ticketTypeId &&
			this->tChips == other.tChips);
	}

	void compose(CommMsgBody& body) const
	{
		CommMsgBody tempBody;

		tempBody.composeUINT64(tournId)
			.composeUINT32(ticketTypeId)
			.composeINT32(tChips)
			.composeINT32(tournBuyInPlusRake)
			.composeString(admission);
		// add new parameters to the end

		body.composeMsgBody(tempBody);
	}

	void parse(CommMsgParser& parser)
	{
		CommMsgBody tempBody;
		parser.parseMsgBody(tempBody);
		CommMsgParser parser2(tempBody);

		parser2.parseUINT64(tournId)
			.parseUINT32(ticketTypeId)
			.parseINT32(tChips)
			.parseINT32(tournBuyInPlusRake)
			.parseStringP(admission);

		// add new parameters to the end with parseEnded condition
	}
};

#endif