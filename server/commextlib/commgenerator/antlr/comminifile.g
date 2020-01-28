grammar comminifile;

options
{
    language = C;
}

@lexer::includes
{
	#include "..\\..\\CGapp.h"
	typedef void (*displayRecognitionErrorIniType) (struct ANTLR3_BASE_RECOGNIZER_struct * recognizer, pANTLR3_UINT8 * tokenNames);
	
#ifdef _WIN32
#pragma warning( disable : 4296 )
#endif // _WIN32
}

@parser::includes
{
	#include "comminifileLexer.h"
}

@lexer::postinclude
{
char lexerinibuf[2048] = {0};
  
displayRecognitionErrorIniType errOldIniLexer;
void displayRecognitionErrorIniLexer (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	setvbuf(stderr, lexerinibuf, _IOFBF, 1024);
	errOldIniLexer(recognizer, tokenNames);
	throw exception(lexerinibuf);
}
}

@lexer::apifuncs 
{
	errOldIniLexer = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorIniLexer;
}

@parser::postinclude
{
char parserinibuf[2048] = {0};
   
displayRecognitionErrorIniType errOldIniParser;
void displayRecognitionErrorIniParser (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	setvbuf(stderr, parserinibuf, _IOFBF, 1024);
	errOldIniParser(recognizer, tokenNames);
	throw exception(parserinibuf);
}
}

@parser::apifuncs 
{
	errOldIniParser = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorIniParser;
}

@members
{
	CGloader* plodr;
	const char* filename;
	string section;
	set<string>* iniloads;
}

prog[CGloader* plodr_, const char* filename_, set<string>* iniloads_]
@init
{
	if(!$plodr_) throw exception("comminifile: No loader initialized");
	plodr = $plodr_;
	if(!$filename_) throw exception("comminifile: No ini filename initialized");
	filename = $filename_;
	if(!$iniloads_) throw exception("comminifile: No ini files container initialized");
	iniloads = $iniloads_;
}
@after
{
}
	: 	
		(COMMENTEX				
	| 	COMMENTINTERNAL	)*
		progfield+	
	;

progfield
@declarations
{
	string field;
	string value;
}
	:	'['a=IDE']' {section = ((char*)$a.text->chars);}
	 	(((b=IDE {field = ((char*)$b.text->chars);} | (e=INT {field = ((char*)$e.text->chars);})) ('=' ((c=IDE {value = ((char*)$c.text->chars);}) | (d=INT {value = ((char*)$d.text->chars);}) )?)? 
		{plodr->addIniValue(section, field, value, filename, iniloads);})
	| 	COMMENTEX				
	| 	COMMENTINTERNAL	)*
	;

	
COMMENTINTERNAL
    :  '///' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;} 
    ;

COMMENTEX
    :  '//' ~('\n'|'\r')* //'\r'? '\n' //{$channel=HIDDEN;} '//' 
    |  ';' ~('\n'|'\r')* //'\r'? '\n' //{$channel=HIDDEN;} '//' 
    |	'/*' ( options {greedy=false;} : . )* '*/'// {$channel=HIDDEN;}
    ;
    
IDE  :	('a'..'z'|'A'..'Z'|'_'|'.'|'-') ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'.'|'-'|':'|'\\'|' '|'*')*
    ;

INT :	'0'..'9'+
    ;

FLOAT
    :   ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    |   '.' ('0'..'9')+ EXPONENT?
    |   ('0'..'9')+ EXPONENT
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;

STRING
    :  '"' ( ESC_SEQ | ~('\\'|'"') )* '"'
    ;

CHAR:  '\'' ( ESC_SEQ | ~('\''|'\\') ) '\''
    ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

fragment
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

fragment
ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    |   UNICODE_ESC
    |   OCTAL_ESC
    ;

fragment
OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;


