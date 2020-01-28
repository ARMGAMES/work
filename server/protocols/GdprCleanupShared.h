// GdprCleanupShared.h 
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GdprCleanupShared_h_included
#define GdprCleanupShared_h_included

#include "commonmacros.h"

// Add your own action IDs to eGdprCleanupAction enumeration and
// provide a comment specifying a server module which will perform this action as a part of
// handling of the MSG_ADMIN_GDPR_GET_DATA & MSG_ADMIN_GDPR_CLEANUP messages on connection
// type "automation" for new connections or reuse existed connections with defined connection types.
//
// ***	SUPER IMPORTANT !!! ***
//
//		near your action please provide a comment if your functionality
//		requires automation to make a call to RO dbm (i.e. MSG_ADMIN_GDPR_GET_DATA)
//		to get a collection of table key data which will be updated in the MSG_ADMIN_GDPR_CLEANUP message
//		If your module will orchestrate clean-up itself Automation will not send MSG_ADMIN_GDPR_GET_DATA
//		and will only send MSG_ADMIN_GDPR_CLEANUP_REPLY with empty dataUser and dataContext bodies.
//		Automation will be configured so it will not be sending MSG_ADMIN_GDPR_GET_DATA for your action
//
//		In cases where you want the Automation to orchestrate a clean-up process
//		Automation will send MSG_ADMIN_GDPR_GET_DATA to your RO dbm and pass the data to RW dbm via MSG_ADMIN_GDPR_CLEANUP call
//		See the example of implementation for the Crm DBM CRMDbManager::performGdprCleanupRead/performGdprCleanupWrite
//
// ***** MAKE MANDATORY DB CHANGE NOTES IN RELATED JIRAS !!! ***** 
//
//		When adding new constants to eGdprCleanupActionIDs ensure that:
//		request is submitted to DBAs to insert the new record into GDPRActions table, with action name and description. (PYR-68914)
//
//
enum eGdprCleanupActionIDs
{
	// Expected syntax: eGdprCleanupAction_<DB or Server>_<Action or table Descr> = <value> // server
	// i.e.
	eGdprCleanupAction_Unknown				= 0, // not used
	eGdprCleanupAction_PYRDOC_InitialAll	= 1, // CrmDbm Core server, ***supports calls to RO DBM
	eGdprCleanupAction_BANROOT_InitialAll	= 2, // Ban on Root, ***supports calls to RO DBM
	eGdprCleanupAction_OLTP_InitialAll		= 3, // OLTP cleanup, ***supports calls to RO DBM
	eGdprCleanupAction_PYRDOC_ReplicateOnly	= 4, // CrmDbm Core server, ***supports calls to RO DBM (data in OLAP only)
	eGdprCleanupAction_SECDBM_InitialAll	= 5, // PYRSEC cleanup, ***supports calls to RO DBM
	eGdprCleanupAction_ChatMon_InitialAll	= 6, // Chat Monitor DBM server, ***supports calls to RO DBM
	eGdprCleanupAction_ITGWAY_InitialAll	= 7, // IT Gaming gateway server, ***supports calls to RO instance
	eGdprCleanupAction_OLTP_ReplicateOnly	= 8, // OLTP cleanup, ***supports calls to RO DBM (data in OLAP only via RequestDbm)
	eGdprCleanupAction_PYRLOG_InitialAll	= 9, // PYRLOG cleanup, ***supports calls to RO instance (OLAP via RequestDbm)
	eGdprCleanupAction_PYRCRR_CASERELATEDTABLES = 10, // PYRCRR, no support for RO
	eGdprCleanupAction_PYRDOC_PmtReplicateNever = 11, // ComplianceDbm, ***supports calls to RO ComplianceDBM  // PYR-61358
	eGdprCleanupAction_OLTP_PmtArchiveable      = 12, // OLTP cleanup, ***supports calls to RO DBM  // PYR-67261
	eGdprCleanupAction_PYRLOG_PmtArchiveable    = 13, // LoginDBM, ***supports calls to RO LoginDBM  // PYR-67261
	eGdprCleanupAction_OLTP_PmtAll              = 14, // OLTP cleanup, ***supports calls to RequestDBM  // PYR-67267
	eGdprCleanupAction_OLTP_PmtCashtrans        = 15, // OLTP cleanup, ***supports calls to RequestDBM  // PYR-67271
	eGdprCleanupAction_OLTP_PmtReplicateOnly    = 16, // OLTP cleanup  // PYR-67269
	eGdprCleanupAction_OLTP_PmtUsers			= 17, // OLTP cleanup  // PYR-67255
	eGdprCleanupAction_OLTP_PmtReplicateOnlyUP  = 18, // OLTP cleanup, ***supports calls to RequestDBM  // PYR-67275
	eGdprCleanupAction_PYRFIN_PmtReplicateNever = 19, // GCDBM, ***supports calls to RO GCDBM  // PYR-67277
	eGdprCleanupAction_FREUD_DbCleanup		= 20, // FREUD DBM
	eGdprCleanupAction_OLTP_PmtUserchanges      = 21, // OLTP cleanup  // PYR-67275
	eGdprCleanupAction_PYRLOG_PmtUsers          = 22, // LoginDBM  // PYR-67255
	eGdprCleanupAction_Apal_InitialAll          = 23, // APAL *** no RO dbm, batched users  // PYR-62084
	eGdprCleanupAction_ArchiveDb_InitialAll     = 24, // Archive DB cleanup ***supports calls to RO instance (OLAP via RequestDbm). RW DBM is a special OLAP instance// PYR-68673
	eGdprCleanupAction_OLTP_UserComments        = 25, // OLTP cleanup, ***supports calls to RequestDBM  // PYR-69864
	eGdprCleanupAction_OLTP_PmtUsers_ExtFiscalcode = 26, // OLTP cleanup, extension for 17, executed up to the deployment of 2018.Main.05  // PYR-70064
	eGdprCleanupAction_PYRLOG_PmtUsers_ExtFiscalcode = 27, // LoginDBM, extension for 22, executed up to the deployment of 2018.Main.05  // PYR-70064

