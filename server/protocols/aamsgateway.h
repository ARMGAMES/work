#ifndef aamsgateway_h_included
#define aamsgateway_h_included

#include "commlib.h"
#include "pyrtime.h"
#include "dbm.h"
#include "clientplatforms.h"
#include "brandutils.h"
#include "shared.h"

//
// Gateway specific error codes
// Actual AAMS error codes >= 1000
//
#define AAMS_NO_ERROR 0
#define AAMS_BATCH_ERROR 100
#define AAMS_CANNOT_PROCESS_ERROR 101
#define AAMS_CANNOT_CREATE_SESSION 102
#define AAMS_NOT_AVAILABLE 200
#define AAMS_SESSION_CLOSED 201
#define AAMS_REPEATED_SHUTDOWN_REQUEST 300


// PYR-18531
#define AAMS_EXCEED_GAME_LIMIT	1426

#define AAMS_100_PERCENT 10000
#define AAMS_TOURN_SPEND_LIMIT 25000 // 250 Euro // TODO: implement configurable restriction. Must be set back to 100 EUR for AAMS v1 protocol!!!

//
// AAMS Gateway
// PokerStars -> AAMS Italian authority
//

#define IT_Q_A_DELTA 1000

// PYR-18708
//
#define EPIS_GAME_TYPE	"extended"
#define EPIS_GAME_CLASS	"classa" // PYR-19436 - they wanted classb, now they want classa again
#define EPIS_GAME_ID	""

enum eReacquisitionType
{
	eReacquisitionTypeNone = 0,
	eReacquisitionTypeRebuyIn = 1,
	eReacquisitionTypeAddOn = 2,
	eReacquisitionTypeRebuyInAddOn = 3
};

enum eGatewayStatus
{
	eGatewayStatusUndefined = 0,
	eGatewayStatusNormal = 1,
	eGatewayStatusNotAvailable = 2
};

enum eGatewayStatusAction
{
	eGatewayStatusActionGet = 1,
	eGatewayStatusActionSet = 2
};


// Lobby asks for user's approval in tournament.
// A gaming session will be created on AAMS side for the first message in the tournament.
#define IT_Q_APPROVE_USER		101 // 44T44<ss>4s4ss4s48444 - regId, tournId, serverTime, tournCost, numAttributes<attribute, value>, game, userId, userIntId, country, state, cost, ipAddress, flags, flags2,clientDevice,osId,appTypeId
// The gateway replies with AAMS response
#define IT_A_APPROVE_USER		(IT_Q_APPROVE_USER + IT_Q_A_DELTA) // 46[s|4ss] - reqId, errCode[errDescr|sessionId, aamsTicketId, aamsSessionId]

// Lobby asks for user's unregistration in tournament.
#define IT_Q_UNREGISTER_USER	102 // 44Ts4 - regId, tournId, serverTime, userId, refundAmount
// The gateway replies with AAMS response
#define IT_A_UNREGISTER_USER	(IT_Q_UNREGISTER_USER + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

// Lobby asks for tournament confirmation before it begins.
#define IT_Q_CONFIRM_TOURN		103 // 44Tb - regId, tournId, serverTime, failIfAAMSNotAvailable
// The gateway replies with AAMS response
#define IT_A_CONFIRM_TOURN		(IT_Q_CONFIRM_TOURN + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

// Lobby reports tournament completion
#define IT_Q_TOURN_ENDED		104 // 44T444<4>4<s48> - regId, tournId, serverTime, prizePoolPercentage, prizePool, numPrizes<prizeAmount>, numWinners<userId, prizeAmount, flags2>
// The gateway confirms that the prize schedule accepted for transmission
#define IT_A_TOURN_ENDED		(IT_Q_TOURN_ENDED + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

// Lobby reports tournament cancellation via refunds
#define IT_Q_TOURN_CANCELLED	105 // 44T4<s4> - regId, tournId, serverTime, numRefunds<userId, refundAmount>
// The gateway confirms that the tournament refunds accepted for transmission
#define IT_A_TOURN_CANCELLED	(IT_Q_TOURN_CANCELLED + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

#define IT_Q_GET_USERS			106 // 44T- regId, tournId, serverTime // read only
#define IT_A_GET_USERS			(IT_Q_GET_USERS + IT_Q_A_DELTA) // 46[s|4<ss>s] - reqId, errCode[errDescr|nUsers<userId, ipAddress>, aamsTournId]

// Lobby asks user's tournaments. Tourns with error codes are not returns
#define IT_Q_GET_USER_TOURNS	107 // 4sb - regId, userId, notFinishedOnly // read only
#define IT_A_GET_USER_TOURNS	(IT_Q_GET_USER_TOURNS + IT_Q_A_DELTA) // 46[s|4<4M>] - reqId, errCode[errDescr|nTourns<tournId, infoBody>] where infoBody: ss444<M> - AAMSID, AAMSTicket, cost, gwStatus, numReacquisitions<reacquisitionBody> where reacquisitionBody: sT44 - AAMSTicket, when, type, cost

// Lobby asks for user's buy-in or add-on approval in tournament.
#define IT_Q_REBUY_ADDON_USER	108 // 44Ts4448444 - regId, tournId, serverTime, userId, cost, eReacquisitionType, uniqueId, flags2,clientDevice,osId,appTypeId
// The gateway replies with AAMS response
#define IT_A_REBUY_ADDON_USER	(IT_Q_REBUY_ADDON_USER + IT_Q_A_DELTA) // 46[s|4ss] - reqId, errCode[errDescr|sessionId, aamsTicketId, aamsSessionId]

// Lobby reports tournament was not allowed by AAMS.
// This message is expected to be sent when IT_A_CONFIRM_TOURN returns an error.
#define IT_Q_TOURN_NOT_ALLOWED	199 // 44T - regId, tournId, serverTime
// The gateway confirms that the session is cancelled by AAMS
#define IT_A_TOURN_NOT_ALLOWED	(IT_Q_TOURN_NOT_ALLOWED + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]


// Lobby forwards Admin's request
#define IT_Q_ADMIN_REQUEST		200 // 4sM - regId, adminId, requestBody
// The gateway processes the request
#define IT_A_ADMIN_REQUEST		(IT_Q_ADMIN_REQUEST + IT_Q_A_DELTA) // 46[s|M] - reqId, errCode[errDescr|replyBody]

enum eAamsGatewayType
{
	eAGTTournament = 0,
	eAGTTable = 1,
	eAGTBlitz = 2,
	eAGTMisc = 3,
};

//
// Admin messages
//

// IT_Q_TOURN_ENDED sent by Admin
// requestBody: 44T444<4>4<s4s> - IT_Q_TOURN_ENDED, IT_Q_TOURN_ENDED message without reqId
// replyBody: empty

// IT_Q_TOURN_CANCELLED sent by Admin
// requestBody: 44T4<s4s> - IT_Q_TOURN_CANCELLED, IT_Q_TOURN_CANCELLED message without reqId
// replyBody: empty

// This message should be used to recover session processing when it's been interrupted (the gateway went down) or failed.
// If failed and should be tried again, an appropriate statuses has to be assigned to session and session users
// in order for recover to succeed.
// This message only works for session finalization such as processWinners and processRefunds
#define IT_ADMIN_MSG_RECOVER_SESSION 222
// requestBody: 44 - IT_ADMIN_MSG_RECOVER_SESSION, tournId
// replyBody: empty

// This message allows to create an instance of AAMS transport module
#define IT_ADMIN_MSG_CREATE_TRANSPORT_MODULE 333
// requestBody: 44 - IT_ADMIN_MSG_CREATE_TRANSPORT_MODULE, instanceIdx (instanceIdx == -1 means, create a new one)
// replyBody: empty

// This message allows to create an instance of PGAD transport module @dld Apr5
#define IT_ADMIN_MSG_CREATE_PGAD_MODULE 334
// requestBody: 44 - IT_ADMIN_MSG_CREATE_PGAD_MODULE, instanceIdx (instanceIdx == -1 means, create a new one)
// replyBody: empty

// This message allows to create an instance of PSAPI transport module
#define IT_ADMIN_MSG_CREATE_PSAPI_MODULE 335
// requestBody: 44 - IT_ADMIN_MSG_CREATE_PSAPI_MODULE, instanceIdx (instanceIdx == -1 means, create a new one)
// replyBody: empty

// This message manages AAMS gateway status
#define IT_ADMIN_MSG_MANAGE_STATUS 350
// requestBody: 444 - IT_ADMIN_MSG_MANAGE_STATUS, eGatewayStatusAction, newStatus (newStatus is ignored for eGatewayStatusActionGet)
// replyBody: 4 - currentStatus

// [AN 2010-03-30] PYR-14365
// Tell AAMS Gateway to ignore all incoming messages - do not process them and do not acknowledge them
// making them accumulate in client's g-connections. Once the AAMS Gateway gets shut down by the operator
// and a new instance of it is started - messages accumulated by the client will be processed by the new instance
// of AAMS gateway
//
// MG: obsolete - use MSG_ADMIN_PREPARE_FOR_SHUTDOWN - see PYR-19591
// MG TODO: comment out this message after Admin part is ready for Aug'30 build
#define IT_ADMIN_PREPARE_FOR_SHUTDOWN 351
// requestBody: 4 - IT_ADMIN_PREPARE_FOR_SHUTDOWN
// replyBody: empty

