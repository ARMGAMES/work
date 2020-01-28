#ifndef commsrvcomposite_h_included
#define commsrvcomposite_h_included

#include "commsrv.h"

class _CommCompositeInterface : public _CommInterface
{

public:
	virtual void _postToPhysConnection(_CommMsg & msg, int srcMux, UINT32 srcMuxChannelId) = 0;
	virtual void _disconnectPhysConnection() = 0;
	virtual void _physConnectionConnected(int isLocal) = 0;
	virtual void _createIncomingPhysConnection (const char * address) = 0;

};

/* abstract */ class CommCompositeServerObject : public CommServerObject
{
private:
	//int slot;
	//UINT32 channelId;
	_CommCompositeInterface* inter;

public:
	CommCompositeServerObject( _CommCompositeInterface& inter_)
		: CommServerObject(inter_)
	{
		inter = &inter_;
	}
	virtual ~CommCompositeServerObject() {}
public:
	//interface functions
	virtual void processOutgoingPhysConnection (const PString & address) {}
	virtual void processPhysDisconnect () {}
	virtual void processPhysMessage (_CommMsg & msg) {}
	virtual UINT32 getInternalQueueSize() {return 0;}

	void createIncomingPhysConnection (const char * address);
	void postToPhysConnection (_CommMsg& msg, int srcMux, UINT32 srcMuxChannelId);
	void disconnectPhysConnection ();
	void physConnectionConnected (int isLocal);

public:
	//helpers for _CommCompositeLocalThread etc.
	bool _safeProcessOutgoingPhysConnect( const PString & address );//returns true if object should be destroyed
	bool _safeProcessPhysDisconnect( );//returns true if object should be destroyed
	bool _safeProcessPhysMessage( _CommMsg& msg );//returns true if object should be destroyed

};

/* abstract */ class CommCompositeServerObjectFactory 
{
public:
	virtual /* new */ CommCompositeServerObject* createServerObject( _CommCompositeInterface& inter ) const = 0;
	virtual ~CommCompositeServerObjectFactory(){}
};


#endif
