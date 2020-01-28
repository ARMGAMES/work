#ifndef SwissGW_GIMO_Errors_H_included
#define SwissGW_GIMO_Errors_H_included

// based on https://confluence.pyrsoftware.ca/confluence/pages/viewpage.action?spaceKey=JP&title=CH%3A+Error+Codes+Mapping+for+API
// Series "0xx" - error codes that start with 1 and do not exceed 99 - Error description in English and only for logging and investigation purposes.
// Series "1xx" - error codes that start with 100 -                    Error Description is provided to be displayed as is to the Player.


// Regulator and Cashier will NOT use these, only used when GW sends an error reply to GIMO (for an Incoming message received from PsGimoIncomingAPI) (these are the codes LynnZ sends to Gimo)
enum SwissGW_ErrToPsGimoIncomingAPI : INT16
{
	eToGimo_Success          =   0
,   eToGimoDebug_RangeBegin  =   1
,   eToGimoDebug_RangeEnd    =  99
,   eToGimoGUI_RangeBegin    = 100
,   eToGimoGUI_RangeEnd      = 199

// Replies to PsGimoIncomingApi  From core servers to GW  (DEBUG range)
,   eToGimo_InvalidInput                  =   1  // business data (usually a numeric enum) received from PsGimoIncomingApi had an invalid value
,   eToGimo_InconsistentData              =   2
,   eToGimo_AccCurrencyMismatch           =   3
,   eToGimo_DBMInternalError              =   4
,   eToGimo_CurrencyNotSupported          =   5
,   eToGimo_Db2DuplicateKey               =   6  // transfer in / out  (transactionId not found)
,   eToGimo_Db2SqlError                   =   7
,   eToGimo_WrongBonusCode                =   8
,   eToGimo_NotAllowed                    =   9
,   eToGimo_DbmBarredReturn               =  10
,   eToGimo_NoSuchRecord                  =  11
,   eToGimo_UnexpectedCount               =  12   // user display name is invalid (only occurs during create account)
,   eToGimo_ReservedUserId                =  13
,   eToGimo_NoSuchUser                    =  14
,   eToGimo_TooEarlytoCashout             =  15
,	eToGimo_Internal                      =  16   // GIMO + SST to investigate and fix their software,  one specific message from LynnZ to be shown to player
,   eToGimo_AcctLicenseMismatch2          =  17	  // PYR-95990 after discussion with LynnZ 2019-06-03
,   eToGimo_DuplicateNewUserEmail         =  18	  // PYR-95990 after discussion with LynnZ 2019-06-03
,   eToGimo_DuplicateNewUserMobile        =  23	  // PYR-95990 after discussion with LynnZ 2019-06-03
,   eToGimo_TooManyFailures               =  19	  // PYR-95990 after discussion with LynnZ 2019-06-03
,   eToGimo_TooManyRequests               =  20	  // PYR-95990 after discussion with LynnZ 2019-06-03
,   eToGimo_InternalLogicalError          =  21	  // PYR-95990 after discussion with LynnZ 2019-06-03
,   eToGimo_ServiceNotAvailable           =  22	  // PYR-95990 after discussion with LynnZ 2019-06-03

// Replies to PsGimoIncomingApi   From core servers to GW  (Localized range)
,   eToGimo_InvalidTinfoManualPromo       = 104
,   eToGimo_NoAlternateUserName           = 107
,   eToGimo_UserAlreadyExist              = 110
,   eToGimo_AlreadyBanned                 = 111
,   eToGimo_AccountClosed                 = 112
,   eToGimo_TransactionPending            = 113
,   eToGimo_UserSelfExcluded              = 114
,   eToGimo_UserBannedByRegulator         = 115
,   eToGimo_UserHasUnopenchests           = 116
,   eToGimo_NotAllowedUseActive           = 117
,   eToGimo_UserOwesChips                 = 118
,   eToGimo_UserHasPendingCashouts        = 119
,   eToGimo_UserHasAssets                 = 120
,   eToGimo_UserAlreadyMigrated           = 121
,   eToGimo_NotEnoughChips                = 126	 // PYR-105137
};

// Regulator and Cashier will NOT use these, only used when GW sends an outgoing HTTP message to GIMO and GIMO sends a reply (these are the codes LynnZ received from Gimo)
// 2019-06-19 from DimaP
// https://starsweb.sharepoint.com/sites/GRP_CentralProjects_SwitzerlandProject/Shared%20Documents/Forms/AllItems.aspx?newTargetListUrl=%2Fsites%2FGRP_CentralProjects_SwitzerlandProject%2FShared%20Documents&viewpath=%2Fsites%2FGRP_CentralProjects_SwitzerlandProject%2FShared%20Documents%2FForms%2FAllItems%2Easpx&id=%2Fsites%2FGRP_CentralProjects_SwitzerlandProject%2FShared%20Documents%2FTSG-GIMO%2FCH%20API%20-%20Returned%20Error%20Codes%2BDescription%2Etxt&parent=%2Fsites%2FGRP_CentralProjects_SwitzerlandProject%2FShared%20Documents%2FTSG-GIMO
// https://starsweb.sharepoint.com/sites/GRP_CentralProjects_SwitzerlandProject/Shared%20Documents/TSG-GIMO/CH%20API%20-%20Returned%20Error%20Codes+Description.txt?csf=1&e=oQUgzo&cid=2ff98098-c24f-43f0-9e13-f4fc2fbbf3b5
enum SwissGW_ErrFromGimoHttp : INT16
{
	eFromGimoHttp_Success              =   0
,	eFromGimoHttpToGwDebug_RangeBegin  =   1
,	eFromGimoHttpToGwDebug_RangeEnd    =  99
,	eFromGimoHttpToGwClient_RangeBegin = 100
,	eFromGimoHttpToGwClient_RangeEnd   = 199

,   eFromGimoHttpToGwDebug_InvalidRequestMethod = 1   //  Invalid request method.
,   eFromGimoHttpToGwDebug_InvalidChecksum      = 2   //  Invalid checksum value.
,   eFromGimoHttpToGwDebug_InvalidAuthCode      = 3   //  The authorization code provided is either invalid or missing  (emulator ERROR_CODE_GM_CHALLENGE_TOKEN_UNKNOWN)
,   eFromGimoHttpToGwDebug_InvalidSessionToken  = 4   //  The session token provided is either invalid or missing
,   eFromGimoHttpToGwDebug_InvalidRefreshToken  = 5   //  The refresh token provided is either invalid or missing
,   eFromGimoHttpToGwDebug_InvalidLinkType      = 6   //  The parameter gmLinkType is either invalid or missing
,   eFromGimoHttpToGwDebug_MalformedJson        = 7   //  Syntax error, malformed JSON (emulator ERROR_CODE_GM_MALFORMED_REQUEST)
,   eFromGimoHttpToGwDebug_EmulInvalidCurrency  =40   //  (emulator ERROR_CODE_GM_INVALID_CURRENCY)
,   eFromGimoHttpToGwDebug_EmulTechnicalError   =99   //  (emulator ERROR_CODE_GM_TECHNICAL_ERROR)
};



#endif // SwissGW_GIMO_Errors_H_included
