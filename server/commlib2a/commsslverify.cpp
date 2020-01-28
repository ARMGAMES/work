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

struct _CommSSLVerifyImpl
{
	EVP_MD_CTX *ctx;
	EVP_PKEY *pubKey;
	_CommSSLVerifyImpl()
	{
		ctx = EVP_MD_CTX_create();
	}
	~_CommSSLVerifyImpl()
	{
		EVP_MD_CTX_destroy(ctx);
	}

};

static inline _CommSSLVerifyImpl* _verifyImpl( void* p )
{
	return (_CommSSLVerifyImpl*)p;
}

_CommSSLVerifyBase::_CommSSLVerifyBase()
{
	impl = new _CommSSLVerifyImpl();
	_verifyImpl( impl )->pubKey = 0;
}

void _CommSSLVerifyBase::setPublicKey( const BYTE* p, size_t sz )
{
	PASSERT3( _verifyImpl( impl )->pubKey == 0 );
#if defined( P_OLD_SSL_VERSION )
	BYTE* pp = (BYTE*)p;
	_verifyImpl( impl )->pubKey = d2i_PublicKey( EVP_PKEY_RSA, NULL, &pp, sz );
#else
	//	BYTE* pp = (BYTE*)p;
	_verifyImpl( impl )->pubKey = d2i_PublicKey( EVP_PKEY_RSA, NULL, &p, sz );
#endif
	PASSERT3( _verifyImpl( impl )->pubKey );

	//	_verifyImpl( impl )->pubKey = PEM_read_bio_PublicKey( in, NULL, NULL /*ssl->ctx->default_passwd_callback*/, (void*)passwd/* ssl->ctx->default_passwd_callback_userdata */ );
}


void _CommSSLVerifyBase::setPublicKey( const char* fileName, const char* passwd )
{
	PASSERT3( _verifyImpl( impl )->pubKey == 0 );
	BIO* in = BIO_new( BIO_s_file() );
	PASSERT3( in );

	int ok = BIO_read_filename( in, fileName );
	PASSERT3( ok > 0 );

	_verifyImpl( impl )->pubKey = PEM_read_bio_PUBKEY( in, NULL, NULL /*ssl->ctx->default_passwd_callback*/, (void*)passwd/* ssl->ctx->default_passwd_callback_userdata */ );
	PASSERT3( _verifyImpl( impl )->pubKey );
	BIO_free( in );
}


_CommSSLVerifyBase::~_CommSSLVerifyBase()
{
//	EVP_MD_CTX_cleanup( _verifyImpl( impl )->ctx ); not necessary due to EVP_MD_CTX_free()
	if( _verifyImpl( impl )->pubKey )
		EVP_PKEY_free( _verifyImpl( impl )->pubKey );
	delete _verifyImpl( impl );
}

void _CommSSLVerifyBase::addData( const BYTE* data, size_t sz )
{
	EVP_VerifyUpdate( _verifyImpl( impl )->ctx, data, sz );
}

void _CommSSLVerifyBase::verifyData( const BYTE* sign, size_t signSz )
{
	int ok = EVP_VerifyFinal( _verifyImpl( impl )->ctx, (BYTE*)sign, signSz, _verifyImpl( impl )->pubKey );
	if( !ok )
		throw PInternalError( "CommSSLVerify: verify failed" );
}

CommSSLVerify::CommSSLVerify()
{
	EVP_VerifyInit( _verifyImpl( impl )->ctx, EVP_sha1() );
}
CommSSLVerify256::CommSSLVerify256()
{
	EVP_VerifyInit( _verifyImpl( impl )->ctx, EVP_sha256() );
}
