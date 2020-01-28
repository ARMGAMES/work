#ifndef	newsmessages_h_included
#define	newsmessages_h_included

#include "ppincludebase.h"
#include "ppstring.h"
#include "pyrtime.h"
#include "sites.h"
#include "PBitmask.h"
#include "sitemasks.h"

#define MSG_NEWS_REREAD_INI			0x1 //4 - reqId
#define MSG_NEWS_REREAD_INI_REPLY	0x2 //4 - reqId

/* obsolete by PYR-46073
#define MSG_NEWS_ADD_UPDATE			0x1000 //4S ReqId,NewsItem
#define MSG_NEWS_ADD_UPDATE_REPLY	0x2000 //46[s] ReqId, err_code [errDescr]

#define MSG_NEWS_EXPIRE_NOW			0x1001 //44 ReqId,News Version
#define MSG_NEWS_EXPIRE_NOW_REPLY	0x2001 //46[s] ReqId, err_code [errDescr]
*/
#define MSG_NEWS_UPDATED_I18N			0x1002 //444 ReqId,TemplateId,locale
#define MSG_NEWS_UPDATED_I18N_REPLY		0x2002 //46[s] ReqId, err_code [errDescr]

// obsolete by PYR-117290
//#define MSG_LOBBYIMAGE_UPDATED		0x1003 //44s - reqId,siteId,country
//#define MSG_LOBBYIMAGE_UPDATED_REPLY	0x2003 //42[s] - reqId,errCode,[errDescr]

#define MSG_PROMOIMAGE_UPDATED			0x1004 //4 - reqId
#define MSG_PROMOIMAGE_UPDATED_REPLY	0x2004 //42[s|4<s4S>] - reqId,errCode,[errDescr|numItems< code, imageId, description >]

// obsolete by PYR-118214
//#define MSG_TABLEFELTGRAPHIC_UPDATED		0x1005 //44o - reqId,siteMask32,sitePBitmask 
//#define MSG_TABLEFELTGRAPHIC_UPDATED_REPLY	0x2005 //42[s] - reqId,errCode,[errDescr]

#define MSG_PIPEITEM_UPDATED			0x1006 //44 - reqId,templateId
#define MSG_PIPEITEM_UPDATED_REPLY		0x2006 //46[s] - reqId,errCode,[errDescr]

#define MSG_PIPEITEM_DELETED			0x1007 //44 - reqId,templateId
#define MSG_PIPEITEM_DELETED_REPLY		0x2007 //46[s] - reqId,errCode,[errDescr]

#define MSG_PIPEIMAGE_UPDATED			0x1008 //4s - reqId,imageName
#define MSG_PIPEIMAGE_UPDATED_REPLY		0x2008 //46[s] - reqId,errCode,[errDescr]

#define MSG_PIPEIMAGE_DELETED			0x1009 //4s - reqId,imageName
#define MSG_PIPEIMAGE_DELETED_REPLY		0x2009 //46[s] - reqId,errCode,[errDescr]

#define MSG_TOURNBRAND_UPDATED			0x100A //4s - reqId,brandName
#define MSG_TOURNBRAND_UPDATED_REPLY	0x200A //46[s] - reqId,errCode,[errDescr]

#define MSG_TOURNBRAND_DELETED			0x100B //4s - reqId,brandName
#define MSG_TOURNBRAND_DELETED_REPLY	0x200B //46[s] - reqId,errCode,[errDescr]

#define MSG_LIVEEVENT_UPDATED			0x100C //44 - reqId,templateId
#define MSG_LIVEEVENT_UPDATED_REPLY		0x200C //46[s] - reqId,errCode,[errDescr]

#define MSG_LIVEEVENT_DELETED			0x100D //44 - reqId,templateId
#define MSG_LIVEEVENT_DELETED_REPLY		0x200D //46[s] - reqId,errCode,[errDescr]

#define MSG_MOBILE_GENNEWS_UPDATE			0x100E //44 - reqId, templateId
#define MSG_MOBILE_GENNEWS_UPDATE_REPLY		0x200E //46[s] - reqId,errCode,[errDescr]

#define MSG_TARGETNEWS_UPDATE				0x100F //444 - reqId, templateId, templateType /* PYR-29329, 29328 */ 
#define MSG_TARGETNEWS_UPDATE_REPLY			0x200F //46[s] - reqId,errCode,[errDescr]

#define MSG_MOBILE_GENNEWS_DELETED			0x1010 //44 - reqId, templateId
#define MSG_MOBILE_GENNEWS_DELETED_REPLY	0x2010 //46[s] - reqId,errCode,[errDescr]

