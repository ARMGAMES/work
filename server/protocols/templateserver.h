#ifndef templateserver_h_included
#define templateserver_h_included

#include "dbm.h"
#include "clientplatforms.h" // PYR-37758

#define LOBBY_IMAGE_SITE_PREFIX			"[Site]:"
#define LOBBY_IMAGE_GEO_PREFIX			"[Geo]:"
#define LOBBY_IMAGE_GEO_PREFIX_EXCLUDE	"[ExGeo]:"
#define LOBBY_IMAGE_POSITION_PREFIX		"[Pos]:"
#define LOBBY_IMAGE_IMAGEID_PREFIX		"[ImId]:"
#define LOBBY_IMAGE_FLAGS_PREFIX		"[Flags]:"
#define LOBBY_IMAGE_PROMO_PREFIX		"[Promo]:"
#define SITES_PREFIX					"[Sites]:" // PYR-21728
//PYR-21295
#define ATTRIB_PREFIX					"[Attr]:"
#define INSTALLID_PREFIX				"[InsId]:"	// PYR-26552
#define LICENSEIDS_PREFIX				"[LicId]:"	// PYR-29926
#define PIPE_LOBBY_BANNER_TYPE			"[FlyTyp]:"	// PYR-31630
#define PIPE_IMAGE_NAME					"[ImgName]:"	// PYR-32225
// PYR-47846
#define DAYS_HOURS_RANGE_PREFIX			"[DHR]:" // examples: "12h"(12 hours), "1d"(24 hours=1 day), "180d+"(180 and more days), "30d-60d"(from 30 days until 60 days).
// PYR-96485
#define ZONETAG_PREFIX					"[Zonetag]:" // StarCRM zone string for the client, e.g. [Zonetag]:DRTESTZONE


enum eTemplateStatus
{
	eTemplateStatusInDevelopment = 0,
	eTemplateStatusActive = 1,
	eTemplateStatusArchived = 2
};

enum eTemplateAuditObjectType
{
	eTemplateAuditObjectTypeTemplate = 1,
	eTemplateAuditObjectTypeContent = 2,
	eTemplateAuditObjectTypeContextData = 3
};

enum eTemplateAuditAction
{
	eTemplateAuditActionCreated = 1,
	eTemplateAuditActionUpdated = 2,
	eTemplateAuditActionDeleted = 3,
	eTemplateAuditActionStatusChanged = 4,
	eTemplateAuditActionMassEmailed = 5,
	eTemplateAuditActionMassSms = 6, // PYR-23088
	eTemplateAuditActionReplaced = 7, // PYR-23002
	eTemplateAuditActionMassPushNotification = 8, // PYR-25781
};

enum eTemplateType
{
	eTemplateTypeSystem = 0,
	eTemplateTypeMassEmail = 1,
	eTemplateTypeLobbyMsg = 2, // obsolete
	eTemplateTypeTableMsg = 3,
	eTemplateTypeGenNews = 4, // obsolete
	eTemplateTypeTargetNews = 5,
	eTemplateTypeDefaultWelcomeEmail = 6,
	eTemplateTypeWelcomeEmail = 7,
	eTemplateTypeCashierPaysystemMsg = 8,
	eTemplateTypeAdminTools = 9,
	eTemplateTypeCashierREC = 10,
	eTemplateTypeSignupQuestionnaire = 11,
	eTemplateTypeLobbyMsg2 = 12, // obsolete
	eTemplateTypePmWinnerEmail = 13,
	//eTemplateTypeLobbyImage = 14, // PYR-117290 obsolete
	eTemplateTypePromoImage = 15,
	//eTemplateTypeTableFeltGraphic = 16, // PYR-118214 obsolete
	//eTemplateTypeAdvertisement = 17, // PYR-118214 obsolete
	//eTemplateTypeMilestoneHand = 18, // PYR-118214 obsolete
	//eTemplateTypeBlitzLobbyMsg = 19,	// PYR-117290 obsolete
	eTemplateTypeInternalErrorMsg = 20, // PYR-19814
	eTemplateTypeGeneralString = 21,	//PYR-21785
	// PYR-55590 obsolete
	//eTemplateTypeVipStatus = 22,	//PYR-21558
	eTemplateTypeLobbyBanner = 23,	//PYR-21295
	//PYR-20445
	eTemplateTypePipeNews = 24,
	eTemplateTypePipeGuides = 25,
	eTemplateTypePipeLobbyMsg = 26,
	eTemplateTypePipeLobbyMsg2 = 27,
	eTemplateTypeTournBrand = 28,	//PYR-20448
	eTemplateTypePipeImage = 29,
	eTemplateTypePromoAvatarDialog = 30, //PYR-23045
//PYR-20445
	eTemplateTypePipeQuickStartFlyerRingGames = 31,
	eTemplateTypePipeQuickStartFlyerZoom = 32,
	eTemplateTypePipeQuickStartFlyerSitAndGo = 33,
	eTemplateTypePipeQuickStartFlyerTournament = 34,
	eTemplateTypePipeQuickStartFlyerLiveEvents = 35,
	eTemplateTypePipeMainLobbyHeader = 36,
	eTemplateTypePipeMainLobbyZoomFlyer = 37,
	eTemplateTypePipePokerSchoolBanner1 = 38,
	eTemplateTypePipePokerSchoolBanner2 = 39,
	eTemplateTypePipePokerSchoolBanner3 = 40,
	eTemplateTypePipePokerSchoolBanner4 = 41,
	eTemplateTypePipePokerSchoolTeamProPlayerCornerForNovices = 42,
	eTemplateTypePipePokerSchoolTeamProPlayerCornerForMembers = 43,
	eTemplateTypeLiveEventsMapping = 44,	//PYR-20438
	eTemplateTypeMassSms = 45, // PYR-23088
	eTemplateTypeMobileGenNews = 46, // PYR-24582
	eTemplateTypeBlockingMsg = 47, // PYR-24812
	eTemplateTypeBonusLocalization = 48, // PYR-24241 (uses subtype as bonus id!!!)
	eTemplateTypePipeRMTriggerPoints = 49, // PYR-24844
	eTemplateTypeGoldenSnGBanner = 50,		//PYR-24382 (obsolete)
	eTemplateTypeEmailValidation = 51, // PYR-24310
	eTemplateTypePopupConfirmMsg = 52, // PYR-26155
	eTemplateTypeAccountScreenBanner = 53,	// PYR-26552
	eTemplateTypeAccountInfoBanner = 54,	// PYR-26552
	eTemplateTypeAccountCreatedBanner = 55,	// PYR-26552
	eTemplateTypeWelcomeScreenHeader = 56, // PYR-25536
	eTemplateTypeWelcomeScreenSidePanel = 57, // PYR-25536
	eTemplateTypeWelcomeScreenMoneyToggle = 58, // PYR-25536
	eTemplateTypeWelcomeScreenOverview = 59, // PYR-25536
	eTemplateTypeWelcomeScreenBanner = 60, // PYR-25536
	eTemplateTypeContactSupportForm = 61, // PYR-21440 (category - section code=0, sub-category - section code >0)
	eTemplateTypeTabs = 62, // PYR-26712
	eTemplateTypeImageGallery = 63, // PYR-26568
	eTemplateTypeLobbyMessage = 64,	// PYR-28848
	eTemplateTypeLobbyMessage2 = 65,	// PYR-28848
	eTemplateTypeAccountContinueRealMoney = 66,	// PYR-26552
	eTemplateTypeAccountContinuePlayMoney = 67,	// PYR-26552
	eTemplateTypeAccountExplore = 68,	// PYR-26552
	//eTemplateTypeDuelAppFirstLogin = 69, // PYR-38321 // PYR-53789 - obsolete
	// below 12 numbers can be reused after July '14 release.
	//eTemplateTypePipeCashGamePlayMoneyFlyers = 70, // PYR-29752
	//eTemplateTypePipeZoomRealMoneyFlyers = 71, // PYR-29752
	//eTemplateTypePipeZoomPlayMoneyFlyers = 72, // PYR-29752
	//eTemplateTypePipeSnGRealMoneyFlyers = 73, // PYR-29752
	//eTemplateTypePipeSnGPlayMoneyFlyers = 74, // PYR-29752
	//eTemplateTypePipeLiveEventRealMoneyFlyers = 75, // PYR-29752
	//eTemplateTypePipeLiveEventPlayMoneyFlyers = 76, // PYR-29752
	//eTemplateTypePipeTournPlayMoneyFlyers = 77, // PYR-29752
	//eTemplateTypePipeOnlineEventPlayMoneyFlyers = 78, // PYR-29752
	//eTemplateTypePipeTournFavoriteRealMoneyFlyers = 79, // PYR-29752
	//eTemplateTypePipeTournFavoritePlayMoneyFlyers = 80, // PYR-29752
	//eTemplateTypePipeTournRealMoneyFlyers = 81, // PYR-30305
	eTemplateTypeIceTaskDescription = 82,	// PYR-29926
	eTemplateTypeIceTaskReward = 83,	// PYR-29926
	eTemplateTypeIceTextData = 84,		// PYR-30401
	eTemplateTypeTargetLobbyBanner = 85,	// PYR-29329 
	eTemplateTypeMgLobbyMessage = 86,	// PYR-42273
	eTemplateTypeIcePromoSurfacing = 87,	// PYR-31090
	eTemplateTypeTargetTableMsg = 88,	// PYR-29328
	eTemplateTypePipeImages2 = 89,		// PYR-32225
	eTemplateTypeIceMultiPromoData = 90,	//PYR-32208
	eTemplateTypePipeSAGPromoText = 91,	// PYR-32224
	eTemplateTypeInstantBonusLocalization = 92,	// PYR-31831 (uses subtype as bonus id!!!)
	eTemplateTypeLeaderBoardContentTool = 93,	// PYR-33799
	eTemplateTypeLeaderBoardPrizeContentTool = 94,	// PYR-33799
	eTemplateTypeSAGPromoTicketImage = 95,	// PYR-34307
	eTemplateTypeSAGPromoTicketText = 96,	// PYR-34307
	eTemplateTypeSAGPromoPayoutTicketText = 97,	// PYR-33574
	eTemplateTypeSAGPromoPayoutTicketImage = 98,// PYR-33574
	eTemplateTypeMobileXMLConfig = 99,// PYR-44339
	eTemplateGameTicketLocalization = 100, // PYR-46163 (uses subtype as game ticket id!!!)
	eTemplateTypeSelfExclusion = 101, // PYR-47846
	eTemplateTypeEmailSignature = 102, // PYR-49335
	eTemplateTypeSelfExclusionExpire = 103, // PYR-50034
	eTemplateTypeMorphLobby = 104,	// PYR-58846
	eTemplateTypeChestText = 105,	// PYR-62106
	eTemplateTypeSAGLobbyName = 106,	//PYR-59682
	eTemplateTypeAlert = 107, // PYR-63486
	eTemplateTypeCustomLobbyTheme = 108, // PYR-78942
	eTemplateTypeCustomTournWidget = 109, // PYR-78942
	eTemplateTypeCustomClientImages = 110,	// PYR-73483
	eTemplateTypeBrandedLobby = 111,	// PYR-111857
	eTemplateTypeHubFeedItem = 112,	// PYR-124681
	// WHEN ADD NEW TYPE CHECK IF IT CAN USE SECTION CODES (1000-1100) FOR OTHER PURPOSES THAN TO SAVE ATTACHMENT.
	// IF IT IS THEN MODIFY TemplateDbManager::checkTemplateForAttachmentAdminRight BY SETTING check TO false FOR THIS TYPE.
	// IF NEW TEMPLATE TYPE IS FOR EMAILS THEN UPDATE FUNCTION isEmailTemplate
	eTemplateTypeMax //should be always the last
};

