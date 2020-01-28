// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genCpp.h"
#include <algorithm>

void CGGcppmain::createCppFiles(void)
{
	const map<string, string>* psec = mainObj->findIniSection("CPP_SUPPRESS");
	for(const auto& docp : mainObj->documents)
	{
		if (psec)
		{
			auto res = psec->find(docp.second.getFileName());
			if (res != end(*psec))
			{
				continue;
			}
		}
		createCppDocument(docp.first, docp.second);
	}
	for(const auto& bcls : mainObj->baseClasses)
	{
		if (psec)
		{
			auto res = psec->find(bcls.first);
			if (res != end(*psec))
			{
				continue;
			}
		}
		createBaseClass(bcls);
	}
	//createFieldNumAccess();
}

inline void CGGcppmain::createFieldNumAccess(void)
{
	CGfile cfile;
	string output;
	const string* p_inifilename = nullptr;

	const auto itiniInfo1 = mainObj->iniInfo.find("ENTER");
	if(itiniInfo1 != end(mainObj->iniInfo))
	{
		const auto itiniInfo2 = itiniInfo1->second.find("fieldnumfile");
		if(itiniInfo2 != end(itiniInfo1->second))
			p_inifilename = &(itiniInfo2->second);
	}

	// H file
	if(mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	if(p_inifilename)
	{
		output.append(*p_inifilename).append(".h");
	}
	else
	{
		output.append("FieldNumAccess.h");
	}
	cfile.open(output);
	output.clear();

	if(p_inifilename)
	{
		output.append(COPYRIGHT"#ifndef ").append(*p_inifilename).append("_h_included\n#define ").append(*p_inifilename).append("_h_included\n\n");
	}
	else
	{
		output.append(COPYRIGHT"#ifndef FieldNumAccess_h_included\n#define FieldNumAccess_h_included\n\n");
	}
	cfile.write(output, true);

	auto it = mainObj->iniInfo.find("FIELD_NUM_H_INCLUDE");
	if(it != end(mainObj->iniInfo))
	{
		for(auto& elem : it->second)
		{
			output.append("#include \"").append(elem.second.c_str()).append("\"\n");
		}
	}
	output.append("\n");

	output.append("//************************************************************************************************\n");
	output.append("// This file contains declaration of functions for classes field information by fields enumeration\n");
	output.append("//************************************************************************************************\n");
	output.append("// WARNING!!! All class field operations are valid ONLY for PUBLIC fields.\n");
	output.append("//************************************************************************************************\n\n");
	output.append("class UpdateParameterHandler;\n\n");

	output.append("// Data structure for a class field\n");
	output.append("struct ClassesNumOffsetDataType\n");
	output.append("{\n");
	output.append("\tClassesNumOffsetDataType(const char* _fullfieldtype, const char* _fieldname, const char* _containertype, UINT32 _offset, UpdateParameterHandler * _handler)\n");
	output.append("\t:\tfullfieldtype(_fullfieldtype)\n");
	output.append("\t,\tfieldname(_fieldname)\n");
	output.append("\t,\tcontainertype(_containertype)\n");
	output.append("\t,\toffset(_offset)\n");
	output.append("\t,\thandler(_handler)\n");
	output.append("\t{}\n");
	output.append("\tconst char*	fullfieldtype;\n");
	output.append("\tconst char*	fieldname;\n");
	output.append("\tconst char*	containertype;\n");
	output.append("\tUINT32		offset;\n");
	output.append("\tUpdateParameterHandler * handler;\n");
	output.append("};\n\n");

	output.append("// Initialization. Must be called ones at start.\n");
	output.append("void initClassNumInventory(void);\n");
	output.append("// Get class field info. Returns pointer to ClassesNumOffsetDataType object if the result is valid or null.\n");
	output.append("const ClassesNumOffsetDataType* getFieldNumInfo(const char* classname, const char* fieldname);\n");
	output.append("// Get class field offset. Returns true if the field was found and the result is valid.\n");
	output.append("bool getFieldNumOffset(UINT32& offset, const char* classname, const char* fieldname);\n");
	output.append("// Returns class field direct pointer or null if not found\n");
	output.append("// 'void* obj' is an input pointer for the object which field is used.\n");
	output.append("void* getFieldNumAddress(void* obj, const char* classname, const char* fieldname);\n");

	if(p_inifilename)
	{
		output.append("\n#endif // ").append(*p_inifilename).append("_h_included\n");
	}
	else
	{
		output.append("\n#endif // FieldNumAccess_h_included\n");
	}

	cfile.write(output, true);
	cfile.close();

	// CPP file
	if(mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	if(p_inifilename)
	{
		output.append(*p_inifilename).append(".cpp");
	}
	else
	{
		output.append("FieldNumAccess.cpp");
	}
	cfile.open(output);
	output.clear();

	output.assign(COPYRIGHT);

	it = mainObj->iniInfo.find("FIELD_NUM_CPP_INCLUDE");
	if(it != end(mainObj->iniInfo))
	{
		for(auto& elem : it->second)
		{
			output.append("#include \"").append(elem.second.c_str()).append("\"\n");
		}
	}
	output.append("\n");

	output.append("//***********************************************************************************************\n");
	output.append("// This file contains definition of functions for classes field information by fields enumeration\n");
	output.append("//***********************************************************************************************\n");
	output.append("// WARNING!!! All class field operations are valid ONLY for PUBLIC fields.\n");
	output.append("//***********************************************************************************************\n\n");

	output.append("// Main container with information\n");
	output.append("static PStringMap<PStringMap<ClassesNumOffsetDataType>*> classesNumOffsets;\n\n");

	output.append("// Get class field info. Returns pointer to ClassesNumOffsetDataType object if the result is valid or null.\n");
	output.append("const ClassesNumOffsetDataType*  getFieldNumInfo(const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tconst ClassesNumOffsetDataType* codt = NULL;\n");
	output.append("\tPStringMap<ClassesNumOffsetDataType>::iterator iter;\n");
	output.append("\tPStringMap<PStringMap<ClassesNumOffsetDataType>*>::iterator iterMap;\n");
	output.append("\titerMap = classesNumOffsets.find(classname);\n");
	output.append("\tif(iterMap != classesNumOffsets.end())\n");
	output.append("\t{\n");
	output.append("\t\titer = iterMap->second->find(fieldname);\n");
	output.append("\t\tif(iter != iterMap->second->end())\n");
	output.append("\t\t{\n");
	output.append("\t\t\tcodt = &(iter->second);\n");
	output.append("\t\t}\n");
	output.append("\t}\n");
	output.append("\treturn codt;\n");
	output.append("}\n\n");

	output.append("// Get class field offset. Returns true if the field was found and the result is valid.\n");
	output.append("bool getFieldNumOffset(UINT32& offset, const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tconst ClassesNumOffsetDataType* codt = getFieldNumInfo(classname, fieldname);\n");
	output.append("\tif(NULL != codt)\n");
	output.append("\t{\n");
	output.append("\t\toffset = codt->offset;\n");
	output.append("\t\treturn true;\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn false;\n");
	output.append("}\n\n");

	output.append("// Returns class field direct pointer. NULL - if not found\n");
	output.append("void* getFieldNumAddress(void* obj, const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tUINT32 offset;\n");
	output.append("\tif((NULL != obj) && (getFieldNumOffset(offset, classname, fieldname)))\n");
	output.append("\t\treturn reinterpret_cast<BYTE*>(obj) + offset;\n");
	output.append("\telse\n");
	output.append("\t\treturn NULL;\n");
	output.append("}\n\n");

	cfile.write(output, true);

	output.append("// Initialization. Must be called ones at start\n");
	output.append("void initClassNumInventory(void)\n");
	output.append("{\n");

	output.append("\tPStringMap<PStringMap<ClassesNumOffsetDataType>*>::iterator iterMap;\n\n");

	for(auto& doc : mainObj->documents)
	{
		for(auto& el : doc.second.getObjects())
		{
			if(el->getObjType() == objTypes::gTypeStruct)
			{
				CGstruct* cls = dynamic_cast<CGstruct*>(el.get());
				if(!cls)
				{
					string err;
					err.append("Wrong dynamic cast to class, file-'").append(doc.second.getName()).append("' class-'").append(el->getName()).append("'");
					throw exception(err.c_str());
				}
				output.append("\t// Class ").append(cls->getName()).append("\n");

				string tmpstr = cls->getName();
				std::transform(tmpstr.begin(), tmpstr.end(), tmpstr.begin(), ::tolower);

				output.append("\tstatic PStringMap<ClassesNumOffsetDataType> ").append(cls->getName()).append("_map;\n");
				//output.append("\titerMap = classesNumOffsets.insert(PStringMap<map<UINT32, ClassesNumOffsetDataType>>::value_type(\"").append(cls->getName()).append("\", map<UINT32, ClassesNumOffsetDataType>())).first;\n\n");
				output.append("\titerMap = classesNumOffsets.insert(PStringMap<PStringMap<ClassesNumOffsetDataType>*>::value_type(\"").append(tmpstr).append("\", &").append(cls->getName()).append("_map)).first;\n\n");

				// fields
				string curAccess = "public";
				for(auto& elem : el->getObjects())
				{
					if(elem->getObjType() == objTypes::gTypeAccess)
						curAccess= elem->getName();
					else if((elem->getObjType() == objTypes::gTypeField) && (curAccess == "public"))
					{
						CGfield* fld = dynamic_cast<CGfield*>(elem.get());
						if(!fld)
						{
							string err;
							err.append("Wrong dynamic cast to field, file-'").append(doc.second.getName()).append("' class-'").append(el->getName()).append("' object-'").append(elem->getName());
							throw exception(err.c_str());
						}
						//output.append("\titerMap->second.insert(map<UINT32, ClassesNumOffsetDataType>::value_type(").append(to_string(fld->getFieldIndex()))
						//	.append(", ClassesNumOffsetDataType(\"").append(fld->getFieldType()->getFullTypeString()).append("\", \"").append(fld->getName()).append("\", \"");
						//output.append(fld->getFieldType()->getBaseContainerType());
						//output.append("\", offsetof(").append(cls->getName()).append(", ").append(fld->getName()).append("))));\n");

						output.append("\titerMap->second->insert(PStringMap<ClassesNumOffsetDataType>::value_type(\"").append(fld->getName())
							.append("\", ClassesNumOffsetDataType(\"").append(fld->getFieldType()->getFullTypeString()).append("\", \"").append(fld->getName()).append("\", \"");
						output.append(fld->getFieldType()->getBaseContainerType());
						output.append("\", offsetof(").append(cls->getName()).append(", ").append(fld->getName()).append("), ");
						if(fld->getUpdateHandler().size())
							output.append("&").append(fld->getUpdateHandler());
						else
							output.append("NULL");
						output.append(")));\n");
					}
				}
				output.append("\n");
				cfile.write(output, true);
			}
		}
	}

	output.append("}\n");


	cfile.write(output, true);
	cfile.close();
}

void CGGcppmain::createBaseClass(const pair<string, pair<map<string, tuple<bool, bool, bool>>, uint64_t>>& bcls)
{
	auto it = mainObj->usedNameOfClass.find(bcls.first);
	if(it == end(mainObj->usedNameOfClass))
	{
		string output;
		CGfile cfile;
		if(mainObj->outdir.length())
		{
			output.assign(mainObj->outdir).append("\\");
		}
		output.append(bcls.first).append(".h");
		cfile.open(output);
		output.clear();

		output.append(COPYRIGHT"#ifndef ").append(bcls.first).append("_h_included\n#define ").append(bcls.first).append("_h_included\n\n");
		cfile.write(output, true);

		output.append("//**********************************\n");
		output.append("// Class ").append(bcls.first);
		output.append("\n");
		output.append("//**********************************\n");

		if (bcls.second.second & CG_CLASS_TEMP_ON)
			output.append("template <T>\nclass ").append(bcls.first).append(": public T\n");
		else
			output.append("class ").append(bcls.first).append("\n");

		output.append("{\n");
		output.append("public:\n");
		cfile.write(output, true);

		//destructor
		output.append("\tvirtual ~").append(bcls.first).append("(void) {};\n");

		// compare
		if (bcls.second.second & CG_CLASS_EQUAL_BASE)
			output.append("\tvirtual bool equals(const ").append(bcls.first).append("& other) const {return true;};\n");

		// copy from
		//if (bcls.second.second & CG_CLASS_COPYBASE_ON)
		if (bcls.second.second & CG_CLASS_ASSIGN_ON)
			output.append("\tvirtual void copyFrom(const ").append(bcls.first).append("& other) = 0;\n");

		// move from
		if (bcls.second.second & CG_CLASS_MOVEBASE_ON)
			output.append("\tvirtual void moveFrom(").append(bcls.first).append("& other) = 0;\n");

		// update
		if (bcls.second.second & CG_CLASS_UPDATE_ON)
		{
			output.append("#ifdef CE_FIELDS_UPDATE\n");
			output.append("\tvirtual bool fieldsUpdate(CommMsgParser & parser, const void* secure_param, void* param1, void* param2, PString& errMsg, PStringSet& fieldsUpdate_fields) = 0;\n");
			output.append("#endif // CE_FIELDS_UPDATE\n");
		}

		if (mainObj->baseRefClass.size() == 0)
			throw exception(string("Empty base reference class").c_str());

		output.append("\tvirtual void composeSelectiveFieldsCounted(CommExtUINT32Vector& path, CommMsgBody& data, size_t& path_pointer, size_t amount, const ");
		output.append(mainObj->baseRefClass);
		output.append("* (*getRefObject)(const char* type, const char* strId, UINT32 intId, void* param1, void* param2), void* param1, void* param2) const{}\n");

		for(auto& el : bcls.second.first)
		{
			if(el.first != "default")
			{
				if(std::get<0>(el.second)) // smart
				{
					if(std::get<1>(el.second))
					{
						makeTabMargin(output, 1);
						output.append("virtual void parseFrom").append(el.first).append("(CommMsgParser& parser, size_t _ce__ext_size) = 0;\n");
						cfile.write(output, true);
					}
					if(std::get<2>(el.second))
					{
						makeTabMargin(output, 1);
						output.append("virtual void composeFor").append(el.first).append("(CommMsgBody& data) const = 0;\n");
						cfile.write(output, true);
					}
				}
				else
				{
					if(std::get<1>(el.second))
					{
						makeTabMargin(output, 1);
						output.append("virtual void parseFrom").append(el.first).append("(CommMsgParser& parser) = 0;\n");
						cfile.write(output, true);
					}
					if(std::get<2>(el.second))
					{
						makeTabMargin(output, 1);
						output.append("virtual void composeFor").append(el.first).append("(CommMsgBody& data) const = 0;\n");
						cfile.write(output, true);
					}
				}
			}
			else
			{
				if(std::get<1>(el.second))
				{
					makeTabMargin(output, 1);
					output.append("virtual void parse").append("(CommMsgParser& parser) = 0;\n");
					cfile.write(output, true);
				}
				if(std::get<2>(el.second))
				{
					makeTabMargin(output, 1);
					output.append("virtual void compose").append("(CommMsgBody& data) const = 0;\n");
					cfile.write(output, true);
				}
			}
		}
		output.append("};\n\n");
		output.append("#endif // ").append(bcls.first).append("_h_included\n");
		cfile.write(output, true);
	}
}

void CGGcppmain::createCppDocument(const string&nameDoc, const CGdocument& doc)
{
	string output;
	CGfile cfile;
	string name = doc.getName();

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
	output.append(name).append(".h");
	cfile.open(output);
	output.clear();

	output.append(COPYRIGHT"#ifndef ").append(name).append("_h_included\n#define ").append(name).append("_h_included\n\n");
	cfile.write(output, true);

	for(auto& elem : doc.getObjects())
	{
		switch(elem->getObjType())
		{
		case objTypes::gTypeStruct:
			{
				createCppClass(nameDoc, elem.get(), cfile);
				break;
			}
		case objTypes::gTypeInclude:
			{
				output.append("#include ").append(elem->getTxt());
				cfile.write(output, true);
				break;
			}
		case objTypes::gTypeText:
			{
				if((!elem->getName().length()) || (elem->getName() == "genCpp")) 
				cfile.write(elem->getTxt());
				break;
			}
		case objTypes::gTypeComment:
			{
				cfile.write(elem->getTxt());
				break;
			}
		case objTypes::gTypeEmptyLine:
			{
				output.insert(0, stoi(elem->getName()), '\n');
				cfile.write(output, true);
				break;
			}
		case objTypes::gTypeAccess:
		case objTypes::gTypeField:
		case objTypes::dTypeDocument:
		case objTypes::gTypeUnknown:
			{
				string err;
				err.append("Unsupported type of document's object, file-'").append(nameDoc).append("' object-'").append(doc.getName()).append("' type-'").append(to_string((int)elem->getObjType())).append("' - ignored");
				WriteLog(err);
				break;
			}
		default:
			{
				string err;
				err.append("Undefined type of document's object, file-'").append(nameDoc).append("' object-'").append(doc.getName()).append("' type-'").append(to_string((int)elem->getObjType()));
				throw exception(err.c_str());
			}
		};
	}

	output.append("#endif // ").append(name).append("_h_included\n");
	cfile.write(output, true);
}

void CGGcppmain::createCppClass(const string&nameDoc, const CGbaseobj* p, CGfile& cfile)
{
	string output;
	string curAccess;
	set<string> decls;

	const CGstruct* cls = dynamic_cast<const CGstruct*>(p);
	if(!cls)
	{
		string err;
		err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' object-'").append(p->getName());
		throw exception(err.c_str());
	}

	clear();
	for(auto& elem : cls->getObjects())
	{
		if(elem->getObjType() == objTypes::gTypeField)
		{
			const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());
			if(!fld)
			{
				string err;
				err.append("Wrong dynamic cast to class, file-'").append(nameDoc).append("' class-'").append(cls->getName()).append("' object-'").append(elem->getName());
				throw exception(err.c_str());
			}

			if (cls->getProperty() & CG_CLASS_PTRS_SMART_RESTR)
			{
				if(cls->getProperty() & CG_CLASS_COPY_ON)
				{
					throw exception(string("Using copy constructor for pointer member in document '").append(nameDoc).append(" class '").append(cls->getName()).append("' - forbidden").c_str());
				}
				if(cls->getProperty() & CG_CLASS_ASSIGN_ON)
				{
					throw exception(string("Using assignment operator for pointer member in document '").append(nameDoc).append(" class '").append(cls->getName()).append("' - forbidden").c_str());
				}
			}

			if(!(fld->getProperty() & CG_CPP_IGNORE_ON))
			{
				fill(fld, cls, decls);
			}
		}
	}

	if(decls.size())
	{
		output.append("class ").append(cls->getName()).append(";\n");
	}
	for(auto elem : decls)
	{
		if(elem.size())
			output.append(elem);
	}



	// create header
	output.append("//**********************************\n");
	output.append("// Class ").append(cls->getName());
	output.append("\n");
	output.append("//**********************************\n");


	if (cls->getProperty() & CG_CLASS_TEMP_ON)
	{
		output.append("template <class T>\n");
	}
	output.append("class ").append(cls->getName());

	if (cls->getProperty() & CG_CLASS_INH_BASE)
	{
		{
			output.append(" : public ").append(cls->getBaseClass());
			if (cls->getProperty() & CG_CLASS_TEMP_ON)
				output.append("<T>");
		}
	}
	else
	{
		if (cls->getProperty() & CG_CLASS_TEMP_ON)
			output.append(" : public T");
	}

	output.append("\n");
	if(cls->getTxt().length())
		output.append(cls->getTxt());
	output.append("{\n");
	cfile.write(output, true);
	writeAccess(curAccess, "public", cfile);

// Default constructor
	writeDefConstructor(cls, cfile);

// Destructor
	writeDestructor(cls, cfile);

// Copy constructor
	if (cls->getProperty() & CG_CLASS_COPY_ON)
	{
		writeCopyConstructor(cls, cfile);
	}

// Move constructor
	if (cls->getProperty() & CG_CLASS_MOVE_ON)
	{
		writeMovConstructor(cls, cfile);
	}

// Assignment operator
	if (cls->getProperty() & CG_CLASS_ASSIGN_ON)
	{
		writeAssignment(cls, cfile);
	}

// Move assignment operator
	if (cls->getProperty() & CG_CLASS_MASSIGN_ON)
	{
		writeMovAssignment(cls, cfile);
	}

// Forbidden section
	if((cls->getProperty() & CG_CLASS_COPY_DENY) || (cls->getProperty() & CG_CLASS_ASSIGN_DENY) || (cls->getProperty() & CG_CLASS_PTRS_SMART_RESTR) || (cls->getProperty() & CG_CLASS_MOVE_DENY) || (cls->getProperty() & CG_CLASS_MASSIGN_DENY))
	{
		writeAccess(curAccess, "private", cfile);
		writeForbidden(cls, cfile);
	}

// Cast to base class
	//if((cls->getProperty() & CG_CLASS_INH_VIRT) && ((cls->getProperty() & CG_CLASS_COPYBASE_ON)||  (cls->getProperty() & CG_CLASS_MOVEBASE_ON) || (cls->getProperty() & CG_CLASS_EQUAL_ON)))
	if((cls->getProperty() & CG_CLASS_INH_VIRT) && ((cls->getProperty() & CG_CLASS_ASSIGN_ON)||  (cls->getProperty() & CG_CLASS_MOVEBASE_ON) || (cls->getProperty() & CG_CLASS_EQUAL_ON)))
	{
		writeAccess(curAccess, "private", cfile);
		writeCast(cls, cfile);
	}

// Copy from
	//if ((cls->getProperty() & CG_CLASS_INH_VIRT) && (cls->getProperty() & CG_CLASS_COPYBASE_ON))
	if ((cls->getProperty() & CG_CLASS_INH_VIRT) && (cls->getProperty() & CG_CLASS_ASSIGN_ON))
	{
		writeAccess(curAccess, "public", cfile);
		writeCopy(cls, cfile);
	}

// Move from
	if ((cls->getProperty() & CG_CLASS_INH_VIRT) && (cls->getProperty() & CG_CLASS_MOVEBASE_ON))
	{
		writeAccess(curAccess, "public", cfile);
		writeMove(cls, cfile);
	}

// Compare
	if ((cls->getProperty() & CG_CLASS_EQUAL_ON) || (cls->getProperty() & CG_CLASS_EQUAL_BYFIELD)) 
	{
		writeAccess(curAccess, "public", cfile);
		writeCompare(cls, cfile);
	}

// Getters/Setters
	if(gGetters.size() || gSetters.size() || gMoveSetters.size())
	{
		writeAccess(curAccess, "public", cfile);
		writeGetters(cfile);
		writeSetters(cfile);
		writeMovSetters(cfile);
	}

// Fields and etc
	if(cls->getObjects().size())
	{
		output.append("// Members\n");
		cfile.write(output, true);
		for(auto& elem : cls->getObjects())
		{
			switch(elem->getObjType())
			{
			case objTypes::gTypeField:
				{

					const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());
					if(!fld)
					{
						string err;
						err.append("Wrong dynamic cast to field, file-'").append(nameDoc).append("' class-'").append(cls->getName()).append("' object-'").append(elem->getName());
						throw exception(err.c_str());
					}
					if(!(fld->getProperty() & CG_CPP_IGNORE_ON))
					{
						writeField(cls, fld, cfile);
					}
					break;
				}
			case objTypes::gTypeAccess:
				{
					writeAccess(curAccess, elem->getName(), cfile);
					break;
				}
			case objTypes::gTypeText:
				{
					if((!elem->getName().length()) || (elem->getName() == "genCpp")) 
						cfile.write(elem->getTxt());
					break;
				}
			case objTypes::gTypeComment:
				{
					cfile.write(elem->getTxt());
					break;
				}
			case objTypes::gTypeEmptyLine:
				{
					output.insert(0, stoi(elem->getName()), '\n');
					cfile.write(output, true);
					break;
				}
			case objTypes::gTypeStruct:
			case objTypes::gTypeInclude:
			case objTypes::dTypeDocument:
			case objTypes::gTypeUnknown:
				{
					output.assign("Unsupported type of class object, file-'").append(nameDoc).append("' class-'").append(cls->getName()).append("' object-'").append(elem->getName()).append("' type-'").append(to_string((int)elem->getObjType())).append("' - ignored");
					WriteLog(output);
					break;
				}
			default:
				{
					output.assign("Undefined type of class object, file-'").append(nameDoc).append("' class-'").append(cls->getName()).append("' object-'").append(elem->getName()).append("' type-'").append(to_string((int)elem->getObjType()));
					throw exception(output.c_str());
				}
			};
		}
		output.append("\n");
		cfile.write(output, true);
	}


// Subscription
	if (cls->getProperty() & CG_CLASS_SUBSCR_LEAFNODE)
	{
		writeAccess(curAccess, "public", cfile);
		writeSubscription(cls, cfile);
	}

// Parsers
	if(cls->getPcChannels().size())
	{
		writeAccess(curAccess, "public", cfile);
		writeParseCompose(cls, cfile);
		output.append("\n");
		cfile.write(output, true);
	}


// Subscription
	if(cls->getProperty() & CG_CLASS_SUBSCR_LEAFNODE)
	{
		writeAccess(curAccess, "public", cfile);
		writePublish(cls, cfile);
		writePublishUpdate(cls, cfile);
	}

	if(cls->getProperty() & CG_CLASS_SELFIELD_ON)
	{
		output.append("#ifdef CE_FRONTEND_SUPPORT\n");
		cfile.write(output, true);
		writeAccess(curAccess, "public", cfile);
		writeSelField1(cls, cfile);
		writeAccess(curAccess, "private", cfile);
		writeSelField2(cls, cfile);
		output.append("#endif // CE_FRONTEND_SUPPORT\n\n");
		cfile.write(output, true);
	}

	if(cls->getProperty() & CG_CLASS_SPECIAL_1)
	{
		writeAccess(curAccess, "public", cfile);
		fillSpecial1(cls, cfile);
	}

	if(cls->getProperty() & CG_CLASS_UPDATE_ON)
	{
		writeAccess(curAccess, "public", cfile);
		writeUpdate(cls, cfile);
	}


	if (cls->getProperty() & CG_JN_ON)
	{
		writeAccess(curAccess, "public", cfile);
		output.append("\tvirtual void composeJSONstring(PString& output, PString fieldname = \"\", UINT32 margin = 0)\n\t{\n");
		output.append("\t\tPString margin_string;\n");
		output.append("\t\tfor (UINT32 i = 0; i < margin; ++i)\n");
		output.append("\t\t\tmargin_string.append(\"  \");\n");
		output.append("\t\tif(fieldname.length())\n");
		output.append("\t\t\toutput.append(margin_string).append(\"  \").append(fieldname).append(\": {\\n\");\n");
		output.append("\t\telse\n");
		output.append("\t\t\toutput.append(margin_string).append(\"  {\\n\");\n");

		bool last = false;
		auto& cont = cls->getObjects();
		//for (auto& elem : cls->getObjects())

		auto itl = cont.end();
		for (auto itt = cont.begin(); itt != cont.end(); ++itt)
		{
			auto& elem = *itt;
			if (elem->getObjType() == objTypes::gTypeField)
			{
				itl = itt+1;
			}
		}

		for (auto it = cont.begin(); it != itl; ++it)
		{
			auto& elem = *it;
			if (elem->getObjType() == objTypes::gTypeField)
			{
				const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());

				if (!(fld->getProperty() & CG_CPP_IGNORE_ON))
				{
					string s;
					if (it + 1 == itl)
						last = true;
					unique_ptr<CGGbasecls> bcls;
					bcls = CGGbasecls::getBaseCls(fld->getFieldTypeStr());
					s = bcls->fillFieldJson(fld->getFieldType(), fld->getName(), fld->getName(), 2, 2, 1, last);
					output.append(s);
				}
			}
		}
		output.append("\t\toutput.append(margin_string).append(\"  }\\n\");\n\t}\n");

		output.append("\tvirtual void parseJSONstring(CommExtJson *pjson)\n\t{\n");

		output.append("\t\tfor(auto& elem0 : pjson->v)\n");
		output.append("\t\t{\n");

		for (auto it = cont.begin(); it != itl; ++it)
		{
			auto& elem = *it;
			if (elem->getObjType() == objTypes::gTypeField)
			{
				const CGfield* fld = dynamic_cast<const CGfield*>(elem.get());

				if (!(fld->getProperty() & CG_CPP_IGNORE_ON))
				{
					string s;
					output.append("\t\t\t");
					if (it != cont.begin())
						output.append("else ");
					unique_ptr<CGGbasecls> bcls;
					bcls = CGGbasecls::getBaseCls(fld->getFieldTypeStr());
					output.append("if(elem0->name.equals(\"").append(fld->getName()).append("\"))\n");
					output.append("\t\t\t{\n");
					s = bcls->getFieldJson(fld, fld->getFieldType(), fld->getName(), 4, 0);
					output.append(s);
					output.append("\t\t\t}\n");
				}
			}
		}

		output.append("\t\t}\n");
		output.append("\t}\n");

		output.append("\tvirtual void parseJSONstring(PString& pstr)\n\t{\n");
		output.append("\t\tCommExtJson cejson;\n");
		output.append("\t\tconst char *strn = pstr.c_str();\n");
		output.append("\t\tcejson.load(strn);\n");
		output.append("\t\tparseJSONstring(&cejson);\n");

		output.append("\t}\n");
	}


	output.append("};\n\n");
	cfile.write(output, true);
}

