#ifndef commssl_h
#define commssl_h

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppsystem.h"//PSystemInfo
#include "commmsgbody.h"//PSystemInfo


//Most of functions are mere wrappers around OpenSSL functionality

class CommSSL
{
public:
	static void startup();//thread-safe; allows multiple calls
	static void cleanup();//thread-safe; allows multiple calls
	static void threadCleanup();

public:
	static void recryptPrivateKey( const char* inFileName, const char* pwd, const char* outFileName, const char* newPwd );
	static void generateRandomBytes( BYTE* buf, size_t sz );

public:
	static void _throwErr();
	static unsigned long _errMsg( PString& ret );
	static void _startup();
};

class CommSSLClient
{
private:
	friend class _CommSSLCommunicator;
	void* ctx;//SSL_CTX
	void* xStore;//X509_STORE
	vector< PString > commonNames;
	PString sniHostName;
	void (*_checkServerCertCallback)(void * ssl);

public:
	CommSSLClient();
	static void encryptCert(const char *clearText, UINT32 keyUser, PString & encryptedText);
	static void encryptCN(const char *clearText, UINT32 keyUser, PString & encryptedText);
	void addTrustedCertificate( const BYTE* b, size_t sz );//PEM format
	void addTrustedCertificateEncrypted( const BYTE* b, UINT32 key  );
	void requireServerCertificateCN( const char* cn_ );//if called more than once, means 'or'
	void requireServerCertificateCNEncrypted( const char* cn_, UINT32 key  );
	void setSniHostName(const char *hostName)
	{
		sniHostName = hostName;
	}
	void setCheckServerCertCallback(void(*fun)(void * ssl))
	{
		_checkServerCertCallback = fun;
	}
	~CommSSLClient();

private:
	void _checkServerCertDefault( void* /* SSL* */ ssl );
	void _checkServerCert(void* ssl)
	{
		if (_checkServerCertCallback)
			(*_checkServerCertCallback)(ssl);
		else
			_checkServerCertDefault(ssl);
	}
};

class CommSSLServer
{
private:
	friend class _CommSSLCommunicator;
	void* ctx;//SSL_CTX
	PString passwd;
	PStringMap<void *> sniCtx;
	void * _createCtx();
	void _setKeyCert( void* ctx_, const char* keyFile, const char* certFile, const char* password);
	void _setKeyCertFromMemory( void* ctx_, const BYTE * keyFileContent, size_t keyFileContentSize,  
										 const BYTE * certFileContent, size_t certFileContentSize,
										 const char* password);
public:
	CommSSLServer();
	void setKeyCert( const char* keyFile, const char* certFile, const char* password, const char *sniName = 0 );
	void setKeyCertFromMemory( const BYTE * keyFileContent, size_t keyFileContentSize,  
		const BYTE * certFileContent, size_t certFileContentSize,
		const char* password, const char *sniName = 0  );
	int checkSni(void *ssl);
	~CommSSLServer();
};

//************** Crypto/encoding/decoding ************************************

class CommSSLBase64
{
public:
	static void encode( PString& ret, const BYTE* p, size_t sz, int lineLen = 0, bool urlSafe = false );
	static void encode( char * ret, size_t retSize, const BYTE* p, size_t sz, bool urlSafe = false );
	static void decode( PBlock& ret, const char* s, size_t sz = (size_t)(-1)  );

	static void encodeOverwritingIntoBlock( PBlock& ret, BYTE const* p, size_t sz );
	static void decodeOverwriting( PBlock& ret, char const* s, size_t sz );

	static void encodeOverwritingIntoBlock( PBlock& ret, PBlock const& b )
	{
		encodeOverwritingIntoBlock(ret, b.ptr(), b.size());
	}
	static void encodeOverwritingIntoBlock( PBlock& ret, PString const& s )
	{
		encodeOverwritingIntoBlock(ret, reinterpret_cast<BYTE const*>(s.c_str()), s.length());
	}

	static void decodeOverwriting( PBlock& ret, PString const& s )
	{
		decodeOverwriting(ret, s.c_str(), s.length());
	}
	static void decodeOverwriting( PBlock& ret, PBlock const& b )
	{
		decodeOverwriting(ret, reinterpret_cast<char const*>(b.ptr()), b.size());
	}
};