//
// End of Admin messages

// Games Cropping messages
//

// request closed games for date
#define IT_Q_CLOSED_GAMES_DATE		400 //4(D) reqId, date
// The gateway processes the request
#define IT_A_CLOSED_GAMES_DATE		(IT_Q_CLOSED_GAMES_DATE + IT_Q_A_DELTA) //44<4<4>> reqId, numTourns, <numIds,<userIntId>>

// delete closed games for date
#define IT_Q_PROCESS_CLOSED_GAMES	401 //444<4> reqId, tournId, numRecords<UserIntId>
// The gateway processes the request
#define IT_A_PROCESS_CLOSED_GAMES	(IT_Q_PROCESS_CLOSED_GAMES + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

//
// End of Games Cropping messages

// Reporting hands for tables messages
//

// request tables that need to be reported for the specified date
#define IT_Q_REPORT_TABLES_DATE				402 //4(D) reqId, date
// The gateway processes the request
#define IT_A_REPORT_TABLES_DATE				(IT_Q_REPORT_TABLES_DATE + IT_Q_A_DELTA) //ReportTablesReply

// submit session hands portion for the day
#define IT_Q_PROCESS_GAME_SESSION_HANDS		403 // GameSessionHandsReport
// The gateway processes the request
#define IT_A_PROCESS_GAME_SESSION_HANDS		(IT_Q_PROCESS_GAME_SESSION_HANDS + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

// submit session summaries
#define IT_Q_PROCESS_GAME_SESSION_SUMMARY	404 // GameSessionSummariesReport
// The gateway processes the request
#define IT_A_PROCESS_GAME_SESSION_SUMMARY	(IT_Q_PROCESS_GAME_SESSION_SUMMARY + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

// get tickets for all the users that sat at the specified table for the day
#define IT_Q_SESSION_TICKETS				405 
// The gateway processes the request
#define IT_A_SESSION_TICKETS				(IT_Q_SESSION_TICKETS + IT_Q_A_DELTA) //46[s|n<o>] reqId, err_code[errDescr|numTickets<UserTicketFull>]

// request all open tables for which user summary is required
#define IT_Q_GET_ALL_OPEN_TABLES				406 //4(D) reqId, date
// The gateway processes the request
#define IT_A_GET_ALL_OPEN_TABLES				(IT_Q_GET_ALL_OPEN_TABLES + IT_Q_A_DELTA) //ReportTablesReply

// get tickets for all the users that sat at the specified table for the day
#define IT_Q_OPEN_SESSION_TICKETS			407 
// The gateway processes the request
#define IT_A_OPEN_SESSION_TICKETS			(IT_Q_OPEN_SESSION_TICKETS + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

// report software signature
#define IT_Q_REPORT_SOFTWARE_SIGNATURE		408 // 44<44ss> - reqId, numModules<type, element code, file name, signature>
#define IT_A_REPORT_SOFTWARE_SIGNATURE		(IT_Q_REPORT_SOFTWARE_SIGNATURE + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

// report software signature from INI file
#define IT_Q_REPORT_SOFTWARE_SIGNATURE_INI	409 // 4 - reqId
#define IT_A_REPORT_SOFTWARE_SIGNATURE_INI	(IT_Q_REPORT_SOFTWARE_SIGNATURE_INI + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

// get open tickets for all the users that sat at the specified table for the day
#define IT_Q_OPEN_SESSION_TICKETS_BY_TABLE_ID				410 
// The gateway processes the request
#define IT_A_OPEN_SESSION_TICKETS_BY_TABLE_ID				(IT_Q_OPEN_SESSION_TICKETS_BY_TABLE_ID + IT_Q_A_DELTA) //46[s|n<o>o] reqId, err_code[errDescr|numTickets<UserTicketFull>AamsSessionInfo]


// Accounting data request message (800)
// Admin requests data from AAMS
#define IT_Q_ACCOUNTING_DATA_REQUEST			420 // dd - fromDate, toDate
// The gateway processes the request
#define IT_A_ACCOUNTING_DATA_REQUEST			(IT_Q_ACCOUNTING_DATA_REQUEST + IT_Q_A_DELTA) // 46[s|4] - reqId, errCode[errDescr|numRecords]

// Updating of end-of-session date (810)
// Admin requests data from AAMS
#define IT_Q_UPDATE_END_OF_SESSION				421 // b4d - isTournament, objectId, endDate
// The gateway processes the request
#define IT_A_UPDATE_END_OF_SESSION				(IT_Q_UPDATE_END_OF_SESSION + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

// re-submit failed session summary 
#define IT_Q_PROCESS_GAME_SESSION_SUMMARY_DELAY	422 // sessionId
// The gateway processes the request
#define IT_A_PROCESS_GAME_SESSION_SUMMARY_DELAY	(IT_Q_PROCESS_GAME_SESSION_SUMMARY_DELAY + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

// get session ids for failed session summaries
#define IT_Q_GET_FAILED_SUMMARIES				423 // 4d4<4> - reqId, reportDate, numChipsTypes<chipsType>
// The gateway processes the request
#define IT_A_GET_FAILED_SUMMARIES				(IT_Q_GET_FAILED_SUMMARIES + IT_Q_A_DELTA) //46[s | 4<4>] reqId, err_code[errDescr | numSessions<sessionId>]

//PYR-20074
#define IT_Q_RING_SESSION_TICKETS_FOR_DATE		424 // reportDate
#define IT_A_RING_SESSION_TICKETS_FOR_DATE		(IT_Q_RING_SESSION_TICKETS_FOR_DATE + IT_Q_A_DELTA) //46[s | 4<4>] reqId, err_code[errDescr | numTickets<UserTicketFull>]

// PYR-49537
// report software signature from INI file. Message 831
#define IT_Q_REPORT_SOFTWARE_SIGNATURE_INI_831	425 // 4 - reqId
#define IT_A_REPORT_SOFTWARE_SIGNATURE_INI_831	(IT_Q_REPORT_SOFTWARE_SIGNATURE_INI_831 + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

//
// End of Reporting hands for tables messages

// report closed table
#define IT_Q_CLOSED_TABLE			501 //484<s444444> reqId,tableId,numUsers,<userId,userIntId,numHands,bankRoll,sumOfBets,collectedRake,wonAmount>
// The gateway processes the request
#define IT_A_CLOSED_TABLE			(IT_Q_CLOSED_TABLE + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]

// Side Table messages:
// don't believe this message is ever sent. when the first IT_Q_TABLE_USER_SIT is sent then the table gets created
#define IT_Q_TABLE_STARTED			601 //481144442t - reqId,tableId,maxSeats,tableType,lowStake,highStake,minChipsLimit,maxChipsLimit,maxRakePercent,srvTime
#define IT_A_TABLE_STARTED			(IT_Q_TABLE_STARTED + IT_Q_A_DELTA) //46[s|s4] - reqId,errCode[errDescr|itId,sessionId]

#define IT_Q_TABLE_USER_SIT			602 //4841444421s44sss48t444 - reqId,tableId,maxSeats,tableType,lowStake,highStake,minChipsLimit,maxChipsLimit,maxRakePercent,handType, userId,originalUserIntId,chips,ipAddr,country,state,uniqueId,flags2,seqTableHandNum,srvTime,clientDevice,osId,appTypeId
#define IT_A_TABLE_USER_SIT			(IT_Q_TABLE_USER_SIT + IT_Q_A_DELTA) //46[s|s4s] - reqId,errCode[errDescr|itId,sessionId,participateId]

#define IT_Q_TABLE_USER_PURCHASE	603 //48s44sss48t - reqId,tableId,userId,originalUserIntId,chips,ipAddr,country,state,uniqueId,flags2,seqTableHandNum,srvTime
#define IT_A_TABLE_USER_PURCHASE	(IT_Q_TABLE_USER_PURCHASE + IT_Q_A_DELTA) //46[s|s] - reqId,errCode[errDescr|participateId]

#define IT_Q_TABLE_USER_LEAVED		604 //48s4444448t1 - reqId,tableId,userId,originalUserIntId,numHands,bankRoll,sumOfBets,collectedRake,wonAmount,flags2,seqTableHandNum,srvTime,maxPlayers
#define IT_A_TABLE_USER_LEAVED		(IT_Q_TABLE_USER_LEAVED + IT_Q_A_DELTA) //4 - reqId

#define IT_Q_TABLE_CLOSED			605 //48t - reqId,tableId,srvTime
#define IT_A_TABLE_CLOSED			(IT_Q_TABLE_CLOSED + IT_Q_A_DELTA) //4 - reqId

#define IT_Q_CLOSED_RING_GAMES_DATE	606 //4(D) reqId, date
#define IT_A_CLOSED_RING_GAMES_DATE	(IT_Q_TABLE_CLOSED + IT_Q_A_DELTA) //4 - reqId

