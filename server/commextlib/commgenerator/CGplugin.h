// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGplugin_h_included
#define CGplugin_h_included

//******************************************************************************
// Plug-in base support
//******************************************************************************
class CGplugin
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
protected:
	CGplugin(const string& _plugname, const string& _plugtype)
		:	plugname(_plugname)
		,	plugtype(_plugtype)
		,	plgPrc(nullptr)
	{}

public:
	CGplugin(CGplugin&& other)
		:	plugname(move(other.plugname))
		,	plugtype(other.plugtype)
		,	plgPrc(other.plgPrc)
	{
		other.plgPrc = nullptr;
	}

	CGplugin& operator =(CGplugin&& other)
	{
		plugname = (move(other.plugname));
		plugtype = (move(other.plugtype));
		plgPrc = other.plgPrc;
		other.plgPrc = nullptr;
		return *this;
	}

	virtual ~CGplugin()
	{}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	const string& getModuleName(void) const {return plugname;}
	const string& getModuleType(void) const {return plugtype;}
	void exec(CGmain* mainObj)
	{
		load();
		if (plgPrc) 
			plgPrc(mainObj);
		else
			throw exception(string("Main function in the plugin '").append(plugname).append("' not found").c_str());
		unload();
	}

//==============================================================================
// Abstract interface section
//==============================================================================
protected:
	virtual void load(void) abstract;
	virtual void unload(void) abstract;

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGplugin(const CGplugin& other);
	CGplugin& operator =(const CGplugin& other);

//==============================================================================
// Data section
//==============================================================================
protected:
	string							plugname;
	string							plugtype;
	PluginProc						plgPrc; // main plugin function
};

#endif	// CGplugin_h_included
