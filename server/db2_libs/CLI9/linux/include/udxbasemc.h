// udxbasemc.h
// Base class for Multi Column User Defined Functions and Aggregates
//
// (C) Copyright IBM Corp. 2009, 2011  All Rights Reserved.

#ifndef UDX_BASEMC_INCLUDED
#define UDX_BASEMC_INCLUDED

#define CHECK_ROW_RANGE(n) if (n < 0 || n >= ((RowFuncCallInfo*)m_pInfo)->numReturnColumns) \
                  throwError(NZ_ERROR_ILLVAL, "Return Column number out of range", n)

namespace nz {
namespace udx_ver2 {

class UdxNonScalarBase : public UdxBase
{
public:
    UdxNonScalarBase(UdxInit *pInit) : UdxBase(pInit) {}
    
 
	
	// New interface for row functions
public:
    // Return methods
    int numReturnColumns() const;
    int returnTypeColumn(int n) const;
    void setReturnColumnNull(int n, bool val) const;
    bool isReturnColumnNull(int n) const;  
    StringReturn* stringReturnColumn(int n) const;
    float* floatReturnColumn(int n) const; 
    double* doubleReturnColumn(int n) const;
    int16* int16ReturnColumn(int n) const;
    int32* int32ReturnColumn(int n) const;
    int64* int64ReturnColumn(int n) const;
    int32* dateReturnColumn(int n) const;
    int64* timeReturnColumn(int n) const;

};


inline int UdxNonScalarBase::numReturnColumns() const
{
    return ((RowFuncCallInfo*)m_pInfo)->numReturnColumns;
}

inline int UdxNonScalarBase::returnTypeColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    return ((RowFuncCallInfo*)m_pInfo)->returnColumnTypes[n];
}

inline void UdxNonScalarBase::setReturnColumnNull(int n, bool val) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    ((RowFuncCallInfo*)m_pInfo)->returnColumnNulls[n] = val;

}
inline bool UdxNonScalarBase::isReturnColumnNull(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    return ((RowFuncCallInfo*)m_pInfo)->returnColumnNulls[n];

}
inline StringReturn* UdxNonScalarBase::stringReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_FIXED && nType != UDX_VARIABLE && nType != UDX_NATIONAL_FIXED &&
        nType != UDX_NATIONAL_VARIABLE && nType != UDX_GEOMETRY && nType != UDX_VARBINARY)
        throwError(NZ_ERROR_ILLVAL, "Expected string, st_geometry, or varbinary column value", nType);
    return (StringReturn*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline float* UdxNonScalarBase::floatReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_FLOAT)
        throwError(NZ_ERROR_ILLVAL, "Expected float column value", nType);
    return (float*) ((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline double* UdxNonScalarBase::doubleReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_DOUBLE)
        throwError(NZ_ERROR_ILLVAL, "Expected double column value", nType);
    return (double*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline int16* UdxNonScalarBase::int16ReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_INT16)
        throwError(NZ_ERROR_ILLVAL, "Expected int16 column value", nType);
    return (int16*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline int32* UdxNonScalarBase::int32ReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_INT32)
        throwError(NZ_ERROR_ILLVAL, "Expected int32 column value", nType);
    return (int32*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline int64* UdxNonScalarBase::int64ReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_INT64)
        throwError(NZ_ERROR_ILLVAL, "Expected int64 column value", nType);
    return (int64*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline int32* UdxNonScalarBase::dateReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_DATE)
        throwError(NZ_ERROR_ILLVAL, "Expected date column value", nType);
    return (int32*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}
inline int64* UdxNonScalarBase::timeReturnColumn(int n) const
{
    CHECK_INFO_AVAILABLE();
    CHECK_ROW_RANGE(n);
    int nType = returnTypeColumn(n);
    if (nType != UDX_TIME)
        throwError(NZ_ERROR_ILLVAL, "Expected time column value", nType);
    return (int64*)((RowFuncCallInfo*)m_pInfo)->returnColumns[n];

}


} // end namespace udx_ver2
} // end namespace nz

#endif //  #ifndef UDX_BASEMC_INCLUDED
