// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJvector_h_included
#define CGJvector_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGJSvectorcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->getUpdateHandler().size())
			res.push_back(string("private _").append(fld->getName()).append(":Set<").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript))
			.append(">= new Set<").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript)).append(">();\n"));
		else
			res.push_back(string(fld->getName()).append(":Set<").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript))
			.append(">= new Set<").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript)).append(">();\n"));
		return res;
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():Set<").append(fld->getFieldType()->getFullTypeString(typeCppToJavaScript)).append(">{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:Set<").append(fld->getFieldType()->getFullTypeString(typeCppToJavaScript)).append(">){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("private _parse_").append(name).append("(node:CommNode, path:number[]):void {\n"));
		res.push_back(string("\tlet sz = node.getSize();\n"));
		res.push_back(string("\tfor(let i=0; i<sz; i++) {\n"));
		unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());
		if(bcls->isLeaf())
		{
			//res.push_back(string("\t\tlet a = new ").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript)).append("();\n"));
			res.push_back(string("\t\tlet a = new CommMsgParser(node.findSubnode([i]).leaf).").append(bcls->fillParseIntrI(fld->getZeroSubType())).append("();\n"));
			res.push_back(string("\t\tif(a)\n"));
			res.push_back(string("\t\t\tthis.").append(name).append(".add(a);\n"));
		}
		else
		{
			res.push_back(string("\t\tlet a = new ").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript)).append("();\n"));
			res.push_back(string("\t\ta.parseNode(node.findSubnode([i]));\n"));
			res.push_back(string("\t\tthis.").append(name).append(".add(a);\n"));
		}
		res.push_back(string("\t}\n"));
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

		//res.push_back(string("\tthis[propname] = parser.").append(fillParseIntrI(fieldType)).append("();\n"));

		//res.push_back(string("\t{\n"));
		res.push_back(string("\tsz = parser.parseUINT32();\n"));
		res.push_back(string("\tfor(let i=0; i<sz; i++) {\n"));

		unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());

		if(bcls->isObject())
		{
			res.push_back(string("\t\tlet obj = new ").append(fld->getZeroSubType()->getFieldTypeStr()).append("();\n"));
			res.push_back(string("\t\tthis.").append(name).append(".add(obj.").append(bcls->fillParseIntrI(fld->getZeroSubType())).append(");\n"));
			//res.push_back(string("\t\tif(a) this[propname].add(a);\n"));
		}
		else
		{
			res.push_back(string("\t\tthis.").append(name).append(".add(").append(bcls->fillParseIntrE(fld->getZeroSubType(), name)).append(");\n"));
		}

		res.push_back(string("\t}\n"));
		//res.push_back(string("\t}\n"));

		res.push_back(string("\tbreak;\n"));
		return res;
	}
	//virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	//{
	//	vector<string> res;
	//	res.push_back(string("case \"").append(name).append("\":\n"));

	//	//res.push_back(string("\tlet sz = ").append(name).append((".size);\n")));
	//	//res.push_back(string("\tmbdy.composeUINT32(sz.size);\n"));

	//	res.push_back(string("\t//TODO\n"));
	//	res.push_back(string("\tbreak;\n"));
	//	return res;
	//}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;

		//res.push_back(string("{\n"));
		res.push_back(string("sz = parser.parseUINT32();\n"));
		res.push_back(string("for(let i=0; i<sz; i++) {\n"));

		unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());

		if(bcls->isObject())
		{
			res.push_back(string("\tlet obj = new ").append(fld->getZeroSubType()->getFieldTypeStr()).append("();\n"));
			res.push_back(string("\tthis.").append(name).append(".add(obj.").append(bcls->fillParseIntrI(fld->getZeroSubType())).append(");\n"));
			//res.push_back(string("\t").append(name).append(".add(this[propname]").append(bcls->fillParseIntrE(fld->getZeroSubType(), "[i]")).append(");\n"));
		}
		else
		{
			res.push_back(string("\tthis.").append(name).append(".add(").append(bcls->fillParseIntrE(fld->getZeroSubType(), name)).append(");\n"));
		}


		res.push_back(string("\t}\n"));
		//res.push_back(string("}\n"));

		return res;
	}
	virtual string fillVersionId(const CGfield* fld)
	{
		string str;
		str.append("composeUINT32(this.").append(fld->getName()).append(".size)");
		return str;
	}
};

