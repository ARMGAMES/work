#include "ppinclude.h"
#ifdef PWIN32
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commproto.h"
#include "commtmrw.h"
#include "_commtcp0.h"
#include "_commtmrw.h"
#include "_commtmrwpc.h"
#include "commtmclient.h"
#include "commoption.h"


_CommRWThreadProcessClient::_CommRWThreadProcessClient( CommRWThreadManagerProcessClient& mgr_, int mgrIdx_ )
: _CommRWThreadBase( mgr_, mgrIdx_, false ),
mgr(mgr_)
{
	init=false;
}


//****************************************************************************


CommRWThreadManagerProcessClient::CommRWThreadManagerProcessClient( CommRoutingTable& routingTable_ )
: CommRWThreadManagerBaseIP( routingTable_,false )
{
	httpsProxyPort = 0;
	tcpNoDelay = false;
	sslConnectTimeout = 0;
	tcpBuffer = -1;
	communicatorProcessName="PokerStarsCommunicate.exe";
	PASSERT3(!connThread);
	connThread = new _CommRWConnectThreadIP(*this);
}


/* virtual */ void CommRWThreadManagerProcessClient::setOption( const char* optName, const char* optValue )
{
	if( strcmp( optName, COMM_OPTION_HTTPS_PROXY ) == 0 )
	{
		const char* col = strchr( optValue, ':' );
		if( col == 0 )
		{
			PLog( "CommSimpleThreadManagerBase: Invalid COMM_OPTION_HTTPS_PROXY option - no ':' found" );
			return;
		}
		httpsProxy.assign( optValue, col );
		httpsProxyPort = atoi( col + 1 );
	}
	else if( strcmp( optName, COMM_OPTION_SSLCONNECT ) == 0 )
		sslConnectTimeout = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_TCPNODELAY ) == 0 )
		tcpNoDelay = atoi( optValue ) != 0;
	else if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0)
		commLoggingName.assign(optValue);
	else if( strcmp( optName, COMM_OPTION_TCPBUFFER ) == 0 )
		tcpBuffer = atoi( optValue );
	else if ( strcmp( optName, COMM_OPTION_COMMUNICATOR_PROCESS ) == 0)
		communicatorProcessName.assign(optValue);
	else
		CommRWThreadManagerBaseIP::setOption( optName, optValue );
}

/* virtual */ _CommRWThreadBase* CommRWThreadManagerProcessClient::_createNewThread( int threadIdx )
{
	return new _CommRWThreadProcessClient( *this, threadIdx );
}

/* virtual */ bool CommRWThreadManagerProcessClient::handles( const char* address ) const
{
	return CommSsl0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManagerProcessClient::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommSsl0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWThreadManagerProcessClient::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommSsl0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}

#endif
