#include "ppincludebase.h"//_before_ openssl headers

#include <openssl/ssl.h>
#include <openssl/sha.h>
//#include <openssl/pem.h>
#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions
#include "commssl.h"


/*static */ void CommSSLPasswordHash::_generate(PBlock & ret, const char * input)
{
	SHA256_CTX c;
#ifdef P_BIG_ENDIAN
	UINT32 _slt=0x2a47e2df;  
#else
	UINT32 _slt=0xdfe2472a;  
#endif

	SHA256_Init(&c);
	SHA256_Update(&c,input, strlen(input));
	SHA256_Update(&c,&_slt, sizeof(UINT32));

	ret.alloc( SHA256_DIGEST_LENGTH );
	SHA256_Final(ret.ptr(),&c);
	OPENSSL_cleanse(&c,sizeof(c));
}


/*static */ bool CommSSLPasswordHash::checkHash(const char * inputHash, const char * input)
{
	PBlock _blockHash;
	CommSSLBase64::decode(_blockHash,inputHash);
	if (_blockHash.size() != 32)
		return false;
	PBlock _blockRet;
	_generate(_blockRet,input);

	return memcmp(_blockRet.ptr(),_blockHash.ptr(),_blockRet.size()) == 0;
}




