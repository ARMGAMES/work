#ifndef commproto_h_included
#define commproto_h_included

#define _COMMTCP0PREFIX "pyrtcp0://"
#define _COMMUDP0PREFIX "pyrudp0://"
#define _COMMTCP0PREFIXLEN 10
#define _COMMUDP0PREFIXLEN 10

struct CommTcp0ProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMTCP0PREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address, int port );
};

struct CommUdp0ProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMUDP0PREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address, int port );
};


//****************************************************************************

#define _COMMSSL0PREFIX "pyrssl0://"
#define _COMMSSL0PREFIXLEN 10

struct CommSsl0ProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMSSL0PREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address, int port );
};

//****************************************************************************

#define _COMMSSLPPREFIX "pyrsslP://"
#define _COMMSSLPPREFIXLEN 10

struct CommSslPProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMSSLPPREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address, int port );
};

#define _COMMCOMPOSITPREFIX "pyrcomposite://"
#define _COMMCOMPOSITREFIXLEN 15

struct CommCompositeProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMCOMPOSITPREFIX );
	}
	static void parseSocketAddr( PString& retAddr, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address );
};


#ifdef UDT_DEVELOPMENT

#define _COMMUDT0PREFIX "pyrudt0://"
#define _COMMUDT0PREFIXLEN 10

struct CommUdt0ProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMUDT0PREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address, int port );
};


#define _COMMUDTSSL0PREFIX "pyrudtssl0://"
#define _COMMUDTSSL0PREFIXLEN 13

struct CommUdtSSL0ProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMUDTSSL0PREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address, int port );
};

#endif


#ifdef PPOSIX
#define _COMMUNIXPREFIX "pyrunix://"
#define _COMMUNIXPREFIXLEN 10

struct CommUnixProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMUNIXPREFIX );
	}
	static void parseSocketAddr( PString& retAddr, const char* address );
	static void composeSocketAddr( PString& retAddr, const char* address );
};

#endif
#endif
