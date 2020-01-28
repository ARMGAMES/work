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

struct _CommSSLSignImpl
{
	EVP_MD_CTX * ctx;
	EVP_PKEY *privKey;
	_CommSSLSignImpl()
	{
		ctx = EVP_MD_CTX_create();
	}
	~_CommSSLSignImpl()
	{
		EVP_MD_CTX_destroy(ctx);
	}
};

static inline _CommSSLSignImpl* _signImpl( void* p )
{
	return (_CommSSLSignImpl*)p;
}

_CommSSLSignBase::_CommSSLSignBase()
{
	impl = new _CommSSLSignImpl();
	_signImpl( impl )->privKey = 0;
}

void _CommSSLSignBase::setPrivateKey( const char* fileName, const char* passwd )
{
	PASSERT3( _signImpl( impl )->privKey == 0 );
	BIO* in = BIO_new( BIO_s_file() );
	PASSERT3( in );

	int ok = BIO_read_filename( in, fileName );
	PASSERT3( ok > 0 );

	_signImpl( impl )->privKey = PEM_read_bio_PrivateKey( in, NULL, NULL /*ssl->ctx->default_passwd_callback*/, (void*)passwd/* ssl->ctx->default_passwd_callback_userdata */ );
	PASSERT3( _signImpl( impl )->privKey );
	BIO_free( in );
}

void _CommSSLSignBase::setPrivateKey( const BYTE* p, size_t sz )
{
	PASSERT3( _signImpl( impl )->privKey == 0 );
	_signImpl( impl )->privKey = d2i_PrivateKey( EVP_PKEY_RSA, NULL, &p, sz );
	PASSERT3( _signImpl( impl )->privKey );
}

_CommSSLSignBase::~_CommSSLSignBase()
{
	if( _signImpl( impl )->privKey )
		EVP_PKEY_free( _signImpl( impl )->privKey );
//	EVP_MD_CTX_cleanup( _signImpl( impl )->ctx ); not necessary due to EVP_MD_CTX_destroy()
	delete _signImpl( impl );
}

void _CommSSLSignBase::addData( const BYTE* data, size_t sz )
{
	EVP_SignUpdate( _signImpl( impl )->ctx, data, sz );
}

void _CommSSLSignBase::sign( PBlock& ret )
{
	size_t bufSz = EVP_PKEY_size( _signImpl( impl )->privKey );//???
	ret.alloc( bufSz );
	unsigned int sz;
	int ok = EVP_SignFinal( _signImpl( impl )->ctx, ret.ptr(), &sz, _signImpl( impl )->privKey );
	PASSERT3( ok > 0 );
	PASSERT3( sz <= bufSz );
	ret.cut( sz );
}

/* static */ void _CommSSLSignBase::publicKeyFromCert( PBlock& ret, const char* fileName )
{
	BIO* in = BIO_new( BIO_s_file() );
	PASSERT3( in );

	int ok = BIO_read_filename( in, fileName );
	PASSERT3( ok > 0 );

	X509* x = PEM_read_bio_X509( in, NULL, NULL, NULL );
	PASSERT3( x );

	EVP_PKEY* key = X509_get_pubkey( x );
	PASSERT3( key );
	int len = i2d_PublicKey( key, NULL );
	ret.alloc( len );
	BYTE* p = ret.ptr();
	ok = i2d_PublicKey( key, &p );
	PASSERT3( ok > 0 );

	X509_free( x );
	BIO_free( in );
}

CommSSLSign::CommSSLSign()
{
	EVP_SignInit( _signImpl( impl )->ctx, EVP_sha1() );
}
CommSSLSign256::CommSSLSign256()
{
	EVP_SignInit( _signImpl( impl )->ctx, EVP_sha256() );
}
