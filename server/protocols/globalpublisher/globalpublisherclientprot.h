/**
 * globalpublisherclientprot.h
 *
 * This file was auto-generated from globalpublisherclientprot.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor globalpublisherclientprot.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_GLOBALPUBLISHERCLIENTPROT_H
#define ATF_PROTOCOL_DEFFILE_GLOBALPUBLISHERCLIENTPROT_H

#include "GlobalPublisher.h"
#include "PBitmask.h"
#include "sitemasks.h"
#include "licensemask.h"


#include "UserDefStructs.h"

namespace GlobalPublisherClient
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
		static bool validateProtocolMessageFromJSONString(const char *_nameSpace, UINT32 _msgId, const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	//---------------------------------------------------------------------------------------------
	// Admin Messages
	//---------------------------------------------------------------------------------------------

	struct Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA : public Atf::MessageProtocol	// Admin -> GlobalPublisher
	{

		Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA();
		void clear() override;
		bool equals(const Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA; }
		const char *getMsgIdString() const override { return "MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY : public Atf::MessageProtocol	// GlobalPublisher -> Admin
	{
		// 2[s] - errCode[errString]
		INT16 errCode;

		PString errString;

		Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY();
		void clear() override;
		bool equals(const Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY; }
		const char *getMsgIdString() const override { return "MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY(Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY&&);
		Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY& operator=(Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY(const Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY&) = default;
		Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY& operator=(const Protocol_MSG_GLOBALPUBLISHER_ADMIN_REREAD_INIDATA_REPLY&) = default;
#endif
#endif
	};

	//---------------------------------------------------------------------------------------------
	// GlobalPublisher Data Publications
	//---------------------------------------------------------------------------------------------

	struct BuildAndVersionPublData	// Leaf { 0, 0 }
	{
		// ss - build,version
		PString build;
		PString version;

		BuildAndVersionPublData();
		void clear();
		bool equals(const BuildAndVersionPublData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		BuildAndVersionPublData(BuildAndVersionPublData&&);
		BuildAndVersionPublData& operator=(BuildAndVersionPublData&&);
	private:
		BuildAndVersionPublData(const BuildAndVersionPublData&);  // prohibit
		BuildAndVersionPublData& operator=(const BuildAndVersionPublData&);  // prohibit
#endif
	};

	struct ServerTimePublData	// Leaf { 1, 0 }
	{
		// t8 - srvTime,encodedTime
		SrvTime srvTime;
		UINT64 encodedTime;

		ServerTimePublData();
		void clear();
		bool equals(const ServerTimePublData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ServerTimePublData(ServerTimePublData&&);
		ServerTimePublData& operator=(ServerTimePublData&&);
	private:
		ServerTimePublData(const ServerTimePublData&);  // prohibit
		ServerTimePublData& operator=(const ServerTimePublData&);  // prohibit
#endif
	};

	struct DataMiningPublData	// Leaf { 2, 0 }
	{
		// 4 - dataMining
		UINT32 dataMining;

		DataMiningPublData();
		void clear();
		bool equals(const DataMiningPublData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		DataMiningPublData(DataMiningPublData&&);
		DataMiningPublData& operator=(DataMiningPublData&&);
	private:
		DataMiningPublData(const DataMiningPublData&);  // prohibit
		DataMiningPublData& operator=(const DataMiningPublData&);  // prohibit
#endif
	};

	struct ServerConnectionInfoPublData	// Leaf { 3, n }
	{
		// ss4 - address,instance,serverStatus
		PString address;
		PString instance;
		INT32 serverStatus;

		ServerConnectionInfoPublData();
		void clear();
		bool equals(const ServerConnectionInfoPublData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ServerConnectionInfoPublData(ServerConnectionInfoPublData&&);
		ServerConnectionInfoPublData& operator=(ServerConnectionInfoPublData&&);
	private:
		ServerConnectionInfoPublData(const ServerConnectionInfoPublData&);  // prohibit
		ServerConnectionInfoPublData& operator=(const ServerConnectionInfoPublData&);  // prohibit
#endif
	};

	struct MonetData
	{
		// sv - name,value
		PString name;
		PBlock value;

		MonetData();
		void clear();
		bool equals(const MonetData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		MonetData(MonetData&&);
		MonetData& operator=(MonetData&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		MonetData(const MonetData&) = default;
		MonetData& operator=(const MonetData&) = default;
#endif
#endif
	};

	struct MonetPublData	// Leaf { 4, 0 }
	{
		// 4<sv> - numVMonetData<name,value>
		Atf::LAtfVector< MonetData, 4 > vMonetData;

		MonetPublData();
		void clear();
		bool equals(const MonetPublData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		MonetPublData(MonetPublData&&);
		MonetPublData& operator=(MonetPublData&&);
	private:
		MonetPublData(const MonetPublData&);  // prohibit
		MonetPublData& operator=(const MonetPublData&);  // prohibit
#endif
	};

	struct NVP
	{
		// M(ss) - M(name,value)
		PString name;
		PString value;

		NVP();
		void clear();
		bool equals(const NVP&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		NVP(NVP&&);
		NVP& operator=(NVP&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		NVP(const NVP&) = default;
		NVP& operator=(const NVP&) = default;
#endif
#endif
	};

	struct HelpSupportPortalPublData	// Leaf { 5, 0 }, Leaf { 5, 1 }
	{
		// 4<M(ss)> - numHelpSupportPortal<M(name,value)>
		Atf::LAtfVector< NVP, 4 > helpSupportPortal;

		HelpSupportPortalPublData();
		void clear();
		bool equals(const HelpSupportPortalPublData&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		HelpSupportPortalPublData(HelpSupportPortalPublData&&);
		HelpSupportPortalPublData& operator=(HelpSupportPortalPublData&&);
	private:
		HelpSupportPortalPublData(const HelpSupportPortalPublData&);  // prohibit
		HelpSupportPortalPublData& operator=(const HelpSupportPortalPublData&);  // prohibit
#endif
	};

	struct ATFExtendedFlag
	{
		// M(88) - M(featureMask,featureMask2)
		UINT64 featureMask;
		UINT64 featureMask2;

		ATFExtendedFlag();
		void clear();
		bool equals(const ATFExtendedFlag&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};

	struct ATFClientDeviceSpecificFeature
	{
		// M(4M(88)) - M(clientDevice,M(featureMask,featureMask2))
		UINT32 clientDevice;	//	enum ClientDevice
		ATFExtendedFlag featuresMask;

		ATFClientDeviceSpecificFeature();
		void clear();
		bool equals(const ATFClientDeviceSpecificFeature&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};

	struct ATFClientTypeSpecificFeature
	{
		// M(4M(88)4<M(4M(88))>) - M(clientType,M(featureMask,featureMask2),numClientDeviceSpecificFeatures<M(clientDevice,M(featureMask,featureMask2))>)
		UINT32 clientType;	//	enum ClientType
		ATFExtendedFlag featuresMask;
		Atf::LAtfVector< ATFClientDeviceSpecificFeature, 4 > clientDeviceSpecificFeatures;

		ATFClientTypeSpecificFeature();
		void clear();
		bool equals(const ATFClientTypeSpecificFeature&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ATFClientTypeSpecificFeature(ATFClientTypeSpecificFeature&&);
		ATFClientTypeSpecificFeature& operator=(ATFClientTypeSpecificFeature&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		ATFClientTypeSpecificFeature(const ATFClientTypeSpecificFeature&) = default;
		ATFClientTypeSpecificFeature& operator=(const ATFClientTypeSpecificFeature&) = default;
#endif
#endif
	};

	struct ATFClientPlatformSpecificFeature
	{
		// M(4M(88)4<M(4M(88)4<M(4M(88))>)>) - M(clientPlatform,M(featureMask,featureMask2),numClientTypeSpecificFeatures<M(clientType,M(featureMask,featureMask2),numClientDeviceSpecificFeatures<M(clientDevice,M(featureMask,featureMask2))>)>)
		UINT32 clientPlatform;	//	enum ClientPlatform
		ATFExtendedFlag featuresMask;
		Atf::LAtfVector< ATFClientTypeSpecificFeature, 4 > clientTypeSpecificFeatures;

		ATFClientPlatformSpecificFeature();
		void clear();
		bool equals(const ATFClientPlatformSpecificFeature&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ATFClientPlatformSpecificFeature(ATFClientPlatformSpecificFeature&&);
		ATFClientPlatformSpecificFeature& operator=(ATFClientPlatformSpecificFeature&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		ATFClientPlatformSpecificFeature(const ATFClientPlatformSpecificFeature&) = default;
		ATFClientPlatformSpecificFeature& operator=(const ATFClientPlatformSpecificFeature&) = default;
#endif
#endif
	};

	struct ATFSiteFeaturesVisibility
	{
		// M(44M(88)4<M(4M(88)4<M(4M(88)4<M(4M(88))>)>)>) - M(brandId,siteId,M(featureMask,featureMask2),numClientPlatformSpecificFeatures<M(clientPlatform,M(featureMask,featureMask2),numClientTypeSpecificFeatures<M(clientType,M(featureMask,featureMask2),numClientDeviceSpecificFeatures<M(clientDevice,M(featureMask,featureMask2))>)>)>)
		UINT32 brandId;
		UINT32 siteId;
		ATFExtendedFlag clientFeaturesMask;
		Atf::LAtfVector< ATFClientPlatformSpecificFeature, 4 > clientPlatformSpecificFeatures;

		ATFSiteFeaturesVisibility();
		void clear();
		bool equals(const ATFSiteFeaturesVisibility&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ATFSiteFeaturesVisibility(ATFSiteFeaturesVisibility&&);
		ATFSiteFeaturesVisibility& operator=(ATFSiteFeaturesVisibility&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		ATFSiteFeaturesVisibility(const ATFSiteFeaturesVisibility&) = default;
		ATFSiteFeaturesVisibility& operator=(const ATFSiteFeaturesVisibility&) = default;
#endif
#endif
	};

	struct ATFCountryException	//	PYR-29125
	{
		// M(sM(88)M(88)44m) - M(country,M(featureMask,featureMask2),M(featureMask,featureMask2),brandIdMask,siteVisibilityMask,siteVisibilityMaskEx)
		PString country;
		ATFExtendedFlag featureMaskOn;
		ATFExtendedFlag featureMaskOff;
		UINT32 brandIdMask;
		UINT32 siteVisibilityMask;
		PSiteMask siteVisibilityMaskEx;

		ATFCountryException();
		void clear();
		bool equals(const ATFCountryException&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ATFCountryException(ATFCountryException&&);
		ATFCountryException& operator=(ATFCountryException&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		ATFCountryException(const ATFCountryException&) = default;
		ATFCountryException& operator=(const ATFCountryException&) = default;
#endif
#endif
	};

	struct ATFClientFeaturesVisibilityEx	// Leaf { 5, 2 }
	{
		// 4<M(44M(88)4<M(4M(88)4<M(4M(88)4<M(4M(88))>)>)>)>4<M(sM(88)M(88)44m)> - numSiteFeaturesVisibilities<M(brandId,siteId,M(featureMask,featureMask2),numClientPlatformSpecificFeatures<M(clientPlatform,M(featureMask,featureMask2),numClientTypeSpecificFeatures<M(clientType,M(featureMask,featureMask2),numClientDeviceSpecificFeatures<M(clientDevice,M(featureMask,featureMask2))>)>)>)>,numCountryExceptions<M(country,M(featureMask,featureMask2),M(featureMask,featureMask2),brandIdMask,siteVisibilityMask,siteVisibilityMaskEx)>
		Atf::LAtfVector< ATFSiteFeaturesVisibility, 4 > siteFeaturesVisibilities;
		Atf::LAtfVector< ATFCountryException, 4 > countryExceptions;	// PYR-29125

		ATFClientFeaturesVisibilityEx();
		void clear();
		bool equals(const ATFClientFeaturesVisibilityEx&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		ATFClientFeaturesVisibilityEx(ATFClientFeaturesVisibilityEx&&);
		ATFClientFeaturesVisibilityEx& operator=(ATFClientFeaturesVisibilityEx&&);
	private:
		ATFClientFeaturesVisibilityEx(const ATFClientFeaturesVisibilityEx&);  // prohibit
		ATFClientFeaturesVisibilityEx& operator=(const ATFClientFeaturesVisibilityEx&);  // prohibit
#endif
	};

	//---------------------------------------------------------------------------------------------
	// GlobalPublisher MinClientVersion Publication
	//---------------------------------------------------------------------------------------------

	struct ClientVersionInfo
	{
		// M(444) - M(platformId,siteId,minClientVersion)
		UINT32 platformId;
		UINT32 siteId;
		UINT32 minClientVersion;

		ClientVersionInfo();
		void clear();
		bool equals(const ClientVersionInfo&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};

	struct MinClientVersionPublication	// Leaf { 0, 0 }
	{
		// 4<M(444)> - numVersions<M(platformId,siteId,minClientVersion)>
		Atf::LAtfVector< ClientVersionInfo, 4 > versions;

		MinClientVersionPublication();
		void clear();
		bool equals(const MinClientVersionPublication&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		MinClientVersionPublication(MinClientVersionPublication&&);
		MinClientVersionPublication& operator=(MinClientVersionPublication&&);
	private:
		MinClientVersionPublication(const MinClientVersionPublication&);  // prohibit
		MinClientVersionPublication& operator=(const MinClientVersionPublication&);  // prohibit
#endif
	};

	//---------------------------------------------------------------------------------------------
	// GlobalPublisher MinClientVersion Publication
	//---------------------------------------------------------------------------------------------

	struct CurrencyRate
	{
		// M(s8) - M(currencyCode,convRateM)
		PString currencyCode;
		INT64 convRateM;	// mid-market rate

		CurrencyRate();
		void clear();
		bool equals(const CurrencyRate&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CurrencyRate(CurrencyRate&&);
		CurrencyRate& operator=(CurrencyRate&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		CurrencyRate(const CurrencyRate&) = default;
		CurrencyRate& operator=(const CurrencyRate&) = default;
#endif
#endif
	};

	struct CurrencyRatePublication	// Leaf { 6, 0 }
	{
		// 4<M(s8)> - numCurrencyRates<M(currencyCode,convRateM)>
		Atf::LAtfVector< CurrencyRate, 4 > currencyRates;

		CurrencyRatePublication();
		void clear();
		bool equals(const CurrencyRatePublication&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CurrencyRatePublication(CurrencyRatePublication&&);
		CurrencyRatePublication& operator=(CurrencyRatePublication&&);
	private:
		CurrencyRatePublication(const CurrencyRatePublication&);  // prohibit
		CurrencyRatePublication& operator=(const CurrencyRatePublication&);  // prohibit
#endif
	};
};

#endif