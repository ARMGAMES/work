/* gzio2.cpp -- clone of gzio.c -- in-memory IO on .gz files
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Compile this file with -DNO_DEFLATE to avoid the compression code.
 */

/* @(#) $Id: gzio2.cpp,v 1.2 2003/07/28 16:31:42 ipsign Exp $ */

#include <stdio.h>

#include "zutil.h"
#include "gzlib2.h"
#include "pplogfile.h"

struct internal_state {int dummy;}; /* for buggy compilers */

#ifndef Z_BUFSIZE
#  ifdef MAXSEG_64K
#    define Z_BUFSIZE 4096 /* minimize memory usage for 16-bit DOS */
#  else
#    define Z_BUFSIZE 16384
#  endif
#endif
#ifndef Z_PRINTF_BUFSIZE
#  define Z_PRINTF_BUFSIZE 4096
#endif

#define ALLOC(size) malloc(size)
#define TRYFREE(p) {if (p) free(p);}

static int gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

typedef struct gz_stream {
    z_stream stream;
    int      z_err;   /* error code for last stream operation */
    int      z_eof;   /* set if end of input file */
//    FILE     *file;   /* .gz file */
	POutputStream* outFile;
	PInputStream* inFile;
    Byte     *inbuf;  /* input buffer */
    Byte     *outbuf; /* output buffer */
    uLong    crc;     /* crc32 of uncompressed data */
    char     *msg;    /* error message */
//    char     *path;   /* path name for debugging only */
    int      transparent; /* 1 if input file is not a .gz file */
    char     mode;    /* 'w' or 'r' */
//    long     startpos; /* start of compressed data in file (header skipped) */
} gz_stream;


local gzFile gz2_open      OF((POutputStream* outStream, PInputStream* inStream, const char* mode));
local int do_flush        OF((gzFile file, int flush));
local int    get_byte     OF((gz_stream *s));
local void   check_header OF((gz_stream *s));
local int    destroy      OF((gz_stream *s));
local void   putLong      OF((POutputStream* stream, uLong x));
local uLong  getLong      OF((gz_stream *s));

/* ===========================================================================
     Initializes gzip stream for reading or writing. The mode parameter
   is as in fopen ("rb" or "wb"). gz2_open return NULL if there was
   insufficient memory to allocate the (de)compression state; 
*/
local gzFile gz2_open (POutputStream* outStream, PInputStream* inStream, const char* mode)
{
    int err;
    int level = Z_DEFAULT_COMPRESSION; /* compression level */
    int strategy = Z_DEFAULT_STRATEGY; /* compression strategy */
    char *p = (char*)mode;
    gz_stream *s;
    char fmode[80]; /* copy of mode, without the compression level */
    char *m = fmode;

//    if (!path || !mode) return Z_NULL;
	if( !outStream && !inStream ) return Z_NULL;

    s = (gz_stream *)ALLOC(sizeof(gz_stream));
    if (!s) return Z_NULL;

    s->stream.zalloc = (alloc_func)0;
    s->stream.zfree = (free_func)0;
    s->stream.opaque = (voidpf)0;
    s->stream.next_in = s->inbuf = Z_NULL;
    s->stream.next_out = s->outbuf = Z_NULL;
    s->stream.avail_in = s->stream.avail_out = 0;
    s->outFile = NULL;
    s->inFile = NULL;
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, Z_NULL, 0);
    s->msg = NULL;
    s->transparent = 0;

//    s->path = (char*)ALLOC(strlen(path)+1);
//    if (s->path == NULL) {
//        return destroy(s), (gzFile)Z_NULL;
//    }
//    strcpy(s->path, path); /* do this early for debugging */

    s->mode = '\0';
    do {
        if (*p == 'r') s->mode = 'r';
        if (*p == 'w' || *p == 'a') s->mode = 'w';
        if (*p >= '0' && *p <= '9') {
	    level = *p - '0';
	} else if (*p == 'f') {
	  strategy = Z_FILTERED;
	} else if (*p == 'h') {
	  strategy = Z_HUFFMAN_ONLY;
	} else {
	    *m++ = *p; /* copy the mode */
	}
    } while (*p++ && m != fmode + sizeof(fmode));
    if (s->mode == '\0') return destroy(s), (gzFile)Z_NULL;

    if (s->mode == 'w') {
		if( !outStream ) return destroy(s), (gzFile)Z_NULL;
#ifdef NO_DEFLATE
        err = Z_STREAM_ERROR;
#else
        err = deflateInit2(&(s->stream), level,
                           Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);
        /* windowBits is passed < 0 to suppress zlib header */

        s->stream.next_out = s->outbuf = (Byte*)ALLOC(Z_BUFSIZE);
#endif
        if (err != Z_OK || s->outbuf == Z_NULL) {
            return destroy(s), (gzFile)Z_NULL;
        }
    } else {
		if( !inStream ) return destroy(s), (gzFile)Z_NULL;
        s->stream.next_in  = s->inbuf = (Byte*)ALLOC(Z_BUFSIZE);

        err = inflateInit2(&(s->stream), -MAX_WBITS);
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
         * present after the compressed stream.
         */
        if (err != Z_OK || s->inbuf == Z_NULL) {
            return destroy(s), (gzFile)Z_NULL;
        }
    }
    s->stream.avail_out = Z_BUFSIZE;

    errno = 0;