enum eTemplateSubtype
{
	eTemplateSubtypeSystem = 0,
	eTemplateSubtypeCRMConversionCycles = 1,
	eTemplateSubtypeCRMCreditPromoFunds = 2,
	eTemplateSubtypeCRMDebitPromoFunds = 3,
	eTemplateSubtypeDebitTeamProPlayingInSpecialTournaments = 4,
	eTemplateSubtypeRakebackMonthlyCreditNotification = 5,
	eTemplateSubtypeRevenueShareMonthlyCreditNotification = 6,
	eTemplateSubtypeWinningInAPokerLeague = 7,
	eTemplateSubtypeSecurity = 8,
	eTemplateSubtypeSecurity_CDMS = 9,
	eTemplateSubtypeSecurityAutoEmails = 10,
	eTemplateSubtypeSecurityBannedUserEmails = 11,
	eTemplateSubtypePRM = 12,
	eTemplateSubtypePLI = 13,
	eTemplateSubtypeAffiliateDeposit = 14,
	eTemplateSubtypeHandsHistory = 15,
	eTemplateSubtypeAffiliateNewsletter = 16,
	eTemplateSubtypeFrenchAccountValidation = 17,
	eTemplateSubtypeFrenchMigration = 18,
	eTemplateSubtypeFrenchBlacklist = 19,
	eTemplateSubtypeEstonianBlacklist = 20,
	eTemplateSubtypePaymentsCashout = 21,
	eTemplateSubtypePaymentServices = 22,
	eTemplateSubtypeHomeGamesDisapproveImageOnly = 23,
	eTemplateSubtypeHomeGamesDispproveImageName = 24,
	eTemplateSubtypeHomeGamesDispproveNameOnly = 25,
	eTemplateSubtypeHomeGamesDisapproveHeaderOnly = 26,
	eTemplateSubtypeHomeGamesDispproveHeaderName = 27,
	eTemplateSubtypeHomeGamesDispproveImageHeader = 28,
	eTemplateSubtypeHomeGamesDispproveImageNameHeader = 29,
	eTemplateSubtypeHomeGamesClubSettingsUpdated = 30,
	eTemplateSubtypeHomeGamesGuestAdded = 31,
	eTemplateSubtypeHomeGamesClubStateChanged = 32,
	eTemplateSubtypeHomeGamesFounderStatusTransferred = 33,
	eTemplateSubtypeHomeGamesClubPasswordReset = 34,
	eTemplateSubtypeHomeGamesGeneral = 35,
	eTemplateSubtypeWBCOOPApprove = 36,
	eTemplateSubtypeWBCOOPDeny = 37,
	eTemplateSubtypeWBCOOPHold = 38,
	eTemplateSubtypeSecurityCashoutHold = 39,
	eTemplateSubtypeSecurityCashoutDeclined = 40,
	eTemplateSubtypeSecurityTransferHold = 41,
	eTemplateSubtypeSecurityTransferDeclined = 42,
	eTemplateSubtypeVIPStoreOrderConfirmation = 43,
	eTemplateSubtypeVIPClubOrderConfirmation = 44,
	eTemplateSubtypeSecurityNegativeBalanceReminder = 45,
	eTemplateSubtypeReminderCycles = 46,
	eTemplateSubtypeMobileNumberValidated = 47,
	eTemplateSubtypeLicenseMigration = 48,
	eTemplateSubtypeItalianAccountValidation = 49,
	eTemplateSubtypeSmsMessageEmails = 50,
	eTemplateSubtypeVIPStoreElectronicVoucher = 51,
	eTemplateSubtypeRingGameLeaderBoardWinners = 52,
	eTemplateSubtypeGameSecutiryTraMailTemplates = 53,
	eTemplateSubtypeEventsTemplates = 54,
	eTemplateSubtypeRlbDivisionNames = 55,	//PYR-21785
	eTemplateSubtypeRlbCreditInfo = 56,
	eTemplateSubtypeTournamentSpecialEmail = 57, //PYR-23166
	eTemplateSubtypeWebCashierEmail = 58, //PYR-23453
	eTemplateSubtypeStarCodeSuccessPopUp = 59,
	eTemplateSubtypeFacebook = 60, //PYR-23923
	eTemplateSubtypeStoreItemInfo = 61, // PYR-24493
	eTemplateSubtypeVipStoreCategory = 62, // PYR-24493
	eTemplateSubtypeVipStoreSubCategory = 63, // PYR-24493
	eTemplateSubtypeCesarAutoReply = 64, // PYR-26002
	eTemplateSubtypeTemporaryBlockLogin = 65, //PYR-24812
	eTemplateSubtypeMobilePushNotification = 66, //PYR-24906
	eTemplateSubtypeOrderCancelReason = 67, // PYR-25209
	eTemplateSubtypeMegaMilestoneStakeShare = 68, // PYR-25811
	eTemplateSubtypeCesarActionTemplates = 69, // PYR-26002
	eTemplateSubtypePopupConfirmMsg = 70, // PYR-26155
	eTemplateSubtypeBroadcastMessage = 71, //PYR-18847
	eTemplateSubtypeSecurityCDMSComments = 72, //PYR-25638
	eTemplateSubtypeVIPStoreEmailsShippingConfirmation = 73, //PYR-26304
	eTemplateSubtypeContactSupportFormHelpContent = 74, // PYR-21440
	eTemplateSubtypeNJBlacklist = 75, // PYR-27257
	eTemplateSubtypeSecurityCashoutApprove = 76,//PYR-27726
	eTemplateSubtypeCesarSatisfactionSurvey = 77, // PYR-30372
	eTemplateSubtypeMobilePushNotificationWithSpell = 78, // PYR-30612
	eTemplateSubtypeGameIntegrityComplainantRefund = 79, //PYR-28632
	eTemplateSubtypeGameIntegrityNonComplainantRefund = 80, //PYR-28632
	eTemplateSubtypeSngrRefundConfirmation = 81, //PYR-28632
	eTemplateSubtypeSecurityTssTemplate = 82, // PYR-26684
	eTemplateSubtypePokerDuelTemplate_obsolete = 83, // PYR-36175 // PYR-53789 - obsolete (cannot delete, since it's stored in DB)
	eTemplateSubtypeInstantBonus = 84, // PYR-31836
	eTemplateSubtypeSportsBook = 85, // PYR-33914
	eTemplateSubtypePortugueseBlacklist = 86, // PYR-40234
	eTemplateSubtypeSecurityPIN = 87, // PYR-40979
	eTemplateSubtypeFullTiltMigration = 88, // PYR-41364
	eTemplateSubtypeDealWinners = 89, // PYR-40427
	eTemplateSubtypeSngrPreProcessConfirmation = 90, // PYR-43728
	eTemplateSubtypeNewAccountReopen = 91, // PYR-45788
	eTemplateSubtypeFreeBetVIPStore = 92, // PYR-41444
	eTemplateSubtypeBonusGameTickets = 93, // PYR-45715
	eTemplateSubtypeWcsfAlertMessage = 94, // PYR-63486
	eTemplateSubtypeSecurityGeolocationAlerts = 95, // PYR-77347
	eTemplateSubtypeResponsibleGaming = 96, // PYR-80956
	eTemplateSubtypeGiftCodes = 97, // PYR-111592
// WHEN ADD NEW TYPE CHECK IF IT CAN USE SECTION CODES (1000-1100) FOR OTHER PURPOSES THAN TO SAVE ATTACHMENT.
// IF IT IS THEN MODIFY TemplateDbManager::isNotAllowToHaveAttachment BY RETURNING TRUE FOR THIS TYPE.
};

enum eTemplEmailSectionCode
{
	eTemplEmailSectionCodeTextBody = 0,
	eTemplEmailSectionCodeSubject = 1,
	eTemplEmailSectionCodeFrom = 2,
	eTemplEmailSectionCodeHtmlBody = 3,
	eTemplEmailSectionCodeURL = 4,
	// PYR-30777 attachments are stored as section code even number between 1000 and 1098,
	// file names of attachments are stored as section code odd number between 1001 and 1099
	eTemplEmailSectionCodeAttachment1 = 1000, // DO NOT CHANGE!!!
	//eTemplEmailSectionCodeAttachmentFileName1 = 1001,
	eTemplEmailSectionCodeAttachmentLast = 1100, // first section code that is not attachment. DO NOT CHANGE!!!
};

enum eTableMessageSectionCode
{
	eTableMessageSectionCodeMessage = 0,
	eTableMessageSectionCodeToolTip = 1,
	eTableMessageSectionCodeURL = 2
};

enum eLobbyImageSectionCode
{
	eLobbyImageSectionCodeTitle = 0,
	eLobbyImageSectionCodeText = 1,
	eLobbyImageSectionCodeURL = 2
};

enum ePaysystemMessageSectionCode
{
	// #8856
	ePaysystemMessageSectionCodeOpen = 0,
	ePaysystemMessageSectionCodeClose = 1,
	// #10867
	ePaysystemMessageSectionCodeFirst = 2,
	// #14359
	ePaysystemMessageSectionCodeDadDeposited = 3, // successfully used in the past
	ePaysystemMessageSectionCodeDadNeverUsed = 4,
};

enum ePromoAvatarSectionCode //#23045
{
	ePromoAvatarSectionCodeClientOption = 0,
	ePromoAvatarSectionCodeClientTitle = 1,
	ePromoAvatarSectionCodeDialogText = 2,

	ePromoAvatarSectionCodeLast	// always the last one
};

enum ePIPEItemSectionCode
{
	eShortText = 0,
	eLongText = 1,
};

enum eSpecialSectionCode
{
	eSpecialSectionCodeCaption = 0,
	eSpecialSectionCodeBanner = 10000, // MUST be less than eSpecialSectionCodeZip
	eSpecialSectionCodeZip = 1000000,
};

enum eTabPubCodeRange
{
	eTabPubCodeMin = 1,
	eTabPubCodeMax = 128,
};

// PYR-24493
enum eStoreItemSectionCode
{
	eItemName = 0,
	eItemDescription = 1,
};

enum eMobileGenNewsSectionCode	// PYR-24582
{
	eMobileGenNewsPhone	= 0,
	eMobileGenNewsTabletHome = 1,
	eMobileGenNewsTabletOther = 2,
};

enum eBonusI18nSectionCode // PYR-24241
{
	eBonusI18nName = 0,
	eBonusI18nDescription = 1,
	eBonusI18nUrl = 2,

	// Insert before this line. The values must be sequential!!!
	eMaxBonusStrings
};

enum eInstantBonusI18nSectionCode // PYR-34104
{
	eInstantBonusI18nShort = 0,
	eInstantBonusI18nLong = 1,
	eInstantBonusI18nGameNamesShort = 2, // PYR-38640
	eInstantBonusI18nGameNamesLong = 3, // PYR-38640

	// Insert before this line. The values must be sequential!!!
	eMaxInstantBonusStrings
};

enum eImageGallerySectionCode // PYR-26568
{
	eImageGalleryImageDescription = 0,
	eImageGalleryCategory = 1,
};

// PYR-33574
enum eSAGPromoPayoutImageSectionCode
{
	eSAGPromoPayoutImageSectionCode_Default = 0,
	eSAGPromoPayoutImageSectionCode_PayoutTicket = 1,
};

// PYR-33574
enum eSAGPromoImageSectionCode
{
	eSAGPromoImageSectionCode_AndroindSagTikcet = 0,
	eSAGPromoImageSectionCode_AndroidSagCheckmark = 1,
	eSAGPromoImageSectionCode_AndroidSagLobby = 2,
	eSAGPromoImageSectionCode_AndroidSagLogo = 3,
	eSAGPromoImageSectionCode_AndroidSagAmbassador = 4,
	eSAGPromoImageSectionCode_AndroidAddSagLogo = 5,
	eSAGPromoImageSectionCode_AndroidSagAddTicket = 6,
	eSAGPromoImageSectionCode_AndroidSagAddCheckmark = 7,
	eSAGPromoImageSectionCode_IosSagTicketiPad = 8,
	eSAGPromoImageSectionCode_IosSagCheckmarkiPad = 9,
	eSAGPromoImageSectionCode_IosSagLobbyiPad = 10,
	eSAGPromoImageSectionCode_IosSagLogoiPad = 11,
	eSAGPromoImageSectionCode_IosSagAmbassadoriPad = 12,
	eSAGPromoImageSectionCode_IosSagTicket = 13,
	eSAGPromoImageSectionCode_IosSagCheckmark = 14,
	eSAGPromoImageSectionCode_IosSagLobby = 15,
	eSAGPromoImageSectionCode_IosAddSagLogoiPad = 16,
	eSAGPromoImageSectionCode_IosSagAddTicketiPad = 17,
	eSAGPromoImageSectionCode_IosSagAddTicket5iPad = 18,
	eSAGPromoImageSectionCode_IosSagAddCheckmarkiPad = 19,
	eSAGPromoImageSectionCode_IosSagAddCheckmark5iPad = 20,
	eSAGPromoImageSectionCode_IosAddSagLogo = 21,
	eSAGPromoImageSectionCode_IosSagAddTicket = 22,
	eSAGPromoImageSectionCode_IosSagAddTicket5 = 23,
	eSAGPromoImageSectionCode_IosSagAddCheckmark = 24,
	eSAGPromoImageSectionCode_IosSagAddCheckmark5 = 25,
	eSAGPromoImageSectionCode_SagPromoBannerPs6 = 26,
	eSAGPromoImageSectionCode_SagPromoBanner = 27,
	eSAGPromoImageSectionCode_SpingoSelectItemRadioButton = 28,
	eSAGPromoImageSectionCode_SpingoViewBackground = 29,
	eSAGPromoImageSectionCode_SpingoViewLogo = 30,
	eSAGPromoImageSectionCode_SpingoTicketItemBgr = 31,
	eSAGPromoImageSectionCode_SpingoObserveItemBgr = 32,
};


// PYR-58846
enum eMorphLobbySectionCode
{
	eMorphLobbySectionCode_tabName = 0,
	eMorphLobbySectionCode_desktopLobbyMhtml = 1,
	eMorphLobbySectionCode_desktopSidecarMhtml = 2,
	eMorphLobbySectionCode_mobileNVP = 3,
	eMorphLobbySectionCode_mobileLogo = 5, // intentionally skip 4 so admin doesn't read images when reading html
};

enum eChestTemplateSectionCode
{
	eChestTemplateSectionCode_Name = 0,
	eChestTemplateSectionCode_Description = 1,
};

// PYR-78942
enum eCustomLobbySectionCode
{
	eCustomLobbySectionCode_tabName = 0,
	eCustomLobbySectionCode_lobbyMhtml = 1,
	eCustomLobbySectionCode_mobileImage = 5, // intentionally skips lower numbers (admins loads first 0-4 sections codes when loading html)
};

// PYR-78942
enum eCustomLobbyWidgetSectionCode
{
	eCustomLobbyWidgetSectionCode_ticketMhtml = 0,
};


// PYR-73483
enum eCustomClientImagesSectionCode
{
	eCustomClientImageSectionCode_tournLobbyLogo = 1,
	eCustomClientImageSectionCode_tournLobbyHeader = 2, 
	eCustomClientImageSectionCode_tournLobbyMiniLogo = 3,
	eCustomClientImageSectionCode_tournBuyInLogo = 4,		// PYR-99347
	eCustomClientImageSectionCode_tournKOEliminated = 5,	// PYR-99347
	eCustomClientImageSectionCode_tournEndOfTourn = 6,		// PYR-99347
};


// PYR-111857
enum eBrandedLobbyFilterSectionCode
{
	eBrandedLobbyFilterSectionCode_displayName = 0,
	eBrandedLobbyFilterSectionCode_gameMask = 1,
	eBrandedLobbyFilterSectionCode_isPM = 2,
	eBrandedLobbyFilterSectionCode_tableSizeMask = 3,
	eBrandedLobbyFilterSectionCode_gameCategoryMask = 4,
	eBrandedLobbyFilterSectionCode_capMask = 5,
	eBrandedLobbyFilterSectionCode_anteMask = 6,
	eBrandedLobbyFilterSectionCode_minDeepRatio = 7,
	eBrandedLobbyFilterSectionCode_maxDeepRatio = 8,
	eBrandedLobbyFilterSectionCode_minBuyIn = 9,
	eBrandedLobbyFilterSectionCode_maxBuyIn = 10,
	eBrandedLobbyFilterSectionCode_speedMask = 11,
	eBrandedLobbyFilterSectionCode_tournVariantMask = 12,
	eBrandedLobbyFilterSectionCode_tournStateMask = 13,
	eBrandedLobbyFilterSectionCode_startingWithin = 14,
	eBrandedLobbyFilterSectionCode_tournGroups = 15,
	eBrandedLobbyFilterSectionCode_lateRegCutoff = 16,
	eBrandedLobbyFilterSectionCode_genderRestrictMask = 17,
	eBrandedLobbyFilterSectionCode_displayTypeMask = 18,
	eBrandedLobbyFilterSectionCode_excludeVariantMask = 19,
	eBrandedLobbyFilterSectionCode_tableNumMask = 20,
	eBrandedLobbyFilterSectionCode_filterType = 21,
	eBrandedLobbyFilterSectionCode_currencyMask = 22,
	eBrandedLobbyFilterSectionCode_tableVariantMask = 23,
	eBrandedLobbyFilterSectionCode_minStake = 24, // PYR-121178
	eBrandedLobbyFilterSectionCode_maxStake = 25, // PYR-121178

	// Insert before this line and update eBrandedLobbyFilterSectionCode_last
	eBrandedLobbyFilterSectionCode_last = eBrandedLobbyFilterSectionCode_maxStake
};

enum eHubFeedSectionCode
{
	eHubFeedSectionCode_FeedType = 1,					// eFeedType
	eHubFeedSectionCode_UserVisibility = 2,				// eFeedUserVisibilityType
	eHubFeedSectionCode_InternalRef = 3,				// String
	eHubFeedSectionCode_PostIcon = 4,					// eFeedIconType
	eHubFeedSectionCode_ButtonLabel = 5,				// string, multiple locales
	eHubFeedSectionCode_ButtonURL = 6,					// string
	eHubFeedSectionCode_GameType = 7,					// GameCategoryTypes
	eHubFeedSectionCode_MinBuyInMultiplier = 8,			// int
	eHubFeedSectionCode_MinPlayers = 9,					// int
	eHubFeedSectionCode_MaxRank = 10,					// int
	eHubFeedSectionCode_PrizeIsTicket = 11,				// 0/1
	eHubFeedSectionCode_BrandMask = 12,					// int brands.h
	eHubFeedSectionCode_AppTypeMask = 13,				// int clientplatforms.h
	eHubFeedSectionCode_SiteMask = 14,					// string PBitmask.toDbString
	eHubFeedSectionCode_PersonaValues = 15,				// int, see PYR-117964
	eHubFeedSectionCode_PostTitle = 16,					// string, multiple locales
	eHubFeedSectionCode_PostBody = 17,					// string, multiple locales
	eHubFeedSectionCode_StartDateTime = 18,				// int64, secs since 1 January 1970 00:00:00
	eHubFeedSectionCode_EndDateTime = 19,				// int64, secs since 1 January 1970 00:00:00
	eHubFeedSectionCode_ScheduleDailyDays = 20,			// int
	eHubFeedSectionCode_ScheduleWeeklyWeeks = 21,		// int
	eHubFeedSectionCode_ScheduleWeeklyDay = 22,			// int (0 - Sunday, 7 - Saturday)
	eHubFeedSectionCode_ScheduleRangeOccurances = 23,	// int
	eHubFeedSectionCode_Button2Label = 24,				// string, multiple locales
	eHubFeedSectionCode_Button2URL = 25,				// string
};


