// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJcontcls_h_included
#define CGJcontcls_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGJvectorcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		string res;
		unique_ptr<CGJbasecls> bcls;

		bcls = CGJbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());

		//res.append("commonType = CommonType.Model").append(bcls->getModelType(fld->getZeroSubType(), fld->getBaseContainerType(CGJbasecls::typeCppToJava)));
		res.append(bcls->getCommonType(fld, 0));

		return res;
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res;



		return res;
	}
};

struct CGJsetcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		string res;
		unique_ptr<CGJbasecls> bcls;
		bcls = CGJbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());
		//res.append("commonType = CommonType.Model").append(bcls->getModelType(fld->getZeroSubType(), fld->getBaseContainerType(CGJbasecls::typeCppToJava)));
		res.append(bcls->getCommonType(fld, 0));

		return res;
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res;



		return res;
	}
};


struct CGJmapcls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		string res1, res2;
		unique_ptr<CGJbasecls> bcls1 = getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		unique_ptr<CGJbasecls> bcls2 = getBaseCls(fld->getFieldType()->getSubTypes()[1]->getFieldTypeStr());
		//res.append("commonType = CommonType.Model").append(bcls->getModelType(fld->getZeroSubType(), fld->getBaseContainerType(CGJbasecls::typeCppToJava)));
		res1.append(bcls1->getCommonType(fld, 0));
		res2.append(bcls1->getCommonType(fld, 2));

		return res1+res2;
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res;



		return res;
	}
};

struct CGJpyrStringPaircls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		string res1, res2;
		//unique_ptr<CGJbasecls> bcls1 = getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		//unique_ptr<CGJbasecls> bcls2 = getBaseCls(fld->getFieldType()->getSubTypes()[1]->getFieldTypeStr());
		//res.append("commonType = CommonType.Model").append(bcls->getModelType(fld->getZeroSubType(), fld->getBaseContainerType(CGJbasecls::typeCppToJava)));
		//res1.append(bcls1->getCommonType(fld));
		//res2.append(bcls1->getCommonType(fld, "value"));

		return ", commonType = CommonType.String, commonType2 = CommonType.String";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res;



		return res;
	}
};

struct CGJpyrUINT32Paircls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		string res1, res2;
		//unique_ptr<CGJbasecls> bcls1 = getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		//unique_ptr<CGJbasecls> bcls2 = getBaseCls(fld->getFieldType()->getSubTypes()[1]->getFieldTypeStr());
		//res.append("commonType = CommonType.Model").append(bcls->getModelType(fld->getZeroSubType(), fld->getBaseContainerType(CGJbasecls::typeCppToJava)));
		//res1.append(bcls1->getCommonType(fld));
		//res2.append(bcls1->getCommonType(fld, "value"));

		return ", commonType = CommonType.UINT32, commonType2 = CommonType.UINT32";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res;



		return res;
	}
};

struct CGJpyrStringUint32Paircls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		return res;
	}
	virtual string getCommonType(const CGfield* fld, int num)
	{
		string res1, res2;
		//unique_ptr<CGJbasecls> bcls1 = getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		//unique_ptr<CGJbasecls> bcls2 = getBaseCls(fld->getFieldType()->getSubTypes()[1]->getFieldTypeStr());
		//res.append("commonType = CommonType.Model").append(bcls->getModelType(fld->getZeroSubType(), fld->getBaseContainerType(CGJbasecls::typeCppToJava)));
		//res1.append(bcls1->getCommonType(fld));
		//res2.append(bcls1->getCommonType(fld, "value"));

		return ", commonType = CommonType.String, commonType2 = CommonType.UINT32";
	}
	virtual string getModelType(const CGfieldtype* fldtype, const string& suffix)
	{
		string res;



		return res;
	}
};


#endif // CGJcontcls_h_included