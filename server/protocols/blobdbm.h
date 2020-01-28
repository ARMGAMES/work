#ifndef blobdbm_h_included
#define blobdbm_h_included

//
// BLOB DBM messages
//

#define BLOB_DBM_REPLY_DELTA 1000

//
// Messages
//

// From LOBBY, Regulator, Admin
#define BDBM_QS_SAVE_IMAGE	100 // 4Bs44 - reqId, imagePBlock, imageDescr, imageStatus, imageId, height, width
#define BDBM_AS_SAVE_IMAGE	(BDBM_QS_SAVE_IMAGE + BLOB_DBM_REPLY_DELTA) // 1100  46[s|4] - reqId, errCode[errDescr|imageId]

#define BDBM_QS_DELETE_PRIVATE_IMAGE	101 // 44 - reqId, imageId
#define BDBM_AS_DELETE_PRIVATE_IMAGE	(BDBM_QS_DELETE_PRIVATE_IMAGE + BLOB_DBM_REPLY_DELTA) // 1101  46[s] - reqId, errCode[errDescr]

#define BDBM_Q_SAVE_SITNGO	102 // 4sss4<IniFile> - reqId, title, filename, createdBy, numScripts<Script>
#define BDBM_A_SAVE_SITNGO	(BDBM_Q_SAVE_SITNGO + BLOB_DBM_REPLY_DELTA) // 1102 46[s|4<4>] - reqId, errCode[errDescr|numScripts<scriptId>]

#define BDBM_Q_GET_SITNGO	103 // 4 - reqId
#define BDBM_A_GET_SITNGO	(BDBM_Q_GET_SITNGO + BLOB_DBM_REPLY_DELTA) // 1103  46[s|4<ss4ssIniFile>] - reqId, errCode[errDescr|numScripts<title, filename, scriptId, createdBy, updatedBy, Script>]

#define BDBM_Q_GET_DEFAULT_IMAGE_AND_RESOLUTIONS	104  //44 req_id, imageId
#define BDBM_A_GET_DEFAULT_IMAGE_AND_RESOLUTIONS	(BLOB_DBM_REPLY_DELTA + BDBM_Q_GET_DEFAULT_IMAGE_AND_RESOLUTIONS) // 1104  //46[s|B444<44>] req_id, err_code, [errDescr|block,height,width,numRes<Height,Widtd>] 

#define BDBM_Q_SAVE_SAG		105 // 4sss4<IniFile> - reqId, title, filename, createdBy, numScripts<Script>
#define BDBM_A_SAVE_SAG		(BDBM_Q_SAVE_SAG + BLOB_DBM_REPLY_DELTA) // 1105 46[s|4<4>] - reqId, errCode[errDescr|numScripts<scriptId>]

#define BDBM_Q_GET_SAG		106 // 4 - reqId
#define BDBM_A_GET_SAG		(BDBM_Q_GET_SAG + BLOB_DBM_REPLY_DELTA) // 1106  46[s|4<ss4ssIniFile>] - reqId, errCode[errDescr|numScripts<title, filename, scriptId, createdBy, updatedBy, Script>]

#define BDBM_Q_SAVE_TABLES_SCRIPT		107 // 4IniFile - reqId, script
#define BDBM_A_SAVE_TABLES_SCRIPT		(BDBM_Q_SAVE_TABLES_SCRIPT + BLOB_DBM_REPLY_DELTA) // 1107   46[s] - reqId, errCode[errDescr]

#define BDBM_Q_GET_LAST_TABLES_SCRIPT	108 // 4 - reqId
#define BDBM_A_GET_LAST_TABLES_SCRIPT	(BDBM_Q_GET_LAST_TABLES_SCRIPT + BLOB_DBM_REPLY_DELTA) // 1108 46[s|IniFile] - reqId, errCode[errDescr|Script]

#define BDBM_Q_SAVE_MINIGAME_SCRIPT		109 // 4s4ss - reqId,createdBy,adminIntId,fileName,script
#define BDBM_A_SAVE_MINIGAME_SCRIPT     (BDBM_Q_SAVE_MINIGAME_SCRIPT + BLOB_DBM_REPLY_DELTA) // 1109  46[s|4] - reqId, errCode[errDescr|scriptId]

#define BDBM_Q_GET_MINIGAME_SCRIPT		110 // 44 - reqId, scriptId
#define BDBM_A_GET_MINIGAME_SCRIPT		(BDBM_Q_GET_MINIGAME_SCRIPT + BLOB_DBM_REPLY_DELTA) // 1110  46[s|s] - reqId, errCode[errDescr|script]

#define BDBM_Q_EXPIRE_SAG		111 // 4s4<4>  reqId, updater, numScripts<scriptId>
#define BDBM_A_EXPIRE_SAG		(BDBM_Q_EXPIRE_SAG + BLOB_DBM_REPLY_DELTA) // 1111  46[s] - reqId, errCode[errDescr]

#define BDBM_Q_SAVE_CVLCONFIG_SCRIPT		112 // 4ssss - reqId,submittedBy,reviewedBy,fileName,script
#define BDBM_A_SAVE_CVLCONFIG_SCRIPT     (BDBM_Q_SAVE_CVLCONFIG_SCRIPT + BLOB_DBM_REPLY_DELTA) // 1112  46[s|4] - reqId, errCode[errDescr|scriptId]

#define BDBM_Q_GET_CVLCONFIG_SCRIPTS		113 // 44<4> - reqId,numScriptIds<scriptIds>
#define BDBM_A_GET_CVLCONFIG_SCRIPTS		(BDBM_Q_GET_CVLCONFIG_SCRIPTS + BLOB_DBM_REPLY_DELTA) // 1113  46[s|s] - reqId, errCode[errDescr|script]

#define BDBM_Q_SAVE_PPP_SCRIPT				114 // 44sss4<IniFile> - reqId, pppId, title, filename, createdBy, numScripts<Script> 
#define BDBM_A_SAVE_PPP_SCRIPT				(BDBM_Q_SAVE_PPP_SCRIPT + BLOB_DBM_REPLY_DELTA) // 1114 46[s|4<4>] - reqId, errCode[errDescr|numScripts<scriptId>]

#define BDBM_Q_GET_PPP_SCRIPTS				115 // 4 - reqId
#define BDBM_A_GET_PPP_SCRIPTS				(BDBM_Q_GET_PPP_SCRIPTS + BLOB_DBM_REPLY_DELTA) // 1115  46[s|4<4ss4ssIniFile>] - reqId, errCode[errDescr|numScripts<pppId, title, filename, scriptId, createdBy, updatedBy, Script>]


// moved from dbmmsg.h - message Ids preserved
// PYR-26568
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define BDBM_Q_IMAGE_WITH_RESOLUTION		7028  //4444 req_id, imageId, height, width
#define BDBM_A_IMAGE_WITH_RESOLUTION		(BDBM_Q_IMAGE_WITH_RESOLUTION + BLOB_DBM_REPLY_DELTA) // 8028  //46[s|B] req_id, err_code, [errDescr|block] 

// PYR-26568
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define BDBM_Q_IMAGE_OR_RESOLUTIONS		7029  //44 req_id, imageId
#define BDBM_A_IMAGE_OR_RESOLUTIONS		(BDBM_Q_IMAGE_OR_RESOLUTIONS + BLOB_DBM_REPLY_DELTA) // 8029  //46[s4<44>|B44>] req_id, err_code, [errDescr,numRes<height,width>|block,height,width>] 


#endif // blobdbm_h_included
