#include "ppinclude.h"
#include "commssl.h"

CommSSLEntropyCollector::CommSSLEntropyCollector( int maxBlock_ )
{
	maxBlock = maxBlock_;
	memset( buf0, 0, sizeof(buf0) );
	buf0Fill = 0;
	buf0Entropy = 0.;
	buf = 0;
	bufUsed = 0;
}

void CommSSLEntropyCollector::addEntropy( const BYTE* p, size_t sz, double entropyBits )
{
	if( buf0Fill + sz >= sizeof(buf0) )
	{//time to calculate SHA1 digest from buf0
		CommSSLChksum chksum;
		chksum.addData( p, sz );
		chksum.addData( buf0, buf0Fill );
		PBlock digest;
		chksum.chksum( digest );
		buf0Entropy += entropyBits;

		int bits = (int)buf0Entropy;
		//whole digest contains only 'bits' bits of entropy
		if( bits > digest.size() * 8 )//we can lose some entropy here - but it is highly unlikely
			bits = digest.size() * 8;
		if( bits >= 1 )
		{
			for( int i=0; i < bits ; ++i )//not optimal but simple
			{
				int ii = i / 8;
				PASSERT3( ii >= 0 && ii < digest.size() );
				bool digestBit = ( digest.ptr()[ ii ] & ( 1 << ( i - ii * 8 ) ) ) != 0;
				if( digestBit )
					buf ^= 1 << bufUsed;
				++bufUsed;
				if( bufUsed == 8 )
				{//got whole random byte
					if( out.size() < maxBlock )//not filled yet
						*(out.append( 1 )) = buf;
					else//otherwise discard 1st byte
					{
						memmove( out.ptr(), out.ptr() + 1, out.size() - 1 );
						out.ptr()[ out.size() - 1 ] = buf;
					}
					buf = 0;
					bufUsed = 0;
				}
			}
			memset( buf0, 0, sizeof(buf0) );
			buf0Fill = 0;
			buf0Entropy = 0.;
		}
		else//normally should not happen, but...
		{
			//leave buf0 as is
			buf0Fill = 0;
		}
	}
	else
	{//continue filling buf0
		for( int i=0; i < sz ; ++i )
			buf0[ buf0Fill++ ] ^= *p++;
		PASSERT3( buf0Fill <= sizeof(buf0) );
		buf0Entropy += entropyBits;
	}
}

void CommSSLEntropyCollector::extractEntropy( PBlock& ret )
{
	ret.moveFrom( out );
	out.clear();//just in case
}

void CommSSLEntropyCollector::extractEntropy( BYTE* p, size_t entropyBytes )
{
	PASSERT3( entropyBytes <= out.size() );
	size_t offset = out.size() - entropyBytes;
	memcpy( p, out.ptr() + offset, entropyBytes );
	out.cut( offset );
}
