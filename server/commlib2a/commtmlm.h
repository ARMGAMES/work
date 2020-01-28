#ifndef commtmlm_h_included
#define commtmlm_h_included


#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#define _COMMSHAREDPREFIX "pyrsmem://"
#define _COMMSHAREDPREFIXLEN 10



class _CommLMAcceptThread;
class _CommLMConnectThread;
class _CommLMThreadBase;
/* abstract */ class CommLMThreadManager : public CommThreadManager
{
public:
	struct CommunicationPoint
	{
		char sharedResourceName[256];
		bool incoming;
		_CommPhysicalConnection* physConn;
	};

private:
	typedef CommThreadManager _Parent;
	bool muxEnabled;


public:
	UINT32 MaxBlockSize;
	_CommProtocolLog * comm_logging;

private:
	/* new */ _CommLMConnectThread* connThread;
	vector< /* new */ _CommLMAcceptThread* > acceptThreads;
	bool started;
	vector <PString> listenResources;

public:
	CommLMThreadManager( CommRoutingTable& routingTable_, bool muxEnabled_);
	void setOption( const char* optName, const char* optValue ) override;
	void listenResource( const char * resource );
	void listenPort( int port ) override;
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommLMThreadManager();
	void acceptCancel() override;
	bool acceptReinstate() override;

public:
	void connect( _CommPhysicalConnection* physConn ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;

	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommLMThreadManager";
	}

	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommLMThreadManager:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	void loggerInformation(CommMsgBody &body) override; 
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	bool handles( const char* address ) const override
	{
		return PString::startsWith( address, _COMMSHAREDPREFIX );
	}


private:
	_CommLMThreadBase* _createNewThread( int threadIdx );
private:
	friend class _CommLMThreadBase;
	friend class _CommLMAcceptThread;
	friend class _CommLMConnectThread;
	void _pushInSocket( const char * sockDescr, const char* addr );
	void _pushOutSocket( const CommunicationPoint& point )
	{
		_pushSocket( point );
		//no need to notify routingTable - will be notified by the RWThread after socket receipt
	}

	bool _freeSocket( int threadIdx );//to be called from RWThread
	//returns true if thread should be terminated
	//no thread operations allowed after freeSocket returned true//no thread operations allowed after freeSocket returned true
private:
	/* inline */ static _CommLMThreadBase* _rwThread( Thread& thread );
	void _pushSocket( const CommunicationPoint& sock );
};

#endif

