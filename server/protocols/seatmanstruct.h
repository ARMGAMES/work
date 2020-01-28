#ifndef seatmanstruct_h_included
#define seatmanstruct_h_included

#include "seatmanstruct_cli.h"

struct BuyInRange
{
	UINT16 bottomMin;
	UINT16 topMin;
	UINT16 bottomMax;
	UINT16 topMax;

	BuyInRange() :
		bottomMin(0), topMin(0), bottomMax(0), topMax(0)
	{}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseUINT16(bottomMin)
			.parseUINT16(topMin)
			.parseUINT16(bottomMax)
			.parseUINT16(topMax);
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeUINT16(bottomMin)
			.composeUINT16(topMin)
			.composeUINT16(bottomMax)
			.composeUINT16(topMax);
	}

	bool isIn(UINT16 minBI, UINT16 maxBI) const
	{
		//PTRACE5("%u<=%u<=%u && %u<=%u<=%u", bottomMin, minBI, topMin, bottomMax, maxBI, topMax);
		return (bottomMin <= minBI && minBI <= topMin && bottomMax <= maxBI && maxBI <= topMax);
	}
};

struct BuyInRanges
{
	bool isRevert;
	vector<BuyInRange> ranges;

	BuyInRanges() :
		isRevert(false)
	{}

	void parse(CommMsgParser& parser)
	{
		UINT32 numRanges;
		parser
			.parseBOOL(isRevert)
			.parseUINT32(numRanges);

		// PYR-52366
		UINT32 reserveNum = min<UINT32>(numRanges, DEFAULT_VECTOR_RESERVE_MAXSIZE);
		ranges.clear();
		ranges.reserve(reserveNum);

		for(INT32 iRange = 0; iRange < numRanges; ++iRange)
		{
			ranges.push_back(BuyInRange());
			ranges.back().parse(parser);
		}
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeBOOL(isRevert)
			.composeUINT32(ranges.size());
		for(int iRange = 0; iRange < ranges.size(); ++iRange)
		{
			ranges[iRange].compose(body);
		}
	}

	bool pass(UINT16 minBI, UINT16 maxBI) const
	{
		bool ret = false;
		for(int iRange = 0; iRange < ranges.size(); ++iRange)
		{
			if(ranges[iRange].isIn(minBI, maxBI))
			{
				ret = true;
				break;
			}
		}
		if(isRevert)
			ret = !ret;
		return ret;
	}
};

//PYR-20182
struct SeatManAdvSearchCriteria
{
	UINT32 minAvgPot;
	UINT32 minAvgStack;
	BYTE minPlayersFlop;
	BYTE minVpip;
	UINT16 minHandsHour;

	SeatManAdvSearchCriteria() :
		minAvgPot(0), minAvgStack(0), minPlayersFlop(0), minVpip(0), minHandsHour(0)
	{}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseUINT32(minAvgPot)
			.parseUINT32(minAvgStack)
			.parseBYTE(minPlayersFlop)
			.parseBYTE(minVpip)
			.parseUINT16(minHandsHour);
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeUINT32(minAvgPot)
			.composeUINT32(minAvgStack)
			.composeBYTE(minPlayersFlop)
			.composeBYTE(minVpip)
			.composeUINT16(minHandsHour);
	}

	bool isAnything() const { return (minAvgPot || minAvgStack || minPlayersFlop || minVpip || minHandsHour); }

	const char* toString(PString& ret) const
	{
		ret.assign("SMASC:")
			.appendUint(minAvgPot).append(',').appendUint(minAvgStack).append(',').appendUint(minPlayersFlop)
			.append(',').appendUint(minVpip).append(',').appendUint(minHandsHour);
		return ret.c_str();
	}
};

struct SeatManSearchCriteria
{
	BYTE minPlayers;
	BYTE game;
	BYTE hiLo;
	BYTE structure;
	BYTE maxPlayersMin;
	BYTE maxPlayersMax;
	INT64 minLoBet;
	INT64 minHiBet;
	INT64 maxLoBet;
	INT64 maxHiBet;
	bool capOn;
	BYTE speedBM;
	UINT32 currencyBM;
	BuyInRanges buyInRanges;

	SeatManSearchCriteria() :
		minPlayers(0), game(0), hiLo(0), structure(0), maxPlayersMin(0), maxPlayersMax(0),
		minLoBet(0), minHiBet(0), maxLoBet(0), maxHiBet(0), capOn(false), speedBM(0), currencyBM(0)
	{}

