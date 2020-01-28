/**********************************************************************
**
**  Source File Name = db2commexit.h
**
**  (C) COPYRIGHT International Business Machines Corp. 2011, 2011
**  All Rights Reserved
**  Licensed Materials - Property of IBM
**
**  US Government Users Restricted Rights - Use, duplication or
**  disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
**  Function = This files includes:
**             1. define for all the API return codes
**             2. maximum size used in the API parameter
**             3. defines used for some of the API parameter
**             4. structures and function used in the API parameter
**             5. initialization API for comm exit library
**             6. structures that returns the API function pointers
**
** Operating System: All
**
***********************************************************************/


#ifndef _DB2COMMEXIT_H
#define _DB2COMMEXIT_H

#if defined(SQLWINT)
#include <sys\types.h>
#include <winsock2.h>
#include <WS2TCPIP.H>
#else
#include <netinet/in.h>
#endif


#include "db2ApiDf.h"
#include "sqlenv.h"
#include "sqlmon.h"



#ifdef __cplusplus
extern "C" {
#endif

#define DB2COMMEXIT_BUFFER_API_VERSION_1   1
#define DB2COMMEXIT_BUFFER_API_VERSION_LATEST DB2COMMEXIT_BUFFER_API_VERSION_1

#define DB2COMMEXIT_RUNTIME_API_VERSION_1  1
#define DB2COMMEXIT_RUNTIME_API_VERSION_LATEST DB2COMMEXIT_RUNTIME_API_VERSION_1

// For backward compatibility only
#define DB2COMMEXIT_API_VERSION_1 DB2COMMEXIT_BUFFER_API_VERSION_1

#define DB2COMMEXIT_INSTANCE_SZ 128
#define DB2COMMEXIT_DBNAME_SZ 128


#define DB2COMMEXIT_SUCCESS                    0
#define DB2COMMEXIT_ERR_UNKNOWN               -1
#define DB2COMMEXIT_ERR_DROP_CONNECTION       -2
#define DB2COMMEXIT_ERR_NOMEM                 -3


#define DB2COMMEXIT_LOG_NONE      0
#define DB2COMMEXIT_LOG_CRITICAL  1
#define DB2COMMEXIT_LOG_ERROR     2
#define DB2COMMEXIT_LOG_WARNING   3
#define DB2COMMEXIT_LOG_INFO      4


#define DB2COMMEXIT_REGISTER_NEW_CONNECTION     1
#define DB2COMMEXIT_REGISTER_AGENT_ASSOCIATION  2


#define DB2COMMEXIT_DEREGISTER_CONNECTION_TERM       1
#define DB2COMMEXIT_DEREGISTER_AGENT_DISASSOCIATION  2


#define DB2COMMEXIT_CONNECTION_UNKNOWN  1
#define DB2COMMEXIT_CONNECTION_LOCAL    2
#define DB2COMMEXIT_CONNECTION_GATEWAY  3


#define DB2COMMEXIT_USERIDENT_NEW_CONNECTION    1
#define DB2COMMEXIT_USERIDENT_TC_SWITCH_USER    2
#define DB2COMMEXIT_USERIDENT_SET_SESSION_USER  3

#define DB2COMMEXIT_SESSION_NEW_CONNECTION    1
#define DB2COMMEXIT_SESSION_TC_SWITCH_USER    2
#define DB2COMMEXIT_SESSION_NEW_INDEPENDENT   3
#define DB2COMMEXIT_SESSION_NEW_ATTACH        4
#define DB2COMMEXIT_SESSION_CONNECTION_TERM   5
#define DB2COMMEXIT_SESSION_INDEPENDENT_TERM  6
#define DB2COMMEXIT_SESSION_DETACH            7

#define DB2COMMEXIT_SESSION_AUTHENTICATION_FAILURE 0
#define DB2COMMEXIT_SESSION_AUTHORIZATION_FAILURE  1

#define DB2COMMEXIT_PRDID_SZ                  8
#define DB2COMMEXIT_APPL_NAME_SZ              20 

// Input flags for db2CommexitSend function
#define DB2COMMEXIT_SEND_IN_FLAG_PURGE          0x0000000000000001


// Input flags for db2CommexitReceive function
#define DB2COMMEXIT_RECV_IN_FLAG_END_DECRYPT    0x0000000000000001

#define DB2COMMEXIT_API_VERSION_LATEST DB2COMMEXIT_API_VERSION_1


#define DB2COMMEXIT_GET_BUFFER_FN_VER(version) (version & 0x0000FFFF)
#define DB2COMMEXIT_GET_RUNTIME_FN_VER(version) (version >> 16)

#define DB2COMMEXIT_SET_BUFFER_FN_VER(version, bufferFnVer) (*version = bufferFnVer) 
    
#define DB2COMMEXIT_SET_RUNTIME_FN_VER(version, runtimeFnVer) (*version = (((db2int32)runtimeFnVer) << 16))

typedef SQL_API_RC ( SQL_API_FN db2commexitLogMessage )
(
   db2int32   level,
   char     * logmsg,
   db2int32   logmsglen
);



SQL_API_RC SQL_API_FN db2commexitInit
(
   db2int32                 version,
   void                   * commexit_fns,
   db2commexitLogMessage  * logMessage_fn,
   char                  ** errormsg,
   db2int32               * errormsglen
);


typedef struct db2commexitBuffer
{
   const unsigned char * pBuffer;
   db2int64              buffer_len;
   db2int32              reserved1;
   db2int32              reserved2;
} db2commexitBuffer;

typedef struct db2NonSqlApiInfo
{
    db2Uint32                 version;      // NonSqlApi Version
    const db2commexitBuffer  *name;         // Name of the NonSqlApi info belongs to
    const db2commexitBuffer  *parameters;   // Length of the structure +
                                            // In/Out Parameter Structure
} db2NonSqlApiInfo;

typedef struct db2commexitIPV4Info
{
   struct sockaddr_in client_sockaddr;
   struct sockaddr_in server_sockaddr;
} db2commexitIPV4Info;

typedef struct db2commexitIPV6Info
{
   struct sockaddr_in6 client_sockaddr;
   struct sockaddr_in6 server_sockaddr;
} db2commexitIPV6Info;

typedef struct db2commexitIPCInfo
{
   void * pSharedMemSegmentHandle;
} db2commexitIPCInfo;

typedef struct db2commexitNamedPipeInfo
{
   void * handle;
} db2commexitNamedPipeInfo;

typedef struct db2commexitCommInfo_v1
{
   db2int32 clientProtocol; // SQL_PROTOCOL_ ...
   db2int32 connectionType; // unknown, local or gateway
   db2int32 hostnameLen;
   db2int32 instanceLen;
   db2int32 dbnameLen;
   db2int32 dbaliasLen;
   db2int32 inbound_appl_id_len;
   db2int32 outbound_appl_id_len;
   db2int32 clientPID;      // Client PID
   db2int32 reserved2;
   db2NodeType member;
   char hostname[SQL_HOSTNAME_SZ+1];
   char instance[DB2COMMEXIT_INSTANCE_SZ + 1];
   char dbname[DB2COMMEXIT_DBNAME_SZ + 1];
   char dbalias[DB2COMMEXIT_DBNAME_SZ + 1];
   char inbound_appl_id[SQLM_APPLID_SZ + 1];
   char outbound_appl_id[SQLM_APPLID_SZ + 1];
   char reservedChar1[128];
   union
   {
      db2commexitIPV4Info      ipv4Info;
      db2commexitIPV6Info      ipv6Info;
      db2commexitIPCInfo       ipcInfo;
      db2commexitNamedPipeInfo namedPipeInfo;
   } comminfo;

} db2commexitCommInfo_v1;


typedef struct db2commexitFunctions_v1
{
   db2int32 version;

   SQL_API_RC ( SQL_API_FN * db2commexitTerm )
   (
      char **errormsg,
      db2int32 *errormsglen
   );

   SQL_API_RC ( SQL_API_FN * db2commexitRegister )
   (
      void                         ** ppConnectionContext,
      const db2commexitCommInfo_v1  * pCommInfo,
      db2int32                        state,
      db2int64                      * pReservedFlags,
      char                         ** errormsg,
      db2int32                      * errormsglen
   );

   SQL_API_RC ( SQL_API_FN * db2commexitDeregister )
   (
      void                          * pConnectionContext,
      const db2commexitCommInfo_v1  * pCommInfo,
      db2int32                        state,
      db2int64                      * pReservedFlags,
      char                         ** errormsg,
      db2int32                      * errormsglen
   );

   SQL_API_RC ( SQL_API_FN * db2commexitRecv )
   (
      void                          * pConnectionContext,
      const db2commexitCommInfo_v1  * pCommInfo,
      const db2commexitBuffer       * pBuffer,
      db2int64                      * pFlags,
      char                         ** errormsg,
      db2int32                      * errormsglen
   );

   SQL_API_RC ( SQL_API_FN * db2commexitSend )
   (
      void                          * pConnectionContext,
      const db2commexitCommInfo_v1  * pCommInfo,
      const db2commexitBuffer       * pBuffer,
      db2int64                      * pFlags,
      char                         ** errormsg,
      db2int32                      * errormsglen
   );

   SQL_API_RC ( SQL_API_FN * db2commexitUserIdentity )
   (
      void                          * pConnectionContext,
      const db2commexitCommInfo_v1  * pCommInfo,
      db2int32                        state,
      db2int32                        usernameLen,
      const char                    * pUsername,
      db2int32                        sessionAuthidLen,
      const char                    * pSessionAuthid,
      db2int64                      * pReservedFlags,
      char                         ** errormsg,
      db2int32                      * errormsglen
   );

   SQL_API_RC ( SQL_API_FN * db2commexitFreeErrormsg )
   (
      char * errormsg
   );

} db2commexitFunctions_v1;

typedef struct db2commexitRuntimeCommInfo_v1
{
  db2int32 clientProtocol;         // SQL_PROTOCOL_ ...
  db2int32 connectionType;         // unknown, local or gateway
  db2int32 hostnameLen;            // Server host name length
  db2int32 clientHostNameLen;      // Client host name length
  db2int32 instanceLen;            // Instance name length
  db2int32 dbnameLen;              // Database name length
  db2int32 dbaliasLen;             // Database alias length
  db2int32 appl_id_len;            // Application ID length
  db2int32 parent_appl_id_len;     // Parent app ID length for autonomous routine
  db2int32 server_product_id_len;  // Server Product ID length 
  db2int32 client_product_id_len;  // Client product ID length
  db2int32 clientPlatform;         // Client platform
  db2int32 serverPlatform;         // Server platform
  db2int32 appl_name_len;          // Application name length
  db2int32 clientPID;              // Client PID
  db2int32 serverPID;              // Server PID
  db2int32 clientOSUserNameLen;    // Client OS User Name length
  db2int16 security_mechanism;     // DRDA security mechanism

  db2int32 reserved1;
  db2int32 reserved2;

  db2NodeType member;              // Member ID

  char hostname[SQL_HOSTNAME_SZ+1];
  char clientHostName[SQL_HOSTNAME_SZ+1];
  char instance[DB2COMMEXIT_INSTANCE_SZ + 1];
  char dbname[DB2COMMEXIT_DBNAME_SZ + 1];
  char dbalias[DB2COMMEXIT_DBNAME_SZ + 1];
  char appl_id[SQLM_APPLID_SZ + 1];
  char parent_appl_id[SQLM_APPLID_SZ + 1];
  char server_product_id[DB2COMMEXIT_PRDID_SZ+1];
  char client_product_id[DB2COMMEXIT_PRDID_SZ+1];
  char appl_name[DB2COMMEXIT_APPL_NAME_SZ+1];
  char clientOSUserName[SQL_MAX_IDENT + 1];

  char trusted_connection;         // Trusted connection? 
   
  char reservedChar1[128];

  // Communication protocol info
  union
  {
    db2commexitIPV4Info ipv4Info;
    db2commexitIPV6Info ipv6Info;
    db2commexitIPCInfo  ipcInfo;
    db2commexitNamedPipeInfo namedPipeInfo;
  };
} db2commexitRuntimeCommInfo_v1;

typedef struct db2commexitPkgID_v1
{
   sqluint16 rdbcolidlen;    // Package creator ID length
   sqluint16 pkgnamelen;     // Package name length
   sqluint16 pkgsn;          // Section number
   sqluint16 reserved;       // Reserved field
   char      rdbcolid[128];  // Package creator ID
   char      pkgname[128];   // Package name
   char      pkgcnstkn[8];   // Package consistency token
} db2commexitPkgID_v1;

typedef struct db2commexitQryInsID_v1
{
  sqluint16 qryInsIDLen;       // Length of the query instance ID
  unsigned char qryInsID[8];   // Query Instance ID
} db2commexitQryInsID_v1;

typedef struct db2commexitNextChunkInfo_v1
{
  char       prgRef[8];             // progressive reference
  sqlint64   iRequestLength;        // Requested chunk length in bytes
  sqluint32  bfFlags;               // flags
  sqluint8   freeRefOption;         // Free reference option
} db2commexitNextChunkInfo_v1;

typedef struct db2commexitXID_v1 
{
  sqlint32 formatID;       // format identifier
  sqlint32 gtrid_length;   // value from 1 through 64
  sqlint32 bqual_length;   // value from 1 through 64
  char data[128];
} db2commexitXID_v1;

typedef struct db2commexitReqInfo_open_v1
{
  struct db2commexitPkgID_v1      pkgID;            // Package Identifier
  db2int64                        cursorAttributes; // Cursor attributes
  db2int64                        rowSetSize;       // Row Set Size
  db2int32                        queryBlockSize;   // Query Block Size
  db2int16                        cursorNameLen;    // Cursor Name length
  char                            cursorName[128];  // Cursor Name
}db2commexitReqInfo_open_v1;

typedef struct db2commexitReqInfo_fetch_v1
{
  struct db2commexitPkgID_v1      pkgID;          // Package Identifier
  struct db2commexitQryInsID_v1   qryInsID;       // Optional Query Instance ID
  db2int64                        cmdSrcID;       // Command source ID
  db2int64                        rowSetSize;     // Row Set Size
  db2int32                        queryBlockSize; // Query Block Size
  db2int32                        orientation;    // Fetch Orientation. 
}db2commexitReqInfo_fetch_v1;

typedef struct db2commexitReqInfo_close_v1
{
  struct db2commexitPkgID_v1      pkgID;     // Package Identifier
  struct db2commexitQryInsID_v1   qryInsID;  // Optional Query Instance ID
  db2int64                        cmdSrcID;  // Command source ID
}db2commexitReqInfo_close_v1;

typedef struct db2commexitReqInfo_describe_v1
{
  struct db2commexitPkgID_v1      pkgID;     // Package Identifier
  struct db2commexitQryInsID_v1   qryInsID;  // Optional Query Instance ID
  db2int64                        cmdSrcID;  // Command Source ID
}db2commexitReqInfo_describe_v1;

typedef struct db2commexitReqInfo_execimmd_v1
{
  struct db2commexitPkgID_v1      pkgID;     // Package Identifier
  struct db2commexitQryInsID_v1   qryInsID;  // Optional Query Instance ID
  db2int64                        cmdSrcID;  // Command Source ID
  struct db2commexitBuffer        SQLAttr;   // SQL Attributes. 
}db2commexitReqInfo_execimmd_v1;

typedef struct db2commexitReqInfo_prepare_v1
{
  struct db2commexitPkgID_v1 pkgID;     // Package Identifier
  struct db2commexitBuffer   SQLAttr;   // SQL Attributes. 
}db2commexitReqInfo_prepare_v1;

typedef struct db2commexitReqInfo_execute_v1
{
  struct db2commexitPkgID_v1      pkgID;     // Package Identifier
  struct db2commexitQryInsID_v1   qryInsID;  // Optional Query Instance ID
  db2int64                        cmdSrcID;  // Command Source ID
  db2int64                        rowSetSize; // Row Set Size
}db2commexitReqInfo_execute_v1;

typedef struct db2commexitReqInfo_compile_v1
{
  struct db2commexitPkgID_v1      pkgID;     // Package Identifier
}db2commexitReqInfo_compile_v1;

typedef struct db2commexitReqInfo_bind_v1
{
  struct db2commexitPkgID_v1 pkgID;       // Package Identifier
  struct sqlopt              bindOptions; // Bind Options
}db2commexitReqInfo_bgnbnd_v1;

typedef struct db2commexitReqInfo_endbnd_v1
{
  struct db2commexitPkgID_v1      pkgID;     // Package Identifier
}db2commexitReqInfo_endbnd_v1;

typedef struct db2commexitReqInfo_XA_v1
{
  struct db2commexitXID_v1        xid;       // XID
}db2commexitReqInfo_XA_v1;

typedef struct db2commexitReqInfo_getNextChunk_v1
{
  struct db2commexitPkgID_v1         pkgID;          // Package ID
  struct db2commexitQryInsID_v1      qryInsID;       // Query Instance ID
  db2int64                           cmdSrcID;       // Command Source ID
  struct db2commexitNextChunkInfo_v1 nextChunkInfo;  // Next chunk info. 
}db2commexitReqInfo_getNextChunk_v1;


// SQL Request types 
typedef enum db2commexitReqType
{
  DB2COMMEXIT_REQTYPE_INVALID = 0,
  DB2COMMEXIT_REQTYPE_OPEN,
  DB2COMMEXIT_REQTYPE_FETCH,
  DB2COMMEXIT_REQTYPE_CLOSE,
  DB2COMMEXIT_REQTYPE_PREPARE,
  DB2COMMEXIT_REQTYPE_DESCRIBE,
  DB2COMMEXIT_REQTYPE_EXECUTE,
  DB2COMMEXIT_REQTYPE_EXECIMMD,
  DB2COMMEXIT_REQTYPE_BGNBND,
  DB2COMMEXIT_REQTYPE_COMPILE,
  DB2COMMEXIT_REQTYPE_ENDBND,
  DB2COMMEXIT_REQTYPE_REBIND,
  DB2COMMEXIT_REQTYPE_COMMIT,
  DB2COMMEXIT_REQTYPE_ROLLBACK,
  DB2COMMEXIT_REQTYPE_SET,
  DB2COMMEXIT_REQTYPE_GET_NEXT_CHUNK,
  DB2COMMEXIT_REQTYPE_XA_RECOVER,
  DB2COMMEXIT_REQTYPE_XA_FORGET,
  DB2COMMEXIT_REQTYPE_XA_REGISTER,
  DB2COMMEXIT_REQTYPE_XA_END,
  DB2COMMEXIT_REQTYPE_XA_PREPARE,
  DB2COMMEXIT_REQTYPE_XA_COMMIT,
  DB2COMMEXIT_REQTYPE_XA_ROLLBACK
}db2commexitReqType;


typedef struct db2commexitRuntimeFunctions_v1
{
  db2int32 version;
  db2int64 flags;
 
  SQL_API_RC ( SQL_API_FN * db2commexitTerm )
    (
     char     **errormsg,
     db2int32 *errormsglen
     );

  

  SQL_API_RC ( SQL_API_FN * db2commexitSessionInit )
    (
     void                                ** ppConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     db2int32                               state,
     db2int32                               usernameLen,
     const char                           * pUserame,
     db2int32                               sessionAuthidLen,
     const char                           * pSessionAuthid,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSessionInitFailure )
    (
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     db2int32                               state,
     struct sqlca                         * pSqlca,
     db2int32                               usernameLen,
     const char                           * pUserame,
     db2int32                               sessionAuthidLen,
     const char                           * pSessionAuthid,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSetSessionAuth )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     db2int32                               usernameLen,
     const char                           * pUserame,
     db2int32                               sessionAuthidLen,
     const char                           * pSessionAuthid,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSessionTerm )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     db2int32                               state,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLStart )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     db2commexitReqType                     requestType,
     db2int16                               nestingLevel,
     const db2commexitBuffer              * pReqInfo,
     const db2commexitBuffer              * pInputVarDefs,
     const db2commexitBuffer              * pStmtText,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLTerm )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     const struct sqlca                   * pSqlca,
     const db2commexitQryInsID_v1         * pQryInsID, 
     const db2commexitBuffer              * pOutputVarDefs,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLCompilationEnv )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     const db2commexitBuffer              * pCompilationEnv,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );


  SQL_API_RC ( SQL_API_FN * db2commexitSQLSpecialRegisters )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     const db2commexitBuffer              * pSpecialRegisters,
     db2int64                             * pReservedFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLGlobalVariables )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     const db2commexitBuffer              * pGVarDefs,
     const db2commexitBuffer              * pData,
     db2int64                             * pFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLInputData )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     const db2commexitBuffer              * pClientDataDefs,
     const db2commexitBuffer              * pDataDefs,
     const db2commexitBuffer              * pData,
     db2int64                             * pFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLOutputData )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     const struct sqlca                   * pSqlca,
     const db2commexitBuffer              * pDataDefs,
     const db2commexitBuffer              * pData,
     db2int64                             * pFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLLobData )
    (
     void                                 * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1  * pCommInfo,
     db2int32                               state,
     const db2commexitBuffer              * pData,
     db2int64                             * pFlags,
     char                                ** errormsg,
     db2int32                             * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitSQLResultSet )
    (
     void                                  * pConnectionContext,
     const db2commexitRuntimeCommInfo_v1   * pCommInfo,
     const db2commexitPkgID_v1             * pPkgID,
     const db2commexitQryInsID_v1          * pQryInsID,
     char                                    prefetchData,
     db2int64                              * pReservedFlags,
     char                                 ** errormsg,
     db2int32                              * errormsglen
     );

  SQL_API_RC ( SQL_API_FN * db2commexitNonSqlApiStart )
  (
   void                                  * pConnectionContext,
   const db2commexitRuntimeCommInfo_v1   * pCommInfo,
   const db2NonSqlApiInfo                * pNonSqlApiInfo,
   db2int64                              * pReservedFlags,
   char                                 ** errormsg,
   db2int32                              * errormsglen
  );

  SQL_API_RC ( SQL_API_FN * db2commexitNonSqlApiTerm)
  (
   void                                  * pConnectionContext,
   const db2commexitRuntimeCommInfo_v1   * pCommInfo,
   const db2NonSqlApiInfo                * pNonSqlApiInfo,
   const struct sqlca                    * pSqlca,
   db2int64                              * pReservedFlags,
   char                                 ** errormsg,
   db2int32                              * errormsglen
  );

  SQL_API_RC ( SQL_API_FN * db2commexitFreeErrormsg )
    (
     char * errormsg
     );

} db2commexitRuntimeFunctions_v1;


