// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGwinstatplugin_h_included
#define CGwinstatplugin_h_included

#ifdef _WIN32

//******************************************************************************
// Class for windows static plug-in
//******************************************************************************
class CGwinstatplugin sealed : public CGplugin
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGwinstatplugin(const string& _plugname)
		:	CGplugin(_plugname, "static")
	{}

	CGwinstatplugin(CGwinstatplugin&& other)
		:	CGplugin(move(other))
	{}

	CGwinstatplugin& operator =(CGwinstatplugin&& other)
	{
		CGplugin::operator = (move(other));
		return *this;
	}

	virtual ~CGwinstatplugin()
	{
	}

//==============================================================================
// Defined interface section
//==============================================================================
protected:
	virtual void load(void)
	{
		plgPrc = reinterpret_cast<PluginProc>(reinterpret_cast<void*>(GetProcAddress(GetModuleHandle(NULL), plugname.c_str())));
		if (!plgPrc) 
			throw exception(string("Cannot find main function in the static plugin '").append(plugname).append("'").c_str());
	}
	virtual void unload(void)
	{}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGwinstatplugin(const CGwinstatplugin& other);
	CGwinstatplugin& operator =(const CGwinstatplugin& other);
};

#endif	// _WIN32
#endif	// CGwinstatplugin_h_included