//	s->file = fd < 0 ? F_OPEN(path, fmode) : (FILE*)fdopen(fd, fmode);
//    if (s->file == NULL) {
//        return destroy(s), (gzFile)Z_NULL;
//    }
	s->outFile = outStream;
	s->inFile = inStream;

    if (s->mode == 'w') {
        /* Write a very simple .gz header:
         */
        //fprintf(s->file, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1],
        //     Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE);
		char hdr[ 16 ];
        sprintf(hdr, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1],
             Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE);
		try
		{
		s->outFile->write( (Byte*)hdr, 10 );
		}
		catch( PError& x )
		{
		PLog( "GZIP: error writing to stream: %s", x.why() );
		s->z_err = Z_ERRNO;
		}

//---	s->startpos = 10L;
	/* We use 10L instead of ftell(s->file) to because ftell causes an
         * fflush on some systems. This version of the library doesn't use
         * startpos anyway in write mode, so this initialization is not
         * necessary.
         */
    } else {
	check_header(s); /* skip the .gz header */
//---	s->startpos = (ftell(s->file) - s->stream.avail_in);
    }
    
    return (gzFile)s;
}

/* ===========================================================================
     Opens a gzip (.gz) file for reading or writing.
*/
gzFile ZEXPORT gz2open (POutputStream* outStream, PInputStream* inStream, const char* mode)
{
    return gz2_open (outStream, inStream, mode);
}

/* ===========================================================================
 * Update the compression level and strategy
 */
int ZEXPORT gz2setparams (gzFile file, int level, int strategy)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    /* Make room to allow flushing */
    if (s->stream.avail_out == 0) {

	s->stream.next_out = s->outbuf;
	try
	{
	s->outFile->write(s->outbuf, Z_BUFSIZE);
	}
	catch( PError& x )
	{
		PLog( "GZIP: error writing to stream: %s", x.why() );
	    s->z_err = Z_ERRNO;
	}
	s->stream.avail_out = Z_BUFSIZE;
    }

    return deflateParams (&(s->stream), level, strategy);
}

