#include "ppincludebase.h"//_before_ openssl headers

//****************************************************************************

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

//****************************************************************************

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "ppsocket.h"
//#include "_commcomm.h"
#include "commssl.h"
//#include "_commssl.h"

#ifndef PNOTHREADS
static unsigned long _id_callback()
{
	return THREADIDREAL();
}
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
static PCriticalSection _sslLocks[ CRYPTO_NUM_LOCKS ];
#else
static PCriticalSection _sslLocks[ CRYPTO_num_locks() ];
#endif
static void _locking_callback( int mode, int num, const char* file, int line )
{
	PASSERT3( num >= 0 && num < CRYPTO_num_locks() );
	if( mode & CRYPTO_LOCK )
		_sslLocks[ num ]._lock();
	else
		_sslLocks[ num ]._unlock();
}

static PCriticalSection _startupCs;
#endif

static int _started = 0;
/* static */ void CommSSL::startup()
{
#ifndef PNOTHREADS
	PLock lock( _startupCs );
#endif
	if( !_started )
	{
		BYTE dummy[8];
#ifndef PNOTHREADS
		CRYPTO_set_id_callback( _id_callback );
		CRYPTO_set_locking_callback( _locking_callback );
#endif
		SSL_load_error_strings();
		SSL_library_init();
		RAND_bytes(dummy,8);
	}
	++_started;
}

/* static */ void CommSSL::cleanup()
{
#ifndef PNOTHREADS
	PLock lock( _startupCs );
#endif
	if( _started > 0 )
	{
		if( --_started == 0 )
		{
			EVP_cleanup();
			CRYPTO_cleanup_all_ex_data();
			ERR_free_strings();
		}
	}
}

/* static */ void CommSSL::threadCleanup()
{
	ERR_remove_state(0);
}