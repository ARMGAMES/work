//
// msguserauth.h
//

#ifndef msguserauth_h_included
#define msguserauth_h_included

#include "pyrtime.h"
#include "dbm.h"
#include "composecolls.h"
#include "pbitmask.h"
#include "sitemasks.h"

//
// The following public functions MUST be used to compose and parse the following messages:
//

//
// DBM_Q_USER_AUTH, DBM_Q_USER_TABLE_AUTH
//
inline void composeUserAuthMsg(
	const char* userId,
	const char* macAddr,
	const char* volumeLabel,
	const char* installId,
	const char* ipAddr,
	INT32 port,
	BYTE objectType,
	UINT64 objectId, // PYR-29549
	const CommMsgBody& bodyAuth,
	UINT32 siteId,
	UINT32 platformId,
	const char* guardName,
	CommMsgBody& request,
	const char* userExtraId = "",
	UINT32 clubId = 0,
	UINT32 objectFlags = 0,
	UINT32 clientExtraInfoMask = 0,
	UINT32 clientType = 0,
	UINT32 clientDevice = 0,
	UINT32 clientVersion = 0,
	UINT32 minClientVersion = 0,
	const char* routerMacAddr = "", // PYR-29604
	const char* pwdHash = "",
	UINT64 parentAppSessionId = 0, // PYR-38296: used only by webtokenserver, 0 for all other cases
	const char* installId2 = ""		//	PYR-51188
	) // IMPORTANT: remember to change composeUserAuthMsgForUserIntId() below at the same time
{
	request
		.composeString(userId)
		.composeString(macAddr)
		.composeString(volumeLabel)
		.composeString(installId)
		.composeString(ipAddr)
		.composeINT32(port)
		.composeBYTE(objectType)
		.composeUINT64(objectId) // PYR-29549
		.composeMsgBody(bodyAuth);

	switch(objectType)
	{
		case OBJ_TYPE_LOBBY:
		case OBJ_TYPE_TABLE:
		{
			request
				.composeUINT32(siteId)
				.composeUINT32(platformId)
				.composeString(userExtraId)
				.composeUINT32(clubId)
				.composeUINT32(objectFlags)
				.composeUINT32(clientExtraInfoMask)
				.composeUINT32(clientType)			// PYR-24582
				.composeUINT32(clientDevice)		// PYR-24582
				.composeUINT32(clientVersion)		// PYR-25260
				.composeUINT32(minClientVersion)
				.composeString(routerMacAddr)
				.composeString(pwdHash)
				.composeUINT64(parentAppSessionId)  // PYR-38296				
				.composeString(installId2)	// PYR-51188
				;
			break;
		}
		default:
			break;
	}

	// note that in future all callers are supposed to provide the connection guard name
	request.composeString(guardName ? guardName : "");
}

// PYR-32891
inline void composeUserAuthMsgForUserIntId(
	UINT32 userIntId,
	const char* macAddr,
	const char* volumeLabel,
	const char* installId,
	const char* ipAddr,
	INT32 port,
	BYTE objectType,
	UINT64 objectId, // PYR-29549
	const CommMsgBody& bodyAuth,
	UINT32 siteId,
	UINT32 platformId,
	const char* guardName,
	CommMsgBody& request,
	const char* userExtraId = "",
	UINT32 clubId = 0,
	UINT32 objectFlags = 0,
	UINT32 clientExtraInfoMask = 0,
	UINT32 clientType = 0,
	UINT32 clientDevice = 0,
	UINT32 clientVersion = 0,
	UINT32 minClientVersion = 0,
	const char* routerMacAddr = "", // PYR-29604
	const char* pwdHash = "",
	UINT64 parentAppSessionId = 0, // PYR-38296: used only by webtokenserver, 0 for all other cases
	const char* installId2 = ""		//	PYR-51188
	)
{
	PString userIntIdStr;
	userIntIdStr.appendUint(userIntId);
	composeUserAuthMsg(
		userIntIdStr, 
		macAddr,
		volumeLabel,
		installId,
		ipAddr,
		port,
		objectType,
		objectId,
		bodyAuth,
		siteId,
		platformId,
		guardName,
		request,
		userExtraId,
		clubId,
		objectFlags,
		clientExtraInfoMask,
		clientType,
		clientDevice,
		clientVersion,
		minClientVersion,
		routerMacAddr,
		pwdHash,
		parentAppSessionId,
		installId2
	);
}

