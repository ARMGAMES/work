#pragma once

//////////////////////////////////////////////////////////////////////////////
//
// HOW TO ADD NEW MESSAGES:
// 1. define new message ID above "Add new messages above this comment!!!" comment
// 2. add reply in the new format e.g. (DBM_DELTA + DBM_Q_YOUR_MSG)
// 3. do not forget to add numeric replyID in the comment 
//
/////////////////////////////////////////////////////////////////////////////

#define DBM_DELTA           1000

//////////////////////////////////////Communication protocol Ids:
#define	DBM_Q_KILL_TABLES	108
#define	DBM_A_KILL_TABLES	109


//
// DBM REQUESTS AND REPLIES
//
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_USER_AUTH							1001  // 4sssss714m4 - q_id, usernick, macAddr, volumeLabel, installId, ipAddr, port, objectType, objectId(see OBJ_TYPE_* constants), messagebody, platformId
#define DBM_A_USER_AUTH							(DBM_DELTA + DBM_Q_USER_AUTH) // 2001	

#define DBM_Q_LOGIN_CONFIRM						1002 	// 4s48 reqId, userId,loginAuthMethod,appSessionId
#define DBM_A_LOGIN_CONFIRM						(DBM_DELTA + DBM_Q_LOGIN_CONFIRM) // 2002 	// 4s444<M>8s444444M88 - reqId, userId, correctedFlags, effectivePrivs, numBannedUsers<bannedUser>, effectivePrivs2, balanceCurrency, chips, playChips, tChips, wChips, owedChips, flags2, licenseSpecificBody, priveleges3, flags3

#define DBM_Q_USER_LOGOUT						1003  //418s484 reqId, objType, objId, userName, logoutType, appSessionId, clientType
#define DBM_A_USER_LOGOUT						(DBM_DELTA + DBM_Q_USER_LOGOUT) // 2003  //42[s] q_id, err [, errDescr] 

#define DBM_Q_INSERT_USER						1004
#define DBM_A_INSERT_USER						(DBM_DELTA + DBM_Q_INSERT_USER) // 2004  //42[S]
