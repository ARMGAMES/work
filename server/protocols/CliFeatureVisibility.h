//------------------------------------------------------------------------------
//	CliFeatureVisibility.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2016 PYR Software Ltd. All rights reserved. This document
//	is for the use of licensed users only and is subject to the terms and
//	conditions of the accompanying and/or applicable license between you and
//	PYR Software Ltd, including without limitation those terms and conditions
//	prohibiting unauthorized translation, reverse engineering, decompilation,
//	disassembly, and other inappropriate use of any software provided. Any
//	unauthorized use, copying, modification, distribution, publication or
//	disclosure of information is a violation of copyright laws. No reproduction
//	in whole or in part of this document may be made without express written
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#if !defined( __CliFeatureVisibility_h_included__ )
#define __CliFeatureVisibility_h_included__

//------------------------------------------------------------------------------

#define eSocialMediaUser						( ONE64 <<  0 )
#define eViewLinkCode							( ONE64 <<  1 )
#define eFullTiltLink							( ONE64 <<  2 )
#define eFullTiltTransfer						( ONE64 <<  3 )
#define eFutureStarCode							( ONE64 <<  4 )
#define eNoSupernovaElite						( ONE64 <<  5 )	// PYR-38773
#define eHallOfFameLevels						( ONE64 <<  6 )	// PYR-23062
#define eRealMoneyFormDepositAssist				( ONE64 <<  7 )	// PYR-30039
#define eBrowserEmbedded						( ONE64 <<  8 )	// PYR-24878
#define eCashierBrowserEmbedded					( ONE64 <<  9 )	// PYR-24878
#define eFreemium								( ONE64 << 10 )	// PYR-26857
#define eFreemiumMobile							( ONE64 << 11 )	// PYR-26857
#define eNetverifyAllowAlternative				( ONE64 << 12 )	// PYR-27931
#define eCashierCloseDepositAssist				( ONE64 << 13 )	// PYR-28038
#define eAppCloseDepositAssist					( ONE64 << 14 )	// PYR-28038
#define eFreerollDepositAssist					( ONE64 << 15 )	// PYR-28038
#define eWebContactSupport						( ONE64 << 16 )	// PYR-21440
#define eLobbyEventTabSupport					( ONE64 << 17 )
//#define eGuestAccountEnabled					( ONE64 << 18 )	// PYR-28588 obsolete by PYR-91342
#define eShowAffiliateTrackOnCreate				( ONE64 << 19 )	// PYR-28007
#define eLoginViaEmailEnabled					( ONE64 << 20 )	// PYR-28548
#define eWebTokenCustomerSupportBrowserEmbedded	( ONE64 << 21 )	// PYR-21440 PYR-29212
#define eDisplayClientDevice					( ONE64 << 22 )	// PYR-27126
#define eWebTokenFreemiumBrowserEmbedded		( ONE64 << 23 )	// PYR-27555
#define eHelpSupportPortal						( ONE64 << 24 )	// PYR-28315
#define eMobileAdXSupport						( ONE64 << 25 )	// PYR-28829
#define eExchangeRateLabel						( ONE64 << 26 )	// PYR-29045
#define eSpinAndGo								( ONE64 << 27 )	// PYR-28752
//#define eInClientPasswordReset					( ONE64 << 28 )	// obsolete with PYR-52410
#define eIceFeatureEnabled						( ONE64 << 29 )	// PYR-30268
#define eCasinoNonAuth							( ONE64 << 30 )	// PYR-29533
#define eCasinoAuth								( ONE64 << 31 )	// PYR-29533
#define eCasinoLimits							( ONE64 << 32 )	// PYR-29581
#define eMobileV2Support						( ONE64 << 33 )	// PYR-29579
#define ePlayFulltiltPoker						( ONE64 << 34 )	// PYR-30114
#define eGeoAssist								( ONE64 << 35 )
#define eDeclineDepositAssist					( ONE64 << 36 )	// PYR-30985
#define eSpinGoNewFlagShown						( ONE64 << 37 )	// PYR-31618
#define eRealMoneyFormCloseDepositAssist		( ONE64 << 38 )	// PYR-32416
#define eLandingScreenTest						( ONE64 << 39 )	// PYR-32613
#define eSportsBookNonAuth						( ONE64 << 40 )	// PYR-32578
#define eSportsBookAuth							( ONE64 << 41 )	// PYR-32578
#define eSportsBookLimits						( ONE64 << 42 )	// PYR-32578
#define eSportsBookOptions						( ONE64 << 43 )	// PYR-32734
#define eCashierDepositAssist					( ONE64 << 44 )	// PYR-32704
#define eCashierTimeoutDepositAssist			( ONE64 << 45 )	// PYR-33064
#define eClientTimeoutDepositAssist				( ONE64 << 46 )	// PYR-33064
#define eSportsBookInBrowser					( ONE64 << 47 )	// PYR-32578
#define eVIPStoreMobile							( ONE64 << 48 )	// PYR-28871
#define eCasinoLiveGames						( ONE64 << 49 )	// PYR-33609
#define eIceLeaderboard							( ONE64 << 50 ) // PYR-33416
#define eShowSentForProcessingNoRMOK			( ONE64 << 51 ) // PYR-33814
#define eFrRequireIBAN							( ONE64 << 52 )	// PYR-33448
#define eInstantBonuses							( ONE64 << 53 )	// PYR-31838
#define eBotpLeaderBoard						( ONE64 << 54 )	// PYR-34826
#define e3DSecureBrowserEmbedded				( ONE64 << 55 )	// PYR-34066
#define eFppBundles								( ONE64 << 56 )	// PYR-32027
#define eSendDocumentsVisible					( ONE64 << 57 )	// PYR-34619
#define eDFSNonAuth								( ONE64 << 58 )	// PYR-36466
#define eDFSAuth								( ONE64 << 59 )	// PYR-36466
#define eSpainVault2							( ONE64 << 60 )	// PYR-36688 - can be reused in Y2016
#define eWebRealMoneyForm						( ONE64 << 61 )	// PYR-36624
#define ePokerDuelApp							( ONE64 << 62 )	// PYR-37062 // PYR-53789 - obsolete
#define eAccountCreationDisabled				( ONE64 << 63 )	// PYR-36811