inline void CGGcppmain::fill(const CGfield* fld, const CGstruct* cls, set<string>& decls)
{
	string s;
	vector<string> buf;

	unique_ptr<CGGbasecls> bcls;
	bcls = CGGbasecls::getBaseCls(fld->getFieldTypeStr());

	s = bcls->fillDefaultConctructorHeader(fld);
	if(s.length()) gDefaultConctructorHeader.push_back(move(s));
	buf = bcls->fillDefaultConctructorBody(fld);
	if(buf.size()) gDefaultConctructorBody.push_back(move(buf));

	if (cls->getProperty() & CG_CLASS_COPY_ON)
	{
		s = bcls->fillCopyConctructorHeader(fld);
		if(s.length()) gCopyConctructorHeader.push_back(move(s));
		buf = bcls->fillCopyConctructorBody(fld);
		if(buf.size()) gCopyConctructorBody.push_back(move(buf));
	}
	if (cls->getProperty() & CG_CLASS_MOVE_ON)
	{
		s = bcls->fillMoveConctructorHeader(fld);
		if(s.length()) gMoveConctructorHeader.push_back(move(s));
		buf = bcls->fillMoveConctructorBody(fld);
		if(buf.size()) gMoveConctructorBody.push_back(move(buf));
	}
	if (cls->getProperty() & CG_CLASS_ASSIGN_ON)
	{
		buf = bcls->fillAssignment(fld, fld->getName(), string("other.").append(fld->getName()));
		if(buf.size()) gAssignment.push_back(move(buf));
	}
	if (cls->getProperty() & CG_CLASS_MASSIGN_ON)
	{
		buf = bcls->fillMoveAssignment(fld, fld->getName(), string("other.").append(fld->getName()));
		if(buf.size()) gMoveAssignment.push_back(move(buf));
	}

	if (((fld->getProperty() & CG_CLASS_EQUAL_ON) || ((cls->getProperty() & CG_CLASS_EQUAL_BYFIELD) && (fld->getProperty() & CG_CLASS_EQUAL_ON))))		
	{
		buf = bcls->fillEqual(0, fld, fld->getFieldType(), fld->getName(), string("other.")+fld->getName(), "return false;", true, mainObj);
		if(buf.size()) gEqual.push_back(move(buf));
	}

	if(fld->getGetter().length())
	{
		buf = bcls->fillGetters(fld);
		if(buf.size()) gGetters.push_back(move(buf));
	}
	if(fld->getSetter().length())
	{
		buf = bcls->fillSetters(fld);
		if(buf.size()) gSetters.push_back(move(buf));
	}
	if(fld->getMoveSetter().length())
	{
		buf = bcls->fillMoveSetters(fld);
		if(buf.size()) gMoveSetters.push_back(move(buf));
	}

	const map<string, tuple<bool,bool,bool>>& channels =  fld->getPcChannels();
	bool dum = false;
	for(auto& elem : channels)
	{
		if (elem.first == "JAVA")
			continue;
		if(get<0>(elem.second))
		{
			if(get<1>(elem.second))
			{
				auto res1 = gParse.insert(make_pair(elem.first, vector<pair<string, vector<string>>>())).first;
				res1->second.push_back(make_pair(fld->getName(), bcls->fillParseSmart(fld, fld->getFieldType(), fld->getName(), elem.first, mainObj, fld->isArray(), dum)));
			}
			if(get<2>(elem.second))
			{
				auto res2 = gCompose.insert(make_pair(elem.first, vector<pair<string, vector<string>>>())).first;
				res2->second.push_back(make_pair(fld->getName(), bcls->fillComposeSmart(fld, fld->getFieldType(), fld->getName(), elem.first, mainObj, fld->isArray(), dum)));
			}
		}
		else
		{
			if(get<1>(elem.second))
			{
				auto res1 = gParse.insert(make_pair(elem.first, vector<pair<string, vector<string>>>())).first;
				res1->second.push_back(make_pair(fld->getName(), bcls->fillParse(fld, fld->getFieldType(), fld->getName(), elem.first, mainObj, fld->isArray(), dum)));
			}
			if(get<2>(elem.second))
			{
				auto res2 = gCompose.insert(make_pair(elem.first, vector<pair<string, vector<string>>>())).first;
				res2->second.push_back(make_pair(fld->getName(), bcls->fillCompose(fld, fld->getFieldType(), fld->getName(), elem.first, mainObj, fld->isArray(), dum)));
				if(elem.first == "default")
				{
					if(fld->getSpecialFieldFuncName().length())
					{
						buf.push_back(string("").append(fld->getSpecialFieldFuncName()).append("(data, this);\n"));
						gSelConvertCompose.push_back(make_pair(fld->getName(), move(buf)));
					}
					else
					{
						gSelConvertCompose.push_back(make_pair(fld->getName(), bcls->fillSelConvertCompose(fld, fld->getFieldType(), fld->getName(), mainObj)));
					}
				}
			}
		}
	}

	if(fld->getProperty() & CG_CLASS_SUBSCR_LEAFNODE)
	{
		vector<string> path;
		buf = bcls->fillPublisher(fld, fld->getFieldType(), mainObj, to_string(fld->getLeafNodeIndex()), fld->getName(), 0);
		if(buf.size()) gPublisher.push_back(move(buf));
		if(!(cls->getProperty() & CG_CLASS_SPECIAL_1))
		{
			buf = bcls->fillSubscrUpdateAll(fld, fld->getFieldType(), mainObj, to_string(fld->getLeafNodeIndex()), fld->getName());
			if(buf.size()) gSubscrUpdateAll.push_back(move(buf));
			buf = bcls->fillSubscrUpdate(fld, fld->getFieldType(), mainObj, to_string(fld->getLeafNodeIndex()));
			if(buf.size()) gSubscrSetters.push_back(move(buf));
			buf = bcls->fillSubscrAdd(fld, fld->getFieldType(), mainObj, to_string(fld->getLeafNodeIndex()));
			if(buf.size()) gSubscrSetters.push_back(move(buf));
			buf = bcls->fillSubscrRemove(fld, fld->getFieldType(), mainObj, to_string(fld->getLeafNodeIndex()));
			if(buf.size()) gSubscrSetters.push_back(move(buf));
		}
	}

	if(!(fld->getProperty() & CG_FIELD_HIDDEN_ON)) 
	{
		if(fld->getProperty() & CG_CLASS_SELFIELD_ON)
		{
			if(fld->getSpecialFieldFuncName().length())
			{
				buf.push_back(string("case ").append(to_string(fld->getFieldIndex())).append(":\n"));
				buf.push_back(string("\t{\n"));
				buf.push_back(string("\t\t").append(fld->getSpecialFieldFuncName()).append("(data, this);\n"));
				buf.push_back(string("\t}\n"));
				buf.push_back(string("\tbreak;\n"));
				if(buf.size()) gSelFields.push_back(move(buf));
				if(fld->getSpecialFieldFuncName().size())
					decls.insert(string("void ").append(fld->getSpecialFieldFuncName()).append("(CommMsgBody& data, const ").append(cls->getName()).append("* obj);\n"));
			}
			else
			{
				buf = bcls->fillSelFields(fld, fld->getFieldType(), mainObj);
				if(buf.size()) gSelFields.push_back(move(buf));
				//decls.insert(fld->getSelConvertFuncName());
				if(fld->getSelConvertFuncName().size())
					decls.insert(string("bool ").append(fld->getSelConvertFuncName()).append("(const char * txt, PString &res, const void* obj, const char* objname, const char* fieldname);\n"));
			}
		}
		if (fld->getProperty() & CG_CLASS_UPDATE_ON)
		{
			buf = bcls->fillSecUpdateAdd(fld, mainObj);
			if(buf.size()) gSecUpdateAdd.push_back(move(buf));
			buf = bcls->fillUpdate(fld, mainObj);
			if(buf.size()) gUpdate.push_back(move(buf));
			buf = bcls->fillAutoUpdate(fld, mainObj);
			if (buf.size()) gAutoUpdate.push_back(move(buf));
		}
	}
}