#define MSG_TARGETNEWS_DELETED				0x1011 //44 - reqId, templateId
#define MSG_TARGETNEWS_DELETED_REPLY		0x2011 //46[s] - reqId,errCode,[errDescr]

// PYR-26712
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_TAB_ITEM_UPDATED		0x1012 //44 - reqId,templateId
#define MSG_TAB_ITEM_UPDATED_REPLY	0x2012 //46[s] - reqId,errCode,[errDescr]

#define MSG_TAB_ITEM_DELETED		0x1013 //44 - reqId,templateId
#define MSG_TAB_ITEM_DELETED_REPLY	0x2013 //46[s] - reqId,errCode,[errDescr]

// PYR-32225
#define MSG_PIPEIMAGE2_UPDATED			0x1014	// 44 - reqId,templateId
#define MSG_PIPEIMAGE2_UPDATED_REPLY	0x2014 //46[s] - reqId,errCode,[errDescr]

#define MSG_PIPEIMAGE2_DELETED			0x1015	// 44 - reqId,templateId
#define MSG_PIPEIMAGE2_DELETED_REPLY	0x2015 //46[s] - reqId,errCode,[errDescr]

// PYR-44339
#define MSG_MOBILE_XML_CONFIG_UPDATE		0x1016 //44 - reqId, templateId
#define MSG_MOBILE_XML_CONFIG_UPDATE_REPLY	0x2016 //46[s] - reqId,errCode,[errDescr]

#define MSG_MOBILE_XML_CONFIG_DELETED		0x1017 //44 - reqId, templateId
#define MSG_MOBILE_XML_CONFIG_DELETED_REPLY	0x2017 //46[s] - reqId,errCode,[errDescr]

// PYR-58846
#define MSG_MORPH_LOBBY_UPDATE				0x1018 //44 - reqId, templateId
#define MSG_MORPH_LOBBY_UPDATE_REPLY		0x2018 //46[s] - reqId,errCode,[errDescr]

#define MSG_MORPH_LOBBY_DELETED				0x1019 //44 - reqId, templateId
#define MSG_MORPH_LOBBY_DELETED_REPLY		0x2019 //46[s] - reqId,errCode,[errDescr]

// PYR-78942
#define MSG_CUSTOM_LOBBY_UPDATE				0x101A //44 - reqId, templateId
#define MSG_CUSTOM_LOBBY_UPDATE_REPLY		0x201A //46[s] - reqId,errCode,[errDescr]

#define MSG_CUSTOM_LOBBY_DELETED			0x101B //44 - reqId, templateId
#define MSG_CUSTOM_LOBBY_DELETED_REPLY		0x201B //46[s] - reqId,errCode,[errDescr]

#define MSG_CUSTOM_LOBBY_WIDGET_UPDATE				0x101C //44 - reqId, templateId
#define MSG_CUSTOM_LOBBY_WIDGET_UPDATE_REPLY		0x201C //46[s] - reqId,errCode,[errDescr]

#define MSG_CUSTOM_LOBBY_WIDGET_DELETED				0x101D //44 - reqId, templateId
#define MSG_CUSTOM_LOBBY_WIDGET_DELETED_REPLY		0x201D //46[s] - reqId,errCode,[errDescr]



#define TRACK_CODE_PREFIX			"[Trk]:"
#define PAYSYSTEM_PREFIX			"[Pay]:"
#define INCLUDE_GEO_PREFIX			"[Geo]:"
#define EXCLUDE_GEO_PREFIX			"[ExGeo]:"
#define INCLUDE_VIP_PREFIX			"[Vip]:"
#define LTFPP_PREFIX				"[Ltfpp]:"
#define INTELLI_POKER				"[IP]:"
#define SITE_PREFIX					"[Site]:"
#define COUNT_PREFIX				"[Count]:"
#define PROMO_CODE_PREFIX			"[Promo]:"
#define SUBPAYSYSTEM_PREFIX			"[PaySS]:" // [AO 2010-09-09] #14359
#define SITES_PREFIX				"[Sites]:" // PYR-21728
#define PMRM_PREFIX					"[PMRM]:"  // PYR-21728 (for PIPES see ePIPEFlags 0-1)

