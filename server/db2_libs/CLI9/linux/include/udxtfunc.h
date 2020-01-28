// udxtfunc.h
// Table Function class for User Defined Functions and Aggregates
//
// (C) Copyright IBM Corp. 2009, 2011  All Rights Reserved.

#ifndef UDX_TFUNC_INCLUDED
#define UDX_TFUNC_INCLUDED
                          
namespace nz {  
namespace udx_ver2 {


class Udtf : public UdxNonScalarBase
{
public:
    Udtf(UdxInit *pInit) : UdxNonScalarBase(pInit) 
    {
        m_correlationType = UnknownCorrelationType;
        m_hasFinal = false;
        m_hasOver = false;
        m_hasPartition = false;
        m_hasOrder = false;
        
        m_callNewInputStream = false;
        m_callNewInputPartition = false;
    }	
	virtual ~Udtf() {}
	
    enum CorrelationType
    {
        UnknownCorrelationType,
        Uncorrelated,
        InnerCorrelation,
        LeftCorrelation
    };

    CorrelationType correlationType() const {
        return m_correlationType;
    }
    void setCorrelationType(CorrelationType type) {
        m_correlationType = type;
    }
 
    bool hasFinal() const {
        return m_hasFinal;
    }

    void setHasFinal(bool final) {
        m_hasFinal = final;

        m_callNewInputStream = final;
    }

    bool hasOver() const {
        return m_hasOver;
    }
    
    void setHasOver(bool over) {
        m_hasOver = over;

        m_callNewInputPartition = over;
    }
    
    bool hasOrder() const {
        return m_hasOrder;
    }

    void setHasOrder(bool order) {
        m_hasOrder = order;
    }

    bool hasPartition() const {
        return m_hasPartition;
    }

    void setHasPartition(bool partition) {
        m_hasPartition = partition;
    }

	// The derived class must have a static method like such:
	// static Udtf* instantiate(UdxInit *pInit);

    enum DataAvailable
    {
        MoreData=0,
        Done=1
    };
    
    // Called a minimum of once per input row or literal parameters.
    virtual DataAvailable nextOutputRow() = 0;
    
    // Called a mimimum of once after the end of input row if
    // function is registered to take it. By Default will
    // return Done
    virtual DataAvailable nextEoiOutputRow() {
        return Done;
    }

    // Called once at the begining on the input stream(called by the  engine)
    virtual void newInputStream() { }

    // Called once per input row or literal parameters(called by the  engine)
    virtual void newInputRow() = 0;

    // Called from Engine
    void performNewInputRow(TableFuncCallInfo *pInfo)
    {
	m_pInfo = pInfo;

        if (m_callNewInputStream)
        {
            newInputStream();
            m_callNewInputStream = false;
        }
        if (m_callNewInputPartition)
        {
            newInputPartition();
            m_callNewInputPartition = false;
        }
        newInputRow();
        m_pInfo = NULL;
    }
  
    // Called from engine
    DataAvailable performNextOutputRow(TableFuncCallInfo *pInfo)
    {
        DataAvailable avail;
        m_pInfo = pInfo;
        avail = nextOutputRow();
        m_pInfo = NULL;
        return avail;
    }

    // Called from engine
    DataAvailable performNextEoiOutputRow(TableFuncCallInfo *pInfo)
    {
        DataAvailable avail;
        m_pInfo = pInfo;
        if (m_callNewInputStream)
        {
          newInputStream();
          m_callNewInputStream = false;
        }
        avail = nextEoiOutputRow();
        m_pInfo = NULL;
        return avail;
    }

    // Called once at the begining on a partition if
    // function is invoked with over (called by the  engine)
    virtual void newInputPartition() {}

    // Called a mimimum of once after the end of parition if
    // function is invoked with over(). By Default will
    // return Done
    virtual DataAvailable nextEopOutputRow() {
        return Done;
    }

    // Called from engine
    DataAvailable performNextEopOutputRow(TableFuncCallInfo *pInfo)
    {
        DataAvailable avail;
        m_pInfo = pInfo;

        if (m_callNewInputStream)
        {
            newInputStream();
            m_callNewInputStream = false;
        }
        if (m_callNewInputPartition)
        {
            newInputPartition();
            m_callNewInputPartition = false;
        }
        avail = nextEopOutputRow();
        m_pInfo = NULL;
        if (avail == Done)
            m_callNewInputPartition = true;
        return avail;
    }

private:
    CorrelationType m_correlationType;
    bool m_hasFinal;
    bool m_hasOrder;
    bool m_hasPartition;
    bool m_hasOver;

    bool m_callNewInputStream;
    bool m_callNewInputPartition;
};
  
} // end namespace udx_ver2
} // end namespace nz

#endif //  #ifndef UDX_RFUNC_INCLUDED
