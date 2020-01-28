//loginhelper.h
#ifndef loginhelper_h_included
#define loginhelper_h_included

#include "ppincludebase.h"
#include "loginerror.h"
#include "ppstring.h"
#include "clientplatforms.h"


#define OBJ_ID_CLIENT_LITE 1 // for login object type

class CommMsgBody;
class CommMsgParser;

class LoginHelper
{
public:
	struct ClientLoginExtraData
	{
		const char* macAddr;
		const char* volumeLabel;
		const char* installId;
		const char* machineGuid;
		const char* adapterGuid;
		const char* userExtraId;
		UINT32 clientSiteId;
		UINT32 clientPlatform; // enum ClientPlatform
		UINT32 clientExtraInfoMask;		//	enum ClientExtraInfoMask
		UINT32 clientType;		// PYR-24582 enum ClientType
		UINT32 clientDevice;	// PYR-24582 enum ClientDevice
		UINT32 clientVersion;	// PYR-25260
		const char* routerMacAddr; // PYR-29604
		UINT32 clientBrand; //PYR-37793
		UINT32 osId;				// PYR-38296 enum OSId
		UINT32 appTypeId;			// PYR-38296
		UINT32 productId;			// PYR-38296
		const char* installId2;		//	PYR-51188
		const char* operatorId;		// PYR-52000
		const char* osVersion;		// PYR-52518 Version of osId.

		ClientLoginExtraData() : macAddr(""),volumeLabel(""),installId(""),machineGuid(""),adapterGuid(""),userExtraId(""),
			clientSiteId(0),clientPlatform(0),clientExtraInfoMask(0),clientType(0),clientDevice(0),clientVersion(0),
			routerMacAddr(""), clientBrand(BrandType_PokerStars), osId(OSId_Unknown), appTypeId(AppTypeId_Unknown) , productId(ProductId_Unknown)
			, installId2(""), operatorId(""), osVersion("")
		{}
		const char* toString(PString& buf) const
		{
			buf.assign("").append("mac=").append(macAddr).append(",vol=").append(volumeLabel).append(",ii=").append(installId)
				.append(",mg=").append(machineGuid).append(",ag=").append(adapterGuid).append(",uei=").append(userExtraId)
				.append(",s=").appendUint(clientSiteId).append(",p=").appendUint(clientPlatform).append(",m=x").appendHexInt(clientExtraInfoMask)
				.append(",t=").appendUint(clientType).append(",d=").appendUint(clientDevice).append(",v=").appendUint(clientVersion)
				.append(",rma=").append(routerMacAddr).append(",b=").appendUint(clientBrand).append(",o=").appendUint(osId)
				.append(",a=").appendUint(appTypeId).append(",p=").appendUint(productId).append(",ii2=").append(installId2)
				.append(",oid=").append(operatorId).append(",ov=").append(osVersion);
			return buf;
		}
	};

	static const char* composePwdError( CommMsgBody& expl );
	static void appendSuccessInfo( CommMsgBody& expl, UINT64 flags2, UINT32 authFlags/*PYR-24802*/, const char* frArjelUserHash );
	static const char* composeClientBody( CommMsgBody& clientBody, UINT64 flags2, UINT32 authFlags, const char* frArjelUserHash, UINT32 userIntId, UINT64 currentLoginId, 
		const CommMsgBody& externalOperatorBody, bool composeExternAuthBody );
	static const char* composeClientBody( CommMsgBody& clientBody, UINT64 flags2, UINT32 authFlags, const char* frArjelUserHash, bool composeExternAuthBody);

	static bool isPublicError( INT16 err_code );
	static void interpretError( INT16 err_code, const char* errDescr, UINT32 location, UINT16& err_code_for_client, PString& errDescrForUser, CommMsgBody& expl );
	static void parseClientLoginExtra( const CommMsgBody& clientExtra, ClientLoginExtraData& e );
	static UINT32 getMinClientVersion( UINT32 _clientPlatform, UINT32 _minClientVersion, UINT32 _minMobileVersion );
	static void encodeTemplateCacheCatSubcat( UINT32 cat, UINT32 subCat, PString& str );
	static void decodeTemplateCacheCatSubcat( const char* str, UINT32& cat, UINT32& subCat );

	static void msgBodyEncode(const CommMsgBody& body, PString& out);
	static void msgBodyDecode(const PString& in, CommMsgBody& body);

	static void composeUserIdTypoForClient(CommMsgBody& explBody, const char* userId, UINT32 loginCredentialType, const char* email);
};