inline void parseUserAuthMsg(
	CommMsgParser& parser,
	const char*& userId,
	const char*& macAddr,
	const char*& volumeLabel,
	const char*& installId,
	const char*& ipAddr,
	INT32& port,
	BYTE& objectType,
	UINT64& objectId, // PYR-29549
	CommMsgBody& bodyAuth,
	UINT32& siteId,
	UINT32& platformId,
	const char*& userExtraId,
	const char*& guardName,
	const char* guardNameDefault,
	UINT32& clubId,
	UINT32& objectFlags,
	UINT32& clientExtraInfoMask,
	UINT32& clientType,
	UINT32& clientDevice,
	UINT32& clientVersion,
	UINT32& minClientVersion,
	const char*& routerMacAddr, // PYR-29604
	const char*& pwdHash,
	UINT64& parentAppSessionId, // PYR-38296: used only by webtokenserver, 0 for all other cases
	const char*& installId2		//	PYR-51188
	)
{
	parser
		.parseString(userId)
		.parseString(macAddr)
		.parseString(volumeLabel)
		.parseString(installId)
		.parseString(ipAddr)
		.parseINT32(port)
		.parseBYTE(objectType)
		.parseUINT64(objectId) // PYR-29549
		.parseMsgBody(bodyAuth);

	switch(objectType)
	{
		case OBJ_TYPE_LOBBY:
		case OBJ_TYPE_TABLE:
		{
			parser
				.parseUINT32(siteId)
				.parseUINT32(platformId)
				.parseString(userExtraId)
				.parseUINT32(clubId)
				.parseUINT32(objectFlags)
				.parseUINT32(clientExtraInfoMask)
				.parseUINT32(clientType)			// PYR-24582
				.parseUINT32(clientDevice)		// PYR-24582
				.parseUINT32(clientVersion)		// PYR-25260
				.parseUINT32(minClientVersion)
				.parseString(routerMacAddr)
				.parseString(pwdHash)
				.parseUINT64(parentAppSessionId)
				.parseString(installId2)
				;
			break;
		}
		default:
		{
			siteId = 0;
			platformId = 0;
			userExtraId = "";
			clubId = 0;
			objectFlags = 0;
			clientExtraInfoMask = 0;
			clientType = 0;
			clientDevice = 0;
			clientVersion = 0;
			minClientVersion = 0;
			routerMacAddr = "";
			pwdHash = "";
			parentAppSessionId = 0;
			installId2 = "";
			break;
		}
	}

	parser.parseString(guardName);
	if( ! *guardName )
		guardName = guardNameDefault;
}

//
// DBM_A_USER_AUTH
//