#define IT_Q_PROCESS_CLOSED_RING_GAMES	607 //444<st>4<48t>4<d> reqId, sessionId, num<userId,entered>, num<msgId,gwId,received>, num<reportDates>
#define IT_A_PROCESS_CLOSED_RING_GAMES	(IT_Q_PROCESS_CLOSED_RING_GAMES + IT_Q_A_DELTA) //46[s] reqId, err_code[errDescr]


// End of Side Table messages

struct ChangeAccountStatusRequest
{
    BYTE reason;
    const char* status;

    void compose(CommMsgBody& body) const
    {
        body
            .composeBYTE(reason)
            .composeString(status);
    }

    void parse(CommMsgParser& parser)
    {
        parser
            .parseBYTE(reason) 
            .parseString(status);
    }
};

enum ePgadOperationType
{
    pgadOperationUnknown = 0,
    pgadOperationBonus,
    pgadOperationBonusCancel,
    pgadOperationDeposit,
    pgadOperationDepositCancel,
    pgadOperationWithdrawal,
    pgadOperationWithdrawalCancel,
	pgadOperationServiceCosts, // #22732
	pgadOperationBonusConversion, // PYR-48975
};

inline bool isPgadBonusType( ePgadOperationType type )
{
    return ( type == pgadOperationBonus || type == pgadOperationBonusCancel );
}

inline bool isPgadCashinType( ePgadOperationType type )
{
    return ( type == pgadOperationDeposit || type == pgadOperationWithdrawalCancel );
}

struct PgadTransactData
{
    // PYR-19360.
    // Depending on the transid source there are different configurable offsets in AAMS Gateway.
    // To make sure that trans id reported to AAMS are unique we put transids from different
    // source into different non-overlapping numeric spaces.
    // For example transids from TRANSACTS table are no less than 30e+9 so their offset is 0 in production,
    // transids from TOURNTICKETSAUDIT table are small so they will be offset by 10e+9,
    // transids for other transactions (not from any table) start from 0.
    // If we will have other sources for transids reported to AAMS we need to create a new enum member
    // as well as new configuration in AAMS gateway to offset it so it doesn't overlap with others.
    enum eTransIdSource
    {
        transIdFromTransacts        = 0,
        transIdFromTTicketsAuditId  = 1,
    };

    INT32 type; // see ePgadOperationType
    UINT64 transId64;
    PString userId;
    INT32 transType; // transType for cash, ticketActionType for tTickets, newStatus for rolls
    UINT32 objectId; // if used - PK in CASHTRANS table
    INT32 chips;
    INT32 chipsAfter;
    INT32 bonusBalance;
    UINT32 adminIntId;
    SrvTime when;
    UINT32 userIntId;
	INT32 tChips;
	INT32 tChipsAfter;
	UINT64 flags2;
    UINT32 flags;
    UINT64 priv2;
    UINT32 priv;
    INT32 tTickets;
    INT32 tTicketsAfter;
    // #19360 - implementing different sources of 64-bit transaction ids sent to AAMS
    // so that those sources do not overlap each other.
    UINT32 transIdSource; // eTransIdSource
	INT32 rollsDelta;
	INT32 rollsAfter;
	UINT64 flags3; // PYR-67696
	UINT64 priv3; // PYR-67696

    PgadTransactData()
        : type(pgadOperationUnknown)
        , transId64(0)
        , transType(0)
        , objectId(0)
        , chips(0)
        , chipsAfter(0)
        , bonusBalance(0)
        , adminIntId(0)
        , userIntId(0)
		, tChips(0)
		, tChipsAfter(0)
		, flags2(0)
        , flags(0)
        , priv2(0)
        , priv(0)
        , tTickets(0)
        , tTicketsAfter(0)
        , transIdSource(transIdFromTransacts)
		, rollsDelta(0)
		, rollsAfter(0)
		, flags3(0)
		, priv3(0)
    {}

    void parse(CommMsgParser& mainParser)
    {
        CommMsgBody pgadBody;
        mainParser.parseMsgBody(pgadBody);

        CommMsgParser pgadParser(pgadBody);
        pgadParser
            .parseINT32(type)
            .parseUINT64(transId64)
            .parseStringP(userId)
            .parseINT32(transType)
            .parseUINT32(objectId)
            .parseINT32(chips)
            .parseINT32(chipsAfter)
            .parseINT32(bonusBalance)
            .parseUINT32(adminIntId)
            .parseSrvTime(when)
            .parseUINT32(userIntId)
            .parseINT32(tChips)
            .parseINT32(tChipsAfter)
            .parseUINT64(flags2)
            .parseUINT32(flags)
            .parseUINT64(priv2)
            .parseUINT32(priv)
            .parseINT32(tTickets)
            .parseINT32(tTicketsAfter)
            .parseUINT32(transIdSource)
            .parseINT32(rollsDelta)
            .parseINT32(rollsAfter);
        if (!pgadParser.parseEnded()) // PYR-67696 TODO FK remove after 2018.Main.11 is released on IOM (NJ not affected)
        {
             pgadParser
                .parseUINT64(flags3)
                .parseUINT64(priv3);
        }

        // !!! this parse() is used for OLAP replication
        // so any changes should adhere to how we do it for OLAP messages
        // e.g. extra members should be parsed at the end after if( parser.parseEnded() )
    }

    void composeBody(CommMsgBody& body) const
    {
        CommMsgBody pgadBody;
		pgadBody
			.composeINT32(type)
			.composeUINT64(transId64)
			.composeString(userId)
			.composeINT32(transType)
			.composeUINT32(objectId)
			.composeINT32(chips)
			.composeINT32(chipsAfter)
			.composeINT32(bonusBalance)
			.composeUINT32(adminIntId)
			.composeSrvTime(when)
			.composeUINT32(userIntId)
			.composeINT32(tChips)
			.composeINT32(tChipsAfter)
			.composeUINT64(flags2)
			.composeUINT32(flags)
			.composeUINT64(priv2)
			.composeUINT32(priv)
			.composeINT32(tTickets)
			.composeINT32(tTicketsAfter)
			.composeUINT32(transIdSource)
			.composeINT32(rollsDelta)
			.composeINT32(rollsAfter)
			.composeUINT64(flags3) // PYR-67696
			.composeUINT64(priv3) // PYR-67696
            ;
        // !!! this composeBody() is used for OLAP replication
        // so any changes should adhere to how we do it for OLAP messages
        // e.g. extra members should be composed at the end

        body.composeMsgBody(pgadBody);
    }

