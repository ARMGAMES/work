// regulator.h
#ifndef regulator_h_included
#define regulator_h_included

enum eRegulatorActionRequest
{
	eRegulatorActionRequest_NONE      = 0,
	eRegulatorActionRequest_Migration = 1, // specificMsgBody: 4 - targetSiteId, 4 - eRegulatorMigrationFlags, 4 - minClientMigrVersion, s - targetLicenseSpecificString
	eRegulatorActionRequest_EULA      = 2, // specificMsgBody: M(4) ; eRegulatorSignEulaOptions
	eRegulatorActionRequest_Reconnect = 3, // specificMsgBody is empty
	eRegulatorActionRequest_ConfirmLimits		= 4, // PYR-27795 specificMsgBody: M(4<M(4488)>) M(4<M(4488)>) ; each body: numLimits <M(4-limitType 4-limitId 8-oldValue 8-newValue)
	eRegulatorActionRequest_lifetimeLimitAck	= 5, // PYR-27262 specificMsgBody: M(14) M(1-typeOfPopUp 4-lifetimeLimit) ; typeOfPopUp: 1-Initial 2-Annual
	eRegulatorActionRequest_MigrDownloadAnotherSite = 6, // PYR-95206 specificMsgBody: 4 - targetSiteId
	eRegulatorActionRequest_CashoutForSelfExcl	= 7, // PYR-30475 specific body is empty
	eRegulatorActionRequest_InfoSelfExcl		= 8, // PYR-32490 specificMsgBody: s - string with self exclusion info PYR-32490
	eRegulatorActionRequest_ShowAccountSummary  = 9, //	PYR-33647 specificMsgBody: 4<8s>4<8s>4<8s>4<8s> - buyInCount<amount,currency>,winLossCount<amount,currency>,depositCount<totalDeposits,currency>,cashOutCount<totalCashouts,currency>
	eRegulatorActionRequest_PreventLogin       = 10, // PYR-86054 specific body is empty
	eRegulatorActionRequest_PasswordWeak		= 11, // PYR-82828 specificMsgBody: 4 - EnumPasswordCheckRes
	eRegulatorActionRequest_DataIntegrityCheckFailed = 12, //PYR-81030 specific body is empty
	eRegulatorActionRequest_UseAnotherSiteId = 13, // PYR-79114, specific body: 4 - required siteId
	eRegulatorActionRequest_PlayerVerification = 14, // PYR-90560 specific body is empty
	eRegulatorActionRequest_MigrCrtAcctInCurrSite = 15, // PYR-95206 specific body is empty
	eRegulatorActionRequest_ReactivateDormantAccount = 16, // PYR-102822 specific body is empty
};

// PYR-76207
enum eRegulatorPopupNotificationButtons
{
	eRegulatorPopupNotificationButton_None               = 0,
	eRegulatorPopupNotificationButton_OK                 = (1 << 0), // no action
	eRegulatorPopupNotificationButton_Close              = (1 << 1), // no action
	eRegulatorPopupNotificationButton_Cancel             = (1 << 2), // no action
	eRegulatorPopupNotificationButton_SendDocuments      = (1 << 3), // asynch action while client is logged in
	eRegulatorPopupNotificationButton_ContactSupportForm = (1 << 4), // asynch action while client is logged in
};

enum eRegulatorAccValEventType
{
	eRegulatorAccValEventType_None            = 0,
	eRegulatorAccValEventType_WaitDocs        = 1,
	eRegulatorAccValEventType_WaitBA          = 2,
	eRegulatorAccValEventType_SuspendedByDocs = 3,
	eRegulatorAccValEventType_SuspendedByBA   = 4,
	eRegulatorAccValEventType_ClosedByDocs    = 5,
	eRegulatorAccValEventType_WaitAvc         = 6,
	eRegulatorAccValEventType_SuspendedByAvc  = 7,
	eRegulatorAccValEventType_ClosedByAvc     = 8,
	eRegulatorAccValEventType_DocsMismatch    = 9,        // PYR-43398
	eRegulatorAccValEventType_DocsMismatchCorrected = 10, // PYR-43398
};

enum eRegulatorAccValEventFlags
{
	eRegulatorAccValEventFlags_None = 0,
	// To be extended with 'show or not show the validation popup upon login' once other (than .FR) license validations moved to regulator
};

enum eRegulatorBanPopupType
{
	eRegulatorBanPopupType_None = 0,
	eRegulatorBanPopupType_Banned = 1,
	eRegulatorBanPopupType_UnBanned = 2,
	eRegulatorBanPopupType_RemainBanned = 3,
};

enum eRegulatorNotificationType
{
	eRegulatorNotificationType_GenericPopup  = 0, // specific body - ss4 Caption, Text-Message, ButtonsMask(eRegulatorPopupNotificationButtons)
	eRegulatorNotificationType_AccValEvent   = 1, // specific body - 4444 - eRegulatorAccValEventType daysLeft timeFrameDays eRegulatorAccValEventFlags (legacy format from lobby)
	eRegulatorNotificationType_BanPopup      = 2, // specific body - 4s - eRegulatorBanPopupType, text-message
};
// PYR-76207 end

enum eRegulatorMigrationFlags
{
	eRegulatorMigrationFlag_UseOption2 = (1 << 0), // applicable to ES migration only for now
	eRegulatorMigrationFlag_UseOption3 = (1 << 1), // applicable to ES migration only // #23155
	eRegulatorMigrationFlag_UseWebMigration = (1 << 2), // PYR-40575
	eRegulatorMigrationFlag_AddCrtAcctOption = (1 << 3), // PYR-95206  applicable to !nj us migration only
};

enum eRegulatorUserOptions
{
	eRegulatorUserOption_FullTiltPairing						= (1 << 0), // #23771 obsolete after PYR-45667
	eRegulatorUserOption_FullTiltTransferOut					= (1 << 1), // #23769 obsolete after PYR-45667
	eRegulatorUserOption_EnableEnterMarketingCode				= (1 << 2),	//	#29811
	eRegulatorUserOption_SecurityQuestionsEnabled				= (1 << 3),	//	#28732
	eRegulatorUserOption_SecurityQuestionsTooManyWrongGuesses	= (1 << 4), //	#28732
	eRegulatorUserOption_FastDepositEnabled						= (1 << 5), //  #24992
	eRegulatorUserOption_CasinoEnabled							= (1 << 6), //  #33242
	eRegulatorUserOption_BgNewRmfEnabled						= (1 << 7), //  PYR-46521
	eRegulatorUserOption_CashinBlocked							= (1 << 8), //  PYR-48848 set using same logic as eUserExtraCashierOptions_BlockedCashin in cashier
	eRegulatorUserOption_BalanceBelowThreshold					= (1 << 9), //  PYR-52150 low balance detected
};

//	PYR-31219
enum eRegulatorRevalidationMethods
{
	eRegulatorRevalidationMethods_None					= 0,
	eRegulatorRevalidationMethods_ChallengeQuestions	= ( 1 << 0),
	eRegulatorRevalidationMethods_SMS					= ( 1 << 1),
	eRegulatorRevalidationMethods_FinancialInstrument	= ( 1 << 2),

	eRegulatorRevalidationMethods_Last = eRegulatorRevalidationMethods_FinancialInstrument, // must be kept up to date
};

enum eRegulatorConfigInfoType // see MSG_REGULATOR_GET_CONFIG_INFO
{
	eRegulatorConfigInfoType_None                       = 0,
	eRegulatorConfigInfoType_AddressLookup              = (1 << 0), //            M(b4) as M(showAddressLookup, addressLookupRMFtypesMask)
	eRegulatorConfigInfoType_AgeVerifyAttempts          = (1 << 1), // PYR-36856, M(4) as M(deshMaxAgeVerifyAttempts)
};

