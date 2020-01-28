#ifndef commclisubscrlight_h_included
#define commclisubscrlight_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pptime.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commidmap.h"
#include "commconn.h"


class CommClientSubscriberLightPool;
class CommClientSubscriberLight
{
private:
	enum { _StateSubscrDisc = 0, _StateSubscrPeerAssigned = 1, _StateSubscrRequestSent = 2, _StateSubscrSync = 3, _StateSubscrDesync = 4, _StateSubscrServerFatalError = -1, _StateSubscrUnexpectedError = -2 };
	CommClientSubscriberLightPool* pool;

private:
	friend class CommClientSubscriberLightPool;
	PString server;
	PString serverObject;
	PString serverChannel;
	int state;//_StateSubscr*
	BYTE priority;
	UINT32 thisId;
	UINT32 peerId;

public:
	CommClientSubscriberLight();
	virtual ~CommClientSubscriberLight();

	bool isSubscribed() const
	{
		return pool != 0;
	}
	UINT32 id() const
	{
		return pool ? thisId : 0;
	}
	const char* getServer() const
	{
		return server;
	}
	const char* getServerObject() const
	{
		return serverObject;
	}
	const char* getServerChannel() const
	{
		return serverChannel;
	}

public:
	virtual void processUpdate(const CommMsgBody & body) {}

	virtual bool closed( int errCode, const char* errMsg );
	// errCode is on of _COMM_ERROR_* messages (commqueue.h) or 0 if gracefully closed by server
	// should return false to re-attempt establish connection, true otherwise
	// by default returns false only to _COMM_ERROR_NOLOCALOBJECT

private://safety wrappers
	void _safeProcessUpdate(const CommMsgBody & body);
	bool _safeClosed( int errCode, const char* errMsg );
};

class CommClientSubscriberLightPool
	//thread-unaware
{
private:
	enum { _ResubscribeTimeout = 1000 };

	bool connected;
	_CommInterface* inter;
	CommIdMap< CommClientSubscriberLight* > subscrMap;

public:
	CommClientSubscriberLightPool( _CommInterface& inter_ );
	void subscribe( CommClientSubscriberLight& subscr, const char* server, const char* serverObject, const char* serverChannel );
	void unsubscribe( CommClientSubscriberLight& subscr );
	~CommClientSubscriberLightPool();

public:
	bool _processMessage( _CommMsg& msg );

private:
	friend class CommClientSubscriberLight;
	void _post( _CommMsg& msg )
	{
		inter->_post( msg );
	}

private:
	CommClientSubscriberLight* _getSubscr( UINT32 thisId, BYTE msgType );
	CommClientSubscriberLight* _getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType );

	void _subscribe( CommClientSubscriberLight& subscr );
	void _resubscribe( CommClientSubscriberLight& subscr );
	void _scheduleResubscribe( CommClientSubscriberLight& subscr );
	void _postUnsubscribe( CommClientSubscriberLight& subscr );

};

#endif
