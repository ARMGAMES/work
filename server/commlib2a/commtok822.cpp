/*
Excerpts from RFC822 (http://www.faqs.org/rfcs/rfc822.html):

=======================================================================

2.7.  #RULE:  LISTS

A construct "#" is defined, similar to "*", as follows:

<l>#<m>element

indicating at least <l> and at most <m> elements, each  separated
by  one  or more commas (","). This makes the usual form of lists
very easy; a rule such as '(element *("," element))' can be shown
as  "1#element".   Wherever this construct is used, null elements
are allowed, but do not  contribute  to  the  count  of  elements
present.   That  is,  "(element),,(element)"  is  permitted,  but
counts as only two elements.  Therefore, where at least one  ele-
ment  is required, at least one non-null element must be present.
Default values are 0 and infinity so that "#(element)" allows any
number,  including  zero;  "1#element" requires at least one; and
"1#2element" allows one or two.

=======================================================================

3.3.  LEXICAL TOKENS

The following rules are used to define an underlying lexical
analyzer,  which  feeds  tokens to higher level parsers.  See the
ANSI references, in the Bibliography.

; (  Octal, Decimal.)
CHAR        =  <any ASCII character>        ; (  0-177,  0.-127.)
ALPHA       =  <any ASCII alphabetic character>
; (101-132, 65.- 90.)
; (141-172, 97.-122.)
DIGIT       =  <any ASCII decimal digit>    ; ( 60- 71, 48.- 57.)
CTL         =  <any ASCII control           ; (  0- 37,  0.- 31.)
character and DEL>          ; (    177,     127.)
CR          =  <ASCII CR, carriage return>  ; (     15,      13.)
LF          =  <ASCII LF, linefeed>         ; (     12,      10.)
SPACE       =  <ASCII SP, space>            ; (     40,      32.)
HTAB        =  <ASCII HT, horizontal-tab>   ; (     11,       9.)
<">         =  <ASCII quote mark>           ; (     42,      34.)
CRLF        =  CR LF

LWSP-char   =  SPACE / HTAB                 ; semantics = SPACE

linear-white-space =  1*([CRLF] LWSP-char)  ; semantics = SPACE
; CRLF => folding

specials    =  "(" / ")" / "<" / ">" / "@"  ; Must be in quoted-
/  "," / ";" / ":" / "\" / <">  ;  string, to use
/  "." / "[" / "]"              ;  within a word.

delimiters  =  specials / linear-white-space / comment

text        =  <any CHAR, including bare    ; => atoms, specials,
CR & bare LF, but NOT       ;  comments and
including CRLF>             ;  quoted-strings are
;  NOT recognized.

atom        =  1*<any CHAR except specials, SPACE and CTLs>

quoted-string = <"> *(qtext/quoted-pair) <">; Regular qtext or
;   quoted chars.

qtext       =  <any CHAR excepting <">,     ; => may be folded
"\" & CR, and including
linear-white-space>

domain-literal =  "[" *(dtext / quoted-pair) "]"

August 13, 1982              - 10 -                      RFC #822

Standard for ARPA Internet Text Messages

dtext       =  <any CHAR excluding "[",     ; => may be folded
"]", "\" & CR, & including
linear-white-space>

comment     =  "(" *(ctext / quoted-pair / comment) ")"

ctext       =  <any CHAR excluding "(",     ; => may be folded
")", "\" & CR, & including
linear-white-space>

quoted-pair =  "\" CHAR                     ; may quote any char


phrase      =  1*word                       ; Sequence of words

word        =  atom / quoted-string

=======================================================================

3.4.2.  WHITE SPACE

Note:  In structured field bodies, multiple linear space ASCII
characters  (namely  HTABs  and  SPACEs) are treated as
single spaces and may freely surround any  symbol.   In
all header fields, the only place in which at least one
LWSP-char is REQUIRED is at the beginning of  continua-
tion lines in a folded field.

When passing text to processes  that  do  not  interpret  text
according to this standard (e.g., mail protocol servers), then
NO linear-white-space characters should occur between a period
(".") or at-sign ("@") and a <word>.  Exactly ONE SPACE should
be used in place of arbitrary linear-white-space  and  comment
sequences.

Note:  Within systems conforming to this standard, wherever  a
member of the list of delimiters is allowed, LWSP-chars
may also occur before and/or after it.

Writers of  mail-sending  (i.e.,  header-generating)  programs
should realize that there is no network-wide definition of the
effect of ASCII HT (horizontal-tab) characters on the  appear-
ance  of  text  at another network host; therefore, the use of
tabs in message headers, though permitted, is discouraged.

3.4.3.  COMMENTS

A comment is a set of ASCII characters, which is  enclosed  in
matching  parentheses  and which is not within a quoted-string
The comment construct permits message originators to add  text
which  will  be  useful  for  human readers, but which will be
ignored by the formal semantics.  Comments should be  retained
while  the  message  is subject to interpretation according to
this standard.  However, comments  must  NOT  be  included  in
other  cases,  such  as  during  protocol  exchanges with mail
servers.

Comments nest, so that if an unquoted left parenthesis  occurs
in  a  comment  string,  there  must  also be a matching right
parenthesis.  When a comment acts as the delimiter  between  a
sequence of two lexical symbols, such as two atoms, it is lex-
ically equivalent with a single SPACE,  for  the  purposes  of
regenerating  the  sequence, such as when passing the sequence
onto a mail protocol server.  Comments are  detected  as  such
only within field-bodies of structured fields.

If a comment is to be "folded" onto multiple lines,  then  the
syntax  for  folding  must  be  adhered to.  (See the "Lexical

August 13, 1982              - 12 -                      RFC #822

Standard for ARPA Internet Text Messages

Analysis of Messages" section on "Folding Long Header  Fields"
above,  and  the  section on "Case Independence" below.)  Note
that  the  official  semantics  therefore  do  not  "see"  any
unquoted CRLFs that are in comments, although particular pars-
ing programs may wish to note their presence.  For these  pro-
grams,  it would be reasonable to interpret a "CRLF LWSP-char"
as being a CRLF that is part of the comment; i.e., the CRLF is
kept  and  the  LWSP-char is discarded.  Quoted CRLFs (i.e., a
backslash followed by a CR followed by a  LF)  still  must  be
followed by at least one LWSP-char.

3.4.4.  DELIMITING AND QUOTING CHARACTERS

The quote character (backslash) and  characters  that  delimit
syntactic  units  are not, generally, to be taken as data that
are part of the delimited or quoted unit(s).   In  particular,
the   quotation-marks   that   define   a  quoted-string,  the
parentheses that define  a  comment  and  the  backslash  that
quotes  a  following  character  are  NOT  part of the quoted-
string, comment or quoted character.  A quotation-mark that is
to  be  part  of  a quoted-string, a parenthesis that is to be
part of a comment and a backslash that is to be part of either
must  each be preceded by the quote-character backslash ("\").
Note that the syntax allows any character to be quoted  within
a  quoted-string  or  comment; however only certain characters
MUST be quoted to be included as data.  These  characters  are
the  ones that are not part of the alternate text group (i.e.,
ctext or qtext).

The one exception to this rule  is  that  a  single  SPACE  is
assumed  to  exist  between  contiguous words in a phrase, and
this interpretation is independent of  the  actual  number  of
LWSP-chars  that  the  creator  places  between the words.  To
include more than one SPACE, the creator must make  the  LWSP-
chars be part of a quoted-string.

Quotation marks that delimit a quoted string  and  backslashes
that  quote  the  following character should NOT accompany the
quoted-string when the string is passed to processes  that  do
not interpret data according to this specification (e.g., mail
protocol servers).

3.4.5.  QUOTED-STRINGS

Where permitted (i.e., in words in structured fields)  quoted-
strings  are  treated  as a single symbol.  That is, a quoted-
string is equivalent to an atom, syntactically.  If a  quoted-
string  is to be "folded" onto multiple lines, then the syntax
for folding must be adhered to.  (See the "Lexical Analysis of

August 13, 1982              - 13 -                      RFC #822

Standard for ARPA Internet Text Messages

Messages"  section  on "Folding Long Header Fields" above, and
the section on "Case  Independence"  below.)   Therefore,  the
official  semantics  do  not  "see" any bare CRLFs that are in
quoted-strings; however particular parsing programs  may  wish
to  note  their presence.  For such programs, it would be rea-
sonable to interpret a "CRLF LWSP-char" as being a CRLF  which
is  part  of the quoted-string; i.e., the CRLF is kept and the
LWSP-char is discarded.  Quoted CRLFs (i.e., a backslash  fol-
lowed  by  a CR followed by a LF) are also subject to rules of
folding, but the presence of the quoting character (backslash)
explicitly  indicates  that  the  CRLF  is  data to the quoted
string.  Stripping off the first following LWSP-char  is  also
appropriate when parsing quoted CRLFs.

3.4.6.  BRACKETING CHARACTERS

There is one type of bracket which must occur in matched pairs
and may have pairs nested within each other:

o   Parentheses ("(" and ")") are used  to  indicate  com-
ments.

There are three types of brackets which must occur in  matched
pairs, and which may NOT be nested:

o   Colon/semi-colon (":" and ";") are   used  in  address
specifications  to  indicate that the included list of
addresses are to be treated as a group.

o   Angle brackets ("<" and ">")  are  generally  used  to
indicate  the  presence of a one machine-usable refer-
ence (e.g., delimiting mailboxes), possibly  including
source-routing to the machine.

o   Square brackets ("[" and "]") are used to indicate the
presence  of  a  domain-literal, which the appropriate
name-domain  is  to  use  directly,  bypassing  normal
name-resolution mechanisms.

3.4.7.  CASE INDEPENDENCE

Except as noted, alphabetic strings may be represented in  any
combination of upper and lower case.  The only syntactic units

August 13, 1982              - 14 -                      RFC #822

Standard for ARPA Internet Text Messages

which requires preservation of case information are:

-  text
-  qtext
-  dtext
-  ctext
-  quoted-pair
-  local-part, except "Postmaster"

When matching any other syntactic unit, case is to be ignored.
For  example, the field-names "From", "FROM", "from", and even
"FroM" are semantically equal and should all be treated ident-
ically.

When generating these units, any mix of upper and  lower  case
alphabetic  characters  may  be  used.  The case shown in this
specification is suggested for message-creating processes.

Note:  The reserved local-part address unit, "Postmaster",  is
an  exception.   When  the  value "Postmaster" is being
interpreted, it must be  accepted  in  any  mixture  of
case, including "POSTMASTER", and "postmaster".

=======================================================================

6.  ADDRESS SPECIFICATION

6.1.  SYNTAX

address     =  mailbox                      ; one addressee
/  group                        ; named list

group       =  phrase ":" [#mailbox] ";"

mailbox     =  addr-spec                    ; simple address
/  phrase route-addr            ; name & addr-spec

route-addr  =  "<" [route] addr-spec ">"

route       =  1#("@" domain) ":"           ; path-relative

addr-spec   =  local-part "@" domain        ; global address

local-part  =  word *("." word)             ; uninterpreted
; case-preserved

domain      =  sub-domain *("." sub-domain)

sub-domain  =  domain-ref / domain-literal

domain-ref  =  atom                         ; symbolic reference

=======================================================================

6.2.3.  DOMAIN TERMS

A domain-ref must be THE official name of a registry, network,
or  host.   It  is  a  symbolic  reference, within a name sub-
domain.  At times, it is necessary to bypass standard  mechan-
isms  for  resolving  such  references,  using  more primitive
information, such as a network host address  rather  than  its
associated host name.

To permit such references, this standard provides the  domain-
literal  construct.   Its contents must conform with the needs
of the sub-domain in which it is interpreted.

Domain-literals which refer to domains within the ARPA  Inter-
net  specify  32-bit  Internet addresses, in four 8-bit fields
noted in decimal, as described in Request for  Comments  #820,
"Assigned Numbers."  For example:

[10.0.3.19]

Note:  THE USE OF DOMAIN-LITERALS IS STRONGLY DISCOURAGED.  It
is  permitted  only  as  a means of bypassing temporary
system limitations, such as name tables which  are  not
complete.

The names of "top-level" domains, and  the  names  of  domains
under  in  the  ARPA Internet, are registered with the Network
Information Center, SRI International, Menlo Park, California.

*/