inline void composeUserAuthOkReplyMsg
	(
	const CommMsgBody& authBody,
	UINT32 privileges,
	const char* city,
	UINT32 flags,
	const char* email,
	UINT32 userIntId,
	const char* country,
	const char* state,
	const CommMsgBody& dummyBody,
	const vector<PString>& rights,
	UINT32 ltFpp,
	UINT32 vipStatus,
	UINT32 locale,
	UINT32 fpp,
	UINT32 vipCounter1,
	UINT32 vipCounter2,
	UINT32 vipCounter3,
	UINT64 privileges2,
	UINT32 emLocale,
	UINT32 timezone_,
	UINT64 flags2,
	UINT32 originalUserIntId,
	const SrvTime& lastLogin,
	const char* countryByIp,
	const char* stateByIp,
	UINT32 licenseId,
	const char* fiscalCode,
	const CommMsgBody& loginInfoBody,
	const char* promoImageCode,
	UINT32 authFlags, // PYR-24802
	const char* frArjelUserHash, // PYR-23674
	UINT64 privileges3, // PYR-30588
	const char* userId, // PYR-32891
	const char* sex,	// PYR-33499
	UINT32 imageId,		//	PYR-35000
	UINT64 flags3,      // PYR-36189
	INT8 vipCounter1CentsOnly // PYR-40766
	,INT8 vipCounter2CentsOnly // PYR-40766
	,const SrvTime& registered // PYR-36446
	,INT32 targetRewardPoints	//	PYR-51670
	,INT32 pokerRPReward		//	PYR-51670
	,const char* defaultCurrency
	,INT32 loginAuthMethod // PYR-80352
	,CommMsgBody& reply  
	)
{
	reply.
		composeMsgBody(authBody).
		composeUINT32(privileges).
		composeString(city).
		composeUINT32(flags).
		composeString(email).
		composeUINT32(userIntId).
		composeString(country).
		composeString(state).
		composeMsgBody(dummyBody).
		composeUINT16( static_cast< UINT16 >( rights.size() ) ); // AN: cast size() explicitly
	for(size_t n = 0; n < rights.size(); n++)
	{
		reply.composeString(rights[n]);
	}
	reply.
		composeUINT32(ltFpp).
		composeUINT32(vipStatus).
		composeUINT32(locale).
		composeUINT32(fpp).
		composeUINT32(vipCounter1).
		composeUINT32(vipCounter2).
		composeUINT64(privileges2).
		composeUINT32(emLocale).
		composeUINT32(timezone_).
		composeSrvTime(SrvTime()). // gracePeriodUntil
		composeUINT64(flags2).
		composeUINT32(originalUserIntId).
		composeSrvTime(lastLogin).
		composeString(countryByIp).
		composeString(stateByIp);
	reply
		.composeUINT32(vipCounter3)
		.composeUINT32(licenseId)
		.composeString(fiscalCode)
		.composeMsgBody(loginInfoBody)
		.composeString(promoImageCode)
		.composeUINT32(authFlags)
		.composeString(frArjelUserHash)
		.composeUINT64(privileges3) // PYR-30588
		.composeString(userId) // PYR-32891
		.composeString(sex)	// PYR-33499
		.composeUINT32(imageId)	//	PYR-35000
		.composeUINT64(flags3) // PYR-36189
		.composeINT8(vipCounter1CentsOnly) // PYR-40766
		.composeINT8(vipCounter2CentsOnly) // PYR-40766
		.composeSrvTime(registered) // PYR-36446
		.composeINT32(targetRewardPoints)	//	PYR-51670
		.composeINT32(pokerRPReward)		//	PYR-51670
		.composeString(defaultCurrency)
		.composeINT32(loginAuthMethod); // PYR-80352
}

inline void composeUserAuthFailedReplyMsg
	(
	const char* errDescr,
	const CommMsgBody& authBody,
	UINT32 locale,
	CommMsgBody& reply
	)
{
	reply
		.composeString(errDescr)
		.composeMsgBody(authBody)
		.composeUINT32(locale);
}

//
// DBM_A_USER_TABLE_AUTH
//

inline void composeUserTableAuthOkReplyMsg
	(
	const CommMsgBody& authBody,
	UINT32 privileges,
	const char* city,
	UINT32 flags,
	const char* email,
	UINT32 fpp,
	UINT32 ltFpp,
	UINT32 chips,
	UINT64 tChips,
	UINT32 wChips,
	UINT64 flags2,
	UINT32 originalUserId,
	const char* country,
	const char* state,
	const char* promoImageCode,
	const char* countryByIp,
	const char* stateByIp,
	UINT32 licenseId,
	UINT64 privileges2,
	UINT32 authFlags, // PYR-24802
	const char* frArjelUserHash, // PYR-23674
	// PYR-32153
	const SrvTime& firstDeposit,
	const SrvTime& registered,
	INT32 monthlyVpp,
	INT32 yearlyVpp,
	INT64 playChips,
	const vector<PString>& trackCodes,
	// end of PYR-32153
	const char* userId,
	UINT64 appSessionId, // PYR-38296
	INT32 targetRewardPoints,	//	PYR-51670
	INT32 pokerRPReward,		//	PYR-51670
	UINT64 privileges3,			//	PYR-79935
	CommMsgBody& reply
	)
{
	reply
		.composeMsgBody(authBody)
		.composeUINT32(privileges)
		.composeString(city)
		.composeUINT32(flags)
		.composeString(email)
		.composeUINT32(fpp)
		.composeUINT32(ltFpp)
		.composeUINT32(chips)
		.composeUINT32(tChips)
		.composeUINT32(wChips)
		.composeUINT64(flags2)
		.composeUINT32(originalUserId)
		.composeString(country)
		.composeString(state)
		.composeString(promoImageCode)
		.composeString(countryByIp)
		.composeString(stateByIp)
		.composeUINT32(licenseId)
		.composeUINT64(privileges2)
		.composeUINT32(authFlags) // PYR-24802
		.composeString(frArjelUserHash)
		// PYR-32153
		.composeSrvTime(firstDeposit)
		.composeSrvTime(registered)
		.composeINT32(monthlyVpp)
		.composeINT32(yearlyVpp)
		.composeINT64(playChips)
		;

	composeVector(reply, trackCodes, ComposerPString()); // PYR-32153
	reply
		.composeString(userId)
		.composeUINT64(appSessionId)
		.composeINT32(targetRewardPoints)	//	PYR-51670
		.composeINT32(pokerRPReward)		//	PYR-51670
		.composeUINT64(privileges3)			//	PYR-79935
		;
}

