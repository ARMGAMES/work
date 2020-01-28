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

struct _CommSSLCipherImpl
{
	EVP_CIPHER_CTX *ctx;
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

#define _ENC_METHOD EVP_des_cbc()

//****************************************************************************

CommSSLEncrypt::CommSSLEncrypt( const char* pwd )
{
	impl = new _CommSSLCipherImpl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD, EVP_md5(), NULL, (BYTE*)pwd, strlen( pwd ), 1, key, iv );
	EVP_EncryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD, key, iv );
}

CommSSLEncrypt::CommSSLEncrypt(const BYTE* key, const BYTE * iv )
{
	impl = new _CommSSLCipherImpl();
	EVP_EncryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD, key, iv );
}

CommSSLEncrypt::~CommSSLEncrypt()
{
	EVP_CIPHER_CTX_cleanup(_cipherImpl( impl )->ctx);
	delete _cipherImpl( impl );
}

void CommSSLEncrypt::addData( const BYTE* data, size_t sz )
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

void CommSSLEncrypt::encrypt( PBlock& ret )
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
}

//****************************************************************************

CommSSLDecrypt::CommSSLDecrypt( const char* pwd )
{
	impl = new _CommSSLCipherImpl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD, EVP_md5(), NULL, (BYTE*)pwd, strlen( pwd ), 1, key, iv );
	EVP_DecryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD, key, iv );
}

CommSSLDecrypt::CommSSLDecrypt( const BYTE* key, const BYTE * iv )
{
	impl = new _CommSSLCipherImpl();
	EVP_DecryptInit( _cipherImpl( impl )->ctx, _ENC_METHOD, key, iv );
}


CommSSLDecrypt::~CommSSLDecrypt()
{
	EVP_CIPHER_CTX_cleanup(_cipherImpl( impl )->ctx);
	delete _cipherImpl( impl );
}

void CommSSLDecrypt::addData( const BYTE* data, size_t sz )
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

void CommSSLDecrypt::decrypt( PBlock& ret )
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
}








struct _CommSSLStreamCipherImpl
{
	EVP_CIPHER_CTX *ctx;
	_CommSSLStreamCipherImpl()
	{
		ctx = EVP_CIPHER_CTX_new();
	}
	~_CommSSLStreamCipherImpl()
	{
		EVP_CIPHER_CTX_free(ctx);
	}


};

static inline _CommSSLStreamCipherImpl* _streamCipherImpl( void* p )
{
	return (_CommSSLStreamCipherImpl*)p;
}

#define _ENC_METHOD_STREAM EVP_rc4()

//****************************************************************************

CommSSLStreamEncrypt::CommSSLStreamEncrypt(  const char* pwd, const unsigned char * salt )
{
	_init((const BYTE*)pwd,strlen(pwd),salt);
}

CommSSLStreamEncrypt::CommSSLStreamEncrypt(  const BYTE* pwd, int pwdLength, const unsigned char * salt )
{
	_init(pwd,pwdLength,salt);
}
void CommSSLStreamEncrypt::_init( const BYTE* pwd, int pwdLength, const unsigned char * salt )
{
	impl = new _CommSSLStreamCipherImpl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD_STREAM, EVP_md5(), salt, (BYTE*)pwd, pwdLength, 1, key, iv );
	EVP_EncryptInit( _streamCipherImpl( impl )->ctx, _ENC_METHOD_STREAM, key, iv );
}
CommSSLStreamEncrypt::~CommSSLStreamEncrypt()
{
	EVP_CIPHER_CTX_cleanup(_streamCipherImpl( impl )->ctx);
	delete _streamCipherImpl( impl );
}

void CommSSLStreamEncrypt::encrypt( const BYTE* data, size_t sz, PBlock & ret )
{
	_CommSSLStreamCipherImpl* _impl = (_CommSSLStreamCipherImpl*)impl;
	ret.append( sz );
	int outl;
	EVP_EncryptUpdate( _impl->ctx, ret.ptr(), &outl, (BYTE*)data, sz );
}

void CommSSLStreamEncrypt::encrypt( const BYTE* data, size_t sz, BYTE * ret )
{
	_CommSSLStreamCipherImpl* _impl = (_CommSSLStreamCipherImpl*)impl;
	int outl;
	EVP_EncryptUpdate( _impl->ctx, ret, &outl, (BYTE*)data, sz );
}

//****************************************************************************

CommSSLStreamDecrypt::CommSSLStreamDecrypt( const char* pwd, const unsigned char * salt )
{
	_init( (const BYTE*)pwd,strlen(pwd),salt);
}

CommSSLStreamDecrypt::CommSSLStreamDecrypt( const BYTE* pwd, int pwdLength, const unsigned char * salt )
{
	_init( pwd,pwdLength,salt);
}
void CommSSLStreamDecrypt::_init(const BYTE* pwd, int pwdLength, const unsigned char * salt)
{
	impl = new _CommSSLStreamCipherImpl();
	BYTE key[ EVP_MAX_KEY_LENGTH ];
	BYTE iv[ EVP_MAX_IV_LENGTH ];
	EVP_BytesToKey( _ENC_METHOD_STREAM, EVP_md5(), salt, (BYTE*)pwd, pwdLength, 1, key, iv );
	EVP_DecryptInit( _streamCipherImpl( impl )->ctx, _ENC_METHOD_STREAM, key, iv );
}
CommSSLStreamDecrypt::~CommSSLStreamDecrypt()
{
	EVP_CIPHER_CTX_cleanup(_streamCipherImpl( impl )->ctx);
	delete _streamCipherImpl( impl );
}

void CommSSLStreamDecrypt::decrypt( const BYTE* data, size_t sz, PBlock &ret )
{
	_CommSSLStreamCipherImpl* _impl = (_CommSSLStreamCipherImpl*)impl;
	ret.append( sz );
	int outl;
	EVP_DecryptUpdate( _impl->ctx, ret.ptr(), &outl, (BYTE*)data, sz );
}

void CommSSLStreamDecrypt::decrypt( const BYTE* data, size_t sz,  BYTE * ret  )
{
	_CommSSLStreamCipherImpl* _impl = (_CommSSLStreamCipherImpl*)impl;
	int outl;
	EVP_DecryptUpdate( _impl->ctx, ret, &outl, (BYTE*)data, sz );
}
