////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef PdServerConnections_h_included
#define PdServerConnections_h_included

#include "../securitytools/Generic/GenericConnections.h"

class PDServerObject;

////////////////////////////////////////////////////////////////////////////////

class AdminSrvConn : public AdminSrvConnBase
{
public:
	explicit AdminSrvConn(GenericServerWithLoginAuthObjectBase& obj_);

	/* virtual */ BYTE getSrvObjType() const override;
};

class AdminSrvConnFactory : public GenericAdminSrvConnBaseFactory
{
public:

	AdminSrvConnFactory(PDServerObject& obj_, CommServerGuardFactory& guardFactory);

	/* new */ CommServerConnection* createConnection() override;
};

////////////////////////////////////////////////////////////////////////////////

#endif // PdServerConnections_h_included
