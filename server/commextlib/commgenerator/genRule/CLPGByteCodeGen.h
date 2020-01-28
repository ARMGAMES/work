// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

#ifndef CLPGByteCodeGen_h_included
#define CLPGByteCodeGen_h_included

struct FuncElement
{
	UINT64 type;
	vector<UINT64> params;
};


//******************************************************************************
// This is the class for rule byte code generation
//******************************************************************************
class CLPGByteCodeGen sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CLPGByteCodeGen(CGmain* mainObj_)
	:	mainObj(mainObj_)
	,	extcounter(CLPG_BC_VAR_OBJ_BASECLASS)	// reserved for basic
	,	labelcounter(0)
	,	datacounter(0)
	{
		errors = 0;
		BYTE* p = stringsdata.append(sizeof(UINT64));
		memset(p, 0, sizeof(UINT64));
	}
	~CLPGByteCodeGen(void) {}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	// Collect rule byte code information
	UINT64 generate(const char* ruleFile);
	// Get main doc
	CGmain* getMainDoc(void) {return mainObj;}
	// Generate label
	void generateLabel(string& l)
	{
		l.clear();
		l.append(to_string(++labelcounter));
	}

	// Reserve data
	UINT64 reserveData(UINT64 amount)
	{
		UINT64 d = datacounter;
		datacounter +=amount;
		return d;
	}

	// Alloc in code pool in UINT64 cells
	UINT64* allocCode(UINT64& addr, UINT64 n = 1)
	{
		BYTE* b = cdata.append(sizeof(UINT64) * n);
		if(!b)	throw exception("Error in append to memory block, function 'allocCode', class 'CLPGByteCodeGen'");
		UINT64* p = reinterpret_cast<UINT64*>(b);
		memset(p, 0, sizeof(UINT64) * n);
		addr = (b - cdata.ptr()) / sizeof(UINT64);
		return p;
	};
	// Data access
	UINT64* getFirst(UINT64 pdata) {return reinterpret_cast<UINT64*>(cdata.ptr()) + pdata;}
	//UINT64* getNext(UINT64 pdata, UINT64 n = 1) {return reinterpret_cast<UINT64*>(cdata.ptr()) + pdata + n;}
	const UINT64* getFirst(UINT64 pdata) const {return reinterpret_cast<const UINT64*>(cdata.ptr()) + pdata;}
	//const UINT64* getNext(UINT64 pdata, UINT64 n = 1) const {return reinterpret_cast<const UINT64*>(cdata.ptr()) + pdata + n;}
	// Data access by mask
	const UINT64 getFirstData(UINT64 pdata, UINT64 mask) const
	{
		const UINT64* first = getFirst(pdata);
		return *first & mask;
	}
	void setFirstData(UINT64 pdata, UINT64 t, UINT64 mask) 
	{
		UINT64* first = getFirst(pdata);
		*first &= ~mask; 
		*first += t;
	}

	// Data sort
	const UINT64 getSort(UINT64 pdata) const {return getFirstData(pdata, CLPG_BC_GEN_SORTMASK);}
	UINT64 setSort(UINT64 pdata, UINT64 s) {setFirstData(pdata, s, CLPG_BC_GEN_SORTMASK);}

	// Data work for values
	// Var type
	const UINT64 getType(UINT64 pdata) const {return getFirstData(pdata, CLPG_BC_VAR_TYPEMASK);}
	const UINT64 getSubtype(UINT64 pdata) const {return getFirstData(pdata, CLPG_BC_VAR_SUBTYPEMASK);}
	void setType(UINT64 pdata, UINT64 t) {setFirstData(pdata, t, CLPG_BC_VAR_TYPEMASK);}  
	void setSubtype(UINT64 pdata, UINT64 t) {setFirstData(pdata, t, CLPG_BC_VAR_SUBTYPEMASK);} 
	// Data amount
	const UINT64  getDataAmount(UINT64 pdata)  const {return getFirstData(pdata, CLPG_BC_VAR_DATAAMOUNTMASK>>32);} 
	void  setDataAmount(UINT64 pdata, UINT64 t)  {setFirstData(pdata, t<<32, CLPG_BC_VAR_DATAAMOUNTMASK);} 
	// Defines const value
	void setConst(UINT64 pdata) {setFirstData(pdata, CLPG_BC_VAR_CONSTDATA, CLPG_BC_VAR_CONSTDATA);} 
	// Defines internal/external address
	void setExternal(UINT64 pdata) {setFirstData(pdata, CLPG_BC_VAR_EXTERNAL, CLPG_BC_VAR_EXTERNAL);}  
	// Is this constant
	const UINT64 isConst(UINT64 pdata) const {return getFirstData(pdata, CLPG_BC_VAR_CONSTDATA);} 
	// Is internal/external address
	const UINT64 isExternal(UINT64 pdata) const  {return getFirstData(pdata, CLPG_BC_VAR_EXTERNAL);} 

	// Put command to code
	UINT64* setCommand(UINT64 command)
	{
		UINT64 d;
		UINT64* p = allocCode(d);
		*p = command;
		return p;
	}
	// Put reference to code
	UINT64* setVarRef(CLPGByteCodeVarData* obj)
	{
		UINT64 d;
		UINT64* p = allocCode(d);
		*p = CLPG_BC_GEN_REFERENCE;
		*p |= obj->getDData() & CLPG_BC_VAR_SUBTYPEMASK;
		return p;
	}

	// Fill first byte for type
	UINT64 makeFirst(const char* type, UINT64& dataamount, bool isConst);

	// External object find
	UINT64 findExternalObj(const char* type);
	// External object get
	UINT64 getExternalObj(const char* type);

	// Const string find
	UINT64 findConstString(const char* str);
	// Const string get
	UINT64 getConstString(const char* str, bool correct);

	// Add label
	void addLabel(const string& name);
	// Find label
	bool findLabel(const string& name, UINT64& addr);
	// Goto label
	void gotoLabel(const string& name, UINT64 cond = 0); // 0- goto, 1-if false, 2-if true
	// Final check unreferenced goto's
	void gotoCheck(void);
	// Check data for type
	bool checkDataType(UINT64 d, UINT64 type);

	// Function declaration
	void funcDecl(const char* name, const char* type, vector<UINT64>& pars);
	// Function
	UINT64 funcRun(const char* name, vector<UINT64>& pars, UINT64 count);

//==============================================================================
// Static section
//==============================================================================
public:
	// Set error
	static void setError(void) {++errors;}
	// Get error counter
	static UINT64 getErrors(void) {return errors;}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CLPGByteCodeGen(const CLPGByteCodeGen& other);
	CLPGByteCodeGen& operator =(const CLPGByteCodeGen& other);

//==============================================================================
// Static data section
//==============================================================================
private:
	static UINT64			errors;			// error counter

//==============================================================================
// Data section
//==============================================================================
private:
	CGmain*						mainObj;			// main document
	PBlock						cdata;			// code storage
	UINT64					datacounter;	// index counter for data

	map<string, UINT64>		labels;			// map of labels
	multimap<string, UINT64>	labelRefs;		// map of labels forward references
	UINT64					labelcounter;	// index counter for generated labels

	PBlock						stringsdata;	// const strings
	map<string, UINT64>		stringsmap;		// map of constant strings

	UINT64					extcounter;		// index counter for external types
	map<string, UINT64>		externals;		// indexes for external types

	map<string, FuncElement>	functions;		// declared functions
};

#endif	// CLPGByteCodeGen_h_included
