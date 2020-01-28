#ifndef commtmlocal_h_included
#define commtmlocal_h_included

#include "ppinclude.h"
#include "ppthread.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commsrv.h"

class CommLocalThreadManager : public CommThreadManager
{
private:
	struct _Factory
	{
		CommServerObjectFactory* factory;
		int maxObjectsPerThread;
	};
	typedef PStringMap< _Factory > _FactoriesType;

private:
	_FactoriesType factories;

public:
	CommLocalThreadManager( CommRoutingTable& routingTable_ );
	void registerServerObjectFactory( const char* typeName, CommServerObjectFactory& factory, int maxObjectsPerThread = 1 );

public:
	bool createServerObject( const char* type, const char* name, const CommMsgBody* initMsg = 0 );
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;
	const char * whoAreYou() const override 
	{
		return "CommLocalThreadManager";
	} 

private:
	friend class _CommLocalThread;
	bool _freeObject( int threadIdx );//to be called from _CommLocalThread
	//returns true if thread should be terminated
	//no thread operations allowed after _freeThread returned true//no thread operations allowed after freeSocket returned true
};

#endif
