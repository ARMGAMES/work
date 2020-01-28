//lobby.h

#ifndef lobby_h_included
#define lobby_h_included

#include "pplib.h"
#include "sites.h"
#include "clientplatforms.h"
#include "licenseid.h"
#include "visibility.h"
#include "commonmacros.h"

// Note that versions must not be incremented for May 2 '12 build!
#define	LOBBY_VERSION "1.75"
#define	LOBBY_VERSION_COMPATIBLE "1.76"

#define	MAX_SECONDS_SAFE	1000000
#define	TOURNAMENT_CHIPS_QUANTUM_DENOMINATED	1

//constants for broadcast flags - bitmask
#define BROADCAST_PLAY_MONEY			0x1
#define BROADCAST_REAL_MONEY			0x2
#define BROADCAST_ALL					(BROADCAST_PLAY_MONEY | BROADCAST_REAL_MONEY)
#define BROADCAST_TABLE_ID				0x4
#define BROADCAST_TABLE_ID_JUBILTYPE	0x8
#define BROADCAST_SITEID_INCLUSIVE		0x10 // [AO 2009-02-05] bug #10387

#define TABLE_PUBLICATION_NAME			"tableData64"
#define TOURN_PUBLICATION_NAME			"tournamentpublisher.2004"
#define TOURN_EXTRA_PUBLICATION_NAME	"extrainfo"
#define TOURN_I18N_PUBLICATION_NAME		"localData"

enum GlobalEventType
{
	glEvMassDisconnectStarted = 1,
	glEvMassDisconnectEnded,
};

enum UserUpdateMask
{
	imageUpdated		= 0x01,
	privilegesUpdated	= 0x02,
};

enum ClientExtraInfoMask
{
	cliExtraInfoExplicitLogin		= 1 << 0, // 1
	cliExtraInfoCashierOnly			= 1 << 1, // 2  // PYR-23163: 0 - login only if there are no other connections in Cashier, 1 - close existing connections and login
	cliExtraInfoNotExtendTokenTime	= 1 << 2, // 4  // PYR-35378: only used by webtokensever (set by web client)
	cliExtraInfoFingerprintLogin	= 1 << 3, // 8  // PYR-33139
	cliExtraInfoDevicePasscodeLogin = 1 << 4, // 16 // PYR-33139
	cliExtraInfoReadyForPoolBlock	= 1 << 5, // 32 // PYR-42791
	//cliExtraInfoReadyForSimpleAutoTournDeals  = 1 << 6 // 64 // !!! This flag is set by 2017.MAIN.01 desktop and mobile clients which were not ready for simple auto tourn deals. Flag can be reclaimed and reused once these clients are extinct in the wild.
	cliExtraInfoReadyForSimpleAutoTournDeals  = 1 << 7, // 128 // PYR-47376: client supports Simplified Automated Tournament Deals
	cliExtraInfoSupportsAdminMsgViaLobbyEtc	  = 1 << 8,	//	256	//	PYR-50915 Client has support for MSG_LOBBY_ADMIN_INFO message on the LobbyEtc connection.
	cliExtraInfoSupportsPresentCrmOffer		  = 1 << 9, // 512 // PYR-52641 - Client supports present of CRM Offer
	cliExtraInfoSilentLogin					  = 1 << 10, // 1024 //PYR-52912 Client is logging in silently, I.e. user did not explicitly enter username/password.
	cliExtraInfoSupportsSimulLogin			  = 1 << 11, // 2048 // PYR-52886 Client supports Simultaneous Login
	cliExtraInfoTransferAPCLogin			  = 1 << 12, // 4096 // PYR-52886 Client wants to login & transfer the Authenticated Poker Connection from another session.
	cliExtraInfoSupportsFlagsPrivUpdate		  = 1 << 13, // 8192 // PYR-52022 - Client supports flags and privileges update notification
	cliExtraInfoReadyForNsfDialog			  = 1 << 14, // 16384 // PYR-55137 - Client supports new NSF dialogs
	cliExtraInfoSupportsNotificationViaLobbyEtc = 1 << 15, // 32768 // PYR-68327 - Client supports notification messages on the LobbyEtc connection
	cliExtraInfoRamMidTier                      = 1 << 16, // 65536 // PYR-71570 - Client is really RAM WMT (not just enum AppTypeId_RAM)
	cliExtraInfoSupportsTempPinErr				= 1 << 17, // 131072 // PYR-63577 - Client supports Temp Pin login error code 
	cliExtraInfoPlcInstalled					= 1 << 18, // PYR-77521: Client has PLC plugin installed
	cliExtraInfoUnsolicitedNotification			= 1 << 19, // PYR-100911 generic unsolicited client notification support
	cliExtraInfoSuppresLobbyBlkListNotif		= 1 << 20, // PYR-102822 for PT client - Lobby/LobbyEtc client notifications are suppressed

	// !!! Warning - cliExtraInfoMask_Last should be updated with new additions

	cliExtraInfoMask_Last			= cliExtraInfoSuppresLobbyBlkListNotif, // must be kept up to date
};

enum eChatModeratorRestrictions
{
	eChatModeratorUnrestricted			= 0,
	eChatModeratorRealMoneyRestricted	= 1,
	eChatModeratorPlayMoneyRestricted	= 2,
};

//PYR-26438
enum eAllowUnregFromTarget
{
	eNotDefined			= 0,
	eAllowToUnreg		= 1,
	eDoNotAllowToUnreg	= 2
};

// PYR-25226
enum BlitzLimboActions
{
	bzNoneAction = 0,
	bzRebuyAction = 1,
	bzReloadStackAction = 2,
	bzChooseInitStacksAction = 3,
};

//these codes are used to exactly identify a place in code where internal error 
// occurred /was received
///codes < 1000 are used by cashier only
#define ERR_CLIENT_ALREADY_CONNECTED    868
#define ERR_INTERNAL_LOGIC_FAILED       869
#define ERR_AGECHECK_SAVED              870
#define ERR_CASHIER_REDIRECT_INIT       871
#define ERR_CASHIER_USER_ACCT_REGISTR   872
#define ERR_CASHIER_RECEIVE_USERAUTH	873
#define ERR_SUBMIT_FPP_PURCHASE			874
#define ERR_INVALID_DATA_FROM_CLIENT	875
#define ERR_RECV_RESEARCH_REPLY			876
#define ERR_RECV_PRECASHIN_DBM_REPLY	879
#define ERR_RECV_BONUSHIST_DBM_REPLY	880
#define ERR_WIRE_RECV_DBM_REPLY			881
#define ERR_MISCONFIGURATION			882
#define ERR_RECV_MRMC_INITSIGNUP_REPLY	883
#define ERR_OBSOLETE_FUNCTIONALITY		884
//#define ERR_RECV_MONEYGRAMINFO_REPLY	885
#define ERR_HOSTED_DONE					886
#define ERR_GCDEBIT_NO_GATEWAY    		887
#define ERR_RECV_CASOUT_INIT_REPLY		888
#define ERR_PARSE_CASHIN_FROM_CLIENT	889
#define ERR_ENVOY_MISCONFIGURATION		890
#define ERR_CASHIER_CREATE_REDEEM_ACCT	891
#define ERR_CASHIER_COMPLETE_BAD_TRAN	892
#define ERR_OLD_AAMS_LICENSE_VERSION    908
#define ERR_RECV_NO_GW_CASHTRANS_REPLY	997
#define ERR_RECV_BAD_CASHTRANS_REPLY	998
#define ERR_RECV_CASHIN_INIT_REPLY		999

#define ERR_REG_USER_INVALID_INPUT		1000	//invalid parameter in requestRegisterNewUser() 
#define ERR_LOBBY_RECEIVE_USERAUTH		1001
#define ERR_RECEIVE_CHIPS_FULL_INFO		1002
#define ERR_RECV_HAND_HISTORY			1003
#define ERR_REQ_FULL_USER_INFO			1004
#define ERR_RECV_FULL_USER_INFO			1005
#define ERR_REQ_USER_UPDATE				1006
#define ERR_RECV_USER_UPDATE			1007
#define ERR_ADD_WAITING_LIST			1008
#define ERR_RECV_ACTIV_ACCOUNT			1009
#define ERR_RECV_USER_STATS				1010
#define ERR_RECV_TOURN_STATS			1011
#define ERR_REG_TOURN_USER_NZP			1012
#define ERR_RECV_USER_TOURN_REG			1013
#define ERR_REQ_USER_TOURN_UNREG		1014
#define ERR_RECV_USER_TOURN_UNREG		1015
#define ERR_WHERE_IS_NOT_STARTED		1016
#define ERR_WHERE_IS_NOT_FOUND			1017
#define ERR_WHERE_IS_UNKNOWN_STATUS		1018
#define ERR_TOURN_USER_STATS			1019
#define ERR_RECV_REG_NEW_USER			1020
#define ERR_MTLOBBY_RECEIVE_USERAUTH	1021
#define ERR_LOBBY_RECEIVE_USER_TLB		1022
#define ERR_RECV_PLAY_TRANSFERS_OBSOLETE	1024
#define ERR_REG_USER_INVALID_PROMO		1023
#define ERR_RECV_RESET_PASSWORD			1025
#define ERR_RECV_REAL_TRANSFER			1026
#define ERR_REQ_MONEY_TRANSFER_OBSOLETE	1027
#define ERR_RECV_DBM_SUPP_TRANSFER		1028
#define ERR_REBUYS_EXHAUSTED			1029
#define ERR_ADDONS_EXHAUSTED			1030
#define ERR_CHIPS_ABOVE_REBUY_LIMIT		1031
#define ERR_CHIPS_ABOVE_ADDON_LIMIT		1032
#define ERR_REBUY_CLOSED				1033
#define ERR_ADDON_CLOSED				1034
#define ERR_ASYNC_REBUY_DISABLED		1035
#define ERR_CAPTCHA_FAILED				1036
#define ERR_FUNDS_REG_TOURN				1037
#define ERR_RESET_PASSWORD_CONNECTED	1038
#define ERR_VIPSTORE_RECEIVE_USERAUTH	1039
#define ERR_REG_USER_IMAGE				1040
#define ERR_RECEIVE_CHIPS_FULL_INFO2	1041
#define ERR_FUNDS_REG_TOURN_NOT_ALLOWED 1042
#define ERR_SET_PROMO_IMAGE				1043
#define ERR_GET_PROMO_IMAGE				1044
#define ERR_SET_BUYIN_LIMIT				1045
#define ERR_GET_BUYIN_LIMIT				1046
#define ERR_SET_PLAYING_TIME_LIMIT		1047
#define ERR_GET_PLAYING_TIME_LIMIT		1048
#define ERR_SET_CLIENT_OPTIONS			1049
#define ERR_GET_CLIENT_OPTIONS			1050
// PYR-14242
#define ERR_SEND_VALIDATION_SMS         1051
#define ERR_RECV_VALIDATION_CODE        1052
// #18708
#define ERR_BE_RM_ACCOUNT_OPEN        	1053
// PYR-18597
#define ERR_SOCIAL_SHARING_DISABLED    	1054
#define ERR_OVER_HOURLY_SHARING_LIMIT	1055
#define ERR_OVER_DAILY_SHARING_LIMIT	1056
#define ERR_COULD_NOT_SHARE_HAND		1057
#define ERR_BLITZLOBBY_RECEIVE_USERAUTH		1058
#define ERR_BLITZLOBBY_RECEIVE_USERBUYIN	1059
#define ERR_BLITZLOBBY_RECEIVE_USERADDON	1060
#define ERR_BLITZLOBBY_RECEIVE_USERINFO		1061
#define ERR_RECV_USER_TOURN_REG_INFO		1062 //PYR-20530
#define ERR_BLITZLOBBY_RECEIVE_USERCHECK	1063
#define ERR_REGULATOR_RECEIVE_USERAUTH		1064
#define ERR_ADMINAUTH_RECEIVE_USERAUTH	1068 // #22082
#define ERR_TICKET_TOURNS				1072 //#23055
#define ERR_CHIPSDBM_RECEIVE_USERAUTH	1073 //#26426
#define ERR_ADYENHOSTED_MISCONFIGURATION	1074 //#36700
#define ERR_MTLOBBY_RECEIVE_DRC_SATELLITECHECK 1075 //#38597
#define ERR_CASHIER_NEEDS_DOC_INFO_FOR_PROFEE 1076 //#47450
#define ERR_RECEIVE_USER_LOGINS			1077 // PYR-52691

// [AO 2009-05-20] VipStore error locations
#define ERR_VIPSTORE_DUPLICATE_MESSAGE	2000	// Message already processed in recovery
#define ERR_VIPSTORE_CHECK_COUPON		2001	// Error in CRM_Q_CHECK_COUPON
#define ERR_VIPSTORE_GET_ITEM			2002	// Error in CRM_Q_GET_FPP_STORE_ITEM
#define ERR_VIPSTORE_CHECK_DEAL			2003	// Error in DBM_Q_VIPSTORE_CHECK_DEAL
#define ERR_VIPSTORE_ITEM_OBSOLETE		2004
#define ERR_VIPSTORE_HISTORY_ERROR		2005	// Error in CRM_Q_ORDER_HISTORY
#define ERR_VIPSTORE_CANCEL_ORDER		2006
#define ERR_VIPSTORE_CANCEL_ORDER2		2007
#define ERR_VIPSTORE_CANCEL_ORDER3		2008

//[AN 2009-11-16] PYR-7801 VIP Store Inventory
#define ERR_VIPSTORE_INVENTORY_ERROR	2009	// error retrieving inventory for the item
#define ERR_VIPSTORE_INVENTORY_NOT_USED	2010	// inventory for this item is not used
#define ERR_VIPSTORE_NOT_REGULAR_ITEM   2011	// inventory is not kept for this type of item

#define ERR_VIPSTORE_BUY_WITH_COUPON	2012	// Error in CRM_Q_BUY_WITH_COUPON
#define ERR_VIPSTORE_MIGRATED_USER		2013	// Error in DBM_Q_GET_MIGRATED_USER_INFO
#define ERR_VIPSTORE_GET_ALT_ADDRESSES	2014	// Error in CRM_Q_GET_USER_ALT_ADDRESSES - PYR-15672: VipStore alternate delivery address
#define ERR_VIPSTORE_UPDATE_ALT_ADDRESS 2015	// Error in CRM_Q_UPDATE_USER_ALT_ADDRESS- PYR-15672: VipStore alternate delivery address

#define ERR_VIPSTORE_NAME_TOO_LONG      2016    // PYR-15806 custom name specified by user is more than maximum allowed
#define ERR_CHECK_NEW_USER_ARCHIVED		2017    // #20399

#define ERR_AUTH_DK_RECEIVE_USERAUTH	2018 // #16706
#define ERR_AUTH_WEB_TOKEN_SERVER_USERAUTH 2019  // PYR-22271 
#define ERR_UISTATS_RECEIVE_USERAUTH	2020 // PYR-21206

#define ERR_VALIDATE_NEW_INSTALL		2021	// #21829
#define ERR_NO_TGM_FOUND				2022	// #24499
#define ERR_ALREADY_KICKED				2023	// #24099
#define ERR_ALREADY_REQUESTED			2024	// #24099
#define ERR_ALREADY_SEATED				2025	// #24099
#define ERR_GET_BUYIN_LIMIT_EX			2026	//PYR-27249
#define ERR_SET_BUYIN_LIMIT_EX			2027	//PYR-27249
#define ERR_NSTACK_TOURN_BLITZ          2028   // PYR-25226

#define ERR_SAGSTARTER_RECEIVE_USERAUTH		2029

#define ERR_ICECLIENTPROXY_RECEIVE_USERAUTH 2030 // PYR-29927
#define ERR_RESOLVER_ERROR					2031 // PYR-30606
#define ERR_NOT_ICE_TOURNAMENT				2032 // PYR-32125
#define ERR_ALL_AAMS_FLIGHTS_SLOTS_USED		2033 // PYR-26095
#define ERR_AAMS_FLIGHTS_INDEX_MISSING		2034 // PYR-26095
#define ERR_AAMS_SESSION_ALREADY_STARTED	2035 // PYR-26095
#define ERR_NOT_FLIGHT_TOURN				2036 // PYR-26095
#define ERR_CHANGE_FLIGHT_TOURN				2037 // PYR-26095
#define ERR_WRONG_TARGET					2038 // PYR-26095
#define ERR_WRONG_FLIGHT_LEVEL				2039 // PYR-26095
#define ERR_FLIGHT_NO_LONGER_AVAILABLE		2040 // PYR-26095
#define ERR_FLIGHT_IS_FULL					2041 // PYR-26095
// range 2042-2044 - available
#define ERR_MGLOBBY_RECEIVE_USERAUTH 		2045 //	PYR-40426

#define ERR_BRAND_NOT_ALLOWED				2046 // PYR-40189
#define ERR_SEATFINDER_CHECK_PRIVS			2047 // PYR-43054
#define ERR_SEATFINDER_JOIN					2048 // PYR-43054
#define ERR_SEATFINDER_UNJOIN				2049 // PYR-43054
#define ERR_SEATFINDER_CHECK_CHIPS			2050 // PYR-43054
#define ERR_SEATFINDER_RECEIVE_CHIPS		2051 // PYR-43054

#define ERR_CRM_RECEIVE_USERAUTH 			2052 //	PYR-49906
#define ERR_GUARANTEED_PRIZE_POOL_RESTRICT	2053 // PYR-53162

#define ERR_PS_RECEIVE_USERAUTH 			2054 //	PYR-66899

#define ERR_AB_RECEIVE_USERAUTH				2055 // PYR-80333

#define ERR_SOC_RECEIVE_USERAUTH			2056 // PYR-77853

#define ERR_HUBFEED_RECEIVE_USERAUTH		2057 // PYR-124676

void formatInternalError( INT16 commonErrorCode, int errorLocationMark, PString& ret);



enum TableMiscFlags
{
	// not published at the table:
	LastTournTable					= 0x0001, 
	noDuplicateIpDetection			= 0x0002,
	RecordingTable					= 0x0004, 
	RecordingTableWithOpenCards		= 0x0008, 
	TABLESCRIPT_FLAG_HIGHROLLER		= 0x0010,
	ImagesSwitchOff					= 0x0020,
	TABLESCRIPT_FLAG_NOCHATOBSERV	= 0x0040,
	ObserverChatDisableAtTheEnd		= 0x0080,
	BlockChatAllIn					= 0x0100,
	BlockChatInHand					= 0x0200,
	TABLESCRIPT_FLAG_SUPERHIGHROLLER= 0x0400,
	TABLESCRIPT_FLAG_BLINDSREVERSE	= 0x0800,
	AllowDoubleRebuy				= 0x1000,
	TABLESCRIPT_FLAG_EXPERIMENTAL	= 0x2000,
	TABLESCRIPT_FLAG_AUTOREBUYON	= 0x4000,
	TABLESCRIPT_FLAG_AAMS			= 0x8000,
	//MaxAllowed					= 0x8000!! no more

	LobbyFlagsMask = 0x0000FFFF, 

	//--------------------------------------

	// published at the table:
	TABLESCRIPT_FLAG_TIMEBANK			=	  0x010000,
	TABLESCRIPT_FLAG_NOALLIN			=	  0x020000,
	NotSaveHandsToDbm					=	  0x040000,
	TABLESCRIPT_FLAG_FASTTABLE			=	  0x080000,
	TABLESCRIPT_FLAG_CHATMODERATOR		=	  0x100000,
	TABLESCRIPT_FLAG_MULTIPLIERx2		=	  0x200000,
	TABLESCRIPT_FLAG_MULTIPLIERx3		=	  0x400000,
	TABLESCRIPT_FLAG_COMPLETENAME		=	  0x800000,
	TABLESCRIPT_FLAG_RESERVED			=	 0x1000000, // PYR-84247 formerly TABLESCRIPT_FLAG_ASKTOPOSTBLINDS, all Production TABTYPES that had it set are obsolete so OK to reuse
	RingGameReplayTable                 =    0x2000000,
	TABLESCRIPT_FLAG_NORATHOLE          =    0x4000000,
	TABLESCRIPT_FLAG_SHOW_ONE_CARD      =    0x8000000,	// #15261
	TABLESCRIPT_FLAG_BLITZ				=   0x10000000, // PYR-19752
	TABLESCRIPT_FLAG_SPLITTIMEBANK		=   0x20000000, // PYR-54102
	TABLESCRIPT_FLAG_RUN_IT_TWICE       =   0x40000000, // PYR-16658
	//InstantHandReplayV7Supported        =   0x80000000, // PYR-72484  // PYR-95129 2019.Main.04 retired this bit (it could be deleted after a couple of builds) -- this bit was only used in table publication!
	//MaxAllowed						=	0x80000000,

	ScriptFlagsMask = 0xFFFF0000
};

//#define TABLESCRIPT_FLAG2_			(ONE64 << 0) #19918
//TableMiscFlags2:
//#define TABLESCRIPT_FLAG2_STARTER							       (ONE64 << 0)
//#define TABLESCRIPT_FLAG2_STARTER_HIDDEN	                       (ONE64 << 1)
//#define TABLESCRIPT_FLAG2_STARTER_PUBLIC	                       (ONE64 << 2)
#define ChatMonitorFinalTableYes			                       (ONE64 << 3)
#define ChatMonitorFinalTableNo				                       (ONE64 << 4)
#define TABLESCRIPT_FLAG2_NO_RATHOLE_OBLIG	                       (ONE64 << 7)	//PYR-25814
#define TABLESCRIPT_FLAG2_MAX_ORBITS_SITOUT_CHECK_NOT_FULL_ALLOWED (ONE64 << 8)	// PYR-25229
#define TABLESCRIPT_FLAG2_HTML5_ALLOWED		                       (ONE64 << 9)	//PYR-33228
#define TABLESCRIPT_FLAG2_WINTHEBUTTON_MODE                        (ONE64 << 10)	//PYR-33587
#define TABLESCRIPT_FLAG2_PROGRESSIVE_KNOCKOUT                     (ONE64 << 11) //PYR-45395
#define TABLESCRIPT_FLAG2_FLOPSTARS                                (ONE64 << 12) // PYR-56338
//#define TABLESCRIPT_FLAG2_SERVER7                                  (ONE64 << 13) // PYR-59959  // retired by PYR-75653 in 2018.Main.07
#define TABLESCRIPT_FLAG2_FORCE_WINNER_TO_SHOW                     (ONE64 << 14) // PYR-64093 showtime poker
//#define TABLESCRIPT_FLAG2_USE_OLD_TABLE_SERVER					   (ONE64 << 15) // PYR-75653 this flag introduced in 2018.Main.07, retired in 2018.Main.08
#define TABLESCRIPT_FLAG2_FUSION_DEAL_TWO_ON_FLOP                  (ONE64 << 16) // PYR-71602 - when set, use modified fusion rule where additional two hole cards are all dealt with the flop
#define TABLESCRIPT_FLAG2_BUTTON_BLIND                             (ONE64 << 17) // PYR-74693 
#define TABLESCRIPT_FLAG2_ALLIN_CASHOUT_ALLOWED                    (ONE64 << 18) // PYR-95110
#define TABLESCRIPT_FLAG2_STEALTH_TABLE                            (ONE64 << 19) // PYR-106138
#define TABLESCRIPT_FLAG2_SAG_FLASH		                           (ONE64 << 20) // PYR-111934
//MaxAllowed								                       (ONE64 << 63)

enum RebuyAddonManagement
{
	 RebuyEnabled = 0x1, AddonEnabled = 0x2
};

enum TableStopDealingState
{
	NormalRunning = 0x0, Stopping, Stopped
};

enum TableServiceFlags
{ //UINT32 'tableServFlags' script parameter
	TABLESERV_FLAG_QUICKSTART = 0x0001,	//PYR-20432
	TABLESERV_FLAG_SHOWSUBSCRIBERS = 0x0002, //PYR-22393
	TABLESERV_FLAG_SHOWSUBSCRIBERS_ADMIN = 0x0004 //PYR-22393
	//MaxAllowed = 0x80000000
};

enum TableExternalPropFlags
{ //bitMask! Not a enumeration
	TABLE_EXT_FLAG_MODERATORCOMPLAINOFF	= 0x0001,
	TABLE_EXT_FLAG_BOUNTY				= 0x0002,
	//TABLE_EXT_FLAG_STARTER			= 0x0004,
	TABLE_EXT_FLAG_SEAT_FINDER			= 0x0008,
	TABLE_EXT_FLAG_SHOOTOUT_ALLIN_TOURN	= 0x0010,  // PYR-74796
};

enum TableAdminMessageFlags
{
	NoPopUp	=	0x1,
	AdminMess =	0x2,
	OptionalM =	0x4,
	ShowAdminId	= 0x8, // #16408
	SuperStarShowdownResults = 0x10,  // #18531
	SaveMessageInHistoryPro  = 0x20,   // PYR-19266
	FilterAutoBroadcasts  = 0x40,   // PYR-23744
	UserSpecific  = 0x80,   // PYR-23129
	SagTopJackpot = 0x100,	// PYR-33657
	SportsBookCrossSell = 0x200,	// PYR-44486
	SnGTourn = 0x400,	// PYR-34908
};

// [AO 2010-05-20] enum Sites moved to sites.h

enum eMigrationType
{
	eMigrationTypeTransfer = 0,		// to linked user with different userId
	eMigrationTypeConversion = 1,	// same userId

	// !!! Warning - eMigrationTypeLast should be updated with new additions
	eMigrationTypeLast = eMigrationTypeConversion,
};

enum eTablePausingStatus
{
	eTableNoPausing = 0,
	eTablePausingPending,
	eTablePausingCountingHands,
	eTablePaused,
};

#define HAND_BIT_FIRST_HAND				 0x01
#define HAND_BIT_LAST_TABLE				 0x02
#define HAND_BIT_LAST_TWO_PLAYERS		 0x04
#define HAND_BIT_WAITING_BREAK			 0x08
#define HAND_BIT_WAITING_BUBBLE			 0x10
#define HAND_BIT_OBSERVER_CHAT_OFF		 0x20
#define HAND_BIT_DISCON_ADD_EXTRA_TIME	 0x40
#define HAND_BIT_OBSERVER_CHAT_STOP		 0x80
#define HAND_BIT_PLAYER_CHAT_STOP		0x100
#define HAND_BIT_RESTRICTED_CHAT		0x200 // Bug#: 2130 temporary frozen
#define HAND_BIT_BUBBLE_OBSERV_CHAT_OFF	0x400
#define HAND_BIT_BUBBLE_PLAYER_CHAT_OFF	0x800
#define HAND_BIT_LAST_TABLE_SPECIAL_5MAX 0x1000	// #19789
#define HAND_BIT_FIRST_HAND_AFTER_SWITCH 0x2000	// #22885
#define HAND_BIT_FORCE_RELOAD_ALL_STACKS 0x4000 // #25226
#define HAND_BIT_TOURN_BREAK_SKIPPED		0x8000 // #29810

#define DEFAULT_CONNECTION_QUALITY	100
#define CONNECTION_QUALITY_DISCONNECTED (-1)


#define NON_TOURNAMENT				0x0
#define DENOMINATED_MT_TOURNAMENT	0x1
#define MT_TOURNAMENT				0x2
#define BLITZ_TOURNAMENT			0x4
#define SAG_TOURNAMENT				0x5


#define MAX_TABLE_PLAYERS	10
//#define MAX_PLAYERS_PER_TABLE 9

#define WAITING_LIST_DELAY_SEC	60

// incoming messages handled by LobbyServerConn::processMessage

#define MSG_LOBBY_HAND					1 //441<s1441>1 = tableId, pot, num_players, <name, pos, chipsStart, chips, connQuality> handCancelled, numPlayers<statFlags, codeByte>
#define MSG_LOBBY_HAND_REPLY			2 //41<s1<s44>> - handFlag,numBounties,<bountyLoser,numWinners,<bountyWinner,amount,bountyToHead>>

#define	MSG_LOBBY_PLAYER_SIT			3 //44s14s4, reqId, tableId, playerId, position, chips, city, unique(if any, or 0)
#define	MSG_LOBBY_PLAYER_SIT_REPLY		4 //4 reqId means OK

#define	MSG_LOBBY_PLAYER_LEFT			5 //44s4, reqId, tableId, playerId, unique(if any, or 0)
#define	MSG_LOBBY_PLAYER_LEFT_REPLY		6 // see REPLY_TABLE_* constants

#define MSG_LOBBY_TABLE_DEAD			7 //4 tableId, no answer needed

//PYR-21902
#define MSG_CLI_LOBBY_FIND_QUICKSTART_SNG2			8
#define MSG_CLI_LOBBY_FIND_QUICKSTART_SNG2_REPLY	9

#define MSG_LOBBY_USER_AUTH					10//
#define MSG_LOBBY_USER_AUTH_REPLY			11//

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_CREATE_TOURNAMENT			12// 47777744<s>ss typeId, year, month, day, hour, minute, reg_pause, numPreregistered<userName>, effCreator, scriptName
#define MSG_LOBBY_CREATE_TOURNAMENT_REPLY	13// 6[s|44 4(s4s)ss] error_code[errDescr | tournId, timeLeft, numErrUsrs(UserId, error_code, errDescr), warning, restartWarning]

#define MSG_CLI_LOBBY_FIND_QUICKSTART_SNG3			14
#define MSG_CLI_LOBBY_FIND_QUICKSTART_SNG3_REPLY	15

#define MSG_LOBBY_MTLOBBY_DEAD				16// 4 tournid
#define MSG_LOBBY_MTLOBBY_DEAD_REPLY		17// 4b tournid, changeStatusToCanceled
/*
#define MSG_LOBBY_LIST_TOURNAMENTS			18//empty
#define MSG_LOBBY_LIST_TOURNAMENTS_REPLY	19//6[s|4<??>]

#define MSG_LOBBY_LIST_TOURN_TYPES			18//empty
#define MSG_LOBBY_LIST_TOURN_TYPES_REPLY	19//6[s|4<??>]
*/
#define MSG_LOBBY_CREATE_TOURN_TYPE			20// ???
#define MSG_LOBBY_CREATE_TOURN_TYPE_REPLY	21// 6[s]	error_code[errDescr]

#define MSG_LOBBY_TIMER_UPDATE_TIME			22//empty
#define MSG_LOBBY_TIMER_LOGGER				23//empty

#define MSG_LOBBY_TIMER_CALC_DISCONN		24 //empty
#define MSG_LOBBY_TIMER_UPDATE_SNG			25 //empty

/*
requestType					requestContents
------------------------------------------------------
HAND_HISTORY_LAST_HANDS		number of last hands
HAND_HISTORY_LAST_SECONDS	number of last seconds 
HAND_HISTORY_HAND_IDS		number of hands requested, followed by list of hands<4>
*/
#define MSG_LOBBY_HISTORY_REQUEST			23	//s14[<4>]	userId, requestType, requestContents[<handId>] 
#define MSG_LOBBY_HISTORY_REQUEST_REPLY		24	//6s	error_code[errDescr|userEmail]
/*
#define MSG_LOBBY_SAVE_IMAGE				25  //sB userId, block
#define MSG_LOBBY_SAVE_IMAGE_REPLY			26  //6[s]	error_code[errDescr]
*/
#define MSG_LOBBY_ADMIN_REREAD_WELCOME_EMAIL 25 //4 reqId
#define MSG_LOBBY_ADMIN_REREAD_WELCOME_EMAIL_REPLY 26 //46[s] reqId, err_code, [errDescr]

//#define MSG_LOBBY_ALLIN_EVENT				25 // 44ss,  tableId, HandId, userId, ipStr
//#define MSG_LOBBY_ALLIN_EVENT_REPLY			26 // no needed

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_REGISTER_NEW_USER			28 //sssssssssss14<1>s44s4D4sssssbss4<1>sbsss4M(ssssss444)44 - 
// userId,pwd,fullName,email,addr_1,addr_2,city,state,country,zipCode,installId,imageType,numBytes<Byte>,promoString,siteId,questType,questString,questFPP,DOB,locale,trackingVersion,trackingGUID,manualPromoCode,trackingCode,trackingInfo,lastAttempt,firstName,lastName,numCaptcha<cBytes>,captchaText,mailList,sex,refDomain,mobile,timezone,M(mac,volume,instId,machineguid,adapterguid,userExtraId,siteId,platform,clientExtraInfoMask),galleryImage,clientFlags
//for imageType constants please see dbm.h

#define MSG_LOBBY_REGISTER_NEW_USER_REPLY	29 // 6[s|4s4s] err_code[errDescr| fppForQuest PromoDescr 1stTimeBonusAmount 1stTimeBonusCurrency]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_LOBBY_GET_USER_INFO				30	//sbb userId, needImage, needPswDate
#define	MSG_LOBBY_GET_USER_INFO_REPLY		31	////46[s|ssssssssB444] - 
//q_id, err_code[errDescr|fullName,email,addr_1,addr_2,city,state,country,zipCode, image, privileges, flags, effectivePrivileges]

//PYR-39379 - no more bots registration
//#define	MSG_LOBBY_BOTS_REQUEST				34 //44 tournId, numBots
//#define	MSG_LOBBY_BOTS_REQUEST_REPLY		35 //6[s] err_code[errDescr]

//user to lobby - I wanna join a waiting list
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_LOBBY_WAITING_LIST				36 // s148 userName, listType, tableId32, tableId64

//lobby to user 
#define	MSG_LOBBY_WAITING_LIST_REPLY		37 //6[s...|48] err_code[errDescr...|tableId32, tableId64] NOTE msg structure in case of an error depends on the error code

//lobby to user - wanna take a seat?
#define	MSG_WAITING_INVITED					38	//4s41114411s4448 - tableId,tableName,seconds,game,isHiLo,structure,smartLoBet,hiBet,isOneOnOne,isPlayMoney,currency,waitingFlags,scalePM,clientReqId, tableId64

//user to lobby - yes/no
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_WAITING_INVITED_REPLY			39	// s41b8 userId, tableId32, answer (see USER_INVITATION_ANSWER_* constants), remove, tableId64

//lobby to itself
#define MSG_LOBBY_TIMER_USER_INVITED		40	// s4 userId, tableId

//lobby to user: you've been eliminated from waiting list
#define MSG_WAITING_SEAT_UNAVAILABLE		42	// 4s8 tableId, tableName32, tableId64

//lobby to table - this key is no longer valid, remove it
#define MSG_KEY_NOT_VALID					45 //44 reqId(0), uniqueKey

//lobby to user in waiting list - you said 'yes', so take your seat at the table
#define MSG_WAITING_TAKE_SEAT				46 // ss444 - server, serverObject, uniqueKey, flags, clientReqId

//lobby to table - user said 'yes' and will arrive soon (hopefully)
#define MSG_WAITING_USER_FOUND				47// 44 reqId(0), uniqueKey

//client to lobby - remove me from waiting list
#define MSG_LOBBY_LIST_REMOVE				48 //???

//from admin to lobby and back
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ACTIVATE_ACCOUNT					50 //// s4 userId, unique
#define MSG_LOBBY_ACTIVATE_ACCOUNT_REPLY			51 //6[s] err [errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_APPROVE_USERS		52 //// 44<s1s4> req_id, numUsers<userName, approval, comment, imageId>
#define MSG_LOBBY_ADMIN_APPROVE_USERS_REPLY	53 //// 46[s] q_id, err [errDescr] 

#define MSG_LOBBY_ADMIN_SET_PRIVILEGES			54 //// s4448888 userName, privileges, prev_privs, priv2, prev_priv2, priv3, prev_priv3
#define MSG_LOBBY_ADMIN_SET_PRIVILEGES_REPLY	55 //// 46[s] reqId, err [errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT		56 /// 44 reqId, tournId	
#define MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT_REPLY	57 /// 46[s] reqId, err [errDescr] 

#define MSG_LOBBY_ADMIN_REVOKE_CHAT				58 // 4[s1ss] numUsers,[userId, bitFlags(see RevokeChatFlags enum at dbm.h), strComment, strMessage]
#define MSG_LOBBY_ADMIN_REVOKE_CHAT_REPLY		59 // 4[6s] numUsers,[errorCode, email | errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_TOURN_ADDR			64 //reqId, 4 tournId
#define MSG_LOBBY_ADMIN_TOURN_ADDR_REPLY	65 //reqId, 6[s|ss] errCode, [errDescr|server, serverObj]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_USER_STATS						66 //s4 userName, numHands
#define MSG_LOBBY_USER_STATS_REPLY					67 //6[s] errCode, [errDescr|email]

#define MSG_LOBBY_TOURN_STATS						68 //1s4 requestType(see dbm.h for details), userName, tournIdOrNumTourns
#define MSG_LOBBY_TOURN_STATS_REPLY					69 //6[s] errCode, [errDescr|email]

/// from admin to lobby
#define MSG_LOBBY_ADMIN_BROADCAST			70 //4s4 reqId, msgText, flags
#define MSG_LOBBY_ADMIN_BROADCAST_REPLY		71 //46 reqId, errCode

// from lobby or mt lobby to table
//obsolete: #define MSG_TABLE_ADMIN_BROADCAST			72 //s4 msgText, flags
//obsolete: #define MSG_TABLE_ADMIN_BROADCAST_REPLY		73 //6 reqId, errCode

#define MSG_LOBBY_TABLE_CREATED				74 //4 tableId
#define MSG_LOBBY_TABLE_CREATED_REPLY		75 // never sent

//user to lobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_LOBBY_LEAVE_WAITING_LIST				76 // s148 userName, listType, tableId32, tableId64
//lobby to user 
#define	MSG_LOBBY_LEAVE_WAITING_LIST_REPLY			77 //6[s|48] err_code[errDescr|tableId32, tableId64]

//reg. lobby to table
#define MSG_LOBBY_TABLE_DEAD_REPLY					78 //44 reqId, tableId

//from lobby to mtlobby
#define MSG_LOBBY_MTLOBBY_DEAD_CONFIRM				79//44 reqId(0), tournId

//from lobby to table - user's privileges have been changed
// from mtlobby to table - the same
#define MSG_LOBBY_BROADCAST_PRIVILEGES				80 //41s444 - reqId(0), updateType, userId, newEffectivePrivileges, flags, imageId
//#define MSG_LOBBY_BROADCAST_PRIVILEGES_REPLY		81 //4 reqId // never sent...

// admin to lobby AND lobby to table
#define MSG_LOBBY_ADMIN_STOP_DEALING				82 //4s reqId, messageText
// lobby to admin only
#define MSG_LOBBY_ADMIN_STOP_DEALING_REPLY			83 //4 reqId

#define MSG_LOBBY_ADMIN_RESUME_DEALING				84 //4 reqId
#define MSG_LOBBY_ADMIN_RESUME_DEALING_REPLY		85 //4 reqId

#define MSG_LOBBY_CLIENT_ERROR						86 //ss userId, info
#define MSG_LOBBY_CLIENT_ERROR_REPLY				87 //empty

