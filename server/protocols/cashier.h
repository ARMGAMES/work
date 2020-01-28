//cashier.h
#ifndef cashier_h_included
#define cashier_h_included
#include "cashtypes.h"

// chipsInfoBitMask constants
//#define chipsInfoBitWuEligible			1  // obsolete after PYR-45735
//#define ageVerificationNeededForSolo	(1 << 1) //2
#define gcDebitEligible					(1 << 2) //4
#define gcDebitStateMask				(3 << 3) //24
// values of gcDebitState ///////
#define gcDebitStateInit				0
#define gcDebitStateRegistered			(1 << 3) //8
#define gcDebitStateVerified			(2 << 3) //16
#define gcDebitStateConfirmed			(3 << 3) //24

#define hideSelfLimits					(1 << 5) //32
#define ageCanBeVerified				(1 << 6) //64
//#define nameCheckNeeded				(1 << 7) //128 // obsolete after #9832
#define firstLastNamesWereSplitFromFull	(1 << 8) //256 // #22265
#define hasDepositedViaExternalPaysystem	(1 << 9) //512 // PYR-70366
/////////////////////////////////

enum eAristotleExternalRequestPurpose
{
	eAristotleRequestForAgeVerification = 0,
	eAristotleRequestForAuthentication = 1,
	eAristotleRequestForScreening = 2,
};

/////////////////////////////////// #18100 - Bank Account Detail Validation Error Code ////////////////
enum eBankDetailValidationErrorCode
{
    eDbtWireBankValNoError = 0,
	eDbtIbanCountriesIncorrectIban = 1,
	eDbtIbanCountriesIncorrectBIC,
	eDbtIbanCountriesIncorrectIbanAndBIC,
    eDbtUKCountriesIncorrectBankAccount,
    eDbtUKCountriesIncorrectBankSortCode,
    eDbtUKCountriesIncorrectBankAccountAndSortCode,
    eDbtNonIbanCountriesIncorrectBankAccount,
    eDbtNonIbanCountriesIncorrectBankSortCode,
    eDbtNonIbanCountriesIncorrectBankAccountAndSortCode,
    eWireIbanCountriesIncorrectIban,
    eWireIbanCountriesIncorrectBIC,
    eWireIbanCountriesIncorrectIbanAndBIC,
    eWireUKCountriesIncorrectIban,
    eWireUKCountriesIncorrectBIC,
    eWireUKCountriesIncorrectIbanAndBIC,
    eWireUSCountryIncorrectSwiftBIC,
    eWireNonIbanCountriesIncorrectSwiftBIC,
    eDbtCountryNotRegistered,
    eDbtWireMissingRequiredFields,
    eBankValInternalError,
    eBankValServiceUnavailable
};

/////////////////////////////////// #18100 - Bank Account Detail Validation CountryPaysystem ////////////////
enum eBankDetailValidationCountryPaysystem
{
    eDbtIbanCountries = 0,
    eDbtUKCountries,
    eDbtNonIbanCountries,
    eWireIbanCountries,
    eWireUKCountries,
    eWireUSCountry,
    eWireNonIbanCountries
};

enum eCashierExtraInfoType // MSG_CASHIER_CHIPS_INFO2_REPLY
{
	eCashierExtraInfoType_Greek = 0, // PYR-28936 Body: s88 currency=EUR,dailyProfit,NAC
	eCashierExtraInfoType_UserHasCashoutHistory = 1, // PYR-44166, .BE license only. Body: b hasCashoutHistory
	eCashierExtraInfoType_IndiaLimits4Pan = 2, // PYR-53001, .IN license, for users with unverified PAN. Body: 444 lifetimeDepositsTotal, panWarningLimit (might be PINT32_MAX), panErrorLimit
	eCashierExtraInfoType_Asia = 3, // PYR-52150, .Asia license. Body: s - externalOperatorId
};

enum ePlayMoneyPurchaseItemFlags // MSG_CASHIER_PM_PURCH_ITEMS_AVAIL_REPLY
{
	ePlayMoneyPurchaseItemFlag_Popular = (1 << 0),
};

enum eDepositCheckReplyFlags // MSG_CASHIER_DEPOSIT_CHECK_REPLY
{
	eDepositCheckReplyFlag_ExtraInfoRequired = (1 << 0), // PYR-31414
	eDepositCheckReplyFlag_HasPayLuckyCard   = (1 << 1),
	eDepositCheckReplyFlag_HasSightlineCard  = (1 << 2), // PYR-39651
};

enum ePlayMoneyRefillResult // PYR-18558
{
	ePmRefillSucceeded = 1,
	ePmRefillAllowenceExceeded = 2,
	ePmRefillGreaterThanTarget = 3
};

enum ePlayMoneyAdWatchResult // PYR-45097
{
	ePlayMoneyAdWatchResult_Undefined  = 0,
	ePlayMoneyAdWatchResult_Complete   = 1,
	ePlayMoneyAdWatchResult_Incomplete = 2,
	ePlayMoneyAdWatchResult_Error = 3,
	// TODO for client - please provide additional types
	ePlayMoneyAdWatchResult_Last = ePlayMoneyAdWatchResult_Error // please keep up-to-date
};

enum ePaysystemsAvailFlags // MSG_CASHIER_PAYSYSTEMS_AVAIL_REPLY
{
	ePaysystemsAvailFlag_None = 0,
	ePaysystemsAvailFlag_CardIOEnable = (1 << 0), // PYR-30314
	ePaysystemsAvailFlag_RmNokAlowed  = (1 << 1), // PYR-33629 User's RMF data is prepopulated by PayPal reply. Deposit request can be done for RMNOK user.
};

enum eCashierPreCashinReplyFlags // MSG_CASHIER_PRE_CASHIN2_REPLY
{
	eCashierPreCashinReplyFlags_None = 0,
	eCashierPreCashinReplyFlags_ReqAuth = (1 << 0), // PYR-51865 Strong Customer Authentication required
};

/*
transaction statuses:
'P' - Pending
''  - Pending
'R' - Rejected
'A' - Approved
'M' - Manual cashin pending (Pending all the same)
*/




////////////////////////////////////////////////////////////////////////////////
// from client to cashier
//#define MSG_CASHIER_CLIENT				5000 // msgBody (PYR-73493: obsolete)
//#define MSG_CASHIER_CLIENT_REPLY		5001 // 6[s|msgBody]

#define cashierDefaultBalance			1
#define cashierPermanentBalance			2

enum eCashierSavedCardFlags // bits in flagsForClient subBodies in MSG_CASHIER_GET_SAVED_CARDS_REPLY
{
	eCashierSavedCardFlag_NotRemovable  = (1 << 0), // PYR-24538: for ECHECK and ELV
	eCashierSavedCardFlag_IsFastDeposit = (1 << 1), // PYR-24538: this card is currently used as fast deposit one
	eCashierSavedCardFlag_CurrencyValid = (1 << 2), // PYR-24538: info about the last used processor currency is known, if not, then client won't show it among FD options
	eCashierSavedCardFlag_ValidationRequired = (1 << 3), // PYR-30281: set when ELV_SEPA CARDINFO entry exists but not eCardInfoTableFlag_Validated
	eCashierSavedCardFlag_HostileBin         = (1 << 4), // PYR-60658: hostile CC bin
	eCashierSavedCardFlag_Verified		= (1 << 5), // PYR-122184: card is verified (initially for CZ only)
};

enum eCashierDepositCheckFlags // bits in flags in MSG_CASHIER_DEPOSIT_CHECK
{
	eCashierDepositCheckFlag_IsFastDeposit  = (1 << 0),

	eCashierDepositCheckFlag_Last = eCashierDepositCheckFlag_IsFastDeposit // please keep up-to-date
};

enum eCashierChipsInfoFlags // bits in flags in MSG_CASHIER_CHIPS_INFO2
{
	eCashierChipsInfoFlag_BonusRollAware  = (1 << 0), // PYR-31834

	eCashierChipsInfoFlag_Last = eCashierChipsInfoFlag_BonusRollAware // please keep up-to-date
};

enum eClientDepositFlags // bits from client in MSG_CASHIER_CASHIN_CC, MSG_CASHIER_CASHIN_HOSTED and MSG_CASHIER_CASHIN_NT
{
	eClientDepositFlags_Deal = (1 << 0),

	eClientDepositFlags_Last = eClientDepositFlags_Deal // please keep up-to-date
};

enum eCashierRollProductFlags
{
	eCashierRollProductsFlags_None				= 0, // PYR-31838
	eCashierRollProductsFlags_Casino			= 1 << 0, // PYR-31838
	eCashierRollProductsFlags_SportsBook		= 1 << 1, // PYR-31838
	eCashierRollProductsFlags_Poker				= 1 << 2, // PYR-47010
};

enum eCashierRollSettingFlags
{
	eCashierRollSettingFlags_ThereIsActiveUserRoll	= 1 << 0, // PYR-31838
};

// Flags to simplify their using in cashier code. Hopefully 32 states is enough
enum eCashierUserRollStateNotif
{
	eCashierUserRollStateNotif_None				= 0,	  // PYR-97802
	eCashierUserRollStateNotif_Issued			= 1 << 0, // PYR-31838
	eCashierUserRollStateNotif_AboutToExpire	= 1 << 1, // PYR-31838
};

// Flags to simplify their using in cashier code. Hopefully 32 states is enough
enum eCashierGameTicketStateNotif
{
	eCashierGameTicketStateNotif_None			= 0,	  // PYR-97802
	eCashierGameTicketStateNotif_Issued			= 1 << 0, // PYR-46074
	eCashierGameTicketStateNotif_AboutToExpire	= 1 << 1, // PYR-46074
};

// Flags to get text data for various dialogs (see MSG_CASHIER_GET_TEXT)
enum eCashierGetTextFlags
{
	eCashierGetTextFlags_RealMoneyPopup         = 1 << 0, // PYR-36193: reply M(sss) as M(title, text, button)

	eCashierGetTextFlags_Last = eCashierGetTextFlags_RealMoneyPopup // please keep up-to-date
};

enum eCashierPreCashoutReplyFlags // MSG_CASHIER_PRE_CASHOUT_REPLY
{
	eCashierPreCashoutReplyFlags_ShowWarningForUserRolls			= 1 << 0, // PYR-36390
};

enum eCashierPreCashoutReplyOptionFlags // MSG_CASHIER_PRE_CASHOUT_REPLY, for OvercreditMessageBody/RollbackCardsBody
{
	eCashierPreCashoutReplyOption_NotVerifiedAccount = 1 << 0, // PYR-47572, payment method is not verified
};

// PYR-41208 Astropay Conditional for CashierPreCashout
enum eAstropayConditionalPreCashoutFlags
{
	eAstropayDirectCashoutNotFound = 0,
	ePSPAYAstropayDirectCashoutFound = 1 << 0,
	eFTPPAYAstropayDirectCashoutFound = 1 << 1,
	eBOLETOAstropayDirectCashoutFound = 1 << 2,
};

// PYR-47572
enum eCashinCompleteSpecificInfo // MSG_CASHIER_CASHIN_CC_COMPLETE/MSG_CASHIER_CASHIN_NT_COMPLETE/MSG_CASHIER_CASHIN_DATA_NEEDED
{
	eCashinCompleteSpecificInfo_UnverifiedPayment = 1, // PYR-47572, M()  - empty body
	eCashinCompleteSpecificInfo_AdditionalText    = 2, // PYR-47572, M(s) - M(text)
	eCashinCompleteSpecificInfo_Worldpay3DS2_JWT  = 3, // PYR-80344, M(s) - M(JWT)
	eCashinCompleteSpecificInfo_Worldpay3DS2_Redirect = 4, // // PYR-80344, M(sss) - M(redirectUrl, JWT, MD)
	eCashinCompleteSpecificInfo_ChildTransId = 5, // PYR-82449 , M(4) - M(childTransId)
	eCashinCompleteSpecificInfo_Secure3DMethodForm = 6, // PYR-80351, M(s) - M(Secure3DMethodForm, transId)
	eCashinCompleteSpecificInfo_Xpay3DS2 = 7, // PYR-78663 M(s) - M(JWT)
};

// PYR-47521
enum eFullFrCashoutFlags // MSG_CASHIER_FULL_FR_CASHOUT_BY_ADMIN
{
	eFullFrCashoutFlags_MarkedForCashoutOnly = 1 << 0,
	eFullFrCashoutFlags_ReportCashout        = 1 << 1, // generate report as well
	eFullFrCashoutFlags_AddUserComment       = 1 << 2, // add user comment
};

// PYR-56040
enum eUserExtraCashierOptions // MSG_CASHIER_GET_USER_PRIVS_2_REPLY
{
	eUserExtraCashierOptions_None               = 0,
	eUserExtraCashierOptions_BlockedCashin      = 1 << 0, // set using same logic as eRegulatorUserOption_CashinBlocked in regulator
};

// PYR-59305 client -> cashier
enum eCashierNetReportBriefCategory // MSG_CASHIER_GET_NET_REPORT_BRIEF
{
	eCashierNetReportBriefCategory_TotalDeposits		= 1,
	eCashierNetReportBriefCategory_TotalWithdrawals		= 2,
	eCashierNetReportBriefCategory_NetTransfers			= 3, // totalTransfersIn-totalTransfersOut
	eCashierNetReportBriefCategory_TotalAdminCredits	= 4,
	eCashierNetReportBriefCategory_NetDepositsWithdrawals = 5, // totalDeposits-totalWithdrawals
	eCashierNetReportBriefCategory_Hwm						 // always the last element
};

// PYR-62177 cashier -> client
enum eRescuePaysystemFlags // MSG_CASHIER_GET_RESCUE_PAYSYSTEMS
{
	eRescuePaysystemFlags_None               = 0,
	eRescuePaysystemFlags_Deposited          = 1 << 0, // PYR-62177 user has deposited with this paysystem

	eRescuePaysystemFlags_Last = eRescuePaysystemFlags_Deposited // please keep up-to-date
};

enum eClientNotificationCodeEx // MSG_NOTIFICATION_TO_CLIENT
{
	eClientNotificationCodeEx_ReplySent = 1, // PYR-37328 specificMsgBody: 4 - replyMsgId, M - replyBody // for 2018.Main.02 implemented only replyMsgId=MSG_CASHIER_STARSWALLET_REGISTER_REPLY
};

enum eUserPaysystemConsent // PYR-70835 MSG_CASHIER_SAVE_PAYSYSTEM_CONSENT
{
	eUserPaysystemConsent_No = 1,
	eUserPaysystemConsent_Yes = 2
};

enum ePaymentSpecificData : UINT32 // PYR-80344 see 'struct PaymentSpecificData'
{
	ePaymentSpecificData_Worldpay3DS2_ReferenceId = 1, // PYR-80344 used in MSG_CASHIER_PRE_CASHIN2, MSG_CASHIER_CASHIN_CC

	ePaymentSpecificData_StarswalletMtan = 4, // PYR-51865
	ePaymentSpecificData_ApplePay = 5, // PYR-36843, M(ss) - M(merchantId,encrPaymentToken)
	ePaymentSpecificData_CbsUrl = 6, // PYR-90717, M(s) - M(cbsUrl)
	ePaymentSpecificData_3dFormDone = 7, // PYR-80351, M(4) - M(transId) - obsolete
	ePaymentSpecificData_ClientCapability = 8, // PYR-80352, M(sssss ssss) - M(clientAcceptHeader, clientJavaEnabled, clientScreenColorDepth, clientScreenHeight, clientScreenWidth,
	                                           // PYR-92093                   clientUserAgent, clientTzOffsetInMin, clientBrowserLocale, clientChallengeWinSize)
};

// PYR-119779 - ApplePay payment capabilities
enum ePKMerchantCapability : UINT32
{
	ePKMerchantCapability3DS	= 1UL << 0, // 1
	ePKMerchantCapabilityEMV	= 1UL << 1, // 2
	ePKMerchantCapabilityCredit	= 1UL << 2, // 4
	ePKMerchantCapabilityDebit	= 1UL << 3, // 8
};

// PYR-91990 indicator for MSG_CASHIER... message number range start
#define MSG_CASHIER_RANGE_START 5000

