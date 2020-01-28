grammar cesar_interpretator;

options
{
    language = C;
}

@lexer::preincludes
{
	#include "..\\..\\genRule\\genRule.h"

#ifdef _WIN32
#pragma warning( disable : 4296 )
#endif // _WIN32
}

@lexer::includes
{
	typedef void	(*displayRecognitionErrorInterpType)  (struct ANTLR3_BASE_RECOGNIZER_struct * recognizer, pANTLR3_UINT8 * tokenNames);
}

@parser::preincludes
{
   #include "cesar_interpretatorLexer.h"
}

@lexer::postinclude
{
char lexerbuf[2048] = {0};
  
displayRecognitionErrorInterpType errOldInterpLexer;
void displayRecognitionErrorInterpLexer (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	CLPGByteCodeGen::setError();
	setvbuf(stderr, lexerbuf, _IOFBF, 1024);
	errOldInterpLexer(recognizer, tokenNames);
	throw exception(lexerbuf);
}
}

@lexer::apifuncs 
{
	errOldInterpLexer = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorInterpLexer;
}

@parser::postinclude
{
char parserbuf[2048] = {0};
   
displayRecognitionErrorInterpType errOldInterpParser;
void displayRecognitionErrorInterpParser (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	CLPGByteCodeGen::setError();
	setvbuf(stderr, parserbuf, _IOFBF, 1024);
	errOldInterpParser(recognizer, tokenNames);
	throw exception(parserbuf);
}
}

@parser::apifuncs 
{
	errOldInterpParser = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorInterpParser;
}

@members
{
	CLPGByteCodeGen* prg;
}

prog[CLPGByteCodeGen* rg]
@declarations
{
	unique_ptr<CLPGByteCodeBlock> topBlockWrap;
	CLPGByteCodeBlock* topBlock;
}
@init
{
	prg = rg;
	if(!rg) throw exception("No current rule generator");
	topBlockWrap = unique_ptr<CLPGByteCodeBlock>(new CLPGByteCodeBlock(NULL, rg));
	topBlock = topBlockWrap.get();
	if(!topBlock) throw exception("ByteCodeBlock creation error");
}
@after
{
	prg->setCommand(CLPG_BC_CMD_STOP);
	prg->gotoCheck();
}
	:	(('label' ID ':' {prg->addLabel((char*)$ID.text->chars);})* action[topBlock])+
	;

action[CLPGByteCodeBlock* currBlock]
	:	block[$currBlock]
	|	declareStat[$currBlock] ';' 
	|	declareFunc[$currBlock] ';' 
	|	assignStat[$currBlock] ';'   {prg->setCommand(CLPG_BC_CMD_CLEAR);}
	|	'goto' ID ';' {prg->gotoLabel((char*)$ID.text->chars);}
	|	ifStat[$currBlock]
	|	forStat[$currBlock]
	|	expr[$currBlock] ';' {prg->setCommand(CLPG_BC_CMD_CLEAR);}
	|	';'
	;

declareStat[CLPGByteCodeBlock* currBlock]
@declarations
{
	bool c, i;
}	
@init
{
	c = false;
	i = false;
}
	:	(('const' {c=true;} | ('init' {i=true;})))* a=ID b=ID {$currBlock->createDataObject((char*)$a.text->chars, (char*)$b.text->chars, c, i);} 
		('=' expr[$currBlock]	{$currBlock->assignment((char*)$b.text->chars, $expr.ret, true);})?	 {prg->setCommand(CLPG_BC_CMD_CLEAR);}
	;
	
assignStat[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
	:	ID '=' expr[$currBlock]	{$currBlock->assignment((char*)$ID.text->chars, $expr.ret);$ret = $expr.ret;} 
	;

block[CLPGByteCodeBlock* topBlock]
@declarations
{
	CLPGByteCodeBlock* currBlock;
}
@init
{
	currBlock = new CLPGByteCodeBlock(topBlock, prg);
	if(!currBlock)
		throw exception("Null pointer in 'block' currBlock, interpretator parcer");
}
@after
{
	prg->setCommand(CLPG_BC_CMD_CLEAR);
	delete currBlock;
}
	:	'{' 
		(('label' ID ':' {prg->addLabel((char*)$ID.text->chars);})* action[currBlock])* 
		'}'
	;	

expr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   (a=condAndExpr[$currBlock] {$ret = $a.ret;})
    	( '||' b=condAndExpr[$currBlock] {$ret = $currBlock->binaryLogic($ret, $b.ret, 2);})*
    ;


condAndExpr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	ret = CLPG_BC_GEN_UNDEF;
}
    :   (a=condBitOr[$currBlock]   {$ret = $a.ret;})
        ( '&&' b=condBitOr[$currBlock] {$ret = $currBlock->binaryLogic($ret, $b.ret, 1);})*
    ;

