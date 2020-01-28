#include "ppinclude.h"
#include "pperror.h"
#include "ppimage.h"
#include "ppblock.h"

#define XMD_H//hack - to avoid duplicate definition of INT16/INT32
#ifdef FAR
#undef FAR//to avoid duplicate definition of FAR
#endif
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"		/* get library error codes too */

// This file is based in part on the work of Independent JPEG Group's.

typedef struct {
	struct jpeg_destination_mgr pub;

	PBlock* out;
	JOCTET* buffer;
} p_byte_destination_mgr;
typedef p_byte_destination_mgr* p_byte_dest_ptr;
#define OUTPUT_BUF_SIZE 4096

static void init_byte_destination( j_compress_ptr cinfo )
{
	p_byte_dest_ptr dest = (p_byte_dest_ptr) cinfo->dest;

	/* Allocate the output buffer --- it will be released when done with image */
	dest->buffer = (JOCTET *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

static boolean empty_byte_output_buffer( j_compress_ptr cinfo )
{
	p_byte_dest_ptr dest = (p_byte_dest_ptr) cinfo->dest;

	memcpy( dest->out->append( OUTPUT_BUF_SIZE ), dest->buffer, OUTPUT_BUF_SIZE );
	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}

static void term_byte_destination( j_compress_ptr cinfo )
{
	p_byte_dest_ptr dest = (p_byte_dest_ptr) cinfo->dest;
	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

	/* Write any data remaining in the buffer */
	if( datacount > 0 )
		memcpy( dest->out->append( datacount ), dest->buffer, datacount );
}

static void jpeg_pbytedest( j_compress_ptr cinfo, PBlock& out )
{
	p_byte_dest_ptr dest;

	/* The destination object is made permanent so that multiple JPEG images
	* can be written to the same file without re-executing jpeg_stdio_dest.
	* This makes it dangerous to use this manager and a different destination
	* manager serially with the same JPEG object, because their private object
	* sizes may be different.  Caveat programmer.
	*/
	if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			SIZEOF(p_byte_destination_mgr));
	}

	dest = (p_byte_dest_ptr)cinfo->dest;
	dest->pub.init_destination = init_byte_destination;
	dest->pub.empty_output_buffer = empty_byte_output_buffer;
	dest->pub.term_destination = term_byte_destination;
	dest->out = &out;
}

static void _toJpeg( PBlock& out, PImageSource& src, int left, int top, int right, int bottom, int quality, int inflate )
{
	out.clear();
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress( &cinfo );

	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults( &cinfo );

	jpeg_default_colorspace( &cinfo );
	jpeg_pbytedest( &cinfo, out );

	int ww = right - left;
	int hh = bottom - top;
	int w = ww * inflate;
	int h = hh * inflate;

	cinfo.in_color_space = JCS_RGB;
	cinfo.input_components = 3;
	cinfo.data_precision = 8;
	cinfo.image_width = (JDIMENSION)w;
	cinfo.image_height = (JDIMENSION)h;
	if( quality >= 0 && quality <= 100 )
		jpeg_set_quality( &cinfo, quality, TRUE );

	jpeg_start_compress( &cinfo, TRUE );

	PBlock line( sizeof(PImage::RGB) * src.width() );
	PBlock line2;
	if( inflate != 1 )
		line2.alloc( sizeof(PImage::RGB) * ww * inflate );
	BYTE* p = line.ptr();
	BYTE* p2 = line2.ptr();
	for( int i=0; i < src.height() ; ++i )
	{
		src.nextRow( (PImage::RGB*)p );
		if( i < top || i >= bottom )
			continue;
		if( inflate == 1 )
		{
			BYTE* pp = p + left * sizeof(PImage::RGB);
			jpeg_write_scanlines( &cinfo, &pp, 1 );
		}
		else
		{
			PImage::RGB* rgbSrc = ((PImage::RGB*)p) + left;
			PImage::RGB* rgbDst = (PImage::RGB*)p2;
			int k;
			for( k=0; k < ww ; ++k )
			{
				for( int j=0; j < inflate ; ++j )
					*rgbDst++ = *rgbSrc;
				++rgbSrc;
			}
			for( k=0; k < inflate ; ++k )
				jpeg_write_scanlines( &cinfo, &p2, 1 );
		}
	}
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
}

static void _toJpeg( PBlock& out, PImageSource& src, int quality, int inflate )
{
	_toJpeg( out, src, 0, 0, src.width(), src.height(), quality, inflate );
}