/*
//from client to cashier, FirePay
#define MSG_CASHIER_CASHIN_FP				5002 ////s4s21 userId, amount, cardNum, expYear, expMonth
// cashier to client
#define MSG_CASHIER_CASHIN_FP_CONFIRM		5003 // 6[4|s] err_code [transactionid | errDescr ]

#define MSG_CASHIER_CASHIN_FP_COMPLETE		5004 // 6[s|???]
*/
/*
//from client to cashier
#define MSG_CASHIER_HISTORY				5009 //s userId
// cashier to client
#define MSG_CASHIER_HISTORY_REPLY		5010 //6[s|4<T1144s412<114s>>] err_code, [errDescr | numTransactions<DateTime, type, paySystem, requestedAmount, returnedAmount, reference, transId, status, numDetails<type, action, amount, reference>>]
//type([P]urchase/[R]edeem), 
//paySystem([V]isa/[M]aster/[F]irePay/[P]ayPal
//status([P]ending/[A]pproved/[R]ejected)
//Detail = 114s type, action, amount, reference
///// type([C]redit card/Che[Q]ue/[P]ayPalRedeem/[B]onus, bonus[F]pp)
///// action(ChequeCa[N]celled/Cheque[I]ssued/Card[C]harged/Card[R]olledback/[P]ayPalRedeem)
*/
//from client to cashier, Credit Card
#define MSG_CASHIER_CASHIN_CC				5011 ////s41ss214ss4bbb444MB userId, amount, paySystem, cardNum, cardCvv, expYear, expMonth, bonusId, bonusCode acctCur grandTotal isFastDep saveFastDepInfo ignorePrevDeclines cardInfoId awardId flags paysystemSpecificBody, blockWithEncryptedCardNumAndCvv

// cashier to client
#define MSG_CASHIER_CASHIN_CC_CONFIRM		5012 // 6[s|4s] err_code [errDescr | transactionid, message ]
#define MSG_CASHIER_CASHIN_CC_COMPLETE		5013 // 6[s|4142<114s4>4] err_code [errDescr | transactionid, status, amount, numDetails<type, action, amount, reference, fee> emailCode]
// also, see MSG_CASHIER_CASHIN_DATA_NEEDED / MSG_CASHIER_CASHIN_DATA_PROVIDED
//status([P]ending/[A]pproved/[R]ejected)
//Detail = 114s4 type, action, amount, reference, systemFee
///// type([C]redit card/Che[Q]ue/[P]ayPal) see also eResultInfoType in "gateway.h"
///// action(ChequeCa[N]celled/Cheque[I]ssued/Card[C]harged/Card[R]olledback/[P]ayPalRedeem) see also eResultInfoAction in "gateway.h"

// if (err=ERR_REPEATED_CASHIN_DECLINE) errDescr, cardTypeByte, prevFailedTransId, optionFlags (see eRepeatedCashinDeclineOption), n=4<MsgBody(optionText)>

//from client to cashier
#define MSG_CASHIER_CASHOUT				5005 //s411s4ss48 userId, amount, paySystem, awareOfBonus, currency, convRate32(obsolete after PYR-54917), balanceCurrency, maskedReference, clientBalance, convRate64
// cashier to client
//#define MSG_CASHIER_CASHOUT_CONFIRM		5007 // 6[s|4] err_code [errDescr | transactionid ]

// cashier to client
#define MSG_CASHIER_CASHOUT_COMPLETE		5008 // 6[s41|4142<114s>] err_code [errDescr prevAmount prevSystemByte| transactionid, status, amount, numDetails<type, action, amount, reference>]
//status([P]ending/[A]pproved/[R]ejected)
//Detail = 114s type, action, amount, reference
///// type([C]redit card/Che[Q]ue/[P]ayPalRedeem)
///// action(ChequeCa[N]celled/Cheque[I]ssued/Card[C]harged/Card[R]olledback/[P]ayPalRedeem)
/*
//from client to cashier, PayPal
#define MSG_CASHIER_CASHIN_PP				5014 ////s4 userId, amount
// cashier to client
#define MSG_CASHIER_CASHIN_PP_REPLY			5015 // 6[s|s4<ss>] err_code [errDescr | actionUrl, numPairs<name, value> ] please see INDICATE_BONUS_AWARENESS description in "dbm.h"
*/
#define MSG_TIMER_REPEAT_PURCHASE_REQUEST	5016 //4 transid
#define MSG_TIMER_REPEAT_REDEEM_REQUEST		5017 //4 transid

/*
///// admin "bonus day" API
#define MSG_ADMIN_GET_CURRENT_BONUS			5018 // 4 reqId
#define MSG_ADMIN_GET_CURRENT_BONUS_REPLY	5019 // 46222 reqId, bonusPercent(*100), fppRatio(*100), handsForCashout

#define MSG_ADMIN_SET_CURRENT_BONUS			5020 // 4222 reqId, bonusPercent(*100), fppRatio(*100), handsForCashout
#define MSG_ADMIN_SET_CURRENT_BONUS_REPLY	5021 // 46[s] reqId, errCode [, errDescr]

#define MSG_ADMIN_CANCEL_CURRENT_BONUS			5022 // 4 reqId
#define MSG_ADMIN_CANCEL_CURRENT_BONUS_REPLY	5023 // 46[s] reqId, errCode [, errDescr]
*/
#define MSG_TIMER_POST_COMPLETED_BAD_CASHIN		5024 //44s transId banTypeInfo bannedParent

#define MSG_TIMER_REPEAT_OVERCREDIT_REQUEST		5025 //4 transid

#define MSG_TIMER_GET_CHIPS_FULL_INFO_DELAY		5026 //44 transId, reqId
/*
#define MSG_CASHIER_EXERCISE_BONUS			5028	//s4 userId, amountToExercise
#define MSG_CASHIER_EXERCISE_BONUS_REPLY	5029	// 6[s]
*/
//#define MSG_CASHIER_PRE_CASHIN				5030 //s4 userId, cashinAmount
//#define MSG_CASHIER_PRE_CASHIN_REPLY		5031 //6[s|444] eligibleDepositAmount, potentialBonusAmount, fppToExercise

#define MSG_CASHIER_CASHIN_NT			5034 //sss44 userId netAccount secureId transAmount bonusId
#define MSG_CASHIER_CASHIN_NT_CONFIRM	5035 // 6[s|4] err_code [errDescr | transactionid ]
#define MSG_CASHIER_CASHIN_NT_COMPLETE	5036 // 6[s|4142<114s>] err_code [errDescr | transactionid, status, amount, numDetails<type, action, amount, reference>]

#define MSG_ADMIN_CASHIER_REINIT		5037 // 4 reqId
#define MSG_ADMIN_CASHIER_REINIT_REPLY	5038 // //46[s|4<M(ssss)>] reqId err_code[errDescr|listSize<MessageBody(sectionName itemName resultingItemValue dbItemValue)>
/* obsolete
#define MSG_CASHIER_HISTORY1			5039 //s userId
#define MSG_CASHIER_HISTORY1_REPLY		5040 //6[s|4<Ts44s4>] //err_code, [errDescr | numTransactions<DateTime, typeDescr, amount, fee, reference, transId>]
*/
#define MSG_CASHIER_PRE_CASHIN2			5041 //s41sssM4bsB userId, cashinAcctAmount, paysystemByte, procCurrency, bonusCode, acctCurrency validationBody processorAmount nonZeroPaysystemFeeApplies, cardNum blockWithEncryptedCardNum
#define MSG_CASHIER_PRE_CASHIN2_REPLY	5042 //6[s|444444sTTs 44444Ts444bMMM4] potentialBonusAmount, fppToExercise, maxPossibleCashinAmount, bonusId, bonusPercent, bonusMax, bonusName, bonusStart, bonusEnd, bonusUrl,
											 //				  outstandingCashout_obsolete=0, releasePortion, fppReleasePortion, bonusFlags, maxNumDepositsLeft, lastAllowedDepositTime, bonusCurrency, daysToDeposit, maxDeposits, acctAmount, beDepositExceedsLossLimit, ticketOnlyBonusBody, rollBody, disclaimerMsgBody
											 //				  eCashierPreCashinReplyFlags
// M ticketOnlyBonusBody = filled if ticketOnly flag: 4s444  numOfTickets, admission, numTicketsPerPortion, maxNumTickets, releasePortion
// M rollBody is filled only if bonus is bound to Roll: 44s4s4s44444 rollId rollAmtInRollCur rollCur rollAmtInAcctCur acctCur cashPrice cashPriceCur rollProducts clearPoints clearBets redemptionLimit clearStakes
// M disclaimerMsgBody is filled only if a disclaimer message should be displayed: ss4<M(s)> header text numOfButtons buttons
/*
#define MSG_CASHIER_USER_BONUSHIST			5043 //s userId
#define MSG_CASHIER_USER_BONUSHIST_REPLY	5044 //6[s|2<sT444444>] errCode[errDescr | numItems<bonusName when transId amount fppToCollect fppCollecred amountCredited creditTransId>]
*/
#define MSG_CASHIER_PRE_CASHOUT			5045   //s4sb1 userId amount balanceCurrency isForFTP userRollbackOption
											   // userRollbackOption used only for Admin connections
#define MSG_CASHIER_PRE_CASHOUT_REPLY	5046   //6[s|1ss4442s22<ss4>4<M>4<M>44]
// err_code[errDescr | needsBonusWarning bonusName bonusUrl bonusPercent, bonusMaxAmount estimatedRollback 0(was numLastCashins) userPhone 0(was numOvercr) 0(was numCardsToRollback) cashOutOptionSize<OvercreditMessageBody> numCardsToRollback<RollbackCardsBody> eCashierPreCashoutReplyFlags taxAmount
// OvercreditMessageBody is 1Tss414ss8sb44 paySystemByte co.when maskedRef procCurr obsoleteConvRate isAllowed obsoleteLimitAmount explanation psI18n convRate64 additionalText isTagged CC_ID eCashierPreCashoutReplyOptionFlags
// RollbackCardsBody is ss418s444 paysystemRBi18n cardMasked amountRB cardTypeByte convRate64 cardCurrency processorAmount CC_ID eCashierPreCashoutReplyOptionFlags

//#define MSG_CASHIER_ADMIN_SETTINGS		5047 //44<ss4> reqId num<gateway cardType percent>
#define MSG_CASHIER_ADMIN_SETTINGS		5047 //4s reqId scriptText (with CRs)
#define MSG_CASHIER_ADMIN_SETTINGS_REPLY	5048 //46[s]
/* obsolete
#define MSG_CASHIER_USER_FPPSTORE			5049   //nothing
#define MSG_CASHIER_USER_FPPSTORE_REPLY		5050   // 4<s444<s4<s>>> numItems<itemdescr fpps imageId numSpecs<specName numValues<value>>>
*/
// [AO 2010-02-03] Obsolete
// #define MSG_VIPSTORE_USER_ORDER_HISTORY			5051 //s userId
// #define MSG_VIPSTORE_USER_ORDER_HISTORY_REPLY	5052 //6[s|4<(T)ss>] err_code [errDescr | numOrders <when Description status> ]
/*
// [AO 2009-06-29] Obsolete - returns an error
#define MSG_CASHIER_FPP_PURCHASE				5053	//s444<s> userId itemId fpp numSpecs<specValue>
#define MSG_CASHIER_FPP_PURCHASE_REPLY			5054	//6[s|4446] errCode [errDescr | fppAfterPurchase, fppToSpend, specialType, fppCentsAfterPurchase]
*/
/* obsolete
#define MSG_CASHIER_GET_IMAGE					5055	//4 imageId
#define MSG_CASHIER_GET_IMAGE_REPLY				5056	//4B imageId, image
*/
// [AO 2009-05-06] Obsolete
// #define MSG_CASHIER_REFRESH_STORE_ITEMS			5057	//4 reqId
// #define MSG_CASHIER_REFRESH_STORE_ITEMS_REPLY	5058	//46[s] reqId err_code(errDrescr)

//[VS 20090618] Obsolete
//#define MSG_CASHIER_CANCEL_CASHOUTS				5059	//s4s userId, bonusId, bonuscode
//#define MSG_CASHIER_CANCEL_CASHOUTS_REPLY		5059	//6[s]

//from admin
#define MSG_CASHIER_SET_CONVERSION_RATE			5060	//
#define MSG_CASHIER_SET_CONVERSION_RATE_REPLY	5061

#define MSG_CASHIER_FINALIZE_CCOIN				5062	//44<41s> reqId, numTrans<transId oldStatus gateway>
#define MSG_CASHIER_FINALIZE_CCOIN_REPLY		5063	//46[s|4<46s>] reqId numTrans<transId,err_code,errDrescr>

#define MSG_CASHIER_FINALIZE_NETELLER			5064	//44<41s> reqId, numTrans<transId oldStatus gateway>
#define MSG_CASHIER_FINALIZE_NETELLER_REPLY		5065	//46[s|4<46s>] reqId numTrans<transId,err_code,errDrescr>
/* obsolete
#define MSG_CASHIER_UPDATE_BIN					5066	//4ss1 reqId, BIN,country,forbidden
#define MSG_CASHIER_UPDATE_BIN_REPLY			5067    //46[s]
*/
#define MSG_CASHIER_GET_WU_SCHEMA				5068	//	?
#define MSG_CASHIER_GET_WU_SCHEMA_REPLY			5069	//	4 //currentWuSchema 0=old 1=automated

#define MSG_CASHIER_INIT_WU_TRANS				5070	//	s441 userId procAmount oldWuTransId useOldNotCreateNew // !!!admin also adds: s4 userEmail userLocale userIntId fullName phone city state country
#define MSG_CASHIER_INIT_WU_TRANS_REPLY			5071	//	6[s|sssD] err_code, [errDescr|receiverName country city whenExpired]

#define MSG_CASHIER_PRE_MTCN					5072	//s userId
#define MSG_CASHIER_PRE_MTCN_REPLY				5073	//6 [s|44sss44sTT] err_code [errDescr|0 wuTransId receiverName country city officeId amount currency whenIssued whenExpired]

#define MSG_CASHIER_SET_WU_MTCN					5074	// ssss4 userId, MTCN, userAmount, receiverName, officeId, bonusCode
#define MSG_CASHIER_SET_WU_MTCN_REPLY			5075	// 6[s] err_code [errDescr]

#define MSG_CASHIER_VIP_INFO					5076	// s userId
#define MSG_CASHIER_VIP_INFO_REPLY				5077	//6[s|4D444D444D664<4444>44<M(444)>448] reqId, err_code[errDescr|currentStatus validUntil monthly.counter monthly.fppToNext monthly.nextStatus monthly.timeToEarn yearly.counter yearly.fppToNext yearly.nextStatus yearly.timeToEarn monthly.counterCents yearly.counterCents numLevels<statusId minFpp fppBonusRate vipToKeepYearlyStatus> lastMonthForYearly numStellarBonuses<StellaBonusLevelBody(groupId vppThreshold amount)> previousYearVPP, dailyVpp, ltVppInCents64]
/* obsolete
#define MSG_CASHIER_USER_VIP_FPPSTORE			5078   // 4 - reserve for future
#define MSG_CASHIER_USER_VIP_FPPSTORE_REPLY		5079   // 4<s44444<s4<s>>> numItems<itemdescr fpps imageId vipLevel specialItem numSpecs<specName numValues<value>>>
*/
#define MSG_CASHIER_SAVED_CARD					5080	//ss4 userId paySystem cardInfoId(0 if unknown)
#define MSG_CASHIER_SAVED_CARD_REPLY			5081	//6[s|4sssssD21B] errCode [errDescr | infoId userId paySystem cardNum ssn dLic bDate expYear expMonth blockWithEncryptedCardNum]

