// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef ColTCADbmprepstmt_h
#define	ColTCADbmprepstmt_h

#include	"dbmstmt.h"
#include	"dbm.h"
#include	"pyrtime.h"

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



#endif	//ColTCADbmprepstmt_h
