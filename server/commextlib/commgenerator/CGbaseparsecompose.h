// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGbaseparsecompose_h_included
#define CGbaseparsecompose_h_included

//******************************************************************************
// Base class for for all elements (channels part)
//******************************************************************************
class CGbaseparsecompose abstract
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGbaseparsecompose( uint64_t _properties)
		:	properties(_properties)
	{}

	CGbaseparsecompose( uint64_t _properties, const map<string, tuple<bool,bool,bool>>& _pcChannels, const string& _updHandler)
		:	pcChannels(_pcChannels)
		,	properties(_properties)
		,	updHandler(_updHandler)
	{
		java_properties = 0;
	}

	CGbaseparsecompose(CGbaseparsecompose&& other)
		:	pcChannels(move(other.pcChannels))
		,	properties(other.properties)
		,	updHandler(move(other.updHandler))
		,	updSecurityFunc(move(other.updSecurityFunc))
		,	secUpdSets(move(other.secUpdSets))
		,	updSecurityFuncAfter(move(other.updSecurityFuncAfter))
		,	secUpdSetsAfter(move(other.secUpdSetsAfter))
		,	updHandler2(move(other.updHandler2))
		, java_properties(other.java_properties)
	{
		other.properties = 0;
	}

	CGbaseparsecompose& operator =(CGbaseparsecompose&& other)
	{
		pcChannels = move(other.pcChannels);
		updHandler = move(other.updHandler);
		properties = other.properties;
		other.properties = 0;
		updSecurityFunc = move(other.updSecurityFunc);
		secUpdSets = move(other.secUpdSets);
		updSecurityFuncAfter = move(other.updSecurityFuncAfter);
		secUpdSetsAfter = move(other.secUpdSetsAfter);
		updHandler2 = move(other.updHandler2);
		java_properties = other.java_properties;
		return *this;
	}

	virtual ~CGbaseparsecompose()
	{}

///==============================================================================
// Parser interface section
//==============================================================================
public:
	void addParse(const char* name)
	{
		auto ret = pcChannels.insert(make_pair(name, make_tuple(false, true, false)));
		if(!ret.second)
		{
			std::get<1>(ret.first->second) = true;
			std::get<2>(ret.first->second) = false;
		}
	}
	void addCompose(const char* name)
	{
		auto ret = pcChannels.insert(make_pair(name, make_tuple(false, false, true)));
		if(!ret.second)
		{
			std::get<1>(ret.first->second) = false;
			std::get<2>(ret.first->second) = true;
		}
	}
	void addOff(const char* name)
	{
		auto ret = pcChannels.insert(make_pair(name, make_tuple(false, false, false)));
		if(!ret.second)
		{
			std::get<1>(ret.first->second) = false;
			std::get<2>(ret.first->second) = false;
		}
	}
	void addParseCompose(const char* name)
	{
		auto ret = pcChannels.insert(make_pair(name, make_tuple(false, true, true)));
		if(!ret.second)
		{
			std::get<1>(ret.first->second) = true;
			std::get<2>(ret.first->second) = true;
		}
	}
	void setProperty(uint64_t mask, uint64_t data)
	{
		properties &= ~mask;
		properties |= data;
	}
	void setJavaProperty(uint64_t mask, uint64_t data)
	{
		java_properties &= ~mask;
		java_properties |= data;
	}
	void setRestrString(const char* str)
	{
		uint64_t val = stol(str);
		if(val > 15)
		{
			WriteLog("Cannot set string restriction, max value is 15 - ignored");
		}
		else
		{
			setProperty(CG_CLASS_RESTR_STR_MASK, (val << CG_CLASS_RESTR_STR_SHIFT));
		}
	}
	void setUpdateHandler(const char* str)
	{
		updHandler = str;
	}
	void addSecUpdate(const char* str, CGloader* loader);
	void addSecUpdateAfter(const char* str, CGloader* loader);
	void addSecUpdateInt(const char* str)
	{
		secUpdSets.insert(stol(str));
	}
	void addSecUpdateIntAfter(const char* str)
	{
		secUpdSetsAfter.insert(stol(str));
	}
	void setUpdateSecurityFunc(const char* str)
	{
		updSecurityFunc = str;
	}
	void setUpdateSecurityFuncAfter(const char* str)
	{
		updSecurityFuncAfter = str;
	}
	void setUpdateHandler2(const char* str)
	{
		updHandler2 = str;
	}
//==============================================================================
// Defined interface section
//==============================================================================
public:
	void addProperty(uint64_t data)
	{
		properties |= data;
	}
	uint64_t getProperty(void) const
	{
		return properties;
	}
	uint64_t getJavaProperty(void) const
	{
		return java_properties;
	}
	bool hasProperty(uint64_t prop) const
	{
		return !!(properties & prop);
	}
	const map<string, tuple<bool,bool,bool>>& getPcChannels(void) const 
	{
		return pcChannels;
	}
	uint64_t getRestrStringValue(void) const
	{
		return ((properties & CG_CLASS_RESTR_STR_MASK) >> CG_CLASS_RESTR_STR_SHIFT) * 1024;
	}
	void setNull(void)
	{
		pcChannels.clear();
		properties = 0;
	}
	const string& getUpdateHandler(void) const
	{
		return updHandler;
	}
	const set<uint64_t>& getUpdateSecCodes(void) const
	{
		return secUpdSets;
	}
	const string& getUpdateSecurityFunc(void) const
	{
		return updSecurityFunc;
	}
	const set<uint64_t>& getUpdateSecCodesAfter(void) const
	{
		return secUpdSetsAfter;
	}
	const string& getUpdateSecurityFuncAfter(void) const
	{
		return updSecurityFuncAfter;
	}
	const string& getUpdateHandler2(void) const
	{
		return updHandler2;
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGbaseparsecompose(const CGbaseparsecompose& other);
	CGbaseparsecompose& operator =(const CGbaseparsecompose& other);

//==============================================================================
// Data section
//==============================================================================
protected:
	map<string, tuple<bool,bool,bool>>	pcChannels;		// channel, smart, parse, compose
	uint64_t							properties;		// info about class functions(constructors, operators, etc)
	string								updHandler;		// update parameter handler name
	string								updSecurityFunc;// security update function
	set<uint64_t>						secUpdSets;		// security update settings
	string								updSecurityFuncAfter;// security update function
	set<uint64_t>						secUpdSetsAfter;		// security update settings
	string								updHandler2;	// update parameter handler name type 2 with return obj 
	uint64_t							java_properties;		// info about class functions(constructors, operators, etc)
};

#endif	// CGbaseparsecompose_h_included

