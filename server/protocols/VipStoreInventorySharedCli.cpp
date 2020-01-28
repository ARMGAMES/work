////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "VipStoreInventorySharedCli.h"


InvItemStocksCli2::InvItemStocksCli2( CommMsgParser& parser )
{
	UINT32 size;
	parser.parseUINT32( size);
	stocks.reserve( size );
	for( int i = 0; i < size; ++i)
	{
		CommMsgBody msgBody;
		parser.parseMsgBody(msgBody);
		CommMsgParser msgBodyParser(msgBody);
		stocks.push_back( new InvItemStockCli2(msgBodyParser));
	}
}

////////////////////////////////////////////////////////////////////////////////

InvItemStockCli2::InvItemStockCli2( CommMsgParser& parser ) : available(false)
{
	parser.parseBOOL( available)
		  .parseINT32(stockCount)
		  .parseBOOL(unlimitedStock);

	UINT32 size;
	parser.parseUINT32( size);
	specs.reserve( size );
	for( int i = 0; i < size; ++i)
	{
		const char* spec;
		parser.parseString( spec);
		specs.push_back( new PString( spec));
	}
}

void InvItemStockCli2::print()
{
	PString specsString;
	InvItemSpecsColl::const_iterator it = specs.begin();
	while( it != specs.end())
	{
		specsString.append((*it)->c_str());
		++it;
		if( it != specs.end())
			specsString.append(",");
		else
			break;
	}
	PLog(	"Specs: %s - %s - %d - %s", 
			specsString.c_str(), 
			isAvailable() ? "Available" : "Not Available", 
			stockCount,
			isUnlimitedStock() ? "unlimited" : "limited");
}

////////////////////////////////////////////////////////////////////////////////

void InvItemStocksCli2::print()
{
	PLog("--- Stock");
	InvItemStockColl2_::const_iterator it = stocks.begin();
	while( it != stocks.end())
	{
		(*it)->print(); 
		++it;
	}
}

////////////////////////////////////////////////////////////////////////////////
