# include <stddef.h>
# include <limits.h>
#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef ORASYS_TYPES
# include <sys/types.h>
# define ORASYS_TYPES
#endif

# define SX_ORACLE
# define SX3_ORACLE

#ifndef TRUE
# define TRUE  1
# define FALSE 0
#endif

#define UB1BITS          CHAR_BIT
#define UB1MASK          ((1 << ((uword)CHAR_BIT)) - 1)


#ifndef lint
typedef   signed short    sb2;
typedef unsigned short    ub2;
typedef unsigned int  ub4;
typedef   signed int  sb4;
typedef unsigned char  ub1;
typedef   signed char  sb1;
typedef          char     eb1;
typedef          short    eb2;
typedef          int      eb4;
typedef         sb1  b1;
typedef         sb2      b2;
typedef         sb4    b4;
typedef          int eword;
typedef unsigned int uword;
typedef   signed int sword;
typedef unsigned short  utext;

#else
#define ub2  unsigned short
#define sb2  signed short
#define eb4 int
#define ub4 unsigned int
#define sb4 signed int
#define ub1 unsigned char
#define sb1 signed char
#define eb1      char
#define eb2      short
#define eb4      int
#define b1 sb1
#define b2 sb2
#define b4 sb4
#define eword int
#define uword unsigned int
#define sword signed int
#define utext  unsigned short
#endif


#ifdef lint
#define oratext unsigned char
#define OraText unsigned char
#else
  typedef  unsigned char oratext;
  typedef oratext OraText;
#endif

#if !defined(LUSEMFC)
# ifdef lint
#  define text unsigned char
#else
   typedef oratext text;
# endif
#endif


#define ORAXB8_DEFINED
#if !defined(DB2HP) && !defined(DB2SUN)
#ifndef lint
#ifndef __GNUC__
 typedef unsigned _int64 oraub8;
 typedef signed _int64 orasb8;
#else
 typedef unsigned long long oraub8;
 typedef signed long long orasb8;
#endif
 typedef oraub8 ub8;
 typedef orasb8 sb8;
#else
# define ub8 oraub8
# define sb8 orasb8
# define oraub8 unsigned _int64
# define orasb8 signed _int64
#endif
#else
 typedef unsigned long long oraub8;
 typedef signed long long orasb8;
 typedef oraub8 ub8;
 typedef orasb8 sb8;
#endif


#if !defined boolean && !defined BOOLEANDEFINED
# define boolean int
# define BOOLEANDEFINED
#endif
#ifdef sparc
# define SIZE_TMAXVAL SB4MAXVAL
#else
# define SIZE_TMAXVAL UB4MAXVAL
#endif


#if !defined(MOTIF) && !defined(LISPL)  && !defined(__cplusplus) && !defined(LUSEMFC)
typedef  oratext *pString;
#endif

///////////////////////////////////////
#ifdef _WIN64

#ifndef lint
typedef unsigned _int64 ubig_ora;
typedef   signed _int64 sbig_ora;
#else
#define ubig_ora unsigned _int64
#define sbig_ora signed _int64
#endif

#define UBIG_ORAMAXVAL ((ubig_ora)_UI64_MAX)
#define UBIG_ORAMINVAL ((ubig_ora)        0)
#define SBIG_ORAMAXVAL ((sbig_ora) _I64_MAX)
#define SBIG_ORAMINVAL ((sbig_ora) _I64_MIN)
#define MINUBIG_ORAMAXVAL ((ubig_ora) 4294967295)
#define MAXUBIG_ORAMINVAL ((ubig_ora)          0)
#define MINSBIG_ORAMAXVAL ((sbig_ora) 2147483647)
#define MAXSBIG_ORAMINVAL ((sbig_ora)-2147483647)

#else

#ifndef lint
typedef unsigned long  ubig_ora;
typedef   signed long  sbig_ora;
#else
#define ubig_ora unsigned long
#define sbig_ora signed long
#endif

#define UBIG_ORAMAXVAL ((ubig_ora)ULONG_MAX)
#define UBIG_ORAMINVAL ((ubig_ora)        0)
#define SBIG_ORAMAXVAL ((sbig_ora) LONG_MAX)
#define SBIG_ORAMINVAL ((sbig_ora) LONG_MIN)
#define MINUBIG_ORAMAXVAL ((ubig_ora) 4294967295)
#define MAXUBIG_ORAMINVAL ((ubig_ora)          0)
#define MINSBIG_ORAMAXVAL ((sbig_ora) 2147483647)
#define MAXSBIG_ORAMINVAL ((sbig_ora)-2147483647)

#endif

#ifndef XMLCTX_DEFINED
# define XMLCTX_DEFINED
struct xmlctx; typedef struct xmlctx xmlctx;
#endif


#if defined(SS_64BIT_SERVER) || defined(__64BIT__)
# define HDA_SIZE 512
#define CDA_SIZE 88
#else
# define HDA_SIZE 256
# define CDA_SIZE 64
#endif


#ifndef OCI_FLAGS
#define OCI_FLAGS
#define OCI_ONE_PIECE   0
#define OCI_FIRST_PIECE 1
#define OCI_NEXT_PIECE  2
#define OCI_LAST_PIECE  3
#endif

#ifndef OCI_ORACLE
#define OCI_ORACLE

#define SQL_SINGLE_RCTX    NULL


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct OCIAdmin         OCIAdmin;
typedef struct OCIAQAgent         OCIAQAgent;
typedef struct OCIAQDeqOptions    OCIAQDeqOptions;
typedef struct OCIAQEnqOptions    OCIAQEnqOptions;
typedef struct OCIAQLisMsgProps   OCIAQLisMsgProps;
typedef struct OCIAQListenOpts    OCIAQListenOpts;
typedef struct OCIAQMsgProperties OCIAQMsgProperties;
typedef struct OCIAQNfyDescriptor OCIAQNfyDescriptor;
typedef struct OCIAQSignature     OCIAQSignature;
typedef struct OCIAuthInfo      OCIAuthInfo;
typedef struct OCIBind          OCIBind;
typedef struct OCIComplexObject OCIComplexObject;
typedef struct OCIComplexObjectComp OCIComplexObjectComp;
typedef struct OCICPool         OCICPool;
typedef struct OCIDateTime OCIDateTime;
typedef struct OCIDefine        OCIDefine;
typedef struct OCIDescribe      OCIDescribe;
typedef struct OCIEnv           OCIEnv;
typedef struct OCIError         OCIError;
typedef struct OCIEvent         OCIEvent;
typedef struct OCIInterval OCIInterval;
typedef struct OCILobLocator    OCILobLocator;
typedef struct OCILobLocator OCIBFileLocator;
typedef struct OCILobLocator OCIBlobLocator;
typedef struct OCILobLocator OCIClobLocator;
typedef struct OCILobRegion     OCILobRegion;
typedef struct OCIParam         OCIParam;
typedef struct OCIResult        OCIResult;
typedef struct OCIRowid OCIRowid;
typedef struct OCISecurity      OCISecurity;
typedef struct OCIServer        OCIServer;
typedef struct OCIServerDNs     OCIServerDNs;
typedef struct OCISession       OCISession;
typedef struct OCISnapshot      OCISnapshot;
typedef struct OCISPool         OCISPool;
typedef struct OCIStmt          OCIStmt;
typedef struct OCISubscription  OCISubscription;
typedef struct OCISvcCtx        OCISvcCtx;
typedef struct OCITrans         OCITrans;
typedef struct OCIUcb           OCIUcb;
typedef ub4 OCILobLength;
typedef ub4 OCILobOffset;
typedef struct OCIColl OCIColl;
typedef OCIColl OCIArray;
typedef OCIColl OCITable;
typedef struct OCIIter OCIIter;
typedef struct OCIRaw OCIRaw;
typedef struct OCIString OCIString;
typedef ub1 nzttTStamp[4];
typedef ub4 nzttKPUsage;
typedef struct nzttIdentity nzttIdentity;
typedef struct nzttIdentityPrivate nzttIdentityPrivate;
typedef struct nzttPersona nzttPersona;
typedef struct nzttPersonaPrivate nzttPersonaPrivate;
typedef struct nzttWallet nzttWallet;
typedef struct nzttWalletPrivate nzttWalletPrivate;
typedef struct nzttWalletObj nzttWalletObj; /* For wallet object */
typedef struct nzssEntry nzssEntry; /* For secretstore */
typedef struct nzpkcs11_Info nzpkcs11_Info;

enum OCIObjectLifetime
{
   OCI_OBJECT_PERSISTENT = 1,
   OCI_OBJECT_TRANSIENT,
   OCI_OBJECT_VALUE
};
typedef enum OCIObjectLifetime OCIObjectLifetime;
//typedef enum OCIRefreshOpt OCIRefreshOpt;
typedef sb2 OCIInd;
typedef struct OCIAnyData OCIAnyData;
typedef struct OCIAnyDataCtx OCIAnyDataCtx;
typedef struct OCIAnyDataSet OCIAnyDataSet;
typedef struct OCIBinXmlReposCtx OCIBinXmlReposCtx;
typedef struct OCIDOMDocument OCIDOMDocument;
typedef struct OCIExtProcContext OCIExtProcContext;
typedef struct OCIFileObject OCIFileObject;
typedef struct OCIIOV OCIIOV;
typedef struct OCIMsg  OCIMsg;
typedef struct OCIPicklerFdo OCIPicklerFdo;
typedef struct OCIPicklerImage OCIPicklerImage;
typedef struct OCIPicklerTds OCIPicklerTds;
typedef struct OCIPicklerTdsCtx OCIPicklerTdsCtx;
typedef struct OCIRef OCIRef;
typedef struct OCIThreadContext OCIThreadContext;
typedef struct OCIThreadHandle  OCIThreadHandle;
typedef struct OCIThreadId      OCIThreadId;
typedef struct OCIThreadKey     OCIThreadKey;
typedef struct OCIThreadMutex   OCIThreadMutex;
typedef struct OCIType OCIType;
typedef struct OCITypeElem OCITypeElem;
typedef struct OCITypeIter OCITypeIter;
typedef struct OCITypeMethod OCITypeMethod;
typedef struct OCIXMLType OCIXMLType;
typedef ub1 OCIObjectPropId;
typedef ub2 OCIDuration;
typedef ub4            OCIWchar;
typedef ub4 OCIPicklerTdsElement;
typedef uword OCIObjectMarkStatus;
typedef void (*OCIThreadKeyDestFunc)(void  *);
typedef ub2 OCITypeCode;
typedef struct OCIDirPathColArray OCIDirPathColArray;
typedef struct OCIDirPathCtx      OCIDirPathCtx;
typedef struct OCIDirPathDesc     OCIDirPathDesc;
typedef struct OCIDirPathFuncCtx  OCIDirPathFuncCtx;
typedef struct OCIDirPathStream   OCIDirPathStream;

#define  B1MAXVAL  SB1MAXVAL
#define  B1MINVAL  SB1MINVAL
#define  B2MAXVAL  SB2MAXVAL
#define  B2MINVAL  SB2MINVAL
#define  B4MAXVAL  SB4MAXVAL
#define  B4MINVAL  SB4MINVAL
#define  EWORDMAXVAL  ((eword) INT_MAX)
#define  EWORDMINVAL  ((eword)       0)
#define  MAXEWORDMINVAL  ((eword)      0)
#define  MAXSWORDMINVAL  ((sword) -2147483647)
#define  MAXUWORDMINVAL  ((uword)           0)
#define  MINEWORDMAXVAL  ((eword)  2147483647)
#define  MINSWORDMAXVAL  ((sword)  2147483647)
#define  MINUWORDMAXVAL  ((uword)  4294967295)
#define  SWORDMAXVAL  ((sword) INT_MAX)
#define  SWORDMINVAL  ((sword) INT_MIN)
#define  UWORDMAXVAL  ((uword)UINT_MAX)
#define  UWORDMINVAL  ((uword)       0)
#define EB1MAXVAL      ((eb1)SCHAR_MAX)
#define EB1MINVAL      ((eb1)        0)
#define EB2MAXVAL      ((eb2) SHRT_MAX)
#define EB2MINVAL      ((eb2)        0)
#define EB4MAXVAL      ((eb4) INT_MAX)
#define EB4MINVAL      ((eb4)        0)
#define MAXEB1MINVAL   ((eb1)    0)
#define MAXEB2MINVAL   ((eb2)     0)
#define MAXEB4MINVAL   ((eb4)          0)
#define MAXORASB8MINVAL ((orasb8)-9223372036854775807)
#define MAXORAUB8MINVAL ((oraub8)0)
#define MAXSB1MINVAL ((sb1) -127)
#define MAXSB2MINVAL ((sb2)-32767)
#define MAXSB4MINVAL ((sb4)-2147483647)
#define MAXUB1MINVAL ((ub1)    0)
#define MAXUB2MINVAL ((ub2)     0)
#define MAXUB4MINVAL ((ub4)          0)
#define MINEB1MAXVAL   ((eb1)  127)
#define MINEB2MAXVAL   ((eb2) 32767)
#define MINEB4MAXVAL   ((eb4) 2147483647)
#define MINORASB8MAXVAL ((orasb8) 9223372036854775807)
#define MINORAUB8MAXVAL ((oraub8)18446744073709551615)
#define MINSB1MAXVAL ((sb1)  127)
#define MINSB2MAXVAL ((sb2) 32767)
#define MINSB4MAXVAL ((sb4) 2147483647)
#define MINUB1MAXVAL ((ub1)  255)
#define MINUB2MAXVAL ((ub2) 65535)
#define MINUB4MAXVAL ((ub4) 4294967295)
#define ORASB8MAXVAL    ((orasb8) 9223372036854775807)
#define ORASB8MINVAL    ((orasb8)-9223372036854775808)
#define ORAUB8MAXVAL    ((oraub8)18446744073709551615)
#define ORAUB8MINVAL    ((oraub8)0)
#define SB1MAXVAL ((sb1)SCHAR_MAX)
#define SB1MINVAL ((sb1)SCHAR_MIN)
#define SB2MAXVAL ((sb2) SHRT_MAX)
#define SB2MINVAL ((sb2) SHRT_MIN)
#define SB4MAXVAL ((sb4) INT_MAX)
#define SB4MINVAL ((sb4) INT_MIN)
#define UB1MAXVAL ((ub1)UCHAR_MAX)
#define UB1MINVAL ((ub1)        0)
#define UB2MAXVAL ((ub2)USHRT_MAX)
#define UB2MINVAL ((ub2)        0)
#define UB4MAXVAL ((ub4)UINT_MAX)
#define UB4MINVAL ((ub4)        0)
#define MINSIZE_TMAXVAL (size_t)4294967295
#define UBIGORABITS      (UB1BITS * sizeof(ubig_ora))
#undef CONST
#define CONST const
#define dvoid void
typedef void (*pLgenfp_t)(void);
typedef struct cdaDef Cda_Def;
typedef struct cdaDef Lda_Def;
#define OCI_EV_TSF 1
#define OCI_EV_DEF 0
#define OCI_LM_DEF 0
#define OCI_LM_NBL 1


#ifndef NZ0DECL_ORACLE
   typedef struct nzctx nzctx;
   typedef struct nzstrc nzstrc;
   typedef struct nzosContext nzosContext;
#endif


#ifndef SQLDEF

#define SQLCS_EXPLICIT 3
#define SQLCS_FLEXIBLE 4
#define SQLCS_IMPLICIT 1
#define SQLCS_LIT_NULL 5
#define SQLCS_NCHAR    2
#define SQLT_AFC  96
#define SQLT_AVC  97
#define SQLT_BDOUBLE 22
#define SQLT_BFILE SQLT_BFILEE
#define SQLT_BFILEE 114
#define SQLT_BFLOAT 21
#define SQLT_BIN  23
#define SQLT_BLOB 113
#define SQLT_CFILE SQLT_CFILEE
#define SQLT_CFILEE 115
#define SQLT_CHR  1
#define SQLT_CLOB 112
#define SQLT_CUR  102
#define SQLT_DAT  12
#define SQLT_DATE                      184
#define SQLT_FILE SQLT_BFILEE
#define SQLT_FLT  4
#define SQLT_IBDOUBLE 101
#define SQLT_IBFLOAT  100
#define SQLT_INT  3
#define SQLT_INTERVAL_DS               190
#define SQLT_INTERVAL_YM               189
#define SQLT_LAB  105
#define SQLT_LBI  24
#define SQLT_LNG  8
#define SQLT_LVB  95
#define SQLT_LVC  94
#define SQLT_NCO  122
#define SQLT_NON  10
#define SQLT_NTY  108
#define SQLT_NUM  2
#define SQLT_ODT  156
#define SQLT_OSL  106
#define SQLT_PDN  7
#define SQLT_PNTY   241
#define SQLT_RDD  104
#define SQLT_REF  110
#define SQLT_RID  11
#define SQLT_RSET 116
#define SQLT_SLS  91
#define SQLT_STR  5
#define SQLT_TIME                      185
#define SQLT_TIME_TZ                   186
#define SQLT_TIMESTAMP                 187
#define SQLT_TIMESTAMP_LTZ             232
#define SQLT_TIMESTAMP_TZ              188
#define SQLT_UIN  68
#define SQLT_VBI  15
#define SQLT_VCS  9
#define SQLT_VNU  6
#define SQLT_VST  155

#endif

