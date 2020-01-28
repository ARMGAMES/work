#include "ppincludebase.h"//_before_ openssl headers

//****************************************************************************

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

//****************************************************************************

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "_commcomm.h"
#include "commssl.h"
#include "_commssl.h"

#define ROTL( x, y ) ( ( (x) << (y) ) | ( (x) >> (32-(y)) ) )

#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
void BIO_set_data(BIO *a, void *ptr)
{
    a->ptr = ptr;
}
void *BIO_get_data(BIO *a)
{
    return a->ptr;
}
void BIO_set_init(BIO *a, int init)
{
    a->init = init;
}
int BIO_get_init(BIO *a)
{
    return a->init;
}
void BIO_set_shutdown(BIO *a, int shut)
{
    a->shutdown = shut;
}
int BIO_get_shutdown(BIO *a)
{
    return a->shutdown;
}
#endif

//****************************************************************************

class _CommSSLBIOCommunicator
{
public:
	_CommVirtualCommunicator* comm;
	UINT32 checkSummRead;
	UINT32 checkSummWrite;
	bool   calcCheckSumm;
	const char* wrBuf;
	size_t wrSz;
	const char* rdBuf;
	size_t rdSz;

public:
	_CommSSLBIOCommunicator( _CommVirtualCommunicator* comm_ )
	{
		comm = comm_;
		wrBuf = 0;
		wrSz = 0;
		rdBuf = 0;
		rdSz = 0;
		calcCheckSumm = false;
		checkSummRead = 0;
		checkSummWrite = 0;
	}

public:
	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		return comm->writeIncomplete( p, sz );
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		size_t ret = comm->readIncomplete( p, sz );
		return ret;
	}
	_CommAsyncRet asyncWriteOk()
	{
		return comm->asyncWriteOk();
	}
	_CommAsyncRet asyncReadOk()
	{
		return comm->asyncReadOk();
	}
};

static int comm_write( BIO* h, const char* buf, int num );
static int comm_read( BIO* h, char* buf, int size );
static int comm_puts( BIO* h, const char* str );
static long comm_ctrl( BIO* h, int cmd, long arg1, void* arg2 );
static int comm_new( BIO* h );
static int comm_free( BIO* data );

#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
static BIO_METHOD methods_commp =
{
	99 | BIO_TYPE_SOURCE_SINK,//99 - why not?
	"CommLib",
	comm_write,
	comm_read,
	comm_puts,
	NULL, /* comm_gets, */
	comm_ctrl,
	comm_new,
	comm_free,
	NULL,
};

static BIO_METHOD* BIO_s_comm()
{
	return &methods_commp;
}
#else
static BIO_METHOD *methods_commp = 0;
PCriticalSection bio_methods_lock;
static BIO_METHOD* BIO_s_comm()
{
	PLock lock(bio_methods_lock);
	if (!methods_commp)
	{
		methods_commp = BIO_meth_new(99 | BIO_TYPE_SOURCE_SINK,"CommLib");
		BIO_meth_set_write(methods_commp,comm_write);
		BIO_meth_set_read(methods_commp,comm_read);
		BIO_meth_set_puts(methods_commp,comm_puts);
		BIO_meth_set_ctrl(methods_commp,comm_ctrl);
		BIO_meth_set_create(methods_commp,comm_new);
		BIO_meth_set_destroy(methods_commp,comm_free);
	}
	return methods_commp;
}

#endif
static BIO* BIO_new_comm( _CommSSLBIOCommunicator* comm )
{
	BIO *ret = BIO_new( BIO_s_comm() );
	if( ret == NULL )
		return NULL;
	//	PASSERT3( sizeof(int) >= sizeof(_CommSSLBIOCommunicator*) );
	BIO_set_fp( ret, comm, 0 );
	return ret;
}

static int comm_new( BIO *bi )
{
	BIO_set_init(bi,0);
//	bi->num = 0; 
	BIO_set_data(bi,0);
	BIO_clear_flags(bi,-1);
	return 1;
}

static int comm_free( BIO* b )
{
	if( b == NULL )
		return 0;
	if( BIO_get_shutdown(b))
	{
		BIO_set_init(b,0);
		BIO_clear_flags(b,-1);
	}
	return 1;
}