class CommSSLChksum//SHA1
{
private:
	void* impl;//pimpl

public:
	CommSSLChksum();
	~CommSSLChksum();

	void addData( const BYTE* data, size_t sz );
	void chksum( PBlock& ret );
};

class CommSSLChksumSHA256//SHA256
{
private:
	void* impl;//pimpl

public:
	CommSSLChksumSHA256();
	~CommSSLChksumSHA256();

	void addData( const BYTE* data, size_t sz );
	void chksum( PBlock& ret );
};

class CommSSLChksumSHA512//SHA512
{
private:
	void* impl;//pimpl

public:
	CommSSLChksumSHA512();
	~CommSSLChksumSHA512();

	void addData( const BYTE* data, size_t sz );
	void chksum( PBlock& ret );
};
class CommSSLChksumMD5//MD5
{
private:
	void* impl;//pimpl

public:
	CommSSLChksumMD5();
	~CommSSLChksumMD5();

	void addData( const BYTE* data, size_t sz );
	void chksum( PBlock& ret );
};

class _CommSSLSignBase//RSA
{
private:
	void* impl;//pimpl

public:
	_CommSSLSignBase();
	void setPrivateKey( const char* fileName, const char* passwd );
	void setPrivateKey( const BYTE* p, size_t sz );//DER format
	~_CommSSLSignBase();

	void addData( const BYTE* data, size_t sz );
	void sign( PBlock& ret );

public:
	static void publicKeyFromCert( PBlock& ret, const char* fileName );//returns pubKey in DER format
	friend class CommSSLSign;
	friend class CommSSLSign256;
};

class CommSSLSign : public _CommSSLSignBase//RSA
{
public:
	CommSSLSign();
};

class CommSSLSign256 : public _CommSSLSignBase//RSA
{
public:
	CommSSLSign256();
};


class _CommSSLVerifyBase//RSA
{
private:
	void* impl;//pimpl

public:
	_CommSSLVerifyBase();
	void setPublicKey( const BYTE* p, size_t sz );//DER format
	void setPublicKey( const char* fileName, const char* passwd );
	~_CommSSLVerifyBase();

	void addData( const BYTE* data, size_t sz );
	void verifyData( const BYTE* sign, size_t signSz );
	friend class CommSSLVerify;
	friend class CommSSLVerify256;
};

class CommSSLVerify : public _CommSSLVerifyBase//RSA
{
public:
	CommSSLVerify();
};

class CommSSLVerify256 : public _CommSSLVerifyBase//RSA
{
public:
	CommSSLVerify256();
};


class CommSSLRSA//RSA
{
protected:
	void* impl;//pimpl

public:
	CommSSLRSA( );
	void setPublicKey( const BYTE* b, size_t sz );//DER format
	void setPrivateKey( const BYTE* p, size_t sz );
	void setPrivateKey( const char* fileName, const char* passwd );
	~CommSSLRSA();
};

class CommSSLDecryptRSA : public CommSSLRSA
{
public:
	void decrypt( PBlock& ret,const BYTE* data, size_t sz );
};
class CommSSLEncryptRSA : public CommSSLRSA
{
public:
	void encrypt( PBlock& ret,const BYTE* data, size_t sz );
};

class CommSSLEncrypt//DES-CBC
{
private:
	void* impl;//pimpl

public:
	CommSSLEncrypt( const char* pwd );
	CommSSLEncrypt( const BYTE* key, const BYTE * iv );
	~CommSSLEncrypt();

	void addData( const BYTE* data, size_t sz );
	void encrypt( PBlock& ret );
};

class CommSSLDecrypt//DES-CBC
{
private:
	void* impl;//pimpl

public:
	CommSSLDecrypt( const char* pwd );
	CommSSLDecrypt( const BYTE* key, const BYTE * iv );
	~CommSSLDecrypt();

	void addData( const BYTE* data, size_t sz );
	void decrypt( PBlock& ret );
};

class CommSSLEncrypt3//3DES-CBC
{
private:
	void* impl;//pimpl

public:
	CommSSLEncrypt3( const char* pwd );
	CommSSLEncrypt3( const BYTE* key, const BYTE * iv);
	~CommSSLEncrypt3();