	// *** Insert enum value before this line ***
	//
	// *** IMPORTANT: *** 
	// MAKE SURE TO ADD DB CHANGE NOTES TO YOUR JIRA DB CHANGE NOTES TO UPDATE TABLE:
	//
	// GDPRActionTypeList( ActionTypeId,ActionTypeName,ActionTypeDescription) in PYRDOC and OLAP
	//
	ENUM_LAST_VALUE(eGdprCleanupActionIDs) // eGdprCleanupActionIDs_Last & eGdprCleanupActionIDs_NextAfterLast
};

// Automation -> RO dbm connection responsible to get record parameters for clean-up
// dataUser - gets returned by the RO dbm and Automation does not understand it
//            if empty - there is no data to clean and MSG_ADMIN_GDPR_CLEANUP does not get sent
//
// dataContext - may be empty. Automation does not understand it and only passes it to RO dbm on subsequent call. 
// On the first call for a specific user the context is empty
// This is a way for RO dbm (if it needs to) to keep a context where it is in the process of retrieving records for cleanup
//
#define MSG_ADMIN_GDPR_GET_DATA          200000 // 4s44M4 - reqId,userId,userIntId,actionId,dataContext,portionSize
#define MSG_ADMIN_GDPR_GET_DATA_REPLY    200001 // 46[s|4<M>M] - reqID,errCode[errDescr|4<dataUser>,dataContext]
//--------------------------------------------------------^ will be sent to RW dbm one by one in cases where automation orchestrated cleanup

// Automation->RW dbm - dataUser contains data received from RO dbm
// 
#define MSG_ADMIN_GDPR_CLEANUP           200002 // 4s44M4 - reqId,userId,userIntId,actionId,dataUser,portionSize
#define MSG_ADMIN_GDPR_CLEANUP_REPLY     200003 // 46[s|[s44]] - reqID,errCode[errDescr|[userId,userIntId,actionId]]

// depending on the DBM dataUser and dataContext can be empty  if a team does not need to use them.


#endif // GdprCleanupShared_h_included
