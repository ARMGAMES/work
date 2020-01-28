// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGwindynplugin_h_included
#define CGwindynplugin_h_included

#ifdef _WIN32

#include "windows.h"

//******************************************************************************
// Class for windows dynamic plug-in
//******************************************************************************
class CGwindynplugin sealed : public CGplugin
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGwindynplugin(const string& _plugname)
		:	CGplugin(_plugname, "dynamic")
		,	hInst(nullptr)
	{}

	CGwindynplugin(CGwindynplugin&& other)
		:	CGplugin(move(other))
		,	hInst(other.hInst)
	{}

	CGwindynplugin& operator =(CGwindynplugin&& other)
	{
		CGplugin::operator = (move(other));
		hInst = other.hInst;
		other.hInst = nullptr;
		return *this;
	}

	virtual ~CGwindynplugin()
	{
		unload();
	}

//==============================================================================
// Defined interface section
//==============================================================================
protected:
	virtual void load(void)
	{
		hInst = LoadLibrary(string(plugname).append(PLUGIN_DYN_EXT).c_str());
		if (!hInst) 
			throw exception(string("Cannot load dynamic plugin '").append(plugname).append("'").c_str());
		plgPrc = reinterpret_cast<PluginProc>(reinterpret_cast<void*>(GetProcAddress(hInst, plugname.c_str())));
		if (!plgPrc) 
			throw exception(string("Cannot find main function in the dynamic plugin '").append(plugname).append("'").c_str());
	}
	virtual void unload(void)
	{
		if(hInst) FreeLibrary(hInst);
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGwindynplugin(const CGwindynplugin& other);
	CGwindynplugin& operator =(const CGwindynplugin& other);

//==============================================================================
// Data section
//==============================================================================
private:
	HINSTANCE						hInst;
};

#endif	// _WIN32
#endif	// CGwindynplugin_h_included
