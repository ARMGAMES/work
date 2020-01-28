#include "ppinclude.h"
#include "pptime.h"
#include "commmail.h"
#include "commurl.h"
#include "_commmime.h"
#include "commtok822.h"
#include "commssl.h"




static void CommAddressNormalizeL(vector<PString> &addr, vector<PString> & addrn, const char * addressString)
{

	CommAddressNormalize(addr,addrn,addressString);
#if 0
	try
	{
		vector<PString> addr_;
		vector<PString> addrn_;
		CommMailAddressNormalize(addr_,addrn_,addressString,CommMailAddressFlagNormalizeQuotedLocalPart);
		if (addrn.size() != addrn_.size())
		{
			PTRACE3("ADDR_NORMILIZE_VECTOR! Address string '%s' was parsed differently", addressString);
		}
		else
		{
			for (int i=0 ; i< addrn.size(); i++)
			{
				if (!addrn[i].equalsIgnoreCase(addrn_[i]))
				{
					PTRACE3("ADDR_NORMILIZE_VECTOR! Address string '%s' , address '%s' is not equal '%s'", addressString,addrn[i].c_str(),addrn_[i].c_str());
				}
			}
		}
	}
	catch(PError(&er))
	{
		PLog("ADDR_NORMILIZE_VECTOR!, PError '%s', address '%s'",er.why(),addressString);
	}
#endif
}



static void CommAddressNormalizeL(PString &addr, PString &addrn, const char * addressString)
{
	CommAddressNormalize(addr,addrn,addressString);
#if 0
	try
	{
		PString addr_;
		PString addrn_;
		CommMailAddressNormalize(addr_,addrn_,addressString,0,CommMailAddressFlagNormalizeQuotedLocalPart);
		if (!addrn.equalsIgnoreCase(addrn_))
		{
			PTRACE3("ADDR_NORMILIZE! Address string '%s' , address '%s' is not equal '%s'", addressString,addrn.c_str(),addrn_.c_str());
		}
	}
	catch(PError(&er))
	{
		PLog("ADDR_NORMILIZE!, PError '%s', address '%s'",er.why(),addressString);
	}
#endif
}

void CommMail::addAddress( int type, const char* addr, bool alreadyProcessed_ )
{
	vector <PString> ad,ad_n;
	CommAddressNormalizeL(ad,ad_n,addr);
	for (int i=0;i<ad.size();i++)
	{
		Address& add = *addrs.insert( addrs.end(), Address() );
		add.addrType = type;
		add.addr = ad[i];
		add.addrNormalized = ad_n[i];
		add.alreadyProcessed = alreadyProcessed_;
	}
}
void CommMail::addAddress( int type, const char* addr, const char * addrNorm, bool alreadyProcessed_ )
{
	Address& add = *addrs.insert( addrs.end(), Address() );
	add.addrType = type;
	add.addr = addr;
	add.addrNormalized = addrNorm;
	add.alreadyProcessed = alreadyProcessed_;
}

void CommMail::setReplyToAndNormalize( const char* f )
{
	CommAddressNormalizeL(replyTo_.addr,replyTo_.addrNormalized,f);
	replyTo_.alreadyProcessed = false;
}
void CommMail::setFromAndNormalize( const char* f )
{
	CommAddressNormalizeL(from_.addr,from_.addrNormalized,f);
	from_.alreadyProcessed = false;
}

static int _hexCharToInt( char c )
{
	if( c >= '0' && c <= '9' )
		return c - '0';
	if( c >= 'A' && c <= 'F' )
		return c - 'A' + 10;
	if( c >= 'a' && c <= 'f' )
		return c - 'a' + 10;
	else
		return -1;
}

static void _quotedPrintableDec( PBlock& ret, const char* intext, const char* intextEnd )
{
	while(intext < intextEnd)
	{
		char c = *intext++;
		if( c != '=' )
			*ret.append( 1 ) = c;
		else
		{
			if( intext == intextEnd )
				return; //ignore
//				throw PInternalError( "CommMail: quoted-printable: unexpected EOF after '=' sign" );
			char c1 = *intext++;
			if( intext == intextEnd )
				return; //ignore
//				throw PInternalError( "CommMail: quoted-printable: unexpected EOF after '=' sign" );
			char c2 = *intext++;

			if( c1 == '\r' && c2 == '\n' )//soft line break
				;//ignore
			else
			{
				int i1 = _hexCharToInt( c1 );
				int i2 = _hexCharToInt( c2 );

				if( i1 < 0 || i2 < 0 )
					return; //ignore
//					throw PInternalError( "CommMail: quoted-printable: unexpected char after '=' sign" );
				*ret.append( 1 ) = (BYTE)( i1 * 16 + i2 );
			}
		}
	}
}

