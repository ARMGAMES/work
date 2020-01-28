//nextrandom.h
#ifndef nextrandom_h_included
#define nextrandom_h_included

class RandomSimple
{
private:
	UINT32 rnd;

public:
	RandomSimple()
	{
		rnd = 127;
	}

public:
	int next()
	{
        return( ((rnd = rnd * 214013L + 2531011L) >> 16) & 0x7fff );
	}
	int next( int mx )
	{
		PASSERT( mx <= 32768 );
		return next() % mx;
	}
};

class Random
{
private:
	UINT32 rnd;

public:
	Random()
	{
		rnd = RANDOMTIME();
	}

public:
	int next()
	{
        return( ((rnd = rnd * 214013L + 2531011L) >> 16) & 0x7fff );
	}
	int next( int mx )
	{
		PASSERT((mx != 0) && (mx <= 32768));
		return next() % mx;
	}
};

class RandomEx
{
private:
	UINT32 rnd;

public:
	RandomEx()
	{
		rnd = 0;
	}
	UINT32 next( UINT32 seed )
	{
		rnd <<= 1;
		rnd ^= seed;
		return( ((rnd * 214013L + 2531011L) >> 16) & 0x7fff );
	}
	int next(  UINT32 seed, int mx )
	{
		PASSERT( mx <= 32768 );
		return next( seed ) % mx;
	}
};

// http://www.jstatsoft.org/v08/i14/paper and http://en.wikipedia.org/wiki/Xorshift
class PrngXorShift64
{
	UINT64 rnd;
public:
	PrngXorShift64()
	{
		seedTime();
	}
	void seedTime()
	{
		UINT64 rndTime = (UINT16)(RANDOMTIME() >> 8);
		rnd = rndTime | (rndTime << 16) | (rndTime << 32) | (rndTime << 48);
	}
	void seed ( UINT64 seed_ )
	{
		rnd = seed_;
	}
	UINT64 next64()
	{
		const UINT32 a = 13, b = 7, c = 17;
		rnd ^= (rnd << a);
		rnd ^= (rnd >> b);
		rnd ^= (rnd << c);
		return rnd;
	}
	UINT32 next32()
	{
		return (UINT32)(next64() >> 16);
	}
	UINT32 next32(UINT32 mx)
	{
		return (next32() % mx);
	}
	UINT32 realNext32(UINT32 mx)	//consult VadimSh before using
	{
		if(mx == 0) return next32();
		if(mx == 1) return 0;

		UINT32 mask = 1;
		while(mask < mx) mask = ( (mask << 1) | 1 );

		UINT32 res;
		do { res = (next32() & mask); } while(res >= mx);
		return res;
	}
	UINT64 realNext64(UINT64 mx)	//consult VadimSh before using
	{
		if(mx == 0) return next64();
		if(mx == 1) return 0;

		UINT64 mask = ONE64;
		while(mask < mx) mask = ( (mask << 1) | ONE64 );

		UINT64 res;
		do { res = (next64() & mask); } while(res >= mx);
		return res;
	}
};

class PrngXorShift256
{
	UINT64 x, y, z, w;
public:
	PrngXorShift256()
	{
		seedTime();
	}
	void seedTime()
	{
		PrngXorShift64 xor64; // RANDOMTIME
		x = xor64.next64();
		y = xor64.next64();
		z = xor64.next64();
		w = xor64.next64();
	}
	void seed(UINT64 x_, UINT64 y_, UINT64 z_, UINT64 w_)
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}
	UINT64 next64()
	{
		const UINT32 a = 22, b = 16, c = 38;
		UINT64 t = x ^ (x << a);
		x = y;
		y = z;
		z = w;
		w = w ^ (w >> c) ^ (t ^ (t >> b));
		return w;
	}
	UINT32 next32()
	{
		return UINT32(next64() >> 16);
	}
	UINT32 next32(UINT32 mx)
	{
		return UINT32(next64() % mx);
	}
	UINT32 realNext32(UINT32 mx)	//consult VadimSh before using
	{
		if(mx == 0) return next32();
		if(mx == 1) return 0;

		UINT32 mask = 1;
		while(mask < mx) mask = ( (mask << 1) | 1 );

		UINT32 res;
		do { res = (next32() & mask); } while(res >= mx);
		return res;
	}
	UINT64 realNext64(UINT64 mx)	//consult VadimSh before using
	{
		if(mx == 0) return next64();
		if(mx == 1) return 0;

		UINT64 mask = ONE64;
		while(mask < mx) mask = ( (mask << 1) | ONE64 );

		UINT64 res;
		do { res = (next64() & mask); } while(res >= mx);
		return res;
	}
};

//http://www.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
//passed all Dieharder and BigCrunch tests, period ~2^127
class PrngJKISS
{
	UINT32 x, y, z, c;

public:
	PrngJKISS(UINT32 x_, UINT32 y_, UINT32 z_, UINT32 c_)
	{
		seed(x_, y_, z_, c_);
	}

	void seed(UINT32 x_, UINT32 y_, UINT32 z_, UINT32 c_)
	{
		x = x_;
		y = (y_ ? y_ : 1);	// != 0
		z = z_;
		c = c_ % 698769068 + 1;	// != 0 and <= 698769068, it garanties that both z and c never 0
	}

	UINT32 next32()
	{
		x = 314527869 * x + 1234567;	//overflow is OK
		y ^= (y << 5);
		y ^= (y >> 7);
		y ^= (y << 22);
		UINT64 t = (UINT64)4294584393 * z + c;  //lint !e694  The type of constant '4294584393' (precision 32) is dialect dependent
		z = (UINT32)t;
		c = (UINT32)(t >> 32);
		return (x + y + z);
	}

	UINT32 realNext32(UINT32 mx)	//consult VadimSh before using
	{
		if(mx == 0) return next32();
		if(mx == 1) return 0;

		UINT32 mask = 1;
		while(mask < mx) mask = ( (mask << 1) | 1 );

		UINT32 res;
		do { res = (next32() & mask); } while(res >= mx);
		return res;
	}

	UINT64 next64()
	{
		UINT64 res = next32();
		return ( (res << 32) + next32() );
	}

	UINT64 realNext64(UINT64 mx)	//consult VadimSh before using
	{
		if(mx == 0) return next64();
		if(mx == 1) return 0;

		UINT64 mask = ONE64;
		while(mask < mx) mask = ( (mask << 1) | ONE64 );

		UINT64 res;
		do { res = (next64() & mask); } while(res >= mx);
		return res;
	}
};

#endif	///nextrandom_h_included
