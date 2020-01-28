#ifndef tsclientshared_h_included
#define tsclientshared_h_included

//don't change, append only
enum eClientEmailCategory
{
	eClientEmailCategory_None = 0,
	eClientEmailCategory_Account,
	eClientEmailCategory_Cashier,
	eClientEmailCategory_Poker,
	eClientEmailCategory_Tournaments,
	eClientEmailCategory_VipClub,
	eClientEmailCategory_Software,
	eClientEmailCategory_Security,
	eClientEmailCategory_HomeGames,
	eClientEmailCategory_Promotions,
	eClientEmailCategory_SoftwareTechnical,

	eClientEmailCategory_Last = eClientEmailCategory_SoftwareTechnical // must be kept up to date
};

enum eClientEmailSubCategoryNone
{
	eClientEmailSubCategoryNone_None = 0,
};

enum eClientEmailSubCategoryAccount
{
	eClientEmailSubCategoryAccount_None = 0,
	eClientEmailSubCategoryAccount_Creating,
	eClientEmailSubCategoryAccount_ValidationCode,
	eClientEmailSubCategoryAccount_Images,
	eClientEmailSubCategoryAccount_General,
	eClientEmailSubCategoryAccount_Password,
	eClientEmailSubCategoryAccount_ResponsibleGaming,
	eClientEmailSubCategoryAccount_Chat,
	eClientEmailSubCategoryAccount_Other,
	eClientEmailSubCategoryAccount_Validation,	// 2010-08-25 PYR-10884
	eClientEmailSubCategoryAccount_ForgotUserId,
	eClientEmailSubCategoryAccount_Complaint,	// PYR-26090
};

enum eClientEmailSubCategoryCashier
{
	eClientEmailSubCategoryCashier_None = 0,
	eClientEmailSubCategoryCashier_Deposits,
	eClientEmailSubCategoryCashier_Help,
	eClientEmailSubCategoryCashier_Cashouts,
	eClientEmailSubCategoryCashier_Other,
	// 2009-11-18 pyr-12913
	eClientEmailSubCategoryCashier_BlockedPaymentMethod,
	eClientEmailSubCategoryCashier_RealMoneyTransfer,
	eClientEmailSubCategoryCashier_TransferLimits,
	// 2010-01-29 pyr-13630
	eClientEmailSubCategoryCashier_DepositLimits,
	eClientEmailSubCategoryCashier_CurrencyConvLimits,
	eClientEmailSubCategoryCashier_TroubleMakingDeposits,
	eClientEmailSubCategoryCashier_InstantEcheckQuestions,
	eClientEmailSubCategoryCashier_DepositBonus,
	eClientEmailSubCategoryCashier_UpdateOrRemovalOfPaymentMethodDetails,
	eClientEmailSubCategoryCashier_TransactionProcessingAndCurrencyConversion,
	eClientEmailSubCategoryCashier_ReturnedTransactions,
	eClientEmailSubCategoryCashier_Complaint,	// PYR-26090
};

enum eClientEmailSubCategoryPoker
{
	eClientEmailSubCategoryPoker_None = 0,
	eClientEmailSubCategoryPoker_Rules,
	eClientEmailSubCategoryPoker_HandHistory,
	eClientEmailSubCategoryPoker_HandReading,
	eClientEmailSubCategoryPoker_Other,
	eClientEmailSubCategoryPoker_GameInformation,
	eClientEmailSubCategoryPoker_CardRoomPolicy,
	eClientEmailSubCategoryPoker_RingGameRules,
	eClientEmailSubCategoryPoker_TournamentRules,
	eClientEmailSubCategoryPoker_TournamentInformation,
	eClientEmailSubCategoryPoker_TournamentSchedule,
	eClientEmailSubCategoryPoker_TournamentProblems,
	eClientEmailSubCategoryPoker_TournamentLeaderboard,
	eClientEmailSubCategoryPoker_TournamentDealOrChop,
	eClientEmailSubCategoryPoker_HandRanking,
	eClientEmailSubCategoryPoker_PokerTerms,
	eClientEmailSubCategoryPoker_PokerSchool,
	eClientEmailSubCategoryPoker_PokerStrategy,
	eClientEmailSubCategoryPoker_Complaint,	// PYR-26090
};

enum eClientEmailSubCategoryTournaments
{
	eClientEmailSubCategoryTournaments_None = 0,
	eClientEmailSubCategoryTournaments_Information,
	eClientEmailSubCategoryTournaments_Rules,
	eClientEmailSubCategoryTournaments_LeaderBoard,
	eClientEmailSubCategoryTournaments_Problems,
	eClientEmailSubCategoryTournaments_Other,
	eClientEmailSubCategoryTournaments_Complaint,	// PYR-26090
};

enum eClientEmailSubCategoryVipClub
{
	eClientEmailSubCategoryVipClub_None = 0,
	eClientEmailSubCategoryVipClub_Orders,
	eClientEmailSubCategoryVipClub_FPP,
	eClientEmailSubCategoryVipClub_VPP,
	eClientEmailSubCategoryVipClub_Benefits,
	eClientEmailSubCategoryVipClub_Other,
	eClientEmailSubCategoryPromotions_VipClubPlayerPointsVppOrFpp,
	eClientEmailSubCategoryPromotions_VipClubTiers,
	eClientEmailSubCategoryPromotions_VipClubRewards,
	eClientEmailSubCategoryPromotions_VipClubCoupon,
	eClientEmailSubCategoryPromotions_VipClubStellarRewards,
	eClientEmailSubCategoryPromotions_VipClubTournaments,
	eClientEmailSubCategoryPromotions_LiveEvents,
	eClientEmailSubCategoryVipClub_Complaint,	// PYR-26090
};

