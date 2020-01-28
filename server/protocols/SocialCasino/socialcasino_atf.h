/**
 * socialcasino_atf.h
 *
 * This file was auto-generated from socialcasino_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: socialcasino_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_SOCIALCASINO_ATF_H
#define ATF_PROTOCOL_DEFFILE_SOCIALCASINO_ATF_H

#include "SocialCasino.h"


#include "UserDefStructs.h"

namespace SocialCasino
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
		static bool validateProtocolMessageFromJSONString(const char *_nameSpace, UINT32 _msgId, const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	struct Protocol_MSG_SOCIALCASINO_DEBITPM : public Atf::MessageProtocol
	{
		// 88s8 - vendorId,gameId,scExternalTransactId,playMoneyAmount
		UINT64 vendorId;
		UINT64 gameId;
		PString scExternalTransactId;
		INT64 playMoneyAmount;

		Protocol_MSG_SOCIALCASINO_DEBITPM();
		void clear() override;
		bool equals(const Protocol_MSG_SOCIALCASINO_DEBITPM&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_SOCIALCASINO_DEBITPM; }
		const char *getMsgIdString() const override { return "MSG_SOCIALCASINO_DEBITPM"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_MSG_SOCIALCASINO_DEBITPM_REPLY : public Atf::MessageProtocol
	{
		// s - scExternalTransactId
		// implied INT16 errCode here. If not-zero (failure) then parse a String instead of below
		PString scExternalTransactId;

		Protocol_MSG_SOCIALCASINO_DEBITPM_REPLY();
		void clear() override;
		bool equals(const Protocol_MSG_SOCIALCASINO_DEBITPM_REPLY&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_SOCIALCASINO_DEBITPM_REPLY; }
		const char *getMsgIdString() const override { return "MSG_SOCIALCASINO_DEBITPM_REPLY"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_MSG_SOCIALCASINO_CREDITPM : public Atf::MessageProtocol
	{
		// 488s8s - userIntId,vendorId,gameId,scExternalTransactId,playMoneyAmount,refExternalTransactId
		UINT32 userIntId;
		UINT64 vendorId;
		UINT64 gameId;
		PString scExternalTransactId;
		INT64 playMoneyAmount;
		PString refExternalTransactId;

		Protocol_MSG_SOCIALCASINO_CREDITPM();
		void clear() override;
		bool equals(const Protocol_MSG_SOCIALCASINO_CREDITPM&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_SOCIALCASINO_CREDITPM; }
		const char *getMsgIdString() const override { return "MSG_SOCIALCASINO_CREDITPM"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_MSG_SOCIALCASINO_CREDITPM_REPLY : public Atf::MessageProtocol
	{
		// s - scExternalTransactId
		// implied INT16 errCode here. If not-zero (failure) then parse a String instead of below
		PString scExternalTransactId;

		Protocol_MSG_SOCIALCASINO_CREDITPM_REPLY();
		void clear() override;
		bool equals(const Protocol_MSG_SOCIALCASINO_CREDITPM_REPLY&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_SOCIALCASINO_CREDITPM_REPLY; }
		const char *getMsgIdString() const override { return "MSG_SOCIALCASINO_CREDITPM_REPLY"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_MSG_SOCIALCASINO_USERINFO : public Atf::MessageProtocol
	{
		// no content required

		Protocol_MSG_SOCIALCASINO_USERINFO();
		void clear() override;
		bool equals(const Protocol_MSG_SOCIALCASINO_USERINFO&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_SOCIALCASINO_USERINFO; }
		const char *getMsgIdString() const override { return "MSG_SOCIALCASINO_USERINFO"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct PlayerInfoWarning
	{
		// M(2s) - M(warningCode,warningString)
		INT16 warningCode;
		PString warningString;

		PlayerInfoWarning();
		void clear();
		bool equals(const PlayerInfoWarning&) const;
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

	struct Protocol_MSG_SOCIALCASINO_USERINFO_REPLY : public Atf::MessageProtocol
	{
		// s4s8ss4<M(2s)> - userId,imageId,country,playMoneyBalance,site,language,numWarnings<M(warningCode,warningString)>
		// implied INT16 errCode here. If not-zero (failure) then parse a String instead of below
		PString userId;
		UINT32 imageId;
		PString country;
		INT64 playMoneyBalance;
		PString site;
		PString language;
		Atf::LAtfVector< PlayerInfoWarning, 4 > warnings;

		Protocol_MSG_SOCIALCASINO_USERINFO_REPLY();
		void clear() override;
		bool equals(const Protocol_MSG_SOCIALCASINO_USERINFO_REPLY&) const;
		bool equals(Atf::MessageProtocol* _other) const override;
		const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
		const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
		bool loadFromXmlString(const PString& _xmlString) override;
		const char *asTraceString(PString& _buf) const override;
		void parseMsg(CommMsgParser& _parser) override;
		void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
		UINT32 getMsgId() const override { return MSG_SOCIALCASINO_USERINFO_REPLY; }
		const char *getMsgIdString() const override { return "MSG_SOCIALCASINO_USERINFO_REPLY"; }
		const char *asJSONString(PString& _buf) const override;
		void loadFromJSONString(const PString& _jsonString) override;
		static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};
};

#endif