//PYR-16500
#define INCLUDE_FLAGS_PREFIX		"[InFl]:"
#define INCLUDE_FLAGS2_PREFIX		"[InFl2]:"
#define INCLUDE_FLAGS3_PREFIX		"[InFl3]:" // PYR-47708
#define EXCLUDE_FLAGS_PREFIX		"[ExFl]:"
#define EXCLUDE_FLAGS2_PREFIX		"[ExFl2]:"
#define EXCLUDE_FLAGS3_PREFIX		"[ExFl3]:" // PYR-47708
#define INCLUDE_PRIVS_PREFIX		"[InPr]:"
#define INCLUDE_PRIVS2_PREFIX		"[InPr2]:"
#define INCLUDE_PRIVS3_PREFIX		"[InPr3]:"
#define EXCLUDE_PRIVS_PREFIX		"[ExPr]:"
#define EXCLUDE_PRIVS2_PREFIX		"[ExPr2]:"
#define EXCLUDE_PRIVS3_PREFIX		"[ExPr3]:"
#define LOCALE_PREFIX				"[Lcl]:"
#define EMLOCALE_PREFIX				"[EmLcl]:"
#define FPP_PREFIX					"[Fpp]:"
#define TOPFPP_PREFIX				"[TopFpp]:"
#define TOPLTFPP_PREFIX				"[TopLtfpp]:"
#define EXCLUDE_TRACK_CODE_PREFIX	"[ExTrk]:"
#define BALANCE_PREFIX				"[Bal]:"
#define TOPBALANCE_PREFIX			"[TopBal]:"
#define REGISTERED_PREFIX			"[Reg]:"
#define TOPREGISTERED_PREFIX		"[TopReg]:"

// PYR-19875
#define LASTLOGIN_PREFIX			"[LstLgin]:"
#define TOPLASTLOGIN_PREFIX			"[TopLstLgin]:"
#define MVPP_PREFIX					"[MVpp]:"
#define TOPMVPP_PREFIX				"[TopMVpp]:"
#define YVPP_PREFIX					"[YVpp]:"
#define TOPYVPP_PREFIX				"[TopYVpp]:"

#define PRIORITY_PREFIX				"[Pri]:"

#define JUBILEEHAND_PREFIX			"[Hnd]:" // PYR-18842
#define SEX_PREFIX					"[Sex]:" // PYR-22916

#define DOMAIN_PREFIX				"[Domn]:" // PYR-23312
#define LIVECODE_PREFIX				"[Code]:" // PYR-20438

#define PLATFORM_PREFIX				"[Ptfs]:"	// PYR-24582

// PYR-24812
#define CLIENTTYPE_PREFIX			"[CliTyp]:"
#define CLIENTDEVICE_PREFIX			"[CliDev]:"

#define RMTRIGGERPOINTS_PREFIX		"[TrPt]:" // PYR-24844 (see ePIPEFlags 2-5)

#define CLIENTSOURCE_PREFIX			"[CliSrc]:"	// PYR-25096

#define TABLETYPE_PREFIX			"[TabTyp]:"	//PYR-26165

// PYR-21440
#define SUBJECT_PREFIX				"[Subj]:"  // edit box
#define CATEGORY_LABELS_PREFIX		"[Lbl]:" // edit box
#define DISPLAY_ON_FORM_PREFIX		"[DispYN]:" // Display on the form radio buttons (yes, no)
#define HELP_ID_PREFIX				"[HlpId]:" // help content combobox
#define FAQ_LINK_PREFIX				"[FAQ]:" // Display FAQ link check box
#define SHOW_FIELDS_MASK_PREFIX		"[Flds]:" // check boxes (language, subject, body, attachment)
#define CONTACT_TYPE_PREFIX			"[Contact]:" // radio buttons (email, chat, phone)
#define BRAND_PREFIX				"[Brnd]:" // checkbox (see BrandTypes)

#define LOBBY_TABTYPE_PREFIX		"[LbyTpe]:"		// PYR-34146

// PYR-27034
#define CONTENT_PREFERENCE_PREFIX	"[ContPref]:" // 0 - no preferences, 1 - PokerStars, 2 - Casino, 4 - Sports book
// PYR-35628
#define NUDGE_PREFIX				"[Nudge]:"		// 0 - don't show nudge, 1 - show nudge

// PYR-34047
#define INTIDMODULO_DIVISOR_PREFIX		"[ModDiv]:"
#define INTIDMODULO_REMAINDERS_PREFIX	"[ModRem]:"

// PYR-38678
#define USER_INT_ID_RANGE_PREFIX	"[UsrIntIdRng]:" // edit box

#define PIPE_PRODUCT_PREFIX			"[Prd]:" // PYR-35484
#define NO_BLOCK_PREFIX				"[NoBlk]:" // PYR-38483