/* Obsoleted after PYR-38829
#define MSG_LOBBY_PLAY_TRANSFER						88 //ss4[8] userFrom, userTo, amount [,amount64]
#define MSG_LOBBY_PLAY_TRANSFER_REPLY				89 //6[s]
*/

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_USER_PROPS						90 //empty
#define MSG_LOBBY_USER_PROPS_REPLY					91 //44...b4bM(4<s4<s>>)bbbs4B(4444444)44<M(444)>b4<M(441)>44M(8888)8ts4
													   // privileges, flags ..., vipCounter3, fastDepositEnabled, 
													   // (numCountries<country, numPrefixes<userPrefix>>),
													   // handSharingEnabledForSite,itAccVerificationOpen,easySeatOn,UserWebId,
													   // numVipLevelBodies,B(statusId,vipCounterType,minFpp,fppBonusRate,vipToKeepYearlyStatus,lastMonthForYearly,missableMonthsYearly),
													   // chatModeratorRestrictions, numMilestoneBonusesNew<M(license,vppThreshold,amount)>,
													   // allowSmsValidationForNewInstallId,numLevels<M(levelId,threshold,flags)>,ltVpp,otherFlags,
													   // M(clientFeatureVisibilityOnMask,clientFeatureVisibilityOffMask,clientFeatureVisibilityOnMask2,clientFeatureVisibilityOffMask2),
													   // privileges3,registeredTime,state,license


//lobby to client - your unique key is no longer valid (user failed to click on "R" seat in time)
#define MSG_WAITING_KEY_EXPIRED						92 //4 unique

#define MSG_LOBBY_ADMIN_CANCEL_RANGE_TOURNAMENT		93 // 444 reqId, tournId_Start, tournId_End
#define MSG_LOBBY_ADMIN_CANCEL_RANGE_TOURNAMENT_REPLY 94 // 46[s] reqId, err [errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_DESTROY_TOURNAMENT			95 // 44 reqId, tournId
#define MSG_LOBBY_ADMIN_DESTROY_TOURNAMENT_REPLY	96 // 46[s] reqId, err [errDescr] 

/// formerly MSG_LOBBY_TABLE_TYPES
#define MSG_LOBBY_SETUP_INFO				101
// 2{TableTypeScript}
// TableTypeScript = 1111112 444 44444 
		// game, hilo, structur, playmoney, oneOnOne, maxpertable, maxtables, 
		// minChipsLimit, minStartChips, rake, 
		// ante, smallBlind, loBet, hiBet, bringin
#define MSG_LOBBY_SETUP_INFO_REPLY			102 //6[s] err_code[errDescr]

// from table to lobby
#define MSG_LOBBY_TABLE_USERS_CONN_QUAL			103 // 4415reqId, tableId, pos, quality
#define MSG_LOBBY_TABLE_USERS_CONN_QUAL_REPLY	104 // 4 reqId

//from admin to lobby
#define MSG_ADMIN_SET_NEWS						105 //4s reqId, newsText
#define MSG_ADMIN_SET_NEWS_REPLY				106 //46[s] reqId, err_code[,errDescr]

//client to mtlobby - user wants to register into tournament - what is his bankroll and tickets?
#define MSG_LOBBY_TOURN_REG_INFO				107 //s44 userId, tournId, TournRegInfoFlags
//answer from lobby to user
#define MSG_LOBBY_TOURN_REG_INFO_REPLY			108 //6[s|444s11444444s111sbmmts84144144b84<UserTicketData>8] - err_code,[errDescr|buyIn,rake,fpp,admissionId,isPlayMoney,isPwdProtected,userChips,userPlayChips,userFpp,numTickets,tChips,wChips(0),preRegMsg,game,isHiLo,structure,currency,enoughMoneyInOtherCurrency,currencyContext,clientCurrencyContext,whenStartAbs,tournName,playChips64,scalePM,speed,minPlayers,maxPlayers,maxPerTable,tournFlags,knockout,isProportional,admPrice,numEffAdm,<errectiveAdmissions>,tournFlags2]

// lobby to client(unsolicited)
//#define MSG_LOBBY_YOU_ARE_ON_WAITING_LIST		109 //empty

#define MSG_LOBBY_LOGOUT						109 //empty, client will wait for reply for certain time (not guaranteed)

#define MSG_LOBBY_REPORT_INSTALL				110 //ssss44M installIdDeprecated, wuid, promo, date, siteId, platformId, loginExtraData
#define MSG_LOBBY_TIMER_SFN_GWL_PLACES_UPDATE	111 

#define MSG_LOBBY_ADMIN_REINIT					112 //4 reqId
#define MSG_LOBBY_ADMIN_REINIT_REPLY			113 //46[s] reqId, err_code, [errDescr]

//admin to lobby
#define MSG_LOBBY_ADMIN_SAY_TO_USER			114 //4ss reqId userId messageText
#define MSG_LOBBY_ADMIN_SAY_TO_USER_REPLY	115 //46[s] reqId, err_code, [errDescr]

#define MSG_LOBBY_TABLES_DISCONN_BROADCAST	116 //14 disconnectState(0|1|2), delay(in sec) 	
#define MSG_LOBBY_MTLOBBY_DISCONN_BROADCAST	117 //14 disconnectState(0|1|2), delay(in sec) 			

#define MSG_LOBBY_TIMER_SAY_USER			118 //4 sayRequestId (no reply)

// from admin to lobby - re-create existing tourney with a new script
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_RECREATE_TOURNAMENT		119 //44(script)4
#define MSG_LOBBY_RECREATE_TOURNAMENT_REPLY	120 //46[s|44]

// PYR-52382: Deprecated
//#define MSG_LOBBY_GET_IMAGE				125 //4 imageId
//#define MSG_LOBBY_GET_IMAGE_REPLY			126 //4B imageId, image

#define MSG_LOBBY_FORCECLIENTLOGOUT			127 // [bs] - [logoutRightAway, errMsg]

#define MSG_LOBBY_ADMIN_FORCELOGOUT_USER	128 //4s reqId, user
#define MSG_LOBBY_ADMIN_FORCELOGOUT_USER_REPLY	129 //46[s] reqId, err_code, [errDescr]

#define MSG_TABLE_MTLOBBY_TIMESTOP			130 //41 tableId, stopClock(bool)
#define MSG_TABLE_MTLOBBY_TIMESTOP_REPLY	131 //6[s] err_code, [errDescr]

#define MSG_LOBBY_MTLOBBY_DEAD_IF_EMPTY		132 // 4 tournid

//#define MSG_LOBBY_ADMIN_NEWSTANDBY			133 // 44<IniFile> reqId, nFiles<iniFile>
//#define MSG_LOBBY_ADMIN_NEWSTANDBY_REPLY	134 //reqId, 6[s] errCode, [errDescr]

#define MTL_LOBBY_CANCEL_TOURN				135 // 4sB tournid, effCanceller, needRefundInfo

#define MSG_LOBBY_ADMIN_INFO				136 // s message
#define MSG_LOBBY_ADMIN_INFO_REPLY			137

#define MSG_LOBBY_TOURN_INVITE				138 // 4ss4444b48 tournId, message, server, tournFlags, clubId, rematchedTournId, sngScriptId, isRestarted, pppId, pppEntryId

#define MSG_LOBBY_TABLE_DISSOLVE			139 // 4s tableId, effCanceller
//#define MSG_LOBBY_TABLE_DISSOLVE_REPLY		140 // 46[s] tableId, err_code [errDescr]

//broadcast to tables
#define MSG_LOBBY_TABLE_SET_FPP_FACTOR		141 //42<24> reqId(=0) numFppFactors<newFppFactor,sites>

#define MSG_LOBBY_TABLE_RATHOLING			142 //4124 reqId(=0) ratholingPercent, ratholingMinutes, ratHoleMax

#define MSG_LOBBY_TIMER_UPDATE_PUBLICATION	143 //empty
#define MSG_LOBBY_TIMER_CREATETOURN			144 // 411, tournId, recovered, dbStatus

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_GET_TLB					145 //s userId
#define MSG_LOBBY_GET_TLB_REPLY				146	//6s, err_code errDescrOrEmail

#define MSG_LOBBY_ADMIN_SET_DISCONN_STATE       147 //1 state 0 - initial,1-mass delayed 2-mass disconnect 
#define MSG_LOBBY_ADMIN_SET_DISCONN_STATE_REPLY	148	//6[s]1 err_code [errDescr] old state

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_RESUME_ALLTOURNAMENTS	149 // 4444<4> reqId, flags, site, tournNum<tournId>
#define MSG_LOBBY_ADMIN_RESUME_ALLTOURNAMENTS_REPLY	150 // 46[s] reqId errCode, [errDescr]

#define MSG_LOBBY_ADMIN_STOP_ALLTOURNAMENTS	151 // 4s4 reqId, AdminMessage, flags
#define MSG_LOBBY_ADMIN_STOP_ALLTOURNAMENTS_REPLY	152 // 46[s] reqId errCode, [errDescr]

#define MSG_STOPDEALING_STATE				153 //441, reqId, tableId, stopDealingState 
#define MSG_STOPDEALING_STATE_REPLY			154	//4, reqId

#define MSG_LOBBY_ADMIN_STOP_TOURNREGEN			155
#define MSG_LOBBY_ADMIN_STOP_TOURNREGEN_REPLY	156

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_RESUME_TOURNREGEN		157
#define MSG_LOBBY_ADMIN_RESUME_TOURNREGEN_REPLY	158

#define MSG_LOBBY_ADMIN_GET_TABLES_STATE		159 //4, reqId
#define MSG_LOBBY_ADMIN_GET_TABLES_STATE_REPLY	160	//4,4<4s114411111>,4<41114444441> reqId NumTables<tableId,tableName,game,isHiLo, hibet,lobet,structure,isplaymoney,State,Sit,Disconn>, NumTourn<tournId,game,isHiLo,structure,buyin,status,enrolled,numberUsers,numberRealUsers,userDisconncted,dealingStatus>

#define MSG_LOBBY_ADMIN_GET_TABLES_STATE2		161 //4 - reqId
#define MSG_LOBBY_ADMIN_GET_TABLES_STATE2_REPLY	162 //44<4s11441111144>4<41114444441144444>4<4s11441114444> - reqId, NumTables<tableId,tableName,game,isHiLo, hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId>, NumTourn<tournId,game,isHiLo,structure,buyin,status,enrolled,numberUsers,numberRealUsers,userDisconncted,dealingStatus,isPlayMoney,fppBuyIn,tournFlags,siteMask,knockout,clubId>, NumBlitz<blitzId,blitzName,game,isHiLo,hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId>

#define MSG_LOBBY_CLI_CHATINIT					163 //4ss sessionId, addr, object
#define MSG_LOBBY_CLI_CHATINIT_REPLY			164 //4 sessionId

#define MSG_LOBBY_ADMIN_CHATINIT				165 //4sss sessionId, username, addr, object
#define MSG_LOBBY_ADMIN_CHATINIT_REPLY			166 //4s[4] err, descr, sessionId

#define MSG_LOBBY_ADMIN_CHAT_STOP				167 //4 sessionId
#define MSG_LOBBY_ADMIN_CHAT_TEXT				168 //4s sessionId, text
#define MSG_LOBBY_CLI_CHAT_TEXT					169 //4s[t] sessionId, text[,timestamp]
#define MSG_LOBBY_CLI_CHAT_PROGRESS				170 //4 sessionId

#define MSG_LOBBY_ADMIN_GET_TABLES_STATE2_EX		171 //4 - reqId
#define MSG_LOBBY_ADMIN_GET_TABLES_STATE2_EX_REPLY	172 //44<M(4s114411111m4>4<M(41114444441144m448)4<M(4s114411144m4)> - reqId, NumTables<M(tableId,tableName,game,isHiLo,hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId)>, NumTourn<M(tournId,game,isHiLo,structure,buyin,status,enrolled,numberUsers,numberRealUsers,userDisconncted,dealingStatus,isPlayMoney,fppBuyIn,tournFlags,siteMask,knockout,clubId,tournFlags2)>, NumBlitz<M(blitzId,blitzName,game,isHiLo,hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId)>

//#define MSG_LOBBY_RESET_PWD						173 //sss userId, zip, addr
//#define MSG_LOBBY_RESET_PWD_REPLY				174 //6s

#define MSG_LOBBY_FIND_TOURN_REG				177 //s userId
#define MSG_LOBBY_FIND_TOURN_REG_REPLY			178 //
/*
#define MSG_LOBBY_REAL_TRANSFER					179 //ss4 userFrom, userTo, amount
#define MSG_LOBBY_REAL_TRANSFER_REPLY			180 //6[s] err_code, [errDescr]
*/
#define MSG_LOBBY_MAILER_BOUNCED				181 //4ss
#define MSG_LOBBY_MAILER_BOUNCED_REPLY			182 //4
/*
#define MSG_LOBBY_REAL_PRE_TRANSFER				183 //ss4 userFrom, userTo, amount
#define MSG_LOBBY_REAL_PRE_TRANSFER_REPLY		184 //6[s|s] err_code, [errDescr], realName
*/
#define MSG_LOBBY_CLI_USER_WHERE				185 //6[s|4<4>4<4>4<4>]

//#define MSG_LOBBY_TIMER_TRANSFER_FUNDS		186 // (obsolete after #11196)

#define MSG_LOBBY_REPLAY_CREATED				187 //4 tableId
#define MSG_LOBBY_REPLAY_CREATED_REPLY			188 //

#define MSG_LOBBY_ADMIN_CREATE_REPLAY			189 //ReplayScript, IniFile
#define MSG_LOBBY_ADMIN_CREATE_REPLAY_REPLY		190 //

#define MSG_LOBBY_ADMIN_CANCEL_REPLAY			191 //4 replayId
#define MSG_LOBBY_ADMIN_CANCEL_REPLAY_REPLY		192	//6[s]

#define MSG_LOBBY_REPLAY_DESTROY				193 //

#define MSG_LOBBY_BROADCAST_MULTI_PRIVILEGES	194 //44<1s444> - reqId(0), numItems, <updateType, userId, newEffectivePrivileges, flags, imageId>

#define MSG_LOBBY_CLI_USERTOURNAMENT_NOTIFY		197 // 41 tournId, reg/unreg

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT_WITHUSERS			198 // see MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT
#define MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT_WITHUSERS_REPLY	199 // see MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT_REPLY

#define MSG_TABLE_MTLOBBY_USERREBUYS			200 //14s6 syncRebuy, tableId, user, rebuy/s (1 = regular, 2-3 = double, triple, etc... rebuys)
#define MSG_TABLE_MTLOBBY_USERREBUYS_REPLY		201 //81s6[s|4444444s8226777] - origTableId,syncRebuy,user,err_code,[errDescr|tournamentchips,chips,playchips,fpp,tchips,wchips,numRebuys,aamsTicket,playChips64,rebuysDone,addonsDone,rebuysLeft,moneyAfter,realChips,realTchips]

#define MSG_TABLE_MTLOBBY_USERADDONS			202 //4s tableId, user
#define MSG_TABLE_MTLOBBY_USERADDONS_REPLY		203 // 8s6[s|444444s8226777] - origTableId,user,err_code,[errDescr|tournamentchips,chips,playchips,fpp,tchips,wchips,aamsTicket,playChips64,rebuysDone,addonsDone,rebuysLeft,moneyAfter,realChips,realTchips]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_FIND_WAIT						206 //s userId
#define MSG_LOBBY_FIND_WAIT_REPLY				207 //6[4<44>4<ss>4<4>] errCode, numItems <tableId, order>, numItems <server, serverObject>, numItems <total>

#define MSG_LOBBY_ADMIN_RESTART_TOURN			208 //44 reqId, tournId
#define MSG_LOBBY_ADMIN_RESTART_TOURN_REPLY		209 //46[s] reqId, err_code [errDescr]

#define MSG_LOBBY_CLI_BOUNCED					210 // s email
#define MSG_LOBBY_CLI_BOUNCED_REPLY				211 // s1[b] userId doReset [sendValidationCode]

#define MSG_LOBBY_ADMIN_CREATE_REPLAY_LIVE		212 // IniFile
#define MSG_LOBBY_ADMIN_CREATE_REPLAY_LIVE_REPLY	213 //

#define MSG_LOBBY_ADMIN_CANCEL_REPLAY_LIVE			214 //4 replayId
#define MSG_LOBBY_ADMIN_CANCEL_REPLAY_LIVE_REPLY		215	//6[s]

#define MSG_TABLE_ADMIN_REINIT					216 //4 reqId
#define MSG_TABLE_ADMIN_REINIT_REPLY			217 //46[s] reqId, err_code, [errDescr]

#define MSG_LOBBY_SECURITY						218 // 4s

#define MSG_LOBBY_ADMIN_CANCEL_RUNNING_TOURN	219 // 444s reqId, tournType(see constants below), rollback/rollforward formula, message
#define MSG_LOBBY_ADMIN_CANCEL_RUNNING_TOURN_REPLY 220 //46[s] reqId, err_code [errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_CREATE_USERTOURN				221
#define MSG_LOBBY_CREATE_USERTOURN_REPLY		222

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_CANCEL_USERTOURN				223
#define MSG_LOBBY_CANCEL_USERTOURN_REPLY		224

#define MSG_TABLE_IMAGES_BROADCAST				225 //41 recId, switch (1-Off, 0-On)

#define MSG_LOBBY_ADMIN_NOIMAGES				226 //41 reqId 
#define MSG_LOBBY_ADMIN_NOIMAGES_REPLY			227 //46[s] reqId, err_code [errDescr]

#define MSG_LOBBY_CLIENT_QUEST					230
#define MSG_LOBBY_CLIENT_QUEST_REPLY			231

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_DONT_SHOW_WHERE				232
#define MSG_LOBBY_DONT_SHOW_WHERE_REPLY			233

#define MSG_LOBBY_REMIND_SID					234
#define MSG_LOBBY_REMIND_SID_REPLY				235 //6sb errCode,[errDescr|email|mobile],viaSMS

#define MSG_LOBBY_HISTORY_REQUEST_ACCEPTED		236

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_CLI_GET_TLB_STANDIDNS2		237 //s1s userId tlbIdx uniqueIdStr
#define MSG_LOBBY_CLI_GET_TLB_STANDIDNS2_REPLY	238 //

//#define MSG_LOBBY_GET_TABLE_ID					239 // s //for test only
//#define MSG_LOBBY_GET_TABLE_ID_REPLY			240 // s4

#define MSG_LOBBY_SEND_PM_TO_OTHER_SITE			241
#define MSG_LOBBY_SEND_PM_TO_OTHER_SITE_REPLY	242

#define MSG_LOBBY_PROMO_COUNTRY					243
#define MSG_LOBBY_PROMO_COUNTRY_REPLY			244

#define MSG_LOBBY_GET_SATELLITES_TO_TOURN		245
#define MSG_LOBBY_GET_SATELLITES_TO_TOURN_REPLY	246

#define MSG_TABLE_ADMIN_BROADCAST2				249 //s44 msgText, flags, site
#define MSG_TABLE_ADMIN_BROADCAST2_REPLY		250 //6 reqId, errCode

#define MSG_LOBBY_TIMER_REQUEST_STATS			251

#define MSG_LOBBY_GET_SATELLITES_TO_TOURN_EX		252
#define MSG_LOBBY_GET_SATELLITES_TO_TOURN_EX_REPLY	253

/* TSALE commented out after 2011-01 build since 1.62 client protocol is no longer supported
#define MSG_LOBBY_TCHIPS_SALE					254	//ss1444s seller buyer is_buyer tChips wChips money currency
#define MSG_LOBBY_TCHIPS_SALE_REPLY				255	// 6[s] err_code [errDescr]

#define MSG_LOBBY_TCHIPS_PRESALE			    256	// ss1444 seller buyer is_buyer tChips wChips money
#define MSG_LOBBY_TCHIPS_PRESALE_REPLY			257	// ss14446[s] seller buyer is_buyer tChips wChips money err_code [errDescr]

#define MSG_LOBBY_TCHIPS_PRECANCEL				258	//s userId
#define MSG_LOBBY_TCHIPS_PRECANCEL_REPLY		259	//(same as DBM_A_PRECANCEL_TSALES)

#define MSG_LOBBY_TCHIPS_CANCELID				260
#define MSG_LOBBY_TCHIPS_CANCELID_REPLY			261
*/

#define MSG_LOBBY_FIND_TOURN					262
#define MSG_LOBBY_FIND_TOURN_REPLY				263

#define MSG_LOBBY_TABLE_KICK_USER				264 // ss7b   userId, messageText, kickReason, tableReadyToDie
//#define MSG_LOBBY_TABLE_KICK_USER_REPLY		265 // 6[s] err_code [errDescr]

#define MSG_LOBBY_ADMIN_KICK_USER_FORM_TABLE	266 // 4sss   reqId, userId, tableName, messageText
#define MSG_LOBBY_ADMIN_KICK_USER_FORM_TABLE_REPLY	267 // 46[s] reqId, err_code [errDescr]

#define MSG_LOBBY_CHAT_COMPL					268 // 8ss44<ss>4<s>4s48 handId64, tableName, comments, tableId32, numLines <name, chatText>, numPlayers <name>, tournId, finalTableText, zoomReplayId, tableId64
#define MSG_LOBBY_CHAT_COMPL_REPLY				269 // 6s err_code [errDescr]

// switched to MSG_LOBBY_USER_TOURNTICKETS2 as MSG_LOBBY_USER_TOURNTICKETS is no longer used
//#define MSG_LOBBY_USER_TOURNTICKETS			270 // s user
//#define MSG_LOBBY_USER_TOURNTICKETS_REPLY		271 // 6[s|4<4sb4t>] err_code[errDescr| numTickets<ticketid, description, multitourn, price, issued>]

#define MSG_LOBBY_GET_TABLE_BY_ID				272 // 448 tableId32, tournId, tableId64
#define MSG_LOBBY_GET_TABLE_BY_ID_REPLY			273 // 6[s|s ] err_code[errDescr|server ]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_SET_TOURN_EMAIL_FLAGS			274
#define MSG_LOBBY_SET_TOURN_EMAIL_FLAGS_REPLY	275

#define MSG_LOBBY_SECURITY2						276 // 44s code, severity, description

#define MSG_LOBBY_ADMIN_MODERATOR_SWITCH		277 //41 reqId, switch
#define MSG_LOBBY_ADMIN_MODERATOR_SWITCH_REPLY	278

#define MSG_LOBBY_TABLES_MODERATOR_SWITCH		279 // 1 (mod_chat turn: 0-off 1-on )

#define MSG_LOBBY_SEND_VC_TO_CLIENT				297 // lobby->client
#define MSG_LOBBY_ASK_MONET_FOR_INFO			298 // lobby->monet
#define MSG_MONET_SEND_TO_LOBBY_INFO			299 // monet->lobby

#define MSG_MONET_ASK_LOBBY_SEND_VC_TO_CLI		300 // monet->lobby
#define MSG_MONET_ASK_LOBBY_SEND_VC_TO_CLI_RPLY 301 // lobby->monet
#define MSG_LOBBY_RECEIVE_VC_FROM_CLIENT_BLOCKABLE 302 // client->lobby: can be blocked by config para reportUserLogInOut2Monet
#define MSG_LOBBY_RECEIVE_VC_FROM_CLIENT		303 // client->lobby
#define MSG_LOBBY_SEND_CLIENT_VC_TO_MONET		304 // lobby->monet
#define MSG_MONET_DUMMY_REPLY					305 // monet->lobby

#define MSG_LOBBY_TELL_MONET_INFO				306 // lobby->monet
//#define MSG_MONET_ADMIN_QUERY					306 // cgi->monet
#define MSG_MONET_ADMIN_BOT_USER_STATE_REPLY	307 // monet->cgi
#define MSG_MONET_ASK_LOBBY_USER_STATE			308 // monet->lobby
#define MSG_MONET_ASK_LOBBY_USER_STATE_REPLY	309 // lobby->monet
#define MSG_LOBBY_TELL_MONET_USER_STATE			310 // monet->lobby

#define MSG_MONET_CHECK							311
#define MSG_MONET_CHECK_REPLY					312

#define MSG_MONET_ADMIN_GET_USER_STATE			313 // cgi->monet
#define MSG_MONET_ADMIN_USER_ACTION				314 // cgi->monet
#define MSG_MONET_ADMIN_REREAD_INI				315 // cgi->monet
#define MSG_MONET_ADMIN_GET_INI					316 // cgi->monet
#define MSG_MONET_ADMIN_UPDATE_DBM				317 // cgi->monet
#define MSG_MONET_ADMIN_DROP_USER_STATE			318 // cgi->monet
#define MSG_MONET_ADMIN_GET_ALL_USER_LIST		319 // cgi->monet
#define MSG_MONET_ADMIN_GET_ACTIVE_USER_LIST	320 // cgi->monet
#define MSG_MONET_ADMIN_GET_GLOBAL_STATE		321 // cgi->monet

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_UPDATE_DOB					322
#define MSG_LOBBY_UPDATE_DOB_REPLY				323

#define MSG_LOBBY_TABLE_JUBILEE_HAND			324 // UINT64 denominator
#define MSG_LOBBY_TABLE_JUBILEE_HAND_REPLY		325 // 

// PYR-58330 - obsolete
#define MSG_LOBBY_LOCALE_SET					328 // s4s userId, locale #, lang.

#define MSG_LOBBY_ADMIN_GET_TEAM_PS_LIST		329 //4 reqId
#define MSG_LOBBY_ADMIN_GET_TEAM_PS_LIST_REPLY	330 //46[s|4<sss> reqId, err_code, [errDescr | listSize<userId, fullname, displayCountry>]

#define MSG_LOBBY_ADMIN_SET_TEAM_PS_LIST		331 //44<sss>4 - reqId,l istSize<userId, fullname, displayCountry>, siteMask
#define MSG_LOBBY_ADMIN_SET_TEAM_PS_LIST_REPLY	332 //46[s|4<s>] reqId,err_code [errDescr | listSize<userId, errDescr>]

#define MSG_LOBBY_CLI_TEAM_PS_ONLINE			332
#define MSG_LOBBY_CLI_TEAM_PS_ONLINE_REPLY		333 // 4<s,s,1>4<Msg(1,s,s,s,s)> - numPlayers<userId, fullname, online>, numPlayers<Msg(online, userId, name, country, displayCountry)>

#define MSG_LOBBY_TABLE_STOP_DEALING_JUBILTYPE	334 //4s reqId, messageText

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_EXCLUDE_PLAYER				335	//s448b1 username days_to_exclude,flags,flags2,groupSelfExclusion,version
#define MSG_LOBBY_EXCLUDE_PLAYER_REPLY			336	//6[s|4T4] days excluded, until, days requested

#define MSG_LOBBY_TIMER_REQUEST_SELFEXCLUDED	337

#define MSG_LOBBY_CLI_NEWS						338 // ss4<msg:4s> server object nSubscr< msg: revision subscrName >

#define MSG_LOBBY_REREAD_MSGS  					343 //44
#define MSG_LOBBY_REREAD_MSGS_REPLY				344 //46[s]

#define MSG_LOBBY_SHADOW_TLB  					345 //
#define MSG_LOBBY_SHADOW_TLB_REPLY				346 //6[...]

#define MSG_LOBBY_SHADOW_TLB_TOP 				347 //4
#define MSG_LOBBY_SHADOW_TLB_TOP_REPLY			348 //6[...]

#define MSG_TABLE_OBSERVER_CHAT					349 // 1, chat state 

#define MSG_TABLE_QUERY_OBSERVER_CHAT			350 // 44 transparent1, transpatrnt2 
#define MSG_TABLE_QUERY_OBSERVER_CHAT_REPLY		351 // 44s1 transparent1, transpatrnt2, tablename, chat state
#define MSG_TABLE_QUERY_OBSERVER_CHAT_REPLY_REPLY	352 // dummy

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_GET_CHAT_STATUS			353 //4s reqId, tableName
#define MSG_LOBBY_ADMIN_GET_CHAT_STATUS_REPLY	354 //46[s|s1] reqId, err_code[errDescr| tableName, status]

#define MSG_LOBBY_ADMIN_SET_CHAT_STATUS			355 //41sb reqId, status, tableid, teamproblocked
#define MSG_LOBBY_ADMIN_SET_CHAT_STATUS_REPLY	356 //46[s] reqId, err_code[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_INTELLIPOKER_SIGNUP			357 //ss	userName IntellipokerPassword
#define MSG_LOBBY_INTELLIPOKER_SIGNUP_REPLY		358 //6[s]	err_code[errDescr]

#define MSG_LOBBY_IMAGE_INFO					359 //4 imageId
#define MSG_LOBBY_IMAGE_INFO_REPLY				360 //44 imageId, status

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_INTELLIPOKER_PWD			359 //ss	userName IntellipokerPassword
#define MSG_LOBBY_INTELLIPOKER_PWD_REPLY		360 //6[s]	err_code[errDescr]

//#define MSG_LOBBY_TIMER_CASH_TRANSFER			361 //ss444sb - userFrom, userTo, transFromId, transToId, transferAmount, comment, awareOfBan

// !!! WARNING !!! If you modify this message, please let testautomation team know.
// PYR-58330 - obsolete
#define MSG_LOBBY_EMLOCALE_SET					362 // s4s userId, locale #, lang.

//#define MSG_LOBBY_CLI_GET_TLB_STANDIDNS_BOP		363
//#define MSG_LOBBY_CLI_GET_TLB_STANDIDNS_BOP_REPLY	364

// PYR-58330 - obsolete
#define MSG_LOBBY_LOCALE_SET_REPLY				365 //6[s|44]	err_code[errDescr|locale, emLocale]
// PYR-58330 - obsolete
#define MSG_LOBBY_EMLOCALE_SET_REPLY			366 //6[s|44]	err_code[errDescr|locale, emLocale]

//#define MSG_LOBBY_USER_BOP						367 //s userId
//#define MSG_LOBBY_USER_BOP_REPLY				368	//

//#define MSG_LOBBY_ADMIN_USER_BOP				369 //4s4 reqId, userId, userIndId
//#define MSG_LOBBY_ADMIN_USER_BOP_REPLY			370	//

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_I18N_BROADCAST			371	//44(4s)41 - reqId,i18nString,flag,dst
#define MSG_LOBBY_ADMIN_I18N_BROADCAST_REPLY	372	//46s - reqId,errCode,errDescr

#define MSG_TABLE_ADMIN_I18N_BROADCAST			373 //4(4s)4 i18nString, flags
#define MSG_TABLE_ADMIN_I18N_BROADCAST_REPLY	374 //46 reqId, errCode

#define MSG_LOBBY_ADMIN_LOBBY_FLYER				375 //41444s[4] reqId, action, attributes, flags, locale, text, [siteId]
#define MSG_LOBBY_ADMIN_LOBBY_FLYER_REPLY		376	//46 reqId, errCode

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_REDIRECTOR_URL						379 // 4Ms4 site, clientExtraData, userId (unused on server), licenseId
#define MSG_REDIRECTOR_URL_REPLY				380 // ssss url, country, ip, region

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_PUBLISH_VIP_STATUS			381 // s1 userId, publish

#define MSG_LOBBY_CLI_NEWS_I18N					382 // ss4<msg:4s>44444mmm server object nSubscr< msg: revision subscrName >, fpp, vipCounter1, vipCounter2, vipStatus, vipCounter3, lobbymessage, lobbymessage2, tablemessage

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_TIMEZONE_SET					383 // s4s userId, timezone, timezone name
#define MSG_LOBBY_TIMEZONE_SET_REPLY			384 //6[s|4]	err_code[errDescr|timezone]

#define MSG_LOBBY_FIND_PLAYER2					385 //ss userId, playername
#define MSG_LOBBY_FIND_PLAYER2_REPLY			386 //6[s|s4<>4<>4<>]		errCode[errDescr|playername,4<table data>,4<tourns data>,4<blitz data>] Note table data is ss4ssb4411bs48 - Added TableId64 to the end

#define MSG_LOBBY_REQUEST_RM_ACCOUNT_NOTIFY		387	// [44] [siteId, [additional flags]]

#define MSG_LOBBY_REQUEST_AUTH_CONFIRM_NOTIFY	388	//

#define	MSG_LOBBY_UPDATE_USER2					389 // UserUpdate, 4 flags, 8 flags2
#define	MSG_LOBBY_UPDATE_USER2_REPLY			390 // 6[s[4]|sbbM(1s)] errCode [errDescr [errReason] | [if(err==TS_ERROR_SMS_MSG_SUBMITTED) errDescr, mobile,] frArjelUserHash, doReconnect, emailAutoValidated, arjelContext(realMoneyUpdate,accountNumber)]

//#define	MSG_LOBBY_UPDATERM_USER2				391 //
//#define	MSG_LOBBY_UPDATERM_USER2_REPLY			392 //

#define	MSG_LOBBY_IT_DOCUMENT					393 //
#define	MSG_LOBBY_IT_DOCUMENT_REPLY				394 //

#define MSG_LOBBY_ADMIN_I18N_AUTOBROADCAST			395	//44(4s)444488444<s> reqId, i18nString, flag, format, stake, struct, tournGame, ringGame, site, vip, countryCount, <countries>
#define MSG_LOBBY_ADMIN_I18N_AUTOBROADCAST_REPLY	396	//46 reqId, errCode

#define MSG_TABLE_ADMIN_I18N_AUTOBROADCAST			397 //4(4s)4UserSpecificNotifyParams i18nString, flags, UserSpecificNotifyParams
#define MSG_TABLE_ADMIN_I18N_AUTOBROADCAST_REPLY	398 //46 reqId, errCode

#define MTL_TOURN_ADMIN_I18N_AUTOBROADCAST			399 //4(4s)4 i18nBroadcastText, flags
#define MTL_TOURN_ADMIN_I18N_AUTOBROADCAST_REPLY	400 // 6 err_code

#define MSG_LOBBY_ASK_VC_FOR_SIGN_UP			401 // lobby -> Monet
#define MSG_LOBBY_ASK_VC_FOR_SIGN_UP_REPLY		402 // Monet -> lobby

#define STANDIDNS_REPLY_NOT_CHANGED				403

#define MSG_LOBBY_ADMIN_CLIENT_DATA_MINING		404 //41 reqId, 
#define MSG_LOBBY_ADMIN_CLIENT_DATA_MINING_REPLY	405

#define MSG_LOBBY_ADMIN_UPDATE_TABLE_NAMES			406 //41 reqId, 
#define MSG_LOBBY_ADMIN_UPDATE_TABLE_NAMES_REPLY	407

// PYR-23394
//obsolete:#define MSG_LOBBY_UPDATE_SMS_MARKETING_FLAG		408 // 1 setNotRemove (client->lobby)
//obsolete:#define MSG_LOBBY_UPDATE_SMS_MARKETING_FLAG_REPLY 409

#define MSG_LOBBY_FIND_TOURN_REG_IT				410 //s userId
#define MSG_LOBBY_FIND_TOURN_REG_IT_REPLY		411 //2[s|4<4M(TournData)>4<4M(TableData 8)>] errCode[errDesc, numTourn<tournId, M(TournData)>, numTable<tableId32, M(TableData, tableId64)>]

#define MSG_LOBBY_FIND_TOURN_IT					412 //s userId
#define MSG_LOBBY_FIND_TOURN_IT_REPLY			413 //2[s|4<4M(TournData)>4<4M(TableData 8)>] errCode[errDesc, numTourn<tournId, M(TournData)>, numTable<tableId32, M(TableData, tableId64)>]

#define MSG_LOBBY_REFRESH_TABLEDBM_SETTINGS		414 // 4 reqid
#define MSG_LOBBY_REFRESH_TABLEDBM_SETTINGS_REPLY		415 // 46s

//#define MSG_LOBBY_CLI_REMATCH_INFO				416 // lobby to client
#define MSG_LOBBY_CLI_REMATCH_ACCEPT			417 // client to lobby
#define MSG_LOBBY_CLI_REMATCH_DECLINE			418 // client to lobby
#define MSG_LOBBY_CLI_REMATCH_CANCEL			419 // lobby to client
#define MSG_LOBBY_CLI_REMATCH_OK				420 // lobby to client
#define MSG_LOBBY_CLI_REMATCH_UPDATE			421 // lobby to client
#define MSG_LOBBY_CLI_REMATCH_REPLY				422 // lobby to client

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_PLAYING_HISTORY_AUDIT			423 // 4sddsb4b - reqId, userId, startDate, endDate, password, includeFPP, eReportFormat, notifyClient
#define MSG_LOBBY_PLAYING_HISTORY_AUDIT_REPLY	424	//reqId, 6[s] errCode, [errDescr]

//obsolete
#define MSG_LOBBY_ADMIN_NEW_SITANDGO			425 // 4ss4<IniFile> reqId, title, fileName, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_NEW_SITANDGO_REPLY		426 //reqId, 6[s] errCode, [errDescr]

//obsolete
#define MSG_LOBBY_ADMIN_UPDATE_SITANDGO			427 // 4ss4<IniFile> reqId, title, fileName, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_UPDATE_SITANDGO_REPLY	428 //reqId, 6[s] errCode, [errDescr]

#define MSG_LOBBY_ADMIN_DELETE_SITANDGO			429 // 4s4 reqId, title, segment
#define MSG_LOBBY_ADMIN_DELETE_SITANDGO_REPLY	430 //reqId, 6[s] errCode, [errDescr]

#define MSG_LOBBY_ADMIN_QUERY_SITANDGO			431 // 4s reqId
#define MSG_LOBBY_ADMIN_QUERY_SITANDGO_REPLY	432 //reqId, 6[s|4<s>] errCode, numTitles<title>

#define MSG_LOBBY_AUTO_CLOSE_ACCOUNT			435 // s4 userId, locale
#define MSG_LOBBY_AUTO_CLOSE_ACCOUNT_REPLY	    436 // 6[s] errCode, [errDescr]

#define MSG_CLI_ACCOUNT_AUTO_CLOSED				437 // s userid

#define MSG_LOBBY_SIGNUP_QUESTIONNAIRE			439	// s4 - country,locale
#define MSG_LOBBY_SIGNUP_QUESTIONNAIRE_REPLY	440 // 6[s|4<4ss4<s>>] errCode,numSources<sourceId,sourceName,pleaseSpecify,numOptions<optionText>>

#define MSG_LOBBY_CLI_REMATCH_INFO_2			441 // MSG44<s>4 // RegInfoMsg, tournId, numPlayers<playername>, timeout

#define MSG_LOBBY_TIMER_REQUEST_REFERRALS		442

#define MSG_WEB_FIND_TOURN						443
#define MSG_WEB_FIND_TOURN_REPLY				444

#define MSG_WEB_FIND_PLAYER2					445 //ss userId, playername
#define MSG_WEB_FIND_PLAYER2_REPLY				446 //66[s|s4<>4<>4<>]		errCode[errDescr|playername,4<table data>,4<tourns data>,4<blitz data>] Note table data is ss4ssb4411bs48 - Added TableId64 to the end

#define MSG_LOBBY_USER_CHIPS_QUERY				447 //4s14s4 - q_id, user_id, is_playM, is_SitIn, currency, minBuyIn
#define MSG_LOBBY_USER_CHIPS_QUERY_REPLY		448 //46[sb|414b] q_id, err_code [err_descr, enoughOnAllAccounts|chips, playAddedMask, playAddedAmount, enoughOnAllAccounts]