	void parse(CommMsgParser& parser)
	{
		UINT32 minLoBet32;
		UINT32 minHiBet32;
		UINT32 maxLoBet32;
		UINT32 maxHiBet32;
		parser
			.parseBYTE(minPlayers)
			.parseBYTE(game)
			.parseBYTE(hiLo)
			.parseBYTE(structure)
			.parseBYTE(maxPlayersMin)
			.parseBYTE(maxPlayersMax)
			.parseUINT32(minLoBet32)
			.parseUINT32(minHiBet32)
			.parseUINT32(maxLoBet32)
			.parseUINT32(maxHiBet32)
			.parseBOOL(capOn)
			.parseBYTE(speedBM)
			.parseUINT32(currencyBM);
		buyInRanges.parse(parser);
		minLoBet = minLoBet32;
		minHiBet = minHiBet32;
		maxLoBet = maxLoBet32;
		maxHiBet = maxHiBet32;
	}

	void parse2(CommMsgParser& parser)
	{
		parser
			.parseBYTE(minPlayers)
			.parseBYTE(game)
			.parseBYTE(hiLo)
			.parseBYTE(structure)
			.parseBYTE(maxPlayersMin)
			.parseBYTE(maxPlayersMax)
			.parseINT64(minLoBet)
			.parseINT64(minHiBet)
			.parseINT64(maxLoBet)
			.parseINT64(maxHiBet)
			.parseBOOL(capOn)
			.parseBYTE(speedBM)
			.parseUINT32(currencyBM);
		buyInRanges.parse(parser);
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeBYTE(minPlayers)
			.composeBYTE(game)
			.composeBYTE(hiLo)
			.composeBYTE(structure)
			.composeBYTE(maxPlayersMin)
			.composeBYTE(maxPlayersMax)
			.composeINT64(minLoBet)
			.composeINT64(minHiBet)
			.composeINT64(maxLoBet)
			.composeINT64(maxHiBet)
			.composeBOOL(capOn)
			.composeBYTE(speedBM)
			.composeUINT32(currencyBM);
		buyInRanges.compose(body);
	}

	const char* toString(PString& ret) const
	{
		ret.assign("SMSC:")
			.appendUint(minPlayers).append(',').appendUint(game).append(',').appendUint(hiLo).append(',').appendUint(structure).append(',')
			.appendUint(maxPlayersMin).append('-').appendUint(maxPlayersMax).append(',')
			.appendInt64(minLoBet).append('-').appendInt64(minHiBet).append('/')
			.appendInt64(maxLoBet).append('-').appendInt64(maxHiBet)
			.append(",0x").appendHexInt(speedBM).append(",0x").appendHexInt(currencyBM);
		if(capOn)
			ret.append(",cap");
		if(buyInRanges.isRevert)
			ret.append(",rev");
		for(int iRange = 0; iRange < buyInRanges.ranges.size(); ++iRange)
		{
			ret.append(':').appendUint(buyInRanges.ranges[iRange].bottomMin).append('-').appendUint(buyInRanges.ranges[iRange].topMin)
				.append('/').appendUint(buyInRanges.ranges[iRange].bottomMax).append('-').appendUint(buyInRanges.ranges[iRange].topMax);
		}
		return ret.c_str();
	}
};

class SeatManCurrencyRules
{
public:
	// PYR-30558 - refactored currency check
	struct Currency
	{
		eSeatManCurrency currBit;
		const char* currName;
	};
	static const Currency* getCurrMap( INT32& size )
	{
		static const Currency currMap[] =
		{
			{ eSeatManCurrency_USD, "USD" },
			{ eSeatManCurrency_EUR, "EUR" },
			{ eSeatManCurrency_CAD, "CAD" },
			{ eSeatManCurrency_GBP, "GBP" },
			{ eSeatManCurrency_INR, "INR" }, // PYR-30558
		};
		size = sizeof(currMap)/sizeof(currMap[0]);
		return currMap;
	}
	static bool checkCurrency( const UINT32 flags, const char* currency )
	{
		INT32 size;
		const Currency* const currMap = getCurrMap( size );
		for( INT32 i = 0; i < size; ++i )
		{
			if( (flags & currMap[i].currBit) != 0 && compareCurrencies( currMap[i].currName, currency ) )
				return true;
		}
		return false;
	}

	static eSeatManCurrency getCurrencyEnumValue(bool playMoney, const char * currencyStr)
	{
		if( playMoney )
			return eSeatManCurrency_PM;
		INT32 size;
		const Currency* const currMap = getCurrMap( size );
		for( INT32 i = 0; i < size; ++i )
		{
			if( PString::equals( currMap[i].currName, currencyStr ) )
				return currMap[i].currBit;
		}
		return eSeatManCurrency_UNK;
	}
};