//	PYR-41017
enum eRegulatorWebPwdResetInputFormat
{
	eRegulatorWebPwdResetInputFormat_UserId					= 1,	//	User entry is a userId
	eRegulatorWebPwdResetInputFormat_EmailAddr				= 2,	//	User entry is in eMail format
	eRegulatorWebPwdResetInputFormat_MultipleEMailAddresses	= 3,	//	User entry is in eMail format, and we found a matching userId. (UserId contains '@')
};

// PYR-45621
enum eRegulatorActionOnRmokFlags
{
	eRegulatorActionOnRmokFlag_NoAction					 =       0,
	eRegulatorActionOnRmokFlag_BgDocVerificationRequired = (1 << 0),
	eRegulatorActionOnRmokFlag_SgcIdRequired			 = (1 << 1),
	eRegulatorActionOnRmokFlag_BgReportingDisabled		 = (1 << 2),
	eRegulatorActionOnRmokFlag_GwVerificationError       = (1 << 3), // PYR-47569
	eRegulatorActionOnRmokFlag_BeRegistrationIncomplete  = (1 << 4), // PYR-63165
	eRegulatorActionOnRmokFlag_EsMandatoryDocuments      = (1 << 5), // PYR-76448
	eRegulatorActionOnRmokFlag_SeBannedByRegulator       = (1 << 6), // PYR-82301
	eRegulatorActionOnRmokFlag_UsNoSport				 = (1 << 7), // PYR-113281
};

// PYR-63165
enum eRegulatorBeError
{
	eRegulatorBeError_OldService = 1,
	eRegulatorBeError_DefaultError,
	eRegulatorBeError_PlayerExists,
	eRegulatorBeError_Banned,
	eRegulatorBeError_BannedOnValidation,
	eRegulatorBeError_MajorError,
	eRegulatorBeError_CommError,
	eRegulatorBeError_AccRestricted,

	eRegulatorBeError_Hwm, // always last element
};



// Used with 'eRegulatorActionRequest_EULA' request, sub messages body in MSG_REGULATOR_USER_GET_INFO_REPLY
enum eRegulatorSignEulaOptions
{
	eRegulatorSignEulaOptions_None		= 0,
	eRegulatorSignEulaOptions_Marketing	= (1 << 0), // PYR-64283
	eRegulatorSignEulaOptions_EuSilentMigr = (1 << 1), // PYR-83113, PYR-89618 indicates that EULA reason was the Alpen EU Silent Migration, that would happen upon EULA sign
};

enum eRegulatorUserGetInfoFlags // PYR-76207 flags in MSG_REGULATOR_USER_GET_INFO
{
	eRegulatorUserGetInfoFlags_None            =       0,
	eRegulatorUserGetInfoFlags_FrAccValSupport = (1 << 0),
};

enum eRegulatorFrAccValAdminAction // PYR-43398 MSG_REGULATOR_FR_ACC_VAL_ADMIN_ACTION
{
	eRegulatorFrAccValAdminAction_PdcRequestPlayerAuthorization = 0,
	eRegulatorFrAccValAdminAction_PdcRequestPlayerAcceptance    = 1,
	eRegulatorFrAccValAdminAction_PdcCancel                     = 2,
};

enum eRegulatorFrAccValPlayerAction // PYR-43398 MSG_REGULATOR_FR_ACC_VAL_PLAYER_ACTION
{
	eRegulatorFrAccValPlayerAction_PdcAuthorize = 0,
	eRegulatorFrAccValPlayerAction_PdcAccept    = 1,
};

enum eRegulatorSeBankIdVerificationStatus // PYR-78110
{
	eRegulatorSeBankIdVerificationStatus_Pending  = 0,
	eRegulatorSeBankIdVerificationStatus_Complete = 1,
	eRegulatorSeBankIdVerificationStatus_Failed   = 2,
};

enum eRegulatorAdminSuspendUserOptions // PYR-83446 MSG_REGULATOR_ADMIN_SUSPEND_USER
{
       eRegulatorAdminSuspendUserOptions_None       = 0,
       eRegulatorAdminSuspendUserOptions_RestrictCO = 1 << 0, // 1
};

// PYR-89580 MSG_REGULATOR_GET_SWISS_URL
enum eRegulatorSwissURLType
{
	eRegulatorSwissURLType_login          =  1,
	eRegulatorSwissURLType_webCashier     =  2,
	eRegulatorSwissURLType_quickDeposit   =  3,
	eRegulatorSwissURLType_supportChat    =  4,
	eRegulatorSwissURLType_supportForm    =  5,
	eRegulatorSwissURLType_migration      =  6,  
	eRegulatorSwissURLType_signup         =  7,
	eRegulatorSwissURLType_commPreferences=  8,
	eRegulatorSwissURLType_depositLimits  =  9,
	eRegulatorSwissURLType_personalDetails= 10,
	eRegulatorSwissURLType_submitDocuments= 11

	//	When adding here, also update swissURLTypeToString
};

inline const char* swissURLTypeToString(eRegulatorSwissURLType urlType)
{
#	pragma warning(push)
#	pragma warning(error: 4062)
	switch (urlType)
	{
		case eRegulatorSwissURLType_login:			return "login";
		case eRegulatorSwissURLType_webCashier:		return "webCashier";
		case eRegulatorSwissURLType_quickDeposit:	return "quickDeposit";
		case eRegulatorSwissURLType_supportChat:	return "supportChat";
		case eRegulatorSwissURLType_supportForm:	return "supportForm";
		case eRegulatorSwissURLType_migration:		return "migration";
		case eRegulatorSwissURLType_signup:			return "signup";
		case eRegulatorSwissURLType_commPreferences:return "commPreferences";
		case eRegulatorSwissURLType_depositLimits  :return "depositLimits";
		case eRegulatorSwissURLType_personalDetails:return "personalDetails";
		case eRegulatorSwissURLType_submitDocuments:return "submitDocuments";
	};
#	pragma warning(pop)
	return "INVALID";
}

enum eUserUpdateRequestContext // PYR-90560
{
	eUserUpdateRequestContext_None    = 0,
	eUserUpdateRequestContext_NewUser = 1,
	eUserUpdateRequestContext_Login   = 2,
};

//	PYR-65389
enum AsiaB2B_LinkType
{
	eAsiaB2B_LinkType_INVALID		= 0,
	eAsiaB2B_LinkType_PlayLink		= 1,
	eAsiaB2B_LinkType_WebCashier	= 2,

	eAsiaB2B_LinkType_Last = eAsiaB2B_LinkType_WebCashier		// must be kept up to date
};

// PYR-92967
enum eRegulatorUsPaBlackListRequestOptions
{
	eRegulatorUsPaBlackListRequestOptions_None           = 0,
	eRegulatorUsPaBlackListRequestOptions_NoSports       = (1 << 0),
	eRegulatorUsPaBlackListRequestOptions_BanByRegulator = (1 << 1),
};

// PYR-109066
enum eBlacklistMonitorStatus
{
	eBlacklistMonitorStatus_Undefined = 0,
	eBlacklistMonitorStatus_Ok = 1,
	eBlacklistMonitorStatus_Fail = 2,
	eBlacklistMonitorStatus_NotAvailable = 3,
};

//PYR-119625
enum eExternalAuthClientFlags
{
	eExternalAuthClientFlags_useOperatorCashier = (1 << 0), // whether to use Operator Cashier or PS Cashier
};

