grammar commdata;

options
{
    language = C;
}

@lexer::includes
{
	#include "..\\..\\CGapp.h"
	typedef void (*displayRecognitionErrorDataType) (struct ANTLR3_BASE_RECOGNIZER_struct * recognizer, pANTLR3_UINT8 * tokenNames);
	
#ifdef _WIN32
#pragma warning( disable : 4296 )
#endif // _WIN32
}

@parser::includes
{
	#include "commdataLexer.h"
}

@lexer::postinclude
{
char lexerdatabuf[2048] = {0};
  
displayRecognitionErrorDataType errOldDataLexer;
void displayRecognitionErrorDataLexer (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	setvbuf(stderr, lexerdatabuf, _IOFBF, 1024);
	errOldDataLexer(recognizer, tokenNames);
	throw exception(lexerdatabuf);
}
}

@lexer::apifuncs 
{
	errOldDataLexer = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorDataLexer;
}

@parser::postinclude
{
char parserdatabuf[2048] = {0};
   
displayRecognitionErrorDataType errOldDataParser;
void displayRecognitionErrorDataParser (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	setvbuf(stderr, parserdatabuf, _IOFBF, 1024);
	errOldDataParser(recognizer, tokenNames);
	throw exception(parserdatabuf);
}
}

@parser::apifuncs 
{
	errOldDataParser = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorDataParser;
}

@members
{
	CGloader* plder;
}

prog[CGloader* plder_]
@init
{
	if(!$plder_) throw exception("commdata: No loader initialized");
	plder = $plder_;
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

	:	('[CONTAINERS]'
	 	(subFieldType
	| 	COMMENTEX				
	| 	COMMENTINTERNAL	
		)* '[END]')
	;
	
subFieldType
@declarations
{
	gsSubFieldTypes ftypes;
}
	:(a=STRING {ftypes.name = plder->correctString((char*)$a.text->chars);} ',' b=INT {ftypes.subFieldsAmount = stoi((char*)$b.text->chars);} ',' 
	 (('true' {ftypes.directIndex = true;})|('false'{ftypes.directIndex = false;})) ',' 
	 c=STRING {ftypes.subTypeForIndexed = plder->correctString((char*)$c.text->chars);} ',' d=STRING {ftypes.baseContainerType = plder->correctString((char*)$d.text->chars);} ';' {plder->addContainersData(ftypes);})
	;
	
COMMENTINTERNAL
    :  '///' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;} 
    ;

COMMENTEX
    :  '//' ~('\n'|'\r')* //'\r'? '\n' //{$channel=HIDDEN;} '//' 
    |  ';' ~('\n'|'\r')* //'\r'? '\n' //{$channel=HIDDEN;} '//' 
    |	'/*' ( options {greedy=false;} : . )* '*/'// {$channel=HIDDEN;}
    ;
    
ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')* 
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



