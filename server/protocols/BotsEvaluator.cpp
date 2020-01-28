// BotsEvaluator.cpp: implementation of the CBotsEvaluator class.
//
//////////////////////////////////////////////////////////////////////

#include "BotsEvaluator.h"
#include "math.h"

int _DEBUG_ = 0;


int IsFloat( LPCTSTR s )
{
	if( *s != '-' && *s != '.' && ( *s > '9' || *s < '0' ) )
		return 0;
	s++;
	while( *s != 0 )
	{
		if( *s != '.' && ( *s > '9' || *s < '0' ) )
			return 0;
		s++;
	}
	return 1;
}

void PStringExt::Replace( const char* what, const char* withWhat )
{
	PString str = c_str();
	while( const char* p = strstr( str.c_str(), what ) )
	{
		PString newStr = str;
		newStr.cut( p - str.c_str() );
		newStr.append( withWhat );
		newStr.append( p + strlen( what ) );
		str = newStr;
	}
	assign( str );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CBotsEvaluator::SetVariableList( CBotsEvaluatorVariableList* vList )
{
	m_pVList = vList;
}

int CBotsEvaluator::SeparateCalculator( char* expre )
{
	m_nTokenCount = 0;
	ern = 0;
	brace = 0;

	int n = Lexical( expre );

	if( ern != 0 )
		return ern;

	m_pCurrToken = m_TokenArray[++m_nTokenCount];

	PrintTokenList( m_nTokenCount + 1 );

	return ern;
}

int CBotsEvaluator::BuildTree()
{
	ern = 0;
	m_pRoot = orexprs();

	if( _DEBUG_ )
		PrintTree( m_pRoot );
	return ern;
}

int CBotsEvaluator::Run()
{
	m_result.m_val = 0;
	m_result.m_type = BOTS_VAR_INVALID;

	if( !m_pRoot )
		return ern;

	ern = 0;

	Valuation( m_pRoot, &m_result );
	//if (ern==0)
	//{
	//}
	return ern;
}

int CBotsEvaluator::Lexical( char* expre )
{
	char c, digit, letter, what = -1;
	int k = 0, i, l;

	for( l = 0; l < MAX_TOKENS; l++ )
	{
		m_TokenArray[l]->pritype = NULL;

		for( i = 0; i <= TOKEN_LEN; i++ )
			m_TokenArray[l]->word[i] = NULL;
	}

	l = 0;
	i = 0;
	while( ( c = *expre ) != '\0' )
	{
		expre++;
		if( c == '\"' )
		{
			k = 0;
			m_TokenArray[++l]->pritype = L_STRING;

			while( ( c = *expre ) != '\0' )
			{
				expre++;
				if( c == '\"' )
					break;
				if( k < TOKEN_LEN )
					m_TokenArray[l]->word[k++] = c;

			}
			continue;
		}

		if( c == ' ' || c == '\t' )
			continue;        // skip while space

		digit = c >= '0' && c <= '9';
		if( what == 16 )
			digit = c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';

		letter = c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c == '?';

		if( what == 16 && digit )
		{
			if( k < TOKEN_LEN )
				m_TokenArray[l]->word[k++] = c;
			what = 16;
		}
		else if( what != 1 && ( digit || c == '.' ) )
		{
			if( what != 0 )
			{
				k = 0;
				m_TokenArray[++l]->pritype = L_CONST;
			}					// get constant
			if( k < 8 )
				m_TokenArray[l]->word[k++] = c;
			what = 0;
		}
		else if( letter || what == 1 && digit )
		{
			if( what != 1 )
			{
				k = 0;
				m_TokenArray[++l]->pritype = L_IDENT;
			}					// get identifer
			if( k < TOKEN_LEN )
				m_TokenArray[l]->word[k++] = c;

			what = 1;
		}
		else
		{
			if( ( what == 3 && c == '*' ) || ( what == 4 && c == '|' ) || ( what == 5 && c == '&' )
				|| ( what >= 6 && what <= 9 && c == '=' ) )
			{
				m_TokenArray[l]->word[1] = c;
			}
			else
			{
				m_TokenArray[++l]->word[0] = c;
			}

			m_TokenArray[l]->pritype = L_OTHER;

			what = 2;
			if( c == '*' )
				what = 3;
			if( c == '|' )
				what = 4;
			if( c == '&' )
				what = 5;
			if( c == '=' )
				what = 6;
			if( c == '!' )
				what = 7;
			if( c == '>' )
				what = 8;
			if( c == '<' )
				what = 9;
			if( c == '~' )
				what = 10;
			if( c == '^' )
				what = 11;
			if( c == '@' )   //  heximal digit
			{
				k = 1;
				m_TokenArray[l]->pritype = L_CONST;
				what = 16;
			}
		}					// get operator
		digit = letter = 0;
	}
	for( i = 1; i <= l; i++ )
	{
		k = m_TokenArray[i]->pritype;
		if( k == L_OTHER )
		{
			m_TokenArray[i]->pritype = prior( m_TokenArray[i]->word );
			if( ern != 0 )
				break;
		}
		else if( k == L_IDENT )
			m_TokenArray[i]->pritype = funct( m_TokenArray[i]->word );
		else if( k == L_STRING )
			m_TokenArray[i]->pritype = E_STRING;
		else
			m_TokenArray[i]->pritype = E_CONST;
	}

	if( ern == 0 )
	{
		if( brace == 0 )
			return l;
		else if( brace > 0 )
			ern = 5;
		else
			ern = 4;
	}
	return -999;
}

void CBotsEvaluator::PrintTokenList( int l0 )
{
	int l = l0;
	if( l < 0 )
		l = m_nTokenCount + 1;

	int i;
	for( i = 0; i <= l; i++ )
	{
		mytrace( " number=%2d pritype=%2d expre=[%s]\n", i, m_TokenArray[i]->pritype, m_TokenArray[i]->word );
	}
}

int CBotsEvaluator::prior( char* s )
{
	switch( s[0] )
	{
	case '=':
		return E_EQON; // return (s[1]=='=') ? E_EQON : E_EQ;
	case '|':
		return ( s[1] == '|' ) ? E_OR : E_BOR;
	case '&':
		return ( s[1] == '&' ) ? E_AND : E_BAND;
	case '~':
		return E_BNOT;
	case '^':
		return E_BXOR;
	case '!':
		return ( s[1] == '=' ) ? E_EQON : E_NOT;
	case '<':
	case '>':
		return E_UNEQ;
	case ',':
		return E_COMM;
	case '(':
		brace++;
		return E_BRACE;
	case ')':
		brace--;
		return E_BRACE;
	case '+':
	case '-':
		return E_ADDSUB;
	case '/':
		return E_MULDIV;
	case '*':
		return ( s[1] == '*' ) ? E_POW : E_MULDIV;
	default :
		ern = 1;
	}
	return E_UNKNOWN;
}

int CBotsEvaluator::funct( char* s )
{
	char i;
	static char function[15][6] =
	{
		"pow","sqrt","log","log10","exp","sin","cos",
		"tan","asin","acos","atan","atan2","abs","int","sqr"
	};

	for( i = 0; i < 15; i++ )
		if( strcmp( s, function[i] ) == 0 )
			return( E_POW + i );

	if( !stricmp( s, "and" ) )
	{
		s[0] = s[1] = '&';
		return E_AND;
	}
	else if( !stricmp( s, "or" ) )
	{
		s[0] = s[1] = '|';
		return E_OR;
	}

	return( E_IDENT );
}

PCBotsBTreeNode CBotsEvaluator::orexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken _or;

	pl = andexprs();

	while( m_pCurrToken->pritype == E_OR )
	{
		_or = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = andexprs();
		p = new CBotsBTreeNode( _or );
		p->m_pLeft = pl;
		p->m_pRight = pr;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::andexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken _and;
	pl = notexprs();
	while( m_pCurrToken->pritype == E_AND )
	{
		_and = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = notexprs();
		p = new CBotsBTreeNode( _and );
		p->m_pLeft = pl;
		p->m_pRight = pr;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::notexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken _not;
	pl = eqonexprs();
	while( m_pCurrToken->pritype == E_NOT )
	{
		_not = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = eqonexprs();
		p = new CBotsBTreeNode( _not );
		p->m_pLeft = pl;
		p->m_pRight = pr;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::eqonexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken eqon;
	pl = uneqexprs();
	while( m_pCurrToken->pritype == E_EQON )
	{
		eqon = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = uneqexprs();
		p = new CBotsBTreeNode( eqon );
		p->m_pLeft = pl;
		p->m_pRight = pr;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::uneqexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken uneq;
	pl = borexprs();
	//		  PrintToken("In uneqexprs pl_0 : ",pl);
	while( m_pCurrToken->pritype == E_UNEQ )
	{
		uneq = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = borexprs();
		//		  PrintToken("In uneqexprs pr : ",pr);
		p = new CBotsBTreeNode( uneq );
		p->m_pLeft = pl;
		p->m_pRight = pr;
		pl = p;
		//		  PrintToken("In uneqexprs pl : ",pl);
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::borexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken uneq;
	pl = bandexprs();
	while( m_pCurrToken->pritype == E_BOR )
	{
		uneq = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = bandexprs();
		p = new CBotsBTreeNode( uneq );
		p->m_pRight = pr;
		p->m_pLeft = pl;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::bandexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken uneq;
	pl = bxorexprs();
	while( m_pCurrToken->pritype == E_BAND )
	{
		uneq = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = bxorexprs();
		p = new CBotsBTreeNode( uneq );
		p->m_pRight = pr;
		p->m_pLeft = pl;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::bxorexprs()
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken uneq;
	pl = bnotexprs();
	while( m_pCurrToken->pritype == E_BXOR )
	{
		uneq = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = bnotexprs();
		p = new CBotsBTreeNode( uneq );
		p->m_pRight = pr;
		p->m_pLeft = pl;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::bnotexprs()
{
	PCBotsBTreeNode p, pl;
	PCBotsEvaluatorToken bnot;

	if( m_pCurrToken->pritype == E_BNOT )
	{
		bnot = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pl = bnotexprs();
		p = new CBotsBTreeNode( bnot );
		p->m_pLeft = pl;
	}
	else
		p = express();
	return p;
}

void CBotsEvaluator::PrintToken( char* message, PCBotsBTreeNode pl )
{
	mytrace( "%20s a: %p l=%p r=%p pty=%d word=[%s]", message,
		pl, pl->m_pLeft, pl->m_pRight,
		pl->m_pToken->pritype, pl->m_pToken->word );
}


PCBotsBTreeNode CBotsEvaluator::express()				   //  +,-
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken addop;


	pl = term();
	while( m_pCurrToken->pritype == E_ADDSUB )
	{
		addop = m_pCurrToken;

		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = term();
		p = new CBotsBTreeNode( addop );
		p->m_pRight = pr;
		p->m_pLeft = pl;
		pl = p;
	}
	return pl;
}


PCBotsBTreeNode CBotsEvaluator::term()				   // for *,/
{
	PCBotsBTreeNode p, pl, pr;
	PCBotsEvaluatorToken multop;
	pl = factor();
	while( m_pCurrToken->pritype == E_MULDIV )
	{
		multop = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = factor();
		p = new CBotsBTreeNode( multop );
		p->m_pLeft = pl;
		p->m_pRight = pr;
		pl = p;
	}
	return pl;
}

PCBotsBTreeNode CBotsEvaluator::factor()
{
	PCBotsBTreeNode p, pl;
	PCBotsEvaluatorToken fname, minus;
	if( m_pCurrToken->pritype == E_BRACE && m_pCurrToken->word[0] == '(' ) // skip "("
	{
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		p = orexprs();
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
	}
	else if( m_pCurrToken->pritype >= E_POW ) // function
	{
		fname = m_pCurrToken;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pl = function();
		p = new CBotsBTreeNode( fname );
		p->m_pLeft = pl;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
	}
	else if( m_pCurrToken->pritype == E_ADDSUB && m_pCurrToken->word[0] == '-' ) // - var
	{
		minus = m_pCurrToken;
		m_pCurrToken->pritype = E_NEG;
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pl = factor();
		p = new CBotsBTreeNode( minus );
		p->m_pLeft = pl;
	}
	else
	{
		p = new CBotsBTreeNode( m_pCurrToken );
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
	}

	return p;
}

PCBotsBTreeNode CBotsEvaluator::function()
{
	PCBotsBTreeNode p, pl, pr;
	m_pCurrToken = m_TokenArray[++m_nTokenCount];
	pl = p = express();
	while( m_pCurrToken->word[0] == ',' )
	{
		m_pCurrToken = m_TokenArray[++m_nTokenCount];
		pr = express();
		p->m_pRight = pr;
		p = pr;
	}
	return pl;
}

void CBotsEvaluator::Valuation( PCBotsBTreeNode p, CBotsEvaluatorValue* pv )
{
	float val1, val2;
	int vi, vii;
	CBotsEvaluatorVariable* pVar;

	CBotsEvaluatorValue v1, v2;

	if( p->m_pLeft != NULL && ern == 0 )
		Valuation( p->m_pLeft, &v2 );
	if( p->m_pRight != NULL && ern == 0 )
		Valuation( p->m_pRight, &v1 );

	if( ern == 0 )
	{
		switch( p->m_pToken->pritype )
		{
		case E_STRING	  :
			mytrace( "string[%s]\n", p->m_pToken->word );
			pv->Set( p->m_pToken->word );
			break;
		case E_CONST	  :
			if( p->m_pToken->word[0] == '@' )
			{
				sscanf( &( p->m_pToken->word[1] ), "%x", &vi );
				val1 = ( float )vi;
				pv->Set( val1 );
				if( _DEBUG_ )
				{
					mytrace( "@const[%s]=@%04X\n", p->m_pToken->word, vi );
				}
			}
			else
			{
				pv->Set( ( float )atof( p->m_pToken->word ) );
				if( _DEBUG_ )
				{
					mytrace( "const[%s]=%12.4f\n", p->m_pToken->word, atof( p->m_pToken->word ) );
				}
			}
			break;
		case E_IDENT	  :
			{
				if( m_pVList->GetVariable( p->m_pToken->word, &pVar ) )
				{
					pv->Set( pVar );
					if( _DEBUG_ )
					{
						if( pVar->m_type == BOTS_VAR_REAL )
							mytrace( "ident[%s]=%f\n", p->m_pToken->word, pVar->m_val );
						else
							mytrace( "ident[%s]=%s\n", p->m_pToken->word, pVar->m_strValue.c_str() );
					}
					goto brk_i;
				}
			}
			ern = 2;
brk_i :
			break;
		case E_UNEQ	  :
			if( p->m_pToken->word[0] == '<' )
			{
				if( p->m_pToken->word[1] == '=' )
				{
					if( v2 <= v1 )
						pv->Set( 1.0 );
					else
						pv->Set( 0.0 );
				}
				else
				{
					if( v2 < v1 )
						pv->Set( 1.0 );
					else
						pv->Set( 0.0 );
				}
			}
			else if( p->m_pToken->word[0] == '>' )
			{
				if( p->m_pToken->word[1] == '=' )
				{
					if( v2 >= v1 )
						pv->Set( 1.0 );
					else
						pv->Set( 0.0 );
				}
				else
				{
					pv->Set( ( v2 > v1 ) ? 1.0f : 0.0f );
				}
			}
			if( _DEBUG_ ) mytrace
				( "%s", p->m_pToken->word );
			break;
		case E_EQON	  :
			if( p->m_pToken->word[0] == '=' )
			{
				if( v2 == v1 )
					pv->Set( 1.0 );
				else
					pv->Set( 0.0 );
			}
			else
			{
				if( v2 != v1 )
					pv->Set( 1.0 );
				else
					pv->Set( 0.0 );
			}
			if( _DEBUG_ ) mytrace
				( "%s", p->m_pToken->word );
			break;
		case E_OR	  :
			if( v1.m_val != 0.0 || v2.m_val != 0.0 )
				pv->Set( 1.0 );
			else
				pv->Set( 0.0 );
			if( _DEBUG_ ) mytrace
				( "||" );
			break;
		case E_AND	  :
			if( v1.m_val != 0.0 && v2.m_val != 0.0 )
				pv->Set( 1.0 );
			else
				pv->Set( 0.0 );
			if( _DEBUG_ ) mytrace
				( "&&" );
			break;
		case E_NOT	  :
			if( v1.m_val != 0.0 )
				pv->Set( 0.0 );
			else
				pv->Set( 1.0 );
			if( _DEBUG_ ) mytrace
				( "!" );
			break;
		case E_BOR	  :
			vi = ( int )v1.m_val;
			vii = ( int )v2.m_val;
			vi = vi | vii;
			val1 = ( float )vi;
			pv->Set( val1 );
			if( _DEBUG_ ) mytrace
				( "|" );
			break;
		case E_BAND	  :
			vi = ( int )v1.m_val;
			vii = ( int )v2.m_val;
			vi = vi & vii;
			val1 = ( float )vi;
			pv->Set( val1 );
			if( _DEBUG_ ) mytrace
				( "&" );
			break;
		case E_BXOR	  :
			vi = ( int )v1.m_val;
			vii = ( int )v2.m_val;
			vi = vi ^ vii;
			val1 = ( float )vi;
			pv->Set( val1 );
			if( _DEBUG_ ) mytrace
				( "^" );
			break;
		case E_BNOT	  :
			vi = ( int )v1.m_val;
			vi = ~vi;
			val1 = ( float )vi;
			pv->Set( val1 );
			if( _DEBUG_ ) mytrace
				( "~" );
			break;
		case E_NEG	  :
			pv->Set( -v1.m_val );
			if( _DEBUG_ ) mytrace
				( "-" );
			break;
		case E_ADDSUB   :
			if( p->m_pToken->word[0] == '+' )
			{
				pv->Set( v1.m_val + v2.m_val );
				if( _DEBUG_ ) mytrace
					( "+" );
			}
			else
			{
				pv->Set( v2.m_val - v1.m_val );
				if( _DEBUG_ ) mytrace
					( "-" );
			}
			break;
		case E_MULDIV   :
			if( p->m_pToken->word[0] == '*' )
			{
				pv->Set( v1.m_val * v2.m_val );
				if( _DEBUG_ ) mytrace
					( "*" );
			}
			else
			{
				val1 = v1.m_val;
				if( val1 != 0.0 )
					pv->Set( v2.m_val / val1 );
				else
					ern = 3;
				if( _DEBUG_ ) mytrace
					( "/" );
			}
			break;
		case E_POW	  :
			val2 = v1.m_val;
			val1 = v2.m_val;
			if( val1 == 0 && val2 == 0 )
				ern = 6;
			else if( val1 < 0 && ( ( int )val2 - val2 ) != 0 )
				ern = 7;
			else
				pv->Set( ( float )pow( val1, val2 ) );

			if( _DEBUG_ ) mytrace
				( "pow" );
			break;
		case E_SQRT	  :
			val1 = v2.m_val;
			if( val1 < 0 )
				ern = 8;
			else
				pv->Set( ( float )sqrt( val1 ) );
			if( _DEBUG_ ) mytrace
				( "sqrt" );
			break;
		case E_LN	  :
			val1 = v2.m_val;
			if( val1 <= 0 )
				ern = 9;
			else
				pv->Set( ( float )log( val1 ) );
			if( _DEBUG_ ) mytrace
				( "log" );
			break;
		case E_LOG10	  :
			val1 = v2.m_val;
			if( val1 <= 0 )
				ern = 10;
			else
				pv->Set( ( float )log10( val1 ) );
			if( _DEBUG_ ) mytrace
				( "log10" );
			break;
		case E_EXP:
			val1 = v2.m_val;
			pv->Set( ( float )exp( val1 ) );
			if( _DEBUG_ ) mytrace
				( "exp" );
			break;
		case E_SIN:
			val1 = v2.m_val;
			pv->Set( ( float )sin( val1 * PI1 ) );
			if( _DEBUG_ ) mytrace
				( "sin" );
			break;
		case E_COS	  :
			val1 = v2.m_val;
			pv->Set( ( float )cos( val1 * PI1 ) );
			if( _DEBUG_ ) mytrace
				( "cos" );
			break;
		case E_TAN	  :
			val1 = v2.m_val;
			pv->Set( ( float )tan( val1 * PI1 ) );
			if( _DEBUG_ ) mytrace
				( "tan" );
			break;
		case E_ASIN	  :
			val1 = v2.m_val;
			if( fabs( val1 ) > 1 )
				ern = 11;
			else
				pv->Set( ( float )asin( val1 ) / PI1 );
			if( _DEBUG_ ) mytrace
				( "asin" );
			break;
		case E_ACOS	  :
			val1 = v2.m_val;
			if( fabs( val1 ) > 1 )
				ern = 12;
			else
				pv->Set( ( float )acos( val1 ) / PI1 );
			if( _DEBUG_ ) mytrace
				( "acos" );
			break;
		case E_ATAN	  :
			val1 = v2.m_val;
			pv->Set( ( float )atan( val1 ) / PI1 );
			if( _DEBUG_ ) mytrace
				( "atan" );
			break;
		case E_ATAN2	  :
			val2 = v1.m_val;
			val1 = v2.m_val;
			if( val1 == 0 && val2 == 0 )
				ern = 13;
			else
				pv->Set( ( float )atan2( val1, val2 ) / PI1 );
			if( _DEBUG_ ) mytrace
				( "atan2" );
			break;
		case E_ABS	  :
			val1 = v2.m_val;
			pv->Set( ( float )fabs( val1 ) );
			if( _DEBUG_ ) mytrace
				( "abs" );
			break;
		case E_INT	  :
			val1 = v2.m_val;
			vi = ( int )val1;
			val2 = ( float )vi;
			if( val1 - val2 > 0.5 )
				val2 += 1;
			pv->Set( val2 );
			if( _DEBUG_ ) mytrace
				( "int" );
			break;
		case E_SQR	  :
			val1 = v2.m_val;
			val2 = val1 * val1;
			pv->Set( val2 );
			if( _DEBUG_ ) mytrace
				( "sqr" );
			break;
		default:
			break;
		}
	}
}

void CBotsEvaluator::PrintTree( PCBotsBTreeNode ptr )
{

	counter = 0;

	if( ptr == NULL )
		ptr = m_pRoot;

	if( ptr == NULL )
		return;

	mytrace( "structure of the binary tree:\n" );
	mytrace( "number\taddress\tllink\trlink\tpritype\tinfo\n" );
	PrintOrder( ptr );
}

void CBotsEvaluator::PrintOrder( PCBotsBTreeNode ptr )
{
	if( ptr == NULL )
		return;

	mytrace( "%d\t%p  %p  %p %3d [%s]\n",
		++counter, ptr, ptr->m_pLeft, ptr->m_pRight, ptr->m_pToken->pritype, ptr->m_pToken->word );

	if( ptr->m_pLeft != NULL )
		PrintOrder( ptr->m_pLeft );
	if( ptr->m_pRight != NULL )
		PrintOrder( ptr->m_pRight );
}

void CBotsEvaluator::Clear()
{
	m_result.m_type = BOTS_VAR_INVALID;
	FreeNode( m_pRoot );
	m_pRoot = 0;

	for( int l = 0; l < MAX_TOKENS; l++ )
	{
		m_TokenArray[l]->pritype = NULL;

		for( int i = 0; i <= TOKEN_LEN; i++ )
			m_TokenArray[l]->word[i] = NULL;
	}

}

int CBotsEvaluator::Prepare( LPCTSTR buf )
{
	int err = 0;
	m_result.m_type = BOTS_VAR_INVALID;

	if( IsFloat( buf ) )
	{
		err = 15;
		return err;
	}

	FreeNode( m_pRoot );
	m_pRoot = 0;

	if( ( err = SeparateCalculator( ( char* )buf ) ) == 0 )
	{
		if( ( err = BuildTree() ) == 0 )
		{
			return 0;
		}
	}
	return err;
}

int CBotsEvaluator::Calculate( LPCTSTR buf )
{

	int err = 0;
	float x;
	if( _DEBUG_ )
		mytrace( "buf=[%s]\n", buf );

	if( IsFloat( buf ) )
	{
		if( sscanf( buf, "%f", &x ) == 1 )
		{
			if( _DEBUG_ ) mytrace
				( "val=[%12.4f]\n", x );
			m_result.Set( x );
			return ern;
		}
	}

	if( ( err = Prepare( buf ) ) == 0 )
	{
		if( ( err = Run() ) == 0 )
		{
			if( _DEBUG_ )
			{
				mytrace( "val=[%12.4f]\n", m_result.m_val );
			}
			return ern;
		}
	}

	if( _DEBUG_ )
	{
		mytrace( "%s", GetError( err ) );
		mytrace( "val=[%12.4f]\n", 0.0f );
	}
	return ern;
}

bool CBotsEvaluator::Evaluate( CBotsEvaluatorVariableList* vList )
{
	SetVariableList( vList );
	m_result.m_val = 0;
	if( Run() )
		return false;
	if( fabs( m_result.m_val - 1 ) > 0.0001 )
		return false;
	return true;
}

LPCTSTR CBotsEvaluator::GetError( int err ) const
{
	switch( err )
	{
	case  0:
		return( "Code   0: No error." );
	case  1:
		return( "Error  1: Syntax error, illegal operator." );
	case  2:
		return( "Error  2: Variable not found." );
	case  3:
		return( "Error  3: Zero divisor." );
	case  4:
		return( "Error  4: Syntax error, missing \")\"." );
	case  5:
		return( "Error  5: Syntax error, missing \"(\"." );
	case  6:
		return( "Error  6: Illegal operator: pow(0,0)." );
	case  7:
		return( "Error  7: Illegal operator: pow(x<0,y!=int)." );
	case  8:
		return( "Error  8: Illegal operator: sqrt(x<0)." );
	case  9:
		return( "Error  9: Illegal operator: ln(x<=0)." );
	case 10:
		return( "Error 10: Illegal operator: log(x<=0)." );
	case 11:
		return( "Error 11: Illegal operator: asin(abs(x)>1)." );
	case 12:
		return( "Error 12: Illegal operator: acos(abs(x)>1)." );
	case 13:
		return( "Error 13: Illegal operator: atan2(x=0,y=0)." );
	case 14:
		return( "Error 14: Illegal calculate: no label." );
	default:
		return( "Unknow error." );
	}
}

