#ifndef __httpshelper_h_included
#define __httpshelper_h_included

#include "ppincludebase.h"
#include "openssl/err.h"
#include "openssl/ssl.h"
#include "ppinclude.h"
#include "commlib.h"

#define SSLASSERT(cond) (void)( (cond) || (throw PAssert(sslDumpError(), __FILE__, __LINE__), 0) )

class _SSLRequestResponseNotConnectedError : public PError
	{
	public:
	_SSLRequestResponseNotConnectedError( const char* msg_ )
	: PError( msg_ )
		{
		}
	};

// PYR-18708
class _SSLResponseReadTimeoutError : public PError
{
public:
	_SSLResponseReadTimeoutError( const char* msg_ )
		: PError( msg_ )
	{
	}
};

// PYR-22476
class _SSLWrapper
{
private:
    SSL* ssl;
	PIPSocket* socket;

public:
    _SSLWrapper() : ssl( NULL ), socket(NULL) {}
    ~_SSLWrapper()
    {
        if( ssl )
        {
            SSL_free( ssl );
        }	

		if(socket)
		{
			socket->close();
			delete socket;
		}
    }
    void set( SSL* ssl_ ) { ssl = ssl_; }
	void setSocket(PIPSocket* sock_) { socket = sock_; }
	PIPSocket* getSocket()  { return socket; }
    operator SSL*() { return ssl; }
    SSL* operator->() { return ssl; }
};

void _sslRequestResponse( SSL_CTX* ctx, const char* host, int port, const char* rqStr, PBlock& ret, const int readTimeout = 0, const int sentSize = 0, const char* sniServerName = "" );
void _sslWriteBlocks( SSL_CTX* ctx, const char* host, int port, const vector<char*>& sentMessages, const vector<int>& sentSizes, int& sentIndex, PBlock& ret, const char* sniServerName = "" );
bool _sslConnect( SSL_CTX* ctx, const char* host, int port, _SSLWrapper& sslWrapper );
void _sslWrite( _SSLWrapper &ssl,  const char* rqStr, int sentSize=0 );
void _sslRead( _SSLWrapper &ssl,  PBlock& ret);

PString sslDumpError();

#if OPENSSL_VERSION_NUMBER >= 0x1000105fL
const
#endif
SSL_METHOD* sslv23Method(void);

#if OPENSSL_VERSION_NUMBER >= 0x1000105fL
const
#endif
SSL_METHOD* sslv3Method(void);

SSL_CTX* sslCtxNew(const SSL_METHOD *meth);
int sslCheckCtxPrivateKey(const SSL_CTX *ctx);
int sslUseCtxPrivateKeyFile(SSL_CTX *ctx, const char *file, int type);
int sslUseCtxCertificateFile(SSL_CTX *ctx, const char *file, int type);

#endif