struct _CommMailRd//for _CommMimeMultipart
{
	const BYTE* p;
	const BYTE* pEnd;

	int read()
	{
		return p < pEnd ? *p++ : -1;
	}
};
/*
static void _decodeFlowFormatText(PString & out, const char * in, int size, const bool deleteSpace)
{
bool beginLine = true;
bool spaceChar = false;
int quotedLevel =0;

while(size>0 && *in)
{
char c = *in;
if (beginLine)
{
if (!strncmp(in,"-- \r\n",5))
{
// check for signature
in+=5;
size-=5;
out.append("-- \r\n");
continue;
}
beginLine = false;
if (c=='>')
{
int level=1;
in++;
size--;
while(*in == '>')
{
level++;
++in;
size--;
}
if (quotedLevel == level)
{
c = *in;
}
else 
{
if (quotedLevel != 0 )
{
// incorrect formatting
out.append("\r\n");
}
else if ((out.length() > 1 && out[out.length()-2] == '\r' && out[out.length()-1] == '\n') == false)
{
// incorrect formatting
out.append("\r\n");
}

// begin of quotation 
quotedLevel = level;
while(level--)
{
out.append('>');
}
c = *in;
}
if (!c || size<=0)
break;
if (c==' ')
{
c = *++in;
size--;
}
// again check for signature
if (!strncmp(in,"-- \r\n",5))
{
// check for signature
in+=5;
size-=5;
beginLine = true;
out.append("-- \r\n");
level = quotedLevel;
while(level--)
{
out.append('>');
}
continue;
}
}
else
{
if (c==' ')
{
c = *++in;
size--;
}
}
}

if (!c || size <= 0)
break;
if (c == ' ')
{
if (spaceChar)
out.append(c);
else
spaceChar = true;
in++;
size--;
continue;
}

if (c == '\r')
{
if (in[1] == '\n')
{
if (spaceChar)
{
// soft line break
if (deleteSpace == false)
{
out.append(' ');
}
spaceChar = false;
}
else
{
// hard line break
out.append("\r\n");
quotedLevel = 0;
}
in+=2;
size-=2;
beginLine = true;
continue;
}
}
if (spaceChar)
{
spaceChar = false;
out.append(' ');
}
out.append(c);
in++;
size--;
}
if (spaceChar)
out.append(' ');
}

*/

static void _decodeBlock( PBlock& ret, const BYTE* p, size_t sz, const char* contentEncoding, UINT32 & encodingFlag )
{
	if(*contentEncoding == 0)
	{
		memcpy( ret.alloc( sz ), p, sz );
	}
	else if (PString::compareIgnoreCase( contentEncoding, "7bit" ) == 0)
	{
		memcpy( ret.alloc( sz ), p, sz );
		encodingFlag|=CommMail::eEncoding7bit;
	}
	else if (PString::compareIgnoreCase( contentEncoding, "binary" ) == 0)
	{
		memcpy( ret.alloc( sz ), p, sz );
		encodingFlag|=CommMail::eEncodingBinary;
	}
	else if (PString::compareIgnoreCase( contentEncoding, "8bit" ) == 0)
	{
		memcpy( ret.alloc( sz ), p, sz );
		encodingFlag|=CommMail::eEncoding8bit;
	}
	else if( PString::compareIgnoreCase( contentEncoding, "quoted-printable" ) == 0 )
	{
		_quotedPrintableDec( ret, (const char*)p, (const char*)( p + sz ) );
		encodingFlag|=CommMail::eEncodingQuotedPrintable;
	}
	else if( PString::compareIgnoreCase( contentEncoding, "base64" ) == 0 )
	{
		CommSSLBase64::decode( ret, (const char*)p, sz );
		encodingFlag|=CommMail::eEncodingBase64;
	}
	else
		throw PInternalError( "Unknown encoding" );
}