static int comm_read( BIO* b, char* out, int outl )
{
	if( out == NULL )
		return 0;
	BIO_clear_retry_flags( b );

	//PTRACE( "_DBG: SSL:comm_read( %d ) called", outl );
	_CommSSLBIOCommunicator* comm = (_CommSSLBIOCommunicator*)BIO_get_data(b);
	size_t rd;
	if( comm->rdBuf )
	{
		PASSERT3( comm->rdBuf == out && comm->rdSz == outl );
		try
		{
			//PTRACE( "_DBG: SSL:comm_read: asyncReadOk" );
			_CommAsyncRet ret = comm->asyncReadOk();
			PASSERT3( ret.written == 0 );//imposes restrictions on underlying communicators
			rd = ret.read;
		}
		catch( PError& err )
		{
			PTRACE3( "Exception in _CommSSLBIOCommunicator::asyncReadOk(): %s", err.why() );
			return -1;
		}
	}
	else
	{
		try
		{
			//PTRACE( "_DBG: SSL:comm_read: readIncomplete( %d )", outl );
			rd = comm->readIncomplete( (BYTE*)out, outl );
		}
		catch( PError& err )
		{
			PTRACE3( "Exception in _CommSSLBIOCommunicator::readIncomplete(): %s", err.why() );
			return -1;
		}
	}
	if( rd > 0 )
	{
		if (comm->calcCheckSumm)
		{
			for (int i=0; i<rd; i++)
			{
				comm->checkSummRead ^= out[i];
				comm->checkSummRead = ROTL(comm->checkSummRead,8);
			}
		}
		//PString n;
		//n="rd";
		//n.appendInt(PID());
		//FILE* f = fopen( n.c_str(), "ab" );
		//fwrite( out, 1, rd, f );
		//fclose( f );

		comm->rdBuf = 0;
		comm->rdSz = 0;
		return rd;
	}
	else
	{
		comm->rdBuf = out;
		comm->rdSz = outl;
		//PTRACE( "_DBG: SSL:comm_read: BIO_set_retry_read" );
		BIO_set_retry_read( b );
		return -1;
	}
}

static int comm_write( BIO* b, const char* in, int inl )
{
	if( in == NULL )
		return 0;
	BIO_clear_retry_flags( b );

	//PTRACE( "_DBG: SSL:comm_write( %d )  called", inl );
	_CommSSLBIOCommunicator* comm = (_CommSSLBIOCommunicator*)BIO_get_data(b);
	size_t wr;
	if( comm->wrBuf )
	{
		PASSERT3( comm->wrBuf == in && comm->wrSz == inl );
		try
		{
			//PTRACE( "_DBG: SSL:comm_write: asyncWriteOk" );
			_CommAsyncRet ret = comm->asyncWriteOk();
			PASSERT3( ret.read == 0 );//imposes restrictions on underlying communicators
			wr = ret.written;
		}
		catch( PError& err )
		{
			PTRACE3( "Exception in _CommSSLBIOCommunicator::asyncWriteOk(): %s", err.why() );
			return -1;
		}
	}
	else
	{
		try
		{
			//PTRACE( "_DBG: SSL:comm_write: writeIncomplete( %d )", inl );
			wr = comm->writeIncomplete( (const BYTE*)in, inl );
		}
		catch( PError& err )
		{
			PTRACE3( "Exception in _CommSSLBIOCommunicator::writeIncomplete(): %s", err.why() );
			return -1;
		}
	}
	if( wr > 0 )
	{
		if (comm->calcCheckSumm)
		{
			for (int i=0; i<wr; i++)
			{
				comm->checkSummWrite ^= in[i];
				comm->checkSummWrite = ROTL(comm->checkSummWrite,8);
			}
		}
		//PString n;
		//n="wr";
		//n.appendInt(PID());
		//FILE* f = fopen( n.c_str(), "ab" );
		//fwrite( in, 1, wr, f );
		//fclose( f );
		comm->wrBuf = 0;
		comm->wrSz = 0;
		return wr;
	}
	else
	{
		comm->wrBuf = in;
		comm->wrSz = inl;
		//PTRACE( "_DBG: SSL:comm_write: BIO_set_retry_write" );
		BIO_set_retry_write( b );
		return -1;
	}
}

