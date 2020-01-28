#ifndef logmessage_h_included
#define logmessage_h_included

#include "commmsgbody.h"
#include "olapmsghandler.h"

struct LogMessage
{
	UINT32			msgId;
	CommMsgBody		msgBody;
	BYTE			msgMask;

	OlapMsgSysInfo sysInfo; // #22086

	LogMessage() : sysInfo(OlapMsgSysInfo::eOlapGroupDefault, eHost_Unknown, MAX_DBM_ID_SUPPORTED) {msgId = 0; msgMask = 0;}
	LogMessage(OlapMsgSysInfo::eOlapDataGroup dataGroup) : sysInfo(dataGroup, eHost_Unknown, MAX_DBM_ID_SUPPORTED) {msgId = 0; msgMask = 0;}
	LogMessage( const LogMessage& other ) : sysInfo(other.sysInfo)
	{
		_assign(other);
	}
	LogMessage& operator=( const LogMessage& other )
	{
		if (this != &other)
		{
			_assign(other);
		}
		return *this;
	}
private:
	void _assign(const LogMessage& other)
	{
		msgId = other.msgId;
		msgMask  = other.msgMask;
		msgBody.copyFrom( other.msgBody );
		sysInfo = other.sysInfo;
	}
};

#endif // logmessage_h_included
