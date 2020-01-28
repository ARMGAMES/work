grammar commgen;

options
{
    language = C;
}


@lexer::includes
{
	#include "..\\..\\CGapp.h"
	typedef void (*displayRecognitionErrorGenType) (struct ANTLR3_BASE_RECOGNIZER_struct * recognizer, pANTLR3_UINT8 * tokenNames);
}

@parser::includes
{
	#include "commgenLexer.h"
}

@lexer::postinclude
{
char lexergenbuf[2048] = {0};
  
displayRecognitionErrorGenType errOldGenLexer;
void displayRecognitionErrorGenLexer (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	setvbuf(stderr, lexergenbuf, _IOFBF, 1024);
	errOldGenLexer(recognizer, tokenNames);
	throw exception(lexergenbuf);
}
}

@lexer::apifuncs 
{
	errOldGenLexer = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorGenLexer;
}

@parser::postinclude
{
char parsergenbuf[2048] = {0};
   
displayRecognitionErrorGenType errOldGenParser;
void displayRecognitionErrorGenParser (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	setvbuf(stderr, parsergenbuf, _IOFBF, 1024);
	errOldGenParser(recognizer, tokenNames);
	throw exception(parsergenbuf);
}
}

@parser::apifuncs 
{
	errOldGenParser = RECOGNIZER->displayRecognitionError;
	RECOGNIZER->displayRecognitionError = displayRecognitionErrorGenParser;
}

@members
{
	CGdocument* pdoc;
	CGloader* pldr;
}

prog[CGloader* pldr_, CGdocument* pdoc_]
@init
{
	if(!$pdoc_) throw exception("commgen: No document initialized");
	if(!$pldr_) throw exception("commgen: No loader initialized");
	pdoc = 	$pdoc_;
	pldr = 	$pldr_;
}
@after
{
}
	: 	progfield+	
	;