#define STOREITEM_TEMPLATENAME_PREFIX	"STOREITEM_"	// PYR-24493 
#define STORECAT_TEMPLATENAME_PREFIX	"STORECAT_"	// PYR-24493
#define STORESUBCAT_TEMPLATENAME_PREFIX	"STORESUBCAT_"	// PYR-24493
//
// VARCHAR fields
//

// table TEMPLATES
#define TEMPLATES_NAME_LEN			(100 +1)
#define TEMPLATES_TEMPLATECODE_LEN	(100 +1)
#define TEMPLATES_DESCRIPTION_LEN	(500 +1)

// table TEMPLATEAUDIT
#define TEMPLATEAUDIT_ACTIONINFO_LEN	(100 + 1)

// table CONTENT
#define CONTENT_CRITERIA_LEN		(3600 + 1)

// table CONTENTDATA
#define CONTENTDATA_DATA_LEN		(3600 + 1)

// table WELCOMEUSERS
#define WELCOMEUSERS_FROMADDR_LEN	(360 + 1)
#define WELCOMEUSERS_PROMONAME_LEN	(360 + 1)

// table MAILCODEUSERS
#define MAILCODEUSERS_MAILCODE_LEN	(100 + 1)

// table REC2CODES
#define REC2CODES_EXTRAINFO_LEN		(360 + 1)

// table SMSMESSAGES
#define SMSMESSAGES_TEXT_LEN        (160 + 1)
#define SMSMESSAGES_ERRMESSAGE_LEN  (256 + 1)

// table INTERNALERRORMAP (PYR-19814)
#define ERRORCODE_LEN				(100 + 1)

// PYR-25209 tables VIPSTORECANCELREASON & VIPSTORECANCELSUBREASON
#define REASON_LEN					(100 + 1)
#define SUBREASON_LEN				(100 + 1)

// PYR-27542 table MASSEMAILREPORTSBYERROR
#define ERROR_LEN					(100 +1)

//
// Generator64 objects
//

#define TEMPLATES_TABLE_NAME		"Templates"
#define TEMPLATEAUDIT_TABLE_NAME	"TemplateAudit"
#define CONTENT_TABLE_NAME			"Content"
#define REC2CODESAUDIT_TABLE_NAME	"Rec2Codes"
#define SMSMESSAGES_TABLE_NAME      "SmsMessages"
// PYR-25209
#define VIPSTORECANCELREASON_TABLE_NAME	"VIPSTORECANCELREASON"
#define VIPSTORECANCELSUBREASON_TABLE_NAME	"VIPSTORECANCELSUBREASON"
// PYR-58846
#define MORPH_LOBBY_TEMPLATES		"MorphLobbyTemplates"
//PYR-79842
#define CUSTOM_LOBBY_TEMPLATES		"CustomLobbyTemplates"
//
// Template Server protocol messages
//

#define TS_REPLY_DELTA	1000

//
// Admin messages
//

enum eSaveTemplateMask
{
	eSaveTemplateMaskName =		1 << 0,
	eSaveTemplateMaskDescr =	1 << 1,
	eSaveTemplateMaskStatus =	1 << 2,
	eSaveTemplateMaskVersion =	1 << 3,
	eSaveTemplateMaskOrig =		1 << 4,
	eSaveTemplateMaskSubType =	1 << 5, // PYR-24241
};

#define TS_Q_ADMIN_SAVE_TEMPLATE		100 // 4444sss4 - reqId, saveTemplateMask, templateId, type, name, templateCode, description, status
#define TS_A_ADMIN_SAVE_TEMPLATE		(TS_Q_ADMIN_SAVE_TEMPLATE + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|templateId]

#define TS_Q_ADMIN_DELETE_TEMPLATE		101 // 44 - reqId, temlateId
#define TS_A_ADMIN_DELETE_TEMPLATE		(TS_Q_ADMIN_DELETE_TEMPLATE + TS_REPLY_DELTA) // 46[s] reqId, errCode[errDescr]

#define TS_Q_ADMIN_SAVE_CONTENT_DATA	102 // 44444<4s> - reqId, templateId, contentId, locale, numSections<sectionCode, text>
#define TS_A_ADMIN_SAVE_CONTENT_DATA	(TS_Q_ADMIN_SAVE_CONTENT_DATA + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|contentId]

#define TS_Q_ADMIN_CHANGE_CONTENT_STATUS	103 // 44444 - reqId, templateId, contentId, locale, status
#define TS_A_ADMIN_CHANGE_CONTENT_STATUS	(TS_Q_ADMIN_CHANGE_CONTENT_STATUS + TS_REPLY_DELTA) // 46[s] reqId, errCode[errDescr]

#define TS_Q_ADMIN_DELETE_CONTENT		104 // 4444 - reqId, templateId, contentId, locale
#define TS_A_ADMIN_DELETE_CONTENT		(TS_Q_ADMIN_DELETE_CONTENT + TS_REPLY_DELTA) // 46[s] reqId, errCode[errDescr]

#define TS_Q_ADMIN_CLONE_TEMPLATE		105 // 44sss - reqId, templateId, newName, newTemplateCode, newDescription
#define TS_A_ADMIN_CLONE_TEMPLATE		(TS_Q_ADMIN_CLONE_TEMPLATE + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|newTemplateId]

#define TS_Q_ADMIN_SAVE_CONTENT_DATA_EX	106 // 444stt44<4s> - reqId, templateId, contentId, criteria, from, until, locale, numSections<sectionCode, text>
#define TS_A_ADMIN_SAVE_CONTENT_DATA_EX	(TS_Q_ADMIN_SAVE_CONTENT_DATA_EX + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|contentId]

#define TS_Q_ADMIN_UPDATE_CONTENT		107 // 444stt - reqId, templateId, contentId, criteria, from, until
#define TS_A_ADMIN_UPDATE_CONTENT		(TS_Q_ADMIN_UPDATE_CONTENT + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|contentId]

#define TS_Q_ADMIN_REREAD_INI			108 //4 - reqId
#define TS_A_ADMIN_REREAD_INI			(TS_Q_ADMIN_REREAD_INI + TS_REPLY_DELTA) // 46[s] reqId, errCode[errDescr]

#define TS_Q_ADMIN_SAVE_TEMPLATE_EX		109 // 44444sss4 - reqId, saveTemplateMask, templateId, type, subType, name, templateCode, description, status
#define TS_A_ADMIN_SAVE_TEMPLATE_EX		(TS_Q_ADMIN_SAVE_TEMPLATE_EX + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|templateId]

#define TS_Q_ADMIN_SET_GATEWAY_CODE		110 //4ssss4s4 - reqId,gateway,paySystem,transType,gatewayCode,templateId,extraInfo,recType
#define TS_A_ADMIN_SET_GATEWAY_CODE		(TS_Q_ADMIN_SET_GATEWAY_CODE + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_DELETE_GATEWAY_CODE	111 //4ssss4 - reqId,gateway,paySystem,transType,gatewayCode,recType
#define TS_A_ADMIN_DELETE_GATEWAY_CODE	(TS_Q_ADMIN_DELETE_GATEWAY_CODE + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_DELETE_CONTENT_DATA	112 //444 - reqId,contentId,sectionCode
#define TS_A_ADMIN_DELETE_CONTENT_DATA	(TS_Q_ADMIN_DELETE_CONTENT_DATA + TS_REPLY_DELTA) //42[s|4] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_DELETE_CONTENT_DATA_BY_RANGE		113 //4444 - reqId,contentId,sectionCodeMin,sectionCodeMax
#define TS_A_ADMIN_DELETE_CONTENT_DATA_BY_RANGE		(TS_Q_ADMIN_DELETE_CONTENT_DATA_BY_RANGE + TS_REPLY_DELTA) //42[s|4] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_REREAD_PMW_EMAILS		114 //4 - reqId
#define TS_A_ADMIN_REREAD_PMW_EMAILS		(TS_Q_ADMIN_REREAD_PMW_EMAILS + TS_REPLY_DELTA) // 46[s] reqId, errCode[errDescr]

// PYR-24241
#define TS_Q_ADMIN_REREAD_BONUS_STRINGS			115 //4 - reqId
#define TS_A_ADMIN_REREAD_BONUS_STRINGS			(TS_Q_ADMIN_REREAD_BONUS_STRINGS + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_REC_CHANGE_TO_ANY_AND_DELETE_OTHERS		116 //4sss4s4 - reqId,gateway,transType,gatewayCode,templateId,extraInfo,recType
#define TS_A_ADMIN_REC_CHANGE_TO_ANY_AND_DELETE_OTHERS		(TS_Q_ADMIN_REC_CHANGE_TO_ANY_AND_DELETE_OTHERS + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_FORMAT_VERIFICATION_FORM		117 //4s44<s> - reqId,userId,formId,numParam,<param> //param is cardNum, bankAcc, etc
#define TS_A_ADMIN_FORMAT_VERIFICATION_FORM		(TS_Q_ADMIN_FORMAT_VERIFICATION_FORM + TS_REPLY_DELTA) //42[s|<pdf>] - reqId,errCode,[errDescr|<pdf>]

// PYR-20098
#define TS_Q_ADMIN_REREAD_INIDATA				118 //4 - reqId
#define TS_A_ADMIN_REREAD_INIDATA				(TS_Q_ADMIN_REREAD_INIDATA + TS_REPLY_DELTA) // 46[s] reqId, errCode[errDescr]

// PYR-19814
#define TS_Q_ADMIN_GET_ALL_INTERNAL_ERRORS_TEMPLATES	119 // 4 - reqId
#define TS_A_ADMIN_GET_ALL_INTERNAL_ERRORS_TEMPLATES	(TS_Q_ADMIN_GET_ALL_INTERNAL_ERRORS_TEMPLATES + TS_REPLY_DELTA) // 42[s|4<s4s>] - reqId,errCode,[errDescr|numRecs<errCode,templateId,templateName>]

#define TS_Q_ADMIN_ADD_INTERNAL_ERROR			120 // 4s4 - reqId, errCode, templateId
#define TS_A_ADMIN_ADD_INTERNAL_ERROR			(TS_Q_ADMIN_ADD_INTERNAL_ERROR + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_UPDATE_INTERNAL_ERROR		121 // 4s4 - reqId, errCode, templateId
#define TS_A_ADMIN_UPDATE_INTERNAL_ERROR		(TS_Q_ADMIN_UPDATE_INTERNAL_ERROR + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_DELETE_INTERNAL_ERROR		122 // 4s - reqId, errCode
#define TS_A_ADMIN_DELETE_INTERNAL_ERROR		(TS_Q_ADMIN_DELETE_INTERNAL_ERROR + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_REREAD_INTERNAL_ERRORS		123 // 4 - reqId
#define TS_A_ADMIN_REREAD_INTERNAL_ERRORS		(TS_Q_ADMIN_REREAD_INTERNAL_ERRORS + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

#define TS_Q_ADMIN_BULK_UPDATE_TEMPLATE_CRITERIA		124 // 44[4s4s...] - reqId, number of templates, [templateId, criteria,...]
#define TS_A_ADMIN_BULK_UPDATE_TEMPLATE_CRITERIA		(TS_Q_ADMIN_BULK_UPDATE_TEMPLATE_CRITERIA + TS_REPLY_DELTA) // 44[46s46s...] reqId, number of errors, [templateId, errCode, errDescr,...]

#define TS_Q_ADMIN_EDIT_COPY_TEMPLATE			125 // 44sss - reqId, templateId, newName, newTemplateCode, newDescription 
#define TS_A_ADMIN_EDIT_COPY_TEMPLATE			(TS_Q_ADMIN_EDIT_COPY_TEMPLATE + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|newTemplateId]

// PYR-25209
#define TS_Q_ADMIN_ADD_VIPSTORE_CANCEL_REASON	126 // 4s - reqId,reason
#define TS_A_ADMIN_ADD_VIPSTORE_CANCEL_REASON	(TS_Q_ADMIN_ADD_VIPSTORE_CANCEL_REASON + TS_REPLY_DELTA) // 42[s] - reqId,errCode[,errDescr]

#define TS_Q_ADMIN_UPDATE_VIPSTORE_CANCEL_REASON	127 // 44s - reqId,reasonCode,reason
#define TS_A_ADMIN_UPDATE_VIPSTORE_CANCEL_REASON	(TS_Q_ADMIN_UPDATE_VIPSTORE_CANCEL_REASON + TS_REPLY_DELTA) // 42[s] - reqId,errCode[,errDescr]

#define TS_Q_ADMIN_SET_ARCHIVED_VIPSTORE_CANCEL_REASON	128 // 444 - reqId,reasonCode,archived (0 - restore, 1 - delete)
#define TS_A_ADMIN_SET_ARCHIVED_VIPSTORE_CANCEL_REASON	(TS_Q_ADMIN_SET_ARCHIVED_VIPSTORE_CANCEL_REASON + TS_REPLY_DELTA) // 42[s] - reqId,errCode[,errDescr]

#define TS_Q_ADMIN_ADD_VIPSTORE_CANCEL_SUBREASON	129 // 44s4444b - reqId,reasonCode,subReason,stateMask,returnToStockMask,templateIdPlayer,tempateIdCsr,creditFpp
#define TS_A_ADMIN_ADD_VIPSTORE_CANCEL_SUBREASON	(TS_Q_ADMIN_ADD_VIPSTORE_CANCEL_SUBREASON + TS_REPLY_DELTA) // 42[s] - reqId,errCode[,errDescr]

#define TS_Q_ADMIN_UPDATE_VIPSTORE_CANCEL_SUBREASON	130 // 444s4444b - reqId,subReasonCode,reasonCode,subReason,stateMask,returnToStockMask,templateIdPlayer,tempateIdCsr,creditFpp
#define TS_A_ADMIN_UPDATE_VIPSTORE_CANCEL_SUBREASON	(TS_Q_ADMIN_UPDATE_VIPSTORE_CANCEL_SUBREASON + TS_REPLY_DELTA) // 42[s] - reqId,errCode[,errDescr]

#define TS_Q_ADMIN_SET_ARCHIVED_VIPSTORE_CANCEL_SUBREASON	131 // 444 - reqId,subReasonCode,archived (0 - restore, 1 - delete)
#define TS_A_ADMIN_SET_ARCHIVED_VIPSTORE_CANCEL_SUBREASON	(TS_Q_ADMIN_SET_ARCHIVED_VIPSTORE_CANCEL_SUBREASON + TS_REPLY_DELTA) // 42[s] - reqId,errCode[,errDescr]

