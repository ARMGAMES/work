#ifndef PredictorAPI_Msgs_H
#define PredictorAPI_Msgs_H

#define PREDICTORSRV_CONN_AuthAPI    "PredictorAuthAPI"
#define PREDICTORSRV_CONN_PlainAPI   "PredictorPlainAPI"

enum PredictorAPI_MsgIDs
{
	Q_GET_USER_INFO                  = 9041   // Victiv AuthAPI
,	A_GET_USER_INFO                  = 9042

,	Q_SPEND_TICKET                   = 9043   // Victiv AuthAPI
,	A_SPEND_TICKET                   = 9044

,	Q_VOID_TICKETS                   = 9045   // Victiv PlainAPI
,	A_VOID_TICKETS                   = 9046

,	Q_GET_RECENT_TICKET_TRANSACTIONS = 9047   // Victiv PlainAPI
,	A_GET_RECENT_TICKET_TRANSACTIONS = 9048

,	Q_TICKET_ISSUE_ICE               = 9049   // ICE
,	A_TICKET_ISSUE_ICE               = 9050

,	Q_UPDATE_TICKET_STATUS_ICE       = 9051   // ICE
,	A_UPDATE_TICKET_STATUS_ICE       = 9052

,	Q_GET_TICKETS                    = 9053   // LobbyEtc
,	A_GET_TICKETS                    = 9054

,	Q_NOTIFY_TICKETS                 = 9055   // Notification to LobbyEtc (should be defined in LobbyEtc headers)
,	A_NOTIFY_TICKETS                 = 9056

,	Q_TICKET_ISSUE_CRM               = 9057   // WebToolsCRM
,	A_TICKET_ISSUE_CRM               = 9058

,	Q_UPDATE_TICKET_STATUS_CRM       = 9059   // WebToolsCRM
,	A_UPDATE_TICKET_STATUS_CRM       = 9060

,	Q_TICKET_ISSUE_AUTOMATION        = 9061   // Automation
,	A_TICKET_ISSUE_AUTOMATION        = 9062

,	Q_UPDATE_TICKET_STATUS_AUTOMATION= 9063   // Automation
,	A_UPDATE_TICKET_STATUS_AUTOMATION= 9064

,   Q_EXPIRE_ALL_NEW_TICKETS         = 9065  // was TrustedAdmin, now accepted from WebToolsCRM (currently they don't send this message)
,   A_EXPIRE_ALL_NEW_TICKETS         = 9066

,	Q_TICKET_ISSUE_REGULATOR         = 9067   // Regulator issues tickets from pending actions which were saved when automation tries to issue tickets for archived users
,	A_TICKET_ISSUE_REGULATOR         = 9068
};

#endif // PB_Server_WAPI_Msgs_H
