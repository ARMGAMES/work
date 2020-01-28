// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGsimplecls_h_included
#define CGGsimplecls_h_included

//******************************************************************************
// Simple classes for text generators
//******************************************************************************
struct CGGpyrdgtcls :public CGGbasecls
{
	virtual string fillFieldJson(const CGfieldtype* fieldType, const string& name1, const string& name2, int margin1, int margin2, int counter, bool last) override
	{
		string res, res1, res2;
		for (int i = 0; i < margin1; ++i)
		{
			res1.append("\t");
		}
		for (int i = 0; i < margin2; ++i)
		{
			res2.append("  ");
		}
		res.append(res1).append("output.append(margin_string).append(\"").append(res2);

		if(name1.size())
			res.append("\\\"").append(name1).append("\\\": ");				

		if (fieldType->isArray())
		{
			res.append("[\\n\");\n");
			res.append(res1).append("for(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n");
			res.append(res1).append("{\n");

			res.append(res1).append("\tif((i+1)<").append(to_string(fieldType->getArraySize())).append(") \n");
			res.append(res1).append("\t{\n");
			res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  \").appendInt(").append(name2).append("[i]).append(\",\\n\");\n");
			res.append(res1).append("\t}\n");
			res.append(res1).append("\telse\n");
			res.append(res1).append("\t{\n");
			res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  \").appendInt(").append(name2).append("[i]).append(\"\\n\");\n");
			res.append(res1).append("\t}\n");

