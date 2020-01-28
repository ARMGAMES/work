#ifndef ppinifile_h_included
#define ppinifile_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppstream.h"
#include "ppi18n.h"

class PIniFile//DEPRECATED IN FAVOR OF PIniFile2
{
public:
	class Item
	{
	public:
		PString name;
		PString value;

	public:
		Item() {}
		Item( const char* name_, const char* value_ )
			: name( name_ ), 
			value( value_ )
		{
		}
	};//Item
	

	class Section
	{
	public:
		PString name;
		vector< Item > items;

	public:
		Section() {}
		Section( const char* name_ )
			: name( name_ )
		{
		}

		const char* getProperty( const char* name ) const;
		void setProperty( const char* name, const char* value );
		void eraseProperty( const char* name );
		const char* getProperty( const char* name_, const char* defaultValue ) const
		{
			const char* ret = getProperty( name_ );
			return ret ? ret : defaultValue;
		}
		int getIntProperty( const char* name, int defaultValue ) const;
		INT64 getInt64Property( const char* name, INT64 defaultValue ) const;
	private:
		vector< Item > _macros;
		const char* _getMacros( const char* name ) const;
		friend class PIniFile;

	};//Section

public:
	vector< Section > sections;

public:
	PIniFile()
	{
	}
	PIniFile( const char* fileName )
	{
		_load( fileName );
	}
	void load( const char* fileName ) { _load( fileName ); }
	void load( const BYTE* p, size_t sz ) { _load( p, sz ); }
	void save( const char* fileName ) const;
	void save( PString& dst ) const;

	Section* getSection( const char* secName );
	const Section* getSection( const char* secName ) const;
	Section* addSection( const char* secName );
	void eraseSection( const char* secName );
	void setSection( const Section& sec );

private:
	PString _dir;

	void _loadLine( vector< Section >::iterator& section, int line, const char* ln );
	void _load( const char* fileName );
	void _loadInclude(vector< Section >::iterator& section, const char* fileName );
	void _load( const BYTE* p, size_t sz );

	void _includeMacros(vector< Section >::iterator& section, PString &res, const char * data);
	const char* _getMacros(vector< Section >::iterator& section, const char* name ) const;

};//PIniFile

class PIniFileEx
{
public:
	class Item
	{
	public:
		PString name;
		PString value;

	public:
		Item() {}
		Item( const char* name_, const char* value_ )
			: name( name_ ), 
			value( value_ )
		{
		}
	};//Item

	class Section
	{
	private:
		PIniFileEx* parent;

	public:
		PString name;
		vector< Item > items;

	public:
		/* Section()
		{
		parent = 0;
		}
		Section( const char* name_ )
		: name( name_ )
		{
		parent = 0;
		}*/

	private:
		friend class PIniFileEx;
		Section( PIniFileEx& parent_ )
			: parent( &parent_ )
		{
		}
		Section( PIniFileEx& parent_, const char* name_ )
			: parent( &parent_ ),
			name( name_ )
		{
		}

	public:
		const char* getProperty( const char* name ) const;
		void setProperty( const char* name, const char* value );
		void eraseProperty( const char* name );
		const char* getProperty( const char* name_, const char* defaultValue ) const
		{
			const char* ret = getProperty( name_ );
			return ret ? ret : defaultValue;
		}
		int getIntProperty( const char* name, int defaultValue ) const;
		INT64 getInt64Property( const char* name, INT64 defaultValue ) const;
	};//Section

public:
	vector< Section > sections;

private:
	friend class PIniFileEx::Section;
	/* new */ PStringEncoding* encString;

public:
	PIniFileEx();
	PIniFileEx( const PStringEncoding& encString_ )
	{
		encString = encString_.clone();
	}
	PIniFileEx( const PIniFileEx& from_ );
	~PIniFileEx()
	{
		delete encString;
	}
	void load( PUniInputStream& f );
	void load( PInputStream& f, const PStreamEncoding& enc );
	void load( const char* fileName );
	void save( const char* fileName ) const;
	void save( PUniOutputStream& f ) const;

	Section* getSection( const char* secName );
	const Section* getSection( const char* secName ) const;
	Section* addSection( const char* secName );
	void eraseSection( const char* secName );
	void setSection( const Section& sec );

private:
	void _loadLine( vector< Section >::iterator& section, int line, const PUNICHAR* ln, const PUNICHAR* lnEnd );
};//PIniFileEx

#endif
