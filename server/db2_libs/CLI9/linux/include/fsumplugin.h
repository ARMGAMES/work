/******************************************************************************
**
** Source File Name: fsumplugin.h
**
** (C) COPYRIGHT International Business Machines Corp. 2007
** All Rights Reserved
** Licensed Materials - Property of IBM
**
** US Government Users Restricted Rights - Use, duplication or
** disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
** Function = Include file for the user mapping plug-in defines
**              
**              
** Operating System: All
**
*******************************************************************************/
#ifndef FS_H_UM_PLUGIN
#define FS_H_UM_PLUGIN

#include "sqlsystm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definition of user mapping option names. */
#define FSUM_REMOTE_AUTHID_OPTION   "REMOTE_AUTHID"
#define FSUM_REMOTE_PASSWORD_OPTION "REMOTE_PASSWORD"

/* Definition of option value types. */
#define FSUM_OPTION_VALUE_BINARY_TYPE 1
#define FSUM_OPTION_VALUE_STRING_TYPE 2

/* Data structure to describe an user option. */
typedef struct _FSUMOption
{
   const  char* optionName;
   size_t optionNameLen;
   char*  optionValue;
   size_t optionValueLen;
   size_t optionValueType;
   struct _FSUMOption* nextOption;
} FSUMOption;

/* Data structure to describe a user mapping entry. A user mapping
   entry might have multiple user mapping options, such as REMOTE_AUTHID 
   and REMOTE_PASSWORD. These options are placed in a linked-list.  This 
   data structure holds the pointer to the first option in the list. */
typedef struct _FSUMEntry
{
   const char* fsInstanceName;
   size_t      fsInstanceNameLen;
   const char* fsDatabaseName;
   size_t      fsDatabaseNameLen;
   const char* fsServerName;
   size_t      fsServerNameLen;
   const char* fsAuthID;
   size_t      fsAuthIDLen;
   FSUMOption* firstOption;
} FSUMEntry;

/* The APIs to implement in addition to the FSUMPluginInit API, 
   which is not in the FSUMPluginAPIs structure. */
typedef struct _FSUMPluginAPIs
{
   size_t version;
   SQL_API_RC (SQL_API_FN * FSUMconnect ) (void** a_FSUMRepository,
                                           const char*  a_cfgFilePath);

   SQL_API_RC (SQL_API_FN * FSUMfetchUM) (void* a_FSUMRepository,
                                          FSUMEntry* a_entry);

   SQL_API_RC (SQL_API_FN * FSUMdisconnect) (void* a_FSUMRepository);

   SQL_API_RC (SQL_API_FN * FSUMPluginTerm) ();
} FSUMPluginAPIs;

/* The federated server provides these utilities as callback functions to the plug-in. */
typedef SQL_API_RC (SQL_API_FN FSUMallocateFP)
                    (size_t a_blkSize,
                     void** a_pblkPtr);

typedef void       (SQL_API_FN FSUMdeallocateFP)
                    (void* a_blkPtr);

typedef SQL_API_RC (SQL_API_FN FSUMloadFP)
                    (const char* a_libName,
                     void** a_lib);

typedef SQL_API_RC (SQL_API_FN FSUMgetFunctionFP)
                    (const char* a_functionName,
                     void* a_lib,
                     void** a_pFuncAddress);

typedef SQL_API_RC (SQL_API_FN FSUMunloadFP)
                    (void* a_lib);

typedef SQL_API_RC (SQL_API_FN FSUMlogErrorMsgFP)
                    (sqlint32 a_level,
                     const char* a_msg,
                     size_t a_length);

typedef SQL_API_RC (SQL_API_FN FSUMaddUMOptionFP)
                    (FSUMEntry *a_entry,
                     const char* optionName,
                     size_t optionNameLen,
                     const char* optionValue, 
                     size_t optionValueLen);

/* Structure to hold the utility functions that the federated server provides. */
typedef struct _FSUMPluginUtilities
{
   FSUMallocateFP      *allocate;
   FSUMdeallocateFP    *deallocate;
   FSUMloadFP          *load;
   FSUMgetFunctionFP   *getFunction;
   FSUMunloadFP        *unload;
   FSUMlogErrorMsgFP   *logErrorMsg;
   FSUMaddUMOptionFP   *addUMOption;
} FSUMPluginUtilities;

/* User mapping plug-in entry point type. */
typedef SQL_API_RC (SQL_API_FN *FSUMPluginInitType)(sqlint32, FSUMPluginAPIs*, FSUMPluginUtilities*);

/* User mapping plug-in C interface entry point. */
typedef SQL_API_RC (*fsum_plugin_hook_type)(const char*, FSUMPluginInitType*, FSUMPluginUtilities*);

/* Definition of return codes for utility functions */
#define FSUM_PLUGIN_UTIL_OK           0
#define FSUM_PLUGIN_UTIL_FAILED       -1

/* Definition for extern C. */
#define FSUM_PLUGIN_EXT_C extern "C"

/* Error severities that the logErrorMsg function uses.*/
#define FSUM_LOG_NONE      0 /* No Logging */
#define FSUM_LOG_CRITICAL  1 /* Severe error encountered */
#define FSUM_LOG_ERROR     2 /* Error encountered */
#define FSUM_LOG_WARNING   3 /* Warning */
#define FSUM_LOG_INFO      4 /* Informational */


/* Error codes that the APIs return.*/
#define FSUM_PLUGIN_OK                0
#define FSUM_INITIALIZE_ERROR         1
#define FSUM_PLUGIN_VERSION_ERROR     2
#define FSUM_CONNECTION_ERROR         3
#define FSUM_LOOKUP_ERROR             4
#define FSUM_DECRYPTION_ERROR         5
#define FSUM_DISCONNECT_ERROR         6
#define FSUM_INVALID_PARAMETER_ERROR  7
#define FSUM_UNAUTHORIZED_CALLER      8
#define FSUM_AUTHENTICATION_ERROR     9
#define FSUM_TERMINATION_ERROR        10

/* Maximum length of a name.*/
#define FSUM_MAX_NAME_LEN             128

/* Maximum length of a file path. */
#define FSUM_MAX_PATH_LEN             256

/* Maximum length of an option value. */
#define FSUM_MAX_OPTION_VALUE_LEN     (2048+1)

/* Maximum length of an error message. */
#define FSUM_MAX_ERROR_MSG_SIZE       2048

#ifdef __cplusplus
}
#endif

#endif /* FS_H_UM_PLUGIN */
