#include "ppincludebase.h"//_before_ openssl headers

//****************************************************************************


//****************************************************************************

#include "ppinclude.h"//_after_ openssl headers - to avoid name pseudo-collisions

#include "pperror.h"
#include "ppthread.h"
#include "commssl.h"

static void _base64dec( PBlock& ret, const char* intext, const char* intextEnd )
{
	unsigned char ibuf[ 4 ];
	unsigned char obuf[ 3 ];
	bool endtext = false;
	int lindex = 0;

	memset( ibuf, 0, sizeof(ibuf) );

	while( intext < intextEnd )
	{
		bool skip = false;
		char ch = *intext;
		PASSERT3( ch );

		if( (ch >= 'A') && (ch <= 'Z') )
			ch = ch - 'A';
		else if( (ch >= 'a') && (ch <= 'z') )
			ch = ch - 'a' + 26;
		else if( (ch >= '0') && (ch <= '9') )
			ch = ch - '0' + 52;
		else if( ch == '+' || ch == '-' )
			ch = 62;
		else if( ch == '=' )/* end of text */
		{   
			if( endtext )
				break;
			endtext = true;
			lindex--;
			if( lindex < 0 )
				lindex = 3;
		}
		else if( ch == '/' || ch == '_' )
			ch = 63;
		else if( endtext )
			break;
		else
            skip = true;

		if( !skip )
		{
			if( !endtext )
			{
				ibuf[ lindex ] = ch;

				lindex++;
				lindex &= 3;    /* use bit arithmetic instead of remainder */
			}

			if( (lindex == 0) || endtext )
			{
				obuf[0] = (ibuf[0] << 2) | ((ibuf[1] & 0x30) >> 4);
				obuf[1] =
					((ibuf[1] & 0x0F) << 4) | ((ibuf[2] & 0x3C) >> 2);
				obuf[2] = ((ibuf[2] & 0x03) << 6) | (ibuf[3] & 0x3F);

				switch( lindex )
				{
				case 1:
					*ret.append( 1 ) = obuf[ 0 ];
					break;
				case 2:
					{
						BYTE* p = ret.append( 2 );
						*p++ = obuf[ 0 ];
						*p = obuf[ 1 ];
					}
					break;
				default:
					{
						BYTE* p = ret.append( 3 );
						*p++ = obuf[ 0 ];
						*p++ = obuf[ 1 ];
						*p = obuf[ 2 ];
					}
					break;
				}
				memset( ibuf, 0, sizeof(ibuf) );
			}
		}
		intext++;
	}
}

static void _base64decFast(PBlock& ret, const char* in, size_t sz)
{
    constexpr static uint8_t map[256] = {
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255, 62,  255, 63,
      52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255, 0,   255, 255, 255,
      255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  255, 255, 255, 255, 63,
      255, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
      41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
    };

    size_t const output_size = 3 * sz / 4;
    ret.ensure(output_size);
    BYTE* ptr = ret.ptr();
    BYTE* const ptr0 = ptr;

    const char* const in_end = in + sz;

#define BASE64DECODE_FOUR_BYTES                                         \
  u32 = static_cast<uint32_t>(map[static_cast<uint8_t>(*in++)]) << 18;  \
  u32 += static_cast<uint32_t>(map[static_cast<uint8_t>(*in++)]) << 12; \
  u32 += static_cast<uint32_t>(map[static_cast<uint8_t>(*in++)]) << 6;  \
  u32 += static_cast<uint32_t>(map[static_cast<uint8_t>(*in++)]);       \
  *ptr++ = static_cast<char>((u32 >> 16) & 0xFF);                       \
  *ptr++ = static_cast<char>((u32 >> 8) & 0xFF);                        \
  *ptr++ = static_cast<char>(u32 & 0xFF)

    uint32_t u32;
    size_t total_fourbyte_blocks = (sz >> 2);
    if (total_fourbyte_blocks) {
        --total_fourbyte_blocks;  // Can have pad chars at the end.
    }
    size_t total_32bytes_blocks = total_fourbyte_blocks / 8;
    total_fourbyte_blocks -= total_32bytes_blocks * 8;
    while (total_32bytes_blocks--) {
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
        BASE64DECODE_FOUR_BYTES;
    }
    while (total_fourbyte_blocks--) {
        BASE64DECODE_FOUR_BYTES;
    }
#undef BASE64DECODE_FOUR_BYTES

    uint16_t acc = 0u;
    uint8_t nbits = 0u;
    while (in != in_end) {
        const char c = *in++;
        if (c == '=') {
            ret.cut(ptr - ptr0);
            return;
        }
        acc = (acc << 6) + map[static_cast<uint8_t>(c)];
        nbits += 6;
        if (nbits >= 8) {
            nbits -= 8;
            *ptr++ = static_cast<char>((acc >> nbits) & 0xFF);
        }
    }
    ret.cut(ptr - ptr0);
}