	void addData( const BYTE* data, size_t sz );
	void encrypt( PBlock& ret );
};

class CommSSLDecrypt3//3DES-CBC
{
private:
	void* impl;//pimpl

public:
	CommSSLDecrypt3( const char* pwd );
	CommSSLDecrypt3( const BYTE* key, const BYTE * iv);
	~CommSSLDecrypt3();

	void addData( const BYTE* data, size_t sz );
	void decrypt( PBlock& ret );
};


class CommSSLStreamEncrypt//DES-CBC
{
private:
	void* impl;//pimpl
	void _init(const BYTE* pwd, int pwdLength, const unsigned char * salt);
public:
	CommSSLStreamEncrypt( const char* pwd, const unsigned char * salt );
	CommSSLStreamEncrypt( const BYTE* pwd, int pwdLength, const unsigned char * salt );
	~CommSSLStreamEncrypt();

	void encrypt( const BYTE* data, size_t sz, PBlock &ret );
	void encrypt( const BYTE* data, size_t sz, BYTE * ret );
};

class CommSSLStreamDecrypt//DES-CBC
{
private:
	void* impl;//pimpl
	void _init(const BYTE* pwd, int pwdLength, const unsigned char * salt);
public:
	CommSSLStreamDecrypt( const char* pwd, const unsigned char * salt );
	CommSSLStreamDecrypt( const BYTE* pwd, int pwdLength, const unsigned char * salt );
	~CommSSLStreamDecrypt();

	void decrypt( const BYTE* data, size_t sz, PBlock &ret );
	void decrypt( const BYTE* data, size_t sz,  BYTE * ret  );

};


class CommSSLEncryptAES
{
private:
	void* impl;//pimpl
	bool block256;

public:
	CommSSLEncryptAES( const BYTE* pwd, int length, bool block256_ = false, const unsigned char * salt = 0 );
	CommSSLEncryptAES( const BYTE* key, const BYTE* iv, bool block256_ = false );
	~CommSSLEncryptAES();

	void addData( const BYTE* data, size_t sz );
	void encrypt( PBlock& ret );
};

class CommSSLDecryptAES //AES256-CBC
{
private:
	void* impl;//pimpl
	bool block256;

public:
	CommSSLDecryptAES( const BYTE* pwd, int length, bool block256_ = false, const unsigned char * salt = 0 );
	CommSSLDecryptAES( const BYTE* key, const BYTE* iv, bool block256_ = false );
	~CommSSLDecryptAES();

	void addData( const BYTE* data, size_t sz );
	void decrypt( PBlock& ret );
};


class CommSSLHMAC_SHA256//SHA256
{
private:
	void* impl;//pimpl

public:
	CommSSLHMAC_SHA256(const BYTE * pwd,int length);
	~CommSSLHMAC_SHA256();

	void addData( const BYTE* data, size_t sz );
	void calculate( PBlock& ret );
};



class CommSSLDH
{
private:
	void * dh;
public:

	CommSSLDH();
	~CommSSLDH();
	void duplicate(CommSSLDH * other);
	void generateParameters(int prime_len);
	void writeParameters(const char * outFileName);
	void readParameters(const char * inFileName);
	void composeParameters(CommMsgBody & body);
	void parseParameters(CommMsgParser & parser);
	void generateKeys(PBlock & pubKey);
	void generateSecret(const BYTE * pubKey, int len ,PBlock & secret);
	void getPrime(PString &p);
	void setPrime(const BYTE *p, int len, int gen);
};

class CommSSLECDH
{
private:
	void * ecdh;
public:

	CommSSLECDH();
	~CommSSLECDH();
//	void composePublicKey(CommMsgBody & body);
	void generateKeys(PBlock & publicKey);
	void generateSecret(const BYTE * pubKey, int len, PBlock & secret);
};



//*************** Entropy ****************************************************

