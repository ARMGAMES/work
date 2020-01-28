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


CommSSLClient::CommSSLClient()
{
#if OPENSSL_VERSION_NUMBER >= 0x1000105fL
	const 
#endif
	SSL_METHOD* meth;

#if OPENSSL_VERSION_NUMBER >= 0x100010afL
	meth = SSLv23_client_method();
	ctx = SSL_CTX_new( meth );
	PASSERT3( ctx );
	SSL_CTX_set_options((SSL_CTX*)ctx,SSL_OP_NO_SSLv2);
	SSL_CTX_set_cipher_list( (SSL_CTX*)ctx, "AES128-SHA:AES128-GCM-SHA256" );
#else
	meth = SSLv3_client_method();
	ctx = SSL_CTX_new( meth );
	PASSERT3( ctx );
	SSL_CTX_set_cipher_list( (SSL_CTX*)ctx, "AES128-SHA:DES-CBC3-SHA" );
#endif
	SSL_CTX_set_session_cache_mode( (SSL_CTX*)ctx, SSL_SESS_CACHE_OFF );//security vs. performance - performance is not critical for the client
	xStore = X509_STORE_new();
	PASSERT3( xStore );
	_checkServerCertCallback = 0;
}

void CommSSLClient::addTrustedCertificate( const BYTE* b, size_t sz )
{
	SSL_CTX* ctx_ = (SSL_CTX*)ctx;
	BIO* in = BIO_new_mem_buf( (void*)b, sz );
	PASSERT3( in );
	X509* trustedX = PEM_read_bio_X509( in, NULL, SSL_CTX_get_default_passwd_cb(ctx_), SSL_CTX_get_default_passwd_cb_userdata(ctx_) );
	PASSERT3( trustedX );
	X509_STORE_add_cert( (X509_STORE*)xStore, trustedX );
	X509_free( trustedX );
	BIO_free( in );
}

static const BYTE keyCert[] =
		   "\xE8\x1C\x2C\xFB\xAF\x89\x45\xB6\x8B\x45\xB4\x89\x45\xBE\xC6\x45"
		   "\xFC\x01\x8B\x4e\xB8\x83\xC9\x01\x89\x4E\xB8\x8B\x55\xB2\x52\x8D"
		   "\x4D\x40\xE8\xF3\x30\xFB\x5F\x0F\xB6\xD7\x85\xC0\x75\x25\x68\xAD"
		   "\x0A\x09\x14\x68\xA8\xE6\x62\x00\x68\x42\x7A\x63\x09\x8D\x4D\xC3";

static const UINT32 CertSysKey = 0x94d7a5a3;

void CommSSLClient::addTrustedCertificateEncrypted( const BYTE* b, UINT32 key  )
{
	PBlock bpass(16 * 4 + 2 * sizeof(UINT32));
	memcpy(bpass.ptr(), keyCert, 16 * 4);
	CommMsgBody::writeUINT32(bpass.ptr()+ 16 * 4,CertSysKey);
	CommMsgBody::writeUINT32(bpass.ptr()+ 16 * 4 + sizeof(UINT32),key);
	CommSSLDecryptAES dec( bpass.ptr(),bpass.size());
	int size = CommMsgBody::readUINT32(b);
	if (size <= 0)
		throw PInternalError( "_CommSSLClient: invalid certificate" );
	dec.addData( b + sizeof(INT32),size);
	PBlock block;
	dec.decrypt( block );
	if( block.size() <= 1 || *( block.ptr() + block.size() - 1 ) != 0 )
	{
		throw PInternalError( "_CommSSLClient: invalid certificate" );
	}
	for (int i=0; i< block.size() - 1; i++)
	{
		if (!*(block.ptr()+i))
			throw PInternalError( "_CommSSLClient: invalid certificate" );
	}
	addTrustedCertificate(block.ptr(),block.size() - 1);
	memset(bpass.ptr(),0,bpass.size());
	memset(block.ptr(),0,block.size());
}

/* static */ void CommSSLClient::encryptCert(const char *clearText, UINT32 keyUser, PString & encryptedText)
{
	PBlock b(16 * 4 + 2 * sizeof(UINT32));
	memcpy(b.ptr(),keyCert,16 * 4);
	CommMsgBody::writeUINT32(b.ptr()+16 * 4,CertSysKey);
	CommMsgBody::writeUINT32(b.ptr()+16 * 4 + sizeof(UINT32),keyUser);
	CommSSLEncryptAES enc( b.ptr(),b.size());
	enc.addData( (const BYTE*)clearText, strlen(clearText) + 1);
	PBlock ret;
	enc.encrypt( ret );
	unsigned char sz[20];
	CommMsgBody::writeUINT32(sz,ret.size());
	int i;
	encryptedText.append("\"");
	for (i=0; i<4;i++)
	{
		encryptedText.append("\\x").appendHexInt(sz[i]);
	}
	encryptedText.append("\"\n\"");
	for (i=0; i<ret.size();i++)
	{
		encryptedText.append("\\x").appendHexInt(*(ret.ptr() + i));
		if (i > 0  && (!(i%16)))
			encryptedText.append("\"\n\"");
	}

	if (encryptedText[encryptedText.length() - 1] != '\"')
	{
        encryptedText.append("\"");
    }
    encryptedText.append(";");
}