/* Excerpt from RFC1123 (http://tools.ietf.org/html/rfc1123):
5.2.15  RFC-822 Syntax Change: RFC-822 Section 6.1

The syntactic definition of "mailbox" in RFC-822 is hereby
changed to:

mailbox =  addr-spec            ; simple address
/ [phrase] route-addr   ; name & addr-spec

That is, the phrase preceding a route address is now OPTIONAL.
This change makes the following header field legal, for
example:

From: <craig@nnsc.nsf.net>
*/

#include "ppinclude.h"
#include "ppstring.h"
#include "commtok822.h"

#define RFC822_HTAB 9
#define RFC822_LF 10
#define RFC822_CR 13

#define RFC822_NOTACHAR 0x01
#define RFC822_CTL 0x02
#define RFC822_SPECIAL 0x04
#define RFC822_DIGIT 0x08
#define RFC822_ALPHA 0x10
#define RFC822_SPACE 0x20

//PRESERVE_* flags - DEPRECATED
//Note: ws within domain-literals is ALWAYS preserved
//#define RFC822_FLAGS_PRESERVE_DOMAIN_WS 0x01
//Note: ws within quoted-strings within local-part is ALWAYS preserved
//#define RFC822_FLAGS_PRESERVE_LOCALPART_WS 0x02
//#define RFC822_FLAGS_PRESERVE_ADDRSPEC_WS 0x04
//#define RFC822_FLAGS_PRESERVE_MAILBOX_NAME 0x08
//Note: RFC822_FLAGS_PRESERVE_NAME_WS covers both group name and mailbox name, and makes sense only if RFC822_FLAGS_PRESERVE_MAILBOX_NAME or RFC822_FLAGS_PRESERVE_GROUP_NAME is specified
//#define RFC822_FLAGS_PRESERVE_NAME_WS 0x10
//#define RFC822_FLAGS_PRESERVE_MAILBOX_WS 0x20
//#define RFC822_FLAGS_PRESERVE_ROUTEADDR_DELIM 0x40
//Note: RFC822_FLAGS_PRESERVE_ROUTEADDR_ROUTE should NOT be used without RFC822_FLAGS_PRESERVE_ROUTEADDR_DELIM
//#define RFC822_FLAGS_PRESERVE_ROUTEADDR_ROUTE 0x80
//#define RFC822_FLAGS_PRESERVE_ROUTEADDR_WS 0x100
//#define RFC822_FLAGS_PRESERVE_ROUTE_WS 0x200
//Note: RFC822_FLAGS_PRESERVE_GROUP means that group addresses are NOT splitted
//#define RFC822_FLAGS_PRESERVE_GROUP 0x400
//Note: RFC822_FLAGS_PRESERVE_GROUP_WS makes sense only if RFC822_FLAGS_PRESERVE_GROUP is specified
//#define RFC822_FLAGS_PRESERVE_GROUP_WS 0x800

