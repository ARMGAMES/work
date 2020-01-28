#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commssl.h"
#include "commoption.h"
#include "commmonitorlogger.h"

#define _TIMER_RECONN		0xFF
#define _TIMER_RESUBSCR		0xFE
#define _TIMER_DELSUBSCR	0xFD
#define _TIMER_SUBSCR_COUNTING	0xFC

#define _TIMER_PERFORMANCE_COUNTING	0xFB

#ifdef COMMLIB_CALCULATE_STATISTICS
#define _TIMER_PERFORMANCE_TRAFFIC_COUNTING	0xFA
#endif
#define _TIMER_PHYSRECONNECT 0xF0
#define _TIMER_SUBSCR_LIGHT_COUNTING	0xF9
#define _TIMER_RESUBSCR_LIGHT			0xF8




#define TIME_SANITY_CHECK	1000

#define LOCK_BRACKET_ON { PWLock lock(cs); if (doCounting) {t1_1=SYSTEMTICKS();}

#define LOCK_BRACKET_OFF  if (doCounting) {tickCounter[(BYTE)msg.type]+=((t1=SYSTEMTICKS())-t0); tickCounterAfterLock[(BYTE)msg.type]+=(t1-t1_1);enterCounter[(BYTE)msg.type]++; if (srcPhysConn && srcPhysConn->mgr) srcPhysConn->mgr->setStatistic(srcPhysConn,t1-t0,t1-t1_1);}}

#if defined(P_USE_READWRITE_LOCK)||defined(P_USE_READWRITE_LOCK_VERSION_2)
#define LOCK_BRACKET_ON_READ { PRLock lock(cs); if (doCounting) {t1_1=SYSTEMTICKS();}
#define LOCK_BRACKET_OFF_READ  if (doCounting) {PInterlockedExchangeAdd(&tickCounter[(BYTE)msg.type], ((t1=SYSTEMTICKS())-t0)); PInterlockedExchangeAdd(&tickCounterAfterLock[(BYTE)msg.type],(t1-t1_1));PInterlockedIncrement(&enterCounter[(BYTE)msg.type]); if (srcPhysConn && srcPhysConn->mgr) srcPhysConn->mgr->setStatistic(srcPhysConn,t1-t0,t1-t1_1);}}
#else
#define LOCK_BRACKET_ON_READ { PWLock lock(cs); if (doCounting) {t1_1=SYSTEMTICKS();}
#define LOCK_BRACKET_OFF_READ  if (doCounting) {tickCounter[(BYTE)msg.type]+=((t1=SYSTEMTICKS())-t0); tickCounterAfterLock[(BYTE)msg.type]+=(t1-t1_1);enterCounter[(BYTE)msg.type]++; if (srcPhysConn && srcPhysConn->mgr) srcPhysConn->mgr->setStatistic(srcPhysConn,t1-t0,t1-t1_1);}}
#endif





static inline char* _dup( const char* s )
{
	return s ? strdup( s ) : 0;
}

static inline void _free( char* s )
{
	if( s )
		free( s );
}

#ifndef COMM_CLIENT_ONLY

#define TNV_SALT_1              "zeiDnc8JK"

bool CommRoutingTable::_internalEncryption(PString &ret, const _RestrictionParms & parm, int index) const
{
	try
	{
		//tablen.$0.$1
		if (index!=2)
			return false;
		const char * key = 0;
		for (int i=0;i<_config.versionEncryption.size();i++)
		{
			if (!strncmp(_config.versionEncryption[i].version.c_str(), parm[0].begin, _config.versionEncryption[i].version.length()))
			{
				key = _config.versionEncryption[i].key;
				break;
			}
		}
		if (!key)
			return false;

		PBlock decoded, decrypted;
		CommSSLBase64::decode(decoded, parm[1].begin, parm[1].end - parm[1].begin);
		if (decoded.size() < 2)
			return false;
		BYTE chksum = decoded.ptr()[0];
		BYTE* salt = (BYTE*) TNV_SALT_1;
		CommSSLStreamDecrypt cryptor(key, salt);
		cryptor.decrypt(&decoded.ptr()[1], decoded.size() - 1, decrypted);
		UINT32 sum = 0;
		for (int i = 0; i < decrypted.size(); ++i)
			sum += decrypted.ptr()[i];
		if (chksum != sum % 256)
			return false;

		ret.assign("table.");
		ret.append( (const char*)decrypted.ptr(), decrypted.size() );
		return true;
	}
	catch (PError &err)
	{
		PLog("_internalEncryption: error %s", err.why());
		return false;
	}
}
#endif

void CommRule::parseMask()
{
	maskObject = "";
	maskChannel = "";
	maskPatterns.clear();

	const char* p = strchr( mask, '\\' );
	if( p )
	{
		const char* pp = strchr( p + 1, '\\' );
		if( pp )
		{
			maskObject.assign( p + 1, pp );
			maskChannel.assign( pp + 1 );
		}
		else
		{
			maskObject.assign( p + 1 );
		}
	}
	
	PString buf;
	for( int i = 0; i < mask.length(); i++ )
	{
		if( mask[i] == '\\' )
			break;
		if( mask[i] == '*' )
		{
			if( 0 == maskPatterns.size() || buf.length() ) // skip consecutive '*' 
				maskPatterns.push_back( buf );
			buf = "";
		}
		else
		{
			buf.append( mask[i] );
		}
	}
	maskPatterns.push_back( buf );

	PTRACE5( "parseMask( %s ) obj '%s' chan '%s'", mask.c_str(), maskObject.c_str(), maskChannel.c_str() );
	for( int i = 0; i < maskPatterns.size(); i++ )
	{
		PTRACE5( "  mask%u '%s'", i, maskPatterns[i].c_str() );
	}
}

#ifdef COMMLIB_CALCULATE_STATISTICS

int CommRoutingTable::getCounterForTrafficStatistics(const vector <_TrafficStatistics> & trafficStat, const char *server, const char * serverObject, const char * serverChannel)
{
	for (int i=0; i<trafficStat.size();i++)
	{
		if (PString::compareWithWildChar(trafficStat[i].server.c_str(),server))
		{
			if (PString::compareWithWildChar(trafficStat[i].serverObject.c_str(),serverObject))
			{
				for (int j=0; j<trafficStat[i].serverChannels.size();j++)
				{
					if (!PString::compareWithWildChar(trafficStat[i].serverChannels[j].serverChannel.c_str(),serverChannel))
					{
						return trafficStat[i].serverChannels[j].counterPosition;
					}
				}
				return trafficStat[i].defaultCounterPosition;
			}
		}
	}
	return 0;
}

void CommRoutingTable::addTrafficStatisticsName(PString *names, const char * parameter)
{
	const char * p = strchr(parameter,';');
	if (p)
	{
		int position = atoi(parameter);
		if (position < 0 || position >=MAX_STAT_TRAFFIC_COUNTERS)
		{
			PTRACE3("Traffic counting: parameter %s has illegal position", parameter);
			return;
		}
		names[position] = p+1;
	}
}

void CommRoutingTable::addTrafficStatistics(vector <_TrafficStatistics> & trafficStat, UINT32 & maxCounter, const char * parameter)
{
	const char * p = strchr(parameter,';');
	if (p)
	{
		int position = atoi(parameter);
		PString server;
		PString serverObject;
		if (position < 0 || position >= MAX_STAT_TRAFFIC_COUNTERS)
		{
			PTRACE3("Traffic counting: parameter %s has illegal position", parameter);
			return;
		}
		const char * pp = strchr(++p,';');
		if (!pp)
		{
			PTRACE3("Traffic counting: illegal parameter %s", parameter);
			return;
		}
		server.assign(p,pp);
		p=pp+1;
		pp = strchr(p,';');
		if (pp)
			serverObject.assign(p,pp);
		else
			serverObject.assign(p);

		struct _TrafficStatistics & st = *trafficStat.insert(trafficStat.end(),_TrafficStatistics());
		st.defaultCounterPosition = position;
		maxCounter = maxCounter > (st.defaultCounterPosition + 1) ? maxCounter : (st.defaultCounterPosition + 1);
		st.server = server;
		st.serverObject = serverObject; 
		PTRACE3("Traffic counting for object '%s/%s' added, default=%d",st.server.c_str(), st.serverObject.c_str(),st.defaultCounterPosition);

		if (!pp)
			return;
		p=pp+1;
		for (;;)
		{
			position = atoi(p);
			if (position < 0 || position >= MAX_STAT_TRAFFIC_COUNTERS)
			{
				PTRACE3("Traffic counting: parameter %s has illegal position for channel %s", parameter, p);
				return;
			}
			p = strchr(p,';');
			if (!p)
				return;
			pp = strchr(++p,';');
			_TrafficStatistics::_TrafficStatisticsChannels & stc = *st.serverChannels.insert(st.serverChannels.end(), _TrafficStatistics::_TrafficStatisticsChannels());
			if (pp)
				stc.serverChannel.assign(p,pp);
			else
				stc.serverChannel.assign(p);
			stc.counterPosition=position;
			maxCounter = maxCounter > ( stc.counterPosition +1 ) ? maxCounter : (stc.counterPosition +1 );
			PTRACE3("      channel '%s' added, position=%d",stc.serverChannel.c_str(),stc.counterPosition);
			if (!pp)
				return;
			p=pp+1;
		}
	}
}

#endif
//****************************************************************************

class _CommRoutingTableTimerThread : public _CommTimerThread
{
private:
	CommRoutingTable* routingTable;

public:
	_CommRoutingTableTimerThread( CommRoutingTable* routingTable_ )
	{
		routingTable = routingTable_;
	}

public://overridden
	void timer( _CommMsg& msg ) override;
};

/* virtual */ void _CommRoutingTableTimerThread::timer( _CommMsg& msg )
{
	if( ! routingTable->_processTimerMessageRead( msg ) )
		routingTable->_processTimerMessage( msg );
}

//****************************************************************************

CommRoutingTable::_PhysConn::_PhysConn( const char* addr, CommThreadManager* mgr_, bool inbound_ )
{
	address = addr;
	mgr = mgr_;
	thread = -1;
	slot = -1;
	state = _StatePhysConnDisc;
	isLocal = 0;
	inbound = inbound_;
	qualityRequired = false;
	quality = -1;
	connCreated=SYSTEMTICKS();
	lastSubscrTime=connCreated;
	subscrCounter=0;
	ddosProtectionAttempts = 0;
}

//****************************************************************************

CommRoutingTable::CommRoutingTable()
	: _config( _writableConfig )
{
	_rand=(UINT32) TIME(0);
	_subscrUniqueId=0;
	shuttingDown = false;
	timerThread = new _CommRoutingTableTimerThread( this );

	optReconnTimeout = COMM_OPTION_TIMEOUT_RECONN_DEFAULT;
	optSubscrExpire = COMM_OPTION_TIMEOUT_SUBSCREXPIRE_DEFAULT;
	optTimeoutSubscrCounting = COMM_OPTION_TIMEOUT_COUNTING_DEFAULT;
	optTimeoutPerformanceCounting = COMM_OPTION_TIMEOUT_PERFORMANCE_COUNTING_DEFAULT;
#ifdef COMMLIB_CALCULATE_STATISTICS
	optTimeoutTrafficCounting = COMM_OPTION_TIMEOUT_TRAFFIC_COUNTING_DEFAULT;
	printTrafficLast=false;
#endif
	optPhysConnLocationAttempts = COMM_OPTION_LOCATION_ATTEMPT_DEFAULT;
	optLocationFailureExpiredPeriod = COMM_OPTION_LOCATION_FAILURE_EXPIRED_PERIOD_DEFAULT;

	optMaxPhysConnFailures = 0;//unlimited
    optDisableAdjustFailureWeight = 0;
	optPhysConnFailuresTimeout = 0;//unlimited
	doCounting = false;
	doTrafficCounting = false;
	memset((void *)tickCounter, 0, sizeof(tickCounter));
	memset((void *)tickCounterAfterLock, 0, sizeof(tickCounterAfterLock));
	memset((void *)tickCounterInOneMsg, 0, sizeof(tickCounterInOneMsg));
	memset(tickCounterInOneMsgSub, 0, sizeof(tickCounterInOneMsgSub));
	memset((void *)enterCounter, 0, sizeof(enterCounter));
	memset(tickCounterTimer, 0, sizeof(tickCounterTimer));
	memset(tickCounterAfterLockTimer, 0, sizeof(tickCounterAfterLockTimer));
	memset(enterCounterTimer, 0, sizeof(enterCounterTimer));

	_totalTicksAfterLock=0;
	_totalMessages=0;
	_maxThreadTime=0;
	_statisticTimeouts = 0;
	_statisticDelays=0;
	_statisticDisconnects=0;
	_statisticConnectionDenied=0;
	_statisticSubscriptionDenied=0;

	exitOnFatalError=false;
	sendNewSubscriptionRequests = false;
	acceptNewSubscriptionRequests = false;
	percentOfDelItemsToStore = 0;
	massCrashNumProcessesThreshold = 0;
	massCrashTimeThreshold = 1000;
	autoRestartParamsResetTimerFreq = 0;

#ifdef COMMLIB_CALCULATE_STATISTICS
	_maxSubscrTrafficCounter=0;
	_maxConnTrafficCounter=0;
	memset(_subscrTrafficStatCounters,0,sizeof(_subscrTrafficStatCounters));
	memset(_connTrafficStatCounters,0,sizeof(_connTrafficStatCounters));
	memset(_subscrTrafficStatPacketCounters,0,sizeof(_subscrTrafficStatPacketCounters));
	memset(_connTrafficStatPacketCounters,0,sizeof(_connTrafficStatPacketCounters));
#endif
}

CommRoutingTable::_Config::_Config()
{
#ifndef COMM_CLIENT_ONLY
	restrictionConnectionDebug = 0;
	restrictionSubscriptionDebug = 0;
	restrictionConnectionTest = 0;
	restrictionSubscriptionTest = 0;
	restrictionForwardEnabled = 1;
#endif
}

#ifdef COMMLIB_CALCULATE_STATISTICS
void CommRoutingTable::_clearTrafficCounters()
{
	_subscrTrafficStat.clear();
	_maxSubscrTrafficCounter = 0;
	_connTrafficStat.clear();
	_maxConnTrafficCounter = 0;
	for( int i = 0; i < MAX_STAT_TRAFFIC_COUNTERS; ++i )
	{
		_connTrafficNames[i].clear();
		_subscrTrafficNames[i].clear();
	}
}
#endif

bool CommRoutingTable::_setDynamicOption( const char* optName, const char* optValue )
{
#ifdef COMMLIB_CALCULATE_STATISTICS
	if( strcmp( optName, COMM_OPTION_TIMEOUT_TRAFFIC ) == 0 )
	{
		optTimeoutTrafficCounting = atoi( optValue );
		PASSERT3( optTimeoutTrafficCounting > 0 );
		return true;
	}
	if( strcmp( optName, COMM_OPTION_SUBSCR_TRAFFIC_COUNTER ) == 0 )
	{
		addTrafficStatistics(_subscrTrafficStat,_maxSubscrTrafficCounter,optValue);
		PASSERT3(_maxSubscrTrafficCounter <= MAX_STAT_TRAFFIC_COUNTERS);
		return true;
	}
	if( strcmp( optName, COMM_OPTION_CONN_TRAFFIC_COUNTER ) == 0 )
	{
		addTrafficStatistics(_connTrafficStat,_maxConnTrafficCounter,optValue);
		PASSERT3(_maxConnTrafficCounter <= MAX_STAT_TRAFFIC_COUNTERS);
		return true;
	}
	if( strcmp( optName, COMM_OPTION_CONN_TRAFFIC_NAME ) == 0 )
	{
		addTrafficStatisticsName(_connTrafficNames,optValue);
		return true;
	}
	if( strcmp( optName, COMM_OPTION_SUBSCR_TRAFFIC_NAME ) == 0 )
	{
		addTrafficStatisticsName(_subscrTrafficNames,optValue);
		return true;
	}
#endif

	if (strcmp(optName, COMM_OPTION_MASS_CRASH_NUM_PROCESSES_THRESHOLD) == 0)
	{
		massCrashNumProcessesThreshold = atoi(optValue);
		return true;
	}
	if (strcmp(optName, COMM_OPTION_MASS_CRASH_TIME_THRESHOLD) == 0)
	{
		massCrashTimeThreshold = atoi(optValue);
		return true;
	}
	if (strcmp(optName, COMM_OPTION_AUTO_RESTART_PARAMS_RESET_TIMER_FREQUENCY) == 0)
	{
		autoRestartParamsResetTimerFreq = atoi(optValue);
		return true;
	}

	return false;
}

void CommRoutingTable::setOption( const char* optName, const char* optValue )
{
	PWLock lock( cs );
	if( strcmp( optName, COMM_OPTION_TIMEOUT_RECONN ) == 0 )
	{
		optReconnTimeout = atoi( optValue );
		PASSERT3( optReconnTimeout > 0 );
	}
	else if( strcmp( optName, COMM_OPTION_TIMEOUT_SUBSCREXPIRE ) == 0 )
	{
		optSubscrExpire = atoi( optValue );
		PASSERT3( optSubscrExpire > 0 );
	}
	else if( strcmp( optName, COMM_OPTION_MAXPHYSICALFAILURES ) == 0 )
	{
		optMaxPhysConnFailures = atoi( optValue );
		PASSERT3( optMaxPhysConnFailures >= 0 );
	}
    else if( strcmp( optName, COMM_OPTION_DISABLE_ADJUST_FAILURE_WEIGHT ) == 0 )
    {
        optDisableAdjustFailureWeight = atoi( optValue );
    }
	else if( strcmp( optName, COMM_OPTION_LOCATION_ATTEMPTS ) == 0 )
	{
		optPhysConnLocationAttempts = atoi( optValue );
		PASSERT3( optPhysConnLocationAttempts >= 0 );
	}
	else if( strcmp( optName, COMM_OPTION_LOCATION_FAILURE_EXPIRED_PERIOD ) == 0 )
	{
		optLocationFailureExpiredPeriod = atoi( optValue );
		PASSERT3( optLocationFailureExpiredPeriod >= 0 );
	}
	else if( strncmp( optName, "locationpriority", strlen("locationpriority") ) == 0 )
	{
		PASSERT5( 0 );
		// moved to setLocationPriority()
	}
	else if( strcmp( optName, COMM_OPTION_PHYSICALFAILURESTIMEOUT ) == 0 )
	{
		optPhysConnFailuresTimeout = atoi( optValue );
		PASSERT3( optPhysConnFailuresTimeout >= 0 );
	}
	else if( strcmp( optName, COMM_OPTION_TIMEOUT_SUBSCRCOUNTING ) == 0 )
	{
		optTimeoutSubscrCounting = atoi( optValue );
		PASSERT3( optTimeoutSubscrCounting > 0 );
	}
	else if( strcmp( optName, COMM_OPTION_EXITONFATALERROR ) == 0 )
	{
		exitOnFatalError = atoi( optValue ) != 0;
	}
	else if( strcmp( optName, COMM_OPTION_SEND_NEW_SUBSCRIPTION ) == 0 )
	{
		sendNewSubscriptionRequests = atoi( optValue ) != 0;
	}
	else if( strcmp( optName, COMM_OPTION_ACCEPT_NEW_SUBSCRIPTION ) == 0 )
	{
		acceptNewSubscriptionRequests = atoi( optValue ) != 0;
	}
	else if( strcmp( optName, COMM_OPTION_PERCENT_OF_DELETED ) == 0 )
	{
		percentOfDelItemsToStore = atoi( optValue );
	}
	else if (strcmp(optName, COMM_OPTION_MASS_CRASH_NUM_PROCESSES_THRESHOLD) == 0)
	{
		massCrashNumProcessesThreshold = atoi(optValue);
	}
	else if (strcmp(optName, COMM_OPTION_MASS_CRASH_TIME_THRESHOLD) == 0)
	{
		massCrashTimeThreshold = atoi(optValue);
	}
	else if (strcmp(optName, COMM_OPTION_AUTO_RESTART_PARAMS_RESET_TIMER_FREQUENCY) == 0)
	{
		autoRestartParamsResetTimerFreq = atoi(optValue);
	}

#ifdef COMMLIB_CALCULATE_STATISTICS
	else if( strcmp( optName, COMM_OPTION_PERFORMANCE_TIMER ) == 0 )
	{
		int dt = atoi( optValue ); 
		if (dt <= 0)
			dt = COMM_OPTION_TIMEOUT_PERFORMANCE_COUNTING_DEFAULT;
		optTimeoutPerformanceCounting = dt;
	}
	else if( strcmp( optName, COMM_OPTION_TRAFFIC_PRINT_LAST ) == 0 )
	{
		printTrafficLast = atoi( optValue ) != 0;
	}
	else if( strcmp( optName, COMM_OPTION_DO_PERFORMANCE_COUNTING ) == 0 )
	{
		bool doCountingTmp = atoi( optValue ) != 0;
		if (doCountingTmp)
		{
			PLog("Turn on performance counting");
			if (!doCounting)
			{
				doCounting=true;
				memset((void *)tickCounter, 0, sizeof(tickCounter));
				memset((void *)tickCounterAfterLock, 0, sizeof(tickCounterAfterLock));
				memset((void *)tickCounterInOneMsg, 0, sizeof(tickCounterInOneMsg));
				memset(tickCounterInOneMsgSub, 0, sizeof(tickCounterInOneMsgSub));
				memset((void *)enterCounter, 0, sizeof(enterCounter));
				memset(tickCounterTimer, 0, sizeof(tickCounterTimer));
				memset(tickCounterAfterLockTimer, 0, sizeof(tickCounterAfterLockTimer));
				memset(enterCounterTimer, 0, sizeof(enterCounterTimer));
				_totalTicksAfterLock=0;
				_totalMessages=0;
				_maxThreadTime=0;
				_statisticTimeouts = 0;
				_statisticDelays=0;
				_statisticDisconnects=0;
				_statisticConnectionDenied=0;
				_statisticSubscriptionDenied=0;

				PLog("Counters initialization");
				try
				{
					for( _ManagerList::const_iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
					{
						PLog("Manager %s :", (*iter)->whoAreYou());
						UINT32 _max = (*iter)->printCounters();
						if (_max > _maxThreadTime)
							_maxThreadTime = _max;
					}
					CommMsgBody timerMsg;
					timerMsg.composeUINT32(SYSTEMTICKS());
					_CommMsg msg( _TIMER_PERFORMANCE_COUNTING, 0, timerMsg );
					timerThread->push( msg, optTimeoutPerformanceCounting );
				}
				catch (...)
				{
					PLog("Catch error in counter initialization - stop counting");
					doCounting = false;
				}
			}
			else
			{
				PLog("Performance counting already on!");
			}
		}
		else
		{
			PLog("Turn off performance counting.");
			doCounting=false;
		}
	}
#endif
	else if( !_setDynamicOption( optName, optValue ) )
	{
		PLog( "CommRoutingTable: Unknown option '%s' - ignored", optName );
	}
}

void CommRoutingTable::_Config::addAlias( const char* alias, const char * inManager_, const char * object_ )
{
	CommRule rule;
	rule.mask = alias;
	if (inManager_ && *inManager_ )
		rule.inManager = inManager_;
	if (object_ && *object_)
		rule.conns.push_back(object_);
	rule.parseMask();
	aliases.push_back( rule );
}

void CommRoutingTable::_Config::addNotAlias( const char* alias, const char * inManager_ )
{
	CommRule rule;
	rule.mask = alias;
	if (inManager_ && *inManager_ )
		rule.inManager = inManager_;
	rule.parseMask();
	notAliases.push_back( rule );
}

void CommRoutingTable::_readConfig( _Config& cfg, const PIniFile& iniFile )
{
	const PIniFile::Section* section = 0;
#ifndef COMM_CLIENT_ONLY
	section = iniFile.getSection( "subscrencryption" );
	if( section )
	{
		int n = section->items.size();
		cfg.versionEncryption.clear();
		PTRACE3("Encryption for subscription:");
		for( int i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			cfg.addVersionEncryption(item.name,item.value);
		}
	}
	section = iniFile.getSection( "restrictconnections" );
	if( section )
	{
		int n = section->items.size();
		cfg.restrictionRulesConnection.clear();
		cfg.restrictionDefaultConnection.clear();
		PTRACE3("Restrictions for connections:");
		cfg.restrictionConnectionDebug = section->getIntProperty("debug",0);
		cfg.restrictionConnectionTest = section->getIntProperty("test",0);
		cfg.restrictionForwardEnabled = section->getIntProperty("forwardenabled",1);
		PTRACE3("Restrictions connection debug=%d, test=%d, forward=%d", cfg.restrictionConnectionDebug, cfg.restrictionConnectionTest, cfg.restrictionForwardEnabled);
		for( int i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if (stricmp(item.name.c_str(),"debug") && stricmp(item.name.c_str(),"test"))
			{
				cfg.addRestrictionConnection(item.name,item.value);
			}
		}
	}
	section = iniFile.getSection( "restrictsubscription" );
	if( section )
	{
		int n = section->items.size();
		cfg.restrictionRulesSubscr.clear();
		cfg.restrictionDefaultSubscr.clear();
		PTRACE3("Restrictions for subscriptions:");
		cfg.restrictionSubscriptionDebug = section->getIntProperty("debug",0);
		cfg.restrictionSubscriptionTest = section->getIntProperty("test",0);
		PTRACE3("Restrictions subscription debug=%d, test=%d", cfg.restrictionSubscriptionDebug, cfg.restrictionSubscriptionTest);
		for( int i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if (stricmp(item.name.c_str(),"debug")&& stricmp(item.name.c_str(),"test"))
			{
				cfg.addRestrictionSubscription(item.name,item.value);
			}
		}
	}
#endif
	section = iniFile.getSection( "comm" );
	if (section)
	{
		cfg.aliases.clear();
		cfg.notAliases.clear();
		cfg.rules.clear();
		int i, n = static_cast< int >( section->items.size() );
		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if( item.name.startsWith( "alias" ) )
			{
				PString manager;
				const char * p = strchr(item.name.c_str(),'_');
				if (p)
				{
					manager.assign(p+1);
				}
				p = strchr( item.value, ' ' );
				if (p && *p)
				{
					PString alias;
					alias.assign(item.value,p);
					while(*p == ' ')
						p++;
					if (manager.length())
					{
						PLog( "Alias '%s' for manager %s redirects to '%s'", alias.c_str(), manager.c_str(), p);
						cfg.addAlias(alias.c_str(),manager.c_str(),p);
					}
					else
					{
						PLog( "Alias '%s' redirects to '%s'", alias.c_str(),  p);
						cfg.addAlias(alias.c_str(),0,p);
					}
				}
				else
				{	
					if (manager.length())
					{
						PLog( "Alias '%s' for manager %s", item.value.c_str(),manager.c_str() );
						cfg.addAlias( item.value.c_str(),manager.c_str(),0 );
					}
					else
					{
						PLog( "Alias '%s'", item.value.c_str() );
						cfg.addAlias( item.value,0,0 );
					}
				}
			}
			else if( item.name.startsWith( "notalias" ) )
			{
				PString manager;
				const char * p = strchr(item.name.c_str(),'_');
				if (p)
				{
					manager.assign(p+1);
				}
				if (manager.length())
				{
					PLog( "Not alias '%s' for manager %s", item.value.c_str(),manager.c_str() );
					cfg.addNotAlias( item.value.c_str(),manager.c_str() );
				}
				else
				{
					PLog( "Not alias '%s'", item.value.c_str() );
					cfg.addNotAlias( item.value,0);
				}
			}
			else if( item.name.equals( "rule" ) )
			{
				const char* value = item.value;
				const char* sp = strchr( value, ' ' );
				PASSERT3( sp );
				CommRule rule;
				rule.mask.assign( value, sp );
				for(;;)
				{
					while(*sp == ' ')
							sp++;
					if (!*sp)
						break;
					const char* sp1 = strchr( sp, ' ' );
					PString ruleConn;
					if( sp1 )
					{
						ruleConn.assign( sp, sp1 );
						sp = sp1;
					}
					else
					{
						ruleConn = sp;
					}
					const char * locBr = strchr(ruleConn.c_str(), '{');
					if (locBr)
					{
						const char * locCloseBr = strchr(locBr, '}');
						const char * sl = strchr(locBr, '/');
						PASSERT3(locCloseBr && sl);
						PString locationName(locBr+1,sl);
						int priority = atoi(sl + 1);
						ruleConn.cut(locBr - ruleConn.c_str());
						const pair< PStringMap< _ConnectProperty >::iterator, bool > r = cfg.locationPropertyMap.insert(make_pair(locationName, _ConnectProperty(priority)));
						if (!r.second)
						{
							(*r.first).second.priority = priority;
						}
						rule.conns.push_back(CommRule::CommRuleConns(ruleConn, 0, locationName.c_str()));
					}
					else
					{
						rule.conns.push_back(ruleConn);
					}
					if (!sp1)
						break;
				}
				cfg.addRule( rule );
			}
		}
	}
}

