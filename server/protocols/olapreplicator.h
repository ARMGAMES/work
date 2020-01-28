//olapreplicator.h

#ifndef olapreplicator_h_defined
#define olapreplicator_h_defined

#include "OlapCommon.h"

class OlapReplicator
{
	DatabaseManagerCommon* dbManagerCommon;
	InTransactionUpdates _inTransactionUpdates;
	OlapManager olapManager;
	PString olapMessagesTableName;
	PString olapLastMsgOrdinalPropName;
	PString olapMessagesObjectName;
	bool dataStorageOpened;
public:
	void init(DatabaseManagerCommon& man);
	void openDataStorage();
	void closeDataStorage();

	const char* getMsgTableName() const { return olapMessagesTableName; }
	const char* getMsgObjectName() const { return olapMessagesObjectName; }
	const char* getOrdinalPropName() const { return olapLastMsgOrdinalPropName; }
	void setNames(const char* olapMessagesTableName_, const char* olapMessagesObjectName_, const char* olapLastMsgOrdinalPropName_)
	{
		PASSERT(!dataStorageOpened);
		olapMessagesTableName = olapMessagesTableName_;
		olapMessagesObjectName = olapMessagesObjectName_;
		olapLastMsgOrdinalPropName = olapLastMsgOrdinalPropName_;
	}
	OlapReplicator();
	OlapReplicator( const char* olapMessagesTableName_, const char* olapMessagesObjectName_, const char* olapLastMsgOrdinalPropName_);
	virtual ~OlapReplicator()
	{
		PLog("Bye ~OlapReplicator(%p)", this);
	}
/*lint -save -e1536 */
	OlapManager* _getOlapManager() { return &olapManager; }
/*lint -restore */
	virtual void commitTransactionMessages(CommServerObjectWithOlapInteface* dbmObj);
	virtual void rollbackTransactionMessages();
	virtual void insertTransactionUpdate(const UINT32 msgId, CommMsgBody& body, BYTE msgMask, const OlapMsgSysInfo& sysInfo);
};

class OlapReplicatorObject
{

};

#endif //olapreplicator_h_defined