#define RFC822_FLAGS_SPACE_AS_ADDRESS_DELIMITER_ALLOWED 0x1000
#define RFC822_FLAGS_BARE_ROUTEADDR_ALLOWED 0x2000
#define RFC822_FLAGS_NORMALIZE_QUOTED_LOCAL_PART 0x4000//normalize "abc"@def.com to abc@def.com
#define RFC822_FLAGS_SINGLE_ADDRESS_ONLY 0x8000

//IMPORTANT: 0 is a formally valid CHAR according to RFC822 but we'll prohibit it due to C/C++ restrictions

const static BYTE _rfc822_Type[ 128 ] = 
{
	RFC822_NOTACHAR         , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , //0x00 - 0x07
	RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , //0x08 - 0x0F
	RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , //0x10 - 0x17
	RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , RFC822_CTL              , //0x18 - 0x1F

	RFC822_SPACE   /* ' ' */, 0              /* '!' */, RFC822_SPECIAL /* '"' */, 0              /* '#' */, 0              /* '$' */, 0              /* '%' */, 0              /* '&' */, 0              /* ''' */, //0x20 - 0x27
	RFC822_SPECIAL /* '(' */, RFC822_SPECIAL /* ')' */, 0              /* '*' */, 0              /* '+' */, RFC822_SPECIAL /* ',' */, 0              /* '-' */, RFC822_SPECIAL /* '.' */, 0              /* '/' */, //0x28 - 0x2F

	RFC822_DIGIT   /* '0' */, RFC822_DIGIT   /* '1' */, RFC822_DIGIT   /* '2' */, RFC822_DIGIT   /* '3' */, RFC822_DIGIT   /* '4' */, RFC822_DIGIT   /* '5' */, RFC822_DIGIT   /* '6' */, RFC822_DIGIT   /* '7' */, //0x30 - 0x37
	RFC822_DIGIT   /* '8' */, RFC822_DIGIT   /* '9' */, RFC822_SPECIAL /* ':' */, RFC822_SPECIAL /* ';' */, RFC822_SPECIAL /* '<' */, 0              /* '=' */, RFC822_SPECIAL /* '>' */, 0              /* '?' */, //0x38 - 0x3F

	RFC822_SPECIAL /* '@' */, RFC822_ALPHA   /* 'A' */, RFC822_ALPHA   /* 'B' */, RFC822_ALPHA   /* 'C' */, RFC822_ALPHA   /* 'D' */, RFC822_ALPHA   /* 'E' */, RFC822_ALPHA   /* 'F' */, RFC822_ALPHA   /* 'G' */, //0x40 - 0x47
	RFC822_ALPHA   /* 'H' */, RFC822_ALPHA   /* 'I' */, RFC822_ALPHA   /* 'J' */, RFC822_ALPHA   /* 'K' */, RFC822_ALPHA   /* 'L' */, RFC822_ALPHA   /* 'M' */, RFC822_ALPHA   /* 'N' */, RFC822_ALPHA   /* 'O' */, //0x48 - 0x4F
	RFC822_ALPHA   /* 'P' */, RFC822_ALPHA   /* 'Q' */, RFC822_ALPHA   /* 'R' */, RFC822_ALPHA   /* 'S' */, RFC822_ALPHA   /* 'T' */, RFC822_ALPHA   /* 'U' */, RFC822_ALPHA   /* 'V' */, RFC822_ALPHA   /* 'W' */, //0x50 - 0x57
	RFC822_ALPHA   /* 'X' */, RFC822_ALPHA   /* 'Y' */, RFC822_ALPHA   /* 'Z' */, RFC822_SPECIAL /* '[' */, RFC822_SPECIAL /* '\' */, RFC822_SPECIAL /* ']' */, 0              /* '^' */, 0              /* '_' */, //0x58 - 0x5F

	0              /* '`' */, RFC822_ALPHA   /* 'a' */, RFC822_ALPHA   /* 'b' */, RFC822_ALPHA   /* 'c' */, RFC822_ALPHA   /* 'd' */, RFC822_ALPHA   /* 'e' */, RFC822_ALPHA   /* 'f' */, RFC822_ALPHA   /* 'g' */, //0x60 - 0x67
	RFC822_ALPHA   /* 'h' */, RFC822_ALPHA   /* 'i' */, RFC822_ALPHA   /* 'j' */, RFC822_ALPHA   /* 'k' */, RFC822_ALPHA   /* 'l' */, RFC822_ALPHA   /* 'm' */, RFC822_ALPHA   /* 'n' */, RFC822_ALPHA   /* 'o' */, //0x68 - 0x6F
	RFC822_ALPHA   /* 'p' */, RFC822_ALPHA   /* 'q' */, RFC822_ALPHA   /* 'r' */, RFC822_ALPHA   /* 's' */, RFC822_ALPHA   /* 't' */, RFC822_ALPHA   /* 'u' */, RFC822_ALPHA   /* 'v' */, RFC822_ALPHA   /* 'w' */, //0x70 - 0x77
	RFC822_ALPHA   /* 'x' */, RFC822_ALPHA   /* 'y' */, RFC822_ALPHA   /* 'z' */, 0              /* '{' */, 0              /* '|' */, 0              /* '}' */, 0              /* '~' */, RFC822_CTL                //0x78 - 0x7F
};