class SeatManGameRules
{
public:
	static const char* getGameStringByEnumValue( HandType enumValue )
	{
		switch( enumValue )
		{
			case HandTypeNone:
				return "None";

			case HandTypeBadugi:
				return "Badugi";

			case HandTypeHoldem:
				return "Holdem";

			case HandTypeOmaha:
				return "Omaha";

			case HandTypeDraw:
				return "Draw";

			case HandTypeSingleDraw27:
				return "SingleDraw27";

			case HandTypeStud:
				return "Stud";

			case HandTypeRazz:
				return "Razz";

			case HandType3Draw27:
				return "3Draw27";

			case HandType3DrawA5:
				return "3DrawA5";

			default:
				return "Other";

		}
	}

	static const char* getPotStructStringByEnumValue( PotStruct enumValue )
	{
		switch( enumValue )
		{
		case StructNonDefined:
			return "None";
		
		case StructLimit:
			return "Limit";

		case StructPotLimit:
			return "Pot Limit";

		case StructNoLimit:
			return "No Limit";

		case StructPotNoLimit:
			return "Pot No Limit";

		case StructMultiGameMixed:
			return "MultiGame Mixed";

		default:
			return "Unknown";
		}
	}

	static const char* getSpeedStringByEnumValue( SpeedDisplayOptions enumValue )
	{
		switch( enumValue )
		{
		case SpeedDisplayNone:
			return "None";

		case SpeedDisplayRegular:
			return "Regular";

		case SpeedDisplayTurbo:
			return "Turbo";

		case SpeedDisplayHyper:
			return "Hyper";

		case SpeedDisplaySlow:
			return "Slow";

		default:
			return "Unknown";
		}
	}
};

struct QuickStartSngSearchCriteria
{
	UINT32 minBuyIn;
	UINT32 maxBuyIn;
	bool isPlayMoney;
	PString currency;
	BYTE /*SpeedDisplayOptions*/ speed;
	BYTE /*HandType*/ game;
	BYTE /* virtually bool*/hiLo;
	BYTE /*PotStruct*/ structure;
	UINT32 minPlayersFrom; //tourn size
	UINT32 minPlayersTo;
	vector< pair<BYTE, BYTE> > tableSizes;
	UINT32 flags;

	void parse(CommMsgParser& parser)
	{
		parser
			.parseBYTE(game)
			.parseBYTE(hiLo)
			.parseBYTE(structure)
			.parseBYTE(speed)
			.parseBOOL(isPlayMoney)
			.parseStringP(currency)
			.parseUINT32(minPlayersFrom)
			.parseUINT32(minPlayersTo)
			.parseUINT32(minBuyIn)
			.parseUINT32(maxBuyIn);
		if( !parser.parseEnded())  //
		{
			UINT32 size;
			parser
				.parseUINT32(flags)
				.parseUINT32(size);

			tableSizes.clear();
			tableSizes.reserve(min<UINT32>(size, DEFAULT_VECTOR_RESERVE_MAXSIZE)); // PYR-52366
			for(UINT32 i=0; i < size; ++i)
			{
				BYTE min, max;
				parser
					.parseBYTE(min)
					.parseBYTE(max);
				tableSizes.push_back(make_pair(min, max));
			}
		}
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeBYTE(game)
			.composeBYTE(hiLo)
			.composeBYTE(structure)
			.composeBYTE(speed)
			.composeBOOL(isPlayMoney)
			.composeString(currency)
			.composeUINT32(minPlayersFrom)
			.composeUINT32(minPlayersTo)
			.composeUINT32(minBuyIn)
			.composeUINT32(maxBuyIn)
			.composeUINT32(flags)
			.composeUINT32(tableSizes.size());
			for(UINT32 i=0; i < tableSizes.size(); ++i)
			{
				body
					.composeBYTE( tableSizes[i].first )
					.composeBYTE( tableSizes[i].second );
			}
	}


	QuickStartSngSearchCriteria(): minBuyIn(0), maxBuyIn(0), speed(SpeedDisplayRegular), game(HandTypeHoldem), 
		hiLo(0), structure(StructNoLimit), minPlayersFrom(0), minPlayersTo(0), isPlayMoney(true), flags(0) {}