typedef eClientEmailSubCategoryVipClub eClientEmailSubCategoryPromotions;

enum eClientEmailSubCategorySoftware
{
	eClientEmailSubCategorySoftware_None = 0,
	eClientEmailSubCategorySoftware_General,
	eClientEmailSubCategorySoftware_Settings,
	eClientEmailSubCategorySoftware_Connect,
	eClientEmailSubCategorySoftware_Disconnections,
	eClientEmailSubCategorySoftware_Issues,
	eClientEmailSubCategorySoftware_Other,
	eClientEmailSubCategorySoftware_SpecialFeatures,
	eClientEmailSubCategorySoftware_MissingOptions,
	eClientEmailSubCategorySoftware_Complaint,	// PYR-26090
};

enum eClientEmailSubCategorySecurity
{
	eClientEmailSubCategorySecurity_None = 0,
	eClientEmailSubCategorySecurity_Collusion,
	eClientEmailSubCategorySecurity_ProhibitedSoftware,
	eClientEmailSubCategorySecurity_MultipleAccounts,
	eClientEmailSubCategorySecurity_AgeVerification,
	eClientEmailSubCategorySecurity_BlockedPaymentMethod,	//moved to cashier
	eClientEmailSubCategorySecurity_Documents,
	eClientEmailSubCategorySecurity_ItalianContract,
	eClientEmailSubCategorySecurity_HackClaim,
	eClientEmailSubCategorySecurity_RealMoneyTransfer,		//moved to cashier
	eClientEmailSubCategorySecurity_TransferLimits,			//moved to cashier
	eClientEmailSubCategorySecurity_Other,
	eClientEmailSubCategorySecurity_GameSecurity,
	eClientEmailSubCategorySecurity_Complaint,	// PYR-26090
};

enum eClientEmailSubCategoryHomeGames
{
	eClientEmailSubCategoryHomeGames_None,
	eClientEmailSubCategoryHomeGames_CreateClub,
	eClientEmailSubCategoryHomeGames_JoinClub,
	eClientEmailSubCategoryHomeGames_Images,
	eClientEmailSubCategoryHomeGames_ClubStandings,
	eClientEmailSubCategoryHomeGames_Passwords,
	eClientEmailSubCategoryHomeGames_PlayerStatistics,
	eClientEmailSubCategoryHomeGames_Members,
	eClientEmailSubCategoryHomeGames_RingGames,
	eClientEmailSubCategoryHomeGames_Tournaments,
	eClientEmailSubCategoryHomeGames_Other,
	eClientEmailSubCategoryHomeGames_InvitationCodes,
	eClientEmailSubCategoryHomeGames_Complaint,	// PYR-26090
};

///////////////////////////////////////////////////////////////////////////
// PYR-16722: document types

// constants defined by AAMS
enum eDocumentTypeCat
{
	eDocumentTypeCat_NationalIdentityCard			= 1,
	eDocumentTypeCat_DriversLicense					= 2,
	eDocumentTypeCat_Passport						= 3,
	eDocumentTypeCat_PersonalCardPubEmployees		= 4,
	eDocumentTypeCat_PersonalCardPubEmployeesFam	= 5,
	eDocumentTypeCat_GunLicense						= 6,
	eDocumentTypeCat_Other							= 10,
};

// required based on UI requirements from PokerStars
enum eDocumentTypeSubCatDriversLicense
{
	eDocumentTypeSubCatDriversLicense_MinistryOfTransport			= 1,
	eDocumentTypeSubCatDriversLicense_VehicleRegistrationAuthority	= 2,
	eDocumentTypeSubCatDriversLicense_PoliceAuthorityPrefettura		= 3,
};

enum eDocumentTypeSubCatPassport
{
	eDocumentTypeSubCatPassport_PoliceAuthorityQuestura		= 1,
};

enum eDocumentTypeSubCatNationalIdentityCard
{
	eDocumentTypeSubCatNationalIdentityCard_CouncilTownHall	= 1,
};

enum eDocumentTypeSubCatPersonalCardPubEmployees
{
	eDocumentTypeSubCatPersonalCardPubEmployees_CouncilTownHall					= 1,
	eDocumentTypeSubCatPersonalCardPubEmployees_MinistryOfTransport				= 2,
	eDocumentTypeSubCatPersonalCardPubEmployees_VehicleRegistrationAuthority	= 3,
	eDocumentTypeSubCatPersonalCardPubEmployees_PoliceAuthorityPrefettura		= 4,
	eDocumentTypeSubCatPersonalCardPubEmployees_PoliceAuthorityQuestura			= 5,
};

enum eDocumentTypeSubCatPersonalCardPubEmployeesFam
{
	eDocumentTypeSubCatPersonalCardPubEmployeesFam_CouncilTownHall				= 1,
	eDocumentTypeSubCatPersonalCardPubEmployeesFam_MinistryOfTransport			= 2,
	eDocumentTypeSubCatPersonalCardPubEmployeesFam_VehicleRegistrationAuthority	= 3,
	eDocumentTypeSubCatPersonalCardPubEmployeesFam_PoliceAuthorityPrefettura	= 4,
	eDocumentTypeSubCatPersonalCardPubEmployeesFam_PoliceAuthorityQuestura		= 5,
};

enum eDocumentTypeSubCatGunLicense
{
	eDocumentTypeSubCatGunLicense_PoliceAuthorityPrefettura		= 1,
	eDocumentTypeSubCatGunLicense_PoliceAuthorityQuestura		= 2,
};


#endif //tsclientshared_h_included