inline void parseUserTableAuthReplyMsg
	(
	CommMsgParser& parser,
	INT16& errCode,
	const char*& errDescr,
	CommMsgBody& authBody,
	UINT32& privileges,
	const char*& city,
	UINT32& flags,
	const char*& email,
	UINT32& fpp,
	UINT32& ltFpp,
	UINT32& chips,
	UINT32& tChips,
	UINT32& wChips,
	UINT64& flags2,
	UINT32& originalUserIntId,
	const char*& country,
	const char*& state,
	const char*& promoImageCode,
	const char*& countryByIp,
	const char*& stateByIp,
	UINT32& licenseId,
	UINT64& privileges2,
	UINT32& authFlags, // PYR-24802
	const char*& frArjelUserHash, // PYR-23674
	// PYR-32153
	SrvTime& firstDeposit,
	SrvTime& registered,
	INT32& monthlyVpp,
	INT32& yearlyVpp,
	INT64& playChips,
	vector<PString>& trackCodes,
	// end of PYR-32153
	const char*& userId,
	UINT64 &appSessionId, // PYR-38296
	INT32& targetRewardPoints,	//	PYR-51670
	INT32& pokerRPReward,		//	PYR-51670
	UINT64& privileges3			//	PYR-79935
	)
{
	parser.parseINT16(errCode);
	if( errCode != DBM_NO_ERROR )
	{
		parser.parseString(errDescr);
		if( errCode != NO_SUCH_USER )
			parser.parseMsgBody(authBody);
	}
	else
	{
		parser.
			parseMsgBody(authBody).
			parseUINT32(privileges).
			parseString(city).
			parseUINT32(flags).
			parseString(email).
			parseUINT32(fpp).
			parseUINT32(ltFpp).
			parseUINT32(chips).
			parseUINT32(tChips).
			parseUINT32(wChips).
			parseUINT64(flags2).
			parseUINT32(originalUserIntId).
			parseString(country).
			parseString(state).
			parseString(promoImageCode).
			parseString(countryByIp).
			parseString(stateByIp).
			parseUINT32(licenseId).
			parseUINT64(privileges2).
			parseUINT32(authFlags). // PYR-24802
			parseString(frArjelUserHash)
		// PYR-32153
			.parseSrvTime(firstDeposit)
			.parseSrvTime(registered)
			.parseINT32(monthlyVpp)
			.parseINT32(yearlyVpp)
			.parseINT64(playChips)
			;

		parseVectorWithOverride(parser, trackCodes, ComposerPString()); // PYR-32153
		parser
			.parseString(userId)
			.parseUINT64(appSessionId)
			.parseINT32(targetRewardPoints)
			.parseINT32(pokerRPReward)
			.parseUINT64(privileges3);
	}
}

//
// DBM_Q_LOGIN_CONFIRM
//
inline void composeLoginConfirmMsg(
	const char* userId,
	CommMsgBody& request,
	UINT64 appSessionId = 0 // PYR-38296
	)
{
	request
		.composeString(userId)
		.composeUINT32(eAuthUnknown)
		.composeUINT64(appSessionId)
		;

	// Note: do not add new field. Consult LingZ first
}

