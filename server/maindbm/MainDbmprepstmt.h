#pragma once

#include "dbmstmt.h"
#include "dbm.h"
#include "maindbmstruct.h"

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

/////////////////////////////////////////////////////////////////////////////////////

