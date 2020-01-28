// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJSbasecls_h_included
#define CGJSbasecls_h_included

//******************************************************************************
// Base class for text generators
//******************************************************************************
struct CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) abstract;
	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) abstract;
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) abstract;
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tthrow 'This type is not supported';\n"));
		return res;
	}
	virtual string fillParseIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string();
	}
	virtual string fillParseIntrI(const CGfieldtype* fieldType) 
	{
		return string();
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tthrow 'This type is not supported';\n"));
		return res;
	}
	virtual string fillComposeIntrE(const CGfieldtype* fieldType, const string& name) 
	{
		return string();
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():").append(fld->getFieldType()->getFullTypeString(typeCppToJavaScript)).append("{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:").append(fld->getFieldType()->getFullTypeString(typeCppToJavaScript)).append("){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}
	virtual bool isObject(void)
	{
		return false;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) abstract;
	virtual bool isLeaf(void) abstract;
	virtual string fillObjectId(const CGfield* fld)
	{
		return string();
	}
	virtual string fillVersionId(const CGfield* fld)
	{
		return string();
	}
	static string typeCppToJavaScript(const string& str);
	static unique_ptr<CGJSbasecls> getBaseCls(const string& str);
};

#endif	// CGJSbasecls_h_included
