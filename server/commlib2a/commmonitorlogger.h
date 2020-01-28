// HeLogger.h: interface for the HeLogger class.
// Copyright (C) 2001-2011 PYR Software Ltd.  All rights reserved.
//////////////////////////////////////////////////////////////////////

#ifndef commhelogger_h
#define commhelogger_h

#include "commlib.h"

#define HE_LOGGER_LOCAL_TIME_ANSWERING ( ( UINT32 )0x00000001 )
class CommMonitorLoggerObject;
class CommMonitorLoggerConn : public CommClientConnection
{
public:
	CommMonitorLoggerObject* m_obj;

	CommMonitorLoggerConn( CommClientGuardFactory& guardFactory ) : CommClientConnection( guardFactory)
	{
		m_obj = 0;
	}

	void connected() override;

	bool closedOrDisconnected( int errCode, const char* errMsg ) override
	{
		PTRACE( "connection closed: %d '%s'", errCode, errMsg );
		return CommClientConnection::closedOrDisconnected( errCode, errMsg ); 
	}

	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
};

class CommMonitorLoggerObject  
{
public:	// You should use this part of the interface
	enum MonitorDataType
	{
		BoolType = 1,
		StringType = 2,
		UIntType = 4,
		IntType = 8
	};

	CommMonitorLoggerObject();
	void init( CommClientConnectionPool& cliPool, const char * strServerAddress, const char * strStationName, const UINT32 flags = 0 );
	void compose( CommMsgBody& msg, const char * strParameterName, const char * strValue ) const;
	void post( CommMsgBody& msg );

private:
	static void compose( CommMsgBody& msg, const UINT32 flag, const char *strParameterName, const char *strValue );

public:
	PString m_strStationName;
	const bool isLocalTimeAnsweringModeOn() const;
	virtual void posted();
	virtual void processMyMessage(const UINT32 msgId, const CommMsgBody &msg);
	virtual void connected();
	virtual ~CommMonitorLoggerObject();

private:
	bool m_isLocalTimeAnswerMode;
	CommClientConnectionPool * m_pCliPool;

protected:
	static CommClientPlainTextPasswordGuardFactory m_guardFactory;
	virtual void createHeConnection();
	CommMonitorLoggerConn * m_pConn;
};

inline void composeLoggerStat( CommMsgBody& body, const char* name, bool value )
{
	body.composeBYTE( CommMonitorLoggerObject::BoolType ).composeString( name ).composeBOOL( value );
}

inline void composeLoggerStat( CommMsgBody& body, const char* name, const char* value )
{
	body.composeBYTE( CommMonitorLoggerObject::StringType ).composeString( name ).composeString( value );
}

inline void composeLoggerStat(CommMsgBody& body, const char* name, INT32 value )
{
	body.composeBYTE( CommMonitorLoggerObject::UIntType ).composeString( name ).composeINT32( value );
}

inline void composeLoggerStat(CommMsgBody& body, const char* name, UINT32 value )
{
	body.composeBYTE( CommMonitorLoggerObject::UIntType ).composeString( name ).composeUINT32( value );
}


#endif 