static long comm_ctrl( BIO* b, int cmd, long num, void* ptr )
{
	long ret = 1;

	switch( cmd )
	{
	case BIO_CTRL_RESET:
	case BIO_C_FILE_SEEK:
	case BIO_C_FILE_TELL:
	case BIO_CTRL_INFO:
		ret = 0;
		break;

	case BIO_C_SET_FILE_PTR:
		PASSERT3( BIO_get_data(b) == 0 );
		BIO_set_data(b,ptr);
		PASSERT3( num == 0 );
		BIO_set_shutdown(b,(int)num);
		BIO_set_init(b, 1);
		break;
		/*
		case BIO_C_GET_FILE_PTR:
		if( b->init )
		{
		//				int* ip = (int*)ptr;
		//				if( ip != NULL )
		//					*ip = b->num;
		//				ret = b->num;
		return b->ptr;
		}
		else
		ret= -1;
		break;
		*/
	case BIO_CTRL_GET_CLOSE:
		ret = BIO_get_shutdown(b);
		break;

	case BIO_CTRL_SET_CLOSE:
		BIO_set_shutdown(b,(int)num);
		break;

	case BIO_CTRL_PENDING:
	case BIO_CTRL_WPENDING:
		ret = 0;
		break;

	case BIO_CTRL_DUP:
	case BIO_CTRL_FLUSH:
		ret = 1;
		break;

	default:
		ret = 0;
		break;
	}
	return ret;
}

static int comm_puts( BIO* bp, const char *str )
{
	return comm_write( bp, str, strlen( str ) );
}

//****************************************************************************

static inline SSL* _SSL( void* ssl )
{
	return (SSL*)ssl;
}

_CommSSLCommunicator::_CommSSLCommunicator()
{
	cli = 0;
	srv = 0;
	vComm = 0;
	comm = 0;
	ssl = 0;
	isServer = false;

	_init();
}

void _CommSSLCommunicator::construct(  CommSSLClient* cli_, CommSSLServer* srv_, _CommVirtualCommunicator* vComm_ )
{
	cli = cli_;
	srv = srv_;
	vComm = vComm_;
}

void _CommSSLCommunicator::_init()
{
	state = 0;

	initErr = 0;
	rdBuf = 0;
	rdSz = 0;
	rdErr = 0;
	wrBuf = 0;
	wrSz = 0;
	wrErr = 0;
}

void _CommSSLCommunicator::_del()
{
	if( ssl )
	{
		SSL_free( _SSL( ssl ) );
		ssl = 0;
	}
	delete comm;
	comm = 0;

	_init();
}

void _CommSSLCommunicator::init( bool inbound )
{
	_del();
	isServer = inbound;
	PASSERT3( isServer ? srv != 0 : cli != 0 );
	SSL_CTX* ctx = isServer ? (SSL_CTX*)srv->ctx : (SSL_CTX*)cli->ctx;
	PASSERT3( ctx );
	ssl = SSL_new( ctx );
	comm = new _CommSSLBIOCommunicator( vComm );
	BIO* sbio = BIO_new_comm( comm );
	SSL_set_bio( _SSL( ssl ), sbio, sbio );
	PASSERT3( ssl );
}

_CommSSLCommunicator::~_CommSSLCommunicator()
{
	_del();
}

bool _CommSSLCommunicator::_checkInitConn()
{
	if( state == 0 )
	{
		if( initErr != 0 )
			return false;
		return _initConn();
	}
	return true;
}

bool _CommSSLCommunicator::_initConn()
{
	PASSERT3( state == 0 );
	int er;
	//PTRACE( "SSL_accept/connect" );

	if( isServer )
		er = SSL_accept( _SSL( ssl ) );
	else
	{
		if (cli->sniHostName.length() > 0)
		{
			SSL_set_tlsext_host_name( _SSL( ssl ),cli->sniHostName.c_str());
		}
		er = SSL_connect( _SSL( ssl ) );
	}
	if( er <= 0 )
	{
		int err = SSL_get_error( _SSL( ssl ), er );
		if( err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE )
		{
			initErr = err;
			return false;
		}
		CommSSL::_throwErr();
	}
	comm->calcCheckSumm = false;
	if( !isServer )
		cli->_checkServerCert( ssl );

	initErr = 0;
	state = 1;
	return true;
}
bool _CommSSLCommunicator::initConnection(UINT32 & checkSumm)
{
	comm->calcCheckSumm = true;
	bool ret = _initConn();
	if (ret)
	{
		comm->calcCheckSumm = false;
		checkSumm = isServer ? comm->checkSummRead : comm->checkSummWrite;
	}
	return ret;
}
bool _CommSSLCommunicator::generateRandom(UINT32 * secureRandom, int size)
{
	CommSSL::generateRandomBytes((BYTE *) secureRandom,size * sizeof(UINT32));
	return true;
}



_CommAsyncRet _CommSSLCommunicator::_reinitConn()
{
	_CommAsyncRet ret;
	bool Ok = _initConn();
	if( !Ok )
		return ret;
	if( wrBuf )
	{
		const BYTE* p = wrBuf;
		size_t sz = wrSz;
		wrBuf = 0;
		wrSz = 0;
		ret.written = writeIncomplete( p, sz );
	}
	if( rdBuf )
	{
		BYTE* p = rdBuf;
		size_t sz = rdSz;
		rdBuf = 0;
		rdSz = 0;
		ret.read = readIncomplete( p, sz );
	}
	return ret;
}

