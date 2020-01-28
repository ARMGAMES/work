#include "httpshelper.h"//MUST be included first to ensure proper include order

#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

//****************************************************************************

#include "pperror.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "_commcomm.h"
#include "commssl.h"
#include "_commssl.h"

//-V::126  (allow long type)

static const int msecTimeout=15*60*1000;
static int comm_write( BIO* h, const char* buf, int num );
static int comm_read( BIO* h, char* buf, int size );
static int comm_puts( BIO* h, const char* str );
static long comm_ctrl( BIO* h, int cmd, long arg1, void* arg2 );
static int comm_new( BIO* h );
static int comm_free( BIO* data );

// PYR-18708
#define READ_TIMEOUT -2

struct ExtraRequestData
	{
	int readTimeout;

	ExtraRequestData()
		{
		readTimeout = 0;
		}
	};

static int getReadTimeout(BIO* b)
{
	if ( b->ptr != NULL)
		{
		ExtraRequestData* erd = (ExtraRequestData*)b->ptr;
		if ( erd != NULL && erd->readTimeout != 0 ) //@@@ Alex N V560 A part of conditional expression is always true: erd != 0.
			{
			return erd->readTimeout;
			}
		}
	return msecTimeout;
}

static int setReadTimeout(BIO* b, int readTimeout_)
	{
	int oldReadTimeout = 0;

	if ( b->ptr != NULL)
		{
		ExtraRequestData* erd = (ExtraRequestData*)b->ptr;
		if ( erd != NULL ) //@@@ Alex N V547 Expression 'erd != 0' is always true
			{
			PTRACE5("SRTM: %d", readTimeout_);
			oldReadTimeout = erd->readTimeout;
			erd->readTimeout = readTimeout_;
			}
		}
	return oldReadTimeout;
	}

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


static BIO_METHOD * getBIO()
	{
	return &methods_commp;
	}

static int comm_new( BIO *bi )
	{
	bi->init = 0;
	bi->num = 0;

	ExtraRequestData* erd = new ExtraRequestData;
	if (erd == NULL) //-V668
		{
		BIOerr(BIO_F_SSL_NEW,ERR_R_MALLOC_FAILURE);
		return(0);
		}
	bi->ptr = erd;
	bi->flags = 0;
	return 1;
	}

static int comm_free( BIO* b )
	{
	if( b == NULL )
		return 0;
	if( b->shutdown )
		{
		b->init = 0;
		b->flags = 0;
		}
	if( b->ptr != NULL )
		{
		delete (ExtraRequestData*)(b->ptr);
		b->ptr = NULL;
		}
	return 1;
	}
	
static int comm_read( BIO* b, char* out, int outl )
	{
	if( out == NULL )
		return 0;
	BIO_clear_retry_flags( b );

	fd_set rdset;
	FD_ZERO( &rdset );
	FD_SET( b->num, &rdset );
	struct timeval timeout;
	
	int usingTimeout = getReadTimeout( b );

	timeout.tv_sec = usingTimeout / 1000;
	timeout.tv_usec = ( usingTimeout - timeout.tv_sec * 1000 ) * 1000;
	
	int sel = ::select( b->num+1, &rdset, 0, 0, &timeout );

	if( sel == 0)
		return READ_TIMEOUT;

	if( sel < 0 )
		return -1;

	return recv(b->num,out,outl,0);
	}

