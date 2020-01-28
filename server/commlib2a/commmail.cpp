#include "ppinclude.h"
#include "commmail.h"
#include "commurl.h"
#include "_commmime.h"
#include "commssl.h"

#define MAX_ADDRESS_LINE_LENGTH 900
static const char* const _wdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char* const _months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
int CommMail::messageIdCounter =1;
const char * CommMail::encodingStrings[5]={"8bit","7bit","quoted-printable","base64","binary"};

/* static  void CommMail::normalizeAddress( PString& ret, PSconst char* addr )
{
const char* rightQ = strrchr(addr,'\"');

if(rightQ)
{
addr = rightQ+1;
}

const char* left = strchr( addr, '<' );
if( left == 0 )
{
ret = addr;
return;
}
const char* right = strchr( addr, '>' );
if( right == 0 || right < left )
{
ret = addr;
return;
}
ret.assign( left + 1, right );
}
*/


void CommMail::addBody( const char* text, const char* charset, UINT32 encoding, const char* contentType )
{
	Body& body = *bodies_.insert( bodies_.end(), Body() );
	body.encoding = encoding;
	body.contentType = contentType ? contentType : "text/plain";
	body.str = text;
	body.charset = charset ? charset : "us-ascii";
}

void CommMail::addHtmlBody( const char* text, const char* charset, UINT32 encoding )
{
	Body& body = *bodies_.insert( bodies_.end(), Body() );
	body.encoding = encoding;
	body.contentType = "text/html";
	body.str = text;
	body.charset = charset ? charset : "us-ascii";
}


void CommMail::addAttachment( const BYTE* p, size_t sz, const char* contentType, const char* name, const char* charset)
{
	Attachment& att = *attachments_.insert( attachments_.end(), Attachment() );
	memcpy( att.block.append( sz ), p, sz );
	att.isBinary = true;
	att.contentType = contentType;
	att.name = name ? name : "";
	if (charset)
		att.charset = charset;
}

void CommMail::addTextAttachment( const char* str, const char* contentType, const char* charset, const char* name )
{
	Attachment& att = *attachments_.insert( attachments_.end(), Attachment() );
	att.isBinary = false;
	att.str = str;
	att.contentType = contentType;
	att.charset = charset ? charset : "us-ascii";
	att.name = name ? name : "";
}

void CommMail::addAddressPure( int type, const char* addr )
{
	Address& add = *addrs.insert( addrs.end(), Address() );
	add.addrType = type;
	add.addr = addr;
	add.addrNormalized = addr;
	add.alreadyProcessed = false;
}

void CommMail::generMessageId(const char * suffix, UINT32 emailId, UINT32 type)
{
	messageId_.assign("<");
	if (emailId)
	{
		messageId_.appendHexInt(emailId).append("!");
	}
	if (type)
	{
		messageId_.appendHexInt(type).append("$");
	}
	messageId_.appendHexInt((UINT32) time(0)).append(".").appendHexInt(SYSTEMTICKS()).appendHexInt(++messageIdCounter).append(suffix).append(">");
}

const char * CommMail::parseMessageId(PString &suffix, UINT32 & emailId, UINT32 & type)
{
	const char * p;
	emailId=0;
	type=0;
	suffix.assign("");
	p = strchr(messageId_,'!');
	if (p)
	{
		emailId = strtoul(messageId_+1,0,16);
	}
	else
	{
		p = messageId_;
	}
	const char * pp = strchr(p,'$');
	if (pp)
	{
		type = strtoul(p+1,0,16);
	}
	p = strchr(messageId_,'@');
	if (p)
	{
		const char * p1 = strchr(p+1,'>');
		if (p1)
		{
			suffix.assign(p,p1);
		}
	}
	return messageId_;
}


/*
static bool _is8bit( const char* s )
{
for( const char* p = s ; *p ; ++p )
if( (unsigned char)*p >= 128 )
return true;
return false;
}
*/

static void _genBoundary( INT32& rand, PString& ret )
{
	ret = "------";
	int j;
	for( j=0; j < 32 ; ++j )
	{
		rand = rand * 214013L + 2531011L;
		int rnd = ( rand >> 16) & 0x7fff;
		ret.append( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[ rnd % 62 ] );
	}
}

