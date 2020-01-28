////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GenericDbmSrvObject.h"

////////////////////////////////////////////////////////////////////////////////

GenericDbmServerObjectBase::GenericDbmServerObjectBase(_CommInterface& inter_)
	: Parent_(inter_)
{

}

void GenericDbmServerObjectBase::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn)
{
	// This function is a re-implementation. Parent function *not* called on purpose
	// Parent_::processCommonServerMessage(msgId, body, srvConnType, conn);

	if (!isConnTypeSupported(srvConnType))
	{
		PString buf("Unsupported ServerConnectionType value: ");
		buf.appendInt(srvConnType);
		throw PError(buf);
	}

	PString extraLogMarkers;
	conn->getExtraLogMarkers(extraLogMarkers);

	PLog("{%s %u %08x%s", conn->getConnMarker(), msgId, conn->id(), extraLogMarkers.c_str());

	HiresCounter counter;

	dbManager->setAutoCommitTrue();
	bool Ok = true;
	for (int n = 0; n < dbManager->getMaxDbReconnectAttempts(); ++n)
	{
		try
		{
			if (Ok)
			{
				bool wasHandled = false;

				switch (srvConnType)
				{
					case TrustedAdmin:
					{
						TrustedAdminSrvConn* conn_ = dynamic_cast<TrustedAdminSrvConn*>(conn);
						processTrustedAdminMessage(msgId, body, conn_);

						wasHandled = true;
						break;
					}
					default:
					{
						wasHandled = false;
						break;
					}
				}

				if (!wasHandled)
				{
					doProcessCommonServerMessage(msgId, body, srvConnType, conn);
				}

				break; // out of the loop
			}
		}
		catch (const PDbDisconnectError& er)
		{
			PLog("%s...Disconnect from database detected, reason: '%s', trying to reconnect, attempt #%d,",
				__FUNCTION__, er.why(), n);
		}

		PThread::sleep(dbManager->getDbReconnectIntervalMsec());
		if (!dbManager->openDataStorage(fullFileName, INI_SECTION_NAME_MAINSTORAGE))
		{
			PLog("%s error ...attempt #%d failed", __FUNCTION__, n);
			Ok = false;
		}
		else
		{
			PLog("%s...reconnect OK", __FUNCTION__);
			Ok = true;
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PTRACE("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void GenericDbmServerObjectBase::doPostInit()
{
	Parent_::doPostInit();

	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	dbManager->init(this);

	if (!dbManager->openDataStorage(fullFileName, INI_SECTION_NAME_MAINSTORAGE))
	{
		throw PError("Error: Main data storage not open");
	}
}

void GenericDbmServerObjectBase::doPreDynamicInit(const PIniFile& ini)
{
	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	dbManager->init(this);

	if (!dbManager->openDataStorage(fullFileName, "MAINSTORAGE"))
	{
		throw PError("Error: Main data storage not open");
	}
}

void GenericDbmServerObjectBase::doPostDynamicInit(const PIniFile& iniFile)
{
	Parent_::doPostDynamicInit(iniFile);

	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	dbManager->dynamicInit(iniFile);
}

void GenericDbmServerObjectBase::doComposePerformanceStats(CommMsgBody& reportBody)
{
	Parent_::doComposePerformanceStats(reportBody);

	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	HeLoggerParameters dbManagerParameters;
	dbManager->composeHeLoggerParameters(dbManagerParameters);
	for (const auto& entry : dbManagerParameters)
	{
		const char* name = entry.first;
		const PString& val = entry.second;
		heLogger->compose(reportBody, name, val);
	}
}

////////////////////////////////////////////////////////////////////////////////

GenericDbmServerWithLoginAuthObjectBase::GenericDbmServerWithLoginAuthObjectBase(_CommInterface& inter_)
	: Parent_(inter_)
{

}

void GenericDbmServerWithLoginAuthObjectBase::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn)
{
	// This function is a re-implementation. Parent function *not* called on purpose
	// Parent_::processCommonServerMessage(msgId, body, srvConnType, conn);

	if (!isConnTypeSupported(srvConnType))
	{
		PString buf("Unsupported eServerConnection value: ");
		buf.appendInt(srvConnType);
		throw PError(buf);
	}

	PString extraLogMarkers;
	conn->getExtraLogMarkers(extraLogMarkers);

	PLog("{%s %u %08x%s", conn->getConnMarker(), msgId, conn->id(), extraLogMarkers.c_str());

	HiresCounter counter;

	dbManager->setAutoCommitTrue();
	bool Ok = true;
	for (int n = 0; n < dbManager->getMaxDbReconnectAttempts(); ++n)
	{
		try
		{
			bool wasHandled = false;

			switch (srvConnType)
			{
				case TrustedAdmin:
				{
					TrustedAdminSrvConn* conn_ = dynamic_cast<TrustedAdminSrvConn*>(conn);
					processTrustedAdminMessage(msgId, body, conn_);

					wasHandled = true;
					break;
				}
				default:
				{
					wasHandled = false;
					break;
				}
			}

			if (!wasHandled)
			{
				doProcessCommonServerMessage(msgId, body, srvConnType, conn);
			}

			break; // out of the loop
		}
		catch (const PDbDisconnectError& er)
		{
			PLog("%s...Disconnect from database detected, reason: '%s', trying to reconnect, attempt #%d,",
				__FUNCTION__, er.why(), n);
		}

		PThread::sleep(dbManager->getDbReconnectIntervalMsec());
		if (!dbManager->openDataStorage(fullFileName, INI_SECTION_NAME_MAINSTORAGE))
		{
			PLog("%s error ...attempt #%d failed", __FUNCTION__, n);
			Ok = false;
		}
		else
		{
			PLog("%s...reconnect OK", __FUNCTION__);
			Ok = true;
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PTRACE("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void GenericDbmServerWithLoginAuthObjectBase::doPostInit()
{
	Parent_::doPostInit();

	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	dbManager->init(this);

	if (!dbManager->openDataStorage(fullFileName, INI_SECTION_NAME_MAINSTORAGE))
	{
		throw PError("Error: Main data storage not open");
	}
}

void GenericDbmServerWithLoginAuthObjectBase::doPostDynamicInit(const PIniFile& iniFile)
{
	Parent_::doPostDynamicInit(iniFile);

	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	dbManager->dynamicInit(iniFile);
}

void GenericDbmServerWithLoginAuthObjectBase::doComposePerformanceStats(CommMsgBody& reportBody)
{
	Parent_::doComposePerformanceStats(reportBody);

	if (!dbManager)
	{
		throw PError("Error: dbManager not initialized");
	}

	HeLoggerParameters dbManagerParameters;
	dbManager->composeHeLoggerParameters(dbManagerParameters);
	for (const auto& entry : dbManagerParameters)
	{
		const char* name = entry.first;
		const PString& val = entry.second;
		heLogger->compose(reportBody, name, val);
	}
}

////////////////////////////////////////////////////////////////////////////////
