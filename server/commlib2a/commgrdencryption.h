#ifndef commgrdencryption_h
#define commgrdencryption_h

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commsrv.h"
#include "commssl.h"


class CommGuardEncryption
{
protected:
	enum {_version_1=1,_version_2=2,_version_3=3};
	PBlock    secret;
	INT32  peerVersion;
public:
	CommGuardEncryption()
	{
		peerVersion = 0;
	}
	~CommGuardEncryption()
	{
	}
public:
	bool isInit() const
	{
		return secret.size() > 0;
	}
	void encryptData(const BYTE *data, size_t len, PBlock & encrypted) const
	{
		if (len > 0)
		{
			UINT32 rnd1=rand()<<16|rand();
			UINT32 rnd2=SYSTEMTICKS();
			if (peerVersion == _version_1)
			{
				encrypted.append( len + 2 * sizeof(UINT32));
				CommMsgBody::writeUINT32(encrypted.ptr(),rnd1);
				CommMsgBody::writeUINT32(encrypted.ptr()+sizeof(UINT32),rnd2);
				CommSSLStreamEncrypt encrypt(secret.ptr(),secret.size(),encrypted.ptr());
				encrypt.encrypt(( const BYTE *)data,len,encrypted.ptr() + 2*sizeof(UINT32));
			}
			else 
			{
				encrypted.append( 2 * sizeof(UINT32));
				CommMsgBody::writeUINT32(encrypted.ptr(),rnd1);
				CommMsgBody::writeUINT32(encrypted.ptr()+sizeof(UINT32),rnd2);
				CommSSLEncryptAES encrypt(secret.ptr(),secret.size(),false,encrypted.ptr());
				encrypt.addData(( const BYTE *)data,len);
				PBlock e;
				encrypt.encrypt(e);
				memcpy(encrypted.append(e.size()),e.ptr(),e.size());
			}
		}

	}
	void decryptData(const BYTE *encrData, size_t len, PBlock & decr) const
	{
		if (len > 0)
		{
			PASSERT3(len > 2*sizeof(UINT32) );
			if (peerVersion == _version_1)
			{
				CommSSLStreamDecrypt decrypt(secret.ptr(),secret.size(),encrData);
				decrypt.decrypt(( const BYTE *)encrData + 2*sizeof(UINT32),len - 2*sizeof(UINT32) ,decr);
			}
			else
			{
				CommSSLDecryptAES decrypt(secret.ptr(),secret.size(),false,encrData);
				decrypt.addData(encrData+ 2*sizeof(UINT32),len - 2*sizeof(UINT32));
				PBlock e;
				decrypt.decrypt(e);
				memcpy(decr.append(e.size()),e.ptr(),e.size());
			}
		}

	}
	void encryptData(const CommMsgBody & src,PBlock & encrypted) const
	{
		if (src._size() > 0)
		{
			UINT32 rnd1=rand()<<16|rand();
			UINT32 rnd2=SYSTEMTICKS();
			if (peerVersion == _version_1)
			{
				encrypted.append( src._size() + 2 * sizeof(UINT32));
				CommMsgBody::writeUINT32(encrypted.ptr(),rnd1);
				CommMsgBody::writeUINT32(encrypted.ptr()+sizeof(UINT32),rnd2);
				CommSSLStreamEncrypt encrypt(secret.ptr(),secret.size(),encrypted.ptr());
				encrypt.encrypt(src._readPtr(), src._size(), encrypted.ptr() + 2*sizeof(UINT32));
			}
			else 
			{
				encrypted.append( 2 * sizeof(UINT32));
				CommMsgBody::writeUINT32(encrypted.ptr(),rnd1);
				CommMsgBody::writeUINT32(encrypted.ptr()+sizeof(UINT32),rnd2);
				CommSSLEncryptAES encrypt(secret.ptr(),secret.size(),false,encrypted.ptr());
				encrypt.addData(src._readPtr(), src._size());
				PBlock e;
				encrypt.encrypt(e);
				memcpy(encrypted.append(e.size()),e.ptr(),e.size());
			}

		}

	}
	void decryptData(const BYTE *encrData, size_t len, CommMsgBody & dst) const
	{
		if (len > 0)
		{
			PASSERT3(len > 2*sizeof(UINT32) );
			if (peerVersion == _version_1)
			{
				dst._append( len-2*sizeof(UINT32));
				CommSSLStreamDecrypt decrypt(secret.ptr(),secret.size(),encrData);
				decrypt.decrypt(( const BYTE *)encrData + 2*sizeof(UINT32),len - 2*sizeof(UINT32), dst._writePtr());
			}
			else
			{
				CommSSLDecryptAES decrypt(secret.ptr(),secret.size(),false,encrData);
				decrypt.addData(encrData+ 2*sizeof(UINT32),len - 2*sizeof(UINT32));
				PBlock e;
				decrypt.decrypt(e);
				memcpy(dst._append(e.size()),e.ptr(),e.size());
			}
		}
	}
};




class CommClientGuardEncryption : public CommGuardEncryption
{
private:
	CommSSLDH *dh;
	CommSSLECDH *ecdh;
	UINT32 myInitVersion;

public:
	CommClientGuardEncryption()
	{
		dh=0;
		ecdh=0;
		myInitVersion = _version_2;
	}
	~CommClientGuardEncryption()
	{
		if (dh)
			delete dh;
		if (ecdh)
			delete ecdh;

	}
public:

	void composeFirstRequest( CommMsgBody& msgBody );
	void processResponse( CommMsgParser& parser )
	{
		secret.clear();
		const BYTE * serverPublicKey;
		size_t pbLen;
		parser.parseINT32(peerVersion);
		if(myInitVersion == _version_3 && peerVersion != _version_3)
		{
			throw PError("CommClientGuardEncryption: incompatible server version");
		}
		parser._parseVarBlock(serverPublicKey,pbLen);
		if(myInitVersion == _version_3)
		{
			ecdh->generateSecret(serverPublicKey,pbLen,secret);
			delete ecdh;
			ecdh=0;
		}
		else
		{
			dh->generateSecret(serverPublicKey,pbLen,secret);
			delete dh;
			dh=0;
		}
	}
};

class CommServerGuardEncryption : public CommGuardEncryption
{
private:
	PBlock publicKey;
public:
	CommServerGuardEncryption()
	{
	}
	void processFirstRequest(  CommMsgParser& parser )
	{
		parser.parseINT32(peerVersion);
		publicKey.clear();
		size_t len;
		const BYTE * clientPublicKey;
		if(peerVersion == _version_3)
		{
			CommSSLECDH *	ecdh = new CommSSLECDH();
			parser._parseVarBlock(clientPublicKey,len);
			ecdh->generateKeys(publicKey);
			ecdh->generateSecret(clientPublicKey,len,secret);
			delete ecdh;
			ecdh=0;
		}
		else
		{
			CommSSLDH *	dh = new CommSSLDH();
			const BYTE * prime;
			parser._parseVarBlock(prime,len);
			dh->setPrime(prime,len,5);
			parser._parseVarBlock(clientPublicKey,len);
			dh->generateKeys(publicKey);
			dh->generateSecret(clientPublicKey,len,secret);
			delete dh;
			dh=0;
		}
	}
	void composeResponse( CommMsgBody& msgBody )
	{
		msgBody.composeINT32(peerVersion);
		msgBody._composeVarBlock(publicKey.ptr(),publicKey.size());
		publicKey.clear();
	}
};

#endif
