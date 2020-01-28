// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJSsimplecls_h_included
#define CGJSsimplecls_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGJSpyrdgtcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript))
				.append("[] = [];\n"));
			//.append("[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append(";\n"));
			else
				res.push_back(string(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append(";\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
			res.push_back(string("\tthis.").append(name).append("[idx] = new CommMsgParser(msgbody).parse").append(fieldType->getFieldTypeStr()).append("();\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tthis.").append(name).append(" = new CommMsgParser(msgbody).parse").append(fieldType->getFieldTypeStr()).append("();\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	//virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	//{
	//	vector<string> res;
	//	res.push_back(string("case \"").append(name).append("\":\n"));
	//	res.push_back(string("\tthis[propname] = parser.parse").append(fieldType->getFieldTypeStr()).append("();\n"));
	//	res.push_back(string("\tbreak;\n"));
	//	return res;
	//}

	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		if(fld->isArray())
		{
			res.push_back(string("\tsz = parser.parseUINT32();\n"));
			res.push_back(string("\tfor(let i=0; i<sz; i++) {\n"));

			res.push_back(string("\t\tthis.").append(name).append(".push(").append(fillParseIntrE(fieldType, name)).append(");\n"));

			res.push_back(string("\t}\n"));
		}
		else
		{
			res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		}

		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("sz = parser.parseUINT32();\n"));
			res.push_back(string("for(let i=0; i<sz; i++) {\n"));

			res.push_back(string("\tthis.").append(name).append(".push(").append(fillParseIntrE(fieldType, name)).append(");\n"));

			res.push_back(string("}\n"));
		}
		else
		{
			res.push_back(string("this.").append(name).append(" = parser.parse").append(fieldType->getFieldTypeStr()).append("();\n"));
		}
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("parser.").append(fillParseIntrI(fieldType)).append("()");
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parse").append(fieldType->getFieldTypeStr());
	}

	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy.").append(fillComposeIntrE(fieldType, name)).append("(this[propname]);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string("compose").append(fieldType->getFieldTypeStr());
	}

	virtual bool isLeaf(void) 
	{
		return true;
	}
	virtual string fillObjectId(const CGfield* fld)
	{
		string str;
		str.append("compose").append(fld->getFieldTypeStr()).append("(this.").append(fld->getName()).append(")");
		return str;
	}
	virtual string fillVersionId(const CGfield* fld)
	{
		string str;
		str.append("compose").append(fld->getFieldTypeStr()).append("(this.").append(fld->getName()).append(")");
		return str;
	}
};