//vector<string> fillSpecFuncCompose(const CGfield* fld, const CGfieldtype* fieldType, CGmain* mainObj) override
//{
//	if(fld->getSpecialFieldFuncName().length() && (!fld->isArray()))
//	{
//		vector<string> res;
//		res.push_back(string("{\n"));
//		res.push_back(string("\t").append(fld->getSpecialFieldFuncName()).append("(data, this);\n"));
//		res.push_back(string("}\n"));
//		return res;
//	}
//	else
//	{
//		bool vecSmartCorr;
//		return fillCompose(fld, fieldType, fld->getName(), "default", mainObj, false, vecSmartCorr);
//	}
//}


inline void CGGcppmain::clear(void)
{
	gDefaultConctructorHeader.clear();
	gDefaultConctructorBody.clear();
	gDestructor.clear();
	gCopyConctructorHeader.clear();
	gCopyConctructorBody.clear();
	gMoveConctructorHeader.clear();
	gMoveConctructorBody.clear();
	gAssignment.clear();
	gMoveAssignment.clear();
	gEqual.clear();
	gSetters.clear();
	gMoveSetters.clear();
	gGetters.clear();
	gParse.clear();
	gCompose.clear();
	gPublisher.clear();
	gSubscrSetters.clear();
	gSubscrUpdateAll.clear();
	gSpecials.clear();
	gSelFields.clear();
	gSelConvertCompose.clear();
	gUpdate.clear();
	gSecUpdateAdd.clear();
	gAutoUpdate.clear();
}