#define TS_Q_ADMIN_FORMAT_BROADCAST_MESSAGE		132 //4444<s> - reqId,templateId,siteId,numParams,<params>
#define TS_A_ADMIN_FORMAT_BROADCAST_MESSAGE		(TS_Q_ADMIN_FORMAT_BROADCAST_MESSAGE + TS_REPLY_DELTA) //42[s|S] - reqId,errCode,[errDescr|i18nPString]

// PYR-21440
#define TS_Q_ADMIN_GET_CSF_TEMPLATES			133 // 4 - reqId
#define TS_A_ADMIN_GET_CSF_TEMPLATES			(TS_Q_ADMIN_GET_CSF_TEMPLATES + TS_REPLY_DELTA) //42[s|<4sssS<S><4>>] - reqId,errCode,[errDescr|<templateId,name,descr,criteria,i18nPString-category,<i18nPString-subcategory><sectionCodes>>]

// PYR-26568
#define TS_Q_ADMIN_GET_IMAGE_GALLERY_TEMPLATES	134 // 4 - reqId
#define TS_A_ADMIN_GET_IMAGE_GALLERY_TEMPLATES	(TS_Q_ADMIN_GET_IMAGE_GALLERY_TEMPLATES + TS_REPLY_DELTA)// 46[4<4<I18nPString>>|s] reqId,errCode,[numRecords,<templateId,<category|subcartegory|imageDescription>>|errDescr]

// PYR-18741
#define TS_Q_ADMIN_GET_MASSEMAIL_STATUSES		135 // 4 - reqId
#define TS_A_ADMIN_GET_MASSEMAIL_STATUSES		(TS_Q_ADMIN_GET_MASSEMAIL_STATUSES + TS_REPLY_DELTA) // 46[s|4<bs44bss4btt>] - reqId,errCode,[errDescr|numRecords<busy,adminId,templateId,sendMask,reportOnly,trackCode,mailCode,numUsers,massSms,started,completed>]

// PYR-29926
#define TS_Q_ICELOBBY_GET_TEMPLATES				136 // 4b - reqId, getGenericTextData
#define TS_A_ICELOBBY_GET_TEMPLATES				(TS_Q_ICELOBBY_GET_TEMPLATES + TS_REPLY_DELTA)// 46[4<o>|s] reqId,errCode,[numRecords,<IceTaskTemplateData>|errDescr]

// PYR-30888
#define TS_Q_ADMIN_GET_CSF_TEMPLATES_BY_LOCALE	137 // 44 - reqId,locale
#define TS_A_ADMIN_GET_CSF_TEMPLATES_BY_LOCALE	(TS_Q_ADMIN_GET_CSF_TEMPLATES_BY_LOCALE + TS_REPLY_DELTA) //42[s|<4ssss<s><4>>] - reqId,errCode,[errDescr|<templateId,name,descr,criteria,category,<subcategory><sectionCodes>>]

// PYR-33799
#define TS_Q_ADMIN_GET_TEMPLATE_CONTENTS			138 // 44 - reqId,templateType
#define TS_A_ADMIN_GET_TEMPLATE_CONTENTS			(TS_Q_ADMIN_GET_TEMPLATE_CONTENTS + TS_REPLY_DELTA) //42[s|4<4sss444s4<4S>>] - reqId,errCode,[errDescr|arraysize<templateId,name,templateCode,descr,version,subType,origTemplateId,criteria,arraySize<sectionCode,subCategory>>]

// PYR-34104
#define TS_Q_ADMIN_REREAD_ROLLBONUS_STRINGS			139 //4 - reqId
#define TS_A_ADMIN_REREAD_ROLLBONUS_STRINGS			(TS_Q_ADMIN_REREAD_ROLLBONUS_STRINGS + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

// PYR-42273
#define TS_Q_ADMIN_REREAD_MGLOBBY_STRINGS			140 //4 - reqId
#define TS_A_ADMIN_REREAD_MGLOBBY_STRINGS			(TS_Q_ADMIN_REREAD_MGLOBBY_STRINGS + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

// PYR-46163
#define TS_Q_ADMIN_REREAD_GAMETICKET_STRINGS		141 //4 - reqId
#define TS_A_ADMIN_REREAD_GAMETICKET_STRINGS		(TS_Q_ADMIN_REREAD_GAMETICKET_STRINGS + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

// PYR-47353
#define TS_Q_ADMIN_CLONE_TEMPLATE_CONTENT			142 // 44s - reqId, contentId, criteria
#define TS_A_ADMIN_CLONE_TEMPLATE_CONTENT			(TS_Q_ADMIN_CLONE_TEMPLATE_CONTENT + TS_REPLY_DELTA) // 46[s|4] reqId, errCode[errDescr|newContentId]

// PYR-50671
#define TS_Q_ADMIN_Q_GET_TS_CONFIGURATION			143 // 4s - reqId, sectionName
#define TS_A_ADMIN_Q_GET_TS_CONFIGURATION			(TS_Q_ADMIN_Q_GET_TS_CONFIGURATION + TS_REPLY_DELTA) // 46[s|s] reqId, errCode[errDescr|sectionContent]

// PYR-62106
#define TS_Q_ADMIN_REREAD_CHEST_TEMPLATES			144 //4 - reqId
#define TS_A_ADMIN_REREAD_CHEST_TEMPLATES			(TS_Q_ADMIN_REREAD_CHEST_TEMPLATES + TS_REPLY_DELTA) //42[s] - reqId,errCode,[errDescr]

// PYR-63486
#define TS_Q_ADMIN_GET_ALERT_TEMPLATES				145 // 4 - reqId
#define TS_A_ADMIN_GET_ALERT_TEMPLATES				(TS_Q_ADMIN_GET_ALERT_TEMPLATES + TS_REPLY_DELTA) //42[s|<4sssttS>] - reqId,errCode,[errDescr|<templateId,name,descr,criteria,from,until,i18nPString-alert>]

// PYR-111857
#define TS_Q_ADMIN_SAVE_BRANDED_LOBBIES				146 // 44<M(444sss44<M(4stt4<M(44<M(4s)>)>)>)> - reqId,numTemplates<M(saveTemplateMask,templateId,type,name,templateCode,description,status,numContent<M(contentId,criteria,from,until,numContentDatas<M(sectionCode,numSections<M(locale,text)>)>)>)>
#define TS_A_ADMIN_SAVE_BRANDED_LOBBIES				(TS_Q_ADMIN_SAVE_BRANDED_LOBBIES + TS_REPLY_DELTA) // 46s reqId, errCode,errDescr
//
// Processing messages
//

enum eSendMassEmailMask
{
	eSendMassEmailExactLocaleOnly			= 1 << 0, // Means: "Don’t send if local not available". If not set means: "Use system fall back if local not available"
	eSendMassEmailOmitNoEmailVerifiedFlag	= 1 << 1,
	eSendMassEmailOmitEmailBouncedFlag		= 1 << 2,
	eSendMassEmailOmitNoMailListPrivilege	= 1 << 3,
	eSendMassEmailExcludeClosedAccounts		= 1 << 4,
	eSendMassEmailExcludeSelfExcludedAccounts = 1 << 5,
	eSendMassEmailAddHabbeas = 1 << 6,
	eSendMassEmailRemoveDuplicates = 1 << 7,
	eSendMassEmailForceLocale = 1 << 8,	// #9885  - forced locale must be provided with TS_Q_MASS_EMAIL message
	eSendMassEmailForceTextOnly = 1 << 9,// #9885
	eSendMassEmailExcludeLicenses = 1 << 10, // PYR-19082
	eSendMassEmailExcludeCountries = 1 << 11, // PYR-19082
	eSendMassEmailExcludeRealMoney = 1 << 12, // PYR-19082
	eSendMassEmailExcludePlayMoney = 1 << 13, // PYR-19082
	eSendMassSmsOmitNoMobileVerifiedFlag = 1 << 14, // PYR-23088
	eSendMassSmsOmitNoMarketingFlag = 1 << 15, // PYR-23088
	eSendMassEmailTrackOpenRate = 1 << 16,// PYR-16105
	eSendMassEmailTrackLinkClickThrough = 1 << 17,// PYR-16105
	eSendMassEmailSmsAllLocales = 1 << 18, // PYR-27201
	eSendMassEmailIncludeRMBlockedCountries = 1 << 19, //PYR-28801
	eSendMassEmailTesting = 1 << 20, // PYR-28845
	eSendMassEmailValidateUserParamsLocale = 1 << 21, // PYR-30077
	eSendMassEmailByUserIntId = 1 << 22, // PYR-39001
	eSendMassEmailByUserWebId = 1 << 23, // PYR-39001
	eSendMassEmailToExecution = 1 << 24, // PYR-47458
	eSendMassEmailOmitBannedByRegulator = 1 << 25, // PYR-77875
};

///
#define TS_Q_MASS_EMAIL			300 // 444bsss44<s<s>>44<4>4<s>[ssss] - reqId, templateId, sendMask, reportOnly, emailReport, trackCode, mailCode, numUsers, numParams<userId<param>>, forcedLocale, numLicenses<licenceId>, numCountries<country>, [subj, txtBody, htmlBody, from]
#define TS_A_MASS_EMAIL			(TS_Q_MASS_EMAIL + TS_REPLY_DELTA) // 46[s|...] reqId, errCode[errDescr|REPORT]
// REPORT: bTs44<444>4<ss>4<s4>4<s>
// reportOnly, dateTime, templateName, totalUsers, numLocales<locale, numUsersSubmitted, numUsersEmailed>, numUsersNotSent<userId, reason>, numUsersMissingLocale<userId, locale>, numMassEmailServerErrorMessages<errMsg>

#define TS_Q_MASS_EMAILCODE_GZIP		301 //4sv - reqId,mailCode,usersGZip
#define TS_A_MASS_EMAILCODE_GZIP		(TS_Q_MASS_EMAILCODE_GZIP + TS_REPLY_DELTA) //46[s|4] reqId, errCode[errDescr|usersAdded]

#define TS_Q_MASS_EMAILCODE_DELETE		302 //4s - reqId,mailCode
#define TS_A_MASS_EMAILCODE_DELETE		(TS_Q_MASS_EMAILCODE_DELETE + TS_REPLY_DELTA) //46s reqId,errCode,errDescr

#define TS_Q_MASS_EMAILCODE_GET_USERS	303 //4s - reqId,mailCode
#define TS_A_MASS_EMAILCODE_GET_USERS	(TS_Q_MASS_EMAILCODE_GET_USERS + TS_REPLY_DELTA) //46[s|4<s>] reqId,errCode,[errDescr|numUsers<userId>]

#define TS_Q_AUTOMATION_MASS_EMAIL		304 //4s44bsss44<s<s>>4[4] - reqId,adminId,templateId,sendMask,reportOnly,emailReport,trackCode,mailCode,numUsers,numParams<userId<param>>,forcedLocale[,progId]
#define TS_A_AUTOMATION_MASS_EMAIL		(TS_Q_AUTOMATION_MASS_EMAIL + TS_REPLY_DELTA) //46s - reqId,errCode,errDescr

#define TS_Q_MASS_EMAILS_RESOLV			305 //44<s> - reqId,numEmails,<userEmail>
#define TS_A_MASS_EMAILS_RESOLV			(TS_Q_MASS_EMAILS_RESOLV + TS_REPLY_DELTA) //46[s|4<s>4<s>] reqId,errCode,[errDescr|numUsers,<userId>,numEmails,<userEmail>]

#define TS_Q_MASS_EMAILCODE_ADD			306 //4s4[s] - reqId,mailCode,numUsers,<userId>
#define TS_A_MASS_EMAILCODE_ADD			(TS_Q_MASS_EMAILCODE_ADD + TS_REPLY_DELTA) //46[s|4] reqId, errCode[errDescr|usersAdded]

// PYR-19601
#define TS_Q_GET_MASS_EMAILCODES		307 //4s - reqId
#define TS_A_GET_MASS_EMAILCODES		(TS_Q_GET_MASS_EMAILCODES + TS_REPLY_DELTA) //46[s|4<s>] reqId,errCode,[errDescr|numMailCodes<mailCode>]

// PYR-23088
#define TS_Q_MASS_SMS					308 // same as TS_Q_MASS_EMAIL
#define TS_A_MASS_SMS					(TS_Q_MASS_SMS + TS_REPLY_DELTA) // 46[s|...] reqId, errCode[errDescr|REPORT]

// PYR-25781
#define TS_Q_AUTOMATION_PUSH_NOTIFICATION	309	// 444444b4<s>b4 reqId, templateId, platformMask, locale option, forcedlocale, siteIdMask, incRMBlockedCountries, num of users, < user ids >, track, brandMask
#define TS_A_AUTOMATION_PUSH_NOTIFICATION	(TS_Q_AUTOMATION_PUSH_NOTIFICATION + TS_REPLY_DELTA)	// 46[s] reqId, errCode[,errDescr]

// PYR-23089
#define TS_Q_AUTOMATION_MASS_SMS		310 // same as TS_Q_AUTOMATION_MASS_EMAIL
#define TS_A_AUTOMATION_MASS_SMS		(TS_Q_AUTOMATION_MASS_SMS + TS_REPLY_DELTA)

/// lobby
#define TS_Q_LOBBY_WELCOME_EMAIL			400 //4ssss4s44 - reqId,userName,email,from,country,siteId,promoName,locale,brandId
#define TS_A_LOBBY_WELCOME_EMAIL			(TS_Q_LOBBY_WELCOME_EMAIL + TS_REPLY_DELTA) //46s regId,errCode[errDescr|userName]

#define TS_Q_LOBBY_CIT_CONTRACT				402 //4s - reqId,userId
#define TS_A_LOBBY_CIT_CONTRACT				(TS_Q_LOBBY_CIT_CONTRACT + TS_REPLY_DELTA) //46[s|<data>] - regId,errCode[errDescr|contract]

#define TS_Q_LOBBY_EXPLETIVES				403 //44[...] - reqId,numVec,[strVec],numVec2,[strVec2]
#define TS_A_LOBBY_EXPLETIVES				(TS_Q_LOBBY_EXPLETIVES + TS_REPLY_DELTA) //46s - regId,errCode,errDescr

#define TS_Q_MTLOBBY_SPECIAL_TEXT			404 //44[s] - reqId,numTitles,<specialTitle>
#define TS_A_MTLOBBY_SPECIAL_TEXT			(TS_Q_MTLOBBY_SPECIAL_TEXT + TS_REPLY_DELTA) //44<sS> - regId,numTitles,<specialTitle,i18nText>

#define TS_Q_LOBBY_SIGNUP_QUESTIONNAIRE		405 //4s - reqId,country
#define TS_A_LOBBY_SIGNUP_QUESTIONNAIRE		(TS_Q_LOBBY_SIGNUP_QUESTIONNAIRE + TS_REPLY_DELTA) //446[s|4<ii4<i>> - reqId,errCode,numSources<i18nSourceName,i18nPleaseSpecify,numOptions<i18nOptionText>>

#define TS_Q_LOBBY_NEED_FLYERS				406 //4 - reqId
#define TS_A_LOBBY_NEED_FLYERS				(TS_Q_LOBBY_NEED_FLYERS + TS_REPLY_DELTA) //46[s|4<data>] - regId,errCode[errDescr|numFlyers,<data>]

