// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef converter_h_included
#define converter_h_included

#define MSG_CONVERTER_GET_ALL_RATES 101 // 4t reqId,time
#define MSG_CONVERTER_GET_ALL_RATES_REPLY 102 // 42[s|4<M(s4)>] reqId, errCode[errDescr|numCurrencies<M(currency,conversionRate)>]

#define MSG_CONVERTER_GET_RATE 103 // 4ts reqId,time,currency
#define MSG_CONVERTER_GET_RATE_REPLY 104 // 42[s|4] reqId,errCode[errDescr|conversionRate]

#define MSG_CONVERTER_CONVERT_AMOUNT 105 // 4t8ssbb reqId,time,srcAmount,srcCurrency,destCurrency,applyRounding,roundUp
#define MSG_CONVERTER_CONVERT_AMOUNT_REPLY 106 // 42[s|88] reqId,errCode[errDescr|destAmount,conversionRate]

#define MSG_CONVERTER_CONVERT_BATCH_AMOUNTS 107 // 4tbb4<M(8ss)> reqId,time,applyRounding,roundUp,numConversions <M(srcAmount,srcCurrency,destCurrency)>
#define MSG_CONVERTER_CONVERT_BATCH_AMOUNTS_REPLY 108 // 42[s|4<M(2[s|88])>] reqId,errCode[errDescr|numConversions<M(errorCode[errDescr|destAmount,conversionRate])>]

#define MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS 109 // 4t reqId,time
#define MSG_CONVERTER_GET_DATA_FOR_CLIENT_CONVERSIONS_REPLY 110 // 42[s|M] reqId errCode[errorDescription|init data message body)]

#define MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE 111 // 4t8ssbb reqId,time,srcAmount,srcCurrency,destCountryCode,applyRounding,roundUp
#define MSG_CONVERTER_CONVERT_AMOUNT_BY_COUNTRY_CODE_REPLY 112 // 42[s|8s8] reqId,errCode[errDescr|destAmount,destCurrency,conversionRate]


#endif	