inline void CGGcppmain::writeDefConstructor(const CGstruct* cls, CGfile& cfile)
{
	string output;
	output.append("// Constructor\n");
	makeTabMargin(output, 1);
	output.append(cls->getName());

	bool first = true;
	if (cls->getProperty() & CG_CLASS_INTERNAL_ON)
	{
		first = false;
		output.append("(CommServerPublisher& publisher_, CommServerPublisher::UpdateMsg& update_)\n");
		makeTabMargin(output, 1);
		output.append(":\tpublisher(publisher_)\n");
		makeTabMargin(output, 1);
		output.append(",\tupdate(update_)\n");
	}
	else
	{
		output.append("(void)\n");
	}

	for(auto& elem : gDefaultConctructorHeader)
	{
		makeTabMargin(output, 1);
		if(first)
		{
			first = false;
			output.append(":\t");
		}
		else
		{
			output.append(",\t");
		}
		output.append(elem);
	}

	if(gDefaultConctructorBody.size())
	{
		makeTabMargin(output, 1);
		output.append("{\n");
		for(auto& elem : gDefaultConctructorBody)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 2);
				output.append(elem1);
			}
		}
		makeTabMargin(output, 1);
		output.append("}\n\n");
	}
	else
	{
		makeTabMargin(output, 1);
		output.append("{}\n\n");
	}
	cfile.write(output);
}