// PYR-112096 MSG_REGULATOR_ADMIN_US_CLOSE_ACCOUNT_IF_BLACKLISTED_REPLY
enum eAdminCloseAccountIfBlacklistedResult
{
	eAdminCloseAccountIfBlacklistedResult_None,
	eAdminCloseAccountIfBlacklistedResult_AccountAlreadyClosed,
	eAdminCloseAccountIfBlacklistedResult_AccountWasClosed
};

//	PYR-65464
#define REGULATOR_EX_AUTH_NATIVE_OBJ_ID			1
#define REGULATOR_EX_AUTH_RAM_OBJ_ID			2

#define	MSG_REGULATOR_USER_GET_INFO				7100 // s4 clientVersion eRegulatorUserGetInfoFlags
#define	MSG_REGULATOR_USER_GET_INFO_REPLY		7101 // 6[s|B4<4B>B(4<4B>)] - errCode[errDescr| CommonBody1 numRequests < eRegulatorActionRequest ActionSpecificMsgBody >
                                                     //                       NotificationMsgBody(numNotifications<eRegulatorNotificationType NotificationSpecificMsgBody>) ]
// CommonBody1: 44848tt8s4s44<Body{s88}>s884ssssdsssss4sss84<M(44st)>
// userOptions(bitwise of eRegulatorUserOptions), flags, flags2, privs, privs2, registered, firstDepositTime, privs3, userWebId, licenseId, country, locale, numOfAccts <Body{currency, chips64(might be negative), tChips64}>, 
// userId, fppInCents, playChips, imageId, email, phone, mobile, sex, birthDate, addr1, addr2, city, state, zipcode, timezone, firstName, lastName, fullName, flags3, size<M(UserProperty)>
// In the accounts the default one always goes first

#define	MSG_REGULATOR_MIGRATION_GET_INFO		7102 // empty - NOT USED
#define	MSG_REGULATOR_MIGRATION_GET_INFO_REPLY	7103 // 6[s|] - errCode[errDescr|]

#define	MSG_REGULATOR_MIGRATION_PERFORM			7104 // 44 - migrFlag,migrSiteId
#define	MSG_REGULATOR_MIGRATION_PERFORM_REPLY	7105 // 6s - errCode,errDescr(unconditionally)

#define	MSG_REGULATOR_EULA_TERMS_SIGNED			7106 // bb - mailList,useMailList
#define	MSG_REGULATOR_EULA_TERMS_SIGNED_REPLY	7107 // 6[s] - errCode[errDescr] - client reconnects afterwards

#define	MSG_REGULATOR_GET_LAST_LOGIN_DETAILS		7108 // empty
#define	MSG_REGULATOR_GET_LAST_LOGIN_DETAILS_REPLY	7109 // for Non ES player:  6[s|tM(s)b] - errCode[errDescr|lastLogin, M(lastLogoutIp),isFirstLoginSession]
													     // for ES player: 	    6[s|tM(M(tts88)M(4<M(4sb)>)M(4<M(8s)>))b] - 
													     //						errCode[errDescr|lastLogin, M(
														 //						   M(lastLogin, lastLogout, reasonDescr, totalPlayed,totalWon),
													     //						   M(numTourns<tournId,tournName,published>)
													     //						   M(numHands<handId,tableName>)
														 //					    ), isFirstLoginSession]

#define	MSG_REGULATOR_REGISTER_NEW_USER2			7110 // 4ss8sssssssD4sss4444 reqId ipAddr id facebookIntId email userInstallId streetAddr city state country zipCode dob locale firstName lastName sex timezone clientPlatform, clientType clientDevice 
#define	MSG_REGULATOR_REGISTER_NEW_USER2_REPLY		7111 // 46[s|4] reqId errCode [errDescr|userIntId]

#define MSG_REGULATOR_ADMIN_REINIT					7112 // 4b - reqId isReadOnly
#define MSG_REGULATOR_ADMIN_REINIT_REPLY			7113 // 46[s|m(4<m(ssss)>)] - reqId errCode [errDescr|<m(iniData)>]

// #23923 from either CORE or Client
#define MSG_REGULATOR_GET_FACEBOOK_LINK_CODE     	7114 // core: 4ss reqId userId email; authed client: empty message
#define MSG_REGULATOR_GET_FACEBOOK_LINK_CODE_REPLY	7115 // [4]6[s|4] - reqId(for Core) errCode [errDescr|linkCode]
// from CORE only
#define MSG_REGULATOR_CREATE_FACEBOOK_LINK      	7116 // 4s84 reqId userId facebookIntId linkCode
#define MSG_REGULATOR_CREATE_FACEBOOK_LINK_REPLY	7117 // 46[s|4s] reqId errCode [errDescr|userIntId userId]

// #23464 - from unAuthed client
#define MSG_REGULATOR_1ST_TIME_FACEBOOK_LOGIN		7118 // sss444M  userId email installIdDeprecated locale siteId platformId loginExtraData
#define MSG_REGULATOR_1ST_TIME_FACEBOOK_LOGIN_REPLY 7119 // 6[s]

// from client
#define MSG_REGULATOR_GET_FTP_PAIRING_CODE    	 	7120 // #23771: empty
#define MSG_REGULATOR_GET_FTP_PAIRING_CODE_REPLY	7121 // 6[s|s] - errCode [errDescr|pairingCode]

// from FullTilt Gateway
// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_SET_FTP_PAIRING	    	 	7122 // 4-reqId FtpPairing
#define MSG_REGULATOR_SET_FTP_PAIRING_REPLY			7123 // 48b6[s|4s] reqId same_ftObjectId isDuplicate errCode [errDescr|psUserIntId psUserId]

// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_FTP_MIGRATION_PERFORM	     	7124 // 4-reqId FTPMigration
#define MSG_REGULATOR_FTP_MIGRATION_PERFORM_REPLY	7125 // 48b6[s|4s Obj] reqId same_ftObjectId isDuplicate errCode [errDescr|licenseId psUserId FullTiltAssetMigrationResult]

#define MSG_REGULATOR_PGAD_DIRECT_MESSAGE			7126 // 44..... reqId,msgId,rest copied to ItAcctGateway
#define MSG_REGULATOR_PGAD_DIRECT_MESSAGE_REPLY		7127 // 4[s]  //#27086,#27192

#define MSG_REGULATOR_REGISTER_EXTERNAL_USER		7128 //
#define MSG_REGULATOR_REGISTER_EXTERNAL_USER_REPLY	7129 //

#define MSG_REGULATOR_CONFIRMED_USER_LIMITS		    7130 // M(4<M(44b)>) M(4<M(44b)>) 1-body deposit limits, 2-body game limits: numLimits M(limitType limitId action)
#define MSG_REGULATOR_CONFIRMED_USER_LIMITS_REPLY	7131 // M(2[s|4<M(…)>) M(2[s|4<M(…)>) 1-body deposit limits, 2-body game limits: Each msg body in vector : M(2s44b44s) errCode sqlError limitType limitId isApprove prevLimit limit currency

// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_FTP_PS_ACCOUNT_CREATED	    7132 // 4-reqId Ps2Ftp_AccountCreated(gcfr.h)
#define MSG_REGULATOR_FTP_PS_ACCOUNT_CREATED_REPLY	7133 // 44[s]-reqId, errCode [errMsg]

//	PYR-27258
#define MSG_REGULATOR_RESET_PWD						7134 //	ssssbsss userId, zip, addr, email, useQ, answer1, answer2, ssn4
#define MSG_REGULATOR_RESET_PWD_REPLY				7135 //	6[s4] - errCode[errDesc,licenseId]

#define MSG_REGULATOR_SMS_RESET_PWD					7136 // sss userId, email, mobile
#define MSG_REGULATOR_SMS_RESET_PWD_REPLY			7137 // 6[s] errCode, [errDescr]

