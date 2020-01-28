#ifndef commpop3_h_included
#define commpop3_h_included

#include "ppinclude.h"
#include "commmail.h"

class CommPop3
{
private:
	PString host;
	int port;
	PString user;
	PString password;

	PIPSocket* sock;
	CommSocketLineReader* in;
	int nOutstandingMessages;
	int  current;
	bool haveMore_;

public:
	CommPop3();
	~CommPop3();
	void init( const char* host_, int port_, const char* user_, const char* password_ );
	void getMail( PNewPtrVector< PString >& ret );

	void getMailDeletePending( PNewPtrVector< PString >& ret,int maxMessages = 10, int timeout = 0 );
	void getNextMailDeletePending( PNewPtrVector< PString >& v, int maxMessages =10 );
	void rollbackDelete();
	void commitDelete();
	void markAsDelete();
	bool haveMore() const
	{
		return haveMore_;
	}

private:
	void _quit();
};

#endif
