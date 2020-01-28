// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJSarraycls_h_included
#define CGJSarraycls_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGJSarraycls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		res.push_back(string(fld->getName()).append(":").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript))
			.append("[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string(name).append(" = commMsgPar.parse").append(fieldType->getFieldTypeStr()).append("();\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string("commMsg.compose").append(fieldType->getFieldTypeStr()).append("(").append(name).append(");\n"));
		return res;
	}
	virtual bool isLeaf(void) 
	{
		return false;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));

		res.push_back(string("\tsz = parser.parseUINT32();\n"));
		res.push_back(string("\tfor(let i=0; i<sz; i++) {\n"));

		unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());

		if(bcls->isObject())
		{
			res.push_back(string("\t\tlet obj = new ").append(fld->getZeroSubType()->getFieldTypeStr()).append("();\n"));
			res.push_back(string("\t\tthis.").append(name).append(".push(obj.").append(bcls->fillParseIntrI(fld->getZeroSubType())).append(");\n"));
		}
		else
		{
			res.push_back(string("\t\tthis.").append(name).append(".push(").append(bcls->fillParseIntrE(fld->getZeroSubType(), name)).append(");\n"));
		}

		res.push_back(string("\t}\n"));
		//res.push_back(string("\t}\n"));

		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;

		res.push_back(string("sz = parser.parseUINT32();\n"));
		res.push_back(string("for(let i=0; i<sz; i++) {\n"));

		unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());

		if(bcls->isObject())
		{
			res.push_back(string("\tlet obj = new ").append(fld->getZeroSubType()->getFieldTypeStr()).append("();\n"));
			res.push_back(string("\tthis.").append(name).append(".push(obj.").append(bcls->fillParseIntrI(fld->getZeroSubType())).append(");\n"));
		}
		else
		{
			res.push_back(string("\tthis.").append(name).append(".push(").append(bcls->fillParseIntrE(fld->getZeroSubType(), name)).append(");\n"));
		}


		res.push_back(string("\t}\n"));

		return res;
	}

};

#endif	// CGJSarraycls_h_included