// ..._CARD_PERCENTAGES, ..._SUSPENDED_METHODS, _GET_GATEWAYS_INFO are no longer used in cashier's code
// PYR-25756: "Admin to remove obsolete messages to cashier"
/*
#define MSG_CASHIER_GET_CARD_PERCENTAGES		5082	//4ss reqId paySystem
#define MSG_CASHIER_GET_CARD_PERCENTAGES_REPLY	5083	//44<s1> reqId, numGateways <gatewayName percentage>
#define MSG_CASHIER_SET_CARD_PERCENTAGES		5084	//44<s1> reqId, paySystem, numGateways <gatewayName percentage>
#define MSG_CASHIER_SET_CARD_PERCENTAGES_REPLY	5085	//46[s] reqId, err_code[errDescr]
#define MSG_CASHIER_GET_SUSPENDED_METHODS		5086	//4 reqId
#define MSG_CASHIER_GET_SUSPENDED_METHODS_REPLY	5087	//46[s|4<s>] reqId, number<paySystem>
#define MSG_CASHIER_SET_SUSPENDED_METHOD		5088	//4bs reqId add_or_remove paySystem
#define MSG_CASHIER_SET_SUSPENDED_METHOD_REPLY	5089	//46[s]
#define MSG_CASHIER_GET_GATEWAYS_INFO			5090	//4 reqId
#define MSG_CASHIER_GET_GATEWAYS_INFO_REPLY		5091	//46[s|4<s4<s1>>] reqId errCode [errDescr | numGateways<gwName numPaySystems<paySystemName percentage>>]
*/

#define MSG_CASHIER_DAD_REQUEST					5092	//4sss4 dadRequestType useId phone time transactionId //reqType 0 = chat, 1 = phone, 2 = none
#define MSG_CASHIER_DAD_REQUEST_REPLY			5093    //6[s|4] errCode[errDescr|reqType]

#define MSG_CASHIER_GET_GW_SETTINGS				5093	//4 reqId 
#define MSG_CASHIER_GET_GW_SETTINGS_REPLY		5094 	//46[s|2<GatewaySetting>] reqId errCode [errDescr | numUpdates <GatewaySetting>]

#define MSG_CASHIER_UPDATE_GW_CARDS 			5095	//442<GatewaySetting>s reqId numUpdates <GatewaySetting> comment
#define MSG_CASHIER_UPDATE_GW_CARDS_REPLY		5096 	//46[s|2<GatewaySetting>] reqId errCode [errDescr | numUpdates <GatewaySetting>]

#define MSG_CASHIER_ONOFF_GW_CARDS				5097	//442<GatewaySetting>s reqId numUpdates <GatewaySetting> comment
#define MSG_CASHIER_ONOFF_GW_CARDS_REPLY		5098	//46[s|2<GatewaySetting>] reqId errCode [errDescr | numUpdates <GatewaySetting>]

#define MSG_CASHIER_READ_BINS					5099	//4 reqId
#define MSG_CASHIER_READ_BINS_REPLY				5100	//46[s] reqId errCode [errDescr]

#define MSG_CASHIER_WIRE_REQUEST				5101	//s4sss userId amount currency bonusCode personalInfo
#define MSG_CASHIER_WIRE_REQUEST_REPLY			5102	//6[s|sssssssss] err_code [errDescr | paymentReference accountHolder bankName city swift specialId bankAcct country additionalRef]

#define MSG_CASHIER_GCWIRE_NOTIFY				5103	//44<s4sTs4> reqId numItems<paymentRef amount currency timeCompleted uniqueId orderId>
#define MSG_CASHIER_GCWIRE_NOTIFY_REPLY			5104	//44<s4sTs4s6s> reqId numItems<paymentRef amount currency timeCompleted uniqueId orderId userId errCode errDescr>

#define MSG_CASHIER_USER_BONUSHIST_NEW			5105	//s userId
#define MSG_CASHIER_USER_BONUSHIST_NEW_REPLY	5106	//6[s|2<msg:sT444444Ts444>s4<BODY{sss}>] errCode[errDescr | numItems<MsgBody(bonusName when transId amount fppToCollect fppCollecred amountCredited creditTransId expirationDate currency, bonusInTickets, creditedTickets, bonusClearedFlags)>bonusDescrAll,numItems<BODY{name, description, url}>]
																																																						  
#define MSG_CASHIER_GET_EXCLUDED_STATES			5107	//4 reqId 
#define MSG_CASHIER_GET_EXCLUDED_STATES_REPLY	5108	//46[s|4<sss4>] reqId err_code [errStr | numRecords <paysystem country state flag]

#define MSG_CASHIER_ADD_EXCLUDED_STATES			5109	//4ss2<s2<s>> reqId comment paysystem numCountries <country numStates <state>>
#define MSG_CASHIER_ADD_EXCLUDED_STATES_REPLY	5110	//46[s]

#define MSG_CASHIER_DEL_EXCLUDED_STATES			5111	//4ss2<s2<s>> reqId comment paysystem numCountries <country numStates <state>>
#define MSG_CASHIER_DEL_EXCLUDED_STATES_REPLY	5112	//46[s]

#define MSG_CASHIER_AGE_CHECK_UK				5113	//4sssDsssssssssssDsssss4
/*title, fullname, gender, dateofbirth, address, city, zip, state, 
country, passportNumber1, passportNumber2, passportNumber3, 
passportNumber4, passportNumber5, passportNumber6,
passportExprireDate, driverNumber1, driverNumber2, 
driverNumber3,driverNumber4, driverpostcode, attemptNumber*/ // attemptNumber starts with 1

#define MSG_CASHIER_AGE_CHECK_UK_REPLY			5114	//6[s|... b4] ... offerLiveSupport, resultId
/* obsoleted with ..2 revision
#define MSG_CASHIER_GCDEBIT_INIT				5115 	//ssss reqId userId bankCode accNum holderName
#define MSG_CASHIER_GCDEBIT_INIT_REPLY			5116	//6[s]
*/
#define MSG_CASHIER_GCDEBIT_VERIFY				5117	//4s44 reqId userId amount1 amount2
#define MSG_CASHIER_GCDEBIT_VERIFY_REPLY		5118	//46[s]

#define MSG_CASHIER_GCDEBIT_CONFIRM				5119	//4s reqId userId
#define MSG_CASHIER_GCDEBIT_CONFIRM_REPLY		5120	//46[s]

#define MSG_CASHIER_GCDEBIT_NOTIFY             	5120	//44<44s4s4DDsssss> reqId numRec <GCDebitNotification>
#define MSG_CASHIER_GCDEBIT_NOTIFY_REPLY       	5121	//46s
/*
#define MSG_CASHIER_GET_FIREPAY_REJECT_TEXT		5122	// (empty)
#define MSG_CASHIER_GET_FIREPAY_REJECT_TEXT_REPLY		5123 //s text
*/
/* #14576 - pair below is obsolete and not supported by server
#define MSG_CASHIER_UPDATE_GW_NOPURCHASE		5123    //4s4<s> reqId gatewayName numCountries<countryCode>
#define MSG_CASHIER_UPDATE_GW_NOPURCHASE_REPLY  5124	//46[s]
*/
//cashier to client
#define MSG_CASHIER_CASHIN_CC_REDIRECT			5125 // 6[s|44s] err_code [errDescr | transactionid amount url]
//client to cashier
#define MSG_CASHIER_CASHIN_CC_REDIRECTED		5126 // 444s transactionid amount status(0 = ok, not 0 = error) errDescr

//client to cashier
// obsolete after PYR-36980
#define MSG_CASHIER_CHECK_PAYSYSTEM				5127 // 1s4 paySystemByte acctCurrency cardInfoId
#define MSG_CASHIER_CHECK_PAYSYSTEM_REPLY		5128 // sbs4<ss44<s>>4<ss4>b4<4s>44<B>b444bbBBB84<B(s)> paySystem isAvailable message numSubSystems<productId, description, logoImageId, currenciesSize<currency>> numLinks<linkDescr linkUrl linkType> ageVerifyNeeded idsSize<id_type, id_descr> dayToClear numCurrencies<oneRateBody> userHasBonusesWithCodes validationFailuresThreshold selfLimitAmount remainingDepositLimitAmount isFdSupportedByThisPs paysystemFeeApplies<feeBody> <SuggestedDeposits> <paysystemSpecificBody> suggestedDepositFlags size<B(prepopulatedCode)>
//oneRateBody = s8b currencyCode depositRate64
//feeBody = 444444b graceDepositsPeriodDays graceDeposits leftGraceDeposits rate_x_100K minFee maxFee altPaysystemByte // this body composed just if paysystemFeeApplies is true
//SuggestedDeposits = 4 allAvailableCurrencies <s processorCurr <4 roundedValidSuggestedAmounts 4roundedValidSuggestedAmount><b4 openValueBody value>>
//paysystemSpecificBody = 44b minLoadLimit maxLoadLimit hasPayLuckyAccount


#define MSG_CASHIER_GET_PAYSYSTEMMESSAGES		5129 // 4 reqId
#define MSG_CASHIER_GET_PAYSYSTEMMESSAGES_REPLY	5130 // 44<sss>4<ss> reqId numPaySystems<paysystem messageOpen messageClosed>

#define MSG_CASHIER_UPDATE_PAYSYSTEMMESSAGE			5131 // 41ss reqId isForClosed paySystem message
#define MSG_CASHIER_UPDATE_PAYSYSTEMMESSAGE_REPLY	5132 // 46[s]

//client to cashier
#define MSG_CASHIER_CASHIN_HOSTED				5133 ////s4s41s4s1s4sbbb444B userId, amount, currency, converted, paySystem, productId, bonusId, bonusCode doNotCancelCashouts balanceCurrency clientBalance cardNum saveCardInfoRequested fastDepositTransaction saveInfoForFastDeposit cardInfoId awardId flags blockWithEncryptedCardNum
#define MSG_CASHIER_CASHIN_HOSTED_REDIRECT		5134 //6[s|ss4s14<ss>] errCode [errDescription|formMethod formAction transId xTransStr cardTypeByte numPostParams <name,value>]

#define MSG_CASHIER_HOSTED_DONE					5135 // s4s1 userId transId xTransStr isDone
#define MSG_CASHIER_HOSTED_DONE_REPLY			5135 // 6[s]

#define MSG_CASHIER_REQUEST_GW_SETTINGS			5136 //4 reqId
#define MSG_CASHIER_REQUEST_GW_SETTINGS_REPLY	5137 //46[s] reqId errCode[errDescr]

#define MSG_CASHIER_VERIFY_AGE					5138 //[fromAdmin ? s]ssDssssssss4s44 [fromAdmin ? userId] fullname gender DOB phone address city zip county state country SSN4 id_type id_descr attemptNumber(starts with 1) extRequestPurpose(0-age,1-auth,2-screen)
#define MSG_CASHIER_VERIFY_AGE_REPLY			5139 //6s4 - errCode errDescr AGEVERIFYRESULTS.resultId

#define MSG_CASHIER_REQUEST_AGE_VERIFY_SETTINGS 5140 //4 reqId
#define MSG_CASHIER_REQUEST_AGE_VERIFY_SETTINGS_REPLY 5141 //4(...) reqId(ageVerificationSettings)

#define MSG_CASHIER_READ_PAYSYSTEMS_COUNTRIES	5142 // 4 reqId
#define MSG_CASHIER_READ_PAYSYSTEMS_COUNTRIES_REPLY	5143 // ????
/* MONEGRAM obsoleted by #16411
#define MSG_CASHIER_MONEYGRAM_INFO			5144 //s4s userId amount bonus
#define MSG_CASHIER_MONEYGRAM_INFO_REPLY	5145 //6[s|sss4] error_code [errDescr|receiverCode companyCode, uniqueId, requestedAmount]
*/
#define MSG_TIMER_HOSTED_PURCHASE_TIMEOUT		5146 //44 transId, reqId
#define MSG_TIMER_HOSTED_PURCHASE_TIMEOUT_PRELAST	5147 //44 transId, reqId
#define MSG_TIMER_HOSTED_PURCHASE_TIMEOUT_LAST		5148 //44 transId, reqId

//client to cashier
#define MSG_CASHIER_PAYSYSTEMS_AVAIL			5149 //s balanceCurrency
#define MSG_CASHIER_PAYSYSTEMS_AVAIL_REPLY		5150 // 6[s|4<M>b] errCode[errDescr|numPaySystems<onePaySystemBody>dadPreDepositAvailable]
//onePaySystemBody: 1s4444ss44s4b4 - psByte paySystemNameI18n minDeposit maxCashinMonthly maxCashinWeekly maxCashinDaily feeFormula feeDescription maxDeposits depositsHours userLimitText maxCashin1Trans isFdSupportedByPs ePaysystemsAvailFlags
//Note: minDeposit, maxCashinMonthly, maxCashinWeekly, maxCashinDaily, maxCashin1Trans are converted to the currency in request, using our rounding algorithm

#define MSG_CASHIER_GET_TRANSACTION_INFO		5151 //44 reqId transId
#define MSG_CASHIER_GET_TRANSACTION_INFO_REPLY	5152 //46[s|4(tr)] reqId, err_code[errDescr|4 transaction]

#define MSG_CASHIER_UPDATE_TRANSACTION			5153 //44b44 reqId transId updateInDb transReqId internalStatus
#define MSG_CASHIER_UPDATE_TRANSACTION_REPLY	5154 //46[s] reqId, err_code[errDescr]

#define MSG_CASHIER_GET_TRANSACTIONS_OLDER			5155 //44 reqId	seconds
#define MSG_CASHIER_GET_TRANSACTIONS_OLDER_REPLY	5156 //46[s|4<4(tr)4>] reqId, err_code[errDescr|numTrans<internalStatus (transaction) transReqId>]


//admin to cashier
#define MSG_CASHIER_GET_DAD_SETTINGS			5157 //4 reqId
#define MSG_CASHIER_GET_DAD_SETTINGS_REPLY		5158 //4b4444b4444 - reqId,isEnabledNow,hourFromNow,minutesFromNow,hourToNow,minutesToNow,isEnabledINI,hoursFromINI,minutesFromINI,hoursToINI,minutesToINI

#define MSG_CASHIER_SET_DAD_SETTINGS			5159 //4b4444 - reqId,enable,hourFromNow,minutesFromNow
#define MSG_CASHIER_SET_DAD_SETTINGS_REPLY		5160 //46[s] - reqId,errCode[errDescr]

#define MSG_CASHIER_SET_MATCHCODE_PASSED		5161 //44b4 reqId,matchCode,passed,authCode
#define MSG_CASHIER_SET_MATCHCODE_PASSED_REPLY	5162 //46[s] reqId, err_code[errDescr]

#define MSG_CASHIER_GET_MATCHCODES				5163 //4 reqId
#define MSG_CASHIER_GET_MATCHCODES_REPLY		5164 //46{s|44sb} reqId, err_code,{errDescr|size{matchCode,description,passed}...}
/* MONEGRAM obsoleted by #16411
#define MSG_ADMIN_MONEYGRAM_INFO				5165 //4s4s reqId userId amount emptyBonusCode
#define MSG_ADMIN_MONEYGRAM_INFO_REPLY			5166 //46[s] - reqId,errCode[errDescr]
*/ 
/* obsolete
#define MSG_CASHIER_MRMC_SIGNUP					5167 //s userId 
#define MSG_CASHIER_MRMC_SIGNUP_REPLY			5168 //
#define MSG_TIMER_REPEAT_MRMC_SIGNUP			5169 //4(m) clientConnId, mrmcInfo
*/

#define MSG_CASHIER_SAVE_WIRE_CASHOUT_INFO		5170 //see WireAccountInfo::HolderInfo::compose() + WireAccountInfo::BankInfo::compose() in "shared.cpp""
#define MSG_CASHIER_SAVE_WIRE_CASHOUT_INFO_REPLY 5171 //6s6 - errCode errDescr bankValidationErrorCode

// from cashier to client
#define MSG_CASHIER_MBOOKERS_REDIRECT			5172 //64s errCode transId errDescriptionOrUrl
//from client to cashier
#define MSG_CASHIER_MBOOKERS_REDIRECTED			5173 //44s transactionid status(0 = ok, not 0 = error) errDescr

// admin to cashier
#define MSG_CASHIER_REREAD_EMAILS				5174 //44
#define MSG_CASHIER_REREAD_EMAILS_REPLY			5175 //46[s]

#define MSG_CASHIER_REREAD_MSGS  				5176 //44
#define MSG_CASHIER_REREAD_MSGS_REPLY			5177 //46[s]

