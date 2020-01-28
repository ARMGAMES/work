////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef WebTokenServer_h_included
#define WebTokenServer_h_included

#include "ppincludebase.h"

#define WEBTOKENSERVER_DELTA		1000

//	Unsolicited messages sent as notifications from WTS to other processes:

//	PYR-62474
#define WTS_U_CASINO_TOKEN_INVALID				500	//	4bs4 - reqId=0,invalidateAllTokens,token,userIntId

// PYR-73672 web token server -> lobbyEtc
#define WTS_U_APP_SESSION_LOGOUT				501	// o - atf protocols\atfgenerated\WebTokenServer_atf.txt

//	PYR-92358
#define WTS_U_NEW_RAM_SESSION					502	// 8o - loginId,userProps


//	PYR-77257. Unsolicited upstream messages to rcsServer.
//	Ensure ranges do not overlay with message definitions in lobby.h
#define WTS_U_TOKEN_EXPIRE_WARNING				2000		//	s44 - token,secondsTillExpiration,idleSeconds
#define WTS_U_TOKEN_INVALID						2001		//	s44 - token,eTokenInvalidReasonCode,validSeconds


// Client <-> Web token server (client connection, no reqId)
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define WTS_Q_GET_TOKEN					1001	// [4] - [tokenType]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define WTS_A_GET_TOKEN					(WEBTOKENSERVER_DELTA + WTS_Q_GET_TOKEN) // 2001 // 6[s|ss] errCode [errDesc | token, signature]

// Cashier/VipStore <-> Web token server
#define CASHIER_WTS_Q_EXTEND_USER_TOKEN_TIME	1002	// 4ssb reqId, userId, tokenStr, extendTime
#define CASHIER_WTS_A_EXTEND_USER_TOKEN_TIME	(WEBTOKENSERVER_DELTA + CASHIER_WTS_Q_EXTEND_USER_TOKEN_TIME)	// 46[s] reqId, errCode [errDesc]

// Cashier <-> Web token server
#define CASHIER_WTS_Q_GET_TOKEN_INFO			1003	// 4ss reqId, userId, tokenStr
#define CASHIER_WTS_A_GET_TOKEN_INFO			(WEBTOKENSERVER_DELTA + CASHIER_WTS_Q_GET_TOKEN_INFO)			// 46[s|4444] reqId, errCode [ errDesc | clientPlatform, clientSite, clientDevice, clientType ]

// AuxHub <-> Web Token server
#define WTS_Q_REMOVE_AUX_SESSIONID				1004	// 4s8 reqId, userId, auxSessionId
#define WTS_A_REMOVE_AUX_SESSIONID				(WEBTOKENSERVER_DELTA + WTS_Q_REMOVE_AUX_SESSIONID)				// 46[s] reqId, errCode [errDesc]

// AuxHub <-> Web Token server, only after AuxHub restart
#define WTS_Q_GET_ALL_ACTIVE_AUX_SESSIONIDS		1005	// 4 reqId
#define WTS_A_GET_ALL_ACTIVE_AUX_SESSIONIDS		(WEBTOKENSERVER_DELTA + WTS_Q_GET_ALL_ACTIVE_AUX_SESSIONIDS)	// 46[s|4<s8ss8>] reqId, errCode [errDesc | num<userId, auxSessionId, countryByIp, regionByIp, appSessionId>]

// WebAccountService <-> Web Token server (client connection, no reqId)
#define WTS_Q_REMOVE_WEBACCOUNTSERVICE_TOKEN	1006	// s token
#define WTS_A_REMOVE_WEBACCOUNTSERVICE_TOKEN	(WEBTOKENSERVER_DELTA + WTS_Q_REMOVE_WEBACCOUNTSERVICE_TOKEN)	// 6[s] errCode [errDesc]

// client <-> web token server (unauthenticated client connection, no reqId)
#define WTS_Q_GET_TOKEN_EXPIRY_TIME				1007 // 4s userIntId, tokenStr
#define	WTS_A_GET_TOKEN_EXPIRY_TIME				(WEBTOKENSERVER_DELTA + WTS_Q_GET_TOKEN_EXPIRY_TIME) // 2[s|4s] errCode [errMsg | tokenValidTimeInSec,userId]