static void _blockToStr( PString& ret, /*non-const!*/ PBlock& block )
{
	if (block.size())
	{
		ret.assignFromBlock(block.ptr(), block.size());
	}
}
bool CommMail::_findBodyContentType(const char * contentType)
{
	for (int i=0;i<bodies_.size();i++)
	{
		if (bodies_[i].contentType.equalsIgnoreCase(contentType))
			return true;
	}
	return false;
}
void CommMail::_loadBodies( const BYTE* p, const BYTE* pEnd, const char* contentType_, const char* charset, const char* boundary, const char* contentEncoding, const char * attachName, const char * contentFormat)
{
	PString contentType(contentType_);
	contentType.toLower();

	if( contentType.startsWith("multipart/"))
	{
		_CommMailRd rd;
		rd.p = p;
		rd.pEnd = pEnd;
		_CommMimeMultipart< _CommMailRd > multipart( rd, boundary );

		multipart.skipFirstBlock();
		for(;;)
		{
			PString altContentType="text/plain";
			PString altCharset;
			PString altEncoding;
			PString altAttachName;
			PString altContentFormat;
			PString altBoundary;

			PString accumulator;
			bool shouldBreak = false;
			for(;;)
			{
				PString line;
				multipart.readHeaderLine( line );
				if( line.length() == 0 )
					shouldBreak = true;
				else if( *line == ' ' || *line == '\t' )
				{
					accumulator.append( line );
					continue;
				}

				const char* ln = accumulator;
				if( *ln )
				{
					PString type;
					_CommMime::getHdrName( type, ln );

					if( PString::compareIgnoreCase( type, "Content-Disposition" ) == 0 )
					{
						type = "";
						_CommMime::getHdrValue( type, ln );
						for(;;)
						{
							if( !*ln )
								break;

							PString attrName;
							PString attrValue;
							_CommMime::getHdrAttr( attrName, attrValue, ln );
							if( PString::compareIgnoreCase( attrName, "filename" ) == 0 )
								altAttachName = attrValue;
						}
					}

					else if( PString::compareIgnoreCase( type, "Content-type" ) == 0 )
					{
						altContentType = "";
						_CommMime::getHdrValue( altContentType, ln );
						for(;;)
						{
							if( !*ln )
								break;

							PString attrName;
							PString attrValue;
							_CommMime::getHdrAttr( attrName, attrValue, ln );
							if( PString::compareIgnoreCase( attrName, "charset" ) == 0 )
								altCharset = attrValue;
							else if( PString::compareIgnoreCase( attrName, "boundary" ) == 0 )
								altBoundary = attrValue;
							else if( PString::compareIgnoreCase( attrName, "name" ) == 0 )
								altAttachName = attrValue;
							else if( PString::compareIgnoreCase( attrName, "format" ) == 0 )
								altContentFormat = attrValue;

						}
					}
					else if( PString::compareIgnoreCase( type, "Content-Transfer-Encoding" ) == 0 )
					{
						PString altEncoding_;
						_CommMime::getHdrValue( altEncoding_, ln );
						altEncoding.assign(altEncoding_.ltrim(),altEncoding_.rtrim());
					}
				}
				accumulator = line;
				if( shouldBreak )
					break;
			}

			PBlock block;
			bool isLast = multipart.readBlock( block );
			_loadBodies( block.ptr(), block.ptr() + block.size(), altContentType, altCharset, altBoundary, altEncoding, altAttachName, altContentFormat );
			if( isLast )
				break;
		}
	}
	else
	{
		bool isBodyContent = false;
		bool isTextContent = false;
		if (PString::compareIgnoreCase( contentType_, "text/plain" ) == 0)
		{
			// check that this is first 
			if (!_findBodyContentType("text/plain"))
			{
				isBodyContent = true;
				isTextContent = true;
			}
		}
		else if (PString::compareIgnoreCase( contentType_, "text/html" ) == 0)
		{
			// check that this is first 
			if (!_findBodyContentType("text/html"))
				isBodyContent = true;
		}
		if (isBodyContent)
		{
			Body& body = *bodies_.insert( bodies_.end(), Body() );
			body.contentType = contentType_;
			body.charset = charset;

			try
			{
				PBlock block;
				_decodeBlock( block, p, pEnd - p, contentEncoding, body.encoding);
				if (isTextContent && PString::compareIgnoreCase( contentFormat, "flowed" ) == 0)
				{
					try
					{
						//						_decodeFlowFormatText(body.str,(const char *) block.ptr(),block.size(), false);
						body.encoding |= CommMail::eFormatFlowed;
					}
					catch(PError &er)
					{
						PLog("FlowFormat Decoding error '%s'", er.why());
					}
				}
				//				if (!(body.encoding & CommMail::eFormatFlowed))
				{
					try
					{
						_blockToStr( body.str, block );
					}
					catch (PError &x)
					{
						body.str = "CommMail: Unable to decode message body: ";
						body.str.append( x.why() );
						body.str.append("\r\nBody is saved as attachment.");
						body.contentType = "text/plain";
						body.charset = "us-ascii";
						body.encoding = 0;
						Attachment& att = *attachments_.insert( attachments_.end(), Attachment() );
						att.isBinary = true;
						att.contentType = contentType;
						att.charset = charset;
						att.name = "Body";
						att.block=block;
					}
				}
				body.encoding|= CommMail::eEncodingDecoded;
			}
			catch( PError& x )
			{
				body.str = "CommMail: Unable to decode message body: ";
				body.str.append( x.why() );
				body.contentType = "text/plain";
				body.charset = "us-ascii";
				body.encoding=0;
			}
		}
		else
		{
			Attachment& att = *attachments_.insert( attachments_.end(), Attachment() );
			att.isBinary = true;
			att.contentType = contentType_;
			att.charset = charset;
			att.name = attachName;

			try
			{
				PBlock block;
				UINT32 encoding=0;
				_decodeBlock( att.block , p, pEnd - p, contentEncoding, encoding );
			}
			catch( PError& x )
			{
				att.isBinary = false;
				att.str = "CommMail: Unable to decode attachment: ";
				att.str.append( x.why() );
				att.contentType = "text/plain";
				att.charset = "us-ascii";
			}
		}
	}

}

