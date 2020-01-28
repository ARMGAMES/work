#include "DbTestingObject.h"
#include "ColossusHand.h"

#define DEFAULT_PORT		7001
#define DEFAULT_INSTANCE	"DbTestingInstance"

////////////////////////////////////////////////////////////////////////////////

class Daemon : public PDaemon
{
private:
	CommRoutingTable theRoutingTable;
	CommRWThreadManager rw;
	CommLocalThreadManager local;
	DbTestingObjectFactory factory;

public:
	Daemon();
	/* virtual */ void start( const char* dir, int nParms, char** parms );
	/* virtual */ void stop();
};

Daemon::Daemon()
	: rw( theRoutingTable ),
	local( theRoutingTable )
{
}

//
// DbTesting.exe [port] [instance name]
//
void Daemon::start( const char* dir, int nParms, char** parms )
{
	PSocket::startup();

	int portNo = 0;
	if (nParms)
	{
		const char* portstr = parms[0];
		portNo = atoi(portstr);
	}
	int realPortNo = portNo ? portNo : DEFAULT_PORT;
	PLog("Listening port %d", realPortNo);

	rw.listenPort( realPortNo );

	PString alias("ip:*:");
	alias.appendInt(realPortNo);
	theRoutingTable.addAlias(alias);

	CommRule rule;
	rule.mask = "ip:*:*";
	rule.conns.push_back( "pyrtcp0://$0:$1" );
	theRoutingTable.addRule( rule );

	local.registerServerObjectFactory( "DbTesting", factory );

	CommMsgBody initBody;
	initBody.composeString( "" );

	const char* instanceName = nParms > 1 ? parms[1] : DEFAULT_INSTANCE;
	local.createServerObject( "DbTesting", instanceName, &initBody );

	theRoutingTable.start();
}

void Daemon::stop()
{
	theRoutingTable.shutdown();
	bool Ok = theRoutingTable.waitUntilTerminated( 5 );
	if( Ok )
		PLog( "Daemon::stop(): Threads terminated" );
	else
		PLog( "Daemon::stop(): Threads not terminated - exiting anyway" );

	PSocket::cleanup();
}

//*****************************************************************************

static bool decode(const PString& linestr, UINT32& msgID, CommMsgBody& sendMsg)
{
	char *pp;

	//	INT32 hour, minutes, seconds;

	//	hour = strtol(linestr.c_str(), &pp, 10);
	//	const char *p = strchr(linestr.c_str(), ':');
	//	const char *p1 = strchr(p+1,':');
	//	minutes = strtol(p+1, &pp, 10);
	//	seconds = strtol(p1+1, &pp,10);

	const char *p = strchr(linestr.c_str(), ' ');
	msgID = strtol(p+1, &pp, 10);
	PBlock msgBlock;
	CommSSLBase64::decode( msgBlock, p+6 );
	if (msgID)
	{
		if (msgBlock.size()) //message body can be empty!
			sendMsg._composeFixedBlock(msgBlock.ptr(),msgBlock.size());	
		return true;
	}
	else
		return false;
}

static void decodeCollusionFinishTransaction()
{
	PString rawMsg = "13:02:17 21705 /wAAACUHAAAAEAAAAAAAAAAAAAAAAAfeAxUNAhIFAAAAAAAAAAAAAAAAAqUor0YA Version=1";
	UINT32 msgId = 0;
	CommMsgBody sendMsg;
	decode(rawMsg, msgId, sendMsg);

	CommMsgBody sysInfoBody;
	UINT32 transId = 0;
	PString transType;
	CommMsgParser parser(sendMsg);
	parser.parseMsgBody(sysInfoBody).parseUINT32(transId).parseStringP(transType);

	printf("transId: %d, transType: '%s'\n\n", transId, transType.c_str());
}

//*****************************************************************************

static void testComposeMsgBodyMarker()
{
	CommMsgBody body1;
	body1.composeBYTE(0x78);
	const BYTE* readPtr = body1._readPtr();
	printf("%02x\n\n", readPtr[0]);

	CommMsgBody body2;
	body2.composeMsgBody(body1);
	const BYTE* readPtr2 = body2._readPtr();
	printf("%02x\n\n", readPtr2[0]);
}

//*****************************************************************************

static void testDecodeHandNewFormat()
{
	PString rawMsg = "13:13:28 20353 /wAAACUDAAAAFgAAAAAAABpBAAAAAAfeAxkNDSACB94DGRENJgIAAAAAxsM4PQAAAAAEd0J2VXJzdWxhAAAAAAGr8pk5AAAAAAAAABQCZgAAADIAAABkAAAAFAAAAAoAAAAPBgAAAAAACAABAAAAAAAAAAAH3gMZDQ0eAgfeAxkNDSgCVVNEAAAAGkEAAAAAAAAAAAAAAAEAAAABAAAAAavymTkxT0kxajRHUmdZRmpDZm85WmtwZFhub21Ca2RFUnlPY3FMUzdJU2F3ME5tVUFBVVlJeVlRa1kyU01KTU1LSmtXUVJKakFGRFBjRkl0a3grTEFGQ1I1SmlSNW9GbmV5VjdKSm5CNVZxQXJuSUMwRVlZckdJR3VjQU54c0xpQ25iRUlxeDVtQmphd0FoRzRqQWNEQVBnbklLZz0AAAAArAL/AAAATnVzcGxheTM1AAEAAAPoAAAACgAEEAE4Y0FzUWQAAVFkOGNBcwAAAAAAAAAAAAAAAAAKAAAD6DEwLjMwLjAuMTI2AAAAAAAAAAAAAAAAAP8AAAABAP8AAABOdXNwbGF5MjMAAgAAA+j////2AAAABUtjSmM0ZAABS2NKYzRkAAAAAAAAAAAAAAAAAAoAAAPoMTAuMzAuMC4xMjYAAAAAAAAAAAAAAAAA/wAAAAEA/wAAAAAAAAAA Version=1";
	UINT32 msgId = 0;
	CommMsgBody handMsg;
	decode(rawMsg, msgId, handMsg);

	CommMsgParser parser(handMsg);
	OlapMsgHandler::extractMsgDataNoReqId(msgId, parser);

	UINT32 reqIdIgnore;
	parser.parseUINT32(reqIdIgnore); // from table to OLTP

	ColossusHand hand;
	hand.parseFromTable(parser);
}

//*****************************************************************************

int main( int argc, char** argv )
{
	//testDecodeHandNewFormat();

	Daemon daemon;
	return PWin32ServiceDaemonMain( daemon, argc, argv, "DbTesting.log", "PYRDBTESTINGDBM" );
}
