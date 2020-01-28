// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJjavamain_h_included
#define CGJjavamain_h_included

#ifdef _WIN32
//#pragma warning(disable:4503)
#endif // _WIN32

//******************************************************************************
// This is the main class for java creation 
//******************************************************************************
class CGJjavamain sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGJjavamain(CGmain* mainObj_)
		:	mainObj(mainObj_)
	{}

	virtual ~CGJjavamain(void){}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	void createJavaFiles(void);

private:
	void createJavaClass(const string& nameDoc, const CGbaseobj* p);
	vector<string> fill(const CGfield* fld, CGJbasecls* bcls);
	void clear(void);

	void createJavaQuery(const string& nameDoc, const CGbaseobj* p);
	void fillQuery(CGfield* fld, CGJbasecls* bcls);
//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGJjavamain(const CGJjavamain& other) = delete;
	CGJjavamain& operator =(const CGJjavamain& other)=delete;
	CGJjavamain(CGJjavamain&& other) = delete;
	CGJjavamain& operator =(CGJjavamain&& other) = delete;

//==============================================================================
// Data section
//==============================================================================
private:
	CGmain*									mainObj;

	set<string>								gImport;
	vector<string>							gFields;
	vector<vector<string>>					gSetters;
	vector<vector<string>>					gGetters;
	vector<vector<string>>					gParse;	// field name, field strings
	vector<vector<string>>					gCompose;

};

#endif	// CGJjavamain_h_included
