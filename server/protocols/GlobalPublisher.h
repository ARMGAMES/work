// GlobalPublisher.h
#ifndef GlobalPublisher_h_included
#define GlobalPublisher_h_included

#include "commonmacros.h"

// PYR-56386: These are the servers that GlobalPublisher publishes connection details for
enum eGlobalPublisherServerType
{
	eGlobalPublisherServerType_Generic				= -1,
	eGlobalPublisherServerType_Cashier				= 0,
	eGlobalPublisherServerType_ThemeUpdate			= 1,
	eGlobalPublisherServerType_VipStore				= 2,
	eGlobalPublisherServerType_HomeGames			= 3,
	eGlobalPublisherServerType_Regulator			= 4,
	eGlobalPublisherServerType_UIStats				= 5,
	eGlobalPublisherServerType_WebTokenServer		= 6,
	eGlobalPublisherServerType_TemplateCache		= 7,
	eGlobalPublisherServerType_LobbyEtc				= 8,
	eGlobalPublisherServerType_ChipsDbm				= 9,
	eGlobalPublisherServerType_CasinoLobby			= 10,
	eGlobalPublisherServerType_MGLobbyDeal			= 11,
	eGlobalPublisherServerType_MGLobbyStarsRewards	= 12,
	eGlobalPublisherServerType_News					= 13,
	eGlobalPublisherServerType_Auth					= 14,
	eGlobalPublisherServerType_MLobby				= 15,
	eGlobalPublisherServerType_SagStarter			= 16,
	eGlobalPublisherServerType_SngStarter			= 17,
	eGlobalPublisherServerType_StormLobby			= 18,
	eGlobalPublisherServerType_Lobby				= 19,
	eGlobalPublisherServerType_MLobbyNG				= 20,
	eGlobalPublisherServerType_Personalization		= 21,
	eGlobalPublisherServerType_InstantHandReplayRetriever = 22,
	eGlobalPublisherServerType_ProgressivePlusStarter = 23,

	ENUM_LAST_VALUE(eGlobalPublisherServerType) // defines eGlobalPublisherServerType_Last & eGlobalPublisherServerType_NextAfterLast
};

enum eGlobalPublisherAvailabilityStatus
{
	eGlobalPublisherAvailabilityStatus_Unknown		= -1,	// status has never been set
	eGlobalPublisherAvailabilityStatus_Unavailable	= 0,	// server is unavailable because it has died or is restarting
	eGlobalPublisherAvailabilityStatus_Available	= 1,	// server is available for connections
};

// PYR-52524
#define MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA				1000	// 4 - reqId
#define MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY			1001	// 46[s] - reqId, errCode[errDescr]

// PYR-52524
#define MSG_GLOBALPUBLISHER_SET_PUBL_PROPERTIES					1002	// 44s - reqId, siteId, propertiesString
#define MSG_GLOBALPUBLISHER_SET_PUBL_PROPERTIES_REPLY			1003	// 46[s] - reqId, errCode[errDescr]

// PYR-52524
#define MSG_GLOBALPUBLISHER_DELETE_PUBL_PROPERTY				1004	// 44s - reqId, siteId, propertyName
#define MSG_GLOBALPUBLISHER_DELETE_PUBL_PROPERTY_REPLY			1005	// 46[s] - reqId, errCode[errDescr]

// PYR-52524
#define MSG_GLOBALPUBLISHER_LOBBY_UPDATE_DATA 					1006	// 44 - reqId, dataMiningOpt
#define MSG_GLOBALPUBLISHER_LOBBY_UPDATE_DATA_REPLY				1007	// 46[s] - reqId, errCode[errDescr]

// PYR-52524
#define MSG_GLOBALPUBLISHER_INI_CHANGED_NOTIFICATION  			1008	// 44 - reqId
#define MSG_GLOBALPUBLISHER_INI_CHANGED_NOTIFICATION_REPLY 		1009	// 46[s] - reqId, errCode[errDescr]

// PYR-52524
#define MSG_GLOBALPUBLISHER_MONET_DATA				  			1010	// 444<ss> - reqId,type,numMonetData<name,value>
#define MSG_GLOBALPUBLISHER_MONET_DATA_REPLY			 		1011	// 46[s] - reqId, errCode[errDescr]

// PYR-56386
#define MSG_GLOBALPUBLISHER_SET_SERVER_AVAILABILITY		  		1012	// 444 - reqId, status
#define MSG_GLOBALPUBLISHER_SET_SERVER_AVAILABILITY_REPLY		1013	// 46[s] - reqId, errCode[errDescr]

#endif //GlobalPublisher_h_included
