// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genJava.h"

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

extern "C" __declspec(dllexport) void __cdecl genJava(CGmain* mainObj)
{
#ifdef _WINDLL
	_set_se_translator(SehTranslatorFunction);
	CreateLog("genJava_"CG_LOG_FILE_NAME, true);
#endif // _WINDLL
	try
	{
		if(!mainObj)
			throw exception("Main class pointer is null.");
		CGJjavamain mainJavaObj(mainObj);
		mainJavaObj.createJavaFiles();
	}
	catch(  exception& exc  )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog(string("genJava error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog( "Unknown genJava error. \n" );
	}
}