// obsoleted by PYR-107820
//#define MSG_LOBBY_WEB_TEAM_PS_OLD				449
//#define MSG_LOBBY_WEB_TEAM_PS_OLD_REPLY		450 // ssss4 - userId, name, country, displayCountry, siteId

#define MSG_LOBBY_REWARD_PAYMENT				451

// [AO 2010-01-05] PYR-13431
#define MSG_LOBBY_VPPS_CHANGED					452	// 4444444 - fpps, monthlyVpps, yearlyVpps, vipStatus, numLevels = 0, dailyVpps, ltVpps
#define MSG_LOBBY_TABLE_VPP_THRESHOLD			453	// 444<4,4> - reqId=0, vppThreshold,perLevelThresholdNum<VIPLevel,vppThreshold>

// PYR-12422
#define MSG_LOBBY_RETRIEVE_USERID		        455	// s44 email locale clientSiteId
#define MSG_LOBBY_RETRIEVE_USERID_REPLY	        456	// 6[s|2<4s>] errCode [errStr|size, id 1, question1, id2, question2]

#define MSG_LOBBY_RELATED_USERS_GROUPS			457
#define MSG_LOBBY_RELATED_USERS_GROUPS_REPLY	458

#define MSG_LOBBY_CHECK_ACTIVE_USER				459 // 4s reqId, userId
#define MSG_LOBBY_CHECK_ACTIVE_USER_REPLY		460 // 4444 - reqId,numTables,numTourns,numBltzs

#define MSG_LOBBY_REPORT_UN_AUTH_MONET			461
#define MSG_LOBBY_REPORT_UN_AUTH_MONET_REPLY	462

//PYR-14427, .FR client <--> lobby
#define MSG_LOBBY_FR_AVC_VALIDATE				463	//4 - avc
#define MSG_LOBBY_FR_AVC_VALIDATE_REPLY			464	//6[|bb] - errCode,accValidated,is1stTime

#define MSG_LOBBY_FR_AVC_RESEND					465
#define MSG_LOBBY_FR_AVC_RESEND_REPLY			466	//6 - errCode

//lobby -> .FR client
#define MSG_CLI_FR_LOGIN_EVENT					467	//444 - event,param,gracePeriod

//PYR-15339, admin <--> lobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_CLOSE_TABLE				468	//4sI[8] - reqId,tableName,message,[tableId]
#define MSG_LOBBY_ADMIN_CLOSE_TABLE_REPLY		469	//46s - reqId,errCode,message
//if close_table was unsuccessful
#define MSG_LOBBY_ADMIN_CLEANUP_DEAD_TABLE			470	//4ss - reqId,tableName,message
#define MSG_LOBBY_ADMIN_CLEANUP_DEAD_TABLE_REPLY	471	//46s - reqId,errCode,message

//PYR-14750
#define MSG_LOBBY_USER_TOURNTICKETS2			472 // s user 
#define MSG_LOBBY_USER_TOURNTICKETS2_REPLY		473 // 6[s|4<M(4sb4tts4)>] - err_code[errDescr| numTickets,<M(ticketid, description, multitourn, price, issued, exp, currency, numTourns)>]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_TICKET_TOURNTIDS				474 // s4 userId, ticketId 
#define MSG_LOBBY_TICKET_TOURNTIDS_REPLY		475 // 6[s|44<M(444d4441111s4441s4)>] - err_code[errDescr|ticketId, numTourns,
													//		<M(tournId, tournStatus, tournFlags, startTime, numEntrants, numPlayers, buyIn, gameType, isHiLo, structure,
													//		 isPlayMoney, server, rake, knockout, fppBuyIn, isRestricted, currency, statusBits, name, tournFlags2)>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_SHOW_NO_CHAT_TOOLTIP			476 // s1 userId, show

// [AO 2010-08-19] #15772
#define	MSG_LOBBY_LEAVE_ALL_WAITING_LISTS		478 // [4] - [flags]
#define	MSG_LOBBY_LEAVE_ALL_WAITING_LISTS_REPLY	479 // 6[s|44] - errCode[errDescr|numTables,flags]

// PYR-25419
#define MSG_LOBBY_CANCEL_RESERVED_SEAT			480 // 418 - tableId32, seat, tableId64 
/* obsolete after #23241
//PYR-15751
#define	MSG_LOBBY_EE_MIGRATION_PERFORM_REPLY	481 // 6[s] - errCode[errDescr]
*/
// lobby -> client
#define MSG_REDIRECTOR_URL_ERROR_REPLY			482

// [MJ 2010-08-20: PYR-15366]
#define MSG_LOBBY_TABLE_UPDATE_PARAMETERS       483 // 4s474<7> - tableId, rake string, noVppGainSites, poolBlockMinHands, numPenalties<poolBlockTimePenalty>
#define MSG_LOBBY_TABLE_UPDATE_PARAMETERS_REPLY 484 // 6 - errCode

//PYR-16022
#define MSG_LOBBY_FIND_SIMILAR_TABLE			485 //41[448] - tableId32,minPlayers,[clientReqId,searchFlags,tableId64]
#define MSG_LOBBY_FIND_SIMILAR_TABLE_REPLY		486 //2[s44|s414412b8s] - errCode,[errDescr,clientReqId,searchFlags|server,tableId32,seat,clientReqId,searchFlags,numOfPlayersInDecrSearchTable, waitingForSeatReservationTimeout,isThereDealing,tableId64,serverObject]

// #16248
#define MSG_LOBBY_LAST_LOGIN_INFO				487 // D - lastLogin

//PYR-16022
#define MSG_LOBBY_FIND_SIMILAR_TABLE_STOP		488 //

//PYR-15847
#define MSG_LOBBY_FIND_SEAT_MANAGER_TABLE			489 //111114444b14b4<2222>[<44112>][44] - minPlayers,game,structure,maxPlayersMin,maxPlayersMax,minLoBet,minHiBet,maxLoBet,maxHiBet,capOn,speedBM,currencyBM,revertBuyIn,numElem,<bottomMin,topMin,bottomMax,topMax>,[<minAvgPot,minAvgStack,minPlayersFlop,minVpip,minHandsHour>],[clientReqId,searchFlags] - see parse of SeatManSearchCriteria,SeatManAdvSearchCriteria
#define MSG_LOBBY_FIND_SEAT_MANAGER_TABLE_REPLY		490 //2[s44|s414412b8s] - errCode,[errDescr,clientReqId,searchFlags|server,tableId32,seat,clientReqId,searchFlags,numOfPlayersInDecrSearchTable,waitingForSeatReservationTimeout,isThereDealing,tableId64,serverObject] (same as MSG_LOBBY_FIND_SIMILAR_TABLE_REPLY)

// PYR-16277
#define MSG_LOBBY_CHALLENGE_QUESTION				491 //2 - locale
#define MSG_LOBBY_CHALLENGE_QUESTION_REPLY			492 //6[s|2<4s>] - errCode, [errDesc | size, < id,question >]

// PYR-15453
#define MSG_LOBBY_OLYMPIC_SNAPSHOT					495 // 
#define MSG_LOBBY_OLYMPIC_SNAPSHOT_REPLY			496 // 6[s|4<444>]

#define MSG_LOBBY_ADMIN_GET_CHALLENGE_QA			497 //4s - requestId, userid
#define MSG_LOBBY_ADMIN_GET_CHALLENGE_QA_REPLY		498 //6[s|b2<4ss4ss>] - requestId, errCode, [errDesc | isRM, size, id 1, question1, answer1, id2, question2, answer2]

#define MSG_LOBBY_CREATE_HOME_TABLE					499 //44422ssb - reqId, typeId, clubId, disableChatModerator, alwaysAllowChatModerator, tableName, effCreator, isVoice
#define MSG_LOBBY_CREATE_HOME_TABLE_REPLY			500 //46 - reqId, errCode

#define MSG_LOBBY_KILL_HOME_TABLE					501 // 44ss - reqId, tableId, reason, effCanceller
#define MSG_LOBBY_KILL_HOME_TABLE_REPLY				502 // 446[s] - reqId, tableId, errCode, [errDesc]

// PYR-15607 - Home games
#define MSG_LOBBY_TRY_TO_DISSOLVE_HG_TABLE          503 // 4 - table id

// #16914
#define MSG_LOBBY_ADMIN_UPDATE_LANG_PATTERNS		504	// 444<s>b - reqId, locale, numPatterns<pattern>, isWhiteList
#define MSG_LOBBY_ADMIN_UPDATE_LANG_PATTERNS_REPLY	505	// 46[s] - reqId, errCode[errDescr]

// #15607 - Home games
#define MSG_LOBBY_HG_GET_USER_TOURNS_TABLES         506 // s - user id
#define MSG_LOBBY_HG_GET_USER_TOURNS_TABLES_REPLY   507 // 4<4>4<4> - #tourns, <tourn id>, #tables, <table id>

// #15607
#define MSG_LOBBY_CLI_USER_WHERE2					508 // 4<M>4<M>4<M>4<M>4<M> - numTables<tableInfo>, numTourns<tournInfo>, sizeWaitingList<listInfo>, numBlitz<blitzInfo>, numTournsAutoUnregistered<M(autoUnregisteredTournInfo)> Where tableInfo is 4ss48 - Added TableId64 to the end, listInfo is 48 - Added TableId64 to the end

// #15607
#define MSG_LOBBY_FIND_TOURN_REG2					509 //s userId
#define MSG_LOBBY_FIND_TOURN_REG2_REPLY				510 //

// PYR-17233 - messages for incorrect clients
#define MSG_LOBBY_COMPARE_SITE_FOR_COUNTRY			511 //4s siteId, country
#define MSG_LOBBY_COMPARE_SITE_FOR_COUNTRY_REPLY	512 //6s4sssss errCode, country, siteId, name, contactEmail, contactURL, downloadURL, faqURL

// PYR-17233 - messages for incorrect clients
#define MSG_LOBBY_COMPARE_SITE_FOR_IP				513 //4 siteId
#define MSG_LOBBY_COMPARE_SITE_FOR_IP_REPLY			514 //6s4sssss errCode, country, siteId, name, contactEmail, contactURL, downloadURL, faqURL

// #17840 begins
#define MSG_LOBBY_ADMIN_GET_TABLE_PAUSING_INFO			515 // 4 - reqId
#define MSG_LOBBY_ADMIN_GET_TABLE_PAUSING_INFO_REPLY	516 //46[s|4<4DD444>] - reqId, errCode[errDescr|numTables<tableId, pausingFrom, pausingTo, pausingAfterNumHands, pausingStatus, numHandsPlayed>]

#define MSG_LOBBY_ADMIN_SET_TABLE_PAUSING_INFO			517 // 48DD4 - reqId, tableId, pausingFrom, pausingTo, pausingAfterNumHands
#define MSG_LOBBY_ADMIN_SET_TABLE_PAUSING_INFO_REPLY	518 // 46[s] - reqId, errCode[errDescr]
// #17840 ends

// PYR-13934
#define MSG_LOBBY_ADMIN_SET_JUBILEE_HAND                519 // 44<488844s> - reqId, numSites<siteId, jubileeHandStops, FromHandId, ToHandId, prizeCoef, prizeIncr, currency>
#define MSG_LOBBY_ADMIN_SET_JUBILEE_HAND_REPLY          520 // 46[s] - reqId, errCode [, errDescr]

// #17783
#define MSG_LOBBY_ADMIN_GET_TABLES_BY_NAME				521 // 4s - reqId, tableName
#define MSG_LOBBY_ADMIN_GET_TABLES_BY_NAME_REPLY		522 // 46[s|s4<411111444s4>] - reqId, errCode[errDescr|tableName, numTables<tableId, game, structure, hiLo, isPlay, isOneOnOne, smallBlind, loBet, hiBet, currency, clubId>]

// #17782
#define MSG_LOBBY_WAIT_ON_TABLES						523 // (empty)
#define MSG_LOBBY_WAIT_ON_TABLES_REPLY					524 // 6[s|4<M(44ssss11bs44b44b4448)>4<M(O)>] - errCode, [errDescr | numTables<M(tableId32, clubId, server, serverObject, name, specialName, game, limit, isHiLo, currency, stakeLo, stakeHi, isPlayMoney, players, maxPlayers, isOneOnOne, waitListSize, waitListPos, scalePM, tableId64)>,numTBS<M(TableStarterCli)>]

#define MSG_LOBBY_ADMIN_TABLE_MESSAGE					525 // 44s44 - reqId, tableId, tableText, options, site
#define MSG_LOBBY_ADMIN_TABLE_MESSAGE_REPLY				526 // 46[s] - reqId, errCode[errDescr]

#define MSG_LOBBY_TIMER_CHECK_RESTART_TIMEFRAME			527 // timer message

#define MSG_LOBBY_ADMIN_GET_TABLE_ADDRESS				528 // 448 - reqId, tableId32,tableId64
#define MSG_LOBBY_ADMIN_GET_TABLE_ADDRESS_REPLY			529 // 46[s|sss] - reqId, errCode[errDescr|server, serverObject, tablePublication]

// #18024
#define MSG_LOBBY_ADMIN_CREATE_RING_GAME_REPLAY			530 // 4oo - reqId, RingGameReplayScript(parser), IniFile(parser)
#define MSG_LOBBY_ADMIN_CREATE_RING_GAME_REPLAY_REPLY   531 // 46[s|4] - q_id, errCode, [errMess|replayId]

#define MSG_LOBBY_ADMIN_CANCEL_RING_GAME_REPLAY			532 // 44 - reqId, replayId
#define MSG_LOBBY_ADMIN_CANCEL_RING_GAME_REPLAY_REPLY	533	// 46[s] - q_id, errCode, [errMess]

#define MSG_LOBBY_RING_GAME_REPLAY_CREATED				534 // 4 tableId
#define MSG_LOBBY_RING_GAME_REPLAY_CREATED_REPLY		535 //

// PYR-14242
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_VALIDATE_MOBILE                       536 //// s4 userId, unique
#define MSG_LOBBY_VALIDATE_MOBILE_REPLY                 537 //6[s] err [errDescr] 
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_RESEND_MOBILE_VALIDATION              538 // sb - userId, newCode /* PYR-30212, default true for compatibility */
#define MSG_LOBBY_RESEND_MOBILE_VALIDATION_REPLY        539 // 6[s] - errCode[errInfo]

// #18024
#define MSG_LOBBY_ADMIN_CANCEL_RING_GAME_REPLAY_BY_NAME			540 // 4s - reqId, replay name
#define MSG_LOBBY_ADMIN_CANCEL_RING_GAME_REPLAY_BY_NAME_REPLY	541	// 46[s] - q_id, errCode, [errMess]
/* obsolete by #23241
//PYR-18397
#define	MSG_LOBBY_MIGRATION_PERFORM						542 // 44 - migrFlag,migrSiteId
#define	MSG_LOBBY_MIGRATION_PERFORM_REPLY				543 // 6[s] - errCode[errDescr]
*/
// #18676
#define MSG_LOBBY_FORCE_LOGOUT							544 // empty

// #18597
#define MSG_LOBBY_SOCIAL_SHARE_HAND						545 // PsapiPublishHandRequest
#define MSG_LOBBY_SOCIAL_SHARE_HAND_REPLY				546 // 6s | ss - errCode, errDescr | translatedSocialToken, boomUrl

// #18541
#define MSG_LOBBY_ADMIN_RESET_STACKS_TABLE              547 // 444 - reqId, tableId
#define MSG_LOBBY_ADMIN_RESET_STACKS_TABLE_REPLY        548 // 46[s] - reqId, errCode[errDescr]

//PYR-18568, .BE client <--> lobby
#define MSG_LOBBY_BE_AVC_VALIDATE				549	//4 - avc
#define MSG_LOBBY_BE_AVC_VALIDATE_REPLY			550	//6 - errCode

#define MSG_LOBBY_BE_AVC_RESEND					551
#define MSG_LOBBY_BE_AVC_RESEND_REPLY			552	//6 - errCode

//lobby -> .BE client
#define MSG_CLI_BE_LOGIN_EVENT					553	//44 - event,showResend

// #18541
#define MSG_TABLE_ADMIN_RESET_STACKS			554 // 4 - numBBs
#define MSG_TABLE_ADMIN_RESET_STACKS_REPLY      555 // 6[s] - errCode, [errMsg]
#define MSG_TABLE_ADMIN_RESET_STACKS_ACK        556 // empty

// 18541
#define MSG_TABLE_ADMIN_SET_SUPERSTAR_SHOWDOWN_RESULTS	557		// s - results

// #18129
#define MSG_LOBBY_ADMIN_CHECK_DUMMY_TOURN_CHANGES		 558		// 444T4 - reqId, tournId, state, start time, max players
#define MSG_LOBBY_ADMIN_CHECK_DUMMY_TOURN_CHANGES_REPLY  559		// 46[s] - q_id, errCode, [errStr]

#define MSG_LOBBY_ADMIN_PERFORM_DUMMY_TOURN_CHANGES		   560		// 444T4 - reqId, tournId, state, start time, max players
#define MSG_LOBBY_ADMIN_PERFORM_DUMMY_TOURN_CHANGES_REPLY  561		// 46[s] - q_id, errCode, [errStr]

// PYR-18621
//lobby -> .IT client
#define MSG_CLI_IT_LOGIN_EVENT					562	//444 - event,param,gracePeriod

#define MSG_LOBBY_REPORT_CREATE_ACCOUNT_GIVE_UP	563 // s44sM - InstallIdDeprecated, Client Site Id, Client Platform, Create Account Wizard Page Name, loginExtraData
#define MSG_LOBBY_REPORT_FIRST_LOGIN			564 // s44M - installIdDeprecated, siteId, platformId, loginExtraData

//PYR-24854
#define MSG_LOBBY_FIND_SEAT_MANAGER_TABLE2			565 //111119999b14b4<2222>[<44112>][44] - minPlayers,game,structure,maxPlayersMin,maxPlayersMax,minLoBet,minHiBet,maxLoBet,maxHiBet,capOn,speedBM,currencyBM,revertBuyIn,numElem,<bottomMin,topMin,bottomMax,topMax>,[<minAvgPot,minAvgStack,minPlayersFlop,minVpip,minHandsHour>],[clientReqId,searchFlags] - see parse of SeatManSearchCriteria,SeatManAdvSearchCriteria
#define MSG_LOBBY_FIND_SEAT_MANAGER_TABLE2_REPLY	MSG_LOBBY_FIND_SEAT_MANAGER_TABLE_REPLY //2[s44|s414412b8s] - errCode,[errDescr,clientReqId,searchFlags|server,tableId32,seat,clientReqId,searchFlags,numOfPlayersInDecrSearchTable,waitingForSeatReservationTimeout,isThereDealing,tableId64,serverObject] (same as MSG_LOBBY_FIND_SIMILAR_TABLE_REPLY)

//PYR-25271
#define MSG_LOBBY_FIND_TABLE_TO_OBSERVE			566 //44 - tableTypeId,clientReqId
#define MSG_LOBBY_FIND_TABLE_TO_OBSERVE_REPLY	567 //6[s4|s448s] - errCode,[errDescr,clientReqId|server,tableId32,clientReqId,tableId64,serverObject]

//PYR-27126
#define MSG_LOBBY_HIDE_MOBILE_ICON				568 //b - hide

// can be reused
//#define MSG_LOBBY_MTLOBBY_SEND_GLOBAL_FLIGHT_INFO_REPLY			  569 // 46[s] - reqId, errCode, [errStr]
//#define MSG_LOBBY_MTLOBBY_RECEIVE_GLOBAL_FLIGHT_INFO              570 // 444 - req, FlightTournInfo(parser)
//#define MSG_LOBBY_MTLOBBY_QUERY_INIT_FLIGHT_INFO				  571 // 44 - reqId, tournid
//#define MSG_LOBBY_MTLOBBY_QUERY_INIT_FLIGHT_INFO_REPLY            572 // 46[s] - reqId, errCode, [errStr]
//#define MSG_LOBBY_MTLOBBY_SEND_INIT_FLIGHT_INFO                   573 // 4 - reqId
//#define MSG_LOBBY_MTLOBBY_SEND_TOTAL_FLIGHT_TO_NEXT_LEVEL		  574 // 44o - reqId, targetId, FlightTournInfo(parser)
//#define MSG_LOBBY_MTLOBBY_SEND_TOTAL_FLIGHT_TO_NEXT_LEVEL_REPLY   575 // 46[s] - reqId, errCode, [errStr]
//#define MSG_LOBBY_MTLOBBY_RECEIVE_TOTAL_FLIGHT_FROM_PREV_FLIGHT   576 // 4o - reqId, FlightTournInfo(parser)

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_BLITZ_ADDR				577 // 44s - reqId,blitzId,name
#define MSG_LOBBY_ADMIN_BLITZ_ADDR_REPLY		578 // 46[s|ss4] - reqId,errCode,[errDescr|server,serverObj,blitzId]

// PYR-18513
#define MSG_LOBBY_TABLE_SET_VOICE_OBJECT						579 //44sss reqId, tableId, voiceObjectAddress, voiceObjectUdpAddress, voiceObjectName
#define MSG_LOBBY_TABLE_SET_VOICE_OBJECT_REPLY					580 //

// PYR-20859
#define MSG_LOBBY_IP_CHECK										581 //

// PYR-20730 .ES,.DK,..
#define MSG_CLI_LOGIN_EVENT					582	//444b - event,param,gracePeriod

#define MSG_LOBBY_MTLOBBY_PARAM_CHANGE				583				// 44s - reqId, paramId, paramValue
#define MSG_LOBBY_MTLOBBY_PARAM_CHANGE_REPLY		584				//

// PYR-21287
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_DONT_SHOW_PERSONAL_INFO           585 // sf - userId, dontShow

// PYR-21685
#define MSG_LOBBY_GET_SELFEXCLUDED_STR				587				// 4s - reqId, originalErrorMsg
#define MSG_LOBBY_GET_SELFEXCLUDED_STR_REPLY		588				// 6s - errCode, errMessage | selfExcludedExplanation

//PYR-20433
#define MSG_CLI_LOBBY_FIND_QUICKSTART_SNG			589
#define MSG_CLI_LOBBY_FIND_QUICKSTART_SNG_REPLY		590

//PYR-21560
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_IMPROVED_BROADCAST			591	//44(4s)44b4(4)4(4b)4(4)4 - reqId,i18nString,flag,site,AllTables/Tourn_only,tourn ID,hand(HiLo),betting,stakes
#define MSG_LOBBY_ADMIN_IMPROVED_BROADCAST_REPLY	592	

//PYR-22570
#define MSG_LOBBY_GET_ACTIVE_USER_GAMES				593
#define MSG_LOBBY_GET_ACTIVE_USER_GAMES_REPLY		594

//PYR-22531
#define MSG_LOBBY_GET_USER_CAPS						595
#define MSG_LOBBY_GET_USER_CAPS_REPLY				596

//#21829
#define MSG_LOBBY_VALIDATE_NEWINSTALL				597	//4 - validationCode
#define MSG_LOBBY_VALIDATE_NEWINSTALL_REPLY			598 //6[s] - errCode, [errDesc]

//#21829
#define MSG_LOBBY_RESEND_NEWINSTALL_VALIDATIONCODE	599	// empty
#define MSG_LOBBY_RESEND_NEWINSTALL_VALIDATIONCODE_REPLY	600 //6[s] - errCode, [errDesc]

// PYR-22494
#define MSG_LOBBY_SUBMIT_STARCODE					601 //s - starCode
#define MSG_LOBBY_SUBMIT_STARCODE_REPLY				602 //6[s] errCode, [errDescr|successDescr]

// PYR-20055
#define MSG_LOBBY_LOGOUT_CONFIRM					603 //empty

// PYR-20052
#define MSG_LOBBY_DISABLE_SNG_AUTO_UNREG			606 // b - disableSnGAutoUnreg

// PYR-20412
#define MSG_LOBBY_FIND_SIMILAR_TABLE_BY_TYPE        607

// PYR-25096 - get targeted news message (Mobile Client)
#define MSG_LOBBY_TARGET_NEWS_MOBILE				608 // 44 - client download source, productMask

// PYR-25096 - get targeted news message (Desktop Client)
#define MSG_LOBBY_TARGET_NEWS_DESKTOP				609	// b4 - onlyTargetBanner targetnewType /* eTargetNewsType */

// from lobby to regulator PYR-25597
#define LOBBY_TO_REGULATOR_REPORT_ARJEL_OFFLINE_HANDS        610 // 4sss4<8> - reqId,userId,userIp,userHash,numHands<handId64>
#define LOBBY_TO_REGULATOR_REPORT_ARJEL_OFFLINE_HANDS_REPLY  611 // 6[s]

#define LOBBY_TO_REGULATOR_GET_ARJEL_EVENTS			612 // 4s4tt4 - reqId,userId,userIntId,startT,entT,locale
#define LOBBY_TO_REGULATOR_GET_ARJEL_EVENTS_REPLY	613 // 6[s|m(4<8>)4m(4<m()>)m(4<m()>)] (<handIds>),cliArjelEvtFlg,(<cliArjelEvt>),(<arjelEvt>)

// PYR-25636 - Integration of Jumio (Netverify)
#define MSG_LOBBY_INIT_NETVERIFY_SESSION			614 // s - redirector url
#define MSG_LOBBY_INIT_NETVERIFY_SESSION_REPLY		615 // 6[s|s] - errCode, [errDescr | url]

//	PYR-25263
#define MSG_LOBBY_FIND_PLAYER3					616 //s playername
#define MSG_LOBBY_FIND_PLAYER3_REPLY			617 //6[s|s4<>4<>4<>]		errCode[errDescr|playername,4<table data>,4<tourns data>,4<blitz data>] Note table data is ss4ssb4411bs48 - Added TableId64 to the end

//	PYR-25263
#define MSG_WEB_FIND_PLAYER3					618 //s playername
#define MSG_WEB_FIND_PLAYER3_REPLY				619 //66[s|s4<>4<>4<>]		errCode[errDescr|playername,4<table data>,4<tourns data>,4<blitz data>] Note table data is ss4ssb4411bs48 - Added TableId64 to the end

// PYR-27244 cesar to lobby
#define MSG_LOBBY_ADMIN_TRY_SAY_TO_USER			620 //4ss reqId userId messageText
#define MSG_LOBBY_ADMIN_TRY_SAY_TO_USER_REPLY	621 //46[s] reqId, err_code, [errDescr]

//	PYR-27579, Client to Lobby.  Responsible Gaming History Audit Report.
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_RESPONSIBLE_GAMING_HISTORY_AUDIT			622 //	sdds4 - userId, startDate, endDate, password, eReportFormat
#define MSG_LOBBY_RESPONSIBLE_GAMING_HISTORY_AUDIT_REPLY	623	//	6[s] - errCode[errDescr]

//PYR-27249 - Responsible gaming - NJ
#define MSG_LOBBY_GET_BUYIN_LIMIT_EX			624 //
#define MSG_LOBBY_GET_BUYIN_LIMIT_EX_REPLY		625 //6[s|6<{44}>] - errCode,[errDescr|numLimits,<{limitType,amount}>]
#define MSG_LOBBY_SET_BUYIN_LIMIT_EX			626 //6<{44}> - numLimits,<{limitType,amount}>
#define MSG_LOBBY_SET_BUYIN_LIMIT_EX_REPLY		627 //6[s|6<{46[s|4b]}>] - errCode,[errDescr|numLimits,<{limitType,errCode,[errDescr|amount,effectiveNow}>]]

// PYR-27257
#define MSG_LOBBY_FORCE_DELAYED_LOGOUT			628 // s userId
#define LOBBY_FORCE_DELAYED_LOGOUT_DELAY		(60 * 1000) // 1 min

// PYR-26639
#define MSG_LOBBY_TSS_CLOSE_ACCOUNT_ACTION			    630
#define MSG_LOBBY_TSS_CLOSE_ACCOUNT_ACTION_REPLY		631

//	PYR-28550
#define MSG_LOBBY_TIMER_REPORT_PERFORMANCE	632

// PYR-27251
#define MSG_LOBBY_ADMIN_FACTORY_DOWN		634 //44<4> reqId, <factoryIds>
#define MSG_LOBBY_ADMIN_FACTORY_DOWN_REPLY	635 //46[s|4<46s>] reqId, err_code, [errDescr|<factoryId, factoryErrCode, factoryErrDesc>]

#define MSG_LOBBY_ADMIN_RESURRECT_FACTORY		636 //444444<4> reqId, objMask, tournFlags, tableFlags, siteId, <factoryIds>
#define MSG_LOBBY_ADMIN_RESURRECT_FACTORY_REPLY	637 //46[s|4<46s>] reqId, err_code, [errDescr|<factoryId, factoryErrCode, factoryErrDesc>]
// end of PYR-27251

//PYR-25702
#define MSG_LOBBY_FIND_REGISTERING_TOURN_BYREF			638	//s - internalRef
#define MSG_LOBBY_FIND_REGISTERING_TOURN_BYREF_REPLY	639	//6[s|4s] - errCode,[errDescr|tournId,tournServer]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_RESOLVER_TAG					640 // s4s4M(ssssss444[444s4444ss])s[b4tM(4<M(ss)>)s4] - tag, siteId, country, locale, clientExtraInfoBody(macAddr, volumeLabel, installId, machineGuid, adapterGuid, userExtraId, clientSiteId, clientPlatform, clientExtraInfoMask, [clientType, clientDevice, clientVersion, routerMacAddr, clientBrand, osId, appTypeId, productId, installId2, operatorId]), urlParams, [isForceTest, userIntId, appSrvTimeUtc, dynParamsBody(numDynParams<M(from, to)>), state, license]
#define MSG_RESOLVER_TAG_REPLY				641 // 6s - errCode,[errDescr|url]

// PYR-28735
#define MSG_COL_LOBBY_PROCESS_REAUTH						642		// 4s		reqId, userId
#define MSG_COL_LOBBY_PROCESS_REAUTH_REPLY					643		// 42[s|4]	reqId, errCode, [descr|status]
#define MSG_COL_LOBBY_TURN_OFF_LIMITED_LOGIN_MODE			644		// 4s		reqId, userId
#define MSG_COL_LOBBY_TURN_OFF_LIMITED_LOGIN_MODE_REPLY		645		// 42[s|4]	reqId, errCode, [descr|status]
// AVAIL 646

// trusted admin -> ip2country
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_RESOLVER_GET_VERSION			647 // 4 - reqId
#define MSG_RESOLVER_GET_VERSION_REPLY		648 // 46s - reqId, err_code, [version | errDescr ]

// unauth admin -> ip2country PYR-31213
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_RESOLVER_REREAD					649 // 4 - reqId
#define MSG_RESOLVER_REREAD_REPLY			650 // 46s - reqId, err_code, [version | errDescr ]

// from lobby to regulator PYR-30474
// PYR-46515 obsolete - comment out once not used by admin
#define LOBBY_TO_REGULATOR_GROUP_SELFEXCLUSION        651 // 4s4 - reqId,userId,days
#define LOBBY_TO_REGULATOR_GROUP_SELFEXCLUSION_REPLY  652 // 42[s] - reqId,errCode,[errDescr]

// from lobby to client  PYR-29932
#define MSG_LOBBY_CLI_NOTIF_EMAIL_VALIDATION_REQUIRED 653 // s email

#define MSG_LOBBY_COLLUSION_SELF_LOCKING				655 // see SecMsgForLobby // PYR-34643
#define MSG_LOBBY_COLLUSION_SELF_LOCKING_REPLY			656 // 42[s]

// PYR-36220
#define MSG_LOBBY_REPLAY_CONFIG					657 //4 replayId
#define MSG_LOBBY_REPLAY_CONFIG_REPLY			658 //

// PYR-39290
// !!! WARNING !!! If you modify this message, please let test automation team know.
#define MSG_RESOLVER_TAG_SRV					659 // 4ss4s4Ms[b4tsM(4<M(ss)>)s4] - reqId, ipAddr, tag, siteId, country, locale, clientExtraInfo, urlParams, [isForceTest, userIntId, appSrvTimeUtc, sid, M(numDynParams<M(from, to)>), state, licenseId]
#define MSG_RESOLVER_TAG_SRV_REPLY				660 // 46s - reqId, errCode, [errDescr|url]

// PYR-38484 
#define MSG_RESOLVER_GET_ISPS				661 // 44<s> - reqId, numOfItems<ipAddress>
#define MSG_RESOLVER_GET_ISPS_REPLY			662 // 46[s4<s>] - reqId, err_code, [errDescr, numOfItems<ISP>]

// PYR-44930
#define MSG_LOBBY_TIMER_BROADCAST_MSG			663 // 4T - broadcastId, broadcastTime

// PYR-47234
#define MSG_LOBBY_TABLE_ITSESSION_EXPIRED		664  // 8 - tableId
#define MSG_LOBBY_TABLE_ITSESSION_EXPIRED_REPLY	665  // empty msg

//PYR-44908
#define MSG_LOBBY_ADMIN_NEW_SITANDGO2			666 // 44ss4<IniFile> reqId, segment. title, fileName, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_NEW_SITANDGO2_REPLY		667 //reqId, 6[s] errCode, [errDescr]

#define MSG_LOBBY_ADMIN_UPDATE_SITANDGO2		668 // 44ss4<IniFile> reqId, segment, title, fileName, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_UPDATE_SITANDGO2_REPLY	669 //reqId, 6[s] errCode, [errDescr]

//PYR-31663
#define MSG_LOBBY_TOURN_AUTO_REPLAY_DONE		670 // 44 - reqId, replayId
#define MSG_LOBBY_TOURN_AUTO_REPLAY_DONE_REPLY	671 // 46[s] - reqId, errCode, [errDescr]

#define MSG_LOBBY_ADMIN_DELETE_SITANDGO2		672 // 44s reqId, segment, title
#define MSG_LOBBY_ADMIN_DELETE_SITANDGO2_REPLY	673 //reqId, 6[s] errCode, [errDescr]

// PYR-49849: lobby to lobbyetc
#define MSG_LOBBY_TO_LOBBYETC_FORCECLIENTLOGOUT			674 // 4s reqId, userId
#define MSG_LOBBY_TO_LOBBYETC_FORCECLIENTLOGOUT_REPLY	675 // 46[s] reqId, errCode, [errDescr]

//	PYR-51670: lobby to tables
#define MSG_TABLE_CVL_USER_BAR_UPDATE					676		//	4s4 - reqId, userId, targetRewardPoints

#define MSG_LOBBY_ADMIN_MSG_CHECKER_RELOAD				677 //44 reqId, moduleType  (see eMsgCheckerModuleType)
#define MSG_LOBBY_ADMIN_MSG_CHECKER_RELOAD_REPLY		678 //46[s] reqId, err_code, [errDescr]

// PYR-52524
#define MSG_LOBBY_TIMER_FLUSH_PROPS_TO_GLOBALPUBLISHER	679 //empty

// PYR-53191 lobby to tables and blitzLobby
#define MSG_LOBBY_DISCONNECT_CONFIG						680 // 4b44 reqId, detectLobbyDisconnect, lobbyDisconnectTimeout, lobbyDisconnectWarning

// PYR-51032
#define MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT_WITHUSERS2		681 // 4<8> - numTournIds<tournIds>
#define MSG_LOBBY_ADMIN_CANCEL_TOURNAMENT_WITHUSERS2_REPLY	682 // 24<8>4<8> - errCode,numSuccessTournIds<successTournIds>,numFailedTournIds<failedTournIds>

// PYR-69655 lobby to ring tables and blitzLobby who forwards it to its blitz tables
#define MSG_TABLE_TOGGLE_LOGMONITOR_FORWARDINGENABLED				683 // 4b reqId, enableLogMonitorAggregation

// PYR-76936
#define MSG_LOBBY_ADMIN_GET_REPLAYS						684 // 4 - reqId
#define MSG_LOBBY_ADMIN_GET_REPLAYS_REPLY				685 // 46[s|4sss4444444Ts4s4<s4<ss>>] - reqId, err_code, [errDescr|replayId, replayName, replayDescr, replayFileName, tournMask, tournMask2, tournMask3, tournMask4, tournMask5, tournMask6, tournSites, startTime, currency, brandAccessMask, visibilityCriteria, iniFileSize<sectionName<itemsSize<itemName, itemValue>>>]
#define MSG_LOBBY_ADMIN_REDEPLOY_REPLAY					686 // 44 - reqId, replayId 
#define MSG_LOBBY_ADMIN_REDEPLOY_REPLAY_REPLY			687 // 46[s] - reqId, err_code, [errDescr]

// PYR-103380
#define MSG_USER_LEFT_WAIT_LIST							688 // see table/tableserverprot.txt

// PYR-102835
#define MSG_LOBBY_ADMIN_KILL_TOURN						689 //48 reqId, tournId
#define MSG_LOBBY_ADMIN_KILL_TOURN_REPLY				690 //46[s]8 reqId, err_code [errDescr],tournId
#define MSG_LOBBY_ADMIN_START_KILLED_TOURN				691 //48 reqId, tournId
#define MSG_LOBBY_ADMIN_START_KILLED_TOURN_REPLY		692 //46[s]8 reqId, err_code [errDescr],tournId
#define MSG_LOBBY_TIMER_CREATETOURN_AFTER_KILL			693 // 8, tournId

#define MSG_LOBBY_ADMIN_KILL_TOURN_BATCH				694 //44<8> reqId, numTurn<tournId>
#define MSG_LOBBY_ADMIN_KILL_TOURN_BATCH_REPLY			695 //46[s|4<86[s]>] reqId, err_code [errDescr|numTourn<tournId,errCode[errDesc]>]
#define MSG_LOBBY_ADMIN_RESTORE_KILLED_TOURN_BATCH		696 //44<8> reqId, numTurn<tournId>
#define MSG_LOBBY_ADMIN_RESTORE_KILLED_TOURN_BATCH_REPLY	697 //46[s|4<86[s]>] reqId, err_code [errDescr|numTourn<tournId,errCode[errDesc]>]

////////////////////////////////////////////////////////////////////////////////

//constants of 'type' in MSG_LOBBY_SEND_VC_TO_CLIENT
#define MONET_LOBBY_VC_TYPE_REQUEST_VC			0
#define MONET_LOBBY_VC_TYPE_REQUEST_STATE		0x10000

//constants of 'style' in MSG_LOBBY_SEND_VC_TO_CLIENT // for type == 0: MONET_LOBBY_VC_TYPE_REQUEST_VC
#define MONET_LOBBY_VC_STYLE_VC_CSR_INIT		1
#define MONET_LOBBY_VC_STYLE_VC_USER_LOGIN		2
#define MONET_LOBBY_VC_STYLE_VC_USER_FAIL		3
#define MONET_LOBBY_VC_STYLE_VC_CLIENT_REQUEST	4

//constants of 'type' in MSG_LOBBY_RECEIVE_VC_FROM_CLIENT
#define MONET_CLIENT_VC_TYPE_USER_INPUT				0
#define MONET_CLIENT_VC_TYPE_MESSAGE_RECEIVED		1
#define MONET_CLIENT_VC_TYPE_DLG_SHOWN				2
#define MONET_CLIENT_VC_TYPE_INFORMATION_RECEIVED	4
#define MONET_CLIENT_VC_TYPE_FAIL_IN_SHOWN_VC		0x10
#define MONET_CLIENT_VC_TYPE_INFO					0x1000
#define MONET_CLIENT_VC_TYPE_USER_STATE				0x10000
#define MONET_CLIENT_VC_TYPE_SUBSCRIBE				0x100000 // make it obsolete (change to 0x400000)
#define MONET_CLIENT_VC_TYPE_MACHINE_INFO			0x200000
#define MONET_CLIENT_VC_TYPE_SUBSCRIBE_NEW			0x400000 // report only open time, ignore close, if same table open twice, then count as 2 tables. as requested by Jeff Aug12,2010

