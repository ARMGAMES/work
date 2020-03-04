
#include "MainDbmobject.h"

/*Lobby messages*/

void MainDbmServerObject::processLobbyMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn)
{
	CommMsgParser parser(body);
	switch (msgId)
	{
		case DBM_Q_INSERT_USER:
		{
			processInsertUser(parser, conn);
			break;
		}
		case DBM_Q_GET_USERS:
		{
			processGetUsers(parser, conn);
			break;
		}
		default:
		{
			PLog(__FUNCTION__" error ... unhandled message ID=%u", msgId);
			break;
		}
	}
}