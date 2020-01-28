/**
 * serverreplay_atf_thin.h
 *
 * This file was auto-generated from serverreplay_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin serverreplay_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_SERVERREPLAY_ATF_THIN_H
#define ATF_PROTOCOL_DEFFILE_SERVERREPLAY_ATF_THIN_H

#include "table.h"
#include "sites.h"
#include "serverreplay.h"
#include "ThinAtfUserDefStructs.h"
#include "Common_atf_thin.h"


namespace ServerReplay
{
	class ProtocolFactory
	{
	public:
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace cli
	{
		struct Protocol_MSG_TABLE_LOGIN
		{
			// s4bbb - user,clientSiteId,notShowWhenAllIn,sitOutNextHand,useAllInitStacks
			PString user;
			UINT32 clientSiteId;
			bool notShowWhenAllIn;
			bool sitOutNextHand;
			bool useAllInitStacks;

			Protocol_MSG_TABLE_LOGIN();
			void clear();
			bool equals(const Protocol_MSG_TABLE_LOGIN&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_TABLE_LOGIN; }
			static const char *getMsgIdString() { return "MSG_TABLE_LOGIN"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_TABLE_LOGIN(Protocol_MSG_TABLE_LOGIN&&);
			Protocol_MSG_TABLE_LOGIN& operator=(Protocol_MSG_TABLE_LOGIN&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_TABLE_LOGIN(const Protocol_MSG_TABLE_LOGIN&) = default;
			Protocol_MSG_TABLE_LOGIN& operator=(const Protocol_MSG_TABLE_LOGIN&) = default;
#endif
#endif
		};

		struct Protocol_MSG_TABLE_LOGIN_REPLY
		{
			// 1s11s4M()1222bbb2 - errCode,errStr,seat,sittingOut,clientIP,privilege,M(),autoRebuyOptions,autoRebuyBigBets,autoRebuyFallUnderBigBets,autoRebuyFallUnderPercents,playerRITFavored,campingPreventionEnabled,isTableDealingHands,campingCountingDownSec
			BYTE errCode;
			PString errStr;
			INT8 seat;
			BYTE sittingOut;
			PString clientIP;
			UINT32 privilege;
			ThinAtf::CopyableMsgBody clientMsg_txtEx;
			INT8 autoRebuyOptions;
			UINT16 autoRebuyBigBets;
			UINT16 autoRebuyFallUnderBigBets;
			UINT16 autoRebuyFallUnderPercents;
			bool playerRITFavored;
			bool campingPreventionEnabled;
			bool isTableDealingHands;
			UINT16 campingCountingDownSec;

			Protocol_MSG_TABLE_LOGIN_REPLY();
			void clear();
			bool equals(const Protocol_MSG_TABLE_LOGIN_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_TABLE_LOGIN_REPLY; }
			static const char *getMsgIdString() { return "MSG_TABLE_LOGIN_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_TABLE_LOGIN_REPLY(Protocol_MSG_TABLE_LOGIN_REPLY&&);
			Protocol_MSG_TABLE_LOGIN_REPLY& operator=(Protocol_MSG_TABLE_LOGIN_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_TABLE_LOGIN_REPLY(const Protocol_MSG_TABLE_LOGIN_REPLY&) = default;
			Protocol_MSG_TABLE_LOGIN_REPLY& operator=(const Protocol_MSG_TABLE_LOGIN_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_TABLE_CHAT
		{
			// s - chat
			PString chat;

			Protocol_MSG_TABLE_CHAT();
			void clear();
			bool equals(const Protocol_MSG_TABLE_CHAT&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_TABLE_CHAT; }
			static const char *getMsgIdString() { return "MSG_TABLE_CHAT"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_TABLE_CHAT(Protocol_MSG_TABLE_CHAT&&);
			Protocol_MSG_TABLE_CHAT& operator=(Protocol_MSG_TABLE_CHAT&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_TABLE_CHAT(const Protocol_MSG_TABLE_CHAT&) = default;
			Protocol_MSG_TABLE_CHAT& operator=(const Protocol_MSG_TABLE_CHAT&) = default;
#endif
#endif
		};

		struct Protocol_MSG_TABLE_CHAT2
		{
			// s - chat
			PString chat;

			Protocol_MSG_TABLE_CHAT2();
			void clear();
			bool equals(const Protocol_MSG_TABLE_CHAT2&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_TABLE_CHAT2; }
			static const char *getMsgIdString() { return "MSG_TABLE_CHAT2"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_TABLE_CHAT2(Protocol_MSG_TABLE_CHAT2&&);
			Protocol_MSG_TABLE_CHAT2& operator=(Protocol_MSG_TABLE_CHAT2&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_TABLE_CHAT2(const Protocol_MSG_TABLE_CHAT2&) = default;
			Protocol_MSG_TABLE_CHAT2& operator=(const Protocol_MSG_TABLE_CHAT2&) = default;
#endif
#endif
		};

		struct Protocol_MSG_TABLE_CHAT2_REPLY
		{
			// 2M()s - errCode,M(),chatMsg
			UINT16 errCode;
			ThinAtf::CopyableMsgBody errMsgBody;
			PString chatMsg;

			Protocol_MSG_TABLE_CHAT2_REPLY();
			void clear();
			bool equals(const Protocol_MSG_TABLE_CHAT2_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_TABLE_CHAT2_REPLY; }
			static const char *getMsgIdString() { return "MSG_TABLE_CHAT2_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_TABLE_CHAT2_REPLY(Protocol_MSG_TABLE_CHAT2_REPLY&&);
			Protocol_MSG_TABLE_CHAT2_REPLY& operator=(Protocol_MSG_TABLE_CHAT2_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_TABLE_CHAT2_REPLY(const Protocol_MSG_TABLE_CHAT2_REPLY&) = default;
			Protocol_MSG_TABLE_CHAT2_REPLY& operator=(const Protocol_MSG_TABLE_CHAT2_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_PAUSE
		{
			// empty message

			Protocol_MSG_REPLAY_PAUSE();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_PAUSE&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_PAUSE; }
			static const char *getMsgIdString() { return "MSG_REPLAY_PAUSE"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_PAUSE_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_PAUSE_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_PAUSE_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_PAUSE_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_PAUSE_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_PAUSE_REPLY(Protocol_MSG_REPLAY_PAUSE_REPLY&&);
			Protocol_MSG_REPLAY_PAUSE_REPLY& operator=(Protocol_MSG_REPLAY_PAUSE_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_PAUSE_REPLY(const Protocol_MSG_REPLAY_PAUSE_REPLY&) = default;
			Protocol_MSG_REPLAY_PAUSE_REPLY& operator=(const Protocol_MSG_REPLAY_PAUSE_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_RESUME
		{
			// empty message

			Protocol_MSG_REPLAY_RESUME();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_RESUME&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_RESUME; }
			static const char *getMsgIdString() { return "MSG_REPLAY_RESUME"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_RESUME_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_RESUME_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_RESUME_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_RESUME_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_RESUME_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_RESUME_REPLY(Protocol_MSG_REPLAY_RESUME_REPLY&&);
			Protocol_MSG_REPLAY_RESUME_REPLY& operator=(Protocol_MSG_REPLAY_RESUME_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_RESUME_REPLY(const Protocol_MSG_REPLAY_RESUME_REPLY&) = default;
			Protocol_MSG_REPLAY_RESUME_REPLY& operator=(const Protocol_MSG_REPLAY_RESUME_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_NEXT_HAND
		{
			// empty message

			Protocol_MSG_REPLAY_NEXT_HAND();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_NEXT_HAND&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_NEXT_HAND; }
			static const char *getMsgIdString() { return "MSG_REPLAY_NEXT_HAND"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_NEXT_HAND_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_NEXT_HAND_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_NEXT_HAND_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_NEXT_HAND_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_NEXT_HAND_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_NEXT_HAND_REPLY(Protocol_MSG_REPLAY_NEXT_HAND_REPLY&&);
			Protocol_MSG_REPLAY_NEXT_HAND_REPLY& operator=(Protocol_MSG_REPLAY_NEXT_HAND_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_NEXT_HAND_REPLY(const Protocol_MSG_REPLAY_NEXT_HAND_REPLY&) = default;
			Protocol_MSG_REPLAY_NEXT_HAND_REPLY& operator=(const Protocol_MSG_REPLAY_NEXT_HAND_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_PREV_HAND
		{
			// empty message

			Protocol_MSG_REPLAY_PREV_HAND();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_PREV_HAND&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_PREV_HAND; }
			static const char *getMsgIdString() { return "MSG_REPLAY_PREV_HAND"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_PREV_HAND_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_PREV_HAND_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_PREV_HAND_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_PREV_HAND_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_PREV_HAND_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_PREV_HAND_REPLY(Protocol_MSG_REPLAY_PREV_HAND_REPLY&&);
			Protocol_MSG_REPLAY_PREV_HAND_REPLY& operator=(Protocol_MSG_REPLAY_PREV_HAND_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_PREV_HAND_REPLY(const Protocol_MSG_REPLAY_PREV_HAND_REPLY&) = default;
			Protocol_MSG_REPLAY_PREV_HAND_REPLY& operator=(const Protocol_MSG_REPLAY_PREV_HAND_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_HANDS_QUANTITY
		{
			// empty message

			Protocol_MSG_REPLAY_HANDS_QUANTITY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HANDS_QUANTITY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HANDS_QUANTITY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HANDS_QUANTITY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY
		{
			// 2s4 - errCode,errMsg,quantity
			UINT16 errCode;
			PString errMsg;
			UINT32 quantity;

			Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HANDS_QUANTITY_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HANDS_QUANTITY_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY(Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY&&);
			Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY& operator=(Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY(const Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY&) = default;
			Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY& operator=(const Protocol_MSG_REPLAY_HANDS_QUANTITY_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_CURR_HAND_INFO
		{
			// empty message

			Protocol_MSG_REPLAY_CURR_HAND_INFO();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_CURR_HAND_INFO&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_CURR_HAND_INFO; }
			static const char *getMsgIdString() { return "MSG_REPLAY_CURR_HAND_INFO"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY
		{
			// 2s44 - errCode,errMsg,result,currentItemResult
			UINT16 errCode;
			PString errMsg;
			UINT32 result;
			UINT32 currentItemResult;

			Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_CURR_HAND_INFO_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_CURR_HAND_INFO_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY(Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY&&);
			Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY& operator=(Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY(const Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY&) = default;
			Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY& operator=(const Protocol_MSG_REPLAY_CURR_HAND_INFO_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_GOTO_I_TH_HAND
		{
			// 4 - handNumber
			UINT32 handNumber;

			Protocol_MSG_REPLAY_GOTO_I_TH_HAND();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_I_TH_HAND; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_I_TH_HAND"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_I_TH_HAND_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_I_TH_HAND_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY(Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY&&);
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY& operator=(Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY&) = default;
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY& operator=(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_HAND_IDS
		{
			// empty message

			Protocol_MSG_REPLAY_HAND_IDS();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HAND_IDS&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HAND_IDS; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HAND_IDS"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_HAND_IDS_REPLY
		{
			// 2s4<4> - errCode,errMsg,numResults<results>
			UINT16 errCode;
			PString errMsg;
			ThinAtf::LAtfVector< UINT32, 4 > results;

			Protocol_MSG_REPLAY_HAND_IDS_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HAND_IDS_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HAND_IDS_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HAND_IDS_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_HAND_IDS_REPLY(Protocol_MSG_REPLAY_HAND_IDS_REPLY&&);
			Protocol_MSG_REPLAY_HAND_IDS_REPLY& operator=(Protocol_MSG_REPLAY_HAND_IDS_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_HAND_IDS_REPLY(const Protocol_MSG_REPLAY_HAND_IDS_REPLY&) = default;
			Protocol_MSG_REPLAY_HAND_IDS_REPLY& operator=(const Protocol_MSG_REPLAY_HAND_IDS_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_GOTO_HAND_ID
		{
			// 4 - handId
			UINT32 handId;

			Protocol_MSG_REPLAY_GOTO_HAND_ID();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_HAND_ID&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_HAND_ID; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_HAND_ID"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_HAND_ID_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_HAND_ID_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY(Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY&&);
			Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY& operator=(Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY(const Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY&) = default;
			Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY& operator=(const Protocol_MSG_REPLAY_GOTO_HAND_ID_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_PAUSE_AFTER_HAND
		{
			// empty message

			Protocol_MSG_REPLAY_PAUSE_AFTER_HAND();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_PAUSE_AFTER_HAND&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_PAUSE_AFTER_HAND; }
			static const char *getMsgIdString() { return "MSG_REPLAY_PAUSE_AFTER_HAND"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_PAUSE_AFTER_HAND_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_PAUSE_AFTER_HAND_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY(Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY&&);
			Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY& operator=(Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY(const Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY&) = default;
			Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY& operator=(const Protocol_MSG_REPLAY_PAUSE_AFTER_HAND_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_HAND_IDS_64
		{
			// empty message

			Protocol_MSG_REPLAY_HAND_IDS_64();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HAND_IDS_64&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HAND_IDS_64; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HAND_IDS_64"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_HAND_IDS_64_REPLY
		{
			// 2s4<8> - errCode,errMsg,numResults<results>
			UINT16 errCode;
			PString errMsg;
			ThinAtf::LAtfVector< UINT64, 4 > results;

			Protocol_MSG_REPLAY_HAND_IDS_64_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HAND_IDS_64_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HAND_IDS_64_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HAND_IDS_64_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_HAND_IDS_64_REPLY(Protocol_MSG_REPLAY_HAND_IDS_64_REPLY&&);
			Protocol_MSG_REPLAY_HAND_IDS_64_REPLY& operator=(Protocol_MSG_REPLAY_HAND_IDS_64_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_HAND_IDS_64_REPLY(const Protocol_MSG_REPLAY_HAND_IDS_64_REPLY&) = default;
			Protocol_MSG_REPLAY_HAND_IDS_64_REPLY& operator=(const Protocol_MSG_REPLAY_HAND_IDS_64_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_CURR_HAND_INFO_64
		{
			// empty message

			Protocol_MSG_REPLAY_CURR_HAND_INFO_64();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_CURR_HAND_INFO_64&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_CURR_HAND_INFO_64; }
			static const char *getMsgIdString() { return "MSG_REPLAY_CURR_HAND_INFO_64"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY
		{
			// 2s88 - errCode,errMsg,result,currentItemResult
			UINT16 errCode;
			PString errMsg;
			UINT64 result;
			UINT64 currentItemResult;

			Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_CURR_HAND_INFO_64_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_CURR_HAND_INFO_64_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY(Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY&&);
			Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY& operator=(Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY(const Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY&) = default;
			Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY& operator=(const Protocol_MSG_REPLAY_CURR_HAND_INFO_64_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_GOTO_HAND_ID_64
		{
			// 8 - handId
			UINT64 handId;

			Protocol_MSG_REPLAY_GOTO_HAND_ID_64();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_HAND_ID_64&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_HAND_ID_64; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_HAND_ID_64"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_HAND_ID_64_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_HAND_ID_64_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY(Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY&&);
			Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY& operator=(Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY(const Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY&) = default;
			Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY& operator=(const Protocol_MSG_REPLAY_GOTO_HAND_ID_64_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_HANDS_QUANTITY_64
		{
			// empty message

			Protocol_MSG_REPLAY_HANDS_QUANTITY_64();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HANDS_QUANTITY_64&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HANDS_QUANTITY_64; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HANDS_QUANTITY_64"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY
		{
			// 2s4 - errCode,errMsg,quantity
			UINT16 errCode;
			PString errMsg;
			UINT32 quantity;

			Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_HANDS_QUANTITY_64_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_HANDS_QUANTITY_64_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY(Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY&&);
			Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY& operator=(Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY(const Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY&) = default;
			Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY& operator=(const Protocol_MSG_REPLAY_HANDS_QUANTITY_64_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64
		{
			// 8 - handNumber
			UINT64 handNumber;

			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_I_TH_HAND_64; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_I_TH_HAND_64"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY
		{
			// 2s - errCode,errMsg
			UINT16 errCode;
			PString errMsg;

			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY();
			void clear();
			bool equals(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY; }
			static const char *getMsgIdString() { return "MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY(Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY&&);
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY& operator=(Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY&) = default;
			Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY& operator=(const Protocol_MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY&) = default;
#endif
#endif
		};

	};
};

#endif