static inline bool isChar( char c )
{
	unsigned int cc = c;
	return cc > 0 && cc <= 127;
}

static inline BYTE getCharType( char c )
{
	unsigned int cc = c;
	if( cc > 127 )
		return RFC822_NOTACHAR;
	return _rfc822_Type[ cc ];
}

static inline void appendCtextChar( PString* ret, char c )
{
	if( !ret )
		return;

	if( c == '(' || c == ')' || c == '\\' || c == RFC822_CR )
	{
		ret->append( '\\' );
		ret->append( c );
	}
	else
		ret->append( c );
}

static void parseCommentSkipped1( PString* ret, const char*& s )
//CROSS-CHECK BUGS: parseDomainLiteralSkipped1 and parseQuotedStringSkipped1 below
{
	for(;;)
	{
		char c = *s;
		if( c == '\\' )
		{//quoted-pair
			++s;
			c = *s;
			if( !isChar( c ) )
				throw PError( "RFC822: invalid char within quoted-pair" );
			appendCtextChar( ret, c );
			++s;
		}
		else if( c == ')' )
		{
			++s;
			break;
		}
		else if( c == '(' )
		{
			if( ret )
				ret->append( '(' );
			++s;
			parseCommentSkipped1( ret, s );//recursive - see RFC822 3.4.3
			if( ret )
				ret->append( ')' );
		}
		else//Note: not prohibiting bare CR here (more liberal approach than specified)
		{
			if( !isChar( c ) )
				throw PError( "RFC822: invalid char within comment" );
			appendCtextChar( ret, c );
			++s;
		}
	}
}

