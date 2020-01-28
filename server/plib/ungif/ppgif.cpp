#include "ppinclude.h"
#include "pperror.h"
#include "ppimage.h"

#include "gif_lib.h"

extern "C" int _gifByteInputFunc( GifFileType* f, GifByteType* buf, int sz );

struct _IGifByteUserData
{
public:
	const BYTE* ptr;
	const BYTE* end;

public:
	_IGifByteUserData( const BYTE* ptr_, int sz )
	{
		ptr = ptr_;
		end = ptr_ + sz;
	}
};
int _gifByteInputFunc( GifFileType* f, GifByteType* buf, int sz )
{
	_IGifByteUserData* ud = (_IGifByteUserData*)f->UserData;
	if( ud->ptr == ud->end )
		return 0;
	if( sz > ( ud->end - ud->ptr ) )
		sz = ud->end - ud->ptr;
	memcpy( buf, ud->ptr, sz );
	ud->ptr += sz;
	return sz;
}

//****************************************************************************

extern "C" int _gifFileInputFunc( GifFileType* f, GifByteType* buf, int sz );

struct _IGifFileUserData
{
public:
	FILE* f;

public:
	_IGifFileUserData( FILE* f_ )
	{
		f = f_;
	}
};
int _gifFileInputFunc( GifFileType* f, GifByteType* buf, int sz )
{
	_IGifFileUserData* ud = (_IGifFileUserData*)f->UserData;
	return fread( buf, 1, sz, ud->f );
}

//****************************************************************************

class _GifColorMap
{
private:
	ColorMapObject* cMap;

public:
	_GifColorMap()
	{
		cMap = 0;
	}
	_GifColorMap( const ColorMapObject* cMap_ )
	{
		_init( cMap_ );
	}
	void init( const ColorMapObject* cMap_ )
	{
		_del();
		_init( cMap_ );
	}
	void init( int nColors )
	{
		_del();
		_init( nColors );
	}
	~_GifColorMap()
	{
		_del();
	}

public:
	ColorMapObject* colorMap()
	{
		return cMap;
	}

private:
	void _init( const ColorMapObject* cMap_ )
	{
		if( cMap_ )
			cMap = MakeMapObject( cMap_->ColorCount, cMap_->Colors );
		else
			cMap = 0;
	}
	void _init( int nColors )
	{
		cMap = MakeMapObject( nColors, 0 );
	}
	void _del()
	{
		if( cMap )
			FreeMapObject( cMap );
	}

private://prohibited
	_GifColorMap( const _GifColorMap& );
	_GifColorMap& operator =( const _GifColorMap& );
};

class _GifLine
{
private:
	GifRowType row;

public:
	_GifLine()
	{
		row = 0;
	}
	_GifLine( int width )
	{
		_init( width );
	}
	void init( int width )
	{
		_del();
		_init( width );
	}
	~_GifLine()
	{
		_del();
	}

public:
	GifRowType line()
	{
		return row;
	}

private:
	void _init( int width )
	{
		row = (GifRowType)malloc( width * sizeof(GifPixelType) );
		//printf( "malloc(GifRowType)\n" );
	}
	void _del()
	{
		if( row )
		{
			free( row );
			//printf( "free(GifRowType)\n" );
		}
	}

private://prohibited
	_GifLine( const _GifLine& );
	_GifLine& operator =( const _GifLine& );
};

class _GifImage
{
public:
	int left;
	int top;
	int width;
	int height;
	int interlace;
	_GifColorMap colorMap;
	int transparent;
private:
	_GifLine* rows;

public:
	_GifImage()
	{
		rows = 0;
		transparent = -1;
	}
	_GifImage( GifFileType* GifFileIn )
	{
		_init( GifFileIn );
	}
	void init( GifFileType* GifFileIn )
	{
		_del();
		_init( GifFileIn );
	}
	~_GifImage()
	{
		_del();
	}

public:
	GifRowType line( int y )
	{
		return rows[ y ].line();
	}

private:
	void _init( GifFileType* GifFileIn )
	{
		left = GifFileIn->Image.Left;
		top = GifFileIn->Image.Top;
		width = GifFileIn->Image.Width;
		height = GifFileIn->Image.Height;
		interlace = GifFileIn->Image.Interlace;
		colorMap.init( GifFileIn->Image.ColorMap );

		rows = new _GifLine[ height ];
		for( int i=0; i < height ; ++i )
			rows[ i ].init( width );
	}
	void _del()
	{
		delete [] rows;
	}

private://prohibited
	_GifImage( const _GifImage& );
	_GifImage& operator =( const _GifImage& );
};

struct _PGifImageSourceImpl
{
	_GifColorMap colorMap;
	_GifImage img;
	int line;
};

static void _check( bool Ok )
{
	if( !Ok )
		throw PInternalError( "GIF format error" );
}

