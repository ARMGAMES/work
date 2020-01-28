#include "ppincludebase.h"//_before_ openssl headers

//****************************************************************************

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

//****************************************************************************

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "commssl.h"

struct _CommSSLChksumImpl
{
	EVP_MD_CTX  *ctx;
	_CommSSLChksumImpl()
	{
		ctx = EVP_MD_CTX_create();
	}
    ~_CommSSLChksumImpl()
	{
		EVP_MD_CTX_destroy(ctx);
	}

};

static inline _CommSSLChksumImpl* _chksumImpl( void* p )
{
	return (_CommSSLChksumImpl*)p;
}

CommSSLChksum::CommSSLChksum()
{
	impl = new _CommSSLChksumImpl();
	EVP_DigestInit( _chksumImpl(impl)->ctx,  EVP_sha1() );
}

CommSSLChksum::~CommSSLChksum()
{
//	EVP_MD_CTX_cleanup( _chksumImpl(impl)->ctx ); not necessary due to EVP_MD_CTX_free()
	delete _chksumImpl( impl );
}

void CommSSLChksum::addData( const BYTE* data, size_t sz )
{
	EVP_DigestUpdate( _chksumImpl( impl )->ctx, data, sz );
}

void CommSSLChksum::chksum( PBlock& ret )
{
	ret.alloc( EVP_MAX_MD_SIZE );
	unsigned int sz;
	EVP_DigestFinal( _chksumImpl( impl )->ctx, ret.ptr(), &sz );
	PASSERT3( sz <= EVP_MAX_MD_SIZE );
	ret.cut( sz );
}

CommSSLChksumMD5::CommSSLChksumMD5()
{
	impl = new _CommSSLChksumImpl();
	EVP_DigestInit( _chksumImpl(impl)->ctx, EVP_md5() );
}

CommSSLChksumMD5::~CommSSLChksumMD5()
{
//	EVP_MD_CTX_cleanup( &_chksumImpl(impl)->ctx ); not necessary due to EVP_MD_CTX_free()
	delete _chksumImpl( impl );
}

void CommSSLChksumMD5::addData( const BYTE* data, size_t sz )
{
	EVP_DigestUpdate( _chksumImpl( impl )->ctx, data, sz );
}

void CommSSLChksumMD5::chksum( PBlock& ret )
{
	ret.alloc( EVP_MAX_MD_SIZE );
	unsigned int sz;
	EVP_DigestFinal( _chksumImpl( impl )->ctx, ret.ptr(), &sz );
	PASSERT3( sz <= EVP_MAX_MD_SIZE );
	ret.cut( sz );
}

CommSSLChksumSHA256::CommSSLChksumSHA256()
{
	impl = new _CommSSLChksumImpl();
	EVP_DigestInit( _chksumImpl(impl)->ctx,  EVP_sha256() );
}

CommSSLChksumSHA256::~CommSSLChksumSHA256()
{
//	EVP_MD_CTX_cleanup( &_chksumImpl(impl)->ctx ); not necessary due to EVP_MD_CTX_free()
	delete _chksumImpl( impl );
}

CommSSLChksumSHA512::CommSSLChksumSHA512()
{
	impl = new _CommSSLChksumImpl();
	EVP_DigestInit( _chksumImpl(impl)->ctx,  EVP_sha512() );
}

CommSSLChksumSHA512::~CommSSLChksumSHA512()
{
//	EVP_MD_CTX_cleanup( &_chksumImpl(impl)->ctx ); not necessary due to EVP_MD_CTX_free()
	delete _chksumImpl( impl );
}

void CommSSLChksumSHA512::addData( const BYTE* data, size_t sz )
{
	EVP_DigestUpdate( _chksumImpl( impl )->ctx, data, sz );
}

void CommSSLChksumSHA512::chksum( PBlock& ret )
{
	ret.alloc( EVP_MAX_MD_SIZE );
	unsigned int sz;
	EVP_DigestFinal( _chksumImpl( impl )->ctx, ret.ptr(), &sz );
	PASSERT3( sz <= EVP_MAX_MD_SIZE );
	ret.cut( sz );
}

void CommSSLChksumSHA256::addData( const BYTE* data, size_t sz )
{
	EVP_DigestUpdate( _chksumImpl( impl )->ctx, data, sz );
}

void CommSSLChksumSHA256::chksum( PBlock& ret )
{
	ret.alloc( EVP_MAX_MD_SIZE );
	unsigned int sz;
	EVP_DigestFinal( _chksumImpl( impl )->ctx, ret.ptr(), &sz );
	PASSERT3( sz <= EVP_MAX_MD_SIZE );
	ret.cut( sz );
}


#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
HMAC_CTX * HMAC_CTX_new()
{
	HMAC_CTX * ctx = new HMAC_CTX();
	memset(ctx,0,sizeof(HMAC_CTX));
	HMAC_CTX_init(ctx);
    return ctx;
}
void HMAC_CTX_free(HMAC_CTX * ctx)
{
	HMAC_CTX_cleanup(ctx);
	delete ctx;
}
#endif



struct _CommSSLHMACImpl
{
	HMAC_CTX * ctx;
	_CommSSLHMACImpl()
	{
		ctx = HMAC_CTX_new();
	}
    ~_CommSSLHMACImpl()
	{
		HMAC_CTX_free(ctx);
	}

};

static inline _CommSSLHMACImpl* _hmacImpl( void* p )
{
	return (_CommSSLHMACImpl*)p;
}


	

CommSSLHMAC_SHA256::CommSSLHMAC_SHA256(const BYTE * pwd,int length)
{
	impl = new _CommSSLHMACImpl();
	HMAC_Init_ex( _hmacImpl( impl )->ctx, pwd, length, EVP_sha256(),0 );
}

CommSSLHMAC_SHA256::~CommSSLHMAC_SHA256()
{
//	HMAC_CTX_cleanup(_hmacImpl( impl )->ctx); not necessary due to HMAC_CTX_free()
	delete _hmacImpl( impl );
}

void CommSSLHMAC_SHA256::addData( const BYTE* data, size_t sz )
{
	HMAC_Update( _hmacImpl( impl )->ctx, data, sz );
}

void CommSSLHMAC_SHA256::calculate( PBlock& ret )
{
	ret.alloc( EVP_MAX_MD_SIZE );
	unsigned int sz;
	HMAC_Final( _hmacImpl( impl )->ctx, ret.ptr(), &sz );
	PASSERT3( sz <= EVP_MAX_MD_SIZE );
	ret.cut( sz );
}