	const char* toString(PString& buf) const // PYR-48975
	{
		buf = "";
		PString logStr1;
		buf
			.append( "t=").appendInt(type)
			.append(",i=").appendUint64(transId64)
			.append(",u=").append(userId)
			.append(",tt=").appendInt(transType)
			.append(",o=").appendUint(objectId)
			.append(",c=").appendInt(chips)
			.append(",ca=").appendInt(chipsAfter)
			.append(",b=").appendInt(bonusBalance)
			.append(",ai=").appendUint(adminIntId)
			.append(",w=").append(srvTimeToString(when, logStr1))
			.append(",ui=").appendUint(userIntId)
			.append(",tc=").appendInt(tChips)
			.append(",tca=").appendInt(tChipsAfter)
			.append(",f2=x").appendHexInt64(flags2)
			.append(",f=x").appendHexInt(flags)
			.append(",p2=x").appendHexInt64(priv2)
			.append(",p=x").appendHexInt(priv)
			.append(",tt=").appendInt(tTickets)
			.append(",tta=").appendInt(tTicketsAfter)
			.append(",ti=").appendUint(transIdSource)
			.append(",r=").appendInt(rollsDelta)
			.append(",ra=").appendInt(rollsAfter)
			.append(",f3=x").appendHexInt64(flags3)
			.append(",p3=x").appendHexInt64(priv3)
			;
		return buf;
	}
};
// ///////////////////////////////////////////////////////////////////
// Start of PGAD Protocol. Not implemented: 4.2 (open legal account) - we don't support that
//
// opening of a gaming account (4.1) - Cashier
// #19722: append the name of the country of birth to IT_Q_ACCOUNT_OPEN
#define IT_Q_ACCOUNT_OPEN					700 //44 - reqId,....
#define IT_A_ACCOUNT_OPEN					(IT_Q_ACCOUNT_OPEN + IT_Q_A_DELTA) //46[s]8s - reqId
// gaming account transaction (4.3) - AAMS replicator
#define IT_Q_ACCOUNT_TRANSACTION			701 //4s48841144t41s - reqId,userId,userIntId,transId,flags2,transIdSource,method,reason,amount,balance,transTime,bonusPart,cancelType,origReceiptId
#define IT_A_ACCOUNT_TRANSACTION			(IT_Q_ACCOUNT_TRANSACTION + IT_Q_A_DELTA) //46[s]8s - reqId,errCode[errDesc],transIdSubmitted,xTransStr
// gaming account status change (4.5) - AAMS replicator
#define IT_Q_ACCOUNT_STATUS_CHANGE			702 //4s48841s - reqId,userId,userIntId,transId,flags2,transIdSource,reason,status
#define IT_A_ACCOUNT_STATUS_CHANGE			(IT_Q_ACCOUNT_STATUS_CHANGE + IT_Q_A_DELTA) //46[s]8s - reqId,errCode,[errDesc]
// gaming account province change (4.7) - Lobby
#define IT_Q_ACCOUNT_PROVINCE_CHANGE		703 //4s4s8 - reqId,userId,userIntId,province,flags2
#define IT_A_ACCOUNT_PROVINCE_CHANGE		(IT_Q_ACCOUNT_PROVINCE_CHANGE + IT_Q_A_DELTA) //46[s]8s - reqId
// get gaming account status (4.8) - not currently sent from anywhere
#define IT_Q_ACCOUNT_GET_STATUS				704 //4s4 - reqId, userid, userintid
#define IT_A_ACCOUNT_GET_STATUS				(IT_Q_ACCOUNT_GET_STATUS + IT_Q_A_DELTA) //42[s|2s] - reqId, errCode [errDesc | code status]
// gaming account transaction (4.6) - Cashier (from Admin connection)
#define IT_Q_ACCOUNT_BALANCE			    705 //44 - reqId,....
#define IT_A_ACCOUNT_BALANCE			    (IT_Q_ACCOUNT_BALANCE + IT_Q_A_DELTA) //46[s]8s - reqId
// gaming bonus movement (4.4) - AAMS replicator
#define IT_Q_BONUS_MOVEMENT 			    706 //44 - reqId,....
#define IT_A_BONUS_MOVEMENT 			    (IT_Q_BONUS_MOVEMENT + IT_Q_A_DELTA) //46[s]8s - reqId
// gaming account sub registration (4.9 -> 4.15) - Lobby (upon every login)
#define IT_Q_ACCOUNT_SUB_REGISTRATION       707 //4s48O - reqId, userid, userintid, flags2, PgadUserAssets
#define IT_A_ACCOUNT_SUB_REGISTRATION       (IT_Q_ACCOUNT_SUB_REGISTRATION + IT_Q_A_DELTA) //46[s]8s - reqId, errCode [errDesc]
// batch operation to combine several account transactions and status changes
// Obsolete
//#define IT_Q_ACCOUNT_BATCH_OPERATION      708 //44<1(M)> - reqId, numOperations<opType(opBody)>
//#define IT_A_ACCOUNT_BATCH_OPERATION      (IT_Q_ACCOUNT_BATCH_OPERATION + IT_Q_A_DELTA) //46[s] - reqId, errCode [errDesc]
#define IT_Q_MIGRATE_LICENSE_VERSION		709 //4s4884444s - reqId, userid, userintid, old_flags2, new_flags2, chips, tchips, tickets, bonus, fiscal
#define IT_A_MIGRATE_LICENSE_VERSION		(IT_Q_MIGRATE_LICENSE_VERSION + IT_Q_A_DELTA) //46[s]8s - reqId, errCode [errDesc]

	//#23644
// (4.12)
#define IT_Q_FUND_TRANSFERS_SUMMARY			710 // 48d - reqId,flags2,forDate
#define IT_A_FUND_TRANSFERS_SUMMARY			(IT_Q_FUND_TRANSFERS_SUMMARY +  IT_Q_A_DELTA) //46[s8s|8s4<m(PgadFundTransferDetails)>]
// (4.13)
#define IT_Q_SERVICE_TRANSACTIONS_SUMMARY	711 // 48d - reqId,flags2,forDate
#define IT_A_SERVICE_TRANSACTIONS_SUMMARY	(IT_Q_SERVICE_TRANSACTIONS_SUMMARY +  IT_Q_A_DELTA) //46[s8s|8s4<m(PgadServiceTransactionDetails)>]
// (4.14)
#define IT_Q_ACCOUNT_TRANSACTION_CANCEL     712  //4s48841144t41s - reqId,userId,userIntId,transId,flags2,transIdSource,method,reason,amount,balance,transTime,bonusPart,cancelType,origReceiptId
#define IT_A_ACCOUNT_TRANSACTION_CANCEL     (IT_Q_ACCOUNT_TRANSACTION_CANCEL + IT_Q_A_DELTA) //46[s]8s - reqId,errCode[errDesc],transIdSubmitted,xTransStr
// (4.20)
#define IT_Q_DORMANT_INIT	                713 // PYR-31811 4d - reqId,forDate
#define IT_A_DORMANT_INIT	                (IT_Q_DORMANT_INIT +  IT_Q_A_DELTA) // PYR-31811 46[s]8ss4<m(PgadDormantGameAccountsDetails)>
// (4.20) reply: reqId,errCode,[errStr],transId,idRicevuta,idFsc,total,<PgadDormantGameAccountsDetails>
// (4.19)
#define IT_Q_DORMANT_ACCOUNT                714 // PYR-31811 448d4 - reqId,userIntid,flags2,forDate,balance amount
#define IT_A_DORMANT_ACCOUNT                (IT_Q_DORMANT_ACCOUNT +  IT_Q_A_DELTA) // PYR-31811 46[s]8s - reqId,errCode[errDesc],transIdSubmitted,xTransStr
// (4.16) PYR-33375
#define IT_Q_SIMPLIFIED_OPENING_GAMING_ACCT 715 // PYR-33375 44s8M reqId,userIntId,userId,flags2,M(struct UserUpdate)
#define IT_A_SIMPLIFIED_OPENING_GAMING_ACCT (IT_Q_SIMPLIFIED_OPENING_GAMING_ACCT + IT_Q_A_DELTA) // PYR-33375 46[s]8s reqId,errCode,[errStr],transIdSubmitted,idRicevuta
// (4.17) PYR-33375
#define IT_Q_COMPLETING_SIMPLIFIED_OPENING  716 // PYR-33375 44s8M reqId,userIntId,userId,flags2,M(struct UserUpdate)
#define IT_A_COMPLETING_SIMPLIFIED_OPENING  (IT_Q_COMPLETING_SIMPLIFIED_OPENING + IT_Q_A_DELTA) // PYR-33375 46[s]8s reqId,errCode,[errStr],transIdSubmitted,idRicevuta
// (4.18) PYR-33375
#define IT_Q_LIST_GAMING_ACCT_WO_SUBRECORD  717 // PYR-33375 4Ds66 reqId,requestDate,status,uint16Start,uint16End
#define IT_A_LIST_GAMING_ACCT_WO_SUBRECORD  (IT_Q_LIST_GAMING_ACCT_WO_SUBRECORD + IT_Q_A_DELTA) // PYR-33375 46[s]8ss4<M(struct PgadGameAccountWoSubRegistration)>
// (4.18) reply: reqId,errCode,[errStr],transId,idRicevuta,idFsc,total,<M(struct PgadGameAccountWoSubRegistration)>

//PYR-51481
#define IT_Q_GET_SW_MODULES					718	// 4 - reqId
#define IT_A_GET_SW_MODULES					(IT_Q_GET_SW_MODULES + IT_Q_A_DELTA) // 42[s|4<M(b44s444<M(ss)>)>] - reqId,errCode[errString|numSwModules<M(active,code,type,name,version,subVersion,numModules<M(sha1,filename)>)>]

//PYR-51481
#define IT_Q_UPDATE_SW_MODULES_VER			719 // 44<M(b4444s)> - reqId,numModulesToUpdate<M(active,code,type,newVersion,newSubVersion,adminComment)>
#define IT_A_UPDATE_SW_MODULES_VER			(IT_Q_UPDATE_SW_MODULES_VER + IT_Q_A_DELTA) // 42[s] - reqId,errCode[aggregatedErrorDescr]

// PYR-51313 4.11 - AGGIORNA DATI DOCUMENTO TITOLARE (UPDATING OF ACCOUNT HOLDER DOCUMENT DATA)
#define IT_Q_UPDATE_ACCOUNT_DATA			720 // 4484s444ss - reqId,userintid,flags2,UpdateAccountDataRequest
#define IT_A_UPDATE_ACCOUNT_DATA			(IT_Q_UPDATE_ACCOUNT_DATA + IT_Q_A_DELTA) //46[s]8s - reqId,errCode,[errDesc]

// (4.20) PYR-50515 4.20
#define IT_Q_DORMANT_INIT_BY_LICENSE        721 // 4d844 - reqId,forDate,reqPgadIdCn,startIdx,endIdx
#define IT_A_DORMANT_INIT_BY_LICENSE        (IT_Q_DORMANT_INIT_BY_LICENSE +  IT_Q_A_DELTA) // 46[s|4]
// (4.20) reply: reqId,errCode,[errStr|numAccounts]

// (4.24) PYR-67541 - GESTIONE AUTOESCLUSIONE TRASVERSALE (MANAGING TRANSVERSAL SELF-EXCLUSION)
#define IT_Q_UPDATE_SELFEXCLUSION           722 // 44s4 - reqId,userintid,fiscalCode,selfexclusionStatus
#define IT_A_UPDATE_SELFEXCLUSION           (IT_Q_UPDATE_SELFEXCLUSION +  IT_Q_A_DELTA) // 46[s] - reqId,errCode[,errDesc]