static void _readImg( _GifImage& img, GifFileType* GifFileIn )
{
	for(;;)
	{
		GifRecordType RecordType;
		int err = DGifGetRecordType( GifFileIn, &RecordType );
		_check( err != GIF_ERROR );

		bool shouldBreak = false;
		switch( RecordType )
		{
		case IMAGE_DESC_RECORD_TYPE:
			{
				err = DGifGetImageDesc( GifFileIn );
				_check( err != GIF_ERROR );

				img.init( GifFileIn );
				if( img.interlace )
				{
					int i;
					int nn = 0;
					for( i=0; i < img.height ; i += 8 )
					{
						GifRowType line = img.line( i );
						err = DGifGetLine( GifFileIn, line, img.width );
						++nn;
						_check( err != GIF_ERROR );
					}
					for( i=4; i < img.height ; i += 8 )
					{
						GifRowType line = img.line( i );
						err = DGifGetLine( GifFileIn, line, img.width );
						++nn;
						_check( err != GIF_ERROR );
					}
					for( i=2; i < img.height ; i += 4 )
					{
						GifRowType line = img.line( i );
						err = DGifGetLine( GifFileIn, line, img.width );
						++nn;
						_check( err != GIF_ERROR );
					}
					for( i=1; i < img.height ; i += 2 )
					{
						GifRowType line = img.line( i );
						err = DGifGetLine( GifFileIn, line, img.width );
						++nn;
						_check( err != GIF_ERROR );
					}
					PASSERT3( nn == img.height );
				}
				else
				{
					for( int i=0; i < img.height ; ++i )
					{
						GifRowType line = img.line( i );
						err = DGifGetLine( GifFileIn, line, img.width );
						_check( err != GIF_ERROR );
					}
				}
				shouldBreak = true;
			}
			break;
		case EXTENSION_RECORD_TYPE:
			{
				int ExtCode;
				GifByteType* Extension;
				err = DGifGetExtension( GifFileIn, &ExtCode, &Extension );
				_check( err != GIF_ERROR );

				while( Extension )
				{
					if( ExtCode == 249 )
					{
						if( Extension[ 1 ] & 1 )
							img.transparent = Extension[ 4 ];
					}
					err = DGifGetExtensionNext( GifFileIn, &Extension );
					_check( err != GIF_ERROR );
				}
			}
			break;
		case TERMINATE_RECORD_TYPE:
			_check( 0 );
			break;
		default:		    /* Should be traps by DGifGetRecordType. */
			break;
		}//switch
		if( shouldBreak )
			break;
	}//forever
}

PGifImageSource::PGifImageSource( const BYTE* ptr, size_t sz )
{
	_impl = new _PGifImageSourceImpl();

	_IGifByteUserData ud( ptr, sz );
	_construct( &ud, (void *)_gifByteInputFunc );
}

PGifImageSource::PGifImageSource( FILE* f )
{
	_impl = new _PGifImageSourceImpl();

	_IGifFileUserData ud( f );
	_construct( &ud, (void *)_gifFileInputFunc );
}

void PGifImageSource::_construct( void* ud, void* func )
{
	_PGifImageSourceImpl* impl = (_PGifImageSourceImpl*)_impl;

	GifFileType* GifFile = DGifOpen( ud, (InputFunc)func );
	PASSERT3( GifFile );

	_GifColorMap screenColorMap( GifFile->SColorMap );

	_readImg( impl->img, GifFile );
	DGifCloseFile( GifFile );

	w = impl->img.width;
	h = impl->img.height;

	ColorMapObject* cMap = impl->img.colorMap.colorMap();
	if( !cMap )
		cMap = screenColorMap.colorMap();
	PASSERT3( cMap );
	impl->colorMap.init( cMap );
	impl->line = 0;
}

void PGifImageSource::nextRow( PImage::RGB* rgb )
{
	_PGifImageSourceImpl* impl = (_PGifImageSourceImpl*)_impl;
	GifRowType ln = impl->img.line( impl->line++ );
	for( int i=0; i < w ; ++i )
	{
		_check( ln[ i ] < impl->colorMap.colorMap()->ColorCount );
		GifColorType color = impl->colorMap.colorMap()->Colors[ ln[ i ] ];
		PImage::RGB& pixel = rgb[ i ];
		pixel.red = color.Red;
		pixel.green = color.Green;
		pixel.blue = color.Blue;
	}
}

void PGifImageSource::nextRowWithAlpha( PImage::RGB* rgb, BYTE* alpha )
{
	_PGifImageSourceImpl* impl = (_PGifImageSourceImpl*)_impl;
	GifRowType ln = impl->img.line( impl->line++ );
	for( int i=0; i < w ; ++i )
	{
		_check( ln[ i ] < impl->colorMap.colorMap()->ColorCount );
		GifColorType color = impl->colorMap.colorMap()->Colors[ ln[ i ] ];
		PImage::RGB& pixel = rgb[ i ];
		pixel.red = color.Red;
		pixel.green = color.Green;
		pixel.blue = color.Blue;

		alpha[ i ] = ln[ i ] == impl->img.transparent ? 0x00 : 0xFF;
	}
}

PGifImageSource::~PGifImageSource()
{
	_PGifImageSourceImpl* impl = (_PGifImageSourceImpl*)_impl;
	delete impl;
}

/* static */ bool PGifImageSource::isGifFile( FILE* f )
{
	int c = fgetc( f );
	if( c != 'G' )
		return false;
	c = fgetc( f );
	if( c != 'I' )
		return false;
	c = fgetc( f );
	if( c != 'F' )
		return false;
	return true;
}

/* static */ bool PGifImageSource::isGifFile( const BYTE* p, size_t sz )
{
	return sz >= 3 && p[ 0 ] == 'G' && p[ 1 ] == 'I' && p[ 2 ] == 'F';
}
