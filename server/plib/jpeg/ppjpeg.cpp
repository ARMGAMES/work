#include "ppinclude.h"
#include "pperror.h"
#include "ppimage.h"

#define XMD_H//hack - to avoid duplicate definition of INT16/INT32
#ifdef FAR
#undef FAR//to avoid duplicate definition of FAR
#endif
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"		/* get library error codes too */

// This file is based in part on the work of Independent JPEG Group's.

#define INPUT_BUF_SIZE  4096

//*****************************************************************************

typedef struct {
	struct jpeg_source_mgr pub;

	FILE* in;
	JOCTET * buffer;
	boolean start_of_file;
} p_file_source_mgr;
typedef p_file_source_mgr* p_file_src_ptr;

static void init_file_source( j_decompress_ptr cinfo )
{
	p_file_src_ptr src = (p_file_src_ptr)cinfo->src;
	src->start_of_file = TRUE;
}

static boolean fill_file_input_buffer( j_decompress_ptr cinfo )
{
	p_file_src_ptr src = (p_file_src_ptr)cinfo->src;
	size_t nbytes = fread( src->buffer, 1, INPUT_BUF_SIZE, src->in );

	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			throw PInternalError( "JPEG: empty file" );
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

static void skip_file_input_data( j_decompress_ptr cinfo, long num_bytes )
{
	p_file_src_ptr src = (p_file_src_ptr)cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			fill_file_input_buffer(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

static void term_file_source( j_decompress_ptr cinfo )
{
	/* no work necessary here */
}

static void jpeg_pfilesrc( j_decompress_ptr cinfo, FILE* in )
{
	p_file_src_ptr src;

	/* The source object and input buffer are made permanent so that a series
	* of JPEG images can be read from the same file by calling jpeg_stdio_src
	* only before the first one.  (If we discarded the buffer at the end of
	* one image, we'd likely lose the start of the next one.)
	* This makes it unsafe to use this manager and a different source
	* manager serially with the same JPEG object.  Caveat programmer.
	*/
	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			SIZEOF(p_file_source_mgr));
		src = (p_file_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * SIZEOF(JOCTET));
	}

	src = (p_file_src_ptr) cinfo->src;
	src->pub.init_source = init_file_source;
	src->pub.fill_input_buffer = fill_file_input_buffer;
	src->pub.skip_input_data = skip_file_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_file_source;
	src->in = in;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

//*****************************************************************************

typedef struct {
	struct jpeg_source_mgr pub;

	const BYTE* ptr;
	size_t sz;
	JOCTET * buffer;
	boolean start_of_file;
} p_byte_source_mgr;
typedef p_byte_source_mgr* p_byte_src_ptr;

static void init_byte_source( j_decompress_ptr cinfo )
{
	p_byte_src_ptr src = (p_byte_src_ptr)cinfo->src;
	src->start_of_file = TRUE;
}

static boolean fill_byte_input_buffer( j_decompress_ptr cinfo )
{
	p_byte_src_ptr src = (p_byte_src_ptr)cinfo->src;
	size_t nbytes = src->sz;
	if( nbytes > INPUT_BUF_SIZE )
		nbytes = INPUT_BUF_SIZE;
	memcpy( src->buffer, src->ptr, nbytes );
	src->ptr += nbytes;
	src->sz -= nbytes;

	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			throw PInternalError( "JPEG: empty file" );
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

static void skip_byte_input_data( j_decompress_ptr cinfo, long num_bytes )
{
	p_byte_src_ptr src = (p_byte_src_ptr)cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			fill_byte_input_buffer(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

static void term_byte_source( j_decompress_ptr cinfo )
{
	/* no work necessary here */
}

static void jpeg_pbytesrc( j_decompress_ptr cinfo, const BYTE* ptr, size_t sz )
{
	p_byte_src_ptr src;

	/* The source object and input buffer are made permanent so that a series
	* of JPEG images can be read from the same file by calling jpeg_stdio_src
	* only before the first one.  (If we discarded the buffer at the end of
	* one image, we'd likely lose the start of the next one.)
	* This makes it unsafe to use this manager and a different source
	* manager serially with the same JPEG object.  Caveat programmer.
	*/
	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			SIZEOF(p_byte_source_mgr));
		src = (p_byte_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * SIZEOF(JOCTET));
	}

	src = (p_byte_src_ptr) cinfo->src;
	src->pub.init_source = init_byte_source;
	src->pub.fill_input_buffer = fill_byte_input_buffer;
	src->pub.skip_input_data = skip_byte_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = term_byte_source;
	src->ptr = ptr;
	src->sz = sz;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

//*****************************************************************************

struct _PJpegImageSourceImpl
{
public:
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

public:
	_PJpegImageSourceImpl( FILE* f, int scale );
	_PJpegImageSourceImpl( const BYTE* ptr, size_t sz, int scale );
	~_PJpegImageSourceImpl();
};

_PJpegImageSourceImpl::_PJpegImageSourceImpl( FILE* f, int scale )
{
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &cinfo );
	jpeg_pfilesrc( &cinfo, f );

	jpeg_read_header( &cinfo, TRUE );
	cinfo.scale_denom = scale;

	cinfo.out_color_space = JCS_RGB;
	jpeg_start_decompress( &cinfo );
}

_PJpegImageSourceImpl::_PJpegImageSourceImpl( const BYTE* ptr, size_t sz, int scale )
{
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &cinfo );
	jpeg_pbytesrc( &cinfo, ptr, sz );

	jpeg_read_header( &cinfo, TRUE );
	cinfo.scale_denom = scale;

	cinfo.out_color_space = JCS_RGB;
	jpeg_start_decompress( &cinfo );
}

_PJpegImageSourceImpl::~_PJpegImageSourceImpl()
{
	try
	{
		cinfo.output_scanline = cinfo.output_height;
		jpeg_finish_decompress( &cinfo );
	}
	catch( PError& )
	{
	}
	jpeg_destroy_decompress( &cinfo );
}

PJpegImageSource::PJpegImageSource( const BYTE* ptr, size_t sz, int scale )
{
	_impl = new _PJpegImageSourceImpl( ptr, sz, scale );
}

PJpegImageSource::PJpegImageSource( FILE* f, int scale )
{
	_impl = new _PJpegImageSourceImpl( f, scale );
}

int PJpegImageSource::width() const
{
	_PJpegImageSourceImpl* impl = (_PJpegImageSourceImpl*)_impl;
	return impl->cinfo.output_width;
}

int PJpegImageSource::height() const
{
	_PJpegImageSourceImpl* impl = (_PJpegImageSourceImpl*)_impl;
	return impl->cinfo.output_height;
}

void PJpegImageSource::nextRow( PImage::RGB* rgb )
{
	_PJpegImageSourceImpl* impl = (_PJpegImageSourceImpl*)_impl;
	BYTE* buf = (BYTE*)rgb;//! hack - will work only as long as PImage::RGB is { BYTE r; BYTE g; BYTE b; } 
	jpeg_read_scanlines( &impl->cinfo, &buf, 1 );
}

PJpegImageSource::~PJpegImageSource()
{
	_PJpegImageSourceImpl* impl = (_PJpegImageSourceImpl*)_impl;
	delete impl;
}

/* static */ bool PJpegImageSource::isJpegFile( FILE* f )
{
	int c = fgetc( f );
	if( c != 0xFF )
		return false;
	c = fgetc( f );
	if( c != 0xD8 )
		return false;
	return true;
}

/* static */ bool PJpegImageSource::isJpegFile( const BYTE* p, size_t sz )
{
	return sz >= 2 && p[ 0 ] == 0xFF && p[ 1 ] == 0xD8;
}
