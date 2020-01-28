#ifndef commrtable_h_included
#define commrtable_h_included


#ifndef COMMLIB_NO_STATISTICS
#define COMMLIB_CALCULATE_STATISTICS 1
#endif

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppinifile.h"
#include "ppcontainer.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commidmap.h"
#include "commtthread.h"
#include "commconn.h"
#include "commsubscr.h"


#ifdef COMMLIB_CALCULATE_STATISTICS
#define MAX_STAT_TRAFFIC_COUNTERS 30
#endif



class CommThreadManager;
struct _CommPhysicalConnection
{
	PString address;

	CommThreadManager* mgr;
	int thread;
	int slot;
	int channelId;//protection
	bool inbound;
	int connFailures;
	int lastErrorCode;
	UINT32 firstFailureTime;
	int failureTimeStatus;//0 - Ok, 1 - failed, -1 - locked
};

struct CommRule
{
	struct CommRuleConns
	{
		PString conns;
		int weight;
		PString location;
		CommRuleConns(const char * r, int weight_=0, const char * location_ = 0)
		{	
			conns.assign(r);
			weight=weight_;
			if (location_)
				location = location_;
		}
		CommRuleConns(const PString& r)
		{	
			conns.assign(r);
			weight=0;
		}
		operator const char*() const
		{
			return (const char *)conns;
		}
	};
	typedef list< CommRuleConns > _List;
	PString mask;
	PString inManager;
	_List conns;
	PString maskObject;
	PString maskChannel;
	vector< PString > maskPatterns;

	void parseMask();
};

class CommRoutingTable
{
private:
	typedef list< CommThreadManager* > _ManagerList;

	struct _PhysConn;

	UINT32 _rand;
	UINT32  _subscrUniqueId;
	int _randLocal()
	{
		_rand = _rand * 214013L + 2531011L;
		return ( _rand >> 16) & 0x7fff;
	}

	struct BalanceListItem
	{
		_PhysConn * conn;
		int weight;
		BalanceListItem(_PhysConn * conn_, int weight_)
		{
			conn=conn_;
			weight=weight_;
		}
	};

	struct _ConnectProperty
	{
		int priority;
		explicit _ConnectProperty( int pri_ ) : priority( pri_ ) {}
	};

	struct _ConnectAttemptItem
	{
		time_t time;
		explicit _ConnectAttemptItem(time_t time_) : time( time_ ) {}
	};
	typedef  deque< _ConnectAttemptItem > _ConnectAttempts;
	PStringMap< _ConnectAttempts > _locationFailMap;

#ifdef COMMLIB_CALCULATE_STATISTICS
	struct _TrafficStatistics
	{
		PString serverObject;
		PString server;
		int defaultCounterPosition;
		struct _TrafficStatisticsChannels
		{
			int counterPosition;
			PString serverChannel;
		};
		vector <_TrafficStatisticsChannels> serverChannels;
	};

	bool printTrafficLast;
	vector <_TrafficStatistics> _subscrTrafficStat;
	vector <_TrafficStatistics> _connTrafficStat;
	PString _subscrTrafficNames[MAX_STAT_TRAFFIC_COUNTERS];
	PString _connTrafficNames[MAX_STAT_TRAFFIC_COUNTERS];
	UINT32 _subscrTrafficStatCounters[6][MAX_STAT_TRAFFIC_COUNTERS]; // 0 - subscribe+defrost, 1 - update, 2 - from client to server, 3 - sent to the next server, 4 - received from the next server, 5 - updates from the next server
	UINT32 _connTrafficStatCounters[2][MAX_STAT_TRAFFIC_COUNTERS]; // 0 - from client to server, 1 - from server to client
	UINT32 _subscrTrafficStatPacketCounters[6][MAX_STAT_TRAFFIC_COUNTERS]; // 0 - subscribe+defrost, 1 - update, 2 - from client to server, 3 - sent to the next server, 4 - received from the next server, 5 - updates from the next server
	UINT32 _connTrafficStatPacketCounters[2][MAX_STAT_TRAFFIC_COUNTERS]; // 0 - from client to server, 1 - from server to client

	UINT32 _maxSubscrTrafficCounter;
	UINT32 _maxConnTrafficCounter;
	static int getCounterForTrafficStatistics(const vector <_TrafficStatistics> & trafficStat, const char * server, const char * serverObject, const char * serverChannel);
	static void addTrafficStatistics(vector <_TrafficStatistics> & trafficStat, UINT32 & maxCounter, const char * parameter);
	static void addTrafficStatisticsName(PString *names, const char * parameter);
#endif

	struct _Conn
	{
		PString server;
		PString serverObject;
		PString serverChannel;

		int state;//_StateConn*
		BYTE priority;
		UINT32 cliThisId;
		UINT32 srvThisId;
		UINT32 cliId;
		UINT32 srvId;
		UINT32 sessionId;
#ifdef COMMLIB_CALCULATE_STATISTICS
		int trafficCntPos;
#endif
		bool wantQualityUpdates;
		bool wantIsAlive;
		UINT32 version;
		UINT32 serverId;

		CommMsgBody request;//body of initial _COMM_MSGTYPE_CONNECT_REQUEST

		_PhysConn* cli;
		int muxCli;
		UINT32 muxCliChannelId;
		_PhysConn* srv;
		int muxSrv;
		UINT32 muxSrvChannelId;
		bool inPreConnectedList;
	};

