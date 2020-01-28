#ifndef commmail_h_included
#define commmail_h_included

#include "ppinclude.h"
#include "ppstring.h"


class CommMail//RFC 822 envelope
{
public:
	enum { addrTo, addrCc, addrBcc };
	enum { eEncoding8bit=0,eEncoding7bit=1,eEncodingQuotedPrintable=2,eEncodingBase64=3,eEncodingBinary=4, eEncodingMask = 7, eFormatFlowed=0x10000000, eEncodingDecoded=0x40000000 };
	static const char *encodingStrings[5];
	//	enum { attachInline = 0x01, attachNone /* html/text alternatives */ = 0x02 };

	struct Address
	{
		int addrType;//addr*
		PString addr;
		PString addrNormalized;
		bool alreadyProcessed;
	};

	struct Body
	{
		UINT32 encoding;
		PString str;
		PString contentType;
		PString charset;//for text contentTypes only
		Body()
		{
			encoding = 0;
		}
	};
	struct Attachment
	{
		//int flags;//attach*
		PString str;
		bool isBinary;
		PBlock block;
		PString contentType;
		PString charset;//for text contentTypes only
		PString name;
	};
	struct CommMailHeader
	{
		PString name;
		PString value;
	};

private:

	vector< CommMailHeader > headers_;
	Address from_;
	Address replyTo_;

	//	PString from_;
	//	PString replyTo_;
	time_t  date_;
	//	PString inReplyTo_;
	vector< Address > addrs;
	PString subj_;
	PString messageId_;
	static int messageIdCounter;

	vector< Body > bodies_;
	vector< Attachment > attachments_;
	//	static void normalizeAddress( PString& ret, const char* addr );
public:
	void setReplyTo( const char* f,const char * fn )
	{
		replyTo_.addr = f;
		replyTo_.addrNormalized=fn;
		replyTo_.alreadyProcessed = false;
	}
	void setReplyToAndNormalize( const char* f );

	CommMail()
	{
		date_ = 0;
	}

	void addAddress( int type, const char* addr,bool alreadyProcessed_ = false );
	void addAddress( int type, const char* addr,const char* addrNorm, bool alreadyProcessed_ = false );
	void addAddressPure( int type, const char* addr );
	void setDate(const time_t d)
	{
		date_=d;
	}
	void setFrom( const char* f,const char * fn )
	{
		from_.addr = f;
		from_.addrNormalized=fn;
		from_.alreadyProcessed = false;
	}
	void setFromAndNormalize( const char* f );
	void setSubject( const char* subj )
	{
		subj_ = subj;
	}
	void setMesageId(const char *messageId__)
	{
		messageId_=messageId__;
	}
	void generMessageId(const char * suffix, UINT32 emailId_ = 0, UINT32 type = 0);
	void addBody( const char* text, const char* charset = 0, UINT32 encoding = eEncoding8bit, const char* contentType = 0 );
	void addHtmlBody( const char* text, const char* charset = 0, UINT32 encoding = eEncoding8bit );
	void addAttachment( const BYTE* p, size_t sz, const char* contentType, const char* name = 0, const char* charset = 0);
	void addTextAttachment( const char* str, const char* contentType, const char* charset = 0, const char* name = 0);

	const Address & replyTo() const
	{
		return *replyTo_.addr ? replyTo_ : from_;
	}

	void out( PString& out, bool includeBcc = false ) const;
	void load( const BYTE* p, size_t sz );


public:
	const vector< Address >& addresses() const
	{
		return addrs;
	}
	const vector< Body >& bodies() const
	{
		return bodies_;
	}
	const vector< Attachment >& attachments() const
	{
		return attachments_;
	}
	const Address & from() const
	{
		return from_;
	}

	const char * messageId() const
	{
		return messageId_;
	}
	const char * parseMessageId(PString & suffix, UINT32 &emailId, UINT32& type );
	const char* subj() const
	{
		return subj_;
	}
	const time_t date() const
	{
		return date_;
	}
	/*	const char* inReplyTo() const
	{
	return inReplyTo_;
	}*/
	void addHeader( const char* name, const char* value );
	const char* getHeader( const char* name );
	const char* getHeader( const char* name, const char* defaultValue )
	{
		const char* ret = getHeader( name );
		return ret ? ret : defaultValue;
	}
	const vector< CommMailHeader >& headers() const
	{
		return headers_;
	}


	static void forward( PBlock& ret, const char* from, const vector< Address >& to, const BYTE* p, size_t sz );

private:
	//void _genBoundary( PString& ret ) const;
	void _printBody( INT32& rand, PString& out ) const;
	//void _decodeBlock( PBlock& ret, const BYTE* p, size_t sz, const char* contentEncoding );
	void _loadBodies( const BYTE* p, const BYTE* pEnd, const char* contentType, const char* charset, const char* boundary, const char* contentEncoding, const char * attachName, const char * contentFormat );
	bool _findBodyContentType(const char * contentType);
};

#endif
