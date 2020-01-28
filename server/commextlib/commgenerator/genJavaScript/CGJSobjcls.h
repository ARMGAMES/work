// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJSobjcls_h_included
#define CGJSobjcls_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGJSobjcls :public CGJSbasecls
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
		if(fld->isArray())
		{
			res.push_back(string("private _parse_").append(name).append("(node:CommNode, path:number[]):void {\n"));
			res.push_back(string("\tfor(let i=0, i<").append(to_string(fld->getFieldType()->getArraySize())).append("; i++) {\n"));
			res.push_back(string("\t\tthis.").append(name).append("[i].parseNode(node.findSubnode([i]), i);\n"));
			res.push_back(string("\t}\n"));
		}
		else
		{
			res.push_back(string("private _parse_").append(name).append("(node:CommNode, path:number[]):void {\n"));
			res.push_back(string("\tthis.").append(name).append(".parseNode(node, path);\n"));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
		res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
		res.push_back(string("\tbreak;\n"));
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
		res.push_back(string("\tif(sz > 0)\n"));
		res.push_back(string("\t\tthis[propname]").append(fillParseIntrE(fieldType, "")).append(";\n"));
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
		return string(name).append(".").append(fillParseIntrI(fieldType)).append("");
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
	virtual bool isObject(void) override
	{
		return true;
	}

};

#endif	// CGJSobjcls_h_included