void CommMail::_printBody( INT32& rand, PString& out ) const
{
	if( bodies_.size() == 0 )
	{
		out.append( "Content-Type: text/plain" );
		out.append( "\r\n" );
		out.append( "Content-Transfer-Encoding: 7bit" ).append( "\r\n" );

		out.append( "\r\n" );
	}
	else if( bodies_.size() == 1 )
	{
		const Body& body = bodies_[ 0 ];
		out.append( "Content-Type: " ).append( body.contentType );
		if( *body.charset )
			out.append( ";\r\n charset=\"" ).append( body.charset ).append( '\"' );
		if( body.encoding & eFormatFlowed)
			out.append( "; format=flowed");

		out.append( "\r\n" );
		out.append( "Content-Transfer-Encoding: " ).append( encodingStrings[body.encoding&eEncodingMask] ).append( "\r\n" );
		out.append( "\r\n" );
		out.append( body.str );
	}
	else
	{
		PString boundary;
		_genBoundary( rand, boundary );
		out.append( "Content-type: multipart/alternative;\r\n boundary=\"" ).append( boundary ).append( "\"\r\n" );
		out.append( "\r\n" );

		int n = bodies_.size();
		for( int i=0; i < n ; ++i )
		{
			out.append( "\r\n--" ).append( boundary ).append( "\r\n" );
			const Body& body = bodies_[ i ];
			out.append( "Content-Type: " ).append( body.contentType );
			if( *body.charset )
				out.append( ";\r\n charset=\"" ).append( body.charset ).append( '\"' );
			if( body.encoding & eFormatFlowed)
				out.append( "; format=flowed");

			out.append( "\r\n" );
			out.append( "Content-Transfer-Encoding: " ).append( encodingStrings[body.encoding&eEncodingMask] ).append( "\r\n" );
			out.append( "\r\n" );
			out.append( body.str );
		}
		out.append( "\r\n--" ).append( boundary ).append( "--\r\n" );
	}
}


static void _appendCurrentDate( PString& s )
{
	int tzdiff = -TIMEZONE();

	bool tzdiffneg = false;
	if( tzdiff < 0 )
	{
		tzdiffneg = true;
		tzdiff = -tzdiff;
	}
	int tzdiffhr = tzdiff / 3600;
	int tzdiffmin = ( tzdiff - tzdiffhr*3600 ) / 60;

	//time_t t = time( 0 );
	//struct tm* tm = localtime( &t );
	struct tm t;
	PCurrentLocalTime( &t );
	char buf[ 64 ];
	sprintf( buf, "%s, %d %s %d %02d:%02d:%02d %s%02d%02d", _wdays[ t.tm_wday ], t.tm_mday, _months[ t.tm_mon ], t.tm_year+1900, t.tm_hour, t.tm_min, t.tm_sec, tzdiffneg ? "-" : "+", tzdiffhr, tzdiffmin );
	s.append( buf );
}

void CommMail::addHeader( const char* name, const char* value )
{
	CommMailHeader& hdr = *headers_.insert( headers_.end(), CommMailHeader() );
	hdr.name = name;
	hdr.value = value;
}

