/**
 * TournCommonStructs.h
 *
 * This file was auto-generated from TournCommonStructs.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor TournCommonStructs.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_TOURNCOMMONSTRUCTS_H
#define ATF_PROTOCOL_DEFFILE_TOURNCOMMONSTRUCTS_H


#include "UserDefStructs.h"

namespace TournCommon
{
	struct P_Event
	{
		// 22 - startNum,stopNum
		UINT16 startNum;
		UINT16 stopNum;

		P_Event();
		void clear();
		bool equals(const P_Event&) const;
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

	struct Error_InsufficientFunds32	// obsolete
	{
		// 4b14sbM()M(8411bs) - minBuyIn,enoughMoneyInOtherCurrency,clientServiceFlag,availableChips,gameCurrency,isAutoConversionOn,M(),M(tournFlags2,bounty,handType,structure,isHiLo,name)
		INT32 minBuyIn;
		bool enoughMoneyInOtherCurrency;
		BYTE clientServiceFlag;
		INT32 availableChips;
		PString gameCurrency;
		bool isAutoConversionOn;
		Atf::CopyableMsgBody clientCurrencyContext;

		INT64 tournFlags2;
		UINT32 bounty;
		BYTE handType;
		BYTE structure;
		bool isHiLo;
		PString name;

		Error_InsufficientFunds32();
		void clear();
		bool equals(const Error_InsufficientFunds32&) const;
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
		Error_InsufficientFunds32(Error_InsufficientFunds32&&);
		Error_InsufficientFunds32& operator=(Error_InsufficientFunds32&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Error_InsufficientFunds32(const Error_InsufficientFunds32&) = default;
		Error_InsufficientFunds32& operator=(const Error_InsufficientFunds32&) = default;
#endif
#endif

	private: // helper functions
		void parseAnonymousMsgBody0(CommMsgParser& _parser);
		static void validateAnonymousMsgBody0(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};

	struct Error_InsufficientFunds
	{
		// 8b18sbM()M(8811bs) - minBuyIn,enoughMoneyInOtherCurrency,clientServiceFlag,availableChips,gameCurrency,isAutoConversionOn,M(),M(tournFlags2,bounty,handType,structure,isHiLo,name)
		INT64 minBuyIn;
		bool enoughMoneyInOtherCurrency;
		BYTE clientServiceFlag;
		INT64 availableChips;
		PString gameCurrency;
		bool isAutoConversionOn;
		Atf::CopyableMsgBody clientCurrencyContext;

		INT64 tournFlags2;
		INT64 bounty;
		BYTE handType;
		BYTE structure;
		bool isHiLo;
		PString name;

		Error_InsufficientFunds();
		void clear();
		bool equals(const Error_InsufficientFunds&) const;
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
		Error_InsufficientFunds(Error_InsufficientFunds&&);
		Error_InsufficientFunds& operator=(Error_InsufficientFunds&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Error_InsufficientFunds(const Error_InsufficientFunds&) = default;
		Error_InsufficientFunds& operator=(const Error_InsufficientFunds&) = default;
#endif
#endif

	private: // helper functions
		void parseAnonymousMsgBody0(CommMsgParser& _parser);
		static void validateAnonymousMsgBody0(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
	};

	struct TournSpendLimit
	{
		// M(b[44]) - M(limitImposed[spendAmount,spendLimit])
		bool limitImposed;

		INT32 spendAmount;
		INT32 spendLimit;

		TournSpendLimit();
		void clear();
		bool equals(const TournSpendLimit&) const;
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
		TournSpendLimit(TournSpendLimit&&);
		TournSpendLimit& operator=(TournSpendLimit&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		TournSpendLimit(const TournSpendLimit&) = default;
		TournSpendLimit& operator=(const TournSpendLimit&) = default;
#endif
#endif
	};
};

#endif