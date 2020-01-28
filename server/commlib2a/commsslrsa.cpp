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

struct _CommSSLRSAImpl
{
	bool publicKey;
	RSA  *rsa;
};

static inline _CommSSLRSAImpl* _rsaImpl( void* p )
{
	return (_CommSSLRSAImpl*)p;
}

CommSSLRSA::CommSSLRSA()
{
	impl = new _CommSSLRSAImpl();
	_rsaImpl( impl )->rsa = 0;
	_rsaImpl( impl )->publicKey = true;
}

void CommSSLRSA::setPublicKey( const BYTE* p, size_t sz )
{
	PASSERT3( _rsaImpl( impl )->rsa == 0 );
#if defined( P_OLD_SSL_VERSION )
	BYTE* pp = (BYTE*)p;
	_rsaImpl( impl )->rsa = d2i_RSA_PUBKEY(0, &pp, sz);
#else
	_rsaImpl( impl )->rsa = d2i_RSA_PUBKEY(0, &p, sz);
#endif
	_rsaImpl( impl )->publicKey = true;
	PASSERT3( _rsaImpl( impl )->rsa );
}

void CommSSLRSA::setPrivateKey( const BYTE* p, size_t sz )
{
	PASSERT3( _rsaImpl( impl )->rsa == 0 );
	EVP_PKEY *prKey;
#if defined( P_OLD_SSL_VERSION )
	BYTE* pp = (BYTE*)p;
	prKey = d2i_PrivateKey( EVP_PKEY_RSA, NULL, &pp, sz );
#else
	prKey = d2i_PrivateKey( EVP_PKEY_RSA, NULL, &p, sz );
#endif
	PASSERT3(prKey);
	_rsaImpl( impl )->rsa = EVP_PKEY_get1_RSA(prKey);
	_rsaImpl( impl )->publicKey = false;
	EVP_PKEY_free(prKey );
	PASSERT3( _rsaImpl( impl )->rsa );
}

void CommSSLRSA::setPrivateKey( const char* fileName, const char* passwd )
{
	PASSERT3( _rsaImpl( impl )->rsa == 0 );
	BIO* in = BIO_new( BIO_s_file() );
	PASSERT3( in );

	int ok = BIO_read_filename( in, fileName );
	PASSERT3( ok > 0 );
	EVP_PKEY *prKey =  PEM_read_bio_PrivateKey( in, NULL, NULL /*ssl->ctx->default_passwd_callback*/, (void*)passwd/* ssl->ctx->default_passwd_callback_userdata */ );
	PASSERT3(prKey);
	_rsaImpl( impl )->rsa = EVP_PKEY_get1_RSA(prKey);
	_rsaImpl( impl )->publicKey = false;

	PASSERT3( _rsaImpl( impl )->rsa );
	BIO_free( in );
	EVP_PKEY_free(prKey );
}


CommSSLRSA::~CommSSLRSA()
{
	if( _rsaImpl( impl )->rsa )
		RSA_free(_rsaImpl( impl )->rsa);
	delete _rsaImpl( impl );
}

void CommSSLEncryptRSA::encrypt( PBlock& ret ,const BYTE* data, size_t sz)
{
	unsigned char * out = (unsigned char *)OPENSSL_malloc(RSA_size(_rsaImpl( impl )->rsa));
	PASSERT3(out);

	int out_len;
	if (_rsaImpl( impl )->publicKey)
		out_len = RSA_public_encrypt(sz, data, out, _rsaImpl( impl )->rsa, RSA_PKCS1_PADDING);
	else
		out_len = RSA_private_encrypt(sz, data, out, _rsaImpl( impl )->rsa, RSA_PKCS1_PADDING);

	PASSERT3(out_len>=0);
	ret.alloc(out_len);
	memcpy(ret.ptr(),out,out_len);
	OPENSSL_free(out);
}


void CommSSLDecryptRSA::decrypt( PBlock& ret ,const BYTE* data, size_t sz)
{
	unsigned char * out = (unsigned char *)OPENSSL_malloc(RSA_size(_rsaImpl( impl )->rsa));
	PASSERT3(out);

	int out_len;
	if (_rsaImpl( impl )->publicKey)
		out_len = RSA_public_decrypt(sz, data, out, _rsaImpl( impl )->rsa, RSA_PKCS1_PADDING);
	else
		out_len = RSA_private_decrypt(sz, data, out, _rsaImpl( impl )->rsa, RSA_PKCS1_PADDING);

	PASSERT3(out_len>=0);
	ret.alloc(out_len);
	memcpy(ret.ptr(),out,out_len);
	OPENSSL_free(out);
}

