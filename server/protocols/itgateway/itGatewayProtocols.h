/**
 * itGatewayProtocols.h
 *
 * This file was auto-generated from itGatewayProtocols.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: itGatewayProtocols.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_ITGATEWAYPROTOCOLS_H
#define ATF_PROTOCOL_DEFFILE_ITGATEWAYPROTOCOLS_H

#include "UserDefStructs.h"
#include "aamsgateway.h"


namespace ItGateway
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
		static bool validateProtocolMessageFromJSONString(const char *_nameSpace, UINT32 _msgId, const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace SwModules
	{
		enum eProductCategory
		{
			eProductCategory_Undefined = 0,
			eProductCategory_Poker = 1,
			eProductCategory_Casino = 2,
		};

		enum eReportStatus
		{
			eReportStatus_Undefined = 0,
			eReportStatus_NoError = 1,
			eReportStatus_ErrorReceived = 2,
			eReportStatus_NotSent = 3,
		};

		struct Protocol_IT_Q_GET_SW_MODULES : public Atf::MessageProtocol
		{
			// empty

			Protocol_IT_Q_GET_SW_MODULES();
			void clear() override;
			bool equals(const Protocol_IT_Q_GET_SW_MODULES&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return IT_Q_GET_SW_MODULES; }
			const char *getMsgIdString() const override { return "IT_Q_GET_SW_MODULES"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Module
		{
			// M(ss) - M(sha1,filename)
			PString sha1;
			PString filename;

			Module();
			void clear();
			bool equals(const Module&) const;
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

		struct SwModule
		{
			// M(4b44s4444<M(ss)>) - M(productCategory,active,code,type,name,version,subVersion,lastReportStatus,numModules<M(sha1,filename)>)
			UINT32 productCategory;	// enum eProductCategory
			bool active;
			UINT32 code;
			UINT32 type;
			PString name;
			UINT32 version;
			UINT32 subVersion;
			UINT32 lastReportStatus;	// enum eReportStatus
			Atf::LAtfVector< Module, 4 > modules;

			SwModule();
			void clear();
			bool equals(const SwModule&) const;
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

		struct Protocol_IT_A_GET_SW_MODULES : public Atf::MessageProtocol
		{
			// 2[s|4<M(4b44s4444<M(ss)>)>] - errCode[errString|numSwModules<M(productCategory,active,code,type,name,version,subVersion,lastReportStatus,numModules<M(sha1,filename)>)>]
			INT16 errCode;

			PString errString;

			Atf::LAtfVector< SwModule, 4 > swModules;

			Protocol_IT_A_GET_SW_MODULES();
			void clear() override;
			bool equals(const Protocol_IT_A_GET_SW_MODULES&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return IT_A_GET_SW_MODULES; }
			const char *getMsgIdString() const override { return "IT_A_GET_SW_MODULES"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct ModuleVersion
		{
			// M(4444s) - M(type,code,newVersion,newSubVersion,adminComment)
			UINT32 type;
			UINT32 code;
			UINT32 newVersion;
			UINT32 newSubVersion;
			PString adminComment;

			ModuleVersion();
			void clear();
			bool equals(const ModuleVersion&) const;
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

		struct Protocol_IT_Q_UPDATE_SW_MODULES_VER : public Atf::MessageProtocol
		{
			// 4<M(4444s)> - numModulesToUpdate<M(type,code,newVersion,newSubVersion,adminComment)>
			Atf::LAtfVector< ModuleVersion, 4 > modulesToUpdate;

			Protocol_IT_Q_UPDATE_SW_MODULES_VER();
			void clear() override;
			bool equals(const Protocol_IT_Q_UPDATE_SW_MODULES_VER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return IT_Q_UPDATE_SW_MODULES_VER; }
			const char *getMsgIdString() const override { return "IT_Q_UPDATE_SW_MODULES_VER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_IT_A_UPDATE_SW_MODULES_VER : public Atf::MessageProtocol
		{
			// 2[s] - errCode[aggregatedErrorDescr]
			INT16 errCode;

			PString aggregatedErrorDescr;

			Protocol_IT_A_UPDATE_SW_MODULES_VER();
			void clear() override;
			bool equals(const Protocol_IT_A_UPDATE_SW_MODULES_VER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return IT_A_UPDATE_SW_MODULES_VER; }
			const char *getMsgIdString() const override { return "IT_A_UPDATE_SW_MODULES_VER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};
	};
};

#endif