//client to cashier
#define MSG_CASHIER_GET_USER_LIMIT				5178 //s userId
#define MSG_CASHIER_GET_USER_LIMIT_REPLY		5179 //6[s|7T44s84] err_code [errDescr|weeklyLimit(negative amount means "none") enforcedUntil maxLim minLim selfLimitCurrency RateToUsd regulatoryWeeklyLimit ]
                                                     // regulatoryWeeklyLimit = -1 if no regulatory limit

//client to cashier
#define MSG_CASHIER_SET_USER_LIMIT				5180 //s74877 userId newWeeklyLimit (negative amount means "remove limit")) flags flags2 dailyLimit monthlyLimit
#define MSG_CASHIER_SET_USER_LIMIT_REPLY		5181 //6[s|4T] err_code [errDescr|weeklyLimit enforcedUntil]

/* obsolete by PYR-45735
#define MSG_TIMER_REPEAT_MTCN_VALIDATION		5182 //
*/

#define MSG_TIMER_MBOOK_PURCHASE_TIMEOUT		5183 //4 transid
/* obsolete
#define MSG_CASHIER_GET_UNCLEARED_DEPOSITS			5184 //
#define MSG_CASHIER_GET_UNCLEARED_DEPOSITS_REPLY	5185 //6[s|4<4s4D>]44 errCode [errDescr|numUncleared<transId paysystem amountUSD clearDateTime>] maxUnclearedNum maxUnclearedSum

// client to cashier
#define MSG_CASHIER_PRE_AGE_VERIFY				5186 //
#define MSG_CASHIER_PRE_AGE_VERIFY_REPLY		5187 //b4[4s...]4 ageVerifyNeeded numIds <idType idDescr> processorId
*/
// #define MSG_TIMER_UB_PURCHASE_TIMEOUT			5188 //44 transId, reqId #20342

// admin to cahier
#define MSG_ADMIN_REQUEST_EXCHANGERATES			5189 //4 reqid
#define MSG_ADMIN_REQUEST_EXCHANGERATES_REPLY	5190 //46s req err_code [result|errDescr]

// Client/Admin to cashier (Admin msg has 'reqId' in the beginning)
#define MSG_CASHIER_GET_DBT_FIELDS				5191//s userId
#define MSG_CASHIER_GET_DBT_FIELDS_REPLY 		5192 //6[s|s4<(mb)>4<s>] err_code[errDescr|numFields<fieldMsgBody>numCurrencies<currencyCode>]

// Client/Admin to cashier (Admin msg has 'reqId' in the beginning)
#define MSG_CASHIER_DBT_VALIDATE				5193 //ss4<(mb)> userId currency numFields<fieldMsgBodyWithInfo>
#define MSG_CASHIER_DBT_VALIDATE_REPLY			5194 //6s6 dbtErrorCode dbtErrorStr bankValidationErrorCode

#define MSG_CASHIER_BPAY_REQUEST				5195 //identical to MSG_CASHIER_WIRE_REQUEST
#define MSG_CASHIER_BPAY_REQUEST_REPLY			5196 //6[s|ss] err_code [errDescr | str1 str2 ]

//from admin to cashier
#define MSG_ADMIN_CASHIER_GET_INIDATA			5197 //4 reqId
#define MSG_ADMIN_CASHIER_GET_INIDATA_REPLY		5198 //46[s|4<M(ssss)>] reqId err_code[errDescr|listSize<MessageBody(sectionName itemName resultingItemValue dbItemValue)>
#define MSG_ADMIN_CASHIER_REINIT_IGNORE_DB       5199 //4 reqId
#define MSG_ADMIN_CASHIER_REINIT_IGNORE_DB_REPLY 5200 //46[s|4<M(ssss)>] reqId err_code[errDescr|listSize<MessageBody(sectionName itemName resultingItemValue dbItemValue)>

// admin to cashier
#define MSG_CASHIER_GET_ACH_ROUTE_TBL			5201 //4 reqId
#define MSG_CASHIER_GET_ACH_ROUTE_TBL_REPLY		5202 //46[s|4<s>4<(ar)>] reqId err_code [errDescr | numItems <gwCandidateName> numItems <AchRoute>

#define MSG_CASHIER_SET_ACH_ROUTE				5203 //4(ar) reqId <AchRoute>
#define MSG_CASHIER_SET_ACH_ROUTE_REPLY			5204 //46[s] reqId err_code [errDescr]

#define MSG_CASHIER_ADD_ACH_ROUTE				5205 //44(ar) reqId <AchRoute>
#define MSG_CASHIER_ADD_ACH_ROUTE_REPLY			5206 //46[s] reqId err_code [errDescr]

#define MSG_CASHIER_DEL_ACH_ROUTE				5207 //44<s> reqId <userFilter>
#define MSG_CASHIER_DEL_ACH_ROUTE_REPLY			5208 //46[s] reqId err_code [errDescr]

#define MSG_CASHIER_GET_ACH_FALLBACK_TBL		5209 //4 reqId
#define MSG_CASHIER_GET_ACH_FALLBACK_TBL_REPLY	5210 //46[s|4<s>4<(af)>] reqId err_code [errDescr | numItems <gwCandidateName> numItems <AchFallback>

#define MSG_CASHIER_SET_ACH_FALLBACK			5211 //4(af) reqId <AchFallback>]
#define MSG_CASHIER_SET_ACH_FALLBACK_REPLY		5212 //46[s] reqId err_code [errDescr]

#define MSG_CASHIER_ADD_ACH_FALLBACK			5213 //4(af) reqId <AchFallback>
#define MSG_CASHIER_ADD_ACH_FALLBACK_REPLY		5214 //46[s] reqId err_code [errDescr]

#define MSG_CASHIER_DEL_ACH_FALLBACK			5215 //4<s> reqId <gwName>
#define MSG_CASHIER_DEL_ACH_FALLBACK_REPLY		5216 //46[s] reqId err_code [errDescr]

#define MSG_CASHIER_INIT_REPRESENTMENT			5217 //44b4ss reqId, transId, isSecondRepresentment, (origTransId, origXTransStr, origGwName) // #10026 added three last fields
#define MSG_CASHIER_INIT_REPRESENTMENT_REPLY	5218 //46s4 reqId, err_code, errDescr, newTransId

#define MSG_TIMER_CNB_PURCHASE_TIMEOUT			5219 //44 transId, reqId

#define MSG_TIMER_REPORT_PERFORMANCE			5220 //

//client to cashier
#define MSG_CASHIER_AVLS_REQUEST				5221	//4sss44 - avlsRequestType, useId, phone, time, avSystem, resultId  //avlsRequestType 0 = chat, 1 = phone, 2 = none
#define MSG_CASHIER_AVLS_REQUEST_REPLY			5222    //6[s|4] errCode[errDescr|avlsRequestType]

//admin to cashier
#define MSG_CASHIER_GET_AVLS_SETTINGS			5223 //4 reqId
#define MSG_CASHIER_GET_AVLS_SETTINGS_REPLY		5224 //4b4444b444444 - reqId,isEnabledNow,hourFromNow,minutesFromNow,hourToNow,minutesToNow,isEnabledINI,hoursFromINI,minutesFromINI,hoursToINI,minutesToINI,displayAfter,displayAfterINI

#define MSG_CASHIER_SET_AVLS_SETTINGS			5225 //4b44444 - reqId,enable,hourFromNow,minutesFromNow,displayAfter
#define MSG_CASHIER_SET_AVLS_SETTINGS_REPLY		5226 //46[s] - reqId,errCode[errDescr]

//client to cashier
#define MSG_CASHIER_CITADEL_GET_BANKS			5227 //s currency
#define MSG_CASHIER_CITADEL_GET_BANKS_REPLY		5228 //6[s|4<(CitadelBankInfoStruct)>] errCode [errMsg|numBanks<CitadelBankInfoStruct>]

#define MSG_CASHIER_CITADEL_BANK_SELECTED		5229 //s(CitadelBankInfoStruct) bonusCode CitadelBankInfoStruct
#define MSG_CASHIER_CITADEL_BANK_SELECTED_REPLY	5230 //6s errCode errMsgOrPopupText

// #20342 obsolete
//#define MSG_CASHIER_MONEYORDER_INFO				5231 // empty
//#define MSG_CASHIER_MONEYORDER_INFO_REPLY		5232 // 6s errorCode errDescrOrText

#define MSG_CASHIER_REDEEM_RETURN				5233 //44 reqId redeemId
#define MSG_CASHIER_REDEEM_RETURN_REPLY			5234 //46[s|ss] reqId errCode[errDescr|userId, country]

#define MSG_CASHIER_GCDEBIT_INIT2				5235 	// identical to MSG_CASHIER_GCDEBIT_INIT
#define MSG_CASHIER_GCDEBIT_INIT2_REPLY			5236	//6[s|14] errCode[errDescr|gcDebitStatus gcDebitFlags]

#define MSG_TIMER_CNB_PURCHASEACK_TIMEOUT		5237 //44 transId, reqId

#define MSG_TIMER_WEBMONEY_PURCHASE_TIMEOUT		5239 //44 transId, reqId
#define MSG_TIMER_WEBMONEY_PURCHASE_TIMEOUT2	5240 //44 transId, reqId

#define MSG_ADMIN_CITADEL_GET_INFO				5241 //44ssss4ss4 reqId,intendedAmount,currency,userId,fullName,countryCode,locale,email,bonusCode,licenseId
#define MSG_ADMIN_CITADEL_GET_INFO_REPLY		5242 //46s reqId errCode errMsg
#define MSG_ADMIN_CITADEL_EMAIL					5243 //s44ss44s userId, fxRetAmount, amountConverted, currency userEmail userEmLocale licenseId fxCurrency
#define MSG_ADMIN_CITADEL_EMAIL_REPLY			5244 //46[s] reqId errCode errMsg

#define MSG_TIMER_ENVOY_PURCHASE_TIMEOUT		5245 //44 transId, reqId
#define MSG_TIMER_ENVOY_PURCHASE_TIMEOUT2		5246 //44 transId, reqId

#define MSG_GET_ACH_NON_USD_CURRENCIES			5247 //4 reqId
#define MSG_GET_ACH_NON_USD_CURRENCIES_REPLY	5249 //46[s|4<s>] reqId errCode[errDescr|size<currency>]

#define MSG_CASHIER_GET_PENDING_CASHOUTS		5250 //empty
#define MSG_CASHIER_GET_PENDING_CASHOUTS_REPLY	5251 //6[s|4<M>1] errCode numCashouts <oneCashoutBody> hasBonusesWithCodes
//oneCashoutBody=4s4s4Tssb1s transId paySystemName balanceAmount processorCurrency processorAmount started status balanceCurrency canBeCancelled cardTypeByte maskedReference

#define MSG_CASHIER_CANCEL_PENDING_CASHOUTS		5252 //4<4> numToCancel<transId>
#define MSG_CASHIER_CANCEL_PENDING_CASHOUTS_REPLY 5253 //6[s|4<6[s]>] errCode[errDescr|numCancelled<errCode[errDescr]>]

#define MSG_CASHIER_READ_EXCLUDED_STATES		5254 //4 reqId
#define MSG_CASHIER_READ_EXCLUDED_STATES_REPLY	5255 //46[s] reqId errCode[errDescr]

#define MSG_CASHIER_PRE_CASHOUT_FILTER          5256 //4ss46<s> reqId country state lt_fpp numPaySystem <paySystem>
#define MSG_CASHIER_PRE_CASHOUT_FILTER_REPLY    5257 //46[s|6<s>] reqId errCode [errDescr | numPaySystem <paySystem>]

#define MSG_CASHIER_REREAD_PAYSYS_MSG_TEMPLATE	5257 //4 reqId
#define MSG_CASHIER_REREAD_PAYSYS_MSG_TEMPLATE_REPLY 5258 //46[s|4] reqId errCode[errDescr,numMessages]

#define MSG_CASHIER_ADMIN_WIRE_REQUEST			5259 //4s4sssssssssss444 reqId userId amount currency bonusCode personalInfo address city state country zip email balanceCurrency fullName licenseId emailLocale timezone
#define MSG_CASHIER_ADMIN_WIRE_REQUEST_REPLY	5260 //46[s|sssssssss] reqId err_code [errDescr | paymentReference accountHolder bankName city swift specialId bankAcct country additionalRef]

#define MSG_CASHIER_ENVOY_LBT_GET_BANKS				5261 //sb1 currency forAllCurrencies paySystem
#define MSG_CASHIER_ENVOY_LBT_GET_BANKS_REPLY		5262 //6[s|4<MsgBody(EnvoyBankInfo)>] errCode [errMsg|numBanks<MsgBody(EnvoyBankInfo)>]

#define MSG_CASHIER_ENVOY_LBT_BANK_SELECTED			5263 //s(EnvoyBankInfo) bonusCode
#define MSG_CASHIER_ENVOY_LBT_BANK_SELECTED_REPLY	5264 //6s errCode errMsgOrPopupText

#define MSG_ADMIN_ENVOY_LBT_EMAIL				5265 //4s44ss44s reqId, userId, fxRetAmount, amountConverted, currency userEmail userEmLocale licenseId fxCurrency
#define MSG_ADMIN_ENVOY_LBT_EMAIL_REPLY			5266 //46[s] reqId errCode errMsg

#define MSG_ADMIN_ENVOY_LBT_BANK_SELECTED		5267 //44ssss4ss4 reqId,intendedAmount,currency,userId,fullName,countryCode,emLocale,email,bonusCode,licenseId
#define MSG_ADMIN_ENVOY_LBT_BANK_SELECTED_REPLY	5268 //46s reqId errCode errMsg

#define MSG_CASHIER_GET_UNCLEARED_DEPOSITS2			5269 //
#define MSG_CASHIER_GET_UNCLEARED_DEPOSITS2_REPLY	5270 //6[s|4<B>]44 errCode [errDescr|numUncleared<oneUnclearedBody>] maxUnclearedNum maxUnclearedSum
// oneUnclearedBody: 4s4D4s transId paysystem amount clearDateTime fppToClear balanceCurrency

#define MSG_CASHIER_SET_CONVMARGIN  		5271 // 44ss14s - reqId, actionType, curSrc, curDst, UpdateNotDelete, (UpdateNotDelete ? SIGNED new margin x MARGIN_MULTIPLIER : old margin), comment
#define MSG_CASHIER_SET_CONVMARGIN_REPLY	5272 // 46[s] - reqId, errCode [errStr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_VIPSTORE_PURCHASE_ITEM			5273
#define MSG_VIPSTORE_PURCHASE_ITEM_REPLY	5274

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_VIPSTORE_PREPURCHASE_ITEM		5275
#define MSG_VIPSTORE_PREPURCHASE_ITEM_REPLY	5276

//from client to cashier
#define MSG_CASHIER_HISTORY2			5277 //s44 userId numOfDays Flags
#define MSG_CASHIER_HISTORY2_REPLY		5278 //6[s|4<commMsgBody(Ts44s4ss4s1s4)>1] //err_code, [errDescr | numTransactions<CommMsgBody(DateTime, typeDescr, amount, fee, reference, transId, balanceCurrency, descriptor, converted, currency, cardTypeByte, referenceHint, eCashierHistoryFlags)> hasMore]


// client->cashier: provide me with the rates from/to the given currency to/from all other currencies
#define MSG_CASHIER_CONVRATES_SUBSET		5279	// 4s1 actionType currency currencyIsSourceNotDestination
#define MSG_CASHIER_CONVRATES_SUBSET_REPLY	5280	// 6[s|4<commMsgBody(s88)>] errCode [errMsg | numCurrencies <CommMsgBody(currency, UINT64(rate*CONV_RATE_BASE)), inversedRate>]

