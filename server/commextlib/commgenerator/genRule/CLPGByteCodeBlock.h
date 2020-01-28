// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#ifndef CLPGByteCodeBlock_h_included
#define CLPGByteCodeBlock_h_included

class CLPGByteCodeGen;
//******************************************************************************
// This is the class for one code block
//******************************************************************************
class CLPGByteCodeBlock sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CLPGByteCodeBlock(CLPGByteCodeBlock* _owner, CLPGByteCodeGen* _gen);
	~CLPGByteCodeBlock(void) {}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	// Q-byte address in PBlock
	const UINT64 getPData(void) const {return pdata;}
	// Byte code generator object
	CLPGByteCodeGen* getCodeGen(void) {return gen;}
	// Block type
	const UINT64 getType(void) const;
	// Address data
	const UINT64 getAddrData(void) const;
	// Size
	const UINT64 getSize(void) const;
	// End block set
	void setEndBlock(void);

public:
	// Find var
	CLPGByteCodeVarData* findVar(const char* varName);
	// Create data object
	void createDataObject(const char* type, const char* name, bool isConst, bool init);


public:
	//  +/- sign, put simple operand in stack
	UINT64 setOperand(const char* varName, bool minus, UINT64 t);
	// ++ 
	UINT64 plusPlus(const char* varName, bool prefix);
	// --
	UINT64 minusMinus(const char* varName, bool prefix);
	// Assignment
	UINT64 assignment(const char* varName, UINT64 t, bool useconst = false);
	// Fields - first
	UINT64 firstField(const char* varName, const char* fieldName);
	// Fields - index
	UINT64 indexField(UINT64 idx, const char* varName, const char* fieldName);
	// Fields - size
	UINT64 sizeField(const char* varName, const char* fieldName);
	// Fields - next
	UINT64 nextField(const char* fieldName, UINT64 t);
	// Fields - index
	UINT64 indexFieldNext(UINT64 idx, const char* fieldName, UINT64 t);
	// Fields - size
	UINT64 sizeFieldNext(const char* fieldName, UINT64 t);
	// +, -, *, /, &, ^, | 
	UINT64 binaryMath(UINT64 t1, UINT64 t2, UINT64 code);
	// ==, !=, >, <, >=, <=
	UINT64 binaryCompare(UINT64 t1, UINT64 t2, UINT64 code);
	// &&, ||
	UINT64 binaryLogic(UINT64 t1, UINT64 t2, UINT64 code);

//==============================================================================
// Data section
//==============================================================================
private:
	CLPGByteCodeBlock*					owner;			// owner block
	CLPGByteCodeGen*					gen;			// byte code generator object
	UINT64							pdata;			// Q-byte address in PBlock
	map<string, CLPGByteCodeVarData>	vars;			// map of  this block variables

};

#endif	// CLPGByteCodeBlock_h_included