struct ClientLoginExtras2
{
	BYTE objectType;
	UINT64 objectId;

	// the rest is the copy from ClientLoginExtraData
	PString macAddr;
	PString installId;

	UINT32 clientSiteId; // enum Sites
	UINT32 clientPlatform;  // enum ClientPlatform
	UINT32 clientExtraInfoMask;	//	enum ClientExtraInfoMask
	UINT32 clientType;		// enum ClientType
	UINT32 clientDevice;	// enum ClientDevice
	UINT32 clientVersion;

	PString routerMacAddr;
	UINT32 brandId;			    // PYR-38296
	UINT32 osId;				// PYR-38296 enum OSId
	UINT32 appTypeId;			// PYR-38296
	UINT32 productId;			// PYR-38296
	UINT64 appSessionId;		// PYR-38296 set by auth ok response
	PString installId2;			//	PYR-51188
	PString volumeLabel;		//	PYR-51188
	PString userExtraId;		//	PYR-51188
	PString operatorId;			//	PYR-52518
	PString osVersion;			//	PYR-52518 Version of osId.

	explicit ClientLoginExtras2(BYTE objectType_)
		: objectType(objectType_)
		, objectId(0)
		, clientSiteId(0)
		, clientPlatform(0/*Client_UnknownPlatform*/)
		, clientExtraInfoMask(0)
		, clientType(0/*ClientType_Unknown*/)
		, clientDevice(0/*ClientType_Unknown*/)
		, clientVersion(0)
		, brandId(BrandType_PokerStars)
		, osId(OSId_Unknown)
		, appTypeId(AppTypeId_Unknown)
		, productId(ProductId_Unknown)
		, appSessionId(0)
	{}
	void init(const LoginHelper::ClientLoginExtraData& e)
	{
		macAddr				= e.macAddr;
		installId			= e.installId;
		clientSiteId		= e.clientSiteId;
		clientPlatform		= e.clientPlatform;
		clientExtraInfoMask = e.clientExtraInfoMask;
		clientType			= e.clientType;
		clientDevice		= e.clientDevice;
		clientVersion		= e.clientVersion;
		routerMacAddr		= e.routerMacAddr;
		brandId				= e.clientBrand;
		osId				= e.osId;
		appTypeId			= e.appTypeId;
		productId			= e.productId;
		installId2			= e.installId2;
		volumeLabel			= e.volumeLabel;
		userExtraId			= e.userExtraId;
		operatorId			= e.operatorId;
		osVersion			= e.osVersion;
	}
	const char* toString1(PString& buffer) const
	{
		buffer.clear();
		buffer
			.append("ins=").append(installId)
			.append(",s=").appendUint(clientSiteId)
			.append(",p=").appendUint(clientPlatform)
			.append(",t=").appendUint(clientType)
			.append(",d=").appendUint(clientDevice)
			.append(",v=").appendUint(clientVersion)
			.append(",ex=0x").appendHexInt(clientExtraInfoMask)
			.append(",b=").appendUint(brandId)
			.append(",os=").appendUint(osId)
			.append(",a=").appendUint(appTypeId)
			.append(",p=").appendUint(productId)
			.append(",ins2=").append(installId2)
			.append(",vol=").append(volumeLabel)
			.append(",ux=").append(userExtraId)
			.append(",op=").append(operatorId)
			.append(",ov=").append(osVersion)
			;
		return buffer.c_str();
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

	void composeAsBody(CommMsgBody& body) const;
	void parseAsBody(CommMsgParser& parser);

	//	PYR-51188
	void composeUserAuthMsg(
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
		UINT64 parentAppSessionId_ ) const;
};

struct AppLoginEssentials	// PYR-42031
{
	UINT64 appSessionId;
	UINT32 brandId; // enum BrandTypes

	AppLoginEssentials() : appSessionId(0), brandId(BrandType_Unknown) {}
	AppLoginEssentials(UINT64 appSessionId_, UINT32 brandId_) : appSessionId(appSessionId_), brandId(brandId_) {}
	AppLoginEssentials(CommMsgParser& parser);
//	AppLoginEssentials(const ClientLoginExtras2& e) : appSessionId(e.appSessionId), brandId(e.brandId) {}

	void init(UINT64 appSessionId_, UINT32 brandId_)
	{
		appSessionId	= appSessionId_;
		brandId			= brandId_;
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

	void composeAsBody(CommMsgBody& body) const;
	void parseAsBody(CommMsgParser& parser);
};

#endif //loginhelper_h_included
