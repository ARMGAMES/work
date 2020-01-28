#include "ppincludebase.h"//_before_ openssl headers

#include <openssl/crypto.h>
//#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "commssl.h"

/* static */ void CommSSL::recryptPrivateKey( const char* inFileName, const char* pwd, const char* outFileName, const char* newPwd )
{
	BIO* in = BIO_new( BIO_s_file() );
	PASSERT3( in );
	int ok = BIO_read_filename( in, inFileName );
	PASSERT3( ok > 0 );
	EVP_PKEY* pkey = PEM_read_bio_PrivateKey( in, NULL, NULL /*default_passwd_callback*/, (void*)pwd /*default_passwd_callback_userdata*/ );
	PASSERT3( pkey );
	BIO_free( in );

	BIO* out = BIO_new( BIO_s_file() );
	PASSERT3( out );
	ok = BIO_write_filename( out, (char*)outFileName );
	PASSERT3( ok > 0 );
	const EVP_CIPHER* enc = EVP_des_ede3_cbc();
	PEM_write_bio_PrivateKey( out, pkey, enc, NULL, 0, NULL /*default_passwd_callback*/, (void*)newPwd /*default_passwd_callback_userdata*/ );
	EVP_PKEY_free( pkey );
	BIO_free( out );
}
