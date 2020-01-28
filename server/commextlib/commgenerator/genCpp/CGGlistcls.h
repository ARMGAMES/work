// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGlistcls_h_included
#define CGGlistcls_h_included

//******************************************************************************
// Set classes for text generators
//******************************************************************************
struct CGGlistcls : public CGGbasecls
{
	virtual string fillFieldJson(const CGfieldtype* fieldType, const string& name1, const string& name2, int margin1, int margin2, int counter, bool last) override
	{
		string res, res1, res2;
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
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

		res.append("[\\n\");\n");
		res.append(res1).append("for(auto& it").append(to_string(counter)).append(" = ").append(name2).append(".begin(); it").append(to_string(counter)).append(" != ").append(name2).append(".end();)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\tauto& elem").append(to_string(counter)).append(" = *it").append(to_string(counter)).append(";\n");
		res.append(res1).append("\tif(++it").append(to_string(counter)).append(" != ").append(name2).append(".end())\n");
		res.append(res1).append("\t{\n");
		auto resInt = bcls->fillFieldJson(fieldType->getZeroSubType(), "", string("elem") + (to_string(counter)), margin1 + 2, margin2 + 1, counter + 1, false);
		res.append(resInt);
		res.append(res1).append("\t}\n");
		res.append(res1).append("\telse\n");
		res.append(res1).append("\t{\n");
		resInt = bcls->fillFieldJson(fieldType->getZeroSubType(), "", string("elem") + (to_string(counter)), margin1 + 2, margin2 + 1, counter + 1, true);
		res.append(resInt);
		res.append(res1).append("\t}\n");
		res.append(res1).append("}\n");
		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("]\")");

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
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}

		res.append(res1).append("for(auto& elem").append(to_string(counter + 1)).append(" : elem").append(to_string(counter)).append("->v)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\t").append(name).append(".push_back(").append(fieldType->getZeroSubType()->getFieldTypeStr()).append("());\n");
		res.append(res1).append("\tauto& refelem").append(to_string(counter + 1)).append(" = ").append(name).append(".back();\n");
		auto resInt = bcls->getFieldJson(fld, fieldType->getZeroSubType(), string("refelem") + to_string(counter + 1), margin + 1, counter + 1);
		res.append(resInt);
		res.append(res1).append("}\n");