/* obsolete after #16321
// #11389: from admin to cashier
#define MSG_CASHIER_UPDATE_BINDETAIL		5281	// 4(binDetail) reqId, BinDetail
#define MSG_CASHIER_UPDATE_BINDETAIL_REPLY	5282    //46[s]
*/
#define MSG_CASHIER_CHIPS_INFO2				5283	//ss4 userId clientVersionInfo infoFlags
#define MSG_CASHIER_CHIPS_INFO2_REPLY		5284	//see below
/* MSG_CASHIER_CHIPS_INFO2_REPLY description:
2[s|M414444114<M>4<M>4<M>s4414488s44<4M>4<M>48M88s
errCode [errDescr | 
	userInfoBody chipsInfoBitMask realMoneyOk vipStatus playChips allocPlayChips fppWithCents bonusInEffect userHasBonuses numBalances <oneBalanceBody> numAllocatedBalances<oneAllocBalanceBody> \
	numBalanceCurrencies<oneCurrBody> defaultCurrencyForCountry totalBalanceInPreferredCurrency totalTBalanceInPreferredCurrency fastDepositPaysystemByte \
	beProtectedFromGamingEur beUnusedLossLimit playChips64 playChipsAlloc64 fulltiltUserId ftCashoutMinLimit specVecSize<eCashierExtraInfoType specBody> userRollsSize<userRollBody> \
	totalAvail4CashoutInPrefCurrency flags3 nacDataBody totalBalanceInUsd totalTChipsInSelectedCurrency tChipsCurrency

userInfoBody:
ssssssssDMsss4<77sT>sM email addr1 addr2 city state country zip fullname birthDate phoneBody firstName lastName fiscalCode numProps<propType propInt propStr propWhen> sex mobilePhoneBody 

phoneBody and mobilePhoneBody:
1sss ePhoneValidation, contryPhoneCode, verifiedPhoneNumber, dialingInfo

oneBalanceBody:
4s4444444444444 balanceFlags(1=isDefault) currency availForSpending unclearedForSpending availForCashout unclearedForCashout owedChips tChips wChips chips markedForCashoutOnly instantBonusesWalletAmount fsbTotalBetsNumber userRollsAllocAmount userRollsSurrenderAmount

oneAllocBalanceBody:
s4 currency amount

oneCurrBody:
s currencyCode

userRollBody: empty

nacDataBody:
	s888 - userNacCurrency, nacTotal, bonusPartOfTheNac, depositPartOfTheNac
*/

#define MSG_CASHIER_SET_DEFAULT_ACCOUNT				5285 //s newDefaultCurrency
#define MSG_CASHIER_SET_DEFAULT_ACCOUNT_REPLY		5286 //6[s] errCode[errDescr]

#define MSG_CASHIER_NEW_CURRENCY_ACCOUNT			5287 //s newCurrencyToAdd
#define MSG_CASHIER_NEW_CURRENCY_ACCOUNT_REPLY		5288 //6[s] errCode[errDescr]

#define MSG_CASHIER_DELETE_CURRENCY_ACCOUNT			5289 //s currencyToDelete
#define MSG_CASHIER_DELETE_CURRENCY_ACCOUNT_REPLY	5290 //6[s] errCode[errDescr]

//#define MSG_CASHIER_CURRENCY_ACCOUNT_TRANSFER		5291 // 4s4s amountSrc currencySrc amountDst currencyDst
//#define MSG_CASHIER_CURRENCY_ACCOUNT_TRANSFER_REPLY	5292 //6[s] errCode[errDescr]

#define MSG_CASHIER_REAL_PRE_TRANSFER				5293 // ss4s userFrom, userTo, amountFrom, currencyFrom
#define MSG_CASHIER_REAL_PRE_TRANSFER_REPLY			5294 //6[s|sss1] err_code, [errDescr], realName, city, country, showCity

// obsolete after PYR-36980
#define MSG_CASHIER_GET_USER_PRIVS					5295 //s userId
#define MSG_CASHIER_GET_USER_PRIVS_REPLY			5296 //48bbb84 privs privs2 modelessCashier forceCashout limitFrenchComCashflow privs3 userExtraCashierOptionsMask(enum eUserExtraCashierOptions)

#define MSG_CASHIER_REAL_TRANSFER					5297 // ss4ss userFrom userTo amountFrom comment currencyFrom
#define MSG_CASHIER_REAL_TRANSFER_REPLY				5298

#define MSG_TIMER_CASH_TRANSFER						5299 // ss444ssb - userFrom, userTo, transFromId, transToId, transferAmount, currencyFrom, comment, awareOfBan

#define MSG_CASHIER_AUTOMATION_SET_USER             5301 //s userId
#define MSG_CASHIER_AUTOMATION_SET_USER_REPLY       5302 //6[s] errCode[errDescr]

#define MSG_CASHIER_CURRENCY_ACCOUNT_MULTI_TRANSFER			5303 // 4<4s4s4> numTransfers<amountSrc currencySrc amountDst currencyDst chipsType>
#define MSG_CASHIER_CURRENCY_ACCOUNT_MULTI_TRANSFER_REPLY	5304 //6[s] errCode[errDescr]

// #11583 Wirecard deposit
// client->cashier: init
#define MSG_CASHIER_INIT_WIRECARD_DEPOSIT			5305	//s4ss userId amount currency bonusCode
#define MSG_CASHIER_INIT_WIRECARD_DEPOSIT_REPLY		5306	//6[s|s] errCode[errDescr | popUpText]
// admin->cashier: approve
#define MSG_CASHIER_COMPLETE_WIRECARD_DEPOSIT		5307	//44ss4ss reqId, WIREORDERS.OrderId, WIREORDERS.paymentRef, wireCardBankReference, amountRet, currencyRet, errorInfo
#define MSG_CASHIER_COMPLETE_WIRECARD_DEPOSIT_REPLY	5308	//46[s]   reqId,errCode[errDescr]

// pyr-11326
#define MSG_TIMER_INSTADEBIT_PURCHASE_TIMEOUT		5309 //44 transId, reqId
#define MSG_TIMER_INSTADEBIT_PURCHASE_TIMEOUT2		5310 //44 transId, reqId

// PYR-12111
#define MSG_CASHIER_CREATE_REDEEM_ACCOUNT			5311 //14[{ss},...] cardType, size, [{name, value}, ...]
#define MSG_CASHIER_CREATE_REDEEM_ACCOUNT_REPLY		5312 //6[s] errCode, [errDescr]

//#PYR-12673
#define MSG_CASHIER_SUGGEST_PAYSYSTEMS				5313 //14 paySystemByte that was just rejected, failed transId
#define MSG_CASHIER_SUGGEST_PAYSYSTEMS_REPLY		5314 //6[s|4<B>] errCode[ errDescr | numPaysystems<onePaysystemBody>]
//onePaysystemBody: 1s paysystemByte paysystemDescription

// pyr-12417 #20343 UMWALLET paysystem removed
//#define MSG_TIMER_UMWALLET_PURCHASE_TIMEOUT	    	5315 //44 transId, reqId
//#define MSG_TIMER_UMWALLET_PURCHASE_TIMEOUT2   	    5316 //44 transId, reqId

// client->cashier pyr-12417
#define MSG_CASHIER_USER_ACCT_PROC_REGISTRATION       5317 // s1 userId, paySystem
#define MSG_CASHIER_USER_ACCT_PROC_REGISTRATION_REPLY 5318 // 6[s] err_code, [errDescr]

// client->vipStore pyr-7801 obsolete after completely switching to PYR-15895
//#define MSG_VIPSTORE_INV_GET_ITEM_STOCK				  5319 // UINT32 store item id
//#define MSG_VIPSTORE_INV_GET_ITEM_STOCK_REPLY		  5320 // InvItemStocks r(parser)

// client-cashier #12953
#define MSG_CASHIER_CHECK_PLI						5321 //1s paysystemByte, accountCurrency
#define MSG_CASHIER_CHECK_PLI_REPLY					5322 //6{s|144444} - errCode, {errDescr|paysystemByte, minDeposit, maxDaily, maxWeekly, maxMonthly, levelId}

#define MSG_CASHIER_INCREASE_PLI					5323 //14 paysystemByte, levelId
#define MSG_CASHIER_INCREASE_PLI_REPLY				5324 //6[s] errCode, [errDescr]

#define MSG_CASHIER_SEND_USER_DOCS					5325 //1 paysystemByte
#define MSG_CASHIER_SEND_USER_DOCS_REPLY			5326 //6{s|pbloc(binary data with PDF form)}

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_VIPSTORE_USER_ORDER_HISTORY2			5327
#define MSG_VIPSTORE_USER_ORDER_HISTORY2_REPLY		5328

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_VIPSTORE_USER_CANCEL_ORDER				5329
#define MSG_VIPSTORE_USER_CANCEL_ORDER_REPLY		5330

#define MSG_TIMER_TSI_REDIRECT_TIMEOUT              5331 // PYR-12655
#define MSG_TIMER_TSI_PURCHASE_TIMEOUT              5332 // PYR-12655

#define MSG_ADMIN_READ_RATES						5333 // 4 reqId
#define MSG_ADMIN_READ_RATES_REPLY					5334 // 46[s] reqId, err_code[, errDescr]
/* obsolete after clent protocol 1.55
#define MSG_CASHIER_PAYSYSTEM_LIMITS				5335 // 1 paysystemByte
#define MSG_CASHIER_PAYSYSTEM_LIMITS_REPLY			5336 // 6[s|4<M> ] errCode [errDescr | numCurrencies <MsgBody{currency, same as M is MSG_CASHIER_PAYSYSTEMS_AVAIL_REPLY}>
*/
#define	MSG_TIMER_FREQUENT_REPORT_PERFORMANCE		5337 // empty

#define MSG_CASHIER_PAYSYSTEMS_LIMITS				5338 // 4{1,...} size,{paysystemByte, ...}
#define MSG_CASHIER_PAYSYSTEMS_LIMITS_REPLY			5339 // 6[s|4{4<M>,...}] errCode [errDescr | vectorSize {numCurrencies <MsgBody{currency, same as M is MSG_CASHIER_PAYSYSTEMS_AVAIL_REPLY}, ...}>

#define MSG_TIMER_PAYPAL_REDIRECT_TIMEOUT           5340 // #14292

// #14492 cashier<->client for the user from the hostile state
#define MSG_CASHIER_CONVERT_ALL_ASSETS      		5341 // client: empty; admin: 4s4 reqId userid, conversionFlags
#define MSG_CASHIER_CONVERT_ALL_ASSETS_REPLY		5342 // [admin:4 reqId] 6[s|B] errCode[errDescr|UserAllAssetsConversion.compose]

#define MSG_CASHIER_GET_RMF_BANKACCOUNT             5343 // empty
#define MSG_CASHIER_GET_RMF_BANKACCOUNT_REPLY       5344 // 6[s|ssbs] errCode[errDescr|bankName, maskedAccountNumber,isValid, bankCountry]

#define MSG_CASHIER_SET_RMF_BANKACCOUNT             5345 // sss bankName, maskedAccountNumber, bankCountry
#define MSG_CASHIER_SET_RMF_BANKACCOUNT_REPLY       5346 // 6[s|sbM(1s)] errCode[errDescr|maskedAccountNumber,isValid,arjelContext(realMoneyOk,accountNumber)]

#define MSG_CASHIER_GET_SPENDING_LIMIT				5347 // empty
#define MSG_CASHIER_GET_SPENDING_LIMIT_REPLY		5348 // 6[s|4444<4>] errCode[errDescr|spendingLimit, availableForSpending, totalBalance, size <limits options>]

#define MSG_CASHIER_SET_SPENDING_LIMIT				5349 // 4 newLimit
#define MSG_CASHIER_SET_SPENDING_LIMIT_REPLY		5350 // 6[s|] errCode[errDescr]

#define MSG_TIMER_DINEROMAIL_PURCHASE_TIMEOUT       5351 //4 transId

// #14432 .Com->.FR migration start
#define MSG_CASHIER_FRMIGRATION_CANCEL				5352 // 4 migrationStep(1,2..)
#define MSG_CASHIER_FRMIGRATION_CANCEL_REPLY		5353 // 6[s] errCode[errDescr]

#define MSG_CASHIER_FRMIGRATION_GET_INFO			5354 // 4s eMigrationType linkedUserId
#define MSG_CASHIER_FRMIGRATION_GET_INFO_REPLY		5355 // 6[s|see below] errCode[errDescr | UserAllAssetsConversion cash, UserAllAssetsConversion tChips, userPoints]
// UserAllAssetsConversion: 4s4<44s48> - total currency size <MsgBody(type typedAmount currency amountDst convRate)>  same as MSG_CASHIER_CONVERT_ALL_ASSETS_REPLY
// userPoints: 444 - fppBalance, monthlyVpp, yearlyVpp

#define MSG_CASHIER_FRMIGRATION						5356 // empty
#define MSG_CASHIER_FRMIGRATION_REPLY				5357 // 6[4,4|s] errCode[transId, migrationId | errDescr]

#define MSG_CASHIER_FRMIGRATION_LIVESUPPORT			5358 // same as MSG_CASHIER_DAD_REQUEST
#define MSG_CASHIER_FRMIGRATION_LIVESUPPORT_REPLY	5359 // 6[s|4] errCode [errDescr|requestType]

#define MSG_CASHIER_FRMIGRATION_DEPOSIT             5360 // doDeposit, transId, migrationId
#define MSG_CASHIER_FRMIGRATION_DEPOSIT_REPLY       5361 // 6[s] errCode[errDescr]
// #14432 .Com->.FR migration end

#define MSG_ADMIN_GET_DAD_SUGGESTED_PAYSYSTEMS          5362 // 4s4ss44M rejectedPaysystem,rec2,country,state,vipstatus,licenseId,depositMaskStruct
#define MSG_ADMIN_GET_DAD_SUGGESTED_PAYSYSTEMS_REPLY    5363 // 46[s reqId,errorMsg |4B totalSuggestions,msgbody] where msgBody - 1s paysystemByte,description

#define MSG_CASHIER_VALIDATION_CANCEL               5364 // 144 psByte, digitsmask, attempts
#define MSG_CASHIER_VALIDATION_CANCEL_REPLY         5365 // 6 errCode=0

#define MSG_TIMER_ROBOKASSA_REDIRECT_TIMEOUT        5366 // #14606
#define MSG_TIMER_ROBOKASSA_CHECK_TIMEOUT           5367 // #14606

#define MSG_CASHIER_NOTIFICATION                    5368 // 6ssb infoCode(0-i,1-w,2-e) infoText, infoSubject, true - to show confirmation checkbox

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_VIPSTORE_USER_CANCEL_ORDERS				5369 // s4<4> - userId, numOrders<orderId>
#define MSG_VIPSTORE_USER_CANCEL_ORDERS_REPLY		5370

// client->vipStore PYR-15895
#define MSG_VIPSTORE_INV_GET_ITEM_STOCK2			5371 // UINT32 store item id
#define MSG_VIPSTORE_INV_GET_ITEM_STOCK2_REPLY		5372 // InvItemStocks2 r(parser)

// #14344 cashier->gcdbm
#define MSG_CASHIER_GET_UNCASHEDCHEQUE_EMAIL        5373 // 4<4,s> numOfDaysInterval, <dayInterval, templateName>  #14344
#define MSG_CASHIER_GET_UNCASHEDCHEQUE_EMAIL_REPLY  5374 // 6[s|4]<B> errCode, [errDescr|numOfEmails], <struct UnCashedChequeEmailReminder>
#define MSG_TIMER_UNCASHEDCHEQUE_SCHEDULE           5375 // #14344 timer msg
/* #16814: obsolete
#define MSG_ADMIN_READ_GEOIPCITY					5376 //4 reqId
#define MSG_ADMIN_READ_GEOIPCITY_REPLY				5377 // 6[s] errcode[,errorDescr]
*/
#define MSG_CASHIER_TCHIPS_PRESALE					5378	// sb44s anotherUserId isBuyer tChips cash currency
#define MSG_CASHIER_TCHIPS_PRESALE_REPLY			5379	// 6[s|ssb44sss1] errCode [errDescr | seller buyer isBuyer tChips cash anotherUserId anotherUserCity anotherUserCountry showCity]

#define MSG_CASHIER_TCHIPS_SALE						5380	// sb44s anotherUserId isBuyer tChips cash currency
#define MSG_CASHIER_TCHIPS_SALE_REPLY				5381	// 6s4 errCode (er=DBM_TSALE_CREATED means sale created, not an error) errDescr expirIntervalInSec

#define MSG_CASHIER_TCHIPS_PRECANCEL				5382	// empty
#define MSG_CASHIER_TCHIPS_PRECANCEL_REPLY			5383	// 6[s|4<MsgBody{4s44bTs}>] errCode[errDescr|size<{saleId anotherUserId tChips cash isStartedByBuyer validUntil currency}>]

