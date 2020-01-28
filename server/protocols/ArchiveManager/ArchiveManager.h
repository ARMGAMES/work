////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Stars Technologies Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//	ArchiveManager.h
#ifndef ArchiveManager_h_included
#define ArchiveManager_h_included

#include "ppincludebase.h"

////////////////////
//	Connection types
#define AUTOMATION_TO_ARCHMAN_CONN_TYPE			"automation"
#define CASHIER_TO_ARCHMAN_CONN_TYPE			"cashier"
#define JOBSCONTROLLER_TO_ARCHMAN_CONN_TYPE		"jobscontroller"
#define LDBM_TO_ARCHMAN_CONN_TYPE				"logindbm"
#define REGULATOR_TO_ARCHMAN_CONN_TYPE			"regulator"
#define DBD_TO_ARCHMAN_CONN_TYPE				"dbd"
#define COLLUSION_TO_ARCHMAN_CONN_TYPE			"collusion"
#define CESAR_TO_ARCHMAN_CONN_TYPE				"mailer"
#define CRM_SERVER_TO_ARCHMAN_CONN_TYPE			"crmserver" // PYR-96234

///////////////////
//	enums
enum eUnarchiveReason : INT32
{
	eUnarchiveReason_unknown		= 0,
	eUnarchiveReason_login			= 1,
	eUnarchiveReason_admin			= 2,
	eUnarchiveReason_automation		= 3,
	eUnarchiveReason_payTax			= 4,	//	from cashier (for archived user who has to pay taxes)
	eUnarchiveReason_jobsController	= 5,
	eUnarchiveReason_regBlacklist	= 6,	//	from regulator, blacklist
	eUnarchiveReason_regUsMigr		= 7,	//	from regulator, us migration
	eUnarchiveReason_regPwdReset	= 8,	//	from regulator, password reset
	eUnarchiveReason_adminFraudFlag = 9,	//	from admin, clear fraudulent account flag

	//	If adding here, also update getUnarchReasonDesc()
};


///////////////////
//	Messages

#define ARCHMAN_DELTA									1000

#define ARCHMAN_Q_UNARCHIVE_USER						70000	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_UNARCHIVE_USER						(ARCHMAN_DELTA + ARCHMAN_Q_UNARCHIVE_USER)	//	71000

#define ARCHMAN_Q_UNARCHIVE_USER_FROM_LOGIN				70001	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_UNARCHIVE_USER_FROM_LOGIN				(ARCHMAN_DELTA + ARCHMAN_Q_UNARCHIVE_USER_FROM_LOGIN)	//	71001

#define ARCHMAN_Q_ARCHIVE_USER							70002	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_ARCHIVE_USER							(ARCHMAN_DELTA + ARCHMAN_Q_ARCHIVE_USER)	//	71002

#define ARCHMAN_Q_SET_ARCH_USER_FLAGS_PRIVS_MASK		70003	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_SET_ARCH_USER_FLAGS_PRIVS_MASK		(ARCHMAN_DELTA + ARCHMAN_Q_SET_ARCH_USER_FLAGS_PRIVS_MASK)	//	71003

#define ARCHMAN_Q_CLOSE_ARCH_ACCOUNT					70004	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_CLOSE_ARCH_ACCOUNT					(ARCHMAN_DELTA + ARCHMAN_Q_CLOSE_ARCH_ACCOUNT)	//	71004

#define ARCHMAN_Q_SET_ARCH_USER_COMM_PREFS				70005	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_SET_ARCH_USER_COMM_PREFS				(ARCHMAN_DELTA + ARCHMAN_Q_SET_ARCH_USER_COMM_PREFS)	//	71005

#define ARCHMAN_Q_GET_ARCH_USERS_BY_EMAIL				70006	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_GET_ARCH_USERS_BY_EMAIL				(ARCHMAN_DELTA + ARCHMAN_Q_GET_ARCH_USERS_BY_EMAIL)		//	71006

#define ARCHMAN_Q_ARCH_CHECK_USER_UPDATE				70007	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_ARCH_CHECK_USER_UPDATE				(ARCHMAN_DELTA + ARCHMAN_Q_ARCH_CHECK_USER_UPDATE)	//	71007

#define ARCHMAN_Q_ARCH_CHECK_NEW_USER					70008	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_ARCH_CHECK_NEW_USER					(ARCHMAN_DELTA + ARCHMAN_Q_ARCH_CHECK_NEW_USER)		//	71008

#define ARCHMAN_Q_PAY_INDIA_ARCH_USER_ZERO_ANNUAL_TAX	70009	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_PAY_INDIA_ARCH_USER_ZERO_ANNUAL_TAX	(ARCHMAN_DELTA + ARCHMAN_Q_PAY_INDIA_ARCH_USER_ZERO_ANNUAL_TAX)		//	71009

#define ARCHMAN_Q_GET_ARCH_USER_WITH_PROPS				70010	//	protocols/ArchiveManager/ArchiveManagerProt.txt
#define ARCHMAN_A_GET_ARCH_USER_WITH_PROPS				(ARCHMAN_DELTA + ARCHMAN_Q_GET_ARCH_USER_WITH_PROPS)	//	71010

#endif	//	ArchiveManager_h_included
