// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"
#include <sstream>
#include "direct.h"

// Exceptions SEH
#ifdef _WIN32
void SehTranslatorFunction(unsigned int code, struct _EXCEPTION_POINTERS*)
{
	std::ostringstream result;
	result << "SEH code "<<std::hex << std::uppercase<<code;
	throw exception(result.str().c_str());
}
#endif // _WIN32

DefineLog
CGmain	mainObj;

int main(int argc, char* argv[])
{
	string	currDir;
	bool stop = false;

#ifdef _WIN32
	_set_se_translator(SehTranslatorFunction);
#endif // _WIN32

	for(auto i = 1; i < argc; ++i)
	{
		string s = argv[i];
		auto pos = s.find_first_of('-');
		if((pos != string::npos) && ((pos+1) < s.length()))
		{
			switch(s[++pos])
			{
			case 'd':
				pos = s.find_first_of(':', pos);
				if((pos == string::npos) || ((pos+1) >= s.length()))
					cerr<<"Working directory is set but not defined in format -d:dir - ignored"<<endl;
				else
				{
					if(currDir.empty())
					{
						// Change working directory
						if(0 != chdir(s.substr(pos+1).c_str()))
						{
							cerr<<"Cannot change working directory to '"<<currDir<<"'"<<endl;
						}
						else
						{
							currDir.assign(s.substr(pos+1));
							cerr<<"Change working directory to '"<<currDir<<"' - done"<<endl;
						}
					}
					else
					{
						cerr<<"Change working directory to '"<<s.substr(pos+1)<<"' ignored. Set before to '"<<currDir<<"'"<<endl;
					}
				}
				break;
			case 'h':
			case '?':
				printf("%s", help);
				stop = true;
			case 'p':
				stop = true;
				break;
			case 'i':
				cerr<<"Input INI file argument '"<<s<<"' found"<<endl;
				break;
			default:
				cerr<<"Unknown input argument '"<<s<<"' - ignored"<<endl;
				break;
			}
		}
	}

	CreateLog(CG_LOG_FILE_NAME, true);
	WriteLog("\nFor help enter option -h\n\nStarting...\n");

	try
	{
		// input load
		{
			CGloader loader(&mainObj);
			stop = loader.exec(argc, argv);
		}

		// plug-ins
		for(auto& t : mainObj.plugins)
		{
			WriteLog(string("Running ").append(t.second->getModuleType()).append(" plugin '").append(t.second->getModuleName()).append("'").c_str());
			t.second->exec(&mainObj);
		}
	}
	catch(  exception& exc  )
	{
		mainObj.finishOK = false;
		WriteLog(string("Main app: Error: ").append(exc.what()).append("\n"));
	}
	catch( ... )
	{
		mainObj.finishOK = false;
		WriteLog( "Main app: Unknown error. \n" );
	}

	if(mainObj.finishOK)
	{
		WriteLog( "...Finished\n" );
		if(stop) system("PAUSE");
		return 0;
	}
	else
	{
		WriteLog( "...ERRORS, NOT Finished\n" );
		system("PAUSE");
		return 1;
	}

}