void CommRoutingTable::dynamicInit(PIniFile& iniFile)
{
	PWLock cfgLock( configLock );
	PWLock lock( cs );

	PSystemExceptionError::clearNumberOfExceptions(); 
	PAssert::clearNumberOfAsserts();
	PSystemMemoryError::clearNumberOfExceptions();

	_readConfig( _writableConfig, iniFile );

	const PIniFile::Section* section = iniFile.getSection( "comm" );
	if( section )
	{
		setOption(COMM_OPTION_PERFORMANCE_TIMER,section->getProperty(COMM_OPTION_PERFORMANCE_TIMER,"60000"));
#ifdef COMMLIB_CALCULATE_STATISTICS
		setOption(COMM_OPTION_TRAFFIC_PRINT_LAST,section->getProperty(COMM_OPTION_TRAFFIC_PRINT_LAST,""));

		// PYR-65989 - Clear the traffic counters before reconfiguring.
		_clearTrafficCounters();
#endif
		int i, n = static_cast< int >( section->items.size() );
		for( i=0; i < n ; ++i )
		{
			// PYR-65989 - Reconfigure dynamic options.
			const PIniFile::Item& item = section->items[ i ];
			if( item.name.startsWith( "option." ) )
			{
				const char* name = item.name + 7 /* strlen("option.")*/;
				if( _setDynamicOption( name, item.value ) )
				{
					PLog( "Option '%s'='%s'", name, item.value.c_str() );
				}
			}
		}
		setOption(COMM_OPTION_DO_PERFORMANCE_COUNTING,section->getProperty(COMM_OPTION_DO_PERFORMANCE_COUNTING,""));

#ifdef COMMLIB_CALCULATE_STATISTICS
		if(!doTrafficCounting && (_maxConnTrafficCounter > 0 || _maxSubscrTrafficCounter > 0))
		{
			// If needed, start the traffic report timer.
			CommMsgBody timerMsg;
			_CommMsg msg( _TIMER_PERFORMANCE_TRAFFIC_COUNTING, 0, timerMsg );
			timerThread->push( msg, optTimeoutTrafficCounting );
			PLog( "Starting traffic counter timer" );
			doTrafficCounting = true;
		}
		else if(doTrafficCounting && _maxConnTrafficCounter == 0 && _maxSubscrTrafficCounter == 0)
		{
			// If needed, stop the traffic report timer.
			PLog( "Stopping traffic counter timer" );
			doTrafficCounting = false;
		}
#endif
	}

	for( _ManagerList::iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
		(*iter)->dynamicInit(iniFile);
}

void CommRoutingTable::_Config::addRule( const CommRule& rule )
{
	rules.insert( rules.end(), rule )->parseMask();
#ifndef COMM_CLIENT_ONLY
	PTRACE3("Rule is added: %s:", rule.mask.c_str());
#endif
	CommRule::_List::const_iterator it;
	for (it=rule.conns.begin(); it !=rule.conns.end(); ++it)
	{
#ifndef COMM_CLIENT_ONLY
		PTRACE3("   >%s", (*it).conns.c_str());
#endif
		if ((*it).location.length())
		{
			locationPropertyMap.insert( make_pair( (*it).location, _ConnectProperty( 0 ) ) );
			PTRACE4("addRule::Location %s added",(*it).location.c_str());
		}
	}
}

#ifndef COMM_CLIENT_ONLY
void CommRoutingTable::_RestrictionRuleHeader::print(char * buffer ) const
{
	sprintf(buffer,"'%s'='%s'",initialName.c_str(),initialValue.c_str());
}

bool CommRoutingTable::_checkWildCharPattern(const char * pattern)
{
	while(*pattern)
	{
		if (*pattern== '*')
		{
			pattern++;
			if (*pattern == '*' || *pattern == '?')
			{
				return false;
			}
		}
		else
			pattern++;
	}
	return true;
}

bool CommRoutingTable::_RestrictionRuleHeader::_checkForward(const char * dest, const char * frw)
{
	int wild = 0;
	while(*dest)
	{
		if (*dest == '*' || *dest == '?')
			wild++;
		dest++;
	}
	if (wild >= _MaxRestrictionParms)
		return false;
	while(*frw)
	{
		if (*frw == '$')
		{
			frw++;
			if ( *frw== '$')
			{
				frw++;
			}
			else if ( *frw=='F')
			{
				frw++;
			}
			else
			{
				int c = *frw - '0';
				if (c>=wild)
					return false;
				frw++;
			}
		}
		else
		{
			frw++;
		}
	}
	return true;
}

bool CommRoutingTable::_makeForward( PString& ret, const _RestrictionParms& parms, int index, const char* forward ) const
{
	ret = "";
	const char* p = forward;
	for(;;)
	{
		const char* dollar = strchr( p, '$' );
		if( !dollar )
		{
			ret.append( p );
			return true;
		}
		ret.append( p, dollar );
		char idx = *( dollar + 1 );
		if( idx == '$' )
		{
			ret.append( '$' );
			p = dollar + 2;
		}
		else if( idx >= '0' && idx < '0' + index )
		{
			int i = idx - '0';
            PASSERT3(i >= 0 && i < _MaxRestrictionParms);
			if (parms[i].begin)
				ret.append( parms[ i ].begin, parms[ i ].end );
			p = dollar + 2;
		}
		else if ( idx == 'F')
		{
			if (_internalEncryption(ret, parms, index)==false)
				return false;
			p = dollar + 2;
		}
		else
			p = dollar + 1;
	}
}

void CommRoutingTable::_RestrictionRuleHeader::_parseAddress(PString & address_, PString &object_, PString & type_, const char * in_)
{
	const char * fSlash = strchr(in_,'/');
	const char * sSlash = fSlash ? strchr(fSlash + 1,'/') : 0;
	if (!fSlash)
	{
		address_=in_;
		object_="*";
		type_="*";
	}
	else
	{
		address_.assign(in_,fSlash);
		if (sSlash)
		{
			object_.assign(fSlash + 1,sSlash);
			type_.assign(sSlash + 1);
		}
		else
		{
			object_.assign(fSlash + 1);
			type_ = "*";
		}
	}
}

void CommRoutingTable::_RestrictionRuleHeader::_parseRule(PString & address_, PString &object_, PString & type_, const char * in_)
{
	const char * fSlash = strchr(in_,'/');
	const char * sSlash = fSlash ? strchr(fSlash + 1,'/') : 0;
	if (!fSlash)
	{
		address_=in_;
		object_="";
		type_="";
	}
	else
	{
		address_.assign(in_,fSlash);
		if (sSlash)
		{
			object_.assign(fSlash + 1,sSlash);
			type_.assign(sSlash + 1);
		}
		else
		{
			object_.assign(fSlash + 1);
			type_ = "";
		}
	}
}

bool CommRoutingTable::_RestrictionRuleHeader::init( const char * to, const char * from)
{
	initialName = to;
	initialValue = from;
	type = 0;
	_parseAddress(destAddress,destObject,destType,to);
	if (_checkWildCharPattern(destAddress.c_str())==false)
	{
		PTRACE3("Incorrect pattern '%s', ignored",destAddress.c_str());
		return false;
	}
	if (_checkWildCharPattern(destObject.c_str())==false)
	{
		PTRACE3("Incorrect pattern '%s', ignored",destObject.c_str());
		return false;
	}
	if (_checkWildCharPattern(destType.c_str())==false)
	{
		PTRACE3("Incorrect pattern '%s', ignored",destType.c_str());
		return false;
	}
	return true;
}

bool CommRoutingTable::_compareWithParams(const char * pattern, const char * value, _RestrictionParms & parms, int & index)
{
	PASSERT5( index >= 0 );
	if (!*value)
	{
		while(*pattern)
		{
			if (*pattern == '*' || *pattern == '?')
			{
				if (index >= _MaxRestrictionParms)
					return false;
				parms[index].begin = parms[index].end = 0;
				index++;
				pattern++;
			}
			else
				break;
		}
	}
	while(*value && *pattern )
	{
		if (*pattern == '*')
		{
			pattern++;
			if (index >= _MaxRestrictionParms)
				return false;
			parms[index].begin=value;
			if (!*pattern)
			{
				parms[index].end = value+strlen(value);
				index++;
				return true;
			}
			while (*value && *value != *pattern)
				value++;
			if (!*value)
				return false;
			parms[index].end = value;
			index++;
			return _compareWithParams(pattern,value,parms,index);
		}
		else if (*pattern == '?')
		{
			pattern++;
			if (index >= _MaxRestrictionParms)
				return false;
			parms[index].begin = value;
			value++;
			parms[index].end = value;
			index++;
		}
		else
		{
			if (*pattern++ != *value++)
				return false;
		}
	}
	if (!*value)
	{
		while(*pattern)
		{
			if (*pattern == '*' || *pattern == '?')
			{
				if (index == _MaxRestrictionParms)
					return false;
				parms[index].begin = parms[index].end = 0;
				index++;
				pattern++;
			}
			else
				break;
		}
	}
	return (!*value && !*pattern);
}

bool CommRoutingTable::_RestrictionRuleHeader::isEqualDest(const char * _destServer, const char * _destObject, const char * _destType,
														   _RestrictionParms &serverParm, int &serverIndex,
														   _RestrictionParms & objectParm,int &objectIndex,
														   _RestrictionParms & typeParm, int &typeIndex) const
{
	if (_compareWithParams(destAddress,_destServer,serverParm,serverIndex))
	{
		if (_compareWithParams(destObject,_destObject,objectParm,objectIndex))
		{
			if (_compareWithParams(destType,_destType,typeParm,typeIndex))
			{
				return true;
			}
		}
	}
	return false;
}

void CommRoutingTable::_RestrictionRuleHeader::debug() const
{
	static const char * const names[]=
	{
		"access allowed",
		"access denied", 
		"route allowed",
		"route denied",
		"forward"
	};
	int n = type&0xff;
	PASSERT3(n > 0 && n<=5);
	if (n<5)
		PTRACE3("To '%s'/'%s'/'%s' %s for %s%s%s",destAddress.c_str(),destObject.c_str(),
		destType.c_str(),names[n-1], ((type&eUseLast)?"last ":""),
		((type&eUseFirst)?"first ":""), 
		((type&eUseAny)?"any ":""));
	else
		PTRACE3("To '%s'/'%s'/'%s' %s for %s%s%s to '%s'/'%s'/'%s'",destAddress.c_str(),destObject.c_str(),
		destType.c_str(),names[n-1], ((type&eUseLast)?"last ":""),
		((type&eUseFirst)?"first ":""), 
		((type&eUseAny)?"any ":""),forwardAddress.c_str(),forwardObject.c_str(),forwardType.c_str());

}

void CommRoutingTable::_RestrictionRule::debug() const
{
	_RestrictionRuleHeader::debug();
	PTRACE3("source address %s",sourceIpAddress.c_str());
}

void CommRoutingTable::_RestrictionDefault::debug() const
{
	_RestrictionRuleHeader::debug();
	PTRACE3("address");
}

bool CommRoutingTable::_RestrictionRule::isEqualIp(const char * ip) const
{
	return PString::compareWithWildChar(sourceIpAddress,ip);
}

int CommRoutingTable::_RestrictionRule::isEqualSource(const vector <const char * > &ips) const
{
	if( ! ips.size() )
	{
		return -1;
	}
	if (type & eUseLast)
	{
		if (isEqualIp(ips[ips.size()-1]))
			return ips.size()-1;
		else
			return -1;
	}
	if (type & eUseFirst)
	{
		if (isEqualIp(ips[0]))
			return 0;
		else
			return -1;
	}
	if (type & eUseAny)
	{
		for (int i=0;i<ips.size();i++)
		{
			if (isEqualIp(ips[i]))
				return i;
		}
		return -1;
	}
	return -1;
}

bool CommRoutingTable::_RestrictionRuleHeader::rule( const char * &r)
{
	while(*r && *r==' ')
		r++;
	if (!strnicmp(r,"allow",5)) 
	{
		type |= eRestrictionAllow;
		r+=5;
	}
	else if (!strnicmp(r,"deny",4)) 
	{
		type |= eRestrictionDeny;
		r+=4;
	}
	else if (!strnicmp(r,"routeallow",10)) 
	{
		type |= eRestrictionAllowRoute;
		r+=10;
	}
	else if (!strnicmp(r,"routedeny",9)) 
	{
		type |= eRestrictionDenyRoute;
		r+=9;
	}
	else if (!strnicmp(r,"forward",7)) 
	{
		type |= eRestrictionForward;
		r+=7;
		const char * p = r;
		while(*p && *p==' ')
			p++;
		if (!*p)
		{
			PTRACE3("Incorrect forward rule '%s'",r);
			return false;
		}
		const char * p1 = strchr(p,' ');
		PString frw;
		if (p1)
			frw.assign(p,p1);
		else
			frw.assign(p);
		_parseRule(forwardAddress,forwardObject,forwardType,frw.c_str());
		if (_checkForward(destAddress,forwardAddress) == false || 
			_checkForward(destObject,forwardObject) == false ||
			_checkForward(destType,forwardType) == false)
		{
			PTRACE3("Incorrect forward rule '%s'",r);
			return false;
		}
	}
	else
	{
		PTRACE3("Unknown rule '%s', ignored", r);
		return false;
	}
	return true;
}

bool CommRoutingTable::_RestrictionRule::rule( const char * r)
{
	if (!_RestrictionRuleHeader::rule(r))
		return false;
	while(*r && *r==' ')
		r++;
	if (!strnicmp(r,"last",4)) 
	{
		type |= eUseLast;
	}
	else if (!strnicmp(r,"first",5)) 
	{
		type |= eUseFirst;
	}
	else if (!strnicmp(r,"any",3)) 
	{
		type |= eUseAny;
	}
	if ( !(type & (eUseLast|eUseFirst|eUseAny)))
		type |= eUseAny; // default 
	return true;
}

bool CommRoutingTable::_RestrictionDefault::rule( const char * r )
{
	if (!_RestrictionRuleHeader::rule(r))
		return false;
	while(*r && *r==' ')
		r++;
	if (*r)
	{
		PTRACE3("Unknown parameter '%s' for default rule, ignored",r);
	}
	type &= ~(eUseLast|eUseFirst|eUseAny);
	type |= eUseAny; 
	return true;
}

bool CommRoutingTable::_RestrictionRule::addAddress(const char * address)
{
	PString _ip(PString::ltrim(address),PString::rtrim(address));
	sourceIpAddress = _ip.c_str();
	return true;
}

void CommRoutingTable::_addRestriction(vector <_RestrictionRule> & rRule, vector <_RestrictionDefault> & dRule,  const char * to, const char * from)
{
	PString _to(PString::ltrim(to),PString::rtrim(to));
	PString _from(PString::ltrim(from),PString::rtrim(from));

	const char * sc = strchr(_from.c_str(),';');
	if (!sc)
	{
		_RestrictionDefault r;
		if (r.init(_to.c_str(),_from.c_str())==false)
			return;
		char buff[1024];
		r.print(buff);
		PTRACE3("New default rule added:  %s", buff);

		if (r.rule(from))
		{
			r.debug();
			dRule.push_back(r);
		}
		return;
	}
	else
	{
		_RestrictionRule r;
		if (r.init(_to.c_str(),_from.c_str())==false)
			return;
		char buff[1024];
		r.print(buff);
		PTRACE3("New rule added:  %s", buff);

		PString ip(_from.c_str(),sc);
		if (!r.addAddress(ip.c_str()))
		{
			return;
		}
		if (r.rule(sc+1))
		{
			r.debug();
			rRule.push_back(r);
		}
		return;
	}
}

bool CommRoutingTable::_checkRestrictions(const vector <_RestrictionRule> & rRule, const vector <_RestrictionDefault> & dRule, PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, const char * inManager, int printDebug, PString* failReason) const
{
	bool isLocal = _isAlias( destServer, destObject, destType, inManager, 0, false );
	int i;
	if (printDebug>2)
	{
		PTRACE3("Check restrictions for %s '%s'/'%s'/%s'",(isLocal?"local":"remote"), destServer.c_str(),destObject.c_str(),destType.c_str());
		for (i=0;i<ips.size();i++)
		{
			PTRACE3("Source address #%d: '%s'",i, ips[i]);
		}
	}

	for (i=0;i<rRule.size();i++)
	{
		_RestrictionParms serverParm;
		int serverIndex = 0;
		_RestrictionParms objectParm;
		int objectIndex = 0;
		_RestrictionParms typeParm;
		int typeIndex = 0;

		if (rRule[i].isEqualDest(destServer,destObject,destType,serverParm,serverIndex,objectParm,objectIndex,typeParm,typeIndex))
		{
			if (printDebug>2)
			{
				char buff[1024];
				rRule[i].print(buff);
				PTRACE3("Rule #%d (%s) matched, check for source address", i,buff);
			}

			int nIp = rRule[i].isEqualSource(ips);
			if (nIp>=0)
			{
				if (printDebug>2)
				{
					PTRACE3("Source address #%d: '%s'",nIp, ips[nIp]);
				}

				if (isLocal)
				{
					if ((rRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionAllow)
					{
						if (printDebug>1)
						{
							char buff[1024];
							rRule[i].print(buff);
							PTRACE3("Allowed from '%s' (%d-%d) to local '%s'/'%s'/'%s' according to %s",ips[nIp],nIp,ips.size(), destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
						}
						return true;
					}
					else if ((rRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionDeny)
					{
						if (printDebug>0 || failReason)
						{
							char buff[1024];
							rRule[i].print(buff);
							if (printDebug > 0)
							{
								PTRACE3("Denied from '%s' (%d-%d) to local '%s'/'%s'/'%s' according to %s", ips[nIp], nIp, ips.size(), destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
							}
							if (failReason)
							{
								failReason->assign("local r").appendInt(i).append(':').append(buff);
							}
						}
						return false;
					}
					else if ((rRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionForward)
					{
						PString _destServer;
						PString _destObject;
						PString _destType;
						if( _makeForward(_destServer,serverParm,serverIndex,rRule[i].forwardAddress) == false ||
							_makeForward(_destObject,objectParm,objectIndex,rRule[i].forwardObject) == false ||
							_makeForward(_destType,typeParm,typeIndex,rRule[i].forwardType) == false)
						{
							if (printDebug>0 || failReason)
							{
								char buff[1024];
								rRule[i].print(buff);
								if (printDebug > 0)
								{
									PTRACE3("From '%s' (%d-%d) to local '%s'/'%s'/'%s' according to %s forward failed", ips[nIp], nIp, ips.size(), destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
								}
								if (failReason)
								{
									failReason->assign("local r").appendInt(i).append(':').append(buff);
								}
							}
							return false;
						}
						if (printDebug>1)
						{
							char buff[1024];
							rRule[i].print(buff);
							PTRACE3("From '%s' (%d-%d) to local '%s'/'%s'/'%s' according to %s forwarded to '%s'/'%s'/'%s'",ips[nIp],nIp,ips.size(), destServer.c_str(),destObject.c_str(),destType.c_str(), buff,_destServer.c_str(),_destObject.c_str(),_destType.c_str());
						}
						if (_config.restrictionForwardEnabled ) 
						{
							destServer.moveFrom(_destServer);
							destObject.moveFrom(_destObject);
							destType.moveFrom(_destType);
						}
						return true;
					}
					else
					{
						if (printDebug>1)
						{
							char buff[1024];
							rRule[i].print(buff);
							PTRACE3("No rule for '%s' (%d-%d) to local '%s'/'%s'/'%s' according to %s, continue",ips[nIp],nIp,ips.size(),destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
						}
					}
				}
				else 
				{
					if ((rRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionAllowRoute)
					{
						if (printDebug>1)
						{
							char buff[1024];
							rRule[i].print(buff);
							PTRACE3("Allowed routing from '%s' (%d-%d) to remote '%s'/'%s'/'%s' according to %s",ips[nIp],nIp,ips.size(),destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
						}
						return true;
					}
					else if ((rRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionDenyRoute)
					{
						if (printDebug>0 || failReason)
						{
							char buff[1024];
							rRule[i].print(buff);
							if (printDebug > 0)
							{
								PTRACE3("Denied routing from '%s' (%d-%d) to remote '%s'/'%s'/'%s' according to %s", ips[nIp], nIp, ips.size(), destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
							}
							if (failReason)
							{
								failReason->assign("remote r").appendInt(i).append(':').append(buff);
							}
						}
						return false;
					}
					else if ((rRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionForward)
					{
						PString _destServer;
						PString _destObject;
						PString _destType;
						if( _makeForward(_destServer,serverParm,serverIndex,rRule[i].forwardAddress) == false ||
							_makeForward(_destObject,objectParm,objectIndex,rRule[i].forwardObject) == false ||
							_makeForward(_destType,typeParm,typeIndex,rRule[i].forwardType) == false)
						{
							if (printDebug>0 || failReason)
							{
								char buff[1024];
								rRule[i].print(buff);
								if (printDebug > 0)
								{
									PTRACE3("From '%s' (%d-%d) to remote '%s'/'%s'/'%s' according to %s forward failed", ips[nIp], nIp, ips.size(), destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
								}
								if (failReason)
								{
									failReason->assign("remote r").appendInt(i).append(':').append(buff);
								}
							}
							return false;
						}
						if (printDebug>1)
						{
							char buff[1024];
							rRule[i].print(buff);
							PTRACE3("From '%s' (%d-%d) to remote '%s'/'%s'/'%s' according to %s forwarded to '%s'/'%s'/'%s'",ips[nIp],nIp,ips.size(), destServer.c_str(),destObject.c_str(),destType.c_str(), buff,_destServer.c_str(),_destObject.c_str(),_destType.c_str());
						}
						if (_config.restrictionForwardEnabled) 
						{
							destServer.moveFrom(_destServer);
							destObject.moveFrom(_destObject);
							destType.moveFrom(_destType);
						}
						return true;
					}
					else
					{
						if (printDebug>1)
						{
							char buff[1024];
							rRule[i].print(buff);
							PTRACE3("No rule for remote '%s' (%d-%d) to remote '%s'/'%s'/'%s' according to %s, continue",ips[nIp],nIp,ips.size(),destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
						}
					}
				}
			}
			else
			{
				if (printDebug>2)
				{
					PTRACE3("Source address doesn't match, continue");
				}
			}	
		}
	}
	for (i=0;i<dRule.size();i++)
	{
		_RestrictionParms serverParm;
		int serverIndex = 0;
		_RestrictionParms objectParm;
		int objectIndex = 0;
		_RestrictionParms typeParm;
		int typeIndex = 0;

		if (dRule[i].isEqualDest(destServer,destObject,destType,serverParm,serverIndex,objectParm,objectIndex,typeParm,typeIndex))
		{
			if (printDebug>2)
			{
				char buff[1024];
				dRule[i].print(buff);
				PTRACE3("Default rule #%d (%s) matched", i,buff);
			}

			if (isLocal)
			{
				if ((dRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionAllow)
				{
					if (printDebug>1)
					{
						char buff[1024];
						dRule[i].print(buff);
						PTRACE3("Allowed to local '%s'/'%s'/'%s' according to default %s",destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
					}
					return true;
				}
				else if ((dRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionDeny)
				{	
					if (printDebug>0 || failReason)
					{
						char buff[1024];
						dRule[i].print(buff);
						if (printDebug > 0)
						{
							PTRACE3("Denied to local '%s'/'%s'/'%s' according to default %s", destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
						}
						if (failReason)
						{
							failReason->assign("local d").appendInt(i).append(':').append(buff);
						}
					}
					return false;
				}
				else if ((dRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionForward)
				{
					PString _destServer;
					PString _destObject;
					PString _destType;
					if( _makeForward(_destServer,serverParm,serverIndex,dRule[i].forwardAddress) == false ||
						_makeForward(_destObject,objectParm,objectIndex,dRule[i].forwardObject) == false ||
						_makeForward(_destType,typeParm,typeIndex,dRule[i].forwardType) == false)
					{
						if (printDebug>0 || failReason)
						{
							char buff[1024];
							dRule[i].print(buff);
							if (printDebug > 0)
							{
								PTRACE3("To local '%s'/'%s'/'%s' according to %s forward failed", destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
							}
							if (failReason)
							{
								failReason->assign("local d").appendInt(i).append(':').append(buff);
							}
						}
						return false;
					}
					if (printDebug>1)
					{
						char buff[1024];
						dRule[i].print(buff);
						PTRACE3("To local '%s'/'%s'/'%s' according to %s forwarded to '%s'/'%s'/'%s'", destServer.c_str(),destObject.c_str(),destType.c_str(), buff,_destServer.c_str(),_destObject.c_str(),_destType.c_str());
					}
					if (_config.restrictionForwardEnabled) 
					{
						destServer.moveFrom(_destServer);
						destObject.moveFrom(_destObject);
						destType.moveFrom(_destType);
					}
					return true;
				}
				else
				{
					if (printDebug>1)
					{
						char buff[1024];
						dRule[i].print(buff);
						PTRACE3("No default rule to local '%s'/'%s'/'%s' according to %s, continue",destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
					}
				}
			}
			else 
			{
				if ((dRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionAllowRoute)
				{
					if (printDebug>1)
					{
						char buff[1024];
						dRule[i].print(buff);
						PTRACE3("Allowed route to remote '%s'/'%s'/'%s' according to default %s",destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
					}
					return true;
				}
				else if ((dRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionDenyRoute)
				{
					if (printDebug>0 || failReason)
					{
						char buff[1024];
						dRule[i].print(buff);
						if (printDebug > 0)
						{
							PTRACE3("Denied route to remote '%s'/'%s'/'%s' according to default %s", destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
						}
						if (failReason)
						{
							failReason->assign("remote d").appendInt(i).append(':').append(buff);
						}
					}
					return false;
				}
				else if ((dRule[i].type & 0xff) == _RestrictionRuleHeader::eRestrictionForward)
				{
					PString _destServer;
					PString _destObject;
					PString _destType;
					if( _makeForward(_destServer,serverParm,serverIndex,dRule[i].forwardAddress) == false ||
						_makeForward(_destObject,objectParm,objectIndex,dRule[i].forwardObject) == false ||
						_makeForward(_destType,typeParm,typeIndex,dRule[i].forwardType) == false)
					{
						if (printDebug>0 || failReason)
						{
							char buff[1024];
							dRule[i].print(buff);
							if (printDebug > 0)
							{
								PTRACE3("To remote '%s'/'%s'/'%s' according to %s forward failed", destServer.c_str(), destObject.c_str(), destType.c_str(), buff);
							}
							if (failReason)
							{
								failReason->assign("remote d").appendInt(i).append(':').append(buff);
							}
						}
						return false;
					}
					if (printDebug>1)
					{
						char buff[1024];
						dRule[i].print(buff);
						PTRACE3("To remote '%s'/'%s'/'%s' according to %s forwarded to '%s'/'%s'/'%s'", destServer.c_str(),destObject.c_str(),destType.c_str(), buff,_destServer.c_str(),_destObject.c_str(),_destType.c_str());
					}
					if (_config.restrictionForwardEnabled) 
					{
						destServer.moveFrom(_destServer);
						destObject.moveFrom(_destObject);
						destType.moveFrom(_destType);
					}
					return true;
				}
				else
				{
					if (printDebug>1)
					{
						char buff[1024];
						dRule[i].print(buff);
						PTRACE3("No default rule for route to remote '%s'/'%s'/'%s' according to %s, continue",destServer.c_str(),destObject.c_str(),destType.c_str(),buff);
					}
				}
			}
		}
	}
	if (printDebug>1)
	{
		PTRACE3("No restrictions found, allowed by default");
	}

	return true;
}

bool CommRoutingTable::checkRestrictionsConnection(PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, const char * inManager, PString* failReason)
{
	try
	{
		bool ret = _checkRestrictions(_config.restrictionRulesConnection, _config.restrictionDefaultConnection, destServer, destObject, destType, ips,inManager, _config.restrictionConnectionDebug, failReason);
		if (!ret)
			_statisticConnectionDenied++;
		if (_config.restrictionConnectionTest)
			return true;
		else
			return ret;
	}
	catch(PError &er)
	{
		PTRACE3("_checkRestrictions: error '%s'", er.why());
		return true;
	}
}

bool CommRoutingTable::checkRestrictionsSubscription(PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, PString* failReason)
{
	try
	{
		bool ret = _checkRestrictions(_config.restrictionRulesSubscr, _config.restrictionDefaultSubscr, destServer, destObject, destType, ips, 0, _config.restrictionSubscriptionDebug, failReason);
		if (!ret)
			_statisticSubscriptionDenied++;
		if (_config.restrictionSubscriptionTest)
			return true;
		else
			return ret;
	}
	catch(PError &er)
	{
		PTRACE3("_checkRestrictions: error '%s'", er.why());
		return true;
	}
}

void CommRoutingTable::_Config::addRestrictionConnection(const char * to, const char * from) 
{
	try
	{
		_addRestriction(restrictionRulesConnection, restrictionDefaultConnection, to, from);
	}
	catch(PError &er)
	{
		PTRACE3("_addRestriction: error '%s'", er.why());
		return; 
	}
}

void CommRoutingTable::_Config::addRestrictionSubscription(const char * to, const char * from) 
{
	try
	{
		_addRestriction(restrictionRulesSubscr, restrictionDefaultSubscr, to,from);
	}
	catch(PError &er)
	{
		PTRACE3("_addRestriction: error '%s'", er.why());
		return;
	}
}

void CommRoutingTable::_Config::addVersionEncryption(const char * ver, const char * key) 
{
	versionEncryption.push_back(VersionEncryption(ver,key));
}

#else
bool CommRoutingTable::checkRestrictionsSubscription(PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, PString* failReason)
{
	return true;
}

bool CommRoutingTable::checkRestrictionsConnection(PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, const char * inManager, PString* failReason)
{
	return true;
}
#endif //COMM_CLIENT_ONLY

void CommRoutingTable::_registerThreadManager( CommThreadManager& mgr )
{
	PWLock lock( cs );
	managers.push_back( &mgr );
}

void CommRoutingTable::start()
{
	PWLock lock( cs );
	timerThread->start();
	for( _ManagerList::iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
		(*iter)->start();
}

void CommRoutingTable::resetAllStats()
{
	PWLock lock( cs );
	for( _PhysConnStringMap::iterator it = outboundConns.begin(); it != outboundConns.end() ; ++it )
	{
		_PhysConn* physConn = (*it).second;
		physConn->connFailures = 0;
		physConn->lastErrorCode = 0;
		physConn->firstFailureTime = 0;
		physConn->failureTimeStatus = 0;
	}
}

void CommRoutingTable::shutdown()
{
	PWLock lock( cs );
	shuttingDown = true;
	timerThread->shutdown();
	for( _ManagerList::iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
	{
		(*iter)->shutdown();
	}
}

bool CommRoutingTable::terminated()
{
	PWLock lock( cs );
	if( !timerThread->terminated() )
		return false;
	for( _ManagerList::iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
	{
		if( !(*iter)->terminated() )
		{
			return false;
		}
	}
	return true;
}

bool CommRoutingTable::waitUntilTerminated( int sec )
{
	{
		PWLock lock( cs );
		if( !shuttingDown )
		{
			PLog( "CommRoutingTable: waitUntilTerminated called without shutdown - shutdown forced" );
			shutdown();
		}
	}
	return _CommWaitUntilTerminated( *this, sec );
}

CommRoutingTable::~CommRoutingTable()
{
	delete timerThread;

	{
		int i;
		for( _Conn** c = cliConnMap.firstIndex( i ); c ; c = cliConnMap.nextIndex( i ) )
		{
			_Conn* conn = *c;
			delete conn;
		}
	}

	{
		for( _SubscrSetByName::iterator it = nameSubscrMap.begin(); it != nameSubscrMap.end() ; ++it )
		{
			_Subscr* subscr = *it;

			for( _SubscrLeafSetByCli::iterator jt = subscr->cli.begin() ; jt != subscr->cli.end() ; ++jt )
			{
				_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*jt);
				delete leaf;
			}
			_free( subscr->serverChannel );
			_free( subscr->serverObject );
			_free( subscr->server );
			delete subscr;
		}
	}

	{
		for( _SubscrLightSetByName::iterator it = nameSubscrLightMap.begin(); it != nameSubscrLightMap.end() ; ++it )
		{
			_SubscrLight* subscr = *it;

			for( _SubscrLeafSetByCli::iterator jt = subscr->cli.begin() ; jt != subscr->cli.end() ; ++jt )
			{
				_SubscrLightLeaf* leaf = *jt;
				delete leaf;
			}
			_free( subscr->serverChannel );
			_free( subscr->serverObject );
			_free( subscr->server );
			delete subscr;
		}
	}


	{
		for( _PhysConnStringMap::iterator it = outboundConns.begin(); it != outboundConns.end() ; ++it )
		{
			_PhysConn* physConn = (*it).second;
			delete physConn;
		}
	}
	{
		for( _PhysConnMuxStringMap::iterator it = localConns.begin(); it != localConns.end() ; ++it )
		{
			_PhysConn* physConn = (*it).second.physConn;

			//multiple references allowed. Delete only if the reference is the last one
			bool found = false;
			_PhysConnMuxStringMap::iterator jt = it;
			for( ++jt; jt != localConns.end() ; ++jt )
				if( physConn == (*jt).second.physConn )
				{
					found = true;
					break;//for( jt )
				}

			if( !found )
				delete physConn;
		}
	}
	{
		for( _PhysConnSet::iterator it = inboundConns.begin(); it != inboundConns.end() ; ++it )
		{
			_PhysConn* physConn = *it;
			delete physConn;
		}
	}
}

_CommPhysicalConnection* CommRoutingTable::createIncomingPhysicalConnection( const char* addr, CommThreadManager* mgr )//in disconnected state
{
	PWLock lock( cs );
	_PhysConn* ret = new _PhysConn( addr, mgr, true );
	ret->state = _PhysConn::_StatePhysConnIncomingDisc;
	ret->quality = -1;
	ret->qualityRequired = false;
	ret->connFailures = 0;
	ret->lastErrorCode = 0;
	ret->firstFailureTime = 0;
	ret->failureTimeStatus = 0;
	inboundConns.insert( ret );
	return ret;
}

void CommRoutingTable::setPhysicalConnectionQuality( _CommPhysicalConnection* physConn_, int quality )
{
	if( quality < 0 )
		quality = 0;
	//	else if( quality > 100 )
	//		quality = 100;

	_PhysConn* physConn = (_PhysConn*)physConn_;//safe because _CommPhysicalConnection is always created by CommRoutingTable as _PhysConn

	PWLock lock( cs );
	if (physConn->qualityRequired == false)
	{
		PTRACE3("CommRoutingTable::setPhysicalConnectionQuality: qualityRequired is not set");
		physConn->qualityRequired = true;
	}
	if (quality<=100)
	{
		if( quality == physConn->quality )
			return;
		physConn->quality = quality;
	}

	UINT32 t0 = SYSTEMTICKS();
	for( _ConnSetByCli::iterator iter = physConn->cliConns.begin() ; iter != physConn->cliConns.end() ; ++iter )
	{
		_Conn* conn = *iter;
		PASSERT3( conn->srv == physConn );
		if( conn->wantQualityUpdates )
			_postCliQualityUpdate( conn, quality );
	}
	for( _ConnSetBySrv::iterator iter = physConn->srvConns.begin() ; iter != physConn->srvConns.end() ; ++iter )
	{
		_Conn* conn = *iter;
		PASSERT3( conn->cli == physConn );
		if( conn->wantQualityUpdates )
			_postSrvQualityUpdate( conn, quality );
	}

	UINT32 t1 = SYSTEMTICKS();
	if( t1 - t0 > TICKS_PER_SECOND )
		PLog( "CommRoutingTable::setPhysicalConnectionQuality: long processing (%d ticks)", t1 - t0 );
}

void CommRoutingTable::_postCliQualityUpdate( _Conn* conn, int quality )
{
	if( conn->srv && conn->cli && quality >= 0)//formerly PASSERT3
	{
		_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE, conn->priority );
		m.body.composeUINT32( conn->cliId ).composeUINT32( conn->cliThisId ).composeBYTE( static_cast< BYTE >( quality ) );
		_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, m );
	}
}

void CommRoutingTable::_postSrvQualityUpdate( _Conn* conn, int quality )
{
	if( conn->srv && conn->cli && quality >= 0 )//formerly PASSERT3
	{
		_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE, conn->priority );
		m.body.composeUINT32( conn->srvId ).composeUINT32( conn->srvThisId ).composeBYTE( static_cast< BYTE >( quality ) );
		_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, m );
	}
}


void CommRoutingTable::serviceCallTransport(_PhysConn* physConn_,UINT32 id,CommMsgBody * msg_body)
{
	_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
	m.body.composeUINT32( id );
	if(msg_body)
	{
		m.body.composeMsgBody(*msg_body);
	}
	_postToPhysConn( physConn_, 0, 0, m );
}


void CommRoutingTable::serviceCall( _CommPhysicalConnection* physConn_,UINT32 id,CommMsgBody * msg_body)
{
	PWLock lock( cs );

	switch(id)
	{
	case _SERVICE_REQUEST_KEEP_ALIVE: 
		// Connection is alive
		physicalConnIsAlive(physConn_);
		break;
	case _SERVICE_REQUEST_QUALITY:
		{
			PASSERT3(msg_body);
			CommMsgParser parser( *msg_body );
			int quality;
			parser.parseINT32(quality);
			setPhysicalConnectionQuality(physConn_, quality );
			if (doCounting && quality == _TRANSPORT_KEEPALIVE_DELAYED)
			{
				PInterlockedIncrement(&_statisticDelays);
			}
		}
		break;
	default:
		PTRACE3("Service request %d", id);
		PASSERT3(0);
	}
}

void CommRoutingTable::physicalConnIsAlive( _CommPhysicalConnection* physConn_ )
{
	_PhysConn* physConn = (_PhysConn*)physConn_;//safe because _CommPhysicalConnection is always created by CommRoutingTable as _PhysConn

	PWLock lock( cs );
	UINT32 t0 = SYSTEMTICKS();
	int n = static_cast< int >( physConn->srvConns.size() );
	if( n )
	{
		for( _ConnSetBySrv::iterator iter = physConn->srvConns.begin() ; iter != physConn->srvConns.end() ; ++iter )
		{
			_Conn* conn = *iter;
			PASSERT3( conn->cli == physConn );
			if( conn->wantIsAlive )
			{
				_postSrvPhysConnIsAlive( conn );
				PTRACE3( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE sent for conn->srvThisId=%08X", conn->srvThisId );
				conn->wantIsAlive = false;
			}
		}
	}

	UINT32 t1 = SYSTEMTICKS();
	if( t1 - t0 > TICKS_PER_SECOND )
		PLog( "CommRoutingTable::physicalConnIsAlive: long processing (%d ticks)", t1 - t0 );
}

void CommRoutingTable::_postSrvPhysConnIsAlive( _Conn* conn )
{
	if( conn->srv && conn->cli )//formerly PASSERT3
	{
		_CommMsg m( _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE, conn->priority );
		m.body.composeUINT32( conn->srvId ).composeUINT32( conn->srvThisId );
		_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, m );
	}
}

void CommRoutingTable::_postToPhysConn( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m )
{
	if( physConn == 0 )
	{
		PLog( "CommRoutingTable: message to null PhysConn - ignored" );
		return;
	}
	if( physConn->state != _PhysConn::_StatePhysConnOk )
	{
		PLog( "CommRoutingTable: message to PhysConn with state=%d != _StatePhysConnOk - ignored", physConn->state );
		return;
	}
	m.internal.mux = static_cast< UINT16 >( mux );
	m.internal.muxChannelId = muxChannelId;
	physConn->mgr->push( physConn, m );
}

void CommRoutingTable::_postToPhysConn( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ )
{
	if( physConn == 0 )
	{
		PLog( "CommRoutingTable: message to null PhysConn - ignored" );
		return;
	}
	if( physConn->state != _PhysConn::_StatePhysConnOk )
	{
		PLog( "CommRoutingTable: message to PhysConn with state=%d != _StatePhysConnOk - ignored", physConn->state );
		return;
	}
	m.internal.mux = static_cast< UINT16 >( mux );
	m.internal.muxChannelId = muxChannelId;
	physConn->mgr->push( physConn, m, tickCounterMsg_ );
}


void CommRoutingTable::_postToPhysConnRead( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m )
{
	if( physConn == 0 )
	{
		PLog( "CommRoutingTable: message to null PhysConn - ignored" );
		return;
	}
	if( physConn->state != _PhysConn::_StatePhysConnOk )
	{
		PLog( "CommRoutingTable: message to PhysConn with state=%d != _StatePhysConnOk - ignored", physConn->state );
		return;
	}
	m.internal.mux = static_cast< UINT16 >( mux );
	m.internal.muxChannelId = muxChannelId;
	physConn->mgr->pushRead( physConn, m );
}

void CommRoutingTable::_postToPhysConnRead( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ )
{
	if( physConn == 0 )
	{
		PLog( "CommRoutingTable: message to null PhysConn - ignored" );
		return;
	}
	if( physConn->state != _PhysConn::_StatePhysConnOk )
	{
		PLog( "CommRoutingTable: message to PhysConn with state=%d != _StatePhysConnOk - ignored", physConn->state );
		return;
	}
	m.internal.mux = static_cast< UINT16 >( mux );
	m.internal.muxChannelId = muxChannelId;
	physConn->mgr->pushRead( physConn, m, tickCounterMsg_ );
}

void CommRoutingTable::_forceDisconnect( _PhysConn* physConn )
{
	physConn->mgr->forceDisconnect( physConn );
}

/* static */ bool CommRoutingTable::_ruleMatch( _RuleParms& parms, const CommRule & rule, const char* server, bool ignoreObjects, const char* serverObject, const char* serverChannel, const char* inManager )
{
	for( int i = 0; i < _MaxRuleParms; ++i )
	{
		parms[ i ].begin = 0; 
		parms[ i ].end = 0;
	}

	if( ignoreObjects == false )
	{
		if( inManager && rule.inManager.length() )
			if( rule.inManager.equals( inManager ) == false )
				return false;
		if( serverObject && rule.maskObject.length() )
			if( PString::compareWithWildChar( rule.maskObject, serverObject ) == false )
				return false;
		if( serverChannel && rule.maskChannel.length() )
			if( PString::compareWithWildChar( rule.maskChannel, serverChannel ) == false )
				return false;
	}
	else if( rule.maskObject.length() || rule.maskChannel.length() )
	{
		return false;
	}

	const size_t n = rule.maskPatterns.size();
	const size_t serverLen = strlen( server );

	size_t total = 0;
	for( size_t i = 0; i < n; i++ )
		total += rule.maskPatterns[i].length();
	if( total > serverLen )
		return false;

	if( n == 0 || n > _MaxRuleParms + 1 )
	{
		PTRACE3( "faulty logic: _ruleMatch2 rule.maskPatterns.size(%u)", n );
		PASSERT5( 0 );
		return false;
	}

	if( n == 1 )
		return rule.maskPatterns[0].equals( server );

	const size_t nFront = rule.maskPatterns[0].length();
	if( memcmp( rule.maskPatterns[0], server, nFront ) )
		return false;

	const size_t nBack = rule.maskPatterns[n-1].length();
	if( memcmp( rule.maskPatterns[n-1], server + serverLen - nBack, nBack ) )
		return false;

	const char* s = server + nFront;
	const char* p = 0;
	parms[0].begin = server + nFront; 
	for( size_t i = 1; i < n - 1; i++ )
	{
		p = strstr( s, rule.maskPatterns[i] );
		if( ! p )
			return false;
		parms[i-1].end = p;
		s = p + rule.maskPatterns[i].length();
		parms[i].begin = s;
	}
	parms[n-2].end = server + serverLen - nBack;
	if( s >= server + serverLen - nBack )
		return false;

	return true;
}

/* static */ void CommRoutingTable::_ruleMake( PString& ret, const _RuleParms& parms, const char* ruleConn )
{
	ret = "";
	const char* p = ruleConn;
	for(;;)
	{
		const char* dollar = strchr( p, '$' );
		if( !dollar )
		{
			ret.append( p );
			return;
		}
		ret.append( p, dollar );
		char idx = *( dollar + 1 );
		if( idx == '$' )
		{
			ret.append( '$' );
			p = dollar + 2;
		}
		else if( idx >= '0' && idx < '0' + _MaxRuleParms )
		{
			int i = idx - '0';
			if (parms[i].begin)
				ret.append( parms[ i ].begin, parms[ i ].end );
			p = dollar + 2;
		}
		else
			p = dollar + 1;
	}
}

/* static */ void CommRoutingTable::substituteAddr( PString& ret, const vector< CommRule >& rules, const char* addr )
{
	int n = static_cast< int >( rules.size() );
	for( int i=0; i < n ; ++i )
	{
		CommRule rule = rules[ i ]; // suboptimal, not intended for time-critical code
		rule.parseMask();
		PASSERT3( rule.conns.size() >= 1 );
		_RuleParms parms;
		if( _ruleMatch( parms, rule, addr, true, 0, 0, 0 ) )
		{
			_ruleMake( ret, parms, *rule.conns.begin() );
			return;
		}
	}

	//match not found - will return the same address
	ret = addr;
	return;
}

void CommRoutingTable::_balanceLocation( PStringSet & validLocations )
{
	if (!_config.locationPropertyMap.size())
		return;

	PTRACE4( "CommRoutingTable: _balanceLocation" );
	int minPriority = -1;
	// remove expired attempts
	time_t cur = plib_time(0);
	PStringMap< _ConnectAttempts >::iterator att = _locationFailMap.begin();
	while(  att != _locationFailMap.end())
	{
		while( att->second.size() )
		{
			if( ( cur - att->second.begin()->time ) > optLocationFailureExpiredPeriod )
			{
				att->second.pop_front();
				PTRACE3("CommRoutingTable::_balanceLocation - failure for %s location expired, removed", att->first);
			}
			else
			{
				break;
			}
		}
		if (att->second.size())
		{
			++att;
		}
		else
		{
			att = _locationFailMap.erase(att);
		}
	}

	for (PStringMap< _ConnectProperty >::const_iterator it = _config.locationPropertyMap.begin(); it != _config.locationPropertyMap.end(); ++it)
	{
		PStringMap< _ConnectAttempts >::const_iterator att = _locationFailMap.find( it->first );
		if( ( att == _locationFailMap.end() ) || ( att->second.size() < optPhysConnLocationAttempts ) )
		{
			if (minPriority != -1)
				minPriority = ( minPriority < (*it).second.priority ) ? minPriority : (*it).second.priority;
			else
				minPriority = (*it).second.priority;
		}
	}
	PTRACE3( "CommRoutingTable: _balanceLocation - minimum priority %d", minPriority );

	if (minPriority>=0)
	{
		for (PStringMap< _ConnectProperty >::const_iterator it = _config.locationPropertyMap.begin(); it != _config.locationPropertyMap.end(); ++it)
		{
			if (minPriority == (*it).second.priority)
			{
				PTRACE3( "CommRoutingTable: _balanceLocation - location %s added", (*it).first );
				validLocations.insert((*it).first);
			}
		}
	}
}

CommRoutingTable::_PhysConn* CommRoutingTable::_balancePhysConn( const vector< BalanceListItem >& balanceList )
{
	int i;
	PTRACE4( "CommRoutingTable: trying _balancePhysConn" );
	PStringSet validLocations;
	_balanceLocation(validLocations);

	int minFailures=-1;

	PTRACE4( "CommRoutingTable: trying _balancePhysConn" );
	for (i =0; i<balanceList.size(); i++)
	{
		if (!*balanceList[i].conn->location.c_str() || !validLocations.size() || validLocations.find(balanceList[i].conn->location) != validLocations.end())
		{
			PTRACE( "CommRoutingTable: _balancePhysConn: conn=%s , failures=%d",balanceList[i].conn->address.c_str(),balanceList[i].conn->connFailures);
			if (minFailures == -1)
				minFailures = balanceList[i].conn->connFailures;
			else
				minFailures = minFailures > balanceList[i].conn->connFailures ? balanceList[i].conn->connFailures : minFailures;
		}
	}


	vector< const BalanceListItem* > tempBalanceList;
	int weight=0;
	for (i =0; i<balanceList.size(); i++)
	{
		if (!*balanceList[i].conn->location.c_str() || !validLocations.size() || validLocations.find(balanceList[i].conn->location) != validLocations.end())
		{
			if (minFailures == balanceList[i].conn->connFailures)
			{
				tempBalanceList.push_back(&balanceList[i]);
				weight+=balanceList[i].weight;
			}
		}
	}

	if (!tempBalanceList.size())
		return 0;

	if (tempBalanceList.size()==1 || !weight)
	{
		PTRACE4( "CommRoutingTable: _balancePhysConn: weight=%d, size=%d, chosen=%s",weight, tempBalanceList.size(),(*tempBalanceList.begin())->conn->address.c_str());
		return (*tempBalanceList.begin())->conn;
	}

	int randWeight = _randLocal()%weight;
	int currentWeight=0;
	for (i =0; i<tempBalanceList.size(); i++)
	{
		currentWeight += tempBalanceList[i]->weight;
		if (randWeight < currentWeight)
		{
			PTRACE4( "CommRoutingTable: connection selected among %d connections: weight=%d, randWeight=%d, currentWeight=%d, chosen=%s", tempBalanceList.size(),weight, randWeight, currentWeight,tempBalanceList[i]->conn->address.c_str() );
			return tempBalanceList[i]->conn;
		}
	}
	//	PASSERT3(0);
	return 0;
}

static inline void _patchMsg8( _CommMsg& msg, UINT32 id0, UINT32 id1, BYTE priority )
{
	PASSERT3( msg.body._size() >= 8 );
	BYTE* p = msg.body._writePtr();
	CommMsgBody::writeUINT32( p, id0 );
	p += 4;
	CommMsgBody::writeUINT32( p, id1 );
	//p += 4;
	msg.priority = priority;
}

void CommRoutingTable::_postConnRequest( _Conn* conn )
{
	PASSERT3( conn->srvThisId == 0 );
	conn->srvThisId = srvConnMap.add( conn );

	CommMsgBody body;
	body.copyFrom( conn->request );
	PASSERT3( body._size() >= 4 );
	BYTE* p = body._writePtr();
	CommMsgBody::writeUINT32( p, conn->srvThisId );
	//p += 4;

	PTRACE4( "CommRoutingTable: posting connection request for cliId=%08X -> srvId=%08X", conn->cliThisId, conn->srvThisId );
	_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST, conn->priority, body );
	if (conn->version == 1)
	{
		m.body.composeUINT32(conn->serverId);
		m.flags |= _COMM_MSGFLAGS_CONNECTION_EX;
		if (conn->serverId)
			m.flags |= _COMM_MSGFLAGS_CONNECTION_EX_RECONNECT;
	}
	_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, m );
	conn->state = _CommConnection::_StateConnRequestSent;
	conn->srvId = 0;//for special error cases
}

void CommRoutingTable::_postSubscrRequest( _Subscr* subscr )
{
	PASSERT3( subscr->srvThisId == 0 );
	subscr->srvThisId = srvSubscrMap.add( subscr );

	CommMsgBody body;
	body.composeUINT32( subscr->srvThisId ).composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel );
	if (subscr->protocolVersion == 0)
	{
		subscr->genSyncRequest( body );
	}
	else
	{
		subscr->genSyncRequestExPhase1( body );
	}

	PTRACE4( "CommRoutingTable: posting subscription request (version=%d) for '%s'/'%s'/'%s' -> srvId=%08X", subscr->protocolVersion, subscr->server, subscr->serverObject, subscr->serverChannel, subscr->srvThisId );
	_CommMsg m( (subscr->protocolVersion == 0? _COMM_MSGTYPE_SUBSCRIPTION_REQUEST : _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1), subscr->priority, body );
#ifdef COMMLIB_CALCULATE_STATISTICS
	if (subscr->trafficCntPos>=0)
	{
		PInterlockedExchangeAdd(&_subscrTrafficStatCounters[3][subscr->trafficCntPos],m.body._size());
		PInterlockedIncrement(&_subscrTrafficStatPacketCounters[3][subscr->trafficCntPos]);
		if (printTrafficLast)
		{
			if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
				PLog("Traffic subscr to server: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
		}

	}
#endif

	_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
	subscr->state = CommSubscription::_StateSubscrRequestSent;
	subscr->srvId = 0;//for special error cases
}

void CommRoutingTable::_postSubscrLightRequest( _SubscrLight* subscr )
{
	PASSERT3( subscr->srvThisId == 0 );
	subscr->srvThisId = srvSubscrLightMap.add( subscr );

	CommMsgBody body;
	body.composeUINT32( subscr->srvThisId ).composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel );
	PTRACE4( "CommRoutingTable: posting light subscription request for '%s'/'%s'/'%s' -> srvId=%08X",  subscr->server, subscr->serverObject, subscr->serverChannel, subscr->srvThisId );
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST , subscr->priority, body );
	_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
	subscr->state = CommSubscription::_StateSubscrRequestSent;
	subscr->srvId = 0;//for special error cases
}



void CommRoutingTable::_postConnAccept( _Conn* conn )
{
	_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED, conn->priority );
	m.body.composeUINT32( conn->cliId ).composeUINT32( conn->cliThisId );
	_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, m );
}

void CommRoutingTable::_postSubscrAccept( const _SubscrLightLeaf* leaf, bool lightType )
{
	_CommMsg m( (lightType?_COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED:_COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED), leaf->subscr->priority );
	m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId );
	_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
}

void CommRoutingTable::_physicallyConnected( _CommPhysicalConnection* physConn_, int thread, int slot, UINT32 channelId, int isLocal )
{
	_PhysConn* physConn = (_PhysConn*)physConn_;//safe because _CommPhysicalConnection is always created by CommRoutingTable as _PhysConn
	PTRACE4( "CommRoutingTable: _physicallyConnected");
	PWLock lock( cs );
	if( shuttingDown )
		return;
	PASSERT3( physConn->state == _PhysConn::_StatePhysConnRequestSent || physConn->state == _PhysConn::_StatePhysConnIncomingDisc || physConn->state == _PhysConn::_StatePhysConnReconn );
	physConn->thread = thread;
	physConn->slot = slot;
	physConn->channelId = channelId;
	if( isLocal >= 0 )
		physConn->isLocal = isLocal;
	physConn->state = _PhysConn::_StatePhysConnOk;
	physConn->connFailures = 0;
	physConn->lastErrorCode = 0;
	physConn->firstFailureTime = 0;
	physConn->failureTimeStatus = 0;
	{
		for( _ConnSetByCli::iterator iter = physConn->cliConns.begin() ; iter != physConn->cliConns.end() ; ++iter )
		{
			_Conn* conn = *iter;
			PASSERT3( conn->srv == physConn );
			PASSERT3( conn->state == _CommConnection::_StateConnDisc );
			_postConnRequest( conn );//sets _StateConnRequestSent
		}
	}

	{
		for( _SubscrSet::iterator iter = physConn->cliSubscrs.begin() ; iter != physConn->cliSubscrs.end() ; ++iter )
		{
			_Subscr* subscr = *iter;
			PASSERT3( subscr->srv == physConn );
			PASSERT3( subscr->state == CommSubscription::_StateSubscrDisc );
			if (subscr->frozen == false)
				_postSubscrRequest( subscr );//sets _StateSubscrRequestSent
		}
	}
	{
		for( _SubscrLightSet::iterator iter = physConn->cliSubscrsLight.begin() ; iter != physConn->cliSubscrsLight.end() ; ++iter )
		{
			_SubscrLight* subscr = *iter;
			PASSERT3( subscr->srv == physConn );
			PASSERT3( subscr->state == CommSubscription::_StateSubscrDisc );
			_postSubscrLightRequest( subscr );
		}
	}

	if (physConn->inbound == false && physConn->mgr->isForceClientDisconnects())
	{
		for( _ManagerList::iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
		{
			if ((*iter)->isAllowedClientDisconnects() && 
				(*iter)->isInStateOfClosedAccept())
			{
				(*iter)->acceptReinstate();
			}
		}
	}

}

void CommRoutingTable::physicallyConnected( _CommPhysicalConnection* physConn_, int thread, int slot, UINT32 channelId, int isLocal )
{
	_physicallyConnected( physConn_, thread, slot, channelId, isLocal );
}

void CommRoutingTable::_reconnect( _Conn* conn )
{
	UINT16 errCode;
	const char* errMsg;
	int muxSrv;
	UINT32 muxSrvChannelId;
	_PhysConn* srvPhysConn = _findOrCreatePhysConn( muxSrv, muxSrvChannelId, conn->server, conn->serverObject,conn->serverChannel, conn->cli->mgr->getType(), false ,errCode, errMsg );
	if( srvPhysConn )
	{
		PASSERT3( conn->srv == 0 );
		conn->srv = srvPhysConn;
		conn->muxSrv = muxSrv;
		conn->muxSrvChannelId = muxSrvChannelId;
		srvPhysConn->cliConns.insert( conn );

		switch( srvPhysConn->state )
		{
		case _PhysConn::_StatePhysConnOk:
			_postConnRequest( conn );//sets _StateConnRequestSent
			break;
		case _PhysConn::_StatePhysConnDisc:
			srvPhysConn->mgr->connect( srvPhysConn );
			srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
			break;
		case _PhysConn::_StatePhysConnRequestSent:
		case _PhysConn::_StatePhysConnReconn:
			//do nothing
			break;
		default:
			PASSERT3( 0 );
			break;
		}
	}
	else
	{
		_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, conn->priority );
		m.body.composeUINT32( conn->cliId ).composeUINT32( conn->cliThisId ).composeUINT16( errCode ).composeString( errMsg );
		_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, m );
		_delConn( conn );
	}
}

void CommRoutingTable::_resubscribe( _Subscr* subscr )
{
	UINT16 errCode;
	const char* errMsg;
	int muxSrv;
	UINT32 muxSrvChannelId;
	_PhysConn* srvPhysConn = _findOrCreatePhysConn( muxSrv, muxSrvChannelId, subscr->server, subscr->serverObject, subscr->serverChannel, 0, true, errCode, errMsg );
	if( srvPhysConn )
	{
		PASSERT3( subscr->srv == 0 );
		subscr->srv = srvPhysConn;
		subscr->muxSrv = muxSrv;
		subscr->muxSrvChannelId = muxSrvChannelId;
		srvPhysConn->cliSubscrs.insert( subscr );

		switch( srvPhysConn->state )
		{
		case _PhysConn::_StatePhysConnOk:
			_postSubscrRequest( subscr );//sets _StateSubscrRequestSent
			break;
		case _PhysConn::_StatePhysConnDisc:
			srvPhysConn->mgr->connect( srvPhysConn );
			srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
			break;
		case _PhysConn::_StatePhysConnRequestSent:
		case _PhysConn::_StatePhysConnReconn:
			//do nothing
			break;
		default:
			PASSERT3( 0 );
			break;
		}
	}
	else
	{
		for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
		{
			_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, subscr->priority );
			m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeUINT16( errCode ).composeString( errMsg );
			_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
		}
		_delSubscr( subscr );
	}
}

void CommRoutingTable::_resubscribeLight( _SubscrLight* subscr )
{
	UINT16 errCode;
	const char* errMsg;
	int muxSrv;
	UINT32 muxSrvChannelId;
	_PhysConn* srvPhysConn = _findOrCreatePhysConn( muxSrv, muxSrvChannelId, subscr->server, subscr->serverObject, subscr->serverChannel, 0, true, errCode, errMsg );
	if( srvPhysConn )
	{
		PASSERT3( subscr->srv == 0 );
		subscr->srv = srvPhysConn;
		subscr->muxSrv = muxSrv;
		subscr->muxSrvChannelId = muxSrvChannelId;
		srvPhysConn->cliSubscrsLight.insert( subscr );

		switch( srvPhysConn->state )
		{
		case _PhysConn::_StatePhysConnOk:
			_postSubscrLightRequest( subscr );//sets _StateSubscrRequestSent
			break;
		case _PhysConn::_StatePhysConnDisc:
			srvPhysConn->mgr->connect( srvPhysConn );
			srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
			break;
		case _PhysConn::_StatePhysConnRequestSent:
		case _PhysConn::_StatePhysConnReconn:
			//do nothing
			break;
		default:
			PASSERT3( 0 );
			break;
		}
	}
	else
	{
		for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
		{
			_SubscrLightLeaf* leaf = *iter;
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, subscr->priority );
			m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeUINT16( errCode ).composeString( errMsg );
			_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
		}
		_delSubscrLight( subscr );
	}
}

void CommRoutingTable::_scheduleDelSubscr( _Subscr* subscr )
{
	subscr->deleteSubscrTime = SYSTEMTICKS();
	if (subscr->timerWasPost == false)
	{
		CommMsgBody timerMsg;
		timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->deleteSubscrTime);
		_CommMsg msg( _TIMER_DELSUBSCR, 0, timerMsg );
		timerThread->push( msg, optSubscrExpire );
		subscr->timerWasPost = true;
	}
}

void CommRoutingTable::_scheduleDelSubscrLight( _SubscrLight* subscr )
{
	CommMsgBody body;
	body.composeUINT32( subscr->srvId ).composeUINT32( subscr->srvThisId );
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE, subscr->priority, body );
	_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
	_delSubscrLight( subscr );
}

void CommRoutingTable::loggerInformation(CommMsgBody & body)
{
	PWLock lock( cs );
	if (doCounting)
	{
		if( optTimeoutPerformanceCounting >= 1000 )
		{
			composeLoggerStat(body, "avgProcessingTime", PUMulDiv(_totalTicksAfterLock,60*1000,optTimeoutPerformanceCounting));
			composeLoggerStat(body, "numberOfMessages", PUMulDiv(_totalMessages,60*1000,optTimeoutPerformanceCounting));
			composeLoggerStat(body, "maxThreadCPU", PUMulDiv(_maxThreadTime,60*1000,optTimeoutPerformanceCounting));
			composeLoggerStat(body, "Number of exceptions", PSystemExceptionError::getNumberOfExceptions());
			composeLoggerStat(body, "Number of Asserts", PAssert::getNumberOfAsserts());
			composeLoggerStat(body, "Number of Memory failurs", PSystemMemoryError::getNumberOfExceptions());
		}

		composeLoggerStat(body, "Delays", _statisticDelays);
		composeLoggerStat(body, "Timeouts", _statisticTimeouts);
		composeLoggerStat(body, "Disconnects", _statisticDisconnects);
		composeLoggerStat(body, "Connections denied", _statisticConnectionDenied);
		composeLoggerStat(body, "Subscriptions denied", _statisticSubscriptionDenied);

		_statisticConnectionDenied=0;
		_statisticSubscriptionDenied=0;
		_statisticTimeouts = 0;
		_statisticDelays=0;
		_statisticDisconnects=0;
	}
	for( _ManagerList::iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
	{
		(*iter)->loggerInformation(body);
	}
}

void CommRoutingTable::dump()
{
	PWLock lock( cs ); // Write lock - to prevent log interruption by other messages
	PLog("Dump===  Routing table dump:");
	{
		for( _PhysConnStringMap::const_iterator it = outboundConns.begin(); it != outboundConns.end() ; ++it )
		{
			connDump((*it).second,"Outbound");
		}
	}
	{
		for( _PhysConnMuxStringMap::const_iterator it = localConns.begin(); it != localConns.end() ; ++it )
		{
			PString name;
			name.assign("Local:'").append((*it).first).append("', mux=").appendInt((*it).second.mux).append(", muxChannelId=").appendHexInt((*it).second.muxChannelId);
			connDump((*it).second.physConn,name);
		}
	}
	{
		for( _PhysConnSet::const_iterator it = inboundConns.begin(); it != inboundConns.end() ; ++it )
		{
			connDump(*it,"Inbound");
		}
	}
	{
		PLog("Dump===  Managers:");
		for( _ManagerList::const_iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
			(*iter)->dump();
	}
}

void CommRoutingTable::connDump( const _PhysConn* physConn, const char * typeConnection ) const
{
	PLog("Dump=== %s phys connection %p, address=%s, thread=%d, slot=%d, channelId=%x, state=%d, isLocal=%d, quality=%d, cliConns.size=%d, srvConns.size=%d, cliPreConns.size=%d, cliSubscrs.size=%d, srvSubscrs.size=%d, cliSubscrsLight.size=%d, srvSubscrsLight.size=%d, location=%s, pseudo='%s'/'%s'/'%s', exists=%u, tableSubscrs=%u, lastSubscr=%u, rateSubscr=%u",
		typeConnection,
		physConn,
		physConn->address.c_str(),
		physConn->thread,
		physConn->slot,
		physConn->channelId,
		physConn->state,physConn->isLocal,physConn->quality,
		physConn->cliConns.size(),
		physConn->srvConns.size(),
		physConn->cliPreConns.size(),
		physConn->cliSubscrs.size(),
		physConn->srvSubscrs.size(),
		physConn->cliSubscrsLight.size(),
		physConn->srvSubscrsLight.size(),
		physConn->location.c_str(),
		physConn->pseudoConnection.server.c_str(),
		physConn->pseudoConnection.serverObject.c_str(),
		physConn->pseudoConnection.serverChannel.c_str(),
		(SYSTEMTICKS() - physConn->connCreated),
		physConn->subscrCounter,
		(SYSTEMTICKS() - physConn->lastSubscrTime),
		physConn->subscrRate(60000)
	);
}

void CommRoutingTable::adjustFailureWeight()
{
	int minFailure=-1;
	_PhysConnStringMap::iterator it;
	for (it=outboundConns.begin();it != outboundConns.end(); ++it)
	{
		if (minFailure == -1)
			minFailure= (*it).second->connFailures;
		else
			minFailure= minFailure > (*it).second->connFailures?(*it).second->connFailures:minFailure;
	}
	if (minFailure>0)
	{
		for (it=outboundConns.begin();it != outboundConns.end(); ++it)
		{
            (*it).second->connFailures-=minFailure;
		}
	}
}

void CommRoutingTable::physicalConnectFailed( _CommPhysicalConnection* physConn_, int failureWeight, int errorCode )
{
	_PhysConn* physConn = (_PhysConn*)physConn_;//safe because _CommPhysicalConnection is always created by CommRoutingTable as _PhysConn

	PTRACE4("physicalConnectFailed %s , failure=%d",physConn_->address.c_str(),failureWeight);
	PWLock lock( cs );
	if( shuttingDown )
		return;
	_forceClientsDisconnect(physConn);
	physConn->state = _PhysConn::_StatePhysConnDisc;

	if (physConn->mgr->getConnectAttemptsDdosProtection() > 0)
	{
		physConn->ddosProtectionAttempts++;
		if (physConn->ddosProtectionAttempts > physConn->mgr->getConnectAttemptsDdosProtection())
			physConn->ddosProtectionAttempts = 0;
	}

	if (physConn->ddosProtectionAttempts == 0)
	{
		if (*physConn->location.c_str())
		{
			PStringMap< _ConnectAttempts >::iterator it;
			it = _locationFailMap.insert(make_pair(physConn->location, _ConnectAttempts())).first;
			if (it != _locationFailMap.end())
			{
				time_t cur = plib_time(0);
				if ((0 == it->second.size()) || (it->second[it->second.size() - 1].time <= cur))
					it->second.push_back(_ConnectAttemptItem(cur));
				PTRACE4("CommRoutingTable::physicalConnectFailed - failure for location %s added", (*it).first);
			}
		}

		if (failureWeight >= 0)
		{
			physConn->connFailures += failureWeight;
			physConn->lastErrorCode = errorCode;

			if (!optDisableAdjustFailureWeight)
			{
				adjustFailureWeight();
			}
			if (optPhysConnFailuresTimeout > 0)//otherwise don't care about physConn->firstFailureTime
			{
				UINT32 now = SYSTEMTICKS();//don't care if now == 0 - will be ignored once
				if (physConn->failureTimeStatus == 0)
				{
					physConn->firstFailureTime = now;
					physConn->failureTimeStatus = 1;
				}
				else if (physConn->failureTimeStatus < 0)//already locked
					;//sic!
				else if (optPhysConnFailuresTimeout > 0 && static_cast<int>(now - physConn->firstFailureTime) > optPhysConnFailuresTimeout * TICKS_PER_SECOND)
				{
					PTRACE3("CommRoutingTable: remotePhysConn exceeded failures timeout - will be ignored");
					physConn->failureTimeStatus = -1;
				}
			}
		}
		else
		{
			PTRACE3("CommRoutingTable: remotePhysConn unrecoverable failure - will be ignored");
			physConn->failureTimeStatus = -1;
			physConn->lastErrorCode = errorCode;
		}
	}
	PASSERT3( physConn->srvConns.size() == 0 );
	PASSERT3( physConn->srvSubscrs.size() == 0 );
	PASSERT3( physConn->srvSubscrsLight.size() == 0 );
	PASSERT3( !physConn->inbound );

	{
		int n = static_cast< int >( physConn->cliConns.size() );
		if( n )
		{
			PArrayAutoPtr< _Conn* > conns( new _Conn*[ n ] );//!cannot call cliConns.erase within cliConns iteration
			int connI = 0;
			for( _ConnSetByCli::iterator iter = physConn->cliConns.begin() ; iter != physConn->cliConns.end() ; ++iter )
			{
				_Conn* conn = *iter;
				PASSERT3( conn->srv == physConn );

				PASSERT3( conn->state == _CommConnection::_StateConnDisc );
				PASSERT3( conn->srvThisId == 0 );
				PASSERT3( conn->cliThisId != 0 );

				PASSERT3( connI < n );
				conns[ connI++ ] = conn;
			}
			PASSERT3( connI == n );

			CommMsgBody timerMsg;
			for( int i=0; i < n ; ++i )
			{
				_Conn* conn = conns[ i ];

				/*
				{
				_CommMsg m( _COMM_MSGTYPE_CONNECT_REQUEST_PENDING, conn->priority );
				m.body.composeUINT32( conn->cliId ).composeUINT32( conn->cliThisId ).composeString( physConn->address );
				_postToPhysConn( conn->cli, m );
				}
				*/

				PASSERT3( conn->srv == physConn );
				_ConnSetByCli::iterator found = physConn->cliConns.find( conn );
				PASSERT3( found != physConn->cliConns.end() );
				physConn->cliConns.erase( found );
				conn->srv = 0;
				timerMsg.composeUINT32( conn->cliThisId );
			}
			timerMsg.composeUINT32( 0 );
			_CommMsg msg( _TIMER_RECONN, 0, timerMsg );
			timerThread->push( msg, optReconnTimeout );
		}
	}

	{
		int n = static_cast< int >( physConn->cliSubscrs.size() );
		if( n )
		{
			PArrayAutoPtr< _Subscr* > subscrs( new _Subscr*[ n ] );//!cannot call cliSubscrs.erase within cliSubscrs iteration
			int subscrI = 0;
			for( _SubscrSet::iterator iter = physConn->cliSubscrs.begin() ; iter != physConn->cliSubscrs.end() ; ++iter )
			{
				_Subscr* subscr = *iter;
				PASSERT3( subscr->srv == physConn );

				PASSERT3( subscr->state == CommSubscription::_StateSubscrDisc );
				PASSERT3( subscr->srvThisId == 0 );
				PASSERT3( subscrI < n );
				subscrs[ subscrI++ ] = subscr;
			}
			PASSERT3( subscrI == n );

			CommMsgBody timerMsg;
			bool haveSomethingToResubscribe = false;
			for( int i=0; i < n ; ++i )
			{
				_Subscr* subscr = subscrs[ i ];
				PASSERT3( subscr->srv == physConn );
				_SubscrSet::iterator found = physConn->cliSubscrs.find( subscr );
				PASSERT3( found != physConn->cliSubscrs.end() );
				physConn->cliSubscrs.erase( found );
				subscr->srv = 0;
				if (subscr->frozen == false)
				{
					timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->_subscrUniqueId);
					haveSomethingToResubscribe = true;
				}
			}
			if (haveSomethingToResubscribe)
			{
				timerMsg.composeString( "" ).composeString( "" ).composeString( "" ).composeUINT32(0);
				_CommMsg msg( _TIMER_RESUBSCR, 0, timerMsg );
				timerThread->push( msg, optReconnTimeout );
			}
		}
	}

	{
		int n = static_cast< int >( physConn->cliSubscrsLight.size() );
		if( n )
		{
			PArrayAutoPtr< _SubscrLight* > subscrs( new _SubscrLight*[ n ] );//!cannot call cliSubscrs.erase within cliSubscrs iteration
			int subscrI = 0;
			for( _SubscrLightSet::iterator iter = physConn->cliSubscrsLight.begin() ; iter != physConn->cliSubscrsLight.end() ; ++iter )
			{
				_SubscrLight* subscr = *iter;
				PASSERT3( subscr->srv == physConn );

				PASSERT3( subscr->state == CommSubscription::_StateSubscrDisc );
				PASSERT3( subscr->srvThisId == 0 );
				PASSERT3( subscrI < n );
				subscrs[ subscrI++ ] = subscr;
			}
			PASSERT3( subscrI == n );

			CommMsgBody timerMsg;
			for( int i=0; i < n ; ++i )
			{
				_SubscrLight* subscr = subscrs[ i ];
				PASSERT3( subscr->srv == physConn );
				_SubscrLightSet::iterator found = physConn->cliSubscrsLight.find( subscr );
				PASSERT3( found != physConn->cliSubscrsLight.end() );
				physConn->cliSubscrsLight.erase( found );
				subscr->srv = 0;
				timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->_subscrUniqueId);
			}
			if ( n )
			{
				timerMsg.composeString( "" ).composeString( "" ).composeString( "" ).composeUINT32(0);
				_CommMsg msg( _TIMER_RESUBSCR_LIGHT, 0, timerMsg );
				timerThread->push( msg, optReconnTimeout );
			}
		}
	}

	if (physConn->mgr->isForceClientDisconnects())
	{
		// restore connection using pseudo connection
		CommMsgBody timerMsg;
		timerMsg.composeString( physConn->pseudoConnection.server ).composeString( physConn->pseudoConnection.serverObject ).composeString( physConn->pseudoConnection.serverChannel );
		_CommMsg msg( _TIMER_PHYSRECONNECT, 0, timerMsg );
		timerThread->push( msg, optReconnTimeout );
	}
}

void CommRoutingTable::_postTransitDisc( _PhysConn* physConn, int mux, UINT32 muxChannelId, BYTE msgType, BYTE priority, UINT32 peerId, UINT32 thisId )
{
	_CommMsg m( msgType, priority );
	m.body.composeUINT32( peerId ).composeUINT32( thisId );
	_postToPhysConn( physConn, mux, muxChannelId, m );
}

void CommRoutingTable::_cleanupPhysConnMux( _PhysConn* physConn, int mux )
{//MUST be locked before call
	{
		int n = static_cast< int >( physConn->cliConns.size() );
		if( n )
		{
			PArrayAutoPtr< _Conn* > conns( new _Conn*[ n ] );//!cannot call cliConns.erase within cliConns iteration
			int connI = 0;
			for( _ConnSetByCli::iterator iter = physConn->cliConns.begin() ; iter != physConn->cliConns.end() ; ++iter )
			{
				_Conn* conn = *iter;
				PASSERT3( conn->srv == physConn );
				PASSERT3( connI < n );
				if( mux < 0 || conn->muxSrv == mux )
					conns[ connI++ ] = conn;
			}
			PASSERT3( connI <= n );

			if( physConn->inbound )
				for( int i=0; i < connI ; ++i )
				{
					_Conn* conn = conns[ i ];
					_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, conn->priority );
					m.body.composeUINT32( conn->cliId ).composeUINT32( conn->cliThisId ).composeUINT16( _COMM_ERROR_INBOUNDCONNCLOSED ).composeString( "Inbound connection disconnected - no way to restore connection" );
					_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, m );
					_delConn( conn );
				}
			else
			{
				CommMsgBody timerMsg;
				for( int i=0; i < connI ; ++i )
				{
					_Conn* conn = conns[ i ];
					_postTransitDisc( conn->cli, conn->muxCli, conn->muxCliChannelId, _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT, _COMM_MSGPRIORITY_HIGHPRIORITY_DISCONNECT, conn->cliId, conn->cliThisId );
					_postTransitDisc( conn->cli, conn->muxCli, conn->muxCliChannelId, _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT, conn->priority, conn->cliId, conn->cliThisId );

					conn->state = _CommConnection::_StateConnDisc;
					if( conn->srvThisId )
						srvConnMap.remove( conn->srvThisId );
					conn->srvThisId = 0;

					PASSERT3( conn->srv == physConn );
					_ConnSetByCli::iterator found = physConn->cliConns.find( conn );
					PASSERT3( found != physConn->cliConns.end() );
					physConn->cliConns.erase( found );
					conn->srv = 0;

					timerMsg.composeUINT32( conn->cliThisId );
				}
				timerMsg.composeUINT32( 0 );
				_CommMsg msg( _TIMER_RECONN, 0, timerMsg );
				timerThread->push( msg, optReconnTimeout );
			}
		}
	}

	{
		int n = static_cast< int >( physConn->srvConns.size() );
		if( n )
		{
			PArrayAutoPtr< _Conn* > conns( new _Conn*[ n ] );//although it seems safe to call _delConn within srvConns iteration, but it changes srvConns, so we will move it out of the loop - just in case
			int connI = 0;
			for( _ConnSetBySrv::iterator iter = physConn->srvConns.begin() ; iter != physConn->srvConns.end() ; ++iter )
			{
				_Conn* conn = *iter;
				PASSERT3( conn->cli == physConn );
				PASSERT3( connI < n );
				if( mux < 0 || conn->muxCli == mux )
				{
					_postTransitDisc( conn->srv, conn->muxSrv, conn->muxSrvChannelId, _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT, _COMM_MSGPRIORITY_HIGHPRIORITY_DISCONNECT, conn->srvId, conn->srvThisId );
					_postTransitDisc( conn->srv, conn->muxSrv, conn->muxSrvChannelId, _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT, conn->priority, conn->srvId, conn->srvThisId );
					conns[ connI++ ] = conn;
				}
			}
			PASSERT3( connI <= n );
			for( int i=0; i < connI ; ++i )
				_delConn( conns[ i ] );
		}
	}

	{
		int n = static_cast< int >( physConn->cliSubscrs.size() );
		if( n )
		{
			PArrayAutoPtr< _Subscr* > subscrs( new _Subscr*[ n ] );//!cannot call cliSubscrs.erase within cliSubscrs iteration
			int subscrI = 0;
			for( _SubscrSet::iterator iter = physConn->cliSubscrs.begin() ; iter != physConn->cliSubscrs.end() ; ++iter )
			{
				_Subscr* subscr = *iter;
				PASSERT3( subscr->srv == physConn );
				PASSERT3( subscrI < n );
				if( mux < 0 || subscr->muxSrv == mux )
					subscrs[ subscrI++ ] = subscr;
			}
			PASSERT3( subscrI <= n );

			if( physConn->inbound )
				for( int i=0; i < subscrI ; ++i )
				{
					_Subscr* subscr = subscrs[ i ];
					for( _SubscrLeafSetByCli::iterator jter = subscr->cli.begin(); jter != subscr->cli.end() ; ++jter )
					{
						_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*jter);

						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, subscr->priority );
						m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeUINT16( _COMM_ERROR_INBOUNDCONNCLOSED ).composeString( "Inbound connection disconnected - no way to restore connection" );
						_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
					}

					_delSubscr( subscr );
				}
			else
			{
				CommMsgBody timerMsg;
				bool haveSomethingToResubscribe = false;
				for( int i=0; i < subscrI ; ++i )
				{
					_Subscr* subscr = subscrs[ i ];
					for( _SubscrLeafSetByCli::iterator jter = subscr->cli.begin(); jter != subscr->cli.end() ; ++jter )
					{
						_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*jter);
						if (leaf->frozen != _SubscrLeaf::_frozenStateFrozen)
							_postTransitDisc( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT, subscr->priority, leaf->cliId, leaf->cliThisId );
					}

					subscr->state = CommSubscription::_StateSubscrDisc;
					if( subscr->srvThisId )
						srvSubscrMap.remove( subscr->srvThisId );
					subscr->srvThisId = 0;

					PASSERT3( subscr->srv == physConn );
					_SubscrSet::iterator found = physConn->cliSubscrs.find( subscr );
					PASSERT3( found != physConn->cliSubscrs.end() );
					physConn->cliSubscrs.erase( found );
					subscr->srv = 0;
					if (subscr->frozen == false)
					{
						timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->_subscrUniqueId);
						haveSomethingToResubscribe = true;
					}
				}
				if (haveSomethingToResubscribe)
				{
					timerMsg.composeString( "" ).composeString( "" ).composeString( "" ).composeUINT32(0);
					_CommMsg msg( _TIMER_RESUBSCR, 0, timerMsg );
					timerThread->push( msg, optReconnTimeout );
				}
			}
		}
	}

	{
		int n = static_cast< int >( physConn->srvSubscrs.size() );
		if( n )
		{
			PArrayAutoPtr< _SubscrLeaf* > leaves( new _SubscrLeaf*[ n ] );//although it seems safe to call _delSubscr within srvSubscrs iteration, but it changes srvSubscrs, so we will move it out of the loop - just in case
			int leafI = 0;
			for( _SubscrLeafSet::iterator iter = physConn->srvSubscrs.begin() ; iter != physConn->srvSubscrs.end() ; ++iter )
			{
				_SubscrLeaf* leaf = *iter;
				PASSERT3( leaf->cli == physConn );
				PASSERT3( leafI < n );
				if( mux < 0 || leaf->muxCli == mux )
					leaves[ leafI++ ] = leaf;
			}
			PASSERT3( leafI <= n );
			for( int i=0; i < leafI ; ++i )
			{
				_Subscr* subscr = static_cast<_Subscr*>(leaves[ i ]->subscr);
				_delSubscrLeaf( leaves[ i ] );
				if( subscr->cli.size() == 0 )
					_scheduleDelSubscr( subscr );
			}
		}

	}

	{
		int n = static_cast< int >( physConn->cliSubscrsLight.size() );
		if( n )
		{
			PArrayAutoPtr< _SubscrLight* > subscrs( new _SubscrLight*[ n ] );
			int subscrI = 0;
			for( _SubscrLightSet::iterator iter = physConn->cliSubscrsLight.begin() ; iter != physConn->cliSubscrsLight.end() ; ++iter )
			{
				_SubscrLight* subscr = *iter;
				PASSERT3( subscr->srv == physConn );
				PASSERT3( subscrI < n );
				if( mux < 0 || subscr->muxSrv == mux )
					subscrs[ subscrI++ ] = subscr;
			}
			PASSERT3( subscrI <= n );

			if( physConn->inbound )
				for( int i=0; i < subscrI ; ++i )
				{
					_SubscrLight* subscr = subscrs[ i ];
					for( _SubscrLeafSetByCli::iterator jter = subscr->cli.begin(); jter != subscr->cli.end() ; ++jter )
					{
						_SubscrLightLeaf* leaf = *jter;

						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, subscr->priority );
						m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeUINT16( _COMM_ERROR_INBOUNDCONNCLOSED ).composeString( "Inbound connection disconnected - no way to restore connection" );
						_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
					}

					_delSubscrLight( subscr );
				}
			else
			{
				CommMsgBody timerMsg;
				for( int i=0; i < subscrI ; ++i )
				{
					_SubscrLight* subscr = subscrs[ i ];
					for( _SubscrLeafSetByCli::iterator jter = subscr->cli.begin(); jter != subscr->cli.end() ; ++jter )
					{
						_SubscrLightLeaf* leaf = *jter;
						_postTransitDisc( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT, subscr->priority, leaf->cliId, leaf->cliThisId );
					}

					subscr->state = CommSubscription::_StateSubscrDisc;
					if( subscr->srvThisId )
						srvSubscrLightMap.remove( subscr->srvThisId );
					subscr->srvThisId = 0;

					PASSERT3( subscr->srv == physConn );
					_SubscrLightSet::iterator found = physConn->cliSubscrsLight.find( subscr );
					PASSERT3( found != physConn->cliSubscrsLight.end() );
					physConn->cliSubscrsLight.erase( found );
					subscr->srv = 0;
					timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->_subscrUniqueId);
				}
				if (subscrI)
				{
					timerMsg.composeString( "" ).composeString( "" ).composeString( "" ).composeUINT32(0);
					_CommMsg msg( _TIMER_RESUBSCR_LIGHT, 0, timerMsg );
					timerThread->push( msg, optReconnTimeout );
				}
			}
		}
	}

	{
		int n = static_cast< int >( physConn->srvSubscrsLight.size() );
		if( n )
		{
			PArrayAutoPtr< _SubscrLightLeaf* > leaves( new _SubscrLightLeaf*[ n ] );//although it seems safe to call _delSubscr within srvSubscrs iteration, but it changes srvSubscrs, so we will move it out of the loop - just in case
			int leafI = 0;
			for( _SubscrLightLeafSet::iterator iter = physConn->srvSubscrsLight.begin() ; iter != physConn->srvSubscrsLight.end() ; ++iter )
			{
				_SubscrLightLeaf* leaf = *iter;
				PASSERT3( leaf->cli == physConn );
				PASSERT3( leafI < n );
				if( mux < 0 || leaf->muxCli == mux )
					leaves[ leafI++ ] = leaf;
			}
			PASSERT3( leafI <= n );
			for( int i=0; i < leafI ; ++i )
			{
				_SubscrLight* subscr = leaves[ i ]->subscr;
				_delSubscrLightLeaf( leaves[ i ] );
				if( subscr->cli.size() == 0 )
					_scheduleDelSubscrLight( subscr );
			}
		}

	}



	if( physConn->inbound )
	{
		//PASSERT3( physConn->srvConns.size() == 0 );
		//PASSERT3( physConn->cliConns.size() == 0 );
		//PASSERT3( physConn->srvSubscrs.size() == 0 );
		//PASSERT3( physConn->cliSubscrs.size() == 0 );

		bool found = false;
		for( _PhysConnMuxStringMap::iterator iter = localConns.begin() ; iter != localConns.end() ;  )
		{
			_PhysConnMuxStringMap::iterator nextIter = iter;
			++nextIter;
			if( (*iter).second.physConn == physConn )
			{
				if( mux < 0 || (*iter).second.mux == mux )
				{
					localConns.erase( iter );//invalidates iter but not nextIter
					found = true;
				}
			}
			iter = nextIter;
		}
		//if( !found )
		//	inboundConns.erase( physConn );
	}
}

void CommRoutingTable::_physicallyDisconnected( _CommPhysicalConnection* physConn_, int physConnState )
{
	_PhysConn* physConn = (_PhysConn*)physConn_;//safe because _CommPhysicalConnection is always created by CommRoutingTable as _PhysConn
	PTRACE4("physicalDisconnected %s , state=%d",physConn_->address.c_str(),physConnState);
	PWLock lock( cs );
	if( shuttingDown )
		return;
	physConn->state = physConnState;
	_cleanupPhysConnMux( physConn, -1 );
	if( physConn->inbound )
	{
		inboundConns.erase( physConn );	
		delete physConn;
	}
}

void CommRoutingTable::cleanMgrConnections(CommThreadManager * mgr, int threadNum)
{
	PWLock lock( cs );
	vector< _PhysConn* > v;
	for( _PhysConnSet::iterator it = inboundConns.begin(); it != inboundConns.end() ;++it)
	{
		_PhysConn* physConn = *it;
		if (physConn->mgr == mgr && physConn->thread == threadNum)
		{
			v.push_back( physConn );
		}
	}
	for( vector< _PhysConn* >::iterator iv = v.begin(); iv != v.end() ;++iv)
	{
		_PhysConn* physConn = *iv;
		physicallyDisconnected(physConn);
	}
}

void CommRoutingTable::physicallyDisconnected( _CommPhysicalConnection* physConn_, int errCode )
{
	if (doCounting)
	{
		_PhysConn* physConn = (_PhysConn*)physConn_;
		if (physConn->inbound && physConn->isLocal == 0)
		{
			_statisticDisconnects++;
			if (errCode == _SOCKET_TIMEOUT_EXPIRED || errCode == _SOCKET_HANDSHAKE_TIMEOUT_EXPIRED)
			{
				_statisticTimeouts++;
			}
		}
	}
	{
		_PhysConn* physConn = (_PhysConn*)physConn_;
		if (*physConn->location.c_str())
		{
			PStringMap< _ConnectAttempts >::iterator it;
			it = _locationFailMap.insert(make_pair(physConn->location, _ConnectAttempts())).first;
			if (it != _locationFailMap.end())
			{
				time_t cur = plib_time(0);
				if( ( 0 == it->second.size() ) || ( it->second[it->second.size()-1].time <= cur ) )
					it->second.push_back( _ConnectAttemptItem(cur) );
				PTRACE4("CommRoutingTable::physicallyDisconnected - failure for location %s added", (*it).first); 
			}
		}
	}
	_physicallyDisconnected( physConn_, _PhysConn::_StatePhysConnDisc );
}


void CommRoutingTable::physicalReconnectBegin( _CommPhysicalConnection* physConn )
{
	_physicallyDisconnected( physConn, _PhysConn::_StatePhysConnReconn );
}

void CommRoutingTable::physicalReconnectEnd( _CommPhysicalConnection* physConn, int thread, int slot, UINT32 channelId )
{
	_physicallyConnected( physConn, thread, slot, channelId, -1 /* leave as is */ );
}

CommRoutingTable::_Conn* CommRoutingTable::_getSrvConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType )
{
	_Conn** connPtr = srvConnMap.find( thisId );
	if( connPtr == 0 )
	{
		PTRACE4( "CommRoutingTable: msg type=%02X for unknown srvConnId=%08X - ignored", msgType, thisId );
		return 0;
	}
	_Conn* conn = *connPtr;
	PASSERT3( conn->srvThisId == thisId );
	if( conn->srv != srcPhysConn || conn->muxSrv != srcMux )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvConnId=%08X from invalid source '%s'/%d - ignored", msgType, thisId, srcPhysConn->address.c_str(), srcMux );
		return 0;
	}
	return conn;
}

CommRoutingTable::_Conn* CommRoutingTable::_getSrvConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, msgType );
	if( conn == 0 )
		return 0;
	if( peerId != conn->srvId )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvConnId=%08X with msg.peerId=%08X != conn.srvId=%08X - ignored", msgType, thisId, peerId, conn->srvId );
		return 0;
	}
	return conn;
}

