#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "ppinifile.h"
#include "pplogfile.h"
#include "ppdaemon.h"

#include "commrtable.h"
#include "commsrv.h"
#include "commsrvcomposite.h"
#include "commtmlocal.h"
#include "commtmrw.h"
#include "commtmcomposite.h"
#include "commappfactory.h"
#include "commredundant.h"
#include "commtmrwudp.h"
#include "commtmclientudp.h"
#include "commtmwebsocket.h"
#include "commdispatcher.h"

static void _loadFile( PBlock& ret, const char * fileName )
{
	PIOFile file;
	file.openFile(fileName,PLIB_FILE_READONLY);
	size_t s = file.getSize();
	file.readFile(ret.alloc(s),s);
	file.closeFile();
}

CommDispatcherDaemon::CommDispatcherDaemon()
:
//  rw( theRoutingTable ),
//  rwMux( theRoutingTable, true ),
//  rwSsl( theRoutingTable ),
local( theRoutingTable ),
//  lm (theRoutingTable,true),
//  lmMux (theRoutingTable,true),
//  rwSslPr( theRoutingTable ),
appFactory( theRoutingTable, local )
{
}

void CommDispatcherDaemon::_readCertificate( const PIniFile & iniFile, const char * sectionName, CommRWThreadManagerBaseSSL& rw)
{
	const PIniFile::Section* sectionCert = iniFile.getSection( sectionName );
	if (sectionCert)
	{
			
		PString keyFilePassword = "";
		const  char * keyFile = sectionCert->getProperty( "serverkey");
		const char* certFile = sectionCert->getProperty( "servercert");
		const char* pswd = sectionCert->getProperty( "serverkeypassword");
		if (pswd)
		{
			CommSSLSystemInfo::decodeIniFilePassword( keyFilePassword, sectionName, "serverkeypassword", pswd );
		}
		const char* sniHostName = sectionCert->getProperty( "serversni");
		if (keyFile && certFile)
		{
			PBlock kf;
			PBlock cf;
			_loadFile( kf, keyFile );
			_loadFile( cf, certFile );
			rw.setServerKeyCert( kf, cf, keyFilePassword, sniHostName );
			if (sniHostName)
				PLog( "%s: key/certificate pair enabled for SNI %s",sectionName,sniHostName );
			else
				PLog( "%s: key/certificate pair enabled",sectionName );
		}
	}
}

void CommDispatcherDaemon::_readCertificates( const PIniFile & iniFile, const char * sectionName, CommRWThreadManagerBaseSSL& rw)
{
	const PIniFile::Section* section = iniFile.getSection( sectionName );
	int i, n = section->items.size();
	for( i=0; i < n ; ++i )
	{
		const PIniFile::Item& item = section->items[ i ];
		if( item.name.equals( "serverkey" ) )
		{
			_readCertificate( iniFile, sectionName, rw);
		}
		else if( item.name.equals( "virtualcert" ) )
		{
			_readCertificate( iniFile,  item.value, rw);
		}
	}
}

void CommDispatcherDaemon::_readOptions( const PIniFile & iniFile, const char * sectionName, CommThreadManager& rw)
{
	const PIniFile::Section* section = iniFile.getSection( sectionName );
	int i, n = section->items.size();
	for( i=0; i < n ; ++i )
	{
		const PIniFile::Item& item = section->items[ i ];
		if( item.name.startsWith( "option." ) )
		{
			const char* name = item.name + 7 /* strlen("option.")*/;
			PLog( "%s Option '%s'='%s'", sectionName, name, item.value.c_str() );
			rw.setOption( name, item.value );
		}
	}
}

