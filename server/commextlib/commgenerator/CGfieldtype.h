// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGfieldtype_h_included
#define CGfieldtype_h_included

//******************************************************************************
// Base class for structures
//******************************************************************************
class CGfieldtype sealed 
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGfieldtype(CGstruct* classPtr_, CGfield* fieldPtr_)
		:	classPtr(classPtr_)
		,	fieldPtr(fieldPtr_)
		,	properties(0)
		,	arrayIndex(0)
	{}

	CGfieldtype(CGfieldtype&& other)
		:	fieldType(move(other.fieldType))
		,	classPtr(other.classPtr)
		,	fieldPtr(other.fieldPtr)
		,	subTypes(move(other.subTypes))
		,	indexedType(move(other.indexedType))
		,	baseContType(move(other.baseContType))
		,	arrayIndex(other.arrayIndex)
		,	zeroSubType(move(other.zeroSubType))
		,	properties(other.properties)
		, fieldJavaType(other.fieldJavaType)
	{
		other.arrayIndex = 0;
		other.properties = 0;
	}

	CGfieldtype& operator =(CGfieldtype&& other)
	{
		fieldType = move(other.fieldType);
		classPtr = other.classPtr;
		fieldPtr = other.fieldPtr;
		subTypes = move(other.subTypes);
		indexedType = move(other.indexedType);
		baseContType = move(other.baseContType);
		arrayIndex = other.arrayIndex;
		other.arrayIndex = 0;
		zeroSubType = move(other.zeroSubType);
		properties = other.properties;
		other.properties = 0;
		fieldJavaType = move(other.fieldJavaType);
		return *this;
	}

	virtual ~CGfieldtype()
	{}

//==============================================================================
// Parser interface section
//==============================================================================
public:
	void setType(const char* type, CGloader* loader);
	void addSubType(unique_ptr<CGfieldtype>& st)
	{
		subTypes.push_back(move(st));
	}
	void setArray(const string& idx, CGloader* loader);

	void setUnsigned(void) {properties |= CG_FIELDTYPE_UNSIGNED;}
	void setDirectIndex(void) {properties |= CG_FIELDTYPE_DIRECTINDEX;}
	void setContainer(void) {properties |= CG_FIELDTYPE_CONTAINER;}
	void setArray(void) {properties |= CG_FIELDTYPE_ARRAY;}
	void setPointer(void) {properties |= CG_FIELDTYPE_POINTER;}
	void setJavaType(const char* type)
	{
		fieldJavaType = type;
	}

//==============================================================================
// Defined interface section
//==============================================================================
public:
	CGstruct* getClass(void) {return classPtr;}
	CGfield* getField(void) {return fieldPtr;}
	const CGstruct* getClass(void) const {return classPtr;}
	const CGfield* getField(void) const {return fieldPtr;}

	bool isUnsigned(void) const {return !!(properties & CG_FIELDTYPE_UNSIGNED);}
	bool isDirectIndex(void) const {return !!(properties & CG_FIELDTYPE_DIRECTINDEX);}
	bool isContainer(void) const {return !!(properties & CG_FIELDTYPE_CONTAINER);}
	bool isArray(void) const {return !!(properties & CG_FIELDTYPE_ARRAY);}
	bool isPointer(void) const {return !!(properties & CG_FIELDTYPE_POINTER);}

	const vector<unique_ptr<CGfieldtype>>&	getSubTypes(void) const {return subTypes;}
	size_t getArraySize(void) const {return arrayIndex;}
	string getArraySizeString(void) const {return to_string(arrayIndex);}
	string getFieldTypeStr(typeConverter conv = nullptr) const
	{
		return conv?conv(fieldType):fieldType;
	}
	string getBaseContainerType(typeConverter conv = nullptr) const 
	{
		return conv?conv(baseContType):baseContType;
	}
	string getIndexedType(typeConverter conv = nullptr) const 
	{
		if(subTypes.size())
			return conv?conv(subTypes[0]->getFieldTypeStr()):subTypes[0]->getFieldTypeStr();
		else
			return conv?conv(indexedType):indexedType;
	}
	//size_t getIndexAmount(void) const
	//{
	//	if(subTypes.size())
	//		return subTypes.size();
	//	else if(indexedType.length())
	//		return 1;
	//	else
	//		return 0;
	//}
	const CGfieldtype* getZeroSubType(void) const 
	{
		if(subTypes.size())
			return subTypes[0].get();
		else if(zeroSubType)
			return zeroSubType.get();
		else
			return nullptr;
	}
	string getFullTypeString(typeConverter conv = nullptr) const
	{
		string s;

		if(subTypes.size())
		{
			s.append(conv?conv(fieldType):fieldType).append("<").append(subTypes[0]->getFullTypeString(conv));
			for(auto i = 1; i < subTypes.size(); ++i)
			{
				s.append(", ").append(subTypes[i]->getFullTypeString(conv));
			}
			s.append(">");
		}
		else if(indexedType.length() && conv)
		{
			s.append(conv(baseContType)).append("<").append(conv(indexedType)).append(">");
		}
		else
		{
			s.append(conv?conv(fieldType):fieldType);
		}

		return s;
	}
	string getJavaType() const { return fieldJavaType; }

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGfieldtype(const CGfieldtype& other) = delete;
	CGfieldtype& operator =(const CGfieldtype& other) = delete;

//==============================================================================
// Data section
//==============================================================================
private:
	CGstruct*							classPtr;
	CGfield*							fieldPtr;
	string								fieldType;
	vector<unique_ptr<CGfieldtype>>		subTypes;
	uint32_t							properties;
	size_t								arrayIndex;
	string								indexedType;
	string								baseContType;
	unique_ptr<CGfieldtype>				zeroSubType;
	string								fieldJavaType;
};

#endif	// CGfieldtype_h_included