CommRoutingTable::_Conn* CommRoutingTable::_getSrvConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType )
{
	_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msgType );
	if( conn == 0 )
		return 0;
	if( sessionId != conn->sessionId )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvConnId=%08X with msg.sessionId=%08X != conn.sessionId=%08X - ignored", msgType, thisId, sessionId, conn->sessionId );
		return 0;
	}
	return conn;
}

CommRoutingTable::_Conn* CommRoutingTable::_getCliConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType )
{
	_Conn** connPtr = cliConnMap.find( thisId );
	if( connPtr == 0 )
	{
		PLog( "CommRoutingTable: msg type=%02X for unknown cliConnId=%08X - ignored", msgType, thisId );
		return 0;
	}
	_Conn* conn = *connPtr;
	PASSERT3( conn->cliThisId == thisId );
	if( conn->cli != srcPhysConn || conn->muxCli != srcMux )
	{
		PLog( "CommRoutingTable: msg type=%02X for connId=%08X from invalid source '%s'/%d - ignored", msgType, thisId, srcPhysConn->address.c_str(), srcMux );
		return 0;
	}
	return conn;
}

CommRoutingTable::_Conn* CommRoutingTable::_getCliConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, msgType );
	if( conn == 0 )
		return 0;
	if( peerId != conn->cliId )
	{
		PLog( "CommRoutingTable: msg type=%02X for cliConnId=%08X with msg.peerId=%08X != conn.cliId=%08X - ignored", msgType, thisId, peerId, conn->cliId );
		return 0;
	}

	if (conn->inPreConnectedList)
	{//received thisId on this connection for the first time - need to remove conn from preConn list
		_CliPreConnMap::iterator found = conn->cli->cliPreConns.find(_CliPreConn(conn->cliId,conn->muxCli));
		if ( found == conn->cli->cliPreConns.end())
		{
			PLog( "CommRoutingTable: connection not found in pre-connection map: cliId=%08X, muxCli=%08X (%d)", conn->cliId ,conn->muxCli,conn->cli->cliPreConns.size());
		}
		else
		{
			conn->cli->cliPreConns.erase( found );
			PTRACE5("CommRoutingTable:_getCliConn cliId=%08X, muxCli=%08X, cliThisId=%08X erased from pre-connection map (%d)",conn->cliId,conn->muxCli, conn->cliThisId,conn->cli->cliPreConns.size());
		}
		conn->inPreConnectedList = false;
	}

	return conn;
}

