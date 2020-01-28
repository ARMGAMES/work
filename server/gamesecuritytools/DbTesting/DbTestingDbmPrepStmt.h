#ifndef DbTestingDbmPrepStmt_h_included
#define DbTestingDbmPrepStmt_h_included

//////////////////////////////////////////////////////////////////////////

#include "dbmstmt.h"

//////////////////////////////////////////////////////////////////////////

class GetLastFinishedAggregationStmt : public DbmStatement
{
public:
	explicit GetLastFinishedAggregationStmt(DatabaseManagerCommon& man);

	void getLastDate(SrvDate& lastDate_) const { lastDate.getValue(lastDate_); }

private:
	// Statement output data
	PSqlDate lastDate;
};


#endif // DbTestingDbmPrepStmt_h_included
