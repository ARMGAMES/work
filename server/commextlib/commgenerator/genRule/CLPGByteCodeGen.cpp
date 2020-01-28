// Copyright (C) 2012 PYR Software Ltd. All rights reserved.

//#include "CLPGlib.h"
#include "..\antlr\output\cesar_interpretatorLexer.h"
#include "..\antlr\output\cesar_interpretatorParser.h"

//******************************************************************************
// CLPGByteCodeGen definitions
//******************************************************************************
// Static section
//******************************************************************************

UINT64 CLPGByteCodeGen::errors = 0;

//******************************************************************************
// Generate rule byte code file
//******************************************************************************
UINT64 CLPGByteCodeGen::generate( const char* ruleFile )
{
	UINT64 size, amount, header;
	string buf;
	PBlock dt;
	BYTE* p;
	UINT64* pu;
	FILE* outputFileHandle = NULL;

	pANTLR3_INPUT_STREAM			input = NULL;    
	pcesar_interpretatorLexer		lex = NULL;   
	pANTLR3_COMMON_TOKEN_STREAM		tokens = NULL;    
	pcesar_interpretatorParser		parser = NULL; 

	//WriteLog("Byte code parser started:\n");

	try
	{
		input = antlr3FileStreamNew((pANTLR3_UINT8)ruleFile, ANTLR3_ENC_8BIT);  
		if(NULL == input) 
			throw exception("Cannot create input stream");

		lex = cesar_interpretatorLexerNew(input);    
		if(NULL == lex) 
			throw exception("Cannot create lexer");

		tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));    
		if(NULL == tokens) 
			throw exception("Cannot create token stream");

		parser = cesar_interpretatorParserNew(tokens);     
		if(NULL == parser) 
			throw exception("Cannot create parser");

		parser->prog(parser, this);
	}
	catch(  exception& exc  )
	{
		mainObj->finishOK = false;
		WriteLog(string("ByteCodeGen parcer error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		mainObj->finishOK = false;
		WriteLog( "ByteCodeGen parcer unknown error.\n" );
	}
	if(parser)	parser ->free(parser);    
	if(tokens)	tokens ->free(tokens);    
	if(lex)		lex    ->free(lex);    
	if(input)	input  ->close(input); 

	//WriteLog(string("Byte code parser finished with ").append(to_string(errors)).append(" errors\n"));

	if(0 != errors)
	{
		mainObj->finishOK = false;
		WriteLog("Byte code not generated\n");
		return errors;
	}

	try
	{
		buf.assign(ruleFile).append(".bc");

		outputFileHandle = fopen( buf.c_str(), "wb" );
		if(NULL == outputFileHandle) 
			throw exception("Cannot open rule file");

		if(cdata.size() % sizeof(UINT64) != 0)
			throw exception("Size of code block is not multiple UINT64");

		// header
		header = 2 + 5 * sizeof(UINT64);
		amount = 1;
		if(0 != stringsdata.size())
		{
			header += 3 * sizeof(UINT64);
			++amount;
		}
		if(0 != datacounter)
		{
			header += 3 * sizeof(UINT64);
			++amount;
		}
		p = dt.append(header);
		*p++ = 'b';
		*p++ = 'c';
		pu = reinterpret_cast<UINT64*>(p);
		*pu++ = CLPG_BC_GEN_VERSION;				// version
		*pu++ = amount;								// sections amount

		size  = header;
		*pu++ = 1;									// 1-st section codename - code
		*pu++ = cdata.size();						// 1-st section size
		*pu++ = size;								// 1-st section offset
		size += cdata.size();
		if(0 != stringsdata.size())
		{
			*pu++ = 2;								// 2-st section codename - const strings
			*pu++ = stringsdata.size();				// 2-st section size
			*pu++ = size;							// 2-st section offset
			size += stringsdata.size();
		}
		if(0 != datacounter)
		{
			*pu++ = 3;								// 3-st section codename - data
			*pu++ = datacounter * sizeof(UINT64);	// 3-st section size
			*pu++ = 0;								// 3-st section offset - need allocation
		}
		// header
		fwrite(dt.ptr(), sizeof(BYTE), header, outputFileHandle);
		// code
		fwrite(cdata.ptr(), sizeof(BYTE), cdata.size(), outputFileHandle);
		// const strings
		if(0 != stringsdata.size())
			fwrite(stringsdata.ptr(), sizeof(BYTE), stringsdata.size(), outputFileHandle);
	}
	catch(  exception& exc  )
	{
		mainObj->finishOK = false;
		WriteLog(string("BByteCodeGen byte-code file error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		mainObj->finishOK = false;
		WriteLog( "ByteCodeGen byte-code file unknown error.\n" );
	}
	if(outputFileHandle)	fclose(outputFileHandle); 

	return errors;
}

//******************************************************************************
// Add label
//******************************************************************************
void CLPGByteCodeGen::addLabel(const string& name)
{
	UINT64 i;
	UINT64* p;
	multimap<string, UINT64>::iterator miter;

	auto iter = labels.find(name);
	if(iter == labels.end())
	{
		labels.insert(make_pair(name, cdata.size() / sizeof(UINT64)));

		while((miter = labelRefs.find(name)) != labelRefs.end())
		{
			p = getFirst(miter->second);
			*p = cdata.size() / sizeof(UINT64);
			labelRefs.erase(miter);
		}
	}
	else
	{
		setError();
		WriteLog(string("Label '").append(name).append("' multiple definition\n"));
	}
}

//******************************************************************************
// Find label
//******************************************************************************
bool CLPGByteCodeGen::findLabel( const string& name, UINT64& addr )
{
	auto iter = labels.find(name);
	if(iter != labels.end())
	{
		addr = iter->second;
		return true;
	}
	else
	{
		return false;
	}
}

//******************************************************************************
// External object find
//******************************************************************************
UINT64 CLPGByteCodeGen::findExternalObj( const char* type )
{
	UINT64 v = 0;

	auto typeiter = externals.find(type);
	if(typeiter != externals.end())
	{
		v = typeiter->second;
	}
	return v;
}

//******************************************************************************
// External object get
//******************************************************************************
UINT64 CLPGByteCodeGen::getExternalObj(const char* type)
{
	UINT64 v = 0;

	auto typeiter = externals.find(type);
	if(typeiter != externals.end())
	{
		v = typeiter->second;
	}
	else
	{
		v = ++extcounter;
		externals.insert(make_pair(type, v));
	}
	return v;
}

//******************************************************************************
// Const string find
//******************************************************************************
UINT64 CLPGByteCodeGen::findConstString(const char* str)
{
	UINT64 v = 0;

	auto striter = stringsmap.find(str);
	if(striter != stringsmap.end())
	{
		v = striter->second;
	}
	return v;
}

//******************************************************************************
// Const string get
//******************************************************************************
UINT64 CLPGByteCodeGen::getConstString(const char* str, bool correct)
{
	BYTE *c;
	const char* s;
	UINT64 size;
	UINT64 v = 0;

	auto striter = stringsmap.find(str);
	if(striter != stringsmap.end())
	{
		v = striter->second;
	}
	else
	{
		size = strlen(str);
		s = str;
		if(correct)
		{
			size -= 2;
			++s;
		}
		c = stringsdata.append(size + 1);
		memcpy(c, s, size);
		memset(c + size, 0, 1);
		v = c - stringsdata.ptr();
		stringsmap.insert(make_pair(str, v));
	}
	return v;
}

//******************************************************************************
// Goto label
//******************************************************************************
void CLPGByteCodeGen::gotoLabel(const string& name, UINT64 cond)
{
	UINT64* p;
	UINT64 d, addr;

	switch (cond)
	{
	case 0:
		setCommand(CLPG_BC_CMD_GOTO);
		break;
	case 1:
		setCommand(CLPG_BC_CMD_GOTOFALSE);
		break;
	case 2:
		setCommand(CLPG_BC_CMD_GOTOTRUE);
		break;
	default:
		setError();
		WriteLog("Goto operation unrecognized \n");
		break;
	}
	p = allocCode(d);
	if(findLabel(name, addr))
	{
		*p = addr;
	}
	else
	{
		labelRefs.insert(make_pair(name, d));
	}
}

//******************************************************************************
// Final check unreferenced goto's
//******************************************************************************
void CLPGByteCodeGen::gotoCheck(void)
{
	multimap<string, UINT64>::iterator miter;

	if(0 != labelRefs.size())
	{
		for(miter = labelRefs.begin(); miter != labelRefs.end(); ++miter)
		{
			setError();
			WriteLog(string("Command 'goto ").append(miter->first).append(";' has no destination\n"));
		}
	}
}

//******************************************************************************
// Check data for type
//******************************************************************************
bool CLPGByteCodeGen::checkDataType(UINT64 d, UINT64 type)
{
	bool ret = false; 
	if((((d & CLPG_BC_GEN_SORTMASK) == CLPG_BC_GEN_VAR) || ((d & CLPG_BC_GEN_SORTMASK) == CLPG_BC_GEN_CONST)) && ((d & CLPG_BC_VAR_TYPEMASK) == type))
		ret = true;
	else
	{
		setError();
		WriteLog(string("Data is not '").append(to_string(type)).append("' type\n"));
	}
	return ret;
}

//******************************************************************************
// Fill first byte for type
//******************************************************************************
UINT64 CLPGByteCodeGen::makeFirst(const char* type, UINT64& dataamount, bool isConst)
{
	UINT64 ret, v;
	const CGstruct* cls;

	ret = CLPG_BC_GEN_VAR;
	dataamount = 1;
	if(!strlen(type))
	{
		setError();
		WriteLog(string("Type is empty\n"));
	}
	else if(!strcmp("INT8", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_INT8 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("BYTE", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_BYTE | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("INT16", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_INT16 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("UINT16", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_UINT16 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("INT32", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_INT32 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("UINT32", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_UINT32 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("INT64", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_INT64 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("UINT64", type))
		ret |= CLPG_BC_VAR_INT | CLPG_BC_VAR_UINT64 | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("bool", type))
		ret |= CLPG_BC_VAR_BOOL | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else if(!strcmp("PString", type))
		ret |= CLPG_BC_VAR_STRING | CLPG_BC_SET_DATAAMOUNT(dataamount);
	else
	{
		ret |= CLPG_BC_VAR_OBJECT | CLPG_BC_SET_DATAAMOUNT(dataamount);
		if(0 == strcmp(type, CLPG_BC_NAME_BASECLASS))
		{
			ret |= CLPG_BC_VAR_OBJ_BASECLASS;
		}
		else
		{
			cls = dynamic_cast<const CGstruct* >(mainObj->findObject(objTypes::gTypeStruct, type));
			if(NULL != cls)
			{
				v = cls->getClassIndex();
			}
			else
			{
				ret |= CLPG_BC_VAR_EXTERNAL;
				v = getExternalObj(type);
			}
			ret |= v & CLPG_BC_VAR_SUBTYPEMASK;
		}
	}
	if(isConst)
		ret |= CLPG_BC_VAR_CONSTDATA;
	++dataamount;
	return ret;
}

//******************************************************************************
// Function declare
//******************************************************************************
void CLPGByteCodeGen::funcDecl(const char* name, const char* type, vector<UINT64>& pars)
{
	FuncElement fe;
	UINT64 d, res = CLPG_BC_GEN_CONST | CLPG_BC_VAR_VOID | CLPG_BC_SET_DATAAMOUNT(1); // for standard return with one qbyte data
	if(!name)
		throw exception("Null pointer in 'funcDecl' name, class'CLPGByteCodeGen'");

	auto iter = functions.find(name);
	if(iter != functions.end())
	{
		setError();
		WriteLog(string("Function '").append(name).append("' multiple definition\n"));
	}
	else 
	{
		if((NULL != type) && (0 != strlen(type)))
		{
			res = makeFirst(type, d, false);
		}
		iter = functions.insert(make_pair(name, fe)).first;
		iter->second.type = res;
		iter->second.params.assign(pars.begin(), pars.end());
	}
}

//******************************************************************************
// Function run
//******************************************************************************
UINT64 CLPGByteCodeGen::funcRun(const char* name, vector<UINT64>& pars, UINT64 count)
{
	UINT64* p;
	UINT64 i, d, ret = CLPG_BC_GEN_UNDEF;

	if(0 == strcmp("printf", name))
	{
		if((pars.size() == 0) || (((*(pars.begin())) & CLPG_BC_VAR_TYPEMASK) != CLPG_BC_VAR_STRING))
		{
			setError();
			WriteLog("First operand for printf must be string\n");
		}
		else
		{
			setCommand(CLPG_BC_CMD_COPY_DATA); // params amount
			p = allocCode(d, 2);
			*p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_INT | CLPG_BC_SET_DATAAMOUNT(1) | CLPG_BC_VAR_BYTE;
			*(p + 1) = pars.size();
			setCommand(CLPG_BC_CMD_PRINT);
		}
	}
	else
	{
		auto iter = functions.find(name);
		if(iter == functions.end())
		{
			setError();
			WriteLog(string("Function '").append(name).append("' not declared \n"));
		}
		else 
		{
			vector<UINT64>& ipars = iter->second.params;
			if(pars.size() != ipars.size())
			{
				setError();
				WriteLog(string("Function '").append(name).append("' declared with different amount of arguments \n"));
			}
			else
			{
				for(i = 0; i < pars.size(); ++i)
				{
					if((pars[i] & CLPG_BC_VAR_TYPEMASK) != (ipars[i] & CLPG_BC_VAR_TYPEMASK))
					{
						setError();
						WriteLog(string("Function '").append(name).append("' with different types of arguments \n"));
						return ret;
					}
				}
			}

			setCommand(CLPG_BC_CMD_COPY_DATA); // params amount
			p = allocCode(d, 2);
			*p = CLPG_BC_GEN_CONST | CLPG_BC_VAR_INT | CLPG_BC_SET_DATAAMOUNT(1) | CLPG_BC_VAR_BYTE;
			*(p + 1) = pars.size();
			setCommand(CLPG_BC_CMD_COPY_DATA); // return type
			p = allocCode(d, 2);
			*p = ret = iter->second.type;
			*(p + 1) = 0;
			setCommand(CLPG_BC_CMD_COPY_DATA); // name func
			p = allocCode(d, 2);
			*p = CLPG_BC_GEN_CONST + CLPG_BC_VAR_STRING + CLPG_BC_SET_DATAAMOUNT(1);
			*(p + 1) = getConstString(name, false);
			setCommand(CLPG_BC_CMD_CALL);
		}
		
	}
	return ret;
}

