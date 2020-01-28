#ifndef __sqlerror_h__
#define __sqlerror_h__

#define STRING_TOO_LONG				34 //moved from dbm.h

#define DB2_INTERNAL_ERROR			(-2)
#define	DB2_SQL_ERROR				(-1)
#define	DB2_NO_ERROR				0

class PSqlError : public PError
{
	INT16    errCode;

public:

	PString	 sqlState;

	PSqlError( const char* msg_ ) : PError( msg_ ){ errCode = DB2_SQL_ERROR; }///DBM_SQL_ERROR

	PSqlError( const char* msg_ , const PString& sqlState_) : PError( msg_ )
	{ 
		errCode = DB2_SQL_ERROR;                 ///DBM_SQL_ERROR
		sqlState.assign(sqlState_);  
	}

	PSqlError( int err_code, const char* msg_, bool trace = false ) : PError( msg_ )
	{ 
		if(trace)
		{
			PLog("PSqlError #%d '%s'", err_code, msg_);
		}
		errCode = err_code; 
	}
	int code() const { return errCode; }
	bool timedOutDb2Query() const { return sqlState.equals("HY008"); }
};

#endif
