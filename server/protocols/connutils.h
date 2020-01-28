// connutils.h -	implements a method for initializing a g-connection from PIniFile
//					from a dedicated or a common section using either default or specific 
//					parameter names and connecting/reconnecting a connection based on
//					connection status and whether connect parameters are the same or not
//
//					Example:
//					bool ConnUtils::createClientConnection(ini, sectionName, cliPool, conn, serverIniParamName, objectIniParamName, typeIniParamName [, typeDefaultVal]);
//
//					or if using default ini key names ( "server", "serverobject", "servertype" ) a shorter call is possible
//
//					bool ConnUtils::createClientConnection(ini, sectionName, cliPool, conn, typeDefaultVal /* can be NULL */);
//
//					if new connection differ from ones before the connection will first be disconnected (if previously connected) and then connected using new settings
//
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012-2013 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#ifndef connutils_h_defined
#define connutils_h_defined

#include	"pplib.h"
#include	"commcli.h"

class ConnUtils
{
	// relies on the caller to provide valid pointers
	static bool isTheSame(CommClientGConnection& conn, const char * server,const char * serverObject, const char * connectionType)
	{
		return isTheSame( conn.underlyingConnection(), server, serverObject, connectionType );
	}
	static bool isTheSame(CommClientConnection& conn, const char * server,const char * serverObject, const char * connectionType)
	{
		return (	!strcmp(conn.getServer(),server)				&&
					!strcmp(conn.getServerObject(),serverObject)	&&
					!strcmp(conn.getServerConnType(),connectionType	)
			);
	}
public:
	static void getConnectionParamsPString(	PIniFile const& iniFile, 
											char const* sectionName,
											PString& address, 
											PString& instance, 
											PString& channel)
	{
		char const* address_ = ""; 
		char const* instance_= ""; 
		char const* channel_ = "";
		
		getConnectionParams( iniFile, sectionName, address_, instance_, channel_ );

		address  = address_;
		instance = instance_;
		channel  = channel_;
	}
	
	static bool getConnectionParams(	PIniFile const& iniFile, 
										char const* sectionName,
										char const*& address, 
										char const*& instance, 
										char const*& channel )
	{
		return getConnectionParams( iniFile, sectionName, address, instance, channel, NULL, NULL, NULL, NULL );
	}