//constants of 'type' in MSG_LOBBY_TELL_MONET_INFO
#define MONET_LOBBY_INFO_MONET_INI		1
#define MONET_LOBBY_INFO_IPADDR_MAC		2

//constants of 'answer' in MSG_WAITING_INVITED_REPLY
#define USER_INVITATION_ANSWER_NO		0
#define USER_INVITATION_ANSWER_YES		1

//// constants used in MSG_LOBBY_PLAYER_LEFT_REPLY ////////////////////////
#define REPLY_TABLE_DIE			0 // 1 table must be destroyed. nothing follows
#define REPLY_TABLE_VACANT		1 // 1 no more users in waiting list
#define REPLY_TABLE_RESERVE		2 // 14 this seat to be reserved uniqueKey follows

///////////////////// constants used in MSG_LOBBY_WAITING_LIST //////////
#define LIST_TYPE_TABLE_ID		0 //this particular table
#define LIST_TYPE_TABLE_TYPE	1 //any table of this type

///////////////////// constants used in MSG_LOBBY_ADMIN_STOP_ALLTOURNAMENTS
#define ALLTOURNAMENTS_NORMAL		0x001
#define ALLTOURNAMENTS_REGENERATED	0x002
#define ALLTOURNAMENTS_PM			0x004 // [AO 2010-07-05] #15232
#define ALLTOURNAMENTS_RM			0x008 // [AO 2010-07-05] #15232
#define ALLTOURNAMENTS_INCLUSIVE	0x010 // [AO 2009-02-06] bug #10387
#define ALLTOURNAMENTS_HG			0x020 // [AN 2010-03-14] bug #17774
#define ALLTOURNAMENTS_SAG			0x040 // [SZ 2014-05-02] bug #28752
#define ALLTOURNAMENTS_STORMLOBBY   0x080 // PYR-44908
#define ALLTOURNAMENTS_SAGMAX		0x100 // PYR-53090
#define ALLTOURNAMENTS_PROGRESSIVEPLUS 0x200 // PYR-109327


// [AO 2010-07-05] #15232 - for broadcasting to all started tournaments
#define ALLTOURNAMENTS_ALL			(ALLTOURNAMENTS_NORMAL | ALLTOURNAMENTS_REGENERATED | ALLTOURNAMENTS_PM | ALLTOURNAMENTS_RM | ALLTOURNAMENTS_HG | ALLTOURNAMENTS_SAG | ALLTOURNAMENTS_SAGMAX | ALLTOURNAMENTS_PROGRESSIVEPLUS)

///////////////////// constants used in MSG_LOBBY_ADMIN_STOP_TOURNREGEN
#define STOP_TOURNREGEN_KILL		0x01
#define STOP_TOURNREGEN_PM			0x02
#define STOP_TOURNREGEN_RM			0x04
#define STOP_TOURNREGEN_INCLUSIVE	0x10 // [AO 2009-02-06] bug #10387

///////////////////// constants used in MSG_LOBBY_ADMIN_RESUME_TOURNREGEN
#define RESUME_TOURNREGEN_PM		0x02
#define RESUME_TOURNREGEN_RM		0x04
#define RESUME_TOURNREGEN_INCLUSIVE	0x10 // [AO 2009-02-06] bug #10387

///////////////////// constants used in MSG_LOBBY_ADMIN_SAG_STOP_REGISTRATION
#define STOP_SAGTOURNREG_RESERVED	0x01 // not currently in use
#define STOP_SAGTOURNREG_PM			0x02 // does not include sagmax tournaments
#define STOP_SAGTOURNREG_RM			0x04 // does not include sagmax tournaments
#define STOP_SAGTOURNREG_INCLUSIVE	0x10 // [AO 2009-02-06] bug #10387
#define STOP_SAGTOURNREG_MAX_PM		0x20 // PYR-53090 sagmax tournaments
#define STOP_SAGTOURNREG_MAX_RM		0x40 // PYR-53090 sagmax tournaments

///////////////////// constants used in MSG_LOBBY_ADMIN_SAG_RESUME_REGISTRATION
#define RESUME_SAGTOURNREG_RESERVED		0x01 // not currently in use
#define RESUME_SAGTOURNREG_PM			0x02 // does not include sagmax tournaments
#define RESUME_SAGTOURNREG_RM			0x04 // does not include sagmax tournaments
#define RESUME_SAGTOURNREG_INCLUSIVE	0x10 // [AO 2009-02-06] bug #10387
#define RESUME_SAGTOURNREG_MAX_PM		0x20 // PYR-53090 sagmax tournaments
#define RESUME_SAGTOURNREG_MAX_RM		0x40 //PYR-53090  sagmax tournaments

////////// status for MSG_LOBBY_IMAGE_INFO_REPLY
#define IMAGE_INFO_PUBLISHED 1
#define IMAGE_INFO_WAIT 2
#define IMAGE_INFO_WRONG_ID 3

/////////////////////////////////////////////////

#define MTL_LOBBY_TOURNAMENT_CREATED		1000 //4sss4 tournId, server, serverObj, publisherName, 
#define MTL_LOBBY_TOURNAMENT_CREATED_REPLY	1001 //4 req_id (GConnection!)

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_REGISTER_USER				1002// // s444s44414sss444sbs884 - userName, buyInRM, buyInPlay, buyInFpp, tournPwd, cameFromTourn, buyInT, buyInW, useTicket, userIntId, country, province, ipAtReg, flags, originalIntId, creditAmount, creditCurrency, creditTMoney, creditComment, flags2, privileges2, brandId
#define MTL_LOBBY_REGISTER_USER_REPLY		1003// 6[s] err_code [errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_UNREGISTER_USER			1004// 4sbs1 - tournId, userName, debitPlayerIfDummy, extraComment, unregType
#define MTL_LOBBY_UNREGISTER_USER_REPLY		1005// 6s[44444ss8] err_code, errDescr [refund, refundPlay, refundFpp,refundTChips,refundWChips,tournamentCurrency,refundCurrency,refundPlay64]

#define MTL_LOBBY_TABLE_CREATED				1006	//4 tableId
//#define MTL_LOBBY_TABLE_CREATED_REPLY		1007	// empty. never send?

#define MTL_LOBBY_TABLE_SIT_USERS			1008	//4<s14ss4144> num_users<userName, pos, chips, city, email, imageId, sitIn, timeBank, effectivePrivileges>
#define MTL_LOBBY_TABLE_SIT_USERS_REPLY		1009	//86[s|1<s>] tableId, err_code [errDescr|numUsers<userId>]

#define MTL_LOBBY_TABLE_NEXT_HAND			1010	//41144414414112 tableId, gameType, isHiLo, loBet hiBet smallBlind, structure bringin ante, handBits, blindsLevel, roundLevel, enrolled
#define MTL_LOBBY_TABLE_NEXT_HAND_REPLY		1011	//46[s|1<11>] tableId, err_code [errDescr|numPairs<bytePosition, byteCode>]

#define MTL_LOBBY_TABLE_KICK_USERS			1012	//4b1<144s4244ss4[rec]b[Is]b4t> - lobbyReqId,timedTournament,numVecUsers<seat,placeI,placeII,str_popup,msgServiceBitMask,isFpp,award,tournId,ticketAdmission,socialToken,nextRecurringTournId[ nextSchedTourn ],flightWinnersToBeAdvanced[nextFlightTournName,nextFlightTargetServer],flightLosersToBeAdvanced,bounty,reEntryDeadline>
#define MTL_LOBBY_TABLE_KICK_USERS_REPLY	1013	//446[s|1<s14>] kickRequestType, tableId, err_code [errDescr|numUsers<userId, IsSitIn, timeBank>]

#define MTL_LOBBY_TABLE_DISSOLVE			1014	//8 tableId
#define MTL_LOBBY_TABLE_DISSOLVE_REPLY		1015	//86[s] tableId, err_code [errDescr]

#define MTL_LOBBY_WHERE_IS_PLAYER			1016	//s userName
#define MTL_LOBBY_WHERE_IS_PLAYER_REPLY		1017	//6[s|sss|4] err_code(see constants below) [err_descr|server, servarAlt, servObj|place ]

#define MTL_LOBBY_TOURNAMENT_STATUS			1018	//44 tournId, newStatus
#define MTL_LOBBY_TOURNAMENT_STATUS_REPLY	1019	//6[s] err_code [errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_TOURNAMENT_USER_STATS			1020	//s userId
#define MTL_LOBBY_TOURNAMENT_USER_STATS_REPLY	1021	//6[s|44ss4sss14S4] - err_code,[errDescr|place,actPlayers,tableName,userName,city,chips,tableServer,tableObject,country,showCity,fromFlightId,fromFlightName,collectedBounty]

//mt lobby to table:
//#define MTL_LOBBY_BREAK_SCHEDULED				1022	//empty
//#define MTL_LOBBY_BREAK_SCHEDULED_REPLY			1023	//never sent ?

#define MTL_LOBBY_BREAK_STARTED					1024	//4s duration(seconds), adminMessage
#define MTL_LOBBY_BREAK_STARTED_REPLY			1025	//never sent ?

// from admin to mt lobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_ADMIN_TOURN_MESSAGE_TEXT			1026 //4s reqId, messageBoardText
#define MTL_LOBBY_ADMIN_TOURN_MESSAGE_TEXT_REPLY	1027 //46 reqId, err_code

// from admin to mt lobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_ADMIN_STOP_TOURNAMENT				1028 // 4s<4s>4 reqId, adminMessage, i18nAdminMessages, allowPausedRegistration
#define MTL_LOBBY_ADMIN_STOP_TOURNAMENT_REPLY		1029 // 46[s] reqId, err [errDescr] 

// from admin to mt lobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_ADMIN_RESUME_TOURNAMENT			1030 // 4 reqId
#define MTL_LOBBY_ADMIN_RESUME_TOURNAMENT_REPLY		1031 // 46[s] reqId, err [errDescr] 

// from mt lobby to lobby - new users registered as satellite winners
#define MTL_LOBBY_HAVE_SATELLITE_WINNERS			1034 //444<4> reqId, targetTournId, numWinners<userIntId>
#define MTL_LOBBY_HAVE_SATELLITE_WINNERS_REPLY		1035 //6s

// from lobby to mt lobby - here are new users registered as satellite winners
#define MTL_LOBBY_TAKE_SATELLITE_WINNERS			1036 // 444<4> 0, satelliteTournId, numWinners<userIntId>
//#define MTL_LOBBY_TAKE_SATELLITE_WINNERS_REPLY		1037 //

//lobby to mtLobby
#define MTL_TOURN_ADMIN_BROADCAST_PRIVILEGES		1038

//admin to mtlobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_BROADCAST_TOURN				1040 //4s4[4<4s>] reqId, messageTex, flags [18n messageText]
#define MTL_ADMIN_BROADCAST_TOURN_REPLY			1041 //46[s] reqId, err_code [errDescr]

#define MTL_LOBBY_BREAK_SCHEDULED_INFO				1042 //empty
#define MTL_LOBBY_BREAK_SCHEDULED_INFO_REPLY		1043 //never sent

//admin to mtlobby
#define MTL_LOBBY_ADMIN_RESET_ALLINS			1044 //4s reqId, userid. (If userid = '', then for all users)
#define MTL_LOBBY_ADMIN_RESET_ALLINS_REPLY		1045 //46[s] reqId, err_code [errDescr]

//mtlobby to mttables - set allIns for user(s)
#define MTL_TABLE_RESET_ALLINS					1046 //s2 userid. allIns (If userid = '', then for all users)
#define MTL_TABLE_RESET_ALLINS_REPLY			1047 //never sent

// from mtlobby to table
#define MTL_LOBBY_TABLE_HANDBYHAND_BUBBLE		1048 //1 startOrEnd (1 = start 0 = end)

// from mtlobby to lobby 
#define MTL_LOBBY_STATISTIC_DISCONNECTS         1049 // 4 tourId, numPlayers,numDelayed, numDisconnected

// from lobby to mtlobby - message to one user
//#define MTL_TOURN_ADMIN_BROADCAST_SAY			1050 //4ss sayRequestId, userId, messageText
//#define MTL_TOURN_ADMIN_BROADCAST_SAY_REPLY		1051 //4 sayRequestId

// from lobby/mtlobby to table - message to one user
//#define MSG_TABLE_SAY_USER						1052 //ss userId messageText
//#define MSG_TABLE_SAY_USER_REPLY				1053 //

// admin to MtLobby - cancel tourn before seating started
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_CANCEL_TOURN					1054 //4 reqId
#define MTL_ADMIN_CANCEL_TOURN_REPLY			1055 //46[s] reqId, err_code [errDescr]

#define MTL_LOBBY_START_UPDATED					1056 // 4t tournId, StartTime
#define MTL_LOBBY_START_UPDATED_REPLY			1057 // 4 reqId

//from unauthorized client connection to MtLobby
//#define MTL_LOBBY_GET_USER_INFO					1026	// s userId
//#define MTL_LOBBY_GET_USER_INFO_REPLY			1027	// sss44 userId, city, tableName, chpis, place

#define MTL_LOBBY_NOTIFY_USERS					1058 // 4s44< s8 > reqId, msg, nUsers < userId, pppEntryId >
#define MTL_LOBBY_TOURNAMENT_DEALING_STATUS     1059 // 441 reqId,tournId,dealingState

#define MTL_LOBBY_OLYMPIC_WAIT					1060
#define MTL_LOBBY_OLYMPIC_WAIT_REPLY			1061

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_MASS_REGISTER					1062 // 4442[s] reqId, tournId, brandId, numPlayers [userNames]
#define MTL_ADMIN_MASS_REGISTER_REPLY			1063 // 6[4<s6s>|s] err_code [numRejectedPrePegUsers< userId, userErrCode, userErrDesc> |errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_MASS_UNREGISTER				1064 // 442[s] reqId, tournId, numPlayers [userNames]
#define MTL_ADMIN_MASS_UNREGISTER_REPLY			1065 // 6[s] err_code [errDescr]

#define MTL_LOBBY_REGISTER_USER_NOTIFY			1066 // 4414[s] reqId, tournId, registered, numPlayers, [userNames], 
#define MTL_LOBBY_REGISTER_USER_NOTIFY_REPLY	1067

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_ADMIN_OBSERVER_CHAT			1068 // 41 reqId, on/off
#define MTL_LOBBY_ADMIN_OBSERVER_CHAT_REPLY		1069 // 46[s] reqId, err_code [errDescr]

//mtlobby to mttables - observers chat on/off
#define MTL_TABLE_OBSERVER_CHAT					1070 // 1 (chat turn: 0-off 1-on )
#define MTL_TABLE_OBSERVER_CHAT_REPLY			1071 // never sent

#define MTL_LOBBY_KICK_USER_NOTIFY				1072 // 444[s] reqId, tournId, nPlayers [userNames]
#define MTL_LOBBY_KICK_USER_NOTIFY_REPLY		1073

#define MTL_TOURN_ADMIN_BROADCAST_MULTI_PRIVILEGES 1074

#define MTL_REGISTER_DELAYED_NOTIFY				1075 // 1
#define MTL_TABLE_REBUY_ADDON_MANAGE			1076 // 1 RebuyAddonManagement enum 

// lobby to mtlobby
#define MTL_DIE_IMMEDIATELY						1077 //4 tournid
#define MTL_DIE_IMMEDIATELY_REPLY				1078 //?

//#define MTL_TABLE_REBUY_ADDONE_MANAGE_REPLY		1077 // never sent
#define MTL_LOBBY_TABLE_DIE						1079 // 87 tableId, delayInSec
//#define MTL_LOBBY_TABLE_DIE_REPLY				1080 //

#define MTL_LOBBY_NOTIFY_USERS_REPLY			1081 // 4
#define MTL_LOBBY_TOURNAMENT_DEALING_STATUS_REPLY     1082 // 4
#define MTL_LOBBY_STATISTIC_DISCONNECTS_REPLY         1083 // 4
#define MTL_LOBBY_REPLY_REPLY					1084 //4

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_CANCEL_RUNNING_TOURN			1085 // 444sf reqId, tournId, rollback/rollforward formula, message, forced
#define MTL_ADMIN_CANCEL_RUNNING_TOURN_REPLY	1086 //46[s] reqId, err_code [errDescr]

#define MTL_LOBBY_ADMIN_CANCEL_RUNNING_TOURN	1087 // 4sss4 formula, text, cancellerId, cancellerIdToPublish, massRollToken

#define MSG_TABLE_MTLOBBY_ISPLAYERINTOURN		1088 // s4   user, connId 
#define MSG_TABLE_MTLOBBY_ISPLAYERINTOURN_REPLY	1089 // s41  user, connId, state

#define MTL_TOURN_IMAGES_BROADCAST				1090  //1 1 = on 0 = off 

#define MTL_ADMIN_GET_TOURN_INFO				1091 //4 reqId
#define MTL_ADMIN_GET_TOURN_INFO_REPLY			1092 //46[s|414<4>4<4s44>1444] reqId err_code[ errorStr | status, break, numAwards <awardAmount> numPlayers <place, userId, award, chips> dealExclusionType, dealExclusion, remainingTickets, targetBuyIn

#define MTL_LOBBY_REGISTER_USER_QUERY			1093
#define MTL_LOBBY_REGISTER_USER_QUERY_REPLY		1094

#define MTL_TOURN_ADMIN_BROADCAST2				1095 //s44 broadcastText, flags, site
#define MTL_TOURN_ADMIN_BROADCAST2_REPLY		1096 // 6 err_code

#define MTL_SAT_TARGET_SERVER					1097
#define MTL_SAT_TARGET_SERVER_REPLY				1098

#define MTL_ADMIN_SET_TOURN_INFO				1099 //4 reqId
#define MTL_ADMIN_SET_TOURN_INFO_REPLY			1100 //46[s|]

#define MTL_LOBBY_FIND_TABLE					1101
#define MTL_LOBBY_FIND_TABLE_REPLY				1102

#define MTL_LOBBY_TABLE_MESSAGE					1103
#define MTL_LOBBY_TABLE_MESSAGE_REPLY			1104

#define MSG_LOBBY_GET_NEWS						1105 // 4sss4848444444<448>d44s reqId,userId,country,state,flags,flags2,privileges,privileges2,siteId,fpp,ltFpp,vipStatus,locale,emLocale,depositMask,lastLogin,mvpp,yvpp,email
#define MSG_LOBBY_GET_NEWS_REPLY				1106 // 44<4s> reqId nSubscr <subscrRevision subscrName>

#define MSG_LOBBY_GET_CONSENTS					1107 // s userId
#define MSG_LOBBY_GET_CONSENTS_REPLY			1108 // 6[s | 4<MSG(44ss)>] - errCode[errDescr | consent Number <consentId, status, title, text>]

#define MSG_LOBBY_SAVE_CONSENT					1109 // s44 usreId, consentId, status
#define MSG_LOBBY_SAVE_CONSENT_REPLY			1110 // 6[s|]

#define MTL_ADMIN_STOP_REGISTRATION				1113 // 441 reqId, tournId, action
#define MTL_ADMIN_STOP_REGISTRATION_REPLY		1114 //

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_REGISTER_TOURN_USER			1115 //
#define MSG_LOBBY_REGISTER_TOURN_USER_REPLY		1116 //6[s|2s4bss] errCode [ errDescr|allIn,text,tournId,seatAvail,srv,socialToken]

#define MTL_LOBBY_REGISTER_USER_REPLY_REPLY		1117

#define MTL_ADMIN_SHUFFLE_USERS					1118 // 44 reqId, tournId
#define MTL_ADMIN_SHUFFLE_USERS_REPLY			1119

#define MTL_LOBBY_DAY_TWO						1120 // 444T reqId, tournId, time_t, Time
#define MTL_LOBBY_DAY_TWO_REPLY					1121

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_SET_DAY2_TIME					1122 // 44T reqId, tournId, Time
#define MTL_ADMIN_SET_DAY2_TIME_REPLY			1123

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_SET_DAY2_TEXT					1124 // 44 i18nPString reqId, tournId, text
#define MTL_ADMIN_SET_DAY2_TEXT_REPLY			1125

#define MTL_LOBBY_RESET_DEALER_POS				1126 //
#define MTL_LOBBY_RESET_DEALER_POS_REPLY		1127

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_GET_TABLE_LIMITS				1128
#define MSG_LOBBY_GET_TABLE_LIMITS_REPLY		1129 // 6[s|74<B>4<B>444]   errCode [errDescr|limit_obsolete numLimits<oneLimitBody> numBalances<oneBalanceBody> autoLimitHour waitingIncreaseNLLimitDays waitingIncreaseFLLimitDays]
//oneLimitBody = 7sb limit currency limitType (see TABLE_LIMIT_* in "dbm.h")
//oneBalanceBody = s currency
/* removed after BUILD1042 PYR-31675
#define MSG_LOBBY_SET_TABLE_LIMITS				1130
#define MSG_LOBBY_SET_TABLE_LIMITS_REPLY		1131
*/
#define MSG_LOBBY_ADMIN_CHANGE_TOURN_NAME		1132 // 44s reqId, tournId, text(i18nPString)
#define MSG_LOBBY_ADMIN_CHANGE_TOURN_NAME_REPLY	1133 // 46[s]

#define MSG_LOBBY_ADMIN_CHANGE_TOURN_STATUS			1134 // 4444 reqId, tournId, oldstatus, newstatus
#define MSG_LOBBY_ADMIN_CHANGE_TOURN_STATUS_REPLY	1135 // 46[s]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_NAME_AVAILABLE				1136 // s4 - userId, localeId
#define MSG_LOBBY_NAME_AVAILABLE_REPLY			1137 // 6[s|s4<s>] - errCode [errDescr | requestedUserId, numAlternativeUserIDs <suggestedUserId> ]

#define MSG_LOBBY_ADMIN_TO_AAMS					1138 // 4M[4] - reqId, requestBody [, gateway type]
#define MSG_LOBBY_ADMIN_TO_AAMS_REPLY			1139 // 46[s]

#define MTL_SAT_INFO_FULL						1140 // lobby -> mt.lobby
#define MTL_SAT_INFO_UPDATE						1141 // lobby -> mt.lobby
#define MTL_SAT_INFO_REMOVE						1142 // lobby -> mt.lobby

#define MSG_LOBBY_GET_CAPTCHA					1143
#define MSG_LOBBY_GET_CAPTCHA_REPLY				1144

#define MTL_LOBBY_TOURNAMENT_STATUS_EX			1145	//44 tournId, newStatus
#define MTL_LOBBY_TOURNAMENT_STATUS_EX_REPLY	1146	//6[s] err_code [errDescr]

#define MSG_LOBBY_JUBILEE_HAND_STARTED			1147	//448 - reqId, tableId, handId
#define MSG_LOBBY_JUBILEE_HAND_STARTED_REPLY	1148	//46[s] - reqId, errCode, errStr
#define MSG_LOBBY_JUBILEE_HAND_FINISHED			1149	//4484<s> - reqId, tableId, handId, winnersNumber, <userId>
#define MSG_LOBBY_JUBILEE_HAND_FINISHED_REPLY	1150	//46[s] - reqId, errCode, errStr

#define MSG_LOBBY_ADMIN_JUBILEE_PRIZES			1151	//4884s1<s4>1<s4> - reqId, handId, handNumber, siteId, currency, nWinners<userId,winPrize>, nLosers<userId,playPrize>
#define MSG_LOBBY_ADMIN_JUBILEE_PRIZES_REPLY	1152	//46[s] - reqId, errCode, errStr

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_TOURN_TEXT_INFO						1153	//
#define MTL_TOURN_TEXT_INFO_REPLY				1154	//

#define MTL_LOBBY_REMATCH						1155	//4444<s>4<M> - reqId, scriptId, tournId, numUsers<userId>, numUsers<rematchInfo> 
#define MTL_LOBBY_REMATCH_REPLY					1156	//4 - reqId

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_MIGRATION_GET_INFO			1157	// s4 - userId, eMigrationType
#define MSG_LOBBY_MIGRATION_GET_INFO_REPLY		1158	// 6[s|s4444444444bD] - errCode[errStr|linkedUserId, cashBalance, tBalance, wBalance, fppBalance, monthlyVpp, yearlyVpp, tickets, bonuses, exchangeRate, eMigrationType, bonusEligible, birthdate]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_MIGRATION_LINK				1159	// s4sD - userId, eMigrationType, linkedUserId, birthday
#define MSG_LOBBY_MIGRATION_LINK_REPLY			1160	// Same as MSG_LOBBY_MIGRATION_GET_INFO_REPLY. See errors 1040, 1041

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_MIGRATION_TRANSFER			1161	// s44444 - userId, cashAmt, tAmt, wAmt, fppAmt, exchangeRate
#define MSG_LOBBY_MIGRATION_TRANSFER_REPLY		1162	// 6[s|44ss] - errCode[errStr|cashAmtEuro, fppAmt, linkedUserId, city]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_MIGRATION_CONVERT				1163	// s4 - userId, exchangeRate
#define MSG_LOBBY_MIGRATION_CONVERT_REPLY		1164	// 6[s] - errCode[errStr]

#define MSG_LOBBY_SYNC_BREAK					1167 //

//#define MTL_LOBBY_RANDOM_TABLE					1168
//#define MTL_LOBBY_RANDOM_TABLE_REPLY			1169

#define MTL_REMARCH_REGISTER_USER				1170
#define MTL_REMARCH_REGISTER_USER_REPLY			1171

#define MSG_LOBBY_CLIENT_TO_SUPPORT_EMAIL		1172 // s44ssss4<sB> - userId,cat,subCat,subj,body,clientVer,osVer,numAttachents,<attName,attData>

#define MSG_LOBBY_CLI_LEFT_WAITING				1173 // lobby to client: left waiting lists - 4<4>4<4>4<8>4<4> - numTables<tableId>,numTableStarters<tableTypeId>,numTables<tableId64>,numSeatFinders<seatFinderId>

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_CLIENT_CURR_CONV_OPT			1174 // sbb - userId, doConvertBuyIn, doConvertWinnings
#define MSG_LOBBY_CLIENT_CURR_CONV_OPT_REPLY	1175 // 6[s] - errCode[errStr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_SET_TABLE_LIMITS2				1176 // ss4141 - userId, currency, limit1, type1, limit2, type2
#define MSG_LOBBY_SET_TABLE_LIMITS2_REPLY		1177 // 2[s|12[s|b]12[s|b]44] - errCode[errDescr|type1, errCode1[errDescr1|inEffect1], type2, errCode2[errDescr2|inEffect2], autoLimitHour, waitingIncreaseLimitDays]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_TOURN_DEAL					1178 // 444<4><s4> reqId, tournamentId, numPlaceAwards<award>, numPersonalAwards<userid, award>
#define MTL_ADMIN_TOURN_DEAL_REPLY				1179 // 46[s] - reqId, errCode[errStr]

// PYR-14186: multiple satellite targets
//#define MTL_SAT_FALLBACK_TARGET_SERVER			1180 // 4s tourn id, server
//#define MTL_SAT_FALLBACK_TARGET_SERVER_REPLY	1181

#define MSG_LOBBY_ADMIN_SET_PUBL_PROPERTY		1184 // 4s1[s|8|MSG]4 - reqId, property name, property type [property value], siteId
#define MSG_LOBBY_ADMIN_SET_PUBL_PROPERTY_REPLY	1185 // 46[s]

#define MSG_LOBBY_ADMIN_DELETE_PUBL_PROPERTY		1186 // 4s4 - reqId, property name, siteId
#define MSG_LOBBY_ADMIN_DELETE_PUBL_PROPERTY_REPLY	1187 // 46[s]

#define MSG_LOBBY_ADMIN_LIST_PUBL_PROPERTY			1188 // 44 - reqId, siteId
#define MSG_LOBBY_ADMIN_LIST_PUBL_PROPERTY_REPLY	1189 // 46[s|4<ss>]

// [AO 2010-04-01] #14033 - See MSG_LOBBY_GET_TABLE_LIMITS
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_GET_TOURN_LIMITS				1190
#define MSG_LOBBY_GET_TOURN_LIMITS_REPLY		1191 // 6[s|4<B>4<B>444] - errCode,[errDescr|numLimits<oneLimitBody>,numBalances<oneBalanceBody>,autoLimitHour,waitingIncreaseTournLimitDays,waitingIncreaseSNGLimitDays]
													 // oneLimitBody: 4sb - limit,currency,limitType.  oneBalanceBody: s - currency

// [AO 2010-04-01] #14033 - See MSG_LOBBY_SET_TABLE_LIMITS2
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_SET_TOURN_LIMITS				1192 // ss4141 - userId, currency, limit1, type1, limit2, type2
#define MSG_LOBBY_SET_TOURN_LIMITS_REPLY		1193 // 2[s|12[s|b]12[s|b]44] - errCode[errDescr|type1, errCode1[errDescr1|inEffect1], type2, errCode2[errDescr2|inEffect2], autoLimitHour, waitingIncreaseLimitDays]

// #14967
#define MSG_LOBBY_SET_PROMO_IMAGE				1194 // bs - doSet, promoImageCode
#define MSG_LOBBY_SET_PROMO_IMAGE_REPLY			1195 // 2[s] - errCode[errDescr]
#define MSG_LOBBY_GET_PROMO_IMAGE				1196 //
#define MSG_LOBBY_GET_PROMO_IMAGE_REPLY			1197 // 2[s] - errCode[errDescr]

// #14235 - Arjel Events
#define MSG_LOBBY_GET_ARJEL_EVENTS				1198 // 4 - reqId
#define MSG_LOBBY_GET_ARJEL_EVENTS_REPLY		1199 // 6[s|44<m(...)>m(4<m(s)>)] - errCode[errDescr | cliArjEvtFlgs, num<m(4proxy)>,m(num<m("txt4plr")>)]

// #14430 - Responsible gaming - France
#define MSG_LOBBY_GET_BUYIN_LIMIT				1200 //
#define MSG_LOBBY_GET_BUYIN_LIMIT_REPLY			1201 // 2[s|44] - errCode[errDescr|limit,autoLimitHour]
#define MSG_LOBBY_SET_BUYIN_LIMIT				1202 // 4 - limit
#define MSG_LOBBY_SET_BUYIN_LIMIT_REPLY			1203 // 2[s|b] - errCode[errDescr|effectiveNow]

#define MTL_LOBBY_CHECK_REGISTER_USER			1204 // s4441s[4] userName, buyIn, buyInPlay, buyInFpp, torunPwd, [cameFromTournId(only from admin)], useTicket
//#define MTL_LOBBY_CHECK_REGISTER_USER_REPLY		1205 // 6[s] err_code [errDescr]

// #15236 - Estonia Global Black List
#define MSG_LOBBY_ADMIN_MASS_UNREGISTER_USER		1206 // st - userName, until
#define MSG_LOBBY_ADMIN_MASS_UNREGISTER_USER_REPLY	1207 // 2[s] - errCode [errDescription]

#define MSG_LOBBY_CLIENT_TO_SUPPORT_EMAIL_REPLY		1208 // 2 - errCode

// #15990 - generic notifications to the client
#define MSG_LOBBY_SEND_NOTIFICATION_TO_USER			1209 // 4s6s - reqId, userId, infoCode, pop-up
#define MSG_LOBBY_SEND_NOTIFICATION_TO_USER_REPLY	1210 // 6[s] - errCode[errDescr]

#define MSG_LOBBY_NOTIFICATION						1211 // 6s infoCode(0-i,1-w,2-e) infoText

#define MSG_LOBBY_SUPPORT_EMAIL_READY				1212 // s - userId
#define MSG_LOBBY_SUPPORT_EMAIL_READY_REPLY			1213 // 2 - errCode

// #15250 - Responsible gaming - Estonia
#define MSG_LOBBY_GET_PLAYING_TIME_LIMIT			1220 //
#define MSG_LOBBY_GET_PLAYING_TIME_LIMIT_REPLY		1221 // 2[s|44] - errCode[errDescr|limit,autoLimitHour]
#define MSG_LOBBY_SET_PLAYING_TIME_LIMIT			1222 // 4 - limit
#define MSG_LOBBY_SET_PLAYING_TIME_LIMIT_REPLY		1223 // 2[s|b] - errCode[errDescr|effectiveNow]

//PYR-16022 lobby <--> table
#define MSG_LOBBY_TABLE_CHECK_USER_FOR_SEAT			1224 //s2b - userId,timeout, requestType
#define MSG_LOBBY_TABLE_CHECK_USER_FOR_SEAT_REPLY	1225 //24s1b - errCode,tableId,userId,seat, requestType
#define MSG_LOBBY_TABLE_CHECK_USER_FOR_SEAT_ACK		1226 //
#define MSG_LOBBY_TIMER_FIND_SIMILAR_TABLE			1227 //

// #15607 - home games
#define MSG_LOBBY_HOME_GAMES_SAVE_IMAGE             1230 // 4B - club id, image
#define MSG_LOBBY_HOME_GAMES_SAVE_IMAGE_REPLY       1231 // 6[s|4] - errCode, [errDescr|imageid]

// #16505 - italian player black listed
#define MSG_LOBBY_PLAYER_BLACKLISTED_NOTIFY			1232 // 2s2s - err code, permBannedByRegText, status, internalError

// #15607 - home games
#define MSG_LOBBY_HOME_GAMES_UNREGISTER_USERS       1233 // 44<s>, club id num users, <user id>
#define MSG_LOBBY_HOME_GAMES_UNREGISTER_USERS_ACK   1234 // 44<s> - reqId, numUsersCannotUnregister, <userId>
#define MSG_LOBBY_HOME_GAMES_KILL_TOURNS            1235 // 4s4<4>, club id, effCanceller, num tourns, <tourn id>
#define MSG_LOBBY_HOME_GAMES_KILL_TOURNS_REPLY      1236 // 4<46[s]>, num tourns, <tourn id, errCode [, errMess] >

// #17065
#define MSG_LOBBY_RESEND_ACTIVATION_EMAIL			1237 // empty
#define MSG_LOBBY_RESEND_ACTIVATION_EMAIL_REPLY		1238 // 6[s] - errCode[errInfo]

// #12881
#define MSG_LOBBY_GET_NEXT_TOURN_BY_REF				1239 // 4s - tournId,internal ref
#define MSG_LOBBY_GET_NEXT_TOURN_BY_REF_REPLY		1240 // 2[s|4] - errCode[errInfo | tournId]

// #15607 - home games
#define MSG_LOBBY_HOME_GAMES_REMOVE_USERS_FROM_TABLE      1241 // 4s4<s...> - club id, reason, num users, <user id, ...>
#define MSG_LOBBY_HOME_GAMES_REMOVE_USERS_FROM_TABLE_ACK  1242 // empty

#define MSG_LOBBY_SECURITY_ALERT			      1243 //
#define MSG_LOBBY_SECURITY_ALERT_REPLY			  1244 // empty

// #16753
#define MSG_LOBBY_GET_CLIENT_OPTIONS				1245 // 4 - optionType
#define MSG_LOBBY_GET_CLIENT_OPTIONS_REPLY			1246 // 6[s|M] - errCode[errInfo|optionsBody]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_SET_CLIENT_OPTIONS				1247 // M4 - optionsBody, optionType
#define MSG_LOBBY_SET_CLIENT_OPTIONS_REPLY			1248 // 6[s] - errCode[errInfo]

//PYR-15846
#define MTL_TABLE_SHOW_DEALOPTION					1249 //11 - showDealOption,flags
#define MSG_TABLE_MTLOBBY_DISCUSS_DEAL				1250 //1s - status,userId

//PYR-15607
#define MSG_LOBBY_HG_RESTRICTED_COUNTRIES			1251 //4<s> - restricted countries vector
#define MSG_LOBBY_HG_RESTRICTED_COUNTRIES_REPLY		1252 //6[s] - errCode[errInfo]

// 	PYR-17878
#define MSG_LOBBY_USER_SUSPENDED_NOTIFY				1253 // 4st uint32, string, when

// PYR-17813 lobby -> mtlobby
#define MTL_LOBBY_SET_NEXT_HG_RECURRING_TOURN       1254 // 4 - next tourn id

// PYR-17813 hglobby <-> lobby
#define MSG_LOBBY_HG_SET_NEXT_HG_RECURRING_TOURN       1255 // 44 - tourn id, next tourn id
#define MSG_LOBBY_HG_SET_NEXT_HG_RECURRING_TOURN_REPLY 1256 // 6[s] - errCode, [errInfo]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_ADMIN_GET_TABLE_ADDRESS					1257 // 448 - reqId, tableId32,tableId64
#define MTL_ADMIN_GET_TABLE_ADDRESS_REPLY			1258 // 46[s|sss] - reqId, errCode[errDescr|server, serverObject, tablePublication]

// PYR-67196 lobbyEtc -> lobby: request to forward notification to user
#define MSG_LOBBY_FORWARD_NOTIFICATION_TO_USER		1259 // 4s4M  reqId userId lobbyToUserMsgId msgBody
#define MSG_LOBBY_FORWARD_NOTIFICATION_TO_USER_REPLY	1260 // 46[s] reqId errCode[errDescr]

#define MSG_LOBBY_DAD_UNFINISHED_TRANS_NOTIFY	1261 // identical to MSG_CASHIER_DAD_UNFINISHED_TRANS_NOTIFY

#define MSG_LOBBY_UPDATE_BY_LICENSE				1262 // 44[4] - clientPlatform,licenseId[,siteId]
#define MSG_LOBBY_UPDATE_BY_LICENSE_REPLY		1263 // 6[s|s] - errCode[ errDescr | updateFolder ]

// #18129
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_ADMIN_GET_DUMMY_TOURN_INFO		1264 // 4 - reqId
#define MTL_LOBBY_ADMIN_GET_DUMMY_TOURN_INFO_REPLY	1265 // 46[s|1T7] - q_id, errCode, [errMsg | status, time, max players]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MTL_LOBBY_ADMIN_SET_DUMMY_TOURN_INFO		1266 // 41T7b - reqId, status, time, max players, checkOnly
#define MTL_LOBBY_ADMIN_SET_DUMMY_TOURN_INFO_REPLY	1267 // 46[s] - q_id, errCode, [errMsg]

// PYR-18621
#define MSG_LOBBY_NOTIFY_CLIENT_START_DATE_SET		1268 // 4s - reqId, userid
#define MSG_LOBBY_NOTIFY_CLIENT_START_DATE_SET_REPLY	1269 // 46[s] - reqId,errCode[errDescr]

//client <--> blitz lobby
#define BL_USER_BUYIN					1270	//4{.} - chips,{currencycontext}
#define BL_USER_BUYIN_REPLY				1271	//6[s|4ssb4bbb17] - errCode,[errDescr|entryId,serverName,tableObjectName,sittingOut,chips,sitoutBB,state3Plus,publOverConn,playingEntries,chipsAfter]

