#include "ppincludebase.h"//_before_ openssl headers

//****************************************************************************

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

//****************************************************************************

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "ppsocket.h"
//#include "_commcomm.h"
#include "commssl.h"
//#include "_commssl.h"

/* static */ unsigned long CommSSL::_errMsg( PString& ret )
{
	const char *file,*data;
	int line,flags;
	unsigned long l = ERR_get_error_line_data( &file, &line, &data, &flags );
	if (l != 0)
	{
		char buf[200];
		ERR_error_string_n(l, buf, sizeof(buf));
		buf[sizeof(buf) - 1] = 0;
		ret.append(buf);
	}
	else
		ret.append("Unknown");
	return l;
}

/* static */ void CommSSL::_throwErr()
{
	PString err("SSL error: ");
	unsigned long errCode = _errMsg(err);
	throw PInternalError(err, errCode);
}