progfield
@declarations
{
}
	:
	 	'global_channels'  		'{' (ID ';' {pldr->addGlobal((char*)$ID.text->chars);})* '}' ';'
	| 	'global_channels_smart'	'{' (ID ';' {pldr->addGlobalSmart((char*)$ID.text->chars);})* '}' ';'
	| 	'channels'  			'{' (ID ';' {pldr->addLocal((char*)$ID.text->chars, pdoc->getName());})* '}' ';'
	| 	'channels_smart'		'{' (ID ';' {pldr->addLocalSmart((char*)$ID.text->chars, pdoc->getName());} )* '}' ';'
	| 	'virtual_channels_smart' 'base' a=ID '{' (b=ID ';' {pldr->addVirtualSmart((char*)$b.text->chars, (char*)$a.text->chars);} )* '}' ';'
	| 	'virtual_channels'  	 'base' a=ID '{' (b=ID ';' {pldr->addVirtual((char*)$b.text->chars, (char*)$a.text->chars);})* '}' ';'
	|	'base_ref_class' '=' a=ID {pldr->addBaseRefClass((char*)$a.text->chars);} ';'
	|	'parse@'ID {pdoc->addParse((char*)$ID.text->chars);} 
	|	('compose@'ID {pdoc->addCompose((char*)$ID.text->chars);}) 
	|	('off@'ID {pdoc->addOff((char*)$ID.text->chars);}) 
	|	(('@'|'all@')ID{pdoc->addParseCompose((char*)$ID.text->chars);}) 
	|	('#on_equal'{pdoc->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ON);})
	|	('#base_equal'{pdoc->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONBASE);})
	|	('#onlog_equal'{pdoc->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONLOG);})
	|	('#baselog_equal'{pdoc->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONBASELOG);})
	|	('#off_equal'{pdoc->setProperty(CG_CLASS_EQUAL_MASK, 0);})
	|	('#on_copyconstructor'{pdoc->setProperty(CG_CLASS_COPY_MASK, CG_CLASS_COPY_ON);})
	|	('#deny_copyconstructor'{pdoc->setProperty(CG_CLASS_COPY_MASK, CG_CLASS_COPY_DENY);})
	|	('#off_copyconstructor'{pdoc->setProperty(CG_CLASS_COPY_MASK, 0);})
	|	('#on_moveconstructor'{pdoc->setProperty(CG_CLASS_MOVE_MASK, CG_CLASS_MOVE_ON);})
	|	('#deny_moveconstructor'{pdoc->setProperty(CG_CLASS_MOVE_MASK, CG_CLASS_MOVE_DENY);})
	|	('#off_moveconstructor'{pdoc->setProperty(CG_CLASS_MOVE_MASK, 0);})
	|	('#on_assignment'{pdoc->setProperty(CG_CLASS_ASSIGN_MASK, CG_CLASS_ASSIGN_ON);})
	|	('#deny_assignment'{pdoc->setProperty(CG_CLASS_ASSIGN_MASK, CG_CLASS_ASSIGN_DENY);})
	|	('#off_assignment'{pdoc->setProperty(CG_CLASS_ASSIGN_MASK, 0);})
	|	('#on_moveassignment'{pdoc->setProperty(CG_CLASS_MASSIGN_MASK, CG_CLASS_MASSIGN_ON);})
	|	('#deny_moveassignment'{pdoc->setProperty(CG_CLASS_MASSIGN_MASK, CG_CLASS_MASSIGN_DENY);})
	|	('#off_moveassignment'{pdoc->setProperty(CG_CLASS_MASSIGN_MASK, 0);})
	|	('#on_copybase'{pdoc->setProperty(CG_CLASS_COPYBASE_MASK, CG_CLASS_COPYBASE_ON);})
	|	('#off_copybase'{pdoc->setProperty(CG_CLASS_COPYBASE_MASK, 0);})
	|	('#on_movebase'{pdoc->setProperty(CG_CLASS_MOVEBASE_MASK, CG_CLASS_MOVEBASE_ON);})
	|	('#off_movebase'{pdoc->setProperty(CG_CLASS_MOVEBASE_MASK, 0);})
	|	('#restrictstr:' c=INT {pdoc->setRestrString((char*)$c.text->chars);})
	|	('#on_validate'{pdoc->setProperty(CG_CLASS_VALIDATE_MASK, CG_CLASS_VALIDATE_ON);})
	|	('#restore_validate'{pdoc->setProperty(CG_CLASS_VALIDATE_MASK, CG_CLASS_VALIDATE_REST);})
	|	('#off_validate'{pdoc->setProperty(CG_CLASS_VALIDATE_MASK, 0);})
	|	('#std_include'{pdoc->setProperty(CG_CLASS_INK_MASK, CG_CLASS_INK_STD);})
	|	('#base_include'{pdoc->setProperty(CG_CLASS_INK_MASK, CG_CLASS_INK_BASE);})
	|	('#all_include'{pdoc->setProperty(CG_CLASS_INK_MASK, CG_CLASS_INK_ALL);})
	|	('#checkparse'{pdoc->setProperty(CG_CLASS_PARSECHK_MASK, CG_CLASS_PARSECHK_ON);})
	|	('#dump'{pdoc->setProperty(CG_CLASS_DUMP_MASK, CG_CLASS_DUMP_ON);})
	|	('#name:' ID  {pdoc->setName((char*)$ID.text->chars);})
	|	('#eol:' d=INT '>' {pdoc->addEmptyLine((char*)$d.text->chars);})
	|	('#eol' {pdoc->addEmptyLine("1");})
	|	('#update_handler'':' ID {pdoc->setUpdateHandler((char*)$ID.text->chars);})
	| 	structure
	| 	COMMENT		{pdoc->addComment((char*)$COMMENT.text->chars);}
	| 	'#include'	'{' (STRING ';' {pdoc->addInclude((char*)$STRING.text->chars);} )* '}' ';'
	| 	'#text'':' e=ID '{' (STRING  {pdoc->addText((char*)$e.text->chars, (char*)$STRING.text->chars);} )* '}' ';'
	| 	'#text''{' (STRING  {pdoc->addText("", (char*)$STRING.text->chars);} )* '}' ';'
 	| 	COMMENTINTERNAL	
	;
	