CommRoutingTable::_Conn* CommRoutingTable::_getCliConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType )
{
	_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msgType );
	if( conn == 0 )
		return 0;
	if( sessionId != conn->sessionId )
	{
		PLog( "CommRoutingTable: msg type=%02X for cliConnId=%08X with msg.sessionId=%08X != conn.sessionId=%08X - ignored", msgType, thisId, sessionId, conn->sessionId );
		return 0;
	}
	return conn;
}

CommRoutingTable::_Subscr* CommRoutingTable::_getSrvSubscr( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType )
{
	_Subscr** subscrPtr = srvSubscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommRoutingTable: msg type=%02X for unknown srvSubscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	_Subscr* subscr = *subscrPtr;
	PASSERT3( subscr->srvThisId == thisId );
	if( subscr->srv != srcPhysConn || subscr->muxSrv != srcMux )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X from invalid source '%s'/%d - ignored", msgType, thisId, srcPhysConn->address.c_str(), srcMux );
		return 0;
	}
	return subscr;
}

CommRoutingTable::_Subscr* CommRoutingTable::_getSrvSubscr( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, msgType );
	if( subscr == 0 )
		return 0;
	if( peerId != subscr->srvId )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X with msg.peerId=%08X != subscr.srvId=%08X - ignored", msgType, thisId, peerId, subscr->srvId );
		return 0;
	}
	return subscr;
}

CommRoutingTable::_SubscrLight* CommRoutingTable::_getSrvSubscrLight( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType )
{
	_SubscrLight** subscrPtr = srvSubscrLightMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommRoutingTable: msg type=%02X for unknown srvSubscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	_SubscrLight* subscr = *subscrPtr;
	PASSERT3( subscr->srvThisId == thisId );
	if( subscr->srv != srcPhysConn || subscr->muxSrv != srcMux )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X from invalid source '%s'/%d - ignored", msgType, thisId, srcPhysConn->address.c_str(), srcMux );
		return 0;
	}
	return subscr;
}

CommRoutingTable::_SubscrLight* CommRoutingTable::_getSrvSubscrLight( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType )
{
	_SubscrLight* subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, msgType );
	if( subscr == 0 )
		return 0;
	if( peerId != subscr->srvId )
	{
		PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X with msg.peerId=%08X != subscr.srvId=%08X - ignored", msgType, thisId, peerId, subscr->srvId );
		return 0;
	}
	return subscr;
}

CommRoutingTable::_SubscrLeaf* CommRoutingTable::_getCliSubscrLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType )
{
	_SubscrLeaf** subscrPtr = cliSubscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommRoutingTable: msg type=%02X for unknown cliSubscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	_SubscrLeaf* subscrLeaf = *subscrPtr;
	PASSERT3( subscrLeaf->cliThisId == thisId );
	if( subscrLeaf->cli != srcPhysConn || subscrLeaf->muxCli != srcMux )
	{
		PLog( "CommRoutingTable: msg type=%02X for subscrId=%08X from invalid source '%s'/%d - ignored", msgType, thisId, srcPhysConn->address.c_str(), srcMux );
		return 0;
	}
	return subscrLeaf;
}

CommRoutingTable::_SubscrLeaf* CommRoutingTable::_getCliSubscrLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType )
{

	_SubscrLeaf* subscrLeaf = _getCliSubscrLeaf( srcMux, srcPhysConn, thisId, msgType );
	if( subscrLeaf == 0 )
		return 0;
	if( peerId != subscrLeaf->cliId )
	{
		PLog( "CommRoutingTable: msg type=%02X for cliSubscrId=%08X with msg.peerId=%08X != subscr.cliId=%08X - ignored", msgType, thisId, peerId, subscrLeaf->cliId );
		return 0;
	}
	return subscrLeaf;
}


CommRoutingTable::_SubscrLightLeaf* CommRoutingTable::_getCliSubscrLightLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType )
{
	_SubscrLightLeaf** subscrPtr = cliSubscrLightMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "CommRoutingTable: msg type=%02X for unknown cliSubscrId=%08X - ignored", msgType, thisId );
		return 0;
	}
	_SubscrLightLeaf* subscrLeaf = *subscrPtr;
	PASSERT3( subscrLeaf->cliThisId == thisId );
	if( subscrLeaf->cli != srcPhysConn || subscrLeaf->muxCli != srcMux )
	{
		PLog( "CommRoutingTable: msg type=%02X for subscrId=%08X from invalid source '%s'/%d - ignored", msgType, thisId, srcPhysConn->address.c_str(), srcMux );
		return 0;
	}
	return subscrLeaf;
}

CommRoutingTable::_SubscrLightLeaf* CommRoutingTable::_getCliSubscrLightLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType )
{

	_SubscrLightLeaf* subscrLeaf = _getCliSubscrLightLeaf( srcMux, srcPhysConn, thisId, msgType );
	if( subscrLeaf == 0 )
		return 0;
	if( peerId != subscrLeaf->cliId )
	{
		PLog( "CommRoutingTable: msg type=%02X for cliSubscrId=%08X with msg.peerId=%08X != subscr.cliId=%08X - ignored", msgType, thisId, peerId, subscrLeaf->cliId );
		return 0;
	}
	return subscrLeaf;
}

CommRoutingTable::_Conn* CommRoutingTable::_createNewConn( const char* server, const char* serverObject, _PhysConn* cliPhysConn, int muxCli, UINT32 muxCliChannelId, UINT32 cliId, CommMsgBody& requestBody, _PhysConn* srvPhysConn, int muxSrv, UINT32 muxSrvChannelId, const char * serverConnType, UINT32 version, UINT32 serverId, BYTE priority )
{
	_Conn* newConn = new _Conn();
	newConn->server = server;
	newConn->serverObject = serverObject;
	newConn->serverChannel = serverConnType;
	newConn->priority = priority;
	newConn->state = _CommConnection::_StateConnDisc;
	newConn->cliThisId = cliConnMap.add( newConn );
	newConn->srvThisId = 0;//invalid value
	newConn->cliId = cliId;
	newConn->srvId = 0;//invalid value
	newConn->sessionId = 0;//invalid value
	newConn->request.moveFrom( requestBody );
	newConn->cli = cliPhysConn;
	newConn->muxCli = muxCli;
	newConn->muxCliChannelId = muxCliChannelId;
	newConn->srv = srvPhysConn;
	newConn->muxSrv = muxSrv;
	newConn->muxSrvChannelId = muxSrvChannelId;
	newConn->wantQualityUpdates = false;
	newConn->wantIsAlive = false;
	newConn->version = version;
	newConn->serverId=serverId; //invalid value
#ifdef COMMLIB_CALCULATE_STATISTICS
	newConn->trafficCntPos = _maxConnTrafficCounter ? getCounterForTrafficStatistics(_connTrafficStat,server, serverObject,serverConnType) : -1;
#endif
	srvPhysConn->cliConns.insert( newConn );
	cliPhysConn->srvConns.insert( newConn );
	cliPhysConn->cliPreConns.insert(_CliPreConnMap::value_type(_CliPreConn(cliId,muxCli),newConn));
	newConn->inPreConnectedList = true;
	PTRACE5("CommRoutingTable:_createNewConn cliId=%08X, muxCli=%08X, cliThisId=%08X added to pre-connection map (%d)",newConn->cliId,newConn->muxCli, newConn->cliThisId,newConn->cli->cliPreConns.size());
	return newConn;
}

void CommRoutingTable::_delConn( _Conn* conn )
{
	if( conn->srv )
	{
		_ConnSetByCli::iterator found = conn->srv->cliConns.find( conn );
		PASSERT3( found != conn->srv->cliConns.end() );
		conn->srv->cliConns.erase( found );
	}

	{
		_ConnSetBySrv::iterator found = conn->cli->srvConns.find( conn );
		PASSERT3( found != conn->cli->srvConns.end() );
		conn->cli->srvConns.erase( found );
	}

	if (conn->inPreConnectedList)
	{
		_CliPreConnMap::iterator found = conn->cli->cliPreConns.find(_CliPreConn(conn->cliId,conn->muxCli));
		if ( found == conn->cli->cliPreConns.end())
		{
			PLog( "CommRoutingTable:_delConn - connection not found in pre-connection map: cliId=%08X, muxCli=%08X (%d)", conn->cliId ,conn->muxCli,conn->cli->cliPreConns.size());
		}
		else
		{
			conn->cli->cliPreConns.erase( found );
			PTRACE5("CommRoutingTable:_delConn cliId=%08X, muxCli=%08X, cliThisId=%08X erased from pre-connection map (%d)",conn->cliId,conn->muxCli, conn->cliThisId, conn->cli->cliPreConns.size());
		}
		conn->inPreConnectedList = false;
	}

	if( conn->srvThisId != 0 )
		srvConnMap.remove( conn->srvThisId );
	cliConnMap.remove( conn->cliThisId );
	delete conn;
}

CommRoutingTable::_Subscr* CommRoutingTable::_createNewSubscr( const char* server, const char* serverObject, const char* serverChannel, _PhysConn* srvPhysConn, int muxSrv, UINT32 muxSrvChannelId )
{
	_Subscr* newSubscr = new _Subscr();
	newSubscr->server = _dup( server );
	newSubscr->serverObject = _dup( serverObject );
	newSubscr->serverChannel = _dup( serverChannel );
	newSubscr->priority = _COMM_MSGPRIORITY_INITSUBSCR;
	newSubscr->state = CommSubscription::_StateSubscrDisc;
	newSubscr->srvThisId = 0;//invalid value
	newSubscr->srvId = 0;//invalid value
	newSubscr->srv = srvPhysConn;
	newSubscr->muxSrv = muxSrv;
	newSubscr->muxSrvChannelId = muxSrvChannelId;
	newSubscr->_subscrUniqueId = ++_subscrUniqueId;
	newSubscr->frozen = false;
	newSubscr->frozenCounter=0;
	newSubscr->needCounting = false;
	newSubscr->protocolVersion = sendNewSubscriptionRequests ? 1 : 0;
	if (acceptNewSubscriptionRequests)
		newSubscr->setPercentOfDelItemsToStore(percentOfDelItemsToStore);
	newSubscr->deleteSubscrTime=0;
	newSubscr->timerWasPost=false;
#ifdef COMMLIB_CALCULATE_STATISTICS
	newSubscr->trafficCntPos = _maxSubscrTrafficCounter ? getCounterForTrafficStatistics(_subscrTrafficStat,server, serverObject,serverChannel) : -1;
#endif
	nameSubscrMap.insert( newSubscr );

	srvPhysConn->cliSubscrs.insert( newSubscr );
	return newSubscr;
}

CommRoutingTable::_SubscrLight* CommRoutingTable::_createNewSubscrLight( const char* server, const char* serverObject, const char* serverChannel, _PhysConn* srvPhysConn, int muxSrv, UINT32 muxSrvChannelId )
{
	_SubscrLight* newSubscr = new _SubscrLight();
	newSubscr->server = _dup( server );
	newSubscr->serverObject = _dup( serverObject );
	newSubscr->serverChannel = _dup( serverChannel );
	newSubscr->priority = _COMM_MSGPRIORITY_INITSUBSCR;
	newSubscr->state = CommSubscription::_StateSubscrDisc;
	newSubscr->srvThisId = 0;//invalid value
	newSubscr->srvId = 0;//invalid value
	newSubscr->srv = srvPhysConn;
	newSubscr->muxSrv = muxSrv;
	newSubscr->muxSrvChannelId = muxSrvChannelId;
	newSubscr->_subscrUniqueId = ++_subscrUniqueId;
	newSubscr->needCounting = false;
#ifdef COMMLIB_CALCULATE_STATISTICS
	newSubscr->trafficCntPos = _maxSubscrTrafficCounter ? getCounterForTrafficStatistics(_subscrTrafficStat,server, serverObject,serverChannel) : -1;
#endif
	nameSubscrLightMap.insert( newSubscr );
	srvPhysConn->cliSubscrsLight.insert( newSubscr );
	return newSubscr;
}

