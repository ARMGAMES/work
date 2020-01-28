// CBotsEvaluator.h: interface for the CBotsEvaluator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOTSEVALUATOR_H__D2407F6D_DEA7_425F_BE99_688641153E21__INCLUDED_)
#define AFX_BOTSEVALUATOR_H__D2407F6D_DEA7_425F_BE99_688641153E21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BOTS_VAR_INVALID 0
#define BOTS_VAR_REAL 1
#define BOTS_VAR_STRING 2

extern int _DEBUG_;

#include "ppinclude.h"
#include "ppstring.h"

#ifndef LPCTSTR
#define LPCTSTR const char *
#endif

class PStringExt : public PString
{
public:

	PStringExt()
		: PString()
	{
	}
	PStringExt( const PStringExt& other )
		: PString( ( const PString& )other )
	{
	}
	PStringExt( const char* s )
		: PString( s )
	{
	}
	PStringExt( const char* s, int sz )
		: PString( s, sz )
	{
	}
	PStringExt( const char* begin, const char* end )
		: PString( begin, end )
	{
	}

	PStringExt& operator=( const PStringExt& other )
	{
		assign( other.c_str() );
		return *this;
	}
	operator const char*() const
	{
		return c_str();
	}

	static inline bool _isSpace( char c )
	{
		return c == ' ' || c == '\t';
	}

	void TrimLeft()
	{
		//while(length()>0 && (_isSpace(c_str()[0]) || c_str()[0]=='\n' ||  c_str()[0]=='\r'))
		//	assign(c_str() + 1);

		if( length() )
		{
			const char* s = c_str();
			for(; ; )
				if( _isSpace( *s ) || *s == '\n' || *s == '\r' )
					++s;
				else
					break;

			if( s != c_str() )
				assign( s );
		}


	}
	void TrimRight()
	{
		//while(length()>0 && (_isSpace(c_str()[length()-1]) || c_str()[length()-1]=='\n' ||  c_str()[length()-1]=='\r'))
		//	cut(length()-1);
		size_t n = length();
		if( n > 0 )
		{
			for(; n > 0; n-- )
			{
				if( _isSpace( c_str()[n - 1] ) || c_str()[n - 1] == '\n' || c_str()[n - 1] == '\r' )
					continue;
				else
					break;
			}
			if( n < length() )
				cut( n );
		}
	}
	void Trim()
	{
		TrimLeft();
		TrimRight();
	}

	void Replace( const char* what, const char* withWhat );

	int CompareNoCase( const char* lpsz ) const
	{
		return compareIgnoreCase( c_str(), lpsz );
	}
	int Compare( const char* lpsz ) const
	{
		return compare( c_str(), lpsz );
	}

	PStringExt Mid( int nFirst, int nCount ) const
	{
		if( nFirst < 0 )
			nFirst = 0;
		if( nCount < 0 )
			nCount = 0;

		if( nFirst + nCount > length() )
			nCount = static_cast< int >( length() ) - nFirst;
		if( nFirst > static_cast< int >( length() ) )
			nCount = 0;

		if( nFirst == 0 && nFirst + nCount == length() )
			return *this;

		PStringExt dest;
		dest = c_str() + nFirst;
		if( nCount < dest.length() )
			dest.cut( nCount );
		return dest;
	}

	PStringExt Mid( int nFirst ) const
	{
		return Mid( nFirst, static_cast< int >( length() ) - nFirst );
	}

	PStringExt Left( int nCount ) const
	{
		if( nCount < 0 )
			nCount = 0;
		if( nCount >= length() )
			return *this;

		PStringExt dest;
		dest = c_str();
		if( nCount < dest.length() )
			dest.cut( nCount );
		return dest;
	}
	PStringExt Right( int nCount ) const
	{
		if( nCount < 0 )
			nCount = 0;
		if( nCount >= length() )
			return *this;

		PStringExt dest;
		dest = c_str() + length() - nCount;
		return dest;
	}

