// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGfile_h_included
#define CGfile_h_included

//******************************************************************************
// File wrapper
//******************************************************************************
class CGfile : public ofstream
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGfile()
	{}
	CGfile(CGfile&& other)
		:	ofstream(std::move(other))
		,	name(std::move(other.name))
	{}
	CGfile& operator =(CGfile&& other)
	{
		ofstream::operator = (std::move(other));
		name = std::move(other.name);
		return *this;
	}
public:
	virtual ~CGfile()
	{}

//==============================================================================
// Defined interface section
//==============================================================================
	void open(const string& name_, ios_base::openmode mode = 0)
	{
		ofstream::open(name_, std::ofstream::out | mode);
		if(!is_open())
			throw exception(string("Cannot open file ").append(name_).c_str());
		name = name_;
	}
	void write(const string& str)
	{
		if(!is_open())
			throw exception(string("Writing to unopened file ").append(name).c_str());
		ofstream::write(str.c_str(), str.length());
	}
	void write(string& str, bool clear)
	{
		write(str);		
		if(clear) str.clear();
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGfile(const CGfile& other);
	CGfile& operator =(const CGfile& other);

//==============================================================================
// Data section
//==============================================================================
protected:
	string	name;
};

#endif	// CGfile_h_included
