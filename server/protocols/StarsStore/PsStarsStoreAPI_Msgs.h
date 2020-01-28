#ifndef PsStarsStoreAPI_Msgs_H
#define PsStarsStoreAPI_Msgs_H

#define SST_SRV_CONN_AuthAPI    "StarsStoreAuthAPI"

enum PsStarsStoreAPI_MsgIDs
{
	MSG_SST_Q_GetUserInfo    = 9041   // auth
,	MSG_SST_A_GetUserInfo    = 9042
,	MSG_SST_Q_DebitStarsCoin = 9043
,	MSG_SST_A_DebitStarsCoin = 9044
};

#endif // PsStarsStoreAPI_Msgs_H