	int Find( char ch ) const
	{
		const char* c = strchr( c_str(), ch );
		if( !c )
			return -1;
		return static_cast< int >( c - c_str() );
	}

	int Find( char ch, int nStart ) const
	{
		if( nStart < 0 )
			return -1;
		const char* c = c_str();
		if( !c )
			return -1;
		if( strlen( c ) <= nStart )
			return -1;
		c = strchr( c_str() + nStart, ch );
		if( !c )
			return -1;
		return static_cast< int >( c - c_str() );
	}

	int Find( const char* ch ) const
	{
		const char* c = strstr( c_str(), ch );
		if( !c )
			return -1;
		return static_cast< int >( c - c_str() );
	}

	int Find( const char* ch, int nStart ) const
	{
		if( nStart < 0 )
			return -1;
		const char* c = c_str();
		if( !c )
			return -1;
		if( strlen( c ) <= nStart )
			return -1;
		c = strstr( c_str() + nStart, ch );
		if( !c )
			return -1;
		return static_cast< int >( c - c_str() );
	}

	int ReverseFind( char ch ) const
	{
		const char* c = strrchr( c_str(), ch );
		if( !c )
			return -1;
		return static_cast< int >( c - c_str() );
	}

	PStringExt& appendFloat( double x, const char* fmt_="%15.2f" )
	{
		char buf[ 128 ] = "";
		sprintf( buf, fmt_, x );
		int i = 0;
		for( i = 0; buf[i] && buf[i] <= ' ' && i < 127; i++ );
		if( i < 127 )
			append( buf + i );
		return *this;
	}
	void MakeLower()
	{
		toLower();
	}

	/*
	PStringExt& append( const PString& other )
	{
	append( other.c_str() );
	return *this;
	}
	PStringExt& append( const PStringExt& other )
	{
	append( other.c_str() );
	return *this;
	}
	*/
	PStringExt& append( const char* s )
	{
		PString::append( s );
		return *this;
	}

	PStringExt& append( const char* begin, const char* end )
	{
		PString::append( begin, end - begin );
		return *this;
	}

	PStringExt& append( const char* s, int sz )
	{
		PString::append( s, sz );
		return *this;
	}

	PStringExt& appendInt( int i )
	{
		PString::appendInt( i );
		return *this;
	}
	PStringExt& appendUint( UINT32 i )
	{
		PString::appendUint( i );
		return *this;
	}
	PStringExt& appendInt64( INT64 i )
	{
		PString::appendInt64( i );
		return *this;
	}
	PStringExt& appendUint64( UINT64 i )
	{
		PString::appendUint64( i );
		return *this;
	}
	PStringExt& appendHexInt( int i )
	{
		PString::appendHexInt( i );
		return *this;
	}
	PStringExt& appendHexInt64( UINT64 i )
	{
		PString::appendHexInt64( i );
		return *this;
	}

	/*
	int operator !=(const char *);
	int operator !=(const PStringExt &);

	int operator==(const PStringExt &) const
	{
	return 1;
	}

	// This function is required for built-in STL list functions like sort
	int operator<(const PStringExt &) const
	{
	return 0;
	}
	int operator>(const PStringExt &) const
	{
	return 0;
	}
	int operator<=(const PStringExt &) const
	{
	return 0;
	}
	int operator>=(const PStringExt &) const
	{
	return 0;
	}
	*/
};

struct PStringExtFunc : public binary_function< const class PStringExt&, const class PStringExt&, bool >
{
	bool operator()( const class PStringExt& l, const class PStringExt& r ) const
	{
		if( l.length() == 0 && r.length() == 0 )
			return false;
		if( l.length() == 0 )
			return true;
		if( r.length() == 0 )
			return false;
		return strcmp( l.c_str(), r.c_str() ) < 0;
	}
};

inline void mytrace( ... ) {}

int IsFloat( const char* s );

#define MAX_TOKENS 128
#define ERR 1.0e20f
#define PI1 0.0174532925199432955f
#define TOKEN_LEN 128


