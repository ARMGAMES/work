// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGbasecls_h_included
#define CGGbasecls_h_included

//******************************************************************************
// Base class for text generators
//******************************************************************************
struct CGGbasecls
{
	virtual string fillFieldJson(const CGfieldtype* fieldType, const string& name1, const string& name2, int margin1, int margin2, int counter, bool last) 
	{ 
		return string(); 
	}
	virtual string getFieldJson(const CGfield* fld, const CGfieldtype* fieldType, const string& name, int margin, int counter)
	{
		return string();
	}

	virtual vector<string> fillFieldAssignment(const CGfield* fld, const string& name1, const string& name2) abstract;
	virtual vector<string> fillFieldMoveAssignment(const CGfield* fld, const string& name1, const string& name2) abstract;
	virtual vector<string> fillFieldNull(const CGfield* fld, const string& name) abstract;

	virtual vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) abstract;
	virtual vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) abstract;

	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) abstract;
	virtual vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) abstract;
	virtual vector<string> fillSelConvertCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj)
	{
		bool vecSmartCorr;
		return fillCompose(fld, fieldType, fld->getName(), "default", mainObj, false, vecSmartCorr);
	}

	virtual vector<string> fillFieldPublisher(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter) abstract; 
	virtual vector<string> fillFieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) abstract; 
	virtual vector<string> fillFieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) abstract; 
	virtual vector<string> fillFieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) abstract; 

	virtual bool isLeaf(void) abstract;
	virtual bool isSubscriptionUpdate(void) abstract;
	virtual bool isSubscriptionAdd(void) abstract;
	virtual bool isSubscriptionRemove(void) abstract;

	virtual vector<string> fieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) abstract; 
	virtual vector<string> fieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) abstract; 
	virtual vector<string> fieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) abstract; 

	virtual vector<string> fillUpdateInternal(const CGfield* fld, const string& name, CGmain* mainObj)
	{
		bool vecSmartCorr;
		vector<string> res, res1;
		if(fld->isArray())
		{
			res = fillParse(fld, fld->getFieldType(), name, "default", mainObj, true, vecSmartCorr);
			res.push_back(string("fieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
		}
		else
		{
			res.push_back(string("").append(fld->getFieldTypeStr()).append(" tmp_").append(name).append(";\n"));
			res1 = fillParse(fld, fld->getFieldType(), string("tmp_")+name, "default", mainObj, false, vecSmartCorr);
			for (auto elem : res1)
			{
				res.push_back(string("").append(move(elem)));
			}
			res1 = fillFieldComparator(1, fld, fld->getFieldType(), string("tmp_")+name, name, "", true, mainObj);
			for (auto elem : res1)
			{
				res.push_back(string("").append(move(elem)));
			}
			res.push_back(string("{\n"));

			res.push_back(string("\t").append(fld->getName()).append(" = move(tmp_").append(fld->getName()).append(");\n"));
			res.push_back(string("\tfieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));

			res.push_back(string("}\n"));
		}
		return res;
	}

	virtual vector<string> fillParseSmart(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr)
	{
		return fillParse(fld, fieldType, name, channel, mainObj, isArray, vecSmartCorr);
	}
	virtual vector<string> fillComposeSmart(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr)
	{
		return fillCompose(fld, fieldType, name, channel, mainObj, isArray, vecSmartCorr);
	}


	virtual vector<string> fillSelFields(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj)//abstract; 
	{
		vector<string> res, res1;

		res.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tPASSERT3(0);\n"));
		res.push_back(string("}\n"));
		return res;
	}

	virtual bool isOurObject(const CGfieldtype* fieldType, CGmain* mainObj)
	{
		return false;
	}

	virtual bool isSimpleString(void)
	{
		return false;
	}

	virtual bool isSimpleNumber(void)
	{
		return false;
	}

	virtual bool isTimeData(void)
	{
		return false;
	}

	virtual string fillDefaultConctructorHeader(const CGfield* fld)
	{
		return string();
	}
	vector<string> fillDefaultConctructorBody(const CGfield* fld)
	{
		//if(!fld->isArray())
			return fillFieldNull(fld, fld->getName());
		//else
		//	return vector<string>();
	}
	virtual string fillCopyConctructorHeader(const CGfield* fld)
	{
		return string();
	}
	virtual vector<string> fillCopyConctructorBody(const CGfield* fld)
	{
		return fillAssignment(fld, fld->getName(), string("other.")+fld->getName());
	}
	virtual string fillMoveConctructorHeader(const CGfield* fld)
	{
		return string();
	}
	virtual vector<string> fillMoveConctructorBody(const CGfield* fld)
	{
		return fillMoveAssignment(fld, fld->getName(), string("other.")+fld->getName());
	}

	virtual vector<string> fillGetters(const CGfield* fld)
	{
		vector<string> res;
		string s;
		
		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = "& ";
		res.push_back(string("const ").append(fld->getFullTypeString()).append(s).append(fld->getGetter()).append("(void) const {return ").append(fld->getName()).append(";}\n"));
		return res;
	}
	virtual vector<string> fillSetters(const CGfield* fld)
	{
		vector<string> res, res1;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = "& ";
		res.push_back(string("void ").append(fld->getSetter()).append("(const ").append(fld->getFullTypeString()).append(s).append(fld->getName()).append("_data)\n"));
		res.push_back(string("{\n"));
		res1 = fillAssignment(fld, fld->getName(), fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillMoveSetters(const CGfield* fld)
	{
		vector<string> res, res1;
		string s;

		if(fld->isPointer() || fld->isArray())
			s = "* ";
		else
			s = "& ";
		res.push_back(string("void ").append(fld->getSetter()).append("(").append(fld->getFullTypeString()).append(s).append(fld->getName()).append("_data)\n"));
		res.push_back(string("{\n"));
		res1 = fillMoveAssignment(fld, fld->getName(), fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}

	vector<string> fillAssignment(const CGfield* fld, const string& name1, const string& name2)
	{
		vector<string> res, res1;

		if(fld->isArray())
		{
			res.push_back(string("for(size_t i=0; i<").append(fld->getArraySizeString()).append("; ++i)\n"));
			res.push_back(string("{\n"));
			res1 = fillFieldAssignment(fld, fld->getName()+"[i]", string("other.")+fld->getName()+"[i]");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}
			res.push_back(string("}\n"));
		}
		else
		{
			return fillFieldAssignment(fld, name1, name2);
		}
		return res;
	}
	vector<string> fillMoveAssignment(const CGfield* fld, const string& name1, const string& name2)
	{
		vector<string> res, res1;

		if(fld->isArray())
		{
			res.push_back(string("for(size_t i=0; i<").append(fld->getArraySizeString()).append("; ++i)\n"));
			res.push_back(string("{\n"));
			res1 = fillFieldMoveAssignment(fld, fld->getName()+"[i]", string("other.")+fld->getName()+"[i]");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}
			res.push_back(string("}\n"));
		}
		else
		{
			return fillFieldMoveAssignment(fld, name1, name2);
		}
		return res;
	}

	vector<string> fillEqual(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj)
	{
		vector<string> res, res1;
		if (fld->getProperty() & CG_CLASS_EQUAL_LOG)
		{
			if(fld->isArray())
			{
				string index = "i"+to_string(counter);
				res.push_back(string("for(size_t ").append(index).append(" = 0; ").append(index).append(" < ").append(to_string(fieldType->getArraySize())).append("; ++").append(index).append(")\n"));
				res.push_back(string("{\n"));
				res1 = fillFieldComparator(counter+1, fld, fieldType, name1+"["+index+"]", name2+"["+index+"]", name3, negat, mainObj);
				for(auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}
				res.push_back(string("}\n"));
			}
			else if(fld->isPointer())
			{
				res.push_back(string("if(").append(name1).append(" != ").append(name2).append(")\n"));
				res.push_back(string("{\n"));
				res.push_back(string("\tif(!").append(name1).append(" || !").append(name2).append(")\n"));
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fieldType->getClass()->getName())
					.append("' type '").append(fieldType->getFullTypeString()).append("' are NOT EQUAL\");\n"));
				res.push_back(string("\t\tresult = false;\n"));
				res.push_back(string("\t}\n"));
				res1 = fillFieldComparatorLog(counter, fld, fieldType, string("(*")+name1+")", string("(*(")+name2+"))", mainObj);
				for(auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}
				res.push_back(string("}\n"));
			}
			else
				return fillFieldComparatorLog(1, fld, fieldType, name1, name2, mainObj);
		}
		else
		{
			if(fld->isArray())
			{
				string index = "i"+to_string(counter);
				res.push_back(string("for(size_t ").append(index).append(" = 0; ").append(index).append(" < ").append(fld->getArraySizeString()).append("; ++").append(index).append(")\n"));
				res.push_back(string("{\n"));
				res1 = fillFieldComparator(counter+1, fld, fieldType, name1+"["+index+"]", name2+"["+index+"]", name3, negat, mainObj);
				for(auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}
				res.push_back(string("}\n"));
			}
			else if(fld->isPointer())
			{
				res.push_back(string("if(").append(fld->getName()).append(" != ").append("other.").append(fld->getName()).append(")\n"));
				res.push_back(string("{\n"));
				res.push_back(string("\tif(").append(negat?"!":"").append(name1).append(" || !").append(name2).append(")").append(name3).append(";\n"));

				res1 = fillFieldComparator(counter, fld, fieldType, string("(*")+name1+")", string("(*(")+name2+"))", name3, negat, mainObj);
				for(auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}
				res.push_back(string("}\n"));
			}
			else
				return fillFieldComparator(1, fld, fieldType, name1, name2, name3, negat, mainObj);
		}
		return res;
	}

	vector<string> fillPublisher(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name, int counter)
	{
		vector<string> res, res1;
		bool dum = false;
		string ppExt, ppInt;
		ppExt = "pp"+to_string(counter);
		ppInt = "pp"+to_string(counter+1);
		if(isLeaf())
		{
			if(fld->isArray())
			{
				res.push_back(string("{\n"));
				res.push_back(string("\tPath ").append(ppInt).append(" = ").append(ppExt).append(";\n"));
				res.push_back(string("\tint idxNode = publisher.appendNode(").append(ppInt).append(".path,").append(ppInt).append(".pathLen,0);\n"));
				res.push_back(string("\tif(idxNode != ").append(lnidx).append(") throw PError(\"")
					.append(fld->getClassName()).append("::publish: field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));

				//res1 = pathAdd(lnidx, counter, 1);
				//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));

				res.push_back(string("\t").append(ppInt).append(".addValue(").append(lnidx).append(");\n"));

				res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n"));
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\tCommMsgBody data;\n"));
				res1 = fillCompose(fld, fieldType, name+"[i]", "default", mainObj, false, dum);
				for(auto elem : res1)
				{
					res.push_back(string("\t\t").append(move(elem)));
				}
				//res.push_back(string("\t\tint idx = publisher.appendLeaf(pp.path").append(",pp.pathLen").append(",data,0);\n"));
				res.push_back(string("\tint idx = publisher.appendLeaf(").append(ppInt).append(".path,").append(ppInt).append(".pathLen,data,0);\n"));
				res.push_back(string("\t}\n"));
				res.push_back(string("}\n"));

			}
			else
			{
				res.push_back(string("{\n"));
				//res.push_back(string("\tPath ").append(ppInt).append(" = ").append(ppExt).append(";\n"));
				res.push_back(string("\tCommMsgBody data;\n"));
				if(fld->isPointer())
					res1 = fillCompose(fld, fieldType, string("(*")+name+")", "default", mainObj, false, dum);
				else
					res1 = fillCompose(fld, fieldType, name, "default", mainObj, false, dum);
				for(auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}
				res.push_back(string("\tint idx = publisher.appendLeaf(").append(ppExt).append(".path,").append(ppExt).append(".pathLen,data,0);\n"));
				res.push_back(string("\tif(idx != ").append(lnidx).append(") throw PError(\"")
					.append(fld->getClassName()).append("::publish: field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));
				res.push_back(string("}\n"));
			}
		}
		else
		{
			res.push_back(string("{\n"));
			res.push_back(string("\tPath ").append(ppInt).append(" = ").append(ppExt).append(";\n"));
			res.push_back(string("\tint idxNode = publisher.appendNode(").append(ppInt).append(".path,").append(ppInt).append(".pathLen,0);\n"));
			res.push_back(string("\tif(idxNode").append(" != ").append(lnidx).append(") throw PError(\"")
				.append(fld->getClassName()).append("::publish: field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));

			//res1 = pathAdd(lnidx, counter, 1);
			//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
			res.push_back(string("\t").append(ppInt).append(".addValue(").append(lnidx).append(");\n"));

			if(fld->isArray())
			{
				res.push_back(string("\t").append(ppInt).append("\t.addValue(0);\n"));
				//res.push_back(string("\tpp.addValue(0);\n"));
				res.push_back(string("\tfor(size_t i=0; i<").append(to_string(fieldType->getArraySize())).append("; ++i)\n"));
				//res1 = fillPublisher(fld, fieldType, mainObj, "i", fld->getName()+"[i]", counter);
				res.push_back(string("\t{\n"));
				res.push_back(string("\tint idxNode = publisher.appendNode(").append(ppInt).append(".path,").append(ppInt).append(".pathLen,0);\n"));
				res.push_back(string("\t\tif(idxNode").append(" != ").append(lnidx).append(") throw PError(\"")
					.append(fld->getClassName()).append("::publish: field:").append(fld->getName()).append(" wrong leaf/node index\");\n"));

				//res1 = pathAdd("i", counter+1, 2);
				//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
				res.push_back(string("\t").append(ppInt).append(".changeLastValue(i);"));

				res1 = fillFieldPublisher(fld, fieldType, mainObj, "0", name+"[i]", counter+1);
				for(auto elem : res1)
				{
					res.push_back(string("\t\t").append(move(elem)));
				}
				res.push_back(string("\t}\n"));
			}
			else
			{
				res1 = fillFieldPublisher(fld, fieldType, mainObj, "0", name, counter+1);
				for(auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}
			}
			res.push_back(string("}\n"));
		}
		return res;
	}

	vector<string> fillSubscrUpdateAll(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name)
	{
		vector<string> res;
		if(isSubscriptionUpdate() && !(fld->getProperty() & CG_CLASS_SPECIAL_1))
		{
			res.push_back(string("publisherUpdate_").append(fld->getName()).append("(other.").append(fld->getName()).append(", pp);\n"));
		}
		return res;
	}
	vector<string> fillSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx)
	{
		vector<string> res, res1;
		if(isSubscriptionUpdate())
		{
			string s;
			if(fld->isPointer() || fld->isArray())
				s = "* ";
			else
				s = "& ";
			if(fld->getProperty() & CG_CLASS_SPECIAL_1)
			{
				res.push_back(string("void publisherUpdate_").append(fld->getName()).append("(UINT32 newValue, Path pp)\n"));
			}
			else
			{
				res.push_back(string("void publisherUpdate_").append(fld->getName()).append("(const ").append(fld->getFieldTypeStr()).append(s)
					.append(fld->getName()).append("_data, Path pp)\n"));
			}
			res.push_back(string("{\n"));
			res.push_back(string("\tif(!published) return;\n"));
			res1 = fillFieldSubscrUpdate(fld, fieldType, mainObj, lnidx, fld->getName(), fld->getName()+"_data");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}
			res.push_back(string("}\n"));
		}
		return res;
	}
	vector<string> fillSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx)
	{
		vector<string> res, res1;
		if(isSubscriptionAdd())
		{
			res.push_back(string("void publisherAdd_").append(fld->getName()).append("(const ").append(fieldType->getZeroSubType()->getFieldTypeStr()).append("& ")
				.append(fld->getName()).append("_data, Path pp)\n"));
			res.push_back(string("{\n"));
			res.push_back(string("\tif(!published) return;\n"));
			res1 = fillFieldSubscrAdd(fld, fieldType, mainObj, lnidx, fld->getName(), fld->getName()+"_data");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}
			res.push_back(string("}\n"));
		}
		return res;
	}
	vector<string> fillSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx)
	{
		vector<string> res, res1;
		if(isSubscriptionRemove())
		{
			res.push_back(string("void publisherRemove_").append(fld->getName()).append("(const ").append(fieldType->getZeroSubType()->getFieldTypeStr()).append("& ")
				.append(fld->getName()).append("_data, Path pp)\n"));
			res.push_back(string("{\n"));
			res.push_back(string("\tif(!published) return;\n"));
			res1 = fillFieldSubscrRemove(fld, fieldType, mainObj, lnidx,fld->getName(), fld->getName()+"_data");
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}
			res.push_back(string("}\n"));
		}
		return res;
	}

	vector<string> fillUpdate(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res, res1;

		res.push_back(string("if(PString::compareIgnoreCase(\"").append(fld->getName()).append("\", fieldsUpdateType) == 0)\n"));
		res.push_back(string("{\n"));

		if((fld->getProperty() & CG_IGNORE_UPDATE_ON) || (fld->getProperty() & CG_AUTO_UPDATE_MASK))
		{
			res.push_back(string("\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' cannot be updated - set ignored or auto\");\n"));
		}
		else
		{
			auto secCodes = fld->getUpdateSecCodes();
			if (!fld->getSecUpdateAdd() && secCodes.size())
			{
				for (auto &i : secCodes)
				{
					//fld->getClass()->addUpdateHandlerSec(fld->getUpdateSecurityFunc());
					res.push_back(string("\tif(").append(fld->getUpdateSecurityFunc()).append("(").append(to_string(i)).append(", this, param1, secure_param)); else\n"));
				}
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' update rights check false\");\n"));
				res.push_back(string("\t\treturn false;\n"));
				res.push_back(string("\t}\n"));

			}

			if (fld->getUpdateHandler().size())
			{
				fld->getClass()->addUpdateHandler(fld->getUpdateHandler());
				res.push_back(string("\tif(!").append(fld->getUpdateHandler()).append("(\"").append(fld->getName()).append("\", parser, \"").append(fld->getClassName()).append("\", this, param1, param2))\n"));
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' : '").append(fld->getUpdateHandler()).append("' returns false\");\n"));
				res.push_back(string("\t\treturn false;\n"));
				res.push_back(string("\t}\n"));
				res.push_back(string("\tfieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
			}
			else
			{
				res1 = fillUpdateInternal(fld, fld->getName(), mainObj);
				for (auto elem : res1)
				{
					res.push_back(string("\t").append(move(elem)));
				}

			}

			auto secCodesAfter = fld->getUpdateSecCodesAfter();
			if (secCodesAfter.size())
			{
				for (auto &i : secCodesAfter)
				{
					//fld->getClass()->addUpdateHandlerSec(fld->getUpdateSecurityFuncAfter());
					res.push_back(string("\tif(").append(fld->getUpdateSecurityFuncAfter()).append("(").append(to_string(i)).append(", this, param1, secure_param)); else\n"));
				}
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' update rights check false\");\n"));
				res.push_back(string("\t\treturn false;\n"));
				res.push_back(string("\t}\n"));

			}
		}

		res.push_back(string("}\n"));
		res.push_back(string("else\n"));
		return res;
	}

	vector<string> fillSecUpdateAdd(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res, res1;

		auto secCodes = fld->getUpdateSecCodes();
		if(fld->getSecUpdateAdd() && secCodes.size())
		{
			res.push_back(string("if(PString::compareIgnoreCase(\"").append(fld->getName()).append("\", fieldsUpdateType) == 0)\n"));
			res.push_back(string("{\n"));

			if ((fld->getProperty() & CG_IGNORE_UPDATE_ON) || (fld->getProperty() & CG_AUTO_UPDATE_MASK))
			{
				res.push_back(string("\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' cannot be updated - set ignored or auto\");\n"));
			}
			else
			{
				for (auto &i : secCodes)
				{
					res.push_back(string("\tif(").append(fld->getUpdateSecurityFunc()).append("(").append(to_string(i)).append(", this, param1, secure_param)); else\n"));
				}
				res.push_back(string("\t{\n"));
				res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' update rights check false\");\n"));
				res.push_back(string("\t\treturn false;\n"));
				res.push_back(string("\t}\n"));

				if (fld->getUpdateHandler().size())
				{
					fld->getClass()->addUpdateHandler(fld->getUpdateHandler());
					res.push_back(string("\tif(!").append(fld->getUpdateHandler()).append("(\"").append(fld->getName()).append("\", parser, \"").append(fld->getClassName()).append("\", this, param1, param2))\n"));
					res.push_back(string("\t{\n"));
					res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' : '").append(fld->getUpdateHandler()).append("' returns false\");\n"));
					res.push_back(string("\t\treturn false;\n"));
					res.push_back(string("\t}\n"));
					res.push_back(string("\tfieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
				}
				else
				{
					res1 = fillUpdateInternal(fld, fld->getName(), mainObj);
					for (auto elem : res1)
					{
						res.push_back(string("\t").append(move(elem)));
					}

				}

				auto secCodesAfter = fld->getUpdateSecCodesAfter();
				if (secCodesAfter.size())
				{
					for (auto &i : secCodesAfter)
					{
						//fld->getClass()->addUpdateHandlerSec(fld->getUpdateSecurityFuncAfter());
						res.push_back(string("\tif(").append(fld->getUpdateSecurityFuncAfter()).append("(").append(to_string(i)).append(", this, param1, secure_param)); else\n"));
					}
					res.push_back(string("\t{\n"));
					res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' update rights check false\");\n"));
					res.push_back(string("\t\treturn false;\n"));
					res.push_back(string("\t}\n"));

				}
			}
			res.push_back(string("}\n"));
			res.push_back(string("else\n"));
		}
		return res;
	}

	vector<string> fillAutoUpdate(const CGfield* fld, CGmain* mainObj)
	{
		vector<string> res;

		if (fld->getProperty() & CG_AUTO_UPDATE_MASK)
		{
			unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getFieldTypeStr());
			if (fld->getProperty() & CG_UPDVER_INC_ON)
			{
				if (bcls->isSimpleNumber())
				{
					res.push_back(string("").append(fld->getName()).append("++;\n"));
					res.push_back(string("fieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
				}
				else
					throw exception(string("Cannot auto update version to the field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append(" - wrong type'\n").c_str());
			}
			else if (fld->getProperty() & CG_UPDATER_ON)
			{
				if (bcls->isSimpleString())
				{
					res.push_back(string("if(updater_string != nullptr)\n"));
					res.push_back(string("{\n"));
					res.push_back(string("\t").append(fld->getName()).append(".assign(updater_string);\n"));
					res.push_back(string("\tfieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
					res.push_back(string("}\n"));
					res.push_back(string("else\n"));
					res.push_back(string("{\n"));
					res.push_back(string("\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' auto update error - nullptr input string\");\n"));
					res.push_back(string("\treturn false;\n"));
					res.push_back(string("}\n"));
				}
				else
					throw exception(string("Cannot auto update string to the field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append(" - wrong type'\n").c_str());
			}
			else if (fld->getProperty() & CG_CURR_TIME_ON)
			{
				if (bcls->isTimeData())
				{
					string str;
					if (fld->getFieldTypeStr() == "CommSrvTime")
						str = "currentLocalTime";
					else if (fld->getFieldTypeStr() == "CommUtcTime")
						str = "currentTime";
					else if (fld->getFieldTypeStr() == "CommSrvDate")
						str = "currentLocalDate";
					else if (fld->getFieldTypeStr() == "CommUtcDate")
						str = "currentLocalDate";
					else
						throw exception(string("Cannot auto update time to the field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append(" - unknown type'\n").c_str());
					res.push_back(string("").append(fld->getName()).append(".").append(str).append("();\n"));
					res.push_back(string("fieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
				}
				else
					throw exception(string("Cannot auto update time to the field - '").append(fld->getName()).append("' class - '").append(fld->getClassName()).append(" - wrong type'\n").c_str());
			}
		}

		return res;
	}

	//vector<string> pathAdd(const string& lnidx, int counter, int margin = 0, int addition = 1)
	//{
	//	string output;
	//	vector<string> res;
	//	makeTabMargin(output, margin);
	//	output.append("int pathLen").append(to_string(counter+1)).append(" = pathLen").append(to_string(counter)).append("+").append(to_string(addition)).append(";\n");
	//	res.push_back(output);
	//	output.clear();
	//	makeTabMargin(output, margin);
	//	output.append("unique_ptr<int> u_path").append(to_string(counter+1)).append("(new int[pathLen").append(to_string(counter+1)).append("]);\n");
	//	res.push_back(output);
	//	output.clear();
	//	makeTabMargin(output, margin);
	//	output.append("int* path").append(to_string(counter+1)).append(" = u_path").append(to_string(counter+1)).append(".get();\n");
	//	res.push_back(output);
	//	output.clear();
	//	makeTabMargin(output, margin);
	//	output.append("if(pathLen").append(to_string(counter)).append(") memcpy(path").append(to_string(counter+1))
	//		.append(", path").append(to_string(counter)).append(", pathLen").append(to_string(counter)).append("*sizeof(int));\n");
	//	res.push_back(output);
	//	output.clear();
	//	makeTabMargin(output, margin);
	//	output.append("path").append(to_string(counter+1)).append("[pathLen").append(to_string(counter)).append("] = ").append(lnidx).append(";\n");
	//	res.push_back(output);
	//	output.clear();
	//	return res;
	//}
	static unique_ptr<CGGbasecls> getBaseCls(const string& str);
};


#endif	// CGGbasecls_h_included
