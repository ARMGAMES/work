#ifndef commsgml_h_included
#define commsgml_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppstream.h"
#include "ppi18n.h"

class CommSgmlWarnings
{
public:
	CommSgmlWarnings()
	{
	}
	virtual ~CommSgmlWarnings()
	{
	}

public:
	virtual void warning( int line, const char* txt )
	{
	}
};

class CommSgmlParser;
class CommSgmlTagAttributes
{
	friend class CommSgmlParser;

private:
	struct _Item
	{
		/* mutable */ bool found;//to warn if unused
		/* new */ char* name;
		/* new */ char* value;
	};

private:
	vector< _Item > items;

public:
	CommSgmlTagAttributes();
	CommSgmlTagAttributes( const CommSgmlTagAttributes& other )
	{
		for( auto&& item : other.items )
			_add( item.name, item.value );
	}
	CommSgmlTagAttributes( /* non-const */ CommSgmlTagAttributes& other, bool move /* MUST be true */ )//'moving constructor'
	{
		PASSERT4( move );
		items.swap( other.items );
	}
	~CommSgmlTagAttributes();

public:
	pair< bool, const char* > find( const char* name, const PStringEncoding* encString ) const;
	pair< bool, const char* > findIgnoreCase( const char* name, const PStringEncoding* encString ) const;

public:
	void warnAboutUnused( CommSgmlWarnings *sgmlWarnings, int line ) const;

public:
	void enumerate( vector< pair< const char*, const char* > >& buf ) const;
	void enumerate( vector< pair< PString, PString > >& buf ) const;

private:
	void _add( const char* name, const char* value /* can be NULL */ );

private://prohibited
	CommSgmlTagAttributes& operator =( const CommSgmlTagAttributes& );
};

class _CommSgmlEntities;
class _CommSgmlInputStream;
class CommSgmlParser
{
	friend class CommSgmlTagAttributes;

private:
	enum { _bufSz = 256 };

protected:
	const PStringEncoding* encString;

private:
	bool useUniChars;
	_CommSgmlEntities* sgmlEntities;
	PUNICHAR buf[ _bufSz ];
	PUNICHAR* bufPos;
	//bool _stopParse;//hack?

protected:
	CommSgmlWarnings *sgmlWarnings;
	int line;//warning purposes only

public:
	CommSgmlParser( const PStringEncoding* encString_, bool useUniCharacters = false, CommSgmlWarnings *sgmlWarnings_ = NULL );//NOTE: ANY encString will do for SGML; restrictions may apply for derivatives (like HTML) though
	void parse( PUniInputStream& uniStream );
	//void stopParse();
	//void recursiveContinueParse();
	virtual ~CommSgmlParser();

public://OVERRIDDABLES
	virtual void characters( const char* begin, const char* end ) {}//only one of characters() and uniCharacters() is called depending on useUniCharaters constructor param
	virtual void uniCharacters( const PUNICHAR* begin, const PUNICHAR* end ) {}//only one of characters() and uniCharacters() is called depending on useUniCharaters constructor param
	virtual void tag( const char* name, /* non-const; change allowed */ CommSgmlTagAttributes* attributes ) {}
	virtual void eof() {}

public://warning helpers
	void warning( const char* txt ) { if( sgmlWarnings ) sgmlWarnings->warning( line, txt ); }

private:
	/* inline */ void _character( PUNICHAR c );
	/* inline */ void _flush();
	/* inline */ void _tag( const char* name, CommSgmlTagAttributes* attributes );
	/* inline */ void _eof();
	void _flushCharacters();
	void _parse( _CommSgmlInputStream& s );

protected:
	void _addTagAttribute( CommSgmlTagAttributes& attributes, const char* name, const char* value /* can be NULL */ )
	{
		attributes._add( name, value );
	}

protected:
	static void _composeCharacter( PString& buf, PUNICHAR_RET c, bool attr, const PStringEncoding* encString );

private://prohibited
	CommSgmlParser( const CommSgmlParser& );
	CommSgmlParser& operator =( const CommSgmlParser& );
};

#endif