#define BL_USER_OUT						1272	//4 - entryId
#define BL_USER_OUT_REPLY				1273	//46[s|] - entryId,errCode,[errDescr|]

#define BL_USER_ADDON					1274	//44{.} - entryId,chips,{currencycontext}
#define BL_USER_ADDON_REPLY				1275	//6[s4|4] - errCode,[errDescr,entryId|entryId]

//blitz lobby -> client
#define BL_USER_KICKED					1276	//4s - entryId,msg
#define BL_USER_WHERE_REPLY				1277	//46[s|ssb4bsb4] - entryId,errCode,[errDescr|serverName,tableObjectName,sittingOut,chips,sitoutBB,itId,state3Plus,scalePM]

//client <--> blitz lobby
#define BL_USER_SITIN					1278	//4 - entryId
#define BL_USER_SITIN_REPLY				1279	//46[s|ssb4bb] - entryId,errCode,[errDescr|serverName,tableObjectName,sittingOut,chips,sitoutBB,state3Plus]

//table <--> blitz lobby
#define BL_TABLE_CREATED				1280	//4 - tableId
#define BL_TABLE_CREATED_REPLY			1281	//empty

#define BL_TABLE_HAND_FINISHED			1282	//4141141<s14454474>48 - tableId,handCancelled,potSize,playersFlop,nbVoluntarilyPutMoneyInPot,averageStackSize,numUsers,<userId,userPos,userChipsStart,userChips,connQuality,statFlags,fppsInCents,chipsWon,usedTimebank>,replyTimeout,handId
#define BL_TABLE_HAND_FINISHED_REPLY	1283	//empty

#define BL_TABLE_USER_FOLD				1284	//4s4444 - tableId,userId,startChips,endChips,flags,usedTimebank

//blitz lobby -> client
#define BL_USER_DISPLAY_MESSAGE			1285	//4s - entryId,msg

//blitz lobby <--> table
#define BL_TABLE_HAND_START				1286	//4 - tableId
#define BL_TABLE_HAND_START_REPLY		1287	//46 - tableId,errorCode (see table.h::enum ErrorCodeSrvMtlb {} )

//blitz lobby <--> main lobby
#define BL_LOBBY_CREATED				1288	//46[s4|4...] - reqId,errCode,[errDescr,blitzId|blitzId,params...]
#define BL_LOBBY_CREATED_REPLY			1289	//46 - reqId,errCode

#define BL_LOBBY_USER_IN				1290	//4s41 - reqId,userId,numUsersPubl,prop
#define BL_LOBBY_USER_IN_REPLY			1291	//4 - reqId

#define BL_LOBBY_USER_OUT				1292	//4s41 - reqId,userId,numUsersPubl,prop
#define BL_LOBBY_USER_OUT_REPLY			1293	//4 - reqId

#define BL_LOBBY_UPDATE_STATS			1294	//444141441412<24T>4 - reqId,blitzId,numPlayers,playersFlop,avgPot,vpip,avgStack,handsHour,blitzStatus,tablesInHand,ClientPublProperty,numUpdates<fppFactor,sites,endT>,getNumMsgCheckErrors
#define BL_LOBBY_UPDATE_STATS_REPLY		1295	//4 - reqId

//lobby -> blitz lobby
#define BL_LOBBY_DIE					1296	//44S4s - 0,sessionId,i18nMsg,flags,adminId		//unsolicited
#define BL_LOBBY_STOP					1297	//44S4s - 0,sessionId,i18nMsg,flags,adminId		//unsolicited
#define BL_LOBBY_RESUME					1298	//44S4s - 0,sessionId,i18nMsg,flags,adminId		//unsolicited
#define BL_LOBBY_KICK_USER				1299	//4sS4 - 0,userId,i18nMsg,reason		//unsolicited

//admin <--> lobby
#define MSG_LOBBY_ADMIN_CREATE_BLITZ		1300	//4... - reqId,...
#define MSG_LOBBY_ADMIN_CREATE_BLITZ_REPLY	1301	//46s - reqId,errCode,errDescr

//blitz lobby <--> main lobby
#define BL_LOBBY_READY_TO_DIE			1302	//44b44b - reqId,sessionId,recreateMe,tableTypeId,maxPlayers,creditStuckUsers
#define BL_LOBBY_READY_TO_DIE_REPLY		1303	//4 - reqId

//client <--> blitz lobby
#define BL_USER_GET_ENTRIES				1304	//
#define BL_USER_GET_ENTRIES_REPLY		1305	//4<{4ssb41222bb}>b - numEntries,<{entryId,serverName,tableObjectName,sittingOut,chips,autoRebuyOptions,autoRebuyBigBets,autoRebuyFallUnderBigBets,autoRebuyFallUnderPercents,sitoutBB,state3Plus}>,itZeroExitSupport

//blitz lobby <--> table
#define BL_TABLE_USER_KICK				1306	//4s4 - tableId,userId,flags
#define BL_TABLE_USER_KICK_REPLY		1307	//46 - tableId,errorCode (see table.h::enum ErrorCodeSrvMtlb {} )

//client <--> blitz lobby
#define BL_USER_GET_BUYIN_INFO			1308	//4 - entryId(0 if new)
#define BL_USER_GET_BUYIN_INFO_REPLY	1309	//6[ss4b|444414m144b448414] - errCode,[errDescr,currency,minChipsLimit,enoughOnAllAccounts|chips,minChipsLimit,maxBuyIn,chipsQuantum,playAddB,playAddU,{currencyContext},entriesLeft,entryId,chipsInPlay,enoughOnAllAccounts,maxRebuyChips,totalRebuyChips,playMoney64,scalePM,clientServiceFlag,defaultBuyIn]


// PYR-20290 
#define MTL_LOBBY_DUMMY_TOURN_UPDATE			1310 // 44tl4 reqId, tournId, StartTime, restricted, maxEntrants 
#define MTL_LOBBY_DUMMY_TOURN_UPDATE_REPLY		1311 // 4 reqId


//admin <--> blitz lobby
#define BL_ADMIN_DIE					1312	//44b[S4] - reqId,sessionId,creditStuckUsers,i18nMsg,flags
#define BL_ADMIN_DIE_REPLY				1313	//46[s] - reqId,errCode,[errDescr]

#define BL_ADMIN_STOP					1314	//44S4 - reqId,sessionId,i18nMsg,flags
#define BL_ADMIN_STOP_REPLY				1315	//46[s] - reqId,errCode,[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define BL_ADMIN_RESUME					1316	//44S4 - reqId,sessionId,i18nMsg,flags
#define BL_ADMIN_RESUME_REPLY			1317	//46[s] - reqId,errCode,[errDescr]

#define BL_ADMIN_KICK_USER				1318	//44sS - reqId,sessionId,userId,i18nMsg
#define BL_ADMIN_KICK_USER_REPLY		1319	//46[s] - reqId,errCode,[errDescr]

//client <--> blitz lobby
#define BL_USER_GET_INFO				1320	//empty
#define BL_USER_GET_INFO_REPLY			1321	//s11111441442s444sb44 - name,isOneOnOne,isPlayMoney,playerPerTable,game,isHiLo,loBet,hiBet,structure,cap,blitzFlags,chatLang,currency,minChipsLimit,maxBuyIn,ante,itId,shortHanded,scalePM,noVppGainSites

//table -> blitz lobby
#define BL_TABLE_USER_SITOUT			1322	//4s - tableId,userId

//blitz lobby -> client
#define BL_USER_ADDON_REPLY2			1323	//46[s|ssb4bb4] - entryId,errCode,[errDescr|serverName,tableObjectName,sittingOut,chips,sitoutBB,state3Plus,scalePM]


// PYR-18513 voice Chat Support
#define MSG_LOBBY_HOME_GAMES_MUTE_USER		 1324 // 44s4 - reqId, clubId, userId, userIntId
#define MSG_LOBBY_HOME_GAMES_MUTE_USER_REPLY 1325 // empty

#define MSG_LOBBY_REQUEST_DOCS_USER_NOTIFY	 1326 // 6ss eClientNotificationCode(0-i,1-w,2-e) text subject

// obsolete #21771
//#define LOBBY_TO_CASHIER_VERIFY_USER_NOTIFICATION	1327 // s448484 userId, licenseId, privs, privs2, flags, flags2, secondsSinceRmSubmission

#define MSG_LOBBY_KICK_UNREGISTER_USER		 1328 // 4ssb - reqId, userId, text, unregPM
#define MSG_LOBBY_KICK_UNREGISTER_USER_REPLY 1329 // 46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBY_WRONG_REALMONEY_INFO_USER_NOTIFY	1330 // 6ss eClientNotificationCode(0-i,1-w,2-e) text subject

//PYR-21113
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_REREAD_INIDATA			1331	//empty
#define MSG_LOBBY_ADMIN_REREAD_INIDATA_REPLY	1332	//46[s] - reqId,errCode,[errDescr]


//admin <--> main lobby
#define MSG_LOBBY_ADMIN_STOP_BLITZ_DEALING			1333	//4S414 - reqId,i18nMsg,msgFlags,flags,sites
#define MSG_LOBBY_ADMIN_STOP_BLITZ_DEALING_REPLY	1334	//46[s] - reqId,errCode,[errDescr]

//client <--> blitz lobby
#define BL_USER_SET_AUTOREBUY				1335	//41222 - entryId,autoRebuyOpt,autoRebuyBigBets,fallUnderBigBets,fallUnderPercents
#define BL_USER_SET_AUTOREBUY_REPLY			1336	//46[s] - entryId,errCode,[errDescr]

//client <--> blitz lobby
#define BL_USER_SITOUT						1337	//4 - entryId
#define BL_USER_SITOUT_REPLY				1338	//46[s] - entryId,errCode,[errDescr]

#define LOBBY_TO_REGULATOR_VERIFY_USER			1339 // 4s448484 reqId, userId, licenseId, privs, privs2, flags, flags2, secondsSinceRmSubmission
#define LOBBY_TO_REGULATOR_VERIFY_USER_REPLY	1340 // 46[s]

//client <--> blitz lobby
#define BL_USER_SITOUT_NEXTBB				1341	//4b - entryId,sitoutBB
#define BL_USER_SITOUT_NEXTBB_REPLY			1342	//46[s|b] - entryId,errCode,[errDescr|sitoutBB]

//admin <--> main lobby
#define MSG_LOBBY_ADMIN_RESUME_BLITZ_DEALING		1343	//4S414 - reqId,i18nMsg,msgFlags,flags,sites
#define MSG_LOBBY_ADMIN_RESUME_BLITZ_DEALING_REPLY	1344	//46[s] - reqId,errCode,[errDescr]

//admin <--> main lobby
#define MSG_LOBBY_ADMIN_GET_TOURN_SCRIPT		1345	//44<4> - reqId, size, <tournId>
#define MSG_LOBBY_ADMIN_GET_TOURN_SCRIPT_REPLY	1346	//44<6[4s|o]>- reqId, size <errCode [tournId errDescr|script>]

// regulator <-> lobby (#22242)
#define MSG_LOBBY_BROADCAST_USER_PRIVILEGES			1347 // 41s44488 - identical to MSG_LOBBY_BROADCAST_PRIVILEGES
#define MSG_LOBBY_BROADCAST_USER_PRIVILEGES_REPLY	1348 // 46[s] - reqId,errCode,[errDescr]

//blitz lobby <--> table
#define BL_TABLE_SIT_USERS						1349	//41<...B> - tableId,numPlayers,<userOnTable,authBody>

//admin <--> lobby	//obsolete
#define MSG_LOBBY_ADMIN_REGENERATE_BLITZ		1350	//4... - reqId,...
#define MSG_LOBBY_ADMIN_REGENERATE_BLITZ_REPLY	1351	//46[s|4] - reqId,errCode,[errDescr|sessionId]

// PYR-21857 - cleanup dead tournament
// if cancel running tournament was unsuccessful
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_CLEANUP_DEAD_TOURN          1352 //44 - reqId,tournId
#define MSG_LOBBY_ADMIN_CLEANUP_DEAD_TOURN_REPLY    1353 //46[s] - reqId,errCode[message]

//lobby -> blitz lobby
#define BL_LOBBY_UPDATE_PARAMS					1354	//44s444444b11<r> - 0,blitzId,updater,maxEntries,sitoutTimeout,timebankInitial,timebankIncrement,timebankHandsIncr,timebankMax,useFastTableLogin,checkRelated,checkIp,<tableType>	//unsolicited

//PYR-21295
#define MSG_LOBBY_ADMIN_LOBBY_FLYER_UPDATE			1355	//44 - reqId,templateId
#define MSG_LOBBY_ADMIN_LOBBY_FLYER_UPDATE_REPLY	1356	//46s - reqId,errCode,errDescr

// create a new table in DB only - from seat finder
#define SFN_LOBBY_CREATE_TABLE_IN_DBM			1357    //44 - reqId,tableTypeId
#define SFN_LOBBY_CREATE_TABLE_IN_DBM_REPLY		1358    //46[s|44s] - reqId,errCode[errDescr|dbmId,tableId,tableName]

// PYR-97488 - obsolete
//#define MSG_OBSOLETE							1359
//#define MSG_OBSOLETE_REPLY					1360

// create the table object only (table must exist in DB) - from seat finder
#define SFN_LOBBY_CREATE_TABLE_FROM_DBM			1361    //4444s51<s14so>s - reqId,dbmId,tableId,typeId,tableName,dealerPos,numUsers<name,position,buyIn,aamsTicket,autoRebuy>,aamsSession
#define SFN_LOBBY_CREATE_TABLE_FROM_DBM_REPLY	1362    //46[s|ss] - reqId,errCode[errDescr|server,serverObject]

// PYR-97488 - obsolete
//#define MSG_OBSOLETE							1363
//#define MSG_OBSOLETE_REPLY					1364

//#define MSG_OBSOLETE							1365
//#define MSG_OBSOLETE_REPLY					1366

//#define MSG_OBSOLETE							1367
//#define MSG_OBSOLETE_REPLY						1368

// PYR-19749
#define MSG_LOBBY_FORWARD_INTERNAL_ERROR_AUDIT			1369 // 4<not parsing> - reqId,<pass the rest of the message on>
#define MSG_LOBBY_FORWARD_INTERNAL_ERROR_AUDIT_REPLY	1370 // 46 - reqId, errCode 

// PYR-97488 - obsolete
//#define MSG_OBSOLETE								1371
//#define MSG_OBSOLETE_REPLY						1372

//#define MSG_OBSOLETE								1373

//PYR-21295
#define MSG_LOBBY_ADMIN_LOBBY_FLYER_REMOVE			1374	//4444<4>4<4> - reqId,templateId,siteId,numAttr,<attributes>,numFlags,<flags>
#define MSG_LOBBY_ADMIN_LOBBY_FLYER_REMOVE_REPLY	1375	//46s - reqId,errCode,errDescr

// PYR-97488 - deprecated
//#define MSG_OBSOLETE                          1376
//#define MSG_OBSOLETE_REPLY                    1377

//#define MSG_OBSOLETE                          1378
//#define MSG_OBSOLETE_REPLY                    1379

//#define MSG_OBSOLETE                          1380
//#define MSG_OBSOLETE_REPLY                    1381

//#define MSG_OBSOLETE                          1382
//#define MSG_OBSOLETE_REPLY                    1383

//#define MSG_OBSOLETE							1384
//#define MSG_OBSOLETE_REPLY					1385

//#define MSG_OBSOLETE							1386
//#define MSG_OBSOLETE_REPLY					1387

/* obsolete
// PYR-20837 client->lobby
#define MSG_LOBBY_GET_ES_LAST_LOGIN_DETAILS			1388	//
#define MSG_LOBBY_GET_ES_LAST_LOGIN_DETAILS_REPLY	1389	//6[s|M(tts88)M(4<M(4sb)>)M(4<M(8s)>)] - err [errDescr| M(lastLogin, lastLogout, reasonDescr, totalPlayed,totalWon),M(numTourns<tournId,tournName,published>)(numHands<handId,tableName>)]
*/

// PYR-20837 client->lobby
#define MSG_LOBBY_GET_ES_LAST_LOGIN_REPORT			1390	//14<4>4<8> - tournStatType,tournCount<tournId>,handCount<handId>
#define MSG_LOBBY_GET_ES_LAST_LOGIN_REPORT_REPLY	1391	//6[s|]

//	PYR-45659
//	LobbyEtc->Lobby, CVL activation
// replaces obsoleted: 
// PYR-19814 client->lobby
// #define MSG_LOBBY_GET_TRANSLATED_INTERNAL_ERROR			1392	//s - internalErrorDescr, clientSiteId, locale
// #define MSG_LOBBY_GET_TRANSLATED_INTERNAL_ERROR_REPLY	1393	//6[s|s] - errCode, [translatedError| errDescr]
#define MSG_LOBBYETC_CVL_ACTIVATION_CHANGE			1392 // 44<CvlConfigKey> - reqId, num<CvlConfigKey>
#define MSG_LOBBYETC_CVL_ACTIVATION_CHANGE_REPLY	1393 // 46[s] - reqId,errCode [ errDescr ]

//PYR-22623
#define BL_LOBBY_BROADCAST_PRIVILEGES			1394	// 41s444 - identical to MSG_LOBBY_BROADCAST_PRIVILEGES
#define BL_LOBBY_BROADCAST_MULTI_PRIVILEGES		1395	// 44<1s444> - reqId(0), numItems, <updateType, userId, newEffectivePrivileges, flags, imageId>

//PYR-22570
#define MSG_LOBBY_KICK_USER_FROM_SPECIFIED_GAMES		1396	//4ss4<4>4<4>4<4> - reqId,userId,text,tables,tournaments,blitz
#define MSG_LOBBY_KICK_USER_FROM_SPECIFIED_GAMES_REPLY	1397	//46 - reqId, errCode

// PYR-97488 - obsolete
//#define MSG_OBSOLETE							1398

//#define MSG_OBSOLETE							1399
//#define MSG_OBSOLETE_REPLY					1400

// PYR-22626
// lobby -> blitz lobby
#define BL_LOBBY_SET_FPP_FACTOR					1401	//42<24> 0,numFppFactors<newFppFactor,sites>

//blitz lobby <--> lobby
#define BL_LOBBY_UPDATE_TABLETYPE				1402	//44s<r> - reqId,blitzId,updater,<tableType>
#define BL_LOBBY_UPDATE_TABLETYPE_REPLY			1403	//444 - reqId,blitzId,tableTypeId

// PYR-21829 Lobby -> Client
#define MSG_LOBBY_CLI_RECONNECT   				1404    // empty

// #21829
#define MSG_LOBBY_ADMIN_MANAGE_INSTALLID		1405	//4ssb	- reqId, userId, installId, validate/invalidate
#define MSG_LOBBY_ADMIN_MANAGE_INSTALLID_REPLY	1406	//46[s] - reqId, errCode[errDescr]

// #22885 blitz mtl -> table
#define BMTL_TABLE_KICK_USERS					1407
#define BMTL_TABLE_KICK_USERS_REPLY				1408

// #22885 blitz mtl -> client
#define MTL_CLI_GOTO_TABLE						1409	//ssbs4 - table address, table name, isPlayMoney, currency, chips
#define MTL_USER_SITIN							1410	// - empty
#define MTL_USER_SITIN_REPLY					1411	//6[s] - errCode[errDescr]
#define MTL_USER_SITOUT							1412	// - empty
#define MTL_USER_SITOUT_REPLY					1413	//6[s] - errCode[errDescr]

#define MTL_TABLE_SITIN							1414	//s - user
#define MTL_TABLE_SITIN_REPLY					1415	//6[4] - errCode[tableId]
#define MTL_TABLE_SITOUT						1416	//s - user
#define MTL_TABLE_SITOUT_REPLY					1417	//6[4] - errCode[TableId]

// PYR-20055 - periodically check if any disconnected users should be unregistered from S&G tournaments
#define MSG_LOBBY_TIMER_UNREG_DISCONNECTED		1418	// empty
#define LOBBY_TIMER_UNREG_DISCONNECTED_MSEC		(10*1000)

#define MSG_LOBBY_TGM_DEPLOYMENT_CHECK			1419
#define MSG_LOBBY_TGM_DEPLOYMENT_CHECK_REPLY	1420

// PYR-97488 - deprecated
//#define MSG_OBSOLETE							1421
//#define MSG_OBSOLETE_REPLY					1422

//#define MSG_OBSOLETE							1423
//#define MSG_OBSOLETE_REPLY					1424

//blitzlobby <--> lobby
#define BL_LOBBY_USERS_ON_START					1425	//4414<s> - reqId,numUsersPubl,prop,numUsers,<userId>
#define BL_LOBBY_USERS_ON_START_REPLY			1426	//4 - reqId

#define MTL_CLI_GOTO_FINISH						1427	//4ss44[t4s4s1b4444b1sb44ss4|]b[4<4s>s]4444t - place, popupText, socialToken, nextSchedTournId[ statTime, clubId, clubName, tournId, tournName,gameType, isHiLo, buyIn, rake, knockout, numPlayers, isRegistered, limit, currency, isPlayMoney, maxPlayers, tournState, serverAddress, serverNAme, flags|], isNonFinalFlight [ i18nNextFlightLevelTargetTournName, nextFlightLevelTargetServer], headBounty, tournUserFinishFlags, award, bounty, reEntryDeadline

//client <--> blitz lobby
#define BL_USER_SET_3PLUS						1428	//4b - entryId,state3Plus
#define BL_USER_SET_3PLUS_REPLY					1429	//46[s|b] - entryId,errCode,[errDescr|state3Plus]

//blitzlobby <--> lobby
#define BL_LOBBY_UPDATE_STATIC_DATA				1430	//4s - reqId,replayerInstance
#define BL_LOBBY_UPDATE_STATIC_DATA_REPLY		1431	//4 - reqId

// PYR-24004: mtlobby <--> table
#define BL_TABLE_INIT_PUBL_SNAPSHOT             1434   //
#define BL_TABLE_INIT_PUBL_SNAPSHOT_REPLY       1435   // m - initPublSnapshot

//PYR-24396
#define MTL_LOBBY_GET_HAPPYHOURS				1436	//4 - reqId
#define MTL_LOBBY_GET_HAPPYHOURS_REPLY			1437	//44<rec> - reqId,numRecords,<TournHappyHours>

//PYR-24099
#define MTL_USER_REBUY							1438	
#define MTL_USER_REBUY_REPLY					1439	
#define MTL_USER_ADDON							1440	
#define MTL_USER_ADDON_REPLY					1441	

#define MTL_LOBBY_HAPPYHOURS_BROADCAST			1442	//44<rec> - 0,numRecords,<TournHappyHours>

//PYR-25413, PYR-27250
#define MSG_LOBBY_NOTIFY_CONNECT				1443	//44 - reqId,<blitzId|tournId>
#define MSG_LOBBY_NOTIFY_CONNECT_REPLY			1444	//4 - reqId

//PYR-25261
#define MSG_LOBBY_TABLEDEALINGCARDSSTATUS_UPDATE   1445   //44b - reqId,tableId,status
#define MSG_LOBBY_TABLEDEALINGCARDSSTATUS_UPDATE_REPLY 1446  //4 -reqId
#define MSG_LOBBY_NOTIFY_CLIENT_RMOK_REMOVED    1447	// PYR-25239

// PYR-22393
// tblServFlags - enum TableServiceFlags
// numSubscr - 'observersRequired' from tourn script
#define MTL_TABLE_START_COUNT_SUBSCRIBERS		1448	//44 - tblServFlags, numSubscr
#define MTL_CLI_FINALIZE_REBUY					1449	//4 - chips to add

// PYR-25419
#define MSG_LOBBY_TABLE_CANCEL_RESERVED_SEAT	1450	//s1 - userId, seat
#define MTL_USER_DECLINE_REBUY					1451

//PYR-24382 mtlobby - lobby
// #define MTL_LOBBY_GOLDENSNG_WON_OBSOLETE					1452	//444 - reqId,tournId,score
// #define MTL_LOBBY_GOLDENSNG_WON_REPLY_OBSOLETE			1453	//44444 - reqId,msId,msBuyinMult,msPrizepoolMult,msUserAward

// #define MTL_LOBBY_GOLDENSNG_USER_WON_OBSOLETE			1454	//444ss4 - reqId,tournId,regSites,userId,currency,score
// #define MTL_LOBBY_GOLDENSNG_USER_WON_REPLY_OBSOLETE		1455	//4 - reqId

// PYR-97488 - obsolete
//#define MSG_OBSOLETE							1456
//#define MSG_OBSOLETE_REPLY					1457

//#define MSG_OBSOLETE							1458

// #define MTL_LOBBY_GOLDENSNG_BROADCAST_OBSOLETE			1459	//4 - 0

#define MTL_LOBBY_SAG_SATELLITE							1460 //PYR-43478
#define MTL_LOBBY_SAG_SATELLITE_REPLY					1461 //PYR-43478

// PYR-25930
#define MSG_LOBBY_TABLE_UPDATE_JUBILEE_AWARDS	1462	//41<s4> - reqId, numUsers<userId, award>

//mtlobby -> table
// #define MTL_TABLE_SET_GOLDENSNG_MULTIPLIER_OBSOLETE		1463	//4 - multiplier

// PYR-26694
#define MTL_GET_CHIP_GRAPH_DATA				1464
#define MTL_GET_CHIP_GRAPH_DATA_REPLY		1465

//PYR-18141
#define MTL_SET_IMREADY						1466	//
#define MTL_SET_IMREADY_REPLY				1467	//6b - errCode,userReady
#define MTL_GET_IMREADY						1468	//
#define MTL_GET_IMREADY_REPLY				1469	//6b - errCode,userReady

// PYR-114224
//blitzlobby <--> lobby
#define BL_LOBBY_UPDATE_EARLY_BIRD				1470	//4s - reqId,earlyBirdMultiplier,earlyBirdMins
#define BL_LOBBY_UPDATE_EARLY_BIRD_REPLY		1471	//4 - reqId

// PYR-97488 - deprecated
//#define MSG_OBSOLETE						1570
//#define MSG_OBSOLETE_REPLY				1571

// PYR-27036
#define MSG_LOBBY_TIMER_REMATCH_CANCEL		1572 // 4s tournid, userId
#define MSG_LOBBY_TIMER_REMATCH_CANCEL_MIN_MSEC	2000 // min value 2 sec
#define MSG_LOBBY_TIMER_REMATCH_CANCEL_MAX_MSEC	5000 // max value 5 sec
#define MSG_LOBBY_TIMER_REMATCH_CANCEL_RANGE_MSEC	(MSG_LOBBY_TIMER_REMATCH_CANCEL_MAX_MSEC-MSG_LOBBY_TIMER_REMATCH_CANCEL_MIN_MSEC)

// PYR-25226
#define MSG_TABLE_MTLOBBY_UPDATE_STACKS    1573		// s44 - userId, chips, stacks
#define BMTL_TABLE_RELOAD_STACKS_NOTIF		1574	// s44 - userId, stacksLoaded, stacks
#define MTL_USER_RELOAD_STACKS				1575	// 4b - reqStacks, isSyncReload
#define MTL_USER_RELOAD_STACKS_REPLY		1576	// 1[sm|4] - errcode [ errstr, errmsg | stacks ]
#define MTL_USER_LIMBO_CLIENT_ACTIONTIMER_STARTED	1577  // empty msg
#define MTL_USER_INVITE_CHOOSE_STARTING_STACKS 1578	// 4 - stacks
#define MTL_USER_AUTO_RELOAD_STACKS_NOTIFY    1580	 // 4 - stacks loaded
#define MSG_TABLE_DBUPDATE_ALLOCATED_UNUSEDSTACKS_BATCH  1582  // 4<b44s44> - vector of < isBlitz, tournId, tableId, userId, chipsAdded, unusedStacks >
#define MSG_TABLE_DBUPDATE_ALLOCATED_UNUSEDSTACKS_BATCH_REPLY  1583  // 1[s] - errcode [ errstr ]

// PYR-28689
#define MSG_TABLE_TOURN_MILESTONE_EVENT_PREANNOUNCE        1584  // 2 - eventIndex
#define MSG_TABLE_TOURN_MILESTONE_EVENT_PREANNOUNCE_REPLY  1585  // 8284<s> - tableId, eventIndex, handId, playerlist
#define MSG_TABLE_TOURN_MILESTONE_EVENT_PREANNOUNCE_CANCEL 1586  // 2 - eventIndex
#define MSG_TABLE_TOURN_MILESTONE_EVENT           1587  // 22m - eventIndex, eventCode, eventMsg
#define MSG_TABLE_TOURN_MILESTONE_EVENT_REPLY     1588  // 8228[4<s>] - tableId, eventIndex, eventCode, handId, playerlist
#define MSG_TABLE_TOURN_MILESTONE_EVENT_ANOUNCE_END_NOHAND   1589  // 8 -tableId
#define MSG_LOBBY_TOURN_MILESONE_UPDATE           1590  // 44s84<prize> // tournId, sites, currency, winTableId, nextMileStoneTime, prize
#define MSG_LOBBY_TOURN_MILESONE_UPDATE_REPLY     1591  // 

#define MTL_TABLE_AUTO_DEAL_STATE				  1592  //1 - state
#define MTL_TABLE_AUTO_DEAL_CHAT                  1593  // 1.. - subActionCode...           the rest of the message depends on subActionCode

// client to lobby
#define MSG_LOBBY_CHECK_PUBLICATIONS			  1594	// 4<s> vector of publication names 
#define MSG_LOBBY_CHECK_PUBLICATIONS_REPLY		  1595	// 4<b> vector of boolean for each publication name indicate if it exists

// lobby to news
#define MSG_LOBBY_CHECK_NEWS_PUBLICATIONS			 1596	// 4M(4<s>)	connection id, message body(vector of publication names)
#define MSG_LOBBY_CHECK_NEWS_PUBLICATIONS_REPLY		 1597	// 4M(4<b>)	connection id, message body(vector of booleans)

//	PYR-29845
//	blitz lobby -> main lobby
#define BL_LOBBY_UPDATE_CLIENT_PROP					1598	//	441 - reqId,blitzId,clientProp
#define BL_LOBBY_UPDATE_CLIENT_PROP_REPLY			1599	//	4 - reqId

// PYR-26675: lobby -> IT client
#define MSG_LOBBY_NOTIFICATION_PGAD_IS_BACK			1600	// 2s - 0, infoText

// PYR-33970: lobby -> IT client
#define MSG_LOBBY_NOTIFICATION_USER_PUT_IN_PGAD_LIMITED_MODE 1602	// 2s - 0, infoText

// PYR-35043 MTL->Lobby
#define MSG_LOBBY_TOURN_SAG_UPDATE                  1603  // 4t tournId, closureTime
#define MSG_LOBBY_TOURN_SAG_UPDATE_REPLY            1604  // 

// PYR-26219 Lobby->client
#define MSG_LOBBY_CLI_RECONNECT_TABLE               1605  // 8ss , tableId, serverName, serverObject

//PYR-39272
#define SNG_LOBBY_RESOLVE_TARGET_REF                1606 // s - targetRef
#define SNG_LOBBY_RESOLVE_TARGET_REF_REPLY		    1607 // 2[s|4sT] - errCode[errInfo | targetTournId, targetServer, targetExpirationTime]
#define SNG_LOBBY_RESOLVE_TARGET_ID                 1608 // 4 - targetTournId
#define SNG_LOBBY_RESOLVE_TARGET_ID_REPLY		    1609 // 2[s|sT] - errCode[errInfo | targetServer, targetExpirationTime]

// PYR-39403
#define BL_TABLE_SIDEGAME_NEW_CARD_DEALT            1610  // see SIDEGAME_MSG_TABLE_EVENT_NEW_CARDS_DEALT in sidegamehandfeedprot.txt
// PYR-35954 - cleanup dead blitz
#define MSG_LOBBY_ADMIN_CLEANUP_DEAD_BLITZ          1611 //4s - reqId,blitzName
#define MSG_LOBBY_ADMIN_CLEANUP_DEAD_BLITZ_REPLY    1612 //46[s] - reqId,errCode[message]

// PYR-32587
#define MTL_TABLE_WINTHEBUTTON_MODE_OFF             1613 // empty 
// PYR-45646
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define BL_ADMIN_RESTART							1614	//44[S4] - reqId,sessionId[,i18nMsg,flags]
#define BL_ADMIN_RESTART_REPLY						1615	//46[s] - reqId,errCode,[errDescr]
#define MSG_LOBBY_TIMER_CREATE_BLITZ				1616 


// PYR-40999
#define MTL_ADMIN_LOBBY_REGISTER_USER_CHECK				1617 // 4sss - reqId, userId, country, province
#define MTL_ADMIN_LOBBY_REGISTER_USER_CHECK_REPLY		1618 // 46[s|4[s]] - reqId, errCode[errDesc|adminPromptFlags[promptMessage]]

// PYR-45646
// blitz lobby -> client
#define BL_BLITZ_RESTART_NOTIFICATION				1619	//empty

// PYR-44930
#define MSG_LOBBY_ADMIN_GET_ALL_SCHED_BROADCAST_MSG			1620 // 4 - reqId
#define MSG_LOBBY_ADMIN_GET_ALL_SCHED_BROADCAST_MSG_REPLY	1621 // 46[s|4<M(I44b4<4>4<4b>4<4>44<S>4<T>4<1>)>] - reqId, errCode[errDescr|numBroadcasts<M(message, flags, sites, tournOnly, numTourns<TournID>, numHandTypes<HandType, hiloFlag>, numBetStructs<BetStruct>, stakesMask, numCountries<Country>, numDatetimes<datetime>)>]

#define MSG_LOBBY_ADMIN_UPDATE_SCHED_BROADCAST_MSG			1622 // 44I44b4<4>4<4b>4<4>44<S>4<T>4<1> - reqId, broadcastID, message, flags, sites, tournOnly, numTourns<TournID>, numHandTypes<HandType, hiloFlag>, numBetStructs<BetStruct>, stakesMask, numCountries<Country>, numDatetimes<datetime>
#define MSG_LOBBY_ADMIN_UPDATE_SCHED_BROADCAST_MSG_REPLY	1623 // 46[s] - reqId, errCode[errDescr]

#define MSG_LOBBY_ADMIN_DELETE_SCHED_BROADCAST_MSG			1624 // 44 - reqId, broadcastID
#define MSG_LOBBY_ADMIN_DELETE_SCHED_BROADCAST_MSG_REPLY	1625 // 46[s] - reqId, errCode[errDescr]

// PYR-22908  (obsolete)
#define MTL_LOBBY_TABLE_SET_FINAL                    1626  // empty
#define MTL_LOBBY_TABLE_SET_FINAL_REPLY              1627  // 6[s] - errCode[errDescr]

// PYR-45462
#define MSG_LOBBY_MGLOBBY_SET_PUBL_PROPERTY				1628 // 44s - reqId, siteId, bannerParamsStr
#define MSG_LOBBY_MGLOBBY_SET_PUBL_PROPERTY_REPLY		1629 // 46[s] - reqId, errCode[errDescr]

// PYR-52524 deprecated
// PYR-52524 TODO - comment out once no longer referenced in coreserver
#define MSG_LOBBY_MGLOBBY_LIST_PUBL_PROPERTY			1630 // 44<4> - reqId, numSites, siteId
#define MSG_LOBBY_MGLOBBY_LIST_PUBL_PROPERTY_REPLY		1631 // 46[s|44t4<ss>] - reqId, errCode[errDescr|numSites, siteId, saveTime, numParams<name, value>]

// PYR-118579
#define MTL_TABLE_SIDEGAME_NEW_CARD_DEALT				1632  // see SIDEGAME_MSG_TABLE_EVENT_NEW_CARDS_DEALT in sidegamehandfeedprot.txt

// PYR-52524 TODO - remove once no longer referenced in coreserver
enum eMGLobbyBannerParametersType //PYR-48982
{
	eMGLobbyBannerParametersType_CardMatch = 0,
	eMGLobbyBannerParametersType_StarsRewards = 1,
};

// PYR-47770 lobby -> client
#define MSG_LOBBY_NOTIFICATION_PLAYINGTIME_UPDATE		1632 // 44 - playingTimeLimit (min), playingTime (sec)

// PYR-49465 table -> mtlobby
#define MTL_TABLE_APPLOGIN_UPDATE                       1633 // s8 - userId,appLoginId

// PYR-49826 table -> mtlobby
#define MTL_TABLE_DEAL_AVAILABILITY				1634 // b - autoDealSupported

#define MTL_LOBBY_UPDATE_STATS					1636 // 44 reqId, numMsgCheckErrors
#define MTL_LOBBY_UPDATE_STATS_REPLY			1637 // 4 reqId

// PYR-52984
#define MSG_LOBBYETC_SET_ICE_PUBL_PROPERTY				1638 // 44<s,8> - reqId, numParams<name,value>
#define MSG_LOBBYETC_SET_ICE_PUBL_PROPERTY_REPLY		1639 // 46[s] - reqId, errCode[errDescr]


// PYR-51560
#define MSG_LOBBY_ADMIN_GET_ARCHIVED_SCHED_BROADCAST_MSG			1640 // 4 - reqId
#define MSG_LOBBY_ADMIN_GET_ARCHIVED_SCHED_BROADCAST_MSG_REPLY	1641 // 46[s|4<M(I44b4<4>4<4b>4<4>44<S>4<T>4<1>)>] - reqId, errCode[errDescr|numBroadcasts<M(message, flags, sites, tournOnly, numTourns<TournID>, numHandTypes<HandType, hiloFlag>, numBetStructs<BetStruct>, stakesMask, numCountries<Country>, numDatetimes<datetime>)>]

// PYR-106264 personalization -> lobby
#define MSG_LOBBY_PS_GET_TABLES_INFO_BY_TABLE_IDS		1642 // 44<8> - reqId, size<tableId>
#define MSG_LOBBY_PS_GET_TABLES_INFO_BY_TABLE_IDS_REPLY	1643 // 46[s|4<table body>] - reqId, errCode [errDescr|size<table body>]

// PYR-57243 mtlobby -> lobby
#define MTL_LOBBY_OK_TO_START							1644 // 4 - reqId
#define MTL_LOBBY_OK_TO_START_REPLY						1645 // 46[s]

// PYR-61693
#define MSG_LOBBY_PS_USER_AT							1646 // 4s - reqId, userId
#define MSG_LOBBY_PS_USER_AT_REPLY						1647 // 46[s]

// PYR-66972 lobby -> mtlobby
#define MTL_LOBBY_TEST_CANCEL_RUNNING_TOURN				1648 // 44 - token,formula

// PYR-66972 mtlobby -> lobby
#define MTL_LOBBY_TEST_CANCEL_RUNNING_TOURN_RESULTS		1649 // 4842[s|4s4844bb44b44b484] - reqId,tournId,token,errcode[errstr|formula,currency,rmToPay,pmToPay,fppToPay,effectiveAddon,lateRegActive,started,playersRemaining,playersTotal,bountyPaid,prizeAddOn,largestIndividualRMPayout,inTheMoney,rmAlreadyPaid,pmAlreadyPaid,fppAlreadyPaid]
#define MTL_LOBBY_TEST_CANCEL_RUNNING_TOURN_RESULTS_REPLY	1650 // 46[s] - reqId, errCode[errDescr]

