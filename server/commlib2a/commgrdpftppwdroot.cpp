#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdpftppwd.h"



/* static */ const char* CommServerFtpPasswordGuard::userAuthHandled()
{
	return _COMM_PFTPPWD_GUARD_NAME;
}

/* static */ void CommServerFtpPasswordGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword, UINT32 salt)
{
	ret.composeString(encrPassword).composeUINT32(salt);
}
