#include "ppincludebase.h"//_before_ openssl headers

#include <openssl/crypto.h>
//#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commssl.h"

#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
static pem_password_cb *SSL_CTX_get_default_passwd_cb(SSL_CTX *ctx)
{
    return ctx->default_passwd_callback;
}

static void *SSL_CTX_get_default_passwd_cb_userdata(SSL_CTX *ctx)
{
    return ctx->default_passwd_callback_userdata;
}
#endif


static int tlsext_servername_callback(SSL *ssl, int *ad, void *arg)
{
	if (arg)
		return ((CommSSLServer *) arg)->checkSni(ssl);
	else
		return SSL_TLSEXT_ERR_NOACK;
}


//static int comm_pwd_callback( char* buf, int num, int w, void *key )

CommSSLServer::CommSSLServer()
{
	ctx=_createCtx();
}

int CommSSLServer::checkSni(void *ssl)
{
    if (ssl == NULL)
        return SSL_TLSEXT_ERR_NOACK;

    const char* sni = SSL_get_servername((SSL *)ssl, TLSEXT_NAMETYPE_host_name);
	if (!sni)
		return SSL_TLSEXT_ERR_NOACK;
	PTRACE4("tlsext_servername_callback: %s\n", sni);
	PStringMap< void *>::iterator it;
	it = sniCtx.find(sni);
	if (it != sniCtx.end())
	{
		SSL_set_SSL_CTX((SSL *)ssl,(SSL_CTX*)(*it).second);
		return SSL_TLSEXT_ERR_OK;
	}
	else
		return SSL_TLSEXT_ERR_NOACK;

}


void * CommSSLServer::_createCtx()
{
#if OPENSSL_VERSION_NUMBER >= 0x1000105fL
	const 
#endif
	SSL_METHOD* meth;
	void * _ctx;

#if OPENSSL_VERSION_NUMBER >= 0x100010afL
	meth = SSLv23_server_method();
	_ctx = SSL_CTX_new( meth );
	PASSERT3( _ctx );
	SSL_CTX_set_options((SSL_CTX*)_ctx,SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3|SSL_OP_NO_TLSv1|SSL_OP_NO_TLSv1_1);
	SSL_CTX_set_cipher_list( (SSL_CTX*)_ctx, "AES128-GCM-SHA256" );
#else
	meth = SSLv3_server_method();
	_ctx = SSL_CTX_new( meth );
	PASSERT3( _ctx );
	SSL_CTX_set_cipher_list( (SSL_CTX*)_ctx, "AES128-SHA:DES-CBC3-SHA" );
#endif

	SSL_CTX_set_session_cache_mode( (SSL_CTX*)_ctx, SSL_SESS_CACHE_SERVER );//security vs. performance
	SSL_CTX_set_timeout( (SSL_CTX*)_ctx, 60 );//security vs. performance - 1 minute timeout
#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable:4191)
#endif
	SSL_CTX_set_tlsext_servername_callback((SSL_CTX*)_ctx,tlsext_servername_callback);
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
	SSL_CTX_set_tlsext_servername_arg((SSL_CTX*)_ctx, (void *)this);
	return _ctx;
}

void CommSSLServer::_setKeyCert( void* ctx_, const char* keyFile, const char* certFile, const char* password)
{
	SSL_CTX_set_default_passwd_cb_userdata(  (SSL_CTX*)ctx_, (void*)password );

	int ok = SSL_CTX_use_certificate_file( (SSL_CTX*) ctx_, certFile, SSL_FILETYPE_PEM );
	PASSERT3( ok > 0 );
	ok = SSL_CTX_use_PrivateKey_file(  (SSL_CTX*)ctx_, keyFile, SSL_FILETYPE_PEM );
	PASSERT3( ok > 0 );
	ok = SSL_CTX_check_private_key(  (SSL_CTX*)ctx_);
	PASSERT3( ok );
	SSL_CTX_set_default_passwd_cb_userdata(  (SSL_CTX*)ctx_, (void*)0);
}
void CommSSLServer::_setKeyCertFromMemory( void* ctx_, const BYTE * keyFileContent, size_t keyFileContentSize,  
										 const BYTE * certFileContent, size_t certFileContentSize,
										 const char* password)
{
	SSL_CTX_set_default_passwd_cb_userdata(  (SSL_CTX*)ctx_, (void*)password );
	BIO* in = BIO_new_mem_buf( (void*)certFileContent, certFileContentSize );
	PASSERT3( in );
	X509* x = PEM_read_bio_X509( in, NULL, SSL_CTX_get_default_passwd_cb( (SSL_CTX*)ctx_), SSL_CTX_get_default_passwd_cb_userdata( (SSL_CTX*)ctx_));
	PASSERT3 (x);
	int ok = SSL_CTX_use_certificate(  (SSL_CTX*)ctx_, x);
	PASSERT3( ok > 0 );
	X509_free(x);
	BIO_free(in);

	in = BIO_new_mem_buf( (void*)keyFileContent, keyFileContentSize );
	PASSERT3(in);
	EVP_PKEY *pkey=PEM_read_bio_PrivateKey(in,NULL, SSL_CTX_get_default_passwd_cb( (SSL_CTX*)ctx_), SSL_CTX_get_default_passwd_cb_userdata( (SSL_CTX*)ctx_));
	PASSERT3(pkey);
	ok=SSL_CTX_use_PrivateKey( (SSL_CTX*)ctx_,pkey);
	PASSERT3( ok > 0 );
	EVP_PKEY_free(pkey);
	BIO_free(in);
	ok = SSL_CTX_check_private_key(  (SSL_CTX*)ctx_ );
	PASSERT3( ok );
	SSL_CTX_set_default_passwd_cb_userdata(  (SSL_CTX*)ctx_, (void*)0);
}

void CommSSLServer::setKeyCert( const char* keyFile, const char* certFile, const char* password, const char * sniName )
{
	if (!sniName)
	{
		passwd = password;
		_setKeyCert(ctx,keyFile,certFile,password);
	}
	else
	{
		void * _ctx = _createCtx();
		_setKeyCert(_ctx,keyFile,certFile,password);
		sniCtx.insert(sniName,_ctx);
	}
}

void CommSSLServer::setKeyCertFromMemory( const BYTE * keyFileContent, size_t keyFileContentSize,  
										 const BYTE * certFileContent, size_t certFileContentSize,
										 const char* password, const char * sniName )
{
	if (!sniName)
	{
		passwd = password;
		_setKeyCertFromMemory(ctx,keyFileContent, keyFileContentSize, certFileContent, certFileContentSize, password);
	}
	else
	{
		void * _ctx = _createCtx();
		_setKeyCertFromMemory(_ctx,keyFileContent, keyFileContentSize, certFileContent, certFileContentSize, password);
		sniCtx.insert(sniName,_ctx);
	}
}

CommSSLServer::~CommSSLServer()
{
	SSL_CTX_free( (SSL_CTX*)ctx );
	PStringMap<void *>::iterator it;
	for (it = sniCtx.begin(); it != sniCtx.end(); ++it)
	{
		SSL_CTX_free( (SSL_CTX*)(*it).second );
	}

}