/* static */ void PJpegImageSource::createJpegFile( PBlock& ret, PImageSource& src, int left, int top, int right, int bottom, int quality )
{
	_toJpeg( ret, src, left, top, right, bottom, quality, 1 );
}

/* static */ void PJpegImageSource::createJpegFile( PBlock& ret, PImageSource& src, int quality )
{
	createJpegFile( ret, src, 0, 0, src.width(), src.height(), quality );
}

/* static */ void PJpegImageSource::createThumbnail( PBlock& ret, int& w, int& h, PImageSource& src, int left, int top, int right, int bottom, int desiredW, int desiredH, int quality, int flags )
{
	w = right - left;
	h = bottom - top;

	if( w == desiredW && h == desiredH )
	{
		_toJpeg( ret, src, left, top, right, bottom, quality, 1 );
		return;
	}

	int div = 0;
	while( w / ( 1 << ( div + 1 ) ) > desiredW || h / ( 1 << ( div + 1 ) ) > desiredH )
		div += 1;

	div += 3;
	int mul = 4;
	if( ( flags & _thumbnailFlagSizeMask ) == thumbnailSmallerThanDesired )
	{
		for( int i=5; i <= 8 ; ++i )
		{
			if( ( w * i ) / ( 1 << div ) > desiredW )
				break;
			if( ( h * i ) / ( 1 << div ) > desiredH )
				break;
			mul = i;
		}
	}
	else
	{
		for( int i=5; i <= 8 ; ++i )
		{
			mul = i;
			if( ( w * i ) / ( 1 << div ) >= desiredW &&
				( h * i ) / ( 1 << div ) >= desiredH )
				break;
		}
	}

	while( div && ( mul % 2 ) == 0 )
	{
		mul /= 2;
		div -= 1;
	}

	if( div == 0 && mul == 1 )
	{
		_toJpeg( ret, src, left, top, right, bottom, quality, 1 );
		return;
	}

	int divDiv = div / 3;
	int divRem = div % 3;

	_toJpeg( ret, src, left, top, right, bottom, 100, 1 );
	if( mul != 1 && divDiv == 0 )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 1 );
		PBlock tmp;
		_toJpeg( tmp, jpeg, 100, mul );
		//		w *= mul;
		//		h *= mul;

		ret = tmp;
		mul = 1;
	}

	if( divRem )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 1 << divRem );
		PBlock tmp;
		_toJpeg( tmp, jpeg, 100, 1 );
		ret = tmp;
	}

	if( mul != 1 && divDiv == 1 )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 1 );
		PBlock tmp;
		_toJpeg( tmp, jpeg, 100, mul );
		//		w *= mul;
		//		h *= mul;

		ret = tmp;
		mul = 1;
	}

	for( int j=0; j < divDiv - 1 ; ++j )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 8 );
		PBlock tmp;
		_toJpeg( tmp, jpeg, 100, 1 );
		ret = tmp;
	}

	if( mul != 1 )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 1 );
		PBlock tmp;
		_toJpeg( tmp, jpeg, 100, mul );
		//		w *= mul;
		//		h *= mul;

		ret = tmp;
		mul = 1;
	}

	if( divDiv )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 8 );
		PBlock tmp;
		_toJpeg( tmp, jpeg, 100, 1 );
		ret = tmp;
	}

	if( quality != 100 || (( flags & _thumbnailFlagSizeMask ) == thumbnailClipToDesired) )
	{
		PJpegImageSource jpeg( ret.ptr(), ret.size(), 1 );
		int left_ = 0;
		int top_ = 0;
		int w_ = jpeg.width();
		int h_ = jpeg.height();
		if( ( flags & _thumbnailFlagSizeMask ) == thumbnailClipToDesired )
		{
			int dw = w_ - desiredW;
			if( dw > 0 )
			{
				left_ = dw / 2;
				w_ = desiredW;
			}
			int dh = h_ - desiredH;
			if( dh > 0 )
			{
				top_ = dh / 2;
				h_ = desiredH;
			}
		}

		PBlock tmp;
		_toJpeg( tmp, jpeg, left_, top_, left_ + w_, top_ + h_, quality, 1 );
		ret = tmp;
	}
}

/* static */ void PJpegImageSource::createThumbnail( PBlock& ret, int& w, int& h, PImageSource& src, int desiredW, int desiredH, int quality, int flags )
{
	createThumbnail( ret, w, h, src, 0, 0, src.width(), src.height(), desiredW, desiredH, quality, flags );
}