condBitOr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :	(a=condBitXor[$currBlock] {$ret = $a.ret;}) 
    	('|' b=condBitXor[$currBlock] {$ret = $currBlock->binaryMath($ret, $b.ret, 7);})*
    ;

condBitXor[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :	(a=condBitAnd[$currBlock] {$ret = $a.ret;}) 
    	('^' b=condBitAnd[$currBlock] {$ret = $currBlock->binaryMath($ret, $b.ret, 6);})*
    ;

condBitAnd[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   (a=condAtomExpr[$currBlock]  			{$ret = $a.ret;})
        ( '&' b=condAtomExpr[$currBlock]  		{$ret = $currBlock->binaryMath($ret, $b.ret, 5);})*
    ;

condAtomExpr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   (a=sumExpr[$currBlock] {$ret = $a.ret;})
            (  '==' b=sumExpr[$currBlock] {$ret = $currBlock->binaryCompare($ret, $b.ret, 1);}
            |  '<' c=sumExpr[$currBlock] {$ret = $currBlock->binaryCompare($ret, $c.ret, 5);}
            |  '>' d=sumExpr[$currBlock] {$ret = $currBlock->binaryCompare($ret, $d.ret, 3);}
            |  '<='e= sumExpr[$currBlock] {$ret = $currBlock->binaryCompare($ret, $e.ret, 6);}
            |  '>=' f=sumExpr[$currBlock]  {$ret = $currBlock->binaryCompare($ret, $f.ret, 4);}    
	   		|  '!=' g=sumExpr[$currBlock]  {$ret = $currBlock->binaryCompare($ret, $g.ret, 2);}   
            )*
    ;

sumExpr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   (a=multExpr[$currBlock]  			{$ret = $a.ret;})
        ( '+' b=multExpr[$currBlock]  		{$ret = $currBlock->binaryMath($ret, $b.ret, 1);})*
        ( '-' c=multExpr[$currBlock] 		{$ret = $currBlock->binaryMath($ret, $c.ret, 2);})*
    ;

multExpr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   (a=atom[$currBlock]  			{$ret = $a.ret;})
        ( '*' b=atom[$currBlock]  		{$ret = $currBlock->binaryMath($ret, $b.ret, 3);})*
        ( '/' c=atom[$currBlock] 		{$ret = $currBlock->binaryMath($ret, $c.ret, 4);})*
        ( '%' c=atom[$currBlock] 		{$ret = $currBlock->binaryMath($ret, $c.ret, 8);})*
    ;

atom[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
	:	objwithfield[$currBlock]	{$ret = $objwithfield.ret;}
	|	func_stat[$currBlock]		{$ret = $func_stat.ret;}
	|	'(' expr[$currBlock] ')'	{$ret = $expr.ret;}
	|	prefixExpr[$currBlock]		{$ret = $prefixExpr.ret;}
	|	postfixExpr[$currBlock]		{$ret = $postfixExpr.ret;}
	|	setOperand[$currBlock]		{$ret = $setOperand.ret;}
	;

setOperand[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@declarations
{
	bool min;
}	
@init
{
	min = false;
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   
		(('+') | ('-' {min = true;}))?
   		simple_operand  {$ret = $currBlock->setOperand((char*)$simple_operand.text->chars, min, $simple_operand.ret);}
    ;

prefixExpr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@declarations
{
	bool min;
}	
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
    :   
		(( '++' {min = false;}) | ('--' {min = true;}))
   		ID  	{if(min)$ret = $currBlock->minusMinus((char*)$ID.text->chars, true); else $ret = $currBlock->plusPlus((char*)$ID.text->chars, true);}
    ;

postfixExpr[CLPGByteCodeBlock* currBlock] returns[UINT64 ret] 
@init
{
	ret = CLPG_BC_GEN_UNDEF;
}
   :   ID
        (( '++' {$ret = $currBlock->plusPlus((char*)$ID.text->chars, false);}) |
        ( '--'  {$ret = $currBlock->minusMinus((char*)$ID.text->chars, false);}))
    ;

simple_operand returns[UINT64 ret]
	:	'true'	{$ret = CLPG_BC_GEN_OP_TRUE;}
	|	'false'	{$ret = CLPG_BC_GEN_OP_FALSE;}
	|	'null'	{$ret = CLPG_BC_GEN_OP_NULL;}
	|	INT		{$ret = CLPG_BC_GEN_OP_INT;}
	|	STRING	{$ret = CLPG_BC_GEN_OP_STR;}
	|	CHAR	{$ret = CLPG_BC_GEN_OP_CHAR;}
	|	ID		{$ret = CLPG_BC_GEN_OP_VAR;}
	;

ifStat[CLPGByteCodeBlock* currBlock]
@declarations
{
	string lab1, lab2;
	bool elseExists;
}	
@init
{
	prg->generateLabel(lab1);
	prg->generateLabel(lab2);
	elseExists = false;
}
	:	'if' '(' (expr[$currBlock] 		{prg->checkDataType($expr.ret, CLPG_BC_VAR_BOOL);prg->gotoLabel(lab1, 1);prg->setCommand(CLPG_BC_CMD_POP);}) ')' (action[$currBlock]) 
		('else' {prg->gotoLabel(lab2);prg->addLabel(lab1);elseExists = true;} action[$currBlock])?	{if(elseExists)prg->addLabel(lab2);else prg->addLabel(lab1);}
	;

forStat[CLPGByteCodeBlock* currBlock]
@declarations
{
	string lab1, lab2, lab3, lab4;
}	
@init
{
	prg->generateLabel(lab1);
	prg->generateLabel(lab2);
	prg->generateLabel(lab3);
	prg->generateLabel(lab4);
}
	:	'for' '(' (assignStat[$currBlock] | declareStat[$currBlock]) ';' {prg->addLabel(lab1);}
		(a=expr[$currBlock]) ';'	{prg->checkDataType($a.ret, CLPG_BC_VAR_BOOL);prg->gotoLabel(lab2, 1);prg->gotoLabel(lab3, 2);prg->setCommand(CLPG_BC_CMD_POP);prg->addLabel(lab4);}
		(b=atom[$currBlock] | b=assignStat[$currBlock]) ')'	{prg->checkDataType($b.ret, CLPG_BC_VAR_INT);prg->gotoLabel(lab1);prg->addLabel(lab3);} 
		(action[$currBlock])		{prg->gotoLabel(lab4);prg->addLabel(lab2);}
	;

objwithfield[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
	:	a=ID '.' b=ID {$ret = $currBlock->firstField((char*)$a.text->chars,(char*)$b.text->chars);}
		((d=objnextfield[$currBlock, $ret] {$ret = $d.ret;}) |
		('[' setOperand[$currBlock] ']' {$ret = $currBlock->indexField($setOperand.ret,(char*)$a.text->chars,(char*)$b.text->chars);} (f=objnextfield[$currBlock, $ret] {$ret = $f.ret;})?) | 
		('.' 'size''('')'{$ret = $currBlock->sizeField((char*)$a.text->chars,(char*)$b.text->chars);}))? 
	;

objnextfield[CLPGByteCodeBlock* currBlock, UINT64 pret] returns[UINT64 ret]
@init
{
	$ret = CLPG_BC_GEN_UNDEF;
}
	:	'.' ID {$ret = $currBlock->nextField((char*)$ID.text->chars,$pret);}
		((a=objnextfield[$currBlock, $ret] {$ret = $a.ret;}) |
		('[' setOperand[$currBlock] ']' {$ret = $currBlock->indexFieldNext($setOperand.ret,(char*)$ID.text->chars,$pret);} (d=objnextfield[$currBlock, $ret] {$ret = $d.ret;})?) | 
		('.' 'size''('')'{$ret = $currBlock->sizeFieldNext((char*)$ID.text->chars,$pret);}))? 
	;

declareFunc[CLPGByteCodeBlock* currBlock]
@declarations
{
	const char* res;
	vector<UINT64> pars;
}	
@init
{
	res = NULL;
}
	:	'function' (a=ID {res = (char*)$a.text->chars;})? b=ID  
		'(' (func_param_decl[pars] (',' func_param_decl[pars] )*)?  ')' {prg->funcDecl((char*)$b.text->chars, res, pars);} 
	;
	
func_stat[CLPGByteCodeBlock* currBlock] returns[UINT64 ret]
@declarations
{
	vector<UINT64> pars;
	UINT64 c;
}	
@init
{
	c = 0;
	$ret = CLPG_BC_GEN_UNDEF;
}
	:	ID '(' (func_param[$currBlock, pars, c] (',' func_param[$currBlock, pars, c] )*)? ')'	{$ret = prg->funcRun((char*)$ID.text->chars, pars, c);}
	;

func_param_decl[vector<UINT64>& cpars]
@declarations
{
	UINT64 d;
}	
	:	ID  {$cpars.push_back(prg->makeFirst((char*)$ID.text->chars, d, false));}
	;
		
func_param[CLPGByteCodeBlock* currBlock, vector<UINT64>& rpars, UINT64& c]
	:	atom[$currBlock]   {$rpars.push_back($atom.ret);++$c;}
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

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
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

