#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdppwd.h"

/* static */ const char* CommServerPlainTextPasswordGuard::userAuthHandled()
{
	return _COMM_PPWD_GUARD_NAME;
}

/* static */ void CommServerPlainTextPasswordGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword)
{
	ret.composeString(encrPassword);
}