structure
@declarations
{
	CGstruct* pclass;
}
@init
{
	pclass = nullptr;
}
@after
{
	//pclass->checkSubscription(pldr);
}

	: 	('struct' | 'class') a=ID {pldr->checkClassesNames((char*)$a.text->chars); pclass = (CGstruct*)pdoc->addClass((char*)$a.text->chars);} 
		((b=COMMENT	{pclass->setFormattedTxt((char*)$b.text->chars, true);}) 
	|	('parse@' c=ID {pclass->addParse((char*)$c.text->chars);}) 
	|	('compose@' c=ID {pclass->addCompose((char*)$c.text->chars);}) 
	|	('off@' c=ID {pclass->addOff((char*)$c.text->chars);}) 
	|	(('@'|'all@') c=ID {pclass->addParseCompose((char*)$c.text->chars);}) 
	|	('#javapackage'':' (c=ID {pclass->addJavaPackageSection((char*)$c.text->chars);}) ('.'d=ID {pclass->addJavaPackageSection((char*)$d.text->chars);})*)
	|	('#javaqueryimp'':' (c=ID {pclass->addJavaQueryImplements((char*)$c.text->chars);}) ('.'d=ID {pclass->addJavaQueryImplements((char*)$d.text->chars);})*)
	|	('#javasearchable' {pclass->setJavaSearchable();})
	|	('#on_equal'{pclass->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ON);})
	|	('#base_equal'{pclass->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONBASE);})
	|	('#onlog_equal'{pclass->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONLOG);})
	|	('#baselog_equal'{pclass->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONBASELOG);})
	|	('#off_equal'{pclass->setProperty(CG_CLASS_EQUAL_MASK, 0);})
	|	('#on_copyconstructor'{pclass->setProperty(CG_CLASS_COPY_MASK, CG_CLASS_COPY_ON);})
	|	('#deny_copyconstructor'{pclass->setProperty(CG_CLASS_COPY_MASK, CG_CLASS_COPY_DENY);})
	|	('#off_copyconstructor'{pclass->setProperty(CG_CLASS_COPY_MASK, 0);})
	|	('#on_moveconstructor'{pclass->setProperty(CG_CLASS_MOVE_MASK, CG_CLASS_MOVE_ON);})
	|	('#deny_moveconstructor'{pclass->setProperty(CG_CLASS_MOVE_MASK, CG_CLASS_MOVE_DENY);})
	|	('#off_moveconstructor'{pclass->setProperty(CG_CLASS_MOVE_MASK, 0);})
	|	('#on_assignment'{pclass->setProperty(CG_CLASS_ASSIGN_MASK, CG_CLASS_ASSIGN_ON);})
	|	('#deny_assignment'{pclass->setProperty(CG_CLASS_ASSIGN_MASK, CG_CLASS_ASSIGN_DENY);})
	|	('#off_assignment'{pclass->setProperty(CG_CLASS_ASSIGN_MASK, 0);})
	|	('#on_moveassignment'{pclass->setProperty(CG_CLASS_MASSIGN_MASK, CG_CLASS_MASSIGN_ON);})
	|	('#deny_moveassignment'{pclass->setProperty(CG_CLASS_MASSIGN_MASK, CG_CLASS_MASSIGN_DENY);})
	|	('#off_moveassignment'{pclass->setProperty(CG_CLASS_MASSIGN_MASK, 0);})
	|	('#on_copybase'{pclass->setProperty(CG_CLASS_COPYBASE_MASK, CG_CLASS_COPYBASE_ON);})
	|	('#off_copybase'{pclass->setProperty(CG_CLASS_COPYBASE_MASK, 0);})
	|	('#on_movebase'{pclass->setProperty(CG_CLASS_MOVEBASE_MASK, CG_CLASS_MOVEBASE_ON);})
	|	('#off_movebase'{pclass->setProperty(CG_CLASS_MOVEBASE_MASK, 0);})
	|	('#base_inheritance:' c=ID {pclass->setProperty(CG_CLASS_INH_MASK, CG_CLASS_INH_BASE);pclass->setBaseClass((char*)$c.text->chars);})
	|	('#virtual_inheritance:' c=ID {pclass->setProperty(CG_CLASS_INH_MASK, CG_CLASS_INH_BASEVIRT);pclass->setBaseClass((char*)$c.text->chars);})
	|	('#virtual' {pclass->setProperty(CG_CLASS_INH_MASK, CG_CLASS_INH_VIRT);})
	|	('#template'{pclass->setProperty(CG_CLASS_TEMP_MASK, CG_CLASS_TEMP_ON);})
	|	('#template:' c=ID {pclass->setProperty(CG_CLASS_TEMP_MASK, CG_CLASS_TEMP_ON);pclass->setBaseClass((char*)$c.text->chars);})
	|	('#restrictstr:' c=INT {pclass->setRestrString((char*)$c.text->chars);})
	|	('#on_validate'{pclass->setProperty(CG_CLASS_VALIDATE_MASK, CG_CLASS_VALIDATE_ON);})
	|	('#restore_validate'{pclass->setProperty(CG_CLASS_VALIDATE_MASK, CG_CLASS_VALIDATE_REST);})
	|	('#off_validate'{pclass->setProperty(CG_CLASS_VALIDATE_MASK, 0);})
	|	('#std_include'{pclass->setProperty(CG_CLASS_INK_MASK, CG_CLASS_INK_STD);})
	|	('#base_include'{pclass->setProperty(CG_CLASS_INK_MASK, CG_CLASS_INK_BASE);})
	|	('#all_include'{pclass->setProperty(CG_CLASS_INK_MASK, CG_CLASS_INK_ALL);})
	|	('#checkparse'{pclass->setProperty(CG_CLASS_PARSECHK_MASK, CG_CLASS_PARSECHK_ON);})
	|	('#dump'{pclass->setProperty(CG_CLASS_DUMP_MASK, CG_CLASS_DUMP_ON);})
	|	('#java_all_override'{pclass->setJavaProperty(CG_JAVA_OVERRIDE_MASK, CG_JAVA_OVERRIDE_ALL);})
	|	('#java_parse_override'{pclass->setJavaProperty(CG_JAVA_OVERRIDE_MASK, CG_JAVA_OVERRIDE_PARSE);})
	|	('#java_compose_override'{pclass->setJavaProperty(CG_JAVA_OVERRIDE_MASK, CG_JAVA_OVERRIDE_COMPOSE);})
	|	('#specialclass1'{pclass->setProperty(CG_CLASS_SPECIAL_MASK, CG_CLASS_SPECIAL_1);})
	|	('#modeltype:' c=ID {pclass->setProperty(CG_CLASS_SELFIELD_MASK, CG_CLASS_SELFIELD_ON);pclass->setModelType((char*)$c.text->chars);})
	|	('#javascript_extend:' c=STRING {pclass->setJSExtends((char*)$c.text->chars);})
	|	('#internalclass'{pclass->setProperty(CG_CLASS_INTERNAL_MASK, CG_CLASS_INTERNAL_ON);})
	|	('#javascript_super'{pclass->setProperty(CG_JS_SUPER_MASK, CG_JS_SUPER_ON);})
	|	('#update_handler'':' c=ID {pclass->setUpdateHandler((char*)$c.text->chars);})
	|	('#archived:' c=ID {pclass->setProperty(CG_CLASS_ARCHIVED_MASK, CG_CLASS_ARCHIVED_ON);;pclass->setArcType((char*)$c.text->chars);})
	|	('#java_alias:' c=ID {pclass->setJavaName((char*)$c.text->chars);})
	|	('#java_descr:' c=STRING {pclass->setJavaDesc((char*)$c.text->chars);})
	|	('#on_update'{pclass->setProperty(CG_CLASS_UPDATE_MASK, CG_CLASS_UPDATE_ON);})
	|	('#off_update'{pclass->setProperty(CG_CLASS_UPDATE_MASK, 0);})
	|	('#json'{pclass->setProperty(CG_JN_MASK, CG_JN_ON);})
	|	('#maskDSL'{pclass->setDSLMask(true);})
	|	('#sec_update_after'':' (f=ID {pclass->setUpdateSecurityFuncAfter((char*)$f.text->chars);})
		'{' ((c=INT {pclass->addSecUpdateIntAfter((char*)$c.text->chars);}) | (c=ID {pclass->addSecUpdateAfter((char*)$c.text->chars, pldr);})) (','((d=INT {pclass->addSecUpdateIntAfter((char*)$d.text->chars);}) | (d=ID {pclass->addSecUpdateAfter((char*)$d.text->chars, pldr);})))* '}')
	|	('#sec_update'':' (f=ID {pclass->setUpdateSecurityFunc((char*)$f.text->chars);})
		'{' ((c=INT {pclass->addSecUpdateInt((char*)$c.text->chars);}) | (c=ID {pclass->addSecUpdate((char*)$c.text->chars, pldr);})) (','((d=INT {pclass->addSecUpdateInt((char*)$d.text->chars);}) | (d=ID {pclass->addSecUpdate((char*)$d.text->chars, pldr);})))* '}')
	|	COMMENTINTERNAL
		)*
		'{' 
		(	('private' ':' 		{pclass->addAccess("private");})
		|	('protected' ':'	{pclass->addAccess("protected");})
		|	('public' ':'		{pclass->addAccess("public");})
		|	fieldData[pclass]  
		|	(f=COMMENT	{pclass->addComment((char*)$f.text->chars);})
		| 	'#text'	':' e=ID '{' (f=STRING  {pclass->addText((char*)$e.text->chars, (char*)$f.text->chars);} )* '}' ';'
		| 	'#text''{' (g=STRING  {pclass->addText("", (char*)$g.text->chars);} )* '}' ';'
		|	('#eol:' d=INT {pclass->addEmptyLine((char*)$d.text->chars);})	
		|	('#eol' {pclass->addEmptyLine("1");})
		|	COMMENTINTERNAL
		)*
		'}' 
		';'
	;
	