#define MSG_CASHIER_TCHIPS_CANCELID					5384	// 4<4> size<saleIdToCancel>
#define MSG_CASHIER_TCHIPS_CANCELID_REPLY			5385	// 6[s|4<MsgBody{46[s]}>] errCode[errDescr|size<{saleId saleErrCode [saleErrDescr]}>]

#define MSG_CASHIER_TCHIPS_SALE_TO_PSTARS			5386	// 44s tChips cash currency
#define MSG_CASHIER_TCHIPS_SALE_TO_PSTARS_REPLY		5387	// 6[s] errCode [errDescr]

#define MSG_TIMER_TSALES_CHECK_EXPIRED				5388

//client to cashier
#define MSG_CASHIER_PAYSYSTEMS_AVAIL_PROMO			5389 //s currency
#define MSG_CASHIER_PAYSYSTEMS_AVAIL_PROMO_REPLY	5390 // 6[s|4<M>b4<M>] errCode[,errDescr|numPaySystems,<onePaySystemBody>,dadPreDepositAvailable,numMessages,<psbyte,str1,str2,str3>]
//onePaySystemBody: 1s4444ss44s4 - paySystemName minDeposit maxCashinMonthly maxCashinWeekly maxCashinDaily feeFormula feeDescription maxDeposits depositsHours userLimitText maxCashin1Trans
//Note: minDeposit, maxCashinMonthly, maxCashinWeekly, maxCashinDaily, maxCashin1Trans are converted to the currency in request, using our rounding algorythm

#define MSG_CASHIER_TSALE_GET_INFO					5391	// empty
#define MSG_CASHIER_TSALE_GET_INFO_REPLY			5392	// 6[s|4b4] errCode [errDescr | minRate_x1M tSaleToPStarsEnabled pStarsRate_x1M]

#define MSG_CASHIER_PSTARSPAY_BANK_SELECTED         5393  //s44s bonusCode bankRefId processorAmt processorCurrency
#define MSG_CASHIER_PSTARSPAY_BANK_SELECTED_REPLY   5394  //6[s|sssbss44ssss] errCode [errDescr | bankName beneficiary beneficiaryAccount useReference bankReference currency amount amountToPay processingTime expiryTimeStr extraInfo url]

#define MSG_CASHIER_VOUCHER_AMOUNTCURRENCY_GET_INFO				5395	// 1 paysystemByte #21516
#define MSG_CASHIER_VOUCHER_AMOUNTCURRENCY_GET_INFO_REPLY		5396	// 6[s|4<MsgBody{4s}]>   errCode [errDescr | count<{amount currency}>

#define MSG_TIMER_PAGOSONLINE_REDIRECT_TIMEOUT      5397 // PYR-17121
#define MSG_TIMER_PAGOSONLINE_PURCHASE_TIMEOUT      5398 // PYR-17121
/* obsolete
#define MSG_ADMIN_GET_USER_AVAIL_PAYSYSTEMS			5399 // s userid	
#define MSG_ADMIN_GET_USER_AVAIL_PAYSYSTEMS_REPLY	5400 // 6[s|4s...] errCode [errInfo | size, vector of paysystemnames]
*/
#define MSG_TIMER_WORLDPAY_REDIRECT_TIMEOUT			5401 // 4 transId

// #18100 cashier to bankval
#define MSG_CASHIER_BANK_DETAIL_VALIDATION          5402 // 44<s> bankval currency paysystem, numfields, <fieldname, fieldvalue>
#define MSG_CASHIER_BANK_DETAIL_VALIDATION_REPLY    5403 // 2ss errorCode, processorErrorMsg, fieldName&Value detail

#define MSG_TIMER_SOFORT_PURCHASE_TIMEOUT           5404 //4 transId

// #18174 - Fast Deposit
#define MSG_CASHIER_GET_FAST_DEPOSIT_INFO			5405 // 44s eFastDepositCallFrom targetAmount targetCurrency
#define MSG_CASHIER_GET_FAST_DEPOSIT_INFO_REPLY		5406 // 6[s|1s44s44] errCode [errMsg | paysystemByte paysystemNameI18N status procAmount procCurrency extraAcctAmount cardInfoId]

#define MSG_CASHIER_UPDATE_FAST_DEPOSIT_INFO		5407 // 144s4 paysystemByte status procAmount procCurrency cardInfoId
#define MSG_CASHIER_UPDATE_FAST_DEPOSIT_INFO_REPLY	5408 // 6[s] errCode [errMsg]

#define MSG_CASHIER_DELETE_FAST_DEPOSIT_INFO		5409 // 1 paysystemByte
#define MSG_CASHIER_DELETE_FAST_DEPOSIT_INFO_REPLY	5410 // 6[s] errCode [errMsg]

#define MSG_ADMIN_PGAD_REPORT_BALANCE				5411 // s44448 - userId, userIntId, balance, bonusBalance, tChips, flags2
#define MSG_ADMIN_PGAD_REPORT_BALANCE_REPLY			5412 // 6[s] errCode[errInfo]

#define MSG_TIMER_DAD_UNFINISHED_TRANSACTIONS_CHECK	5413
// cashier->client notification
#define MSG_CASHIER_DAD_UNFINISHED_TRANS_NOTIFY		5414 // 4s transId msg (identical to MSG_LOBBY_DAD_UNFINISHED_TRANS_NOTIFY)

// #19207
#define MSG_CASHIER_LOOKUP_FISCAL_CODE				5415 // s - fiscalCode
#define MSG_CASHIER_LOOKUP_FISCAL_CODE_REPLY		5416 // 6[s|4<M>[2<M>b]] - errCode[errMsg|numEntries<MsgBody>[numCodes<MsgBody2>],itClientUseSentCountryOfBirthName] MsgBody = ss (province, city) MsgBody2 = ss (country code, country name)

// #19393
#define MSG_CASHIER_GET_RM_NOK_HELD_FUNDS           5417 // empty
#define MSG_CASHIER_GET_RM_NOK_HELD_FUNDS_REPLY     5418 // 6[s|4<M>4s] errCode[errMsg|numTransactions<MsgBody(transId fxAmount fxCurrency started cardTypeByte Paysystem)> totalOnHold denominatedCurrency]
#define MSG_CASHIER_REJECT_RM_NOK_HELD_FUNDS        5419 // empty
#define MSG_CASHIER_REJECT_RM_NOK_HELD_FUNDS_REPLY  5420 // 6[s]

#define MSG_TIMER_QIWIWALLET_REDIRECT_TIMEOUT       5421 // #19820
#define MSG_TIMER_WALLETONE_PURCHASE_TIMEOUT		5422 // #20110
/* obsolete - functionality moved to regulator
#define	MSG_CASHIER_MIGRATION_GET_INFO				5423 // empty - NOT USED for 2011-12
#define	MSG_CASHIER_MIGRATION_GET_INFO_REPLY		5424 // 6[s|] - errCode[errDescr|]

#define	MSG_CASHIER_MIGRATION_PERFORM				5425 // 44 - migrFlag,migrSiteId
#define	MSG_CASHIER_MIGRATION_PERFORM_REPLY			5426 // 6s - errCode,errDescr(unconditionally)
*/
#define MSG_CASHIER_GET_USER_LIMIT_EX				5427 //
#define MSG_CASHIER_GET_USER_LIMIT_EX_REPLY			5428 // 6[s|{4T}4<{44T4444}>s8 - errCode[errDescr|B{quizStatus, quizTime}, size<B{UNT32 limitType, INT32 limit, SrvTime inEffectUntil, INT32 limitMax, INT32 limitMin, INT32 limitThreshold, INT32 regulatoryLimit}> selfLimitCurrency RateToUsd]
														 // quizStatus = 0 – SELFLIMIT_TEST_NA, 1 – SELFLIMIT_TEST_PASSED, 2 – SELFLIMIT_TEST_FAILED (dbm.h)
														 // limitType = 1 – daily, 7 – weekly, 30 – monthly (see dbm.h SelfDepositLimitType)
                                                         // regulatoryLimit = -1 if no regulatory limit

#define MSG_CASHIER_SET_USER_LIMIT_EX				5429 // 4{4msgBody<44>,...} <UNT32 quizResult>, {UINT32 size<UNT32 limitType, INT32 newLimit>,…}, b-groupDepositLimit
#define MSG_CASHIER_SET_USER_LIMIT_EX_REPLY			5430 // 6{s|{4msgBody<6s>,...}} INT16 dummyErrCode, STR dummyErrStr, {UINT32 size<INT32 limitType, INT16 errCode, STR errMsg>,…}

#define MSG_TIMER_PENDING_DEPOSIT_LIMITS_CHECK		5431 // empty
#define MSG_TIMER_WAIT4PENDING_TRANSACTION			5432 // 44 transId counter

#define MSG_CASHIER_PSTARSTOPUP_GET_INFO			5433	// empty
#define MSG_CASHIER_PSTARSTOPUP_GET_INFO_REPLY		5434	// 6[s|4<MsgBody{4s}]>   errCode [errDescr | count{amount currency}>

// from regulator to cashier
#define MSG_CASHIER_APPROVE_RM_NOK_HELD_FUNDS_NOTIFICATION 5435 // s userId

#define MSG_CASHIER_AGEVERIVICATION2				5436	// BBB CommonBody PassportBody DrivLicBody
#define MSG_CASHIER_AGEVERIVICATION2_REPLY			5437	// 6[s[4<B(s)>]] errCode [errDescr [if(err=ERR_AGEVERIFICATION_NEEDS_DATA) size<MsgBody(countryWithDrivLic)>] ]

#define MSG_CASHIER_AGEVERIVICATION2_CANCEL			5438	// empty
#define MSG_CASHIER_AGEVERIVICATION2_CANCEL_REPLY	5439	// 6[s] errCode [errDescr]

#define MSG_TIMER_YANDEXMONEY_REDIRECT_TIMEOUT      5440    // #22220

#define MSG_CASHIER_WEBTOKEN_INVALID				5441	// 4ss reqId, token, userid
#define MSG_CASHIER_WEBTOKEN_INVALID_REPLY			5442	// 46[s] reqId, errCode [errDesc]

// #22434 ES Migration 2
#define MSG_CASHIER_ESMIGRATION2_CANCEL				5443 // 4 migrationStep(1,2.. see eEsMigrationCancelCode)
#define MSG_CASHIER_ESMIGRATION2_CANCEL_REPLY		5444 // 6[s] errCode[errDescr]

#define MSG_CASHIER_ESMIGRATION2_GET_INFO			5445 // empty
#define MSG_CASHIER_ESMIGRATION2_GET_INFO_REPLY		5446 // same as MSG_CASHIER_FRMIGRATION_GET_INFO_REPLY

#define MSG_CASHIER_ESMIGRATION2					5447 // empty
#define MSG_CASHIER_ESMIGRATION2_REPLY				5448 // 6[s|444s] errCode[errDescr|overcrTransId migrationId totalEur voucher]

#define MSG_CASHIER_ESMIGRATION2_DEPOSIT            5449 // b444 doDeposit overcrTransId migrationId totalEur
#define MSG_CASHIER_ESMIGRATION2_DEPOSIT_REPLY      5450 // 6s errCode errDescr
// end of #22434 ES Migration 2

/* obsolete PYR-61156
#define MSG_CASHIER_YANDEX_RECONCILE_REGISTER		5451 // tt start time, end time
#define MSG_CASHIER_YANDEX_RECONCILE_REGISTER_REPLY 5452 // 4{4,4,s,t,4,...} size,{transid,amount,currency,completed,cardnum,...}
*/

#define MSG_WCASHIER_GET_INFO						5453 // empty
#define MSG_WCASHIER_GET_INFO_REPLY					5454 // 6444848444t488	errCode userIntId licenseId privs privs2 flags flags2 locale emLocale timezone registered origUserIntId privs3 flags3

#define MSG_WCASHIER_GET_CHALLENGE_QUESTIONS		5455 // 4 localeId
#define MSG_WCASHIER_GET_CHALLENGE_QUESTIONS_REPLY	5456 // 2[s|4<ss>] errCode [errStr|count<question,answer>]

#define MSG_WCASHIER_SEND_VALIDATION_EMAIL			5457 // s4 validationCode, validity duration in minutes
#define MSG_WCASHIER_SEND_VALIDATION_EMAIL_REPLY	5458 // 2[s] errCode [errStr]

#define MSG_TIMER_GET_HELD_EXCEES_DEPOSITS          5459 // #18264

#define MSG_CASHIER_CHECK_WEBCASHIN					5460 // 44<ss> transid, numpostparam, <paramname, paramvalue>
#define MSG_CASHIER_CHECK_WEBCASHIN_REPLY			5461 // s44488 status{'P'|'R'|'A'},isWebCashier{0|1},licenseId,localeId,trans.flags,trans.flags2

// PYR-45667 obsolete, TODO comment out after FT gateway code is removed
#define MSG_FULLTILT_DEPOSIT						5462 // 4 reqId Ftp2Ps_Transfer
#define MSG_FULLTILT_DEPOSIT_REPLY					5463 // 48b6s4 reqId same_ftObjectId isDuplicate errCode errDescr psCashTransId

#define MSG_TIMER_WEB_CALLBACK_PURCHASE_TIMEOUT     5464 // 44 transId, reqId

#define MSG_TIMER_MOBILEPAYMENT_ORDER_TIMEOUT       5465 // PYR-24787 transId
#define MSG_CASHIER_MOBILEPAYMENT_NOTIFY            5466 // 4s44sss reqid, status, transid, procamount, proccurrency, xtransstr, errcode. PYR-24787
#define MSG_CASHIER_MOBILEPAYMENT_NOTIFY_REPLY      5467 // 4 reqid. PYR-24787

#define MSG_CASHIER_GET_SAVED_CARDS					5468 // empty
#define MSG_CASHIER_GET_SAVED_CARDS_REPLY			5469 // 6[s|4<{4414ss21Ts4B1}>] errCode [errDescr | vector_size < MsgBody{cardInfoId ccId_ignored paySystemByte flagsForClient(enum eCashierSavedCardFlags) plainCardNum maskedCardNum expYear expMonth lastTimeUsedNullable procCurrency(empty means unknown) procAmount blockWithEncryptedCardNum internalPaysystemByte} >

#define MSG_CASHIER_DELETE_SAVED_CARD				5470 // 14 paysystemByte cardInfoId
#define MSG_CASHIER_DELETE_SAVED_CARD_REPLY			5471 // 6[s] errCode [errMsg]
#define MSG_TIMER_CASHTRANS_MAX_TIMESTAMP			5472

#define MSG_WCASHIER_VERIFY_PWD						5473 // s password
#define MSG_WCASHIER_VERIFY_PWD_REPLY				5474 // 6[s] errCode [errMsg]

// Called when <Submit> button is pressed
#define MSG_CASHIER_DEPOSIT_CHECK					5475 // 18s8s4sB paysystemByte, procAmount64, procCurrency, acctAmount64, acctCurrency, eCashierDepositCheckFlags cardNum blockWithEncryptedCardNum
#define MSG_CASHIER_DEPOSIT_CHECK_REPLY				5476 // 6[s | MsgBody{44ss4<MsgBody{8888}>4]  errCode[errDescr | UpsellBody{awardId eAwardTypes headerText awardCurrency(USD or EUR) vecSize<MsgBody{proposedProcAmount64, proposedAcctAmount64, awardAmt64, awardAmt64InAcctCur}> timerValueSecs} eDepositCheckReplyFlag]

#define MSG_CASHIER_PLAY_TRANSFER                   5477 // PYR-26613. PYR-81908: obsoleted
#define MSG_CASHIER_PLAY_TRANSFER_REPLY             5478
// #define MSG_TIMER_PLAY_TRANSFER_STATS_RESET 		5479 // PYR-26613. PYR-81908: obsoleted
#define MSG_TIMER_HOSTILE_BINS_STATS_RESET 		    5480 // PYR-26564

#define MSG_CASHIER_3V_PREREGISTER					5481 // empty
#define MSG_CASHIER_3V_PREREGISTER_REPLY			5482 // 6[s|s] errCode [errDescr | url]

#define MSG_CASHIER_LIFETIME_LIMIT_CONFIRMED		5483 // empty PYR-27262
#define MSG_CASHIER_LIFETIME_LIMIT_CONFIRMED_REPLY	5484 // 2[s] errCode [errDescr]

