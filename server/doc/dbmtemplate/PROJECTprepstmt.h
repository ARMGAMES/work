#ifndef PROJECTprepstmt_h
#define	PROJECTprepstmt_h

#include	"dbmstmt.h"
#include	"dbm.h"
#include    "dbmstruct.h"
#include	"pyrtime.h"

class SomeDummyStmt : public DbmStatement
{
	PSqlInt						dummy1;
    void prepareStmt();
public:
    SomeDummyStmt(DatabaseManagerCommon& man) : DbmStatement(man)
    {
        prepareStmt();
    }
    void init(UINT32 dummy1_);
};

#endif	//PROJECTprepstmt_h
