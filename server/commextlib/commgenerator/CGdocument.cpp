// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"
#include "antlr\output\commgenLexer.h"
#include "antlr\output\commgenParser.h"

//******************************************************************************
// Document info
//******************************************************************************
// Load input files
//******************************************************************************
void CGdocument::load(CGloader* pldr, const string& fullName)
{
	bool res = true;
	string errmsg;

	// lexer/parser definitions
	pANTLR3_INPUT_STREAM			input = nullptr;    
	pcommgenLexer					lex = nullptr;   
	pANTLR3_COMMON_TOKEN_STREAM		tokens = nullptr;    
	pcommgenParser					parser = nullptr; 

	try
	{
		input = antlr3FileStreamNew((pANTLR3_UINT8)(fullName.c_str()), ANTLR3_ENC_8BIT);  
		if(nullptr == input) 
			throw exception("Cannot create input stream");

		lex = commgenLexerNew(input);    
		if(nullptr == lex) 
			throw exception("Cannot create lexer");

		tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));    
		if(nullptr == tokens) 
			throw exception("Cannot create token stream");

		parser = commgenParserNew(tokens);     
		if(nullptr == parser) 
			throw exception("Cannot create parser");

		parser->prog(parser, pldr, this);
	}
	catch( exception& err )
	{
		res = false;
		errmsg.append("commgen: Lexer/parser error, file - '").append(fullName).append("' - ").append(err.what()).append("\n");
	}
	catch( ... )
	{
		res = false;
		errmsg.append("commgen: Lexer/parser general error, file - '").append(fullName).append("'\n");
	}

	if(parser)	parser ->free(parser);    
	if(tokens)	tokens ->free(tokens);    
	if(lex)		lex    ->free(lex);    
	if(input)	input  ->close(input); 

	if(!res)
		throw exception(errmsg.c_str());
}

// Input info check
void CGdocument::checkChannels1(CGloader* pldr)
{
	for(auto& obj : objects)
	{
		if(obj->getObjType() == objTypes:: gTypeStruct)
		{
			CGstruct* cls = dynamic_cast<CGstruct*>(obj.get());
			if(!cls)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(fileName).append("' object-'").append(obj->getName()).append("'");
				throw exception(err.c_str());
			}
			if(cls->hasProperty(CG_CLASS_INH_VIRT))
			{
				auto pit1 = pldr->getMainObj()->baseClasses.insert(make_pair(cls->getBaseClass(), make_pair(map<string, tuple<bool, bool, bool>>(), uint64_t(0))));	
				auto pit2 = pit1.first->second.first.insert(make_pair("default", make_tuple(false, true, true)));
			}

			cls->checkChannels(pldr, this);
		}
	}
}

void CGdocument::checkChannels2(CGloader* pldr)
{
	for(auto& obj : objects)
	{
		if(obj->getObjType() == objTypes:: gTypeStruct)
		{
			CGstruct* cls = dynamic_cast<CGstruct*>(obj.get());
			if(!cls)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(fileName).append("' object-'").append(obj->getName()).append("'");
				throw exception(err.c_str());
			}
			cls->checkVirtualChannels1(pldr);
		}
	}
}

void CGdocument::checkChannels3(CGloader* pldr)
{
	for(auto& obj : objects)
	{
		if(obj->getObjType() == objTypes:: gTypeStruct)
		{
			CGstruct* cls = dynamic_cast<CGstruct*>(obj.get());
			if(!cls)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(fileName).append("' object-'").append(obj->getName()).append("'");
				throw exception(err.c_str());
			}
			cls->checkVirtualChannels2(pldr);
		}
	}
}

