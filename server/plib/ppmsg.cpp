#include "ppinclude.h"
#include "ppmsg.h"

#define MAX_LOCALE_DEPTH	5
#define LOCALE_DEFAULT		0

void _PMsg( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, int nArgs, const char* args[] )
{
	if( !msgId.isValid() || msgId.isNull() )
		return;

	char fallbackMsg[256];
	const char* currMsg = 0;
	unsigned int savedId = localeId;
	const PMsgLocale* locales = localeTable.locales;
	unsigned int maxLocales = localeTable.max;
	const PMsgCData* msgData = msgId.msgTable->msgData;
	unsigned int id = msgId.msgId;
	unsigned int rowSize = msgId.msgTable->rowSize;
	const PStringEncoding* encoding = localeTable.encoding;

	PASSERT(msgData);
	PASSERT5(localeId < maxLocales); // debug only

	int i;
	for( i = 0; i < MAX_LOCALE_DEPTH; ++i )
	{
		// invalid locale id - use default
		if( maxLocales <= localeId )
		{
			localeId = LOCALE_DEFAULT;
			currMsg = msgData[localeId * rowSize + id].msg;
			break;
		}
		// try message in the current table
		currMsg = msgData[localeId * rowSize + id].msg;
		if( currMsg )
			break;
		// reached the tree top
		if( localeId == locales[localeId].parentId )
			break;
		// revert to parent locale
		localeId = locales[localeId].parentId;
	}

	// probably an unintended cycle - use default
	if( i == MAX_LOCALE_DEPTH )
	{
		localeId = LOCALE_DEFAULT;
		currMsg = msgData[localeId * rowSize + id].msg;
	}

	// message not found - form a message stub
	if( !currMsg )
	{
		char* cp = fallbackMsg;
		cp += sprintf( cp, "LC_%d_MSG_%d ", savedId, id );
		for( int j = 0; j < nArgs; ++j )
		{
			cp += sprintf( cp, "%%%d ", j );
		}
		currMsg = fallbackMsg;
	}

	// at this point, we *always* have a valid message template
	PASSERT(currMsg);
	encoding->preparePMsg( ret, currMsg, nArgs, args );
}

void PMsg( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId )
{
	_PMsg( ret, localeTable, localeId, msgId, 0, 0 );
}

void PMsg1( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0 )
{
	const char* args[] = { p0 };
	_PMsg( ret, localeTable, localeId, msgId, 1, args );
}

void PMsg2( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1 )
{
	const char* args[] = { p0, p1 };
	_PMsg( ret, localeTable, localeId, msgId, 2, args );
}

void PMsg3( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1, const char* p2 )
{
	const char* args[] = { p0, p1, p2 };
	_PMsg( ret, localeTable, localeId, msgId, 3, args );
}

void PMsg4( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1, const char* p2, const char* p3 )
{
	const char* args[] = { p0, p1, p2, p3 };
	_PMsg( ret, localeTable, localeId, msgId, 4, args );
}

void PMsg5( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4 )
{
	const char* args[] = { p0, p1, p2, p3, p4 };
	_PMsg( ret, localeTable, localeId, msgId, 5, args );
}

PMsgId PMsgIdFromMsgStr( const PMsgStrTable& strTable, const char* msgStr )
{
	const PMsgStrCData* strData = strTable.strData;
	unsigned int dataSize = strTable.dataSize;
	const PMsgStrCData comp = {0, 0, msgStr};
	const PMsgStrCData* it = lower_bound( strData, strData + dataSize, comp );
	if( it < strData + dataSize && strcmp( ( *it ).msgStr, msgStr ) == 0 )
	{
		return PMsgId( ( *it ).msgTable, ( *it ).msgId );
	}
	else
	{
		// String tag not found: default to NO ACTION in production code
		PASSERT5(0);
		return PMsgId();
	}
}

UINT32 PMsgStr( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* msgStr )
{
	const PMsgId id = PMsgIdFromMsgStr( strTable, msgStr );
	if( id.isNull() )
		return 0;

	_PMsg( ret, localeTable, localeId, id, 0, nullptr );
	return PMsgFlags( localeId, id );
}

UINT32 PMsgStr1( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* msgStr, const char* p0 )
{
	const PMsgId id = PMsgIdFromMsgStr( strTable, msgStr );
	if( id.isNull() )
		return 0;

	const char* args[] = { p0 };
	_PMsg( ret, localeTable, localeId, id, 1, args );
	return PMsgFlags( localeId, id );
}