//	Retrieve all challenge questions
#define MSG_REGULATOR_CHALLENGE_QUESTION			7138 // 4 - locale
#define MSG_REGULATOR_CHALLENGE_QUESTION_REPLY		7139 // 6[s|2<4s>] - errCode, [errDesc | size, < id,question >]

//	Retrieve a user's challenge questions
#define MSG_REGULATOR_PWD_RESET_QUESTIONS			7140 // ss4 - userid, email, locale
#define MSG_REGULATOR_PWD_RESET_QUESTIONS_REPLY		7141 // 6[s|b2<4s>b] - errCode, [errDesc | isRM, size, id 1, question1, id2, question2, SmsResetAllowed]

//	Retrieve a user's challenge questions - no user email provided
#define MSG_REGULATOR_PWD_RESET_QUESTIONS2			7142 // s44 - userid, locale, userIntId
#define MSG_REGULATOR_PWD_RESET_QUESTIONS2_REPLY	7143 // 6[s|2<4s>] - errCode, [errDesc | size, id 1, question1, id2, question2]

//	Clear a user's "Strong Authentication Required" setting
#define MSG_REGULATOR_CLEAR_STRONG_AUTH_REQD		7144 // ssss - userid, answer1, answer2, ssn4
#define MSG_REGULATOR_CLEAR_STRONG_AUTH_REQD_REPLY	7145 //	6s errCode, errDescr

#define MSG_REGULATOR_REGISTER_NEW_USER_BY_ADMIN		7146 // see RegisterNewUserRequest::parseFromAdmin
#define MSG_REGULATOR_REGISTER_NEW_USER_BY_ADMIN_REPLY	7147 // reqId errCode [descr | userIntId, unique ]

#define MSG_REGULATOR_UPDATE_PROMOCODES				7148 // 44<s> reqId, size, <promocode>
#define MSG_REGULATOR_UPDATE_PROMOCODES_REPLY		7149 // 46[s] reqId, errCode[, errDescr]

//	PYR-28472
#define MSG_REGULATOR_RETRIEVE_USERID2				7150	//	ssss44 - email, answer1, answer2, ssn4, locale, clientSiteId
#define MSG_REGULATOR_RETRIEVE_USERID2_REPLY		7151	// 6s errCode, errDescr

//	PYR-26665
#define MSG_REGULATOR_GET_REMAINING_IMAGE_CHANGES			7152	//	4448 - reqId,userIntId,privileges,privileges2
#define MSG_REGULATOR_GET_REMAINING_IMAGE_CHANGES_REPLY		7153	//	46[s|4] reqId,errCode, [errDescr|remainingImageChanges]

//  PYR-29386
#define MSG_REGULATOR_CLOSE_ARJEL_ACCOUNT		    7154	// 4sb - reqId,userId,doCashout
#define MSG_REGULATOR_CLOSE_ARJEL_ACCOUNT_REPLY		7155	//

/* obsolete after PYR-52410
//	PYR-28733.  Client unauth message to get account type - PM or RM
#define MSG_REGULATOR_IS_ACCOUNT_PM_FOR_PWD_RESET			7156	//	ss44M - userIdOrEmail,installIdDeprecated,locale,clientSiteId, loginExtraData
#define MSG_REGULATOR_IS_ACCOUNT_PM_FOR_PWD_RESET_REPLY		7157	//	6[s|ssbbb2<4s>b] - errCode[errDescr|userId,email,isPMnotRM,userIdFoundNotEmail,installIdMatch,size,id 1,question1,id2,question2,SmsResetAllowed]

//	PYR-28733.  Client unauth message to set password
#define MSG_REGULATOR_SET_NEW_PWD							7158	//	ss44sM - userId,installIdDeprecated,locale,clientSiteId,newPwd, loginExtraData
#define MSG_REGULATOR_SET_NEW_PWD_REPLY						7159	//	6[s] - errCode[errDescr]

//	PYR-28733.  Client unauth message to set email address given userId
#define MSG_REGULATOR_RETRIEVE_EMAIL						7160	//	ss44s - userId,locale,clientSiteId
#define MSG_REGULATOR_RETRIEVE_EMAIL_REPLY					7161	//	6[s] - errCode[errDescr]
*/

//	PYR-29811. Client authenticated message to set manual promo code.
#define MSG_REGULATOR_SET_PROMO_CODE						7162	//	s - promoCode
#define MSG_REGULATOR_SET_PROMO_CODE_REPLY					7163	//	6[s] - errCode[errDescr]

//	PYR-30114.  Client and GW messages to get xml+hash for FT launcher
// obsolete
#define MSG_REGULATOR_CLIENT_FTPLAUNCHER						7164	//	s - userId
#define MSG_REGULATOR_CLIENT_FTPLAUNCHER_REPLY			        7165	//	6s[sss] - errCode,errDescr/ftpUserId, [url, xml, hash]
// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_FTPLAUNCHER_GW						    7166	//	4s4ssssssss444d4444ss44ssss- userIntId,userId,privilage,fullname,email,addr1,city,state,country,zipcode,phone,lflpp,vipstatus,ownedchips,bDate,locale,privilege2,emlocale,timezone,firstname,lastname,licenseId,flags2,sex,mobile,pairingcode,title
#define MSG_REGULATOR_FTPLAUNCHER_GW_REPLY			            7167	//	ss - xml, hash

//PYR-30480
// client<->regulator
#define MSG_REGULATOR_SET_PWD_REMINDER							7168	// b - reminder
#define MSG_REGULATOR_SET_PWD_REMINDER_REPLY					7169	// 6[s] - reqId, errCode[errStr]

//	PYR-28732. Client -> Reg. Auth message to verify challenge answers
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_REGULATOR_VALIDATE_CHALLENGE_ANSWERS				7170	//	4<s> - size<answers>
#define MSG_REGULATOR_VALIDATE_CHALLENGE_ANSWERS_REPLY			7171	//	6[s|bb]  - errCode[errStr|isMatch,allowMoreAttempts]

//	PYR-28732. Client -> Reg. Auth message to remove challenge questions.
#define MSG_REGULATOR_REMOVE_CHALLENGE_QUESTIONS				7172	//	
#define MSG_REGULATOR_REMOVE_CHALLENGE_QUESTIONS_REPLY			7173	//	6[s]  - errCode[errStr]

//	PYR-28732. Client -> Reg. Auth message to set or remove challenge question reminder prompt.
#define MSG_REGULATOR_SET_CHALLENGE_QUESTIONS_REMINDER			7174	//	b - setNotRemove
#define MSG_REGULATOR_SET_CHALLENGE_QUESTIONS_REMINDER_REPLY	7175	//	6[s]  - errCode[errStr]

//	PYR-28732. Client -> Reg. Auth message set the timestamp when the user refused to setup their challenge questions
#define MSG_REGULATOR_CHALLENGE_QUESTION_SETUP_REFUSED				7176	//
#define MSG_REGULATOR_CHALLENGE_QUESTION_SETUP_REFUSED_REPLY		7177	//	6[s]  - errCode[errStr]

//	PYR-31158. Admin -> Reg
#define MSG_REGULATOR_ADMIN_REMOVE_CHALLENGE_QUESTIONS				7178	//	44 - reqId,userIntId
#define MSG_REGULATOR_ADMIN_REMOVE_CHALLENGE_QUESTIONS_REPLY		7179	//	46[s]  - reqId,errCode[errStr]

