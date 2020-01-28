/**
 * converter_atf_thin.h
 *
 * This file was auto-generated from converter_atf.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin converter_atf.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_CONVERTER_ATF_THIN_H
#define ATF_PROTOCOL_DEFFILE_CONVERTER_ATF_THIN_H

#include "converter.h"
#include "pyrtime.h"


#include "ThinAtfUserDefStructs.h"

namespace converter
{
	class ProtocolFactory
	{
	public:
		static bool validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded);
	};


	struct CountryCurrency
	{
		// M(ss) - M(countryCode,localCurrencyIsoCode)
		PString countryCode;
		PString localCurrencyIsoCode;

		CountryCurrency();
		void clear();
		bool equals(const CountryCurrency&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

		// @Override
		CountryCurrency(PString code, PString isoCode);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CountryCurrency(CountryCurrency&&);
		CountryCurrency& operator=(CountryCurrency&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		CountryCurrency(const CountryCurrency&) = default;
		CountryCurrency& operator=(const CountryCurrency&) = default;
#endif
#endif
	};

	struct CurrencyConvertRate
	{
		// M(s4) - M(currency,rate)
		PString currency;
		INT32 rate;

		CurrencyConvertRate();
		void clear();
		bool equals(const CurrencyConvertRate&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CurrencyConvertRate(CurrencyConvertRate&&);
		CurrencyConvertRate& operator=(CurrencyConvertRate&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		CurrencyConvertRate(const CurrencyConvertRate&) = default;
		CurrencyConvertRate& operator=(const CurrencyConvertRate&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_GET_ALL_RATES	//101
	{
		// t - time
		SrvTime time;

		Protocol_MSG_CONVERTER_GET_ALL_RATES();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_GET_ALL_RATES&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_GET_ALL_RATES; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_GET_ALL_RATES"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY	//102
	{
		// 2[s|4<M(s4)>] - errCode[errorDescription|numRates<M(currency,rate)>]
		INT16 errCode;

		PString errorDescription;

		ThinAtf::LAtfVector< CurrencyConvertRate, 4 > rates;

		Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_GET_ALL_RATES_REPLY; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_GET_ALL_RATES_REPLY"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY(Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY&&);
		Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY& operator=(Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY(const Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY&) = default;
		Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY& operator=(const Protocol_MSG_CONVERTER_GET_ALL_RATES_REPLY&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_GET_RATE	//103
	{
		// ts - time,currency
		SrvTime time;
		PString currency;

		Protocol_MSG_CONVERTER_GET_RATE();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_GET_RATE&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_GET_RATE; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_GET_RATE"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_GET_RATE(Protocol_MSG_CONVERTER_GET_RATE&&);
		Protocol_MSG_CONVERTER_GET_RATE& operator=(Protocol_MSG_CONVERTER_GET_RATE&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_GET_RATE(const Protocol_MSG_CONVERTER_GET_RATE&) = default;
		Protocol_MSG_CONVERTER_GET_RATE& operator=(const Protocol_MSG_CONVERTER_GET_RATE&) = default;
#endif
#endif
	};
	struct Protocol_MSG_CONVERTER_GET_RATE_REPLY	//104
	{
		// 2[s|4] - errCode[errorDescription|rate]
		INT16 errCode;

		PString errorDescription;

		INT32 rate;

		Protocol_MSG_CONVERTER_GET_RATE_REPLY();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_GET_RATE_REPLY&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_GET_RATE_REPLY; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_GET_RATE_REPLY"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_GET_RATE_REPLY(Protocol_MSG_CONVERTER_GET_RATE_REPLY&&);
		Protocol_MSG_CONVERTER_GET_RATE_REPLY& operator=(Protocol_MSG_CONVERTER_GET_RATE_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_GET_RATE_REPLY(const Protocol_MSG_CONVERTER_GET_RATE_REPLY&) = default;
		Protocol_MSG_CONVERTER_GET_RATE_REPLY& operator=(const Protocol_MSG_CONVERTER_GET_RATE_REPLY&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_CONVERT_AMOUNT	//105
	{
		// t8ssbb - time,srcCurrencyAmount,srcCurrencyType,destCurrencyType,applyRounding,roundUp
		SrvTime time;
		INT64 srcCurrencyAmount;
		PString srcCurrencyType;
		PString destCurrencyType;
		bool applyRounding;
		bool roundUp;

		Protocol_MSG_CONVERTER_CONVERT_AMOUNT();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_CONVERT_AMOUNT; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_CONVERT_AMOUNT"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT(Protocol_MSG_CONVERTER_CONVERT_AMOUNT&&);
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT& operator=(Protocol_MSG_CONVERTER_CONVERT_AMOUNT&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT&) = default;
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT& operator=(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT&) = default;
#endif
#endif
	};
	struct Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY	//106
	{
		// 2[s|88] - errCode[errorDescription|destAmount,rate]
		INT16 errCode;

		PString errorDescription;

		INT64 destAmount;
		INT64 rate;

		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_CONVERT_AMOUNT_REPLY; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_CONVERT_AMOUNT_REPLY"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY(Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY&&);
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY& operator=(Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY&) = default;
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY& operator=(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_REPLY&) = default;
#endif
#endif
	};

	struct CurrencyConvertBatchRequest
	{
		// M(8ss) - M(srcCurrencyAmount,srcCurrencyType,destCurrencyType)
		INT64 srcCurrencyAmount;
		PString srcCurrencyType;
		PString destCurrencyType;

		CurrencyConvertBatchRequest();
		void clear();
		bool equals(const CurrencyConvertBatchRequest&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CurrencyConvertBatchRequest(CurrencyConvertBatchRequest&&);
		CurrencyConvertBatchRequest& operator=(CurrencyConvertBatchRequest&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		CurrencyConvertBatchRequest(const CurrencyConvertBatchRequest&) = default;
		CurrencyConvertBatchRequest& operator=(const CurrencyConvertBatchRequest&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS	//107
	{
		// tbb4<M(8ss)> - time,applyRounding,roundUp,numConvertRequests<M(srcCurrencyAmount,srcCurrencyType,destCurrencyType)>
		SrvTime time;
		bool applyRounding;
		bool roundUp;
		ThinAtf::LAtfVector< CurrencyConvertBatchRequest, 4 > convertRequests;

		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_CONVERT_BATCH_AMOUNTS; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_CONVERT_BATCH_AMOUNTS"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS(Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS&&);
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS& operator=(Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS(const Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS&) = default;
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS& operator=(const Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS&) = default;
#endif
#endif
	};

	struct CurrencyConvertBatchReply
	{
		// M(2[s|88]) - M(errCode[errorDescription|destAmount,rate])
		INT16 errCode;

		PString errorDescription;

		INT64 destAmount;
		INT64 rate;

		CurrencyConvertBatchReply();
		void clear();
		bool equals(const CurrencyConvertBatchReply&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CurrencyConvertBatchReply(CurrencyConvertBatchReply&&);
		CurrencyConvertBatchReply& operator=(CurrencyConvertBatchReply&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		CurrencyConvertBatchReply(const CurrencyConvertBatchReply&) = default;
		CurrencyConvertBatchReply& operator=(const CurrencyConvertBatchReply&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY	//108
	{
		// 2[s|4<M(2[s|88])>] - errCode[errorDescription|numConversionResults<M(errCode[errorDescription|destAmount,rate])>]
		INT16 errCode;

		PString errorDescription;

		ThinAtf::LAtfVector< CurrencyConvertBatchReply, 4 > conversionResults;

		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY(Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY&&);
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY& operator=(Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY(const Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY&) = default;
		Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY& operator=(const Protocol_MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS	//109
	{
		// t - time
		SrvTime time;

		Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);
	};

	struct CurrencyConvertData
	{
		// M(84<8>4<M(s4)>) - M(roundingFactorsCommonRatio,numRoundingFactors<roundingFactors>,numRates<M(currency,rate)>)
		INT64 roundingFactorsCommonRatio;
		ThinAtf::LAtfVector< INT64, 4 > roundingFactors;
		ThinAtf::LAtfVector< CurrencyConvertRate, 4 > rates;

		CurrencyConvertData();
		void clear();
		bool equals(const CurrencyConvertData&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CurrencyConvertData(CurrencyConvertData&&);
		CurrencyConvertData& operator=(CurrencyConvertData&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		CurrencyConvertData(const CurrencyConvertData&) = default;
		CurrencyConvertData& operator=(const CurrencyConvertData&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY	//110
	{
		// 2[s|M(84<8>4<M(s4)>)] - errCode[errorDescription|M(roundingFactorsCommonRatio,numRoundingFactors<roundingFactors>,numRates<M(currency,rate)>)]
		INT16 errCode;

		PString errorDescription;

		CurrencyConvertData payload;

		Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY(Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY&&);
		Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY& operator=(Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY(const Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY&) = default;
		Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY& operator=(const Protocol_MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY&) = default;
#endif
#endif
	};

	struct Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE	//111
	{
		// t8ssbb - time,srcCurrencyAmount,srcCurrencyType,destCountryCode,applyRounding,roundUp
		SrvTime time;
		INT64 srcCurrencyAmount;
		PString srcCurrencyType;
		PString destCountryCode;
		bool applyRounding;
		bool roundUp;

		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE(Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE&&);
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE& operator=(Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE&) = default;
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE& operator=(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE&) = default;
#endif
#endif
	};
	struct Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY	//112
	{
		// 2[s|8s8] - errCode[errorDescription|destAmount,destCurrencyType,rate]
		INT16 errCode;

		PString errorDescription;

		INT64 destAmount;
		PString destCurrencyType;
		INT64 rate;

		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY();
		void clear();
		bool equals(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY&) const;
		const char *asTraceString(PString& _buf) const;
		void parseMsg(CommMsgParser& _parser);
		void composeMsg(CommMsgBody& _body) const;

		static UINT32 getMsgId() { return MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY; }
		static const char *getMsgIdString() { return "MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY"; }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY(Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY&&);
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY& operator=(Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY&) = default;
		Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY& operator=(const Protocol_MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY&) = default;
#endif
#endif
	};
};

#endif