/* ===========================================================================
     Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/
local int get_byte(gz_stream *s)
{
    if (s->z_eof) return EOF;
    if (s->stream.avail_in == 0) {
	errno = 0;
	try
	{
	s->stream.avail_in = s->inFile->read(s->inbuf, Z_BUFSIZE);
	}
	catch( PError& x )
	{
	PLog( "GZIP: error reading from stream: %s", x.why() );
	s->z_err = Z_ERRNO;
    return EOF;
	}
	if (s->stream.avail_in == 0) {
	    s->z_eof = 1;
//	    if (s->inFile->error()) s->z_err = Z_ERRNO;
	    return EOF;
	}
	s->stream.next_in = s->inbuf;
    }
    s->stream.avail_in--;
    return *(s->stream.next_in)++;
}

/* ===========================================================================
      Check the gzip header of a gz_stream opened for reading. Set the stream
    mode to transparent if the gzip magic header is not present; set s->err
    to Z_DATA_ERROR if the magic header is present but the rest of the header
    is incorrect.
    IN assertion: the stream s has already been created sucessfully;
       s->stream.avail_in is zero for the first time, but may be non-zero
       for concatenated .gz files.
*/
local void check_header(gz_stream *s)
{
    int method; /* method byte */
    int flags;  /* flags byte */
    uInt len;
    int c;

    /* Check the gzip magic header */
    for (len = 0; len < 2; len++) {
	c = get_byte(s);
	if (c != gz_magic[len]) {
	    if (len != 0) s->stream.avail_in++, s->stream.next_in--;
	    if (c != EOF) {
		s->stream.avail_in++, s->stream.next_in--;
		s->transparent = 1;
	    }
	    s->z_err = s->stream.avail_in != 0 ? Z_OK : Z_STREAM_END;
	    return;
	}
    }
    method = get_byte(s);
    flags = get_byte(s);
    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
	s->z_err = Z_DATA_ERROR;
	return;
    }

    /* Discard time, xflags and OS code: */
    for (len = 0; len < 6; len++) (void)get_byte(s);

    if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
	len  =  (uInt)get_byte(s);
	len += ((uInt)get_byte(s))<<8;
	/* len is garbage if EOF but the loop below will quit anyway */
	while (len-- != 0 && get_byte(s) != EOF) ;
    }
    if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
	while ((c = get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
	while ((c = get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
	for (len = 0; len < 2; len++) (void)get_byte(s);
    }
    s->z_err = s->z_eof ? Z_DATA_ERROR : Z_OK;
}

 /* ===========================================================================
 * Cleanup then free the given gz_stream. Return a zlib error code.
   Try freeing in the reverse order of allocations.
 */
local int destroy (gz_stream *s)
{
    int err = Z_OK;

    if (!s) return Z_STREAM_ERROR;

    TRYFREE(s->msg);

    if (s->stream.state != NULL) {
	if (s->mode == 'w') {
#ifdef NO_DEFLATE
	    err = Z_STREAM_ERROR;
#else
	    err = deflateEnd(&(s->stream));
#endif
	} else if (s->mode == 'r') {
	    err = inflateEnd(&(s->stream));
	}
    }
	/*
    if (s->file != NULL && fclose(s->file)) {
#ifdef ESPIPE
	if (errno != ESPIPE) // fclose is broken for pipes in HP/UX 
#endif
	    err = Z_ERRNO;
    }*/
    if (s->z_err < 0) err = s->z_err;

    TRYFREE(s->inbuf);
    TRYFREE(s->outbuf);
//    TRYFREE(s->path);
    TRYFREE(s);
    return err;
}

/* ===========================================================================
     Reads the given number of uncompressed bytes from the compressed file.
   gzread returns the number of bytes actually read (0 for end of file).
*/
int ZEXPORT gz2read (gzFile file, voidp buf, unsigned len)
{
    gz_stream *s = (gz_stream*)file;
    Bytef *start = (Bytef*)buf; /* starting point for crc computation */
    Byte  *next_out; /* == stream.next_out but not forced far (for MSDOS) */

    if (s == NULL || s->mode != 'r') return Z_STREAM_ERROR;

    if (s->z_err == Z_DATA_ERROR || s->z_err == Z_ERRNO) return -1;
    if (s->z_err == Z_STREAM_END) return 0;  /* EOF */

    next_out = (Byte*)buf;
    s->stream.next_out = (Bytef*)buf;
    s->stream.avail_out = len;

    while (s->stream.avail_out != 0) {

	if (s->transparent) {
	    /* Copy first the lookahead bytes: */
	    uInt n = s->stream.avail_in;
	    if (n > s->stream.avail_out) n = s->stream.avail_out;
	    if (n > 0) {
		zmemcpy(s->stream.next_out, s->stream.next_in, n);
		next_out += n;
		s->stream.next_out = next_out;
		s->stream.next_in   += n;
		s->stream.avail_out -= n;
		s->stream.avail_in  -= n;
	    }
	    if (s->stream.avail_out > 0) {
		try
		{
		s->stream.avail_out -= s->inFile->read(next_out, s->stream.avail_out);
		}
		catch( PError& x )
		{
		PLog( "GZIP: error reading from stream: %s", x.why() );
		//do nothing, s->stream.avail_out remains unchanged
		}
	    }
	    len -= s->stream.avail_out;
	    s->stream.total_in  += (uLong)len;
	    s->stream.total_out += (uLong)len;
            if (len == 0) s->z_eof = 1;
	    return (int)len;
	}
        if (s->stream.avail_in == 0 && !s->z_eof) {

            errno = 0;
			try
			{
            s->stream.avail_in = s->inFile->read(s->inbuf, Z_BUFSIZE );
			}
			catch( PError& x )
			{
			PLog( "GZIP: error reading from stream: %s", x.why() );
			s->z_err = Z_ERRNO;
			break;
			}
            if (s->stream.avail_in == 0) {
                s->z_eof = 1;
//		if (s->inFile->error()) {
//		    s->z_err = Z_ERRNO;
//		    break;
//		}
            }
            s->stream.next_in = s->inbuf;
        }
        s->z_err = inflate(&(s->stream), Z_NO_FLUSH);

	if (s->z_err == Z_STREAM_END) {
	    /* Check CRC and original size */
	    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));
	    start = s->stream.next_out;

	    if (getLong(s) != s->crc) {
		s->z_err = Z_DATA_ERROR;
	    } else {
	        (void)getLong(s);
                /* The uncompressed length returned by above getlong() may
                 * be different from s->stream.total_out) in case of
		 * concatenated .gz files. Check for such files:
		 */
		check_header(s);
		if (s->z_err == Z_OK) {
		    uLong total_in = s->stream.total_in;
		    uLong total_out = s->stream.total_out;

		    inflateReset(&(s->stream));
		    s->stream.total_in = total_in;
		    s->stream.total_out = total_out;
		    s->crc = crc32(0L, Z_NULL, 0);
		}
	    }
	}
	if (s->z_err != Z_OK || s->z_eof) break;
    }
    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));

    return (int)(len - s->stream.avail_out);
}