// PYR-44339
#define PERCENTAGE_PREFIX			"[Prcnt]:"  // 1% - 100%
#define AMS_TRACKERS_PREFIX			"[AmsTrk]:" // edit box
// PYR-48434
#define USER_INT_ID_LAST_DIGITS_PREFIX	"[UsrIntIdLstDgst]:" // edit box

// PYR-48421
#define LANG_SENS_PREFIX			"[LangSens]:" // radio button

// PYR-64857
#define PREF_CURRENCY_PREFIX		"[PrefCurr]:"

//PYR-59862 
#define VISIBILITY_CRITERIA_PREFIX	"[VisCrt]:"

#define HELP_SUPPORT_PORTAL_PREFIX		"[HelpSupport]:"	// PYR-58846
#define GAME_TYPE_PREFIX				"[GameType]:"	// PYR-58846
#define TAB_PREFIX						"[Tab]:"	// PYR-64785
#define HELP_SUPPORT_PORTAL_SHOW_AS_NEW_PREFIX		"[HspNew]:"	// PYR-64785
#define HELP_SUPPORT_PORTAL_DEFAULT_TAB_PREFIX		"[HspDefault]:"	// PYR-64785
#define DEFAULT_WEIGHT_PREFIX	"[DefWeight]:"	// PYR-64785
#define WIDGET_TOURN_BRANDING	"[WtBrand]:"	// PYR-78942

#define SITES_EX_PREFIX				"[SitesEx]:" // PYR-123378

enum ContentPreference // PYR-27034
{
	NoContentPreference = 0,
	PokerStarsPref = (1 << 0), 
	CasinoPref = (1 << 1), 
	SportsBookPref = (1 << 2), 
};

enum ClientDownloadSource		// PYR-25096
{ 
	Unknown_downloadSource = 0,
	PokerStars = (1 << 0), 
	GooglePlay = (1 << 1), 
	Amazon = (1 << 2), 
	Samsung = (1 << 3), 
	Yandex = (1 << 4),
};

enum eNewsPlatform
{
	eDesktopWinPlatform = (1 << 0),
	eDesktopMacPlatform = (1 << 1),
	eIosPhonePlatform = (1 << 2),
	eIosTabletPlatform = (1 << 3),
	eAndroidPhonePlatform = (1 << 4),
	eAndroidTabletPlatform = (1 << 5),
};

enum eTargetLobbyBannerTemplateFlags		// PYR-30654 move from NewServer.h
{
	eTargetLobbyBannerPS6Flag	= (1 << 0),		// enum ClientType
	eTargetLobbyBannerPS7Flag	= (1 << 1),		// enum ClientType
	eTargetLobbyBanner2Flag		= (1 << 2),
};

enum eLobbyTabTypes		// PYR-34146
{
	eLobbyTabQuickSeatCashGame		= (1 << 0),
	eLobbyTabQuickSeatTournament	= (1 << 1),
	eLobbyTabMainLobbyCashGame		= (1 << 2),
	eLobbyTabMainLobbyZoom			= (1 << 3),
	eLobbyTabMainLobbySitNGo		= (1 << 4),
	eLobbyTabMainLobbySpinGo		= (1 << 5),
	eLobbyTabMainLobbyTournament	= (1 << 6),
	eLobbyTabMainLobbyOnlineEvent	= (1 << 7),
	eLobbyTabMainLobbyLiveEvent		= (1 << 8),
	eLobbyTabCasino					= (1 << 9),
	eLobbyTabSportsBook				= (1 << 10),
	eLobbyTabQuickSeatKoPoker		= (1 << 11),
	eLobbyTabVegas					= (1 << 12),
	eLobbyTabBeatTheClock			= (1 << 13),
	eLobbyTabPupil					= (1 << 14),
	eLobbyTabQuickSeatSpinGo 		= (1 << 15),
	eLobbyTabSocialCasino 			= (1 << 16), // PYR-52963
	eLobbyTabMorph					= (1 << 17), // PYR-58846
	eLobbyTabGameAppPage			= (1 << 18), // PYR-115618
};

//Criteria sample
//============================
//News for players from US states Michigan,Pennsylvania,Florida & England & Germany who used Visa Or MAsterCard,
//[Pay]:0x11,[Geo]:US(MI,PA,FL),GB,DE 
//New for players who belong to one of the 3 track codes
//[Trk]:TrackCode1,TrackCodeN,AnotherTrackCode"
//New for players who aren't from United States and Canada
//[ExGeo]:US,CA"
//Players with equal or more than 100 LT FPPs
//[Ltfpp]:100
//Players with german locale 
//[Lcl]:4

