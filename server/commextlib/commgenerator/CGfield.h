// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGfield_h_included
#define CGfield_h_included

class CGstruct;
//******************************************************************************
// Class field info
//******************************************************************************
class CGfield sealed : public CGbaseobj, public CGbaseparsecompose
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGfield(CGstruct* classPtr_)
		:	CGbaseobj("", objTypes::gTypeField)
		,	CGbaseparsecompose(classPtr_->getProperty(), classPtr_->getPcChannels(), classPtr_->getUpdateHandler())
		,	classPtr(classPtr_)
		,	leafnodeIndex(-1)
		,	fieldIndex(-1)
		,	secUpdateAdd(false)
		, javaFilterCode(-1)
		, javaSortCode(-1)
		, useChannels(false)
	{
		fieldIndex = classPtr_->getFieldIndex();
	}

	CGfield(CGfield&& other)
		:	CGbaseobj(move(other))
		,	CGbaseparsecompose(move(other))
		,	classPtr(other.classPtr)
		,	fieldType(move(other.fieldType))
		,	getter(move(other.getter))
		,	setter(move(other.setter))
		,	movesetter(move(other.movesetter))
		,	leafnodeIndex(other.leafnodeIndex)
		,	fieldIndex(other.fieldIndex)
		,	defValue(move(other.defValue))
		,	refClass(move(other.refClass))
		,	selectConvertFuncName(move(other.selectConvertFuncName))
		,	specialFieldFuncName(move(other.specialFieldFuncName))
		,	fieldModificator(move(other.fieldModificator))
		,	secUpdateAdd(other.secUpdateAdd)
		, javaFilterCode(other.javaFilterCode)
		, javaSortCode(other.javaSortCode)
		, useChannels(other.useChannels)
	{
		other.leafnodeIndex = -1;
		other.fieldIndex = -1;
		other.secUpdateAdd = false;
	}

	CGfield& operator =(CGfield&& other)
	{
		CGbaseobj::operator = (move(other));
		CGbaseparsecompose::operator = (move(other));
		classPtr = other.classPtr;
		fieldType = move(other.fieldType);
		getter = move(other.getter);
		setter = move(other.setter);
		movesetter = move(other.movesetter);
		defValue = move(other.defValue);
		refClass = move(other.refClass);
		selectConvertFuncName = move(other.selectConvertFuncName);
		specialFieldFuncName = move(other.specialFieldFuncName);
		fieldModificator = move(other.fieldModificator);
		secUpdateAdd = other.secUpdateAdd;
		javaFilterCode = other.javaFilterCode;
		javaSortCode = other.javaSortCode;
		other.secUpdateAdd = false;
		useChannels = other.useChannels;
		other.useChannels = false;
		return *this;
	}

	virtual ~CGfield()
	{}

//==============================================================================
// Parser interface section
//==============================================================================
public:
	void setData(const char* _name, CGloader* loader);
	void setType(unique_ptr<CGfieldtype>& st)
	{
		fieldType = move(st);
	}
	void setGetter(const char* txt, CGloader* loader);
	void setSetter(const char* txt, CGloader* loader);
	void setMoveSetter(const char* txt, CGloader* loader);
	void setLeafNodeIndex(int32_t idx)
	{
		leafnodeIndex = idx;
	}
	void setFieldIndex(int32_t idx)
	{
		fieldIndex = idx;
	}
	void setDefaultStr(const char* txt_, CGloader* loader)
	{
		defValue = txt_;
		if(*(defValue.begin()) == '\"')
		{
			defValue.erase(defValue.begin());
		}
		if(*(defValue.end()-1) == '\"')
		{
			defValue.erase(defValue.end()-1);
		}
		//while(*(defValue.begin()) == ' ')
		//	defValue.erase(defValue.begin());
	}
	void setRefClass(const char* txt1, const char* txt2,CGloader* loader)
	{
		refClass.first = txt1;
		refClass.second = txt2;
	}
	void setDefaultInt(const char* txt, bool neg, CGloader* loader);
	void setSelConvertFuncName(const char* txt_)
	{
		selectConvertFuncName = txt_;
	}
	void setSpecialFieldFuncName(const char* txt_)
	{
		specialFieldFuncName = txt_;
	}
	void setSecUpdateAdd()
	{
		secUpdateAdd = true;
	}
	void setFieldModificator(const char* txt_)
	{
		fieldModificator = txt_;
	}
	void setJavaFilterCode(const char* v)
	{
		javaFilterCode = atoi(v);
	}
	void setJavaSortCode(const char* v)
	{
		javaSortCode = atoi(v);
	}
	void addJavaFilterOperands(const char* s)
	{
		javaFilterOperands.push_back(s);
	}
	void usePCChannels() 
	{ 
		useChannels = true; 
	}