// from regulator to cashier
#define MSG_CASHIER_APPROVE_LIFETIME_LIMIT_HELD_FUNDS_NOTIFICATION 5485 // PYR-27262 s userId

#define MSG_CASHIER_CASHOUT_BY_ADMIN				5486 // 4s4ssssB reqId userId amount paysystem currency cardNum balanceCur blockWithEncryptedCardNum
#define MSG_CASHIER_CASHOUT_BY_ADMIN_REPLY			5487 // see MSG_CASHIER_CASHOUT_COMPLETE

//from cashier->gcdbm PYR-14982
#define MSG_CASHIER_INVALIDATE_DBTACCOUNT           5488 //4ss reqId, userId, countryCode
#define MSG_CASHIER_INVALIDATE_DBTACCOUNT_REPLY     5489 //42[s] reqId, errCode [errDescr] 

#define MSG_CASHIER_BUY_PM							5490 // 4s8M1MbM converted currency pmAmountMajorUnits userBody psByte cardBody saveCardInfo payWallBody
#define MSG_CASHIER_BUY_PM_COMPLETE					5491 // 6s[4] errCode text [if (err=0 or TRANSACTION_REJECTED) transId]
#define MSG_CASHIER_BUY_PM_CONFIRM					5492 // identical to MSG_CASHIER_BUY_CC_CONFIRM

#define MSG_CASHIER_PAYSYSTEMS_AVAIL_PM				5493 // empty
#define MSG_CASHIER_PAYSYSTEMS_AVAIL_PM_REPLY		5494 // 6[s|4<M>] errCode[errDescr|numPaySystems<onePaySystemBody>]
//onePaySystemBody: identical to onePaySystemBody in MSG_CASHIER_PAYSYSTEMS_AVAIL_REPLY (above)

#define MSG_CASHIER_PM_PURCH_ITEMS_AVAIL			5495 // 4 storeId (enum ClientPlayChipsStore)
#define MSG_CASHIER_PM_PURCH_ITEMS_AVAIL_REPLY		5496 // 6[s|4<M>] errCode[errDescr|numItems<oneItemBody>]
// oneItemBody: 4s8s44 cashPrice currency playMoneyAmt productId_SKU ePlayMoneyPurchaseItemFlags savingRate_x1M
// savingRate_x1M is the saving percentage multiplied by 1 million

#define MSG_CASHIER_BUY_PM_GOOGLE					5497 // ss order signature
#define MSG_CASHIER_BUY_PM_GOOGLE_REPLY				5498 // 6s[4] errCode text [if (err=0) transId]

#define MSG_CASHIER_BUY_PM_APPLE					5499 // s order
#define MSG_CASHIER_BUY_PM_APPLE_REPLY				5500 // 6s[4] errCode text [if (err=0) transId]

// PYR-68305 admin to cashier: 4s* reqId onBehalfUserId {clientFormat}
// PYR-27624 client to cashier
#define MSG_CASHIER_GET_ASTROPAY_BANKINFO           5501 // B[1] isForCashout [cardType]
#define MSG_CASHIER_GET_ASTROPAY_BANKINFO_REPLY     5502 // 6[s|64<T>] errCode [errDescr | index, numberOfRecords, <T>]
                                                         // T = 4sssssssss = bankId,bankCode,bankName,bankCurrency,branchName,nicAlias,bankCountry,nicValue,
                                                         // userCashoutBankAccountNumber,userCashoutBankAccountType

#define MSG_CASHIER_ASTROPAY_CASHIN                 5503 // 4s8s bankId, CPF, signAmount64, bonusCode
#define MSG_CASHIER_ASTROPAY_CASHIN_REDIRECT        5504 // 6[s|s8T] errCode [errDescr | redirectUrl, signAmount64, T]
                                                         // T = 4sssssssss = bankId,bankCode,bankName,bankCurrency,branchName,nicAlias,bankCountry,nicValue,
                                                         // userCashoutBankAccountNumber,userCashoutBankAccountType

// PYR-68305 admin to cashier: 4s* reqId onBehalfUserId {clientFormat}
// PYR-27624 client to cashier
#define MSG_CASHIER_SAVE_ASTROPAY_BANKINFO          5505 // 4sssssssss = bankId,bankCode,bankName,bankCurrency,branchName,nicAlias,bankCountry,nicValue,
                                                         // userCashoutBankAccountNumber,userCashoutBankAccountType
#define MSG_CASHIER_SAVE_ASTROPAY_BANKINFO_REPLY    5506 // 6[s] errCode [ errDescr ]

#define MSG_CASHIER_CONVERT_FUNDS_TO_ONE_ACCT       5507 // s currencyDst
#define MSG_CASHIER_CONVERT_FUNDS_TO_ONE_ACCT_REPLY 5508 // 2[s] errCode [ errDescr ]

#define MSG_ADMIN_CASHIER_APPROVE_ELV_MANDATE       5509 // 4s4sDs reqId userId userIntId signedWhere signedWhen userCountry
#define MSG_ADMIN_CASHIER_APPROVE_ELV_MANDATE_REPLY 5510 // 42[s] reqId errCode [ errDescr ]

#define MSG_CASHIER_GENERATE_ELV_MANDATE_EMAIL      5511 // empty
#define MSG_CASHIER_GENERATE_ELV_MANDATE_EMAIL_REPLY 5512 // 2[s] errCode [ errDescr ]

// call below for PayLucky, SightLine, SightLineNR
#define MSG_CASHIER_GET_PAYLUCKY_INFO               5513 // 1 paysystemByte
#define MSG_CASHIER_GET_PAYLUCKY_INFO_REPLY         5514 // 2[s|4] paysystemBalanceAmount

#define MSG_TIMER_PSIOB_REDIRECT_TIMEOUT            5515 // PYR-28858
#define MSG_TIMER_PSIOB_PURCHASE_TIMEOUT            5516 // PYR-28858

// from cashier to regulator PYR-30521
// PYR-46515 obsolete
// #define CASHIER_TO_REGULATOR_GROUP_DEPOSIT_LIMIT       5517 // 4s4 - reqId,userId,limit
// #define CASHIER_TO_REGULATOR_GROUP_DEPOSIT_LIMIT_REPLY 5518 // 42[s] - reqId,errCode

#define MSG_CASHIER_GET_VANILLA_AMTCURRENCY         5519 // ss pin, balanceCurrency - PYR-31264 
#define MSG_CASHIER_GET_VANILLA_AMTCURRENCY_REPLY   5520 // 6[s|4s4ss] errcode [errdescr | procAmount, procCurrency, balanceAmount, balanceCurrency, pin] - PYR-31264

// PYR-18558
#define MSG_CASHIER_PLAY_MONEY_REFILL               5521 // empty
#define MSG_CASHIER_PLAY_MONEY_REFILL_REPLY         5522 // 6[s|M(48888)4(M(8t4)|M(t4)|M()]
#define MSG_TIMER_PLAY_MONEY_REFILLS_CLEANUP 		5523 //

// PYR-33725
#define MSG_CASHIER_CHIPS_INFO_WITH_OPTIONS				5524	// 4<4> size<eUserPropertyType>
#define MSG_CASHIER_CHIPS_INFO_WITH_OPTIONS_REPLY		5525	// 2[s|4<MsgBody{44st}>484888s84<MsgBody{s88}>8]
// errCode[errDescr | size<MsgBody{UserProperty}> flags, flags2, privs, privs2, privs3, playMoney, preferredCurrency, totalBalanceInPreferredCurrency, numOfAccts <MsgBody{currency, chips64(might be negative), tChips64}>, flags3]

#define MSG_CASHIER_TO_CLIENT_INFO_NOTIFICATION			5526	// 8-playMoney, 4-numOfAccts <MsgBody{s-currency, 8-chips(might be negative), 8-tChips}>, 8-fppInCents, 8-totalBalanceInPrefCurrencyNoTChips, 8-totalBalanceInUSD, 8-totalTChipsInSelectedCurrency, s-tChipsCurrency

// Begin of PYR-31838
#define MSG_CASHIER_USER_LOGGED_IN					5527 // empty
#define MSG_CASHIER_USER_LOGGED_IN_REPLY			5528 // 2[s] errcode [errdescr]

// Renamed for PYR-46074
#define MSG_CASHIER_TO_CLIENT_USERROLLS_GAMETICKETS_DATA_NOTIFICATION 5529 // 4<MsgBody{4444ss4t4444444444ss444}>4<MsgBody{8444444tt444s44sBs}4<MsgBody{444444s44st444444ss44}>>
// sizeUserRolls<MsgBody{userRollId,state(eCashierUserRollStateNotif),initialAmount,currentAmount,currency,rollName_i18n,products(eCashierRollProductFlags),expiryTime,rollFlags,userRollFlags,flags(eCashierRollSettingFlags),pointsToClearInMillicents,betsToClear,status(eUserRollStatus),redemptionLimit,clearStakes,brandsMask,rewardAssetType(eRewardAssetType),shortEligibleGameNames,longEligibleGameNames,verticalMask,cashToPay,fppToPay>
// sizeGameTickets<MsgBody{gameTicketId,state(eCashierGameTicketStateNotif),playsInitial,playsSpent,status,flags,whenStarted,expiration,gameTicketTypeId,ticketTypeFlags,maxNumPlays,gameTicketName,brandsMask,costFpp,imageUrl,suppress,gameTicketTypeName}>
// sizeFreeSpinBonuses<MsgBody{userRollId,state(eCashierUserRollStateNotif),initialAmount,currentAmount,fsbBetAmount,fsbWinningAmount,currency,clearBets,numOfBets,rollName_i18n,expiration,rollFlags,userRollFlags,status(eUserRollStatus),redemptionLimit,brandsMask,rewardAssetType(eRewardAssetType),shortEligibleGameNames,longEligibleGameNames,cashToPay,fppToPay}>
// expiryTime could be empty - no expiration

// Message # 5530 is not used after API change

#define MSG_CASHIER_TO_CLIENT_USERROLL_CLEARED_NOTIFICATION 5531 // 4<MsgBody{444ss4t444b4}>
// size<MsgBody{userRollId,initialAmount,currentAmount,currency,rollName_i18n,products(eCashierRollProductFlags),expiryTime,rollFlags,userRollFlags,amtToPay,redemptionLimitReached,brandsMask}>
// expiryTime could be empty - no expiration

#define MSG_CASHIER_GET_CURRENT_USERROLLS			5532 // empty
#define MSG_CASHIER_GET_CURRENT_USERROLLS_REPLY		5533 // 2[s|4<MsgBody{44s4444s44444t44s4ss444444}>4<MsgBody{44s4444s44t44s4ss4444}>]
// errCode [errMsg|
// size<MsgBody{userRollId,status(eUserRollStatus),rollName_i18n,paidFpp,paidCash,initialAmount,currentAmount,currency,surrenderValue,collectedPointsInCents,clearPointsInCents,numOfBets,clearBets,expiryTime,rollFlags,userRollFlags,eligibleGames(json string),redemptionLimit,shortEligibleGameNames,longEligibleGameNames,stakes,clearStakes,brandsMask,rewardAssetType(eRewardAssetType),rewardAssetId,verticalMask}>
// size<MsgBody{userRollId,status(eUserRollStatus),rollName_i18n,initialAmount,currentAmount,fsbBetAmount,fsbWinningAmount,currency,numOfBets,clearBets,expiration,rollFlags,userRollFlags,eligibleGames(json string),redemptionLimit,shortEligibleGameNames,longEligibleGameNames,brandsMask,rewardAssetType(eRewardAssetType),rewardAssetId,verticalMask}>
// ]
// expiryTime could be empty - no expiration

#define MSG_CASHIER_SET_USERROLL_STATUS				5534 // 444 userRollId,currentStatus(eUserRollStatus),action(eUserRollAction)
#define MSG_CASHIER_SET_USERROLL_STATUS_REPLY		5535 // 2[s] errcode [errdescr]
// End of PYR-31838

// PYR-35203
#define MSG_CASHIER_UPDATE_USERROLLS_ORDER			5540 // 4<M(4)> - size<M(userRollId)>
#define MSG_CASHIER_UPDATE_USERROLLS_ORDER_REPLY	5541 // 2[s] errcode [errdescr]

// PYR-36700
#define MSG_TIMER_ADYENHOSTED_REDIRECT_TIMEOUT      5542 // 4 transId

// PYR-28733 - deprecated, should be removed after 08.2015 build
#define MSG_CASHIER_SET_USER_PRIVILEGES             5544  //488 newPrivs, newPrivs2, newPrivs3
#define MSG_CASHIER_SET_USER_PRIVILEGES_REPLY       5545  //2[s] - errCode[errDescr] 

// PYR-28733
#define MSG_CASHIER_SET_USER_ALL_PRIVILEGES             5546  //448888 - privsMaskToSet privsMaskToClear privs2MaskToSet privs2MaskToClear privs3MaskToSet privs3MaskToClear
#define MSG_CASHIER_SET_USER_ALL_PRIVILEGES_REPLY       5547  //2[s] - errCode[errDescr] 

// PYR-36193
#define MSG_CASHIER_GET_TEXT                        5548  // 4 - bitset(eCashierGetTextFlags)
#define MSG_CASHIER_GET_TEXT_REPLY                  5549  // 2[s|4<4bM>] errCode[errDescr|size<eCashierGetTextFlag, processedOk, M>].
                                                          // 'M' depends on 'eCashierGetTextFlag' value, i.e. for RealMoneyPopup it is 'sss'
                                                          // 'M' could be empty if processedOk is 'false'

// from cashier to regulator PYR-38214
// PYR-46515 obsolete
//#define CASHIER_TO_REGULATOR_GROUP_DEPOSIT_LIMIT_EX       5550 // 4s444 - reqId,userId,newDailyLimit,newWeeklyLimit,newMonthlyLimit
//#define CASHIER_TO_REGULATOR_GROUP_DEPOSIT_LIMIT_EX_REPLY 5551 // 42[s] - reqId,errCode

// PYR-23517 timer msgId
#define MSG_TIMER_REPEAT_3DSVERIFY_REQUEST          5552  // 4B transId, wasNotSend? 

// client to cashier PYR-36980
#define MSG_CASHIER_CHECK_PAYSYSTEM2				5554 // 1s4M[|s4] paySystemByte acctCurrency cardInfoId, M - (fastdepositcurrency, fastdepositamount) or empty
#define MSG_CASHIER_CHECK_PAYSYSTEM2_REPLY		    5555 // 2[s|sbs4<ss44<s>>4<ss4>b4<4s>44<B>b444bbBbB4BB84<B(s)>] - errCode[errMsg | paySystem isAvailable message numSubSystems<productId, description, logoImageId, currenciesSize<currency>> numLinks<linkDescr linkUrl linkType> ageVerifyNeeded idsSize<id_type, id_descr> dayToClear numCurrencies<oneRateBody> userHasBonusesWithCodes validationFailuresThreshold selfLimitAmount remainingDepositLimitAmount isFdSupportedByThisPs isItPs4AuthUser selfLimitsBody paysystemFeeApplies<feeBody> cardNumLength <SuggestedDeposits> <paysystemSpecificBody> suggestedDepositFlags size<B(prepopulatedCode)>]
// oneRateBody - s88 - currency conversionRate conversionRateInverted
// selfLimitsBody - 444 - maxCashinDaily maxCashinWeekly maxCashinMonthly

// client to cashier PYR-36980
#define MSG_CASHIER_GET_USER_PRIVS_2    			5556 // s userId
#define MSG_CASHIER_GET_USER_PRIVS_2_REPLY			5557 // 2[s|48bbb84] errCode[errMsg | privs privs2 modelessCashier forceCashout limitFrenchComCashflow privs3 userExtraCashierOptionsMask(enum eUserExtraCashierOptions)]

#define MSG_TIMER_SOFORT_REDIRECT_TIMEOUT           5558 // PYR-39358

