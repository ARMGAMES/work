#ifndef commappfactoryinterface_h_included
#define commappfactoryinterface_h_included

#define COMMAPPFACTORY_CREATEOBJECT			0x1357 //why not?
//"4ssm" requestId, objectType, objectName, initMsg
#define COMMAPPFACTORY_CREATEOBJECT_REPLY	0x1358
//"42" ["s"] requestId, errCode, [errMsg]
#define COMMAPPFACTORY_DUMP					0x135a
#define COMMAPPFACTORY_DUMP_REPLY			0x135b

#define COMMAPPFACTORY_DYNAMIC_INIT         0x135e
#define COMMAPPFACTORY_DYNAMIC_INIT_REPLY   0x135f

#define COMMAPPFACTORY_DUMP_EX				0x1360
#define COMMAPPFACTORY_DUMP_REPLY_EX		0x1361

#define COMMAPPFACTORY_CHANGE_EXE_NAME		 0x1362
#define COMMAPPFACTORY_CHANGE_EXE_NAME_REPLY 0x1363

#define COMMAPPFACTORY_LOGFILE				0x1364 //"4s" requestId, objectName
#define COMMAPPFACTORY_LOGFILE_REPLY		0x1365

#define COMMAPPFACTORY_LOGFILE_REOPEN		0x1366
#define COMMAPPFACTORY_LOGFILE_REOPEN_REPLY	0x1367

#define COMMAPPFACTORY_GET_OBJECT_BY_PID		0x1368 // 44 reqId, PID -  request Id, PID
#define COMMAPPFACTORY_GET_OBJECT_BY_PID_REPLY	0x1369 // 46[s|4<s>]  - request Id, errCode [errDescr or vector of object names]

#define COMMAPPFACTORY_CHANGE_LOGFILE_DIR		0x136A 
#define COMMAPPFACTORY_CHANGE_LOGFILE_DIR_REPLY	0x136B 

#define COMMAPPFACTORY_REOPEN_OBJECT_LOGFILE		0x136C 
#define COMMAPPFACTORY_REOPEN_OBJECT_LOGFILE_REPLY	0x136D 

#define COMMAPPFACTORY_GET_PHYS_CONNECTIONS_STATUS			0x136E 
#define COMMAPPFACTORY_GET_PHYS_CONNECTIONS_STATUS_REPLY	0x136F 




#define COMMAPPFACTORY_ERROR_OK				0x00
#define COMMAPPFACTORY_ERROR_UNKNOWNTYPE	0x01
#define COMMAPPFACTORY_ERROR_INITFAILED		0x02
#define COMMAPPFACTORY_ERROR_DUPLICATENAME	0x03
#define COMMAPPFACTORY_ERROR_NOTSUPPORTED	0x04
#define COMMAPPFACTORY_ERROR_NOTEXIST		0x05

#define _COMMAPPFACTORY_KEEPALIVE			0x1359

#endif