	/*
	struct _ConnSetCmpCli : public binary_function< const _Conn*, const _Conn*, bool >
	{
	public:
	bool operator() ( const _Conn* a, const _Conn* b ) const
	{
	if( a->cli != b->cli )
	return a->cli < b->cli;
	if( a->muxCli != b->muxCli )//not sure whether its needed
	return a->muxCli < b->muxCli;
	return a < b;//to ensure uniqueness
	}
	};
	struct _ConnSetCmpSrv : public binary_function< const _Conn*, const _Conn*, bool >
	{
	public:
	bool operator() ( const _Conn* a, const _Conn* b ) const
	{
	if( a->srv != b->srv )
	return a->srv < b->srv;
	if( a->muxSrv != b->muxSrv )//not sure whether its needed
	return a->muxSrv < b->muxSrv;
	return a < b;//to ensure uniqueness
	}
	};
	typedef set< _Conn*, _ConnSetCmpCli > _ConnSetByCli;
	typedef set< _Conn*, _ConnSetCmpSrv > _ConnSetBySrv;
	--- removed as obsolete and causing data incostintency due to srv & muxSrv changes after putting element to set */
	typedef set< _Conn* > _ConnSetByCli;
	typedef set< _Conn* > _ConnSetBySrv;



	struct _SubscrLight;
	struct _SubscrLightLeaf
	{
		_SubscrLight* subscr;
		UINT32 cliThisId;
		UINT32 cliId;
		_PhysConn* cli;
		int muxCli;
		UINT32 muxCliChannelId;
		UINT32 count; 
	};
	typedef set< _SubscrLightLeaf* > _SubscrLightLeafSet;
	typedef set< _SubscrLightLeaf* > _SubscrLeafSetByCli;

	struct _SubscrKey
	{
		/* new */ char* server;
		/* new */ char* serverObject;
		/* new */ char* serverChannel;
	};

	struct _SubscrLight : public _SubscrKey
	{
		UINT32 _subscrUniqueId;

		int state;//_StateSubscr*
		BYTE priority;
		UINT32 srvThisId;
		UINT32 srvId;
		bool needCounting;
		_PhysConn* srv;
		int muxSrv;
		UINT32 muxSrvChannelId;
		_SubscrLeafSetByCli  cli;
#ifdef COMMLIB_CALCULATE_STATISTICS
		int trafficCntPos;
#endif
	};

	struct _SubscrSetCmpName : public binary_function< const _SubscrKey*, const _SubscrKey*, bool >
	{
		bool operator() ( const _SubscrKey* a, const _SubscrKey* b ) const
		{
			int cmp = strcmp( a->server, b->server );
			if( cmp )
				return cmp < 0;
			cmp = strcmp( a->serverObject, b->serverObject );
			if( cmp )
				return cmp < 0;
			return strcmp( a->serverChannel, b->serverChannel ) < 0;
		}
	};

	typedef set< _SubscrLight* > _SubscrLightSet;//unique
	class _SubscrLightSetByName : public set< _SubscrLight*, _SubscrSetCmpName > //unique
	{
		typedef set< _SubscrLight*, _SubscrSetCmpName > _Parent;
		const _SubscrLight* _findByName( const char* server, const char* object, const char* channel ) const
		{
			_SubscrKey key;
			key.server = const_cast< char* >( server ); // safe cast - will be used for search only
			key.serverObject = const_cast< char* >( object );
			key.serverChannel = const_cast< char* >( channel );
			_Parent::const_iterator found = find( static_cast< _SubscrLight* >( &key ) );
			if( found == end() )
				return 0;
			return *found;
		}

	public:
		const _SubscrLight* findByName( const char* server, const char* object, const char* channel ) const
		{
			return _findByName( server, object, channel );
		}
		_SubscrLight* findByName( const char* server, const char* object, const char* channel )
		{
			return const_cast< _SubscrLight* >( _findByName( server, object, channel ) );
		}
	};

	struct _Subscr;
	struct _SubscrLeaf : public _SubscrLightLeaf
	{
		enum { _frozenStateNormal = 0, _frozenStateFrozen=1, _frozenStateResync = 2};
		bool sync;//child sync with local (not necesserily with root). No request needed
		int frozen;
		CommMsgBody request;//body of initial _COMM_MSGTYPE_SUBSCRIPTION_REQUEST; exists only in _StateSubscrRequestSent/_StateSubscrPeerAssigned
		int protocolVersion;
	};

	typedef set< _SubscrLeaf* > _SubscrLeafSet;


	struct _Subscr : public _SubscrLight, CommSubscription
	{
		bool frozen;
		int  frozenCounter;
		int protocolVersion;
		UINT32 deleteSubscrTime;
		bool timerWasPost;
		void setPercentOfDelItemsToStore(int size)
		{
			percentOfDelItemsToStore = size;
		}
	};

	typedef set< _Subscr* > _SubscrSet;//unique
	class _SubscrSetByName : public set< _Subscr*, _SubscrSetCmpName > //unique
	{
		typedef set< _Subscr*, _SubscrSetCmpName > _Parent;
		const _Subscr* _findByName( const char* server, const char* object, const char* channel ) const
		{
			_SubscrKey key;
			key.server = const_cast< char* >( server ); // safe cast - will be used for search only
			key.serverObject = const_cast< char* >( object );
			key.serverChannel = const_cast< char* >( channel );
			_Parent::const_iterator found = find( static_cast< _Subscr* >( &key ) );
			if( found == end() )
				return 0;
			return *found;
		}