struct db2commexitDataDescriptor_v1;

typedef struct db2commexitDataDescriptorVars_v1
{
   sqlint32  extent;     // Array Demension:
                         //    0   not an array,
                         //    -1  array of unknown size
                         //    1-N array size
   sqluint32 length;     // Variable data length
   sqluint32 indexLength; // Index length for associative arrays
   sqluint16 namelen;    // Name length
   sqluint16 schemaLen;  // Schema length
   sqluint16 moduleLen;  // Module length
   sqluint16 type;       // Variable data type
   sqluint16 indexType;  // Index type for associative arrays
   sqluint16 codepage;   // Codepage
   sqluintptr name;      // name of the variable.
   struct db2commexitDataDescriptor_v1* nestedDD; // Nested DD for complex data type
}db2commexitDataDescriptorVars_v1;

typedef struct db2commexitDataDescriptor_v1
{
  sqluint32              numVars;  // Num of variables
  sqluint32              size;     // size of the descriptor
  sqluint64              totalLen; // Total Length of the variables
  sqluint32              flags;    // Internal Flags
  sqluint32              reserved; 
  struct db2commexitDataDescriptorVars_v1 var[1];      // first variable
}db2commexitDataDescriptor_v1;

typedef struct db2commexit_sqlvar_v1
{
  sqluint16        nullind;  // Null indicator
  sqluint16        reserved; 
  sqluint32        length;  // Length
  sqluintptr       data;    // data pointer
} db2commexit_sqlvar_v1;

