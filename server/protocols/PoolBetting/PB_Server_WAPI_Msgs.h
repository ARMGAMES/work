#ifndef PB_Server_WAPI_Msgs_H
#define PB_Server_WAPI_Msgs_H

#define PB_SERVER_CONN_TYPE_PB_AuthWAPI    "PB_AuthWAPI"
#define PB_SERVER_CONN_TYPE_PB_PlainWAPI   "PB_PlainWAPI"

enum PB_WAPI_MsgIDs
{
	Q_GET_USER_INFO        = 841
,	A_GET_USER_INFO        = 842

,	Q_PLACE_TICKET         = 843
,	A_PLACE_TICKET         = 844

,	Q_SETTLE_TICKET        = 845
,	A_SETTLE_TICKET        = 846

,	Q_CASHOUT_TICKET       = 847
,	A_CASHOUT_TICKET       = 848

,	Q_VOID_TICKET          = 849
,	A_VOID_TICKET          = 850

,	Q_CASHOUT_TICKET2      = 851
,	A_CASHOUT_TICKET2      = 852

//,	Q_UNSETTLE_BET         = 851 deprecated
//,	A_UNSETTLE_BET         = 852 deprecated
};

#endif // PB_Server_WAPI_Msgs_H