// (4.22) PYR-68104 - Update of Deposit Limits on Player account
#define IT_Q_UPDATE_DEPOSIT_LIMITS          723 // 44844 - reqId,userintid,flags2,SelfDepositLimitType,limitamount
#define IT_A_UPDATE_DEPOSIT_LIMITS          (IT_Q_UPDATE_DEPOSIT_LIMITS +  IT_Q_A_DELTA) // 46[s] - reqId,errCode[,errDesc]

// (4.25) PYR-67720 - Get National Exclusion List
#define IT_Q_GET_BLACKLIST			        724 // 444 - reqId,startIdx,endIdxs
#define IT_A_GET_BLACKLIST					(IT_Q_GET_BLACKLIST +  IT_Q_A_DELTA) // 46[s]4<ItBlackListItem> - reqId,errCode[,errDesc]

// (4.26) PYR-68104 - opening of a gaming account 2
#define IT_Q_ACCOUNT_OPEN2					725 //44 - reqId,....
#define IT_A_ACCOUNT_OPEN2					(IT_Q_ACCOUNT_OPEN2 + IT_Q_A_DELTA) //46[s]8s - reqId
// end of messages

// start of constants
// PGAD ACCOUNT STATUS CODES
#define PGAD_ACCOUNT_STATUS_OPENED					1	// AP: Opened
#define PGAD_ACCOUNT_STATUS_SUSPENDED				2	// SP: Suspended
#define PGAD_ACCOUNT_STATUS_CLOSED					3	// CH: Closed
#define PGAD_ACCOUNT_STATUS_DORMANT					4	// DO: Dormant
#define PGAD_ACCOUNT_STATUS_BLOCKED					5	// BL: Blocked // PYR-66028

#define PGAD_ACCOUNT_STATUS_OPENED_STR2				"1"	// AP: Opened
#define PGAD_ACCOUNT_STATUS_SUSPENDED_STR2			"2"	// SP: Suspended
#define PGAD_ACCOUNT_STATUS_CLOSED_STR2				"3"	// CH: Closed
#define PGAD_ACCOUNT_STATUS_DORMANT_STR2			"4"	// DO: Dormant
#define PGAD_ACCOUNT_STATUS_BLOCKED_STR2			"5"	// BL: Blocked // PYR-66028

#define PGAD_ACCOUNT_STATUS_OPENED_STR				"AP" // : Opened
#define PGAD_ACCOUNT_STATUS_SUSPENDED_STR			"SP" // : Suspended
#define PGAD_ACCOUNT_STATUS_CLOSED_STR				"CH" // : Closed
#define PGAD_ACCOUNT_STATUS_DORMANT_STR				"DO" // : Dormant
#define PGAD_ACCOUNT_STATUS_BLOCKED_STR				"BL" // : Blocked // PYR-66028

// account changes reasons as per version 2 of the protocol
#define PGAD_ACCOUNT_STATUS_CHANGE_AAMS             1
#define PGAD_ACCOUNT_STATUS_CHANGE_OPERATOR         2
#define PGAD_ACCOUNT_STATUS_CHANGE_ACCT_HOLDER      3
#define PGAD_ACCOUNT_STATUS_CHANGE_JUDICIAL_AUTH    4
#define PGAD_ACCOUNT_STATUS_CHANGE_DEATH            5
#define PGAD_ACCOUNT_STATUS_CHANGE_FAILURE_TO_SEND_DOCS 6
#define PGAD_ACCOUNT_STATUS_CHANGE_FRAUD            7

// define the string version of these constants
#define PGAD_ACCOUNT_STATUS_CHANGE_AAMS_STR             "1"
#define PGAD_ACCOUNT_STATUS_CHANGE_OPERATOR_STR         "2"
#define PGAD_ACCOUNT_STATUS_CHANGE_ACCT_HOLDER_STR      "3"
#define PGAD_ACCOUNT_STATUS_CHANGE_JUDICIAL_AUTH_STR    "4"
#define PGAD_ACCOUNT_STATUS_CHANGE_DEATH_STR            "5"
#define PGAD_ACCOUNT_STATUS_CHANGE_FAILURE_TO_SEND_DOCS_STR "6"
#define PGAD_ACCOUNT_STATUS_CHANGE_FRAUD_STR            "7"

// account self-exclusion status
#define PGAD_SELFEXCLUSION_STATUS_NONE       0 // not self-excluded
#define PGAD_SELFEXCLUSION_STATUS_INDEFINITE 1 // table 5.13
#define PGAD_SELFEXCLUSION_STATUS_30DAYS     2
#define PGAD_SELFEXCLUSION_STATUS_60DAYS     3
#define PGAD_SELFEXCLUSION_STATUS_90DAYS     4

// ========================================================================
// PGAD

// PGAD REPLY CODES
#define PGAD_REPLY_SUCCESS									1024
#define PGAD_REPLY_OPERATION_BLOCKED						1025
#define PGAD_REPLY_REQUEST_STILL_PROCESSED					1026
#define PGAD_REPLY_DUP_TRANSACTION_ID						1027
#define PGAD_REPLY_INVALID_USERNAME_TOKEN					1028
#define PGAD_REPLY_GENERAL_ERROR         					1029 // PYR-23644
#define PGAD_REPLY_UNKNOWN_TRANSMITTING_AGENT				1100
#define PGAD_REPLY_UNKNOWN_ACCOUNT_OWNER_AGENT				1101
#define PGAD_REPLY_MISMATCH_OWNER_VAT_NUMBER				1102
#define PGAD_REPLY_USER_HAS_ACCOUNT							1200
#define PGAD_REPLY_EXISTING_ACCOUNT							1201
#define PGAD_REPLY_INEXISTING_ACCOUNT						1202
#define PGAD_REPLY_BLOCKED_CLOSED_ACCOUNT					1203
#define PGAD_REPLY_BLOCKED_INACTIVE_ACCOUNT					1204
#define PGAD_REPLY_PAYOUT_INACTIVE_ACCOUNT					1205
#define PGAD_REPLY_PROVINCE_CLOSED_ACCOUNT					1206
#define PGAD_REPLY_PROVINCE_INACTIVE_ACCOUNT				1207
#define PGAD_REPLY_REOPENING_OPENED_ACCOUNT                 1208
#define PGAD_REPLY_REOPENING_CLOSED_ACCOUNT                 1209
#define PGAD_REPLY_REOPENING_INACTIVE_ACCOUNT               1210
#define PGAD_REPLY_SUSPENDING_SUSPENDED_ACCOUNT				1211
#define PGAD_REPLY_SUSPENDING_CLOSED_ACCOUNT				1212
#define PGAD_REPLY_SUSPENDING_INACTIVE_ACCOUNT				1213
#define PGAD_REPLY_CLOSING_CLOSED_ACCOUNT					1214
#define PGAD_REPLY_CLOSING_INACTIVE_ACCOUNT					1215
#define PGAD_REPLY_TRANSACTION_ZERO_OR_LESS					1216
#define PGAD_REPLY_NEGATIVE_PAYOUT_AMOUNT					1217
#define PGAD_REPLY_NEGATIVE_BONUS_AMOUNT					1218
#define PGAD_REPLY_PAYOUT_MISMATCH							1219
#define PGAD_REPLY_BONUSPAYOUT_GREATER_THAN_PAYOUT			1220
#define PGAD_REPLY_INVALID_BONUSPAYOUT_INEXISTENT_GAME		1221
#define PGAD_REPLY_INVALID_BONUSPAYOUT_TOTAL				1222
#define PGAD_REPLY_INVALID_BONUSPAYOUT_DUPLICATE_GAME		1223
#define PGAD_REPLY_INVALID_BONUSPAYOUT_SUM_TOTAL			1224
#define PGAD_REPLY_BONUS_GREATER_EQUAL_ZERO					1225
#define PGAD_REPLY_BONUSPAYOUT_LESS_THAN_BONUS				1226
#define PGAD_REPLY_INVALID_BONUS_INEXISTENT_GAME			1227
#define PGAD_REPLY_INVALID_BONUS_TOTAL						1228
#define PGAD_REPLY_INVALID_BONUS_DUPLICATE_GAME				1229
#define PGAD_REPLY_INVALID_BONUS_SUM_TOTAL					1230
#define PGAD_REPLY_INVALID_BONUS_MISMATCH_BONUSPAYOUT		1231
#define PGAD_REPLY_INVALID_PROVINCE							1232
#define PGAD_REPLY_NOT_AUTHORIZED                           1233
#define PGAD_REPLY_INVALID_REASON                           1234
#define PGAD_REPLY_NOT_PERMITTED_ACCOUNT_CLOSED				1260
#define PGAD_REPLY_NOT_PERMITTED_ACCOUNT_DORMANT			1261
#define PGAD_REPLY_NOT_PERMITTED_ACCOUNT_BLOCKED			1272
#define PGAD_REPLY_NOT_PERMITTED_LEGAL_PERSON_ACCOUNT		1281
#define PGAD_REPLY_NOT_PERMITTED_SIMPLIFIED_REGISTRATION	1282
#define PGAD_REPLY_INVALID_SE_TYPE							1292
#define PGAD_REPLY_FORBIDDEN_SE_NO_ACCOUNT					1293
#define PGAD_REPLY_FORBIDDEN_REACTIVATION_NOT_INDEFINITE_SE	1294
#define PGAD_REPLY_REACTIVATION_IN_PROGRESS					1295
#define PGAD_REPLY_FORBIDDEN_REACTIVATION_SE_LESS_SIX_MONTH	1296
#define PGAD_REPLY_FORBIDDEN_SE_ALREADY_IN_PROGRESS			1297
#define PGAD_REPLY_SELF_EXCLUDED_SUBJECT					1298
#define PGAD_REPLY_INCONSISTENT_LIMIT_AMOUNTS				1299
#define PGAD_REPLY_UNKNOWN_PERSON							1300
#define PGAD_REPLY_DECEASED_PERSON							1301
#define PGAD_REPLY_UNDERAGE_PERSON							1302
#define PGAD_REPLY_INVALID_PERSONAL_DATA					1303
#define PGAD_REPLY_INVALID_PERSONAL_TAX_ID					1304
#define PGAD_REPLY_ACCOUNT_ALREADY_DORMANT					1400

