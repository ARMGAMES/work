// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genCpp.h"

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

//string nameGen(void)
//{
//	static int val = 0;
//	string ret = "generated_cpp_";
//
//	ret.append(to_string(++val));
//
//	return ret;
//}

extern "C" __declspec(dllexport) void __cdecl genCpp(CGmain* mainObj)
{
#ifdef _WINDLL
	_set_se_translator(SehTranslatorFunction);
	CreateLog("genCpp_"CG_LOG_FILE_NAME, true);
#endif // _WINDLL
	try
	{
		if(!mainObj)
			throw exception("Main class pointer is null.");
		CGGcppmain mainCppObj(mainObj);
		mainCppObj.createCppFiles();
	}
	catch(  exception& exc  )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog(string("genCpp error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		if(mainObj) mainObj->finishOK = false;
		WriteLog( "Unknown genCpp error. \n" );
	}
}

