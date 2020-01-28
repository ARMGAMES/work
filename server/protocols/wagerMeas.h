// ===========================================================
// Copyright (C) 2016 Amaya Software Ltd. All rights reserved.
// ===========================================================

#ifndef wagerMeas_h_included
#define wagerMeas_h_included

#include "pyrtime.h"

////////////////////////////////////////////////////////////////////////////////
// CONNECTION TYPES

#define WMSRV_CONN_TYPE_TRUSTEDADMIN	"trustedAdmin"
#define WMSRV_CONN_TYPE_ADMIN			"admin"
#define WMSRV_CONN_TYPE_SECSRV			"collusion"
#define WMSRV_CONN_TYPE_ATAS			"atas"
#define WMSRV_CONN_TYPE_TCA				"tca"
#define WMSRV_CONN_TYPE_CCDA			"ccda"
#define WMSRV_CONN_TYPE_CDH				"cdh"
#define WMSRV_CONN_TYPE_CSI				"csi"
#define WMCLIENT_CONN_TYPE				"wagermeas"


////////////////////////////////////////////////////////////////////////////////
// MESSAGE IDS

#define WM_Q_GET_RM_WAGER				990001			// 44tt4<s4>  -  reqId ProductType timeFrom timeTo nUsers<userId userIntId>
#define WM_A_GET_RM_WAGER				990002			// 42[s|4<s888t>] - reqId errCode [errStr|nUsers<userId TotPokCents TotCasCents TotSptCents timestamp>]

// Stats DB
// Note that nUsers in reply could be less than in request if data for any user is not available. Only results from request for lifetime (both from/to are null) will be cached in server
#define WM_Q_GET_STATS_WAGERING			990003			// 44tt4<s4>  -  reqId ProductType timeFrom timeTo nUsers<userId userIntId>
#define WM_A_GET_STATS_WAGERING			990004			// 42[s|4<s4<s8>4<s8>>dd] - reqId errCode [errStr|nUsers<userId numCurrCas<curr Amount> numCurrSports<curr Amount>> lastDateCas lastDateSport]

#define WM_Q_GET_STATS_UPDATE_READY		990005			// 44  -  reqId ProductType
#define WM_A_GET_STATS_UPDATE_READY		990006			// 42[s|44t] - reqId errCode [errStr|nCasinoHostsUpd nSportsHostsUpd updDate]

#define WM_Q_GET_RM_WAGER_FR			990007			// 44tt4<s4>  -  reqId ProductType timeFrom timeTo nUsers<userId userIntId>
#define WM_A_GET_RM_WAGER_FR			990008			// 42[s|4<s888t4<s8>4<s8>4<s8>>] - reqId errCode [errStr|nUsers<userId TotPokCents TotCasCents TotSptCents timestamp nCurPok<cur amt> nCurCas<cur amt> nCurSpt<cur amt>>]

// STAR dbm
#define WM_Q_GET_RT_SPORT_WAGERING		990009			// 4ttt4<s4>  -  reqId timeFrom timeTo nUsers<userId userIntId>
#define WM_A_GET_RT_SPORT_WAGERING		990010			// 42[s|4<s4<s8>>] - reqId errCode [errStr|nUsers<userId numCurr<curr Amount>>]

// OLAPAUX dbm
#define WM_Q_GET_RT_CASINO_WAGERING		990011			// 4ttt4<s4>  -  reqId timeFrom timeTo nUsers<userId userIntId>
#define WM_A_GET_RT_CASINO_WAGERING		990012			// 42[s|4<s4<s8>>] - reqId errCode [errStr|nUsers<userId numCurr<curr Amount>>]


////////////////////////////////////////////////////////////////////////////////

#define WMEAS_UNDEF_WAGER_VALUE			PUINT64_MAX

namespace WagerMeas
{
	enum eProductType
	{
		eProductTypePoker        = 1<<0,
		eProductTypeCasino       = 1<<1,
		eProductTypeSports       = 1<<2,

		eProductTypeAll          = (eProductTypePoker | eProductTypeCasino | eProductTypeSports)
	};

	struct WMUser
	{
		PString id;
		UINT32 intId;
		WMUser() : intId(0) {}
		WMUser(const char* userId_, UINT32 userIntId_) : id(userId_), intId(userIntId_) {}
		void parse(CommMsgParser& parser)
		{
			parser
				.parseStringP(id)
				.parseUINT32(intId)
				;
		}
		void compose(CommMsgBody& body) const
		{
			body
				.composeString(id)
				.composeUINT32(intId)
				;
		}
	};

	struct DateRange
	{
		SrvDate from;
		SrvDate to;
	};

	struct WageringPerCurr
	{
		PString currency;
		UINT64 amount;
		WageringPerCurr() : amount(0) {}
		WageringPerCurr(const char* curr, UINT64 amt) : currency(curr), amount(amt) {}
		void parse(CommMsgParser& parser)
		{
			parser
				.parseStringP(currency)
				.parseUINT64(amount)
				;
		}
		void compose(CommMsgBody& body) const
		{
			body
				.composeString(currency)
				.composeUINT64(amount)
				;
		}
	};
}

#endif
