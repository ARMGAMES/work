// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGpointercls_h_included
#define CGGpointercls_h_included

//******************************************************************************
// Pointer classes for text generators
//******************************************************************************
struct CGGpointercls : public CGGbasecls
{
	virtual vector<string> fillDestructor(const CGfield* fld) override
	{
		vector<string> res;
		res.push_back(string("if(").append(fld->getName()).append(" != nullptr) delete ").append(fld->getName()).append(";\n"));
		return res;
	}
	virtual string fillDefaultConctructorHeader(const CGfield* fld) override
	{
		return string(fld->getName()).append("(nullptr)\n");
	}
	virtual string fillMoveConctructorHeader(const CGfield* fld) override
	{
		return string(fld->getName()).append("(other.").append(fld->getName()).append(")\n");
	}
	virtual vector<string> fillMoveConctructorBody(const CGfield* fld) override
	{
		vector<string> res;
		res.push_back(string("other.").append(fld->getName()).append(" = nullptr;\n"));
		return res;
	}
	virtual vector<string> fillMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append("; ").append(name2).append(" = nullptr;\n"));
		return res;
	}
	virtual vector<string>  fillGetters(const CGfield* fld) override
	{
		vector<string> res;
		res.push_back(string("const ").append(fld->getFieldType()->getFullTypeString(true)).append(" ").append(fld->getGetter())
					.append("(void) const {return ").append(fld->getName()).append(";}\n"));
		return res;
	}
	virtual vector<string>  fillSetters(const CGfield* fld) override
	{
		vector<string> res;
		res.push_back(string("void ").append(fld->getSetter()).append("(const ").append(fld->getFieldType()->getFullTypeString(true)).append(" ")
					.append(fld->getName()).append("_data)\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tif(").append(fld->getName()).append(" != nullptr) delete ").append(fld->getName()).append(";\n"));
		res.push_back(string("\t").append(fld->getName()).append(" = ").append(fld->getName()).append("_data;\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain*	mainObj) override
	{
		vector<string> res, res1; 
		res.push_back(string("if(").append(name).append(" != nullptr) delete ").append(name).append(";\n"));
		res.push_back(string(name).append(" = new ").append(fld->getFieldType()->getFullTypeString()).append("();\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillParse(fld, fieldType->getSubTypes()[0].get(), string("(*")+name+")", channel, mainObj);
		for(auto elem : res1)
		{
			res.push_back(move(elem));
		}
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
	{
		vector<string> res, res1; 
		res.push_back(string("if(!").append(name).append(") ").append("throw PError(\"Compose null-pointer value in class '").append(fld->getClassName()).append("' field '").append(fld->getName()).append("'\");\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillCompose(fld, fieldType->getSubTypes()[0].get(), string("(*")+name+")", channel, mainObj);
		for(auto elem : res1)
		{
			res.push_back(move(elem));
		}
		return res;
	}

	virtual vector<string> getFieldComparator(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res, res1;
		res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tif(").append(negat?"!":"").append(name1).append(" || !").append(name2).append(")").append(name3).append(";\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->getFieldComparator(counter, fld, fieldType->getSubTypes()[0].get(), string("(*")+name1+")", string("(*(")+name2+"))", name3, negat, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> getFieldComparatorLog(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res, res1;
		res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tif(!").append(name1).append(" || !").append(name2).append(")\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fieldType->getClass()->getName())
			.append("' type '").append(fieldType->getFullTypeString(true)).append("' are NOT EQUAL\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->getFieldComparatorLog(counter, fld, fieldType->getSubTypes()[0].get(), string("(*")+name1+")", string("(*(")+name2+"))", mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		return res;
	}
};

struct CGGsmartptrcls : public CGGbasecls
{
	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain*	mainObj) override
	{
		if(!fieldType->getSubTypes().size())
		{
			string err;
			err.append("No subtype in 'unique_ptr' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw PError(err.c_str());
		}
		vector<string> res, res1; 
		res.push_back(string("if(!").append(name).append(".get()) ").append(name).append(".reset(new").append(fld->getFieldType()->getFullTypeString()).append("());\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillParse(fld, fieldType->getSubTypes()[0].get(), string("(*(")+name+".get()))", channel, mainObj);
		for(auto elem : res1)
		{
			res.push_back(move(elem));
		}
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
	{
		if(!fieldType->getSubTypes().size())
		{
			string err;
			err.append("No subtype in 'unique_ptr' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1; 
		res.push_back(string("if(!").append(name).append(".get()) ").append("throw PError(\"Compose null-pointer value in class '").append(fld->getClassName()).append("' field '").append(fld->getName()).append("'\");\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->fillCompose(fld, fieldType->getSubTypes()[0].get(), string("(*(")+name+".get()))", channel, mainObj);
		for(auto elem : res1)
		{
			res.push_back(move(elem));
		}
		return res;
	}

	virtual vector<string> getFieldComparator(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2,  const string& name3, bool negat, CGmain* mainObj) override
	{
		if(!fieldType->getSubTypes().size())
		{
			string err;
			err.append("No subtype in 'unique_ptr' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		res.push_back(string("if(").append(name1).append(".get() != ").append(name2).append(".get())\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tif(").append(negat?"!":"").append(name1).append(".get() || !").append(name2).append(".get())").append(name3).append(";\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->getFieldComparator(counter, fld, fieldType->getSubTypes()[0].get(), string("(*(")+name1+".get()))", string("(*(")+name2+".get()))", name3, negat, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> getFieldComparatorLog(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		if(!fieldType->getSubTypes().size())
		{
			string err;
			err.append("No subtype in 'unique_ptr' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		res.push_back(string("if(").append(name1).append(".get() != ").append(name2).append(".get())\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tif(!").append(name1).append(".get() || !other.").append(name2).append(".get())\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fieldType->getClass()->getName())
			.append("' type '").append(fieldType->getFullTypeString(true)).append("' are NOT EQUAL\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		res1 = bcls->getFieldComparatorLog(counter, fld, fieldType->getSubTypes()[0].get(), string("(*(")+name1+".get()))", string("(*(")+name2+".get()))", mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		return res;
	}
};

#endif	// CGGpointercls_h_included
