/**
 * ChipsDbm_atf.h
 *
 * This file was auto-generated from ChipsDbm_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: ChipsDbm_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_CHIPSDBM_ATF_H
#define ATF_PROTOCOL_DEFFILE_CHIPSDBM_ATF_H

#include "pplib.h"
#include "chipsdbm.h"


#include "UserDefStructs.h"

namespace ChipsDbm
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
		static bool validateProtocolMessageFromJSONString(const char *_nameSpace, UINT32 _msgId, const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace cli_mobile
	{
		struct Protocol_CHIPSDBM_Q_APPLE_STORE_PURCHASE : public Atf::MessageProtocol
		{
			// sb - receipt,useNewPaywall
			PString receipt;
			bool useNewPaywall;

			Protocol_CHIPSDBM_Q_APPLE_STORE_PURCHASE();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_Q_APPLE_STORE_PURCHASE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_Q_APPLE_STORE_PURCHASE; }
			const char *getMsgIdString() const override { return "CHIPSDBM_Q_APPLE_STORE_PURCHASE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_CHIPSDBM_A_APPLE_STORE_PURCHASE : public Atf::MessageProtocol
		{
			// 2[s|8] - errCode[errDescr|purchaseToken]
			INT16 errCode;

			PString errDescr;

			UINT64 purchaseToken;

			Protocol_CHIPSDBM_A_APPLE_STORE_PURCHASE();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_A_APPLE_STORE_PURCHASE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_A_APPLE_STORE_PURCHASE; }
			const char *getMsgIdString() const override { return "CHIPSDBM_A_APPLE_STORE_PURCHASE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_CHIPSDBM_Q_GOOGLE_PLAY_PURCHASE : public Atf::MessageProtocol
		{
			// sssb - notUsedStr,orderJson,signature,useNewPaywall
			PString notUsedStr;
			PString orderJson;
			PString signature;
			bool useNewPaywall;

			Protocol_CHIPSDBM_Q_GOOGLE_PLAY_PURCHASE();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_Q_GOOGLE_PLAY_PURCHASE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_Q_GOOGLE_PLAY_PURCHASE; }
			const char *getMsgIdString() const override { return "CHIPSDBM_Q_GOOGLE_PLAY_PURCHASE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_CHIPSDBM_A_GOOGLE_PLAY_PURCHASE : public Atf::MessageProtocol
		{
			// 2[s|8] - errCode[errDescr|purchaseToken]
			INT16 errCode;

			PString errDescr;

			UINT64 purchaseToken;

			Protocol_CHIPSDBM_A_GOOGLE_PLAY_PURCHASE();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_A_GOOGLE_PLAY_PURCHASE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_A_GOOGLE_PLAY_PURCHASE; }
			const char *getMsgIdString() const override { return "CHIPSDBM_A_GOOGLE_PLAY_PURCHASE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_CHIPSDBM_Q_GET_CHIPS_TRANS : public Atf::MessageProtocol
		{
			// 4 - purchaseId
			UINT32 purchaseId;	// Represents id, no need to limit

			Protocol_CHIPSDBM_Q_GET_CHIPS_TRANS();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_Q_GET_CHIPS_TRANS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_Q_GET_CHIPS_TRANS; }
			const char *getMsgIdString() const override { return "CHIPSDBM_Q_GET_CHIPS_TRANS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_CHIPSDBM_A_GET_CHIPS_TRANS : public Atf::MessageProtocol
		{
			// 2[s|4488ssssstts84424242] - errCode[errDescr|purchaseId,parentId,amount,playChipsAmount,gateway,status,errCodeChipsInfo,errInfo,extRefId,started,completed,userId,auditId,userIntId,paywallId,paywallVersionId,segmentId,segmentVersionId,bundleId,bundleVersionId]
			INT16 errCode;

			PString errDescr;

			UINT32 purchaseId;
			UINT32 parentId;
			UINT64 amount;
			UINT64 playChipsAmount;
			PString gateway;
			PString status;
			PString errCodeChipsInfo;
			PString errInfo;
			PString extRefId;
			CommSrvTime started;
			CommSrvTime completed;
			PString userId;
			UINT64 auditId;
			UINT32 userIntId;
			UINT32 paywallId;
			INT16 paywallVersionId;
			UINT32 segmentId;
			INT16 segmentVersionId;
			UINT32 bundleId;
			INT16 bundleVersionId;

			Protocol_CHIPSDBM_A_GET_CHIPS_TRANS();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_A_GET_CHIPS_TRANS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_A_GET_CHIPS_TRANS; }
			const char *getMsgIdString() const override { return "CHIPSDBM_A_GET_CHIPS_TRANS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_CHIPSDBM_Q_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG : public Atf::MessageProtocol
		{
			// 4 - storeId
			UINT32 storeId;	// Represents id, no need to limit

			Protocol_CHIPSDBM_Q_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_Q_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_Q_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG; }
			const char *getMsgIdString() const override { return "CHIPSDBM_Q_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct PlayChipsConfigStore
		{
			// M(44s) - M(configId,storeId,productId)
			UINT32 configId;
			UINT32 storeId;
			PString productId;

			PlayChipsConfigStore();
			void clear();
			bool equals(const PlayChipsConfigStore&) const;
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
		struct PlayChipsConfig
		{
			// M(4884t4t4424<M(44s)>) - M(configId,amount,playChipsAmount,payTier,whenCreated,createdBy,whenObsoleted,obsoletedBy,platform,popular,numStoreConfigs<M(configId,storeId,productId)>)
			UINT32 configId;
			INT64 amount;
			INT64 playChipsAmount;
			UINT32 payTier;
			CommSrvTime whenCreated;
			UINT32 createdBy;
			CommSrvTime whenObsoleted;
			UINT32 obsoletedBy;
			UINT32 platform;
			UINT16 popular;
			Atf::LAtfVector< PlayChipsConfigStore, 4 > storeConfigs;

			PlayChipsConfig();
			void clear();
			bool equals(const PlayChipsConfig&) const;
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
		struct Protocol_CHIPSDBM_A_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG : public Atf::MessageProtocol
		{
			// 2[s|4<M(4884t4t4424<M(44s)>)>] - errCode[errDescr|numPlayChipsConfig<M(configId,amount,playChipsAmount,payTier,whenCreated,createdBy,whenObsoleted,obsoletedBy,platform,popular,numStoreConfigs<M(configId,storeId,productId)>)>]
			INT16 errCode;

			PString errDescr;

			Atf::LAtfVector< PlayChipsConfig, 4 > playChipsConfig;

			Protocol_CHIPSDBM_A_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG();
			void clear() override;
			bool equals(const Protocol_CHIPSDBM_A_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return CHIPSDBM_A_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG; }
			const char *getMsgIdString() const override { return "CHIPSDBM_A_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};
	};
};

#endif
