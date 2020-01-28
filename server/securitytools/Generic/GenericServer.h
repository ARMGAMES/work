////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericServer_h_included
#define GenericServer_h_included

////////////////////////////////////////////////////////////////////////////////

static constexpr const char* ADMIN_CONNTYPE = "admin";
static constexpr const char* TRUSTEDADMIN_CONNTYPE = "trustedAdmin";
static constexpr const char* INI_SECTION_NAME_MAINSTORAGE = "MAINSTORAGE";

////////////////////////////////////////////////////////////////////////////////

enum ServerConnectionType : int
{
	LowerGuard = 0,
	TrustedAdmin, Admin,

	// Add new connection types here, and also to "CONN_TYPES"
	ExampleSrv,
	SecPortalSrv,

	UpperGuard
};

struct CONN_TYPE_STRUCT
{
	int connectionType;
	const char* marker;
};

static const CONN_TYPE_STRUCT CONN_TYPES[] =
{
	{ LowerGuard, "" }
	, { TrustedAdmin, "TA" }
	, { Admin, "A" }
	, { ExampleSrv, "EX" }
	, { SecPortalSrv, "SPS" }
	, { UpperGuard, "" }
};

const char* getConnMarker(int connType);

////////////////////////////////////////////////////////////////////////////////

#endif // GenericServer_h_included
