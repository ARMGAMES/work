// ===========================================================
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef	tourn_milestone_prize_h_included
#define	tourn_milestone_prize_h_included

#include "commmsgbody.h"
#include "dbm.h"
#include "UserDefStructs.h"
#include "AtfValidator.h"

struct TournMileStonePrize
{
	const static INT32 CASH_MAX = 10000000;
	const static INT32 TOURNCASH_MAX = 10000000;
	const static INT32 FPP_MAX = 100000;
	const static INT32 PLAYMONEY_MAX = PINT32_MAX;
	const static INT32 NUMTICKETS_MAX = 100;

	INT32 cash;
	INT32 tournCash;
	INT32 fpp;
	INT32 playMoney;  // in dollars, not in cents
	UINT32 ticketTypeId;
	PString ticketName;
	PString ticketCurrency;
	INT64 ticketPrize;
	UINT32 ticketTypeFlags;
	INT32 numTickets;

	TournMileStonePrize()
	{
		clear();
	}

	void clear()
	{
		cash = 0;
		tournCash = 0;
		fpp = 0;
		playMoney = 0;
		ticketTypeId = 0;
		ticketName.clear();
		ticketCurrency.clear();
		ticketPrize = 0;
		ticketTypeFlags = 0;
		numTickets = 0;
	}

	const char* toTraceString(PString& _buf) const
	{
		_buf.append('{');
		_buf.append("cash=");
		_buf.appendInt(cash);
		_buf.append(',');
		_buf.append("tournCash=");
		_buf.appendInt(tournCash);
		_buf.append(',');
		_buf.append("fpp=");
		_buf.appendInt(fpp);
		_buf.append(',');
		_buf.append("playMoney=");
		_buf.appendInt(playMoney);
		_buf.append(',');
		_buf.append("ticketTypeId=");
		_buf.appendUint(ticketTypeId);
		_buf.append(',');
		_buf.append("ticketName=");
		_buf.append(ticketName);
		_buf.append(',');
		_buf.append("ticketCurrency=");
		_buf.append(ticketCurrency);
		_buf.append(',');
		_buf.append("ticketPrize=");
		_buf.appendInt64(ticketPrize);
		_buf.append(',');
		_buf.append("ticketTypeFlags=");
		_buf.appendUint(ticketTypeFlags);
		_buf.append(',');
		_buf.append("numTickets=");
		_buf.appendInt(numTickets);
		_buf.append('}');
		return _buf.c_str();
	}

	void compose(CommMsgBody& msg) const
	{
		msg
			.composeINT32(cash)
			.composeINT32(tournCash)
			.composeINT32(fpp)
			.composeINT32(playMoney)
			.composeUINT32(ticketTypeId)
			.composeString(ticketName)
			.composeString(ticketCurrency)
			.composeINT64(ticketPrize)
			.composeUINT32(ticketTypeFlags)
			.composeINT32(numTickets)
			;
	}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseINT32(cash)
			.parseINT32(tournCash)
			.parseINT32(fpp)
			.parseINT32(playMoney)
			.parseUINT32(ticketTypeId)
			.parseStringP(ticketName)
			.parseStringP(ticketCurrency)
			.parseINT64(ticketPrize)
			.parseUINT32(ticketTypeFlags)
			.parseINT32(numTickets)
			;
	}

	void composeMsg(CommMsgBody& body, bool _ignoreJSON = false) const { compose(body); }
	void parseMsg(CommMsgParser& parser) { parse(parser); }

	bool equals(const TournMileStonePrize& other) const
	{
		return cash == other.cash &&
			tournCash == other.tournCash &&
			fpp == other.fpp &&
			playMoney == other.playMoney &&
			ticketTypeId == other.ticketTypeId &&
			numTickets == other.numTickets;
	}

	bool isValid(bool isPlayMoney) const
	{
		if(isPlayMoney)
		{
			if(cash || tournCash || fpp)
			{
				return false;
			}
			if(!playMoney && !ticketTypeId)
			{
				return false;
			}
			if(playMoney && playMoney > PLAYMONEY_MAX)  //lint !e685 //-V560
			{
				return false;
			}
			if(ticketTypeId && (numTickets < 1 || numTickets > NUMTICKETS_MAX))
			{
				return false;
			}
			return true;
		}
		else
		{
			if(playMoney)
			{
				return false;
			}
			if(!cash && !tournCash && !fpp && !ticketTypeId)
			{
				return false;
			}
			if(cash && cash > CASH_MAX)
			{
				return false;
			}
			if(tournCash && tournCash > TOURNCASH_MAX)
			{
				return false;
			}
			if(fpp && fpp > FPP_MAX)
			{
				return false;
			}
			if(ticketTypeId && (numTickets < 1 || numTickets > NUMTICKETS_MAX))
			{
				return false;
			}
			return true;
		}
		
	}

	static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
	{
		INT32 cash; _parser.parseINT32(cash);
		AtfValidator::validateInt(_descr, "cash", cash, _checker, __FILE__, __LINE__);
		INT32 tournCash; _parser.parseINT32(tournCash);
		AtfValidator::validateInt(_descr, "tournCash", tournCash, _checker, __FILE__, __LINE__);
		INT32 fpp; _parser.parseINT32(fpp);
		AtfValidator::validateInt(_descr, "fpp", fpp, _checker, __FILE__, __LINE__);
		INT32 playMoney; _parser.parseINT32(playMoney);
		AtfValidator::validateInt(_descr, "playMoney", playMoney, _checker, __FILE__, __LINE__);
		UINT32 ticketTypeId; _parser.parseUINT32(ticketTypeId);
		AtfValidator::validateInt(_descr, "ticketTypeId", ticketTypeId, _checker, __FILE__, __LINE__);
		PString ticketName; _parser.parseStringP(ticketName);
		PString ticketCurrency; _parser.parseStringP(ticketCurrency);
		INT64 ticketPrize; _parser.parseINT64(ticketPrize);
		AtfValidator::validateInt(_descr, "ticketPrize", ticketPrize, _checker, __FILE__, __LINE__);
		UINT32 ticketTypeFlags; _parser.parseUINT32(ticketTypeFlags);
		AtfValidator::validateInt(_descr, "ticketTypeFlags", ticketTypeFlags, _checker, __FILE__, __LINE__);
		INT32 numTickets; _parser.parseINT32(numTickets);
		AtfValidator::validateInt(_descr, "numTickets", numTickets, _checker, __FILE__, __LINE__);
	}

	bool isEmpty() const
	{
		return cash == 0 && tournCash == 0 && fpp == 0 && playMoney == 0 && numTickets == 0;
	}

	const char *toStringByLocale(UINT32 locale, const PString& tournCurrency, PString& outBuf) const;

	class PrizeFormatter
	{
		const TournMileStonePrize& prize;
		enum prizeItems { PI_NONE, PI_CASH, PI_TOURNCASH, PI_FPP, PI_PLAYMONEY, PI_TICKET, PI_LAST };
		prizeItems nextIndex;
	public:
		PrizeFormatter(const TournMileStonePrize& prize_);
		bool hasNext() const;
		const char *formatNextPrizeItem(UINT32 locale, const PString& currency, PString& buf);
	private:
		void gotoNextPrizeItem();
	};

	// placeholders
	const char *toXmlString(const char *_element, PString& _str) const { return _str; }
	bool fromXmlString( const PString& _str ) { return true; }
	const char *toJSONString( PString& _str ) const { return _str; }
	void fromJSONString(const PString& _str) {}
	static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent) {}
};


