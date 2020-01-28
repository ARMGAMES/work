/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Jul 26 08:32:34 2001
 */
/* Compiler settings for ibmdadb2.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __ibmdadb2_h__
#define __ibmdadb2_h__

#ifdef __cplusplus
extern "C"{
#endif

/* Forward Declarations */

#ifndef __IBMDADB2_FWD_DEFINED__
#define __IBMDADB2_FWD_DEFINED__

#ifdef __cplusplus
typedef class IBMDADB2 IBMDADB2;
#else
typedef struct IBMDADB2 IBMDADB2;
#endif /* __cplusplus */

#endif  /* __IBMDADB2_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * );


/* interface __MIDL_itf_ibmdadb2_0000 */
/* [local] */

/*  The following GUIDs are no longer defined.  Please use CLSIDFromProgID to 
    obtain the CLSID for IBMDADB2.
    
extern const GUID LIBID_IBMDADB2Lib;
extern const GUID CLSID_IBMDADB2;
extern const GUID CLSID_IBMDADB2ERROR;
extern const GUID CLSID_IBMDADB2ENUM;
extern const GUID CLSID_IBMDADB2ENUMERATOR;
extern const GUID CLSID_IBMDADB2CONNPAGE;
extern const GUID CLSID_IBMDADB2PROPPAGE;
*/

/* IBMDADB2 specific GUID */
#ifdef DBINITCONSTANTS

extern const GUID DBPROPSET_DB2ROWSET     = {0xd65e3e60,0xa37b,0x11d3,{0xaf,0x7c,0x00,0x06,0x29,0xb3,0xcd,0x56}};
extern const GUID DBPROPSET_DB2DATASOURCE = {0x8a80412a,0x7d94,0x4fec,{0x87,0x3e,0x6c,0xd1,0xcd,0x42,0x0d,0xcd}};

#else

extern const GUID DBPROPSET_DB2ROWSET;
extern const GUID DBPROPSET_DB2DATASOURCE;

#endif

/* Property IDs for DBPROPSET_DB2ROWSET */
#define DB2PROP_ISLONGMINLENGTH         1
#define DB2PROP_DEFERRED_PREPARE        5

#define DBPROP_ISLONGMINLENGTH          DB2PROP_ISLONGMINLENGTH

/* Property IDs for DBPROPSET_DB2DATASOURCE */

#define DB2PROP_RETURNCHARASWCHAR       2
#define DB2PROP_SORTBYORDINAL           3
#define DB2PROP_REPORTISLONGFORLONGTYPES 4


extern RPC_IF_HANDLE __MIDL_itf_ibmdadb2_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ibmdadb2_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