inline void composeLoginConfirmMsgWithClientType(
	const char* userId,
	UINT32 clientType,
	CommMsgBody& request,
	UINT64 appSessionId  // PYR-38296
	)
{	
	request
		.composeString(userId)		
		.composeUINT32(eAuthUnknown)
		.composeUINT64(appSessionId)
		.composeUINT32(clientType)
		;
}
//
// DBM_Q_USER_LOGOUT
//
inline void composeUserLogout(
	BYTE objectType,
	UINT64 objectId, // PYR-29549
	const char* userId,
	INT32 logoutType,
	CommMsgBody& request,
	UINT64 appSessionId = 0  // PYR-38296
	)
{
	request
		.composeBYTE(objectType)
		.composeUINT64(objectId) // PYR-29549
		.composeString(userId)
		.composeINT32(logoutType)
		.composeUINT64(appSessionId)
		;

	// Note: do not add new field. Consult LingZ first
}

 // PYR-37210
inline void composeUserLogoutWithClientType(
	BYTE objectType,
	UINT64 objectId, // PYR-29549
	const char* userId,
	INT32 logoutType,
	UINT32 clientType,
	CommMsgBody& request,
	UINT64 appSessionId  // PYR-38296
	)
{
	request
		.composeBYTE(objectType)
		.composeUINT64(objectId) // PYR-29549
		.composeString(userId)
		.composeINT32(logoutType)
		.composeUINT64(appSessionId)
		.composeUINT32(clientType) // PYR-37210
		;
}

inline void parseUserLogout(
	CommMsgParser& parser,
	  BYTE& objectType,
	  UINT64& objectId, // PYR-29549
	  const char*& userId,
	  INT32& logoutType,
	  UINT64& appSessionId, // PYR-38296
	  UINT32& clientType // PYR-37210
	  )
{
	parser
		.parseBYTE(objectType)
		.parseUINT64(objectId) // PYR-29549
		.parseString(userId)
		.parseINT32(logoutType)
		.parseUINT64(appSessionId)
		.parseUINT32(clientType);
		;

}

class LastLoginInfoComposer
{
public:
	struct AppSessionInfo	//	PYR-49316
	{
		UINT64	appSessionId;
		CommMsgBody mappedInfoBody;	// only used by webtokenserver
		UINT32 sessionFlags;		//	enum eLoginSessionFlags
		UINT32 parentAppTypeId;		//	enum AppTypeId