	static bool getConnectionParams(	PIniFile const& iniFile, 
										char const* sectionName,
										char const*& address, 
										char const*& instance, 
										char const*& channel,
										const char* const serverName,
										const char* const serverObject,
										const char* const serverType,
										const char* serverTypeDefault
										)
	{
		PIniFile::Section const* pSection = iniFile.getSection(sectionName);
		if (!pSection)
		{
			PString err;
			err.append("Cannot read section ").append(sectionName); //lint !e1793
			PLog("%s", err.c_str());
			return false;
		}

		const char* propName = serverName ? serverName : "server";
		address = pSection->getProperty( propName );
		if ( NULL == address)
		{
			PString err;
			err.append("Cannot read address property '").append(propName).append( "' in section ").append(sectionName); //lint !e1793
			PLog("%s", err.c_str());
			return false;
		}

		propName = serverObject ? serverObject : "serverobject";
		instance = pSection->getProperty( propName );
		if (NULL == instance)
		{
			PString err;
			err.append("Cannot read instance property '").append(propName).append( "' in section ").append(sectionName); //lint !e1793
			PLog("%s", err.c_str());
			return false;
		}

		if( !serverTypeDefault )
			serverTypeDefault = "";

		propName = serverType ? serverType : "servertype";
		channel = pSection->getProperty( propName, serverTypeDefault );
		if (NULL == channel)
		{
			PString err;
			err.append("Cannot read channel property '").append(propName).append( "' in section ").append(sectionName); //lint !e1793
			PLog("%s", err.c_str());
			return false;
		}
		return true;
	}
	static void createClientConnection( const PIniFile& ini, 
										const char* sectionName, 
										CommClientConnectionPool& cliPool, 
										CommClientGConnection& conn,
										const char* serverTypeDefault )
	{
		createClientConnection( ini, sectionName, cliPool, conn, NULL, NULL, NULL, serverTypeDefault );
	}
	static void createClientConnection( const PIniFile& ini, 
		const char* sectionName, 
		CommClientConnectionPool& cliPool, 
		CommClientGConnection& conn,
		const char* const serverName,
		const char* const serverObject,
		const char* const serverType,
		const char* serverTypeDefault = NULL )
	{
		createClientConnection( ini, sectionName, cliPool, conn.underlyingConnection(), serverName, serverObject, serverType, serverTypeDefault );
	}
	static void createClientConnection( const PIniFile& ini, 
										const char* sectionName, 
										CommClientConnectionPool& cliPool, 
										CommClientConnection& conn,
										const char* const serverName,
										const char* const serverObject,
										const char* const serverType,
										const char* serverTypeDefault = NULL )
	{
		char const* address = 0;
		char const* instance = 0;
		char const* channel = 0;

		const PIniFile::Section* sec;
		sec = ini.getSection( sectionName );
		if( sec )
		{
			if( getConnectionParams(ini, sectionName, address, instance, channel, serverName, serverObject, serverType, serverTypeDefault)
				&&
				!isTheSame(conn, address, instance, channel) ) // parameters checked in the getConnectionParams
			{
				if( conn.isConnected() )
				{
					conn.closedOrDisconnected( 0,"" );

					cliPool.disconnect( conn );
				}
				cliPool.connect( conn, address, instance, channel );
			}
		}
		else
		{
			PLog("createClientConnection: Section '%s' is absent", sectionName);
		}
	}
	static void createClientConnection( CommClientConnectionPool& cliPool, 
										CommClientGConnection& conn,
										const char* const serverName,
										const char* const serverObject,
										const char* const serverType )
	{
		createClientConnection( cliPool, conn.underlyingConnection(), serverName, serverObject, serverType );
	}
	static void createClientConnection( CommClientConnectionPool& cliPool, 
										CommClientConnection& conn,
										const char* const serverName,
										const char* const serverObject,
										const char* const serverType )
	{
		// check input connection parameter pointers before calling isTheSame
		if( !serverName )
		{
			PLog( "createClientConnection: serverName is NULL" );
			return;
		}
		if( !serverObject )
		{
			PLog( "createClientConnection: serverObject is NULL" );
			return;
		}
		if( !serverType )
		{
			PLog( "createClientConnection: serverType is NULL" );
			return;
		}
		// connection parameter pointers are valid
		if( !isTheSame(conn, serverName, serverObject, serverType) )
		{
			if( conn.isConnected() )
			{
				conn.closedOrDisconnected( 0,"" );

				cliPool.disconnect( conn );
			}
		}
		if( !conn.isConnected() )
		{
			// only attempt connection if server name is not empty
			if( *serverName )
			{
				cliPool.connect( conn, serverName, serverObject, serverType );
			}
			else
			{
				PLog( "Server Name is empty." );
			}
		}
	}
	// functor to use with vectors of g connections or their pointers like so:
	// for_each( v.begin(), v.end(), DisconnectGConnFunctor( cliPool ) );
	class DisconnectGConnFunctor
	{
		CommClientConnectionPool& cliPool;
		
	public:
		DisconnectGConnFunctor( CommClientConnectionPool& cliPool_ ) : cliPool( cliPool_ ) 
		{
			PTRACE5( "DisconnectGConnFunctor %p", &cliPool );
		}
		bool disconnect( CommClientConnection& conn )
		{
			if( conn.isConnected() )
			{
				PTRACE5( "DGCF: disconnect %d", conn.id() );

				// if you need to pass a different error code or message - add code to the functor to supply it
				conn.closedOrDisconnected( 0, "" );

				cliPool.disconnect( conn );

				return true;
			}
			PTRACE5( "DGCF: %d already disconnected", conn.id() );
			return false;
		}
		bool disconnect( CommClientGConnection& conn )	{	return disconnect( conn.underlyingConnection() );	}
		bool disconnect( CommClientConnection* conn )	{	return ( conn == nullptr ) ? false : disconnect( *conn );	}
		bool disconnect( CommClientGConnection* conn )	{	return ( conn == nullptr ) ? false : disconnect( conn->underlyingConnection() );	}
		
		void operator()( CommClientGConnection&	conn ) { disconnect( conn ); }
		void operator()( CommClientGConnection*	conn ) { disconnect( conn ); }
		void operator()( CommClientConnection&	conn ) { disconnect( conn ); }
		void operator()( CommClientConnection*	conn ) { disconnect( conn ); }
	};
	class IsConnectedChecker
	{
	public:
		static bool isConnected( const CommClientConnection&	conn ) {	return conn.isConnected();	}
		static bool isConnected( const CommClientGConnection&	conn ) {	return conn.underlyingConnection().isConnected();			}
		static bool isConnected( const CommClientGConnection*	conn ) {	return ( conn == nullptr ) ? false : isConnected( *conn );	}
		static bool isConnected( const CommClientConnection*	conn ) {	return ( conn == nullptr ) ? false : conn->isConnected();	}
	};
};

#endif // connutils_h_defined
