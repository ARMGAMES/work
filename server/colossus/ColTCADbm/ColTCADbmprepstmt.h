// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef ColTCADbmprepstmt_h
#define	ColTCADbmprepstmt_h

#include	"dbmstmt.h"
#include	"dbm.h"
#include    "dbmstruct.h"
#include	"pyrtime.h"
#include	"TCADetector.h"

class ColPairMap;

template<typename T>
class NoExecStmtT : public T
{
public:
	explicit NoExecStmtT<T>(::DatabaseManagerCommon& man) : T(man) {}

protected: // Disallow invocation of execute methods
	using T::execute;
	using T::executeRet;
	using T::execUpdateDelete;
};

typedef NoExecStmtT<DbmStatement> NoExecStmt;

class GetTCAPartnersStmt : public NoExecStmt
{
	// SQL Outputs
	PSqlString<USER_NAME_LEN>	userId;
	PSqlString<USER_NAME_LEN>	partnerId;
	PSqlInt						totalTourns;
	PSqlInt						numAllInConfrontations;

public:
	GetTCAPartnersStmt(DatabaseManagerCommon& man, const PString& query);
	void execGet(UINT32 queryTimeLimitMinutes, UINT32 maxColPairSize, ColPairMap& colPairMap);
};

class GetTCAUserTournStmt : public NoExecStmt
{
	// SQL Outputs
	PSqlString<USER_NAME_LEN>	userId;
	PSqlInt						totalTourns;

public:
	GetTCAUserTournStmt(DatabaseManagerCommon& man, const PString& query);
	void execGet(UINT32 queryTimeLimitMinutes, UserTournDataMap& userTournMap);
};

#endif	//ColTCADbmprepstmt_h
