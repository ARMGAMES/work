// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#ifndef CLPGByteCodeVarData_h_included
#define CLPGByteCodeVarData_h_included

class CLPGByteCodeBlock;
class CLPGByteCodeGen;

//******************************************************************************
// This is the class for a any data 
//******************************************************************************
class CLPGByteCodeVarData sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CLPGByteCodeVarData(UINT64 _pdata, UINT64 _ddata, const char* name)
		:	pdata(_pdata)
		,	ddata(_ddata)
	{}

	CLPGByteCodeVarData(CLPGByteCodeVarData&& other)
		:	pdata(other.pdata)	
		,	ddata(other.ddata)
	{
		other.pdata = 0;
		other.ddata = 0;
	}

	CLPGByteCodeVarData& operator =(CLPGByteCodeVarData&& other)
	{
		pdata = other.pdata;	
		other.pdata = 0;
		ddata = other.ddata;	
		other.ddata = 0;
		return *this;
	}

	~CLPGByteCodeVarData(void) {}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	// Data access
	UINT64 getFirst(void) {return pdata;}
	const UINT64 getFirst(void) const {return pdata;}
	// Q-byte offset in PBlock
	const UINT64 getDData(void) const {return ddata;}
	// Var type
	const UINT64 getType(void) const {return pdata & CLPG_BC_VAR_TYPEMASK;}
	const UINT64 getSubtype(void) const {return pdata & CLPG_BC_VAR_SUBTYPEMASK;}
	// Data amount
	const UINT64  getDataAmount(void)  const {return CLPG_BC_GET_DATAAMOUNT(pdata);}
	// Is this constant
	const UINT64 isConst(void) const {return pdata & CLPG_BC_VAR_CONSTDATA;} 
	// Is internal/external address
	const UINT64 isExternal(void) const {return pdata & CLPG_BC_VAR_EXTERNAL;} 

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CLPGByteCodeVarData(const CLPGByteCodeVarData& other);
	CLPGByteCodeVarData& operator =(const CLPGByteCodeVarData& other);

//==============================================================================
// Data section
//==============================================================================
private:
	UINT64			pdata;		// First Q-byte
	UINT64			ddata;		// Q-byte offset in data
};

#endif	// CLPGByteCodeVarData_h_included