// PYR-66972 mtlobby -> lobby
#define MTL_LOBBY_CANCEL_RUNNING_TOURN_RESULTS			1651 // 4842[s] - reqId,tournId,token,errcode[errstr]
#define MTL_LOBBY_CANCEL_RUNNING_TOURN_RESULTS_REPLY	1652 // 46[s] - reqId, errCode[errDescr]

// PYR-66972 admin -> mtlobby
#define MTL_ADMIN_TEST_CANCEL_RUNNING_TOURN				1653 // 44 - reqId,formula
#define MTL_ADMIN_TEST_CANCEL_RUNNING_TOURN_REPLY		1654 // 42[s|8s4s4444<M(s4bb88888)>] - reqId,errcode[errstr|tournId,name,buyIn,currency,prizePool,totalChipsInPlay,formula,numPlayerRefunds<M(userId,chips,isPlayMoney,isFPPs,refundCalcProportionalPart,refundCalcEqualSplitPart,refundCalcKnockoutPart,refundCalcRakePart,totalRefund)>]

/////////////////////////////////////////////////////
#define MSG_TIMER_RELAY		5001

#define SHADOW_TOURN_INIT_DATA					3000
#define SHADOW_TOURN_INIT_DATA_REPLY			3001

#define SHADOW_TOURN_NOTIFY_APPEND				3002
#define SHADOW_TOURN_NOTIFY_REMOVE				3003
#define SHADOW_TOURN_NOTIFY_DYNA				3004
#define SHADOW_TOURN_NOTIFY_STATIC				3005
#define SHADOW_TOURN_NOTIFY_FLYER				3006
// PYR-20433
#define SHADOW_TOURN_NOTIFY_SITNGOS				3007 // 4<44b1111s44114[b<s>]> - size<scriptId,visibility,visibleInQuickLoby,game,structure,isHiLo,isPM,currency,buyin,rake,speed,playersPerTable,restrCountriesSize[restrictionInverted,<restrCountry>]>

// PYR-66899
#define SHADOW_SUBSCRIBE_ADDR_INIT				3008
#define SHADOW_SUBSCRIBE_ADDR_INIT_REPLY		3009
#define SHADOW_SUBSCRIBE_ADDR_NOTIFY			3010

#define SHADOW_SAG_TOURN_NOTIFY_APPEND			3011
#define SHADOW_SAG_TOURN_NOTIFY_REMOVE			3012
#define SHADOW_SAG_TOURN_NOTIFY_DYNA			3013
#define SHADOW_SAG_TOURN_NOTIFY_STATIC			3014
#define SHADOW_PSNG_TOURN_NOTIFY_APPEND			3015
#define SHADOW_PSNG_TOURN_NOTIFY_REMOVE			3016
#define SHADOW_PSNG_TOURN_NOTIFY_DYNA			3017
#define SHADOW_PSNG_TOURN_NOTIFY_STATIC			3018

#define SHADOW_TABLE_INIT_DATA					3100
#define SHADOW_TABLE_INIT_DATA_REPLY			3101

#define SHADOW_TABLE_NOTIFY_APPEND				3102
#define SHADOW_TABLE_NOTIFY_REMOVE				3103
#define SHADOW_TABLE_NOTIFY_DYNA				3104
#define SHADOW_TABLE_NOTIFY_FLYER				3105

#define SHADOW_TLB_INIT_DATA					3106
#define SHADOW_TLB_INIT_DATA_REPLY				3107
#define SHADOW_TLB_NOTIFY_UPDATE				3108

#define CLUB_TABLE_NOTIFY_TYPES					3109
// PYR-20432
#define SHADOW_TABLETYPES_NOTIFY				3110 // 4<4111144144s4444[b<s>]> - size<typeId,isPM,maxPlayers,game,isHiLo,lobet,hibet,structure,cap,visibility,currency,minBuyIn,maxBuyIn,ante,restrCountriesSize[restrictionInverted,<restrCountry>]>

#define SHADOW_SEAT_FINDERS_NOTIFY				3111 // 4<11114444s44444[b<s>]44444> - size<seatFinderId,game,structure,isHiLo,maxPlayers,loBet,hiBet,ante,cap,currency,maxBuyIn,maxBuyIn,visibilityMask,defaultBuyIn,numRestrictedCountries,[restrictionInverted,<restrCountry>],avgHandsPerHour,avgPot,avgStack,tables,players>
#define SHADOW_SEAT_FINDERS_UPDATE_NOTIFY		3112 // 4<444444> - size<seatFinderId,avgHandsPerHour,avgPot,avgStack,tables,players>
#define SHADOW_ONE_SEAT_FINDER_UPDATE_INSIGNIFICANT_STATIC_DATA	3113

//obsolete
//#define SHADOW_SEAT_FINDERS_MORPH_NOTIFY		3114 // 4<11114444s44444[b<s>]44444> - size<seatFinderId,game,structure,isHiLo,maxPlayers,loBet,hiBet,ante,cap,currency,maxBuyIn,maxBuyIn,visibilityMask,defaultBuyIn,numRestrictedCountries,[restrictionInverted,<restrCountry>],avgHandsPerHour,avgPot,avgStack,tables,players>
//#define SHADOW_SEAT_FINDERS_MORPH_UPDATE_NOTIFY	3115 // 4<444444> - size<seatFinderId,avgHandsPerHour,avgPot,avgStack,tables,players>
//#define SHADOW_ONE_SEAT_FINDER_MORPH_UPDATE_INSIGNIFICANT_STATIC_DATA	3116


#define CLUB_NOTIFY_USER_MOVED					3119 // s48bb userId,clubId,objectId,isTourn,isJoined
#define SHADOW_MLOBBYNG_INIT_DATA				3120
#define SHADOW_MLOBBYNG_INIT_DATA_REPLY			3121
#define SHADOW_SFTABLE_NOTIFY_APPEND			3122
#define SHADOW_SFTABLE_NOTIFY_REMOVE			3123
#define SHADOW_SFTABLE_NOTIFY_DYNA				3124

#define CLUB_SHADOW_INIT_DATA					3200
#define CLUB_SHADOW_INIT_DATA_REPLY				3201
#define CLUB_TOURN_NOTIFY_APPEND				3202
#define CLUB_TOURN_NOTIFY_REMOVE				3203
#define CLUB_TOURN_NOTIFY_DYNA					3204
#define CLUB_TOURN_NOTIFY_STATIC				3205
#define CLUB_TABLE_NOTIFY_APPEND				3206
#define CLUB_TABLE_NOTIFY_REMOVE				3207
#define CLUB_TABLE_NOTIFY_DYNA					3208
#define CLUB_TOURN_NOTIFY_CANCEL				3209
#define CLUB_TOURN_NOTIFY_RESTART_TIME			3210

// PYR-18024
#define SHADOW_RING_GAME_REPLAY_NOTIFY_APPEND       3211
#define SHADOW_RING_GAME_REPLAY_NOTIFY_REMOVE       3212

// PYR-18476
#define CLUB_GAME_ACTION_AUDIT 					3213 //44444ss game, action, gameId, subaction, clubId, userId, userIdToPublish, comment 

// PYR-97488 obsolete
//#define OBSOLETE								3214
//#define OBSOLETE								3215


#define SHADOW_BLITZ_INIT_DATA					3300
#define SHADOW_BLITZ_INIT_DATA_REPLY			3301
#define SHADOW_BLITZ_NOTIFY_APPEND				3302
#define SHADOW_BLITZ_NOTIFY_REMOVE				3303
#define SHADOW_BLITZ_NOTIFY_DYNA				3304
#define SHADOW_BLITZ_NOTIFY_STATIC				3305
#define SHADOW_BLITZ_NOTIFY_FLYER				3306

#define MSG_LOBBY_U_CRM_2_LOBBY_REPORT			3500 // 4sssss4 - reqId=0, userId, server, object, type, fileLocation, reportType
#define MSG_LOBBY_U_CLI_NOTIFY_REPORT			3501 // ssss4 - server, object, type, fileLocation, reportType

#define CHAT_USER_COMPL							4001 // 4s8ss4444<ss>4<s>s44 - reqId, complainerId, handId64, tableName, comments, tableId, tableLevel, locale, numLines<user, text>, numPlayers<user>, finalTableText, zoomReplayId, connId
#define CHAT_USER_COMPL_REPLY					4002 // 46s4 reqId, err_code, errDescr, connId
#define CHAT_COMPLAINER_MESSAGE					4003 // 4ss reqId, complainerId, text
#define CHAT_USER_WARNING_CHAT					4004 //4sss4s4ss4 - reqId,complainerId,offenderId,offenderText,tableId,tableText,tournId,adminId,adminComment,blitzId
#define CHAT_USER_REVOKE_CHAT					4005 //4sss4s4ss4 - reqId,complainerId,offenderId,offenderText,tableId,tableText,tournId,adminId,adminComment,blitzId
#define CHAT_USER_RESTORE_CHAT					4006 //4sss - reqId,offenderId,sdminId,adminComment
#define CHAT_USER_GET_TABLE_ADDRESS				4007 //44444 - reqId,claimId,tableId,tournId,blitzId
#define CHAT_USER_GET_TABLE_ADDRESS_REPLY		4008 // 44sss reqId, claimId, server, serverObject, serverChannel

//PYR-21295
#define SHADOW_TOURN_NOTIFY_I18NFLYER			4009
#define SHADOW_TABLE_NOTIFY_I18NFLYER			4010
#define SHADOW_BLITZ_NOTIFY_I18NFLYER			4011

//#22691
#define MSG_LOBBY_TGM_CREATED					4012
#define MSG_LOBBY_TGM_CREATED_REPLY				4013
#define MTL_TGM_ADD_TOURN_INFO					4014
#define MTL_TGM_ADD_TOURN_INFO_REPLY			4015
#define MSG_ADMIN_LOBBY_SHUTDOWN_TGM			4016 //4s4bb - reqId,groupName,type,restart,force
#define MSG_ADMIN_LOBBY_SHUTDOWN_TGM_REPLY		4017 //46[s] - reqId,errCode[errDescr]
#define MSG_LOBBY_TGM_SHUTDOWN					4018
#define MSG_LOBBY_TGM_SHUTDOWN_REPLY			4019
#define MTL_TGM_REGISTER_USER_QUERY				4020
#define MTL_TGM_REGISTER_USER_QUERY_REPLY		4021
#define MTL_TGM_REGISTER_USER					4022
#define MTL_TGM_REGISTER_USER_REPLY				4023

#define MTL_TGM_UPDATE_FLIGHT_INFO				4024
#define MTL_TGM_UPDATE_FLIGHT_INFO_REPLY		4025
#define TGM_MTL_UPDATE_INFO_FROM_FLIGHT			4026

#define MTL_TGM_SEND_GLOBAL_FLIGHT_INFO			4027
#define MTL_TGM_SEND_GLOBAL_FLIGHT_INFO_REPLY	4028
#define TGM_MTL_RECEIVE_GLOBAL_FLIGHT_INFO		4029

#define MTL_TGM_REGISTER_USER_NOTIFY			4030 // 444s1[44] - reqId, tgmType, userIntId, userId, reg [satellietId, numReentries]
#define MTL_TGM_REGISTER_USER_NOTIFY_REPLY		4031 // 46[s]
// Final flight finished - close the TGM
#define MTL_TGM_FINAL_CLOSED					4032
#define MTL_TGM_FINAL_CLOSED_REPLY				4033

#define MTL_TGM_SEND_TOTAL_FLIGHT_TO_NEXT_LEVEL			4034 // 44O4 - reqId, tgmType, FlightTournInfo, targetId
#define MTL_TGM_SEND_TOTAL_FLIGHT_TO_NEXT_LEVEL_REPLY	4035 // 46[s] - reqId, errCode[errDescr]
//#define TGM_MTL_RECEIVE_TOTAL_FLIGHT_FROM_PREV_FLIGHT	4036 // 4O4 - reqId, FlightTournInfo, satelliteId

#define MTL_TGM_USERS_OUT						4037
#define MTL_TGM_USERS_OUT_REPLY					4038

#define MSG_LOBBY_TGM_CLOSED_NOTIFY				4039
#define MSG_LOBBY_TGM_CLOSED_NOTIFY_REPLY		4040

#define TGM_MTL_TAKE_SATELLITE_WINNERS			4041

#define MTL_TGM_REGISTER_SAT_WINNERS			4042 // 44M - reqId, tgmType, M(Dbm request body)
#define MTL_TGM_REGISTER_SAT_WINNERS_REPLY		4043 // 46[s|4<s>O] - reqId, errCode[errDescr|4<userId>,correctedSatWinners]

#define MTL_TGM_REMOVE_TOURN_INFO				4044
#define MTL_TGM_REMOVE_TOURN_INFO_REPLY			4045

#define TGM_MTL_RECEIVE_LOCAL_FLIGHT_INFO		4046
#define TGM_MTL_RECEIVE_TARGET_FLIGHT_INFO		4047

#define MTL_TGM_REMOVE_CANCELLED_LOSERS			4048
#define MTL_TGM_REMOVE_CANCELLED_LOSERS_REPLY	4049
#define TGM_MTL_REMOVE_CANCELLED_LOSERS			4050

#define MSG_LOBBY_GET_PLAYERS_VPP				4051	//44 reqId, tableReqId
#define MSG_LOBBY_GET_PLAYERS_VPP_REPLY			4052	//44t1<s,4> - reqId, tableReqId, timestamp,size<userId,VPP>

#define MSG_ADMIN_LOBBY_GET_JH_SIMILAR_PLAYERS_VPP			4053 //48b -reqId, tableId, test
#define MSG_ADMIN_LOBBY_GET_JH_SIMILAR_PLAYERS_VPP_REPLY	4054 //46[s|4<8t4<s4>>] - reqId, errCode[errDesc|numTables<tableId, timestampt, numPLayers< userId, vpp> >]

#define MSG_LOBBY_TIMER_JUBILEE_HAND_READY					4055	//8 tableId
#define MSG_ADMIN_LOBBY_SHARED_STAKES_USER_NOTIFY			4056	//4s4s4 - reqId, tableName, totalAmount, userId, amount
#define MSG_ADMIN_LOBBY_SHARED_STAKES_USER_NOTIFY_REPLY		4057	//46[s] - reqId, errCode[errDescr]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_ADMIN_LOBBY_SHARED_STAKES_TABLE_NOTIFY			4058	//4s48 - reqId, tableName, totalAmount, tableId
#define MSG_ADMIN_LOBBY_SHARED_STAKES_TABLE_NOTIFY_REPLY	4059	//46[s] - reqId, errCode[errDescr]

#define MSG_ADMIN_LOBBY_REMOVE_FLIGHT_USER			4060	//44ss -reqId, tournamentId, userId, comment
#define MSG_ADMIN_LOBBY_REMOVE_FLIGHT_USER_REPLY	4061	//46[s] -reqId, errCode[errDescr]

#define MTL_LOBBY_REMOVE_FLIGHT_USER			4062	//44ss -reqId, tournamentId, userId, comment
#define MTL_LOBBY_REMOVE_FLIGHT_USER_REPLY		4063	//46[s] -reqId, errCode[errDescr]
#define MTL_LOBBY_REMOVE_FLIGHT_USER_REPLY_REPLY 4064	//4 -reqId

#define MSG_LOBBY_TGM_REMOVE_FLIGHT_USER			4065	//44ss -reqId, tournamentId, userId, comment
#define MSG_LOBBY_TGM_REMOVE_FLIGHT_USER_REPLY		4066	//46[s] -reqId, errCode[errDescr]

// PYR-26155
// admin->lobby
#define MSG_ADMIN_LOBBY_CLI_POPUP_TO_CONFIRM		4067	//44<s>44 - reqId,numUsers<userId>,templateId,tableId /* table Id can be 0 */
#define MSG_ADMIN_LOBBY_CLI_POPUP_TO_CONFIRM_REPLY	4068	//46[s | 4<s6s> ] - reqId,errCode [ errDescr | numErrors<userId,errCode,errDescr> ]
// client->lobby
#define MSG_CLI_LOBBY_CONFIRM_POPUP					4069	//4b - confirmCode, understood
// lobby->client
#define MSG_LOBBY_CLI_POPUP_TO_CONFIRM_NOTIFY		4070	//44s8 - confirmCode, tableId32, popUpMessage, tableId64

//	PYR-25263
//	LobbyEtc->Lobby
#define MSG_LOBBYETC_GET_DATA						4071	//	4 - reqId
#define MSG_LOBBYETC_GET_DATA_REPLY					4072	//	46[s|4<s4>2<224>2<224>] - reqId,errCode [ errDescr | arraySize<userid,licenseId>,rh_cfg_size,<rh_threshold, rh_minutes, rh_sites>, zoom_cfg_size,<zoom_threshold, zoom_minutes, zoom_sites>]

// obsolete by PYR-91342
//#define MSG_LOBBYETC_GA_POOL_STATUS_CHANGE				4073	//	44b - reqId, siteId, isAvailable
//#define MSG_LOBBYETC_GA_POOL_STATUS_CHANGE_REPLY		4074	//	46[s] - reqId,errCode [ errDescr ]

#define MTL_TGM_SET_AAMS_SESSION_STATUS					4075	//44<s> - reqId, tgmtype
#define MTL_TGM_SET_AAMS_SESSION_STATUS_REPLY			4076	//46[s|4<s4>] - reqId, errCode[errMsg]

// obsolete by PYR-91342
//#define MSG_LOBBYETC_GA_KICK_UNREGISTGER_LOGOUT_USER		4077		//	44<s> - reqId,vectorSize<userIds>
//#define MSG_LOBBYETC_GA_KICK_UNREGISTGER_LOGOUT_USER_REPLY	4078		//	46[s] - reqId,errCode [ errDescr ]

//PYR-25970
#define MSG_ADMIN_LOBBY_GET_TGMS							4079	//	44b - reqId,type,activeOnly
#define MSG_ADMIN_LOBBY_GET_TGMS_REPLY						4080	//	46[ s| 4M<44stt4st44t>] - reqId, errCode [ errMsg| num<tgmId, type, groupName, start, end,maxEntry, adminId, created, tgmFlags, period, periodStart>]
#define MSG_ADMIN_LOBBY_CREATE_TGM							4081	//	44stt444t - reqId, type, groupName, startTime, endTime, maxentry, tgmFlags, period, periodStart
#define MSG_ADMIN_LOBBY_CREATE_TGM_REPLY					4082	//	46[ s| 4] - reqId, errCode [ errMsg| tgmId ]
#define MSG_ADMIN_LOBBY_UPDATE_TGM							4083	//	444stt444t - reqId, tgmId, type, groupName, startTime, endTime, maxentry, tgmFlags, period, periodStart
#define MSG_ADMIN_LOBBY_UPDATE_TGM_REPLY					4084	//	46[ s ] - reqId, errCode [ errMsg ]
#define MSG_ADMIN_LOBBY_DELETE_TGM							4085	//	444 - reqId, tgmId, type
#define MSG_ADMIN_LOBBY_DELETE_TGM_REPLY					4086	//	46[ s ] - reqId, errCode [ errMsg ]
#define MSG_LOBBY_TGM_UPDATE								4087	//	4tt4 - reqId, start, end, maxEntry
#define MSG_LOBBY_TGM_UPDATE_REPLY							4088	//	46[ s ] - reqId, errCode [ errMsg ]
#define MSG_LOBBY_TGM_DELETE								4089	//	4 - reqId
#define MSG_LOBBY_TGM_DELETE_REPLY							4090	//	46[ s ] - reqId, errCode [ errMsg ]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_ADMIN_LOBBY_ALLOW_ONE_MORE_ENTRY				4091	//	4444<s> - reqId, tgmId, type, tgmnumUsers<userId>
#define MSG_ADMIN_LOBBY_ALLOW_ONE_MORE_ENTRY_REPLY			4092	//	46[ s|4<ss> ] - reqId, errCode [ errMsg | numFailedUsers< userId, errMsag> ]
#define MSG_ADMIN_LOBBY_REMOVE_TOURN_FROM_GROUP				4093	//	4444 - reqId, tgmId, type, tournId
#define MSG_ADMIN_LOBBY_REMOVE_TOURN_FROM_GROUP_REPLY		4094	//	46[ s ] - reqId, errCode [ errMsg ]

// PYR-26568 LobbyEtc->Lobby
#define MSG_LOBBYETC_LOBBY_USER_CAN_CHANGE_IMAGE			4095		//	44s - reqId,userId
#define MSG_LOBBYETC_LOBBY_USER_CAN_CHANGE_IMAGE_REPLY		4096		//	46[s] - reqId,errCode [ errDescr ]

#define MSG_LOBBY_TGM_ALLOW_ONE_MORE_ENTRY					4097	//	4444<s> - reqId, tgmId, type, tgmnumUsers<userId>
#define MSG_LOBBY_TGM_ALLOW_ONE_MORE_ENTRY_REPLY			4098	//	46[ s|4<ss> ] - reqId, errCode [ errMsg | numFailedUsers< userId, errMsag> ]
#define MSG_LOBBY_TGM_REMOVE_TOURN_FROM_GROUP				4099	//	4444 - reqId, tgmId, type, tournId
#define MSG_LOBBY_TGM_REMOVE_TOURN_FROM_GROUP_REPLY			4100	//	46[ s ] - reqId, errCode [ errMsg ]

#define MTL_TGM_SET_CLOSED									4101	//	44 - reqId, type
#define MTL_TGM_SET_CLOSED_REPLY							4102	//	46[ s ] - reqId, errCode [ errMsg ]

#define MSG_LOBBY_TGM_PREREGISTER_USERS						4103
#define MSG_LOBBY_TGM_PREREGISTER_USERS_REPLY				4104

#define LOBBY_SAG_ENABLE									4105	// 42 -reqId, enabled
#define LOBBY_SAG_ENABLE_REPLY								4106	//	46[ s|2 ] - reqId, errCode [ errMsg|enabled ]

#define MSG_LOBBY_ADMIN_NEW_SAG								4107	// 4ss4<IniFile> reqId, title, filename, n<PayoutTables>, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_NEW_SAG_REPLY						4108	//reqId, 6[s] errCode, [errDescr]

#define MSG_LOBBY_ADMIN_UPDATE_SAG							4109	// 4ss4<IniFile> reqId, title, fileName, n<PayoutTables>, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_UPDATE_SAG_REPLY					4110	//reqId, 6[s] errCode, [errDescr]

#define MSG_LOBBY_ADMIN_DELETE_SAG							4111	// 4s4<IniFile> reqId, title, n<PayoutTables>, nFiles<iniFile>
#define MSG_LOBBY_ADMIN_DELETE_SAG_REPLY					4112	//reqId, 6[s] errCode, [errDescr]

#define MSG_LOBBY_ADMIN_QUERY_SAG							4113	// 4s reqId
#define MSG_LOBBY_ADMIN_QUERY_SAG_REPLY						4114	//reqId, 6[s|4<s>] errCode, numTitles<title>

// PYR-25565
#define MTL_USER_DEAL_NEGOTIATE								4115	// 4 - dealGeneration                                                      
#define MTL_USER_DEAL_NEGOTIATE_REPLY						4116	// 6[s|b] - errCode [errMessage | youAreDealManagerNow]    -- see AutoDealErrorCode for error codes sent in the reply
#define MTL_USER_DEAL_NEGOTIATE_CONFIRM						4117	// 4 - dealGeneration
#define MTL_USER_DEAL_NEGOTIATE_CONFIRM_REPLY				4118	// 6[s] - errCode [errMessage]
#define MTL_USER_DEAL_SUBMIT_PROPOSAL						4119	// 414<7>7 - dealGeneration, dealType, customDealPayments<payment>, customDealLeftForFirst
#define MTL_USER_DEAL_SUBMIT_PROPOSAL_REPLY					4120	// 6[s] - errCode [errMessage]
#define MTL_USER_DEAL_PASS_CONTROL							4121	// 4s - dealGeneration, targetUser
#define MTL_USER_DEAL_PASS_CONTROL_REPLY					4122	// 6[s] - errCode [errMessage]
#define MTL_USER_DEAL_UPDATE_PROPOSAL						4123	// 44<17>111 - dealGeneration, updateCustomPayment<position,value>, updateDealType, cursorLine, cursorColumn    
                                                                                   // use position=-1 for leaveFor1stPayment and 0..(players-1) for other payments
                                                                                   // use updateDealType=0 if deal is not changed
                                                                                   // use cursorLine, cursorColumn = -1 if you do not want to use them
#define MTL_USER_DEAL_UPDATE_PROPOSAL_REPLY					4124	// 6[s] - errCode [errMessage]
#define MTL_USER_DEAL_AGREE									4125	// 41 - dealGeneration, subcommand             // subcommand: eSet, eReset, eQuery as at AutoDealAgreeSubcommand
#define MTL_USER_DEAL_AGREE_REPLY							4126	// 6[s|b1] - errCode [errMessge | agreeCurState, playersAgreedNumber]
#define MTL_USER_DEAL_DECLINE								4127    // 4 - dealGeneration
#define MTL_USER_DEAL_DECLINE_REPLY							4128	// 6[s] - errCode [errMessage]
#define MTL_USER_DEAL_CALL_SUPPORT							4129    // 4 - dealGeneration
#define MTL_USER_DEAL_CALL_SUPPORT_REPLY 					4130	// 6[s|4S] - errCode [errMessage|result,i18popupMessage] 
#define MTL_ADMIN_AUTO_DEAL_INTERRUPT						4131    // 41    - reqId, mode
#define MTL_ADMIN_AUTO_DEAL_INTERRUPT_REPLY					4132	// 46[s] - reqId, errCode [errMessage]

#define SAG_MTL_CANCEL_TOURN								4133	// 44s - reqId, tournId, msg
#define MTL_SAG_TOURN_CANCELLED_NOTIFY						4134	// 442 - reqId, tournId, running
#define MTL_SAG_TOURN_CANCELLED_NOTIFY_REPLY				4135	// 4 - reqId
#define MSG_SAG_TOURN_REG_INFO_BY_SCRIPTID					4136	// 414 - sagScriptId, numGames, TournRegInfoFlags
#define MTL_SAG_TOURN_CREATED								4137	// 444444s41111444<4s4> - reqId, tournId, buyin, rake, maxPlayers, tournSites, currency, scalePm, game, isHilo, structure, speed, status, statusBits, numUsers<userIntId, userId, imageId>
#define MTL_SAG_TOURN_CREATED_REPLY							4138	// 46[s|b4] - reqId, errCode [errMsg|psngSample,sagMaxAllinDelay]
#define SAG_LOBBY_CREATE_TOURNAMENT							4139	// 4o444 - reqId, script, payoutId, minPP, maxPP
#define SAG_LOBBY_CREATE_TOURNAMENT_REPLY					4140	// 46[s|4] - reqId, errCode [errMsg|tournId]
#define MTL_SAG_UNREGISTER_USER_NOTIFY						4141	// 444<s4>] reqId, tournId, userCount, <userId, userIntId> 
#define MTL_SAG_UNREGISTER_USER_NOTIFY_REPLY				4142	// 4 - reqId
#define MTL_SAG_GET_DRAW_RESULTS							4143	// 44 - reqId, tournId
#define MTL_SAG_GET_DRAW_RESULTS_REPLY						4144	// 46[2|4ss44444] - reqId, errCode [ errStr| prizePool, awards, prizePoolsStr, level, payoutId, minPrizePool, maxPrizePool, totalBuyIn ]
#define SAG_MTL_DUMMY_REPLY									4145	// 4 - reqId
#define MTL_SAG_TOURN_FINISHED_NOTIFY						4146	// 444<s444> reqId, tournId, winnerCount,<userId,userIntId,userImageId,award>
#define MTL_SAG_TOURN_FINISHED_NOTIFY_REPLY					4147	// 4 - reqId
#define MSG_SAG_TOURNS_REGISTRATION_SUMMARY					4148	// 6[s|4<6s4s>] - errCode[ errMsg| num <errCode, errMsg, tournId, srvName> ]
#define MSG_SAG_REGISTER_TOURNAMENT_USER					4149	// s444s144M - userId, buyInRM, buyInPM, buyInFpp, pwd, useTicket, buyInTM, buyInWM, currencyContext
#define MSG_SAG_REGISTER_TOURNAMENT_USER_REPLY				4150	// 6s[6s4bssMs] - errCode, errMsg [allInOption, locale, tournId, seatAvail, servrName, socialToken, whereIsBody, aamsTicket]
#define MSG_ADMIN_SAG_STOP_REGISTRATION						4151	
#define MSG_ADMIN_SAG_STOP_REGISTRATION_REPLY				4152
#define MSG_ADMIN_SAG_RESUME_REGISTRATION					4153
#define MSG_ADMIN_SAG_RESUME_REGISTRATION_REPLY				4154
#define MSG_SAG_REGISTER_TOURNAMENT_USER_BY_SCRIPTID		4155	// 41 - sagScriptId, numGames
#define MTL_LOBBY_GROUP_REGISTER_USER						4156// // s444s44414sss444sbs88 - userName, buyInRM, buyInPlay, buyInFpp, tournPwd, cameFromTourn, buyInT, buyInW, useTicket, userIntId, country, province, ipAtReg, flags, originalIntId, creditAmount, creditCurrency, creditTMoney, creditComment, flags2, privileges2
#define LOBBY_SAG_FACTORY_STATUS							4157	//44b - reqId, factoryId, status
#define LOBBY_SAG_FACTORY_STATUS_REPLY						4158	//46[s] - reqId, errCode [errStr]
#define LOBBYETC_SAG_GAME_REQUEST							4159	// 4{PromoDealSagRequest} - regId, {PromoDealSagRequest}
#define LOBBYETC_SAG_GAME_REQUEST_REPLY						4160	//46[s|8]- reqId, errCode [errStr|SagScriptId]
//PYR-28842
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_RESUME_RESURRECTED_TOURS			4161// 444bb4 reqId, tournFlag, siteId, regular, guaranteed, minutes
#define MSG_LOBBY_ADMIN_RESUME_RESURRECTED_TOURS_REPLY		4162 // 46[s|4] reqId errCode, [errDescr|count]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_ROLLFORWARD_RESURRECTED_TOURS		4163 // 444bb4s reqId, tournFlag, siteId, regular, guaranteed, minutes, txtMsg
#define MSG_LOBBY_ADMIN_ROLLFORWARD_RESURRECTED_TOURS_REPLY	4164 // 46[s|4] reqId errCode, [errDescr|count]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_GET_UNHANDLED_RESURRECTED_TOURS		4165 // 444bb44 reqId, tournFlag, siteId, regular, guaranteed, minutesMin, minutesMax
#define MSG_LOBBY_ADMIN_GET_UNHANDLED_RESURRECTED_TOURS_REPLY 4166 // 46[s|4<4stb44444s>] reqId errCode, [errDescr|count<tournId, tournName, startTime, playMoney, status, buyIn, guaranteedPrizePool, siteId, currency>]
// PYR-27091 LobbyEtc->Lobby
#define MSG_LOBBYETC_LOBBY_REG_DEAL_USER					4167 // 4{DealUserReg}, reqId, DealUserReg
#define MSG_LOBBYETC_LOBBY_REG_DEAL_USER_REPLY				4168 // 46sb, reqId, errCode, errDescr, retry

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_GET_UNHANDLED_RESURRECTED_TOURS2		4169 // 444bb44 reqId, tournFlag, siteId, regular, guaranteed, minutesMin, minutesMax
#define MSG_LOBBY_ADMIN_GET_UNHANDLED_RESURRECTED_TOURS2_REPLY	4170 // 46[s|4<M(4stb4444ms)>] reqId errCode, [errDescr|count<tournId, tournName, startTime, playMoney, status, buyIn, guaranteedPrizePool, siteMask, currency>]

#define SAG_LOBBY_PROPERTIES_UPDATE							4171 // 44<448s8s84> - reqId, num< marketId, flags, countup, runningTourn, buyIn, currency,prizePool, maxPlayed>
#define SAG_LOBBY_PROPERTIES_UPDATE_REPLY					4172 // 46[s]

//PYR-30480
// lobby -> client: password change reminder
#define MSG_LOBBY_CLI_PWD_CHANGE_REMINDER					4173 //  - empty body
// CC->Lobby: password change reminder
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_PWD_CHANGE_REMINDER						4174 // 4s reqId,userId
#define MSG_LOBBY_PWD_CHANGE_REMINDER_REPLY					4175 // 46[s] reqId,errCode,[errDescr]

//PYR-27091
// admin<->lobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_MASS_RESUME_RESURRECTED_TOURNS			4176 // 44<4> reqId, tournNum, <tournId> 
#define MSG_LOBBY_ADMIN_MASS_RESUME_RESURRECTED_TOURNS_REPLY	4177 // 46[s] reqId errCode, [errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_MASS_ROLLBACK_RESURRECTED_TOURNS		4178 // 44s4<4> reqId, rollback/rollforward formula, message, tournNum, <tournId> 
#define MSG_LOBBY_ADMIN_MASS_ROLLBACK_RESURRECTED_TOURNS_REPLY	4179 // 46[s] reqId errCode, [errDescr]

//	PYR-28732.  lobby -> client.  Prompt the user to setup their challenge questions
#define MSG_LOBBY_CLI_SETUP_CHALLENGE_QUESTIONS_PROMPT		4180	//	- empty body

// PYR-28732.  CC -> Lobby
#define MSG_LOBBY_SETUP_CHALLENGE_QUESTIONS_PROMPT			4181	// 4s reqId,userId
#define MSG_LOBBY_SETUP_CHALLENGE_QUESTIONS_PROMPT_REPLY	4182	// 46[s] reqId,errCode,[errDescr]

// PYR-27091 LobbyEtc->Lobby
#define MSG_LOBBYETC_LOBBY_REG_RG_DEAL_USER					4183 // 4{RGDealUserReg}, reqId, RGDealUserReg
#define MSG_LOBBYETC_LOBBY_REG_RG_DEAL_USER_REPLY			4184 // 46[s|8], reqId, errCode, [errDescr|tableId]

// PYR-28735
#define MSG_LOBBY_CLI_FORCE_REAUTH							4185 // 4s			reqId, userId
// AVAIL 4186
// PYR-30436
#define MTL_TGM_CANCEL_RUNNING_TOURN						4187 // see DBM_Q_CANCEL_RUNNING_TOURN
#define MTL_TGM_CANCEL_RUNNING_TOURN_REPLY					4188 // see DBM_A_CANCEL_RUNNING_TOURN
// AVAIL 4189

//lobby to client 
#define MSG_TAKE_THIS_SEAT									4190 // ss814 - server, serverObject, tableId64, seat, price

// PYR-30436
#define MTL_TGM_SET_FINAL_FLIGHT_END_DATE					4191 // 44 - reqId, tournId
#define MTL_TGM_SET_FINAL_FLIGHT_END_DATE_REPLY				4192 // 46[s] - reqId, errCode[errDescr]

// PYR-27243: lobbyetc -> lobby 
#define MSG_LOBBYETC_USER_GEOIP_UNKNOWN						4193 // 4s - reqId, userId
#define MSG_LOBBYETC_USER_GEOIP_UNKNOWN_REPLY				4194 // 4 - reqId

// PYR-27243: notif message: lobby -> table, blitz lobby, mtlobby
#define MSG_LOBBY_NOTIF_USER_GEOIP_UNKNOWN					4196 // 4s - reqId (0), userId

// PYR-27243: notif message: mtlobby -> table
#define MTL_TABLE_NOTIF_USER_GEOIP_UNKNOWN					4198 // 4s - reqId (0), userId

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_MASS_CANCEL_RESURRECTED_TOURNS		 4199 // 44<4> reqId, tournCount, <tournIds> 
#define MSG_LOBBY_ADMIN_MASS_CANCEL_RESURRECTED_TOURNS_REPLY 4200 // 46[s] reqId errCode, [errDescr]

// PYR-28673: notif message": Lobby/MTL/BL -> tables
#define MSG_TABLE_NOTIF_ACM_CONFIG_UPDATE                   4201 // 4ssm - reqId (0), acmServerAddr, acmServerInstance, msgBody
#define MTL_TABLE_NOTIF_ACM_CONFIG_UPDATE                   4202 // ssm - acmServerAddr, acmServerInstance, msgBody
#define BL_TABLE_NOTIF_ACM_CONFIG_UPDATE                    4203 // ssm - acmServerAddr, acmServerInstance, msgBody
#define MTL_LOBBY_NOTIF_ACM_CONFIG_UPDATE                   4204 // 4ssm - reqId (0), acmServerAddr, acmServerInstance, msgBody
#define BL_LOBBY_NOTIF_ACM_CONFIG_UPDATE                    4205 // 4ssm - reqId (0), acmServerAddr, acmServerInstance, msgBody

// PYR-28673: notif message": Lobby -> ACM
#define MSG_ACM_LOBBY_INIDATA_UPDATE                        4206 // 4m - reqId(0), msgBody
#define MSG_ACM_LOBBY_INIDATA_UPDATE_REPLY                  4207 // 4m - reqId, msgBody
#define MSG_ACM_LOBBY_INIDATA_UPDATE_REPLY_ACK              4208 // 4 - reqId

// PYR-28673: notif messge: Lobby -> ACM
//#define MSG_ACM_LOBBY_DOMAINREGEX_UPDATE                    4209 // 4m - reqId(0), msgBody
//#define MSG_ACM_LOBBY_DOMAINREGEX_UPDATE_REPLY              4210 // 4m - reqId, msgBody
//#define MSG_ACM_LOBBY_DOMAINREGEX_UPDATE_REPLY_ACK          4211 // 4 - reqId

// PYR-28673: update user chat ban status, ACM -> Lobby
#define MSG_LOBBY_ACM_CHATBAN_UPDATE						4212 // 4s44481s8 - reqId, userId, userIntId, priv, flags, priv2, chatBannedOrNot, comment, priv3
#define MSG_LOBBY_ACM_CHATBAN_UPDATE_REPLY					4213 // 46[s] - reqId, errCode [errStr]

// PYR-30555 client to lobby
#define MSG_LOBBY_GET_SIMILAR_SATELLITES					4214 // 4 tournId
#define MSG_LOBBY_GET_SIMILAR_SATELLITES_REPLY				4215 // 6[s|4<M(4t1111s4444444844ss)>] - errCode [errStr|numSatellites<satBody(tournId, startTime, game, stryct, hiLo, isPlayMoney, currency, scalePM, buyIn, rake, knockout, status, statusBits, tournFlags, tournFlags2, maxPlayers, numPlayers, serverAddr, serverName)>]

#define MSG_ICE_LOBBY_REGISTER_TOURN_USER					4216 //
#define MSG_ICE_LOBBY_REGISTER_TOURN_USER_REPLY				4217 //6[s|2s4bss] errCode [ errDescr ]

#define MTL_LOBBY_REGISTER_USER_ICE							4218 // s444s44414sss444sbs88 - iceReqId, userName, buyInRM, buyInPlay, buyInFpp, tournPwd, cameFromTourn, buyInT, buyInW, useTicket, userIntId, country, province, ipAtReg, flags, originalIntId, creditAmount, creditCurrency, creditTMoney, creditComment, flags2, privileges2
#define MTL_LOBBY_REGISTER_USER_ICE_REPLY					4219 // 6[s] err_code [errDescr]

