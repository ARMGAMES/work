// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genJava.h"
#include "direct.h"

void CGJjavamain::createJavaFiles(void)
{
	for(const auto& docp : mainObj->documents)
	{
		for(auto& elem : docp.second.getObjects())
		{
			switch(elem->getObjType())
			{
			case objTypes::gTypeStruct:
				{
					createJavaClass(docp.first, elem.get());
					createJavaQuery(docp.first, elem.get());
					break;
				}
			default:
					break;
			};
		}
	}
	//{
	//	string output;
	//	CGfile cfile;
	//	if(mainObj->outdir.length())
	//	{
	//		output.assign(mainObj->outdir).append("\\");
	//	}
	//	output.append("ICommlib.java");
	//	cfile.open(output);
	//	output.clear();
	//	output.append(COPYRIGHT);
	//	cfile.write(output, true);

	//	output.append("interface ICommlib {\n");
	//	output.append("\tpublic void parse(CommMsgParser commMsgPar);\n");
	//	output.append("\tpublic CommMsgBody compose(CommMsgBody commMsg);\n");
	//	output.append("}\n");
	//	cfile.write(output, true);
	//}
}

void CGJjavamain::createJavaClass(const string& nameDoc, const CGbaseobj* p)
{
	string output;
	CGfile cfile;
	vector<string> key, str;

	const CGstruct* cls = dynamic_cast<const CGstruct*>(p);
	if(!cls)
	{
		string err;
		err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' object-'").append(p->getJavaName());
		throw exception(err.c_str());
	}
	string name = cls->getJavaName().empty() ? cls->getName() : cls->getJavaName();


	clear();
	for(auto& elem : cls->getObjects())
	{
		if(elem->getObjType() == objTypes::gTypeField)
		{
			const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());
			if(!fld)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' class-'").append(cls->getJavaName()).append("' object-'").append(elem->getJavaName());
				throw exception(err.c_str());
			}

			unique_ptr<CGJbasecls> bcls;
			bcls = CGJbasecls::getBaseCls(fld->getFieldType()->getJavaType().empty()?fld->getFieldType()->getFieldTypeStr(): fld->getFieldType()->getJavaType());
			str = fill(fld, bcls.get());			
			if (key.empty()) key = move(str);

		}
	}
	if (gFields.empty()) return;

	if(mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	else
	{
		auto z = nameDoc.find_last_of("\\");
		if (z == string::npos)
			z = nameDoc.find_last_of("/");
		if (z != string::npos)
			output.assign(nameDoc.substr(0, z+1));
	}

	if (!cls->getJavaPackage().empty())
	{
		if (output.empty())
			output.append(".\\");
		for (auto& elem : cls->getJavaPackage())
		{
			output.append(elem).append("\\");
			int a = mkdir(output.c_str());
		}
	}

	output.append(name).append(".java");
	cfile.open(output);
	output.clear();

	output.append(COPYRIGHT);
	cfile.write(output, true);

	if(cls->getJavaPackage().empty())
	{
		auto it = mainObj->iniInfo.find("JAVA_PACKAGE");
		if(it != end(mainObj->iniInfo))
		{
			for(auto& elem : it->second)
			{
				output.append("package ").append(elem.first.c_str()).append(";\n");
			}
		}
		output.append("\n\n");
	}
	else
	{
		output.append("package ");
		for (auto i = 0; i < cls->getJavaPackage().size(); ++i)
		{
			if(i>0) output.append(".");
			output.append(cls->getJavaPackage()[i]);
		}
		output.append(";\n\n");
	}

	for(auto& elem : gImport)
	{
		output.append("import ").append(elem).append(";\n");
	}
	{
		auto it = mainObj->iniInfo.find("JAVA_IMPORT");
		if(it != end(mainObj->iniInfo))
		{
			for(auto& elem : it->second)
			{
				output.append("import ").append(elem.first).append(";\n");
			}
		}
	}

	//output.append("import com.ps.commlib2a.*;\n");
	output.append("import java.util.*;\n");
	output.append("import com.ps.cesar.enums.*;\n");
	output.append("import com.ps.cesar.api.*;\n");
	output.append("import com.ps.cesar.annotation.*;\n");
	output.append("import com.ps.commlib2a.transfer.*;\n");
	output.append("import io.leangen.graphql.annotations.*;\n");
	output.append("\n");


	output.append("public class ").append(name).append(" implements Model {\n");

	for(auto& elem : gFields)
	{
		makeTabMargin(output, 1);
		output.append(elem);
	}
	output.append("\n");
	cfile.write(output, true);
//constructors
	//makeTabMargin(output, 1);
	//output.append("public ").append(name).append("(){\n");
	//makeTabMargin(output, 1);
	//output.append("}\n\n");
	//makeTabMargin(output, 1);
	//output.append("public ").append(name).append("(boolean archived){\n");
	//makeTabMargin(output, 2);
	//output.append("this.archived = archived;\n");
	//makeTabMargin(output, 1);
	//output.append("}\n\n");

	//cfile.write(output, true);


	//for (auto& elem : key)
	//{
	//	makeTabMargin(output, 1);
	//	output.append(elem);
	//}
	//output.append("\n");
	//cfile.write(output, true);

	makeTabMargin(output, 1);
	output.append("@Override\n");
	makeTabMargin(output, 1);
	output.append("public String getModelType() {\n");
	makeTabMargin(output, 2);
	//if (cls->hasProperty(CG_CLASS_ARCHIVED_ON))
	//{
	//	output.append("return isArchived() ? \"").append(cls->getArcType()).append("\" : \"").append(cls->getModelType()).append("\";\n");
	//}
	//else
	//{
		output.append("return \"").append(cls->getModelType()).append("\";\n");
	//}

	makeTabMargin(output, 1);
	output.append("}\n");
	output.append("\n");
	cfile.write(output, true);

	//for(auto& elem : gSetters)
	//{
	//	for(auto& elem1 : elem)
	//	{
	//		makeTabMargin(output, 1);
	//		output.append(elem1);
	//	}
	//}
	//output.append("\n");
	//cfile.write(output, true);

	for(auto& elem : gGetters)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	//output.append("\n");
	cfile.write(output, true);

	//if (cls->getJavaProperty() & CG_JAVA_OVERRIDE_PARSE)
	//	output.append("\t@Override\n");
	//output.append("\tpublic void parse(CommMsgParser commMsgPar) {\n");
	//output.append("\t\tif(!(commMsgPar)) throw new Exception(\"CommMsgParser object is null in class ").append(cls->getJavaName()).append("\");\n");
	//for(auto& elem : gParse)
	//{
	//	for(auto& elem1 : elem)
	//	{
	//		makeTabMargin(output, 2);
	//		output.append(elem1);
	//	}
	//}
	//output.append("\t}\n");

	//output.append("\n");
	//cfile.write(output, true);

	//if (cls->getJavaProperty() & CG_JAVA_OVERRIDE_COMPOSE)
	//	output.append("\t@Override\n");
	//output.append("\tpublic CommMsgBody compose(CommMsgBody commMsg) {\n");
	//output.append("\t\tif (commMsg == null) commMsg = new CommMsgBody();\n");
	//for(auto& elem : gCompose)
	//{
	//	for(auto& elem1 : elem)
	//	{
	//		makeTabMargin(output, 2);
	//		output.append(elem1);
	//	}
	//}

	//output.append("\t\treturn commMsg;\n");
	output.append("}\n");
	cfile.write(output, true);
}

