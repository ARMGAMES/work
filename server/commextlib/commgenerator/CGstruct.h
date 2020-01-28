// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGstruct_h_included
#define CGstruct_h_included

//******************************************************************************
// Class info
//******************************************************************************
class CGstruct sealed : public CGbaseobj, public CGbaseparsecompose
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGstruct(const char* _name, uint64_t _properties, const map<string, tuple<bool,bool,bool>>& _pcChannels, const string& _updHandler)
		:	CGbaseobj(_name, objTypes::gTypeStruct)
		,	CGbaseparsecompose(_properties, _pcChannels, _updHandler)
		,	leafNodeIndex(0)
		,	fieldIndex(0)
		,	maskDSLMap(false)
		, javaSearchable(false)
	{
		index = ++counter;
	}

	CGstruct(CGstruct&& other)
		:	CGbaseobj(move(other))
		,	CGbaseparsecompose(move(other))
		,	baseClass(move(other.baseClass))
		,	index(other.index)
		,	leafNodeIndex(other.leafNodeIndex)
		,	fieldIndex(other.fieldIndex)
		,	modelType(move(other.modelType))
		,	arcType(move(other.arcType))
		,	JSExtends(move(other.JSExtends))
		,	updateHandlers(move(other.updateHandlers))
		,	updateHandlers2(move(other.updateHandlers2))
		, maskDSLMap(other.maskDSLMap)
		, javaPackage(other.javaPackage)
		, javaSearchable(other.javaSearchable)
		//,	updateHandlersSec(move(other.updateHandlersSec))
	{
		other.index = 0;
		other.leafNodeIndex = 0;
	}

	CGstruct& operator =(CGstruct&& other)
	{
		CGbaseobj::operator = (move(other));
		CGbaseparsecompose::operator = (move(other));
		baseClass = move(other.baseClass);
		modelType = move(other.modelType);
		index = other.index;
		other.index = 0;
		leafNodeIndex = other.leafNodeIndex;
		other.leafNodeIndex = 0;
		fieldIndex = other.fieldIndex;
		other.fieldIndex = 0;
		modelType = move(other.modelType);
		arcType = move(other.arcType);
		JSExtends = move(other.JSExtends);
		updateHandlers = (move(other.updateHandlers));
		updateHandlers2 = (move(other.updateHandlers2));
		maskDSLMap = other.maskDSLMap;
		javaSearchable = other.javaSearchable;
		javaPackage = (move(other.javaPackage));
		//updateHandlersSec = (move(other.updateHandlersSec));
		return *this;
	}

	virtual ~CGstruct()
	{}

//==============================================================================
// Parser interface section
//==============================================================================
public:
	void addAccess(const char* name_)
	{
		objects.push_back(unique_ptr<CGbaseobj>(new CGbaseobj(name_, objTypes::gTypeAccess)));
	}
	CGbaseobj* addField(unique_ptr<CGbaseobj>& obj)
	{
		objects.push_back(move(obj));
		return objects.back().get();
	}
	void setBaseClass(const char* name_)
	{
		baseClass = name_;
	}
	void setModelType(const char* mtype)
	{
		modelType = mtype;
	}
	void setArcType(const char* atype)
	{
		arcType = atype;
	}
	void setJSExtends(const char* txt_)
	{
		JSExtends = txt_;
		if(*(JSExtends.begin()) == '\"')
		{
			JSExtends.erase(JSExtends.begin());
		}
		if(*(JSExtends.end()-1) == '\"')
		{
			JSExtends.erase(JSExtends.end()-1);
		}
	}
	void checkSubscr(CGfield* fld, CGloader* loader);

	void setDSLMask(bool msk) { maskDSLMap = msk; }

	void addJavaPackageSection(const char* s)
	{
		javaPackage.push_back(s);
	}
	void addJavaQueryImplements(const char* s)
	{
		javaQueryImplements.push_back(s);
	}
	void setJavaSearchable()
	{
		javaSearchable = true;
	}

//==============================================================================
// Defined interface section
//==============================================================================
public:

	const string& getBaseClass(void) const 
	{
		return baseClass;
	}
	const string& getModelType(void) const 
	{
		return modelType;
	}
	const string& getArcType(void) const 
	{
		return arcType;
	}
	const string& getJSExtends(void) const 
	{
		return JSExtends;
	}
	void setEqualByField(uint64_t prop)
	{
		properties |= CG_CLASS_EQUAL_BYFIELD | (prop & CG_CLASS_EQUAL_MASK & ~CG_CLASS_EQUAL_ON);
	}
	void setSmartRestr()
	{
		properties |= CG_CLASS_PTRS_SMART_RESTR;
	}
	void checkChannels(CGloader* pldr, CGdocument* pdoc);
	void checkVirtualChannels1(CGloader* pldr);
	void checkVirtualChannels2(CGloader* pldr);
	uint64_t getClassIndex(void) const {return index;}
	int32_t getFieldIndex(void) {return fieldIndex++;}

	const set<string>& getUpdateHandlers(void) const {return updateHandlers;}
	const set<string>& getUpdateHandlers2(void) const {return updateHandlers2;}
	//const set<string>& getUpdateHandlersSec(void) const {return updateHandlersSec;}
	void addUpdateHandler(const string& h) {updateHandlers.insert(h);}
	void addUpdateHandler2(const string& h) {updateHandlers2.insert(h);}
	//void addUpdateHandlerSec(const string& h) {updateHandlersSec.insert(h);}
	bool isDSLMasked() const { return maskDSLMap; }
	const vector<string>& getJavaPackage(void) const { return javaPackage; }
	const vector<string>& getJavaQueryImplements(void) const { return javaQueryImplements; }
	bool isJavaSearhable()const { return javaSearchable; } 
//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGstruct(const CGstruct& other)=delete;
	CGstruct& operator =(const CGstruct& other)=delete;

//==============================================================================
// Data section
//==============================================================================
private:
	string				baseClass;		// base class name if defined 
	uint64_t			index;			// just unique class ID
	int32_t				leafNodeIndex;	// just unique subscription field ID counter in this class
	int32_t				fieldIndex;		// just unique subscription field ID counter in this class
	string				modelType;
	string				arcType;
	string				JSExtends;
	set<string>			updateHandlers;
	set<string>			updateHandlers2;
	//set<string>			updateHandlersSec;
	bool				maskDSLMap;
	vector<string>		javaPackage;
	vector<string>		javaQueryImplements;
	bool				javaSearchable;
private:
	static uint64_t		counter;		// unique class ID counter
};

#endif	// CGstruct_h_included