inline void CGGcppmain::writeDestructor(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Destructor\n");
	makeTabMargin(output, 1);
	if (cls->getProperty() & CG_CLASS_INH_VIRT)
	{
		output.append("virtual ");

	}
	output.append("~").append(cls->getName()).append("(void)\n");
	if(gDestructor.size())
	{
		makeTabMargin(output, 1);
		output.append("{\n");
		for(auto& elem : gDestructor)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 2);
				output.append(elem1);
			}
		}
		makeTabMargin(output, 1);
		output.append("}\n\n");
	}
	else
	{
		makeTabMargin(output, 1);
		output.append("{}\n\n");
	}
	cfile.write(output);
}

inline void CGGcppmain::writeCopyConstructor(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Copy constructor\n");
	makeTabMargin(output, 1);
	output.append(cls->getName()).append("(const ").append(cls->getName()).append("& other)\n");

	bool first = true;
	if (cls->getProperty() & CG_CLASS_INH_BASE)
	{
		makeTabMargin(output, 1);
		first = false;
		output.append(":\t");
		output.append(cls->getBaseClass()).append("(other)\n");
	}
	else if (cls->getProperty() & CG_CLASS_TEMP_ON)
	{
		makeTabMargin(output, 1);
		first = false;
		output.append(":\t");
		output.append("T(other)\n");
	}
	if (cls->getProperty() & CG_CLASS_INTERNAL_ON)
	{
		makeTabMargin(output, 1);
		if(first)
		{
			first = false;
			output.append(":\t");
		}
		else
		{
			output.append(",\t");
		}
		output.append("publisher(other.publisher)\n");
		makeTabMargin(output, 1);
		output.append(",\tupdate(other.update)\n");
	}
	for(auto& elem : gCopyConctructorHeader)
	{
		makeTabMargin(output, 1);
		if(first)
		{
			first = false;
			output.append(":\t");
		}
		else
		{
			output.append(",\t");
		}
		output.append(elem);
	}

	if(gCopyConctructorBody.size())
	{
		makeTabMargin(output, 1);
		output.append("{\n");
		for(auto& elem : gCopyConctructorBody)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 2);
				output.append(elem1);
			}
		}
		makeTabMargin(output, 1);
		output.append("}\n\n");
	}
	else
	{
		makeTabMargin(output, 1);
		output.append("{}\n\n");
	}
	cfile.write(output, true);
}

inline void CGGcppmain::writeMovConstructor(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Move constructor\n");
	makeTabMargin(output, 1);
	output.append(cls->getName()).append("(").append(cls->getName()).append("&& other)\n");

	bool first = true;
	if (cls->getProperty() & CG_CLASS_INH_BASE)
	{
		makeTabMargin(output, 1);
		first = false;
		output.append(":\t");
		output.append(cls->getBaseClass()).append("(move(other))\n");
	}
	else if (cls->getProperty() & CG_CLASS_TEMP_ON)
	{
		makeTabMargin(output, 1);
		first = false;
		output.append(":\t");
		output.append("T(move(other))\n");
	}
	if (cls->getProperty() & CG_CLASS_INTERNAL_ON)
	{
		makeTabMargin(output, 1);
		if(first)
		{
			first = false;
			output.append(":\t");
		}
		else
		{
			output.append(",\t");
		}
		output.append("publisher(other.publisher)\n");
		makeTabMargin(output, 1);
		output.append(",\tupdate(other.update)\n");
	}
	for(auto& elem : gMoveConctructorHeader)
	{
		makeTabMargin(output, 1);
		if(first)
		{
			first = false;
			output.append(":\t");
		}
		else
		{
			output.append(",\t");
		}
		output.append(elem);
	}

	if(gMoveConctructorBody.size())
	{
		makeTabMargin(output, 1);
		output.append("{\n");
		for(auto& elem : gMoveConctructorBody)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 2);
				output.append(elem1);
			}
		}
		makeTabMargin(output, 1);
		output.append("}\n\n");
	}
	else
	{
		makeTabMargin(output, 1);
		output.append("{}\n\n");
	}
	cfile.write(output);
}

inline void CGGcppmain::writeAssignment(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Assignment operator\n");
	makeTabMargin(output, 1);
	output.append(cls->getName()).append("& operator = (const ").append(cls->getName()).append("& other)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	if (cls->getProperty() & CG_CLASS_INH_BASE)
	{
		makeTabMargin(output, 2);
		output.append(cls->getBaseClass()).append("::operator = (other);\n");
	}
	else if (cls->getProperty() & CG_CLASS_TEMP_ON)
	{
		makeTabMargin(output, 2);
		output.append("T::operator = (other);\n");
	}
	for(auto& elem : gAssignment)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 2);
			output.append(elem1);
		}
	}
	makeTabMargin(output, 2);
	output.append("return *this;\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output);
}

inline void CGGcppmain::writeMovAssignment(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Move assignment operator\n");
	makeTabMargin(output, 1);
	output.append(cls->getName()).append("& operator = (").append(cls->getName()).append("&& other)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	if (cls->getProperty() & CG_CLASS_INH_BASE)
	{
		makeTabMargin(output, 2);
		output.append(cls->getBaseClass()).append("::operator = (move(other));\n");
	}
	else if (cls->getProperty() & CG_CLASS_TEMP_ON)
	{
		makeTabMargin(output, 2);
		output.append("T::operator = (move(other));\n");
	}
	for(auto& elem : gMoveAssignment)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 2);
			output.append(elem1);
		}
	}
	makeTabMargin(output, 2);
	output.append("return *this;\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output);
}

inline void CGGcppmain::writeForbidden(const CGstruct* cls, CGfile& cfile)
{
	string output;

	if((cls->getProperty() & CG_CLASS_COPY_DENY) || (cls->getProperty() & CG_CLASS_PTRS_SMART_RESTR))
	{
		makeTabMargin(output, 1);
		output.append(cls->getName()).append("(const ").append(cls->getName()).append("& other);\n");
	}
	if((cls->getProperty() & CG_CLASS_ASSIGN_DENY) || (cls->getProperty() & CG_CLASS_PTRS_SMART_RESTR))
	{
		makeTabMargin(output, 1);
		output.append(cls->getName()).append("& operator = (").append(cls->getName()).append("& other);\n");
	}
	if(cls->getProperty() & CG_CLASS_MOVE_DENY)
	{
		makeTabMargin(output, 1);
		output.append(cls->getName()).append("(").append(cls->getName()).append("&& other);\n");
	}
	if(cls->getProperty() & CG_CLASS_MASSIGN_DENY)
	{
		makeTabMargin(output, 1);
		output.append(cls->getName()).append("& operator = (").append(cls->getName()).append("&& other);\n");
	}
	output.append("\n");
	cfile.write(output);
}

inline void CGGcppmain::writeCast(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Casting function\n");
	makeTabMargin(output, 1);
	output.append("const ").append(cls->getName()).append("& castBase(const ").append(cls->getBaseClass()).append("& other) const\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("try\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("return dynamic_cast<const ").append(cls->getName()).append("&>(other);\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("catch(...)\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("throw PError(\"").append(cls->getName()).append("::castBase: Bad cast exception\");\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output);
}

inline void CGGcppmain::writeCopy(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Copy function\n");
	makeTabMargin(output, 1);
	output.append("void copyFrom(const ");
	output.append(cls->getBaseClass());
	output.append("& other)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("*this = castBase(other);\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output);
}

inline void CGGcppmain::writeMove(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Move function\n");
	makeTabMargin(output, 1);
	output.append("void moveFrom(");
	output.append(cls->getBaseClass());
	output.append("& other)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("*this = move(castBase(other));\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output, true);
}

inline void CGGcppmain::writeCompare(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Comparator\n");
	makeTabMargin(output, 1);
	if (cls->getProperty() & CG_CLASS_INH_VIRT)
	{
		output.append("virtual ");
	}
	output.append("bool equals(const ");

	if (cls->getProperty() & CG_CLASS_INH_VIRT)
	{
		output.append(cls->getBaseClass()).append("& base) const\n");
	}
	else
	{
		output.append(cls->getName()).append("& other) const\n");
	}
	makeTabMargin(output, 1);
	output.append("{\n");
	if (cls->getProperty() & CG_CLASS_EQUAL_LOG)
	{
		makeTabMargin(output, 2);
		output.append("bool result = true;\n");
	}
	if (cls->getProperty() & CG_CLASS_EQUAL_BASE)
	{
		bool body = false;
		if (cls->getProperty() & CG_CLASS_INH_VIRT)		
		{
			makeTabMargin(output, 2);
			output.append("if(!").append(cls->getBaseClass()).append("::equals(base))");
			body = true;
		}
		else if (cls->getProperty() & CG_CLASS_INH_BASE)		
		{
			makeTabMargin(output, 2);
			output.append("if(!").append(cls->getBaseClass()).append("::equals(other))");
			body = true;
		}
		else if (cls->getProperty() & CG_CLASS_TEMP_ON)
		{
			makeTabMargin(output, 2);
			output.append("if(!T::equals(other))");
			body = true;
		}
		if(body)
		{
			if (cls->getProperty() & CG_CLASS_EQUAL_LOG)
			{
				output.append("\n");
				makeTabMargin(output, 2);
				output.append("{\n");
				makeTabMargin(output, 3);
				output.append("PLog(\"Base classes in class '").append(cls->getName()).append("' are NOT EQUAL\");\n");
				makeTabMargin(output, 3);
				output.append("result = false;\n");
				makeTabMargin(output, 2);
				output.append("}\n");
			}
			else
			{
				output.append(" return false;\n");
			}
		}
	}
	if (cls->getProperty() & CG_CLASS_INH_VIRT)		
	{
		makeTabMargin(output, 2);
		output.append("const ").append(cls->getName()).append("& other = castBase(base);\n");
	}
	for(auto& elem : gEqual)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 2);
			output.append(elem1);
		}
	}

	if (cls->getProperty() & CG_CLASS_EQUAL_LOG)
	{
		makeTabMargin(output, 2);
		output.append("return result;\n");
	}
	else
	{
		makeTabMargin(output, 2);
		output.append("return true;\n");
	}
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output);
}