/* ===========================================================================
      Reads one byte from the compressed file. gzgetc returns this byte
   or -1 in case of end of file or error.
*/
int ZEXPORT gz2getc(gzFile file)
{
    unsigned char c;

    return gz2read(file, &c, 1) == 1 ? c : -1;
}


/* ===========================================================================
      Reads bytes from the compressed file until len-1 characters are
   read, or a newline character is read and transferred to buf, or an
   end-of-file condition is encountered.  The string is then terminated
   with a null character.
      gzgets returns buf, or Z_NULL in case of error.

      The current implementation is not optimized at all.
*/
char * ZEXPORT gz2gets(gzFile file, char *buf, int len)
{
    char *b = buf;
    if (buf == Z_NULL || len <= 0) return Z_NULL;

    while (--len > 0 && gz2read(file, buf, 1) == 1 && *buf++ != '\n') ;
    *buf = '\0';
    return b == buf && len > 0 ? Z_NULL : b;
}


#ifndef NO_DEFLATE
/* ===========================================================================
     Writes the given number of uncompressed bytes into the compressed file.
   gzwrite returns the number of bytes actually written (0 in case of error).
*/
int ZEXPORT gz2write (gzFile file, const voidp buf, unsigned len)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.next_in = (Bytef*)buf;
    s->stream.avail_in = len;

    while (s->stream.avail_in != 0) {

        if (s->stream.avail_out == 0) {

            s->stream.next_out = s->outbuf;
			try
			{
            s->outFile->write(s->outbuf, Z_BUFSIZE);
			}
			catch( PError& x )
			{
				PLog( "GZIP: error writing to stream: %s", x.why() );
                s->z_err = Z_ERRNO;
                break;
            }
            s->stream.avail_out = Z_BUFSIZE;
        }
        s->z_err = deflate(&(s->stream), Z_NO_FLUSH);
        if (s->z_err != Z_OK) break;
    }
    s->crc = crc32(s->crc, (const Bytef *)buf, len);

    return (int)(len - s->stream.avail_in);
}

/* ===========================================================================
     Converts, formats, and writes the args to the compressed file under
   control of the format string, as in fprintf. gzprintf returns the number of
   uncompressed bytes actually written (0 in case of error).
*/
#ifdef STDC
#include <stdarg.h>

int ZEXPORTVA gz2printf (gzFile file, const char *format, /* args */ ...)
{
    char buf[Z_PRINTF_BUFSIZE];
    va_list va;
    int len;

    va_start(va, format);
#ifdef HAS_vsnprintf
    (void)vsnprintf(buf, sizeof(buf), format, va);
#else
    (void)vsprintf(buf, format, va);
#endif
    va_end(va);
    len = strlen(buf); /* some *sprintf don't return the nb of bytes written */
    if (len <= 0) return 0;

    return gz2write(file, buf, (unsigned)len);
}
#else /* not ANSI C */

