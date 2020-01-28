// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genRule.h"

#ifdef _WINDLL
// Exceptions SEH
void SehTranslatorFunction(unsigned int code, struct _EXCEPTION_POINTERS*)
{
	std::ostringstream result;
	result << "SEH code "<<std::hex << std::uppercase<<code;
	throw exception(result.str().c_str());
}
DefineLog
#else
#ifdef _DEBUG
#pragma comment( lib, "..\\..\\plib\\x64\\debug\\plib.lib" )
#else
#pragma comment( lib, "..\\..\\plib\\x64\\release\\plib.lib" )
#endif // _DEBUG
#endif // _WINDLL

extern "C" __declspec(dllexport) void __cdecl genRule(CGmain* mainObj)
{
#ifdef _WINDLL
	_set_se_translator(SehTranslatorFunction);
	CreateLog("genRule_"CG_LOG_FILE_NAME, true);
#endif // _WINDLL

	try
	{
		if(!mainObj)
			throw exception("Main class pointer is null.");
		CGRrulemain mainRuleObj(mainObj);
		mainRuleObj.generateRule();
	}
	catch(  exception& exc  )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog(string("genRule error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog( "Unknown genRule error. \n" );
	}
}


