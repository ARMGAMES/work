// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genJavaScript.h"

#ifdef _WINDLL
// Exceptions SEH
void SehTranslatorFunction(unsigned int code, struct _EXCEPTION_POINTERS*)
{
	std::ostringstream result;
	result << "SEH code "<<std::hex << std::uppercase<<code;
	throw exception(result.str().c_str());
}
DefineLog
#endif // _WINDLL

	extern "C" __declspec(dllexport) void __cdecl genJavaScript(CGmain* mainObj)
{
#ifdef _WINDLL
	_set_se_translator(SehTranslatorFunction);
	CreateLog("genJavaScript_"CG_LOG_FILE_NAME, true);
#endif // _WINDLL
	try
	{
		if(!mainObj)
			throw exception("Main class pointer is null.");
		CGJSjavascriptmain mainJavaScriptObj(mainObj);
		mainJavaScriptObj.createJavaScriptFiles();
	}
	catch(  exception& exc  )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog(string("genJavaScript error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog( "Unknown genJavaScript error. \n" );
	}
}

