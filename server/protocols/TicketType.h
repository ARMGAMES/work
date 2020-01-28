// ===========================================================
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef	TicketType_h_included
#define	TicketType_h_included

#include "ppinclude.h"
#include "commmsgbody.h"
#include "pyrtime.h"

enum TicketTypeCode
{
	eUndefined = 0,	// Undefined code
	eGeneral = 1,	// General (All Purpose)
	eDepositBonus,	// Deposit Bonus Ticket
	eTournamentWin,  // Tournament Win
	eFppStore	// Purchased from FPP Store
};

enum TicketTypeFlags
{
	eTicketTypeFlagsUndefined		= 0x0000,	// Undefined code
	eTicketTypeFlagsGeneral			= 0x0001,	// General (All Purpose)
	eTicketTypeFlagsDepositBonus	= 0x0002,	// Deposit Bonus Ticket
	eTicketTypeFlagsTournamentWin	= 0x0004,	// Tournament Win
	eTicketTypeFlagsFppStore		= 0x0008,	// Purchased from FPP Store
	eTicketTypeFlagsFppTicket		= 0x0010,	// The ticket has only FPP value and its currency should not be considered
	eTicketTypeFlagsPlayMoneyTicket = 0x0020,	// The ticket has only Play Money value and its currency should not be considered
	eTicketTypeFlagsLiveEvent		= 0x0040,	// (1 << 6) PYR-29272 - initially created for GReek taxation
	eTicketTypeFlagsInDev			= 0x0080,   // (1 << 7) PYR-42977 - if set indicates ticket type is in development; it cannot be added to award schemas and instantiated; can be changed	
};

struct TicketType
{
	UINT32 ticketTypeId;
	UINT32 ticketTypeFlags;
	TicketTypeCode	ticketTypeCode;
	PString	admission;
	bool multiTourn;
	INT32 addChips;
	SrvTime	created;
	SrvTime	expDate;
	INT32	expPeriod;
	SrvTime	obsolete;
	UINT32	adminIntId;
	INT32	value;
	INT32	fppValue;
	PString	currency;
	INT64 pmValue64; // PYR-24938
	UINT32 budgetCodeId; // PYR-27615

	// !!! Upon adding new data member please update isChangeAllowed member function below

	void composeData( CommMsgBody& body ) const;
	void parseData( CommMsgParser& parser );
	void logTicketType() const;

	TicketType()
	{
		ticketTypeFlags = eTicketTypeFlagsInDev;
		ticketTypeCode = eUndefined;
		ticketTypeId = addChips = expPeriod = adminIntId = value = fppValue = 0;
		pmValue64 = 0; // PYR-24938
		multiTourn = false;
		budgetCodeId = 0; // PYR-27615
	}
	void clear()
	{
		ticketTypeFlags = eTicketTypeFlagsInDev;
		ticketTypeCode = eUndefined;
		ticketTypeId = addChips = expPeriod = adminIntId = value = fppValue = 0;
		created.setNull();
		expDate.setNull();
		obsolete.setNull();
		admission = "";
		currency = "";
		pmValue64 = 0; // PYR-24938
		budgetCodeId = 0; // PYR-27615
	}
	void getTicketExpiration( const SrvTime& srvNow, SrvTime& ticketExpiration ) const;

	bool isFppTicket() const
	{
		return isFppTicket( ticketTypeFlags );
	}
	bool isObsolete() const
	{
		return !obsolete.isNull();
	}
	static bool isFppTicket( UINT32 ticketTypeFlags )
	{
		return 0 != ( ticketTypeFlags & eTicketTypeFlagsFppTicket );
	}
	bool isPlayMoneyTicket() const
	{
		return isPlayMoneyTicket( ticketTypeFlags );
	}
	static bool isPlayMoneyTicket( UINT32 ticketTypeFlags )
	{
		return 0 != ( ticketTypeFlags & eTicketTypeFlagsPlayMoneyTicket );
	}
	bool isNoCurrencyTicket() const
	{
		return isNoCurrencyTicket( ticketTypeFlags );
	}
	static bool isNoCurrencyTicket( UINT32 flags )
	{
		return ( 0 != ( flags & ( eTicketTypeFlagsFppTicket | eTicketTypeFlagsPlayMoneyTicket ) ) );
	}
	bool isFinalizedTicket() const
	{
		return isFinalizedTicket( ticketTypeFlags );
	}
	static bool isFinalizedTicket( UINT32 flags )
	{
		return 0 == ( flags & eTicketTypeFlagsInDev );
	}
	// PYR-42977
	bool isChangeAllowed( const TicketType& changedTicketType ) const;
};

#endif	//TicketType_h_included