inline void CGGcppmain::writeGetters(CGfile& cfile)
{
	string output;

	output.append("// Getters\n");
	for(auto& elem : gGetters)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	output.append("\n");
	cfile.write(output);
}

inline void CGGcppmain::writeSetters(CGfile& cfile)
{
	string output;

	output.append("// Setters\n");
	for(auto& elem : gSetters)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	output.append("\n");
	cfile.write(output);
}

inline void CGGcppmain::writeMovSetters(CGfile& cfile)
{
	string output;

	output.append("// Moving setters\n");
	for(auto& elem : gMoveSetters)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	output.append("\n");
	cfile.write(output);
}

inline void CGGcppmain::writeParseCompose(const CGstruct* cls, CGfile& cfile)
{
	string output;
	output.append("// Communication channels\n");
	{
		auto it1 = cls->getPcChannels().find("default");
		if(it1 != end(cls->getPcChannels()))
		{
			if(std::get<1>(it1->second))
			{
				makeTabMargin(output, 1);
				if (cls->getProperty() & CG_CLASS_INH_VIRT)
				{
					output.append("virtual ");
				}
				output.append("void parse").append("(CommMsgParser& parser)\n");
				makeTabMargin(output, 1);
				output.append("{\n");
				auto it2 = gParse.find("default");
				if(it2 != end(gParse))
				{
					for(auto& elem : it2->second)
					{
						if (cls->getProperty() & CG_CLASS_PARSECHK_ON)
						{
							makeTabMargin(output, 2);
							output.append("if(parser.parseEnded())\n");
							makeTabMargin(output, 2);
							output.append("{\n");
							makeTabMargin(output, 3);
							output.append("PLog(\"Parser channel '").append(it1->first).append("' is not finished by the end of the message before field '").append(elem.first).append("'\");\n");
							makeTabMargin(output, 3);
							output.append("return;\n");
							makeTabMargin(output, 2);
							output.append("}\n");
						}
						for(auto& elem1 : elem.second)
						{
							makeTabMargin(output, 2);
							output.append(elem1);
						}
					}
				}
				makeTabMargin(output, 1);
				output.append("}\n");
				cfile.write(output, true);
			}
			if(get<2>(it1->second))
			{
				makeTabMargin(output, 1);
				if (cls->getProperty() & CG_CLASS_INH_VIRT)
				{
					output.append("virtual ");
				}
				output.append("void compose").append("(CommMsgBody& data) const\n");
				makeTabMargin(output, 1);
				output.append("{\n");
				auto it2 = gCompose.find("default");
				if(it2 != end(gCompose))
				{
					for(auto& elem : it2->second)
					{
						for(auto& elem1 : elem.second)
						{
							makeTabMargin(output, 2);
							output.append(elem1);
						}
					}
				}
				makeTabMargin(output, 1);
				output.append("}\n");

				output.append("#ifdef CE_FRONTEND_SUPPORT\n");
				makeTabMargin(output, 1);
				output.append("void selConvertCompose").append("(CommMsgBody& data) const\n");
				makeTabMargin(output, 1);
				output.append("{\n");
				for(auto& elem : gSelConvertCompose)
				{
					for(auto& elem1 : elem.second)
					{
						makeTabMargin(output, 2);
						output.append(elem1);
					}
				}

				makeTabMargin(output, 1);
				output.append("}\n");
				output.append("#endif // CE_FRONTEND_SUPPORT\n");
				cfile.write(output, true);
			}
		}
	}
	for(auto& el : cls->getPcChannels())
	{
		if (el.first == "JAVA")
			continue;

		if(el.first != "default")
		{
			if(std::get<0>(el.second)) // smart
			{
				if(std::get<1>(el.second))
				{
					makeTabMargin(output, 1);
					if (cls->getProperty() & CG_CLASS_INH_VIRT)
					{
						output.append("virtual ");
					}
					output.append("void parseFrom").append(el.first).append("(CommMsgParser& parser, size_t _ce__ext_size)\n");
					makeTabMargin(output, 1);
					output.append("{\n");
					auto it = gParse.find(el.first);
					bool firstPassParse = true;
					if(it != end(gParse))
					{
						for(auto& elem : it->second)
						{
							if (firstPassParse)
							{
								makeTabMargin(output, 2);
								output.append("_ce__ext_size += parser._shift();\n");
								makeTabMargin(output, 2);
								output.append("while(!").append("parser").append(".parseEnded() && parser._shift() < _ce__ext_size)\n");
								makeTabMargin(output, 2);
								output.append("{\n");
								makeTabMargin(output, 3);
								output.append("const char * __ce__name;\n");
								makeTabMargin(output, 3);
								output.append("UINT32 __ce__size;\n");
								makeTabMargin(output, 3);
								output.append("parser").append(".parseString(__ce__name).parseUINT32(__ce__size);\n");
								makeTabMargin(output, 3);
								output.append("if (!strcmp(__ce__name,\"").append(elem.first).append("\"))\n");
								firstPassParse=false;
							}
							else
							{
								makeTabMargin(output, 3);
								output.append("else if (!strcmp(__ce__name,\"").append(elem.first).append("\"))\n");
							}
							makeTabMargin(output, 3);
							output.append("{\n");
							for(auto& elem1 : elem.second)
							{
								makeTabMargin(output, 4);
								output.append(elem1);
							}
							makeTabMargin(output, 3);
							output.append("}\n");
						}
					}
					if (!firstPassParse)
					{
						makeTabMargin(output,3);
						output.append("else\n");
						makeTabMargin(output, 3);
						output.append("{\n");
						makeTabMargin(output, 4);
						output.append("PTRACE4(\"").append(cls->getName()).append("::parseFrom").append(el.first).append(" - unknown name '%s', size %d\", __ce__name,__ce__size);\n");
						makeTabMargin(output, 4);
						output.append("parser").append(".skip(__ce__size);\n");
						makeTabMargin(output, 3);
						output.append("}\n");
						makeTabMargin(output, 2);
						output.append("}\n");
					}
					makeTabMargin(output, 1);
					output.append("}\n");
					cfile.write(output, true);
				}
				if(std::get<2>(el.second))
				{
					makeTabMargin(output, 1);
					if (cls->getProperty() & CG_CLASS_INH_VIRT)
					{
						output.append("virtual ");
					}
					output.append("void composeFor").append(el.first).append("(CommMsgBody& data) const\n");
					makeTabMargin(output, 1);
					output.append("{\n");
					auto it = gCompose.find(el.first);
					bool firstPassCompose = true;
					if(it != end(gCompose))
					{
						for(auto& elem : it->second)
						{
							if (firstPassCompose)
							{
								makeTabMargin(output, 2);
								output.append("size_t __ce__shift=0;\n");
								makeTabMargin(output, 2);
								output.append("size_t __ce__size=0;\n");
								firstPassCompose=false;
							}
							makeTabMargin(output, 2);
							output.append("data").append(".composeString(\"").append(elem.first).append("\").composeVectorSize(0,__ce__shift);\n");
							makeTabMargin(output, 2);
							output.append("__ce__size = data._size();\n");
							for(auto& elem1 : elem.second)
							{
								makeTabMargin(output, 2);
								output.append(elem1);
							}
							makeTabMargin(output, 2);
							output.append("data").append(".updateVectorSize( ").append("data").append("._size() - __ce__size, __ce__shift);\n");
						}
					}
					makeTabMargin(output, 1);
					output.append("}\n");
					cfile.write(output, true);
				}
			}
			else
			{
				if(std::get<1>(el.second))
				{
					makeTabMargin(output, 1);
					if (cls->getProperty() & CG_CLASS_INH_VIRT)
					{
						output.append("virtual ");
					}
					output.append("void parseFrom").append(el.first).append("(CommMsgParser& parser)\n");
					makeTabMargin(output, 1);
					output.append("{\n");
					auto it = gParse.find(el.first);
					if(it != end(gParse))
					{
						for(auto& elem : it->second)
						{
							if (cls->getProperty() & CG_CLASS_PARSECHK_ON)
							{
								makeTabMargin(output, 2);
								output.append("if(parser.parseEnded())\n");
								makeTabMargin(output, 2);
								output.append("{\n");
								makeTabMargin(output, 3);
								output.append("PLog(\"Parser channel '").append(el.first).append("' is not finished by the end of the message before field '").append(elem.first).append("'\");\n");
								makeTabMargin(output, 3);
								output.append("return;\n");
								makeTabMargin(output, 2);
								output.append("}\n");
							}
							for(auto& elem1 : elem.second)
							{
								makeTabMargin(output, 2);
								output.append(elem1);
							}
						}
					}
					makeTabMargin(output, 1);
					output.append("}\n");
					cfile.write(output, true);
				}
				if(std::get<2>(el.second))
				{
					makeTabMargin(output, 1);
					if (cls->getProperty() & CG_CLASS_INH_VIRT)
					{
						output.append("virtual ");
					}
					output.append("void composeFor").append(el.first).append("(CommMsgBody& data) const\n");
					makeTabMargin(output, 1);
					output.append("{\n");
					auto it = gCompose.find(el.first);
					if(it != end(gCompose))
					{
						for(auto& elem : it->second)
						{
							for(auto& elem1 : elem.second)
							{
								makeTabMargin(output, 2);
								output.append(elem1);
							}
						}
					}
					makeTabMargin(output, 1);
					output.append("}\n");
					cfile.write(output, true);
				}
			}
		}
	}
}