CommRoutingTable::_SubscrLeaf* CommRoutingTable::_createNewSubscrLeaf( _Subscr* subscr, _PhysConn* cliPhysConn, int muxCli, UINT32 muxCliChannelId, UINT32 cliId, CommMsgBody& requestBody, int protocolVersion )
{
	_SubscrLeaf* newSubscrLeaf = new _SubscrLeaf();
	newSubscrLeaf->subscr = subscr;
	newSubscrLeaf->cliThisId = cliSubscrMap.add( newSubscrLeaf );
	newSubscrLeaf->cliId = cliId;
	newSubscrLeaf->cli = cliPhysConn;
	newSubscrLeaf->muxCli = muxCli;
	newSubscrLeaf->muxCliChannelId= muxCliChannelId;
	newSubscrLeaf->sync = false;
	newSubscrLeaf->frozen = _SubscrLeaf::_frozenStateNormal;
	newSubscrLeaf->count=1;
	newSubscrLeaf->request.moveFrom( requestBody );
#ifdef COMMLIB_CALCULATE_STATISTICS
	if (newSubscrLeaf->subscr->trafficCntPos>=0)
	{
		PInterlockedExchangeAdd(&_subscrTrafficStatCounters[2][newSubscrLeaf->subscr->trafficCntPos],newSubscrLeaf->request._size());
		PInterlockedIncrement(&_subscrTrafficStatPacketCounters[2][newSubscrLeaf->subscr->trafficCntPos]);
		if (printTrafficLast)
		{
			if (newSubscrLeaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
				PLog("Traffic subscr from client: %s/%s/%s", newSubscrLeaf->subscr->server,newSubscrLeaf->subscr->serverObject,newSubscrLeaf->subscr->serverChannel);
		}
	}
#endif

	newSubscrLeaf->protocolVersion=protocolVersion;
	subscr->cli.insert( newSubscrLeaf );
	cliPhysConn->srvSubscrs.insert( newSubscrLeaf );
	if (PString::startsWith(subscr->serverObject,"table."))
	{
		cliPhysConn->subscrCounter++;
		cliPhysConn->lastSubscrTime=SYSTEMTICKS();
	}
	return newSubscrLeaf;
}

CommRoutingTable::_SubscrLightLeaf* CommRoutingTable::_createNewSubscrLightLeaf( _SubscrLight* subscr, _PhysConn* cliPhysConn, int muxCli, UINT32 muxCliChannelId, UINT32 cliId)
{
	_SubscrLightLeaf* newSubscrLeaf = new _SubscrLightLeaf();
	newSubscrLeaf->subscr = subscr;
	newSubscrLeaf->cliThisId = cliSubscrLightMap.add( newSubscrLeaf );
	newSubscrLeaf->cliId = cliId;
	newSubscrLeaf->cli = cliPhysConn;
	newSubscrLeaf->muxCli = muxCli;
	newSubscrLeaf->muxCliChannelId= muxCliChannelId;
	newSubscrLeaf->count=1;
#ifdef COMMLIB_CALCULATE_STATISTICS
	if (newSubscrLeaf->subscr->trafficCntPos>=0)
	{
		PInterlockedIncrement(&_subscrTrafficStatPacketCounters[2][newSubscrLeaf->subscr->trafficCntPos]);
		if (printTrafficLast)
		{
			if (newSubscrLeaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
				PLog("Traffic subscr from client: %s/%s/%s", newSubscrLeaf->subscr->server,newSubscrLeaf->subscr->serverObject,newSubscrLeaf->subscr->serverChannel);
		}
	}
#endif

	subscr->cli.insert( newSubscrLeaf );
	cliPhysConn->srvSubscrsLight.insert( newSubscrLeaf );
	return newSubscrLeaf;
}

void CommRoutingTable::_delSubscrLeaf( _SubscrLeaf* subscrLeaf )
{
	_Subscr* subscr = static_cast<_Subscr*>(subscrLeaf->subscr);
	_SubscrLeafSetByCli::iterator found = subscr->cli.find( subscrLeaf );
	PASSERT3( found != subscr->cli.end() );
	subscr->cli.erase( found );
	if (subscrLeaf->frozen == _SubscrLeaf::_frozenStateFrozen)
	{
		subscr->frozenCounter--;
		PASSERT3(subscr->frozenCounter>=0);
	}

	{
		_SubscrLeafSet::iterator found = subscrLeaf->cli->srvSubscrs.find( subscrLeaf );
		PASSERT3( found != subscrLeaf->cli->srvSubscrs.end() );
		subscrLeaf->cli->srvSubscrs.erase( found );
	}

	cliSubscrMap.remove( subscrLeaf->cliThisId );
	delete subscrLeaf;
}

void CommRoutingTable::_delSubscrLightLeaf( _SubscrLightLeaf* subscrLeaf )
{
	_SubscrLight* subscr = subscrLeaf->subscr;
	_SubscrLeafSetByCli::iterator found = subscr->cli.find( subscrLeaf );
	PASSERT3( found != subscr->cli.end() );
	subscr->cli.erase( found );

	{
		_SubscrLightLeafSet::iterator found = subscrLeaf->cli->srvSubscrsLight.find( subscrLeaf );
		PASSERT3( found != subscrLeaf->cli->srvSubscrsLight.end() );
		subscrLeaf->cli->srvSubscrsLight.erase( found );
	}

	cliSubscrLightMap.remove( subscrLeaf->cliThisId );
	delete subscrLeaf;
}

void CommRoutingTable::_freezeSubscrLeaf( _SubscrLeaf* subscrLeaf )
{
	PASSERT3(subscrLeaf->frozen != _SubscrLeaf::_frozenStateFrozen);
	_Subscr* subscr = static_cast<_Subscr*>(subscrLeaf->subscr);
	subscrLeaf->frozen = _SubscrLeaf::_frozenStateFrozen;
	subscrLeaf->sync = false;
	subscr->frozenCounter++;
}

void CommRoutingTable::_defrostSubscrLeaf( _SubscrLeaf* subscrLeaf, CommMsgBody& requestBody )
{
	PASSERT3(subscrLeaf->frozen == _SubscrLeaf::_frozenStateFrozen);
	_Subscr* subscr = static_cast<_Subscr*>(subscrLeaf->subscr);
	PTRACE4( "CommRoutingTable: _defrostSubscrLeaf cliId=%08X, thisId=%08X peerId=%08X",subscrLeaf->cliId,subscr->srvThisId,subscr->srvId);
	subscr->frozenCounter--;
	PASSERT3(subscr->frozenCounter>=0);
	if (subscr->frozen == false)
	{
		// generate RESYNC in reply.
		PASSERT3(subscrLeaf->sync == false);
		if( subscr->state == CommSubscription::_StateSubscrSync )
		{
			subscrLeaf->frozen = _SubscrLeaf::_frozenStateNormal;
			PTRACE4( "CommRoutingTable: processing subscription defrost for '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
			_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_RESYNC, subscr->priority );
			m.body.composeUINT32( subscrLeaf->cliId ).composeUINT32( subscrLeaf->cliThisId );
			CommMsgParser rqParser( requestBody );
			subscr->processSyncRequest( m.body, rqParser );
			_postToPhysConn( subscrLeaf->cli, subscrLeaf->muxCli, subscrLeaf->muxCliChannelId, m );
			subscrLeaf->sync = true;
		}
		else
		{
			PTRACE4( "CommRoutingTable: processing subscription defrost/delayed for '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
			subscrLeaf->frozen = _SubscrLeaf::_frozenStateResync;
			subscrLeaf->request._dealloc();
			subscrLeaf->request.moveFrom( requestBody );
		}
	}
	else
	{
		PTRACE4( "CommRoutingTable: processing subscription defrost/resubscribe for '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
		subscr->frozen = false;
		subscrLeaf->frozen = _SubscrLeaf::_frozenStateResync;
		subscrLeaf->request._dealloc();
		subscrLeaf->request.moveFrom( requestBody );
		_resubscribe(subscr);
	}
}

void CommRoutingTable::_freezeSubscr( _Subscr* subscr )
{
	PTRACE4( "CommRoutingTable: freezeSubscr for '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
	if( subscr->srv )
	{
		_SubscrSet::iterator found = subscr->srv->cliSubscrs.find( subscr );
		PASSERT3( found != subscr->srv->cliSubscrs.end() );
		subscr->srv->cliSubscrs.erase( found );
		subscr->srv=0;
	}
	if( subscr->srvThisId != 0 )
	{
		srvSubscrMap.remove( subscr->srvThisId );
		subscr->srvThisId = 0;
	}
	subscr->state = CommSubscription::_StateSubscrDisc;
	subscr->frozen = true;
}

void CommRoutingTable::_delSubscr( _Subscr* subscr )
{
	{
		_SubscrSetByName::iterator found = nameSubscrMap.find( subscr );
		PASSERT3( found != nameSubscrMap.end() );
		nameSubscrMap.erase( found );
	}

	{
		int n = static_cast< int >( subscr->cli.size() );
		if( n )
		{
			PArrayAutoPtr< _SubscrLeaf* > leaves( new _SubscrLeaf*[ n ] );//although it seems safe to call _delSubscrLeaf within subscr->cli iteration, but it changes subscr->cli, so we will move it out of the loop - just in case
			int leafI = 0;
			for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
			{
				_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
				PASSERT3( leafI < n );
				leaves[ leafI++ ] = leaf;
			}
			PASSERT3( leafI == n );
			for( int i=0; i < n ; ++i )
				_delSubscrLeaf( leaves[ i ] );
		}
	}

	if( subscr->srv )
	{
		_SubscrSet::iterator found = subscr->srv->cliSubscrs.find( subscr );
		PASSERT3( found != subscr->srv->cliSubscrs.end() );
		subscr->srv->cliSubscrs.erase( found );
	}

	if( subscr->srvThisId != 0 )
		srvSubscrMap.remove( subscr->srvThisId );
	_free( subscr->serverChannel );
	_free( subscr->serverObject );
	_free( subscr->server );
	delete subscr;
}

void CommRoutingTable::_delSubscrLight( _SubscrLight* subscr )
{
	{
		_SubscrLightSetByName::iterator found = nameSubscrLightMap.find( subscr );
		PASSERT3( found != nameSubscrLightMap.end() );
		nameSubscrLightMap.erase( found );
	}

	{
		int n = static_cast< int >( subscr->cli.size() );
		if( n )
		{
			PArrayAutoPtr< _SubscrLightLeaf* > leaves( new _SubscrLightLeaf*[ n ] );//although it seems safe to call _delSubscrLeaf within subscr->cli iteration, but it changes subscr->cli, so we will move it out of the loop - just in case
			int leafI = 0;
			for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
			{
				_SubscrLightLeaf* leaf = *iter;
				PASSERT3( leafI < n );
				leaves[ leafI++ ] = leaf;
			}
			PASSERT3( leafI == n );
			for( int i=0; i < n ; ++i )
				_delSubscrLightLeaf( leaves[ i ] );
		}
	}

	if( subscr->srv )
	{
		_SubscrLightSet::iterator found = subscr->srv->cliSubscrsLight.find( subscr );
		PASSERT3( found != subscr->srv->cliSubscrsLight.end() );
		subscr->srv->cliSubscrsLight.erase( found );
	}

	if( subscr->srvThisId != 0 )
		srvSubscrLightMap.remove( subscr->srvThisId );
	_free( subscr->serverChannel );
	_free( subscr->serverObject );
	_free( subscr->server );
	delete subscr;
}

CommRoutingTable::_Conn* CommRoutingTable::_lookForCliConn( UINT32 cliId, _PhysConn* srcPhysConn, int srcMux )
{
	_CliPreConnMap::iterator it = srcPhysConn->cliPreConns.find(_CliPreConn(cliId,srcMux));
	if (it != srcPhysConn->cliPreConns.end())
		return (*it).second;
	else 
	{//fallback to old algorithm - just in case, shoould never happen
		//		int i;
		//		for( _Conn** c = cliConnMap.firstIndex( i ); c ; c = cliConnMap.nextIndex( i ) )
		//			if( (*c)->cliId == cliId && (*c)->cli == srcPhysConn && (*c)->muxCli == srcMux )
		//				{
		//				PTRACE3("CommRoutingTable:_lookForCliConn cliId=%08X, muxCli=%08X, cliThisId=%08X found in global list not in pre-connection map (%d)",(*c)->cliId,(*c)->muxCli, (*c)->cliThisId, (*c)->cli->cliPreConns.size());
		//				return *c;
		//				}
		return 0;
	}
}

CommRoutingTable::_SubscrLeaf* CommRoutingTable::_lookForCliSubscrLeaf( UINT32 cliId, _PhysConn* srcPhysConn, int srcMux )
{
	int i;
	for(_SubscrLeaf** l = cliSubscrMap.firstIndex( i ); l ; l = cliSubscrMap.nextIndex( i ) )
		if( (*l)->cliId == cliId && (*l)->cli == srcPhysConn && (*l)->muxCli == srcMux )
			return *l;
	return 0;
}

CommRoutingTable::_SubscrLightLeaf* CommRoutingTable::_lookForCliSubscrLightLeaf( UINT32 cliId, _PhysConn* srcPhysConn, int srcMux )
{
	int i;
	for(_SubscrLightLeaf** l = cliSubscrLightMap.firstIndex( i ); l ; l = cliSubscrLightMap.nextIndex( i ) )
		if( (*l)->cliId == cliId && (*l)->cli == srcPhysConn && (*l)->muxCli == srcMux )
			return *l;
	return 0;
}

bool CommRoutingTable::_isAlias( const char* server, const char* serverObject, const char* serverChannel, const char * inManager, PString* newObject, bool forSubscr) const
{
	if( forSubscr == false )
	{
		_RuleParms dummyParms;
		for( _RuleTable::const_iterator iter = _config.notAliases.begin(); iter != _config.notAliases.end(); ++iter )
		{
			if( _ruleMatch( dummyParms, *iter, server, false, serverObject, serverChannel, inManager ) )
				return false;
		}
	}

	_RuleParms ruleParms;
	for( _RuleTable::const_iterator iter = _config.aliases.begin(); iter != _config.aliases.end(); ++iter )
	{
		const CommRule& aliasRule = *iter;
		if( _ruleMatch( ruleParms, aliasRule, server, forSubscr, serverObject, serverChannel, inManager ) )
		{
			if( newObject )
			{
				if( forSubscr == false && aliasRule.conns.size() > 0 )
					_ruleMake( *newObject, ruleParms, *aliasRule.conns.begin() );
				else
					*newObject = serverObject;
			}
			return true;
		}
	}
	return false;
}

bool CommRoutingTable::_isAliasServer( const char* server ) const
{
	_RuleParms dummyParms;
	for( _RuleTable::const_iterator iter = _config.aliases.begin(); iter != _config.aliases.end(); ++iter )
	{
		if( _ruleMatch( dummyParms, *iter, server, true, 0, 0, 0 ) )
			return true;
	}
	return false;
}

CommRoutingTable::_PhysConn* CommRoutingTable::_findLocalPhysConn( int& mux, UINT32& muxChannelId, const char* serverObject, UINT16& errCode, const char*& errMsg )
{
	_PhysConnMuxStringMap::iterator found = localConns.find( serverObject );
	if( found == localConns.end() )
	{
		errCode = _COMM_ERROR_NOLOCALOBJECT;
		errMsg = "No local object";
		return 0;
	}
	else
	{
		PTRACE4( "CommRoutingTable: localPhysConn found" );
		_PhysConn* srvPhysConn = (*found).second.physConn;
		PASSERT3( srvPhysConn->inbound );

		mux = (*found).second.mux;
		muxChannelId = (*found).second.muxChannelId;
		return srvPhysConn;
	}
}

CommRoutingTable::_PhysConn* CommRoutingTable::_findOrCreateRemotePhysConn( int& mux, UINT32& muxChannelId, const char* server, const char* serverObject, const char * serverChannel, const char * inManager, UINT16& errCode, const char*& errMsg )
{
	bool failedConns = false;
	int lastErrorCode = 0;
	_RuleParms ruleParms;
	for( _RuleTable::const_iterator iter = _config.rules.begin(); iter != _config.rules.end() ; ++iter )
	{
		const CommRule& rule = *iter;
		if( _ruleMatch( ruleParms, rule, server, false, serverObject, serverChannel, inManager ) )
		{
			_PhysConn* srvPhysConn = 0;
			vector< BalanceListItem > balanceList;
			for( CommRule::_List::const_iterator jter = rule.conns.begin(); jter != rule.conns.end() ; ++jter )
			{
				PString connName;
				_ruleMake( connName, ruleParms, *jter );
				_PhysConnStringMap::iterator found = outboundConns.find( connName );
				if( found == outboundConns.end() )
				{
					PTRACE4( "CommRoutingTable: trying to create remotePhysConn %s", connName.c_str() );
					CommThreadManager* mgr = 0;
					for( _ManagerList::iterator mter = managers.begin() ; mter != managers.end() ; ++mter )
						if( (*mter)->handles( connName ) )
						{
							mgr = *mter;
							break;
						}

					if( mgr != 0 )
					{
						_PhysConn* newConn = new _PhysConn( connName, mgr, false );
						newConn->state = _PhysConn::_StatePhysConnDisc;
						newConn->quality = -1;
						newConn->qualityRequired = false;
						newConn->connFailures = 0;
						newConn->lastErrorCode = 0;
						newConn->firstFailureTime = 0;
						newConn->failureTimeStatus = 0;
						newConn->location = (*jter).location;
						newConn->ddosProtectionAttempts = 0;
						// these is necessary for reconnecting in the case if there is no logical connections/subscriptions
						// It happens in "force client diconnect" scenario
						// Shouldn't never be used in other cases
						// One more thing - it is useful for dumping, it makes clear what logical connection/subscription was the reason of
						// creation of this connection. 
						{
							newConn->pseudoConnection.server = server;
							newConn->pseudoConnection.serverObject = serverObject;
							newConn->pseudoConnection.serverChannel = serverChannel;
						}
						found = outboundConns.insert( _PhysConnStringMap::value_type( connName, newConn ) ).first;
						PTRACE4( "CommRoutingTable: remotePhysConn created  %s", connName.c_str() );
					}
					else
					{
						PLog( "Unable to find ThreadManager to handle '%s' address", connName.c_str() );
					}
				}
				else
				{
					_PhysConn* c = (*found).second;
					if (c->mgr->getConnectAttemptsDdosProtection() > 0 && c->ddosProtectionAttempts > 0)
					{
						; //do nothing
					}
					else
					{
						PTRACE4("CommRoutingTable: remotePhysConn found %s", c->address.c_str());
						if (optMaxPhysConnFailures > 0 && c->connFailures >= optMaxPhysConnFailures)
						{
							PTRACE4("CommRoutingTable: remotePhysConn has too many failures - ignored, lastErrorCode = %d", c->lastErrorCode);
							found = outboundConns.end();
							failedConns = true;
							lastErrorCode = c->lastErrorCode;
						}
						else if (optPhysConnFailuresTimeout > 0 && c->failureTimeStatus < 0)
						{
							PTRACE4("CommRoutingTable: remotePhysConn with exceeded failures timeout - ignored, lastErrorCode = %d", c->lastErrorCode);
							found = outboundConns.end();
							failedConns = true;
							lastErrorCode = c->lastErrorCode;
						}
					}
				}

				if( found /* already */ != outboundConns.end() )
				{
					_PhysConn* conn2 = (*found).second;
					if (conn2->mgr->getConnectAttemptsDdosProtection() > 0)
					{
						if (conn2->ddosProtectionAttempts > 0)
						{
							srvPhysConn = conn2;
							break;
						}
					}

					if (conn2->state != _PhysConn::_StatePhysConnDisc)
					{
						PTRACE4( "CommRoutingTable: existing remotePhysConn found %s, state=%d", conn2->address.c_str(),conn2->state );
						srvPhysConn = conn2;
						break;
					}
					balanceList.push_back(BalanceListItem(conn2,(*jter).weight));
				}
			}

			if (srvPhysConn == 0)
			{
				int n = static_cast< int >( balanceList.size() );
				if (n == 0)
					break;
				if (n == 1)
					srvPhysConn = balanceList.begin()->conn;
				else
				{
					srvPhysConn = _balancePhysConn(balanceList);
					if (!srvPhysConn)
					{
						failedConns = true;
						break;
					}
				}
			}

			if( srvPhysConn != 0 )
			{
				PASSERT3( !srvPhysConn->inbound );
				mux = 0;//at least currently no multiplexing over remote connectinons
				muxChannelId = 0;
				return srvPhysConn;
			}
			else
				break;//forever
		}//if( ruleMatch )
	}//for( iter )

	if( failedConns )
	{
		if (lastErrorCode == _SOCKET_SSLCERTIFICATEFAILED)
			errCode = _COMM_ERROR_SSLCERTIFICATEFAILED;
		else
			errCode = _COMM_ERROR_PHYSICALMULTIPLEFAILURES;
		errMsg = "Multiple failures trying to establish physical connection";
	}
	else
	{
		errCode = _COMM_ERROR_NOROUTETOHOST;
		errMsg = "No route to host";
	}
	return 0;
}

CommRoutingTable::_PhysConn* CommRoutingTable::_findOrCreatePhysConn( int& mux, UINT32& muxChannelId, const char* server, const char* serverObject, const char * serverChannel, const char * inManager, bool forSubscr, UINT16& errCode, const char*& errMsg )
{
	PString newObject;
	bool isLocal = _isAlias( server, serverObject, serverChannel, inManager, &newObject, forSubscr);
	if( isLocal )
		return _findLocalPhysConn( mux, muxChannelId, newObject, errCode, errMsg );
	else
		return _findOrCreateRemotePhysConn( mux, muxChannelId, server, serverObject, serverChannel,inManager, errCode, errMsg );
}

void CommRoutingTable::_postToSubscrLeafs( _SubscrLight* subscr, BYTE msgType, CommMsgBody& body )
{
	body._setRefCounted();
	for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
	{
		_SubscrLightLeaf* leaf = *iter;
		_CommMsg m( msgType, subscr->priority, body, CommMsgBody::_RefCountFlag() );
		m.internal.flags |= _CommMsg::Patch8;
		m.internal.patch0 = leaf->cliId;
		m.internal.patch1 = leaf->cliThisId;
		_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
	}
}

void CommRoutingTable::_startCountingRead( _SubscrLight* subscr, bool lightType, CommMsgBody& body )
{
	if (!subscr->needCounting)
	{
		subscr->needCounting = true;
		body._setRefCounted();
		for( _SubscrLeafSetByCli::const_iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
		{
			const _SubscrLightLeaf* leaf = *iter;
			if (leaf->cli->isLocal !=0 )
			{
				_CommMsg m( (lightType ? _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST:_COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST), subscr->priority, body, CommMsgBody::_RefCountFlag() );
				m.internal.flags |= _CommMsg::Patch8;
				m.internal.patch0 = leaf->cliId;
				m.internal.patch1 = leaf->cliThisId;
				_postToPhysConnRead( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
			}
		}
		CommMsgBody timerMsg;
		timerMsg.composeUINT32( subscr->srvThisId ).composeUINT32( subscr->srvId ).composeUINT32( subscr->_subscrUniqueId );
		_CommMsg msg( (lightType ? _TIMER_SUBSCR_LIGHT_COUNTING:_TIMER_SUBSCR_COUNTING), 0, timerMsg );
		timerThread->push( msg, optTimeoutSubscrCounting );
	}
}

void CommRoutingTable::_stopCountingRead( _SubscrLight* subscr, bool lightType, CommMsgBody& body )
{
	if (subscr->needCounting)
	{
		subscr->needCounting = false;
		body._setRefCounted();
		for( _SubscrLeafSetByCli::const_iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
		{
			const _SubscrLightLeaf* leaf = *iter;
			if (leaf->cli->isLocal !=0 )
			{
				_CommMsg m( (lightType?_COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST:_COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST), subscr->priority, body, CommMsgBody::_RefCountFlag() );
				m.internal.flags |= _CommMsg::Patch8;
				m.internal.patch0 = leaf->cliId;
				m.internal.patch1 = leaf->cliThisId;
				_postToPhysConnRead( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
			}
		}
	}
}

void CommRoutingTable::_postCountingUpdateRead( const _SubscrLight* subscr, bool light )
{
	CommMsgBody body;
	UINT32 count = 0;
	body.composeUINT32( subscr->srvId ).composeUINT32( subscr->srvThisId );

	for( _SubscrLeafSetByCli::const_iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
	{
		const _SubscrLightLeaf* leaf = *iter;
		count += leaf->count;
	}
	body.composeUINT32(count);
	_CommMsg m( (light? _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE : _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE), subscr->priority, body );
	_postToPhysConnRead( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
}

void CommRoutingTable::_conditionalPostToSubscrLeafs( _Subscr* subscr, BYTE msgType, CommMsgBody& body, UINT32 * ticksCounter )
{
	if (ticksCounter)
		ticksCounter[0] = SYSTEMTICKS();

#ifdef COMMLIB_CALCULATE_STATISTICS
	if (subscr->trafficCntPos>=0)
	{
		PInterlockedExchangeAdd(&_subscrTrafficStatCounters[1][subscr->trafficCntPos],body._size() * subscr->cli.size());
		PInterlockedExchangeAdd(&_subscrTrafficStatPacketCounters[1][subscr->trafficCntPos],subscr->cli.size());
		if (printTrafficLast)
		{
			if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
				PLog("Traffic subscr update: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
		}
	}
#endif

	body._setRefCounted();
	for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
	{
		_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
		if (leaf->frozen == _SubscrLeaf::_frozenStateNormal)
		{
			_CommMsg m( msgType, subscr->priority, body, CommMsgBody::_RefCountFlag() );
			m.internal.flags |= _CommMsg::Patch8;
			m.internal.patch0 = leaf->cliId;
			m.internal.patch1 = leaf->cliThisId;
			if (ticksCounter)
			{
				UINT32 tmp[8];
				memset(tmp,0,sizeof(tmp));
				tmp[0] = SYSTEMTICKS();
				_postToPhysConnRead( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m, tmp + 1);
				tmp[7] = SYSTEMTICKS();
				for (int i=1; i<8 ;i++)
				{
					if (tmp[i])
					{
						ticksCounter[i] += tmp[i] - tmp[i-1];
					}
					else
						break;
				}
			}
			else
				_postToPhysConnRead( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
		}
	}
	if (ticksCounter)
		ticksCounter[0] = SYSTEMTICKS() - ticksCounter[0];

}

void CommRoutingTable::_conditionalPostToSubscrLightLeafs( _SubscrLight* subscr, BYTE msgType, CommMsgBody& body)
{
#ifdef COMMLIB_CALCULATE_STATISTICS
	if (subscr->trafficCntPos>=0)
	{
		PInterlockedExchangeAdd(&_subscrTrafficStatCounters[1][subscr->trafficCntPos],body._size() * subscr->cli.size());
		PInterlockedExchangeAdd(&_subscrTrafficStatPacketCounters[1][subscr->trafficCntPos],subscr->cli.size());
		if (printTrafficLast)
		{
			if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
				PLog("Traffic subscr update: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
		}
	}
#endif

	body._setRefCounted();
	for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
	{
		_SubscrLightLeaf* leaf = *iter;
		_CommMsg m( msgType, subscr->priority, body, CommMsgBody::_RefCountFlag() );
		m.internal.flags |= _CommMsg::Patch8;
		m.internal.patch0 = leaf->cliId;
		m.internal.patch1 = leaf->cliThisId;
		_postToPhysConnRead( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
	}
}


void CommRoutingTable::_processSubscrRequest( _SubscrLeaf* leaf )
{
	if (leaf->frozen == _SubscrLeaf::_frozenStateResync)
	{
		// Special case - subscription response after defrost.
		// Answer with RESYNC instead of SYNC_RESPONSE
		PASSERT3(leaf->sync == false);
		leaf->frozen = _SubscrLeaf::_frozenStateNormal;
		PTRACE4( "CommRoutingTable: processing subscription request after defrost for '%s'/'%s'/'%s'", leaf->subscr->server, leaf->subscr->serverObject, leaf->subscr->serverChannel );
		_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_RESYNC, leaf->subscr->priority );
		m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId );
		CommMsgParser rqParser( leaf->request );
		static_cast<_Subscr*>(leaf->subscr)->processSyncRequest( m.body, rqParser );
#ifdef COMMLIB_CALCULATE_STATISTICS
		if (leaf->subscr->trafficCntPos>=0)
		{
			PInterlockedExchangeAdd(&_subscrTrafficStatCounters[0][leaf->subscr->trafficCntPos],m.body._size());
			PInterlockedIncrement(&_subscrTrafficStatPacketCounters[0][leaf->subscr->trafficCntPos]);
			if (printTrafficLast)
			{
				if (leaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
					PLog("Traffic subscr: %s/%s/%s", leaf->subscr->server,leaf->subscr->serverObject,leaf->subscr->serverChannel);
			}
		}
#endif
		_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
		leaf->sync = true;
	}
	else
	{
		PTRACE4( "CommRoutingTable: processing subscription request for '%s'/'%s'/'%s'", leaf->subscr->server, leaf->subscr->serverObject, leaf->subscr->serverChannel );
		_CommMsg m( (leaf->protocolVersion == 0 ? _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE:_COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE1), leaf->subscr->priority );
		m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeBYTE( leaf->subscr->priority );
		CommMsgParser rqParser( leaf->request );
		if (leaf->protocolVersion == 0)
			static_cast<_Subscr*>(leaf->subscr)->processSyncRequest( m.body, rqParser );//recovers as much as makes sense, otherwise throws
		else
			static_cast<_Subscr*>(leaf->subscr)->processSyncRequestExPhase1( m.body, rqParser );//recovers as much as makes sense, otherwise throws
#ifdef COMMLIB_CALCULATE_STATISTICS
		if (leaf->subscr->trafficCntPos>=0)
		{
			PInterlockedExchangeAdd(&_subscrTrafficStatCounters[0][leaf->subscr->trafficCntPos],m.body._size());
			PInterlockedIncrement(&_subscrTrafficStatPacketCounters[0][leaf->subscr->trafficCntPos]);
			if (printTrafficLast)
			{
				if (leaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
					PLog("Traffic subscr: %s/%s/%s", leaf->subscr->server,leaf->subscr->serverObject,leaf->subscr->serverChannel);
			}
		}
#endif
		_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
		leaf->sync = leaf->frozen == _SubscrLeaf::_frozenStateNormal ? true : false;// never synchronize frozen leaf!!! 
		// Client level is ready to handle this message in frozen
		// state, moreover, it uses this message to set priority of subscription.
		// Sync should be always false in frozen state. Next update will not go
		// to the client, so I consider client and routing table not synchronized.
	}
}

void CommRoutingTable::_processSubscrLightRequest( _SubscrLightLeaf* leaf )
{
	PTRACE4( "CommRoutingTable: processing light subscription request for '%s'/'%s'/'%s'", leaf->subscr->server, leaf->subscr->serverObject, leaf->subscr->serverChannel );
	_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE, leaf->subscr->priority );
	m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeBYTE( leaf->subscr->priority );
#ifdef COMMLIB_CALCULATE_STATISTICS
	if (leaf->subscr->trafficCntPos>=0)
	{
		PInterlockedExchangeAdd(&_subscrTrafficStatCounters[0][leaf->subscr->trafficCntPos],m.body._size());
		PInterlockedIncrement(&_subscrTrafficStatPacketCounters[0][leaf->subscr->trafficCntPos]);
		if (printTrafficLast)
		{
			if (leaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
				PLog("Traffic subscr: %s/%s/%s", leaf->subscr->server,leaf->subscr->serverObject,leaf->subscr->serverChannel);
		}
	}
#endif
	_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
}

bool CommRoutingTable::localNameExists( const char* name )
{
	PWLock lock( cs );
	return localConns.find( name ) != localConns.end();
}

void CommRoutingTable::_forceClientsDisconnect(_PhysConn * physConn)
{
	if( physConn->mgr->isForceClientDisconnects() && physConn->inbound == false)
	{
		PTRACE3( "CommRoutingTable::_forceClientsDisconnect: %s",physConn->address.c_str());
		set <_PhysConn *> physConnSet;
		for( _ConnSetByCli::iterator iter = physConn->cliConns.begin() ; iter != physConn->cliConns.end() ; ++iter )
		{
			_Conn* conn = *iter;
			PASSERT3( conn->srv == physConn );
			if (conn->cli->mgr->isAllowedClientDisconnects())
			{
				physConnSet.insert(conn->cli);
			}
		}
		for( _SubscrSet::iterator siter = physConn->cliSubscrs.begin() ; siter != physConn->cliSubscrs.end() ; ++siter )
		{
			_Subscr* subscr = static_cast<_Subscr*>(*siter);
			PASSERT3( subscr->srv == physConn );
			for( _SubscrLeafSetByCli::iterator jter = subscr->cli.begin(); jter != subscr->cli.end() ; ++jter )
			{
				_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*jter);
				if (leaf->cli->mgr->isAllowedClientDisconnects())
				{
					physConnSet.insert(leaf->cli);
				}
			}
		}
		for( _SubscrLightSet::iterator siter = physConn->cliSubscrsLight.begin() ; siter != physConn->cliSubscrsLight.end() ; ++siter )
		{
			_SubscrLight* subscr = *siter;
			PASSERT3( subscr->srv == physConn );
			for( _SubscrLeafSetByCli::iterator jter = subscr->cli.begin(); jter != subscr->cli.end() ; ++jter )
			{
				_SubscrLightLeaf* leaf = *jter;
				if (leaf->cli->mgr->isAllowedClientDisconnects())
				{
					physConnSet.insert(leaf->cli);
				}
			}
		}

		for( _ManagerList::iterator miter = managers.begin() ; miter != managers.end() ; ++miter )
		{
			if ((*miter)->isAllowedClientDisconnects() && 
				(*miter)->isInStateOfClosedAccept() == false)
			{
				(*miter)->acceptCancel();
			}
		}
		set <_PhysConn *>::iterator phiter;
		for( phiter = physConnSet.begin() ; phiter != physConnSet.end() ; ++phiter )
		{
			_PhysConn *ph = *phiter;
			_forceDisconnect(ph);

		}
	}
}




void CommRoutingTable::routeIncomingMessage( _CommPhysicalConnection* srcPhysConn_, int srcMux, UINT32 srcMuxChannelId, _CommMsg& msg )
{
	_PhysConn* srcPhysConn = (_PhysConn*)srcPhysConn_;//safe because _CommPhysicalConnection is always created by CommRoutingTable as _PhysConn

	if( shuttingDown )
		return;

	UINT32 t0 = SYSTEMTICKS();
	UINT32 t1 = 0;
	UINT32 t1_1 = 0;

	try
	{
		switch( msg.type )
		{

		case _COMM_MSGTYPE_REGISTER_SERVEROBJECT:
			{
				if( srcPhysConn->isLocal <= 1 )
				{
					PLog( "CommRoutingTable: _COMM_MSGTYPE_REGISTER_SERVEROBJECT from non-local physical connection '%s'", srcPhysConn->address.c_str() );
					break;
				}

				CommMsgParser parser( msg.body );
				UINT32 reqId;
				const char* name;
				parser.parseUINT32( reqId ).parseString( name );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_REGISTER_SERVEROBJECT: name='%s'", name );

				LOCK_BRACKET_ON

					_PhysConnSet::iterator found = inboundConns.find( srcPhysConn );
				if( found != inboundConns.end() )//can be == for muxed inbound conns
					inboundConns.erase( found );
				BYTE err = 0;
				//if( localConns.find( name ) != localConns.end() )
				if( localNameExists( name ) )
				{
					PLog( "CommRoutingTable: _COMM_MSGTYPE_REGISTER_SERVEROBJECT: duplicate name='%s'", name );
					err = 1;
				}
				else
				{
					_PhysConnMux localConn;
					localConn.physConn = srcPhysConn;
					localConn.mux = srcMux;
					localConn.muxChannelId = srcMuxChannelId;
					localConns.insert( _PhysConnMuxStringMap::value_type( name, localConn ) );
					PTRACE3( "CommRoutingTable: server object '%s' registered", name );
				}

				_CommMsg m( _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY, msg.priority );
				m.body.composeUINT32( reqId ).composeBYTE( err );
				_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );

				LOCK_BRACKET_OFF

					break;
			}

		case _COMM_MSGTYPE_UNREGISTER_SERVEROBJECT:
			{
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_UNREGISTER_SERVEROBJECT: mux=%d", srcMux );
				if( srcPhysConn->isLocal <= 1 )
				{
					PLog( "CommRoutingTable: _COMM_MSGTYPE_UNREGISTER_SERVEROBJECT from non-local physical connection '%s'", srcPhysConn->address.c_str() );
					break;
				}

				LOCK_BRACKET_ON

					_cleanupPhysConnMux( srcPhysConn, srcMux );

				LOCK_BRACKET_OFF
			}
			break;

		case _COMM_MSGTYPE_CONNECT_REQUEST:
			{
				CommMsgParser parser( msg.body );
				CommMsgBody newBody;
				CommMsgBody tmpBody;
				UINT32 peerId;
				PString  server;
				PString  serverObject;
				PString  serverConnType;
				parser.parseUINT32( peerId ).parseStringP( server ).parseStringP( serverObject ).parseStringP( serverConnType );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST: server='%s' serverObject='%s' serverConnType='%s' from '%s'", server.c_str(), serverObject.c_str(), serverConnType.c_str(), srcPhysConn->address.c_str() );

				UINT16 errCode = 0;
				const char* errMsg = 0;

				//skip guard list
				for(;;)
				{
					const char* guardName;
					parser.parseString( guardName );
					tmpBody.composeString( guardName );
					if( *guardName == 0 )
						break;
				}
				//find end of physical address list
				vector <const char *> ips;
				for( int i=0 ;; ++i )
				{
					if( i >= _COMM_MAXHOPS )
					{
						errCode = _COMM_ERROR_ROUTINGLOOP;
						errMsg = "Maximum hops exceeded";
						break;//forever
					}

					const char* physAddr;
					parser.parseString( physAddr );
					if( *physAddr == 0 )
						break;

					if( strcmp( physAddr, srcPhysConn->address ) == 0 )
					{
						errCode = _COMM_ERROR_ROUTINGLOOP;
						errMsg = "Loop found";
						break;//forever
					}
					tmpBody.composeString( physAddr );
					ips.push_back( physAddr );
				}
				if( srcPhysConn->isLocal < 3 )
					tmpBody.composeString( srcPhysConn->address );
				tmpBody.composeString( "" );
				ips.push_back( srcPhysConn->address );

				UINT32 version = 0;
				UINT32 serverId = 0;
				PString newObject;
				bool isLocal = false;

				PRLock cfgLock( configLock );

				if (errCode == 0)
				{
					PString checkRestrictionsFailReason;
					if (!checkRestrictionsConnection(server,serverObject,serverConnType,ips, srcPhysConn->mgr->getType(), &checkRestrictionsFailReason))
					{
						errCode = _COMM_ERROR_NOROUTETOHOST;
						errMsg = "No allowed route to host";
						PTRACE3("CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST: server='%s' serverObject='%s' serverConnType='%s' from '%s' rejected: %s", server.c_str(), serverObject.c_str(), serverConnType.c_str(), srcPhysConn->address.c_str(), checkRestrictionsFailReason.c_str());
					}
					else
					{
						newBody.composeUINT32( peerId ).composeString( server ).composeString( serverObject ).composeString( serverConnType );
						newBody.merge(tmpBody);
						if (msg.flags & _COMM_MSGFLAGS_CONNECTION_EX)
						{
							UINT32 clientId;
							version=1;
							parser.parseUINT32(clientId).parseUINT32(serverId);
							newBody.composeUINT32(clientId);
							if (msg.flags & _COMM_MSGFLAGS_CONNECTION_EX_RECONNECT)
								PASSERT3(serverId != 0);
						}
						isLocal = _isAlias( server, serverObject, serverConnType, srcPhysConn->mgr->getType(), &newObject, false );
					}
				}

				LOCK_BRACKET_ON
					if( errCode == 0 )
					{
						int muxSrv;
						UINT32 muxSrvChannelId;
						_PhysConn* srvPhysConn;
						if( isLocal )
							srvPhysConn = _findLocalPhysConn( muxSrv, muxSrvChannelId, newObject, errCode, errMsg );
						else
							srvPhysConn = _findOrCreateRemotePhysConn( muxSrv, muxSrvChannelId, server, serverObject, serverConnType, srcPhysConn->mgr->getType(), errCode, errMsg );
						if( srvPhysConn )
						{
							PASSERT5( errCode == 0 );
							_Conn* newConn = _createNewConn( server, serverObject, srcPhysConn, srcMux, srcMuxChannelId, peerId, newBody, srvPhysConn, muxSrv, muxSrvChannelId, serverConnType, version, serverId, msg.priority );
							PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST ('%s'/'%s'/'%s') accepted: cliThisId=%08X", server.c_str(), serverObject.c_str(), serverConnType.c_str(), newConn->cliThisId );
							_postConnAccept( newConn );
							switch( srvPhysConn->state )
							{
							case _PhysConn::_StatePhysConnOk:
								_postConnRequest( newConn );//sets _StateConnRequestSent
								break;
							case _PhysConn::_StatePhysConnDisc:
								srvPhysConn->mgr->connect( srvPhysConn );
								srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
								break;
							case _PhysConn::_StatePhysConnRequestSent:
							case _PhysConn::_StatePhysConnReconn:
								//do nothing
								break;
							default:
								PASSERT3( 0 );
								break;
							}
						}
					}

					if( errCode != 0 )
					{
						PLog( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST ('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverConnType.c_str(), errMsg );
						_CommMsg m( _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR, _COMM_MSGPRIORITY_INITCONNECT );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
						_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
					}

					LOCK_BRACKET_OFF
			}
			break;

		case _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED: thisId=%08X peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnRequestSent )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						conn->srvId = peerId;
						conn->state = _CommConnection::_StateConnPeerAssigned;
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

			/*
			case _COMM_MSGTYPE_CONNECT_REQUEST_PENDING:
			{
			CommMsgParser parser( msg.body );
			UINT32 thisId, peerId;
			const char* failedAddress;
			parser.parseUINT32( thisId ).parseUINT32( peerId ).parseString( failedAddress );
			PTRACE3( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST_PENDING: thisId=%08X peerId=%08X failedAddress='%s'", thisId, peerId, failedAddress );

			_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
			if( conn )
			{
			if( conn->state != _CommConnection::_StateConnPeerAssigned )
			{
			PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_PENDING msg for connId=%08X in state=%d - ignored", thisId, conn->state );
			}
			else
			{
			_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
			_postToPhysConn( conn->cli, msg );
			}
			}
			}
			break;
			*/

		case _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				BYTE priority;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED: thisId=%08X peerId=%08X priority=%d", thisId, peerId, priority );

				LOCK_BRACKET_ON

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, msg.type );
				if( conn )
				{
					if( conn->state == _CommConnection::_StateConnRequestSent )
					{
						conn->srvId = peerId;
						conn->state = _CommConnection::_StateConnPeerAssigned;
					}
					if( peerId != conn->srvId )
					{
						PLog( "CommRoutingTable: msg type=%02X for srvConnId=%08X with msg.peerId=%08X != conn.srvId=%08X - ignored", msg.type, thisId, peerId, conn->srvId );
						break;
					}
					if( conn->state != _CommConnection::_StateConnPeerAssigned )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommRoutingTable: invalid priority=%d in _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
						}
						else
						{
							if (conn->priority < priority && conn->cli->isLocal == 0)
							{
								conn->priority = priority;
								_postConnAccept( conn );
							}
							else
							{
								conn->priority = priority;
							}
						}
						_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
						_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );

						conn->state = _CommConnection::_StateConnGuardNegotiationNoSession;
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_GUARD_REQUEST:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

				LOCK_BRACKET_ON

					_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnGuardNegotiationNoSession )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->srv != 0 );//not in this conn->state
						conn->sessionId = sessionId;
						_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
						_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );

						conn->state = _CommConnection::_StateConnGuardNegotiation;
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

				LOCK_BRACKET_ON

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnGuardNegotiation )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
						_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

				LOCK_BRACKET_ON

					_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnGuardNegotiation )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->srv != 0 );//not in this conn->state
						_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
						_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				BYTE priority;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId ).parseBYTE( priority );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED: thisId=%08X peerId=%08X sessionId=%08X priority=%d", thisId, peerId, sessionId, priority );

				LOCK_BRACKET_ON
					_Conn* conn = 0;
				if (msg.flags & _COMM_MSGFLAGS_CONNECTION_EX_RECONNECT)
				{
					// reconnect
					conn = _getSrvConn( srcMux, srcPhysConn, thisId, msg.type );
					if (conn)
					{
						if (conn->version != 1)
						{
							PLog( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X , incorrect version of connection - ignored", thisId );
						}
						else
						{
							UINT32 clientId;
							UINT32 serverId;
							parser.parseUINT32(serverId).parseUINT32(clientId);
							conn->serverId = serverId;
							if( conn->state == _CommConnection::_StateConnRequestSent )
							{
								conn->srvId = peerId;
								conn->state = _CommConnection::_StateConnPeerAssigned;
							}
							if( peerId != conn->srvId )
							{
								PLog( "CommRoutingTable: msg type=%02X for srvConnId=%08X with msg.peerId=%08X != conn.srvId=%08X - ignored", msg.type, thisId, peerId, conn->srvId );
							}
							else
							{
								if( conn->state != _CommConnection::_StateConnPeerAssigned )
								{
									PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
								}
								else
								{
									if( priority >= _COMM_MSGPRIORITIES )
									{
										PLog( "CommRoutingTable: invalid priority=%d in _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
									}
									else
									{
										if (conn->priority < priority && conn->cli->isLocal == 0)
										{
											conn->priority = priority;
											_postConnAccept( conn );
										}
										else
										{
											conn->priority = priority;
										}
									}
									conn->sessionId = sessionId;
									_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
									_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
									conn->state = _CommConnection::_StateConnOk;
								}
							}
						}
					}

				}
				else
				{
					conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
					if( conn )
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommRoutingTable: invalid priority=%d in _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED for connId=%08X - ignored", priority, thisId );
						}
						else
							conn->priority = priority;

						if( conn->state != _CommConnection::_StateConnGuardNegotiation )
						{
							PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED msg for connId=%08X in state=%d - ignored", thisId, conn->state );
						}
						else
						{
							if (msg.flags & _COMM_MSGFLAGS_CONNECTION_EX)
							{
								if (conn->version != 1)
								{
									PLog( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED msg for connId=%08X , incorrect version of connection - ignored", thisId );
									break;
								}
								UINT32 clientId;
								UINT32 serverId;
								parser.parseUINT32(serverId).parseUINT32(clientId);
								conn->serverId = serverId;
							}
							_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
							_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );

							conn->state = _CommConnection::_StateConnOk;
						}
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				//no trace
				UINT32 tickCounterMsg[PERFORMANCE_COUNTERS_INONEMSG_SIZE];
				if (doCounting)
				{
					memset(tickCounterMsg, 0, sizeof(tickCounterMsg));
					tickCounterMsg[0]=SYSTEMTICKS();
				}
				LOCK_BRACKET_ON_READ
					if (doCounting)
						tickCounterMsg[1]=SYSTEMTICKS();
				_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
				if (doCounting)
					tickCounterMsg[2]=SYSTEMTICKS();
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->srv != 0 );//not in this conn->state
						_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
						if (doCounting)
							tickCounterMsg[3]=SYSTEMTICKS();
#ifdef COMMLIB_CALCULATE_STATISTICS
						if (conn->trafficCntPos>=0)
						{
							PInterlockedExchangeAdd(&_connTrafficStatCounters[0][conn->trafficCntPos],msg.body._size());
							PInterlockedIncrement(&_connTrafficStatPacketCounters[0][conn->trafficCntPos]);
							if (printTrafficLast)
							{
								if (conn->trafficCntPos == _maxConnTrafficCounter - 1)
									PLog("Traffic connection from client: %s/%s/%s", conn->server.c_str(),conn->serverObject.c_str(),conn->serverChannel.c_str());
							}
						}
#endif
						if (doCounting)
							_postToPhysConnRead( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg, tickCounterMsg+4 );
						else
							_postToPhysConnRead( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg);
					}
				}
				if (doCounting)
				{
					tickCounterMsg[10]=SYSTEMTICKS();
					UINT32 t1_2=SYSTEMTICKS();
					for (int i=0;i<sizeof(tickCounterMsg)/sizeof(tickCounterMsg[0]);++i)
					{
						if (tickCounterMsg[i])
						{
							PInterlockedExchangeAdd(&tickCounterInOneMsg[i],t1_2 - tickCounterMsg[i]);
						}
					}
				}

				LOCK_BRACKET_OFF_READ

			}
			break;

		case _COMM_MSGTYPE_CONNECT_SERVER_USERMSG:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				//no trace

				LOCK_BRACKET_ON_READ

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_SERVER_USERMSG msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
#ifdef COMMLIB_CALCULATE_STATISTICS
						if (conn->trafficCntPos>=0)
						{
							PInterlockedExchangeAdd(&_connTrafficStatCounters[1][conn->trafficCntPos],msg.body._size());
							PInterlockedIncrement(&_connTrafficStatPacketCounters[1][conn->trafficCntPos]);
							if (printTrafficLast)
							{
								if (conn->trafficCntPos == _maxConnTrafficCounter - 1)
									PLog("Traffic connection to client: %s/%s/%s", conn->server.c_str(),conn->serverObject.c_str(),conn->serverChannel.c_str());
							}
						}
#endif
						_postToPhysConnRead( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
					}
				}

				LOCK_BRACKET_OFF_READ

			}
			break;

		case _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

				LOCK_BRACKET_ON

					_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->srv != 0 );//not in this conn->state
						_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
						_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );

						_delConn( conn );
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId, sessionId;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32( sessionId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT: thisId=%08X peerId=%08X sessionId=%08X", thisId, peerId, sessionId );

				LOCK_BRACKET_ON

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, sessionId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
						_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );

						_delConn( conn );
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_CLIENT_ERROR:
		case _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_CLIENT_{ERROR/TRANSITDISCONNECT}(%02X): thisId=%08X peerId=%08X", msg.type, thisId, peerId );

				LOCK_BRACKET_ON

					_Conn* conn = 0;
				if( thisId )
					conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					conn = _lookForCliConn( peerId, srcPhysConn, srcMux );
					if( !conn )
					{
						PLog( "Unable to find conn for %d type with cliId=%08X and physConn='%s'", msg.type, peerId, srcPhysConn->address.c_str() );
					}
				}

				if( conn )
				{
					_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
					if( conn->srv )
						_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );

					_delConn( conn );
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_Conn* conn = 0;
				if( thisId )
					conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					conn = _lookForCliConn( peerId, srcPhysConn, srcMux );
					if( !conn )
					{
						PLog( "Unable to find conn for %d type with cliId=%08X and physConn='%s'", msg.type, peerId, srcPhysConn->address.c_str() );
					}
				}

				if( conn )
				{
					_patchMsg8( msg, conn->srvId, conn->srvThisId, _COMM_MSGPRIORITY_HIGHPRIORITY_DISCONNECT );
					if( conn->srv )
						_postToPhysConnRead( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );
				}
				LOCK_BRACKET_OFF_READ
			}
			break;

		case _COMM_MSGTYPE_CONNECT_SERVER_ERROR:
		case _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR:
		case _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_SERVER_{ERROR/FATALERROR/TRANSITDISCONNECT}(%02X): thisId=%08X peerId=%08X", msg.type, thisId, peerId );

				LOCK_BRACKET_ON

					_Conn* conn = 0;
				if( peerId )
					conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
					conn = _getSrvConn( srcMux, srcPhysConn, thisId, msg.type );
				if( conn )
				{
					_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
					_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );

					if( msg.type == _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT )
						conn->state = _CommConnection::_StateConnPeerAssigned;
					else
						_delConn( conn );
				}

				LOCK_BRACKET_OFF

			}
			break;
		case _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT: thisId=%08X peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_Conn* conn = 0;
				if( peerId )
					conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
					conn = _getSrvConn( srcMux, srcPhysConn, thisId, msg.type );
				if( conn )
				{
					_patchMsg8( msg, conn->cliId, conn->cliThisId, _COMM_MSGPRIORITY_HIGHPRIORITY_DISCONNECT );
					_postToPhysConnRead( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
				}

				LOCK_BRACKET_OFF_READ

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST:
			{
				CommMsgParser parser( msg.body );
				UINT32 peerId;
				PString server;
				PString serverObject;
				PString serverChannel;
				parser.parseUINT32( peerId ).parseStringP( server ).parseStringP( serverObject ).parseStringP( serverChannel );
				msg.body._shift( parser._shift() );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST : server='%s' serverObject='%s' serverChannel='%s' from '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), srcPhysConn->address.c_str() );

				UINT16 errCode = 0;
				const char* errMsg = 0;
				bool isLocal = false;
				vector <const char * > ips;
				ips.push_back(srcPhysConn->address);

				PString checkRestrictionsFailReason;
				PRLock cfgLock( configLock );
				if (!checkRestrictionsSubscription(server,serverObject,serverChannel,ips, &checkRestrictionsFailReason))
				{
					errCode = _COMM_ERROR_NOROUTETOHOST;
					errMsg = "No allowed route to host";
					PTRACE3("CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST: server='%s' serverObject='%s' serverChannel='%s' from '%s' rejected: %s", server.c_str(), serverObject.c_str(), serverChannel.c_str(), srcPhysConn->address.c_str(), checkRestrictionsFailReason.c_str());
				}
				else
				{
					isLocal = _isAliasServer( server );
					if( isLocal )
						server = "";//special value for local server
				}

				LOCK_BRACKET_ON
					if (errCode == 0)
					{
						_Subscr* subscr = nameSubscrMap.findByName( server, serverObject, serverChannel );
						try
						{
							if( subscr )
							{
								PTRACE4( "CommRoutingTable: found subscription '%s'/'%s'/'%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );
							}
							else
							{
								_PhysConn* srvPhysConn = 0;
								int muxSrv = 0;
								UINT32 muxSrvChannelId = 0;

								if( isLocal )
									srvPhysConn = _findLocalPhysConn( muxSrv, muxSrvChannelId, serverObject, errCode, errMsg );
								else
									srvPhysConn = _findOrCreateRemotePhysConn( muxSrv, muxSrvChannelId, server, serverObject, serverChannel, 0, errCode, errMsg );
								if( srvPhysConn )
								{
									PTRACE4( "CommRoutingTable: creating new subscription '%s'/'%s'/'%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );
									subscr = _createNewSubscr( server, serverObject, serverChannel, srvPhysConn, muxSrv, muxSrvChannelId );
									switch( srvPhysConn->state )
									{
									case _PhysConn::_StatePhysConnOk:
										_postSubscrRequest( subscr );//sets _StateSubscrRequestSent
										break;
									case _PhysConn::_StatePhysConnDisc:
										srvPhysConn->mgr->connect( srvPhysConn );
										srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
										break;
									case _PhysConn::_StatePhysConnRequestSent:
									case _PhysConn::_StatePhysConnReconn:
										//do nothing
										break;
									default:
										PASSERT3( 0 );
										break;
									}
								}
								else
								{
									PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), errMsg );
									_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
									m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
									_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
								}
							}
							if( subscr )
							{
								_SubscrLeaf* leaf = _createNewSubscrLeaf( subscr, srcPhysConn, srcMux, srcMuxChannelId, peerId, msg.body, 0 );
								PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST('%s'/'%s'/'%s') accepted: leaf.cliThisId=%08X", server.c_str(), serverObject.c_str(), serverChannel.c_str(), leaf->cliThisId );
								_postSubscrAccept( leaf,false );

								if (subscr->needCounting)
								{
									if (leaf->cli->isLocal !=0 )
									{
										CommMsgBody body;
										body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeBYTE(1);
										_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST, subscr->priority, body );
										_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
									}
								}

								if( subscr->state == CommSubscription::_StateSubscrSync )
								{
									_processSubscrRequest( leaf );
								}
								else if (subscr->frozen)
								{
									PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST('%s'/'%s'/'%s'): defrosing subscription'", subscr->server, subscr->serverObject, subscr->serverChannel );
									subscr->frozen = false;
									_resubscribe(subscr);
								}
							}
						}
						catch( PError& err )
						{
							PLog( "Exception in _COMM_MSGTYPE_SUBSCRIPTION_REQUEST '%s' err='%s'\n"
								"server='%s', serverObject='%s', serverChannel='%s'"
								"subscription %s ", srcPhysConn->address.c_str(), err.why(), server.c_str(), serverObject.c_str(), serverChannel.c_str(), (subscr ? "found" : "not found") );
							throw;
						}
					}
					else
					{
						PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), errMsg );
						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
						_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
					}
				LOCK_BRACKET_OFF
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1:
			{
				CommMsgParser parser( msg.body );
				UINT32 peerId;
				PString server;
				PString serverObject;
				PString serverChannel;
				parser.parseUINT32( peerId ).parseStringP( server ).parseStringP( serverObject ).parseStringP( serverChannel );
				msg.body._shift( parser._shift() );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1: server='%s' serverObject='%s' serverChannel='%s from '%s''", server.c_str(), serverObject.c_str(), serverChannel.c_str(), srcPhysConn->address.c_str() );

				UINT16 errCode = 0;
				const char* errMsg = 0;
				bool isLocal = false;
				vector <const char * > ips;
				ips.push_back(srcPhysConn->address);

				PString checkRestrictionsFailReason;
				PRLock cfgLock( configLock );
				if (!checkRestrictionsSubscription(server,serverObject,serverChannel,ips, &checkRestrictionsFailReason))
				{
					errCode = _COMM_ERROR_NOROUTETOHOST;
					errMsg = "No allowed route to host";
					PTRACE3("CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1: server='%s' serverObject='%s' serverChannel='%s' from '%s' rejected: %s", server.c_str(), serverObject.c_str(), serverChannel.c_str(), srcPhysConn->address.c_str(), checkRestrictionsFailReason.c_str());
				}
				else
				{
					isLocal = _isAliasServer( server );
					if( isLocal )
						server = "";//special value for local server
				}

				LOCK_BRACKET_ON
					if (errCode == 0)
					{
						_Subscr* subscr = nameSubscrMap.findByName( server, serverObject, serverChannel );
						try
						{
							if (acceptNewSubscriptionRequests == false)
							{
								PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1('%s'/'%s'/'%s') failed: 'protocol isn't supported'", server.c_str(), serverObject.c_str(), serverChannel.c_str());
								_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
								m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( _COMM_ERROR_PROTOCOLNOTSUPPORTED ).composeString( "Protocol not supported" );
								_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
							}
							else
							{
								if( subscr )
								{
									PTRACE4( "CommRoutingTable: found subscription '%s'/'%s'/'%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );
								}
								else
								{
									_PhysConn* srvPhysConn = 0;
									int muxSrv = 0;
									UINT32 muxSrvChannelId = 0;
									if( isLocal )
										srvPhysConn = _findLocalPhysConn( muxSrv, muxSrvChannelId, serverObject, errCode, errMsg );
									else
										srvPhysConn = _findOrCreateRemotePhysConn( muxSrv, muxSrvChannelId, server, serverObject, serverChannel, 0, errCode, errMsg );
									if( srvPhysConn )
									{
										PTRACE4( "CommRoutingTable: creating new subscription '%s'/'%s'/'%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );
										subscr = _createNewSubscr( server, serverObject, serverChannel, srvPhysConn, muxSrv, muxSrvChannelId );
										switch( srvPhysConn->state )
										{
										case _PhysConn::_StatePhysConnOk:
											_postSubscrRequest( subscr );//sets _StateSubscrRequestSent
											break;
										case _PhysConn::_StatePhysConnDisc:
											srvPhysConn->mgr->connect( srvPhysConn );
											srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
											break;
										case _PhysConn::_StatePhysConnRequestSent:
										case _PhysConn::_StatePhysConnReconn:
											//do nothing
											break;
										default:
											PASSERT3( 0 );
											break;
										}
									}
									else
									{
										PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), errMsg );
										_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
										m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
										_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
									}
								}
								if( subscr )
								{
									_SubscrLeaf* leaf = _createNewSubscrLeaf( subscr, srcPhysConn, srcMux, srcMuxChannelId, peerId, msg.body, 1 );
									PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1('%s'/'%s'/'%s') accepted: leaf.cliThisId=%08X", server.c_str(), serverObject.c_str(), serverChannel.c_str(), leaf->cliThisId );
									_postSubscrAccept( leaf,false );
									if (subscr->needCounting)
									{
										if (leaf->cli->isLocal !=0 )
										{
											CommMsgBody body;
											body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeBYTE(1);
											_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST, subscr->priority, body );
											_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
										}
									}
									if( subscr->state == CommSubscription::_StateSubscrSync )
									{
										_processSubscrRequest( leaf );
									}
								}
							}
						}
						catch( PError& err )
						{
							PLog( "Exception in _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1 '%s' err='%s' "
								"server='%s', serverObject='%s', serverChannel='%s'"
								"subscription %s", srcPhysConn->address.c_str(), err.why(), server.c_str(), serverObject.c_str(), serverChannel.c_str(), (subscr ? "found": "not found") );
							throw;
						}
					}
					else
					{
						PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), errMsg );
						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
						_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
					}

					LOCK_BRACKET_OFF
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					if( subscr->state != CommSubscription::_StateSubscrRequestSent )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						subscr->srvId = peerId;
						subscr->state = CommSubscription::_StateSubscrPeerAssigned;
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE2:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				msg.body._shift( parser._shift() );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE2: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON
					_SubscrLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					leaf = _lookForCliSubscrLeaf( peerId, srcPhysConn, srcMux );
					if( !leaf )
					{
						PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
					}
				}

				if( leaf )
				{
					_CommMsg m(_COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE2, leaf->subscr->priority );
					m.body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeBYTE( leaf->subscr->priority );
					leaf->request.moveFrom( msg.body );
#ifdef COMMLIB_CALCULATE_STATISTICS
					if (leaf->subscr->trafficCntPos>=0)
					{
						PInterlockedExchangeAdd(&_subscrTrafficStatCounters[2][leaf->subscr->trafficCntPos],leaf->request._size());
						PInterlockedIncrement(&_subscrTrafficStatPacketCounters[2][leaf->subscr->trafficCntPos]);
						if (printTrafficLast)
						{
							if (leaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
								PLog("Traffic subscr from client: %s/%s/%s", leaf->subscr->server,leaf->subscr->serverObject,leaf->subscr->serverChannel);
						}
					}
#endif
					CommMsgParser rqParser( leaf->request );
					static_cast<_Subscr*>(leaf->subscr)->processSyncRequestExPhase2( m.body, rqParser );//recovers as much as makes sense, otherwise throws
#ifdef COMMLIB_CALCULATE_STATISTICS
					if (leaf->subscr->trafficCntPos>=0)
					{
						PInterlockedExchangeAdd(&_subscrTrafficStatCounters[0][leaf->subscr->trafficCntPos],m.body._size());
						PInterlockedIncrement(&_subscrTrafficStatPacketCounters[0][leaf->subscr->trafficCntPos]);
						if (printTrafficLast)
						{
							if (leaf->subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
								PLog("Traffic subscr: %s/%s/%s", leaf->subscr->server,leaf->subscr->serverObject,leaf->subscr->serverChannel);
						}
					}
#endif
					_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				BYTE priority;
				bool flagResendAccept = false;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE : thisId=%08X, peerId=%08X priority=%d", thisId, peerId, priority );

				LOCK_BRACKET_ON

					_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					if( subscr->state == CommSubscription::_StateSubscrRequestSent )
					{
						subscr->srvId = peerId;
						subscr->state = CommSubscription::_StateSubscrPeerAssigned;
					}
					if( peerId != subscr->srvId )
					{
						PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X with msg.peerId=%08X != subscr.srvId=%08X - ignored", msg.type, thisId, peerId, subscr->srvId );
						break;
					}
					if( subscr->state != CommSubscription::_StateSubscrPeerAssigned )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommRoutingTable: invalid priority=%d in _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE for subscrId=%08X - ignored", priority, thisId );
						}
						else
						{
							if (subscr->priority < priority)
								flagResendAccept = true;
							subscr->priority = priority;
						}

						try
						{
							subscr->processSyncResponse( parser );
						}
						catch( PError& )
						{
							PLog( "CommRoutingTable: exception in CommSubscription::processSyncResponse() - subscription cleared, exception rethrown" );
							subscr->clear();
							subscr->state = CommSubscription::_StateSubscrUnexpectedError;
							throw;//to enforce disconnect. On reconnect will operate on cleared subscription
						}
						subscr->state = CommSubscription::_StateSubscrSync;
#ifdef COMMLIB_CALCULATE_STATISTICS
						if (subscr->trafficCntPos>=0)
						{
							PInterlockedExchangeAdd(&_subscrTrafficStatCounters[4][subscr->trafficCntPos],msg.body._size());
							PInterlockedIncrement(&_subscrTrafficStatPacketCounters[4][subscr->trafficCntPos]);
							if (printTrafficLast)
							{
								if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
									PLog("Traffic subscr from server: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
							}
						}
#endif
						msg.body._shift( 1 );//to skip priority byte. ids will be broken, but it does not matter as they will be patched anyway
						msg.body._setRefCounted();

						int n = static_cast< int >( subscr->cli.size() );
						if( n )
						{
							PArrayAutoPtr< _SubscrLeaf* > leaves( new _SubscrLeaf*[ n ] );//although it seems safe to call _delSubscrLeaf within subscr->cli iteration, but it changes subscr->cli, so we will move it out of the loop - just in case
							int leafI = 0;
							int sync = 0;
							for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
							{
								_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
								PASSERT3( leafI < n );
								leaves[ leafI++ ] = leaf;
								if( leaf->sync )
									sync++;
							}
							PASSERT3( leafI == n );
#ifdef COMMLIB_CALCULATE_STATISTICS
							if (subscr->trafficCntPos >= 0 && sync > 0 )
							{
								PInterlockedExchangeAdd(&_subscrTrafficStatCounters[0][subscr->trafficCntPos],msg.body._size()*sync);
								PInterlockedExchangeAdd(&_subscrTrafficStatPacketCounters[0][subscr->trafficCntPos],sync);
								if (printTrafficLast)
								{
									if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
										PLog("Traffic subscr: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
								}
							}
#endif
							for( int i=0; i < n ; ++i )
							{
								_SubscrLeaf* leaf = leaves[ i ];
								if( leaf->sync )
								{
									PASSERT3(leaf->frozen == _SubscrLeaf::_frozenStateNormal);
									_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_RESYNC, subscr->priority, msg.body, CommMsgBody::_RefCountFlag() );
									m.internal.flags |= _CommMsg::Patch8;
									m.internal.patch0 = leaf->cliId;
									m.internal.patch1 = leaf->cliThisId;
									_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
								}
								else
								{
									try
									{
										if (flagResendAccept && leaf->cli->isLocal == 0)
											_postSubscrAccept( leaf, false );
										_processSubscrRequest( leaf );
									}
									catch( PError& err )
									{
										PLog( "Exception while processing subscription delayed request for leaf.cliThisId=%08X - leaf deleted, physical connection will be closed: '%s'", leaf->cliThisId, err.why() );
										_forceDisconnect( leaf->cli );
										_delSubscrLeaf( leaf );
										if( subscr->cli.size() == 0 || (subscr->cli.size() == subscr->frozenCounter && subscr->frozen == false))
											_scheduleDelSubscr( subscr );
									}
								}
							}//for( i )
						}//if( n )
					}
				}

				LOCK_BRACKET_OFF

			}
			break;
		case _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE1:
		case _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE2:
			{
				CommMsgParser parser( msg.body );
				CommMsgBody syncBody;
				UINT32 thisId, peerId;
				BYTE priority;
				bool flagResendAccept = false;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE (%x): thisId=%08X, peerId=%08X priority=%d", msg.type, thisId, peerId, priority );

				LOCK_BRACKET_ON

					_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
#ifdef COMMLIB_CALCULATE_STATISTICS
					if (subscr->trafficCntPos>=0)
					{
						PInterlockedExchangeAdd(&_subscrTrafficStatCounters[4][subscr->trafficCntPos],msg.body._size());
						PInterlockedIncrement(&_subscrTrafficStatPacketCounters[4][subscr->trafficCntPos]);
						if (printTrafficLast)
						{
							if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
								PLog("Traffic subscr from server: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
						}

					}
#endif

					if( subscr->state == CommSubscription::_StateSubscrRequestSent )
					{
						subscr->srvId = peerId;
						subscr->state = CommSubscription::_StateSubscrPeerAssigned;
					}
					if( peerId != subscr->srvId )
					{
						PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X with msg.peerId=%08X != subscr.srvId=%08X - ignored", msg.type, thisId, peerId, subscr->srvId );
						break;
					}
					if( subscr->state != CommSubscription::_StateSubscrPeerAssigned )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						bool nextPhase = false;
						bool needSyncRequest = false;
						for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
						{
							_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
							if (leaf->sync)
							{
								needSyncRequest = true;
								break;
							}
						}

						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommRoutingTable: invalid priority=%d in _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE for subscrId=%08X - ignored", priority, thisId );
						}
						else
						{
							if (subscr->priority < priority)
								flagResendAccept = true;
							subscr->priority = priority;
						}

						try
						{
							CommMsgBody sBody;
							PASSERT3(subscr->protocolVersion == 1);
							if (needSyncRequest)
							{
								subscr->genSyncRequest(sBody);
							}
							if ( msg.type == _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE1)
							{
								if (subscr->processSyncResponseEx( parser ) == false)
								{
									// next phase
									PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE1 - Send phase2 request: thisId=%08X, peerId=%08X priority=%d", thisId, peerId, priority );
									nextPhase = true;
									CommMsgBody body;
									body.composeUINT32( subscr->srvId ).composeUINT32(subscr->srvThisId);
									subscr->genSyncRequestExPhase2( body );
									_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE2, subscr->priority, body );
									_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
								}
							}
							else
							{
								subscr->processSyncResponseEx( parser );
							}

							if (needSyncRequest && nextPhase == false )
							{
								CommMsgParser syncParser(sBody);
								syncBody.composeUINT32(thisId).composeUINT32(peerId);
								subscr->processSyncRequest( syncBody, syncParser );
							}
						}
						catch( PError& er )
						{
							PLog( "CommRoutingTable: exception in CommSubscription::processSyncResponseEx() - subscription cleared, exception rethrown, %s" ,er.why());
							subscr->clear();
							subscr->state = CommSubscription::_StateSubscrUnexpectedError;
							throw;//to enforce disconnect. On reconnect will operate on cleared subscription
						}
						if (nextPhase == false)
						{
							subscr->state = CommSubscription::_StateSubscrSync;

							if (needSyncRequest)
								syncBody._setRefCounted();

							int n = static_cast< int >( subscr->cli.size() );
							if( n )
							{
								PArrayAutoPtr< _SubscrLeaf* > leaves( new _SubscrLeaf*[ n ] );//although it seems safe to call _delSubscrLeaf within subscr->cli iteration, but it changes subscr->cli, so we will move it out of the loop - just in case
								int leafI = 0;
								int sync = 0;
								for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
								{
									_SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
									PASSERT3( leafI < n );
									leaves[ leafI++ ] = leaf;
									if (leaf->sync)
										sync++;
								}
								PASSERT3( leafI == n );
#ifdef COMMLIB_CALCULATE_STATISTICS
								if (subscr->trafficCntPos >= 0 && sync > 0 )
								{
									PInterlockedExchangeAdd(&_subscrTrafficStatCounters[0][subscr->trafficCntPos],msg.body._size()*sync);
									PInterlockedExchangeAdd(&_subscrTrafficStatPacketCounters[0][subscr->trafficCntPos],sync);
									if (printTrafficLast)
									{
										if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
											PLog("Traffic subscr: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
									}
								}
#endif
								for( int i=0; i < n ; ++i )
								{
									_SubscrLeaf* leaf = leaves[ i ];
									if( leaf->sync )
									{
										PASSERT3(needSyncRequest);
										PASSERT3(leaf->frozen == _SubscrLeaf::_frozenStateNormal);
										_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_RESYNC, subscr->priority, syncBody, CommMsgBody::_RefCountFlag() );
										m.internal.flags |= _CommMsg::Patch8;
										m.internal.patch0 = leaf->cliId;
										m.internal.patch1 = leaf->cliThisId;
										_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
									}
									else
									{
										try
										{
											if (flagResendAccept && leaf->cli->isLocal == 0)
												_postSubscrAccept( leaf, false );
											_processSubscrRequest( leaf );
										}
										catch( PError& err )
										{
											PLog( "Exception while processing subscription delayed request for leaf.cliThisId=%08X - leaf deleted, physical connection will be closed: '%s'", leaf->cliThisId, err.why() );
											_forceDisconnect( leaf->cli );
											_delSubscrLeaf( leaf );
											if( subscr->cli.size() == 0 || (subscr->cli.size() == subscr->frozenCounter && subscr->frozen == false))
												_scheduleDelSubscr( subscr );
										}
									}
								}//for( i )
							}//if( n )
						}
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				//no trace
				UINT32 tickCounterMsg[12];
				if (doCounting)
				{
					memset(tickCounterMsg, 0, sizeof(tickCounterMsg));
					tickCounterMsg[0]=SYSTEMTICKS();
				}

				LOCK_BRACKET_ON_READ
					if (doCounting)
						tickCounterMsg[1]=SYSTEMTICKS();

				_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if (doCounting)
					tickCounterMsg[2]=SYSTEMTICKS();
				if( subscr )
				{
#ifdef COMMLIB_CALCULATE_STATISTICS
					if (subscr->trafficCntPos>=0)
					{
						PInterlockedExchangeAdd(&_subscrTrafficStatCounters[5][subscr->trafficCntPos],msg.body._size());
						PInterlockedIncrement(&_subscrTrafficStatPacketCounters[5][subscr->trafficCntPos]);
						if (printTrafficLast)
						{
							if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
								PLog("Traffic subscr from server: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
						}
					}
#endif

					if( subscr->state != CommSubscription::_StateSubscrSync )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						try
						{
							subscr->processOnlineUpdate( parser );
							if (doCounting)
								tickCounterMsg[3]=SYSTEMTICKS();
						}
						catch( PError& )
						{
							PLog("CommRoutingTable: exception in CommSubscription::processOnlineUpdate() (%s/%s/%s) - subscription cleared, unregistering server object", subscr->server, subscr->serverObject, subscr->serverChannel);
							subscr->clear();
							subscr->state = CommSubscription::_StateSubscrUnexpectedError;
                            _CommMsg m(_COMM_MSGTYPE_DESTROY_SERVEROBJECT, msg.priority);
                            m.body.composeUINT32(m.internal.mux);
                            _postToPhysConnRead(srcPhysConn, srcMux, srcMuxChannelId, m);
						}
						if (doCounting)
						{
							// 7 additional elements of tickCounterMsg array will be filled in the next call
							_conditionalPostToSubscrLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE, msg.body, tickCounterMsg+4 );
						}
						else
						{
							_conditionalPostToSubscrLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE, msg.body, 0 );
						}
					}
				}
				if (doCounting)
				{
					UINT32 t_12=SYSTEMTICKS();
					int i;
					for (i=0;i<4;++i)
					{
						if (tickCounterMsg[i])
						{
							PInterlockedExchangeAdd(&tickCounterInOneMsgSub[i],t_12 - tickCounterMsg[i]);
						}
					}
					for (i=4;i<12;++i)
					{
						if (tickCounterMsg[i])
						{
							PInterlockedExchangeAdd(&tickCounterInOneMsgSub[i],tickCounterMsg[i]);
						}
					}
				}

				LOCK_BRACKET_OFF_READ

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				BYTE code;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE(code);
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST: thisId=%08X, peerId=%08X,  code=%d", thisId, peerId, (int)code );

				LOCK_BRACKET_ON_READ

					_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( subscr )
				{
					if (code == 1)
						_startCountingRead( subscr, false, msg.body );
					else
						_stopCountingRead( subscr, false, msg.body );
				}

				LOCK_BRACKET_OFF_READ
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				UINT32 count;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32(count);
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_SubscrLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
				}
				if( leaf )
				{
					leaf->count = count;
				}

				LOCK_BRACKET_OFF_READ

			}
			break;


		case _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_SubscrLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					leaf = _lookForCliSubscrLeaf( peerId, srcPhysConn, srcMux );
					if( !leaf )
					{
						PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
					}
				}

				if( leaf )
				{
					_Subscr* subscr = static_cast<_Subscr*>(leaf->subscr);
					_delSubscrLeaf( leaf );
					if( subscr->cli.size() == 0 || (subscr->frozenCounter == subscr->cli.size() && subscr->frozen == false))
						_scheduleDelSubscr( subscr );
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_FREEZE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_FREEZE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_SubscrLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					leaf = _lookForCliSubscrLeaf( peerId, srcPhysConn, srcMux );
					if( !leaf )
					{
						PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_FREEZE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
					}
				}

				if( leaf )
				{
					_Subscr* subscr = static_cast<_Subscr*>(leaf->subscr);
					if (leaf->frozen != _SubscrLeaf::_frozenStateFrozen)
					{
						_freezeSubscrLeaf( leaf );
						if (subscr->cli.size() == subscr->frozenCounter && subscr->frozen == false )
							_scheduleDelSubscr( subscr );
					}
					else
					{
						PLog( "Incorrect state %d of subscr for _COMM_MSGTYPE_SUBSCRIPTION_FREEZE , thisId = %08X, cliId=%08X and physConn='%s'", leaf->frozen, thisId, peerId, srcPhysConn->address.c_str() );
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_DEFROST:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				msg.body._shift( parser._shift() );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_DEFROST: thisId=%08X, peerId=%08X", thisId, peerId );

				PRLock cfgLock( configLock );
				LOCK_BRACKET_ON

					_SubscrLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					leaf = _lookForCliSubscrLeaf( peerId, srcPhysConn, srcMux );
					if( !leaf )
					{
						PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_FREEZE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
					}
				}

				if( leaf )
				{
					if (leaf->frozen == _SubscrLeaf::_frozenStateFrozen)
					{
						_defrostSubscrLeaf( leaf, msg.body );
					}
					else
					{
						PLog( "Incorrect state %d of subscr for _COMM_MSGTYPE_SUBSCRIPTION_DEFROST , thisId = %08X, cliId=%08X and physConn='%s'", leaf->frozen, thisId, peerId, srcPhysConn->address.c_str() );
					}
				}

				LOCK_BRACKET_OFF
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				UINT16 errCode=0;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				if (!parser.parseEnded())
					parser.parseUINT16(errCode);
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Subscr* subscr;
				if( peerId )
					subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
					subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					if (errCode == _COMM_ERROR_PROTOCOLNOTSUPPORTED)
					{
						CommMsgBody body;
						body.composeUINT32( subscr->srvThisId ).composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel );
						subscr->protocolVersion = 0;
						subscr->genSyncRequest( body );
						PTRACE4( "CommRoutingTable: posting subscription request (version=0) for '%s'/'%s'/'%s' -> srvId=%08X", subscr->server, subscr->serverObject, subscr->serverChannel, subscr->srvThisId );
						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_REQUEST, subscr->priority, body );
						_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
						subscr->state = CommSubscription::_StateSubscrRequestSent;
						subscr->srvId = 0;//for special error cases
					}
					else
					{
						_postToSubscrLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR, msg.body );
						_delSubscr( subscr );
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Subscr* subscr;
				if( peerId )
					subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
					subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					_conditionalPostToSubscrLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT, msg.body, 0 );
					if( subscr->state == CommSubscription::_StateSubscrSync )
						subscr->state = CommSubscription::_StateSubscrDesync;
					//else state should not be changed
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_RESYNC:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_RESYNC: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Subscr* subscr = _getSrvSubscr( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( subscr )
				{
					if( subscr->state != CommSubscription::_StateSubscrDesync )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_RESYNC msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						try
						{
							subscr->processSyncResponse( parser );
						}
						catch( PError& )
						{
							PLog( "CommRoutingTable: exception in CommSubscription::processSyncResponse() - subscription cleared, exception rethrown" );
							subscr->clear();
							subscr->state = CommSubscription::_StateSubscrUnexpectedError;
							throw;//to enforce disconnect. On reconnect will operate on cleared subscription
						}
						_conditionalPostToSubscrLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_RESYNC, msg.body, 0 );
						subscr->state = CommSubscription::_StateSubscrSync;
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNQUALITY_CLISUBSCR msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->srv != 0 );//not in this conn->state
						if( conn->srv->isLocal == 0 )//far remote
						{
							conn->wantQualityUpdates = true;
							_postCliQualityUpdate( conn,_TRANSPORT_KEEPALIVE_CONNECT );
							if (conn->srv->qualityRequired)
							{
								_postCliQualityUpdate( conn,conn->srv->quality );
							}
							else
							{
								conn->srv->qualityRequired = true;
								serviceCallTransport(conn->srv,_SERVICE_REQUEST_QUALITY,0);
							}
							
						}
						else
						{
							_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
							_postToPhysConn( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );
						}
					}
				}

				LOCK_BRACKET_OFF

			}
			break;
		case _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

				_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVSUBSCR msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->cli != 0 );//not in this conn->state
						if( conn->cli->isLocal == 0 )//far remote
						{
							conn->wantQualityUpdates = true;
							_postSrvQualityUpdate( conn, _TRANSPORT_KEEPALIVE_CONNECT );
							if (conn->cli->qualityRequired)
							{
								_postSrvQualityUpdate( conn, conn->cli->quality );
							}
							else
							{
								conn->cli->qualityRequired = true;
								serviceCallTransport(conn->cli,_SERVICE_REQUEST_QUALITY,0);
							}
						}
						else
						{
							_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
							_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
						}
					}
				}

				LOCK_BRACKET_OFF

			}
			break;
		case _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						PASSERT3( conn->cli != 0 );//not in this conn->state
						if( conn->cli->isLocal == 0 )//far remote
						{
							conn->wantIsAlive = true;
							serviceCallTransport(conn->cli,_SERVICE_REQUEST_FORCE_KEEP_ALIVE,0);
						}
						else
						{
							_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
							_postToPhysConn( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
						}
					}
				}

				LOCK_BRACKET_OFF

			}
			break;


		case _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_Conn* conn = _getSrvConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						_patchMsg8( msg, conn->cliId, conn->cliThisId, conn->priority );
						_postToPhysConnRead( conn->cli, conn->muxCli, conn->muxCliChannelId, msg );
					}
				}

				LOCK_BRACKET_OFF_READ

			}
			break;
		case _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
						_postToPhysConnRead( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );
					}
				}

				LOCK_BRACKET_OFF_READ

			}
			break;
		case _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_Conn* conn = _getCliConn( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( conn )
				{
					if( conn->state != _CommConnection::_StateConnOk )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE msg for connId=%08X in state=%d - ignored", thisId, conn->state );
					}
					else
					{
						_patchMsg8( msg, conn->srvId, conn->srvThisId, conn->priority );
						_postToPhysConnRead( conn->srv, conn->muxSrv, conn->muxSrvChannelId, msg );
					}
				}

				LOCK_BRACKET_OFF_READ

			}
			break;

