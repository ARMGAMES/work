#ifndef comextemail_h_included
#define	comextemail_h_included

#include "commext.h"




class CommExtEmail 
{
	enum {MaxLineLength = 990};
public:
	bool hasError;
	bool ignoreAllErrors;
	bool noDebugPrint;
	
	struct Attribute
	{
		PString value;
		INT8	delim;
		Attribute()
		{
			delim = 0;
		}
		void compose(CommMsgBody & body) const
		{
			body.composeString(value)
				.composeINT8(delim);
		}
		void parse(CommMsgParser &parser)
		{
			parser.parseStringP(value)
				  .parseINT8(delim);  
		}

	};
	struct Header
	{
		PString value;
		CommExtPairVector<Attribute> attr;
		void compose(CommMsgBody & body) const
		{
			body.composeString(value);
			attr.compose(body);
		}
		void parse(CommMsgParser &parser)
		{
			parser.parseStringP(value);
			attr.parse(parser);
		}
	};

	class Address
	{
	public:
		PString address;
		PString normalAddress;
		void compose(CommMsgBody &body) const
		{
			body.composeString(address.c_str());
			body.composeString(normalAddress.c_str());
		}
		void parse(CommMsgParser &parser)
		{
			parser.parseStringP(address);
			parser.parseStringP(normalAddress);
		}
	};

	class Data
	{
	public:
		CommExtPairVector <Header> headers;
		CommExtPtrVector <Data> data;
		PString emailbody;

		void compose(CommMsgBody & body) const
		{
			body.composeString(emailbody);
			data.compose(body);
			headers.compose(body);
		}
		void parse(CommMsgParser &parser)
		{
			parser.parseStringP(emailbody);
			data.parse(parser);
			headers.parse(parser);
		}
	};

	CommExtPairVector <Address> addresses;
//	CommExtPairVector <Header> headers;
//	CommExtPtrVector <Data> data; 
	Data data;

	CommExtEmail()
	{
		noDebugPrint = false;
		hasError = false;
		ignoreAllErrors = false;
	}
	void load(const char* source);
	void load(const BYTE* source,size_t sz);
	void loadHeaders( const char*& source);
	size_t loadHeaders( const BYTE* source, size_t sz);
	void _loadHeader( const char *& p, CommExtPairVector <Header> &h);
	void _loadHeader( const char *& p);
	void _loadBodies(const char* source, Data & d, size_t sz);
	void _printHeaders( PString & out, CommExtPairVector <Header> &h);
	void _printData(  PString & out, Data & d);
	void printHeaders( PString & out);
	void print(PString &out);
	bool findBody(const char * &source, const char * type, PString & encoding, PString & charset, bool & formatFlowed, const Data &d) const;
};



#endif


