/**
 * SngStarter_atf_thin.h
 *
 * This file was auto-generated from SngStarter_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin SngStarter_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_SNGSTARTER_ATF_THIN_H
#define ATF_PROTOCOL_DEFFILE_SNGSTARTER_ATF_THIN_H

#include "ThinAtfUserDefStructs.h"
#include "Common_atf_thin.h"
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
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	namespace cli
	{
		struct Protocol_MSG_LOBBY_TOURN_REG_INFO
		{
			// 414 - scriptId,numGames,tournRegInfoFlags
			UINT32 scriptId;
			BYTE numGames;
			UINT32 tournRegInfoFlags;

			Protocol_MSG_LOBBY_TOURN_REG_INFO();
			void clear();
			bool equals(const Protocol_MSG_LOBBY_TOURN_REG_INFO&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_LOBBY_TOURN_REG_INFO; }
			static const char *getMsgIdString() { return "MSG_LOBBY_TOURN_REG_INFO"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_CHECKIN
		{
			//! s444s144M()4184< <FIXME>CommonThin::AtfShared::UserTicketDataBase</FIXME> > - userId,buyInRM,buyInPlay,buyInFpp,tournPwd,useTicket,buyInT,buyInW,M(),scriptId,numGames,admissionPrice,numAdmissions< <FIXME>CommonThin::AtfShared::UserTicketDataBase</FIXME> >
			PString userId;
			INT32 buyInRM;
			INT32 buyInPlay;
			INT32 buyInFpp;
			PString tournPwd;
			BYTE useTicket;
			INT32 buyInT;
			INT32 buyInW;	// only 0 is allowed
			ThinAtf::CopyableMsgBody currencyContext;	// we switched to an empty msgbody because trying to buy into a tournament with a user that does not have auto convert currency set and sending a non-empty currencyContext will return an error
			UINT32 scriptId;
			BYTE numGames;
			INT64 admissionPrice;
			ThinAtf::LAtfVector< CommonThin::AtfShared::UserTicketDataBase, 4 > admissions;

			Protocol_MSG_SNG_POOL_CHECKIN();
			void clear();
			bool equals(const Protocol_MSG_SNG_POOL_CHECKIN&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_SNG_POOL_CHECKIN; }
			static const char *getMsgIdString() { return "MSG_SNG_POOL_CHECKIN"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_SNG_POOL_CHECKIN(Protocol_MSG_SNG_POOL_CHECKIN&&);
			Protocol_MSG_SNG_POOL_CHECKIN& operator=(Protocol_MSG_SNG_POOL_CHECKIN&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_SNG_POOL_CHECKIN(const Protocol_MSG_SNG_POOL_CHECKIN&) = default;
			Protocol_MSG_SNG_POOL_CHECKIN& operator=(const Protocol_MSG_SNG_POOL_CHECKIN&) = default;
#endif
#endif
		};

		struct Protocol_MSG_SNG_POOL_CHECKOUT
		{
			// 4 - scriptId
			UINT32 scriptId;

			Protocol_MSG_SNG_POOL_CHECKOUT();
			void clear();
			bool equals(const Protocol_MSG_SNG_POOL_CHECKOUT&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_SNG_POOL_CHECKOUT; }
			static const char *getMsgIdString() { return "MSG_SNG_POOL_CHECKOUT"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
		};

		struct Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER
		{
			// s - userId
			PString userId;	// client bug: can send email instead of userId, change limit to 100

			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER();
			void clear();
			bool equals(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_SNG_POOL_WHERE_IS_PLAYER; }
			static const char *getMsgIdString() { return "MSG_SNG_POOL_WHERE_IS_PLAYER"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER(Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER&&);
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER& operator=(Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER&) = default;
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER& operator=(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER&) = default;
#endif
#endif
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
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			UserTicketDataATF(UserTicketDataATF&&);
			UserTicketDataATF& operator=(UserTicketDataATF&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			UserTicketDataATF(const UserTicketDataATF&) = default;
			UserTicketDataATF& operator=(const UserTicketDataATF&) = default;
#endif
#endif
		};

		struct Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY
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
			ThinAtf::CopyableMsgBody currencyContext;
			ThinAtf::CopyableMsgBody clientCurrencyContext;
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
			ThinAtf::LAtfVector< UserTicketDataATF, 4 > effectiveAdmissions;
			UINT64 tournFlags2;

			Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY();
			void clear();
			bool equals(const Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_LOBBY_TOURN_REG_INFO_REPLY; }
			static const char *getMsgIdString() { return "MSG_LOBBY_TOURN_REG_INFO_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY(Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY&&);
			Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY& operator=(Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY(const Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY&) = default;
			Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY& operator=(const Protocol_MSG_LOBBY_TOURN_REG_INFO_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_SNG_POOL_CHECKIN_REPLY
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
			void clear();
			bool equals(const Protocol_MSG_SNG_POOL_CHECKIN_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_SNG_POOL_CHECKIN_REPLY; }
			static const char *getMsgIdString() { return "MSG_SNG_POOL_CHECKIN_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_SNG_POOL_CHECKIN_REPLY(Protocol_MSG_SNG_POOL_CHECKIN_REPLY&&);
			Protocol_MSG_SNG_POOL_CHECKIN_REPLY& operator=(Protocol_MSG_SNG_POOL_CHECKIN_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_SNG_POOL_CHECKIN_REPLY(const Protocol_MSG_SNG_POOL_CHECKIN_REPLY&) = default;
			Protocol_MSG_SNG_POOL_CHECKIN_REPLY& operator=(const Protocol_MSG_SNG_POOL_CHECKIN_REPLY&) = default;
#endif
#endif
		};

		struct Protocol_MSG_SNG_POOL_CHECKOUT_REPLY
		{
			// 421 - scriptId,errCode,cancelledGames
			UINT32 scriptId;
			INT16 errCode;
			BYTE cancelledGames;

			Protocol_MSG_SNG_POOL_CHECKOUT_REPLY();
			void clear();
			bool equals(const Protocol_MSG_SNG_POOL_CHECKOUT_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_SNG_POOL_CHECKOUT_REPLY; }
			static const char *getMsgIdString() { return "MSG_SNG_POOL_CHECKOUT_REPLY"; }
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
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			SngGameBody(SngGameBody&&);
			SngGameBody& operator=(SngGameBody&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			SngGameBody(const SngGameBody&) = default;
			SngGameBody& operator=(const SngGameBody&) = default;
#endif
#endif
		};

		struct Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY
		{
			//! 2 <FIXME_COND_IF> s <FIXME_COND_ELSE> 4<M(414s4441111s4s8)> <FIXME_COND_ENDIF>  - errCode <FIXME_COND_IF> ,errMsg <FIXME_COND_ELSE> ,numSngs<M(scriptId,numGames,tournId,sampleAddress,buyIn,fppBuyIn,rake,game,hiLo,structure,isPlayMoney,currency,scalePM,name,tournFlags2)> <FIXME_COND_ENDIF> 
			INT16 errCode;

			PString errMsg;

			ThinAtf::LAtfVector< SngGameBody, 4 > sngs;

			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY();
			void clear();
			bool equals(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY&) const;
			const char *asTraceString(PString& _buf) const;
			void parseMsg(CommMsgParser& _parser);
			void composeMsg(CommMsgBody& _body) const;

			static UINT32 getMsgId() { return MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY; }
			static const char *getMsgIdString() { return "MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY"; }
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY(Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY&&);
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY& operator=(Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY&) = default;
			Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY& operator=(const Protocol_MSG_SNG_POOL_WHERE_IS_PLAYER_REPLY&) = default;
#endif
#endif
		};
	};
};

#endif
