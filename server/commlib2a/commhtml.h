#ifndef commhtml_h_included
#define commhtml_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "commurl.h"
#include "ppi18n.h"
#include "ppunistring.h"
#include "ppcontainer.h"

//------------------------------------------------------------------------------

class CommHtmlTemplate // DEPRECATED for server-side e-mail templates,  see below
{
private:
	struct _Item : PString
	{
		char type; // 'T'ext, 'R'eference

		_Item( char type_, const char* s, int sz )
			: PString( s, sz ), type( type_ )
		{
		}
	};

private:
	typedef vector< _Item > _Entity;
	typedef PStringMap< _Entity > _EntityMap;
	typedef _EntityMap::iterator iterator;

	_EntityMap entities;

protected:
	const PStringEncoding* const encString;

public:
	CommHtmlTemplate( const PStringEncoding* encString_ = NULL )
		: encString( encString_ )
	{
	}
	virtual ~CommHtmlTemplate()
	{
	}

public:
	void load( const char* fName );
	void load( FILE* f );
	void load( const BYTE* p, size_t sz );

public:
	void merge( CommHtmlTemplate& other, bool overwrite );

public:
	void clear()
	{
		entities.clear();
	}

private:
	bool _compose( PString& buf, iterator iter );

public:
	bool magic( PString& buf, const char* name );
	virtual void spell( PString& buf, const char* name ) {}

public:
	void composeAll( PString& buf );

public: // HELPER
	static void htmlEncode( PString& ret, const char* s ) // NOTE: APPENDS
	{
		CommUrl::htmlEncode( ret, s );
	}

private:
	void _parse( const BYTE* p, size_t sz );
	void _characters( iterator iter, const char* s, int n );

private: // prohibit default copy constructor & assignment operator
	CommHtmlTemplate( const CommHtmlTemplate& other );
	CommHtmlTemplate& operator=( const CommHtmlTemplate& other );
};

//------------------------------------------------------------------------------

class _CommHtmlEntities;
class _CommHtmlByteStream;
class _CommHtmlParserStream;
class CommHtmlParser
{
private:
	_CommHtmlEntities* htmlEntities;
	_CommHtmlByteStream* byteStream;
	_CommHtmlParserStream* parserStream;
	const BYTE* p;
	size_t sz;
	char buf[ 256 ];
	char* bufPos;
	bool _stopParse;//hack?
	bool doUppercase;

public:
	CommHtmlParser( const BYTE* p_, size_t sz_ );
	void parse();
	void stopParse();
	void recursiveContinueParse();
	void setUppercasing( bool doUppercase_ ) { doUppercase = doUppercase_; }
	virtual ~CommHtmlParser();

public://OVERRIDDABLES
	virtual void characters( const char* begin, const char* end ) {}
	virtual void tag( const char* name, const PStringMap< PString >* attributes ) {}
	virtual void eof() {}

private:
	/* inline */ void _character( char c );
	/* inline */ void _flush();
	/* inline */ void _tag( const char* name, const PStringMap< PString >* attributes );
	/* inline */ void _eof();
	void _parse( _CommHtmlParserStream& s );
};

///////////////////////  I18N AWARE CommHtmlTmpl //////////////////////////

// PYR-29744
struct PUinStringCmp : public binary_function< PUniString, PUniString, bool >
{
public:
	bool operator() ( const PUniString& a, const PUniString& b ) const
	{
		return PUniString::compare(a.c_str(), b.c_str()) < 0;
	}
};

class CommHtmlTemplateEx
{
private:
	struct _Item
	{
		char type;//'T'ext, 'R'eference
		PUniString data;

		_Item()
		{
		}
		_Item( char type_, const PUNICHAR* s, int sz )
			: type( type_ ),
			data( s, sz )
		{
		}
		_Item& operator =( const _Item& other )//only due to g++ 2.7.2.1 linking problem
		{
			type = other.type;
			data = other.data;
			return *this;
		}
	};

private:
	map< PUniString, vector< _Item >, PUinStringCmp > entities; // PYR-29744
	typedef map< PUniString, vector< _Item >, PUinStringCmp >::const_iterator ConstEntitiesIterator;
	typedef map< PUniString, vector< _Item >, PUinStringCmp >::iterator EntitiesIterator;

protected:
	/* new */ PStringEncoding* encString;

public:
	CommHtmlTemplateEx();
	void init(const PStringEncoding* encString_ );
	CommHtmlTemplateEx( const PStringEncoding* encString_ );
	// old load() methods are deprecated
	void load( PUniInputStream& f );
	void load( PInputStream& f, const PStreamEncoding& enc );
	void load( const PUNICHAR* p, size_t sz );
	virtual ~CommHtmlTemplateEx();

	bool magic( PString& buf, const char* name );
	virtual void spell( PString& buf, const char* name ) {}

	//HELPER
	static void htmlEncode( PString& ret, const char* s )//NOTE: APPENDS
	{
		CommUrl::htmlEncode( ret, s );
	}

private:
	void _parse( const PUNICHAR* p, size_t sz );
	void _characters( EntitiesIterator& curEntity, const PUNICHAR* s, int n );

private://prohibited
	CommHtmlTemplateEx( const CommHtmlTemplateEx& other );
	CommHtmlTemplateEx& operator =( const CommHtmlTemplateEx& other );
};

#endif