	public:
		const _Subscr* findByName( const char* server, const char* object, const char* channel ) const
		{
			return _findByName( server, object, channel );
		}
		_Subscr* findByName( const char* server, const char* object, const char* channel )
		{
			return const_cast< _Subscr* >( _findByName( server, object, channel ) );
		}
	};

	struct _CliPreConn
	{
		UINT32 cliId;
		int    srcMux;
		_CliPreConn(UINT32 cliId_, int srcMux_)
		{
			cliId=cliId_;
			srcMux=srcMux_;
		}
		//_CliPreConn(){}
		bool operator < (const _CliPreConn & other) const
		{
			if (cliId == other.cliId)
				return srcMux < other.srcMux;
			return cliId < other.cliId;
		}
	};

	typedef map<_CliPreConn, _Conn *> _CliPreConnMap;

	struct _PhysConn : public _CommPhysicalConnection
	{
		enum _State { _StatePhysConnDisc, _StatePhysConnIncomingDisc, _StatePhysConnRequestSent, _StatePhysConnOk, _StatePhysConnReconn };
		int state;
		int isLocal;//0 - far remote, 1 - near remote, 2 - local, 3 - internal
		int quality;
		bool qualityRequired;
		int ddosProtectionAttempts;

		_ConnSetByCli cliConns;
		_ConnSetBySrv srvConns;
		_CliPreConnMap cliPreConns;

		_SubscrSet		    cliSubscrs;
		_SubscrLightSet     cliSubscrsLight;
		_SubscrLeafSet      srvSubscrs;
		_SubscrLightLeafSet srvSubscrsLight;


		PString location;
		struct _pseudoConnection
		{
			PString server;
			PString serverObject;
			PString serverChannel;
		} pseudoConnection; 
		UINT32 subscrCounter;
		UINT32 lastSubscrTime;
		UINT32 connCreated;
		_PhysConn( const char* addr, CommThreadManager* mgr_, bool inbound_ );
		UINT32 subscrRate(UINT32 norma) const
		{
			if (!norma)
				return 0;
			UINT32 t = SYSTEMTICKS();
			if (t == connCreated)
				return 0;
			return PUMulDiv( subscrCounter, norma, t - connCreated );
		}
	};
	typedef set< _PhysConn* > _PhysConnSet;//unique
	typedef PStringMap< _PhysConn* > _PhysConnStringMap;//unique
	struct _PhysConnMux { _PhysConn* physConn; int mux; UINT32 muxChannelId; };
	typedef PStringMap< _PhysConnMux > _PhysConnMuxStringMap;//unique

	typedef list< CommRule > _RuleTable;
	enum { _MaxRuleParms = 2 /*not more than 10*/ };
	enum { _MaxRestrictionParms = 10};
	struct _RuleParm
	{
		const char* begin;
		const char* end;
	};
	typedef _RuleParm _RuleParms[ _MaxRuleParms ];
	typedef _RuleParm _RestrictionParms[ _MaxRestrictionParms ];


#ifndef COMM_CLIENT_ONLY
	struct VersionEncryption
	{
		PString key;
		PString version;
		VersionEncryption(const char *version_, const char * key_)
		{
			key = key_;
			version = version_;
		}
	};

	static bool _checkWildCharPattern(const char * pattern);
	static bool _compareWithParams(const char * pattern, const char * value, _RestrictionParms &parms, int & index);
	bool _internalEncryption(PString &ret, const _RestrictionParms & parm, int index) const;
	bool _makeForward( PString& ret, const _RestrictionParms& parms, int index, const char* forward ) const;
	struct _RestrictionRuleHeader
	{
		enum {eRestrictionAllow = 1,eRestrictionDeny=2,eRestrictionAllowRoute=3,eRestrictionDenyRoute=4,eRestrictionForward=5,eUseLast=0x20000,eUseFirst=0x40000,eUseAny = 0x80000 };
		PString destAddress;
		PString destObject;
		PString destType;
		PString initialName;
		PString initialValue;
		int type;
		PString forwardAddress;
		PString forwardObject;
		PString forwardType;
		bool init(const char *to, const char * from); 
		bool rule(const char *&r);
		bool isEqualDest(const char * _destServer, const char * _destObject, const char * _destType,
			_RestrictionParms &serverParm, int &serverIndex,
			_RestrictionParms & objectParm,int &objectIndex,
			_RestrictionParms & typeParm, int &typeIndex) const;
		void print(char * buffer) const;
		void debug() const;
		static void _parseAddress(PString & address_, PString &object_, PString & type_, const char * in_);
		static void _parseRule(PString & address_, PString &object_, PString & type_, const char * in_);
		static bool _checkForward(const char * dest, const char * frw);
	};

	struct _RestrictionRule : public _RestrictionRuleHeader
	{
		PString sourceIpAddress;
		bool addAddress(const char * address);
		bool isEqualIp(const char * ip) const ;
		int  isEqualSource(const vector <const char *> &ips) const;
		void debug() const;
		bool rule(const char *r);
	};
	struct _RestrictionDefault : public _RestrictionRuleHeader 
	{
		void debug() const;
		bool rule(const char *r);
	};