struct PgadFundTransferDetails
{
	BYTE   operationCode; // aka Transaction Reason (table 5.5 in PGAD protocol)
	UINT32 numberOfOperations;
	INT64  totalAmount;
	static void composeVector(CommMsgBody& parentBody, const vector<PgadFundTransferDetails>& v)
	{
		parentBody.composeUINT32(v.size());
		for (int i = 0; i < v.size(); ++i)
		{
			CommMsgBody recordBody;
			recordBody
				.composeBYTE(v[i].operationCode)
				.composeUINT32(v[i].numberOfOperations)
				.composeINT64(v[i].totalAmount);
			parentBody.composeMsgBody(recordBody);
		}
	}
	static void parseVector(CommMsgParser& parentParser, vector<PgadFundTransferDetails>& v)
	{
		v.clear();
		UINT32 size;
		parentParser.parseUINT32(size);
		v.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			CommMsgBody recordBody;
			parentParser.parseMsgBody(recordBody);
			CommMsgParser recordParser(recordBody);
			PgadFundTransferDetails record;
			recordParser
				.parseBYTE(record.operationCode)
				.parseUINT32(record.numberOfOperations)
				.parseINT64(record.totalAmount);
			v.push_back(record);
		}
	}
};

struct PgadServiceTransactionDetails
{
	BYTE   operationCode; // aka Transaction Reason (table 5.10 in PGAD protocol)
	UINT32 numberOfOperations;
	static void composeVector(CommMsgBody& parentBody, const vector<PgadServiceTransactionDetails>& v)
	{
		parentBody.composeUINT32(v.size());
		for (int i = 0; i < v.size(); ++i)
		{
			CommMsgBody recordBody;
			recordBody
				.composeBYTE(v[i].operationCode)
				.composeUINT32(v[i].numberOfOperations);
			parentBody.composeMsgBody(recordBody);
		}
	}
	static void parseVector(CommMsgParser& parentParser, vector<PgadServiceTransactionDetails>& v)
	{
		v.clear();
		UINT32 size;
		parentParser.parseUINT32(size);
		v.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			CommMsgBody recordBody;
			parentParser.parseMsgBody(recordBody);
			CommMsgParser recordParser(recordBody);
			PgadServiceTransactionDetails record;
			recordParser
				.parseBYTE(record.operationCode)
				.parseUINT32(record.numberOfOperations);
			v.push_back(record);
		}
	}
};

// end of PGAD Protocol
// ///////////////////////////////////////////////////////////////////

// .BE begins
#define BE_Q_ACCOUNT_OPEN						800 // 4ts4<4M> - reqId, timestamp, userId, numUpdates<updateType, updateBody> (see UserUpdate.parse())
#define BE_A_ACCOUNT_OPEN						(BE_Q_ACCOUNT_OPEN + IT_Q_A_DELTA) // 4 - reqId

#define BE_Q_ACQUIRE_GAME_CARD					801 // 4stsss8 - reqId, userId, timestamp, firstName, lastName, playerToken, episRequestId
#define BE_A_ACQUIRE_GAME_CARD					(BE_Q_ACQUIRE_GAME_CARD + IT_Q_A_DELTA) // 4 - reqId

#define BE_Q_RELEASE_GAME_CARD					802 // 4stss7777778 - reqId, userId, timestamp, playerToken, gameCardId, lossCredit, creditPeriod, bets, wins, results, playedTime, episRequestId
#define BE_A_RELEASE_GAME_CARD					(BE_Q_RELEASE_GAME_CARD + IT_Q_A_DELTA) // 4 - reqId

// LDbm -> beLimitController
#define BE_Q_USER_LOGGED_IN						803 // 48s7O - reqId, loginId, playerToken, tokenType, User, TODO: #dmitryst remove notifications to limits controller after migration to new BE token complete
#define BE_A_USER_LOGGED_IN						(BE_Q_USER_LOGGED_IN + IT_Q_A_DELTA) // 4 - reqId

#define BE_Q_USER_LOGGED_OUT					804 // 48s8O - reqId, loginId, playerToken, loginTime, User, TODO: #dmitryst remove notifications to limits controller after migration to new BE token complete
#define BE_A_USER_LOGGED_OUT					(BE_Q_USER_LOGGED_OUT + IT_Q_A_DELTA) // 4 - reqId

#define BE_Q_USER_UPDATE                        805 // 4ts4<4M> - reqId, timestamp, playerToken, numUpdates<updateType, updateBody> (see UserUpdate.parse())
#define BE_A_USER_UPDATE                        (BE_Q_USER_UPDATE + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

// PYR-19884
#define BE_Q_SET_IGNORED_REG_ERRORS             806 // 44<4> - reqId, numErrors<ignError>
#define BE_A_SET_IGNORED_REG_ERRORS             (BE_Q_SET_IGNORED_REG_ERRORS + IT_Q_A_DELTA) // 46[s] - reqId, errCode[errDescr]

#define BE_ERROR_DUPLICATE_USER					1001 // PYR-19436 - should not happen anymore
#define BE_ERROR_INVALID_EMAIL					1002
#define BE_ERROR_REG_EMAIL_FAILED				1003
#define BE_ERROR_INVALID_NATREGNUMBER           1004 // invalid national registry number entered

#define BE_ERROR_USER_PENALIZED					2001
#define BE_ERROR_CARD_ALREADY_ACTIVE			2002
#define BE_ERROR_ACQUIRE_PLAYERTOKEN_NOT_EXISTS 2003
#define BE_ERROR_TOKEN_NOT_MATCH_NAME           2004
#define BE_ERROR_GAMECLASS_UNKNOWN              2005
#define BE_ERROR_GAMETYPE_UNKNOWN               2006
#define BE_ERROR_SWITCH_NOT_YET_ALLOWED         2007

// PYR-19436 - update player error codes
#define BE_ERROR_UPDATE_PLAYERTOKEN_NOT_EXISTS  4001 // player token doesn't exists
#define BE_ERROR_PLAYER_MISMATCH                4002 // player doesn't correspond to the given properties
#define BE_ERROR_NEW_EMAIL_INVALID              4003

#define BE_ERROR_USER_EXCLUDED					9001
#define BE_ERROR_UNEXPECTED						9002
#define BE_ERROR_INCORRECT						9003

#define BE_ERROR_TIMEOUT						25555

// .BE ends


//////////////////////////////////////////////////////////////////////////
// start social media gateway protocol

#define PSAPI_Q_SOCIAL_SHARE_HAND				900 //4sbbbb - reqId, handText, showHoleCards, showRealTableName, showRealNicks, showHandNumber
#define PSAPI_A_SOCIAL_SHARE_HAND				(PSAPI_Q_SOCIAL_SHARE_HAND + IT_Q_A_DELTA) //42[s] - reqId, errCode [errDesc]

#define PSAPI_ERR_OPERATION_BLOCKED				10000

// end social media gateway protocol
//////////////////////////////////////////////////////////////////////////


// max size for AMMS crop vectors
//
#define MAX_AAMS_MESSAGE_CROP_PORTION_SIZE		100
#define MAX_AAMS_USER_CROP_PORTION_SIZE			100
#define MAX_AAMS_REPORT_DATE_CROP_PORTION_SIZE	100
#define MAX_AAMS_HAND_REPORT_PORTION_SIZE		1000
#define MAX_AAMS_DEBUG_DATE_SHIFT				7
#define MAX_AAMS_USER_SUMMARY_DELAY_SEC			3600