struct CGJSUINT32mapsetcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->getUpdateHandler().size())
			res.push_back(string("private _").append(fld->getName()).append(":Map<number, Set<number>>= new Map<number, Set<number>>();\n"));
		else
			res.push_back(string(fld->getName()).append(":Map<number, Set<number>>= new Map<number, Set<number>>();\n"));
		return res;
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():Map<number, Set<number>>{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:Map<number, Set<number>>){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
	//	res.push_back(string("private _parse_").append(name).append("(node:CommNode, path:number[]):void {\n"));
	//	res.push_back(string("\tlet sz = node.getSize();\n"));
	//	res.push_back(string("\tfor(let i=0; i<sz; i++) {\n"));
	//	unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());
	//	if(bcls->isLeaf())
	//	{
	//		//res.push_back(string("\t\tlet a = new ").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript)).append("();\n"));
	//		res.push_back(string("\t\tlet a = new CommMsgParser(node.findSubnode([i]).leaf).").append(bcls->fillParseIntrI(fld->getZeroSubType())).append("();\n"));
	//		res.push_back(string("\t\tif(a)\n"));
	//		res.push_back(string("\t\t\tthis.").append(name).append(".add(a);\n"));
	//	}
	//	else
	//	{
	//		res.push_back(string("\t\tlet a = new ").append(fld->getFieldType()->getZeroSubType()->getFieldTypeStr(typeCppToJavaScript)).append("();\n"));
	//		res.push_back(string("\t\ta.parseNode(node.findSubnode([i]));\n"));
	//		res.push_back(string("\t\tthis.").append(name).append(".add(a);\n"));
	//	}
	//	res.push_back(string("\t}\n"));
	//	res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
	//	res.push_back(string("case ").append(to_string(fld->getLeafNodeIndex())).append(":\n"));
	//	res.push_back(string("\tthis._parse_").append(name).append("(leafnode,path);\n"));
	//	res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		//res.push_back(string("\tthis[propname] = this.").append(name).append(".parse(parser);\n"));
		//res.push_back(string("\tself._parseMapSet(parser);\n"));
			res.push_back(string("\tsz = parser.parseUINT32();\n"));
			res.push_back(string("\tfor(let i=0;i<sz;i++){\n"));
			res.push_back(string("\t\tlet itemOrdinal = parser.parseUINT32();\n"));
			res.push_back(string("\t\tlet docsz = parser.parseUINT32(),setordinals= new Set<number>();\n"));
			res.push_back(string("\t\tfor(let j=0;j<docsz;j++){\n"));
			res.push_back(string("\t\t\tsetordinals.add(parser.parseUINT32());\n"));
			res.push_back(string("\t\t}\n"));
			res.push_back(string("\t\tthis.").append(name).append(".set(itemOrdinal,setordinals);\n"));
			res.push_back(string("\t}\n"));


		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string("this._parseMapSet(parser);\n"));
		res.push_back(string("sz = parser.parseUINT32();\n"));
		res.push_back(string("for(let i=0;i<sz;i++){\n"));
		res.push_back(string("\tlet itemOrdinal = parser.parseUINT32();\n"));
		res.push_back(string("\tlet docsz = parser.parseUINT32(),setordinals= new Set<number>();\n"));
		res.push_back(string("\tfor(let j=0;j<docsz;j++){\n"));
		res.push_back(string("\t\tsetordinals.add(parser.parseUINT32());\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\tthis.").append(name).append(".set(itemOrdinal,setordinals);\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual bool isLeaf(void) 
	{
		return false;
	}
	//virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	//{
	//	vector<string> res;
	//	res.push_back(string("case \"").append(name).append("\":\n"));

	//	//res.push_back(string("\tthis[propname] = parser.").append(fillParseIntrI(fieldType)).append("();\n"));

	//	res.push_back(string("\t{\n"));
	//	res.push_back(string("\t\tlet sz = parser.parseUINT32();\n"));
	//	res.push_back(string("\t\tfor(let i=0; i<sz; i++) {\n"));

	//	unique_ptr<CGJSbasecls> bcls = CGJSbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());

	//	res.push_back(string("\t\t\t").append(name).append(".add(this[propname]").append(bcls->fillParseIntrE(fld->getZeroSubType(), "[i]")).append(");\n"));
	//	//res.push_back(string("\t\t\tif(a) this[propname].add(a);\n"));


	//	res.push_back(string("\t\t}\n"));
	//	res.push_back(string("\t}\n"));

	//	res.push_back(string("\tbreak;\n"));
	//	return res;
	//}

};

struct CGJStringUINT32UINT32Vectorcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->getUpdateHandler().size())
			res.push_back(string("private _").append(fld->getName()).append(":Array<UserEssentials>= new Array<UserEssentials>();\n"));
		else
			res.push_back(string(fld->getName()).append(":Array<UserEssentials>= new Array<UserEssentials>();\n"));
		return res;
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():Array<UserEssentials>{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:Array<UserEssentials>){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tsz = parser.parseUINT32();\n"));
		res.push_back(string("\tfor(let i=0;i<sz;i++){\n"));
		res.push_back(string("\t\tlet ue = new UserEssentials();\n"));
		res.push_back(string("\t\tue.parse(parser);\n"));
		res.push_back(string("\t\tthis[propname].push(ue);\n"));
		res.push_back(string("\t}\n"));


		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string("this._parseMapSet(parser);\n"));
		res.push_back(string("sz = parser.parseUINT32();\n"));
		res.push_back(string("for(let i=0;i<sz;i++){\n"));
		res.push_back(string("\tlet ue = new UserEssentials();\n"));
		res.push_back(string("\tue.parse(parser);\n"));
		res.push_back(string("\tthis.otherUsers.push(ue);\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual bool isLeaf(void) 
	{
		return false;
	}
};

struct CGJStringUINT32PairVectorcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->getUpdateHandler().size())
			res.push_back(string("private _").append(fld->getName()).append(":Map<string,number>= new Map<string,number>();\n"));
		else
		res.push_back(string(fld->getName()).append(":Map<string,number>= new Map<string,number>();\n"));
		return res;
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():Map<string,number>{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:Map<string,number>){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tsz = parser.parseUINT32();\n"));
		res.push_back(string("\tfor(let i=0;i<sz;i++){\n"));
		res.push_back(string("\t\tthis[propname].set(parser.parseString(),parser.parseUINT32());\n"));
		res.push_back(string("\t}\n"));


		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string("this._parseMapSet(parser);\n"));
		res.push_back(string("sz = parser.parseUINT32();\n"));
		res.push_back(string("for(let i=0;i<sz;i++){\n"));
		res.push_back(string("\tthis.").append(name).append(".set(parser.parseString(),parser.parseUINT32());\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual bool isLeaf(void) 
	{
		return false;
	}
};

struct CGJStringPairVectorcls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if(fld->getUpdateHandler().size())
			res.push_back(string("private _").append(fld->getName()).append(":Map<string,string>= new Map<string,string>();\n"));
		else
			res.push_back(string(fld->getName()).append(":Map<string,string>= new Map<string,string>();\n"));
		return res;
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():Map<string,string>{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:Map<string,string>){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tsz = parser.parseUINT32();\n"));
		res.push_back(string("\tfor(let i=0;i<sz;i++){\n"));
		res.push_back(string("\t\tthis[propname].set(parser.parseString(),parser.parseString());\n"));
		res.push_back(string("\t}\n"));


		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string("this._parseMapSet(parser);\n"));
		res.push_back(string("sz = parser.parseUINT32();\n"));
		res.push_back(string("for(let i=0;i<sz;i++){\n"));
		res.push_back(string("\tthis.").append(name).append(".set(parser.parseString(),parser.parseString());\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual bool isLeaf(void) 
	{
		return false;
	}
};


struct CGJUINT32Paircls :public CGJSbasecls
{
	virtual vector<string> fillFields(const CGfield* fld) override
	{
		vector<string> res;
		if (fld->getUpdateHandler().size())
			res.push_back(string("private _").append(fld->getName()).append(":Map<number,number>= new Map<number,number>();\n"));
		else
			res.push_back(string(fld->getName()).append(":Map<number,number>= new Map<number,number>();\n"));
		return res;
	}
	virtual vector<string> fillGetterSetter(const CGfield* fld)
	{
		vector<string> res;
		res.push_back(string("get ").append(fld->getName()).append("():Map<number,number>{\n"));
		res.push_back(string("\treturn this._").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n\n"));
		res.push_back(string("set ").append(fld->getName()).append("(v:Map<number,number>){\n"));
		res.push_back(string("\tif(!this._init)\n"));
		res.push_back(string("\t\tthis._dirtyprops.add(\"").append(fld->getName()).append("\");\n"));
		res.push_back(string("\tthis._").append(fld->getName()).append(" = v;\n"));
		res.push_back(string("}\n\n"));
		return res;
	}

	virtual vector<string> fillSubscrParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillSubscr(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("case \"").append(name).append("\":\n"));
		res.push_back(string("\tthis[propname].set(parser.parseUINT32(),parser.parseUINT32());\n"));
		//res.push_back(string("\t}\n"));


		res.push_back(string("\tbreak;\n"));
		return res;
	}
	virtual vector<string> fillParseF(const CGfield* fld, CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;
		//res.push_back(string("this._parseMapSet(parser);\n"));
		res.push_back(string("\tthis.").append(name).append(".set(parser.parseUINT32(),parser.parseUINT32());\n"));
		return res;
	}
	virtual bool isLeaf(void)
	{
		return false;
	}
};


#endif	// CGJvector_h_included