#define TS_Q_LOBBY_GET_FLYER				407 //44 - reqId,templateId
#define TS_A_LOBBY_GET_FLYER				(TS_Q_LOBBY_GET_FLYER + TS_REPLY_DELTA) //46[s|4<data>] - regId,errCode[errDescr|numFlyers,<data>]

#define TS_Q_GET_STARCODE_POPUP				408 // 4ss4 - reqId, [adminId - do not parse it out in the function itself it's parsed out previously], userId,templateId
#define TS_A_GET_STARCODE_POPUP				(TS_Q_GET_STARCODE_POPUP + TS_REPLY_DELTA) //46[s|s] - regId,errCode[errDescr|poup_text]

/// news
#define TS_Q_TARGETNEWS_NEED_DATA			500 //44 - reqId, template type
#define TS_A_TARGETNEWS_NEED_DATA			(TS_Q_TARGETNEWS_NEED_DATA + TS_REPLY_DELTA) //46[s|b4<...>] regId,errCode[errDescr|isLastBatch,num<News>]

#define TS_Q_NEWS_UPDATED					501 //444 reqId,templateId,locale
#define TS_A_NEWS_UPDATED					(TS_Q_NEWS_UPDATED + TS_REPLY_DELTA) //46[s|4<...>] regId,errCode[errDescr|num<News>]

// PYR-117290 obsolete
//#define TS_Q_LOBBYIMAGE_NEED_DATA			502 //4 - reqId
//#define TS_A_LOBBYIMAGE_NEED_DATA			(TS_Q_LOBBYIMAGE_NEED_DATA + TS_REPLY_DELTA) //42[s|4<...>] - regId,errCode,[errDescr|num,<Data>]

// PYR-117290 obsolete
//#define TS_Q_LOBBYIMAGE_UPDATED				503 //44s - reqId,siteId,country
//#define TS_A_LOBBYIMAGE_UPDATED				(TS_Q_LOBBYIMAGE_UPDATED + TS_REPLY_DELTA) //42[s|4<...>] - regId,errCode,[errDescr|num,<Data>]

#define TS_Q_PROMOIMAGE_UPDATED				504 //4 - reqId
#define TS_A_PROMOIMAGE_UPDATED				(TS_Q_PROMOIMAGE_UPDATED + TS_REPLY_DELTA) //42[s|4<s4S>4<M>] - regId,errCode,[errDescr|num,<country,imageId,comment>,num<PromoImageDialog>]

// PYR-118214 obsolete
//#define TS_Q_TABLEFELTGRAPHIC_UPDATED		505 //44 - reqId,siteMask
//#define TS_A_TABLEFELTGRAPHIC_UPDATED		(TS_Q_TABLEFELTGRAPHIC_UPDATED + TS_REPLY_DELTA) //42[s|44<44B>] - regId,errCode,[errDescr|siteMask,num,<siteMask,typeMask,image>]

#define TS_Q_PIPEIMAGES_NEED_DATA			506 //4 - reqId
#define TS_A_PIPEIMAGES_NEED_DATA			(TS_Q_PIPEIMAGES_NEED_DATA + TS_REPLY_DELTA) //42[s|4<s4444b4<s>S>] - regId,errCode,[errDescr|numImages,<name,templateId,contentId,site,flags,incCountries,munCountries,<country>,<imagesB64>>]

#define TS_Q_PIPEIMAGES_UPDATED				507 //4s - reqId,name
#define TS_A_PIPEIMAGES_UPDATED				(TS_Q_PIPEIMAGES_UPDATED + TS_REPLY_DELTA) //42[s|4<s4444b4<s>S>] - regId,errCode,[errDescr|numImages,<name,templateId,contentId,site,flags,incCountries,munCountries,<country>,<imagesB64>>]

#define TS_Q_PIPEITEMS_NEED_DATA			508 //4 - reqId
#define TS_A_PIPEITEMS_NEED_DATA			(TS_Q_PIPEITEMS_NEED_DATA + TS_REPLY_DELTA) //42[s|4<444444ttb4<s>SS>] - regId,errCode,[errDescr|numItems,<type,templateId,contentId,site,flags,priority,from,until,incCountries,numCountries,<country>,<shortText>,<longText>>]

#define TS_Q_PIPEITEMS_UPDATED				509 //44 - reqId,templateId
#define TS_A_PIPEITEMS_UPDATED				(TS_Q_PIPEITEMS_UPDATED + TS_REPLY_DELTA) //42[s|4<444444b4<s>SS>] - regId,errCode,[errDescr|numItems,<type,templateId,contentId,site,flags,priority,incCountries,numCountries,<country>,<shortText>,<longText>>]

#define TS_Q_PIPETOURNBRANDS_NEED_DATA		510 //4 - reqId
#define TS_A_PIPETOURNBRANDS_NEED_DATA		(TS_Q_PIPETOURNBRANDS_NEED_DATA + TS_REPLY_DELTA) //42[s|4<s4444b4<s>S>] - regId,errCode,[errDescr|numItems,<name,templateId,contentId,site,flags,incCountries,numCountries,<country>,<shortText>>]

#define TS_Q_PIPETOURNBRANDS_UPDATED		511 //44 - reqId,name
#define TS_A_PIPETOURNBRANDS_UPDATED		(TS_Q_PIPETOURNBRANDS_UPDATED + TS_REPLY_DELTA) //42[s|4<s4444b4<s>S>] - regId,errCode,[errDescr|numItems,<name,templateId,contentId,site,flags,incCountries,numCountries,<country>,<shortText>>]

// PYR-20438
#define TS_Q_PIPEGPS_NEED_DATA				512 //4 - reqId
#define TS_A_PIPEGPS_DATA					(TS_Q_PIPEGPS_NEED_DATA + TS_REPLY_DELTA) //42[s|4<s4444tt4b4<s>S>] - regId,errCode,[errDescr|numItems,<liveCode,templateId,contentId,subType,site,from,until,flags,incCountries,numCountries,<country>,<imagesB64>>]

#define TS_Q_PIPEGPS_UPDATED				513 //44 - reqId,liveCode
#define TS_A_PIPEGPS_UPDATED				(TS_Q_PIPEGPS_UPDATED + TS_REPLY_DELTA) //42[s|4<s4444tt4b4<s>S>] - regId,errCode,[errDescr|numItems,<liveCode,templateId,contentId,subType,site,from,until,flags,incCountries,numCountries,<country>,<imagesB64>>]

// PYR-24582

#define TS_Q_MOBILE_GENNEWS_UPDATED			514 //444 - reqId, templateId, templateType
#define TS_A_MOBILE_GENNEWS_UPDATED			(TS_Q_MOBILE_GENNEWS_UPDATED + TS_REPLY_DELTA) //46[s|4<...>] reqId,errCode[errDescr|num<I18nNews>]

// PYR-24465
#define TS_Q_TARGETNEWS_UPDATED				515 //44 - reqId, templateId
#define TS_A_TARGETNEWS_UPDATED				(TS_Q_TARGETNEWS_UPDATED + TS_REPLY_DELTA) //46[s|4<...>] reqId,errCode[errDescr|num<I18nNews>]

// PYR-26712
#define TS_Q_TAB_ITEMS_UPDATED				516 //44 - reqId,templateId
#define TS_A_TAB_ITEMS_UPDATED				(TS_Q_TAB_ITEMS_UPDATED + TS_REPLY_DELTA) //42[s|TabItemTs] - regId,errCode,[errDescr|TabItemTs]

#define TS_Q_TAB_ITEMS_NEED_DATA			517 //4 - reqId
#define TS_A_TAB_ITEMS_NEED_DATA			(TS_Q_TAB_ITEMS_NEED_DATA + TS_REPLY_DELTA) //42[s|MultipleTabItemTs] - regId,errCode,[errDescr|MultipleTabItemTs]

// PYR-32225
#define TS_Q_PIPEIMAGES2_NEED_DATA			518	//4 - reqId
#define TS_A_PIPEIMAGES2_NEED_DATA			(TS_Q_PIPEIMAGES2_NEED_DATA + TS_REPLY_DELTA) //42[s|4<s4444b4<s>Stt>] - regId,errCode,[errDescr|numImages,<name,templateId,contentId,site,flags,incCountries,munCountries,<country>,<imagesB64>,from,until>]

#define TS_Q_PIPEIMAGES2_UPDATED			519	//44 - reqId,templateId
#define TS_A_PIPEIMAGES2_UPDATED			(TS_Q_PIPEIMAGES2_UPDATED + TS_REPLY_DELTA) //42[s|4<s4444b4<s>Stt>] - regId,errCode,[errDescr|numImages,<name,templateId,contentId,site,flags,incCountries,munCountries,<country>,<imagesB64>,from,until>]

//#define TS_Q_OBSOLETE_MESSAGE				520
//#define TS_A_OBSOLETE_MESSAGE

//#define TS_Q_OBSOLETE_MESSAGE				521 
//#define TS_Q_OBSOLETE_MESSAGE			

/// PYR-111857  Personalization
#define TS_Q_PS_NEED_DATA					522 //44 - reqId,templateType
#define TS_A_PS_NEED_DATA					(TS_Q_PS_NEED_DATA + TS_REPLY_DELTA) //46[s|4<...>] regId,errCode[errDescr|num<NewsEx>]

/// common
#define TS_Q_COMMON_SEND_EMAIL				600 //44... - reqId,emailType,parameters
#define TS_A_COMMON_SEND_EMAIL				(TS_Q_COMMON_SEND_EMAIL + TS_REPLY_DELTA) //46s - reqId,errCode,errDescr|retStr

#define TS_Q_COMMON_TO_BE_SEND_EMAIL		601 //44... - reqId,emailType,parameters
#define TS_A_COMMON_TO_BE_SEND_EMAIL		(TS_Q_COMMON_TO_BE_SEND_EMAIL + TS_REPLY_DELTA) //46s - reqId,errCode,errDescr|retStr
// PYR-14242
#define TS_Q_COMMON_SEND_SMS                602 //44... - reqId,smsType,parameters
#define TS_A_COMMON_SEND_SMS                (TS_Q_COMMON_SEND_SMS + TS_REPLY_DELTA) //46s - reqId,errCode,errDescr|retStr

// CESAR
#define TS_Q_CESAR_REQUEST_EMAIL            603 //44s14<s>s4ss - reqId,templateId,userId,fromEmail,numParams,<params>,emailAddr,locale,orgEmailText,orgEmailHtml
#define TS_A_CESAR_REQUEST_EMAIL            (TS_Q_CESAR_REQUEST_EMAIL + TS_REPLY_DELTA) //46s - reqId,errCode,errDescr|retStr

// PYR-36862
#define TS_Q_CESAR_EMAIL_BY_TEMPLATEID      604 //44s14<s> - reqId,templateId,userId,fromEmail,numParams,<params>
#define TS_A_CESAR_EMAIL_BY_TEMPLATEID      (TS_Q_CESAR_EMAIL_BY_TEMPLATEID + TS_REPLY_DELTA) //46[s|bmss] - reqId,errCode[errDescr|emailBounced,emaiBody,textBody,htmlBody]

// PYR-110371
#define TS_Q_CESAR_REQUEST_TEMPLATES		605 // 444 - reqId,type,subType
#define TS_A_CESAR_REQUEST_TEMPLATES		(TS_Q_CESAR_REQUEST_TEMPLATES + TS_REPLY_DELTA) //46[s|4<4s444<s4>>] - reqId,errCode[errDescr|numTemplates<templateId,name,type,subType,numLocales<localeName,localeId>>]


/// cashier to TS API:
#define TS_Q_CASHIER_PAYSYSTEM_MESSAGES		700 //4 reqId
#define TS_A_CASHIER_PAYSYSTEM_MESSAGES		(TS_Q_CASHIER_PAYSYSTEM_MESSAGES + TS_REPLY_DELTA) //46[s|4<sss4S>] - regId,errCode,[errDescr|numMessages,<paySystem,subPaySystem,country,type,messageToDisplay(i18nPString)>]

#define TS_Q_CASHIER_GET_ERR_TEMPLATE		701 //4sssssss4 - reqId,userId,gateway,paySystem,transType,gatewayCode,BIN,countryByBin,recType
#define TS_A_CASHIER_GET_ERR_TEMPLATE		(TS_Q_CASHIER_GET_ERR_TEMPLATE + TS_REPLY_DELTA) //42[s|4s] - reqId,errCode,[errDescr|recCode,textForLocale]

#define TS_Q_CASHIER_PLI_FORMS				702 //4ss reqId, userId, paysystem
#define TS_A_CASHIER_PLI_FORMS				(TS_Q_CASHIER_PLI_FORMS + TS_REPLY_DELTA) //42[s|b] - reqId,errCode,[errDescr|pbloc binary with pdf document]
/// cashier to TS API end

/// AMS to TS API
#define TS_Q_AMS_GET_NEWSLETTERS_LIST		800 //4 - reqId
#define TS_A_AMS_GET_NEWSLETTERS_LIST		(TS_Q_AMS_GET_NEWSLETTERS_LIST + TS_REPLY_DELTA) //42[s|4<4s>] - reqId,errCode,[errDescr|num,<templateId,templateName>]

#define TS_Q_AMS_GET_NEWSLETTER_DATA		801 //44 - reqId,templateId
#define TS_A_AMS_GET_NEWSLETTER_DATA		(TS_Q_AMS_GET_NEWSLETTER_DATA + TS_REPLY_DELTA) //42[s|SSS] - reqId,errCode,[errDescr|subj,body,html]

#define TS_Q_AMS_SEND_NEWSLETTER			802 //44ss4<s44<s>> - reqId,templateId,from,adminEmail,numEmails,<email,locale,numParam,<param>>
#define TS_A_AMS_SEND_NEWSLETTER			(TS_Q_AMS_SEND_NEWSLETTER + TS_REPLY_DELTA) //42s - reqId,errCode,errDescr
/// AMS to TS API end

// Request from admin proxy
// PYR-18543
#define TS_Q_IS_ALIVE						900 // 4 - reqId (obsolete - TS_A_IS_ALIVE is sent with reqId=0)
#define TS_A_IS_ALIVE						(TS_Q_IS_ALIVE + TS_REPLY_DELTA) //4 - reqId

// PYR-18694 notification message
#define TS_N_COMMON_SEND_SMS                (901 + TS_REPLY_DELTA) //4s6s4 - reqId=0,userId,errCode,errDescr,replyMsgId

// *** NEW RANGE FOR DBM LOCALIZED MESSAGES ***
// PYR-21685
// -> templateCache
#define TS_Q_I18N_SELFEXCLUDED_EXPLANATION	 2000 // 4s - reqId,userId
#define TS_A_I18N_SELFEXCLUDED_EXPLANATION	 (TS_Q_I18N_SELFEXCLUDED_EXPLANATION + TS_REPLY_DELTA) // 46s reqId,errCode,[errDescr|text]

// PYR-19814
// -> templateCache
#define TS_Q_I18N_ERROR_MESSAGE				 2001 // 4s4244t[4] - reqId,userId,locale,internalErrCode,errLocationMark,module,when[,errNamespace]
#define TS_A_I18N_ERROR_MESSAGE				 (TS_Q_I18N_ERROR_MESSAGE + TS_REPLY_DELTA) // 46s reqId,errCode,[text|errDesc]

