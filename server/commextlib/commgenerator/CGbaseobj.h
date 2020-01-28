// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGbaseinputobj_h_included
#define CGbaseinputobj_h_included

//******************************************************************************
// Base class for all elements
//******************************************************************************
class CGbaseobj
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGbaseobj(const char* _name, objTypes _objType)
		:	name(_name)
		,	objType(_objType)
	{}

	CGbaseobj(CGbaseobj&& other)
		:	name(move(other.name))
		,	objType(other.objType)
		,	txt(move(other.txt))
		,	objects(move(other.objects))
		,	nameJava(move(other.nameJava))
		,	descJava(move(other.descJava))
	{
		other.objType = objTypes::gTypeUnknown;
	}

	CGbaseobj& operator =(CGbaseobj&& other)
	{
		name = move(other.name);
		objType = other.objType;
		other.objType = objTypes::gTypeUnknown;
		txt = move(other.txt);
		objects = move(other.objects);
		nameJava = move(other.nameJava);
		descJava = move(other.descJava);
		return *this;
	}

	virtual ~CGbaseobj()
	{}

//==============================================================================
// Parser interface section
//==============================================================================
public:
	void addComment(const char* input)
	{
		objects.push_back(unique_ptr<CGbaseobj>(new CGbaseobj("", objTypes::gTypeComment)));
		objects.back().get()->setFormattedTxt(input, true);
	}
	void addInclude(const char* input)
	{
		objects.push_back(unique_ptr<CGbaseobj>(new CGbaseobj("", objTypes::gTypeInclude)));
		objects.back().get()->setFormattedTxt(input, true);
	}
	void addText(const char* sel, const char* input)
	{
		objects.push_back(unique_ptr<CGbaseobj>(new CGbaseobj(sel, objTypes::gTypeText)));
		objects.back().get()->setFormattedTxt(input, false);
	}
	void addEmptyLine(const char* name_)
	{
		objects.push_back(unique_ptr<CGbaseobj>(new CGbaseobj(name_, objTypes::gTypeEmptyLine)));
	}

	void setJavaName(const char* n) { nameJava = n;}
	void setJavaType(const char* n) { typeJava = n;}
	void setJavaDesc(const char* d) { descJava = d; }
//==============================================================================
// Defined interface section
//==============================================================================
public:
	const string& getName(void) const {return name;}
	const string& getJavaName(void) const {return nameJava;}
	const string& getJavaTypeAlias(void) const { return typeJava; }
	const string& getJavaDescr(void) const { return descJava; }
	objTypes getObjType(void) const {return objType;}
	const string& getTxt(void) const {return txt;}
	void setFormattedTxt(const char* txt, bool eraseFirstWhites);	// add formatted txt
	void setTxt(const char* t) {txt = t;}
	void setName(const char* n) {name = n;}
	const vector<unique_ptr<CGbaseobj>>& getObjects(void) const {return	objects;}
	const CGbaseobj* findObject(objTypes type_, const string& name_) const
	{
		if(((type_ == objType) || (type_ == objTypes::gTypeUnknown)) && (name_ == name))
			return this;
		for(auto& objelem : objects)
		{
			const CGbaseobj* obj = objelem->findObject(type_, name_);
			if(obj) return obj;
		}
		return nullptr;
	}
	string getJavaDescrCut(void) 
	{ 
		auto pos = descJava.find_last_of('\"');
		if ((pos == string::npos) || (pos >= descJava.length()))
			return descJava; 
		else
			return descJava.substr(0, pos);
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGbaseobj() = delete;
	CGbaseobj(const CGbaseobj& other) = delete;
	CGbaseobj& operator =(const CGbaseobj& other) = delete;

//==============================================================================
// Data section
//==============================================================================
protected:
	string							name;			
	objTypes						objType;
	string							txt;			// any txt like comment
	vector<unique_ptr<CGbaseobj>>	objects;		// included objects
	string							nameJava;
	string							typeJava;
	string							descJava;
};

#endif	// CGbaseinputobj_h_included