static int comm_write( BIO* b, const char* in, int inl )
	{
	if( in == NULL )
		return 0;

	BIO_clear_retry_flags( b );
	fd_set wrset;
	FD_ZERO( &wrset );
	FD_SET( b->num, &wrset );
	struct timeval timeout;
	timeout.tv_sec = msecTimeout / 1000;
	timeout.tv_usec = ( msecTimeout - timeout.tv_sec * 1000 ) * 1000;
	int sel = ::select( b->num+1, 0, &wrset, 0, &timeout );

	if( sel <= 0 )
		return -1;
	return send(b->num,in,inl,0);
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
			ret = 0;                        //-V1037
			break;

		case BIO_C_SET_FD:
//			PASSERT3( b->num == 0 );
			b->num = *((int*)ptr); //-V206
			PASSERT3( num == 0 );
			b->shutdown = (int)num;
			b->init = 1;
			break;

		case BIO_C_GET_FD:
			if( b->init )
				{
				int* ip = (int*)ptr; //-V206
				if( ip != NULL )
					*ip = b->num;
				ret = b->num;
				}
			else
				ret= -1;
			break;

		case BIO_CTRL_GET_CLOSE:
			ret = b->shutdown;
			break;

		case BIO_CTRL_SET_CLOSE:
			b->shutdown = (int)num;
			break;

		case BIO_CTRL_PENDING:
		case BIO_CTRL_WPENDING:
			ret = 0;
			break;

		case BIO_CTRL_DUP:
		case BIO_CTRL_FLUSH:
			ret = 1; //-V1048
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

static unsigned long _sslErrMsg( PString& ret )
	{
	const char *file,*data;
	int line,flags;
	unsigned long l = ERR_get_error_line_data( &file, &line, &data, &flags );
	if( l != 0 )
		{
		char buf[200];
		ERR_error_string_n( l, buf, sizeof( buf ) );
		buf[ sizeof(buf) - 1 ] = 0;
		ret = buf;
        // PYR-20705 - more error info
        if( file && *file )
            ret.append( ":" ).append( file ).append( ":" ).appendInt( line );
        if( (flags & ERR_TXT_STRING) && data && *data )
            ret.append( ":" ).append( data );
		}
	else
		ret = "";
    // PYR-20705
    return l;
	}

static PString sslGetError(SSL* ssl, int ret)
{
	PString err, sslErrStr;
	int sslErr = SSL_get_error(ssl, ret);
	sslErrStr.append("Ret:").appendInt(ret).append(" SSLErr:").appendInt(sslErr);

	if (sslErr == SSL_ERROR_SYSCALL && ret == -1) // see SSL_get_error man page
		sslErrStr.append(" errno:").appendInt(errno).append(" sockerr:").appendInt(SOCKETERRNO());
	while (_sslErrMsg(err))
		sslErrStr.append(" ").append(err);
	ERR_clear_error(); // clear the error queue

	return sslErrStr;
}

static void _throwSSLErr(  SSL* ssl, int ret )
{
	PString sslErrStr = sslGetError(ssl, ret);
	ERR_remove_thread_state(NULL); // PYR-58817
	SSL_shutdown(ssl);
	throw PInternalError(sslErrStr);
}

void _sslRequestResponse( SSL_CTX* ctx, const char* host, int port, const char* rqStr, PBlock& ret, const int readTimeout /* = 0 */, const int sentSize /* = 0 */, const char* sniServerName /* = "" */)
	{
	PLog( "Entering _sslRequestResponse..." );
	PIPSocket sock;
	bool Ok = sock.enableKeepAlive();
	if( !Ok )
		PLog( "_sslRequestResponse: unable to enable keepAlive" );

	SSL* ssl_ = 0;
    _SSLWrapper ssl; // #22476
	try
	{
		PString ipAddress;
		PIPSocket::hostByName(ipAddress,host);

		PLog( "Connecting to %s:%d", host, port );
		PLog("resolved ip address: %s",ipAddress.c_str());		

		sock.connect(ipAddress.c_str(),port);		
		//sock.connect( host, port );
		PLog( "Connected to %s:%d", ipAddress.c_str(), port );

		PIPSocket::Descriptor sockDescr;
		sock.copyDescriptor( sockDescr );
		SOCKET sd = sockDescr.sock;

		ssl_ = SSL_new( ctx );
		PASSERT( ssl_ );
		ssl.set( ssl_ );
		if (sniServerName && *sniServerName)
		{
			PTRACE5("SNI server name: '%s'", sniServerName);
			long res = SSL_set_tlsext_host_name(ssl, sniServerName);
			if (res != 1)
			{
				PLog("Failed to set SNI server name '%s' (%ld)", sniServerName, res);
			}
		}
		BIO *bio = BIO_new( getBIO() );

		BIO_set_fd( bio, (int)sd, 0 );
		SSL_set_bio(  ssl , bio, bio );
		//	SSL_set_fd( ssl, sd );

		PLog( "Establishing SSL connection..." );
		int err = SSL_connect( ssl );
		if( err <= 0 )
			_throwSSLErr( ssl, err );

		/* Get the cipher - opt */
		PLog( "SSL connection using %s", SSL_get_cipher(ssl) );

		/* Get server's certificate (note: beware of dynamic allocation) - opt */
		X509* server_cert = SSL_get_peer_certificate( ssl );
  		char* str = X509_NAME_oneline( X509_get_subject_name( server_cert ), 0, 0 );
		PASSERT( str );
		PLog("Server cert subject: %s", str);
		OPENSSL_free( str );

		str = X509_NAME_oneline( X509_get_issuer_name( server_cert ), 0, 0 );
		PASSERT( str );
		PLog( "Server cert issuer: %s", str );
		OPENSSL_free( str );

		/* We could do all sorts of certificate verification stuff here before
		 deallocating the certificate. */

		X509_free( server_cert );
	}
	catch( PHostNameError& x)
	{
		throw x;
	}
	catch( PError& x )
	{
		throw _SSLRequestResponseNotConnectedError( x.why() );
	}

	int wrPos = 0;
	int wrSz;
	if(sentSize)
		wrSz = sentSize; //for sending binary buffer
	else
	    wrSz = strlen( rqStr );

//PLog( "Sending %d chars:'%s'\n", wrSz, rqStr );
PLog( "Sending %d chars of data...", wrSz );
	for(; wrPos < wrSz ;)
		{
		int wr = SSL_write( ssl, rqStr + wrPos, wrSz - wrPos );
		if( wr <= 0 )
			_throwSSLErr( ssl, wr );
		wrPos += wr;
		}

PLog( "Receiving data..." );

	int oldReadTimeout = 0;
	if( readTimeout != 0 )
		{
		oldReadTimeout = setReadTimeout( ssl->rbio, readTimeout);
		}

	for(;;)
		{
		char buf[ 1024 ];
		
		const int rd = SSL_read( ssl, buf, sizeof(buf) - 1 );
		
		if( rd < 0 )
        {
            //long err = ERR_get_error();
            const int err = SSL_get_error(ssl, rd);
			if( err == SSL_ERROR_SYSCALL && rd == READ_TIMEOUT)
				{
				throw _SSLResponseReadTimeoutError( "Response read timeout" );
				}
            if( err != SSL_ERROR_WANT_READ)
				{
			    _throwSSLErr( ssl, rd );
				}
        }        
		else if( rd == 0 )
			break;
        else
		    memcpy( ret.append( rd ), buf, rd );
		}

	if( readTimeout != 0 )
		{
		setReadTimeout( ssl->rbio, oldReadTimeout );
		}

PLog( "Closing SSL/socket..." );
	ERR_remove_thread_state(NULL); // PYR-58817
	SSL_shutdown( ssl );  /* send SSL/TLS close_notify */
	sock.close();
	//SSL_free( ssl ); //commented in the scope of #22476
PLog( "Exiting _sslRequestResponse.." );
	}


void _sslWriteBlocks( SSL_CTX* ctx, const char* host, int port, const vector<char*>& sentMessages, const vector<int>& sentSizes, int& sentIndex, PBlock& ret, const char* sniServerName /* = "" */)
{
PLog( "Entering _sslRequestResponse..." );
	PIPSocket sock;
	bool Ok = sock.enableKeepAlive();
	if( !Ok )
		PLog( "_sslRequestResponse: unable to enable keepAlive" );

	SSL* ssl_ = 0;
    _SSLWrapper ssl; // #22476
	try
	{
		PString ipAddress;
		PIPSocket::hostByName(ipAddress,host);

		PLog( "Connecting to %s:%d", host, port );
		PLog("resolved ip address: %s",ipAddress.c_str());		

		sock.connect(ipAddress.c_str(),port);		
		//sock.connect( host, port );
		PLog( "Connected to %s:%d", ipAddress.c_str(), port );

	PIPSocket::Descriptor sockDescr;
	sock.copyDescriptor( sockDescr );
	SOCKET sd = sockDescr.sock;

	ssl_ = SSL_new( ctx );
	PASSERT( ssl_ );
    ssl.set( ssl_ );
	if (sniServerName && *sniServerName)
	{
		PTRACE5("SNI server name: '%s'", sniServerName);
		long res = SSL_set_tlsext_host_name(ssl, sniServerName);
		if (res != 1)
		{
			PLog("Failed to set SNI server name '%s' (%ld)", sniServerName, res);
		}
	}
	BIO *bio = BIO_new( getBIO() );

	BIO_set_fd( bio, (int)sd, 0 );
	SSL_set_bio(  ssl , bio, bio );
//	SSL_set_fd( ssl, sd );

PLog( "Establishing SSL connection..." );
	int err = SSL_connect( ssl );
	if( err <= 0 )
		_throwSSLErr( ssl, err );

	/* Get the cipher - opt */
	PLog( "SSL connection using %s", SSL_get_cipher(ssl) );

	/* Get server's certificate (note: beware of dynamic allocation) - opt */
	X509* server_cert = SSL_get_peer_certificate( ssl );
  	char* str = X509_NAME_oneline( X509_get_subject_name( server_cert ), 0, 0 );
	PASSERT( str );
	PLog("Server cert subject: %s", str);
	OPENSSL_free( str );

	str = X509_NAME_oneline( X509_get_issuer_name( server_cert ), 0, 0 );
	PASSERT( str );
	PLog( "Server cert issuer: %s", str );
	OPENSSL_free( str );

	/* We could do all sorts of certificate verification stuff here before
	 deallocating the certificate. */

	X509_free( server_cert );
	}
	catch( PHostNameError& x)
	{
		throw x;
	}
	catch( PError& x )
	{
		throw _SSLRequestResponseNotConnectedError( x.why() );
	}

	int sentTotal=sentMessages.size();
	for(; sentIndex<sentTotal; ++sentIndex)
	{
		int wrPos = 0;
		int wrSz= sentSizes[sentIndex]; //for sending binary buffer
		const char* rqStr = sentMessages[sentIndex];
		PLog( "Sending %d chars of data, index %d...", wrSz, sentIndex );
		bool writeErr = false;
		for(; wrPos < wrSz ;)
		{
			int wr = SSL_write( ssl, rqStr + wrPos, wrSz - wrPos );
			if( wr <= 0 )
				//_throwSSLErr( ssl, wr );
			{
				//do not throw so the following SSL read will be called
				PString errStr = sslGetError(ssl, wr);

				PLog("Error on send message index %d, %s", sentIndex, errStr.c_str());

				writeErr = true;
				break;
			}
			wrPos += wr;
		}

		if(writeErr)
			break;

	}


	int oldReadTimeout = 0;
	oldReadTimeout = setReadTimeout( ssl->rbio, 3000);
		
	for(;;)
		{
		char buf[ 1024 ];
		
		const int rd = SSL_read( ssl, buf, sizeof(buf) - 1 );
		
		if( rd <= 0 )
        {
            //long err = ERR_get_error();
            const int err = SSL_get_error(ssl, rd);
			if( err == SSL_ERROR_SYSCALL && rd == READ_TIMEOUT)
			{
				PLog( "Response read timeout" );
				break;
			}
			else if(err == SSL_ERROR_ZERO_RETURN)
			{
				PLog("Server Connection closed");
				break;
			}			
            else if( err != SSL_ERROR_WANT_READ)
			{
			    _throwSSLErr( ssl, rd );
			}
        }        
        else
		    memcpy( ret.append( rd ), buf, rd );
		}

	setReadTimeout( ssl->rbio, oldReadTimeout );		

	PLog( "Closing SSL/socket..." );
	ERR_remove_thread_state(NULL); // PYR-58817
	SSL_shutdown( ssl );  /* send SSL/TLS close_notify */
	sock.close();
	//SSL_free( ssl ); //commented in the scope of #22476
	PLog( "Exiting _sslRequestResponse.." );
}

bool _sslConnect( SSL_CTX* ctx, const char* host, int port, _SSLWrapper &ssl )
{	
	PIPSocket* sock = new PIPSocket(); 
	ssl.setSocket(sock);

	bool Ok = sock->enableKeepAlive();
	if( !Ok )
		PLog( "_sslRequestResponse: unable to enable keepAlive" );

	SSL* ssl_ = 0;    
	try
	{
		PString ipAddress;
		PIPSocket::hostByName(ipAddress,host);

		PLog( "Connecting to %s:%d", host, port );
		PLog("resolved ip address: %s",ipAddress.c_str());		

		sock->connect(ipAddress.c_str(),port);		
		//sock.connect( host, port );
		PLog( "Connected to %s:%d", ipAddress.c_str(), port );

		PIPSocket::Descriptor sockDescr;
		sock->copyDescriptor( sockDescr );
		SOCKET sd = sockDescr.sock;

		ssl_ = SSL_new( ctx );
		PASSERT( ssl_ );
		ssl.set( ssl_ );
		BIO *bio = BIO_new( getBIO() );

		BIO_set_fd( bio, (int)sd, 0 );
		SSL_set_bio(  ssl , bio, bio );
		//	SSL_set_fd( ssl, sd );

		PLog( "Establishing SSL connection..." );
		int err = SSL_connect( ssl );
		if( err <= 0 )
			_throwSSLErr( ssl, err );

		/* Get the cipher - opt */
		PLog( "SSL connection using %s", SSL_get_cipher(ssl) );

		/* Get server's certificate (note: beware of dynamic allocation) - opt */
		X509* server_cert = SSL_get_peer_certificate( ssl );
  		char* str = X509_NAME_oneline( X509_get_subject_name( server_cert ), 0, 0 );
		PASSERT( str );
		PLog("Server cert subject: %s", str);
		OPENSSL_free( str );

		str = X509_NAME_oneline( X509_get_issuer_name( server_cert ), 0, 0 );
		PASSERT( str );
		PLog( "Server cert issuer: %s", str );
		OPENSSL_free( str );

		/* We could do all sorts of certificate verification stuff here before
		 deallocating the certificate. */

		X509_free( server_cert );

		return true;
	}
	catch( PError& x )
	{
		PLog( "Error in _sslConnect: '%s'", x.why() );	
		return false;
	}
}

void _sslWrite( _SSLWrapper &ssl,  const char* rqStr,  int sentSize )
{
	int wrPos = 0;
	int wrSz;
	if(sentSize)
		wrSz = sentSize; //for sending binary buffer
	else
	    wrSz = strlen( rqStr );

	PLog( "Sending %d chars of data...", wrSz );
	for(; wrPos < wrSz ;)
	{
		int wr = SSL_write( ssl, rqStr + wrPos, wrSz - wrPos );
		PLog("SSL_Write return %d", wr);
		if( wr <= 0 )
		{		
			_throwSSLErr( ssl, wr );
		}
		wrPos += wr;
	}
}

void _sslRead( _SSLWrapper &ssl,  PBlock& ret )
{
	fd_set rfd;
	FD_ZERO(&rfd);
	PIPSocket::Descriptor sockDescr;
	ssl.getSocket()->copyDescriptor( sockDescr );
	SOCKET sd = sockDescr.sock;
    FD_SET(sd, &rfd);
    
    int rdNum = select(sd+1, &rfd, 0, 0, NULL);
	if(rdNum)
	{
   
		char buf[ 1024 ];
		
		const int rd = SSL_read( ssl, buf, sizeof(buf) - 1 );
		
		if( rd <= 0 )
		{
			//long err = ERR_get_error();
			const int err = SSL_get_error(ssl, rd);
			if( err == SSL_ERROR_SYSCALL && rd == READ_TIMEOUT)
			{
				PLog( "_sslRead:timeout" );
			}
			else if(err == SSL_ERROR_ZERO_RETURN)
			{
				PLog("_sslRead:Server Connection closed");			
			}			
			else
			{
				PLog("ssl read errno %d", err);
			}

			_throwSSLErr( ssl, rd );
		}        
		else
		{
			memcpy( ret.append( rd ), buf, rd );
		}
	}
	else
	{
		PLog("socket select return value %d", rdNum);
	}
	
}

PString sslDumpError()
{
	PString buf, sslErr;
	while (_sslErrMsg(buf))
		sslErr.append(" ").append(buf);
	ERR_clear_error();
	ERR_remove_thread_state(NULL);
	return sslErr;
}

void sslLogErrIfAny(bool bOK)
{
	if (bOK)
		return;

	PString errStr = sslDumpError();
	PLog("%s", errStr.c_str());
}

#if OPENSSL_VERSION_NUMBER >= 0x1000105fL
const SSL_METHOD *sslv23Method(void)
{
	const SSL_METHOD* method = SSLv23_client_method();
	sslLogErrIfAny(method);
	return method;
}

const SSL_METHOD *sslv3Method(void)
{
	const SSL_METHOD* method = SSLv3_client_method();
	sslLogErrIfAny(method);
	return method;
}
#elif
SSL_METHOD *sslv23Method(void)
{
	SSL_METHOD* method = SSLv23_client_method();

	sslLogErrIfAny(method);
	return method;
}

SSL_METHOD *sslv3Method(void)
{
	SSL_METHOD* method = SSLv3_client_method();
	sslLogErrIfAny(method);
	return method;
}
#endif

SSL_CTX *sslCtxNew(const SSL_METHOD *meth)
{
	SSL_CTX* ctx = SSL_CTX_new(meth);
	sslLogErrIfAny(ctx);
	return ctx;
}
int sslCheckCtxPrivateKey(const SSL_CTX *ctx)
{
	int errCode = SSL_CTX_check_private_key(ctx);
	sslLogErrIfAny((errCode == 1));
	return errCode;
}

int sslUseCtxPrivateKeyFile(SSL_CTX *ctx, const char *file, int type)
{
	int errCode = SSL_CTX_use_PrivateKey_file(ctx, file, type);
	sslLogErrIfAny((errCode == 1));
	return errCode;
}

int sslUseCtxCertificateFile(SSL_CTX *ctx, const char *file, int type)
{
	int errCode = SSL_CTX_use_certificate_file(ctx, file, type);
	sslLogErrIfAny((errCode == 1));
	return errCode;
}