void CommMail::load( const BYTE* pp, size_t sz )
{
	from_.addr = "";
	from_.addrNormalized = "";
	replyTo_.addr = "";
	replyTo_.addrNormalized = "";
	addrs.clear();
	subj_ = "";
	date_=0;
	//	contentType_ = "text/plain";
	//	charset_ = "us-ascii";

	PString contentType="text/plain";
	PString charset;
	PString boundary;
	PString encoding;
	PString attachName;
	PString contentFormat;

	PString accumulator;
	const BYTE* ppEnd = pp + sz;
	for(;;)
	{
		bool shouldBreak = false;
		PString ln;
		if( !CommLine::read( ln, pp, ppEnd - pp ) )
			throw PInternalError( "CommMail: Unexpected end of message" );
		if( *ln == 0 )
			shouldBreak = true;
		else if( *ln == ' ' || *ln == '\t' )
		{
			accumulator.append( ln );
			continue;
		}

		const char* p = accumulator;
		if( *p )//else is a first line
		{
			PString name;
			_CommMime::getHdrName( name, p );

			if( PString::compareIgnoreCase( name, "From" ) == 0 )
			{
				PString psFrom;
				setFromAndNormalize(psFrom.assign( PString::ltrim( p ), PString::rtrim( p )));
			}
			else if( PString::compareIgnoreCase( name, "Message-Id" ) == 0 )
			{
				messageId_ = "";
				_CommMime::getHdrValue( messageId_, p );
			}
			else if( PString::compareIgnoreCase( name, "Reply-To" ) == 0 )
			{
				PString psReplyTo;
				setReplyToAndNormalize(psReplyTo.assign( PString::ltrim( p ), PString::rtrim( p )));
			}
			else if( PString::compareIgnoreCase( name, "Date" ) == 0 )
			{
				struct tm tm;
				const char *_month[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
				const char *ppDate = p;
				char buf[4];
                memset(buf, 0, sizeof(buf));
				char zone[6];
                memset(zone, 0, sizeof(zone));
				memset(&tm,0,sizeof(tm));
				date_=plib_time(0);

				if (strchr(ppDate,','))
					ppDate = strchr(ppDate,',')+1;

				if (sscanf(ppDate,"%d %3s %d %d:%d:%d %5s",&tm.tm_mday,buf,&tm.tm_year,&tm.tm_hour,&tm.tm_min,&tm.tm_sec,zone)==7)
				{
					if (tm.tm_mday > 0 && tm.tm_mday < 32 && tm.tm_year >= 1970 &&
						tm.tm_hour >= 0 && tm.tm_hour < 24 &&
						tm.tm_min >= 0 && tm.tm_min < 60 &&
						tm.tm_sec >= 0 && tm.tm_sec < 60)
					{
						for (tm.tm_mon = 0; tm.tm_mon < 12; tm.tm_mon++)
						{
							if (!stricmp(buf,_month[tm.tm_mon]))
								break;
						}
						if (tm.tm_mon < 12)
						{
							tm.tm_year -= 1900;
							date_ = plib_mkUTCtime(&tm);
							if (date_ == (time_t) -1)
								date_ = plib_time(0);
							else
							{

								int hour = 0;
								int min = 0;
								bool sign = true;
								if (zone[0] == '+' || zone[0] == '-')
								{
									sign = zone[0] == '-';
									min = atoi(zone+3);
									zone[3]=0;
									hour = atoi(zone+1); 
								}
								else
								{
									struct tm_zones
									{
										const char * zoneName;
										int hour;
									};
									const tm_zones tmz[] = 
									{{"UT",0},{"GMT",0},{"EST",5},{"EDT",4},{"CST",6},{"CDT",5},
									{"MST",7},{"MDT",6},{"PST",8},{"PDT",7}};

									for (int z = 0;z<sizeof(tmz)/sizeof(tmz[0]);z++)
									{
										if (!strnicmp(zone,tmz[z].zoneName,strlen(tmz[z].zoneName)))
										{
											hour = tmz[z].hour;
											break;
										}
									}
								}
								if (sign)
									date_ += hour*60*60 + min*60;
								else
									date_ -= hour*60*60 + min*60;
							}
						}
					}
				}
			}
			/*
			else if( PString::compareIgnoreCase( name, "In-Reply-To" ) == 0 )
			{
			inReplyTo_.assign( PString::ltrim( p ), PString::rtrim( p ) );
			}
			*/
			else if( PString::compareIgnoreCase( name, "Subject" ) == 0 )
			{
				subj_.assign( PString::ltrim( p ), PString::rtrim( p ) );
			}
			else if( PString::compareIgnoreCase( name, "To" ) == 0 )
			{
				addAddress( addrTo, p );
			}
			else if( PString::compareIgnoreCase( name, "Cc" ) == 0 )
			{
				addAddress( addrCc, p );
			}
			else if( PString::compareIgnoreCase( name, "Bcc" ) == 0 )
			{
				addAddress( addrBcc, p );
			}
			else if( PString::compareIgnoreCase( name, "Content-Disposition" ) == 0 )
			{
				name = "";
				_CommMime::getHdrValue( name, p );
				for(;;)
				{
					if( !*p )
						break;

					PString attrName;
					PString attrValue;
					_CommMime::getHdrAttr( attrName, attrValue, p );
					if( PString::compareIgnoreCase( attrName, "filename" ) == 0 )
						attachName = attrValue;
				}
			}

			else if( PString::compareIgnoreCase( name, "Content-type" ) == 0 )
			{
				contentType = "";
				_CommMime::getHdrValue( contentType, p );
				for(;;)
				{
					if( !*p )
						break;

					PString attrName;
					PString attrValue;
					_CommMime::getHdrAttr( attrName, attrValue, p );
					if( PString::compareIgnoreCase( attrName, "charset" ) == 0 )
						charset = attrValue;
					else if( PString::compareIgnoreCase( attrName, "boundary" ) == 0 )
						boundary = attrValue;
					else if( PString::compareIgnoreCase( attrName, "name" ) == 0 )
						attachName = attrValue;
					else if( PString::compareIgnoreCase( attrName, "format" ) == 0 )
						contentFormat = attrValue;

				}

			}
			else if( PString::compareIgnoreCase( name, "Content-Transfer-Encoding" ) == 0 )
			{
				PString encoding_;
				_CommMime::getHdrValue( encoding_, p );
				encoding.assign(encoding_.ltrim(),encoding_.rtrim());
			}
			else
			{
				PString v;
				v.assign( PString::ltrim( p ), PString::rtrim( p ));
				addHeader(name,v.c_str());
			}
		}

		accumulator = ln;
		if( shouldBreak )
			break;
	}

	_loadBodies(pp, ppEnd, contentType, charset, boundary, encoding, attachName, contentFormat );

}

const char* CommMail::getHeader( const char* name )
{
	int n = headers_.size();
	for( int i=0; i < n ; ++i )
		if( PString::compareIgnoreCase( headers_[ i ].name, name ) == 0 )
			return headers_[ i ].value;
	return 0;
}
