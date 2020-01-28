// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGlog_h_included
#define CGlog_h_included

#define CreateLog(x,y) CGlog::create(x,y);
#define WriteLog(x) CGlog::write(x);
#define DefineLog CGfile CGlog::cfile;

//******************************************************************************
// Log file handler
//******************************************************************************
class CGlog sealed
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
private:
	CGlog(void)
	{}

public:
	~CGlog()
	{}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	static void create(const char* fileOut, bool rewriteFile)
	{
		if(fileOut)
		{
			if(rewriteFile)
				cfile.open(fileOut);
			else
				cfile.open(fileOut, std::ofstream::app);
		}
	}
	static void write(const string& msg)
	{
		cerr<<msg<<"\n\n";
		cfile.write(msg+"\n\n");
		cfile.flush();
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGlog(const CGlog& other);
	CGlog& operator =(const CGlog& other);
	CGlog(CGlog&& other);
	CGlog& operator =(CGlog&& other);

//==============================================================================
// Static data section
//==============================================================================
private:
	static CGfile		cfile;
};

#endif	// CGlog_h_included

