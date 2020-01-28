// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGmain_h_included
#define CGmain_h_included

//******************************************************************************
// This is the main class for all input documents and generated data
//******************************************************************************
class CGmain sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGmain(void)
		:	finishOK(true)
	{
		//logstr = "printf";
	}

	~CGmain(void){}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	const map<string, string>* findIniSection(const string& name) const
	{
		auto it = iniInfo.find(name);
		if(it != end(iniInfo))
			return &(it->second);
		else
			return nullptr;
	}
	void deleteIniSection(const string& name)
	{
		iniInfo.erase(name);
	}
	const CGbaseobj* findObject(objTypes type_, const string& name_) const
	{
		for(auto& elem : documents)
		{
			const CGbaseobj* obj = elem.second.findObject(type_, name_);
			if(obj) return obj;
		}
		return nullptr;
	}
	const CGstruct* findClass(uint64_t index) const
	{
		for(auto& elem : documents)
		{
			const CGstruct* obj = elem.second.findClass(index);
			if(obj) return obj;
		}
		return nullptr;
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGmain(const CGmain& other);
	CGmain& operator =(const CGmain& other);
	CGmain(CGmain&& other);
	CGmain& operator =(CGmain&& other);

//==============================================================================
// Data section
//==============================================================================
public:
	map<string, CGdocument>												documents;			// document files loaded
	map<string, set<string>>											usedNameOfClass;
	map<string, pair<map<string, tuple<bool, bool, bool>>, uint64_t>>	baseClasses;		// base class name, channel names, is smart, parse, compose, properties
	map<string, unique_ptr<CGplugin>>									plugins;
	map<string, map<string, string>>									iniInfo;			// section, field, value
	string																outdir;
	//string																logstr;
	bool																finishOK;
	string																baseRefClass;		// base reference class for "composeSelectiveFields"
};

#endif	// CGmain_h_included
