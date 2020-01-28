//loginhelper.cpp    Copyright (C) 2009 PYR Software Ltd. All rights reserved.
#include "loginhelper.h"
#include "loginerror.h"
#include "dbm.h"
#include "pyrtime.h"
#include "commqueue.h"
#include "licenserules.h"
#include "commssl.h"
#include "brands.h"
#include "sqlerror.h"
#include "msguserauth.h"

#define TXT_AUTH_ERROR_PASSWORD "The password you have entered is incorrect."
#define TXT_AUTH_DOUBLE_LOGIN_ATTEMPT "You are already logged-in from another computer."
#define TXT_AUTH_ACCT_WAS_CONVERTED "Your account does not correspond with this version of the PokerStars client software."
#define TXT_AUTH_ACCT_LICENSE_MISMATCH_IT "The username does not correspond with this version of the PokerStars client software."
#define TXT_AUTH_ACCT_LICENSE_MISMATCH_OTHER "The username does not correspond with this version of the PokerStars.it client software."
#define TXT_AUTH_LOGIN_RESTRICTED "Access to your account on the PokerStars.com site is not permitted from your current location."
#define TXT_AUTH_INTERNAL_ERROR "Internal Error " // + error description
#define TXT_AUTH_ITALY_NOT_ALLOWED_ON_COM "Login prohibited: Italian player on .COM"
#define TXT_AUTH_ACCTOUNT_CLOSED "You are unable to Log In as your PokerStars account has been frozen."
#define TXT_AUTH_ACCT_LICENSE_MISMATCH "The username does not correspond with this version of the PokerStars client software."
#define TXT_AUTH_FRENCH_NOT_ALLOWED_ON_COM "Login prohibited: French player on .COM"
#define TXT_AUTH_ESTONIA_NOT_ALLOWED_ON_COM "Login prohibited: Estonian player on .COM"
#define TXT_AUTH_SPAIN_NOT_ALLOWED_ON_COM "Login prohibited: Spanish player on .COM"
#define TXT_AUTH_DANISH_NOT_ALLOWED_ON_COM "Login prohibited: Danish player on .COM"
#define TXT_AUTH_HOME_GAMES_CLUB_NOT_ALLOWED "Sorry, you cannot access this club."
#define TXT_AUTH_HOME_GAMES_ALLOWED "Sorry, you cannot access Home Games."
#define TXT_AUTH_USER_LOGIN_BLOCKED_BY_CODE "Due to system maintenance, login is not allowed. Please try again in a few minutes."
#define TXT_AUTH_USER_LOGIN_BLOCKED_LOWER_CLIENT_VERSION "The table or tournament is not available in your current PokerStars software. Please update your software to join."
#define TXT_AUTH_USER_LOGIN_BLOCKED_STRONG_AUTH_REQD "Strong Authentication Required."
#define TXT_AUTH_USER_LOGIN_IT_BLOCKED "Due to maintenance of the AAMS (Agency of Customs and Monopolies) systems, access to gaming accounts is not allowed. We apologize for any inconvenience."	// PYR-28691
#define TXT_AUTH_USER_LOGIN_BLOCKED_REVALIDATION_REQD "Account revalidation required."
#define TXT_AUTH_USER_LOGIN_BLOCKED_NJ_LOGIN_LIMIT_REACHED "Sorry, it is not possible to login to your account at this time.\nPlease note that the PokerStars NJ software is currently undergoing 'soft-launch' which includes restrictions on the hours of play and maximum number of players. Please see <a href=\"[#softlaunch_login_unavailable#]\">our website</a> for more details."
#define TXT_AUTH_USER_LOGIN_BLOCKED_HTML5_CLIENT "Access to our web client is not currently enabled for your account. Please install the downloadable client."

#define TXT_AUTH_ACCOUNT_CLOSED_2 "In accordance with the French regulations, your account has been closed for one of the following reasons:\nInactivity in real money\nMissing documents\nPermanent self-exclusion or responsible gaming request\n\nPlease note that you can open a new account if your account has been closed for inactivity or missing documents.\n\nContact support@pokerstars.fr for more information."

#define TXT_AUTH_LOGIN_FTPUSER_ALREADY_MIGRATED "You have already migrated. Please input your PokerStars credentials." // PYR-40997
#define TXT_AUTH_LOGIN_BLOCKED_BANNED_BY_REGULATOR "Your account privileges have been suspended because you are listed in the Government exclusion list. We apologise for any inconvenience. If you have any questions, please contact us at support@pokerstarsnj.com"
#define TXT_AUTH_USER_LOGIN_BLOCKED_PT_RSA_NOT_ALLOWED "Login prohibited: PT RSA not allowed."

