#ifndef commpkscs7_h
#define commpkscs7_h
#include "ppincludebase.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pkcs7.h>
#include <openssl/asn1_mac.h>
#include <openssl/x509.h>


class CommPKCS7_Sign
{
private:
	X509 *x509;
	EVP_PKEY *pkey;
public:
	CommPKCS7_Sign()
	{
		x509=0;
		pkey=0;
	}

	~CommPKCS7_Sign()
	{
		if (pkey)
			EVP_PKEY_free(pkey);
		if (x509)
			X509_free(x509);
	}
	void init(const char * x509file, const char * pkeyFile);
	void sign(const BYTE * dataToSign, size_t sizeData, PBlock & ret);
};

class CommPKCS7_Verify
{
private:
	X509 *x509;
public:
	CommPKCS7_Verify()
	{
		x509=0;
	}

	~CommPKCS7_Verify()
	{
		if (x509)
			X509_free(x509);
	}
	void init(const char * x509file);
	bool verify(BYTE * dataToVerify, size_t sizeData, PBlock & ret);
};


#endif