// logindbm <-> web token server
#define WTS_Q_LDBM_APPSESSION_REMOVED			1008 // 4<8> num<appSessionId>
#define WTS_A_LDBM_APPSESSION_REMOVED			(WEBTOKENSERVER_DELTA + WTS_Q_LDBM_APPSESSION_REMOVED) // 2[s] errCode [errMsg]

// server <-> web token server
#define WTS_Q_CHECK_TOKEN						1009 // 44s reqId, userIntId, tokenStr
#define	WTS_A_CHECK_TOKEN						(WEBTOKENSERVER_DELTA + WTS_Q_CHECK_TOKEN) // 42[s] reqId, errCode [errMsg]

// lobbyetc <-> web token server
#define WTS_Q_EXPIRE_USER_ALL_TOKENS			1010 // 4s4 reqId, userId, enum eTokenInvalidReasonCode
#define WTS_A_EXPIRE_USER_ALL_TOKENS			(WEBTOKENSERVER_DELTA + WTS_Q_EXPIRE_USER_ALL_TOKENS) // 42[s] reqId, errCode [errMsg]

//	PYR-52912 regulator -> web token server
#define WTS_Q_EXPIRE_USER_TOKEN_TYPE			1011 // 4s44 reqId, userId, tokenType, eTokenInvalidReasonCode
#define WTS_A_EXPIRE_USER_TOKEN_TYPE			(WEBTOKENSERVER_DELTA + WTS_Q_EXPIRE_USER_TOKEN_TYPE) // 42[s] reqId, errCode [errMsg]

//	PYR-52912
#define WTS_Q_ADMIN_REREAD_INIDATA				1012	//4 - reqId
#define WTS_A_ADMIN_REREAD_INIDATA				(WEBTOKENSERVER_DELTA + WTS_Q_ADMIN_REREAD_INIDATA)		// 2012		//	46[s] reqId, errCode[errDescr]

#define WTS_Q_VERIFY_PSIP_TOKEN					1013	// o - atf protocols\atfgenerated\WebTokenServer_atf.txt
#define WTS_A_VERIFY_PSIP_TOKEN					(WEBTOKENSERVER_DELTA + WTS_Q_VERIFY_PSIP_TOKEN)		// 2013		// o - atf protocols\atfgenerated\WebTokenServer_atf.txt

//	PYR-52518 cashier -> web token server
#define WTS_Q_GET_EXTERN_AUTH_INFO				1014	//	4ss - reqId, userId, token
#define WTS_A_GET_EXTERN_AUTH_INFO				(WEBTOKENSERVER_DELTA + WTS_Q_GET_EXTERN_AUTH_INFO)	//	2014	//	46[s|ss] - reqId, errCode[errMsg|operatorId,operatorUserId]

enum eWebTokenType
{
	eWebTokenTypeInvalid				= 0,
	eWebTokenTypeCashier				= 1,
	eWebTokenTypeFreemium				= 2,
	eWebTokenTypeCustomerSupport		= 3,
	eWebTokenTypeIce					= 4,
	eWebTokenTypeCasino					= 5,
    eWebTokenTypeSportsbook				= 6, // SBK-56
	eWebTokenTypeWebAccountService		= 7, // PYR-32891 (i.e., RAM token)
	eWebTokenTypeVipStore				= 8, // PYR-28871
	eWebTokenTypeRationalWebServices	= 9, // PYR-33416
	eWebTokenTypeDFSports				=10, // PYR-36465
	//eWebTokenTypePokerDuel				=11, // PYR-37209 // PYR-53789 - obsolete
	eWebTokenTypeFtpMigrate				=12, // PYR-40997
	eWebTokenTypeStorm					=13, // PYR-44178
	eWebTokenTypePoolBetting			=14, // PYR-42410
	eWebTokenTypeSilentLogin			=15, // PYR-52912
	eWebTokenTypePMCasino3rdParty		=16, // PYR-54541
	eWebTokenTypePSIP					=17, // PYR-54800
	eWebTokenTypePredictor				=18, // PYR-63110
	eWebTokenTypeExternalAuth			=19, // PYR-52518
	eWebTokenTypeExternalAuthRAM		=20, // PYR-65464
	eWebTokenTypeFacebook				=21, // PYR-123692
	// enum has to be consecutive, and always update eWebTokenLast for new tokentype

