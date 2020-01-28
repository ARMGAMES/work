// DBM messages related to the login process
#ifndef MsgDbmLogin_h_included
#define MsgDbmLogin_h_included

#include "commmsgbody.h"
#include "pyrtime.h"
#include "dbm.h"
#include "MsgCommon.h"
#include "msguserauth.h"
#include "cashtypes.h"
#include "i18n.localedef.h"

// auth request message wrapper
class MsgDbmQUserAuth
{
public:
	const char* userId;
	const char* macAddr;
	const char* volumeLabel;
	const char* installId;
	const char* ipAddr;
	INT32 port;
	BYTE objectType;
	UINT64 objectId; // PYR-29549
	CommMsgBody bodyIn; // for incoming messages
	const CommMsgBody& bodyInRef; // for outgoing messages
	UINT32 siteId;
	UINT32 platformId;
	const char* userExtraId;
	const char* guardName;
	UINT32 clubId;
	UINT32 objectFlags;
	UINT32 clientExtraInfoMask;
	UINT32 clientType;		// PYR-24582
	UINT32 clientDevice;	// PYR-24582
	UINT32 clientVersion;	// PYR-25260
	UINT32 minClientVersion;// PYR-25260 
	const char* routerMacAddr; // PYR-29604
	UINT32 clientBrand; //PYR-37793
	UINT32 osId;				// PYR-38296
	UINT32 appTypeId;			// PYR-38296
	UINT32 productId;			// PYR-38296
	const char* pwdHash;
	UINT64 parentAppSessionId; // PYR-38296
	const char* installId2;	//	PYR-51188

	// constructor for an incoming message
	MsgDbmQUserAuth( CommMsgParser& parser )
	: bodyInRef( bodyIn )
	{
		parseUserAuthMsg(
			parser,
			userId,
			macAddr,
			volumeLabel,
			installId,
			ipAddr,
			port,
			objectType,
			objectId,
			bodyIn,
			siteId,
			platformId,
			userExtraId,
			guardName,
			"",
			clubId,
			objectFlags,
			clientExtraInfoMask,
			clientType,
			clientDevice,
			clientVersion,
			minClientVersion,
			routerMacAddr,
			clientBrand,
			osId,
			appTypeId,
			productId,
			pwdHash,
			parentAppSessionId,
			installId2);
	}

