// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGRrulemain_h_included
#define CGRrulemain_h_included

//******************************************************************************
// Base class for rules
//******************************************************************************
class CGRrulemain sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGRrulemain(CGmain* mainObj_)
		:	mainObj(mainObj_)
	{}

	virtual ~CGRrulemain(void){}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	void generateRule(void);

private:
	void generateDsl(void);

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGRrulemain(const CGRrulemain& other);
	CGRrulemain& operator =(const CGRrulemain& other);
	CGRrulemain(CGRrulemain&& other);
	CGRrulemain& operator =(CGRrulemain&& other);

//==============================================================================
// Data section
//==============================================================================
private:
	CGmain*													mainObj;
};

#endif	// CGRrulemain_h_included