	eWebTokenTypeFirst					= eWebTokenTypeCashier,
	eWebTokenTypeLast					= eWebTokenTypeFacebook,


	// The following is reserved for client use only. Server will never use it.
	eWebTokenTypeClientOnlyInstantBonus	= 100,	// same as eWebTokenTypeRationalWebServices
	eWebTokenTypeClientOnlySportsbookExternal,	// same as eWebTokenTypeWebAccountService
	eWebTokenTypeClientOnlyVegas,				// same as eWebTokenTypeCasino

	eWebTokenTypeALL					= 255, // PYR-32121 (used to indicate expected token type can be any) 

/*
	For server: steps to add a new web token type:
	1. append a new web token type to eWebTokenType
	2. create a new server connection factory that takes the new token type as a parameter if needed
	3. check if emailVerified is necessary before replying a get-token request
	4. update getWebTokenTypeName() below
	note: use eWebTokenTypeIce as an example
*/
};

//	PYR-77257
enum eTokenInvalidReasonCode
{
	eTokenInvalidReasonCode_expire				= 1,
	eTokenInvalidReasonCode_adminForceLogout	= 2,
	eTokenInvalidReasonCode_sessionTransferred  = 3,
	eTokenInvalidReasonCode_playingTimeReached  = 4,
	eTokenInvalidReasonCode_explicitLogout		= 5,
	eTokenInvalidReasonCode_appSessionDeleted   = 6,
	eTokenInvalidReasonCode_pwdChange			= 7,
	eTokenInvalidReasonCode_clientReconnect		= 8,		//	AQA-17023
};

class WebTokenRules
{
public:
	static const char* getTokenTypeName(UINT32 tokenType)
	{
		switch (tokenType)
		{
			case eWebTokenTypeInvalid:				return "Invalid";
			case eWebTokenTypeCashier:				return "Cashier";
			case eWebTokenTypeFreemium:				return "Freemium";
			case eWebTokenTypeCustomerSupport:		return "CustomerSupport";
			case eWebTokenTypeIce:					return "Ice";
			case eWebTokenTypeCasino:				return "Casino";
			case eWebTokenTypeSportsbook:			return "Sportsbook";
			case eWebTokenTypeWebAccountService:	return "RAM";
			case eWebTokenTypeVipStore:				return "VipStore";
			case eWebTokenTypeRationalWebServices:	return "RWS";
			case eWebTokenTypeDFSports:				return "DFSports";
			case eWebTokenTypeFtpMigrate:			return "FtpMigrate";
			case eWebTokenTypeStorm:				return "Storm";
			case eWebTokenTypePoolBetting:			return "PoolBetting";
			case eWebTokenTypeSilentLogin:			return "SilentLogin";
			case eWebTokenTypePSIP:					return "PSIP";
			case eWebTokenTypePredictor:			return "Predictor";
			case eWebTokenTypeExternalAuth:			return "ExternalAuth";
			case eWebTokenTypeExternalAuthRAM:		return "ExternalAuthRAM";
			case eWebTokenTypeFacebook:				return "Facebook";
			default:								return "Unknown";
		}
	}
};

// PYR-32891
#define REGULATOR_CONN_TYPE				"regulator"
#define CASHIER_CONN_TYPE				"cashier"
#define WEB_ACCOUNT_SERVICE_CONN_TYPE	"webaccountservice"
#define WEB_FACEBOOK_CLI_CONN_TYPE		"facebook"

#define WEB_ACCOUNT_SERVICE_OBJ_ID 1
#define VIPSTORE_CONN_TYPE			  "vipstore" 
#define TABLE_CONN_TYPE				  "table"
#define LOBBY_CONN_TYPE				  "lobby"
#define BLITZLOBBY_CONN_TYPE		  "blitzlobby"
#define AUXHUB_WTS_CONN_TYPE			"auxhub"

#endif // WebTokenServer_h_included
