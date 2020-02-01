// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "ColTCADbmprepstmt.h"
#include "initdatetime.h"
#include "TCADetector.h"

GetTCAPartnersStmt::GetTCAPartnersStmt(DatabaseManagerCommon& man, const PString& query)
	: NoExecStmt(man)
{
	prepare(query);

	PDumpEndLine(query);
	bindFirstCol(userId);
	bindNextCol(partnerId);
	bindNextCol(totalTourns);
	bindNextCol(numAllInConfrontations);
}

void GetTCAPartnersStmt::execGet(UINT32 queryTimeLimitMinutes, UINT32 maxColPairSize, ColPairMap& colPairMap)
{
	UINT32 count = 0;

	setTimeOut(queryTimeLimitMinutes * SECONDS_IN_A_MINUTE);
	execute();

	while (fetch())
	{
		count++;
		if (count > maxColPairSize)
			break;

		colPairMap.insertPair(UserPartnerPair(userId.c_str(), partnerId.c_str()), ColPairData(totalTourns.value, numAllInConfrontations.value));
	}

	closeCursor();
}


GetTCAUserTournStmt::GetTCAUserTournStmt(DatabaseManagerCommon& man, const PString& query)
	: NoExecStmt(man)
{
	prepare(query);

	PDumpEndLine(query);
	bindFirstCol(userId);
	bindNextCol(totalTourns);
}

void GetTCAUserTournStmt::execGet(UINT32 queryTimeLimitMinutes, UserTournDataMap& userTournMap)
{
	setTimeOut(queryTimeLimitMinutes * SECONDS_IN_A_MINUTE);
	execute();

	while (fetch())
	{
		userTournMap.insertUserTournData(userId.c_str(), totalTourns.value);
	}

	closeCursor();
}