	static void _addRestriction( vector <_RestrictionRule> & rRule, vector <_RestrictionDefault> & dRule, const char * to, const char * from );
	bool _checkRestrictions(const  vector <_RestrictionRule> & rRule, const vector <_RestrictionDefault> & dRule,  PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, const char * inManager, int printDebug, PString* failReason = nullptr) const;
#endif //COMM_CLIENT_ONLY

private:
#define MSG_PERFORMANCE_COUNTERS_SIZE 256
	volatile UINT32 tickCounter[MSG_PERFORMANCE_COUNTERS_SIZE];
	volatile UINT32 tickCounterAfterLock[MSG_PERFORMANCE_COUNTERS_SIZE];
	volatile UINT32 enterCounter[MSG_PERFORMANCE_COUNTERS_SIZE];

	static bool _isRMsg( size_t n );
	static bool _isRTimer( size_t n );

#define TIMER_PERFORMANCE_COUNTERS_SIZE 7
	UINT32 tickCounterTimer[TIMER_PERFORMANCE_COUNTERS_SIZE];
	UINT32 tickCounterAfterLockTimer[TIMER_PERFORMANCE_COUNTERS_SIZE];
	UINT32 enterCounterTimer[TIMER_PERFORMANCE_COUNTERS_SIZE];

#define PERFORMANCE_COUNTERS_INONEMSG_SIZE 11
	volatile UINT32 tickCounterInOneMsg[PERFORMANCE_COUNTERS_INONEMSG_SIZE];
#define PERFORMANCE_COUNTERS_INONEMSGSUB_SIZE 12
	UINT32 tickCounterInOneMsgSub[PERFORMANCE_COUNTERS_INONEMSGSUB_SIZE];
	UINT32 _totalTicksAfterLock;
	UINT32 _totalMessages;
	UINT32 _maxThreadTime;
	UINT32 _statisticTimeouts;
	UINT32 _statisticDelays;
	UINT32 _statisticDisconnects;
	UINT32 _statisticConnectionDenied;
	UINT32 _statisticSubscriptionDenied;

	bool sendNewSubscriptionRequests;
	bool acceptNewSubscriptionRequests;
	int percentOfDelItemsToStore;
	int massCrashNumProcessesThreshold;
	int massCrashTimeThreshold;
	int autoRestartParamsResetTimerFreq;
	PString _ident;

	struct _Config
	{
		_Config();

#ifndef COMM_CLIENT_ONLY
		vector < _RestrictionRule > restrictionRulesConnection;
		vector < _RestrictionDefault > restrictionDefaultConnection;
		vector < _RestrictionRule > restrictionRulesSubscr;
		vector < _RestrictionDefault > restrictionDefaultSubscr;
		int restrictionConnectionDebug;
		int restrictionSubscriptionDebug;
		int restrictionConnectionTest;
		int restrictionForwardEnabled;
		int restrictionSubscriptionTest;

		vector<VersionEncryption> versionEncryption;
#endif // COMM_CLIENT_ONLY

		_RuleTable aliases;
		_RuleTable notAliases;
		_RuleTable rules;

		PStringMap< _ConnectProperty > locationPropertyMap;

		void addAlias( const char* alias, const char * inManager_, const char * object_ );
		void addNotAlias( const char* alias, const char * inManager_ );
		void addRule( const CommRule& rule );
#ifndef COMM_CLIENT_ONLY
		void addVersionEncryption( const char* ver, const char* key );
		void setRestrictionConnectionDebug(int n)
		{
			restrictionConnectionDebug = n;
		}
		void setRestrictionSubscriptionDebug(int n)
		{
			restrictionSubscriptionDebug = n;
		}
		void addRestrictionConnection( const char * to, const char * from );
		void addRestrictionSubscription( const char * to, const char * from );
#endif //COMM_CLIENT_ONLY
	};

	_Config _writableConfig;
	const _Config& _config;

	bool   doCounting;
	bool   doTrafficCounting;
	friend class CommThreadManager;
	PReadWriteLock cs;
	PReadWriteLock configLock;
	bool shuttingDown;

	CommIdMap< /* new */ _Conn* > cliConnMap;
	CommIdMap< _Conn* > srvConnMap;

	CommIdMap< _SubscrLeaf* > cliSubscrMap;
	CommIdMap< _Subscr* > srvSubscrMap;
	_SubscrSetByName /* new */ nameSubscrMap;

	CommIdMap< _SubscrLightLeaf* > cliSubscrLightMap;
	CommIdMap< _SubscrLight* > srvSubscrLightMap;
	_SubscrLightSetByName /* new */ nameSubscrLightMap;



	_PhysConnStringMap /* new */ outboundConns;
	_PhysConnMuxStringMap /* new */ localConns;
	_PhysConnSet /* new */ inboundConns;//not including localConns

	_ManagerList managers;
	_CommTimerThread* timerThread;

	//options:
	int optReconnTimeout;
	int optSubscrExpire;
	int optMaxPhysConnFailures;
    int optDisableAdjustFailureWeight;
	int optPhysConnFailuresTimeout;
	int optTimeoutSubscrCounting;
	int optTimeoutPerformanceCounting;

#ifdef COMMLIB_CALCULATE_STATISTICS
	int optTimeoutTrafficCounting;
#endif

	int optPhysConnLocationAttempts;
	int optLocationFailureExpiredPeriod;
	bool exitOnFatalError;

public:
	CommRoutingTable();
	~CommRoutingTable();

