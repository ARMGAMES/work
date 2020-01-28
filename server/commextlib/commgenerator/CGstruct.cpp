// Copyright (C) Amaya Software Ltd. All rights reserved.

#include "CGapp.h"

//******************************************************************************
// CGstruct definitions
//******************************************************************************
uint64_t CGstruct::counter = 0;

//******************************************************************************
void CGstruct::checkSubscr(CGfield* fld, CGloader* loader)
{
	if (fld->hasProperty(CG_CLASS_SUBSCR_LEAFNODE))
	{
		fld->setLeafNodeIndex(leafNodeIndex++);
	}
}

void CGstruct::checkChannels(CGloader* pldr, CGdocument* pdoc)
{
	for(auto& obj : objects)
	{
		if(obj->getObjType() == objTypes:: gTypeField)
		{
			CGfield* fld = dynamic_cast<CGfield*>(obj.get());
			if(!fld)
			{
				string err;
				err.append("Wrong dynamic cast to field, file-'").append(pdoc->getFileName()).append("' class-'").append(name).append("' object-'").append(obj->getName()).append("'");
				throw exception(err.c_str());
			}
			fld->checkChannels(pldr, pdoc, this);

			auto fldchnls = fld->getPcChannels();		// class has to have all channels from fields
			for(auto fldch : fldchnls)
			{
				auto it = pcChannels.insert(fldch).first;
				if(std::get<0>(fldch.second))
					std::get<0>(it->second) = true;
				if(std::get<1>(fldch.second))
					std::get<1>(it->second) = true;
				if(std::get<2>(fldch.second))
					std::get<2>(it->second) = true;
			}
		}
	}
}

void CGstruct::checkVirtualChannels1(CGloader* pldr)
{
	if(hasProperty(CG_CLASS_SUBSCR_LEAFNODE))
	{
		addAccess("public");
		unique_ptr<CGbaseobj> pfobj1;
		unique_ptr<CGfieldtype> pftobj1;
		pfobj1 = unique_ptr<CGbaseobj>(new CGfield(this));
		CGfield* pfield1 = (CGfield*)pfobj1.get();
		pftobj1 = unique_ptr<CGfieldtype>(new CGfieldtype(this, pfield1));

		pftobj1->setType("bool", pldr);
		pfield1->setData("published", pldr);
		pfield1->setType(pftobj1);
		pfield1->setNull();
		addField(pfobj1);

		//unique_ptr<CGbaseobj> pfobj2;
		//unique_ptr<CGfieldtype> pftobj2;
		//pfobj2 = unique_ptr<CGbaseobj>(new CGfield(this));
		//CGfield* pfield2 = (CGfield*)pfobj2.get();
		//pftobj2 = unique_ptr<CGfieldtype>(new CGfieldtype(this, pfield2));

		//pftobj2->setType("CommServerPublisher::UpdateMsg", pldr);
		//pfield2->setData("update", pldr);
		//pfield2->setType(pftobj2);
		//pfield2->setNull();
		//addField(pfobj2);

		//unique_ptr<CGbaseobj> pfobj3;
		//unique_ptr<CGfieldtype> pftobj3;
		//pfobj3 = unique_ptr<CGbaseobj>(new CGfield(this));
		//CGfield* pfield3 = (CGfield*)pfobj3.get();
		//pftobj3 = unique_ptr<CGfieldtype>(new CGfieldtype(this, pfield3));

		//pftobj3->setType("CommServerPublisher", pldr);
		//pfield3->setData("publisher", pldr);
		//pfield3->setType(pftobj3);
		//pfield3->setNull();
		//addField(pfobj3);

		//unique_ptr<CGbaseobj> pfobj4;
		//unique_ptr<CGfieldtype> pftobj4;
		//pfobj4 = unique_ptr<CGbaseobj>(new CGfield(this));
		//CGfield* pfield4 = (CGfield*)pfobj4.get();
		//pftobj4 = unique_ptr<CGfieldtype>(new CGfieldtype(this, pfield3));

		//pftobj4->setType("INT32", pldr, false);
		//pfield4->setData("publisherPathLen", pldr);
		//pfield4->setType(pftobj4);
		//pfield4->setNull();
		//addField(pfobj4);

		//unique_ptr<CGbaseobj> pfobj5;
		//unique_ptr<CGfieldtype> pftobj51;
		//unique_ptr<CGfieldtype> pftobj52;
		//pfobj5 = unique_ptr<CGbaseobj>(new CGfield(this));
		//CGfield* pfield5 = (CGfield*)pfobj5.get();
		//pftobj51 = unique_ptr<CGfieldtype>(new CGfieldtype(this, pfield5));
		//pftobj52 = unique_ptr<CGfieldtype>(new CGfieldtype(this, pfield5));

		//pftobj51->setType("INT32", pldr, false);
		//pftobj52->addSubType(pftobj51);
		//pftobj52->setType("vector", pldr, false);
		//pfield5->setData("publisherPath", pldr);
		//pfield5->setType(pftobj52);
		//pfield5->setNull();
		//addField(pfobj5);

	}
	if (hasProperty(CG_CLASS_INH_VIRT))
	{
		auto itb = pldr->getMainObj()->baseClasses.find(baseClass);
		if(itb != end(pldr->getMainObj()->baseClasses))
		{
			for(auto& elem : itb->second.first)
			{
				auto itd = pcChannels.insert(make_pair(elem.first, make_tuple(false, false, false))).first;
				if(std::get<0>(elem.second))
					std::get<0>(itd->second) = true;
				if(std::get<1>(elem.second))
					std::get<1>(itd->second) = true;
				if(std::get<2>(elem.second))
					std::get<2>(itd->second) = true;
			}
			itb->second.second |= properties;
		}
	}
}

void CGstruct::checkVirtualChannels2(CGloader* pldr)
{
	auto itb = pldr->getMainObj()->baseClasses.find(name); // if this class is base for others
	if(itb != end(pldr->getMainObj()->baseClasses))
	{
		for(auto& elem : itb->second.first)
		{
			auto itd = pcChannels.insert(make_pair(elem.first, make_tuple(false, false, false))).first;
			if(std::get<0>(elem.second))
				std::get<0>(itd->second) = true;
			if(std::get<1>(elem.second))
				std::get<1>(itd->second) = true;
			if(std::get<2>(elem.second))
				std::get<2>(itd->second) = true;
		}
		if (itb->second.second & CG_CLASS_EQUAL_BASE)
		{
			properties |= CG_CLASS_EQUAL_ON;
		}
		if (itb->second.second & CG_CLASS_COPYBASE_ON)
		{
			properties |= CG_CLASS_COPYBASE_ON;
		}
		if (itb->second.second & CG_CLASS_MOVEBASE_ON)
		{
			properties |= CG_CLASS_MOVEBASE_ON;
		}
		if (itb->second.second & CG_CLASS_UPDATE_ON)
		{
			properties |= CG_CLASS_UPDATE_ON;
		}
	}
}