class CBotsEvaluatorValue
{
public:
	int m_type;
	int ern;
	float m_val;
	PStringExt m_strValue;

	CBotsEvaluatorValue()
	{
		m_val = 0;
		m_type = BOTS_VAR_REAL;
	}
	CBotsEvaluatorValue( CBotsEvaluatorValue* v )
	{
		m_val = v->m_val;
		m_type = v->m_type;
		;
		m_strValue = v->m_strValue;
		m_strValue.TrimLeft();
		m_strValue.TrimRight();
	}
	CBotsEvaluatorValue( float fVal )
	{
		m_val = fVal;
		m_type = BOTS_VAR_REAL;
	}
	CBotsEvaluatorValue( const char* sValue )
	{
		m_strValue = sValue;
		m_strValue.TrimLeft();
		m_strValue.TrimRight();
		m_type = BOTS_VAR_STRING;
		m_val = 0;
	}
	void Set( CBotsEvaluatorValue* v )
	{
		m_val = v->m_val;
		m_type = v->m_type;
		;
		m_strValue = v->m_strValue;
		m_strValue.TrimLeft();
		m_strValue.TrimRight();
	}
	void Set( const char* sValue )
	{
		m_strValue = sValue;
		m_strValue.TrimLeft();
		m_strValue.TrimRight();
		m_type = BOTS_VAR_STRING;
	}
	void Set( float fVal )
	{
		m_val = fVal;
		m_type = BOTS_VAR_REAL;
	}
	bool operator >( const CBotsEvaluatorValue& v2 ) const
	{
		switch( m_type )
		{
		case BOTS_VAR_REAL:
			if( v2.m_type != BOTS_VAR_REAL )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_val > v2.m_val;
		case BOTS_VAR_STRING:
			if( v2.m_type != BOTS_VAR_STRING )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_strValue.CompareNoCase( v2.m_strValue ) > 0;
		}
		return 0;
	}
	bool operator <( const CBotsEvaluatorValue& v2 ) const
	{
		switch( m_type )
		{
		case BOTS_VAR_REAL:
			if( v2.m_type != BOTS_VAR_REAL )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_val < v2.m_val;
		case BOTS_VAR_STRING:
			if( v2.m_type != BOTS_VAR_STRING )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_strValue.CompareNoCase( v2.m_strValue ) < 0;
		}
		return 0;
	}
	bool operator >=( const CBotsEvaluatorValue& v2 ) const
	{
		switch( m_type )
		{
		case BOTS_VAR_REAL:
			if( v2.m_type != BOTS_VAR_REAL )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_val >= v2.m_val;
		case BOTS_VAR_STRING:
			if( v2.m_type != BOTS_VAR_STRING )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_strValue.CompareNoCase( v2.m_strValue ) >= 0;
		}
		return 0;
	}
	bool operator <=( const CBotsEvaluatorValue& v2 ) const
	{
		switch( m_type )
		{
		case BOTS_VAR_REAL:
			if( v2.m_type != BOTS_VAR_REAL )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_val <= v2.m_val;
		case BOTS_VAR_STRING:
			if( v2.m_type != BOTS_VAR_STRING )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_strValue.CompareNoCase( v2.m_strValue ) <= 0;
		}
		return 0;
	}
	bool operator ==( const CBotsEvaluatorValue& v2 ) const
	{
		switch( m_type )
		{
		case BOTS_VAR_REAL:
			if( v2.m_type != BOTS_VAR_REAL )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_val == v2.m_val;
		case BOTS_VAR_STRING:
			if( v2.m_type != BOTS_VAR_STRING )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_strValue.CompareNoCase( v2.m_strValue ) == 0;
		}
		return 0;
	}
	bool operator !=( const CBotsEvaluatorValue& v2 ) const
	{
		switch( m_type )
		{
		case BOTS_VAR_REAL:
			if( v2.m_type != BOTS_VAR_REAL )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_val != v2.m_val;
		case BOTS_VAR_STRING:
			if( v2.m_type != BOTS_VAR_STRING )
			{
				mytrace( "Error\n" );
				return 0;
			}
			return m_strValue.CompareNoCase( v2.m_strValue ) != 0;
		}
		return 0;
	}
};