		return res;
	}

	vector<string> fillFieldAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string("").append(name1).append(".clear();\n"));
		res.push_back(string(name1).append(".insert(").append(name1).append(".end(), ").append(name2).append(".begin(), ").append(name2).append(".end());\n"));
		return res;
	}
	vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string(name1).append(" = move(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		return vector<string>();
	}

	virtual vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		//if(!fieldType->getSubTypes().size())
		//{
		//	string err;
		//	err.append("No subtype in 'set' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
		//	throw exception(err.c_str());
		//}
		vector<string> res, res1;
		string iter1 = fld->getName()+"_iter1_"+to_string(counter);
		string iter2 = fld->getName()+"_iter2_"+to_string(counter);

		res.push_back(string("{\n"));
		res.push_back(string("\t").append(fieldType->getFullTypeString()).append("::const_iterator ").append(iter1).append(";\n"));
		res.push_back(string("\t").append(fieldType->getFullTypeString()).append("::const_iterator ").append(iter2).append(";\n"));
		res.push_back(string("\tif(").append(name1).append(".size() != ").append(name2).append(".size())\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' type '").append(fieldType->getFullTypeString()).append("' are NOT EQUAL by size.\");\n"));
		res.push_back(string("\t\tresult = false;\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\telse for(").append(iter1).append("=").append(name1).append(".begin(), ").append(iter2)
			.append("=").append(name2).append(".begin(); ").append(iter1).append("!=").append(name1).append(".end(); ++").append(iter1).append(", ++").append(iter2).append(")\n"));
		res.push_back(string("\t{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillFieldComparatorLog(counter+1, fld, fieldType->getZeroSubType(), string("(*")+iter1+")", string("(*")+iter2+")", mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}

	virtual vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		//if(!fieldType->getSubTypes().size())
		//{
		//	string err;
		//	err.append("No subtype in 'set' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
		//	throw exception(err.c_str());
		//}
		vector<string> res, res1;
		string iter1 = fld->getName()+"_iter1_"+to_string(counter);
		string iter2 = fld->getName()+"_iter2_"+to_string(counter);

		res.push_back(string("{\n"));
		res.push_back(string("\t").append(fieldType->getFullTypeString()).append("::const_iterator ").append(iter1).append(";\n"));
		res.push_back(string("\t").append(fieldType->getFullTypeString()).append("::const_iterator ").append(iter2).append(";\n"));
		res.push_back(string("\tif(").append(name1).append(".size() != ").append(name2).append(".size())").append(name3).append("\n"));

		res.push_back(string("\telse for(").append(iter1).append("=").append(name1).append(".begin(), ").append(iter2)
			.append("=").append(name2).append(".begin(); ").append(iter1).append("!=").append(name1).append(".end(); ++").append(iter1).append(", ++").append(iter2).append(")\n"));
		res.push_back(string("\t{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillFieldComparator(counter+1, fld, fieldType->getZeroSubType(), string("(*")+iter1+")", string("(*")+iter2+")", name3, negat, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}

	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		//if(!fieldType->getSubTypes().size())
		//{
		//	string err;
		//	err.append("No subtype in 'set' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
		//	throw exception(err.c_str());
		//}
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("{\n"));
		res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
		res.push_back(string("\t").append(name).append(".clear();\n"));
		res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
		res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), fld->getName()+"_data", channel, mainObj, isArray, dum);
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(fieldType->getZeroSubType()->getFieldTypeStr()).append(" ").append(fld->getName()).append("_data;\n"));
			res.push_back(string("\t\t").append(move(elem)));
			res.push_back(string("\t\t").append(name).append(".push_back(").append("move(").append(fld->getName()).append("_data").append("));\n"));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		//if(!fieldType->getSubTypes().size())
		//{
		//	string err;
		//	err.append("No subtype in 'set' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
		//	throw exception(err.c_str());
		//}
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("{\n"));
		res.push_back(string("\t").append(fieldType->getFullTypeString()).append("::const_iterator ").append(fld->getName()).append("_iter;\n"));
		res.push_back(string("\tsize_t ").append(fld->getName()).append("_size = ").append(fld->getName()).append(".size();\n"));
		res.push_back(string("\tdata.composeUINT32(").append(fld->getName()).append("_size);\n"));

		res.push_back(string("\tfor(").append(fld->getName()).append("_iter=").append(name).append(".begin(); ").append(fld->getName()).append("_iter!=")
			.append(name).append(".end(); ++").append(fld->getName()).append("_iter)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), string("*(")+fld->getName()+"_iter)", channel, mainObj, isArray, dum);
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}

	vector<string> fillFieldPublisher(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter) override
	{
		return vector<string>();
		//vector<string> res, res1;

		//res.push_back(string("for(size_t i=0; i<").append(name).append(".size(); ++i)\n"));
		//unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		//res1 = bcls->fillPublisher(fld, fieldType->getZeroSubType(), mainObj, "i", fld->getName()+"[i]");
		//res.insert(res.end(), res1.begin(), res1.end());
		//return res;
	}
	vector<string> fillFieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		return vector<string>();
	}
	vector<string> fillFieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		return vector<string>();
		//vector<string> res, res1;


		//res.push_back(string("for(size_t i=0; i<").append(fld->getName()).append(".size(); ++i)\n"));
		//res.push_back(string("{\n"));
		//unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		//res1 = bcls->fillFieldComparator(1, fld, fieldType, fld->getName()+"[i]", fld->getName()+"_data", " return;", false, mainObj);
		//for(auto elem : res1)
		//{
		//	res.push_back(string("\t").append(move(elem)));
		//}
		//res.push_back(string("}\n"));
		//res.push_back(string("").append(name1).append(".push_back(").append(fld->getName()).append("_data);\n"));

		////res1 = pathAdd(lnidx, counter, 0);
		////res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
		//res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));

		//res1 = bcls->fieldSubscrAdd(fld, fieldType->getZeroSubType(), mainObj, "", "", fld->getName()+"_data");
		//for(auto elem : res1)
		//{
		//	res.push_back(string("").append(move(elem)));
		//}

		//return res;
	}
	virtual vector<string> fillFieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		return vector<string>();
		//vector<string> res, res1;

		//res.push_back(string("size_t i;\n"));
		//res.push_back(string("for(i=0; i<").append(fld->getName()).append(".size(); ++i)\n"));
		//res.push_back(string("{\n"));
		//unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		//res1 = bcls->fillFieldComparator(1, fld, fieldType, fld->getName()+"[i]", fld->getName()+"_data", " break;", false, mainObj);
		//for(auto elem : res1)
		//{
		//	res.push_back(string("\t").append(move(elem)));
		//}
		//res.push_back(string("}\n"));

		//res.push_back(string("if(i == ").append(name1).append(".size()) return;\n"));
		//res.push_back(string("").append(name1).append(".erase(").append(name1).append(".begin()+i);\n"));

		////res1 = pathAdd(lnidx, counter, 0);
		////res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
		//res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));
		//res.push_back(string("\tpp.addValue(i);\n"));

		////res.push_back(string("path").append(to_string(counter+1)).append("[pathLen").append(to_string(counter)).append("+1] = i;\n"));
		////res.push_back(string("\tpublisher.removeLeaf(path").append(to_string(counter+1)).append(",pathLen").append(to_string(counter+1)).append(",data,&update);\n"));
		//res1 = bcls->fieldSubscrRemove(fld, fieldType->getZeroSubType(), mainObj, "", "", fld->getName()+"_data");
		//for(auto elem : res1)
		//{
		//	res.push_back(string("").append(move(elem)));
		//}
		//return res;
	}
	bool isLeaf(void) override
	{
		return false;
	}
	bool isSubscriptionUpdate(void) override
	{
		return false;
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
		return res;
	}
	vector<string> fieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		//res.push_back(string("CommMsgBody data;\n"));
		//res1 = fillCompose(fld, fieldType, name1, "default", mainObj, false);
		//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
		return res;
	}
	vector<string> fieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		return res;
	}

	vector<string> fillSelFields(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj) override
	{
		vector<string> res, res1;
		bool dum = false;

		res.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\t").append(fieldType->getFullTypeString()).append("::const_iterator ").append(fld->getName()).append("_iter;\n"));
		res.push_back(string("\tsize_t ").append(fld->getName()).append("_size;\n"));
		res.push_back(string("\tif(composeSelectiveFieldsInt_filtersize)\n"));
		res.push_back(string("\t\t").append(fld->getName()).append("_size = (").append(fld->getName()).append(".size() < composeSelectiveFieldsInt_filtersize)?").append(fld->getName()).append(".size():composeSelectiveFieldsInt_filtersize;\n"));
		res.push_back(string("\telse\n"));
		res.push_back(string("\t\t").append(fld->getName()).append("_size = ").append(fld->getName()).append(".size();\n"));

		//res.push_back(string("\tdata.composeUINT32(").append(fld->getName()).append("_size);\n"));
		res.push_back(string("\tsize_t shift;\n"));
		res.push_back(string("\tUINT32 count = 0;\n"));
		res.push_back(string("\tdata.composeVectorSize(0,shift);\n"));

		res.push_back(string("\tsize_t i;\n"));
		res.push_back(string("\tfor(").append(fld->getName()).append("_iter=").append(fld->getName()).append(".begin(), i=0; ((").append(fld->getName()).append("_iter!=")
			.append(fld->getName()).append(".end())&&(i<").append(fld->getName()).append("_size)); ++").append(fld->getName()).append("_iter,++i)\n"));
		res.push_back(string("\t{\n"));
		//res.push_back(string("\t\tsize_t path_pointer_int = path_pointer;\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		if(bcls->isOurObject(fieldType->getZeroSubType(), mainObj))
		{

			res.push_back(string("\t\tsize_t path_pointer_int = path_pointer;\n"));
			res.push_back(string("\t\tcount++;\n"));
			res.push_back(string("\t\t(*(").append(fld->getName()).append("_iter)).composeSelectiveFieldsCounted(path, data, path_pointer_int, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));

		}
		else
		{
			if(fld->getRefClass().first.size())
			{
				const CGbaseobj* obj = mainObj->findObject(objTypes::gTypeStruct, fld->getRefClass().first);
				if(obj && (bcls->isSimpleNumber() || bcls->isSimpleString()))
				{
					res.push_back(string("\t\tif(composeSelectiveFieldsInt_code & 0x80) // reference\n"));
					res.push_back(string("\t\t{\n"));

					if(bcls->isSimpleString())
					{
						if (mainObj->baseRefClass.size() == 0)
							throw exception(string("Empty base reference class").c_str());
						res.push_back(string("\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", *(").append(fld->getName()).append("_iter), 0, param1, param2);\n"));
					}
					else if(bcls->isSimpleNumber())
					{
						if (mainObj->baseRefClass.size() == 0)
							throw exception(string("Empty base reference class").c_str());
						res.push_back(string("\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", nullptr, *(").append(fld->getName()).append("_iter), param1, param2);\n"));
					}

					//res.push_back(string("\t\t\tif(!objPtr) throw PError(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'\");\n"));
					res.push_back(string("\t\t\tif(!objPtr)\n"));
					res.push_back(string("\t\t\t{\n"));
					res.push_back(string("\t\t\t\tPString err;\n"));
					res.push_back(string("\t\t\t\terr.append(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
						.append(fld->getClassName()).append("' value '\").").append(bcls->isSimpleNumber()?"appendInt(*(":"append(*(").append(fld->getName()).append("_iter)).append(").append("\"'\");\n"));
					res.push_back(string("\t\t\t\tPLog(err.c_str());\n"));


					//res.push_back(string("\t\t\t\tPLog(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'\");\n"));
					res.push_back(string("\t\t\t\tcontinue;\n"));
					res.push_back(string("\t\t\t}\n"));
					res.push_back(string("\t\t\tsize_t path_pointer_int = path_pointer;\n"));
					res.push_back(string("\t\t\tcount++;\n"));
					res.push_back(string("\t\t\tobjPtr->composeSelectiveFieldsCounted(path, data, path_pointer_int, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));

					res.push_back(string("\t\t}\n"));
					res.push_back(string("\t\telse\n"));
					res.push_back(string("\t\t{\n"));
					res.push_back(string("\t\t\tcount++;\n"));
					res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), string("*(")+fld->getName()+"_iter)", "default", mainObj, false, dum);
					for(auto elem : res1)
					{
						res.push_back(string("\t\t\t").append(move(elem)));
					}
					res.push_back(string("\t\t}\n"));
				}
				else
					throw exception(string("Wrong reference class '").append(fld->getRefClass().first).append("  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'").c_str());
			}
			else
			{
				res.push_back(string("\t\tcount++;\n"));
				res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), string("*(")+fld->getName()+"_iter)", "default", mainObj, false, dum);
				for(auto elem : res1)
				{
					res.push_back(string("\t\t").append(move(elem)));
				}
			}
		}

		res.push_back(string("\t}\n"));
		res.push_back(string("\tdata.updateVectorSize(count,shift);\n"));
		res.push_back(string("\tpath_pointer += composeSelectiveFieldsInt_size;\n"));
		res.push_back(string("\tbreak;\n"));
		res.push_back(string("}\n"));

		return res;
	}

};


#endif	// CGGlistcls_h_included