fieldData[CGstruct* pclass]
@declarations
{
	int ln;
	string cmn, arrsize;
	CGfield* pfield;
	unique_ptr<CGbaseobj> pfobj;
	unique_ptr<CGfieldtype> pftobj;
	bool neg;
}
@init
{
	ln = 0;
	neg=false;
	pfobj = unique_ptr<CGbaseobj>(new CGfield(pclass));
	pfield = (CGfield*)pfobj.get();
	pftobj = unique_ptr<CGfieldtype>(new CGfieldtype(pclass, pfield));
}
	:	(('getter%' a=ID {pfield->setGetter((char*)$a.text->chars, pldr);})   
	|	('setter%' a=ID {pfield->setSetter((char*)$a.text->chars, pldr);})
	|	('movesetter%' a=ID {pfield->setMoveSetter((char*)$a.text->chars, pldr);})
	|	('parse@' a=ID {pfield->addParse((char*)$a.text->chars);}) 
	|	('compose@' a=ID {pfield->addCompose((char*)$a.text->chars);}) 
	|	('off@' a=ID {pfield->addOff((char*)$a.text->chars);}) 
	|	(('@'|'all@') a=ID {pfield->addParseCompose((char*)$a.text->chars);}) 
	|	('#on_equal'{pfield->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ON);})
	|	('#base_equal'{pfield->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONBASE);})
	|	('#onlog_equal'{pfield->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONLOG);})
	|	('#baselog_equal'{pfield->setProperty(CG_CLASS_EQUAL_MASK, CG_CLASS_EQUAL_ONBASELOG);})
	|	('#off_equal'{pfield->setProperty(CG_CLASS_EQUAL_MASK, 0);})
	|	('#restrictstr:' c=INT {pfield->setRestrString((char*)$c.text->chars);})
	|	('#on_validate'{pfield->setProperty(CG_CLASS_VALIDATE_MASK, CG_CLASS_VALIDATE_ON);})
	|	('#restore_validate'{pfield->setProperty(CG_CLASS_VALIDATE_MASK, CG_CLASS_VALIDATE_REST);})
	|	('#off_validate'{pfield->setProperty(CG_CLASS_VALIDATE_MASK, 0);})
	|	('#leaf_node'{pfield->setProperty(CG_CLASS_SUBSCR_MASK, CG_CLASS_SUBSCR_LEAFNODE);})
	|	('#specialclass1'{pfield->setProperty(CG_CLASS_SPECIAL_MASK, CG_CLASS_SPECIAL_1);})
	|	('#java_transient'{pfield->setJavaProperty(CG_JAVA_TRANSIENT_MASK, CG_JAVA_TRANSIENT_ON);}) 
	|	('#java_fieldoverride'{pfield->setJavaProperty(CG_JAVA_FIELDOVERRIDE_MASK, CG_JAVA_FIELDOVERRIDE_ON);}) 
	|	('#javascript_ignore'{pfield->setProperty(CG_JS_IGNORE_MASK, CG_JS_IGNORE_ON);}) 
	|	('#cpp_ignore'{pfield->setProperty(CG_CPP_IGNORE_MASK, CG_CPP_IGNORE_ON);}) 
	|	('#internalclass'{pfield->setProperty(CG_CLASS_INTERNAL_MASK, CG_CLASS_INTERNAL_INCL);})	
	|	('#refclass:' c=ID ':' d=ID {pfield->setProperty(CG_CLASS_REFFIELD_MASK, CG_CLASS_REFFIELD_ON);pfield->setRefClass((char*)$c.text->chars, (char*)$d.text->chars, pldr);})
	|	('#update_handler'':' c=ID {pfield->setUpdateHandler((char*)$c.text->chars);pclass->setProperty(CG_CLASS_UPDATEHANDLER_MASK, CG_CLASS_UPDATEHANDLER_ON);})
	|	('#object_id'{pfield->setProperty(CG_CLASS_OBJECTID_MASK, CG_CLASS_OBJECTID_ON);})
	|	('#version_id'{pfield->setProperty(CG_CLASS_VERSION_MASK, CG_CLASS_VERSION_ON);})
	|	('#hidden'{pfield->setProperty(CG_FIELD_HIDDEN_MASK, CG_FIELD_HIDDEN_ON);})
	|	('#convert_func'':' c=ID {pfield->setSelConvertFuncName((char*)$c.text->chars);})
	|	('#specialfield_func'':' c=ID {pfield->setSpecialFieldFuncName((char*)$c.text->chars);})
	|	('#on_update'{pfield->setProperty(CG_CLASS_UPDATE_MASK, CG_CLASS_UPDATE_ON);})
	|	('#off_update'{pfield->setProperty(CG_CLASS_UPDATE_MASK, 0);})
	|	('#ignore_update'{pfield->setProperty(CG_IGNORE_UPDATE_MASK, CG_IGNORE_UPDATE_ON);})
	|	('#sec_update_add'{pfield->setSecUpdateAdd();})
	|	('#sec_update_after'':' (f=ID {pfield->setUpdateSecurityFuncAfter((char*)$f.text->chars);})
		'{' ((c=INT {pfield->addSecUpdateIntAfter((char*)$c.text->chars);}) | (c=ID {pfield->addSecUpdateAfter((char*)$c.text->chars, pldr);})) (','((d=INT {pfield->addSecUpdateIntAfter((char*)$d.text->chars);}) | (d=ID {pfield->addSecUpdateAfter((char*)$d.text->chars, pldr);})))* '}')
	|	('#sec_update'':' (f=ID {pfield->setUpdateSecurityFunc((char*)$f.text->chars);})
		'{' ((c=INT {pfield->addSecUpdateInt((char*)$c.text->chars);}) | (c=ID {pfield->addSecUpdate((char*)$c.text->chars, pldr);})) (','((d=INT {pfield->addSecUpdateInt((char*)$d.text->chars);}) | (d=ID {pfield->addSecUpdate((char*)$d.text->chars, pldr);})))* '}')
	|	('#update_handler2'':' c=ID {pfield->setUpdateHandler2((char*)$c.text->chars);pclass->setProperty(CG_CLASS_UPDATEHANDLER2_MASK, CG_CLASS_UPDATEHANDLER2_ON);})
	|	('#updver_increase'{pfield->setProperty(CG_UPDVER_INC_MASK, CG_UPDVER_INC_ON);})
	|	('#updater_set'{pfield->setProperty(CG_UPDATER_MASK, CG_UPDATER_ON);})
	|	('#currtime_set'{pfield->setProperty(CG_CURR_TIME_MASK, CG_CURR_TIME_ON);})
	|	('#modify'':' c=ID {pfield->setFieldModificator((char*)$c.text->chars);})
	|	('#java_alias:' c=ID {pfield->setJavaName((char*)$c.text->chars);})
	|	('#javatype_alias:' c=ID {pfield->setJavaType((char*)$c.text->chars);})
	|	('#java_descr:' c=STRING {pfield->setJavaDesc((char*)$c.text->chars);})
	|	('#javafilter'':' c=INT {pfield->setJavaFilterCode((char*)$c.text->chars);} (':' d=ID {pfield->addJavaFilterOperands((char*)$d.text->chars);} (',' e=ID {pfield->addJavaFilterOperands((char*)$e.text->chars);})*)?)
	|	('#javasort'':' c=INT {pfield->setJavaSortCode((char*)$c.text->chars);})
	|	('#javatype'':' c=ID {pftobj->setJavaType((char*)$c.text->chars);})
	|	('#usechannels'{pfield->usePCChannels();})
	)*
		(fieldType[pftobj.get()]
		b=ID  {pfield->setData((char*)$b.text->chars, pldr); pfield=(CGfield*)pclass->addField(pfobj); ln = $b.line; pclass->checkSubscr(pfield, pldr);} 
		('[' ((d=ID {arrsize = (char*)$d.text->chars;}) | (e=INT {arrsize = (char*)$e.text->chars;})) ']' {pftobj->setArray(arrsize, pldr);})? 
		{pfield->setType(pftobj);})
		('=' ((f=STRING {pfield->setDefaultStr((char*)$f.text->chars, pldr);}) | (('-' {neg=true;})? h=INT {pfield->setDefaultInt((char*)$h.text->chars, neg, pldr);}) | (m=ID {pfield->setDefaultInt((char*)$m.text->chars, false, pldr);})))? ';'
		(c=COMMENT	{if(ln == $c.line) pfield->setFormattedTxt((char*)$c.text->chars, true); else pclass->addComment((char*)$c.text->chars);})?
		(COMMENTINTERNAL)?
	;	

fieldType[CGfieldtype* pfType]
@declarations
{
	unique_ptr<CGfieldtype> pftobj1;
}
	:	(('unsigned' {pfType->setUnsigned();}) | ('signed'))? n=ID
		('<'{pftobj1 = unique_ptr<CGfieldtype>(new CGfieldtype(pfType->getClass(), pfType->getField()));} p=fieldType[pftobj1.get()] {pfType->addSubType(pftobj1);} 
		(','{pftobj1 = unique_ptr<CGfieldtype>(new CGfieldtype(pfType->getClass(), pfType->getField()));} q=fieldType[pftobj1.get()] {pfType->addSubType(pftobj1);})* '>')?
		 ('*' {pfType->setPointer();})?
		 {pfType->setType((char*)$n.text->chars, pldr);}
	;

COMMENTINTERNAL
    :  '///' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;} 
    ;

COMMENT
    :  '//' ~('\n'|'\r')*
    |	'/*' ( options {greedy=false;} : . )* '*/'
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
