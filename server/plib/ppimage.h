#ifndef ppimage_h_included
#define ppimage_h_included

#include "ppincludebase.h"
#include "ppblock.h"

/* abstract */ class PImageSource;
class PImage
{
public:
	struct RGB { BYTE red; BYTE green; BYTE blue; };//don't change! some implementation (namely jpeg) relies on it

private:
	size_t w, h;
	RGB** lines;
	BYTE** alphaLines;

public:
	PImage( int w_, int h_ );
	PImage();
	void init( int w_, int h_ );
	void initWithAlpha( int w_, int h_ );
	~PImage();

	size_t width() const
	{
		return w;
	}
	size_t height() const
	{
		return h;
	}

	RGB* line( int i )
	{
		return lines[ i ];
	}
	const RGB* line( int i ) const
	{
		return lines[ i ];
	}
	BYTE* alphaLine( int i )
	{
		return alphaLines ? alphaLines[ i ] : 0;
	}
	const BYTE* alphaLine( int i ) const
	{
		return alphaLines ? alphaLines[ i ] : 0;
	}

	void load( PImageSource& src );
	void loadWithAlpha( PImageSource& src );

private:
	/* inline */ void _init();
	/* inline */ void _init( int w_, int h_ );
	/* inline */ void _initWithAlpha( int w_, int h_ );
	/* inline */ void _del();

private://prohibited
	PImage( const PImage& other );
	PImage& operator =( const PImage& other );
};

//****************************************************************************

/* abstract */ class PImageSource
{
public:
	virtual ~PImageSource() {}

public:
	virtual int width() const = 0;
	virtual int height() const = 0;

	virtual void nextRow( PImage::RGB* rgb ) = 0;

	virtual void nextRowWithAlpha( PImage::RGB* rgb, BYTE* alpha )
	{//defaults to nextRow()
		nextRow( rgb );
		//leave alpha intact, assuming that it was set to 0 originally
	}
};

class PImageImageSource : public PImageSource
{
private:
	PImage& img;
	int i;

public:
	PImageImageSource( PImage& img_ )
		: img( img_ )
	{
		i = 0;
	}

public:
	int width() const override
	{
		return static_cast< int >( img.width() );
	}
	int height() const override
	{
		return static_cast< int >( img.height() );
	}

	void nextRow( PImage::RGB* rgb ) override
	{
		PASSERT3( i < height() );
		memcpy( rgb, img.line( i ), img.width() * sizeof( PImage::RGB ) );
		++i;
	}

private: // prohibit default assignment operator
	PImageImageSource& operator=( const PImageImageSource& other );
};

//****************************************************************************

class PGifImageSource : public PImageSource
{
private:
	int w;
	int h;
	void* _impl;//private implementation a.k.a. pimpl

public:
	PGifImageSource( FILE* f );
	PGifImageSource( const BYTE* p, size_t sz );
	~PGifImageSource();

public:
	int width() const override { return w; }
	int height() const override { return h; }

	void nextRow( PImage::RGB* rgb ) override;
	void nextRowWithAlpha( PImage::RGB* rgb, BYTE* alpha ) override;

public:
	static bool isGifFile( FILE* f );//'G''I''F' header; IMPORTANT: changes file position within f
	static bool isGifFile( const BYTE* p, size_t sz );//'G''I''F' header

private:
	void _construct( void* ud, void* func );
};

class PJpegImageSource : public PImageSource
{
public:
	enum { thumbnailSmallerThanDesired = 0x00, thumbnailBiggerThanDesired = 0x01, thumbnailClipToDesired = 0x02 };

private:
	enum { _thumbnailFlagSizeMask = 0x03 };

private:
	void* _impl;//private implementation a.k.a. pimpl

public:
	PJpegImageSource( FILE* f, int scale = 1 /* allowed values are: 1, 2, 4, 8 */ );
	PJpegImageSource( const BYTE* p, size_t sz, int scale = 1 /* allowed values are: 1, 2, 4, 8 */ );
	~PJpegImageSource();

public:
	static bool isJpegFile( FILE* f );//FFD8 header; IMPORTANT: changes file position within f
	static bool isJpegFile( const BYTE* p, size_t sz );//FFD8 header
	static void createJpegFile( PBlock& ret, PImageSource& src, int quality = -1 );
	static void createJpegFile( PBlock& ret, PImageSource& src, int left, int top, int right, int bottom, int quality = -1 );
	static void createThumbnail( PBlock& ret, int& w, int& h, PImageSource& src, int desiredW, int desiredH, int quality = -1, int flags = 0 );
	static void createThumbnail( PBlock& ret, int& w, int& h, PImageSource& src, int left, int top, int right, int bottom, int desiredW, int desiredH, int quality = -1, int flags = 0 );

public:
	int width() const override;
	int height() const override;

	void nextRow( PImage::RGB* rgb ) override;
};

#endif
