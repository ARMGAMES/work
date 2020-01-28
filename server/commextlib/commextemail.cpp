#include <commlib.h>
#include <_commmime.h>
#include <commtok822.h>
#include "commextemail.h"

struct _CommExtMailRd//for _CommMimeMultipart
{
	size_t sz;
	const BYTE * p;
	int read()
	{
		if (sz == -1)
			return *p != 0 ? *p++ : -1;
		else
		{
			if(!sz)
				return -1;
			sz--;
			return *p != 0 ? *p++ : -1;
		}
	}
};

void CommExtEmail::_loadBodies(const char* source, Data & d, size_t sz)
{

	Header * cType = d.headers.find("content-type");
	PString contentType;
	if (cType)
		contentType = cType->value;
	contentType.toLower();

	if( contentType.startsWith("multipart/"))
	{
		_CommExtMailRd rd;
		rd.p = (const BYTE *) source;
		rd.sz = sz;
		PASSERT3(cType);
		const Attribute * boundary = cType->attr.find("boundary");
		PASSERT3(boundary);

		_CommMimeMultipart< _CommExtMailRd > multipart( rd, boundary->value );

		try
		{
			multipart.skipFirstBlock();
		}
		catch(PError &er)
		{
			if (!noDebugPrint)
			{
				PLog("CommExtEmail::_loadBodies skipFirstBlock error %s, p position = %d",er.why(),(rd.p - (const BYTE *)source)); 
				PDump(source);
			}
			if(ignoreAllErrors)
			{
				hasError = true;
				return; 
			}
			throw er;
		}
		for(;;)
		{
			PString accumulator;
			bool shouldBreak = false;
			Data  * body = *d.data.insert(d.data.end(), new Data());
			for(;;)
			{
				PString line;
				try
				{
					multipart.readHeaderLine( line );
				}
				catch(PError &er)
				{
					if (!noDebugPrint)
					{
						PLog("CommExtEmail::_loadBodies readHeaderLine error %s, p position = %d",er.why(),(rd.p - (const BYTE *)source)); 
						PDump(source);
						PLog("line:");
						PDump(line);
					}
					if(ignoreAllErrors)
					{
						hasError = true;
						return; 
					}
					throw er;
				}

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
					_loadHeader(ln,body->headers);
				}
				accumulator.moveFrom(line);
				if( shouldBreak )
					break;
			}
			PString block;
			bool isLast;
			try
			{
				isLast = multipart.readBlock( block );
			}
			catch(PError &er)
			{
				PLog("CommExtEmail::_loadBodies readBlock error %s, p position = %d",er.why(),(rd.p - (const BYTE *)source)); 
				if (!strcmp(er.why(),"_CommMimeMultipart: Unexpected end of input stream"))
				{
					// closing boundary not found, ignore and continue
					isLast = true;
					hasError = true;
				}
				else
				{
					if (!noDebugPrint)
					{
						PDump(source);
						PLog("block:");
						PDump(block);
					}
					if(ignoreAllErrors)
					{
						hasError = true;
						return; 
					}
					throw er;
				}
			}
			_loadBodies( block.c_str(), *body, -1);
			if( isLast || hasError)
				break;
		}
	}
	else
	{
		if(sz == -1)
			d.emailbody.assign(source);
		else
			d.emailbody.assign(source,sz);
	}
}


void CommExtEmail::_loadHeader( const char *& p, CommExtPairVector <Header> &h)
{
	PString name;
	PString value;
	_CommMime::getHdrName( name, p );
	name.cut(name.rtrim() - name.c_str());
	_CommMime::getHdrValue( value, p );
	value.cut(value.rtrim() - value.c_str());
	Header & _h = h.insertMove(name);
	_h.value.moveFrom( value );
	_CommMime::skipSpace(p);
	for(;;)
	{
		if( !*p )
			break;

		PString attrName;
		PString attrValue;
		BYTE delim = 0;
		_CommMime::getHdrAttrNoName( attrName, attrValue, p, delim );
		attrName.cut(attrName.rtrim() - attrName.c_str());
		attrValue.cut(attrValue.rtrim() - attrValue.c_str());
		Attribute &a = _h.attr.insertMove(attrName);
		a.value.moveFrom( attrValue );
		a.delim = delim;
	}
}