// -> templateServer
#define TS_Q_I18N_GET_INTERNAL_ERROR_TEMPLATES	2002 // 4 - reqId
// Can be used as notification if reqId=0. In this case can update/delete/insert of elements of collection
#define TS_A_I18N_GET_INTERNAL_ERROR_TEMPLATES	(TS_Q_I18N_GET_INTERNAL_ERROR_TEMPLATES + TS_REPLY_DELTA) // 46[4<s,I18nPString> reqId,numInternalErrors,<internalErrorName,internalErrorText>|errDescr]

// -> templateServer
#define TS_Q_I18N_ERROR_MESSAGE_AUDIT		 2003 // 4ss44t - reqId, internalErrorName, userId, mapped, module, when
#define TS_A_I18N_ERROR_MESSAGE_AUDIT		 (TS_Q_I18N_ERROR_MESSAGE_AUDIT + TS_REPLY_DELTA) // 46s reqId,errCode,[errDesc]

// -> templateCache
#define TS_Q_I18N_ERROR_MESSAGE_AUDIT_ONLY	 2004 // 4s4244t[4] - reqId,userId,locale,internalErrCode,errLocationMark,module,when[,errNamespace]
#define TS_A_I18N_ERROR_MESSAGE_AUDIT_ONLY	 (TS_Q_I18N_ERROR_MESSAGE_AUDIT_ONLY + TS_REPLY_DELTA) // 46[s] reqId,errCode[,errDesc]

// PYR-24812
// -> templateServer
#define TS_Q_I18N_GET_MESSAGE_TEMPLATES		2005 // 4 - reqId
#define TS_A_I18N_GET_MESSAGE_TEMPLATES		(TS_Q_I18N_GET_MESSAGE_TEMPLATES + TS_REPLY_DELTA) // 46[4<4,s,s,I18nPString>|s] reqId,errCode,[numMsgs,<templateType,templateName,criteria,templateText>|errDescr]

// PYR-24812
// client ->
#define TS_Q_I18N_GET_TRANSLATED_MESSAGE	2006 // 4444444 category,subcategory,clientPlatformId,clientType,clientDevice,clientSiteId,locale
#define TS_A_I18N_GET_TRANSLATED_MESSAGE	(TS_Q_I18N_GET_TRANSLATED_MESSAGE + TS_REPLY_DELTA) // 6s 0,templateText

// PYR-24812
// client ->
#define TS_Q_I18N_GET_TRANSLATED_INTERNAL_ERROR	2007	//s44444 - internalErrorDescr,clientPlatformId,clientType,clientDevice,clientSiteId,locale
#define TS_A_I18N_GET_TRANSLATED_INTERNAL_ERROR	(TS_Q_I18N_GET_TRANSLATED_INTERNAL_ERROR + TS_REPLY_DELTA)	//6s - 0,translatedError

// PYR-24241
// cashier/client ->
#define TS_Q_I18N_GET_TRANSLATED_BONUSES_STRINGS 2008 // 44<4> - localeId,numBonuses,<bonusId>
#define TS_A_I18N_GET_TRANSLATED_BONUSES_STRINGS (TS_Q_I18N_GET_TRANSLATED_BONUSES_STRINGS + TS_REPLY_DELTA) // 4<4sss> - numRecords<M(bonusId,name,description,url)>

// PYR-24241
// -> templateServer
#define TS_Q_I18N_GET_BONUSES_TEMPLATES		2009 // 4 - reqId
#define TS_A_I18N_GET_BONUSES_TEMPLATES		(TS_Q_I18N_GET_BONUSES_TEMPLATES + TS_REPLY_DELTA) // 46[4<4 <I18nPStringEx>>|s] reqId,errCode,[numRecords,<bonusId,<bonusName,bonusDescr,bonusUrl>>|errDescr]

// PYR-24241
// templateServer ->
#define TS_N_I18N_REREAD_BONUSES_TEMPLATES	2010 // 4 - reqId

// PYR-24241
// templateServer -> templateCache
#define TS_N_I18N_GET_BONUSES_TEMPLATES	2011 // format the same as TS_A_I18N_GET_BONUSES_TEMPLATES

// PYR-26155
// lobby -> templateCache
#define TS_Q_I18N_GET_USER_POPUP_MESSAGE	2012 // templateId
#define TS_A_I18N_GET_USER_POPUP_MESSAGE	(TS_Q_I18N_GET_USER_POPUP_MESSAGE + TS_REPLY_DELTA) // 6[s|S] errCode,[ errDescr | i18nTemplateText ]

// PYR-26809
#define TS_Q_I18N_SET_RESTART_CODE			2013 // restartCode
#define TS_A_I18N_SET_RESTART_CODE			(TS_Q_I18N_SET_RESTART_CODE + TS_REPLY_DELTA) // 6[s|S] errCode,[ errDescr ]

// PYR-32547
#define TS_Q_LOBBY_GET_SPEC_MAIL_TEMPLATES	2014 // 4 - reqId
#define TS_A_LOBBY_GET_SPEC_MAIL_TEMPLATES	(TS_Q_LOBBY_GET_SPEC_MAIL_TEMPLATES + TS_REPLY_DELTA) //46[s|4<sb>] - regId,errCode[errDescr|numElems,<name,status>]

// PYR-34104
// cashier -> templatecache
#define TS_Q_I18N_GET_TRANSLATED_ROLLS_STRINGS 2015 // 44<4> - localeId,numRolls,<rollId>
#define TS_A_I18N_GET_TRANSLATED_ROLLS_STRINGS (TS_Q_I18N_GET_TRANSLATED_ROLLS_STRINGS + TS_REPLY_DELTA) // 4<4ss> - numRecords<rollId,shortDescr,longDescr>

// PYR-34104
// templatecache/cashier/auxhub -> templateServer
#define TS_Q_I18N_GET_ROLLBONUSES_TEMPLATES		2016 // 4 - reqId
#define TS_A_I18N_GET_ROLLBONUSES_TEMPLATES		(TS_Q_I18N_GET_ROLLBONUSES_TEMPLATES + TS_REPLY_DELTA) // 46[4<4 <I18nPStringEx>>|s] reqId,errCode,[numRecords,<bonusId,<bonusShort,bonusLong>>|errDescr]

// PYR-34104
// templateServer -> templatecache/cashier/auxhub
#define TS_N_I18N_REREAD_ROLLBONUSES_TEMPLATES	2017 // 4 - reqId

// PYR-34104
// templateServer -> templateCache
#define TS_N_I18N_GET_ROLLBONUSES_TEMPLATES		2018 // format the same as TS_A_I18N_GET_ROLLBONUSES_TEMPLATES

//PYR-33574
#define TS_Q_NEED_SAG_PAYOUT_TICKETS_DATA		2019 //44<s>... - reqId,numTickets<ticketTypeIdOrTargetRef>
#define TS_A_NEED_SAG_PAYOUT_TICKETS_DATA		(TS_Q_NEED_SAG_PAYOUT_TICKETS_DATA + TS_REPLY_DELTA) //46[s|4<()>} - reqId,errCode[errDescr|numTickets<(DataBod)y>]

// PYR-42273
// mglobby -> templateserver
#define TS_Q_I18N_GET_MGLOBBY_MESSAGES			2020 // 4 - reqId
#define TS_A_I18N_GET_MGLOBBY_MESSAGES			(TS_Q_I18N_GET_MGLOBBY_MESSAGES + TS_REPLY_DELTA) // 46[4<s,I18nPString>|s] reqId,errCode,[numMsgs,<resId,templateText>|errDescr]

// templateServer ->
#define TS_N_I18N_REREAD_MGLOBBY_TEMPLATES		2021 // 4 - reqId

// PYR-46163
// cashier -> templateServer
#define TS_Q_I18N_GET_GAMETICKETS_TEMPLATES		2022 // 4 - reqId
#define TS_A_I18N_GET_GAMETICKETS_TEMPLATES		(TS_Q_I18N_GET_GAMETICKETS_TEMPLATES + TS_REPLY_DELTA) // 46[4<4 <I18nPStringEx>>|s] reqId,errCode,[numRecords,<gameTicketId,<gameTicketShort,gameTicketLong>>|errDescr]

// PYR-46163
// templateServer -> cashier
#define TS_N_I18N_REREAD_GAMETICKETTYPES_TEMPLATES	2023 // 4 - reqId

// PYR-62676
// lobbyetc -> templateserver
#define TS_Q_I18N_GET_CHEST_TEMPLATES			2024 // 4 - reqId
#define TS_A_I18N_GET_CHEST_TEMPLATES			(TS_Q_I18N_GET_CHEST_TEMPLATES + TS_REPLY_DELTA) // 46[4<s,I18nPString>|s] reqId,errCode,[numMsgs,<resId,templateText>|errDescr]

// templateServer -> LobbyETC (push)
#define TS_N_I18N_REREAD_CHEST_TEMPLATES		2025 // 4 - reqId
#define TS_N_UPDATE_RM_BLOCKED_COUNTRIES_STATES	2026 // 44<s> - reqId,rmBlockedCountriesStates<countryState>


// PYR-124681  HubFeedServer
#define TS_Q_HFS_NEED_DATA					2100 //44 - reqId,templateType
#define TS_A_HFS_NEED_DATA					(TS_Q_HFS_NEED_DATA + TS_REPLY_DELTA) //46[s|4<...>] regId,errCode[errDescr|num<TemplateStruct>]

#define TS_Q_HFS_GET_TEMPLATE_DATA			2101 //44 - reqId,templateId
#define TS_A_HFS_GET_TEMPLATE_DATA			(TS_Q_HFS_GET_TEMPLATE_DATA + TS_REPLY_DELTA) //46[s|4<...>] regId,errCode[errDescr|num<TemplateStruct>]

enum eTypeEmailAddress
{
	eTypeEmailAddressSupport = 0,
	eTypeEmailAddressCashier = 1,
	eTypeEmailAddressSecurity = 2,
	eTypeEmailAddressCollusion = 3,
	eTypeEmailAddressFppStore = 4,
	eTypeEmailAddressVipStore = 5,
	eTypeEmailAddressWUSupport = 6,
	eTypeEmailAddressMGSupport = 7,
	eTypeEmailAddressMGCashier = 8,
	eTypeEmailAddressVerification = 9,
	eTypeEmailAddressChat = 10,
	eTypeEmailAddressWecare = 11,
	eTypeEmailAddressDad = 12,
	eTypeEmailAddressVipClub = 13,
	eTypeEmailAddressCashierGen = 14,
    eTypeEmailAddressPaymentServices = 15, // #16826
	eTypeEmailAddressTournDeal = 16, //PYR-15846
	eTypeEmailAddressNoreply = 17,
    eTypeEmailAddressHomeGames = 18, // #17590
	eTypeEmailAddressAV = 19, // PYR-17003
	eTypeEmailAddressEvents = 20, // PYR-25273
	eTypeEmailAddressPMPurchse = 21, // PYR-29144
	eTypeEmailAddressPromotions = 22, // PYR-66669

	eTypeEmailAddressIgnoreIt = 255
};

enum eSubTypeFrAccValidation
{
	eSubTypeFrAccValidationInfo = 1,
	eSubTypeFrAccSuspendedDocs = 3,
	eSubTypeFrAccValidatedAvc = 4,
	eSubTypeFrAccClosedDocs = 6,
	eSubTypeFrAccSuspendedAvc = 7,
	eSubTypeFrAccClosedAvc = 8,
	eSubTypeFrAccReminderDocs = 9,
	eSubTypeFrAccReminderAvc = 10,
	eSubTypeFrAccValidationInfoBA = 11,
	eSubTypeFrAccSuspendedBA = 12,
	eSubTypeFrAccClosedBA = 13,
	eSubTypeFrAccMismatchPdc = 14,
	eSubTypeFrAccClosedNoUserPermission = 15,
	eSubTypeFrAccClosedChangeNotAccepted = 16,
	eSubTypeFrAccLoginRequest = 17,
	eSubTypeFrAccReminderDocs1 = 901,
	eSubTypeFrAccReminderDocs2 = 902,
	eSubTypeFrAccReminderDocs3 = 903,
	eSubTypeFrAccReminderDocs4 = 904,
	eSubTypeFrAccReminderDocs5 = 905,
	eSubTypeFrAccReminderDocs6 = 906,
};

// PYR-18621
enum eSubTypeItAccVerification
{
	eSubTypeItAccSuspendedDocs = 3,
	eSubTypeItAccClosedDocs = 6,
};

// PYR-20730
enum eSubTypeAccVerification
{
	eSubTypeAccSuspendedDocs = 3,
	eSubTypeAccClosedDocs = 6,
};

// PYR-16705
enum eSubTypeAccMigration
{
	eSubTypeAccMigrationSuccess = 0,
	eSubTypeAccMigrationCancel  = 1,
};

// PYR-18694
enum eSubTypeSmsValidation
{
	eSubInitialValidation = 1,
	eSubChangedValidation = 2,
	eSubSuccessfulValidation = 3,
	eSubChangedInstallIdValidation = 4,	// PYR-21829
};

enum eSubTypeSmsPwdReset
{
	eSubSmsPwdResetSuccess = 1,
	eSubSmsPwdResetFailure = 2,
};

//	PYR-28732
enum eSubTypeChallengeQuestions
{
	eSubChallengeQuestionAdd = 1,
	eSubChallengeQuestionEdit,
	eSubChallengeQuestionDelete
};

