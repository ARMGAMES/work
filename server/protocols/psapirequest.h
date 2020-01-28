#ifndef psapirequest_h_included
#define psapirequest_h_included

#include "pplib.h"

// what to search in the table name
#define PSAPI_DANIELS_ROOM_TABLE        "Daniel's Rm"
#define PSAPI_SUPERSTAR_SHOWDOWN_TABLE  "Showdown"
// strings for typeOfShow parameter of PSAPI request
#define PSAPI_TOS_REGULAR               "Regular"
#define PSAPI_TOS_SUPERSHOWDOWN         "SuperShowdown"
#define PSAPI_TOS_DANIELSROOM           "DanielsRoom"

class PsapiPublishHandReply
{
public:
	PString url;
	PString longText;
	PString shortText;
	PsapiPublishHandReply()
	{
	}
	PsapiPublishHandReply( CommMsgParser& parser)
	{
		parse(parser);
	}
	CommMsgBody& compose( CommMsgBody& body) const
	{
		body.composeString(url)
			.composeString(longText)
			.composeString(shortText)
			;
		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser)
	{
		parser.parseStringP(url)
			  .parseStringP(longText)
			  .parseStringP(shortText)
			  ;
		return parser;
	}
	void print() const
	{
		PLog("PsapiPublishHandReply:\r\n"
			 "url : n%s\r\n"
			 "longText: n%s\r\n"
			 "shortText: n%s\r\n",
			  url.c_str(),
			  longText.c_str(),
			  shortText.c_str()
			  );
	}
};

class PsapiPublishHandRequest
{
public:
    PString tournamentName;
	PString tableName;
	PString nick;
	PString	handText;
	bool	showHoleCards;
	bool	showRealTableName;
	bool	showAmountInBB;
	bool	showRealNicks;
	bool	showHandId;
	bool	showHeroNick;
	PString heroTheme;
	PString opponentTheme;
	PString longText;
	PString shortText;
    PString typeOfShow; // Regular (Default), SuperShowdown, DanielsRoom
	PString	language;
 // PYR-19288
	PString methodOfShare;
	PString client;


public:
	PsapiPublishHandRequest()
	{	
		showHoleCards		= true;
		showRealTableName	= true;
		showAmountInBB		= false;
		showRealNicks		= true; // see PYR-18597 BRD 5.4
		showHandId			= false; // see PYR-18597 BRD 5.4
		showHeroNick		= true; // always show posting player's nick - see PYR-18597 BRD 5.4
		heroTheme			= "Hero";
		opponentTheme		= "Player (%n)"; // PYR-19288
        // if typeOfShow is empty it is calculated in the Lobby based on tableName
	}
	PsapiPublishHandRequest( CommMsgParser& parser)
	{
		parse(parser);
	}
	CommMsgBody& compose( CommMsgBody& body) const
	{
		body.composeString(tournamentName)
			.composeString(tableName)
			.composeString(nick)
			.composeString(handText)
			.composeString(heroTheme)
			.composeString(opponentTheme)
			.composeString(longText)
			.composeString(shortText)
			.composeBOOL(showHoleCards)
			.composeBOOL(showRealTableName)
			.composeBOOL(showAmountInBB)
			.composeBOOL(showRealNicks)
			.composeBOOL(showHandId)
			.composeBOOL(showHeroNick)
            .composeString(typeOfShow)
			.composeString(language)
 // PYR-19288
			.composeString(methodOfShare)
			.composeString(client)
			;
		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser)
	{
		parser.parseStringP(tournamentName)
			  .parseStringP(tableName)
			  .parseStringP(nick)
			  .parseStringP(handText)
			  .parseStringP(heroTheme)
			  .parseStringP(opponentTheme)
			  .parseStringP(longText)
			  .parseStringP(shortText)
			  .parseBOOL(showHoleCards)
			  .parseBOOL(showRealTableName)
			  .parseBOOL(showAmountInBB)
			  .parseBOOL(showRealNicks)
			  .parseBOOL(showHandId)
			  .parseBOOL(showHeroNick)
              .parseStringP(typeOfShow)
			  ;
		// remove conditional parsing after Oct 2011
		if( !parser.parseEnded())
		{
			parser.parseStringP(language);
            // PYR-19288
            // remove conditional parsing after Nov 2011
            if( !parser.parseEnded())
            {
                parser.parseStringP(methodOfShare)
                      .parseStringP(client);
            }
		}
 
		return parser;
	}
	void print() const
	{
		PLog("PublishHandRequest:\r\n"
			"hand:\r\n%s\r\n",
			handText.c_str()
			);
	}
};

#endif //psapirequest_h_included