static const char* skipWs( const char* s, bool skipAlsoCommas = false )
//CROSS-CHECK BUGS: skipWsAndCommas below
{
	const char* ss = s;
	for(;;)
	{
		char c = *ss;
		if( c == ' ' || c == RFC822_HTAB )
			++ss;
		else if( skipAlsoCommas && c == ',' )
			++ss;
		else if( c == RFC822_CR )
		{
			if( ss[ 1 ] == RFC822_LF )
				ss += 2;
			else
				break;
		}
		else if( c == '(' )
		{//comment
			++ss;
			parseCommentSkipped1( 0, ss );
		}
		else
			break;
	}
	return ss;
}

static inline const char* skipWsAndCommas( const char* s )
{
	return skipWs( s, true );
}

#define RFC822_NOT_ATOM_CHAR ( RFC822_NOTACHAR | RFC822_SPECIAL | RFC822_CTL | RFC822_SPACE )

static bool isValidAtomChar( char c )
{
	BYTE t = getCharType( c );
	return ( t & RFC822_NOT_ATOM_CHAR ) == 0;
}

static void parseAtom( PString* ret, const char*& s )
{
	char c0 = *s;
	BYTE t0 = getCharType( c0 );
	if( t0 & RFC822_NOT_ATOM_CHAR )
		throw PError( "RFC822: invalid first symbol of the atom" );
	if( ret )
		ret->append( c0 );
	++s;

	for(;;)
	{
		char c = *s;
		BYTE t = getCharType( c );
		if( t & RFC822_NOT_ATOM_CHAR )
			break;
		if( ret )
			ret->append( c );
		++s;
	}
}

static inline void appendQtextChar( PString* ret, char c )
{
	if( !ret )
		return;

	if( c == '"' || c == '\\' || c == RFC822_CR )
	{
		ret->append( '\\' );
		ret->append( c );
	}
	else
		ret->append( c );
}

static void parseQuotedStringSkipped1( PString* ret, const char*& s )
//CROSS-CHECK BUGS: parseDomainLiteralSkipped1 below and parseCommentSkipped1 above
{
	for(;;)
	{
		char c = *s;
		if( c == '\\' )
		{//quoted-pair
			++s;
			c = *s;
			if( !isChar( c ) )
				throw PError( "RFC822: invalid char within quoted-pair" );
			appendQtextChar( ret, c );
			++s;
		}
		else if( c == '"' )
		{
			++s;
			break;
		}
		else//Note: not prohibiting bare CR here (more liberal approach than specified)
		{
			if( !isChar( c ) )
				throw PError( "RFC822: invalid char within quoted-string" );
			appendQtextChar( ret, c );
			++s;
		}
	}
}

/* static void parseQuotedString( PString* ret, const char*& s )
{
if( *s != '"' )
throw PError( "RFC822: invalid first symbol of the quoted-string" );
++s;
parseQuotedStringSkipped1( ret, s );
}*/

static bool isValidWordFirstChar( char c )
{
	if( c == '"' )
		return true;
	return isValidAtomChar( c );
}

static bool parseWord( PString* ret, const char*& s )//returns true if quoted-string
{
	if( *s == '"' )
	{
		if( ret )
			ret->append( '"' );
		++s;
		parseQuotedStringSkipped1( ret, s );
		if( ret )
			ret->append( '"' );
		return true;
	}
	else
	{
		parseAtom( ret, s );
		return false;
	}
}

static int parsePhraseOrNull( PString* ret, const char*& s, bool& word0Quoted /*, bool preserveWs */ )
{
	word0Quoted = false;
	int n = 0;
	for(;;)
	{
		if( !isValidWordFirstChar( *s ) )
			break;

		bool quoted = parseWord( ret, s );
		if( n == 0 )
			word0Quoted = quoted;
		++n;

		const char* ss = skipWs( s );
		if( !isValidWordFirstChar( *ss ) )
			break;
		if( ret )
		{
			//if( preserveWs )
			//	ret->append( s, ss );
			//else
			ret->append( ' ' );
		}

		s = ss;
	}
	return n;
}

static inline void appendDtextChar( PString* ret, char c )
{
	if( !ret )
		return;

	if( c == '[' || c == ']' || c == '\\' || c == RFC822_CR )
	{
		ret->append( '\\' );
		ret->append( c );
	}
	else
		ret->append( c );
}

static void parseDomainLiteralSkipped1( PString* ret, const char*& s )
//CROSS-CHECK BUGS: parseQuotedStringSkipped1 and parseCommentSkipped1 above
{
	for(;;)
	{
		char c = *s;
		if( c == '\\' )
		{//quoted-pair
			++s;
			c = *s;
			if( !isChar( c ) )
				throw PError( "RFC822: invalid char within quoted-pair" );
			appendDtextChar( ret, c );
			++s;
		}
		else if( c == ']' )
		{
			++s;
			break;
		}
		else//Note: not prohibiting bare CR or '[' here (more liberal approach than specified)
		{
			if( !isChar( c ) )
				throw PError( "RFC822: invalid char within domain-literal" );
			appendDtextChar( ret, c );
			++s;
		}
	}
}

