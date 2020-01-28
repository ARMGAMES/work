////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericDbmSrvObject_h_included
#define GenericDbmSrvObject_h_included

////////////////////////////////////////////////////////////////////////////////

#include "GenericSrvObject.h"
#include "GenericDbmmanager.h"

////////////////////////////////////////////////////////////////////////////////

class GenericDbmServerObjectBase : public GenericServerObjectBase
{
	using Parent_ = GenericServerObjectBase;

protected:

	unique_ptr<GenericDbmManager> dbManager;

public:

	explicit GenericDbmServerObjectBase(_CommInterface& inter_);
	virtual ~GenericDbmServerObjectBase() { }

	virtual void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn) override;

protected:

	// Overridden Life cycle events. Override again as required
	virtual void doPostInit() override;
	virtual void doPreDynamicInit(const PIniFile& ini) override;
	virtual void doPostDynamicInit(const PIniFile& iniFile) override;
	virtual void doComposePerformanceStats(CommMsgBody& reportBody) override;
};

////////////////////////////////////////////////////////////////////////////////

class GenericDbmServerWithLoginAuthObjectBase : public GenericServerWithLoginAuthObjectBase
{
	using Parent_ = GenericServerWithLoginAuthObjectBase;

protected:
	unique_ptr<GenericAdminDbmServerConnBaseFactory> connAdminFactory;

	unique_ptr<GenericDbmManager> dbManager;

public:

	explicit GenericDbmServerWithLoginAuthObjectBase(_CommInterface& inter_);
	virtual ~GenericDbmServerWithLoginAuthObjectBase() { }

	virtual void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn) override;

protected:

	// Overridden Life cycle events. Override again as required
	virtual void doPostInit() override;
	virtual void doPostDynamicInit(const PIniFile& iniFile) override;
	virtual void doComposePerformanceStats(CommMsgBody& reportBody) override;
};

////////////////////////////////////////////////////////////////////////////////

#endif // GenericDbmSrvObject_h_included