	void setOption( const char* optName, const char* optValue );
	void setLocationPriority( const char* locationName, int priority );
	void _setIdent(const char * ident)
	{
		_ident = ident;
	}
	const char * _getIdent() const
	{
		return _ident.c_str();
	}
	void addAlias( const char* alias, const char * inManager_ = 0, const char * object_ = 0  );
	void addNotAlias( const char* alias, const char * inManager_ );
	void addRule( const CommRule& rule );
	void _registerThreadManager( CommThreadManager& mgr );
	void start();
	void resetAllStats();//resets failure stats for physConns - in the future may also reset some other stats
	bool terminated();
	void shutdown();
	bool isShuttingDown() const {return shuttingDown;}
	bool waitUntilTerminated( int sec = 5 );
	void dynamicInit(PIniFile& iniFile);

	void dump();
	void dumpEx(CommMsgParser& parser);
	void cleanMgrConnections(CommThreadManager * mgr, int threadNum);
	void loggerInformation(CommMsgBody &body);
	void getPhysConnections(CommMsgBody &reply);

	int getMassCrashNumProcessesThreshold() const {return massCrashNumProcessesThreshold;}
	int getMassCrashTimeThreshold() const {return massCrashTimeThreshold;}
	int getAutoRestartParamsResetTimerFreq() const {return autoRestartParamsResetTimerFreq;}

public:
	typedef _PhysConn::_State PhysConnState;
	void getPhysConnectionsEx( CommMsgBody& body );

public:
	_CommPhysicalConnection* createIncomingPhysicalConnection( const char* addr, CommThreadManager* mgr );//creates in disconnected state
	void physicallyConnected( _CommPhysicalConnection* physConn, int thread, int slot, UINT32 channelId, int isLocal );//0 - far remote, 1 - near remote, 2 - local, 3 - internal
	void physicalConnectFailed( _CommPhysicalConnection* physConn, int failureWeight = 1, int errorCode = 0 );
	void setPhysicalConnectionQuality( _CommPhysicalConnection* physConn, int quality );//quality: 0 - 100
	void physicalConnIsAlive( _CommPhysicalConnection* physConn_ );
	void physicallyDisconnected( _CommPhysicalConnection* physConn, int errCode = 0 );
	void physicalReconnectBegin( _CommPhysicalConnection* physConn );
	void physicalReconnectEnd( _CommPhysicalConnection* physConn, int thread, int slot, UINT32 channelId );
	void routeIncomingMessage( _CommPhysicalConnection* srcPhysConn, int mux, UINT32 muxChannelId, _CommMsg& msg );
	bool _processTimerMessageRead( _CommMsg& msg );//for timerThread only
	void _processTimerMessage( _CommMsg& msg );//for timerThread only
	void serviceCall( _CommPhysicalConnection* physConn, UINT32 id, CommMsgBody * msg_body );
	bool localNameExists( const char* name );
	static void substituteAddr( PString& ret, const vector< CommRule >& rules, const char* addr );//makes addr substitution if it matches any of the rules. Only *conns.begin() is used from each rule

private:
	bool checkRestrictionsConnection(PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips,const char * inManager, PString* failReason = nullptr);
	bool checkRestrictionsSubscription(PString & destServer, PString & destObject, PString & destType, const vector<const char * >& ips, PString* failReason = nullptr);