int ZEXPORTVA gzprintf (file, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
	               a11, a12, a13, a14, a15, a16, a17, a18, a19, a20)
    gzFile file;
    const char *format;
    int a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
	a11, a12, a13, a14, a15, a16, a17, a18, a19, a20;
{
    char buf[Z_PRINTF_BUFSIZE];
    int len;

#ifdef HAS_snprintf
    snprintf(buf, sizeof(buf), format, a1, a2, a3, a4, a5, a6, a7, a8,
	     a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
#else
    sprintf(buf, format, a1, a2, a3, a4, a5, a6, a7, a8,
	    a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
#endif
    len = strlen(buf); /* old sprintf doesn't return the nb of bytes written */
    if (len <= 0) return 0;

    return gz2write(file, buf, len);
}
#endif

/* ===========================================================================
      Writes c, converted to an unsigned char, into the compressed file.
   gzputc returns the value that was written, or -1 in case of error.
*/
int ZEXPORT gz2putc(gzFile file, int c)
{
    unsigned char cc = (unsigned char) c; /* required for big endian systems */

    return gz2write(file, &cc, 1) == 1 ? (int)cc : -1;
}


/* ===========================================================================
      Writes the given null-terminated string to the compressed file, excluding
   the terminating null character.
      gzputs returns the number of characters written, or -1 in case of error.
*/
int ZEXPORT gz2puts(gzFile file, const char *s)
{
    return gz2write(file, (char*)s, (unsigned)strlen(s));
}


/* ===========================================================================
     Flushes all pending output into the compressed file. The parameter
   flush is as in the deflate() function.
*/
local int do_flush (gzFile file, int flush)
{
    uInt len;
    int done = 0;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.avail_in = 0; /* should be zero already anyway */

    for (;;) {
        len = Z_BUFSIZE - s->stream.avail_out;

        if (len != 0) {
			try
			{
            s->outFile->write(s->outbuf, len);
			}
			catch( PError& x )
			{
			PLog( "GZIP: error writing to stream: %s", x.why() );
            s->z_err = Z_ERRNO;
            return Z_ERRNO;
			}
            s->stream.next_out = s->outbuf;
            s->stream.avail_out = Z_BUFSIZE;
        }
        if (done) break;
        s->z_err = deflate(&(s->stream), flush);

	/* Ignore the second of two consecutive flushes: */
	if (len == 0 && s->z_err == Z_BUF_ERROR) s->z_err = Z_OK;

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer: 
         */
        done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);
 
        if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
    }
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}

int ZEXPORT gz2flush (gzFile file, int flush)
{
    gz_stream *s = (gz_stream*)file;
    int err = do_flush (file, flush);

    if (err) return err;
//---    fflush(s->file);
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}
#endif /* NO_DEFLATE */

/* ===========================================================================
     Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero.
*/
int ZEXPORT gz2eof (gzFile file)
{
    gz_stream *s = (gz_stream*)file;
    
    return (s == NULL || s->mode != 'r') ? 0 : s->z_eof;
}

/* ===========================================================================
   Outputs a long in LSB order to the given file
*/
local void putLong (POutputStream* stream, uLong x)
{
    int n;
    for (n = 0; n < 4; n++) {
        //fputc((int)(x & 0xff), file);
		Byte xx = (Byte)( x & 0xff);
		stream->write( &xx, 1 );
        x >>= 8;
    }
}

/* ===========================================================================
   Reads a long in LSB order from the given gz_stream. Sets z_err in case
   of error.
*/
local uLong getLong (gz_stream *s)
{
    uLong x = (uLong)get_byte(s);
    int c;

    x += ((uLong)get_byte(s))<<8;
    x += ((uLong)get_byte(s))<<16;
    c = get_byte(s);
    if (c == EOF) s->z_err = Z_DATA_ERROR;
    x += ((uLong)c)<<24;
    return x;
}

/* ===========================================================================
     Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (de)compression state.
*/
int ZEXPORT gz2close (gzFile file)
{
    int err;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL) return Z_STREAM_ERROR;

    if (s->mode == 'w') {
#ifdef NO_DEFLATE
	return Z_STREAM_ERROR;
#else
        err = do_flush (file, Z_FINISH);
        if (err != Z_OK) return destroy((gz_stream*)file);

        putLong (s->outFile, s->crc);
        putLong (s->outFile, s->stream.total_in);
#endif
    }
    return destroy((gz_stream*)file);
}

/* ===========================================================================
     Returns the error message for the last error which occured on the
   given compressed file. errnum is set to zlib error number. If an
   error occured in the file system and not in the compression library,
   errnum is set to Z_ERRNO and the application may consult errno
   to get the exact error code.
*/
const char*  ZEXPORT gz2error (gzFile file, int *errnum)
{
    char *m;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL) {
        *errnum = Z_STREAM_ERROR;
        return (const char*)ERR_MSG(Z_STREAM_ERROR);
    }
    *errnum = s->z_err;
    if (*errnum == Z_OK) return (const char*)"";

    m =  (char*)(*errnum == Z_ERRNO ? zstrerror(errno) : s->stream.msg);

    if (m == NULL || *m == '\0') m = (char*)ERR_MSG(s->z_err);

    TRYFREE(s->msg);
    s->msg = (char*)ALLOC(/*--- strlen(s->path) + */strlen(m) + 3);
    //strcpy(s->msg, s->path);
    strcat(s->msg, ": ");
    strcat(s->msg, m);
    return (const char*)s->msg;
}