void CommDispatcherDaemon::start( const char* dirName, int nParms, char** parms )
{
	PLog( "Starting dispatcher daemon, PID=%d...", PID() );
	PSocket::startup();
	CommSSL::startup();

	appFactory.setDir( dirName );



	//	managers.push_back(CommRWThreadManager(theRoutingTable));

	const char* iniName = nParms > 0 ? parms[ 0 ] : "dispatcher.ini";
	int tcpMuxPort = -1;
	int smemMuxPort = -1;
#ifdef PPOSIX
	PString unixMuxName;
#endif

	PString iniFullFileName(dirName);
	iniFullFileName.append(iniName);

	appFactory.setIniFile(iniFullFileName.c_str());
	PIniFile iniFile( iniFullFileName.c_str() );

	local.setWhoAreYou("local");
	const PIniFile::Section* section = iniFile.getSection( "comm" );
	PASSERT3( section );
	const char* appFactoryName = "AppFactory";
	const char* appFactoryKeepAliveServer = 0;
	const char* appFactoryKeepAliveObject = 0;
	const char* appFactoryKeepAliveId = 0;
	int i, n = section->items.size();
	for( i=0; i < n ; ++i )
	{
		const PIniFile::Item& item = section->items[ i ];
		if( item.name.equals( "objectname" ) )
			appFactoryName = item.value;
		else if( item.name.equals( "keepaliveserver" ) )
			appFactoryKeepAliveServer = item.value;
		else if( item.name.equals( "keepaliveobject" ) )
			appFactoryKeepAliveObject = item.value;
		else if( item.name.equals( "keepaliveid" ) )
			appFactoryKeepAliveId = item.value;
		else if( item.name.equals( "identification" ) )
			theRoutingTable._setIdent(item.value);
		else if( item.name.startsWith( "option." ) )
		{
			const char* name = item.name + 7 /* strlen("option.")*/;
			PLog( "Option '%s'='%s'", name, item.value.c_str() );
			theRoutingTable.setOption( name, item.value );
		}
		else if( item.name.equals( "appfactoryfile" ) )
		{
			appFactory.setLogFile(item.value);
		}
	}

	section = iniFile.getSection( "managers" );
	if (section)
	{
		n = section->items.size();

		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if( item.name.equalsIgnoreCase( "manager" ) )
			{
				const PIniFile::Section* manSection = iniFile.getSection( item.value );
				PASSERT3( manSection );
				int manN = manSection->items.size();
				const  char * type = manSection->getProperty( "type");
				PASSERT3(type);
				CommThreadManager * rw=0;
				if (!stricmp(type,"tcp"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManager(theRoutingTable));
				}
#ifdef PWIN32
				else if (!stricmp(type,"udp"))
				{
					rw = *managers.insert(managers.end(),new CommRWUDPThreadManager(theRoutingTable));
				}
				else if (!stricmp(type,"udpclient"))
				{
					rw = *managers.insert(managers.end(),new CommClientUDPThreadManager(theRoutingTable));
				}
#ifdef UDT_DEVELOPMENT
				else if (!stricmp(type,"udt"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManagerUDT(theRoutingTable));
				}
#endif

#endif
				else if (!stricmp(type,"ws"))
				{
					rw = *managers.insert(managers.end(),new CommThreadManagerWebSocket(theRoutingTable));
				}
				else if (!stricmp(type,"tcpmux"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManager(theRoutingTable,true));
				}

#ifdef PPOSIX
				else if (!stricmp(type,"unix"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManagerUnix(theRoutingTable));
				}
				else if (!stricmp(type,"unixmux"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManagerUnix(theRoutingTable,true));
				}
#endif
				else if (!stricmp(type,"ssl"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManagerSSL(theRoutingTable));
					CommRWThreadManagerSSL * rwSsl = (CommRWThreadManagerSSL *)rw;
					_readCertificates( iniFile, item.value, *rwSsl);
					for( int i1=0; i1 < manN ; ++i1 )
					{
						const PIniFile::Item& item1 = manSection->items[ i1 ];
						if( item1.name.equals( "clienttrustedcert" ) )
						{
							PBlock block;
							_loadFile( block, item1.value );
							rwSsl->addClientTrustedCertificate( block.ptr(), block.size() );
						}
						else if( item1.name.equals( "clientrequirecertcn" ) )
							rwSsl->clientRequireServerCertificateCN( item1.value );
					}
				}
				else if (!stricmp(type,"wss"))
				{
					rw = *managers.insert(managers.end(),new CommThreadManagerWebSocketSSL(theRoutingTable));
					CommThreadManagerWebSocketSSL * rwSsl = (CommThreadManagerWebSocketSSL *)rw;
					_readCertificates( iniFile, item.value, *rwSsl);
				}

#ifdef PWIN32
				else if (!stricmp(type,"sslp"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManagerProcess(theRoutingTable));
					CommRWThreadManagerProcess * rwSslPr = (CommRWThreadManagerProcess *)rw;
					_readCertificates( iniFile, item.value, *rwSslPr);
					for( int i1=0; i1 < manN ; ++i1 )
					{
						const PIniFile::Item& item1 = manSection->items[ i1 ];
						if ( item1.name.equals( "logdir" ) )
						{
							rwSslPr->setLogFileDir(item1.value);
						}
					}
					rwSslPr->setDir(dirName);

				}
				else if (!stricmp(type,"sslmp"))
				{
					rw = *managers.insert(managers.end(),new CommRWThreadManagerMultiProcess(theRoutingTable));
					CommRWThreadManagerMultiProcess * rwSslPr = (CommRWThreadManagerMultiProcess *)rw;
					_readCertificates( iniFile, item.value, *rwSslPr);
					for( int i1=0; i1 < manN ; ++i1 )
					{
						const PIniFile::Item& item1 = manSection->items[ i1 ];
						if ( item1.name.equals( "logdir" ) )
						{
							rwSslPr->setLogFileDir(item1.value);
						}
					}
					rwSslPr->setDir(dirName);

				}

#endif
				else if (!stricmp(type,"smemmux"))
				{
					rw = *managers.insert(managers.end(),new CommLMThreadManager(theRoutingTable,true));
				}
				else if (!stricmp(type,"smem"))
				{
					rw = *managers.insert(managers.end(),new CommLMThreadManager(theRoutingTable,false));
				}
				else if (!stricmp(type,"redundant"))
				{

					rw = *managers.insert(managers.end(),new CommCompositeLocalThreadManager(theRoutingTable));
					CommCompositeLocalThreadManager * rwcl = (CommCompositeLocalThreadManager *) rw;

					CommRedundantConnectionsObjectFactory * rdFactory = (CommRedundantConnectionsObjectFactory *) *factories.insert(factories.end(),new CommRedundantConnectionsObjectFactory());
					rwcl->registerServerObjectFactory( item.value, *rdFactory );
					for( int i1=0; i1 < manN ; ++i1 )
					{
						const PIniFile::Item& item1 = manSection->items[ i1 ];
						if( item1.name.equals( "object" ) )
						{
							const char * name = item1.value;
							const char * p = strchr(name,';');
							PString objectName;
							PString objectParameters;
							if (p)
							{
								objectName.assign(name,p);
								objectParameters.assign(p+1);
							}
							else
							{
								objectName.assign(name);
							}
							CommMsgBody initMsg;
							initMsg.composeString(objectParameters);
							rwcl->createServerObject( item.value, objectName.c_str(), &initMsg );
						}

					}

				}

				else 
				{
					PASSERT3(0);
				}

				rw->setWhoAreYou(item.value.c_str());
				int i1;
				_readOptions( iniFile, item.value, *rw);
				//listenPort - last!
				for( i1=0; i1 < manN ; ++i1 )
				{
					const PIniFile::Item& item1 = manSection->items[ i1 ];
					if( item1.name.equals( "port" ) )
					{
						if (!stricmp(type,"unix") || 
							!stricmp(type,"unixmux") 
							)
						{
							PLog( "%s: Listening name %s", item.value.c_str(),item1.value.c_str() );
							rw->listenName( item1.value.c_str() );
#ifdef PPOSIX
							if (!stricmp(type,"unixmux") && unixMuxName.length() == 0)
							{
								unixMuxName = item1.value;
							}
#endif
						}
						else
						{
							int p = atoi( item1.value );
							PLog( "%s: Listening port %d", item.value.c_str(), p );
							rw->listenPort( p );
							if (!stricmp(type,"tcpmux") &&  tcpMuxPort < 0)
							{
								tcpMuxPort = p;
							}
							if (!stricmp(type,"smemmux") &&  smemMuxPort < 0)
							{
								smemMuxPort = p;
							}

						}
					}
				}
			}
		}
	}
	else
	{
		CommRWThreadManager &rw = *(CommRWThreadManager *)*managers.insert(managers.end(),new CommRWThreadManager(theRoutingTable));
		rw.setWhoAreYou("tcp");
		CommRWThreadManager &rwMux = *(CommRWThreadManager*)*managers.insert(managers.end(),new CommRWThreadManager(theRoutingTable,true));
		rwMux.setWhoAreYou("tcpmux");
		CommRWThreadManagerSSL &rwSsl= *(CommRWThreadManagerSSL*)*managers.insert(managers.end(),new CommRWThreadManagerSSL(theRoutingTable));
		rwSsl.setWhoAreYou("ssl");
#ifdef PWIN32
		CommLMThreadManager &lmMux= *(CommLMThreadManager*)*managers.insert(managers.end(),new CommLMThreadManager(theRoutingTable,true));
		lmMux.setWhoAreYou("smemmux");
		CommRWThreadManagerProcess &rwSslPr= *(CommRWThreadManagerProcess*)*managers.insert(managers.end(),new CommRWThreadManagerProcess(theRoutingTable));
		rwSslPr.setWhoAreYou("sslp");

#endif
		section = iniFile.getSection( "tcp" );
		if( section )
		{
			_readOptions( iniFile, "tcp", rw);
			//listenPort - last!
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "port" ) )
				{
					int p = atoi( item.value );
					PLog( "TCP: Listening port %d", p );
					rw.listenPort( p );
				}
			}
		}

		section = iniFile.getSection( "tcpmux" );
		if( section )
		{
			_readOptions( iniFile, "tcpmux", rwMux);
			n = section->items.size();
			//listenPort - last!
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "port" ) )
				{
					int p = atoi( item.value );
					PLog( "TCPMUX: Listening port %d", p );
					rwMux.listenPort( p );

					if( tcpMuxPort < 0 )
						tcpMuxPort = p;
				}
			}
		}
