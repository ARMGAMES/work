#include "pdserverConnections.h"
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "pdserverConnections.h"
#include "dbm.h"
#include "pdserverSrvObject.h"

////////////////////////////////////////////////////////////////////////////////

AdminSrvConn::AdminSrvConn(GenericServerWithLoginAuthObjectBase& obj_)
	: AdminSrvConnBase(obj_)
{
}

BYTE AdminSrvConn::getSrvObjType() const
{
	return _OBJ_TYPE_COLUTIL_SERVER;
}

////////////////////////////////////////////////////////////////////////////////

AdminSrvConnFactory::AdminSrvConnFactory(PDServerObject& obj_, CommServerGuardFactory& guardFactory)
	: GenericAdminSrvConnBaseFactory(obj_, guardFactory)
{
}

CommServerConnection* AdminSrvConnFactory::createConnection()
{
	return new AdminSrvConn(obj);
}

////////////////////////////////////////////////////////////////////////////////