// PYR-27186
#define MSG_REGULATOR_MATCH_ZIP_CODE                            7180   // sss              - streetNum zip countryCode
#define MSG_REGULATOR_MATCH_ZIP_CODE_REPLY                      7181   // 6[s|4<M(sssss)>] - errCode[errDescr|size address city state zip country]

//	PYR-31219. Unauth client -> Reg.
#define MSG_REGULATOR_GET_AVAIL_REVALIDATION_METHODS			7182	//	s444 - userId,locale,clientSiteId,userIntId
#define MSG_REGULATOR_GET_AVAIL_REVALIDATION_METHODS_REPLY		7183	//	6[s|4s] - errCode[errDescr|revalidationMask,mobilePhoneMask]

//	PYR-31219 Unauth client -> Reg.
#define MSG_REGULATOR_CLEAR_ACCOUNT_REVALIDATION				7184	//	s444ss4 - userid, locale, clientSiteId, validationMethodUsed, answer1, answer2, userIntId
#define MSG_REGULATOR_CLEAR_ACCOUNT_REVALIDATION_REPLY			7185	//	6[s] - errCode[errStr]

//	PYR-31219 Unauth client -> Reg.
#define MSG_REGULATOR_ACCOUNT_REVALIDATION_CANCELLED			7186	//	s444 - userId,locale,clientSiteId,userIntId
#define MSG_REGULATOR_ACCOUNT_REVALIDATION_CANCELLED_REPLY		7187	//	6[s] - errCode[errStr]

//	PYR-31219 Unauth client -> Reg.
#define MSG_REGULATOR_ACCOUNT_REVALIDATION_SEND_SMS				7188	//	s444 - userId,locale,clientSiteId,userIntId
#define MSG_REGULATOR_ACCOUNT_REVALIDATION_SEND_SMS_REPLY		7189	//	6[s] - errCode[errStr]

//	PYR-31219 Unauth client -> Reg.  Get last used credit cards
#define MSG_REGULATOR_ACCOUNT_REVALIDATION_GET_LAST_CC				7190	//	s444 - userId,locale,clientSiteId,userIntId
#define MSG_REGULATOR_ACCOUNT_REVALIDATION_GET_LAST_CC_REPLY		7191	//	6[s|4<ssd>] - errCode[errStr|arraySize<paysystem,creditCardMask,dateUsed>]

// PYR-30474
// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_FTP_PS_SELFEXCLUDED						7192 //
#define MSG_REGULATOR_FTP_PS_SELFEXCLUDED_REPLY					7193 //

// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_FTP_PS_LIMITS_UPDATED						7194 //
#define MSG_REGULATOR_FTP_PS_LIMITS_UPDATED_REPLY				7195 //

// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_PS_FTP_SELFEXCLUDED						7196 //
#define MSG_REGULATOR_PS_FTP_SELFEXCLUDED_REPLY					7197 //

// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_REGULATOR_PS_FTP_LIMITS_UPDATED						7198 //
#define MSG_REGULATOR_PS_FTP_LIMITS_UPDATED_REPLY				7199 //

#define MSG_REGULATOR_CONVERT_FUNDS_TO_ONE_ACCT_FOR_NJMIGR        7200 // 
#define	MSG_REGULATOR_CONVERT_FUNDS_TO_ONE_ACCT_FOR_NJMIGR_REPLY  7201 // 2[s|4b] errCode [errDescr|amount accountChanged]

#define MSG_REGULATOR_ACTIVATE_ACCOUNT							7202 // s - encrypted_string
#define MSG_REGULATOR_ACTIVATE_ACCOUNT_REPLY					7203 // 2[s] errCode[errDesc]

//	PYR-32706. Client -> Reg. Unauth message to verify challenge answers
#define MSG_REGULATOR_VALIDATE_CHALLENGE_ANSWERS2				7204	//	s444<s> - userId,locale,clientSiteId,size<answers>
#define MSG_REGULATOR_VALIDATE_CHALLENGE_ANSWERS2_REPLY			7205	//	6[s|b]  - errCode[errStr|isMatch]

// PYR-33086
// client<->regulator
#define MSG_REGULATOR_SET_SHOW_CASINO_GAMES_FLAG				7206	// bss - set, userId, comment
#define MSG_REGULATOR_SET_SHOW_CASINO_GAMES_FLAG_REPLY			7207	// 6[s] - reqId, errCode[errStr]

// PYR-29932
#define	MSG_REGULATOR_REPORT_USER_EMAIL_UNAUTHORIZED			7208 // 4s reqId, encryptedString
#define	MSG_REGULATOR_REPORT_USER_EMAIL_UNAUTHORIZED_REPLY		7209 //	42[s] - reqId, errCode[errDescr]

// PYR-31215
#define MSG_REGULATOR_GET_CONFIG_INFO                           7210 // 4 eRegulatorConfigInfoTypeMask
#define MSG_REGULATOR_GET_CONFIG_INFO_REPLY                     7211 // [s|4<4B>] - errCode[errDescr| numConfigTypes< eRegulatorCongigInfoType ConfigTypeSpecificMsgBody >]   eRegulatorConfigInfoType_AddressLookup: b4 - showAddressLookup, eRegulatorRmFormTypeMask

#define	MSG_REGULATOR_GET_USER_WINNINGS						7212 // t loginTime
#define	MSG_REGULATOR_GET_USER_WINNINGS_REPLY				7213 // 6[s|88] - errCode[errDescr|totalPlayed,totalWon]

// Obsolete PYR-54639 #define	MSG_REGULATOR_USERID_SUGGESTION							7214 // 4s reqId
// Obsolete PYR-54639 #define	MSG_REGULATOR_USERID_SUGGESTION_REPLY					7215 //	42[s|4<s>] - reqId, errCode[errDescr|<userIds>]

//	PYR-33276. Client -> Reg. Set USERS.FLAGS2 rateMobileAppFlag2
#define MSG_REGULATOR_SET_RATE_MOBILE_FLAG						7216 //	bss - setNotClear, userId, comment
#define MSG_REGULATOR_SET_RATE_MOBILE_FLAG_REPLY				7217 // 6[s] - errCode[errDescr]

// PYR-33375
#define MSG_REGULATOR_CALCULATE_IT_PGAD_VERSION_FLAGS2                7218 // 8 flags2
#define MSG_REGULATOR_CALCULATE_IT_PGAD_VERSION_FLAGS2_REPLY          7219 // 68 errcode, it pgad version calculated flags2

// PYR-35937
#define MSG_REGULATOR_SUBMIT_STARCODE							7220 // 4s reqId, encryptedString
#define MSG_REGULATOR_SUBMIT_STARCODE_REPLY						7221 // 46s reqId, errCode, [errDescr|successDescr]

//PYR-35209
#define MSG_REGULATOR_ADMIN_UPDATE_SELFEXCLUSION_FR 			7222    //4s444 reqID, userId, newDaysActual, exclusionFlags, seFlags
#define MSG_REGULATOR_ADMIN_UPDATE_SELFEXCLUSION_FR_REPLY 		7223    // 46[s] - reqId, errCode[errDescr] 

// PYR-38623 Admin -> Reg
#define MSG_REGULATOR_ADMIN_RESET_ID_VERIFICATION_COUNTER		7224    //44 reqId, userIntId
#define MSG_REGULATOR_ADMIN_RESET_ID_VERIFICATION_COUNTER_REPLY	7225    //46[s]  - reqId, errCode[errStr]

#define MSG_REGULATOR_LINKED_INI_CHANGED					7226 //
#define MSG_REGULATOR_LINKED_INI_CHANGED_REPLY				7227 //