	static void _readConfig( _Config& cfg, const PIniFile& iniFile );
	void _forceClientsDisconnect(_PhysConn * phys);
	void _startCountingRead( _SubscrLight* subscr, bool lightType, CommMsgBody& body );
	void _stopCountingRead( _SubscrLight* subscr, bool lightType, CommMsgBody& body );
	void _postCountingUpdateRead( const _SubscrLight* subscr, bool light );
	void connDump(const _PhysConn* physConnconst, const char *) const;
	void serviceCallTransport(_PhysConn*  physConn_,UINT32 id,CommMsgBody * msg_body);
	_Conn* _getSrvConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType );
	_Conn* _getSrvConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType );
	_Conn* _getSrvConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType );
	_Conn* _getCliConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType );
	_Conn* _getCliConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType );
	_Conn* _getCliConn( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType );
	_Conn* _lookForCliConn( UINT32 cliId, _PhysConn* srcPhysConn, int srcMux );

	_Subscr* _getSrvSubscr( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType );
	_Subscr* _getSrvSubscr( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType );
	_SubscrLight* _getSrvSubscrLight( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType );
	_SubscrLight* _getSrvSubscrLight( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType );

	_SubscrLeaf* _getCliSubscrLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType );
	_SubscrLeaf* _getCliSubscrLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType );
	_SubscrLightLeaf* _getCliSubscrLightLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, BYTE msgType );
	_SubscrLightLeaf* _getCliSubscrLightLeaf( int srcMux, _PhysConn* srcPhysConn, UINT32 thisId, UINT32 peerId, BYTE msgType );
	_SubscrLeaf* _lookForCliSubscrLeaf( UINT32 cliId, _PhysConn* srcPhysConn, int srcMux );
	_SubscrLightLeaf* _lookForCliSubscrLightLeaf( UINT32 cliId, _PhysConn* srcPhysConn, int srcMux );
	
	/* new */ _Conn* _createNewConn( const char* server, const char* serverObject, _PhysConn* cliPhysConn, int muxCli, UINT32 muxCliChannelId, UINT32 cliId, CommMsgBody& requestBody, _PhysConn* srvPhysConn, int muxSrv, UINT32 muxSrvChannelId, const char * serverConnType , UINT32 version, UINT32 serverId, BYTE priority );
	void _reconnect( _Conn* conn );
	void _delConn( _Conn* conn );

	/* new */ _Subscr* _createNewSubscr( const char* server, const char* serverObject, const char* serverChannel, _PhysConn* srvPhysConn, int muxSrv, UINT32 muxSrvChannelId );
	/* new */ _SubscrLight* _createNewSubscrLight( const char* server, const char* serverObject, const char* serverChannel, _PhysConn* srvPhysConn, int muxSrv, UINT32 muxSrvChannelId );
	_SubscrLeaf* _createNewSubscrLeaf( _Subscr* subscr, _PhysConn* cliPhysConn, int muxCli, UINT32 muxCliChannelId, UINT32 cliId, CommMsgBody& requestBody, int protocolVersion );
	_SubscrLightLeaf* _createNewSubscrLightLeaf( _SubscrLight* subscr, _PhysConn* cliPhysConn, int muxCli, UINT32 muxCliChannelId, UINT32 cliId);
	void _resubscribe( _Subscr* subscr );
	void _resubscribeLight( _SubscrLight* subscr );
	void _delSubscrLeaf( _SubscrLeaf* subscrLeaf );
	void _delSubscrLightLeaf( _SubscrLightLeaf* subscrLeaf );
	void _delSubscr( _Subscr* subscr );
	void _delSubscrLight( _SubscrLight* subscr );

	void _postConnRequest( _Conn* conn );//sets _StateConnRequestSent
	void _postSubscrRequest( _Subscr* subscr );//sets _StateSubscrRequestSent
	void _postSubscrLightRequest( _SubscrLight* subscr );
	void _postConnAccept( _Conn* conn );
	void _postSubscrAccept( const _SubscrLightLeaf* leaf, bool lightType );
	void _postTransitDisc( _PhysConn* physConn, int mux, UINT32 muxChannelId, BYTE msgType, BYTE priority, UINT32 peerId, UINT32 thisId );
	void _postToPhysConn( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m );
	void _postToPhysConn( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ );

	void _postToPhysConnRead( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m );
	void _postToPhysConnRead( _PhysConn* physConn, int mux, UINT32 muxChannelId, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ );

	void _postToSubscrLeafs( _SubscrLight* subscr, BYTE msgType, CommMsgBody& body );
	void _processSubscrRequest( _SubscrLeaf* leaf );
	void _processSubscrLightRequest( _SubscrLightLeaf* leaf );
	void _scheduleDelSubscr( _Subscr* subscr );
	void _scheduleDelSubscrLight( _SubscrLight* subscr );
	void _postCliQualityUpdate( _Conn* conn,int quality );
	void _postSrvQualityUpdate( _Conn* conn, int quality );
	void _postSrvPhysConnIsAlive( _Conn* conn );
	void _freezeSubscrLeaf( _SubscrLeaf* subscrLeaf );
	void _conditionalPostToSubscrLeafs( _Subscr* subscr, BYTE msgType, CommMsgBody& body, UINT32 * ticksCounter );
	void _conditionalPostToSubscrLightLeafs( _SubscrLight* subscr, BYTE msgType, CommMsgBody& body);
	void _defrostSubscrLeaf( _SubscrLeaf* subscrLeaf, CommMsgBody& requestBody );
	void _freezeSubscr( _Subscr* subscr );

	void _forceDisconnect( _PhysConn* physConn );

	static bool _ruleMatch( _RuleParms& parms, const CommRule & rule, const char* server, bool ignoreObject, const char* serverObject, const char* serverChannel, const char* inManager );
	static void _ruleMake( PString& ret, const _RuleParms& parms, const char* ruleConn );
	bool _isAlias( const char* server, const char* serverObject, const char* serverChannel, const char* inManager, PString* newObject, bool forSubscr ) const;
	bool _isAliasServer( const char* server ) const;
	_PhysConn* _balancePhysConn( const vector< BalanceListItem >& balanceList );
	void _balanceLocation( PStringSet& validLocations );
	void adjustFailureWeight();

	_PhysConn* _findLocalPhysConn( int& mux, UINT32& muxSrvChannelId, const char* serverObject, UINT16& errCode, const char*& errMsg );
	_PhysConn* _findOrCreateRemotePhysConn( int& mux, UINT32& muxSrvChannelId, const char* server, const char* serverObject,  const char * serverChannel, const char * inManager, UINT16& errCode, const char*& errMsg );
	_PhysConn* _findOrCreatePhysConn( int& mux, UINT32& muxSrvChannelId, const char* server, const char* serverObject, const char * serverChannel,  const char * inManager, bool forSubscr, UINT16& errCode, const char*& errMsg );

	void _cleanupPhysConnMux( _PhysConn* physConn, int mux );

	void _physicallyConnected( _CommPhysicalConnection* physConn_, int thread, int slot, UINT32 channelId, int isLocal );
	void _physicallyDisconnected( _CommPhysicalConnection* physConn_, int physConnState );

	void _physDump(const _PhysConn * physConn, const char * name, bool detailed) const;
	void _dumpLogicalConnection(const _Conn* conn) const;
	void _dumpSubscription(const _Subscr* subscr, bool detailed) const;
	void _dumpSubscriptionLeaf(const _SubscrLeaf* subscrLeaf) const;
	void _dumpSubscriptionLight(const _SubscrLight* subscr, bool detailed) const;
	void _dumpSubscriptionLightLeaf(const _SubscrLightLeaf* subscrLeaf) const;
	void _physConnectionDump(const char * address,int type,bool detailed) const;
	void _cliConnectionDumpById(UINT32 thisId) const;
	void _cliConnectionDumpByName(const char * server, const char * serverObject, const char * serverChannel) const;
	void _srvConnectionDumpById(UINT32 thisId) const;
	void _srvConnectionDumpByName(const char * server, const char * serverObject, const char * serverChannel) const;
	void _tableSubscrForPhysConnectionDump(int num, const char * objectName, const char * channelName) const;
	void _tableSubscrForPhysConnectionDumpByRate(UINT32 resubscrNumber, UINT32 resubscrRate, UINT32 lastResubscr, UINT32 norma) const;


	void _srvSubscriptionDumpById(UINT32 thisId,bool detailed) const;
	void _srvSubscriptionDumpByName(const char * server, const char * serverObject, const char * serverChannel,bool detailed) const;
	void _cliSubscriptionLeafDumpById(UINT32 thisId) const;
	void _srvSubscriptionDumpByNameEx(const char * server, const char * serverObject, const char * serverChannel,bool detailed) const;
	void _srvSubscriptionLightDumpById(UINT32 thisId,bool detailed) const;
	void _srvSubscriptionLightDumpByName(const char * server, const char * serverObject, const char * serverChannel,bool detailed) const;
	void _cliSubscriptionLightLeafDumpById(UINT32 thisId) const;
	void _srvSubscriptionLightDumpByNameEx(const char * server, const char * serverObject, const char * serverChannel,bool detailed) const;

	// PYR-65989 - Move options that may be reloaded dynamically to this method. This will be called by setOption on start, and by dynamicInit.
	bool _setDynamicOption( const char* optName, const char* optValue );

	// PYR-65989 - Clear traffic counter lists before reloading options.