	// constructor for outgoing RSA request
	MsgDbmQUserAuth(
		const char* _userId,
		const char* _macAddr,	
		const char* _volumeLabel,
		const char* _installId,
		const char* _ipAddr,
		INT32 _port,
		BYTE _objectType,
		UINT64 _objectId, // PYR-29549
		const CommMsgBody& _bodyIn,
		UINT32 _siteId,
		UINT32 _platformId,
		const char* _userExtraId,
		const char* _guardName,
		UINT32 _clubId = 0,
		UINT32 _objectFlags = 0,
		UINT32 _clientExtraInfoMask = 0,
		UINT32 _clientType = 0,
		UINT32 _clientDevice = 0,
		UINT32 _clientVersion = 0,
		UINT32 _minClientVersion = 0,
		const char* _routerMacAddr = "", // PYR-29604
		UINT32 _clientBrand = BrandType_PokerStars, // PYR-37793
		UINT32 _osId = OSId_Unknown,
		UINT32 _appTypeId = AppTypeId_Unknown,
		UINT32 _productId = ProductId_Unknown,
		const char* _pwdHash = "",
		UINT64 _parentAppSessionId = 0,  // PYR-38296
		const char* _installId2 = ""	// PYR-51188
		)
	: bodyInRef(_bodyIn)
	{
		userId = _userId;	
		macAddr = _macAddr;
		volumeLabel = _volumeLabel;
		installId = _installId;
		ipAddr = _ipAddr;
		port = _port;
		objectType = _objectType;
		objectId = _objectId;
		siteId = _siteId;
		platformId = _platformId;
		userExtraId = _userExtraId;
		guardName = _guardName;
		clubId = _clubId;
		objectFlags = _objectFlags;
		clientExtraInfoMask = _clientExtraInfoMask;
		clientType = _clientType;
		clientDevice = _clientDevice;
		clientVersion = _clientVersion;
		minClientVersion = _minClientVersion;
		routerMacAddr = _routerMacAddr;
		clientBrand = _clientBrand;
		osId = _osId;
		appTypeId = _appTypeId;
		productId = _productId;
		pwdHash = _pwdHash;
		parentAppSessionId = _parentAppSessionId;
		installId2 = _installId2;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{ 
		composeUserAuthMsg(
			userId,
			macAddr,
			volumeLabel,
			installId,
			ipAddr,
			port,
			objectType,
			objectId,
			bodyInRef,
			siteId,
			platformId,
			guardName,
			msg,
			userExtraId,
			clubId,
			objectFlags,
			clientExtraInfoMask,
			clientType,
			clientDevice,
			clientVersion,
			minClientVersion,
			routerMacAddr,
			clientBrand,
			osId,
			appTypeId,
			productId,
			pwdHash,
			parentAppSessionId,
			installId2);
		return msg;
	}
};

// response to DBM auth request 
class MsgDbmAUserAuth
{
public:
	INT16 errCode;
	const char* errDescr;	
	CommMsgBody authBody;
	UINT32 privileges;
	const char* city;
	UINT32 flags;
	const char* email;
	UINT32 userIntId;
	const char* country;
	const char* state;
	CommMsgBody dummyBody;
	std::vector<const char*> rights;
	PaysystemMask depositMask;
	UINT32 ltFpp;
	UINT32 vipStatus;
	UINT32 locale;
	UINT32 fpp;
	UINT32 vipCounter1;
	UINT32 vipCounter2;
	UINT32 vipCounter3;
	UINT64 privileges2;
	UINT32 emLocale;
	UINT32 timezone;
	SrvTime gracePeriodUntil_dummy;
	UINT64 flags2;
	UINT32 originalUserIntId;
	SrvTime lastLogin;
	const char* countryByIp;
	const char* stateByIp;
	vector<PString> rights_;
	UINT32 licenseId;
	const char* fiscalCode;
	CommMsgBody bodyLastLoginInfo; // PYR-20837
	const char* promoImageCode;
	UINT32 authFlags; // PYR-24506, enum eAuthResponseFlags
	const char* frArjelUserHash; // PYR-23674
	UINT64 privileges3; // PYR-30588
	const char* userId;
	const char* sex;	// PYR-33499
	UINT32 imageId;		//	PYR-35000
	UINT64 flags3;      // PYR-36189
	INT8 vipCounter1CentsOnly; // PYR-40766
	INT8 vipCounter2CentsOnly; // PYR-40766
	SrvTime registered; // PYR-36446
	INT32 targetRewardPoints;	//	PYR-51670
	INT32 pokerRPReward;		//	PYR-51670
	const char* defaultCurrency;
	INT32 loginAuthMethod; // enum eLoginAuthMethod // PYR-80352

	// constructor for an incoming message
	MsgDbmAUserAuth( CommMsgParser& parser )
		: MsgDbmAUserAuth()
	{
		parse( parser );
	}