class CBotsEvaluatorVariable : public CBotsEvaluatorValue
{
public:
	//CBotsEvaluatorVariable();
	//virtual ~CBotsEvaluatorVariable();

	PStringExt m_sName;
	CBotsEvaluatorVariable* next;

	CBotsEvaluatorVariable( const char* sName, float fVal )
	{
		m_type = BOTS_VAR_REAL;
		//strcpy(m_sName,sName)   ;
		m_sName = sName;
		m_sName.TrimLeft();
		m_sName.TrimRight();
		this->m_val = fVal;
		next = 0;
	}

	CBotsEvaluatorVariable( const char* sName, const char* value )
	{
		m_type = BOTS_VAR_STRING;
		//strcpy(m_sName,sName)   ;
		m_sName = sName;
		m_strValue = value;
		m_sName.TrimLeft();
		m_sName.TrimRight();
		m_strValue.TrimLeft();
		m_strValue.TrimRight();
		next = 0;
	}

	CBotsEvaluatorVariable( CBotsEvaluatorVariable* v )
	{
		Copy( v );
		next = 0;
	}

	void Copy( const CBotsEvaluatorVariable* v )
	{
		m_type = v->m_type;
		m_sName = v->m_sName;
		m_strValue = v->m_strValue;
		m_sName.TrimLeft();
		m_sName.TrimRight();
		m_strValue.TrimLeft();
		m_strValue.TrimRight();
	}

	CBotsEvaluatorVariable(const CBotsEvaluatorVariable &other){copy(other);}
	CBotsEvaluatorVariable& operator=(const CBotsEvaluatorVariable& other){copy(other);return *this;}
	void copy(const CBotsEvaluatorVariable &other)
	{
		Copy(&other);
	}

};

class CBotsEvaluatorVariableList
{
public:
	//	CBotsEvaluatorVariableList();
	//	virtual ~CBotsEvaluatorVariableList();

	CBotsEvaluatorVariable* m_head;
public:
	CBotsEvaluatorVariableList()
	{
		m_head = 0;
	}
	~CBotsEvaluatorVariableList()
	{
		CBotsEvaluatorVariable* d;
		while( m_head != 0 )
		{
			d = m_head;
			m_head = m_head->next;
			delete d;
		}
	}
	void Clone( CBotsEvaluatorVariableList* pVList )
	{
		CBotsEvaluatorVariable* d;
		for( d = pVList->m_head; d != 0; d = d->next )
		{
			Add( d );
		}
	}

	CBotsEvaluatorVariable* Add( float fVal, const char* sName )
	{
		CBotsEvaluatorVariable* dd;
		if( GetVariable( sName, &dd ) )
		{
			dd->Set( fVal );
			return dd;
		}
		dd = new CBotsEvaluatorVariable( sName, fVal );
		CBotsEvaluatorVariable* d;

		if( m_head == 0 )
			m_head = dd;
		else
		{
			for( d = m_head; d->next != 0; d = d->next );
			d->next = dd;
		}
		return dd;
	}

	CBotsEvaluatorVariable* Add( CBotsEvaluatorVariable* v ) // b
	{
		CBotsEvaluatorVariable* d;
		if( GetVariable( v->m_sName, &d ) )
		{
			d->Copy( v );
			return d;
		}
		d = new CBotsEvaluatorVariable( v );
		d->next = m_head;
		m_head = d;
		return d;
	}

	CBotsEvaluatorVariable* Add( const char* sName, float fVal ) // b
	{
		CBotsEvaluatorVariable* d;
		if( GetVariable( sName, &d ) )
		{
			d->Set( fVal );
			return d;
		}
		d = new CBotsEvaluatorVariable( sName, fVal );
		d->next = m_head;
		m_head = d;
		return d;
	}