#define MTL_TGM_GET_AAMS_NUMBER_OF_ENTRY_FLIGHTS			4220 //44 , reqId, tgmType
#define MTL_TGM_GET_AAMS_NUMBER_OF_ENTRY_FLIGHTS_REPLY		4221 //41 , reqId, aamsNumEntryFlights

#define MSG_LOBBY_TGM_REDEPLOYMENT_CHECK					4222
#define MSG_LOBBY_TGM_REDEPLOYMENT_CHECK_REPLY				4223

// PYR-32120 lobbyetc to lobby for webclient/client exclude
#define MSG_LOBBYETC_EXCLUDE_PLAYER							4224 //4s448b8 - reqId, username,days_to_exclude,flags,flags2,groupSelfExclusion, appSessionId
#define MSG_LOBBYETC_EXCLUDE_PLAYER_REPLY					4225 //46[s|4T4] - reqId, errCode [errDescr|days excluded, until, days requested]

// PYR-32121 lobbyetc to lobby for webclient time zone
#define MSG_LOBBYETC_LOBBY_TIMEZONE_UPDATE					4226 //	4s4 - reqId,userId, timezone
#define MSG_LOBBYETC_LOBBY_TIMEZONE_UPDATE_REPLY			4227 //	46[s] - reqId,errCode [ errDescr ]

// PYR-32547
#define MSG_LOBBY_TS_TEMPLATE_UPDATE_NOTIFY				    4228 //	41s444s444 - reqId=0,operation,origName,origType,origSubType,origStatus,newName,newType,newSubType,newStatus

// PYR-33537
#define MSG_LOBBY_ADMIN_SET_HIDE_CASINO_GAMES_FLAGS2		4230 // 4sb   reqId, userName, setNotRemove
#define MSG_LOBBY_ADMIN_SET_HIDE_CASINO_GAMES_FLAGS2_REPLY	4231 // 46[s] reqId, err [errDescr] 

//PYR-32003
#define MTL_TGM_UPDATE_STATUS								4232 // 4444   reqId, tgmType, tournId, status
#define MTL_TGM_UPDATE_STATUS_REPLY							4233 // 46[s] reqId, err [errDescr] 

//PYR-32003
#define TGM_MTL_SAME_LEVEL_FLIGHTS_AVAILABLE				4234 // 42   reqId, flightsAvailable

//PYR-32003
#define MSG_MTL_GET_SAME_LEVEL_FLIGHTS						4235 // 4  tournId
#define MSG_MTL_GET_SAME_LEVEL_FLIGHTS_REPLY				4236 // 6[s|4<4t4>] err [errDescr|numTournd<tournId,startTime,numEnrolled>] 

//PYR-32003
#define MSG_MTL_CHANGE_FLIGHT								4237 // tournId
#define MSG_MTL_CHANGE_FLIGHT_REPLY							4238 // 6[s] err [errDescr] 

//PYR-32003
#define MTL_TGM_GET_SAME_LEVEL_FLIGHTS						4239 // 44   reqId, tournId
#define MTL_TGM_GET_SAME_LEVEL_FLIGHTS_REPLY				4240 // 46[s|44<4t4>] reqId, err [errDescr|level,numTournd<tournId,startTime,numEnrolled>] 

//PYR-32003
#define MTL_TGM_MOVE_FLIGHT_USER							4241 // 4444s tournIdFrom, TournIdTo, userIntId, userId
#define MTL_TGM_MOVE_FLIGHT_USER_REPLY						4242 // 6[s] err [errDescr] 

#define TGM_MTL_INJECT_FLIGHT_USER_NOTIFY					4243 // 4M() reqId,M(userData)
#define TGM_MTL_INJECT_FLIGHT_USER							4244 // 4 reqId, err [errDescr] 
#define TGM_MTL_INJECT_FLIGHT_USER_REPLY					4245 // 4 reqId, err [errDescr] 
#define TGM_MTL_STUCK_USER_NOTIFY							4246 // 4 reqId

//PYR-35875
// LobbyEtc -> Lobby
#define MSG_LOBBYETC_TOURN_STATS							4247 // 41s44844s reqId, requestType(see dbm.h for details), userName, tournIdOrNumTourns, flags, flags2, locale, clientSiteId, email
#define MSG_LOBBYETC_TOURN_STATS_REPLY						4248 // 6[s] errCode, [errDescr|email]

//PYR-119274
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_GET_OBJ_STATE_EX					4249 //414 - reqId,objType,statusMask
#define MSG_LOBBY_ADMIN_GET_OBJ_STATE_EX_REPLY				4250 //44<M(4s114411111m4)>4<M(41114444441144m448)>4<M(4s114411144m4)> - reqId, NumTables<M(tableId,tableName,game,isHiLo, hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId)>, NumTourn<M(tournId,game,isHiLo,structure,buyin,status,enrolled,numberUsers,numberRealUsers,userDisconncted,dealingStatus,isPlayMoney,fppBuyIn,tournFlags,siteMask,knockout,clubId,tournFlags2)>, NumBlitz<M(blitzId,blitzName,game,isHiLo,hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId)>

// PYR-36408
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_GET_OBJ_STATE						4251 //414 - reqId,objType,statusMask
#define MSG_LOBBY_ADMIN_GET_OBJ_STATE_REPLY					4252 //44<4s11441111144>4<41114444441144444>4<4s11441114444> - reqId, NumTables<tableId,tableName,game,isHiLo, hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId>, NumTourn<tournId,game,isHiLo,structure,buyin,status,enrolled,numberUsers,numberRealUsers,userDisconncted,dealingStatus,isPlayMoney,fppBuyIn,tournFlags,siteMask,knockout,clubId>, NumBlitz<blitzId,blitzName,game,isHiLo,hibet,lobet,structure,isplaymoney,State,Sit,Disconn,siteMask,clubId>
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBY_ADMIN_GET_OBJ_SUMMARY						4253 //4 - reqId
#define MSG_LOBBY_ADMIN_GET_OBJ_SUMMARY_REPLY				4254 //4 4444 4444 4444 4444 4444 4444  44 44 4<4 TableStat>4<4 Tourntat>4<4 ZoomStat>- reqId,tableActive,tableDisconnect,tournActive,tounrDisconnect,sngActive,sngDisconnect,sagActive,sagDisconnect, tableNum<site,TableStat>,tournNum<site,TournStat>,zoomNum<site,ZoomStat>

#define MSG_LOBBY_ADMIN_RESTORE_DEAD_TABLE					4255 // 48 reqId, tableId
#define MSG_LOBBY_ADMIN_RESTORE_DEAD_TABLE_REPLY			4256 // 46[s] - reqId, errCode[errMsg]

//PYR-37900
// LobbyEtc -> Lobby
#define MSG_LOBBYETC_ROFUS_GAMBLERCHECK						4257 //44ss48 - reqId,userIntId, usertId, fiscalCode, flag flag2
#define MSG_LOBBYETC_ROFUS_GAMBLERCHECK_REPLY				4258 //46[s]
// PYR-38492
// obsoleted by PYR-71623
//#define MSG_LOBBYETC_AAMS_BLACKLIST_CHECK					4259 //44s48<PgadUserAssets> - reqId,userIntId, usertId, flag flag2,<PgadUserAssets>
//#define MSG_LOBBYETC_AAMS_BLACKLIST_CHECK_REPLY			4260 //46[s]

#define LOBBY_SNG_ENABLE									4261 // 42 -reqId, enabled
#define LOBBY_SNG_ENABLE_REPLY								4262 //	46[ s|2 ] - reqId, errCode [ errMsg|enabled ]

#define LOBBY_SNG_FACTORY_STATUS							4263 //44b - reqId, factoryId, status
#define LOBBY_SNG_FACTORY_STATUS_REPLY						4264 //46[s] - reqId, errCode [errStr]

#define MSG_LOBBY_ADMIN_TICKET_TOURNIDS						4265 // 44s4 - reqId, clientLicenseId, userId, ticketId
#define MSG_LOBBY_ADMIN_TICKET_TOURNIDS_REPLY				4266 // 46[s|44<M(444d4441111s4441s4)>] - reqId, err_code[errDescr|ticketId, numTourns,
																 //		<M(tournId, tournStatus, tournFlags, startTime, numEntrants, numPlayers, buyIn, gameType, isHiLo, structure,
																 //		 isPlayMoney, server, rake, knockout, fppBuyIn, isRestricted, currency, statusBits, name, tournFlags2)>]
#define MSG_SNG_TOURNS_REGISTRATION_SUMMARY					4267	// 6[s|4<6s4s>] - errCode[ errMsg| num <errCode, errMsg, tournId, srvName> ]
#define MSG_SNG_REGISTER_TOURNAMENT_USER					4268	// s444s144M - userId, buyInRM, buyInPM, buyInFpp, pwd, useTicket, buyInTM, buyInWM, currencyContext
#define MSG_SNG_REGISTER_TOURNAMENT_USER_REPLY				4269	// 6s[6s4bssMs] - errCode, errMsg [allInOption, locale, tournId, seatAvail, servrName, socialToken, whereIsBody, aamsTicket]

#define MTL_SAG_BIG_JACKPOT									4270 //44 - reqId, tournId
#define MTL_SAG_BIG_JACKPOT_REPLY							4271 //46[s] - reqId, errCode [errStr]

// PYR-39535
#define MTL_TABLE_ENABLE_PUBLIC_OBSERVATION					4272 // empty
#define MTL_TABLE_DISABLE_PUBLIC_OBSERVATION				4273 // empty

//PYR-39272
#define MSG_SNG_POOL_CHECKIN								4274 // s444s144M - userId, buyInRM, buyInPM, buyInFpp, pwd, useTicket, buyInTM, buyInWM, currencyContext
#define MSG_SNG_POOL_CHECKIN_REPLY							4275 // 6[s|1] - errCode[errMsg| numGames]
#define MSG_SNG_POOL_CHECKOUT								4276 // 4 - scriptId
#define MSG_SNG_POOL_CHECKOUT_REPLY							4277 // 6[s|1] - errCode[errMsg| numGames]

//	PYR-41017
#define MSG_LOBBY_ADMIN_GET_CLIENT_VISIBILITY				4278	//	44s - reqId,licenseId,country
#define MSG_LOBBY_ADMIN_GET_CLIENT_VISIBILITY_REPLY			4279	//	46[s|4<8>] - reqId,errCode[errStr|size<visibilities>]

//PYR-39272
#define MSG_SNG_POOL_WHERE_IS_PLAYER						4280 // s - userId
#define MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY					4281 // 6[s|4<41>] - errCode[errMsg| numGames<scriptId, numPending>]

#define SAG_MTL_GO_PUBLIC_NOTIFY							4282 //44 - reqId, tournId

#define MTL_LOBBY_UPDATE_DYNAMIC_FLAGS						4283 //44 - reqId, tournId
#define MTL_LOBBY_UPDATE_DYNAMIC_FLAGS_REPLY				4284 //4 - reqId

#define MTL_SNG_STARTER_REMATCH								4285 //4444<s>4<M> - reqId, scriptId, tournId, numUsers<userId>, numUsers<rematchInfo> 
#define MTL_SNG_STARTER_REMATCH_REPLY						4286 //4 - reqId

#define SNG_STARTER_LOBBY_REMATCH							4287 //4444<s>4<M>o - reqId, scriptId, tournId, numUsers<userId>, numUsers<rematchInfo>, tournscript 
#define SNG_STARTER_LOBBY_REMATCH_REPLY						4288 //4 -reqId

#define LOBBY_SNG_STARTER_CHECHOUT_USER						4289 //4s - reqId, userID
#define LOBBY_SNG_STARTER_CHECHOUT_USER_REPLY				4290 //46[s] - reqId, errCode[errStr]

#define STARTER_MTL_REGISTER_CHECK							4291
#define STARTER_MTL_REGISTER_CHECK_REPLY					4292

#define STARTER_LOBBY_PAUSED_REGISTRATION_NOTIFY			4293
#define STARTER_LOBBY_PAUSED_REGISTRATION_NOTIFY_REPLY		4294

#define STARTER_LOBBY_USER_POOL_CHECK_NOTIFY				4295
#define STARTER_LOBBY_USER_POOL_CHECK_NOTIFY_REPLY			4296

// PYR-43054 seat finder -> lobby
#define SFN_LOBBY_INIT_DATA                                 4297 // 4 - reqId
#define SFN_LOBBY_INIT_DATA_REPLY							4298 // 424<ss>4<44M4<s4444sss44884<s>>4<84<1>>> - reqId,errCode,tableDbmsNum<server,serverObject>findersNum<finderId,tableTypeId,tableTypeBody,usersNum<userId,buyIn,seatFinderId,userIntId,clientSiteId,country,countryByIp,stateByIp,clientPlatform,clientVersion,appSessionId,joinTime,relatedGroupsNum<group>>,tablesNum<tableId,emptyPositionsNum<position>>>

#define SFN_LOBBY_INIT_DATA_COMPLETE                        4299 // 4 - reqId
#define SFN_LOBBY_INIT_DATA_COMPLETE_REPLY                  4300 // 4 - reqId

#define SFN_LOBBY_REMOVE_PLAYER								4301 // 4414 - reqId,userIntId,reason,finderId
#define SFN_LOBBY_REMOVE_PLAYER_REPLY						4302 // 42 - reqId,errCode

// PYR-43054 client -> lobby
#define MSG_LOBBY_JOIN_SEATFINDER                           4303 // 44s12224 - finderId,buyIn,password,autoRebuyOpt,autoRebuyBigBets,fallUnderBigBets,fallUnderPercents
#define MSG_LOBBY_JOIN_SEATFINDER_REPLY                     4304 // 2[s4|44] - errCode,[errDescr,finderId | finderId,placeInList]

#define MSG_LOBBY_UNJOIN_SEATFINDER                         4305 // 4 - finderId
#define MSG_LOBBY_UNJOIN_SEATFINDER_REPLY                   4306 // 2[s]4 - errCode,[errDescr],finderId

#define MSG_LOBBY_GET_JOINED_SEATFINDERS                    4307 // empty request
#define MSG_LOBBY_GET_JOINED_SEATFINDERS_REPLY              4308 // 4<441111s44> - findersNum<finderId,placeInList,game,struct,hiLo,realMoney,currency,lowBet,hiBet>

// PYR-43054 admin -> lobby
#define MSG_LOBBY_ADMIN_GET_SEATFINDERS_STATS				4309 // 44tt - reqId,siteId,fromSrvTime,toSrvTime
#define MSG_LOBBY_ADMIN_GET_SEATFINDERS_STATS_REPLY			4310 // 42[s|4<M(1s441448888)>] - myReqId,errCode,[errDescr|findersNum<M(maxPlayersPerTable,currency,lowBet,hiBet,game,minBuyIn,maxBuyIn,avgHandsPlayedPerTable,AvgTimeBeforeSeating,avgTimeBeforeCancel,avgTimeBeforeRemoving)>]
																 // averages are multiplied by 100 in order to have two digits after decimal point
// PYR-43054 lobby -> client - user was removed from seat finder for some internal reason
#define MSG_LOBBY_CLI_SEATFINDER_REMOVE_NOTIFY				4311 // 14<4> - reason,findersNum<finderId>
#define MSG_LOBBY_CLI_SEATFINDER_SEATED						4312 // ss44488 - server,serverObject,seatFinderId,tableTypeId,maxPlayers,tableId,mergedFromTableId

// PYR-43054 lobby -> seat finder
#define LOBBY_SFN_JOIN                                      4313 // 4s4444sss4484 - reqId,userId,finderId,buyIn,userIntId,clientSiteId,country,countryByIp,stateByIp,clientPlatform,clientVersion,appSessionId,blindDebt
#define LOBBY_SFN_JOIN_REPLY                                4314 // 42[s|s4444sss44848<s>] - reqId,errCode,[errDescr|userId,finderId,buyIn,userIntId,clientSiteId,country,countryByIp,stateByIp,clientPlatform,clientVersion,appSessionId,joinTime,relatedGroupsNum<group>]

#define LOBBY_SFN_UNJOIN                                    4315 // 44s4 - reqId,userIntId,userId,finderId
#define LOBBY_SFN_UNJOIN_REPLY                              4316 // 42[s] - reqId,errCode,[errDescr]

#define LOBBY_SFN_UNJOIN_FROM_ALL                           4317 // 44 - reqId,userIntId
#define LOBBY_SFN_UNJOIN_FROM_ALL_REPLY                     4318 // 44<4> - reqId,unjoinedFindersNum<finderId>

#define LOBBY_SFN_SEAT_FINDERS_REFRESH                      4319 // 44<4>4<44Mb> - reqId,removedFindersNum<finderId>,newOrUpdatedFindersNum<finderId,tableTypeId,tableTypeBody,isSimilarType>
#define LOBBY_SFN_SEAT_FINDERS_REFRESH_REPLY                4320 // 42[s|4<s4<4>>] - reqId,errCode,[errDescr|usersNum<userId,findersNum<finderId>>]

#define LOBBY_SFN_PLAYER_LEFT								4321 // 48s44 - reqId,tableId,userId,seatFinderId,chips
#define LOBBY_SFN_PLAYER_LEFT_REPLY							4322 // 4 - reqId

#define LOBBY_SFN_CHANGE_DEALING_STATE						4323 // 484 - reqId,tableId,seatFinderId
#define LOBBY_SFN_CHANGE_DEALING_STATE_REPLY				4324 // 4 - reqId

// PYR-43054 lobby -> client - update of place in waiting list
#define MSG_LOBBY_CLI_GWL_PLACE_UPDATE						4325 // 44 - seatFinderId,placeInList

// PYR-45728 protect Monet-Lobby-Client message transmission
#define MSG_MONET_SERVER_MUTE_ENABLE			4326 // mute monet
#define MSG_MONET_SERVER_MUTE_ENABLE_REPLY		4327 // mute monet
#define MSG_MONET_SERVER_MUTE_DISABLE			4328 // mute monet
#define MSG_MONET_SERVER_MUTE_DISABLE_REPLY		4329 // mute monet

// PYR-43054 client -> lobby
#define MSG_LOBBY_SEATFINDER_CHECK_CHIPS					4330 // 4 - seatFinderId
#define MSG_LOBBY_SEATFINDER_CHECK_CHIPS_REPLY				4331 // 2[s...|44M(O)41] - errCode[errDescr... | availChips,defaultBuyIn,M(SeatFinderCli),userRollId,clientServiceFlag]

// PYR-43054 seat finder -> lobby
#define SFN_LOBBY_SIT_USER									4332 // 484s14 - reqId,tableId,userIntId,userId,position,buyIn
#define SFN_LOBBY_SIT_USER_REPLY							4333 // 42[s] - reqId,errCode[errDescr]

// PYR-43054 lobby -> table and table ->lobby
#define LOBBY_TABLE_SIT_USER								4334 // 4448A - reqId,sfnConnId,sfnReqId,tableId,AuthUserOnTable
#define TABLE_LOBBY_USER_SEATED								4335 // 414481s44844ss48{BrandImages} - reqId,tableErrCode,sfnConnId,sfnReqId,tableId,position,userId,userIntId,chips,privileges2,privileges,imageId,city,country,licenseId,flags2,brandImages
#define TABLE_LOBBY_USER_SEATED_REPLY						4336 // 4 - reqId

// PYR-43054 lobby -> sfn
#define LOBBY_SFN_TABLE_DEAD								4337 // 448 - reqId,seatFinderId,tableId
#define LOBBY_SFN_TABLE_DEAD_REPLY							4338 // 4 - reqId

// PYR-43054, PYR-42791
// lobby --> LobbyEtc
#define MSG_LOBBYETC_CHECK_POOL_BLOCK_ELIGIBILITY			4339 // 41<44> - reqId,usersNum<licenseId,vipStatus>
#define MSG_LOBBYETC_CHECK_POOL_BLOCK_ELIGIBILITY_REPLY		4340 // 421<b> - reqId,errCode,usersNum<eligibility>

// lobby --> LobbyEtc
#define MSG_LOBBYETC_IS_USER_POOL_BLOCKED					4341 // 44 - reqId,usersIntId
#define MSG_LOBBYETC_IS_USER_POOL_BLOCKED_REPLY				4342 // 42[s|t] - reqId,errCode[errDescr|blockedUntil]

// obsolete
//#define LOBBY_SFN_TABLE_UPDATE_DEALER_POSITION			4343 // 4481 - reqId,seatFinderId,tableId,dealerPos
//#define LOBBY_SFN_TABLE_UPDATE_DEALER_POSITION_REPLY		4344 // 4 - reqId

#define LOBBY_SFN_TABLE_HAND_FINISHED						4345 // 448 - reqId,seatFinderId,tableId
#define LOBBY_SFN_TABLE_HAND_FINISHED_REPLY					4346 // 4 - reqId

// PYR-43054
#define LOBBY_TABLE_ENABLE_PUBLIC_OBSERVATION				4347 // empty
#define LOBBY_TABLE_DISABLE_PUBLIC_OBSERVATION				4348 // empty

#define MTL_TGM_FLIGHT_LOSER_NOTIFY							4349 // 44s444444 - reqId, tgmType, userId, userIntId, chipsLastHand, timeInTourn, levelEliminated, satelliteId, numReentries
#define MTL_TGM_FLIGHT_LOSER_NOTIFY_REPLY					4350 // 4 - reqId

// PYR-31663
#define MTL_TABLE_NOTIFY_TOURN_FINISHED						4351 // 4444444 - reqId, 1stPlacePrize, 2ndPlacePrize, 3rdPlacePrize, 1stPlaceBounty, 2ndPlaceBounty, 3rdPlaceBounty

// PYR-43054
#define SFN_LOBBY_SEATFINDER_REMOVED						4352 // 44 - reqId,seatFinderId
#define SFN_LOBBY_SEATFINDER_REMOVED_REPLY					4353 // 4 - reqId

// PYR-47376 Simple Auto Deals
#define MTL_USER_SIMPLE_DEAL_STATE							4354	// empty
#define MTL_USER_SIMPLE_DEAL_STATE_REPLY					4355	// 6[s|841844<s444>411111s4<14>] - errCode[errMessage|timestamp, dealGeneration, dealState, timeout, dealFailureState, participantsInfo<name,chips,deal1Share,deal2Share>, leftFor1st, deal1Type, deal2Type, playerAgreedType, deal1AgreedCount, deal2AgreedCount, currency, noDealPayouts<position, payout>]
#define MTL_USER_SIMPLE_DEAL_AGREE							4356    // 41 - dealGeneration, dealType // dealType: member of eDealType
#define MTL_USER_SIMPLE_DEAL_AGREE_REPLY					4357    // 6[s] - errCode [errMessage]
#define MTL_USER_SIMPLE_DEAL_DECLINE						4358    // 4 - dealGeneration
#define MTL_USER_SIMPLE_DEAL_DECLINE_REPLY					4359	// 6[s] - errCode [errMessage]

// PYR-48904
#define LOBBY_SFN_POOL_BLOCK_AND_CHIPS_QUERY				4360	// 44s4144 - reqId,seatFinderId,userId,userIntId,brandId,clientPlatform,clientVersion
#define LOBBY_SFN_POOL_BLOCK_AND_CHIPS_QUERY_REPLY			4361	// 424 - reqId, errCode, {the rest depends on errCode}

//PYR-48837
#define LOBBY_STARTER_TOURN_INIT_FAILED						4362	// 4446s	reqId,sriptId,tournId,errCode,errDesc
#define LOBBY_STARTER_TOURN_INIT_FAILED_REPLY				4363	// 46[s]	reqId, errCode, errDesc

//PYR-43054
#define SFN_LOBBY_GET_DISCONNECTED_USERS					4364	// 44<s4> - reqId,usersNum<userId,userIntId>
#define SFN_LOBBY_GET_DISCONNECTED_USERS_REPLY				4365	// 44<s4> - reqId,usersNum<userId,userIntId>

#define LOBBY_SFN_USER_DISCONNECTED							4366	// 4s44<4> - reqId,userId,userIntId,seatFindersNum<seatFinderId>
#define LOBBY_SFN_USER_DISCONNECTED_REPLY					4367	// 4 - reqId

// PYR-50064
#define MSG_LOBBY_ADMIN_GET_OBLIGATION_SUMMARY_REPORT		4368	// 4477tt - reqId,clientSiteId,minBuyInBB,maxBuyInBB,fromTime,toTime
#define MSG_LOBBY_ADMIN_GET_OBLIGATION_SUMMARY_REPORT_REPLY	4369	// 427777 - reqId,errCode(always=0),creationsNum,updatesNum,avgObligationBuyIn,usersNum

#define LOBBY_SFN_GET_OBLIGATION_SUMMARY_REPORT				4370	// 4477tt - reqId,clientSiteId,minBuyInBB,maxBuyInBB,fromTime,toTime
#define LOBBY_SFN_GET_OBLIGATION_SUMMARY_REPORT_REPLY		4371	// 427777 - reqId,errCode(always=0),creationsNum,updatesNum,avgObligationBuyIn,usersNum

#define LOBBY_SFN_SET_SITE_DEALING_STATUS					4372	// 444 - reqId,allowSiteInclusiveMask,allowSiteExclusiveMask
#define LOBBY_SFN_SET_SITE_DEALING_STATUS_REPLY				4373	// 4 - reqId

//	PYR-50915, LobbyEtc -> Lobby
#define MSG_LOBBYETC_FORWARD_ADMIN_MSG_TO_CLIENT			4374	//	4ss - reqId, userId, message
#define MSG_LOBBYETC_FORWARD_ADMIN_MSG_TO_CLIENT_REPLY		4375	//	46[s] - reqId, errCode [errDescr]

#define MSG_ADMIN_SAG_SMT_UPLOAD							4376	// 4s4<M(O)>4<M(O)> - reqId, filename, numPayTables<M(SagPayoutTable)>, numScripts<M(IniFileEx)>
#define MSG_ADMIN_SAG_SMT_UPLOAD_REPLY						4377	// 42[s]	reqId, errCode, errDesc

#define MSG_ADMIN_SAG_SMT_DELETE							4378	// 44<4> - reqId, num<ScriptIds>
#define MSG_ADMIN_SAG_SMT_DELETE_REPLY						4379	// 42[s]	reqId, errCode, errDesc

#define MSG_ADMIN_SAG_SMT_PAUSE_RESUME						4380	// 414<4> - reqId, resume, num<ScriptIds>
#define MSG_ADMIN_SAG_SMT_PAUSE_RESUME_REPLY				4381	// 42[s]	reqId, errCode, errDesc

#define MSG_ADMIN_SAG_SMT_PAYTABLE							4382	// 44 - reqId, scriptId
#define MSG_ADMIN_SAG_SMT_PAYTABLE_REPLY					4383	// 42[s|M(O)4<M(O)>] - reqId, M(sagPayoutTable), num<M(SagPayoutTicket)>

#define MSG_ADMIN_SAG_SMT_GET								4384	// 4O - reqId, SagSmtFilter
#define MSG_ADMIN_SAG_SMT_GET_REPLY							4385	// 42[s|4<M(O)>] - reqId, num<M(SagSmtDescriptor)>

// PYR-50713 MtLobby -> Client
#define MTL_CLIENT_SIMPLE_DEAL_ATTEMPT_COMPLETED			4386	// 4s - dealGeneration,message

//	PYR-51670 LobbyEtc -> Lobby
#define MSG_LOBBYETC_CVL_USER_BAR_UPDATE					4387	//	4s4 - reqId, userId, targetRps
#define MSG_LOBBYETC_CVL_USER_BAR_UPDATE_REPLY				4388	//	46[s] - reqId, errCode [errDescr]

// PYR-48860 Admin -> Lobby
#define MSG_ADMIN_LOBBY_RESTORE_TGM							4389 //4s4 - reqId,groupName,type
#define MSG_ADMIN_LOBBY_RESTORE_TGM_REPLY					4390 //46[s] - reqId,errCode[errDescr]

// PYR-42280
#define MSG_TABLE_INIT_SFN_REPLAYER_ADDRESS					4391 // ss - replayerAddress, replayerObject

// PYR-52360
#define BL_LOBBY_NOTIF_MSG_CHECKER_UPDATE                   4395     // 4... - reqId,...

#define MSG_TABLE_MTLOBBY_SAGMAX_USER_DRAW					4396	 //s4 user, usersChoice

#define MTL_TABLE_CHAT_CONTROL								4397	// 4b - flags, enabled

// PYR-55173
#define TABLE_LOBBY_USER_POSTED_BIG_BLIND					4398 // 4s8 - reqId,userId,tableId
#define TABLE_LOBBY_USER_POSTED_BIG_BLIND_REPLY				4399 // 4 - reqId

#define LOBBY_SFN_USER_POSTED_BIG_BLIND						4400 // 4484 - reqId,userIntId,tableId,seatFinderId
#define LOBBY_SFN_USER_POSTED_BIG_BLIND_REPLY				4401 // 4 - reqId

///////////////////// constants used in MSG_ADMIN_MASS_CLOSE_RING_TABLES
#define RING_TABLES_INCLUDE_HG	0x001
#define RING_TABLES_PM			0x002
#define RING_TABLES_RM			0x004
#define RING_TABLES_INCLUSIVE	0x008
#define RING_TABLES_ALL			(RING_TABLES_PM | RING_TABLES_RM | RING_TABLES_INCLUDE_HG)
#define RING_TABLES_INACTIVE	0x010

// PYR-53673
#define MSG_ADMIN_LOBBY_MASS_CLOSE_RING_TABLES				4402 // 4O4I - reqId, siteIdMask, flags, tableMessage
#define MSG_ADMIN_LOBBY_MASS_CLOSE_RING_TABLES_REPLY		4403 // 44 - reqId, closedTablesNum

#define MSG_ADMIN_LOBBY_MASS_CLOSE_RING_TABLES_BY_IDS		4404 // 44<8>I - reqId, numTables<tableId>, tableMessage
#define MSG_ADMIN_LOBBY_MASS_CLOSE_RING_TABLES_BY_IDS_REPLY	4405 // 44 - reqId, closedTablesNum

#define LOBBY_SFN_TABLE_READY_TO_DIE						4406 // 448 - reqId, seatFinderId, tableId
#define LOBBY_SFN_TABLE_READY_TO_DIE_REPLY					4407 // 46 - reqId, errCode (always 0)

// PYR-49647
#define MSG_LOBBY_TIMER_PREPARE_FOR_SHUTDOWN				4408	// empty
#define MSG_LOBBY_TIMER_RESTART_MODE                        4409	// empty
#define LOBBY_MTL_NEED_RECOVERY_INFO						4410	// empty
#define MTL_LOBBY_RECOVERY_INFO								4411	// large and complex TODO MDS RESTART: generate format string with ATF?
#define MTL_LOBBY_RECOVERY_INFO_REPLY						4412	// 46[s] - reqId,errCode[errDescr]
#define LOBBY_BL_NEED_RECOVERY_INFO							4413	// empty
#define BL_LOBBY_RECOVERY_INFO								4414	// large and complex TODO MDS RESTART: generate format string with ATF?
#define BL_LOBBY_RECOVERY_INFO_REPLY						4415	// 46[s] - reqId,errCode[errDescr]
#define LOBBY_REPLAY_NEED_RECOVERY_INFO						4416	// empty
#define REPLAY_LOBBY_RECOVERY_INFO							4417	// ff - hasEverStarted, isCompletelyFinished
#define REPLAY_LOBBY_RECOVERY_INFO_REPLY					4418	// 46[s] - reqId,errCode[errDescr]

#define MSG_SERVER_REPLAY_UPDATE_STATS						4419 //444 - reqId, clientId, numErrors
#define MSG_SERVER_REPLAY_UPDATE_STATS_REPLY				4420 //4 - reqId
#define MSG_LOBBY_REPLAY_MSG_CHECKER_UPDATE		      		4421 //4O - reqId, object

#define MSG_BLLOBBY_REPLAY_NOTIF_MSG_CHECKER_UPDATE			4422 //4O - reqId, object

// PYR-49647 lobby -> seatfinder
#define LOBBY_SFN_NEED_INFO									4423	// 4 - reqId
#define LOBBY_SFN_NEED_INFO_REPLY							4424	// 46[s|4<44>4<4s74s995666>] - reqId,errCode[errDescr|numSeatFinders<tableTypeId,seatFinderId>,numUsers<seatFinderId,userId,buyIn,userIntId,ipAddress,joinTime,joinOrder,autoRebuyOpt,autoRebuyBigBets,fallUnderBigBets,fallUnderPercents>]

#define MSG_LOBBY_TIMER_RESTART_RECOVERY_INFO_TIMEOUT		4425	// 684 - type,id,connId

// PYR-50064
#define MSG_LOBBY_ADMIN_GET_OBLIGATION_USER_REPORT			4426	// 4477tt - reqId,userIntId,minBuyInBB,maxBuyInBB,fromTime,toTime
#define MSG_LOBBY_GET_ADMIN_OBLIGATION_USER_REPORT_REPLY	4427	// 424<M(444477ttt887)> - reqId,errCode(always=0),num<M(obligationId,tableTypeId,clientSiteId,seatfinderId,userIntId,initialBuyIn,obligationBuyIn,timeJoinedTable,creationTime,expirationTime,createdAtTableId,usedAtTableId,updatesNum)>

#define LOBBY_SFN_GET_OBLIGATION_USER_REPORT				4428	// 4477tt - reqId,userIntId,minBuyInBB,maxBuyInBB,fromTime,toTime
#define LOBBY_SFN_GET_OBLIGATION_USER_REPORT_REPLY			4429	// 424<M(444477ttt887)> - reqId,errCode(always=0),num<M(obligationId,tableTypeId,clientSiteId,userIntId,initialBuyIn,obligationBuyIn,timeJoinedTable,creationTime,expirationTime,createdAtTableId,usedAtTableId,updatesNum)>

// PYR-49647
#define LOBBY_TABLE_NEED_RECOVERY_INFO						4430	// empty
#define TABLE_LOBBY_RECOVERY_INFO							4431	// 4fff1s55555555555 - reqId,itSessionExpired,delayedClosing,dealCardsStatus,stopDealingState,cancelledBy,dealerPos,connQuality0,connQaulity1,connQaulity2,connQuality3,connQuality4,connQuality5,connQuality6,connQaulity7,connQuality8,connQuality9
#define TABLE_LOBBY_RECOVERY_INFO_REPLY						4432	// 46[s] - reqId,errCode[errDescr]

#define MSG_TABLE_MTLOBBY_GET_REBUY_ADDON_AMOUNT			4433	// b1s - isRebuy,seat,userId
#define MSG_TABLE_MTLOBBY_GET_REBUY_ADDON_AMOUNT_REPLY		4434	// 214M(8411bs) - errCode,seat,amount,extraBody(tournFlags2,knockout,game,structure,isHiLo,name)

// PYR-66019
#define MSG_ADMIN_LOBBY_RESTART_STATUS						4435	// 4b - reqId,detailedMode
#define MSG_ADMIN_LOBBY_RESTART_STATUS_REPLY				4436	// 46[s|4<sb444<8>>4] - reqId,errCode[errDescr|numTypes<typeName,typeReady,totalObjects,remainingObjects,numIds<objectId>>,delayedMessages]

#define MSG_ADMIN_LOBBY_RESTART_FINALIZE					4437	// 4 - reqId
#define MSG_ADMIN_LOBBY_RESTART_FINALIZE_REPLY				4438	// 46[s] - reqId,errCode[errDescr]

#define MTL_SAG_REQUEST_FREE_BET_AWARD						4439	// 44s44s - reqId,userIntId,userCurr,campaignId,stake,currency
#define MTL_SAG_REQUEST_FREE_BET_AWARD_REPLY				4440	// 46[s|84s] - reqId,errCode[errDescr|sbTransId,amount,currency]

#define MSG_TABLE_BROADCAST_UPDATE                          4441   // 4<4M> num<id,msg>

// PYR-67696
#define MSG_ADMIN_LOBBY_UPDATE_SELFEXCLUSION				4442	// 444ss444 - reqId, userIntId, licenseId, fiscalCode,userId,days,exclusionFlags,seFlags
#define MSG_ADMIN_LOBBY_UPDATE_SELFEXCLUSION_REPLY			4443	// 46[s] - reqId,errCode[errDescr]

// Obsoleted by PYR-82532
#define MSG_ADMIN_LOBBY_REMOVE_FROM_SE_LIST					4444	// 444s - reqId, userIntId, licenseId, fiscalCode
#define MSG_ADMIN_LOBBY_REMOVE_FROM_SE_LIST_REPLY			4445	// 46[s] - reqId,errCode[errDescr]

// PYR-66972
#define MSG_ADMIN_LOBBY_MASS_ROLL_INIT						4446	// 4s4<8>44b24 - reqId,cancellationText,numTournIds<tournIds>,flags,siteMask,enabled,comparator,effectiveAddon
#define MSG_ADMIN_LOBBY_MASS_ROLL_INIT_REPLY				4447	// 42[s|4] - reqId,errCode[errDesc|token]

#define MSG_ADMIN_LOBBY_MASS_ROLL_SUMMARY_STATUS			4448	// 44 - reqId,token
#define MSG_ADMIN_LOBBY_MASS_ROLL_SUMMARY_STATUS_REPLY		4449	// 42[s|4<M(4b44884<s8>)>4<st>] - reqId,errCode[errDesc|numTournAndRollTypeTotals<M(tournType,performRoll,formula,numTourns,pmRefunds,fppRefunds,numRmRefunds<currency,amount>)>,numRecentUsers<adminId,initTime>]

#define MSG_ADMIN_LOBBY_MASS_ROLL_DETAILED_STATUS			4450	// 44 - reqId,token
#define MSG_ADMIN_LOBBY_MASS_ROLL_DETAILED_STATUS_REPLY		4451	// 42[s|4<M(8stsb4444444bb4b44<M(4888)>sb484)>] - reqId,errCode[errDesc|numTournDetails<M(tournId,name,startTime,currency,isPlayMoney,tournType,buyIn,rake,fppBuyIn,amountGuaranteed,totalPrizePool,effectiveAddon,started,lateRegActive,playersRemaining,performRoll,selectedFormula,numAvailableRollTypes<M(formula,rmRefund,pmRefund,fppRefund)>,noRollReason,inTheMoney,rmAlreadyPaid,pmAlreadyPaid,fppAlreadyPaid)>]

#define MSG_ADMIN_LOBBY_MASS_ROLL_UPDATE					4452	// 444<8b4> - reqId,token,numUpdates<tournId,performRoll,formula>
#define MSG_ADMIN_LOBBY_MASS_ROLL_UPDATE_REPLY				4453	// 42[s] - reqId,errCode[errDesc]

#define MSG_ADMIN_LOBBY_MASS_ROLL_COMMIT					4454	// 44 - reqId,token
#define MSG_ADMIN_LOBBY_MASS_ROLL_COMMIT_REPLY				4455	// 42[s] - reqId,errCode[errDesc]