//////////////////
///////////////////
#define OCI_AC_DDL    0
#define OCI_AFC_PAD_ON      0x00008000
#define OCI_ALWAYS_BLOCKING 0x00000400
#define OCI_AQ_LAST_ACKNOWLEDGED 1
#define OCI_AQ_LAST_ENQUEUED     0
#define OCI_AQ_RESERVED_1      0x0002
#define OCI_AQ_RESERVED_2      0x0004
#define OCI_AQ_RESERVED_3      0x0008
#define OCI_AQ_RESERVED_4      0x0010
#define OCI_AQ_STREAMING_FLAG  0x02000000
#define OCI_ASM_VOLUME_SUPPORTED   1
#define OCI_ASM_VOLUME_UNSUPPORTED 0
#define OCI_ATCH_ENABLE_BEQ  0x4000
#define OCI_ATCH_RESERVED_1  0x0020
#define OCI_ATCH_RESERVED_2  0x0080
#define OCI_ATCH_RESERVED_3  0x0100
#define OCI_ATCH_RESERVED_4  0x0400
#define OCI_ATCH_RESERVED_5  0x2000
#define OCI_ATCH_RESERVED_6  0x8000
#define OCI_ATCH_RESERVED_7  0x10000
#define OCI_ATCH_RESERVED_8  0x20000
#define OCI_ATTR_ACCESS_BANNER              307
#define OCI_ATTR_ACTION                     367
#define OCI_ATTR_ACTION_CONTEXT             351
#define OCI_ATTR_ADMIN_PFILE                389
#define OCI_ATTR_AGENT_ADDRESS          65
#define OCI_ATTR_AGENT_NAME             64
#define OCI_ATTR_AGENT_PROTOCOL         66
#define OCI_ATTR_ALLOC_DURATION 37
#define OCI_ATTR_APPCTX_ATTR               276
#define OCI_ATTR_APPCTX_LIST               274
#define OCI_ATTR_APPCTX_NAME               275
#define OCI_ATTR_APPCTX_SIZE               273
#define OCI_ATTR_APPCTX_VALUE              277
#define OCI_ATTR_AQ_ERROR_INDEX         OCI_ATTR_DML_ROW_OFFSET
#define OCI_ATTR_AQ_NTFN_GROUPING_COUNT            237
#define OCI_ATTR_AQ_NTFN_GROUPING_MSGID_ARRAY      236
#define OCI_ATTR_AQ_NUM_ERRORS          OCI_ATTR_NUM_DML_ERRORS
#define OCI_ATTR_ASM_VOL_SPRT               205
#define OCI_ATTR_ATTEMPTS               59
#define OCI_ATTR_AUDIT_BANNER               308
#define OCI_ATTR_AUDIT_SESSION_ID           362
#define OCI_ATTR_AUTOCOMMIT_DDL            271
#define OCI_ATTR_BIND_COUNT                 190
#define OCI_ATTR_BIND_DN         155
#define OCI_ATTR_BIND_ROWCBK                301
#define OCI_ATTR_BIND_ROWCTX                302
#define OCI_ATTR_BUF_ADDR               76
#define OCI_ATTR_BUF_SIZE               77
#define OCI_ATTR_CACHE            115
#define OCI_ATTR_CACHE_CLIENT_CONTEXT      251
#define OCI_ATTR_CACHE_MAX_SIZE 35
#define OCI_ATTR_CACHE_OPT_SIZE 34
#define OCI_ATTR_CALL_TIME                  370
#define OCI_ATTR_CANONICAL_ALGO             171
#define OCI_ATTR_CATALOG_LOCATION          268
#define OCI_ATTR_CERTIFICATE                169
#define OCI_ATTR_CHAR_COUNT  15
#define OCI_ATTR_CHAR_SIZE                 286
#define OCI_ATTR_CHAR_USED                 285
#define OCI_ATTR_CHARSET  20
#define OCI_ATTR_CHARSET_FORM 32
#define OCI_ATTR_CHARSET_ID 31
#define OCI_ATTR_CHDES_DBNAME            405
#define OCI_ATTR_CHDES_NFYTYPE           406
#define OCI_ATTR_CHDES_QUERIES 425
#define OCI_ATTR_CHDES_ROW_OPFLAGS       413
#define OCI_ATTR_CHDES_ROW_ROWID         412
#define OCI_ATTR_CHDES_TABLE_CHANGES     408
#define OCI_ATTR_CHDES_TABLE_NAME        409
#define OCI_ATTR_CHDES_TABLE_OPFLAGS     410
#define OCI_ATTR_CHDES_TABLE_ROW_CHANGES 411
#define OCI_ATTR_CHDES_XID               407
#define OCI_ATTR_CHNF_CHANGELAG           404
#define OCI_ATTR_CHNF_OPERATIONS          403
#define OCI_ATTR_CHNF_REGHANDLE          414
#define OCI_ATTR_CHNF_ROWIDS              402
#define OCI_ATTR_CHNF_TABLENAMES          401
#define OCI_ATTR_CLIENT_IDENTIFIER         278
#define OCI_ATTR_CLIENT_INFO                368
#define OCI_ATTR_CLUSTERED        105
#define OCI_ATTR_COL_COUNT              82
#define OCI_ATTR_COL_ENC        102
#define OCI_ATTR_COL_ENC_SALT   103
#define OCI_ATTR_COLLECT_CALL_TIME          369
#define OCI_ATTR_COLLECTION_ELEMENT        227
#define OCI_ATTR_COLLECTION_TYPECODE       217
#define OCI_ATTR_COMMENT                    343
#define OCI_ATTR_COMPLEXOBJECT_COLL_OUTOFLINE   53
#define OCI_ATTR_COMPLEXOBJECT_LEVEL            52
#define OCI_ATTR_COMPLEXOBJECTCOMP_TYPE         50
#define OCI_ATTR_COMPLEXOBJECTCOMP_TYPE_LEVEL   51
#define OCI_ATTR_CONDITION                  342
#define OCI_ATTR_CONN_BUSY_COUNT            179
#define OCI_ATTR_CONN_INCR                  185
#define OCI_ATTR_CONN_MAX                   184
#define OCI_ATTR_CONN_MIN                   183
#define OCI_ATTR_CONN_NOWAIT               178
#define OCI_ATTR_CONN_OPEN_COUNT            180
#define OCI_ATTR_CONN_TIMEOUT               181
#define OCI_ATTR_CONNECTION_CLASS  425
#define OCI_ATTR_CONSUMER_NAME          50
#define OCI_ATTR_CORRELATION            58
#define OCI_ATTR_CQ_QUERYID               304
#define OCI_ATTR_CQDES_OPERATION 422
#define OCI_ATTR_CQDES_QUERYID 424
#define OCI_ATTR_CQDES_TABLE_CHANGES 423
#define OCI_ATTR_CURRENT_ERRCOL            2003
#define OCI_ATTR_CURRENT_POSITION           164
#define OCI_ATTR_CURRENT_SCHEMA             224
#define OCI_ATTR_CURSOR_COMMIT_BEHAVIOR    266
#define OCI_ATTR_DATA_SIZE      1
#define OCI_ATTR_DATA_TYPE      2
#define OCI_ATTR_DATEFORMAT             75
#define OCI_ATTR_DB_CHARSET_ID     416
#define OCI_ATTR_DB_NCHARSET_ID    417
#define OCI_ATTR_DBDOMAIN         399
#define OCI_ATTR_DBNAME           391
#define OCI_ATTR_DEFAULT_LOBPREFETCH_SIZE     438
#define OCI_ATTR_DELAY                  56
#define OCI_ATTR_DEQ_MODE               51
#define OCI_ATTR_DEQ_MSGID              54
#define OCI_ATTR_DEQCOND                    146
#define OCI_ATTR_DESC_PUBLIC               250
#define OCI_ATTR_DESC_SYNBASE              284
#define OCI_ATTR_DESCRIBE_NATIVE            189
#define OCI_ATTR_DIGEST_ALGO                168
#define OCI_ATTR_DIGEST_VALUE               173
#define OCI_ATTR_DIRPATH_BADROW             329
#define OCI_ATTR_DIRPATH_BADROW_LENGTH      330
#define OCI_ATTR_DIRPATH_CONVERT            328
#define OCI_ATTR_DIRPATH_DCACHE_DISABLE     307
#define OCI_ATTR_DIRPATH_DCACHE_HITS        306
#define OCI_ATTR_DIRPATH_DCACHE_MISSES      305
#define OCI_ATTR_DIRPATH_DCACHE_NUM         303
#define OCI_ATTR_DIRPATH_DCACHE_SIZE        304
#define OCI_ATTR_DIRPATH_EXPR_TYPE  150
#define OCI_ATTR_DIRPATH_FILE               139
#define OCI_ATTR_DIRPATH_FN_CTX             167
#define OCI_ATTR_DIRPATH_GRANULE_OFFSET     333
#define OCI_ATTR_DIRPATH_GRANULE_SIZE       332
#define OCI_ATTR_DIRPATH_INDEX_MAINT_METHOD 138
#define OCI_ATTR_DIRPATH_INPUT         151
#define OCI_ATTR_DIRPATH_LOCK_WAIT          359
#define OCI_ATTR_DIRPATH_MODE           78
#define OCI_ATTR_DIRPATH_NOLOG          79
#define OCI_ATTR_DIRPATH_OBJ_CONSTR         206
#define OCI_ATTR_DIRPATH_OID                187
#define OCI_ATTR_DIRPATH_PARALLEL       80
#define OCI_ATTR_DIRPATH_RESERVED_1         334
#define OCI_ATTR_DIRPATH_RESERVED_10       2001
#define OCI_ATTR_DIRPATH_RESERVED_11       2002
#define OCI_ATTR_DIRPATH_RESERVED_12       2005
#define OCI_ATTR_DIRPATH_RESERVED_13       2006
#define OCI_ATTR_DIRPATH_RESERVED_14       2007
#define OCI_ATTR_DIRPATH_RESERVED_15       2008
#define OCI_ATTR_DIRPATH_RESERVED_16       2009
#define OCI_ATTR_DIRPATH_RESERVED_17       2010
#define OCI_ATTR_DIRPATH_RESERVED_18       2011
#define OCI_ATTR_DIRPATH_RESERVED_19       2012
#define OCI_ATTR_DIRPATH_RESERVED_2         335
#define OCI_ATTR_DIRPATH_RESERVED_3         337
#define OCI_ATTR_DIRPATH_RESERVED_4         338
#define OCI_ATTR_DIRPATH_RESERVED_5         357
#define OCI_ATTR_DIRPATH_RESERVED_6         358
#define OCI_ATTR_DIRPATH_RESERVED_7         326
#define OCI_ATTR_DIRPATH_RESERVED_8         327
#define OCI_ATTR_DIRPATH_RESERVED_9        2000
#define OCI_ATTR_DIRPATH_SID                194
#define OCI_ATTR_DIRPATH_SKIPINDEX_METHOD   145
#define OCI_ATTR_DIRPATH_SORTED_INDEX    137
#define OCI_ATTR_DIRPATH_STORAGE_INITIAL    140
#define OCI_ATTR_DIRPATH_STORAGE_NEXT       141
#define OCI_ATTR_DIRPATH_STREAM_VERSION     212
#define OCI_ATTR_DIRPATH_SUBTYPE_INDEX     2004
#define OCI_ATTR_DIRPATH_WRITE_ORDER        331
#define OCI_ATTR_DISP_NAME      100
#define OCI_ATTR_DISP_SIZE      3
#define OCI_ATTR_DISTINGUISHED_NAME        300
#define OCI_ATTR_DML_ROW_OFFSET         74
#define OCI_ATTR_DN_COUNT        161
#define OCI_ATTR_DRIVER_NAME                424
#define OCI_ATTR_DURATION         132
#define OCI_ATTR_ECONTEXT_ID                371
#define OCI_ATTR_ECONTEXT_SEQ               372
#define OCI_ATTR_EDITION                   288
#define OCI_ATTR_ENCAPSULATION             235
#define OCI_ATTR_ENCC_SIZE      101
#define OCI_ATTR_ENQ_TIME               62
#define OCI_ATTR_ENV  5
#define OCI_ATTR_ENV_CHARSET_ID   OCI_ATTR_CHARSET_ID
#define OCI_ATTR_ENV_NCHARSET_ID  OCI_ATTR_NCHARSET_ID
#define OCI_ATTR_ENV_UTF16                  209
#define OCI_ATTR_ERRONEOUS_COLUMN           203
#define OCI_ATTR_EVAL_CONTEXT_NAME          346
#define OCI_ATTR_EVAL_CONTEXT_OWNER         345
#define OCI_ATTR_EVALUATION_FUNCTION        347
#define OCI_ATTR_EVENTTYPE        400
#define OCI_ATTR_EVTCBK                     304
#define OCI_ATTR_EVTCTX                     305
#define OCI_ATTR_EXCEPTION_QUEUE        61
#define OCI_ATTR_EXPIRATION             57
#define OCI_ATTR_EXTERNAL_NAME   26
#define OCI_ATTR_FDO          39
#define OCI_ATTR_FETCH_ROWID                 448
#define OCI_ATTR_FLOW_CONTROL_TIMEOUT       423
#define OCI_ATTR_FNCODE  1
#define OCI_ATTR_FOCBK        43
#define OCI_ATTR_FSPRECISION OCI_ATTR_PDSCL
#define OCI_ATTR_HA_SOURCE        401
#define OCI_ATTR_HA_SRVFIRST      403
#define OCI_ATTR_HA_SRVNEXT       404
#define OCI_ATTR_HA_STATUS        402
#define OCI_ATTR_HA_TIMESTAMP     395
#define OCI_ATTR_HANDLE_POSITION            191
#define OCI_ATTR_HAS_DEFAULT    212
#define OCI_ATTR_HAS_FILE                  226
#define OCI_ATTR_HAS_LOB                   225
#define OCI_ATTR_HAS_NESTED_TABLE          224
#define OCI_ATTR_HAS_RESULT                240
#define OCI_ATTR_HAS_SUBTYPES        321
#define OCI_ATTR_HEAPALLOC 30
#define OCI_ATTR_HOSTNAME         390
#define OCI_ATTR_HW_MARK          117
#define OCI_ATTR_IN_V8_MODE   44
#define OCI_ATTR_INCR             114
#define OCI_ATTR_INDEX_ONLY       107
#define OCI_ATTR_INITIAL_CLIENT_ROLES  100
#define OCI_ATTR_INST_TYPE                  207
#define OCI_ATTR_INSTNAME         392
#define OCI_ATTR_INSTSTARTTIME    394
#define OCI_ATTR_INTERNAL_NAME   25
#define OCI_ATTR_IOMODE         213
#define OCI_ATTR_IS_CONSTANT               239
#define OCI_ATTR_IS_CONSTRUCTOR            241
#define OCI_ATTR_IS_DESTRUCTOR             242
#define OCI_ATTR_IS_EXTERNAL                216
#define OCI_ATTR_IS_FINAL_METHOD           281
#define OCI_ATTR_IS_FINAL_TYPE             279
#define OCI_ATTR_IS_INCOMPLETE_TYPE        219
#define OCI_ATTR_IS_INLINE                 238
#define OCI_ATTR_IS_INSTANTIABLE_METHOD    282
#define OCI_ATTR_IS_INSTANTIABLE_TYPE      280
#define OCI_ATTR_IS_INVOKER_RIGHTS 133
#define OCI_ATTR_IS_JAVA_TYPE              287
#define OCI_ATTR_IS_MAP                    244
#define OCI_ATTR_IS_NULL        7
#define OCI_ATTR_IS_OPERATOR               243
#define OCI_ATTR_IS_ORDER                  245
#define OCI_ATTR_IS_OVERRIDDEN_METHOD 325
#define OCI_ATTR_IS_OVERRIDING_METHOD      283
#define OCI_ATTR_IS_PREDEFINED_TYPE        221
#define OCI_ATTR_IS_RNDS                   246
#define OCI_ATTR_IS_RNPS                   247
#define OCI_ATTR_IS_SELFISH                236
#define OCI_ATTR_IS_SUBTYPE                258
#define OCI_ATTR_IS_SYSTEM_GENERATED_TYPE  223
#define OCI_ATTR_IS_SYSTEM_TYPE            220
#define OCI_ATTR_IS_TEMPORARY     130
#define OCI_ATTR_IS_TRANSIENT_TYPE         222
#define OCI_ATTR_IS_TYPED         131
#define OCI_ATTR_IS_VIRTUAL                237
#define OCI_ATTR_IS_WNDS                   248
#define OCI_ATTR_IS_WNPS                   249
#define OCI_ATTR_IS_XMLTYPE          315
#define OCI_ATTR_KERBEROS_CID               341
#define OCI_ATTR_KERBEROS_CID_KEY           364
#define OCI_ATTR_KERBEROS_KEY               363
#define OCI_ATTR_KERBEROS_TICKET           301
#define OCI_ATTR_LDAP_AUTH       158
#define OCI_ATTR_LDAP_CRED       156
#define OCI_ATTR_LDAP_CTX        159
#define OCI_ATTR_LDAP_HOST       153
#define OCI_ATTR_LDAP_PORT       154
#define OCI_ATTR_LEVEL          211
#define OCI_ATTR_LFPRECISION OCI_ATTR_PDPRC
#define OCI_ATTR_LINK             111
#define OCI_ATTR_LIST_ARGUMENTS   108
#define OCI_ATTR_LIST_COLUMNS     103
#define OCI_ATTR_LIST_OBJECTS              261
#define OCI_ATTR_LIST_SCHEMAS              263
#define OCI_ATTR_LIST_SUBPROGRAMS 109
#define OCI_ATTR_LIST_SUBTYPES       323
#define OCI_ATTR_LIST_TABLE_ALIASES         352
#define OCI_ATTR_LIST_TYPE_ATTRS           229
#define OCI_ATTR_LIST_TYPE_METHODS         231
#define OCI_ATTR_LIST_VARIABLE_TYPES        353
#define OCI_ATTR_LOB_REGION_MIME          447
#define OCI_ATTR_LOB_REGION_OFFSET        445
#define OCI_ATTR_LOB_REGION_PRIMARY       442
#define OCI_ATTR_LOB_REGION_PRIMOFF       443
#define OCI_ATTR_LOB_REGIOnLen        446
#define OCI_ATTR_LOBEMPTY     45
#define OCI_ATTR_LOBPREFETCH_LENGTH           440
#define OCI_ATTR_LOBPREFETCH_SIZE             439
#define OCI_ATTR_LOCKING_MODE              272
#define OCI_ATTR_LTYPE            128
#define OCI_ATTR_MAP_METHOD                232
#define OCI_ATTR_MAX              113
#define OCI_ATTR_MAX_CATALOG_NAMELEN       267
#define OCI_ATTR_MAX_COLUMN_LEN            265
#define OCI_ATTR_MAX_PROC_LEN              264
#define OCI_ATTR_MAXCHAR_SIZE               163
#define OCI_ATTR_MAXDATA_SIZE 33
#define OCI_ATTR_MEMPOOL_APPNAME        90
#define OCI_ATTR_MEMPOOL_HOMENAME       91
#define OCI_ATTR_MEMPOOL_INSTNAME       89
#define OCI_ATTR_MEMPOOL_MODEL          92
#define OCI_ATTR_MEMPOOL_SIZE           88
#define OCI_ATTR_MESSAGE_CSCN               360
#define OCI_ATTR_MESSAGE_DSCN               361
#define OCI_ATTR_MIGSESSION             86
#define OCI_ATTR_MIN              112
#define OCI_ATTR_MODES                  93
#define OCI_ATTR_MODULE                     366
#define OCI_ATTR_MSG_DELIVERY_MODE 407
#define OCI_ATTR_MSG_PROP               72
#define OCI_ATTR_MSG_STATE              63
#define OCI_ATTR_NAME           4
#define OCI_ATTR_NAVIGATION             52
#define OCI_ATTR_NCHAR   21
#define OCI_ATTR_NCHARSET_ID               262
#define OCI_ATTR_NESTED_PREFETCH_MEMORY 14
#define OCI_ATTR_NESTED_PREFETCH_ROWS 12
#define OCI_ATTR_NETWORK_FILE_DESC       415
#define OCI_ATTR_NFY_FLAGS        406
#define OCI_ATTR_NFY_MSGID              71
#define OCI_ATTR_NOCACHE                87
#define OCI_ATTR_NONBLOCKING_MODE  3
#define OCI_ATTR_NOWAIT_SUPPORT            270
#define OCI_ATTR_NUM_ARGS       215
#define OCI_ATTR_NUM_ATTRS        120
#define OCI_ATTR_NUM_COLS         102
#define OCI_ATTR_NUM_DML_ERRORS         73
#define OCI_ATTR_NUM_OPEN_STMTS             188
#define OCI_ATTR_NUM_PARAMS       121
#define OCI_ATTR_NUM_ROWS               81
#define OCI_ATTR_NUM_SUBTYPES        322
#define OCI_ATTR_NUM_TYPE_ATTRS            228
#define OCI_ATTR_NUM_TYPE_METHODS          230
#define OCI_ATTR_numElements                 234
#define OCI_ATTR_OBJ_ID           136
#define OCI_ATTR_OBJ_NAME         134
#define OCI_ATTR_OBJ_SCHEMA       135
#define OCI_ATTR_OBJ_SUBS                   336
#define OCI_ATTR_OBJECT   2
#define OCI_ATTR_OBJID            122
#define OCI_ATTR_ORA_DEBUG_JDWP            302
#define OCI_ATTR_ORDER            116
#define OCI_ATTR_ORDER_METHOD              233
#define OCI_ATTR_ORIGINAL_MSGID         69
#define OCI_ATTR_OVERLOAD       210
#define OCI_ATTR_OVERLOAD_ID      125
#define OCI_ATTR_PARAM            124
#define OCI_ATTR_PARAM_COUNT 18
#define OCI_ATTR_PARSE_ERROR_OFFSET 129
#define OCI_ATTR_PARTITIONED      106
#define OCI_ATTR_PASSWORD 23
#define OCI_ATTR_PDPRC   17
#define OCI_ATTR_PDSCL   16
#define OCI_ATTR_pinDur 38
#define OCI_ATTR_PINOPTION 36
#define OCI_ATTR_POSITION       11
#define OCI_ATTR_POSTPROCESSING_CALLBACK 40
#define OCI_ATTR_POSTPROCESSING_CONTEXT 41
#define OCI_ATTR_PRECISION      5
#define OCI_ATTR_PREFETCH_MEMORY 13
#define OCI_ATTR_PREFETCH_ROWS  11
#define OCI_ATTR_PRIORITY               55
#define OCI_ATTR_PRIVATE_KEY                172
#define OCI_ATTR_PROXY_CLIENT            416
#define OCI_ATTR_PROXY_CREDENTIALS      99
#define OCI_ATTR_PTYPE            123
#define OCI_ATTR_PURITY            426
#define OCI_ATTR_PURITY_DEFAULT    0x00
#define OCI_ATTR_PURITY_NEW        0x01
#define OCI_ATTR_PURITY_SELF       0x02
#define OCI_ATTR_QUEUE_NAME             70
#define OCI_ATTR_RADIX          214
#define OCI_ATTR_RDBA             104
#define OCI_ATTR_READONLY_TXN               201
#define OCI_ATTR_RECEIVE_TIMEOUT            436
#define OCI_ATTR_RECIPIENT_LIST         60
#define OCI_ATTR_REF_TDO          110
#define OCI_ATTR_RELATIVE_MSGID         48
#define OCI_ATTR_RESERVED_10                211
#define OCI_ATTR_RESERVED_12                214
#define OCI_ATTR_RESERVED_13                215
#define OCI_ATTR_RESERVED_14               303
#define OCI_ATTR_RESERVED_15                217
#define OCI_ATTR_RESERVED_16                219
#define OCI_ATTR_RESERVED_17                220
#define OCI_ATTR_RESERVED_18                221
#define OCI_ATTR_RESERVED_19                222
#define OCI_ATTR_RESERVED_2                 147
#define OCI_ATTR_RESERVED_20                223
#define OCI_ATTR_RESERVED_21                415
#define OCI_ATTR_RESERVED_22      396
#define OCI_ATTR_RESERVED_23      397
#define OCI_ATTR_RESERVED_24      398
#define OCI_ATTR_RESERVED_25       418
#define OCI_ATTR_RESERVED_26                422
#define OCI_ATTR_RESERVED_27      425
#define OCI_ATTR_RESERVED_28               426
#define OCI_ATTR_RESERVED_29               427
#define OCI_ATTR_RESERVED_3                 165
#define OCI_ATTR_RESERVED_30               428
#define OCI_ATTR_RESERVED_31               429
#define OCI_ATTR_RESERVED_32               430
#define OCI_ATTR_RESERVED_33               433
#define OCI_ATTR_RESERVED_34               434
#define OCI_ATTR_RESERVED_35                437
#define OCI_ATTR_RESERVED_36               444
#define OCI_ATTR_RESERVED_37              449
#define OCI_ATTR_RESERVED_4                 166
#define OCI_ATTR_RESERVED_41               454
#define OCI_ATTR_RESERVED_5                 192
#define OCI_ATTR_RESERVED_6                 200
#define OCI_ATTR_RESERVED_7                 202
#define OCI_ATTR_RESERVED_8                 204
#define OCI_ATTR_RESERVED_9                 210
#define OCI_ATTR_ROW_COUNT   9
#define OCI_ATTR_ROWID   19
#define OCI_ATTR_ROWS_FETCHED               197
#define OCI_ATTR_ROWS_RETURNED 42
#define OCI_ATTR_SAVEPOINT_SUPPORT         269
#define OCI_ATTR_SCALE          6
#define OCI_ATTR_SCHEMA_NAME    9
#define OCI_ATTR_SCN_BASE                   198
#define OCI_ATTR_SCN_WRAP                   199
#define OCI_ATTR_SEND_TIMEOUT               435
#define OCI_ATTR_SENDER_ID              68
#define OCI_ATTR_SEQUENCE_DEVIATION     49
#define OCI_ATTR_SERVER 6
#define OCI_ATTR_SERVER_BUSY                193
#define OCI_ATTR_SERVER_DN       162
#define OCI_ATTR_SERVER_DNS      160
#define OCI_ATTR_SERVER_GROUP           85
#define OCI_ATTR_SERVER_STATUS              143
#define OCI_ATTR_SERVICENAME      393
#define OCI_ATTR_SESSION 7
#define OCI_ATTR_SESSION_MIGRATED           377
#define OCI_ATTR_SESSION_PRESERVE_STATE     388
#define OCI_ATTR_SESSION_STATE              373
#define OCI_ATTR_SESSION_STATE_CLEARED      376
#define OCI_ATTR_SESSION_STATETYPE          374
#define OCI_ATTR_SESSLANG     46
#define OCI_ATTR_SHARED_HEAPALLOC       84
#define OCI_ATTR_SIGNATURE                  175
#define OCI_ATTR_SIGNATURE_ALGO             170
#define OCI_ATTR_SIGNATURE_VAL              174
#define OCI_ATTR_SKIP_BUFFER                303
#define OCI_ATTR_SPOOL_BUSY_COUNT           310
#define OCI_ATTR_SPOOL_GETMODE              309
#define OCI_ATTR_SPOOL_INCR                 314
#define OCI_ATTR_SPOOL_MAX                  313
#define OCI_ATTR_SPOOL_MIN                  312
#define OCI_ATTR_SPOOL_OPEN_COUNT           311
#define OCI_ATTR_SPOOL_STMTCACHESIZE        208
#define OCI_ATTR_SPOOL_AUTH                 460
#define OCI_ATTR_SPOOL_TIMEOUT              308
#define OCI_ATTR_SQLCODE  4
#define OCI_ATTR_SQLFNCODE 10
#define OCI_ATTR_STATEMENT                  144
#define OCI_ATTR_STMT_IS_RETURNING          218
#define OCI_ATTR_STMT_STATE                 182
#define OCI_ATTR_STMT_TYPE   24
#define OCI_ATTR_STMTCACHE_CBK              421
#define OCI_ATTR_STMTCACHE_CBKCTX           420
#define OCI_ATTR_STMTCACHESIZE              176
#define OCI_ATTR_STREAM_OFFSET          83
#define OCI_ATTR_SUB_NAME       10
#define OCI_ATTR_SUBSCR_CALLBACK        95
#define OCI_ATTR_SUBSCR_CQ_QOSFLAGS         229
#define OCI_ATTR_SUBSCR_CQ_REGID            230
#define OCI_ATTR_SUBSCR_CTX             96
#define OCI_ATTR_SUBSCR_NAME            94
#define OCI_ATTR_SUBSCR_NAMESPACE       98
#define OCI_ATTR_SUBSCR_NAMESPACE_CTX       228
#define OCI_ATTR_SUBSCR_NTFN_GROUPING_CLASS        231
#define OCI_ATTR_SUBSCR_NTFN_GROUPING_REPEAT_COUNT 235
#define OCI_ATTR_SUBSCR_NTFN_GROUPING_START_TIME   234
#define OCI_ATTR_SUBSCR_NTFN_GROUPING_TYPE         233
#define OCI_ATTR_SUBSCR_NTFN_GROUPING_VALUE        232
#define OCI_ATTR_SUBSCR_PAYLOAD         97
#define OCI_ATTR_SUBSCR_PAYLOADCBK          226
#define OCI_ATTR_SUBSCR_PORTNO              390
#define OCI_ATTR_SUBSCR_QOSFLAGS            225
#define OCI_ATTR_SUBSCR_RECPT               148
#define OCI_ATTR_SUBSCR_RECPTPRES           195
#define OCI_ATTR_SUBSCR_RECPTPROTO          149
#define OCI_ATTR_SUBSCR_TIMEOUT             227
#define OCI_ATTR_SUPERTYPE_NAME            260
#define OCI_ATTR_SUPERTYPE_SCHEMA_NAME     259
#define OCI_ATTR_TABLE_ENC         41
#define OCI_ATTR_TABLE_ENC_ALG     418
#define OCI_ATTR_TABLE_ENC_ALG_ID  419
#define OCI_ATTR_TABLE_NAME                 356
#define OCI_ATTR_TABLESPACE       126
#define OCI_ATTR_TAF_ENABLED      405
#define OCI_ATTR_TDO              127
#define OCI_ATTR_TIMESTAMP        119
#define OCI_ATTR_TRANS   8
#define OCI_ATTR_TRANS_LOCK 28
#define OCI_ATTR_TRANS_NAME 29
#define OCI_ATTR_TRANS_TIMEOUT              142
#define OCI_ATTR_TRANSACTION_NO             365
#define OCI_ATTR_TRANSFORMATION             196
#define OCI_ATTR_TYPE_NAME      8
#define OCI_ATTR_TYPE_SCHEMA      118
#define OCI_ATTR_TYPECODE                  216
#define OCI_ATTR_UCI_CONSTRUCT             252
#define OCI_ATTR_UCI_COPY                  254
#define OCI_ATTR_UCI_DESTRUCT              253
#define OCI_ATTR_UCI_PICKLE                255
#define OCI_ATTR_UCI_REFRESH               257
#define OCI_ATTR_UCI_UNPICKLE              256
#define OCI_ATTR_UNK              101
#define OCI_ATTR_USER_MEMORY               306
#define OCI_ATTR_USER_PROPERTY          67
#define OCI_ATTR_USERNAME 22
#define OCI_ATTR_VALUE                      344
#define OCI_ATTR_VAR_METHOD_FUNCTION        350
#define OCI_ATTR_VAR_TYPE                   348
#define OCI_ATTR_VAR_VALUE_FUNCTION         349
#define OCI_ATTR_VERSION                   218
#define OCI_ATTR_VISIBILITY             47
#define OCI_ATTR_WAIT                   53
#define OCI_ATTR_WALL_LOC        157
#define OCI_ATTR_XADFIELD_RESERVED_1        339
#define OCI_ATTR_XADFIELD_RESERVED_2        340
#define OCI_ATTR_XID     27
#define OCI_ATTR_XML_HRCHY_ENABLED   324
#define OCI_ATTR_XMLELEMENT_NAME     317
#define OCI_ATTR_XMLpTypeStored_OBJ  320
#define OCI_ATTR_XMLSCHEMA_NAME      316
#define OCI_ATTR_XMLSQLTYPE_NAME     319
#define OCI_ATTR_XMLSQLTYPSCH_NAME   318
#define OCI_ATTR_XMLTYPE_BINARY_XML  422
#define OCI_AUTH         0x08
#define OCI_AUTH_RESERVED_1 0x00000020
#define OCI_AUTH_RESERVED_2 0x00000400
#define OCI_AUTH_RESERVED_3 0x00000800
#define OCI_AUTH_RESERVED_4 0x00001000
#define OCI_AUTH_RESERVED_5 0x00002000
#define OCI_AUTH_RESERVED_6 0x00010000
#define OCI_BATCH_ERRORS           0x00000080
#define OCI_BATCH_MODE             0x00000001
#define OCI_BIND_RESERVED_2   0x00000010
#define OCI_BIND_RESERVED_3   0x00000100
#define OCI_BIND_SOFT         0x00000040
#define OCI_CBK_STMTCACHE_STMTPURGE  0x01
#define OCI_CL_END   1
#define OCI_CL_START 0
#define OCI_CLIENT_STATS    0x10
#define OCI_COMMIT_ON_SUCCESS      0x00000020
#define OCI_CONTINUE -24200
#define OCI_CPOOL            0x0200
#define OCI_CPOOL_REINITIALIZE 0x111
#define OCI_CRED_EXT        2
#define OCI_CRED_PROXY      3
#define OCI_CRED_RDBMS      1
#define OCI_CRED_RESERVED_1 4
#define OCI_CRED_RESERVED_2 5
#define OCI_CURSOR_CLOSED 1
#define OCI_CURSOR_OPEN   0
#define OCI_DATA_AT_EXEC      0x00000002
#define OCI_DEFAULT         0x00000000
#define OCI_DEFINE_RESERVED_1 0x00000008
#define OCI_DEFINE_RESERVED_2 0x00000020
#define OCI_DEFINE_SOFT       0x00000080
#define OCI_DEQ_BROWSE          1
#define OCI_DEQ_FIRST_MSG        1
#define OCI_DEQ_GETSIG          5
#define OCI_DEQ_IMMEDIATE       1
#define OCI_DEQ_LOCKED          2
#define OCI_DEQ_MULT_TRANSACTION 5
#define OCI_DEQ_NEXT_MSG         3
#define OCI_DEQ_NEXT_TRANSACTION 2
#define OCI_DEQ_NO_WAIT         0
#define OCI_DEQ_ON_COMMIT       2
#define OCI_DEQ_REMOVE          3
#define OCI_DEQ_REMOVE_NODATA   4
#define OCI_DEQ_RESERVED_1      0x000001
#define OCI_DEQ_WAIT_FOREVER    -1
#define OCI_DESC_RESERVED_1        0x00001000
#define OCI_DESCRIBE_ONLY          0x00000010
#define OCI_DIRPATH_INPUT_OCI         0x04
#define OCI_DIRPATH_INPUT_STREAM      0x02
#define OCI_DIRPATH_INPUT_TEXT        0x01
#define OCI_DIRPATH_INPUT_UNKNOWN     0x08
#define OCI_DIRPATH_STREAM_VERSION_1        100
#define OCI_DIRPATH_STREAM_VERSION_2        200
#define OCI_DIRPATH_STREAM_VERSION_3        300
#define OCI_DT_DAY_BELOW_VALID     0x2
#define OCI_DT_DAY_MISSING_FROM_1582 0x1000
#define OCI_DT_HOUR_BELOW_VALID    0x80
#define OCI_DT_INVALID_DAY         0x1
#define OCI_DT_INVALID_FORMAT      0x8000
#define OCI_DT_INVALID_HOUR        0x40
#define OCI_DT_INVALID_MINUTE      0x100
#define OCI_DT_INVALID_MONTH       0x4
#define OCI_DT_INVALID_SECOND      0x400
#define OCI_DT_INVALID_TIMEZONE    0x4000
#define OCI_DT_INVALID_YEAR        0x10
#define OCI_DT_MINUTE_BELOW_VALID  0x200
#define OCI_DT_MONTH_BELOW_VALID   0x8
#define OCI_DT_SECOND_BELOW_VALID  0x800
#define OCI_DT_YEAR_BELOW_VALID    0x20
#define OCI_DT_YEAR_ZERO           0x2000
#define OCI_DTYPE_AQAGENT 60
#define OCI_DTYPE_AQDEQ_OPTIONS 58
#define OCI_DTYPE_AQENQ_OPTIONS 57
#define OCI_DTYPE_AQLIS_MSG_PROPERTIES 76
#define OCI_DTYPE_DATETIME 62
#define OCI_DTYPE_AQLIS_OPTIONS 75
#define OCI_DTYPE_AQMSG_PROPERTIES 59
#define OCI_DTYPE_AQNFY_DESCRIPTOR  64
#define OCI_DTYPE_CHDES         77
#define OCI_DTYPE_COMPLEXOBJECTCOMP  55
#define OCI_DTYPE_CQDES  80
#define OCI_DTYPE_DATE 65
#define OCI_DTYPE_FILE 56
#define OCI_DTYPE_FIRST 50
#define OCI_DTYPE_INTERVAL_DS 63
#define OCI_DTYPE_INTERVAL_YM 62
#define OCI_DTYPE_LAST          81
#define OCI_DTYPE_LOB 50
#define OCI_DTYPE_LOB_REGION    81
#define OCI_DTYPE_LOCATOR 61
#define OCI_DTYPE_PARAM 53
#define OCI_DTYPE_RESERVED_1    74
#define OCI_DTYPE_ROW_CHDES     79
#define OCI_DTYPE_ROWID  54
#define OCI_DTYPE_RSET 52
#define OCI_DTYPE_SIGNATURE     73
#define OCI_DTYPE_SNAP 51
#define OCI_DTYPE_SRVDN         72
#define OCI_DTYPE_TABLE_CHDES   78
#define OCI_DTYPE_TIME 66
#define OCI_DTYPE_TIME_TZ 67
#define OCI_DTYPE_TIMESTAMP 68
#define OCI_DTYPE_TIMESTAMP_LTZ 70
#define OCI_DTYPE_TIMESTAMP_TZ 69
#define OCI_DTYPE_UCB           71
#define OCI_DUMP_HEAP       0x80
#define OCI_DYNAMIC_FETCH     0x00000002
#define OCI_ENABLE_NLS_VALIDATION     0x01000000
#define OCI_ENQ_BEFORE          2
#define OCI_ENQ_IMMEDIATE       1
#define OCI_ENQ_ON_COMMIT       2
#define OCI_ENQ_TOP             3
#define OCI_ENV_NO_MUTEX 0x08
#define OCI_ENV_NO_UCB 0x01
#define OCI_ENVCR_RESERVED3 0x00010000
#define OCI_ERROR -1
#define OCI_ERROR_MAXMSG_SIZE   1024
#define OCI_EVENT_DEREG 0x5
#define OCI_EVENT_DROP_DB 0x4
#define OCI_EVENT_NONE 0x0
#define OCI_EVENT_OBJCHANGE 0x6
#define OCI_EVENT_QUERYCHANGE 0x7
#define OCI_EVENT_SHUTDOWN 0x2
#define OCI_EVENT_SHUTDOWN_ANY 0x3
#define OCI_EVENT_STARTUP 0x1
#define OCI_EVENTS          0x00000004
#define OCI_EVENTTYPE_HA            0
#define OCI_EXACT_FETCH            0x00000002
#define OCI_EXACT_FETCH_RESERVED_1 0x00000200
#define OCI_EXEC_RESERVED_2        0x00000800
#define OCI_EXEC_RESERVED_3        0x00002000
#define OCI_EXEC_RESERVED_4        0x00004000
#define OCI_EXEC_RESERVED_5        0x00008000
#define OCI_EXEC_RESERVED_6        0x00010000
#define OCI_FASTPATH         0x0010
#define OCI_FETCH_ABSOLUTE   0x00000020
#define OCI_FETCH_CURRENT    0x00000001
#define OCI_FETCH_FIRST      0x00000004
#define OCI_FETCH_LAST       0x00000008
#define OCI_FETCH_NEXT       0x00000002
#define OCI_FETCH_PRIOR      0x00000010
#define OCI_FETCH_RELATIVE   0x00000040
#define OCI_FETCH_RESERVED_1 0x00000080
#define OCI_FETCH_RESERVED_2 0x00000100
#define OCI_FETCH_RESERVED_3 0x00000200
#define OCI_FETCH_RESERVED_4 0x00000400
#define OCI_FETCH_RESERVED_5 0x00000800
#define OCI_FLOW_CONTROL_NO_TIMEOUT      -1
#define OCI_FNCODE_APPCTXCLEARALL   137
#define OCI_FNCODE_APPCTXSET        136
#define OCI_FNCODE_AQDEQ  91
#define OCI_FNCODE_AQDEQARRAY  111
#define OCI_FNCODE_AQENQ  90
#define OCI_FNCODE_AQENQARRAY  110
#define OCI_FNCODE_AQENQSTREAM        124
#define OCI_FNCODE_AQGETREPLAY        125
#define OCI_FNCODE_AQLISTEN  96
#define OCI_FNCODE_AQRESETREPLAY      126
#define OCI_FNCODE_ARRAYDESCRIPTORALLOC 127
#define OCI_FNCODE_ARRAYDESCRIPTORFREE  128
#define OCI_FNCODE_ATTRGET 54
#define OCI_FNCODE_ATTRSET 55
#define OCI_FNCODE_BINDARRAYOFSTRUCT   20
#define OCI_FNCODE_BINDBYNAME 67
#define OCI_FNCODE_BINDBYPOS 66
#define OCI_FNCODE_BINDDYNAMIC   17
#define OCI_FNCODE_BINDOBJECT  18
#define OCI_FNCODE_CPOOLCREATE  100
#define OCI_FNCODE_CPOOLDESTROY 101
#define OCI_FNCODE_DBSHUTDOWN 121
#define OCI_FNCODE_DBSTARTUP 120
#define OCI_FNCODE_DEFINEARRAYOFSTRUCT  27
#define OCI_FNCODE_DEFINEBYPOS 65
#define OCI_FNCODE_DEFINEDYNAMIC   26
#define OCI_FNCODE_DEFINEOBJECT  25
#define OCI_FNCODE_DESCRIBEANY  32
#define OCI_FNCODE_DESCRIPTORALLOC  4
#define OCI_FNCODE_DESCRIPTORFREE  5
#define OCI_FNCODE_ENVINIT   6
#define OCI_FNCODE_ERRORGET   37
#define OCI_FNCODE_HANDLEALLOC  2
#define OCI_FNCODE_HANDLEFREE  3
#define OCI_FNCODE_INITIALIZE     1
#define OCI_FNCODE_KERBATTRSET 52
#define OCI_FNCODE_LDATOSVCCTX 59
#define OCI_FNCODE_LOBAPPEND  43
#define OCI_FNCODE_LOBARRAYREAD       122
#define OCI_FNCODE_LOBARRAYWRITE      123
#define OCI_FNCODE_LOBASSIGN  68
#define OCI_FNCODE_LOBCHARSETFORM  73
#define OCI_FNCODE_LOBCHARSETID  72
#define OCI_FNCODE_LOBCLOSE  82
#define OCI_FNCODE_LOBCLOSEFILE  39
#define OCI_FNCODE_LOBCOPY  42
#define OCI_FNCODE_LOBCOPY2    112
#define OCI_FNCODE_LOBCREATETEMP  87
#define OCI_FNCODE_LOBDISABLEBUFFERING 78
#define OCI_FNCODE_LOBENABLEBUFFERING  71
#define OCI_FNCODE_LOBERASE  44
#define OCI_FNCODE_LOBERASE2   113
#define OCI_FNCODE_LOBFILECLOSEALL  86
#define OCI_FNCODE_LOBFILEEXISTS  85
#define OCI_FNCODE_LOBFILEGETNAME  75
#define OCI_FNCODE_LOBFILEISOPEN  84
#define OCI_FNCODE_LOBFILESETNAME  74
#define OCI_FNCODE_LOBFLUSHBUFFER 79
#define OCI_FNCODE_LOBFRAGDEL       132
#define OCI_FNCODE_LOBFRAGINS       131
#define OCI_FNCODE_LOBFRAGMOV       133
#define OCI_FNCODE_LOBFRAGREP       134
#define OCI_FNCODE_LOBFREETEMP  88
#define OCI_FNCODE_LOBGETDEDUPLICATEREGIONS 135
#define OCI_FNCODE_LOBGETOPT        129
#define OCI_FNCODE_LOBGETSTORAGELIMIT 119
#define OCI_FNCODE_LOBISEQUAL  69
#define OCI_FNCODE_LOBISINIT  70
#define OCI_FNCODE_LOBISOPEN  83
#define OCI_FNCODE_LOBISTEMP  89
#define OCI_FNCODE_LOBLENGTH  45
#define OCI_FNCODE_LOBLENGTH2  114
#define OCI_FNCODE_LOBLOADFROMFILE 80
#define OCI_FNCODE_LOBLOADFROMFILE2  115
#define OCI_FNCODE_LOBLOCATORASSIGN 94
#define OCI_FNCODE_LOBOPEN  81
#define OCI_FNCODE_LOBOPENFILE  38
#define OCI_FNCODE_LOBREAD  47
#define OCI_FNCODE_LOBREAD2    116
#define OCI_FNCODE_LOBSETOPT        130
#define OCI_FNCODE_LOBTRIM  46
#define OCI_FNCODE_LOBTRIM2    117
#define OCI_FNCODE_LOBWRITE  48
#define OCI_FNCODE_LOBWRITE2   118
#define OCI_FNCODE_LOGOFF 77
#define OCI_FNCODE_LOGON 76
#define OCI_FNCODE_LOGON2 102
#define OCI_FNCODE_MAXFCN             137
#define OCI_FNCODE_PARAMGET 57
#define OCI_FNCODE_PARAMSET 56
#define OCI_FNCODE_PASSWORDCHANGE   12
#define OCI_FNCODE_RESET  92
#define OCI_FNCODE_ROWIDTOCHAR  103
#define OCI_FNCODE_SERVERATTACH   7
#define OCI_FNCODE_SERVERDETACH   8
#define OCI_FNCODE_SERVERVERSION  51
#define OCI_FNCODE_SESSIONBEGIN  10
#define OCI_FNCODE_SESSIONEND   11
#define OCI_FNCODE_SESSIONGET   106
#define OCI_FNCODE_SESSIONRELEASE 107
#define OCI_FNCODE_SPOOLCREATE  104
#define OCI_FNCODE_SPOOLDESTROY 105
#define OCI_FNCODE_STMTEXECUTE  21
#define OCI_FNCODE_STMTFETCH   28
#define OCI_FNCODE_STMTGETBIND   29
#define OCI_FNCODE_STMTGETPIECEINFO   58
#define OCI_FNCODE_STMTPREPARE   13
#define OCI_FNCODE_STMTPREPARE2 108
#define OCI_FNCODE_STMTRELEASE 109
#define OCI_FNCODE_STMTSETPIECEINFO   61
#define OCI_FNCODE_SVC2HST 97
#define OCI_FNCODE_SVCCTXBREAK 50
#define OCI_FNCODE_SVCCTXTOLDA  93
#define OCI_FNCODE_SVCRH   98
#define OCI_FNCODE_TRANSCOMMIT  35
#define OCI_FNCODE_TRANSDETACH  34
#define OCI_FNCODE_TRANSFORGET 62
#define OCI_FNCODE_TRANSMULTIPREPARE   99
#define OCI_FNCODE_TRANSPREPARE 63
#define OCI_FNCODE_TRANSROLLBACK  64
#define OCI_FNCODE_TRANSSTART  33
#define OCI_FNCODE_UBINDBYNAME 95
#define OCI_FO_ABORT        0x00000002
#define OCI_FO_BEGIN        0x00000008
#define OCI_FO_END          0x00000001
#define OCI_FO_ERROR        0x00000010
#define OCI_FO_NONE           0x00000001
#define OCI_FO_REAUTH       0x00000004
#define OCI_FO_RETRY        25410
#define OCI_FO_SELECT         0x00000004
#define OCI_FO_SESSION        0x00000002
#define OCI_FO_TXNAL          0x00000008
#define OCI_HA_SOURCE_ASM_INSTANCE        5
#define OCI_HA_SOURCE_DATABASE            1
#define OCI_HA_SOURCE_INSTANCE            0
#define OCI_HA_SOURCE_NODE                2
#define OCI_HA_SOURCE_SERVICE             3
#define OCI_HA_SOURCE_SERVICE_MEMBER      4
#define OCI_HA_SOURCE_SERVICE_PRECONNECT  6
#define OCI_HA_STATUS_DOWN          0
#define OCI_HA_STATUS_UP            1
#define OCI_HTYPE_ADMIN         28
#define OCI_HTYPE_AUTHINFO      OCI_HTYPE_SESSION
#define OCI_HTYPE_BIND           5
#define OCI_HTYPE_COMPLEXOBJECT 11
#define OCI_HTYPE_CPOOL         26
#define OCI_HTYPE_DEFINE         6
#define OCI_HTYPE_DESCRIBE       7
#define OCI_HTYPE_DIRPATH_COLUMN_ARRAY 15
#define OCI_HTYPE_DIRPATH_CTX   14
#define OCI_HTYPE_DIRPATH_FN_COL_ARRAY 19
#define OCI_HTYPE_DIRPATH_FN_CTX       18
#define OCI_HTYPE_DIRPATH_STREAM       16
#define OCI_HTYPE_ENV            1
#define OCI_HTYPE_ERROR          2
#define OCI_HTYPE_EVENT         29
#define OCI_HTYPE_FIRST          1
#define OCI_HTYPE_LAST          29
#define OCI_HTYPE_PROC                 17
#define OCI_HTYPE_SECURITY      12
#define OCI_HTYPE_SERVER         8
#define OCI_HTYPE_SESSION        9
#define OCI_HTYPE_SPOOL         27
#define OCI_HTYPE_STMT           4
#define OCI_HTYPE_SUBSCRIPTION  13
#define OCI_HTYPE_SVCCTX         3
#define OCI_HTYPE_TRANS         10
#define OCI_HTYPE_XADFIELD      22
#define OCI_HTYPE_XADGRANULE    23
#define OCI_HTYPE_XADIO         25
#define OCI_HTYPE_XADRECORD     24
#define OCI_HTYPE_XADSESSION    20
#define OCI_HTYPE_XADTABLE      21
#define OCI_INSTANCE_TYPE_OSM      2
#define OCI_INSTANCE_TYPE_RDBMS    1
#define OCI_INSTANCE_TYPE_UNKNOWN  0
#define OCI_INTER_DAY_BELOW_VALID     0x2
#define OCI_INTER_FRACSEC_BELOW_VALID 0x2000
#define OCI_INTER_HOUR_BELOW_VALID    0x80
#define OCI_INTER_INVALID_DAY         0x1
#define OCI_INTER_INVALID_FRACSEC     0x1000
#define OCI_INTER_INVALID_HOUR        0x40
#define OCI_INTER_INVALID_MINUTE      0x100
#define OCI_INTER_INVALID_MONTH       0x4
#define OCI_INTER_INVALID_SECOND      0x400
#define OCI_INTER_INVALID_YEAR        0x10
#define OCI_INTER_MINUTE_BELOW_VALID  0x200
#define OCI_INTER_MONTH_BELOW_VALID   0x8
#define OCI_INTER_SECOND_BELOW_VALID  0x800
#define OCI_INTER_YEAR_BELOW_VALID    0x20
#define OCI_INVALID_HANDLE -2
#define OCI_IOV               0x00000200
#define OCI_LOBMAXSIZE MINUB4MAXVAL
#define OCI_LOCK_DELAYED   1
#define OCI_LOCK_IMMEDIATE 0
#define OCI_LOGON2_CPOOL       OCI_CPOOL
#define OCI_LOGON2_PROXY       0x0008
#define OCI_LOGON2_SPOOL       0x0001
#define OCI_LOGON2_STMTCACHE   0x0004
#define OCI_MAX_FNS   100
#define OCI_MEM_CLN         0x02
#define OCI_MEM_FLUSH       0x04
#define OCI_MEM_INIT        0x01
#define OCI_MIGRATE         0x00000001
#define OCI_MSG_BUFFERED                 2
#define OCI_MSG_EXPIRED         3
#define OCI_MSG_NO_DELAY        0
#define OCI_MSG_NO_EXPIRATION -1
#define OCI_MSG_PERSISTENT               1
#define OCI_MSG_PERSISTENT_OR_BUFFERED   3
#define OCI_MSG_PROCESSED       2
#define OCI_MSG_READY           0
#define OCI_MSG_WAITING         1
#define OCI_MUTEX_ENV_ONLY  0x00080000
#define OCI_MUTEX_TRY                 0x00200000
#define OCI_NCHAR_LITERAL_REPLACE_OFF 0x00800000
#define OCI_NCHAR_LITERAL_REPLACE_ON  0x00400000
#define OCI_NEED_DATA 99
#define OCI_NEW_LENGTH_SEMANTICS  0x00020000
#define OCI_NO_AC_DDL 1
#define OCI_NO_DATA 100
#define OCI_NO_MUTEX        0x00000080
#define OCI_NO_MUTEX_STMT   0x00040000
#define OCI_NO_RESULT_CACHE        0x00040000
#define OCI_NO_SHARING        0x01
#define OCI_NO_UCB          0x00000040
#define OCI_NON_BLOCKING           0x00000040
#define OCI_NTFN_GROUPING_FOREVER -1
#define OCI_NTV_SYNTAX 1
#define OCI_NW_SUPPORTED   0
#define OCI_NW_UNSUPPORTED 1
#define OCI_OBJECT          0x00000002
#define OCI_OPCODE_ALLOPS 0x0
#define OCI_OPCODE_ALLROWS 0x1
#define OCI_OPCODE_ALTER 0x10
#define OCI_OPCODE_DELETE 0x8
#define OCI_OPCODE_DROP 0x20
#define OCI_OPCODE_INSERT 0x2
#define OCI_OPCODE_UNKNOWN 0x40
#define OCI_OPCODE_UPDATE 0x4
#define OCI_OTYPE_FUNC          5
#define OCI_OTYPE_NAME 1
#define OCI_OTYPE_PKG           6
#define OCI_OTYPE_PROC          4
#define OCI_OTYPE_PTR  3
#define OCI_OTYPE_REF  2
#define OCI_OTYPE_STMT          7
#define OCI_OTYPE_SYN           3
#define OCI_OTYPE_TABLE         1
#define OCI_OTYPE_UNK           0
#define OCI_OTYPE_VIEW          2
#define OCI_PARAM_IN 0x01
#define OCI_PARAM_OUT 0x02
#define OCI_PARSE_ONLY             0x00000100
#define OCI_PIECEWISE         0x00000004
#define OCI_PRELIM_AUTH     0x00000008
#define OCI_PREP_AFC_PAD_OFF  0x08
#define OCI_PREP_AFC_PAD_ON   0x04
#define OCI_PREP_RESERVED_1   0x02
#define OCI_PREP2_CACHE_SEARCHONLY    0x0010
#define OCI_PREP2_GET_PLSQL_WARNINGS  0x0020
#define OCI_PREP2_RESERVED_1          0x0040
#define OCI_REG_LDAPONLY    0x00002000
#define OCI_RESERVED_FOR_INT_USE 200
#define OCI_RESERVED1       0x00000008
#define OCI_RESERVED2       0x00000020
#define OCI_RESULT_CACHE           0x00020000
#define OCI_ROWCBK_DONE   -24201
#define OCI_ROWID_LEN             23
#define OCI_SB2_IND_PTR       0x00000001
#define OCI_SERVER_NORMAL               0x1
#define OCI_SERVER_NOT_CONNECTED        0x0
#define OCI_SERVER_STATS    0x20
#define OCI_SESSEND_RESERVED_1 0x0001
#define OCI_SESSEND_RESERVED_2 0x0002
#define OCI_SESSGET_CPOOL      OCI_CPOOL
#define OCI_SESSGET_CREDEXT    0x0010
#define OCI_SESSGET_CREDPROXY  0x0008
#define OCI_SESSGET_PURITY_NEW     0x0040
#define OCI_SESSGET_PURITY_SELF    0x0080
#define OCI_SESSGET_SPOOL      0x0001
#define OCI_SESSGET_SPOOL_MATCHANY 0x0020
#define OCI_SESSGET_STMTCACHE  0x0004
#define OCI_SESSION_STATEFUL   2
#define OCI_SESSION_STATELESS  1
#define OCI_SESSION_STATELESS_APP  3
#define OCI_SESSION_STATELESS_CAL  1
#define OCI_SESSION_STATELESS_DEF  0
#define OCI_SESSION_STATELESS_TXN  2
#define OCI_SESSRLS_DROPSESS 0x0001
#define OCI_SESSRLS_RETAG    0x0002
#define OCI_SHARED          0x00000010
#define OCI_SHARED_EXT      0x00000100
#define OCI_SHOW_DML_WARNINGS      0x00000400
#define OCI_SP_SUPPORTED   0
#define OCI_SP_UNSUPPORTED 1
#define OCI_SPC_HOMOGENEOUS  0x0002
#define OCI_SPC_NO_RLB       0x0008
#define OCI_SPC_REINITIALIZE 0x0001
#define OCI_SPC_STMTCACHE    0x0004
#define OCI_SPD_FORCE        0x0001
#define OCI_SPOOL_ATTRVAL_FORCEGET 2
#define OCI_SPOOL_ATTRVAL_NOWAIT   1
#define OCI_SPOOL_ATTRVAL_WAIT     0
#define OCI_SQLSTATE_SIZE 5
#define OCI_SRVATCH_RESERVED5 0x01000000
#define OCI_SRVATCH_RESERVED6 0x02000000
#define OCI_STATELESS_APP   0x00000200
#define OCI_STATELESS_CALL  0x00000080
#define OCI_STATELESS_TXN   0x00000100
#define OCI_STILL_EXECUTING -3123
#define OCI_STM_RESERVED4   0x00100000
#define OCI_STMT_CACHE      0x00000040
#define OCI_STMT_SCROLLABLE_READONLY 0x00000008
#define OCI_STMT_STATE_END_OF_FETCH 0x0003
#define OCI_STMT_STATE_EXECUTED     0x0002
#define OCI_STMT_STATE_INITIALIZED  0x0001
#define OCI_STRLS_CACHE_DELETE      0x0010
#define OCI_SUBSCR_CQ_QOS_BEST_EFFORT 0x02
#define OCI_SUBSCR_CQ_QOS_CLQRYCACHE 0x04
#define OCI_SUBSCR_CQ_QOS_QUERY  0x01
#define OCI_SUBSCR_NAMESPACE_ANONYMOUS   0
#define OCI_SUBSCR_NAMESPACE_AQ          1
#define OCI_SUBSCR_NAMESPACE_DBCHANGE    2
#define OCI_SUBSCR_NAMESPACE_MAX         3
#define OCI_SUBSCR_NTFN_GROUPING_CLASS_TIME 1
#define OCI_SUBSCR_NTFN_GROUPING_TYPE_LAST    2
#define OCI_SUBSCR_NTFN_GROUPING_TYPE_SUMMARY 1
#define OCI_SUBSCR_PRES_DEFAULT             0
#define OCI_SUBSCR_PRES_MAX                 2
#define OCI_SUBSCR_PRES_XML                 1
#define OCI_SUBSCR_PROTO_HTTP               3
#define OCI_SUBSCR_PROTO_MAIL               1
#define OCI_SUBSCR_PROTO_MAX                4
#define OCI_SUBSCR_PROTO_OCI                0
#define OCI_SUBSCR_PROTO_SERVER             2
#define OCI_SUBSCR_QOS_MULTICBK             0x20
#define OCI_SUBSCR_QOS_PAYLOAD              0x02
#define OCI_SUBSCR_QOS_PURGE_ON_NTFN        0x10
#define OCI_SUBSCR_QOS_RELIABLE             0x01
#define OCI_SUBSCR_QOS_REPLICATE            0x04
#define OCI_SUBSCR_QOS_SECURE               0x08
#define OCI_SUCCESS 0
#define OCI_SUCCESS_WITH_INFO 1
#define OCI_SUPPRESS_NLS_VALIDATION   0x00100000
#define OCI_SYSASM          0x00008000
#define OCI_SYSDBA          0x00000002
#define OCI_SYSOPER         0x00000004
#define OCI_TEMP_BLOB 1
#define OCI_TEMP_CLOB 2
#define OCI_TEMP_NCLOB 3
#define OCI_THREADED        0x00000001
#define OCI_TRANS_ISOLMASK     0x0000ff00
#define OCI_TRANS_JOIN         0x00000002
#define OCI_TRANS_LOOSE        0x00010000
#define OCI_TRANS_NEW          0x00000001
#define OCI_TRANS_NOMIGRATE    0x00100000
#define OCI_TRANS_OTHRMASK     0xfff00000
#define OCI_TRANS_OTSRESUME    0x00400000
#define OCI_TRANS_PROMOTE      0x00000008
#define OCI_TRANS_READONLY     0x00000100
#define OCI_TRANS_READWRITE    0x00000200
#define OCI_TRANS_RESUME       0x00000004
#define OCI_TRANS_SEPARABLE    0x00200000
#define OCI_TRANS_SERIALIZABLE 0x00000400
#define OCI_TRANS_STARTMASK    0x000000ff
#define OCI_TRANS_TIGHT        0x00020000
#define OCI_TRANS_TWOPHASE      0x01000000
#define OCI_TRANS_TYPEMASK     0x000f0000
#define OCI_TRANS_WRITEBATCH    0x00000001
#define OCI_TRANS_WRITEIMMED    0x00000002
#define OCI_TRANS_WRITENOWAIT   0x00000008
#define OCI_TRANS_WRITEWAIT     0x00000004
#define OCI_UCS2ID            1000
#define OCI_USE_LDAP        0x00001000
#define OCI_UTF16           0x00004000
#define OCI_UTF16ID           1000
#define OCI_V7_SYNTAX 2
#define OCI_V8_SYNTAX 3
#define OCIP_ATTR_DIRPATH_VARRAY_INDEX      213
#define OCIP_ICACHE         0x00000010
#define   OCI_FILE_BIN      0
#define   OCI_FILE_STDERR   4
#define   OCI_FILE_STDIN    2
#define   OCI_FILE_STDOUT   3
#define   OCI_FILE_TEXT     1
#define  OCI_DURATION_BEGIN (OCIDuration)10
#define  OCI_DURATION_CALLOUT (OCIDuration)(OCI_DURATION_BEGIN+4)
#define  OCI_DURATION_DEFAULT (OCIDuration)(OCI_DURATION_BEGIN-2)
#define  OCI_DURATION_INVALID 0xFFFF
#define  OCI_DURATION_LAST OCI_DURATION_CALLOUT
#define  OCI_DURATION_NEXT (OCIDuration)(OCI_DURATION_BEGIN-4)
#define  OCI_DURATION_NULL (OCIDuration)(OCI_DURATION_BEGIN-1)
#define  OCI_DURATION_PROCESS (OCIDuration)(OCI_DURATION_BEGIN-5)
#define  OCI_DURATION_SESSION (OCIDuration)(OCI_DURATION_BEGIN)
#define  OCI_DURATION_STATEMENT (OCIDuration)(OCI_DURATION_BEGIN+3)
#define  OCI_DURATION_TRANS (OCIDuration)(OCI_DURATION_BEGIN+1)
#define  OCI_DURATION_USER_CALLBACK (OCIDuration)(OCI_DURATION_BEGIN-3)
#define  OCI_DURATION_CALL (OCIDuration)(OCI_DURATION_BEGIN+2)
#define  OCI_STMT_ALTER   7
#define  OCI_STMT_BEGIN   8
#define  OCI_STMT_CALL    10
#define  OCI_STMT_CREATE  5
#define  OCI_STMT_DECLARE 9
#define  OCI_STMT_DELETE  3
#define  OCI_STMT_DROP    6
#define  OCI_STMT_INSERT  4
#define  OCI_STMT_SELECT  1
#define  OCI_STMT_UNKNOWN 0
#define  OCI_STMT_UPDATE  2
#define OCI_ADJUST_UNK            10
#define OCI_ANSI_DATE             1
#define OCI_ATTR_CACHE_ARRAYFLUSH 0x00000040
#define OCI_ATTR_OBJECT_DETECTCHANGE            0x00000020
#define OCI_ATTR_OBJECT_NEWNOTNULL   0x00000010
#define OCI_COHERENCY_ALWAYS (OCIRefreshOpt)5
#define OCI_COHERENCY_NONE   (OCIRefreshOpt)2
#define OCI_COHERENCY_NULL   (OCIRefreshOpt)4
#define OCI_DATE_DAY_BELOW_VALID        0x2
#define OCI_DATE_DAY_MISSING_FROM_1582  0x1000
#define OCI_DATE_HOUR_BELOW_VALID       0x80
#define OCI_DATE_INVALID_DAY            0x1
#define OCI_DATE_INVALID_FORMAT         0x8000
#define OCI_DATE_INVALID_HOUR           0x40
#define OCI_DATE_INVALID_MINUTE         0x100
#define OCI_DATE_INVALID_MONTH          0x4
#define OCI_DATE_INVALID_SECOND         0x400
#define OCI_DATE_INVALID_YEAR           0x10
#define OCI_DATE_MINUTE_BELOW_VALID     0x200
#define OCI_DATE_MONTH_BELOW_VALID      0x8
#define OCI_DATE_SECOND_BELOW_VALID     0x800
#define OCI_DATE_YEAR_BELOW_VALID       0x20
#define OCI_DATE_YEAR_ZERO              0x2000
#define OCIDateGetTime(/*IN*/date, /*OUT*/hour, /*OUT*/min, /*OUT*/sec) /* LOGIC REMOVED BY IAN HAY */
#define OCIDateGetDate(/*IN*/date, /*OUT*/year, /*OUT*/month, /*OUT*/day) /* LOGIC REMOVED BY IAN HAY */
#define OCIDateSetTime(/*OUT*/date, /*IN*/hour, /*IN*/min, /*IN*/sec) /* LOGIC REMOVED BY IAN HAY */
#define OCIDateSetDate(date, year, month, day) /* LOGIC REMOVED BY IAN HAY */

