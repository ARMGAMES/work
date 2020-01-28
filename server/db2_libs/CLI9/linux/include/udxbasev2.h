// udxbasev2.h
// Base class for Ver 2 User Defined Functions and Aggregates
//
// (C) Copyright IBM Corp. 2009, 2011  All Rights Reserved.

#ifndef UDX_BASEV2_INCLUDED
#define UDX_BASEV2_INCLUDED

// These macros are what should be used to return from evaluate or finalResult
#define NZ_UDX_RETURN_NULL() do { setReturnNull(true); return (void*)NULL; } while (0)

#define NZ_UDX_RETURN_STRING(x) return stringReturn(x)
#define NZ_UDX_RETURN_BOOL(x) return boolReturn(x)
#define NZ_UDX_RETURN_DATE(x) return dateReturn(x)
#define NZ_UDX_RETURN_TIME(x) return timeReturn(x)
#define NZ_UDX_RETURN_TIMETZ(x) return timetzReturn(x)
#define NZ_UDX_RETURN_NUMERIC32(x) return numeric32Return(x)
#define NZ_UDX_RETURN_NUMERIC64(x) return numeric64Return(x)
#define NZ_UDX_RETURN_NUMERIC128(x) return numeric128Return(x)
#define NZ_UDX_RETURN_FLOAT(x) return floatReturn(x)
#define NZ_UDX_RETURN_DOUBLE(x)  return doubleReturn(x)
#define NZ_UDX_RETURN_INTERVAL(x) return intervalReturn(x)
#define NZ_UDX_RETURN_INT64(x) return int64Return(x)
#define NZ_UDX_RETURN_INT32(x) return int32Return(x)
#define NZ_UDX_RETURN_INT16(x) return int16Return(x)
#define NZ_UDX_RETURN_INT8(x) return int8Return(x)
#define NZ_UDX_RETURN_TIMESTAMP(x) return timestampReturn(x)

#define CHECK_FUNC_RANGE(n) if (n < 0 || n >= m_pInfo->numArgs) \
		  throwError(NZ_ERROR_ILLVAL, "Argument number out of range", n)
#define CHECK_INFO_AVAILABLE() if (!m_pInfo) \
                  throwError(NZ_ERROR_ILLVAL, "UDF/UDA not yet initialized", 0)

#ifndef MAX_TABLE_COLUMNS
#define MAX_TABLE_COLUMNS 4000
#endif

#define NZ_TABLE_FUNC_MAX_RETURN_COLUMNS MAX_TABLE_COLUMNS 

namespace nz {
namespace udx_ver2 {

using nz::udx_ver2::FuncDatum ;
using nz::udx_ver2::ReturnValue ;
using nz::udx_ver2::StringReturn ;
using nz::udx_ver2::StringArg ;

class UdxEnvironment;
class UdxParameter;

class CallInfo
{
public:
    int     	numArgs;
    int		argTypes[NZ_FUNC_MAX_ARGS];
    FuncDatum	args[NZ_FUNC_MAX_ARGS];
    bool        argNulls[NZ_FUNC_MAX_ARGS];
    bool        argConsts[NZ_FUNC_MAX_ARGS];
};

class FuncCallInfo : public CallInfo
{
 public:
  FuncDatum	pReturnInfo;
  bool*   	pReturnNull;
  int		returnType;
  
};

class RowFuncCallInfo : public CallInfo
{
public:

    int        numReturnColumns;
    int        returnColumnTypes[NZ_TABLE_FUNC_MAX_RETURN_COLUMNS];
    FuncDatum  returnColumns[NZ_TABLE_FUNC_MAX_RETURN_COLUMNS];
    bool       returnColumnNulls[NZ_TABLE_FUNC_MAX_RETURN_COLUMNS];

};

class TableFuncCallInfo : public RowFuncCallInfo
{
public:
    

};

class UdxInit
{
public:
    UdxInit() { m_dummy = 0; }
    ~UdxInit() { }

protected:
    int m_dummy;
};

class UdxBase
{
 public:
    UdxBase(UdxInit *pInit)
    {
      m_pInfo = NULL;
      m_pInit = pInit;
      m_nLogMask = 0;
      m_memory = 0;
      m_pEnviron = NULL;
      m_pParameter = NULL;
    }
  virtual ~UdxBase() {
  } 
  
