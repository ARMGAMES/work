#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmrw.h"
#include "commproto.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commtmrw.h"
#include "commtmrwplain.h"
#include "commmonitorlogger.h"





//typedef _CommCompressedCommunicator< _CommRWCommunicator<PMultiSocketRW> > _CommCompressedRWCommunicator;
//typedef _CommCompleteCommunicator< _CommCompressedRWCommunicator > _CommCompleteCompressedRWCommunicator;


//****************************************************************************

/* virtual */ _CommRWThreadBase* CommRWThreadManager::_createNewThread( int threadIdx )
{
	return new _CommRWThread< _CommRWPlain<PMultiSocketRW>,PMultiSocketRW >( *this, threadIdx, muxEnabled );
}

/* virtual */ bool CommRWThreadManager::handles( const char* address ) const
{
	return CommTcp0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommTcp0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommTcp0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}

void CommRWThreadManager::loggerInformation(CommMsgBody &body)
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
		if (isGatherStatistic()>1)
		{
			s.assign("Max slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotSize);
			s.assign("Max time in slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotTime);
			qSlotSize=0;
			qSlotTime=0;
		}
	}

}


//****************************************************************************
#ifdef PPOSIX
/* virtual */ _CommRWThreadBase* CommRWThreadManagerUnix::_createNewThread( int threadIdx )
{
	return new _CommRWThread< _CommRWPlain<PMultiSocketRW>,PMultiSocketRW >( *this, threadIdx, muxEnabled );
}

/* virtual */ bool CommRWThreadManagerUnix::handles( const char* address ) const
{
	return CommUnixProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManagerUnix::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommUnixProtocolHandler::parseSocketAddr( retAddr, address );
	port = 0;
}

/* virtual */ void CommRWThreadManagerUnix::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommUnixProtocolHandler::composeSocketAddr( retAddr, address );
}
#endif
