// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"

//******************************************************************************
// CGfield definitions
//******************************************************************************
void CGfield::setData(const char* _name, CGloader* loader)
{
	name.assign(_name);
	loader->checkIDs(classPtr->getName(), name);
}

void CGfield::setGetter(const char* txt, CGloader* loader)
{
	if(getter.empty())
		getter = txt;
	else if(getter != txt)
		throw exception(string("Multiple getters defined in class ").append(classPtr->getName()).append(", old - '").append(txt).append("', new - '").append(txt).append("'").c_str());
	loader->checkIDs(classPtr->getName(), getter);
}

void CGfield::setSetter(const char* txt, CGloader* loader)
{
	if(setter.empty())
		setter = txt;
	else if(setter != txt)
		throw exception(string("Multiple setters defined in class ").append(classPtr->getName()).append(", old - '").append(txt).append("', new - '").append(txt).append("'").c_str());
	loader->checkIDs(classPtr->getName(), setter);
}

void CGfield::setMoveSetter(const char* txt, CGloader* loader)
{
	if(movesetter.empty())
		movesetter = txt;
	else if(movesetter != txt)
		throw exception(string("Multiple move setters defined in class ").append(classPtr->getName()).append(", old - '").append(txt).append("', new - '").append(txt).append("'").c_str());
	loader->checkIDs(classPtr->getName(), movesetter);
}

void CGfield::setDefaultInt(const char* txt, bool neg, CGloader* loader)
{
	CGmain* mainObj = loader->getMainObj();
	const map<string, string>* psec = mainObj->findIniSection("DEFAULT");
	if(psec)
	{
		const map<string, string>& nameDefs = *psec;
		auto res = nameDefs.find(txt);
		if(res != end(nameDefs))
		{
			defValue = res->second;
		}
		else
			defValue = neg?string("-").append(txt):txt;
	}
	else
		defValue = neg?string("-").append(txt):txt;
}

void CGfield::checkChannels(CGloader* pldr, CGdocument* pdoc, CGstruct* pcls)
{
	if(properties & CG_CLASS_EQUAL_MASK)
	{
		pcls->setEqualByField(properties);
	}
	if (properties & CG_CLASS_SUBSCR_LEAFNODE)
	{
		pcls->setProperty(CG_CLASS_SUBSCR_MASK, CG_CLASS_SUBSCR_LEAFNODE);
	}
	if (properties & CG_AUTO_UPDATE_MASK)
	{
		pcls->setProperty(CG_AUTO_UPDATE_MASK, CG_AUTO_UPDATE_MASK);
	}

	bool cont = false;
	for(auto& elem : pcChannels)
	{
		if(elem.first == "default")
		{
			continue;
		}
		if(pldr->globalSmart.find(elem.first) != end(pldr->globalSmart))
		{
			std::get<0>(elem.second) = true;		// set smart
			continue;
		}
		if(pldr->global.find(elem.first) != end(pldr->global))
		{
			continue;
		}
		for(auto& evs : pldr->virtSmart)
		{
			if(evs.second.find(elem.first) != end(evs.second))
			{
				string bname;
				if((properties & CG_CLASS_INH_MASK) == CG_CLASS_INH_BASEVIRT)
				{
					if(evs.first != pcls->getBaseClass())
						throw exception(string("Channel '").append(elem.first).append("' used in 'VirtualSmart' in class '").append(classPtr->getName()).append("' field '").append(name).append("' with different base class '").append(evs.first).append("'").c_str());
					bname = evs.first;
				}
				else
				{
					bname = pcls->getName();	// this class is base class
				}

				std::get<0>(elem.second) = true;
				addBaseClass(pldr, bname, elem);
				cont = true;
				break;
			}
		}
		if(cont) continue;
		for(auto& ev : pldr->virt)
		{
			if(ev.second.find(elem.first) != end(ev.second))
			{
				string bname;
				if((properties & CG_CLASS_INH_MASK) == CG_CLASS_INH_BASEVIRT)
				{
					if(ev.first != pcls->getBaseClass())
						throw exception(string("Channel '").append(elem.first).append("' used in 'Virtual' in class '").append(classPtr->getName()).append("' field '").append(name).append("' with different base class '").append(ev.first).append("'").c_str());
					bname = ev.first;
				}
				else
				{
					bname = pcls->getName();	// this class is base class
				}

				std::get<0>(elem.second) = false;
				addBaseClass(pldr, bname, elem);
				cont = true;
				break;
			}
		}
		if(cont) continue;
		auto it = pldr->localSmart.find(pdoc->getName());
		if(it != end(pldr->localSmart))
		{
			if(it->second.find(elem.first) != end(it->second))
			{
				std::get<0>(elem.second) = true;
				continue;
			}
		}
		it = pldr->local.find(pdoc->getName());
		if(it != end(pldr->local))
		{
			if(it->second.find(elem.first) != end(it->second))
			{
				std::get<0>(elem.second) = false;
				continue;
			}
		}
		throw exception(string("Channel '").append(elem.first).append("' description is not found in class '").append(classPtr->getName()).append("' field '").append(name).append("'").c_str());
	}
}

inline void CGfield::addBaseClass(CGloader* pldr, const string& name, const pair<string, tuple<bool,bool,bool>>& dat)
{
	auto pit1 = pldr->getMainObj()->baseClasses.insert(make_pair(name, make_pair(map<string, tuple<bool, bool, bool>>(), uint64_t(0))));		// main list of base classes
	auto pit2 = pit1.first->second.first.insert(make_pair(dat.first, make_tuple(false, false, false)));
	if(std::get<0>(dat.second))
		std::get<0>(pit2.first->second) = true;
	if(std::get<1>(dat.second))
		std::get<1>(pit2.first->second) = true;
	if(std::get<2>(dat.second))
		std::get<2>(pit2.first->second) = true;
	pit1.first->second.second |= properties;
}