typedef struct db2commexit_asscarrayelem_v1
{
  struct db2commexit_sqlvar_v1 index;  // The index of the element
  struct db2commexit_sqlvar_v1 value;  // The element value
}db2commexit_asscarrayelem_v1;


typedef struct db2commexit_asscarraydesc_v1
{
   sqluint64                    cardinality; // Cardinality of the array
   struct db2commexit_asscarrayelem_v1 element[1]; // The first element
}db2commexit_asscarraydesc_v1;


typedef struct db2commexit_arraydesc_v1
{
   sqluint64                    cardinality; // cardinality of the array
   struct db2commexit_sqlvar_v1 element[1];  // The first element
}db2commexit_arraydesc_v1;

typedef struct db2commexit_rowdesc_v1
{
   sqluint32                    numFields; // Number of row fields
   sqluint32                    reserved; 
   struct db2commexit_sqlvar_v1 fields[1]; // The first row field
}db2commexit_rowdesc_v1;

typedef struct db2commexit_sqldata_v1
{
  sqluint32                     numVars; // Number of variables. 
  sqluint32                     reserved; 
  struct db2commexit_sqlvar_v1  sqlvars[1]; // The first sql variable. 
}db2commexit_sqldata_v1;

typedef struct db2commexitSpecRegVal_v1
{
  sqluint32 specRegLen;       // Length of the special register
  sqluint16 nameLen;          // Length of the name
  sqluint16 reserved;         // Reserved field. 
  char      specRegName[128]; // Name of the special Register
  sqluintptr specRegVal;      // Ptr/offset to the data
}db2commexitSpecRegVal_v1;

typedef struct db2commexitSpecRegs_v1
{
  sqluint32 numSpecRegs;          // Number of special registers
  sqluint32 reserved; 
  struct db2commexitSpecRegVal_v1 registers[1]; // The first register
}db2commexitSpecRegs_v1;


// State for db2commexitSQLLobData
#define DB2COMMEXIT_LOB_DATA_INPUT     0
#define DB2COMMEXIT_LOB_DATA_OUTPUT    1
#define DB2COMMEXIT_LOB_DATA_GLOBALVAR 2


// Commit or rollback occured flag for dbcommexitSQLTerm 
#define DB2COMMEXIT_SQLTERM_IN_FLAG_COMMIT_OCCURED   0x0000000000000001
#define DB2COMMEXIT_SQLTERM_IN_FLAG_ROLLBACK_OCCURED 0x0000000000000002



#ifdef __cplusplus
}
#endif

#endif

