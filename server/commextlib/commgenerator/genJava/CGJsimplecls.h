// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJsimplecls_h_included
#define CGJsimplecls_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGJpyrUINT8cls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseBYTE();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeBYTE").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType"+(num?to_string(num):"")+" = CommonType.BYTE";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.BYTE" + suffix;
		return res;
	}
};

struct CGJpyrUINT16cls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseUINT16();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeUINT16").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.UINT16";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.UINT16" + suffix;
		return res;
	}
};

struct CGJpyrUINT32cls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseUINT32();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeUINT32").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		auto refCls = fld->getRefClass().first;
		if (refCls.empty())
		{
			return ", commonType" + (num ? to_string(num) : "") + " = CommonType.UINT32";
		}
		else
		{
			string res = ", ref = true";
			//string res = "commonType = CommonType.Model";
			//res += getModelType(fld->getFieldType(), "");
			return res;
		}
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.UINT32" + suffix;
		return res;
	}
};

struct CGJpyrUINT64cls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseUINT64();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeUINT64").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.UINT64";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.UINT64" + suffix;
		return res;
	}
};

struct CGJpyrBOOLcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseBOOL();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeBOOL").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.BOOL";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.BOOL" + suffix;
		return res;
	}
};

struct CGJpyrStringcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseString();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeString").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		auto refCls = fld->getRefClass().first;
		if (refCls.empty())
		{
			return ", commonType" + (num ? to_string(num) : "") + " = CommonType.String";
		}
		else
		{
			string res = ", ref = true";
			//string res = "commonType = CommonType.Model";
			//res += getModelType(fld->getFieldType(), "");
			return res;
		}
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		auto refCls = fldtype->getField()->getRefClass().first;
		if (refCls.empty())
		{
			string res = ", modelType = ModelType.String" + suffix;
			return res;
		}
		else
		{
			string res = ", modelType = ModelType.";

			if (refCls.compare(0, 2, "CE"))
				res += refCls;
			else
				res += refCls.substr(2);
			return res;
		}
	}
};

struct CGJpyrCommMsgBodycls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string(name).append(" = commMsgPar.parseMsgBody();\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg.composeMsgBody").append("(").append(name).append(");\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.VarBlock";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.VarBlock" + suffix;
		return res;
	}
};

struct CGJpyrPBlockcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back("{\n");
		res.push_back("\tPIntRef sz = new PIntRef();\n");
		res.push_back("\tint p  = parser._parseVarBlock(sz);\n");
		res.push_back(string("\t").append(name).append(" = new byte[sz.getInt()];\n"));
		res.push_back(string("\tPBlock.memcpy(").append(name).append(", 0, parser._ptr(), p, sz.getInt());\n"));
		res.push_back("}\n");
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res;
		res.push_back(string("commMsg._composeVarBlock(").append("(").append(name).append(", 0, ").append(name).append(".length);\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.VarBlock";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.VarBlock" + suffix;
		return res;
	}
};

struct CGJpyrCommUtcTimecls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		if (fieldType->isContainer())// && (fieldType->getIndexAmount() == 1))
		{
			res.push_back("{\n");
			res.push_back("\tint counter = commMsg.parseUINT32()\n");
			res.push_back(string("\t").append(name).append(" = new ArrayList<").append(fieldType->getIndexedType(typeCppToJava)).append(">(counter);\n"));
			res.push_back("\tfor(int i=0; i<counter; i++){\n");
			unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getIndexedType(typeCppToJava));
			res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), name + "[i]", mainObj);
			for (auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			res.push_back("\t}\n");
			res.push_back("}\n");
		}
		else
			res.push_back(string(name).append(".parse(commMsgPar);\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		if (fieldType->isContainer())// && (fieldType->getIndexAmount() == 1))
		{
			res.push_back("{\n");
			res.push_back(string("\tcommMsg.composeUINT32(").append(name).append(".size()\n"));
			res.push_back("\tfor(int i=0; i<counter; i++){\n");
			unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getIndexedType(typeCppToJava));
			res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), name + "[i]", mainObj);
			for (auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			res.push_back("\t}\n");
			res.push_back("}\n");
		}
		else
			res.push_back(string(name).append(".compose(commMsg);\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.UtcTime";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.UtcTime" + suffix;
		return res;
	}
};

struct CGJpyrCommUtcDatecls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		if (fieldType->isContainer())// && (fieldType->getIndexAmount() == 1))
		{
			res.push_back("{\n");
			res.push_back("\tint counter = commMsg.parseUINT32()\n");
			res.push_back(string("\t").append(name).append(" = new ArrayList<").append(fieldType->getIndexedType(typeCppToJava)).append(">(counter);\n"));
			res.push_back("\tfor(int i=0; i<counter; i++){\n");
			unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getIndexedType(typeCppToJava));
			res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), name + "[i]", mainObj);
			for (auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			res.push_back("\t}\n");
			res.push_back("}\n");
		}
		else
			res.push_back(string(name).append(".parse(commMsgPar);\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		if (fieldType->isContainer())// && (fieldType->getIndexAmount() == 1))
		{
			res.push_back("{\n");
			res.push_back(string("\tcommMsg.composeUINT32(").append(name).append(".size()\n"));
			res.push_back("\tfor(int i=0; i<counter; i++){\n");
			unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getIndexedType(typeCppToJava));
			res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), name + "[i]", mainObj);
			for (auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			res.push_back("\t}\n");
			res.push_back("}\n");
		}
		else
			res.push_back(string(name).append(".compose(commMsg);\n"));
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		return ", commonType" + (num ? to_string(num) : "") + " = CommonType.UtcDate";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res = ", modelType = ModelType.UtcDate" + suffix;
		return res;
	}
};


#endif	// CGJsimplecls_h_included