vector<string> CGJjavamain::fill(const CGfield* fld, CGJbasecls* bcls)
{
	string s;
	vector<string> key;
	vector<string> buf;

	const map<string, tuple<bool,bool,bool>>& channels =  fld->getPcChannels();
	auto it = channels.find("JAVA");
	if(it != end(channels))
	{
		//if (!fld->getGetter().empty())
		//{
		//	key.push_back(string("@GraphQLIgnore\n"));
		//	key.push_back(string("@Override\n"));
		//	if(fld->getFieldTypeStr() == "PString")
		//	{
		//		key.push_back(string("public String getKey() {\n"));
		//		key.push_back(string("\treturn ") + fld->getJavaName() + ";}\n");
		//		key.push_back(string("}\n"));
		//	}
		//	else
		//	{
		//		key.push_back(string("public Integer getKey() {\n"));
		//		key.push_back(string("\treturn ") + fld->getJavaName() + ";}\n");
		//		key.push_back(string("}\n"));
		//	}
		//}
		s.append("@MappedAttribute (path = ").append(to_string(fld->getFieldIndex())).append(bcls->getCommonType(fld)).append(")\n");
		gFields.push_back(move(s));
		s.clear();

		if (!fld->getJavaName().empty() || !fld->getJavaDescr().empty())
		{
			s.append("@GraphQLQuery (");

			if(!fld->getJavaName().empty())
			{
				s.append("name = \"").append(fld->getJavaName()).append("\"");
				if (!fld->getJavaDescr().empty())
					s.append(", ");
			}
			if(!fld->getJavaDescr().empty())
				s.append("description = ").append(fld->getJavaDescr());

			s.append(")\n");
			gFields.push_back(move(s));
			s.clear();
		}

		if (fld->getJavaProperty() & CG_JAVA_TRANSIENT_ON)
			s.append("transient ");

		auto refCls = fld->getRefClass().first;
		if (!refCls.empty() && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "Set"))
		{
			string str;
			str = CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj);
			str.replace(0, 3, "List");
			s.append(string("private ").append(str).append(" ").append(fld->getName()).append(";\n"));
		}
		else
		{
			s.append(string("private ").append(CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj)).append(" ").append(fld->getName()));
			if (fld->isArray())
			{
				s.append(" = new ").append(CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj, true, to_string(fld->getFieldType()->getArraySize())));
			}
			s.append(";\n");
		}
		gFields.push_back(move(s));
		s.clear();


		gGetters.push_back(bcls->fillGetters(fld, mainObj));
		gSetters.push_back(bcls->fillSetters(fld, mainObj));


		s = fld->getBaseContainerType(CGJbasecls::typeCppToJava);
		if ((s == "Pair") || (fld->getFieldTypeStr() == "CommExtStringPairVector") || (fld->getFieldTypeStr() == "CommExtStringUINT32PairVector") || (fld->getFieldTypeStr() == "CommExtStringUINT32UINT32Vector"))
		{
			gImport.insert("com.ps.cesar.model.Pair");
		}
		if ((fld->getFieldType()->getJavaType() == "CommExtStringPairVector") || (fld->getFieldType()->getJavaType() == "CommExtStringUINT32PairVector") || (fld->getFieldType()->getJavaType() == "CommExtStringUINT32UINT32Vector"))
		{
			gImport.insert("com.ps.cesar.model.Pair");
		}
		//else if ((s == "List") || ((s == "Set") && (!refCls.empty())))
		//{
		//	gImport.insert("java.util.*");
		//}
		//else if (s == "Set")
		//{
		//	gImport.insert("java.util.*");
		//}
		//else if (s == "Map")
		//{
		//	gImport.insert("java.util.*");
		//}
		//else if (s == "Date")
		//{
		//	gImport.insert("java.util.*");
		//}

		//s = fld->getFieldTypeStr(CGJbasecls::typeCppToJava);
		//if (s == "Date")
		//{
		//	gImport.insert("java.util.*");
		//}
		auto cls = (CGstruct*)mainObj->findObject(objTypes::gTypeStruct, fld->getFieldType()->getFieldTypeStr());
		if (cls)
		{
			string simp = "com.ps.cesar.model.";
			simp.append(cls->getJavaName().empty() ? cls->getName() : cls->getJavaName());
			gImport.insert(simp);
		}


		if(get<2>(it->second))
		{
			gParse.push_back(bcls->fillParse(fld, fld->getFieldType(), fld->getJavaName(), mainObj));
		}
		if(get<1>(it->second))
		{
			gCompose.push_back(bcls->fillCompose(fld, fld->getFieldType(), fld->getJavaName(), mainObj));
		}
	}
	return key;
}