void CommExtEmail::_loadHeader( const char *& p)
{
	PString name;
	_CommMime::getHdrName( name, p );
	name.cut(name.rtrim() - name.c_str());
	if( PString::compareIgnoreCase( name, "From" ) == 0 ||
		PString::compareIgnoreCase( name, "To" ) == 0 ||
		PString::compareIgnoreCase( name, "Reply-To" ) == 0 ||
		PString::compareIgnoreCase( name, "Sender" ) == 0 ||
		PString::compareIgnoreCase( name, "Cc" ) == 0 ||
		PString::compareIgnoreCase( name, "Bcc" ) == 0 || 
		PString::compareIgnoreCase( name, "Return-path" ) == 0
	  )
	{
		Header & _h = data.headers.insert(name);
		_h.value.assign( PString::ltrim( p ), PString::rtrim( p ));

		vector <PString> ad,ad_n;
		CommAddressNormalize(ad,ad_n,_h.value.c_str());
		for (int i=0;i<ad.size();i++)
		{
			Address & _a = addresses.insert(name);
			_a.address.moveFrom(ad[i]);
			_a.normalAddress.moveFrom(ad_n[i]);
		}
	}
	else if( PString::compareIgnoreCase( name, "Subject" ) == 0 )
	{
		Header & _h = data.headers.insertMove(name);
		_h.value.assign( PString::ltrim( p ), PString::rtrim( p ));
	}
	else
	{
		Header & _h = data.headers.insertMove(name);
		_CommMime::getHdrValue( _h.value, p );
		_CommMime::skipSpace(p);
		for(;;)
		{
			if( !*p )
				break;
			PString attrName;
			PString attrValue;
			BYTE delim = 0;
			_CommMime::getHdrAttrNoName( attrName, attrValue, p, delim );
			attrName.cut(attrName.rtrim() - attrName.c_str());
			attrValue.cut(attrValue.rtrim() - attrValue.c_str());
			Attribute &a = _h.attr.insert(attrName);
			a.value.moveFrom( attrValue );
			a.delim = delim;
		}
	}
}



void CommExtEmail::load( const char* source )
{
	loadHeaders(source );
	if (!hasError)
		_loadBodies(source, data, -1);
}

void CommExtEmail::load( const BYTE* source, size_t sz )
{
	size_t used_sz = loadHeaders(source, sz );
	if (!hasError)
		_loadBodies((const char *)(source + used_sz), data, sz - used_sz);
}

void CommExtEmail::loadHeaders( const char*& source )
{
	PString accumulator;
	for(;;)
	{
		bool shouldBreak = false;
		PString ln;
		if( !CommLine::read( ln, source ) )
		{
			if(ignoreAllErrors)
			{
				hasError = true;
				return; 
			}
			throw PInternalError( "ParsedEmail: Unexpected end of message" );
		}
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
			_loadHeader(p);
		}
		accumulator.moveFrom(ln);
		if( shouldBreak )
			break;
	}
}

size_t CommExtEmail::loadHeaders( const BYTE* sourceOrig, size_t sz )
{
	PString accumulator;
	const BYTE* source = sourceOrig;
	for(;;)
	{
		bool shouldBreak = false;
		PString ln;

		if( !CommLine::read( ln, source, sz - (source - sourceOrig)))
		{
			if(ignoreAllErrors)
			{
				hasError = true;
				return source - sourceOrig; 
			}
			throw PInternalError( "ParsedEmail: Unexpected end of message" );
		}
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
			_loadHeader(p);
		}
		accumulator.moveFrom(ln);
		if( shouldBreak )
			break;
	}
	return source - sourceOrig;
}