#ifdef PWIN32
		section = iniFile.getSection( "smemmux" );
		if( section )
		{
			_readOptions( iniFile, "smemmux", lmMux);
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "port" ) )
				{
					int p = atoi( item.value );
					PLog( "SMEMMUX: Listening port %d", p );
					lmMux.listenPort( p );

					if( smemMuxPort < 0 )
						smemMuxPort = p;
				}
			}
		}
#endif		

		section = iniFile.getSection( "ssl" );
		if( section )
		{
			_readOptions( iniFile, "ssl", rwSsl);
			_readCertificates( iniFile, "ssl", rwSsl);
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "clienttrustedcert" ) )
				{
					PBlock block;
					_loadFile( block, item.value );
					rwSsl.addClientTrustedCertificate( block.ptr(), block.size() );
				}
				else if( item.name.equals( "clientrequirecertcn" ) )
					rwSsl.clientRequireServerCertificateCN( item.value );
			}

			//listenPort - last!
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "port" ) )
				{
					int p = atoi( item.value );
					PLog( "SSL: Listening port %d", p );
					rwSsl.listenPort( p );
				}
			}

		}
#ifdef PWIN32
		section = iniFile.getSection( "sslp" );
		if( section )
		{
			_readOptions( iniFile, "sslp", rwSslPr);
			_readCertificates( iniFile, "sslp", rwSslPr);
			n = section->items.size();
			//listenPort - last!
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "port" ) )
				{
					int p = atoi( item.value );
					PLog( "SSLP: Listening port %d", p );
					rwSslPr.listenPort( p );
				}
			}
			rwSslPr.setDir(dirName);
		}
		section = iniFile.getSection( "sslmp" );
		if( section )
		{
			CommRWThreadManagerMultiProcess &rwSslMPr= *(CommRWThreadManagerMultiProcess*)*managers.insert(managers.end(),new CommRWThreadManagerMultiProcess(theRoutingTable));
			rwSslMPr.setWhoAreYou("sslmp");
			_readOptions( iniFile, "sslmp", rwSslMPr);
			_readCertificates( iniFile, "sslmp", rwSslMPr);
			n = section->items.size();
			//listenPort - last!
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if( item.name.equals( "port" ) )
				{
					int p = atoi( item.value );
					PLog( "SSLMP: Listening port %d", p );
					rwSslMPr.listenPort( p );
				}
			}
			rwSslMPr.setDir(dirName);
		}