struct CGJSpyr64cls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript))
				.append("[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append(";\n"));
			else
				res.push_back(string(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append(";\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
			res.push_back(string("\tthis.").append(name).append("[idx] = new BitMask(new CommMsgParser(msgbody).parse").append(fieldType->getFieldTypeStr()).append("());\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tthis.").append(name).append(" = new BitMask(new CommMsgParser(msgbody).parse").append(fieldType->getFieldTypeStr()).append("());\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("this.").append(name).append(" = new BitMask(parser.parse").append(fieldType->getFieldTypeStr()).append("());\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("new BitMask(parser.").append(fillParseIntrI(fieldType)).append("())");
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parse").append(fieldType->getFieldTypeStr());
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy.").append(fillComposeIntrE(fieldType, name)).append("(this[propname]);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string("compose").append(fieldType->getFieldTypeStr());
	}
	virtual bool isLeaf(void) 
	{
		return true;
	}

};

struct CGJSpyrStringcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":string[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":string;\n"));
			else
				res.push_back(string(fld->getName()).append(":string;\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
			res.push_back(string("\tthis.").append(name).append("[idx] = new CommMsgParser(msgbody).parseString();\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tthis.").append(name).append(" = new CommMsgParser(msgbody).parseString();\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	//virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	//{
	//	vector<string> res;
	//	res.push_back(string("case \"").append(name).append("\":\n"));
	//	res.push_back(string("\tthis[propname] = parser.parseString();\n"));
	//	res.push_back(string("\tbreak;\n"));
	//	return res;
	//}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		//res.push_back(string("\tthis[propname] = parser.").append(fillParseIntr(fieldType)).append("();\n"));
		res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("this.").append(name).append(" = parser.parseString();\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("parser.").append(fillParseIntrI(fieldType)).append("()");
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parseString");
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy.").append(fillComposeIntrE(fieldType, name)).append("(this[propname]);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string("composeString");
	}
	virtual bool isLeaf(void) 
	{
		return true;
	}
	virtual string fillObjectId(const CGfield* fld)
	{
		string str;
		str.append("composeString(this.").append(fld->getName()).append(")");
		return str;
	}
};

struct CGJSpyrBoolcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":boolean[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":boolean;\n"));
			else
				res.push_back(string(fld->getName()).append(":boolean;\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
			res.push_back(string("\tthis.").append(name).append("[idx] = new CommMsgParser(msgbody).parseBOOL();\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tthis.").append(name).append(" = new CommMsgParser(msgbody).parseBOOL();\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		//res.push_back(string("\tthis[propname] = parser.").append(fillParseIntr(fieldType)).append("();\n"));
		res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("this.").append(name).append(" = parser.parseBOOL();\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("parser.").append(fillParseIntrI(fieldType)).append("()");
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parseBOOL");
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy.").append(fillComposeIntrE(fieldType, name)).append("(this[propname]);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string("composeBOOL");
	}
	virtual bool isLeaf(void) 
	{
		return true;
	}
};

struct CGJSpyrUINT8cls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":number[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":number;\n"));
			else
			res.push_back(string(fld->getName()).append(":number;\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
			res.push_back(string("\tthis.").append(name).append("[idx] = new CommMsgParser(msgbody).parseBYTE();\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tthis.").append(name).append(" = new CommMsgParser(msgbody).parseBYTE();\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		//res.push_back(string("\tthis[propname] = parser.").append(fillParseIntr(fieldType)).append("();\n"));
		res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("this.").append(name).append(" = parser.parseBYTE();\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("parser.").append(fillParseIntrI(fieldType)).append("()");
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parseBYTE");
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy.").append(fillComposeIntrE(fieldType, name)).append("(this[propname]);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string("composeBYTE");
	}
	virtual bool isLeaf(void) 
	{
		return true;
	}
	virtual string fillObjectId(const CGfield* fld)
	{
		string str;
		str.append("composeByte(this.").append(fld->getName()).append(")");
		return str;
	}
	virtual string fillVersionId(const CGfield* fld)
	{
		string str;
		str.append("composeByte(this.").append(fld->getName()).append(")");
		return str;
	}
};

struct CGJSpyrUTCtimecls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":Array<").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript))
				.append("> = new Array<").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append(">(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript))
					.append("= new ").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append("();\n"));
			else
				res.push_back(string(fld->getName()).append(":").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript))
					.append("= new ").append(fld->getFieldType()->getFieldTypeStr(typeCppToJavaScript)).append("();\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//if(fld->isArray())
		//{
		//	res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
		//	res.push_back(string("\t").append(name).append("[idx].parse(parser);\n"));
		//}
		//else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tlet parser = new CommMsgParser(msgbody);\n"));
			res.push_back(string("\tthis.").append(name).append(".parse(parser);\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		//res.push_back(string("\tthis[propname] = this.").append(name).append(".parse(parser);\n"));
		res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("this.").append(name).append(".parse(parser);\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("this.").append(name).append(".").append(fillParseIntrI(fieldType));
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parse(parser)");
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy = this.").append(fillComposeIntrE(fieldType, name)).append(".compose(mbdy);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string(name);
	}
	virtual bool isLeaf(void) 
	{
		return true;
	}
};

struct CGJSpyrPBlockcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string(fld->getName()).append(":Uint8Array[] = new Array(").append(to_string(fld->getFieldType()->getArraySize())).append(");\n"));
		}
		else
		{
			if(fld->getUpdateHandler().size())
				res.push_back(string("private _").append(fld->getName()).append(":Uint8Array;\n"));
			else
				res.push_back(string(fld->getName()).append(":Uint8Array;\n"));
		}
		return res;
	}
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer, idx:number):void {\n"));
			res.push_back(string("\tthis.").append(name).append("[idx] = new CommMsgParser(msgbody).parseVarBlock();\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(msgbody:ArrayBuffer):void {\n"));
			res.push_back(string("\tthis.").append(name).append(" = new CommMsgParser(msgbody).parseVarBlock();\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		if(fld->isArray())
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		else
			res.push_back(string("\tthis._parse_").append(name).append("(leafnode);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		//res.push_back(string("\tthis[propname] = parser.").append(fillParseIntr(fieldType)).append("();\n"));
		res.push_back(string("\tthis[propname] = ").append(fillParseIntrE(fieldType, name)).append(";\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("this.").append(name).append(" = parser.parseVarBlock();\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) override
	{
		return string("parser.").append(fillParseIntrI(fieldType)).append("()");
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) override
	{
		return string("parseVarBlock");
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tmbdy.").append(fillComposeIntrE(fieldType, name)).append("(this[propname]);\n"));
		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string("composeVarBlock");
	}
	virtual bool isLeaf(void) 
	{
		return true;
	}
};

#endif	// CGJSsimplecls_h_included
