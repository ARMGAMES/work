// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//dbm.h
#ifndef	dbm_h_included
#define	dbm_h_included

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//                All dbm messages are moved to dbmmsg.h               //
//   The last non-separated version is in tag BUILD989 (ver. 1.3089)   //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "gamedefs.h"
#include "dbmmsg.h"
#include "ppincludebase.h"

// HandType enum moved to gamedefs.h

enum HandFlags ///bitmask
{
	hFlagDoNotSave = 1,
	hFlagPlayMoney = 1 << 1, //2
	hFlagRITCheckPerformed = 1 << 2,  //4   
	hFlagRunItTwice = 1 << 3,  //8   
	hFlagBlitzModeTourn = 1 << 4,  //16
	hFlagNStackTournHand = 1 << 5,  //32
	hFlagSagTournHand = 1 << 6,  //64
	hFlagZoomTourn = 1 << 7,  //128   // for both blitz mode and normal playing mode
	hFlagStormMana = 1 << 8,  //256  // PYR-36799 - for indicating Storm hands with mana cost
	hFlagStorm = 1 << 9, //512 // PYR-38782 - for indicating that the hand is a Storm hand.
	hFlagDealerButtonWonFromLastHand = 1 << 10,  //1024 // PYR-32587 win-the-button tournament
	hFlagProgressiveKnockoutTourn = 1 << 11,  //2048 // PYR-45395 progressive knockout tournament
	hFlagButtonBlind = 1 << 12, //4096 // PYR-74693
	hFlagGiantBlind = 1 << 13,  //8192 // PYR-82834
	hFlagCashoutOffered = 1 << 14,  //16384 // PYR-95107
	hFlagStealthTable = 1 << 15, //32768 // PYR-106120
};

enum HandSaveMode
{
	eSaveFull,
	eSaveChatOnly,
	eSaveNone
};

enum UserTrackCodeDB
{
	crmDB = 0,
	rootDB
};

enum TrackCodeStatus
{
	PASSIVE_TRACKCODE = 0,
	ACTIVE_TRACKCODE  = 1
};

//	PYR-25971
enum TrackCodeFlags
{
	TRACKCODEFLAG_LOCKED		= 1 << 0, //1
	TRACKCODEFLAG_ICE_ISSUABLE	= 1 << 1, //2
};

enum UserPrivileges //bitmask
{
	privPlay			= 1 << 0, //1
	privChat			= 1 << 1, //2,
	privShowImage		= 1 << 2, //4,
	privChangeImage		= 1 << 3, // 8,
	privTimeoutAllIn	= 1 << 4, //16,
	privNoMailList		= 1 << 5, //32, for details see priv3NoMailListPoker, priv3NoMailListCasino and priv3NoMailListSports
	privDontShowWhere	= 1 << 6, //64,
	privAdmin			= 1 << 7, //128,
	privDuplicate		= 1 << 8, //256,
	privBonusAfterCashout	= 1 << 9, //512,
	privChatMask		= (1 << 10) | (1 << 11) | (1 << 12), //1024 | 2048 | 4096,
	privVip				= 1 << 13, //8192,
	privEarlyCashout	= 1 << 14, //16384,
	privNoAutoBan		= 1 << 15, //32768,
	privHighRoller		= 1 << 16, //65536,
	privTransferVerified = 1 << 17, //131072,
	privHidePersonalInfo = 1 << 18, //262144,
	privRollbackOverride = 1 << 19, //524288,
	privRequestTourn	 = 1 << 20, //1048576,
	privNoCityChange	 = 1 << 21, // 2097152
	privTMoneyTrader	 = 1 << 22,  // 4194304
	privVipTest			 = 1 << 23,  // 8388608
	privSuperHighRoller	 = 1 << 24, // 16777216
	privDoNotSendTournWinnerEmail = 1 << 25, //33554432
	privSelfExcluded	= 1 << 26, //67108864
	privTestCashier	    = 1 << 27, //134,217,728 // #14848 also .FR tester for cashflow manipulations
	privInstaClearCashout = 1 << 28, //268,435,456
	privNotSpendUncleared = 1 << 29, //536,870,912
	privBypassVelocityControl = 1 << 30, //1,073,741,824
	privNoBanByCountry = 1 << 31, //lint !e648 (int overflow) 2,147,483,648
};

// User privileges2 (UINT64)
#define priv2HideUserInfo				(ONE64 << 0)
#define priv2NotQualifiedFirstTransfer	(ONE64 << 1)
#define priv2ShowVipStatusToOthers		(ONE64 << 2)
#define priv2CannotSendTransfer			(ONE64 << 3)
#define priv2CannotReceiveTransfer		(ONE64 << 4)
#define priv2IgnoreMicroTransfers		(ONE64 << 5)
#define priv2Reserved6					(ONE64 << 6) // was: priv2RsaLoginAllowed (RSA token login method allowed for this user)
#define priv2Reserved7					(ONE64 << 7) // was: priv2RsaLoginAllowed (PokerStars PIN login method allowed for this user)
#define priv2TransferAffiliate			(ONE64 << 8) // obsolete #16374 Can be reused when cleared for all users (TA account should not be allowed to receive transfers from other players. #10656)
#define priv2PSRelated					(ONE64 << 9) // Flag PS and related companies employees accounts for internal auditing. #9865
#define priv2CanRegisterMCTourns		(ONE64 << 10)// Can be register in tournaments with different currency
#define priv2CannotAddCurrencyAccount	(ONE64 << 11)// User cannot add a new currency account
#define priv2MultiCurrBetaTester		(ONE64 << 12)// User is beta-tester for MC
#define priv2AffiliateDoNotPay			(ONE64 << 13)// Affiliate is not paid for this user
#define priv2TeamPro					(ONE64 << 14)// PS TeamPro member
#define priv2NoSelfPLI					(ONE64 << 15)// user is not eligible for self PLI
#define priv2PermanentClosuse           (ONE64 << 16)// user requested permanent closure of the account
#define priv2ExcludeStellarReward		(ONE64 << 17) // user is excluded from Stellar Bonus rewards
#define priv2OneTimeBypassConvLimits	(ONE64 << 18) // #13713: One Time Bypass of inter-account transfer of the conversion limits
#define priv2HeadsUpTablesNotAllowed	(ONE64 << 19) // #14108: Gives PokerStars support the ability to restrict players who abuse the system from playing in Heads Up games without restricting them in other game types.
#define priv2ShowNoChatTooltip			(ONE64 << 20) // #15760: If chat revoked show "No Chat" tooltip
#define priv2NoHighRiskRestriction      (ONE64 << 21) // #15596: Don't apply high-risk PS restriction
#define priv2AlwaysHighRiskRestriction  (ONE64 << 22) // #15596: Always apply high-risk PS restriction
#define priv2ExemptFrom3DSecure         (ONE64 << 23) // #16088: exempt from 3D Secure verification
#define priv2BypassIpBlocking			(ONE64 << 24) // #16614: bypass IP based blocking
#define priv2BetaTester					(ONE64 << 25) // #16940: Global Beta Tester Privilege
#define priv2CannotJoinHGClub			(ONE64 << 26) // #16182: Block Player from Joining HG Clubs
#define priv2CannotCreateHGClub			(ONE64 << 27) // #16182: Block Player from Creating HG Clubs
#define priv2HGShadowFounder			(ONE64 << 28) // #16182: Shadow Founder Privilege
#define priv2HGNoEmails					(ONE64 << 29) // #16182: No club emails
#define priv2SafeDepositMode			(ONE64 << 30) // #14415: A user can deposit being cash banned
#define priv2NoTableLimitIncrease       (ONE64 << 31) // #15894: player cannot increase their table limits
#define priv2NoTournLimitIncrease       (ONE64 << 32) // #15894: player cannot increase their tournament limits
#define priv2NoDepositLimitIncrease     (ONE64 << 33) // #17417: player cannot increase their deposit limits
#define priv2MobileMarketingMessages    (ONE64 << 34) // #14242: player accepts Mobile Marketing messages (PYR-25063) - OBSOLETE privilege
#define priv2NoFastDeposit				(ONE64 << 35) // #18174: player cannot use 'fast deposit'
#define priv2LinkEmailToAccount			(ONE64 << 36) // #16703: Link email address to one account in case of several accounts with same e-mail
#define priv2ChatMask					((ONE64 << 37) | (ONE64 << 38) | (ONE64 << 39))  // #18548
#define priv2UserGlobalMute				(ONE64 << 40) // #18513: voice chat support - user prohibited from using voice
#define priv2DontShowPersonalInfo       (ONE64 << 41) // #21287: don't show personal info to other players in the client
//#define priv2Obsolete					(ONE64 << 42) // deprecated PYR-97488 - available for reuse
#define priv2NoPotLimitOmahaCashGames	(ONE64 << 43) // #20119 don't allow player join Pot Limit Omaha cash games
#define priv2NoZoomGames				(ONE64 << 44) // #20119 don't allow player join Zoom games
#define priv2NoHeadsUpSitNGo            (ONE64 << 45) // #20119 don't allow player join Heads Up Sit and Go games.
#define priv2NoHyperTurboSitNGo         (ONE64 << 46) // #20119 don't allow player join Hyper/Turbo Sit and Go games.
#define priv2SuspendRM         			(ONE64 << 47) // PYR-20489 blocks players from doing any RM activity.
#define priv2NoSMSPasswordReset			(ONE64 << 48) // PYR-22184 no SMS password reset
#define priv2NoAutomatedUnregister		(ONE64 << 49) // PYR-20055: no automatic unregistering from Sit&Goes on client disconnect
#define priv2HideMobileIconToOthers		(ONE64 << 50) // PYR-27126: hide mobile icon for user
#define priv2BizAccount					(ONE64 << 51) // PYR-28864: internal business account
#define priv2NoAutoImageChange			(ONE64 << 52) // PYR-26665 "Change Image" automatic permissions
#define priv2AffiliateDNPP				(ONE64 << 53) // PYR-28993: do not pay affiliate for player's poker activity
#define priv2AffiliateDNPC				(ONE64 << 54) // PYR-28993: do not pay affiliate for player's casino activity
#define priv2NoLiveAssistance			(ONE64 << 55) // PYR-29934: no live support via chat
#define priv2NoSagTourns				(ONE64 << 56) // PYR-30225: don't allow player join SpinAndGo tournaments
#define priv2NoCasinoLimitIncrease      (ONE64 << 57) // PYR-29581: player cannot increase their casino limits
#define priv2NoCasinoPlay               (ONE64 << 58) // PYR-29782: casino play not allowed
#define priv2NoPwdChangeReminder        (ONE64 << 59) // PYR-30480: don't remind the player to change the password
#define priv2NoSetupCQPrompt			(ONE64 << 60) // PYR-28732: don't prompt the player to setup their Challenge Questions
#define priv2NjTesting					(ONE64 << 61) // PYR-31299: allow business to do RM testing for NJ
#define priv2ChatTempBannedByACM		(ONE64 << 62) // PYR-28673 chat banned by ACM for a short period
// IMPORTANT: do not use the 63 bit, it will cause overflow in DB2, add to privilege3 instead
// #TODO #sergeysh - privXCashierDynamicProps constants are used by client to identify privileges that might be overwritten
// by cashier (be different from ones in DB). This legacy code in client should be removed and then constants can be removed as well
#define priv2LastUsedBit                62 // please keep up-to-date
#define priv2CashierDynamicProps		(priv2CannotAddCurrencyAccount | priv2NoSelfPLI | priv2SafeDepositMode)
#define privCashierDynamicProps			0

enum UserPrivilegesChat
{
	privChatDefault		= 0,
	privChatRoomManager = 1024 * 1,
	privChatSupport		= 1024 * 2,
	privChatSupervisor 	= 1024 * 3,
	privChatCommentator	= 1024 * 4,
	privChatModerator	= 1024 * 5,
	privChatTeamPro		= 1024 * 6,
	privChatTeamOnline	= 1024 * 7
	//	No more chat privileges can be added here!	
};

// UserPrivileges2Chat
#define	priv2ChatDefault			0
#define priv2ChatCopyAndPaste		((ONE64 << 37 ) * 1)  // #18548: chat copy+paste privilege
// Add new chat privileges here
// maximum privilege ( ONE64 << 37 ) * 7

// add privileges3 here
// User privileges3 (UINT64)
// When privNoMailList is on all 3 privileges below are on. These privileges used for mass emails, news and push notifications
#define priv3NoMailListPoker		(ONE64 << 0) // = 1   PYR-27034
#define priv3NoMailListCasino		(ONE64 << 1) // = 2   PYR-27034
#define priv3NoMailListSports		(ONE64 << 2) // = 4   PYR-27034
#define priv3NoSportsLimitIncrease  (ONE64 << 3) // = 8   PYR-32180: player is not allowed to increase their Sports Stake Limit
#define priv3NoSportsPlay           (ONE64 << 4) // = 16  PYR-32180: player is not allowed to place Sportsbook bets (same effect as Sports Stake Limit set to zero)
#define priv3NoHTML5Client			(ONE64 << 5) // = 32  PYR-33221: user not allowed to use HTML5 client
#define priv3NoLeaderBoard			(ONE64 << 6) // = 64  PYR-33586: user opt out leader board
#define priv3NoSurveyInvites		(ONE64 << 7) // = 128 PYR-34345: user opt out cusSat survey
#define priv3FppBundles				(ONE64 << 8) // = 256 PYR-32027: Fpp Bundles
#define priv3CasinoVIP				(ONE64 << 9) // = 512 PYR-35568: Casino VIP attribute manually managed by CRM
#define priv3NoDFSPlay				(ONE64 << 10) // = 1024 PYR-36470 disabling Daily Fantasy Sports
// PYR-54814 - these are available for reuse since they don't exist anywhere in DB
//#define priv3AvailableForReuse		(ONE64 << 11) // = 2048
//#define priv3AvailableForReuse        (ONE64 << 12) // = 4096
#define priv3NoCashoutCancellation	(ONE64 << 13) // = 8192 PYR-28773 prevent players from cancellation of cash-outs for Responsible Gaming issues
#define priv3SportsVip				(ONE64 << 14) // = 16,384 // PYR-36335 Sports VIP attribute manually managed by CRM
#define priv3NoSuggestedDeposit		(ONE64 << 15) // = 32768 // PYR-31386 No Suggested Deposit attribute
#define priv3NoUpsell				(ONE64 << 16) // = 65536 // PYR-31386 No Upsell attribute
#define priv3NoSportsBonus			(ONE64 << 17) // = 131072 PYR-39041: Used by UNICA for excluding bonus-abusers from receiving Sports bonuses
#define priv3PokerPremiumPlayer		(ONE64 << 18) // = 262144 // PYR-39618
#define priv3CasinoPremiumPlayer	(ONE64 << 19) // = 524288 // PYR-39618
#define priv3SportsPremiumPlayer	(ONE64 << 20) // = 1048576 // PYR-39618
#define priv3BadBeatCutoff			(ONE64 << 21) // = 2097152 // PYR-40656
#define priv3NoKOTourns          (ONE64 << 22) // = 4194304 // PYR-43888
#define priv3PMPremiumPlayer		(ONE64 << 23) // = 8388608 // PYR-47106
#define priv3BypassNAC2				(ONE64 << 24) // = 16777216 // PYR-33529 // bypass Biz NAC (NAC2) for the player
#define priv3BlockPMPurchase		(ONE64 << 25) // = 33554432 // PYR-41219
#define priv3DoNotPayBonus			(ONE64 << 26) // = 67108864 // PYR-50956
#define priv3BypassMAAAutoActions	(ONE64 << 27) // = 134217728 // PYR-56097
#define priv3NoPowerUpPlay			(ONE64 << 28) // = 268435456 // PYR-45178: power up play not allowed
#define priv3IncidentReportFiled    (ONE64 << 29) // = 536870912 // PYR-48917
#define priv3NoSportLiveAlert		(ONE64 << 30) // = 1073741824 // PYR-60282
#define priv3NoUnfoldCashGames		(ONE64 << 31) // = 2147483648 // PYR-70870
#define priv3BypassOTP				(ONE64 << 32) // = 4294967296 // PYR-86272: Bypass One Time Pin assignment.
#define priv3NoAllInCashout			(ONE64 << 33) // = 8589934592 // PYR-79935: prevent player from cashing out during all-in scenarios
#define priv3NoSideBets             (ONE64 << 34) // = 17179869184 // PYR-106798
#define priv3PspcDisplay            (ONE64 << 35) // = 34359738368 // PYR-118097

// IMPORTANT: do not use the 63 bit, it will cause overflow in DB2, add to privilege4 instead
// #TODO #sergeysh - privXCashierDynamicProps constants are used by client to identify privileges that might be overwritten
// by cashier (be different from ones in DB). This legacy code in client should be removed and then constants can be removed as well
#define priv3LastUsedBit            34 // please keep up-to-date
#define priv3CashierDynamicProps	0

const UINT64 priv3NoMailListAll = (priv3NoMailListPoker | priv3NoMailListCasino | priv3NoMailListSports | priv3NoSportLiveAlert);

enum vipCounterType 
{
	eMonthly = 0,
	eYearly = 1
};

enum BonusFlags //bitmask
{
	firstTimeDeposit      = 1,
	notAllowCashout	      = 1 << 1,
	forFPPStoreOnly       = 1 << 2,
	rakebackDeductable    = 1 << 3,
	tCurrencyBonus        = 1 << 4, // TODO consider commenting out, obsolete after PYR-27692
	gradualRelease        = 1 << 5,
	gradualDepositAllowed = 1 << 6,
	bonusFlagTTicketOnly  = 1 << 7, // #18338
	notAllowFullTiltTransferOut = 1 << 8, // PYR-26443
	countryListIsForbidden = (1 << 9), // PYR-29406: 1 - Black List (forbidden), 0 - White List (allowed)
	
	// PYR-32329, 2 next flags have complicated meanings
	// Refer to functions Bonus::composeClearedWithFlags, Bonus::isClearedWithCPP, Bonus::isClearedWithVPP
	// in order to see how to use them
	clearedWithCPPAndVPPCombination1 = 1 << 10, // PYR-32329
	clearedWithCPPAndVPPCombination2 = 1 << 11, // PYR-32329

	clearedWithSPP        = 1 << 12, // PYR-32329
	eBonusFlag_WasIssued  = 1 << 13, // PYR-36462
	eBonusFlag_AdminOnly  = 1 << 14, // PYR-36462

	eBonusFlag_NeverPlayedRMCasino = 1 << 15, // PYR-45068
	eBonusFlag_PaysystemMaskIsForbidden = 1 << 16, // PYR-45523: 1 - Black List (forbidden), 0 - White List (allowed)
	eBonusFlag_NeverPlayedRMSport = 1 << 17, // PYR-48545
	eBonusFlag_SupressEmails = 1 << 18, // PYR-30576
	eBonusFlag_SupressPopups = 1 << 19, // PYR-30576
};

#define BONUS_ALL_LICENSES 0xFFFFFFFF

enum UserFlags //bitmask
{
	emailVerified		=	1 << 0, // 1
	nickNameChecked		=	1 << 1, // 2
	imageChecked		=	1 << 2, // 4
	realMoneyOk			=	1 << 3, // 8

	cashBanned			=	1 << 4, // 16
	accountClosed		=	1 << 5, // 32
	emailBounced		=	1 << 6, // 64
	isAffiliate			=	1 << 7, // 128

	firstDeposit		=	1 << 8, // 256	// Dynamic flag, not stored in DB, there was a deposit if set
	_restrictedRM_available_to_use		=	1 << 9, // 512 2017-10-20 not in code, not in DB, free to use
	barredForReturn		=	1 << 10, // 1024
	hasRelated			=	1 << 11, // 2048

	ageVerified			=	1 << 12, // 4096
	usedSelfExclusion	=	1 << 13, // 8192 // ever been self excluded
	hasMrmcAccount		=	1 << 14, // 16384
	hasWireAccount		=	1 << 15, // 32768

	hasIndividualLimits =	1 << 16, // 65536
	hasUnclearedDeposits =	1 << 17, // 131072
	intelliPokerRegistered = 1 << 18, // 262144
	RMPlayerDepositTransfer = 1 << 19, // 524288	// Dynamic flag, not stored in DB

	authVerified		=	1 << 20, // 1048576 // Know Your Customer, user's identity was verified by 3rd party system
    holdCashout         =   1 << 21, // 2097152 // Security alerts - hold cashout if set (#7093, 7094, etc.)
	hasFirstTransfer	=	1 << 22, // 4194304 // Dynamic flag, not stored in DB
	ipAddrIsExempt		=	1 << 23, // 8388608 // Dynamic flag

	hasTableLimit		= 1 << 24, // 16777216
	hasNonUsdAccount	= 1 << 25,  // 33554432
	rsaRequired			= 1 << 26,  // 67,108,864
	inGracePeriod_obsolete		= 1 << 27, //134,217,728 //Dynamic flag, NOT in use after #21414, can be re-used
	pastGracePeriod_obsolete	= 1 << 28, // 268,435,456 //Dynamic flag, NOT in use after #21414, can be re-used
	pspRequired			= 1 << 29, // 536,870,912 // PokerStars PIN is required to log in
	limitedLogin		= 1 << 30, // 1,073,741,824 // Dynamic flag - user logged in in a limited mode
	// Obsolete #11125
	//accountRenamed		= 1 << 31, //  2,147,483,648 // Dynamic flag - prevUserId not empty
};
#define userFlagsLastUsedBit 30 // please keep up-to-date

// bitmask for Users.Flags2, UINT64
#define pspGenerated		(ONE64 << 0) // PokerStars PIN was generated
#define rsaActivated		(ONE64 << 1) // user has activated RSA token
#define accountConverted	(ONE64 << 2) // user account was converted (.COM/.NET to .IT and .FR so far)
#define hasNonPmBalance		(ONE64 << 3) // dynamic flag, should not be used beyond DBM_A_LOGIN_CONFIRM processing. Set to true in DBM_A_LOGIN_CONFIRM if user has balance in any currency, including tChips
#define autoConvertBuyIn	(ONE64 << 4) // User prefers to convert required buy-in funds automatically
#define autoConvertWinnings	(ONE64 << 5) // User prefers to convert winnings to default currency account
#define hasSuperRelated		(ONE64 << 6) // User belongs to "super related" group
#define hasTournLimit		(ONE64 << 7) // [AO 2010-03-29] #14033
#define needWelcomeEmail	(ONE64 << 8) // PYR-12055, set for new users
#define hasPlayingTimeLimit (ONE64 << 9) // #15250
#define verifiedByRegulator_NEVER_USED (ONE64 << 10) // !!! NOT IN USE !!! NEVER SET on production !!! can be reused // old JV's comment: #PYR-16696: flag to mark whether the user is verified by regulator
#define playingTimeLimitReached (ONE64 << 11) // #15250: Dynamic flag - user logged in but cannot sit at table or register in tournaments
#define pepWatchScreenedAndValid (ONE64 << 12) // #16866: PEP-Watch screening was made AND still VALID (USERPROPERTIES type=31 contains last screening details)
#define homeGamesPlayer 	(ONE64 << 13) // #16182: User is an approved club member
#define playedRMGame 		(ONE64 << 14) // #17032: User participated in RM ring game or RM tournament
#define hasPendingGameLimit	(ONE64 << 15) // #15894: has TABLELIMITS.STATUS=0 (pending) for any limit TABLELIMITS.TYPE
#define mobileVerified      (ONE64 << 16) // #14242: user's mobile phone number is verified
#define hasActiveFastDeposit	(ONE64 << 17) // #18174
#define itKeepRmMoneyOk		(ONE64 << 18) // #19039
#define adminCreatedAcct	(ONE64 << 19) // #16191: user account created by admin, MAA should ignore account with this flag
#define hasRmNokHeldDeposit	(ONE64 << 20) // #19393: user has cashtrans in 'N' status due to no RealMoneyOK flag
#define licenseVersionMask 	((ONE64 << 21) | (ONE64 << 22))  // #19372
#define hasTTicketsTotalCalculated 	(ONE64 << 23) // #19360
#define itSuspendedClosed			(ONE64 << 24) // PYR-18621 Italian user whose account is "closed"
#define hasPendingDepositLimit		(ONE64 << 25) // #19893: has LIMITSAUDIT.STATUS='P'
#define subsequentAccount		(ONE64 << 26) // #18897: account is created from an InstallID or Email(PYR-23655) that has previously been used to create an account
#define userIsArchivedFlag2 	(ONE64 << 27) // #20399: non-active user was moved from OLTP.USERS to PYRLOG.USERS_ARC; this flag should NEVER appear on OLTP
#define bannedByRegulator		(ONE64 << 28) // #20724
#define vppMinimumEarnedInternal	(ONE64 << 29) // #21602: Internal flag used to mark users with yearly VIP statuses who earned required VPP minimum in current month. !!!DON'T use this flag unless cleared by AR or LZ!!!
#define smsValidationForNewInstallId	(ONE64 << 30) // PYR-21829 user requested to send SMS validation code on new install id
#define newInstallIdForSmsDetected		(ONE64 << 31) // PYR-21829 new install id detected for SMS validation - dynamic flag
#define affiliatePaid					(ONE64 << 32) // PYR-23655 affiliate has been paid for this user - used as synoninum to priv2AffiliateDoNotPay (see #23655 for details)
#define hasFullTiltPairedAccount		(ONE64 << 33) // #23771
#define smsMarketingFlag				(ONE64 << 34) // PYR-23394 & PYR-23088 opt-in/out marketing SMSs
#define noSMSPasswordReset				(ONE64 << 35) // PYR-22184
#define guestAccount					(ONE64 << 36) // PYR-25263 //PYR-91342, deprecated 
#define hasDepositAttemptFlag2			(ONE64 << 37) // PYR-22920
#define boughtPMchips					(ONE64 << 38) // PYR-26499 indicates a Freemium player (1st purchase of PM chips)
#define strongAuthReqd					(ONE64 << 39) // PYR-27258 Strong Authentication Required NJ. Set after too many failed login attempts.
#define noLoginEmailNotification		(ONE64 << 40) // PYR-27972
#define hasBuyinLimitEx					(ONE64 << 41) // PYR-27249 NJ has daily, weekly or monthly
#define userHasDummyEmailFlag2			(ONE64 << 42) // PYR-29871
#define hasCasinoGameLimit   			(ONE64 << 43) // PYR-29581
#define playedRMCasinoGame   			(ONE64 << 44) // PYR-29782 casino bet with real cash (not the winning with free bet, not the ROLL betting even with RMC)
#define hasChallengeQuestionsFlag2		(ONE64 << 45) // PYR-28732 Challenge questions have been setup for this account.
#define dormantByAutomationFlag2		(ONE64 << 46) // PYR-28735,PYR-30455,PYR-40493 This flag will be raised by automation server and released by Collusion. Flag renamed as part of PYR-40493.
#define accountRevalidationReqdFlag2	(ONE64 << 47) // PYR-28735 At next login User must proceed account Revalidation. User should be able to choose between Sms, 8 last digit of Credit card or Challenge questions methods. Note that for Challenge questions there is a flag "strongAuthReqd"
#define hasSportsStakeLimit   			(ONE64 << 48) // PYR-32180
#define emailAutoValidatedFlag2			(ONE64 << 49) // PYR-29932 Email automatically verified
#define emailAlwaysValidateFlag2		(ONE64 << 50) // PYR-29932 Email needs to be validated the old way (though validation code)
#define emailUnauthorizedFlag2			(ONE64 << 51) // PYR-29932 Email is reported to be unauthorized by user
#define autoAddressFlag2                (ONE64 << 52) // PYR-33102 Address generated by capscan gateway from user's ZIP
#define hideCasinoGamesFlags2           (ONE64 << 53) // PYR-33086 - not set by default
#define createdFromVirtualMachineFlag2  (ONE64 << 54) // PYR-33232
#define dormantAccountFlag2             (ONE64 << 55) // PYR-33374
// Obsolete PYR-54639 #define progressiveAcctFlag2            (ONE64 << 56) // PYR-34898
#define hasSportsAccumulatedLimitFlag2	(ONE64 << 57) // PYR-34388 7-day accumulated bet limit for .FR and .IT
#define rateMobileAppFlag2				(ONE64 << 58) // PYR-33276 If set, mobile players will be asked to rate app
#define hasPokerDuelLimitFlag2			(ONE64 << 59) // PYR-35875, // PYR-53789 - obsolete
#define dfsFirstUsageFlag2				(ONE64 << 60) // PYR-37510
#define fppOneTime2016IncreaseFlag2		(ONE64 << 61) // PYR-39146
#define nemIdAuthenticated				(ONE64 << 62) // PYR-33675
// DO NOT USE BIT 63 because both DB2 and Java have issues with unsigned types
#define userFlags2LastUsedBit           62            // please keep up-to-date

#define licenseVersionShift  (21) // correspond to licenseVersionMask

// bitmask for Users.Flags3, UINT64
#define ftConversionDoneFlag3		(ONE64 << 0)  // PYR-39878 Full Tilt account was converted into associated PS account
#define ftKYCTrustedUserFlag3		(ONE64 << 1)  // PYR-40514 Full Tilt account had KYC flag at conversion time
#define firstSportsBetUserFlag3		(ONE64 << 2)  // PYR-39679 User has done at least one sports bet
#define hasExtendedRelatedFlag3		(ONE64 << 3) //PYR-44179
#define hasRollbackHistoryCalculatedFlag3 (ONE64 << 4) //PYR-44166 if set then 'hasRollbackHistory' flag contains legitimate value
#define hasRollbackHistoryFlag3           (ONE64 << 5) //PYR-44166 valid only if 'hasRollbackHistoryCalculated' is set. Lazy update only for BE license
#define customerDueDiligenceFlag3         (ONE64 << 6) // PYR-47486 + PYR-47543 + PYR-64085 User must provide certain documents. Implemented for .UK and .AsiaB2B users (search UKCDD in jira)
#define hadValidatedPaymentMethodFlag3    (ONE64 << 7) // PYR-47569 indicates that payment method (instrument) has been once validated for this user
#define hasLossLimitFlag3				  (ONE64 << 8)	//	PYR-47770. CZ, Responsible gaming, player has a loss per day/month limit (poker & casino).
#define hasSpendLimitFlag3				  (ONE64 << 9)	//	PYR-47770. CZ, Responsible gaming, player has a spend limit per day/month (poker & casino).
#define hasSportsLossLimitFlag3			  (ONE64 << 10)	//	PYR-47770. CZ, Responsible gaming, player has a sports loss per day/month limit.
#define hasSportsSpendLimitFlag3		  (ONE64 << 11)	//	PYR-47770. CZ, Responsible gaming, player has a sports spend limit per day/month.
#define hasLoginTotalLimitFlag3			  (ONE64 << 12)	//	PYR-47770. CZ, Responsible gaming, player has a limit on number of logins per month.	
#define cvlOptedInFlag3					  (ONE64 << 13)	//	PYR-45659
#define cvlExcludedFlag3				  (ONE64 << 14)	//	PYR-51534
#define fiscalCodeVerifiedFlag3			  (ONE64 << 15)	//	PYR-53001
#define licenseMobileVerifiedFlag3		  (ONE64 << 16)	//	PYR-54101 means that user ones verified mobile phone, sets only, different from existing mobileVerified (flags2)
#define tempPINFromFailedLoginsFlag3	  (ONE64 << 17) //	PYR-51853 A temporary PIN was generated because of too many failed logins.
#define registeredInRegulatorFlag3		  (ONE64 << 18) //	PYR-54565 .FR player is currently registered in regulator (Arjel), PYR-59300 .UK player passed RMF after 2018.main.02
#define GDPRreadyFlag3					  (ONE64 << 19) //  PYR-58575 player is marked for anonymization
#define tempPINFromNewDeviceFlag3		  (ONE64 << 20) //	PYR-60673 A temporary PIN was generated because of a new device login.
#define KYCCompletedFlag3				  (ONE64 << 21) //	PYR-64085 Know Your Customer Completed, for Asia CCD.
#define fraudulentAccountFlag3			  (ONE64 << 22) //	PYR-69503 Fraudulent Account bit, see eFraudulentAccountReasons below
#define tempPINFromAdminMassAssignFlag3	  (ONE64 << 23) //	PYR-68134 A temporary PIN was generated from the admin Mass Pin Assignment tool
#define generateTempPinOnEachLoginFlag3	  (ONE64 << 24) //  PYR-76895 User's opt-in setting for temp pin on each login.
#define playedRmLiveCasinoGameFlag3		  (ONE64 << 25) //  PYR-78143 "live" subset of playedRMCasinoGame
#define acctCreatedByPerfectPickFlag3	  (ONE64 << 26) //	PYR-90462 User account was created by Perfect Pick (mobile NG app)
#define marketBlackListFlag3			  (ONE64 << 27) //	PYR-96224 DK regulations
#define hasLoginCoolOffLimitFlag3		  (ONE64 << 28)	//	PYR-105467 .CZ, Responsible gaming, player has a limit on interval between sessions.	
#define hasSideBetsLimitFlag3			  (ONE64 << 29) //  PYR-106798
#define security3DSTriggerFlag3			  (ONE64 << 30) //  PYR-104952
#define occupationVerifiedFlag3			  (ONE64 << 31) //  PYR-108884 profession or occupation verified
#define userFlags3LastUsedBit			  31 // please keep up-to-date

#define tempPINFlag3Mask		( tempPINFromFailedLoginsFlag3 | tempPINFromNewDeviceFlag3 | tempPINFromAdminMassAssignFlag3 )		//	PYR-60673

static_assert(userFlagsLastUsedBit < 31, "DO NOT USE BIT 31 in userFlags because both DB2 and Java have issues with unsigned types");
static_assert(userFlags2LastUsedBit < 63, "DO NOT USE BIT 63 in userFlags2 because both DB2 and Java have issues with unsigned types");
static_assert(userFlags3LastUsedBit < 63, "DO NOT USE BIT 63 in userFlags3 because both DB2 and Java have issues with unsigned types");

enum TransactionType
{
// !! NOTE !! when adding new constants to TransactionType ensure that:
// !! NOTE !! 1. validateTransType() is changed (see userbankroll.cpp)
//            2. isExternalMoneyTransType() function includes it into the logic
// !! Also ensure that new values ARE NOT less than 1000!!
// !! NOTE !! New transaction codes must be added to the "New Transact Types" field in JIRA.
// !! NOTE !! All transaction codes are processed by automated script to produce transaction descriptions for Admin and CRM Server.
	eTableBuyIn				=	2000,
	eTableReBuy				=	2010,
	// eTableUnReBuy		=	2011, Obsolete
	// eTableAllocCap		=	2012, Obsolete
	eTableAutoRebuy			=	2013,
	eTableLeave				=	2020,
	eTableForcedLeave       =   2021, // PYR-25229
	// eCleanupLeftAlloc	=	2030, Obsolete
	eCleanupObsoleteTables	=	2031,
	eCleanupDeadTable		=	2032,
	// eChipsFullInfo		=	2040, Obsolete
							/// 2041 is taken manually
	eMoveAllocFpp			=   2042, //
	eTableAutoConvertion	=	2043,
	eTableChipsReturn		=	2044,
	//
	eBlitzBuyIn				=	2200,
	eBlitzReBuy				=	2210,
	eBlitzAutoRebuy			=	2213,
	eBlitzLeave				=	2220,
	eBlitzCreditSinceLeft 	=	2221, // User is credited with money/VPP earned in hands that have finished since user left Blitz session
	eBlitzMoveAllocFpp		=	2242,
	// eBlitzAutoConvertion	=	2243, // PYR-70992 - reserved for future use
	eBlitzChipsReturn		=	2244,
	eBlitzReconcileLoan		=	2245, // PYR-101785 objectid=BLITZSESSIONS.BLITZID, chips>0 - user contributed money to a hand and then folded. If the hand where money was contributed was cancelled due to crash, this transaction will return this money to the original user
	//
	// PYR-35000 - poker duel related trans types
	// PYR-53789 - all duel transtypes are obsolete
	eDuelDirectChallenge			= 2300, // user challenges another user (challengeId in objectId, challengee id in adminIntId)
	eDuelDirectChallengeAccepted	= 2301, // the challengee accepts the challenge (challengeId in objectId and challenger id in adminIntId)
	eDuelRandomChallenge			= 2305, // user challenges anybody who accepts (challengeId in objectId)
	eDuelRandomChallengeAccepted	= 2306, // random user accepted a random challenge (challengeId in objectId and challenger id in adminIntId)
	eDuelChallengeNotPlayed			= 2310, // user cancels the challenge before it is accepted (challengeId in objectId)
	eDuelRandomMatchNotPlayed		= 2311, // user cancels the random challenge before it is accepted (challengeId in objectId)
	eDuelRandomMatchTimeOut			= 2312, // PYR-37411 - random challenge cancelled due to timeout (challengeId in objectId)
	eDuelFinished					= 2320, // duel is finished normally (duelId in objectId)
	eDuelAdminCancelled				= 2330, // duel is cancelled by admin and all monies (buyin+rake) returned (duelId in objectId)
	// end of PYR-35000
	eTournRegister			=	3000,
	eTournMassRegister		=	3001,
	eTournPreRegister		=	3002,
	eTournReentry			=	3005, // PYR-24444: objectId=TOURNS.TOURNID, adminIntId=0
	eTournUnregister		=	3010, //
	eTournMassUnregister	=	3011, //
	eTournUnregisterExclusion = 3012,
	eLiveTournDebit	=			3013, // [AO 2010-08-10] #15728
	eLiveTournCredit		=	3014, // [AO 2010-08-18] #15728
	// eTournFinished		=	3020, Obsolete by #19097
	eTournDeal				=	3021,
	eTournFppReward			=	3022,
	eTournFinishedWithDeal	=	3023, // [AO 2010-08-10] #14719
	eSatelliteWinner		=	3025, // objectId=targetId, objectId2=satelliteId, comment=-targetBuyin
	eSatWinnerTicket		=	3026, // objectId=satelliteId, objectId2=targetId(except after winning a satellite), adminIntId=ticketId, comment=ticketValue(RM) or "0"(PM)
	eSatelliteWon			=	3027, // objectId=satelliteId, objectId2=targetId, comment=targetBuyin
	// eSatelliteNotFoundRefund	=	3028, Obsolete by #19360
	eTournCancelled			=	3030,
	eTournCancelledRollback	=	3031, //running tourney cancelled, buyin + rake + rebuys + addons are given back
	eTournCancelledRollforw	=	3032, //running tourney cancelled, prize pool was divided according to formula
	eTournCancelledRakeRefund	=	3033, //running tourney cancelled, rake refunded
	// eTournRegRefund			=   3040, Obsolete by #19360
	eTournRebuy				=   3045,
	eTournAddon				=	3046,
	eTournRefundRebuy		=   3047,
	eTournRefundAddon		=	3048,
	eTournAutoConvertion	=	3049,
	//eTournRepeatQual		=   3050, //repeated qualifier, buyIn lost ///!!!no longer exists, replaced with eTournRepeatedWinner
//	eTournRepeatTChips		=	3055,
	// eTournRepeatedWinner	=	3056, Obsolete by #19360
	eTournFinishedRegular	=	3057, // #19097
	eTournFinishedFreeroll	=	3058, // #19097
	eTournFinishedFpp		=	3059, // #19097
	eTournTicketIssued		=	3060,
	// eTournTicketRemoved		=	3061, // Obsolete
	eTournTicketWonInTournament	=	3062, // objectId=tournId, adminIntId=ticketId, comment=ticketValue(RM) or "0"(PM)
	eTournKnockoutBounty	=	3063,
	eTournFixedBounty		=	3064,
	eTournTeamProBounty		=	3065,
	eTournRepeatedWinnerRegular 	= 3066, // #19360
	eTournRepeatedWinnerFreeroll	= 3067, // #19360
	eTournRegRefundRegular			= 3068, // #19360
	eTournRegRefundFreeroll			= 3069, // #19360
	// eTournRegSatellByAdmin	=	3070, //registered by admin as satellite winner // Obsolete
	eSatellNotFoundRefundRegular	= 3071, // #19360
	eSatellNotFoundRefundFreeroll	= 3072, // #19360

	// the following four (Golden SNG) are obsoleted, yet preserved
	eGoldenSngBuyinAddon		= 3073, // PYR-24382, objectId=tournId,objectId2=goldenSngId
	eGoldenSngPrizepoolAddon	= 3074, // PYR-24382, objectId=tournId,objectId2=goldenSngId
	eMilestoneGoldenSngBuyinAddon		= 3075, //PYR-29642, objectId=tournId,objectId2=msGoldenSngId
	eMilestoneGoldenSngPrizepoolAddon	= 3076, //PYR-29642, objectId=tournId,objectId2=msGoldenSngId

	eTournMileStonePromot		= 3077, // PYR-28689  // comments=<see TournMileStoneWinningTableData::encodeTransComment()>
	eTournMileStonePromotTicket = 3078, // PYR-28689  // adminIntId=ticketId, comments=ticketValue
	eFlightSwitchMoveOut		= 3079, // PYR-32003
	eFlightSwitchMoveIn			= 3080, // PYR-32003
	eTournFixedBountyTournEntryRefund	= 3081, // PYR-31658,  objectId=targetTourn,adminIntId=bountyTournId
	eTournFixedBountyTicketPrize		= 3082, // PYR-31658,  objectId=bountyTournId,adminIntId=ticketId
	eTournFixedBountyTournEntryWon		= 3083, // PYR-31658,  objectId=bountyTournId,adminIntId=targetTourn
	eTournFixedBountyTournEntry			= 3084, // PYR-31658,  objectId=targetTourn,adminIntId=bountyTournId
	eTournKnockoutAddOnBounty			= 3085, // PYR-48893,  objectId=bountyTournId,adminIntId=eliminatedIntId
	eTournInterimPayout					= 3086, // PYR-50812,  objectId=tournId
	eTournKnockoutSpinBounty			= 3087, // PYR-120482,  objectId=bountyTournId,adminIntId=eliminatedIntId (note: as part of PYR-125764, not used for progressive plus, but left in case spin bounties used in normal KO tourns)
		
	// progressive plus transactions very related to tournaments, so grouped in 3000s
	eProgressivePlusBuyin					= 3200, // PYR-109327, objectId=PPPSCRIPTS.pppId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS<=0
	eProgressivePlusUnregister				= 3201, // PYR-109327, objectId=PPPSCRIPTS.pppId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>=0
	eProgressivePlusCashoutWinner			= 3202, // PYR-118560, objectId=PPPSCRIPTS.pppId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>=0
	eProgressivePlusCashoutAdmin			= 3203, // PYR-118427, objectId=PPPSCRIPTS.pppId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>=0
	eProgressivePlusTournRegister			= 3204, // PYR-125764, objectId=TOURNS.tournId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS<=0 (should be zero though for all prog+ tourns. This is the prog+ version of eTournRegister)
	eProgressivePlusFppReward				= 3205, // PYR-125764, objectId=TOURNS.tournId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.FPP>0 (and/or TRANSACTS.FPPCENTS). This is the prog+ version of eTournFppReward
	eProgressivePlusKnockoutBounty			= 3206, // PYR-125764, objectId=TOURNS.tournId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>0. This is the prog+ version of eTournKnockoutBounty.
	eProgressivePlusKnockoutSpinBounty		= 3207, // PYR-125764, objectId=TOURNS.tournId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>0. This is the prog+ version of eTournKnockoutSpinBounty.
	eProgressivePlusTournCancelledRollforw	= 3208, // PYR-125764, objectId=TOURNS.tournId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>0. This is the prog+ version of eTournCancelledRollforw.
	eProgressivePlusTournCancelled			= 3209, // PYR-125764, objectId=TOURNS.tournId,objectId2=PPPENTRIES.pppEntryId,TRANSACTS.CHIPS>=0. (should be zero though for all prog+ tourns. This is the prog+ version of eTournCancelled: players unregistered due to tournament cancelled without running)

	// Maximum value for tournament related transaction must be < eAdminAction (see isPmTransact())
	// so that all play money tournament transactions are stored in TRANSACTS table instead of TRANSACTSPM

	eAdminAction			=	4000,
	eAdminTransCrbackable	=	4001, //set transaction (not) creditbackable; objId=cashtrId, chips=0
	eAdminCreditList		=	4002, // objectId=adminIntId
	// obsolete PYR-120713
	//eAdminCreditBOP			=	4003, // admin issued a prize for Battle of the Planets score; objId=cashtransId, ps=BOP
	eAdminCreditTournLeague	=	4004, // admin issued a prize for tournament league score; objId=cashtransId, ps=PS_LEAGUE
	eAdminCreditListRakeback=	4005, // objId=adminIntId
	eIntelliPokerCredit		=	4006, // credits with INTELLIPOKER paysystem PYR-28351, objectId=cashtransId
	eRealTransferSent		=	4008, // real money, objectId=cashtransId
	eRealTransferReceived	=   4009, // real money, objectId=cashtransId
	eTransferSent			=	4010, // play money, objectId=anotherUserIntId. This transaction type is obsoleted by PYR-81908
	eTransferReceived		=   4020, // play money, objectId=anotherUserIntId. This transaction type is obsoleted by PYR-81908
	eTChipsSold				=   4021,
	eTChipsBought			=   4022,
	eTChipsSoldToPStars		=   4023, // objectId=cashtransId
	eBonusAdmin				=	4030, //admin has created a record in "USERBONUS" table or added cashtrans with ps - HAND_AWARD_NAME 
	eBonusAdminRevoke		=	4031, //admin has deleted a record in "USERBONUS" table; objId=userBonusId
	eAdminCasinoMilestone	=	4032, // PYR-46254, adminIntId != 0, chips>0, objectId = cashtransId
	eAdminCreditAvail4Cashout = 4033, // PYR-52973, NAC2-Free credit: adminIntId != 0, chips>0, objectId = cashtransId
	eAdminChqReject			=	4040, // admin has rejected an overcredit; objectId=chequeCancelTransId
	eAdminCardBlocked		=	4050,
	eAdminCardUnBlocked		=	4055,
	eAdminApproveUnknown	=	4060, // admin approved transaction with 'U' status
	eAdminApproveHold		=	4061, // admin approved transaction with 'N' status
	eAdminApproveWaiting	=	4062, // admin approved transaction with 'W' status
	eAdminApproveLocked 	=	4063, // admin approved transaction with 'L' status
	eAdminRejectUnknown		=	4070, // admin rejected transaction with 'U' status
	eAdminRejectHold		=	4071, // admin rejected transaction with 'N' status
	eAdminRejectWaiting		=	4072, // admin rejected transaction with 'W' status
	eAdminRejectLocked 		=	4073, // admin rejected transaction with 'L' status
	eAdminVipStatusSet		=	4100,
	eAdminFppReset			=	4110,
	eAdminFppRestore		=	4111,
/* obsolete after 2010-07 PYR-14303
	eAdminTransferSent		=	4200, // real money, legacy only, objectId=anotherUserIntId
	eAdminTransferReceived  =   4201, // real money, legacy only, objectId=anotherUserIntId
	eAdminTransferRejected  =   4202, // real money, legacy only, objectId=anotherUserIntId
*/
	eAdminRealTransferSent	=	4203, // real money, objectId=cashtransId
	eAdminRealTransferReceived=	4204, // real money, objectId=cashtransId
	eAdminRealTransferRejected=	4205, // real money, objectId=cashtransId
	eAdminTransferBalanceSent = 4210, // objectId=anotherUser.userIntId
	eAdminTransferBalanceReceived = 4211, // objectId=anotherUser.userIntId
	eAdminGdprBalanceCleanUp= 4212, // PYR-64479 objectId=cashtransId, may be 0 when chips = 0 and tchips > 0
	eAdminUpdateNac  		= 4251, // objectId=0, adminIntId!=0, chips=0
	eAdminUpdateItaBonus	= 4252, // objectId=0, adminIntId!=0, chips=0
//	eAdminUpdatePtNac		= 4253, // PYR-39019, removed for PYR-33529, never used on production, several records in QA, do not re-use
	eAdminResetNac2         = 4254, // PYR-33529, objectId=0, adminIntId!=0, chips=0
	eAdminUpdateUsedBalance	= 4255, // PYR-54303, PYR-54925, objectId=0, adminIntId!=0, chips=0

	eAdminChargebackReduction		= 4300, // redeem CHARGEBACK_NAME PYR-19085 see addApprovedTrans()
	eAdminChargebackRepayment		= 4301, // DEBITADJUST_NAME
	eAdminAdjustReduction			= 4302, // redeem REPAYCHARGEBACK_NAME
	eAdminAdjustCredit				= 4303, // CREDITADJUST_NAME
	eAdminProcessorFeeRepay			= 4304, // objId=cashtrId, ps=REFUNDFEE_NAME
	eAdminGoodWillCredit			= 4305, // GOODWILLCREDIT_NAME
	eAdminCancelCashout				= 4306, // CHQ_CANCEL_NAME, REDEEM_RETURN
	eAdminRejectApprovedRedeem		= 4307, // TRANS_CORRECTION - no cases on production
	eAdminApproveRejectedRedeem		= 4308, // TRANS_CORRECTION
	eAdminRevokeBonusCredit			= 4309, // TRANS_CORRECTION or redeem PROMOFUNDS_NAME
	ePromoFundsCredit				= 4310, // PROMOFUNDS_NAME
	eAdminTChipsCredit				= 4311, // DatabaseManager::creditTWChips()
	eAdminCreditSilent				= 4400, // PYR-19932
	eAdminDebitSilent				= 4401, // PYR-19932
	eAdminCreditRLB					= 4402, // admin issued a prize for Ring Leader Board, objectId=cashtransId // PYR-21785
	eAdminCreditSNGR				= 4403, // admin issued refund for Sit And Go Refunds objectId=cashtransId // PYR-30091
	eAdminLiveEventPayout		    = 4404, // objId=cashtrId, ps=LIVEEVENTPAYOUT_NAME // PYR-47840
	eAdminManuallyBonusCashCredit	= 4405, // objectId=cashtransId // PYR-63665 Admin Tool ONLY use cases: motivate to play, correct technical mistake CREDIT only
	eAdminManuallyBonusCashDebit	= 4406, // objectId=cashtransId // PYR-63665 Admin Tool ONLY use cases: motivate to play, correct technical mistake	DEBIT only
	eAdminFraudAdjustCredit			= 4407, // objectId=cashtransId // PYR-63665 Admin Tool ONLY use cases: fraud adjustment CREDIT only
	eAdminFraudAdjustDebit			= 4408, // objectId=cashtransId // PYR-63665 Admin Tool ONLY use cases: fraud adjustment DEBIT only

	ePendingActionCreditCash        = 4410, // PYR-50049: objectId=cashTransId; objectId2=USERPENDINGACTIONS.UserActionId
	ePendingActionCreditFpp         = 4411, // PYR-50049: objectId=0; objectId2=USERPENDINGACTIONS.UserActionId
	ePendingActionCreditPlayMoney   = 4412, // PYR-50049: objectId=USERPENDINGACTIONS.UserActionId
	ePendingActionCreditTChips      = 4413, // PYR-50049: objectId=USERPENDINGACTIONS.UserActionId
	ePendingActionCreditBonus       = 4414, // PYR-50049: objectId=USERBONUS.UserBonusId

	eBuyChips				=	5000,
	eBuyChipsManual			=	5001,
	eBuyChipsBonus			=	5002, // objId=cashtrId, ps=BONUS
/* not in use
	eBuyChipsBonusFpp		=   5003,
	eBuyChipsReapprove		=	5004,
	eBuyChipsBonusPartial	=	5005,
	eBuyChipsBonusClosed	=	5006,
	eBuyChipsIpnByEmail		=	5007, /// unknown IPN, user found by email match
	eBuyChipsCheque			=	5008, // positive amount = chq cancelled; negative = cheuque issued
	eBuyChipsWUnion			=	5009, // legacy, objectId=WUTRANSACTS.TRANSID
*/
	eCashout				=	5010,
	eBuyChipsWire			=   5011, // GC wire
//	eBuyChipsMoneyGram		=   5012, // not in use
	eCashoutByAdmin			=	5013,
	eCancelCashout			=	5014, // user cancels pending cashout, objectId=cancelCashtransId (not parentId)
	eCashoutRefund			=	5015, // objectId=cashoutParentId
	eCancelCashoutReturn	=	5016, // cancel pending cashout because of return/chargeback
	eConvertChipsToTchips   =   5017,
	eConsolidateDebt		=	5018, //Obsolete PYR-44529
	eBuyChipsWirecard		=	5019,
//	eCashoutRollback		=   5020, // NOT IN USE since BUILD365 (2002-12)
	eMigrationTransfer		=	5021,
	eConvertTChipsToChips   =   5022, // #14492
	eConvertFppToChips		=   5023, // #14492
	eConvertTTicketToChips	=   5024, // #14492 (adminIntId=TOURNTICKETS.ticketId)
	eBuyChipsWUnionCt		=	5026, // automated Western Union objectId=cashtransId
	eAssetsConsolidationBegin = 5027, // objectId=0
//	eAssetsConsolidationEnd =   5028, // objectId=0 // closing bracket - it was decided not to use
	eCashoutAtAcctClosure	=	5029, // created for .FR full cashout PYR-29578, objectId=parent CashtransId
	eAffiliatePayment		=	5030, // objId=cashtrId, ps=AFFILIATE
	eAchChargeBack			=   5040, // objectId=returnCashtransId comes from DBM_Q_PAYMENT_GFSL_NOTIF
//	eAchChargeBackAhead		=	5041, // not in use
//	eAchChargeBackReversed	=   5042, // not in use
	eAutoChargeBack			=   5045, // objectId = original cashtransId
	eRefundForRejectedOvercredit = 5046, // PYR-26874 objectId = refunding deposit cashtransId, adminIntId=0
	eGCDebitReject			= 	5052,	// GCDebit category = 2
	eGCDebitReverse 		= 	5053,	// GCDebit category = 3
	eGCDebitWithdrawReverse	= 	5054,	// GCDebit category = 4
	eGCDebitReversedReturn	= 	5055,	// GCDebit category = 4
	ePaymentNotification	=	5060, // comes from DBM_Q_PROCESS_PAYMENT_NOTIFICATION, objectId=returnCashTransId
	eCashierApproveHold		=	5061, // cashier approved transaction with 'N' status, objectId=cashtransId
	eCashierRejectHold		=	5062, // cashier rejected transaction with 'N' status, objectId=cashtransId
	eCashierApproveRmNokHold=	5063, // cashier approved transaction with 'N' status and RM_NOK flag, objectId=cashtransId
	eCashierRejectRmNokHold	=	5064, // cashier rejected transaction with 'N' status and RM_NOK flag, objectId=cashtransId
	
	eApprovedRmNokWinning	=	5065, // PYR-54565 objectId=cashtransId dedicated transType for crediting RmNok winnings - paired with eHoldRmNokWinnings

	eAdminApproveRmNokHold  =	5066, // admin approved transaction with 'N' status and RM_NOK flag, objectId=cashtransId
	eAdminRejectRmNokHold	=	5067, // admin rejected transaction with 'N' status and RM_NOK flag, objectId=cashtransId

	eCashierApproveRmOkRemovedHold=	5068, // cashier approved transaction with 'N' status and CASHTRANS_FLAG_RMOK_REMOVED_HOLD flag, objectId=cashtransId
	eCashierRejectRmOkRemovedHold =	5069, // cashier rejected transaction with 'N' status and CASHTRANS_FLAG_RMOK_REMOVED_HOLD flag, objectId=cashtransId

	eInterAcctTransfer		=	5070,
// two below are not in use
//	eInterAcctAutoTransferTourn	=	5071,
//	eStellarBonusPayment	=	5072,
	eHoldRmNokWinnings      =   5073, // objectId=cashTransId,adminIntId=tournId
	ePaysystemFeeRedeem		=	5074, // objectId=cashTransId
	eUpsellAwardCash		=   5075, // PYR-24840 objectId=CASHTRANS.TransId
	eUpsellAwardFpp 		=   5076, // PYR-24840 objectId=USERAWARDS.userAwardId
	ePlayMoneyPurchase		=   5077, // PYR-29144 objectId=CASHTRANSPM.TransId
	eIceQuestCashCredit		=	5078, // PYR-32134 objectId=ICETASKUSERSDONE.ACCEPTINSTANCE
	eIceQuestFppCredit		=	5079, // PYR-32134 objectId=ICETASKUSERSDONE.ACCEPTINSTANCE, chips=0
	eIceQuestPlayMoneyCredit=	5080, // PYR-32134 objectId=ICETASKUSERSDONE.ACCEPTINSTANCE, chips=0
	eConsolidateDebtEx      =   5081, // PYR-44529 objectId=parent transactionId 
	eConvertForTargetAmount =	5082, // PYR-44657 objectId=0, 2*N consecutive records in Transacts (debit,credit) joined by 1 ConvTransacts with ConversionActionType = eConvActionAutoInterAcctTransfer
// #14432 COM->FR
	eFrMigrTTicketsToTChips =	5101,
	eFrMigrBonusToCash      =	5102,
//	eFrMigrTChipsDebit		=	5110, // never in use on production
//	eFrMigrTChipsCredit		=	5111, // never in use on production
	eFrMigrCashDebit		=	5112,
	eFrMigrCashCredit		=	5113,
	eFrMigrNoCashCredit		=	5114,

	eAcctCreationCredit     =	5115, // PYR-47641 TRANSACTS (not PM) objectId=0, playChips > 0
	eUsaAcctMigrationCredit =	5116, // PYR-47641 TRANSACTS, objectId=MigratedUsers.migrationId
	eIomAcctDebitOnUsaMigr  =	5117, // PYR-53184 TRANSACTS, objectId=IOM.UserChanges.UpdateId

	// #20725
	eMigrationBegin				= 5120, // objId=migrationId
	eMigrationTTicketsToTChips	= 5121, // objId=migrationId, admId=ticketId
	eMigrationZeroTTicketsToFpp	= 5122, // objId=migrationId, admId=ticketId
	eMigrationBonusToCash		= 5123, // objectId=cashTransId,adminIntId=migrationId
	eMigrationInterAcctTransfer	= 5124, // objectId=cashTransId,adminIntId=migrationId
	eMigrationHoldExceedingFunds= 5125, // objectId=cashTransId,adminIntId=migrationId
	eMigrationHoldRmNokFunds    = 5126, // objectId=cashTransId,adminIntId=migrationId
	eMigrationCashout		    = 5127, // objectId=cashTransId,adminIntId=migrationId
	eMigrationDeposit			= 5128, // objectId=cashTransId,adminIntId=migrationId
	eMigrationCashToFpp			= 5129, // objectId=cashTransId,adminIntId=migrationId
	eConvertTTicketsToTChips    = 5130, // objectId=ticketId

//PYR-18397 ->BE
	eBeMigrBonusToCash		=	5202,	// objectId=cashTransId,adminIntId=migrationId
	eBeMigrNoCashCredit		=	5214,	// objectId=migrationId,adminIntId=0

	eFullTiltAssetMigration	=	5220, // #24070: objectId=FTASSETMIGRATION.assetMigrationId,adminIntId=cashTransId
	eFullTiltVipMigration	=	5221, // #24070: objectId=VIPAUDIT.vipAuditId, adminIntId=FTASSETMIGRATION.assetMigrationId
	eFullTiltDeposit	  	=	5222, // #24069: objectId=cashTransId - obsolete
	eFullTiltCashout	  	=	5223, // #27432: objectId=cashTransId - obsolete
	eFullTiltConversion     =	5224, // PYR-39878: objectId=FT_USERS_MIGRATION.PLAYERIX, objectId2=cashtransId(may be 0)
	eFullTiltVIPConversion  =	5225, // PYR-39878: objectId=VIPAUDIT.vipAuditId, objectId2=FT_USERS_MIGRATION.PLAYERIX
	eFullTiltConvApprovedFromHold = 5226, // PYR-39878: objectId=CASHTRANS.TransId (from cashtrans on hold)

	eHoldRmOkRemovedFunds   =   5230, // PYR-25200: objectId=cashTransId
	eGreekTaxesPaidOut      =   5232, // PYR-28936: objectId=cashTransId
	eGreekTaxesReceived      =  5233, // PYR-28936: objectId=cashTransId
	eItDormantBalancePaidOut =  5234, // PYR-31811: objectId=cashTransId - do NOT add to IT [TRANSACTION_TYPES]
	eItDormantBalanceReceived=  5235, // PYR-31811: objectId=cashTransId - do NOT add to IT [TRANSACTION_TYPES]
	eNjMigrDonationPaidOut =    5236, // PYR-27224: objectId=cashTransId
	eNjMigrDonationReceived=    5237, // PYR-27224: objectId=cashTransId
	eDebitLeftoverOnAcctClosure   = 5238, // PYR-35097: objectId=cashtransId
	eFrTChipsPaidOutOnAcctClosure = 5239, // PYR-35097: objectId=deadBalanceId , - do NOT add to FR [TRANSACTION_TYPES]
	eFrTChipsReceivedOnAcctClosure= 5240, // PYR-35097: objectId=deadBalanceId , - do NOT add to FR [TRANSACTION_TYPES]
	eHoldOnAcctClosure            = 5241, // PYR-35097: objectId=cashtransId
	eHoldRmNokAfterUserGotFunds   = 5242, // PYR-32116: objectId=cashtransId chips<0 
	eDefaultCurrencyChange        = 5243, // PYR-33529: objectId=USERCHANGES.UPDATEID, chips=tChips=0, COMMENT=OLD_CURRENCY
	eIndiaTaxesPaidOut       =  5244, // PYR-30309: objectId=cashTransId, objectId2=receiverTransactId
	eIndiaTaxesReceived      =  5245, // PYR-30309: objectId=cashTransId userId=INDIA_TAX_RECEIVER, objectId2=payerTransactId
	eDeductDormantFunds      =  5246, // PYR-48917: objectId=cashTransId (might be 0 if no cash is surrendered)
	eIndiaAnnualTaxesPaidOut =  5247, // PYR-54304: objectId=cashTransId, objectId2=receiverTransactId
	eIndiaAnnualTaxesReceived=  5248, // PYR-54304: objectId=cashTransId userId=INDIA_TAX_RECEIVER, objectId2=payerTransactId
	eIndiaTaxedUserCreditOnCashoutRefund  = 5249, // PYR-71242: chips > 0, objectId=cashTransId, objectId2=receiverTransactId
	eIndiaTaxReceiverDebitOnCashoutRefund = 5250, // PYR-71242: chips < 0, objectId=cashTransId userId=INDIA_TAX_RECEIVER, objectId2=payerTransactId
	eRomTaxesPaidOut                      = 5251, // PYR-69310: chips < 0, objectId=cashTransId, comment "RON=taxInRon"
	eRomTaxedUserCreditOnCashoutRefund    = 5252, // PYR-69310: chips > 0, objectId=cashTransId, comment "RON=taxRefundInRon"

	// PYR-77762 USNJ PYR-94573 USPA Withholding Tax on Win; !!! eUsaTaxes*TRANSTYPES HAVE SIGNIFICANT DATA IN THE COMMENT !!!
	eUsaTaxesFederalPaidOut  = 5253, // PYR-77762: chips<0, obj=fedPayCashTransId, obj2=winTransactId !!!SIGNIFICANT DATA IN THE COMMENT !!!
	eUsaTaxesStatePaidOut    = 5254, // PYR-77762: chips<0, obj=statePayCashTransId, obj2=winTransactId !!!SIGNIFICANT DATA IN THE COMMENT !!!
	eUsaTaxesReceived        = 5255, // PYR-77762: userId=[NJ|PA]_TAX_RECEIVER_[FEDERAL|STATE], chips>0, obj=cashTransId, obj2=PayOutTransactId(above) !!!SIGNIFICANT DATA IN THE COMMENT !!!
	// PYR-84531 USNJ PYR-94573 USPA Tax Refund on Unsettlement (Reverse of Settle)
	eUsaTaxedUserFederalRefund = 5257, // PYR-84531 : chips>0, obj=fedRefundCashTransId  , obj2=unsettleTransactId (debit - reverse of settle) !!!SIGNIFICANT DATA IN THE COMMENT !!!
	eUsaTaxedUserStateRefund   = 5258, // PYR-84531 : chips>0, obj=stateRefundCashTransId, obj2=unsettleTransactId (debit - reverse of settle) !!!SIGNIFICANT DATA IN THE COMMENT !!!
	eUsaTaxReceiverDebitOnReverse = 5259, // PYR-84531 : userId=[NJ|PA]_TAX_RECEIVER_[FEDERAL|STATE], chips<0, obj=cashTransId, obj2=refundTransactId(above) !!!SIGNIFICANT DATA IN THE COMMENT !!!

	eReplenishPlay			=	6000, // obsolete by PYR-84950
	eInitGuestPlayChips = 6002, // PYR-25263 //  obsolete by PYR-91342
	ePlayChipsPurchase      =   6003, // PYR-26426
	ePlayChipsRefill        =   6004, // PYR-18558: objectId=0
	ePlayChipsWatch2EarnRefill = 6005, // PYR-45097: objectId=PLAYMONEYADEVENTS.adEventId, TRANSACTS.CHIPS=0, TRANSACTS.PLAYCHIPS>0
	ePlaySiteTransfer		=	6010, // PYR-39810, obsolete

// eCorePlayMoneyCredit	=	6020, // #23344 obsolete by PYR-27640
	eCorePlayMoneyDebit 	=	6021, // #23344
	eCorePlayMoneyFreeCredit=   6022, // PYR-27640
	eCorePlayMoneyPurchase  =   6023, // PYR-27640

	eDoubleUserOld			=	7000,
	eDoubleUserNew			=	7010,

	eXcGameOverNac				=	7090, // PYR-34110, obj=xcGames.gameId , chips=0 - added so to have ground for NAC update
	eXcAutoConvertForGameIn  	=	7092, // PYR-36381, obj=xcTrans.xcReqId, (1)chips<0, (2)chips>0
	eXcAutoConvertOnGameOut	    =	7093, // PYR-45419, obj=TRANSACTS.transId (winning credit transaction), (1)chips<0, (2)chips>0

	eXcRacePay                  =   7096, // PYR-83160, obj=XC_RACE_SESSIONS.RACESESSIONID, chips>0; MikhailA agreed to not save transacts if no wins

	eXcGameBet      			=	7101, // PYR-29620, obj=xcTrans.xcReqId, chips<0
	eXcGameReserve  			=	7102, // PYR-29620, obj=xcTrans.xcReqId, chips<0
	eXcGameUnreserve			=	7103, // PYR-29620, obj=xcTrans.xcReqId, chips>0
	eXcGamePay      			=	7104, // PYR-29620, obj=xcTrans.xcReqId, chips>=0 (=0 for lost but got some CPP)
	eXcGameUnreserve4Bet		=	7105, // PYR-29620, obj=xcTrans.xcReqId, chips>0, objectId is same as for eXcGameBet
	eXcGameReturnUnresolvedBet	=	7106, // PYR-29620, obj=xcTrans.xcReqId, chips>=0, adminIntId!=0
	eXcGameUnreserveOnGameOver	=	7107, // PYR-29620, obj=xcTrans.xcReqId, chips>0
	eXcGameBuyIn  				=	7108, // PYR-33487, obj=xcTrans.xcReqId, chips<0
	eXcGameCancelBuyIn			=	7109, // PYR-33487, obj=xcTrans.xcReqId, chips>0

	eRollIssuedPending      =	7111, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0 (<0 for bought userrolls with Debit flag is on), TRANSACTROLLS.AmountDelta = 0
	eRollAccepted           =	7112, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS<=0, TRANSACTROLLS.AmountDelta > 0
	eRollRejected           =	7113, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0, TRANSACTROLLS.AmountDelta = 0
	eRollIssued             =	7114, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0 (<0 for bought userrolls with Debit flag is on), TRANSACTROLLS.AmountDelta > 0
	eRollVoided             =	7115, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0, TRANSACTROLLS.AmountDelta < 0
	eRollSurrendered        =	7116, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS>=0 (>0 if RMC left), TRANSACTROLLS.AmountDelta < 0
	eRollClearedAndPaid     =	7117, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS>=0, TRANSACTROLLS.AmountDelta = -TRANSACTS.CHIPS, TRANSPROPS.OBJECTID2=Cashtrans.TransId
	eRollExpiryChanged      =	7118, // PYR-31829: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0
	eRollVoidedAfterExpir   =	7119, // PYR-32557: obj=UserRolls.userRollId, TRANSACTS.CHIPS>=0, TRANSACTROLLS.AmountDelta <= 0
	eRollLeftoverConvertedToFpp  = 7120, // PYR-33880: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0, TRANSACTS.FPP>0, TRANSACTROLLS.AmountDelta < 0, admin = 0
	eRollLeftoverConvertedToCash = 7121, // PYR-33880: obj=UserRolls.userRollId, TRANSACTS.CHIPS>0, TRANSACTROLLS.AmountDelta < 0, admin = 0
	eRollLeftoverVoided     =	7122, // PYR-33880: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0, TRANSACTROLLS.AmountDelta < 0
	eRollRmNokPayment		=	7123, // PYR-33405: obj=UserRolls.userRollId, TRANSACTS.CHIPS<0
	eRollClearedAndNewRollIssued   = 7124, // PYR-45953, clear Roll (FSB) into 2nd Roll, obj=1st UserRolls.userRollId, TRANSACTS.CHIPS=0, TRANSACTROLLS.AmountDelta<0, TRANSPROPS.OBJECTID2=2nd UserRolls.userRollId
	eRollFullyUsed			=	7125, // PYR-103348: obj=UserRolls.userRollId, TRANSACTS.CHIPS=0, no TRANSACTROLLS, transId => UserRollsAudit.transId for StatusNew=eUserRollStatus_FullyUsed

	eGameTicketIssued              = 7130, // PYR-45645: obj=GameTickets.gameTicketId,TRANSACTS.CHIPS=0, TRANSACTS.FPP=0
	eGameTicketAcceptedWithNoCost  = 7131, // PYR-45645: obj=GameTickets.gameTicketId,TRANSACTS.CHIPS=0, TRANSACTS.FPP=0
	eGameTicketRejected            = 7132, // PYR-45645: obj=GameTickets.gameTicketId,TRANSACTS.CHIPS=0, TRANSACTS.FPP=0
	eGameTicketVoided              = 7133, // PYR-45645: obj=GameTickets.gameTicketId,TRANSACTS.CHIPS=0, TRANSACTS.FPP=0
	eGameTicketExpired             = 7134, // PYR-45645: obj=GameTickets.gameTicketId,TRANSACTS.CHIPS=0, TRANSACTS.FPP=0
	eGameTicketAcceptedWithFppCost = 7135, // PYR-45645: obj=GameTickets.gameTicketId,TRANSACTS.CHIPS=0, TRANSACTS.FPP<0

	eSportsBookFundsTaken      = 7201, // SBK-62   debit  the user's account  objectId=sportsTransId
	eSportsBookFreeBetAwarded  = 7203, // PYR-38966 a freebet was awarded to the player TRANSACTS.CHIPS==0, objectId=sportsTransId
	eSportsBookFundsReturned   = 7204, // SBK-62   credit the user's account (user won)	objectId=sportsTransId
	eSportsBookFundsUnsettled  = 7208, // SBK-62, PYR-32567   debit  the user's account (undo the winning, balance can go negative!!)  objectId=sportsTransId
	eSportsBookFreeBetExpired  = 7209, // PYR-50768 a freebet was expired (this message is reserved but not sent to TRANSACTS yet, PYR-51878 will handle that)
	eSportsBookCreditPoints    = 7210, // PYR-33399, deposit accumulated SPP,FPP objectId=sportsTransId

	eSportsBookAutoConversion  = 7211, // SBK-62    autoconvert flag for autoConvertTranType objectId=sportsTransId
	eStarsDraftAutoConversion  = 7212, // PYR-40346 autoconvert flag for autoConvertTranType objectId=dfsTransId
	ePoolBettingAutoConversion = 7214, // PYR-42410 autoconvert flag for autoConvertTranType objectId=PB_TransId

	eSportsBookBetSPP          = 7220, // PYR-35979 add SPP,FPP earned for a specific bet             TRANSACTS.CHIPS==0, objectId=sportsTransId
	eSportsBookBetPlace        = 7221, // PYR-35979 debit/withdraw money from account to pay for bet  TRANSACTS.CHIPS<=0, objectId=sportsTransId, PYR-53041 (2018.build02) TRANSRPOPS.objectId2=betId
	eSportsBookBetSettle       = 7222, // PYR-35979 credit/deposit into account bet returns,          TRANSACTS.CHIPS>=0, objectId=sportsTransId, PYR-53041 (2018.build02) TRANSRPOPS.objectId2=betId
	eSportsBookFreeBetUsed     = 7223, // PYR-38966 a freebet was at least partially used to purchase the bet TRANSACTS.CHIPS==0, objectId=sportsTransId
	eSportsBookBetCashout      = 7225, // PYR-35979 credit/deposit into account                       TRANSACTS.CHIPS>=0, objectId=sportsTransId, PYR-53041 (2018.build02) TRANSRPOPS.objectId2=betId
	eSportsBookBetVoid         = 7226, // PYR-35979 credit/deposit into account                       TRANSACTS.CHIPS>=0, objectId=sportsTransId, PYR-53041 (2018.build02) TRANSRPOPS.objectId2=betId
	eSportsBookBetUnsettle     = 7228, // PYR-35979 balance can become negative!!                     TRANSACTS.CHIPS<=0, objectId=sportsTransId, PYR-53041 (2018.build02) TRANSRPOPS.objectId2=betId

	eStarsDraftSecureFunds     = 7231, // PYR-40346 debit/withdraw cash from account to pay for contest TRANSACTS.CHIPS<0,  objectId=DFS_TranId
	eStarsDraftContestPayout   = 7232, // PYR-40346 credit/deposit cash returns to account              TRANSACTS.CHIPS>=0, objectId=DFS_TranId
	eStarsDraftRefundPartial   = 7233, // PYR-40346 credit/deposit cash that was not used               TRANSACTS.CHIPS>=0, objectId=DFS_TranId
	eStarsDraftVoidEntry       = 7236, // PYR-40346 credit/deposit cash for voided contest              TRANSACTS.CHIPS>=0, objectId=DFS_TranId
	eStarsDraftReversePayout   = 7238, // PYR-40346 debit/withdraw cash, balance can become negative!!  TRANSACTS.CHIPS<=0, objectId=DFS_TranId

	ePoolBettingBetPlace       = 7241, // PYR-42410 TRANSACTS.CHIPS<0,  TRANSACTS.objectId=PB_TRANSACTS.tranId
	ePoolBettingBetSettle      = 7242, // PYR-42410 TRANSACTS.CHIPS>=0, TRANSACTS.objectId=PB_TRANSACTS.tranId
	ePoolBettingBetCashout     = 7245, // PYR-42410 TRANSACTS.CHIPS>=0, TRANSACTS.objectId=PB_TRANSACTS.tranId
	ePoolBettingBetVoid        = 7246, // PYR-42410 TRANSACTS.CHIPS>=0, TRANSACTS.objectId=PB_TRANSACTS.tranId
//	ePoolBettingBetUnsettle    = 7248, // obsolete PYR-42410

	eSocialCasinoDebit			= 7250, //PYR-77583: TRANSACTSPM.PLAYCHIPS<=0, objId=SC_TRANSACTS.scTransactId
	eSocialCasinoCredit			= 7251, //PYR-77583: TRANSACTSPM.PLAYCHIPS>=0, objId=SC_TRANSACTS.scTransactId

	eAccountClosed			=	8000,
	eAccountReopened		=	8001,
	eFppForQuestionnaire	=	8002,
	eNjAccountBlcklistedAndClosed =	8003, // #27257 objectId=NJBLACKLIST.blacklistId
	eRmfAccountChanged      =   8005, // PYR-37733 chips=0, objId = FrBankAccountsAudit.ID
	eCashOverflowProtected  =	8006, // PYR-77123, PYR-85287 chips=0, objectId=wouldBeOverflow_TransactId_with_now_0_chips, TRANSRPOPS.objectId2=cashTransId 'on hold' (see its CASHTRANS_PROP_TYPE_OVERFLOW_LINK), COMMENT=AmountOnHold
	eAccountDisabled		=	8010,
	eVipStatusSet			=	8100,
	eCppStatusSet			=	8110, // objectId=CPPSTATUSAUDIT.AuditId
	eCppCountersMonthlyReset=	8111, // objectId=CPPSTATUSAUDIT.AuditId
	eCppCountersAdminUpdate	=	8112, // objectId=CPPSTATUSAUDIT.AuditId, adminIntId != 0
	eSppStatusSet			=	8114, // objectId=SPPSTATUSAUDIT.AuditId  PYR-33399
	eSppCountersMonthlyReset=	8115, // objectId=SPPSTATUSAUDIT.AuditId
	eSppCountersAdminUpdate	=	8116, // objectId=SPPSTATUSAUDIT.AuditId, adminIntId != 0
	eIntelliPokerFPPTransfer =	8200, // objectId=IPTransferRequests.requestId

//	eFppStorePurchase		=	9000, // obsolete. instead eFppStorePurchaseWithCart and eFppStaffPurchaseWithCart are used
	eFppStoreBonusPurchase	=	9001, // objectId = UserBonus.UserBonusId
	eFppStoreCouponRedemption =	9002,
	eFppStorePurchaseWithCart =	9003, // objectId = StoreOrders.CardId
	eFppStorePurchaseCancelled = 9020,
	eFppStaffPurchaseWithCart	= 9021, // PYR-22653
	eFppStaffPurchaseCancelled	= 9022,
	eFppStoreTicketPurchase	=	9023, // objectId = TOURNTICKETS.TicketId PYR-42063
	eFppStoreGiftCardPurchase	=	9024, // objectId = CLAIMCODES.claimCodeId PYR-42063
	eFppStoreRegularPurchase	=	9025, // objectId = STOREITEMS.ItemId PYR-42063


	eMigrationFppRemoval		= 9100, // #23155: FPP License Migration FPP removal (Origin); objId=vipAuditId
	eMigrationFppRecredit		= 9101, // #23155: FPP License Migration FPP re-credit (Destination); objId=vipAuditId

	eFppBundlesAward			= 9200, // PYR-32027
	eFppOneTime2016Increase		= 9201, // PYR-39146

	eMinigameEntry						= 9300, // PYR-37300; objId=MG_PLAYS.mgPlayId.
	eMinigamePrize						= 9301, // PYR-37300; objId=MG_PLAYS.mgPlayId.
	eMinigameJackpotParticipantShare	= 9302, // PYR-37300; objId=MG_PLAYS.mgPlayId.
	eMinigameJackpotWinnerShare			= 9303, // PYR-37300; objId=MG_PLAYS.mgPlayId.
	eMinigameTicketPrize				= 9304, // PYR-37300 - player won a tournament ticket; objId=MG_PLAYS.mgPlayId, adminIntId=TOURNTICKETS.ticketId
	eCardMatchPrize						= 9305, // PYR-39403; objId=MG_PLAYS.mgPlayId.
	eCardMatchTicketPrize				= 9306, // PYR-39403 - player won a tournament ticket; objId=MG_PLAYS.mgPlayId, adminIntId=TOURNTICKETS.ticketId
	eCardMatchEntry                     = 9307, // PYR-40872; objId=MG_PLAYS.mgPlayId.

	eFppToCashAutoConversion			= 9310, // PYR-43886 fpp converted to cash automatically;

	eBonusGameEntry                     = 9320, //PYR-40872; objId=MG_PLAYS.mgPlayId.
	eBonusGamePrize                     = 9321, //PYR-40872; objId=MG_PLAYS.mgPlayId.
	eBonusGameTicketPrize               = 9322, //PYR-40872; player won a tournament ticket; objId=MG_PLAYS.mgPlayId, adminIntId=TOURNTICKETS.ticketId
	eChestGameMiniGamePrize			= 9323, //PYR-45550; prize awarded out of a chest game; objId=MG_PLAYS.mgPlayId
	eChestGameMiniGameTicketPrize	= 9324, //PYR-45550; player won a tournament ticket from a chest game; objId=MG_PLAYS.mgPlayId, adminIntId=TOURNTICKETS.ticketId

	//PYR-55866
	eSuitRaceEntry					= 9325, //PYR-55866: objId=MG_PLAYS.mgPlayId, TRANSACTS.CHIPS<=0, TRANSACTS.FPP<=0
	eSuitRacePrize					= 9326, //PYR-55866: objId=MG_PLAYS.mgPlayId, TRANSACTS.CHIPS>=0
	eSuitRaceTicketPrize			= 9327, //PYR-55866: objId=MG_PLAYS.mgPlayId, adminIntId=TOURNTICKETS.ticketId, TRANSACTS.CHIPS=0

	eFppStarsStoreDebit				= 9328, //PYR-81644: objId=SST_TRANSACTS.refTransId, TRANSACTS.CHIPS=0, TRANSACTS.FPP<0
	
	//PYR-105874
	ePokerSideBetEntry				= 9329, //PYR-105874: objId=MG_PLAYS.mgPlayId, TRANSACTS.CHIPS<=0
	ePokerSideBetPrize				= 9330, //PYR-105874: objId=MG_PLAYS.mgPlayId, TRANSACTS.CHIPS>=0 - payout from a winning side bet
	ePokerSideBetRefundPlayer		= 9331, //PYR-105874: objId=MG_PLAYS.mgPlayId, TRANSACTS.CHIPS>=0 - refund for a sidebet cancelled on player request
	ePokerSideBetRefundSystem		= 9332, //PYR-105874: objId=MG_PLAYS.mgPlayId, TRANSACTS.CHIPS>=0 - refund for a sidebet cancelled by the system
	
	// PYR-106460
	ePokerSideBetVPPReward			= 9333, //PYR-106460: objId=MG_PLAYS.mgPlayId - VPP credit from a poker side bet
	eMpcChestMiniGamePrize			= 9334, // PYR-87284: prize awarded out of a MPC chest; objId=MG_PLAYS.mgPlayId, CHIPS>=0, TCHIPS>=0, FPP>=0

	eTransTypeUndefined		=	9999
// !! NOTE !! when adding new constant to TransactionType ensure that 
//            isExternalMoneyTransType() function includes it into the logic
//            and that if it the adminIntId = ticketId that the transactsAdminIntIdIsTicketId() function is updated
// !! Also ensure that new values ARE NOT less than 1000!!
};

// #11412 #11413, Multicurrency, CONVMARGINS and CONVTRANSACTS ActionType field
// !! NOTE !! when adding new constants to ConversionActionType ensure that 
//            accountIsSourceNotDest() function includes them into the logic
enum ConversionActionType	// defines (a) the margin applied to the mid-price convertion rate and 
{							//		   (b) the ACCT<->BILL direction of the transaction
	eConvActionDefault				= 0, // use default margin
	eConvActionZeroMargin			= 1, // guaranteed Zero Margin; can not be updated by Admin

	eConvActionTournRegister		= 2,
	eConvActionTournUnregister		= 3,
	eConvActionTournWon				= 4,
	eConvActionTournPsCancelled		= 5, // same for both cancellation and cancellation with Rollback
	eConvActionTournPsCancelRollforw= 6,
	eConvActionDeposit				= 7, // Player deposits
	eConvActionCashout				= 8, // Player cashouts
	eConvActionForcedInterAcctTransfer = 9, // money action is requested against closed account - applied to default with conversion
	eConvActionReconcileDebt		= 10, // Conversion for user debt reconciliation
	eConvActionP2PTransfer			= 11, // Player to Player Real Money Transfer
	eConvActionTableLeave			= 12, // Money taken from ring tables
	eConvActionInterAcctTransfer	= 20,
	eConvActionAutoInterAcctTransfer = 21, // #13245 Auto intra account transfer
	eConvActionQuestCashCredit		= 22, // PYR-32134
	eConvActionSportsBookGeneric	= 23, // PYR-35556
	eConvActionSportsBookWin		= 24, // PYR-35556
	eConvActionSportsBookVoid		= 25, // PYR-35556
	eConvActionStarsDraftSecureFunds  = 26, // PYR-40346
	eConvActionStarsDraftContestPayout= 27, // PYR-40346
	eConvActionStarsDraftVoidEntry    = 28, // PYR-40346
	eConvActionStarsDraftReversePayout= 29, // PYR-40346
	eConvActionStarsDraftRefundPartial= 30, // PYR-40346
	eConvActionPoolBettingPlace       = 31, // PYR-42410
	eConvActionPoolBettingSettle      = 32, // PYR-42410
	eConvActionPoolBettingCashout     = 33, // PYR-42410
	eConvActionPoolBettingVoid        = 34, // PYR-42410
//	eConvActionPoolBettingUnsettle    = 35, // PYR-42410 obsolete

// !! NOTE !! when adding new constants to ConversionActionType ensure that 
//            accountIsSourceNotDest() and validateConversionActionType() functions include them into the logic

	eConvActionPoolBettingLast        = eConvActionPoolBettingVoid // please keep up-to-date
};

/// field type USERCHANGES.UPDATEDFIELD
enum eFieldType
{
	fieldAddr1 = 1,
	fieldAddr2 = 2,
	fieldCity = 3,
	fieldState = 4,
	fieldCountry = 5,
	fieldZip = 6,
	fieldEmail = 7,
	fieldPassword = 8,
	fieldFullName = 9,
	fieldPhone = 10,
	fieldPrivileges = 11,
	fieldFlags = 12,
	fieldImage = 13,
	fieldCategory = 14,
	fieldSecureId = 15,
	fieldNoField = 16, //for doubleUser - more than one field is updated
	fieldVisited = 17,
	fieldBirthDate = 18,
	fieldLocale = 19,
	fieldDepositLevel = 20,
	fieldPrivileges2 = 21,
	fieldClearedMask = 22,
	fieldEmLocale = 23,
	fieldTimezone = 24,
	fieldFirstName = 25,
	fieldLastName = 26,
	fieldFiscalCode = 27,
	fieldFlags2 = 28, // [AO 2009-03-05] #10684
	fieldAdminFlags = 29, // #10424
	fieldDefaultCurrency = 30,
	fieldCurrencyUserAccounts = 31,
	fieldLicenseId = 32,
	fieldSex = 33, // #11771
	fieldMobile = 34, // #14241
	fieldFrSpendingLimit = 35, // #14614: User::frSpendingLimit is bound to USERS.PENDING_BONUS
	fieldBlankPersonalInfo = 36,	// #15466 - blank out personal info
	fieldAdminCreateUserAcct = 37, // PYR-16191
	fieldPlatformsUsed = 38, // PYR-23460
	fieldFacebookIntId = 39, // #23343
	fieldBlockingCode = 40, // PYR-24812
	fieldIdExtension = 41, // PYR-29384
	fieldPrivileges3 = 42, // PYR-30588
	fieldFlags3 = 43, // PYR-36189
	fieldBrandsUsed = 44, // PYR-37793
	fieldFiscalCodeEncr = 45, // PYR-50129 audit (IUD) for field UserFiscalEncr.FiscalEncr
	fieldGdprAnonymized = 46, // PYR-67255 PYR-70064 virtual field changed from 0 to >0 when user is anonymized
	fieldVisitedEx = 47, // PYR-99719
};

/// update type USERCHANGES.UPDATETYPE
enum eUpdateType
{
	eUpdateType_UNDEFINED = 0, // PYR-84289 will never appear in DB
	typeUpdateUserInfo = 1,
	//typeUpdateEmail = 2,
	//typeUpdateOneString = 3,
	typeRemovePrivDuplicate = 4,
	typeRemovePrivChangeImg = 5,
	typeApproveUsers = 6,	
	typeCloseAccount = 7,
	typeReopenAccount = 8,
	typeAdminUpdatePriv = 9,
	typeActivateAccount = 10,
	typeAdminBan = 11,
	typeAdminUnban = 12,
	typeAdminUpdatePwd = 13,
	typeAdminSetCategory = 14,
	typeRevokeEarlyCashout = 15,
	typeAutoBanUserByCard = 16,
	typeUpdateNoEmailPriv = 17,
	typeRevokeBonusAfterCashout = 18,
	typeResetPwd = 19,
	typeBouncedEmail = 20,
	typeCashoutUpdatePhone = 21,
	typeFppPurchaseUpdarePhone = 22,
	typeResetBounced = 23,
	typeAdminLoginError = 24,
	typeChatRevokeMass = 25,
	typeCreateAffiliate = 26,
	typeResetBouncedMass = 27,
	typeUserUpdatePriv = 28,
	typeAdminUpdateSid = 29,
	typeDoubleUser = 30,
	typeVisitedMask = 31,
	typeCashinUpdatePhone = 32,
	typeChatReinstateAdmin = 33,
	typeCBackauto = 34,
	typeCBackCleared = 35,
	typeAutoReturnChat = 36,
	typeChatRevokeAdmin = 37,
	typeChatReturnAdmin = 38,
	typeSetHasRelated = 39,
	typeUserAgeVerified = 40,
	typeUserAdminAgeVerified = 41,
	typeUserAgeVerifiedAch = 42,
	typeSelfExclusion = 43,
	typeSelfExclusionExpired = 44,
	typeSelfExclusionRemoved = 45,
	typeUpdateUserLocale = 46,
	typeHaveMrmcAccount = 47,
	typeHaveWireAccount = 48,
	eTypeSetLimits = 49,
	typeDepositUncleared = 50,
	typeDepositsCleared = 51,
	typeIntelliPokerRegistration = 52,
	typeInsertUserLevel = 53,
	typeUpdateUserLevel = 54,
	typeUserAuthVerified = 55,
	typeDepositsClearedAdmin = 56,
	typeUserAdminAuthVerified = 57,
    typeHoldCashoutsSet = 58,
    typeHoldCashoutsCleared = 59,
	typeUpdateUserEmLocale = 60,
	typeChangeSetIntelliPokerImage = 61,
	typeChangeResetIntelliPokerImage = 62,
	typeEncryptPassword = 63,
	typeClearOpenTextPassword = 64,
	typeSelfExclusionByAdmin = 65,
	typeSetTableLimit = 66,
	typeUpdateUserTimezone = 67,
	typeUpdateAuthFlags = 68,
	typeAuthServerUpdateSid = 69,
	typeAdminUpdatePriv2 = 70,
	typeRsaActivated = 71,
	typeUpdateAdminFlagsByAdmin   = 72, // #10424 USER.AdminFlags
	typeUpdateAdminFlagsByLobby   = 73,
	typeUpdateAdminFlagsByCashier = 74,
	typeChangeDefaultBalanceCurrency = 75,
	typeAddNewAccountUserAccounts = 76,
	typeChangeLicenseId = 77,
	typeAccountConversion = 78, //account gets converted (presently .COM to .IT)
	typeDeleteCurrencyAccount = 79,
	typePSTeamMembership = 80,
	typeAutoConversionPreference = 81,
	typeDropOneTimeBypassConvLimits = 82, // #13713
	typeSetTournLimit = 83, // [AO 2010-03-29] #14033
	typeSentWelcomeEmail = 84, // PYR-12055
	typeUpdateFrSpendingLimitByUser = 85, // #14614
	typeUpdateFrAccValidation = 86, // #14427
	typeSetPlayingTimeLimit = 87, // #15250
	typeUserPepWatchScreenedFlagSet = 88, // #16866
	typeHomeGameParticipation = 89, // 16182
	typeBlankPersonalInfo = 90, // 15466
	typePlayedFirstRMGame = 91, // #17032
	typeSetHasPendingGameLimitFlag2 = 92, // #15894
	typeSetNoHomeGamesEmailsPrivilege = 93, // #17767
	typeUpdateFastDepositInfo = 94, // #18174
    typeValidateMobile = 95, // #14242
	typeRealMoneyOkRevoked = 96, // #19039
	typeUpdateBeAccVerification = 97, //PYR-18568
	typeTChips2ChipsConverted = 98, //PYR-19017
	typeUpdateItAccVerification = 99, // PYR-18621
	typeAdminCreateUserAcct = 100, // PYR-16191
	typeUserHasRmNokHeldDeposit = 101, // #19393
	typeUserHasTTicketsTotalCalculated = 102, // #19360
	typeSetHasPendingDepositLimitFlag2 = 103, // #19662 + #19893
	typeUserGetsArchived = 104, // #20399
	typeUserGetsUnArchived = 105,
//	typeEsMigration = 106,
	typeUpdateAccVerification = 107, // PYR-20730
	typeUserVerificationByRegulator = 108, // #20724
	typeUserRofusBlacklisted = 109,		//PYR-16706
	typeUserHasRmNokHeldWinnings = 110,
	typeChangeBanByRegulatorFlag2 = 111,
	typeTermsOfServiceSigned = 112,
    typeClearScreenFlag2 = 113, 	// PYR-16913
	typeChangeVppMinimumFlag = 114, // #21602
	typeUserHasRmNokHeldTransfer = 115, // PYR-21948
	typeSafeModeUpdate = 116,		// PYR-21732
	typeItLicenseVersionMigration = 117,	// #22023
	typeFppPurchaseUpdateFiscalCode = 118, // PYR-18383
	typeAdminUpdateFiscalCode = 119, // PYR-18383
	typeSmsValidatedForNewInstallId = 120, // PYR-21829
	typePlatformsUsedMask = 121, // PYR-23460
	typeFacebookAcctLink = 122, // #23923
	typeSMSPwdReset = 123, // PYR-22184
	typeFullTiltPairingCreated = 124, // #23771 - obsolete
	typeFullTiltPairingDeleted = 125, // #23771
	typeSmsMareketingFlags2 = 126, // PYR-23394
	typeClearHasRelated = 127, // PYR-23446
	typeAdminSmsMareketingFlags2 = 128, // PYR-23394
	typeAdminUpdateBlockingCode = 129, // PYR-24812
	typeRealMoneyOkRevokedByAutoVerify = 130, // PYR-25239
	typeDeleteAllUserDepositGroupLevels = 131, // PYR-24754
	typeInitGuestUserPwd = 132, // PYR-25263 // obsolete by PYR-91342
	typeDepositAttemptFlag2Set = 133, // PYR-22920
	typeDepositAttemptFlag2Cleared = 134, // PYR-22920
	typeBoughtPMchipsFlag2Set = 135, // PYR-26499
	typeSaveEcheckSsn = 136, // PYR-27975
	typeStrongAuthReqdSet = 137,		//	PYR-27258
	typeStrongAuthReqdCleared = 138,	//	PYR-27258
	typeLoginEmailNotification = 139, // PYR-27972
	typeSetBuyinLimitEx = 140,			// PYR-27249
	typeNjMigrationCashConsolidation = 141,	// PYR-27224
	typeSetStrongAuthReqd = 142,		//	PYR-28552
	typeDoubleUserNew = 143, // PYR-29009
	typeUserIdExtensionUpdated = 144, // PYR-29384
	typePlayMoneyPurchaseApproval = 145, // PYR-29055
	typeRealMoneyOkRevokedByArjelAccClose = 146, // PYR-29386
	typeSetCasinoGameLimit = 147, // PYR-29581
	typePlayedFirstRMCasinoGame = 148, // PYR-29782
	typeInClientPwdSetPM = 149,		//	PYR-30393
	typeInClientPwdSetRM = 150,		//	PYR-30393
	typeAdminUpdatePriv3 = 151, // PYR-30588
	typeChallengeQuestions = 152,	//	PYR-28732
	typeLoginLimitedMode = 153,	//	PYR-28735
	typeAccountRevalidationReqd = 154, // PYR-28735
	typeAdminSetAffiliate = 155,			//	PYR-30714
	typeSetSportsStakeLimit = 156,  // PYR-32180
	typeEmailUnauthorized = 157, // PYR-29932
	typeAdminSetEmailValidationFlags = 158, //	PYR-29932
	typeSetSportsAccumulatedLimit = 159, // PYR-34388
	typeUserHasRmNokHeldRollPayment = 160, // PYR-33405
	typeRateMobileApp = 161,				//	PYR-33276
	// Obsolete PYR-54639 typeConvertProgressiveToRegular = 162,				//	PYR-34900
	//typeSetPokerDuelLimit = 163,			//PYR-35875 // PYR-53789 - obsolete
	typeDfsFirstUsageFlags2 = 164,			// PYR-37510
	typeBrandsUsedMask = 165, // PYR-37793
	typeSetFpOneTime2016IncreaseFlag2 = 166, // PYR-39146
	typeHoldCashoutsSetOnBigWin = 167, // PYR-39500
	typeNemIdAuthenticated = 168, // PYR-33675
	typeCancelAllPendingLimits = 169, // PYR-40639
	typeFullTiltConvertion = 170, // PYR-39878
	typeWebPwdReset	= 171,			//	PYR-41017
	typeFirstSportsBet	= 172,		//	PYR-39679
	typeChangeBanByPortugueseRegulatorFlag = 173, // PYR-38554 
	typeInvalidateWireAccount = 174, // PYR-37140
	typeCBackautoPM = 175, // PYR-29824
	typeAuxSetAutoconversion = 176, // PYR-44656
	typeAuxCreateNewAccOnStartGame = 177, // PYR-45869
	typeUserHasRollbackHistoryCalculated = 178, // PYR-44166
	typeUserHasRollbackApproved = 179, // PYR-44166
	typeAdminModifyDueDiligence = 180, // PYR-47543
	typeSecServerModifyDueDiligence = 181,	//	PYR-47597
	typeSecServerModifyPriv2 = 182,			//	PYR-47597
	typeDirectDatabaseUpdateByDBA = 183,			//	PYR-47161
	typeCardVerified = 184,			//	PYR-47572
	typeSetLossLimit = 185,			//	PYR-47770
	typeSetSportsLossLimit = 186,	//	PYR-47770
	typeSetSportsSpendLimit = 187,	//	PYR-47770
	typeSetSpendLimit = 188,		//	PYR-47770
	typeLoginTotalLimit = 189,		//  PYR-47770
	typeCashierSetAutoconversion = 190, // PYR-44657
	typeCvlOptedIn = 191, // PYR-45659
	typeUnregisterBgPlayer = 192,   // PYR-49027
	typeInconsistentAccountData = 193, // PYR-51048
	typePayPalCheckoutDetails = 194, // PYR-33629 see MSG_PAYPAL_GET_PERSONAL_INFO_REPLY from gateway
	typeCvlExcluded = 195, // PYR-51534
	typeAdminModifyFiscalCodeVerified = 196, // PYR-53001
	typeTempPinUpdateSid				= 197,	//	PYR-51853
	typeSecModifyFiscalCodeVerified = 198, // PYR-53001
	typeUsBlacklistSuspisiousUser = 199, // PYR-54709, PYR-111146
	typeUserAutoUpdateNoMail = 200, // PYR-56207
	typeUserFlagGdprReady = 201, // PYR-58575
	typeTempPinFromNewDevice = 202,	//	PYR-60673
	typeCloseArchivedAccount = 203,	//	PYR-58575
	typeGdprAnonymization = 204, // PYR-67255
	typeEulaSigned = 205, // PYR-64283
	typeChangeBanByItalianRegulatorFlag = 206, // PYR-67720
	typeExternOpSetKYCCompleted = 207,			// PYR-64085
	typeAdminSetFraudulentAccountFlag = 208, // PYR-69503
	typeAdminClearFraudulentAccountFlag = 209, // PYR-69503
	typeTempPinFromAdminMassAssign = 210,	//	PYR-68134
	typeClearTempPinFromLogin = 211,		//	PYR-68134
	typeClearTempPinFromAdmin = 212,			//	PYR-68134
	typeArcUserUpdateCommPrefs = 213, // PYR-73376 for archived user only
	typeKYCRemovedByAsiaCDD = 214,	//	PYR-64085
	typeKYCSetByAdmin = 215,		//	PYR-64085
	typePtDormantSuspended = 216,	//	PYR-77866
	typePtDormantCancelled = 217,	//	PYR-77866
	typeSetTempPinEachLogin = 218,		//	PYR-76895
	typeClearTempPinEachLogin = 219,	//	PYR-76895
	typeSeBankIdVerification = 220, // PYR-78110
	typeDepositAttemptFlag2PreCashin = 221, // PYR-76456 post-fix for PYR-22920
	typeSetBanByItalianRegulatorAamsReplicatorFlag = 222, // PYR-67696
	typeSetBanByItalianRegulatorOnRmfFlag = 223, // PYR-67696
	typeApplyRestrictionsOnSochiDeposit = 224, // PYR-84289
	typeChangePrivilegesByUserRequest = 225, // via MSG_CASHIER_SET_USER_ALL_PRIVILEGES -> DBM_Q_SET_PRIVILEGES (very limited num of privs)
	typeAdminSuspendUserAccount = 226, // PYR-83446
	typeAdminUnSuspendUserAccount = 227, // PYR-83446
	typePlayerVerificationRequired = 228, // PYR-90560
	typePlayerVerificationPassed = 229, // PYR-90560 Obsolete: Not Used in Production
	typeAutomationSetFlags = 230, // PYR-92700
	typeMarketingCommListFlags = 231, // PYR-96234
	typeSwsSetBannedByReg = 232,	  // PYR-98295
	typeSwsClearBannedByReg = 233,	  // PYR-98295
	typeRGSuspendRMPriv = 234, // PYR-80956
	typeAutomationSetFlags2 = 235, // PYR-88251
	typeChangeBanByUsPaRegulatorFlag = 236, // PYR-92697
	typeClearDormantAccountFlag2OnLogin = 237, // PYR-88251
	typeUsMigrationCloseIomUser = 238, // PYR-92691
	typeLoginCoolOffLimitFlag3 = 239, // PYR-105467
	typeReactivateDormantAccountOnLogin = 240, // PYR-102822
	typeSetSideBetsLimit = 241, //PYR-106798
	typeUsBlacklistNoSportMatched = 242, //PYR-113281
	typeUsBlacklistNoSportSuspisious = 243, //PYR-113281
	typeSecurity3DSTrigger = 244, // PYR-104952
	typeAutoAddNewCurrencyAccount = 245, // PYR-118290
};


// CHEQUESHISTORY.WHAT
enum eChequeHistoryUpdateType
{
	eFieldStatus = 1,
	eFieldCancelled = 2
};

enum eRevokeChatFlags
{
	eRevokeChatPriv	= 0x1,
//	ePutComment		= 0x2,
//	eSendMessage	= 0x3
};

enum eUserPropertyType
{
//	<<== @license2-cup-enum: Conversion user property enumeration @ ====
	eUserPropertyDotFrConverted = 19, 	// #14432
	eUserPropertyDotEeConverted = 24, 	// PYR-15751
	eUserPropertyDotBeConverted = 42, 	// PYR-18397
	eUserPropertyDotEsConverted = 49, 	// int - origLicenseId
	eUserPropertyDotDkConverted = 55, 	// int - origLicenseId
	eUserPropertyDotEuConverted = 57, 	// int - origLicenseId, str - source SiteIdCode for silent migration only
	eUserPropertyUsConverted = 68, 	// int - origLicenseId
	eUserPropertyDotUkConverted = 73, 	// int - origLicenseId
	eUserPropertyDotBgConverted = 75, 	// int - origLicenseId
	eUserPropertyToDESHConverted = 86, 	// PYR-33564. SH - Desktop migration, int - origLicenseId
	eUserPropertyDotPtConverted = 90, 	// PYR-38563. int - origLicenseId
	eUserPropertyDotRoConverted = 97, 	// PYR-43114. int - origLicenseId
	eUserPropertyDotCzConverted = 100, 	// PYR-47567 int - origLicenseId
	eUserPropertyDotInConverted = 110, 	// PYR-30559 int - origLicenseId
	eUserPropertyDotSeConverted = 130, 	// PYR-78227. int - origLicenseId
	eUserPropertyDotChConverted = 134, 	// PYR-89650. int - origLicenseId
	eUserPropertyDotArbaConverted = 137, 	// PYR-114733. int - origLicenseId
	eUserPropertyDotGrConverted = 140, 	// int - origLicenseId
//	==== @license2-cup-enum: Conversion user property enumeration @ ==>>

//	eLtFppQualification = 1,
	ePromoName = 2,			//	int - promo code source, str - promo code
	eQuestionnaire = 3,
	ePromotionCode = 4,
	eUruVerified = 5,
	eNegativeCategory = 6, // int - eClosedAccountCategories
//	eGracePeriodEnd = 7,
//	eLicenseDocs = 8,
	eUserPropertyCountryOfBirth = 9,
	eUserPropertyProvinceOfBirth = 10,
	eUserPropertyCityOfBirth = 11,
//	eUserPropertySex = 12, // obsolete #11771
	eUserPropertyNextUserId = 13,
	eLtFppAtLastDeposit = 14,
//	eEcheckGatewayAffinity = 15, // #10125 // obsolete after #13757
	eOriginalUserIntId = 16, // #11125
	eMoneyBookesAccount = 17, // #12111 int - CARDNUMS.CardNumId, str - acct number
	eNetellerCashoutAccount = 18, // #11754 int - CARDNUMS.CardNumId, str - acct number
//	eUserPropertyZZZConverted = 19,	// defined in automation script section 'license2-cup-enum' above
	eCleanedUpInactiveUser = 20, // #15428
	eEstoniaPersonalIDCode = 21, // #15236
	eEstoniaCertLastName = 22, // #15236
	eEstoniaCertFirstName = 23, // #15236
//	eUserPropertyZZZConverted = 24,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertySidLastModified = 25, // PYR-15984
	eTransferAffiliate = 26, // PYR-14504
	eUserPropertyRetainUserData = 27,	// PYR-15466 PYR-19318 prevents the user's data from blankout
	eLastAgeVerification = 28, // #15448
	eUserPropertyChallengeQuestion1 = 29, // PYR-16517
	eUserPropertyChallengeQuestion2 = 30, // PYR-16517
	eLastPepWatchScreening = 31, // #16866: str - Age verify table name, int - corresponding PK value, when - last time of screening
	eUserPropertyAAMSAccountStatus = 32, // PYR-16696: cache of AAMS account status
	eUserPropertyDocumentInfo = 33, // int-eDocumentTypeCat, str-doc_id, time-issue_date; PYR-16722: Italy: Collection of ID information on signup. PYR-19436 - passportId for Belgium, PYR-33390 Bulgaria: passport/driver data for non-residents, PYR-47569 Czech: id/passport/driver data, time: expiration date
	eUserPropertyDocumentIssuingAuthority = 34, // PYR-16722: Italy: Collection of ID information on signup, PYR-33390 Bulgaria: passport/driver issuer for non-residents
	eUserPropertyVipBarOptions = 35, // #16753: Serialized CommMsgBoby with user's Vip bar options
	eUserSuspension = 36, // #PYR-17878 - .BE - Suspend unverified accounts 30 days after first deposit
	eUserFastDepositInfo = 39, // #18174: str = PAYSYSTEM.STATUS(0,1).PROC_AMOUNT.PROC_CURRENCY; int = [PYR-24538 after 2013-02] CARDINFO.INFOID or GCDEBITDATA.GcDebitId (for ELV)
	eUserPropertyBelgiumAccount = 40, // PYR-18708: Belgium account info
	eEcheckAccount = 41, // #18737 ECHECK cashout account info
//	eUserPropertyZZZConverted = 42,	// defined in automation script section 'license2-cup-enum' above
    eUserPropertyProfession = 43, // PYR-19436
	eUserPropertySecondName = 44, // #20724 str
	eUserPropertySpainTaxRegion = 45, // #20724: str [00 .. 20]
	eUserPropertySpainPersonalIdType = 46, // #20724: int = enum eSpainUserIdentityType
	eUserPropertySpainDepositSelfLimitsQuiz = 47, // #20921 int 0 - not offered, 1 - passed, 2 - failed, when timestamp of quiz given; #23177 str - when not empty, toString(UINT32 answers), bit=0 - answer No, bit=1 - answer Yes
    eUserPropertyMaxTablesLimit = 48, // #20052 - maximum number of tables the player can sit at (real money)
//	eUserPropertyZZZConverted = 49,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertySpainMaxDailySelfLimit = 50,   // #20921 int - limit value, when - timestamp of approval
	eUserPropertySpainMaxWeeklySelfLimit = 51,  // #20921 int - limit value, when - timestamp of approval
	eUserPropertySpainMaxMonthlySelfLimit = 52, // #20921 int - limit value, when - timestamp of approval
	eUserPropertyCitizenship = 53, // int=0, str - Citizenship
	eUserPropertyCountryOfResidence = 54, // int=0, str - Country Of Residence
//	eUserPropertyZZZConverted = 55,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyIntentToPlay = 56, // #16725: int-eUserIntentToPlay, str-empty
//	eUserPropertyZZZConverted = 57,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyEulaSigned = 58, // int - licenseId, when - time of signature
	eUserPropertyTitle = 59, // #17003: int - eUserTitle (Mr, Ms, etc), str - empty
	eUserPropertyCprRejectCount = 60, //#22151 (.DK users) int - count, str - empty
	eUserPropertyFacebookLinkCode = 61, // #23923 int - linkCode, str - empty
	eUserPropertyConvertedFromFacebook = 62, // int - targetLicenseId
	eUserPropertyFullTiltPairingCode = 63, // #23771 string - code
	eUserPropertyLastSuccessfulNemIdCheck = 64, // #24151 when - timestamp, int=0, str - empty
	eUserPropertyConvertedFromDotNet = 65, // int - targetLicenseId
	eUserPropertryLocale2 = 66, // PYR-24001 int - second language
	eUserPropertryLocale3 = 67, // PYR-24001 int - third language
//	eUserPropertyZZZConverted = 68,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyUsNjMigrStart = 69, // int - MIGRATEDUSERS.MIGRATIONID, when - consolidated
	eUserPropertyConvertedToIom = 70, // int - origLicenseId, used in PYR-34786 as well
	eUserPropertyLifetimeDeposits = 71, // NJ only (not PA) // PYR-27262 int - total lifetime deposits, str - time of client acknowledgment, when - timestamp of last change 
	eUserPropertyArjelAccClosure = 72, // PYR-29386 int - val from eArjelAccCloseStatus, str - arjelUserId
//	eUserPropertyZZZConverted = 73,	// defined in automation script section 'license2-cup-enum' above
	eUserProperty3vRegister = 74, //PYR-26837 int - 3v register status, 0-unregistered, 1-pre-register, 2-fully-register
//	eUserPropertyZZZConverted = 75,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertySetupChallengeQuestionsRefused = 76,	//	PYR-28732.  when - timestamp of when the player last refused to setup their challenge questions
	eUserPropertyLastPwdChangeReminderRefused = 77,		//	PYR-30480.  when - timestamp of when the player last time refused to change password upon getting reminder
	eUserPropertyFirstLicensedDeposit = 78,                // PYR-30476 int - licenseId, str - str(transId)
	eUserPropertyUkLimitsWereSet = 79,  // PYR-30541: int - bit flags from eUserPropertyLimitsType, when - timestamp of last change
	eUserPropertyPayLuckyAccount = 80,  // PYR-31414: int - MasterCard CC_ID, str - MasterCard token
	eUserPropertyItalianDormantAccount = 81,  // PYR-31811
	eUserPropertyFrInactivityEmail = 82,  // PYR-33448: int - inactivity days reported, when - current timestamp
	eUserPropertyItInactivityEmail = 83,  // PYR-33374: int - inactivity seconds reported, when - current timestamp
//	eUserPropertyToDotConverted = 84, // incorrectly added, exists only on QA, not on production (was PYR-34786 - migrationId reverse migration to com)
	eUserPropertyRateMobileApp = 85,	//	PYR-33276. when - timestamp of when the player was shown the rate mobile app popup
//	eUserPropertyZZZConverted = 86,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyBirthName = 87,  //  PYR-33645 SH RM-form, int=0, str - birthName; PYR-56387 CZ: str - birth last name, int - eUserPropertySubmissionStatus
	eUserPorpertyDfsFirstUsage = 88, // PYR-37510
	eUserPropertyBGNraId = 89,       //PYR-37704 Identification number which Bulgarian regulator give gamer
//	eUserPropertyZZZConverted = 90,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyUsUserHash = 91,	// PYR-39487, PYR-115253. int - UserChanges.UpdateId , str - SHA1 hash
	eUserPropertySightlineAccount = 92,  // PYR-39651: int - token CC_ID (plain), str - Sightline token
	eUserPropertyCasinoRealityCheck = 93,  // PYR-35364: int - interval value in minutes (-1 - not set or not applicable; 0 - no reminder), str - empty, when - timestamp of last change
	//eUserPropertyFullTiltImageId	= 94, // never used in production PYR-41469 - imageId for FullTilt user in Pokerstars system used only when the user is logged in via FullTilt skin, int - imageId
	eUserPropertyRegulatorVerificationResult = 95, // PYR-48708 - license specific, for CZ: int - psErrCode (non-zero if communication/protocol error occurred), str - verification error code
	eUserPropertyVanillaReloadAccount = 96, // PYR-39936
//	eUserPropertyZZZConverted = 97,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyFiscalCodeAdditionalInfo = 98,	//	PYR-46195. For BG: int - fiscal code type (1 - UCN/EGN, 2 - PNF, 3 - PIN)
	eUserPropertyDocumentIssuingCountryCode = 99, // PYR-46621. Bulgaria: str - 2-char country code
//	eUserPropertyZZZConverted = 100,	// defined in automation script section 'license2-cup-enum' above
	eSpecialInstructions = 101, // #12424 // TODO: Will be obsolete after Dec,2011 build (PYR-18101)
	eUserPropertyTransactHistoryLastRequested = 102,		// #14235: ARJEL Events when - last requested timestamp, PYR-61891 str - copy of last requested timestamp before arjelReportPendingOfflineMessagesEnabled switched to true
	eUserPropertyVipManager = 103, // PYR-50010, int=0, str - adminUserId (vip manager of the user)
	eUserPropertyProfeeCardsAccount = 104, // PYR-47450, int - CC_ID, str - phone@maskedCardNumber@custId
	eUserPropertyProfeeQiwiAccount = 105, // PYR-47450
	eUserPropertyProfeeYandexAccount = 106, // PYR-47450
	eUserPropertyProfeeMobilepaymentAccount = 107, // PYR-47450
	eUserPropertyProfeeWalletAccount = 108, // PYR-47450
	eUserPropertyStarsWalletAccount = 109, // PYR-37328, int - CC_ID, str - loadingNumber
//	eUserPropertyZZZConverted = 110,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyIndiaLtApprAndPendDeposits = 111, // PYR-53001 int - total lifetime approved and pending deposits in INR, when - timestamp of last change 
	eUserPropertyOptionsMadeByUser = 112, // PYR-56207 int - bit mask of eOptionsMadeByUser flags, str - empty, when - timestamp of last change
	eUserPropertyConfiscationAmount = 113, // PYR-48917 int - Confiscation Amount value
	eUserPropertyBirthNumber = 114, // PYR-56387 CZ: str - birth number, int - eUserPropertySubmissionStatus
	eUserPropertyDataBoxIdentifier = 115, // PYR-56387 CZ: str - data box identifier, int - eUserPropertySubmissionStatus
	eUserPropertyStarsWalletRegMobileNumber = 116, // PYR-37328 str - mobile number used in StarsWallet Registration Dialog
	eUserPropertyMuchbetterAccount = 117, // PYR-61756 int - CC_ID, str - mobile number
	eUserPropertyFraudulentAccountReasons = 118, // PYR-69503 int - bit mask of eFraudulentAccountReasons flags, str - empty, when - timestamp of last change
	eUserPropertyUserRiskLevelAutomation = 119, // PYR-71294 int - user risk level
	eUserPropertyUserRiskLevelAdmin = 120, // PYR-71294 int - user risk level
	eUserPropertyVanillaDirectAccount = 121, // PYR-73312 int - CC_ID, str - accountId
	eUserPropertyVanillaDirectBarcode = 122, // PYR-73312 str - barcode
	eUserPropertyAsiaGamingCurrency = 123,	 // PYR-75040 str - currency
	eUserPropertyNGBelgiumAccount = 124,	// PYR-63165 str - new Belgium user token
	eUserPropertyBeUnvalidatedUser = 125,	// PYR-63165 when - timestamp when failed to validate Belgian user due to missing connection to Epis NG
	eUserPropertySessionPopupInterval = 126,  // PYR-77065: int - interval value in minutes (-1 - not set or not applicable; 0 - no reminder), empty means default value
	eUserPropertyDocumentInfoEx = 127,        // PYR-67030: when - expiration date of the document referenced in eUserPropertyDocumentInfo
	eLastRmfPassed = 128,                     // PYR-76448 int-licenseId, str - empty, when - timestamp of last RMF. Introduced for ES license only om Main.2018.09
	eUserPropertyBanByRegulatorReasons = 129, // PYR-77866: int - bit mask of eBanByRegulatorReasons, 'no property' means 'default reason', when - latest change of bitmask
//	eUserPropertyZZZConverted = 130,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyEulaSignedSochiClient = 131, // PYR-81985: same as eUserPropertyEulaSigned but for ru-sochi site ID; non-traditional special case for handling user logging in from Sochi Client
	eUserPropertySuspendedAccountCategory = 132, // PYR-83446 int - eSuspendedAccountCategory
	eUserPropertyRemovedPrivPlay = 133, // PYR-90560 license specific, for .UK: when - timestamp when privPlay was removed due to AgeVerif requirements
//	eUserPropertyZZZConverted = 134,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyTwitchToken = 135, // PYR-108724 - int - (1 - first connect, 0 - otherwise), str - token, when - last token update
	eUserPropertyFirstTwitchConnect = 136, // PYR-108724 - str - twitchId, when - time of first connection to Twitch
//	eUserPropertyZZZConverted = 137,	// defined in automation script section 'license2-cup-enum' above
	eUserPropertyRegulatoryDepositLimitStatus = 138, // PYR-113183: .BE int - eRegulatoryDepositLimitStatus, str - LIMITSAUDIT.requestId, when - timestamp of last change
	eUserPropertyLicLifeTimeDepositsTotal = 139, // PYR-114611: .ES int - total lifetime deposits in LicenseRules::getSelfDepositLimitCurrency, str - time of client acknowledgment, when - timestamp of last change 
//	eUserPropertyZZZConverted = 140-160,	// enumeration range reserved for CUP enumeration. Actual properties are defined via automation script section 'license2-cup-enum' above
	// !! when adding new constant to eUserPropertyType check [with BA] if it should be added to isDoubleUserProp() switch and getUserPropTypesToBePreseveredAtGdpr() and isUserPropTypeModifiableAfterGDPRReady()

	// #18028
	eClientCustomOptionsBegin = 200,
	eClientCustomOptionsEnd = 299,

	// TODO once userProperty grows over 299 then atf max limits for UserPropertyLegacy.propType/UserPropertyBody.propType should be changed
};

enum eUserPropertyLimitsType
{
	eUserPropertyLimitsType_Table = 1<<0,
	eUserPropertyLimitsType_Tourn = 1<<1,
	eUserPropertyLimitsType_Casino = 1<<2,
	eUserPropertyLimitsType_SportBooks = 1<<3,
	//eUserPropertyLimitsType_PokerDuel = 1<<4	//PYR-35875 // PYR-53789 - obsolete
};

// PYR-18101
enum eSpecialInstructionsType
{
	eSpecialInstructionsPokerRoom = 0,
	eSpecialInstructionsVipStore = 1,
	eSpecialInstructionsPaymentServices = 2, // for future use
	eSpecialInstructionsDAD = 3, // for future use
	eSpecialInstructionsVipManagement = 4, // PYR-50347
};

enum eUserPropertyAction
{
    eUserPropertyActionInsert = 0,
    eUserPropertyActionUpdate = 1,
    eUserPropertyActionDelete = 2,
};

enum eClosedAccountCategories
{
    eNormal         = 0,				// Meaning changed to "Obvious Duplicate Account" PYR-18496
	//									// According to http://dev.csr.pstars/wiki/index.php/Account_Closure_Reason it is "Closed Account was re-opened"
    eColluder       = 1,
    eChipDumper     = 2,
    eBotsUser       = 3,
    eMAA            = 4,
    eAffBonusAbuser = 5,//Meaning changed to "Affiliate Bonus Abuser" PYR-18496
    eFraud          = 6,
    eChargeback     = 7,
    ePermanent      = 8,//#13069 | Changed meaning to "Account Closed Request" PYR-18496
	/* PYR-18496 : Start */
	eAbusiveLanguage		= 9,
	eAcctCloseRequest25		= 10,
	eAAMSPlayerRequest		= 11,
	eMarketingBonusAbuse	= 12,
	eApprUseridChngReq		= 13,
	eChipDumpReciever		= 14,
	eClientHacking			= 15,
	eGameManipulationAbuse	= 16,
	eGamblingIssue			= 17,
	eHackCase				= 18,
	eLateRegAbuse			= 19,
	eMAAforAnonymity		= 20,
	eMAAforTourns			= 21,
	eAAMSRequest			= 22,
	eProhibitedRegion		= 23,
	ePkrRuleViolations		= 24,
	ePSformerStuff			= 25,
	eReceiverFraudTsfr		= 26,
	eRelationshipBarred		= 27,
	eResponsibleGaming		= 28,
	eProhibitedSWuse		= 29,
	eSuspeciousActivity		= 30,
	eUnderagePlayer			= 31,
	eAAMSDeceased			= 32,
	/* PYR-18496 : End */
	/* PYR-19984: Start */
	eEmailHarrassment		= 33,
	eManagementDecision		= 34,
	eOther					= 35,
	eUnderSecurityInvestigation	= 36,
	eDuplicateAccount		= 37,
	/* PYR-19984: End */
	eRofusRegistered		= 38,	//PYR-16706	
	eDkTempAccountClose		= 39,	//PYR-16740
	eFrArjelBlackListed		= 40,	//#21659
	eAAMSFailureToSendDocuments = 41,   //#27086
	eAAMSFraudSuspected     = 42,   //#27086
	eUsNjBlacklisted        = 43,   // PYR-27240
	eTSS			        = 44,   // PYR-26639
	eUnderInvestigationByPS	= 45,   // PYR-28409
	eFreemiumFraud	        = 46,   // PYR-28265
	eFrAccNotVerified       = 47,   // PYR-29441
	eFrAccSelfExcluded      = 48,   // PYR-29441
	eFrAccInactive          = 49,   // PYR-29441
	eChipDumperPMAutoClose	= 50,	// PYR-30223
	eItAccDormant			= 51,	// PYR-33374
	ePtDeactivated          = 52,	// PYR-38623
	eEuAccDormant			= 53,	// PYR-38474
	eNJCasinoEmployee       = 54,   // PYR-40782
	eFTConversion           = 55,   // PYR-39878
	eROTempAcctClosure		= 56,	// PYR-43675
	eFailureProvideDocs     = 57,   // PYR-47548
	eChargebackPM			= 58,	// PYR-48544
	eSecurityScorecard		= 59,	// PYR-51233
	eClosedAccountCategory_PtDeceased = 60,	// PYR-51033
	eNjAccDormant			= 61,	// PYR-48917
	eBioMatchAbuse			= 62,	// PYR-55171/PYR-58263
	eGdprDataProtection		= 63,	// PYR-58575
	eAsiaCDD				= 64,	// PYR-64085
	eGeolocMismatch			= 65,	// PYR-66110
	eBeRegistrationFailed	= 66,	// PYR-63165
	eFrAccDocMismatch       = 67,	// PYR-43398
	ePtIndefiniteSE			= 68,	// PYR-90121
	ePtDormant				= 69,	// PYR-90121
	eDeceased               = 70,   // PYR-90007
	eSwissAccClosedByGimo   = 71,   // PYR-94287
	eEsAccDormant			= 72,	// PYR-88251
	ePAAccDormant			= 73,	// PYR-72749
	eUsPaBlacklisted		= 74,	// PYR-112096

	// Please make sure to keep this value up-to-date
	eClosedAccountCategoryLast = eUsPaBlacklisted
};

//PYR-83446  - eUserPropertySuspendedAccountCategory
enum eSuspendedAccountCategory
{
	eSuspendedAccountCategory_None                      = 0,
	eSuspendedAccountCategory_AccountUnderInvestigation = 1,
	eSuspendedAccountCategory_SuspendedByCompliance     = 2,

	// new values to be added above this line
	eSuspendedAccountCategory_End // must be the last value
};

enum eBonusType // Bonuses2.RefId and UserBonus.BonusType
{
	eBonusType_Poker = 0, // aka release bonus
//	ePromotions = 1
	eBonusType_Roll  = 2, // aka instant bonus // PYR-31834 Bonuses2.RefId = Rolls.RollId; UserBonus.ObjectId = UserRolls.UserRollId
};

enum eMoneyHandleType
{
	eTransfer = 0,
	eCredit = 1
};

enum eDadContactType
{
	eDadNone = 0,
	eDadChat = 1,
	eDadEmail = 2,
	eDadPhone = 3
};

enum VipStatusLevel
{
	// These values are also used as power of 2 to produce a 32-bit VIP bit mask.
	// Do NOT use status value to compare it with another status! See VipRules class in viprules.h
	eVipUndefined = -1,
	eVipBase = 0,
	eVipSilver = 1,
	eVipGold = 2,
	eVipPlatinum = 3,
	eVipSuperNova = 4,
	eVipEliteSuperNova = 5,
	// PYR-24186
	eVipChrome = 8, // This level is actually between eVipBase and eVipSilver
	
	
	// PYR-32027
	// This is not a level but a constant that describes an actual max valid ID for a Vip Level
	// if a new vip level is added with an ID that's greater than eVipChrome - 
	// update: 
	//	eVipMaxId
	//	NUM_VIP_STATUS
	//	isValidVipStatusLevel
	eVipMaxId = eVipChrome
};

#define NUM_VIP_STATUS 7 // moved from scripts.h

inline bool isValidVipStatusLevel( INT32 lev )
{
	switch(lev)
	{
	case eVipBase:
	case eVipSilver:
	case eVipGold:
	case eVipPlatinum:
	case eVipSuperNova:
	case eVipEliteSuperNova:
	case eVipChrome:
		return true;
	default:
		return false;
	}
}

inline const char* getVipStatusDescr(UINT32 vipStatus)
{
	switch(vipStatus)
	{
	case eVipBase:				return "BronzeStar";
	case eVipChrome:			return "ChromeStar"; // PYR-24186
	case eVipSilver:			return "SilverStar";
	case eVipGold:				return "GoldStar";
	case eVipPlatinum:			return "PlatinumStar";
	case eVipSuperNova:			return "Supernova";
	case eVipEliteSuperNova:	return "SupernovaElite";
	default:					return "";
	}
}

// PYR-23129
inline UINT32 vipStatusLevelFlag(VipStatusLevel tp)
{
	switch(tp)
	{
	case eVipBase:
	case eVipSilver:
	case eVipGold:
	case eVipPlatinum:
	case eVipSuperNova:
	case eVipEliteSuperNova:
	case eVipChrome:
		return 1<<tp;
	case eVipUndefined:
	default:
		return 0;
	}
}

// PYR-23129
inline bool isVipStatusLevelSet(UINT32 flags, VipStatusLevel tp)
{
	return ( ( flags & vipStatusLevelFlag(tp) ) != 0 ) ;
}

enum FppStoreSpecial
{
	eFppStoreRegular = 0,
	eFppStoreTournTicket = 1,
	eFppStoreBonus = 2,
	eFppGiftCard = 3,
	eFppStoreRoll = 4, // PYR-31836
	eFppStoreGameTicket = 5, // PYR-45715
	eFppStoreSbFreeBet = 6, // PYR-41444
	eFppStoreTypeLast
};

//
// Fpp Store Order statuses
//
#define STORE_ORDER_STATUS_NEW			""
#define STORE_ORDER_STATUS_PROCESSED	"P"
#define STORE_ORDER_STATUS_MAILED		"M"
#define STORE_ORDER_STATUS_CANCELLED	"C"

enum FppStoreVertical // PYR-37076 bit mask
{
	eFppStoreVerticalPokerItem = 1 << 0, // 1
	eFppStoreVerticalCasinoItem = 1 << 1, // 2
	eFppStoreVerticalSportsItem = 1 << 2, // 4
};

enum AdminRightAction
{
	eAddRightRecord	   = 1,
	eRemoveRightRecord = 2,
	eCreateAdminRightRecord = 3,
	eUpdateAdminRightRecord = 4,
	eDeleteAdminRightRecord = 5,
	eAddRightOwner = 6,
	eRemoveRightOwner = 7,
	eAddGroupOwner = 8,
	eRemoveGroupOwner = 9
};

enum VectorSizeLimit
{
	eSmallVector = 0,
	eMediumVector = 1,
	eLargeVector = 2,
	eVectorSizeLimitSize
};

enum eTournTicketAction
{
	eTournTicketActionIssuedByAdmin = 1,
	eTournTicketActionIssuedTournPrize = 2,
	eTournTicketActionIssuedFppPurchase = 3,
	eTournTicketActionIssuedBonus = 4,
//	eTournTicketActionIssuedOnRegistr = 5, // RegActions removed in scope of PYR-39488
	eTournTicketActionIssuedSatelliteWinner = 6,
	eTournTicketActionIssuedExclusive = 7, // obsolete
	eTournTicketActionRegistered = 8, // adminIntId is tournId - this comment obsolete with PYR-25533
	eTournTicketActionSetUsedByAdmin = 9,
	eTournTicketActionSetUnusedByAdmin = 10,
	eTournTicketActionUpdatedExpDate = 11,
	eTournTicketActionRecalledWithBonus = 12,
	eTournTicketActionReturnedUnregTourn = 13,
	eTournTicketActionDeletedUnregTournExclusive = 14, // obsolete
	eTournTicketActionIssuedByIntelliPoker = 15,
	//eTournTicketActionIssuedForBattleOfThePlanets = 16, // obsolete PYR-120713
	eTournTicketActionAnnuledAtConvertion = 17,
	eTournTicketActionReturnedUnregSatelliteWinner = 18, // obsolete
	eTournTicketActionAnnuledAtForcedCashout = 19, // #14492
	//eTournTicketActionIssuedUsedExclusive = 20, // #15862 //PYR-46488 Deprecated in 2016.Main.10
	//eTournTicketActionCroppedFromRootUsed = 21, // #19360 //PYR-46488 Deprecated in 2016.Main.10
	eTournTicketActionCroppedFromRootUnused = 22, // #19360 - UNUSED (USED=NULL) tickets cropped by dbd after expiration
	eTournTicketActionIssuedByMileStoneEvent = 23, // PYR-28689
	eTournTicketActionIssuedForIceEvent = 24,	// PYR-29926
	eTournTicketActionAnnuledAtClosingAccount = 25, // PYR-31811 should NOT be added to bonusCancel
	eTournTicketActionDuelRegistered = 26, // PYR-36881 - registered for a poker duel (challenge or random) // PYR-53789 - obsolete
	eTournTicketActionDuelReturnUnreg = 27, // PYR-36881 - unregistered for a random poker duel (challenge or random) // PYR-53789 - obsolete
	eTournTicketActionMinigamePrize = 28, // PYR-37300 - player awarded tourn ticket as minigame prize
	eTournTicketAction_CancelledTPaidAndCroppedFromRoot_AtArch = 29, // PYR-38054
	eTournTicketAction_ForfeitedAndCroppedFromRoot_AtArch = 30,      // PYR-38054
	//eTournTicketAction_CroppedFromRootUsed_AtArch = 31,              // PYR-38054, PYR-46488 Deprecated in 2016.Main.10
	eTournTicketActionCardMatchPrize = 32, // PYR-39403 - player awarded tourn ticket as cardmatch prize
	eTournTicketActionFixedBountyPrize = 33, // PYR-31658 - player awarded tourn ticket as alt bounty prize
	eTournTicketActionBonusGamePrize = 34, // PYR-40872 - player awarded tourn ticket as bonusgame prize
	eTournTicketActionChestGameMiniGamePrize = 35, // PYR-45550 - player awarded tourn ticket as chest game prize
	eTournTicketActionSuitRacePrize = 36, // PYR-55866 - player awarded tourn ticket as suitrace prize
	eTournTicketAction_CancelledTPaidAndCroppedFromRoot_AtGdpr = 37, // PYR-64479
	eTournTicketAction_ForfeitedAndCroppedFromRoot_AtGdpr = 38,      // PYR-64479
	eTournTicketAction_CancelledTPaidAndCroppedFromRoot_Dormant = 39, // PYR-88251 - dormant account; surrender TT
	eTournTicketAction_ForfeitedAndCroppedFromRoot_Dormant = 40,      // PYR-88251
	eTournTicketAction_MPC_Chest_Prize = 41, // PYR-87284
	eTournTicketAction_RegisteredInProgressivePlus = 42, //PYR-118438
	eTournTicketAction_UnregisteredFromProgressivePlus = 43, //PYR-118438

// !! NOTE !! when adding new constant to eTournTicketAction ensure that 
//            getTTicketActionSign() and EsBotTournGetTournBalanceAndMovementsStmt include it into the logic
//			  it is added to the [TRANSACTION_TYPES] INIDATA section bonus[Cancel] item
//	new values MUST be LESS than a 1000 (PYR-19360)
};

enum eBanAction
{
	eBanActionBannedByAdmin = 1,
	eBanActionBannedOnLogin = 2,
	eBanActionClearedFromCaseOnly = 3,
	eBanActionClearedFromCaseAndUnbanned = 4,
	eBanActionAddedToCase = 5,
	eBanActionUpdatedInCaseOnly = 6,
	eBanActionBannedByCard = 7,
	eBanActionBannedByCountry = 8,
	eBanActionBannedByFailedPayment = 9,
	eBanActionBannedByOldBanSystem = 10, // Set by scripts only
	eBanActionBanCaseUpdated = 11,
	eBanActionBannedBySelfExclusion = 12,
	eBanActionBannedByAccountCreation = 13    // PYR-34667
};

enum eBanReasonCode
{
	eBanReasonCodeFraudRing					= 1,
	eBanReasonCodeChargeBack				= 2,
	eBanReasonCodeSelfExclusion				= 3,
	eBanReasonCodeBannedByCountry			= 4,
	eBanReasonCodeBannedByFailedPayment		= 5,
	eBanReasonCodeBannedByOldBanSystem		= 6,	// Set by scripts only
	eBanReasonCodeCollusion					= 7,
	eBanReasonCodeMAA						= 8,
	eBanReasonCodeAdminSelfExclusion		= 9,
	eBanReasonCodeChipDumping				= 10,
	eBanReasonCodeMoneyLaundering			= 11,
	eBanReasonCodeHacking					= 12,
	eBanReasonCodeGamblingProblem			= 13,
	eBanReasonCodeSuspiciousActivity		= 14,
	eBanReasonCodeAffiliateFraud			= 15,
	eBanReasonCodePromotionsFreerollAbuser	= 16,
	eBanReasonCodeOther						= 17,
	eBanReasonBotUser						= 18,
	eBanReasonUnderage						= 19,
	eBanReasonAbuseProfanity				= 20,
	eBanReasonBeValidation					= 21,	// PYR-18731
	eBanSuspectedFraud						= 22,	// PYR-21732
	eBanReturnsRtbtReversal					= 23,	// PYR-21732
	eBanSitRequestInformation				= 24,	// PYR-21732
	eBanReasonCodeFreemiumChargeBack		= 25,	// PYR-48544
	eBanReasonCodeVMAccountCreation			= 26,   // PYR-34667
};

enum eBanCaseStatus
{
	eBanCaseStatusActive = 1,
	eBanCaseStatusHold = 2,
	eBanCaseStatusCleared = 3
};

enum PaysystemsFields
{
	eAllowedCountries		= 1,
	eForbiddenCountries		= 2,
	eMinLtFpp				= 3,
	eMinVip					= 4,
	eUserFilter				= 5,
	eDeposited				= 6,
	eHideFlags				= 7,
	eShowOrder				= 8,
	eDaysToClear			= 9,
	eGroupId				= 10,
	eTrackCodes				= 11,
	eXorder					= 12,
    eLicenses               = 13,
	eFlags					= 14,
	eBrandsMask				= 15, // PYR-40411
	eForbiddenAppTypeMask   = 16, // PYR-73343
	ePaysystemsFields_CvlFilter = 17, // PYR-55099, PYR-85473
	eLicensesEx				= 18, // PYR-100877
};

enum ConsentStatus
{
	eConsentNotExisted	= 0,
	eConsentDeclined	= 1,
	eConsentAgreed		= 2
};

enum ePromoCodeSource
{
	eInstallPromo = 0,
	eTInfoManualPromo = 1,
	eTInfoPromoCode = 2,
	ePromoCodeSetByAdmin = 3,			//	PYR-29811
};

enum eRealTransferProcessing
{
	eRealTransferAutoOk = 1,
	eRealTransferManualOnly = 2,
	eRealTransferNotPossible = 3,
	eRealTransferExceedsLimitIn = 4
};

enum eFrozenObjectType
{
	eFrozenUndefined = 0,
	eFrozenInstall = 1,
	eFrozenMAC = 2,
	eFrozenIP = 3,
	eFrozenVolume = 4,
	eFrozenRouterMAC = 5, // PYR-30499
	//	When updating, also update handler for DBM_Q_LDBM_CHECK_FROZEN_OBJECT in loginDbm
};

enum eLiveSupportRequestType
{
	// PYR-52360
	// !! NOTE !! when adding new constants to eLiveSupportRequestType ensure that:
	// !! NOTE !! AgeVerificationLiveSupportHandler::validateELiveSupportRequestType() is changed (see ageverifylivesupport.cpp)
	eLiveSupportRequestChat = 0,
	eLiveSupportRequestPhone = 1,
	eLiveSupportRequestNone = 2,
	eLiveSupportRequestEmail = 3,
	eLiveSupportRequestIndirect = 4,
};

enum eAgeVerificationSystem
{
	eAgeVerificationSystemAristotle = 0,
	eAgeVerificationSystemURU = 1
};

enum eLiveSupportType
{
	eLiveSupportTypeUndefined = 0,
	eLiveSupportTypeDAD = 1,		// objectId = 0
	eLiveSupportTypeAristotle = 2,
	eLiveSupportTypeURU = 3,
	eLiveSupportTypeMigration = 4,	// #14432, objectId = 0
	eLiveSupportTypeRmIT = 5,		// #18620, objectId = errCode
};

// field types TICKETTYPECHANGES.UPDATEDFIELD
enum eTicketTypeFieldType
{
	eTicketTypeFieldCode = 1,
	eTicketTypeFieldAdmission = 2,
	eTicketTypeFieldMultiTurn = 3,
	eTicketTypeFieldAddChips = 4,
	eTicketTypeFieldExpDate = 5,
	eTicketTypeFieldExpPeriod = 6,
	eTicketTypeFieldObsolete = 7,
	eTicketTypeFieldValue = 8,
	eTicketTypeFieldFlags = 9,
	eTicketTypeFieldFppValue = 10,
	eTicketTypeFieldPmValue = 11,
};

// SECURITYALERTS.SOURCEMASK (#8158) - used in secdbmanager.cpp
enum eMAASourceMatchMask
{
    // first-level sources
    eMAASourceSameMAC           = 1<<0,
    eMAASourceSameInstall       = 1<<1,
    eMAASourceSameIP            = 1<<2,     // TRA only
    eMAASourceSameCC            = 1<<3,
    eMAASourceSameRMT           = 1<<4,
    eMAASourceSameAffiliate     = 1<<5,     // POSSIBLY AVAILABLE!     check if still in use
    eMAASourceSingleDeposit     = 1<<6,     // determined in OLAP
    eMAASourceMultiInstall      = 1<<7,     // users.installid not loggedout
    eMAASourceMultiFiscal       = 1<<8,     // users.fiscalcode
    eMAASourceSameVolume        = 1<<9,     // TRA only
    eMAASourceSameTournament    = 1<<10,    // TRA trigger
    eMAASourceAffiliateNoToYes  = 1<<11,
    eMAASourceMultiGovmtId      = 1<<12,
    eMAASourceMultiBankNumber   = 1<<13,
	eMAASourceSameRouterMac     = 1<<14,

	eMAASourceMaskAll           = (1<<16) - 1,

    // second-level matches
    eMAAMatchDOB                = 1<<16,
    eMAAMatchFullName           = 1<<17,
    eMAAMatchAddrCity           = 1<<18,
    eMAAMatchPassword           = 1<<19,
    eMAAMatchPhone              = 1<<20,
    eMAAMatchFirstName          = 1<<21,
    eMAAMatchZIP                = 1<<22,
    eMAAMatchLogins             = 1<<23,
    eMAAMatchEmail              = 1<<24,
    eMAAMatchLastName           = 1<<25 
};

// ATAS transtypes (#9846)
enum eATASTransType
{
    eATASPurchase               = 1,
    eATASRedeem                 = 2,
    eATASTransferSent           = 3,
    eATASTransferRecv           = 4
};

// security server notification types (#11242)
enum eSecurityNotificationType
{
    eSecurityNotificationPURCHASE   = 1,
    eSecurityNotificationREDEEM     = 2,
    eSecurityNotificationOVERCREDIT = 3
};

enum eExpireFppStatus
{
	eExpireFppStatusNotProcessed = 0,
	eExpireFppStatusProcessed = 1,
	eExpireFppStatusRestored = 2,
	eExpireFppStatusNoMatch = 3
};

enum eFlyerUpdateAction
{
	eFlyerUpdateActionSave = 1,
	eFlyerUpdateActionDelete = 2
};

enum ReceivedBy 
{ // Bug#: 8191
	AllTables		= 0,
	Tourney_This	= 1<<0,
	Tourneys_All	= 1<<1,
	Ring_All		= 1<<2,
	Ring_Holdem		= 1<<3,
	Ring_Omaha		= 1<<4,
	Ring_Stud		= 1<<5,
	Ring_HORSE		= 1<<6,
	Ring_Draw		= 1<<7,
/*	VIP_Bronze		= 1<<8,
	VIP_Silver		= 1<<9,
	VIP_Gold		= 1<<10,
	VIP_Platinum	= 1<<11,
	VIP_SuperNova	= 1<<12,
	VIP_Elite		= 1<<13,
//	TrackingCode	= 1<<14, --
//	AffiliateID		= 1<<15  -- */
};

enum eProcessPasswordAction
{
	eProcessPasswordActionEncrypt = 1 << 0,
	eProcessPasswordActionClearOpenText = 1 << 1
};

enum ePhoneValidation
{
	ePhoneValidationNotValidated = 0,
	ePhoneValidationValid = 1,
	ePhoneValidationInvalid = 2,
	ePhoneValidationNone = 3 // [AO 2008-09-29]
};

enum eTableLimitType //#12825, [AO 2010-03-29] #14033
{
	eTableLimitTypeAny	= 0, // obsolete, does NOT exist in ROOT.TABLELIMITS.TYPE (can NOT be renamed: used in client's code)
	eTableLimitTypeNL	= 1,
	eTableLimitTypeFL	= 2,
	eLimitTypeTourn		= 3,
	eLimitTypeSNG		= 4,
	eLimitTypeBuyIn		= 5, // see buyInLimitSupported
	eLimitTypePlayingTime = 6, // #15250. Daily playing (poker) time limit for .EE, Daily login time limit for .USNJ, .CZ, .SE. See enum eTimeLimitType
	//PYR-27249
	eLimitTypeBuyInDaily	= 7,
	eLimitTypeBuyInWeekly	= 8,
	eLimitTypeBuyInMonthly	= 9,
	// PYR-29581 casino limits
	eLimitTypeCasinoBlackjack = 10, // "twenty-one"
	eLimitTypeCasinoRoulette  = 11,
	eLimitTypeCasinoSlots     = 12, // one arm b
	eLimitTypeSportsStake     = 13, // PYR-32180
	eLimitTypeCasinoOther     = 14, // PYR-33582
	eLimitTypeSportsAccumulated = 15, // PYR-34388 - 7-day accumulated betting limit for .FR and .IT
	//eLimitTypePokerDuel         = 16, //PYR-35875 // obsolete (several old records still exist in ROOT.TABLELIMITS.TYPE)- PYR-53789

	//	PYR-47770 CZ license
	eLimitTypeNetLossPerDay					= 17,	//	Net loss per day, Live Games (Poker & Casino)
	eLimitTypeNetLossPerMonth				= 18,	//	Net loss per month, Live Games (Poker & Casino)
	eLimitTypeSpendingLimitPerDay			= 19,	//	Spending limit per day, Live Games (Poker & Casino)
	eLimitTypeSpendingLimitPerMonth			= 20,	//	Spending limit per monthly, Live Games (Poker & Casino)
	eLimitTypeSportsNetLossPerDay			= 21,	//	Net loss per day, sports
	eLimitTypeSportsNetLossPerMonth			= 22,	//	Net loss per month, sports
	eLimitTypeSportsSpendingLimitPerDay		= 23,	//	Spending limit per day, sports
	eLimitTypeSportsSpendingLimitPerMonth	= 24,	//	Spending limit per monthly, sports
	eLimitTypeLoginTotalLimitPerMonth		= 25,   //  Login total limit per month

	//	PYR-77066 SE license
	eLimitTypeLoginTimeWeekly				= 26,	//	Weekly login time limit
	eLimitTypeLoginTimeMonthly				= 27,	//	Monthly login time limit

	// PYR-106798
	eLimitTypeSideBet                       = 28,

	// PYR-105467 - CZ cool-off
	eLimitTypeLoginCoolOff					= 29,	//	PYR-105467 - Login cool off time between sessions in minutes - bigger value is stricter, unlike other limits.

	// do NOT skip numbers when adding new Types!
	// update getLimitTypeComment function
	numLimitTypes
};

enum eTableLimitStatus
{
	eTableLimitStatusPending = 0,
	eTableLimitStatusApproved = 1,
	eTableLimitStatusCancelled = 2
};

enum eUserUpdateBooleanAction
{
	eUserUpdateBooleanActionAutoAddress = (1 << 0), // PYR-33102
	eUserUpdateBooleanActionBGDocsValidated = (1 << 1), // PYR-46521
};

enum eUserUpdateType
{
	eUserUpdateTypeUndefined = 0,
	eUserUpdateTypeAddress = (1 << 0),
	eUserUpdateTypeEmail = (1 << 1),
	eUserUpdateTypePassword = (1 << 2),
	eUserUpdateTypeImage = (1 << 3),
	eUserUpdateTypeFullName = (1 << 4),
	eUserUpdateTypePhone = (1 << 5),
	eUserUpdateTypeRealMoney = (1 << 6), // body is empty
	eUserUpdateTypeFirstNameLastName = (1 << 7),
	eUserUpdateTypeFiscalCode = (1 << 8),
	eUserUpdateTypeProperties = (1 << 9),
	eUserUpdateTypeBirthday = (1 << 10),
	eUserUpdateTypeImageId = (1 << 11), // client never sends it
	eUserUpdateTypeSex = (1 << 12),
	eUserUpdateTypeMobile = (1 << 13),
	eUserUpdateTypeEstoniaValidation = (1 << 14),		// for user update for estonia users
	eUserUpdateTypeMailList = (1 << 15),				// PYR-15911: update no mail list
	eUserUpdateTypeExternalValidation = (1 << 16),		// PYR-16722: information required by italy for verification
	eUserUpdateTypeRealMoneyOkFlag = (1 << 17),		//PYR-16868 // only admin uses it, body is bool:[re]set
	eUserUpdateTypeSetLimits = (1 << 18),		// PYR-19467
	eUserUpdateTypeSMSPwdReset = (1 << 19),			// PYR-22184
	eUserUpdateTypeSmsMarketingFlag = (1 << 20), // PYR-23394
	eUserUpdateTypeEncrFiscalCode = (1 << 21), // PYR-27913 - never sent by client, only by server
	eUserUpdateTypeSwitchImageId = (1 << 22), // PYR-26568 - use already approved custom user image id or gallery image id. not affected by privChangeImage
	eUserUpdateTypePwdStrongAuth = (1 << 23),	//	PYR-32479 - NJ RM Strong Authentication required for password change
	eUserUpdateTypeMailListNew = (1 << 24), // PYR-27034
	eUserUpdateTypeBooleanActions = (1 << 25), // PYR-33102 see enum eUserUpdateBooleanAction
	eUserUpdateTypeLeaderBoard = (1 << 26), // PYR-33586
	eUserUpdateTypeInternalServerNonUpdInfo = (1 << 27), // PYR-42053 NOT filled by client, but by Server. Body: AppLoginId, BrandId, adminIntId
	eUserUpdateTypeAgeVerification          = (1 << 28), // PYR-90560 body: 4 - eUserUpdateRequestContext

	eUserUpdateTypeLast = eUserUpdateTypeAgeVerification,	//	PYR-39431. Must be kept up to date.

	// PYR-73732 - GDPR Personal Identity Information mask
	eUserUpdateType_GdprPersonalInfoMask = ( eUserUpdateTypeAddress		| eUserUpdateTypeEmail				| eUserUpdateTypeImage		| eUserUpdateTypeFullName	| 
											 eUserUpdateTypePhone		| eUserUpdateTypeFirstNameLastName	| eUserUpdateTypeFiscalCode | eUserUpdateTypeImageId	| 
											 eUserUpdateTypeBirthday	| eUserUpdateTypeMobile				| eUserUpdateTypeProperties ),

	// PYR-77866 - PT Personal Details mask
	eUserUpdateType_PtPersonalDetailsMask = (eUserUpdateTypeAddress | eUserUpdateTypeEmail | eUserUpdateTypeFullName |
		eUserUpdateTypePhone | eUserUpdateTypeFirstNameLastName | eUserUpdateTypeBirthday | eUserUpdateTypeMobile),

// consider to use eUserUpdateTypeBooleanActions before introducing new update type
// remember to update UserUpdate::toString1(), UserUpdate::parse(), UserUpdate::composeUpdateBody() function as well
};

enum eTlbBoardType
{
	// eBoPBoard = 0, // obsolete PYR-120713
	eLeagueBoard = 1
};

enum eAuthFlagsAction
{
	eAuthFlagsActionReset = 0,
	eAuthFlagsActionSetRsaRequired = 1,
	eAuthFlagsActionSetPspRequired = 2,
	eAuthFlagsActionSetTempPinOnEachLogin = 3,	//	PYR-76895

	eAuthFlagsActionLast = eAuthFlagsActionSetTempPinOnEachLogin,		//	please keep up-to-date
};

// ROLESHIERARCHY.FLAGS
enum eRolesHierarchyFlags
{
    eRolesHierarchyNone     = 0,    
    eRolesHierarchyTop      = (1<<0),
    eRolesHierarchyBranch   = (1<<1),
    eRolesHierarchyLeaf     = (1<<2),
};

enum eRefundType
{
	eRefundTypeTournRebuy = 1,
	eRefundTypeTournAddon = 2
};

enum eTextType
{
	eTextTypeReplay = 1,
	eTextTypeTournEmailBody = 2
	// NB!!! don't forget to add appropriate object name to dbmgenerators.cpp after adding another value here!
};

// authentication method used by user
enum eLoginAuthMethod
{
	eAuthUnknown			= 0,
	eAuthPasswordOnly		= 1,
	eAuthPokerStarsPin		= 2,
	eAuthRsaToken			= 3,
	eAuthOneTimePassword	= 4,
	eAuthLimitedLogin		= 5,
	eAuthWebToken			= 6, // PYR-23485
	eAuthWebTokenInt		= 7, // PYR-32891
	eAuthTempPin			= 8, // PYR-51853
	eAuthExternalOperator	= 9, // PYR-52518.	Client authentication done by external operator with operator's password.
	eAuthExternalOperatorToken = 10, // PYR-65464. Client authentication done by external operator with operator's token.
	eAuthTempPinEachLogin	= 11,	//	PYR-76895
};

enum eLoginSessionFlags
{
	//	eLoginSession... implies the flag is applicable to login session and app session
	//  appSession... implies the flag is applicable to app session only

	eLoginSessionExplicitLogin	= 1 << 0, // PYR-25561
	ipResolvedByMaxmind			= 1 << 1, // 2 // PYR-28594
	ipResolvedByGeoComply		= 1 << 2, // 4 // PYR-28594
	installIdChecked			= 1 << 3, // 8 // PYR-31259
	appSessionFlag_confirmed			= 1 << 4, // 16 // PYR-38296: used only by app session flags
	appSessionFlag_hasChildAppSession	= 1 << 5, // 32 // PYR-38296: used only by app session flags
	appSessionFlag_oldLoginInfoMappedToNewLoginInfo = 1 << 6, // 64 // PYR-38296: used only by app session flags
	eLoginSessionFingerprintLogin	 = 1 << 7,  // 128 // PYR-33139
	eLoginSessionDevicePasscodeLogin = 1 << 8,  // 256 // PYR-33139
	eLoginSessionEmailLogin = 1 << 9,			// 512 // PYR-40445
	eLoginSessionDoubleErrors = 1 << 10,		// 1024 // PYR-41301: failed in both logindbm & pwd
	appSessionFlag_firstSession = 1 << 11,		// 2048 // PYR-38917
	appSessionFlag_lastSession = 1 << 12,		// 4096 // PYR-38917
	eLoginSessionSilentLogin = 1 << 13,			// 8192 // PYR-52912
	appSessionFlag_supportsSimulLogin = 1 << 14,	//	16384 // PYR-52886
	appSessionFlag_sessionTransferAPC = 1 << 15,	//	32768 // PYR-52886. The Authenticated Poker Connection was transferred from another Poker session.
	eLoginSessionFlag_installId2IsPrimary = 1 << 16,	//	65536	//	PYR-52886. The installIds were swapped, using installId2 as the primary installId.
	appSessionFlag_embeddedWebComponent = 1 << 17,		//	131072	//	PYR-113227

	// Note: for appSessionFlags, the following flags are only added after the session is created (the rest is set when session is created)
	// installIdChecked, appSessionFlag_confirmed, appSessionFlag_hasChildAppSession. 
	// ipResolvedByGeoComply is not used by appSession.

};

enum eBlockedLoginFlags
{
	eBlockedLoginFlag_EmailLogin   = 1 << 0,		// 1 // PYR-40445
	eBlockedLoginFlag_DoubleErrors = 1 << 1,		// 2 // PYR-41301
};

// #11794: ban type - defines the type of message to be sent to the banned user on deposit/cashout
enum eBanTypeForUserInfo
{
	eBanTypeForUserInfoUnknown     = 0,
	eBanTypeForUserInfoGeneration0 = 1,
	eBanTypeForUserInfoCC          = 2,
	eBanTypeForUserInfoNonCC       = 3,
	eBanTypeForUserInfoMacOrInst   = 4,
};

// PYR-12977
enum eTournScriptAction
{
	eTournScriptActionUnknown = 0,
	eTournScriptActionCreated = 1,
	eTournScriptActionRecreated = 2,
	eTournScriptActionDeltaUpdated = 3,
	eTournScriptActionCloned = 4
};

// PYR-13041 PYR-8643 
enum eCashErrorGroupsFlag //bitmask
{
    eCashErrorGroupsAdminNeedAlert   = 1 << 0, //1
    eCashErrorGroupsAdminWasSent     = 1 << 1, //2
};

enum eTableAuditAction
{
    eTableAuditActionInsert = 0,
    eTableAuditActionUpdate = 1,
    eTableAuditActionDelete = 2,

    eTableAuditActionBulkInsert = 10,
    eTableAuditActionBulkUpdate = 11,
};

//PYR-46504
enum eCheckRelated
{
	eCheckNone = 0,
	eCheckRelated = 0x1,
	eCheckSuperRelated = 0x2,
	eCheckExtendedRelated = 0x04,

	//Update check all when adding new values here
	eCheckAll = eCheckRelated | eCheckSuperRelated | eCheckExtendedRelated
};

enum eAssetType // #14492
{
	eAssetTypeUndefined = 0,
	eAssetTypeChipsOnly = 1,
	eAssetTypeTChipsOnly= 2,
	eAssetTypeChipsAndT = 3,
	eAssetTypeFpp       = 4,
	eAssetTypeTickets   = 5,
	eAssetTypeBonus     = 6,
	eAssetTypeVipRewardBonus	= 7,
	eAssetTypeUserRolls	= 8, // PYR-50803
	eAssetTypeUncleared = 10,
	eAssetTypeNac2      = 11, // PYR-50803
};

enum eAssetConversionFlags // PYR-16354 + PYR-14492
{
	eAssetConversionFlagChips     = (1 << eAssetTypeChipsOnly ), //     2 // chips to preferred currency
	eAssetConversionFlagTChips    = (1 << eAssetTypeTChipsOnly), //     4 // T-chips to same currency chips
	eAssetConversionFlagFpp       = (1 << eAssetTypeFpp       ), //    16  // fpp to preferred currency
	eAssetConversionFlagTTicket   = (1 << eAssetTypeTickets   ), //    32  // Tourn Tickets to preferred currency
	eAssetConversionFlagBonus     = (1 << eAssetTypeBonus     ), //    64  // PYR-50803 partially clears unfinished release bonuses
	eAssetConversionFlagUserRolls = (1 << eAssetTypeUserRolls ), //   256  // PYR-50803 surrenders userrolls
	eAssetConversionFlagUncleared = (1 << eAssetTypeUncleared ), //  1024  // clears fpp-based uncleared funds
	eAssetConversionFlagNac2      = (1 << eAssetTypeNac2      ), //  2048  // PYR-50803 resets Biz Nac (Users.Nac2) to 0
};

enum eBinDetailsFlags // bits for BINDETAILS.FLAGS field
{
    eBinDetailsFlagPrepaidCard                 = 1 << 0, //  1	#14909: gift card
    eBinDetailsFlag3DSecureRequirementDefined  = 1 << 1, //  2	#16088: if(0) then the next bit is ignored and 3DS verif necessity is defined by country
    eBinDetailsFlag3DSecureRequired            = 1 << 2, //  4	#16088: is considered only if the previous bit is set to 1
	eBinDetailsFlagOvercreditDisabled          = 1 << 3, //  8	#16231: same logic as per obsolete CARDBINS.CFTFORBIDDEN transferred to this bit
	eBinDetailsFlagRollbackDisabled            = 1 << 4, // 16	#16365: rollbacks are not allowed on this card-bins
};

enum eFrMigrationCancelCode
{
	eFrMigrationCancelCode1 = 1,
	eFrMigrationCancelCode2 = 2,
	eFrMigrationCancelCode3 = 3,
	eFrMigrationCancelCodePCU = 4,

	eFrMigrationCancelCodeLast = eFrMigrationCancelCodePCU // please keep up-to-date
};

enum eEsMigrationCancelCode
{
	eEsMigrationCancelCode1 = 1,
	eEsMigrationCancelCode2 = 2,
//	eEsMigrationCancelCode3 = 3,

	eEsMigrationCancelCodeLast = eEsMigrationCancelCode2 // please keep up-to-date
};

// #17783
enum eTableFlags
{
	eTableFlagsNone			= 0,
	eTableFlagCustomName = 1 << 0, // #17783
	eTableFlagBlitzTable = 1 << 1,
	eTableFlagVoiceTable = 1 << 2, // #18513
    eTableFlagTbsTable   = 1 << 3, // 8 #17441 (to distinguish table started from table starter)
	eTableFlagFinalTournTable = 1 << 4, //16 PYR-71860
	eTableFlagHiddenTournTable = 1 << 5, //32 PYR-57246
};

enum eTrackingCodeStatus
{
	eTrackingCodePassive	= 0,
	eTrackingCodeActive		= 1
};

// #20134
enum eVipLevelAuditAction
{
	eVipLevelInserted = 1,
	eVipLevelUpdated = 2,
	eVipLevelDeleted = 3
};

// PYR-20489
enum eSuspendRmAction
{
	eSuspendRmActionNoChange = 0,
	eSuspendRmActionSet = 1,
	eSuspendRmActionReset = 2,
};

// CASHTRANS FLAGS (UINT64) #10874
// flags that may be set by DBM at cashtrans record insertion, rare - at update
#define CASHTRANS_FLAG_ACTIVE                         (ONE64 <<  0) // always set after #10874
#define CASHTRANS_FLAG_reserved1                      (ONE64 <<  1) // reserved for potential version control feature
#define CASHTRANS_FLAG_NEW_CASHOUT_LOGIC_obsolete     (ONE64 <<  2) // PYR-26312
#define CASHTRANS_FLAG_USER_HAS_CASHOUT_ONLY_FUNDS    (ONE64 <<  3) // #14614 [.FR client] with markedForCashoutOnly != 0
#define CASHTRANS_FLAG_USER_1ST_DEPOS_ANY_PS          (ONE64 <<  4) // set at UPDATE CASHTRANS
#define CASHTRANS_FLAG_USER_1ST_DEPOS_THIS_PS         (ONE64 <<  5) // set at UPDATE CASHTRANS
#define CASHTRANS_FLAG_USER_CANNOT_SPEND_UNCLEARED    (ONE64 <<  6) // user flags and privileges section
#define CASHTRANS_FLAG_USER_HAS_CLEARED_DEPOS_THIS_PS (ONE64 <<  7)
#define CASHTRANS_FLAG_USER_AUTHENTICATED             (ONE64 <<  8)
#define CASHTRANS_FLAG_USER_HAS_HOLD_CASHOUT          (ONE64 <<  9)
#define CASHTRANS_FLAG_USER_HAS_EVER_SELF_EXCLUDED    (ONE64 << 10)
#define CASHTRANS_FLAG_USER_HAS_OWED_FUNDS            (ONE64 << 11)
#define CASHTRANS_FLAG_USER_LAST_REJECT               (ONE64 << 12) // #10285 marks the last rejected overcredit
#define CASHTRANS_FLAG_USER_IS_INTERNAL               (ONE64 << 13) // either Admin or 'PS and related companies employees accounts' 
#define CASHTRANS_FLAG_2ND_GATEWAY                    (ONE64 << 14) // transaction was re-routed (reject-reroute-aprove)
#define CASHTRANS_FLAG_REPRESENTMENT_1ST              (ONE64 << 15) // representment transaction
#define CASHTRANS_FLAG_REPRESENTMENT_2ND              (ONE64 << 16)
#define CASHTRANS_FLAG_HAS_REVERSAL                   (ONE64 << 17)
//#define CASHTRANS_FLAG_CONVRATEBASE_100M_obsolete   (ONE64 << 18) // CONV_RATE_BASE was 100 Mil for this transaction; Bit NOT in use after 2013-08; Bit re-used after 2019.Build.04
#define CASHTRANS_FLAG_CONVRATE_HONORED               (ONE64 << 18) // PYR-95256 previous 'promised' convrate was honored (valid after 2019.Build.04)
#define CASHTRANS_FLAG_USER_DAD_ELIGIBLE              (ONE64 << 19) // #14357 set for deposits when user is eligible for DAD
// flags that may be updated by Admin
#define CASHTRANS_FLAG_CSR_INIT_CASHOUT               (ONE64 << 20)
#define CASHTRANS_FLAG_HAS_ADJUSTMENT                 (ONE64 << 21)
#define CASHTRANS_FLAG_PMTS_CASHOUT_RDY_TO_SEND       (ONE64 << 22)
#define CASHTRANS_FLAG_PMTS_free_to_use               (ONE64 << 23) // was 'ready to review', same as two neighbours are zero
#define CASHTRANS_FLAG_PMTS_CASHOUT_RDY_TO_REJECT     (ONE64 << 24)
#define CASHTRANS_FLAG_CSR_CHANGED_STATUS             (ONE64 << 25)
#define CASHTRANS_FLAG_CSR_MANUAL_MATCH				  (ONE64 << 26)
#define CASHTRANS_FLAG_CSR_MAP_FROM_U				  (ONE64 << 27)
#define CASHTRANS_FLAG_PMTS_COURIER_SEND			  (ONE64 << 28)
#define CASHTRANS_FLAG_REVERSAL_TRANS				  (ONE64 << 29) // PYR-28404

// these four bits are reserved to count REPRESENTMENTS and OVERCREDIT duplicates (#10285)
#define CASHTRANS_FLAG_COUNTER_1		              (ONE64 << 32)
#define CASHTRANS_FLAG_COUNTER_2		              (ONE64 << 33)
#define CASHTRANS_FLAG_COUNTER_3		              (ONE64 << 34)
#define CASHTRANS_FLAG_COUNTER_4		              (ONE64 << 35)

#define CASHTRANS_FLAG_USER_REPAID_OWED_FUNDS         (ONE64 << 36) // #14303
#define CASHTRANS_FLAG_3DSECURE_SHOULD_BE_SENT        (ONE64 << 37) // #16088 - 3D-secure is required for cashin transaction
#define CASHTRANS_FLAG_3DSECURE_WAS_SENT              (ONE64 << 38) // #16088 - gateway OKed to do 3DSecure and sent a url (client may still cancel)
#define CASHTRANS_FLAG_ACHBATCH_APPROVED              (ONE64 << 39) // #17160
#define CASHTRANS_FLAG_ACHBATCH_REJECTED              (ONE64 << 40) // #17160
#define CASHTRANS_FLAG_CASHIN_LIMIT_EXCEEDED          (ONE64 << 41) // #18177: [offline] cashtrans had 'N' status due to exceeding the deposit limits
#define CASHTRANS_FLAG_SAVE_INFO_FOR_FAST_DEPOSIT	  (ONE64 << 42) // #18174
#define CASHTRANS_FLAG_FAST_DEPOSIT			          (ONE64 << 43) // #18174
#define CASHTRANS_FLAG_REPEATED_DECLINES_IGNORED      (ONE64 << 44) // #17995
#define CASHTRANS_FLAG_RM_NOK_HOLD					  (ONE64 << 45) // #19393: cashtrans had 'N' status due to no RealMoneyOK users.flag
#define CASHTRANS_FLAG_GATEWAY_WAS_ASSIGNED			  (ONE64 << 46) // #19660
#define CASHTRANS_FLAG_EXCL_BANNED_CLOSED_HOLD		  (ONE64 << 47) // #18432 + #7667: cashtrans had 'N' status due to user in(cashBanned, selfExcl, acctClosed)
#define CASHTRANS_FLAG_VERYOLD_HOLD					  (ONE64 << 48) // #18432 + #10039: cashtrans had 'N' status due to the approved notification on very old transaction
#define CASHTRANS_FLAG_PAYSYSTEM_FEE_APPLICABLE		  (ONE64 << 49) // #22732
#define CASHTRANS_FLAG_PAYSYSTEM_FEE_CHARGED		  (ONE64 << 50) // #22732
#define CASHTRANS_FLAG_WEB_CASHIER					  (ONE64 << 51) // #23829
#define CASHTRANS_FLAG_RMOK_REMOVED_HOLD			  (ONE64 << 52) // #25200: cashtrans has/had 'N' status due to withdrawal of user's RealMoneyOK flag when user has balance
#define CASHTRANS_FLAG_SAVED_CARD_USED				  (ONE64 << 53) // #24538: transaction was done using paysystem with saved CARDINFO
// these two bits represent ccard bin type: Debit(01), Credit(10), Undefined(00) (#21794)
#define CASHTRANS_FLAG_CCARD_BINTYPE_1				  (ONE64 << 54) // 1 - Debit #21794
#define CASHTRANS_FLAG_CCARD_BINTYPE_2				  (ONE64 << 55) // 1 - Credit #21794
#define CASHTRANS_FLAG_SAVE_CARDINFO				  (ONE64 << 56) // #24538
#define CASHTRANS_FLAG_USER_1ST_DEPOS_OR_TRANSFER     (ONE64 << 57) // PYR-25462 1st external incoming money event extends bit 4
#define CASHTRANS_FLAG_AWARD_APPLIES				  (ONE64 << 58) // PYR-24840 Upsell
#define CASHTRANS_FLAG_HOSTILE_BIN_ATTEMPTED          (ONE64 << 59) // PYR-26564 attempt to process hostile bin
#define CASHTRANS_FLAG_AWARD_WAS_SUGGESTED			  (ONE64 << 60) // PYR-27353 Upsell was suggested (but may have been rejected)
// Add to CASHTRANS_FLAG only if you need Admin to update it. Otherwise add to CASHTRANS_FLAG2

#define FIRST_TRANSACTIONS_COUNTER_BIT 32 // !!! see CASHTRANS_FLAG_COUNTER_1
#define TRANSACTIONS_COUNTER_MASK (CASHTRANS_FLAG_COUNTER_1 | CASHTRANS_FLAG_COUNTER_2 | CASHTRANS_FLAG_COUNTER_3 | CASHTRANS_FLAG_COUNTER_4)
#define CASHTRANS_FLAGS_COPY_FROM_PARENT  (CASHTRANS_FLAG_CSR_INIT_CASHOUT | CASHTRANS_FLAG_USER_HAS_CASHOUT_ONLY_FUNDS | CASHTRANS_FLAG_WEB_CASHIER )
#define MAX_TRANSACTIONS_COUNTER	0xF

#define CASHTRANS_BITNUMBER_USER_HAS_CASHOUT_ONLY_FUNDS ( 3)
#define CASHTRANS_BITNUMBER_USER_1ST_DEPOS_THIS_PS		( 5)
#define CASHTRANS_BITNUMBER_USER_LAST_REJECT            (12) // #10285 marks the last rejected overcredit
#define CASHTRANS_BITNUMBER_HAS_ADJUSTMENT              (21)
#define CASHTRANS_BITNUMBER_PMTS_CASHOUT_RDY_TO_SEND    (22)
#define CASHTRANS_BITNUMBER_USER_REPAID_OWED_FUNDS      (36) // #14303
#define CASHTRANS_BITNUMBER_RM_NOK_HOLD					(45) // #19393: cashtrans has/had 'N' status due to no RealMoneyOK users.flag
#define CASHTRANS_BITNUMBER_PAYSYSTEM_FEE_CHARGED		(50) // #22732
#define CASHTRANS_BITNUMBER_USER_1ST_DEPOS_OR_TRANSFER  (57) // PYR-25462 1st external incoming money event extends bit 4
#define CASHTRANS_BITNUMBER_AWARD_APPLIES				(58) // PYR-24840 Upsell

// CASHTRANS FLAGS2 (UINT64) PYR-27712
#define CASHTRANS_FLAG2_LIFETIME_LIMIT_HOLD			  (ONE64 <<  0) // PYR-27262
#define CASHTRANS_FLAG2_CSR_INIT_TRANS_ON_USER_BEHALF (ONE64 <<  1)
#define CASHTRANS_FLAG2_HOLD_EXPIRED_RDY_TO_REJECT	  (ONE64 <<  2) // PYR-29092 
#define CASHTRANS_FLAG2_HOLD_EXPIRED_REJECTED		  (ONE64 <<  3) // PYR-29092
#define CASHTRANS_FLAG2_PLAYMONEY_RELATED			  (ONE64 <<  4) // PYR-29055 - all records in CASHTRANSPM
#define CASHTRANS_FLAG2_DO_NOT_RESEND_IF_FAILED		  (ONE64 <<  5) // PYR-29578
#define CASHTRANS_FLAG2_1ST_CARD_USE				  (ONE64 <<  6) // PYR-29872
#define CASHTRANS_FLAG2_1ST_PLAYMONEY_PURCHASE		  (ONE64 <<  7) // PYR-29992 successful approved PMP by the user without boughtPMchips, only 1 cashtrans can be marked as such
#define CASHTRANS_FLAG2_ONBOARDING					  (ONE64 <<  8) // PYR-30095
#define CASHTRANS_FLAG2_NO_SUCCESSFUL_PURCHASE_BEFORE (ONE64 <<  9) // PYR-30265 can be raised in both CASHTRANS and CASHTRANSPM tables many times until and including 1st successful PM and RM deposits
#define CASHTRANS_FLAG2_DEALS_DEPOSIT				  (ONE64 << 10) // PYR-27091 raised when a client provides a corresponding flag
#define CASHTRANS_FLAG2_CASHIN_REROUTED				  (ONE64 << 11) // PYR-34976 set when a deposit has been re-routed
#define CASHTRANS_FLAG2_WEB_CASHIER_VERSION_2         (ONE64 << 12) // PYR-39376
#define CASHTRANS_FLAG2_NOT_SHOW_IN_HISTORY			  (ONE64 << 13) // PYR-32116
#define CASHTRANS_FLAG2_HOSTILE_BIN                   (ONE64 << 14) // PYR-41694 process hostile CC bin in cashin request
#define CASHTRANS_FLAG2_CHARGEBACKED                  (ONE64 << 15) // PYR-29824 set on the ORIGINAL chargebacked transaction. Currently for PM only, jira for RM is pending.
#define CASHTRANS_FLAG2_CC_ID_VERIFIED                (ONE64 << 16) // PYR-47572 set if CC_ID+userIntID is present in VERIFIEDCARDS
#define CASHTRANS_FLAG2_AVS_ENABLED                   (ONE64 << 17) // PYR-44398
#define CASHTRANS_FLAG2_RMNOK_PRE_PURCHASE            (ONE64 << 18) // PYR-33629 stored in CASHTRANSPRE
#define CASHTRANS_FLAG2_RMNOK_PRE_PURCHASE_COPIED     (ONE64 << 19) // PYR-33629 stored in both CASHTRANS and CASHTRANSPRE for record copied from PRE to Main one
#define CASHTRANS_FLAG2_CONVRATE_x_100K               (ONE64 << 20) // PYR-54917 CASHTRANS.CONVRATE (only ConvRate, not fxRate) multiplier for this trans is 100K, instead of usual CONV_RATE_BASE=100M
#define CASHTRANS_FLAG2_PM_AMOUNT_OVER_2B             (ONE64 << 21) // PYR-49042 when flag is set 1M multiplier CASHTRANS_PM_MULTIPLIER applied to CASHTRANSPM.bonusFpp mapped to playMoneyPurchase
#define CASHTRANS_FLAG2_AUTOMATION_ORIGINATED         (ONE64 << 22) // PYR-66692 set when transaction created by request from automation tool
#define CASHTRANS_FLAG2_SOCHI_CLIENT_INITIATED        (ONE64 << 23) // PYR-81879 sochi-initiated cashtrans should have dedicated merchantId
#define CASHTRANS_FLAG2_CASH_OVERFLOW_HOLD      	  (ONE64 << 24) // PYR-77123, PYR-85287
#define CASHTRANS_FLAG2_3DS2_DEPOSIT                  (ONE64 << 25) // PYR-80344 3DS2 was used for the cashin

#define CASHTRANS_FLAGS2_COPY_FROM_PARENT  (CASHTRANS_FLAG2_CSR_INIT_TRANS_ON_USER_BEHALF | CASHTRANS_FLAG2_PLAYMONEY_RELATED | CASHTRANS_FLAG2_ONBOARDING | CASHTRANS_FLAG2_NO_SUCCESSFUL_PURCHASE_BEFORE | CASHTRANS_FLAG2_WEB_CASHIER_VERSION_2 | CASHTRANS_FLAG2_SOCHI_CLIENT_INITIATED | CASHTRANS_FLAG2_3DS2_DEPOSIT)

#define CASHTRANS_BITNUMBER2_1ST_CARD_USE				(6) // PYR-29872
#define CASHTRANS_BITNUMBER2_CHARGEBACKED				(15) // PYR-29824
#define CASHTRANS_BITNUMBER2_RMNOK_PRE_PURCHASE_COPIED  (19) // PYR-33629

enum USER_ADMINFLAGS
{
	USER_ADMINFLAG_reserved               = (1<<0),
	USER_ADMINFLAG_PMTS_NAME_CHECKED      = (1<<1), // #10424
	USER_ADMINFLAG_PMTS_ADDR_CHECKED      = (1<<2),
	USER_ADMINFLAG_PMTS_WIRE_ACCT_CHECKED = (1<<3),
	USER_ADMINFLAG_PMTS_rsrv_ACCT_CHECKED = (1<<4),
};

enum eExclStatesAction
{
    eExclStatesActionDelete = 0,
    eExclStatesActionInsert = 1,
    eExclStatesActionUpdate = 2,
};

enum eAgeVerifyResultFlags // #16866
{
	eAgeVerifyResultFlagsAuth            = (1   ),
	eAgeVerifyResultFlagsPepWatchRequest = (1<<1),
	eAgeVerifyResultFlagsPepResult       = (1<<2),
	eAgeVerifyResultFlagsWatchResult     = (1<<3),
	eAgeVerifyResultFlagsSilentRequest   = (1<<4), // filled after 2010-12-01 build
	eAgeVerifyResultFlagsTransferRecipient=(1<<5),
	eAgeVerifyResultFlagsAdminRequest    = (1<<6),
	eAgeVerifyResultFlagsAfterRmfRequest = (1<<7), // #17003
	eAgeVerifyResultFlagsNjRmfRequest    = (1<<8), // PYR-27240
	eAgeVerifyResultFlagsAfterAVformRequest	 = (1<<9), // PYR-90560
};

enum eUserCountersAuditFlags // USERCOUNTERSAUDIT.FLAGS
{
	eUserCountersAuditFlagLogin      = 1,
	eUserCountersAuditFlagLogout     = (1<<1),
	eUserCountersAuditFlagMigration  = (1<<2),
	eUserCountersAuditFlagExtUpdate  = (1<<3), // LimitController or Admin, not the gaming action
	eUserCountersAuditFlagLogConfirm = (1<<4),
};

// #18129
enum eDummyTournsAction
{
	eDummyTournsActionChangeStartTime = 1,
	eDummyTournsActionChangeMaxPlayers = 2,
};

// #20724
enum eSpainUserIdentityType // stored in USERPROPERTIES.PROPERTYINT for PROPERTYID=eUserPropertySpainPersonalIdType=46
{
	eSpainUserResidentUNDEFINED = 0,
	eSpainUserResidentNationalId = 1,
	eSpainUserNonResidentDriverLicense = 2,
	eSpainUserNonResidentIdCard = 3,
	eSpainUserNonResidentPassport = 4,
	eSpainUserNonResidentSSN = 5, // PYR-36821
	eSpainUserNonResidentOther = 6,
};
//constants TLB formula

#define TLB_FORMULA_ZERO	0
#define	TLB_FORMULA_ORIG	1
#define	TLB_FORMULA_JAN03	2

//constants for new broadcast flags (added after build 594)
//these will replace previous values in message MSG_LOBBY_ADMIN_BROADCAST
#define BROADCAST2_PLAY_DOT_COM	10
#define BROADCAST2_REAL_DOT_COM	11
#define BROADCAST2_ALL_DOT_COM	12
#define BROADCAST2_ALL_DOT_NET	13
#define BROADCAST2_ALL_DOT_ALL	14
#define BROADCAST2_ALL_DOT_ITA	15
#define BROADCAST2_ALL_DOT_FRA	16
#define BROADCAST2_ALL_DOT_EST	17
#define BROADCAST2_ALL_DOT_ESPN	18
#define BROADCAST2_ALL_DOT_FOX	19
#define BROADCAST2_ALL_DOT_BEL	20
#define BROADCAST2_ALL_DOT_SPA	21
#define BROADCAST2_ALL_DOT_DEN	22
#define BROADCAST2_ALL_DOT_EU	23
#define BROADCAST2_ALL_DOT_FB	24
#define BROADCAST2_ALL_DOT_UK	25
#define BROADCAST2_ALL_DOT_BG	26
#define BROADCAST2_ALL_DOT_POR	27
#define BROADCAST2_ALL_DOT_ROM	28
#define BROADCAST2_ALL_DOT_CZ	29
#define BROADCAST2_ALL_DOT_RUSO		30
#define BROADCAST2_ALL_DOT_RUSOPM	31
#define BROADCAST2_ALL_DOT_ASIA		32
#define BROADCAST2_ALL_DOT_SE		33
#define BROADCAST2_ALL_DOT_NJ		34
#define BROADCAST2_ALL_DOT_PA		35
#define BROADCAST2_ALL_DOT_CH		36
//	<<== @site2-broandcast2-enum: Site enumerations for BROADCAST2 message @ ====
#define BROADCAST2_ALL_DOT_ARBA		37	// PYR-117597
#define BROADCAST2_ALL_DOT_GR		38	// PYR-121068
#define BROADCAST2_ALL_DOT_USCO		39	// PYR-124292
#define BROADCAST2_ALL_DOT_DE		40	// PYR-124555
//	==== @site2-broandcast2-enum: Site enumerations for BROADCAST2 message @ ==>>

//constants for promotion state
#define PROMO_STATE_NOT_PUBLISHED	0  //promotion is not published
#define PROMO_STATE_BEFORE_RUN		1	// published but not in effect yet
#define PROMO_STATE_DURING_RUN		2	// in effect now
#define PROMO_STATE_AFTER_RUN		3	// not in effect anymore but still published

#define dbm_reqEmailsAll		0
#define dbm_reqAssingnedTo		1
#define dbm_reqStatus			2
#define dbm_reqEmailId			3
#define dbm_reqUserId			4
#define dbm_StatusAndAssigned	5
#define dbm_reqSetUser			6
#define dbm_reqSetAssign		7
#define dbm_reqSetStatus		8

//constants objectType for DBM_Q_USER_AUTH
#define	OBJ_TYPE_LOBBY		0
#define	OBJ_TYPE_TABLE		1
#define	OBJ_TYPE_MT_LOBBY	2
#define	OBJ_TYPE_CASHIER	3
#define OBJ_TYPE_CHATSERV	4
#define OBJ_TYPE_REPLAY		5
#define OBJ_TYPE_MONITOR	6
#define OBJ_TYPE_RBS		7
#define OBJ_TYPE_TMI		8
#define OBJ_TYPE_GC         9
#define OBJ_TYPE_NEWS		10
#define	OBJ_TYPE_COLLUSION	11
//#define OBJ_TYPE_ROUTER		12 // obsolete
#define OBJ_TYPE_OLAPMD		13
#define OBJ_TYPE_TEMPLSERV	14
#define OBJ_TYPE_CDMS       15
#define OBJ_TYPE_DBMPROXY   16
#define OBJ_TYPE_CRMSERVER  17
#define OBJ_TYPE_MONET		18 // obsolete
#define OBJ_TYPE_GAME_INT	18
#define OBJ_TYPE_AUTH		19
#define OBJ_TYPE_VIPSTORE	20
#define OBJ_TYPE_ATAS       21
#define OBJ_TYPE_SCHEDULER	22
#define OBJ_TYPE_AUTOMATION 23
#define OBJ_TYPE_M2NPROXY   24
#define OBJ_TYPE_BANSERVER  25
#define OBJ_TYPE_IDSSERVER  26
#define OBJ_TYPE_HG_LOBBY   27
#define OBJ_TYPE_COLOSSUS	28
#define OBJ_TYPE_REPORTDBM	29
#define OBJ_TYPE_HGDBMSERV  30
#define OBJ_TYPE_TSPROXY    31
#define OBJ_TYPE_LIMITCONTR 32  // Limit Controller
#define OBJ_TYPE_COLSCHED	33	// Colossus Scheduler
#define OBJ_TYPE_SECDBM  	34
#define OBJ_TYPE_WRDBM  	35	// Web Report DBM
#define OBJ_TYPE_BLITZ_LOBBY	36
#define OBJ_TYPE_AUTHRDR    37  // Authentication Redirector #19591
#define OBJ_TYPE_VOICECONTROL	38
#define OBJ_TYPE_VOICEDATA	    39  // Authentication Redirector #19591
#define OBJ_TYPE_QUOVAIP	40	// Quova IP Server #20584
#define OBJ_TYPE_IPBLOCKER 	41  // IP Blocker
#define OBJ_TYPE_AUTH_DK	42 // Special Auth server object for .DK users #16706
#define OBJ_TYPE_REGULATOR	43 // #21569
#define OBJ_TYPE_OLAPSAVER	44 // #21617
#define OBJ_TYPE_COLDBM		45	// Colossus Util DBM
#define OBJ_TYPE_ADMIN_AUTH	46	// Admin Auth server
#define OBJ_TYPE_MLOBBYPROXY	47 // Mobile lobby proxy
#define OBJ_TYPE_WEBCASHIER	48 // when web client cashier connects
#define OBJ_TYPE_UISTATS	49 // #21206
#define OBJ_TYPE_LOGSERVER	50 // #23995
#define OBJ_TYPE_WEBTOKENSERVER	51 // 24506
#define OBJ_TYPE_BLITZ_REPLAYER 52 // 24004
#define OBJ_TYPE_CRON		53	//27879
#define OBJ_TYPE_COLDATA	54	// #26321
#define OBJ_TYPE_LOBBY_ETC	55	// #25263
#define OBJ_TYPE_PITBOSS	56	// #31624
#define OBJ_TYPE_DEGAS		57  // #27213
#define OBJ_TYPE_PMCASHIER  58  // #26426
#define OBJ_TYPE_DGEENTROPY	59	// #27747
#define OBJ_TYPE_SAGLOBBY	60	// #28752
#define OBJ_TYPE_ICE_LOBBY	61	// #28693 
#define OBJ_TYPE_ICECLIENTPROXY 62 // PYR-29927
#define	OBJ_TYPE_BLOCK_ALL_EXCLUDE_CASHIER	63
#define _OBJ_TYPE_AUXHUB     64 // PYR-30319: (todo) Anton will decide what to do with this type that is over the bitmask boundary
#define _OBJ_TYPE_TABLE_SUBSCR_OVER_CONN	65
#define _OBJ_TYPE_SPORTSBOOK  66  // SBK-57  
#define _OBJ_TYPE_WEB_ACCOUNT_SERVICE 67 // PYR-32117
#define _OBJ_TYPE_RATIONAL_WEB_SERVICES 68 // PYR-33416
//#define _OBJ_TYPE_POKERDUEL_LOBBY 69			//	PYR-35000 // PYR-53789 - obsolete
//#define _OBJ_TYPE_POKER_NOTIF_SERVER 70 // PYR-35401
#define _OBJ_TYPE_DAILY_FANTASY_SPORTS 71  // PYR-36465
//#define _OBJ_TYPE_FRIEND_SERVER	72 // PYR-35600
#define _OBJ_TYPE_WEB_PULL_NOTIFICATIONS	73 // PYR-37887
#define _OBJ_TYPE_WEB_PUSH_NOTIFICATIONS	74 // PYR-37887
#define _OBJ_TYPE_APAL		75 // PYR-37587 // PYR-57632 - Renamed Orion to APAL
#define _OBJ_TYPE_MG_LOBBY 76			//	PYR-40426
#define _OBJ_TYPE_REGULATOR_FTP_MIGRATE 77		// PYR-40997
#define _OBJ_TYPE_AUX_SERVER 78		// PYR-41972: for admin login only
#define _OBJ_TYPE_SNGLOBBY	79	// #39272
#define _OBJ_TYPE_RCS	80	// 43217
#define _OBJ_TYPE_MLV	81	// 43027
#define _OBJ_TYPE_POOL_BETTING 82  // PYR-42410
#define _OBJ_TYPE_SEAT_FINDER	83	// PYR-43054
#define _OBJ_TYPE_WAGERMEAS		84  // PYR-46309
#define _OBJ_TYPE_COL_SHD		85  // PYR-46976
#define _OBJ_TYPE_GLOBAL_PUBLISHER	86  // PYR-52524
#define _OBJ_TYPE_BIOMAA		87		// PYR-55171
#define _OBJ_TYPE_COLUTIL_SERVER	88  // PYR-55078
#define _OBJ_TYPE_TCA_DETECTOR		89  // PYR-56474
#define _OBJ_TYPE_COL_CDA			90  // PYR-64013
#define _OBJ_TYPE_PREDICTOR			91  // PYR-63214, PYR-63110
#define _OBJ_TYPE_DEGASDBM			92  // PYR-66054
#define _OBJ_TYPE_COL_CCPD			93	// PYR-66732
#define _OBJ_TYPE_PERSONALIZATION	94 // PYR-66899
#define _OBJ_TYPE_HERECEPTION		95 // PYR-61352
#define _OBJ_TYPE_ASIACDD			96 // PYR-64085
#define _OBJ_TYPE_INSTANTHANDHISTORY_RETRIEVER 97 // PYR-72484
#define _OBJ_TYPE_ADMINBROKER		98 // PYR-80333
#define _OBJ_TYPE_CSI				99 // PYR-83394
#define _OBJ_TYPE_SOCIALCASINO		100 // PYR-77583
#define _OBJ_TYPE_STARSSTORE		101 // PYR-81644 
#define _OBJ_TYPE_ASIAB2B		    102 // PYR-105007
#define _OBJ_TYPE_PPPLOBBY			103 // PYR-114749
#define _OBJ_TYPE_HUBFEED			104 // PYR-124676
#define _OBJ_TYPE_REPA   			105 // PYR-123767
#define _OBJ_TYPE_POKER_DOJO		106 // PYR-126838

// NOTE! 1.) if new object type is subjected to web session/platform, please change "SessionInfoStruct::isWebSession" method accordingly!
//		 2.) add to composeUserAuthMsg() and parseUserAuthMsg() if it is client-facing module.

// !!! Do not add more object types before clearing with AR !!! (See LoginDbmServerObject::isObjectLoginRestricted())

enum ObjTypeFlags
{
	eObjTypeRealMoney				= (1 << 0),	//	1
	eObjTypeTournTable				= (1 << 1), //	2	//	Only applies to OBJ_TYPE_TABLE. 0 means ring game.
	eObjTypeEmbeddedWebComponent	= (1 << 2),	//	4	//	PYR-113227. Sent from WebTokenServer to LoginDbm
};

#define	GUESS_BASED_ON_EMAIL		0
#define	GUESS_BASED_ON_OTHER_EMAILS	1

//accidental logout types
#define LOGOUT_DISCONNECTED	(-1)
#define LOGOUT_CLEANUP		(-2)
#define LOGOUT_RECOVERY		(-3)
#define LOGOUT_ADMIN_FORCE	(-4)
#define LOGOUT_BLACKLIST_NOT_AVAILABLE (-5)		//	PYR-86054
#define LOGOUT_SESSION_TRANSFER	(-6)			//	PYR-86410

#define REAL_CHIPS			0
#define REAL_CHIPS_STR		"0"
#define PLAY_CHIPS			1
#define PLAY_CHIPS_STR		"1"
#define TOURN_CHIPS			2
#define TOURN_CHIPS_STR		"2"
#define BLITZ_REAL_CHIPS	3
#define BLITZ_REAL_CHIPS_STR	"3"
#define BLITZ_PLAY_CHIPS	4
#define BLITZ_PLAY_CHIPS_STR	"4"
#define POKER_DUEL_CHIPS	5
#define POKER_DUEL_CHIPS_STR	"5"

#define fullPlayerPrivileges	(privPlay|privChat|privShowImage|privChangeImage|privTimeoutAllIn)

enum UserApproval
{
	approveAll			= 1, 
	disapproveImage		= 2, 
	disapproveAll		= 3,
	disapproveImgDeny	= 4,
	disapproveUnclear	= 5,
	disapproveId		= 6,
	disapproveIdDeny	= 7
};

enum eFastDepositStatus // #18174
{
	eFastDepositStatusNotExists = -1,
	eFastDepositStatusDisabled = 0,
	eFastDepositStatusActive = 1,
};

enum eFastDepositCallFrom // #18487
{
	eFastDepositCallFromUnknown = 0,
	eFastDepositCallFromCashier = 1,
	eFastDepositCallFromRingGame = 2,
	eFastDepositCallFromTournament = 3,

	eFastDepositCallFromLast = eFastDepositCallFromTournament // please keep up-to-date
};

// bits in DBM_Q_MAA_DEEP_RESEARCH mask (#7897)
#define MAA_DEEP_RESEARCH_INSTALL           0x00001
#define MAA_DEEP_RESEARCH_MACADDR           0x00002
#define MAA_DEEP_RESEARCH_CARDS             0x00004
#define MAA_DEEP_RESEARCH_TRANSFER          0x00008
#define MAA_DEEP_RESEARCH_INSTALL_BYPARAM   0x00010
#define MAA_DEEP_RESEARCH_MACADDR_BYPARAM   0x00020
#define MAA_DEEP_RESEARCH_AFFILIATE         0x00040
#define MAA_DEEP_RESEARCH_AFFILIATE_BYPARAM 0x00080
#define MAA_DEEP_RESEARCH_FIRSTLOGIN        0x00100
#define MAA_DEEP_RESEARCH_SINGLEDEPOSIT     0x00200
#define MAA_DEEP_RESEARCH_FISCALCODE        0x00400
#define MAA_DEEP_RESEARCH_AFFILIATE_NOTOYES 0x00800
#define MAA_DEEP_RESEARCH_GOVERNMENT_ID     0x01000
#define MAA_DEEP_RESEARCH_FRENCH_BANK       0x02000
#define MAA_DEEP_RESEARCH_ROUTERMAC         0x04000
#define MAA_DEEP_RESEARCH_ROUTERMAC_BYPARAM 0x08000
#define MAA_DEEP_RESEARCH_FISCALCODEHASH	0x10000			// PYR-42943

// types of objects in EXCLUDEDOBJECTS and UNLIMITEDOBJECTS tables (#8688)
#define EXCLUDEDOBJECTS_TYPE_USERINTID      0
#define EXCLUDEDOBJECTS_TYPE_USERID         1
#define EXCLUDEDOBJECTS_TYPE_MACADDRESS     2
#define EXCLUDEDOBJECTS_TYPE_INSTALLID      3
#define EXCLUDEDOBJECTS_TYPE_IPADDRESS      4
#define EXCLUDEDOBJECTS_TYPE_AFFILINTID     5
#define EXCLUDEDOBJECTS_TYPE_AFFILID        6
#define EXCLUDEDOBJECTS_TYPE_FISCALCODE     7
#define EXCLUDEDOBJECTS_TYPE_GOVERNMENTID   8
#define EXCLUDEDOBJECTS_TYPE_FRENCH_BANK    9
#define EXCLUDEDOBJECTS_TYPE_ROUTERMAC		10
// Update the following as new types are introduced
#define EXCLUDEDOBJECTS_TYPE_MAX			10

#define	DEFAULT_PLAY_CHIPS_AMOUNT	100000 //in cents, default amount of play money

#define NO_TABLE_LIMIT_MARKER (-1)

// PYR-20301
// bitmask for Logindata.Flags, UINT64
#define nonUSFromUS			(ONE64 << 0) // country non-US, IP is US

//20 chars max!!!					|--------------------|
//#define PROP_NAME_ALIVE			"Alive"

//#define PROP_NAME_BONUS				"Bonus"
#define PROP_MAX_REAL_TABLES_AT			"MaxRealTablesAt"
#define PROP_MAX_PLAY_TABLES_AT			"MaxPlayTablesAt"
#define PROP_MAX_REAL_TABLES_AT_HU		"MaxRealTablesAtHU" // where HU is "heads-up"
#define PROP_RATHOLE_PERCENT			"RatholePercent"
#define PROP_RATHOLE_MINUTES			"RatholeMinutes"
#define PROP_USE_HISTORY_RESERVE		"UseHistoryReserve"		/// 0 - normal, 1 - reserve
#define PROP_RATHOLE_PERCENT_PM			"RatholePercentPM"
#define PROP_RATHOLE_MINUTES_PM			"RatholeMinutesPM"
#define SYSTEM_RESTART_TIME_PROP_ID		"SYSTEM_RESTART_TIME" // PYR-17774
#define PROP_POOL_BLOCK_RESET_TIME		"PoolBlockResetTime"  // PYR-42791
#define PROP_SITE_MAX_REAL_TABLES_AT	"SiteMaxRealTablesAt" // PYR-46390
#define PROP_FRESH_TABLES_GLOBAL_CAP	"FreshTableGlobalCap"
#define PROP_FRESH_TABLES_CAP			"FreshTableCap"
#define PROP_FRESH_TABLES_CAP_PERIOD	"FreshTableCapPeriod"
#define PROP_FRESH_TABLES_PB_THRESHOLD	"FreshTablePbThrshld"
#define PROP_FRESH_TABLES_CAP_EXPIRY	"FreshTableCapExpiry"
#define PROP_INDIA_CURRENT_TAX_YEAR	    "IndiaCurrentTaxYear" // PYR-54304 start year of India current annual taxation period. Example: 2017 start year for taxation period [2017/04/01-2018/03/31]. Will be changed to 2018 upon cashier scheduler will processing of annual tax event April 1st, 2018.
//20 chars max!!!						|--------------------|

//maximum length of user name limited by application
#define MAX_USER_NAME_LEN	15

//	Attention!!! when changin these - ...
//		... make sure table [Users] structure is changed accordingly!!!
#define	USER_NAME_LEN		21
#define MIN_USER_NAME_LEN	4
#define	USER_PWD_LEN		21
#define	USER_FULLNAME_LEN	322
#define	USER_EMAIL_LEN		101
#define	USER_ADDR_LEN		322
#define	USER_ADDR2_LEN		81
#define	USER_CITY_LEN		82
#define	USER_STATE_LEN		21
#define	USER_COUNTRY_LEN	21
#define	USER_ZIP_LEN		11
#define USER_INSTALLID_LEN	31
#define USER_COMMENTS_LEN	1001 //increased from 251, requires db update - script "0104.txt"
#define USER_CATEGORY_LEN	21
#define USER_SECUREID_LEN	21
//#define USER_PHONE_LEN		21
#define	RELATEDGROUP_NAME_LEN		51
#define	RELATED_COMMENT_LEN		251
//#define	USER_IMAGE_LEN		101
#define	USER_ENCRPWD_LEN		101
#define USER_FIRST_NAME_LEN	(201 + 1)
#define USER_LAST_NAME_LEN	(201 + 1)
#define USER_FISCAL_CODE_LEN	(33 + 1)
#define USER_SEX_LEN		(1 + 1)
#define USER_MOBILE_LEN		(20 + 1)
#define USER_IDEXTENSION_LEN (2 + 1) // PYR-29384

#define FT_PLAYERID_LEN (20+1)
#define FT_PWDHASH_LEN (28+1)
#define FT_EMAIL_LEN (100+1)

#define PHONENUMBERS_EXTRA_INFO_LEN	(20 + 1) // PYR-20319

// AAMS - moved from aamsprepstmt.h for PYR-21342
//
#define AAMS_TICKET_ID_LEN (20 + 1) // used for AAMSTRANS table PYR-35077: .IT Sports: AAMS History for Sports
#define ITGW_AAMS_TICKET_ID_LEN (16 + 1) // used for ITGW tables PYR-42381


// [AN 2010-03-15] PYR-13674 Store Data from the 'Contact Support' emails form

#define CLIENT_TO_SUPPORT_OS_LEN		65
#define CLIENT_TO_SUPPORT_VER_LEN		9
#define CLIENT_TO_SUPPORT_SUBJECT_LEN	257
#define CLIENT_TO_SUPPORT_CONTENT_LEN	3001

//	Attention!!! when changin these - ...
//		... make sure table [Tables] structure is changed accordingly!!!
#define	TABLE_NAME_LEN		31 // #17783
#define	DISP_NAME_LEN		21
#define TABLE_LOGLOCATION_LEN   61 // #21877

#define	GAMETYPE_NAME_LEN	21
#define	RAKE_FORMULA_LEN	21
#define	GAME_CONFIG_LEN		251
#define	TABLE_CONFIG_LEN	251

#define	IMAGE_DESCR_LEN		31

#define PRIZE_FORMULA_LEN	101

#define HANDS_BOARD_LEN				15

#define USER_IN_HAND_CARDS_LEN		15
#define USER_IN_HAND_COMB_HI_LEN	11
#define USER_IN_HAND_COMB_LO_LEN	11

#define HISTORY_CONTENTS_LEN		251

//#define	OBJECT_NAME_LEN				31 ////moved to "dbmgenerator.h"

//TABTYPES
#define	RAKE_LEN					3001
#define TABTYPES_PWD_LEN			21
#define TABTYPES_SPECNAME_LEN		41

//TABTYPEPROPS
#define TABLE_PROPERTY_LEN			3601

//CROSSGAMEBUTTONS
#define CROSSGAMEBUTTON_NAME_LEN		101

//CROSSGAMEBUTTONPROPS
#define CROSSGAMEBUTTON_PROPSTR_LEN		3601

//TOURNS
#define AWARDS_LEN					1025
#define TOURN_NAME_LEN				501
#define TOURN_PWD_LEN				21
#define TOURN_ADMISSION_LEN			51
#define HANDBYHAND_LEN				1025
#define DISCONMODE_LEN				101
#define TOURN_COUNTRIES_LEN			251
#define TOURN_COUNTRIES_TITLE_LEN	51
#define TOURN_FOUNDEDBY_LEN			21
#define TOURN_AFFILIATE_ID_LEN		21
#define TOURN_MSGBOARD_LEN			1001
#define TOURN_INTERNALREF_LEN		21		// PYR-12881
//#define MESSAGE_BOARD_LEN			1001
#define SAG_AWARDS_LEN				129

//PROPERTIES
#define PROP_ID_LEN		61 // increased from 21, requires db update - script "2008 0227.txt", #11310: 31->61
#define PROP_STR_LEN	3601 //increased from 1001, requires db update - script "2005 0310.txt"

//ACCOUNTS
#define ACCOUNT_NAME_LEN	21
//LOGGEDIN
#define MAC_ADDR_LEN		21
#define VOLUME_LABEL_LEN	13
#define INSTALL_ID_LEN		21
#define IP_ADDR_LEN			41 //big enough for IP6 support

// CARDNUMS
#define ENC_CC_LEN			(132+1) //encrypted cardnum widened from 102+1 in scope of PYR-63860
#define ENC_BIN_LEN			31
#define CARDNUM_HASH_LEN	(96 + 1)

#define NEWS_TEXT_LEN		16001 

///table "EMAILS":
#define EMAIL_ADDR_LEN			101
#define EMAIL_SUBJ_LEN			1001 
#define EMAIL_MSGID_LEN			80

//table "TRANSACTS"
#define TRANSACT_COMMENT_LEN	101

//table "CASHTRANS"
#define	TRANSTYPE_LEN		2
#define	CARDNUM_LEN			101
#define	CARDEXP_LEN			6
#define	CUSTID_LEN			(100+1) // widened from  50+1 in scope of PYR-63860
#define	FULLNAME_LEN		322
#define	MIDNAME_LEN			31
#define	LASTNAME_LEN		31
#define	USER_PHONE_LEN		101
#define	TRANS_STATUS_LEN	2
#define	ERRORINFO_LEN		257
//#define	RESULTINFO_LEN		101
#define	GATEWAY_LEN			21
#define	PAY_SYSTEM_LEN		21
#define	XTRANSSTR_LEN		(70 + 1) // Increased from 40 to 70 in scope of PYR-95749
#define	CARD_CVV_LEN		(20 + 1) // #14953
#define GATEWAYS_LEN		101
#define CURRENCY_LEN		4
#define ISSUE_NUM_LEN		11
#define START_DATE_LEN		11
#define PRODUCT_ID_LEN		21
#define CASHTRANS_ZIP_LEN	21
#define ERRCODE_LEN			129

// modified table "CASHTRANS" for Pausa project
#define	PAUSA_CASHTRANS_QUOVACOUNTRY_LEN	(10 + 1)		// #27096

//table cashdetails
#define	CASHDETAIL_TYPE_LEN			2
#define	CASHDETAIL_ACTION_LEN		2
#define	CASHDETAIL_REFERENCE_LEN	101
#define CASHDETAIL_STATUS_LEN		2

//table ChargeBacks
#define	CHARGEBACK_REASON_LEN		31
#define	CHARGEBACK_COMMENT_LEN		501

#define BANNED_COMMENT_LEN			251
#define USERBANS_COMMENT_LEN		BANNED_COMMENT_LEN

#define BANNEDCARDS_CARDNUM_LEN		CARDNUM_LEN
#define BANNEDCARDS_COMMENT_LEN		USERBANS_COMMENT_LEN

#define ADMIN_RIGHT_LEN				21

// table PROMOFPPS
#define PROMOID_LEN					21

// table TRANSPAIRS
#define PAIR_NAME_LEN				21
#define PAIR_VALUE_LEN				101

// table adminchat
#define ADMINCHAT_MESSAGE_LEN		2001    // PYR-21314: change from 1001 to 2001

// table USERCHANGES
#define USERCHANGES_VALUE_LEN		101

//table USERPROMOS
#define PROMO_NAME_LEN				31

//table CHEQUEHISTORY
#define CHQHIST_COMMENT_LEN			251
#define CHQHIST_VALUE_LEN			51

// table CHQAUDIT
#define WAYBILL_LEN					51

// table BONUSES2
#define	BONUS_NAME_LEN				101
#define	SPEC_CONDITIONS_LEN			(24 + 1)
#define	BONUS_URL_LEN				251
#define BONUS_DESCR_LEN				(2300 + 1) // PYR-45518
#define BONUS_COUNTRIES_LEN			(755 + 1) //#11301,PYR-23368,PYR-45518
//table USERCARD

#define USERCARD_COMMENT_LEN		251

// table GroupNames

#define GROUP_NAME_LEN				51
#define RIGHT_NAME_LEN				21
#define GROUPDISPLAYNAME_LEN		(100 + 1)
#define GROUPDESCRIPTION_LEN		(300 + 1)
#define RIGHTDISPLAYNAME_LEN		(100 + 1)
#define RIGHTDESCRIPTION_LEN		(300 + 1)

// table REGUSERPROMOS
#define REGPROMO_LEN				51

//#define PROPERTYSTR_LEN		1001

// table WUTRANSACTS
// obsoleted PYR-45735
/*
#define WU_CTRL_NUM_LEN		17
#define CR_CTRL_NUM_LEN		 5
#define WU_OPERATOR_LEN		21
#define WU_COMMENT_LEN		1001
#define WU_RECVR_NAME_LEN	51 // #7861
#define WU_USER_AMOUNT_LEN	17
*/

//table WUOFFICES
// obsoleted PYR-45735
/*
#define WU_OFFICES_COUNTRY_LEN	51
#define WU_OFFICES_CITY_LEN		51
#define WU_OFFICES_FORBIDDENCOUNTRIES_LEN 513
*/

//table STOREITEMS
#define FPP_ITEM_DESCR_LEN	101
#define FPP_ITEM_URL_LEN	251
#define FPP_PROP_NAME_LEN	31
#define FPP_PROP_VALUE_LEN	51
#define FPP_ITEM_FULLDESCR_LEN	2001
#define FPP_ITEM_COUNTRIES_LEN	1025

// inventory
#define INV_FPP_RET_REFNUM_LEN		51
#define INV_FPP_RET_REASON_LEN		256
#define INV_FPP_RET_COMMENT_LEN		256
#define INV_FPP_STOCKID_LEN			51
#define INV_FPP_WAREHOUSE_NAME_LEN	31
#define INV_FPP_WAREHOUSE_DESCR_LEN	101
#define INV_FPP_OFFICE_NAME_LEN		31
#define INV_FPP_COUNTRY_CODE_LEN	21
#define INV_FPP_USERID_LEN			21
#define INV_FPP_EMAIL_LEN			256
#define INV_FPP_EMAIL_NAME_LEN		202
#define INV_FPP_AUDIT_COMMENT		256
#define ALL_ITEMS					0
#define ALL_WAREHOUSES				0
#define ALL_COUNTRIES				"ALL"

//table STOREORDERS
#define ORDER_SPEC_LEN		51
#define ORDER_STATUS_LEN	2
#define	ORDER_WAYBILL_LEN	51
#define ORDER_COMMENT_LEN	401
#define ORDER_SHIPPER_LEN	101
#define ORDER_HIST_COMMENT_LEN	ORDER_COMMENT_LEN

//table SECUREVENTS
#define SECUREVENTS_CONTENTS_LEN	101

//table TOURNPROPS
#define TOURN_PROPERTY_LEN	   3601

//table HISTZIP
#define HIST_BODY_LEN		16001

/*
//table MESSAGES
#define MSG_BODY_LEN		16001
#define MSG_ERRDESCR		251
*/

//table REPLAYS
#define REPLAY_NAME_LEN			551    // PYR-50148: TOURN_NAME_LEN + 50
#define REPLAY_DESCR_LEN		1001
#define REPLAY_FILENAME_LEN		251
#define REPLAY_VISIBILITY_CRITERIA_LEN	101		//PYR-64944

//table reportrights
#define REPORT_NAME_LEN			31
/*
//table CARDBINS - obsolete
#define CARD_BIN_LEN			11
*/
#define COUNTRY_CODE_LEN		3

//table WURECEIVERS
#define	WURECVRID_LEN			31
#define	WURECVR_COMMENT_LEN		1001

//table scripts
#define SCRIPT_OBJ_NAME_LEN		31

//table UserBots
#define USER_BOT_COMMENT_LEN	251

//table userProperties
#define USER_PROPERTYSTR_LEN	202

//table userProperties
#define TGM_PROPERTYSTR_LEN		202

 // PYR-18101
//table SpecialInstructions
#define USER_INSTRUCTIONS_LEN	301
//table SpecialInstructionsAudit
#define USERINSTRUCTIONSAUDIT_COMMENTS_LEN      101


// table QualProps
#define QIALPROP_CHARID_LEN		21
#define QIALPROP_NAME_LEN		51

// table PaymentProps
#define PAYMENTPROP_CHARID_LEN		21
#define PAYMENTPROP_NAME_LEN		51
#define PAYMENTPROP_REWARD_LEN		501

#define BANHIST_COMMENT_LEN			251

//#define DAD_TRANS_COMMENT_LEN		101
//#define DAD_TRANS_PAYMENT_LEN		21

#define CASHIN_CATNAME_LEN			21
#define CASHIN_DISPNAME_LEN			31
#define CASHGROUP_NAME_LEN			21
#define BONUS_CODE_LEN				21
#define BONUS_PROMOCODE_LEN			31
#define BONUSCODESAUDIT_TBNAME_LEN	31


//table admmoneylimits
#define ADMMONEYLIMITS_NAME_LEN		31

// table ACH_DATA
#define SEC_NUM_LEN					101
#define DRIV_LICENSE_LEN			101

#define TOURNEMAIL_TITLE_LEN		81

// table GWSETTINGS
#define GWSETTINGS_COMMENT_LEN		101
#define	GWSETTINGS_CURRENCIES_LEN	256

//TrackCodes
#define TABLE_TRACKCODES_LEN		21
#define TABLE_TRACKDESC_LEN			101

//WireOrders
#define PAYMENTREF_LEN				21

//RegActions PYR-39488 removed

// table EXCLSTATES
#define EXST_COMMENT_LEN			101

// tables URUDETAILS & URURELSUTS
#define URU_AUTHID_LEN				51
#define URU_DETAIL_TYPE_LEN			2
#define URU_DETAIL_TEXT_LEN			256

// table AGE192DETAILS
#define AGE192DETAILS_NAME_LEN		(255 + 1)
#define AGE192DETAILS_VALUE_LEN		(1024 + 1)
/*
// table GCDEBITDATA
#define GCD_STATUS_LEN				2
#define GCD_BANKDATA_LEN			41
#define GCD_HOLDER_LEN				322
*/
// table GCDEBITHIST
#define GCD_CURRENCY_LEN			4
#define GCD_DEBTOR_LEN				31
#define GCD_REJECTID_LEN			4
#define GCD_REJECTREASON_LEN		(75 + 1)
#define GCD_REJECTIND_LEN			2
#define GCD_INTERNALID_LEN			100
/*
// table GCDEBITAUDIT
#define GCD_OLDVALUE_LEN			41
#define GCD_NEWVALUE_LEN			GCD_OLDVALUE_LEN
*/
// table SUBPAYTYPES
#define SPT_PRODUCTID_LEN			PRODUCT_ID_LEN
#define SPT_DESCRIPTION_LEN			51
#define SPT_COUNTRIES_LEN			101
#define SPT_CURRENCIES_LEN			101

//sitngo scripts
#define SITNGO_TITLE_LEN			61
#define SAG_PAYOUT_NAME_LEN			(128 + 1)
#define SAG_PAYOUT_LEVEL_AWARDS_LEN	(128 + 1)
#define SITNGO_FILE_NAME_LEN		(30+1)

//TournMessages
#define TOURNMESSAGE_LIST_LEN		1001

//PStarsTeam
#define DISPLAY_NAME_LEN			51
#define DISPLAY_COUNTRY_LEN			51

// COUNTRYIDS
#define ID_DESCRIPTION_LEN			50
// MATCHCODES
#define MATCHCODE_DESCR_LEN			250
// AGEVERIFYRESULTS
#define AGE_XTRANSSTR_LEN			51
#define COUNTY_LEN					3
#define GENDER_LEN					2
#define ID_MASKED_LEN				51
#define SSN_4_LEN					21

//AGEVERIFYCONTROL
#define USER_FILTER_LEN				81

/* MGDATA - obsoleted by #16411
#define PSID_LEN		21
#define MGID_LEN		21
#define MGRECEIVED_LEN	51
#define MGNAME_LEN		71
*/

//table PAYSYSTEMS2
#define PAYSYSTEMS2_ALLOWED_COUNTRIES_LEN				513
#define PAYSYSTEMS2_FORBIDDEN_COUNTRIES_LEN				PAYSYSTEMS2_ALLOWED_COUNTRIES_LEN
#define PAYSYSTEMS2_USERNAME_FILTER_LEN					(256 + 1)
#define PAYSYSTEMS2_PAYSYSTEM_TRACK_CODES_LEN			526
#define PAYSYSTEMS2_CVLFILTER_LEN						(256 + 1)

//table UNCLEARED
#define UNCLEARED_COMMENT_LEN				256
#define OVERRIDE_IP_COMMENT_LEN				256

// table TargetNews
#define TARGET_NEWS_NAME_LEN		51
#define TARGET_NEWS_BODY_LEN		8001
#define TARGET_NEWS_CRITERIA_LEN	3601
/*
// table MRMCACCOUNTS
#define MRMCDATA_ERRINFO_LEN		256
#define MRMCDATA_RETURL_LEN			301
*/
// table WIREACCOUNTS
#define WIREACCOUNTS_HOLDERNAME_LEN	322
#define WIREACCOUNTS_PHONE_LEN		51
#define WIREACCOUNTS_ADDR_LEN		322
#define WIREACCOUNTS_CITY_LEN		82
#define WIREACCOUNTS_STATE_LEN		21
#define WIREACCOUNTS_ZIPCODE_LEN	11
#define WIREACCOUNTS_COUNTRY_LEN	21
#define WIREACCOUNTS_IBAN_LEN		50
#define WIREACCOUNTS_SWIFTCODE_LEN	101
#define WIREACCOUNTS_BANKNAME_LEN	402
#define WIREACCOUNTS_BANKADDR_LEN	402
#define WIREACCOUNTS_CURRENCY_LEN	51
#define WIREACCOUNTS_MSKCARDNUM_LEN	51
#define WIREACCOUNTS_COMMENTS_LEN	501
#define WIREACCOUNTS_XINFO1_LEN		501
#define WIREACCOUNTS_XINFO2_LEN		501

//table WUREQUESTS
// obsoleted PYR-45735
/*
#define WUREQUESTS_TRSTATUS_LEN		11
#define WUREQUESTS_TRSTATE_LEN		51
#define WUREQUESTS_ERRMSG_LEN		256
#define WUREQUESTS_XTRANSID_LEN		21
#define WUREQUESTS_MCODE_LEN		21
#define WUREQUESTS_RCVRCODE_LEN		21
#define WUREQUESTS_RCVRNAME_LEN		51
#define WUREQUESTS_RCVRCITY_LEN		51
#define WUREQUESTS_RCVRSTATE_LEN	51
#define WUREQUESTS_RCVRCOUNTRY_LEN 101
*/

//table WUAUDIT
// obsoleted PYR-45735
// #define WUAUDIT_COMMENT_LEN			501

//table INIDATA
#define INIDATA_OBJECT_LEN			33	
#define INIDATA_SECTION_LEN			65
#define INIDATA_ITEMNAME_LEN		513
#define INIDATA_ITEMVALUE_LEN		1025
#define INIDATA_DESCRIPTION_LEN		513
#define INIDATA_COMMENT_LEN			513

#define INSERT_INIDATA				'I'
#define UPDATE_INIDATA				'U'

#define LIMIT_AUDIT_COMMENT_LEN		201
#define LIMIT_AUDIT_STATUS_LEN		2

//table PAYSYSTEMGROUPS
#define PSG_GROUP_NAME_LEN			51

//table DEPOSITLEVELS
#define DL_LEVEL_NAME_LEN			51
#define DL_PLIFORBIDDENCOUNTRIES_LEN (512+1) //#23340

//table Vendors
#define VENDORS_NAME_LEN	51
#define VENDORS_DESCR_LEN	501

//table ClaimCodes
#define CLAIMCODES_CLAIMCODE_LEN	129

//table Products
#define PRODUCTS_DESCR_LEN	501
#define PRODUCTS_CURRENCY_LEN	4
#define PRODUCTS_ALLOWEDCOUNTRIES_LEN 513

//table PAYMENTSAUDIT
#define PAYMENTSAUDIT_OBJNAME_LEN	21
#define PAYMENTSAUDIT_OBJID_LEN		21
#define PAYMENTSAUDIT_OLDVALUE_LEN	501
#define PAYMENTSAUDIT_NEWVALUE_LEN	501
#define PAYMENTSAUDIT_COMMENT_LEN	257

//table BANCASES
#define BANCASES_COMMENT_LEN		251

//table USERBANAUDIT
#define USERBANAUDIT_COMMENT_LEN		251

//table TINFO
#define TINFO_VERSION_LEN		6
#define TINFO_GUID_LEN		33
#define TINFO_TINFOTEXT_LEN		1025
#define TINFO_TINFO_RDN_LEN		(255 + 1)
#define TINFO_TINFO_GCLID_LEN	(255 + 1)
#define TINFO_TINFO_CLICKID_LEN	(22 + 1)

//table FROZENOBJECTS
#define FROZENOBJECTS_COMMENT_LEN		251

//table FREEZEEVENTS
#define FREEZEEVENTS_EXTRAINFO_LEN		101

//table LIVESUPPORTREQUESTS
#define LIVESUPPORTREQUESTS_COMMENTS_LEN	201

//table IPTRANSFERREQUESTS	
#define IPTRANSFERREQUESTS_FPPTRANSACTID_LEN	21
#define IPTRANSFERREQUESTS_COMMENTS_LEN	101

//table USERPROPAUDIT
#define USERPROPAUDIT_COMMENTS_LEN      101

//table TICKETTYPECHANGES
#define TICKETTYPECHANGES_VALUE_LEN      51

// table(s) SECURITYALERT[AUDIT] - moved to secdbm.h
// #define SECURITYALERTS_PARAM_LEN         101
// #define SECURITYALERTS_COMMENT_LEN       201

// table EXCLUDEDOBJECTS
#define EXCLUDEDOBJECTS_STRVALUE_LEN     41
#define EXCLUDEDOBJECTS_COMMENT_LEN      101

// table UNLIMITEDOBJECTS
#define UNLIMITEDOBJECTS_STRVALUE_LEN     41
#define UNLIMITEDOBJECTS_COMMENTS_LEN	  (1000+1)

//table EXPIREDFPP
#define EXPIREDFPP_REASON_LEN		21
#define EXPIREDFPP_COMMENT_LEN		201

//table CONSENTRULES
#define CONSENTRULES_RULESTR_LEN	251

//table TLBPRIZES
#define TLBPRIZES_COMMENT_LEN		201

//table BOPEXCLUDED // obsolete PYR-120713
//#define BOPEXCLUDED_EXCLUDEDWHY_LEN		201
//#define BOPEXCLUDED_REINSTATEDWHY_LEN	201

//table ADMINDOCS
#define ADMINDOCS_DATA_LEN			(3900 + 1)

//table FLYERS
#define FLYERS_TEXT_LEN				(3959 + 1) // #10998: 1024 -> 3959

//table VELOCITYCONTROL
#define COUNTRIES_LEN				(512 + 1)

//table TOURNNOTIFYMESSAGES
#define TOURNNOTIFYMESSAGES_TEXT_LEN		(3000 + 1)
#define TOURNNOTIFYMESSAGES_COUNTRYLIST_LEN	(100 + 1)
#define TOURNNOTIFYMESSAGES_TRACKCODES_LEN	(100 + 1)

//table CASHTRANSPROPS
#define CASH_PROPSTR_LEN					(512 + 1)

// table PROMODEALS
#define PROMO_DEAL_LEN				(40 + 1)
//table PROMODEALPROPS
#define PROMO_PROPERTY_LEN			(3600 + 1)

//table IMAGES
#define IMAGE_LEN 51200
//

// PYR-44930
//table SCHED_BROADCAST_MSG_PARAMS
#define SCHED_BROADCAST_MSG_PARAM_STR_LEN	   3601

//table CONVMARGINS
#define CONVMARGINS_COMMENT_LEN 101
#define CONVMARGINS_DEFAULT_CURRENCY	"000"

//table BINDETAILS
#define BINDETAILS_BIN_LEN              11
#define BINDETAILS_PAYSYSTEM_LEN        21
#define BINDETAILS_BINTYPE_LEN          51
#define BINDETAILS_ISSUERNAME_LEN       101
#define BINDETAILS_ISSUERREGION_LEN     11
#define BINDETAILS_ISSUERCONTACT_LEN    51
#define BINDETAILS_BANKNAME_LEN         402
#define BINDETAILS_BANKCONTACT_LEN      51
#define BINDETAILS_COMMENT_LEN          256
#define BINDETAILS_DATASOURCE_LEN       51
#define BINDETAILS_CARDCATEGORY_LEN     (40+1)
#define BINDETAILS_CARDBRAND_LEN        (40+1)

//table TEXTS
#define TEXT_DATA_LEN					(3600 + 1)

//table ErrorCodeGroups
#define ERRORCODE_GROUP_NAME_LEN			101
#define ERRORCODE_GROUP_DESCRIPTIPN_LEN		513

//table CASHOUTROUTINGRULES
#define CASHOUTROUTINGRULES_XMLDATA_LEN			(3900 + 1)

//table FRBANKACCOUNTS
#define FRBANKACCOUNTS_BANKNAME_LEN                    (401 + 1)
#define FRBANKACCOUNTS_ACCOUNTNUMBER_LEN               (100 + 1)
#define FRBANKACCOUNTS_MASKED_ACCOUNTNUMBER_LEN        (60  + 1)
#define FRBANKACCOUNTS_COMMENTS_LEN     (200 + 1)
#define FRBANKACCOUNTS_BANKCOUNTRY_LEN  (20 + 1)

// table PROMOIMAGES
#define PROMOIMAGES_IMAGECODE_LEN		(4  + 1)
// PYR-23045
#define PROMOIMAGESAUDIT_TABLE	"PromoImagesAudit"

// table FRACCVALIDATIONAUDIT
#define FRACCVALIDATIONAUDIT_VALUE_LEN		(32 + 1)
#define FRACCVALIDATIONAUDIT_COMMENT_LEN	(300 + 1)

// table BEACCVERIFICATIONAUDIT
#define BEACCVERIFICATIONAUDIT_VALUE_LEN		(32 + 1)
#define BEACCVERIFICATIONAUDIT_COMMENT_LEN		(300 + 1)

// table ITACCVERIFICATIONAUDIT // PYR-18621
#define ITACCVERIFICATIONAUDIT_VALUE_LEN		(32 + 1)
#define ITACCVERIFICATIONAUDIT_COMMENT_LEN		(300 + 1)

// table ACCVERIFICATIONAUDIT // PYR-20730
#define ACCVERIFICATIONAUDIT_VALUE_LEN		(32 + 1)
#define ACCVERIFICATIONAUDIT_COMMENT_LEN	(300 + 1)

// table ACMCHATVIOLATIONS  // PYR-28673
#define ACM_CHAT_CONTENT_LEN                (250 + 1)
#define ACM_ACTION_REMARKS_LEN              (64 + 1)

//table GEOIPOVERRIDE
#define STATE_CODE_LEN		(16 + 1)		//	PYR-69448

//table LANGPATTERNS
#define LANGPATTERNS_PATTERN_LEN		(64 + 1)

// table ACCOUNTCREATESTATS // PYR-17950
#define USER_ACCOUNT_REGISTER_PAGE_NAME_LEN (40 + 1) 

// table LOGINEVENTSAUDIT
#define LOGINAUDIT_COMMENTS_LEN (100 + 1) // PYR-20301

// table VIPLEVELS
#define VIPLEVELS_STATUSNAME_LEN (64 + 1) // PYR-20134

// table BLITZSESSIONS
#define BLITZSESSIONS_SESSIONNAME_LEN (200 + 1) // #20582

// table HANDHISTORYAUDIT
#define HANDHISTORYAUDIT_PARAM_LEN (300 + 1) // PYR-17411
#define HANDHISTORYAUDIT_CRX_LEN (10 + 1) // PYR-17411

// table DAILYBUYIN
#define DAILYBUYIN_STATUS_LEN (1 + 1) // PYR-21307

// table MBOOKERSCASHVOUCHER
#define MBOOKERSCASHVOUCHER_URL_LEN (250 + 1) // PYR-22585

// table YANDEXTRANSREQTIME
#define YANDEXREQUESTDT_LEN (25 + 1) // PYR-22713

// table ESBOTREPORTDAILY
#define CNJ_GAME_TYPE_LEN (3+1)  // #22093 

//	PYR-42113
#define REGION_CODE_LEN (10 + 1)
#define REGION_NAME_LEN (64 + 1)
#define REGION_COMMENT_LEN (100 + 1)

// table IPADDRESSAUDIT
#define IPADDRESSAUDIT_TABLE "IPADDRESSAUDIT"
#define IPADDRESSAUDIT_ACTION_ADD "A" // added new
#define IPADDRESSAUDIT_ACTION_REMOVE "R" // removed, no longer used
#define IPADDRESSAUDIT_ACTION_LEN (1 + 1) // PYR-21739

// table FTMESSAGES
#define FTMESSAGES_ERRINFO_LEN (200 + 1)
// table FTPAIRING
#define FTPAIRING_FTUSERID_LEN (16 + 1)
#define FTPAIRING_FTEMAIL_LEN (64 + 1)
#define FTPAIRING_FTFirstName_LEN (75 + 1)
#define FTPAIRING_FTLastName_LEN (75 + 1)
#define FTPAIRING_FTFullName_LEN (152 + 1)
#define FTPAIRING_FTADDRESS_LEN (294 + 1)
#define FTPAIRING_FTCITY_LEN (110 + 1)
#define FTPAIRING_FTSTATE_LEN (75 + 1)
#define FTPAIRING_FTCOUNTRY_LEN (2 + 1)
#define FTPAIRING_FTZIP_LEN (50 + 1)
#define FTPAIRING_FTPHONE_LEN (16 + 1)
#define FTPAIRING_FTIPADDR_LEN (40 + 1)
#define FTPAIRING_FTBRAND_LEN (3 + 1)

#define APPKEY_LEN	(64 + 1) // PYR-88312 tables MOBILEHARDWARE, MOBILEHARDWARE2, MOBILEHARDWAREAUDIT, MOBILEHARDWAREAUDIT2

//	table MOBILEHARDWARE
#define MOBILE_DEVICE_TOKEN_LEN		 (64 + 1)		//	PYR-24587, for iOS

//	table MOBILEHARDWARE2
#define MOBILE_DEVICE_TOKEN_ANDROID_1_LEN	 (1000 + 1)		//	PYR-24967, for Android
#define MOBILE_DEVICE_TOKEN_ANDROID_2_LEN	 (7192 + 1)		//	PYR-24967, for Android

// table ZOOMREPLAYCHAT
#define ZOOM_CHAT_CONTENT_LEN		(250 + 1)	// PYR-24004

// table PLAYCHIPSCONFIG
#define	PLAYCHIPSCONFIG_EXTREFID_LEN	(40 + 1) // PYR-26426

// table NJBLACKLIST alias USBLACKLIST
#define USBLACKLIST_FIRSTNAME_LEN ( 200 + 1 )
#define USBLACKLIST_LASTNAME_LEN  ( 200 + 1 )
#define USBLACKLIST_ALIASES_LEN   ( 200 + 1 )
#define USBLACKLIST_SSN4_LEN      (  20 + 1 )
#define USBLACKLIST_COMMENT_LEN   ( 200 + 1 )
#define USBLACKLIST_REFNO_LEN	  (  20 + 1 )

// table GEOCOMPLYRESPONSE
#define GEOCOMPLY_RESPONSE_XML_LEN		( 8096 + 1 )
#define GEOCOMPLY_RESPONSE_ERRORMSG_LEN	( 64 + 1 )

// table ADXEVENT
#define ADXEVENT_NETWORK_LEN (64 + 1)
#define ADXEVENT_REFERENCE_LEN (64 + 1)
#define ADXEVENT_DEVICE_ID_LEN (40 + 1)
#define WEB_ID_LEN (7 + 1)

// table EsUsersIdAndBanCheck
#define ESUSERSCHECK_CNJSTATUS_LEN		(5+1)
#define ESUSERSCHECK_CHECK_RESULT_LEN	(10+1)
#define ESUSERSCHECK_CHECK_DESCR_LEN	(200+1)

// table ASTROPAYDIRECTBANKS, ASTROPAYDIRECTUSERBANKREF
#define ASTROPAYDIRECT_BANKCODE_LEN (10 + 1) // PYR-56075 length 3->10
#define ASTROPAYDIRECT_BANKNAME_LEN (45 + 1)
#define ASTROPAYDIRECT_BRANCHNAME_LEN (45 + 1)
#define ASTROPAYDIRECT_NICALIAS_LEN (10 + 1) // PYR-56075 length 3->10
#define ASTROPAYDIRECT_NICVALUE_LEN (30 + 1)
#define ASTROPAYDIRECT_PAYMENTCODE_LEN (2 + 1)
#define ASTROPAYDIRECT_ACCTNUM_LEN (45 + 1)
#define ASTROPAYDIRECT_ACCTTYPE_LEN (1 + 1)
#define ASTROPAYDIRECT_BENEFICIARY_LEN (70 + 1)
#define ASTROPAYDIRECT_BENEFICIARY_ACCT_LEN (70 + 1)

// PYR-46043
//table "PAYWALL/PAYWALLSEGMENT/PAYWALLBUNDLE"
#define	PAYWALL_NAME_LEN		101
#define	PAYWALL_DESC_LEN		3601
#define	SEGMENT_DESC_LEN		3001
#define ADMIN_NAME_LEN			101
#define BUNDLE_IMAGE_NAME		3601
#define PAYWALL_PARAM_STR_LEN	3601



// table Signals (PYR-35623)
#define SIGNALID_LEN	(15 + 1)
#define APPSFLYERID_LEN (64 + 1)

// table USERSINHAND_STORM (PYR-38782)
#define STORM_STARTING_POWERS_LEN (255 + 1)

#define FTBLOCKED_COMMENTS_LEN	(512+1) // PYR-41600
#define FTUNBLOCKED_COMMENTS_LEN	(512+1) // PYR-41600

//	PYR-41017
#define PASSWORD_RESET_TOKEN_LEN ( 80 + 1)

// table WEBCLIENTINFO (PYR-33126)
#define WEBCLIENTINFO_BROWSER_LEN  (128+1)
#define WEBCLIENTINFO_OSDETAIL_LEN (256+1)
#define WEBCLIENTINFO_PLAYLANG_LEN (128+1)

// table XCJACKPOTS (PYR-49208)
#define XCJACKPOTS_JPNAME_LEN			(30+1) // Must correspond to column sizes in XC database
#define XCJACKPOTS_VENDORJACKPOTID_LEN	(128+1) // Must correspond to column sizes in XC database

// table BDUSERATTRIBUTES (PYR-54580)
#define CONTAINER_LEN	(500+1)

// table EXTERNALUSERS (PYR-56325)
#define OPUSERID_LEN	(32 + 1)

// table OPERATORS (PYR-52518)
#define OPERATORS_OPNETWORK_LEN		(20 + 1)
#define OPERATORS_OPERATORID_LEN	(32 + 1)

// table ASIAREPORTEVENTS (PYR-64516)
#define ASIAREPORTEVENTS_MESSAGEBODY_LEN  (3500 + 1) 
#define ASIAREPORTEVENTS_AGUNIQUEID_LEN   (50 + 1)

//	PYR-95715
#define SITEMASK_EX_LEN				(32 + 1)
#define LICENCEMASK_EX_LEN			(32 + 1)

// table CARDINFO
#define CARDINFO_PAYSYSTEMDATA_LEN		(201+1) // PYR-116520 Same as USER_PROPERTYSTR_LEN

// table PAM_USERS (PYR-121468)
#define PAM_USERS_EMAIL_LEN			(320 + 1)
#define PAM_USERS_FULL_NAME_LEN		(321 + 1)
#define PAM_USERS_PHONE_LEN			(100 + 1)

// types of requests used in DBM_Q_USER_EMAIL 
#define	EMAIL_REQ_TYPE_TOURNAMENT	0
#define	EMAIL_REQ_TYPE_ALLUSERS		1
#define	EMAIL_REQ_TYPE_ONEUSER		2
#define EMAIL_REQ_TYPE_USERS_LIST	3
#define EMAIL_REQ_TYPE_TRACKCODE	4

#define CHQ_REQUEST_TYPE_NOT_SENT	0
#define CHQ_REQUEST_TYPE_DATES		1
#define CHQ_REQUEST_TYPE_QHQNUM		2
#define CHQ_REQUEST_TYPE_USER_PEND	3

#define ADMINCHAT_TYPE_ONEWAY		0
#define ADMINCHAT_TYPE_TOUSER		1
#define ADMINCHAT_TYPE_FROMUSER		2
#define ADMINCHAT_TYPE_AUTOMATED	3

//reqId, our_txn_id, invoiceStr, status, pending_reason, payment_gross,
//payment_fee, ext_txn_id, txn_type, payer_email, payment_type

#define  TOURN_STATS_TOURN_ID		0
#define  TOURN_STATS_NUM_TOURN		1
#define  TOURN_STATS_NUM_TOURN_NOSTAT	2
#define  TOURN_STATS_TOURN_ID_NOSTAT	3

// Direct Debit codes
#define DIRECTDEBIT_PAYMENT  1
#define DIRECTDEBIT_REJECT   2
#define DIRECTDEBIT_REVERSAL 3
#define DIRECTDEBIT_WITHDRAWN_REVERSAL 4
#define DIRECTDEBIT_REVERSED_RETURN 5

//constants for TournEvents.EventType
#define TOURN_EVENT_NONE	0
#define TOURN_EVENT_LEVEL	1	
#define TOURN_EVENT_BREAK	2
#define TOURN_EVENT_MAXTIMEBANK 3 // #19424
#define TOURN_EVENT_MILESTONETOURN  4 // PYR-28689

//cheque statuses:
#define CHQ_PENDING_APPROVAL	0	//
#define CHQ_APPROVED			'A' // approved by our security
#define CHQ_DISAPPROVED			'D' // disapproved by our security (comment)
#define CHQ_SENT_TO_PROCESS		'S' // sent for processing (within batch)
#define CHQ_MAILED_FEDEX		'F' // we have FedEx number
#define CHQ_MAILED_MAIL			'M' // confirmed mailing to user
// possible status transitions:
//0 -> 'A'|'D'
//'D' -> 'A'
//'A' -> 'S'|'D'
//'S' -> 'F' | 'M'

/// constants for hand history request
#define HAND_HISTORY_LAST_HANDS		0
#define HAND_HISTORY_LAST_SECONDS	1
#define HAND_HISTORY_HAND_IDS		2
#define HAND_HISTORY_TOURNAMENT		3
#define HAND_HISTORY_LAST_HANDS_BEFORE	4
#define HAND_HISTORY_LAST_HANDS_BEFORE_DATE 5
#define HAND_HISTORY_TABLE			6	// 4t48 - requestContents_Not_Used, time, nHands, tableId
#define HAND_HISTORY_TABLE_FOR_DAY	7	// 48tt84f4f4[<8>] - requestContents_Not_Used, tableId, timeStart, timeEnd, lastHandId, method, isBriefHistory, olapPortionSize, useSuppliedHands, handsSize[<handId>]
// PYR-54814 - removed obsolete hand history request types

#define HAND_HISTORY_TABLE_FOR_DAY_METHOD_START_WITH_HAND_ID 0
#define HAND_HISTORY_TABLE_FOR_DAY_METHOD_START_WITH_TIME	 1

/// constants for DBM_Q_USER_UPDATE
#define IMAGE_TYPE_NONE			0 //set no image
//#define IMAGE_TYPE_PUBLIC		1 // set public image #imageId
#define IMAGE_TYPE_PRIVATE		2 // set private image, image follows
#define IMAGE_TYPE_SAME			3 // leave the same image I use now (don't do anything)
#define IMAGE_TYPE_LAST         IMAGE_TYPE_SAME // please keep up-to date

//constants for UpdateUserOneStringStmt
#define UPDATE_BIT_ADDRESS		(1 << 0) //1 // sssssss - userId, addr_1, addr_2, city, state, country, zipCode
#define UPDATE_BIT_EMAIL		(1 << 1) //2 // ss - user, new_email
#define UPDATE_BIT_PASSWORD		(1 << 2) //4 // ss - user, new_pwd
#define UPDATE_BIT_IMAGE		(1 << 3) //8 // s1 - {.|4|B} user, imageType, {nothing OR image(private) OR imageId(public)}
#define UPDATE_BIT_FULLNAME		(1 << 4) //16 // ss - userId, fullName
#define UPDATE_BIT_PHONE		(1 << 5) //32 // ss - userId, phone
#define UPDATE_BIT_REAL_MONEY	(1 << 6) //has nothing to do with parsing, means that user checked his account for "real money compliance"
#define UPDATE_BIT_SECURE_ID	(1 << 7) //has nothing to do with parsing, changes user's secureId
#define UPDATE_BIT_FIRSTNAME_LASTNAME	(1 << 8) // sss - userId, firstName, lastName
#define UPDATE_BIT_FISCAL_CODE	(1 << 9) // ss - userId, fiscalCode
#define UPDATE_BIT_DEFAULT_CURRENCY (1 << 10)
#define UPDATE_BIT_SEX			(1 << 11)
#define UPDATE_BIT_MOBILE		(1 << 12)

#define IMAGE_STATUS_PUBLIC		0
#define IMAGE_STATUS_PRIVATE	1
//#define IMAGE_STATUS_PENDING	1
//#define IMAGE_STATUS_APPROVED	1

//error codes
#define DBM_UPDATABLE_TABLE_PROCESSING_STOPPED (-4) // PYR-80335
#define DBM_CASHDBM_ERROR			(-3)
#define DBM_INTERNAL_ERROR			(-2)
#define	DBM_SQL_ERROR				(-1)
#define	DBM_NO_ERROR				0
#define	USER_ALREADY_EXISTS			1
#define NO_SUCH_USER				2
#define UNEXPECTED_COUNT			3 // amount of records affected by statement is unexpected
#define NO_SUCH_TABLE				4 // table with specified id not found among live ones
#define NOT_ENOUGH_CHIPS			5 // user does not have requested amount in bankroll
#define INCONSISTENT_DATA			6
#define INTERNAL_LOGICAL_ERROR		7
#define	NO_SUCH_IMAGE				8
#define	NO_SUCH_TOURNAMENT			9
#define	ALREADY_REGISTERED			10
#define	USER_NOT_REGISTERED			11
#define	TABLE_ALREADY_EXISTS		12
#define REGISTRATION_CLOSED			13
#define USER_ALREADY_SITTING		14
#define USER_NOT_SITTING			15
#define NO_SUCH_HAND				16
#define ALREADY_IN_LIST				17
#define UNREGISTRATION_CLOSED		18
#define WRONG_ACTIVATION_ID			19
#define ALREADY_ACTIVATED			20
#define NOT_ALLOWED					21
#define NO_SATELLITE_TARGET			22
#define WRONG_TOURNAMENT_STATUS		23 //tournament status does not allow to perform requested operation
#define NO_SUCH_PROPERTY			24
#define NOT_SET_PROPERTY			25
#define SATELLITE_WINNER			26
#define NO_SEATS_AVAILABLE			27
#define NOT_IN_WAITING_LIST			28
#define ALREADY_INVITED				29
#define DOUBLE_LOGIN_ATTEMPT		30
#define NO_TOURNAMENTS_PLAYED		31
#define WRONG_BUYIN_FPP				32
#define WRONG_BUYIN					33
//#define STRING_TOO_LONG				34 moved to "dbcommon.h"
#define DUPLICATE_NEW_USER_EMAIL	35
#define DUPLICATE_NEW_USER_INSTALL	36
#define TOURN_IN_PROGRESS			37
#define NO_NEWS_FOUND				38
#define NO_SUCH_EMAILBODY			39
#define NO_SUCH_EMAIL				40
#define DOUBLE_AT_TABLE				41
#define DOUBLE_IN_TOURN				42
#define NO_CHIPS_INFO				43
#define NOT_READY_FOR_REAL_MONEY	44
#define TRANSACTION_NOT_FOUND		45
#define TRANSACTION_PENDING			46
#define INVALID_INVOICE_STRING		47
#define TRANSACTION_COMPLETED		48
#define CASHIN_LIMIT_EXCEEDED		49
#define TOO_EARLY_TO_CASHOUT		50 //4s4 err_code, errDescr, restrictionPeriodInSec - PYR-91016
#define NO_TICKET_FOUND				51
#define NOT_ENOUGH_FPP				52
#define CURRENT_BONUS_NONE			53
#define BONUS_IN_EFFECT				54
//#define NOT_ENOUGH_HANDS			55 //not enough raked hands played after bonus deposit
#define INDICATE_BONUS_AWARENESS	56 
//IPN error codes:
#define DBM_IPN_NOTFOUND_IGNORED		57 //IPN - txn not found and status is not ok. No Action.
#define DBM_IPN_NOTFOUND_CREATED		58 //IPN - txn not found and status is ok. New unfinished errorneous transaction 'e' created.
#define DBM_IPN_COMPLETED_IGNORED		59 //IPN - txn found, but is already completed and infos match. No action
#define DBM_IPN_COMPLETED_ERROR			60 //IPN - txn found, but is already completed and infos do not match. No action
#define DBM_IPN_FAILED					61 //IPN - txn found, not completed and PayPal returned "Failed". Trans closed as Rejected
#define DBM_IPN_PENDING					62 //IPN - txn found, not completed and PayPal returned "Pending" and pending reason is NOT "echeck". Trans not closed
#define DBM_IPN_MISMATCH				63 //IPN - txn found, but infos do not match and status = ok. New unfinished errorneous trans 'E' created
#define DBM_IPN_XFOUND_MATCH			64 //IPN - found xTransStr, all fields match, nothing is done
#define DBM_IPN_XFOUND_CREATED			65 //IPN - found xTransStr, fields do not match, status ok - new trans created 'E'
#define DBM_IPN_XFOUND_IGNORED			66 //IPN - found xTransStr, fields do not match, status not ok - ignored
#define DBM_IPN_MISMATCH_IGNORED		67 //IPN - txn found, but infos do not match and status not ok. ignored
#define DBM_IPN_EMAIL_MISMATCH			68 //IPN - txn found, but emails do not match and status = ok. Trans marked 'e', uncompleted
#define DBM_IPN_EMAIL_MISMATCH_IGNORED	69 //IPN - txn found, but emails do not match and status not ok. ignored
#define DBM_IPN_PENDING_ECHECK			72 //IPN - txn found, not completed and PayPal returned "Pending" and pending reason is "echeck" Trans not closed
#define DBM_IPN_USERBYEMAIL_ONLY		80 //IPN - txn not found and status is ok. User found by unique email. New approved transaction created.
#define DBM_PAYPAL_TRANS_XED			70 // PayPal trans was marked as bad because user is banned
#define DBM_ACCOUNT_CLOSED				71	// account closed
/// next is 73, not 72!!!				73!!!
#define USER_ID_MISMATCH				73  //see DatabaseManager::acceptErrorneousTransaction
#define USER_EMAIL_MISMATCH				74  //see DatabaseManager::acceptErrorneousTransaction
#define ERROR_NON_RESTORABLE			75  //see DatabaseManager::acceptErrorneousTransaction
#define BANKROLL_AMOUNT_MISMATCH		76  //see DatabaseManager::addApprovedTrans
#define WAITINGLIST_EMPTYSEATAVAIL		77  //attempted to sit, but empty seat available->go to the table. Message will contain '6ss': err_code, serverName, serverObject
#define DBM_CARD_BANNED					78  // attempt to init GCash purchase trans whth a card that is banned
#define INVALID_CC_NUMBER				79  // invalid credit card number
// !!!!!!!!!!!!!					//  80 is already used !!!DBM_IPN_USERBYEMAIL_ONLY
#define RELATED_USER_SITTING			81  // related user is already sitting at this table
#define CHEQUE_NOT_FOUND				82  // cheque to update was not found
#define CHQ_COMMENT_REQUIRED			83
#define TOO_MANY_CHEQUES				84 //DatabaseManager::setChequeCancelled
#define NO_AUTO_BAN_USER				85 // cannot ban user with noAutoBan privilege
#define CARD_BLOCKED					86 // user's credit card is blocked - no purchase
#define AT_TOO_MANY_TABLES				87 // user is playing at too many tables
#define	HISTORY_MAINTENANCE				88 // we are working in "reserve" history mode
/* obsolete by PYR-45748
#define	WU_RESTRICTED_STATE				89 // players' state is "restricted" for Western Union transfers
*/
#define USER_CASH_BANNED				90 // user's acoount is banned for cash
#define DBM_CLOSED_AND_BANNED			91 // user's acoount is banned for cash
#define NOT_ENOUGH_TCHIPS				92 //
#define ADDR_ZIP_NOT_VERIFIED			93 // user tries to reset password but zip or address is incorrect
#define USER_NAME_TYPO					94 //
#define LIKELY_PLAY_HAND				95 // hand id not found. likely it was a play money hand which we do not save
#define FREEROLL_TOURN					96 // tourn found, but it was a freeroll in which we do not save hands
#define LOAD_USERS_ERRORS               97 // errors while try to load users from oltp to olap
#define PROCESS_SAVED_ERRORS            98 // errors while try to get saved messages 
#define NO_ADMIN_PRIVILEGES				99 //
#define WRONG_PWD						100 //
#define DB2_DUPLICATE_KEY               101 
#define INSTALLID_FROZEN				102 //user is tying to login with an installId that is among frozen ones (table "bannedinst")
//#define OLAP_SYNCHRONIZATION_ERRORS     103 // errors while synchronizing OLAP and OLTP
/* obsolete by PYR-45735
#define DBM_INCOMPLETE_WU_TRANS			104	//WU transaction has MTCN but is not complete (received is null)
#define DBM_NO_INCOMPLETE_WU_TRANS		105 //users attempts to enter MTCN but no appr. transaction is found
#define NO_WU_RECEIVERS					106 // no available receivers found
#define DOUBLE_MTCN						107 //attempt to enter existing MTCN again
*/
//#define WRONG_PROMOTION_CODE			108 //new account with unexisting or mismatch code
#define MAC_FROZEN						109 //user is tying to login with a MAC address that is among frozen ones (table "bannedmacs")
#define	DBM_TSALE_MISMATCH				110 //tmoney sale found but the other party specified different parameters
#define DBM_TSALE_DUPLICATE				111 // user is trying to initiate more than one tmoney sale
#define DBM_TSALE_CREATED				112 // t-chips sale created
#define DBM_TSALE_BUYER_BAD				113 // buyer's account is either banned or closed
#define DBM_TSALE_BUYER_IS_SELLER		114 // buyer is the same as seller
#define DBM_TSALE_BOTH_T_AND_W			115 // both tChips amount and wChips amount are not 0
#define DBM_TSALE_NEITHER_T_NOR_W		116 // both tChips amount and wChips amount are = 0
#define DBM_TSALE_NO_MONEY				117 // sale price in 'real' $ is 0
#define DBM_TSALE_PRICE_OVER			118 // sale price is higher than T/W amount
#define DBM_TSALE_PRICE_UNDER			119 // sale price is less than minimal allowed percent
#define DBM_TSALE_SELLER_BAD			120 // seller's account is either banned or closed
#define DBM_TSALE_BUYER_SHORT			121 // buyer doesn't have enough 'real' $
#define DBM_TSALE_SELLER_SHORT			122 // seller doesn't have enough 'real' $
#define DBM_TSALE_EXPIRED				123	// sale is being cancelled but it's already expired
#define DBM_TSALE_CANCELLED				124	// sale is being cancelled but it's already cancelled
#define DBM_TSALE_COMPLETE				125	// sale is being cancelled but it's already complete
#define DBM_TSALE_OPPOSITE				126 // opposite sale exists			
#define DBM_TSALE_BAD_EMAIL				127 // e-mail is not verified			
#define	DBM_PAYSYSTEM_SUSPENDED			128 // paysystem requested is suspended
#define	DBM_WRONG_VIP_STATUS			129 // don't meet requirements by VIP status
#define	DBM_OLAP_SAVER_ERROR			130 // olap saver can not save message in file
#define DBM_ACHNOTIF_IGNORED			131 // ach return notification ignored
#define DBM_BARRED_RETURN				132 // user received ACH return and is cash-barred
#define DBM_WRONG_BONUS_CODE			133 // user provided wrong code
#define DBM_BONUS_CODE_INELIGIBLE		134 // user provided code, but it belongs to other bonus
#define DBM_BONUS_CODE_CANCELLED		135 // user provided wrong code
#define GATEWAY_WIRE_ERROR				136 // global wire error
#define DBM_NO_PAYSYSTEM_SUPPORT		137 // paysystem does not support excluded states functionality
#define ERROR_UNDER_AGE					138 // paysystem does not support excluded states functionality
#define DBM_PENALTY_TICKET_EXIST		139 // paysystem does not support excluded states functionality
#define EMAIL_MISMATCH					140 // user tries to reset password but email is incorrect
#define TOO_MANY_ITEMS					141 // Too many items in the list to be processed
/* EPassporte is obsolete
#define EP_TOO_MANY_ACCOUNTS			142 // Too many EP accounts used by user
#define EP_ACCOUNT_IN_USE				143 // EP account is used by another user
#define EP_TIMED_OUT					144 // EP trans found with 'T' status
*/
#define USER_HAS_VPP					145 // user has VPP
#define NOT_VERIFIED					146 // user's age was not verified (score is not enough)
#define ALREADY_SELF_EXCLUDED			147 // users requests self-exclusion while being self-excluded
#define USER_SELF_EXCLUDED				148 // user is self-excluded
#define TOO_MANY_ACCOUNTS				149 // the user already has an account
#define TRANSACTION_NSF					150 // cashin transaction rejected due to insufficient funds (PayTru)
#define TOO_LARGE_PERIOD				151 // period requested period too long
#define CASHIN_SELFLIMIT_EXCEEDED		152 // requested cashin amount exceeds user's self-imposed limit
#define ACCOUNT_BEING_CREATED			153 //user has an MRMC account that is being created (no reply from PayTru?)
#define FAILED_VELOCITY_CONTROL			154 // user has too many uncleared transactions / too much uncleared funds
#define NO_TICKET_TYPE					155 // wrong ticket type was specified
#define GIFT_CARD_UNAVAILABLE			156 // a gift card is unavailable or "out of stock"
#define WRONG_TICKET_TYPE				157 // wrong ticket type was specified
#define ALREADY_BANNED					158 // user is already banned (cashBanned)
#define CASHTRANS_FOUND					159 // specific case, we're looking for wire order by Id but it's actually a cashtransId (see #6120)
/* obsolete by PYR-45735
#define WURECEIVER_EXPIRED				160 // wu receiver that we issued has expired
*/
#define INVALID_TINFO_MANUAL_PROMO 		161 // manualPromo from T-Info is invalid.
#define MAX_DEPOSITS_LIMIT_EXCEEDED		162 // too many deposits when checking velocity control
#define MAX_ACCOUNTS_LIMIT_EXCEEDED		163 // too many accounta used when checking velocity control
#define VOLUME_FROZEN					164 //user is tying to login with a volume label that is among frozen ones
#define IP_FROZEN						165 //user is tying to login with a IP address that is among frozen ones
#define NOT_VERIFIED_OFFER_LIVE_SUPPORT	166 // user's age was not verified (score is not enough). User is eligible for live support.
#define NO_SUCH_TRACKCODE				167 // specified by admin tracking code does not exists
#define NO_GATEWAY						168	// gateway not found
#define NOT_INITIALIZED                 169 // not initiazed
#define USER_EMAIL_ALREADY_EXISTS		170 // user cannot change his/her email because a similar one already exists
#define DBM_QUERY_TIMEOUT	            171 // select timed out
#define INVALID_PHONE					172 // phone number did not pass validation
#define TABLE_LIMIT_EXCEEDED			173 // user is not allowed to play at the table when table's high bet value greater than user's table limit
#define ALREADY_REQUESTED_LIMIT_INCREASE	174 // user has already requested a table limit increase
#define ACCT_CURRENCY_MISMATCH			175 // an operation was attempted that cannot be completed because of currency limitations
#define IN_GRACE_PERIOD					176 // user in grace period, cashouts are held
#define ACCT_LICENSE_MISMATCH			177 // user client's license doesn't match user account's license
#define ACCT_WAS_CONVERTED				178 // same as 177 but accoun license was changed by us
//#define PAST_GRACE_PERIOD				179 // user is trying to login, he's not authenticated (KYC) and his grace period has expired; #21414: not in use anymore
#define LOGIN_RESTRICTED				180 //user's country is restricted for login with this siteIdMask
#define TRANSFER_SENDER_DISABLED		181 //transfer sender has "transfer disabled" privilege
#define TRANSFER_RECIPIENT_DISABLED		182 //transfer recipient has "transfer disabled" privilege
#define SATELLITE_TARGET_IS_SNGO		183 // target is SNGO
#define NOT_ENOUGH_YEARLY_VPP			184 // user do not have enough yearly VPP to buy in vipstore
#define USER_HAS_BONUS					185 // user already has requested bonus
#define WAITINGLIST_TOO_LONG			186 // Waiting list too long [AO 2009-02-26]
#define WRONG_AFFILIATE_ID				187 // specified affiliate ID is not valid
#define USER_HAS_PENDING_CASHOUTS		188 // operation cannot be completed until user has pending cashouts
#define AAMS_REBUY_ERROR				189 // rebuy not approved by AAMS
#define AAMS_ADDON_ERROR				190 // addon not approved by AAMS

// [AO 2009-05-15] Bug #8481: Coupons
#define STORE_ITEM_NOT_FOUND			191
#define ILLEGAL_COUPON					192
#define COUPON_NOT_ACTIVE				193
#define COUPON_EXPIRED					194
#define USER_INELIGIBLE_FOR_COUPON		195
#define ITEM_INELIGIBLE_FOR_COUPON		196

#define USER_CURRENCY_ACCOUNT_NOT_FOUND	197
#define MAXIMUM_ITEM_ORDERS_EXCEEDED	198 // [AO 2009-08-26] PYR-12394

// [AO 2010-09-22] #15895
#define NO_MULTIPLE_ITEMS_WITH_COUPON	199
#define NO_MULTIPLE_SPECIAL_ITEMS		200

// [AO 2008-12-23] Generic error codes
#define	RECORD_ALREADY_EXISTS			201
#define NO_SUCH_RECORD					202

#define ILLEGAL_SITE_ID					203
#define SERVICE_NOT_AVAILABLE			204
#define MIGRATION_LINK_BIRTHDAYS_DONT_MATCH	205
#define MIGRATION_LINK_CITIES_DONT_MATCH	206
#define CONDITIONS_HAVE_CHANGED			207
#define NOT_ALLOWED_USER_ACTIVE			206 // Yes, it's a duplicate, but used in a different context
#define ITALY_NOT_ALLOWED_ON_COM		209 // PYR-11846: .COM Italian account cannot login
#define MESSAGE_ALREADY_PROCESSED		210 // [AO 2009-07-09] Recovered message on a G-connection was already processed
#define USER_HAS_UNFINISHED_CASHTRANS	211
#define USER_HAS_UNCLEARED_FUNDS		212

#define USER_CURRENCY_ACCOUNT_NOT_FOUND_NOT_ALLOWED	213

#define ERR_INV_ROUTING_CONFLICTS			214 // [AN 2009-10-30] PYR-7801 - set routing could not complete due to routing conflicts
#define ERR_INV_ROUTING_ILLEGAL_ITEMID		215 // [AN 2009-10-30] PYR-7801 - incorrect item id specified for get or set routing 
#define ERR_INV_INVALID_STOCK_ITEMID		216 // [AN 2009-10-30] PYR-7801 - incorrect stock item Id specified
#define ERR_INV_STALE_STOCK_ITEM			217 // [AN 2009-10-30] PYR-7801 - warehouse stock had updated by someone else
#define ERR_INV_NO_AUDIT_COMMENT			218 // [AN 2009-10-30] PYR-7801 - missing audit comment
#define ERR_INV_NO_ROUTE_FOUND				219 // [AN 2009-10-30] PYR-7801 - could not resolve routing (warehouse->item->country)
#define ERR_INV_NO_STOCKID					220 // [AN 2009-10-30] PYR-7801 - there's no stockId for a specified combination of properties
#define ERR_INV_OUT_OF_STOCK				221 // [AN 2009-10-30] PYR-7801 - item is out of stock
#define ERR_INV_NO_STOCK_FOR_ORDER			222 // [AN 2009-10-30] PYR-7801 - the order does not have a warehouse item associated with it
#define ERR_INV_ORDER_MOVE_FAILED			223 // [AN 2009-10-30] PYR-7801 - could not move order. no inventory in the target warehouse.
#define ERR_INV_NO_ORDERS					224 // [AN 2009-10-30] PYR-7801 - source warehouse does not have any orders that can be moved
#define ERR_INV_IMMOVABLE_ORDER				225 // [AN 2009-10-30] PYR-7801 - cannot move an order if it has already been canceled or mailed
#define ERR_INV_STOCK_NOT_IN_WAREHOUSE		226 // [AN 2009-10-30] PYR-7801 - specified physical item is not stocked by the specified warehouse
#define ERR_INV_NO_PHYSICAL_ITEMS_FOR_ITEM	227 // [AN 2009-10-30] PYR-7801 - there are no physical items for the specified item
#define ERR_INV_UNEXPECTED_ERROR			228 // [AN 2009-10-30] PYR-7801 - something unexpected in the inventory code

#define NOT_ENOUGHT_INFO					229 // PYR-12953 - err code for client

#define INCORRECT_ORDER_STATUS				230 // Store order is in an incorrect status for this operation

#define ERR_INV_RET_ORDER_ALREADY_EXISTS	231 // [AN 2009-10-30] PYR-7801 - Order Id is already associated with a different reference number for a returned item

#define REQUEST_ALTERNATIVE_PAYSYSTEM		232 // #12806 cashier->client: upon receiving this error code client should request other available paysystems to be suggested to the user

#define ERR_INV_CUSTOM_ROUTING_CHANGED		233 // [AN 2009-12-22] PYR-7801 Custom routing was changed for the item
#define ERR_INV_ROUTING_REVISION_ABSENT		234 // [AN 2009-12-22] PYR-7801 Routing revision does not exist

// #13439
#define ERR_DEFAULT_CURRENCY_ACCOUNT		235 
#define ERR_CURRENCY_ACCOUNT_IS_NOT_EMPTY	236
#define ERR_USER_HAS_ALLOCATED_CHIPS		237

#define USER_PREFERENCE_HAS_CHANGED			238
#define CONVERSION_RATES_DECLINED			239

#define ERR_USER_OWES_CHIPS					240 // [AO 2010-02-09] PYR-13649

#define ERR_INV_REVISION_COMPLETED			241 // [AN 2009-10-30] PYR-7801 - cannot change custom routing for a completed revision

#define ERR_SATELLITE_TARGET_BUYIN_MISMATCH		242 // [JV 2010-03-17] PYR-14186
#define ERR_SATELLITE_TARGET_FPPBUYIN_MISMATCH	243 // [JV 2010-03-17] PYR-14186

//PYR-13013
#define ERR_NO_ADMISSION					244
#define ERR_WRONG_ADMISSION					245
// #14108
#define AT_TOO_MANY_HU_TABLES				246 // User is playing at too many heads-up tables

//PYR-14051
#define ERR_GENDER_MISMATCH					247

#define OPERATION_ABORTED					248

// #14108
#define NOT_ALLOWED_AT_HU_TABLES			250 // User is not allowed at heads-up tables having priv2HeadsUpTablesNotAllowed
// #14277 
#define MUST_HAVE_DEPOSIT_LIMIT				251 // User is not allowed at heads-up tables having priv2HeadsUpTablesNotAllowed
// #14463
#define BALANCE_AMOUNT_WITH_CENTS_OVERFLOW	252 // either retrieving the data from or setting the data in AmountWithCents 
												// that causes an overflow condition
// #14492
#define ERR_EXCLUDED_STATE					253 // User's state is listed as excluded
#define BLOCKED_RM_TABLES					254 // User is not allowed to play at RM tables due to blockingCountryLevel for his country
#define BLOCKED_RM_TOURNS					255 // User is not allowed to play RM tournaments due to blockingCountryLevel for his country
#define BLOCKED_PM_TABLES					256 // User is not allowed to play at PM tables due to blockingCountryLevel for his country
#define BLOCKED_PM_TOURNS					257 // User is not allowed to play PM tournaments due to blockingCountryLevel for his country

// #14426
#define ACCT_LICENSE_MISMATCH2				258 // User client's license doesn't match user account's license. An addition to ACCT_LICENSE_MISMATCH which implies .IT related problem.
#define USER_BIRTHDAY_REQUIRED				259 // User's birthday is required to process authorization request.
#define USER_BIRTHDAY_INVALID				260 // User's birthday is invalid. Authorization declined.

// #14425
#define INVALID_MOBILE_PHONE				261 // User's mobile phone is invalid.

// #14544 blacklist
#define PLAYER_BLACKLISTED					262 // Player is black listed

// PYR-14428
#define AVC_VALIDATION_NOT_REQUESTED		263 // AVC validation was not requested for the user

#define MIGRATION_LINK_LASTNAMES_DONT_MATCH	264
#define MIGRATION_LINK_ZIPCODES_DONT_MATCH	265
#define FRENCH_NOT_ALLOWED_ON_COM			266 // PYR-14432: .COM French account cannot login
#define MIGRATION_USER_PROPERTIES_CHANGED	267
#define MIGRATION_LINK_FAILURE				268
#define ERR_COUNTRY_RESTRICTED				269

// PYR-14428
#define AVC_NO_CHANGES_REQUESTED			270 // No changes have been requested
// PYR-15175
#define TOURN_UNAVAILABLE_AFTER_MIGRATION	271 // tournament information unavailable to the user after migration

// #14430
#define ERR_BUYIN_LIMIT_NOT_SET			272 // RM .FR user must set a buy-in limit to play RM games
#define ERR_BUYIN_LIMIT_REACHED			273 // RM .FR user reached his/her buy-in limit

//#14427
#define ERR_ACCOUNT_SUSPENDED			274 // RM .FR user can't cashin if suspended
#define ERR_ALREADY_DONE				275

// #14430
#define MUST_HAVE_DEPOSIT_AND_BUYIN_LIMITS	276
//#13970
#define USER_SELF_EXCLUDED_SENDER       277 // sender is self-excluded of transfer request
//#14750
#define NO_TOURNAMENTS_FOR_TICKET		278 // ticket does not afford an admission to any existing tournaments
// #15369
#define MUST_HAVE_WITHDRAWAL_LIMIT		279 // user must setup automatic withdrawal limit
// #15238
#define MISSING_PERSONAL_ID_CODE		280 // user must specify personal id code

//PYR-15751
#define USER_IS_UNDERAGE				281 // User's underage

#define ESTONIANS_NOT_ALLOWED_ON_COM	282 // PYR-15751: .COM Estonian account cannot login

#define BLOCKED_CASHIN					283 // User is not allowed to cashin           due to blockingCountryLevel for country
#define BLOCKED_TRANSFER_IN				284 // User is not allowed to send transfer    due to blockingCountryLevel for country
#define BLOCKED_TRANSFER_OUT			285 // User is not allowed to receive transfer due to blockingCountryLevel for country
#define BLOCKED_TSALE					286 // User is not allowed to do TSale		   due to blockingCountryLevel for country
#define BLOCKED_CASINO					287 // User is not allowed to do Play Casino   due to blockingCountryLevel for country

#define SID_EXPIRED						288	// PYR-15984: expired secure ID

#define REPEATED_REQUEST				289 // #14028: Repeated request is either detected or assumed.
#define CARD_BLOCKED_WITH_REASON		290 // #16054 The reason of card blocking is provided

#define CHALLENGE_QUESTIONS_NOT_VERIFIED	291 // PYR-16515: challenge question / answer for password reset	
// #16898
#define HOME_GAMES_CLUB_NOT_ALLOWED		292 // User is not allowed to access Home Game club
// PYR-17065
#define USER_EMAIL_NOT_VALIDATED        293 // User's email address is not yet validated

// PYR-13934
#define SET_JUBILEE_HANDS_ERROR         294 // error found for setting jubilee hands
// #17783
#define MULTIPLE_TABLES_FOUND           295
// #17835
#define CHANGE_CLUBS_RECREATE_TOURN     296 // cannot change club ids while recreating tournaments

// PYR-17813
#define TOURN_HG_CLUBID_MISMATCH        297  // club id mismatch on tournament
#define HGCLUB_TOURN_NOT_RECURRING      298  // HG tournament is not recurring

// #17774
#define SET_RESTART_TIME_TOO_CLOSE      299 // Restart time closer than %u minutes
#define INVALID_RESTART_TIME			300 // cannot create RYO tournament too close to restart

// #18004
#define RESERVED_USER_ID				301 // User Id cannot be used because it begins with a reserved name

//PYR-15847
#define SEATMANAGER_TABLE_NOT_FOUND		302
#define SEATMANAGER_NOTHING_TO_SEARCH	303 //No tables of suitable type
#define SEATMANAGER_WAITING_LIST		304 //User appended to waiting list by SeatManager

// #17774
#define SET_RESTART_TIME_TOO_FAR		305 // Restart time farther than %u minutes

// #14242
#define USER_MOBILE_ALREADY_EXISTS      306 // user tried to enter mobile #, which already exists
#define ALREADY_VALIDATED               307 // user tried to validate mobile #, which was already validated
#define WRONG_VALIDATION_ID             308 // user entered wrong mobile phone validation code
#define DUPLICATE_NEW_USER_MOBILE       309 // new user provided mobile #, which already belongs to someone
#define RESEND_VALIDATION_ID            310 // user failed to validate mobile# too many times, so another validation code was generated

// PYR-18024
#define ERR_RING_GAME_REPLAY_NOT_FOUND  311
#define ERR_RING_GAME_REPLAY_DUP_NAME   312
#define ERR_RING_GAME_REPLAY_INIT_ERROR 313

// PYR-18543
#define ALL_SERVERS_BUSY	314 // response to admin when all instances of template server are busy

// 	PYR-18701
#define NO_ALTERNATE_USER_NAME			315 // Could not find a suitable alternate user name.
// #18904
#define NOT_ALLOWED_WITHOUT_PLAYER_TOKEN	316 // .BE specific: user not allowed to sit at RM tables and register RM tournaments without having player token.

// PYR-18708
#define ERR_SOAP_REQUEST_READ_TIMEOUT	317

// PYR-18710
#define ERR_PGAD_ACCOUNT_OPEN			318

// PYR-18541
#define ERR_ADMIN_RESET_STACKS_TABLE_NOT_PAUSED		319
#define ERR_ADMIN_RESET_STACKS_NOT_ALLOWED_IT		320

// PYR-18129
#define ERR_CHANGE_DUMMY_TOURN_PARAMS	321

// PYR-19104
#define HOME_GAMES_NOT_ALLOWED_FR		322 // .FR user is not allowed to access Home Games

#define ERR_REPEATED_CASHIN_DECLINE		323 // #17995

// #19149
#define NOT_ALLOWED_WITHOUT_RM_OK		324 // Request is declined due to absence of realMoneyOk flag

//PYR-15847
#define SEATMANAGER_TOO_BUSY			325		//exceeded maximum queue length

// #19442
#define DBM_PERROR_EXCEPTION            326     // receive PError in DBM
#define DBM_SENDER_ACCOUNT_CLOSED		327 // #19411

// #19167 - cannot (un)register from flight tournaments
#define CANNOT_REGISTER_TARGET_FLIGHT_TOURN    328
#define CANNOT_UNREGISTER_TARGET_FLIGHT_TOURN  329

// #19167 - mistmatch scripts
#define MISMATCH_SCRIPT_PARAM_SAME_FLIGHT_LEVEL  330
#define MISMATCH_SCRIPT_BUYIN_TARGET             331

// #19167 - cannot register level 1 flight tournament if already won 
#define CANNOT_REG_ALREADY_WON_LEVEL_1_FLIGHT_TOURN		 332
#define CANNOT_REG_CONCURRENT_LEVEL_1_FLIGHTS			 333

// #19436
#define ERR_EPIS_ACCOUNT_OPEN           334
#define USER_TTICKETS_TOTAL_ALREADY_CALCULATED		335 // #19360

#define VALIDATION_CODE_EXPIRED			336 // PYR-18694

// PYR-18237, for msg MSG_LOBBY_UPDATE_USER2_REPLY, followed by errStr ("") and isPlayMoneyPlayer
// used only in LobbyClientConn, and the error message is sent only when the client is .NET
#define USER_PLAYMONEY_INFO             337 

// #19474
#define CANNOT_CANCEL_FLIGHT_TOURN		338

// #19167/#19474
#define CANNOT_REDEPLOY_FLIGHT_TOURN	339

// PYR-20187
// TemplateServer error codes
#define TS_ERROR_INTERNAL               340   // some internal processing error
#define TS_ERROR_SMSGW_NOT_CONNECTED    341   // SMS GW is not connected
#define TS_ERROR_SMS_GATEWAY            342   // some error from SMS GW
#define TS_ERROR_SMS_GATEWAY_NO_RETRY   343   // should not retry after this error from SMS GW
// PYR-18694
#define TS_ERROR_SMS_MSG_SUBMITTED		344	  // not an error, just notification that SMS was submitted.

#define USER_IS_ARCHIVED				345	  // #20399 User is not in OLTP, but in PYRLOG.USERS_ARCH - special error for Admin to show "unarchive" button
#define USER_CANNOT_BE_ARCHIVED			346	  // #20399 User requested to be archived had just logged in

#define CURRENCY_NOT_SUPPORTED			347 // currency is neither USD nor in the CONVRATE list
#define SPAIN_NOT_ALLOWED_ON_COM		348 // #20725: .ES migration
#define DANISH_NOT_ALLOWED_ON_COM		349

#define USER_LIMIT_PENDING_48_HOURS		351 // increase below the max-limit
#define USER_LIMIT_PENDING_7_DAYS		352 // increase above the max-limit, after passing the test
#define USER_LIMIT_MANUAL_APPROVAL		353 // increase above max-limit, test passed and after 3 month
#define USER_LIMIT_REJECTED_1			354 // fails the test
#define USER_LIMIT_REJECTED_2			355 // rejected increase request within 3 month after passed the test
#define USER_LIMIT_NEED_TEST			356 // user has to pass the test
#define USER_LIMIT_UNCHANGED			357 // the limit has not been changed

#define RM_DATA_REJECTED_BY_REGULATOR	358 // regulator replied that user real money form data was incorrect

#define WAIT_FOR_REPLY					359	// error code for short reply
#define ERR_AGEVERIFICATION_NEEDS_DATA	360	// #17003
#define COMMENT_REQUIRED				361	// #21618
#define REPEATED_SHUTDOWN_REQUEST		362 // #21855
#define CSR_CANNOT_VIEW_COUNTRY			363	// #21975

#define TRACK_CODE_NOT_FOUND			364 // PYR-19565
#define TRACK_CODE_WRONG_STATUS			365 // PYR-19565
#define TRACK_CODE_DISABLED				366 // PYR-19565

#define MIGRATION_CASHOUT_FAILED		367 //
#define MIGRATION_DEPOSIT_FAILED		368 //

#define ACCESS_NEW_IP_RESOLUTION		369 // #22500. User's login from new IP address within the same session is rejected due to its resolution into different country/state.
#define USER_ALREADY_MIGRATED			370 // #22434
#define COUPON_REQUIRED					371 // PYR-20830

#define CANNOT_CHANGE_BIRTHDATE			372 // #22870
#define CANNOT_CHANGE_USER_IMAGE		373 // #22870

#define ES_TOO_MANY_TOURNS_REQUESTED	374 // #20837
#define ES_TOO_MANY_HANDS_REQUESTED		375 // #20837
#define ES_LOGIN_REPORT_INVALID_LICENSE	376 // #20837
#define RANGE_MIN_HANDID_NOT_FOUND		377 // #20837
#define RANGE_MAX_HANDID_NOT_FOUND		378 // #20837

#define SEATMANAGER_CAPS_REACHED		380 // #22531
#define TOO_MUCH_CASH_TO_MIGRATE		381 // #23157
#define INTERNAL_ERROR_NOT_TRANSLATED	382 // #19814
#define NOT_ALLOWED_AT_PL_OMAHA_TABLES	383	// #20119
#define NOT_ALLOWED_AT_BLITZ_TABLES		384	// #20119
#define NOT_ALLOWED_AT_HU_SITNGOS		385	// #20119
#define NOT_ALLOWED_AT_HYPER_TURBO_SITNGOS		386	// #20119
#define SEATMANAGER_CAPS_REACHED_RM		387 // #22531
#define SEATMANAGER_CAPS_REACHED_PM		388 // #22531

#define NO_VALIDATED_MOBILE					389	// #21829
#define INSTALL_VALIDATIONCODE_MISMATCH		390 // #21829
#define INSTALL_VALIDATION_LIMIT			391 // #21829

#define ERR_STARCODE_DOES_NOT_EXIST			392 // #22494
#define ERR_STARCODE_ALREADY_USED			393 // #22494
#define ERR_STARCODE_MAXUSE_REACHED			394 // #22494
#define ERR_STARCODE_BEING_PROCESSED		395 // #22494
#define ERR_STARCODE_WRONG_TIMEFRAME		396 // #22494
#define ERR_STARCODE_INACTIVE				397 // #22494

#define CPRCHECK_HIDDEN_ADDRESS         	398 // #22151

#define ERR_NO_HIGHROLLER_PRIVS				399 //PYR-23520

#define ERR_STARCODE_EXCLUEDED				400 // #22494

#define FACEBOOK_LINK_CODE_MISMATCH			401 // #23923
#define USER_ALREADY_HAS_FACEBOOK_LINK		402 // #23923
#define TOO_MANY_REQUESTS					403
#define TOO_MANY_FAILURES					404

#define MOBILE_NOT_MATCH					405 // #22184
#define CANNOT_CHANGE_FACEBOOK_PASSWORD		406 // #24052

// PYR-23485
#define ERR_NON_FACEBOOK_USER				407
#define ERR_FACEBOOK_TOKEN_NOT_FOUND		408
#define ERR_FACEBOOK_TOKEN_MISMATCH			409 

#define INSTALL_VALIDATIONCODE_INCORRECT	410	// #21829
#define INSTALL_VALIDATIONCODE_NOTFOUND		411	// #21829

// obsolete FT error codes
#define ERR_FULLTILT_INCONSISTENT_DUPLICATE_MSG	412
#define ERR_FULLTILT_PS_USER_ALREADY_PAIRED		413
#define ERR_FULLTILT_FT_USER_ALREADY_PAIRED		414
#define ERR_FULLTILT_PAIRING_CODE_NOT_FOUND		415
#define ERR_FULLTILT_PAIRING_CODE_MISMATCH		416
// end of obsolete FT error codes
#define ERR_FULLTILT_FT_USER_NOT_FOUND			417
#define ERR_FULLTILT_FT_USER_ALREADY_CONVERTED	418
#define ERR_FULLTILT_PS_USER_ALREADY_CONVERTED	419
#define ERR_FULLTILT_FORBIDDEN_LICENSE_AND_COUNTRY	420

#define USER_BANNED_BY_REGULATOR				421
#define ERR_TEMP_NO_TABLE_STARTER				422

#define ERR_SMSPWDRESET_LIMIT_REACHED			423 // PYR-22184

#define ERR_UNDEFINED_STATE						424 // PYR-24314
#define USER_CURRENTLY_LOGGED_IN				425 // PYR-24812
#define NO_TOURNHAPPYHOUR_PROMO					426	//PYR-24396
#define ERR_NO_TABLE_FOUND_TO_OBSERVE			427 //PYR-25271
#define DB_ERR_ALLOWED_ONLY_FOR_OLAP			428 // PYR-25352
#define CLOSE_RM_FORM							429 // PYR-25239
#define ERR_BL_TOOLATE_TO_SITOUT				430 // PYR-24751
//#define NO_GOLDENSNG_PROMO_OBSOLETE			431	//PYR-24382 (obsolete)
#define ZOOM_TOURNAMENT_CONVERSION				432	//PYR-25736

#define TRACK_CODE_LOCKED						433 // PYR-25971

// PYR-24890
#define INSTALL_ID_INVALID_LOCK_TYPE			434 // PYR-24890
#define INSTALL_ID_ALREADY_FROZEN			    435 // PYR-24890
#define INSTALL_ID_NOT_FOUND					436 // PYR-24890
#define CANNOT_UNREGISTER_REMOVED_USER			437 //PYR-26055
#define USER_NOT_LOGGED_IN						438 // PYR-26155
// PYR-26251
#define DBM_NOTHING_TO_DO						439 // real chips update transaction is not needed

#define SEATMANAGER_NO_PLAY_PRIVILEGES			440	//PYR-26500
#define SEATMANAGER_EMAIL_NOT_VALIDATED			441	//PYR-26500
#define ACTIVITY_RESTRICTED_FOR_STAFF			442 // PYR-26026
#define ORIGINAL_TRANSACTION_NOT_FOUND			443 // PYR-23644
#define INCORRECT_AWARD							444 // PYR-24840

#define RATHOLING_BYPASS						445 // PYR-25814
#define RATHOLING_OBLIGATION_NOT_FOUND			446 // PYR-25814
#define RATHOLING_INVALID_DATA					447 // PYR-25814

#define SERVER_NOT_READY						448 //PYR-27620
#define HOST_LICENSE_MISMATCH				    449
#define USER_HAS_CASH_FUNDS					    450
#define OUTGOING_CONNECTION_NOT_FOUND			451
#define INCORRECT_FISCAL_CODE					452
#define FISCAL_CODE_ALREADY_EXISTS				453

#define LOGINTIME_LIMIT_REACHED					454	// PYR-27741

// obsolete
// #define ERR_GEOIP_MOBILE_PHONE_NOT_PROVIDED 	455  // PYR-27243
// #define ERR_GEOIP_MOBILE_PHONE_NOT_REGISTERED	456  // PYR-27243

#define PASSWORD_RESET_FAIL						457	//	PYR-27258, generic password reset failure (for NJ license)
#define LIFETIME_LIMIT_EXCEEDED					458	//	PYR-27258, Client only logs this error message
#define FISCAL_CODE_NOT_FOUND					459

#define MOBILE_PHONE_NOT_FOUND					460  // PYR-27243
#define ERR_LIVE_SUPPORT_ELIGIBLE				461  // client should show live support web page upon getting this special error code

#define LIMITED_DUE_TO_GEOIPPENDING             462  // PYR-27243
#define LIMITED_DUE_TO_GEOIPREJECTED            463  // PYR-27243

#define USER_EXISTS_ON_OTHER_LICENSE			464  // PYR-28217 must have licenseId after errDescr string
#define DRC_PROXY_CONNECTION_NOT_FOUND			465		//PYR-25970 
#define CURRENCY_CONVERSION_NEEDED				466 // PYR-26990
#define CANNOT_CHANGE_SEATED_USER_IMAGE			467 // PYR-26568

#define CANNOT_RETRIEVE_USERID_STRONG_AUTH_REQD 468	//	PYR-28472
#define TRANSACTION_NOT_FOUND_2					469 // PYR-23644

#define IMAGE_HAS_MULTIPLE_RESOLUTIONS			470 // PYR-26568

#define USER_LOGIN_VIA_EMAIL					472 // PYR-28548 its errDescr is base64-encoded errExplBody (looks like garbage if printed)
#define TRANSACTION_REJECTED					473 // PYR-29055

// PYR-25565
#define	AUTODEAL_WRONG_GENERATION				474
#define	AUTODEAL_WRONG_STATE					475 // a command is not expected in the current state
#define	AUTODEAL_NOT_DEAL_MANAGER				476 // only a deal manager may send this command, and you are not a deal manager
#define	AUTODEAL_WRONG_PLAYER					477 // just not a player playing in the tournament
#define	AUTODEAL_WRONG_DEAL						478
#define	AUTODEAL_OTHER_ERROR					479

#define	ERR_DUPLICATE_REQUEST					480

#define AUTODEAL_PENDING_DBM_REQUEST            481 // PYR-25565
#define AUTODEAL_ALREADY_COMPLETED              482 // PYR-25565

#define ERR_STARCODE_PROGRAM_CONFLICT			483	// PYR-25799
#define NO_SUCH_ADMIN							484
#define IDEMPOTENCY_FAILURE						485
#define XC_TRANS_INCONSISTENT_HANDID			486 // bet or pay for handId != active handId
#define XC_ORIG_GAME_NOT_FOUND					487
#define XC_ORIG_GAME_WRONG_STATE				488

#define ERR_NO_SUCH_FACTORY						489
#define ERR_FACTORY_NOT_CONNECTED				490
#define ERR_FACTORY_IS_DOWN						491

#define ACCOUNT_TEMPORARY_BLOCKED				492 // PYR-30365
#define RECORD_NOT_FOUND						493 // PYR-29620  - can be used for generic purpose, better have Table name and PK in sqlErr
#define NOT_ALLOWED_AT_SAG_TOURNS				494	// PYR-30225
#define XC_GAME_WRONG_STATE						495
#define XC_CASINO_NOT_ALLOWED					496 // PYR-29620
#define DEAL_TOTAL_LIMIT_EXCEEDED				497 // PYR-27091
#define DEAL_PER_USER_LIMIT_EXCEEDED			498 // PYR-27091

#define PS3V_NOT_ALLOWED					    499 // PYR-26839
#define PLAYER_NOT_LOGGED_IN                    500 // PYR-29620

#define LIMITED_DUE_TO_GEOIPUNKNOWN             501 // PYR-27243
#define LIMITED_DUE_TO_GEOIPSOFTWAREBLOCKED     502 // PYR-27243
#define LIMITED_DUE_TO_GEOIPUSINGPROXY		    503 // PYR-27243

#define DBM_ACCOUNT_CLOSED_LICENSE_MISMATCH		504	//	PYR-28389
#define VALUE_OUT_OF_RANGE                      505
#define INTERSITE_CONN_DOWN                     506 // PYR-32341
#define ERR_USER_HAS_ROLL						507

#define ROUTERMAC_FROZEN						508 // PYR-30499: user is trying to login with a router MAC address that is among frozen ones (table "frozenobjects")

#define ERR_SMS_VALIDATION_REQUIRED				509 // PYR-32891
#define ERR_USER_IN_LIMITED_MODE				510 // PYR-32891
#define DBM_ACCOUNT_CLOSED_2                    511 // PYR-33448
#define COOLING_PERIOD_IN_EFFECT				512 // PYR-33955

#define NOT_ALLOWED_PLAYING_RM					513 // PYR-34948

#define PS3V_REGISTER_ERROR                     514 // PYR-34321: generic PS3V error code for registration process. 
#define RMOK_SET_ID_NOT_VERIFIED                515 // PYR-33645
#define NO_BUNDLES_FOR_LICENCE_COUNTRY_YEAR		516 // PYR-32027
#define NOT_FPP_BUNDLE_USER						517 // PYR-32027
#define HOSTNAME_DNS_ERROR                      518 // PYR-34138
#define SSL_REQ_RESP_NOT_CONNECTED_ERROR        519 // PYR-34138
#define DBM_DBDEADLOCK_EXCEPTION				520 // PYR-36823
#define DBM_DBDISCONNECT_EXCEPTION				521 // PYR-36823

#define REGISTRATION_PAUSED						522 // PYR-55808
#define USER_BANNED_BY_REGULATOR_AND_RM_SUSPENDED 523 // PYR-71889
#define ERR_USER_SUSPENDED_UNTIL_SUBMIT_DOCS	524 // PYR-84747

// range 525-531 - available

#define ES_ACCOUNT_HISTORY_INVALID_LICENSE		532 // PYR-35275
// PYR-56967
#define ERR_ES_NIC_RESULT_INCORRECT_FIRST_NAME       533
#define ERR_ES_NIC_RESULT_INCORRECT_LAST_NAME        534
#define ERR_ES_NIC_RESULT_INCORRECT_SECOND_LAST_NAME 535
#define ERR_ES_NIC_RESULT_INCORRECT_DOB              536
#define ERR_ES_NIC_RESULT_ID_DUPLICATE_FOUND         537

#define LIMITED_DUE_TO_EXCLUSION_BOUNDARY       538 // PYR-110584

#define ERR_TRY_AGAIN							539 // PYR-38100

#define ERR_DEGAS_REPORT_DMI					540 // PYR-38496

#define ERR_INVALID_CHARACTER		            541 // PYR-36008   

#define ERR_ONLY_ALLOWED_ON_NJ_SITE	            542 // PYR-39772

#define ERR_VIPSTORE_UNIQUE_ROLL_USED			543	// PYR-38673

#define ERR_FULLTILT_PAIRED_TO_CLOSED_PS_ACCOUNT		544 // PYR-39878
#define ERR_FULLTILT_ACCOUNT_CONVERSION_NOT_ALLOWED		545 // PYR-39878
#define ERR_FULLTILT_EMAIL_MATCHES_TO_CLOSED_PS_ACCOUNT 546 // PYR-39878

#define BG_LOGIN_HISTORY_INVALID_LICENSE				547 // PYR-38484

#define ERR_FULLTILT_USER_BLOCKED					    548 // PYR-41600
#define ERR_FULLTILT_FISCAL_CODE_MATCHES_MULTIPLE_PS_ACCOUNTS 549 // PYR-40462
#define ERR_INSUFFICIENT_ADMIN_RIGHTS           550 // PYR-40386
#define ERR_MINIGAME_ALREADY_EXISTS				551 // PYR-37300
#define ERR_MINIGAME_INVALID_FEE				552 // PYR-37300
#define ERR_MINIGAME_INVALID_FEEAMOUNT			553 // PYR-37300
#define ERR_MINIGAME_TOO_MANY_ACCOUNT_ADJUSTS	554 // PYR-37300
#define ERR_MINIGAME_UNKNOWN_ACCOUNTID			555 // PYR-37300
#define ERR_MINIGAME_ALREADY_OVER               556 // PYR-37300
#define ERR_MINIGAME_UNSUPPORTED_PRIZE			557 // PYR-37300
#define ERR_MINIGAME_INCORRECT_ACCOUNT_ADJUSTS	558 // PYR-37300
#define ERR_MINIGAME_UNSUPPORTED_REASON			559 // PYR-37300
#define ERR_MINIGAME_MISCONFIGURED_ACCOUNT		560 // PYR-37300
#define ERR_MINIGAME_CURRENCY_MISMATCH          561 // PYR-37300
#define ERR_FULLTILT_USD_VALET_PROHIBITD 		562 // PYR-39878
#define ERR_FULLTILT_USER_FILTERED_OUT	 		563 // PYR-43038
#define ERR_FULLTILT_CONVERSION_NOT_ALLOWED_PT	564 // PYR-42379
#define ERR_WRONG_BRANDID						565 // PYR-42352
#define ERR_USER_HAS_POOL_BLOCK_TIME_PENALTY	566 // PYR-42791
#define ERR_NOT_ALLOWED_LOWER_CLIENT_VERSION	567	// PYR-39535
#define ERR_FULLTILT_TOO_MANY_LIMITS            568	// PYR-40515
#define ERR_TS_TEMPLATE_ERROR					569 // PYR-44170
#define TEMP_ACCOUNT_LIMIT_EXCEEDED				570 // PYR-43140

#define ERR_TRY_AGAIN_DO_NOT_REPLY				571 // PYR-45147
#define ERR_USER_POOL_BLOCKED					572 // PYR-43054
#define ERR_NO_SUCH_SEATFINDER					573 // PYR-43054

#define ERR_MINIGAME_PRIZE_LIMIT_REACHED        574 // PYR-45636 
#define ERR_MINIGAME_UNAVAILABLE                575 // PYR-45636

#define ERR_VIPSTORE_UNIQUE_BONUS_USED			576	// PYR-37884

#define ERR_NO_SUCH_GAMETICKETTYPE				577	// PYR-45645
#define ERR_NO_SUCH_GAMETICKET					578	// PYR-45645
#define AT_TOO_MANY_RING_TABLES					579 // PYR-44990
#define AT_TOO_MANY_BLITZ_ENTRIES				580 // PYR-44990

#define ERR_SCHED_BROADCAST_TIME_IN_PAST		581 // PYR-44930
#define ERR_SCHED_BROADCAST_MODIFIED_PAST_TIME  582 // PYR-44930
#define ERR_EXCEEDED_SCHED_BROADCAST_LIMIT		583 // PYR-44930
#define ERR_EXCEEDED_SCHED_BROADCAST_SIM_LIMIT	584 // PYR-44930

#define NOT_ENOUGH_CHIPS_BUT_ENOUGH_ON_ALL_ACCOUNTS 585 // PYR-44656

#define NOT_ALLOWED_AT_KO_TOURNS 586 //PYR-43888
#define ERR_USERROLL_NOT_BOUND_TO_GAME 587
#define ERR_USERROLL_ALREADY_BOUND_TO_GAME 588

#define NO_BOUNTY_TOURN_TARGET					589 // PYR-31658
#define ERR_ADMISSIONS_OBSOLETE					590 // PYR-22549
#define SPECIAL_4_MAX_CONFLICT					591 // PYR-45970
#define ERR_BG_REGISTRATION_FAILED				592 // PYR-46521
#define ERR_BG_RMF_DOCS_REQUIRED				593 // PYR-46521
#define ERR_CUSTOMER_DUE_DILIGENCE				594 // PYR-47620 has customerDueDiligenceFlag3
#define ALREADY_REGISTERED_IN_TARGET            595 // PYR-38597

//	PYR-47770
#define ERR_DAILY_LOSS_LIMIT_REACHED			596		//	Net loss per day reached, Live Games (Poker & Casino)
#define ERR_MONTHLY_LOSS_LIMIT_REACHED			597		//	Net loss per month reached, Live Games (Poker & Casino)
#define ERR_DAILY_SPEND_LIMIT_REACHED			598		//	Spending limit per day reached, Live Games (Poker & Casino)
#define ERR_MONTHLY_SPEND_LIMIT_REACHED			599		//	Spending limit per monthly reached, Live Games (Poker & Casino)

#define FREESPINBONUS_NOT_VALID					600 // PYR-48946
#define ERR_MINIGAME_TICKET_EXPECTED			601 // PYR-47643

#define ERR_NOT_CVL_USER						602 // PYR-45659
#define USER_HAS_UNOPEN_CHESTS					603 // PYR-50736 operation cannot be completed until user has uncleared chests
#define ERR_USER_IS_NOT_ARCHIVED		    	604 // PYR-44386 returned on attempt to save pending action for not archived user
#define ERR_UNKNOWN_PENDING_ACTION_TYPE    		607 // PYR-44386 returned when unknown pending action type received from admin tool
#define USER_HAS_ASSETS							608 // PYR-50419 operation cannot be completed until user has assets
#define USER_HAS_BONUS_ASSETS					609 // PYR-50804 operation cannot be completed until user has assets

#define PAYWALL_MISSING_NAME					610 // PYR-46043 required name is missing
#define PAYWALL_NO_SUCH_OBJECT					611 // PYR-46043 segment with specified id not found among live ones
#define PAYWALL_INVALID_CHIP_AMT				612 // PYR-46043 bundle chip amount is not specified or not positive
#define PAYWALL_PRICE_REQUIRED					613 // PYR-46043 required price is missing
#define PAYWALL_PRIORITY_REQUIRED				614 // PYR-46043 required priority is missing
#define PAYWALL_OBJECT_ASSIGNED					615 // PYR-46043 bundle/segment assigned to paywall
#define ERR_MINIGAME_PAUSED						616 // minigame is paused
#define ERR_MINIGAME_UPDATE_ALREADY_PROCESSED	617 // minigame update already processed
#define ERR_TOO_MANY_BLITZ_ENTRIES				618 // PYR-47880

#define USER_NOT_VIP_PLAYER						619 // PYR-50347, PYR-50357, PYR-50344, PYR-50412
#define SOCIAL_MEDIA_NOT_FOUND					620 // PYR-50412
#define SOCIAL_MEDIA_NOT_ACTIVE					621 // PYR-50412
#define FAVORITE_NOT_FOUND						622 // PYR-50344
#define FAVORITE_NOT_ACTIVE						623 // PYR-50344

#define ERR_VIPSTORE_UNIQUE_GAME_TICKET_USED	624	// PYR-45715

#define ERR_CLIENT_NOT_CONNECTED				625	// PYR-52641
#define ERR_NOT_SUPPORTED_BY_CLIENT				626	// PYR-52641

#define ERR_CVL_USER_EXCLUDED 					627 //PYR-51534
#define DOCUMENT_ID_ALREADY_EXISTS				628 // PYR-53555
#define ERR_BONUS_REQUIRES_MIN_DEPOSIT			629 // PYR-51199, user provided code, but deposit is less than the minimum amount for the bonus
#define ERR_RG_LIMITS_NOT_SET					630 // PYR-51502, For CZ all responsible gaming limits must be explicitly set to play RM games.
#define NOT_ENOUGH_CHIPS_TO_PAY_TAX				631 // PYR-30309

#define ERR_NO_AUTH_PAN_NOT_STORED				632 // PYR-53807
#define ERR_NO_AUTH_PAN_STORED_NOT_VERIFIED		633 // PYR-53807
#define ERR_NO_AUTH_PAN_VERIFIED				634 // PYR-53807
#define ERR_AUTH_PAN_NOT_STORED					635 // PYR-53807
#define ERR_AUTH_PAN_STORED_NOT_VERIFIED		636 // PYR-53807

#define ERR_APC_ON_DIFF_DEVICE					637 // PYR-52886, Authenticated Poker Connection exists on a different device
#define ERR_APC_ON_SAME_DEVICE					638 // PYR-52886, Authenticated Poker Connection exists on same device
#define ERR_TRANSFER_APC_NOT_READY				639 // PYR-52886, Transfer of Authenticated Poker Connection is in progress

#define ERR_STARCODE_SUPPRRESS_MESSAGE			640 // PYR-55698

#define ERR_EXT_LIABILITY_NOT_READY				641 // PYR-51686
#define ERR_TAX_AMOUNT_CHANGED					642 // PYR-30309
#define FR_TEMP_NOT_ALLOWED_IN_SL				643 // PYR-55375
#define ERR_INSUFFICIENT_FUNDS					644 // PYR-55137 used only for new unified NSF dialog in desktop client
#define ERR_PARTIAL_SUCCESS						645 // PYR-53641
#define NOT_ALLOWED_AT_POWERUP					646 // PYR-45178
#define ERR_BRANCH_NOT_VALID					647 // PYR-56075
#define ERR_BANK_ACCOUNT_NOT_VALID				648 // PYR-56075
#define ERR_PGAD_ACCOUNT_UPDATE                 649 // PYR-57116

#define ERR_UNABLE_TO_CHECK_BLACKLIST			650 // PYR-50999
#define ERR_EXTERNALUSERFOUND_LOCALNOTFOUND		651 // PYR-52518
#define ERR_EXTERNAL_USER_NOT_FOUND				652 // PYR-52518
#define ERR_RECURSION_LEVEL_EXCEEDED			653 // PYR-66089

#define LOBBY_PREPARING_FOR_SHUTDOWN			654 // PYR-72417
#define LOBBY_NOT_RESTARTING					655 // PYR-72417

#define NOT_ALLOWED_AT_UNFOLD_TABLES			656	// PYR-70870 
#define NOT_ALLOWED_FOR_GDPR_READY				657	// PYR-73732

#define ERR_DOCUMENT_EXPIRED                    658 // PYR-67030
#define NOT_ALLOWED_CURRENCY_FOR_MIGRATION		659 // PYR-61982 user has a currency that would not be allowed by MultiCurrencySettings2 config post migration. ErrDesc is offending currency.
#define ERR_SE_BANKID_ERROR						660 // PYR-78110
#define ERR_RMF_DOCS_REQUIRED                   661 // PYR-82301
#define TEMP_ACCOUNT_LIMIT_EXCEEDED_WITH_BODY   662 // PYR-76449
#define ERR_MUCHBETTER_INVALID_PHONE            663 // PYR-83940
#define ERR_NO_CARDBIN_RECORD                   664 // PYR-74280
#define ERR_STARSWALLET_AUTH_REQUIRED           665 // PYR-51865
#define ERR_DOCUMENTS_INVALID                   666 // PYR-84500
#define INTEGRITY_VIOLATION_ATTEMPT             667 // PYR-89770
#define PWD_TOO_WEAK							668 // PYR-89685
#define APPLEPAY_MERCHANT_VALIDATION_FAIL		669 // PYR-36843
#define APPLEPAY_DECRYPT_PAYMENT_TOKEN_FAIL		670 // PYR-36843
#define X3D_SECURE2_DATA_REQUIRED_WITH_BODY		671 // PYR-80344 Cashier -> Client
#define TEMP_ACCOUNT_CASHOUT_LIMIT_EXCEEDED		672 // PYR-89627
#define ERR_USER_IDED_AS_DECEASED               673 // PYR-90007
#define ERR_TRANSFER_APC_ERROR					674 // PYR-99738, Transfer of Authenticated Poker Connection failed/timed out

#define TABLE_SEAT_RESERVATION_CLIENT_LOGGED_OUT        675  // PYR-97209 
#define TABLE_SEAT_RESERVATION_CLIENT_CONNECTION_LOST   676  // PYR-97209
#define TABLE_SEAT_RESERVATION_RAT_HOLLING              677  // PYR-97209
#define TABLE_SEAT_RESERVATION_NOT_ENOUGH_FUNDS         678  // PYR-97209
#define TABLE_SEAT_RESERVATION_CLIENT_TIMEOUT           679  // PYR-97209  
#define TABLE_SEAT_RESERVATION_BASIC_CHECK_FAILED       680  // PYR-97209
#define TABLE_SEAT_RESERVATION_AAMS_ERROR               681  // PYR-97209
#define TABLE_SEAT_RESERVATION_TABLE_CLOSING            682  // PYR-97209
#define TABLE_SEAT_RESERVATION_UNDETECTED_ERROR         683  // PYR-97209 // NOTE: not for use by Dali

#define CANNOT_REGISTER_REMOVED_USER				    684  // PYR-103079 
#define ERR_FUNCTIONALITY_NOT_IMPLEMENTED			    685  // PYR-92691

#define SPEND_LIMIT_DAY_VS_WEEK_ERROR                   686  // PYR-101001
#define SPEND_LIMIT_DAY_VS_MONTH_ERROR                  687  // PYR-101001
#define SPEND_LIMIT_WEEK_VS_MONTH_ERROR                 688  // PYR-101001

#define ERR_USER_HAS_UNSETTLED_SB_BET					689  // PYR-92691

#define TABLE_SEAT_FINDER_NOT_FOUND                     690  // PYR-109407
#define ERR_FULLTILT_USER_ANONYMIZED					691  // PYR-84803
#define ERR_PLAID_GET_ENVIRONMENT_FAIL					692  // PYR-116520
#define ERR_PLAID_EXCHANGE_TOKEN_FAIL					693  // PYR-116520
#define ERR_PLAID_AUTH_FAIL								694  // PYR-116520
#define ERR_PLAID_BANK_INFO_CONFLICT					695  // PYR-116520
#define ERR_PLAID_TEMPORARILY_UNAVAILABLE				696  // PYR-116520
#define ERR_PLAID_INVALID_PLAID_DATA					697  // PYR-116520

#define ERR_INVALID_CARDINFOS							698  // PYR-120453
#define ERR_CARD_NOT_VERIFIED							699  // PYR-121725

#define PPP_ERROR_SOME_FAILURES							750 // PYR-109327 - TODO: remove gap
#define USER_NOT_ALLOWED_AT_PPP		                    751 // PYR-118758

#define TOURN_SPEND_LIMIT_EXCEEDED	                    752 // PYR-90566


#define USER_LOGIN_ERROR_TYPO			(_COMM_ERROR_USERERROR + 1) // 3841 // errDescr=srvUserId, Body: s4s srvUserId eLoginCredentialType srvEmail (empty for Type_IserId)
// Obsolete #define USER_LOGIN_LICENCE_MISMATCH		(_COMM_ERROR_USERERROR + 2) // 3842 translation of ACCT_LICENSE_MISMATCH
#define USER_LOGIN_ACCT_CONVERTED		(_COMM_ERROR_USERERROR + 3) // 3843 translation of ACCT_WAS_CONVERTED
#define USER_LOGIN_GRACE_EXPIRED		(_COMM_ERROR_USERERROR + 4) // 3844 Grace period has expired
#define USER_LOGIN_RSA_REQUIRED			(_COMM_ERROR_USERERROR + 5) // 3845 RSA login method has to be used instead of Pwd/Sid
#define USER_LOGIN_NOT_ALLOWED			(_COMM_ERROR_USERERROR + 6) // 3846 connection is not allowed due to limited access login
#define USER_LOGIN_CASHIER_CLOSED		(_COMM_ERROR_USERERROR + 7) // 3847 connection is not allowed: cashier is closed for public
#define USER_LOGIN_EXCLUDED_STATE		(_COMM_ERROR_USERERROR + 8) // 3848 connection is not allowed: user's state is listed as excluded
#define USER_LOGIN_BIRTHDAY_REQUIRED	(_COMM_ERROR_USERERROR + 9) // 3849  User's birthday is required to process authorization request
#define USER_LOGIN_BIRTHDAY_INVALID		(_COMM_ERROR_USERERROR + 10) // 3850  User's birthday is invalid. Authorization declined
#define USER_LOGIN_LIMIT_REACHED		(_COMM_ERROR_USERERROR + 11) // 3851  #15250. Playing time limit reached. Authorization declined.
#define USER_LOGIN_TABLE_BLOCKED		(_COMM_ERROR_USERERROR + 12) // 3852  #19968. Table login is blocked due to a blocking level in [blocking2]. Authorization declined.
#define USER_LOGIN_MUTE_ERROR			(_COMM_ERROR_USERERROR + 13) // 3853 
#define USER_LOGIN_EXPLICIT_DK_REQUIRED	(_COMM_ERROR_USERERROR + 14) // 3854  Explicit login to .DK site is required #16706
#define USER_LOGIN_OBJECT_PREPARING_FOR_SHUTDOWN (_COMM_ERROR_USERERROR + 15)	// 3855 server object (regulator for #22591) is in 'prepare for shutdown' mode
#define USER_LOGIN_USER_ALREADY_CONNECTED		 (_COMM_ERROR_USERERROR + 16)	// 3856  #23163: cashier-related only, same userId already connected to cashier
#define USER_DISCONNECTED_BY_SAME_USERID		 (_COMM_ERROR_USERERROR + 17)	// 3857  #23163: cashier-related only, same userId disconnected this connection
#define USER_WEB_CASHIER_NEVER_USED_MOBILE_PLATFORM	(_COMM_ERROR_USERERROR + 18)// 3858  PYR-23460 user has never used mobile platform so he cannot use web cashier
#define USER_LOGIN_NOT_REGISTERED				(_COMM_ERROR_USERERROR + 19)	// 3859 PYR-22885 user can't open perm. connection because not registered (mtl only)
#define USER_LOGIN_ALREADY_FINISHED				(_COMM_ERROR_USERERROR + 20)	// 3860 PYR-22885 user can't open perm. connection because already finished (mtl only)
#define USER_LOGIN_TMP_PWD_EXPIRED				(_COMM_ERROR_USERERROR + 21)	// 3861 PYR-22184 user temp password is expired.
#define USER_LOGIN_LIMITED_DUE_TO_INSTALLID		(_COMM_ERROR_USERERROR + 22)	// 3862 PYR-21829 user (table) login not allowed due to unvalidated install id.
#define USER_LOGIN_BLOCKED_BY_CODE				(_COMM_ERROR_USERERROR + 23)	// 3863 PYR-24812 user cannot log in because his blocking code != 0
#define USER_LOGIN_BLOCKED_LOWER_CLIENT_VERSION	(_COMM_ERROR_USERERROR + 24)	// 3864 PYR-25260 user cannot log in because the client version is lower than minimum required version
#define USER_LOGIN_BLOCKED_GUEST_ACCOUNT		(_COMM_ERROR_USERERROR + 25)	// 3865 PYR-25263 feature not allowed for guest accounts //obsolete by PYR-91342
#define USER_LOGIN_GUEST_ACCOUNT_TABLE_LIMIT	(_COMM_ERROR_USERERROR + 26)	// 3866 PYR-25263 table limit for guest user reached //obsolete by PYR-91342
#define USER_LOGIN_LIMITED_DUE_TO_GEOIPPENDING	(_COMM_ERROR_USERERROR + 27)	// 3867 PYR-27243 user login not allowed due to GeoIp pending
#define USER_LOGIN_LIMITED_DUE_TO_GEOIPREJECTED	(_COMM_ERROR_USERERROR + 28)	// 3868 PYR-27243 user login not allowed due to GeoIp rejected
#define USER_LOGIN_BLOCKED_STRONG_AUTH_REQD		(_COMM_ERROR_USERERROR + 29)	// 3869 PYR-27258 user login not allowed because the "Strong Authentication Required" user privilege is set
#define USER_LOGIN_ITALY_USER_LOGIN_BLOCKED		 (_COMM_ERROR_USERERROR + 31)	// 3871 PYR-28691 .IT user login not allowed because AAMS connection down
#define USER_LOGIN_BLOCKED_REVALIDATION_REQD	 (_COMM_ERROR_USERERROR + 32)	// 3872 PYR-31219 user login not allowed because account revalidation is required
#define USER_LOGIN_LIMITED_DUE_TO_GEOIPUNKONWN			(_COMM_ERROR_USERERROR + 33)	// 3873 PYR-27243 user login not allowed due to GeoIp uknown
#define USER_LOGIN_LIMITED_DUE_TO_GEOIPBLOCKEDSOFTWARE	(_COMM_ERROR_USERERROR + 34)	// 3874 PYR-27243 user login not allowed due to GeoIp blocked software
#define USER_LOGIN_LIMITED_DUE_TO_GEOIPUSINGPROXY		(_COMM_ERROR_USERERROR + 35)	// 3875 PYR-27243 user login not allowed due to GeoIp using proxy
#define USER_LOGIN_BLOCKED_NJ_ON_IOM			(_COMM_ERROR_USERERROR + 36)	// 3876 PYR-28389 NJ user login not allowed on IOM site
#define USER_CONNECTION_ERROR_GEOIPUNKNOWN		(_COMM_ERROR_USERERROR + 37)	// 3877 PYR-27243 error code in server disconnecting player connection due to GeoIpUnknown  
#define USER_LOGIN_BLOCKED_NJ_LOGIN_LIMIT_REACHED		(_COMM_ERROR_USERERROR + 38)	// 3878 PYR-31758 NJ user login not allowed due to maximum number of normal logins reached
#define USER_LOGIN_BLOCKED_HTML5_CLIENT			(_COMM_ERROR_USERERROR + 39)	// 3879 PYR-33221 User login blocked as it logs in with html5 client and has priv2NoHTML5Client privilege
#define USER_TABLE_LOGIN_BLOCKED_NOT_HTML5_CLIENT       (_COMM_ERROR_USERERROR + 40)    // 3880 PYR-33236 only html5 client can connect using webtokenint guard
#define USER_TABLE_LOGIN_BLOCKED_HTML5_CLIENT           (_COMM_ERROR_USERERROR + 41)    // 3881 PYR-33236 html5 client not allowed at this table
#define USER_FORCED_DISCONNECT_AS_WEBTOKEN_EXPIRED      (_COMM_ERROR_USERERROR + 42)    // 3882 PYR-33228 html5 client web token was expired or no longer valid at this table
#define USER_FORCED_DISCONNECT_AT_SHUTDOWN      (_COMM_ERROR_USERERROR + 43)    // 3883 PYR-36277 server object sends logouts at shutdown
#define USER_LOGIN_GOTO_REGULATOR_FIRST         (_COMM_ERROR_USERERROR + 44)    // 3884 PYR-39475
#define USER_LOGIN_BLOCKED_BANNED_BY_REGULATOR	(_COMM_ERROR_USERERROR + 45)	// 3885 PYR-40995 
#define USER_LOGIN_FTPUSER_ALREADY_MIGRATED		(_COMM_ERROR_USERERROR + 46)	// 3886 PYR-40997
#define USER_LOGIN_BLOCKED_TOO_MANY_LOGINS		(_COMM_ERROR_USERERROR + 47)	// 3887 PYR-43160
#define USER_LOGIN_BLOCKED_TOO_MANY_LOGINS_PER_OBJTYPE_AND_ID	(_COMM_ERROR_USERERROR + 48)	// 3888 PYR-43375
#define USER_LOGIN_BLOCKED_ONLY_STORM_ALLOWED   (_COMM_ERROR_USERERROR + 49)    // 3889 PYR-44178 storm app can only login to storm objects (based on appTypeId)
#define USER_LOGIN_BLOCKED_PT_RSA_NOT_ALLOWED	(_COMM_ERROR_USERERROR + 50)	// 3890 PYR-44757 .PT cannot login with RSA token
#define USER_LOGIN_BLOCKED_LOGIN_TOTAL_LIMIT_REACHED (_COMM_ERROR_USERERROR + 51)	// 3891 PYR-47770 login total reached
#define USER_LOGIN_BLOCKED_LOGIN_TIME_LIMIT_REACHED  (_COMM_ERROR_USERERROR + 52)	// 3892 PYR-47770 login time reached
#define USER_LOGIN_BLOCKED_RWS_CLIENT           (_COMM_ERROR_USERERROR + 53)	// 3893 PYR-47376 RWS not allowed to connect (based on appTypeId) as auto deals disabled
#define USER_LOGIN_BLOCKED_SILENT_LOGIN_NOT_ALLOWED (_COMM_ERROR_USERERROR + 54)	// 3894 PYR-52912 Silent login not allowed
#define USER_FORCED_DISCONNECT_AS_SEAT_REMOVED_ON_TABLE (_COMM_ERROR_USERERROR + 55)  // 3895 PYR-46464 general errcode in server disconnecting player connection when the player is kicked from table
#define USER_FORCED_DISCONNECT_AS_DUPLICATED_CONN_ON_TABLE (_COMM_ERROR_USERERROR + 56)  // 3896 PYR-46464 general errcode in server disconnecting player connection when the player has diuplicated table connection
#define USER_LOGIN_BLOCKED_BY_EXT_OPERATOR				(_COMM_ERROR_USERERROR + 57)	// 3897 PYR-52518 login blocked by external operator
#define USER_LOGIN_BLOCKED_SERVICE_NOT_AVAILABLE		(_COMM_ERROR_USERERROR + 58)	// 3898 PYR-52518 login blocked for technical reasons, I.e. gateway down
#define USER_TABLE_LOGIN_BLOCKED_OBSERVERS		(_COMM_ERROR_USERERROR + 59)	// 3899 PYR-106138 observer connection is blocked on table server (usually on stealth table)
#define USER_LOGIN_LIMITED_DUE_TO_EXCLUSION_BOUNDARY		(_COMM_ERROR_USERERROR + 60)	// 3900 PYR-103540 user login not allowed due to GeoIp exclusion boundary
#define USER_LOGIN_BLOCKED_LOGIN_COOL_OFF					(_COMM_ERROR_USERERROR + 61)	// 3901 PYR-105467 user login not allowed due to login cool off

// PYR-27414
#define AAMS_TRY_AGAIN_ERROR		9001

//possible outcomes of user's registration to tournament
#define REGISTERED_NOT  			0 //does not need registration
#define REGISTERED_NORMAL			1 //ok
#define REGISTERED_REFUND			2 // was alredy registered by paying buyIn. refund made
//#define REGISTERED_CHIPS_ADDED		3 // won another satellite, chips were added
#define REGISTERED_ALREADY			4 // already registered and trying to pay buyin again
#define REGISTERED_ERROR			5 // screwed up...
#define REGISTERED_REFUND_FPP		6 // was alredy registered by redeeming FPP, refund made to FPP
#define REGISTERED_CHIPS_NOT_ADDED	7 // won another satellite, chips were NOT added because tournament was not configured to do so
#define REGISTERED_TMONEY_GIVEN		8 // won another satellite, T-Chips were given amount = buy-in + rake
//#define REGISTERED_WMONEY_GIVEN		9 // won another satellite, W-Chips were given amount = buy-in + rake
#define REGISTERED_PMONEY_GIVEN		10 // won another satellite, play money Chips were given amount = buy-in + rake
#define REGISTERED_TICKET_GIVEN		11 // tournament ticket given, valid for target tournament
#define REGISTERED_TICKET_USED		12 // tournament ticket given and used, in case of unregistration ticket will be returned
#define REGISTERED_TARGET_NOT_AVAIL	13 // satellite target cannot be registered to, T-Chips were given amount = buy-in + rake

/// maximum amount of hands retrieved from history
#define MAX_HANDS_ALLOWED			200 ///????

//
// IntelliPoker specific
//
// Error codes
#define PSIP_ERROR_USER_NOT_FOUND 1
#define PSIP_ERROR_TRANSFER_NOT_ALLOWED 2
#define PSIP_ERROR_TRANSFER_OVER_LIMIT 3
// Obsolete #define PSIP_ERROR_TRANSFER_ALREADY_PROCESSED 4
#define PSIP_ERROR_INTERNAL_ERROR 5
#define PSIP_ERROR_TRANSFER_NOT_ALLOWED_BANNED 6
#define PSIP_ERROR_TRANSFER_NOT_ALLOWED_CLOSED 7
#define PSIP_ERROR_TRANSFER_NOT_ALLOWED_EXCLUDED 8
#define PSIP_ERROR_UNKNOWN_AVATAR_CODE 9

// Transfer status
#define PSIP_TRANSFER_COMPLETED 1
#define PSIP_TRANSFER_REJECTED 2

// Codes are reassigned and moved up
//// TemplateServer error codes
//#define TS_ERROR_INTERNAL               1   // some internal processing error
//#define TS_ERROR_SMSGW_NOT_CONNECTED    2   // SMS GW is not connected
//#define TS_ERROR_SMS_GATEWAY            3   // some error from SMS GW
//#define TS_ERROR_SMS_GATEWAY_NO_RETRY   4   // should not retry after this error from SMS GW
//// PYR-18694
//#define TS_ERROR_SMS_MSG_SUBMITTED		5	// not an error, just notification that SMS was submitted.

//****************************************************************************
	// S->DBM

//Hand History Events:
#define TABLE_HISTORY_NONE                  0x00    // dummy event: won't go to database, client should ignore this event
#define TABLE_HISTORY_ACTION				0x01
#define TABLE_HISTORY_DEALING				0x02
#define TABLE_HISTORY_TIMEOUT				0x03
#define TABLE_HISTORY_ALLIN					0x04
#define TABLE_HISTORY_MOVETOPOT				0x05
#define TABLE_HISTORY_TAKEFROMPOT			0x06
#define TABLE_HISTORY_EXTRA					0x07
#define TABLE_HISTORY_TIMEBANK				0x08
#define TABLE_HISTORY_BEENSITOUT			0x09
// Extension for HandHistory on Client: 
#define TABLE_HISTORY_STARTNEWHAND			0x0A
#define TABLE_HISTORY_USERINHAND			0x0B
#define TABLE_HISTORY_ENDHAND				0x0C
#define TABLE_HISTORY_ENDHAND_PRELIM		0x0D
#define TABLE_HISTORY_STARTNEWGAME			0x0E

//Table History Events:
#define TABLE_HISTORY_JOINS					0x10
#define TABLE_HISTORY_SITIN					0x11
#define TABLE_HISTORY_CONNECT				0x12
#define TABLE_HISTORY_KICKED				0x13
#define TABLE_HISTORY_CHAT					0x14


// Extension for HandHistory on Client: 
#define TABLE_HISTORY_CHATADMIN				0x15
#define TABLE_HISTORY_CHATDEALERALWAYS		0x16
#define TABLE_HISTORY_CHATDEALERMIN			0x17
#define TABLE_HISTORY_CHATDEALERMAX			0x18
#define TABLE_HISTORY_CHATUSER				0x19
#define TABLE_HISTORY_CHATOBSERVER			0x1A
#define TABLE_HISTORY_CHATMANAGER			0x1B
#define TABLE_HISTORY_CHATSUPPORT			0x1C
#define TABLE_HISTORY_CHATSUPERVISOR		0x1D
#define TABLE_HISTORY_CHATHIDDEN			0x1E
#define TABLE_HISTORY_CHATCOMMENTATOR		0x1F
#define TABLE_HISTORY_CHATOPTIONAL			0x20
#define TABLE_HISTORY_CHAT_BROADCASTALL		0x21
#define TABLE_HISTORY_CHAT_BROADCASTCOM		0x22
#define TABLE_HISTORY_CHAT_BROADCASTNET		0x23
#define TABLE_HISTORY_CHAT_MODERATOR		0x24
#define TABLE_HISTORY_CHAT_TEAMPRO			0x25
#define TABLE_HISTORY_CHAT_AUTOBROADCAST	0x26
#define TABLE_HISTORY_CHAT_BROADCASTITA		0x27
#define TABLE_HISTORY_CHAT_BROADCASTFRA		0x28
#define TABLE_HISTORY_CHAT_BROADCASTEST		0x29
#define TABLE_HISTORY_CHAT_TEAMONLINE		0x2A
#define TABLE_HISTORY_CHAT_BROADCASTBEL		0x2B
#define TABLE_HISTORY_CHAT_BROADCASTSPA		0x2C
#define TABLE_HISTORY_CHAT_BROADCASTDEN		0x2D
#define TABLE_HISTORY_CHAT_BROADCASTEU		0x2E
#define TABLE_HISTORY_CHAT_BROADCASTUS_NJ	0x2F
#define TABLE_HISTORY_CHAT_AUTODEAL			0x30
#define TABLE_HISTORY_CHAT_BROADCASTUK		0x31
#define TABLE_HISTORY_CHAT_BROADCASTBUL		0x32
#define TABLE_HISTORY_CHAT_BROADCASTDE_SH	0x33
#define TABLE_HISTORY_CHAT_BROADCASTPOR		0x34
#define TABLE_HISTORY_CHAT_BROADCASTROM		0x35
#define TABLE_HISTORY_CHAT_BROADCASTCZ		0x36
#define TABLE_HISTORY_CHAT_BROADCASTRUSO	0x37
#define TABLE_HISTORY_CHAT_BROADCASTRUSOPM	0x38
#define TABLE_HISTORY_CHAT_BROADCASTINDIA	0x39
#define TABLE_HISTORY_CHAT_BROADCASTUSPA	0x3A
#define TABLE_HISTORY_CHAT_BROADCASTASIA	0x3B
#define TABLE_HISTORY_CHAT_BROADCASTSWE		0x3C
#define TABLE_HISTORY_CHAT_BROADCASTCH		0x3D
//	<<== @site2-hhchat-enum: Site enumerations for hand history chat @ ====
#define TABLE_HISTORY_CHAT_BROADCASTARBA	0x3E
#define TABLE_HISTORY_CHAT_BROADCASTUSCO	0x3F
#define TABLE_HISTORY_CHAT_BROADCASTDE		0x40
#define TABLE_HISTORY_CHAT_BROADCASTGR		0x41
#define TABLE_HISTORY_CHAT_RESERVED			0x42	//  reserved range: 0x42 - 0x4F
//	==== @site2-hhchat-enum: Site enumerations for hand history chat @ ==>>

#define TABLE_HISTORY_STORM_POWER           0x50
#define TABLE_HISTORY_STORM_EFFECT          0x51
#define TABLE_HISTORY_STORM_POWERPLAY_END   0x52

#define TABLE_HISTORY_UNFOLD_ANTERETURNED   0x60  // PYR-65734
#define TABLE_HISTORY_UNFOLD_TAKEFROMPOT    0x61  // PYR-65734
#define TABLE_HISTORY_HOUSE_TAKEFROMPOT     0x62  // PYR-95110 for the cashout player's hand wins

#define TABLE_HISTORY_THROW					0x63 // PYR-98733


/// cashtrans possible statuses
#define REJECTED_STR			"R"
#define APPROVED_STR			"A"
#define PENDING_STR				"P"
#define PENDING_OUTSIDE_STR		"Q" //transaction is in process, will be completed and we'll be notified about it. Take no action
#define APPROVED_PARTTIALLY_STR	"a"
#define MANUAL_PENDING			"M"
#define MANUAL_PENDING_BANNED	"m"
#define ERROR_STR				"E"
#define BANNED_USER_STR			"X"
#define ERROR_RESTORABLE_STR	"e"
#define CHQ_CANCELLED_STR		"C"
#define UNKNOWN_NT_STR			"U" // neteller trans, status unknown
#define VERIFIED_STR			"V" //neteller redeem or cheque
#define SENT_FORPROCESSING_STR	"S" //neteller redeem or cheque
#define CCOIN_BEING_FINALIZED	"F" //central coin - finalization started
#define TIMED_OUT_STR			"T" // EP trans timed out
#define WAITING_LAST_CHANCE		"W"
#define NOT_APPROVED_STR		"Z" // for rejected overcredits
#define CASHOUT_ON_HOLD_STR		"H" // redeem/cashout/transfer-out on hold to be reviewed by admins (no longer applicable to deposits)
#define LOCKED_STR				"L" // cashout locked; #11929
#define EMPTY_STR               " "
#define DEPOSIT_ON_HOLD_STR		"N" // PYR-34183

//status bytes 
#define STATUSBYTE_REJECTED		'R'
#define STATUSBYTE_APPROVED		'A'
#define STATUSBYTE_PENDING		'P'
#define STATUSBYTE_INCORRECT	'I'
#define STATUSBYTE_MANUAL		'M'
#define STATUSBYTE_VERIFIED		'V'

/// transType
#define STR_PURCHASE	"P"
#define STR_REDEEM		"R"

//paysystem names (cardtype)
#define PAYPAL_NAME					"PAYPAL"
#define RETURN_PAYPAL_NAME			"RET_PAYPAL"
#define PAYPAL_CANCELLEDREVERSAL    "PAYPAL_REPAYCBACK"
#define CHEQUE_NAME					"CHEQUE"
#define CREDIT_NAME					"CREDIT"
#define DEBIT_NAME					"DEBIT"
#define FIREPAY_NAME				"FIREPAY"
#define MASTERCARD_NAME				"MASTERCARD"
#define PAYPALM_NAME				"PAYPALM"
#define TRANSFER_NAME				"TRANSFER"
#define VISA_NAME					"VISA"
#define MIR_NAME					"MIR"
#define WIRE_NAME					"WIRE"
#define BONUS_NAME					"BONUS"
#define	NETELLER_NAME				"NETELLER"
#define WUNION_NAME					"WUNION"
#define LOOPBACK_NAME				"LOOPBACK"
#define AFFILIATE_NAME				"AFFILIATE"
#define CCOIN_NAME					"CENTRALCOIN"
#define TOURNDEAL					"TOURNDEAL"
#define CHARGEBACK_NAME				"CHARGEBACK"
#define REFUND_NAME					"REFUND"
#define RECREDIT_NAME				"RECREDIT"
#define DINERS_NAME					"DINERS"
#define SOLO_NAME					"SOLO"
#define TSALE_NAME					"TSALE"
#define RETURN_ACH_NAME				"RET_ACH"
#define RETURN_ELV_NAME				"RET_ELV"
#define ECHECK_NAME					"ECHECK"
#define WUCORRECT_NAME				"WUCORRECT"
#define WUADVANCE_NAME				"WUADVANCE"
#define GLOBALWIRE_NAME 			"GC_WIRE"
#define CHQ_CANCEL_NAME 			"CHQCANCEL"
#define GLOBALDIRECT_NAME			"GC_DIRECT" // ELV
#define EPASSPORTE_NAME 			"PASSPORTE"
#define RTBT_NAME					"RTBT"
#define PAYSAFE_NAME				"PAYSAFE"
#define UKASH_NAME      			"UKASH"
#define MONEYGRAM_NAME				"MONEYGRAM"
#define MGCORRECT_NAME				"MGCORRECT"
#define MONEYORDER_NAME				"MONEYORDER"
#define MRMC_NAME					"VISADEBIT"
#define MBOOKERS_NAME				"MBOOKERS"
#define USEMYBANK_NAME  			"UBANK"
#define DBT_NAME					"DIRBANK"
#define BPAY_NAME					"BPAY"
#define CLICKANDBUY_NAME			"CNBUY"
#define CNBUY_CANC_NAME 			"CNB_CANC"
#define CNB_CBACK_NAME				"CNB_CBACK"
#define CNB_CBACK_LFT_NAME			"CNB_CBACK_LFT"
#define CNB_BOUT_NAME				"CNB_BOUT"
#define CNB_2COLL_NAME				"CNB_2COLL"
#define CNB_BIN_NAME				"CNB_BIN"
#define CNB_COLLD_NAME				"CNB_COLLD"
#define CNB_NCOLLD_NAME 			"CNB_NCOLLD"
#define CITADEL_NAME				"CITADEL"
#define WEBMONEY_NAME				"WEBMONEY"
#define ENVOY_RTBT_NAME				"ENV_RTBT"
#define AFF_DEPOSIT_NAME			"AFFDEPOSIT"
#define REDEEM_RETURN				"REDEEM_RET"
#define FALSE_RETURN				"FALSE_RET"
#define TRANS_CORRECTION			"TRANSCORR"
#define DEBITADJUST_NAME			"DEBITADJUST"	 // PYR-19085
#define REPAYCHARGEBACK_NAME		"REPAYCBACK"	 // PYR-19085	
#define CREDITADJUST_NAME			"CREDITADJUST"   // PYR-19085
#define REFUNDFEE_NAME				"REFUNDFEE"      // PYR-19085
#define GOODWILLCREDIT_NAME			"GWC"			 // PYR-19085
#define PROMOFUNDS_NAME				"PROMOFUNDS" // PYR-19085
#define MG_CASHOUT_NAME				"MG_CASHOUT"
#define CLICK2PAY_NAME				"CLICK2PAY"
#define C2P_RETURN_NAME				"RET_C2P"
#define C2P_COLLECTION_NAME			"COLL_C2P"
#define ELV_REVERSED_RETURN			"REV_RET_ELV"
#define TOURN_LEAGUE_NAME			"PS_LEAGUE"
#define INTELLIPOKER_NAME			"INTELLIPOKER"
#define ENVOY_LBT_NAME				"ENV_LBT"
#define MONETA_RBT_NAME				"MONETA"
#define CARTE_BLEUE_NAME			"CARTE_BLEUE"
#define BUGSY_CONVERSION_NAME		"BUGSY"
#define ENTROPAY_NAME				"ENTROPAY"
#define TRANSFER_OUT_NAME			"TRANSFER_OUT"
#define TRANSFER_IN_NAME			"TRANSFER_IN"
#define BOLETO_NAME					"BOLETO"
#define ATAS_APPROVAL_NAME			"ATAS_LIMIT"
#define IT_CONVERSION_NAME			"IT_CONVERSION"
#define MC_TRANSFER_NAME			"MCTRANSFER"
#define INSTADEBIT_NAME				"INSTADEBIT"
#define WIRECARD_WBT_NAME			"WC_WBT"
#define INSTADEBIT_RETURN_NAME		"RET_INSTADEBIT"
#define DANKORT_NAME				"DANKORT"
#define UMWALLET_NAME				"UMWALLET"
#define CONVERT_FUNDS_NAME			"CONVERT_FUNDS"
#define ASTROPAY_NAME				"ASTROPAY"
#define TICKETPREMIUM_NAME			"TICKETPREMIUM"
#define CUENTADIGITAL_NAME			"CUENTADIGITAL" // productId not a pysystem, the paysystem is ENVOY_RTBT_NAME in Argentina
#define PAGOFACIL_NAME          	"PAGOFACIL"    // #14134 replaces CUENTADIGITAL
#define STELLAR_NAME				"STELLAR"
#define HAND_AWARD_NAME				"HANDAWARD" // #13711
#define TODITO_NAME					"TODITO" // #12627
#define QIWI_NAME					"QIWI" // #14114
#define EPAY_NAME               	"EPAY" // #14258
#define DINEROMAIL_NAME         	"DINEROMAIL" // #13440
#define FR_MIGRATION_NAME	    	"FRMIGRATION"
#define ROBOKASSA_NAME          	"ROBOKASSA" // #14606
#define PSTARSPAY_NAME          	"PSTARSPAY" // #16406
#define PAGOSONLINE_NAME        	"PAGOSONLINE" // #17121
#define RICARICA_NAME				"RICARICA" // #17270
#define LOTTOMATICA_PRODUCT_NAME	"LOTTOMATICA"
#define EPAYCARD_PRODUCT_NAME	    "EPAYCARD"
#define GIROPAY_NAME				"GIROPAY"
#define IDEAL_NAME					"IDEAL"
#define EPS_NAME					"EPS"
#define STARSCARD_NAME				"STARSCARD"
#define SOFORT_NAME					"SOFORT"
#define BANKLINKSWED_NAME			"BANKLINKSWED"
#define EUTELLER_NAME				"EUTELLER"
#define BE_MIGRATION_NAME			"BEMIGRATION"
#define ES_MIGRATION_NAME			"ESMIGRATION"
#define CZ_MIGRATION_NAME			"CZMIGRATION"
#define MIGRATION_NAME				"MIGRATION"
#define VODAFONE_NAME           	"VODAFONE"
#define O2_NAME                 	"O2"
#define TMOBILE_NAME            	"TMOBILE"
#define MBOOKOBT_NAME               "MBOOKOBT"
#define ALFABANK_NAME               "ALFABANK"
#define QIWIWALLET_NAME             "QIWIWALLET"
#define CREDITCORRECTION_NAME		"CORRECTIONCRD"   // PYR-19932
#define DEBITCORRECTION_NAME		"CORRECTIONDBT"   // PYR-19932
#define TRUSTPAY_NAME				"TRUSTPAY" // envoy rtbt paysystem #13495
#define INSTANTBANK_NAME            "INSTANTBANK"
#define WALLETONE_NAME				"WALLETONE" // #20110
#define NORDEA_NAME					"NORDEA" // part of ENVOY RTBT
#define NEMKONTO_NAME			    "NEMKONTO" // for .DK only
#define DEBIT_RMNOK_FUNDS_NAME		"DEBIT_RMNOK" // #21348
#define PSTARSTOPUP_NAME			"PSTARSTOPUP" // #21516
#define PSPREPAIDCARD_NAME          "PSPREPAIDCARD" // #21531
#define RECHARGEPS_NAME             "RECHARGEPS" // #21531
#define PSPREPAID_NAME              "PSPREPAID" // #21531
#define YANDEXMONEY_NAME            "YANDEXMONEY" // #22220
#define SEVENELEVEN_NAME            "7ELEVEN" // #22585
#define OXXO_NAME                   "OXXO" // #22585
#define PROVINCIAPAGOS_NAME         "PROVINCIAPAGOS" // #22585
#define COBROEXPRESS_NAME           "COBROEXPRESS" // #22585
#define RIPSAPAGOS_NAME             "RIPSAPAGOS" // #22585
#define PAYZONE_PRODUCT_NAME        "PAYZONE" //#22529
#define PAYPOINT_PRODUCT_NAME       "PAYPOINT" //#22529
#define RECHARGEPOKERSTARS_NAME		"RECHARGEPOKERSTARS" // #22844
#define PAYSYSTEM_FEE_NAME			"FEE" // #22732
#define FULLTILT_NAME				"FULLTILT"
#define FULLTILT_ASSET_MIGR_NAME	"FTMIGRATION" // #24070
#define MOVIPIN_NAME                "MOVIPIN" // #23888 paysystem name
#define MOBILEPAYMENT_NAME          "MOBILEPAYMENT" // #24787 paysystem name
#define ENV_BALOTO_NAME				"BALOTO"  // #24834
#define PAGOEFECTIVO_NAME           "PAGOEFECTIVO" // #23692 paysystem name
#define PSTARSPREPAID_NAME          "PSTARSPREPAID" // #24836 paysystem name
#define PAGOEFECTIVO_RETURN_NAME    "RET_PAGOEFECTIVO" // #23692 pagoefectivo chargeback
#define PSCASH_NAME                 "PSCASH" // #25850 PSCASH paysystem name
#define HIPAY_NAME                  "HIPAY" // #26072 paysystem name
#define BOKU_NAME                   "BOKU" // #26276 paysystem name
#define BOKUTHREEUK_NAME            "BOKU_THREEUK" // #26276 paysystem name
#define BOKUVODAFONE_NAME           "BOKU_VODAFONE" // #26276 paysystem name
#define BOKUO2_NAME                 "BOKU_O2" // #26276 paysystem name
#define BOKUORANGE_NAME             "BOKU_ORANGE" // #26276 paysystem name
#define UPSELL_AWARD_NAME           "AWARD" // PYR-24840 Upsell
#define PAYSYSTEM_3V_NAME		    "3V" // PYR-26837
#define PAYSYSTEM_3VREFUND_NAME		"3VREFUND" // PYR-26837
#define GATEWAY_3V_NAME				"3V" // PYR-26837
#define NJ_MIGRATION_NAME			"NJMIGRATION"
#define PAYNEARME_NAME              "PAYNEARME"
#define RESORT_NAME					"RESORTS" // PYR-27726
#define PAYSYSTEM_TAX_NAME			"TAX" // PYR-28936
#define AGMO_NAME					"AGMO"
#define SAFETYPAY_NAME				"SAFETYPAY" //PYR-29634
#define PLAYMONEY_PURCHASE_GROUPNAME	"PLAYMONEY_PURCHASE" // PYR-29144 PAYSYSTEMGROUPS.GroupName
#define DEBIT_FUNDS_RMOK_REMOVED_NAME "DEBIT_RMOK_REMOVED" // #25200
#define GAME_INTEGRITY_NAME			 "GAMEINTEGRITY" // PYR-54664
#define GOOGLEPLAY_NAME				"GPLAY"
#define APPLESTORE_NAME				"APPLE"
#define PSPAY_ASTROPAY_NAME			"PSPAY_ASTROPAY"
#define RET_PSPAY_ASTROPAY_NAME     "RET_PSPAYASTROPAY"
#define REV_PSPAY_ASTROPAY_NAME     "REV_PSPAYASTROPAY"
#define ELV_SEPA_NAME				"ELV_SEPA" // PYR-30281
#define PAYLUCKY_NAME				"PAYLUCKY" // PYR-31414
#define PSIOB_NAME				    "PSIOB" // PYR-28858
#define RET_PSIOB_NAME              "RET_PSIOB" // PYR-28858
#define VANILLA_NAME                "VANILLA" // PYR-31264
#define ROLL_PAYSYSTEM_NAME			"INST_BONUS" // PYR-31829
#define ICE_REWARD_NAME				"ICE_REWARD" // PYR-32134
#define IDEBIT_NAME					"IDEBIT" // PYR-36349
#define SBERBANK_NAME				"SBERBANK" // PYR-37082
#define VIVAPAYMENTS_NAME           "VIVAPAYMENTS" // PYR-37445
#define STARSVOUCHER_NAME           "STARSVOUCHER" // PYR-31435
#define STARSVOUCHER_GW_NAME        "STARSVOUCHER" // PYR-31435
#define RAKEBACK_NAME               "RAKEBACK" //PYR-32176
#define SIGHTLINE_NAME				"SIGHTLINE" // PYR-39651 Name of 'rescue' paysystem, and gateway
#define SIGHTLINE_NR_NAME			"SIGHTLINENR" // PYR-39934 Name of 'non-rescue' paysystem, gw Name is still "SIGHTLINE"
#define FTPPAY_NAME			        "FTPPAY" // PYR-41208
#define RET_FTPPAY_NAME             "RET_FTPPAY"
#define REV_FTPPAY_NAME             "REV_FTPPAY"
#define GATEWAY_CROSSPAY_NAME   	"CROSSPAY"      //PYR-39936
#define VANILLARELOAD_NAME          "VANILLARELOAD" //PYR-39936
#define RET_BOLETO_NAME             "RET_BOLETO" // PYR-35738
#define REV_BOLETO_NAME             "REV_BOLETO" // PYR-35738
#define NEOSURF_NAME                "NEOSURF" //PYR-45220
#define BOLETO_ASTROPAY_NAME		"BOLETO_ASTROPAY" // PYR-35738
#define RMFBANKACCT_NAME            "RMFBANKACCT" // PYR-48393 identifier for RMF payment method (not paysystem)
#define PROFEE_CARDS_NAME           "PROFEE_CARDS"
#define PROFEE_QIWI_NAME            "PROFEE_QIWI"
#define PROFEE_YANDEX_NAME          "PROFEE_YANDEX"
#define PROFEE_MOBILEPAYMENT_NAME   "PROFEE_MOBILEPMT"
#define PROFEE_WALLET_NAME          "PROFEE_WALLET"
#define RETURN_PROFEE_NAME          "RET_PROFEE"
#define PROFEE_GW_NAME              "PROFEE"
#define ECOPAYZ_NAME                "ECOPAYZ"
#define STARSWALLET_NAME            "STARSWALLET" // PYR-37328
#define LIVEEVENTPAYOUT_NAME        "LIVEEVENTPAYOUT" // PYR-47840
#define TECHPROCESS_GW_NAME         "TECHPROCESS" // PYR-35615
#define IN_RTBT_NAME                "IN_RTBT" // PYR-35616
#define IN_WALLET_NAME              "IN_WALLET" // PYR-35616
#define CASHFREE_NAME               "CASHFREE" // PYR-55411 paysystem name (gateways have suffixes _HOSTED & _DBT)
#define MUCHBETTER_NAME             "MUCHBETTER" // PYR-61756
#define PAYSYSTEM_AG_NAME           "AG" // PYR-52150
#define PAYMONEY_YANDEX_NAME        "PAYMONEY_YANDEX" // PYR-66127
#define ADMIN_CASH_ADJUST_NAME      "ADMCASHADJ" // PYR-64665
#define ADMIN_WIN_ADJUST_NAME       "ADMWINADJ" // PYR-64665
#define VANILLA_DIRECT_NAME         "VANILLADIRECT" // PYR-73312
#define GAMERSCARD_NAME             "GAMERSCARD" // PYR-60047 for Client only. Processed as MASTERCARD_NAME
#define SE_MIGRATION_NAME			"SEMIGRATION" // PYR-78227
#define CUP_RTBT_NAME				"CUP_RTBT" // PYR-69195
#define GENERIC_CREDITCARD_NAME		"CREDITCARD" // PYR-74280 for Client only. Saved in PRODUCTID
#define GENERIC_DEBITCARD_NAME		"DEBITCARD"  // PYR-74280 for Client only. Saved in PRODUCTID
#define RUPAY_NAME					"RUPAY"      // PYR-74280
#define CASHFREE_NETBANKING_NAME	"CASHFREE_NETBANK" // PYR-70255
#define CASHFREE_UPI_NAME			"CASHFREE_UPI"     // PYR-70255
#define CASHFREE_MOBILEWALLET_NAME	"CASHFREE_MWALLET" // PYR-70255
#define PAYTM_NAME                  "PAYTM" // PYR-55412
#define APPLE_PAY_NAME              "APPLE_PAY" // PYR-36843
#define SWISSGW_NAME				"GIMO" // PYR-90603
#define GIMO_TRANSFER_NAME			"CH_TRANSFER" // PYR-90603 - paysystem for transfer in/out between poker and casino
#define GIMO_PAYMENT_NAME			"CH_PAYMENT" // PYR-90603 - paysystem for deposit/cashout between poker and user's (bank) account
#define AG_PRODUCTID_AUTO			"AUTO_PROCESSED" // PYR-79428, PYR-98700 - productid for AsiaGaming gateway initiated transfer
#define SWISH_PAYSYSTEM_NAME		"SWISH" // PYR-98052 - paysystem for deposit/cashout between poker and user's Swish account
#define SE_SWISH_GW_NAME			"SWISH" // PYR-98052 - GW popular in Sweden
#define INTERAC_PAYSYSTEM_NAME		"INTERAC" // PYR-101793 PYR-110056
#define GIGADAT_GW_NAME				"GIGADAT" // PYR-101793 PYR-110056
#define NOW2PAYMOBILE_NAME          "NOW2PAYMOBILE" // PYR-105915
#define NOW2PAYWEB_NAME             "NOW2PAYWEB" // PYR-105915
#define NOW2PAY_GW_NAME             "NOW2PAY" // PYR-105915

#define CHQ_ISSUED		"QI"		
#define CHQ_CANCEL		"QN"		
//#define NT_ROLLBACK		"NR"	 //neteller rollback
									
//results of processDelta()			
#define DELTA_RESULT_INSANE		0 //uh-oh...
#define DELTA_RESULT_ROLLBACKS	1 // some rollbacks were initiated
//#define DELTA_RESULT_PAYPAL		2 // paypal manual redeem created
#define DELTA_RESULT_CHEQUE		3 // cheque has to be issued to cover delta
#define DELTA_RESULT_OVERCREDIT 4 // overcredit created
#define DELTA_RESULT_LOOPBACK	5 // no further redeem

// #define DELTA_RESULT_NETELLER	4 // neteller manual redeem created 
// #define DELTA_RESULT_FIREPAY	5 // firepay manual redeem created
// #define DELTA_RESULT_CCOIN		7 // firepay manual redeem created
// #define DELTA_RESULT_ECHECK		8 // ach final redeem created
// #define DELTA_RESULT_EPASSPORTE 9 //ePassporteFinal redeem created
// #define DELTA_RESULT_MRMC		10 //MRMC redeem created
// #define DELTA_RESULT_SOLO		11 // SOLO manual redeem created
// #define DELTA_RESULT_WIRE		12 // Wire redeem created
// #define DELTA_RESULT_N

//results of initCashRedeem2 and completeRollback
#define REDEEM_SAVED_OK			0 // no delta, case closed
#define REDEEM_SAVED_ROLLBACKS	1 // some rollbacks were initiated
//#define REDEEM_SAVED_PAYPAL		2 // paypal manual redeem created
//#define REDEEM_SAVED_CHEQUE		3 // cheque has to be issued to cover delta
#define REDEEM_SAVED_NOT_DONE	4 // not all "children" are back yet
#define REDEEM_SAVED_CHQ_STOP	5 // an overcredit transaction (cheque) was initiated
#define REDEEM_SAVED_OVERCREDIT	6 // an overcredit transaction (not cheque) was initiated
#define REDEEM_SAVED_ERROR		100 // something wrong with this trans, happens only in completeChequeCashout, may become obsolete later

#define ADMIN_GW_NAME			"ADMIN"
#define ROBOKASSA_GATEWAY_NAME  "ROBOKASSA"
#define PAYPAL_GATEWAY_NAME		"PAYPALDIRECT"  // #19197 paypal no longer use XPAY
#define GCASH_GATEWAY_NAME		"GCash"
#define GFSL_GATEWAY_NAME		"GFSL"
#define CHEQUE_GATEWAY_NAME		"Cheques"
#define SF_GATEWAY_NAME			"SF"
#define NT_GATEWAY_NAME			NETELLER_NAME
#define CANCEL_GATEWAY_NAME		"CANCEL"
#define FIREPAY_GATEWAY_NAME	FIREPAY_NAME	//only for overcredits!!!
#define CCOIN_GATEWAY_NAME		CCOIN_NAME		//only for overcredits!!!
#define SCHARGE_GATEWAY_NAME	"CHARGE"	//
#define RBS_GATEWAY_NAME		"RBS"
//#define ACH_GATEWAY_NAME		"ACH"
#define ECHECK_GATEWAY_NAME		"ECHECK"
#define GLOBALWIRE_GATEWAY_NAME	"GLOBAL_W"
#define GCDIRECT_GATEWAY_NAME   "GlobalDirect"
#define GLOBAL_GATEWAY_NAME		"GLOBAL"
#define EPASSPORTE_GATEWAY_NAME "EPASSPORTE"
#define GC_HOSTED_GATEWAY_NAME	"GC_HOSTED"
#define ATS_HOSTED_GW_NAME		"ATS" // #18938
#define PAYSAFE_HOSTED_GW_NAME	"PAYSAFE"
#define MONEYGRAM_GATEWAY_NAME	"MONEYGRAM"
#define MRMC_GATEWAY_NAME		"GFSLPT"
#define WIRE_GATEWAY_NAME		WIRE_NAME
#define MBOOKERS_GATEWAY_NAME	"MBOOK"
#define USEMYBANK_GATEWAY_NAME  "UBANK"
#define DBT_GATEWAY_NAME		"DIRBANK"
#define ECHECK_PENDING_GW_NAME	"ACH_PND"
#define CLICKANDBUY_GW_NAME		"CNBUY"
#define WEBMONEY_GW_NAME		"WEBMONEY"
#define ROUTED_CHEQUE_PREFIX	"CHK_"
#define ENVOY_GW_NAME			"ENVOY"
#define CLICK2PAY_GW_NAME		"C2PAY"
#define INTABILL_GW_NAME		"INTABILL"
#define INSTADEBIT_GATEWAY_NAME "INSTADEBIT"
#define QTENDER_GATEWAY_NAME    "QTENDER"
#define TSI_GATEWAY_NAME        "TSI"
#define DINEROMAIL_GATEWAY_NAME "DINEROMAIL"
#define UKASHVOUCHER_GATEWAY_NAME     "PSUKASHVOUCHER"
#define UKASHVOUCHER_OPENLOOP_GW_NAME "PSUKASHVOUCHER_OL"
#define UKASH_GATEWAY_NAME		"UKASH"
#define NO_GATEWAY_NAME			"NOT_SENT"
#define BOGUS_GATEWAY_NAME		"BOGUS"
#define LOBANET_GATEWAY_NAME	"LOBANET" // for PSTARSPAY
#define PAGOSONLINE_GATEWAY_NAME  "PAGOSONLINE" //#17121
#define LOTTOMATICA_GW_NAME		LOTTOMATICA_PRODUCT_NAME
#define WORLDPAYHOSTED_GW_NAME  "WORLDPAYHOSTED"
#define SOFORT_GATEWAY_NAME     "SOFORT"
#define MBOOKOBT_GATEWAY_NAME   "MBOOKOBT" // #19502
#define EPAYCARD_GATEWAY_NAME	EPAYCARD_PRODUCT_NAME
#define INSTANTBANK_GATEWAY_NAME "GLUEFINANCE"
#define WALLETONE_GATEWAY_NAME	"WALLETONE"
#define PAYZONE_GATEWAY_NAME    PAYZONE_PRODUCT_NAME
#define PAYPOINT_GATEWAY_NAME   PAYPOINT_PRODUCT_NAME
#define MBOOKERSCASH_GW_NAME    "MBOOKERSCASH"
#define DELTA_VOUCHER_GW_NAME   "DELTA"
#define VISA_PENDING_GW_NAME	"VISA_PND"
#define MASTERCARD_PENDING_GW_NAME	"MASTERCARD_PND"
#define MOBILEPAYMENT_GW_NAME   "ACCENTPAY" // #24787
#define ALVADIS_VOUCHER_GW_NAME "ALVADIS" // #24836
#define PSCASH_MANUAL_VOUCHER_GW_NAME "PSCASH_MANUAL" // #25850
#define PAGOEFECTIVO_GW_NAME    "PAGOEFECTIVO" // #26180
#define FREEMIUM_GW_NAME		"FREEMIUM"
#define PSPAY_ASTROPAY_GW_NAME  "ASTROPAYDIRECT" // #27624 PYR-41208 also for FULLTILTPAY
#define PSIOB_GATEWAY_NAME      "MAZOOMA" // PYR-28858
#define VANILLA_GATEWAY_NAME    "INCOMM" // PYR-31264
#define ADYEN_HOSTED_GW_NAME    "ADYENHOSTED" // PYR-36700
#define VIVAPAYMENTS_GW_NAME    "VIVAPAYMENTS" // PYR-37445
#define ECOPAYZ_GW_NAME         "ECOPAYZ" // PYR-48188
#define STARSWALLET_GW_NAME     "STARSWALLET" // PYR-37328
#define CASHFREE_HOSTED_GW_NAME "CASHFREE_HOSTED" // PYR-55411 deposit & rollback
#define CASHFREE_DBT_GW_NAME    "CASHFREE_DBT"    // PYR-55212 overcredit as DBT
#define VITESSE_DBT_GW_NAME     "VITESSE" // PYR-51402
#define MUCHBETTER_GW_NAME      "MUCHBETTER" // PYR-61756
#define AG_GW_NAME              "AG" // PYR-52150
#define VANILLADIRECT_GW_NAME   "VANILLADIRECT" // PYR-73312
#define GAMERSCARD_GW_NAME      "CREDTA" // PYR-60047
#define PAYTM_GW_NAME           "PAYTM" // PYR-55412
#define WORLDPAY_WPG_GW_NAME    "WORLDPAY_WPG" // PYR-36843
#define WORLDPAY_WPG_APPLEPAY_GW_NAME "WORLDPAY_WPG_A" // PYR-36843
#define FDMS_3DS_GW_NAME        "FDMS_3DS" // PYR-80351
#define FDMS_GW_NAME            "FDMS" // PYR-80351
#define BE2BILL_GW_NAME         "BE2BILL" // PYR-80352
#define XPAY_3DS_GW_NAME        "XPAY_FDMS" // PYR-78663
#define XPAY_GW_NAME            "XPAY"      // PYR-78663


#define TOURNS_TABLE_GENERATOR_NAME	"Tournaments"
#define TOURNS_TABLE_NAME			"TOURNS"
#define CASHTRANS_TABLE_NAME	"CashTrans"
#define CASHTRANSFERS_TABLE_NAME	"CASHTRANSFERS"  // PYR-25875 - this table uses CashTrans generator
#define CASHCONVERSION_TABLE_NAME	"CASHCONVERSION" // PYR-25875 - this table uses CashTrans generator
#define CASHTRANS_PM_TABLE_NAME	"CASHTRANSPM"		 // PYR-29055 - this table uses CashTrans generator
#define CASHTRANS_PRE_TABLE_NAME	"CASHTRANSPRE"	 // PYR-33629 - this table uses CashTrans generator
#define CASHTRANSAUDIT_TABLE_NAME	"CASHTRANSAUDIT" // PYR-38468
#define USERCHANGES_TABLE_NAME	"UserChanges"
#define USERBONUS_TABLE_NAME	"UserBonus"
#define BONUSSTEPAUDIT_TABLE_NAME "BonusStepAudit"
#define USERCSARDS_TABLE_NAME	"UserCards"
#define HAPPPY_HOUR_TABLE_NAME	"HappyHours"
#define ACTIVEHANDS_TABLE_NAME	"ActiveHands"
// #define AFFPAYMENTS_TABLE_NAME	"AffPayments"
#define QUALPROPS_TABLE_NAME	"QualProps"
#define PAYPROPS_TABLE_NAME		"PayProps"
#define AFFACCOUNTS_TABLE_NAME	"AffAccounts"
#define PAYMENTITEMS_TABLE_NAME	"PaymentItems"
// #define BANINSTHIST_TABLE_NAME	"BanInstHist"
// #define BANMACHIST_TABLE_NAME	"BanMacHist"
#define VIPAUDIT_TABLE_NAME		"VipAudit"
#define ACH_CBACKS_TABLENAME	"AchCBacks"
// RegActions removed PYR-39488
#define EXCLSTATES_TABLE_NAME	"ExcludedStates"
#define ADMINGROUPAUDIT_TABLE_NAME	"AdminGroupAudit"
#define GROUPRIGHTAUDIT_TABLE_NAME	"GroupRightAudit"
#define URU_RESULTS_TABLE_NAME	"UruResults"
#define GCDIRECT_TABLE_NAME		"GCDirect"
#define GCDIRECTHIST_TABLE_NAME	"GCDirectHist"
#define GCDIRECTAUDIT_TABLE_NAME	"GCDirectAudir"
#define HAPPPY_HAND_TABLE_NAME	"HappyHands"
#define SELF_EXCLUDED_TABLE_NAME	"SELFEXCLUDED"
#define MGDATA_TABLE_NAME		"MgData"
#define MRMCACCOUNTS_TABLE_NAME	"MrmcAccounts"
#define WIREACCOUNTS_TABLE_NAME	"WireAccounts"
#define TOURNTICKETS_TABLE_NAME "TournTickets"
//#define DADTRANS_TABLE_NAME		"DadTrans"
#define LIMITSAUDIT_TABLE_NAME	"LimitsAudit"
#define WUAUDIT_TABLE_NAME		"WuAudit"
#define TICKETTYPES_TABLE_NAME	"TicketTypes"
#define UNCLEARED_TABLE_NAME	"Uncleared"
#define CONVRATE_TABLE_NAME			"CONVRATE"
#define CONVRATE_AUTO_TABLE_NAME	"CONVRATEAUTO"
#define VENDORS_TABLE_NAME	"Vendors"
#define PRODUCTS_TABLE_NAME	"Products"
#define CLAIMCODES_TABLE_NAME	"ClaimCodes"
#define INIDATA_TABLE_NAME			"INIDATA"
#define PSGROUPS_TABLE_NAME		"PAYSYSTEMGROUPS"
#define DEPOSITLEVELS_NAME		"DEPOSITLEVELS"
#define USERGROUPLEVELS_NAME	"USERGROUPLEVELS"
//#define CATEGORYGROUPLEVELS_NAME "CATEGORYGROUPLEVELS"
#define PAYSYSTEMSCHANGES_TABLE_NAME "PAYSYSTEMSCHANGES"
#define TOURNTICKETSAUDIT_TABLE_NAME	"TournTicketsAudit"
#define BANCASES_TABLE_NAME	"BanCases"
#define PAYMENTSAUDIT_TABLE "PAYMENTSAUDIT"
#define USERBANAUDIT_TABLE_NAME	"UserBanAudit"
#define CHQAUDIT_TABLE_NAME	"CHQAUDIT"
#define OBJECTBANAUDIT_TABLE_NAME "ObjectBanAudit"
#define TINFO_TABLE_NAME "TInfo"
#define FROZENOBJECTS_TABLE_NAME "FrozenObjects"
#define FREEZEEVENTS_TABLE_NAME "FreezeEvents"
#define AGEVERIFYRESULTS_TABLE_NAME	"AgeVerifyResults"
#define LIVESUPPORTREQUESTS_TABLE_NAME	"LiveSupportRequests"
#define IPTRANSFERREQUESTS_TABLE_NAME	"IPTransferRequests"
#define TICKETTYPECHANGES_TABLE_NAME	"TicketTypeChanges"
//#define SECURITYALERTS_TABLE_NAME       "SecAlerts"
//#define SECURITYALERTSAUDIT_TABLE_NAME  "SecAlertsAudit"
//#define CBMDCONTROL_TABLE_NAME          "CBMDControl"
#define TLBPRIZESL_TABLE_NAME			"TLBPrizes"
//#define BOPEXCLUDED_TABLE_NAME			"BOPExcluded" // obsolete PYR-120713
#define FLYERS_AUDIT_TABLE_NAME			"FlyersAudit"
#define AGE192RESULTS_TABLE_NAME		"AGE192RESULTS"
#define AGE192DETAILS_TABLE_NAME		"AGE192DETAILS"
#define TABLELIMITS_TABLE_NAME			"TableLimits"
#define CASHOUTBATCHES_TABLE_NAME       "CASHOUTBATCHES"
#define MIGRATEDUSERS_TABLE_NAME		"MIGRATEDUSERS"
#define TEXTS_TABLE_NAME		"TEXTS"
//#define BOP_MAP_TABLE_NAME		"BOPMAP" // obsolete PYR-120713
#define FAILEDACCOUNTS_TABLE_NAME		"FailedAccounts"
#define UIDRETRIEVALS_TABLE_NAME	    "UIDRetrievals"
#define BONUSESAUDIT_TABLE_NAME			"BONUSES2AUDIT"
#define BONUSCURRENCIESAUDIT_TABLE_NAME	"BONUSCURRENCIESAUDIT"
#define BONUSALLCODESAUDIT_TABLE_NAME	"BONUSALLCODESAUDIT"
#define BONUSTICKETTYPESAUDIT_TABLE_NAME "BONUSTICKETTYPESAUDIT" // PYR-23376
#define DEADBALANCE_TABLE_NAME  	    "DEADBALANCE"
#define CASHERRORSAUDIT_TABLE_NAME		"CASHERRORSAUDIT"
#define USER_COUNTERS_AUDIT_TABLE_NAME	"USERCOUNTERSAUDIT"
#define DUMMYTOURNSAUDIT_NAME			"DUMMYTOURNSAUDIT"		// #18129
#define FLIGHTTOURNEXTRA_NAME           "FLIGHTTOURNEXTRA"		// #19167
#define FLIGHTTOURNMONEY_NAME			"FLIGHTTOURNMONEY"		// #19167
#define NEWCLIENTINSTALL_NAME           "NEWCLIENTINSTALL"  // PYR-17950
#define FIRSTINSTALLLOGIN_NAME          "FIRSTINSTALLLOGIN" // PYR-17950
#define ACCOUNTCREATE_NAME              "ACCOUNTCREATE"     // PYR-17950
#define LOGINEVENTSAUDIT_NAME			"LOGINEVENTSAUDIT"	// PYR-20301
#define TGMS_TABLE_NAME					"TOURNGROUPMANAGERS"	// #22691
#define TGMTOURNS_TABLE_NAME			"TGMTOURNS"				// #22691
#define TGMUSERS_TABLE_NAME				"TGMUSERS"				// #28135
#define SAGTOURNS_TABLE_NAME			"SAGTOURNS"				// #28752
#define SAGTOURNEXTRA_TABLE_NAME		"SAGTOURNEXTRA"			// #31423
#define TOURNUSERS_TABLE_NAME			"TOURNUSERS"
#define USERINSTALLIDAUDIT_NAME			"USERINSTALLIDAUDIT"	// #21829
#define TOURNHAPPYHOURSAUDIT_TABLE_NAME	"TOURNHAPPYHOURSAUDIT"	//PYR-24396
#define ZOOMCHATSEQNUM_NAME				"ZOOMCHATSEQNUM"		// PYR-24004

// The following four are obsoleted (Golden SNG) and left for reference
// #define GOLDENSNG_TABLE_NAME			"GOLDENSNG"	// PYR-24382
// #define GOLDENSNGAUDIT_TABLE_NAME		"GOLDENSNGAUDIT" // PYR-24382
// #define MILESTONEGOLDENSNG_TABLE_NAME	"MILESTONEGOLDENSNG"			//PYR-28690
// #define MILESTONEGOLDENSNGAUDIT_TABLE_NAME	"MILESTONEGOLDENSNGAUDIT"	//PYR-28690

#define SATELLITEWINNERS_TABLE_NAME		"SATELLITEWINNERS" // PYR-26990
#define IMAGE_APPROVALS_AUDIT_TABLE_NAME "IMAGEAPPROVALSAUDIT" // PYR-26568
#define TOURNUSERSEXTRA_TABLE_NAME		"TOURNUSERSEXTRA"
#define TOURNUSERPROPS_TABLE_NAME		"TOURNUSERPROPS"
#define BLITZTOURNTABLES_TABLE_NAME		"BLITZTOURNTABLES"
#define CANCELLED_TGMTOURNS_TABLE_NAME	"CANCELLEDTGMTOURNS"	// #26094
#define SAGPAYOUTS_TABLE_NAME			"SAGPAYOUTS"
#define SAGPAYOUTLEVELS_TABLE_NAME		"SAGPAYOUTLEVELS"
#define SAGPAYOUT_TICKETS_TABLE_NAME	"SAGPAYOUTTICKETS"
#define OLYMPICSNAPSHOTS_TABLE_NAME		"OLYMPICSNAPSHOTS" // PYR-29846 - moved table name to define
#define ICE_EVENTID_GENERATOR			"IceEventId" // PYR-30164
#define ACM_CHATVIOLATIONS_TABLE_NAME	"ACMCHATVIOLATIONS"  // PYR-28673
#define ACM_CHATVIOLATION_SEQNUM_NAME   "AcmViolationSeqNum" // PYR-28673
#define USERROLLSAUDIT_TABLE_NAME		"USERROLLSAUDIT"
#define TGMPROPS_TABLE_NAME				"TGMPROPS" //PYR-33444
#define FLIGHTUSERMOVEDATA_TABLE_NAME	"FLIGHTUSERMOVEDATA"
#define FLIGHTFORFEITSTACK_TABLE_NAME	"FLIGHTFORFEITSTACK" //PYR-103089
#define ACTIVETABLES_TABLE_NAME			"ActiveTables" // PYR-35000 - !!! DON'T change to uppercase as it is also used as a generator name for tableId generation
#define ESBOTREPORTDAILY_TABLE_NAME     "EsBotReportDaily" // !!! DON'T change to uppercase as it is also used as a generator name for tableId generation
#define FRSHIPSCOREAUDIT_TABLE_NAME		"FRSHIPSCOREAUDIT" // PYR-35600
#define SELFEXCLUDEDAUDIT_TABLE_NAME	"SELFEXCLUDEDAUDIT" // PYR-40105
// PYR-37300 - mini games (The Deal)
#define MG_PLAYS_TABLE_NAME				"MG_PLAYS"
#define MG_PLAYS_SIDEBET_TABLE_NAME		"MG_PLAYS_SIDEBET"
#define MG_ACCOUNTS_TABLE_NAME			"MG_ACCOUNTS"
#define MG_GAMES_TABLE_NAME				"MG_GAMES"
#define MG_SCHEMA_TABLE_NAME			"MG_SCHEMA"
#define MG_JPSHAREPAYOUTS_TABLE_NAME	"MG_JPSHAREPAYOUTS"
#define MG_ACCOUNTACTIONS_TABLE_NAME	"MG_ACCOUNTACTIONS"
#define MG_GAMESAUDIT_TABLE_NAME		"MG_GAMESAUDIT"
#define MG_SCHEMAAUDIT_TABLE_NAME		"MG_SCHEMAAUDIT"
// end of PYR-37300

// PYR-109327
#define PPP_ENTRIES_TABLE_NAME			"PPPENTRIES"
#define PPP_SCRIPT_AUDIT_TABLE_NAME		"PPPSCRIPTAUDIT"
#define PPP_EVENT_TABLE_NAME			"PPPEVENTS"
// end of PYR-109327
#define PPP_ENTRIES_AUDIT_TABLE_NAME	"PPPENTRIESAUDIT" // PYR-122775


#define FTBLOCKEDAUDIT_TABLE_NAME		"FTBLOCKEDAUDIT" // PYR-41600
#define FTCONVERSIONREJECTS_TABLE_NAME	"FTCONVERSIONREJECTS" // PYR-42449
#define FTPASSWORDS_TABLE_NAME			"FT_PWDS"			//	PYR-41017

#define PWD_RESET_AUDIT_TABLE_NAME			"PWDRESETAUDIT"		//	PYR-41017
#define PWD_RESET_AUDIT_ACCOUNT_TABLE_NAME	"PWDRESETAUDITACCOUNTS"	//	PYR-41017

#define CROSSLINKBUTTONS_TABLE_NAME		"CROSSLINKBUTTONS" // PYR-42114

#define TOURNBLINTERVALRULE_TABLE_NAME		"TOURNBLINTERVALRULE" // PYR-32616
#define TOURNBLINTERVALTIMING_TABLE_NAME	"TOURNBLINTERVALTIMING" // PYR-32616

#define NOTCASHINSAUDIT_TABLE_NAME		"NOTCASHINSAUDIT" // PYR-37639
#define GAMETICKETSAUDIT_TABLE_NAME		"GAMETICKETSAUDIT"

#define SCHED_BROADCAST_MSG_TABLE_NAME		"SCHED_BROADCAST_MSGS" // PYR-44930

#define MG_USABLEGAMETICKETS_TABLE_NAME			"MG_USABLEGAMETICKETS" // PYR-46075
#define MG_USABLEGAMETICKETSAUDIT_TABLE_NAME	"MG_USABLEGAMETICKETSAUDIT" // PYR-46075

#define PLAYMONEYADEVENTS_TABLE_NAME	"PLAYMONEYADEVENTS" // PYR-45097

#define RELATEDUSERAUDIT_TABLE_NAME "RELATEDUSERSAUDIT" //PYR-41075
#define VERIFIEDCARDSAUDIT_TABLE_NAME "VERIFIEDCARDSAUDIT" // PYR-48809
#define ACCTCREATIONFAILURES_TABLE_NAME	"ACCOUNTCREATIONFAILURES" // PYR-45046

#define MG_PRIZESPAID_TABLE_NAME	"MG_PRIZESPAID" // PYR-45550
#define MG_PRIZESPAIDFAILURES_TABLE_NAME	"MG_PRIZESPAID_FAILURES" // PYR-45550
#define MG_CATALOG_PRIZES_TABLE_NAME "MG_CATALOG_PRIZES" // PYR-45550
#define MG_CATALOG_PRIZE_CATEGORY_TABLE_NAME	"MG_CATALOG_PRIZE_CATEGORY" // PYR-45550
#define MG_PRIZE_CATEGORIES_TABLE_NAME "MG_PRIZE_CATEGORIES" // PYR-51427
#define MG_PRIZE_LIMITS_TABLE_NAME "MG_PRIZE_LIMITS" // PYR-53982
#define MG_PRIZE_LIMITS_USAGE_TABLE_NAME "MG_PRIZE_LIMITS_USAGE" // PYR-53982

#define MG_STARSREWARDS_EVDATA_TABLE_NAME "MG_STARSREWARDS_EV" // PYR-53503
#define MG_SIDEBETS_PAYOUT_ODDS_DATA_TABLE_NAME "MG_SIDEBETS_PAYOUT_ODDS" // PYR-118135

#define MG_PROPS_TABLE_NAME "MG_PROPS" //PYR-120470
#define MG_CATALOG_PRIZE_PROPERTIES_TABLE_NAME "MG_CATALOG_PRIZE_PROPERTIES" //PYR-62676
#define MG_FIXED_DRAW_PRIZESPAID_TABLE_NAME "MG_FIXED_DRAW_PRIZESPAID" //PYR-88293

#define TOURN_DEAL_AVAILABILITY_TABLE_NAME "TOURNDEALAVAILABILITY" // PYR-49826
#define TOURN_DEAL_ATTEMPTS_TABLE_NAME "TOURNDEALATTEMPTS" // PYR-49826

#define PAYWALL_TABLE_NAME				"PAYWALL" // PYR-46043
#define PAYWALL_SEGMENT_TABLE_NAME		"PAYWALLSEGMENT" // PYR-46043
#define PAYWALL_BUNDLE_TABLE_NAME		"PAYWALLBUNDLE" // PYR-46043
#define JOBSCTRLLOG_TABLE_NAME       	"JOBSCTRLLOG" // PYR-50202
#define JOBSCTRLRESULTS_TABLE_NAME "JOBSCTRLRESULTS" // PYR-50202
#define USERPAYMENTS_CONSENT_TABLE_NAME "USERPAYMENTSCONSENTS" // PYR-70835

#define TOURN_INTERNAL_ACCOUNTS_TABLE_NAME "TOURNINTERNALACCOUNTS" // PYR-64730
#define REFCONVRATES_TABLE_NAME			   "REFCONVRATES" // PYR-69310
#define REFCONVRATESAUDIT_TABLE_NAME	   "REFCONVRATESAUDIT" // PYR-69310

#define BLITZ_INTERNAL_ACCOUNTS_TABLE_NAME "BLITZINTERNALACCOUNTS" //PYR-64735

#define USERAWARDS_TABLE_NAME "USERAWARDS" // PYR-81423
#define USERROLLS_TABLE_NAME "USERROLLS"
#define CHARGEBACKS_TABLE_NAME "ChargeBacks"
#define ADMINCHAT_TABLE_NAME "AdminChat"
#define ACCOUNTACCESS_TABLE_NAME "AccountAccess"
#define USERBONUSAUDIT_TABLE_NAME "UserBonusAudit"
#define USERPENDINGACTIONS_TABLE_NAME "USERPENDINGACTIONS"
#define WIREORDERS_TABLE_NAME "WireOrders"
#define FRBANKACCOUNTSAUDIT_TABLE_NAME "FRBANKACCOUNTSAUDIT"
#define VALIDATEFAILS_TABLE_NAME "VALIDATEFAILS"
#define CARDINFO_TABLE_NAME "CardInfo"
#define ESUSERSIDANDBANCHECK_TABLE_NAME "EsUsersIdAndBanCheck"
#define SECUREVENTS_TABLE_NAME "SecurEvents"
#define SPECIALINSTRUCTIONSAUDIT_TABLE_NAME "SpecialInstructionsAudit"
#define SPPAUDIT_TABLE_NAME "SPPAUDIT"
#define USERPROPAUDIT_TABLE_NAME "UserPropAudit"
#define XCPPAUDIT_TABLE_NAME "XCPPAUDIT"
#define USERCOMMENTS_TABLE_NAME "UserComments"
#define STOREORDERS_TABLE_NAME "StoreOrders"

#define TGM_FLIGHT_USER_TABLE_NAME "TGMFLIGHTUSERS" //PYR-99198
#define THROWABLESAUDIT_TABLE_NAME "THROWABLESAUDIT" // PYR-100755

#define ALIASBLOCKSAUDIT_TABLE_NAME	   "ALIASBLOCKSAUDIT" // PYR-106112
#define OSDUNIQUEMSGID_NAME "OSDUNIQUEMSGID" // PYR-120129
#define PAM_USERS_TABLE_NAME "PAM_USERS" // PYR-121468

// NB!!! don't forget to add each new generator to generators list as shared or local !!!

#define UNCLEARED_STATUS_UNCLEARED	0
#define UNCLEARED_STATUS_CLEARED	1
#define UNCLEARED_STATUS_RETURN		2
#define UNCLEARED_STATUS_ADMIN		3

///////////////////// Constants for DBM_Q_IPN //////////////////////////////////
//status, 
#define IPN_TXN_STATUS_COMPLETED	'C'	//"Completed"
#define IPN_TXN_STATUS_PENDING		'P'	//"Pending"
#define IPN_TXN_STATUS_FAILED		'F'	//"Failed"
#define IPN_TXN_STATUS_DENIED		'D'	//"Denied"
//pending_reason, 
#define IPN_PENDING_ECHECK		'E'	//"echeck"	eCheck, which has not yet cleared.
#define IPN_PENDING_INTL		'I'	//"intl" you, the merchant, hold an international account and do not have a withdraw mechanism.
#define IPN_PENDING_VERIFY		'V'	//"verify"	you, the merchant, are not yet verified.
#define IPN_PENDING_ADDRESS		'A'	//"address"	your customer did not include a confirmed shipping address
#define IPN_PENDING_UPGRADE		'U'	//"upgrade"	made via credit card and the merchant must upgrade account to Business or Premier
#define IPN_PENDING_UNILATERAL	'L'	//"unilateral"	payment to an email address that is not yet registered or confirmed.
#define IPN_PENDING_OTHER		'O'	//"other"
//txn_type
#define IPN_TXN_TYPE_WEBACCEPT	'W'	//"web_accept"
#define IPN_TXN_TYPE_CART		'C'	//"cart"
#define IPN_TXN_TYPE_SENDMONEY	'S'	//"send_money"
//payment_type
#define IPN_PAY_TYPE_ECHECK		'E'	//"echeck"
#define IPN_PAY_TYPE_INSTANT	'I'	//"instant"

/// admin rights. make sure they are not longer than 20 chars
								//  |01234567890123456789|
//#define RIGHT_CHANGE_PRIVILEGES	"change_priv"
#define RIGHT_CHANGE_ADMIN_PRIV		"change_admpriv"
#define RIGHT_CHANGE_VIP_PRIV		"change_vippriv"
#define	HANDLE_MONEY_STR			"HandleMoney"
#define	SYSADMIN_STR				"SysAdmin"
#define	SECURITY_STR				"Security"
#define	SUPERVISOR_STR				"Supervisor"
#define	CASHIER_STR					"Cashier"
#define MASS_EMAIL_STR				"MassEmail"
#define UPLOAD_EMAIL_CODE_FILE		"UploadMailCode"
#define BROADCAST_STR				"Broadcast"
#define LIST_EMAIL_STR				"ListEmail"
#define CHANGE_HIGHROLL_PRIV		"ChangeHighRoller"
#define CREATE_AFFILIATE_ACCOUNT	"CreateAffAccount"
#define MAKE_AFFILIATE_PAYMENT		"MakeAffPayment"	
#define STOP_START_TOURN			"StopStartTourn"
#define TOURN_OBSERVER_CHAT_SWITCH	"TrnObsrvChatSwitch"
#define CANCEL_RUNNING_TOURN		"CancelRunningTourn"
#define CHANGE_REQ_TRN_PRIV	        "ChangeReqTournPriv"
#define UPLOAD_CASHIER_SCRIPT_PRIV  "UploadCashierScript"
#define UPLOAD_NEWS_PRIV            "UploadNews"
//#define DESTROY_CANCELLED_PRIV	"DestroyCancelledTrn"
#define CANCEL_TOURN_PRIV			"CancelTournPriv"
#define DESTROY_TRN_RANGE_PRIV		"DestroyTournRange"
#define SWITCH_IMAGES_PUBLICATION   "SwitchImagesPubl"
#define FORCE_DBM_LOGOUT_STR		"ForceDbmUserLogout"
#define REVOKE_USER_CHAT_STR		"RevokeUserChat"
#define SEE_PORTALS_STR				"see_portals" //without this right admin will not see main admin tools & reports page
#define CHANGE_PWD_STR				"ChangeUserPwd"
#define UPLOAD_TOURN_SCRIPT_PRIV	"UploadTournScript"
#define CANCEL_PRIVATE_TOURN_STR	"CancelPrivateTourn"
#define	TOURN_DEAL_PRIV				"TournDealPriv"
#define AFFILIATE_SCRIPT_PRIV		"AffiliateScript"
#define CREDIT_USERS_LIST_PRIV		"CreditUsersList"
#define HANDLE_FRONTIMAGES_PRIV		"HandleFrontImages"
#define TOURN_MASS_REG_UNREG		"TournMassRegUnreg"
#define CLOSE_ADMIN_ACCOUNT			"CloseAdminAccount"
#define SET_USER_SECUREID			"SetUserSecureId"
#define HANDLE_BAN_INSTALLID		"HandleBanInstallId"
#define EXPORT_LIABILITY			"ExportLiability"
#define HANDLE_DAD					"HandleDAD"
#define APPROVE_USERS_PRIV			"ApproveUsers"
#define HANDLE_CASHIN_LIMITS		"HandleCashinLimits"
#define CREATE_TOURN_TICKETS		"CreateTournTickets"
#define HANDLE_ADMIN_MONEY_LIMITS	"HandleAdmMoneyLimit"
#define KICK_USER_FROM_TABLE		"KickUserFromTable"
#define HANDLE_CARD_INFO			"HandleCardInfo"
#define HANDLE_CASHIER_SETTINGS		"HandleCashierSetting"
#define HANDLE_ACH_CBACKS			"HandleAchCBacks"
#define REREAD_FPP_ITEMS			"RereadFppItems"
#define CREATE_EDIT_STORE_ITEMS		"CreateEditStoreItems"	// [AO 2010-04-09] #13676
#define UPDATE_ORDER_STATUS			"UpdateOrderStatus"		// [AO 2010-04-09] #13676
#define TRACK_CODES					"TrackCodes"
#define TRACK_CODES_SPECIAL			"TrackCodesSpecial"
#define AUTO_ACTIVE_TRACK_CODES		"AutoActiveTrackCode"	// PYR-30948
#define CHANGE_NO_AUTOBAN			"ChangeNoAutoBan"
#define CHANGE_TVP_PRIV				"ChangeTvpPriv"
#define HANDLE_BONUS_LIST			"HandleBonusList"
#define ADD_SUPER_RUG				"AddSuperRUG"
#define SWITCH_CHAT_MODER_CALL		"SwitchChatModerCall"
// RegActions PYR-39488 removed
#define SET_AGE_VERIFY_FLAG			"SetAgeVerifyFlag"
#define HANDLE_GCDEBIT				"HandleGCDebit"
#define HANDLE_CFT_SPLIT			"HandleCftSplit"
#define ASSIGN_FPP_STORE_BONUS		"AssignFppStoreBonus"
#define	SET_BLOCKED_STATES			"SetBlockedStates"
#define	SET_EXCLUDED_STATES			"SetExcludedStates"
#define	SET_SHOWDEPOSIT_ONLY		"SetShowDepositOnly"
// #define SET_WUOFFICE_PERCENTAGES	"SetWUOfficePercents" // obsolete by PYR-45735
#define RELEASE_USER_BONUS			"ReleaseUserBonus"
#define DOUBLE_USER					"DoubleUser"
#define RESET_USER_FPP				"ResetUserFPP"
#define CHANGE_TEAM_PS_LIST			"ChangeTeamPSList"
#define HANDLE_SELF_EXCLUSION		"HandleSelfExclusion"
#define HANDLE_MONEYGRAM			"HandleMoneygram"
#define HANDLE_STUCK_TRANS			"HandleStuckTrans"
#define UPDATE_STUCK_TRANS			"UpdateStuckTrans"
#define CHANGE_DAD_SETTINGS			"ChangeDadSettings"
#define CHANGE_MATCHCODE_PASS		"ChangeMatchCodePass"
#define HANDLE_MRMC					"HandleMRMC"
#define SET_VIP_STATUS				"SetVipStatus"
#define HANDLE_WIRE_ACCOUNT			"HandleWireAccount"
// #define HANDLE_WU					"HandleWU" // obsolete by PYR-45735
#define HANDLE_TICKETTYPE_LIST		"HandleTicketTypeList"
#define HANDLE_CONV_RATES			"HandleConRates"
#define HANDLE_TABLE_OBSERVER_CHAT	"HandleTblObsevChat"
#define HANDLE_DEPOSIT_LIMIT		"HandleDepositLimit"
#define HANDLE_VENDOR_LIST			"HandleVendorList"
#define HANDLE_PRODUCT_LIST			"HandleProductList"
#define HANDLE_CLAIM_CODES			"HandleClaimCodes"
#define HANDLE_INIDATA				"HandleIniData"
#define INSERT_PAYSYSTEM			"InsertPaysystem"
#define UPDATE_PAYSYSTEM			"UpdatePaysystem"
#define DELETE_PAYSYSTEM			"DeletePaysystem"
#define ROUTE_ACH_OVERCREDITS		"RouteAchOvercredits"
#define VIEW_ECHECK_ROUTING			"ViewEcheckRouting"
#define CHANGE_ECHECK_ROUTING		"ChangeEcheckRouting"
#define OVERRIDE_ECHECK_ROUTING		"OverideEcheckRouting"
#define REPRESENTMENTS_STR			"Representments"
#define MANAGE_ADMIN_RIGHT_STR		"ManageAdminRights"
#define CHANGE_REPRESENTMENT		"ChangeRepresentment"
#define CLEAR_UNCLEARED				"ClearUncleared"
#define HANDLE_LEVELPERMS           "HandleLevelPerms"
#define CHANGE_AVLS_SETTINGS		"ChangeAVLSSettings"
#define UPDATE_TICKET_VALUE			"UpdateTicketValue"
#define VIEW_HIDDEN_ACCOUNT         "ViewHiddenAccount"
#define CREDIT_BONUS_LIST           "CreditBonusList"
#define TRACKCODE_EMAIL_STR			"TrackcodeEmail"
// #define HANDLE_CITADEL				"HandleCitadel"
#define COMMENT_MASS_ACCOUNT        "CommentMassAccount"
#define CLOSE_MASS_ACCOUNT          "CloseMassAccount"
// #define HANDLE_CBMD_SETTINGS        "HandleCBMDSettings"
#define HANDLE_POKERSTRATEGY		"HandlePokerStrategy"
#define HANDLE_EXPIRED_FPP			"HandleExpiredFpp"
#define HANDLE_EXPIRED_SELF_EXCL	"HandleExpSelfExcl"
#define VIEW_CASHIER_INI			"ViewCashierIni"
#define UPDATE_CASHIER_INI			"UpdateCashierIni"
#define UPDATE_CASHIER_INI_X		"UpdateCashierIniX"
#define ISSUE_LEAGUE_PRIZES			"IssueLeaguePrizes"
#define CHANGE_NOT_QUALIFIED		"ChangeNotQualified"
#define HANDLE_SELFIMPOSED_LIMIT	"HandleSelfLimit"
#define UPDATE_COLLUSION_INI		"UpdateCollusionIni"
#define UPDATE_OLAPMD_INI 			"UpdateOlapMDIni"
#define CANCEL_STORE_ORDER 			"CancelStoreOrder"
#define MANAGE_ADMIN_DOCS 			"ManageAdminDocs"
#define HANDLE_FLYER	 			"HandleFlyer"
#define TS_MANAGE_TEMPLATE	 		"ManageTemplate"
#define TS_MANAGE_TEMPLATE_CONTENT	"ManageTemplateCont"
#define TS_MANAGE_LARGE_TMPL_CONTENT "ManageLargeTmplCont"
#define TS_MANAGE_TEMPLATE_SERVER	"ManageTemplateServer"
#define TS_EDITCOPY_TEMPLATE	 	"EditCopyTemplate"
#define REQUEST_USER_CASHOUT		"RequestUserCashout"
#define CARD_DEALING_STOP_RESUME	"CardDealingStopRes"
#define SNG_REGEN_STOP_RESUME		"SnGRegenStopRes"
#define TOURNAMENT_ADMIN			"TournamentAdmin"
#define HANDLE_MASS_DISCONN			"HandleMassDisconnect"
#define SAVE_WIRE_INFO				"SaveWireInfo"
#define HANDLE_VELOCITY_CONTROL		"VelocityControl"
#define CLOSE_TOURN_REGISTRATION	"CloseTournReg"
#define BLOCK_ROLLBACKS				"BlockRollbacks"
#define READ_CDMS_DOCS 				"ReadCDMSDocs"
#define MANAGE_CDMS_DOCS			"ManageCDMSDocs"
#define MULTIDAY_TOURN_ADMIN		"MultiDayTournAdmin"
#define MANAGE_RSA 					"ManageRSA"
#define MANAGE_AAMS_GATEWAY			"ManageAamsGw"
#define MANAGE_USER_LICENSE_DOCS	"ManageLicenseDocs"
#define HANDLE_TSS_SETTINGS         "HandleTSSSettings"
#define MANAGE_GEO_IP_SETTINGS		"ManageGeoIp"
#define IMPORT_RETURN_FILE          "ImportReturn"
#define DELETE_ECHECK_SSN           "DeleteECheckSsn"
#define REREAD_PAYSYS_MSG_TEMPLATES "RereadPaySysMsg"
#define HANDLE_ENV_LBT				"HandleEnvLbt"
#define MANAGE_ATAS_RULES           "ManageATASRules"
#define MANAGE_PS_RELATED           "ManagePSRelated"
#define MANAGE_SECURITY				"ManageSecurity"
#define UPLOAD_RSA 					"UploadRSA"
#define GENERATE_OTP				"GenerateOTP"
#define RIGHT_SET_USER_ADMINFLAGS   "UpdateUserAdminFlags"
#define HANDLE_BALANCE_ACCOUNT		"HandleBalanceAccount"
#define UPDATE_CONVERSION_MARGIN	"UpdateConvMargin"
#define VIEW_ATAS_INI			    "ViewAtasIni"
#define UPDATE_ATAS_INI			    "UpdateAtasIni"
#define HANDLE_MIGRATION			"HandleMigration"
#define PMTS_SEC_HANDLE_TRANS		"PmtsSecHandleTrans"
#define PMTS_SEC_BLOCK_CARDS		"PmtsSecBlockCards"
#define PMTS_SEC_HANDLE_CBACKS		"PmtsSecHandleCBacks"
#define PMTS_SEC_HANDLE_BAN			"PmtsSecHandleBan"
#define HANDLE_USER_CACHE			"HandleUserCache"
#define	MANAGE_ADMIN_MONEY_LIMITS	"MngAdminMoneyLimits"
#define VIEW_RUG_SETTINGS           "ViewRUGSettings"
#define HANDLE_RUG_EXCLUDED         "HandleRUGExluded"
#define HANDLE_RUG_SETTINGS         "HandleRUGSettings"
#define HANDLE_KYC_SCORE            "handleKYCscore"
#define UPDATE_DESCRIPTORS			"UpdateDescriptors"
#define HANDLE_WIRECARD				"HandleWirecard"
#define HANDLE_CASHERR_GROUPS		"HandleCashErrGroups"
#define HANDLE_MBOOKS_ACCOUNT		"HandleMBooksAccount"
#define HANDLE_NETELLER_ACCOUNT		"HandleNetellAccount"
#define SET_SPECIAL_INSTRUCTIONS	"SetSpecialInstructs"
#define SET_SPECIAL_INSTRUCTIONSVIP	"SpecialInstructsVIP" // PYR-18101
#define SIGNUP_QUESTIONNAIRE		"SignupQuestionnaire"
#define MANAGE_STELLAR_REWARDS		"ManageStellar"
#define VIRTUAL_ORDER_TERMINAL		"VirtualOrderTerm"   // [AN 2009-12-22] PYR-7801
#define VIRTUAL_ORDER_RESERVED		"VirtualOrderReserv" // [AN 2009-12-22] PYR-7801
#define VIP_EDIT_CUSTOM_ROUTING     "EditCustomRouting"  // [AN 2010-04-12] PYR-13678
#define VIP_EDIT_DEFAULT_ROUTING    "EditDefaultRouting" // [AN 2010-04-12] PYR-13678
#define VIP_EDIT_STOCK_ITEM         "EditStockItem"      // [AN 2010-04-12] PYR-13678
#define VIEW_CASHOUT_AUDIT			"ViewCashoutAudit"   // #12888 used by admin only
#define REREAD_INIDATA              "RereadIniData"
#define MANAGE_HEADS_UP_EXCLUSION	"ManageHUExcl" // #14108
#define HANDLE_FRBANKACCOUNT        "HandleFrBankAccount"
#define HANDLE_FRAVC_MAILED_STATUS  "HandleFrAVCMailed"
#define CREATE_FORM_ATTACHMENT      "CreateFormAttachment"
#define MANAGE_CDMS_SELFIMPORT      "ManageCDMSSelfImport"
#define MASS_EMAIL_VIA_CODE_STR		"MassEmailViaCode"
#define LIVE_EVENT_CREDIT_DEBIT		"LiveEventCreditDebit" // [AO 2010-08-11] #15728
#define ASSESS_CHIP_DUMPS       	"AssessChipDumps" //#16033
#define HANDLE_CDA_SETTINGS         "HandleCDASettings" //#16033
#define CONSOLIDATE_USER_ASSETS		"ConsolidatUserAssets"
#define BRUTE_TOURN_REGISTRATION	"BruteTournReg" // #16448
#define SET_RETAIN_DATA_PROP		"SetRetainDataProp" // PYR-15466: Blank Out Personal Information for Inactive Players
#define MANAGE_IP_BLOCKING_BYPASS	"ManageIpBlockBypass" // #16614: bypass IP based blocking
#define UNREGISTER_FROM_DUMMY		"UnregFromDummy" // #16437
#define MANAGE_USER_PROPS			"ManageUserProps" // #16530
#define MANAGE_TRANSFER_AFFILIATES	"ManageTransferAff" // #16530
#define MANAGE_BETA_TESTERS			"ManageBetaTesters" // #16940: Global Beta Tester Privilege
#define MANAGE_HOME_GAMES			"ManageHomeGames" // #16182
#define HG_MANAGE_GLOBAL_SETTINGS	"HgMngGlobalSettgs"		// #PYR-17604
#define HG_MANAGE_GLOBAL_STAKES		"HgMngGlobalStakes"		// #PYR-17604
#define HG_MANAGE_RESTRICTED_CNTRS	"HgMngRestrictedCntrs"	// #PYR-17604
#define HG_MANAGE_CENSORSHIP_TOOL	"HgMngCensorshipTool"	// #PYR-17604
#define HG_MANAGE_PLAYER_ACCOUNT	"HgMngPlayerAccount"	// #PYR-17604
#define HG_MANAGE_SHADOW_FOUNDER	"HgMngShadowFounder"	// #PYR-17604
#define MANAGE_LANG_PATTERNS		"ManageLangPatters" // #16914
#define BLOCK_PLAY_LIMIT_INCREASE   "BlockPlayLimitIncr" // #15894+17417: change user's "no table/tournament/deposit limit increase" privileges
#define MANAGE_HG_SHOWCASE_CLUBS	"HgShowcaseClubs"    // #16627
#define ARJEL_FILE_UPLOAD			"ArjelFileUpload" // PYR-17261
#define ARJEL_FILE_VIEW				"ArjelFileView" // PYR-17261
#define MANAGE_SHOWDOWN				"ManageShowdown" // #17840
#define MANAGE_JUBILEE_HANDS        "ManageJubileeHands"	// PYR-13934
#define LOGSERVERUSER_STR           "LogServerUser" //#23995
#define LOGSERVERINIUSER_STR        "LogServerIniUser" //#42687
#define PLAYCHIPS_ADMIN_PRIV        "PlayChipsAdminPriv" // #26426
#define RECEIVE_MSG_IN_ALL_LOCALES	"RcvMsgInAllLocales"	// PYR-27201
#define MANAGE_NO_LIVE_ASSIST_PRIV	"MngNoLiveAssistance"	// PYR-29934
#define SAVE_ECHECK_INFO			"SaveEcheckInfo" // PYR-27568
#define CAN_MIGRATE_TO_COM          "CanMigrateToCom" //PYR-34786
#define MANAGE_BG_GAMING_NUMBER     "ManageBgGamingNumber" //PYR-46521
#define MANAGE_W2G_REPORT			"ManageW2GReport" // PYR-48498
#define MANAGE_TWITCH_GATEWAY		"ManageTwitchGw"

// PYR-17349 - all country-specific rights for mass email have this format:
// "CountryPriv_<two-letter country code>"
#define COUNTRY_PRIV_STR_PREFIX     "CountryPriv_"
#define COUNTRY_PRIV_ALL_SUFFIX     "All" // PYR-18373 - Special 'country' suffix meaning that all countries are allowed
#define LICENSE_PRIV_STR_PREFIX		"LicensePriv_" // PYR-20297
#define CHANGE_MASS_EMAIL_FLAGS     "ChangeMassEmailFlags"  // #17349 - change "Send anyway if" and "Exclude" options for mass emails
#define UPDATE_SENT_OVERCREDIT		"UpdateSentOvercredit"	// #17924
#define MANAGE_RESTART_TIME			"SetRestartTime"		// PYR-17774
#define HANDLE_SECURITY_ALERTS      "HandleSecurityAlerts"
#define HANDLE_INV_ALERTS           "HandleINVAlerts"
#define HANDLE_RUG_ALERTS           "HandleRUGAlerts"
#define HANDLE_TRA_ALERTS           "HandleTRAAlerts"
#define HANDLE_LOGIN_ALERTS         "HandleLoginAlerts" // #20301
#define SEND_SMS_PRIV               "Allow_Sending_SMS" // PYR-14242
#define HANDLE_FAST_DEPOSIT_PRIV	"HandleFastDeposit" // #18174
#define START_STOP_RG_REPLAYS       "StartStopRGReplay"	// #18024
#define AAMS_PLAYER_INFO_UPLOAD		"AamsPlayerInfoUpload" // #16696
#define AAMS_PLAYER_BALANCE_REPORT	"AamsBalanceReport" // #18481
#define HANDLE_BEAVC_MAILED_STATUS	"HandleBeAVCMailed" //PYR-18568
#define HANDLE_BE_VALIDATION_STATUS	"HandleBeValidation" //PYR-18568
#define MANAGE_DUMMY_TOURNAMENTS    "ManageDummyTourns"		// #18129
#define HANDLE_BE_USER_LOSS_LIMIT	"HandleBeLossLimit"		// #18926
#define HANDLE_KILL_ITSELF			"HandleKillItself" // PYR-19031
#define LCTR_MANAGE_GAME_CARDS      "ManageGameCards" // PYR-19282
#define CREATE_USER_ACCOUNT			"CreateUserAcct" // PYR-16191
#define CALCULATE_TTICKETS_TOTAL	"CalcTTicketsTotal" // PYR-16360
#define COLOSSUS_GENERAL			"ColGeneral"		// PYR-21099
#define COLOSSUS_INVESTIGATOR		"ColInvestigator"	// PYR-21099
#define COLOSSUS_MANAGER			"ColDeleteImports"	// PYR-19569
#define MANAGE_EPIS_GATEWAY         "ManageEpisGw" // PYR-19884
#define MANAGE_PSAPI_GATEWAY        "ManagePsapiGw" // PYR-19591
#define HANDLE_RMNOK_HELD_FUNDS     "HandleRmNokHeldFunds" // #19475
#define MANAGE_SPM_GATEWAY          "ManageSpmGw" // PYR-20450 (Danish Gaming Authority gateway)
#define MANAGE_CNJ_GATEWAY          "ManageCnjGw" // PYR-20482 (Spanish Gaming Commission gateway)
#define MANAGE_ASIA_GATEWAY         "ManageAsiaGw" // PYR-64516 (Swedish Gaming Commission gateway)
#define MANAGE_DESH_GATEWAY         "ManageDeshGw" // PYR-35064 (Ministry of the Interior of State for Schleswig-Holstein gateway)
#define MANAGE_BG_GATEWAY           "ManageBGGw"   // PYR-37704 (Bulgarian gateway)
#define MANAGE_PT_GATEWAY           "ManagePtGw"   // PYR-38561 (Portugal gateway)
#define MANAGE_SWEDISH_GATEWAY      "ManageSwedishGw" // PYR-78900 (Swedish gateway)
#define MANAGE_CZ_GATEWAY			"ManageCzGw" // PYR-91513 (Czechish gateway)
#define MANAGE_VIP_LEVELS        	"ManageVipLevels" // PYR-20134
#define HANDLE_ARCHIVED_USERS      	"HandleArchivedUsers" // #20399
#define HANDLE_IPBLOCK_RULES_LM    	"HandleIpBlockRulesLM" // #20675
#define HANDLE_IPBLOCK_RULES_SM    	"HandleIpBlockRulesSM" // #20675
#define HANDLE_USER_TABLE_LIMIT     "HandleUserTableLimit" // #20052
#define CLEAR_SECURITY_ALERTS		"ClearSecurityAlerts" // PYR-20849
#define TOURN_DEAL_CHAT             "TournDealChat"         // PYR-18684
#define MASS_OPERATE_EXCEPTIONS     "MassOperatExceptions"  // PYR-20589
#define GC_ADMIN_DOWNLOAD_VOUCHERS  "DownloadVouchers"      //17474 moved from gcdbm.h
#define HANDLE_TRAMAIL_CONFIG       "HandleTRAMailConfig"   // PYR-21469
#define DEL_CDMS_SCANS				"DeleteCdmsScans"		// PYR-21618
#define MANAGE_TABLE_DBM			"ManageTableDbm"		// PYR-21855
#define MANAGE_ZOOM_SESSIONS		"ManageZoomSessions"	//PYR-22132
#define HANDLE_HSMT_CONFIG          "HandleHSMTConfig"      // PYR-16557
#define VIPCLUB_AUTO_EMAILS         "VIPClubAutoEmails"     // PYR-21558
#define TS_MANAGE_INTERNAL_ERRORS	"ManageInternalErrors"	// PYR-19814
#define VERIFY_FISCAL_CODE			"UpdateVerifyTaxID"		// PYR-18383
#define GC_ADMIN_VALIDATE_VOUCHERS  "ValidateVouchers"      // PYR-22529
#define GC_ADMIN_UPDATE_VOUCERBATCH "UpdateVoucherBatch"    // PYR-22529
#define HANDLE_REGULATOR_BAN		"ChangeBanByRegulator"  // PYR-23145
#define UPDATE_SPAINBANCHECK_MD5KEY "UpdateSpainMd5Key"     // PYR-25449
#define VERIFY_SPAINBANCHECK        "VerifySpainBanCheck"   // PYR-28586
#define ASTROPAYDIRECT_ADMIN        "AstropayDirectAdmin"   // PYR-27624
#define MANAGE_TCA_CONTROL			"ManageTCAControl"		// PYR-56471
#define MANAGE_CCDA_CONTROL			"ManageCCDAControl"		// PYR-64013
#define VIEW_CCDA_RULES				"ViewCCDARules"			// PYR-89273
								//  |01234567890123456789|
#define CSI_VIEW_ENGINES_AND_RULES	"CSIViewEnginesRules"	// PYR-87582
#define CSI_MODIFY_ENGINES			"CSIModifyEngines"		// PYR-87582
#define CSI_MODIFY_RULES			"CSIModifyRules"		// PYR-87582
#define CSI_RULE_PERFORMANCE		"CSIRulePerformance"	// PYR-87582

#define MANAGE_PLO_EXCLUSION		"ManagePLOExcl"			// PYR-20119
#define MANAGE_ZOOM_EXCLUSION		"ManageZoomExcl"		// PYR-20119
#define MANAGE_HU_SITNGO_EXCLUSION	"ManageHuSitNGoExcl"	// PYR-20119
#define MANAGE_HT_SITNGO_EXCLUSION	"ManageHTSitNGoExcl"	// PYR-20119
#define MANAGE_SAG_EXCLUSION		"ManageSagExcl"			// PYR-30225

#define USERINSTALLID_PRIV			"InstallIdValidation"	// #21829
#define SETINSTALL_VALIDATION		"SetSMSValidation"		// #21829

#define CAN_CHANGE_TRANS_STATUS		"CanChangeTransStatus"
#define MANAGE_SUSPEND_RM			"ManageSuspendRM"		// PYR-20489
#define MANAGE_PIPEITEMS			"ManagePipeItems"		//PYR-20426
#define MANAGE_STAR_CODES			"ManageStarCodes"		//PYR-22494
#define UPDATE_NAC_ITALY            "UpdateNacItaly"        // #20871
#define MANAGE_FACEBOOK_LINK        "ManageFacebookLink"    // #23925
#define MANAGE_SMS_PWDRESET			"ManageSmsPwdReset"		// PYR-22184
#define MANAGE_HOLD_USER_FUNDS		"ManageHoldUserFunds"	// PYR-23464
#define SEND_MASS_SMS				"SendMassSMS"			// PYR-23088
#define SEND_MASS_SMS_VAL			"SendMassSMSVal"		// PYR-23088
#define MANAGE_MASS_SMS				"ManageMassSMS"			// PYR-23088
#define MANAGE_FULLTILT_UNPAIRING	"ManageFullTiltUnpair"	// PYR-23771
#define MANAGE_FULLTILT_SCORING  	"ManageFullTiltScore" 	// PYR-24174
#define REREAD_REGULATOR_INI        "RereadRegulatorIni"    // #24326
#define MANAGE_SMS_MARKETING_FLAG	"ManageSmsMarketFlag"	// PYR-23394
#define HANDLE_TRUE_IP_WHITELIST	"HandleTrueIpWhiteLst"	// PYR-35051
#define MANAGE_USER_BLOCKING_CODES	"ManageBlockingCodes"	// PYR-24812
#define MANAGE_TOURN_HAPPY_HOURS	"ManageTournHappyHour"	//PYR-24396
#define SEND_MOBILE_PUSH_NOTIF      "MobileNotification"	//PYR-24906
#define MANAGE_VALIDATION_EMAIL		"ManageValidationEm"	//PYR-24310
#define UNLIMITED_HAND_HISTORY		"UnlimitedHandHistory"	// PYR-26099
#define USER_POPUP_TO_CONFIRM		"UserPopupToConfirm"	// PYR-26155
#define VIP_MANAGE_SHIPPERS			"ManageShippingComp"	// PYR-26304
#define MANAGE_AWARDS_RIGHT			"ManageAwards"			// PYR-24840
#define PAUSA_SUPERADMIN			"PausaSuperAdmin"		// PYR-27646
#define MANAGE_DGE_ENTROPY			"ManageDgeEntropy"		// PYR-27747
#define MANAGE_NJBLACKLIST          "ManageNjBlacklist"     // PYR-27257
#define MANAGE_TAB_ITEMS			"ManageTabItems"		// PYR-26712
#define FULLTILT_ACCOUNT_VIEW       "FullTiltAccountView"   // PYR-28270
#define CASHOUT_ON_USER_BEHALF		"CashoutOnUserBehalf"	// PYR-27726
#define DEPOSIT_ON_USER_BEHALF		"DepositOnUserBehalf"	// PYR-27726
#define MANAGE_PLACEMENTS			"ManagePlacements"		// PYR-27251
#define BYPASS_VIP_RESTRICTION		"BypassVIPLRestrict"	// PYR-26884
#define MANAGE_STRONG_AUTH_REQD     "ManageStrongAuthReqd"	// PYR-28552
#define MANAGE_IMAGE_GALLERY		"ManageImgGallery"		// PYR-26568
#define UPDATE_IT_USER_LOGIN_STATUS "UpdateITLoginStatus"	// PYR-28691
#define MANAGE_BIZ_ACCOUNTS			"ManageBizAccounts"		// PYR-28864
#define CHANGE_PM_USER_PWD			"ChangePMUserPwd"		// PYR-28261
#define MANAGE_IMAGE_CRITERIA		"ManageImgCriteria"		// PYR-28965
#define MANAGE_ICE					"ManageIce"				// PYR-29926
#define MANAGE_EXCLUDED_OBJECTS		"ManageExclObjects"		// PYR-29604
#define MANAGE_CHALLENGE_QUESTIONS  "ManageChallengeQuest"	// PYR-28732
#define UNREGISTER_BG_PLAYER		"UnregisterBgPlayer"    // PYR-49027
#define CHECK_ESTONIA_BLACKLIST     "CheckEstoniaBList"     // PYR-51747
// PYR-29702
#define MASS_EMAIL_100              "MassEmail100"
#define MASS_EMAIL_1000             "MassEmail1000"
#define MASS_EMAIL_5000             "MassEmail5000"
#define MASS_EMAIL_10000            "MassEmail10000"
#define MASS_EMAIL_20000            "MassEmail20000"
#define MASS_EMAIL_50000            "MassEmail50000"
#define MASS_EMAIL_100000           "MassEmail100000"
#define MASS_EMAIL_300000           "MassEmail300000"
#define MASS_EMAIL_500000           "MassEmail500000"
#define MASS_EMAIL_1000000          "MassEmail1000000"
#define MASS_EMAIL_5000000          "MassEmail5000000"
#define MASS_EMAIL_UNLIMITED        "MassEmailUnlimited"

#define MANUAL_LICENSE_CHANGE		"ManualLicenseChange" // PYR-26723
#define MANAGE_XC_GAME_REMOVAL		"ManageXcGameRemoval" // PYR-29620
#define MANAGE_ELV_MANDATE			"ManageElvMandate"    // PYR-30281
#define MANAGE_EMAIL_ATTACHMENT		"ManageEmailAttachmnt" // PYR-30777
#define REREAD_AUXHUB_INIDATA		"RereadAuxhubIniData"  // PYR-31538
#define MANAGE_NJ_TESTING_PRIV2		"ManageNjTestingPriv2" // PYR-31299
#define REMOVE_USER_LOGINDBM_CACHE	"RemUsrLoginDBM" // PYR-31793
#define MANAGE_ACCT_REVALIDATION	"ManageAcctReval"		//	PYR-28735
#define MANAGE_SET_AFFILIATE		"ManageSetAffiliate"	//	PYR-30714
#define MANAGE_ROLLS_RIGHT			"ManageRolls"			// PYR-31829
#define ISSUE_USERROLLS_RIGHT		"IssueUserRolls"		// PYR-31829
#define CHANGE_USERROLLS_RIGHT		"ChangeUserRolls"		// PYR-31829
#define HANDLE_STR_CONFIG			"HandleSTRConfig"		// PYR-24460
#define MANAGE_PROMO_DEALS			"ManagePromoDeals"		// PYR-32612
#define MANAGE_CASINO_LIMIT_INCREASE "ManageCasinoLimInc"   // PYR-32703
#define MANAGE_CASINO_PLAY          "ManageCasinoPlay"     // PYR-32703
#define MANAGE_SPORTS_LIMIT_INCREASE "ManageSportsLimInc"   // PYR-32180
#define MANAGE_SPORTS_PLAY 	        "ManageSportsPlay"     // PYR-32180
#define MANAGE_SHOW_CASINO_FLAG		"ManageShowCasinoFlag" // PYR-33086
#define MANAGE_SET_EMAIL_VALIDATION "ManageSetEmailValid"	//	PYR-29932
#define REMOVE_USER_DBM_CACHE		"RemUsrDBM"				//	PYR-33660
#define MANAGE_WEB_DOCS				"WebDocsAdmin"			//	PYR-33416
#define MANAGE_GEOCOMPLY_REQUEST    "GeocomplyRequest"	    // PYR-30754
#define SHUTDOWN_OLTP_RIGHT			"ShutdownOLTP"			// PYR-34662
#define VIEW_ACCOUNT_MANAGEMENT		"ViewAcctMgmt"			// PYR-33481
#define INVALIDATEASTROPAY_ADMIN    "InvalidateAstropay"   // PYR-31945
#define MANAGE_RATE_MOBILE_APP      "ManageRateMobileApp"	//	PYR-33276
#define MANAGE_CASINO_VIP			"ManageCasinoVip"		// PYR-35568
#define MANAGE_FPP_BUNDLES			"ManageFppBundles"		// PYR-32027
#define HANDLE_LOGIN_SETTINGS       "HandleLoginSettings"	// PYR-36092
#define HANDLE_FREEMIUM_SETTINGS    "HandleFreemium"		// PYR-36092
#define MANAGE_PRIVS_IN_BATCH		"ManagePrivsInBatch"	// PYR-32027
#define MANAGE_SPORTS_VIP			"ManageSportsVip"		// PYR-36335
#define HANDLE_ORION				"HandleOrion"			// PYR-37587
#define MANAGE_NO_SUGGESTED_DEPOSIT	"ManageNoSugDeposit"	// PYR-31386
#define MANAGE_NO_UPSELL			"ManageNoUpsell"		// PYR-31386
#define MANAGE_NO_SPORTS_BONUS		"ManageNoSportsBonus"	// PYR-39041
#define SUBMIT_ADMIN_RMF			"SubmitAdminRMF"		// PYR-39612
//#define UPDATE_NAC_PORTUGAL         "UpdateNacPortugal"     // PYR-39019
//#define USE_FRIEND_TOOL				"UseFriendTool"			// PYR-36105 // PYR-53789 - obsolete
#define PERFORM_FT_CONVERSION		"PerformFtConversion"	// PYR-39878
#define FT_BLOCK_USER				"FtBlockUser"			// PYR-41600
#define MANAGE_MINIGAMES			"ManageMinigames"		// PYR-37300 - DEPRECATED AS OF MAIN.07 - superceded by MANAGE_THEDEAL and MANAGE_CARDMATCH
#define MANAGE_MINIGAME_ACCOUNTS	"ManageMinigameAccts"   // PYR-37300
#define FINALIZE_TICKETTYPE			"FinalizeTicketType"	// PYR-42977
#define HANDLE_MLV					"HandleMLV"				// PYR-42977
#define HANDLE_FREUD_CONFIG			"HandleFreudConfig"		// PYR-51363
#define UPDATE_NAC_ANY_LICENSE      "UpdateNacAnyLicense"   // PYR-43381 + PYR-39019
#define HANDLE_NOTCASHINS			"HandleNotcashins"		// PYR-37639
#define CREDIT_FPP_LIST				"CreditFPPList"			// PYR-27421
#define RESET_FPP_LIST				"ResetFPPList"			// PYR-27421
#define CREDIT_FPP					"CreditFPP"				// PYR-27421
#define CREDIT_PLAY_MONEY			"CreditPlayMoney"		// PYR-27421
#define MANAGE_GAMETICKETTYPE_RIGHT	"ManageGameTicketType"	// PYR-45645
#define ISSUE_GAMETICKET_RIGHT	    "IssueGameTicket"	    // PYR-45645
#define CHANGE_GAMETICKET_RIGHT	    "ChangeGameTicket"      // PYR-45645
#define MANAGE_REGIONS				"ManageRegions"			// PYR-42113
#define MANAGE_THEDEAL				"ManageDeal"            // PYR-45836
#define MANAGE_CARDMATCH			"ManageCardMatch"		// PYR-45836
#define MANAGE_NO_KO_TOURNS			"ManageNoKOTourns"  // PYR-43888
#define VIEW_HIDDEN_ACCOUNT_RO      "ViewHiddenAccountRO"	// PYR-31795
#define MANAGE_BONUSGAMES           "ManageBonusGames"      // PYR-40872
#define CANCEL_AVC_CHECK            "CancelAvcCheck"        // PYR-35827
#define MASS_EMAIL_TEXT_INPUT		"MassEmailTextInput"	// PYR-29598
#define SET_DUE_DILIGENCE_FLAG		"SetDueDiligenceFlag"	// PYR-47543
#define HANDLE_WAGERMEAS			"HandleWagerMeas"		// PYR-46309
#define VERIFY_CARD					"VerifyCard"			// PYR-47572
#define MANAGE_XC_FORCE_GAME_REM    "ManageXcForceGameRem"	// PYR-48559
#define GET_NET_LOSSES_IN_PERIOD    "GetNetLossesInPeriod"	// PYR-47770
#define ISSUE_PENDING_ACTION	    "IssuePendingAction"	// PYR-44386
#define FINALIZE_PENDING_ACTION	    "FinalizePendingAct"	// PYR-44386
#define MANAGE_STARS_REWARDS		"ManageSRConfig"		// PYR-49481
#define MANAGE_SR_PLAYER			"ManageSRPlayer"		// PYR-49459
#define VIP_MANAGER_ASSIGN			"VipManagerAssign"		// PYR-50010
#define MANAGE_SR_CATALOG			"ManageSRCatalog"		// PYR-50857
#define GC_ADMIN_GC_RIGHTS          "GCProcessing"
#define GC_ADMIN_DBT_RIGHTS         "HandleDBTAccount"
#define GC_ADMIN_AFFILIATEDEALS     "HandleAffiliateDeals"
#define GC_ADMIN_ADD_UNCASHEDCHEQUES_RIGHTS    "uncashedchqreminder" // #14344
#define VIP_MANAGER_ACCESS			"VipManagerAccess"		// PYR-50347, PYR-50357, PYR-50344, PYR-50412
#define HANDLE_BLOCK_PM_PURCHASE	"BlockPMPurchase"		// PYR-41219
#define HANDLE_LIVEEVENTPAYOUT 	    "HandleLiveEventPO"	    // PYR-47840
#define BLOCKED_CARD_ALLOW_CASHOUT  "BlockCrdAllowCashout"  // PYR-51733
#define MANAGE_JOBSCONTROLLER		"ManageJobsCtrl"		// PYR-50202
#define MANAGE_POOL_BLOCK_STATUS	"PoolblockSt"			// PYR-54027
#define UPDATE_USER_USEDBALANCE     "UpdateUsedBalance"     // PYR-54303
#define SET_FISCALCODE_VERIFIED		"SetFiscalCodeVrfd"	    // PYR-53001
#define HANDLE_APAL_SETTINGS		"HandleApal"			// PYR-55171
#define MANAGE_TEMP_PIN				"ManageTempPin"			// PYR-51853
#define MESSAGE_SINGLE_USER			"MsgSingleUser"			// PYR-53014
#define MANAGE_DFMA					"ManageDFMA"			// PYR-53577
#define BYPASS_AUTO_ACTION_MAA		"BypassAutoActionMAA"	// PYR-56097
#define LOAD_TABLE_SCRIPT			"LoadTableScript"		// PYR-48096
#define MANAGE_POWERUP_PLAY			"ManagePowerUpPlay"		// PYR-45178
#define STARSWALLET_CHECK			"StarsWalletCheck"		// PYR-57078
#define MANAGE_GDPR					"ManageGdpr"			// PYR-58575
#define PREDICTOR_ISSUE_TICKET		"PredictorIssueTicket"	// PYR-63214
#define PREDICTOR_VOID_TICKET		"PredictorVoidTicket"	// PYR-63214
#define MANAGE_ICE_SYSTEM_TASKS		"ManageIceSystemTasks"	// PYR-29926
#define HANDLE_GEOLOC_CONFIG		"HandleGeolocConfig"	// PYR-66110
#define APPROVE_STARS_REWARDS		"ApproveSRConfig"		// PYR-59426
#define ASIAGAMING_PORTAL_RIGHTS	"AGPortal"				// PYR-64517
#define DEBIT_FPP					"DebitFPP"				// PYR-67536
#define MANAGE_UNFOLD_EXCLUSION		"ManageUnfoldExcl"		// PYR-70870
#define REMOVE_HANGING_LOGINS_RIGHT	"RemoveHangingLogins"	// PYR-72821
#define MANAGE_RG_RISK_LEVEL		"ManageRgRiskLevel"		// PYR-71294
#define ASIA_KYC_UPLOAD				"AsiaKYCUpload"			// PYR-64085
#define RECENT_PLAYED_LIST			"RecentPlayedList"		// PYR-87795
#define MANAGE_OTP_BYPASS			"ManageOTPBypass"		// PYR-86272
#define MANAGE_SUGGESTEDDEPOSITS	"ManageSDCampaigns"		// PYR-76141
#define RG_CHAT_MONITOR				"RGChatMonitor"			// PYR-80956
#define MANAGE_NO_ALL_IN_CASHOUT	"ManageNoAllInCashOut"	// PYR-79935
#define READ_CDMS_QUEUE				"ReadCDMSQueue"			// PYR-98229
#define MANAGE_USPABLACKLIST        "ManageUsPaBlacklist"   // PYR-92697
#define RESEND_ASIA_MESSAGES        "ResendAsiaMessages"    // PYR-105007
#define STEALTH_TABLES				"StealthTables"			// PYR-106112
#define MANAGE_GIFT_CODES			"ManageGiftCodes"		// PYR-111568
#define BOLD_CHAT_OUTBOUND			"BoldChatOutbound"		// PYR-117862
#define CH_MIGRATION_RIGHT			"CHmigration"			// PYR-117281
#define IT_REPORT_DEPOSIT_LIMITS	"ITReportDepLimits"		// PYR-106575
#define CAN_MIGRATE_TO_EU_RIGHT		"CanMigrateToEu"		// PYR-118031
#define MANAGE_PSPC					"ManagePSPC"			// PYR-118097
#define MANAGE_WEBUXASSETS			"ManageWebUxAssets"		// PYR-123307
//								//  |01234567890123456789|
//								// Maximum length is 20 symbols

#define BONUS_CENTSPERFPP 100


#define	SECUR_OTHER_COUNTRY		1
#define	SECUR_EARLY_CASHIN		2
#define	SECUR_OTHER_COUNTRY_BIN	3

//TOURNPROPS
#define	PROP_TYPE_AWARDS					0
#define PROP_TYPE_NOTIFY					1
#define PROP_TYPE_SERVINT					2
#define PROP_TYPE_CREATOR					3
#define PROP_TYPE_RECREATOR					4
#define PROP_TYPE_SERV_FLAGS				5
#define PROP_TYPE_REGISTR_MES				6
#define PROP_TYPE_PRE_REG_MES				7
#define PROP_TYPE_HAND_BY_HAND				8
#define PROP_TYPE_NOREGISTR_MES				9
#define PROP_TYPE_COUNTRIES					10
#define PROP_TYPE_ALREADYREGISTR			11
#define PROP_TYPE_TICKETS					12
#define PROP_TYPE_FINALTABLE_TEXT			13
#define PROP_TYPE_ADMISSION					14
#define	PROP_TYPE_AWARDS_CONTINUE			15
#define PROP_TYPE_HAND_BY_HAND_CONTINUE		16
#define PROP_TYPE_COUNTRY_STRING			17
#define PROP_TYPE_MESSAGE_BOARD_TEXT		18
#define PROP_TYPE_TOURN_NAME				19
#define PROP_TYPE_MULTIDAYPROPS				20
#define PROP_TYPE_SPECIAL_WINNER_EMAIL		21
#define	PROP_TYPE_BOUNTIES					22
#define	PROP_TYPE_BOUNTIES_CONTINUE			23
#define	PROP_TYPE_PRO_BOUNTIES				24
#define	PROP_TYPE_BOUNTY_MULTIPLIERS		25
#define	PROP_TYPE_FALLBACK_SATELLITE_TARGETS 26
#define	PROP_TYPE_SATELLITE_TARGET_REF		27 // PYR-26990
#define	PROP_TYPE_PLACE_DEALS				28	// [AO 2010-04-20] #13832
#define	PROP_TYPE_PLACE_DEALS_CONTINUE		29	// [AO 2010-04-20] #13832
#define	PROP_TYPE_PERSONAL_DEALS			30	// [AO 2010-04-20] #13832
#define	PROP_TYPE_PERSONAL_DEALS_CONTINUE	31	// [AO 2010-04-20] #13832
#define	PROP_TYPE_SINGLE_PLAYER_COUNTRY_RESTRICTION		32	//PYR-15506
#define PROP_TYPE_REGISTRATION_BLOCKING_OVERRIDE		33 // #16379
#define PROP_TYPE_SPEED_DISPLAY             34  // #16783
#define PROP_TYPE_TOURNAMENT_SNG			35  // #16783
#define PROP_TYPE_STEP						36  // #16783
#define PROP_TYPE_AFFILIATE_FUNDED			37	// #18692
#define PROP_TYPE_ASYNC_REGISTRATION		38	//PYR-19016
#define PROP_TYPE_TIMED_TOURNAMENT			39	//PYR-18546
#define PROP_TYPE_FLIGHT_TOURN_STATS		40  // #19167 - may be present for target flights, collects data from ended feeder flights
#define PROP_TYPE_FINAL_FLIGHT_INFO         41  // #19167 - is present for all flights except final
#define PROP_TYPE_TRACKING_CODES			42  // #18146
#define PROP_TYPE_AFFILIATE_IDS             43  // #18146
#define PROP_TYPE_PROMO_CODES               44  // #18146
#define PROP_TYPE_AFFILIATE_INTIDS          45  // #18146
#define PROP_TYPE_EFF_CREATOR				46	//PYR-18476
#define PROP_TYPE_EFF_CANCELLER				47	//PYR-18476	
#define PROP_TYPE_DISPLAY_RAKE_AND_BUYIN	48	//PYR-20531 - indicates that the tournament is a flight target. if it ever needs to be changed then flight liability must change too as it depends on this fact.
#define PROP_TYPE_VPP_MULTIPLIERS			49	//PYR-21113
#define PROP_TYPE_DK_PLAYER_REGISTERED		50	//PYR-16996
#define PROP_TYPE_LOG_LOCATION              51  //PYR-21877 - save IP and log file name into this property to speed up the search
#define PROP_TYPE_FLIGHT_REF				52  //PYR-22691 - is always present for flight tournaments
#define PROP_TYPE_TGMS						53  //PYR-22691
#define PROP_TYPE_SPECIAL_EMAIL				54  //PYR-22533
#define PROP_TYPE_BLITZ_NORMAL_DEALING_THRESHOLD		55  //PYR-22885 number of players
#define PROP_TYPE_BLITZ_NORMAL_DEALING		56  //PYR-22885 
#define PROP_TYPE_MAIN_DBM					57	// #18279 - set only when forcing tournament to go to OLTP or TournDbm
#define PROP_TYPE_TOURNBRAND				58	//PYR-20448
#define PROP_TYPE_QUICKLOBBY_VISIBLE		59	// #20434 - 1 - visible, 0 (or missing) - not visible in QuickLobby
#define PROP_TYPE_AUTOUNREGDELAY			60	// #20055 - in seconds, 0 if missing
#define PROP_TYPE_LIVEEVENTSATELLITE		61	//PYR-20448
#define PROP_TYPE_SCALEPM					62	// PYR-23932
#define PROP_TYPE_AVG_LEVEL_LENGTH			63	// PYR-23736
#define PROP_TYPE_HAPPYHOURS				64	//PYR-24396
#define PROP_TYPE_HAPPYHOURS_MULTIPLIERS	65	//PYR-24396, set on start
#define PROP_TYPE_USE_PERM_CONNECTION		66	//PYR-23950
#define PROP_TYPE_MINCLIENTVERSION			67  //PYR-25260
#define PROP_TYPE_MINMOBILEVERSION			68  //PYR-25260
#define PROP_TYPE_SHOWOBSERVERS				69	//PYR-22393
#define PROP_TYPE_GOLDENSNG					70	//PYR-24382 (obsolete)
#define PROP_TYPE_GOLDENSNG_RESULT			71	//PYR-24382 (obsolete)
#define PROP_TYPE_TABLE_TEXT				72	//PYR-26178
#define PROP_TYPE_NUM_REENTRIES				73	//PYR-24444
#define PROP_TYPE_NUM_FAV_REFERENCE			74	//PYR-26708
#define PROP_TYPE_LIMITED_ENTRY_GROUP		75	//PYR-25970
#define PROP_TYPE_AUTOPUBLISH				76	//PYR-27969
#define PROP_TYPE_AUTOPUBLISH_CONTINUE		77	//PYR-27969
#define PROP_TYPE_LIMITED_ENTRY_GROUP_END	78	//PYR-25970
#define PROP_TYPE_OPTSKIPBREAK				79	//PYR-18141
#define PROP_TYPE_FACTORY_DEAD				80	//PYR-27251
#define PROP_TYPE_FACTORY_RESURRECTED		81	//PYR-27251
#define PROP_TYPE_LIMITED_ENTRY_TGM_ID		82	//PYR-28521
#define PROP_TYPE_MAX_OPEN_FOR_REG			83	//PYR-20127
#define PROP_TYPE_CAN_UNREG_FROM_TARGET		84	//PYR-26438
#define PROP_TYPE_NSTACK_TOURN				85  //PYR_25226
#define PROP_TYPE_NEXT_TOURN_MILESTONE_EVENT_INDEX 86  //PYR-28689
#define PROP_TYPE_AUTOMATED_DEAL            87  //PYR-25565
#define PROP_TYPE_SAG_PAYOUT_TABLE          88  //PYR-25565
#define PROP_TYPE_TOURN_MILESTONE_AWARD_REQID  89 //PYR-28269
#define PROP_TYPE_SAG_PAYOUT_INFO           90  //PYR-28752 goes to SAGTOURNS table
#define PROP_TYPE_TOURN_GROUP_REF			91	//PYR-30165
#define PROP_TYPE_DEAL_CASHIER				92	//PYR-27091
#define PROP_TYPE_AUTODEAL_TYPE             93  //PYR-25565
#define PROP_TYPE_RESURRECTED_TOURN_HANDLED 94 //PYR-28842
#define PROP_TYPE_SCRIPTNAME				95 // PYR-20814
#define PROP_TYPE_BLOCK_BROADCASTS			96	//PYR-23744
#define PROP_TYPE_DEAL_EXCLUDE			    97	//PYR-22909
#define PROP_TYPE_AAMS_ENTRY_FLIGHT_INDEX   98	//PYR-26095
#define PROP_TYPE_REBUY_ADDON_RAKE_COUNTRIES 99	//PYR-32776
#define PROP_TYPE_CANCELLED_FLIGHTS_AAMS_INDICES	100	//PYR-26095 outdated, is replaced with TGM_PROP_TYPE_AAMS_DATA
#define PROP_TYPE_NEXT_PHASE_PRIZE			101	//PYR-31690
#define PROP_TYPE_PROMOSPIN_DATA			102	//PYR-33384
#define PROP_TYPE_PROMOSPIN_ANNOUNCEMENT	103	//PYR-33657
#define PROP_TYPE_PROMOSPIN_RESULT			104	//PYR-33657
#define PROP_TYPE_PROMOSPIN_DELAYS			105	//PYR-33657
#define PROP_TYPE_RMPM_LOBBY                106 //PYR-33483
#define PROP_TYPE_STORM                     107 //PYR-34849
#define PROP_TYPE_STORM_CONTINUE            108 //PYR-34849
#define PROP_TYPE_FLIGHT_TRANSFER_AMOUNTS   109 //PYR-32003 stores effective money and users transferred to the flight
#define PROP_TYPE_DELAYED_PLACE_DELTA	    110 //PYR-37048 
#define PROP_TYPE_BUBBLE_LOGIC_MODE		    111 //PYR-37048 
#define PROP_TYPE_CUSTOM_PRIZES_DESC	    112 //PYR-33574 list of prizes in English (for tourn stats)
#define PROP_TYPE_ICE_EVENT_EXCLUDE			113 //PYR-38485
#define PROP_TYPE_PS_LIVE_REF				114 //PYR-34863
#define PROP_TYPE_FAST_FLIGHT_LOSERS_REMOVAL 115  //PYR-42171
#define PROP_TYPE_TOURN_CHUNK_USERS         116  // PYR-42171
#define PROP_TYPE_TOURN_NEW_STANDINGS       117  // PYR-44791 (obsolete, not in use)
#define PROP_TYPE_MIN_RESERVED_TABLES       118  // PYR-45030
#define PROP_TYPE_CUSTOM_SPEED_DISPLAY		119	//PYR-33657
#define PROP_TYPE_WIN_THE_BUTTON_THRESHOLD	120 //PYR-32587
#define PROP_TYPE_WIN_THE_BUTTON_OVER		121 //PYR-32587 if exists and set to 1, then win the button mode is over
#define PROP_TYPE_TOURN_RESPAWN_TYPE        122  // PYR-25525
#define PROP_TYPE_SAG_TARGET_REFS			123 //PYR-43478
#define PROP_TYPE_FINAL_TABLE_REDRAW        124 //PYR-22908 (obsolete)
#define PROP_TYPE_ALT_BOUNTIES				125	//PYR-31658
#define PROP_TYPE_ALT_BOUNTIES_CONTINUE		126	//PYR-31658
#define PROP_TYPE_FINAL_TABLE_SIZE			127 //PYR-45970
#define PROP_TYPE_REPLAY_STARTTIME_ENDTIME	128	//PYR-31336
#define PROP_TYPE_DEAL_TIMEOUT				129 //PYR-47376
#define PROP_TYPE_QA_TEST_PARAMS			130 //PYR-48837 for easy error emulation in QA, not for use in production
#define PROP_TYPE_INCLUDE_DO_NOT_PAY		131	//PYR-21559
#define PROP_TYPE_REPLAY_TOURNMASKS			132	//PYR-50470
#define PROP_TYPE_ALLIN_LEVEL				133 //PYR-51309
#define PROP_TYPE_SAGMAX_TRIPLE_SPINNER		134 //PYR-51310
#define PROP_TYPE_HANDS_UNTIL_ALLIN_LEVEL	135 //PYR-55378
#define PROP_TYPE_EARLY_PAYOUTS				136 //PYR-50812
#define PROP_TYPE_MULTIDAYPROPS_LIST		137 //PYR-25137
#define PROP_TYPE_SAG_FREE_BET				138 //PYR-59874
#define PROP_TYPE_VISIBILITY_CRITERIA		139 //PYR-64994
#define PROP_TYPE_MOBILE_NAME				140	//PYR-66779
#define PROP_TYPE_GROUP_TYPE_MASK			141	//PYR-66779
#define PROP_TYPE_REPLAY_FASTFORWARD		142	//PYR-63168
#define PROP_TYPE_CANCEL_BY_ADMIN_INFO		143 //PYR-66851
#define PROP_TYPE_NONQUALIFIERS_ADDITIONALRESTRICTIONS  144 //PYR-58739
#define PROP_TYPE_CVLSCHEMA					145 // PYR-86838
#define PROP_TYPE_LATE_REENTRY				146 // PYR-50362
#define PROP_TYPE_MAX_PAYOUT_SAG			147 // PYR-98177
#define PROP_TYPE_ELIGIBLE_GROUPS			148	//PYR-66779
#define PROP_TYPE_TOURN_STATUS				149 // PYR-112271 - the current internal tournament status (as in mtlobby instance)
#define PROP_TYPE_PROCESSID					150 // PYR-112271 - will be removed once the tournament is closed/cancelled
#define PROP_TYPE_LAST_INSTANCE_ADDRESS		151 //PYR-109267
#define PROP_TYPE_OVERLAY_ADMIN_EMAIL		152 // PYR-107722
#define PROP_TYPE_KNOCKOUT_SPIN_PAY_TABLE	153 // PYR-120482
#define PROP_TYPE_KNOCKOUT_SPIN_PAY_TABLE_CONTINUE	154 // PYR-120482
#define PROP_TYPE_TABLE_TRAIT				155 // PYR-122216
#define PROP_TYPE_PPPID						156 // PYR-123713
#define PROP_TYPE_HIGHLIGHT_COLOR			157 // PYR-112667

//ACTIVETOURNPROPS
#define ACTPROP_TYPE_CURRENT_BLINDS_LEVEL		0  //PYR-30443
#define ACTPROP_TYPE_DYNAMIC_FLAGS				1  // PYR-42474 dynamically changing tourn flags 
#define ACTPROP_TYPE_PUBLISHED_STATUS			2  // PYR-49647 indicates whether tourn is currently published
#define ACTPROP_TYPE_PRIZEPOOL_FINALIZED		3  // PYR-89625 indicates prizepool has been finalized via DBM_Q_MTL_PRIZE_ADDON

//TOURNUSERPROPS
#define TU_PROP_TYPE_FLIGHTWINNER			0  //PYR-24907
#define TU_PROP_TYPE_FLIGHTLOSER			1  //PYR-24907
#define TU_PROP_TYPE_CHIPS_TO_REMOVE		2  //PYR-26055
#define TU_PROP_TYPE_CANCEL_CHIPS			3  //PYR-28503 - user chips when tournament was cancelled, and Tchips refunded
#define TU_PROP_TYPE_REFUND_INFO			4  //PYR-31765 - user got refund when tournament was cancelled by DBM_Q_MTL_UNREG_USERS
#define TU_PROP_TYPE_SAT_WIN_REFUND_INFO	5  //PYR-32351 - sat winners got refund when tournament was cancelled by DBM_Q_REFUND_SAT_WINNERS
#define TU_PROP_TYPE_FLIGHT_MOVE_DATA		6  //PYR-32003 - temporary property which marks flight move transaction (REQID:DSTID)
#define TU_PROP_TYPE_CANCEL_CALC_INFO		7  //PYR-66851 - property exposing intermediate calculations for a roll sideways/forwards
#define TU_PROP_TYPE_CANCEL_PAYMENT_INFO	8  //PYR-66851 - property summarizing value paid to player on tourn cancel by admin
#define TU_PROP_TYPE_MILESTONE_PAYMENT		9  //PYR-102950 - user was awarded a milestone prize in this tournament, currently only populated for CH users
#define TU_PROP_TYPE_PROGRESSIVE_PLUS_INFO	10 //PYR-114764 - intProp0: initial chips, intProp1: initial buyin tier id, intProp2: rake, bigIntProp: progressive plus seed value on head of user at tournament start
#define TU_PROP_TYPE_PROGRESSIVE_PLUS_ENTRYID 11 //PYR-120476 - bigIntProp: progressive plus entryId
#define TU_PROP_TYPE_PROGRESSIVE_PLUS_INITIAL_BUYIN_TRANSID 12 //PYR-123658 - bigIntProp: initial buyin transid

//TGMPROPS
#define	TGM_PROP_TYPE_ADDON_REBUY			0
#define TGM_PROP_TYPE_AAMS_DATA		1
#define TGM_PROP_TYPE_FLIGHT_INTERMEDIATE_PAYOUTS	2
#define TGM_PROP_TYPE_SNG_FLIGHT_NODE_RANGE		3


//TABTYPEPROPS
// OBSOLETED #define	PROP_TABTYPE_ACCESS								1  // Bug#: 3766
//#define PROP_TABTYPE_OBSCHAT							2  // Bug#: 3389
#define PROP_TABTYPE_RESTRICT_COUNTRY					3  // Bug#: 9411
#define PROP_TABTYPE_CAP								4  // Bug#: 14309
#define PROP_TABTYPE_RESTRICT							5  // Bug#: 16043 //PROP_TYPE_SINGLE_PLAYER_COUNTRY_RESTRICTION 32 //PYR-15506
#define PROP_TABTYPE_CLUB								6  // Home Games
#define PROP_TABTYPE_ICON								7	//PYR-16988
#define PROP_TABTYPE_FILTER								8	//PYR-16988
#define PROP_TABTYPE_RECORDING_SHOWDOWN					9	// #18024
#define PROP_TABTYPE_SERVINT							10	// #19917
#define PROP_TABTYPE_BLITZ_SHORTHANDED					11	//PYR-23733
#define PROP_TABTYPE_BLITZ_FLAGS						12	//PYR-23520
#define PROP_TABTYPE_BLITZ_OBSERVPARAM					13  //PYR-24004
#define PROP_TABTYPE_HAPPYHOURS							14	// PYR-23500
#define PROP_TABTYPE_MINCLIENTVERSION					15 //PYR-25260
#define PROP_TABTYPE_MINMOBILEVERSION					16 //PYR-25260
#define PROP_TABTYPE_MAXNONDEALINGTABLES				17 //PYR-25513
#define PROP_TABTYPE_SHOWNSECINSTOPPINGDEALINGTABLE		18 //PYR-25513
#define PROP_TABTYPE_CAMPING_TIMEOUT					19  //PYR-25229
#define PROP_TABTYPE_MAX_SITOUT_HANDS					20  //PYR-25229
#define PROP_TABTYPE_DEAL_CASHIER						21  //PYR-27091
#define PROP_TABTYPE_BLITZ_STICKYFOLD					22	//PYR-29088
#define PROP_TABTYPE_BLITZ_MAXINSTANCES					23	//PYR-23766
#define PROP_TABTYPE_NO_VPP_GAIN_SITES					24	//PYR-39371
#define PROP_TABTYPE_POOL_BLOCK							25 //PYR-42791
#define PROP_TABTYPE_SEAT_FINDER_DELAY					26 //PYR-43054
#define PROP_TABTYPE_SEAT_FINDER_MIN_PLAYERS			27 //PYR-43054
#define PROP_TABTYPE_SEAT_FINDER_OBSERV_PARAMS			28 //PYR-42280
#define PROP_TABTYPE_SEAT_FINDER_OBLIGATION_THRESHOLD	29 //PYR-48904
#define PROP_TABTYPE_SEAT_FINDER_OBLIGATION_TIMEOUT		30 //PYR-48904
#define PROP_TABTYPE_SEAT_FINDER_OBLIGATION_HANDS		31 //PYR-48904
#define PROP_TABTYPE_TIME_BANK_START					32 //PYR-54102
#define PROP_TABTYPE_TIME_BANK_TIME_INCREMENT			33 //PYR-54102
#define PROP_TABTYPE_TIME_BANK_HAND_INCREMENT			34 //PYR-54102
#define PROP_TABTYPE_TIME_BANK_MAX						35 //PYR-54102
#define PROP_TABTYPE_PREFLOP_TIME_BANK_START			36 //PYR-54102
#define PROP_TABTYPE_PREFLOP_TIME_BANK_TIME_INCREMENT	37 //PYR-54102
#define PROP_TABTYPE_PREFLOP_TIME_BANK_HAND_INCREMENT	38 //PYR-54102
#define PROP_TABTYPE_PREFLOP_TIME_BANK_MAX				39 //PYR-54102
#define PROP_TABTYPE_POSTFLOP_TIME_BANK_START			40 //PYR-54102
#define PROP_TABTYPE_POSTFLOP_TIME_BANK_TIME_INCREMENT	41 //PYR-54102
#define PROP_TABTYPE_POSTFLOP_TIME_BANK_HAND_INCREMENT	42 //PYR-54102
#define PROP_TABTYPE_POSTFLOP_TIME_BANK_MAX				43 //PYR-54102
#define PROP_TABTYPE_MORPH_LOBBY						44 //PYR-62595
#define PROP_TABTYPE_UNFOLD_BET_MULTIPLIER				45 //PYR-67163
#define PROP_TABTYPE_SEAT_FINDER_SEND_TO_AVG_STACK		46 //PYR-74194
#define PROP_TABTYPE_SEAT_FINDER_SEND_TO_PLAYERS_NUM	47 //PYR-74194
#define PROP_TABTYPE_SEAT_FINDER_SEND_TO_PLAYERS_NUM_VEC	48 //PYR-74194
#define PROP_TABTYPE_VARIABLE_ANTES						49 //PYR-83034
#define PROP_TABTYPE_SEAT_FINDER_EXCLUDE_SITTING_OUT_PLAYERS 50 // 	PYR-86050
#define PROP_TABTYPE_SEAT_FINDER_TABLE_MERGE			51 // 	PYR-84882
#define PROP_TABTYPE_SEAT_FINDER_MIN_MERGE_TIME			52 // 	PYR-84882
#define PROP_TABTYPE_CASHOUT_RAKE						53 // PYR-95091
#define PROP_TABTYPE_HYBRID_SINGLE						54 //PYR-95142
#define PROP_TABTYPE_MINS_TO_KEEP_STEALTH_ALIAS			55 //PYR-106118
#define PROP_TABTYPE_BLITZ_EARLY_BIRD					56 //PYR-112434

#define BLITZ_MAX_ONEPLAYER_INSTANCES_DEFAULT		4	//PYR-23766

//BLITZPROPS
#define	BLITZ_PROP_TYPE_ALT_RAKE			0 //PYR-29089


// PYR-42114
//CROSSGAMEBUTTONPROPS
#define	PROP_CROSSGAMEBUTTON_C2L_LINK		1 
#define	PROP_CROSSGAMEBUTTON_ICON			2
#define	PROP_CROSSGAMEBUTTON_LABEL			3
#define	PROP_CROSSGAMEBUTTON_TOOLTIP		4
#define	PROP_CROSSGAMEBUTTON_USER_CRITERIA	5
#define	PROP_CROSSGAMEBUTTON_GAME_CRITERIA	6
#define	PROP_CROSSGAMEBUTTON_TYPE			7 // PYR-45282


//// reasons for loopBack within cashout transaction
#define LB_REASON_REQUEST		"LR"	//user has requested loopback
#define LB_REASON_CHQ_MINIMUM	"QM"	//amount less than minimum for a cheque
#define LB_REASON_NT_CENTS		"NC"	//neteller does not accept cents
#define LB_REASON_NT_MINIMUM	"NM"	//amount less than minimum for neteller
#define LB_REASON_NT_BLOCKED	"NB"	//email is blocked for neteller cashouts
#define LB_REASON_FP_BLOCKED	"FB"	//email is blocked for firepay cashouts
#define LB_REASON_CK_BLOCKED	"KB"	//email is blocked for centralcoin cashouts
#define LB_REASON_OC_FAILED		"OF"	//overcredit request to gateway failed
#define LB_REASON_OC_UNKNOWN	"OU"	//overcredit request to gateway - status unknown
#define LB_REASON_ECHECK_MAX    "EM"    //cashout/overcredit request for ECHECK exceeds the max limit
#define LB_REASON_SOLO_MAX		"SM"    //cashout/overcredit request for SOLO exceeds the max limit
#define LB_REASON_MBOOK_MAX		"BM"    //cashout/overcredit request for MoneyBookers exceeds the max limit
#define LB_REASON_CNBUY_MAX		"CM"	//cashout/overcredit request for ClickAndBuy exceeds the max limit
#define LB_REASON_CASHOUT_MAX   "MM"	//cashout/overcredit request for exceeds the max limit for the paysystem (to be used in generalization of overcredits

#define reportManageTypeAdd			1
#define reportManageTypeDelete		2

#define TEXT_URL_SEPARATOR	'|'

enum TournCancelFormula
{
	formulaCancelledRollback		= 0,
	formulaCancelledRollforward		= 1,
	formulaCancelledAllInTheMoney	= 2,
	formulaCancelledRollSideways	= 3, // PYR-48391
};

#define TEMP_PWD_PREFIX	"tmp"

#define TLB_LIST_LENGTH_WEEKLY	30
#define TLB_LIST_LENGTH_WEEKLY_FR	150 /* #21988 */
#define TLB_LIST_LENGTH_MONTHLY	100
#define TLB_LIST_LENGTH_YEARLY	150
#define TLB_LIST_PORTION		200
//#define TLB_BOP_LIST_PORTION	100 // obsolete PYR-120713

#define maxEventsCountForYearDefault	100
#define maxEventsCountForMonthDefault	20
#define maxEventsCountForWeekDefault	10

// PYR-34930 - not used for a long time
//#define LOGGER_Q_SAVE 10001
//#define LOGGER_A_SAVE 20001

//status of WU transaction:
/* obsolete by PYR-45735, PYR-45748
#define STATUS_WU_UNKN	0
#define STATUS_WU_INFO	1
#define STATUS_WU_MTCN	2
#define STATUS_WU_SUCC	3
#define STATUS_WU_CANC	4
#define STATUS_WU_SENT_INFO		10
#define STATUS_WU_SENT_MTCN		11
#define STATUS_WU_SENT_PENDING	12
#define STATUS_WU_SENT_REJECTED	13
#define STATUS_WU_SENT_ACCEPTED	14 
*/

enum TournAwardCurrencyType
{
	eTypeMoney = 0,
	eTypePlayMoney = 1,
	eTypeFpp = 2
};

enum ePlayerType
{
	dotComReal = 0,
	dotNetRestricted = 1,
	dotComPlay = 2,
	dotNetUnrestricted = 3,
	dotItaly = 4,
	dotFrance = 5,
	dotBelgium = 6,
	playerTypeHWM = 7 //NB !!! change this value when adding more types!!!
};

enum uruVerificationStatus
{
	statusVerified			= 0,
	statusMisMatch			= 1,
    statusNetworkFailure	= 2,
    statusIgnored			= 3,
	statusNamePick			= 4,
	statusAddressPick		= 5,
	statusCriticalFraud		= 100
};
/*
// Account states in GCDEBITDATA table
#define GCDEBIT_STATE_INIT			0
#define GCDEBIT_STATE_OPENED		'O'
#define GCDEBIT_STATE_QUERIED		'Q' // transient, inside dbm
#define GCDEBIT_STATE_SENT    		'E' // not yet processed
#define GCDEBIT_STATE_DEPOSITED		'D'
#define GCDEBIT_STATE_FIRST_FAULT	'1'
#define GCDEBIT_STATE_SECOND_FAULT	'2'
#define GCDEBIT_STATE_SUSPENDED   	'S'
#define GCDEBIT_STATE_READY       	'R'
#define GCDEBIT_STATE_ACTIVE      	'A'
#define GCDEBIT_STATE_CLOSED      	'X'
*/
// user passes URU check with the score higher than threshold
#define URU_AGE_VERIFIED_THRESHOLD 			999

#define	eWireInfoStarted	0	//info requested by user
#define	eWireInfoOk			1	//info given to us by GC
#define	eWireInfoNok		2	//info was not given by GC, error
#define	eWireCompleted		3	//wire transfer completed
#define	eWireRejected		4	//wire transfer not completed
#define eWireDiscarded		99  //user requested a new info
#define	eWireInfoUnknown	100 //?


#define BLOCKING_LEVEL_NONE			0
#define BLOCKING_LEVEL_CASHIN		(1     ) // 1	
#define BLOCKING_LEVEL_RM			(1 << 1) // 2 
#define BLOCKING_LEVEL_PM			(1 << 2) // 4
#define BLOCKING_LEVEL_TRANSFER_IN	(1 << 3) // 8
#define BLOCKING_LEVEL_STATE_CHANGE	(1 << 4) // 16
#define BLOCKING_LEVEL_REGISTER		(1 << 5) // 32
#define BLOCKING_LEVEL_RM_TABLES	(1 << 6) // 64
#define BLOCKING_LEVEL_RM_TOURNS	(1 << 7) // 128
#define BLOCKING_LEVEL_TRANSFER_OUT	(1 << 8) // 256
#define BLOCKING_LEVEL_INTERSTATE_TRANSFER_IN  (1 <<  9) //  512
#define BLOCKING_LEVEL_INTERSTATE_TRANSFER_OUT (1 << 10) // 1024
#define BLOCKING_LEVEL_TSALE		(1 << 11) // 2048
#define BLOCKING_LEVEL_CASINO_ALL_RM	(1 << 12) // 4096
#define BLOCKING_LEVEL_CASINO_ALL_PM	(1 << 13) // 8192
#define BLOCKING_LEVEL_CASINO_BLACKJACK	(1 << 14) // 16384
#define BLOCKING_LEVEL_CASINO_ROULETTE	(1 << 15) // 32768
#define BLOCKING_LEVEL_CASINO_SLOTS 	(1 << 16) // 65536
#define BLOCKING_LEVEL_SPORTSBOOK		(1 << 17) // 131072  PYR-33233 (blocking2 feature)
#define BLOCKING_LEVEL_DFSPORTS_US		(1 << 18) // 262144  PYR-36467                                  //DFS_US!
#define BLOCKING_LEVEL_POOLBETTING		(1 << 19) // 524288  PYR-42410 (blocking2 feature)
#define BLOCKING_LEVEL_DFSPORTS_INTL	(1 << 20) // 1048576 PYR-48971                                  //DFS_INTL!
#define BLOCKING_LEVEL_PREDICTOR		(1 << 21) // 2097152 PYR-64639


#define BLOCKING_LEVEL_FORCE_CASHOUT	(ONE64 << 31)

// these constant exponents are to be used in [COUNTRY_LICENSE_RULES] section in payments.ini
#define COUNTRY_LICENSE_RULE_MIGRATION_IS_OPEN			(ONE64     )
//#define COUNTRY_LICENSE_RULE_......					(ONE64 << 1)

// PYR-45869
#define ADMININTID_ZERO			0
#define ZERO_APP_LOGINID        ((UINT64)0) // PYR-42053

#define ONE_THOUSAND			1000
#define ONE_MILLION				1000000
#define ONE_BILLION				1000000000
#define CONV_RATE_BASE_1M		1000000		// one million - was a conv rate base before #9329
#define CONV_RATE_BASE			100000000	// 100 million - conv rate base after #9329
#define MARGIN_MULTIPLIER		1000000
#define CASHTRANS_PM_MULTIPLIER ((INT64)1000000) // PYR-49042 1M PM multiplier for CASHTRANSPM.bonusFpp (playMoneyPurchase) applied when flag CASHTRANS_FLAG2_PM_AMOUNT_OVER_2B is set

#define TSALE_STATUS_CREATED	0
#define TSALE_STATUS_COMPLETE	1
#define TSALE_STATUS_EXPIRED	2
#define TSALE_STATUS_CANCELLED	3

///constants for "ActionTaken" field in AchCBacks table
#define CB_ACTION_NONE				0
/* obsolete
#define CB_ACTION_FROZEN			1
#define CB_ACTION_FROZEN_AHEAD		2
#define CB_ACTION_FROZEN_CONFIRMED	3
*/
//# RESERVED and NOT USED           (1 << 0) 
//# RESERVED and NOT USED           (1 << 1) // 2
#define CB_ACTION_BLOCK_ACCOUNT		(1 << 2) // 4

#define CB_ACTION_FREEZE_USER		(1 << 3) // 8
#define CB_ACTION_BAN_USER			(1 << 4) // 16
#define CB_ACTION_SET_LIMIT			(1 << 5) // 32
#define CB_ACTION_DEBIT_USER		(1 << 6) // 64

#define CB_ACTION_SEND_NOTIFICATION			(1 << 7) // 128
/*!obsolete!*/#define CB_ACTION_SET_UNCLEARED				(1 << 8) // 256
#define CB_ACTION_ADD_COMMENT				(1 << 9) // 512
#define CB_ACTION_DELETE_CARD_INFO			(1 << 10)// 1024
#define CB_ACTION_CREDIT_USER				(1 << 11)// 2048
#define CB_ACTION_CANCEL_CASHOUTS			(1 << 12)// 4096
#define CB_ACTION_CANCEL_TRANSFERS			(1 << 13)// 8192
#define CB_ACTION_SET_CANT_SPEND_UNCLEARED	(1 << 14)// 16384
#define CB_ACTION_SET_UNCLEARED_STATUS		(1 << 15)// 32768
#define CB_ACTION_SET_NO_PLAY				(1 << 16) //  PYR-14416
#define CB_ACTION_SET_NO_TRANSFER_OUT		(1 << 17)
#define CB_ACTION_SET_SAFE_MODE				(1 << 18)

#define ACH_RETURN_CSR_NEW			0
#define ACH_RETURN_CSR_REVIEWED		1 // not used
//#define ACH_RETURN_CSR_FROZEN		2
#define ACH_RETURN_CSR_CLEARED		3
#define ACH_RETURN_FALSE			4 //return was later declared false
//#define ACH_RETURN_CSR_CLEARED_AUTO	4

// values for SELFEXCLUDED.STATUS
#define SELF_EXCLUSION_STATUS_ACTIVE			0
#define SELF_EXCLUSION_STATUS_EXPIRED			1
#define SELF_EXCLUSION_STATUS_OVERRIDEN			2
#define SELF_EXCLUSION_STATUS_CANCELLED_ADMIN	3
#define SELF_EXCLUSION_STATUS_SET_BY_ADMIN		4
#define SELF_EXCLUSION_STATUS_CANCELLED_FR_MIGRATION	5
#define SELF_EXCLUSION_STATUS_CANCELLED_OVERRIDEN     	6

// PYR-13619 : possible expired action to take after the self exclusion period ended
// - or -
// PYR-15236: reasons why the self exclusion was in place.
enum SelfExclusionExpiredActions
{
	selfExclusionExpiredAction_NoAction				= 0,
	selfExclusionExpiredAction_SetNoMailList		= 1 << 0,
	selfExclusionExpiredAction_EstonianRegulations	= 1 << 1,
	selfExclusionExpiredAction_UnderAge				= 1 << 2
	,selfExclusionExpiredAction_SetNoMailListPoker	= 1 << 3 //8 PYR-41471
	,selfExclusionExpiredAction_SetNoMailListCasino	= 1 << 4 //16 PYR-41471
	,selfExclusionExpiredAction_SetNoMailListSports	= 1 << 5 //32 PYR-41471
};

#define REAL_HAND_DENOMINATOR		100000
// PYR-27114 - separate hand denominator for non-Com sites, since those sites have a lot less hands than .COM
#define REAL_HAND_DENOMINATOR_NONCOM 10000

//representments statuses
#define	REPRS_STATUS_CREATED		0 //there was a representable return and we decided when we want to represent
#define REPRS_STATUS_BALANCE_OK		1 //at the tome of sending either 1-st or 2-nd representment or after receiving return to 1-st, balance is positive
#define REPRS_STATUS_ADMIN_CLOSE	3 //set by admin only
#define REPRS_STATUS_CLOSED_MAX_RETRY		4 //max retries has been reached representment #13559
#define REPRS_STATUS_CLOSED_CARD_BLOCKED	5 //the card is blocked #13559
#define REPRS_STATUS_CLOSED_ERR_CODE		6 //closed because of a special errcode see cashier.ini [REPRESENTMENT_REJECT] eReprRejectClose #13559

#define REPRS_STATUS_FIRST_SENT		101 //first reprsentment was sent to gateway
#define REPRS_STATUS_FIRST_OK		102 //first reprsentment was sent to gateway and approved
#define REPRS_STATUS_FIRST_FAILED	103 //first reprsentment was sent to gateway and rejected
#define REPRS_STATUS_FIRST_RETURN	104 //first reprsentment was sent to gateway and approved but there was a return later
#define REPRS_STATUS_FIRST_FAILED_SPECIAL 105 //first reprsentment was sent to gateway and rejected with a special error code (deferred for manual processing) #13559

#define	REPRS_STATUS_SECOND_CREATED	200 //there was a representable return on the 1st representment
#define REPRS_STATUS_SECOND_SENT	201 //second reprsentment was sent to gateway
#define REPRS_STATUS_SECOND_OK		202 //second reprsentment was sent to gateway and approved
#define REPRS_STATUS_SECOND_FAILED	203 //second reprsentment was sent to gateway and rejected
#define REPRS_STATUS_SECOND_RETURN	204 //second reprsentment was sent to gateway and approved but there was a return later
#define REPRS_STATUS_SECOND_FAILED_SPECIAL 205 //second reprsentment was sent to gateway and rejected with a special error code (deferred for manual processing) #13559

//representments actions
#define R_ACT_NONE		   (-1)
#define R_ACT_CLOSE1		0
#define R_ACT_CLOSE2		1
#define R_ACT_ADD_FIRST		2
#define R_ACT_ADD_SECOND	3

// representment indication
#define REPRESENTMENT_1_STR	"_R"
#define REPRESENTMENT_2_STR	"_r"

// representment special control processing
enum eReprRejectControl // #13559 see cashier.ini [REPRESENTMENT_REJECT]
{
	eReprRejectNormal	 = 0,
	eReprRejectClose	 = 1,
	eReprRejectSpecial	 = 2,
};

//
// CASH-DBM Project: message dictionary
//

// New format headers (0)
#define DBM_QM_HEAD       						110000 // Question-to-Main
#define DBM_MA_HEAD       						120000 // Main-Answer, stage 3
#define DBM_MC_HEAD          					130000 // Main-to-Cash, stage 1
#define DBM_CM_HEAD        						140000 // Cash-to-Main, stage 2
#define DBM_EC_HEAD        						150000 // Echo-to-Cash, stage 4
#define DBM_PC_HEAD        						160000 // Pingpong-to-Cash
#define DBM_CP_HEAD        						170000 // from-Cash-Pingpong
#define DBM_XX_HEAD        						180000 // maX

// NAK/ABORT frame: generated and handled only in VM (1)
#define DBM_QM_NAK      						(DBM_QM_HEAD + 1) // never sent
#define DBM_MA_NAK	     						(DBM_MA_HEAD + 1) // framed error reply to client
#define DBM_MC_ABORT	  						(DBM_MC_HEAD + 1) // never sent
#define DBM_CM_ABORT   							(DBM_CM_HEAD + 1) // abort frame from cash DBM, error reply frame will follow
#define DBM_EC_ABORT   							(DBM_EC_HEAD + 1) // abort frame from main DBM, error reply frame will follow

// Control frames (2-19)
#define DBM_QM_POST_TO_OLAP   					(DBM_QM_HEAD + 2)
#define DBM_MA_POST_TO_OLAP   					(DBM_MA_HEAD + 2)
#define DBM_MC_POST_TO_OLAP   					(DBM_MC_HEAD + 2)
#define DBM_CM_POST_TO_OLAP   					(DBM_CM_HEAD + 2)

// Business logic frames
#define DBM_QM_GET_USER_INFO 					(DBM_QM_HEAD + 20)
#define DBM_MA_GET_USER_INFO 					(DBM_MA_HEAD + 20)
#define DBM_MC_GET_USER_INFO 					(DBM_MC_HEAD + 20)
#define DBM_CM_GET_USER_INFO 					(DBM_CM_HEAD + 20)

#define DBM_QM_GET_USER_BONUS					(DBM_QM_HEAD + 21)
#define DBM_MA_GET_USER_BONUS					(DBM_MA_HEAD + 21)
#define DBM_MC_GET_USER_BONUS					(DBM_MC_HEAD + 21)
#define DBM_CM_GET_USER_BONUS					(DBM_CM_HEAD + 21)

#define DBM_QM_GET_USER_CASHINS   				(DBM_QM_HEAD + 22)
#define DBM_MA_GET_USER_CASHINS					(DBM_MA_HEAD + 22)
#define DBM_MC_GET_USER_CASHINS					(DBM_MC_HEAD + 22)
#define DBM_CM_GET_USER_CASHINS					(DBM_CM_HEAD + 22)

#define DBM_QM_UPDATE_USER_BANKROLL				(DBM_QM_HEAD + 23)
#define DBM_MA_UPDATE_USER_BANKROLL				(DBM_MA_HEAD + 23)
#define DBM_MC_UPDATE_USER_BANKROLL				(DBM_MC_HEAD + 23)
#define DBM_CM_UPDATE_USER_BANKROLL				(DBM_CM_HEAD + 23)

#define DBM_QM_MARK_USER_BANNED   				(DBM_QM_HEAD + 24)
#define DBM_MA_MARK_USER_BANNED   				(DBM_MA_HEAD + 24)
#define DBM_MC_MARK_USER_BANNED   				(DBM_MC_HEAD + 24)
#define DBM_CM_MARK_USER_BANNED   				(DBM_CM_HEAD + 24)

#define DBM_QM_SAVE_SECURITY_EVENTS       		(DBM_QM_HEAD + 25)
#define DBM_MA_SAVE_SECURITY_EVENTS       		(DBM_MA_HEAD + 25)
#define DBM_MC_SAVE_SECURITY_EVENTS       		(DBM_MC_HEAD + 25)
#define DBM_CM_SAVE_SECURITY_EVENTS       		(DBM_CM_HEAD + 25)

#define DBM_QM_GET_SESSION_INFO             	(DBM_QM_HEAD + 26)
#define DBM_MA_GET_SESSION_INFO                	(DBM_MA_HEAD + 26)
#define DBM_MC_GET_SESSION_INFO                	(DBM_MC_HEAD + 26)
#define DBM_CM_GET_SESSION_INFO                	(DBM_CM_HEAD + 26)

#define DBM_QM_COMPOSE_WHOLE_PURCHASE_TRANS		(DBM_QM_HEAD + 27)
#define DBM_MA_COMPOSE_WHOLE_PURCHASE_TRANS		(DBM_MA_HEAD + 27)
#define DBM_MC_COMPOSE_WHOLE_PURCHASE_TRANS		(DBM_MC_HEAD + 27)
#define DBM_CM_COMPOSE_WHOLE_PURCHASE_TRANS		(DBM_CM_HEAD + 27)

#define DBM_QM_GET_USER_CHIPSINFO				(DBM_QM_HEAD + 28)
#define DBM_MA_GET_USER_CHIPSINFO				(DBM_MA_HEAD + 28)
#define DBM_MC_GET_USER_CHIPSINFO				(DBM_MC_HEAD + 28)
#define DBM_CM_GET_USER_CHIPSINFO				(DBM_CM_HEAD + 28)

#define DBM_QM_LOG_SUCCESSFUL_DEPOSIT			(DBM_QM_HEAD + 29)
#define DBM_MA_LOG_SUCCESSFUL_DEPOSIT			(DBM_MA_HEAD + 29)
#define DBM_MC_LOG_SUCCESSFUL_DEPOSIT			(DBM_MC_HEAD + 29)
#define DBM_CM_LOG_SUCCESSFUL_DEPOSIT			(DBM_CM_HEAD + 29)

//#define DBM_QM_SET_AUDIT_ID64        			(DBM_QM_HEAD + 30)
//#define DBM_MA_SET_AUDIT_ID64        			(DBM_MA_HEAD + 30)
//#define DBM_MC_SET_AUDIT_ID64        			(DBM_MC_HEAD + 30)
//#define DBM_CM_SET_AUDIT_ID64        			(DBM_CM_HEAD + 30)
#define DBM_EC_SET_AUDIT_ID64        			(DBM_EC_HEAD + 30)

#define DBM_QM_GET_BONUS_ARRAY        			(DBM_QM_HEAD + 31)
//#define DBM_MA_GET_BONUS_ARRAY        			(DBM_MA_HEAD + 31)
#define DBM_MC_GET_BONUS_ARRAY        			(DBM_MC_HEAD + 31)
//#define DBM_CM_GET_BONUS_ARRAY        			(DBM_CM_HEAD + 31)

#define DBM_QM_CREATE_USERBONUS        			(DBM_QM_HEAD + 32)
#define DBM_MA_CREATE_USERBONUS        			(DBM_MA_HEAD + 32)
#define DBM_MC_CREATE_USERBONUS        			(DBM_MC_HEAD + 32)
#define DBM_CM_CREATE_USERBONUS        			(DBM_CM_HEAD + 32)

//#define DBM_QM_MINUTES_SINCE_REGISTERED 		(DBM_QM_HEAD + 33)
//#define DBM_MA_MINUTES_SINCE REGISTERED    		(DBM_MA_HEAD + 33)
//#define DBM_MC_MINUTES_SINCE_REGISTERED       	(DBM_MC_HEAD + 33)
//#define DBM_CM_MINUTES_SINCE_REGISTERED        	(DBM_CM_HEAD + 33)

//#define DBM_QM_CARDS_AND_USERS_CHAIN 			(DBM_QM_HEAD + 34)
//#define DBM_MA_CARDS_AND_USERS_CHAIN 			(DBM_MA_HEAD + 34)
#define DBM_MC_CARDS_AND_USERS_CHAIN       		(DBM_MC_HEAD + 34)
#define DBM_CM_CARDS_AND_USERS_CHAIN        	(DBM_CM_HEAD + 34)
//#define DBM_EC_CARDS_AND_USERS_CHAIN        	(DBM_EC_HEAD + 34)
#define DBM_PC_CARDS_AND_USERS_CHAIN        	(DBM_PC_HEAD + 34)
#define DBM_CP_CARDS_AND_USERS_CHAIN        	(DBM_CP_HEAD + 34)

#define DBM_QM_UPDATE_USER_PHONE				(DBM_QM_HEAD + 35)
#define DBM_MA_UPDATE_USER_PHONE				(DBM_MA_HEAD + 35)
#define DBM_MC_UPDATE_USER_PHONE				(DBM_MC_HEAD + 35)
#define DBM_CM_UPDATE_USER_PHONE				(DBM_CM_HEAD + 35)

#define AGE_PROCESSOR_TYPE_ARISTOTEL	0
#define AGE_PROCESSOR_TYPE_URU			1
#define AGE_PROCESSOR_TYPE_192			2

#define CACHED_INTIDS_TYPE_ALL_USERS	0
#define CACHED_INTIDS_TYPE_LOGGED_IN	1

// CASHTRANSPROPS.PROPTYPE(s)
enum eCashtransPropType
{
	CASHTRANS_PROP_TYPE_DESCRIPTOR			= 0, //dynamic descriptor, as returned by gateway
	CASHTRANS_PROP_TYPE_DESCRIPTOR_MOD		= 1, //corrected descriptor, updated by admin's request #12043
	CASHTRANS_PROP_TYPE_REC2				= 2, //REC2, #8643
	CASHTRANS_PROP_TYPE_BEFORE_ACHBATCH		= 3, // #17160
	CASHTRANS_PROP_TYPE_PAYPAL_PAYERID		= 4, // #19197 paypal unique userid
	CASHTRANS_PROP_TYPE_YANDEX_CASHOUT_TIME = 5, 
	CASHTRANS_PROP_TYPE_MIGRATIONID         = 6, // #22434 [ES] migrationId
// obsolete FullTilt props
	CASHTRANS_PROP_TYPE_FTP_TRANSID         = 7, // #24069 FullTilt transId
	CASHTRANS_PROP_TYPE_FTP_IPADDR          = 8, // #24069 FullTilt ipAddr
	CASHTRANS_PROP_TYPE_FTP_LT_POINTS       = 9, // #24069 FullTilt lifetime points
	CASHTRANS_PROP_TYPE_PAYPAL_EMAIL	    =10, // PYR-25884
	CASHTRANS_PROP_TYPE_AWARDID			    =11, // PYR-24840 For parent purchase. int - AWARDS.awardId, str - USERAWARDS.userAwardId
	CASHTRANS_PROP_TYPE_USERAWARDID		    =12, // PYR-24840 For ps='AWARD'.      int - USERAWARDS.userAwardId, str - AWARDS.awardId
	CASHTRANS_PROP_TYPE_BUDGETCODE		    =13, // PYR-22519 int - BudgetCodeId, str - AdminId(if known)
	CASHTRANS_PROP_TYPE_SUGGESTED_DEPOSIT   =14, // PYR-27356 For parent purchase. str - suggested deposit description
	CASHTRANS_PROP_TYPE_FREEMIUM_2		    =15, // PYR-29627 str:itemName, int:PYRFIN.FREEMIUMRECEIPT.RECEIPTID
	CASHTRANS_PROP_TYPE_ELV_SEPA_PROFILE    =16, // PYR-30281 str:profileId, int:0
	CASHTRANS_PROP_TYPE_PAYLUCKY_LOAD       =17, // PYR-31414 str:mastercardMaskedCardNum (not token), int:loadedAmount
	CASHTRANS_PROP_TYPE_SIGHTLINE2_LOAD_SRC =19, // PYR-41694 from VISA str= GW:xTransStr, int:loadedAmount
	CASHTRANS_PROP_TYPE_SIGHTLINE2_LOAD_DST =20, // PYR-41694 to SIGHTLINE str= xTransStr, int:loadedAmount
	CASHTRANS_PROP_TYPE_APPROVE_HOLD_TRANSTYPE =21, // PYR-42009 int - transType
	CASHTRANS_PROP_TYPE_CHARGEBACKED        =22, // PYR-29824 str - chargeback reason, int - CBACKS[PM].cbackId // as of PYR-29824 implemented only for PM
	CASHTRANS_PROP_TYPE_REVERSED            =23, // PYR-28404 int - old(reversed) transfer.transId
	CASHTRANS_PROP_TYPE_CASHOUT_TAXABLE     =24, // PYR-30309 int - .IN taxable amount = 'used portion' of cashout; PYR-69310 .RO RON equiv of REQUESTED cashout
	CASHTRANS_PROP_TYPE_CASHOUT_2_TAX_LINK  =25, // PYR-30309 + PYR-69310 int - IN & RO tax cashtrans.transId
	CASHTRANS_PROP_TYPE_TAX_2_CASHOUT_LINK  =26, // PYR-30309 + PYR-69310 int - IN & RO parent cashout transId
	CASHTRANS_PROP_TYPE_CASHOUT_REFUND      =27, // PYR-30309 int - refundedAmount, str - refund TransactsId; bound to parentCashout
	CASHTRANS_PROP_TYPE_ADMIN_INIT          =28, // int - adminIntId, str - adminId (not always set yet; for future population in a dedicated task)
	CASHTRANS_PROP_TYPE_IND_ANNUAL_TAX_LINK =29, // PYR-54304 int - .IN taxPaid cashtrans.transId
	CASHTRANS_PROP_TYPE_CASHOUT_2_TAX_REFUND_LINK = 30, // PYR-71242 + PYR-69310 int - IN only (not RO after PYR-84764) tax refund-deposit cashtrans.transId
	CASHTRANS_PROP_TYPE_TAX_REFUND_2_CASHOUT_LINK = 31, // PYR-71242 + PYR-69310 int - IN & RO parent cashout transId
	CASHTRANS_PROP_TYPE_OVERFLOW_LINK       =32, // PYR-77123, PYR-85287 int: orig_TransactId, str: eCashOverflowProtected_TransactId
	CASHTRANS_PROP_TYPE_ORIG_TAXABLE_TRANSACTION = 33, //  PYR-77762 int: USNJ settle/unsettle/cashout Transacts.TransId
	CASHTRANS_PROP_TYPE_TAX_USER_2_RECEIVER_LINK = 34, //  PYR-77762 int: USNJ receiver cashtrans.transId
	CASHTRANS_PROP_TYPE_TAX_RECEIVER_2_USER_LINK = 35, //  PYR-77762 int: USNJ taxpayer cashtrans.transId
	CASHTRANS_PROP_TYPE_TAX_REFUND         = 36, // PYR-84764 int - refundedTaxAmount; bound to TAX withdrawal cashtrans; incrementable
	CASHTRANS_PROP_TYPE_MERCHANT_PREFERENCE_3DS  = 37, //  PYR-95169 str: one of MERCHANT_PREFERENCE_ strings from srvshared/shared3ds.h
};

#define DAYS_IN_A_YEAR		365
#define DAYS_IN_MONTH		30
#define DAYS_IN_WEEK		7
#define DAYS_IN_DAY			1
#define HOURS_IN_A_DAY		24
#define MINUTES_IN_AN_HOUR	60
#define MINUTES_IN_A_DAY	1440
#define SECONDS_IN_A_DAY	86400
#define SECONDS_IN_AN_HOUR	3600
#define SECONDS_IN_A_MINUTE	60
#define MILLISECS_IN_A_MINUTE	(SECONDS_IN_A_MINUTE * 1000)
#define MILLISECS_IN_AN_HOUR	(MILLISECS_IN_A_MINUTE * MINUTES_IN_AN_HOUR)
#define MILLISECS_IN_A_DAY		(MILLISECS_IN_AN_HOUR * HOURS_IN_A_DAY)

#define INDETERMINATE_SELFEXCLUSION_DAYS	(DAYS_IN_A_YEAR * 100) // PYR-84829

#define MAXNUMSPECS	4
#define PROPERTIES_ECHECK_GW_PREFIX				ECHECK_NAME
#define PROPERTIES_CCARD_PREFIX					"CCARD"

#define CB_CONTROL_DONOT_CLOSE_ACCOUNT	1
#define CB_CONTROL_DONOT_BLOCK_CARD		(1 << 1) // 2

//#9866 - admin control flags for DBM_Q_SAVE_APPROVED_PURCHASE from admin
#define APPROVED_PURCHASE_BYPASS_BALANCE_CHECK		1
#define APPROVED_PURCHASE_CHECK_BALANCE_CURRENCY	(1 << 1) //2
#define APPROVED_PURCHASE_CHECK_BANNED				(1 << 2) //4
#define APPROVED_PURCHASE_CHECK_SELFEXCLUDED		(1 << 3) //8
#define APPROVED_PURCHASE_CHECK_ACCT_CLOSED			(1 << 4) //16
#define APPROVED_PURCHASE_CHECK_BANNED_BY_REGULATOR	(1 << 5) //32

enum eApprovedPurchaseControlFlagsEx
{
	eApprovedPurchaseControlFlag_CreateParent = (1 << 0),
};

#define TABLE_LIMIT_ANY		0
#define TABLE_LIMIT_FIXED	1
#define TABLE_LIMIT_NL_PL	2

enum eChipsType 
{ 
	eWChips = 0,
	eTChips = 1,
	eChips = 2,
	eOwedChips = 3,
};

//#12043
#define GATEWAY_DESCRIPTOR_INVALID	1 //this descriptor should raise an alert if no GATEWAY_DESCRIPTOR_REPORTED flag is set
#define GATEWAY_DESCRIPTOR_REPORTED	(1 << 1) //we've notified business about this descriptor once

//constants for CONSENTRULES table
#define CONSENT_ID_POKERSTRATEGY			1 //CONSENTRULES.CONSENTID
#define CONSENT_ID_POKERSTRATEGY2			2	//PYR-33689

#define RULE_TYPE_INFO_SHARING_CONSENT		1 //CONSENTRULES.RULETYPE
#define RULE_TYPE_PROMO_DEPOSIT_ELIGIBLE	2

// PYR-14428 Admin to DBM

enum eFrAccountValidationFieldId
{
	eFracFID_CREATERECORD	= 0,
	eFracFID_DOCSREQUESTED  = 1,
	eFracFID_DOCSVALIDATED  = 2,
	eFracFID_REMAINDERSENT  = 3,
	eFracFID_AVC            = 4,
	eFracFID_AVCREQUESTED   = 5,
	eFracFID_AVCVALIDATED   = 6,
	eFracFID_AVCSENT        = 7,
	eFracFID_AVCREGENERATED = 8,
	eFracFID_ACCDEACTIVATED = 9,
	eFracFID_ACCCLOSED      = 10,
	eFracFID_ACTIONMASK     = 11,
	eFracFID_PDCREQUESTED   = 12, // PYR-43398
};

enum eFrDocsValidatedAction
{
	eFracDVAC_validated		= 1,
	eFracDVAC_inValidated	= 2,
};

enum eBeAccountVerificationFieldId
{
	eBeAcFID_CREATERECORD	=  0,
	eBeAcFID_STARTDATE		=  1,
	eBeAcFID_SNAPDEPOSITS	=  2,
	eBeAcFID_SNAPCASHOUTS	=  3,
	eBeAcFID_SNAPDAYS		=  4,
	eBeAcFID_SNAPTIME		=  5,
	eBeAcFID_AVC			=  6,
	eBeAcFID_AVCREQUESTED	=  7,
	eBeAcFID_AVCVALIDATED	=  8,
	eBeAcFID_AVCSENT		=  9,
	eBeAcFID_AVCREGENERATED	= 10,
	eBeAcFID_TRIGGERPOINT	= 11,
	eBeAcFID_ACCSUSPENDED	= 12,
	eBeAcFID_ACTIONMASK		= 13,
};

// PYR-18621
enum eItAccountVerificationFieldId
{
	eItacFID_CREATERECORD	= 0,
	eItacFID_DOCSREQUESTED  = 1,
	eItacFID_DOCSVALIDATED  = 2,
	eItacFID_REMAINDERSENT  = 3,
	eItacFID_ACCDEACTIVATED = 4,
	eItacFID_ACCCLOSED      = 5,
	eItacFID_ACTIONMASK     = 6,
};

// PYR-20730
enum eAccountVerificationFieldId
{
	eAcFID_CREATERECORD	  = 0,
	eAcFID_DOCSREQUESTED  = 1,
	eAcFID_DOCSVALIDATED  = 2,
	eAcFID_ACCDEACTIVATED = 4,
	eAcFID_ACCCLOSED      = 5,
	eAcFID_ACTIONMASK     = 6,
};

enum eItDocsValidatedAction
{
	eItacDVAC_validated		= 1,
	eItacDVAC_inValidated	= 2,
};

// PYR-20730 & PYR-16740
enum eDocsValidatedAction
{
	eAcDVAC_validated	= 1,
	eAcDVAC_inValidated	= 2,
};

// PYR-14700
enum eTournTicketNeedCleanUp
{
	eTTNC_CleanupNotNeeded = 0,
	eTTNC_NeedCleanUp = 1,
	eTTNC_CleanUpInProgress = 2,
	eTTNC_CleanUpCompleted = 3,
};


enum eClientNotificationCode // #15990
{
	eClientNotificationCodeInfo = 0,
	eClientNotificationCodeWarning = 1,
	eClientNotificationCodeError = 2,
	eClientNotificationCode1stDepositInfo = 3, // PYR-30476
};

// PYR-16398 for USEREVENTS table
enum eUserStatEventType
{
    // #17874 - went to negative not by a specific reason (credit card, echeck, ELV)
    eUserStatEvent_balanceNegative  = 1,
	//#17321
	eUserStatEvent_aff_firstClubSignup = 2,
	eUserStatEvent_aff_firstCreatedHg = 3,
	eUserStatEvent_aff_firstAffClubSignup = 4,
	eUserStatEvent_aff_firstHgRm = 5,
	eUserStatEvent_aff_firstOwnHgRm = 6,
	eUserStatEvent_aff_firstAffHgRm = 7,
    eUserStatEvent_negativeByCreditCard = 8,
    eUserStatEvent_negativeByEcheck     = 9,
    eUserStatEvent_negativeByELV        = 10,
};

// #PYR-18100 for PYRFIN DBTACCOUNTS and OLAP
enum eDbtWireBankDetailValidation
{
    eDbtWireBankValidationOK = 0,
    eDbtWireBankValidationWasNotValidated = 1,
    eDbtWireBankValidationServiceWasUnavailable = 2
};

// PYR-69729 moved from WireAccountInfo::eStatus in shared.h
enum eWireAccountStatus
{
	eWireAccountStatus_Created = 0,
	eWireAccountStatus_Invalidated = 1
};

// PYR-18024
enum ReplayStatus
{
	replayStatusActive = 0,
	replayStatusNonActive = 1
};

enum eRepeatedCashinDeclineOption // #17995
{
	 eRepeatedCashinDeclineOptionOpenCashier = (1     ),
	 eRepeatedCashinDeclineOptionLiveSupport = (1 << 1),
	 eRepeatedCashinDeclineOptionWebSuggest  = (1 << 2),
	 eRepeatedCashinDeclineOptionDepositNow  = (1 << 3),
};

// PYR-18694
enum SMSValidationStatus
{
	eSMSStatusPending = 0,
	eSMSStatusSent = 1,
	eSMSStatusValidated = 2,
	eSMSStatusTooManyAttemts = 3,
	eSMSExpired = 4,
// DO NOT MODIFY BECAUSE THEY ARE STORED IN DATABASE
};

// PYR-18694
enum SMSReasonCode
{
	eSMSReasonInitialValidation = 1,
	eSMSReasonChangeNumber = 2,
	eSMSReasonLostRSAToken = 3,
	eSMSReasonBelgianVerification = 4,
	eSMSReasonCashoutConfirm = 5,
	eSMSReasonNewInstallId = 6, // PYR-21829
// DO NOT MODIFY BECAUSE THEY ARE STORED IN DATABASE
};

// #19474
enum FlightTournFlags
{
	eFlightTournIsNonFinal		   = 0x1,
	eFlightTournIsTarget		   = 0x2,
	eFlightTournIsNonStartedTarget = 0x4,
};

enum eUpdateRealChipsRequestFlags // PYR-26251 - request-specific flags
{
	eUpdateRealChipsRequest_Addon				= ( 1 << 0 ),  // 0x1, // 1 - addon, 0 - rebuy
	eUpdateRealChipsRequest_Blitz				= ( 1 << 1 ),  // 0x2,
	//eUpdateRealChipsRequest_ExclTicket			= ( 1 << 2 ),  // 0x4, //PYR-46488 Deprecated
	eUpdateRealChipsRequest_DkRegistered		= ( 1 << 3 ),  // 0x8,
	eUpdateRealChipsRequest_AutoAdd				= ( 1 << 4 ),  // 0x10,
	eUpdateRealChipsRequest_FirstTime			= ( 1 << 5 ),  // 0x20,
	eUpdateRealChipsRequest_AllInTheMoney		= ( 1 << 6 ),  // 0x40,
	eUpdateRealChipsRequest_Rollback			= ( 1 << 7 ),  // 0x80,
	eUpdateRealChipsRequest_FinalBatch			= ( 1 << 8 ),  // 0x100 // PYR-28503
	eUpdateRealChipsRequest_MultiDbm			= ( 1 << 9 ),  // 0x200 // PYR-28728
	eUpdateRealChipsRequest_Dummy				= ( 1 << 10 ), // 0x400 // PYR-30164
	eUpdateRealChipsRequest_AlreadyEliminated	= ( 1 << 11 ), // 0x800 // PYR-30436
	eUpdateRealChipsRequest_ReportDk			= ( 1 << 12 ), // 0x1000 // PYR-38802
	eUpdateRealChipsRequest_RollSideways		= ( 1 << 13 ), // 0x2000 // PYR-48391
	eUpdateRealChipsRequest_Rollback_SAG		= ( 1 << 14 ), // 0x4000 // PYR-54513
	eUpdateRealChipsRequest_BountyTourn			= ( 1 << 15 ), // 0x8000 // PYR-52678
	eUpdateRealChipsRequest_ReentryTourn		= ( 1 << 16 ), // 0x10000 // PYR-52678
	eUpdateRealChipsRequest_PlayerInitiated		= ( 1 << 17 ), // 0x20000 // PYR-52678
	eUpdateRealChipsRequest_SatelliteFR			= ( 1 << 18 ), // 0x40000 // PYR-52678
	eUpdateRealChipsRequest_MilestoneTourn		= ( 1 << 19 ), // 0x80000 // PYR-102950
};

enum eUpdateRealChipsUserFlags // PYR-26251 - user-specific flags
{
	eUpdateRealChipsUser_NotEliminated			= ( 1 << 0 ),  // 0x1
	eUpdateRealChipsUser_ReturnTargetTicket		= ( 1 << 1 ),  // 0x2 PYR-26990
	eUpdateRealChipsUser_WonSatellite			= ( 1 << 2 ),  // 0x4 PYR-26990
	eUpdateRealChipsUser_DebitDummy				= ( 1 << 3 ),  // 0x8 PYR-27271
	eUpdateRealChipsUser_CreditTMoney			= ( 1 << 4 ),  // 0x10 PYR-27271
	eUpdateRealChipsUser_CameFromSatellite		= ( 1 << 5 ),  // 0x20 PYR-31753 - player really came from satellite (for unregistration purposes)
	eUpdateUserRealChips_MultipleFlightEntries	= ( 1 << 6 ),  // 0x40 // PYR-30436
	eUpdateRealChipsUser_GiveTargetTicket		= ( 1 << 7 ),  // 0x80 // PYR-30437
	eUpdateRealChipsUser_ReentryUnregistration	= ( 1 << 8 ),  // 0x100 // PYR-36943
	eUpdateRealChipsUser_HasBoundUserRoll		= ( 1 << 9 ),  // 0x200 // PYR-38743
	eUpdateRealChipsUser_Winner					= ( 1 << 10 ), // 0x400 // PYR-52678 - to distinguish FR winner from loser on TournOut reply
	eUpdateRealChipsUser_TournNextFlight		= ( 1 << 11 ), // 0x800 // PYR-72636 - to distinguish winning a flighted tournament (except final flight)
	eUpdateRealChipsUser_DontCheckLimits		= ( 1 << 12 ), // 0x1000 // PYR-84882 - true in case of seat finder table merge
};

// NB!!! Make sure all accessors in CreditRealChipsRequestCommonStruct and DebitRealChipsRequestCommonStruct
// classes are properly changed when adding new requestors!!!
enum eUpdateRealChipsRequestor // #19752
{
	eUpdateRealChipsRequestorGeneric  = 0,

	// !!! when you add more table/blitz requestors
	// please update CreditRealChipsCommonStruct and DebitRealChipsCommonStruct accessors accordingly
	eUpdateRealChipsRequestorTableIn  = 1, //TableIn
	eUpdateRealChipsRequestorTableOut = 2, //TableOut
	eUpdateRealChipsRequestorTableChipsReturn = 3, // used for ring tables and blitz (AAMS didn't approve of buyin) //TableChipsReturn
	eAddRealChipsRequestorTable = 4, //TableRebuy
	eUpdateRealChipsRequestorTableClosed = 5, // PYR-26251 - DBM_Q_KILL_TABLE or DBM_Q_CLEANUP_DEAD_TABLE //TableClosed

	eUpdateRealChipsRequestorBlitzIn  = 11, //BlitzIn
	eUpdateRealChipsRequestorBlitzOut = 12, //BlitzOut
	eAddRealChipsRequestorBlitz = 14, //BlitzRebuy
	eUpdateRealChipsRequestorMoveFpp = 15, //MoveFPP
	eUpdateRealChipsRequestorMoveFppBlitz = 16, //MoveFPP Blitz
	eUpdateRealChipsRequestorBlitzReconcileLoan = 17, //ReconcileLoan
	// !!! when you add more table/blitz requestors
	// please update CreditRealChipsCommonStruct and DebitRealChipsCommonStruct accessors accordingly

	// PYR-26251
	// !!! when you add more tourn requestors
	// please update CreditRealChipsRequestCommonStruct and DebitRealChipsRequestCommonStruct accessors accordingly
	eAddRealChipsRequestorTourn = 20, //TournRebuyAddon
	eUpdateRealChipsRequestorTournIn = 21, // buyin to a tournament - DBM_Q_MTL_REGISTER_USER //TournIn
	eUpdateRealChipsRequestorTournBounty = 22, // PYR-26251 - DBM_Q_MTL_TOURN_BOUNTY //TournBounty
	eUpdateRealChipsRequestorTournChipsReturn = 23, // PYR-26251 - rebuy/addon wasn't approved by AAMS //TournChipsReturn
	eUpdateRealChipsRequestorTournOut = 24, // PYR-26251 - DBM_Q_MTL_USERS_OUT //TournOut
	// PYR-50732 - obsoleted
	//eUpdateRealChipsRequestorTournCancel = 25, // PYR-26251 - DBM_Q_TDBM_CANCEL_RUNNING_TOURN //TournCancelRunning
	eUpdateRealChipsRequestorTournUnregister = 26, // PYR-26251 - DBM_Q_MTL_UNREGISTER_USER or DBM_Q_MTL_UNREG_USERS //TournUnreg
	eUpdateRealChipsRequestorRefundSatWinners = 27, // PYR-26990 - DBM_Q_REFUND_SAT_WINNERS //RefundSatWinners
	eUpdateRealChipsRequestorRefundFailedSatWinners = 28, // PYR-26990 - DBM_Q_MTL_REGISTER_SAT_WINNERS only for winners who failed to register //RefundFailedSatWinners
	eUpdateRealChipsRequestorTournCancelBatch = 29, // PYR-28503 - DBM_Q_CANCEL_RUNNING_TOURN_BATCH //TournCancelRunningBatch
	eUpdateRealChipsRequestorTournMileStone = 30,  // PYR-28689 - DBM_Q_CREDIT_TOURN_MILESTONE_WINNING_TABLE //TournMileStonPrize
	eUpdateRealChipsRequestorMoveFlightUser = 31,  // PYR-32003 - DBM_Q_MOVE_FLIGHT_USER
	eUpdateRealChipsRequestorTournEarlyPayout = 32, // PYR-50812 - DBM_Q_MTL_EARLY_PAYOUT
	eUpdateRealChipsRequestor_RegSatWinners = 33, // PYR-72636 - DBM_Q_MTL_REGISTER_SAT_WINNERS

	// !!! when you add more tourn requestors
	// please update CreditRealChipsRequestCommonStruct and DebitRealChipsRequestCommonStruct accessors accordingly

	eRequestorSportsBook= 40, // SBK-62
	eRequestorFppBundles= 41, // PYR-32027
	eRequestorStarsDraft= 42, // PYR-40346
	eRequestorDfsIntl= 42, // PYR-40346, PYR-48971 renamed from eRequestorStarsDraft
	eRequestorPoolBetting=43, // PYR-42410
	eRequestorSportsBookInitial= 44, // PYR-53041  this requestor only used for placeBet

	eRequestorStarsStore = 45, // PYR-81644  - DBM_QG_DEBIT_FUNDS

	// PYR-37300 - add more when needed
	// !!!Don't forget to update isMiniGameRequest() in CreditRealChipsRequestCommonStruct or DebitRealChipsRequestCommonStruct
	eUpdateRealChipsRequestor_StartMiniGame			= 70, // player starts a minigame
	eUpdateRealChipsRequestor_PayJackpotShare		= 71, // player get paid Jackpot share
	eUpdateRealChipsRequestor_PayMiniGamePrize		= 72, // player gets paid a normal prize from a minigame at the end
	eUpdateRealChipsRequestor_PayMiniGamePrizeUpdate = 73, // player gets paid a prize partway through a minigame

	// PYR-109372
	// Note that the following are related to isProgressivePlusRequest() in CreditRealChipsRequestCommonStruct or DebitRealChipsRequestCommonStruct
	eUpdateRealChipsRequestor_ProgressivePlus_Buyin = 80, // player buys in to progressive plus pool (DBM_Q_PPP_REGISTER)
	eUpdateRealChipsRequestor_ProgressivePlus_Unregister = 81, // player unregisters from to progressive plus pool (DBM_Q_PPP_UNREGISTER)
	eUpdateRealChipsRequestor_ProgressivePlus_Cashout = 82, // player is paid their entry's value progressive plus pool (DBM_Q_PPP_CASHOUT)
};

enum SelfDepositLimitType
{
	eDailyDepositLimit   = 1,
	eWeeklyDepositLimit  = 7,
	eMonthlyDepositLimit = 30,
};

enum eDefaultSelfDepositLimits // #20921
{
	eDefaultSelfDepositLimit_SpainDaily = 60000,
	eDefaultSelfDepositLimit_SpainWeekly = 150000,
	eDefaultSelfDepositLimit_SpainMonthly = 300000,
};

enum eCashierHistoryFlags
{
	eCashierHistoryFlag_HeldFunds = 1, // #21235
};

enum eUserIntentToPlay // #16725 - DK: PROPERTYINT for USERPROPERTIES.PROPERTYID=eUserPropertyIntentToPlay=56
{
	eUserIntentToPlay_UNKNOWN         = 0,
	eUserIntentToPlayMonthly_0_10     = 1,
	eUserIntentToPlayMonthly_10_100   = 2,
	eUserIntentToPlayMonthly_100_1000 = 3,
	eUserIntentToPlayMonthly_1000_up  = 4,
};

// #21516 - VOUCHER PAYSYSTEM: VOUCHERS.FLAGS for vouchers Gateway
enum eVouchersGatewaysFlags
{
	eVouchersLottomatica  = 0, // LOTTOMATICA_GW_NAME
	eVouchersEpayCard = 1,     // EPAYCARD_GATEWAY_NAME
	eVouchersPayZone = 2,      // PAYZONE_GATEWAY_NAME
	eVouchersPayPoint = 3,     // PAYPOINT_GATEWAY_NAME
	eVouchersDelta = 4,        // DELTA_VOUCHER_GW_NAME
	eVouchersAlvadis = 5,      // ALVADIS_VOUCHER_GW_NAME
	eVouchersPSCashManual = 6, // PSCASH_MANUAL_VOUCHER_GW_NAME
	eVouchersStarsVoucher = 7, // STARSVOUCHER_GW_NAME
};

enum eUserTitle // #17003 - UserProperties.propertyId = eUserPropertyTitle = 59
{
	eUserTitleNone = 0,
	eUserTitleMr   = 1,
	eUserTitleMrs  = 2,
	eUserTitleMiss = 3,
	eUserTitleMs   = 4,
	eUserTitleDr   = 5,
};

enum eAgeVerificationPersonalIdType // #17003 matches COUNTRYIDS.ID_TYPE
{
	eAgeVerificationPersonalIdTypeNone = 0,
	eAgeVerificationPersonalIdTypePassport = 1,
	eAgeVerificationPersonalIdTypePersonalId  = 2,
	eAgeVerificationPersonalIdTypeIdentityCard = 3, // also SIN
	eAgeVerificationPersonalIdTypeDriversLicense = 4,

	eAgeVerificationPersonalIdTypeTravelDocument = 8,
};

enum eEsMigrationOption
{
	eEsMigrationOption_OneShot        = 0, // #20725
	eEsMigrationOption_UKashCloseLoop = 1, // #22434
	eEsMigrationOption_UKashOpenLoop  = 2, // #23157
};

#define SELFLIMIT_TEST_NA		    0
#define SELFLIMIT_TEST_PASSED		1
#define SELFLIMIT_TEST_FAILED		2

/*
// PYR-20319
enum ePhoneTypes
{
	ePhoneLandline = 1,
	ePhoneMobile   = 2,
};
*/

// PYR-23045
enum ePromoImagesAuditAction
{
	ePromoImagesAuditAdd		= 0,
	ePromoImagesAuditDelete		= 1,
	ePromoImagesAuditChange		= 2, 
};

// PYR-23743
enum OrderFlags	
{
	eFlagsNone = 0,
	eStaffPurchase	   = (1 << 0),
	eCanceledAfterShip = (1 << 1), // PYR-25209
	eCanceledAfterProcessed = (1 << 2), // PYR-25209
	eMailedLost = (1 << 3), // PYR-25779
	eMailedLostFoundAtStore = (1 << 4), // PYR-25779
	eMailedLostFoundByPlayer = (1 << 5), // PYR-25779
	eMailedLostFoundByPlayerKeepingNoFPP = (1 << 6), // PYR-25779 (Player keeps item no FPP taken)
	eMailedLostFoundByPlayerKeepingFPPTaken = (1 << 7), // PYR-25779 (Player keeps item FPP taken)
	eMailedLostFoundRequestedSendBack = (1 << 8), // PYR-25779
	eMailedLostFoundSentBack = (1 << 9), // PYR-25779
	eCancelReturnToStock = (1 << 10), // PYR-25779
	eCancelCreditFpp = (1 << 11), // PYR-25779
};

// obsolete
enum eFullTiltMessageType // FTMESSSAGES.MSGTYPE
{
	eFullTiltMessageType_Pairing = 0,
	eFullTiltMessageType_Migration = 1,
	eFullTiltMessageType_DepositToPs = 4,
	eFullTiltMessageType_RmOkReply = 5, // FTOBJECTID=gen("FTMESSAGES5"), FTUSERINTID=0, PSOBJECTID=USERS.UserIntId, errInfo="USERCHNAGES.UPDATEID,..."
	eFullTiltMessageType_SelfExclude = 7,
	eFullTiltMessageType_LimitUpdated = 9,
};

// obsolete
enum eFullTiltUserFlags
{
	eFullTiltUserFlag_FtPlayerVerified = (1 << 0), // #23771
	eFullTiltUserFlag_FtPlayerIsFR     = (1 << 1), // #23771
	eFullTiltUserFlag_FtPlayerReserv1  = (1 << 2), // reserved for Brand extension if necessary
	eFullTiltUserFlag_FtPlayerReserv2  = (1 << 3),
	eFullTiltUserFlag_FtWasBlackCard   = (1 << 4), // #24070
};

enum eChipsInfoRequests // #24118
{
	eChipsInfoRequest_NoExtras	 	 = 0,

	eChipsInfoRequest_Allocated      = (1 << 0),
	eChipsInfoRequest_MoveAllocFpp   = (1 << 1),
	eChipsInfoRequest_CashinLimits   = (1 << 2),
//	eChipsInfoRequest_Bonus          = (1 << 3),
//	eChipsInfoRequest_Elv            = (1 << 4),
	eChipsInfoRequest_AllUserProps   = (1 << 5), // returns ALL user properties PYR-73514
	eChipsInfoRequest_TrackCodes     = (1 << 6),
	eChipsInfoRequest_Uncleared      = (1 << 7),
	eChipsInfoRequest_FullTilt       = (1 << 8),
	eChipsInfoRequest_UserRolls      = (1 << 9), // PYR-31838

	eChipsInfoRequest_ALL			 = ~0,
};

#define LOG_TRACE_FAULTY_LOGIC "faulty logic "
#define LOG_TRACE_FAULTY_LOGIC_WITH_STACK "stack trace faulty logic"
#define LOG_TRACE_FAULTY_LOGIC_CANDIDATE "|failedlogic candidate| " // for dev to check questionable behavior - a candidate for further "faulty logic"

#define MAX_BANKROLL_SIZE	2000000000 // $20,000,000.00
#define HUNDRED_CENTS ((INT64)100)
#define THOUSAND_MILLICENTS ((INT64)1000)

// moved from aamsprepstmt.h PYR-24630
#define AAMS_UNIQUE_IDENTIFIER_LEN (16 + 1)

//	PYR-23221 table CLIENTINFO
#define CLIENTINFO_USERID_LEN           (20 + 1)
#define CLIENTINFO_GUIVER_LEN           (64 + 1)
#define CLIENTINFO_WINVER_LEN           (128 + 1)
#define CLIENTINFO_OSDETAIL_LEN         (256 + 1)
#define CLIENTINFO_PLAYLANG_LEN         (128 + 1)
#define CLIENTINFO_USERLANG_LEN         (128 + 1)
#define CLIENTINFO_DEFLANG_LEN          (128 + 1)
#define CLIENTINFO_KBDLANG_LEN          (128 + 1)
#define CLIENTINFO_VMWTYPE_LEN          (128 + 1)
#define CLIENTINFO_MONITORINFO_LEN      (256 + 1)
#define CLIENTINFO_OSSID_LEN            (184 + 1)
#define CLIENTINFO_LOGINID_LEN			(128 + 1)
#define CLIENTINFO_OS_TIMEZONE_LEN		(256 + 1)
#define CLIENTINFO_SYSMANUF_LEN			(256 + 1)
#define CLIENTINFO_SYSPRODUCTNAME_LEN	(256 + 1)
#define CLIENTINFO_SYSVERSION_LEN		(256 + 1)
#define CLIENTINFO_DEVICE_ID_LEN		(32 + 1)


//	PYR-106112 table ALIASWORDS
#define ALIASBLOCKS_BLOCK_LEN           (20 + 1)

//	PYR-106112 table ASSIGNEDALIASES
#define ASSIGNEDALIASES_ALIAS_LEN           (20 + 1)

enum eAuthResponseFlags // PYR-24802: bit mask for MsgDbmAUserAuth::authFlags (UINT32).
{
	eAuthResponseFlags_LimitedModeEULA					= (1 << 0), // PYR-24802: user is in limited mode due to EULA 
	eAuthResponseFlags_LimitedModeRSA					= (1 << 1), // PYR-24969: user is in limited mode due to RSA token
	eAuthResponseFlags_LimitedModeGeoIPPending			= (1 << 2), // PYR-27243: user is set to limited mode due to GeoIP pending at the first login
	eAuthResponseFlags_LimitedModeGeoIPRejected			= (1 << 3), // PYR-27243: user is in limited mode due to failed GeoIP check
	eAuthResponseFlags_GeoIPCarryOver					= (1 << 4), // PYR-27243: LimitedModeGeoIP flags are carried over between login sessions
	eAuthResponseFlags_PGADDown							= (1 << 5), // PYR-26675: IT user is in limited mode due to PGAD is down
	eAuthResponseFlags_LimitedModeGeoIPUnknown			= (1 << 6), // PYR-27243: user is in limited mode due to GeoIP result is unknown
	eAuthResponseFlags_LimitedModeGeoIPBlockedSoftware	= (1 << 7), // PYR-27243: user is in limited mode due to GeoIP result is blocked software detected
	eAuthResponseFlags_LimitedModeGeoIPUsingProxy		= (1 << 8), // PYR-27243: user is in limited mode due to GeoIP result is proxy detected
	//eAuthResponseFlags_LimitedModeDormantUser			= (1 << 9), // PYR-40493. No longer used but business may reinstate.
	eAuthResponseFlags_LimitedToPlayMoneyOnly			= (1 << 11), // PYR-34948 if set the player is allowed to play PM games only
	eAuthResponseFlags_LimitedModeLTDL					= (1 << 12), // PYR-42099: user is in limited mode due to life time deposit limit 
	eAuthResponseFlags_LimitedModeFrAccDocsMismatch		= (1 << 13), //	PYR-43398: user is in limited mode due to mismatch between docs and RMF
	eAuthResponseFlags_BypassEmailVerificationForPm 	= (1 << 14), //	PYR-55365: bypass email verification for Play Money games
	eAuthResponseFlags_BypassEmailVerificationForRm 	= (1 << 15), //	PYR-55365: bypass email verification for Real Money games
	eAuthResponseFlags_GeoIPModeMandatory				= (1 << 16), // PYR-62562: user is in 'mandatory' Geo IP mode
	eAuthResponseFlags_GeoIPModeOff						= (1 << 17), // PYR-62562: user is in 'off' Geo IP mode
	eAuthResponseFlags_IsFirstLoginThisObjectType		= (1 << 18), // PYR-63165: First login from object type during user session (loginId) - not appSession.
	eAuthResponseFlags_LimitedModeInExclusionBoundary	= (1 << 19), // PYR-74823: user is in limited mode due to exclusion boundary (i.e. Casino or in close proximity of Casino)

	// NOTE: remember to update eAuthResponseFlags_LimitedModeNoneGeoIPFlags below

	eAuthResponseFlags_AllLimitedModeGeoIPFlags =  (eAuthResponseFlags_LimitedModeGeoIPRejected | 
													eAuthResponseFlags_LimitedModeGeoIPUnknown  |
													eAuthResponseFlags_LimitedModeGeoIPBlockedSoftware |
													eAuthResponseFlags_LimitedModeGeoIPUsingProxy |
													eAuthResponseFlags_LimitedModeGeoIPPending	|
													eAuthResponseFlags_LimitedModeInExclusionBoundary),

	// limitation flags that are not GeoIp related. See PYR-27243
	eAuthResponseFlags_LimitedModeNoneGeoIPFlags =  (eAuthResponseFlags_LimitedModeEULA | 
													eAuthResponseFlags_LimitedModeRSA  |
													eAuthResponseFlags_PGADDown |
													eAuthResponseFlags_LimitedToPlayMoneyOnly |
													eAuthResponseFlags_LimitedModeLTDL),
	// PYR-62562
	eAuthResponseFlags_AllGeoIPMode				 =  (eAuthResponseFlags_GeoIPModeMandatory | eAuthResponseFlags_GeoIPModeOff),
};

enum eUserExLoginFlags // PYR-24802: bit mask for UserEx::loginFlags (UINT64)
{
	eUserExLoginFlags_None								= 0, // PYR-62562: no flags
	eUserExLoginFlags_LimitedModeEULA					= (1 << 0), // PYR-24802: user is in limited mode due to EULA 
										 							// (1 << 1) is reserved
	eUserExLoginFlags_LimitedModeGeoIPPending			= (1 << 2), // PYR-27243: user is set to limited mode due to GeoIP pending at the first login
	eUserExLoginFlags_LimitedModeGeoIPRejected			= (1 << 3), // PYR-27243: user is in limited mode due to failed GeoIP check
	eUserExLoginFlags_GeoIPCarryOver					= (1 << 4), // PYR-27243: LimitedModeGeoIP flags are carried over between login sessions
	eUserExLoginFlags_PGADDown							= (1 << 5), // PYR-26675: IT user is in limited mode due to PGAD is down
	eUserExLoginFlags_LimitedModeGeoIPUnknown			= (1 << 6), // PYR-27243: user is in limited mode due to GeoIP result is unknown
	eUserExLoginFlags_LimitedModeGeoIPBlockedSoftware	= (1 << 7), // PYR-27243: user is in limited mode due to GeoIP result is blocked software detected
	eUserExLoginFlags_LimitedModeGeoIPUsingProxy		= (1 << 8), // PYR-27243: user is in limited mode due to GeoIP result is proxy detected
	//eUserExLoginFlags_LimitedModeDormantUser			= (1 << 9),	//	PYR-40493. No longer used but business may reinstate.
	eUserExLoginFlags_TemporaryChatBannedByACM			= (1 << 10), // PYR-28673: user chat is banned for a short period  
	eUserExLoginFlags_LimitedToPlayMoneyOnly			= (1 << 11), // PYR-34948 if set the player is allowed to play PM games only
	eUserExLoginFlags_LimitedModeLTDL					= (1 << 12), // PYR-42099: user is in limited mode due to life time deposit limit 
	eUserExLoginFlags_LimitedModeFrAccDocsMismatch		= (1 << 13), //	PYR-43398: user is in limited mode due to mismatch between docs and RMF
	eUserExLoginFlags_BypassEmailVerificationForPm		= (1 << 14), //	PYR-55365: bypass email verification for Play Money games
	eUserExLoginFlags_BypassEmailVerificationForRm 		= (1 << 15), //	PYR-55365: bypass email verification for Real Money games
	eUserExLoginFlags_GeoIPModeMandatory				= (1 << 16), // PYR-62562: user is in 'mandatory' Geo IP mode
	eUserExLoginFlags_GeoIPModeOff						= (1 << 17), // PYR-62562: user is in 'off' Geo IP mode
	eUserExLoginFlags_LimitedModeInExclusionBoundary	= (1 << 18), // PYR-74823: user is in limited mode due to exclusion boundary (i.e. Casino or in close proximity of Casino)

	eUserExLoginFlags_AllLimitedModeGeoIPFlags =   (eUserExLoginFlags_LimitedModeGeoIPRejected | 
												    eUserExLoginFlags_LimitedModeGeoIPUnknown  |
													eUserExLoginFlags_LimitedModeGeoIPBlockedSoftware |
													eUserExLoginFlags_LimitedModeGeoIPUsingProxy |
													eUserExLoginFlags_LimitedModeGeoIPPending	|
													eUserExLoginFlags_LimitedModeInExclusionBoundary),
	// PYR-62562
	eUserExLoginFlags_AllGeoIPMode			   =   (eUserExLoginFlags_GeoIPModeMandatory | eUserExLoginFlags_GeoIPModeOff),
};

enum eTournUserFlags //PYR-22276
{
	eTournUserFlags_ThisFlightLoser = (1 << 0), // 1
	eTournUserFlags_PrevFlightLoser = (1 << 1), // 2
	eTournUserFlags_Removed			= (1 << 2), // 4
	eTournUserFlags_HasReentries	= (1 << 3), // 8 - PYR-24444
	eTournUserFlags_WasRefunded		= (1 << 4), // 16 - PYR-25970
	// MG TODO: PYR-72636 - remove this flag when all target tournaments with this flag set are completed, since it's no longer set.
	eTournUserFlags_GiveTargetTicket= (1 << 5), // 32 - PYR-26990 - when user unregisters from target, refund them with a target ticket
	eTournUserFlags_FromFreeroll	= (1 << 6), // 64 - PYR-26990 - when satellite is a freeroll - needed to properly assign transType
	eTournUserFlags_CanUnregisterFromTarget		= (1 << 7), // 128 - PYR-26438 came from satellite that allows unreg from target
	eTournUserFlags_CanNotUnregisterFromTarget	= (1 << 8), // 256 - PYR-26438 came from satellite that does not allow unreg from target
// PYR-34930 - obsolete
//	eTournUserFlags_SatOverwrite	= (1 << 9), // 512 - PYR-26438 the satellite winner overwrites already registered one (used only in main dbm tournaments)
	eTournUserFlags_PlayedHand		= (1 << 10), // 1024 - PYR-30164 - user played at least one hand in the tournament (used to generate "played in tournament" ICE event)
	eTournUserFlags_FromSatUpdated	= (1 << 11), // 2048 - PYR-31753 - user's FROMSATELLITE column was updated (to prevent unregistration)
	eTournUserFlags_NoRebuyAddonRakeTaken = (1 << 12), // 4096 - PYR-32776 - user's rebuy/addon rake was not charged as configured in [RebuyRakeCountries] section in lobby INIDATA
	eTournUserFlags_ChangingFlights = (1 << 13), // 8192 - PYR-32003 - user is changing flights
	eTournUserFlags_RegFromAltBounty			= (1 << 14), // 16384 - PYR-31658 - assign proper refund transType when registration is from alt bounty
	eTournUserFlags_AttemptedToAward			= (1 << 15), // 32768 - PYR-54545 - do not try to award again if already awarded a 0 prize
	eTournUserFlags_EligibleForSAGFreeBetReward	= (1 << 16), // 65536 - PYR-59874 - eligible for FreeBet reward in SAG tournament
	eTournUserFlags_WonSAGFreeBetReward			= (1 << 17), // 131072 - PYR-59874 - won the FreeBet reward in SAG tournament
	// PYR-72636 - won satellite with new way of registration.
	// TODO: remove after all tournaments switched to new way including flights, which is not implemented yet,
	// since this flag is needed for refund and register to target purposes in the period when old and new winners coexist in the system.
	eTournUserFlags_NewRegisterSatWinners		= (1 << 18), // 262144
	eTournUserFlags_FlightEntryToForfeit = ( 1 << 19 ),			// 524288  in DBM will be set on feeder's user only
	eTournUserFlags_FlightEntryWithBetterStack = ( 1 << 20 ),	// 1048576 in DBM will be set on feeder's user only
	eTournUserFlags_FligtUserStackForfeitPending = ( 1 << 21 ), // 2097152 in DBM will be set on target's user to detect stuck operation
	eTournUserFlags_FlightEntryWithBetterStackForfeitIndirect = ( 1 << 22 ), // 4194304 in DBM will be set on feeder's user only
};

// PYR-24812
enum eTemplateCacheMessageCategory
{
	eUserBlocking		= (1 << 0),
	eUserSecurityFreeze	= (1 << 1),

	eTemplateCacheMessageCategory_Last = eUserSecurityFreeze,	//	must be kept up to date
};

//PYR-24396 table TOURNHAPPYHOURS
#define TOURNHAPPYHOURS_NAME_LEN			( 50+1)
#define TOURNHAPPYHOURS_GAMETYPES_LEN		(200+1)
#define TOURNHAPPYHOURS_BETTINGTYPES_LEN	( 50+1)
#define TOURNHAPPYHOURS_WEEKDAYS_LEN		( 15+1)

//PYR-25162
enum eUserTrackCodesAction
{
	eAssignUserTrackCode = 0,
	eRemoveUserTrackCode = 1,
};

enum eCardInfoTableFlags // PYR-24538: bits in CARDINFO.FLAGS
{
	eCardInfoTableFlag_After24538              = (1 << 0), // always set for new records after 2013-02 release, 0 before
	eCardInfoTableFlag_WasFastDepositWhenSaved = (1 << 1), // but not necessarily is a fast deposit option currently
	eCardInfoTableFlag_TriedResolveCcIdAndCur  = (1 << 2), // so not to attempt lazy update the second time if the first failed; if CC_ID != 0 then resolving was successful
	eCardInfoTableFlag_CreditCardSubType       = (1 << 3), // CreditCardSubType was used to initiate the deposit
	eCardInfoTableFlag_DepositResolvedCcId     = (1 << 4), // old (expired) CARDINFO entry with cc_id=0 was overwritten after deposit
	eCardInfoTableFlag_UsedForPlayMoneyPurch   = (1 << 5), // PYR-29055
	eCardInfoTableFlag_UsedForRealMoneyDepos   = (1 << 6), // PYR-29055 flag matters for prod INFOID>9M. All previous records are RM deposits always.
	eCardInfoTableFlag_Validated               = (1 << 7), // PYR-30281 flag matters for ELV_SEPA
	eCardInfoTableFlag_RecordAddedByPlaid      = (1 << 8), // PYR-116520
	eCardInfoTableFlag_PlaidWasAddedLater      = (1 << 9), // PYR-116520
	eCardInfoTableFlag_VisaSubType = eCardInfoTableFlag_CreditCardSubType, // #vasilyt to be removed
};

enum eUserBlockingCode // PYR-24812
{
	eUserBlockingCode_Unblocked = 0,
	eUserBlockingCode_ItDormant = 1,	// PYR-31811
	eUserBlockingCode_ItLicenseVersion = 24825,	//#24825
};

#define ITOLDLICENSEHOLDERS_COMMENTS_LEN (1000+1) //#24825

enum eCashtransTableId // PYR-25875 - ref view CASHTRANSINDINFO, all these tables use same 'CashTrans' generator
{
	eCashtransTableId_NotExist   = 0, // record not found
	eCashtransTableId_Cashtrans  = 1, // transId is in CASHTRANS
	eCashtransTableId_Transfers  = 2, // transId is in CASHTRANSFERS  - ROOT-only table, view on OLAP
	eCashtransTableId_Conversion = 3, // transId is in CASHCONVERSION - ROOT-only table, view on OLAP
	eCashtransTableId_CashtransPM= 4, // transId is in CASHTRANSPM    - exists on both ROOT and OLAP, not in PYRFIN
	eCashtransTableId_Cashtrans4Upd= 5, // transId is in CASHTRANS4UPD - exists on ROOT only
	eCashtransTableId_CashtransPre = 6, // PYR-33629 transId is in CASHTRANSPre  - exists on both ROOT and OLAP
};

enum eUserRequestConfirmStatus // PYR-26155
{
	eUserRequestConfirmStatus_NotConfirmed			= 0,
	eUserRequestConfirmStatus_Confirmed				= 1,
	eUserRequestConfirmStatus_Offline				= 2,
	eUserRequestConfirmStatus_DontUnderstand		= 3,
};

// CASHTRANS.ERRCODE INTERNAL CODES (moved here from cashierobject.h after BUILD1011)
#define TRANS_ERR_CODE_7995								"PS:7995"   // #12806
#define TRANS_ERR_CODE_CLIENT_DISCONNECTED				"PS:CL_DC"  // #14266
#define TRANS_ERR_NO_GATEWAY							"PS:NOGW"
#define TRANS_ERR_NO_3DS_GATEWAY						"PS:NOGW-3DS" // #23283
#define TRANS_ERR_NOT_REALMONEY_OK						"PS:RM_NOK"
#define TRANS_ERR_CREDITCARDS_NOT_ALLOWED_LICENSE		"PS:CREDITCARDS_NOT_ALLOWED_LICENSE"      // PYR-21794
#define TRANS_ERR_CREDITCARDS_NOT_ALLOWED_BIN_COUNTRY	"PS:CREDITCARDS_NOT_ALLOWED_BIN_COUNTRY"  // PYR-21794
#define TRANS_ERR_CREDITCARDS_NOT_ALLOWED_USER_COUNTRY	"PS:CREDITCARDS_NOT_ALLOWED_USER_COUNTRY" // PYR-21794
// PYR-25884
#define TRANS_ERR_USER_CARD_CHECK_VIOLATION 			"PS:UserCardCheckViolation"
#define TRANS_ERR_BLOCKED_CARD              			"PS:BlockedCard"
#define TRANS_ERR_AMOUNT_MISMATCH						"PS:AmountMismatch"
#define TRANS_ERR_NO_PAYPAL_PAYER_ID					"PS:NoPaypalPayerID"
// PYR-27652
#define TRANS_ERR_INVALID_NOTIFICATION                  "PS:InvalidNotif"
#define TRANS_ERR_HELD_EXPIRED						    "PS:HELD_EXP" // PYR-29092
#define TRANS_ERR_REJECTED_WHEN_CROPPED				    "PS:CROP" // PYR-29839

#define TRANS_ERR_CARD_NOT_VERIFIED						"PS:CardNotVerified" // PYR-121725

// PYR-23644 table PGADTRANSACTS
#define PGADTRANSACTS_TABLE_NAME "PGADTRANSACTS"
#define PGADTRANSACTS_XTRANSSTR_LEN (32+1)
#define PGADTRANSACTS_PROPSTR_LEN (32+1)
enum ePgadTransactFlags // bits in PGADTRANSACTS.FLAGS
{
	ePgadTransactFlag_TTicketTotalUpdate = 1, // if set then PGADTRANSACTS.TRANSID is bound to TournTicketsAudit.AUDITID (see PgadTransactData::transIdFromTTicketsAuditId)
	ePgadTransactFlag_SentByAdmin		 = ( 1 << 1 ), // PYR-106575
};

enum eDepositLevelsFlags // bits in DEPOSITLEVELS.FLAGS
{
	eDepositLevelsFlag_mobileVerifiedRequired = ( 1 << 0 ), //#23340
};

// table AWARDS
#define AWARDS_AWARDNAME_LEN (32 + 1)
#define AWARDS_DESCRIPTION_LEN (256 + 1)
#define AWARDS_COUNTRYLIMITS_LEN (256 + 1)
#define AWARDS_USERFILTER_LEN (80 + 1)
enum eAwardTypes // AWARDS.AWARDTYPE
{
	eAwardType_Undefined = 0,
	eAwardType_Cash = 1,
	eAwardType_Fpp = 2,
};
enum eAwardStatuses // AWARDS.STATUS
{
	eAwardStatus_InDevelopment = 0,
	eAwardStatus_Active = 1,
	eAwardStatus_Cancelled = 2,
	// add more per Admin's request: Expired, etc.
};

enum eAwardFlags // bits in AWARDS.FLAGS
{
	eAwardFlag_AppliesTo1stDeposit = (1 << 0), // PYR-29073
	eAwardFlag_OnlyFor1stDeposit   = (1 << 1), // PYR-29073
};

enum eUsBanType // #27257
{
	eUsBanType_Unknown      = 0,
	eUsBanType_Regulator    = 1,
	eUsBanType_SelfExcluded = 2,
	eUsBanType_SP_NBA = 3,
	eUsBanType_SP_Colledge = 4,
};

// PYR-27913 table USERFISCALENCR
#define USERFISCALENCR_TABLE_NAME "USERFISCALENCR"
#define USERFISCALENCR_ENCR_LEN (64+1)

// tables USERFISCALENCR, NJBLACKLIST
#define FISCAL_HASH_LEN (48+1)

enum eChangeRecordAction
{
	eChangeRecordAction_NoChange = 0,
	eChangeRecordAction_Insert = 1,
	eChangeRecordAction_Update = 2,
	eChangeRecordAction_Delete = 3,
};

enum eTimeLimitType // PYR-27741
{
	eNoTimeLimit = 0,
	eLoginTimeLimit = 1,
	ePlayingTimeLimit = 2,
};

#define JUBILEE_TOURN_DENOMINATOR	10000

enum eLobbyEventTabFlag // PYR-26712
{
	eLobbyEventTabNonPipe	= 0x01,
	eLobbyEventTabPipe		= 0x02,
	eLobbyEventTabFeatured	= 0x04,
};

enum eLobbyEventSubTabFlag // PYR-26712
{
	eLobbyEventTabSitGo		= 0x01,
};

enum eDailyTournLiabilityType
{
	eDTLT_ExtendedGeneral	= 0,
	eDTLT_ExtendedES		= 2,
	eDTLT_ExtendedUSNJ		= 3,
};

enum eLoginCredentialType // PYR-28548
{
	eLoginCredentialType_UserId	= 1,
	eLoginCredentialType_Email	= 2,
};

enum eUsersSimilarityType // prev called CheckUserEmailInstallStmt::eCheckUserType - moved from CheckUserEmailInstallStmt and renamed
{
	eUsersSimilarityType_None = 0,
	eUsersSimilarityType_Email = 1,
	eUsersSimilarityType_Install = 2,
	eUsersSimilarityType_NormalizedId = 3,
	eUsersSimilarityType_InstallWithExempt = 4 //#13362
};

enum eArjelAccCloseStatus // PYR-29386
{
	eArjelAccCloseStatus_Closed = 0,
	eArjelAccCloseStatus_InProgress = 1,
	eArjelAccCloseStatus_Failed = 2,
	eArjelAccCloseStatus_OfflineHands = 3,
};

enum eArjelAccRegStatus // PYR-54565
{
	eArjelAccRegStatus_Undefined = 0,
	eArjelAccRegStatus_Registered = 1,
	eArjelAccRegStatus_UnRegistered = 2,
};

enum ClientPlayChipsStore // MOVED here from chipsdbm.h with PYR-29627
{
	ClientPlayChipsStore_unknown = 0,
	ClientPlayChipsStore_web     = 1,
	ClientPlayChipsStore_apple   = 2,
	ClientPlayChipsStore_google  = 3,
	ClientPlayChipsStore_amazon  = 4,
	ClientPlayChipsStore_samsung = 5,
	ClientPlayChipsStore_yandex  = 6,
		// Temporary items below needed until PYR-29627 can be completed on mobile client
	ClientPlayChipsStore_apple_net   = 7,
	ClientPlayChipsStore_apple_be    = 8,
	ClientPlayChipsStore_apple_dk    = 9,
	ClientPlayChipsStore_apple_ee    = 10,
	ClientPlayChipsStore_apple_es    = 11,
	ClientPlayChipsStore_apple_eu    = 12,
	ClientPlayChipsStore_apple_fr    = 13,
	ClientPlayChipsStore_apple_it    = 14,
	ClientPlayChipsStore_apple_bg    = 15,
	ClientPlayChipsStore_apple_uk    = 16,
	ClientPlayChipsStore_apple_nj    = 17,

	ClientPlayChipsStore_apple_ft_com = 18,
	ClientPlayChipsStore_apple_ft_net = 19,
	ClientPlayChipsStore_apple_ft_eu  = 20,
	ClientPlayChipsStore_apple_ft_uk  = 21,
	ClientPlayChipsStore_apple_ft_dk  = 22,
	ClientPlayChipsStore_apple_ro     = 23,
	ClientPlayChipsStore_apple_pt     = 24,
	ClientPlayChipsStore_apple_cz     = 25,
	ClientPlayChipsStore_apple_ruso   = 26,
	ClientPlayChipsStore_apple_rusopm = 27,
	ClientPlayChipsStore_apple_in     = 28, // PYR-35573
	ClientPlayChipsStore_apple_se     = 29, // PYR-75137
	ClientPlayChipsStore_apple_ft_se  = 30, // PYR-75137
	ClientPlayChipsStore_apple_br     = 31, // PYR-86551
	ClientPlayChipsStore_apple_ch     = 32, // PYR-85019
	ClientPlayChipsStore_apple_pa     = 33, // PYR-70723
    ClientPlayChipsStore_apple_arba   = 34, // PYR-114727
    ClientPlayChipsStore_apple_gr     = 35, // PYR-120944

	ClientPlayChipsStore_Last         = ClientPlayChipsStore_apple_gr // please keep up-to-date
};

enum eGreekTaxesFlags // bits in GREEKTAXES.FLAGS
{
	eGreekTaxesFlag_UserOwesAfter  = (1 << 0), // PYR-29962
	eGreekTaxesFlag_UserOwedBefore = (1 << 1), // PYR-29962
	eGreekTaxesFlag_AddedEurAcct   = (1 << 2), // PYR-29962
};

enum eXcGameStatus // xcGames.Status - aded for xcGames table indexing purpose
{
	eXcGameStatus_Started     = 0,
	eXcGameStatus_Final       = 1,
	eXcGameStatus_Interrupted = 2,
	eXcGameStatus_Restored    = 3, // PYR-43408
	eXcGameStatus_AdminRemoved= 5, // PYR-43408
	eXcGameStatus_StartFailed = 7, // PYR-43408
	eXcGameStatus_StuckCleanup = 8, // PYR-45835
	eXcGameStatus_AdminForceRemoved= 9, // PYR-48559
	eXcGameStatus_DirectUpdateToFinished = 21, // PYR-74966 Should never be set in the code
};

enum eXcGameFlags // xcGames.Flags
{
	eXcGameFlag_None                = 0,
	eXcGameFlag_Interrupted         = (1 << 0), // hub->dbm

	eXcGameFlag_LastHandIsOver      = (1 << 2),
	eXcGameFlag_LastHandInterrupted = (1 << 3),
	eXcGameFlag_Restored            = (1 << 4),
	eXcGameFlag_AdminRemoved        = (1 << 5),
	eXcGameFlag_NoHandIsOverYet     = (1 << 6), // affects how to treat an unresolved bet, is used in dbm\sql\GetMoneyInXcGameByUserStmt.sql
	eXcGameFlag_MultiPlayer         = (1 << 7), // PYR-38510, PYR-38803 hub->dbm, PYR-50205 - DK reporting ignores this flag for live dealer games (always reported as MP)
	eXcGameFlag_MayHaveJackpot      = (1 << 8), // PYR-46366
	eXcGameFlag_UsesFreeSpinBonus   = (1 << 9), // PYR-44841, for diff between FSB and userrolls please check the file "Difference between userroll and FSB.txt" attached to PYR-44841
	eXcGameFlag_LastFinishedHandWithRoll = (1 << 10), // PYR-48608
	eXcGameFlag_ForceClosedStuckGame= (1 << 11), // PYR-48559
};

enum eXcTransFlags // xcTrans.Flags
{
	eXcTransFlag_UnreservedOnGameOver = (1 << 0),
	eXcTransFlag_1stBetInHand         = (1 << 1), // hub->dbm
	eXcTransFlag_HandOver             = (1 << 2), // hub->dbm
	eXcTransFlag_InterruptedHand      = (1 << 3), // hub->dbm
	eXcTransFlag_RollUsed             = (1 << 4),
	eXcTransFlag_RollUnboundAfter     = (1 << 5),
	eXcTransFlag_RestoreHand          = (1 << 6), // PYR-42363 - mark XcTrans records until the 1st hand in over a resoration game
	eXcTransFlag_CheckNextFlagNot1stBetHand = (1 << 7), // PYR-44495, hub->dbm
	eXcTransFlag_UseBetInResponsibleGaming = (1 << 8), // PYR-44495, hub->dbm
	eXcTransFlag_AutoConversionTookPlace = (1 << 9), // PYR-48869
	eXcTransFlag_ForceClosingStuckGame = (1 << 10), // PYR-48559
	eXcTransFlag_InHouseJackpotPay     = (1 << 11), // PYR-49208, hub->dbm
	eXcTransFlag_InterruptedHandZeroUnresolved = (1 << 12), // PYR-51438 - on when casino sends interrupted hand with HandOver.unresolvedBet=0
	eXcTransFlag_RollDeltaGameIsValid = (1 << 13), // PYR-32392 - always set after 2017.Main.05. Indicates validity of XcTrans.rollDeltaGame.
	eXcTransFlag_3rdPartyJackpot       = (1 << 14), // PYR-51815
	eXcTransFlag_PayIsRefund          = (1 << 15), // PYR-70556 - applicable to cancelled bets
	eXcTransFlag_3rdPartyJackpotPay     = (1 << 16), // PYR-60671, hub->dbm
	eXcTransFlag_HandHadRefund         = (1 << 17), // PYR-77334
};

enum eXcTransType // xcTrans.TransType
{
	// amount sign indicates money direction: + towards the table, - towards the user wallet
	eXcTransType_Undefined  =  0, // impossible value
	eXcTransType_Bet        =  1, // dGame > 0  // hub->dbm
	eXcTransType_HResult    =  2, // dGame <=0  // hub->dbm
	eXcTransType_Reserve    =  3, // dRsrv > 0  // hub->dbm
	eXcTransType_Unreserve  =  4, // dRsrv < 0  // not coming from hub, xctranstype updated inside dbm
	eXcTransType_UnreserveOnGameOver = 5, // dRsrv < 0
	eXcTransType_ReturnBet  =  6, // dGame < 0, admin removes interrupted game and unresolved bet is returned back to user
	eXcTransType_BuyIn      =  7, // dRsrv > 0
	eXcTransType_CancelBuyIn=  8, // dRsrv < 0
};

//moved from dbmstruct.h
#define JANUARY_1ST_1990_STR	"1990-01-01-00.00.00"

// PYR-20741
#define _BASE_26_DICTIONARY_ "PQLAMZWOSKXNBCDJEIURHFGVTY"

enum ePlayerPointsType
{
	ePlayerPointsType_VPP = 0,
	ePlayerPointsType_CPP = 1, // PYR-29509
	ePlayerPointsType_SPP = 2, // PYR-32329
};

enum ePlayerPointsMask
{
	ePlayerPointsMask_VPP = (1 << ePlayerPointsType_VPP),
	ePlayerPointsMask_CPP = (1 << ePlayerPointsType_CPP),
	ePlayerPointsMask_SPP = (1 << ePlayerPointsType_SPP),
};

enum eTournRemoveFlags
{
	eTournRemoveFlags_CleanBouties           = (1<<0), //0x01    // [AO 2010-04-22] #14580
	eTournRemoveFlags_RemoveTournUserProps   = (1<<1), //0x02    // PYR-24907
	eTournRemoveFlags_RemoveSagTourns        = (1<<2), //0x04    // PYR-28752
	eTournRemoveFlags_RemoveActiveProperties = (1<<3), //0x08    // PYR-30443
	eTournRemoveFlags_RemoveInternalAccounts = (1<<4), //0x10    // PYR-64730
	eTournRemoveFlags_RemoveUserReentries	 = (1<<5), //0x20    // PYR-92281
};

enum eDealExcludeAmountType             //PYR-22909
{
	eDealExcludeAmountType_None    = 0,    // means no deal exclusion defined
	eDealExcludeAmountType_Money   = 1,
	eDealExcludeAmountType_Percent = 2,
};

// PYR-31829 ROLLS
#define ROLLS_ROLLNAME_LEN (32 + 1)
#define ROLLS_SUPPORTEDGAMES_LEN (3200 + 1)

enum eRewardStatus // bound to SMALLINT
{
	eRewardStatus_InDevelopment = 0,
	eRewardStatus_Active = 1,
	eRewardStatus_Cancelled = 2,
	// add more per Admin's request
};

enum eRewardIssuer // INTEGER ROLLS.IssuersMask and USERROLLS.IssuerType
{                                          // ID means..
	eRewardIssuer_Undefined    = 0,
	eRewardIssuer_Admin        = (1 << 0), // adminIntId
	eRewardIssuer_Automation   = (1 << 1), // adminIntId
	eRewardIssuer_StarCode     = (1 << 2), // ID = ??
	eRewardIssuer_Deposit      = (1 << 3), // CASHTRANS.transId
	eRewardIssuer_TournPrize   = (1 << 4), // tournId
	eRewardIssuer_VipStore     = (1 << 5), // STOREORDERS.orderId, price is in FPP
	eRewardIssuer_CashPurchase = (1 << 6), // PROMODEALS.dealId,   price is in currency
	eRewardIssuer_Ice          = (1 << 7), // ICEUSERREWARDS130x.acceptInstance
	eRewardIssuer_CasinoRaces  = (1 << 8), // PYR-82820, XC_RACE_SESSIONS.RACESESSIONID
	eRewardIssuer_MinigamePrize = (1 << 9), // PYR-40872, PYRTAB.MG_PLAYS.minigamePlayId
	eRewardIssuer_RollClearancePrize = (1 << 10), // PYR-45953, clear FreeSpinBonus into another Roll
	eRewardIssuer_ChestGameMinigamePrize = (1 << 11), // PYR-45550, PYRTAB.MG_PLAYS.minigameplayId
};

enum eRewardAssetType // PYR-45953
{
	eRewardAssetType_Undefined		= 0,
	eRewardAssetType_Cash			= 1,	// id is 0
	eRewardAssetType_Roll			= 10,	// id points to ROLLS.ROLLID column
};

// MOVED here from rewardstruct.h after BUILD1077
enum eGameVerticalFlag // PYR-38741
{
	eGameVerticalFlagNone			= 0,
	eGameVerticalFlagPoker			= 1 << 0,
	eGameVerticalFlagCasino			= 1 << 1,
	eGameVerticalFlagSportsbook		= 1 << 2,
};

// MOVED here from rewardstruct.h after BUILD1077
enum ePokerGameModeFlag // PYR-38741
{
	ePokerGameModeFlagNone			= 0,
	ePokerGameModeFlagRingGame		= 1 << 0,
	// ePokerGameModeFlagBlitz			= 1 << 1, No need for now, maybe will be used later
};

// MOVED here from rewardstruct.h after BUILD1077
enum ePokerGameTypeFlag // PYR-38741, corresponding to HandType
{
	ePokerGameTypeFlagNone				= 0,
	ePokerGameTypeFlagBadugi			= 1 << 0,
	ePokerGameTypeFlagHoldem			= 1 << 1,
	ePokerGameTypeFlag5Omaha			= 1 << 2,
	ePokerGameTypeFlagOmaha				= 1 << 3,
	ePokerGameTypeFlagDraw				= 1 << 4,
	ePokerGameTypeFlagSingleDraw27		= 1 << 5,
	ePokerGameTypeFlagStud				= 1 << 6,
	ePokerGameTypeFlagRazz				= 1 << 7,
	ePokerGameTypeFlag3Draw27			= 1 << 8,
	ePokerGameTypeFlag3DrawA5			= 1 << 9,
	ePokerGameTypeFlagCourchevel		= 1 << 10,
	ePokerGameTypeFlag6Omaha			= 1 << 11,

	ePokerGameTypeFlagMGHose			= 1 << (16 + 0),
	ePokerGameTypeFlagMGHorse			= 1 << (16 + 1),
	ePokerGameTypeFlagMGEightGame		= 1 << (16 + 2),
	ePokerGameTypeFlagMGHoldemMixed		= 1 << (16 + 3),
	ePokerGameTypeFlagMGOmahaHiLoMixed	= 1 << (16 + 4),
	ePokerGameTypeFlagMGHoldemOmahaPl	= 1 << (16 + 5),
	ePokerGameTypeFlagMGTripleStud		= 1 << (16 + 6),
	ePokerGameTypeFlagMGHoldemNlOmaha	= 1 << (16 + 7),
	ePokerGameTypeFlagMGOmahaMixed		= 1 << (16 + 8),
	// if in future there are more than 31 HandTypes we'll have to switch to defines
};

// MOVED here from rewardstruct.h after BUILD1077
enum ePotStructFlag // PYR-38741, corresponding to PotStruct
{
	ePotStructFlagNone					= 0,
	ePotStructFlagLimit					= 1 << 1,
	ePotStructFlagPotLimit				= 1 << 2,
	ePotStructFlagNoLimit				= 1 << 3,
	ePotStructFlagPotNoLimit			= 1 << 4,
	ePotStructFlagMultiGameMixed		= 1 << 10,
};

// MOVED here from rewardstruct.h after BUILD1077
enum eRollType // PYR-44841
{
	eRollType_Default = 0,
	eRollType_FreeSpinBonus = 1,
};

enum eRollFlags // bits in INTEGER ROLLS.FLAGS
{
	eRollFlag_None                 = 0,
	eRollFlag_WasIssued            = (1 <<  0),
	eRollFlag_RequiresAcceptance   = (1 <<  1),
	eRollFlag_ExpirAfterAccept     = (1 <<  2), // if set then Roll::ValidSpan starts after acceptance, not issuance
	eRollFlag_ClearedByCPP         = (1 <<  3),
	eRollFlag_ClearedByBet         = (1 <<  4),
	eRollFlag_ClearedByStakes      = (1 <<  5), // PYR-38741
// reserved (1 << 6) for yet another ClearedBy option
	eRollFlag_DebitValueAtClear    = (1 <<  7), // remove initial funds when clearing
	eRollFlag_GiveFppOnUsage       = (1 <<  8), // 0 - CPP only, 1 - CPP & FPP
	eRollFlag_ForfeitOnWithdrawal  = (1 <<  9), // PYR-36390
	eRollFlag_WasBound             = (1 << 10), // PYR-50622
	eRollFlag_EarlyCreditToUser    = (1 << 11), // PYR-61040
};

// PYR-85828 - obsoleted
enum eRollLeftoverAction // ROLLS.LeftoverAction: what to do with UserRoll leftover for when it is < ROLLS.GameMinBet and can not be merged with cash (ES, IT)
{
	eRollLeftoverAction_DoNothing      = 0,
//	eRollLeftoverAction_Void           = 1,
//	eRollLeftoverAction_ConvertToCash  = 2,
//	eRollLeftoverAction_ConvertToFpp   = 3,
};

enum eSportsbookRollBetTypeFlag // PYR-53041
{
	eSportsbookRollBetTypeFlag_None			= 0,
	eSportsbookRollBetTypeFlag_Single		= 1 << 0,
	eSportsbookRollBetTypeFlag_Accumulator	= 1 << 1,
	eSportsbookRollBetTypeFlag_System		= 1 << 2,
};

enum eUserRollIssueFlags // non-persistent - for DBM_Q_ISSUE_USERROLL
{
	eUserRollIssueFlag_None = 0,
	eUserRollIssueFlag_DebitUser  = (1 << 0), // debit user with Roll's cost
	eUserRollIssueFlag_PaidInCash = (1 << 1),
	eUserRollIssueFlag_PaidInFpp  = (1 << 2),
};

enum eUserRollAction // non-persistent - for DBM_Q_UPDATE_USERROLL_STATUS
{
	eUserRollAction_None = 0,
	eUserRollAction_AcceptIssued   = 1,
	eUserRollAction_Void           = 2, // reject if issued, void if active
	eUserRollAction_Surrender      = 3, // surrender if active
	eUserRollAction_ClearAndPay    = 4,
	eUserRollAction_Expire         = 5, // by JobsController
	eUserRollAction_Pause          = 6,
	eUserRollAction_Resume         = 7,

	eUserRollAction_ChangeStatusMask = (1 << 4) - 1,
	eUserRollAction_ChangeExpiry   = (1 << 4),
};

enum eUserRollStatus // SMALLINT UserRolls.Status
{
	eUserRollStatus_Undefined        =  0, // never in the database
	eUserRollStatus_Issued           =  1, // not Accepted yet, "Pending"
	eUserRollStatus_Rejected         =  2,
	eUserRollStatus_ACTIVE           = 10, // ACTIVE or Expired but status not yet updated
	eUserRollStatus_Cleared          = 20,
	eUserRollStatus_AdminCleared     = 21,
	eUserRollStatus_Voided           = 30,
	eUserRollStatus_VoidedByAdmin    = 31,
	eUserRollStatus_Expired          = 32,
	eUserRollStatus_VoidByMinLeftover= 33,

	eUserRollStatus_Surrendered      = 40,
	eUserRollStatus_AdminSurrendered = 41,
	eUserRollStatus_FullyUsed        = 50,

	eUserRollStatus_Last = eUserRollStatus_FullyUsed
};

enum eUserRollFlags // bits in INTEGER UserRolls.FLAGS
{
	eUserRollFlag_None         = 0,
	eUserRollFlag_Paused       = (1 << 0), // PYR-34107
	eUserRollFlag_Accepted     = (1 << 1),
	eUserRollFlag_ExternalPrice= (1 << 2), // PYR-31836 UserRoll price was calculated outside
	eUserRollFlag_WonInHouseJackpot = (1 << 3), // PYR-47194
	eUserRollFlag_WasCreditedToUser = (1 << 4), // PYR-61040, will always be 1 for URs cleared after 2018.Main.02, but may exist in not yet cleared URs
	eUserRollFlag_Won3rdPartyJackpot = (1 << 5), // PYR-60671
	eUserRollFlag_DebitUserOnAcceptance = (1 << 6), // PYR-121439
};

// end of PYR-31829 ROLLS

#define GREEK_TAX_RECEIVER "GRekHolding"
#define IT_DORMANT_BALANCE_RECEIVER "ITALDormantA"
#define DEAD_BALANCE_RECEIVER_PREFIX "PsDeadBl." // appends with 2 letters of license code or 00 if no user found
#define NJ_DONATE_BALANCE_RECEIVER "PSCharityNJ"
#define NJ_MIGRATION_BALANCE_RECEIVER "NJMigrated" // PYR-39475
#define INDIA_TAX_RECEIVER "INdiaTaxHolding" // PYR-30309 15 chars
#define USNJ_TAX_RECEIVER_FEDERAL "NJFedTaxHolding" // PYR-77762
#define USNJ_TAX_RECEIVER_STATE "NJTaxHolding" // PYR-77762
#define USPA_TAX_RECEIVER_FEDERAL "PAFedTaxHolding" // PYR-94573
#define USPA_TAX_RECEIVER_STATE "PATaxHolding" // PYR-94573

enum eTournStatsFlags     // PYR-33674
{
	eTournStatsFlags_Details = (1 << 0), // 0x01
	eTournStatsFlags_Hands   = (1 << 1), // 0x02
};

enum eAllocatedFlags // PYR-33236
{
	eAllocatedFlags_HTML5Client		= (1 << 0), // 1 - PYR-33236 - user is seated from HTML5 client
};

enum eRegisterNewUserFlags // MSG_LOBBY_REGISTER_NEW_USER
{
	eRegisterNewUserFlags_VMDevice = (1 << 0), // PYR-33232

	eRegisterNewUserFlags_Last = eRegisterNewUserFlags_VMDevice // please keep up-to-date
};

// PYR-34563 moved in
#define DEFAULT_USERS_TABLE_NAME "USERS"
#define DEFAULT_USERPROPERTIES_TABLE_NAME "USERPROPERTIES"
#define DEFAULT_EXTERNALUSERS_TABLE_NAME "EXTERNALUSERS" // PYR-56325
#define USERPROPERTIES2_TABLE_NAME "USERPROPERTIES2"	// PYR-24203

enum eRMPMLobby        //PYR-33483 (PROP_TYPE_RMPM_LOBBY)
{
	eRMPMLobby_Default = 0,
	eRMPMLobby_PM      = 1,
	eRMPMLobby_RM      = 2,
	eRMPMLobby_Both    = 3
};

enum eUserMigrationFlags
{
	eUserMigrationFlag_CancelTTickets = (1 << 0), // PYR-34786 
};

enum eExtLiabilityFlags // PYR-33749
{
	eExtLiabilityFlags_None						= 0,
	eExtLiabilityFlags_UserMoneyOnTable			= (1 << 0),
	eExtLiabilityFlags_SportsBook				= (1 << 1),
	eExtLiabilityFlags_MoneyInXcGame			= (1 << 2),
	eExtLiabilityFlags_UserChipsDaily			= (1 << 3),	
	eExtLiabilityFlags_PendingCashOut			= (1 << 4),
	eExtLiabilityFlags_PendingCashSnap			= (1 << 5),
	eExtLiabilityFlags_DailyTournUserData		= (1 << 6),

	// must be last and include all the flags
	eExtLiabilityFlags_All =
		eExtLiabilityFlags_UserMoneyOnTable
		|eExtLiabilityFlags_SportsBook
		|eExtLiabilityFlags_MoneyInXcGame
		|eExtLiabilityFlags_UserChipsDaily
		|eExtLiabilityFlags_PendingCashOut
		|eExtLiabilityFlags_PendingCashSnap
		|eExtLiabilityFlags_DailyTournUserData
};

enum eCardReportingType // PYR-34736 CardFirstUsage.CardIdType
{
	eCardReportingType_Undefined     = 0, // impossible value
	eCardReportingType_CardNumId     = 1, // VISA, NETELLER, DIRBANK etc., cardObjectId = CardNums.CardNumId
	eCardReportingType_1AcctPerUser  = 2, // empty CashTrans.Cardnum (QI, PAYSAFE, RTBT clones), then cardObjectId identifies this User and cardObjectId = 0
	eCardReportingType_1AcctPerTrans = 3, // for paysystemsWithoutCcId (vouchers: RICARICA, PSPREPAIDCARD, etc) with non-empty cardnum, cardObjectId=cashtrans.transId
};

// PYR-36096
//	When adding here, also update getBrandIdFromMobileAppId() in brandUtils.h
enum eMobileAppId
{
	eMobileAppId_Pokerstars = 0,
};

// PYR-35077 .IT Sports: AAMS History for Sports
enum eAamsTransTicketType
{
	eAamsTransTicketType_Poker		= 0,
	eAamsTransTicketType_SportBook	= 1,
};

// table ARJELEVENTS - PYR-36002
#define ARJELEVENTS_IDJOUER_LEN        (20 + 1)
#define ARJELEVENTS_EVENTTYPE_LEN      (20 + 1)
#define ARJELEVENTS_CONFIRMATIONID_LEN (40 + 1)

// table DESHVAULTREPORTS - PYR-37487
#define DESHVAULTREPORTS_FILENAME_LEN  (100 + 1)
#define DESHVAULTREPORTS_HASH_LEN      (100 + 1)
#define DESHVAULTREPORTS_MANIFESTURI_LEN (200 + 1)

// table PTCASINOHANDOVERS - PYR-41350
#define PTCASINOHANDOVERS_MESSAGEBODY_LEN (3900 + 1)
// table PTVAULTREPORTS - PYR-47103
#define PTVAULTREPORTS_FILENAME_LEN    (100 + 1)
// table PTCASINOHANDDETAILS - PYR-50066
#define PTCASINOHANDDETAILS_MESSAGEBODY_LEN (6000 + 1)


enum eArjelEventFlags // market.ARJELEVENTS.FLAGS 
{
	eArjelEventFlag_None                        = 0,
	eArjelEventFlag_ImportedFromFile            = (1 << 0), // 1
	eArjelEventFlag_SentToSafeSportsCompartment = (1 << 1), // 2 PYR-44866
};

enum eCashtransAuditObjectType // PYR-38468 CashtransAudit.ObjectType
{
	eCashtransAuditObjectType_None	    = 0, // objId = 0
	eCashtransAuditObjectType_Admin     = 1, // objectId = adminIntId
	eCashtransAuditObjectType_Rollback  = 2, // objectId = rollback.transId : for deposit RB_CONV or CRB_ABLE update. Can appear only in CashTrans4UpdAudit, never in CashTransAudit
	eCashtransAuditObjectType_Cancel    = 3, // objectId = cancellation cashtrans.transId
	eCashtransAuditObjectType_Migration = 4, // objectId = MIGRATIONINFO.migrationId
};

enum eRollGameObjType // PYR-38742 UserRollGames.ObjectType
{
	eRollGameObjType_Undefined  = 0,
	eRollGameObjType_PokerTable = 1, // objectId = Tables.TableId
	eRollGameObjType_PokerBlitz = 2, // objectId = BlitzSessions.BLITZID
//	eRollGameObjType_PokerTourn = 3,
	eRollGameObjType_Casino     =10, // objectId = XcGames.GameId, not populated in UserRollGames because the binding is stored in XcGames
	eRollGameObjType_Sportsbook =20, // objectId = SB_BETS.betId
};

enum eUserGameSessionFlags // PYR-38802 PYRDOC.USERGAMESESSIONS.FLAGS 
{
	eUserGameSessionFlags_None                = 0,
	eUserGameSessionFlags_FirstHandPlayed     = (1 << 0),
};

/// PYR-38553, 'Occupation list' in the corresponding BRD, stored in String property of UserProperties[eUserPropertyProfession]
enum ptProfessionType
{
	ptProfessionTypeNoOccupation      = 0,
	ptProfessionTypeSelfEmployed      = 1,
	ptProfessionTypeWorkerForOthers   = 2,
	ptProfessionTypeFreelancer        = 3,
	ptProfessionTypeStudent           = 4,
	ptProfessionTypeRetired           = 5,
	ptProfessionTypeTrainee           = 6,
	ptProfessionTypeUnemployed        = 7
};

/// PYR-38553, stored in Int property of UserProperties[eUserPropertyDocumentInfo]
enum ptDocumentType
{
	ptDocumentTypeUnknown             = 0, // type not defined
	ptDocumentTypeNationalCivilId     = 1,
	ptDocumentTypePassport            = 2,
	ptDocumentTypeDriversLicense      = 3,
	ptDocumentTypeOther               = 4
};

// Describes source of REFID in table SELFEXCLUDED
enum eSelfExclusionRefType 
{
	eSelfExclusionRefType_None	      = 0,
	eSelfExclusionRefType_PTBlacklist = 1,  // PYR-38554 SELFEXCLUDED.REFID = REGBLACKLIST.BLACKLISTID
	eSelfExclusionRefType_Admin		  = 2,  // PYR-38554 SELFEXCLUDED.REFID = AdminIntId
	eSelfExclusionRefType_FTConv	  = 3,  // PYR-39878 SELFEXCLUDED.REFID = FTUserIntId (playerix)
};

enum eFulltiltConversionFlags // PYR-41184 OLAP.FTCONVERSION.ConvFlags
{
	eFulltiltConversionFlag_NONE		=  0,

	eFulltiltConversionFlag_notInUse	= (1 << 0),
	eFulltiltConversionFlag_UserInfo	= (1 << 1), // address, ...
	eFulltiltConversionFlag_USD			= (1 << 2),
	eFulltiltConversionFlag_tUSD		= (1 << 3),
	eFulltiltConversionFlag_EUR			= (1 << 4),
	eFulltiltConversionFlag_tEUR		= (1 << 5),
	eFulltiltConversionFlag_PM			= (1 << 6),
	eFulltiltConversionFlag_FPP			= (1 << 7),
	eFulltiltConversionFlag_VIP			= (1 << 8),
	eFulltiltConversionFlag_DepositLim	= (1 << 9),
	eFulltiltConversionFlag_GameLim		= (1 << 10),

	eFulltiltConversionFlag_ALL			= ~0
};

enum eFulltiltConvInternalFlags // PYR-39878 OLAP.FTCONVERSION.Flags
{
	eFulltiltConvInternalFlag_NONE		=  0,
	eFulltiltConvInternalFlag_CTONHOLD	= (1 << 0),
};

enum eFullTiltConversionType	// PYR-39878 OLAP.FTCONVERSION.FTCONVTYPE
{
	eFullTiltConversionType_Unknown = 0,
	eFullTiltConversionType_New     = 1,
	eFullTiltConversionType_Pair    = 2,
	eFullTiltConversionType_Login   = 3,
	eFullTiltConversionType_Email   = 4,
	eFullTiltConversionType_Admin   = 5,
	eFullTiltConversionType_FiscalCode = 6,

	eFullTiltConversionType_Last = eFullTiltConversionType_FiscalCode, // must be kept up to date
};

enum eFullTiltUsersMigrationFlags // PYR-39878 OLAP.FT_USERS_MIGRATION.Flags
{
	eFullTiltUsersMigrationFlag_HideFromFindPlayer                 =  (1 <<  0),
	eFullTiltUsersMigrationFlag_HasDeposited					   =  (1 <<  1),
	eFullTiltUsersMigrationFlag_HasP2P							   =  (1 <<  2),
	eFullTiltUsersMigrationFlag_NoTableChat					       =  (1 <<  3),
	eFullTiltUsersMigrationFlag_ContactCSAfterExclusionExpires	   =  (1 <<  4),
	eFullTiltUsersMigrationFlag_NoCasino						   =  (1 <<  5),
	eFullTiltUsersMigrationFlag_NoJackpotTourns				       =  (1 <<  6),
	eFullTiltUsersMigrationFlag_NoMixedGames					   =  (1 <<  7),
	eFullTiltUsersMigrationFlag_NoPlayChipPurchase				   =  (1 <<  8),
	eFullTiltUsersMigrationFlag_NoRingOrRushGames				   =  (1 <<  9),
	eFullTiltUsersMigrationFlag_NoRushRingGames				       =  (1 << 10),
	eFullTiltUsersMigrationFlag_NoTournaments					   =  (1 << 11),
	eFullTiltUsersMigrationFlag_DoNotPayAffilate				   =  (1 << 12),
	eFullTiltUsersMigrationFlag_Has2FAToken				    	   =  (1 << 13),
	eFullTiltUsersMigrationFlag_Has2FAHardwareToken		    	   =  (1 << 14),
	eFullTiltUsersMigrationFlag_KYC							       =  (1 << 15),
	eFullTiltUsersMigrationFlag_NoDeposit						   =  (1 << 16),
	eFullTiltUsersMigrationFlag_NoLogin						       =  (1 << 17),
	eFullTiltUsersMigrationFlag_NoP2P							   =  (1 << 18),
	eFullTiltUsersMigrationFlag_NoRealMoneyActions				   =  (1 << 19),
	eFullTiltUsersMigrationFlag_NoTransfer						   =  (1 << 20),
	eFullTiltUsersMigrationFlag_NoWithdrawal					   =  (1 << 21),
	eFullTiltUsersMigrationFlag_IsSubsequentAccount			       =  (1 << 22),
	eFullTiltUsersMigrationFlag_LoginUsingEmailAddress			   =  (1 << 23),
	eFullTiltUsersMigrationFlag_LoginUsingPIN					   =  (1 << 24),
	eFullTiltUsersMigrationFlag_LoginUsing2FAToken				   =  (1 << 25),
	eFullTiltUsersMigrationFlag_Trapped						       =  (1 << 26),
	eFullTiltUsersMigrationFlag_Paused							   =  (1 << 27),
	eFullTiltUsersMigrationFlag_DKAuthenticationStatus 		       =  (1 << 28),
	eFullTiltUsersMigrationFlag_AgeVerifiedStatus                  =  (1 << 29),
	eFullTiltUsersMigrationFlag_DKTempClosed                       =  (1 << 30),
};

#define MAX_SELFEXCLUDE_PERIOD_DAYS    (365*60)
// moved from dbmstruct.h on cleanup
enum eSelfExcludedFlags // bitmask
{
	eSelfExcludedFlag_None				= 0,
	ePsGroupSelfExcluded				= 1 << 0,	// 1 PYR-30474
	eFtpGroupSelfExcluded				= 1 << 1,	// 2 PYR-30474
	ePermanentSelfExcluded				= 1 << 2,	// 4 PYR-35209 .FR only for now, used to indicate a special "permanent SE" case, not to be confused with a regular indeterminate "daysRequested=0" case
	eRestorePlayAfterSelfExcluded		= 1 << 3,	// 8 PYR-38635
	eRestoreChatAfterSelfExcluded		= 1 << 4,	// 16 PYR-38635
	ePtRevocationSelfExcluded			= 1 << 5,	// 32 PYR-51114
	ePtIndeterminateSelfExcludedRevoked	= 1 << 6,	// 64 PYR-84829 - can be set only if ePtRevocationSelfExcluded is set
};

// PYR-37300
#define MINIGAME_HISTORY_LEN			(512 + 1)
#define MINIGAME_NAME_LEN				(256 + 1)
#define MINIGAME_SCHEMA_LEN				(3900 + 1)
#define MINIGAME_ACCOUNTNAME_LEN		(64 + 1)
// PYR-45550
#define MINIGAME_CATALOG_NAME_LEN		(256 + 1)
#define MINIGAME_PRIZENAME_LEN			(256 + 1)
#define MINIGAME_COMMENT_LEN			(256 + 1)
#define MINIGAME_RESOURCE_LEN			(100 + 1)
#define MINIGAME_CATEGORY_LEN			(256 + 1)
#define MINIGAME_SCRIPT_NAME_LEN		(256 + 1)
#define MINIGAME_FAILURE_REASON_LEN		(256 + 1)
// PYR-50493
#define MINIGAME_ASSETNAME_LEN			(256 + 1)
//PYR-53982
#define MINIGAME_PRIZE_LIMIT_LEN (256 + 1)
//PYR-56060
#define MINIGAME_PRIZE_COUNTRIES_LEN (1024 + 1)
//PYR-53503
#define MINIGAME_COUNTRY_LEN			(256 + 1)

//PYR-62676 MG_CATALOG_PRIZE_PROPERTIES Constants
#define MINIGAME_PRIZE_PROPNAME_LEN (256 + 1)
#define MINIGAME_PRIZE_PROPVALUE_LEN (1024 + 1)
// PYR-112089
#define MINIGAME_SIDEBET_CARDS_LEN	(64 + 1)

// PYR-120470
#define MINIGAME_PROP_VALUE_LEN (256 + 1)
	

enum eFullTiltConversionSecAlertMismatchType // PYR-42107 bitmask
{
	eFullTiltConversionSecAlertMismatchType_None                   = 0,
	eFullTiltConversionSecAlertMismatchType_FirstName              = 1 << 0, // 1
	eFullTiltConversionSecAlertMismatchType_LastName               = 1 << 1, // 2
	eFullTiltConversionSecAlertMismatchType_DOB                    = 1 << 2, // 4
	eFullTiltConversionSecAlertMismatchType_AddressCountryCode     = 1 << 3, // 8
	eFullTiltConversionSecAlertMismatchType_AddressStateCode       = 1 << 4, // 16
	eFullTiltConversionSecAlertMismatchType_NationalityCountryCode = 1 << 5, // 32

	eFullTiltConversionSecAlertMismatchType_Last = eFullTiltConversionSecAlertMismatchType_NationalityCountryCode, //	must be kept up to date
};

// PYR-43408 XcInterruptedHands.RestoreStatus
enum eXcInterruptedHandStatus // tracing a chain of interrupted casino hands
{
	eXcInterruptedHandStatus_NotRestored            =  0, // init value
	eXcInterruptedHandStatus_RestoreHandStarted     =  1,
	eXcInterruptedHandStatus_RestoreHandInterrupted =  2,
	eXcInterruptedHandStatus_RestoreHandOverOK      = 10,
	eXcInterruptedHandStatus_AdminRemovedGame       = 20, // then XcReqIdRestoration is valid and points to XcTrans with eXcTransType_ReturnBet
};

// PYR-45645 GAMETICKETS
#define GAMETICKETTYPE_NAME_LEN (64 + 1)
#define GAMETICKETTYPE_DESCR_LEN (200 + 1)
#define GAMETICKETTYPE_URL_LEN (250 + 1)	// PYR-107053

enum eGameTicketTypeFlags // bits in INTEGER GameTicketTypes.FLAGS
{
	eGameTicketTypeFlag_None              = 0,
	eGameTicketTypeFlag_WasIssued         = (1 << 0),
	eGameTicketTypeFlag_BoundToGame       = (1 << 1),
	eGameTicketTypeFlag_RequireAcceptance = (1 << 2), // PYR-50765
	eGameTicketTypeFlag_FixedPrize        = (1 << 3), // = 8 PYR-121297 entry ticket for fixed prize
};

enum eGameTicketStatus // SMALLINT GameTickets.Status
{
	eGameTicketStatus_Undefined     = 0, // never in the database
	eGameTicketStatus_Issued        = 1, // not Accepted yet, "Pending"
	eGameTicketStatus_Rejected      = 2, // final
	eGameTicketStatus_ACTIVE        = 10, // become active after acceptance or on issue if it is "free" for user
	eGameTicketStatus_Voided        = 30, // final
	eGameTicketStatus_VoidedByAdmin = 31, // final
	eGameTicketStatus_Expired       = 32, // final
	eGameTicketStatus_FullyUsed     = 50, // final

	eGameTicketStatus_Last          = eGameTicketStatus_FullyUsed // please keep up-to-date
};

enum eGameTicketFlags // bits in INTEGER GameTickets.FLAGS
{
	eGameTicketFlag_None = 0,
	eGameTicketFlag_UserNotifiedOfIssuance = (1 << 0), // PYR-89409
};

enum eGameTicketIssueFlags // non-persistent - for DBM_Q_ISSUE_GAMETICKET
{
	eGameTicketIssueFlag_None = 0,
};

enum eGameTicketAction // non-persistent - for DBM_Q_UPDATE_GAMETICKET_STATUS
{
	eGameTicketAction_None = 0,
	eGameTicketAction_Accept = 1,
	eGameTicketAction_Void = 2,
	eGameTicketAction_VoidByAdmin = 3,
	eGameTicketAction_Expire = 4, // by JobsController
	eGameTicketAction_Reject = 5,
	eGameTicketAction_MarkAsUserNotifiedOfIssuance = 6, // PYR-89409
};

enum eRespawnType // PYR-25525
{
	eRespawnType_onTournStart = 0,
    eRespawnType_onFirstSit = 1,
    eRespawnType_onRegistrationEnd = 2,
    eRespawnType_onTournEnd = 3
};

enum eRelatedUserSeverity //PYR-44179
{
	eRelatedUserSeverityRegular  = 0,
	eRelatedUserSeveritySuper    = 1,
	eRelatedUserSeverityExtended = 2
};

enum eBgRequestType // PYR-37705
{
	eBgRequestType_Undefined          = 0,
	eBgRequestType_UserReg            = 1,
	eBgRequestType_UserChange         = 2,
	eBgRequestType_UserPropChange     = 3,
	eBgRequestType_PkrHand            = 4,
	eBgRequestType_PkrTourn           = 5,
	eBgRequestType_CasinoHand         = 6,
	eBgRequestType_UserUnReg          = 7,
	eBgRequestType_SportsBet          = 8,
	eBgRequestType_HorseDogRacingBet  = 9,
	eBgRequestType_PkrSideBet         = 10,
};

enum eBgRequestStatus // PYR-37705
{
	eBgRequestStatus_Unreported    = 0,
	eBgRequestStatus_Success       = 1,
	eBgRequestStatus_NonFinalError = 2,
	eBgRequestStatus_FinalError    = 3,
};

enum eBgRequestUserFlags // PYR-37705
{
	eBgRequestUserFlags_RegByBatch=  (1     ),
	eBgRequestUserFlags_RegByAdmin = (1 << 1)
};

enum eBgRequestGameFlags // PYR-37705
{
	// for future use
};

enum eRelatedUserAuditAction //PYR-41075
{
	eRelatedUserAuditActionAdded = 1,
	eRelatedUserAuditActionRemoved = 2,
};

enum ePlayMoneyAdTriggerType // PYR-45097 PLAYMONEYADEVENTS.TRIGGERTYPE
{
	ePlayMoneyAdTriggerType_Undefined = 0,
	ePlayMoneyAdTriggerType_Cashier = 1,
	ePlayMoneyAdTriggerType_BuyIn_Table = 2,
	ePlayMoneyAdTriggerType_BuyIn_Tourn = 3,
	ePlayMoneyAdTriggerType_BuyIn_Blitz = 4,
	ePlayMoneyAdTriggerType_Lobby = 5,

	ePlayMoneyAdTriggerType_Last = ePlayMoneyAdTriggerType_Lobby // please keep up-to-date
};

enum ePlayMoneyAdEventStatus // PYR-45097 PLAYMONEYADEVENTS.STATUS
{
	ePlayMoneyAdEventStatus_Undefined = 0,
	ePlayMoneyAdEventStatus_Started = 1,
	ePlayMoneyAdEventStatus_Credited = 2,
	ePlayMoneyAdEventStatus_WatchedIncomplete = 3,
	ePlayMoneyAdEventStatus_Expired = 4,
	ePlayMoneyAdEventStatus_WatchedError = 5,
};

// PLAYMONEYADEVENTS table (PYR-45097)
#define PLAYMONEYADEVENT_XTRANSSTR_LEN	(70 + 1) // Increased from 40 to 70 in scope of PYR-95749
#define PLAYMONEYADEVENT_INFOSTR_LEN	(256 + 1)

enum  ePendingActionStatus // PYR-44386 USERPENDINGACTIONS.STATUS
{
	ePendingActionStatus_None       = 0, // NEVER exists in DB
	ePendingActionStatus_Pending    = 1, //
	// final statuses. Not exist on ROOT. Once the status changed to final , the record deleted from ROOT.
	ePendingActionStatus_Issued     = 10, // USERPENDINGACTIONS.TransId != 0, ErrCode = 0; action was issued to user account during unarchived operation
	ePendingActionStatus_Canceled   = 11, // action was canceled by admin / automation tool USERPENDINGACTIONS.TransId = 0
	ePendingActionStatus_Expired    = 12, // action expired USERPENDINGACTIONS.TransId = 0
	ePendingActionStatus_Failed     = 13, // issue process failed, see error code in db, USERPENDINGACTIONS.TransId = 0
};

// table USERPENDINGACTIONS (PYR-44386)
#define USERPENDINGACTION_STRVALUE_LEN (255 + 1)

enum eUserPendingActionFlags // bits in INTEGER USERPENDINGACTIONS.FLAGS
{
	eUserPendingActionFlag_None = 0,
};

enum ePendingActionType // PYR-44386 INTEGER USERPENDINGACTIONS.AssetType 
{                                              // ASSETTYPEID means                | USERASSETID means
	ePendingActionType_Undefined          = 0, // never exists in DB               |
	ePendingActionType_Cash               = 1, // id is 0                          | CashTrans.TransId
	ePendingActionType_PlayMoney          = 2, // id is 0                          | id is 0
	ePendingActionType_FPP                = 3, // id is 0                          | id is 0
	ePendingActionType_TChips             = 4, // id is 0                          | id is 0
	ePendingActionType_InsertTrackingCode = 5, // id is 0                          | id is 0
	ePendingActionType_DeleteTrackingCode = 6, // id is 0                          | id is 0
	ePendingActionType_UserComment        = 7, // id is 0                          | UserComments.CommentId
	ePendingActionType_Roll               = 10,// ROLLS.ROLLID                     | UserRolls.UserRollId
	ePendingActionType_Bonus              = 11,// BONUSES2.BONUSID column          | UserBonus.UserBonusId
	ePendingActionType_TournTicket        = 12,// TicketTypes.TicketTypeId         | TournTickets.TicketId
	ePendingActionType_GameTicket         = 13,// GAMETICKETTYPES.GAMETICKETTYPEID | GameTickets.GameTicketId

	// remember to add new enum to function isActionTypeCorrect
};


// PYR-49208
enum eJackpotFlags
{
	eJackpotFlag_UseResetValue     = 1 << 0,
	eJackpotFlag_IsMultiCurrency   = 1 << 1,
};

// PYR-49208
enum eJackpotEventType
{
	eJackpotEventType_Undefined		= 0,  // never exists in DB
	eJackpotEventType_JpUserBet		= 1,  // PSBO_MSG_Q_JACKPOT_BET -> DBM_Q_XC_JACKPOT_USERBET, JP contribution from user
	eJackpotEventType_JpWithdrawal	= 2,  // PSBO_MSG_Q_JACKPOT_PAY -> DBM_Q_XC_JACKPOT_WITHDRAWAL
	eJackpotEventType_JpUserPay		= 3,  // PSBO_MSG_Q_HAND_RESULT with pay.reason=2 -> DBM_Q_XC_TRANSACTION

	eJackpotEventType_3rdPartyJpUserBet	= 11,  // PYR-51815 PSBO_MSG_Q_HAND_RESULT with handOver.jackpots.size() > 0 -> DBM_Q_XC_TRANSACTION
	eJackpotEventType_3rdPartyJpUserPay	= 13,  // PYR-51815 PSBO_MSG_Q_HAND_RESULT with handOver.jackpots.size() > 0 -> DBM_Q_XC_TRANSACTION

	eJackpotEventType_ManualMoveIn		= 101,  // PYR-90600
	eJackpotEventType_ManualMoveOut		= 102,  // PYR-90600
};

// PYR-50357
enum eUserCommentsFlags // INTEGER USERCOMMENTS.FLAGS
{
	eUserCommentsFlag_None			= 0,
	eUserCommentsFlag_VipManager	= (1 << 0), // Comment visible to VipManager ("VipManagerAccess")
};

// PYR-50412
enum eSocialmediaStatus // SMALLINT SOCIALMEDIA.STATUS
{
	eSocialmediaStatus_Archived = 0,
	eSocialmediaStatus_Active	= 1,
};

// PYR-50344
enum eFavoriteStatus // SMALLINT FAVORITES.STATUS
{
	eFavoriteStatus_Archived = 0,
	eFavoriteStatus_Active	= 1,
};

// PYR-51733
enum eUserBlockedCardFlags // INTEGER USERCARDS.FLAGS
{
	eUserBlockedCardFlag_None				= 0,
	// state
	eUserBlockedCardFlag_BlockedForAllUsers	= (1 << 0),
	eUserBlockedCardFlag_AllowCashouts		= (1 << 1),

	// reason
	eUserBlockedCardFlag_AtSelfExclusion	= (1 << 8),
	eUserBlockedCardFlag_AtNocReturn		= (1 << 9), // PYR-16054
	eUserBlockedCardFlag_AtChargeback		= (1 << 10),
	eUserBlockedCardFlag_AtInvalidation		= (1 << 11), // PYR-58539
};

// PYR-50202
//-----------------------------------|
// PGADMSG | BONUSPART |   PROPSTR   |
//---------|-----------|-------------|
//    8    | act status|   reason    |
//---------|-----------|-------------|
//   26    |  limit    |             |
//---------|-----------|-------------|
//         |           |             |
//-----------------------------------|
enum ePgadTransactMsg // SMALLINT PGADTRANSACTS.PGADMSG
{
	ePgadTransactMsg_Undefined                  = -1, // should never appear in DB
	ePgadTransactMsg_Obsolete                   =  0, // for backward compatibility, default DB value for records before 2017.Main.05
	// values are based on PGAD protocol 1.7 documentation (4. Structure of messages)
	// follow the same convention when adding new values
	ePgadTransactMsg_GamingTransaction          =  3, // 4.3, see comments on PGADTRANSACTS columns
	ePgadTransactMsg_BonusMovement              =  4, // 4.4, see comments on PGADTRANSACTS columns
	ePgadTransactMsg_GetAccountStatus           =  8, // 4.8 PYR-66028
	ePgadTransactMsg_UpdateAccountData          = 11, // 4.11, PYR-51313 TRANSID=0,BALANCEDELTA=0,BALANCEAFTER=0,BONUSPART=0
	ePgadTransactMsg_TransactionCancel          = 14, // 4.14, see comments on PGADTRANSACTS columns
	ePgadTransactMsg_DormantAccount             = 19, // 4.19, TRANSID=TRANSACTS.TRANSID(eAccountClosed),BALANCEDELTA=0,BALANCEAFTER=chips-owedChips,BONUSPART=0
	ePgadTransactMsg_DepositLimitsUpdate        = 22, // 4.22, PYR-68104 TRANSID=LIMITSAUDIT.REQUESTID,BALANCEDELTA=0,BALANCEAFTER=0,BONUSPART=limitAmount
	ePgadTransactMsg_PlayerRegistration         = 26, // 4.26, PYR-68104 TRANSID=LIMITSAUDIT.REQUESTID,BALANCEDELTA=0,BALANCEAFTER=0,BONUSPART=limitAmount
};

enum eJobsCtrlJobType // INTEGER JOBSCTRLLOG.JOBTYPE
{
	eJobsCtrlJobType_Undefined						=  0,
	eJobsCtrlJobType_CashoutAutoComplete			=  1,
	eJobsCtrlJobType_WarnHeldTrans					=  2,
	eJobsCtrlJobType_RejectHeldTrans                =  3,
	eJobsCtrlJobType_ArjelCloseAccounts             =  4,
	eJobsCtrlJobType_CloseSelfExcluded              =  5,
	eJobsCtrlJobType_CloseInactive                  =  6,
	eJobsCtrlJobType_CloseNotVerified               =  7,
	eJobsCtrlJobType_ItCloseInactive                =  8,
	eJobsCtrlJobType_ItEmailInactive                =  9,
	eJobsCtrlJobType_EsIdAndBanCheck                = 10,
	eJobsCtrlJobType_VoidExpiredUserRolls           = 11,
	eJobsCtrlJobType_RegisterBGPlayers              = 12,
	eJobsCtrlJobType_VoidExpiredGameTickets         = 13,
	eJobsCtrlJobType_VoidExpiredUserPendingActions  = 14,
	eJobsCtrlJobType_ItSendUpdateAccDataFromFile    = 15, // PYR-51313
	eJobsCtrlJobType_ItMigrateOldLicense            = 16, // PYR-55504
	eJobsCtrlJobType_CalcTournTicketTotals          = 17, // PYR-65782
	eJobsCtrlJobType_ItReportDepositLimits          = 18, // PYR-67700
	eJobsCtrlJobType_ItCheckAccountStatus           = 19, // PYR-66028
	eJobsCtrlJobType_ArjelReportPendingOfflineMessages = 20, // PYR-61891
	eJobsCtrlJobType_BeTokenMigration               = 21, // PYR-70907
};

enum eJobsCtrlLogFlags // INTEGER JOBSCTRLLOG.FLAGS
{
	eJobsCtrlLogFlags_None		               = 0,
	eJobsCtrlLogFlags_NotFinalized             = (1 << 0),
};

enum eJobsCtrlResultType // INTEGER JOBSCTRLRESULTS.RESULTTYPE
{
	eJobsCtrlResultType_Undefined                              = 0,
	// ItCloseInactive results
	eJobsCtrlResultType_ItCloseInactive_Processed              = 1001, // int - total number of processed accounts (including active, ignored, failed, closed)
	eJobsCtrlResultType_ItCloseInactive_Active                 = 1002, // int - total number of active accounts
	eJobsCtrlResultType_ItCloseInactive_PgadMsg419Sent         = 1003, // int - total number of sent to PGAD
	eJobsCtrlResultType_ItCloseInactive_Closed                 = 1004, // int - total number of closed accounts
	eJobsCtrlResultType_ItCloseInactive_FailedBalance          = 1005, // int - total balance of accounts for those close procedure failed with positive balance
	eJobsCtrlResultType_ItCloseInactive_NegativeBalance        = 1006, // int - total absolute balance of accounts for those close procedure failed with negative balance
	eJobsCtrlResultType_ItCloseInactive_ClosedBalance          = 1007, // int - total balance of closed accounts
	eJobsCtrlResultType_ItCloseInactive_IgnoredDueToPrevErrors = 1008, // int - total number of ignored accounts (not sent to PGAD)
	eJobsCtrlResultType_ItCloseInactive_PgadError              = 1009, // int - count, str - PGAD error code
	eJobsCtrlResultType_ItCloseInactive_PsError                = 1010, // int - count, str - PokerStars error code

	// start next set of results above this line from 2001 and update comment (+1000)
};

#define JOBSCTRLRESULTS_RESULTVALUESTR_LEN (200 + 1) // table JOBSCTRLRESULTS

enum eDbtAccountFlags // PYR-55212 INTEGER PYRFIN.DBTACCOUNTS.FLAGS
{
	eDbtAccountFlags_None = 0,
	eDbtAccountFlags_accountVerified       = 1 << 0, // PYR-47572, dynamic flag: set if the accountId exists in VERIFIEDACCOUNTS table
	eDbtAccountFlags_RegisteredWithProcessor = 1 << 1, // PYR-55212, account was successfully registered in an external processor (CashFree)
	eDbtAccountFlags_unRegisterationFailed = 1 << 2, // PYR-55212, account invalidation failed in CashFree
	eDbtAccountFlags_RegisterationFailed = 1 << 3, // PYR-55212, account registration failed in CashFree
	eDbtAccountFlags_AdminCreated = 1 << 4, // PYR-76119, set if the account was created by Admin
};

enum eAvatarGameType // PYR-54689 SMALLINT ROOT.BRANDIMAGES.AVATARGAMETYPE
{
	eAvatarGameType_Undefined	= 0,
	eAvatarGameType_Storm		= 1,

	// !!! Keep eAvatarGameType_Last updated
	eAvatarGameType_Last		= eAvatarGameType_Storm,

	eAvatarGameType_Invalid		= -1, // client only, not to be used in server code
};

// PYR-56207 - Used as int bit mask by eUserPropertyOptionsMadeByUser property.
// Meaning: corresponding bit (for example 'priv3NoMailListCasino', etc.) was changed by user/admin explicitly
enum eOptionsMadeByUser
{
	eOptionsMadeByUser_None		= 0,
	eOptionsMadeByUser_Casino	= 1<<0, // Referring to priv3NoMailListCasino
	eOptionsMadeByUser_Sports	= 1<<1, // Referring to priv3NoMailListSports
	eOptionsMadeByUser_All		= eOptionsMadeByUser_Casino | eOptionsMadeByUser_Sports,
};

// PYR-56387 - indicates if a specific user property has been provided by a user,
// or if a user chose not to disclose it. There's no 'None' or 'Undefined' status for
// backwards compatibility reasons, existing properties get 'Provided' status automatically.
enum eUserPropertySubmissionStatus
{
	eUserPropertySubmissionStatus_Provided = 0,
	eUserPropertySubmissionStatus_ChoseNotToProvide = 1,
};

// PYR-54304
enum eIndiaAnnualTaxesFlags // bits in INDIATAXESANNUAL.FLAGS
{
};

// PYR-60681
enum eNonRegLoginAttemptFlags	//	NONREGLOGINATTEMPTS.FLAGS
{
	eNonRegLoginAttemptFlags_installId2IsPrimary  = 1<<0,
	eNonRegLoginAttemptFlags_loginViaEMailAttempt = 1<<1,
};

//	PYR-60673
//	LOGINTEMPDATA.REASONCODE
enum eTempPinReasonCode
{
	eTempPinReasonCode_undefined = 0,
	eTempPinReasonCode_failedLogins = 1,
	eTempPinReasonCode_newDevice = 2,
	eTempPinReasonCode_adminMassAssign = 3,		//	PYR-68134
	eTempPinReasonCode_everyLogin = 4,			//	PYR-76895

	//	To add a new enum:
	//		1.) Update eTempPinReasonCode_last
	//		2.) Update CUserEx::getTempPinReasonCodeFromFlags
	//		3.) Update TempPinReasonCodeUtils

	eTempPinReasonCode_last = eTempPinReasonCode_everyLogin			//	Keep up to date
};

// PYR-61358 Data values to be used to overwrite anonymized fields
#define GDPR_CLEANUP_BLANK_STR "*a*b*"
#define GDPR_CLEANUP_BLANK_DATE_YEAR 1911
#define GDPR_CLEANUP_BLANK_DATE_MONTH 11
#define GDPR_CLEANUP_BLANK_DATE_DAY 11
#define GDPR_CLEANUP_BLANK_DATE_STR "1911-11-11"
#define GDPR_CLEANUP_BLANK_STR2 "*a"

#define NOT_ALLOWED_FOR_GDPR_READY_ERR_STR "Action cannot proceed, since the account has GDPR ready flag."

//	PYR-52518
#define INVALID_ASIA_OPID		0


enum eAsiaReportEventsRefType // PYR-64516 ASIAREPORTEVENTS.REFTYPE
{
	eAsiaReportEventsRefType_Undefined = 0,
	eAsiaReportEventsRefType_Transacts = 1,
	eAsiaReportEventsRefType_TTicketsAudit = 2,
	eAsiaReportEventsRefType_CashTrans = 3,

	// add new values above this line
};

// PYR-69503 - Used as int bit mask by eUserPropertyFraudulentAccountReasons property.
// See DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG and DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG messages.
enum eFraudulentAccountReasons
{
	eFraudulentAccountReasons_None					= 0,	// 0x00
	eFraudulentAccountReasons_BonusAbuse			= 1<<0,	// 0x01
	eFraudulentAccountReasons_FinancialFraud		= 1<<1,	// 0x02
	eFraudulentAccountReasons_MaliciousMAA			= 1<<2,	// 0x04
	eFraudulentAccountReasons_IdentifyTheft			= 1<<3,	// 0x08
	eFraudulentAccountReasons_DataMining			= 1<<4,	// 0x10
	eFraudulentAccountReasons_RelatedToFraudAccount	= 1<<5,	// 0x20
	eFraudulentAccountReasons_LocationAbuse			= 1<<6, // 0x40

	eFraudulentAccountReasons_Min					= eFraudulentAccountReasons_None,
	// #IMPORTANT: Keep eFraudulentAccountReasons_Max updated
	eFraudulentAccountReasons_Max					= (eFraudulentAccountReasons_LocationAbuse * 2 - 1),
};

// flags for GameUserIn structure defined in gamestructs.h
namespace GameUserInFlags
{
	enum GameUserInFlags
	{
		eIsPLOGame      = 1 << 0,      // 0x00
		eIsUnfoldGame   = 1 << 1,      // 0x01
	};
};

// PYR-65734
enum eHandExtraPotFlags  // bit mask
{
	eHandExtraPotFlags_Pot_Should_Return_To_Players = (1 << 0),   // 0x01
};

enum eBanByRegulatorReasons // bit mask, used in UserProperty[eUserPropertyBanByRegulatorReasons].propertyInt
{
	eBanByRegulatorReasons_None             = 0,
	eBanByRegulatorReasons_Default          = 1 << 0, // added automatically if (previous) reason is unknown
	eBanByRegulatorReasons_DormantSuspended = 1 << 1, // PYR-77866 account suspended due to inactivity
	eBanByRegulatorReasons_DormantCanceled  = 1 << 2, // PYR-77866 account cancelled
	eBanByRegulatorReasons_AccountSuspended = 1 << 3, // PYR-83446
	eBanByRegulatorReasons_AccountSuspendedNoCO = 1 << 4, // PYR-83446

	eBanByRegulatorReasons_AccountSuspendedAny = (eBanByRegulatorReasons_AccountSuspended | eBanByRegulatorReasons_AccountSuspendedNoCO),
	eBanByRegulatorReasons_ALL = ~0,
};

// PYR-84531
enum eSbReversibleTaxesFlags // bits in UINT32 SBReversibleTaxes.FLAGS
{
	sbReversableTaxesFlag_None                       = 0,
	sbReversableTaxesFlag_HasFullCashout             = 1 << 0, // 0x01
	sbReversableTaxesFlag_HasOnePartialCashout       = 1 << 1, // 0x02
	sbReversableTaxesFlag_HasMultiplePartialCashout  = 1 << 2, // 0x04
	sbReversableTaxesFlag_HasOnePartialUnsettle      = 1 << 3, // 0x08
	sbReversableTaxesFlag_HasMultiplePartialUnsettle = 1 << 4, // 0x10
};

// PYR-76141
#define SUGGESTEDDEPOSIT_NAME_LEN (64 + 1)
#define SUGGESTEDDEPOSIT_DESCRIPTION_LEN (256 + 1)
#define SUGGESTEDDEPOSIT_COUNTRIES_LEN (256 + 1)
#define SUGGESTEDDEPOSIT_LICENSES_LEN (256 + 1)
#define SUGGESTEDDEPOSIT_USERFILTER_LEN (256 + 1)

#define SUGGESTEDDEPOSITAMOUNT_VALUES_LEN (64 + 1)

enum eSuggestedDepositFlags // bits in INTEGER SuggestedDeposit.FLAGS
{
	eSuggestedDepositFlag_None = 0,
	eSuggestedDepositFlag_RegularDeposit = (1 << 0),
	eSuggestedDepositFlag_FastDeposit = (1 << 1),
};
enum eSuggestedDepositStatus // SUGGEDTEDDEPOSITS.STATUS
{
	eSuggestedDepositStatus_InDevelopment = 0,
	eSuggestedDepositStatus_Active = 1,
	eSuggestedDepositStatus_Cancelled = 2,
	// add more per Admin's request: Expired, etc.
};
enum eSuggestedDepositTypes
{
	eSuggestedDepositTypes_FTD = 0, // first time deposit
	eSuggestedDepositTypes_RLD = 1, // reload deposit
	eSuggestedDepositTypes_OLD = 2, // old reload deposit
};

// PYR-105216
enum eTransferRecoveryActionType // PAYMENTRECOVERY.ACTIONTYPE
{
	eTransferRecoveryActionType_Undefined = 0,
	eTransferRecoveryActionType_Init      = 1,
	eTransferRecoveryActionType_Approval  = 2,
	eTransferRecoveryActionType_Rejection = 3,
	eTransferRecoveryActionType_DeadBalance = 4, // PYR-105425
	eTransferRecoveryActionType_AllAssets   = 5, // PYR-105421
	eTransferRecoveryActionType_FullAccount = 6, // PYR-119847
};
enum ePaymentRecoveryFlags // PAYMENTRECOVERY.FLAGS
{
	ePaymentRecoveryFlags_None                               = 0,
	ePaymentRecoveryFlags_AwareOfBan                         = 1 << 0,
	ePaymentRecoveryFlags_Approval_FailWithComment           = 1 << 1,
	ePaymentRecoveryFlags_Approval_Success                   = 1 << 2,
	ePaymentRecoveryFlags_Approval_ReceiverRmNokHeldTransfer = 1 << 3,
	ePaymentRecoveryFlags_Init_OkForAutoTransfer             = 1 << 4,
	ePaymentRecoveryFlags_Init_TransferExceedsLimitIn        = 1 << 5,
};

// PYR-109066
enum eBlacklistMonitorId // BLACKLISTMONITORS.BLACKLISTMONITORID
{
	eBlacklistMonitorId_Undefined = 0,
	eBlacklistMonitorId_SwedenBulkCheck = 1,
	eBlacklistMonitorId_SwedenUserCheck = 2,
	eBlacklistMonitorId_PortugalBulkCheck = 3, // PYR-109115
	eBlacklistMonitorId_BelgiumUserCheck = 4,
	eBlackListMonitorId_UkUserCheck = 5, // PYR-109112
	eBlacklistMonitorId_SpainBanBulkCheck = 6,
	eBlacklistMonitorId_SpainUserCheck = 7,
	eBlacklistMonitorId_SpainDeceasedBulkCheck = 8,
	eBlacklistMonitorId_ItalyBulkCheck = 9,
	eBlacklistMonitorId_DkMarketingCommBulkCheck = 10,
	eBlacklistMonitorId_DkUserCheck = 11,
	eBlacklistMonitorId_End
};

// PYR-114342
enum eLimitHitType // LIMITHITS.HITTYPE
{
	eLimitHitType_Undefined = 0,
	eLimitHitType_ThresholdReached = 1,
	eLimitHitType_ThresholdUpdated = 2,
};

// PYR-114342
enum eLimitCategory // LIMITHITS.LIMITCATEGORY
{
	eLimitCategory_Undefined = 0,
	eLimitCategory_DepositLimit = 1,
	eLimitCategory_TableLimit = 2,
};

// PYR-113183
enum eRegulatoryDepositLimitStatus // USERPROPERTIES.PROPERTYINT for PROPERTYID=eUserPropertyRegulatoryDepositLimitStatus=138
{
	eRegulatoryDepositLimitStatus_Undefined = 0,
	eRegulatoryDepositLimitStatus_OptIn = 1,
	eRegulatoryDepositLimitStatus_OptOut = 2,
	eRegulatoryDepositLimitStatus_BelowRegLimit = 3, // for server use only
};

#endif	//dbm_h_included