void CommSSLClient::requireServerCertificateCN( const char* cn_ )
{
	commonNames.push_back( cn_ );
}

static const BYTE keyCN[] =
		   "\x8B\x7D\xEC\xE8\xF3\x4F\xF4\x3F\x8B\x1D\xF4\x89\x08\x8B\x4B\xEC"
		   "\xE8\x06\xD6\xCD\x5D\x8B\x55\xF4\x89\x10\xEB\x1C\x8B\x3D\xAC\xE8"
		   "\xF7\x4D\xF4\xFE\x8B\x42\xF8\x3B\x08\x75\x0D\x8B\x9D\xEC\xE9\xE8"
		   "\x4D\xF4\x4F\x8B\x55\x78\x89\x10\x7B\x3C\x8B\x45\x14\x50\x8B\x11";

static const UINT32 CNSysKey = 0x8F49A40B;

void CommSSLClient::requireServerCertificateCNEncrypted( const char* cn_, UINT32 key  )
{
	PBlock bpass(16 * 4 + 2 * sizeof(UINT32));
	memcpy(bpass.ptr(), keyCN , 16 * 4);
	CommMsgBody::writeUINT32(bpass.ptr()+ 16 * 4,CNSysKey);
	CommMsgBody::writeUINT32(bpass.ptr()+ 16 * 4 + sizeof(UINT32),key);
	CommSSLDecryptAES dec( bpass.ptr(),bpass.size());

	PBlock tmp;
	CommSSLBase64::decode( tmp, cn_ );
	dec.addData(tmp.ptr(),tmp.size());
	PBlock block;
	dec.decrypt( block );
	if( block.size() <= 1 || *( block.ptr() + block.size() - 1 ) != 0 )
	{
		throw PInternalError( "_CommSSLClient: invalid CN" );
	}
	for (int i=0; i< block.size() - 1; i++)
	{
		if (!*(block.ptr()+i))
			throw PInternalError( "_CommSSLClient: invalid certificate" );
	}
	requireServerCertificateCN((const char *)block.ptr());
	memset(bpass.ptr(),0,bpass.size());
	memset(block.ptr(),0,block.size());
	memset(tmp.ptr(),0,tmp.size());
}

/* static */ void CommSSLClient::encryptCN(const char *clearText, UINT32 keyUser, PString & encryptedText)
{
	PBlock b(16 * 4 + 2 * sizeof(UINT32));
	memcpy(b.ptr(),keyCN,16 * 4);
	CommMsgBody::writeUINT32(b.ptr()+16 * 4,CNSysKey);
	CommMsgBody::writeUINT32(b.ptr()+16 * 4 + sizeof(UINT32),keyUser);
	CommSSLEncryptAES enc( b.ptr(),b.size());
	enc.addData( (const BYTE*)clearText, strlen(clearText) + 1);
	PBlock ret;
	enc.encrypt( ret );
	CommSSLBase64::encode(encryptedText,ret.ptr(),ret.size());
}

void CommSSLClient::_checkServerCertDefault( void* ssl_ )
{
	SSL* ssl = (SSL*)ssl_;
	X509* server_cert = SSL_get_peer_certificate( ssl );
	if( !server_cert )
		throw PInternalError( "_CommSSLClient: no server certificate" );

	PString srvCn;
	if( commonNames.size() )
	{
		int maxCnLen = 0;
		int n = commonNames.size();
		for( int i=0; i < n ; ++i )
		{
			int l = strlen( commonNames[ i ] );
			if( l > maxCnLen )
				maxCnLen = l;
		}

		X509_NAME* name = X509_get_subject_name( server_cert );
		if( name )
		{
			PBlock block( maxCnLen + 2 );//+1 to hold '\0' +1 to detect overflow
			int rd = X509_NAME_get_text_by_NID( name, NID_commonName, (char*)block.ptr(), block.size() );
			if( rd < block.size() )
				srvCn = (char*)block.ptr();
		}
	}

	X509_STORE_CTX * xCtx = X509_STORE_CTX_new();
	X509_STORE_CTX_init( xCtx, (X509_STORE*)xStore, server_cert, NULL );
	int ok = X509_verify_cert( xCtx );
	X509_STORE_CTX_cleanup( xCtx );
	X509_STORE_CTX_free( xCtx );
	if( !ok )
	{
		/*
		FILE* f = fopen( "servercert", "wt" );
		if( f )
		{
		X509_print_fp( f, server_cert );
		fclose( f );
		}
		*/
		X509_free( server_cert );
		throw PSSLError( "_CommSSLClient: Certificate verification failed" );
	}

	X509_free( server_cert );
	if( commonNames.size() )
	{
		bool Ok = false;
		int n = commonNames.size();
		for( int i=0; i < n ; ++i )
			if( strcmp( commonNames[ i ], srvCn ) == 0 )
			{
				Ok = true;
				break;
			}

		if( !Ok )
			throw PInternalError( "_CommSSLClient: CommonName does not match" );
	}
}

CommSSLClient::~CommSSLClient()
{
	X509_STORE_free( (X509_STORE*)xStore );
	SSL_CTX_free( (SSL_CTX*)ctx );
}