#ifdef COMMLIB_CALCULATE_STATISTICS
	void _clearTrafficCounters();
#endif
};

//****************************************************************************

class _CommManageableThread : public PThread
{
private:
	bool exited;
	bool cleanup;
protected:
	CommMsgTimeControlQueue q;

	UINT64 userCpuUsage;
	UINT64 kernelCpuUsage;
	volatile UINT32 routingTableTime;
	volatile UINT32 afterLockTime;


public:
	_CommManageableThread();

public:
	virtual void push( /*non-const!*/ _CommMsg& msg )
	{
		q.push( msg );
	}
	virtual void push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_  )
	{
		q.push( msg, tickCounterMsg_  );
	}

	void exit(bool cleanup_=false);//MUST be called by overridden run()
	void setStatistic(UINT32 routingTableTime_,UINT32 afterLockTime_)
	{
		PInterlockedExchangeAdd(&routingTableTime, routingTableTime_);
		PInterlockedExchangeAdd(&afterLockTime, afterLockTime_);
	}
	void initCounters(int n)
	{
		if (getTimers(kernelCpuUsage,userCpuUsage) == false)
		{
			userCpuUsage=(UINT64)(-1);
			kernelCpuUsage=(UINT64)(-1);
		}
		else
		{
			PLog("Thread #%d (%d) initial values -- kernel = %u-%u,  user = %u-%u",n, getThreadId(), 
				(unsigned int)(kernelCpuUsage>>32),
				(unsigned int)(kernelCpuUsage&0xffffffff),
				(unsigned int)(userCpuUsage>>32),
				(unsigned int)(userCpuUsage&0xffffffff));
		}
	}
	virtual UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime)
	{
		UINT64 kernelT,userT;
		if (userCpuUsage == -1 || kernelCpuUsage == -1)
		{
			// counters are not initialized.
			initCounters(n);
			routingTableTime=0;
			afterLockTime=0;
			return 0;
		}
		else if (getTimers(kernelT,userT))
		{
			UINT32 totalCpuUsage;
			kernelCpuUsage = kernelT - kernelCpuUsage;
			userCpuUsage = userT - userCpuUsage;
			UINT32 autoReset;
			UINT32 manualReset;
			UINT32 setEvent;
			UINT32 contentions;
			q.getCounters(autoReset,manualReset,setEvent,contentions);

			PLog("Thread #%d (%d) -- kernel=%u-%u, user=%u-%u, routingTable=%u, afterLock=%u, aReset=%u, mReset=%u, sEvent=%u, contentions=%u ",n,getThreadId(), 
				(unsigned int)(kernelCpuUsage>>32),
				(unsigned int)(kernelCpuUsage&0xffffffff),
				(unsigned int)(userCpuUsage>>32),
				(unsigned int)(userCpuUsage&0xffffffff),
				routingTableTime,afterLockTime,autoReset,manualReset,setEvent,contentions);
			totalCpuUsage = (UINT32)( kernelCpuUsage + userCpuUsage) / 10000;
			kernelCpuUsage = kernelT;
			userCpuUsage = userT;
			routingTableTime=0;
			afterLockTime=0;
			return totalCpuUsage;
		}
		else
			return 0;
	}
	void shutdown();
	bool inStateOfExit() const
	{
		return exited;
	}
	bool isCleanupNeeded() const
	{
		return cleanup;
	}
	virtual void dump();
	virtual void rundump();
};