	const char* print( PString& ret ) const
	{
		ret = PString(" minBuyIn=").appendUint(minBuyIn)
			.append(" maxBuyIn=").appendUint(maxBuyIn)
			.append(" isPlayMoney=").append(isPlayMoney ? "yes" : "no")
			.append(" currency=").append(currency)
			.append(" speed=").append(SeatManGameRules::getSpeedStringByEnumValue((SpeedDisplayOptions)speed) )
			.append(" game=").append(SeatManGameRules::getGameStringByEnumValue((HandType)game))
			.append(" hiLo=").append(hiLo? "yes" : "no")
			.append(" structure=").append(SeatManGameRules::getPotStructStringByEnumValue((PotStruct)structure))
			.append(" minPlayersFrom=").appendUint((UINT32)minPlayersFrom)
			.append(" minPlayersTo=").appendUint((UINT32)minPlayersTo)
			.append(" flags=").appendUint(flags);

		for( UINT32 i = 0; i < tableSizes.size(); ++i )
		{
			ret.append(" ts[").appendUint(i).append("]=(")
				.appendUint(tableSizes[i].first).append(",").appendUint(tableSizes[i].second).append(")");
		}
		return ret.c_str();
	}
};

struct QuickStartSngSearchCriteria2
{
	UINT64 minBuyIn;
	UINT64 maxBuyIn;
	BYTE /*HandType*/ game;
	BYTE /* virtually bool*/hiLo;
	BYTE /*PotStruct*/ structure;
	UINT32 minPlayersFrom; //tourn size
	UINT32 minPlayersTo;
	UINT32 flags;
	vector< pair<BYTE, BYTE> > tableSizes;
	vector< pair<UINT16, UINT16> > durations;	//PYR-45020

	void parse(CommMsgParser& parser)
	{
		UINT32 tSize, dSize;

		parser
			.parseBYTE(game)
			.parseBYTE(hiLo)
			.parseBYTE(structure)
			.parseUINT32(minPlayersFrom)
			.parseUINT32(minPlayersTo)
			.parseUINT64(minBuyIn)
			.parseUINT64(maxBuyIn)
			.parseUINT32(flags)
			.parseUINT32(tSize);

		tableSizes.clear();
		tableSizes.reserve(min<UINT32>(tSize, DEFAULT_VECTOR_RESERVE_MAXSIZE)); // PYR-52366
		for(int iT=0; iT < tSize; ++iT)
		{
			BYTE minT, maxT;
			parser
				.parseBYTE(minT)
				.parseBYTE(maxT);
			tableSizes.push_back(make_pair(minT, maxT));
		}
		durations.clear();
		if(!parser.parseEnded())
		{//PYR-45020
			parser.parseUINT32(dSize);
			for(int iD = 0; iD < dSize; ++iD)
			{
				UINT16 minD, maxD;
				parser.parseUINT16(minD).parseUINT16(maxD);
				durations.push_back(make_pair(minD, maxD));
			}
		}
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeBYTE(game)
			.composeBYTE(hiLo)
			.composeBYTE(structure)
			.composeUINT32(minPlayersFrom)
			.composeUINT32(minPlayersTo)
			.composeUINT64(minBuyIn)
			.composeUINT64(maxBuyIn)
			.composeUINT32(flags)
			.composeUINT32(tableSizes.size());
		for(int iT=0; iT < tableSizes.size(); ++iT)
		{
			body
				.composeBYTE( tableSizes[iT].first )
				.composeBYTE( tableSizes[iT].second );
		}
		//PYR-45020
		body.composeUINT32(durations.size());
		for(int iD = 0; iD < durations.size(); ++iD)
		{
			body
				.composeUINT16(durations[iD].first)
				.composeUINT16(durations[iD].second);
		}
	}

	QuickStartSngSearchCriteria2(): minBuyIn(0), maxBuyIn(0), game(HandTypeHoldem), 
		hiLo(0), structure(StructNoLimit), minPlayersFrom(0), minPlayersTo(0), flags(0) {}

	const char* print( PString& ret ) const
	{
		ret = PString(" minBuyIn=").appendUint64(minBuyIn)
			.append(" maxBuyIn=").appendUint64(maxBuyIn)
			.append(" game=").append(SeatManGameRules::getGameStringByEnumValue((HandType)game))
			.append(" hiLo=").append(hiLo? "yes" : "no")
			.append(" structure=").append(SeatManGameRules::getPotStructStringByEnumValue((PotStruct)structure))
			.append(" minPlayersFrom=").appendUint((UINT32)minPlayersFrom)
			.append(" minPlayersTo=").appendUint((UINT32)minPlayersTo)
			.append(" flags=").appendUint(flags);

		for( int iT = 0; iT < tableSizes.size(); ++iT )
		{
			ret.append(" ts[").appendInt(iT).append("]=(")
				.appendUint(tableSizes[iT].first).append(",").appendUint(tableSizes[iT].second).append(")");
		}
		for(int iD = 0; iD < durations.size(); ++iD)
		{//PYR-45020
			ret.append(" dur[").appendInt(iD).append("]=(")
				.appendUint(durations[iD].first).append(",").appendUint(durations[iD].second).append(")");
		}
		return ret.c_str();
	}
};


#endif //seatmanstruct_h_included
