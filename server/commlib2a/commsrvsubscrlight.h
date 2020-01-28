#ifndef commsrvsubscrlight_h_included
#define commsrvsubscrlight_h_included

#include "ppinclude.h"
#include "pptime.h"
#include "ppthread.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commidmap.h"
#include "commtime.h"
#include "ppcontainer.h"



class CommServerPublisherLightPool;

class CommServerPublisherLight 
{
public:

	BYTE priority;

private:
	friend class CommServerPublisherLightPool;
	struct _Subscr
	{
		CommServerPublisherLight* publisher;
		UINT32 thisId;
		UINT32 peerId;
	};

private:
	CommServerPublisherLightPool* pool;
	typedef set< _Subscr* > _SubscrSet;
	_SubscrSet children;

	PString name;
	UINT32 numberOfSubscribers;
	bool   countingOn;

public:
	CommServerPublisherLight();
	bool isPublished() const
	{
		return pool != 0;
	}
	const char* publishedAs() const
	{
		return pool ? name.c_str() : 0;
	}
	const UINT32 getNumberOfSubscribers() const
	{
		if (countingOn)
			return numberOfSubscribers;
		else
			return 0;
	}

	void postUpdate( CommMsgBody& update );
	void startCountNumberOfSubscribers();
	void stopCountNumberOfSubscribers();


private:
	void _startCountNumberOfSubscribers();
};

class CommServerPublisherLightPool
{
private:
	typedef PStringMap< CommServerPublisherLight* > _PublisherMap;
	_PublisherMap publisherMap;
	bool connected;
	_CommInterface* inter;
	CommIdMap< CommServerPublisherLight::_Subscr* > subscrMap;

public:
	CommServerPublisherLightPool( _CommInterface& inter_ );
	void publish( const char* name, CommServerPublisherLight& publisher );
	bool havePublisher( const char* name ) const
	{
		return publisherMap.find( name ) != publisherMap.end();
	}
	void unpublish( const char* name );
	void _postUpdate( CommServerPublisherLight& publisher, CommMsgBody & update );

public:
	bool _processMessage( _CommMsg& msg );

private:
	friend class CommServerPublisherLight;

private:
	CommServerPublisherLight::_Subscr* _getSubscr( UINT32 thisId, BYTE msgType );
	CommServerPublisherLight::_Subscr* _getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType );
	CommServerPublisherLight::_Subscr* _lookForSubscr( UINT32 cliId );
	void _delSubscr( CommServerPublisherLight::_Subscr* subscr );
};

#endif