const char* LoginHelper::composePwdError( CommMsgBody& expl )
{
	expl.composeUINT32( AuthSubErrorPwd );
	return TXT_AUTH_ERROR_PASSWORD;
}

void LoginHelper::appendSuccessInfo( CommMsgBody& expl, UINT64 flags2, UINT32 authFlags, const char* frArjelUserHash )
{
	AuthLogInCode authLogInCode;
	if ((authFlags & eAuthResponseFlags_LimitedModeRSA ) != 0) // PYR-24969
	{
		authLogInCode = AuthLogInLimitedRSA;
	}
	else if ((authFlags & eAuthResponseFlags_LimitedModeEULA ) != 0) // PYR-24802
	{
		authLogInCode = AuthLogInLimitedEULA;
	}
	else if ((authFlags & eAuthResponseFlags_LimitedModeLTDL ) != 0) // PYR-42099
	{
		authLogInCode = AuthLogInLimitedLTDL;
	}
	else if ((flags2 & newInstallIdForSmsDetected) != 0)
	{
		authLogInCode = AuthLogInLimitedSMS;
	}
	else if (authFlags & eAuthResponseFlags_LimitedModeGeoIPPending)
	{
		authLogInCode = AuthLogInLimitedGeoIpPending;
	}
	else if (authFlags & eAuthResponseFlags_LimitedModeGeoIPRejected)
	{
		authLogInCode = AuthLogInLimitedGeoIpRejected;
	}
	else if (authFlags & eAuthResponseFlags_LimitedModeGeoIPUnknown)
	{
		authLogInCode = AuthLogInLimitedGeoIpUnknown;
	}
	else if (authFlags & eAuthResponseFlags_LimitedModeGeoIPBlockedSoftware)
	{
		authLogInCode = AuthLogInLimitedGeoIpBlockedSoftware;
	}
	else if (authFlags & eAuthResponseFlags_LimitedModeGeoIPUsingProxy)
	{
		authLogInCode = AuthLogInLimitedGeoIpUsingProxy;
	}
	else if (authFlags & eAuthResponseFlags_LimitedModeInExclusionBoundary) // PYR-74823
	{
		authLogInCode = AuthLogInLimitedInExclusionBoundary;
	}
	else
	{
		authLogInCode = AuthLogInNormal;
	}

	CommMsgBody successInfo;
	successInfo
		.composeUINT32((UINT32)authLogInCode)
		.composeString(frArjelUserHash); // PYR-23674
	expl.composeMsgBody(successInfo);
}

//	PYR-52518
const char* LoginHelper::composeClientBody( CommMsgBody& clientBody, UINT64 flags2, UINT32 authFlags, const char* frArjelUserHash, UINT32 userIntId, UINT64 currentLoginId, 
	const CommMsgBody& externalOperatorBody, bool composeExternAuthBody)
{
	const char* explText = composePwdError( clientBody );
	appendSuccessInfo( clientBody, flags2, authFlags/*PYR-24802*/, frArjelUserHash ); // PYR-21829

	clientBody
		.composeUINT32( userIntId )
		.composeUINT64( currentLoginId );	//	currently only non-zero when composed by WebTokenServer

	if (composeExternAuthBody)
	{
		clientBody.composeMsgBody(externalOperatorBody);
	}
	return explText;
}

const char* LoginHelper::composeClientBody( CommMsgBody& clientBody, UINT64 flags2, UINT32 authFlags, const char* frArjelUserHash, bool composeExternAuthBody)
{
	return composeClientBody( clientBody, flags2, authFlags, frArjelUserHash, 0 /* userIntId */, 0 /* currentLoginId */, CommMsgBody() /* externalOperatorBody */, composeExternAuthBody);
}