// PYR-39878
#define MSG_REGULATOR_FULLTILT_GET_CONVERSION_DATA	            7228 // 4 - playerix
#define MSG_REGULATOR_FULLTILT_GET_CONVERSION_DATA_REPLY        7229 // 6[s|44<s>s884444444 4<444s644646> 4<4444444s44> 484444444] - errCode[errDescr|psUserMatchingType,size<userId>,ftEmail,balance_playchips,credit_playchips,balance_ftp,balance_ftcoins,credit_starcoins,balance_cashusd,balance_tournusd,balance_casheur,balance_tourneur size<FullTiltMigrationUserTicket> size<FullTiltMigrationUserBonus> FullTiltMigrationPreconsolidatedUserBalance]

#define MSG_REGULATOR_FULLTILT_CONVERSION_PERFORM		        7230 // 444 - playerix,userIntId,convertionType
#define MSG_REGULATOR_FULLTILT_CONVERSION_PERFORM_REPLY	    	7231 // 46[s|2] - reqId,errCode[errDescr|isRMOK]

//	PYR-41017. Client (unauth connection) -> regulator
#define MSG_REGULATOR_SEND_WEB_PWD_RESET_EMAIL					7232	//	s44s4M - userIdOrEmail,locale,clientSiteId,installId,brandId,loginExtraData
#define MSG_REGULATOR_SEND_WEB_PWD_RESET_EMAIL_REPLY			7233	//	6[s|4s] - errCode[errStr|eRegulatorC2LPwdResetInputFormat,eMailMask]

//	PYR-41017. RAM -> regulator
#define MSG_REGULATOR_WEB_PWD_SET_NEW_PWD						7234	//	sss - token,newPassword,clientsIp
#define MSG_REGULATOR_WEB_PWD_SET_NEW_PWD_REPLY					7235	//	6[s[4]]  - errCode[errStr[errReason]]

//	PYR-41017. Admin -> regulator
#define MSG_REGULATOR_ADMIN_SEND_WEB_PWD_RESET_EMAIL			7236	//	4s - reqId,userId
#define MSG_REGULATOR_ADMIN_SEND_WEB_PWD_RESET_EMAIL_REPLY		7237	//	46[s]  - reqId, errCode[errStr]

//	PYR-41017. RAM -> regulator
#define MSG_REGULATOR_WEB_PWD_GET_TOKEN_INFO					7238	//	s - token
#define MSG_REGULATOR_WEB_PWD_GET_TOKEN_INFO_REPLY				7239	//	6[s|sbssd4t]  - errCode[errStr|userId,rmOk,firstName,lastName,DOB,license,tokenExpiry]

//	PYR-43867. Client -> regulator
#define MSG_REGULATOR_GET_USER_SESSION_WINNINGS					7240	//	empty
#define MSG_REGULATOR_GET_USER_SESSION_WINNINGS_REPLY			7241	//	6[s|btt88] - errCode[errStr|isLastSession,sessionStartTime,sessionEndTime,wager,winLoss]

//	PYR-37989. Ram (unauth) -> regulator
#define MSG_REGULATOR_SET_PRIV_NO_SURVEY						7242	//	s - token
#define MSG_REGULATOR_SET_PRIV_NO_SURVEY_REPLY					7243	//	6[s]  - errCode[errStr]

//	PYR-42399. Ram (unauth) -> regulator
#define MSG_REGULATOR_EMAIL_AVAILABLE							7244	//	s4{ClientLoginExtraData} - email, locale, {ClientLoginExtraData}
#define MSG_REGULATOR_EMAIL_AVAILABLE_REPLY						7245	//	6[s[4]] - errCode[errStr[if errCode==DUPLICATE_NEW_USER_EMAIL then BrandId same as for PYR-39894]]

// PYR-43398  admin -> regulator
#define MSG_REGULATOR_FR_ACC_VAL_ADMIN_ACTION                   7246    // 4s4s - reqId,userId,eRegulatorFrAccValAdminAction,comment
#define MSG_REGULATOR_FR_ACC_VAL_ADMIN_ACTION_REPLY             7247    // 46[s] - reqId,errCode,[errDesc]

// PYR-43398  Client -> regulator
#define MSG_REGULATOR_FR_ACC_VAL_PLAYER_ACTION                  7248    // eRegulatorFrAccValPlayerAction
#define MSG_REGULATOR_FR_ACC_VAL_PLAYER_ACTION_REPLY            7249    // 6[s] - errCode,[errDesc]

// PYR-45841  Client(auth/unauth) -> regulator
#define MSG_REGULATOR_VALIDATE_USER_UPDATE						7250    // {UserUpdate}4 - UserUpdate, locale
#define MSG_REGULATOR_VALIDATE_USER_UPDATE_REPLY				7251    // 46[s|4<46s>] - reqId,errCode,[errDesc|size<updateType,errCode,errStr>]

//  PYR-45631
#define MSG_REGULATOR_REGISTER_BG_USER							7252	// 44 - reqId,userIntId
#define MSG_REGULATOR_REGISTER_BG_USER_REPLY					7253	// 46[s] - reqId,errCode,[errDesc]

//  PYR-47521 cashier -> regulator
#define MSG_REGULATOR_REPORT_CASHOUT_CLOSED_ACCOUNT_ARJEL       7254	// 4s4s - reqId,userId,cashoutAmount,iban
#define MSG_REGULATOR_REPORT_CASHOUT_CLOSED_ACCOUNT_ARJEL_REPLY	7255	// 46[s] - reqId,errCode,[errDesc]

//  PYR-49027 admin -> regulator
#define MSG_REGULATOR_UNREGISTER_BG_USER						7256	// 44 - reqId,userIntId
#define MSG_REGULATOR_UNREGISTER_BG_USER_REPLY					7257	// 46[s] - reqId,errCode,[errDesc]

//  PYR-50428 client/admin -> regulator
#define MSG_REGULATOR_CHECK_USER_DOCUMENT_UNIQUE				7258	// from client s4 - docStr, docType, country | from admin 44s4s - userIntid, licenseId, docStr, docType, country
#define MSG_REGULATOR_CHECK_USER_DOCUMENT_UNIQUE_REPLY			7259	// 6[s] - errCode,[errDesc]

//  PYR-52691 admin -> regulator
#define MSG_REGULATOR_BE_CHECK_IF_USER_BLACKLISTED				7260	// 44s - reqId,userIntid, userId
#define MSG_REGULATOR_BE_CHECK_IF_USER_BLACKLISTED_REPLY		7261	// 42[s|b] - reqId,errCode,[errDesc|isBlacklisted]

//  PYR-50999 admin -> regulator
#define MSG_REGULATOR_UK_CHECK_IF_USER_BLACKLISTED				7262	// 44s - reqId,userIntId, userId
#define MSG_REGULATOR_UK_CHECK_IF_USER_BLACKLISTED_REPLY		7263	// 42[s|b] - reqId,errCode,[errDesc|isBlacklisted]

//	PYR-64017 Ram (unauth) -> regulator
#define MSG_REGULATOR_SET_COMM_PREFS							7264	//	sbbbbbbb - token,poker,casino,sports,orgPoker,orgCasino,orgSports,isUserArchived
#define MSG_REGULATOR_SET_COMM_PREFS_REPLY						7265	//	6[s]  - errCode[errStr]

//	PYR-64017 Ram (unauth) -> regulator
#define MSG_REGULATOR_GET_COMM_PREFS							7266	//	s - token
#define MSG_REGULATOR_GET_COMM_PREFS_REPLY						7267	//	6[s|sbbbb]  - errCode[errStr|userId,poker,casino,sports,isUserArchived]