	void parse( CommMsgParser& parser )
	{
		parser.parseINT16(errCode);
		if( errCode != DBM_NO_ERROR )
		{
			parser
				.parseString(errDescr)
				.parseMsgBody(authBody) // even if NO_SUCH_USER it will be parsable
				.parseUINT32(emLocale);
		}
		else
		{
			UINT16 numRights;
			UINT32 depositMask32;
			UINT64 depositMask64;
			parser
				.parseMsgBody(authBody)
				.parseUINT32(privileges)
				.parseString(city)
				.parseUINT32(flags)
				.parseString(email)
				.parseUINT32(userIntId)
				.parseString(country)
				.parseString(state)
				.parseMsgBody(dummyBody)
				.parseUINT16(numRights);
			rights.reserve(numRights);
			for(UINT32 i = 0; i < numRights; ++i)
			{
				const char* right;
				parser.parseString(right);
				rights_.push_back(right);
			}
			parser
				.parseUINT32(depositMask32) // obsolete
				.parseUINT32(ltFpp)
				.parseUINT32(vipStatus)
				.parseUINT32(locale)
				.parseUINT32(fpp)
				.parseUINT32(vipCounter1)
				.parseUINT32(vipCounter2)
				.parseUINT64(privileges2)
				.parseUINT32(emLocale)
				.parseUINT32(timezone)
				.parseSrvTime(gracePeriodUntil_dummy)
				.parseUINT64(flags2)
				.parseUINT32(originalUserIntId)
				.parseUINT64(depositMask64) // #17229: obsolete
				.parseSrvTime(lastLogin)
				.parseString(countryByIp)
				.parseString(stateByIp);
			depositMask.parse(parser); // #17229
			parser
				.parseUINT32(vipCounter3)
				.parseUINT32(licenseId)
				.parseString(fiscalCode)
				.parseMsgBody(bodyLastLoginInfo)
				.parseString(promoImageCode)
				.parseUINT32(authFlags)
				.parseString(frArjelUserHash)
				.parseUINT64(privileges3)
				.parseString(userId)
				.parseString(sex)
				.parseUINT32(imageId)
				.parseUINT64(flags3) // PYR-36189
				.parseINT8(vipCounter1CentsOnly) // PYR-40766
				.parseINT8(vipCounter2CentsOnly) // PYR-40766
				.parseSrvTime(registered) // PYR-36446
				.parseINT32(targetRewardPoints)	//	PYR-51670
				.parseINT32(pokerRPReward)		//	PYR-51670
				.parseString(defaultCurrency)
				.parseINT32(loginAuthMethod)	//	PYR-80352
				;

			// To keep it consistent with MsgDbmAUserAuth::rights type.
			rights.reserve(rights_.size());
			for(UINT32 i = 0; i < rights_.size(); ++i)
			{
				rights.push_back(rights_[i]);
			}
		}
	}

	// default constructor 
	MsgDbmAUserAuth() :
		  errCode(0)
		, errDescr("")
		, privileges(0)
		, city("")
		, flags(0)
		, email("")
		, userIntId(0)
		, country("")
		, state("")
		, ltFpp(0)
		, vipStatus(0)
		, locale(LOCALE_DEFAULT)
		, fpp(0)
		, vipCounter1(0)
		, vipCounter2(0)
		, vipCounter3(0)
		, privileges2(0)
		, emLocale(0)
		, timezone(0)
		, flags2(0)
		, originalUserIntId(0)
		, countryByIp("")
		, stateByIp("")
		, licenseId(0)
		, fiscalCode("")
		, promoImageCode("")
		, authFlags(0) // PYR-24506
		, frArjelUserHash("") // PYR-23674
		, privileges3(0) // PYR-30588
		, userId("")
		, sex("") // PYR-33499
		, imageId(0) //	PYR-35000
		, flags3(0) // PYR-36189
		, vipCounter1CentsOnly(0) // PYR-40766
		, vipCounter2CentsOnly(0) // PYR-40766
		, targetRewardPoints(0)	  //	PYR-51670
		, pokerRPReward(0)		  //	PYR-51670
		, defaultCurrency("")
		, loginAuthMethod(eAuthUnknown) // PYR-80352
	{}

