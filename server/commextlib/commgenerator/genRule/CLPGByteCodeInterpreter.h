// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#ifndef CLPGByteCodeInterpreter_h_included
#define CLPGByteCodeInterpreter_h_included

#include "CLPGByteCodeDefs.h"


enum RuleRetCode
{
	ruleOk,ruleFailed,ruleStop
};


class CECase;
class CESRequest;
class CETag;
class CEQueue;
class CommExtObject;

//******************************************************************************
// Stack storage class
//******************************************************************************
class CLPGByteCodeStack
{
//==============================================================================
// Defined interface section
//==============================================================================
public:
	UINT64 size(void) const {return storage.size();}
	bool empty(void) const {return storage.empty();}
	void clear(void) {storage.clear();}
	UINT64 top(void) const {return (!storage.empty())?storage.back():CLPG_BC_GEN_UNDEF;}
	void push(UINT64 val) {storage.push_back(val);}
	void pop(void) {storage.pop_back();}
	UINT64 get(void) {UINT64 val = (!storage.empty())?storage.back():CLPG_BC_GEN_UNDEF; storage.pop_back(); return val;}

//==============================================================================
// Data section
//==============================================================================
private:
	deque<UINT64>			storage;	// Stack body
};

//******************************************************************************
// This is the class executing interpretation 
//******************************************************************************
class CLPGByteCodeInterpreter
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CLPGByteCodeInterpreter(void)
	:	CS(NULL)
	,	DS(NULL)
	,	SS(NULL)
	,	IP(NULL)
	,	BS(NULL)
	,	CSize(0)
	{}
	virtual ~CLPGByteCodeInterpreter(void)
	{clean();}

//==============================================================================
// Virtual interface section
//==============================================================================
public:
	virtual CommExtObject* initObj(const char* name) = 0;
	virtual RuleRetCode callFunction(const char* name, vector<CLPGByteCodeDATATYPE>& params, CLPGByteCodeDATATYPE& ret, PString& errorMsg) = 0;

//==============================================================================
// Defined interface section
//==============================================================================
public:
	// Init
	void init(const BYTE* data, UINT64 datasize);
	// Execute code
	RuleRetCode  exec(bool ignore, PString& errorMsg);
	// Clean data 
	void clean(void);

private:
	// Execute one command
	RuleRetCode command(PString& errorMsg);

	// Stack and memory
	void commandPop(void);						// delete one data from stack
	void commandClear(void);					// erase stack data
	void commandCopyData(void);					// copy data to stack
	void commandCopyRef(void);					// copy data by reference to stack
	void commandDataAlloc(void);				// data alloc

	// Unary math
	void commandDoublePlusMinus(bool minus);	// ++/--
	void commandSignMinus(void);				// - for sign

	// Binary math
	void commandAssignment(void);				// assignment
	void commandBinaryMath(UINT64 code);		// +, -, *, /, &, ^, | 
	void commandBinaryComp(UINT64 code);		// ==, !=, >, <, >=, <= 
	void commandBinaryLogic(UINT64 code);		// &&, || 
	void commandStringPlus(void);				// + for strings
	void commandStringComp(UINT64 code);		// ==, != for strings
	void commandObjComp(UINT64 code);			// ==, != for objects

	// Triple math

	// Access
	void commandObjField(void);					// class field
	void commandObjFieldIdx(void);				// class field vector index
	void commandObjFieldSize(void);				// class field vector size
	void commandObjInit(void);					// standard class object init

	// Operators
	void commandGoto(UINT64 code);				// 0- goto, 1-if false, 2-if true

	// Functions
	void commandPrintf(void);					// log output
	RuleRetCode commandCall(PString& errorMsg);		// external function call

//==============================================================================
// Data section
//==============================================================================
private:
	CLPGByteCodeStack					stack;		// Data stack
	PBlock								dBlock;		// Data
	const UINT64*						CS;			// Code segment address
	UINT64*								DS;			// Data segment address
	const char*							SS;			// Const strings segment address
	const UINT64*						IP;			// Current command address
	const UINT64*						BS;			// Current block address
	UINT64								CSize;		// Data segment size

};

#endif	// CLPGByteCodeInterpreter_h_included