static void _base64enc( PString& out, const BYTE* p, size_t sz, int lineLen /* MUST be a multiple of 4, 0 means 'unlimited' */, bool urlSafe )
{
	static const char _dtableUrlNotSafe[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const char _dtableUrlSafe[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
	const char * _dtable = urlSafe ? _dtableUrlSafe : _dtableUrlNotSafe;
	int nLine = 0;
	for(;;)
	{
		BYTE igroup[ 3 ];
		bool end = false;
		int n = 3;
		if( n > sz )
		{
			n = sz;
			end = true;
		}
		if( n > 0 )
			memcpy( igroup, p, n );
		if( n < 3 )
			memset( igroup + n, 0, 3 - n );
		p += n;
		sz -= n;

		char ogroup[ 4 ];
		if( n > 0 )
		{
			ogroup[0] = _dtable[ igroup[0] >> 2 ];
			ogroup[1] = _dtable[ ((igroup[0] & 3) << 4) | (igroup[1] >> 4) ];
			ogroup[2] = n >= 2 ? _dtable[ ((igroup[1] & 0xF) << 2) | (igroup[2] >> 6) ] : '=';
			ogroup[3] = n >= 3 ? _dtable[ igroup[2] & 0x3F ] : '=';

			out.append( ogroup, ogroup + 4 );
			if( sz > 0 && lineLen && ( nLine += 4 ) >= lineLen )
			{
				out.append( "\r\n" );
				nLine = 0;
			}
		}
		if( end )
			break;
	}
}

static void _base64encFastIntoBlock(PBlock& out, const BYTE* input, size_t input_size)
{
    constexpr static char map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const size_t result_size = 4 * (input_size / 3) + ((input_size % 3) ? 4 : 0);
    out.ensure(result_size);
    char* ptr = reinterpret_cast<char*>(out.ptr());
    char* const end = ptr + result_size;
    out.cut(result_size);

    uint8_t const* in = reinterpret_cast<uint8_t const*>(input);  // NOTE(dimak): This type must be unsigned.
    uint8_t const* const in_end = in + input_size;

#if 0
#define DO_BASE64ENCODE_THREE_BYTES \
  ptr[3] = map[u32 & 0x3f];         \
  u32 >>= 6;                        \
  ptr[2] = map[u32 & 0x3f];         \
  u32 >>= 6;                        \
  ptr[1] = map[u32 & 0x3f];         \
  u32 >>= 6;                        \
  ptr[0] = map[u32 & 0x3f];
#else
#define DO_BASE64ENCODE_THREE_BYTES \
  ptr[0] = map[(u32 >> 18) & 0x3f]; \
  ptr[1] = map[(u32 >> 12) & 0x3f]; \
  ptr[2] = map[(u32 >> 6) & 0x3f];  \
  ptr[3] = map[u32 & 0x3f];
#endif

#define BASE64ENCODE_THREE_BYTES            \
  u32 = static_cast<uint32_t>(*in++) << 16; \
  u32 += static_cast<uint32_t>(*in++) << 8; \
  u32 += static_cast<uint32_t>(*in++);      \
  DO_BASE64ENCODE_THREE_BYTES               \
  ptr += 4

    uint32_t u32;
    size_t total_threebyte_blocks = input_size / 3;
    size_t total_30bytes_blocks = total_threebyte_blocks / 10;
    total_threebyte_blocks -= total_30bytes_blocks * 10;
    while (total_30bytes_blocks--) {
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
        BASE64ENCODE_THREE_BYTES;
    }
    while (total_threebyte_blocks--) {
        BASE64ENCODE_THREE_BYTES;
    }
#undef BASE64ENCODE_THREE_BYTES
#undef DO_BASE64ENCODE_THREE_BYTES

    uint16_t acc = 0u;
    uint8_t nbits = 0;
    while (in != in_end) {
        acc = (acc << 8) + (*in++);
        nbits += 8;
        while (nbits >= 6) {
            nbits -= 6;
            *ptr++ = map[(acc >> nbits) & 0x3F];
        }
    }

    if (nbits > 0) {
        *ptr++ = map[((acc << 6) >> nbits) & 0x3F];
    }

    while (ptr != end) {
        *ptr++ = '=';
    }
}

static void _base64enc( char * out, size_t out_sz, const BYTE* p, size_t sz, bool urlSafe )
{
	static const char _dtableUrlNotSafe[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const char _dtableUrlSafe[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
	const char * _dtable = urlSafe ? _dtableUrlSafe : _dtableUrlNotSafe;
	for(;;)
	{
		BYTE igroup[ 3 ];
		bool end = false;
		int n = 3;
		if( n > sz )
		{
			n = sz;
			end = true;
		}
		if( n > 0 )
			memcpy( igroup, p, n );
		if( n < 3 )
			memset( igroup + n, 0, 3 - n );
		p += n;
		sz -= n;

		if( n > 0 )
		{
			PASSERT3(out_sz >= 4);
			out_sz -= 4;
			out[0] = _dtable[ igroup[0] >> 2 ];
			out[1] = _dtable[ ((igroup[0] & 3) << 4) | (igroup[1] >> 4) ];
			out[2] = n >= 2 ? _dtable[ ((igroup[1] & 0xF) << 2) | (igroup[2] >> 6) ] : '=';
			out[3] = n >= 3 ? _dtable[ igroup[2] & 0x3F ] : '=';
			out+=4;
		}
		if( end )
		{
			out[0]=0;
			break;
		}
	}
}

/* static */ void CommSSLBase64::encode( PString& ret, const BYTE* p, size_t sz, int lineLen, bool urlSafe )
{
	if( !sz )
		return;
	size_t encSize = ( ( sz + 2 ) / 3 ) * 4;
	if( lineLen )
		encSize += ( (encSize - 1)/lineLen ) * 2;
	ret.reserve( ret.length() + encSize );
	_base64enc( ret, p, sz, lineLen, urlSafe );
}

/* static */ void CommSSLBase64::encode( char * ret, size_t retSize, const BYTE* p, size_t sz, bool urlSafe )
{
	_base64enc( ret, retSize, p, sz, urlSafe );
}

/* static */ void CommSSLBase64::encodeOverwritingIntoBlock(PBlock& ret, const BYTE* p, size_t sz)
{
    if (!sz)
    {
        ret.cut(0);
        return;
    }
    _base64encFastIntoBlock(ret, p, sz);
}

/* static */ void CommSSLBase64::decode( PBlock& ret, const char* s, size_t sz )
{
	if( sz == (size_t)(-1) )
		sz = strlen( s );
	ret.reserve( ret.size() + sz );
	_base64dec( ret, s, s+sz );
}

/* static */ void CommSSLBase64::decodeOverwriting(PBlock& ret, const char* s, size_t sz)
{
	if (!sz)
	{
		ret.cut(0);
		return;
	}
	_base64decFast( ret, s, sz );
}
