// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genJavaScript.h"

#define DEFAULT_CLS CGJSobjcls

unique_ptr<CGJSbasecls> CGJSbasecls::getBaseCls(const string& str)
{
	if((str == "INT8") || (str == "UINT16") || (str == "INT16") || (str == "UINT32") || (str == "INT32"))
		return unique_ptr<CGJSbasecls>(new CGJSpyrdgtcls());
	else if((str == "UINT64") || (str == "INT64"))
		return unique_ptr<CGJSbasecls>(new CGJSpyr64cls());
	else if(str == "PString")
		return unique_ptr<CGJSbasecls>(new CGJSpyrStringcls());
	else if(str == "bool")
		return unique_ptr<CGJSbasecls>(new CGJSpyrBoolcls());
	else if(str == "UINT8")
		return unique_ptr<CGJSbasecls>(new CGJSpyrUINT8cls());
	else if((str == "CommUtcTime") || (str == "CommSrvTime") || (str == "CommUtcDate") || (str == "CommSrvDate"))
		return unique_ptr<CGJSbasecls>(new CGJSpyrUTCtimecls());
	else if((str == "vector") || (str == "CommExtVector") || (str == "CommExtPtrVector") || (str == "CommExtUINT32Set") ||
		(str == "set") || (str == "PStringSet") || (str == "CommExtStringSet") || (str == "CommExtUINT32Vector") || (str == "CommExtPStringVector")) 
		return unique_ptr<CGJSbasecls>(new CGJSvectorcls());
	else if(str == "PBlock")
		return unique_ptr<CGJSbasecls>(new CGJSpyrPBlockcls());
	//else if(str == "simple_array")
	//	return unique_ptr<CGJSbasecls>(new CGJSarraycls());
	else if(str == "CommExtUINT32MapSet")
		return unique_ptr<CGJSbasecls>(new CGJSUINT32mapsetcls());
	else if(str == "CommExtStringUINT32UINT32Vector")
		return unique_ptr<CGJSbasecls>(new CGJStringUINT32UINT32Vectorcls());
	else if(str == "CommExtStringUINT32PairVector")
		return unique_ptr<CGJSbasecls>(new CGJStringUINT32PairVectorcls());
	else if (str == "CommExtUINT32Pair")
		return unique_ptr<CGJSbasecls>(new CGJUINT32Paircls());
	else if(str == "CommExtStringPairVector")
		return unique_ptr<CGJSbasecls>(new CGJStringPairVectorcls());
	else
		return unique_ptr<CGJSbasecls>(new DEFAULT_CLS);
}

string CGJSbasecls::typeCppToJavaScript(const string& str)
{
	if((str == "INT8") || (str == "UINT8") || (str == "INT16") || (str == "UINT16") || (str == "INT32") || (str == "UINT32") )
		return "number";
	else if (str == "bool")
		return "boolean";
	else if((str == "INT64") || (str == "UINT64"))
		return "BitMask";
	else if(str == "PString")
		return "string";
	else if(str == "PBlock")
		return "Uint8Array";
	else if(str == "CommSrvTime")
		return "CommUtcTime";
	else if(str == "CommSrvDate")
		return "CommUtcDate";
	else if((str == "vector") || (str == "CommExtPtrVector") || (str == "CommExtVector") || (str == "CommExtUINT32Set") ||
		(str == "set") || (str == "PStringSet") || (str == "CommExtStringSet") || (str == "CommExtUINT32Vector") || (str == "CommExtPStringVector")) 
	{
		return "Set";
	}
	else
		return str;

}