void CGGcppmain::writeField(const CGstruct* cls, const CGfield* fld, CGfile& cfile)
{
	string output;
	makeTabMargin(output, 1);

	if (fld->getFieldModificator().length())
	{
		output.append(fld->getFieldModificator()).append("\t");
	}
	output.append(fld->getFieldType()->getFullTypeString()).append("\t").append(fld->getName());
	if(fld->getFieldType()->getArraySize())
	{
		output.append("[").append(to_string(fld->getFieldType()->getArraySize())).append("]");
	}
	output.append("; ");

	if(fld->getTxt().length())
		output.append(fld->getTxt());
	else
		output.append("\n");

	cfile.write(output, true);
}

void CGGcppmain::writeSubscription(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Subscription support\n");

	makeTabMargin(output, 1);
	if (cls->getProperty() & CG_CLASS_INTERNAL_ON)
		output.append("CommServerPublisher::UpdateMsg&	update;\n");
	else
		output.append("CommServerPublisher::UpdateMsg	update;\n");
	makeTabMargin(output, 1);
	if (cls->getProperty() & CG_CLASS_INTERNAL_ON)
		output.append("CommServerPublisher&	publisher;\n");
	else
		output.append("CommServerPublisher	publisher;\n");
	makeTabMargin(output, 1);
	output.append("void startUpdateMsg()\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("if (published) publisher.startUpdateMsg(update);\n");
	makeTabMargin(output, 1);
	output.append("}\n");

	makeTabMargin(output, 1);
	output.append("void finishUpdateMsg()\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("if (published)\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("publisher.finishUpdateMsg(update);\n");
	makeTabMargin(output, 3);
	output.append("publisher.postUpdate(update);\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");

	cfile.write(output, true);


}

void CGGcppmain::writePublish(const CGstruct* cls, CGfile& cfile)
{
	string output;
	output.append("// Publisher\n");
	makeTabMargin(output, 1);
	output.append("void publish(CommServerPublisherPool & pool, const char* name)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("if(published) throw PError(\"").append(cls->getName()).append("::publish: Already published exception\");\n");

	makeTabMargin(output, 2);
	output.append("Path pp;\n");


	makeTabMargin(output, 2);
	output.append("publish(pp);\n");
	makeTabMargin(output, 2);
	output.append("pool.publish( name, publisher);\n");
	makeTabMargin(output, 1);
	output.append("}\n");
	makeTabMargin(output, 1);
	output.append("void publish(Path pp0)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("published=true;\n");
	for(auto& elem : gPublisher)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 2);
			output.append(elem1);
		}
	}
	makeTabMargin(output, 1);
	output.append("}\n");
	cfile.write(output, true);
}

void CGGcppmain::writePublishUpdate(const CGstruct* cls, CGfile& cfile)
{
	string output;
	makeTabMargin(output, 1);
	output.append("void publisherUpdate").append("(const ").append(cls->getName()).append("& other")
		.append(", Path pp)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("if(published)\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	for(auto& elem : gSubscrUpdateAll)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 3);
			output.append(elem1);
		}
	}
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n");

	makeTabMargin(output, 1);
	output.append("int publisherAdd").append("(const ").append(cls->getName()).append("& other, Path pp)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	//makeTabMargin(output, 2);
	//output.append("if(published)\n");
	//makeTabMargin(output, 2);
	//output.append("{\n");

	makeTabMargin(output, 2);
	output.append("int idxNode = publisher.appendNode(pp.path,pp.pathLen,0);\n");
	makeTabMargin(output, 2);
	output.append("pp.addValue(idxNode);\n");


	makeTabMargin(output, 2);
	output.append("publish(pp);\n");
	makeTabMargin(output, 2);
	output.append("return idxNode;\n");

	//makeTabMargin(output, 2);
	//output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n");




	for(auto& elem : gSubscrSetters)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 1);
			output.append(elem1);
		}
	}
	cfile.write(output, true);
}

