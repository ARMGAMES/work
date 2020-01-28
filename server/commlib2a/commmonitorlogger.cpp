// Copyright (C) 2001-2011 PYR Software Ltd.  All rights reserved.
//////////////////////////////////////////////////////////////////////

#include "commhercules.h"
#include "commmonitorlogger.h"
#include "commtime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CommMonitorLoggerObject::CommMonitorLoggerObject()
{
	m_isLocalTimeAnswerMode = false;
	m_pCliPool = NULL;
	m_pConn = NULL;
}

CommMonitorLoggerObject::~CommMonitorLoggerObject()
{
	try
	{
		if (m_pCliPool && m_pConn->isConnected())
		{
			m_pCliPool->disconnect(*m_pConn);
			m_pCliPool = NULL;
		}

		delete m_pConn;
	}
	catch (...)
	{
	}
}

void CommMonitorLoggerObject::init(CommClientConnectionPool& cliPool, const char * strServerAddress, const char *strStationName, const UINT32 flags)
{
	createHeConnection();
	m_pConn->m_obj = this;

	cliPool.connect( *m_pConn, strServerAddress, HERCULES_INSTANCE_NAME, HERCULES_SND_CONN_NAME );
	m_strStationName = strStationName;
	m_pCliPool = &cliPool;
	m_isLocalTimeAnswerMode = ( ( flags & HE_LOGGER_LOCAL_TIME_ANSWERING ) == HE_LOGGER_LOCAL_TIME_ANSWERING );
}

void CommMonitorLoggerObject::compose(CommMsgBody &msg, const char *strParameterName, const char *strValue) const
{
	if ( msg._size() == 0 )
		msg.composeString( m_strStationName );

	compose( msg, HERCULES_HAS_DATA, strParameterName, strValue );
}

void CommMonitorLoggerObject::post(CommMsgBody &msg)
{
	compose( msg, HERCULES_NO_MORE_DATA, "", "" );
	m_pConn->post( MSG_HERCULES_SEND_REPORT, msg );
}

void CommMonitorLoggerObject::compose(CommMsgBody &msg, const UINT32 flag, const char *strParameterName, const char *strValue)
{
	msg.composeUINT32( flag );
	msg.composeString( strParameterName );
	msg.composeString( strValue );
}


/* static */ CommClientPlainTextPasswordGuardFactory CommMonitorLoggerObject::m_guardFactory( "user", "user" );


//////////////////////////////////////////////////////////////////////

/* virtual */ void CommMonitorLoggerConn::connected()
{
	CommMsgBody body;
	body.composeString( "Hello, server!" );
	post( 0x0001, body );
}


/* virtual */ void CommMonitorLoggerConn::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	switch( msgId )
	{
	case 0x0002:
		{
			CommMsgParser parser( body );
			const char* s;
			parser.parseString( s );

			if ( m_obj->isLocalTimeAnsweringModeOn() )
			{
				CommMsgBody report;
				report.composeString( m_obj->m_strStationName );
				post( MSG_HERCULES_I_HAVE_LOCAL_TIME, report );
			}

			m_obj->connected();
			break;
		}

	default:
		m_obj->processMyMessage( msgId, body );
	}
}

/* vitual */ void CommMonitorLoggerObject::connected()
{
	// Do nothning
}

void CommMonitorLoggerObject::processMyMessage(const UINT32 msgId, const CommMsgBody &msg)
{
	CommMsgParser parser( msg );
	UINT32 resultCode;

	switch( msgId )
	{
	case MSG_HERCULES_SEND_REPORT_REPLY:
		parser.parseUINT32( resultCode );
		if ( resultCode != 0 )
		{
			const char * strErrorDescription;
			parser.parseString( strErrorDescription );
			PTRACE( "Hercules: %s", strErrorDescription );
		}

		posted();
		break;

	case MSG_HERCULES_GET_LOCAL_TIME:
		{
			CommSrvTime timeNow;
			timeNow.currentLocalTime();

			CommMsgBody reply;
			reply.composeUINT32( HERCULES_OK ).composeString( "" ).composeString( m_strStationName );
			timeNow.compose( reply );

			m_pConn->post( MSG_HERCULES_GET_LOCAL_TIME_REPLY, reply );
			break;
		}

	case MSG_HERCULES_GET_LOCAL_TIME_REPLY2:
	case MSG_HERCULES_I_HAVE_LOCAL_TIME_REPLY:
		// Do nothing
		break;

	case MSG_HERCULES_ENABLE_EMAIL_REPLY:
		posted();
		break;

	default:
		PTRACE( "Hercules: Unknown message #0x%X", msgId );
		break;
	}
}


const bool CommMonitorLoggerObject::isLocalTimeAnsweringModeOn() const
{
	return m_isLocalTimeAnswerMode;
}


/* vitual */ void CommMonitorLoggerObject::posted()
{
	// Do nothning
}

/* virtual */ void CommMonitorLoggerObject::createHeConnection()
{
	m_pConn = new CommMonitorLoggerConn( m_guardFactory );
}
