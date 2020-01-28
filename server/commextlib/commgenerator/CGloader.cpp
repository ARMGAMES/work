// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"
#include "direct.h"
#include "antlr\output\comminifileLexer.h"
#include "antlr\output\comminifileParser.h"
#include "antlr\output\commdataLexer.h"
#include "antlr\output\commdataParser.h"

#ifdef _WIN32
#ifdef NDEBUG
#define loaderStatPlugin(x) \
	extern "C" __declspec(dllexport) void _cdecl x(const CGmain*); \
	void fStatPlugin_##x (void) { x(nullptr); } \
	__pragma (comment(lib, "Release/"#x".lib")) 
#else // NDEBUG
#define loaderStatPlugin(x) \
	extern "C" __declspec(dllexport) void _cdecl x(const CGmain*); \
	void fStatPlugin_##x (void) { x(nullptr); } \
	__pragma (comment(lib, "Debug/"#x".lib")) 
#endif // NDEBUG
#endif // _WIN32

// Static links for plugins
loaderStatPlugin(genCpp)
//loaderStatPlugin(genRule)
loaderStatPlugin(genJavaScript)
loaderStatPlugin(genJava)

//******************************************************************************
// CGLLMain definitions
//******************************************************************************
// Plugins loader
//******************************************************************************
void CGloader::loadPlugins()
{
	// Stat plugins 
	mainObj->plugins.insert(make_pair("genCpp", unique_ptr<CGplugin>(new CGwinstatplugin("genCpp"))));
	//mainObj->plugins.insert(make_pair("genRule", unique_ptr<CGplugin>(new CGwinstatplugin("genRule"))));
	mainObj->plugins.insert(make_pair("genJavaScript", unique_ptr<CGplugin>(new CGwinstatplugin("genJavaScript"))));
	mainObj->plugins.insert(make_pair("genJava", unique_ptr<CGplugin>(new CGwinstatplugin("genJava"))));

	// Dynamic plugins
	auto psec = mainObj->findIniSection("DYNAMIC_PLUGINS");
	if(psec)
	{
		const map<string, string>& namePlugins = *psec;
		for(auto& plg : namePlugins)
		{
			auto res = mainObj->plugins.insert(make_pair(plg.first, unique_ptr<CGplugin>(new CGwindynplugin(plg.first))));
			if(!res.second)
				throw exception(string("Cannot load dynamic plugin '").append(plg.first).append("' because the static plugin with the same name used").c_str());
		}
		mainObj->deleteIniSection("DYNAMIC_PLUGINS");
	}
}

//******************************************************************************
// Main work routine
//******************************************************************************
bool CGloader::exec(int argc, char* argv[])
{
	bool stop = true;
	string	datFile = CG_DATA_FILE_NAME;
	set<string> initFiles;
	int counter = 0;

	// find ini file and input file in command line
	for(auto i = 1; i < argc; ++i)
	{
		string s = argv[i];
		auto pos = s.find_first_of('-');
		if(pos != string::npos)
		{
			if((pos+1) < s.length())
			{
				if(s[pos+1] == 'i')
				{
					pos = s.find_first_of(':', ++pos);
					if((pos == string::npos) || ((pos+1) >= s.length()))
					{
						WriteLog("INI file set but not defined in format -i:name - ignored");
					}
					else
					{
						initFiles.insert(s.substr(pos+1));
					}
				}
				else if(s[pos+1] == 'v')
				{
					pos = s.find_first_of(':', ++pos);
					if((pos == string::npos) || ((pos+1) >= s.length()))
					{
						WriteLog("DAT file set but not defined in format -v:name - ignored");
					}
					else
					{
						datFile = s.substr(pos+1);
					}
				}
			}
		}
		else
		{
			auto pos = s.find_first_of(':');
			if((pos == string::npos) || ((pos+1) >= s.length()))
			{
				WriteLog(string("Parameter set but not defined in format name:parameter '").append(s).append("' - ignored"));
			}
			else
			{
				if(s.substr(0, pos) == "INILOAD")
				{
					WriteLog("Input parameter name 'INILOAD' is reserved - ignored");
				}
				else
				{
					string value, field;
					auto sec = mainObj->iniInfo.insert(make_pair(s.substr(0, pos), map<string, string>())).first;
					auto pos1 = s.find_first_of('=', pos+1);
					if((pos1 != string::npos) && ((pos1+1) < s.length()))
					{
						value = s.substr(pos1+1);
						field = s.substr(pos+1, pos1-pos-1);
					}
					else
					{
						field = s.substr(pos+1);
					}
					auto res = sec->second.insert(make_pair(field, value)).second;
					if(!res)
						WriteLog(string("Input parameter '").append(s).append("' has doubled item - ignored"));
				}
			}
		}
	}

	// Ini files information loading
	// No ini files - use standard
	if(initFiles.empty())
	{
		initFiles.insert(CG_INI_FILE_NAME);
	}

	// Find all ini included
	for(auto& ini : initFiles)
	{
		loadIniFile(ini, counter);
	}

	// Load data file
	loadDataFile(datFile);

	// Prepare class properties
	uint64_t properties = prepareProperties();

	// Load documents
	auto psec = mainObj->findIniSection("INPUT");
	if(psec)
	{
		const map<string, string>& secInput = *psec;
		for(auto& item : secInput)
		{	
			string p;
			CGfile cfile;

			auto z = item.first.find_last_of("\\");
			if (z == string::npos)
				z = item.first.find_last_of("/");
			if (z != string::npos)
				p = item.first.substr(z+1);
			else
				p = item.first;

			z = p.find_first_of(".");
			if (z != string::npos)
			{
				if(item.first.substr(z+1) != CG_STANDARD_INFILE_EXT)
				{
					WriteLog(string("Input file '").append(item.first).append("' has wrong extention - '.").append(item.first.substr(z+1)).append("' must be '"CG_STANDARD_INFILE_EXT"' - ignored"));
				}
				else
				{
					p.resize(z);
					auto pr = mainObj->documents.insert(make_pair(item.first, CGdocument(item.second.c_str(), p.c_str(), properties)));
					if(pr.second)
						pr.first->second.load(this, item.first);
				}
			}
			else
			{
				WriteLog(string("Input file '").append(item.first).append("' has no extention, must be '"CG_STANDARD_INFILE_EXT"' - ignored"));
			}
		}
		mainObj->deleteIniSection("INPUT");
	}
	else
		throw exception("No input documents");

	// Check channels
	checkDescChannels();

	// Input info check
	for(auto& pr : mainObj->documents)
	{
		pr.second.checkChannels1(this);
	}
	for(auto& pr : mainObj->documents)
	{
		pr.second.checkChannels2(this);
	}
	for(auto& pr : mainObj->documents)
	{
		pr.second.checkChannels3(this);
	}

	// Plugins loader
	loadPlugins();

	return stop;
}

void CGloader::loadDataFile(const string& fileName)
{

	bool res = true;
	string errmsg;
	set<string> iniloads;

	// lexer/parser definitions
	pANTLR3_INPUT_STREAM			input = nullptr;    
	pcommdataLexer					lex = nullptr;   
	pANTLR3_COMMON_TOKEN_STREAM		tokens = nullptr;    
	pcommdataParser					parser = nullptr; 

	try
	{
		input = antlr3FileStreamNew((pANTLR3_UINT8)(fileName.c_str()), ANTLR3_ENC_8BIT);  
		if(nullptr == input) 
		{
			string err;
			err.append("Cannot create input stream for file ").append(fileName);
			throw exception(err.c_str());
		}

		lex = commdataLexerNew(input);    
		if(nullptr == lex) 
			throw exception("Cannot create lexer");

		tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));    
		if(nullptr == tokens) 
			throw exception("Cannot create token stream");

		parser = commdataParserNew(tokens);     
		if(nullptr == parser) 
			throw exception("Cannot create parser");

		parser->prog(parser, this);
	}
	catch( exception& err )
	{
		res = false;
		errmsg.append("comminifile: Lexer/parser error, file - ").append(fileName).append("' - ").append(err.what()).append("\n");
	}
	catch( ... )
	{
		res = false;
		errmsg.append("comminifile: Lexer/parser general error, file - '").append(fileName).append("'\n");
	}

	if(parser)	parser ->free(parser);    
	if(tokens)	tokens ->free(tokens);    
	if(lex)		lex    ->free(lex);    
	if(input)	input  ->close(input); 

	if(!res)
		throw exception(errmsg.c_str());
}

void CGloader::loadIniFile(const string& fileName, int& counter)
{
	if(counter > CG_INI_FILE_MAX)
		throw exception(string("INI files amount is bigger than ").append(to_string(CG_INI_FILE_MAX)).append(" - stopped").c_str());
	++counter;

	bool res = true;
	string errmsg;
	set<string> iniloads;

	// lexer/parser definitions
	pANTLR3_INPUT_STREAM			input = nullptr;    
	pcomminifileLexer				lex = nullptr;   
	pANTLR3_COMMON_TOKEN_STREAM		tokens = nullptr;    
	pcomminifileParser				parser = nullptr; 

	try
	{
		input = antlr3FileStreamNew((pANTLR3_UINT8)(fileName.c_str()), ANTLR3_ENC_8BIT);  
		if(nullptr == input) 
		{
			string err;
			err.append("Cannot create input stream for file ").append(fileName);
			throw exception(err.c_str());
		}

		lex = comminifileLexerNew(input);    
		if(nullptr == lex) 
			throw exception("Cannot create lexer");

		tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));    
		if(nullptr == tokens) 
			throw exception("Cannot create token stream");

		parser = comminifileParserNew(tokens);     
		if(nullptr == parser) 
			throw exception("Cannot create parser");

		parser->prog(parser, this, fileName.c_str(), &iniloads);
	}
	catch( exception& err )
	{
		res = false;
		errmsg.append("comminifile: Lexer/parser error, file - ").append(fileName).append("' - ").append(err.what()).append("\n");
	}
	catch( ... )
	{
		res = false;
		errmsg.append("comminifile: Lexer/parser general error, file - '").append(fileName).append("'\n");
	}

	if(parser)	parser ->free(parser);    
	if(tokens)	tokens ->free(tokens);    
	if(lex)		lex    ->free(lex);    
	if(input)	input  ->close(input); 

	if(!res)
		throw exception(errmsg.c_str());

	for(auto& s : iniloads)
	{
		loadIniFile(s, counter);
	}
}

