
#include "ColTCADbmprepstmt.h"
#include "initdatetime.h"

/////////////////////////////////////////////////////////////////////////////////////

void SampleTestingStmt::prepareStmt()
{
	prepare("INSERT INTO GENERATOR64(ObjectName, ObjectId) VALUES (?, ?)");

	bindFirstParam(objectName);
	bindNextParam(objectId);
}

void SampleTestingStmt::init(const char* name, UINT64 object_id)
{
	objectName.initCut(name);
	objectId = object_id;
}
