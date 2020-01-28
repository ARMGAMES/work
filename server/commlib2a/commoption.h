#ifndef commoption_h_included
#define commoption_h_included

#define COMM_OPTION_TIMEOUT_RECONN "reconntimeout"
#define COMM_OPTION_TIMEOUT_RECONN_DEFAULT 1000
#define COMM_OPTION_TIMEOUT_SUBSCREXPIRE "subcrexpire"
#define COMM_OPTION_TIMEOUT_SUBSCREXPIRE_DEFAULT 5000
#define COMM_OPTION_TIMEOUT_COUNTING_DEFAULT 10000
#define COMM_OPTION_TIMEOUT_PERFORMANCE_COUNTING_DEFAULT 60000
#define COMM_OPTION_TIMEOUT_TRAFFIC "traffictimer"
#define COMM_OPTION_TIMEOUT_TRAFFIC_COUNTING_DEFAULT 60000
#define COMM_OPTION_LOCATION_ATTEMPT_DEFAULT 0  
#define COMM_OPTION_LOCATION_FAILURE_EXPIRED_PERIOD_DEFAULT (30*60)   // 30 minutes
#define COMM_OPTION_MAXPHYSICALFAILURES "maxfailures"//default 0 - unlimited
#define COMM_OPTION_DISABLE_ADJUST_FAILURE_WEIGHT "disableadjustfailureweight"
#define COMM_OPTION_PHYSICALFAILURESTIMEOUT "failurestimeout"//default 0 - unlimited
#define COMM_OPTION_TIMEOUT_SUBSCRCOUNTING "counttimer"
#define COMM_OPTION_DO_PERFORMANCE_COUNTING "performance"
#define COMM_OPTION_PERFORMANCE_TIMER "performancetimer"
#define COMM_OPTION_EXITONFATALERROR  "exitonfatalerror" 
#define COMM_OPTION_NUMBEROFCONNECTIONSPERTHREAD   "maxconnectionsperthread" 
#define COMM_OPTION_GATHERSTATISTIC         "gatherstatistic" 
#define COMM_OPTION_BASETHREADPRIORITY		"basethreadpriority" 
#define COMM_OPTION_NETWORKSTATSLOGTHRESHOLD         "networkstatslogthreshold" 
#define COMM_OPTION_CONNECTATTEMPTSDDOSPROTECTION    "connectattempts" 

#define COMM_OPTION_ACCEPT_NEW_SUBSCRIPTION  "acceptnewsubscription"
#define COMM_OPTION_SEND_NEW_SUBSCRIPTION  "sendnewsubscription"
#define COMM_OPTION_PERCENT_OF_DELETED  "percentofdeleteditems"
#define COMM_OPTION_LOCATION_ATTEMPTS  "locationattempts"
//#define COMM_OPTION_LOCATION_PRIORITY  "locationpriority" // use setLocationPriority() instead
#define COMM_OPTION_LOCATION_FAILURE_EXPIRED_PERIOD "locationfailureexpired"
#define COMM_OPTION_MASS_CRASH_NUM_PROCESSES_THRESHOLD "masscrashnumprocessesthreshold"
#define COMM_OPTION_MASS_CRASH_TIME_THRESHOLD "masscrashtimethreshold"
#define COMM_OPTION_AUTO_RESTART_PARAMS_RESET_TIMER_FREQUENCY "autorestartparamsresettimerfreq"

#ifdef COMMLIB_CALCULATE_STATISTICS
#define COMM_OPTION_SUBSCR_TRAFFIC_COUNTER  "subscrtrafficcounter"
#define COMM_OPTION_CONN_TRAFFIC_COUNTER  "conntrafficcounter"
#define COMM_OPTION_CONN_TRAFFIC_NAME  "conntrafficname"
#define COMM_OPTION_SUBSCR_TRAFFIC_NAME  "subscrtrafficname"
#define COMM_OPTION_TRAFFIC_PRINT_LAST  "trafficprintlast"
#endif

#define COMM_OPTION_HTTPS_PROXY				"httpsproxy"
#define COMM_OPTION_FREEZERECONNECT			"freezereconnect"
#define COMM_OPTION_TCPCONNECT				"tcpconnect"
#define COMM_OPTION_SSLCONNECT				"sslconnect"
#define COMM_OPTION_THREADPRIORITY			"threadpriority"
#define COMM_OPTION_TCPNODELAY				"tcpnodelay"
#define COMM_OPTION_TCPBUFFER			    "tcpbuffer"
#define COMM_OPTION_TCPBUFFER_RCV		    "tcpbufferrcv"
#define COMM_OPTION_TCPBUFFER_SND		    "tcpbuffersnd"
#define COMM_OPTION_FRAMESIZE				"framesize" 
#define COMM_OPTION_COMPRESS                "compressbits"
#define COMM_OPTION_PROTOCOL                "protocol"
#define COMM_OPTION_COMMUNICATOR_PROCESS    "communicator"