	// constructor for error message
	MsgDbmAUserAuth( UINT16 _resultCode, const char* _errStr )
		: MsgDbmAUserAuth()
	{
		errCode = _resultCode;
		errDescr = _errStr;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{ 
		msg.composeINT16(errCode);
		if( errCode != DBM_NO_ERROR )
		{
			composeUserAuthFailedReplyMsg(
				errDescr,
				authBody,
				locale,
				msg);
		}
		else
		{
			// To keep it consistent with MsgDbmAUserAuth::rights type.
			vector<PString> rights_;
			rights_.reserve(rights.size());
			for(int n = 0; n < rights.size(); n++)
				rights_.push_back(rights[n]);

			composeUserAuthOkReplyMsg(
				authBody,
				privileges,
				city,
				flags,
				email,
				userIntId,
				country,
				state,
				dummyBody,
				rights_,
				depositMask,
				ltFpp,
				vipStatus,
				locale,
				fpp,
				vipCounter1,
				vipCounter2,
				vipCounter3,
				privileges2,
				emLocale,
				timezone,
				flags2,
				originalUserIntId,
				lastLogin,
				countryByIp,
				stateByIp,
				licenseId,
				fiscalCode,
				bodyLastLoginInfo, // PYR-20837
				promoImageCode,
				authFlags,
				frArjelUserHash,
				privileges3, // PYR-30588
				userId, // PYR-32891
				sex,	// PYR-33499
				imageId,	//	PYR-35000
				flags3, // PYR-36189
				vipCounter1CentsOnly // PYR-40766
				,vipCounter2CentsOnly // PYR-40766
				,registered // PYR-36446
				,targetRewardPoints	//	PYR-51670
				,pokerRPReward		//	PYR-51670
				,defaultCurrency
				,loginAuthMethod // PYR-80352
				,msg
				);
		}
		return msg;
	}
};

// response to table auth request
class MsgDbmAUserTableAuth
{
public:
	INT16 errCode;
	const char* errDescr;	
	CommMsgBody authBody;
	UINT32 privileges;
	const char* city;
	UINT32 flags;
	const char* email;
	UINT32 fpp;
	UINT32 lt_fpp;
	UINT32 chips;
	UINT32 tchips;
	UINT32 wchips;
	UINT64 flags2;
	UINT32 originalUserIntId;
	const char* country;
	const char* state;
	const char* promoImageCode;
	const char* countryByIp;
	const char* stateByIp;
	UINT32 licenseId;
	UINT64 privileges2;
	UINT32 authFlags; // PYR-24802
	const char* frArjelUserHash; // PYR-23674
	// PYR-32153
	SrvTime firstDeposit;
	SrvTime registered;
	INT32 monthlyVpp;
	INT32 yearlyVpp;
	INT64 playChips;
	vector<PString> trackCodes;
	const char* userId;
	UINT64 appSessionId; // PYR-38296
	INT32 targetRewardPoints;	//	PYR-51670
	INT32 pokerRPReward;		//	PYR-51670
	UINT64 privileges3;			//	PYR-79935

public:

	// constructor for an incoming message
	MsgDbmAUserTableAuth( CommMsgParser& parser )
	{
		parseUserTableAuthReplyMsg(
			parser,
			errCode,
			errDescr,
			authBody,
			privileges,
			city,
			flags,
			email,
			fpp,
			lt_fpp,
			chips,
			tchips,
			wchips,
			flags2,
			originalUserIntId,
			country,
			state,
			promoImageCode,
			countryByIp,
			stateByIp,
			licenseId,
			privileges2,
			authFlags, // PYR-24802
			frArjelUserHash,
			// PYR-32153
			firstDeposit,
			registered,
			monthlyVpp,
			yearlyVpp,
			playChips,
			trackCodes,
			userId,
			appSessionId,
			targetRewardPoints,
			pokerRPReward,
			privileges3
			);
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeINT16(errCode);
		if( errCode != DBM_NO_ERROR )
		{
			composeUserAuthFailedReplyMsg(
				errDescr,
				authBody,
				0, // LOCALE_DEFAULT
				msg);
		}
		else
		{
			composeUserTableAuthOkReplyMsg(
				authBody,
				privileges,
				city,
				flags,
				email,
				fpp,
				lt_fpp,
				chips,
				tchips,
				wchips,
				flags2,
				originalUserIntId,
				country,
				state,
				promoImageCode,
				countryByIp,
				stateByIp,
				licenseId,
				privileges2,
				authFlags,
				frArjelUserHash,
				// PYR-32153
				firstDeposit,
				registered,
				monthlyVpp,
				yearlyVpp,
				playChips,
				trackCodes,
				// end of PYR-32153
				userId,
				appSessionId,
				targetRewardPoints,
				pokerRPReward,
				privileges3,
				msg
				);
		}
		return msg;
	}
};

// auth request message wrapper
class MsgDbmQUserLogout
{
public:
	BYTE objectType;
	UINT64 objectId; // PYR-29549
	const char* userId;
	INT32 logoutType;
	UINT64 appSessionId;
	UINT32 clientType;

	// constructor for an incoming message
	MsgDbmQUserLogout( CommMsgParser& parser )
	{
		parseUserLogout(
			parser,
			objectType,
			objectId,
			userId,
			logoutType,
			appSessionId,
			clientType);
	}

	// constructor for outgoing RSA request
	MsgDbmQUserLogout(
		BYTE _objectType,
		UINT64 _objectId, // PYR-29549
		const char* _userId,
		INT32 _logoutType,
		UINT64 _appSessionId = 0,
		UINT32 _clientType = ClientType_Unknown)
	{
		objectType = _objectType;
		objectId = _objectId;
		userId = _userId;
		logoutType = _logoutType;
		appSessionId = _appSessionId;
		clientType = _clientType;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{ 
		composeUserLogoutWithClientType(
			objectType,
			objectId,
			userId,
			logoutType,
			clientType,
			msg,
			appSessionId
			);

		return msg;
	}
};

// auth request message wrapper
class MsgDbmQLoginConfirm
{
public:
	const char* userId;	
	UINT32 loginAuthMethod;
	UINT64 appSessionId;