#define OCI_DBSHUTDOWN_ABORT              4
#define OCI_DBSHUTDOWN_FINAL              5
#define OCI_DBSHUTDOWN_IMMEDIATE           3
#define OCI_DBSHUTDOWN_TRANSACTIONAL      1
#define OCI_DBSHUTDOWN_TRANSACTIONAL_LOCAL 2
#define OCI_DBSTARTUPFLAG_FORCE 0x00000001
#define OCI_DBSTARTUPFLAG_RESTRICT 0x00000002
#define OCI_EXTRACT_APPEND_VALUES      0x4
#define OCI_EXTRACT_CASE_SENSITIVE     0x1
#define OCI_EXTRACT_MULTIPLE           0x8
#define OCI_EXTRACT_TYPE_BOOLEAN         1
#define OCI_EXTRACT_TYPE_INTEGER         3
#define OCI_EXTRACT_TYPE_OCINUM          4
#define OCI_EXTRACT_TYPE_STRING          2
#define OCI_EXTRACT_UNIQUE_ABBREVS     0x2
#define OCI_FILE_APPEND            8
#define OCI_FILE_BACKWARD    2
#define OCI_FILE_CREATE            1
#define OCI_FILE_EXCL              2
#define OCI_FILE_EXIST             0
#define OCI_FILE_FORWARD     1
#define OCI_FILE_READ_ONLY         1
#define OCI_FILE_READ_WRITE        3
#define OCI_FILE_READONLY 1
#define OCI_FILE_SEEK_BEGINNING    1
#define OCI_FILE_SEEK_CURRENT      2
#define OCI_FILE_SEEK_END          3
#define OCI_FILE_TRUNCATE          4
#define OCI_FILE_WRITE_ONLY        2
#define OCI_IND_BADNULL (OCIInd)(-2)
#define OCI_IND_NOTNULL (OCIInd)0
#define OCI_IND_NOTNULLABLE (OCIInd)(-3)
#define OCI_IND_NULL (OCIInd)(-1)
#define OCI_INTHR_UNK 24
#define OCI_KERBCRED_CLIENT_IDENTIFIER   2
#define OCI_KERBCRED_PROXY               1
#define OCI_LOB_APPENDONLY    4
#define OCI_LOB_BUFFER_FREE   1
#define OCI_LOB_BUFFER_NOFREE 2
#define OCI_LOB_COMPRESS_OFF  0
#define OCI_LOB_COMPRESS_ON   1
#define OCI_LOB_DEDUPLICATE_OFF 0
#define OCI_LOB_DEDUPLICATE_ON  4
#define OCI_LOB_ENCRYPT_OFF   0
#define OCI_LOB_ENCRYPT_ON    2
#define OCI_LOB_FULLOVERWRITE 5
#define OCI_LOB_FULLREAD      6
#define OCI_LOB_OPT_ALLOCSIZE 8
#define OCI_LOB_OPT_COMPRESS  1
#define OCI_LOB_OPT_DEDUPLICATE 4
#define OCI_LOB_OPT_ENCRYPT   2
#define OCI_LOB_OPT_MIMETYPE 16
#define OCI_LOB_OPT_MODTIME  32
#define OCI_LOB_READONLY 1
#define OCI_LOB_READWRITE 2
#define OCI_LOB_WRITEONLY     3
#define OCI_LTYPE_ARG_FUNC      3
#define OCI_LTYPE_ARG_PROC      2
#define OCI_LTYPE_COLUMN        1
#define OCI_LTYPE_DB_SCH        10
#define OCI_LTYPE_NAME_VALUE    14
#define OCI_LTYPE_SCH_OBJ       9
#define OCI_LTYPE_SUBPRG        4
#define OCI_LTYPE_TABLE_ALIAS   12
#define OCI_LTYPE_TYPE_ARG_FUNC 8
#define OCI_LTYPE_TYPE_ARG_PROC 7
#define OCI_LTYPE_TYPE_ATTR     5
#define OCI_LTYPE_TYPE_METHOD   6
#define OCI_LTYPE_TYPE_SUBTYPE  11
#define OCI_LTYPE_UNK           0
#define OCI_LTYPE_VARIABLE_TYPE 13
#define OCI_MAJOR_VERSION  11
#define OCI_MEMORY_CLEARED  1
#define OCI_METHOD_IS_ABSTRACT(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_CONSTANT(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_CONSTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_DESTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_INLINE(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_MAP(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_OPERATOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_ORDER(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_OVERRIDING(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_PIPELINED(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_RNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_RNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_SELFISH(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_VIRTUAL(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_WNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_METHOD_IS_WNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_MINOR_VERSION  1
#define OCI_NLS_ABDAYNAME1    8
#define OCI_NLS_ABDAYNAME2    9
#define OCI_NLS_ABDAYNAME3    10
#define OCI_NLS_ABDAYNAME4    11
#define OCI_NLS_ABDAYNAME5    12
#define OCI_NLS_ABDAYNAME6    13
#define OCI_NLS_ABDAYNAME7    14
#define OCI_NLS_ABLANGUAGE    53
#define OCI_NLS_ABMONTHNAME1  27
#define OCI_NLS_ABMONTHNAME10 36
#define OCI_NLS_ABMONTHNAME11 37
#define OCI_NLS_ABMONTHNAME12 38
#define OCI_NLS_ABMONTHNAME2  28
#define OCI_NLS_ABMONTHNAME3  29
#define OCI_NLS_ABMONTHNAME4  30
#define OCI_NLS_ABMONTHNAME5  31
#define OCI_NLS_ABMONTHNAME6  32
#define OCI_NLS_ABMONTHNAME7  33
#define OCI_NLS_ABMONTHNAME8  34
#define OCI_NLS_ABMONTHNAME9  35
#define OCI_NLS_ABTERRITORY   80
#define OCI_NLS_AD            43
#define OCI_NLS_AM            41
#define OCI_NLS_BC            44
#define OCI_NLS_BINARY            0x1
#define OCI_NLS_CALENDAR      57
#define OCI_NLS_CASE_INSENSITIVE  0x10
#define OCI_NLS_CHARACTER_SET 55
#define OCI_NLS_CHARSET_FIXEDWIDTH 92
#define OCI_NLS_CHARSET_ID    93
#define OCI_NLS_CHARSET_MAXBYTESZ 91
#define OCI_NLS_CREDIT        48
#define OCI_NLS_CS_IANA_TO_ORA   0
#define OCI_NLS_CS_ORA_TO_IANA   1
#define OCI_NLS_DATEFORMAT    49
#define OCI_NLS_DAYNAME1      1
#define OCI_NLS_DAYNAME2      2
#define OCI_NLS_DAYNAME3      3
#define OCI_NLS_DAYNAME4      4
#define OCI_NLS_DAYNAME5      5
#define OCI_NLS_DAYNAME6      6
#define OCI_NLS_DAYNAME7      7
#define OCI_NLS_DDATEFORMAT   81
#define OCI_NLS_DEBIT         47
#define OCI_NLS_DECIMAL       45
#define OCI_NLS_DTIMEFORMAT   82
#define OCI_NLS_DUAL_CURRENCY 78
#define OCI_NLS_GROUP         46
#define OCI_NLS_INT_CURRENCY  50
#define OCI_NLS_INT_CURRENCYSEP 90
#define OCI_NLS_LANG_ISO_TO_ORA  2
#define OCI_NLS_LANG_ORA_TO_ISO  3
#define OCI_NLS_LANGUAGE      52
#define OCI_NLS_LINGUISTIC        0x2
#define OCI_NLS_LINGUISTIC_NAME    56
#define OCI_NLS_LISTSEP       86
#define OCI_NLS_LOC_CURRENCY  51
#define OCI_NLS_LOCALE_A2_ISO_TO_ORA 8
#define OCI_NLS_LOCALE_A2_ORA_TO_ISO 9
#define OCI_NLS_LOWERCASE         0x40
#define OCI_NLS_MAXBUFSZ   100
#define OCI_NLS_MONDECIMAL    87
#define OCI_NLS_MONGROUP      88
#define OCI_NLS_MONGROUPING   89
#define OCI_NLS_MONTHNAME1    15
#define OCI_NLS_MONTHNAME10   24
#define OCI_NLS_MONTHNAME11   25
#define OCI_NLS_MONTHNAME12   26
#define OCI_NLS_MONTHNAME2    16
#define OCI_NLS_MONTHNAME3    17
#define OCI_NLS_MONTHNAME4    18
#define OCI_NLS_MONTHNAME5    19
#define OCI_NLS_MONTHNAME6    20
#define OCI_NLS_MONTHNAME7    21
#define OCI_NLS_MONTHNAME8    22
#define OCI_NLS_MONTHNAME9    23
#define OCI_NLS_NCHARSET_ID   94
#define OCI_NLS_NO            40
#define OCI_NLS_NUMGROUPING   85
#define OCI_NLS_PM            42
#define OCI_NLS_SFDATEFORMAT  83
#define OCI_NLS_SFTIMEFORMAT  84
#define OCI_NLS_TERR_ISO_TO_ORA  4
#define OCI_NLS_TERR_ISO3_TO_ORA 6
#define OCI_NLS_TERR_ORA_TO_ISO  5
#define OCI_NLS_TERR_ORA_TO_ISO3 7
#define OCI_NLS_TERRITORY     54
#define OCI_NLS_UPPERCASE         0x20
#define OCI_NLS_WRITINGDIR    79
#define OCI_NLS_YES           39
#define OCI_NUMBER_DEFAULTPREC ((ub1)0)
#define OCI_NUMBER_DEFAULTSCALE ((sb1)MAXSB1MINVAL)
#define OCI_NUMBER_SIZE 22
#define OCI_OBJECT_DELETED 0x0002
#define OCI_OBJECT_IS_DELETED(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_OBJECT_IS_DIRTY(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_OBJECT_IS_NEW(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_OBJECT_IS_UPDATED(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_OBJECT_NEW     0x0001
#define OCI_OBJECT_UPDATED 0x0004
#define OCI_OBJECTCOPY_NOREF (ub1)0x01
#define OCI_OBJECTFREE_FORCE      (ub2)0x0001
#define OCI_OBJECTFREE_HEADER     (ub2)0x0004
#define OCI_OBJECTFREE_NONULL     (ub2)0x0002
#define OCI_OBJECTPROP_ALLOC_DURATION 5
#define OCI_OBJECTPROP_LIFETIME 1
#define OCI_OBJECTPROP_LOCK 6
#define OCI_OBJECTPROP_MARKSTATUS 7
#define OCI_OBJECTPROP_pinDur 4
#define OCI_OBJECTPROP_SCHEMA 2
#define OCI_OBJECTPROP_TABLE 3
#define OCI_OBJECTPROP_VIEW 8
#define OCI_ORACLE_DATE           0
#define OCI_PTYPE_ARG                 10
#define OCI_PTYPE_COL                 9
#define OCI_PTYPE_DATABASE            18
#define OCI_PTYPE_EVALUATION_CONTEXT  21
#define OCI_PTYPE_FUNC                4
#define OCI_PTYPE_LIST                11
#define OCI_PTYPE_NAME_VALUE          24
#define OCI_PTYPE_PKG                 5
#define OCI_PTYPE_PROC                3
#define OCI_PTYPE_RULE                19
#define OCI_PTYPE_RULE_SET            20
#define OCI_PTYPE_SCHEMA              17
#define OCI_PTYPE_SEQ                 8
#define OCI_PTYPE_SYN                 7
#define OCI_PTYPE_TABLE               1
#define OCI_PTYPE_TABLE_ALIAS         22
#define OCI_PTYPE_TYPE                6
#define OCI_PTYPE_TYPE_ARG            15
#define OCI_PTYPE_TYPE_ATTR           12
#define OCI_PTYPE_TYPE_COLL           13
#define OCI_PTYPE_TYPE_METHOD         14
#define OCI_PTYPE_TYPE_RESULT         16
#define OCI_PTYPE_UNK                 0
#define OCI_PTYPE_VARIABLE_TYPE       23
#define OCI_PTYPE_VIEW                2
#define OCI_STRING_MAXLEN  4000
#define OCI_TYPECODE_BDOUBLE     SQLT_IBDOUBLE
#define OCI_TYPECODE_BFILE       SQLT_BFILE
#define OCI_TYPECODE_BFLOAT      SQLT_IBFLOAT
#define OCI_TYPECODE_BLOB        SQLT_BLOB
#define OCI_TYPECODE_CFILE       SQLT_CFILE
#define OCI_TYPECODE_CHAR        SQLT_AFC
#define OCI_TYPECODE_CLOB        SQLT_CLOB
#define OCI_TYPECODE_DATE        SQLT_DAT
#define OCI_TYPECODE_DECIMAL     SQLT_PDN
#define OCI_TYPECODE_DOUBLE      22
#define OCI_TYPECODE_ERRHP         283
#define OCI_TYPECODE_FLOAT       SQLT_FLT
#define OCI_TYPECODE_INTEGER     SQLT_INT
#define OCI_TYPECODE_INTERVAL_DS SQLT_INTERVAL_DS
#define OCI_TYPECODE_INTERVAL_YM SQLT_INTERVAL_YM
#define OCI_TYPECODE_MLSLABEL    SQLT_LAB
#define OCI_TYPECODE_NAMEDCOLLECTION  SQLT_NCO
#define OCI_TYPECODE_NCHAR       286
#define OCI_TYPECODE_NCLOB       288
#define OCI_TYPECODE_NONE          0
#define OCI_TYPECODE_NUMBER      SQLT_NUM
#define OCI_TYPECODE_NVARCHAR2   287
#define OCI_TYPECODE_OBJECT      SQLT_NTY
#define OCI_TYPECODE_OCTET       245
#define OCI_TYPECODE_OPAQUE       58
#define OCI_TYPECODE_OTMFIRST    228
#define OCI_TYPECODE_OTMLAST     320
#define OCI_TYPECODE_PLS_INTEGER   266
#define OCI_TYPECODE_PTR         32
#define OCI_TYPECODE_RAW         SQLT_LVB
#define OCI_TYPECODE_REAL        21
#define OCI_TYPECODE_REF         SQLT_REF
#define OCI_TYPECODE_SIGNED16    28
#define OCI_TYPECODE_SIGNED32    29
#define OCI_TYPECODE_SIGNED8     27
#define OCI_TYPECODE_SMALLINT    246
#define OCI_TYPECODE_SYSFIRST    228
#define OCI_TYPECODE_SYSLAST     235
#define OCI_TYPECODE_TABLE       248
#define OCI_TYPECODE_TIME        SQLT_TIME
#define OCI_TYPECODE_TIME_TZ     SQLT_TIME_TZ
#define OCI_TYPECODE_TIMESTAMP   SQLT_TIMESTAMP
#define OCI_TYPECODE_TIMESTAMP_LTZ  SQLT_TIMESTAMP_LTZ
#define OCI_TYPECODE_TIMESTAMP_TZ  SQLT_TIMESTAMP_TZ
#define OCI_TYPECODE_UNSIGNED16  25
#define OCI_TYPECODE_UNSIGNED32  26
#define OCI_TYPECODE_UNSIGNED8   SQLT_BIN
#define OCI_TYPECODE_UROWID      SQLT_RDD
#define OCI_TYPECODE_VARCHAR     SQLT_CHR
#define OCI_TYPECODE_VARCHAR2    SQLT_VCS
#define OCI_TYPECODE_VARRAY      247
#define OCI_TYPEELEM_IS_REF(elem_flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEELEM_REF        0x8000
#define OCI_TYPEMETHOD_CLEAR_CONSTANT(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_CONSTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_DESTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_INLINE(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_MAP(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_OPERATOR(flag)  /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_ORDER(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_RNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_RNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_SELFISH(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_VIRTUAL(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_WNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_CLEAR_WNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_ABSTRACT(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_CONSTANT(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_CONSTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_DESTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_INLINE(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_MAP(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_OPERATOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_ORDER(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_OVERRIDING(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_PIPELINED(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_RNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_RNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_SELFISH(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_VIRTUAL(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_WNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_IS_WNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_CONSTANT(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_CONSTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_DESTRUCTOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_INLINE(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_MAP(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_OPERATOR(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_ORDER(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_RNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_RNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_SELFISH(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_VIRTUAL(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_WNDS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEMETHOD_SET_WNPS(flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEPARAM_IS_REQUIRED(param_flag) /* LOGIC REMOVED BY IAN HAY */
#define OCI_TYPEPARAM_REQUIRED  0x0800
#define OCI_UCBTYPE_ENTRY       1
#define OCI_UCBTYPE_EXIT        2
#define OCI_UCBTYPE_REPLACE     3
#define OCI_VARRAY_MAXSIZE 4000
#define OCI_XMLTYPE_CREATE_BLOB      3
#define OCI_XMLTYPE_CREATE_CLOB      2
#define OCI_XMLTYPE_CREATE_OCISTRING 1
#define OCICoherency OCIRefreshOpt
#define OCIEXTPROC_ERROR   1
#define OCIEXTPROC_SUCCESS 0
#define OCIExtProcAllocCallMemory(withContext, amount) /* LOGIC REMOVED BY IAN HAY */
#define OCIExtProcGetEnv(withContext, envh, svch, errh) /* LOGIC REMOVED BY IAN HAY */
#define OCIExtProcRaiseExcp(withContext, errnum) /* LOGIC REMOVED BY IAN HAY */
#define OCIExtProcRaiseExcpWithMsg(withContext, errnum, errmsg, msglen) /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatChar(variable)     /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatDouble(variable)   /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatDP 6
#define OCIFormatDvoid(variable)    /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatEb1(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatEb2(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatEb4(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatEnd                /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatEword(variable)    /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatSb1(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatSb2(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatSb4(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatSbig_ora(variable)/* LOGIC REMOVED BY IAN HAY */
#define OCIFormatSword(variable)    /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatText(variable)     /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatUb1(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatUb2(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatUb4(variable)      /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatUbig_ora(variable) /* LOGIC REMOVED BY IAN HAY */
#define OCIFormatUword(variable)    /* LOGIC REMOVED BY IAN HAY */
#define OCIInitializeStatementHandle(wctx, cursorno,  svch, stmthp, errh) /* LOGIC REMOVED BY IAN HAY */
#define OCINumberToLnx(num) ((lnxnum_t *)num)

#define NZERROR_SSL_TO_ORACLE(ssl_error_) /* LOGIC REMOVED BY IAN HAY */

#define NZT_DEFAULT_WRL    ((text *)"default:")
#define NZT_ENTR_WRL       ((text *)"entr:")
#define NZT_FILE_WRL       ((text *)"file:")
#define NZT_MAX_MD5  16
#define NZT_MAX_SHA1 20
#define NZT_MCS_WRL        ((text *)"mcs:")
#define NZT_ORACLE_WRL      ((text *)"oracle:")
#define NZT_REGISTRY_WRL   ((text *)"reg:")
#define NZT_SQLNET_WRL     ((text *)"sqlnet:")
#define NZTTKPUSAGE_CERT_SIGN 16
#define NZTTKPUSAGE_CODE_SIGN 8
#define NZTTKPUSAGE_INVALID_USE 0xffff
#define NZTTKPUSAGE_NONE 0
#define NZTTKPUSAGE_SMIME_ENCR 2
#define NZTTKPUSAGE_SMIME_SIGN 4
#define NZTTKPUSAGE_SSL 1
#define NZTTKPUSAGE_SSL_CLIENT 32
#define NUM_OCIXMLDBPARAMS 2
#define OCI_DIRPATH_COL_COMPLETE 0
#define OCI_DIRPATH_COL_ERROR    3
#define OCI_DIRPATH_COL_NULL     1
#define OCI_DIRPATH_COL_PARTIAL  2
#define OCI_DIRPATH_CONVERT      3
#define OCI_DIRPATH_DATASAVE_FINISH   1
#define OCI_DIRPATH_DATASAVE_PARTIAL  2
#define OCI_DIRPATH_DATASAVE_SAVEONLY 0
#define OCI_DIRPATH_EXPR_OBJ_CONSTR  1
#define OCI_DIRPATH_EXPR_REF_TBLNAME 3
#define OCI_DIRPATH_EXPR_SQL         2
#define OCI_DIRPATH_INDEX_MAINT_DONT_SKIP_UNUSABLE  3
#define OCI_DIRPATH_INDEX_MAINT_SINGLE_ROW     1
#define OCI_DIRPATH_INDEX_MAINT_SKIP_ALL            4
#define OCI_DIRPATH_INDEX_MAINT_SKIP_UNUSABLE       2
#define OCI_DIRPATH_LOAD         1
#define OCI_DIRPATH_NORMAL       1
#define OCI_DIRPATH_NOT_PREPARED 3
#define OCI_DIRPATH_PARTIAL      2
#define OCI_DIRPATH_UNLOAD       2

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct cda_head {
    sb2          v2_rc;
    ub2          ft;
    ub4          rpc;
    ub2          peo;
    ub1          fc;
    ub1          rcs1;
    ub2          rc;
    ub1          wrn;
    ub1          rcs2;
    sword        rcs3;
    struct {
        struct {
           ub4    rcs4;
           ub2    rcs5;
           ub1    rcs6;
        } rd;
        ub4    rcs7;
        ub2    rcs8;
    } rid;
    sword        ose;
    ub1           chk;
    void         *pRcsp;
};


struct cdaDef {
    sb2          v2_rc;
    ub2          ft;
    ub4          rpc;
    ub2          peo;
    ub1          fc;
    ub1          rcs1;
    ub2          rc;
    ub1          wrn;
    ub1          rcs2;
    sword        rcs3;
    struct {
        struct {
           ub4    rcs4;
           ub2    rcs5;
           ub1    rcs6;
        } rd;
        ub4    rcs7;
        ub2    rcs8;
    } rid;
    sword        ose;
    ub1           chk;
    void         *pRcsp;
    ub1          rcs9[CDA_SIZE - sizeof (struct cda_head)];
};

enum OCITypeGetOpt
{
  OCI_TYPEGET_HEADER,
  OCI_TYPEGET_ALL
};
typedef enum OCITypeGetOpt OCITypeGetOpt;


enum OCITypeEncap
{
  OCI_TYPEENCAP_PRIVATE,
  OCI_TYPEENCAP_PUBLIC
};
typedef enum OCITypeEncap OCITypeEncap;

struct OCIIOV
{
 void     *bfp;
 ub4       bfl;
};

enum OCIPinOpt
{
  OCI_PIN_DEFAULT = 1,
  OCI_PIN_ANY = 3,
  OCI_PIN_RECENT = 4,
  OCI_PIN_LATEST = 5
};
typedef enum OCIPinOpt OCIPinOpt;


enum OCILockOpt
{
  OCI_LOCK_NONE = 1,
  OCI_LOCK_X = 2,
  OCI_LOCK_X_NOWAIT = 3
};
typedef enum OCILockOpt OCILockOpt;

enum OCIMarkOpt
{
  OCI_MARK_DEFAULT = 1,
  OCI_MARK_NONE = OCI_MARK_DEFAULT,
  OCI_MARK_UPDATE
};
typedef enum OCIMarkOpt OCIMarkOpt;

enum OCILobMode
{
  OCI_LOBMODE_READONLY = 1,
  OCI_LOBMODE_READWRITE = 2
};
typedef enum OCILobMode OCILobMode;

//enum OCIRefreshOpt
//{
//  OCI_REFRESH_LOADED = 1
//};

enum OCITypeMethodFlag
{
  OCI_TYPEMETHOD_ABSTRACT = 0x2000,
  OCI_TYPEMETHOD_CONSTANT = 0x0002,
  OCI_TYPEMETHOD_CONSTRUCTOR = 0x0008,
  OCI_TYPEMETHOD_DESTRUCTOR = 0x0010,
  OCI_TYPEMETHOD_INLINE = 0x0001,
  OCI_TYPEMETHOD_MAP = 0x0080,
  OCI_TYPEMETHOD_OPERATOR  = 0x0020,
  OCI_TYPEMETHOD_ORDER  = 0x0100,
  OCI_TYPEMETHOD_OVERRIDING = 0x4000,
  OCI_TYPEMETHOD_PIPELINED = 0x8000,
  OCI_TYPEMETHOD_RNDS= 0x0200,
  OCI_TYPEMETHOD_RNPS= 0x0800,
  OCI_TYPEMETHOD_SELFISH = 0x0040,
  OCI_TYPEMETHOD_VIRTUAL = 0x0004,
  OCI_TYPEMETHOD_WNDS= 0x0400,
  OCI_TYPEMETHOD_WNPS= 0x1000
};
typedef enum OCITypeMethodFlag OCITypeMethodFlag;

