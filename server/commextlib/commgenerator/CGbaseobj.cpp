// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"

//******************************************************************************
// CGbaseobj definitions
//******************************************************************************
// Add formatted txt
//******************************************************************************
void CGbaseobj::setFormattedTxt(const char* input, bool eraseFirstWhites)
{
	txt = input;
	if(*(txt.begin()) == '\"')
	{
		txt.erase(txt.begin());
	}
	if(*(txt.end()-1) == '\"')
	{
		txt.erase(txt.end()-1);
	}
	if(eraseFirstWhites)
	{
		while(*(txt.begin()) == ' ')
			txt.erase(txt.begin());
	}

	for(auto it=txt.begin(); it!=txt.end(); ++it)
	{
		if(*it == '\\')
		{
			if(*(it+1)== 'n')
			{
				it = txt.erase(it);
				*it = '\n';
			}
			else if(*(it+1)== 't')
			{
				it = txt.erase(it);
				*it = '\t';
			}
			else if(*(it+1)== 'r')
			{
				it = txt.erase(it);
				*it = '\r';
			}
			else if(*(it+1)== 'f')
			{
				it = txt.erase(it);
				*it = '\f';
			}
			else if(*(it+1)== 'b')
			{
				it = txt.erase(it);
				*it = '\b';
			}
			else if(*(it+1)== '\"')
			{
				it = txt.erase(it);
				*it = '\"';
			}
			else if(*(it+1)== '\'')
			{
				it = txt.erase(it);
				*it = '\'';
			}
			else if(*(it+1)== '\\')
			{
				it = txt.erase(it);
				*it = '\\';
			}
		}
		else if(*it == '\r')
		{
			if(*(it+1)== '\n')
				it = txt.erase(it);
			else
				*it = '\n';
		}
	};

	txt.push_back('\n');
}


void CGbaseparsecompose::addSecUpdate(const char* str, CGloader* loader)
{
	CGmain* mainObj = loader->getMainObj();
	const map<string, string>* psec = mainObj->findIniSection("DEFAULT");
	if(psec)
	{
		const map<string, string>& nameDefs = *psec;
		auto res = nameDefs.find(str);
		if(res != end(nameDefs))
		{
			secUpdSets.insert(stol(res->second));
		}
	}
	else
	{
		throw exception(string("Security update value '").append(str).append("' is not defined").c_str());
	}
}

void CGbaseparsecompose::addSecUpdateAfter(const char* str, CGloader* loader)
{
	CGmain* mainObj = loader->getMainObj();
	const map<string, string>* psec = mainObj->findIniSection("DEFAULT");
	if(psec)
	{
		const map<string, string>& nameDefs = *psec;
		auto res = nameDefs.find(str);
		if(res != end(nameDefs))
		{
			secUpdSetsAfter.insert(stol(res->second));
		}
	}
	else
	{
		throw exception(string("Security update value '").append(str).append("' is not defined").c_str());
	}
}


