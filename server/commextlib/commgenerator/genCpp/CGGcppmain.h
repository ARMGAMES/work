// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGcppmain_h_included
#define CGGcppmain_h_included

#ifdef _WIN32
#pragma warning(disable:4503)
#endif // _WIN32

//******************************************************************************
// This is the main class for cpp creation 
//******************************************************************************
class CGGcppmain sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGGcppmain(CGmain* mainObj_)
		:	mainObj(mainObj_)
	{}

	virtual ~CGGcppmain(void){}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	void createCppFiles(void);

private:
	void createCppDocument(const string& nameDoc, const CGdocument& doc);
	void createBaseClass(const pair<string, pair<map<string, tuple<bool, bool, bool>>, uint64_t>>& bcls);
	void createFieldNumAccess(void);
	void createCppClass(const string& nameDoc, const CGbaseobj* p, CGfile& cfile);
	void fill(const CGfield* fld, const CGstruct* cls, set<string>& decls);
	void clear(void);
	void writeAccess(string& curAccess, const string& acc, CGfile& cfile)
	{
		if(curAccess != acc)
		{
			curAccess = acc;
			cfile.write(curAccess+":\n");
		}
	}
	void writeDefConstructor(const CGstruct* cls, CGfile& cfile);
	void writeDestructor(const CGstruct* cls, CGfile& cfile);
	void writeCopyConstructor(const CGstruct* cls, CGfile& cfile);
	void writeMovConstructor(const CGstruct* cls, CGfile& cfile);
	void writeAssignment(const CGstruct* cls, CGfile& cfile);
	void writeMovAssignment(const CGstruct* cls, CGfile& cfile);
	void writeForbidden(const CGstruct* cls, CGfile& cfile);
	void writeCast(const CGstruct* cls, CGfile& cfile);
	void writeCopy(const CGstruct* cls, CGfile& cfile);
	void writeMove(const CGstruct* cls, CGfile& cfile);
	void writeCompare(const CGstruct* cls, CGfile& cfile);
	void writeGetters(CGfile& cfile);
	void writeSetters(CGfile& cfile);
	void writeMovSetters(CGfile& cfile);
	void writeParseCompose(const CGstruct* cls, CGfile& cfile);
	void writeField(const CGstruct* cls, const CGfield* fld, CGfile& cfile);
	void writeSubscription(const CGstruct* cls, CGfile& cfile);
	void writePublish(const CGstruct* cls, CGfile& cfile);
	void writePublishUpdate(const CGstruct* cls, CGfile& cfile);
	void fillSpecial1(const CGstruct* cls, CGfile& cfile);
	void writeSelField1(const CGstruct* cls, CGfile& cfile);
	void writeSelField2(const CGstruct* cls, CGfile& cfile);
	void writeUpdate(const CGstruct* cls, CGfile& cfile);

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGGcppmain(const CGGcppmain& other);
	CGGcppmain& operator =(const CGGcppmain& other);
	CGGcppmain(CGGcppmain&& other);
	CGGcppmain& operator =(CGGcppmain&& other);

//==============================================================================
// Data section
//==============================================================================
private:
	CGmain*													mainObj;

	vector<string>											gDefaultConctructorHeader;
	vector<vector<string>>									gDefaultConctructorBody;
	vector<vector<string>>									gDestructor;
	vector<string>											gCopyConctructorHeader;
	vector<vector<string>>									gCopyConctructorBody;
	vector<string>											gMoveConctructorHeader;
	vector<vector<string>>									gMoveConctructorBody;
	vector<vector<string>>									gAssignment;
	vector<vector<string>>									gMoveAssignment;
	vector<vector<string>>									gEqual;
	vector<vector<string>>									gSetters;
	vector<vector<string>>									gMoveSetters;
	vector<vector<string>>									gGetters;
	map<string, vector<pair<string, vector<string>>>>		gParse;	// channel, field name, field strings
	map<string, vector<pair<string, vector<string>>>>		gCompose;
	vector<vector<string>>									gPublisher;
	vector<vector<string>>									gSubscrSetters;
	vector<vector<string>>									gSubscrUpdateAll;
	vector<vector<string>>									gSpecials;
	vector<vector<string>>									gSelFields;
	vector<pair<string, vector<string>>>					gSelConvertCompose;
	vector<vector<string>>									gUpdate;
	vector<vector<string>>									gSecUpdateAdd;
	vector<vector<string>>									gAutoUpdate;
};

#endif	// CGGcppmain_h_included