void CGGcppmain::fillSpecial1(const CGstruct* cls, CGfile& cfile)
{
	string output;
	const CGfield* fld = nullptr;

	for(auto& elem : cls->getObjects())
	{
		if(elem->getObjType() == objTypes::gTypeField)
		{
			fld = dynamic_cast<const CGfield*>(elem.get());
			if(!fld)
			{
				string err;
				err.append("Wrong dynamic cast to class-'").append(cls->getName()).append("' object-'").append(elem->getName());
				throw exception(err.c_str());
			}
			if(!fld->isArray())
			{
				fld = nullptr;
			}
			else
				break;
		}
	}

	//const CGbaseobj* obj = cls->findObject(objTypes::gTypeField, cls->getName());
	if(!fld) throw exception(string("wrong Special1 attributes in class ").append(cls->getName()).c_str());
	//const CGfield* fld = dynamic_cast<const CGfield*>(obj);
	size_t z = fld->getFieldType()->getArraySize();
	uint32_t idx = fld->getLeafNodeIndex();

	makeTabMargin(output, 1);
	output.append("void publisher_special1(UINT32 newValue, Path pp)\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	//makeTabMargin(output, 2);
	//output.append("pp.addValue(").append(to_string(idx)).append(");\n");
	makeTabMargin(output, 2);
	output.append("pp.addValue(0);\n");
	makeTabMargin(output, 2);
	output.append("if(").append(fld->getName()).append("Pos == ").append(to_string(z-1)).append(")\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("if(").append(fld->getName()).append("Full == false)\n");
	makeTabMargin(output, 3);
	output.append("{\n");
	makeTabMargin(output, 4);
	output.append(fld->getName()).append("Full = true;\n");
	makeTabMargin(output, 4);
	output.append("CommMsgBody data;\n");
	makeTabMargin(output, 4);
	output.append("data.composeBOOL(true);\n");

	//makeTabMargin(output, 4);
	//output.append("int pathLen1 = pathLen+1;\n");
	//makeTabMargin(output, 4);
	//output.append("unique_ptr<int> u_path(new int[pathLen1]);\n");
	//makeTabMargin(output, 4);
	//output.append("int* path1 = u_path.get();\n");
	//makeTabMargin(output, 4);
	//output.append("if(pathLen) memcpy(path1").append(", path, pathLen*sizeof(int));\n");
	//makeTabMargin(output, 4);
	//output.append("path1[pathLen] = ").append(to_string(idx+2)).append(";\n");

	makeTabMargin(output, 4);
	output.append("pp.changeLastValue(").append(to_string(idx+2)).append(");\n");

	makeTabMargin(output, 4);
	output.append("publisher.updateLeaf(pp.path, pp.pathLen,data,&update);\n");
	makeTabMargin(output, 3);
	output.append("}\n");
	makeTabMargin(output, 3);
	output.append(fld->getName()).append("Pos = 0;\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("else\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append(fld->getName()).append("Pos++;\n");
	makeTabMargin(output, 2);
	output.append("}\n");

	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("CommMsgBody data;\n");
	makeTabMargin(output, 3);
	output.append("data.composeUINT32(").append(fld->getName()).append("Pos);\n");

	//makeTabMargin(output, 3);
	//output.append("int pathLen1 = pathLen+1;\n");
	//makeTabMargin(output, 3);
	//output.append("unique_ptr<int> u_path(new int[pathLen1]);\n");
	//makeTabMargin(output, 3);
	//output.append("int* path1 = u_path.get();\n");
	//makeTabMargin(output, 3);
	//output.append("if(pathLen) memcpy(path1").append(", path, pathLen*sizeof(int));\n");
	//makeTabMargin(output, 3);
	//output.append("path1[pathLen] = ").append(to_string(idx+1)).append(";\n");

	makeTabMargin(output, 3);
	output.append("pp.changeLastValue(").append(to_string(idx+1)).append(");\n");

	makeTabMargin(output, 3);
	output.append("publisher.updateLeaf(pp.path, pp.pathLen,data,&update);\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("bucket[bucketPos] = newValue;\n");

	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("CommMsgBody data;\n");
	makeTabMargin(output, 3);
	output.append("data.composeUINT32(newValue);\n");

	//makeTabMargin(output, 3);
	//output.append("int pathLen1 = pathLen+2;\n");
	//makeTabMargin(output, 3);
	//output.append("unique_ptr<int> u_path(new int[pathLen1]);\n");
	//makeTabMargin(output, 3);
	//output.append("int* path1 = u_path.get();\n");
	//makeTabMargin(output, 3);
	//output.append("if(pathLen) memcpy(path1").append(", path, pathLen*sizeof(int));\n");
	//makeTabMargin(output, 3);
	//output.append("path1[pathLen] = ").append(to_string(idx)).append(";\n");
	//makeTabMargin(output, 3);
	//output.append("path1[pathLen+1] = ").append(fld->getName()).append("Pos;\n");

	makeTabMargin(output, 3);
	output.append("pp.changeLastValue(").append(to_string(idx)).append(");\n");
	makeTabMargin(output, 3);
	output.append("pp.addValue(").append(fld->getName()).append("Pos);\n");

	makeTabMargin(output, 3);
	output.append("publisher.updateLeaf(pp.path, pp.pathLen,data,&update);\n");
	makeTabMargin(output, 2);
	output.append("}\n");

	makeTabMargin(output, 1);
	output.append("}\n\n");

	cfile.write(output, true);
}

void CGGcppmain::writeSelField1(const CGstruct* cls, CGfile& cfile)
{
	string output;
	if (mainObj->baseRefClass.size() == 0)
		throw exception(string("Empty base reference class").c_str());

	makeTabMargin(output, 1);
	output.append("void composeSelectiveFields(CommExtUINT32Vector& path, CommMsgBody& data, const ");
	output.append(mainObj->baseRefClass);
	output.append("* (*getRefObject)(const char* type, const char* strId, UINT32 intId, void* param1, void* param2), void* param1, void* param2) const\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("size_t path_pointer = 0;\n");
	//makeTabMargin(output, 2);
	//output.append("data.composeUINT32(1);\n");
	makeTabMargin(output, 2);
	output.append("if(path.size())\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("while(path_pointer < path.size())\n");
	makeTabMargin(output, 3);
	output.append("{\n");
	makeTabMargin(output, 4);
	output.append("composeSelectiveFieldsInt(path, data, path_pointer, getRefObject, param1, param2);\n");
	makeTabMargin(output, 3);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("else\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("selConvertCompose(data);\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");

	makeTabMargin(output, 1);
	output.append("void composeSelectiveFieldsCounted(CommExtUINT32Vector& path, CommMsgBody& data, size_t& path_pointer, size_t amount, const ");
	output.append(mainObj->baseRefClass);
	output.append("* (*getRefObject)(const char* type, const char* strId, UINT32 intId, void* param1, void* param2), void* param1, void* param2) const\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output, 2);
	output.append("size_t counter = path_pointer;\n");
	makeTabMargin(output, 2);
	output.append("if(amount)\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("while((path_pointer < path.size()) && ((path_pointer - counter) < amount))\n");
	makeTabMargin(output, 3);
	output.append("{\n");
	makeTabMargin(output, 4);
	output.append("composeSelectiveFieldsInt(path, data, path_pointer, getRefObject, param1, param2);\n");
	//makeTabMargin(output, 4);
	//output.append("counter++;\n");
	makeTabMargin(output, 3);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 2);
	output.append("else\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("selConvertCompose(data);\n");
	makeTabMargin(output, 2);
	output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");

	cfile.write(output, true);
}

void CGGcppmain::writeSelField2(const CGstruct* cls, CGfile& cfile)
{
	string output;
	if (mainObj->baseRefClass.size() == 0)
		throw exception(string("Empty base reference class").c_str());

	makeTabMargin(output, 1);
	output.append("void composeSelectiveFieldsInt(CommExtUINT32Vector& path, CommMsgBody& data, size_t& path_pointer, const ");
	output.append(mainObj->baseRefClass);
	output.append("* (*getRefObject)(const char* type, const char* strId, UINT32 intId, void* param1, void* param2), void* param1, void* param2) const\n");
	makeTabMargin(output, 1);
	output.append("{\n");
	makeTabMargin(output,2);
	output.append("UINT32 composeSelectiveFieldsInt_fieldnum = path[path_pointer] & 0xFF;\n");
	makeTabMargin(output, 2);
	output.append("UINT32 composeSelectiveFieldsInt_filtersize = (path[path_pointer] & 0xFF00) >> 8;\n");
	makeTabMargin(output, 2);
	output.append("UINT32 composeSelectiveFieldsInt_size = (path[path_pointer] & 0xFF0000) >> 16;\n");
	makeTabMargin(output,2);
	output.append("UINT32 composeSelectiveFieldsInt_code = (path[path_pointer++] & 0xFF000000) >> 24;\n");


	//makeTabMargin(output, 2);
	//output.append("if (strcmp(name,\"").append(cls->getModelType()).append("\"))\n");
	//makeTabMargin(output,2);
	//output.append("{\n");
	//makeTabMargin(output,3);
	//output.append("throw PError(\"Format request supported for ").append(cls->getName()).append(" object only\");\n");
	//makeTabMargin(output, 2);
	//output.append("}\n");

	//makeTabMargin(output, 2);
	//output.append("for(size_t i = 0; i < size; ++i)\n");
	//makeTabMargin(output,2);
	//output.append("{\n");
	makeTabMargin(output,2);
	output.append("switch(composeSelectiveFieldsInt_fieldnum)\n");
	makeTabMargin(output,2);
	output.append("{\n");

	for(auto& elem : gSelFields)
	{
		for(auto& elem1 : elem)
		{
			makeTabMargin(output, 3);
			output.append(elem1);
		}
	}
	makeTabMargin(output, 3);
	output.append("default:\n");
	makeTabMargin(output,3);
	output.append("{\n");
	makeTabMargin(output,4);
	output.append("PASSERT3(0);\n");
	makeTabMargin(output,3);
	output.append("}\n");
	makeTabMargin(output,2);
	output.append("}\n");
	//makeTabMargin(output,2);
	//output.append("}\n");
	makeTabMargin(output, 1);
	output.append("}\n\n");
	cfile.write(output, true);
}
//output.append("#ifdef CE_FRONTEND_SUPPORT\n");
//output.append("#endif // CE_FRONTEND_SUPPORT\n");

inline void CGGcppmain::writeUpdate(const CGstruct* cls, CGfile& cfile)
{
	string output;

	output.append("// Update\n");
	output.append("#ifdef CE_FIELDS_UPDATE\n");

	makeTabMargin(output, 1);
	if (cls->getProperty() & CG_CLASS_INH_VIRT)
	{
		output.append("virtual ");
	}
	output.append("bool fieldsUpdate(CommMsgParser & parser, const void* secure_param, void* param1, void* param2, PString& errMsg, PStringSet& fieldsUpdate_fields, const char* updater_string) override\n");
	makeTabMargin(output, 1);
	output.append("{\n");

	makeTabMargin(output, 2);
	output.append("UINT32 counter;\n");
	makeTabMargin(output, 2);
	output.append("const char * fieldsUpdateType;\n");
	makeTabMargin(output, 2);
	output.append("fieldsUpdate_fields.clear();\n");
	makeTabMargin(output, 2);
	output.append("parser.parseUINT32(counter);\n");
	makeTabMargin(output, 2);
	output.append("if(!counter)\n");
	makeTabMargin(output, 2);
	output.append("{\n");
	makeTabMargin(output, 3);
	output.append("errMsg.assign(\"Class '").append(cls->getName()).append("' nothing to update\");\n");
	makeTabMargin(output, 3);
	output.append("return false;\n");
	makeTabMargin(output, 2);
	output.append("}\n");


	auto secCodes = cls->getUpdateSecCodes();
	if(secCodes.size())
	{
		for(auto &i: secCodes)
		{
			makeTabMargin(output, 2);
			output.append("if(").append(cls->getUpdateSecurityFunc()).append("(").append(to_string(i)).append(", this, param1, secure_param)); else\n");
		}

		makeTabMargin(output, 2);
		output.append("{\n");

		if(gSecUpdateAdd.size())
		{

			makeTabMargin(output, 3);
			output.append("while(counter--)\n");
			makeTabMargin(output, 3);
			output.append("{\n");
			makeTabMargin(output, 4);
			output.append("parser.parseString(fieldsUpdateType);\n");

			for(auto& elem : gSecUpdateAdd)
			{
				for(auto& elem1 : elem)
				{
					makeTabMargin(output, 4);
					output.append(elem1);
				}
			}

			makeTabMargin(output, 4);
			output.append("{\n");
			makeTabMargin(output, 5);
			output.append("errMsg.assign(\"Class '").append(cls->getName()).append("' update rights false\");\n");
			makeTabMargin(output, 5);
			output.append("return false;\n");
			makeTabMargin(output, 4);
			output.append("}\n");

			makeTabMargin(output, 3);
			output.append("}\n");
			makeTabMargin(output, 3);
			output.append("return true;\n");
		}
		if(!gSecUpdateAdd.size())
		{
			makeTabMargin(output, 3);
			output.append("errMsg.assign(\"Class '").append(cls->getName()).append("' update rights false\");\n");
			makeTabMargin(output, 3);
			output.append("return false;\n");
		}
		
		makeTabMargin(output, 2);
		output.append("}\n");
	}

	//if(gUpdate.size())
	{
		makeTabMargin(output, 2);
		output.append("while(counter--)\n");
		makeTabMargin(output, 2);
		output.append("{\n");
		makeTabMargin(output, 3);
		output.append("parser.parseString(fieldsUpdateType);\n");

		for(auto& elem : gUpdate)
		{
			for(auto& elem1 : elem)
			{
				makeTabMargin(output, 3);
				output.append(elem1);
			}
		}

		makeTabMargin(output, 3);
		output.append("{\n");
		makeTabMargin(output, 4);
		output.append("errMsg.assign(\"Error parsing object class '").append(cls->getName()).append("' : field name '\").append(fieldsUpdateType).append(\"' not found\");\n");
		makeTabMargin(output, 4);
		output.append("return false;\n");
		makeTabMargin(output, 3);
		output.append("}\n");

		makeTabMargin(output, 2);
		output.append("}\n");
	}

	auto secCodesAfter = cls->getUpdateSecCodesAfter();
	if(secCodesAfter.size())
	{
		for(auto &i: secCodesAfter)
		{
			makeTabMargin(output, 2);
			output.append("if(").append(cls->getUpdateSecurityFuncAfter()).append("(").append(to_string(i)).append(", this, param1, secure_param)); else\n");
		}
		makeTabMargin(output, 2);
		output.append("{\n");
		makeTabMargin(output, 3);
		output.append("errMsg.assign(\"Class '").append(cls->getName()).append("' update rights false\");\n");
		makeTabMargin(output, 3);
		output.append("return false;\n");
		makeTabMargin(output, 2);
		output.append("}\n");
	}

	if (cls->getProperty() & CG_AUTO_UPDATE_MASK)
	{
		makeTabMargin(output, 2);
		output.append("if(fieldsUpdate_fields.size() > 0)\n");
		makeTabMargin(output, 2);
		output.append("{\n");
		for (auto& elem : gAutoUpdate)
		{
			for (auto& elem1 : elem)
			{
				makeTabMargin(output, 3);
				output.append(elem1);
			}
		}

		makeTabMargin(output, 2);
		output.append("}\n");
	}

	makeTabMargin(output, 2);
	output.append("return true;\n");

	makeTabMargin(output, 1);
	output.append("}\n\n");
	output.append("#endif // CE_FIELDS_UPDATE\n");
	cfile.write(output);
}