//**********************************


		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST:
			{
				CommMsgParser parser( msg.body );
				UINT32 peerId;
				PString server;
				PString serverObject;
				PString serverChannel;
				parser.parseUINT32( peerId ).parseStringP( server ).parseStringP( serverObject ).parseStringP( serverChannel );
				msg.body._shift( parser._shift() );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST : server='%s' serverObject='%s' serverChannel='%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );

				UINT16 errCode = 0;
				const char* errMsg = 0;
				bool isLocal = false;
				vector <const char * > ips;
				ips.push_back(srcPhysConn->address);

				PString checkRestrictionsFailReason;
				PRLock cfgLock( configLock );
				if (!checkRestrictionsSubscription(server,serverObject,serverChannel,ips, &checkRestrictionsFailReason))
				{
					errCode = _COMM_ERROR_NOROUTETOHOST;
					errMsg = "No allowed route to host";
					PTRACE3("CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST: server='%s' serverObject='%s' serverChannel='%s' from '%s' rejected: %s", server.c_str(), serverObject.c_str(), serverChannel.c_str(), srcPhysConn->address.c_str(), checkRestrictionsFailReason.c_str());
				}
				else
				{
					isLocal = _isAliasServer( server );
					if( isLocal )
						server = "";//special value for local server
				}

				LOCK_BRACKET_ON
					if (errCode == 0)
					{
						_SubscrLight* subscr = nameSubscrLightMap.findByName( server, serverObject, serverChannel );
						try
						{
							if( subscr )
							{
								PTRACE4( "CommRoutingTable: found light subscription '%s'/'%s'/'%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );
							}
							else
							{
								_PhysConn* srvPhysConn = 0;
								int muxSrv = 0;
								UINT32 muxSrvChannelId = 0;

								if( isLocal )
									srvPhysConn = _findLocalPhysConn( muxSrv, muxSrvChannelId, serverObject, errCode, errMsg );
								else
									srvPhysConn = _findOrCreateRemotePhysConn( muxSrv, muxSrvChannelId, server, serverObject, serverChannel, 0, errCode, errMsg );
								if( srvPhysConn )
								{
									PTRACE4( "CommRoutingTable: creating new subscription '%s'/'%s'/'%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str() );
									subscr = _createNewSubscrLight( server, serverObject, serverChannel, srvPhysConn, muxSrv, muxSrvChannelId );
									switch( srvPhysConn->state )
									{
									case _PhysConn::_StatePhysConnOk:
										_postSubscrLightRequest( subscr );
										break;
									case _PhysConn::_StatePhysConnDisc:
										srvPhysConn->mgr->connect( srvPhysConn );
										srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
										break;
									case _PhysConn::_StatePhysConnRequestSent:
									case _PhysConn::_StatePhysConnReconn:
										//do nothing
										break;
									default:
										PASSERT3( 0 );
										break;
									}
								}
								else
								{
									PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), errMsg );
									_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
									m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
									_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
								}
							}
							if( subscr )
							{
								_SubscrLightLeaf* leaf = _createNewSubscrLightLeaf( subscr, srcPhysConn, srcMux, srcMuxChannelId, peerId );
								PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST('%s'/'%s'/'%s') accepted: leaf.cliThisId=%08X", server.c_str(), serverObject.c_str(), serverChannel.c_str(), leaf->cliThisId );
								_postSubscrAccept( leaf, true );
								if (subscr->needCounting)
								{
									if (leaf->cli->isLocal !=0 )
									{
										CommMsgBody body;
										body.composeUINT32( leaf->cliId ).composeUINT32( leaf->cliThisId ).composeBYTE(1);
										_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST, subscr->priority, body );
										_postToPhysConn( leaf->cli, leaf->muxCli, leaf->muxCliChannelId, m );
									}
								}
								if( subscr->state == CommSubscription::_StateSubscrSync )
								{
									_processSubscrLightRequest( leaf );
								}
							}
						}
						catch( PError& err )
						{
							PLog( "Exception in _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST '%s' err='%s'\n"
								"server='%s', serverObject='%s', serverChannel='%s'"
								"subscription %s ", srcPhysConn->address.c_str(), err.why(), server.c_str(), serverObject.c_str(), serverChannel.c_str(), (subscr ? "found": "not found") );
							throw;
						}
					}
					else
					{
						PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST('%s'/'%s'/'%s') failed: '%s'", server.c_str(), serverObject.c_str(), serverChannel.c_str(), errMsg );
						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, _COMM_MSGPRIORITY_INITSUBSCR );
						m.body.composeUINT32( peerId ).composeUINT32( 0 ).composeUINT16( errCode ).composeString( errMsg );
						_postToPhysConn( srcPhysConn, srcMux, srcMuxChannelId, m );
					}
					LOCK_BRACKET_OFF
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

					_SubscrLight* subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					if( subscr->state != CommSubscription::_StateSubscrRequestSent )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						subscr->srvId = peerId;
						subscr->state = CommSubscription::_StateSubscrPeerAssigned;
					}
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				BYTE priority;
				bool flagResendAccept = false;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE( priority );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE : thisId=%08X, peerId=%08X priority=%d", thisId, peerId, priority );

				LOCK_BRACKET_ON

					_SubscrLight* subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					if( subscr->state == CommSubscription::_StateSubscrRequestSent )
					{
						subscr->srvId = peerId;
						subscr->state = CommSubscription::_StateSubscrPeerAssigned;
					}
					if( peerId != subscr->srvId )
					{
						PLog( "CommRoutingTable: msg type=%02X for srvSubscrId=%08X with msg.peerId=%08X != subscr.srvId=%08X - ignored", msg.type, thisId, peerId, subscr->srvId );
						break;
					}
					if( subscr->state != CommSubscription::_StateSubscrPeerAssigned &&
						subscr->state != CommSubscription::_StateSubscrDesync 
					  )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						if( priority >= _COMM_MSGPRIORITIES )
						{
							PLog( "CommRoutingTable: invalid priority=%d in _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE for subscrId=%08X - ignored", priority, thisId );
						}
						else
						{
							if (subscr->priority < priority)
								flagResendAccept = true;
							subscr->priority = priority;
						}

						subscr->state = CommSubscription::_StateSubscrSync;