//	PYR-64017 Ram (unauth) -> regulator
#define MSG_REGULATOR_SET_COMM_PREFS_SV							7268	//	sbbbbbbb - userId,poker,casino,sports,orgPoker,orgCasino,orgSports,isUserArchived
#define MSG_REGULATOR_SET_COMM_PREFS_SV_REPLY					7269	//	6[s]  - errCode[errStr]

//	PYR-64017 Ram (unauth) -> regulator
#define MSG_REGULATOR_GET_COMM_PREFS_SV							7270	//	4 - userIntId
#define MSG_REGULATOR_GET_COMM_PREFS_SV_REPLY					7271	//	6[s|sbbbb]  - errCode[errStr|userId,poker,casino,sports,isUserArchived]

//	PYR-51935 Auth Client -> regulator
#define MSG_REGULATOR_GET_ASIA_GAMING_URL						7272	//	6 - AsiaB2B_LinkType
#define MSG_REGULATOR_GET_ASIA_GAMING_URL_REPLY					7273	//	6[s|s] - errCode[errStr|url]

// PYR-71570 client (auth) -> regulator, no actions returned
#define	MSG_REGULATOR_USER_GET_INFO_NO_ACTIONS					7274 // s clientVersion
#define	MSG_REGULATOR_USER_GET_INFO_NO_ACTIONS_REPLY			7275 // 6[s|B0B(0)] - errCode[errDescr| CommonBody1 numRequests(=0) NotificationMsgBody(numNotifications(=0))]
// CommonBody1: see MSG_REGULATOR_USER_GET_INFO

//	PYR-65464. Ram -> Regulator
#define MSG_REGULATOR_GET_EXTERNAL_OP_TOKEN						7276	//
#define MSG_REGULATOR_GET_EXTERNAL_OP_TOKEN_REPLY				7277	//	6[s|ss4] - errCode[errDescr|token,tokenSignature,userIntId]

// PYR-66028. JobsController/Admin -> regulator
#define MSG_REGULATOR_GET_IT_PGAD_ACCT_STATUS					7278	//	4s48 -reqId, userId, userIntId, flags2
#define MSG_REGULATOR_GET_IT_PGAD_ACCT_STATUS_REPLY				7279	//	6[s|4] - errCode[errStr|<PgadTransactEssential>]

//	PYR-64085. AG gateway, AsiaCDD gateway, admin -> regulator
#define MSG_REGULATOR_UPDATE_KYC								7280	//	4sbbs - reqId,userId,starsUserIdNotAg,setNotRemove,operatorId
#define MSG_REGULATOR_UPDATE_KYC_REPLY							7281	//	6[s] - errCode[errStr]

// PYR-61891 LobbyEtc -> Regulator
#define MSG_REGULATOR_SET_USER_TRANSACT_LASTREQUESTED			7282	//	4sTT - reqId,userId,oldLastRequested,newLastRequested
#define MSG_REGULATOR_SET_USER_TRANSACT_LASTREQUESTED_REPLY		7283	//	6[s] - errCode[errDescr]

// PYR-61891 JobsController -> Regulator
#define MSG_REGULATOR_ARJEL_REPORT_PENDING_OFFLINE_MESSAGES		7284	//  44 - reqId,userIntId
#define MSG_REGULATOR_ARJEL_REPORT_PENDING_OFFLINE_MESSAGES_REPLY 7285	//	6[s] - errCode[errDescr]

// PYR-78110 Client -> Regulator
#define MSG_REGULATOR_SE_BANKID_VERIFY                          7286	//	sb - personalNumber,mobileBankIdOnly
#define MSG_REGULATOR_SE_BANKID_VERIFY_REPLY                    7287	//	2[s|4M([s4|])] - errCode[errDescr|status[autoStartToken,orderId|]]

// PYR-78110 Client -> Regulator
#define MSG_REGULATOR_SE_BANKID_COLLECT                         7288	//	4b - orderId,bankIdClientStartedAutomatically
#define MSG_REGULATOR_SE_BANKID_COLLECT_REPLY                   7289	//	2[s|4M([s|s|sssbdsbssss])] - errCode[errDesc|status[messageCode|messageCode|firstName,lastName,personalNumber,ageVerified,DOB,sex,hasAddr,address,postalCode,city,country]]

//  PYR-82303 admin -> regulator
#define MSG_REGULATOR_SE_CHECK_IF_USER_BLACKLISTED				7290	// 44ss - reqId,userIntid, userId, fiscalCode
#define MSG_REGULATOR_SE_CHECK_IF_USER_BLACKLISTED_REPLY		7291	// 42[s|b] - reqId,errCode,[errDesc|isBlacklisted]

// PYR-82532 Client -> regulator
#define MSG_REGULATOR_GET_USER_BLACKLIST_INFO			        7292	//
#define MSG_REGULATOR_GET_USER_BLACKLIST_INFO_REPLY	            7293	// 2[s|b[M(4TT)]] - errCode,[errDesc|isBlacklisted, [M(durationDays,timeFrom,timeUntil)]]

// PYR-82532 Client -> regulator
#define MSG_REGULATOR_REQUEST_USER_BLACKLISTED			        7294	// 4 - durationDays
#define MSG_REGULATOR_REQUEST_USER_BLACKLISTED_REPLY            7295	// 2[s|M(4TT)] - errCode,[errDesc|M(durationDays,timeFrom,timeUntil)]

// Obsolete PYR-82532 Admin -> regulator
// #define MSG_REGULATOR_ADMIN_GET_USER_BLACKLIST_INFO		    7296	// 44s - reqId,userIntId,userId
// #define MSG_REGULATOR_ADMIN_GET_USER_BLACKLIST_INFO_REPLY	7297	// 42[s|b[M(4TT)]] - reqId,errCode,[errDesc|isBlacklisted, [M(durationDays,timeFrom,timeUntil)]]

// PYR-82532 Admin -> regulator
#define MSG_REGULATOR_ADMIN_REQUEST_USER_BLACKLISTED			7298	// 44s4 - reqId,userIntId,userId,durationDays
#define MSG_REGULATOR_ADMIN_REQUEST_USER_BLACKLISTED_REPLY      7299	// 42[s|M(4TT)] - reqId,errCode,[errDesc|M(durationDays,timeFrom,timeUntil)]

// PYR-82532 Admin -> regulator
#define MSG_REGULATOR_ADMIN_REQUEST_USER_UNBLACKLISTED			7300	// 44s - reqId,userIntId,userId
#define MSG_REGULATOR_ADMIN_REQUEST_USER_UNBLACKLISTED_REPLY    7301	// 42[s] - reqId,errCode,[errDesc]

// PYR-79474 Admin -> regulator
#define MSG_REGULATOR_ADMIN_REQUEST_ITBLACKLIST_UPDATE			7302	// 4 - reqId
#define MSG_REGULATOR_ADMIN_REQUEST_ITBLACKLIST_UPDATE_REPLY    7303	// 42[s] - reqId,errCode,[errDesc]

// PYR-83446 Admin -> regulator
#define MSG_REGULATOR_ADMIN_SUSPEND_USER						7304	// 44s44s - reqId,userIntId,userId,eRegulatorAdminSuspendUserOptions,eSuspendedAccountCategory,comment
#define MSG_REGULATOR_ADMIN_SUSPEND_USER_REPLY					7305	// 42[s] - reqId,errCode,[errDesc]

// PYR-83446 Admin -> regulator
#define MSG_REGULATOR_ADMIN_UNSUSPEND_USER						7306	// 44ss - reqId,userIntId,userId,comment
#define MSG_REGULATOR_ADMIN_UNSUSPEND_USER_REPLY				7307	// 42[s] - reqId,errCode,[errDesc]