  enum DataType
    {
      UDX_UNKNOWN=-1,
      UDX_FIXED,
      UDX_VARIABLE,
      UDX_NATIONAL_FIXED,
      UDX_NATIONAL_VARIABLE,
      UDX_BOOL,
      UDX_DATE,
      UDX_TIME,
      UDX_TIMETZ,
      UDX_NUMERIC32,
      UDX_NUMERIC64,
      UDX_NUMERIC128,
      UDX_FLOAT,
      UDX_DOUBLE,
      UDX_INTERVAL,
      UDX_INT8,
      UDX_INT16,
      UDX_INT32,
      UDX_INT64,
      UDX_TIMESTAMP,
      UDX_GEOMETRY,
      UDX_VARBINARY,
      UDX_NUMERIC=UDX_NUMERIC128 // Used for calculateSize only
    };
  
  
  // Arguments methods
  int numArgs() const;

  bool isArgConst(int n) const;
  int argType(int n) const;
  bool isArgNull(int n) const;

  StringArg* stringArg(int n) const;
  int16 int16Arg(int n) const;
  int32 int32Arg(int n) const;
  int64 int64Arg(int n) const;
  float floatArg(int n) const;
  double doubleArg(int n) const;
  int32 dateArg(int n) const;
  int64 timeArg(int n) const;
  int64 timestampArg(int n)const;  
 private:
  int8 m_nLogMask;
  int  m_memory;

 public:
  CallInfo *m_pInfo;
  UdxEnvironment *m_pEnviron;
  UdxParameter *m_pParameter;
  UdxInit *m_pInit;
};



class UdxScalarBase : public UdxBase
{
public:
    UdxScalarBase(UdxInit *pInit) : UdxBase(pInit) {}
  
  // Return methods
  ReturnValue stringReturn(StringReturn* ret) const;
  ReturnValue int16Return(int16 ret) const;
  ReturnValue int32Return(int32 ret) const;
  ReturnValue int64Return(int64 ret) const;
  ReturnValue floatReturn(float ret) const;
  ReturnValue doubleReturn(double ret) const;
  ReturnValue dateReturn(int32 ret) const;
  ReturnValue timeReturn(int64 ret) const;
  ReturnValue timestampReturn(int64 ret)const;

  void setReturnNull(bool val) const;
  int returnType() const;

  StringReturn* stringReturnInfo() const;

};

inline int UdxBase::numArgs() const
{
    CHECK_INFO_AVAILABLE();
  return m_pInfo->numArgs;
}

inline bool UdxBase::isArgConst(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  
  return m_pInfo->argConsts[n];
}
inline int UdxBase::argType(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  return m_pInfo->argTypes[n];
}
inline bool UdxBase::isArgNull(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  return m_pInfo->argNulls[n];
}

inline StringArg* UdxBase::stringArg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_FIXED && nType != UDX_VARIABLE && nType != UDX_NATIONAL_FIXED &&
      nType != UDX_NATIONAL_VARIABLE && nType != UDX_GEOMETRY && nType != UDX_VARBINARY)
    throwError(NZ_ERROR_ILLVAL, "Expected string, st_geometry, or varbinary argument", nType);
  return ((StringArg*)m_pInfo->args[n]);
}
inline float UdxBase::floatArg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_FLOAT)
    throwError(NZ_ERROR_ILLVAL, "Expected float argument", nType);
  return *((float*)m_pInfo->args[n]);
}

inline double UdxBase::doubleArg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_DOUBLE)
    throwError(NZ_ERROR_ILLVAL, "Expected double argument", nType);
  return *((double*)m_pInfo->args[n]);
}
inline int16 UdxBase::int16Arg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_INT16)
    throwError(NZ_ERROR_ILLVAL, "Expected int16 argument", nType);
  return *((int16*)m_pInfo->args[n]);
}
inline int32 UdxBase::int32Arg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_INT32)
    throwError(NZ_ERROR_ILLVAL, "Expected int32 argument", nType);
  return *((int32*)m_pInfo->args[n]);
} 
inline int64 UdxBase::int64Arg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_INT64)
    throwError(NZ_ERROR_ILLVAL, "Expected int64 argument", nType);
  return *((int64*)m_pInfo->args[n]);
}

inline int32 UdxBase::dateArg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_DATE)
    throwError(NZ_ERROR_ILLVAL, "Expected date argument", nType);
  return *((int32*)m_pInfo->args[n]);
}
inline int64 UdxBase::timeArg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_TIME)
    throwError(NZ_ERROR_ILLVAL, "Expected time argument", nType);
  return *((int64*)m_pInfo->args[n]);
}

