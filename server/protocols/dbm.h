#pragma once

#include "dbmmsg.h"
#include "ppincludebase.h"

enum UserPrivileges : UINT64
{
	AdminUser = ONE64 << 0, // 1
};

enum UserFlags : UINT64
{


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

enum eUserPropertyType
{
	
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

// authentication method used by user
enum eLoginAuthMethod
{
	eAuthUnknown = 0,
	eAuthPasswordOnly = 1,
	eAuthPokerStarsPin = 2,
	eAuthRsaToken = 3,
	eAuthOneTimePassword = 4,
	eAuthLimitedLogin = 5,
	eAuthWebToken = 6, // PYR-23485
	eAuthWebTokenInt = 7, // PYR-32891
	eAuthTempPin = 8, // PYR-51853
	eAuthExternalOperator = 9, // PYR-52518.	Client authentication done by external operator with operator's password.
	eAuthExternalOperatorToken = 10, // PYR-65464. Client authentication done by external operator with operator's token.
	eAuthTempPinEachLogin = 11,	//	PYR-76895
};

//constants objectType for DBM_Q_USER_AUTH
#define	OBJ_TYPE_LOBBY		0
#define	OBJ_TYPE_TABLE		1

//accidental logout types
#define LOGOUT_DISCONNECTED	(-1)
#define LOGOUT_CLEANUP		(-2)
#define LOGOUT_RECOVERY		(-3)
#define LOGOUT_ADMIN_FORCE	(-4)
#define LOGOUT_BLACKLIST_NOT_AVAILABLE (-5)		//	PYR-86054
#define LOGOUT_SESSION_TRANSFER	(-6)			//	PYR-86410

/*error codes*/
#define	DBM_NO_ERROR				0
#define	USER_ALREADY_EXISTS			1
#define NO_SUCH_USER				2
#define UNEXPECTED_COUNT			3 
#define NOT_ALLOWED					4 
#define NOT_ENOUGH_CHIPS			5 
#define INCONSISTENT_DATA			6
#define INTERNAL_LOGICAL_ERROR		7

/* Admin privileges*/
#define	SYSADMIN_STR				"SysAdmin"
#define HANDLE_KILL_ITSELF			"HandleKillItself" // PYR-19031

/*time constants*/
#define DAYS_IN_A_YEAR			365
#define DAYS_IN_MONTH			30
#define DAYS_IN_WEEK			7
#define DAYS_IN_DAY				1
#define HOURS_IN_A_DAY			24
#define MINUTES_IN_AN_HOUR		60
#define MINUTES_IN_A_DAY		1440
#define SECONDS_IN_A_DAY		86400
#define SECONDS_IN_AN_HOUR		3600
#define SECONDS_IN_A_MINUTE		60
#define MILLISECS_IN_A_MINUTE	(SECONDS_IN_A_MINUTE * 1000)
#define MILLISECS_IN_AN_HOUR	(MILLISECS_IN_A_MINUTE * MINUTES_IN_AN_HOUR)
#define MILLISECS_IN_A_DAY		(MILLISECS_IN_AN_HOUR * HOURS_IN_A_DAY)

#define ONE_THOUSAND			1000
#define ONE_MILLION				1000000
#define ONE_BILLION				1000000000

#define TEMP_PWD_PREFIX	"tmp"


enum eUserTitle // UserProperties.propertyId = eUserPropertyTitle = 59
{
	eUserTitleNone = 0,
	eUserTitleMr   = 1,
	eUserTitleMrs  = 2,
	eUserTitleMiss = 3,
	eUserTitleMs   = 4,
	eUserTitleDr   = 5,
};


#define LOG_TRACE_FAULTY_LOGIC "faulty logic "
#define LOG_TRACE_FAULTY_LOGIC_WITH_STACK "stack trace faulty logic"
#define LOG_TRACE_FAULTY_LOGIC_CANDIDATE "|failedlogic candidate| " // for dev to check questionable behavior - a candidate for further "faulty logic"

#define _BASE_26_DICTIONARY_ "PQLAMZWOSKXNBCDJEIURHFGVTY"