struct TournMileStoneWinningTableData
{
	UINT32 tournId;
	bool isPlayMoney;
	UINT64 tableId;
	PString tableSeqIdStr;
	PString currency;
	UINT32 milestoneEventTimeSecInScript;
	TournMileStonePrize prize;
	vector< PString > players;
	PString userComment;
	UINT64 appLoginId; // PYR-42034
	UINT64 finalFlightId; // PYR-52678
	UINT64 buyInTransId; // PYR-52678
	UINT64 tournFlags2; // PYR-52678
	INT32 cvlSchema = 0; // PYR-86838

	TournMileStoneWinningTableData()
		: tournId(0), isPlayMoney(false), tableId(0), milestoneEventTimeSecInScript(0)
		, appLoginId( 0 ) // PYR-42034
		, finalFlightId( 0 ) // PYR-52678
		, buyInTransId( 0 ) // PYR-52678
		, tournFlags2( 0 ) // PYR-52678
	{
	}

	void compose(CommMsgBody& body) const
	{
		body.
			composeUINT32(tournId).
			composeString(currency).
			composeBOOL(isPlayMoney).
			composeUINT64(tableId).
			composeString(tableSeqIdStr).
			composeUINT32(milestoneEventTimeSecInScript);

		prize.compose(body);

		body.composeUINT32(players.size());
		for(size_t i = 0; i < players.size(); ++i)
		{
			body.composeString(players[i]);
		}

		PASSERT5(*userComment);
		body
			.composeString(userComment)
			.composeUINT64( appLoginId ) // PYR-42034
			.composeUINT64( finalFlightId ) // PYR-52678
			.composeUINT64( buyInTransId ) // PYR-52678
			.composeUINT64( tournFlags2 ) // PYR-52678
			.composeINT32( cvlSchema ) // PYR-86838
		;
	}

	void parse(CommMsgParser& parser)
	{
		parser.
			parseUINT32(tournId).
			parseStringP(currency).
			parseBOOL(isPlayMoney).
			parseUINT64(tableId).
			parseStringP(tableSeqIdStr).
			parseUINT32(milestoneEventTimeSecInScript);

		PASSERT5(tournId);
		PASSERT5(tableId);
		PASSERT5(milestoneEventTimeSecInScript);

		prize.parse(parser);

		UINT32 num;
		parser.parseUINT32(num);
		for(size_t i = 0; i < num; ++i)
		{
			const char *userId;
			parser.parseString(userId);
			players.push_back(userId);
		}

		parser
			.parseStringP(userComment)
			.parseUINT64( appLoginId ) // PYR-42034
			.parseUINT64( finalFlightId ) // PYR-52678
			.parseUINT64( buyInTransId ) // PYR-52678
			.parseUINT64( tournFlags2 ) // PYR-52678
			.parseINT32( cvlSchema ) // PYR-86838
			;
	}

	void encodeTransComment(PString& buf) const
	{
		buf.
			appendUint(prize.ticketTypeId).
			append(';').
			appendUint(prize.numTickets).
			append(';').
			appendUint64(tableId).
			append(';').
			append(tableSeqIdStr).
			append(';').
			appendUint(TOURN_EVENT_MILESTONETOURN).
			append(';').
			appendUint(milestoneEventTimeSecInScript);
	}
};

#endif