void LoginHelper::interpretError(INT16 err_code, const char* errDescr, UINT32 location, UINT16& err_code_for_client, PString& errDescrForUser, CommMsgBody& expl )
{
	switch(err_code)
	{
		case NO_SUCH_USER:
		case INSTALLID_FROZEN:  // PYR-27302
		case MAC_FROZEN:		// PYR-43862
		case VOLUME_FROZEN:		// PYR-43862
		case IP_FROZEN:			// PYR-43862
		case ROUTERMAC_FROZEN:	// PYR-43862
		case ERR_FULLTILT_FT_USER_NOT_FOUND: // PYR-40997
		case STRING_TOO_LONG:	// PYR-49451
		{
			errDescrForUser.assign( TXT_AUTH_ERROR_PASSWORD );
			expl.composeUINT32( AuthSubErrorPwd );
			break;
		}
		case ACCESS_NEW_IP_RESOLUTION: // #22500
		case DOUBLE_LOGIN_ATTEMPT:
		{
			errDescrForUser.assign( TXT_AUTH_DOUBLE_LOGIN_ATTEMPT );
			expl.composeUINT32( AuthSubErrorDoubleLoginAttempt );
			break;
		}
		case USER_NAME_TYPO: // when (norm(cliUserId) == norm (srvUserId))
		{
			err_code_for_client = USER_LOGIN_ERROR_TYPO;
			errDescrForUser = errDescr; // Provided by Dbm
			composeUserIdTypoForClient(expl, errDescr/*srvUserId*/, eLoginCredentialType_UserId, ""/*email ignored*/);
			break;
		}
		case USER_LOGIN_VIA_EMAIL:
		{
			err_code_for_client = USER_LOGIN_ERROR_TYPO;
			msgBodyDecode(errDescr/*in*/, expl/*output param*/);
			CommMsgParser parser(expl);
			parser.parseStringP(errDescrForUser); // srvUserId
			break;
		}
		case USER_LOGIN_RSA_REQUIRED:
		{
			err_code_for_client = USER_LOGIN_RSA_REQUIRED;
			// PYR-16777
			UINT64 flags2 = p_atou64(errDescr);
			expl.composeUINT64( flags2 );
			break;
		}
		case USER_LOGIN_EXCLUDED_STATE:
		{
			err_code_for_client = USER_LOGIN_EXCLUDED_STATE;
			errDescrForUser = errDescr; // Provided by Dbm
			break;
		}
		case USER_BIRTHDAY_REQUIRED:
		{
			err_code_for_client = USER_LOGIN_BIRTHDAY_REQUIRED;
			errDescrForUser = errDescr; // Provided by Dbm
			break;
		}
		case USER_BIRTHDAY_INVALID:
		{
			err_code_for_client = USER_LOGIN_BIRTHDAY_INVALID;
			errDescrForUser = errDescr; // Provided by Dbm
			break;
		}
		case ACCT_WAS_CONVERTED:
		{
			errDescrForUser.assign( TXT_AUTH_ACCT_WAS_CONVERTED );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorWasConverted );
			break;
		}
		case ACCT_LICENSE_MISMATCH:
		{
			UINT32 accountLicenseId = atoi(errDescr);
			if(accountLicenseId)//italian account, .net/.com client
			{
				errDescrForUser.assign( TXT_AUTH_ACCT_LICENSE_MISMATCH_IT );
				expl.composeUINT32( AuthSubErrorLicenseMismatchIt );
			}
			else//.net/.com account, italian client
			{
				errDescrForUser.assign( TXT_AUTH_ACCT_LICENSE_MISMATCH_OTHER );
				expl.composeUINT32( AuthSubErrorLicenseMismatchNotIt );
			}
			err_code_for_client = 0;
			break;
		}
		case USER_LOGIN_BLOCKED_NJ_ON_IOM: // PYR-28389
		case ACCT_LICENSE_MISMATCH2:
		{
			UINT32 accountLicenseId = atoi(errDescr);
			errDescrForUser.assign( TXT_AUTH_ACCT_LICENSE_MISMATCH );
			expl
				.composeUINT32(AuthSubErrorLicenseMismatch)
				.composeUINT32(accountLicenseId);
			err_code_for_client = 0;
			break;
		}
		case USER_LOGIN_GOTO_REGULATOR_FIRST: // PYR-39475
		{
			errDescrForUser = ""; // .assign( TXT_AUTH_ACCT_LICENSE_MISMATCH );
			err_code_for_client = err_code;
			break;
		}
		case LOGIN_RESTRICTED:
		{
			errDescrForUser.assign( TXT_AUTH_LOGIN_RESTRICTED );
			err_code_for_client = 0;
			expl
				.composeUINT32( AuthSubErrorRestricted )
				.composeString(errDescr); // Country code determined by user's IP
			break;
		}
		case DBM_ACCOUNT_CLOSED:
		{
			errDescrForUser.assign( TXT_AUTH_ACCTOUNT_CLOSED );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorAcctountClosed );
			break;
		}
		case DBM_ACCOUNT_CLOSED_2:
		{
			errDescrForUser.assign( TXT_AUTH_ACCOUNT_CLOSED_2 );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorAcctountClosed2 );
			break;
		}
		case ITALY_NOT_ALLOWED_ON_COM:
		{
			errDescrForUser.assign( TXT_AUTH_ITALY_NOT_ALLOWED_ON_COM );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorItOnCOM );
			break;
		}
		case FRENCH_NOT_ALLOWED_ON_COM:
		{
			errDescrForUser.assign( TXT_AUTH_FRENCH_NOT_ALLOWED_ON_COM );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorFrOnCOM );
			break;
		}
		case ESTONIANS_NOT_ALLOWED_ON_COM:
		{
			errDescrForUser.assign( TXT_AUTH_ESTONIA_NOT_ALLOWED_ON_COM );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorEeOnCOM );
			break;
		}
		case SPAIN_NOT_ALLOWED_ON_COM:
		{
			errDescrForUser.assign( TXT_AUTH_SPAIN_NOT_ALLOWED_ON_COM );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorEsOnCOM );
			break;
		}
		case DANISH_NOT_ALLOWED_ON_COM:
		{
			errDescrForUser.assign( TXT_AUTH_DANISH_NOT_ALLOWED_ON_COM );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorDkOnCOM );
			break;
		}
		case HOME_GAMES_CLUB_NOT_ALLOWED:
		{
			errDescrForUser.assign( TXT_AUTH_HOME_GAMES_CLUB_NOT_ALLOWED );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorHgNotAllowed );
			break;
		}
		case HOME_GAMES_NOT_ALLOWED_FR:
		{
			errDescrForUser.assign( TXT_AUTH_HOME_GAMES_ALLOWED );
			err_code_for_client = 0;
			expl.composeUINT32( AuthSubErrorHgNotAllowedFr );
			break;
		}
		case USER_LOGIN_TABLE_BLOCKED:
		{
			err_code_for_client = USER_LOGIN_TABLE_BLOCKED;
			UINT32 isPlayMoney = atoi(errDescr);
			expl.composeBOOL(isPlayMoney != 0);
			break;
		}
		case USER_LOGIN_EXPLICIT_DK_REQUIRED: // PYR-16706
		{
			err_code_for_client = USER_LOGIN_EXPLICIT_DK_REQUIRED;
			// #22156
			// errDescr format: userIntId;time_t;sha1
			UINT32 userIntId = 0;
			const char* firstDelimiter = PString::find(errDescr, ';');
			if(firstDelimiter)
			{
				userIntId = atoi(PString(errDescr, firstDelimiter));
			}
			expl
				.composeUINT32(userIntId)
				.composeString(errDescr);
			break;
		}
		case USER_LOGIN_USER_ALREADY_CONNECTED:
		{
			err_code_for_client = USER_LOGIN_USER_ALREADY_CONNECTED;
			errDescrForUser = errDescr; // i18n_ed by caller-cashier
			break;
		}
		case USER_WEB_CASHIER_NEVER_USED_MOBILE_PLATFORM: // PYR-23460
		{
			err_code_for_client = USER_WEB_CASHIER_NEVER_USED_MOBILE_PLATFORM;
			errDescrForUser = errDescr;
			break;
		}
		case USER_LOGIN_BLOCKED_BY_CODE: // PYR-24812
		{
			err_code_for_client = USER_LOGIN_BLOCKED_BY_CODE;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_BY_CODE;
		
			// errDescr contains: subCat,cat
			UINT32 cat = 0, subCat = 0;
			decodeTemplateCacheCatSubcat( errDescr, cat, subCat );
			expl.composeUINT32( subCat ) // blockingCode
				.composeUINT32( cat );	 // type of blocking
			break;
		}
		case USER_LOGIN_BLOCKED_LOWER_CLIENT_VERSION: // PYR-25260
		{
			err_code_for_client = USER_LOGIN_BLOCKED_LOWER_CLIENT_VERSION;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_LOWER_CLIENT_VERSION;
			UINT32 blockingCode = atoi(errDescr);
			expl.composeUINT32( blockingCode );
			break;
		}
		case USER_LOGIN_BLOCKED_STRONG_AUTH_REQD:	//	PYR-27258
		{
			err_code_for_client = USER_LOGIN_BLOCKED_STRONG_AUTH_REQD;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_STRONG_AUTH_REQD;
			break;
		}
		case USER_LOGIN_LIMITED_DUE_TO_GEOIPREJECTED: // PYR-28443
		{
			err_code_for_client = USER_LOGIN_LIMITED_DUE_TO_GEOIPREJECTED;
			errDescrForUser = errDescr;
			break;
		}
		case USER_LOGIN_ITALY_USER_LOGIN_BLOCKED:	// PYR-28691
		{
			err_code_for_client = USER_LOGIN_ITALY_USER_LOGIN_BLOCKED;
			errDescrForUser = TXT_AUTH_USER_LOGIN_IT_BLOCKED;
			break;
		}
		case USER_LOGIN_BLOCKED_REVALIDATION_REQD:	//	PYR-31219
		{
			err_code_for_client = USER_LOGIN_BLOCKED_REVALIDATION_REQD;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_REVALIDATION_REQD;
			break;
		}
		case USER_LOGIN_BLOCKED_NJ_LOGIN_LIMIT_REACHED: // PYR-31758
		{
			err_code_for_client = USER_LOGIN_BLOCKED_NJ_LOGIN_LIMIT_REACHED;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_NJ_LOGIN_LIMIT_REACHED;
			break;
		}
		case USER_LOGIN_BLOCKED_HTML5_CLIENT:  //PYR-33221
		{
			err_code_for_client = USER_LOGIN_BLOCKED_HTML5_CLIENT;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_HTML5_CLIENT;
			break;
		}
		case ERR_FULLTILT_FT_USER_ALREADY_CONVERTED: // PYR-40997
		{
			err_code_for_client = USER_LOGIN_FTPUSER_ALREADY_MIGRATED;
			errDescrForUser = TXT_AUTH_LOGIN_FTPUSER_ALREADY_MIGRATED;
			break;
		}
		case USER_LOGIN_BLOCKED_BANNED_BY_REGULATOR:	//	PYR-40995
		{
			err_code_for_client = USER_LOGIN_BLOCKED_BANNED_BY_REGULATOR;
			errDescrForUser = TXT_AUTH_LOGIN_BLOCKED_BANNED_BY_REGULATOR;
			break;
		}
		case USER_LOGIN_BLOCKED_PT_RSA_NOT_ALLOWED:		//	PYR-44757
		{
			err_code_for_client = USER_LOGIN_BLOCKED_PT_RSA_NOT_ALLOWED;
			errDescrForUser = TXT_AUTH_USER_LOGIN_BLOCKED_PT_RSA_NOT_ALLOWED;
			break;
		}
		case USER_LOGIN_BLOCKED_LOGIN_TOTAL_LIMIT_REACHED: // PYR-47770
		{
			err_code_for_client = USER_LOGIN_BLOCKED_LOGIN_TOTAL_LIMIT_REACHED;
			errDescrForUser = "user login total limit has reached";
			INT32 loginTotalLimit = NO_TABLE_LIMIT_MARKER;
			PString dateStr;
			vector<PString> strVec;
			readStrVectorFromString(errDescr, strVec, ',');
			if (strVec.size()==2)
			{
				loginTotalLimit = atoi(strVec[0]);
				dateStr = strVec[1];
			}
			expl
				.composeINT32(loginTotalLimit)
				.composeString(dateStr);
			PTRACE5("USER_LOGIN_BLOCKED_LOGIN_TOTAL_LIMIT_REACHED: loginTotalLimit=%d, date='%s'", loginTotalLimit, dateStr.c_str());
			break;
		}
		case USER_LOGIN_BLOCKED_LOGIN_COOL_OFF: // PYR-105467
		{
			err_code_for_client = USER_LOGIN_BLOCKED_LOGIN_COOL_OFF;
			errDescrForUser = "user login cool off";
			INT32 loginCoolOffLimit = NO_TABLE_LIMIT_MARKER;
			INT32 remainingMinutes = NO_TABLE_LIMIT_MARKER;
			vector<PString> strVec;
			readStrVectorFromString(errDescr, strVec, ',');
			if (strVec.size() == 2)
			{
				loginCoolOffLimit = atoi(strVec[0]);
				remainingMinutes = atoi(strVec[1]);
			}
			expl
				.composeINT32(loginCoolOffLimit)
				.composeINT32(remainingMinutes);
			PTRACE5("USER_LOGIN_BLOCKED_LOGIN_COOL_OFF: loginCoolOffLimit=%d, blockedUntil='%d'", loginCoolOffLimit, remainingMinutes);
			break;
		}
		case USER_LOGIN_BLOCKED_LOGIN_TIME_LIMIT_REACHED: // PYR-47770
		{
			err_code_for_client = USER_LOGIN_BLOCKED_LOGIN_TIME_LIMIT_REACHED;
			errDescrForUser = "user login time limit has reached";
			INT32 loginTimeLimit = NO_TABLE_LIMIT_MARKER;
			PString dateStr;
			INT32 limitType = eLimitTypePlayingTime;
			vector<PString> strVec;
			readStrVectorFromString(errDescr, strVec, ',');
			if (strVec.size() == 3) 
			{
				loginTimeLimit = atoi(strVec[0]);
				dateStr = strVec[1];
				limitType = atoi( strVec[2] );
			}
			expl
				.composeINT32(loginTimeLimit)
				.composeString(dateStr)
				.composeINT32( limitType );
			PTRACE5("USER_LOGIN_BLOCKED_LOGIN_TIME_LIMIT_REACHED: loginTimeLimit=%d, date='%s'", loginTimeLimit, dateStr.c_str());
			break;
		}
		case USER_LOGIN_BLOCKED_SILENT_LOGIN_NOT_ALLOWED:	//	PYR-52912
		{
			err_code_for_client = USER_LOGIN_BLOCKED_SILENT_LOGIN_NOT_ALLOWED;
			errDescrForUser = "Silent login not allowed";
			break;
		}
		case ERR_APC_ON_DIFF_DEVICE:		//	PYR-52886. Authenticated Poker Connection exists on a different device.
		{
			err_code_for_client = 0;
			errDescrForUser = "APC exists on a different device";
			expl.composeUINT32( AuthSubErrorAPCOnDiffDevice );
			break;
		}
		case ERR_APC_ON_SAME_DEVICE:		//	PYR-52886. Authenticated Poker Connection exists on the same device.
		{
			err_code_for_client = 0;
			errDescrForUser = "APC exists on the same device";
			expl.composeUINT32( AuthSubErrorAPCOnSameDevice );
			break;
		}
		case ERR_TRANSFER_APC_NOT_READY:	//	PYR-52886. Transfer of Authenticated Poker Connection is not ready.
		{
			err_code_for_client = 0;
			errDescrForUser = "Transfer of APC not ready";
			expl.composeUINT32( AuthSubErrorTransferAPCNotReady );
			break;
		}
		case USER_LOGIN_BLOCKED_SERVICE_NOT_AVAILABLE:	//	PYR-52518
		{
			err_code_for_client = USER_LOGIN_BLOCKED_SERVICE_NOT_AVAILABLE;
			errDescrForUser = "Service not available. Please try again later.";
			break;
		}
		default:
		{
			PString strCommonErr;
			PString strLocation;
			strCommonErr.appendInt( err_code );
			strLocation.appendUint( location ) ;
			errDescrForUser.assign( TXT_AUTH_INTERNAL_ERROR );
			errDescrForUser.append("(").append( strCommonErr ).append( '/' ).append( strLocation ).append(")");
			expl.composeUINT32( AuthSubErrorInternal ).composeINT32( err_code ).composeUINT32( location );
			break;
		}
	}
}

