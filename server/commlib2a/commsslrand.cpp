#include "ppincludebase.h"//_before_ openssl headers

#include <openssl/crypto.h>
//#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "commssl.h"

/* static */ void CommSSL::generateRandomBytes( BYTE* buf, size_t sz )
{
	int ok = RAND_bytes( buf, sz );
	if( !ok )
		_throwErr();
}