size_t _CommSSLCommunicator::writeIncomplete( const BYTE* p, size_t sz )
{
	//PTRACE( "_DBG: SSLComm: writeIncomplete( %d )  called", sz );
	bool Ok = _checkInitConn();
	if( !Ok )
	{
		wrBuf = p;
		wrSz = sz;
		return 0;
	}

	PASSERT3( wrBuf == 0 && wrErr == 0 );
	//PTRACE( "_DBG: SSLComm: SSL_write( %d )", sz );
	int wr = SSL_write( _SSL( ssl ), (const char*)p, sz );
	//PTRACE( "SSL_write: %d->%d", sz, wr );
	if( wr <= 0 )
	{
		int err = SSL_get_error( _SSL( ssl ), wr );
		if( err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE )
		{
			wrErr = err;
			wrBuf = p;
			wrSz = sz;
			return 0;
		}
		CommSSL::_throwErr();
	}

	return wr; 
}

size_t _CommSSLCommunicator::readIncomplete( BYTE* p, size_t sz )
{
	//PTRACE( "_DBG: SSLComm: readIncomplete( %d ) called", sz );
	bool Ok = _checkInitConn();
	if( !Ok )
	{
		rdBuf = p;
		rdSz = sz;
		return 0;
	}

	PASSERT3( rdBuf == 0 && rdErr == 0 );
	//PTRACE( "_DBG: SSLComm: SSL_read( %d )", sz );
	int rd = SSL_read( _SSL( ssl ), (char*)p, sz );
	//PTRACE( "SSL_read: %d->%d", sz, rd );
	if( rd <= 0 )
	{
		int err = SSL_get_error( _SSL( ssl ), rd );
		if( err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE )
		{
			rdErr = err;
			rdBuf = p;
			rdSz = sz;
			return 0;
		}
		CommSSL::_throwErr();
	}

	return rd;
}

_CommAsyncRet _CommSSLCommunicator::asyncWriteOk()
{
	//PTRACE( "_DBG: SSLComm: asyncWriteOk() called" );

	PASSERT3( initErr == SSL_ERROR_WANT_WRITE || wrErr == SSL_ERROR_WANT_WRITE || rdErr == SSL_ERROR_WANT_WRITE );
	if( initErr )
	{
		PASSERT3( initErr == SSL_ERROR_WANT_WRITE );
		return _reinitConn();
	}

	_CommAsyncRet ret;
	if( wrErr == SSL_ERROR_WANT_WRITE )
	{
		const BYTE* p = wrBuf;
		size_t sz = wrSz;
		wrBuf = 0;
		wrSz = 0;
		wrErr = 0;
		//PTRACE( "_DBG: SSLComm: writeIncomplete( %d )", sz );
		ret.written = writeIncomplete( p, sz );
	}
	if( rdErr == SSL_ERROR_WANT_WRITE )
	{
		BYTE* p = rdBuf;
		size_t sz = rdSz;
		rdBuf = 0;
		rdSz = 0;
		rdErr = 0;
		//PTRACE( "_DBG: SSLComm: readIncomplete( %d )", sz );
		ret.read = readIncomplete( p, sz );
	}
	return ret;
}

_CommAsyncRet _CommSSLCommunicator::asyncReadOk()
{
	//PTRACE( "_DBG: SSLComm: asyncReadOk() called" );

	PASSERT3( initErr == SSL_ERROR_WANT_READ || wrErr == SSL_ERROR_WANT_READ || rdErr == SSL_ERROR_WANT_READ );
	_CommAsyncRet ret;
	if( initErr )
	{
		PASSERT3( initErr == SSL_ERROR_WANT_READ );
		return _reinitConn();
	}

	if( wrErr == SSL_ERROR_WANT_READ )
	{
		const BYTE* p = wrBuf;
		size_t sz = wrSz;
		wrBuf = 0;
		wrSz = 0;
		wrErr = 0;
		//PTRACE( "_DBG: SSLComm: writeIncomplete( %d )", sz );
		ret.written = writeIncomplete( p, sz );
	}
	if( rdErr == SSL_ERROR_WANT_READ )
	{
		BYTE* p = rdBuf;
		size_t sz = rdSz;
		rdBuf = 0;
		rdSz = 0;
		rdErr = 0;
		//PTRACE( "_DBG: SSLComm: readIncomplete( %d )", sz );
		ret.read = readIncomplete( p, sz );
	}
	return ret;
}