// PYR-108884
#define MSG_CASHIER_GET_TEMP_ACCOUNT_DEPOSIT_STATUS 5559 // 
#define MSG_CASHIER_GET_TEMP_ACCOUNT_DEPOSIT_STATUS_REPLY 5560
//#define MSG_CASHIER_PAYSYSTEM_SIGNUP                5559 // 1 paysystemByte
//#define MSG_CASHIER_PAYSYSTEM_SIGNUP_REPLY          5560 // 2[s] errCode errDescr

#define MSG_CASHIER_SIGHTLINE_SIGNUP                5561 // s geoIp
#define MSG_CASHIER_SIGHTLINE_SIGNUP_REPLY          5562 // 2[s] errCode [errDescr]

// PYR-38926
#define MSG_CASHIER_TO_RAM_NOTIF_USERROLLS_DATA_NOTIFICATION 5563 // 4sM - userIntId,userId,msgBody with data from MSG_CASHIER_TO_CLIENT_USERROLLS_GAMETICKETS_DATA_NOTIFICATION

// PYR-38926
#define MSG_CASHIER_TO_RAM_NOTIF_USERROLL_CLEARED_NOTIFICATION 5564 // 4sM - userIntId,userId,msgBody with data from MSG_CASHIER_TO_CLIENT_USERROLL_CLEARED_NOTIFICATION

// PYR-38926
#define MSG_WCASHIER_USER_LOGGED_IN					5565 // no data
#define MSG_WCASHIER_USER_LOGGED_IN_REPLY			5566 // 2[s] - errCode[errMsg]

#define MSG_CASHIER_VALINNARELOAD_SIGNUP                5567 // 
#define MSG_CASHIER_VALINNARELOAD_SIGNUP_REPLY          5568 // 2[s] errCode [errDescr]

#define MSG_CASHIER_VALINNARELOAD_GET_BARCODE           5569 // 
#define MSG_CASHIER_VALINNARELOAD_GET_BARCODE_REPLY     5570 // 2[s] errCode [errDescr]

// PYR-46074
#define MSG_CASHIER_SET_GAMETICKET_STATUS				5571 // 844 gameTicketId,currentStatus(eGameTicketStatus),action(eGameTicketAction)
#define MSG_CASHIER_SET_GAMETICKET_STATUS_REPLY			5572 // 2[s] errcode [errdescr]

// PYR-47011
#define MSG_CASHIER_PLAY_MONEY_REFILL_GET_INFO          5573 // empty
#define MSG_CASHIER_PLAY_MONEY_REFILL_GET_INFO_REPLY    5574 // 6[s|4888tM{|t8}]  playChipsRefillPeriodSec,playChipsAllowedForPeriodCents,targetRefillAmountCents,availableForRefill,nextRefillTime,M{nextAdEventTime,adWatchRewardAmountCents | empty if not eligible }

// PYR-45097
#define MSG_CASHIER_PM_WATCH2EARN_START					5575 // 48 triggerType(enum ePlayMoneyAdTriggerType),triggerId
#define MSG_CASHIER_PM_WATCH2EARN_START_REPLY			5576 // 6[s|4t] errCode[errDescr|eventId,nextAdEventTime]

// PYR-45097
#define MSG_CASHIER_PM_WATCH2EARN_FINISH				5577 // 44ss eventId,status(enum ePlayMoneyAdWatchResult),xTransStr,infoStr
#define MSG_CASHIER_PM_WATCH2EARN_FINISH_REPLY			5578 // 6[s] errCode[errDescr]

// PYR-44657
#define MSG_CASHIER_TURN_ON_OFF_GAMEIN_AUTOCONVERSION		5579 // b setNotRemove
#define MSG_CASHIER_TURN_ON_OFF_GAMEIN_AUTOCONVERSION_REPLY	5580 // 2[s] errCode[errDescr]

// PYR-44657
#define MSG_CASHIER_CONVERT_FOR_TARGET_AMOUNT			5581 // 4s totalAmountNeeded currency
#define MSG_CASHIER_CONVERT_FOR_TARGET_AMOUNT_REPLY		5582 // 2[s|4] errCode[errDescr|amountAfter]

// PYR-47572  admin->Cashier
#define MSG_CASHIER_VERIFY_CARD                         5583 // 4s44s4 - reqId, userId, userIntId, CC_ID, paysystem, refId // both userId/userIntId must be set
#define MSG_CASHIER_VERIFY_CARD_REPLY                   5584 // 46[s] - reqId, errCode[errDesc]

// PYR-45097
#define MSG_TIMER_PLAY_MONEY_AD_EVENTS_CLEANUP 			5585 //

// PYR-48809  admin->Cashier
#define MSG_CASHIER_UNVERIFY_CARD                       5586 // 4s44s4 - reqId, userId, userIntId, CC_ID, paysystem, refId // both userId/userIntId must be set
#define MSG_CASHIER_UNVERIFY_CARD_REPLY                 5587 // 46[s] - reqId, errCode[errDesc]

// PYR-44657
#define MSG_CASHIER_GET_DENOMINATED_BALANCE             5588 // s4 - targetCurrencyTC convAction (=eConvActionAutoInterAcctTransfer for PYR-44657)
#define MSG_CASHIER_GET_DENOMINATED_BALANCE_REPLY       5589 // 6[s4<M{s88888}>] errCode[errDesc | targetCurrencyTC, acctVecSize< acctBody{acctCurrency convRate*100M chips chipsInTC tChips tChipsInTC} >
// vector is ordered: TC, default, chips descending

// PYR-47521 admin->Cashier
#define MSG_CASHIER_FULL_FR_CASHOUT_BY_ADMIN            5590 // 4s44 reqId, userId, userIntId, enum eFullFrCashoutFlags
#define MSG_CASHIER_FULL_FR_CASHOUT_BY_ADMIN_REPLY      5591 // see MSG_CASHIER_CASHOUT_COMPLETE

// PYR-37328 client->cashier
#define MSG_CASHIER_STARSWALLET_REGISTER				5592 // s - mobile
#define MSG_CASHIER_STARSWALLET_REGISTER_REPLY			5593 // 6[s|] errCode [errDescr|]

#define MSG_TIMER_PRE_PURCHASE_RMF_REQUEST_TIMEOUT      5594 // PYR-33629

// PYR-52377
#define MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST			5595
#define MSG_VIPSTORE_USER_ORDER_HISTORY_DIGEST_REPLY	5596 // 6[s|4<44>] errCode [errDescr|numRecs<itemId,numPurchased>]

// PYR-37328 Client->cashier
#define MSG_CASHIER_STARSWALLET_CHANGE_MOBILE			5597 // s - mobile
#define MSG_CASHIER_STARSWALLET_CHANGE_MOBILE_REPLY		5598 // 6[s|] errCode [errDescr|]

// PYR-37328 Client->cashier
#define MSG_CASHIER_STARSWALLET_RESEND_OTP				5599 //
#define MSG_CASHIER_STARSWALLET_RESEND_OTP_REPLY		5600 // 6[s|] errCode [errDescr|]

// PYR-37328 Client->cashier
#define MSG_CASHIER_STARSWALLET_OTP_RECEIVED			5601 //
#define MSG_CASHIER_STARSWALLET_OTP_RECEIVED_REPLY		5602 // 6[s|] errCode [errDescr|]

// PYR-30309 WMT -> cashier
#define MSG_CASHIER_GET_IND_USER_TAX_INFO				5603 // 4 cashoutAmount (might be -1, then only coefs matter in reply)
#define MSG_CASHIER_GET_IND_USER_TAX_INFO_REPLY			5604 // 6[s|444444] errCode[errDescr|taxAmount (or -1 if cashoutAmount=-1) totalBalance unUsedBalance dailyTaxableCashout indiaTaxFreeCashout indiaTaxRate_x1M]

#define MSG_TIMER_CASHFREE_HOSTED_REDIRECT_TIMEOUT		5605 // PYR-55411 4 transId

// PYR-45337 client/WMT -> cashier
#define MSG_CASHIER_CC_BIN_CHECK						5606 // s BIN(6 digits)
#define MSG_CASHIER_CC_BIN_CHECK_REPLY					5607 // 6[s|b1] errCode [errDescr|isHostile, paysystemByte]

// PYR-52613
#define MSG_CASHIER_BONUS_CHECK							5608 // 1s paysystemByte bonusCode
#define MSG_CASHIER_BONUS_CHECK_REPLY					5609 // 6[s] errCode [errDescr]

// PYR-57078 admin -> cashier
#define MSG_CASHIER_STARSWALLET_CHECK_STATUS			5610 // 4s - reqId, userId
#define MSG_CASHIER_STARSWALLET_CHECK_STATUS_REPLY		5611 // 46[s|] reqId, errCode [errDescr|]

#define MSG_TIMER_INDIA_ANNUAL_TAX_EVENT                5612 // PYR-54304

// PYR-62719 client -> cashier
#define MSG_CASHIER_GET_RECENT_SUCCESSFUL_DEPOSITS			5613 // empty
#define MSG_CASHIER_GET_RECENT_SUCCESSFUL_DEPOSITS_REPLY	5614 // 6[s|4<M{14sTT4s4s}>] errCode [errDescr|size<M{paysystemByte transId maskedCard started completed acctAmount acctCurrency procAmount procCurrency}>]

#define MSG_TIMER_PAY_INDIA_USER_ANNUAL_TAX             5615 // PYR-54304 44 failedCounter, successCounter 

// PYR-62177
#define MSG_CASHIER_GET_RESCUE_PAYSYSTEMS               5616 // empty
#define MSG_CASHIER_GET_RESCUE_PAYSYSTEMS_REPLY         5617 // 6[s|4<B>] errCode[ errDescr | numPaysystems<onePaysystemBody>]
//onePaysystemBody: 1 paysystemByte

// PYR-59305 client -> cashier
#define MSG_CASHIER_GET_NET_REPORT_BRIEF				5618 // 
#define MSG_CASHIER_GET_NET_REPORT_BRIEF_REPLY			5619 // 6[s|884<M{4M{s84}4<M{s84}>}>] errCode [errDescr| sinceTimeUtc, effectiveTimeUtc, numCategories < { categoryId, {currencyAggregate, amountAggregate, countAggregate}, numCurrencies < {currency, amountPerCurrency, countPerCurrency} > } >  // category items are present even if all numbers are zero; perCurrency items are omitted if all values are zero; times are 'time_t'; only UK supported, others will get NOT_ALLOWED

// timer msgId - hook recurring scheduled events on it
#define MSG_TIMER_EVERY_SECOND							5620 // 

#define MSG_NOTIFICATION_TO_CLIENT						5621 // 4M - notificationCode (enum eClientNotificationCodeEx), specificMsgBody

// PYR-61756 client -> cashier
#define MSG_CASHIER_GET_USER_COUNTRY_PHONE_PREFIX		5622 //
#define MSG_CASHIER_GET_USER_COUNTRY_PHONE_PREFIX_REPLY	5623 // 6[s|s] errCode [errDescr|countryPrefix], 'countryPrefix' is based on User.Country

// PYR-52150 client -> cashier
#define MSG_CASHIER_GET_AG_OPERATOR_BALANCE				5624 // s - balanceCurrency
#define MSG_CASHIER_GET_AG_OPERATOR_BALANCE_REPLY		5625 // 6[s|s8s8] errCode[errDescr | operatorCurrency operatorAmount balanceCurrency balanceAmount]

// PYR-57431 client/WMT -> cashier
#define MSG_CASHIER_GET_TRANSINFO						5626 // 4 - transId
#define MSG_CASHIER_GET_TRANSINFO_REPLY					5627 // 6[s|4s1s4s4ss] errCode[errDescr|transId, status, paysystemByte, gateway, procAmount, procCurrency, balanceAmount, balanceCurrency, paysystemNameI18N]

// PYR-70835
#define  MSG_CASHIER_SAVE_PAYSYSTEM_CONSENT				5628 // 14 paySystemByte,userPaySystemConsent
#define  MSG_CASHIER_SAVE_PAYSYSTEM_CONSENT_REPLY		5629 // 2[s|] errCode [errDescr]

// PYR-73312 client -> cashier
#define  MSG_CASHIER_GET_VANILLADIRECT_BARCODE			5630 //
#define  MSG_CASHIER_GET_VANILLADIRECT_BARCODE_REPLY	5631 // 2[s|s] errCode [errDescr|barcode]

// PYR-69310
#define MSG_CASHIER_CALC_ROMANIA_CASHOUT_TAX			5632 // 8s cashoutAmount balanceCurrency
#define MSG_CASHIER_CALC_ROMANIA_CASHOUT_TAX_REPLY		5633 // 6[s|88] errCode[errDescr|taxAmountInBalanceCurrency cashoutRemainder]

// PYR-63165 regulator->cashier
#define MSG_CASHIER_STOP_BE_DEPOSITS					5634	// 4b - reqId, rejectDepositsBeNg - reject deposits from Belgium NG users if true, accept deposits if false
#define MSG_CASHIER_STOP_BE_DEPOSITS_REPLY				5635	// 46 - reqId, errCode

// PYR-51865 client -> cashier
#define MSG_CASHIER_STARSWALLET_RESEND_MTAN				5636 // 
#define MSG_CASHIER_STARSWALLET_RESEND_MTAN_REPLY		5637 // 6[s|] errCode [errDescr|]

// PYR-51865 client -> cashier
#define MSG_CASHIER_STARSWALLET_CHECK_MTAN				5638 // s - mTAN
#define MSG_CASHIER_STARSWALLET_CHECK_MTAN_REPLY		5639 // 6[s|] errCode [errDescr|]

// PYR-36843 client -> cashier
#define MSG_CASHIER_APPLEPAY_MERCHANT_VALIDATION		5640 // ss - initiative, validationURL
#define MSG_CASHIER_APPLEPAY_MERCHANT_VALIDATION_REPLY	5641 // 6[s|s] - errCode, [errMsg|merchantSession]

// PYR-89627
#define MSG_CASHIER_GET_TEMP_ACCOUNT_CASHOUT_LIMIT_STATUS		5642 
#define MSG_CASHIER_GET_TEMP_ACCOUNT_CASHOUT_LIMIT_STATUS_REPLY	5643 

// PYR-80351 cashier -> client
#define MSG_CASHIER_CASHIN_DATA_NEEDED					5644 // CashinSpecificOutput
// PYR-80351 client -> cashier 
#define MSG_CASHIER_CASHIN_DATA_PROVIDED				5645 // PaymentSpecificData

// PYR-80351
#define MSG_TIMER_FDMS_3DFORM_TIMEOUT					5646 //4 transId

// PYR-80351
#define MSG_CASHIER_FDMS_NOTIFY_3DFORM_COMPLETED		5647 // 4 - transId
#define MSG_CASHIER_FDMS_NOTIFY_3DFORM_COMPLETED_REPLY	5648 // 6[s|] errCode [errDescr|]

// PYR-114611 regulator -> cashier
#define MSG_CASHIER_CALCULATE_LIC_LIFETIME_DEPOSITS_TOTAL		5649 // 4oo - reqId, UserChipsInfo, ClientLoginExtras2
#define MSG_CASHIER_CALCULATE_LIC_LIFETIME_DEPOSITS_TOTAl_REPLY	5650 // 42[s|4s] - reqId, errCode[errDesc|lifetimeDeposits, currency]

// PYR-116520 client -> cashier
#define MSG_CASHIER_GET_PLAID_DATA						5651 // 14s - paysystemByte, amount, currency
#define MSG_CASHIER_GET_PLAID_DATA_REPLY				5652 // 2[s|sssb] - errCode, [errMsg|plaidEnvironment, plaidPublicKey, bankLookupPreference, isPlaidAccountInSystem]

// PYR-116520 client -> cashier
#define MSG_CASHIER_ADD_PLAID_ACCOUNT					5653 // ss - plaidPublicToken, plaidBankAccountId
#define MSG_CASHIER_ADD_PLAID_ACCOUNT_REPLY				5654 // 2[s] - errCode, [errMsg]


///// END OF MESSAGES ////////////////////////////////////////////////////////

// PYR-91990 indicator for MSG_CASHIER... message number range end
#define MSG_CASHIER_RANGE_END 5999

//time to repeat pending request to gateway
#define repeatInterval						300 //seconds

#define USER_BONUS_REQUEST_FLAG_BONUS		1
#define USER_BONUS_REQUEST_FLAG_PROMOS		2

#endif //cashier_h_included