void CommMail::out( PString& out, bool includeBcc ) const
{
	INT32 rand = RANDOMTIME();

	out.append( "Date: " );
	_appendCurrentDate( out );
	out.append( "\r\n" );

	int n = headers_.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		out.append(headers_[i].name).append(": ").append(headers_[i].value).append("\r\n");
	}

	out.append( "From: " ).append( from_.addr ).append( "\r\n" );

	n = addrs.size();
	PString toAddress;
	PString ccAddress;
	PString bccAddress;
	int lengthTo=6; // "To: " plus "\r\n" = 6 symbols
	int lengthCc=6; // "Cc: " plus "\r\n" = 6 symbols
	int lengthBcc=7; // "Bcc: " plus "\r\n" = 7 symbols
	for( i=0; i < n ; ++i )
	{
		switch( addrs[ i ].addrType )
		{
		case addrTo:
			if (toAddress.length())
			{
				toAddress.append(", ");
				lengthTo+=2;
				if (lengthTo + addrs[ i ].addr.length() > MAX_ADDRESS_LINE_LENGTH)
				{
					toAddress.append("\r\n ");
					lengthTo = 3; // Leading space plus final \r\n = 3 symbols
				}
			}
			toAddress.append(addrs[ i ].addr);
			lengthTo += addrs[ i ].addr.length();
			break;
		case addrCc:
			if (ccAddress.length())
			{
				ccAddress.append(", ");
				lengthCc+=2;
				if (lengthCc + addrs[ i ].addr.length() > MAX_ADDRESS_LINE_LENGTH)
				{
					ccAddress.append("\r\n ");
					lengthCc = 3;// Leading space plus final \r\n = 3 symbols
				}
			}
			ccAddress.append(addrs[ i ].addr);
			lengthCc += addrs[ i ].addr.length();
			break;
		case addrBcc:
			if (includeBcc)
			{
				if (bccAddress.length())
				{
					bccAddress.append(", ");
					lengthBcc+=2;
					if (lengthBcc + addrs[ i ].addr.length() > MAX_ADDRESS_LINE_LENGTH)
					{
						bccAddress.append("\r\n ");
						lengthBcc = 3;// Leading space plus final \r\n = 3 symbols
					}
				}
				bccAddress.append(addrs[ i ].addr);
				lengthBcc += addrs[ i ].addr.length();
			}
			
			break;
		default:
			PASSERT3( 0 );
			break;
		}
	}
	if( toAddress.length() )
		out.append( "To: " ).append( toAddress ).append( "\r\n" );
	if( ccAddress.length() )
		out.append( "Cc: " ).append( ccAddress ).append( "\r\n" );
	if (bccAddress.length())
	{
		if( bccAddress.length() )
			out.append( "Bcc: " ).append( bccAddress ).append( "\r\n" );
	}
	out.append( "Subject: " ).append( subj_ ).append( "\r\n" );

	if (messageId_.length())
	{
		out.append( "Message-Id: " ).append( messageId_ ).append( "\r\n" );
	}
	out.append( "MIME-Version: 1.0\r\n" );
	if( attachments_.size() == 0 )
	{
		_printBody( rand, out );
		return;
	}

	PString boundary;
	_genBoundary( rand, boundary );
	out.append( "Content-type: multipart/mixed;\r\n boundary=\"" ).append( boundary ).append( "\"\r\n" );
	out.append( "\r\n" );
	out.append( "This is a multi-part message in MIME format\r\n" );

	n = attachments_.size();
	for( i=-1; i < n ; ++i )
	{
		out.append( "\r\n--" ).append( boundary ).append( "\r\n" );
		if( i < 0 )
			_printBody( rand, out );
		else
		{
			const Attachment& att = attachments_[ i ];

			out.append( "Content-type: " ).append( att.contentType );
			if( *att.charset )
				out.append( ";\r\n charset=\"" ).append( att.charset ).append( '\"' );
			if( *att.name )
				out.append( ";\r\n name=\"" ).append( att.name ).append( '\"' );
			out.append( "\r\n" );

			const char* enc = "base64";
			if( !att.isBinary )
				//enc = _is8bit( att.str ) ? "8bit" : "7bit";
				enc = "8bit";
			out.append( "Content-Transfer-Encoding: " ).append(  enc ).append( "\r\n" );

			out.append( "Content-Disposition: attachment" );
			if( *att.name )
				out.append( ";\r\n filename=\"" ).append( att.name ).append( "\"\r\n" );
			else
				out.append( "\r\n" );

			out.append( "\r\n" );
			if( !att.isBinary )
				out.append( att.str );
			else
				CommSSLBase64::encode( out, att.block.ptr(), att.block.size(), 72 );
		}
	}
	out.append( "\r\n--" ).append( boundary ).append( "--\r\n" );
}


/* static */ void CommMail::forward( PBlock& ret, const char* from, const vector< Address >& addrs, const BYTE* p, size_t sz )
{
	PString hdrs;
	hdrs.append( "Resent-Date: " );
	_appendCurrentDate( hdrs );
	hdrs.append( "\r\n" );
	hdrs.append( "Resent-From: " ).append( from ).append( "\r\n" );

	int n = addrs.size();
	PString toAddress;
	PString ccAddress;
	int lengthTo=13; // "Resent-To: " plus "\r\n" = 13 symbols
	int lengthCc=13; // "Resent-Cc: " plus "\r\n" = 13 symbols

	for( int i=0; i < n ; ++i )
	{
		switch( addrs[ i ].addrType )
		{
		case addrTo:
			if (toAddress.length())
			{
				toAddress.append(", ");
				lengthTo+=2;
				if (lengthTo + addrs[ i ].addr.length() > MAX_ADDRESS_LINE_LENGTH)
				{
					toAddress.append("\r\n ");
					lengthTo = 3;// Leading space plus final \r\n = 3 symbols
				}
			}
			toAddress.append(addrs[ i ].addr);
			lengthTo += addrs[ i ].addr.length();
			break;
		case addrCc:
			if (ccAddress.length())
			{
				ccAddress.append(", ");
				lengthCc+=2;
				if (lengthCc + addrs[ i ].addr.length() > MAX_ADDRESS_LINE_LENGTH)
				{
					ccAddress.append("\r\n ");
					lengthCc = 3;// Leading space plus final \r\n = 3 symbols
				}
			}
			ccAddress.append(addrs[ i ].addr);
			lengthCc += addrs[ i ].addr.length();
			break;
		case addrBcc:
			//leave type as is; Bcc's will not be included to output
			break;
		default:
			PASSERT3( 0 );
			break;
		}
	}
	if( toAddress.length() )
		hdrs.append( "Resent-To: " ).append( toAddress ).append( "\r\n" );
	if( ccAddress.length() )
		hdrs.append( "Resent-Cc: " ).append( ccAddress ).append( "\r\n" );

	hdrs.append( "Resent-From: " ).append( from ).append( "\r\n" );
}
