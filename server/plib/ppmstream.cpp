#include "ppstream.h"
#include "ppstring.h"
#include "pperror.h"

PMemOutputStream::PMemOutputStream( PBlock& out_ )
: out( out_ )
{
}

/* virtual */ void PMemOutputStream::write( const unsigned char* buf, size_t sz )
{
	memcpy( out.append( sz ), buf, sz ); 
}

PMemInputStream::PMemInputStream( PBlock& in_ )
: in( in_ )
{
	pos = 0;
}

/* virtual */ size_t PMemInputStream::read( unsigned char* buf, size_t sz )
{
	PASSERT5( pos <= in.size() );
	size_t left = in.size() - pos;
	if( left == 0 )
		return 0;
	if( sz > left )
		sz = left;
	memcpy( buf, in.ptr() + pos, sz );
	pos += sz;
	return sz;
}
