// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "genRule.h"

void CGRrulemain::generateRule(void)
{
	generateDsl();
	auto it = mainObj->iniInfo.find("GENRULE");
	if(it != end(mainObj->iniInfo))
	{
		for(auto& elem : it->second)
		{
			CLPGByteCodeGen codeGen(mainObj);
			codeGen.generate(elem.first.c_str());
		}
	}
	else
	{
		WriteLog("No input files defined.");
	}
}

inline void CGRrulemain::generateDsl(void)
{
	string output;
	CGfile cfile;
	set<string> usedIndexedTypes;

	// H file
	if(mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	output.append("DSLClassInventory.h");
	cfile.open(output);
	output.clear();

	output.append(COPYRIGHT"#ifndef DSLClassInventory_h_included\n#define DSLClassInventory_h_included\n\n");
	cfile.write(output, true);

	// parse ini file - DSL_SUPPORT
	auto it = mainObj->iniInfo.find("DSL_SUPPORT_H_INCLUDE");
	if(it != end(mainObj->iniInfo))
	{
		for(auto& elem : it->second)
		{
			output.append("#include \"").append(elem.second.c_str()).append("\"\n");
		}
	}
	output.append("\n");

	output.append("//***********************************************************************\n");
	output.append("// This file contains declaration of functions for an interpreter support\n");
	output.append("//***********************************************************************\n");
	output.append("// WARNING!!! All class field operations are valid ONLY for PUBLIC fields.\n");
	output.append("//***********************************************************************\n\n");

	output.append("// Initialization. Must be called ones at start.\n");
	output.append("void initClassInventory(void);\n");
	output.append("// Get class field info. Returns true if the field was found and the result is valid.\n");
	output.append("// 'UINT32& offset' is this field offset, 'PString& fieldtype' is this field type\n");
	output.append("// 'PString& fieldsubtype' is this field subtype - container template type, 'PString& fullfieldtype' is this field type as it's printed in output file\n");
	output.append("bool getFieldInfo(UINT32& offset, PString& fieldtype, PString& fieldsubtype, PString& fullfieldtype, const char* classname, const char* fieldname);\n");
	output.append("// Get class field offset. Returns true if the field was found and the result is valid.\n");
	output.append("bool getFieldOffset(UINT32& offset, const char* classname, const char* fieldname);\n");
	output.append("// Returns class field direct pointer. NULL - if not found\n");
	output.append("// 'void* obj' is an input pointer for the object which field is used.\n");
	output.append("void* getFieldAddress(void* obj, const char* classname, const char* fieldname);\n");
	output.append("// Returns pointer to container element by index. NULL - if the field was not found, the index is out of range or this field is not container. \n");
	output.append("// 'void* obj' is an input pointer for the object which field is used.\n");
	output.append("void* getFieldContainerElement(void* obj, const char* classname, const char* fieldname, UINT32 index);\n");
	output.append("// Get container size. Returns true if the field was found as container and the result is valid.\n");
	output.append("// 'void* obj' is an input pointer for the object which field is used.\n");
	output.append("bool getFieldContainerSize(UINT32& size, void* obj, const char* classname, const char* fieldname);\n");

	output.append("\n#endif // DSLClassInventory_h_included\n");

	cfile.write(output, true);
	cfile.close();

	// CPP file
	if(mainObj->outdir.length())
	{
		output.assign(mainObj->outdir).append("\\");
	}
	output.append("DSLClassInventory.cpp");
	cfile.open(output);
	output.clear();

	output.assign(COPYRIGHT);
	output.append("#define CE_FIELDS_UPDATE\n");
	// parse ini file - DSL_SUPPORT
	it = mainObj->iniInfo.find("DSL_SUPPORT_CPP_INCLUDE");
	if(it != end(mainObj->iniInfo))
	{
		for(auto& elem : it->second)
		{
			output.append("#include \"").append(elem.second.c_str()).append("\"\n");
		}
	}
	output.append("\n");

	output.append("//***********************************************************************\n");
	output.append("// This file contains definition of functions for an interpreter support\n");
	output.append("//***********************************************************************\n");
	output.append("// WARNING!!! All class field operations are valid ONLY for PUBLIC fields.\n");
	output.append("//***********************************************************************\n\n");

	output.append("// Function typedefs\n");
	output.append("typedef void* (*getIndexValueType)(void*, UINT32);\n");
	output.append("typedef UINT32 (*getSizeType)(const void*);\n\n");

	output.append("// Vector functions\n");
	output.append("template<class T>\n");
	output.append("inline static void* getIndexVectorTempl(void* obj, UINT32 idx)\n");
	output.append("{\n");
	output.append("\tvector<T>* p = reinterpret_cast<vector<T>*>(obj);\n");
	output.append("\tif((NULL != p) && (idx < p->size()))\n");
	output.append("\t{\n");
	output.append("\t\treturn &(p->at(idx));\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn NULL;\n");
	output.append("}\n\n");

	output.append("template<class T>\n");
	output.append("inline static UINT32 getSizeVectorTempl(const void* obj)\n");
	output.append("{\n");
	output.append("\tif(NULL != obj)\n");
	output.append("\t\treturn static_cast<UINT32>(reinterpret_cast<const vector<T>*>(obj)->size());\n");
	output.append("\telse\n");
	output.append("\t\treturn 0;\n");
	output.append("}\n\n");

	output.append("// Data structure for a class field\n");
	output.append("struct ClassesOffsetDataType\n");
	output.append("{\n");
	output.append("\tClassesOffsetDataType(const char* _fullfieldtype, const char* _fieldtype, const char* _fieldsubtype, UINT32 _offset, getIndexValueType _getIndexValue, getSizeType _getSize)\n");
	output.append("\t:\tfullfieldtype(_fullfieldtype)\n");
	output.append("\t,\tfieldtype(_fieldtype)\n");
	output.append("\t,\tfieldsubtype(_fieldsubtype)\n");
	output.append("\t,\toffset(_offset)\n");
	output.append("\t,\tgetIndexValue(_getIndexValue)\n");
	output.append("\t,\tgetSize(_getSize)\n");
	output.append("\t{}\n");
	output.append("\tconst char*			fullfieldtype;\n");
	output.append("\tconst char*			fieldtype;\n");
	output.append("\tconst char*			fieldsubtype;\n");
	output.append("\tUINT32				offset;\n");
	output.append("\tgetIndexValueType	getIndexValue;\n");
	output.append("\tgetSizeType			getSize;\n");
	output.append("};\n\n");

	output.append("// Main container with information\n");
	output.append("static PStringMap<PStringMap<ClassesOffsetDataType>*> classesOffsets;\n\n");

	output.append("// Find field - helper function, DON'T CALL directly\n");
	output.append("inline ClassesOffsetDataType* __CLPG_getField(const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tClassesOffsetDataType* codt = NULL;\n");
	output.append("\tPStringMap<ClassesOffsetDataType>::iterator iter;\n");
	output.append("\tPStringMap<PStringMap<ClassesOffsetDataType>*>::iterator iterMap;\n");
	output.append("\titerMap = classesOffsets.find(classname);\n");
	output.append("\tif(iterMap != classesOffsets.end())\n");
	output.append("\t{\n");
	output.append("\t\titer = iterMap->second->find(fieldname);\n");
	output.append("\t\tif(iter != iterMap->second->end())\n");
	output.append("\t\t{\n");
	output.append("\t\t\tcodt = &(iter->second);\n");
	output.append("\t\t}\n");
	output.append("\t}\n");
	output.append("\treturn codt;\n");
	output.append("}\n\n");

	output.append("// Get class field info. Returns true if the field was found and the result is valid.\n");
	output.append("// 'UINT32& offset' is this field offset, 'PString& fieldtype' is this field type\n");
	output.append("// 'PString& fieldsubtype' is this field subtype - container template type, 'PString& fullfieldtype' is this field type as it's printed in output file\n");
	output.append("bool getFieldInfo(UINT32& offset, PString& fieldtype, PString& fieldsubtype, PString& fullfieldtype, const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);\n");
	output.append("\tif(NULL != codt)\n");
	output.append("\t{\n");
	output.append("\t\toffset = codt->offset;\n");
	output.append("\t\tfieldtype.assign(codt->fieldtype);\n");
	output.append("\t\tfieldsubtype.assign(codt->fieldsubtype);\n");
	output.append("\t\tfullfieldtype.assign(codt->fullfieldtype);\n");
	output.append("\t\treturn true;\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn false;\n");
	output.append("}\n\n");

	output.append("// Get class field offset. Returns true if the field was found and the result is valid.\n");
	output.append("bool getFieldOffset(UINT32& offset, const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);\n");
	output.append("\tif(NULL != codt)\n");
	output.append("\t{\n");
	output.append("\t\toffset = codt->offset;\n");
	output.append("\t\treturn true;\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn false;\n");
	output.append("}\n\n");

	output.append("// Returns class field direct pointer. NULL - if not found\n");
	output.append("void* getFieldAddress(void* obj, const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tUINT32 offset;\n");
	output.append("\tif((NULL != obj) && (getFieldOffset(offset, classname, fieldname)))\n");
	output.append("\t\treturn reinterpret_cast<BYTE*>(obj) + offset;\n");
	output.append("\telse\n");
	output.append("\t\treturn NULL;\n");
	output.append("}\n\n");

	output.append("// Returns pointer to container element by index. NULL - if the index is out of range or this field is not container. \n");
	output.append("// 'void* obj' is an input pointer for the object which field is used.\n");
	output.append("void*  getFieldContainerElement(void* obj, const char* classname, const char* fieldname, UINT32 index)\n");
	output.append("{\n");
	output.append("\tClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);\n");
	output.append("\tif((NULL != codt) && (NULL != codt->getIndexValue))\n");
	output.append("\t{\n");
	output.append("\t\treturn codt->getIndexValue(obj, index);\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn NULL;\n");
	output.append("}\n\n");
	
	output.append("// Get container size. Returns true if the field was found as container and the result is valid.\n");
	output.append("// 'void* obj' is an input pointer for the object which field is used.\n");
	output.append("bool getFieldContainerSize(UINT32& size, void* obj, const char* classname, const char* fieldname)\n");
	output.append("{\n");
	output.append("\tClassesOffsetDataType* codt = __CLPG_getField(classname, fieldname);\n");
	output.append("\tif((NULL != codt) && (NULL != codt->getSize))\n");
	output.append("\t{\n");
	output.append("\t\tsize = codt->getSize(obj);\n");
	output.append("\t\treturn true;\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn false;\n");
	output.append("}\n\n");

	output.append("// Vector functions for pointers, DON'T CALL directly\n");
	output.append("static void* getIndex_PTR(void* obj, unsigned __int32 idx)\n");
	output.append("{\n");
	output.append("\tvector<void*>* p = reinterpret_cast<vector<void*>*>(obj);\n");
	output.append("\tif((NULL != p) && (idx < p->size()))\n");
	output.append("\t{\n");
	output.append("\t\treturn &(p->at(idx));\n");
	output.append("\t}\n");
	output.append("\telse\n");
	output.append("\t\treturn NULL;\n");
	output.append("}\n");

	output.append("static UINT32 getSize_PTR(const void* obj)\n");
	output.append("{\n");
	output.append("\tif(NULL != obj)\n");
	output.append("\t\treturn static_cast<UINT32>(reinterpret_cast<const vector<void*>*>(obj)->size());\n");
	output.append("\telse\n");
	output.append("\t\treturn 0;\n");
	output.append("}\n\n");

	output.append("// Vector helper functions, DON'T CALL directly\n");
	//output.append("static void* getIndexVectorINT8(void* obj, UINT32 idx) {return getIndexVectorTempl<INT8>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorINT8(const void* obj) {return getSizeVectorTempl<INT8>(obj);}\n");
	//output.append("static void* getIndexVectorBYTE(void* obj, UINT32 idx) {return getIndexVectorTempl<BYTE>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorBYTE(const void* obj) {return getSizeVectorTempl<BYTE>(obj);}\n");
	//output.append("static void* getIndexVectorbool(void* obj, UINT32 idx) {return getIndexVectorTempl<bool>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorbool(const void* obj) {return  getSizeVectorTempl<bool>(obj);}\n");
	//output.append("static void* getIndexVectorINT16(void* obj, UINT32 idx) {return getIndexVectorTempl<INT16>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorINT16(const void* obj) {return getSizeVectorTempl<INT16>(obj);}\n");
	//output.append("static void* getIndexVectorUINT16(void* obj, UINT32 idx) {return getIndexVectorTempl<UINT16>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorUINT16(const void* obj) {return getSizeVectorTempl<UINT16>(obj);}\n");
	//output.append("static void* getIndexVectorINT32(void* obj, UINT32 idx) {return getIndexVectorTempl<INT32>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorINT32(const void* obj) {return getSizeVectorTempl<INT32>(obj);}\n");
	//output.append("static void* getIndexVectorUINT32(void* obj, UINT32 idx) {return getIndexVectorTempl<UINT32>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorUINT32(const void* obj) {return getSizeVectorTempl<UINT32>(obj);}\n");
	//output.append("static void* getIndexVectorINT64(void* obj, UINT32 idx) {return getIndexVectorTempl<INT64>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorINT64(const void* obj) {return getSizeVectorTempl<INT64>(obj);}\n");
	//output.append("static void* getIndexVectorUINT64(void* obj, UINT32 idx) {return getIndexVectorTempl<UINT64>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorUINT64(const void* obj) {return getSizeVectorTempl<UINT64>(obj);}\n");
	//output.append("static void* getIndexVectorPString(void* obj, UINT32 idx) {return getIndexVectorTempl<PString>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorPString(const void* obj) {return getSizeVectorTempl<PString>(obj);}\n");
	//output.append("static void* getIndexVectorCommMsgBody(void* obj, UINT32 idx) {return getIndexVectorTempl<CommMsgBody>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorCommMsgBody(const void* obj) {return getSizeVectorTempl<CommMsgBody>(obj);}\n");
	//output.append("static void* getIndexVectorPBlock(void* obj, UINT32 idx) {return getIndexVectorTempl<PBlock>(obj, idx);}\n");
	//output.append("static UINT32 getSizeVectorPBlock(const void* obj) {return getSizeVectorTempl<PBlock>(obj);}\n");
	cfile.write(output, true);

	
	for(auto& doc : mainObj->documents)
	{
		for(auto& el : doc.second.getObjects())
		{
			if((el->getObjType() == objTypes::gTypeStruct) && (!((const CGstruct*)(el.get()))->isDSLMasked()))
			{
				for(auto& elem : el->getObjects())
				{
					if(elem->getObjType() == objTypes::gTypeField)
					{
						CGfield* fld = dynamic_cast<CGfield*>(elem.get());
						if(!fld)
						{
							string err;
							err.append("Wrong dynamic cast to field, file-'").append(doc.second.getName()).append("' class-'").append(el->getName()).append("' object-'").append(elem->getName());
							throw exception(err.c_str());
						}
						if(fld->getFieldType()->isDirectIndex())
						{
							string s = fld->getFieldType()->getIndexedType();
							usedIndexedTypes.insert(s);
						}
					}
				}
			}
		}
	}
	for(const string& s : usedIndexedTypes)
	{
		output.append("static void* getIndexVector").append(s).append("(void* obj, UINT32 idx) {return getIndexVectorTempl<").append(s).append(">(obj, idx);}\n");
		output.append("static UINT32 getSizeVector").append(s).append("(const void* obj) {return getSizeVectorTempl<").append(s).append(">(obj);}\n");
	}

	cfile.write(output, true);

	output.append("\n");
	output.append("// Initialization. Must be called ones at start\n");
	output.append("void initClassInventory(void)\n");
	output.append("{\n");

	output.append("\tPStringMap<PStringMap<ClassesOffsetDataType>*>::iterator iterMap;\n\n");

	for(auto& doc : mainObj->documents)
	{
		for(auto& el : doc.second.getObjects())
		{
			if ((el->getObjType() == objTypes::gTypeStruct) && (!((const CGstruct*)(el.get()))->isDSLMasked()))
			{
				CGstruct* cls = dynamic_cast<CGstruct*>(el.get());
				if(!cls)
				{
					string err;
					err.append("Wrong dynamic cast to class, file-'").append(doc.second.getName()).append("' class-'").append(el->getName()).append("'");
					throw exception(err.c_str());
				}
				output.append("\t// Class ").append(cls->getName()).append("\n");
				output.append("\tstatic PStringMap<ClassesOffsetDataType> ").append(cls->getName()).append("_map;\n");
				output.append("\titerMap = classesOffsets.insert(PStringMap<PStringMap<ClassesOffsetDataType>*>::value_type(\"").append(cls->getName()).append("\", &").append(cls->getName()).append("_map)).first;\n\n");

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
						output.append("\titerMap->second->insert(PStringMap<ClassesOffsetDataType>::value_type(\"").append(fld->getName())
							.append("\", ClassesOffsetDataType(\"").append(fld->getFieldType()->getFullTypeString()).append("\", \"").append(fld->getFieldType()->getFieldTypeStr()).append("\", \"");
						output.append(fld->getFieldType()->getIndexedType());
						output.append("\", offsetof(").append(cls->getName()).append(", ").append(fld->getName()).append("), ");
						if(fld->getFieldType()->isDirectIndex())
							output.append("getIndexVector").append(fld->getFieldType()->getIndexedType()).append(", ").append("getSizeVector").append(fld->getFieldType()->getIndexedType()).append(")));\n");
						else
							output.append("NULL, NULL)));\n");
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