// PYR-89580 Unauth + auth client -> regulator
#define MSG_REGULATOR_GET_SWISS_URL								7308	// M()444<s> - M(loginExtraData),locale,eRegulatorSwissURLType,size<params>
#define MSG_REGULATOR_GET_SWISS_URL_REPLY						7309	// 2[s|s] - errCode[errDesc|url]

// PYR-89580 Swiss gateway -> regulator
#define MSG_REGULATOR_GET_FULL_USER_BY_USERINTID				7310	//	44 - reqId, userIntId
#define MSG_REGULATOR_GET_FULL_USER_BY_USERINTID_REPLY			7311	//	2[s|M()] - errCode[errDesc|M(userBody)]

// PYR-89650 Client -> Regulator
#define MSG_REGULATOR_MIGRATION_CHECK_ONLY						7314	// 44 - migrFlag,migrSiteId
#define MSG_REGULATOR_MIGRATION_CHECK_ONLY_REPLY				7315	// 2[s|4M()] - errCode,[errDescr| targetLicenseId licenseSpecificBody (.CH: s – token)]

// PYR-89470 not yet implemented, to be removed after removed from admin code
// PYR-46312 refactored and used
#define MSG_REGULATOR_CHECK_USER_FISCALCODE_UNIQUE              7316 // 44ss - reqId userIntId userId fiscalCode
#define MSG_REGULATOR_CHECK_USER_FISCALCODE_UNIQUE_REPLY        7317 // 46[s] - reqId, errCode [errDescr]

// PYR-92697 Admin -> regulator
#define MSG_REGULATOR_ADMIN_USPA_UPDATE_USER_BLACKLIST           7318	// 44<s>4b - reqId, size<userId>, eRegulatorUsPaBlackListRequestOptions, setNotRemove
#define MSG_REGULATOR_ADMIN_USPA_UPDATE_USER_BLACKLIST_REPLY     7319	// 42[s|4<M(s2[s])>] - reqId, errCode,[errDesc | size<M(userId, errCode, [errDesc])>]

// 5 pairs below can be used for Central-to-Peripheral r-2-r requests after 2019.Build.06 is settled on all peripheral regulators
#define MSG_REGULATOR_LINKED_Reserved_1      					7320 //
#define MSG_REGULATOR_LINKED_Reserved_1_REPLY					7321 //
#define MSG_REGULATOR_LINKED_Reserved_2      					7322 //
#define MSG_REGULATOR_LINKED_Reserved_2_REPLY					7323 //
#define MSG_REGULATOR_LINKED_Reserved_3      					7324 //
#define MSG_REGULATOR_LINKED_Reserved_3_REPLY					7325 //
#define MSG_REGULATOR_LINKED_Reserved_4      					7326 //
#define MSG_REGULATOR_LINKED_Reserved_4_REPLY					7327 //
#define MSG_REGULATOR_LINKED_Reserved_5      					7328 //
#define MSG_REGULATOR_LINKED_Reserved_5_REPLY					7329 //

// PYR-102822 Client -> Regulator
#define MSG_REGULATOR_REACTIVATE_DORMANT_ACCOUNT                7330 // empty
#define MSG_REGULATOR_REACTIVATE_DORMANT_ACCOUNT_REPLY          7331 // 6[s] - errCode[errDescr]

// PYR-113281
#define MSG_REGULATOR_ADMIN_US_UPLOAD_BLACKLIST					7332 // 44ss - reqId, licenseId, fileName, fileContent
#define MSG_REGULATOR_ADMIN_US_UPLOAD_BLACKLIST_REPLY			7333 // 46[s] - reqId, errCode [errDescr]

// PYR-109066 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_GET_BLACKLIST_MONITORS				7334 // 44<4> - reqId, size<monitorId>
#define MSG_REGULATOR_ADMIN_GET_BLACKLIST_MONITORS_REPLY		7335 // 42[s|4<M(44t)>] - reqId, errCode, [errDesc|size<M(monitorId, eBlacklistMonitorStatus, lastChecked)>]

// PYR-109118 Lobby -> Regulator
#define MSG_REGULATOR_ROFUS_GAMBLERCHECK						7336 //44ss - reqId, userIntId, userId, cpr number
#define MSG_REGULATOR_ROFUS_GAMBLERCHECK_REPLY					7337 //46[s|4] - reqId, errCode, [errDescr|ERofusStatus]

// PYR-60240 wRMF -> Regulator
#define MSG_REGULATOR_ES_IDSCAN_DOCUMENT_SUBMISSION				7338 //s - journeyId
#define MSG_REGULATOR_ES_IDSCAN_DOCUMENT_SUBMISSION_REPLY		7339 //2[s] - errCode, [errDescr]

// PYR-117281 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_MIGRATION_CHECK						7340 // 44s4 - reqId, userIntId, userId, targetSiteId
#define MSG_REGULATOR_ADMIN_MIGRATION_CHECK_REPLY				7341 // 42[s] - reqId, errCode[errDesc]

// PYR-117281 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_MIGRATION_PERFORM					7342 // 44s4 - reqId, userIntId, userId, targetSiteId
#define MSG_REGULATOR_ADMIN_MIGRATION_PERFORM_REPLY				7343 // 42[s] - reqId, errCode[errDesc]

// PYR-106575 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_PGAD_REPORT_USERS_DEPOSIT_LIMITS		7344 // 44<4> - reqId, size<userIntId>
#define MSG_REGULATOR_ADMIN_PGAD_REPORT_USERS_DEPOSIT_LIMITS_REPLY	7345 // 42[s|4<M(42[s])>] - reqId, errCode, [errDesc|size<M(userIntId, errCode, [errDesc])>]

// PYR-121976 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_REVOKE_SELF_EXCLUSION				7346 // 44s4 - reqId, userIntId, userId, currSelfExclId
#define MSG_REGULATOR_ADMIN_REVOKE_SELF_EXCLUSION_REPLY			7347 // 42[s] - reqId, errCode[errDesc]

// PYR-118031 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_REVERSE_MIGRATION_CHECK				7348 // 44s4 - reqId, userIntId, userId, targetLicenseId
#define MSG_REGULATOR_ADMIN_REVERSE_MIGRATION_CHECK_REPLY		7349 // 42[s] - reqId, errCode[errDesc]

// PYR-118031 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_REVERSE_MIGRATION_PERFORM			7350 // 44s44 - reqId, userIntId, userId, targetLicenseId, eUserMigrationFlags
#define MSG_REGULATOR_ADMIN_REVERSE_MIGRATION_PERFORM_REPLY		7351 // 42[s] - reqId, errCode[errDesc]

// PYR-112096 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_US_BLACKLIST_CHECK					7352 // 44s - reqId, userIntId, userId
#define MSG_REGULATOR_ADMIN_US_BLACKLIST_CHECK_REPLY			7353 // 42[s|b] - reqId, errCode[errDesc|isBlacklisted]

// PYR-112096 Admin -> Regulator
#define MSG_REGULATOR_ADMIN_US_CLOSE_ACCOUNT_IF_BLACKLISTED			7354 // 44s - reqId, userIntId, userId
#define MSG_REGULATOR_ADMIN_US_CLOSE_ACCOUNT_IF_BLACKLISTED_REPLY	7355 // 42[s|4] - reqId, errCode[errDesc|eAdminCloseAccountIfBlacklistedResult]

// PYR-124003 Client -> Regulator
#define MSG_REGULATOR_VALIDATE_REGISTRATION_COUNTRY				7356 // M()s4 - M(loginExtraData), country, localeId
#define MSG_REGULATOR_VALIDATE_REGISTRATION_COUNTRY_REPLY		7357 // 2[s] - errCode, [errDescr]

#endif //regulator_h_included
