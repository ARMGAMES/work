// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGarraycls_h_included
#define CGGarraycls_h_included

//******************************************************************************
// Array class for text generators
//******************************************************************************
struct CGGarraycls :public CGGbasecls
{
	virtual vector<string> fillCopyConctructorBody(const CGfield* fld)  override 
	{
		vector<string> res, res1;
		res.push_back(string("for(size_t i=0; i<").append(to_string(fld->getFieldType()->getArraySize())).append("; ++i)\n"));
		res.push_back(string("{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillAssignment(fld, fld->getName()+"[i]", string("other.")+fld->getName()+"[i]");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillMoveConctructorBody(const CGfield* fld)  override 
	{
		vector<string> res, res1;
		res.push_back(string("for(size_t i=0; i<").append(to_string(fld->getFieldType()->getArraySize())).append("; ++i)\n"));
		res.push_back(string("{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillMoveAssignment(fld, fld->getName()+"[i]", string("other.")+fld->getName()+"[i]");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillAssignment(const CGfield* fld, const string& name1, const string& name2)  override 
	{
		vector<string> res, res1;
		res.push_back(string("for(size_t i=0; i<").append(to_string(fld->getFieldType()->getArraySize())).append("; ++i)\n"));
		res.push_back(string("{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillAssignment(fld, fld->getName()+"[i]", string("other.")+fld->getName()+"[i]");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillMoveAssignment(const CGfield* fld, const string& name1, const string& name2)  override 
	{
		vector<string> res, res1;
		res.push_back(string("for(size_t i=0; i<").append(to_string(fld->getFieldType()->getArraySize())).append("; ++i)\n"));
		res.push_back(string("{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillMoveAssignment(fld, fld->getName()+"[i]", string("other.")+fld->getName()+"[i]");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillGetters(const CGfield* fld)  override 
	{
		vector<string> res;
		res.push_back(string("const ").append(fld->getFieldType()->getFullTypeString(true)).append("* ").append(fld->getGetter())
			.append("(void) const {return ").append(fld->getName()).append(";}\n"));
		return res;
	}
	virtual vector<string> fillSetters(const CGfield* fld)  override 
	{
		vector<string> res, res1;
		res.push_back(string("void ").append(fld->getSetter()).append("(const ").append(fld->getFieldType()->getFullTypeString(true)).append("* ")
			.append(fld->getName()).append("_data) \n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getFieldType()->getArraySize())).append("; ++i)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillAssignment(fld, fld->getName()+"[i]", fld->getName()+"_data"+"[i]");
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillMoveSetters(const CGfield* fld)  override 
	{
		vector<string> res, res1;
		res.push_back(string("void ").append(fld->getMoveSetter()).append("(").append(fld->getFieldType()->getFullTypeString(true)).append("* ")
			.append(fld->getName()).append("_data) \n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getFieldType()->getArraySize())).append("; ++i)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldType()->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillMoveAssignment(fld, fld->getName()+"[i]", fld->getName()+"_data"+"[i]");
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
	{
		vector<string> res, res1;
		res.push_back(string("for(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n"));
		res.push_back(string("{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), name+"[i]", channel, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}

		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
	{
		vector<string> res, res1;
		res.push_back(string("for(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n"));
		res.push_back(string("{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), name+"[i]", channel, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}

		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> getFieldComparator(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2,  const string& name3, bool negat, CGmain* mainObj)  override 
	{
		vector<string> res, res1;
		string index = "i"+to_string(counter);
		res.push_back(string("for(size_t ").append(index).append(" = 0; ").append(index).append(" < ").append(to_string(fieldType->getArraySize())).append("; ++").append(index).append(")\n"));
		res.push_back(string("{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->getFieldComparator(counter+1, fld, fieldType->getZeroSubType(), name1+"["+index+"]", name2+"["+index+"]", name3, negat, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> getFieldComparatorLog(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj)  override 
	{
		vector<string> res, res1;
		string index = "i"+to_string(counter);
		res.push_back(string("for(size_t ").append(index).append(" = 0; ").append(index).append(" < ").append(to_string(fieldType->getArraySize())).append("; ++").append(index).append(")\n"));
		res.push_back(string("{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->getFieldComparatorLog(counter+1, fld, fieldType->getZeroSubType(), name1+"["+index+"]", name2+"["+index+"]", mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillFieldPublisher(const CGfield* fld, CGfieldtype* fieldType, CGmain* mainObj, const vector<string>& path, const string& lnidx, int counter) override
	{
		vector<string> res, res1;

		res.push_back(string("for(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillPublisher(fld, fieldType->getZeroSubType(), mainObj, path, "i", fld->getName()+"[i]", counter);
		res.insert(res.end(), res1.begin(), res1.end());
		return res;
	}
	virtual bool isLeaf(void) {return false;}
};

#endif	// CGGarraycls_h_included
