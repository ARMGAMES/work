// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"

//******************************************************************************
// CGfieldtype definitions
//******************************************************************************
void CGfieldtype::setType(const char* type, CGloader* loader)
{
	if(isPointer())
	{
		classPtr->setSmartRestr();
	}
	auto it = loader->getFieldSubTypes().find(type);
	if(it != end(loader->getFieldSubTypes()))
	{
		if(subTypes.size() != it->second.subFieldsAmount)
			throw exception(string("Different number of subfields for field type in class '").append(classPtr->getName()).append("', type - '").append(type).append("', must be ").append(to_string(it->second.subFieldsAmount)).c_str());
		if(it->second.directIndex) setDirectIndex();
		indexedType = it->second.subTypeForIndexed;
		baseContType = it->second.baseContainerType;
		setContainer();
		if(!subTypes.size() && indexedType.length())
		{
			zeroSubType = unique_ptr<CGfieldtype>(new CGfieldtype(classPtr, fieldPtr));
			zeroSubType->setType(indexedType.c_str(), loader);
		}
	}
	fieldType = type;
	if(fieldType == "unique_ptr")
		classPtr->setSmartRestr();
}

void CGfieldtype::setArray(const string& idx, CGloader* loader)
{
	if(idx.length())
	{
		setArray();

		CGmain* mainObj = loader->getMainObj();
		const map<string, string>* psec = mainObj->findIniSection("DEFAULT");
		if(psec)
		{
			const map<string, string>& nameDefs = *psec;
			auto res = nameDefs.find(idx);
			if(res != end(nameDefs))
			{
				arrayIndex = stoi(res->second);
			}
			else
				arrayIndex = stoi(idx);
		}
		else
			arrayIndex = stoi(idx);
	}
}