#define MSG_ADMIN_LOBBY_MASS_ROLL_RESULTS					4456	// 44 - reqId,token
#define MSG_ADMIN_LOBBY_MASS_ROLL_RESULTS_REPLY				4457	// 42[s|44<8ss>] - reqId,errCode[errDesc|numSuccessfulRolls,numIncompleteRolls<tournId,name,reason>]

// PYR-53981
#define MSG_LOBBY_TABLE_NOTIF_MSG_CHECKER_UPDATE            4458  // 4... - reqId,...

#define TABLE_LOBBY_UPDATE_STATS							4459 // 44 reqId, numMsgCheckErrors
#define TABLE_LOBBY_UPDATE_STATS_REPLY						4460 // 4 reqId

// PYR-53981
#define MSG_BLLOBBY_TABLE_NOTIF_MSG_CHECKER_UPDATE          4461  // msgchecker body
#define MSG_MTLOBBY_TABLE_NOTIF_MSG_CHECKER_UPDATE          4462  // msgchecker body

// PYR-85668
#define MSG_TABLE_LOBBY_SITTING_STATUS						4463 // 4s48b - reqId, userId, userIntId, tableId, isSitIn
#define MSG_TABLE_LOBBY_SITTING_STATUS_REPLY				4464 // 4 - reqId

#define MSG_LOBBY_SFN_SITTING_STATUS						4465 // 4s448b - reqId, userId, userIntId, seatFinderId, tableId, isSitIn
#define MSG_LOBBY_SFN_SITTING_STATUS_REPLY					4466 // 4 - reqId

#define MSG_LOBBY_SFN_ALL_PLAYERS_SITTING_OUT				4467 // 448 - reqId, seatFinderId, tableId
#define MSG_LOBBY_SFN_ALL_PLAYERS_SITTING_OUT_REPLY			4468 // 4 - reqId

#define SFN_LOBBY_DISBANDED_TABLE_STOP_DEALING_FOR_MERGE			4469 // 4884<1> - reqId, disbandedTableId, receivingTableId, num<kickCandidatePos>
#define SFN_LOBBY_DISBANDED_TABLE_STOP_DEALING_FOR_MERGE_REPLY		4470 // 46[s|4<M>] - reqId, errCode, [errDescr|numPlayers<playerStateBody>]

#define LOBBY_TABLE_DISBANDED_TABLE_STOP_DEALING_FOR_MERGE			4471 // 4484<1> - sfnReqId, sfnConnId, receivingTableId, num<kickCandidatePos>

#define TABLE_LOBBY_DISBANDED_TABLE_DEALING_STOPPED_FOR_MERGE		4472 // 48446[s|4<M>] - reqId, tableId, sfnReqId, sfnConnId, errCode, [errDescr|numPlayers<playerStateBody>]
#define TABLE_LOBBY_DISBANDED_TABLE_DEALING_STOPPED_FOR_MERGE_REPLY	4473 // 4 - reqId

#define SFN_LOBBY_RECEIVING_TABLE_KICK_AND_CHECK_FOR_MERGE			4474 // 4884<1> - reqId, receivingTableId, num<kickCandidatePos>
#define SFN_LOBBY_RECEIVING_TABLE_KICK_AND_CHECK_FOR_MERGE_REPLY	4475 // 46[s|1] - reqId, errCode, [errDescr|receivingTableDealerPos]

#define LOBBY_TABLE_RECEIVING_TABLE_KICK_AND_CHECK_FOR_MERGE		4476 // 444<1> - sfnReqId, sfnConnId, num<kickCandidatePos>

#define TABLE_LOBBY_RECEIVING_TABLE_KICKED_AND_CHECKED_FOR_MERGE	4477 // 48446[s|1] - reqId, tableId, sfnReqId, sfnConnId, errCode, [errDescr|receivingTableDealerPos]
#define TABLE_LOBBY_RECEIVING_TABLE_KICKED_AND_CHECKED_FOR_MERGE_REPLY	4478 // 4 - reqId

#define SFN_LOBBY_SIT_MERGED_USER									4479 // 448b... - reqId, seatFinderId, disbandedTableId, receivingTableId, playerStateFields...
#define SFN_LOBBY_SIT_MERGED_USER_REPLY								4480 // 46[s] - reqId, errCode, [errDescr]

#define LOBBY_TABLE_SIT_MERGED_USER									4481 // 4448... - reqId, sfnReqId, sfnConnId, disbandedTableId, authUserOnTable, playerForMergeTableData

#define TABLE_LOBBY_MERGED_USER_SEATED								4482 // 44488...6[s] - sfnConnId, sfnReqId, receivingTableId, disbandedTableId, playerStateFields..., errCode, [errDescr]
#define TABLE_LOBBY_MERGED_USER_SEATED_REPLY						4483 // 4 - reqId

#define SFN_LOBBY_RECEIVING_TABLE_RESUME_DEALING_AFTER_MERGE		4484 // 48 - reqId, tableId
#define SFN_LOBBY_RECEIVING_TABLE_RESUME_DEALING_AFTER_MERGE_REPLY	4485 // 4 - reqId

#define LOBBY_TABLE_RECEIVING_TABLE_RESUME_DEALING_AFTER_MERGE		4486 // 4 - delaySec

#define MTL_TGM_USER_STACK_FORFEITED_NOTIFY							4487 // 44O - reqId, tgmType, FlightUserStackForfeitInfo
#define MTL_TGM_USER_STACK_FORFEITED_NOTIFY_REPLY					4488 // 46[s]

#define TGM_MTL_PENDING_FORFEIT_USER_STACK_COMPLETED_NOTIFY			4489 //44 - reqId, userIntId

#define TGM_MTL_UPDATE_FINAL_FLIGHT_TRANSFERS_NOTIFY				4490 //44 - reqId, moneyTransferred

#define SFN_LOBBY_INIT_STEALTH_DATA_COMPLETE                        4491 // 4 - reqId
#define SFN_LOBBY_INIT_STEALTH_DATA_COMPLETE_REPLY                  4492 // 4 - reqId

//#define MTL_TABLE_UPDATE_PARAMS_ON_SAG_DRAW							4493 // 444444O - timebank, loBet, hiBet, bringIn, smallBlind, ante, iniFile

#define MSG_ADMIN_LOBBY_GET_TOURN_TABLE_OBJECTS						4494 // 48 - reqId,tournId
#define MSG_ADMIN_LOBBY_GET_TOURN_TABLE_OBJECTS_REPLY				4495 // 42[s|s4<s>] - reqId,errCode[errDesc|address,numInstances<instance>]

#define LOBBY_TGM_UPDATE_PARAM										4496 // 44s - reqId, paramEnum, value
#define LOBBY_TGM_UPDATE_PARAM_REPLY								4497 //	46[s] - reqId, errCode [errDescr]

#define TGM_MTL_REMOVE_FORFEITED_STACK								4498 //4444 - reqId, userIntId, chips, money

#define MSG_PPP_REG_INFO											4499 // 444 - pppId,seqId,buyInTierId
#define MSG_PPP_REG_INFO_REPLY										4500 // 2[s4b14sbM()M(8411bs)|s8b|88sbs84s888bM()M()14414811b] - errCode[errDescr,minBuyIn,enoughMoneyInOtherCurrency,clientServiceFlag,availableChips,gameCurrency,isAutoConversionOn,M(),M(tournFlags2,bounty,handType,structure,isHiLo,name)|errDescr,currency,buyInAndRake,enoughMoneyInOtherCurrency|buyIn,rake,currency,isPlayMoney,admissionId,admPrice,numTickets,name,userRmBalance,userPmBalance,userTChipsBalance,enoughMoneyInOtherCurrency,M(),M(),speed,minPlayers,maxPlayers,maxPerTable,tournFlags,tournFlags2,handType,structure,isHiLo]

#define MSG_PPP_REGISTER_USER										4501 // 444888bM() - pppId,seqId,buyInTierId,buyInRM,buyInTChips,buyInPM,useTicket,M()
#define MSG_PPP_REGISTER_USER_REPLY									4502 // 2[s|8] - errCode[errDescr|pppEntryId]

#define MSG_PPP_UNREGISTER_USER										4503 // 48 - pppId,pppEntryId
#define MSG_PPP_UNREGISTER_USER_REPLY								4504 // 2[s|484ss] - errCode[errDescr|refund,refundPlay,refundTChips,tournamentCurrency,refundCurrency]

#define MSG_PPP_GET_ENTRIES											4505 // empty message
#define MSG_PPP_GET_ENTRIES_REPLY									4506 // 2[s|4<M(4828b8)>] - errCode[errMsg|numEntries<M(pppId,pppEntryId,status,amountOnHead,canUnregister,tournId)>]

#define MSG_PPP_SET_ENTRY_STATUS									4507 // 482 - pppId,pppEntryId,newStatus
#define MSG_PPP_SET_ENTRY_STATUS_REPLY								4508 // 2[s] - errCode[errMsg]

#define MTL_PPP_KNOCKOUT_NOTIFICATION								4509 // 4<M(44848444)> - numKnockouts<M(bountyEventId,eliminatorUserIntId,eliminatorEntryId,eliminatedUserIntId,eliminatedEntryId,amountPaid,amountToHead,knockoutSpinSeedAmount)>
#define MTL_PPP_KNOCKOUT_NOTIFICATION_REPLY							4510 // 2[s|b] - errCode[errString|lastKnockoutProcessed]

#define MSG_PPP_LOBBY_CHECK_TOURN_LIMIT								4511 // s4b - userId,locale,isPM
#define MSG_PPP_LOBBY_CHECK_TOURN_LIMIT_REPLY						4512 // 2[s] - errCode[errString]

#define MSG_ADMIN_PPP_REGISTER_USER									4513 // s4444888bM() - userId,userIntId,pppId,seqId,buyInTierId,buyInRM,buyInTChips,buyInPM,useTicket,M()
#define MSG_ADMIN_PPP_REGISTER_USER_REPLY							4514 // 2[s|8] - errCode[errDescr|pppEntryId]

#define MSG_ADMIN_PPP_CASHOUT_USER									4515 // 448 - userIntId,pppId,pppEntryId
#define MSG_ADMIN_PPP_CASHOUT_USER_REPLY							4516 // 2[s|484s] - errCode[errDescr|cashout,cashoutPlay,cashoutTChips,cashoutCurrency]

#define MSG_ADMIN_PPP_CASHOUT_ALL_USERS								4517 // 4 - pppId
#define MSG_ADMIN_PPP_CASHOUT_ALL_USERS_REPLY						4518 // 2[s] - errCode[errDescr]

#define MTL_PPP_CANCEL_RUNNING_TOURN								4519 // b4<M(4884)> - finalizeMode,numRemainingUsers<M(userIntId,entryId,newValue,refund)>
#define MTL_PPP_CANCEL_RUNNING_TOURN_REPLY							4520 // 2[s] - errCode[errString]

#define MSG_ADMIN_PPP_LIST											4521 // empty
#define MSG_ADMIN_PPP_LIST_REPLY									4522 // 4<M()> // TODO:

#define MSG_ADMIN_PPP_UPLOAD_SCRIPT									4523 // TODO
#define MSG_ADMIN_PPP_UPLOAD_SCRIPT_REPLY							4524 // TODO

#define MSG_ADMIN_PPP_DELETE_SCRIPT									4525 // TODO
#define MSG_ADMIN_PPP_DELETE_SCRIPT_REPLY							4526 // TODO

#define MSG_ADMIN_PPP_PAUSE_RESUME									4527 // TODO
#define MSG_ADMIN_PPP_PAUSE_RESUME_REPLY							4528 // TODO

#define MSG_ADMIN_PPP_GET_USER_ENTRIES								4529 // TODO
#define MSG_ADMIN_PPP_GET_USER_ENTRIES_REPLY						4530 // TODO

#define MSG_PPP_ADMIN_BROADCAST_PRIVILEGES							4531 // as in MTL_TOURN_ADMIN_BROADCAST_PRIVILEGES // Lobby to PPPStarter
#define MSG_PPP_ADMIN_BROADCAST_PRIVILEGES_REPLY					4532 // 2[s] - errCode[errString]

#define MSG_PPP_ADMIN_BROADCAST_MULTI_PRIVILEGES					4533 // as in MTL_TOURN_ADMIN_BROADCAST_MULTI_PRIVILEGES // Lobby to PPPStarter
#define MSG_PPP_ADMIN_BROADCAST_MULTI_PRIVILEGES_REPLY				4534 // 2[s] - errCode[errString]

#define MTL_PPP_TABLE_INFORMATION									4535 // 8ssss - tournId,mtLobbyAddress,mtLobbyObject,tableAddress,tableObject
#define MTL_PPP_TABLE_INFORMATION_REPLY								4536 // 2[s] - errCode[errString]

// PYR-47523
// message id range 5000-5499 USED for DALI
// defined in protocols/dali/protocol.h
//															5000
//															....
//															5499

//possible answers to MTL_LOBBY_WHERE_IS_PLAYER
#define WHERE_IS_NOT_FOUND		(-1) // don't know anything about you (s)
#define WHERE_IS_PLAYING		0	// no error, expect server, servObj (ss)
#define WHERE_IS_PLACE			1	// done with tournament, expect place (4)
#define WHERE_IS_NOT_STARTED	2	// i know this user but tournament not started yet or already over (s)
#define WHERE_IS_CLOSED			3	//tournament is already closed
#define WHERE_IS_UNKNOWN_STATUS	4	// status of the tournament is unknown - internal error
#define WHERE_IS_BLITZ_TRANSIT	5	// this player is waiting to be seated to next table in blitz tournament

// possible answers to MSG_LOBBY_OLYMPIC_SNAPSHOT
#define OLYMPIC_SNAPSHOT_OK				0	// everything is fine, the snapshot follows
#define OLYMPIC_SNAPSHOT_UNAVAILABLE	1	// the snapshot is unavailable yet


//tournament status stored in DB

#define TOURN_STATUS_NORMAL					0
#define TOURN_STATUS_CLOSED					1
#define TOURN_STATUS_CANCELLED_ADMIN		2
#define TOURN_STATUS_CANCELLED_SHOW			3
#define TOURN_STATUS_CANCELLED_UNPUBLISHED	4


//possible values of tournament status
#define MT_STATUS_PUBLISHED			0
//#define	MT_STATUS_REG_START			1	// registration started
//#define	MT_STATUS_REG_END			2	// registration closed
#define MT_STATUS_SEATS_AVAIL		1	// users might take their seats
#define MT_STATUS_TOURN_STARTED		2	// tournament started
#define	MT_STATUS_TOURN_CLOSED		3	// tournament closed
#define	MT_STATUS_TOURN_ALREADY_CLOSED	4//used only in crash recovery
#define	MT_STATUS_TOURN_CANCELLED	5	//tourney is cancelled for some reason
#define	MT_STATUS_TOURN_CANCELLED_ADMIN	6	//tourney is cancelled for some reason

#define MT_STATUS_MASK				7 // all but registration bits

#define MT_STATUS_BIT_REGISTRATION_OPEN		(1 << 7)
#define MT_STATUS_BIT_UNREGISTRATION_OPEN	(1 << 8)
#define MT_STATUS_BIT_REGISTRATION_CLOSED	(1 << 9)
#define MT_STATUS_BIT_UNREGISTRATION_CLOSED	(1 << 10)
#define MT_STATUS_BIT_LIMIT_REACHED			(1 << 11)
#define MT_STATUS_BIT_REBUY_OPEN			(1 << 12)
#define MT_STATUS_BIT_ADDON_OPEN			(1 << 13)
#define MT_STATUS_BIT_HUBRACKET_AVAILABLE	(1 << 14)
#define MT_STATUS_BIT_BLITZ_MODE			(1 << 15)
#define MT_STATUS_BIT_REGISTRATION_PAUSED	(1 << 16)
#define MT_STATUS_BIT_BUBBLE_REACHED		(1 << 17) // PYR-71953 

// PYR-18141
#define MT_OPT_EARLY_START					(1 << 0)
#define MT_OPT_SKIP_BREAK					(1 << 1)
#define MT_OPT_ANY							(MT_OPT_EARLY_START | MT_OPT_SKIP_BREAK)

#define ONE_SECOND				1000
#define ONE_MINUTE				(60 * ONE_SECOND)
#define ONE_HOUR				(60 * ONE_MINUTE)

//award schemes
#define	PRIZE_ANOTHER_TOURN					1 //winners go to another tournament
#define	PRIZE_MONEY_PERCENT					2 //winners receive money
#define	PRIZE_MONEY_AND_ANOTHER_TOURN		3 //specified number of winners goes to another tourn. winners receive money
#define	PRIZE_ANOTHER_TOURN_FPP				4 //winners go to another tournament, fppBuyIn != 0
#define	PRIZE_TOURN_TICKETS					5 //winners get tickets (and perhaps $$+%)
#define	PRIZE_CHIPS_PROPORTIONAL			6 //winners get buy-in + money proportional to chips.
#define PRIZE_NEXT_FLIGHT_TOURN				7 //winners to go to the next flight tournament, #19167

// for MTL_LOBBY_TOURNAMENT_DEALING_STATUS
#define MT_BREAK_SCHEDULED	1
#define MT_BREAK_STARTED	2
#define MT_BREAK_FORCED		0x80

#define ALLIN_FOLDCONNECTED	(-3)
#define ALLIN_PRIVREVOKED	(-2)
#define ALLIN_NOALLINTOURN	(-1)

//PYR-37049 
#define DELAYED_PLACE_BASE 1000000000
#define DELAYED_PLACE_CUTOFF (DELAYED_PLACE_BASE/2)

//#define TEMP_PWD_PREFIX	"tmp" ///moved to dbm.h

enum QuestionaireTypes
{
	FriendReferral = 1,
	TVAd = 2,
	RadioAd = 3,
	Magazine = 4,
	WebSite = 5,
	Newsgroup = 6,
	SearchEngine = 7,
	TVShow = 8,
	RadioShow = 9,
	Newspaper = 10,
	MagazineAd = 11,
	Other = 12,
	TV = 13,
	Radio = 14,
	EPT = 15,
	YahooOnline = 16,
	Billboard = 17,
	MagazineAll = 18,
	AstonMartinRacing = 19,
	MotorRacing = 20,
	BoxingEvent = 21,
	CinemaAdvertising = 22,
	LAPT = 23,
	AAPT = 24,
};

enum BotDetectionCodes
{
	ProhibitedApp = 1,
	Network = 2
};

enum LobbyFlyerAttribute
{
	FlyerAttribTourn = 1,

	FlyerAttribTableFirst = 2,
	FlyerAttribHoldemPM = 2,
	FlyerAttribHoldemPMFL = 3,
	FlyerAttribHoldemPMNL = 4,
	FlyerAttribHoldemPMPL = 5,
	FlyerAttribHoldemFL = 6,
	FlyerAttribHoldemFLHigh = 7,
	FlyerAttribHoldemFLMedium = 8,
	FlyerAttribHoldemFLLow = 9,
	FlyerAttribHoldemFLMicro = 10,
	FlyerAttribHoldemNL = 11,
	FlyerAttribHoldemNLHigh = 12,
	FlyerAttribHoldemNLMedium = 13,
	FlyerAttribHoldemNLLow = 14,
	FlyerAttribHoldemNLMicro = 15,
	FlyerAttribOmahaHL = 16,
	FlyerAttribOmaha = 17,
	FlyerAttribStudHL2 = 18,
	FlyerAttribStud2 = 19,
	FlyerAttribRazz = 20,
	FlyerAttribDraw = 21,
	FlyerAttribTripleDraw27 = 22,
	FlyerAttribHOSE = 23,
	FlyerAttribHORSE = 24,
	FlyerAttribBADUGI = 25,
	FlyerAttribSingleDraw27 = 26,
	FlyerAttribEightGame = 27,
	FlyerAttribHoldemMixed = 28,
	FlyerAttribOmahaHiLoMixed = 29,
	FlyerAttribHoldemOmahaPl = 30,
	FlyerAttribMultiTripleStud = 31,

	// #17098
	FlyerAttribHoldemRM = 32,
	FlyerAttribOmahaRM = 33,
	FlyerAttribOmahaPM = 34,

	//PYR-20220
	FlyerAttribHoldemRMNL = 35,
	FlyerAttribHoldemRMNLHigh = 36,
	FlyerAttribHoldemRMNLMedium = 37,
	FlyerAttribHoldemRMNLLow = 38,
	FlyerAttribHoldemRMNLMicro = 39,

	//PYR-20220
	FlyerAttribHoldemRMPL = 40,
	FlyerAttribHoldemRMPLHigh = 41,
	FlyerAttribHoldemRMPLMedium = 42,
	FlyerAttribHoldemRMPLLow = 43,
	FlyerAttribHoldemRMPLMicro = 44,

	//PYR-21911
	FlyerAttribOmaha_RM = 45,
	FlyerAttribOmahaRMHigh = 46,
	FlyerAttribOmahaRMMedium = 47,
	FlyerAttribOmahaRMLow = 48,
	FlyerAttribOmahaRMMicro = 49,
	FlyerAttribOmahaHLRM = 50,
	FlyerAttribOmahaHLRMHigh = 51,
	FlyerAttribOmahaHLRMMedium = 52,
	FlyerAttribOmahaHLRMLow = 53,
	FlyerAttribOmahaHLRMMicro = 54,
	FlyerAttribOmaha_PM = 55,
	FlyerAttribOmahaHLPM = 56,

	//PYR-24197
	FlyerAttribOmaha5RM = 57,
	FlyerAttribOmaha5HLRM = 58,
	FlyerAttribCourchevelRM = 59,
	FlyerAttribCourchevelHLRM = 60,

	FlyerAttribOmaha5PM = 61,
	FlyerAttribOmaha5HLPM = 62,
	FlyerAttribCourchevelPM = 63,
	FlyerAttribCourchevelHLPM = 64,

	FlyerAttribTableLast = 64,

	FlyerAttribBlitz = 1000,
	//maxFlyerAttrib = 1000,
};

enum MAAClosedType //#13069
{
	MAANotClosed            = 0, // PYR-42376
    MAAClosedExcluded       = 1, // wecare
    MAAClosedPermanent      = 2, // support
    MAAClosedDuplicatePM    = 3,
    MAAClosedDuplicateRM    = 4,
    MAAClosedRelClosed      = 5, // obsoleted by #22133
    MAAClosedNegBal         = 6,
    MAAClosedGovernmentId   = 7, //18956
    MAAClosedNoPlay         = 8, //18503
    MAAClosedCSU            = 9, //18503
    MAAClosedBanned         = 10, //18503 - obsoleted by #22133
    MAAClosedDuplicateOLD   = 11, //22133
};

enum TSSClosedType
{
    // continues from MAAClosedType
	TSSAutoActionsDefault	= 21,							// PYR-26639
	TSSBlacklistedCountry	= 22,							// PYR-26639
	TSSNonFrenchPlayer		= 23,							// PYR-26639
};

enum LoginClosedType
{
	// continues from TSSClosedType
	LoginSelfLocking		= 41,							// PYR-34643
};

//#14427
enum eFrAccValUserEvent
{
	eFrAccValUserEventNoEvent = 0,
	eFrAccValUserEventWaitDocs = 1,
	eFrAccValUserEventWaitBA = 2,
	eFrAccValUserEventSuspendedByDocs = 3,
	eFrAccValUserEventSuspendedByBA = 4,
	eFrAccValUserEventClosedByDocs = 5,
	eFrAccValUserEventWaitAvc = 6,
	eFrAccValUserEventSuspendedByAvc = 7,
	eFrAccValUserEventClosedByAvc = 8,
};

// PYR-18621
enum eItAccVerifUserEvent
{
	eItAccVerifUserEventNoEvent = 0,
	eItAccVerifUserEventWaitDocs = 1,
	eItAccVerifUserEventSuspendedByDocs = 3,
	eItAccVerifUserEventClosedByDocs = 5,
};

// PYR-20730
enum eAccVerifUserEvent
{
	eAccVerifUserEventNoEvent = 0,
	eAccVerifUserEventWaitDocs = 1,
	eAccVerifUserEventWaitNationalIDVerification = 2,
	eAccVerifUserEventSuspendedByDocs = 3,
	eAccVerifUserEventClosedByDocs = 5,
};

//PYR-18568
enum eBeAccVerifUserEvent
{
	eBeAccVerifUserEventNoEvent = 0,
	eBeAccVerifUserEventWaitAvc = 1,
	eBeAccVerifUserEventBanned = 2,
	eBeAccVerifUserEventSuspended = 3,
};

//PYR-15751
enum eMigrationFlag
{
	eMigrationFlagCancel = 0,
	eMigrationFlagOK = 1 << 0,
	eMigrationFlagConvertCashToFpp = 1 << 1, // PYR-27224

	eMigrationFlagLast = eMigrationFlagConvertCashToFpp // please keep up-to-date
};

// PYR-89650
enum eMigrationCheckFlags
{
	eMigrationCheckFlags_None = 0,
	eMigrationCheckFlags_Check = 1 << 0,

	eMigrationCheckFlagLast = eMigrationCheckFlags_Check // please keep up-to-date
};

// PYR-15607 - home games
enum ErrorCodeRegisterHomeGame
{
	eRegisterHGOk = 0,
	eRegisterHGNotFound = 1,
	eRegisterHGNotClubMember = 2,
	eRegisterHGCannotRegister = 3,
	eRegisterHGAlreadyRegistered = 4
};

//PYR-15846
enum eShowDealOption
{
	eShowDealOptionNoShow = 0,
	eShowDealOptionShowEnabled = 1,		//checkbox enabled, initially unchecked
	eShowDealOptionShowDisabled = 2,	//host arrived - checkbox disabled, checked
	eShowDealOptionShowEnabledSet = 3,		//checkbox enabled, all checked
};

//PYR-50837
enum eShowDealOptionFlags
{
	eShowDealOptionFlag_AutoDealEnabled = 1 << 0, // 0x01 - auto deal is enabled
	eShowDealOptionFlag_SuppressCheckboxClear = 1 << 1, // 0x02 - do not clear checkbox on transition
};

enum eDiscussDeal
{
	eDiscussDealDeclined = 0,	//one unchecked after all checked
	eDiscussDealBegin = 1,		//all checked
};

// PYR-15607 - error code if table cannot be created
#define ERR_CODE_HG_TABLE_CANNOT_BE_CREATED 1
#define ERR_CODE_HG_TABLE_CANNOT_BE_CREATED_NAME_NOT_UNIQUE 2


// #15261 - Options enabling/disabling "Fold and Show/Show One Card"
enum ShowSomeCardOptions
{
	eShowSomeCardsOff          = 0,		// feature completely off
	eShowSomeCardsTableSetting = 1,		// follow table type setting
	eShowSomeCardsPlayMoney    = 2,		// play money on, real money follow table type setting
	eShowSomeCardsOn           = 3,		// feature completely on
};

//PYR-18546
enum eTimeToStopMatchPayout
{
	eTimeToStopMatchPayoutNone			= 0,
	eTimeToStopMatchPayoutClassic		= 1,
	eTimeToStopMatchPayoutRefund		= 2,
	eTimeToStopMatchPayoutProportional	= 3,
};

enum eTimeToStopMatchType
{
	eTimeToStopMatchTypeNone		= 0,
	eTimeToStopMatchTypeLevels		= 1,
	eTimeToStopMatchTypeMinutes		= 2,
};

enum eWaitingListFlags
{
	eWaitingListFlagNone		= 0,
	eWaitingListFlagSeatManager	= (1 << 0),
	eWaitingListFlagFindSeat	= (1 << 1),

	ENUM_LAST_VALUE(eWaitingListFlags) // defines eWaitingListFlags_Last & eWaitingListFlags_NextAfterLast
};

// PYR-18476
#define POKERSTARS_ADMIN_NAME "PokerStars Admin"
#define SYSTEM_ADMIN_NAME "System"

// eHgAuditSubactionType is used to trace actions related to HG
// which bypasses home lobby, on each action 
// home lobby should be notified by lobby with requestClubGameAudit()
enum eHgAuditSubactionType
{
	eHgAuditSubactionDoNotLog							= 0, //action needn't to be recorded (usually already is)
	eHgAuditSubactionAdminCreateTourn					= 1,
	eHgAuditSubactionAdminCreateMultiTourn				= 2,
	eHgAuditSubactionAdminCloneTourn					= 3,
	eHgAuditSubactionAdminUpdateTourn					= 4,
	eHgAuditSubactionAdminRedeployTourn					= 5,
	eHgAuditSubactionAdminCancelTournBeforeSitting		= 6,
	eHgAuditSubactionAdminCancelDestroyTourn			= 7,
	eHgAuditSubactionAdminCancelDestroyTournWithUsers	= 8,
	eHgAuditSubactionAdminCancelRangeTourn				= 9,
	eHgAuditSubactionAdminCancelRunningTourn			= 10,
	eHgAuditSubactionAdminRestartTourn					= 11,
	eHgAuditSubactionAdminKickUsersCloseTable			= 12,

	eHgAuditSubactionSystemCancelTournNoPlayers			= 100,
	eHgAuditSubactionSystemCancelTournItaNoSession		= 101,
	eHgAuditSubactionSystemCancelTournItaNotConfirmed   = 102,


	//insert new values here
	eHgAuditSubactionTypeLast			
};

enum updatableMtLobbyParams
{
	eUpdateTournParamPrelimAwards = 1,
	eUpdateTournEnableAutoDeals = 2,
	eUpdateTournSupportAutoDealDisconnections = 3,
	eUpdateTournSatelliteWinnerRugCheck = 4,
	eUpdateNumSitoutsForBalancing = 5,
	eUpdateMovingBetweenFlightsEnabled = 6,
	eUpdateSendEmailOnFlightUserMoveEnabled = 7,
	eUpdateConfigMtlFlags = 8, // PYR-41406
	eUpdateMtLobbyMsgChecker = 9,  // PYR-52360
	eUpdateMtTableMsgChecker = 10,  // PYR-52360
	eUpdateEnableLogMonitorAggregation = 11, //PYR-69655
	eUpdateCheckSpainDummyTourns = 12, // PYR-84666
	eUpdateFlightMultiEntryLicenseMask = 13, // PYR-108205
	eUpdateTournEnableSagDeals = 14, // PYR-105481 
	eUpdateTournSpendLimits = 15, // PYR-90566
	eUpdatableTournParamLast
};

enum UpdateTgmParams
{
	eUpdateTgmParamsFlightMultiEntryLicenseMask = 1, // PYR-108205
	eUpdateTgmParamsLast
};

enum SpeedDisplayOptions
{	// #16783:
	SpeedDisplayNone = 0,
	SpeedDisplayRegular = 1,
	SpeedDisplayTurbo = 2,
	SpeedDisplayHyper = 3,
	SpeedDisplaySlow = 4,

	SpeedDisplay_LastSlot   // keep it last item
};

enum eBlitzLobbyStatus
{
	eBlitzLobbyStatus_None	= 0,	//just created -> Play
	eBlitzLobbyStatus_Play	= 1,	//normal operations
	eBlitzLobbyStatus_Stop	= 2,	//no more playing
	eBlitzLobbyStatus_Die	= 3,	//going to die
};

enum eLobbyTournStatus
{// #21822 - status will be sent to a tournament on creation
    eLobbyTournStatus_Paused    = 0,    // created tournament should be on break
    eLobbyTournStatus_Normal    = 1,    // created tournament should operate normally
};

//PYR-22528
enum eTournPrizePoolUnit
{
	eTournPrizePoolUnit_None = 0,
	eTournPrizePoolUnit_Mask = 127,
	eTournPrizePoolUnit_Plus = 128,

	eTournPrizePoolUnit_PM = 1,
	eTournPrizePoolUnit_RM = 2,
	eTournPrizePoolUnit_Fpp = 3,
	eTournPrizePoolUnit_Seat = 4,
	eTournPrizePoolUnit_SeatPlus = (eTournPrizePoolUnit_Seat + eTournPrizePoolUnit_Plus),
	eTournPrizePoolUnit_Ticket = 5,
	eTournPrizePoolUnit_TicketPlus = (eTournPrizePoolUnit_Ticket + eTournPrizePoolUnit_Plus),
	eTournPrizePoolUnit_Package = 6,
	eTournPrizePoolUnit_PackagePlus = (eTournPrizePoolUnit_Package + eTournPrizePoolUnit_Plus),
};

enum BlitzTableType
{
	bttNone = 0,
	bttRingGame = 1,
	bttTournament = 2
};

enum TournDestroyType
{
	tdtCleanup = 0,
	tdtCancel = 1,
	tdtRecreate = 2
};

// PYR-25597
enum ClientArjelOfflineEventFlags
{
	eArjelOfflineAccountChange	= 0x01,
	eArjelOfflineTransactions	= 0x02,
	eArjelOfflineTournaments	= 0x04,
};

// PYR-26104
enum OtherFlags
{
	eSmsPwdResetIniSettingsOn  = 0x01,
	eCanUseNetverify           = 0x02,
};

// PYR-47376
enum eSimpleAutoDealState
{
	eSimpleAutoDealState_NoDealStarted = 0,     // no deal in progress
	eSimpleAutoDealState_DealInProgress = 1,    // deal in progress
	eSimpleAutoDealState_DealAgreedDelay = 2,   // deal agreed, delay for player notification, will transition to eSimpleAutoDealState_DealAgreed when elapsed
	eSimpleAutoDealState_DealFailedDelay = 3,   // deal failed, delay for player notification, will transition to eSimpleAutoDealState_NoDealStarted when elapsed. See eAutoDealFailureState for failure reason.
	eSimpleAutoDealState_DealAgreed = 4         // deal agreed and tournament has resumed
};

enum eDealType
{
	eDealType_None = 0,   // not a deal, serves just for MTL_USER_DEAL_UPDATE_PROPOSAL
	eDealType_ICM = 1,
	eDealType_ChipChop = 2,
	eDealType_Custom = 3,
	eDealType_EqualSplit = 4,
	eDealType_Last
};

enum eAutoDealContinuationStateBits    // details of what the tournament continues for after the deal made
{
	eAutoDealContinuationStateBits_Init                        =  0x0,
	eAutoDealContinuationStateBits_WasAutoDealDone             =  1,    // 0x1,
	eAutoDealContinuationStateBits_TournContinuesAfterTheDeal  =  1<<1, // 0x2,
	eAutoDealContinuationStateBits_PlayingForMoney             =  1<<2, // 0x4, 
	eAutoDealContinuationStateBits_PlayingForTLBPoints         =  1<<3, // 0x8
	eAutoDealContinuationStateBits_PlayingForKOBounties        =  1<<4, // 0x10
};

enum eAutoDealFailureState  // details of why the deal was not made after when auto deal session finishes
{
	eAutoDealFailureState_NoFailure = 0,
	eAutoDealFailureState_DealCancelledByPlayer = 1,
	eAutoDealFailureState_AdminToolIntervention = 2,
	eAutoDealFailureState_SystemError = 3,
	eAutoDealFailureState_Timeout = 4,
	eAutoDealFailureState_PlayerDisconnected = 5,
};

enum eBlitzKickUserFlag
{
	eBlitzKickUserFlag_None = 0
	, eBlitzKickUserFlag_StickyFold = 1		//0x01
	, eBlitzKickUserFlag_creditStuckUsers = 2	//0x02
};

enum eTournUserFinishFlags
{
	eTournUserFinishFlags_EntryFlight = 1,
	eTournUserFinishFlags_Satellite = 1<<1, //0x2
	eTournUserFinishFlags_CustomPrize = 1<<2, //0x4
	eTournUserFinishFlags_ReEntryAvailable = 1<<3, //0x8 PYR-50362
};
//PYR-26095
#define AAMS_MAX_ENTRY_FLIGHTS_NUMBER 16 

//PYR-32543
#define NUM_SITOUTS_FOR_TOURN_BALANCING 400

// PYR-32547
enum eTemplateUpdateNotify
{
	eTemplateUpdateNotify_Created = 1,
	eTemplateUpdateNotify_Deleted = 2,
	eTemplateUpdateNotify_Updated = 3,
};

// PYR-34698
enum eKickUserOutReason
{
	eKickUserOutDefault			= 0,
	eKickUserOutGeoIPUnknown	= 1,
	eKickUserOutPGADDown		= 2,
};

// PYR-35275
enum eClientReportType
{
	eClientReportType_PlayAudit			= 0,
	eClientReportType_EsAccountHistory	= 1,
	eClientReportType_FppAudit			= 2,
	eClientReportType_MGSideBetAudit	= 3,
	// IMPORTANT: always update eClientReportType_Last
	eClientReportType_Last				= eClientReportType_MGSideBetAudit,
};

// PYR-36408
enum eObjStateTypes
{
	eObjStateTypes_Table = 1,
	eObjStateTypes_Tourn = 2,
	eObjStateTypes_Blitz = 3,
};

// PYR-36408
enum eTournStatusMask
{
	eTournStateMask_Announced =			(1<<0), // 1
	eTournStateMask_Seating =			(1<<1), // 2
	eTournStateMask_Running =			(1<<2), // 4
	eTournStateMask_Closed =			(1<<3), // 8
	eTournStateMask_Canceled =			(1<<4), // 16
	eTournStateMask_CanceledByAdmin =	(1<<5), // 32
};

// PYR-41406
enum eMtLobbyConfigFlags
{
	eMtLobbyConfigFlags_None = 0,
	eMtLobbyConfigFlags_RegUserBeforeAamsCheck = 1 << 0,
};

enum eSngSegment //PYR-44908
{
	eSngSegment_Invalid = -1,
	eSngSegment_Regular = 0,
	eSngSegment_PooledSng = 1,
	eSngSegment_Sag = 2,
	eSngSegment_Storm = 3, //PYR-44908
	eSngSegment_ProgressivePlus = 4, // PYR-114749
};

enum eMsgCheckerModuleType
{
	eMsgCheckerModuleType_Lobby      = 1<<0,  // 0x01
	eMsgCheckerModuleType_MtLobby    = 1<<1,  // 0x02
	eMsgCheckerModuleType_BlitzLobby = 1<<2,  // 0x04
	eMsgCheckerModuleType_RingTable	 = 1<<3,  // 0x08
	eMsgCheckerModuleType_BlitzTable = 1<<4,  // 0x10
	eMsgCheckerModuleType_MtTable	 = 1<<5,  // 0x20
	eMsgCheckerModuleType_BlitzReplay = 1<<6, // 0x40
	eMsgCheckerModuleType_ServerReplay = 1<<7,  // 0x80
};

enum eTableChatCategoryType
{
	eTableChatCategoryType_Players		= 1<<0, //0x01 
	eTableChatCategoryType_Observers	= 1<<1, //0x02
};

// PYR-53981
enum updatableBlitzLobbyParams
{
	eUpdateBlitzLobbyMsgChecker = 1<<0, //0x01 
	eUpdateBlitzTableMsgChecker = 1<<1, //0x02
	eUpdateBlitzReplayMsgChecker = 1<<2, //0x04
};

enum tableBroadcastUpdateId //PYR-68999
{
	tableBroadcastUpdateId_LogRotationPeriodId	= 0,
	tableBroadcastUpdateId_TableFeature			= 1,
	tableBroadcastUpdateId_ReservationsReportData = 2,
};

enum eLogoutFlags
{
	eLogoutFlags_Timeout = 1 << 0, //0x01 
}; 

#endif	//lobby_h_included
