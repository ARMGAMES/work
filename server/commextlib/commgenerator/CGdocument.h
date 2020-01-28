// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CGdocument_h_included
#define CGdocument_h_included

//******************************************************************************
// Document info
//******************************************************************************
class CGdocument sealed : public CGbaseobj, public CGbaseparsecompose
{
//==============================================================================
// Constructor/destructor section
//==============================================================================
public:
	CGdocument(const char* initStr, const char* fileN, uint64_t	properties_)
		:	CGbaseobj(fileN, objTypes::dTypeDocument)
		,	CGbaseparsecompose(properties_)
		,	fileName(fileN)
	{
		txt = initStr;
	}

	CGdocument(CGdocument&& other)
		:	CGbaseobj(move(other))
		,	CGbaseparsecompose(move(other))
		,	fileName(move(other.fileName))
	{}

	CGdocument& operator =(CGdocument&& other)
	{
		CGbaseobj::operator = (move(other));
		CGbaseparsecompose::operator = (move(other));
		fileName = move(other.fileName);
		return *this;
	}

	virtual ~CGdocument(void){}

//==============================================================================
// Parser interface section
//==============================================================================
public:
	// Add data from file
	CGbaseobj* addClass(const char* name_)
	{
		objects.push_back(unique_ptr<CGbaseobj>(new CGstruct(name_, properties, pcChannels, updHandler)));
		return objects.back().get();
	}

//=============================================================================
// Defined interface section
//==============================================================================
public:
	// Load input files
	void load(CGloader* pldr, const string& fullName);
	// Input info check
	void checkChannels1(CGloader* pldr);
	void checkChannels2(CGloader* pldr);
	void checkChannels3(CGloader* pldr);
	const string& getFileName(void) const {return fileName;}
	const CGstruct* findClass(uint64_t index) const
	{
		for(auto& elem : objects)
		{
			if(elem->getObjType() == objTypes::gTypeStruct)
			{
				const CGstruct* cls = dynamic_cast<const CGstruct*>(elem.get());
				if(!cls)
				{
					string err;
					err.append("Wrong dynamic cast to class, file-'").append(fileName).append("' object-'").append(elem->getName());
					throw exception(err.c_str());
				}
				if(cls->getClassIndex() == index)
					return cls;
			}
		}
		return nullptr;
	}

//==============================================================================
// Prohibited section
//==============================================================================
private:
	CGdocument() = delete;
	CGdocument(const CGdocument& other) = delete;
	CGdocument& operator =(const CGdocument& other) = delete;

//==============================================================================
// Data section
//==============================================================================
private:
	string				fileName;
};

#endif	// CGdocument_h_included