	// constructor for an incoming message
	MsgDbmQLoginConfirm( CommMsgParser& parser )
	{
		parser
			.parseString(userId)
			.parseUINT32(loginAuthMethod)
			.parseUINT64(appSessionId);
	}

	// constructor for outgoing login confirm request
	MsgDbmQLoginConfirm( const char* _userId, UINT32 _loginAuthMethod = eAuthUnknown, UINT64 _appSessionId = 0  )
	{
		userId = _userId;	
		loginAuthMethod = _loginAuthMethod;
		appSessionId = _appSessionId;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{ 
		return msg
			.composeString(userId)
			.composeUINT32(loginAuthMethod)
			.composeUINT64(appSessionId);
	}
};

class MsgDBMALoginConfirm
{
public:
	INT16 errCode;
	const char* errDescr;
	const char* userId;
	UINT32 flags;
	UINT32 effectivePrivs;
	UINT64 effectivePrivs2;
	const char* defaultCurrency;
	UINT32 chips;
	UINT32 playChips;
	UINT32 tChips;
	UINT32 wChips;
	UINT32 owedChips;
	UINT64 flags2;
	CommMsgBody licenseMsgBody;
	UINT64 privileges3;
	UINT64 correctedUserFlags3;

	MsgDBMALoginConfirm( CommMsgParser& parser )
	{
		parse( parser );
	}

	void parse( CommMsgParser& parser )
	{
		parser.parseINT16( errCode );
		if( errCode != DBM_NO_ERROR )
		{
			parser
				.parseString( errDescr );
		}
		else
		{
			UINT32 bannedUsersNumber;

			parser.parseString( userId )
					.parseUINT32( flags )
					.parseUINT32( effectivePrivs)
					.parseUINT32( bannedUsersNumber ) // banned users number
					.parseUINT64( effectivePrivs2)
					.parseString( defaultCurrency )
					.parseUINT32( chips )
					.parseUINT32( playChips )
					.parseUINT32( tChips )
					.parseUINT32( wChips )
					.parseUINT32( owedChips )
					.parseUINT64( flags2 )  // [AO 2009-03-06] #10684
					.parseMsgBody( licenseMsgBody ) // PYR-31446
					.parseUINT64( privileges3 )  // PYR-27034
					.parseUINT64( correctedUserFlags3 ) // PYR-36189
					;
		}
	}

	// constructor for error message
	MsgDBMALoginConfirm( UINT16 _resultCode, const char* _errStr )   //-V730 
	{
		errCode = _resultCode;
		errDescr = _errStr;
	} //lint !e1401 #PYR-41234 (many fields are not initialized by ctor), ancient code

	MsgDBMALoginConfirm()
	:
	errCode( 0 )
	, errDescr( "" )
	, userId( "" )
	, flags( 0 )
	, effectivePrivs( 0 )
	, effectivePrivs2( 0 )
	, defaultCurrency( 0 )
	, chips( 0 )
	, playChips( 0 )
	, tChips( 0 )
	, wChips( 0 )
	, owedChips( 0 )
	, flags2( 0 )
	, privileges3( 0 )
	, correctedUserFlags3( 0 )
	{

	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		return msg
			.composeString( userId )
			.composeUINT32( flags)
			.composeUINT32( effectivePrivs)
			.composeUINT32( 0 ) // banned users number
			.composeUINT64( effectivePrivs2)
			.composeString( defaultCurrency )
			.composeUINT32( chips )
			.composeUINT32( playChips )
			.composeUINT32( tChips )
			.composeUINT32( wChips )
			.composeUINT32( owedChips )
			.composeUINT64( flags2 )  // [AO 2009-03-06] #10684
			.composeMsgBody( licenseMsgBody ) // PYR-31446
			.composeUINT64( privileges3 )  // PYR-27034
			.composeUINT64( correctedUserFlags3 ) // PYR-36189
			;
	}
	
};

#endif
