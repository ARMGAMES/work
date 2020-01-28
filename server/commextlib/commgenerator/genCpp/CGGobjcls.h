// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGobjcls_h_included
#define CGGobjcls_h_included

//******************************************************************************
// Objects classes for text generators
//******************************************************************************
struct CGGobjcls :public CGGbasecls
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

		res.append(res1).append(name2).append(".composeJSONstring(output, \"").append(name2).append("\", margin+2);\n");

		res.append(res1).append("output.append(margin_string).append(\"").append(res2).append("\")");
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
		res.append(res1).append(name).append(".parseJSONstring(elem").append(to_string(counter)).append(");\n");
		return res;
	}

	string fillDefaultConctructorHeader(const CGfield* fld) override
	{
		string s;
		if(fld->getProperty() & CG_CLASS_INTERNAL_INCL)
		{
			s.append(fld->getName()).append("(publisher, update)\n");
		}
		return s;
	}
	string fillCopyConctructorHeader(const CGfield* fld) override
	{
		string s;
		if(fld->getProperty() & CG_CLASS_INTERNAL_INCL)
		{
			s.append(fld->getName()).append("(other.").append(fld->getName()).append(")\n");
		}
		return s;
	}
	string fillMoveConctructorHeader(const CGfield* fld) override
	{
		string s;
		if(fld->getProperty() & CG_CLASS_INTERNAL_INCL)
		{
			s.append(fld->getName()).append("(other.").append(fld->getName()).append(")\n");
		}
		return s;
	}
	vector<string> fillCopyConctructorBody(const CGfield* fld) override
	{
		if(!(fld->getProperty() & CG_CLASS_INTERNAL_INCL))
			return fillAssignment(fld, fld->getName(), string("other.")+fld->getName());
		else
			return vector<string>();
	}
	 vector<string> fillMoveConctructorBody(const CGfield* fld)
	{
		if(!(fld->getProperty() & CG_CLASS_INTERNAL_INCL))
			return fillMoveAssignment(fld, fld->getName(), string("other.")+fld->getName());
		else
			return vector<string>();
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
		res.push_back(string(name1).append(" = move(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		return vector<string>();
	}

	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		vector<string> res;
		bool standr = false;
		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		if(it1 != end(mainObj->iniInfo))
		{
			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
			if(it2 != end(it1->second))
				standr = true;
		}
		//if(standr)
			res.push_back(string("if(").append(negat?"!":"").append(name1).append(".equals(").append(name2).append("))").append(name3).append("\n"));
		//else
		//	res.push_back(string("if(").append(name1).append(negat?" != ":" == ").append(name2).append(")").append(name3).append(";\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		vector<string> res;
		bool standr = false;
		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		if(it1 != end(mainObj->iniInfo))
		{
			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
			if(it2 != end(it1->second))
				standr = true;
		}
		if(standr)
			res.push_back(string("if(!").append(name1).append(".equals(").append(name2).append("))"));
		else
			res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fieldType->getClass()->getName())
			.append("' type '").append(fieldType->getFullTypeString()).append("' are NOT EQUAL\");\n"));
		res.push_back(string("\tresult = false;\n"));
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res; 

		if((channel == "default"))
		{
			res.push_back(string(name).append(".parse(parser);\n"));
			return res;
		}

		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		if(it1 != end(mainObj->iniInfo))
		{
			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
			if(it2 != end(it1->second))
			{
				if (fld->isUsingChannels())
					res.push_back(string(name).append(".parseFrom").append(channel).append("(parser);\n"));
				else
					res.push_back(string(name).append(".parse(parser);\n"));
				return res;
			}
		}
		const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		if(cls)
		{
			auto it4 = cls->getPcChannels().find(channel);
			if(it4 != end(cls->getPcChannels()))
			{
				if(std::get<1>(it4->second))
				{
					if(std::get<0>(it4->second))
					{
						//res.push_back(string("\t\tUINT32 __ce__size;\n"));
						//res.push_back(string("\t\tparser.parseUINT32(__ce__size);\n"));
						vecSmartCorr = true;
						res.push_back(string(name).append(".parseFrom").append(channel).append("(parser, __ce__size);\n"));
						return res;
					}
					else
					{
						res.push_back(string(name).append(".parseFrom").append(channel).append("(parser);\n"));
						return res;
					}
				}
				else
				{
					auto it5 = cls->getPcChannels().find("default");
					if(it5 != end(cls->getPcChannels()))
					{
						res.push_back(string(name).append(".parse(parser);\n"));
						return res;
					}
					else
					{
						throw exception(string("No parseFrom").append(channel).append(" or parse method in class ").append(cls->getName()).append(" used in class ").append(fld->getClassName()).append(" field ").append(fld->getName()).c_str());
					}
				}
			}
			else
			{
				auto it5 = cls->getPcChannels().find("default");
				if(it5 != end(cls->getPcChannels()))
				{
					res.push_back(string(name).append(".parse(parser);\n"));
					return res;
				}
				else
				{
					throw exception(string("No parseFrom").append(channel).append(" or parse method in class ").append(cls->getName()).append(" used in class ").append(fld->getClassName()).append(" field ").append(fld->getName()).c_str());
				}
			}
		}
		if (fld->isUsingChannels())
			res.push_back(string(name).append(".parseFrom").append(channel).append("(parser);\n"));
		else
			res.push_back(string(name).append(".parse(parser);\n"));

		//string chres = "parseFrom"+channel;
		//if((channel == "default"))
		//{
		//	chres = "parse";
		//	res.push_back(string(name).append(".").append(chres).append("(parser);\n"));
		//}
		//else 
		//{
		//	auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		//	if(it1 != end(mainObj->iniInfo))
		//	{
		//		auto it2 = it1->second.find(fieldType->getFieldTypeStr());
		//		if(it2 != end(it1->second))
		//			chres = "parse";
		//	}
		//	if(chres != "parse")
		//	{
		//		auto it3 = mainObj->usedNameOfClass.find(fieldType->getFieldTypeStr());
		//		if(it3 == end(mainObj->usedNameOfClass))
		//			chres = "parse";
		//		else
		//		{
		//			const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		//			if(cls)
		//			{
		//				auto it4 = cls->getPcChannels().find(channel);
		//				if(it4 != end(cls->getPcChannels()))
		//				{
		//					if(!std::get<1>(it4->second))
		//						chres = "parse";
		//				}
		//				else
		//					chres = "parse";
		//			}
		//		}
		//	}
		//	const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		//	if(cls)
		//	{
		//		auto it4 = cls->getPcChannels().find(channel);
		//		if(it4 != end(cls->getPcChannels()))
		//		{
		//			//res.push_back(string("\t\tUINT32 __ce__size;\n"));
		//			//res.push_back(string("\t\tparser.parseUINT32(__ce__size);\n"));

		//			if(std::get<0>(it4->second))
		//				res.push_back(string(name).append(".").append(chres).append("(parser, __ce__size);\n"));
		//		}
		//		else
		//		{
		//			res.push_back(string(name).append(".").append(chres).append("(parser);\n"));
		//		}
		//	}
		//	else
		//	{
		//		res.push_back(string(name).append(".").append(chres).append("(parser);\n"));
		//	}
		//}
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		vector<string> res;
		if((channel == "default"))
		{
			res.push_back(string(name).append(".compose(data);\n"));
			return res;
		}

		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		if(it1 != end(mainObj->iniInfo))
		{
			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
			if(it2 != end(it1->second))
			{
				if (fld->isUsingChannels())
					res.push_back(string(name).append(".composeFor").append(channel).append("(data);\n"));
				else
					res.push_back(string(name).append(".compose(data);\n"));
				return res;
			}
		}

		const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		if(cls)
		{
			auto it4 = cls->getPcChannels().find(channel);
			if(it4 != end(cls->getPcChannels()))
			{
				if(std::get<2>(it4->second))
				{
					if(std::get<0>(it4->second))
					{
						vecSmartCorr = true;
					}
					res.push_back(string(name).append(".composeFor").append(channel).append("(data);\n"));
					return res;
				}
				else
				{
					auto it5 = cls->getPcChannels().find("default");
					if(it5 != end(cls->getPcChannels()))
					{
						res.push_back(string(name).append(".compose(data);\n"));
						return res;
					}
					else
					{
						throw exception(string("No composeFor").append(channel).append(" or compose method in class ").append(cls->getName()).append(" used in class ").append(fld->getClassName()).append(" field ").append(fld->getName()).c_str());
					}
				}
			}
			else
			{
				auto it5 = cls->getPcChannels().find("default");
				if(it5 != end(cls->getPcChannels()))
				{
					res.push_back(string(name).append(".compose(data);\n"));
					return res;
				}
				else
				{
					throw exception(string("No composeFor").append(channel).append(" or compose method in class ").append(cls->getName()).append(" used in class ").append(fld->getClassName()).append(" field ").append(fld->getName()).c_str());
				}
			}
		}
		if(fld->isUsingChannels())
			res.push_back(string(name).append(".composeFor").append(channel).append("(data);\n"));
		else
			res.push_back(string(name).append(".compose(data);\n"));



		//string chres = "composeFor"+channel;
		//if((channel == "default"))
		//	chres = "compose";
		//else 
		//{
		//	auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		//	if(it1 != end(mainObj->iniInfo))
		//	{
		//		auto it2 = it1->second.find(fieldType->getFieldTypeStr());
		//		if(it2 != end(it1->second))
		//			chres = "compose";
		//	}
		//	if(chres != "compose")
		//	{
		//		auto it3 = mainObj->usedNameOfClass.find(fieldType->getFieldTypeStr());
		//		if(it3 == end(mainObj->usedNameOfClass))
		//			chres = "compose";
		//		else
		//		{
		//			const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		//			if(cls)
		//			{
		//				auto it4 = cls->getPcChannels().find(channel);
		//				if(it4 != end(cls->getPcChannels()))
		//				{
		//					//res.push_back(string("\t\tsize_t __ce__shift=0;\n"));
		//					//res.push_back(string("\t\tsize_t __ce__size=0;\n"));
		//					//res.push_back(string("\t\tdata.composeVectorSize(0,__ce__shift);\n"));
		//					//res.push_back(string("\t\t__ce__size = data._size();\n"));
		//					if(!std::get<2>(it4->second))
		//						chres = "compose";
		//					//res.push_back(string("\t\tdata.updateVectorSize( data._size() - __ce__size, __ce__shift);\n"));
		//				}
		//				else
		//					chres = "compose";
		//			}
		//		}
		//	}
		//}
		//res.push_back(string(name).append(".").append(chres).append("(data);\n"));
		return res;
	}

	vector<string> fillSelConvertCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		vector<string> res;

		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
		if(it1 != end(mainObj->iniInfo))
		{
			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
			if(it2 != end(it1->second))
			{
				res.push_back(string(name).append(".compose(data);\n"));
				return res;
			}
		}

		res.push_back(string(name).append(".selConvertCompose(data);\n"));
		return res;
	}


	vector<string> fillFieldPublisher(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter) override 
	{
		vector<string> res;
		res.push_back(string(name).append(".publish(pp").append(to_string(counter)).append(");\n"));
		return res;
	}
	vector<string> fillFieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		vector<string> res, res1;

		if(fld->getProperty() & CG_CLASS_SPECIAL_1)
		{
			//res1 = pathAdd(lnidx, counter, 0);
			//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
			res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));

			res.push_back(string(name1).append(".publisher_special1(newValue, pp);\n"));
		}
		else
		{
			if(fieldType->isArray())
			{
				res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));
				res.push_back(string("\tpp.addValue(0);\n"));
				res.push_back(string("for(size_t i=0; i<").append(fieldType->getArraySizeString()).append("; ++i)\n"));
				res.push_back(string("{\n"));
				res.push_back(string("\tif(").append(name1).append("[i] != ").append(name2).append("[i])\n"));
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\t").append(name1).append("[i] = ").append(name2).append("[i];\n"));

				//res1 = pathAdd(lnidx, counter, 2, 2);
				//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
				res.push_back(string("\tpp.changeLastValue(i);\n"));

				//res.push_back(string("\t\tpath").append(to_string(counter+1)).append("[pathLen").append(to_string(counter)).append("+1] = i").append(to_string(counter)).append(";\n"));

				res1 = fieldSubscrUpdate(fld, fieldType, mainObj, lnidx, name2+"[i]", name1+"[i]");
				for(auto elem : res1)
				{
					res.push_back(string("\t\t").append(move(elem)));
				}

				res.push_back(string("\t}\n"));
				res.push_back(string("}\n"));
			}
			else
			{
				//res1 = pathAdd(lnidx, counter, 0);
				//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
				res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));

				res1 = fieldSubscrUpdate(fld, fieldType, mainObj, lnidx, name2, name1);
				for(auto elem : res1)
				{
					res.push_back(string("").append(move(elem)));
				}
			}
		}
		return res;
	}
	vector<string> fillFieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		throw exception(string("Cannot use subscription add to the object field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append("'\n").c_str());
	}
	vector<string> fillFieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override 
	{
		throw exception(string("Cannot use subscription remove to the object field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append("'\n").c_str());
	}

	bool isLeaf(void) override
	{
		return false;
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
		vector<string> res;
		res.push_back(string("").append(name2).append(".publisherUpdate").append("(").append(name1).append(", pp);\n"));
		return res;
	}
	vector<string> fieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res;
		if(lnidx.length())
		{
			res.push_back(string("int idx = ").append(name2).append(".publisherAdd").append("(pp);\n"));
			res.push_back(string("if(idx != ").append(lnidx).append(") throw PError(\"")
				.append(fld->getClassName()).append(" field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));
		}
		else
			res.push_back(string("").append(name2).append(".publisherAdd").append("(pp);\n"));
		return res;
	}
	vector<string> fieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res;
		res.push_back(string("publisher.removeNode(pp,&update);\n"));
		//res.push_back(string("").append(name2).append(".publisherRemove").append("(path")
		//	.append(to_string(counter+1)).append(", pathLen").append(to_string(counter+1)).append(");\n"));
		return res;
	}
	vector<string> fillSelFields(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj) override
	{
		vector<string> res, res1;
		bool dum = false;

		res.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
		//res.push_back(string("{\n"));

		res.push_back(string("\t{\n"));
		const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		if(cls)
		{
			res.push_back(string("\t\tdata.composeUINT32(1);\n"));

			res.push_back(string("\t\t").append(fld->getName()).append(".composeSelectiveFieldsCounted(path, data, path_pointer, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));

			//res.push_back(string("\telse\n"));
			//res.push_back(string("\t{\n"));

			//res.push_back(string("\t}\n"));
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

	virtual bool isOurObject(const CGfieldtype* fieldType, CGmain* mainObj)
	{
		const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
		if(cls)
			return true;
		else
			return false;
	}


	//virtual string fillDefaultConctructorHeader(const CGfield* fld) override
	//{
	//	return "";
	//}
	//virtual vector<string> fillDefaultConctructorBody(const CGfield* fld) override
	//{
	//	return vector<string>();
	//}
	//virtual vector<string> fillDestructor(const CGfield* fld) override
	//{
	//	return vector<string>();
	//}
	//virtual string fillCopyConctructorHeader(const CGfield* fld) override
	//{
	//	return string(fld->getName()).append("(other.").append(fld->getName()).append(")\n");
	//}
	//virtual vector<string> fillCopyConctructorBody(const CGfield* fld) override
	//{
	//	return vector<string>();
	//}
	//virtual string fillMoveConctructorHeader(const CGfield* fld) override
	//{
	//	return string(fld->getName()).append("(move(other.").append(fld->getName()).append("))\n");
	//}
	//virtual vector<string> fillMoveConctructorBody(const CGfield* fld) override
	//{
	//	return vector<string>();
	//}
	//virtual vector<string> fillAssignment(const CGfield* fld, const string& name1, const string& name2) override
	//{
	//	vector<string> res;
	//	res.push_back(string(name1).append(" = ").append(name2).append(";\n"));
	//	return res;
	//}
	//virtual vector<string> fillMoveAssignment(const CGfield* fld, const string& name1, const string& name2) override
	//{
	//	vector<string> res;
	//	res.push_back(string(name1).append(" = move(").append(name2).append(");\n"));
	//	return res;
	//}
	//virtual vector<string> fillComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	//{
	//	vector<string> res;
	//	bool standr = false;
	//	auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
	//	if(it1 != end(mainObj->iniInfo))
	//	{
	//		auto it2 = it1->second.find(fieldType->getFieldTypeStr());
	//		if(it2 != end(it1->second))
	//			standr = true;
	//	}
	//	if(standr)
	//		res.push_back(string("if(").append(negat?"!":"").append(name1).append(".equals(").append(name2).append("))").append(name3).append(";\n"));
	//	else
	//		res.push_back(string("if(").append(name1).append(negat?" != ":" == ").append(name2).append(")").append(name3).append(";\n"));
	//	return res;
	//}
	//virtual vector<string> fillComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	//{
	//	vector<string> res;
	//	bool standr = false;
	//	auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
	//	if(it1 != end(mainObj->iniInfo))
	//	{
	//		auto it2 = it1->second.find(fieldType->getFieldTypeStr());
	//		if(it2 != end(it1->second))
	//			standr = true;
	//	}
	//	if(standr)
	//		res.push_back(string("if(!").append(name1).append(".equals(").append(name2).append("))"));
	//	else
	//		res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
	//	res.push_back(string("{\n"));
	//	res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fieldType->getClass()->getName())
	//		.append("' type '").append(fieldType->getFullTypeString()).append("' are NOT EQUAL\");\n"));
	//	res.push_back(string("\tresult = false;\n"));
	//	res.push_back(string("}\n"));
	//	return res;
	//}
	//virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain*	mainObj) override
	//{
	//	vector<string> res; 
	//	string chres = "parseFrom_"+channel;
	//	if((channel == "default"))
	//		chres = "parse";
	//	else 
	//	{
	//		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
	//		if(it1 != end(mainObj->iniInfo))
	//		{
	//			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
	//			if(it2 != end(it1->second))
	//				chres = "parse";
	//		}
	//		if(chres != "parse")
	//		{
	//			auto it3 = mainObj->usedNameOfClass.find(fieldType->getFieldTypeStr());
	//			if(it3 == end(mainObj->usedNameOfClass))
	//				chres = "parse";
	//			else
	//			{
	//				const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
	//				if(cls)
	//				{
	//					auto it4 = cls->getPcChannels().find(channel);
	//					if(it4 != end(cls->getPcChannels()))
	//					{
	//						if(!std::get<1>(it4->second))
	//							chres = "parse";
	//					}
	//					else
	//						chres = "parse";
	//				}
	//			}
	//		}
	//	}
	//	res.push_back(string(name).append(".").append(chres).append("(parser);\n"));
	//	return res;
	//}
	//virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
	//{
	//	vector<string> res;
	//	string chres = "composeFor_"+channel;
	//	if((channel == "default"))
	//		chres = "compose";
	//	else 
	//	{
	//		auto it1 = mainObj->iniInfo.find("STANDARD_OBJECTS");
	//		if(it1 != end(mainObj->iniInfo))
	//		{
	//			auto it2 = it1->second.find(fieldType->getFieldTypeStr());
	//			if(it2 != end(it1->second))
	//				chres = "compose";
	//		}
	//		if(chres != "compose")
	//		{
	//			auto it3 = mainObj->usedNameOfClass.find(fieldType->getFieldTypeStr());
	//			if(it3 == end(mainObj->usedNameOfClass))
	//				chres = "compose";
	//			else
	//			{
	//				const CGstruct* cls = dynamic_cast<const CGstruct*>(mainObj->findObject(objTypes::gTypeStruct, fieldType->getFieldTypeStr()));
	//				if(cls)
	//				{
	//					auto it4 = cls->getPcChannels().find(channel);
	//					if(it4 != end(cls->getPcChannels()))
	//					{
	//						if(!std::get<2>(it4->second))
	//							chres = "compose";
	//					}
	//					else
	//						chres = "compose";
	//				}
	//			}
	//		}
	//	}
	//	res.push_back(string(name).append(".").append(chres).append("(data);\n"));
	//	return res;
	//}
	//virtual vector<string> fillFieldPublisher(const CGfield* fld, CGfieldtype* fieldType, CGmain* mainObj, const vector<string>& path, const string& lnidx, int counter) override
	//{
	//	vector<string> res, res1;

	//	res.push_back(string(fld->getName()).append(".publish(path").append(to_string(counter)).append(", pathLen").append(to_string(counter)).append(");\n"));

	//	//unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getFieldTypeStr());
	//	//res1 = bcls->fillPublisher(fld, fieldType, mainObj, path, lnidx, fld->getName(), counter);
	//	//res.insert(res.end(), res1.begin(), res1.end());
	//	return res;
	//}
	//virtual bool isLeaf(void) {return false;}


	//virtual vector<string> fillSubscrUpdate(const CGfield* fld, CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, int counter)
	//{
	//	vector<string> res, res1;
	//	res.push_back(string("void publisher_").append(fld->getName()).append("(UINT32 newValue)\n"));
	//	res.push_back(string("{\n"));
	//	res.push_back(string("\tif(published)\n"));
	//	res.push_back(string("\t{\n"));
	//	res.push_back(string("\t\tpublisher_").append(fld->getName()).append("(newValue, 0, 0);\n"));
	//	res.push_back(string("\t}\n"));
	//	res.push_back(string("}\n"));


	//	res.push_back(string("void publisher_").append(fld->getName()).append("(UINT32 newValue, const int* path")
	//		.append(to_string(counter)).append(", int pathLen").append(to_string(counter)).append(");\n"));
	//	res.push_back(string("{\n"));
	//	unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getFieldTypeStr());
	//	res1 = bcls->fillFieldSubscrUpdate(fld, fieldType, mainObj, lnidx, fld->getName(), counter);
	//	for(auto elem : res1)
	//	{
	//		res.push_back(string("\t").append(move(elem)));
	//	}
	//	res.push_back(string("}\n"));
	//	return res;
	//}
	//virtual vector<string> fillFieldSubscrUpdate(const CGfield* fld, CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter)
	//{
	//	vector<string> res, res1;
	//	if(fld->getProperty() & CG_CLASS_SPECIAL_1)
	//	{
	//		res.push_back(string("int pathLen").append(to_string(counter+1)).append(" = pathLen").append(to_string(counter)).append("+1;\n"));
	//		res.push_back(string("unique_ptr<int> u_path").append(to_string(counter+1)).append("(new int[pathLen").append(to_string(counter+1)).append("]);\n"));
	//		res.push_back(string("int* path").append(to_string(counter+1)).append(" = u_path").append(to_string(counter+1)).append(".get();\n"));
	//		res.push_back(string("if(pathLen").append(to_string(counter)).append(") memcpy(path").append(to_string(counter+1))
	//			.append(", path").append(to_string(counter)).append(", pathLen").append(to_string(counter)).append("*sizeof(int));\n"));
	//		res.push_back(string("path").append(to_string(counter+1)).append("[pathLen").append(to_string(counter)).append("] = ").append(lnidx).append(";\n"));
	//		res.push_back(string(name).append(".publisher_special1(newValue, path")
	//			.append(to_string(counter+1)).append(", pathLen").append(to_string(counter+1)).append(");\n"));

	//	}

	//	return res;
	//}

};

struct CGGuint32mapsetcls : public CGGobjcls
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

		res.append("[\\n\");\n");
		res.append(res1).append("for(auto it").append(to_string(counter)).append(" = ").append(name2).append(".begin(); it").append(to_string(counter)).append(" != ").append(name2).append(".end();)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\tauto& elem").append(to_string(counter)).append(" = *it").append(to_string(counter)).append(";\n");
		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("  {\\n\");\n");

		//auto resInt = bcls1->fillFieldJson(fieldType->getZeroSubType(), "first", string("elem") + (to_string(counter)) + ".first", margin1 + 1, margin2 + 2, counter + 1, false);
		//res.append(resInt);
		//resInt = bcls2->fillFieldJson(fieldType->getZeroSubType(), "second", string("elem") + (to_string(counter)) + ".second", margin1 + 1, margin2 + 2, counter + 1, true);
		//res.append(resInt);
		//res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("    \\\"value\\\": \").appendInt(it").append(to_string(counter)).append("->value).append(\"\\n\");\n");
		res.append("// TO DO\n");

		res.append(res1).append("\tif(++it").append(to_string(counter)).append(" != ").append(name2).append(".end())\n");
		res.append(res1).append("\t{\n");

		res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  },\\n\");\n");


		res.append(res1).append("\t}\n");
		res.append(res1).append("\telse\n");
		res.append(res1).append("\t{\n");

		res.append(res1).append("\t\toutput.append(margin_string).append(\"").append(res2).append("  }\\n\");\n");

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
		//unique_ptr<CGGbasecls> bcls1 = CGGbasecls::getBaseCls(fieldType->getSubTypes()[0]->getFieldTypeStr());
		//unique_ptr<CGGbasecls> bcls2 = CGGbasecls::getBaseCls(fieldType->getSubTypes()[1]->getFieldTypeStr());
		//for (int i = 0; i < margin; ++i)
		//{
		//	res1.append("\t");
		//}

		//res.append(res1).append("for(auto& elem").append(to_string(counter + 1)).append(" : elem").append(to_string(counter)).append("->v)\n");
		//res.append(res1).append("{\n");

		////res.append(res1).append("\t").append(fieldType->getSubTypes()[0]->getFieldTypeStr()).append(" first").append(to_string(counter + 1)).append(";\n");
		//res.append(res1).append("\tfor(size_t i=0; ((i<2)&&(i<(elem").append(to_string(counter + 1)).append("->v).size())); ++i)\n");
		//res.append(res1).append("\t{\n");
		//res.append(res1).append("\t\t").append(fieldType->getSubTypes()[0]->getFieldTypeStr()).append(" first").append(to_string(counter + 1)).append(";\n");
		//res.append(res1).append("\t\t").append(fieldType->getSubTypes()[1]->getFieldTypeStr()).append(" second").append(to_string(counter + 1)).append(";\n");

		//res.append(res1).append("\t\tfor(auto& elem").append(to_string(counter + 2)).append(" : elem").append(to_string(counter + 1)).append("->v)\n");
		//res.append(res1).append("\t\t{\n");
		//res.append(res1).append("\t\t\tif(elem").append(to_string(counter + 2)).append("->name.equals(\"first\"))\n");
		//res.append(res1).append("\t\t\t{\n");

		//auto resInt = bcls1->getFieldJson(fld, fieldType->getSubTypes()[0].get(), string("first") + to_string(counter + 1), margin + 4, counter + 2);
		//res.append(resInt);

		//res.append(res1).append("\t\t\t}\n");
		//res.append(res1).append("\t\t\telse if(elem").append(to_string(counter + 2)).append("->name.equals(\"second\"))\n");
		//res.append(res1).append("\t\t\t{\n");

		//resInt = bcls2->getFieldJson(fld, fieldType->getSubTypes()[1].get(), string("second") + to_string(counter + 1), margin + 4, counter + 2);
		//res.append(resInt);

		//res.append(res1).append("\t\t\t}\n");
		//res.append(res1).append("\t\t}\n");
		//res.append(res1).append("\t\t").append(name).append(".emplace(move(first").append(to_string(counter + 1)).append("), move(second").append(to_string(counter + 1)).append("));\n");

		//res.append(res1).append("\t}\n");
		//res.append(res1).append("}\n");

		res.append("// TO DO\n");
		return res;
	}

};


#endif	// CGGobjcls_h_included