enum OCITypeParamMode
{
  OCI_TYPEPARAM_IN = 0,
  OCI_TYPEPARAM_OUT,
  OCI_TYPEPARAM_INOUT,
  OCI_TYPEPARAM_BYREF,
  OCI_TYPEPARAM_OUTNCPY,
  OCI_TYPEPARAM_INOUTNCPY
};
typedef enum OCITypeParamMode OCITypeParamMode;

struct OCINumber
{
  ub1 OCINumberPart[OCI_NUMBER_SIZE];
};
typedef struct OCINumber OCINumber;

#define OCI_NUMBER_UNSIGNED 0
#define OCI_NUMBER_SIGNED   2

////////////////////////////////////////////////////////////////////////////////
//////////////////////  D A T E   T Y P E

struct OCITime
{
  ub1 OCITimeHH;         //  between  0 -23
  ub1 OCITimeMI;         //  between  0 -59
  ub1 OCITimeSS;         //  between  0 -59
};
typedef struct OCITime OCITime;

struct OCIDate
{
  sb2 OCIDateYYYY;        // from -4712 to 9999
  ub1 OCIDateMM;          //  between  1 -12
  ub1 OCIDateDD;          //  between  1 -31
  OCITime OCIDateTime;
};
typedef struct OCIDate OCIDate;
////////////////////      E R R O R
typedef enum nzerror
{
  NZEROR_BIND_SUBKEY_COUNT = 28798,
  NZERROR_3DES_SELF_TEST_FAILED = 29151,
  NZERROR_AES_SELF_TEST_FAILED = 29165,
  NZERROR_ALREADY_INITIALIZED = 28770,
  NZERROR_ASNBadEncodingErr = 28866,
  NZERROR_ASNIntegerTooBigErr = 28867,
  NZERROR_ATTRIBUTE_FINISH_FAILED = 28795,
  NZERROR_ATTRIBUTE_INIT = 28794,
  NZERROR_AUTH_SHARED_MEMORY = 28799,
  NZERROR_BAD_FILE_ID = 28772,
  NZERROR_BAD_MAGIC_NUMBER = 28768,
  NZERROR_BAD_PARAMETER_METHOD = 28780,
  NZERROR_BAD_PASSWORD    = 28852,
  NZERROR_BAD_VERSION_NUMBER = 28767,
  NZERROR_BadCipherSuite = 28884,
  NZERROR_BIND_SERVICE_ERROR = 43050,
  NZERROR_BINDING_CREATION_FAILED = 28777,
  NZERROR_BS_CERTOBJ_CREAT_FAILED = 29140,
  NZERROR_BS_DER_IMP_FAILED = 29141,
  NZERROR_CANNOT_MODIFY_AL = 43072,
  NZERROR_CERT_ALREADY_INSTALLED = 29225,
  NZERROR_CERT_CHAIN_CREATION  = 29223,
  NZERROR_CERT_IN_CRL = 29178,
  NZERROR_CERT_IN_CRL_CHECK_FAILED = 29179,
  NZERROR_CERT_NOT_IN_CRL = 29177,
  NZERROR_CERTIFICATE_VERIFY = 28791,
  NZERROR_CertNotInstalled = 29000,
  NZERROR_CIC_ERR_BAD_CTX = 29094,
  NZERROR_CIC_ERR_BAD_INDEX = 29095,
  NZERROR_CIC_ERR_BAD_LENGTH = 29096,
  NZERROR_CIC_ERR_CODING_BAD_ENCODING = 29097,
  NZERROR_CIC_ERR_CODING_BAD_PEM = 29070,
  NZERROR_CIC_ERR_CODING_BASE = 29071,
  NZERROR_CIC_ERR_DER_BAD_ENCODING = 29072,
  NZERROR_CIC_ERR_DER_BAD_ENCODING_LENGTH = 29073,
  NZERROR_CIC_ERR_DER_BASE = 29074,
  NZERROR_CIC_ERR_DER_ELEMENT_TOO_LONG = 29075,
  NZERROR_CIC_ERR_DER_INDEFINITE_LENGTH = 29076,
  NZERROR_CIC_ERR_DER_NO_MORE_ELEMENTS = 29077,
  NZERROR_CIC_ERR_DER_OBJECT_TOO_LONG = 29078,
  NZERROR_CIC_ERR_DER_TAG_SIZE = 29079,
  NZERROR_CIC_ERR_DER_TIME_OUT_OF_RANGE = 29080,
  NZERROR_CIC_ERR_DER_UNUSED_BITS_IN_BIT_STR = 29081,
  NZERROR_CIC_ERR_GENERAL_BASE = 29082,
  NZERROR_CIC_ERR_HASH_BASE = 29083,
  NZERROR_CIC_ERR_ILLEGAL_PARAM = 29084,
  NZERROR_CIC_ERR_MEM_NOT_OURS = 29085,
  NZERROR_CIC_ERR_MEM_OVERRUN = 29086,
  NZERROR_CIC_ERR_MEM_UNDERRUN = 29087,
  NZERROR_CIC_ERR_MEM_WAS_FREED = 29088,
  NZERROR_CIC_ERR_NO_PTR = 29091,
  NZERROR_CIC_ERR_NOT_FOUND = 29090,
  NZERROR_CIC_ERR_PKCS_AUTH_FAILED = 29054,
  NZERROR_CIC_ERR_PKCS_BAD_CONTENT_TYPE = 29055,
  NZERROR_CIC_ERR_PKCS_BAD_INPUT = 29056,
  NZERROR_CIC_ERR_PKCS_BAD_PADDING = 29057,
  NZERROR_CIC_ERR_PKCS_BAD_SN = 29058,
  NZERROR_CIC_ERR_PKCS_BAD_SnLen = 29059,
  NZERROR_CIC_ERR_PKCS_BAD_VERSION = 29060,
  NZERROR_CIC_ERR_PKCS_BASE = 29061,
  NZERROR_CIC_ERR_PKCS_FIELD_NOT_PRESENT = 29062,
  NZERROR_CIC_ERR_PKCS_NEED_CERTVAL = 29063,
  NZERROR_CIC_ERR_PKCS_NEED_PASSWORD = 29064,
  NZERROR_CIC_ERR_PKCS_NEED_PKC = 29065,
  NZERROR_CIC_ERR_PKCS_NEED_PRV_KEY = 29066,
  NZERROR_CIC_ERR_PKCS_NEED_TRUSTED = 29067,
  NZERROR_CIC_ERR_PKCS_UNSUP_PRVKEY_TYPE = 29069,
  NZERROR_CIC_ERR_PKCS_UNSUPPORTED_CERT_FORMAT = 29068,
  NZERROR_CIC_ERR_PKCS12_MISSING_ALG = 29053,
  NZERROR_CIC_ERR_RANDOM = 29135,
  NZERROR_CIC_ERR_SMALL_BUFFER = 29136,
  NZERROR_CIC_ERR_SSL_ALERT_CB_FAILURE = 29004,
  NZERROR_CIC_ERR_SSL_BAD_CERTIFICATE = 29005,
  NZERROR_CIC_ERR_SSL_BAD_CERTIFICATE_REQUEST = 29006,
  NZERROR_CIC_ERR_SSL_BAD_CLEAR_KEY_LEN = 29007,
  NZERROR_CIC_ERR_SSL_BAD_CONTEXT = 29137,
  NZERROR_CIC_ERR_SSL_BAD_DHPARAM_KEY_LENGTH = 29008,
  NZERROR_CIC_ERR_SSL_BAD_ENCRYPTED_KEY_LEN = 29009,
  NZERROR_CIC_ERR_SSL_BAD_EXPORT_KEY_LENGTH = 29010,
  NZERROR_CIC_ERR_SSL_BAD_FINISHED_MESSAGE = 29011,
  NZERROR_CIC_ERR_SSL_BAD_KEY_ARG_LEN = 29012,
  NZERROR_CIC_ERR_SSL_BAD_MAC = 29013,
  NZERROR_CIC_ERR_SSL_BAD_MAX_FRAGMENT_LENGTH_EXTENSION = 29014,
  NZERROR_CIC_ERR_SSL_BAD_MESSAGE_LENGTH = 29015,
  NZERROR_CIC_ERR_SSL_BAD_PKCS1_PADDING = 29016,
  NZERROR_CIC_ERR_SSL_BAD_PREMASTER_SECRET_LENGTH = 29017,
  NZERROR_CIC_ERR_SSL_BAD_PREMASTER_SECRET_VERSION = 29018,
  NZERROR_CIC_ERR_SSL_BAD_PROTOCOL_VERSION = 29019,
  NZERROR_CIC_ERR_SSL_BAD_RECORD_LENGTH = 29020,
  NZERROR_CIC_ERR_SSL_BAD_SECRET_KEY_LEN = 29021,
  NZERROR_CIC_ERR_SSL_BAD_SIDE = 29022,
  NZERROR_CIC_ERR_SSL_BUFFERS_NOT_EMPTY = 29023,
  NZERROR_CIC_ERR_SSL_CERT_CHECK_CALLBACK = 29025,
  NZERROR_CIC_ERR_SSL_CERTIFICATE_VALIDATE_FAILED = 29024,
  NZERROR_CIC_ERR_SSL_DECRYPT_FAILED = 29026,
  NZERROR_CIC_ERR_SSL_ENTROPY_COLLECTION = 29027,
  NZERROR_CIC_ERR_SSL_FAIL_SERVER_VERIFY = 29028,
  NZERROR_CIC_ERR_SSL_HANDSHAKE_ALREADY_COMPLETED = 29029,
  NZERROR_CIC_ERR_SSL_HANDSHAKE_REQUESTED = 29030,
  NZERROR_CIC_ERR_SSL_HANDSHAKE_REQUIRED = 29031,
  NZERROR_CIC_ERR_SSL_INCOMPLETE_IDENTITY = 29032,
  NZERROR_CIC_ERR_SSL_INVALID_PFX = 29033,
  NZERROR_CIC_ERR_SSL_NEEDS_CIPHER_OR_CLIENTAUTH = 29034,
  NZERROR_CIC_ERR_SSL_NEEDS_PRNG = 29035,
  NZERROR_CIC_ERR_SSL_NO_CERTIFICATE = 29037,
  NZERROR_CIC_ERR_SSL_NO_CLIENT_AUTH_MODES = 29098,
  NZERROR_CIC_ERR_SSL_NO_MATCHING_CERTIFICATES = 29038,
  NZERROR_CIC_ERR_SSL_NO_MATCHING_CIPHER_SUITES = 29039,
  NZERROR_CIC_ERR_SSL_NO_SUPPORTED_CIPHER_SUITES = 29040,
  NZERROR_CIC_ERR_SSL_NOT_SUPPORTED = 29036,
  NZERROR_CIC_ERR_SSL_NULL_CB = 29041,
  NZERROR_CIC_ERR_SSL_READ_BUFFER_NOT_EMPTY = 29042,
  NZERROR_CIC_ERR_SSL_READ_REQUIRED = 29043,
  NZERROR_CIC_ERR_SSL_RENEGOTIATION_ALREADY_REQUESTED = 29044,
  NZERROR_CIC_ERR_SSL_RENEGOTIATION_REFUSED = 29045,
  NZERROR_CIC_ERR_SSL_RESUMABLE_SESSION = 29046,
  NZERROR_CIC_ERR_SSL_TLS_EXTENSION_MISMATCH = 29047,
  NZERROR_CIC_ERR_SSL_UNEXPECTED_MSG = 29048,
  NZERROR_CIC_ERR_SSL_UNKNOWN_RECORD = 29049,
  NZERROR_CIC_ERR_SSL_UNSUPPORTED_CLIENT_AUTH_MODE = 29050,
  NZERROR_CIC_ERR_SSL_UNSUPPORTED_PUBKEY_TYPE = 29051,
  NZERROR_CIC_ERR_SSL_WRITE_BUFFER_NOT_EMPTY = 29052,
  NZERROR_CIC_ERR_TIMEOUT = 29092,
  NZERROR_CIC_ERR_UNIT_MASK = 29093,
  NZERROR_CKEYPAIR_SELF_TEST_FAILED = 29155,
  NZERROR_CREATE_KEY_OBJ_ERROR = 43051,
  NZERROR_CREATE_PKCS10_OBJECT = 43053,
  NZERROR_CRL_EXPIRED = 29195,
  NZERROR_CRL_NOT_IN_CACHE = 29194,
  NZERROR_CRL_SIG_VERIFY_FAILED = 29176,
  NZERROR_CRLDP_NO_CRL_FOUND = 29193,
  NZERROR_CRNG_SELF_TEST_FAILED = 29156,
  NZERROR_DAC_SELF_TEST_FAILED = 29161,
  NZERROR_DATA_SOURCE_INIT_FAILED = 28752,
  NZERROR_DATA_SOURCE_TERM_FAILED = 28753,
  NZERROR_DECRYPT_FAILED = 28786,
  NZERROR_DES_SELF_TEST_FAILED = 29150,
  NZERROR_DN_MATCH  =            29222,
  NZERROR_DRNG_SELF_TEST_FAILED = 29154,
  NZERROR_ELEMENT_ADD_FAILED = 28762,
  NZERROR_ENCRYPT_FAILED = 28787,
  NZERROR_EntrustGetInfoFailed = 28891,
  NZERROR_EntrustGetNameFailed = 28893,
  NZERROR_EntrustLoadCertificateFailed = 28892,
  NZERROR_EntrustLoginFailed = 28890,
  NZERROR_EXTERNAL_PKCS12_NOT_SUPPORTED_FIPS_MODE = 29164,
  NZERROR_FILE_BAD_PERMISSION = 28842,
  NZERROR_FILE_CLOSE_FAILED = 28775,
  NZERROR_FILE_NAME_TRANSLATION = 28784,
  NZERROR_FILE_OPEN_FAILED = 28759,
  NZERROR_FILE_OSD_ERROR = 28843,
  NZERROR_FILE_WRITE_FAILED = 28774,
  NZERROR_FIPS_LIB_DIFFERS = 29160,
  NZERROR_FIPS_LIB_OPEN_FAILED = 29158,
  NZERROR_FIPS_LIB_READ_ERROR = 29159,
  NZERROR_FIPS_PATHNAME_ERROR = 29157,
  NZERROR_GENERIC = 28750,
  NZERROR_GET_CERT_FIELDS = 43052,
  NZERROR_GET_PKCS10_DER = 43056,
  NZERROR_ImproperClientCredentials = 28878,
  NZERROR_ImproperServerCredentials = 28877,
  NZERROR_INITIALIZATION_FAILED   =  28853,
  NZERROR_INITIALIZE_CERTC = 43057,
  NZERROR_INSERT_PRIVATE_KEY = 43058,
  NZERROR_INVALID_CERT_STATUS_PROTOCOL = 29180,
  NZERROR_INVALID_HEADER_LENGTH = 43070,
  NZERROR_INVALID_INPUT = 28788,
  NZERROR_INVALID_KEY_DATA_TYPE = 28797,
  NZERROR_INVALID_PARAMETER = 28783,
  NZERROR_KD_CREATE = 28810,
  NZERROR_LAST_ERROR = 43499,
  NZERROR_LDAP_BIND_FAILED = 29182,
  NZERROR_LDAP_COUNT_ENTRIES_FAILED = 29188,
  NZERROR_LDAP_FIRSTATTR_FAILED = 29185,
  NZERROR_LDAP_GETVALUESLEN_FAILED = 29186,
  NZERROR_LDAP_MULTIPLE_ENTRIES_FOUND = 29190,
  NZERROR_LDAP_NO_ENTRY_FOUND = 29189,
  NZERROR_LDAP_OPEN_FAILED = 29181,
  NZERROR_LDAP_RESULT_FAILED = 29184,
  NZERROR_LDAP_SEARCH_FAILED = 29183,
  NZERROR_LDAP_UNSUPPORTED_VALMEC = 29187,
  NZERROR_LDAP_VALMEC_NOT_SET = 29192,
  NZERROR_LIST_CREATION_FAILED = 28760,
  NZERROR_LOCKEYID_CREATE_FAILED = 29100,
  NZERROR_MEMORY_ALLOC_0_BYTES = 28757,
  NZERROR_MEMORY_ALLOC_FAILED = 28756,
  NZERROR_MEMORY_FREE_FAILED = 28758,
  NZERROR_METHOD_NOT_FOUND = 28769,
  NZERROR_MUTEX_DESTROY_FAILED = 29139,
  NZERROR_MUTEX_INITIALIZE_FAILED = 29138,
  NZERROR_NAME_TYPE_NOT_FOUND = 28789,
  NZERROR_NLS_STRING_OPEN_FAILED = 28790,
  NZERROR_NO_CERTIFICATE_ALERT    = 28845,
  NZERROR_NO_CLEAR_PRIVATE_KEY_FILE   = 28847,
  NZERROR_NO_CLEAR_TRUSTPOINT_FILE    = 28850,
  NZERROR_NO_ELEMENT = 28761,
  NZERROR_NO_ENCRYPTED_PRIVATE_KEY_FILE = 28848,
  NZERROR_NO_ENCRYPTED_TRUSTPOINT_FILE = 28851,
  NZERROR_NO_LIST = 28765,
  NZERROR_NO_MATCHING_CERT_REQ = 29224,
  NZERROR_NO_MATCHING_PRIVATE_KEY = 29226,
  NZERROR_NO_MEMORY = 28751,
  NZERROR_NO_PRIVATE_KEY          = 28846,
  NZERROR_NO_SUCH_PARAMETER = 28785,
  NZERROR_NO_TRUSTPOINTS     = 28849,
  NZERROR_NO_WALLET  = 28844,
  NZERROR_NoKeyPairForKeyUsage = 28885,
  NZERROR_NONFIPS_CIPHERSUITE = 29162,
  NZERROR_NoProtocolSideSet = 28879,
  NZERROR_NOT_ALLOCATED = 28782,
  NZERROR_NOT_INITIALIZED = 28771,
  NZERROR_OBJECT_GET_FAILED = 28755,
  NZERROR_OBJECT_STORE_FAILED = 28754,
  NZERROR_OCI_BIND_FAILED = 28793,
  NZERROR_OCI_PLSQL_FAILED = 28792,
  NZERROR_OID_INFO_NOT_SET = 29191,
  NZERROR_OK = 0,
  NZERROR_OUTPUT_BUFFER_TOO_SMALL = 28776,
  NZERROR_P12_ADD_ATTRIB_FAILED = 29117,
  NZERROR_P12_ADD_CERT_FAILED = 29102,
  NZERROR_P12_ADD_CERTREQ_FAILED = 29104,
  NZERROR_P12_ADD_PKCS11INFO_FAILED = 29122,
  NZERROR_P12_ADD_PVTKEY_FAILED = 29101,
  NZERROR_P12_ADD_SECRET_FAILED = 29121,
  NZERROR_P12_CREATE_FAILED = 29107,
  NZERROR_P12_CRT_ATTRIB_FAILED = 29118,
  NZERROR_P12_DEST_FAILED = 29107,
  NZERROR_P12_EXP_CERT_FAILED = 29120,
  NZERROR_P12_EXP_PVTKEY_FAILED = 29115,
  NZERROR_P12_GET_ATTRIB_FAILED = 29116,
  NZERROR_P12_GET_CERT_FAILED = 29112,
  NZERROR_P12_GET_PKCS11INFO_FAILED = 29123,
  NZERROR_P12_GET_PVTKEY_FAILED = 29113,
  NZERROR_P12_IMP_CERT_FAILED = 29119,
  NZERROR_P12_IMP_PVTKEY_FAILED = 29114,
  NZERROR_P12_INVALID_BAG = 29110,
  NZERROR_P12_INVALID_INDEX = 29111,
  NZERROR_P12_MULTIPLE_PKCS11_CERTLABEL = 29131,
  NZERROR_P12_MULTIPLE_PKCS11_LIBNAME = 29124,
  NZERROR_P12_MULTIPLE_PKCS11_TOKENLABEL = 29125,
  NZERROR_P12_MULTIPLE_PKCS11_TOKENPASSPHRASE = 29126,
  NZERROR_P12_PKCS11_LIBNAME_NOT_SET = 29128,
  NZERROR_P12_PKCS11_TOKENLABEL_NOT_SET = 29129,
  NZERROR_P12_PKCS11_TOKENPASSPHRASE_NOT_SET = 29130,
  NZERROR_P12_PVTKEY_CRT_FAILED = 29109,
  NZERROR_P12_RAND_ERROR = 29108,
  NZERROR_P12_UNKNOWN_PKCS11INFO = 29127,
  NZERROR_P12_WLT_CREATE_FAILED = 29103,
  NZERROR_P12_WLT_EXP_FAILED = 29105,
  NZERROR_P12_WLT_IMP_FAILED = 29106,
  NZERROR_PARAMETER_BAD_TYPE = 28763,
  NZERROR_PARAMETER_MALFORMED = 28778,
  NZERROR_PARAMETER_NO_DATA = 28781,
  NZERROR_PARAMETER_NO_METHOD = 28779,
  NZERROR_PARAMETER_RETRIEVAL = 28764,
  NZERROR_PKCS11_BAD_PASSPHRASE = 43002,
  NZERROR_PKCS11_CERT_WITH_LABEL_NOT_USER_CERT  = 43021,
  NZERROR_PKCS11_CHANGE_PROVIDERS_ERROR = 43013,
  NZERROR_PKCS11_CREATE_KEYPAIR_ERROR = 43015,
  NZERROR_PKCS11_GET_FUNC_LIST = 43003,
  NZERROR_PKCS11_GET_PRIVATE_KEY_ERROR = 43014,
  NZERROR_PKCS11_GET_SLOT_LIST = 43006,
  NZERROR_PKCS11_GET_TOKEN_INFO = 43008,
  NZERROR_PKCS11_INITIALIZE = 43004,
  NZERROR_PKCS11_LIBRARY_NOT_FOUND = 43000,
  NZERROR_PKCS11_MULTIPLE_CERTS_ON_TOKEN_WITH_GIVEN_LABEL = 43020,
  NZERROR_PKCS11_NO_CERT_ON_TOKEN = 43017,
  NZERROR_PKCS11_NO_CERT_ON_TOKEN_WITH_GIVEN_LABEL = 43019,
  NZERROR_PKCS11_NO_TOKENS_PRESENT = 43005,
  NZERROR_PKCS11_NO_USER_CERT_ON_TOKEN = 43018,
  NZERROR_PKCS11_SYMBOL_NOT_FOUND = 43009,
  NZERROR_PKCS11_TOKEN_LOGIN_FAILED = 43011,
  NZERROR_PKCS11_TOKEN_NOT_FOUND = 43001,
  NZERROR_PKCS11_WALLET_CONTAINS_P11_INFO = 43016,
  NZERROR_PLSQL_ORACLE_TO_REAL = 28814,
  NZERROR_PLSQL_REAL_TO_ORACLE = 28815,
  NZERROR_RIO_ACCESS_DESCRIPTOR = 28811,
  NZERROR_RIO_CLOSE = 28804,
  NZERROR_RIO_DELETE = 28809,
  NZERROR_RIO_INFO = 28808,
  NZERROR_RIO_IO = 28803,
  NZERROR_RIO_MODE = 28802,
  NZERROR_RIO_OBJECT_TYPE = 28801,
  NZERROR_RIO_OPEN = 28800,
  NZERROR_RIO_RECORD = 28812,
  NZERROR_RIO_RECORD_TYPE = 28813,
  NZERROR_RIO_RETRIEVE = 28805,
  NZERROR_RIO_STORE = 28806,
  NZERROR_RIO_UPDATE = 28807,
  NZERROR_RSA_ERROR = 43059,
  NZERROR_RSA_SELF_TEST_FAILED = 29153,
  NZERROR_ServerDNMisConfigured = 29003,
  NZERROR_ServerDNMisMatched = 29002,
  NZERROR_SET_PKCS10_FIELDS = 43054,
  NZERROR_setCertFailed = 28881,
  NZERROR_setPersonaFailed = 28880,
  NZERROR_setTPFailed = 28883,
  NZERROR_setVKeyFailed = 28882,
  NZERROR_SHA_SELF_TEST_FAILED = 29152,
  NZERROR_SIGN_CERT_REQUEST = 43055,
  NZERROR_SLTSCTX_INIT_FAILED = 43060,
  NZERROR_SLTSCTX_TERM_FAILED = 43062,
  NZERROR_SLTSKYC_FAILED = 43061,
  NZERROR_SLTSKYS_FAILED = 43063,
  NZERROR_SSLBadParameterErr = 28873,
  NZERROR_SSLConnectionClosedError = 28865,
  NZERROR_SSLConnectionClosedGraceful = 28864,
  NZERROR_SSLFatalAlert = 28860,
  NZERROR_SSLIOClosedOverrideGoodbyeKiss = 28874,
  NZERROR_SSLIOErr = 28862,
  NZERROR_SSLMemoryErr = 28854,
  NZERROR_SSLNegotiationErr = 28859,
  NZERROR_SSLOverflowErr = 28856,
  NZERROR_SSLProtocolErr = 28858,
  NZERROR_SSLSessionNotFoundErr = 28863,
  NZERROR_SSLUnknownErr = 28857,
  NZERROR_SSLUnsupportedErr = 28855,
  NZERROR_SSLWouldBlockErr = 28861,
  NZERROR_TERMINATE_FAIL = 28766,
  NZERROR_THIS_MUST_BE_LAST,
  NZERROR_TK_ALGORITHM = 29240,
  NZERROR_TK_BAD_HEADER_LENGTH = 29230,
  NZERROR_TK_BAD_MAGIC_NUMBER = 29229,
  NZERROR_TK_BYTES_NEEDED = 29228,
  NZERROR_TK_CANNOT_GROW = 29241,
  NZERROR_TK_CE_ALGAPPLY = 29238,
  NZERROR_TK_CE_ALGFINISH = 29237,
  NZERROR_TK_CE_ALGINIT = 29239,
  NZERROR_TK_CE_DECODE_KEY = 29234,
  NZERROR_TK_CE_ENCODE_KEY = 29233,
  NZERROR_TK_CE_FUNC = 29245,
  NZERROR_TK_CE_GEYKEYINFO = 29235,
  NZERROR_TK_CE_INIT = 29231,
  NZERROR_TK_CE_KEYINIT = 29232,
  NZERROR_TK_CE_STATE = 29249,
  NZERROR_TK_INV_CIPHR_TYPE = 28827,
  NZERROR_TK_INV_DATA_FMT = 28832,
  NZERROR_TK_INV_ENG_FUNC = 28826,
  NZERROR_TK_INV_IDENT_TYPE = 28828,
  NZERROR_TK_INVALID_STATE = 28824,
  NZERROR_TK_KEYSIZE = 29242,
  NZERROR_TK_KEYTYPE = 29243,
  NZERROR_TK_NOTOPEN = 29247,
  NZERROR_TK_PLSQL_BUF_TOO_SMALL = 28834,
  NZERROR_TK_PLSQL_GENERIC = 28817,
  NZERROR_TK_PLSQL_INSUFF_INFO = 28833,
  NZERROR_TK_PLSQL_INV_IDENT_DESC = 28835,
  NZERROR_TK_PLSQL_NO_BYTES = 28823,
  NZERROR_TK_PLSQL_NO_CIPHER_TYPE = 28829,
  NZERROR_TK_PLSQL_NO_CONTEXT = 28818,
  NZERROR_TK_PLSQL_NO_DATA_FMT = 28831,
  NZERROR_TK_PLSQL_NO_DIST_NAME = 28819,
  NZERROR_TK_PLSQL_NO_ENG_FUNC = 28825,
  NZERROR_TK_PLSQL_NO_IDENT_TYPE = 28830,
  NZERROR_TK_PLSQL_NO_IDENTITY = 28838,
  NZERROR_TK_PLSQL_NO_INPUT = 28821,
  NZERROR_TK_PLSQL_NO_PASSWORD = 28816,
  NZERROR_TK_PLSQL_NO_PERSONA = 28839,
  NZERROR_TK_PLSQL_NO_SEED = 28822,
  NZERROR_TK_PLSQL_NO_STATE = 28820,
  NZERROR_TK_PLSQL_NO_WALLET = 28837,
  NZERROR_TK_PLSQL_NO_WRL = 29244,
  NZERROR_TK_PLSQL_WALLET_NOTOPEN = 28836,
  NZERROR_TK_PLSQL_WALLET_OPEN = 28840,
  NZERROR_TK_SEED_RANDOM = 29236,
  NZERROR_TK_TDU_FORMAT = 29246,
  NZERROR_TK_WRLTYPE = 29248,
  NZERROR_UNSUPPORTED = 28841,
  NZERROR_UNSUPPORTED_METHOD = 28796,
  NZERROR_VALIDITY_EXPIRED = 29227,
  NZERROR_VENDOR_NOT_SUPPORTED_FIPS_MODE = 29163,
  NZERROR_WALLET_CONTAINS_USER_CREDENTIALS = 43071,
  NZERROR_WRITE_MAGIC_VERSION = 28773,
  NZERROR_X509CertChainIncompleteErr = 28871,
  NZERROR_X509CertChainInvalidErr = 28868,
  NZERROR_X509CertExpiredErr = 28869,
  NZERROR_X509DataNotFoundErr = 28872,
  NZERROR_X509IESGCErr       =  28876,
  NZERROR_X509MozillaSGCErr  =  28875,
  NZERROR_X509NamesNotEqualErr = 28870
} nzerror;

enum nzttwrl
{
   NZTTWRL_DEFAULT = 1,
   NZTTWRL_SQLNET,
   NZTTWRL_FILE,
   NZTTWRL_ENTR,
   NZTTWRL_MCS,
   NZTTWRL_ORACLE,
   NZTTWRL_NULL,
   NZTTWRL_REGISTRY
};
typedef enum nzttwrl nzttwrl;

//processing flags
enum nzttces
{
   NZTTCES_CONTINUE = 1,
   NZTTCES_END,
   NZTTCES_RESET
};
typedef enum nzttces nzttces;

enum nzttcef
{
   NZTTCEF_DETACHEDSIGNATURE = 1,
   NZTTCEF_SIGNATURE,
   NZTTCEF_ENVELOPING,
   NZTTCEF_PKENCRYPTION,
   NZTTCEF_ENCRYPTION,
   NZTTCEF_KEYEDHASH,
   NZTTCEF_HASH,
   NZTTCEF_RANDOM,

   NZTTCEF_LAST
};
typedef enum nzttcef nzttcef;

enum nzttState
{
   NZTTSTATE_EMPTY = 0,
   NZTTSTATE_REQUESTED,
   NZTTSTATE_REAday,
   NZTTSTATE_INVALID,
   NZTTSTATE_RENEWAL
};
typedef enum nzttState nzttState;

enum nzttVersion
{
   NZTTVERSION_X509v1 = 1,
   NZTTVERSION_X509v3,
#ifdef NZDEPRECATED
   NZTTVERSION_SYMMETRIC,         /* Symmetric */
#endif
   NZTTVERSION_INVALID_TYPE
};
typedef enum nzttVersion nzttVersion;

enum nzttCipherType
{
   NZTTCIPHERTYPE_RSA = 1,
   NZTTCIPHERTYPE_DES,
   NZTTCIPHERTYPE_RC4,
   NZTTCIPHERTYPE_MD5DES,
   NZTTCIPHERTYPE_MD5RC2,
   NZTTCIPHERTYPE_MD5,
   NZTTCIPHERTYPE_SHA
};
typedef enum nzttCipherType nzttCipherType;

enum nztttdufmt
{
   NZTTTDUFMT_PKCS7 = 1,
   NZTTTDUFMT_RSAPAD,
   NZTTTDUFMT_ORACLEv1,
   NZTTTDUFMT_LAST
};
typedef enum nztttdufmt nztttdufmt;

enum nzttValState
{
   NZTTVALSTATE_NONE = 1,
   NZTTVALSTATE_GOOD,
   NZTTVALSTATE_REVOKED
};
typedef enum nzttValState nzttValState;

enum nzttPolicy
{
   NZTTPOLICY_NONE = 0,
   NZTTPOLICY_RETRY_1,
   NZTTPOLICY_RETRY_2,
   NZTTPOLICY_RETRY_3
};
typedef enum nzttPolicy nzttPolicy;

#ifdef NZDEPRECATED_MULTIPLECERTS
enum nzttUsage
{
   NZTTUSAGE_NONE = 0,
   NZTTUSAGE_SSL  /* persona for SSL usage */
};
typedef enum nzttUsage nzttUsage;
#endif


typedef ub1 nzttID[16];

enum nzttIdentType
{
   NZTTIDENTITYTYPE_INVALID_TYPE = 0,
   NZTTIDENTITYTYPE_CERTIFICTAE,
   NZTTIDENTITYTYPE_CERT_REQ,
   NZTTIDENTITYTYPE_RENEW_CERT_REQ,
   NZTTIDENTITYTYPE_CLEAR_ETP,
   NZTTIDENTITYTYPE_CLEAR_UTP,
   NZTTIDENTITYTYPE_CLEAR_PTP
};
typedef enum nzttIdentType nzttIdentType;

struct nzttBufferBlock
{
# define NZT_NO_AUTO_REALLOC     0x1

   uword flagsNzttBufferBlock;
   ub4 bufLenNzttBufferBlock;
   ub4 usedlenNzttBufferBlock;
   ub1 *bufferNzttBufferBlock;
};
typedef struct nzttBufferBlock nzttBufferBlock;

struct nzttWallet
{
   ub1 *pLdapNameNzttWallet;
   ub4  ldapNamelenNzttWallet;
   nzttPolicy securePolicyNzttWallet;
   nzttPolicy openPolicyNzttWallet;
   nzttPersona *pPersonaNzttWallet;
   nzttWalletPrivate *pRivateNzttWallet;
#ifdef NZDEPRECATED
   ub4 npersonaNzttWallet;            /* Number of personas */
#endif
};

struct nzttPersona
{
   ub1 *genericNameNzttPersona;
   ub4  genericNamelenNzttPersona;
   nzttPersonaPrivate *pRivateNzttPersona;
   nzttIdentity *pMycertreqsNzttPersona;
   nzttIdentity *pMycertsNzttPersona;
   nzttIdentity *pMytpsNzttPersona;
   nzssEntry *pMystoreNzttPersona;
   nzpkcs11_Info *pMypkcs11InfoNzttPersona;
   struct nzttPersona *pNextNzttPersona;
#ifdef NZDEPRECATED_MULTIPLECERTS
   nzttUsage usageNzttPersona;
   nzttState stateNzttPersona;
   ub4 ntpsNzttPersona;
#endif
};

struct nzttIdentity
{
   text *dnNzttIdentity;
   ub4 dnlenNzttIdentity;
   text *pCommentNzttIdentity;
   ub4 commentlenNzttIdentity;
   nzttIdentityPrivate *pRivateNzttIdentity;
   nzttIdentity *pNextNzttIdentity;
};

struct nzttB64Cert
{
   ub1 *b64CertNzttB64Cert;
   ub4  b64CertlenNzttB64Cert;
   struct nzttB64Cert *pNextNzttB64Cert;
};
typedef struct nzttB64Cert nzttB64Cert;


struct nzttPKCS7ProtInfo
{
   nzttCipherType mictypeNzttPKCS7ProtInfo;
   nzttCipherType symmtypeNzttPKCS7ProtInfo;
   ub4 keylenNzttPKCS7ProtInfo;
};
typedef struct nzttPKCS7ProtInfo nzttPKCS7ProtInfo;

union nzttProtInfo
{
   nzttPKCS7ProtInfo pkcs7NzttProtInfo;
};
typedef union nzttProtInfo nzttProtInfo;

struct nzttPersonaDesc
{
   ub4 privlenNzttPersonaDesc;
   ub1 *pRivNzttPersonaDesc;
   ub4 prllenNzttPersonaDesc;
   text *pRlNzttPersonaDesc;
   ub4 aliaslenNzttPersonaDesc;
   text *aliasNzttPersonaDesc;
   ub4 longlenNzttPersonaDesc;
   text *pLongNzttPersonaDesc;
};
typedef struct nzttPersonaDesc nzttPersonaDesc;

