//////////////////////////////////////////////////////////////////////////

#include "DbTestingObject.h"
#include "counters.h"
#include "admincommon.h"
#include "shared.h"

#define DB_TESTING_SOME_KINDA_MSG		(1000)
#define DB_TESTING_SOME_KINDA_MSG_REPLY	(DB_TESTING_SOME_KINDA_MSG + 1)

//////////////////////////////////////////////////////////////////////////

void DbTestingServerConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	obj->processServerMessage(msgId, body, this);
}

//////////////////////////////////////////////////////////////////////////

void DbTestingObject::processServerMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	PLog("{TestConn %u", msgId);

	HiresCounter counter;

	bool Ok = true;
	for (int n = 0; n < dbManagerMain.getMaxDbReconnectAttempts(); n++)
	{
		try
		{
			if (Ok)
			{
				CommMsgParser parser(body);
				switch(msgId)
				{
				case DB_TESTING_SOME_KINDA_MSG:
					processGetAggEndDate(parser, conn);
					break;
				case MSG_ADMIN_REREAD_INI:
					processRefreshSettings(parser, conn);
						//PIniFile ini(fullFileName);
						//dbManagerMain.readDynamicConfiguration(ini);
					break;
				default:
					PLog("DbTestingObject::processServerMessage error ... unhandled message ID=%u", msgId);
					break;
				}

				break; // out of the loop
			}
		}

		catch (const PDbDisconnectError& er)
		{
			PLog("processServerMessage...Disconnect from database detected, reason: '%s', trying to reconnect, attempt #%d,", er.why(), n);
		}

		PThread::sleep(dbManagerMain.getDbReconnectInterval());
		if (!dbManagerMain.openDataStorage(fullFileName, "MAINSTORAGE"))
		{
			PLog("processServerMessage error ...attempt #%d failed", n);
			Ok = false;
		}
		else
		{
			PLog("processServerMessage...reconnect OK");
			Ok = true;
		}
	}

	counter.stop();
	UINT32 cnt = counter.getElapsedTick();
	PLog("}TestConn %u %u%s", msgId, cnt, (cnt > TICKS_PER_SECOND) ? " overspan" : "");
}


void DbTestingObject::processGetAggEndDate(CommMsgParser& parser, CommServerConnection* conn)
{
	PLog("processGetAggEndDate");

	UINT32 reqId;
	parser.parseUINT32(reqId);

	CommMsgBody reply;
	reply.composeUINT32(reqId);

	SrvDate endDate;
	PString errBuf;
	INT16 errCode = dbManagerMain.getAggEndDate(endDate, errBuf);
	reply.composeINT16(errCode);
	if (errCode)
	{
		reply.composeString(errBuf);
		PLog("Error: failed to get date extents. reason: '%s'", errBuf.c_str());
	}
	else
	{
		reply.composeSrvDate(endDate);
	}

	conn->post(DB_TESTING_SOME_KINDA_MSG_REPLY, reply);
}


void DbTestingObject::processRefreshSettings(CommMsgParser& parser, CommServerConnection* conn)
{
	PLog("processRefreshSettings");

	UINT32 reqId;
	parser.parseUINT32(reqId);

	CommMsgBody reply;
	reply.composeUINT32(reqId);

	try
	{
		PIniFile ini(fullFileName);
		dbManagerMain.readDynamicConfiguration(ini);

		reply.composeINT16(DBM_NO_ERROR);
		conn->post(MSG_ADMIN_REREAD_INI_REPLY, reply);
	}

	catch(const PError& er)
	{
		PLog("Fatal error processing refresh settings.  Server shutdown.  Reason '%s'", er.why());
		shutdown();
		this->exit();

		// No reply
	}
}

//////////////////////////////////////////////////////////////////////////
