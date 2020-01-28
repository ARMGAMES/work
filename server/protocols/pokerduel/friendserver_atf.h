#ifndef ATF_PROTOCOL_DEFFILE_FRIENDSERVER_ATF_H
#define ATF_PROTOCOL_DEFFILE_FRIENDSERVER_ATF_H

#include "UserDefStructs.h"
#include "dbmmsg.h"
#include "pokerduel/friendserver.h"


namespace friendserver
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
	};


	namespace cli
	{
		struct FriendStruct
		{
			// M(s4bbbb4) - M(name,imageId,isOnline,hasPendingChallenge,isNewFriend,hasRMPendingChallenge,playerFlag)
			PString name;
			UINT32 imageId;
			bool isOnline;
			bool hasPendingChallenge;
			bool isNewFriend;
			bool hasRMPendingChallenge;
			UINT32 playerFlag;	//	enum eDuelUserFlags

			FriendStruct();
			void clear();
			bool equals(const FriendStruct&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			void composeMsg(Atf::MsgBodyWithJSON& _msg) const { _msg.compose(*this); }
			void parseMsg(Atf::MsgParserWithJSON& _parser);
		};
		struct FriendReqStruct
		{
			// M(s4b4) - M(name,imageId,isOnline,playerFlag)
			PString name;
			UINT32 imageId;
			bool isOnline;
			UINT32 playerFlag;	//	enum eDuelUserFlags

			FriendReqStruct();
			void clear();
			bool equals(const FriendReqStruct&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			void composeMsg(Atf::MsgBodyWithJSON& _msg) const { _msg.compose(*this); }
			void parseMsg(Atf::MsgParserWithJSON& _parser);
		};
		struct BlockedUserStruct
		{
			// M(s) - M(name)
			PString name;

			BlockedUserStruct();
			void clear();
			bool equals(const BlockedUserStruct&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			void composeMsg(Atf::MsgBodyWithJSON& _msg) const { _msg.compose(*this); }
			void parseMsg(Atf::MsgParserWithJSON& _parser);
		};

		// every messages will be in pair (first is from client to friend server, and the second is the reply from server to client)
		struct Protocol_MSG_FRDSRV_GET_FRIEND_LIST : public Atf::MessageProtocol
		{
			// empty message body

			Protocol_MSG_FRDSRV_GET_FRIEND_LIST();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_FRIEND_LIST&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_FRIEND_LIST; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_FRIEND_LIST"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY : public Atf::MessageProtocol
		{
			// 2[s|4<M(s4bbbb4)>] - errCode[errMsg|numFriends<M(name,imageId,isOnline,hasPendingChallenge,isNewFriend,hasRMPendingChallenge,playerFlag)>]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< FriendStruct, 4 > friends;

			Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_FRIEND_LIST_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_FRIEND_LIST_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_REMOVE_FRIEND : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_REMOVE_FRIEND();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_REMOVE_FRIEND&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_REMOVE_FRIEND; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_REMOVE_FRIEND"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_REMOVE_FRIEND_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_REMOVE_FRIEND_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_GET_BLOCKED_LIST : public Atf::MessageProtocol
		{
			// empty message body

			Protocol_MSG_FRDSRV_GET_BLOCKED_LIST();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_BLOCKED_LIST&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_BLOCKED_LIST; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_BLOCKED_LIST"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY : public Atf::MessageProtocol
		{
			// 2[s|4<M(s)>] - errCode[errMsg|numFriends<M(name)>]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< BlockedUserStruct, 4 > friends;

			Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_BLOCKED_LIST_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_BLOCKED_LIST_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_UNBLOCK_USER : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_UNBLOCK_USER();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_UNBLOCK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_UNBLOCK_USER; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_UNBLOCK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_UNBLOCK_USER_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_UNBLOCK_USER_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST : public Atf::MessageProtocol
		{
			// empty message body

			Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_FRIEND_REQ_LIST; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_FRIEND_REQ_LIST"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY : public Atf::MessageProtocol
		{
			// 2[s|4<M(s4b4)>] - errCode[errMsg|numFriends<M(name,imageId,isOnline,playerFlag)>]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< FriendReqStruct, 4 > friends;

			Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_ADD_FRIEND_REQ : public Atf::MessageProtocol
		{
			// sb - userId,throughUserSearch
			PString userId;
			bool throughUserSearch;

			Protocol_MSG_FRDSRV_ADD_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_ADD_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_ADD_FRIEND_REQ; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_ADD_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_ADD_FRIEND_REQ_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_ADD_FRIEND_REQ_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_DENY_FRIEND_REQ : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_DENY_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DENY_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_DENY_FRIEND_REQ; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DENY_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DENY_FRIEND_REQ_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DENY_FRIEND_REQ_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_ACCEPT_FRIEND_REQ; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_ACCEPT_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_BLOCK_USER : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_BLOCK_USER();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_BLOCK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_BLOCK_USER; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_BLOCK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_BLOCK_USER_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_BLOCK_USER_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_BLOCK_USER_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_BLOCK_USER_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_BLOCK_USER_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_CHECK_USER : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_CHECK_USER();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_CHECK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_CHECK_USER; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_CHECK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_CHECK_USER_REPLY : public Atf::MessageProtocol
		{
			// 2[s|b44sb4] - errCode[errMsg|isOnline,relation,imageId,originalUserId,addFrdNotAllowed,playerFlag]
			INT16 errCode;

			PString errMsg;

			bool isOnline;
			UINT32 relation;
			UINT32 imageId;
			PString originalUserId;
			bool addFrdNotAllowed;
			UINT32 playerFlag;	//	enum eDuelUserFlags

			Protocol_MSG_FRDSRV_CHECK_USER_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_CHECK_USER_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_CHECK_USER_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_CHECK_USER_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ : public Atf::MessageProtocol
		{
			// b - setNotRemove
			bool setNotRemove;

			Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct RecentOppStruct
		{
			// M(s4b44) - M(name,imageId,isOnline,frdRelation,playerFlag)
			PString name;
			UINT32 imageId;
			bool isOnline;
			UINT32 frdRelation;
			UINT32 playerFlag;	//	enum eDuelUserFlags

			RecentOppStruct();
			void clear();
			bool equals(const RecentOppStruct&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			void composeMsg(Atf::MsgBodyWithJSON& _msg) const { _msg.compose(*this); }
			void parseMsg(Atf::MsgParserWithJSON& _parser);
		};
		struct Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST : public Atf::MessageProtocol
		{
			// empty message body

			Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_RECENT_PLAYED_LIST; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_RECENT_PLAYED_LIST"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY : public Atf::MessageProtocol
		{
			// 2[s|4<M(s4b44)>] - errCode[errMsg|numRecentPlayedUsers<M(name,imageId,isOnline,frdRelation,playerFlag)>]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< RecentOppStruct, 4 > recentPlayedUsers;

			Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN : public Atf::MessageProtocol
		{
			// s - token
			PString token;

			Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_USE_FRD_SHARE_TOKEN; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_USE_FRD_SHARE_TOKEN"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY : public Atf::MessageProtocol
		{
			// 2[s|s] - errCode[errMsg|tokenUserId]
			INT16 errCode;

			PString errMsg;

			PString tokenUserId;

			Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		// notification message (server -> client)
		struct Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE : public Atf::MessageProtocol
		{
			// 4s - friendUpdateType,userId
			UINT32 friendUpdateType;
			PString userId;

			Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		// notification message (server -> client)
		struct Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE : public Atf::MessageProtocol
		{
			// bs - addOrDrop,userId
			bool addOrDrop;
			PString userId;

			Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE();
			void clear();
			bool equals(const Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE; }
			const char *getMsgIdString() const override { return "MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_GET_SHARE_URL : public Atf::MessageProtocol
		{

			Protocol_MSG_FRDSRV_GET_SHARE_URL();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_SHARE_URL&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_SHARE_URL; }
// !!! WARNING !!! If you modify this message, please let testautomation team know.
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_SHARE_URL"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY : public Atf::MessageProtocol
		{
			// 2[s|s] - errCode[errMsg|url]
			INT16 errCode;

			PString errMsg;

			PString url;

			Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_GET_SHARE_URL_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_GET_SHARE_URL_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
	};

	namespace rodbm
	{
		struct UserId_UserIntId
		{
			// s4 - userId,userIntId
			PString userId;
			UINT32 userIntId;

			UserId_UserIntId();
			void clear();
			bool equals(const UserId_UserIntId&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			void composeMsg(Atf::MsgBodyWithJSON& _msg) const { _msg.compose(*this); }
			void parseMsg(Atf::MsgParserWithJSON& _parser);
		};

		struct Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID();
			void clear();
			bool equals(const Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_GET_USERINTID_BY_NORMALIZED_ID; }
			const char *getMsgIdString() const override { return "DBM_Q_GET_USERINTID_BY_NORMALIZED_ID"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID : public Atf::MessageProtocol
		{
			// 2[s|4s] - errCode[errMsg|userIntId,originalUserId]
			INT16 errCode;

			PString errMsg;

			UINT32 userIntId;
			PString originalUserId;

			Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID();
			void clear();
			bool equals(const Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_GET_USERINTID_BY_NORMALIZED_ID; }
			const char *getMsgIdString() const override { return "DBM_A_GET_USERINTID_BY_NORMALIZED_ID"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
	};

	namespace frienddbm
	{
		struct FriendStruct
		{
			// sb - name,hasNewFriendPendingAction
			PString name;
			bool hasNewFriendPendingAction;

			FriendStruct();
			void clear();
			bool equals(const FriendStruct&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			void composeMsg(Atf::MsgBodyWithJSON& _msg) const { _msg.compose(*this); }
			void parseMsg(Atf::MsgParserWithJSON& _parser);
		};

		// every messages will be in pair (first is from friend server to friend dbm, and the second is from friend dbm to friend server)
		struct Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA : public Atf::MessageProtocol
		{
			// 4 - userIntId
			UINT32 userIntId;

			Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_LOAD_USER_DATA; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_LOAD_USER_DATA"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_LOAD_USER_DATA : public Atf::MessageProtocol
		{
			// 2[s|4<sb>4<s>4<s>4b4] - errCode[errMsg|numFriendUsers<name,hasNewFriendPendingAction>,numFriendReqUserIds<friendReqUserIds>,numBlockedUserIds<blockedUserIds>,flags,hasScore,score]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< FriendStruct, 4 > friendUsers;
			Atf::LAtfVector< PString, 4 > friendReqUserIds;
			Atf::LAtfVector< PString, 4 > blockedUserIds;
			UINT32 flags;
			bool hasScore;
			UINT32 score;

			Protocol_DBM_A_FRDSRV_LOAD_USER_DATA();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_LOAD_USER_DATA&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_LOAD_USER_DATA; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_LOAD_USER_DATA"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND : public Atf::MessageProtocol
		{
			// 4s - userIntId1,userId2
			UINT32 userIntId1;
			PString userId2;

			Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_REMOVE_FRIEND; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_REMOVE_FRIEND"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_REMOVE_FRIEND : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_REMOVE_FRIEND();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_REMOVE_FRIEND&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_REMOVE_FRIEND; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_REMOVE_FRIEND"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_UNBLOCK_USER : public Atf::MessageProtocol
		{
			// 4s - userIntId1,userId2
			UINT32 userIntId1;
			PString userId2;

			Protocol_DBM_Q_FRDSRV_UNBLOCK_USER();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_UNBLOCK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_UNBLOCK_USER; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_UNBLOCK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_UNBLOCK_USER : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_UNBLOCK_USER();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_UNBLOCK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_UNBLOCK_USER; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_UNBLOCK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ : public Atf::MessageProtocol
		{
			// s4s4 - userId1,userIntId1,userId2,userIntId2
			PString userId1;
			UINT32 userIntId1;
			PString userId2;
			UINT32 userIntId2;

			Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_ADD_FRIEND_REQ; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_ADD_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_ADD_FRIEND_REQ; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_ADD_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ : public Atf::MessageProtocol
		{
			// s4s - userId1,userIntId1,userId2
			PString userId1;
			UINT32 userIntId1;
			PString userId2;

			Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_DENY_FRIEND_REQ; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_DENY_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_DENY_FRIEND_REQ; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_DENY_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ : public Atf::MessageProtocol
		{
			// s4s4 - userId1,userIntId1,userId2,userIntId2
			PString userId1;
			UINT32 userIntId1;
			PString userId2;
			UINT32 userIntId2;

			Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_ACCEPT_FRIEND_REQ; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_ACCEPT_FRIEND_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_Q_FRDSRV_BLOCK_USER : public Atf::MessageProtocol
		{
			// s4s4 - userId1,userIntId1,userId2,userIntId2
			PString userId1;
			UINT32 userIntId1;
			PString userId2;
			UINT32 userIntId2;

			Protocol_DBM_Q_FRDSRV_BLOCK_USER();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_BLOCK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_BLOCK_USER; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_BLOCK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_BLOCK_USER : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_BLOCK_USER();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_BLOCK_USER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_BLOCK_USER; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_BLOCK_USER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE : public Atf::MessageProtocol
		{
			// s444 - userId,userIntId,updateType,score
			PString userId;
			UINT32 userIntId;
			UINT32 updateType;
			INT32 score;

			Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_SET_USER_FRIEND_SCORE; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_SET_USER_FRIEND_SCORE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ : public Atf::MessageProtocol
		{
			// s4b - userId,userIntId,setNotRemove
			PString userId;
			UINT32 userIntId;
			bool setNotRemove;

			Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION : public Atf::MessageProtocol
		{
			// s44<s> - userId1,userIntId1,numUserIds<userIds>
			PString userId1;
			UINT32 userIntId1;
			Atf::LAtfVector< PString, 4 > userIds;

			Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO : public Atf::MessageProtocol
		{
			// s44<s> - userId_sender,userIntId_sender,numUserIds_receiver<userIds_receiver>
			PString userId_sender;
			UINT32 userIntId_sender;
			Atf::LAtfVector< PString, 4 > userIds_receiver;

			Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_GET_FRDREQ_INFO; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_GET_FRDREQ_INFO"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO : public Atf::MessageProtocol
		{
			// 2[s|4<4>] - errCode[errMsg|numVecReqSent<vecReqSent>]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< UINT32, 4 > vecReqSent;

			Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_GET_FRDREQ_INFO; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_GET_FRDREQ_INFO"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP : public Atf::MessageProtocol
		{
			// s4s4 - userId1,userIntId1,userId2,userIntId2
			PString userId1;
			UINT32 userIntId1;
			PString userId2;
			UINT32 userIntId2;

			Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP();
			void clear();
			bool equals(const Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP; }
			const char *getMsgIdString() const override { return "DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP();
			void clear();
			bool equals(const Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP; }
			const char *getMsgIdString() const override { return "DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
	};

	namespace olap
	{
		struct Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS : public Atf::MessageProtocol
		{
			// s44 - userId,userIntId,maxOpponents
			PString userId;
			UINT32 userIntId;
			INT32 maxOpponents;

			Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS();
			void clear();
			bool equals(const Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_Q_PDUEL_GET_RECENT_OPPONENTS; }
			const char *getMsgIdString() const override { return "DBM_Q_PDUEL_GET_RECENT_OPPONENTS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS : public Atf::MessageProtocol
		{
			// 2[s|4<s>] - errCode[errMsg|numRecentPlayedUsers<recentPlayedUsers>]
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< PString, 4 > recentPlayedUsers;

			Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS();
			void clear();
			bool equals(const Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return DBM_A_PDUEL_GET_RECENT_OPPONENTS; }
			const char *getMsgIdString() const override { return "DBM_A_PDUEL_GET_RECENT_OPPONENTS"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
	};

	namespace dlobby
	{
		struct Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP : public Atf::MessageProtocol
		{
			// ss - userId1,userId2
			PString userId1;
			PString userId2;

			Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY : public Atf::MessageProtocol
		{
			// 2[s|b] - errCode[errMsg|isFriend]
			INT16 errCode;

			PString errMsg;

			bool isFriend;

			Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;

			Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY : public Atf::MessageProtocol
		{
			// 2[s|4] - errCode[errMsg|numFriends]
			INT16 errCode;

			PString errMsg;

			INT32 numFriends;

			Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE : public Atf::MessageProtocol
		{
			// ss - userId1,userId2
			PString userId1;
			PString userId2;

			Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY : public Atf::MessageProtocol
		{
			// 2[s|4b4] - errCode[errMsg|numFriends,isOnline,frdRelation]
			INT16 errCode;

			PString errMsg;

			UINT32 numFriends;
			bool isOnline;
			UINT32 frdRelation;

			Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		// notification only
		struct Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE : public Atf::MessageProtocol
		{
			// ssb - userId1,userId2,addOrRemove
			PString userId1;
			PString userId2;
			bool addOrRemove;

			Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED : public Atf::MessageProtocol
		{
			// ss - userId1,userId2
			PString userId1;
			PString userId2;

			Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};

		struct Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT : public Atf::MessageProtocol
		{
			// ss - userId1,userId2
			PString userId1;
			PString userId2;

			Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_CHALLENGE_SENT; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_CHALLENGE_SENT"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
		struct Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY : public Atf::MessageProtocol
		{
			// 2[s] - errCode[errMsg]
			INT16 errCode;

			PString errMsg;

			Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY();
			void clear();
			bool equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY; }
			const char *getMsgIdString() const override { return "MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
		};
	};
};

#endif
