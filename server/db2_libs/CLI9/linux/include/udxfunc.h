// udxfuncv2.h
// Function Ver 2 class for User Defined Functions and Aggregates
//
// (C) Copyright IBM Corp. 2009, 2011  All Rights Reserved.

#ifndef UDX_FUNCV2_INCLUDED
#define UDX_FUNCV2_INCLUDED

#define CHECK_SIZER_RANGE(n) if (n < 0 || n >= m_pInfo->numArgs) \
                  throwError(NZ_ERROR_ILLVAL, "Sizer argument number out of range", n)

#define CHECK_SIZER_TYPMOD(n, typmod) if (typmod == 0xFFFFFFFFFFFFFFFFLL) \
                  throwError(NZ_ERROR_ILLVAL, "Sizer argument invalid", n)
                          
namespace nz {  
namespace udx_ver2 {

class Udf : public UdxScalarBase
{
public:
	Udf(UdxInit *pInit) : UdxScalarBase(pInit) {}   	
	virtual ~Udf() {}

	// Interface methods
	virtual ReturnValue evaluate() = 0;
	
	// The derived class must have a static method like such:
	// static Udf* instantiate();

	// Called from engine
	ReturnValue performEvaluate(FuncCallInfo *pFcInfo)
	{
    ReturnValue oRet;
	  m_pInfo = pFcInfo;
    oRet = evaluate();
	  m_pInfo = NULL;
	  return oRet;
	}
};
   
} // end namespace udx_ver2
} // end namespace nz

#endif //  #ifndef UDX_FUNCV2_INCLUDED