UINT32 PMsgStr2( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* msgStr, const char* p0, const char* p1 )
{
	const PMsgId id = PMsgIdFromMsgStr( strTable, msgStr );
	if( id.isNull() )
		return 0;

	const char* args[] = { p0, p1 };
	_PMsg( ret, localeTable, localeId, id, 2, args );
	return PMsgFlags( localeId, id );
}

UINT32 PMsgStr3( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* msgStr, const char* p0, const char* p1, const char* p2 )
{
	const PMsgId id = PMsgIdFromMsgStr( strTable, msgStr );
	if( id.isNull() )
		return 0;

	const char* args[] = { p0, p1, p2 };
	_PMsg( ret, localeTable, localeId, id, 3, args );
	return PMsgFlags( localeId, id );
}

UINT32 PMsgStr4( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* msgStr, const char* p0, const char* p1, const char* p2, const char* p3 )
{
	const PMsgId id = PMsgIdFromMsgStr( strTable, msgStr );
	if( id.isNull() )
		return 0;

	const char* args[] = { p0, p1, p2, p3 };
	_PMsg( ret, localeTable, localeId, id, 4, args );
	return PMsgFlags( localeId, id );
}

UINT32 PMsgStr5( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* msgStr, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4 )
{
	const PMsgId id = PMsgIdFromMsgStr( strTable, msgStr );
	if( id.isNull() )
		return 0;

	const char* args[] = { p0, p1, p2, p3, p4 };
	_PMsg( ret, localeTable, localeId, id, 5, args );
	return PMsgFlags( localeId, id );
}

UINT32 PMsgFlags( unsigned int localeId, const PMsgId& msgId )
{
	if( !msgId.isValid() || msgId.isNull() )
		return 0;

	const PMsgCData* msgData = msgId.msgTable->msgData;
	unsigned int id = msgId.msgId;
	unsigned int rowSize = msgId.msgTable->rowSize;
	return msgData[localeId * rowSize + id].flags;
}

void _PMsg( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, int nArgs, const char* args[] )
{
	PASSERT(msg);
	// at this point, we *always* have a valid message template
	localeTable.encoding->preparePMsg( ret, msg, nArgs, args );
}

void PMsg( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg )
{
	_PMsg( ret, localeTable, localeId, msg, 0, 0 );
}

void PMsg1( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0 )
{
	const char* args[] = { p0 };
	_PMsg( ret, localeTable, localeId, msg, 1, args );
}

void PMsg2( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1 )
{
	const char* args[] = { p0, p1 };
	_PMsg( ret, localeTable, localeId, msg, 2, args );
}

void PMsg3( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1, const char* p2 )
{
	const char* args[] = { p0, p1, p2 };
	_PMsg( ret, localeTable, localeId, msg, 3, args );
}

void PMsg4( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1, const char* p2, const char* p3 )
{
	const char* args[] = { p0, p1, p2, p3 };
	_PMsg( ret, localeTable, localeId, msg, 4, args );
}

void PMsg5( PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4 )
{
	const char* args[] = { p0, p1, p2, p3, p4 };
	_PMsg( ret, localeTable, localeId, msg, 5, args );
}

// Deprecated functionality
void _PXMsg( PString& ret, const char* format, int nArgs, const char* args[] )
{
	for( const char* s = format; *s; ++s )
	{
		if( *s == '%' )
		{
			char c = s[ 1 ];
			if( c == '%' )
			{
				ret.append( '%' );
				++s;
			}
			else if( c >= '0' && c <= '9' )
			{
				int i = c - '0';
				if( i < nArgs )
					ret.append( args[ i ] );
				++s;
			}
			else
			{
				//invalid msg format - ignored
			}
		}
		else
			ret.append( *s );
	}
}

void PXMsg( PString& ret, const char* format )
{
	_PXMsg( ret, format, 0, 0 );
}

void PXMsg1( PString& ret, const char* format, const char* p0 )
{
	const char* args[] = { p0 };
	_PXMsg( ret, format, 1, args );
}

void PXMsg2( PString& ret, const char* format, const char* p0, const char* p1 )
{
	const char* args[] = { p0, p1 };
	_PXMsg( ret, format, 2, args );
}

void PXMsg3( PString& ret, const char* format, const char* p0, const char* p1, const char* p2 )
{
	const char* args[] = { p0, p1, p2 };
	_PXMsg( ret, format, 3, args );
}

void PXMsg4( PString& ret, const char* format, const char* p0, const char* p1, const char* p2, const char* p3 )
{
	const char* args[] = { p0, p1, p2, p3 };
	_PXMsg( ret, format, 4, args );
}

void PXMsg5( PString& ret, const char* format, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4 )
{
	const char* args[] = { p0, p1, p2, p3, p4 };
	_PXMsg( ret, format, 5, args );
}
