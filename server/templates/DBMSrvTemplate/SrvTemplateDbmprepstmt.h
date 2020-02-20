#pragma once

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

/////////////////////////////////////////////////////////////////////////////////////

class SampleTestingStmt : public DbmStatement
{
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					objectId;

	void prepareStmt();

public:
	SampleTestingStmt(DatabaseManagerCommon& man) : DbmStatement(man)
	{
		prepareStmt();
	}
	void init(const char* name, UINT64 object_id);
};