//for i18n: locale obsoleted
//PM Players on .com and .net
//[Site]:0x6 // PYR-70721 format obsolete, use [Sites] and [PMRM] tags instead
//Display once
//[Count]:1
//Display sticky
//[Count]:0

// #14359
// [PaySS]:<string>

enum
{
	NewsOnce = 1,
	NewsSticky 
};

enum
{
	NewsNameChanged		= 0x0001,
	NewsBodyChanged		= 0x0002,
	NewsPriorityChanged	= 0x0004,
	NewsDisplayChanged	= 0x0008,
	//NewsSiteChanged		= 0x0010, // PYR-70721 obsolete
	NewsStartChanged	= 0x0020,
	NewsEndChanged		= 0x0040,
	NewsCriteriaChanged	= 0x0080,
	NewsAllChanged		= 0x00FF
};

enum
{
	NewsVIPBronze			= 0x0001,
	NewsVIPSilver			= 0x0002,
	NewsVIPGold				= 0x0004,
	NewsVIPPlatinum			= 0x0008,
	NewsVIPSuperNova		= 0x0010,
	NewsVIPEliteSuperNova	= 0x0020,
};
// PYR-21728
enum MoneyType
{
	PLAY_MONEY = 0x01,
	REAL_MONEY = 0x02
};

// PYR-24844
enum ePIPEFlags
{
	ePlayMoney = PLAY_MONEY,
	eRealMoney = REAL_MONEY,
	eRMTriggerPointDeposit = (eRealMoney << 1),
	eRMTriggerPointCashout = (eRealMoney << 2),
	eRMTriggerPointEnterRMGame = (eRealMoney << 3),
	eRMTriggerPointReceiveCredit = (eRealMoney << 4),
	eOddInstallId = (eRealMoney << 5),		// PYR-26552
	eEvenIntallId = (eRealMoney << 6),		// PYR-26552
	ePokerStarsPref = (eRealMoney << 7),	// PYR-27034 
	eCasinoPref = (eRealMoney << 8),		// PYR-27034 
	eSportsBookPref = (eRealMoney << 9),	// PYR-27034 
	eDisplayNudge = (eRealMoney << 10),		// PYR-35628 
	eXmlConfig =  (eRealMoney << 11),		// PYR-44339 
	eLangSensitivity = (eRealMoney << 12),	// PYR-48421 
};

// PYR-29328
enum eTargetNewsType
{
	eTargetNewsGeneral = 0x01,
	eTargetNewsLobbyBanners = 0x02,
	eTargetNewsTableMessage = 0x04,

	// NOTE: add new types here also needs to change eTargetNewsAll !!!
	eTargetNewsAll = (eTargetNewsGeneral | eTargetNewsLobbyBanners | eTargetNewsTableMessage),
};

struct NewsItem
{
	UINT32	newsVer;
	PString newsName;
	PString newsBody;
	UINT32	newsPriority;
	UINT32	newsDisplayType;
	SrvTime newsStartTime;
	SrvTime newsEndTime;
	PString newsCriteria;
	UINT32	changesMask;
	PSiteMask newsTargetSites;		// PYR-21728
	UINT32	newsTargetMoneyType;	// PYR-21728: play money / real money

	NewsItem()
	{
		newsPriority=0;
		newsVer=0;
		newsDisplayType=NewsOnce;
		newsStartTime.setNull();
		newsEndTime.setNull();
		changesMask = NewsAllChanged;
		newsTargetMoneyType = PLAY_MONEY | REAL_MONEY;
	}

	void print() const;
};

//PYR-26165
enum eTableMsgType
{
	eTableMsgType_None			=       0,
	eTableMsgType_Ring			= (1 << 0),
	eTableMsgType_ZoomRing		= (1 << 1),
	eTableMsgType_Tourn			= (1 << 2),
	eTableMsgType_TournSat		= (1 << 3),
	eTableMsgType_SnG			= (1 << 4),
	eTableMsgType_SnGSat		= (1 << 5),
	eTableMsgType_ZoomTourn		= (1 << 6),
};

enum Brand
{
	eBrandPokerStars = 0x01,
	eBrandFullTilt   = 0x02,
};

UINT32 /*enum ePIPEFlags*/ convertContentPreferenceToPipeFlag(UINT32 contentPreference /*enum ContentPreference*/);

// PYR-78942
enum eCustomLobbyNodeType //enumerates the nodes of the CustomLobbyPublisher
{
	eCustomLobbyNodeType_Knockout = 0,

	eCustomLobbyNodeType_End //should go the last
};

#endif //newsmessages_h_included
