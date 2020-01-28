/**
 * SngStarter_atf.h
 *
 * This file was auto-generated from SngStarter_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: SngStarter_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_SNGSTARTER_ATF_H
#define ATF_PROTOCOL_DEFFILE_SNGSTARTER_ATF_H

#include "UserDefStructs.h"
#include "Common_atf.h"
#include "lobby.h"


#define ATF_SNG_MAX_REG_GAMES        100
#define ATF_SNG_MAX_BUYIN_REAL      (20000 * 100) 	// $20,000
#define ATF_SNG_MAX_BUYIN_PM        (PINT32_MAX / 2)   	// no practical limit
#define ATF_SNG_MAX_BUYIN_FPP        PINT32_MAX        	// no limit
#define ATF_SNG_MAX_ADMISSIONS       100
#define ATF_SNG_MAX_ADMISSION_PRICE (ATF_SNG_MAX_BUYIN_PM * ATF_MAX_PM_SCALE_LL)

#define ATF_SNG_MAX_PASSWORD_LEN 100

namespace SngStarter
{
	class ProtocolFactory
	{
	public:
		static Atf::MessageProtocol* createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId);
		static bool validateProtocolMessageFromJSONString(const char *_nameSpace, UINT32 _msgId, const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace cli
	{
		struct Protocol_MSG_LOBBY_TOURN_REG_INFO : public Atf::MessageProtocol
		{
			// 414 - scriptId,numGames,tournRegInfoFlags
			UINT32 scriptId;
			BYTE numGames;
			UINT32 tournRegInfoFlags;

			Protocol_MSG_LOBBY_TOURN_REG_INFO();
			void clear() override;
			bool equals(const Protocol_MSG_LOBBY_TOURN_REG_INFO&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_LOBBY_TOURN_REG_INFO; }
			const char *getMsgIdString() const override { return "MSG_LOBBY_TOURN_REG_INFO"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_CHECKIN : public Atf::MessageProtocol
		{
			//! s444s144M()4184< <FIXME>Common::AtfShared::UserTicketDataBase</FIXME> > - userId,buyInRM,buyInPlay,buyInFpp,tournPwd,useTicket,buyInT,buyInW,M(),scriptId,numGames,admissionPrice,numAdmissions< <FIXME>Common::AtfShared::UserTicketDataBase</FIXME> >
			PString userId;
			INT32 buyInRM;
			INT32 buyInPlay;
			INT32 buyInFpp;
			PString tournPwd;
			BYTE useTicket;
			INT32 buyInT;
			INT32 buyInW;	// only 0 is allowed
			Atf::CopyableMsgBody currencyContext;	// we switched to an empty msgbody because trying to buy into a tournament with a user that does not have auto convert currency set and sending a non-empty currencyContext will return an error
			UINT32 scriptId;
			BYTE numGames;
			INT64 admissionPrice;
			Atf::LAtfVector< Common::AtfShared::UserTicketDataBase, 4 > admissions;

			Protocol_MSG_SNG_POOL_CHECKIN();
			void clear() override;
			bool equals(const Protocol_MSG_SNG_POOL_CHECKIN&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_SNG_POOL_CHECKIN; }
			const char *getMsgIdString() const override { return "MSG_SNG_POOL_CHECKIN"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_CHECKOUT : public Atf::MessageProtocol
		{
			// 4 - scriptId
			UINT32 scriptId;

			Protocol_MSG_SNG_POOL_CHECKOUT();
			void clear() override;
			bool equals(const Protocol_MSG_SNG_POOL_CHECKOUT&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_SNG_POOL_CHECKOUT; }
			const char *getMsgIdString() const override { return "MSG_SNG_POOL_CHECKOUT"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER : public Atf::MessageProtocol
		{
			// s - userId
			PString userId;	// client bug: can send email instead of userId, change limit to 100

			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER();
			void clear() override;
			bool equals(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_SNG_POOL_WHERE_IS_PLAYER; }
			const char *getMsgIdString() const override { return "MSG_SNG_POOL_WHERE_IS_PLAYER"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct UserTicketDataATF
		{
			// s44 - admissionId,totalCount,reserved
			PString admissionId;
			UINT32 totalCount;
			UINT32 reserved;

			UserTicketDataATF();
			void clear();
			bool equals(const UserTicketDataATF&) const;
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

		struct Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY : public Atf::MessageProtocol
		{
			//! 2 <FIXME_COND_IF> s <FIXME_COND_ELSE> 444s11444444s111sbM()M()ts84144144b84<s44>8 <FIXME_COND_ENDIF>  - errCode <FIXME_COND_IF> ,errMsg <FIXME_COND_ELSE> ,buyin,rake,fppBuyin,admission,pm,pwd,chips,playChips32,fpp,tickets,tchips,notused,preRegistrMessage,game,isHiLo,structure,currency,enoughMoneyInOtherCurrency,M(),M(),whenStartAbs,name,playChips,scalePM,speedDisplay,minPlayers,maxPlayers,maxPerTable,tournFlags,knockout,boolValue,admissionPrice,numEffectiveAdmissions<admissionId,totalCount,reserved>,tournFlags2 <FIXME_COND_ENDIF> 
			INT16 errCode;

			PString errMsg;

			UINT32 buyin;
			UINT32 rake;
			UINT32 fppBuyin;
			PString admission;
			BYTE pm;
			BYTE pwd;
			UINT32 chips;
			UINT32 playChips32;
			UINT32 fpp;
			UINT32 tickets;
			UINT32 tchips;
			UINT32 notused;
			PString preRegistrMessage;
			BYTE game;
			BYTE isHiLo;
			BYTE structure;
			PString currency;
			bool enoughMoneyInOtherCurrency;
			Atf::CopyableMsgBody currencyContext;
			Atf::CopyableMsgBody clientCurrencyContext;
			SrvTime whenStartAbs;
			PString name;
			UINT64 playChips;
			UINT32 scalePM;
			BYTE speedDisplay;
			UINT32 minPlayers;
			UINT32 maxPlayers;
			BYTE maxPerTable;
			UINT32 tournFlags;
			UINT32 knockout;
			bool boolValue;
			INT64 admissionPrice;
			Atf::LAtfVector< UserTicketDataATF, 4 > effectiveAdmissions;
			UINT64 tournFlags2;

			Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY();
			void clear() override;
			bool equals(const Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_LOBBY_TOURN_REG_INFO_REPLY; }
			const char *getMsgIdString() const override { return "MSG_LOBBY_TOURN_REG_INFO_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_CHECKIN_REPLY : public Atf::MessageProtocol
		{
			//! 42 <FIXME_COND_IF> s <FIXME_COND_ELSE> 14s4441111s4s <FIXME_COND_ENDIF>  - scriptId,errCode <FIXME_COND_IF> ,errMsg <FIXME_COND_ELSE> ,totalGamesNow,tournId,sampleAddress,buyIn,fppBuyIn,rake,game,hiLo,structure,isPlayMoney,icurrency,scalePM,name <FIXME_COND_ENDIF> 
			UINT32 scriptId;
			INT16 errCode;

			PString errMsg;

			BYTE totalGamesNow;
			UINT32 tournId;
			PString sampleAddress;
			UINT32 buyIn;
			UINT32 fppBuyIn;
			UINT32 rake;
			BYTE game;
			BYTE hiLo;
			BYTE structure;
			BYTE isPlayMoney;
			PString icurrency;
			UINT32 scalePM;
			PString name;

			Protocol_MSG_SNG_POOL_CHECKIN_REPLY();
			void clear() override;
			bool equals(const Protocol_MSG_SNG_POOL_CHECKIN_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_SNG_POOL_CHECKIN_REPLY; }
			const char *getMsgIdString() const override { return "MSG_SNG_POOL_CHECKIN_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_CHECKOUT_REPLY : public Atf::MessageProtocol
		{
			// 421 - scriptId,errCode,cancelledGames
			UINT32 scriptId;
			INT16 errCode;
			BYTE cancelledGames;

			Protocol_MSG_SNG_POOL_CHECKOUT_REPLY();
			void clear() override;
			bool equals(const Protocol_MSG_SNG_POOL_CHECKOUT_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_SNG_POOL_CHECKOUT_REPLY; }
			const char *getMsgIdString() const override { return "MSG_SNG_POOL_CHECKOUT_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct SngGameBody
		{
			// M(414s4441111s4s8) - M(scriptId,numGames,tournId,sampleAddress,buyIn,fppBuyIn,rake,game,hiLo,structure,isPlayMoney,currency,scalePM,name,tournFlags2)
			UINT32 scriptId;
			BYTE numGames;
			UINT32 tournId;
			PString sampleAddress;
			UINT32 buyIn;
			UINT32 fppBuyIn;
			UINT32 rake;
			BYTE game;
			BYTE hiLo;
			BYTE structure;
			BYTE isPlayMoney;
			PString currency;
			UINT32 scalePM;
			PString name;
			UINT64 tournFlags2;

			SngGameBody();
			void clear();
			bool equals(const SngGameBody&) const;
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

		struct Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY : public Atf::MessageProtocol
		{
			//! 2 <FIXME_COND_IF> s <FIXME_COND_ELSE> 4<M(414s4441111s4s8)> <FIXME_COND_ENDIF>  - errCode <FIXME_COND_IF> ,errMsg <FIXME_COND_ELSE> ,numSngs<M(scriptId,numGames,tournId,sampleAddress,buyIn,fppBuyIn,rake,game,hiLo,structure,isPlayMoney,currency,scalePM,name,tournFlags2)> <FIXME_COND_ENDIF> 
			INT16 errCode;

			PString errMsg;

			Atf::LAtfVector< SngGameBody, 4 > sngs;

			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY();
			void clear() override;
			bool equals(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY&) const;
			bool equals(Atf::MessageProtocol* _other) const override;
			const char *asXmlString(PString& _buf, bool _logTimeStamp) const override;
			const char *asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const override;
			bool loadFromXmlString(const PString& _xmlString) override;
			const char *asTraceString(PString& _buf) const override;
			void parseMsg(CommMsgParser& _parser) override;
			void composeMsg(CommMsgBody& _body, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const override;
			UINT32 getMsgId() const override { return MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY; }
			const char *getMsgIdString() const override { return "MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY"; }
			const char *asJSONString(PString& _buf) const override;
			void loadFromJSONString(const PString& _jsonString) override;
			static void validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};
	};
};

#endif
