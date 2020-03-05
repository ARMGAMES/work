
#include "MainDbmobject.h"

////////////////////////////////////////////////////////////////////////////////////////

/* process messages */

void MainDbmServerObject::processInsertUser(CommMsgParser& parser, GenericSrvConnection* conn)
{
	PLog(__FUNCTION__);

	UINT32 reqId = 0;
	User user;

	parser
		.parseUINT32(reqId);

	user.parse(parser);
	user.registered.currentLocalTime();

	CommMsgBody reply;
	reply.composeUINT32(reqId);

	PString sqlErr;
	INT16 errCode = dbManager.insertUser(user, sqlErr);

	reply.composeINT16(errCode);
	if (errCode)
	{
		reply.composeString(sqlErr);
	}

	conn->post(DBM_A_INSERT_USER, reply);
}


void MainDbmServerObject::processGetUsers(CommMsgParser& parser, GenericSrvConnection* conn)
{
	PLog(__FUNCTION__);

	UINT32 reqId = 0;
	UINT32 count = 0;
	vector<PString> userIds;

	parser
		.parseUINT32(reqId)
		.parseUINT32(count);
	userIds.resize(count);
	for (int i = 0; i < count; ++i)
	{
		parser.parseStringP(userIds[i]);
	}

	CommMsgBody usersBody;

	CommMsgBody reply;
	reply.composeUINT32(reqId);

	PString sqlErr;
	INT16 errCode = dbManager.getUsers(userIds, usersBody, sqlErr);

	reply.composeINT16(errCode);
	if (errCode)
	{
		reply.composeString(sqlErr);
	}
	else
	{
		reply.merge(usersBody);
	}
	conn->post(DBM_A_INSERT_USER, reply);
}