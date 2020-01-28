#include <pplib.h>
#include <commlib.h>
#include "commextobjectext.h"


void CommExtObjectEx::parse(CommMsgParser & parser)
{
	while(!parser.parseEnded())
	{
		const char * name;
		parser.parseString(name);
		if (*name == 0)
			return;
		if (!strcmp(name,"idObject"))
			PASSERT3(0);
		MemberMap * memberMap = getMap();
		if (memberMap)
		{
			MemberMap::iterator found = memberMap->find(name);
			if (found != memberMap->end())
			{
				CommExtObjectMember * m = (*found).second;
				m->parse(this,parser);
				continue;
			}
		}
		ItemMap * itemMap = getItemMap();
		if (itemMap)
		{
			ItemMap::iterator found = itemMap->find(name);
			if (found != itemMap->end())
			{
				CommExtObjectItemVector * m = (*found).second;
				m->parse(this,parser);
				continue;
			}
		}

		_CommExtObjectUnknownMember *v;
		if (*name == 'i')
		{
			v = new _CommExtObjectUnknownMemberUINT32();
		}
		else if (*name == 's')
		{
			v = new _CommExtObjectUnknownMemberString();
		}
		else if (*name == 'a')
		{
			v = new _CommExtObjectUnknownMemberUINT64();
		}
		else if (*name == 'b')
		{
			v = new _CommExtObjectUnknownMemberBlock();
		}
		else if (*name == 'd')
		{
			v = new _CommExtObjectUnknownMemberDate();
		}
		else if (*name == 't')
		{
			v = new _CommExtObjectUnknownMemberTime();
		}
		else PASSERT3(0);
		v->parse(parser);
		unknownMembers.push_back(v);
	}
}
void CommExtObjectEx::compose(CommMsgBody & body) const
{
	MemberMap * memberMap = getMap();
	if (memberMap)
	{
		MemberMap::iterator it;
		for (it = memberMap->begin(); it != memberMap->end(); ++it)
		{
			CommExtObjectMember * m = (*it).second;
			body.composeString((*it).first);
			m->compose(this, body);
		}
	}
	ItemMap * itemMap = getItemMap();
	if (itemMap)
	{
		ItemMap::iterator it;
		for (it = itemMap->begin(); it != itemMap->end(); ++it)
		{
			CommExtObjectItemVector * m = (*it).second;
			body.composeString((*it).first);
			m->compose(this, body);
		}
	}

	list <_CommExtObjectUnknownMember *>::const_iterator itl;
	for (itl = unknownMembers.begin(); itl != unknownMembers.end(); ++itl)
	{
		body.composeString((*itl)->name);
		(*itl)->compose(body);
	}

	body.composeString("");
}

CommExtObjectEx::~CommExtObjectEx()
{
	list <_CommExtObjectUnknownMember *>::iterator it;
	for (it = unknownMembers.begin(); it != unknownMembers.end(); ++it)
	{
		delete (*it);
	}
}