static bool parseSubDomain( PString* ret, const char*& s, int flags )//returns true if domain-literal
{
	if( *s == '[' )
	{//domain-literal
		if( ret )
			ret->append( '[' );
		++s;
		parseDomainLiteralSkipped1( ret, s );
		if( ret )
			ret->append( ']' );
		return true;
	}
	else//domain-ref
	{
		parseAtom( ret, s );
		return false;
	}
}

static void parseDomain( PString* ret, const char*& s, int flags, CommMailAddressParseData* domainParseData )
//CROSS-CHECK BUGS: parseLocalPart below
{
	if( domainParseData )
	{
		PASSERT4( domainParseData->nSubDomains == 0 );
		PASSERT4( domainParseData->nDomainLiterals == 0 );
	}
	for(;;)
	{
		PString subDomain;
		bool domainLiteral = parseSubDomain( &subDomain, s, flags );
		if( ret )
			ret->append( subDomain );
		if( domainParseData )
		{
			++domainParseData->nSubDomains;
			if( domainLiteral )
				++domainParseData->nDomainLiterals;
			domainParseData->lastSubDomain = subDomain;
		}
		const char* ss = skipWs( s );
		if( *ss != '.' )
			break;

		if( ret )
		{
			//if( ( flags & RFC822_FLAGS_PRESERVE_DOMAIN_WS ) != 0 )
			//	ret->append( s, ss );
			ret->append( '.' );
		}
		s = ss + 1;

		ss = skipWs( s );
		//if( ret && ( flags & RFC822_FLAGS_PRESERVE_DOMAIN_WS ) != 0 )
		//	ret->append( s, ss );
		s = ss;
	}
}

static void appendLocalPartWord( PString& ret, const char* word, int flags, bool quoted )
{
	if( quoted && ( ( flags & RFC822_FLAGS_NORMALIZE_QUOTED_LOCAL_PART ) != 0 ) )
	{
		PASSERT4( *word == '"' );
		int l = strlen( word );
		PASSERT4( l >= 2 );
		PASSERT4( word[ l - 1 ] == '"' );

		bool ok = true;
		for( const char* s = word + 1; s < word + l - 1; ++s )
		{
			if( !isValidAtomChar( *s ) )//excludes '\\'
			{
				ok = false;
				break;
			}
		}

		if( ok )
		{
			ret.append( word + 1, word + l - 1 );
			return;
		}
	}

	ret.append( word );
}

static void parseLocalPartSkippedWord( PString* ret, const char*& s, int flags )
//CROSS-CHECK BUGS: parseDomain above
{
	for( bool first = true;; first = false )
	{
		if( !first )
		{
			PString word;
			bool quoted = parseWord( &word, s );
			if( ret )
				appendLocalPartWord( *ret, word, flags, quoted );
		}
		const char* ss = skipWs( s );
		if( *ss != '.' )
			return;

		if( ret )
		{
			//if( ( flags & RFC822_FLAGS_PRESERVE_LOCALPART_WS ) != 0 )
			//	ret->append( s, ss );
			ret->append( '.' );
		}
		s = ss + 1;

		ss = skipWs( s );
		//if( ret && ( flags & RFC822_FLAGS_PRESERVE_LOCALPART_WS ) != 0 )
		//	ret->append( s, ss );
		s = ss;
	}
}

static void parseAddrSpecSkippedWord( PString* ret, const char*& s, int flags, CommMailAddressParseData* parseData )
{
	parseLocalPartSkippedWord( ret, s, flags );
	const char* ss = skipWs( s );
	if( *ss != '@' )
		throw PError( "RFC822: '@' expected within addr-spec" );

	if( ret )
	{
		//if( ( flags & RFC822_FLAGS_PRESERVE_ADDRSPEC_WS ) != 0 )
		//	ret->append( s, ss );
		ret->append( '@' );
	}
	s = ss + 1;

	ss = skipWs( s );
	//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ADDRSPEC_WS ) != 0 )
	//	ret->append( s, ss );
	s = ss;

	parseDomain( ret, s, flags, parseData );
}

static void parseAddrSpec( PString* ret, const char*& s, int flags, CommMailAddressParseData* parseData )
{
	PString word;
	bool quoted = parseWord( &word, s );
	if( ret )
		appendLocalPartWord( *ret, word, flags, quoted );

	parseAddrSpecSkippedWord( ret, s, flags, parseData );
}

static void parseRouteSkipped1( PString* ret, const char*& s, int flags )
{
	for(;;)
	{
		const char* ss = skipWs( s );
		//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTE_WS ) != 0 )
		//	ret->append( s, ss );
		s = ss;
		parseDomain( ret, s, flags, 0 );

		ss = skipWs( s );
		if( *ss != ':' && *s != ',' )
			throw PError( "RFC822: ':' or ',' expected within route" );

		//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTE_WS ) != 0 )
		//	ret->append( s, ss );
		s = ss;

		if( *s == ':' )
		{
			if( ret )
				ret->append( ':' );
			++s;
			break;
		}
		else
		{
			PASSERT5( *s == ',' );
			if( ret )
				ret->append( ',' );
			++s;
			ss = skipWsAndCommas( s );//handling ",," - see RFC822, paragraph 2.7

			if( *ss != '@' )
				throw PError( "RFC822: '@' expected within route" );

			//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTE_WS ) != 0 )
			//	ret->append( s, ss );
			s = ss;
			if( ret )
				ret->append( '@' );
			++s;
		}
	}
}

