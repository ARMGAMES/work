enum AsiaB2B_ErrorCode : INT16
{
	eAsiaErr_Success       =     0
,	eAsiaErr_AG_RangeBegin =     1
,	eAsiaErr_AG_RangeEnd   =    99
,	eAsiaErr_Op_RangeBegin =   100
,	eAsiaErr_Op_RangeEnd   = 29999
,	eAsiaErr_PS_RangeBegin = 30000
,	eAsiaErr_PS_RangeEnd   = 32767

,	eAsiaErr_MissingRequestId                      =  1 // as received from AG
,	eAsiaErr_MissingRequestChecksum                =  2 // as received from AG
,	eAsiaErr_MissingRequestParameter               =  3 // as received from AG
,	eAsiaErr_ChecksumMismatch                      =  4 // as received from AG
,	eAsiaErr_AccountDoesNotExistOrIncorrectPassword= 10 // as received from AG
,	eAsiaErr_WrongAgAccountId                      = 20 // as received from AG
,	eAsiaErr_InvalidToken                          = 21 // as received from AG
,	eAsiaErr_InsufficientFunds                     = 22 // as received from AG
,	eAsiaErr_InvalidOperatorID                     = 23 // as received from AG
,	eAsiaErr_InternalServerError97                 = 97 // as received from AG
,	eAsiaErr_InternalServerError98                 = 98 // as received from AG
,	eAsiaErr_InternalServerError99                 = 99 // as received from AG

,	eAsiaErr_Internal       = 30000
,	eAsiaErr_InvalidInput   = 30001
,	eAsiaErr_HttpFailed     = 30002
,	eAsiaErr_UserNotLoggedIn= 30003
,	eAsiaErr_NoUser         = 30004 // PYR-75040
,	eAsiaErr_GatewayError   = 30005 // PYR-75040
};