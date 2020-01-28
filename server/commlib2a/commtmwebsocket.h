#ifndef commtmwebsocket_h_included
#define commtmwebsocket_h_included

#include <openssl/crypto.h>
//#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ppinclude.h"
#include "commlib.h"
#include "commrtable.h"
#include "commoption.h"
#include "commtmrw.h"

/*
class _CommAcceptThreadWebSocket;
class _CommThreadWebSocketBase;


*/

class CommThreadManagerWebSocket : public CommRWThreadManagerBaseIP
{

public:

	CommThreadManagerWebSocket(CommRoutingTable& routingTable_):
		CommRWThreadManagerBaseIP(routingTable_,false)
	{
	}
	virtual ~CommThreadManagerWebSocket()
	{}
	const char * whoAreYou() const override
	{
		return "CommThreadManagerWebSocket";
	} 
	void setOption( const char* optName, const char* optValue ) override
	{
		CommRWThreadManagerBaseIP::setOption( optName, optValue );
	}
//	void loggerInformation(CommMsgBody &body) override; 
//	void dynamicInit(PIniFile& iniFile) override;

private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;

};


class CommThreadManagerWebSocketSSL : public CommThreadManagerWebSocket, public CommRWThreadManagerBaseServerSSL
{
public:

	CommThreadManagerWebSocketSSL(CommRoutingTable& routingTable_) : CommThreadManagerWebSocket(routingTable_),CommRWThreadManagerBaseServerSSL()
	{
		srv = new CommSSLServer();
	}
	~CommThreadManagerWebSocketSSL()
	{
	}
	const char * whoAreYou() const override
	{
		return "CommThreadManagerWebSocketSSL";
	} 

	_CommRWThreadBase* _createNewThread(int threadIdx) override;
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;

};


#endif