class CommThreadManager
{
protected:
	struct Thread
	{
		_CommManageableThread* thread;//0 means 'inactive'
		int maxSlots;
		int slotsLeft;
	};

public:
	enum { _flagPrepatch = 0x01, _flagVirtualPush = 0x02 };
	BYTE flags;
	CommRoutingTable& routingTable;

protected:
	//	PCriticalSection& cs;
	PReadWriteLock& cs;
	vector< Thread > threads;
	UINT32 channelIdGen;
	int maxNumberOfConnectionsPerThread;
	PString managerIdent;
	PString managerType;
	bool forceClientDisconnect;
	bool allowClientDisconnect;
	bool inStateOfClosedAccept;
	int gatherStatistic;
    int networkStatsLogThreshold;
	int connectAttemptsDDOSProtection;
	bool exitOnFatalError;
	PString baseThreadPriority;
	UINT32 qSize;
	UINT32 qTime;
	UINT32 qSlotSize;
	UINT32 qSlotTime;

public:
	CommThreadManager( CommRoutingTable& routingTable, BYTE flags_ = 0x00);
	virtual void setWhoAreYou(const char * managerIdent_)
	{
		managerIdent.assign("CommThreadManager:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	void setStatistic(_CommPhysicalConnection* physConn, UINT32 totalTime_,UINT32 afterLockTime_);

	virtual void setOption( const char* optName, const char* optValue );
	virtual void dynamicInit(PIniFile& iniFile);
	virtual void listenPort( int port ){}
	virtual void listenName( const char * name ){}
	virtual void acceptCancel(){PASSERT(0);}
	virtual bool acceptReinstate(){PASSERT(0);}

	UINT32 nextChannelId();
	virtual void dump();

	virtual void start();
	void _checkExitingThreads();
	void push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m );
	void push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ );
	void pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m );
	void pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ );

	virtual void virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) { PASSERT3( 0 ); }
	virtual void virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) { PASSERT3( 0 ); }
	virtual bool handles( const char* address ) const { return false; }
	virtual void parseSocketAddr( PString& retAddr, int& port, const char* address ) { PASSERT3( 0 ); }
	virtual void composeSocketAddr( PString& retAddr, const char* address, int port ) { PASSERT3( 0 ); }
	virtual void connect( _CommPhysicalConnection* physConn ) { PASSERT3( 0 ); }
	virtual void forceDisconnect( _CommPhysicalConnection* physConn ) = 0;
	const char * getType() const
	{
		return managerType.c_str();
	}
	virtual const char * whoAreYou() const 
	{
		if (managerIdent.length())
		{
			return managerIdent.c_str();
		}
		else
			return "CommThreadManager";
	} 
	virtual void loggerInformation(CommMsgBody &body) {}

	virtual void shutdown();
	virtual bool terminated();
	virtual ~CommThreadManager();

	virtual UINT32 printCounters();


public:
	static void _copyPatchMsg( _CommMsg& msg, const _CommMsg& srcMsg );
	void _push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m );
	void _push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ );
	void _pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m );
	void _pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ );

	bool isForceClientDisconnects() const
	{
		return forceClientDisconnect;
	}
	bool isAllowedClientDisconnects() const
	{
		return allowClientDisconnect;
	}
	bool isInStateOfClosedAccept() const
	{
		return inStateOfClosedAccept;
	}
	int isGatherStatistic() const
	{
		return gatherStatistic;
	}
	bool isExitOnFatalError() const
	{
		return exitOnFatalError;
	}
    int getNetworkStatsLogThreshold() const
    {
        return networkStatsLogThreshold;
    }
	int getConnectAttemptsDdosProtection() const
	{
		return connectAttemptsDDOSProtection;
	}
	int getMaxNumberOfConnectionsPerThread() const
	{
		return maxNumberOfConnectionsPerThread;
	}

protected:
	int _findSlot();
	int _allocThread();

private: // prohibit default assignment operator
	CommThreadManager& operator=( const CommThreadManager& other );
};

class CommThreadManagerWithTimerThread : public CommThreadManager
{
protected:
	typedef CommThreadManager _Parent;

	_CommTimerThread* timerThread;

	virtual void _pushToThread(_CommManageableThread* dummyThread_, _CommMsg& m) = 0;

public:
	CommThreadManagerWithTimerThread(CommRoutingTable& routingTable, BYTE flags_ = 0x00);

	void _timer(_CommMsg& msg);
	UINT32 _postTimer(_CommPhysicalConnection* physConn, _CommMsg& msg, int msec);
};

template< class Terminatable >
//Terminatable requirements: 
//  bool terminated()
void _CommWaitUntilTerminated( Terminatable& term )//waits infinite
{
	for(;;)
	{
		if( term.terminated() )
			return;
		PThread::sleep( 100 );
	}
}

template< class Terminatable >
//Terminatable requirements: 
//  bool terminated()
bool _CommWaitUntilTerminated( Terminatable& term, int sec )
{
	int n = sec * 10;
	for( int i=0; i < n ; ++i )
	{
		if( term.terminated() )
			return true;
		PThread::sleep( 100 );
	}
	return false;
}

#endif
