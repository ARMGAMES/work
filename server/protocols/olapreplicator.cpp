//olapreplicator.cpp
#include "olapreplicator.h"

OlapReplicator::OlapReplicator()
{
	olapMessagesTableName = OLAPMESSAGES_TABLE_NAME;
	olapMessagesObjectName = OLAPMESSAGES_OBJECT_NAME;
	olapLastMsgOrdinalPropName = PROP_LAST_MSG_ORDINAL_OLAP;
	dbManagerCommon = 0;
	dataStorageOpened = false;
}

OlapReplicator::OlapReplicator( const char* olapMessagesTableName_, const char* olapMessagesObjectName_, const char* olapLastMsgOrdinalPropName_ ) 
:
olapMessagesTableName(olapMessagesTableName_),
olapLastMsgOrdinalPropName(olapLastMsgOrdinalPropName_),
olapMessagesObjectName(olapMessagesObjectName_)
{
	dbManagerCommon = 0;
	dataStorageOpened = false;
}

void OlapReplicator::commitTransactionMessages( CommServerObjectWithOlapInteface* dbmObj )
{
	_inTransactionUpdates.setSequenceEnd();
	_inTransactionUpdates.commit(dbmObj);
}

void OlapReplicator::rollbackTransactionMessages()
{
	_inTransactionUpdates.rollback();
}

void OlapReplicator::insertTransactionUpdate( const UINT32 msgId, CommMsgBody& body, BYTE msgMask, const OlapMsgSysInfo& sysInfo )
{
	_inTransactionUpdates.insertMessage(msgId, body, msgMask, sysInfo); 
}

void OlapReplicator::openDataStorage()
{
	if(dbManagerCommon)
		olapManager.prepareStatements(*dbManagerCommon);
	dataStorageOpened = true;
}

void OlapReplicator::closeDataStorage()
{
	olapManager.deleteStatements();
	dataStorageOpened = false;
}

void OlapReplicator::init( DatabaseManagerCommon& man )
{
	dbManagerCommon = &man;
	olapManager.init(dbManagerCommon);
}