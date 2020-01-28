////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericPrepStmtTemplates_h_included
#define GenericPrepStmtTemplates_h_included

////////////////////////////////////////////////////////////////////////////////

#include "dbmstmt.h"

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

template<typename T>
class BindingStmtT : public T
{
public:
	explicit BindingStmtT<T>(::DatabaseManagerCommon& man)
		: T(man), isFirstParamBound(false), isFirstColumnBound(false)
	{ }

	bool isFirstParamBound;
	bool isFirstColumnBound;

	template<typename T> void bindParam(T& param)
	{
		if (isFirstParamBound)
		{
			bindNextParam(param);
		}
		else
		{
			bindFirstParam(param);
		}
		isFirstParamBound = true;
	}

	template<typename T> void bindCol(T& param)
	{
		if (isFirstColumnBound)
		{
			bindNextCol(param);
		}
		else
		{
			bindFirstCol(param);
		}
		isFirstColumnBound = true;
	}

private: // Disallow invocation of bindFirst, bindNext, etc.
	using T::bindFirstCol;
	using T::bindNextCol;
	using T::bindFirstParam;
	using T::bindNextParam;
};

////////////////////////////////////////////////////////////////////////////////

template<typename T>
class NoExecBindingStmtT : public NoExecStmtT<T>
{
public:
	NoExecBindingStmtT<T>(::DatabaseManagerCommon& man)
		: NoExecStmtT<T>(man)
		, isFirstParamBound(false), isFirstColumnBound(false)
	{ }

	bool isFirstParamBound;
	bool isFirstColumnBound;

	template<typename T> void bindParam(T& param)
	{
		if (isFirstParamBound)
		{
			bindNextParam(param);
		}
		else
		{
			bindFirstParam(param);
		}
		isFirstParamBound = true;
	}

	template<typename T> void bindCol(T& param)
	{
		if (isFirstColumnBound)
		{
			bindNextCol(param);
		}
		else
		{
			bindFirstCol(param);
		}
		isFirstColumnBound = true;
	}
};

////////////////////////////////////////////////////////////////////////////////

using NoExecStmt = NoExecStmtT<DbmStatement>;
using BindingStmt = BindingStmtT<DbmStatement>;
using NoExecBindingStmt = NoExecBindingStmtT<DbmStatement>;

////////////////////////////////////////////////////////////////////////////////

#endif // GenericPrepStmtTemplates_h_included
