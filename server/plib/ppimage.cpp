#include "ppincludebase.h"
#include "pperror.h"
#include "ppimage.h"

inline void PImage::_init()
{
	w = h = 0;
	lines = 0;
	alphaLines = 0;
}

inline void PImage::_init( int w_, int h_ )
{
	w = w_;
	h = h_;

	lines = new RGB*[ h ];
	alphaLines = 0;
	for( size_t i=0; i < h ; ++i )
		lines[ i ] = new RGB[ w ];
}

inline void PImage::_initWithAlpha( int w_, int h_ )
{
	w = w_;
	h = h_;

	lines = new RGB*[ h ];
	alphaLines = new BYTE*[ h ];
	for( size_t i=0; i < h ; ++i )
	{
		lines[ i ] = new RGB[ w ];
		alphaLines[ i ] = new BYTE[ w ];
	}
}

inline void PImage::_del()
{
	for( size_t i=0; i < h ; ++i )
	{
		delete [] lines[ i ];
		if( alphaLines )
			delete [] alphaLines[ i ];
	}
	delete [] lines;
	delete [] alphaLines;
}

PImage::PImage( int w_, int h_ )
{
	_init( w_, h_ );
}

PImage::PImage()
{
	_init();
}

void PImage::init( int w_, int h_ )
{
	_del();
	_init( w_, h_ );
}

void PImage::initWithAlpha( int w_, int h_ )
{
	_del();
	_initWithAlpha( w_, h_ );
}

PImage::~PImage()
{
	_del();
}

void PImage::load( PImageSource& src )
{
	int width = src.width();
	int height = src.height();
	init( width, height );

	for( int i=0; i < h ; ++i )
		src.nextRow( line( i ) );
}

void PImage::loadWithAlpha( PImageSource& src )
{
	int width = src.width();
	int height = src.height();
	initWithAlpha(width, height);

	for( int i=0; i < height; ++i )
	{
		memset( alphaLine( i ), 0xFF, width);
		src.nextRowWithAlpha( line( i ), alphaLine( i ) );
	}
}
