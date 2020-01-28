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
#include "_commtmrwp.h"
#include "commoption.h"
#include "commmonitorlogger.h"

_CommRWThreadProcess::_CommRWThreadProcess( CommRWThreadManagerProcess& mgr_, int mgrIdx_ )
: _CommRWThreadBase( mgr_, mgrIdx_, false ),
mgr(mgr_)
{
	init=false;
	_statInit();
}


//****************************************************************************


CommRWThreadManagerProcess::CommRWThreadManagerProcess( CommRoutingTable& routingTable_ )
: CommRWThreadManagerBaseIP( routingTable_,false )
{
	PASSERT3(!connThread);
	connThread = new _CommRWConnectThreadIP(*this);
#ifdef PWIN64
	communicatorProcessName = "pyrssl64.exe";
#else
	communicatorProcessName = "pyrssl.exe";
#endif
}


/* virtual */ void CommRWThreadManagerProcess::setOption( const char* optName, const char* optValue )
{
	if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0)
		commLoggingName.assign(optValue);
	else if ( strcmp( optName, COMM_OPTION_COMMUNICATOR_PROCESS ) == 0)
		communicatorProcessName.assign(optValue);
	else
		CommRWThreadManagerBaseIP::setOption( optName, optValue );
}

/* virtual */ _CommRWThreadBase* CommRWThreadManagerProcess::_createNewThread( int threadIdx )
{
	return new _CommRWThreadProcess( *this, threadIdx );
}

/* virtual */ bool CommRWThreadManagerProcess::handles( const char* address ) const
{
	return CommSsl0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManagerProcess::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommSsl0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWThreadManagerProcess::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommSsl0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}
void CommRWThreadManagerProcess::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	CommRWThreadManagerBase::loggerInformation(body);

	if (isGatherStatistic())
	{
		PString s;
		s.assign("Max queue size(").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qSize);

		s.assign("Max time in queue (").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qTime);
		qSize=0;
		qTime=0;
	}
}

#endif