	CBotsEvaluatorVariable* Add( const char* sName, const char* value )
	{
		CBotsEvaluatorVariable* d;
		if( GetVariable( sName, &d ) )
		{
			d->Set( value );
			return d;
		}
		d = new CBotsEvaluatorVariable( sName, value );

		d->next = m_head;
		m_head = d;
		return d;
	}

	CBotsEvaluatorVariable* AddUnique( const char* sName )
	{
		CBotsEvaluatorVariable* d;
		for( d = m_head; d != 0; d = d->next )
		{
			if( !d->m_sName.CompareNoCase( sName ) )
				return d;
		}
		return Add( sName, "" );
	}

	void ShowVars()
	{
		CBotsEvaluatorVariable* d;
		mytrace( "================= ShowVars ==================\n" );
		d = m_head;
		while( d != 0 )
		{
			mytrace( "pVar= %p [%s] = %12.4f\n", d, ( const char* )d->m_sName, d->m_val );
			d = d->next;
		}
	}

	void ShowVar( char* sName )
	{
		CBotsEvaluatorVariable* d;

		if( GetVariable( sName, &d ) )
			mytrace( "[%s] = %f\n", ( const char* )d->m_sName, d->m_val );
		else
			mytrace( "var [%s] not find\n", sName );
	}

	void ShowVar_x( char* sName )
	{
		CBotsEvaluatorVariable* d;
		int vi;

		d = m_head;
		while( d != 0 )
		{
			if( !d->m_sName.Compare( sName ) )
			{
				vi = static_cast< int >( d->m_val );
				mytrace ( "[%s] = @%04x\n", ( const char* )d->m_sName, vi );
				return;
			}
			d = d->next;
		}
		mytrace( "var [%s] not find\n", sName );
	}
	int GetVariable( const char* sName, CBotsEvaluatorVariable** pVar )
	{
		CBotsEvaluatorVariable* d;

		for( d = m_head; d != 0; d = d->next )
		{
			if( !strcmp( sName, d->m_sName ) )
			{
				*pVar = d;
				return 1;
			}
		}
		mytrace( "var [%s] not find\n", sName );
		return 0;
	}

	const char* GetValue( const char* sName )
	{
		CBotsEvaluatorVariable* d;

		for( d = m_head; d != 0; d = d->next )
		{
			if( !strcmp( sName, d->m_sName ) )
			{
				return d->m_strValue;
			}
		}
		//mytrace("var [%s] not find\n",sName);
		return 0;
	}

};

class CBotsEvaluatorToken
{
public:
	char pritype;
	char word[TOKEN_LEN+1];
};

typedef class CBotsEvaluatorToken* PCBotsEvaluatorToken;

//--------------------- Lexical pritype --------------------
#define L_CONST     0	//	constants
#define L_STRING    1	//	constants string
#define L_IDENT     2	//	identifers
#define L_OTHER     3	//	others

//--------------------- prior pritype  ---------------------

#define E_UNKNOWN     -999 // error

#define E_COMM     -4	//	,
#define E_STRING   -3	//	constants string
#define E_CONST    -2	//	constants
#define E_IDENT    -1	//	identifers

#define E_EQ	     0	//	=
#define E_OR	     1	//	||
#define E_AND	     2	//	&&
#define E_NOT	     3	//	!
#define E_EQON       4	//	==   !=
#define E_UNEQ       5	//	<   >	<=   >=
#define E_BOR	     6	//	|
#define E_BAND       7	//	&
#define E_BXOR       8	//	^
#define E_BNOT       9	//	~
#define E_NEG	    10	//	negative(Valuation)
#define E_ADDSUB    11	//	+  -
#define E_MULDIV    12	//	*  /
#define E_BRACE     13	//	(  )

#define E_POW	    20	//	pow   **
#define E_SQRT      21	//	sqrt
#define E_LN	    22	//	ln
#define E_LOG10     23	//	log10
#define E_EXP	    24	//	exp
#define E_SIN	    25	//	sin
#define E_COS	    26	//	cos
#define E_TAN	    27	//	tan
#define E_ASIN      28	//	asin
#define E_ACOS      29	//	acos
#define E_ATAN      30	//	atan
#define E_ATAN2     31	//	atan2
#define E_ABS	    32	//	abs
#define E_INT	    33	//	int
#define E_SQR	    34	//	sqr

