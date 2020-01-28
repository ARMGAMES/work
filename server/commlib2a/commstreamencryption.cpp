#include "commstreamencryptionpike.h"
#include "commmsgbody.h"

#define highBITUINT32 0x80000000

static inline void commsum(UINT32 *f, int sizeF, const UINT32 * s, int sizeS)
{
	bool carry = false;
	while(sizeF--)
	{
		if (carry)
		{
			*f += 1;
			if (*f<1)
				carry=true;
			else 
				carry=false;
		}
		if (sizeS)
		{
			sizeS--;
			UINT32 mn = *f < *s ? *f : *s;
			*f += *s;
			if (*f<mn)
				carry = true;
			s++;
		}
		else if (carry == false)
			break;
		f++;
	}
}


static inline void commmultiply( const UINT32 *f, int sizeF, const UINT32 *s, int sizeS, UINT32 *result)
{
	int i;
	for (i=0;i<sizeS + sizeF;i++)
		result[i] = 0;
	for (int i=0;i<sizeS; i++)
	{
		if (s[i])
		{
			UINT32 *p = result + i;
			for (int j=0;j<sizeF;j++)
			{
				if (f[j])
				{
					UINT64 r;
					r  = (UINT64) f[j]* (UINT64) s[i];
#ifdef P_BIG_ENDIAN
					UINT32 r1[2];
					r1[1] = r >> 32;
					r1[0] = r & 0xffffffff;
					commsum(p , sizeS + sizeF - i,r1,2);
#else
					commsum(p , sizeS + sizeF - i,(UINT32 *)&r,2);
#endif

				}
				p++;
			}
		}
	}
}

static inline int commcompare(const UINT32 *f, const UINT32 *s, int size)
{
	while(size--)
	{
		if (*(f+size) > *(s+size))
		{
			return 1;
		}
		else if (*(f+size) < *(s+size))
		{
			return -1;
		}
	}
	return 0;
}

static inline void commsub(UINT32 *f, const UINT32 *  s, int size)
{
	bool carry = false;
	while(size--)
	{
		if (carry)
		{
			if (*f > 0)
				carry = false;
			else
				carry = true;
			(*f)--;
		}
		carry = carry || (*f < *s);
		*f++ -= *s++;
	}
	if (carry)
		return ;
}


static inline void commleftshift(UINT32 *s, int size)
{
	bool carry = false;
	while(size--)
	{
		bool carry1 = !!(*s & highBITUINT32);
		*s<<=1;
		if (carry)
			*s|=1;
		carry = carry1;
		s++;
	}
}
static inline void commrightshift(UINT32 *s, int size)
{
	bool carry = false;
	s += (size - 1);
	while(size--)
	{
		bool carry1 = !!(*s & 1);
		*s>>=1;
		if (carry)
			*s|=highBITUINT32;
		carry = carry1;
		s--;
	}
}

static inline bool commmod(UINT32 * dividend, int sizeDd, const UINT32 *divisor, int sizeDv)
{
	//reminder will be in dividend
	int nDividend = 0;
	int i;
	for (i =0; i<sizeDd;i++)
	{
		if (dividend[i])
			nDividend = i;
	}

	int nDivisor = 0;

	for (i =0; i<sizeDv;i++)
	{
		if (divisor[i])
			nDivisor = i;
	}
	if (!nDivisor && !divisor[0])
		return false;
	if (!nDividend && !dividend[0])
		return true;

	if (nDividend < nDivisor)
		return true;

	UINT32 * D = new UINT32[sizeDd];

	for (i =0; i<sizeDd ;i++)
	{
		if (i<sizeDv)
			D[i] = divisor[i];
		else
			D[i]=0;
	}

	int nBitsDividend = nDividend * sizeof(UINT32) * 8;
	UINT32 v = dividend[nDividend];
	while(v)
	{
		nBitsDividend++;
		v>>=1;
	}
	int nBitsDivisor = nDivisor * sizeof(UINT32) * 8;
	v = divisor[nDivisor];
	while(v)
	{
		nBitsDivisor++;
		v>>=1;
	}
	int bits = nBitsDividend - nBitsDivisor;

	for (i=0;i<bits;i++)
		commleftshift(D,sizeDd);

	for (i=bits; i>=0; i--)
	{
		if (commcompare(dividend,D,sizeDd)>=0)
		{
			commsub(dividend,D,sizeDd);
		}
		commrightshift(D,sizeDd);
	}
	delete [] D;
	return true;
}

void CommRSA::encrypt(const UINT32 * key, const UINT32 *publicKey, BYTE * encrypted, int size)
{
	UINT32 * D = new UINT32[size * 2];
	UINT32 * D1 = new UINT32[size * 2];
	commmultiply(key,size,key,size,D);
	commmod(D,size * 2,publicKey,size);
	commmultiply(D,size,key,size,D1);
	commmod(D1,size * 2,publicKey,size);
	for (int i=0; i < size; i++)
	{
		CommMsgBody::writeUINT32( encrypted + i * sizeof(UINT32), D1[i] );
	}

	delete [] D;
	delete [] D1;
}

#ifndef COMM_CLIENT_ONLY

void CommRSA::_convertTo(BIGNUM * to,   const UINT32 *from, int size)
{

	int bytes = size * sizeof(UINT32);
	unsigned char * b1 = new unsigned char[bytes];
	unsigned char * b = b1;

	int i = bytes;
	while (i--)
	{
		*(b++)=(unsigned char)(from[i/4]>>(8*(i%4)))&0xff;
	}
	BN_bin2bn(b1, bytes, to);
	delete [] b1;
}

void CommRSA::_convertFrom(BIGNUM * from,  UINT32 *to, int size)
{

	int bytes = size * sizeof(UINT32);
	unsigned char * b1 = new unsigned char[bytes];
	unsigned char * b = b1;

	memset(b,0,bytes);
	int n = BN_bn2bin(from,b);

	int i;
	for (i=0;i<size;i++)
		to[i] = 0;

	i     = ((n-1)/sizeof(UINT32)) + 1;
	int m = (n-1)%sizeof(UINT32);

	PASSERT3(i<=size);
	UINT32 l=0;
	while (n--)
	{
		l=(l<<8L)| *(b++);
		if (m-- == 0)
		{
			to[--i]=l;
			l=0;
			m = sizeof(UINT32) - 1;
		}
	}
	delete []b1;
}



void CommRSA::decrypt(UINT32 * decryptedKey, const UINT32 * publicKey, const UINT32 *privateKey, const BYTE * encrypted, int size)
{
	BIGNUM *encryptedBN = BN_new();
	BIGNUM *publicKeyBN = BN_new();
	BIGNUM *privateKeyBN = BN_new();
	_convertTo(publicKeyBN,publicKey,size);
	_convertTo(privateKeyBN,privateKey,size);

	int i;
	UINT32 _encrypted[10];
	for (i=0; i < size; i++)
	{
		_encrypted[i] = CommMsgBody::readUINT32( encrypted + i * sizeof(UINT32));
	}
	_convertTo(encryptedBN,_encrypted,size);
	BIGNUM *decryptedBN = BN_new();
	BN_CTX *ctx = BN_CTX_new();	
	BN_mod_exp(decryptedBN, encryptedBN,privateKeyBN,publicKeyBN, ctx);
	_convertFrom(decryptedBN,decryptedKey,size);
	BN_free(encryptedBN);
	BN_free(decryptedBN);
	BN_free(publicKeyBN);
	BN_free(privateKeyBN);
	BN_CTX_free(ctx);
}
#endif