inline int64 UdxBase::timestampArg(int n) const
{
    CHECK_INFO_AVAILABLE();
  CHECK_FUNC_RANGE(n);
  int nType = argType(n);
  if (nType != UDX_TIMESTAMP)
    throwError(NZ_ERROR_ILLVAL, "Expected timestamp argument", nType);
  return *((int64*)m_pInfo->args[n]);
}

inline int UdxScalarBase::returnType() const
{
    CHECK_INFO_AVAILABLE();
  return ((FuncCallInfo*)m_pInfo)->returnType;
}

inline ReturnValue UdxScalarBase::stringReturn(StringReturn* ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_FIXED && ((FuncCallInfo*)m_pInfo)->returnType != UDX_VARIABLE &&
      ((FuncCallInfo*)m_pInfo)->returnType != UDX_NATIONAL_FIXED && ((FuncCallInfo*)m_pInfo)->returnType != UDX_NATIONAL_VARIABLE &&
      ((FuncCallInfo*)m_pInfo)->returnType != UDX_GEOMETRY && ((FuncCallInfo*)m_pInfo)->returnType != UDX_VARBINARY)
    throwError(NZ_ERROR_ILLVAL, "Expected string, st_geometry, or varbinary return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)(void*)ret;
}

inline ReturnValue UdxScalarBase::int16Return(int16 ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_INT16)
    throwError(NZ_ERROR_ILLVAL, "Expected int16 return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}
inline ReturnValue UdxScalarBase::int32Return(int32 ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_INT32)
    throwError(NZ_ERROR_ILLVAL, "Expected int32 return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}
inline ReturnValue UdxScalarBase::int64Return(int64 ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_INT64)
    throwError(NZ_ERROR_ILLVAL, "Expected int64 return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}
inline ReturnValue UdxScalarBase::floatReturn(float ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_FLOAT)
    throwError(NZ_ERROR_ILLVAL, "Expected float return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}
inline ReturnValue UdxScalarBase::doubleReturn(double ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_DOUBLE)
    throwError(NZ_ERROR_ILLVAL, "Expected double return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}
inline ReturnValue UdxScalarBase::dateReturn(int32 ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_DATE)
    throwError(NZ_ERROR_ILLVAL, "Expected date return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}
inline ReturnValue UdxScalarBase::timeReturn(int64 ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_TIME)
    throwError(NZ_ERROR_ILLVAL, "Expected time return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}

inline ReturnValue UdxScalarBase::timestampReturn(int64 ret) const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_TIMESTAMP)
    throwError(NZ_ERROR_ILLVAL, "Expected timestamp return type", ((FuncCallInfo*)m_pInfo)->returnType);
  setReturnNull(false);
  return (ReturnValue)ret;
}

inline void UdxScalarBase::setReturnNull(bool val) const
{
    CHECK_INFO_AVAILABLE();
  *((FuncCallInfo*)m_pInfo)->pReturnNull = val;
}

inline StringReturn* UdxScalarBase::stringReturnInfo() const
{
    CHECK_INFO_AVAILABLE();
  if (((FuncCallInfo*)m_pInfo)->returnType != UDX_FIXED && ((FuncCallInfo*)m_pInfo)->returnType != UDX_VARIABLE && 
      ((FuncCallInfo*)m_pInfo)->returnType != UDX_NATIONAL_FIXED && ((FuncCallInfo*)m_pInfo)->returnType != UDX_NATIONAL_VARIABLE &&
      ((FuncCallInfo*)m_pInfo)->returnType != UDX_GEOMETRY && ((FuncCallInfo*)m_pInfo)->returnType != UDX_VARBINARY)
    throwError(NZ_ERROR_ILLVAL, "Expected string, st_geometry, or varbinary return type.", ((FuncCallInfo*)m_pInfo)->returnType);
  if (!((FuncCallInfo*)m_pInfo)->pReturnInfo)
    throwError(NZ_ERROR_ILLVAL, "Expected string return, geometry structure, or varbinary structure.", 0);
  return (StringReturn*)((FuncCallInfo*)m_pInfo)->pReturnInfo;
}

} // end namespace udx_ver2
} // end namespace nz

#endif //  #ifndef UDX_BASEV2_INCLUDED
