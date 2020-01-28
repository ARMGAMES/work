/**
 * MultidayTourn_thin.h
 *
 * This file was auto-generated from MultidayTourn.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin MultidayTourn.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_MULTIDAYTOURN_THIN_H
#define ATF_PROTOCOL_DEFFILE_MULTIDAYTOURN_THIN_H


#include "ThinAtfUserDefStructs.h"

namespace TournClientThin
{
	enum MultidayTournProps 	//order / bitmask
	{
		nonMultiday		= 0,
		multidayPlrs	= 1,
		multidayLvls	= 2,
		multidayTime	= 3,
		multidayMoney	= 4,
		multidayPropsMask = 0x000000FF,

		multidaySeatRedraw = 1 << 8 	//256  SeatRedraw=Yes script parameter
	};

	struct MultidayTourn
	{
		// 444t - props,dayOneStopValue,dayTwoStartMinutes,dayTwoStart
		UINT32 props;	// refers to MultidayTournProps enum
		UINT32 dayOneStopValue;	// Day1End script parameter (Players | Levels | Time)
		UINT32 dayTwoStartMinutes;	// Day2Start/tt script parameter
		CommSrvTime dayTwoStart;	// Day2Start/specific time script parameter

		MultidayTourn();
		void clear();
		bool equals(const MultidayTourn&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};
};

#endif