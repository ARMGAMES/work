//olapreplicatordummy.cpp
#include "olapreplicator.h"
void OlapReplicator::commitTransactionMessages( CommServerObjectWithOlapInteface* dbmObj ){}
void OlapReplicator::rollbackTransactionMessages(){}
void OlapReplicator::insertTransactionUpdate( const UINT32 msgId, CommMsgBody& body, BYTE msgMask, const OlapMsgSysInfo& sysInfo ){}
void OlapReplicator::openDataStorage(){}
void OlapReplicator::closeDataStorage(){}