#endif
	}

	section = iniFile.getSection( "objectfactories" );
	if (section)
	{
		appFactory.setServer( "ip:localhost:", appFactoryName );

		n = section->items.size();

		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if( item.name.equalsIgnoreCase( "objectfactory" ) )
			{
				const PIniFile::Section* objectSection = iniFile.getSection( item.value );
				PASSERT3( objectSection );
				const  char * type = objectSection->getProperty( "type");
				int muxPort = objectSection->getIntProperty( "port" , tcpMuxPort);
#ifdef PPOSIX
				const char * muxName = objectSection->getProperty( "name" , unixMuxName.c_str());
#endif
				PASSERT3(type);
				int nObject = objectSection->items.size();
				for( int j=0; j < nObject ; ++j )
				{
					const PIniFile::Item& item = objectSection->items[ j ];
					if (stricmp(item.name,"type") && stricmp(item.name,"port")  && stricmp(item.name,"name"))
					{
						PString exeName, logName;
						int maxNumRestarts = 0, minInterRestartPeriod = 0;
						parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
						
						if (!stricmp(type,"multiexe"))
						{
							PASSERT3(muxPort>0);
							appFactory.registerMultiExeObjectFactory( item.name, exeName,logName, muxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) );
							PLog( "MultiObject factory '%s', muxPort=%d, executable '%s' registered", item.name.c_str(),muxPort, exeName.c_str());
						}
						else if(!stricmp(type,"exe"))
						{
							PASSERT3(muxPort>0);
							appFactory.registerExeObjectFactory( item.name, exeName, logName, muxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) );
							PLog( "Object factory '%s', muxPort=%d, executable '%s' registered", item.name.c_str(),muxPort, exeName.c_str() );
						}
#ifdef PPOSIX
						else if (!stricmp(type,"unixmultiexe"))
						{
							PASSERT3(strlen(muxName) > 0);
							appFactory.registerMultiExeObjectFactory( item.name, exeName,logName, muxName );
							PLog( "MultiObject factory '%s', muxName=%s, executable '%s' registered", item.name.c_str(),muxName,exeName.c_str() );
						}
						else if(!stricmp(type,"unixexe"))
						{
							PASSERT3(strlen(muxName) > 0);
							appFactory.registerExeObjectFactory( item.name, exeName, logName, muxName );
							PLog( "Object factory '%s', muxName=%s, executable '%s'  registered", item.name.c_str(),muxName,exeName.c_str() );
						}
#endif

						else
							PASSERT3(0);
					}
				}
			}
		}
	}			
	else
	{
		PString server;
		server.append( "ip:localhost:" );
		appFactory.setServer( server, appFactoryName );

		section = iniFile.getSection( "smemmultiexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				PASSERT3( smemMuxPort > 0 );
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				appFactory.registerMultiExeObjectFactory( item.name, exeName,logName, smemMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) );
				PLog( "SmemMultiObject factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
		section = iniFile.getSection( "multiexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				PASSERT3( tcpMuxPort > 0 );
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				appFactory.registerMultiExeObjectFactory( item.name, exeName,logName, tcpMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) );
				PLog( "MultiObject factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}



		section = iniFile.getSection( "exe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				PASSERT3( tcpMuxPort > 0 );
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				appFactory.registerExeObjectFactory( item.name, exeName, logName, tcpMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) );
				PLog( "Object factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}

#ifdef PPOSIX
		section = iniFile.getSection( "unixmultiexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				PASSERT3( unixMuxName.length() > 0 );
				PString exeName;
				PString logName;
				const char* sc = strchr( item.value, ';' );
				if( sc )
				{
					exeName.assign( item.value, sc );
					logName = sc + 1;
				}
				else
				{
					exeName = item.value;
				}

				appFactory.registerMultiExeObjectFactory( item.name, exeName,logName, unixMuxName.c_str() );
				PLog( "MultiObject factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}



		section = iniFile.getSection( "unixexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				PASSERT3( unixMuxName.length() > 0 );
				PString exeName;
				PString logName;
				const char* sc = strchr( item.value, ';' );
				if( sc )
				{
					exeName.assign( item.value, sc );
					logName = sc + 1;
				}
				else
				{
					exeName = item.value;
				}

				appFactory.registerExeObjectFactory( item.name, exeName, logName, unixMuxName.c_str() );
				PLog( "Object factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
#endif 

#ifdef PWIN32
		section = iniFile.getSection( "smemexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				PASSERT3( smemMuxPort > 0 );
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				appFactory.registerExeObjectFactory( item.name, exeName,logName, smemMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) );
				PLog( "Object factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
#endif
	}


	local.registerServerObjectFactory( "CommAppFactory", appFactory );

	section = iniFile.getSection( "dispatchers" );
	if( section )
	{
		n = section->items.size();
		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			PString iniName;
			PString logName;
			PString keepAliveId;
			int keepAlive = 0;
			const char* sc = strchr( item.value, ';' );
			if( sc )
			{
				iniName.assign( item.value, sc );
				const char* sc2 = strchr( sc + 1, ';' );
				if( sc2 )
				{
					logName.assign( sc + 1, sc2 );
					const char* sc3 = strchr( sc2 + 1, ';' );
					if( sc3 )
					{
						keepAliveId.assign( sc2 + 1, sc3 );
						keepAlive = atoi( sc3 + 1 );
					}
					else
						keepAliveId = sc2 + 1;
				}
				else
					logName = sc + 1;
			}
			else
				iniName = item.value;

			appFactory.registerAndStartDispatcher( item.name, iniName, logName, keepAliveId, keepAlive );
			PLog( "Dispatcher #%d [ keepAliveId = '%s' ] registered and started", i, keepAliveId.c_str() );
		}
	}

	section = iniFile.getSection( "start" );
	if( section )
	{
		n = section->items.size();
		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			PString objectName;
			CommMsgBody initBody;
			const char* value = item.value;
			const char* scolon = strchr( value, ';' );
			if( scolon )
			{
				objectName.assign( value, scolon );
				composeSrvObjInitMsg(initBody, scolon + 1, objectName);
			}
			else
				objectName = value;

			PLog( "Creating object '%s'...", objectName.c_str() );
			PString errMsg;
			int err = appFactory.createObject( errMsg, item.name, objectName, initBody );
			if (err == COMMAPPFACTORY_ERROR_OK)
				PLog( "Object %s created", item.value.c_str() );
			else
				PLog( "Object %s wasn't created: error %d (%s)", item.value.c_str(), err, errMsg.c_str() );
		}
	}

	if( appFactoryKeepAliveServer && appFactoryKeepAliveObject && appFactoryKeepAliveId )
	{
		appFactory.setKeepAlive( appFactoryKeepAliveServer, appFactoryKeepAliveObject, appFactoryKeepAliveId );
		PLog( "CommAppFactory::setKeepAlive( '%s', '%s', '%s' )", appFactoryKeepAliveServer, appFactoryKeepAliveObject, appFactoryKeepAliveId );
	}

	section = iniFile.getSection( "logger" );
	if( section )
	{
		const char* server = section->getProperty( "server" );
		PASSERT(server);
		const char* station = section->getProperty( "station" );
		PASSERT( station );
		const char* suffix = section->getProperty( "suffix","" );
		bool reportSEH = section->getIntProperty( "reportseh",0 )!=0;
		appFactory.setLoggerInformation(server, station, suffix, reportSEH);
	}

	CommMsgBody initFactoryMsg;
	appFactory.prepareInitServerObjectMsg( initFactoryMsg );
	local.createServerObject( "CommAppFactory", appFactoryName, &initFactoryMsg );
	
	theRoutingTable.dynamicInit(iniFile);
	theRoutingTable.start();
	PLog( "Dispatcher daemon started" );
}

void CommDispatcherDaemon::stop()
{
	PLog( "Stopping dispatcher daemon..." );
	//	appFactory.killExes();
	local.shutdown();
	bool Ok =  _CommWaitUntilTerminated( local, 10 );
	if( Ok )
		PLog( "Dispatcher daemon: Local thread terminated" );
	else
	{
		PLog( "Dispatcher daemon: Local thread not terminated - continue" );
	}

	theRoutingTable.shutdown();
	Ok = theRoutingTable.waitUntilTerminated( 10 );
	if( Ok )
		PLog( "Dispatcher daemon: Threads terminated" );
	else
	{
		PLog( "Dispatcher daemon: Threads not terminated - exiting anyway" );
		FATALEXIT();
	}
	PLog( "Dispatcher daemon: Waiting for executables termination..." );
	//	appFactory.waitForExes();
	PLog( "Dispatcher daemon: Executables terminated" );

	CommSSL::cleanup();
	PSocket::cleanup();
	PLog( "Dispatcher daemon stopped" );
}
