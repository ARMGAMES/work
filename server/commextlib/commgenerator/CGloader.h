// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGloader_h_included
#define CGloader_h_included

//******************************************************************************
// This is the loader for input information
//******************************************************************************
class CGloader sealed
{
	friend class CGfield;
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGloader(CGmain* mainObj_)
		:	mainObj(mainObj_)
	{
		if(!forbiddenWords.size())
			for(auto i = 0; i < ForbiddenDefinitionsSize; ++i)
				forbiddenWords.insert(ForbiddenDefinitions[i]);
		if(!fieldSubTypes.size())
			for(auto i = 0; i < gsSubFieldTypesSize; ++i)
				fieldSubTypes.insert(make_pair(gsSubFieldTypesData[i].name, gsSubFieldTypesData[i]));
	}

	~CGloader(void){}

//==============================================================================
// Parser interface section
//==============================================================================
public:
	void addIniValue(const string& section, const string& field, const string& value, const char* filename, set<string>* iniloads) 
	{
		if(section == "INILOAD")
		{
			iniloads->insert(field);
		}
		else
		{
			auto sec = mainObj->iniInfo.insert(make_pair(section, map<string, string>())).first;
			auto res = sec->second.insert(make_pair(field, value)).second;
			if(!res)
				WriteLog(string("INI file '").append(filename).append("' section '").append(section).append("' has doubled item '").append(field).append("' - ignored"));
		}
	}
	void addContainersData(gsSubFieldTypes& ftypes)
	{
		fieldSubTypes.insert(make_pair(ftypes.name, move(ftypes)));
		WriteLog(string("DAT file  section 'CONTAINERS' field '").append(ftypes.name).append("' loaded"));
	}

	// Add global data from files
	void addGlobal(const char* name)
	{
		global.insert(name);
	}
	void addGlobalSmart(const char* name)
	{
		globalSmart.insert(name);
	}
	void addVirtual(const char* name, const char* base)
	{
		auto it = virt.insert(make_pair(base, set<string>()));
		it.first->second.insert(name);
	}
	void addVirtualSmart(const char* name, const char* base)
	{
		auto it = virtSmart.insert(make_pair(base, set<string>()));
		it.first->second.insert(name);
	}
	void addLocal(const char* name, const string& doc)
	{
		auto it = local.insert(make_pair(doc, set<string>()));
		it.first->second.insert(name);
	}
	void addLocalSmart(const char* name, const string& doc)
	{
		auto it = localSmart.insert(make_pair(doc, set<string>()));
		it.first->second.insert(name);
	}
	string correctString(const char* inp)
	{
		string txt = inp;
		if(*(txt.begin()) == '\"')
		{
			txt.erase(txt.begin());
		}
		if(*(txt.end()-1) == '\"')
		{
			txt.erase(txt.end()-1);
		}
		return txt;
	}
	void addBaseRefClass(const char* name)
	{
		if(mainObj->baseRefClass.size())
			throw exception(string("Add base reference class for 'composeSelectiveFields' - already exists: ").append(mainObj->baseRefClass).append(" / ").append(name).c_str());
		mainObj->baseRefClass.assign(name);
	}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	// Main work routine
	bool exec(int argc, char* argv[]);

	// Check classes names
	void checkClassesNames(const char* uClass)
	{
		auto usedClassesRet = mainObj->usedNameOfClass.insert(make_pair(uClass, set<string>()));
		if(!usedClassesRet.second)
			throw exception(string("Double class name used - '").append(uClass).append("'").c_str());
	}
	// Check used words
	void checkIDs(const string& uClass, const string& uWord)
	{
		auto forbiddenIt = forbiddenWords.find(uWord);
		if(forbiddenIt != end(forbiddenWords))
			throw exception(string("Forbidden ID used in class '").append(uClass).append("', ID - '").append(uWord).append("'").c_str());
		auto usedClassesIt = mainObj->usedNameOfClass.find(uClass);
		if(usedClassesIt == end(mainObj->usedNameOfClass))
			throw exception(string("Class '").append(uClass).append("' not found in ID check, ID - '").append(uWord).append("'").c_str());
		auto usedWordsRet = usedClassesIt->second.insert(uWord);
		if(!usedWordsRet.second)
			throw exception(string("Double ID used in class '").append(uClass).append("', ID - '").append(uWord).append("'").c_str());
	}

	const set<string>& getForbiddenWords(void) const {return forbiddenWords;}
	CGmain* getMainObj(void) {return mainObj;}
	const map<string, gsSubFieldTypes>& getFieldSubTypes(void) const {return fieldSubTypes;}

private:
	void loadIniFile(const string& fileName, int& counter);
	void loadDataFile(const string& fileName);
	void loadPlugins(void);
	uint64_t prepareProperties(void);
	void checkDescChannels(void);

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGloader(const CGloader& other);
	CGloader& operator =(const CGloader& other);
	CGloader(CGloader&& other);
	CGloader& operator =(CGloader&& other);

//==============================================================================
// Data section
//==============================================================================
private:
	CGmain*								mainObj;
	set<string>							forbiddenWords;
	map<string, gsSubFieldTypes>		fieldSubTypes;
	set<string>							global;				// global channels
	set<string>							globalSmart;		// global  smart channels
	map<string, set<string>>			virt;				// virtual channels (base class, channel names)
	map<string, set<string>>			virtSmart;			// virtual  smart channels
	map<string, set<string>>			local;				// local channels (document name, channel names)
	map<string, set<string>>			localSmart;			// local  smart channels
};

#endif	// CGloader_h_included
