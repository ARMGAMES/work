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


//****************************************************************************

CommSSLDH::CommSSLDH()
{
	dh = 0;
}

CommSSLDH::~CommSSLDH()
{
	if(dh) 
		DH_free((DH*)dh);
}

void CommSSLDH::duplicate(CommSSLDH * other)
{
	PASSERT3(other->dh);
	if (dh)
		DH_free((DH*)dh);
	dh = (void *)DH_new();
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
	((DH*)dh)->p=BN_dup(((DH*)other->dh)->p);
	((DH*)dh)->g=BN_dup(((DH*)other->dh)->g);
#else
	const BIGNUM *p;
	const BIGNUM *g;
	DH_get0_pqg((DH*)other->dh,&p,0,&g);
	DH_set0_pqg((DH*)dh,BN_dup(p),0,BN_dup(g));
#endif	
}

void CommSSLDH::generateParameters(int prime_len)
{
	int retCode = 0;
	do
	{
		if (dh)
			DH_free((DH*)dh);
		dh = (void *)DH_new();
		PASSERT3(dh);
		int ok = DH_generate_parameters_ex((DH*)dh, prime_len, DH_GENERATOR_5, 0);
		PASSERT3(ok);
		ok = DH_check((DH*)dh, &retCode);
		PASSERT3(ok);
	} 
	while(retCode & (DH_CHECK_P_NOT_PRIME | DH_UNABLE_TO_CHECK_GENERATOR | DH_NOT_SUITABLE_GENERATOR));
}

void CommSSLDH::writeParameters(const char * outFileName)
{
	PASSERT3(dh);
	BIO* out = BIO_new( BIO_s_file() );
	PASSERT3( out );
	int ok = BIO_write_filename( out, (char*)outFileName );
	PASSERT3( ok > 0 );
	PEM_write_bio_DHparams(out,(DH*)dh);
	BIO_free( out );
}


void CommSSLDH::composeParameters(CommMsgBody & body)
{
	PASSERT3(dh);
	const BIGNUM *p;
	const BIGNUM *g;
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
	p = ((DH*)dh)->p;
	g = ((DH*)dh)->g;
#else
	DH_get0_pqg((DH*)dh,&p,0,&g);
#endif
	int len = BN_num_bytes(p );
	BYTE * abuf=(BYTE *)OPENSSL_malloc(len);
	BN_bn2bin( p, abuf);
	body._composeVarBlock(abuf,len);
	OPENSSL_free(abuf);
	len = BN_num_bytes( g );
	abuf=(BYTE *)OPENSSL_malloc(len);
	BN_bn2bin( g, abuf);
	body._composeVarBlock(abuf,len);
	OPENSSL_free(abuf);
}

void CommSSLDH::getPrime(PString &pst)
{
	PASSERT3(dh);
	const BIGNUM *p;
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
	p = ((DH*)dh)->p;
#else
	DH_get0_pqg((DH*)dh,&p,0,0);
#endif

	int len = BN_num_bytes( p );
	BYTE * abuf=(BYTE *)OPENSSL_malloc(len);
	BN_bn2bin( p, abuf);

	for (int i=0;i<len; i++)
	{
		if (i)
			pst.append(",");
		pst.append("0x").appendHexInt(abuf[i]);
	}
}
void CommSSLDH::setPrime(const BYTE *ptr, int len, int gen)
{
	if (dh)
	{
		DH_free((DH*)dh);
		dh=0;
	}
	dh=(void *)DH_new();
	PASSERT3(dh);
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
	((DH*)dh)->p = BN_bin2bn(ptr,len,0); 
	((DH*)dh)->g = BN_new();
	BN_set_word(((DH*)dh)->g, gen);
#else
	BIGNUM *p;
	BIGNUM *g;
	p = BN_bin2bn(ptr,len,0); 
	g = BN_new();
	BN_set_word(g, gen);
	DH_set0_pqg((DH*)dh,p,0,g);
#endif
}


void CommSSLDH::parseParameters(CommMsgParser & parser)
{
	if (dh)
	{
		DH_free((DH*)dh);
		dh=0;
	}
	dh=(void *)DH_new();
	PASSERT3(dh);
	const BYTE * ptr;
	size_t len;
	parser._parseVarBlock(ptr,len);
	BIGNUM *p = BN_bin2bn(ptr,len,0); 
	parser._parseVarBlock(ptr,len);
	BIGNUM *g = BN_bin2bn(ptr,len,0); 
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
	((DH*)dh)->p = p;
	((DH*)dh)->g = g;
#else
	DH_set0_pqg((DH*)dh,p,0,g);
#endif

}	