void LoginHelper::parseClientLoginExtra( const CommMsgBody& clientExtra, ClientLoginExtraData& e )
{
	CommMsgParser parser(clientExtra);
	if( parser.parseEnded() ) // admin
	{
		e.macAddr = "";
		e.volumeLabel = "";
		e.installId = "";
		e.machineGuid = "";
		e.adapterGuid = "";
		e.userExtraId = "";
		e.clientSiteId = 0;
		e.clientPlatform = 0;
		e.clientExtraInfoMask = 0;
		e.clientType = 0;	// PYR-24582
		e.clientDevice = 0;	// PYR-24582
		e.clientVersion = 0; // PYR-25260
		e.routerMacAddr = ""; // PYR-29604
		e.clientBrand = BrandType_PokerStars; // PYR-37793
		e.osId = OSId_Unknown;
		e.appTypeId = AppTypeId_Unknown;
		e.productId = ProductId_Unknown;
		e.installId2 = "";
		e.operatorId = ""; // PYR-52000
		e.osVersion = "";  // PYR-52518
		return;
	}

	parser.
		parseString( e.macAddr ).
		parseString( e.volumeLabel ).
		parseString( e.installId ).
		parseString( e.machineGuid ).
		parseString( e.adapterGuid ).
		parseString( e.userExtraId ).
		parseUINT32( e.clientSiteId ).
		parseUINT32( e.clientPlatform ).
		parseUINT32( e.clientExtraInfoMask );
	// Client protocol version: LOBBY_VERSION "1.75"; LOBBY_VERSION_COMPATIBLE "1.76"
	if(!parser.parseEnded())	// PYR-24582
	{
		parser.parseUINT32( e.clientType )
			  .parseUINT32( e.clientDevice );
	}
	else
	{
		e.clientDevice = 0;
		e.clientType = 0;
		PLog("old client - clientDevice s=%u,p=%u", e.clientSiteId, e.clientPlatform);
	}

	if(!parser.parseEnded())	// PYR-25260
	{
		parser.parseUINT32( e.clientVersion );
	}
	else
	{
		e.clientVersion = 0;
		PLog("old client - clientVersion s=%u,p=%u,t=%u,d=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice);
	}

	if (!parser.parseEnded()) // PYR-29604
	{
		parser.parseString( e.routerMacAddr );
	}
	else
	{
		e.routerMacAddr = "";
		PLog("old client - no routerMac s=%u,p=%u,t=%u,d=%u,v=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice, e.clientVersion);
	}

	if( !parser.parseEnded()) // PYR-37793
	{
		parser.parseUINT32(e.clientBrand);
	}
	else
	{
		e.clientBrand = BrandType_PokerStars;
		PLog("old client - no brandsUsed s=%u,p=%u,t=%u,d=%u,v=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice, e.clientVersion);
	}
	
	// PYR-38296
	if (!parser.parseEnded())
	{
		parser
			.parseUINT32(e.osId)
			.parseUINT32(e.appTypeId)
			.parseUINT32(e.productId);
	}
	else
	{
		e.osId = OSId_Unknown;
		e.appTypeId = AppTypeId_Unknown;
		e.productId = ProductId_Unknown;
		// lingz todo: change to PLog after most clients have reported these parameters
		PTRACE5("old client - no productId etc - s=%u,p=%u,t=%u,d=%u,v=%u, b=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice, e.clientVersion, e.clientBrand);
	}

	if ( !parser.parseEnded() )		//	PYR-51188
	{
		parser.parseString( e.installId2 );
	}
	else
	{
		e.installId2 = "";
		//	TODO: Gordonu: change to PLog after most clients have reported these parameters
		PTRACE5("old client - no secondaryInstallId - s=%u,p=%u,t=%u,d=%u,v=%u, b=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice, e.clientVersion, e.clientBrand);
	}

	if ( !parser.parseEnded() )		//	PYR-52000
	{
		parser.parseString( e.operatorId );
	}
	else
	{
		e.operatorId = "";
		//	TODO: andriyb: change to PLog after most clients have reported these parameters
		PTRACE5( "old client - no operatorId - s=%u,p=%u,t=%u,d=%u,v=%u, b=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice, e.clientVersion, e.clientBrand );
	}

	if ( !parser.parseEnded() )		//	PYR-52518
	{
		parser.parseString( e.osVersion );
	}
	else
	{
		e.osVersion = "";
		//	TODO: Gordonu: change to PLog after most clients have reported.
		PTRACE5( "old client - no osVersion - s=%u,p=%u,t=%u,d=%u,v=%u,b=%u", e.clientSiteId, e.clientPlatform, e.clientType, e.clientDevice, e.clientVersion, e.clientBrand );
	}
}

bool LoginHelper::isPublicError( INT16 err_code )
{
	// PYR-11185, PYR-28389
	return ( USER_LOGIN_BLOCKED_NJ_ON_IOM == err_code ||
			 INSTALLID_FROZEN == err_code			  || // PYR-27302
			 MAC_FROZEN == err_code					  || // PYR-43862
			 VOLUME_FROZEN == err_code				  || // PYR-43862
			 IP_FROZEN == err_code					  || // PYR-43862
			 ROUTERMAC_FROZEN == err_code 			  || // PYR-43862
			 STRING_TOO_LONG == err_code				 // PYR-49451
			);
}

UINT32 LoginHelper::getMinClientVersion( UINT32 _clientPlatform, UINT32 _minClientVersion, UINT32 _minMobileVersion )
{
	if (PlatformRules::isMobilePlatform(_clientPlatform))
	{
		return _minMobileVersion;
	}
	else if (PlatformRules::isDesktopPlatform(_clientPlatform))
	{
		return _minClientVersion;
	}
	else
	{
		// minimum required version is 0 for web/core
		return 0;
	}
}

//static 
void LoginHelper::encodeTemplateCacheCatSubcat( UINT32 cat, UINT32 subCat, PString& str )
{
	// format: subCat,cat 
	// subCat is first because it was implemented first
	str.assign("").appendUint(subCat).append(',').appendUint(cat);
}

//static 
void LoginHelper::decodeTemplateCacheCatSubcat( const char* str, UINT32& cat, UINT32& subCat )
{
	// format: subCat,cat 
	// subCat is first because it was implemented first
	cat = 0;
	char* delim;
	subCat = strtoul(str, &delim, 10); // will interpret up to the first non-digit
	if( *delim == ',' )
	{
		cat = strtoul( delim + 1, NULL, 10 );
	}
}

void LoginHelper::msgBodyEncode(const CommMsgBody& body, PString& out)
{
	CommSSLBase64::encode(out, body._readPtr(), body._size());
}
void LoginHelper::msgBodyDecode(const PString& in, CommMsgBody& body)
{
	PBlock data;
	CommSSLBase64::decode(data, in.c_str(), in.length());
	memcpy(body._alloc(data.size()), data.ptr(), data.size()); //lint !e864 ... depends on order of evaluation
}

void LoginHelper::composeUserIdTypoForClient(CommMsgBody& explBody, const char* srvUserId, UINT32 loginCredentialType, const char* srvEmail)
{
	// for USER_LOGIN_ERROR_TYPO
	explBody
		.composeString(srvUserId)
		.composeUINT32(loginCredentialType)
		.composeString(srvEmail);
	// append only
}

void ClientLoginExtras2::parse(CommMsgParser& parser)
{
	parser
		.parseBYTE(objectType)
		.parseUINT64(objectId)
		.parseStringP(macAddr)
		.parseStringP(installId)
		.parseUINT32(clientSiteId)
		.parseUINT32(clientPlatform)
		.parseUINT32(clientExtraInfoMask)
		.parseUINT32(clientType)
		.parseUINT32(clientDevice)
		.parseUINT32(clientVersion)
		.parseStringP(routerMacAddr)
		.parseUINT32(brandId)
		.parseUINT32(osId)
		.parseUINT32(appTypeId)
		.parseUINT32(productId)
		.parseUINT64(appSessionId)
		.parseStringP(installId2)
		.parseStringP(volumeLabel)
		.parseStringP(userExtraId)
		;
	if ( !parser.parseEnded() )		//	PYR-52518. Remove after 2018.Main.04 has been deployed to NJ. GordonU - Remove AFTER 2019.04 if no faulty logic.
	{
		parser.parseStringP(operatorId);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "parseEnded ClientLoginExtras2::operatorId");
	}
	if ( !parser.parseEnded() )		//	PYR-52518.  Remove after 2018.main.06 has been deployed to NJ. GordonU - Remove AFTER 2019.04 if no faulty logic.
	{
		parser.parseStringP( osVersion );
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "parseEnded ClientLoginExtras2::osVersion");
	}
	// conditional parsing only
}
void ClientLoginExtras2::compose(CommMsgBody& body) const
{
	body
		.composeBYTE(objectType)
		.composeUINT64(objectId)
		.composeString(macAddr)
		.composeString(installId)
		.composeUINT32(clientSiteId)
		.composeUINT32(clientPlatform)
		.composeUINT32(clientExtraInfoMask)
		.composeUINT32(clientType)
		.composeUINT32(clientDevice)
		.composeUINT32(clientVersion)
		.composeString(routerMacAddr)
		.composeUINT32(brandId)
		.composeUINT32(osId)
		.composeUINT32(appTypeId)
		.composeUINT32(productId)
		.composeUINT64(appSessionId)
		.composeString(installId2)
		.composeString(volumeLabel)
		.composeString(userExtraId)
		.composeString(operatorId)
		.composeString(osVersion)
		;
	// append only
}
void ClientLoginExtras2::composeAsBody(CommMsgBody& body) const
{
	CommMsgBody thisBody;
	compose(thisBody);
	body.composeMsgBody(thisBody);
}
void ClientLoginExtras2::parseAsBody(CommMsgParser& parser)
{
	CommMsgBody thisBody;
	parser.parseMsgBody(thisBody);
	CommMsgParser thisParser(thisBody);
	parse(thisParser);
}
void ClientLoginExtras2::composeUserAuthMsg(
	CommMsgBody& request_,
	const char* userId_,
	const char* ipAddr_,
	INT32 port_,
	const CommMsgBody& bodyAuth_,
	const char* guardName_,
	UINT32 clubId_,
	UINT32 objectFlags_,
	UINT32 clientVersion_,
	UINT32 minClientVersion_,
	const char* pwdHash_,
	UINT64 parentAppSessionId_ ) const
{
	::composeUserAuthMsg( 
		userId_,
		macAddr,
		volumeLabel,
		installId,
		ipAddr_,
		port_,
		objectType,
		objectId,
		bodyAuth_,
		clientSiteId,
		clientPlatform,
		guardName_,
		request_,
		userExtraId,
		clubId_,
		objectFlags_,
		clientExtraInfoMask,
		clientType,
		clientDevice,
		clientVersion_,			//	Do NOT use loginExtras.clientVersion!!
		minClientVersion_,
		routerMacAddr,
		brandId,
		osId,
		appTypeId,
		productId,
		pwdHash_,
		parentAppSessionId_,
		installId2
		);
}

AppLoginEssentials::AppLoginEssentials(CommMsgParser& parser)
{
	parse(parser);
}

void AppLoginEssentials::parse(CommMsgParser& parser)	// PYR-42031
{
	parser
		.parseUINT64(appSessionId)
		.parseUINT32(brandId)
		;
	// conditional parsing only
}
void AppLoginEssentials::compose(CommMsgBody& body) const
{
	body
		.composeUINT64(appSessionId)
		.composeUINT32(brandId)
		;
	// apppend only
}
void AppLoginEssentials::composeAsBody(CommMsgBody& body) const
{
	CommMsgBody thisBody;
	compose(thisBody);
	body.composeMsgBody(thisBody);
}
void AppLoginEssentials::parseAsBody(CommMsgParser& parser)
{
	CommMsgBody thisBody;
	parser.parseMsgBody(thisBody);
	CommMsgParser thisParser(thisBody);
	parse(thisParser);
}