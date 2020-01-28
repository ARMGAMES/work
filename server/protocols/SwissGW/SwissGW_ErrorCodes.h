#ifndef SwissGW_ErrorCodes_H_included
#define SwissGW_ErrorCodes_H_included


// These are the errors GW uses 
// The range 30000-30999 is used by Regulator and Cashier
// when an incomming message is sent from GW to a core server the reply comes back and is translated into one of these
// when an outgoing  message is sent from a core server to GW the GW will reply with one of these
// GIMO does not care about these
enum SwissGW_ErrorCode : INT16
{
	eSwissErr_Success                       =     0
 ,	eSwissErr_FromGWToPsDebug_RangeBegin    = 30000	// these are sent to Regulator,Cashier when GW fails an outgoing message
 ,	eSwissErr_FromGWToPsDebug_RangeEnd      = 30999
 ,	eSwissErr_FromPsToGwDebug_RangeBegin    = 31000	// these will be sent to GIMO as errors <100
 ,	eSwissErr_FromPsToGwDebug_RangeEnd      = 31499
 ,	eSwissErr_FromPsTranslated_RangeBegin   = 31500 // these will be sent to GIMO as errors >100  see: isSwissGW_ErrorCode_Localized()
 ,	eSwissErr_FromPsTranslated_RangeEnd     = 31999


// Replies to "outgoing messages from core servers to GW", (SwissGW tries to send those msgs to GIMO but fails)
,   eSwissErr_InvalidInput                  = 30001 // invalid value was found in the outgoing msg from PS to GW (so GW cannot send anything to GIMO)
,	eSwissErr_NoUser                        = 30002 // the user was not found (regulator returned userIntId zero)
,	eSwissErr_UserNotLoggedIn               = 30003 // the user session is needed and the user is not logged in
,   eSwissErr_HttpFailed                    = 30004 // GW sent an HTTP msg to GIMO, GIMO replied with a HTTP error reply like 500, so GW informs our core servers that something went wrong with the outgoing msg
,   eSwissErr_WrongChecksum                 = 30005 // GW sent an HTTP msg to GIMO, GIMO replied and the checksum in the reply HTTP header did not match, so GW informs our core servers that something went wrong with the outgoing msg
,	eSwissErr_InvalidChallengeToken         = 30006 // GIMO rejected the VerifyToken
,	eSwissErr_Internal                      = 30007 // some faulty logic in the gateway
,   eSwissErr_InvalidOutputToGimo           = 30008 // GIMO detected an invalid input



// Replies to PsGimoIncomingApi  From core servers to GW  (DEBUG range)
,   eSwissErr_InconsistentData              = 31002 // INCONSISTENT_DATA=6       
,   eSwissErr_AccCurrencyMismatch           = 31003 // ACCT_CURRENCY_MISMATCH=175
,   eSwissErr_DBMInternalError              = 31004 // DBM_INTERNAL_ERROR=-2     
,   eSwissErr_CurrencyNotSupported          = 31005 // CURRENCY_NOT_SUPPORTED=347
,   eSwissErr_Db2DuplicateKey               = 31006 // DB2_DUPLICATE_KEY=101     
,   eSwissErr_Db2SqlError                   = 31007 // DB2_SQL_ERROR=(-1)        
,   eSwissErr_WrongBonusCode                = 31008 // DBM_WRONG_BONUS_CODE=133  
,   eSwissErr_NotAllowed                    = 31009 // NOT_ALLOWED = 21          
,   eSwissErr_DbmBarredReturn               = 31010 // DBM_BARRED_RETURN=132     
,   eSwissErr_NoSuchRecord                  = 31011 // NO_SUCH_RECORD=202        
,   eSwissErr_UnexpectedCount               = 31012 // UNEXPECTED_COUNT=3        
,   eSwissErr_ReservedUserId                = 31013 // RESERVED_USER_ID=301      
,   eSwissErr_NoSuchUser                    = 31014 // NO_SUCH_USER=2            
,   eSwissErr_TooEarlytoCashout             = 31015 // TOO_EARLY_TO_CASHOUT=50   

,   eSwissErr_AcctLicenseMismatch2          = 31021 // ACCT_LICENSE_MISMATCH2=258    
,   eSwissErr_DuplicateNewUserEmail         = 31022 // DUPLICATE_NEW_USER_EMAIL=35   
,   eSwissErr_DuplicateNewUserMobile        = 31023 // DUPLICATE_NEW_USER_MOBILE=30  
,   eSwissErr_TooManyFailures               = 31025 // TOO_MANY_FAILURES=404         
,   eSwissErr_TooManyRequests               = 31026 // TOO_MANY_REQUESTS=403         
,   eSwissErr_InternalLogicalError          = 31028 // INTERNAL_LOGICAL_ERROR 7      
,   eSwissErr_ServiceNotAvailable           = 31029 // SERVICE_NOT_AVAILABLE 204     

// Replies to PsGimoIncomingApi   From core servers to GW  (Localized range)
,   eSwissErr_InvalidTinfoManualPromo       = 31504 // INVALID_TINFO_MANUAL_PROMO=161
,   eSwissErr_NoAlternateUserName           = 31507 // NO_ALTERNATE_USER_NAME=315    
,   eSwissErr_UserAlreadyExist              = 31510 // USER_ALREADY_EXISTS=1         
,   eSwissErr_AlreadyBanned                 = 31511 // ALREADY_BANNED=158            
,   eSwissErr_AccountClosed                 = 31512 // DBM_ACCOUNT_CLOSED=71         
,   eSwissErr_TransactionPending            = 31513 // TRANSACTION_PENDING=46        
,   eSwissErr_UserSelfExcluded              = 31514 // USER_SELF_EXCLUDED 148        
,   eSwissErr_UserBannedByRegulator         = 31515 // USER_BANNED_BY_REGULATOR 421  
,   eSwissErr_UserHasUnopenchests           = 31516 // USER_HAS_UNOPEN_CHESTS 603    
,   eSwissErr_NotAllowedUseActive           = 31517 // NOT_ALLOWED_USER_ACTIVE 206   
,   eSwissErr_UserOwesChips                 = 31518 // ERR_USER_OWES_CHIPS 240       
,   eSwissErr_UserHasPendingCashouts        = 31519 // USER_HAS_PENDING_CASHOUTS 188 
,   eSwissErr_UserHasAssets                 = 31520 // USER_HAS_ASSETS 608           
,   eSwissErr_UserAlreadyMigrated           = 31521 // USER_ALREADY_MIGRATED 370     
,   eSwissErr_NotEnoughChips                = 31522 // NOT_ENOUGH_CHIPS=5             // PYR-105137
};






#endif // SwissGW_ErrorCodes_H_included