//==============================================================================
// Defined interface section
//==============================================================================
public:
	// Input info check
	void checkChannels(CGloader* pldr, CGdocument* pdoc, CGstruct* pcls);
	CGfieldtype* getFieldType(void) const {return fieldType.get();}
	const string& getSetter(void) const {return setter;}
	const string& getMoveSetter(void) const {return movesetter;}
	const string& getGetter(void) const {return getter;}
	const string& getClassName(void) const {return classPtr->getName();}
	CGstruct* getClass(void) const {return classPtr;}
	const int32_t getLeafNodeIndex(void) const {return leafnodeIndex;}
	string getLeafNodeIndexString(void) const {return to_string(leafnodeIndex);}
	const int32_t getFieldIndex(void) const {return fieldIndex;}
	string getFieldIndexString(void) const {return to_string(fieldIndex);}
	const pair<string, string>& getRefClass(void) const {return refClass;}

	// Wrappers
	bool isUnsigned(void) const {return fieldType->isUnsigned();}
	bool isDirectIndex(void) const {return fieldType->isDirectIndex();}
	bool isContainer(void) const {return fieldType->isContainer();}
	bool isArray(void) const {return fieldType->isArray();}
	bool isPointer(void) const {return fieldType->isPointer();}
	string getFullTypeString(typeConverter conv = nullptr) const {return fieldType->getFullTypeString(conv);}
	size_t getArraySize(void) const  {return fieldType->getArraySize();}
	string getArraySizeString(void) const {return fieldType->getArraySizeString();}
	string getFieldTypeStr(typeConverter conv = nullptr) const {return fieldType->getFieldTypeStr(conv);}
	string getBaseContainerType(typeConverter conv = nullptr) const {return fieldType->getBaseContainerType(conv);} 
	//string getIndexedType(typeConverter conv = nullptr) const {return fieldType->getIndexedType(conv);}
	//size_t getIndexAmount(void) const {return fieldType->getIndexAmount();}
	const CGfieldtype* getZeroSubType(void) const {return fieldType->getZeroSubType();} 
	const string& getDefault(void) const {return defValue;}
	const string& getSelConvertFuncName(void) const {return selectConvertFuncName;}
	const string& getSpecialFieldFuncName(void) const {return specialFieldFuncName;}
	bool getSecUpdateAdd(void) const {return secUpdateAdd;}
	const string& getFieldModificator(void) const {return fieldModificator;}
	const vector<string>& getJavaFilterOperands(void) const {return javaFilterOperands;}
	const int getJavaFilterCode(void) const { return javaFilterCode; }
	const int getJavaSortCode(void) const { return javaSortCode; }
	const bool isUsingChannels() const { return useChannels; }

private:
	void addBaseClass(CGloader* pldr, const string& name, const pair<string, tuple<bool,bool,bool>>& dat);

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGfield(const CGfield& other);
	CGfield& operator =(const CGfield& other);

//==============================================================================
// Data section
//==============================================================================
private:
	CGstruct*						classPtr;
	unique_ptr<CGfieldtype>			fieldType;
	string							getter;
	string							setter;
	string							movesetter;
	uint32_t						leafnodeIndex;
	uint32_t						fieldIndex;
	string							defValue;
	pair<string, string>			refClass;
	string							selectConvertFuncName;
	string							specialFieldFuncName;
	bool							secUpdateAdd;
	string							fieldModificator;

	int								javaFilterCode;
	int								javaSortCode;
	vector<string>					javaFilterOperands;
	bool							useChannels;
};

#endif	// CGfield_h_included