#define ePokerDuelCrashlytics					( ONE64 <<  0 )	// PYR-38910 // PYR-53789 - obsolete
#define eKOTabQSSNK								( ONE64 <<  1 )	// PYR-42013
#define eKOTabsDefaultLoad						( ONE64 <<  2 )	// PYR-39944
#define eWebCashier2							( ONE64 <<  3 )	// PYR-40292
#define eLiveChatSupport						( ONE64 <<  4 )	// PYR-39787
#define eKOTabQSScheduled						( ONE64 <<  5 )	// PYR-42013
#define eKOTabAGSNK								( ONE64 <<  6 )	// PYR-42013
#define eKOTabAGScheduled						( ONE64 <<  7 )	// PYR-42013
#define eWebPasswordReset						( ONE64 <<  8 ) // PYR-41017
#define eWebCashier2deposit						( ONE64 <<  9 )	// PYR-40292
#define eZoomTabDisable							( ONE64 << 10 )	// PYR-43016
#define eVegasNonAuth							( ONE64 << 11 )	// PYR-42295
#define eVegasAuth								( ONE64 << 12 )	// PYR-42295
#define eEasyLobby								( ONE64 << 13 )	// PYR-40531
#define eBTCTabQSSNG							( ONE64 << 14 )	// PYR-43017
#define eBTCTabQSMTT							( ONE64 << 15 )	// PYR-43017
#define eBTCTabAllSNG							( ONE64 << 16 )	// PYR-43017
#define eBTCTabAllMTT							( ONE64 << 17 )	// PYR-43017
#define eBTCTabsDefaultLoad						( ONE64 << 18 )	// PYR-43017
#define ePublicLiveChatSupport					( ONE64 << 19 )	// PYR-45817
#define eFreeSpinsBonuses						( ONE64 << 20 )	// PYR-44841
#define eMiniGameTickets						( ONE64 << 21 )	// PYR-46074
#define eCvl									( ONE64 << 22 )	// PYR-45659
#define eSpinAndGoOmaha							( ONE64 << 23 )	// PYR-50211
#define eSpinAndGoMax							( ONE64 << 24 )	// PYR-51311
#define ePowerUpNewFlagShown					( ONE64 << 25 )	// PYR-52434 
#define eTempPinPerLogin						( ONE64 << 26 ) // PYR-76895
// #define eSkybetSportsTab						( ONE64 << 27 ) // PYR-92704 - Can be reused in the future. Business' decision.
#define eAllInCashOut							( ONE64 << 28 ) // PYR-79935

//------------------------------------------------------------------------------
#endif // !defined( __CliFeatureVisibility_h_included__ )
//------------------------------------------------------------------------------
