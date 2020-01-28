#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "ppinifile.h"
#include "pplogfile.h"
#include "ppdaemon.h"

#include "commrtable.h"
#include "commsrv.h"
#include "commtmlocal.h"
#include "commtmrw.h"
#include "commappfactory.h"

#include "commdispatcher.h"

/* static */ void CommDispatcherDaemon::composeInitMsg( CommMsgBody& ret, const char* s )
{
	for(;;)
	{
		char fmt = *s++;
		if( fmt == 0 )
			break;//forever
		char eq = *s++;
		PASSERT3( eq == '=' );
		if( fmt == 's' )
		{
			PString str;
			for(;;)//forever #2
			{
				char c = *s;
				if( c == 0 )
					break;
				++s;
				if( c == ';' )
					break;//forever #2
				if( c == '\\' )
				{
					c = *s++;
					PASSERT3( c != 0 );
				}
				str.append( c );
			}
			ret.composeString( str );
		}
		else
		{
			const char* scolon = strchr( s, ';' );
			int i = atoi( s );
			switch( fmt )
			{
			case '1':
				ret.composeBYTE( i );
				break;
			case '2':
				ret.composeUINT16( i );
				break;
			case '4':
				ret.composeUINT32( i );
				break;
			case '5':
				ret.composeINT8( i );
				break;
			case '6':
				ret.composeINT16( i );
				break;
			case '7':
				ret.composeINT32( i );
				break;
			default:
				PASSERT3( 0 );
				break;
			}
			if( scolon )
				s = scolon + 1;
			else
				s = s + strlen( s );
		}
	}
}

//PYR-121303
// Used to parse server object configuration specified as key value pairs
// input format [k1=v1;k2=v2;k3=v3;...]
/* static */ void CommDispatcherDaemon::composeInitMsgKVPairs(CommMsgBody& ret, const char* s)
{
	const char* startBracket = strchr(s, '[');
	const char* endBracket = strchr(s, ']');
	if (!startBracket || !endBracket)
	{
		PLog("Missing brackets [ or ], cannot read K-V pairs from dispatcher config");
		return;
	}

	// Read key value pairs
	PString key;
	PString value;	
	s = startBracket + 1;

	while (s && *s != *endBracket && *s != '\0')
	{
		const char* equal = strchr(s, '=');
		if (!equal)
			break;

		const char* scolon = strchr(equal, ';');

		// Assign Key
		key.assign(s, equal);

		// Assign Value
		if (scolon)
		{
			value.assign(equal + 1, scolon);
			s = scolon + 1;
		}
		else
		{
			// May be hit for last key-value pair
			value.assign(equal + 1, endBracket);
			s = endBracket;
		}

		// Compose key and value
		ret.composeString(key);
		ret.composeString(value);
	}
}

// PYR-124776
// Used to compose init msg for a server object based on the config used
// input format old config: "s=d:\\poker\\" or "s=d:\\poker\\;s=worldpaywpg_apple.ini" or "s=d:\\poker\\HgamesRO\\;s=QA26.HomeGames.DbmRO2"
// input format new K-V config: "ver=1;[sp=ArchiveManager_SP;ini_path=D:\poker\;station_name=QA212.ArchiveManager;role=AMRole;service_path=D:\Work\services\services.json]"
void CommDispatcherDaemon::composeSrvObjInitMsg(CommMsgBody& ret, const char* input, const PString& objectName)
{
	if (!input || !*input)
	{
		PLog("Empty Input, srvObj:'%s'", objectName.c_str());
		return;
	}

	static const PString verKey = SERVER_START_VER_STR"=";

	const char* verStart = strstr(input, verKey); // PYR-121303
	if (!verStart)
	{
		// Positional based old config 
		composeInitMsg(ret, input);
	}
	else
	{
		// Key-Value pair based new config
		PTRACE5("Using key-value pair config for '%s'", objectName.c_str());

		const char* verValueStart = verStart + verKey.length();
		const char* verValueEnd = strchr(verValueStart, ';');
		if (verValueEnd && *verValueEnd)
		{
			PString verValue(verValueStart, verValueEnd);
			INT32 ver = atoi(verValue);
			ret
				.composeString(SERVER_START_VER_STR)
				.composeINT32(ver);

			if (ver > 0)
			{
				// Compose K-V pairs
				composeInitMsgKVPairs(ret, verValueEnd + 1);
			}
			else
			{
				PLog("Invalid version: '%s'", input);
			}
		}
		else
		{
			PLog("Invalid start: '%s'", input);
		}
	}
}