uint64_t CGloader::prepareProperties(void)
{
	uint64_t res = 0;
	auto psec = mainObj->findIniSection("ENTER");
	if(psec)
	{
		const map<string, string>& secEnter = *psec;
		auto it = secEnter.find("equal");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_EQUAL_MASK;
				res |= CG_CLASS_EQUAL_ON;
			}
			else if(it->second == "base")
			{
				res &= ~CG_CLASS_EQUAL_MASK;
				res |= CG_CLASS_EQUAL_ONBASE;
			}
			if(it->second == "onlog")
			{
				res &= ~CG_CLASS_EQUAL_MASK;
				res |= CG_CLASS_EQUAL_ONLOG;
			}
			else if(it->second == "baselog")
			{
				res &= ~CG_CLASS_EQUAL_MASK;
				res |= CG_CLASS_EQUAL_ONBASELOG;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("copyconstructor");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_COPY_MASK;
				res |= CG_CLASS_COPY_ON;
			}
			else if(it->second == "deny")
			{
				res &= ~CG_CLASS_COPY_MASK;
				res |= CG_CLASS_COPY_DENY;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("moveconstructor");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_MOVE_MASK;
				res |= CG_CLASS_MOVE_ON;
			}
			else if(it->second == "deny")
			{
				res &= ~CG_CLASS_MOVE_MASK;
				res |= CG_CLASS_MOVE_DENY;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("assignment");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_ASSIGN_MASK;
				res |= CG_CLASS_ASSIGN_ON;
			}
			else if(it->second == "deny")
			{
				res &= ~CG_CLASS_ASSIGN_MASK;
				res |= CG_CLASS_ASSIGN_DENY;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("moveassignment");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_MASSIGN_MASK;
				res |= CG_CLASS_MASSIGN_ON;
			}
			else if(it->second == "deny")
			{
				res &= ~CG_CLASS_MASSIGN_MASK;
				res |= CG_CLASS_MASSIGN_DENY;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("copybase");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_COPYBASE_MASK;
				res |= CG_CLASS_COPYBASE_ON;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("movebase");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_MOVEBASE_MASK;
				res |= CG_CLASS_MOVEBASE_ON;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("checkparse");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_PARSECHK_MASK;
				res |= CG_CLASS_PARSECHK_ON;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("dump");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_DUMP_MASK;
				res |= CG_CLASS_DUMP_ON;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("restrictstr");
		if(it != end(secEnter))
		{
			uint64_t val = stoll(it->second);
			if(val > 15)
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' cannot be more than 15.").c_str());
			}
			else
			{
				res &= ~CG_CLASS_RESTR_STR_MASK;
				res |= (val << 36);
			}
		}

		it = secEnter.find("include");
		if(it != end(secEnter))
		{
			if(it->second == "std")
			{
				res &= ~CG_CLASS_INK_MASK;
				res |= CG_CLASS_INK_STD;
			}
			else if(it->second == "base")
			{
				res &= ~CG_CLASS_INK_MASK;
				res |= CG_CLASS_INK_BASE;
			}
			else if(it->second == "all")
			{
				res &= ~CG_CLASS_INK_MASK;
				res |= CG_CLASS_INK_ALL;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("validate");
		if(it != end(secEnter))
		{
			if(it->second == "on")
			{
				res &= ~CG_CLASS_VALIDATE_MASK;
				res |= CG_CLASS_VALIDATE_ON;
			}
			else if(it->second == "restore")
			{
				res &= ~CG_CLASS_VALIDATE_MASK;
				res |= CG_CLASS_VALIDATE_REST;
			}
			else
			{
				WriteLog(string("Value '").append(it->second).append("' for field '").append(it->first).append("' is not allowed in 'ENTER' section").c_str());
			}
		}

		it = secEnter.find("outdirname");
		if(it != end(secEnter))
		{
			mainObj->outdir = it->second;
		}
		//it = secEnter.find("log");
		//if(it != end(secEnter))
		//{
		//	mainObj->logstr = it->second;
		//}
	}

	return res;
}

void CGloader::checkDescChannels(void)
{
	for(auto& elem : globalSmart)
	{
		if(elem == "default")
			throw exception(string("Channel 'default' found in 'GlobalSmart' section").c_str());
		if(global.find(elem) != end(global))
			throw exception(string("Channel '").append(elem).append("' has double description in 'GlobalSmart' and 'Global'").c_str());
		for(auto& evs : virtSmart)
		{
			if(evs.second.find(elem) != end(evs.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'GlobalSmart' and 'VirtualSmart' base class '").append(evs.first).append("'").c_str());
		}
		for(auto& ev : virt)
		{
			if(ev.second.find(elem) != end(ev.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'GlobalSmart' and 'Virtual' base class '").append(ev.first).append("'").c_str());
		}
		for(auto& els : localSmart)
		{
			if(els.second.find(elem) != end(els.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'GlobalSmart' and 'LocalSmart' document '").append(els.first).append("'").c_str());
		}
		for(auto& el : local)
		{
			if(el.second.find(elem) != end(el.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'GlobalSmart' and 'Local' document '").append(el.first).append("'").c_str());
		}
	}
	for(auto& elem : global)
	{
		if(elem == "default")
			throw exception(string("Channel 'default' found in 'Global' section").c_str());
		for(auto& evs : virtSmart)
		{
			if(evs.second.find(elem) != end(evs.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'Global' and 'VirtualSmart' base class '").append(evs.first).append("'").c_str());
		}
		for(auto& ev : virt)
		{
			if(ev.second.find(elem) != end(ev.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'Global' and 'Virtual' base class '").append(ev.first).append("'").c_str());
		}
		for(auto& els : localSmart)
		{
			if(els.second.find(elem) != end(els.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'Global' and 'LocalSmart' document '").append(els.first).append("'").c_str());
		}
		for(auto& el : local)
		{
			if(el.second.find(elem) != end(el.second))
				throw exception(string("Channel '").append(elem).append("' has double description in 'Global' and 'Local' document '").append(el.first).append("'").c_str());
		}
	}
	for(auto& elem : virtSmart)
	{
		for(auto& l : elem.second)
		{
			if(l == "default")
				throw exception(string("Channel 'default' found in 'VirtualSmart' section base class '").append(elem.first).append("'").c_str());
			for(auto& ev : virt)
			{
				if(ev.second.find(l) != end(ev.second))
					throw exception(string("Channel '").append(l).append("' has double description in 'VirtualSmart' base class '").append(elem.first).append("' and 'Virtual' base class '").append(ev.first).append("'").c_str());
			}
			for(auto& els : localSmart)
			{
				if(els.second.find(l) != end(els.second))
					throw exception(string("Channel '").append(l).append("' has double description in 'VirtualSmart' base class '").append(elem.first).append("' and 'LocalSmart' document '").append(els.first).append("'").c_str());
			}
			for(auto& el : local)
			{
				if(el.second.find(l) != end(el.second))
					throw exception(string("Channel '").append(l).append("' has double description in 'VirtualSmart' base class '").append(elem.first).append("' and 'Local' document '").append(el.first).append("'").c_str());
			}
		}
	}
	for(auto& elem : virt)
	{
		for(auto& l : elem.second)
		{
			if(l == "default")
				throw exception(string("Channel 'default' found in 'Virtual' section base class '").append(elem.first).append("'").c_str());
			for(auto& els : localSmart)
			{
				if(els.second.find(l) != end(els.second))
					throw exception(string("Channel '").append(l).append("' has double description in 'Virtual' base class '").append(elem.first).append("'LocalSmart' document '").append(els.first).append("'").c_str());
			}
			for(auto& el : local)
			{
				if(el.second.find(l) != end(el.second))
					throw exception(string("Channel '").append(l).append("' has double description in 'Virtual' base class '").append(elem.first).append("' 'Local' document '").append(el.first).append("'").c_str());
			}
		}
	}
	for(auto& elem : localSmart)
	{
		auto itd = elem.second.find("default");
		if(itd != end(elem.second))
			throw exception(string("Channel 'default' found in 'LocalSmart' section in document '").append(elem.first).append("'").c_str());
		auto it = local.find(elem.first);
		if(it != end(local))
		{
			for(auto& l : elem.second)
			{
				if(it->second.find(l) != end(it->second))
					throw exception(string("Channel '").append(l).append("' has double description in 'LocalSmart' and 'Local' document '").append(elem.first).append("'").c_str());
			}
		}
	}
	for(auto& elem : local)
	{
		auto itd = elem.second.find("default");
		if(itd != end(elem.second))
			throw exception(string("Channel 'default' found in 'Local' section in document '").append(elem.first).append("'").c_str());
	}
}