struct nzttIdentityDesc
{
   ub4 publenNzttIdentityDesc;
   ub1 *pPubNzttIdentityDesc;
   ub4 dnlenNzttIdentityDesc;
   text *dnNzttIdentityDesc;
   ub4 longlenNzttIdentityDesc;
   text *pLongNzttIdentityDesc;
   ub4 quallenNzttIdentityDesc;
   text *pTrustqualNzttIdentityDesc;
};
typedef struct nzttIdentityDesc nzttIdentityDesc;

#ifdef NZ_OLD_TOOLS

nzerror nztidGetIdentityDesc(/*IN*/nzctx *pOssContext,
                             /*IN*/nzttIdentity *pIdentity,
                             /*IN/OUT*/nzttIdentityDesc **pIdentityDescription);

nzerror nztidFreeIdentityDesc(/*IN*/nzctx *pOssContext,
                              /*IN/OUT*/nzttIdentityDesc **pIdentityDescription);
#endif /* NZ_OLD_TOOLS */

////////////////////////////////////////////////////////////////
///////  C O L L E C T I O N S     D E F I N I T I O N S
//////////////////////////////////////////////////////////////////////////////
//////////// Dynamic Callback Function Pointers
typedef void * (*OCICallbackMalloc)( void * ctxp, size_t size );
typedef void * (*OCICallbackRaloc)( void * ctxp, void * memp, size_t newsize );
typedef void (*OCICallbackFree)( void * ctxp, void * memptr );
typedef sb4 (*OCICallbackInBind)(void  *pIctxp, OCIBind *pBindp, ub4 iter,
                                  ub4 index, void  **pBufpp, ub4 *pAlenp,
                                  ub1 *pPiecep, void  **pIndp);

typedef sb4 (*OCICallbackOutBind)(void  *pOctxp, OCIBind *pBindp, ub4 iter,
                                 ub4 index, void  **pBufpp, ub4 **pAlenp,
                                 ub1 *pPiecep, void  **pIndp,
                                 ub2 **pRcodep);

typedef sb4 (*OCICallbackDefine)(void  *pOctxp, OCIDefine *pDefnp, ub4 iter,
                                 void  **pBufpp, ub4 **pAlenp, ub1 *pPiecep,
                                 void  **pIndp, ub2 **pRcodep);

typedef sword (*OCIUserCallback)(void  *pCtxp, void  *pHndlp, ub4 type,
                                 ub4 fcode, ub4 when, sword returnCode,
                                 sb4 *pErrnop, va_list arglist);

typedef sword (*OCIEnvCallbackType)(/*IN/OUT*/OCIEnv *pEnvironment, ub4 mode,
                                   size_t xtramemSz, void  *pUsrmemp,
                                   OCIUcb *pUcbDesc);

typedef sb4 (*OCICallbackLobRead)(void  *pCtxp, const void  *pBufp,
                                             ub4 Len, ub1 piece);

typedef sb4 (*OCICallbackLobWrite)(void  *pCtxp, void  *pBufp,
                                          ub4 *pLenp, ub1 *pPiece);


typedef sb4 (*OCICallbackLobRead2)(void  *pCtxp, const void  *pBufp, oraub8 Len,
                                   ub1 piece, void  **pChangedbUfpp,
                                   oraub8 *pChangedLenp);

typedef sb4 (*OCICallbackLobWrite2)(void  *pCtxp, void  *pBufp, oraub8 *pLenp,
                                    ub1 *pPiece, void  **pChangedbUfpp,
                                    oraub8 *pChangedLenp);

typedef sb4 (*OCICallbackLobArrayRead)(void  *pCtxp, ub4 arrayIter,
                                       const void  *pBufp, oraub8 Len,
                                       ub1 piece, void  **pChangedbUfpp,
                                       oraub8 *pChangedLenp);

typedef sb4 (*OCICallbackLobArrayWrite)(void  *pCtxp, ub4 arrayIter,
                                        void  *pBufp, oraub8 *pLenp,
                                        ub1 *pPiece, void  **pChangedbUfpp,
                                        oraub8 *pChangedLenp);

typedef sb4 (*OCICallbackLobGetDeduplicateRegions)(void *pCtxp,
                                              OCILobRegion *pRegions,
                                              ub4 count, ub1 piece,
                                              OCILobRegion **pChangedReg,
                                              ub4 *pChangedCount);

typedef sb4 (*OCICallbackAQEnq)(void  *pCtxp, void  **pPayload,
                                void  **pPayloadInd);

typedef sb4 (*OCICallbackAQEnqStreaming)(void  *pCtxp, void  **pPayload,
                                         void  **pPayloadInd,
                                         OCIAQMsgProperties  **pMsgprop,
                                         OCIType **pPinnedType);

typedef sb4 (*OCICallbackAQDeq)(void  *pCtxp,
                                void  **pPayload,
                                void  **pPayloadInd);


///////////////////////////////////////////////////////////////////////////////
/////////////////////////Failover Callback Structure //////////////////////////
typedef sb4 (*OCICallbackFailover)(void  *pSvcctx, void  *pEnvctx,
                                   void  *pFoCtx, ub4 foType,
                                   ub4 foEvent);

typedef struct
{
  OCICallbackFailover callback_function;
  void  *fo_ctx;
}
OCIFocbkStruct;

typedef sword (*OCICallbackStmtCache)(void *pCtx, OCIStmt *pStmt, ub4 mode);

typedef void (*OCIEventCallback)(void  *pEvtctx, OCIEvent *pEventhp);



//////////////   Row Callback Interfaces
typedef sword (*OCIBindRowCallback)(void  *pCtx);
typedef sword (*OCIFetchRowCallback)(void  *pCtx);


////////  Client Notification
typedef ub4 (*OCISubscriptionNotify)(void  *pCtx,
                                     OCISubscription *pSubscrhp,
                                     void  *pPay,
                                     ub4 payl,
                                     void  *pDesc,
                                     ub4 mode);

typedef enum
{
  XCTXINIT_OCIDUR  = 1,
  XCTXINIT_ERRHDL  = 2
} ocixmldbpname;

typedef struct ocixmldbparam
{
  ocixmldbpname   name_ocixmldbparam;
  void           *pValue_ocixmldbparam;
} ocixmldbparam;




////////////////////////////////////////////////////////////////////////////////////////////////////////
///////   F U N C T I O N S        D E F I N I T I O N   ///////////////////////////////////////////////

sword OCITypeByName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
    /*IN*/const OCISvcCtx *pServiceContext,
    /*IN*/const oratext *pSchemaName,
    /*IN*/ub4 sLength,
    /*IN*/const oratext *pTypeName,
    /*IN*/ub4 tLength,
    /*IN*/const oratext *pVersionName,
    /*IN*/ub4 vLen,
    /*IN*/OCIDuration pinDur,
    /*IN*/OCITypeGetOpt getOpt,
    /*OUT*/OCIType **pPinnedType);

sword OCITypeMethodByName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OCIType *pPinnedType,
                          /*IN*/const oratext *pMethodName,
                          /*IN*/ub4 mLen,
                          /*OUT*/OCITypeMethod **pMethodsType);

sword OCITypeMethodNext(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN/OUT*/OCITypeIter *pIterNextMethod,
                        /*OUT*/OCITypeMethod **pMethodType);

sword OCITypeArrayByName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
    /*IN*/const OCISvcCtx *pServiceContext,
    /*IN*/ub4 arrayLength,
    /*IN*/const oratext *pSchemaName[],
    /*IN*/ub4 sLength[],
    /*IN*/const oratext *pTypeName[],
    /*IN*/ub4 tLength[],
    /*IN*/const oratext *pVersionName[],
    /*IN*/ub4 vLen[],
    /*IN*/OCIDuration pinDur,
    /*IN*/OCITypeGetOpt getOpt,
    /*OUT*/OCIType **pPinnedType);

sword OCITypeIterNew(/*IN/OUT*/OCIEnv *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/ OCIType *pPinnedType,
                        /*OUT*/ OCITypeIter **pIterOrt);

sword OCITypeAttrNext(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN/OUT*/OCITypeIter *pIterAttr,
                      /*OUT*/OCITypeElem **pTypeElement);


sword OCITypeCollElem(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCIType *pPinnedType,
                      /*IN/OUT*/OCITypeElem **pTypeElement);

sword OCITypeIterSet(/*IN/OUT*/OCIEnv *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/OCIType *pPinnedType,
                      /*IN/OUT*/OCITypeIter *pIterOrt);

sword OCITypeIterFree(/*IN/OUT*/OCIEnv *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN/OUT*/OCITypeIter *pIterOrt);

sword   OCITypeArrayByRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                              /*IN*/ub4 arrayLength,
                              /*IN*/const OCIRef **pTypeReference,
                              /*IN*/OCIDuration pinDur,
                              /*IN*/OCITypeGetOpt getOpt,
                              /*OUT*/OCIType **pPinnedType);

oratext* OCITypeName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCIType *pPinnedType,
                     /*OUT*/ub4 *pNLen );

ub4 OCITypeAttrs(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                 /*IN*/const OCIType *pPinnedType);

ub4 OCITypeMethods(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCIType *pPinnedType);

ub4 OCITypeElemFlags(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCITypeElem *pTypeElem);

ub4 OCITypeElemLength(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCITypeElem *pTypeElem);

ub2 OCITypeElemCharSetID(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCITypeElem *pTypeElem);

ub2 OCITypeElemCharSetForm(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/const OCITypeElem *pTypeElem);

sword OCITypeElemParameterizedType(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                   /*IN*/const OCITypeElem *pTypeElem,
                                   /*OUT*/OCIType **pTypeStored);

OCITypeCode OCITypeElemExtTypeCode(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                   /*IN*/const OCITypeElem *pTypeElem);

ub1 OCITypeElemNumPrec(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCITypeElem *pTypeElem);

sb1 OCITypeElemNumScale(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCITypeElem *pTypeElem);

sword   OCITypeByRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIRef *pTypeReference,
                         /*IN*/OCIDuration pinDur,
                         /*IN*/OCITypeGetOpt getOpt,
                         /*OUT*/OCIType **pPinnedType);

sword OCITypeBeginCreate(/*IN*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OCITypeCode typeCode,
                         OCIDuration dur,
                         /*OUT*/OCIType **pType);

OCITypeCode OCITypeTypeCode(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN*/const OCIType *pPinnedType);

sword OCITypeSetCollection(/*IN*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN/OUT*/OCIType *pType,
                           OCIParam *pCollectionElementInfo,
                           ub4 collectionElementCount);

sword OCITypeSetBuiltin(/*IN*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN/OUT*/OCIType *pType,
                        OCIParam *pBuiltinInfo);

sword OCITypeAddAttr(/*IN*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN/OUT*/OCIType *pType,
                     /*IN, optional */const oratext *pAttrName,
                     /*IN, optional */ub4 attrNameLen,
                     OCIParam *pAttrInfo);

sword OCITypeElemType(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCITypeElem *pTypeElem,
                      /*OUT*/OCIType **pElemTdo);

oratext* OCITypeSchema(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCIType *pPinnedType,
                       /*OUT*/ub4 *pNLen);

oratext* OCITypeElemName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCITypeElem *pTypeElem,
                         /*OUT*/ub4 *pNLen);

OCITypeCode OCITypeElemTypeCode(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                /*IN*/const OCITypeElem *pTypeElem);

sword OCITypeAttrByName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIType *pPinnedType,
                        /*IN*/const oratext *pAttrName, ub4 nLen,
                        /*OUT*/OCITypeElem **pTypeElement);

OCITypeCode OCITypeCollTypeCode(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                /*IN*/const OCIType *pPinnedType);

oratext* OCITypeVersion(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIType *pPinnedType,
                        /*OUT*/ub4 *pVLen);

sword OCITypeEndCreate(/*IN*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN/OUT*/OCIType *pType);

sword OCITypeCollSize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCIType *pPinnedType,
                      /*OUT*/ub4 *pNumElements);

sword OCITypeCollExtTypeCode(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN*/const OCIType *pPinnedType,
                             /*OUT*/OCITypeCode *pSqlTypeCode);

ub4 OCITypeMethodOverload(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OCIType *pPinnedType,
                          /*IN*/const oratext *pMethodName,
                          /*IN*/ub4 mLen);

oratext *OCITypeMethodName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/const OCITypeMethod *pMethodType,
                           /*OUT*/ub4 *pNLen);

OCITypeEncap OCITypeMethodEncap(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                /*IN*/const OCITypeMethod *pMethodType);

OCITypeMethodFlag OCITypeMethodFlags(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                     /*IN*/const OCITypeMethod *pMethodType);

sword OCITypeMethodMap(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCIType *pPinnedType,
                       /*OUT*/OCITypeMethod **pMethodType);

sword OCITypeMethodOrder(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIType *pPinnedType,
                         /*OUT*/OCITypeMethod **pMethodType);

ub4 OCITypeMethodParams(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCITypeMethod *pMethodType);

sword OCITypeResult(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCITypeMethod *pMethodType,
                    /*OUT*/OCITypeElem **pTypeElement);

sword OCITypeParamByPos(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCITypeMethod *pMethodType,
                        /*IN*/ub4 position,
                        /*OUT*/OCITypeElem **pTypeElement);

sword OCITypeParamByName(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCITypeMethod *pMethodType,
                         /*IN*/const oratext *pParamName,
                         /*IN*/ub4 nLen,
                         /*OUT*/OCITypeElem **pTypeElement);

sword OCITypeParamPos(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCITypeMethod *pMethodType,
                      /*IN*/const oratext *pParamName,
                      /*IN*/ub4 nLen,
                      /*OUT*/ub4 *pPosition,
                      /*OUT*/OCITypeElem **pTypeElement);

OCITypeParamMode OCITypeElemParamMode(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                      /*IN*/const OCITypeElem *pTypeElem);

oratext* OCITypeElemDefaultValue(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                  /*IN*/const OCITypeElem *pTypeElem,
                                  /*OUT*/ub4 *pDefValueLen);

sword OCITypeVTInit(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle);

sword OCITypeVTInsert(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const oratext *pSchemaName,
                      /*IN*/ub4 schemaNameLen,
                      /*IN*/const oratext *pTypeName,
                      /*IN*/ub4 typeNameLen,
                      /*IN*/const oratext *pUserVersion,
                      /*IN*/ub4 userVersionLen);

sword OCITypeVTSelect(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const oratext *pSchemaName,
                      /*IN*/ub4 schemaNameLen,
                      /*IN*/const oratext *pTypeName,
                      /*IN*/ub4 typeNameLen,
                      /*OUT, optional*/oratext **pUserVersion,
                      /*OUT, optional*/ub4 *pUserVersionLen,
                      /*OUT, optional*/ub2 *pVersion);

//For compatibility only.
sword ortgcty(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle, OCIType *pCollTdo,
                  OCIType **pCollpElemTdo);









sword OCIObjectPin(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/OCIRef *pObjectRef,
                   /*IN*/OCIComplexObject *pCorhdl,
                   /*IN*/OCIPinOpt pinOption,
                   /*IN*/OCIDuration pinDur,
                   /*IN*/OCILockOpt lockOption,
                   /*OUT*/void  **pObject);

sword OCIObjectUnpin(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/void  *pObject);


sword OCIObjectArrayPin(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/OCIRef **pReferencesArray,
                        /*IN*/ub4 arraySize,
                        /*IN*/OCIComplexObject **pCorArray,
                        /*IN*/ub4 corArraySize,
                        /*IN*/OCIPinOpt pinOption,
                        /*IN*/OCIDuration pinDuration,
                        /*IN*/OCILockOpt lockOption,
                        /*OUT*/void  **pObjectsArray,
                        /*OUT*/ub4 *pPos);

sword OCIObjectPinCountReset(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN*/void  *pObject);

sword OCIObjectLock(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/void  *pObject);

sword OCIObjectNew(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCISvcCtx *pServiceContext,
                   /*IN*/OCITypeCode typecode,
                   /*IN*/OCIType *pPinnedType,
                   /*IN, optional */void  *pTable,
                   /*IN*/OCIDuration dur,
                   /*IN*/boolean value,
                   /*OUT*/void  **pInstance);

sword OCIObjectLockNoWait(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/void  *pObject);

sword OCIObjectUnmarkByRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/OCIRef *pObjectReference);

sword OCIObjectFree(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/void  *pInstance,
                    /*IN*/ub2 flags);

sword OCIObjectMarkUpdate(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/void  *pObject);

sword OCIObjectUnmark(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/void  *pObject);

sword OCIObjectFlush(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/void  *pObject);

sword OCIObjectMarkDeleteByRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                               /*IN*/OCIRef *pObjectReference);

sword OCIObjectMarkDelete(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/void  *pInstance);

sword OCIObjectCopy(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCISvcCtx *pServiceContext,
                    /*IN*/void  *pSource,
                    /*IN*/void  *pNnullSource,
                    /*IN*/void  *pTarget,
                    /*IN*/void  *pNullTarget,
                    /*IN*/OCIType *pPinnedType,
                    /*IN*/OCIDuration dur,
                    /*IN*/ub1 option);

sword OCIObjectRefresh(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/void  *pObject);

sword OCIObjectMakeObjectRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN*/const OCISvcCtx *pServiceContext,
                             /*IN*/void  *pTable,
                             /*IN*/void  **pValues,
                             /*IN*/ub4 attrlistArrayLen,
                             /*OUT*/OCIRef *pObjectReference);

sword OCIObjectGetTypeRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/void  *pInstance,
                          /*IN*/OCIRef *pTypeReference);


sword OCIObjectPinTable(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCISvcCtx *pServiceContext,
                        /*IN*/const oratext *pSchemaName,
                        /*IN*/ub4 sNLen,
                        /*IN*/const oratext *pObjectName,
                        /*IN*/ub4 oNLen,
                        /*IN*/const OCIRef *pScopeObjRef,
                        /*IN*/OCIDuration pinDur,
                        /*OUT*/void **pObject);

sword OCIObjectExists(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/void  *pInstance,
                      /*OUT*/boolean *pExist);

sword OCIObjectGetObjectRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN*/void  *pObject,
                            /*OUT*/OCIRef *pObjectReference);

sword OCIObjectGetPrimaryKeyTypeRef(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                                    /*IN*/const OCISvcCtx *pServiceContext,
                                    /*IN*/void  *pTable,
                                    /*OUT*/OCIRef *pTypeReference);

sword OCIObjectGetInd(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/void  *pInstance,
                      /*OUT*/void  **pNullStruct);

sword OCIObjectIsLocked(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/void  *pInstance,
                        /*OUT*/boolean *pLock);

sword OCIObjectIsDirty(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/void  *pInstance,
                       /*OUT*/boolean *pDirtyStatus);

sword OCIObjectGetProperty(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/const void  *pObj,
                           /*IN*/OCIObjectPropId propertyId,
                           /*OUT*/void  *pProperty,
                           /*IN/OUT*/ub4 *pSize);

sword OCICacheFree(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN, optional*/const OCISvcCtx *pServiceContext);

sword OCICacheUnmark(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN, optional*/const OCISvcCtx *pServiceContext);

//sword OCICacheRefresh(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
//                      /*IN, optional*/const OCISvcCtx *pServiceContext,
//                      /*IN, optional*/OCIRefreshOpt option,
//                      /*IN, optional*/void  *pContext,
//                      /*IN, optional*/OCIRef *(*get)(void  *pContext),
//                      /*OUT, optional*/OCIRef **pReference);

sword OCICacheUnpin(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN, optional*/const OCISvcCtx *pServiceContext);

sword OCICacheFlush(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN, optional*/const OCISvcCtx *pServiceContext,
                    /*IN, optional*/void  *pContext, // argument to the callback function 'get'
                    /*IN, optional*/OCIRef *(*get)(void  *pContext, ub1 *pLast),//'get' is client callback function
                    /*OUT, optional*/OCIRef **pReference );


sword OCIDurationEnd(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN/OUT*/const OCISvcCtx *pServiceContext,
                     /*OUT*/OCIDuration duration);

sword OCIDurationBegin(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN/OUT*/const OCISvcCtx *pServiceContext,
                       /*IN*/OCIDuration parentDuration,
                       /*OUT*/OCIDuration *pDur);




////////////////////////////////////////////////////////////////////////////////
//////////////////////  N U M B E R       F U N C T I O N S


sword OCINumberIntPower(/*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCINumber *pBase,
                        /*IN*/const sword exp,
                        /*OUT*/OCINumber *pOutputNum);

sword OCINumberShift(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pNumber,
                     /*IN*/const sword nDig,
                     /*OUT*/OCINumber *pOutputNum);

sword OCINumberToInt(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pNumber,
                     /*IN*/uword desiredResultLen,
                     /*IN*/uword signFlag, //signed or unsigned flag
                     /*OUT*/void  *pResult);

sword OCINumberLn(/*IN/OUT*/OCIError *pErrorHandle,
                  /*IN*/const OCINumber *pNum,
                  /*OUT*/OCINumber *pOutputNum);