//MOVED here from itgamestructs.h
//
// Status code convention:
//  < 10 - accepted, not sent
//  > 9 && < 100 - confirmed by AAMS but not final (5x - error code returned by AAMS)
//  > 99 - confirmed by AAMS and final
//
enum eSessionStatus
{
	eSessionStatusUndefined = 0,
	eSessionStatusApproved = 10,
	eSessionStatusRejected = 999,
	eSessionStatusHasApprovedParticipants = 1,
	eSessionStatusConfirmed = 11,
	eSessionStatusPendingPrizeSchedule = 2,
	eSessionStatusPrizeScheduleReported = 12,
	eSessionStatusPrizeScheduleFailed = 52,
	eSessionStatusPendingWinners = 3,
	eSessionStatusWinnersReported = 13,
	eSessionStatusWinnersFailed = 53,
	eSessionStatusPendingCredits = 4,
	eSessionStatusCreditsReported = 14,
	eSessionStatusCreditsFailed = 54,
	//eSessionStatusPendingRefunds = 5,
	//eSessionStatusRefundsReported = 15,
	//eSessionStatusRefundsFailed = 55,
	eSessionStatusPendingUnregistrations = 6,
	eSessionStatusUnregistrationsReported = 16,
	eSessionStatusUnregistrationsFailed = 56,
	eSessionStatusCloseFailed = 50,
	eSessionStatusClosed = 100,
	eSessionStatusNotAllowed = 1000,
	eSessionStatusCancelled = 1001,
	eSessionStatusCancelFailed = 1002
};

enum eSessionUserStatus
{
	eSessionUserStatusUndefined = 0,
	eSessionUserStatusPendingWinner = 1,
	//eSessionUserStatusPendingAward = 2,
	eSessionUserStatusPendingRefund = 3,
	eSessionUserStatusApproved = 100,
	eSessionUserStatusRejected = 999,
	eSessionUserStatusWinner = 11,
	eSessionUserStatusWinnerFailed = 51,
	eSessionUserStatusAwardFailed = 52,
	eSessionUserStatusCancellationFailed = 53,
	eSessionUserStatusRefundFailed = 54,
	eSessionUserStatusAwarded = 102,
	eSessionUserStatusCancelled = 101,
	//eSessionUserStatusRefunded = 103
};

enum eSessionType
{
	eSessionTypeOld			= 0,
	eSessionTypeTournament	= 1,
	eSessionTypeRingTable	= 2,
	eSessionTypeNoSession	= 3,
};

enum eAamsGameMod 
{ 
	eAamsGameMod_Unknown = 0, 
	eAamsGameMod_Standard = 1, 
	eAamsGameMod_Blitz = 2, 
	eAamsGameMod_Sag = 3, 
	eAamsGameMod_Flight = 4,
	eAamsGameMod_Storm = 5 // PYR-56283
};

struct AamsSessionInfo
{
	UINT32 sessionId;
	UINT32 sessionStatus;
	PString aamsSessionId;
	UINT16 lastAamsError;
	UINT32 game;
	PString gamingSessionId;
	UINT32 chipsType;
	SrvTime created; // PYR-24066
	eAamsGameMod mod;	//not parsable or composable
	UINT32 flightFlags; //PYR-26095 not parsable or composable

	AamsSessionInfo()
		: sessionId(0)
		, sessionStatus(eSessionStatusUndefined)
		, lastAamsError(0)
		, game(0)
		, chipsType(0)
		, mod(eAamsGameMod_Unknown)
		, flightFlags(0)
	{}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body
			.composeUINT32(sessionId)
			.composeUINT32(sessionStatus)
			.composeString(aamsSessionId)
			.composeUINT16(lastAamsError)
			.composeUINT32(game)
			.composeString(gamingSessionId)
			.composeUINT32(chipsType)
			.composeSrvTime(created) // PYR-24066
			;
		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32(sessionId)
			.parseUINT32(sessionStatus)
			.parseStringP(aamsSessionId)
			.parseUINT16(lastAamsError)
			.parseUINT32(game)
			.parseStringP(gamingSessionId)
			.parseUINT32(chipsType)
			.parseSrvTime(created) // PYR-24066
			;
		return parser;
	}
	bool isBlitz() const { return chipsType == BLITZ_REAL_CHIPS; }

	eAamsGameMod getGameMod() const
	{
		if( eAamsGameMod_Unknown == mod )
		{
			PTRACE("AamsSessionInfo.mod not initialized");
			PASSERT(0);
		}
		return mod;
	}


};
//END MOVED

struct PgadTransactEssential // see table PGADTRANSACTS PYR-23644
{
	UINT32 pgadTransId; // surrogate Key - autoincremented by OLTP
	UINT64 transactId; // TRANSACTS.TRANSID, or TournTicketsAudit.AUDITID if FLAGS.ePgadTransactFlag_TTicketTotalUpdate is 1
	UINT32 flags; // enum ePgadTransactFlags
	UINT32 userIntId;
	UINT64 transactIdReported; // PS TransId reported to IT
	SrvTime whenReported; // when reported (may differ from TRANSACTS.WHEN) it is filled by OLTP
	INT32 balanceDelta; // reported delta cash = TRANSACTS.CHIPS     , except when balance is crossing negative boundary
	INT32 balanceAfter; // reported cash after = TRANSACTS.CHIPSAFTER, except when balance is crossing negative boundary
	INT32 bonusPart; // ref PYR-19019; bound to IT USERS.PENDING_BONUS; <= balanceAfter
	INT32 errCode;     // .IT PGAD reply
	PString xTransStr; // receipt Id from .IT PGAD - 25 chars
	INT16 pgadMsg; // PYR-50202 - ePgadTransactMsg
	PString propStr; // PYR-66028

	PgadTransactEssential() : pgadTransId(0), transactId(0), flags(0), userIntId(0), transactIdReported(0), 
		balanceDelta(0), balanceAfter(0), bonusPart(0), errCode(0), pgadMsg(ePgadTransactMsg_Undefined) {}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseUINT32(pgadTransId)
			.parseUINT64(transactId)
			.parseUINT32(flags)
			.parseUINT32(userIntId)
			.parseUINT64(transactIdReported)
			.parseSrvTime(whenReported)
			.parseINT32(balanceDelta)
			.parseINT32(balanceAfter)
			.parseINT32(bonusPart)
			.parseINT32(errCode)
			.parseStringP(xTransStr)
			.parseINT16(pgadMsg)
			;
		if (!parser.parseEnded()) // TODO FK remove after 2018.Main.07 is released on IOM
		{
			parser.parseStringP(propStr); // PYR-66028
		}
	}
	void compose(CommMsgBody& body) const
	{
		body
			.composeUINT32(pgadTransId)
			.composeUINT64(transactId)
			.composeUINT32(flags)
			.composeUINT32(userIntId)
			.composeUINT64(transactIdReported)
			.composeSrvTime(whenReported)
			.composeINT32(balanceDelta)
			.composeINT32(balanceAfter)
			.composeINT32(bonusPart)
			.composeINT32(errCode)
			.composeString(xTransStr)
			.composeINT16(pgadMsg)
			.composeString(propStr)
			;
	}
	const char* toString(PString& buf) const
	{
		PString logStr1;
		buf = "";
		buf
			.append( "pid=").appendUint(pgadTransId)
			.append(",tid=").appendUint64(transactId)
			.append(",f=x").appendHexInt(flags)
			.append(",u=").appendUint(userIntId)
			.append(",tidr=").appendUint64(transactIdReported)
			.append(",w=").append(srvTimeToString(whenReported, logStr1))
			.append(",bal=").appendInt(balanceDelta).append(' ').appendInt(balanceAfter).append(' ').appendInt(bonusPart)
			.append(",err=").appendInt(errCode)
			.append(",xts=").append(xTransStr)
			.append(",pgmsg=").appendInt(pgadMsg)
			.append(",prp=").append(propStr)
			;
		return buf;
	}
};

struct PgadUserAssets // PYR-31446
{
	INT32 cash;
	INT32 tChips;
	INT32 tTicketsTotal;
	INT32 bonusPart;
	INT32 rollsTotal;
	PgadUserAssets() : cash(0), tChips(0), tTicketsTotal(0), bonusPart(0), rollsTotal(0) {}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseINT32(cash)
			.parseINT32(tChips)
			.parseINT32(tTicketsTotal)
			.parseINT32(bonusPart)
			.parseINT32(rollsTotal);
	}
	void compose(CommMsgBody& body) const
	{
		body
			.composeINT32(cash)
			.composeINT32(tChips)
			.composeINT32(tTicketsTotal)
			.composeINT32(bonusPart)
			.composeINT32(rollsTotal);
	}
	const char* toString(PString& out) const
	{
		out.assign("").appendInt(cash).append('+').appendInt(tChips).append('+').appendInt(tTicketsTotal).append(" b=").appendInt(bonusPart).append(" r=").appendInt(rollsTotal);
		return out.c_str();
	}
	INT32 getTotalBalance() const
	{
		INT32 total = cash + tChips + tTicketsTotal;
		if (total < 0)
		{
			PLog("total=%d<0", total);
			total = 0;
		}
		return total;
	}
	bool adjustBonusPart()
	{
		// no need to process rollsTotal here
		INT32 total = getTotalBalance();
		if (bonusPart > total)
		{
			PLog("bonusPart=%d > total=%d", bonusPart, total);
			bonusPart = total;
			return true;
		}
		return false;
	}
};

