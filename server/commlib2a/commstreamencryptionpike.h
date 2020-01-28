#ifndef commstreamencryptionpike_h_included
#define commstreamencryptionpike_h_included
#include "commstreamencryption.h"

#define ROTR( x, y ) ( ( (x) >> (y) ) | ( (x) << (32-(y)) ) )
class CommAdditiveGenerator
{
private:
	UINT32 * regs;
	int    size;
	UINT32 current;
	int    currentPosition;
	int	   secondPosition;
	int    carryBit;
public:
	inline CommAdditiveGenerator(int size_,int second_ )
	{
		size = size_;
		regs = new UINT32[size_];
		regs[0] = 0x2b4c7a78;
		for (int i=1 ; i<size; i++)
			regs[i] = ++regs[i-1] * 371349861; 
		current = 0;
		currentPosition = 0;
		secondPosition = size - second_;
		carryBit = 0;
	}

	inline ~CommAdditiveGenerator()
	{
		memset(regs,0,sizeof(UINT32) * size);
		delete [] regs;
	}
	inline void init(UINT32 & r, const UINT32 * initData, int initDataSize)
	{
		int i;
		for (i=0;i<size;)
		{
			for (int j=0;j<initDataSize && i<size;j++,i++)
			{
				r ^= initData[j] ^ regs[i];
				r = ROTR(r,3);
				regs[i] = r;
			}
		}
		for (i=0;i<256;i++)
			clock();
	}
	inline UINT32 clock()
	{
		UINT32 curr = regs[currentPosition];
		UINT32 scnd = regs[secondPosition];
		UINT32 m = curr < scnd ? curr : scnd;
		current = regs[currentPosition] = curr + scnd;
		carryBit = m > current ? 1 : 0; 
		if (++currentPosition >= size)
			currentPosition = 0;
		if (++secondPosition >= size)
			secondPosition = 0;
		return current;
	}
	inline UINT32 get() const
	{
		return current;
	}
	inline int getCarry() const
	{
		return carryBit;
	}

};


class CommPikeStream : public CommStreamEncryption 
{
private:
	int inbound;
	struct _Gens
	{
		CommAdditiveGenerator g1;
		CommAdditiveGenerator g2;
		CommAdditiveGenerator g3;
		int shift;
		UINT32 rnd;
		_Gens():
		g1(55,24),
			g2(57,7),
			g3(58,19)
		{
			shift = 0;
		}
	} gen[2];
public:
	CommPikeStream(int inbound_)
	{
		inbound = inbound_;
	}
	inline BYTE get(_Gens &g) const
	{
		if (!g.shift)
		{
			int r = ( g.g3.getCarry() ^ g.g1.getCarry() ) << 2 |
				( g.g3.getCarry() ^ g.g2.getCarry() ) << 1 |
				( g.g2.getCarry() ^ g.g1.getCarry() );
			g.rnd = ( ((r & 6) == 6) ? g.g3.get() : g.g3.clock() ) ^
				( ((r & 3) == 3) ? g.g2.get() : g.g2.clock() ) ^
				( ((r & 5) == 5) ? g.g1.get() : g.g1.clock() );
			g.shift = 32;
		}
		g.shift-=8;
		return ( g.rnd >>g. shift ) & 0xff;
	}
	BYTE getRead()
	{
		return get(gen[inbound ? 0 : 1]);
	}
	BYTE getWrite()
	{
		return get(gen[inbound ? 1 : 0]);
	}

	inline void setKey(UINT32 initKey, const UINT32 *key, int keySize)
	{
		UINT32 r = 0x1a4b7c9d ^ initKey;
		gen[0].g1.init(r,key,keySize);
		gen[0].g2.init(r,key,keySize);
		gen[0].g3.init(r,key,keySize);
		gen[1].g1.init(r,key,keySize);
		gen[1].g2.init(r,key,keySize);
		gen[1].g3.init(r,key,keySize);

	}
};

#ifndef COMM_CLIENT_ONLY
#include <openssl/bn.h>
#endif 

class CommRSA
{
public:
	static void encrypt(const UINT32 * key, const UINT32 *publicKey, BYTE * encrypted, int size);
#ifndef COMM_CLIENT_ONLY
	static void decrypt(UINT32 * decryptedKey, const UINT32 * publicKey, const UINT32 *privateKey, const BYTE * encrypted, int size);
private:
	static void _convertTo(BIGNUM * to,   const UINT32 *from, int size);
	static void _convertFrom(BIGNUM * from,  UINT32 *to, int size);
#endif
};


#endif