void CGJjavamain::clear(void)
{
	gImport.clear();
	gFields.clear();
	gSetters.clear();
	gGetters.clear();
	gParse.clear();
	gCompose.clear();
}


void CGJjavamain::createJavaQuery(const string& nameDoc, const CGbaseobj* p)
{
	string output;
	CGfile cfile;

	const CGstruct* cls = dynamic_cast<const CGstruct*>(p);
	if (!cls)
	{
		string err;
		err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' object-'").append(p->getJavaName());
		throw exception(err.c_str());
	}
	string name = cls->getJavaName().empty() ? cls->getName() : cls->getJavaName();

	if (!cls->isJavaSearhable()) return;

	clear();

	for (auto& elem : cls->getObjects())
	{
		if (elem->getObjType() == objTypes::gTypeField)
		{
			CGfield* fld = dynamic_cast<CGfield*>(elem.get());
			if (!fld)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' class-'").append(cls->getJavaName()).append("' object-'").append(elem->getJavaName());
				throw exception(err.c_str());
			}

			unique_ptr<CGJbasecls> bcls;
			bcls = CGJbasecls::getBaseCls(fld->getFieldType()->getJavaType().empty() ? fld->getFieldType()->getFieldTypeStr() : fld->getFieldType()->getJavaType());
			fillQuery(fld, bcls.get());

		}
	}
	if (gFields.empty()) return;

	if (mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	else
	{
		auto z = nameDoc.find_last_of("\\");
		if (z == string::npos)
			z = nameDoc.find_last_of("/");
		if (z != string::npos)
			output.assign(nameDoc.substr(0, z + 1));
	}

	if (!cls->getJavaPackage().empty())
	{
		if (output.empty())
			output.append(".\\");
		for (auto& elem : cls->getJavaPackage())
		{
			output.append(elem).append("\\");
			mkdir(output.c_str());
		}
		output.append("query\\");
		mkdir(output.c_str());
	}

	output.append(name).append("Query.java");
	cfile.open(output);
	output.clear();

	output.append(COPYRIGHT);
	cfile.write(output, true);

	if (cls->getJavaPackage().empty())
	{
		auto it = mainObj->iniInfo.find("JAVA_PACKAGE");
		if (it != end(mainObj->iniInfo))
		{
			for (auto& elem : it->second)
			{
				output.append("package ").append(elem.first.c_str()).append(";\n");
			}
		}
		output.append("\n\n");
	}
	else
	{
		output.append("package ");
		for (auto i = 0; i < cls->getJavaPackage().size(); ++i)
		{
			if (i > 0) output.append(".");
			output.append(cls->getJavaPackage()[i]);
		}
		output.append(".query;\n\n");
	}

	for (auto& elem : gImport)
	{
		output.append("import ").append(elem).append(";\n");
	}
	{
		auto it = mainObj->iniInfo.find("JAVA_IMPORT");
		if (it != end(mainObj->iniInfo))
		{
			for (auto& elem : it->second)
			{
				output.append("import ").append(elem.first).append(";\n");
			}
		}
	}

	output.append("import java.util.*;\n");
	output.append("import com.ps.cesar.enums.*;\n");
	output.append("import com.ps.cesar.api.*;\n");
	output.append("import com.ps.cesar.annotation.*;\n");
	output.append("import com.ps.commlib2a.transfer.*;\n");
	output.append("import io.leangen.graphql.annotations.*;\n");
	output.append("import com.ps.cesar.model.*;\n");
	output.append("\n");


	output.append("public class ").append(name).append("Query extends BaseQuery<").append(cls->getJavaName()).append(">");

	if(!cls->getJavaQueryImplements().empty())
	{
		output.append(" implements ");
		for (auto i = 0; i < cls->getJavaQueryImplements().size(); ++i)
		{
			if (i > 0) output.append(",");
			output.append("QueryWith").append(cls->getJavaQueryImplements()[i]).append("<").append(cls->getJavaName()).append(">");
		}
	}

	output.append(" {\n");

	for (auto& elem : gFields)
	{
		makeTabMargin(output, 1);
		output.append(elem);
	}
	output.append("\n");
	cfile.write(output, true);

	if (!cls->getJavaQueryImplements().empty())
	{
		for (auto i = 0; i < cls->getJavaQueryImplements().size(); ++i)
		{
			makeTabMargin(output, 1);
			output.append("private ").append(cls->getJavaQueryImplements()[i]).append(" value").append(cls->getJavaQueryImplements()[i]).append(";\n");
		}
	}
	output.append("\n");
	cfile.write(output, true);

	makeTabMargin(output, 1);
	output.append("@Override\n");
	makeTabMargin(output, 1);
	output.append("public Class<").append(name).append("> getModelClass() {\n");
	makeTabMargin(output, 2);
	output.append("return ").append(name).append(".class;\n");
	makeTabMargin(output, 1);
	output.append("}\n");

	output.append("\n");
	makeTabMargin(output, 1);
	output.append("@Override\n");
	makeTabMargin(output, 1);
	output.append("public Class<").append(name).append("Query> getQueryClass() {\n");
	makeTabMargin(output, 2);
	output.append("return ").append(name).append("Query.class;\n");
	makeTabMargin(output, 1);
	output.append("}\n");
	output.append("\n");

	cfile.write(output, true);

	if (!cls->getJavaQueryImplements().empty())
	{
		for (auto i = 0; i < cls->getJavaQueryImplements().size(); ++i)
		{
			makeTabMargin(output, 1);
			output.append("@Override\n");
			makeTabMargin(output, 1);
			output.append("public ").append(cls->getJavaQueryImplements()[i]).append(" get").append(cls->getJavaQueryImplements()[i]).append("() {\n");
			makeTabMargin(output, 2);
			output.append("return value").append(cls->getJavaQueryImplements()[i]).append(";\n");
			makeTabMargin(output, 1);
			output.append("}\n");

			output.append("\n");
			makeTabMargin(output, 1);
			output.append("@Override\n");
			makeTabMargin(output, 1);
			output.append("public void set").append(cls->getJavaQueryImplements()[i]).append("(").append(cls->getJavaQueryImplements()[i]).append(" value").append(cls->getJavaQueryImplements()[i]).append(") {\n");
			makeTabMargin(output, 2);
			output.append("this.value").append(cls->getJavaQueryImplements()[i]).append(" = value").append(cls->getJavaQueryImplements()[i]).append(";\n");
			makeTabMargin(output, 1);
			output.append("}\n");
			output.append("\n");
		}
	}

	cfile.write(output, true);


	for (auto& elem : gGetters)
	{
		for (auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	cfile.write(output, true);

	for (auto& elem : gSetters)
	{
		for (auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	cfile.write(output, true);


	output.append("}\n");
	cfile.write(output, true);
}

void CGJjavamain::fillQuery(CGfield* fld, CGJbasecls* bcls)
{
	string s;
	vector<string> buf;

	const map<string, tuple<bool, bool, bool>>& channels = fld->getPcChannels();
	auto it = channels.find("JAVA");
	if (it != end(channels))
	{
		if (fld->getJavaFilterCode() < 0) return;

		if (!fld->getJavaName().empty() || !fld->getJavaDescr().empty())
		{
			s.append("@GraphQLQuery (");

			if (!fld->getJavaName().empty())
			{
				s.append("name = \"").append(fld->getJavaName()).append("\"");
				if (!fld->getJavaDescr().empty())
					s.append(", ");
			}
			if (!fld->getJavaDescr().empty())
			{
				s.append("description = ").append(fld->getJavaDescrCut()).append(". Accepts: ");
				if (!fld->getJavaFilterOperands().empty())
				{
					for (auto i = 0; i < fld->getJavaFilterOperands().size(); ++i)
					{
						if (i > 0) s.append(", ");
						s.append("Operand.").append(fld->getJavaFilterOperands()[i]);
					}
				}
				s.append("\"");
			}

			s.append(")\n");
			gFields.push_back(move(s));
			s.clear();
		}

		s.append("@FilterAttribute(code = ").append(to_string(fld->getJavaFilterCode()));
		if(!fld->getJavaFilterOperands().empty())
		{
			s.append(", operands = {");
			for (auto i = 0; i < fld->getJavaFilterOperands().size(); ++i)
			{
				if (i > 0) s.append(",");
				s.append(" Operand.").append(fld->getJavaFilterOperands()[i]);
			}
			s.append(" }");
		}
		//if (!fld->getJavaName().empty())
		//{
		//	s.append(", targetAttribute = \"").append(fld->getName()).append("\"");
		//}
		s.append(")\n");

		gFields.push_back(move(s));
		s.clear();
		if (fld->getJavaSortCode() >= 0)
		{
			s.append("@SortableAttribute(code = ").append(to_string(fld->getJavaSortCode())).append(")\n");
			gFields.push_back(move(s));
			s.clear();
		}

		if (!fld->getJavaName().empty())
		{
	//		s.append("name = \"").append(fld->getJavaName()).append("\"");
	//		if (!fld->getJavaDescr().empty())
	//			s.append(", ");
		}

		string str;
		str = CGJbasecls::simpleToObj(CGJbasecls::getJavaTypeString(fld, fld->getFieldType(), mainObj, false));
		//auto refCls = fld->getRefClass().first;
		if ((fld->getFieldType()->getIndexedType().length()) && CGJbasecls::isMultFilter(fld->getZeroSubType()->getFieldTypeStr()) && (fld->getFieldType()->getBaseContainerType(CGJbasecls::typeCppToJava) == "List"))
		{
			str.replace(0, 4, "Set");
		}

		s.append("private ");
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
		s.append(str);
		if (!fld->isContainer() && !fld->isArray())
			s.append(">");
		s.append("> ").append(fld->getName()).append(";\n");

		gFields.push_back(move(s));
		s.clear();


		gGetters.push_back(bcls->fillQueryGetters(fld, mainObj));
		gSetters.push_back(bcls->fillQuerySetters(fld, mainObj));


		s = fld->getBaseContainerType(CGJbasecls::typeCppToJava);
		if ((s == "Pair") || (fld->getFieldTypeStr() == "CommExtStringPairVector") || (fld->getFieldTypeStr() == "CommExtStringUINT32PairVector") || (fld->getFieldTypeStr() == "CommExtStringUINT32UINT32Vector"))
		{
			gImport.insert("com.ps.cesar.model.Pair");
		}

	}
}


