// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genJava.h"

#define DEFAULT_CLS CGJobjcls

//string CGJbasecls::fillFieldType(CGfieldtype* fieldType)
//{
//	const string str = fieldType->getFieldType();
//	if((str == "INT8") || (str == "UINT8"))
//		return "byte";
//	else if((str == "INT16") || (str == "UINT16"))
//		return "short";
//	else if((str == "INT32") || (str == "UINT32"))
//		return "int";
//	else if((str == "INT64") || (str == "UINT64"))
//		return "long";
//	else if(str == "bool")
//		return "boolean";
//	else if(str == "PString")
//		return "String";
//	else if(str == "PBlock")
//		return "byte[]";
//	else if((str == "un_unique_ptr") || (str == "unique_ptr"))
//		return fieldType->getSubTypes()[0]->getFieldType();
//	else if((str == "vector") || (str == "CommExtPtrVector") || (str == "CommExtVector") || (str == "set")) 
//	{
//		return string("List<").append(fillFieldType(fieldType->getSubTypes()[0].get())).append(">");
//	}
//	else if((str == "PStringSet") || (str == "CommExtStringSet")) 
//	{
//		return string("List<String>");
//	}
//	else
//		return str;
//}

string CGJbasecls::typeCppToJava(const string& str)
{
	if((str == "INT8") || (str == "UINT8"))
		return "byte";
	else if((str == "INT16") || (str == "UINT16"))
		return "short";
	else if((str == "INT32") || (str == "UINT32"))
		return "int";
	else if((str == "INT64") || (str == "UINT64"))
		return "long";
	else if(str == "bool")
		return "boolean";
	else if(str == "PString")
		return "String";
	else if((str == "PBlock") || (str == "CommMsgBody"))
		return "byte[]";
	else if((str == "vector") || (str == "CommExtUINT32Vector") || (str == "CommExtVector") || 
		(str == "list") || (str == "CommExtPStringVector"))// || (str == "PStringSet") || (str == "CommExtStringSet")) 
	{
		return "List";
	}
	else if ((str == "set") || (str == "PStringSet") || (str == "CommExtStringSet") || (str == "CommExtUINT32Set"))
	{
		return "Set";
	}
	else if ((str == "map"))
	{
		return "Map";
	}
	else if ((str == "pair")|| (str == "CommExtStringPair"))
	{
		return "Pair";
	}
	else if ((str == "CommUtcTime") || (str == "CommSrvTime") || (str == "CommUtcDate") || (str == "CommSrvDate"))
	{
		return "Date";
	}
	else
	{
		//if (str.compare(0, 2, "CE"))
			return str;
		//else
		//	return str.substr(2);
	}

}

bool CGJbasecls::isMultFilter(const string& str)
{
	if ((str == "INT8") || (str == "UINT8") || (str == "INT16") || (str == "UINT16") || (str == "INT32") || (str == "UINT32") || (str == "INT64") || (str == "UINT64") || (str == "PString"))
		return true;
	else
		return false;
}

string CGJbasecls::simpleToObj(const string& str)
{
	if (str == "byte")
		return "Byte";
	else if (str == "short")
		return "Short";
	else if (str == "int")
		return "Integer";
	else if (str == "long")
		return "Long";
	else if (str == "bool")
		return "Boolean";
	if (str == "byte[]")
		return "Set<Byte>";
	else if (str == "short[]")
		return "Set<Short>";
	else if (str == "int[]")
		return "Set<Integer>";
	else if (str == "long[]")
		return "Set<Long>";
	else if (str == "bool[]")
		return "Set<Boolean>";
	else
		return str;
}

string CGJbasecls::typeCommonJava(const string& str)
{
	if ((str == "INT8") || (str == "UINT8"))
		return "BYTE";
	else if (str == "bool")
		return "BOOL";
	else if (str == "PString")
		return "String";
	else if (str == "PBlock")
		return "VarBlock";
	else if ((str == "CommUtcTime") || (str == "CommSrvTime"))
		return "UtcTime";
	else if ((str == "CommUtcTime") || (str == "CommSrvTime"))
		return "UtcTime";
	else if ((str == "CommUtcDate") || (str == "CommSrvDate") || (str == "CommUtcDate") || (str == "CommSrvDate"))
		return "UtcDate";
	else if((str == "vector") || (str == "CommExtPtrVector") || (str == "CommExtVector") || 
		(str == "list"))// || (str == "PStringSet") || (str == "CommExtStringSet")) 
	{
		return "List";
	}
	else if ((str == "set") || (str == "PStringSet") || (str == "CommExtStringSet") || (str == "CommExtUINT32Set"))
	{
		return "Set";
	}
	else if ((str == "map"))
	{
		return "Map";
	}
	else if ((str == "pair"))
	{
		return "Pair";
	}
	else
		return str;

}

unique_ptr<CGJbasecls> CGJbasecls::getBaseCls(const string& str)
{
	if ((str == "UINT16") || (str == "INT16"))
		return unique_ptr<CGJbasecls>(new CGJpyrUINT16cls());
	if ((str == "UINT32") || (str == "INT32") )
		return unique_ptr<CGJbasecls>(new CGJpyrUINT32cls());
	if ((str == "UINT64") || (str == "INT64"))
		return unique_ptr<CGJbasecls>(new CGJpyrUINT64cls());
	else if((str == "INT8") || (str == "UINT8"))
		return unique_ptr<CGJbasecls>(new CGJpyrUINT8cls());
	else if(str == "bool")
		return unique_ptr<CGJbasecls>(new CGJpyrBOOLcls());
	else if(str == "PString")
		return unique_ptr<CGJbasecls>(new CGJpyrStringcls());
	else if(str == "CommMsgBody")
		return unique_ptr<CGJbasecls>(new CGJpyrCommMsgBodycls());
	else if ((str == "CommUtcTime") || (str == "CommSrvTime"))
		return unique_ptr<CGJbasecls>(new CGJpyrCommUtcTimecls());
	else if ((str == "CommUtcDate") || (str == "CommSrvDate"))
		return unique_ptr<CGJbasecls>(new CGJpyrCommUtcDatecls());
	else if(str == "PBlock")
		return unique_ptr<CGJbasecls>(new CGJpyrPBlockcls());
	else if((str == "CommExtStringPair") || (str == "CommExtStringPairVector"))
		return unique_ptr<CGJbasecls>(new CGJpyrStringPaircls());
	else if(str == "CommExtUINT32Pair")
		return unique_ptr<CGJbasecls>(new CGJpyrUINT32Paircls());
	else if((str == "CommExtStringUINT32Pair") || (str == "CommExtStringUINT32PairVector"))
		return unique_ptr<CGJbasecls>(new CGJpyrStringUint32Paircls());
	else if ((str == "vector") || (str == "CommExtVector") || (str == "CommExtUINT32Vector") || (str == "CommExtPStringVector") || (str == "CommExtStringPairVector") || (str == "list"))
		return unique_ptr<CGJbasecls>(new CGJvectorcls());
	else if ((str == "PStringSet") || (str == "CommExtStringSet") || (str == "set") || (str == "CommExtUINT32Set"))
		return unique_ptr<CGJbasecls>(new CGJsetcls());
	else
		return unique_ptr<CGJbasecls>(new DEFAULT_CLS);
}
