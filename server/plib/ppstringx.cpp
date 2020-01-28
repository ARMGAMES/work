#include "ppinclude.h"
#include "ppstring.h"
#include "ppi18n.h"
#include "ppi18n_x.h"

/* virtual */ void PStringEncodingPStringX::_append( PString& s, PUNICHAR_32 c ) const
{
	if( c == 0 )
	{
		if( flags & FlagErrorOnNullChar )
			throw PInternalError( "PStringEncodingPStringX: unexpected NULL character" );
		else if( flags & FlagDefaultCharOnNullChar )
		{
			PStringX::staticAppend( s, defaultChar );
			return;
		}
		else
			return;//ignore
	}

	PStringX::staticAppend( s, c );
}

/* virtual */ void PStringEncodingPStringX::preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const
{
	bool srcUTF, argUTF, retUTF;

	srcUTF = PStringX::isUtf8(msg);
	retUTF = PStringX::isUtf8(ret);
	int i;
	for (i = 0; i < argc; ++i)
		if (PStringX::isUtf8(argv[i]))
			break;
	argUTF = (argc && i != argc);

	if (!retUTF && !srcUTF && !argUTF)
	{
		for(const char* s = msg; *s ; ++s)
		{
			if(*s != '%')
			{
				ret.append(*s);
				continue;
			}
			//%
			if(*(++s) == 0)
				break;
			if(*s == '%')
			{
				ret.append('%');
				continue;
			}
			//L or l
			bool isLower = false;
			if(*s == 'L' || *s == 'l')
			{
				isLower = true;
				if(*(++s) == 0)
					break;
			}
			//0-9
			if(*s >= '0' && *s <= '9')
			{
				int m = *s - '0';
				if(m < argc)
				{
					if(isLower)
					{
						PString insert(argv[m]);
						insert.toLower();
						ret.append(insert);
					}
					else
						ret.append(argv[m]);
				}
			}
			else
			{
				//invalid msg format - ignored
			}
		}
	}
	else
	{
		PStringX::StringParser srcParser;
		PStringX::StringParser argParser;

		srcParser.init(msg, 0);

		for(PUNICHAR_RET s = srcParser.nextChar(); s > 0; s = srcParser.nextChar())
		{
			if(s != '%')
			{
				PStringX::staticAppend(ret, s);
				continue;
			}
			//%
			if((s = srcParser.nextChar()) <= 0)
				break;
			if(s == '%')
			{
				PStringX::staticAppend(ret, '%');
				continue;
			}
			//L or l
			bool isLower = false;
			if(s == 'L' || s == 'l')
			{
				isLower = true;
				if((s = srcParser.nextChar()) <= 0)
					break;
			}
			//0-9
			if(s >= '0' && s <= '9')
			{
				int m = s - '0';
				if(m < argc)
				{
					argParser.init(argv[m]);
					for(PUNICHAR_RET t = argParser.nextChar(); t > 0; t = argParser.nextChar())
						PStringX::staticAppend(ret, isLower ? i18nCharToLowerCS(t) : t);
				}
			}
			else
			{
				//invalid msg format - ignored
			}
		}
	}
}
