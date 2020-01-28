#ifndef _commmime_h_included
#define _commmime_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"

class _CommMime
{
public:
	static void getHdrName( PString& ret, const char*& s );
	static void getHdrValue( PString& ret, const char*& s );
	static void getHdrAttr( PString& name, PString& value, const char*& s );
	static void getHdrAttrNoName( PString& name, PString& value, const char*& s, BYTE  &delim );
	static inline void skipSpace(const char*& s )
	{
		while( *s == ' ' || *s == '\t' ) ++s;
	}
};

template< class Rd >
//Rd requirements:
//int read()
class _CommMimeMultipart
{
private:
	Rd* rd;
	PString boundary;

public:
	_CommMimeMultipart( Rd& rd_, const char* boundary_ )
	{
		rd = &rd_;
		boundary = boundary_;
	}

	void skipFirstBlock();
	void readHeaderLine( PString& ret );
	bool readBlock( PBlock& block );
	bool readBlock( PString& block );

private:
	/* inline */ void _blockAppend( PBlock& block, int c );
	/* inline */ void _blockAppend( PBlock& block, const void* p, int n );
	/* inline */ void _blockAppend( PString& block, int c );
	/* inline */ void _blockAppend( PString& block, const char* p, int n );

	/* inline */ int _readC();
	/* inline */ void _readC( char cc );
};

template< class Rd >
inline void _CommMimeMultipart< Rd >::_blockAppend( PBlock& block, int c )
{
	*block.append( 1 ) = c;
}

template< class Rd >
inline void _CommMimeMultipart< Rd >::_blockAppend( PBlock& block, const void* p, int n )
{
	memcpy( block.append( n ), p, n );
}

template< class Rd >
inline void _CommMimeMultipart< Rd >::_blockAppend( PString& block, int c )
{
	block.append( (char)c );
}

template< class Rd >
inline void _CommMimeMultipart< Rd >::_blockAppend( PString& block, const char* p, int n )
{
	block.append( p, n );
}

template< class Rd >
inline int _CommMimeMultipart< Rd >::_readC()
{
	int c = rd->read();
	if( c < 0 )
		throw PInternalError( "_CommMimeMultipart: Unexpected end of input stream" );
	return c;
}

template< class Rd >
inline void _CommMimeMultipart< Rd >::_readC( char cc )
{
	int c = _readC();
	if( c != cc )
		throw PInternalError( PString( "_CommMimeMultipart: '" ).append( cc ).append( "' expected" ) );
}

template< class Rd >
void _CommMimeMultipart< Rd >::readHeaderLine( PString& ret )
{
	for(;;)
	{
		int c = _readC();
		if( c < 0 )
			throw PInternalError( "_CommMimeMultipart: Unexpected end of line" );
		if( c == '\r' )
		{
			c = _readC();
			if( c != '\n' )
				throw PInternalError( "_CommMimeMultipart: '\n' expected" );
			return;
		}
		else if( c == '\n' )
			return;
		ret.append( (char)c );
	}
}

template< class Rd >
void _CommMimeMultipart< Rd >::skipFirstBlock()
{
	static const char* delim = "--";
	int nFit = 0;
	for(;;)
	{
		int c = _readC();
		if( nFit < 2 )
		{
			if( c == delim[ nFit ] )
				++nFit;
			else
				nFit = 0;
		}
		else
		{
			if( c != boundary[ nFit - 2 ] )
				nFit = 0;
			else if( boundary[ ++nFit - 2 ] == 0 )
				break;
		}
	}

	_readC( '\r' );
	_readC( '\n' );
}

template< class Rd >
bool _CommMimeMultipart< Rd >::readBlock( PBlock& block )
{
	static const char* delim = "\r\n--";
	int nFit = 0;
	int cc = -1;
	for(;;)
	{
		int c;
		if( cc >= 0 )
		{
			c = cc;
			cc = -1;
		}
		else
			c = _readC();

		if( nFit == 0 )
		{
			if( c != '\r' )
				_blockAppend( block, c );
			else
				nFit = 1;
		}
		else if( nFit < 4 )
		{
			if( c != delim[ nFit ] )
			{
				_blockAppend( block, delim, nFit );
				cc = c;
				nFit = 0;
			}
			else
				++nFit;
		}
		else
		{
			if( c != boundary[ nFit - 4 ] )
			{
				_blockAppend( block, delim, 4 );
				_blockAppend( block, boundary, nFit - 4 );
				cc = c;
				nFit = 0;
			}
			else if( boundary[ ++nFit - 4 ] == 0 )
				break;
		}
	}

	//	_blockAppend( block, '\r' ); // see comments later
	//	_blockAppend( block, '\n' );
	///	_blockAppend( block, 0 );//sic!
	int c = _readC();
	if( c == '\r' )
	{
		_readC( '\n' );
		return false;
	}
	else if( c == '-' )
	{
		_readC( '-' );
		//		_readC( '\r' ); // Outlook in some situation doesn't append additional \r\n at the
		//		_readC( '\n' ); // end of attachments. According RFC everything after close boundary
		return true;    // has to be droped.
	}
	else
		throw PInternalError( "_CommMimeMultipart: unexpected symbol after boundary" );
}
template< class Rd >
bool _CommMimeMultipart< Rd >::readBlock( PString& block )
{
	static const char* delim = "\r\n--";
	int nFit = 0;
	int cc = -1;
	for(;;)
	{
		int c;
		if( cc >= 0 )
		{
			c = cc;
			cc = -1;
		}
		else
			c = _readC();

		if( nFit == 0 )
		{
			if( c != '\r' )
				_blockAppend( block, c );
			else
				nFit = 1;
		}
		else if( nFit < 4 )
		{
			if( c != delim[ nFit ] )
			{
				_blockAppend( block, delim, nFit );
				cc = c;
				nFit = 0;
			}
			else
				++nFit;
		}
		else
		{
			if( c != boundary[ nFit - 4 ] )
			{
				_blockAppend( block, delim, 4 );
				_blockAppend( block, boundary, nFit - 4 );
				cc = c;
				nFit = 0;
			}
			else if( boundary[ ++nFit - 4 ] == 0 )
				break;
		}
	}

	//	_blockAppend( block, '\r' ); // see comments later
	//	_blockAppend( block, '\n' );
	///	_blockAppend( block, 0 );//sic!
	int c = _readC();
	if( c == '\r' )
	{
		_readC( '\n' );
		return false;
	}
	else if( c == '-' )
	{
		_readC( '-' );
		//		_readC( '\r' ); // Outlook in some situation doesn't append additional \r\n at the
		//		_readC( '\n' ); // end of attachments. According RFC everything after close boundary
		return true;    // has to be droped.
	}
	else
		throw PInternalError( "_CommMimeMultipart: unexpected symbol after boundary" );
}

#endif
