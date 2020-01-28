// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGJbasecls_h_included
#define CGJbasecls_h_included

//******************************************************************************
// Base class for text generators
//******************************************************************************
struct CGJbasecls
{
	virtual vector<string> fillGetters(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res;
		string str = fld->getName();
		*str.begin() = toupper(*str.begin());

		auto refCls = fld->getRefClass().first;
		if (!refCls.empty() && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "Set"))
		{
			string str1;
			str1 = CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj);
			str1.replace(0, 3, "List");
			res.push_back(string("public ").append(str1).append(" get").append(str).append("() {\n"));
		}
		else
		{
			res.push_back(string("public ").append(getJavaTypeString(fld, fld->getFieldType(), mainObj)).append(" get").append(str).append("() {\n"));
		}

		res.push_back(string("\treturn ").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillSetters(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res;
		string str = fld->getName();
		*str.begin() = toupper(*str.begin());

		auto refCls = fld->getRefClass().first;
		if (refCls.empty())
		{
			res.push_back(string("public void set").append(str).append("(").append(getJavaTypeString(fld, fld->getFieldType(), mainObj)).append(" ").append(fld->getName()).append(") {\n"));
		}
		else
		{
			res.push_back(string("public void set").append(str).append("(").append(CGJbasecls::typeCppToJava(refCls)).append(" ").append(fld->getName()).append(") {\n"));
		}
		res.push_back(string("\tthis.").append(fld->getName()).append(" = ").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n"));
		return res;
	}



	virtual vector<string> fillQueryGetters(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res;
		string str = fld->getName();
		*str.begin() = toupper(*str.begin());

		string str1, s;
		str1 = CGJbasecls::simpleToObj(CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj, false));
		if ((fld->getFieldType()->getIndexedType().length()) && CGJbasecls::isMultFilter(fld->getZeroSubType()->getFieldTypeStr()) && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "List"))
		{
			str.replace(0, 4, "Set");
		}
		//auto refCls = fld->getRefClass().first;
		//if (!refCls.empty() && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "Set"))
		//{
		//	str1.replace(0, 3, "List");
		//}

		s = "public ";

		if (fld->getJavaSortCode() >= 0)
		{
			s.append("SortableQueryParameter<");
		}
		else
		{
			s.append("QueryParameter<");
		}
		if (!fld->isContainer() && !fld->isArray())
			s.append("Set<");
		//s.append(str1).append("> ").append(fld->getJavaName().empty() ? fld->getName() : fld->getJavaName()).append(";\n");
		s.append(str1);
		if (!fld->isContainer() && !fld->isArray())
			s.append(">");
		s.append("> get").append(str).append("() {\n");
		res.push_back(s);


		//res.push_back(string("").append(str1).append("> get").append(str).append("() {\n"));

		res.push_back(string("\treturn ").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillQuerySetters(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res;
		string str = fld->getName();
		*str.begin() = toupper(*str.begin());

		string str1, s;
		str1 = CGJbasecls::simpleToObj(CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj, false));
		if ((fld->getFieldType()->getIndexedType().length()) && CGJbasecls::isMultFilter(fld->getZeroSubType()->getFieldTypeStr()) && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "List"))
		{
			str.replace(0, 4, "Set");
		}
		//auto refCls = fld->getRefClass().first;
		//if (!refCls.empty() && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "Set"))
		//{
		//	str1.replace(0, 3, "List");
		//}

		s.append("public void set").append(str).append("(");
		if (fld->getJavaSortCode() >= 0)
		{
			s.append("SortableQueryParameter<");
		}
		else
		{
			s.append("QueryParameter<");
		}
		if (!fld->isContainer() && !fld->isArray())
			s.append("Set<");
		s.append(str1);
		if (!fld->isContainer() && !fld->isArray())
			s.append(">");
		s.append("> ").append(fld->getName()).append(") {\n");
		res.push_back(s);

		res.push_back(string("\tthis.").append(fld->getName()).append(" = ").append(fld->getName()).append(";\n"));
		res.push_back(string("}\n"));
		return res;
	}
	//static string fillFieldType(CGfieldtype* fieldType);
	static string typeCppToJava(const string& str);
	static string simpleToObj(const string& str);
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj) abstract;
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj) abstract;

	virtual string getCommonType(const CGfield* fld, int num = 0) abstract;

	static string typeCommonJava(const string& str);

	static unique_ptr<CGJbasecls> getBaseCls(const string& str);

	static bool isMultFilter(const string& str);


	static string getJavaTypeString(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, bool useRefClass = true, const string& arraysize = "")
	{
		string s, refCls, ftype;
		if(useRefClass)
			refCls = fld->getRefClass().first;

		ftype = fieldType->getFieldTypeStr();
		if (!fieldType->getJavaType().empty())
		{
			ftype = fieldType->getJavaType();
		}
		if (ftype == "CommExtStringPair")
		{
			s = "Pair<String, String>";
		}
		else if (ftype == "CommExtStringUINT32Pair")
		{
			s = "Pair<String, Integer>";
		}
		else if (ftype == "CommExtStringUINT32UINT32")
		{
			s = "Pair<Integer, Integer>";
		}
		else if (fieldType->getSubTypes().size())
		{
			s.append(typeCppToJava(ftype)).append("<").append(simpleToObj(getJavaTypeString(fld, fieldType->getSubTypes()[0].get(), mainObj, useRefClass)));
			for (auto i = 1; i < fieldType->getSubTypes().size(); ++i)
			{
				s.append(", ").append(simpleToObj(getJavaTypeString(fld, fieldType->getSubTypes()[1].get(), mainObj, useRefClass)));
			}
			s.append(">");
		}
		else if (fieldType->getIndexedType().length())
		{
			if (!refCls.empty())
			{
				auto cls = (CGstruct*)mainObj->findObject(objTypes::gTypeStruct, refCls);
				if (cls)
				{
					s.append(typeCppToJava(fieldType->getBaseContainerType())).append("<").append(cls->getJavaName().empty() ? cls->getName() : cls->getJavaName()).append(">");
				}
				else
				{
					s.append(typeCppToJava(fieldType->getBaseContainerType())).append("<").append(typeCppToJava(refCls)).append(">");
				}
			}
			else
			{
				s.append(typeCppToJava(fieldType->getBaseContainerType())).append("<").append(simpleToObj(getJavaTypeString(fld, fieldType->getZeroSubType(), mainObj, useRefClass))).append(">");
			}
		}
		else
		{
			if (!refCls.empty())
			{
				auto cls = (CGstruct*)mainObj->findObject(objTypes::gTypeStruct, refCls);
				if (cls)
				{
					s.append(cls->getJavaName().empty() ? cls->getName() : cls->getJavaName());
				}
				else
				{
					s.append(typeCppToJava(refCls));
				}
			}
			else
			{
				auto cls = (CGstruct*)mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr());
				if (cls)
				{
					s.append(cls->getJavaName().empty() ? cls->getName() : cls->getJavaName());
				}
				else
				{
					if(fld->getJavaTypeAlias().empty())
					{
						s.append(typeCppToJava(ftype));
					}
					else
					{
						s.append(fld->getJavaTypeAlias());
					}
					if (fld->isArray()) 
					{
						s.append("[");
						if (!arraysize.empty()) s.append(arraysize);
						s.append("]");
					}
				}
			}
		}

		return s;
	}


};

#endif	// CGJbasecls_h_included
