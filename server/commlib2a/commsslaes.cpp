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

#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
unsigned char *EVP_CIPHER_CTX_iv_noconst(EVP_CIPHER_CTX *ctx)
{
    return ctx->iv;
}
#endif


struct _CommSSLCipherImpl
{
	EVP_CIPHER_CTX * ctx;
	PBlock buf;
	size_t reserved;

public:
	_CommSSLCipherImpl()
	{
		reserved = 0;
		ctx = EVP_CIPHER_CTX_new();
	}
    ~_CommSSLCipherImpl()
	{
		EVP_CIPHER_CTX_free(ctx);
	}
};

static inline _CommSSLCipherImpl* _cipherImpl( void* p )
{
	return (_CommSSLCipherImpl*)p;
}

#define _ENC_METHOD_AES (block256 ? EVP_aes_256_cbc() : EVP_aes_128_cbc())

//****************************************************************************

CommSSLEncryptAES::CommSSLEncryptAES( const BYTE* pwd, int length, bool block256_, const unsigned char * salt )
{
	block256 = block256_;
	impl = new _CommSSLCipherImpl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD_AES , EVP_md5(), salt, pwd, length, 1, key, iv );
	EVP_EncryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD_AES, key, iv );
}

CommSSLEncryptAES::CommSSLEncryptAES(const BYTE* key, const BYTE* iv, bool block256_ )
{
	block256 = block256_;
	impl = new _CommSSLCipherImpl();
	EVP_EncryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD_AES, key, iv );
}

CommSSLEncryptAES::~CommSSLEncryptAES()
{
	EVP_CIPHER_CTX_cleanup(_cipherImpl( impl )->ctx);
	delete _cipherImpl( impl );
}

void CommSSLEncryptAES::addData( const BYTE* data, size_t sz )
{
	_CommSSLCipherImpl* _impl = (_CommSSLCipherImpl*)impl;
	size_t blk = EVP_CIPHER_CTX_block_size( _impl->ctx );
	if( _impl->reserved < sz + blk )
	{
		_impl->buf.append( sz + blk - _impl->reserved );
		_impl->reserved = sz + blk;
	}
	BYTE* dst = _impl->buf.ptr() + _impl->buf.size() - _impl->reserved;
	int outl;
	EVP_EncryptUpdate( _impl->ctx, dst, &outl, (BYTE*)data, sz );
	PASSERT3( outl <= _impl->reserved );
	_impl->reserved -= outl;
}

void CommSSLEncryptAES::encrypt( PBlock& ret )
{
	_CommSSLCipherImpl* _impl = (_CommSSLCipherImpl*)impl;
	size_t blk = EVP_CIPHER_CTX_block_size( _impl->ctx );
	if( _impl->reserved < blk )
	{
		_impl->buf.append( blk - _impl->reserved );
		_impl->reserved = blk;
	}
	BYTE* dst = _impl->buf.ptr() + _impl->buf.size() - _impl->reserved;
	int outl;
	EVP_EncryptFinal( _impl->ctx, dst, &outl );
	PASSERT3( outl <= _impl->reserved );
	_impl->reserved -= outl;
	_impl->buf.cut( _impl->buf.size() - _impl->reserved );
	ret.moveFrom( _impl->buf );
	_impl->reserved = 0;
	EVP_EncryptInit( _cipherImpl( impl )->ctx, 0, 0, EVP_CIPHER_CTX_iv_noconst(_cipherImpl( impl )->ctx));
}

//****************************************************************************

CommSSLDecryptAES::CommSSLDecryptAES(  const BYTE* pwd, int length, bool block256_,  const unsigned char * salt )
{
	block256 = block256_;
	impl = new _CommSSLCipherImpl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD_AES, EVP_md5(), salt, pwd, length, 1, key, iv );
	EVP_DecryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD_AES, key, iv );
}

CommSSLDecryptAES::CommSSLDecryptAES( const BYTE* key, const BYTE* iv, bool block256_ )
{
	block256 = block256_;
	impl = new _CommSSLCipherImpl();
	EVP_DecryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD_AES, key, iv );
}


CommSSLDecryptAES::~CommSSLDecryptAES()
{
	EVP_CIPHER_CTX_cleanup(_cipherImpl( impl )->ctx);
	delete _cipherImpl( impl );
}

void CommSSLDecryptAES::addData( const BYTE* data, size_t sz )
{
	_CommSSLCipherImpl* _impl = (_CommSSLCipherImpl*)impl;
	size_t blk = EVP_CIPHER_CTX_block_size( _impl->ctx );
	if( _impl->reserved < sz + blk )
	{
		_impl->buf.append( sz + blk - _impl->reserved );
		_impl->reserved = sz + blk;
	}
	BYTE* dst = _impl->buf.ptr() + _impl->buf.size() - _impl->reserved;
	int outl;
	EVP_DecryptUpdate( _impl->ctx, dst, &outl, (BYTE*)data, sz );
	PASSERT3( outl <= _impl->reserved );
	_impl->reserved -= outl;
}

void CommSSLDecryptAES::decrypt( PBlock& ret )
{
	_CommSSLCipherImpl* _impl = (_CommSSLCipherImpl*)impl;
	size_t blk = EVP_CIPHER_CTX_block_size( _impl->ctx );
	if( _impl->reserved < blk )
	{
		_impl->buf.append( blk - _impl->reserved );
		_impl->reserved = blk;
	}
	BYTE* dst = _impl->buf.ptr() + _impl->buf.size() - _impl->reserved;
	int outl;
	EVP_DecryptFinal( _impl->ctx, dst, &outl );
	PASSERT3( outl <= _impl->reserved );
	_impl->reserved -= outl;
	_impl->buf.cut( _impl->buf.size() - _impl->reserved );
	ret.moveFrom( _impl->buf );
	_impl->reserved = 0;
	EVP_DecryptInit( _cipherImpl( impl )->ctx, 0, 0, EVP_CIPHER_CTX_iv_noconst(_cipherImpl( impl )->ctx));
}





