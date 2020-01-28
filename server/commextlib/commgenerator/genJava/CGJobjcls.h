// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJobjcls_h_included
#define CGJobjcls_h_included

//******************************************************************************
// Objects classes for text generators
//******************************************************************************
struct CGJobjcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		if(fieldType->isContainer())// && (fieldType->getIndexAmount() == 1))
		{
			res.push_back("{\n");
			res.push_back("\tint counter = commMsg.parseUINT32()\n");
			res.push_back(string("\t").append(name).append(" = new ArrayList<").append(fieldType->getIndexedType(typeCppToJava)).append(">(counter);\n"));
			res.push_back("\tfor(int i=0; i<counter; i++){\n");
			unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getIndexedType(typeCppToJava));
			res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), name+"[i]", mainObj);
			for(auto elem : res1)
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
		if(fieldType->isContainer())// && (fieldType->getIndexAmount() == 1))
		{
			res.push_back("{\n");
			res.push_back(string("\tcommMsg.composeUINT32(").append(name).append(".size()\n"));
			res.push_back("\tfor(int i=0; i<counter; i++){\n");
			unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getIndexedType(typeCppToJava));
			res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), name+"[i]", mainObj);
			for(auto elem : res1)
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
		//string res = "commonType = CommonType.Model";
		//res += getModelType(fld->getFieldType(), "");
		//return res;
		return "";
	}
};

#endif // CGJobjcls_h_included