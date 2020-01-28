// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJpointercls_h_included
#define CGJpointercls_h_included

//******************************************************************************
// Pointer classes for text generators
//******************************************************************************
struct CGJpointercls :public CGJbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillParse(fld, fieldType->getSubTypes()[0].get(), name, mainObj);
		for(auto elem : res1)
		{
			res.push_back(move(elem));
		}
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		unique_ptr<CGJbasecls> bcls = CGJbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillCompose(fld, fieldType->getSubTypes()[0].get(), string("(*")+name+")", mainObj);
		for(auto elem : res1)
		{
			res.push_back(move(elem));
		}
		return res;
	}
};


#endif	// CGJpointercls_h_included
