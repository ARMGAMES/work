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

struct _CommSSLCipher3Impl
{
	EVP_CIPHER_CTX *ctx;
	PBlock buf;
	size_t reserved;

public:
	_CommSSLCipher3Impl()
	{
		reserved = 0;
		ctx = EVP_CIPHER_CTX_new();
	}
	~_CommSSLCipher3Impl()
	{
		EVP_CIPHER_CTX_free(ctx);
	}
};

static inline _CommSSLCipher3Impl* _cipher3Impl( void* p )
{
	return (_CommSSLCipher3Impl*)p;
}

#define _ENC_METHOD3 EVP_des_ede3_cbc()

//****************************************************************************

CommSSLEncrypt3::CommSSLEncrypt3( const char* pwd )
{
	impl = new _CommSSLCipher3Impl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD3, EVP_md5(), NULL, (BYTE*)pwd, strlen( pwd ), 1, key, iv );
	EVP_EncryptInit( _cipher3Impl( impl )->ctx, _ENC_METHOD3, key, iv );
}

CommSSLEncrypt3::CommSSLEncrypt3( const BYTE* key, const BYTE * iv )
{
	impl = new _CommSSLCipher3Impl();
	EVP_EncryptInit( _cipher3Impl( impl )->ctx, _ENC_METHOD3, key, iv );
}

CommSSLEncrypt3::~CommSSLEncrypt3()
{
	EVP_CIPHER_CTX_cleanup(_cipher3Impl( impl )->ctx);
	delete _cipher3Impl( impl );
}

void CommSSLEncrypt3::addData( const BYTE* data, size_t sz )
{
	_CommSSLCipher3Impl* _impl = (_CommSSLCipher3Impl*)impl;
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

void CommSSLEncrypt3::encrypt( PBlock& ret )
{
	_CommSSLCipher3Impl* _impl = (_CommSSLCipher3Impl*)impl;
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
}

//****************************************************************************

CommSSLDecrypt3::CommSSLDecrypt3( const char* pwd )
{
	impl = new _CommSSLCipher3Impl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD3, EVP_md5(), NULL, (BYTE*)pwd, strlen( pwd ), 1, key, iv );
	EVP_DecryptInit( _cipher3Impl( impl )->ctx, _ENC_METHOD3, key, iv );
}
CommSSLDecrypt3::CommSSLDecrypt3( const BYTE* key, const BYTE * iv )
{
	impl = new _CommSSLCipher3Impl();
	EVP_DecryptInit( _cipher3Impl( impl )->ctx, _ENC_METHOD3, key, iv );
}

CommSSLDecrypt3::~CommSSLDecrypt3()
{
	EVP_CIPHER_CTX_cleanup(_cipher3Impl( impl )->ctx);
	delete _cipher3Impl( impl );
}

void CommSSLDecrypt3::addData( const BYTE* data, size_t sz )
{
	_CommSSLCipher3Impl* _impl = (_CommSSLCipher3Impl*)impl;
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

void CommSSLDecrypt3::decrypt( PBlock& ret )
{
	_CommSSLCipher3Impl* _impl = (_CommSSLCipher3Impl*)impl;
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
}
