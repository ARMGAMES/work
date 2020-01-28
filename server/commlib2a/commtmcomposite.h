#ifndef commtmcomposite_h_included
#define commtmcomposite_h_included


#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmlocal.h"
#include "commproto.h"


class _CommCompositeConnectThread;
class _CommCompositeLocalThread;
/* abstract */ class CommCompositeLocalThreadManager : public CommThreadManager
{
private:

	struct _Slot
	{
		int threadId;
		int slotId;
		_Slot(int tr_,int sl_)
		{
			threadId = tr_;
			slotId = sl_;
		}
	};

	typedef PStringMap <_Slot> _Slots;

	_Slots slots;

	struct _Factory
	{
		CommCompositeServerObjectFactory* factory;
		int maxObjectsPerThread;
	};
	typedef PStringMap< _Factory > _FactoriesType;
	_CommCompositeConnectThread * connThread;
	bool started;

protected:
	_FactoriesType factories;

public:
	CommCompositeLocalThreadManager( CommRoutingTable& routingTable_);
	virtual ~CommCompositeLocalThreadManager( ); 
	void setOption( const char* optName, const char* optValue ) override;
public:
	void connect( _CommPhysicalConnection* physConn ) override;
	void start() override;
	void shutdown() override;
	bool terminated() override;	
	void virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;

	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommCompositeLocalThreadManager";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommCompositeLocalThreadManager:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	bool createServerObject( const char* type, const char* name, const CommMsgBody* initMsg = 0 );
	virtual void forceDisconnect( _CommPhysicalConnection* physConn );

	bool handles( const char* address ) const override
	{
		return PString::startsWith( address, _COMMCOMPOSITPREFIX );
	}
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;


	void addServerObject(const char * name, int threadId, int slotId);
	void removeServerObject(const char * name);

	void registerServerObjectFactory( const char* typeName, CommCompositeServerObjectFactory& factory, int maxObjectsPerThread = 1 );

private:

	void		 pushOutConnection( _CommPhysicalConnection* physConn );
protected:
	friend class _CommCompositeLocalThread;
	friend class _CommCompositeConnectThread;
	bool _freeObject( int threadIdx );

};

#endif