			res.append(res1).append("}\n");
			res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("]\")");
		}
		else
		{
			res.append("\").appendInt(").append(name2).append(")");
		}

		if (!last)
		{
			res.append(".append(\",\")");

		}
		res.append(".append(\"\\n\");\n");
		return res;
	}

	virtual string getFieldJson(const CGfield* fld, const CGfieldtype* fieldType, const string& name, int margin, int counter)
	{
		string res, res1;
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}
		if (fieldType->isArray())
		{
			res.append(res1).append("for(size_t i=0; ((i<").append(to_string(fieldType->getArraySize())).append(")&&(i<(elem0->v).size())); ++i)\n");
			res.append(res1).append("{\n");
			res.append(res1).append("\t").append(name).append("[i] = elem0->v[i]->i;\n");
			res.append(res1).append("}\n");
		}
		else
		{
			res.append(res1).append(name).append(" = elem").append(to_string(counter)).append("->i;\n");
		}
		return res;
	}

	vector<string> fillFieldAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append(";\n"));
		return res;
	}
	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append("; ").append(name2).append(" = 0;\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		vector<string> res;
		if(fld->isArray())
		{
			res.push_back(string("memset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
		}
		else if(fld->getDefault().length())
		{
			//std::stoi(fld->getDefault());
			res.push_back(string(name).append(" = ").append(fld->getDefault()).append(";\n"));
		}
		else
		{
			res.push_back(string(name).append(" = 0;\n"));
		}
		return res;
	}
	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("if(").append(name1).append(negat?" != ":" == ").append(name2).append(")").append(name3).append("\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fieldType->getClass()->getName()).append("' are NOT EQUAL\");\n"));
		res.push_back(string("\tresult = false;\n"));
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parse").append(fld->getFullTypeString()).append("(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("parser.parse").append(fieldType->getFullTypeString()).append("(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.compose").append(fld->getFullTypeString()).append("(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.compose").append(fieldType->getFullTypeString()).append("(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillSelConvertCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		if(fld->getSelConvertFuncName().length() && (!fld->isArray()))
		{
			vector<string> res;
			res.push_back(string("{\n"));
			res.push_back(string("\t").append(fieldType->getFullTypeString()).append(" _temp_").append(fld->getName()).append(";\n"));
			res.push_back(string("\tbool _temp_bool_").append(fld->getName()).append(" = ").append(fld->getSelConvertFuncName()).append("(").append(fld->getName()).append(", _temp_").append(fld->getName()).append(", this, \"").append(fld->getClassName()).append("\", \"").append(fld->getName()).append("\");\n"));
			res.push_back(string("\tif(_temp_bool_").append(fld->getName()).append(")\n"));
			res.push_back(string("\t\t").append("data.compose").append(fieldType->getFullTypeString()).append("(_temp_").append(fld->getName()).append(");\n"));
			res.push_back(string("\telse\n"));
			res.push_back(string("\t\t").append("data.compose").append(fieldType->getFullTypeString()).append("(").append(fld->getName()).append(");\n"));
			res.push_back(string("}\n"));
			return res;
		}
		else
		{
			bool vecSmartCorr;
			return fillCompose(fld, fieldType, name, "default", mainObj, fld->isArray(), vecSmartCorr);
		}
	}


	vector<string> fillFieldPublisher(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter) override 
	{
		return vector<string>();
	}
	vector<string> fillFieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		vector<string> res, res1;
		if(fieldType->isArray())
		{
			res.push_back(string("for(size_t i=0; i<").append(fieldType->getArraySizeString()).append("; ++i)\n"));
			res.push_back(string("{\n"));
			//res.push_back(string("\tif(").append(name1).append("[i").append(to_string(counter)).append("] != ").append(name2).append("[i").append(to_string(counter)).append("])\n"));
			res1 =  fillFieldComparator(0, fld, fieldType, name1+"[i]", name2+"[i]", "", true, mainObj);
			for(auto elem : res1)
			{
				res.push_back(string("").append(move(elem)));
			}

			res.push_back(string("\t{\n"));
			res.push_back(string("\t\t").append(name1).append("[i] = ").append(name2).append("[i];\n"));

			//res1 = pathAdd(lnidx, counter, 2, 2);
			//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
			res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));
			res.push_back(string("\tpp.addValue(i);\n"));

			//res.push_back(string("\t\tpp.path").append("[pp.pathLen").append("+1] = i;\n"));

			res1 = fieldSubscrUpdate(fld, fieldType, mainObj, lnidx, name2+"[i]", name1);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
		{
			//res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
			res =  fillFieldComparator(0, fld, fieldType, name1, name2, "", true, mainObj);

			res.push_back(string("{\n"));
			res.push_back(string("\t").append(name1).append(" = ").append(name2).append(";\n"));
			//res1 = pathAdd(lnidx, counter, 1);
			//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
			res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));

			res1 = fieldSubscrUpdate(fld, fieldType, mainObj, lnidx, name2, "");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}

			res.push_back(string("}\n"));
		}
		return res;
	}
	vector<string> fillFieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		throw exception(string("Cannot use subscription add to the simple field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append("'\n").c_str());
	}
	vector<string> fillFieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		throw exception(string("Cannot use subscription remove to the simple field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append("'\n").c_str());
	}

	bool isLeaf(void) override
	{
		return true;
	}
	bool isSubscriptionUpdate(void) override
	{
		return true;
	}
	bool isSubscriptionAdd(void) override
	{
		return false;
	}
	bool isSubscriptionRemove(void) override
	{
		return false;
	}
	vector<string> fieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("CommMsgBody data;\n"));

		res1 = fillCompose(fld, fieldType, name1, "default", mainObj, false, dum);
		res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));

		res.push_back(string("publisher.updateLeaf(pp.path,pp.pathLen,data,&update);\n"));
		return res;
	}
	vector<string> fieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("CommMsgBody data;\n"));

		res1 = fillCompose(fld, fieldType, name2, "default", mainObj, false, dum);
		res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));

		if(lnidx.length())
		{
			res.push_back(string("int idx = publisher.appendLeaf(pp.path,pp.pathLen,data,&update);\n"));
			res.push_back(string("if(idx != ").append(lnidx).append(") throw PError(\"")
				.append(fld->getClassName()).append(" field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));
		}
		else
			res.push_back(string("publisher.appendLeaf(pp.path,pp.pathLen,data,&update);\n"));
		return res;
	}
	vector<string> fieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string("publisher.removeLeaf(pp.path,pp.pathLen,&update);\n"));
		return res;
	}
	virtual bool isSimpleNumber(void) override
	{
		return true;
	}

	vector<string> fillSelFields(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj) override
	{
		vector<string> res, res1;
		bool dum = false;

		res.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
		//res.push_back(string("{\n"));
		if(fld->getRefClass().first.size())
		{
			const CGbaseobj* obj = mainObj->findObject(objTypes::gTypeStruct, fld->getRefClass().first);
			if(obj)
			{
				if (mainObj->baseRefClass.size() == 0)
					throw exception(string("Empty base reference class").c_str());
				res.push_back(string("\tif(composeSelectiveFieldsInt_code & 0x80) // reference\n"));
				res.push_back(string("\t{\n"));
				//res.push_back(string("\t\tPASSERT3(0);\n"));

				res.push_back(string("\t\tif(").append(fld->getName()).append(")\n"));
				res.push_back(string("\t\t{\n"));
				res.push_back(string("\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", nullptr, ").append(fld->getName()).append(", param1, param2);\n"));
				res.push_back(string("\t\t\tif(objPtr)\n"));
				res.push_back(string("\t\t\t{\n"));
				res.push_back(string("\t\t\t\tdata.composeUINT32(1);\n"));
				res.push_back(string("\t\t\t\tobjPtr->composeSelectiveFieldsCounted(path, data, path_pointer, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));
				res.push_back(string("\t\t\t}\n"));
				res.push_back(string("\t\t\telse\n"));
				res.push_back(string("\t\t\t{\n"));


				res.push_back(string("\t\t\t\tdata.composeUINT32(0);\n"));
				res.push_back(string("\t\t\t\tpath_pointer += composeSelectiveFieldsInt_size;\n"));
				//res.push_back(string("\t\t\terr.append(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
				//	.append(fld->getClassName()).append("' value '\").").append("appendInt(").append(fld->getName()).append(").append(").append("\"'\");\n"));
				//res.push_back(string("\t\t\tthrow PError(err.c_str());\n"));
				//res.push_back(string("\t\t\tunique_ptr<").append(fld->getRefClass().first).append("> objP (new ").append(fld->getRefClass().first).append(");\n"));
				//res.push_back(string("\t\t\tobjP->composeSelectiveFieldsCounted(path, data, path_pointer, size, getRefObject, param1, param2);\n"));


				res.push_back(string("\t\t\t}\n"));
				res.push_back(string("\t\t}\n"));
				res.push_back(string("\t\telse\n"));
				res.push_back(string("\t\t{\n"));
				res.push_back(string("\t\t\tdata.composeUINT32(0);\n"));
				res.push_back(string("\t\t\tpath_pointer += composeSelectiveFieldsInt_size;\n"));
				res.push_back(string("\t\t}\n"));

				res.push_back(string("\t}\n"));
				res.push_back(string("\telse\n"));
			}
			else
				throw exception(string("Wrong reference class '").append(fld->getRefClass().first).append("  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'").c_str());
		}

		res.push_back(string("\t{\n"));

		if(fld->getSelConvertFuncName().length() && (!fld->isArray()))
		{
			res1 = fillSelConvertCompose(fld, fieldType, fld->getName(), mainObj);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			//res.push_back(string("\t\t").append(fieldType->getFullTypeString()).append(" _temp_").append(fld->getName()).append(";\n"));
			//res.push_back(string("\t\tbool _temp_bool_").append(fld->getName()).append(" = ").append(fld->getSelConvertFuncName()).append("(").append(fld->getName()).append(", _temp_").append(fld->getName()).append(");\n"));
			//res.push_back(string("\t\tif(_temp_bool_").append(fld->getName()).append(")\n"));
			//res.push_back(string("\t\t\t").append("data.compose").append(fieldType->getFullTypeString()).append("(_temp_").append(fld->getName()).append(");\n"));
			//res.push_back(string("\t\telse\n"));
			//res.push_back(string("\t\t\t").append("data.compose").append(fieldType->getFullTypeString()).append("(").append(fld->getName()).append(");\n"));
		}
		else
		{
			res1 = fillCompose(fld, fieldType, fld->getName(), "default", mainObj, fld->isArray(), dum);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("\tbreak;\n"));
		//res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillGetters(const CGfield* fld)
	{
		vector<string> res;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = " ";
		res.push_back(string("").append(fld->getFullTypeString()).append(s).append(fld->getGetter()).append("(void) const {return ").append(fld->getName()).append(";}\n"));
		return res;
	}
	virtual vector<string> fillSetters(const CGfield* fld)
	{
		vector<string> res, res1;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = " ";
		res.push_back(string("void ").append(fld->getSetter()).append("(").append(fld->getFullTypeString()).append(s).append(fld->getName()).append("_data)\n"));
		res.push_back(string("{\n"));
		res1 = fillAssignment(fld, fld->getName(), fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
};

struct CGGpyrUINT8cls : public CGGpyrdgtcls
{
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parseBYTE(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("parser.parseBYTE(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.composeBYTE(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.composeBYTE(").append(name).append(");\n"));
		return res;
	}
};

struct CGGpyrboolcls : public CGGpyrdgtcls
{
	virtual string fillFieldJson(const CGfieldtype* fieldType, const string& name1, const string& name2, int margin1, int margin2, int counter, bool last) override
	{
		string res, res1, res2;
		for (int i = 0; i < margin1; ++i)
		{
			res1.append("\t");
		}
		for (int i = 0; i < margin2; ++i)
		{
			res2.append("  ");
		}
		res.append(res1).append("output.append(margin_string).append(\"").append(res2);

		if (name1.size())
			res.append("\\\"").append(name1).append("\\\": ");

		if (fieldType->isArray())
		{
			res.append("[\\n\");\n");
			res.append(res1).append("for(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n");
			res.append(res1).append("{\n");

			res.append(res1).append("\tif((i+1)<").append(to_string(fieldType->getArraySize())).append(") \n");
			res.append(res1).append("\t{\n");
			res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  \").append(").append(name2).append("[i]?\"true\":\"false\").append(\",\\n\");\n");
			res.append(res1).append("\t}\n");
			res.append(res1).append("\telse\n");
			res.append(res1).append("\t{\n");
			res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  \").append(").append(name2).append("[i]?\"true\":\"false\").append(\"\\n\");\n");
			res.append(res1).append("\t}\n");

			res.append(res1).append("}\n");
			res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("]\")");
		}
		else
		{
			res.append("\").append(").append(name2).append("?\"true\":\"false\")");
		}

		if (!last)
		{
			res.append(".append(\",\")");

		}
		res.append(".append(\"\\n\");\n");
		return res;
	}

	virtual string getFieldJson(const CGfield* fld, const CGfieldtype* fieldType, const string& name, int margin, int counter)
	{
		string res, res1;
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}
		if (fieldType->isArray())
		{
			res.append(res1).append("for(size_t i=0; ((i<").append(to_string(fieldType->getArraySize())).append(")&&(i<(elem0->v).size())); ++i)\n");
			res.append(res1).append("{\n");
			res.append(res1).append("\t").append(name).append("[i] = elem0->v[i]->b;\n");
			res.append(res1).append("}\n");
		}
		else
		{
			res.append(res1).append(name).append(" = elem").append(to_string(counter)).append("->b;\n");
		}
		return res;
	}

	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append("; ").append(name2).append(" = false;\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		vector<string> res;
		if(fld->getDefault().length() && fld->getDefault() != "false")
		{
			res.push_back(string(name).append(" = true;\n"));
		}
		else
		{
			res.push_back(string(name).append(" = false;\n"));
		}
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parseBOOL(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
		{
			res.push_back(string("parser.parseBOOL(").append(name).append(");\n"));
			//res.push_back(string("bool bool_").append(fld->getName()).append("; parser.parseBOOL(bool_").append(fld->getName()).append("); ").append(name).append(" = bool_").append(fld->getName()).append(";\n"));
		}
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.composeBOOL(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.composeBOOL(").append(name).append(");\n"));
		return res;
	}
};

struct CGGpyrtimetcls : public CGGpyrdgtcls
{
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		vector<string> res;
		if(fld->getDefault().length())
		{
			//std::stoi(fld->getDefault());
			res.push_back(string(name).append(" = ").append(fld->getDefault()).append(";\n"));
		}
		else
		{
			res.push_back(string(name).append(" = plib_time(0);\n"));
		}
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parseINT64(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("parser.parseINT64(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.composeINT64(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.composeINT64(").append(name).append(");\n"));
		return res;
	}
};

struct CGGpyrPStringcls :public CGGpyrdgtcls
{
	virtual string fillFieldJson(const CGfieldtype* fieldType, const string& name1, const string& name2, int margin1, int margin2, int counter, bool last) override
	{
		string res, res1, res2;
		for (int i = 0; i < margin1; ++i)
		{
			res1.append("\t");
		}
		for (int i = 0; i < margin2; ++i)
		{
			res2.append("  ");
		}
		res.append(res1).append("output.append(margin_string).append(\"").append(res2);

		if (name1.size())
			res.append("\\\"").append(name1).append("\\\": ");

		if (fieldType->isArray())
		{
			res.append("[\\n\");\n");
			res.append(res1).append("for(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n");
			res.append(res1).append("{\n");

			res.append(res1).append("\tif((i+1)<").append(to_string(fieldType->getArraySize())).append(") \n");
			res.append(res1).append("\t{\n");
			res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  \\\"\").append(").append(name2).append("[i]).append(\"\\\",\\n\");\n");
			res.append(res1).append("\t}\n");
			res.append(res1).append("\telse\n");
			res.append(res1).append("\t{\n");
			res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  \\\"\").append(").append(name2).append("[i]).append(\"\\\"\\n\");\n");
			res.append(res1).append("\t}\n");

			res.append(res1).append("}\n");
			res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("]\")");
		}
		else
		{
			res.append("\\\"\").append(").append(name2).append(").append(\"\\\"\")");
		}

		if (!last)
		{
			res.append(".append(\",\")");

		}
		res.append(".append(\"\\n\");\n");
		return res;
	}

	virtual string getFieldJson(const CGfield* fld, const CGfieldtype* fieldType, const string& name, int margin, int counter)
	{
		string res, res1;
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}
		if (fieldType->isArray())
		{
			res.append(res1).append("for(size_t i=0; ((i<").append(to_string(fieldType->getArraySize())).append(")&&(i<(elem0->v).size())); ++i)\n");
			res.append(res1).append("{\n");
			res.append(res1).append("\t").append(name).append("[i] = elem0->v[i]->s;\n");
			res.append(res1).append("}\n");
		}
		else
		{
			res.append(res1).append(name).append(" = elem").append(to_string(counter)).append("->s;\n");
		}
		return res;
	}

	vector<string> fillFieldAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append(";\n"));
		return res;
	}
	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(".moveFrom(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		vector<string> res;
		if(fld->getDefault().length())
		{
			res.push_back(string(name).append(" = \"").append(fld->getDefault()).append("\";\n"));
		}
		return res;
	}

	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res;
		uint64_t restr = fld->getRestrStringValue();
		if(restr)
		{
			res.push_back(string("if(").append(negat?"":"!").append("strncmp(").append(name1).append(".c_str(),").append(name2).append(".c_str(),").append(to_string(restr)).append("))").append(name3).append("\n"));
		}
		else
			res.push_back(string("if(").append(negat?"!":"").append(name1).append(".equals(").append(name2).append("))").append(name3).append("\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res;
		uint64_t restr = fld->getRestrStringValue();
		if(restr)
		{
			res.push_back(string("if(strncmp(").append(name1).append(".c_str(),").append(name2).append(".c_str(),").append(to_string(restr)).append("))\n"));
		}
		else
			res.push_back(string("if(!").append(name1).append(".equals(").append(name2).append("))\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL\");\n"));
		res.push_back(string("\tresult = false;\n"));
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parseStringP(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("parser.parseStringP(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.composeString(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.composeString(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillSelConvertCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		if(fld->getSelConvertFuncName().length() && (!fld->isArray()))
		{
			vector<string> res;
			res.push_back(string("{\n"));
			res.push_back(string("\t").append(fieldType->getFullTypeString()).append(" _temp_").append(fld->getName()).append(";\n"));
			res.push_back(string("\tbool _temp_bool_").append(fld->getName()).append(" = ").append(fld->getSelConvertFuncName()).append("(").append(fld->getName()).append(", _temp_").append(fld->getName()).append(", this, \"").append(fld->getClassName()).append("\", \"").append(fld->getName()).append("\");\n"));
			res.push_back(string("\tif(_temp_bool_").append(fld->getName()).append(")\n"));
			res.push_back(string("\t\t").append("data.composeString(_temp_").append(fld->getName()).append(");\n"));
			res.push_back(string("\telse\n"));
			res.push_back(string("\t\t").append("data.composeString(").append(fld->getName()).append(");\n"));
			res.push_back(string("}\n"));
			return res;
		}
		else
		{
			bool vecSmartCorr;
			return fillCompose(fld, fieldType, name, "default", mainObj, false, vecSmartCorr);
		}
	}

	virtual bool isSimpleString(void) override
	{
		return true;
	}

	virtual bool isSimpleNumber(void) override
	{
		return false;
	}

	vector<string> fillSelFields(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj) override
	{
		vector<string> res, res1;
		bool dum = false;

		res.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
		//res.push_back(string("{\n"));
		if(fld->getRefClass().first.size())
		{
			const CGbaseobj* obj = mainObj->findObject(objTypes::gTypeStruct, fld->getRefClass().first);
			if(obj)
			{
				if (mainObj->baseRefClass.size() == 0)
					throw exception(string("Empty base reference class").c_str());
				res.push_back(string("\tif(composeSelectiveFieldsInt_code & 0x80) // reference\n"));
				res.push_back(string("\t{\n"));
				//res.push_back(string("\t\tPASSERT3(0);\n"));

				res.push_back(string("\t\tif(").append(fld->getName()).append(".length())\n"));
				res.push_back(string("\t\t{\n"));
				res.push_back(string("\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", ").append(fld->getName()).append(", 0, param1, param2);\n"));
				res.push_back(string("\t\t\tif(objPtr)\n"));
				res.push_back(string("\t\t\t{\n"));
				res.push_back(string("\t\t\t\tdata.composeUINT32(1);\n"));
				res.push_back(string("\t\t\t\tobjPtr->composeSelectiveFieldsCounted(path, data, path_pointer, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));


				//res.push_back(string("\t\t\tPString err;\n"));
				//res.push_back(string("\t\t\terr.append(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
				//	.append(fld->getClassName()).append("' value '\").").append("append(").append(fld->getName()).append(").append(").append("\"'\");\n"));
				//res.push_back(string("\t\t\tPLog(err.c_str());\n"));
				//res.push_back(string("\t\t\tthrow PError(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'\");\n"));
				//res.push_back(string("\t\t\tunique_ptr<").append(fld->getRefClass().first).append("> objP (new ").append(fld->getRefClass().first).append(");\n"));
				//res.push_back(string("\t\t\tobjP->composeSelectiveFieldsCounted(path, data, path_pointer, size, getRefObject, param1, param2);\n"));
				//res.push_back(string("\t\t\tthrow PError(err.c_str());\n"));

				res.push_back(string("\t\t\t}\n"));
				res.push_back(string("\t\t\telse\n"));
				res.push_back(string("\t\t\t{\n"));
				res.push_back(string("\t\t\t\tdata.composeUINT32(0);\n"));
				res.push_back(string("\t\t\t\tpath_pointer += composeSelectiveFieldsInt_size;\n"));
				res.push_back(string("\t\t\t}\n"));
				res.push_back(string("\t\t}\n"));
				res.push_back(string("\t\telse\n"));
				res.push_back(string("\t\t{\n"));
				res.push_back(string("\t\t\tdata.composeUINT32(0);\n"));
				res.push_back(string("\t\t\tpath_pointer += composeSelectiveFieldsInt_size;\n"));
				res.push_back(string("\t\t}\n"));

				res.push_back(string("\t}\n"));
				res.push_back(string("\telse\n"));
			}
			else
				throw exception(string("Wrong reference class '").append(fld->getRefClass().first).append("  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'").c_str());
		}
		res.push_back(string("\t{\n"));

		if(fld->getSelConvertFuncName().length() && (!fld->isArray()))
		{
			res1 = fillSelConvertCompose(fld, fieldType, fld->getName(), mainObj);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			//res.push_back(string("\t\t").append(fieldType->getFullTypeString()).append(" _temp_").append(fld->getName()).append(";\n"));
			//res.push_back(string("\t\tbool _temp_bool_").append(fld->getName()).append(" = ").append(fld->getSelConvertFuncName()).append("(").append(fld->getName()).append(", _temp_").append(fld->getName()).append(");\n"));
			//res.push_back(string("\t\tif(_temp_bool_").append(fld->getName()).append(")\n"));
			//res.push_back(string("\t\t\t").append("data.composeString(_temp_").append(fld->getName()).append(");\n"));
			//res.push_back(string("\t\telse\n"));
			//res.push_back(string("\t\t\t").append("data.composeString(").append(fld->getName()).append(");\n"));
		}
		else
		{
			res1 = fillCompose(fld, fieldType, fld->getName(), "default", mainObj, fld->isArray(), dum);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("\tbreak;\n"));
		//res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillGetters(const CGfield* fld)
	{
		vector<string> res;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = " ";
		res.push_back(string("const char* ").append(fld->getGetter()).append("(void) const {return ").append(fld->getName()).append(";}\n"));
		return res;
	}
	virtual vector<string> fillSetters(const CGfield* fld)
	{
		vector<string> res, res1;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = " ";
		res.push_back(string("void ").append(fld->getSetter()).append("(const char* ").append(fld->getName()).append("_data)\n"));
		res.push_back(string("{\n"));
		res1 = fillAssignment(fld, fld->getName(), fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
};

struct CGGpyrPBlockcls :public CGGpyrdgtcls
{
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		return vector<string>();
	}
	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(".moveFrom(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parsePBlock(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("parser.parsePBlock(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.composePBlock(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.composePBlock(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("if((").append(name1).append(".size() ").append(negat?"!= ":"== ").append(name2).append(".size())")
			.append(negat ? " || " : " && ").append("(0 != memcmp(").append(name1).append(".ptr(), ").append(name2)
			.append(".ptr(), ").append(name1).append(".size())))").append(name3).append("\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("{\n"));
		res.push_back(string("\tif(").append(name1).append(".size() != ").append(name2).append(".size())\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL by size.\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\telse if(0 != memcmp(").append(name1).append(".ptr(), ").append(name2)
			.append(".ptr(), ").append(name1).append(".size()))\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL by body compare.\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
};

struct CGGpyrCommMsgBodycls :public CGGpyrdgtcls
{
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		return vector<string>();
	}
	vector<string> fillFieldAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(".copyFrom(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(".moveFrom(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tparser.parseMsgBody(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("parser.parseMsgBody(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\tdata.composeMsgBody(").append(name).append("[i]);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string("data.composeMsgBody(").append(name).append(");\n"));
		return res;
	}
	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("if((").append(name1).append("._size() ").append(negat?"!= ":"== ").append(name2).append("._size())")
			.append(negat ? " || " : " && ").append("(0 != memcmp(").append(name1).append("._readPtr(), ").append(name2)
			.append("._readPtr(), ").append(name1).append("._size())))").append(name3).append("\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("{\n"));
		res.push_back(string("\tif(").append(name1).append("._size() != ").append(name2).append("._size)\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL by size.\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\telse if(0 != memcmp(").append(name1).append("._readPtr(), ").append(name2)
			.append("._readPtr(), ").append(name1).append("._size()))\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL by body compare.\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
};


struct CGGpyrUTCtimecls :public CGGbasecls
{
	virtual string fillFieldJson(const CGfieldtype* fieldType, const string& name1, const string& name2, int margin1, int margin2, int counter, bool last) override
	{
		string res, res1, res2;
		for (int i = 0; i < margin1; ++i)
		{
			res1.append("\t");
		}
		for (int i = 0; i < margin2; ++i)
		{
			res2.append("  ");
		}
		res.append(res1).append("output.append(margin_string).append(\"").append(res2);

		if (name1.size())
			res.append("\\\"").append(name1).append("\\\": ");

		res.append("{\\n\");\n");

		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_year\\\": \").appendInt(").append(name2).append("._year).append(\",\\n\");\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_month\\\": \").appendInt(").append(name2).append("._month).append(\",\\n\");\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_day\\\": \").appendInt(").append(name2).append("._day).append(\",\\n\");\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_hour\\\": \").appendInt(").append(name2).append("._hour).append(\",\\n\");\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_minute\\\": \").appendInt(").append(name2).append("._minute).append(\",\\n\");\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_second\\\": \").appendInt(").append(name2).append("._second).append(\",\\n\");\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("  \\\"_wday\\\": \").appendInt(").append(name2).append("._wday).append(\"\\n\");\n");

		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("}\")");

		if (!last)
		{
			res.append(".append(\",\")");

		}
		res.append(".append(\"\\n\");\n");
		return res;
	}

	virtual string getFieldJson(const CGfield* fld, const CGfieldtype* fieldType, const string& name, int margin, int counter)
	{
		string res, res1;
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}

		res.append(res1).append("for(size_t i=0; i<(elem").append(to_string(counter)).append("->v).size(); ++i)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_year\")) ").append(name).append("._year = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_month\")) ").append(name).append("._month = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_day\")) ").append(name).append("._day = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_hour\")) ").append(name).append("._hour = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_minute\")) ").append(name).append("._minute = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_second\")) ").append(name).append("._second = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("\tif(elem").append(to_string(counter)).append("->v[i]->name.equals(\"_wday\")) ").append(name).append("._wday = elem").append(to_string(counter)).append("->v[i]->i;\n");
		res.append(res1).append("}\n");

		return res;
	}

	bool isTimeData(void) override
	{
		return true;
	}

	vector<string> fillFieldAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append(";\n"));
		return res;
	}
	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = ").append(name2).append("; ").append(name2).append(".setNull();\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		vector<string> res;
		res.push_back(string(name).append(".setNull();\n"));
		return res;
	}
	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("if(").append(negat?"!":"").append(name1).append(".equals(").append(name2).append("))").append(name3).append("\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res;
		res.push_back(string("if(!").append(name1).append(".equals(").append(name2).append("))\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL\");\n"));
		res.push_back(string("\tresult = false;\n"));
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
			res.push_back(string("\tmemset(").append(name).append(",0,sizeof(").append(name).append("));\n"));
			res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
			res.push_back(string("\tif(").append(fld->getName()).append("_count >= ").append(to_string(fld->getArraySize())).append(") ").append(fld->getName()).append("_count = ").append(to_string(fld->getArraySize())).append(";\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\t").append(name).append("[i].parse").append("(parser);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string(name).append(".parse").append("(parser);\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if(isArray)
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tdata.composeUINT32(").append(to_string(fld->getArraySize())).append(");\n"));
			res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fld->getArraySize())).append("; ++i)\n"));
			res.push_back(string("\t{\n"));

			res.push_back(string("\t\t").append(name).append("[i].compose").append("(data);\n"));

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
			res.push_back(string(name).append(".compose").append("(data);\n"));
		return res;
	}
	vector<string> fillFieldPublisher(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter) override 
	{
		return vector<string>();
	}
	vector<string> fillFieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		vector<string> res, res1;
		if(fieldType->isArray())
		{
			res.push_back(string("for(size_t i=0; i<").append(fieldType->getArraySizeString()).append("; ++i)\n"));
			res.push_back(string("{\n"));
			//res.push_back(string("\tif(").append(name1).append("[i").append(to_string(counter)).append("] != ").append(name2).append("[i").append(to_string(counter)).append("])\n"));
			res1 =  fillFieldComparator(0, fld, fieldType, name1+"[i]", name2+"[i]", "", true, mainObj);
			for(auto elem : res1)
			{
				res.push_back(string("").append(move(elem)));
			}

			res.push_back(string("\t{\n"));
			res.push_back(string("\t\t").append(name1).append("[i] = ").append(name2).append("[i];\n"));

			//res1 = pathAdd(lnidx, counter, 2, 2);
			//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
			res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));
			res.push_back(string("\tpp.addValue(i);\n"));

			//res.push_back(string("\t\tpp.path").append("[pp.pathLen").append("+1] = i;\n"));

			res1 = fieldSubscrUpdate(fld, fieldType, mainObj, lnidx, name2+"[i]", name1);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}

			res.push_back(string("\t}\n"));
			res.push_back(string("}\n"));
		}
		else
		{
			//res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
			res =  fillFieldComparator(0, fld, fieldType, name1, name2, "", true, mainObj);

			res.push_back(string("{\n"));
			res.push_back(string("\t").append(name1).append(" = ").append(name2).append(";\n"));
			//res1 = pathAdd(lnidx, counter, 1);
			//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
			res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));

			res1 = fieldSubscrUpdate(fld, fieldType, mainObj, lnidx, name2, "");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}

			res.push_back(string("}\n"));
		}
		return res;
	}
	vector<string> fillFieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		throw exception(string("Cannot use subscription add to the simple field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append("'\n").c_str());
	}
	vector<string> fillFieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		throw exception(string("Cannot use subscription remove to the simple field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append("'\n").c_str());
	}

	bool isLeaf(void) override
	{
		return true;
	}
	bool isSubscriptionUpdate(void) override
	{
		return true;
	}
	bool isSubscriptionAdd(void) override
	{
		return false;
	}
	bool isSubscriptionRemove(void) override
	{
		return false;
	}
	vector<string> fieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("CommMsgBody data;\n"));

		res1 = fillCompose(fld, fieldType, name1, "default", mainObj, false, dum);
		res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));

		res.push_back(string("publisher.updateLeaf(pp.path,pp.pathLen,data,&update);\n"));
		return res;
	}
	vector<string> fieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("CommMsgBody data;\n"));

		res1 = fillCompose(fld, fieldType, name2, "default", mainObj, false, dum);
		res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));

		if(lnidx.length())
		{
			res.push_back(string("int idx = publisher.appendLeaf(pp.path,pp.pathLen,data,&update);\n"));
			res.push_back(string("if(idx != ").append(lnidx).append(") throw PError(\"")
				.append(fld->getClassName()).append(" field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));
		}
		else
			res.push_back(string("publisher.appendLeaf(pp.path,pp.pathLen,data,&update);\n"));
		return res;
	}
	vector<string> fieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string("publisher.removeLeaf(pp.path,pp.pathLen,&update);\n"));
		return res;
	}
	vector<string> fillSelFields(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj) override
	{
		vector<string> res, res1;
		bool dum = false;

		res.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
		//res.push_back(string("{\n"));
		//res.push_back(string("\tif(code & 0x80) // ref\n"));
		//res.push_back(string("\t{\n"));
		//res.push_back(string("\t\tPASSERT3(0);\n"));
		//res.push_back(string("\t}\n"));
		//res.push_back(string("\telse\n"));
		res.push_back(string("\t{\n"));
		res1 = fillCompose(fld, fieldType, fld->getName(), "default", mainObj, fld->isArray(), dum);
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("\tbreak;\n"));
		//res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillGetters(const CGfield* fld)
	{
		vector<string> res;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = " ";
		res.push_back(string("").append(fld->getFullTypeString()).append(s).append(fld->getGetter()).append("(void) const {return ").append(fld->getName()).append(";}\n"));
		return res;
	}
	virtual vector<string> fillSetters(const CGfield* fld)
	{
		vector<string> res, res1;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = " ";
		res.push_back(string("void ").append(fld->getSetter()).append("(").append(fld->getFullTypeString()).append(s).append(fld->getName()).append("_data)\n"));
		res.push_back(string("{\n"));
		res1 = fillAssignment(fld, fld->getName(), fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
};


#endif	// CGGsimplecls_h_included

