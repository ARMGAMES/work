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
#include "_commtmrwmp.h"
#include "commoption.h"


//****************************************************************************


CommRWThreadManagerMultiProcess::CommRWThreadManagerMultiProcess( CommRoutingTable& routingTable_ )
: CommRWThreadManagerProcess( routingTable_ )
{
}


/* virtual */ void CommRWThreadManagerMultiProcess::setOption( const char* optName, const char* optValue )
{
	if ( strcmp( optName, COMM_OPTION_NUMBEROFCONNECTIONSPERTHREAD ) == 0)
	{
		PString b;
		b.appendInt(atoi(optValue) * _SlotsForMultiProcess::MaxSlot);
		CommRWThreadManagerProcess::setOption( optName, b.c_str() );
	}
	else
		CommRWThreadManagerProcess::setOption( optName, optValue );
}

/* virtual */ _CommRWThreadBase* CommRWThreadManagerMultiProcess::_createNewThread( int threadIdx )
{
	return new _CommRWThreadMultiProcess( *this, threadIdx );
}

_CommRWThreadMultiProcess::_CommRWThreadMultiProcess( CommRWThreadManagerMultiProcess& mgr_, int mgrIdx_ )
	: _CommRWThreadBase( mgr_, mgrIdx_, false ),
	mgr(mgr_)
{
	maxActiveSlots = 0;
	_statInit();
}


#endif
