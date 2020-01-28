#include "ppincludebase.h"//_before_ openssl headers

#include <openssl/ssl.h>
#include <openssl/sha.h>
//#include <openssl/pem.h>
#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions
#include "commssl.h"


/*static */ void CommSSLPasswordHash::generateHash(PString & ret, const char * input)
{
	PBlock _blockRet;
	_generate(_blockRet,input); 
	ret.assign("");
	CommSSLBase64::encode(ret,_blockRet.ptr(),_blockRet.size());
}




