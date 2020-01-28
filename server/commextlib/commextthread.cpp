#include "commextthread.h"


void CommExtThread::postToParent( UINT32 msgId, CommMsgBody& body)
{
	if (!pool)
		return;
	_CommMsg msg( _COMM_MSGTYPE_INTERNAL_COMMUNICATION, _COMM_MSGPRIORITY_INITCONNECT );
	msg.body.moveFrom( body );
	msg.internal.patch0 = thisId;
	msg.internal.patch1 = msgId;
	return pool->_postToParent( msg );
}
void CommExtThread::_finished()
{
	if (!pool)
		return;
	_CommMsg msg( _COMM_MSGTYPE_INTERNAL_COMMUNICATION_CLOSE, _COMM_MSGPRIORITY_INITCONNECT );
	msg.internal.patch0 = thisId;
	return pool->_postToParent( msg );
}
