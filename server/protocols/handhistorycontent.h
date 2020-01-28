#ifndef	handhistorycontent_h_included
#define	handhistorycontent_h_included

#include "commmsgbody.h"
#include "commssl.h"


template <typename T>
void encodeDataStructAsHandHistoryContentString(const T& t, PString& content)
{
	CommMsgBody body;
	t.composeMsg(body);
	content.cut(0);
	CommSSLBase64::encode(content, body._readPtr(), body._size(), 0, true);   // "true" will make encode function to use "-_" pair instead of "+/" pair for the last two indexes (62th & 63th)
}

template <typename T>
void decodeDataStructFromHandHistoryContentString(const PString& content, T& t)
{
	PBlock ret;
	CommSSLBase64::decode(ret, content, content.length());
	CommMsgBody body;
	body.moveFrom(ret);
	CommMsgParser parser(body);
	t.parseMsg(parser);
}


#endif