#ifndef COMM_CLIENT_ONLY
#define COMM_OPTION_PROTOCOL_MINIMUM		"minimumprotocol"
#define COMM_OPTION_BLOCK_ENFORCED			"blockenforced"
#endif

#define COMM_OPTION_PROTOCOL_LOG_FILE		"logfile" 
#define COMM_OPTION_MAXIMUM_MESSAGE_SIZE        "maxmessagesize"
#define COMM_OPTION_MAXIMUM_MESSAGE_RATE		"maxmessagerate"
#define COMM_OPTION_MAXIMUM_MESSAGE_RATE_PERIOD "maxmessagerateperiod"
#define COMM_OPTION_MAXIMUM_MESSAGE_SIZE_DEBUG  "maxmessagesizedebug"
#define COMM_OPTION_MAXIMUM_MESSAGE_RATE_DEBUG  "maxmessageratedebug"



#define COMM_OPTION_CLIENT_ENCRYPTION		"clientencryption"

#ifndef COMM_CLIENT_ONLY
#define COMM_OPTION_SERVER_ENCRYPTION		"serverencryption"
#endif


#define DEFAULT_TIMEOUT_FOR_CONNECTING      10000

#define COMM_OPTION_KEEPALIVE				"keepalive"
#define COMM_OPTION_ISLOCAL					"islocal"
#define COMM_OPTION_LISTENBACKLOG			"listenbacklog"
#define COMM_OPTION_INTCPNODELAY			"intcpnodelay"
#define COMM_OPTION_OUTTCPNODELAY			"outtcpnodelay"
#define COMM_OPTION_INTCPBUFFER				"intcpbuffer"
#define COMM_OPTION_INTCPBUFFER_RCV			"intcpbufferrcv"
#define COMM_OPTION_INTCPBUFFER_SND			"intcpbuffersnd"
#define COMM_OPTION_OUTTCPBUFFER			"outtcpbuffer"
#define COMM_OPTION_OUTTCPBUFFER_RCV		"outtcpbufferrcv"
#define COMM_OPTION_OUTTCPBUFFER_SND		"outtcpbuffersnd"
#define COMM_OPTION_FRAMESIZE_VER5			"largeframesize"
#define COMM_OPTION_OPTIMIZEPACKETSIZE		"optimizepacket"
#define COMM_OPTION_CLIENTDISCONNECT		"forceclientdisconnect" 
#define COMM_OPTION_ALLOWCLIENTDISCONNECT   "allowclientdisconnect" 
#define COMM_OPTION_ACCEPTTHREADPRIORITY    "acceptthreadpriority" 
#define COMM_OPTION_CONNECTTHREADPRIORITY   "connectthreadpriority" 
#define COMM_OPTION_MAXNUMBEROFCONNECTIONS  "maxnumberofconnections" 

#define COMM_OPTION_INUDPBUFFER				"inudpbuffer"
#define COMM_OPTION_OUTUDPBUFFER			"outudpbuffer"
#define COMM_OPTION_UDPMSS					"udpmss"
#define COMM_OPTION_UDT_INFLIGHTSIZE		"udtinflightsize"
#define COMM_OPTION_UDT_SENDBUFFER			"udtsendbuffer"
#define COMM_OPTION_UDT_RECVBUFFER			"udtrecvbuffer"
#define COMM_OPTION_UDT_RTT					"udtrtt"
#define COMM_OPTION_UDT_BANDWIDTH			"udtbandwidth"


#define COMM_OPTION_WORKINGTHREADS      "workingthreads" 
#define COMM_OPTION_BUFFERS		        "buffers" 
#define COMM_OPTION_CONCURRENT	        "concurrent" 
#define COMM_OPTION_KEEPALIVEPOLL       "keepalivepoll" 
#define COMM_OPTION_SLOTS		        "maxconnectionsperthread" 
#define COMM_OPTION_RESEND		        "resend" 
#define COMM_OPTION_UDPBUFFER			"udpbuffer"
#define COMM_OPTION_UDPBUFFER_RCV		"udpbufferrcv"
#define COMM_OPTION_UDPBUFFER_SND		"udpbuffersnd"

#define COMM_OPTION_UDPCONNECT			"udpconnect"    
#define COMM_OPTION_HANDSHAKE_TIMEOUT	"handshaketimeout"

#endif
