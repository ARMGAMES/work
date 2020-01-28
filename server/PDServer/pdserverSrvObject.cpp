////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "pdserverSrvObject.h"
#include "pdserverConnections.h"
#include "pdserver.h"

////////////////////////////////////////////////////////////////////////////////

PDServerObject::PDServerObject(_CommInterface& inter_)
	: GenericServerWithLoginAuthObjectBase(inter_)
{
	connAdminFactory = make_unique<AdminSrvConnFactory>(*this, authGuardFactory);
}

/* virtual */
const char* PDServerObject::getConfigFileName() const
{
	return PDSERVER_CONFIG_FILE_NAME;
}

/* virtual */
const char* PDServerObject::getLoginAuthDbmConnType() const
{
	return PDSRV_LOGINAUTH_CONNTYPE;
}

////////////////////////////////////////////////////////////////////////////////