enum eEmailType
{
	eEmailGeneral = 0,							//ssss1 - userId,subj,body,htmlBody,fromEmail
	//admin
	eEmailDisapproval = 1,						//1bs - subType,deny,userId
	eEmailRevokeChat = 2,						//1bsss - subType,isComplainer,userId,signature,line
	eEmailSecureId = 3,							//ss - userId,sid
	//admin+cashier
	eEmailFundTransfer = 4,						//1ss44s[84s] - subType,userFrom,userTo,amount,price,currency[,convRate,converted,convCurrency]
	//admin
	eEmailAdminTransfer = 5,					//11ss444s[84s] - subType,reason,userFrom,userTo,transIdFrom,transIdTo,amount,currency[,convRate,converted,convCurrency]
	//admin+vipstore
	eEmailCancelFPPOrder = 6,					//1s44<4s4>[4] - subType,userId,Total FPPs,numOrders<orderId,itemName,itemId>[,templateid]
	//admin
	eEmailIssueBOPPrizes = 7,					//s1144 - userId,division,subDivision,ticketTypeId,amount
	eEmailLimitRequestResult = 8,				//s1444 - userId,status,currentLimit,requestedLimit,approvedLimit
	eEmailPendingMGApproved_obsolete = 9,		//sss4s - userId,mgRN,refId,amount,currency
	eEmailMicrodepositReject = 10,				//s - userId
	eEmailDBTCashoutReject = 11,				//s4t4s - userId,transId,time,amount,currency
	eEmailTransCorrection = 12,					//1s4st4s4s - subType,userId,transId,paySystem,depositDate,amount,currency,converted,convCurrency
	eEmailAdminReject = 13,						//s4st4s4s - userId,transId,paySystem,transDate,amount,currency,converted,convCurrency
	eEmailUnknownRejectTrans_obsolete = 14,		//sb4t4ss - userId,isApproved,transId,transDate,amount,currency,cardNum
	eEmailWireCashouts = 15,					//s44sts - userId,transId,amount,currency,requestedTime,bankAccount
	eEmailEarlyExclusionExpiration = 16,		//ss - userId,fromEmail
	eEmailVisaDebitLink_obsolete = 17,			//ss - userId,userUrl
	eEmailNTRejectTrans = 18,					//s44s - userId,action,amount,currency
	eEmailTMICourierTrack = 19,					//s14sds - userId,courier,amount,currency,whenMailed,wayBill
	//admin+cashier
	eEmailWuReceiver = 20,						//ssssd44s - userId,wuReceiver,wuCity,wuCountry,whenExpired,maxAmount,minAmount,currency
	eEmailWuTransact = 21,						//s4s4s4<attr>4s - userId,amount,currency,bonusAmount,bonusCurrency,fppAmount,extraAttr,converted,convCurrency
	eEmailWuTransactR = 22,						//ssss - userId,amountReqStr,currency,gwErrCode
	//admin
	eEmailCDMS = 23,							//1ss - subType,userId,agent
	eEmailPendingGCLBTTrans = 24,				//s4s4s - userId,amount,currency,converted,convCurrency
	eEmailMatchMGApproved_obsolete = 25,		//sss4s - userId,mgrn,refId,amount,currency
	eEmailMgTrack_obsolete = 26,				//s4ssst - userId,amount,currency,wayBill,sender,started
	//lobby
	eEmailEmailChanged = 27,					//ss - userId,oldEmail
	eEmailActivationCode = 28,					//sss44s44 - userId,email,from,code,locale,country,siteId,platformId
	eEmailRemindSid = 29,						//sss - userId,email,sid
	eEmailResetPwd = 30,						//ss - userId,password
	eEmailAccountClosed = 31,					//s - userId
	eEmailSelfExclude = 32,						//s4tb - userId,days,until,groupSelfExclusion
	eEmailExcludedExpired = 33,					//s4 - userId,days_hours
	eEmailTableLimitsNew = 34,					//s4444 - userId,limitHours,limitDays,limitType,limitAmount_notUsed
	eEmailTableLimitsRemove = 35,				//s - userId
	eEmailVipStatus = 36,						//s4444 - userId,vipStatus,minFpp,minFppNext,fppRate (OBSOLETE!!!)
	eEmailTWChipsTradeReply = 37,				//ss4s4 - sellerId,buyerId,amount,currency,tChips
	eEmailTWSaleExpired = 38,					//bss4s4 - isSeller,sellerId,buyerId,amount,currency,tChips
	eEmailECheckBounced = 39,					//s44st4sd - userId,transId,amount,currency,completed,authCode,authMsg,representmentDate
	eEmailTransferFailedAge = 40,				//s - userId
	eEmailChatRestored = 41,					//s - userId
	eEmailAdminRevoked = 42,					//s44[s] - userId,adminLoginId,numEmails,[emails]
	//admin
	eEmailLeaguePrize = 43,						//s4s4s - userId,place,leagueName,amount,currency
	//auth
	eEmailPsPin = 44,							//ss - userId,pin
	//lobby. PYR-81908: obsolete
	eEmailPMTransfers_obsolete = 45,			//sdd4[rec] - userId,dateFrom,dateTo,numTrans,[pmTransfer]
	eEmailLeaderBoard = 46,						//sdd44[rec] - userId,dateFrom,dateTo,brandId,numRec[lpRec]
	eEmailExcludedUnregistered = 47,			//s4[] - userId,numTournBuyIns,[tournBuyIn]
	//admin
	eEmailTournChop = 48,						//4s4[ss444] - tournId,currency,numUsers,[userId,place,award,payout,brandId]
	eEmailUTrans = 49,							//s1444s4st4[41ss4s4s] - userId,subType,amount,refund,overcredit,currency,converted,convertedCurrency,time,numTrans,[transId,status,paySystem,payMethod,amount,currency,converted,convertedCurrency]
	//table
	eEmailTimeoutReply_obsolete = 50,			//s4 - userId,handId
	//auth
	eEmailLoginSettings = 51,					//s - userId
	//admin
	eEmailRakebackReport = 52,					//4s4<s44444444s8> - templateId,adminEmail,numUsers,<userId,rake,freerolls,tournFees,fpps,msBonuses,msFpps,percentage*100,amount,currency,convRate>
	eEmailRevenueShareReport = 53,				//4s4<s44444s> - templateId,adminEmail,numUsers,<userId,revenue,expenses,netRevenue,percentage*100,amount,currency>
	//cashier
	eEmailPurchaseLimitIncrease = 54,			//ss444s - userId,group,limit01,limit07,limit30,currency
	//mtlobby
	eEmailCancelTournament = 55,				//1s<rec><rec>4441444s444 - subType,userId,tournHeader,user5Cont,numPlayers,chipsLeft,chipsInPlay,awardPool,equalShare,bounty,addonRebuyRakeRefundForRollforward,brandId
	eEmailTournamentReply = 56,					//1s<rec>44444s144s1s4<444<s>>4 - subType,userId,tournHeader,place,award,targetId,twMoney0,twMoney1,admission,canUnreg,points,boardType,specialTitle,isTAward,tournName,numBounties,<bountyType,bountyAmount,numPlayers,<playerId>>,brandId
	//auth
	eEmailRSAActivated = 57,					//s - userId
	//admin
	eEmailCustomOrderAppr = 58,					//ss4s - userId,item,price,itemName
	eEmailCustomOrderReject = 59,				//s - userId
	eEmailDocsRequest = 60,						//s - userId
	eEmailFsaTracking = 61,						//4<sdsss4<4s4>> - numUsers,<userId,shippingDate,shippingUrl,trackingCode,shipper,numOrders<orderI,itemName,itemId>>
	eEmailInvC2pRet_obsolete = 62,				//s4d4s - userId,transId,depositDate,converted,convCurrency
	eEmailBugsyPwdReset = 63,					//ss - userEmail,newBugsyPwd
	//cashier
	eEmailBugsyAccConf = 64,					//s4 - userId,balance
	eEmailFundsCleared = 65,					//ss4s - userId,paysystem,amount,currency
	//chatmon
	eEmailChatMonRes = 66,						//s4ds4 - userId,resolution,violationDate,quotes,violationType
	//vipstore
	eEmailFppStoreOrder = 67,					//s1s444<ss> - userId,hidePI,item,price,coupon,numProps,<name,value>
	eEmailFppStoreGiftCardOrder = 68,			//s4ssssd4 - userId,price,currency,claimCode,vendor,vendorDescr,expirationDate,orderId
	eEmailFppStoreBonusOrder = 69,				//s44s44t - userId,fppPrice,bonusId,bonusName,mustEarn,expPeriod,expDate
	eEmailFppStoreTournsOrder = 70,				//ss4 - userId,itemName,fppPrice
	eEmailFppStoreSpecialOrder = 71,			//s4 - userId,emailId
	//lobby
	eEmailTournStats = 72,						//s1b4<4>44<..>4- userId,subType,noStat,numElems,<prizePool>,tournId,numTourns,<lastHandId64,TournHeader,stats,players>,brandId
	eEmailGameStats = 73,						//s4<rec>4 - userId,reqHands,gameStats,brandId
	eEmailHandHistory_obsolete = 74,			//s4184<rec>4 - userId,locale,requestType,requestContents,numHands,<hands>,brandId
	//crmserver
	eEmailAuditAccountReport = 75,				//stts - userId,startTime,endTime,reportUrl
	eEmailAuditAccountReportErr = 76,			//sttbs - userId,startTime,endTime,isKnown,errDescr
	//cashier
// PYR-16411 eEmailMoneyGram is obsolete	
	eEmailMoneyGram_obsolete = 77,				//rss - <rec>,receiverCode,companyCode
	eEmailUserSetLimit = 78,					//s14st - userId,status,limit,currency,enforcedUntil
	eEmailAgeVerifyFailed = 79,					//s - userId
	eEmailAvlsRequest = 80,						//s11tss - userId,requestType,avSystem,requestTime,phone,whenContact
	eEmailTooManyAVFailedAttempts = 81,			//s14 - userId,avSystem,attempts
	//lobby
	eEmailCom2ItTransferred = 82,				//ssd44444 - senderId,receiverId,date,amountTEUR,amountUSD,amountTUSD,amountWUSD,fpp
	eEmailCom2ItConverted = 83,					//sd4444444444 - userId,date,amountTEUR,amountUSD,amountTUSD,amountWUSD,amountTicket,amountBonus,fpp,vipStatus,monthlyVpp,yearlyVpp
	eEmailCom2ItClosed = 84,					//ssd4444444444 - senderId,receiverId,date,amountTEUR,amountUSD,amountTUSD,amountWUSD,amountTicket,amountBonus,fpp,vipStatus,monthlyVpp,yearlyVpp
	//cashier
	eEmailDadRequest = 85,						//s1tss - userId,requestType,requestTime,phone,whenContact
	eEmailGCDebit = 86,							//s<rec> - userId,<item>
	eEmailChargeBackNotif = 87,					//s4ss444st4[b] - userId,flags,cardNum,paySystem,amount,transId,owedAmount,currency,completed,reason[,isPM]
	eEmailLbtPurchase = 88,						//s4s4ss - userId,amount,currency,converted,convCurrency,paySystem
	eEmailMoneyOrderInfo_obsolete = 89,			//ss - userId,country
	eEmailWireInfo = 90,						//<rec>44s - bankInfo,minDeposit,maxDaily,currency
	eEmailWireComplete = 91,					//<rec> - notify
	eEmailLbtRequest = 92,						//s<rec>ss44ss - userId,bankInfo,bankCountry,uniqueRef,minDeposit,maxDaily,currency,procRef
	eEmailCitadelRequest_obsolete = 93,			//s<rec>s44s - userId,info,uniqueRef,minDeposit,maxDaily,currency  // PYR-49205 Removed
	eEmailRedeemReturn = 94,					//s4s44st - userId,redeemId,paySystem,amount,converted,currency,completed
	eEmailRepresentmentNotif = 95,				//s444s - userId,transId,amount,converted,currency
	eEmailEPassReject_obsolete = 96,			//2ss4sst4 - rejectCode,userId,userName,amount,currency,account,started,timeout
	eEmailCnbuyNotif = 97,						//s144s4st - userId,subType,transId,amount,currency,converted,convCurrency,dateTime
												//subType: 0 - debit, positive balance; 1 - debit, negative balance; 2 - credit
	//admin+cashier
	eEmailBoletoComplete = 98,					//s4s4s - userId,amount,currency,converted,convCurrency
	//cashier
	eEmailClick2PayReturn_obsolete = 99,		//s44st - userId,purchaseId,amount,currency,dateTime // IPONS #20342 todo: remove
	eEmailClick2PayCollection_obsolete = 100,	//s44st - userId,purchaseId,amount,currency,dateTime // IPONS #20342 todo: remove
	eEmailGracePeriodStarted_obsolete = 101,	//sb - userId,isTransfer
	eEmailCancelPendingCashout = 102,			//s<rec> - userId,cancelPendingCashouts
	eEmailBoletoRequest = 103,					//s - userId
	//mtlobby
	eEmailSatelliteWinnerRefund = 104,			//44<s444s44> - targetTournId,numUsers,<userId,chips,fpp,tRefund,currency,ticketId,brandId>
	//admin
	eEmailAchDelayedNotif = 105,				//s44stt - userId,transId,amount,currency,started,endTime
	eEmailAchDelayedDebit = 106,				//s44st1 - userId,transId,amount,currency,started,isNegative
	//cashier
	eEmailTicketBonus = 107,					//ss44 - userId,bonusName,bonusId,brandId
	eEmailDepositBonus = 108,					//s4<rec> - userId,bonusAmount,bonusExtraAttributes
	//lobby
	eEmailAccountAutoClosed = 109,				//s4 - userId,subType
	eEmailClientToSupport = 110,				//s444ssss4<sB> - userId,siteId,cat,subCat,subj,body,clientVersion,clientOsInfo,numAtt,<attName,attData>
	//admin
	eEmailLeaguePrizeEx = 111,					//s44s4s - userId,subType,place,leagueName,amount,currency
	//common
	eEmailDirect = 112,							//sssss[4<sb>] - toEmail,fromEmail,subj,body,htmlBody[numAttch<fileName,binData>]
	//cashier
	eEmailWirecardInfo = 113,					//sss4 - userId,paymentRef,currency,minDeposit
	eEmailWirecardComplete = 114,				//<rec> - notify
	eEmailTransact = 115,						//<rec>bss - trans,hideInfo,invoice,gatewayTranslated
	eEmailUserBanned = 116,						//s4ss - userId,subType,parentUserId,maskedCardNum
	//admin
	eEmailGenericComplete = 117,				//s44s4s - userId,transId,amount,currency,converted,convCurrency
	//cashier
	eEmailInstadebitWalletReturn = 118,			//s44st - userId,purchaseId,amount,currency,dateTime
	eEmailOwedChipsLiquidated = 119,			//s4s4s4s4<4s>4 - userId,<credited>,<owedBefore>,<owedAfter>,<infos>,chipsType
	eEmailUmwRegistration_obsolete = 120,		//ssss - userId,umwUserId,umwPin,umwActivationUrl
	eEmailCuentaDigitalRequest = 121,			//s - userId
	//admin
	eEmailChequeCancel = 122,					//st4s4s4s - userId,started,transId,chequeNumber,amount,currency,converted,convCurrency
	eEmailCuentaDigitalComplete = 123,			//s4s4s - userId,amount,currency,converted,convCurrency
	//admin+cashier
	eEmailEnvoyRequest = 124,					//ss4 - userId,serviceName,validDays
	//fppstore
	eEmailLowUnitAlert = 125,					//4ssss4444<s> - itemId,itemDescr,itemSpecs,stockId,warehousName,numAvailable,numReserved,numOrdered,numEmails,<emailTo>
	//lobby
	eEmailStellarRewardPayment = 126,			//s4s4 - userId, amount, currency, vppLevel
	eEmailForgotUserId = 127,					//s - userId
	//admin
	eEmailChequeCorrection = 128,				//s4s4s4std - userId,transId,chequeNum,cashoutAmount,cashoutCurrency,chequeAmount,chequeCurrency,requestTime,courierDate
	//cashier
	eEmailChequesMailed = 129,					//s44s4sTsD - userId,transId,acctAmt,acctCur,chqAmt,chqCur,startedTime,chqNumber,mailedDate
	//admin+cashier
	eEmailAffiliateDeposit = 130,				//sss44s4s44 - templateName,userId,affiliateId,transId,amount,currency,converted,convCurrency,fpp,expiryDays
	//lobby
	eEmailPokerRoomManagementAlertStopSNG = 131,//4<s>ss<rec> - number of emails, emails, script title, reason, tourn header
	eEmailTournLimitsNew = 132,					//s4444 - userId,limitHours,limitDays,limitType,limitAmount_notUsed
	eEmailTournLimitsRemove = 133,				//s - userId
	//mtlobby
	eEmailPlayMoneyWinner = 134,				//s444 - userId,tournId,place,award
	//admin
	eEmailDeliveryNotif = 135,					//4<sdsss4<4s4>> - numUsers,<userId,shippingDate,shippingUrl,trackingCode,shipper,numOrders<orderI,itemName,itemId>>
	//cashier
	eEmailFrMigrationWarning = 136,				//s4 - userId,subType
	eEmailFrMigrationFinished = 137,			//sst44444s444 - userId,userIdFr,time,vipStatusCom,vipStatusFr,cashBalance,bonusBalance,tmoneyBalance,currency,fpp,montlyVpp,yearlyVpp
	//lobby+cashier
	eEmailFrAccValidation = 138,				//s4 - userId,subType[,numCustomParams<customParam>]
	eEmailAccountBlacklisted = 139,				//s4 - userId
	//lobby
	eEmailBuyInLimitApproved = 140,				//s - userId
	//cashier
	eEmailUkashVoucherCashout = 141,			//st4ss44 - userId,time,amount,currency,voucher,transId,licenseId
	eEmailUkashVoucherDeposit = 142,			//st4ss44 - userId,time,amount,currency,voucher,transId,licenseId
	eEmailUkashVoucherCancel = 143,				//s4s - userId,amount,currency
	//common
	eEmailGeneralWithAttachments = 144,			//ssss1s4<sB> - userId,subj,body,htmlBody,fromEmailInd,fromEmail,numAtt,<fileName,attachment>
	eEmailDirectMultiTo = 145,					//4<s>ssss[4<sb>] - numEmails,<toEmail>,fromEmail,subj,body,htmlBody[numAttch<fileName,binData>]
	//admin+cashier
	eEmailVerificationForm = 146,				//s44<s> - userId,formId,numParam,<param>
	eEmailExcludedUnregisteredEstonia = 147,	//s4[] - userId,numTournBuyIns,[tournBuyIn]
	eEmailExcludedUnregisteredUnderAge = 148,	//s4[] - userId,numTournBuyIns,[tournBuyIn]
	//lobby
	eEmailEeMigration = 149,					//s4 - userId,migrFlag
	eEmailDocsUploadFailed = 150,				//s - userId
	//cashier
	eEmailExcludePlayer = 151,					//s4 - userId,reason
	//lobby
	eEmailPlayingTimeRestriction = 152,			//s1 - userId,subType (1 - set, 2 -remove)
    //cashier PYR-16749
    eEmailTChipsSoldToPStars = 153,             //ss44 - userId, currency, amount, tChips
	//lobby ( OBSOLETE!!!)
	eEmailStellarReward = 154,					//ss444444 - userId,currency,firstVpp,firstReward,nextVpp,nextReward,totalNum,totalReward
	//mtlobby
	eEmailSemiAutoDeal = 155,					//4144<s> - tournId,subType,siteId,numUsers,<userId>
    // PYR-17160
    eEmailAchNotSubmitted = 156,                //s44stts - userId,transId,amount,currency,started,completed,descriptor
	eEmailNTicketsBonus = 157,					//sss44 - userId,bonusName,admission,numTickets,bonusId
	//lobby PYR-18621
	eEmailItAccVerification = 158,				//s4 - userId,subType
// PYR-18694
	eEmailSMSValidation = 159,					//sss44s4 - userId,email,from,code,locale,country,siteId
	// PYR-20725
	eEmailEsMigrationSuccess = 160,				//s - userId
	// PYR-20730
	eEmailEsAccVerification_obsolete = 161,		//s4 - userId,subType
	// PYR-16705
	eEmailDkMigration = 162,					//s - userId,subType (eSubTypeAccMigration)
	eEmailUserSetSelfLimits = 163,				//ss4{B} - userId,currency,numLimits,{limitBody}
	// PYR-23166
	eEmailTournamentReplySpecial = 164,			//M4<444<s>>4 - TournEmailTemplateRequest,numBounties<bountyType,bountyAmount,numPlayers,<playerId>>,brandId
	eEmailFirstFpp = 165,						//s - userId (OBSOLETE!!!)
	eEmailHallOfFame = 166,						//s41 - userId,LTVPP,emailTo(0-user,1-CSR) (OBSOLETE!!!)
	//crmserver - PYR-27579
	eEmailAuditResponsibleGamingAccountReport = 167,	//stts - userId,startTime,endTime,reportUrl
	//	PYR-27258
	eMailPwdChanged = 168,						//	s - userId
	// PYR-27726 & PYR-28406
	eEmailAdminResortDeposit = 169,				// s4s - userId,amount,currency
	//	PYR-28611
	eEmailForgotUserId2 = 170,					//	s444<s> - email,locale,siteId,size<userid>
	// PYR-26438
	eEmailFlightWinner = 171,					//s4b4 - userId,nextTournId,canUnreg,brandId
	// PYR-29729
	eEmailGreekTaxPaid = 172,					// s4d444s - userId,curEURBalance,taxDay,gameProfit,taxPaid,cashTransId,currency
	// PYR-28733
	eEmailForgotEmail = 173,					//s - userId
	// PYR-29581
	eEmailCasinoLimitApproved = 174,			//s - userId
	eEmailCasinoLimitsNew = 175,				//s4444 - userId, hourLimit, daysLimit, limitType, limitAmount_notUsed
	//	PYR-28732
	eMailChallengeQuestions = 176,				//1s - eSubTypeChallengeQuestions,userId
	// PYR-27624
	eEmailAstropayDirectPurchaseRequest = 177,  //sss4s userId, bankName, bankUrl, amount, currency
	// PYR-25911
	eEmailSatWinnerRugUnreg = 178,				//ss44<s>4 - userId, userId2,tournId,tournName(I18nPString),brandId
	// PYR-30476
	eEmailFirstDepositNotification = 179,		//s - userId
	//PYR-32180
	eEmailSportsLimitApproved = 180,
	// PYR-29932
	eEmailValidationUnathorized_obsolete = 181,	// sss4s4 - userId,email,from,locale,country,siteId
	// PYR-33955
	eEmailSportBooksLimitsNew = 182,			//s4444 - userId, hourLimit, daysLimit, limitType, limitAmount_notUsed
	// PYR-34388
	eEmailSportsAccumulatedLimitApproved = 183,	//s - userId
	// PYR-35273
	eEmailPokerRoomManagementAlertStopSAG = 184,//4<s>ss<rec> - number of emails, emails, script title, reason, tourn header
	// PYR-32003
	eEmailFlightUserMoved = 185,				//ss4tt4 - userId,tournName,phaseNum,from,until,brandId
	// PYR-34643
	eEmailInstallIdForceReAuth = 186,			// ssssss844b - userId,IPaddress,location,oldInstallId,newInstallId,routerMac,loginId,lobbySiteId,platformId,includeSingedLink
	// PYR-36066 // PYR-53789 - obsolete
	//eEmailDuelCanceled = 187,					// ss88sbb - userId, userId2, duelId, refundInCents, currency, isPlayMoney, isTicket
	// PYR-36593 // PYR-53789 - obsolete
	//eEmailDuelHistory = 188,					// s14<4tt8s4<s4s4>[M()]> - userId, requestType, numOfDuels<duelId,startTime,endTime,buyIn,duelName,numOfParticipants<userId,score,country,clientChatLastOrdinal>,[M()]>
	// PYR-38321
	eEmailFirstLogin = 189,						// s44 - userId,platformId,brand
	// PYR-38100 THE FOLLOWING 2 EMAIL TYPES ARE USED INTERNALLY BY TEMPLATE SERVER
	eEmailSmsPwdReset = 190,					// s4 - userId,subType
	eEmailSmsValidation = 191,					// s4 - userId,subType
	// PYR-38762
	eEmailAAMSSessionError = 192,
	// PYR-41364
	eEmailFullTiltMigration = 193,				// ss4<44s>t444448s44 - fulltiltUserId,userId,numLimits<limitType,limitValue,limitCurrency>,until,usdBalance,eurBalance,usdTChips,eurTChips,fpp,pmBalance,fullTiltEmail,actualLocale,timeZone
	// PYR-27972
	eEmailLoginNotification = 194,				//sst	- userId, ip, login time
	// PYR-37603 & PYR-41017
	eEmailC2LResetPwd = 195,					// 4<ss>s[ss44ss] - numOfUsers,<userId,token>,expiry[,fullTilUserId,token,siteId,emLocale,email,country]
	// PYR-40427
	eEmailTheDealPrizeWinner = 196,				// ss4 - userId,resourceId,brandId
	// PYR-40428
	eEmailTheDealJackpotWinner = 197,			// s48s4 - userId,jackPortPercentage,jackPotAmount,currency,brandId
	eEmailTheDealJackpotShareWinner = 198,		// s8s4 - userId,jackPotShareAmount,currency,brandId
	// PYR-41760
	eEmailAuditFppReport = 199,					// stts - userId,startTime,endTime,reportUrl
	// PYR-47874
	eEmailAuditW2GReport = 200,					// stts - userId,startTime,endTime,reportUrl
	// PYR-45757
	eEmailHandHistoryNew = 201,					// sm44 - userId,handHistRequest,locale,brandId - reply: 2s - errCode[errDescr|userEmail]
	//	PYR-47770
	eMailCzLimitNew = 202,						// s4<4444> - userId, numLimits<hourLimit, daysLimit, limitType, limitAmount>
	//	PYR-51853, PYR-60673
	eMailOneTimePIN = 203,						//	ss4 - userId, PIN, reasonCode
	// PYR-73312
	eEmailVanillaDirectBarcode = 204,			// s4s4s - userId, amount, currency, convertedAmount, convertedCurrency
	// PYR-51654
	eEmailPayPalRefundedWithdrawal = 205,		// s44st4 - userId,amount,refund,currency,time,transId
	// PYR-77762
	eEmailNjTaxDeductionToUser = 206,			// s4st4s - userId,amount,amountCurrency,when,taxes,taxCurrency
	// PYR-77762
	eEmailNjTaxDeductionNotEnoughFunds = 207,	// s4sts4s - userId,amount,amountCurrency,when,gameType,taxDiff,taxDiffCurrency
	// PYR-77762
	eEmailNjTaxDeductionUnableToReturn = 208,	// s4sts4 - userId,amount,amountCurrency,when,gameType
	// PYR-77762
	eEmailNjTaxDeductionCancelledBet = 209,		// s4s8 - userId,amount,amountCurrency,betId
	// PYR-106798
	eEmailSideBetLimitNew = 210,				//s4444 - userId, hourLimit, daysLimit, limitType, limitAmount_notUsed
	eEmailSideBetLimitApproved = 211,			//s - userId
	// PYR-106819
	eEmailAuditMGSideBetReport = 212,			// stts - userId,startTime,endTime,reportUrl