static void parseRouteAddrSkipped1( PString* ret, const char*& s, int flags, CommMailAddressParseData* parseData )
{
	const char* ss = skipWs( s );
	//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTEADDR_WS ) != 0 )
	//	ret->append( s, ss );
	s = ss;

	if( *s == '@' || *s == ',' )
	{//explicit route
		if( *s == ',' )//null first element in list - see RFC822, paragraph 2.7
		{
			ss = skipWsAndCommas( s );
			//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTEADDR_WS ) != 0 )
			//	ret->append( s, ss );
			s = ss;
			if( *s != '@' )
				throw PError( "RFC822: '@' expected within route-addr" );
		}

		PString* wr = 0;
		/* if( ( flags & RFC822_FLAGS_PRESERVE_ROUTEADDR_ROUTE ) != 0 )
		{
		wr = ret;
		if( ret )
		ret->append( '@' );
		}*/
		PASSERT4( *s == '@' );
		++s;
		parseRouteSkipped1( wr, s, flags );
	}

	parseAddrSpec( ret, s, flags, parseData );

	ss = skipWs( s );
	if( *ss != '>' )
		throw PError( "RFC822: '>' expected within route-addr" );

	//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTEADDR_WS ) != 0 )
	//		ret->append( s, ss );

	s = ss + 1;
}

static void parseWsAndMailboxRouteAddr( PString* ret, const char*& s, int flags, CommMailAddressParseData* parseData )
{
	const char* ss = skipWs( s );
	PASSERT4( *ss == '<' );

	//if( ret && ( flags & RFC822_FLAGS_PRESERVE_MAILBOX_WS ) != 0 )
	//	ret->append( s, ss );
	s = ss + 1;

	//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTEADDR_DELIM ) != 0 )
	//	ret->append( '<' );
	parseRouteAddrSkipped1( ret, s, flags, parseData );
	//if( ret && ( flags & RFC822_FLAGS_PRESERVE_ROUTEADDR_DELIM ) != 0 )
	//	ret->append( '>' );
}

static void parseMailboxSkippedPhrase( PString* ret, const char*& s, int flags, const char* skippedPhrase, int nWords, bool word0Quoted, CommMailAddressParseData* parseData )
{
	const char* ss = skipWs( s );
	char cc = *ss;
	if( cc == '.' || cc == '@' )
	{//it was addr-spec/local-part
		if( nWords != 1 )
			throw PError( "RFC822: too many words within local-part" );

		if( ret )
			appendLocalPartWord( *ret, skippedPhrase, flags, word0Quoted );
		parseAddrSpecSkippedWord( ret, s, flags, parseData );
	}
	else if( cc == '<' )
	{//it was phrase route-addr
		if( nWords == 0 && ( flags & RFC822_FLAGS_BARE_ROUTEADDR_ALLOWED ) == 0 )
			throw PError( "RFC822: bare route-addr is not allowed" );

		//if( ret && ( flags & RFC822_FLAGS_PRESERVE_MAILBOX_NAME ) != 0 )
		//	ret->append( skippedPhrase );
		parseWsAndMailboxRouteAddr( ret, s, flags, parseData );
	}
	else
		throw PError( "RFC822: '.' or '@' or ':' or '<' expected within address" );
}

static void parseMailbox( PString* ret, const char*& s, int flags, CommMailAddressParseData* parseData )
{
	PString phrase;
	bool word0Quoted = false;
	int nWords = parsePhraseOrNull( &phrase, s, word0Quoted /*, ( flags & RFC822_FLAGS_PRESERVE_NAME_WS ) != 0 */ );
	parseMailboxSkippedPhrase( ret, s, flags, phrase, nWords, word0Quoted, parseData );
}

