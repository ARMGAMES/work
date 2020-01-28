
/***************************************************************************************************
**
** Source File Name: db2ApiInstall.h
**
** (C) COPYRIGHT International Business Machines Corp. 1987, 2005
** All Rights Reserved
** Licensed Materials - Property of IBM
**
** US Government Users Restricted Rights - Use, duplication or
** disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
** Function = Include file defining: db2SelectDB2Copy
**
** API Name
** db2SelectDB2Copy - Selects the DB2 copy you want your application to use
**
** A longer description of what the API does:
** The api sets the environment required by your application to use a
** particular DB2 Copy or the location specified.  If your environment is
** already set up for the copy of DB2 you want to use then you do not need to
** call this API.  If however you need to use a different copy of DB2 you must
** call this API.  You must call this API before loading any DB2 dll's within
** your process.  This call can only be made once per process.
**
**
** API parameters and data structure parameters descriptions:
**
** versionNumber:
** Input.
**   Specifies the version number and release level of the variable passed in
**   as the second parameter, pDB2SelectInstallationStruct.
**
** pDB2SelectDB2CopyStruct:
** Input.
**   A pointer to the db2SelectDB2CopyStruct structure.
**
** typedef enum DB2CopyParmType
** {
**    DB2CopyInvalid=0,
**    DB2CopyName,
**    DB2CopyPath
** } DB2CopyParmType;
**
** typedef struct DB2SelectDB2CopyStruct
** {
**    DB2CopyParmType  Type;
**    char             *psziDB2Copy;
** } DB2SelectDB2CopyStruct
**
** Type:
** Input.
**   This can either DB2CopyName or DB2CopyPath.
**
** psziDB2Copy:
** Input.
**   If Type is specified as  DB2CopyName, then psziDB2Copy is a the name of
**   the DB2 copy.        e.g. MYTESTCOPY
**   If Type is specified as  DB2CopyPath, then psziDB2Copy is a the DB2
**   installation path.   e.g. i:\sqllib
**   This cannot be NULL.
**
**
** Usage notes
**
** To use the API, you will need to include db2ApiInstall.h, which will force
** your application to statically link in db2ApiInstall.lib
** In addition this api must be called before loading any db2 libraries and
** can only be called once by an application.  You can avoid loading DB2
** libraries by making use the /delayload option when linking db2 libraries or
** you can load these libraries dynamically using LoadLibraryEx and specifying
** the LOAD_WITH_ALTERED_SEARCH_PATH parameter.
**
** This is a windows only api.
**
** WARNING:
** When db2SelectDB2Copy is called by an application, it will insert the path of
** the selected DB2 copy at the front of the PATH environment of the application
** process. Your application must call db2SelectDB2Copy before any other DB2 APIs.
** Since you are delayloading the DB2 DLLs, when the first DB2 API is called,
** Windows system will then load the DB2 DLLs. All Windows DLL loading rules still
** apply.
** Note that the rules say that unless LoadLibrayEx api is used with the
** LOAD_WITH_ALTERED_SEARCH_PATH flag, "the directory from which the application
** is loaded" and "the current directory" have higher preference than what is
** defined in the PATH environment. YOUR APPLICATION SHOULD NOT BE PLACED IN THE
** SAME DIRECTORY AS THE DB2 DLLS.
**
** Operating System:  Windows NT
**************************************************************************************************/
#ifndef SQL_H_DB2APIINSTALL
#define SQL_H_DB2APIINSTALL

#ifdef __cplusplus
extern "C" {
#endif

#include "db2ApiDf.h"

#ifdef _WIN64
#pragma pack(8)
#else
#pragma pack(4)
#endif

/********************************************************************************/
/* DB2_BUILDING_DB2APIINSTALLLIB is used internally by DB2. Applications do not */
/* need to specify this.                                                        */
/********************************************************************************/
#ifndef DB2_BUILDING_DB2APIINSTALLLIB
#pragma comment(lib, "db2ApiInstall")
#endif

typedef enum DB2CopyParmType
{
   DB2CopyInvalid=0,
   DB2CopyName,
   DB2CopyPath
} DB2CopyParmType;

typedef struct DB2SelectDB2CopyStruct
{
   DB2CopyParmType  Type;
   char             *psziDB2Copy;
} DB2SelectDB2CopyStruct;

SQL_API_RC SQL_API_FN
db2SelectDB2Copy (
                    db2Uint32 versionNumber,
                    void *pDB2SelectDB2CopyStruct
                 );

#ifdef __cplusplus
}
#endif

#endif /* SQL_H_DB2APIINSTALL */