struct UpdateAccountDataRequest // PYR-51313, for message 4.11
{
	INT32 idType;
	PString documentNumberStr;
	INT32 dayOfMonth;
	INT32 month;
	INT32 year;
	PString issuingAuthority;
	PString placeOfIssue;
	INT32 authorityId;

	UpdateAccountDataRequest() : idType(0), dayOfMonth(0), month(0), year(0), authorityId(0) {}

	void compose(CommMsgBody& body) const
	{
		body
			.composeINT32(idType)
			.composeString(documentNumberStr)
			.composeINT32(dayOfMonth)
			.composeINT32(month)
			.composeINT32(year)
			.composeString(issuingAuthority)
			.composeString(placeOfIssue)
			.composeINT32(authorityId);
	}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseINT32(idType)
			.parseStringP(documentNumberStr)
			.parseINT32(dayOfMonth)
			.parseINT32(month)
			.parseINT32(year)
			.parseStringP(issuingAuthority)
			.parseStringP(placeOfIssue);
		if(!parser.parseEnded())
		{
			parser.parseINT32(authorityId);
		}
	}
	const char* toString(PString& buf) const
	{
		buf = "";
		buf
			.append( "idType=").appendInt(idType)
			.append(",documentNumberStr=").append(documentNumberStr)
			.append(",when=").appendInt(dayOfMonth).append('-').appendInt(month).append('-').appendInt(year)
			.append(",issuingAuthority=").append(issuingAuthority)
			.append(",placeOfIssue=").append(placeOfIssue)
			.append(",authorityId=").appendInt(authorityId)
			;
		return buf;
	}
};

struct PgadDormantGameAccountsDetails  // PYR-31811 for message 4.20
{
	BYTE    accountNetworkId;
	UINT64  idChAccount;
	PString accountCode;
	INT32   balanceAmount;

	PgadDormantGameAccountsDetails() : accountNetworkId(0), idChAccount(0), balanceAmount(0) {}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseBYTE(accountNetworkId)
			.parseUINT64(idChAccount)
			.parseStringP(accountCode)
			.parseINT32(balanceAmount);
	}
	void compose(CommMsgBody& body) const
	{
		body
			.composeBYTE(accountNetworkId)
			.composeUINT64(idChAccount)
			.composeString(accountCode)
			.composeINT32(balanceAmount);
	}
	const char* toString(PString& out) const
	{
		// TODO
		return out.c_str();
	}
};

struct PgadDormantGameAccountReportData // PYR-50515
{
	UINT64 transactionid;
	PString receiptid;
	PgadDormantGameAccountsDetails accountDetails;

	PgadDormantGameAccountReportData() : transactionid(0) {}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseUINT64(transactionid)
			.parseStringP(receiptid)
			;
		accountDetails.parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body
			.composeUINT64(transactionid)
			.composeString(receiptid)
			;
		accountDetails.compose(body);
	}
};

enum eAamsEndTournFlightType
{
	eAamsNonFinalFlightRollForward = 1,
	eAamsFinalFlightEnd = 2,
};

// PYR-33375 for message (4.18)
struct PgadGameAccountWoSubRegistration
{
	BYTE    accountNetworkId; // Account Network ID
	UINT64  idChAccount;      // ID CH Account
	PString accountCode;      // Account Code
	INT32   balanceAmount;    // Balance Amount

	void compose(CommMsgBody& body) const
	{
		body
			.composeBYTE(accountNetworkId)
			.composeUINT64(idChAccount)
			.composeString(accountCode)
			.composeINT32(balanceAmount)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseBYTE(accountNetworkId)
			.parseUINT64(idChAccount)
			.parseStringP(accountCode)
			.parseINT32(balanceAmount)
			;
	}
	PgadGameAccountWoSubRegistration()
		: accountNetworkId(0)
		, idChAccount(0)
		, balanceAmount(0)
	{}
};

// PYR-47234
//	                                      Device Code
//	                     Android/Unix like	iOS	     Windows	Other OS
//	Desktop + browser	        10	        11	       12		   14
//	Desktop + app	            20	        21	       22    	   24
//	Mobile + browser	        30	        31	       32	  	   34
//	Smartphone + app	        40	        41	       42	 	   44
//	Tablet + app	            50	        51	       52	  	   54

// do not support such devices: 
// telephone betting with  operator, automatic telephone betting, TV dedicated channel, Blackberry OS

// default device code is 22. (desktop windows application.)


class AamsDeviceCodeUtils
{
	static const UINT16 IT_DESKTOP_BROWSER		= 10;
	static const UINT16 IT_DESKTOP_APP			= 20;
	static const UINT16 IT_MOBILE_BROWSER		= 30;
	static const UINT16 IT_PHONE_APP			= 40;
	static const UINT16 IT_TABLET_APP			= 50;

	static const UINT16 IT_OS_UNIX_ANDROID_MOD	= 0;
	static const UINT16 IT_OS_IOS_MOD			= 1;
	static const UINT16 IT_OS_WINDOWS_MOD		= 2;
	static const UINT16 IT_OS_OTHER_MOD			= 4;

	static const UINT16 IT_DEFAULT_DEVICE_CODE	= 22; 
public:
	// clientDeviceId is refer to enum ClientDevice in clientplatforms.h
	// appclicationType is refer to enum AppTypeId in clientplatforms.h
	// oSId is refer to enum OSId in clientplatforms.h

	// if any value in client device id, osId, appTypeId is unknown, will return default device code.
	static UINT16 getDeviceCode( const UINT32 clientDeviceId, const UINT32 appTypeId, const UINT32 oSId )
	{	
		PLog( "ClientDevice=%u OSId=%u AppTypeId=%u", clientDeviceId, oSId, appTypeId );

		if( ClientDevice::ClientDevice_Unknown == clientDeviceId || OSId::OSId_Unknown == oSId || AppTypeId::AppTypeId_Unknown == appTypeId )
		{
			return IT_DEFAULT_DEVICE_CODE;
		}

		UINT16 osCodeModifier = IT_OS_OTHER_MOD;
		switch( oSId )
		{
			case OSId::OSId_Windows:	osCodeModifier	= IT_OS_WINDOWS_MOD;		break;
			case OSId::OSId_iOS:		osCodeModifier	= IT_OS_IOS_MOD;			break;
			case OSId::OSId_AndroId:
			case OSId::OSId_Linux:		osCodeModifier	= IT_OS_UNIX_ANDROID_MOD;	break;
		}

		const bool isNativeExecutable = AppTypeRules::isNativeApp(appTypeId);

		if( isNativeExecutable )
		{
			switch( clientDeviceId )
			{
			case ClientDevice::ClientDevice_Desktop:	return IT_DESKTOP_APP + osCodeModifier;
			case ClientDevice::ClientDevice_Tablet:		return IT_TABLET_APP + osCodeModifier;
			case ClientDevice::ClientDevice_Phone:		return IT_PHONE_APP + osCodeModifier;
			}
		}
		else // browser
		{
			switch( clientDeviceId )
			{
			case ClientDevice::ClientDevice_Desktop:	return IT_DESKTOP_BROWSER + osCodeModifier;
			case ClientDevice::ClientDevice_Tablet:		
			case ClientDevice::ClientDevice_Phone:		return IT_MOBILE_BROWSER + osCodeModifier;
			}
		}
		return IT_DEFAULT_DEVICE_CODE;
	}
};

// PYR-67720
struct PgadSelfExclusionDetails  // for message 4.25
{
	BYTE    accountNetworkId;
	UINT64  idChAccount;
	PString userIntIdAsStr; // PS userIntId passed to regulator as string
	BYTE    selfExclusionType; // see defines starting from PGAD_SELFEXCLUSION_STATUS_NONE
	SrvTime startDate; // CET
	SrvTime endDate;   // CET

	PgadSelfExclusionDetails() : accountNetworkId(0), idChAccount(0), selfExclusionType(PGAD_SELFEXCLUSION_STATUS_NONE) {}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseBYTE(accountNetworkId)
			.parseUINT64(idChAccount)
			.parseStringP(userIntIdAsStr)
			.parseBYTE(selfExclusionType)
			.parseSrvTime(startDate)
			.parseSrvTime(endDate)
			;
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeBYTE(accountNetworkId)
			.composeUINT64(idChAccount)
			.composeString(userIntIdAsStr)
			.composeBYTE(selfExclusionType)
			.composeSrvTime(startDate)
			.composeSrvTime(endDate)
			;
	}

	const char* toString(PString& out) const
	{
		PString logStr1, logStr2;
		out.assign("")
			.append("nId=").appendInt(accountNetworkId)
			.append(",aId=").appendUint64(idChAccount)
			.append(",uId=").append(userIntIdAsStr)
			.append(",seT=").appendInt(selfExclusionType)
			.append(",sD=").append(srvTimeToString(startDate, logStr1))
			.append(",eD=").append(srvTimeToString(endDate, logStr2));
		return out.c_str();
	}

	bool checkData() const
	{
		for (int i = 0; i < userIntIdAsStr.length(); ++i)
		{
			if (!isdigit(userIntIdAsStr[i]))
				return false;
		}
		return true;
	}

};
#endif //aamsgateway_h_included