#ifdef COMMLIB_CALCULATE_STATISTICS
						if (subscr->trafficCntPos>=0)
						{
							PInterlockedExchangeAdd(&_subscrTrafficStatCounters[4][subscr->trafficCntPos],msg.body._size());
							PInterlockedIncrement(&_subscrTrafficStatPacketCounters[4][subscr->trafficCntPos]);
							if (printTrafficLast)
							{
								if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
									PLog("Traffic subscr from server: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
							}
						}
#endif

						msg.body._shift( 1 );//to skip priority byte. ids will be broken, but it does not matter as they will be patched anyway
						msg.body._setRefCounted();

						int n = static_cast< int >( subscr->cli.size() );
						if( n )
						{
							PArrayAutoPtr< _SubscrLightLeaf* > leaves( new _SubscrLightLeaf*[ n ] );//although it seems safe to call _delSubscrLeaf within subscr->cli iteration, but it changes subscr->cli, so we will move it out of the loop - just in case
							int leafI = 0;
							for( _SubscrLeafSetByCli::iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
							{
								_SubscrLightLeaf* leaf = *iter;
								PASSERT3( leafI < n );
								leaves[ leafI++ ] = leaf;
							}
							PASSERT3( leafI == n );
							for( int i=0; i < n ; ++i )
							{
								_SubscrLightLeaf* leaf = leaves[ i ];
								try
								{
									if (flagResendAccept && leaf->cli->isLocal == 0)
										_postSubscrAccept( leaf, true );
									_processSubscrLightRequest( leaf );
								}
								catch( PError& err )
								{
									PLog( "Exception while processing subscription delayed request for leaf.cliThisId=%08X - leaf deleted, physical connection will be closed: '%s'", leaf->cliThisId, err.why() );
									_forceDisconnect( leaf->cli );
									_delSubscrLightLeaf( leaf );
									if( subscr->cli.size() == 0 )
										_scheduleDelSubscrLight( subscr );
								}
							}//for( i )
						}//if( n )
					}
				}

				LOCK_BRACKET_OFF
			}
			break;
		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				//no trace

				LOCK_BRACKET_ON_READ

				_SubscrLight* subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( subscr )
				{
#ifdef COMMLIB_CALCULATE_STATISTICS
					if (subscr->trafficCntPos>=0)
					{
						PInterlockedExchangeAdd(&_subscrTrafficStatCounters[5][subscr->trafficCntPos],msg.body._size());
						PInterlockedIncrement(&_subscrTrafficStatPacketCounters[5][subscr->trafficCntPos]);
						if (printTrafficLast)
						{
							if (subscr->trafficCntPos == _maxSubscrTrafficCounter - 1)
								PLog("Traffic subscr from server: %s/%s/%s", subscr->server,subscr->serverObject,subscr->serverChannel);
						}
					}
#endif
					if( subscr->state != CommSubscription::_StateSubscrSync )
					{
						PLog( "CommRoutingTable: unexpected _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE msg for subscrId=%08X in state=%d - ignored", thisId, subscr->state );
					}
					else
					{
						_conditionalPostToSubscrLightLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE, msg.body);
					}
				}
				LOCK_BRACKET_OFF_READ
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				BYTE code;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseBYTE(code);
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST: thisId=%08X, peerId=%08X,  code=%d", thisId, peerId, (int)code );

				LOCK_BRACKET_ON_READ

					_SubscrLight* subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, peerId, msg.type );
				if( subscr )
				{
					if (code == 1)
						_startCountingRead( subscr, true, msg.body );
					else
						_stopCountingRead( subscr, true, msg.body );
				}

				LOCK_BRACKET_OFF_READ
			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				UINT32 count;
				parser.parseUINT32( thisId ).parseUINT32( peerId ).parseUINT32(count);
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON_READ

					_SubscrLightLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLightLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
				}
				if( leaf )
				{
					leaf->count = count;
				}

				LOCK_BRACKET_OFF_READ

			}
			break;


		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

				_SubscrLightLeaf* leaf = 0;
				if( thisId )
					leaf = _getCliSubscrLightLeaf( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
				{//special case: looking for peerId
					leaf = _lookForCliSubscrLightLeaf( peerId, srcPhysConn, srcMux );
					if( !leaf )
					{
						PLog( "Unable to find subscr for _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE with cliId=%08X and physConn='%s'", peerId, srcPhysConn->address.c_str() );
					}
				}

				if( leaf )
				{
					_SubscrLight* subscr = leaf->subscr;
					_delSubscrLightLeaf( leaf );
					if( subscr->cli.size() == 0 )
						_scheduleDelSubscrLight( subscr );
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				UINT16 errCode=0;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				if (!parser.parseEnded())
					parser.parseUINT16(errCode);
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

				_SubscrLight* subscr;
				if( peerId )
					subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
					subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					_postToSubscrLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR, msg.body );
					_delSubscrLight( subscr );
				}

				LOCK_BRACKET_OFF

			}
			break;

		case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT:
			{
				CommMsgParser parser( msg.body );
				UINT32 thisId, peerId;
				parser.parseUINT32( thisId ).parseUINT32( peerId );
				PTRACE4( "CommRoutingTable: _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT: thisId=%08X, peerId=%08X", thisId, peerId );

				LOCK_BRACKET_ON

				_SubscrLight* subscr;
				if( peerId )
					subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, peerId, msg.type );
				else
					subscr = _getSrvSubscrLight( srcMux, srcPhysConn, thisId, msg.type );
				if( subscr )
				{
					_conditionalPostToSubscrLightLeafs( subscr, _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT, msg.body );
					if( subscr->state == CommSubscription::_StateSubscrSync )
						subscr->state = CommSubscription::_StateSubscrDesync;
					//else state should not be changed
				}

				LOCK_BRACKET_OFF

			}
			break;




//**********************************


		default:
			PLog( "CommRoutingTable: unknown %02X msg.type from '%s' - ignored", msg.type, srcPhysConn->address.c_str() );
			break;
		}
	}

	catch( PFatalError& err )
	{
		PWLock lock( cs );
		PLog( "Fatal error exception in CommRoutingTable::routeIncomingMessage() while processing msg with type %02X from '%s' - physical connection will be closed: '%s'", msg.type, srcPhysConn->address.c_str(), err.why() );
		_forceDisconnect( srcPhysConn );
		if (exitOnFatalError)
		{
			FATALEXIT();
		}
	}
	catch( PSystemError& err  )
	{
		PWLock lock( cs );
		PLog( "System Exception in CommRoutingTable::routeIncomingMessage() while processing msg with type %02X from '%s' - physical connection will be closed: '%s'", msg.type, srcPhysConn->address.c_str(), err.why() );
		_forceDisconnect( srcPhysConn );
	}
	catch( PError& err  )
	{
		PWLock lock( cs );
		PLog( "Exception in CommRoutingTable::routeIncomingMessage() while processing msg with type %02X from '%s' - physical connection will be closed: '%s'", msg.type, srcPhysConn->address.c_str(), err.why() );
		_forceDisconnect( srcPhysConn );
	}

	UINT32 t2;
	if (0 == t1_1)
		t2 = SYSTEMTICKS()-t0;
	else
		t2 = SYSTEMTICKS()-t1_1;
	if( t2 > TICKS_PER_SECOND )
		PLog( "CommRoutingTable::routeIncomingMessage(%02X): long processing (%u ticks)", msg.type, t2 );
}

bool CommRoutingTable::_processTimerMessageRead( _CommMsg& msg )
{
	int index=-1;
	UINT32 t0_1 = SYSTEMTICKS();
	UINT32 t0 = t0_1;
	try
	{
		switch( msg.type )
		{
		case _TIMER_SUBSCR_COUNTING:
			{
				index=4;
				CommMsgParser parser( msg.body );
				UINT32 srvId, srvThisId, uniqueId;
				parser.parseUINT32( srvThisId ).parseUINT32( srvId ).parseUINT32( uniqueId );

				PRLock lock( cs );
				t0 = SYSTEMTICKS();

				const _Subscr* const * subscrPtr = srvSubscrMap.find( srvThisId );
				if( subscrPtr == 0 )
				{
					PTRACE4( "CommRoutingTable: _TIMER_SUBSCR_COUNTING for unknown srvSubscrId=%08X - ignored", srvThisId );
					break;
				}
				const _Subscr* subscr = *subscrPtr;
				PASSERT3( subscr->srvThisId == srvThisId );
				if (srvId != subscr->srvId  || uniqueId != subscr->_subscrUniqueId )
				{
					PLog( "CommRoutingTable: _TIMER_SUBSCR_COUNTING invalid for srvSubscrId=%08X: srvId=%08X, subscr->srvId=%08X uniqueId = %08X, subscr->_subscrUniqueId=%08X",subscr->srvThisId,srvId, subscr->srvId, uniqueId, subscr->_subscrUniqueId);
					break;
				}
				if (subscr->needCounting)
				{
					_postCountingUpdateRead(subscr,false);
					CommMsgBody timerMsg;
					timerMsg.composeUINT32( subscr->srvThisId ).composeUINT32( subscr->srvId ).composeUINT32( subscr->_subscrUniqueId );
					_CommMsg tmpMsg( _TIMER_SUBSCR_COUNTING, 0, timerMsg );
					timerThread->push( tmpMsg, optTimeoutSubscrCounting );
				}
			}
			break;
		case _TIMER_SUBSCR_LIGHT_COUNTING:
			{
				index=4;
				CommMsgParser parser( msg.body );
				UINT32 srvId, srvThisId, uniqueId;
				parser.parseUINT32( srvThisId ).parseUINT32( srvId ).parseUINT32( uniqueId );

				PRLock lock( cs );
				t0 = SYSTEMTICKS();

				const _SubscrLight* const * subscrPtr = srvSubscrLightMap.find( srvThisId );
				if( subscrPtr == 0 )
				{
					PTRACE4( "CommRoutingTable: _TIMER_SUBSCR_LIGHT_COUNTING for unknown srvSubscrId=%08X - ignored", srvThisId );
					break;
				}
				const _SubscrLight* subscr = *subscrPtr;
				PASSERT3( subscr->srvThisId == srvThisId );
				if (srvId != subscr->srvId  || uniqueId != subscr->_subscrUniqueId )
				{
					PLog( "CommRoutingTable: _TIMER_SUBSCR_LIGHT_COUNTING invalid for srvSubscrId=%08X: srvId=%08X, subscr->srvId=%08X uniqueId = %08X, subscr->_subscrUniqueId=%08X",subscr->srvThisId,srvId, subscr->srvId, uniqueId, subscr->_subscrUniqueId);
					break;
				}
				if (subscr->needCounting)
				{
					_postCountingUpdateRead(subscr,true);
					CommMsgBody timerMsg;
					timerMsg.composeUINT32( subscr->srvThisId ).composeUINT32( subscr->srvId ).composeUINT32( subscr->_subscrUniqueId );
					_CommMsg tmpMsg( _TIMER_SUBSCR_LIGHT_COUNTING, 0, timerMsg );
					timerThread->push( tmpMsg, optTimeoutSubscrCounting );
				}
			}
			break;

		default:
			return false;
		}
	}
	catch( PFatalError& err )
	{
		PLog( "Fatal error exception in CommRoutingTable::_processTimerMessage() while processing msg with type %02X: '%s'", msg.type, err.why() );
		if (exitOnFatalError)
		{
			FATALEXIT();
		}
	}
	catch( PSystemError& err  )
	{
		PLog( "System Exception in CommRoutingTable::_processTimerMessage() while processing msg with type %02X: '%s'", msg.type,  err.why() );
	}
	catch( PError& err  )
	{
		PLog( "Exception in CommRoutingTable::_processTimerMessage() while processing msg with type %02X: '%s'", msg.type, err.why() );
	}

	UINT32 t1 = SYSTEMTICKS();
	if( t1 - t0 > TICKS_PER_SECOND )
		PLog( "CommRoutingTable::_processTimerMessage(%02X): long processing (%u ticks)", msg.type, t1 - t0 );

	if (doCounting)
	{
		if (index>=0 && index < TIMER_PERFORMANCE_COUNTERS_SIZE)
		{
			PInterlockedExchangeAdd( tickCounterTimer + index, t1 - t0_1 );
			PInterlockedExchangeAdd( tickCounterAfterLockTimer + index, t1 - t0 );
			PInterlockedIncrement( enterCounterTimer + index );
		}
	}

	return true;
}

bool CommRoutingTable::_isRMsg( size_t n )
{
	PASSERT5( n < MSG_PERFORMANCE_COUNTERS_SIZE );
	switch( n )
	{
	case _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG:
	case _COMM_MSGTYPE_CONNECT_SERVER_USERMSG:
	case _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT:
	case _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT:
	case _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST:
	case _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE:
	case _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE:
	case _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE:
	case _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE:
	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE:
	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST:
	case _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE:
		return true;
	default:
		break;
	}
	return false;
}

bool CommRoutingTable::_isRTimer( size_t n )
{
	PASSERT5( n < TIMER_PERFORMANCE_COUNTERS_SIZE );
	switch( n )
	{
	case _TIMER_SUBSCR_COUNTING:
	case _TIMER_SUBSCR_LIGHT_COUNTING:
		return true;
	default:
		break;
	}
	return false;
}

