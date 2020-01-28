// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGGvectorcls_h_included
#define CGGvectorcls_h_included

//******************************************************************************
// Vector classes for text generators
//******************************************************************************
struct CGGvectorcls : public CGGbasecls
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

		res.append(res1).append("for(auto& elem").append(to_string(counter+1)).append(" : elem").append(to_string(counter)).append("->v)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\t").append(name).append(".push_back(").append(fieldType->getZeroSubType()->getFieldTypeStr()).append("());\n");
		res.append(res1).append("\tauto& refelem").append(to_string(counter + 1)).append(" = ").append(name).append(".back();\n");
		auto resInt = bcls->getFieldJson(fld, fieldType->getZeroSubType(), string("refelem")+ to_string(counter + 1), margin + 1, counter + 1);
		res.append(resInt);
		res.append(res1).append("}\n");

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
		res.push_back(string(name1).append(" = move(").append(name2).append(");\n"));
		return res;
	}
	vector<string> fillFieldNull(const CGfield* fld, const string& name) override
	{
		return vector<string>();
	}

	vector<string> fillFieldComparator(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
	{
		if(!fieldType->getZeroSubType())
		{
			string err;
			err.append("FieldComparator::No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		string index = "i"+to_string(counter);
		res.push_back(string("if(").append(name1).append(".size() ").append(negat?"!= ":"== ").append(name2).append(".size())").append(name3).append("\n"));
		res.push_back(string("else for(size_t ").append(index).append(" = 0; ").append(index).append(" < ").append(name1).append(".size(); ++").append(index).append(")\n"));
		res.push_back(string("{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillFieldComparator(counter+1, fld, fieldType->getZeroSubType(), name1+"["+index+"]", name2+"["+index+"]", name3, negat, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillFieldComparatorLog(int counter, const CGfield* fld, const CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
	{
		if(!fieldType->getZeroSubType())
		{
			string err;
			err.append("FieldComparator::No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		string index = "i"+to_string(counter);
		res.push_back(string("if((").append(name1).append(").size() != (").append(name2).append(").size())\n"));
		res.push_back(string("{\n"));
		res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' are NOT EQUAL by size.\");\n"));
		res.push_back(string("\tresult = false;\n"));
		res.push_back(string("}\n"));
		res.push_back(string("else for(size_t ").append(index).append(" = 0; ").append(index).append(" < (").append(name1).append(").size(); ++").append(index).append(")\n"));
		res.push_back(string("{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillFieldComparatorLog(counter+1, fld, fieldType->getZeroSubType(), name1+"["+index+"]", name2+"["+index+"]", mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		return res;
	}



	virtual vector<string> fillUpdateInternal(const CGfield* fld, const string& name, CGmain* mainObj)
	{
		vector<string> res, res1;
		bool vecSmartCorr;
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fld->getZeroSubType()->getFieldTypeStr());
		res.push_back(string("UINT32 cnt;\n"));
		res.push_back(string("parser.parseUINT32(cnt);\n"));


		if(fld->getUpdateHandler2().size())
		{
			res.push_back(string("if(cnt)\n"));
			res.push_back(string("{\n"));
			res.push_back(string("\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' : You cannot delete with handler defined\");\n"));
			res.push_back(string("\treturn false;\n"));
			res.push_back(string("}\n"));
			res.push_back(string("parser.parseUINT32(cnt);\n"));

			res.push_back(string("if(cnt) fieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));

			res.push_back(string("while(cnt--)\n"));
			res.push_back(string("{\n"));

			res.push_back(string("\t").append(name).append(".push_back(").append(fld->getZeroSubType()->getFullTypeString()).append("());\n"));
			res.push_back(string("\t").append(fld->getZeroSubType()->getFullTypeString()).append("& val = ").append(name).append(".back();\n"));

			fld->getClass()->addUpdateHandler2(fld->getUpdateHandler2());

			res.push_back(string("\tif(!").append(fld->getUpdateHandler2()).append("(\"").append(fld->getName()).append("\", parser, \"").append(fld->getZeroSubType()->getFullTypeString()).append("\", this, param1, param2, &val))\n"));
			res.push_back(string("\t{\n"));
			res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' : '").append(fld->getUpdateHandler2()).append("' returns false\");\n"));
			res.push_back(string("\t\treturn false;\n"));
			res.push_back(string("\t}\n"));


			res.push_back(string("}\n"));
		}
		else
		{
			res.push_back(string("if(cnt) fieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));

			res.push_back(string("while(cnt--)\n"));
			res.push_back(string("{\n"));
			res.push_back(string("\tbool found=false;\n"));

			res.push_back(string("\t").append(fld->getZeroSubType()->getFullTypeString()).append(" val;\n"));
			res1 = bcls->fillParse(fld, fld->getZeroSubType(), "val", "default", mainObj, fld->isArray(), vecSmartCorr);
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}

			res.push_back(string("\tsize_t i;\n"));
			res.push_back(string("\tfor(i=0; i<").append(name).append(".size(); ++i)\n"));
			res.push_back(string("\t{\n"));
			res1 = bcls->fillFieldComparator(1, fld, fld->getZeroSubType(), name+"[i]", "val", "{found=true; break;}", false, mainObj);
			for(auto elem : res1)
			{
				res.push_back(string("\t\t").append(move(elem)));
			}
			res.push_back(string("\t}\n"));

			res.push_back(string("\tif(found)\n"));
			res.push_back(string("\t{\n"));
			res.push_back(string("\t\t").append(name).append(".erase(").append(name).append(".begin()+i);\n"));
			res.push_back(string("\t}\n"));
			res.push_back(string("\telse\n"));
			res.push_back(string("\t{\n"));
			res.push_back(string("\t\terrMsg.assign(\"Error parsing object class '").append(fld->getClassName()).append("' field name '").append(fld->getName()).append("' does not contain value to delete").append("").append("\");\n"));
			res.push_back(string("\t\treturn false;\n"));
			res.push_back(string("\t}\n"));

			res.push_back(string("}\n"));
			res.push_back(string("parser.parseUINT32(cnt);\n"));
			res.push_back(string("if(cnt) fieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
			res.push_back(string("while(cnt--)\n"));
			res.push_back(string("{\n"));
			res.push_back(string("\t").append(name).append(".push_back(").append(fld->getZeroSubType()->getFullTypeString()).append("());\n"));
			res.push_back(string("\t").append(fld->getZeroSubType()->getFullTypeString()).append("& val = ").append(name).append(".back();\n"));

			res1 = bcls->fillParse(fld, fld->getZeroSubType(), "val", "default", mainObj, fld->isArray(), vecSmartCorr);
			for(auto elem : res1)
			{
				res.push_back(string("\t").append(move(elem)));
			}
			res.push_back(string("}\n"));

			res.push_back(string("parser.parseUINT32(cnt);\n"));
			res.push_back(string("if(cnt)\n"));
			res.push_back(string("{\n"));

			res.push_back(string("\tfieldsUpdate_fields.insert(\"").append(fld->getName()).append("\");\n"));
			res.push_back(string("\tsize_t i;\n"));
			res.push_back(string("\tvector<UINT32> sort_vector;\n"));
			res.push_back(string("\t").append(fld->getFullTypeString()).append(" val_container;\n"));
			res.push_back(string("\tval_container.resize(").append(name).append(".size());\n"));
			res.push_back(string("\tvector<bool> validate_vector;\n"));
			res.push_back(string("\tvalidate_vector.resize(").append(name).append(".size());\n"));
			res.push_back(string("\twhile(cnt--)\n"));
			res.push_back(string("\t{\n"));
			res.push_back(string("\t\tUINT32 sort_input;\n"));
			res.push_back(string("\t\tparser.parseUINT32(sort_input);\n"));
			res.push_back(string("\t\tsort_vector.push_back(sort_input);\n"));
			res.push_back(string("\t}\n"));

			res.push_back(string("\tif(sort_vector.size() != ").append(name).append(".size())\n"));
			res.push_back(string("\t{\n"));
			//res.push_back(string("\t\tval_container.push_back(move(").append(name).append("[i]));\n"));
			res.push_back(string("\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field '").append(name).append("' update: Wrong size sorting vector - \").appendInt(sort_vector.size()).append(\" != \").appendInt(").append(name).append(".size());\n"));
			res.push_back(string("\t\treturn false;\n"));
			res.push_back(string("\t}\n"));

			res.push_back(string("\tfor(i = 0; i < ").append(name).append(".size(); ++i)\n"));
			res.push_back(string("\t{\n"));
			//res.push_back(string("\t\tif((sort_vector[i] > ").append(name).append(".size()) || (sort_vector[i] < 1))\n"));
			//res.push_back(string("\t\t{\n"));
			//res.push_back(string("\t\t\tval_container.push_back(move(").append(name).append("[i]));\n"));
			//res.push_back(string("\t\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field '").append(name).append("' update: Wrong size of sorting vector.\");\n"));
			//res.push_back(string("\t\t\treturn false;\n"));
			//res.push_back(string("\t\t}\n"));
			res.push_back(string("\t\tif(validate_vector[sort_vector[i]-1])\n"));
			res.push_back(string("\t\t{\n"));
			//res.push_back(string("\t\t\tval_container.push_back(move(").append(name).append("[i]));\n"));
			res.push_back(string("\t\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field '").append(name).append("' update: Double values in sorting vector - \").appendInt(sort_vector[i]);\n"));
			res.push_back(string("\t\t\treturn false;\n"));
			res.push_back(string("\t\t}\n"));
			res.push_back(string("\t\telse\n"));
			res.push_back(string("\t\t{\n"));
			res.push_back(string("\t\t\tval_container[sort_vector[i]-1] = move(").append(name).append("[i]);\n"));
			res.push_back(string("\t\t\tvalidate_vector[sort_vector[i]-1] = true;\n"));
			res.push_back(string("\t\t}\n"));

			res.push_back(string("\t}\n"));

			res.push_back(string("\tfor(i = validate_vector.size(); i > 0; --i)\n"));
			res.push_back(string("\t{\n"));
			res.push_back(string("\t\tif(!validate_vector[i-1])\n"));
			res.push_back(string("\t\t{\n"));
			//res.push_back(string("\t\t\tval_container.erase(val_container.begin() + i - 1);\n"));
			res.push_back(string("\t\t\terrMsg.assign(\"Class '").append(fld->getClassName()).append("' field '").append(name).append("' update: Field position is not valid - \").appendInt(i-1);\n"));
			res.push_back(string("\t\t\treturn false;\n"));
			res.push_back(string("\t\t}\n"));
			res.push_back(string("\t}\n"));

			res.push_back(string("\t").append(name).append(".swap(val_container);\n"));
			res.push_back(string("}\n"));
		}

		return res;
	}
	virtual vector<string> fillParse(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		if(!fieldType->getZeroSubType())
		{
			string err;
			err.append("FieldComparator::No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		res.push_back(string("{\n"));
		res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
		res.push_back(string("\t").append(name).append(".clear();\n"));
		res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
		res.push_back(string("\t").append(name).append(".resize(").append(fld->getName()).append("_count);\n"));
		res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		bool vecSmartC = false;
		res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), name+"[i]", channel, mainObj, false, vecSmartC);
		if(vecSmartC)
		{
			res.push_back(string("\t\tUINT32 __ce__size;\n"));
			res.push_back(string("\t\tparser.parseUINT32(__ce__size);\n"));
		}
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}

		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj, bool isArray, bool& vecSmartCorr) override
	{
		if(!fieldType->getZeroSubType())
		{
			string err;
			err.append("FieldComparator::No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		res.push_back(string("{\n"));
		res.push_back(string("\tsize_t ").append(fld->getName()).append("_size = ").append(fld->getName()).append(".size();\n"));
		res.push_back(string("\tdata.composeUINT32(").append(fld->getName()).append("_size);\n"));
		res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_size; ++i)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());

		bool vecSmartC = false;
		res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), name+"[i]", channel, mainObj, false, vecSmartC);
		if(vecSmartC)
		{
			res.push_back(string("\t\tsize_t __ce__shift=0;\n"));
			res.push_back(string("\t\tsize_t __ce__size=0;\n"));
			res.push_back(string("\t\tdata.composeVectorSize(0,__ce__shift);\n"));
			res.push_back(string("\t\t__ce__size = data._size();\n"));
		}
		for(auto elem : res1)
		{
			res.push_back(string("\t\t").append(move(elem)));
		}
		if(vecSmartC)
		{
			res.push_back(string("\t\tdata.updateVectorSize( data._size() - __ce__size, __ce__shift);\n"));
		}

		res.push_back(string("\t}\n"));
		res.push_back(string("}\n"));
		return res;
	}
	vector<string> fillSelConvertCompose(const CGfield* fld, const CGfieldtype* fieldType, const string& name, CGmain* mainObj) override
	{
		if(!fieldType->getZeroSubType())
		{
			string err;
			err.append("FieldComparator::No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
			throw exception(err.c_str());
		}
		vector<string> res, res1;
		res.push_back(string("{\n"));
		res.push_back(string("\tsize_t ").append(fld->getName()).append("_size = ").append(fld->getName()).append(".size();\n"));
		res.push_back(string("\tdata.composeUINT32(").append(fld->getName()).append("_size);\n"));
		res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_size; ++i)\n"));
		res.push_back(string("\t{\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());

		res1 = bcls->fillSelConvertCompose(fld, fieldType->getZeroSubType(), name+"[i]", mainObj);
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
		vector<string> res, res1;

		res.push_back(string("for(size_t i=0; i<").append(name).append(".size(); ++i)\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillPublisher(fld, fieldType->getZeroSubType(), mainObj, "i", fld->getName()+"[i]", counter);
		res.insert(res.end(), res1.begin(), res1.end());
		return res;
	}
	vector<string> fillFieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		return vector<string>();
	}
	vector<string> fillFieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;


		res.push_back(string("for(size_t i=0; i<").append(fld->getName()).append(".size(); ++i)\n"));
		res.push_back(string("{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillFieldComparator(1, fld, fieldType, fld->getName()+"[i]", fld->getName()+"_data", " return;", false, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));
		res.push_back(string("").append(name1).append(".push_back(").append(fld->getName()).append("_data);\n"));

		//res1 = pathAdd(lnidx, counter, 0);
		//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
		res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));

		res1 = bcls->fieldSubscrAdd(fld, fieldType->getZeroSubType(), mainObj, "", "", fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("").append(move(elem)));
		}

		return res;
	}
	virtual vector<string> fillFieldSubscrRemove(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;

		res.push_back(string("size_t i;\n"));
		res.push_back(string("for(i=0; i<").append(fld->getName()).append(".size(); ++i)\n"));
		res.push_back(string("{\n"));
		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		res1 = bcls->fillFieldComparator(1, fld, fieldType, fld->getName()+"[i]", fld->getName()+"_data", " break;", false, mainObj);
		for(auto elem : res1)
		{
			res.push_back(string("\t").append(move(elem)));
		}
		res.push_back(string("}\n"));

		res.push_back(string("if(i == ").append(name1).append(".size()) return;\n"));
		res.push_back(string("").append(name1).append(".erase(").append(name1).append(".begin()+i);\n"));

		//res1 = pathAdd(lnidx, counter, 0);
		//res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
		res.push_back(string("\tpp.addValue(").append(lnidx).append(");\n"));
		res.push_back(string("\tpp.addValue(i);\n"));

		//res.push_back(string("path").append(to_string(counter+1)).append("[pathLen").append(to_string(counter)).append("+1] = i;\n"));
		//res.push_back(string("\tpublisher.removeLeaf(path").append(to_string(counter+1)).append(",pathLen").append(to_string(counter+1)).append(",data,&update);\n"));
		res1 = bcls->fieldSubscrRemove(fld, fieldType->getZeroSubType(), mainObj, "", "", fld->getName()+"_data");
		for(auto elem : res1)
		{
			res.push_back(string("").append(move(elem)));
		}
		return res;
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
		return true;
	}
	bool isSubscriptionRemove(void) override
	{
		return true;
	}
	vector<string> fieldSubscrUpdate(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		return res;
	}
	vector<string> fieldSubscrAdd(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj, const string& lnidx, const string& name1, const string& name2) override
	{
		vector<string> res, res1;
		bool dum = false;
		res.push_back(string("CommMsgBody data;\n"));
		res1 = fillCompose(fld, fieldType, name1, "default", mainObj, false, dum);
		res.insert(res.end(), std::make_move_iterator(res1.begin()), std::make_move_iterator(res1.end()));
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
		res.push_back(string("\tsize_t ").append(fld->getName()).append("_size;\n"));
		res.push_back(string("\tsize_t shift;\n"));
		res.push_back(string("\tUINT32 count = 0;\n"));
		res.push_back(string("\tif((composeSelectiveFieldsInt_code & 0x1) || (!(composeSelectiveFieldsInt_code & 0x7F))) // first elements or all\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tdata.composeVectorSize(0,shift);\n"));

		res.push_back(string("\t\tif(composeSelectiveFieldsInt_filtersize)\n"));
		res.push_back(string("\t\t\t").append(fld->getName()).append("_size = (").append(fld->getName()).append(".size() < composeSelectiveFieldsInt_filtersize)?").append(fld->getName()).append(".size():composeSelectiveFieldsInt_filtersize;\n"));
		res.push_back(string("\t\telse\n"));
		res.push_back(string("\t\t\t").append(fld->getName()).append("_size = ").append(fld->getName()).append(".size();\n"));

		//res.push_back(string("\tdata.composeUINT32(").append(fld->getName()).append("_size);\n"));


		res.push_back(string("\t\tfor(size_t i=0; i<").append(fld->getName()).append("_size; ++i)\n"));

		res.push_back(string("\t\t{\n"));
		//res.push_back(string("\t\tsize_t path_pointer_int = path_pointer;\n"));

		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
		if(bcls->isOurObject(fieldType->getZeroSubType(), mainObj))
		{

			res.push_back(string("\t\t\tsize_t path_pointer_int = path_pointer;\n"));
			res.push_back(string("\t\t\tcount++;\n"));
			res.push_back(string("\t\t\t").append(fld->getName()).append("[i].composeSelectiveFieldsCounted(path, data, path_pointer_int, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));
		}
		else
		{
			if(fld->getRefClass().first.size())
			{
				const CGbaseobj* obj = mainObj->findObject(objTypes::gTypeStruct, fld->getRefClass().first);
				if(obj && (bcls->isSimpleNumber() || bcls->isSimpleString()))
				{
					res.push_back(string("\t\t\tif(composeSelectiveFieldsInt_code & 0x80) // reference\n"));
					res.push_back(string("\t\t\t{\n"));

					if(bcls->isSimpleString())
					{
						if (mainObj->baseRefClass.size() == 0)
							throw exception(string("Empty base reference class").c_str());
						res.push_back(string("\t\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", ").append(fld->getName()).append("[i], 0, param1, param2);\n"));
					}
					else if(bcls->isSimpleNumber())
					{
						if (mainObj->baseRefClass.size() == 0)
							throw exception(string("Empty base reference class").c_str());
						res.push_back(string("\t\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", nullptr, ").append(fld->getName()).append("[i], param1, param2);\n"));
					}

					//res.push_back(string("\t\t\tif(!objPtr) throw PError(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'\");\n"));
					res.push_back(string("\t\t\t\tif(!objPtr)\n"));
					res.push_back(string("\t\t\t\t{\n"));
					res.push_back(string("\t\t\t\t\tPString err;\n"));
					res.push_back(string("\t\t\t\t\terr.append(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
						.append(fld->getClassName()).append("' value '\").").append(bcls->isSimpleNumber()?"appendInt(":"append(").append(fld->getName()).append("[i]).append(").append("\"'\");\n"));
					res.push_back(string("\t\t\t\t\tPLog(err.c_str());\n"));
					//res.push_back(string("\t\t\t\tPLog(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
					//	.append(fld->getClassName()).append("' value '").append("err").append("'\");\n"));
					res.push_back(string("\t\t\t\t\tcontinue;\n"));
					res.push_back(string("\t\t\t\t}\n"));
					res.push_back(string("\t\t\t\tsize_t path_pointer_int = path_pointer;\n"));
					res.push_back(string("\t\t\t\tcount++;\n"));
					res.push_back(string("\t\t\t\tobjPtr->composeSelectiveFieldsCounted(path, data, path_pointer_int, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));

					res.push_back(string("\t\t\t}\n"));
					res.push_back(string("\t\t\telse\n"));
					res.push_back(string("\t\t\t{\n"));
					res.push_back(string("\t\t\t\tcount++;\n"));
					res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), fld->getName()+"[i]", "default", mainObj, false, dum);
					for(auto elem : res1)
					{
						res.push_back(string("\t\t\t\t").append(move(elem)));
					}
					res.push_back(string("\t\t\t}\n"));
				}
				else
					throw exception(string("Wrong reference class '").append(fld->getRefClass().first).append("  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'").c_str());
			}
			else
			{
				res.push_back(string("\t\t\tcount++;\n"));
				res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), fld->getName()+"[i]", "default", mainObj, false, dum);
				for(auto elem : res1)
				{
					res.push_back(string("\t\t\t").append(move(elem)));
				}
			}
		}

		res.push_back(string("\t\t}\n"));
		res.push_back(string("\t\tdata.updateVectorSize(count,shift);\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\telse if(composeSelectiveFieldsInt_code & 0x2) // element index \n"));
		res.push_back(string("\t{\n"));

//////////////////
//		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());

		//res.push_back(string("\t\tsize_t i = composeSelectiveFieldsInt_filtersize;\n"));
		res.push_back(string("\t\tif(composeSelectiveFieldsInt_filtersize >= ").append(fld->getName()).append(".size())\n"));
		res.push_back(string("\t\t{\n"));
		res.push_back(string("\t\t\tPString err;\n"));
		res.push_back(string("\t\t\terr.append(\"Out of range index in class '").append(fld->getClassName()).append("' field '")
			.append(fld->getName()).append("' received-\").appendInt(composeSelectiveFieldsInt_filtersize).append(\" size-\").appendInt(")
			.append(fld->getName()).append(".size()").append(");\n"));
		res.push_back(string("\t\t\tPLog(err.c_str());\n"));
		res.push_back(string("\t\t\tdata.composeUINT32(0);\n"));
		//res.push_back(string("\t\t\tbreak;\n"));
		res.push_back(string("\t\t}\n"));
		res.push_back(string("\t\telse\n"));
		res.push_back(string("\t\t{\n"));
		res.push_back(string("\t\t\tdata.composeUINT32(1);\n"));

		if(bcls->isOurObject(fieldType->getZeroSubType(), mainObj))
		{

			res.push_back(string("\t\t\tsize_t path_pointer_int = path_pointer;\n"));
			res.push_back(string("\t\t\t").append(fld->getName()).append("[composeSelectiveFieldsInt_filtersize].composeSelectiveFieldsCounted(path, data, path_pointer_int, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));
		}
		else
		{
			if(fld->getRefClass().first.size())
			{
				const CGbaseobj* obj = mainObj->findObject(objTypes::gTypeStruct, fld->getRefClass().first);
				if(obj && (bcls->isSimpleNumber() || bcls->isSimpleString()))
				{
					res.push_back(string("\t\t\tif(composeSelectiveFieldsInt_code & 0x80) // reference\n"));
					res.push_back(string("\t\t\t{\n"));

					if(bcls->isSimpleString())
					{
						if (mainObj->baseRefClass.size() == 0)
							throw exception(string("Empty base reference class").c_str());
						res.push_back(string("\t\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", ").append(fld->getName()).append("[composeSelectiveFieldsInt_filtersize], 0, param1, param2);\n"));
					}
					else if(bcls->isSimpleNumber())
					{
						if (mainObj->baseRefClass.size() == 0)
							throw exception(string("Empty base reference class").c_str());
						res.push_back(string("\t\t\t\tconst ").append(mainObj->baseRefClass).append("* objPtr = getRefObject(\"").append(fld->getRefClass().second).append("\", nullptr, ").append(fld->getName()).append("[composeSelectiveFieldsInt_filtersize], param1, param2);\n"));
					}

					//res.push_back(string("\t\t\tif(!objPtr) throw PError(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'\");\n"));
					res.push_back(string("\t\t\t\tif(!objPtr)\n"));
					res.push_back(string("\t\t\t\t{\n"));
					res.push_back(string("\t\t\t\t\tPString err;\n"));
					res.push_back(string("\t\t\t\t\terr.append(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
						.append(fld->getClassName()).append("' value '\").").append(bcls->isSimpleNumber()?"appendInt(":"append(").append(fld->getName()).append("[composeSelectiveFieldsInt_filtersize]).append(").append("\"'\");\n"));
					res.push_back(string("\t\t\t\t\tPLog(err.c_str());\n"));
					//res.push_back(string("\t\t\t\tPLog(\"Empty reference class '").append(fld->getRefClass().first).append("'  in field '").append(fld->getName()).append("' class '")
					//	.append(fld->getClassName()).append("' value '").append("err").append("'\");\n"));
					res.push_back(string("\t\t\t\t\tcontinue;\n"));
					res.push_back(string("\t\t\t\t}\n"));
					res.push_back(string("\t\t\t\tsize_t path_pointer_int = path_pointer;\n"));
					res.push_back(string("\t\t\t\tobjPtr->composeSelectiveFieldsCounted(path, data, path_pointer_int, composeSelectiveFieldsInt_size, getRefObject, param1, param2);\n"));

					res.push_back(string("\t\t\t}\n"));
					res.push_back(string("\t\t\telse\n"));
					res.push_back(string("\t\t\t{\n"));
					res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), fld->getName()+"[composeSelectiveFieldsInt_filtersize]", "default", mainObj, false, dum);
					for(auto elem : res1)
					{
						res.push_back(string("\t\t\t\t").append(move(elem)));
					}
					res.push_back(string("\t\t\t}\n"));
				}
				else
					throw exception(string("Wrong reference class '").append(fld->getRefClass().first).append("  in field '").append(fld->getName()).append("' class '").append(fld->getClassName()).append("'").c_str());
			}
			else
			{
				res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), fld->getName()+"[composeSelectiveFieldsInt_filtersize]", "default", mainObj, false, dum);
				for(auto elem : res1)
				{
					res.push_back(string("\t\t\t\t").append(move(elem)));
				}
			}
		}

		res.push_back(string("\t\t}\n"));

///////////////////
		res.push_back(string("\t}\n"));
		res.push_back(string("\telse if(composeSelectiveFieldsInt_code & 0x4) // vector size \n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tdata.composeUINT32(").append(fld->getName()).append(".size());\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\telse\n"));
		res.push_back(string("\t{\n"));
		res.push_back(string("\t\tdata.composeUINT32(0);\n"));
		res.push_back(string("\t}\n"));
		res.push_back(string("\tpath_pointer += composeSelectiveFieldsInt_size;\n"));
		res.push_back(string("\tbreak;\n"));
		res.push_back(string("}\n"));

		return res;
	}
};

struct CGGstringpairvectorcls : public CGGvectorcls
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
		res.append(res1).append("for(auto& it").append(to_string(counter)).append(" = ").append(name2).append(".begin(); it").append(to_string(counter)).append(" != ").append(name2).append(".end();)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\tauto& elem").append(to_string(counter)).append(" = *it").append(to_string(counter)).append(";\n");
		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("  {\\n\");\n");

		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("    \\\"name\\\": \\\"\").append(it").append(to_string(counter)).append("->name).append(\"\\\",\\n\");\n");
		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("    \\\"value\\\": \\\"\").append(it").append(to_string(counter)).append("->value).append(\"\\\"\\n\");\n");

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
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}

		res.append(res1).append("for(auto& elem").append(to_string(counter + 1)).append(" : elem").append(to_string(counter)).append("->v)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\t").append(name).append(".push_back(").append(fieldType->getZeroSubType()->getFieldTypeStr()).append("());\n");
		res.append(res1).append("\tauto& refelem").append(to_string(counter + 1)).append(" = ").append(name).append(".back();\n");

		res.append(res1).append("\tfor(size_t i=0; i<(elem").append(to_string(counter+1)).append("->v).size(); ++i)\n");
		res.append(res1).append("\t{\n");
		res.append(res1).append("\t\tif(elem").append(to_string(counter+1)).append("->v[i]->name.equals(\"name\")) refelem").append(to_string(counter + 1)).append(".name = elem").append(to_string(counter+1)).append("->v[i]->s;\n");
		res.append(res1).append("\t\telse if(elem").append(to_string(counter+1)).append("->v[i]->name.equals(\"value\")) refelem").append(to_string(counter + 1)).append(".value = elem").append(to_string(counter+1)).append("->v[i]->s;\n");
		res.append(res1).append("\t}\n");
		res.append(res1).append("}\n");

		return res;
	}
};

struct CGGstringunit32pairvectorcls : public CGGvectorcls
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
		res.append(res1).append("for(auto& it").append(to_string(counter)).append(" = ").append(name2).append(".begin(); it").append(to_string(counter)).append(" != ").append(name2).append(".end();)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\tauto& elem").append(to_string(counter)).append(" = *it").append(to_string(counter)).append(";\n");
		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("  {\\n\");\n");

		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("    \\\"name\\\": \\\"\").append(it").append(to_string(counter)).append("->name).append(\"\\\",\\n\");\n");
		res.append(res1).append("\toutput.append(margin_string).append(\"").append(res2).append("    \\\"value\\\": \").appendInt(it").append(to_string(counter)).append("->value).append(\"\\n\");\n");

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
		for (int i = 0; i < margin; ++i)
		{
			res1.append("\t");
		}

		res.append(res1).append("for(auto& elem").append(to_string(counter + 1)).append(" : elem").append(to_string(counter)).append("->v)\n");
		res.append(res1).append("{\n");
		res.append(res1).append("\t").append(name).append(".push_back(").append(fieldType->getZeroSubType()->getFieldTypeStr()).append("());\n");
		res.append(res1).append("\tauto& refelem").append(to_string(counter + 1)).append(" = ").append(name).append(".back();\n");

		res.append(res1).append("\tfor(size_t i=0; i<(elem").append(to_string(counter + 1)).append("->v).size(); ++i)\n");
		res.append(res1).append("\t{\n");
		res.append(res1).append("\t\tif(elem").append(to_string(counter + 1)).append("->v[i]->name.equals(\"name\")) refelem").append(to_string(counter + 1)).append(".name = elem").append(to_string(counter + 1)).append("->v[i]->s;\n");
		res.append(res1).append("\t\telse if(elem").append(to_string(counter + 1)).append("->v[i]->name.equals(\"value\")) refelem").append(to_string(counter + 1)).append(".value = elem").append(to_string(counter + 1)).append("->v[i]->i;\n");
		res.append(res1).append("\t}\n");
		res.append(res1).append("}\n");

		return res;
	}
};



// struct CGGpyrextptrvectorcls : public CGGbasecls
// {
// 	virtual vector<string> getFieldComparator(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2, const string& name3, bool negat, CGmain* mainObj) override
// 	{
// 		if(!fieldType->getZeroSubType())
// 		{
// 			string err;
// 			err.append("No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
// 			throw exception(err.c_str());
// 		}
// 		vector<string> res, res1;
// 		string index = "i"+to_string(counter);
// 		res.push_back(string("if(").append(name1).append(".size() != ").append(name2).append(".size())").append(name3).append(";\n"));
// 		res.push_back(string("else for(size_t ").append(index).append(" = 0; ").append(index).append(" < ").append(name1).append(".size(); ++").append(index).append(")\n"));
// 		res.push_back(string("{\n"));
// 		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
// 		res1 = bcls->getFieldComparator(counter+1, fld, fieldType->getZeroSubType(), string("*(")+name1+"["+index+"])", string("(*(")+name2+"["+index+"]))", name3, negat, mainObj);
// 		for(auto elem : res1)
// 		{
// 			res.push_back(string("\t").append(move(elem)));
// 		}
// 		res.push_back(string("}\n"));
// 		return res;
// 	}
// 
// 	virtual vector<string> getFieldComparatorLog(int counter, const CGfield* fld, CGfieldtype* fieldType, const string& name1, const string& name2, CGmain* mainObj) override
// 	{
// 		if(!fieldType->getZeroSubType())
// 		{
// 			string err;
// 			err.append("No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
// 			throw exception(err.c_str());
// 		}
// 		vector<string> res, res1;
// 		string index = "i"+to_string(counter);
// 		res.push_back(string("if((").append(name1).append(").size() != (").append(name2).append(").size())\n"));
// 		res.push_back(string("{\n"));
// 		res.push_back(string("\tPLog(\"Fields '").append(fld->getName()).append("' in class '").append(fld->getClassName()).append("' type '").append(fieldType->getFullTypeString(true)).append("' are NOT EQUAL by size.\");\n"));
// 		res.push_back(string("\tresult = false;\n"));
// 		res.push_back(string("}\n"));
// 		res.push_back(string("else for(size_t ").append(index).append(" = 0; ").append(index).append(" < (").append(name1).append(").size(); ++").append(index).append(")\n"));
// 		res.push_back(string("{\n"));
// 		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
// 		res1 = bcls->getFieldComparatorLog(counter+1, fld, fieldType->getZeroSubType(), string("(*(")+name1+"["+index+"]))", string("(*(")+name2+"["+index+"]))", mainObj);
// 		for(auto elem : res1)
// 		{
// 			res.push_back(string("\t").append(move(elem)));
// 		}
// 		res.push_back(string("}\n"));
// 		return res;
// 	}
// 
// 	virtual vector<string> fillParse(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
// 	{
// 		if(!fieldType->getZeroSubType())
// 		{
// 			string err;
// 			err.append("No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
// 			throw exception(err.c_str());
// 		}
// 		vector<string> res, res1;
// 		res.push_back(string("{\n"));
// 		res.push_back(string("\tUINT32 ").append(fld->getName()).append("_count;\n"));
// 		res.push_back(string("\t").append(name).append(".clear();\n"));
// 		res.push_back(string("\tparser.parseUINT32(").append(fld->getName()).append("_count);\n"));
// 		res.push_back(string("\t").append(name).append(".resize(").append(fld->getName()).append("_count);\n"));
// 		res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_count; ++i)\n"));
// 		res.push_back(string("\t{\n"));
// 
// 		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
// 		res1 = bcls->fillParse(fld, fieldType->getZeroSubType(), string("(*(")+name+"[i]))", channel, mainObj);
// 		for(auto elem : res1)
// 		{
// 			res.push_back(string("\t\t").append(move(elem)));
// 		}
// 
// 		res.push_back(string("\t}\n"));
// 		res.push_back(string("}\n"));
// 		return res;
// 	}
// 	virtual vector<string> fillCompose(const CGfield* fld, CGfieldtype* fieldType, const string& name, const string& channel, CGmain* mainObj) override
// 	{
// 		if(!fieldType->getZeroSubType())
// 		{
// 			string err;
// 			err.append("No subtype in 'vector' field, class-'").append(fld->getClassName()).append("' field-'").append(fld->getName());
// 			throw exception(err.c_str());
// 		}
// 		vector<string> res, res1;
// 		res.push_back(string("{\n"));
// 		res.push_back(string("\tsize_t ").append(fld->getName()).append("_size = ").append(fld->getName()).append(".size();\n"));
// 		res.push_back(string("\tdata.composeUINT32(").append(fld->getName()).append("_size);\n"));
// 		res.push_back(string("\tfor(size_t i=0; i<").append(fld->getName()).append("_size; ++i)\n"));
// 		res.push_back(string("\t{\n"));
// 
// 		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
// 		res1 = bcls->fillCompose(fld, fieldType->getZeroSubType(), string("(*(")+name+"[i]))", channel, mainObj);
// 		for(auto elem : res1)
// 		{
// 			res.push_back(string("\t\t").append(move(elem)));
// 		}
// 
// 		res.push_back(string("\t}\n"));
// 		res.push_back(string("}\n"));
// 		return res;
// 	}
// 	virtual vector<string> fillFieldPublisher(const CGfield* fld, CGfieldtype* fieldType, CGmain* mainObj, const vector<string>& path, const string& lnidx, int counter) override
// 	{
// 		vector<string> res, res1;
// 
// 		res.push_back(string("for(size_t i=0; i<").append(fld->getName()).append(".size(); ++i)\n"));
// 		unique_ptr<CGGbasecls> bcls = CGGbasecls::getBaseCls(fieldType->getZeroSubType()->getFieldTypeStr());
// 		res1 = bcls->fillPublisher(fld, fieldType->getZeroSubType(), mainObj, path, "i", fld->getName()+"[i]", counter);
// 		res.insert(res.end(), res1.begin(), res1.end());
// 		return res;
// 	}
// 	virtual bool isLeaf(void) {return false;}
// 
//};

#endif	// CGGvectorcls_h_included