		AppSessionInfo()
			: appSessionId( 0 )
			, sessionFlags( 0 )
			, parentAppTypeId( 0 )
		{}
		void compose( CommMsgBody& body ) const
		{
			body.composeUINT64( appSessionId )
				.composeMsgBody( mappedInfoBody )
				.composeUINT32( sessionFlags )
				.composeUINT32( parentAppTypeId );
		}
		void parse( CommMsgParser& parser )
		{
			parser.parseUINT64( appSessionId )
				  .parseMsgBody( mappedInfoBody )
				  .parseUINT32( sessionFlags )
				  .parseUINT32( parentAppTypeId );
		}
		const char* asTraceString( PString& _buf ) const
		{
			_buf.cut(0);
			_buf.append("appSId=").appendUint64(appSessionId);
			_buf.append(",sF=").appendHexInt(sessionFlags);
			_buf.append(",pATId=").appendUint(parentAppTypeId);
			return _buf.c_str();
		}
	};
	// PYR-99719 - Before using this, make sure the resulting MsgBody will be composed as a body into a message and nothing is added to it outside this method.
	static void composeLastLogoutInfo(const SrvTime& lastLogout,
		const UINT64 lastLoginId,
		const INT32 lastLogoutReason,
		const PSiteMask& visitedMaskEx,
		const UINT64 currentLoginId,	//	PYR-23221
		const char* lastLogoutIp, // PYR-31913
		const AppSessionInfo& appSessionInfo,	//	PYR-38296 + PYR-49316
		CommMsgBody& body)
	{
			CommMsgBody appSessionBody;
			appSessionInfo.compose( appSessionBody );

			body
				.composeSrvTime(lastLogout)
				.composeUINT64(lastLoginId)
				.composeINT32(lastLogoutReason)
				.composeUINT32(visitedMaskEx.getLower32())
				.composeUINT64(currentLoginId)
				.composeString(lastLogoutIp)
				.composeMsgBody(appSessionBody)
				;
		visitedMaskEx.compose(body);
	}
	static void parseLastLogoutInfo(const CommMsgBody& body,
									SrvTime& lastLogout, 
									UINT64& lastLoginId, 
									INT32& lastLogoutReason, 
									UINT32& visitedMask,
									UINT64& currentLoginId,		//	PYR-23221
									PString& lastLogoutIp, // PYR-31913 
									CommMsgBody& appSessionBody // PYR-38296
									)
	{
		CommMsgParser parser(body);
		parser
			.parseSrvTime(lastLogout)
			.parseUINT64(lastLoginId)
			.parseINT32(lastLogoutReason)
			.parseUINT32(visitedMask)
			.parseUINT64(currentLoginId)
			.parseStringP(lastLogoutIp)
			.parseMsgBody(appSessionBody)
			;
	}
	//	PYR-49316
	// PYr-99719 - deprecated, use the overloaded method below.
	static void parseLastLogoutInfo(const CommMsgBody& body, 
		SrvTime& lastLogout, 
		UINT64& lastLoginId, 
		INT32& lastLogoutReason, 
		UINT32& visitedMask,
		UINT64& currentLoginId,
		PString& lastLogoutIp,
		AppSessionInfo& appSessionInfo
		)
	{
		CommMsgBody appSessionBody;
		parseLastLogoutInfo( body, lastLogout, lastLoginId, lastLogoutReason, visitedMask, currentLoginId, lastLogoutIp, appSessionBody );
		CommMsgParser appSessionParser( appSessionBody );
		appSessionInfo.parse( appSessionParser );
	}
	// PYR-99719 - use this method to make use of PSiteMask visited mask.
	static void parseLastLogoutInfo(const CommMsgBody& body,
		SrvTime& lastLogout,
		UINT64& lastLoginId,
		INT32& lastLogoutReason,
		PSiteMask& visitedMaskEx,
		UINT64& currentLoginId,
		PString& lastLogoutIp,
		AppSessionInfo& appSessionInfo
	)
	{
		UINT32 visitedLegacy;
		CommMsgBody appSessionBody;
		CommMsgParser parser(body);
		parser
			.parseSrvTime(lastLogout)
			.parseUINT64(lastLoginId)
			.parseINT32(lastLogoutReason)
			.parseUINT32(visitedLegacy)
			.parseUINT64(currentLoginId)
			.parseStringP(lastLogoutIp)
			.parseMsgBody(appSessionBody)
			;
		CommMsgParser appSessionParser(appSessionBody);
		appSessionInfo.parse(appSessionParser);
		if (!parser.parseEnded())
			visitedMaskEx.parse(parser);
		else
		{
			PTRACE5("parseLastLogoutInfo old client"); // TODO: change to FL when 2019.main.05 deployed to NJ
			visitedMaskEx = PSiteMask::fromUINT64(visitedLegacy); // approved usage of PSiteMask::fromUINT64()
		}
	}
	static void parseAppSessionIdOnly(const CommMsgBody& body, UINT64& appSessionId)
	{
		SrvTime lastLogout; 
		UINT64 lastLoginId; 
		INT32 lastLogoutReason; 
		UINT32 visitedMask;
		UINT64 currentLoginId;
		PString lastLogoutIp;
		CommMsgBody appSessionBody;
		CommMsgParser parser(body);
		parser
			.parseSrvTime(lastLogout)
			.parseUINT64(lastLoginId)
			.parseINT32(lastLogoutReason)
			.parseUINT32(visitedMask)
			.parseUINT64(currentLoginId)
			.parseStringP(lastLogoutIp)
			.parseMsgBody(appSessionBody)
			;
		CommMsgParser appSessionParser(appSessionBody);
		appSessionParser.parseUINT64(appSessionId);
	}
};


#endif // msguserauth_h_included
