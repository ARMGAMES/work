#include "initdata.h"
#include "dbmstmt.h"

char* strCopy( char *strDestination, int bufferSize, const char *strSource )
{
	UINT32 sourceLen = PString::length(strSource);
	if ( bufferSize < sourceLen + 1 )
	{
		const char* errInfo = "";
#ifdef _DEBUG
		PString errMsg("Error STRING_TOO_LONG: Source length = ");
		errMsg
			.appendInt(sourceLen)
			.append(", Max length = ")
			.appendInt(bufferSize - 1)
			.append(", Source = '")
			.append(strSource)
			.append("'")
			;
		errInfo = errMsg.c_str();
#else
		char errInfoBuff[256];
		sprintf(errInfoBuff, "Error STRING_TOO_LONG: Source length = %d, Max length = %d", sourceLen, bufferSize - 1);
		errInfo = errInfoBuff;
#endif // DEBUG
		PLog("%s", errInfo);
		throw PSqlError(STRING_TOO_LONG, errInfo);
	}
	return strcpy( strDestination, strSource );
}

char * strCopyCut( char *strDestination, int bufferSize, const char *strSource )
{
	if ( bufferSize < PString::length(strSource) + 1 )
	{
		strncpy( strDestination, strSource, bufferSize );
		strDestination[bufferSize - 1] = '\0';
		return strDestination;
	}
	return strcpy( strDestination, strSource );
}