#define E_FUNC	    120	//	function

//	error label
//	 0: no error
//	 1: syntax error, illegal operator
//	 2: formal paramater ignored
//	 3: zero divisor
//	 4: syntax error, missing ")"
//	 5: syntax error, missing "("
//	 6: illegal operator: pow(0,0)
//	 7: illegal operator: pow(x<0,y!=int)
//	 8: illegal operator: sqrt(x<0)
//	 9: illegal operator: ln(x<=0)
//	10: illegal operator: log(x<=0)
//	11: illegal operator: asin(abs(x)>1)
//	12: illegal operator: acos(abs(x)>1)
//	13: illegal operator: atan2(x=0,y=0)
//	14: illegal calculate: no label
//	15: function not found
//	16: function running error

class CBotsBTreeNode
{
public:
	CBotsEvaluatorToken* m_pToken;
	CBotsBTreeNode* m_pLeft;
	CBotsBTreeNode* m_pRight;

	CBotsBTreeNode( CBotsEvaluatorToken* pToken )
	{
		m_pToken = pToken;
		m_pLeft = 0;
		m_pRight = 0;
	}
};

typedef class CBotsBTreeNode* PCBotsBTreeNode;

class CBotsEvaluator
{
	CBotsEvaluatorVariableList* m_pVList;
	int ern;

	PCBotsBTreeNode m_pRoot;
	int m_nTokenCount;

	PCBotsEvaluatorToken m_pCurrToken;
	PCBotsEvaluatorToken m_TokenArray[MAX_TOKENS];

	char brace;
	int counter;

	PCBotsBTreeNode express(),term(),power(),factor();
	PCBotsBTreeNode function(),orexprs();
	PCBotsBTreeNode andexprs(),notexprs(),eqonexprs(),uneqexprs();
	PCBotsBTreeNode bandexprs(),borexprs(),bxorexprs(),bnotexprs();

	int Lexical( char* expre );
	void Valuation( PCBotsBTreeNode p, CBotsEvaluatorValue* pv );
	int prior( char* s );
	int funct( char* s );

	void PrintOrder( PCBotsBTreeNode ptr );

	int SeparateCalculator( char* expre );
	int BuildTree();

	void PrintToken( char* message, PCBotsBTreeNode pl );
	CBotsEvaluatorValue m_result;

public:
	CBotsEvaluator()
	{
		for( int i = 0; i < MAX_TOKENS; i++ )
			m_TokenArray[i] = new CBotsEvaluatorToken;

		brace = 0;
		m_nTokenCount = 0;
		ern = 0;
		m_pRoot = 0;
	}
	~CBotsEvaluator()
	{
		for( int i = 0; i < MAX_TOKENS; i++ )
			delete m_TokenArray[i];
		FreeNode( m_pRoot );
		m_pRoot = 0;
	}

	void FreeNode( PCBotsBTreeNode ptr )
	{
		if( !ptr )
			return;
		if( ptr->m_pLeft )
			FreeNode( ptr->m_pLeft );
		if( ptr->m_pRight )
			FreeNode( ptr->m_pRight );
		delete ptr;
	}

	void SetVariableList( CBotsEvaluatorVariableList* vList );

	int Calculate( const char* buf );
	int Prepare( const char* buf );
	int Run();

	bool Evaluate( CBotsEvaluatorVariableList* vList );

	void PrintTokenList( int l=-1 );
	void PrintTree( PCBotsBTreeNode ptr=0 );


	CBotsEvaluatorValue& GetResult()
	{
		return m_result;
	}

	void Clear();
	const char* GetError( int err ) const;
};

#endif // !defined(AFX_BOTSEVALUATOR_H__D2407F6D_DEA7_425F_BE99_688641153E21__INCLUDE_)