void CommExtEmail::_printHeaders( PString & out, CommExtPairVector <Header> &h)
{
	for ( int i = 0; i < h.size(); i++)
	{
		int length = h[i].name.length() + 2;
		out.append( h[i].name).append(": ");
		const char * pStart = h[i].value.value.c_str();
		int valueLength = h[i].value.value.length();
		while (1)
		{
			if ( length + valueLength > MaxLineLength)
			{
				const char * pEnd = pStart + MaxLineLength - length;
				while ( pEnd != pStart && *pEnd != ',')
					pEnd--;
				if ( pEnd == pStart)
				{
					pEnd = pStart + MaxLineLength - length;
					while ( pEnd != pStart && *pEnd != ' ')
						pEnd--;
					if ( pEnd == pStart)
					{
						pEnd = pStart + MaxLineLength - length;
					}
					else
						pEnd++;
				}
				else
					pEnd++;
				valueLength -= pEnd - pStart;
				out.append( pStart, pEnd).append("\r\n ");
				pStart = pEnd;
				length = 1;
			}
			else
			{
				length += valueLength;
				out.append( pStart );
				pStart += length;
				break;
			}
		}
		for ( int j = 0; j < h[i].value.attr.size(); j++)
		{
			int valueLength = (h[i].value.attr[j].name.length() ? (h[i].value.attr[j].name.length() + 1) : 0) +  h[i].value.attr[j].value.value.length() + 2 + (h[i].value.attr[j].value.delim ? 2 : 0);
			if ( length + valueLength > MaxLineLength)
			{
				out.append(";\r\n ");
				length = 1;
			}
			else
			{
				out.append("; ");
				length += 2;
			}
			if (h[i].value.attr[j].name.length())
			{
				if (h[i].value.attr[j].value.delim)
				{
					out.append(h[i].value.attr[j].name).append('=').append(h[i].value.attr[j].value.delim).append(h[i].value.attr[j].value.value).append(h[i].value.attr[j].value.delim);
					length += 1 + h[i].value.attr[j].name.length() + h[i].value.attr[j].value.value.length() + 2; 
				}
				else 
				{
					out.append(h[i].value.attr[j].name).append('=').append(h[i].value.attr[j].value.value);
					length += 1 + h[i].value.attr[j].name.length() + h[i].value.attr[j].value.value.length(); 
				}

			}
			else 
			{
				out.append(h[i].value.attr[j].value.value);
				length += h[i].value.attr[j].value.value.length(); 
			}
		}
		out.append("\r\n");
	}
	out.append("\r\n");

}


void CommExtEmail::printHeaders( PString & out)
{
	_printHeaders(out,data.headers);
}

void CommExtEmail::_printData( PString & out, Data &d)
{
	_printHeaders(out,d.headers);
	if ( d.emailbody.length())
		out.append(d.emailbody);

	else
	{
		for ( int i = 0; i < d.data.size(); i++)
		{
			_printData(  out, *d.data[i]);
		}
	}
}

void CommExtEmail::print( PString & out)
{
	_printData(out,data);

}

bool CommExtEmail::findBody(const char * &source, const char * type, PString & encoding, PString & charset, bool & formatFlowed, const Data &d) const
{
	formatFlowed = false;
	const Header * cType = d.headers.find("content-type");
	if( cType && !strnicmp(cType->value.c_str(),"multipart/",10))
	{
		for (int i=0; i<d.data.size(); i++)
		{
			if (findBody(source,type,encoding,charset, formatFlowed, *d.data[i]))
				return true;
		}
		return false;
	}
	else if (cType && cType->value.equalsIgnoreCase(type))
	{
		const Attribute * fileNameContentType = 0;
		fileNameContentType =cType->attr.find("name"); 
		if (fileNameContentType)
			return false;
		const Header * contentDisposition = d.headers.find("Content-Disposition");
		if (contentDisposition && !contentDisposition->value.equals("inline"))
			return false;

		const Header * contentTypeEncoding;
		contentTypeEncoding = d.headers.find("Content-Transfer-Encoding");
		if (contentTypeEncoding)
			encoding = contentTypeEncoding->value;
		const Attribute * attr;
		attr = cType->attr.find("charset");
		if (attr)
		{
			charset = attr->value;
		}
		if (cType->value.equalsIgnoreCase("text/plain"))
		{
			const Attribute * format = cType->attr.find("format");
			if (format && format->value.equalsIgnoreCase("flowed"))
				formatFlowed = true;
		}
		source = d.emailbody.c_str();
		return true;
	}
	else
		return false;
}
