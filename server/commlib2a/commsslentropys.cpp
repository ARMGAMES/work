#include "ppinclude.h"
#include "commssl.h"

CommSSLEntropyFilter::CommSSLEntropyFilter()
{
	memset( buf, 0, sizeof(buf) );
}

CommSSLEntropyFilter::CommSSLEntropyFilter( BYTE init[64] )
{
	if( init == 0 )
		memset( buf, 0, sizeof(buf) );
	else
	{
		PASSERT3( sizeof(buf) == 64 );
		memcpy( buf, init, sizeof(buf) );
	}
}

UINT32 CommSSLEntropyFilter::next( UINT32 in )
{
	CommSSLChksum chksum;
	chksum.addData( buf, sizeof(buf) );

	PBlock digest;
	chksum.chksum( digest );
	PASSERT3( digest.size() >= 4 );
	size_t mv = sizeof(buf) - digest.size() - 4;
	PASSERT3( mv > 0 );

	memmove( buf + sizeof(buf) - mv, buf, mv );
	memcpy( buf, digest.ptr(), digest.size() );

	UINT32* pp = (UINT32*)( buf + digest.size() - 4 );//sic - don't care about byte order
	pp[ 1 ] = in;
	return pp[ 0 ];
}
