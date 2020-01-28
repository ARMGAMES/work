// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJjavascriptmain_h_included
#define CGJjavascriptmain_h_included

#ifdef _WIN32
//#pragma warning(disable:4503)
#endif // _WIN32

//******************************************************************************
// This is the main class for javascript creation 
//******************************************************************************
class CGJSjavascriptmain sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGJSjavascriptmain(CGmain* mainObj_)
		:	mainObj(mainObj_)
	{}

	virtual ~CGJSjavascriptmain(void){}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	void createJavaScriptFiles(void);

private:
	void createJavaScriptClass(const string& nameDoc, const CGbaseobj* p, const vector<string>& txts);
	void fill(const CGfield* fld, CGJSbasecls* bcls);
	void clear(void);
	void fillSpecial1(const CGstruct* cls, CGfile& cfile);

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGJSjavascriptmain(const CGJSjavascriptmain& other);
	CGJSjavascriptmain& operator =(const CGJSjavascriptmain& other);
	CGJSjavascriptmain(CGJSjavascriptmain&& other);
	CGJSjavascriptmain& operator =(CGJSjavascriptmain&& other);

//==============================================================================
// Data section
//==============================================================================
private:
	CGmain*									mainObj;

	set<string>								gImport;
	vector<vector<string>>					gFields;
	vector<vector<string>>					gSubscrParse;	// field name, field strings
	vector<vector<string>>					gSubscr;
	map<int, string>						gRemoveCont;
	vector<vector<string>>					gParse;	
	vector<vector<string>>					gParseFull;	
	vector<vector<string>>					gGetterSetter;	
	string									gObjectId;
	string									gVersionId;
	vector<vector<string>>					gCompose;	
};

#endif	// CGJjavascriptmain_h_included