class CommSSLEntropyCollector
	/* CommSSLEntropyCollector collects entropy from various sources
	* with uneven entropy distribution (e.g. time when message arrived
	* has very little entropy, but entropy got from user - more entropy).
	* Collector makes sure that entropy it generates on output is good
	* enough regardless of concentration of entropy in input. 
	* Collector uses SHA-1 to mix entropy. It provides buffer 
	* of maxBlock size for storing collected entropy.
	*/
{
private:
	int maxBlock;
	BYTE buf0[ 64 ];//64 - 'native' SHA-1 source buf
	size_t buf0Fill;
	double buf0Entropy;//bits NOT bytes
	BYTE buf;
	int bufUsed;//bits
	PBlock out;

public:
	CommSSLEntropyCollector( int maxBlock_ );

public:
	void addEntropy( const BYTE* p, size_t sz, double entropyBits );

	void extractEntropy( PBlock& ret );
	size_t entropyBytesAvailable() const { return out.size(); }
	void extractEntropy( BYTE* p, size_t entropyBytes );
};

#ifdef PWIN32
class CommWin32SSLEntropyCollector : public CommSSLEntropyCollector
	/* CommWin32SSLEntropyCollector is a wrapper to use on client Win32 machines
	* Allows to use entropy from client mouse moves/timer events
	*/
{
private:
	int lastx, lasty, lastdx, lastdy;

public:
	CommWin32SSLEntropyCollector( int maxBlock_ );
	void addMouseMove( int x, int y );
	void addTimer();
};
#elif defined(P_MAC10X)
class CommMac10XSSLEntropyCollector : public CommSSLEntropyCollector
	/* CommMac10XSSLEntropyCollector is a wrapper to use on client Mac OS X machines
	* Allows to use entropy from client mouse moves/timer events
	*/
{
private:
	int lastx, lasty, lastdx, lastdy;

public:
	CommMac10XSSLEntropyCollector( int maxBlock_ );
	void addMouseMove( int x, int y );
	void addTimer();
};
#endif

class CommSSLEntropyFilter
	/* CommSSLEntropyFilter is a SHA-1 based filter to prevent attacks on entropy
	* generator. Similar to the one of Intel RNG (described in section 4.5 of 
	* "THE INTEL RANDOM NUMBER GENERATOR" by Cryptography Research 
	* (http://www.cryptography.com/resources/whitepapers/IntelRNG.pdf). 
	* Generates 1 bit of output per 1 bit of input. Initial 64-byte state needs 
	* to be initialized (preferably from independent entropy source).
	*/
{
private:
	BYTE buf[ 64 ];//'native' source size for SHA-1

public:
	CommSSLEntropyFilter();
	CommSSLEntropyFilter( BYTE init[64] );
	UINT32 next( UINT32 in );
};

//****************************************************************************

class CommSSLSystemInfo : public PSystemInfo
{
public:
	void generateInstallId( UINT32& a, UINT32& b ) override;
	void generateMD5( const PString& s, PString& md5hash ) override;

	virtual void generateMD5( const BYTE* ptr, size_t len, PString& md5hash );	// PYR-37796

	static void encryptPassword( PBlock& ret, const vector< PNameValue >& sysInfo, const char* password );
	static void decryptPassword( PString& ret, const vector< PNameValue >& sysInfo, const BYTE* p, size_t sz );
	static void encryptPasswordToString( PString& ret, const vector< PNameValue >& sysInfo, const char* password );
	static void decryptPasswordFromString( PString& ret, const vector< PNameValue >& sysInfo, const char* s );

	static void decodeIniFilePassword( PString& ret, const char* sectionName, const char* itemName, const char* itemValue );
	//if password is encrypted, decrypts; if it is plain-text, logs warning and encrypted password
	static void encodePasswordEx( PString& ret, int encodingId, const char* passwrd);
	static void decodePasswordEx( PString& ret, const char* passwrd);
private:
	static void _makeSysPwd( PString& ret, const vector< PNameValue >& sysInfo_ );
};

bool CommSSLGetSystemInfo( vector< PNameValue >& ret );////NOTE: may generate and set install id, returns true if new install id was generated

class CommSSLPasswordHash
{
private:
	static void _generate(PBlock & ret, const char * input);
public:
	static void generateHash(PString & ret, const char * input);
	static bool checkHash(const char * hash, const char * input);
};

#endif
