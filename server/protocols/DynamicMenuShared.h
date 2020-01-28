////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef DYNAMICMENUSHARED_H
#define DYNAMICMENUSHARED_H

// PYR-21301

#ifndef PROTOCOL_CLIENT_ONLY
#include "commlib.h"
#endif

#include "pyrtime.h"
#include "composecolls.h"
#include "i18n.strmap.h"

class ClientDynamicMenus
{
public:
	enum EnumClientDynamicMenuID
	{
		eITBOPMENU = 1,
	};

	struct Item
	{
		UINT32 id;
		PString extra;
		I18nPString name;		

		void compose(CommMsgBody& body) const
		{
			body.composeUINT32(id)
				.composeString(extra);
			name.compose(body);
		}

		void parse(CommMsgParser& parser)
		{
			parser
				.parseUINT32(id)
				.parseStringP(extra);
			name.parse(parser);
		}
	};

	struct ClientDynamicMenu
	{
		bool enabled; 
		UINT32 menuId;
		UINT32 siteMask;
		vector<Item> items;

		void compose(CommMsgBody& body) const
		{
			body
				.composeBOOL(enabled)
				.composeUINT32(menuId)
				.composeUINT32(siteMask);
			composeVectorOfMessageBodiesOfComposableValues(items, body);
		}

		void parse(CommMsgParser& parser)
		{
			parser
				.parseBOOL(enabled)
				.parseUINT32(menuId)
				.parseUINT32(siteMask);
			parseVectorOfParsableMessageBodies(items, parser);			
		}
	};	

	vector<ClientDynamicMenu> menus;

	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(menus, body);
	}

	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(menus, parser);		
	}

	ClientDynamicMenu* getMenu( UINT32 siteId, UINT32 menuId )
	{
		for( int i = 0; i < menus.size(); i++ )
		{
			if( ( menus[i].menuId == menuId ) && ( menus[i].siteMask & siteId ) )
				return &menus[i];
		}
		return NULL;
	}
};

#endif // DYNAMICMENUSHARED_H