sword OCINumberNeg(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNumber,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberToText(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*IN*/const oratext *pConvFormat,
                      /*IN*/ub4 convFormatLen,
                      /*IN*/const oratext *pNlsParamsFormat,
                      /*IN*/ub4 nlsParamsFormatLen,
                      /*IN/OUT*/ub4 *pBufferSize,
                      /*OUT*/oratext *pBuffer);

sword OCINumberFromText(/*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const oratext *pStr,
                        /*IN*/ub4 strLength,
                        /*IN*/const oratext *pConvFormat,
                        /*IN*/ub4 convFormatLen,
                        /*IN*/const oratext *pNlsParams,
                        /*IN*/ub4 nlsParamsLen,
                        /*OUT*/OCINumber *pNumber);

sword OCINumberLog(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pBase,
                   /*IN*/const OCINumber *pNum,
                   /*OUT*/OCINumber *pOutputNum);


sword OCINumberToReal(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*IN*/uword resultLen,
                      /*OUT*/void  *pResult);

sword OCINumberCmp(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum1,
                   /*IN*/const OCINumber *pNum2,
                   /*OUT*/sword *pResult);

sword OCINumberSign(/*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCINumber *pNum,
                    /*OUT*/sword *pResult);

sword OCINumberIsZero(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*OUT*/boolean *pResult);

sword OCINumberToRealArray(/*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/const OCINumber **pNum,
                           /*IN*/uword numUpperBoundArray,
                           /*IN*/uword resultLen,
                           /*OUT*/void  *pResult);

sword OCINumberFromInt(/*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const void  *pInt, //integer to be converted
                       /*IN*/uword pIntLen,
                       /*IN*/uword pIntSignFlag,
                       /*OUT*/OCINumber *pNumber);

sword OCINumberFromReal(/*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const void  *pFloatNum,
                        /*IN*/uword floatNumLen,
                        /*OUT*/OCINumber *pNum);

sword OCINumberAbs(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberCeil(/*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCINumber *pNum,
                    /*OUT*/OCINumber *pOutputNum);

sword OCINumberFloor(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pNum,
                     /*OUT*/OCINumber *pOutputNum);

sword OCINumberSqrt(/*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCINumber *pNum,
                    /*OUT*/OCINumber *pOutputNum);

sword OCINumberTrunc(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pNum,
                     /*IN*/sword numDecDigit,
                     /*OUT*/OCINumber *pOutputNum);

sword OCINumberPower(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pBase,
                     /*IN*/const OCINumber *pExponent,
                     /*OUT*/OCINumber *pOutputNum);

sword OCINumberIsInt(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pNum,
                     /*OUT*/boolean *pResult);

sword OCINumberAssign(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNumToAssigned,
                      /*OUT*/OCINumber *pNumCopiedInto);

sword OCINumberRound(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCINumber *pSourceNum,
                     /*IN*/sword numDecDigit,
                     /*OUT*/OCINumber *pOutputNum);

sword OCINumberPrec(/*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCINumber *pNum,
                    /*IN*/sword nDigs,
                    /*OUT*/OCINumber *pOutputNum);

sword OCINumberHypSin(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNumber,
                      /*OUT*/OCINumber *pOutputNum);

sword OCINumberCos(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberArcTan(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*OUT*/OCINumber *pOutputNum);

sword OCINumberInc(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN/OUT*/OCINumber *pNum);

sword OCINumberDec(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN/OUT*/OCINumber *pNum);

sword OCINumberArcCos(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*OUT*/OCINumber *pOutputNum);

sword OCINumberHypCos(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*OUT*/OCINumber *pOutputNum);

sword OCINumberSin(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberArcSin(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pNum,
                      /*OUT*/OCINumber *pOutputNum);

sword OCINumberTan(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum,
                   /*OUT*/OCINumber *pOutputNum);

//Initialize the  'num' to 0.
void OCINumberSetZero(/*IN/OUT*/OCIError *pErrorHandle,
                      /*OUT*/OCINumber *pNum);

void OCINumberSetPi(/*IN/OUT*/OCIError *pErrorHandle,
                    /*OUT*/OCINumber *pNum);

sword OCINumberMul(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum1,
                   /*IN*/const OCINumber *pNum2,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberHypTan(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCINumber *pArgument,
                      /*OUT*/OCINumber *pOutputNum);

sword OCINumberExp(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberDiv(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum1,
                   /*IN*/const OCINumber *pNum2,
                   /*OUT*/OCINumber *pOutputNum);

//Result == 'num2' + 'num1'
sword OCINumberAdd(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum1,
                   /*IN*/const OCINumber *pNum2,
                   /*OUT*/OCINumber *pOutputNum);

//Result == 'number2' - 'number1'
sword OCINumberSub(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum1,
                   /*IN*/const OCINumber *pNum2,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberMod(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCINumber *pNum1,
                   /*IN*/const OCINumber *pNum2,
                   /*OUT*/OCINumber *pOutputNum);

sword OCINumberArcTan2(/*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCINumber *pArgumentX,
                       /*IN*/const OCINumber *pArgumentY,
                       /*OUT*/OCINumber *pOutputNum);





////////////////////////////////////////////////////////////////////////////////
//////////////////////  D A T E    F U N C T I O N S


sword OCIDateFromText(/*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const oratext *pDateString,
                      /*IN*/ub4 dateStringLen,
                      /*IN*/const oratext *pConvFormat,
                      /*IN*/ub1 convFormatLen,
                      /*IN*/const oratext *pLanguageName,
                      /*IN*/ub4 languageNameLen,
                      /*OUT*/OCIDate *pDateResult);

sword OCIDateZoneToZone(/*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIDate *pDate1,
                        /*IN*/const oratext *zon1,
                        /*IN*/ub4 zonLen1,
                        /*IN*/const oratext *zon2,
                        /*IN*/ub4 zonLen2,
                        /*OUT*/OCIDate *pDate2);

sword OCIDateSysDate(/*IN/OUT*/OCIError *pErrorHandle,
                     /*OUT*/OCIDate *pSysDate);

sword OCIDateCompare(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCIDate *pDate1,
                     /*IN*/const OCIDate *pDate2,
                     /*OUT*/sword *pCompResult);

sword OCIDateAddMonths(/*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCIDate *pInputDate,
                       /*IN*/sb4 numMonths,
                       /*OUT*/OCIDate *pDateResult);

sword OCIDateCheck(/*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCIDate *pInputDate,
                   /*OUT*/uword *pValidDate);

sword OCIDateAssign(/*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCIDate *pSourceDate,
                    /*OUT*/OCIDate *pTargetDate);

sword OCIDateToText(/*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCIDate *pInputDate,
                    /*IN*/const oratext *pConvFormat,
                    /*IN*/ub1 convFormatLen,
                    /*IN*/const oratext *pLanguageName,
                    /*IN*/ub4 languageNameLen,
                    /*IN/OUT*/ub4 *pBufferSize,
                    /*OUT*/oratext *pBuffer);

sword OCIDateNextDay(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCIDate *pInputDate,
                     /*IN*/const oratext *pFirstDay,
                     /*IN*/ub4 firstDayLen,
                     /*OUT*/OCIDate *pNextSay);

sword OCIDateDaysBetween(/*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIDate *pInputDate1,
                         /*IN*/const OCIDate *pInputDate2,
                         /*OUT*/sb4 *pNumDays);

sword OCIDateAddDays(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCIDate *pInputDate,
                     /*IN*/sb4 numDays,
                     /*OUT*/OCIDate *pDateResult);

sword OCIDateLastDay(/*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCIDate *pInputDate,
                     /*OUT*/OCIDate *pLastDay);





////////////////////////////////////////////////////////////////////////////////////
/////////         V A R I A B L E   L E N G T H    S T R I N G         /////////////
sword OCIStringAllocSize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIString *pStr,
                         /*OUT*/ub4 *pAllocSize);

ub4 OCIStringSize(/*IN/OUT*/OCIEnv *pEnvironment,
                  /*IN*/const OCIString *pVs);

oratext *OCIStringPtr(/*IN/OUT*/OCIEnv *pEnvironment,
                      /*IN*/const OCIString *pVs);

sword OCIStringAssignText(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const oratext *pRightString,
                          /*IN*/ub4 rightStringLen,
                          /*IN/OUT*/OCIString **pLeftString);

sword OCIStringAssign(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCIString *pRightString,
                      /*IN/OUT*/OCIString **pLeftString);

sword OCIStringResize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/ub4 newSize,
                      /*IN/OUT*/OCIString **pStr);


////////////////////////////////////////////////////////////////////////////////////
////   V A R I A B L E     L E N G T H     R A W     F U N C T I O N S     /////////
sword OCIRawAssignBytes(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const ub1 *pRightString,
                        /*IN*/ub4 rightStringLen,
                        /*IN/OUT*/OCIRaw **pLeftString);

sword OCIRawAllocSize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCIRaw *pRaw,
                      /*OUT*/ub4 *pAllocSize);

sword OCIRawAssignRaw(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OCIRaw *pRightString,
                      /*IN/OUT*/OCIRaw **pLeftString);

ub1 *OCIRawPtr(/*IN/OUT*/OCIEnv *pEnvironment,
               /*IN*/const OCIRaw *pRaw);

sword OCIRawResize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/ub4 newSize,
                   /*IN*/OCIRaw **pRaw);

ub4 OCIRawSize(/*IN/OUT*/OCIEnv * EnvironmentHandle,
               /*IN*/const OCIRaw * Raw );


////////////////////////////////////////////////////////////////////////////////////
/////   O B J E C T      R E F E R E N C E      O P E R A T I O N S        /////////
void OCIRefClear(/*IN/OUT*/OCIEnv *pEnvironment,
                 /*IN/OUT*/OCIRef *pReference);

sword OCIRefAssign(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCIRef *pSourceRef,
                   /*IN/OUT*/OCIRef **pTargetRef);

boolean OCIRefIsEqual(/*IN/OUT*/OCIEnv *pEnvironment,
                      /*IN*/const OCIRef *pReference1,
                      /*IN*/const OCIRef *pReference2);

boolean OCIRefIsNull(/*IN/OUT*/OCIEnv *pEnvironment,
                     /*IN*/const OCIRef *pReference);

ub4 OCIRefHexSize(/*IN/OUT*/OCIEnv *pEnvironment,
                  /*IN*/const OCIRef *pReference);

sword OCIRefFromHex(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCISvcCtx *pServiceContext,
                    /*IN*/const oratext *pHexStr,
                    /*IN*/ub4 Len,
                    /*IN/OUT*/OCIRef **pReference);

sword OCIRefToHex(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                  /*IN*/const OCIRef *pReference,
                  /*OUT*/oratext *pHex,
                  /*IN/OUT*/ub4 *pHexLen);



//////////////////////////////////////////////////////////////////////////////
//////////////////     C O L L E C T I O N   F U N C T I O N S        ////////

sword OCICollGetElemArray(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OCIColl *pCollection,
                          /*IN*/sb4 indexElement,
                          /*OUT*/boolean *pElemExists,
                          /*OUT*/void  **pTypeElement,
                          /*OUT*/void  **pElementIndicators,
                          /*IN/OUT*/uword *pUpperBoundElemsIndicator);

sword OCICollGetElem(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCIColl *pCollection,
                     /*IN*/sb4 indexElement,
                     /*OUT*/boolean *pElemExists,
                     /*OUT*/void  **pTypeElement,
                     /*OUT*/void  **pElementIndicators);

sword OCICollAssignElem(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/sb4 indexElement,
                        /*IN*/const void  *pTypeElement,
                        /*IN*/const void  *pElementIndicators,
                        /*IN/OUT*/OCIColl *pCollection);

sword OCICollAppend(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const void  *pTypeElement,
                    /*IN*/const void  *pElementIndicators,
                    /*IN/OUT*/OCIColl *pCollection);

sword OCICollAssign(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCIColl *pSourceCollection,
                    /*OUT*/OCIColl *pTargetCollection);

sword OCICollTrim(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                  /*IN*/sb4 elementsTrimNum,
                  /*IN/OUT*/OCIColl *pCollection);

sword OCICollIsLocator(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCIColl *pCollection,
                       /*OUT*/boolean *pResult);

sword OCICollSize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                  /*IN*/const OCIColl *pCollection,
                  /*OUT*/sb4 *pSize);

sb4 OCICollMax(/*IN/OUT*/OCIEnv *pEnvironment,
               /*IN*/const OCIColl *pCollection);




sword OCIIterInit(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                  /*IN*/const OCIColl *pCollection,
                  /*IN/OUT*/OCIIter *pItr);

sword OCIIterNext(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                  /*IN/OUT*/OCIIter *pItr,
                  /*OUT*/void  **pElement,
                  /*OUT*/void  **pElementIndicators,
                  /*OUT*/boolean *pEndOfCollection);

sword OCIIterPrev(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                  /*IN/OUT*/OCIIter *pItr,
                  /*OUT*/void  **pElement,
                  /*OUT*/void  **pElementIndicators,
                  /*OUT*/boolean *pBeginningOfCollection);

sword OCIIterCreate(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCIColl *pCollection,
                    /*OUT*/OCIIter **pCollectionItr);

sword OCIIterGetCurrent(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIIter *pItr,
                        /*OUT*/void  **pElement,
                        /*OUT*/void  **pElementIndicators);

sword OCIIterDelete(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN/OUT*/OCIIter **pCollectionItr);


/////////////////////////////////////////////////////////////////////////////////////
///////FUNCTIONS WHICH OPERATE ONLY ON NESTED TABLE OCITable*()
sword OCITableFirst(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/const OCITable *pScannedTable,
                    /*OUT*/sb4 *pIndexElement );

sword OCITableSize(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCITable *pNestedTable,
                   /*OUT*/sb4 *pSize);

sword OCITableLast(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/const OCITable *pScannedTable,
                   /*OUT*/sb4 *pIndexElement );

sword OCITablePrev(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/sb4 index,
                   /*IN*/const OCITable *pScannedTable,
                   /*OUT*/sb4 *pRevIndex,
                   /*OUT*/boolean *pIndexExists);

sword OCITableExists(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/const OCITable *pNestedTable,
                     /*IN*/sb4 indexElement,
                     /*OUT*/boolean *pElementExists);

sword OCITableDelete(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/sb4 indexElement,
                     /*IN*/OCITable *pScannedTable);

sword OCITableNext(/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/sb4 indexElement,
                   /*IN*/const OCITable *pScannedTable,
                   /*OUT*/sb4 *pNextIndex,
                   /*OUT*/boolean *pIndexExists);


//////////////////////// P R I V A T E   F U N C T I O N S  ////////////////////
void  *pOciepacm(OCIExtProcContext *pWithContext, size_t amount);
size_t ocieperr(OCIExtProcContext *pWithContext, int errorNumber);
size_t ociepmsg(OCIExtProcContext *pWithContext, int errorNumber,
                oratext *pErrorMessage, size_t len);
sword ociepgoe(OCIExtProcContext *pWithContext, OCIEnv **pEnvh,
               OCISvcCtx **pSvch, /*IN/OUT*/OCIError **pErrh);


/////     BIND (Piecewise or with Skips)
sword  ocon  (struct cdaDef *pLda);
sword  obindps(struct cdaDef *pCursor, ub1 opcode,
               OraText *pSqlvar,
               sb4 sqlvl, ub1 *pPvctx,
               sb4 progvl,
               sword ftype, sword scale,
               sb2 *pIndp, ub2 *pALen,
               ub2 *pArcode,
               sb4 pvSkip, sb4 indSkip,
               sb4 alenSkip, sb4 rcSkip,
               ub4 maxsiz, ub4 *pCursiz,
               oratext *pConvFormat,
               sb4 fmtl, sword fmtt);

sword obndrn( struct cdaDef * pCursor,
              sword sqlvn,
              ub1 * pPvctx,
              sword progvl,
              sword ftype,
              sword scale,
              sb2 * pIndp,
              text * pConvFormat,
              sword fmtl,
              sword fmtt );

#define obndra( cursor, sqlvar, sqlvl, progv, progvl, ftype, scale, indp, alen, arcode, maxsiz, cursiz, fmt, fmtl, fmtt )  \
    obindps( cursor, 1, sqlvar, sqlvl, progv, progvl, ftype, scale, indp, alen, arcode, 0, 0, 0, 0, maxsiz, cursiz, fmt, fmtl, fmtt )

#define obndrv( cursor, sqlvar, sqlvl, progv, progvl, ftype, scale, indp, fmt, fmtl, fmtt )  \
    obindps( cursor, 1, sqlvar, sqlvl, progv, progvl, ftype, scale, indp, NULL, NULL, 0, 0, 0, 0, 0, NULL, fmt, fmtl, fmtt )

sword  ocan  (struct cdaDef *pCursor);
sword  ocom  (struct cdaDef *pLda);
sword  ocof  (struct cdaDef *pLda);



 /////     Define (Piecewise or with Skips)
 sword  odescr(struct cdaDef *pCursor,
              sword pos, sb4 *pDbsize,
              sb2 *pDbtype, sb1 *pCbuf,
              sb4 *pCbufl, sb4 *pDsize,
              sb2 *pRec, sb2 *pScale, sb2 *pNullok);

 sword  odefinps(struct cdaDef *pCursor,
                ub1 opcode, sword pos,
                ub1 *pBufctx, sb4 bufl,
                sword ftype, sword scale,
                sb2 *pIndp, oratext *pConvFormat,
                sb4 fmtl, sword fmtt,
                ub2 *pRLen, ub2 *pRcode,
                sb4 pvSkip, sb4 indSkip,
                sb4 alenSkip, sb4 rcSkip);

#define odefin( cursor, pos, buf, bufl, ftype, scale, indp, fmt, fmtl, fmtt, rlen, rcode )  \
    odefinps( cursor, 1, pos, buf, bufl, ftype, scale, indp, fmt, fmtl, fmtt, rlen, rcode, 0, 0, 0, 0 )

sword  oclose(struct cdaDef *pCursor);
sword  obreak(struct cdaDef *pLda);

sword  osetpi   (struct cdaDef *pCursor,
                 ub1 piece, void  *pBufp,
                 ub4 *pLenp);

void sqlld2     (struct cdaDef *pLda,
                 OraText *pCname,
                 sb4 *pCnLen);

void sqllda     (struct cdaDef *pLda);

sword  odessp(struct cdaDef *pCursor, OraText *pObjnam, size_t onLen,
              ub1 *pRsv1, size_t rsv1ln, ub1 *pRsv2, size_t rsv2ln,
              ub2 *pOvrld, ub2 *pPos, ub2 *pLevel, OraText **pArgnam,
              ub2 *pArnLen, ub2 *pDtype, ub1 *pDefsup, ub1 *pMode,
              ub4 *pDtsiz, sb2 *pRec, sb2 *pScale, ub1 *pRadix,
              ub4 *pSpare, ub4 *pArrsiz);


sword  oerhms   (struct cdaDef *pLda,
                 sb2 rcode,
                 oratext *pBuffer,
                 sword bufsiz);

sword  oermsg   (sb2 rcode, oratext *pBuffer);
sword  oparse   (struct cdaDef *pCursor,
                 OraText *pSqlstm, sb4 sqlLen,
                 sword defflg, ub4 lngflg);

sword  orol     (struct cdaDef *pLda);

sword  oexn     (struct cdaDef *pCursor, sword iters, sword rowOff);
sword  ofen     (struct cdaDef *pCursor, sword nRows);
sword  ofetch   (struct cdaDef *pCursor);
sword  oflng    (struct cdaDef *pCursor, sword pos, ub1 *pBuffer,
                 sb4 bufl, sword dtype, ub4 *pRetl, sb4 offset);
sword  ogetpi   (struct cdaDef *pCursor, ub1 *pPiecep, void  **pCtxpp,
                 ub4 *pIterp, ub4 *pIndexp);
sword  oopt     (struct cdaDef *pCursor, sword rbopt, sword waitopt);
sword  opinit   (ub4 mode);
sword  oexec    (struct cdaDef *pCursor);
sword  oexfet   (struct cdaDef *pCursor, ub4 nRows,
                 sword cancel, sword exact);


///////// non-blocking functions
sword  orlon    (struct cdaDef *pLda, ub1 *pHda,
                 OraText *pUid, sword uidl,
                 OraText *pSwd, sword pswdl,
                 sword audit);

sword onbtst    (struct cdaDef *pLda);

sword  olog     (struct cdaDef *pLda, ub1 *pHda,
                 OraText *pUid, sword uidl,
                 OraText *pSwd, sword pswdl,
                 OraText *pConn, sword connl,
                 ub4 mode);

sword  ologof   (struct cdaDef *pLda);

sword ognfd     (struct cdaDef *pLda, void *pFdp);

sword  olon     (struct cdaDef *pLda,
                 OraText *pUid,
                 sword uidl,
                 OraText *pSwd,
                 sword pswdl,
                 sword audit);

sword  oname    (struct cdaDef *pCursor, sword pos,
                 sb1 *pTbuf, sb2 *pTbufl,
                 sb1 *pBuffer, sb2 *pBufl);


sword  osql3    (struct cdaDef *pCda,
                 OraText *pSqlstm,
                 sword sqlLen);

sword  odsc     (struct cdaDef *pCursor,
                 sword pos, sb2 *pDbsize,
                 sb2 *pFsize, sb2 *pRcode,
                 sb2 *pDtype, sb1 *pBuffer,
                 sb2 *pBufl, sb2 *pDsize);

sword onbset    (struct cdaDef *pLda);

sword  oopen    (struct cdaDef *pCursor,
                 struct cdaDef *pLda,
                 OraText *pDbn, sword dbnl,
                 sword arsize,
                 OraText *pUid, sword uidl);
sword onbclr    (struct cdaDef *pLda);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////    P U B L I C     E R R O R       F U N C T I O N S      ///////////////////////////

//Convert SSL errors to Oracle errors.


nzerror nzteOpenPersona(/*IN*/nzctx *pOssContext,
                        /*IN/OUT*/nzttPersona *pPersona);


nzerror nztwRetrievePersonaCopy(/*IN*/nzctx *pOssContext,
                                /*IN*/nzttWallet *pWallet,
                                /*IN*/ub4 index,
                                /*OUT*/nzttPersona **pPersonaFound);

nzerror nzteClosePersona(/*IN*/nzctx *pOssContext,
                         /*IN/OUT*/nzttPersona *pPersona);

nzerror nztific_FreeIdentityContent(/*IN*/nzctx *pOssContext,
                                    /*IN/OUT*/nzttIdentity *pIdentity);

nzerror nztSign(/*IN*/nzctx *pOssContext,
                /*IN*/nzttPersona *pPersona,
                /*IN*/nzttces signatureState,
                /*IN*/ub4 inputPartLen,
                /*IN*/ub1 *pInputPart,
                /*IN/OUT*/nzttBufferBlock *pTduBuffer);

nzerror nztwOpenWallet(/*IN*/nzctx *pOssContext,
                       /*IN*/ub4 wrlLen,
                       /*IN*/text *pWrl,
                       /*IN*/ub4 passwordLen,
                       /*IN*/text *pPassword,
                       /*IN/OUT*/nzttWallet *pWallet);

nzerror nztwCloseWallet(/*IN*/nzctx *pOssContext,
                        /*IN/OUT*/nzttWallet *pWallet);

nzerror nztwRetrievePersonaCopyByName(/*IN*/nzctx *pOssContext,
                                      /*IN*/nzttWallet *pWallet,
                                      /*IN*/char *pPersonaName,
                                      /*OUT*/nzttPersona **pPersonaFound);
nzerror nzteDestroyPersona(/*IN*/nzctx *pOssContext,
                           /*IN/OUT*/nzttPersona **pPersona);


nzerror nzteX509CreatePersona(/*IN*/nzctx *pOssContext,
                              /*IN*/ub1 *pCertificate,
                              /*IN*/ub4 certificateLen,
                              /*OUT*/nzttPersona **pPersona);

nzerror nztiCreateIdentity(/*IN*/nzctx *pOssContext,
                           /*IN*/nzttVersion identityType,
                           /*IN*/nzttIdentityDesc *pIdentityDescription,
                           /*IN/OUT*/nzttIdentity ** pPersonaIdentity);

nzerror nztiAbortIdentity(/*IN*/nzctx *pOssContext,
                          /*IN/OUT*/nzttIdentity **pIdentity);


nzerror nztVerify(/*IN*/nzctx *pOssContext,
                  /*IN*/nzttPersona *pPersona,
                  /*IN*/nzttces verificationState,
                  /*IN*/ub4 tduLen,
                  /*IN*/ub1 tdu,
                  /*IN/OUT*/nzttBufferBlock *pMessageExtracted,
                  /*OUT*/boolean *pVerified,
                  /*OUT*/boolean *pValidated,
                  /*OUT*/nzttIdentity ** partyIdentity);

nzerror nztValidate(/*IN*/nzctx *pOssContext,
                    /*IN*/nzttPersona *pPersona,
                    /*IN*/nzttIdentity *pIdentity,
                    /*OUT*/boolean *pValidated);

nzerror nztsd_SignDetached(/*IN*/nzctx *pOssContext,
                           /*IN*/nzttPersona *pPersona,
                           /*IN*/nzttces signatureState,
                           /*IN*/ub4 inputPartLen,
                           /*IN*/ub1 *pInputPart,
                           /*IN/OUT*/nzttBufferBlock *pTduBuffer);

nzerror nztrn_RandomNumber(/*IN*/nzctx *pOssContext,
                           /*IN*/nzttPersona *pPersona,
                           /*OUT*/uword *pNum);

nzerror nztbbInitBlock(/*IN*/nzctx *pOssContext,
                       /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztbbReuseBlock(/*IN*/nzctx *pOssContext,
                        /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztved_VerifyDetached(/*IN*/nzctx *pOssContext,
                              /*IN*/nzttPersona *pPersona,
                              /*IN*/nzttces verificationState,
                              /*IN*/ub4 dataLen,
                              /*IN*/ub1 *pData,
                              /*IN*/ub4 tduLen,
                              /*IN*/ub1 *pTdu,
                              /*OUT*/boolean *pVerifiedSignature,
                              /*OUT*/boolean *pValidatedIdentity,
                              /*OUT*/nzttIdentity **pPartyIdentity);

nzerror nztkec_PKEncrypt(/*IN*/nzctx *pOssContext,
                         /*IN*/nzttPersona *pPersona,
                         /*IN*/ub4 recipientsNum,
                         /*IN*/nzttIdentity *pRecipientsList,
                         /*IN*/nzttces encryptionState,
                         /*IN*/ub4 inputPartLen,
                         /*IN*/ub1 *pInputPart,
                         /*IN/OUT*/nzttBufferBlock *pTduBuffer);

nzerror nztxkec_PKEncryptExpansion(/*IN*/nzctx *pOssContext,
                                   /*IN*/nzttPersona *pPersona,
                                   /*IN*/ub4 recipientsNum,
                                   /*IN*/ub4 inputLen,
                                   /*OUT*/ub4 *pBufferLen);

nzerror nztkdc_PKDecrypt(/*IN*/nzctx *pOssContext,
                         /*IN*/nzttPersona *pPersona,
                         /*IN*/nzttces encryptionState,
                         /*IN*/ub4 inputPartLen,
                         /*IN*/ub1 *pInputPart,
                         /*IN/OUT*/nzttBufferBlock *pTduBuffer);

nzerror nztHash(/*IN*/nzctx *pOssContext,
                /*IN*/nzttPersona *pPersona,
                /*IN*/nzttces encryptionState,
                /*IN*/ub4 inputLen,
                /*IN*/ub1 *pInput,
                /*IN/OUT*/nzttBufferBlock *pTduBuffer );

nzerror nztSeedRandom(/*IN*/nzctx *pOssContext,
                      /*IN*/nzttPersona *pPersona,
                      /*IN*/ub4 seedLen,
                      /*IN*/ub1 *pSeed);

nzerror nztrb_RandomBytes(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttPersona *pPersona,
                          /*IN*/ub4 bufferLen,
                          /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztbbSizeBlock(/*IN*/nzctx *pOssContext,
                       /*IN*/ub4 bufferMinLen,
                       /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztbbGrowBlock(/*IN*/nzctx *pOssContext,
                       /*IN*/ub4 increaseNum,
                       /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztGetCertChain(/*IN*/nzctx *pOssContext,
                        /*IN*/nzttWallet *pWallet);

nzerror nztCompareDN(/*IN*/nzctx *pOssContext,
                     /*IN*/ub1 *pDistinguishedName1,
                     /*IN*/ub4 distName1Len,
                     /*IN*/ub1 *pDistinguishedName2,
                     /*IN*/ub4 distName2Len,
                     /*OUT*/boolean *pResult);

nzerror nztbbPurgeBlock(/*IN*/nzctx *pOssContext,
                        /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztbbSetBlock(/*IN*/nzctx *pOssContext,
                      /*IN*/uword setFlag,
                      /*IN*/ub4 bufferLen,
                      /*IN*/ub4 usedLen,
                      /*IN*/ub1 *pBuffer,
                      /*IN/OUT*/nzttBufferBlock *pBlockBuffer );

nzerror nztiGetSecInfo(/*IN*/nzctx *pOssContext,
                       /*IN*/nzttPersona *pPersona,
                       /*OUT*/text **pDistinguishedName,
                       /*OUT*/ub4 *pDistinguishedNameLen,
                       /*OUT*/text **pIssuerName,
                       /*OUT*/ub4 *pIssuerNameLen,
                       /*OUT*/ub1 **pCertificateHash,
                       /*OUT*/ub4 *pHashLen);

nzerror nztiGetDName(/*IN*/nzctx *pOssContext,
                     /*IN*/nzttIdentity *pIdentity,
                     /*OUT*/text **pDnName,
                     /*OUT*/ub4 *pDnLen);

nzerror nztiGetIssuerName(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttIdentity *pIdentity,
                          /*OUT*/text **pIssuerName,
                          /*OUT*/ub4 *pIssuerNameLen);

nzerror nztgch_GetCertHash(/*IN*/nzctx *pOssContext,
                           /*IN*/nzttIdentity *pIdentity,
                           /*OUT*/ub1 **pCertHashBuffer,
                           /*OUT*/ub4 *pCertHashBufferLen);

nzerror nztdbuf_DestroyBuf(/*IN*/nzctx *pOssContext,
                           /*IN*/void  **pBuffer);

#ifdef NZ_OLD_TOOLS
nzerror nztIPDuplicate(/*IN*/nzctx *pOssContext,
                       /*OUT*/nzttIdentityPrivate **pTargetIdentityPrivateStruct;
                       /*IN*/nzttIdentityPrivate **pSourceIdentityPrivateStruct);

nzerror nztiDupIdentList(/*IN*/nzctx *pOssContext,
                         /*IN*/nzttIdentity  *pIdentityList,
                         /*OUT*/ub4 *pIdentityListLen,
                         /*OUT*/nzttIdentity **pContextIdentity);

nzerror nztFreeIdentList(/*IN*/nzctx *pOssContext,
                         /*OUT*/nzttIdentity **pContextIdentity);

nzerror nztIPrivateAlloc(/*IN*/nzctx *pOssContext,
                         /*OUT*/nzttIdentityPrivate **pIdentityPrivateStruct);

nzerror nztIdentityAlloc(/*IN*/nzctx *pOssContext,
                         /*OUT*/nzttIdentity **pContextIdentity);

nzerror nztIDupContent(/*IN*/nzctx *pOssContext,
                       /*OUT*/nzttIdentity *pTargetIdentity,
                       /*IN*/nzttIdentity *pSourceIdentity);
#endif

nzerror nzteCreatePersona(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttVersion identityType,
                          /*IN*/nzttCipherType cipherType,
                          /*IN*/nzttPersonaDesc *pPersonaDescription,
                          /*OUT*/nzttPersona **pPersona);

nzerror nztiStoreTrustedIdentity(/*IN*/nzctx *pOssContext,
                                 /*OUT*/nzttIdentity **pTrustedIdentity,
                                 /*IN/OUT*/nzttPersona *pPersona);

nzerror nztCheckValidity(/*IN*/nzctx *pOssContext,
                         /*IN*/ub4 startTime,
                         /*IN*/ub4 endTime);

nzerror nztwCreateWallet(/*IN*/nzctx *pOssContext,
                         /*IN*/ub4 wrlLen,
                         /*IN*/text *pWrl,
                         /*IN*/ub4 passwordLen,
                         /*IN*/text *pPassword,
                         /*IN/OUT*/nzttWallet *pWallet);

nzerror nztwDestroyWallet(/*IN*/nzctx *pOssContext,
                          /*IN*/ub4 wrlLen,
                          /*IN*/text *pWrl,
                          /*IN*/ub4 passwordLen,
                          /*IN*/text *pPassword);

nzerror nzteStorePersona(/*IN*/nzctx *pOssContext,
                         /*IN/OUT*/nzttPersona **pPersona,
                         /*IN/OUT*/nzttWallet *pWallet);

nzerror nzteRemovePersona(/*IN*/nzctx *pOssContext,
                          /*OUT*/nzttPersona **pPersona);

nzerror nzteSetProtection(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttPersona *pPersona,
                          /*IN*/nzttcef ceFunc,
                          /*IN*/nztttdufmt tduFormat,
                          /*IN*/nzttProtInfo *pRotectionInfo);

nzerror nzteGetProtection(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttPersona *pPersona,
                          /*IN*/nzttcef ceFunc,
                          /*OUT*/nztttdufmt *pTduFormat,
                          /*OUT*/nzttProtInfo *pRotectionInfo);

nzerror nztiRemoveIdentity(/*IN*/nzctx *pOssContext,
                           /*IN/OUT*/nzttIdentity **pIdentity);

nzerror nztifdn(/*IN*/nzctx  *pOssctx,
                /*IN*/ub4    Len,
                /*IN*/text   *pDistinguishedName,
                /*OUT*/nzttIdentity **pCreatedIdentity);

nzerror nztxSignExpansion(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttPersona *pPersona,
                          /*IN*/ub4 inputLen,
                          /*OUT*/ub4 *pTduLen);

nzerror nztxsd_SignDetachedExpansion(/*IN*/nzctx *pOssContext,
                                     /*IN*/nzttPersona *pPersona,
                                     /*IN*/ub4 inputLen,
                                     /*OUT*/ub4 *pSignatureBufferLen);

nzerror nztEncrypt(/*IN*/nzctx *pOssContext,
                   /*IN*/nzttPersona *pPersona,
                   /*IN*/nzttces state,
                   /*IN*/ub4 inputPartLen,
                   /*IN*/ub1 *pInputPart,
                   /*IN/OUT*/nzttBufferBlock *pTduBuffer );

nzerror nztxEncryptExpansion(/*IN*/nzctx *pOssContext,
                             /*IN*/nzttPersona *pPersona,
                             /*IN*/ub4 inputPartLen,
                             /*OUT*/ub4 *pTduLen);

nzerror nztDecrypt(/*IN*/nzctx *pOssContext,
                   /*IN*/nzttPersona *pPersona,
                   /*IN*/nzttces decryptionState,
                   /*IN*/ub4 inputPartLen,
                   /*IN*/ub1 *pInputPart,
                   /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztEnvelope(/*IN*/nzctx *pOssContext,
                    /*IN*/nzttPersona *pPersona,
                    /*IN*/ub4 recipientsListLen,
                    /*IN*/nzttIdentity *pRecipientsList,
                    /*IN*/nzttces encryptionState,
                    /*IN*/ub4 inputPartLen,
                    /*IN*/ub1 *pInputPart,
                    /*IN/OUT*/nzttBufferBlock *pTduBuffer );

nzerror nztiae_IsAuthEnabled(/*IN*/nzctx *pOssContext,
                             /*IN*/ub2 cipherSuite,
                             /*OUT*/boolean *pAuthEnabled);

nzerror nztiee_IsEncrEnabled(/*IN*/nzctx *pOssContext,
                             /*IN*/ub2 cipherSuite,
                             /*OUT*/boolean *pEncrEnabled);

nzerror nztihe_IsHashEnabled(/*IN*/nzctx *pOssContext,
                             /*IN*/ub2 cipherSuite,
                             /*OUT*/boolean *pHashEnabled);

nzerror nztDeEnvelope(/*IN*/nzctx *pOssContext,
                      /*IN*/nzttPersona *pPersona,
                      /*IN*/nzttces State,
                      /*IN*/ub4 inputPartLen,
                      /*IN*/ub1 *pInputPart,
                      /*OUT*/nzttBufferBlock *pBuffer,
                      /*OUT*/boolean *pVerified,
                      /*OUT*/boolean *pValidated,
                      /*OUT*/nzttIdentity **pSenderIdentity);

nzerror nztKeyedHash(/*IN*/nzctx *pOssContext,
                     /*IN*/nzttPersona *pPersona,
                     /*IN*/nzttces hashState,
                     /*IN*/ub4 inputLen,
                     /*IN*/ub1 *pInput,
                     /*IN/OUT*/nzttBufferBlock *pBuffer );

nzerror nztxKeyedHashExpansion(/*IN*/nzctx *pOssContext,
                               /*IN*/nzttPersona *pPersona,
                               /*IN*/ub4 inputLen,
                               /*OUT*/ub4 *pTduLen);


nzerror nztxHashExpansion(/*IN*/nzctx *pOssContext,
                          /*IN*/nzttPersona *pPersona,
                          /*IN*/ub4 inputLen,
                          /*OUT*/ub4 *pTduLen);

////////////////////////////////////////////////////
nzerror nztGetSubjectName(/*IN*/nzctx *pOssContext,
                          nzttIdentity *pIdentity,
                          ub1  **pSubjectname,
                          ub4   *pSubjectnameLen);

nzerror nztGetPublicKey(/*IN*/nzctx *pOssContext,
                        nzttIdentity *pIdentity,
                        ub1  **pPubKey,
                        ub4   *pPubKeyLen);

nzerror nztGetBase64Cert(/*IN*/nzctx *pOssContext,
                         nzttIdentity *pIdentity,
                         ub1  **pB64cert,
                         ub4   *pB64certLen);

nzerror nztGetSerialNumber(/*IN*/nzctx *pOssContext,
                           nzttIdentity *pIdentity,
                           ub1   **pSerialNum,
                           ub4    *pSerialNumLen);

nzerror nztGetIssuerName(/*IN*/nzctx *pOssContext,
                         nzttIdentity *pIdentity,
                         ub1  **pIssuername,
                         ub4   *pIssuernameLen);

nzerror nztGetValidDate(/*IN*/nzctx *pOssContext,
                        nzttIdentity *pIdentity,
                        ub4  *pStartDate,
                        ub4  *pEndDate );

nzerror nztSetAppDefaultLocation(/*IN*/nzctx *pOssContext,
                                 text *,
                                 size_t);

nzerror nztSearchNZDefault(/*IN*/nzctx *pOssContext,
                           boolean *pSearch);

nzerror nztSetLightWeight(/*IN*/nzctx *pOssContext,
                          boolean flag);

nzerror nztGetVersion(/*IN*/nzctx *pOssContext,
                      nzttIdentity *pIdentity,
                      nzstrc *pVerStr );

nzerror nztGenericDestroy(/*IN*/nzctx *pOssContext,
                          ub1  **pVar);



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//////////          A P I           F U N C T I O N S            ///////////////////
sword   OCITerminate(/*IN*/ub4 mode);

sword   OCIInitialize   (/*IN*/ub4 mode, /*IN*/void  *pCtxp,
                 /*IN*/OCICallbackMalloc malocfp,
                 /*IN*/OCICallbackRaloc ralocfp,
                 /*IN*/OCICallbackFree mfreefp );


sword   OCIEnvCreate (/*OUT*/OCIEnv **pEnvp,
                      /*IN*/ub4 mode,
                      /*IN*/void  *pCtxp,
                 /*IN*/OCICallbackMalloc malocfp,
                 /*IN*/OCICallbackRaloc ralocfp,
                 /*IN*/OCICallbackFree mfreefp,
                      /*IN*/size_t xtramemSize,
                      /*OUT*/void  **pUsrmempp);

sword   OCIEnvNlsCreate (/*IN/OUT*/OCIEnv **pEnvp, ub4 mode, void  *pCtxp,
                 void  *(*pMalocfp)(void  *pCtxp, size_t size),
                 void  *(*pRalocfp)(void  *pCtxp, void  *pMemptr, size_t newSize),
                 void   (*pMfreefp)(void  *pCtxp, void  *pMemptr),
                 size_t xtramemSize, void  **pUsrmempp,
                 ub2 charset, ub2 ncharset);

sword   OCIFEnvCreate (/*IN/OUT*/OCIEnv **pEnvp, ub4 mode, void  *pCtxp,
                 void  *(*pMalocfp)(void  *pCtxp, size_t size),
                 void  *(*pRalocfp)(void  *pCtxp, void  *pMemptr, size_t newSize),
                 void   (*pMfreefp)(void  *pCtxp, void  *pMemptr),
                 size_t xtramemSize, void  **pUsrmempp, void  *pFupg);

sword   OCIHandleAlloc( /*IN*/const void  *pParenth,
                        /*OUT*/void  **pHndlpp,
                        /*IN*/const ub4 type,
                        /*IN*/const size_t xtramemSize,
                        /*OUT*/void  **pUsrmempp);

sword   OCIArrayDescriptorAlloc(const void  *pParenth, void  **pDescpp,
                           const ub4 type, ub4 arraySize,
                           const size_t xtramemSize, void  **pUsrmempp);

sword   OCIDescriptorFree(void  *pDescp, const ub4 type);

sword   OCIArrayDescriptorFree(void  **pDescp, const ub4 type);

sword   OCIEnvInit (/*OUT*/OCIEnv **pEnvp,
                    /*IN*/ub4 mode,
                    /*IN*/size_t xtramemSize,
                    /*OUT*/void  **pUsrmempp);

sword   OCIServerAttach  (/*IN/OUT*/OCIServer *pSrvhp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OraText *pDblink,
                          /*IN*/sb4 dblinkLen,
                          /*IN*/ub4 mode);

sword   OCIServerDetach  (/*IN*/OCIServer *pSrvhp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/ub4 mode);

sword   OCISessionBegin  (/*IN*/OCISvcCtx *pServiceContexthp,
                          /*IN*/OCIError *pErrorHandle,
                          /*IN/OUT*/OCISession *pUsrhp,
                          /*IN*/ub4 credt,
                          /*IN*/ub4 mode);


sword   OCIHandleFree(/*IN*/void  *pHndlp,
                      /*IN*/const ub4 type);


sword   OCIDescriptorAlloc(const void  *pParenth, void  **pDescpp,
                           const ub4 type, const size_t xtramemSize,
                           void  **pUsrmempp);

sword   OCISessionEnd   (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OCISession *pUsrhp,
                         /*IN*/ub4 mode);

sword   OCILogon2 (/*IN*/OCIEnv *pEnvironment,
                   /*IN/OUT*/OCIError *pErrorHandle,
                   /*OUT*/OCISvcCtx **pSvchp,
                  /*IN*/const OraText *pUserName,
                  /*IN*/ub4 unameLen,
                  /*IN*/const OraText *pPassword,
                  /*IN*/ub4 passwdLen,
                  /*IN*/const OraText *pDbname,
                  /*IN*/ub4 dbnameLen,
                  /*IN*/ub4 mode);

sword   OCILogoff (/*IN*/OCISvcCtx *pServiceContexthp,
                   /*IN/OUT*/OCIError *pErrorHandle);

sword   OCILogon (/*IN*/OCIEnv *pEnvironment,
                  /*IN/OUT*/OCIError *pErrorHandle,
                  /*OUT*/OCISvcCtx **pSvchp,
                  /*IN*/const OraText *pUserName,
                  /*IN*/ub4 unameLen,
                  /*IN*/const OraText *pPassword,
                  /*IN*/ub4 passwdLen,
                  /*IN*/const OraText *pDbname,
                  /*IN*/ub4 dbnameLen);


sword OCIStmtPrepare2 (/*IN*/OCISvcCtx *pServiceContexthp,
                       /*OUT*/OCIStmt **pStmtp,
                       /*IN*/OCIError *pErrorHandle,
                       /*IN*/const OraText *pStmt,
                       /*IN*/ub4 stmtLen,
                       /*IN*/const OraText *pKey,
                       /*IN*/ub4 keyLen,
                       /*IN*/ub4 language,
                       /*IN*/ub4 mode);

sword OCIStmtRelease (/*IN*/OCIStmt *pStmtp,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/const OraText *pKey,
                       /*IN*/ub4 keyLen,
                       /*IN*/ub4 mode);

sword   OCIPasswordChange   (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                             /*IN*/OCIError *pErrorHandle,
                             /*IN*/const OraText *pUserName,
                             /*IN*/ub4 usernmLen,
                             /*IN*/const OraText *pOpasswd,
                             /*IN*/ub4 opasswdLen,
                             /*IN*/const OraText *pNpasswd,
                             /*IN*/ub4 npasswdLen,
                             /*IN*/ub4 mode);

sword   OCIBindArrayOfStruct   (/*IN*/OCIBind *pBindp,
                                /*IN*/OCIError *pErrorHandle,
                                /*IN*/ub4 pvskip,
                                /*IN*/ub4 indskip,
                                /*IN*/ub4 alskip,
                                /*IN*/ub4 rcskip);

sword   OCIStmtGetPieceInfo   (/*IN*/OCIStmt *pStmtp,
                               /*OUT*/OCIError *pErrorHandle,
                               /*OUT*/void  **pHndlpp,
                               /*OUT*/ub4 *pTypep,
                               /*OUT*/ub1 *pInOutp,
                               /*OUT*/ub4 *pIterp,
                               /*OUT*/ub4 *pIdxp,
                               /*OUT*/ub1 *pPiecep);

sword   OCIBindByPos  (/*IN/OUT*/OCIStmt *pStmtp,
                       /*IN/OUT*/OCIBind **pBindp,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/ub4 position,
                       /*IN/OUT*/void  *pValuep,
                       /*IN*/sb4 valueSize,
                       /*IN*/ub2 dty,
                       /*IN/OUT*/void  *pIndp,
                       /*IN/OUT*/ub2 *pAlenp,
                       /*OUT*/ub2 *pRcodep,
                       /*IN*/ub4 maxarrLen,
                       /*IN/OUT*/ub4 *pCurelep,
                       /*IN*/ub4 mode);

sword   OCIBindByName   (/*IN/OUT*/OCIStmt *pStmtp,
                         /*IN/OUT*/OCIBind **pBindp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OraText *placeholder,
                         /*IN*/sb4 placehLen,
                         /*IN/OUT*/void  *pValuep,
                         /*IN*/sb4 valueSize,
                         /*IN*/ub2 dty,
                         /*IN/OUT*/void  *pIndp,
                         /*IN/OUT*/ub2 *pAlenp,
                         /*OUT*/ub2 *pRcodep,
                         /*IN*/ub4 maxarrLen,
                         /*IN/OUT*/ub4 *pCurelep,
                         /*IN*/ub4 mode);

sword   OCIStmtExecute  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIStmt *pStmtp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/ub4 iters,
                         /*IN*/ub4 rowoff,
                         /*IN*/const OCISnapshot *pSnapIn,
                         /*OUT*/OCISnapshot *pSnapOut,
                         /*IN*/ub4 mode);

sword   OCIStmtPrepare   (/*IN*/OCIStmt *pStmtp,
                          /*IN*/OCIError *pErrorHandle,
                          /*IN*/const OraText *pStmt,
                          /*IN*/ub4 stmtLen,
                          /*IN*/ub4 language,
                          /*IN*/ub4 mode);

sword   OCIBindObject  (/*IN/OUT*/OCIBind *pBindp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIType *pType,
                        /*IN/OUT*/void  **pgvpp,
                        /*IN/OUT*/ub4 *pPvszsp,
                        /*IN/OUT*/void  **pIndpp,
                        /*IN/OUT*/ub4 *pIndszp);

sword   OCIBindDynamic   (/*IN/OUT*/OCIBind *pBindp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/void  *pIctxp,
                          /*IN*/OCICallbackInBind icbfp,
                          void  *pOctxp,
                          OCICallbackOutBind ocbfp);

sword   OCIStmtSetPieceInfo   (/*IN/OUT*/void  *pHndlp, ub4 type,
                               /*IN*/OCIError *pErrorHandle,
                               /*OUT*/const void  *pBufp,
                               /*IN/OUT*/ub4 *pAlenp,
                               /*IN*/ub1 piece,
                               /*IN/OUT*/const void  *pIndp,
                               /*IN/OUT*/ub2 *pRcodep);

sword   OCIDefineArrayOfStruct  (OCIDefine *pDefnp, /*IN/OUT*/OCIError *pErrorHandle, ub4 pvskip,
                                 ub4 indskip, ub4 rlskip, ub4 rcskip);

sword   OCIDefineObject  (OCIDefine *pDefnp, /*IN/OUT*/OCIError *pErrorHandle,
                          const OCIType *pType, void  **pPgvpp,
                          ub4 *pPvszsp, void  **pIndpp, ub4 *pIndszp);

sword   OCIDefineByPos  (/*IN*/OCIStmt *pStmtp,
                         /*IN/OUT*/OCIDefine **pDefnp,
                         /*IN*/OCIError *pErrorHandle,
                         /*IN*/ub4 position,
                         /*IN/OUT*/void  *pValuep,
                         /*IN*/sb4 valueSize,
                         /*IN*/ub2 dty,
                         /*IN*/void  *pIndp,
                         /*IN/OUT*/ub2 *pRlenp,
                         /*OUT*/ub2 *pRcodep,
                         /*IN*/ub4 mode);

sword   OCIDefineDynamic   (OCIDefine *pDefnp, /*IN/OUT*/OCIError *pErrorHandle, void  *pOctxp,
                            OCICallbackDefine ocbfp);

sword   OCIRowidToChar  (OCIRowid *pRowidDesc, OraText *pOutbfp, ub2 *pOutbflp,
                         OCIError *pErrorHandle);


sword   OCIStmtFetch   (OCIStmt *pStmtp, /*IN/OUT*/OCIError *pErrorHandle, ub4 nRows,
                        ub2 orientation, ub4 mode);

sword   OCIStmtFetch2   (OCIStmt *pStmtp, /*IN/OUT*/OCIError *pErrorHandle, ub4 nRows,
                        ub2 orientation, sb4 scrollOffset, ub4 mode);

sword   OCIStmtGetBindInfo   (OCIStmt *pStmtp, /*IN/OUT*/OCIError *pErrorHandle, ub4 size,
                              ub4 startloc,
                              sb4 *pFound, OraText *pBvnp[], ub1 bvnl[],
                              OraText *pInvp[], ub1 inpl[], ub1 dupl[],
                              OCIBind **pHndl);

sword   OCIDescribeAny  (/*IN/OUT*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle,
                         void  *pObjptr,
                         ub4 objnmLen, ub1 objptrTyp, ub1 infoLevel,
                         ub1 objtyp, OCIDescribe *pDschp);

sword   OCIParamGet (const void  *pHndlp, ub4 htype,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     void  **pParmdpp, ub4 pos);

sword   OCIParamSet(void  *pHdlp, ub4 htyp, /*IN/OUT*/OCIError *pErrorHandle, const void  *pDscp,
                    ub4 dtyp, ub4 pos);

sword   OCITransStart  (/*IN/OUT*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle,
                        uword timeout, ub4 flags);

sword   OCITransDetach  (/*IN/OUT*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle, ub4 flags);

sword   OCITransMultiPrepare (/*IN/OUT*/OCISvcCtx *pServiceContexthp, ub4 numBranches,
                              OCITrans **pTxns, /*IN/OUT*/OCIError **pErrorHandle);

sword   OCITransForget (/*IN/OUT*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle, ub4 flags);

sword   OCIErrorGet   (/*IN*/void  *pHndlp,
                       /*IN*/ub4 recordno,
                       /*OUT*/OraText *pSqlstate,
                       /*OUT*/sb4 *pErrcodep,
                       /*OUT*/OraText *pBufp,
                       /*IN*/ub4 bufsiz,
                       /*IN*/ub4 type);

sword   OCITransCommit  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle, ub4 flags);

sword   OCITransRollback  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                           /*IN/OUT*/OCIError *pErrorHandle, ub4 flags);

sword   OCITransPrepare (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle, ub4 flags);

///////////////////////////////////////////////////////////////////////////
//////////       L O B           F U N C T I O N S
sword OCILobCreateTemporary(OCISvcCtx          *pSvchp,
                            OCIError           *pErrorHandle,
                            OCILobLocator      *pLocp,
                            ub2                 csid,
                            ub1                 csfrm,
                            ub1                 lobtype,
                            boolean             cache,
                            OCIDuration         duration);

sword   OCILobCopy (/*IN/OUT*/OCISvcCtx *pServiceContexthp, /*IN/OUT*/OCIError *pErrorHandle, OCILobLocator *pDstLocp,
                    OCILobLocator *pSrcLocp, ub4 amount, ub4 dstOffset,
                    ub4 srcOffset);

sword   OCILobFileClose (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         OCILobLocator *pFilep);

sword   OCILobFileCloseAll (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            OCIError *pErrorHandle);

sword   OCILobFileExists (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCILobLocator *pFilep,
                          boolean *pFlag);

sword   OCILobFileGetName (/*IN/OUT*/OCIEnv *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           const OCILobLocator *pFilep,
                           OraText *pDirAlias, ub2 *pDLength,
                           OraText *pFilename, ub2 *pFLength);


sword   OCILobFileOpen (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCILobLocator *pFilep,
                        ub1 mode);

sword   OCILobAssign (/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                      const OCILobLocator *pSrcLocp,
                      OCILobLocator **pDstLocpp);

sword   OCILobFileIsOpen (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCILobLocator *pFilep,
                          boolean *pFlag);

sword   OCILobCharSetForm (/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                           const OCILobLocator *pLocp, ub1 *pCsfrm);

sword   OCILobCharSetId (/*IN/OUT*/OCIEnv *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle,
                         const OCILobLocator *pLocp, ub2 *pCsid);

sword   OCILobAppend  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       OCILobLocator *pDstLocp,
                       OCILobLocator *pSrcLocp);

sword OCILobClose(OCISvcCtx        *pSvchp,
                   OCIError         *pErrorHandle,
                   OCILobLocator    *pLocp);


sword   OCILobDisableBuffering (OCISvcCtx      *pSvchp,
                                OCIError       *pErrorHandle,
                                OCILobLocator  *pLocp);

sword   OCILobEnableBuffering (OCISvcCtx      *pSvchp,
                               OCIError       *pErrorHandle,
                               OCILobLocator  *pLocp);

sword   OCILobErase (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     OCILobLocator *pLocp,
                      ub4 *pAmount, ub4 offset);

sword   OCILobFileSetName (/*IN/OUT*/OCIEnv *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OCILobLocator **pFilepp,
                           const OraText *pDirAlias, ub2 dLength,
                           const OraText *pFilename, ub2 fLength);

sword OCILobGetChunkSize(OCISvcCtx         *pSvchp,
                         OCIError          *pErrorHandle,
                         OCILobLocator     *pLocp,
                         ub4               *pChunksizep);

sword OCILobIsOpen(OCISvcCtx     *pSvchp,
                    OCIError      *pErrorHandle,
                    OCILobLocator *pLocp,
                    boolean       *pFlag);

sword OCILobIsTemporary(/*IN/OUT*/OCIEnv  *pEnv,
                        OCIError          *pErrorHandle,
                        OCILobLocator     *pLocp,
                        boolean           *pIsTemporary);

sword   OCILobOpen(OCISvcCtx        *pSvchp,
                   OCIError         *pErrorHandle,
                   OCILobLocator    *pLocp,
                   ub1               mode);

sword   OCILobRead  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     OCILobLocator *pLocp,
                     ub4 *pAmtp, ub4 offset,
                     void  *pBufp, ub4 bufl,
                     void  *pCtxp,
                     OCICallbackLobRead cbfp,
                     ub2 csid, ub1 csfrm);

sword   OCILobLoadFromFile (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            OCILobLocator *pDstLocp,
                            OCILobLocator *pSrcFilep,
                            ub4 amount, ub4 dstOffset,
                            ub4 srcOffset);

sword   OCILobLocatorAssign  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              const OCILobLocator *pSrcLocp,
                              OCILobLocator **pDstLocpp);


sword   OCILobLocatorIsInit (/*IN/OUT*/OCIEnv *pEnvironment,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             const OCILobLocator *pLocp,
                             boolean *pIsInitialized);


sword   OCILobGetLength  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCILobLocator *pLocp,
                          ub4 *pLenp);

sword   OCILobFlushBuffer (OCISvcCtx       *pSvchp,
                           OCIError        *pErrorHandle,
                           OCILobLocator   *pLocp,
                           ub4              flag);

sword OCILobFreeTemporary(OCISvcCtx          *pSvchp,
                          OCIError           *pErrorHandle,
                          OCILobLocator      *pLocp);

sword   OCILobIsEqual  (/*IN/OUT*/OCIEnv *pEnvironment,
                        const OCILobLocator *x,
                        const OCILobLocator *y,
                        boolean *pIsEqual);

sword   OCILobTrim  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     OCILobLocator *pLocp,
                     ub4 newLen);

sword   OCILobWrite  (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                      /*IN/OUT*/OCIError *pErrorHandle, OCILobLocator *pLocp,
                      ub4 *pAmtp, ub4 offset, void  *pBufp, ub4 bufLen,
                      ub1 piece,  void  *pCtxp, OCICallbackLobWrite cbfp,
                      ub2 csid, ub1 csfrm);

sword OCILobGetDeduplicateRegions(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                                  /*IN/OUT*/OCIError *pErrorHandle,
                                  OCILobLocator *pLocp,
                                  OCILobRegion *pRegp, ub4 *pCount, ub1 piece,
                                  void *pCtxp,
                                  OCICallbackLobGetDeduplicateRegions cbfp);

sword OCILobWriteAppend(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle, OCILobLocator *pLobp,
                        ub4 *pAmtp, void  *pBufp, ub4 bufl, ub1 piece,
                        void *pCtxp, OCICallbackLobWrite cbfp, ub2 csid,
                        ub1 csfrm);

sword   OCIBreak (void  *pHndlp, /*IN/OUT*/OCIError *pErrorHandle);

sword   OCIReset (void  *pHndlp, /*IN/OUT*/OCIError *pErrorHandle);

sword   OCIServerVersion  (void  *pHndlp,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OraText *pBufp,
                           ub4 bufsz,
                           ub1 hndltype);

sword   OCIServerRelease  (void  *pHndlp,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OraText *pBufp,
                           ub4 bufsz,
                           ub1 hndltype, ub4 *pVersion);


sword   OCILdaToSvcCtx (/*IN/OUT*/OCISvcCtx **pSvchpp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        Lda_Def *pLdap);

sword   OCIResultSetToStmt (OCIResult *pRsetdp,
                            OCIError *pErrorHandle);

sword   OCIAttrGet (const void  *pTrgthndlp, ub4 trghndltyp,
                    void  *pAttributep, ub4 *pSizep, ub4 attrtype,
                    OCIError *pErrorHandle);

sword   OCIAttrSet (void  *pTrgthndlp, ub4 trghndltyp, void  *pAttributep,
                    ub4 size, ub4 attrtype, /*IN/OUT*/OCIError *pErrorHandle);

sword   OCISvcCtxToLda (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        Lda_Def *pLdap);

sword   OCIUserCallbackRegister(void  *pHndlp,
                                ub4 type,
                                void  *pEhndlp,
                                OCIUserCallback callback,
                                void  *pCtxp,
                                ub4 fcode, ub4 when,
                                OCIUcb *pUcbDesc);

sword   OCIUserCallbackGet(void  *pHndlp, ub4 type,
                           void  *pEhndlp,
                           ub4 fcode, ub4 when,
                           OCIUserCallback *pCallbackp,
                           void  **pCtxpp, OCIUcb *pUcbDesc);

sword   OCISharedLibInit(void  *pMetaCtx, void  *pLibCtx, ub4 argfmt, sword argc,
                         void  **pArgv, OCIEnvCallbackType envCallback);

///////////////////////////////////////////////////////////////////////////
//////////       F I L E          F U N C T I O N S
sword OCIFileClose (/*IN*/void   *pHndl,
                    /*OUT*/OCIError *pErrorHandle,
                    /*IN*/OCIFileObject *pFilep);

sword OCIFileExists (/*IN*/void   *pHndl,
                     /*OUT*/OCIError *pErrorHandle,
                     /*IN*/OraText *pFilename,
                     /*OUT*/OraText *pPath, ub1 *pFlag );

sword OCIFileGetLength(/*IN*/void  *pEnvironment,
                       /*OUT*/OCIError *pErrorHandle,
                       /*IN*/OraText *pFilename,
                       /*IN*/OraText *pPath,
                       /*OUT*/ubig_ora *pLenp );

sword OCIFileInit (/*IN*/void  *pEnvironment,
                   /*OUT*/OCIError *pErrorHandle);

sword OCIFileWrite (/*IN*/void  *pEnvironment,
                    /*OUT*/OCIError *pErrorHandle,
                    /*IN/OUT*/OCIFileObject   *pFilep,
                    /*IN*/void  *pBufp, ub4 bufLen,
                    /*OUT*/ub4 *pBytesWritten);

sword OCIFileOpen (/*OUT*/void  *pEnvironment,
                   /*OUT*/OCIError *pErrorHandle,
                   /*OUT*/OCIFileObject **pFilep,
                   /*IN*/OraText *pFilename,
                   /*IN*/OraText *pPath, ub4 mode,
                   /*IN*/ub4 create,
                   /*IN*/ub4 type);

sword OCIFileRead (/*IN*/void  *pEnvironment,
                   /*OUT*/OCIError *pErrorHandle,
                   /*IN/OUT*/OCIFileObject *pFilep,
                   /*IN*/void  *pBufp,
                   /*IN*/ub4 bufl,
                   /*OUT*/ub4 *pBytesread);

sword OCIFileSeek (/*IN*/void  *pEnvironment,
                   /*OUT*/OCIError *pErrorHandle,
                   /*IN/OUT*/OCIFileObject *pFilep,
                   uword origin, ubig_ora offset, sb1 dir);

sword OCIFileTerm (/*IN*/void  *pEnvironment,
                   /*OUT*/OCIError *pErrorHandle);

sword OCIFileFlush(/*IN*/void  *pEnvironment,
                   /*OUT*/OCIError *pErrorHandle,
                   /*IN*/OCIFileObject *pFilep );


#ifdef ORAXB8_DEFINED

sword   OCILobErase2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      OCILobLocator *pLocp,
                      oraub8 *pAmount, oraub8 offset);


