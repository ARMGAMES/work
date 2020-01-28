#ifndef aamstransport_h_included
#define aamstransport_h_included

#include "pyrtime.h"
#include "basestructs.h"

//
// AAMS communication
//
#define BEGINNING_OF_SESSION "200"
#define RIGHT_TO_PARTICIPATE "220"
#define CANCELLATION_OF_RIGHT_TO_PARTICIPATE "230"
#define PRIZE_SCHEDULE "240"
#define CONFIRMATION_OF_THE_SESSION "250"
#define LIST_OF_WINNERS "260"
#define CREDITING_OF_WINNINGS "280"
#define CREDITING_OF_REFUNDS "290"
#define END_OF_SESSION "300"
#define CANCELLATION_OF_SESSION "320"

#define RING_START_OF_SESSION "600"
#define RING_PURCHASE_OF_PARTICIPATION_RIGHT "620"
#define RING_END_OF_PARTICIPATION "630"
// #define RING_FINAL_STAKE_CREDITING "680"		// NO LONGER A REQUIRED MESSAGE
#define RING_END_OF_SESSION "700"

#define REPORT_GAME_BEING_PLAYED "780"
#define REPORT_GAME_SESSION_BALANCED "790"

#define ACCOUNTING_DATA_REQUEST "800"
#define UPDATE_END_OF_SESSION "810"

#define REPORT_SOFTWARE_SIGNATURE "830"

#define REPORT_SOFTWARE_SIGNATURE_831 "831"

//CAS-3400: Auxiliary (Casino) AAMS message types
#define AUX_RING_START_OF_SESSION "400"
#define AUX_RING_PURCHASE_OF_PARTICIPATION_RIGHT "420"
#define AUX_RING_END_OF_PARTICIPATION "430"
#define AUX_RING_END_OF_SESSION "500"

#define AUX_REPORT_GAME_BEING_PLAYED "580"
#define AUX_REPORT_GAME_SESSION_BALANCED "590"

enum AamsProtocol
{
	ONE_BYTE_LEN = 1,
	TWO_BYTE_LEN = 2,
	MESSAGE_TYPE_LEN = 4,
	GAMING_CODE_LEN = 6,
	CIRCUIT_CODE_LEN = 6,
	IP_ADDRESS_LEN = 15,
	TRANSACTION_CODE_LEN = 16,
	GAMING_SESSION_ID_LEN = 16,
	HAND_ID_LEN = 16,
	AAMS_DATETIME_LEN = 14,
	AAMS_DATE_LEN = 8,
	SESSION_TICKET_LEN = 16,
	SESSION_ATTRIBUTE_LEN = 3,
	SESSION_ATTRIBUTE_VALUE_LEN = 10,
	SESSION_ATTRIBUTE_VALUE_LEN_V2 = 16,
	DATE_LEN = 6,
	REFERENCE_DATE_LEN = 8,
	SOFTWARE_FILENAME_LEN = 40,
	SOFTWARE_SIGNATURE_LEN = 40,

	MAX_ACCOUNT_LEN = 35,
	MAX_ACCOUNT_LEN_V2 = 20,

	HEADER_LENGTH_V2 = 42, // according to spec
	HEADER_LENGTH = 52,

	RESULT_CODE_LEN = 2,
	MESSAGE_200_REPLY_LENGTH = 18,
	MESSAGE_220_REPLY_LENGTH = 24,

	MESSAGE_600_REPLY_LENGTH = 18,
	MESSAGE_620_REPLY_LENGTH = 24,
	MESSAGE_630_REPLY_LENGTH = 2,
	MESSAGE_680_REPLY_LENGTH = 2,
	MESSAGE_700_REPLY_LENGTH = 2,

	//CAS-3400: Auxiliary reply message length
	AUX_MESSAGE_400_REPLY_LENGTH = 18,
	AUX_MESSAGE_420_REPLY_LENGTH = 24,
	AUX_MESSAGE_430_REPLY_LENGTH = 8,
	AUX_MESSAGE_500_REPLY_LENGTH = 2,
	AUX_MESSAGE_580_REPLY_LENGTH = 2,
	AUX_MESSAGE_590_REPLY_LENGTH = 2,
};

enum AmmsGameType
{
	aamsGameTypeFile = 0,
	aamsGameTypeTourn = 1,
	aamsGameTypeTable = 3,
	aamsGameTypeCasinoTable = 2,	//CAS-3400: casino cash game game type
};

#define IT_TRANSPORT_Q_A_DELTA 1000

// The AAMS gateway submits a request to the AAMS site
#define IT_TRANSPORT_Q_SEND_MESSAGE		100 // 4sB - regId, messageType, aamsRequest
// The transport module replies with AAMS response
#define IT_TRANSPORT_A_SEND_MESSAGE		(IT_TRANSPORT_Q_SEND_MESSAGE + IT_TRANSPORT_Q_A_DELTA) // 46[s|B] - reqId, errCode[errDescr|aamsReply]

#endif // aamstransport_h_included