	//^insert here^
	//only generic below
	//////////////////////////////////////////
	//by template id, for everybody
	eEmailByTemplateId = 1000000,					//4s14<s>[4] - templateId,userId,fromEmail,numParams,<params>[,brandId]
	eEmailByTemplateCode = 1000001,					//ss14<s> - templateCode,userId,fromEmail,numParams,<params>
	eEmailByTemplateName = 1000002,					//ss14<s>[4] - templateName,userId,fromEmail,numParams,<params>[,brandId]

	eEmailByTemplateIdGen = 1000003,				//4ss4144<s> - templateId,userId,userEmail,locale,fromEmail,licenseId,numParams,<params>
	eEmailByTemplateCodeGen = 1000004,				//sss4144<s> - templateCode,userId,userEmail,locale,fromEmail,licenseId,numParams,<params>
	eEmailByTemplateNameGen = 1000005,				//sss4144<s> - templateName,userId,userEmail,locale,fromEmail,licenseId,numParams,<params>

	eEmailByTemplateIdWithAttachments = 1000006,	//4s14<s>4<sB>[4[4]] - templateId,userId,fromEmail,numParams,<params>,numAtt<fileName,attachments>[,locale[,brandId]]
	eEmailByTemplateIdVipStore = 1000007,			//4s1444<s>	- templateId, userId, fromEmail, itemId, itemSpecialType, numParams, <params> PYR-24493
	eEmailByTemplateNameNoUser = 1000008,			//ss14<s>444ss[4] - templateName,userId,fromEmail,numParams,<params>,emLocale,siteId,loginBrandId,country,email[,regBrand]
	eEmailByTemplateNameWithAttachments = 1000009,	//ss14<s>4<sB>[4[4]] - templateName,userId,fromEmail,numParams,<params>,numAtt<fileName,attachments>[,locale[,brandId]] PYR-31767
	eEmailByTemplateIdWithAttachmentsByUserIntId = 1000010,	//4414<s>4<sB>[4[4]] - templateId,userIntId,fromEmail,numParams,<params>,numAtt<fileName,attachments>[,locale[,brandId]]
	eEmailByTemplateNameWithAttachmentsByUserIntId = 1000011,	//s414<s>4<sB>[4[4]] - templateName,userIntId,fromEmail,numParams,<params>,numAtt<fileName,attachments>[,locale[,brandId]] PYR-31767
	//////////////////////////////////////////
	//no append here!
};

// PYR-14242
enum eSmsType
{
    eSmsGeneral         = 0,                //ssss - userId,body,destination#,origin#
    eSmsActivationCode  = 1,                //ss44sbb - userId,destination#,code,locale,country,initialValidation,fromAdmin
	eSMSOtp				= 2,				//ss44sb - userId,destination#,code,locale,country,fromAdmin //PYR-16777
	eSMSPin				= 3,				//ss44sb - userId,destination#,code,locale,country,fromAdmin //PYR-16777
	eSMSPwd				= 4,				//ss44sb - userId,destination#,code,locale,country,fromAdmin //PYR-22184
	eInstallIdSmsCode	= 5,				//ss44sbb - userId,destination#,code,locale,country,initialValidation,fromAdmin // PYR-21829
};

enum eTypeSmsOrig
{
    eTypeSmsOrigGeneric         = 0,
    eTypeSmsOrigVerification    = 1,
};

enum eCloneType	// PYR-23002
{
	eClone = 0,
	eEditCopy = 1,
};

typedef enum 
{ // PYR-24334 verbatim copy from autochequeobject.h
	eWaybillFedex = 0, // fedex shipping
	eWaybillInterlink = 1, // interlink
	eWaybillFastway = 2, // fastway
	eWaybillUPS = 3, // ups
	eWaybillTNT = 4, // TNT
	eWaybillGLS = 5, // GLS
	eWaybillUnknown = 6
} ECourier;

// PYR-24539
enum eRecInfoType
{
	eRecInfoDefault = 0, // desktop
	eRecInfoWebCashier = 1,
};

enum eLobbyFlyerType	// PYR-30651, shared between client and Admin.
{
	eLobbyFlyerTypeCashGameRealMoneyFlyers = (1 << 0),
	eLobbyFlyerTypeCashGamePlayMoneyFlyers = (1 << 1),
	eLobbyFlyerTypeZoomRealMoneyFlyers = (1 << 2),
	eLobbyFlyerTypeZoomPlayMoneyFlyers = (1 << 3),
	eLobbyFlyerTypeSnGRealMoneyFlyers = (1 << 4),
	eLobbyFlyerTypeSnGPlayMoneyFlyers = (1 << 5),
	eLobbyFlyerTypeLiveEventRealMoneyFlyers = (1 << 6),
	eLobbyFlyerTypeLiveEventPlayMoneyFlyers = (1 << 7),
	eLobbyFlyerTypeTournRealMoneyFlyers = (1 << 8),
	eLobbyFlyerTypeTournPlayMoneyFlyers = (1 << 9),
	eLobbyFlyerTypeTournFavoriteRealMoneyFlyers = (1 << 10),
	eLobbyFlyerTypeTournFavoritePlayMoneyFlyers = (1 << 11),
	eLobbyFlyerTypeOnlineEventPlayMoneyFlyers = (1 << 12),
};


#endif // templateserver_h_included