void CommSSLDH::readParameters(const char * inFileName)
{
	if (dh)
	{
		DH_free((DH*)dh);
		dh=0;
	}
	BIO* in = BIO_new( BIO_s_file() );
	PASSERT3( in );
	int ok = BIO_read_filename( in, inFileName );
	PASSERT3( ok > 0 );
	dh = (void *)PEM_read_bio_DHparams(in,0,0,0);
	BIO_free( in );
}

void CommSSLDH::generateKeys(PBlock & pubKey)
{
	PASSERT3(dh);
	int ok = DH_generate_key((DH*)dh);
	PASSERT3(ok > 0);
	const BIGNUM * pub_key;
#if OPENSSL_VERSION_NUMBER < 0x10100000L || OPENSSL_VERSION_NUMBER == 0x20000000L
	pub_key = ((DH*)dh)->pub_key;
#else
	DH_get0_key((DH*)dh,&pub_key,0);
#endif

	pubKey.alloc( BN_num_bytes( pub_key ));
	BN_bn2bin( pub_key, pubKey.ptr());
}

void CommSSLDH::generateSecret(const BYTE * pubKey, int len ,PBlock & secret)
{

	BIGNUM * pKey;
	PASSERT3(dh);

	pKey = BN_bin2bn(pubKey,len,0);
	int alen=DH_size((DH*)dh);
	PASSERT3(alen > 0);
	BYTE * abuf=(BYTE *	)OPENSSL_malloc(alen);
	int aout=DH_compute_key(abuf,pKey,(DH*)dh);
	if (aout <= 0)
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLDH::generateSecret SSL error %d, public key length=%d", errReson,len);
		PString hPub;
		if (len>32)
			len=32;
		while(len-->0)
		{
			hPub.append("0x").appendHexInt(*pubKey++).append(",");
		}
		PTRACE3("pubkey: %s",hPub.c_str());
		throw PError("CommSSLDH::generateSecret SSL error");
	}
	BN_free(pKey);
	secret.append(aout);
	memcpy(secret.ptr(),abuf,aout);
	OPENSSL_free(abuf);
}



CommSSLECDH::CommSSLECDH()
{
	ecdh = 0;
}

CommSSLECDH::~CommSSLECDH()
{
	if(ecdh) 
		EC_KEY_free((EC_KEY*)ecdh);
}
void CommSSLECDH::generateKeys(PBlock & publicKey)
{
	ecdh = EC_KEY_new_by_curve_name(NID_secp224r1);
	if(!ecdh)
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateKeys EC_KEY_new_by_curve_name SSL error %d", errReson);
		throw PError("CommSSLECDH::generateKeys SSL error");
	}
	if (!EC_KEY_generate_key((EC_KEY*)ecdh))
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateKeys EC_KEY_generate_key SSL error %d", errReson);
		throw PError("CommSSLECDH::generateKeys SSL error");
	}
	int len = i2o_ECPublicKey((EC_KEY*)ecdh,0);
	if(len <= 0)
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateKeys i2o_ECPublicKey SSL error %d", errReson);
		throw PError("CommSSLECDH::generateKeys SSL error");
	}
	publicKey.alloc(len);
	unsigned char * p = publicKey.ptr();
	len = i2o_ECPublicKey((EC_KEY*)ecdh,&p);
	if(len <= 0)
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateKeys i2o_ECPublicKey SSL error %d", errReson);
		throw PError("CommSSLECDH::generateKeys SSL error");
	}
}
void CommSSLECDH::generateSecret(const BYTE * pubKey, int len, PBlock & secret)
{
	PASSERT3(ecdh);
	EC_KEY *peerKey;
	peerKey = EC_KEY_new_by_curve_name(NID_secp224r1);
	if(!peerKey)
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateSecret EC_KEY_new_by_curve_name SSL error %d", errReson);
		throw PError("CommSSLECDH::generateSecret SSL error");
	}
	if(!o2i_ECPublicKey(&peerKey,&pubKey,len ))
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateSecret o2i_ECPublicKey SSL error %d", errReson);
		throw PError("CommSSLECDH::generateSecret SSL error");
	}
	int field_size = EC_GROUP_get_degree(EC_KEY_get0_group((EC_KEY *)ecdh));
	int secret_len = (field_size+7)/8;
	secret.alloc(secret_len);
	secret_len = ECDH_compute_key(secret.ptr(), secret_len, EC_KEY_get0_public_key(peerKey),(EC_KEY *)ecdh, NULL);
	if(secret_len <=0 )
	{
		int errReson = ERR_GET_REASON(ERR_get_error());
		PTRACE3("PError: CommSSLECDH::generateSecret ECDH_compute_key SSL error %d", errReson);
		throw PError("CommSSLECDH::generateSecret SSL error");
	}
	EC_KEY_free(peerKey);
}