void CommRoutingTable::_processTimerMessage( _CommMsg& msg )
{
	int index=-1;
	UINT32 t0_1 = SYSTEMTICKS();
	PRLock cfgLock( configLock );
	PWLock lock( cs );
	UINT32 t0 = SYSTEMTICKS();
	try
	{
		switch( msg.type )
		{
		case _TIMER_PHYSRECONNECT:
			{
				index=0;
				CommMsgParser parser( msg.body );
				const char * server;
				const char * serverObject;
				const char * serverChannel;
				parser.parseString(server)
					.parseString(serverObject)
					.parseString(serverChannel);

				UINT16 errCode;
				const char* errMsg;
				int muxSrv;
				UINT32 muxSrvChannelId;
				PTRACE3("CommRoutingTable: timer _TIMER_PHYSRECONNECT: '%s'/'%s'/'%s'", server,serverObject,serverChannel);
				_PhysConn* srvPhysConn = _findOrCreatePhysConn( muxSrv, muxSrvChannelId, server, serverObject, serverChannel, 0, true, errCode, errMsg );
				if (srvPhysConn)
				{
					if (srvPhysConn->mgr->isForceClientDisconnects() && !srvPhysConn->inbound)
					{
						if (srvPhysConn->state == _PhysConn::_StatePhysConnDisc)
						{
							srvPhysConn->mgr->connect( srvPhysConn );
							srvPhysConn->state = _PhysConn::_StatePhysConnRequestSent;
						}
					}
				}
				else
				{
					PTRACE3("CommRoutingTable: timer _TIMER_PHYSRECONNECT: phys connection not found, errCode=%d,errMsg='%s'",errCode,errMsg);
				}
			}
			break;
		case _TIMER_RECONN:
			{
				index=1;
				CommMsgParser parser( msg.body );
				for(;;)
				{
					UINT32 cliThisId;
					parser.parseUINT32( cliThisId );
					if( cliThisId == 0 )
						break;
					_Conn** connPtr = cliConnMap.find( cliThisId );
					if( connPtr == 0 )//already deleted
					{
						PTRACE4( "CommRoutingTable: reconnect request for unknown cliId=%08X - ignored", cliThisId );
						continue;//forever
					}

					_reconnect( *connPtr );
				}
			}
			break;

		case _TIMER_RESUBSCR:
			{
				index=2;
				CommMsgParser parser( msg.body );
				for(;;)
				{
					const char* server;
					const char* serverObject;
					const char* serverChannel;
					UINT32 uniqueId;
					parser.parseString( server ).parseString( serverObject ).parseString( serverChannel ).parseUINT32(uniqueId);
					if( *server == 0 && *serverObject == 0 && *serverChannel == 0 )
						break;

					_Subscr* subscr = nameSubscrMap.findByName( server, serverObject, serverChannel );
					if( ! subscr )
					{
						PTRACE4( "CommRoutingTable: resubscribe request for unknown subscription ('%s'/'%s'/'%s') - ignored", server, serverObject, serverChannel );
						continue;//forever
					}
					if (subscr->_subscrUniqueId != uniqueId)
					{
						PTRACE3( "CommRoutingTable: subscription name conflict: resubscribe request for subscription ('%s'/'%s'/'%s') - ignored", server, serverObject, serverChannel );
						continue;//forever
					}
					if (subscr->frozen == false)
					{
						// Subscription could be frozen after timer sent
						PTRACE4( "CommRoutingTable: timer resubscribe '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
						_resubscribe( subscr );
					}
				}
			}
			break;

		case _TIMER_RESUBSCR_LIGHT:
			{
				index=2;
				CommMsgParser parser( msg.body );
				for(;;)
				{
					const char* server;
					const char* serverObject;
					const char* serverChannel;
					UINT32 uniqueId;
					parser.parseString( server ).parseString( serverObject ).parseString( serverChannel ).parseUINT32(uniqueId);
					if( *server == 0 && *serverObject == 0 && *serverChannel == 0 )
						break;

					_SubscrLight* subscr = nameSubscrLightMap.findByName( server, serverObject, serverChannel );
					if( ! subscr )
					{
						PTRACE4( "CommRoutingTable: resubscribe request for unknown light subscription ('%s'/'%s'/'%s') - ignored", server, serverObject, serverChannel );
						continue;//forever
					}
					if (subscr->_subscrUniqueId != uniqueId)
					{
						PTRACE3( "CommRoutingTable: subscription name conflict: resubscribe request for light subscription ('%s'/'%s'/'%s') - ignored", server, serverObject, serverChannel );
						continue;//forever
					}
					PTRACE4( "CommRoutingTable: timer resubscribe LIGHT '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
					_resubscribeLight( subscr );
				}
			}
			break;

		case _TIMER_DELSUBSCR:
			{
				index=3;
				CommMsgParser parser( msg.body );
				const char* server;
				const char* serverObject;
				const char* serverChannel;
				UINT32 whenTimerWasSent;

				parser.parseString( server ).parseString( serverObject ).parseString( serverChannel ).parseUINT32(whenTimerWasSent);

				_Subscr* subscr = nameSubscrMap.findByName( server, serverObject, serverChannel );
				if( ! subscr )
				{
					PTRACE4( "CommRoutingTable: delsubscribe request for unknown subscription ('%s'/'%s'/'%s') - ignored", server, serverObject, serverChannel );
					break;
				}

				subscr->timerWasPost = false;
				if (0 == subscr->srvId && subscr->srv && subscr->srv->state == _PhysConn::_StatePhysConnOk)  // to avoid llinear search on server side (_lookForCliSubscrLeaf)
				{
					subscr->deleteSubscrTime = SYSTEMTICKS();
					CommMsgBody timerMsg;
					timerMsg.composeString( server ).composeString( serverObject ).composeString( serverChannel ).composeUINT32(subscr->deleteSubscrTime);
					_CommMsg tmpMsg( _TIMER_DELSUBSCR, 0, timerMsg );
					timerThread->push(tmpMsg, optSubscrExpire );
					PTRACE3( "CommRoutingTable: unsubscribing from subscription '%s'/'%s'/'%s' - zero srvId, timer rescheduled", subscr->server, subscr->serverObject, subscr->serverChannel );
					subscr->timerWasPost = true;
				}
				else if( subscr->cli.size() == 0 )
				{
					PTRACE4( "CommRoutingTable: unsubscribing from subscription '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
					INT32 del = subscr->deleteSubscrTime - whenTimerWasSent;
					if ( del > TIME_SANITY_CHECK)
					{
						// somebody called _scheduleDelSubscr() after we had posted this timer
						// Reschedule timer
						CommMsgBody timerMsg;
						timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->deleteSubscrTime);
						_CommMsg tmpMsg( _TIMER_DELSUBSCR, 0, timerMsg );
						timerThread->push(tmpMsg, del);
						subscr->timerWasPost = true;
					}
					else
					{
						// nobody tried to delete subscription after this timer was post.
						// In conjunction with cli.size() == 0 it means that we are ready to delete subscription
						CommMsgBody body;
						body.composeUINT32( subscr->srvId ).composeUINT32( subscr->srvThisId );
						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE, subscr->priority, body );
						_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
						_delSubscr( subscr );
					}
				}
				else if (subscr->frozenCounter == subscr->cli.size() && subscr->frozen == false)
				{
					PTRACE4( "CommRoutingTable: freesing subscription '%s'/'%s'/'%s'", subscr->server, subscr->serverObject, subscr->serverChannel );
					INT32 del = subscr->deleteSubscrTime - whenTimerWasSent;
					if ( del > TIME_SANITY_CHECK)
					{
						CommMsgBody timerMsg;
						timerMsg.composeString( subscr->server ).composeString( subscr->serverObject ).composeString( subscr->serverChannel ).composeUINT32(subscr->deleteSubscrTime);
						_CommMsg tmpMsg( _TIMER_DELSUBSCR, 0, timerMsg );
						timerThread->push(tmpMsg, del );
						subscr->timerWasPost = true;
					}
					else
					{
						CommMsgBody body;
						body.composeUINT32( subscr->srvId ).composeUINT32( subscr->srvThisId );
						_CommMsg m( _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE, subscr->priority, body );
						_postToPhysConn( subscr->srv, subscr->muxSrv, subscr->muxSrvChannelId, m );
						_freezeSubscr( subscr );
					}
				}
			}
			break;

#ifdef COMMLIB_CALCULATE_STATISTICS
		case _TIMER_PERFORMANCE_TRAFFIC_COUNTING:
			{
				index=5;
				int i;
				if(_maxConnTrafficCounter > 0)
				{
					PLog("Connections       | From cli | Numbers|  To cli  | Numbers|");
					for (i=0;i<_maxConnTrafficCounter;i++)
					{
						PLog("%2d.%15.15s|%10u|%8u|%10u|%8u|",
							i,_connTrafficNames[i].c_str(),
							_connTrafficStatCounters[0][i],_connTrafficStatPacketCounters[0][i],_connTrafficStatCounters[1][i],_connTrafficStatPacketCounters[1][i]);
					}
				}

				if(_maxSubscrTrafficCounter > 0)
				{
					PLog("Subscriptions     | Subscr   | Numbers| Update   | Numbers| From cli | Numbers| To srv   | Numbers| From srv | Numbers|Srv update| Numbers|");
					for (i=0;i<_maxSubscrTrafficCounter;i++)
					{
						PLog("%2d-%15.15s|%10u|%8u|%10u|%8u|%10u|%8u|%10u|%8u|%10u|%8u|%10u|%8u|",
							i,_subscrTrafficNames[i].c_str(),
							_subscrTrafficStatCounters[0][i],_subscrTrafficStatPacketCounters[0][i],
							_subscrTrafficStatCounters[1][i],_subscrTrafficStatPacketCounters[1][i],
							_subscrTrafficStatCounters[2][i],_subscrTrafficStatPacketCounters[2][i],
							_subscrTrafficStatCounters[3][i],_subscrTrafficStatPacketCounters[3][i],
							_subscrTrafficStatCounters[4][i],_subscrTrafficStatPacketCounters[4][i],
							_subscrTrafficStatCounters[5][i],_subscrTrafficStatPacketCounters[5][i]);
					}
				}
				memset(_subscrTrafficStatCounters,0,sizeof(_subscrTrafficStatCounters));
				memset(_connTrafficStatCounters,0,sizeof(_connTrafficStatCounters));
				memset(_subscrTrafficStatPacketCounters,0,sizeof(_subscrTrafficStatPacketCounters));
				memset(_connTrafficStatPacketCounters,0,sizeof(_connTrafficStatPacketCounters));

				if(doTrafficCounting)
				{
					CommMsgBody timerMsg;
					_CommMsg tmpMsg( _TIMER_PERFORMANCE_TRAFFIC_COUNTING, 0, timerMsg );
					timerThread->push(tmpMsg, optTimeoutTrafficCounting );
				}
			}
			break;
#endif
		case _TIMER_PERFORMANCE_COUNTING:
			{
				index=6;
				if (doCounting)
				{
					UINT32 whenStart;
					CommMsgParser parser(msg.body);
					parser.parseUINT32(whenStart);
					UINT32 totalTicksR=0, totalTicksAfterLockR=0, totalEntersR=0;
					UINT32 totalTicksW=0, totalTicksAfterLockW=0, totalEntersW=0;
					for (size_t i = 0;i<sizeof(tickCounter)/sizeof(tickCounter[0]);i++)
					{
						if (enterCounter[i])
						{
							PLog("CommRoutingTable msg=%02u/0x%02X: ticks=%d, ticksAfterLock=%d, number=%d",i,i, tickCounter[i],tickCounterAfterLock[i],enterCounter[i]);
							if (i == _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG)
							{
								static const char * const names[PERFORMANCE_COUNTERS_INONEMSG_SIZE]=
								{
									"commrtable: Before LOCK",
									"commrtable: After LOCK",
									"commrtable: After getCliConn",
									"commrtable: After patchMsg",
									"CommThreadManager::push",
									"CommThreadManager::_push after PLock",
									"CommMsgQueue::push",
									"PQueue:push - after lock",
									"PQueue:push - after push",
									"PQueue:push - after event",
									"commrtable: Last call"
								};
								for (int j = 0;j<sizeof(tickCounterInOneMsg)/sizeof(tickCounterInOneMsg[0]);j++)
								{
									PLog("   %40s: ticks=%d", names[j], tickCounterInOneMsg[j]);
									tickCounterInOneMsg[j]=0;
								}
							}
							if (i == _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE)
							{
								static const char * const names[PERFORMANCE_COUNTERS_INONEMSGSUB_SIZE]=
								{
									"commrtable: Before LOCK",
									"commrtable: After LOCK",
									"commrtable: After _getSrvSubscr",
									"commrtable: After processOnlineUpdate",
									"commrtable: PostToSubscrLeafs",
									"CommThreadManager::push",
									"CommThreadManager::_push on PLock",
									"CommMsgQueue::push",
									"PQueue:push - on lock",
									"PQueue:push - on push",
									"PQueue:push - on event",
									"PQueue:push - on exit (unlock)"
								};
								for (int j = 0;j<sizeof(tickCounterInOneMsgSub)/sizeof(tickCounterInOneMsgSub[0]);j++)
								{
									PLog("   %40s: ticks=%d", names[j], tickCounterInOneMsgSub[j]);
									tickCounterInOneMsgSub[j]=0;
								}
							}

							if( _isRMsg( i ) )
							{
								totalTicksR += tickCounter[i];
								totalTicksAfterLockR += tickCounterAfterLock[i];
								totalEntersR += enterCounter[i];
							}
							else
							{
								totalTicksW += tickCounter[i];
								totalTicksAfterLockW += tickCounterAfterLock[i];
								totalEntersW += enterCounter[i];
							}
						}
						tickCounter[i]=0;
						tickCounterAfterLock[i]=0;
						enterCounter[i]=0;
					}
					for (size_t i = 0;i<sizeof(enterCounterTimer)/sizeof(enterCounterTimer[0]);i++)
					{
						static const char* const names[TIMER_PERFORMANCE_COUNTERS_SIZE]=
						{
							"_TIMER_PHYSRECONNECT",
							"_TIMER_RECONN",
							"_TIMER_RESUBSCR",
							"_TIMER_DELSUBSCR",
							"_TIMER_SUBSCR_COUNTING",
							"_TIMER_PERFORMANCE_TRAFFIC_C",
							"_TIMER_PERFORMANCE_COUNTING"
						};
						if (enterCounterTimer[i])
						{
							PLog("%28s: ticks=%d, ticksAfterLock=%d, number=%d", names[i], tickCounterTimer[i], tickCounterAfterLockTimer[i], enterCounterTimer[i]);

							if( _isRTimer( i ) )
							{
								totalTicksR += tickCounterTimer[i];
								totalTicksAfterLockR += tickCounterAfterLockTimer[i];
								totalEntersR += enterCounterTimer[i];
							}
							else
							{
								totalTicksW += tickCounterTimer[i];
								totalTicksAfterLockW += tickCounterAfterLockTimer[i];
								totalEntersW += enterCounterTimer[i];
							}
						}
						tickCounterTimer[i]=0;
						tickCounterAfterLockTimer[i]=0;
						enterCounterTimer[i]=0;
					}

					PLog("CommRoutingTable counters total (%d msek): ticksR=%u, ticksW=%u, afterLockR=%u, afterLockW=%u, numberR=%u, numberW=%u, contentions=%u", SYSTEMTICKS()-whenStart, totalTicksR, totalTicksW, totalTicksAfterLockR, totalTicksAfterLockW, totalEntersR, totalEntersW, cs.getContentions());
					_totalTicksAfterLock = totalTicksAfterLockR + totalTicksAfterLockW;
					_totalMessages = totalEntersR + totalEntersW;
					_maxThreadTime = 0;
					try
					{
						for( _ManagerList::const_iterator iter = managers.begin() ; iter != managers.end() ; ++iter )
						{
							PLog("Manager %s :", (*iter)->whoAreYou());
							UINT32 _max = (*iter)->printCounters();
							if (_max > _maxThreadTime) 
								_maxThreadTime = _max;
						}
					}
					catch (...)
					{
						PLog("Catch error in _TIMER_PERFORMANCE_COUNTING - stop counting");
						doCounting = false;
						break;
					}

					CommMsgBody timerMsg;
					timerMsg.composeUINT32(SYSTEMTICKS());
					_CommMsg tmpMsg( _TIMER_PERFORMANCE_COUNTING, 0, timerMsg );
					timerThread->push(tmpMsg, optTimeoutPerformanceCounting );
				}
				else
				{
					PLog("Performance counting has been turned off");
					memset((void *)tickCounter,0,sizeof(tickCounter));
					memset((void *)enterCounter,0,sizeof(enterCounter));
					memset((void *)tickCounterAfterLock, 0, sizeof(tickCounterAfterLock));
					memset((void *)tickCounterInOneMsg, 0, sizeof(tickCounterInOneMsg));
					memset(tickCounterInOneMsgSub, 0, sizeof(tickCounterInOneMsgSub));
					memset(tickCounterTimer, 0, sizeof(tickCounterTimer));
					memset(tickCounterAfterLockTimer, 0, sizeof(tickCounterAfterLockTimer));
					memset(enterCounterTimer, 0, sizeof(enterCounterTimer));
				}
			}
			break;

		default:
			PLog( "CommRoutingTable::_processTimerMessage: unknown %02X msg.type - ignored", msg.type);
			break;
		}
	}
	catch( PFatalError& err )
	{
		PLog( "Fatal error exception in CommRoutingTable::_processTimerMessage() while processing msg with type %02X: '%s'", msg.type, err.why() );
		if (exitOnFatalError)
		{
			FATALEXIT();
		}
	}
	catch( PSystemError& err  )
	{
		PLog( "System Exception in CommRoutingTable::_processTimerMessage() while processing msg with type %02X: '%s'", msg.type,  err.why() );
	}
	catch( PError& err  )
	{
		PLog( "Exception in CommRoutingTable::_processTimerMessage() while processing msg with type %02X: '%s'", msg.type, err.why() );
	}

	UINT32 t1 = SYSTEMTICKS();
	if( t1 - t0 > TICKS_PER_SECOND )
		PLog( "CommRoutingTable::_processTimerMessage(%02X): long processing (%u ticks)", msg.type, t1 - t0 );

	if (doCounting)
	{
		if (index>=0 && index < TIMER_PERFORMANCE_COUNTERS_SIZE)
		{
			tickCounterTimer[index] += t1 - t0_1;
			tickCounterAfterLockTimer[index] += t1 - t0;
			enterCounterTimer[index]++;
		}
	}
}

void CommRoutingTable::_dumpLogicalConnection(const _Conn* conn) const
{
	PLog("Dump=== Connection: '%s'/'%s'/'%s', state=%d, priority=%d, cliThisId=%x, srvThisId=%x, cliId=%x, srvId=%x, sessionId=%x, physCli='%s', physSrv='%s', muxCli=%d, muxCliChannelId=%x, muxSrv=%d, muxSrvChannelId=%x, wantQualityUpdates=%s,wantIsAlive=%s,inPreConnectedList=%s",
		conn->server.c_str(),
		conn->serverObject.c_str(),
		conn->serverChannel.c_str(),
		conn->state,
		(int)conn->priority,
		conn->cliThisId,
		conn->srvThisId,
		conn->cliId,
		conn->srvId,
		conn->sessionId,
		(conn->cli?conn->cli->address.c_str():"null"),
		(conn->srv?conn->srv->address.c_str():"null"),
		conn->muxCli,
		conn->muxCliChannelId,
		conn->muxSrv,
		conn->muxSrvChannelId,
		(conn->wantQualityUpdates?"yes":"no"),
		(conn->wantIsAlive?"yes":"no"),
		(conn->inPreConnectedList?"yes":"no"));
}

void CommRoutingTable::_dumpSubscription(const _Subscr* subscr, bool detailed) const
{
	PLog("Dump=== Subscription: '%s'/'%s'/'%s', state=%d, priority=%d, srvThisId=%x, srvId=%x, physSrv='%s', muxSrv=%d, muxSrvChannelId=%x, leafSize=%d, frozen=%s, frozenCounter=%d, timerPost=%s, version=%d",
		subscr->server,
		subscr->serverObject,
		subscr->serverChannel,
		subscr->state,
		(int)subscr->priority,
		subscr->srvThisId,
		subscr->srvId,
		(subscr->srv?subscr->srv->address.c_str():"null"),
		subscr->muxSrv,
		subscr->muxSrvChannelId,
		subscr->cli.size(),
		(subscr->frozen?"yes":"no"),
		subscr->frozenCounter,
		(subscr->timerWasPost?"yes":"no"),
		subscr->protocolVersion);
	if (detailed)
	{
		for( _SubscrLeafSetByCli::const_iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
		{
			const _SubscrLeaf* leaf = static_cast<_SubscrLeaf*>(*iter);
			_dumpSubscriptionLeaf(leaf);
		}
	}
}

void CommRoutingTable::_dumpSubscriptionLeaf(const _SubscrLeaf* subscrLeaf) const
{
	PLog("Dump=== Leaf: Subscription: '%s'/'%s'/'%s', cliThisId=%x, cliId=%x, physSrv='%s', muxCli=%d, muxCliChannelId=%x, sync=%s, frozen=%s,version=%d",
		(subscrLeaf->subscr ? subscrLeaf->subscr->server:"null"),
		(subscrLeaf->subscr ? subscrLeaf->subscr->serverObject:"null"),
		(subscrLeaf->subscr ? subscrLeaf->subscr->serverChannel:"null"),
		subscrLeaf->cliThisId,
		subscrLeaf->cliId,
		(subscrLeaf->cli?subscrLeaf->cli->address.c_str():"null"),
		subscrLeaf->muxCli,
		subscrLeaf->muxCliChannelId,
		(subscrLeaf->sync?"yes":"no"),
		(subscrLeaf->frozen?"yes":"no"),
		subscrLeaf->protocolVersion);
}

void CommRoutingTable::_dumpSubscriptionLight(const _SubscrLight* subscr, bool detailed) const
{
	PLog("Dump=== Subscription light: '%s'/'%s'/'%s', state=%d, priority=%d, srvThisId=%x, srvId=%x, physSrv='%s', muxSrv=%d, muxSrvChannelId=%x, leafSize=%d",
		subscr->server,
		subscr->serverObject,
		subscr->serverChannel,
		subscr->state,
		(int)subscr->priority,
		subscr->srvThisId,
		subscr->srvId,
		(subscr->srv?subscr->srv->address.c_str():"null"),
		subscr->muxSrv,
		subscr->muxSrvChannelId,
		subscr->cli.size());
	if (detailed)
	{
		for( _SubscrLeafSetByCli::const_iterator iter = subscr->cli.begin() ; iter != subscr->cli.end() ; ++iter )
		{
			_dumpSubscriptionLightLeaf(*iter);
		}
	}
}

void CommRoutingTable::_dumpSubscriptionLightLeaf(const _SubscrLightLeaf* subscrLeaf) const
{
	PLog("Dump=== Leaf: Subscription light: '%s'/'%s'/'%s', cliThisId=%x, cliId=%x, physSrv='%s', muxCli=%d, muxCliChannelId=%x",
		(subscrLeaf->subscr ? subscrLeaf->subscr->server:"null"),
		(subscrLeaf->subscr ? subscrLeaf->subscr->serverObject:"null"),
		(subscrLeaf->subscr ? subscrLeaf->subscr->serverChannel:"null"),
		subscrLeaf->cliThisId,
		subscrLeaf->cliId,
		(subscrLeaf->cli?subscrLeaf->cli->address.c_str():"null"),
		subscrLeaf->muxCli,
		subscrLeaf->muxCliChannelId);
}


void CommRoutingTable::_physDump(const _PhysConn * physConn, const char * name, bool detailed) const
{
	connDump(physConn,name);
	PLog("Dump=== Client connections:");
	for( _ConnSetByCli::const_iterator iter = physConn->cliConns.begin() ; iter != physConn->cliConns.end() ; ++iter )
	{
		_dumpLogicalConnection(*iter);
	}
	PLog("Dump=== Server connections:");
	for( _ConnSetBySrv::const_iterator iters = physConn->srvConns.begin() ; iters != physConn->srvConns.end() ; ++iters )
	{
		_dumpLogicalConnection(*iters);
	}
	PLog("Dump=== Subscriptions:");
	for( _SubscrSet::const_iterator siter = physConn->cliSubscrs.begin() ; siter != physConn->cliSubscrs.end() ; ++siter )
	{
		_dumpSubscription(*siter,detailed);
	}

	PLog("Dump=== Subscription leaves:");
	for( _SubscrLeafSet::const_iterator iterl = physConn->srvSubscrs.begin() ; iterl != physConn->srvSubscrs.end() ; ++iterl )
	{
		_dumpSubscriptionLeaf(*iterl);
	}
	PLog("Dump=== Subscriptions light:");
	for( _SubscrLightSet::const_iterator siterl = physConn->cliSubscrsLight.begin() ; siterl != physConn->cliSubscrsLight.end() ; ++siterl )
	{
		_dumpSubscriptionLight(*siterl,detailed);
	}

	PLog("Dump=== Subscription light leaves:");
	for( _SubscrLightLeafSet::const_iterator iterll = physConn->srvSubscrsLight.begin() ; iterll != physConn->srvSubscrsLight.end() ; ++iterll )
	{
		_dumpSubscriptionLightLeaf(*iterll);
	}
}



void CommRoutingTable::_physConnectionDump(const char * address,int type, bool detailed) const
{
	const _PhysConn * physConn = 0;
	PString name;
	if (type == 0)
	{
		// outbound connection
		name.assign("Outbound");
		_PhysConnStringMap::const_iterator found = outboundConns.find( address );
		if( found != outboundConns.end() )
		{
			physConn = (*found).second;
		}
	}
	else if (type == 1)
	{
		// inbound connection
		name.assign("Inbound");
		for( _PhysConnSet::const_iterator it = inboundConns.begin(); it != inboundConns.end() ; ++it )
		{
			const _PhysConn* physConn_ = *it;
			if (!strcmp(physConn_->address,address))
			{
				physConn = physConn_;
				break;
			}
		}
	}
	else if (type == 2)
	{
		// local connection
		_PhysConnMuxStringMap::const_iterator found = localConns.find( address );
		name.assign("Local");
		if( found != localConns.end() )
		{
			physConn = (*found).second.physConn;
			name.append(":'").append((*found).first).append("', mux=").appendInt((*found).second.mux).append(", muxChannelId=").appendHexInt((*found).second.muxChannelId);
		}
	}
	else
	{
		PLog("Dump=== Connection %s - incorrect request code %d", address,type);
		return;
	}
	if (!physConn)
	{
		PLog("Dump=== Connection %s not found", address);
		return;
	}
	_physDump(physConn,name.c_str(),detailed);
}



void CommRoutingTable::_tableSubscrForPhysConnectionDump(int num, const char * objectName, const char * channelName) const
{
	for( _PhysConnSet::const_iterator it = inboundConns.begin(); it != inboundConns.end() ; ++it )
	{
		const _PhysConn* physConn = *it;
		int count=0;
		for( _SubscrLeafSet::const_iterator siter = physConn->srvSubscrs.begin() ; siter != physConn->srvSubscrs.end() ; ++siter )
		{
			const _SubscrLeaf* subscrLeaf = *siter;
			if (subscrLeaf->subscr &&  PString::startsWith(subscrLeaf->subscr->serverObject,objectName) && 
				(!channelName || ( subscrLeaf->subscr->serverChannel && PString::startsWith( subscrLeaf->subscr->serverChannel,channelName)))
			   )
			{
				count++;
			}
		}
		if (count >= num)
		{
			PLog("Dump=== number of subscriptions exceeds %d", num);
			_physDump(physConn,"Inbound",false);
		}
	}
}

void CommRoutingTable::_tableSubscrForPhysConnectionDumpByRate(UINT32 resubscrNumber, UINT32 resubscrRate, UINT32 lastResubscr, UINT32 norma) const
{

	for( _PhysConnSet::const_iterator it = inboundConns.begin(); it != inboundConns.end() ; ++it )
	{
		const _PhysConn* physConn = *it;
		if ( (!resubscrNumber || physConn->subscrCounter >= resubscrNumber) &&
			 (!resubscrRate || physConn->subscrRate(norma) >= resubscrRate) &&
			 (!lastResubscr || (SYSTEMTICKS() - physConn->lastSubscrTime) <= lastResubscr)
		   )	 
		{
			PLog("Dump=== subscription rate exceeds treshold");
			_physDump(physConn,"Inbound",false);
		}
	}
}


void CommRoutingTable::_cliConnectionDumpById(UINT32 thisId) const
{
	const _Conn* const* connPtr = cliConnMap.find( thisId );
	if( connPtr == 0 )
	{
		PLog( "Dump=== Connection %x not found",thisId);
		return;
	}
	_dumpLogicalConnection(*connPtr);
}

void CommRoutingTable::_cliConnectionDumpByName(const char * server, const char * serverObject, const char * serverChannel) const
{
	int i;
	for( const _Conn* const * c = cliConnMap.firstIndex( i ); c ; c = cliConnMap.nextIndex( i ) )
	{
		const _Conn* conn = *c;
		if (!strcmp(conn->server.c_str(),server))
		{
			if (!*serverObject || !strcmp(conn->serverObject.c_str(),serverObject))
			{
				if (!*serverChannel || !strcmp(conn->serverChannel.c_str(),serverChannel))
				{
					_dumpLogicalConnection(conn);
				}
			}
		}
	}
}

void CommRoutingTable::_srvConnectionDumpById(UINT32 thisId) const
{
	const _Conn* const * connPtr = srvConnMap.find( thisId );
	if( connPtr == 0 )
	{
		PLog( "Dump=== Connection %x not found",thisId);
		return;
	}
	_dumpLogicalConnection(*connPtr);
}

void CommRoutingTable::_srvConnectionDumpByName(const char * server, const char * serverObject, const char * serverChannel) const
{
	int i;
	for( const _Conn* const* c = srvConnMap.firstIndex( i ); c ; c = srvConnMap.nextIndex( i ) )
	{
		const _Conn* conn = *c;
		if (!strcmp(conn->server.c_str(),server))
		{
			if (!*serverObject || !strcmp(conn->serverObject.c_str(),serverObject))
			{
				if (!*serverChannel || !strcmp(conn->serverChannel.c_str(),serverChannel))
				{
					_dumpLogicalConnection(conn);
				}
			}
		}
	}
}

void CommRoutingTable::_srvSubscriptionDumpById(UINT32 thisId,bool detailed) const
{
	const _Subscr* const* subscrPtr = srvSubscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "Dump=== Subscription %x not found",thisId);
		return;
	}
	_dumpSubscription(*subscrPtr,detailed);
}

void CommRoutingTable::_srvSubscriptionDumpByName(const char * server, const char * serverObject, const char * serverChannel, bool detailed) const
{
	const _Subscr* subscr = nameSubscrMap.findByName( server, serverObject, serverChannel );
	if( ! subscr )
	{
		PLog( "Dump=== Subscription '%s'/'%s'/'%s' not found",server,serverObject,serverChannel);
		return;
	}
	_dumpSubscription(subscr,detailed);
}

void CommRoutingTable::_srvSubscriptionDumpByNameEx(const char * server, const char * serverObject, const char * serverChannel, bool detailed) const
{
	for( _SubscrSetByName::const_iterator it = nameSubscrMap.begin(); it != nameSubscrMap.end() ; ++it )
	{
		const _Subscr* subscr = *it;
		if (!strcmp(subscr->server,server))
		{
			if (!*serverObject || !strcmp(subscr->serverObject,serverObject))
			{
				if (!*serverChannel || !strcmp(subscr->serverChannel,serverChannel))
				{
					_dumpSubscription(subscr,detailed);
				}
			}
		}
	}
}

void CommRoutingTable::_cliSubscriptionLeafDumpById(UINT32 thisId) const
{
	const _SubscrLeaf* const* subscrPtr = cliSubscrMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "Dump=== Subscription %x not found",thisId);
		return;		
	}
	_dumpSubscriptionLeaf(*subscrPtr);
}

void CommRoutingTable::_srvSubscriptionLightDumpById(UINT32 thisId,bool detailed) const
{
	const _SubscrLight* const* subscrPtr = srvSubscrLightMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "Dump=== Light subscription %x not found",thisId);
		return;
	}
	_dumpSubscriptionLight(*subscrPtr,detailed);
}

void CommRoutingTable::_srvSubscriptionLightDumpByName(const char * server, const char * serverObject, const char * serverChannel,bool detailed) const
{
	const _SubscrLight* subscr = nameSubscrLightMap.findByName( server, serverObject, serverChannel );
	if( subscr == 0 )
	{
		PLog( "Dump=== Light subscription '%s'/'%s'/'%s' not found",server,serverObject,serverChannel);
		return;
	}
	_dumpSubscriptionLight(subscr, detailed);
}

void CommRoutingTable::_srvSubscriptionLightDumpByNameEx(const char * server, const char * serverObject, const char * serverChannel,bool detailed) const
{
	for( _SubscrLightSetByName::const_iterator it = nameSubscrLightMap.begin(); it != nameSubscrLightMap.end() ; ++it )
	{
		const _SubscrLight* subscr = *it;
		if (!strcmp(subscr->server,server))
		{
			if (!*serverObject || !strcmp(subscr->serverObject,serverObject))
			{
				if (!*serverChannel || !strcmp(subscr->serverChannel,serverChannel))
				{
					_dumpSubscriptionLight(subscr,detailed);
				}
			}
		}
	}
}

void CommRoutingTable::_cliSubscriptionLightLeafDumpById(UINT32 thisId) const
{
	const _SubscrLightLeaf* const* subscrPtr = cliSubscrLightMap.find( thisId );
	if( subscrPtr == 0 )
	{
		PLog( "Dump=== Light subscription %x not found",thisId);
		return;		
	}
	_dumpSubscriptionLightLeaf(*subscrPtr);
}

void CommRoutingTable::dumpEx(CommMsgParser & parser)
{
	PWLock lock( cs ); // Write lock - to prevent log interruption by other messages
	INT32 code;
	parser.parseINT32(code);
	switch(code)
	{
	case 0: // dump physical connection
		{
			const char * address;
			INT32 type; // 0 - otbound, 1-inbound, 2-local
			bool detailed = false;
			parser.parseString(address)
				.parseINT32(type);
			PLog( "Dump=== Physical connection '%s'", address);
			if (!parser.parseEnded())
				parser.parseBOOL(detailed);
			_physConnectionDump(address,type,detailed);
		}
		break;
	case 1: // dump client connection by ID
		{
			UINT32 thisId;
			parser.parseUINT32(thisId);
			PLog( "Dump=== client connection by ID=%x",thisId);
			_cliConnectionDumpById(thisId);
		}
		break;
	case 2: // dump client connection by address
		{
			const char * server;
			const char * serverObject;
			const char * serverChannel;
			parser.parseString(server)
				.parseString(serverObject)
				.parseString(serverChannel);
			PLog( "Dump=== client connection by address '%s'/'%s'/'%s'", server,serverObject,serverChannel);
			_cliConnectionDumpByName(server,serverObject,serverChannel);
		}
		break;
	case 3: // dump server connection by ID
		{
			UINT32 thisId;
			parser.parseUINT32(thisId);
			PLog( "Dump=== server connection by ID=%x",thisId);
			_srvConnectionDumpById(thisId);
		}
		break;
	case 4: // dump server connection by address
		{
			const char * server;
			const char * serverObject;
			const char * serverChannel;
			parser.parseString(server)
				.parseString(serverObject)
				.parseString(serverChannel);
			PLog( "Dump=== server connection by address '%s'/'%s'/'%s'", server,serverObject,serverChannel);
			_srvConnectionDumpByName(server,serverObject,serverChannel);
		}
		break;
	case 5: // dump subscription by ID
		{
			UINT32 thisId;
			parser.parseUINT32(thisId);
			bool detailed = false;
			PLog( "Dump=== subscription by ID=%x",thisId);
			if (!parser.parseEnded())
				parser.parseBOOL(detailed);
			_srvSubscriptionDumpById(thisId,detailed);
		}
		break;
	case 6:  // dump subscription by address
		{
			const char * server;
			const char * serverObject;
			const char * serverChannel;
			parser.parseString(server)
				.parseString(serverObject)
				.parseString(serverChannel);
			PLog( "Dump=== subscription by address '%s'/'%s'/'%s'", server,serverObject,serverChannel);
			bool detailed = false;
			if (!parser.parseEnded())
				parser.parseBOOL(detailed);

			_srvSubscriptionDumpByName(server,serverObject,serverChannel,detailed);
		}
		break;
	case 7: // dump subscription leaf by ID
		{
			UINT32 thisId;
			parser.parseUINT32(thisId);
			PLog( "Dump=== subscription leaf by ID=%x",thisId);
			_cliSubscriptionLeafDumpById(thisId);
		}
		break;
	case 8:  // dump all subscriptions by address
		{
			const char * server;
			const char * serverObject;
			const char * serverChannel;
			parser.parseString(server)
				.parseString(serverObject)
				.parseString(serverChannel);
			PLog( "Dump=== all subscriptions by address '%s'/'%s'/'%s'", server,serverObject,serverChannel);
			bool detailed = false;
			if (!parser.parseEnded())
				parser.parseBOOL(detailed);

			_srvSubscriptionDumpByNameEx(server,serverObject,serverChannel,detailed);
		}
		break;
	case 9:  // dump light subscription by address
		{
			const char * server;
			const char * serverObject;
			const char * serverChannel;
			parser.parseString(server)
				.parseString(serverObject)
				.parseString(serverChannel);
			PLog( "Dump=== light subscription by address '%s'/'%s'/'%s'", server,serverObject,serverChannel);
			bool detailed = false;
			if (!parser.parseEnded())
				parser.parseBOOL(detailed);

			_srvSubscriptionLightDumpByName(server,serverObject,serverChannel,detailed);
		}
		break;
	case 10: // dump light subscription leaf by ID
		{
			UINT32 thisId;
			parser.parseUINT32(thisId);
			PLog( "Dump=== light subscription leaf by ID=%x",thisId);
			_cliSubscriptionLightLeafDumpById(thisId);
		}
		break;
	case 11:  // dump all light subscriptions by address
		{
			const char * server;
			const char * serverObject;
			const char * serverChannel;
			parser.parseString(server)
				.parseString(serverObject)
				.parseString(serverChannel);
			PLog( "Dump=== all light subscriptions by address '%s'/'%s'/'%s'", server,serverObject,serverChannel);
			bool detailed = false;
			if (!parser.parseEnded())
				parser.parseBOOL(detailed);

			_srvSubscriptionLightDumpByNameEx(server,serverObject,serverChannel,detailed);
		}
		break;
	case 12:  // dump phys connections with number of subscriptions exceeds num
		{
			const char * serverObject;
			const char * serverChannel = 0;
			int num;
			parser.parseINT32(num)
				.parseString(serverObject);
			if (!parser.parseEnded())
				parser.parseString(serverChannel);
			PLog( "Dump=== all phys connections with number of subscriptions '%s'*/'%s' exceeds %d", serverObject,serverChannel, num);

			_tableSubscrForPhysConnectionDump(num,serverObject,serverChannel);
		}
		break;
	case 13:  // dump phys connections with subscribe rate exceeds 
		{
			UINT32 resubscrNumber;
			UINT32 resubscrRate;
			UINT32 lastResubscr;
			UINT32 norma;  

			parser.parseUINT32(resubscrNumber)
				  .parseUINT32(resubscrRate)
				  .parseUINT32(lastResubscr)
				  .parseUINT32(norma);

			PLog( "Dump=== all phys connections with number of table subscriptions exceeds %d, rate exceeds %d and last subscription less than %d, norma %d ", resubscrNumber, resubscrRate, lastResubscr, norma);
			_tableSubscrForPhysConnectionDumpByRate(resubscrNumber, resubscrRate, lastResubscr, norma);
		}
		break;

	default:
		PLog("Dump=== Dump: unknown request code %d",code);
		break;
	}
}

void CommRoutingTable::setLocationPriority( const char* locationName, int priority )
{
	if( locationName && *locationName && ( priority >= 0 ) )
	{
		{
			PWLock cfgLock( configLock );
			const pair< PStringMap< _ConnectProperty >::iterator, bool > r = _writableConfig.locationPropertyMap.insert( make_pair( locationName, _ConnectProperty( priority ) ) );
			if( !r.second )
			{
				(*r.first).second.priority = priority;
			}
		}
		PTRACE4( "setLocationPriority::Location %s added, priority set to %d", locationName, priority );
	}
}

void CommRoutingTable::addAlias( const char* alias, const char * inManager_, const char * object_ )
{
	PWLock cfgLock( configLock );
	_writableConfig.addAlias( alias, inManager_, object_ );
}

void CommRoutingTable::addNotAlias( const char* alias, const char * inManager_ )
{
	PWLock cfgLock( configLock );
	_writableConfig.addNotAlias( alias, inManager_ );
}

void CommRoutingTable::addRule( const CommRule& rule )
{
	PWLock cfgLock( configLock );
	_writableConfig.addRule( rule );
}
void CommRoutingTable::getPhysConnections(CommMsgBody &reply)
{
	PWLock lock( cs );
	reply.composeUINT32(static_cast< UINT32 >( outboundConns.size() ));
	for( _PhysConnStringMap::const_iterator it = outboundConns.begin(); it != outboundConns.end() ; ++it )
	{

		reply.composeString((*it).second->address.c_str())
			 .composeUINT32( (*it).second->state )
			 .composeUINT32( (*it).second->connFailures)
			 .composeUINT32( (*it).second->failureTimeStatus)
			 .composeUINT32( (*it).second->firstFailureTime);
	}
}

void CommRoutingTable::getPhysConnectionsEx( CommMsgBody& body )
{
	PWLock lock( cs );

	body.composeUINT32( static_cast< UINT32 >( outboundConns.size() ) );

	for( auto&& iter : outboundConns )
	{
		CommMsgBody msgBody;
		msgBody.composeUINT32( iter.second->state )
			.composeString( iter.second->mgr->whoAreYou() )
			.composeString( iter.second->address )
			.composeUINT32( iter.second->connFailures )
			.composeUINT32( iter.second->failureTimeStatus )
			.composeUINT32( iter.second->firstFailureTime );

		body.composeMsgBody( msgBody );
	}
}