sword OCILobGetOptions (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCILobLocator *pLobp,
                        ub4 optypes, void *pOptionsp,
                        ub4 *pOptionslenp,
                        ub4 mode);

sword OCILobSetOptions (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCILobLocator *pLobp,
                        ub4 optypes, void *pOptionsp,
                        ub4 optionsLen,
                        ub4 mode);

sword   OCILobGetLength2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCILobLocator *pLocp,
                          oraub8 *pLenp);

sword   OCILobCopy2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     OCILobLocator *pDstLocp,
                     OCILobLocator *pSrcLocp, oraub8 amount,
                     oraub8 dstOffset,
                     oraub8 srcOffset);

sword   OCILobRead2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     OCILobLocator *pLocp,
                     oraub8 *pByteAmtp,
                     oraub8 *pCharAmtp, oraub8 offset,
                     void  *pBufp, oraub8 bufl,
                     ub1 piece, void  *pCtxp,
                     OCICallbackLobRead2 cbfp,
                     ub2 csid, ub1 csfrm);

sword   OCILobLoadFromFile2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             OCILobLocator *pDstLocp,
                             OCILobLocator *pSrcFilep,
                             oraub8 amount, oraub8 dstOffset,
                             oraub8 srcOffset);

sword OCILobWriteAppend2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCILobLocator *pLobp,
                          oraub8 *pByteAmtp, oraub8 *pCharAmtp, void  *pBufp,
                          oraub8 bufl, ub1 piece, void  *pCtxp,
                          OCICallbackLobWrite2 cbfp, ub2 csid, ub1 csfrm);

sword OCILobGetStorageLimit (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             OCILobLocator *pLobp,
                             oraub8 *pLimitp);

sword   OCILobTrim2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     OCILobLocator *pLocp,
                     oraub8 newLen);

sword   OCILobWrite2 (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                      /*IN/OUT*/OCIError *pErrorHandle, OCILobLocator *pLocp,
                      oraub8 *pByteAmtp, oraub8 *pCharAmtp, oraub8 offset,
                      void  *pBufp, oraub8 bufLen, ub1 piece, void  *pCtxp,
                      OCICallbackLobWrite2 cbfp, ub2 csid, ub1 csfrm);

sword   OCILobArrayWrite (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle, ub4 *pArrayIter,
                          OCILobLocator **pLobpArr, oraub8 *pByteAmtArr,
                          oraub8 *pCharAmtArr, oraub8 *pOffsetArr,
                          void  **pBufpArr, oraub8 *pBuflArr, ub1 piece,
                          void  *pCtxp, OCICallbackLobArrayWrite cbfp, ub2 csid,
                          ub1 csfrm);

sword   OCILobArrayRead (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle, ub4 *pArrayIter,
                         OCILobLocator **pLobpArr, oraub8 *pByteAmtArr,
                         oraub8 *pCharAmtArr, oraub8 *pOffsetArr,
                         void  **pBufpArr, oraub8 *pBuflArr,
                         ub1 piece, void  *pCtxp,
                         OCICallbackLobArrayRead cbfp, ub2 csid,
                         ub1 csfrm);

#endif
///////////////////////////////////////////////////////////////////////////
//////////       S E C U R I T Y           F U N C T I O N S

sword OCISecurityStorePersona(OCISecurity *pOsshandle,
                              OCIError *pErrorHandle,
                              nzttPersona **pPersona,
                              nzttWallet *pWallet);

sword OCISecurityOpenPersona(OCISecurity *pOsshandle,
                             OCIError *pErrorHandle,
                             nzttPersona *pPersona);

sword OCISecurityClosePersona(OCISecurity *pOsshandle,
                              OCIError *pErrorHandle,
                              nzttPersona *pPersona);

sword OCISecurityRemovePersona(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttPersona **pPersona);

sword OCISecurityCreatePersona(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttIdentType identityType,
                               nzttCipherType cipherType,
                               nzttPersonaDesc *pDesc,
                               nzttPersona **pPersona);
sword   OCISecurityInitialize (OCISecurity *pSechandle, /*IN/OUT*/OCIError *pErrorHandle);

sword   OCISecurityTerminate (OCISecurity *pSechandle, /*IN/OUT*/OCIError *pErrorHandle);

sword OCISecurityOpenWallet(OCISecurity *pOsshandle,
                            OCIError *pErrorHandle,
                            size_t wrlLen,
                            OraText *pWalletResourceLocator,
                            size_t pwdLen,
                            OraText *pPassword,
                            nzttWallet *pWallet);

sword OCISecurityCloseWallet(OCISecurity *pOsshandle,
                             OCIError *pErrorHandle,
                             nzttWallet *pWallet);

sword OCISecurityCreateWallet(OCISecurity *pOsshandle,
                              OCIError *pErrorHandle,
                              size_t wrlLen,
                              OraText *pWalletResourceLocator,
                              size_t pwdLen,
                              OraText *pPassword,
                              nzttWallet *pWallet);

sword OCISecurityDestroyWallet(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               size_t wrlLen,
                               OraText *pWalletResourceLocator,
                               size_t pwdLen,
                               OraText *pPassword);


sword OCISecuritySetProtection(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttPersona *pPersona,
                               nzttcef cryptoEngineFunction,
                               nztttdufmt dataUnitFormat,
                               nzttProtInfo *pRotectionInfo);

sword OCISecurityGetProtection(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttPersona *pPersona,
                               nzttcef cryptoEngineFunction,
                               nztttdufmt *pDataUnitFormat,
                               nzttProtInfo *pRotectionInfo);

sword OCISecurityRemoveIdentity(OCISecurity *pOsshandle,
                                OCIError *pErrorHandle,
                                nzttIdentity **pIdentity);

sword OCISecurityCreateIdentity(OCISecurity *pOsshandle,
                                OCIError *pErrorHandle,
                                nzttIdentType type,
                                nzttIdentityDesc *pDesc,
                                nzttIdentity **pIdentity);

sword OCISecurityAbortIdentity(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttIdentity **pIdentity);

sword OCISecurityFreeIdentity(OCISecurity *pOsshandle,
                              OCIError *pErrorHandle,
                              nzttIdentity **pIdentity);


sword OCISecurityStoreTrustedIdentity(OCISecurity *pOsshandle,
                                      OCIError *pErrorHandle,
                                      nzttIdentity **pIdentity,
                                      nzttPersona *pPersona);

sword OCISecuritySign(OCISecurity *pOsshandle,
                      OCIError *pErrorHandle,
                      nzttPersona *pPersona,
                      nzttces signatureState,
                      size_t inputLength,
                      ub1 *pInput,
                      nzttBufferBlock *pBufferBlock);

sword OCISecuritySignExpansion(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttPersona *pPersona,
                               size_t inputLen,
                               size_t *pSignatureLength);

sword OCISecurityVerify(OCISecurity *pOsshandle,
                        OCIError *pErrorHandle,
                        nzttPersona *pPersona,
                        nzttces signatureState,
                        size_t sigLen,
                        ub1 *pSignature,
                        nzttBufferBlock *pExtractedMessage,
                        boolean *pVerified,
                        boolean *pValidated,
                        nzttIdentity **pSigningPartyIdentity);

sword OCISecurityValidate(OCISecurity *pOsshandle,
                          OCIError *pErrorHandle,
                          nzttPersona *pPersona,
                          nzttIdentity *pIdentity,
                          boolean *pValidated);

sword OCISecuritySignDetached(OCISecurity *pOsshandle,
                              OCIError *pErrorHandle,
                              nzttPersona *pPersona,
                              nzttces signatureState,
                              size_t inputLength,
                              ub1 *pInput,
                              nzttBufferBlock *pSignature);

sword OCISecuritySignDetExpansion(OCISecurity *pOsshandle,
                                  OCIError *pErrorHandle,
                                  nzttPersona *pPersona,
                                  size_t inputLength,
                                  size_t *pRequiredbUfferLength);

sword OCISecurityVerifyDetached(OCISecurity *pOsshandle,
                                OCIError *pErrorHandle,
                                nzttPersona *pPersona,
                                nzttces signatureState,
                                size_t dataLength,
                                ub1 *pData,
                                size_t sigLen,
                                ub1 *pSignature,
                                boolean *pVerified,
                                boolean *pValidated,
                                nzttIdentity **pSigningPartyIdentity);

sword OCISecurity_PKEncrypt(OCISecurity *pOsshandle,
                            OCIError *pErrorHandle,
                            nzttPersona *pPersona,
                            size_t numberOfRecipients,
                            nzttIdentity *pRecipientList,
                            nzttces encryptionState,
                            size_t inputLength,
                            ub1 *pInput,
                            nzttBufferBlock *pEncryptedData);

sword OCISecurityPKEncryptExpansion(OCISecurity *pOsshandle,
                                    OCIError *pErrorHandle,
                                    nzttPersona *pPersona,
                                    size_t numberRecipients,
                                    size_t inputLength,
                                    size_t *pBufferLengthRequired);

sword OCISecurityPKDecrypt(OCISecurity *pOsshandle,
                           OCIError *pErrorHandle,
                           nzttPersona *pPersona,
                           nzttces encryptionState,
                           size_t inputLength,
                           ub1 *pInput,
                           nzttBufferBlock *pEncryptedData);

sword OCISecurityEncrypt(OCISecurity *pOsshandle,
                         OCIError *pErrorHandle,
                         nzttPersona *pPersona,
                         nzttces encryptionState,
                         size_t inputLength,
                         ub1 *pInput,
                         nzttBufferBlock *pEncryptedData);

sword OCISecurityEncryptExpansion(OCISecurity *pOsshandle,
                                  OCIError *pErrorHandle,
                                  nzttPersona *pPersona,
                                  size_t inputLength,
                                  size_t *pEncryptedDataLength);

sword OCISecurityDecrypt(OCISecurity *pOsshandle,
                         OCIError *pErrorHandle,
                         nzttPersona *pPersona,
                         nzttces decryptionState,
                         size_t inputLength,
                         ub1 *pInput,
                         nzttBufferBlock *pDecryptedData);

sword OCISecurityEnvelope(OCISecurity *pOsshandle,
                          OCIError *pErrorHandle,
                          nzttPersona *pPersona,
                          size_t numberOfRecipients,
                          nzttIdentity *pIdentity,
                          nzttces encryptionState,
                          size_t inputLength,
                          ub1 *pInput,
                          nzttBufferBlock *pEnvelopedData);

sword OCISecurityDeEnvelope(OCISecurity *pOsshandle,
                            OCIError *pErrorHandle,
                            nzttPersona *pPersona,
                            nzttces decryptionState,
                            size_t inputLength,
                            ub1 *pInput,
                            nzttBufferBlock *pOutputMessage,
                            boolean *pVerified,
                            boolean *pValidated,
                            nzttIdentity **pSenderIdentity);

sword OCISecurityKeyedHash(OCISecurity *pOsshandle,
                           OCIError *pErrorHandle,
                           nzttPersona *pPersona,
                           nzttces hashState,
                           size_t inputLength,
                           ub1 *pInput,
                           nzttBufferBlock *pKeyedHash);

sword OCISecurityKeyedHashExpansion(OCISecurity *pOsshandle,
                                    OCIError *pErrorHandle,
                                    nzttPersona *pPersona,
                                    size_t inputLength,
                                    size_t *pRequiredbUfferLength);

sword OCISecurityHash(OCISecurity *pOsshandle,
                      OCIError *pErrorHandle,
                      nzttPersona *pPersona,
                      nzttces hashState,
                      size_t input,
                      ub1 *pInputLength,
                      nzttBufferBlock *pHash);

sword OCISecurityHashExpansion(OCISecurity *pOsshandle,
                               OCIError *pErrorHandle,
                               nzttPersona *pPersona,
                               size_t inputLength,
                               size_t *pRequiredbUfferLength);

sword OCISecuritySeedRandom(OCISecurity *pOsshandle,
                            OCIError *pErrorHandle,
                            nzttPersona *pPersona,
                            size_t seedLength,
                            ub1 *pSeed);

sword OCISecurityRandomBytes(OCISecurity *pOsshandle,
                             OCIError *pErrorHandle,
                             nzttPersona *pPersona,
                             size_t numberOfBytesDesired,
                             nzttBufferBlock *pRandomBytes);

sword OCISecurityRandomNumber(OCISecurity *pOsshandle,
                              OCIError *pErrorHandle,
                              nzttPersona *pPersona,
                              uword *pRandomNumber);

sword OCISecurityInitBlock(OCISecurity *pOsshandle,
                           OCIError *pErrorHandle,
                           nzttBufferBlock *pBufferBlock);

sword OCISecurityReuseBlock(OCISecurity *pOsshandle,
                            OCIError *pErrorHandle,
                            nzttBufferBlock *pBufferBlock);

sword OCISecurityPurgeBlock(OCISecurity *pOsshandle,
                            OCIError *pErrorHandle,
                            nzttBufferBlock *pBufferBlock);

sword OCISecuritySetBlock(OCISecurity *pOsshandle,
                          OCIError *pErrorHandle,
                          uword flagsToSet,
                          size_t bufferLength,
                          size_t usedbUfferLength,
                          ub1 *pBuffer,
                          nzttBufferBlock *pBufferBlock);

sword OCISecurityGetIdentity(OCISecurity   *pOsshandle,
                             OCIError      *pErrorHandle,
                             size_t         nameLen,
                             OraText       *pDistinguishedName,
                             nzttIdentity **pIdentity);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
sword OCIExtractInit(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle);

sword OCIAQDeqArray(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                    /*IN/OUT*/OCIError *pErrorHandle,
                    OraText *pQueueName,
                    OCIAQDeqOptions *pDeqopt,
                    ub4 *pIters,
                    OCIAQMsgProperties **pMsgprop,
                    OCIType *pPayloadTdo,
                    void  **pPayload,
                    void  **pPayloadInd,
                    OCIRaw **pMsgid,
                    void  *pCtxp,
                    OCICallbackAQDeq deqcbfp,
                    ub4 flags);

sword OCIAQDeq(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
               /*IN/OUT*/OCIError *pErrorHandle,
               OraText *pQueueName,
               OCIAQDeqOptions *pDeqopt,
               OCIAQMsgProperties *pMsgprop,
               OCIType *pPayloadTdo,
               void  **pPayload,
               void  **pPayloadInd,
               OCIRaw **pMsgid, ub4 flags);

sword OCIAQEnqArray(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                    /*IN/OUT*/OCIError *pErrorHandle,
                    OraText *pQueueName,
                    OCIAQEnqOptions *pEnqopt,
                    ub4 *pIters,
                    OCIAQMsgProperties **pMsgprop,
                    OCIType *pPayloadTdo,
                    void  **pPayload,
                    void  **pPayloadInd,
                    OCIRaw **pMsgid,
                    void  *pCtxp, OCICallbackAQEnq enqcbfp,
                    ub4 flags);

sword OCIAQEnq(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
               /*IN/OUT*/OCIError *pErrorHandle,
               OraText *pQueueName,
               OCIAQEnqOptions *pEnqopt,
               OCIAQMsgProperties *pMsgprop,
               OCIType *pPayloadTdo,
               void  **pPayload,
               void  **pPayloadInd,
               OCIRaw **pMsgid, ub4 flags);

sword OCIAQEnqStreaming(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OraText *pQueueName,
                        OCIAQEnqOptions *pEnqopt,
                        OCIType *pPayloadTdo,
                        void  *pCtxp,
                        OCICallbackAQEnqStreaming enqcbfp,
                        ub4 flags);

sword OCIAQListen2(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                   /*IN/OUT*/OCIError *pErrorHandle,
                   OCIAQAgent **pAgentList,
                   ub4 numAgents,
                   OCIAQListenOpts  *pLopts,
                   OCIAQAgent **pAgent,
                   OCIAQLisMsgProps *pLmops,
                   ub4 flags);

sword OCIAQGetReplayInfo(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         OraText *pQueueName,
                         OCIAQAgent *pSender,
                         ub4 replayAttribute,
                         OraText *pCorrelation,
                         ub2 *pCorrLen);

sword OCIAQListen(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                  /*IN/OUT*/OCIError *pErrorHandle,
                  OCIAQAgent **pAgentList,
                  ub4 numAgents,
                  sb4 wait,
                  OCIAQAgent **pAgent,
                  ub4 flags);

sword OCIAQResetReplayInfo(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OraText *pQueueName,
                           OCIAQAgent *pSender,
                           ub4 replayAttribute);

///////////////////////////////////////////////////////////////////////////
sword OCIExtractReset(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle);

sword OCIExtractSetNumKeys(/*IN/OUT*/void  *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/uword numkeys);

sword OCIExtractFromStr(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/ub4 flag,
                        /*IN*/OraText *pInput);

sword OCIExtractToInt(/*IN*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle, /*IN*/OraText *pKeyName,
                      /*IN*/uword valno,
                      /*OUT*/sb4 *pRetval);

sword OCIExtractSetKey(/*IN/OUT*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OraText *pName,
                       /*IN*/ub1 type, ub4 flag,
                       /*IN*/const void  *pDefval,
                       /*IN*/const sb4 *pIntrange,
                       /*IN*/const OraText *const *pStrlist);

sword OCIExtractFromFile(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/ub4 flag,
                         /*IN*/OraText *pFilename);

sword OCIExtractFromList(/*IN*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*OUT*/uword index,
                         /*OUT*/OraText **pName,
                         /*OUT*/ub1 *pType,
                         /*OUT*/uword *pNumvals,
                         /*OUT*/void  ***pValues);

sword OCIExtractToStr(/*IN*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/OraText *pKeyName,
                      /*IN*/uword valno,
                      /*OUT*/OraText *pRetval,
                      /*IN*/uword bufLen);

sword OCIExtractToBool(/*IN*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/OraText *pKeyName,
                       /*IN*/uword valno,
                       /*OUT*/ub1 *pRetval);

sword OCIExtractToOCINum(/*IN*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OraText *pKeyName,
                         /*IN*/uword valno,
                         /*OUT*/OCINumber *pRetval);

sword OCIExtractToList(/*IN*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*OUT*/uword *pNumkeys);

sword OCIExtractTerm(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle);

////////////////////////////////////////////////////////////////////////////////////////
sword OCIMemoryResize(void  *pHdl,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      void  **pMem,
                      ub4 newSize,
                      ub4 flags);

sword OCIMemoryAlloc(void  *pHdl,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     void  **pMem,
                     OCIDuration dur, ub4 size,
                     ub4 flags);

sword OCIMemoryFree(void  *pHdl, /*IN/OUT*/OCIError *pErrorHandle,
                    void  *pMem);

///////////////////////////////////////////////////////////////////////////////////////
sword OCIContextClearValue(void  *pHdl,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           ub1 *pKey,
                           ub1 keyLen);

sword OCIContextGenerateKey(void  *pHdl,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            ub4 *pKey);

sword OCIContextSetValue(void  *pHdl, /*IN/OUT*/OCIError *pErrorHandle,
                         OCIDuration duration,
                         ub1 *pKey, ub1 keyLen,
                         void  *pCtxValue);

sword OCIContextGetValue(void  *pHdl,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         ub1 *pKey,
                         ub1 keyLen,
                         void  **pCtxValue);

sword OCIMemorySetCurrentIDs(void  *pHdl,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             ub4 currSessionId,
                             ub4 currTransId,
                             ub4 currStmtId);

///////////////////////////////////////////////////////////
sword OCIPicklerTdsCtxInit(/*IN/OUT*/OCIEnv *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OCIPicklerTdsCtx **pTdsc);

sword OCIPicklerTdsInit(/*IN/OUT*/OCIEnv *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIPicklerTdsCtx *pTdsc,
                        OCIPicklerTds **pTdsh);

sword OCIPicklerTdsCreateElementChar(/*IN/OUT*/OCIEnv *pEnvironment,
                                     /*IN/OUT*/OCIError *pErrorHandle,
                                     OCIPicklerTds *pTdsh, ub2 Len,
                                     OCIPicklerTdsElement *pElt);

sword OCIPicklerTdsCreateElementVarchar(/*IN/OUT*/OCIEnv *pEnvironment,
                                        /*IN/OUT*/OCIError *pErrorHandle,
                                        OCIPicklerTds *pTdsh, ub2 Len,
                                        OCIPicklerTdsElement *pElt);

sword OCIPicklerTdsFree(/*IN/OUT*/OCIEnv *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIPicklerTds *pTdsh);

sword OCIPicklerTdsCreateElementNumber(/*IN/OUT*/OCIEnv *pEnvironment,
                                       /*IN/OUT*/OCIError *pErrorHandle,
                                       OCIPicklerTds *pTdsh, ub1 prec,
                                       sb1 scale, OCIPicklerTdsElement *pElt);

sword OCIPicklerTdsCtxFree(/*IN/OUT*/OCIEnv *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OCIPicklerTdsCtx *pTdsc);

sword OCIPicklerImageCollAddScalar(/*IN/OUT*/OCIEnv *pEnvironment,
                                   /*IN/OUT*/OCIError *pErrorHandle,
                                   OCIPicklerImage *pImgh, void  *pScalar,
                                   ub4 bufLen, OCIInd ind);

sword OCIPicklerImageCollEnd(/*IN/OUT*/OCIEnv *pEnvironment,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             OCIPicklerImage *pImgh);

sword OCIPicklerTdsCreateElementRaw(/*IN/OUT*/OCIEnv *pEnvironment,
                                    /*IN/OUT*/OCIError *pErrorHandle,
                                    OCIPicklerTds *pTdsh,
                                    ub2 Len,
                                    OCIPicklerTdsElement *pElt);

sword OCIPicklerTdsCreateElement(/*IN/OUT*/OCIEnv *pEnvironment,
                                 /*IN/OUT*/OCIError *pErrorHandle,
                                 OCIPicklerTds *pTdsh, OCITypeCode dty,
                                 OCIPicklerTdsElement *pElt);

sword OCIPicklerTdsAddAttr(/*IN/OUT*/OCIEnv *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           OCIPicklerTds *pTdsh, OCIPicklerTdsElement elt);

sword OCIPicklerTdsGenerate(/*IN/OUT*/OCIEnv *pEnvironment,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            OCIPicklerTds *pTdsh);

sword OCIPicklerTdsGetAttr(/*IN/OUT*/OCIEnv *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           const OCIPicklerTds *pTdsh, ub1  attrno,
                           OCITypeCode *pTyp, ub2  *pLen);

sword OCIPicklerFdoInit(/*IN/OUT*/OCIEnv *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                                OCIPicklerFdo **pFdoh);

sword OCIPicklerFdoFree(/*IN/OUT*/OCIEnv *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIPicklerFdo *pFdoh);

sword OCIPicklerImageInit(/*IN/OUT*/OCIEnv *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCIPicklerFdo *pFdoh,
                          OCIPicklerTds *pTdsh,
                          OCIPicklerImage **pImgh);

sword OCIPicklerImageFree(/*IN/OUT*/OCIEnv *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                                OCIPicklerImage *pImgh);

sword OCIPicklerImageAddScalar(/*IN/OUT*/OCIEnv *pEnvironment,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               OCIPicklerImage *pImgh,
                               void  *pScalar, ub4  len);

sword OCIPicklerImageGenerate(/*IN/OUT*/OCIEnv *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                                OCIPicklerImage *pImgh);

sword OCIPicklerImageGetScalarSize(/*IN/OUT*/OCIEnv *pEnvironment,
                                   /*IN/OUT*/OCIError *pErrorHandle,
                                OCIPicklerImage *pImgh,
                                ub4  attrno, ub4  *pSize);

sword OCIPicklerImageGetScalar(/*IN/OUT*/OCIEnv *pEnvironment,
                               /*IN/OUT*/OCIError *pErrorHandle,
                                OCIPicklerImage *pImgh, ub4  attrno,
                                void  *pBuffer, ub4  *pLen, OCIInd *pInd);

sword OCIPicklerImageAddNullScalar(/*IN/OUT*/OCIEnv *pEnvironment,
                                   /*IN/OUT*/OCIError *pErrorHandle,
                                OCIPicklerImage *pImgh);

sword OCIPicklerImageCollBegin(/*IN/OUT*/OCIEnv *pEnvironment,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               OCIPicklerImage *pImgh,
                               const OCIPicklerTds *pColltdsh);

sword OCIPicklerImageCollBeginScan(/*IN/OUT*/OCIEnv *pEnvironment,
                                   /*IN/OUT*/OCIError *pErrorHandle,
                                   OCIPicklerImage *pImgh,
                                   const OCIPicklerTds *pCollTdsh,
                                   ub4 attrnum, ub4 startidx,
                                   OCIInd *pInd);