static void parseAddress( vector< PString >& retOrig, vector< PString >& ret, const char*& s, int flags, CommMailAddressParseData* parseData )
{
	PASSERT4( parseData == 0 || ( flags & RFC822_FLAGS_SINGLE_ADDRESS_ONLY ) != 0 );

	//both 'mailbox' and 'group' are starting with 'phrase'
	PString name;
	bool word0Quoted = false;
	const char* origBegin = s;
	int nWords = parsePhraseOrNull( &name, s, word0Quoted /* , ( flags & RFC822_FLAGS_PRESERVE_NAME_WS ) != 0 */ );
	const char* ss = skipWs( s );
	char cc = *ss;
	if( cc == ':' )
	{//it was group
		//PString groupAddr;//for RFC822_FLAGS_PRESERVE_GROUP only
		//bool preserveGroup = ( flags & RFC822_FLAGS_PRESERVE_GROUP ) != 0;
		//const char* groupOrigBegin = origBegin;
		/*if( preserveGroup )
		{
		groupAddr.append( name );
		if( ( flags & RFC822_FLAGS_PRESERVE_GROUP_WS ) != 0 )
		groupAddr.append( s, ss );
		groupAddr.append( ':' );
		}*/
		s = ss + 1;

		for( int nAddr = 0;; ++nAddr )
		{
			ss = skipWsAndCommas( s );//also handles null first element in list - see RFC822, paragraph 2.7
			//if( preserveGroup && ( flags & RFC822_FLAGS_PRESERVE_GROUP_WS ) != 0 )
			//	groupAddr.append( s, ss );
			s = ss;

			if( nAddr > 0 && ( flags & RFC822_FLAGS_SINGLE_ADDRESS_ONLY ) != 0 )
				throw PError( "RFC822: multi-address group encountered while only single address allowed" );

			PString mbox;
			origBegin = s;
			parseMailbox( &mbox, s, flags, parseData );
			const char* origEnd = s;

			ss = skipWs( s );
			if( *ss != ';' && *ss != ',' )
				throw PError( "RFC822: ';' or ',' expected within group" );

			/*if( preserveGroup )
			{
			groupAddr.append( mbox );
			if( ( flags & RFC822_FLAGS_PRESERVE_GROUP_WS ) != 0 )
			groupAddr.append( s, ss );
			}
			else*/
			{
				retOrig.push_back( PString( origBegin, origEnd ) );
				ret.push_back( mbox );
			}

			if( *ss == ';' )
			{
				//if( preserveGroup && ( flags & RFC822_FLAGS_PRESERVE_GROUP_WS ) != 0 )
				//	groupAddr.append( ';' );
				s = ss + 1;
				break;
			}
			else
			{
				PASSERT5( *ss == ',' );
				s = ss;
				//handling ",," - see RFC822, paragraph 2.7
				ss = skipWsAndCommas( s );
				//if( preserveGroup && ( flags & RFC822_FLAGS_PRESERVE_GROUP_WS ) != 0 )
				//	groupAddr.append( s, ss );
				s = ss;
			}
		}

		/*if( preserveGroup )
		{
		retOrig.push_back( PString( groupOrigBegin, s ) );
		ret.push_back( groupAddr );
		}*/
	}
	else
	{//it was mailbox
		PString mbox;
		parseMailboxSkippedPhrase( &mbox, s, flags, name, nWords, word0Quoted, parseData );
		retOrig.push_back( PString( origBegin, s ) );
		ret.push_back( mbox );
	}
}

static void parseAddressList( vector< PString >& retOrig, vector< PString >& ret, const char*& s, int flags )
{
	for(;;)
	{
		const char* ss = skipWsAndCommas( s );//also handles null first element in list and ",," - see RFC822, paragraph 2.7
		if( *ss == 0 )
		{
			s = ss;
			break;
		}
		s = ss;
		parseAddress( retOrig, ret, s, flags, 0 );

		ss = skipWs( s );
		if( *ss == 0 )
		{
			s = ss;
			break;
		}
		if( ( flags & RFC822_FLAGS_SPACE_AS_ADDRESS_DELIMITER_ALLOWED ) == 0 && *ss != ',' )
			throw PError( "RFC822: ',' expected within address list" );
		s = ss;
	}

	//if( ret.size() == 0 )
	//	throw PError( "RFC822: empty address list" );
}

static int translateCommFlagsToRfc822Flags( int commFlags )
{
	int ret = RFC822_FLAGS_BARE_ROUTEADDR_ALLOWED;//allowed by RFC1123
	if( commFlags & CommMailAddressFlagSpaceAsAddressDelimiterAllowed )
		ret |= RFC822_FLAGS_SPACE_AS_ADDRESS_DELIMITER_ALLOWED;
	if( commFlags & CommMailAddressFlagBareRouteAddrDisallowed )
		ret &= ~RFC822_FLAGS_BARE_ROUTEADDR_ALLOWED;
	if( commFlags & CommMailAddressFlagNormalizeQuotedLocalPart )
		ret |= RFC822_FLAGS_NORMALIZE_QUOTED_LOCAL_PART;
	return ret;
}

void CommMailAddressNormalize( vector<PString>& origAddrs, vector<PString>& addrs, const char* addressString, int commFlags )
{
	int flags = translateCommFlagsToRfc822Flags( commFlags );
	parseAddressList( origAddrs, addrs, addressString, flags );
	PASSERT4( origAddrs.size() == addrs.size() );
	PASSERT4( *addressString == 0 );
}

void CommMailAddressNormalize( PString& origAddr, PString& addr, const char* addressString, CommMailAddressParseData* parseData, int commFlags )
{
	if( parseData )
	{
		parseData->lastSubDomain = "";
		parseData->nDomainLiterals = 0;
		parseData->nSubDomains = 0;
	}

	int flags = translateCommFlagsToRfc822Flags( commFlags );

	const char* ss = skipWs( addressString );
	addressString = ss;
	if( *ss == 0 )
	{
		origAddr = "";
		addr = "";
		return;
	}
	vector< PString > addrs;
	vector< PString > origAddrs;
	parseAddress( origAddrs, addrs, addressString, flags | RFC822_FLAGS_SINGLE_ADDRESS_ONLY, parseData );
	PASSERT4( origAddrs.size() == addrs.size() );
	PASSERT4( addrs.size() > 0 );

	ss = skipWs( addressString );
	if( *ss != 0 )
		throw PError( "RFC822: extra symbols within address" );

	if( addrs.size() > 1 )
		throw PError( "RFC822: group address when only single address expected" );
	PASSERT5( addrs.size() == 1 );
	addr = addrs[ 0 ];
	origAddr = origAddrs[ 0 ];
}
