// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genCpp.h"

#define DEFAULT_CLS CGGobjcls

unique_ptr<CGGbasecls> CGGbasecls::getBaseCls(const string& str)
{
	if((str == "INT8") || (str == "INT16") || (str == "UINT16") || (str == "INT32") || (str == "UINT32") || (str == "INT64") || (str == "UINT64") || (str == "int") || (str == "double"))
		return unique_ptr<CGGbasecls>(new CGGpyrdgtcls());
	else if(str == "UINT8")
		return unique_ptr<CGGbasecls>(new CGGpyrUINT8cls());
	else if(str == "bool")
		return unique_ptr<CGGbasecls>(new CGGpyrboolcls());
	else if(str == "time_t")
		return unique_ptr<CGGbasecls>(new CGGpyrtimetcls());
	else if(str == "PString")
		return unique_ptr<CGGbasecls>(new CGGpyrPStringcls());
	else if((str == "CommUtcTime") || (str == "CommSrvTime"))
		return unique_ptr<CGGbasecls>(new CGGpyrUTCtimecls());
	//else if(str == "simple_array")
	//	return unique_ptr<CGGbasecls>(new CGGarraycls());
	//else if(str == "un_unique_ptr")
	//	return unique_ptr<CGGbasecls>(new CGGpointercls());
	//else if(str == "unique_ptr")
	//	return unique_ptr<CGGbasecls>(new CGGsmartptrcls());
	else if(str == "PBlock")
		return unique_ptr<CGGbasecls>(new CGGpyrPBlockcls());
	else if(str == "CommMsgBody")
		return unique_ptr<CGGbasecls>(new CGGpyrCommMsgBodycls());
	else if((str == "vector") || (str == "CommExtVector") || (str == "CommExtUINT32Vector") || (str == "CommExtPStringVector")) 
		return unique_ptr<CGGbasecls>(new CGGvectorcls());
	else if(str == "CommExtStringUINT32PairVector")
		return unique_ptr<CGGbasecls>(new CGGstringunit32pairvectorcls());
	else if(str == "CommExtStringPairVector")
		return unique_ptr<CGGbasecls>(new CGGstringpairvectorcls());
	else if((str == "set") || (str == "CommExtUINT32Set"))
		return unique_ptr<CGGbasecls>(new CGGsetcls());
	else if((str == "map"))
		return unique_ptr<CGGbasecls>(new CGGmapcls());
	else if ((str == "CommExtUINT32MapSet"))
		return unique_ptr<CGGbasecls>(new CGGuint32mapsetcls());
	else if((str == "PStringSet") || (str == "CommExtStringSet")) 
		return unique_ptr<CGGbasecls>(new CGGpyrstringsetcls());
	else if(str == "list") 
		return unique_ptr<CGGbasecls>(new CGGlistcls());
	else
		return unique_ptr<CGGbasecls>(new DEFAULT_CLS);
}
