#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdppwdsid.h"

/* static */ const char* CommServerPlainTextPasswordSidGuard::userAuthHandled()
{
	return _COMM_PPWDSID_GUARD_NAME;
}

/* static */ void CommServerPlainTextPasswordSidGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword, const char* sid, INT16 sidErrorCode )
{
	ret.composeString(encrPassword).composeString(sid);
	if (sidErrorCode)
		ret.composeINT16(sidErrorCode);
}
