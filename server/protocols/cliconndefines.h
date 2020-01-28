//------------------------------------------------------------------------------
//	cliconndefines.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2018 PYR Software Ltd. All rights reserved. This document
//	is for the use of licensed users only and is subject to the terms and
//	conditions of the accompanying and/or applicable license between you and
//	PYR Software Ltd, including without limitation those terms and conditions
//	prohibiting unauthorized translation, reverse engineering, decompilation,
//	disassembly, and other inappropriate use of any software provided. Any
//	unauthorized use, copying, modification, distribution, publication or
//	disclosure of information is a violation of copyright laws. No reproduction
//	in whole or in part of this document may be made without express written
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#if !defined( __cliconndefines_h_included__ )
#define __cliconndefines_h_included__

//------------------------------------------------------------------------------

#define CLIENT_CONNTYPE_PREFIX		"CLI."

//------------------------------------------------------------------------------

#define PUBLIC_CLIENT_CONNTYPE		CLIENT_CONNTYPE_PREFIX "NonAuth"
#define TABLE_CLIENT_CONNTYPE		CLIENT_CONNTYPE_PREFIX "table_client"
#define TOURN_CLIENT_CONNTYPE		CLIENT_CONNTYPE_PREFIX "butler"
#define BLITZ_CLIENT_CONNTYPE		CLIENT_CONNTYPE_PREFIX "blobby_client"
#define IHHR_CLIENT_CONNTYPE		CLIENT_CONNTYPE_PREFIX "ihhr_client"
#define ICP_CLIENT_CONNTYPE			CLIENT_CONNTYPE_PREFIX "icp_client"
#define P13N_CLIENT_CONNTYPE		CLIENT_CONNTYPE_PREFIX "pserver_client"
#define PPP_CLIENT_CONNTYPE			CLIENT_CONNTYPE_PREFIX "client"

//------------------------------------------------------------------------------
#endif // !defined( __cliconndefines_h_included__ )
//------------------------------------------------------------------------------