sword OCIPicklerImageCollGetScalarSize(/*IN/OUT*/OCIEnv *pEnvironment,
                                       /*IN/OUT*/OCIError *pErrorHandle,
                                       const OCIPicklerTds *pCollTdsh,
                                       ub4 *pSize);

sword OCIPicklerImageCollGetScalar(/*IN/OUT*/OCIEnv *pEnvironment,
                                   /*IN/OUT*/OCIError *pErrorHandle,
                                   OCIPicklerImage *pImgh, void  *pBuffer,
                                   ub4 *pBufferLen, OCIInd *pInd);

/////////////////////////////////////////////////////////////////////
sword OCIAnyDataDestroy(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIAnyData *pSdata);

sword OCIAnyDataCollAddElem(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            OCIAnyData *pSdata,
                            OCITypeCode typeCode,
                            OCIType *pType,
                            void  *pInd,
                            void  *pAttrVal,
                            ub4 length,
                            boolean isAny,
                            boolean lastElem);

sword OCIAnyDataAttrSet(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIAnyData *pSdata,
                        OCITypeCode typeCode,
                        OCIType *pType,
                        void  *pInd, void  *pAttrVal,
                        ub4 length, boolean isAny);

sword OCIAnyDataIsNull(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       OCIAnyData *pSdata,
                       boolean *pIsnull);

sword OCIAnyDataConvert(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCITypeCode typeCode,
                        OCIType *pType, OCIDuration dur,
                        void  *pInd, void  *pDataVal,
                        ub4 Len, OCIAnyData **pSdata);

sword OCIAnyDataBeginCreate(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            OCITypeCode typeCode,
                            OCIType *pType, OCIDuration dur,
                            OCIAnyData **pSdata);

sword OCIAnyDataGetType(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIAnyData *pSdata,
                        OCITypeCode *pTc,
                        OCIType **pType);

sword OCIAnyDataEndCreate(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCIAnyData *pSdata);

sword OCIAnyDataAccess(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       OCIAnyData *pSdata,
                       OCITypeCode typeCode,
                       OCIType *pType,
                       void  *pInd,
                       void  *pAttrVal,
                       ub4 *pLength);

sword OCIAnyDataCollGetElem(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            OCIAnyData *pSdata,
                            OCITypeCode typeCode,
                            OCIType *pType,
                            void  *pInd,
                            void  *pCelemVal,
                            ub4 *pLength,
                            boolean isAny);

sword OCIAnyDataSetBeginCreate(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               /*IN*/OCITypeCode typeCode,
                               /*IN*/const OCIType *pType,
                               /*IN*/OCIDuration duration,
                               /*OUT*/OCIAnyDataSet ** pDataSet);


sword OCIAnyDataSetDestroy(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                           /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN/OUT*/OCIAnyDataSet *pDataSet);

sword OCIAnyDataSetGetType (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN*/OCIAnyDataSet *pDataSet,
                            /*OUT*/OCITypeCode *pTc,
                            /*OUT*/OCIType **pType);

sword OCIAnyDataSetGetCount(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN*/OCIAnyDataSet *pDataSet,
                            /*OUT*/ub4 *pCount);

sword OCIAnyDataGetCurrAttrNum(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               OCIAnyData *pSdata, ub4 *pAttrnum);

sword OCIAnyDataAttrGet(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        OCIAnyData *pSdata,
                        OCITypeCode typeCode,
                        OCIType *pType,
                        void  *pInd,
                        void  *pAttrVal,
                        ub4 *pLength,
                        boolean isAny);

sword OCIAnyDataSetGetInstance(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               /*IN*/OCIAnyDataSet *pDataSet,
                               /*IN/OUT*/OCIAnyData **pData);

sword OCIAnyDataSetAddInstance(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               /*IN/OUT*/OCIAnyDataSet *pDataSet,
                               /*IN/OUT*/OCIAnyData **pData);

sword OCIAnyDataSetEndCreate(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN/OUT*/OCIAnyDataSet *pDataSet);

///////////////////////////////////////////////////////////////////////////
sword OCIFormatTerm(/*IN/OUT*/void  *pEnvironment,
                    OCIError *pErrorHandle);

sword OCIFormatInit(/*IN/OUT*/void  *pEnvironment, /*IN/OUT*/OCIError *pErrorHandle);

sword OCIFormatString(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      OraText *pBuffer,
                      sbig_ora bufferLength,
                      sbig_ora *pReturnLength,
                      const OraText *pFormatString, ...);

sword OCIFormatTEb2(void);
sword OCIFormatTSb1(void);
sword OCIFormatTUb2(void);
sword OCIFormatTSb4(void);
sword OCIFormatTUbig_ora(void);
sword OCIFormatTText(void);
sword OCIFormatTSword(void);
sword OCIFormatTSbig_ora(void);
sword OCIFormatTEword(void);
sword OCIFormatTEb1(void);
sword OCIFormatTEb4(void);
sword OCIFormatTDouble(void);
sword OCIFormatTDvoid(void);
sword OCIFormatTEnd(void);
sword OCIFormatTChar(void);
sword OCIFormatTUb1(void);
sword OCIFormatTSb2(void);
sword OCIFormatTUb4(void);
sword OCIFormatTUword(void);

sword OCINlsGetInfo(/*IN/OUT*/void  *pEnvironment,
                    /*IN/OUT*/OCIError *pErrorHandle,
                    /*OUT*/OraText *pBuffer,
                    /*IN*/size_t bufferLen,
                    /*IN*/ub2 item);

OCISvcCtx *xaoSvcCtx(/*IN*/OraText *pDbName);

OCISvcCtx *xaosvch(/*IN*/OraText *pDbName);

int xaosterr(/*IN/OUT*/OCISvcCtx *pServiceContexth,
             sb4 errCode);

OCIEnv *xaoEnv(/*IN*/OraText *pDbName);

/////////////////////////////////////////////////////////////////////////////
OCIWchar *OCIWideCharStrrchr(/*IN/OUT*/void  *pEnvironment,
                             /*IN*/const OCIWchar *pSourceStr,
                             /*IN*/OCIWchar wChar);

size_t OCIWideCharStrCaseConversion(/*IN/OUT*/void  *pEnvironment,
                                    /*OUT*/OCIWchar *pDestStr,
                                    /*IN*/const OCIWchar *pSourceStr,
                                    /*IN*/ub4 convertFlag);

size_t OCIWideCharDisplayLength(/*IN/OUT*/void  *pEnvironment,
                                /*IN*/OCIWchar wChar);

ub2 OCINlsCharSetNameToId(/*IN/OUT*/void  *pEnvironment,
                          /*IN*/const oratext *pName);

sword OCINlsNameMap(/*IN/OUT*/void  *pEnvironment,
                    /*OUT*/oratext *pBuffer,
                    /*IN*/size_t bufferLen,
                    /*IN*/const oratext *pSrcbuf,
                    /*IN*/ub4 flag);

sword OCINlsNumericInfoGet(/*IN/OUT*/void  *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           /*OUT*/sb4 *pOutputNumVariable,
                           /*IN*/ub2 environmentItem);

sword OCINlsCharSetIdToName(/*IN/OUT*/void  *pEnvironment,
                            /*OUT*/oratext *pBuffer,
                            /*IN*/size_t bufferLen,
                            /*IN*/ub2 setId);


sword OCIMultiByteInSizeToWideChar(/*IN/OUT*/void  *pEnvironment,
                                   /*OUT*/OCIWchar *pDestination,
                                   /*IN*/size_t destBufferSize,
                                   /*IN*/const OraText *pSource,
                                   /*IN*/size_t sourceStringLen,
                                   /*OUT*/size_t *pStrOutputSize);

sword OCIMultiByteToWideChar(/*IN/OUT*/void  *pEnvironment,
                             /*OUT*/OCIWchar *pDestination,
                             /*IN*/const OraText *pSourceStr,
                             /*OUT*/size_t *pStrOutputSize);


////////////////////////////////////////////////////////////////////
sword OCIWideCharInSizeToMultiByte(/*IN/OUT*/void  *pEnvironment,
                                   /*OUT*/OraText  *pDestination,
                                    /*OUT*/size_t destinationBufferSize,
                                    /*IN*/const OCIWchar *pSource,
                                    /*IN*/size_t sourceBufferSize,
                                    /*OUT*/size_t *pStrOutputSize);

boolean OCIWideCharIsAlnum(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

sword OCIWideCharToMultiByte(/*IN/OUT*/void  *pEnvironment,
                             /*OUT*/OraText  *pDestination,
                             /*IN*/const OCIWchar *pSource,
                             /*OUT*/size_t *pStrOutputSize);

boolean OCIWideCharIsLower(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

boolean OCIWideCharIsPrint(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

int OCIWideCharStrcmp(/*IN/OUT*/void  *pEnvironment,
                      /*IN*/const OCIWchar *pWCharStr1,
                      /*IN*/const OCIWchar *pWCharStr2,
                      /*IN*/int flag);

int OCIWideCharStrncmp(/*IN/OUT*/void  *pEnvironment,
                       /*IN*/const OCIWchar *pWCharStr1,
                       /*IN*/size_t strLen1,
                       /*IN*/const OCIWchar *pWCharStr2,
                       /*IN*/size_t strLen2,
                       /*IN*/int flag);

boolean OCIWideCharIsCntrl(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

size_t OCIWideCharStrcat(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIWchar *pDestWCharStr,
                         /*IN*/const OCIWchar *pSourceWCharStr);

OCIWchar *OCIWideCharStrchr(/*IN/OUT*/void  *pEnvironment,
                            /*IN*/const OCIWchar *pWCharStr,
                            /*IN*/OCIWchar searchWChar);

boolean OCIWideCharIsDigit(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

boolean OCIWideCharIsGraph(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

boolean OCIWideCharIsSpace(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

boolean OCIWideCharIsUpper(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

boolean OCIWideCharIsXdigit(/*IN/OUT*/void  *pEnvironment,
                            /*IN*/OCIWchar wChar);

boolean OCIWideCharIsSingleByte(/*IN/OUT*/void  *pEnvironment,
                                /*IN*/OCIWchar wChar);

OCIWchar OCIWideCharToLower(/*IN/OUT*/void  *pEnvironment,
                            /*IN*/OCIWchar wChar);

OCIWchar OCIWideCharToUpper(/*IN/OUT*/void  *pEnvironment,
                            /*IN*/OCIWchar wChar);


size_t OCIWideCharStrcpy(/*IN/OUT*/void  *pEnvironment,
                         /*OUT*/OCIWchar *pDestWCharStr,
                         /*IN*/const OCIWchar *pSourceWCharStr);

size_t OCIWideCharStrlen(/*IN/OUT*/void  *pEnvironment,
                         /*IN*/const OCIWchar *pSourceWCharStr);

size_t OCIWideCharStrncat(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIWchar *pDestWCharStr,
                         /*IN*/const OCIWchar *pSourceWCharStr,
                         /*IN*/size_t strAppendLen);

size_t OCIWideCharStrncpy(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIWchar *pDestWCharStr,
                         /*IN*/const OCIWchar *pSourceWCharStr,
                         /*IN*/size_t strCopyLen);

size_t OCIWideCharMultiByteLength(/*IN/OUT*/void  *pEnvironment,
                                  /*IN*/OCIWchar wChar);

boolean OCIWideCharIsPunct(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

boolean OCIWideCharIsAlpha(/*IN/OUT*/void  *pEnvironment,
                           /*IN*/OCIWchar wChar);

// Embedded SQL support
sword SQLEnvGet(dvoid *ctx, OCIEnv **pEnvironment);
sword SQLSvcCtxGet(dvoid *ctx, text *dbname, sb4 dbnamelen, OCISvcCtx **pSvcCtx);


/////////////////////////////////////////////////////////////////////
int OCIMultiByteStrcmp(/*IN/OUT*/void  *pEnvironment,
                       /*IN*/const OraText *pStr1,
                       /*IN*/const OraText *pStr2,
                       /*IN*/int comparisonMethodFlag);

int OCIMultiByteStrncmp(/*IN/OUT*/void  *pEnvironment,
                        /*IN*/const OraText *pStr1,
                        /*IN*/size_t pStrLen1,
                        /*IN*/OraText *pStr2,
                        /*IN*/size_t pStrLen2,
                        /*IN*/int comparisonMethodFlag);

size_t OCIMultiByteStrcat(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OraText *pDestStr,
                          /*IN*/const OraText *pSourceStr);

size_t OCIMultiByteStrcpy(/*IN/OUT*/void  *pEnvironment,
                          /*OUT*/OraText *pDestStr,
                          /*IN*/const OraText *pSourceStr);

size_t OCIMultiByteStrlen(/*IN/OUT*/void  *pEnvironment,
                          /*IN*/const OraText *pStr);

size_t OCIMultiByteStrncat(/*IN/OUT*/void  *pEnvironment,
                           /*OUT*/OraText *pDestStr,
                           /*IN*/const OraText *pSourceStr,
                           /*IN*/size_t strAppendLen);

size_t OCIMultiByteStrncpy(/*IN/OUT*/void  *pEnvironment,
                           /*OUT*/OraText *pDestStr,
                           /*IN*/const OraText *pSourceStr,
                           /*IN*/size_t strCopyLen);

size_t OCIMultiByteStrnDisplayLength(/*IN/OUT*/void  *pEnvironment,
                                     /*IN*/const OraText *pStr1,
                                     /*IN*/size_t num);

size_t OCIMultiByteStrCaseConversion(/*IN/OUT*/void  *pEnvironment,
                                    /*OUT*/OraText *pDestStr,
                                    /*IN*/const OraText *pSourceStr,
                                    /*IN*/ub4 convertFlag);

/////////////////////////////////////////////////////////////////////
boolean OCICharSetConversionIsReplacementUsed(/*IN/OUT*/void  *pEnvironment);

sword OCICharSetToUnicode(/*IN/OUT*/void  *pEnvironment,
                          /*OUT*/ub2 *pDestBuffer,
                          /*IN*/size_t destBufferLen,
                          /*IN*/const OraText *pSourceStr,
                          /*IN*/size_t  sourceLen,
                          /*OUT*/size_t *pStrOutputSize);

sword OCINlsEnvironmentVariableGet(/*OUT*/void   *pBuffer,
                                   /*IN*/size_t sizeBuffer,
                                   /*IN*/ub2 itemValue,
                                   /*IN*/ub2 CharacterSet,
                                   /*OUT*/size_t *pStrOutputSize);

OraText *OCIMessageGet(/*IN/OUT*/OCIMsg *pMsgh,
                       /*IN*/ub4 messageNum,
                       /*OUT*/OraText *pMessageBuffer,
                       /*IN*/size_t bufLen);

sword OCIMessageClose(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN/OUT*/OCIMsg *pMessage);

sword OCIMessageOpen(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*OUT*/OCIMsg **pMassage,
                     /*IN*/const OraText *pRoductName,
                     /*IN*/const OraText *pFacilityName,
                     /*IN*/OCIDuration duration);


sword OCIUnicodeToCharSet(/*IN/OUT*/void  *pEnvironment,
                          /*OUT*/OraText  *pDestination,
                          /*IN*/size_t destBufferLen,
                          /*IN*/const ub2 *pSourceStr,
                          /*IN*/size_t  sourceLen,
                          /*OUT*/size_t *pStrOutputSize);

sword OCINlsCharSetConvert(/*IN/OUT*/void  *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/ub2 destId,
                           /*OUT*/void  *pDestBuffer,
                           /*IN*/size_t destLen,
                           /*IN*/ub2 sourceId,
                           /*IN*/const void  *pSrcp,
                           /*IN*/size_t sourceBuffer,
                           /*OUT*/size_t *pStrOutputSize);






///////////////////////////////////////////////////////////////////
boolean OCIThreadIsMulti();

sword OCIThreadMutexInit(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN/OUT*/OCIThreadMutex **pMutex);

sword OCIThreadMutexDestroy(/*IN/OUT*/void  *pEnvironment,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN/OUT*/OCIThreadMutex **pMutex);

sword OCIThreadIdSetNull(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OCIThreadId *pThreadId);

sword OCIThreadIdGet(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/OCIThreadId *pThreadId);

sword OCIThreadMutexAcquire(/*IN/OUT*/void  *pEnvironment,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN/OUT*/OCIThreadMutex *pMutex);

void OCIThreadProcessInit();

sword OCIThreadInit(/*IN/OUT*/void  *pEnvironment,
                    /*IN/OUT*/OCIError *pErrorHandle);

sword OCIThreadTerm(/*IN/OUT*/void  *pEnvironment,
                    /*IN/OUT*/OCIError *pErrorHandle);

sword OCIThreadJoin(/*IN/OUT*/void  *pEnvironment,
                    /*IN/OUT*/OCIError *pErrorHandle,
                    /*IN*/OCIThreadHandle *pThread);

sword OCIThreadHandleGet(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN/OUT*/OCIThreadHandle *pThread);

sword OCIThreadKeyGet(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/OCIThreadKey *pKey,
                      void  **pValue);

sword OCIThreadIdDestroy(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN/OUT*/OCIThreadId **pTid);

sword OCIThreadIdSet(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*OUT*/OCIThreadId *pThreadIdDest,
                     /*OUT*/OCIThreadId *pThreadIdSrc);

sword OCIThreadIdNull(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/OCIThreadId *pThreadId,
                      /*IN/OUT*/boolean *pResult);

sword OCIThreadKeySet(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/OCIThreadKey *pKey,
                      /*IN*/void  *pThreadValue);

sword OCIThreadIdInit(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN/OUT*/OCIThreadId **pTid);

sword OCIThreadHndInit(/*IN/OUT*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       OCIThreadHandle **pThnd);

sword OCIThreadHndDestroy(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          OCIThreadHandle **pThnd);

sword OCIThreadClose(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN/OUT*/OCIThreadHandle *pThread);

sword OCIThreadMutexRelease(/*IN/OUT*/void  *pEnvironment,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN/OUT*/OCIThreadMutex *pMutex);

sword OCIThreadKeyInit(/*IN/OUT*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*OUT*/OCIThreadKey **pNewKey,
                       /*IN*/OCIThreadKeyDestFunc destructorForKey);

sword OCIThreadKeyDestroy(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN/OUT*/OCIThreadKey **pKey);

sword OCIThreadCreate(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/void (*pStart)(void  *),
                      /*IN*/void  *pArgument,
                      /*IN*/OCIThreadId *pThreadId,
                      /*IN/OUT*/OCIThreadHandle *pThread);

sword OCIThreadIdSame(/*IN/OUT*/void  *pEnvironment,
                      /*IN/OUT*/OCIError *pErrorHandle,
                      /*IN*/OCIThreadId *pThreadId1,
                      /*IN*/OCIThreadId *pThreadId2,
                      /*IN/OUT*/boolean *pResult);





///////////////////////////////////////////////////////////////////////
sword OCISubscriptionUnRegister(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                                OCISubscription *pSubscrhp,
                                /*IN/OUT*/OCIError *pErrorHandle,
                                ub4 mode);

sword OCISubscriptionPost(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                          OCISubscription **pSubscrhpp,
                          ub2 count,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          ub4 mode);

sword OCISubscriptionEnable(OCISubscription *pSubscrhp,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            ub4 mode);

sword OCISubscriptionDisable(OCISubscription *pSubscrhp,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             ub4 mode);

sword OCISubscriptionRegister(/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                              OCISubscription **pSubscrhpp,
                              ub2 count,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              ub4 mode);

 ////////////////////////////////////////////////////
/////////   D A T E   F U N C T I O N S    /////////
sword OCIDateTimeConstruct(/*IN/OUT*/void   *pHndl,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/OCIDateTime *pDateTime,
                           /*IN*/sb2 year,
                           /*IN*/ub1 month,
                           /*IN*/ub1 day,
                           /*IN*/ub1 hour,
                           /*IN*/ub1 min,
                           /*IN*/ub1 sec,
                           /*IN*/ub4 fractionalSeconds,
                           /*IN*/OraText  *pTimeZone,
                           /*IN*/size_t timezoneLen);


sword OCIDateTimeSysTimeStamp(/*IN/OUT*/void  *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              /*OUT*/OCIDateTime *pSysDate);

sword OCIDateTimeSubtract(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/OCIDateTime *pIndate1,
                          /*IN*/OCIDateTime *pIndate2,
                          /*OUT*/OCIInterval *pInterval);

sword OCIDateTimeIntervalAdd(/*IN/OUT*/void  *pEnvironment,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN*/OCIDateTime *pDateTime,
                             /*IN*/OCIInterval *pInterval,
                             /*OUT*/OCIDateTime *pResultDateTime);

sword OCIDateTimeToText(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIDateTime *pDate,
                        /*IN*/const oratext *pConvFormat,
                        /*IN*/ub1 convFormatLen,
                        /*IN*/ub1 fractionlSeconds,
                        /*IN*/const oratext *pLanguageName,
                        /*IN*/size_t langLength,
                        /*IN*/ub4 *pBufferSize,
                        /*OUT*/oratext *pBuffer);

sword OCIDateTimeFromText(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OraText *pDdateStr,
                          /*IN*/size_t dstrLength,
                          /*IN*/const oratext *pConvFormat,
                          /*IN*/ub1 convFormatLen,
                          /*IN*/const oratext *pLanguageName,
                          /*IN*/size_t langLength,
                          /*IN*/OCIDateTime *pDate);

sword OCIDateTimeIntervalSub(/*IN/OUT*/void  *pEnvironment,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN*/OCIDateTime *pDateTime,
                             /*IN*/OCIInterval *pInterval,
                             /*OUT*/OCIDateTime *pResultDateTime);

sword OCIDateTimeAssign(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIDateTime *pSourceDateTime,
                        /*OUT*/OCIDateTime *pTargetDateTime);

sword OCIDateTimeCompare(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIDateTime *pDdate1,
                         /*IN*/const OCIDateTime *pDate2,
                         /*OUT*/sword *pResult);

sword OCIDateTimeGetDate(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIDateTime *pDate,
                         /*OUT*/sb2 *pYear,
                         /*OUT*/ub1 *pMonth,
                         /*OUT*/ub1 *pDy);

sword OCIDateTimeGetTimeZoneOffset(/*IN/OUT*/void  *pEnvironment,
                                   /*IN/OUT*/OCIError *pErrorHandle,
                                   /*IN*/const /*IN*/OCIDateTime *pDateTime,
                                   /*OUT*/sb1 *pHour,
                                   /*OUT*/sb1 *pMm);

sword OCIDateTimeCheck(/*IN/OUT*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCIDateTime *pCheckedDate,
                       /*OUT*/ub4 *pValid);

sword OCIDateTimeGetTimeZoneName(/*IN/OUT*/void  *pEnvironment,
                                 /*IN/OUT*/OCIError *pErrorHandle,
                                 /*IN*/const OCIDateTime *pDateTime,
                                 /*OUT*/ub1 *pBuffer,
                                 /*IN/OUT*/ub4 *pBufferLen);

sword OCIDateTimeToArray(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/const OCIDateTime *pDateTime,
                         /*IN*/const OCIInterval *pTimezoneInterval,
                         /*OUT*/ub1 *pResultArray,
                         /*OUT*/ub4 *pArrayLen,
                         /*IN*/ub1 fractionlSeconds);

sword OCIDateTimeFromArray(/*IN/OUT*/void  *pEnvironment,
                           /*IN/OUT*/OCIError *pErrorHandle,
                           /*IN*/ub1 *pInarray,
                           /*IN*/ub4 Len,
                           /*IN*/ub1 type,
                           /*OUT*/OCIDateTime *pDateTime,
                           /*IN*/ const OCIInterval *pTimezoneInterval,
                           /*IN*/ub1 fractionlSeconds);

sword OCIDateTimeGetTime(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*//*IN*/OCIDateTime *pDateTime,
                         /*OUT*/ub1 *pHour,
                         /*OUT*/ub1 *pMinute,
                         /*OUT*/ub1 *pSecond,
                         /*OUT*/ub4 *pFsec);

sword OCIDateTimeConvert(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OCIDateTime *pSourceDate,
                         /*OUT*/OCIDateTime *pTargetDate);



////////////////////////////////////////////////////////////////
sword OCIIntervalSetYearMonth(/*IN/OUT*/void  *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              /*IN*/sb4 year,
                              /*IN*/sb4 month,
                              /*OUT*/OCIInterval *pResult);

sword OCIIntervalGetYearMonth(/*IN/OUT*/void  *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              sb4 *pYear,
                              sb4 *pMonth,
                              const OCIInterval *pResult);

sword OCIIntervalToNumber(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OCIInterval *pInterval,
                          /*OUT*/OCINumber *pNumber);

sword OCIIntervalMultiply(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OCIInterval *pInterval,
                          /*IN*/OCINumber *pNfactor,
                          /*OUT*/OCIInterval *pResult);

sword OCIIntervalDivide(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/OCIInterval *pDividend,
                        /*IN*/OCINumber *pDivisor,
                        /*OUT*/OCIInterval *pResult);

sword OCIIntervalCompare(/*IN/OUT*/void  *pEnvironment,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OCIInterval *pInter1,
                         /*IN*/OCIInterval *pInter2,
                         /*OUT*/sword *pResult);

sword OCIIntervalFromNumber(/*IN/OUT*/void  *pEnvironment,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*OUT*/OCIInterval *pInterval,
                            /*IN*/OCINumber *pNumber);

sword OCIIntervalSetDaySecond(/*IN/OUT*/void  *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              /*IN*/sb4 day,
                              /*IN*/sb4 hour,
                              /*IN*/sb4 minute,
                              /*IN*/sb4 second,
                              /*IN*/sb4 fractionalSeconds,
                              /*OUT*/OCIInterval *pResult);

sword OCIIntervalGetDaySecond(/*IN/OUT*/void  *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              /*OUT*/sb4 *pDay,
                              /*OUT*/sb4 *pHour,
                              /*OUT*/sb4 *pMinute,
                              /*OUT*/sb4 *pSecond,
                              /*OUT*/sb4 *pFractionalSeconds,
                              /*IN*/const OCIInterval *pResult);

sword OCIIntervalSubtract(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/OCIInterval *pMinuend,
                          /*IN*/OCIInterval *pSubtrahend,
                          /*OUT*/OCIInterval *pResult);

sword OCIIntervalAdd(/*IN/OUT*/void  *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN*/OCIInterval *pAddend1,
                     /*IN*/OCIInterval *pAddend2,
                     /*OUT*/OCIInterval *pResult);

sword OCIIntervalFromText(/*IN/OUT*/void  *pEnvironment,
                          /*IN/OUT*/OCIError *pErrorHandle,
                          /*IN*/const OraText *pInpstr,
                          /*IN*/size_t strLen,
                          /*OUT*/OCIInterval *pResult);

sword OCIIntervalToText(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIInterval *pInterval,
                        /*IN*/ub1 lfprec,
                        /*IN*/ub1 fractionlSeconds,
                        /*OUT*/OraText *pBuffer,
                        /*IN*/size_t bufLen,
                        /*OUT*/size_t *pResultLen);

sword OCIIntervalCheck(/*IN/OUT*/void  *pEnvironment,
                       /*IN/OUT*/OCIError *pErrorHandle,
                       /*IN*/const OCIInterval *pInterval,
                       /*OUT*/ub4 *pValid);

sword OCIIntervalAssign(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const OCIInterval *pIninter,
                        /*OUT*/OCIInterval *pResultInterval);

sword OCIIntervalFromTZ(/*IN/OUT*/void  *pEnvironment,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        /*IN*/const oratext *pInputString,
                        /*IN*/size_t strLen,
                        /*OUT*/OCIInterval *pResult);

////////////////////////////////////////////////////////////////
sword OCIConnectionPoolDestroy(/*IN/OUT*/OCICPool *pPoolhp,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               /*IN*/ub4 mode);

sword OCIConnectionPoolCreate(/*IN/OUT*/OCIEnv *pEnvironment,
                              /*IN/OUT*/OCIError *pErrorHandle,
                              /*IN/OUT*/OCICPool *pPoolhp,
                              /*OUT*/OraText **pPoolName,
                              /*OUT*/sb4 *pPoolNameLen,
                              /*IN*/const OraText *pDblink,
                              /*IN*/sb4 dblinkLen,
                              /*IN*/ub4 connMin,
                              /*IN*/ub4 connMax,
                              /*IN*/ub4 connIncr,
                              /*IN/OUT*/const OraText *pPoolUserName,
                              /*IN*/sb4 poolUserLen,
                              /*IN/OUT*/const OraText *pPoolPassword,
                              /*IN*/sb4 poolPassLen,
                              /*IN*/ub4 mode);

sword OCISessionPoolDestroy (/*IN/OUT*/OCISPool *pSpool,
                             /*IN/OUT*/OCIError *pErrorHandle,
                             /*IN*/ub4 mode);

sword OCISessionGet (/*IN/OUT*/OCIEnv *pEnvironment,
                     /*IN/OUT*/OCIError *pErrorHandle,
                     /*IN/OUT*/OCISvcCtx **pSvchp,
                     /*IN/OUT*/OCIAuthInfo *pAuthhp,
                     /*IN*/OraText *pPoolName,
                     /*IN*/ub4 poolNameLen,
                     /*IN*/const OraText *pTagInfo,
                     /*IN*/ub4 tagInfoLen,
                     /*OUT*/OraText **pRetTagInfo,
                     /*OUT*/ub4 *pRetTagInfoLen,
                     /*OUT*/boolean *pFound,
                     /*IN*/ub4 mode);

sword OCISessionRelease (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         /*IN*/OraText *pTag,
                         /*IN*/ub4 tagLen,
                         /*IN*/ub4 mode);

sword OCISessionPoolCreate (/*IN/OUT*/OCIEnv *pEnvironment,
                            /*IN/OUT*/OCIError *pErrorHandle,
                            /*IN/OUT*/OCISPool *pSpool,
                            /*OUT*/OraText **pPoolName,
                            /*OUT*/ub4 *pPoolNameLen,
                            /*IN*/const OraText *pConnectStr,
                            /*IN*/ub4 connectStrLen,
                            /*IN*/ub4 sessMin,
                            /*IN*/ub4 sessMax,
                            /*IN*/ub4 sessIncr,
                            /*IN*/OraText *pUserId,
                            /*IN*/ub4 useridLen,
                            /*IN*/OraText *pPassword,
                            /*IN*/ub4 passwordLen,
                            /*IN*/ub4 mode);

sword  OCIMemStats(void     *pHndlp,
                   OCIError *pErrorHandle,
                   OCIEnv   **pEnvironment,
                   ub4      mode,
                   ub4      mode1,
                   oratext  *pTabname);

sword OCIPing (/*IN/OUT*/OCISvcCtx *pServiceContexthp,
               OCIError *pErrorHandle,
               ub4 mode);

sword OCIAppCtxClearAll(void *pSession,
                        void  *pNamespaceStr,
                        ub4 nsptrLen,
                        /*IN/OUT*/OCIError *pErrorHandle,
                        ub4 mode);

sword OCIAppCtxSet(/*IN/OUT*/void * pSession,
                   /*IN*/void  *pNamespaceStr,
                   /*IN*/ub4 nsptrLen,
                   /*IN*/void  *pAttr,
                   /*IN*/ub4 pAttrLen,
                   /*IN*/void  *pValue,
                   /*IN*/ub4 pValueLen,
                   /*IN/OUT*/OCIError *pErrorHandle,
                   /*IN*/ub4 mode);

///////////- Kerberos Authentication prototypes
sword OCIKerbAttrSet(/*IN*/OCISession *pTrgthndlp,
                     /*IN*/ub4 credUse,
                     /*IN*/ub1 *pFtgtTIcket,
                     /*IN*/ub4 ticketLen,
                     /*IN*/ub1 *pSessionKey,
                     /*IN*/ub4 skeyLen,
                     /*IN*/ub2 ftgtKeytype,
                     /*IN*/ub4 ftgtTIcketFlags,
                     /*IN*/sb4 ftgtAuthTIme,
                     /*IN*/sb4 ftgtStartTIme,
                     /*IN*/sb4 ftgtEndTIme,
                     /*IN*/sb4 ftgtRenewTIme,
                     /*IN*/oratext *pFtgtClientPrincipal,
                     /*IN*/ub4 ftgtClientPrincipalLen,
                     /*IN*/oratext *pFtgtClientRealm,
                     /*IN*/ub4 ftgtClientRealmLen,
                     /*IN/OUT*/OCIError *pErrorHandle);


sword OCIInitEventHandle(OCIError *pErrorHandle,
                         OCIEvent *pEvent,
                         text     *pStr,
                         ub4       size);

sword OCIDBShutdown(OCISvcCtx     *pSvchp,
                    OCIError      *pErrorHandle,
                    OCIAdmin      *pAdmhp,
                    ub4            mode);

void OCIClientVersion(sword *pMajorVersion,
                      sword *pMinorVersion,
                      sword *pUpdateNum,
                      sword *pPatchNum,
                      sword *pPortUpdateNum);

struct xmlctx *OCIXmlDbInitXmlCtx(/*IN*/OCIEnv *pEnvironment,
                                  /*IN*/OCISvcCtx *pService,
                                  /*IN*/OCIError *pErrorHandle,
                                  /*IN*/ocixmldbparam *pParameters,
                                  /*IN*/int nParams);

sword OCIDBStartup (OCISvcCtx     *pSvchp,
                    OCIError      *pErrorHandle,
                    OCIAdmin      *pAdmhp,
                    ub4            mode,
                    ub4            flags);

void OCIXmlDbFreeXmlCtx(struct xmlctx *xctx);


///////////////////////////////////////////////////////////////
sword OCIDirPathFinish(OCIDirPathCtx *pDirectPath,
                       /*IN/OUT*/OCIError  *pErrorHandle);

sword OCIDirPathStreamReset(OCIDirPathStream *pDpstr,
                            OCIError *pErrorHandle);

sword OCIDirPathColArrayEntrySet(OCIDirPathColArray *pColumnArray,
                                 /*IN/OUT*/OCIError *pErrorHandle,
                               ub4 nRow,
                               ub2 colIdx,
                               ub1 *pCvalp,
                               ub4 cLen,
                               ub1 cflg);

sword OCIDirPathFlushRow(OCIDirPathCtx *pDirectPath,
                         /*IN/OUT*/OCIError  *pErrorHandle);

sword OCIDirPathColArrayEntryGet(OCIDirPathColArray *pColumnArray,
                                 /*IN/OUT*/OCIError *pErrorHandle,
                              ub4 nRow,
                              ub2 colIdx,
                              ub1 **pCvalpp,
                              ub4 *pClenp,
                              ub1 *pCflgp);

sword OCIDirPathColArrayToStream(OCIDirPathColArray *pColumnArray,
                                 OCIDirPathCtx *pDpctx,
                                 OCIDirPathStream   *pDpstr,
                                 OCIError      *pErrorHandle,
                                 ub4 rowcnt, ub4 rowoff);

sword OCIDirPathColArrayReset(OCIDirPathColArray *pColumnArray,
                              /*IN/OUT*/OCIError *pErrorHandle);

sword OCIDirPathColArrayRowGet(OCIDirPathColArray *pColumnArray,
                               /*IN/OUT*/OCIError *pErrorHandle,
                               ub4 nRow,
                               ub1 ***pCvalppp,
                               ub4 **pClenpp,
                               ub1 **pCflgpp);

sword OCIDirPathAbort(OCIDirPathCtx *pDirectPath,
                      /*IN/OUT*/OCIError *pErrorHandle);

sword OCIDirPathDataSave(OCIDirPathCtx *pDirectPath,
                         /*IN/OUT*/OCIError *pErrorHandle,
                         ub4 action);

sword OCIDirPathPrepare(OCIDirPathCtx *pDirectPath,
                        OCISvcCtx *pServiceContexthp,
                        /*IN/OUT*/OCIError  *pErrorHandle);

sword OCIDirPathLoadStream(OCIDirPathCtx *pDirectPath,
                           OCIDirPathStream *pDpstr,
                           /*IN/OUT*/ OCIError    *pErrorHandle);


#endif


#ifdef __cplusplus
}
#endif

