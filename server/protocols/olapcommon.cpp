#include	"OlapCommon.h"
#include	"shared.h"
#include	"dbmolap.h"
#include	"dbmstmt.h"
#include	"initdatetime.h"

class MessagesBaseStmt : public DbmStatement
{
protected:
	PSqlInt ordinal, fromQueue, msgId, olapId;
	PSqlSmallInt errCode;
	PSqlString<MSG_BODY_LEN> msgBodyStr;
	PSqlString<MSG_ERRDESCR> errDescr;
	PSqlTimestamp when; // it can never by null, but in DB it's nullable (legacy)
	PSqlSmallInt version; // PYR-22086
	PSqlInt nextCnt; // PYR-23301
	PSqlSmallInt dbmId; // PYR-51852

	const PString tableName;
	const bool newFormat; // PYR-51852 -  // #mgrosman TODO - eventually remove once all dbm switch to the new format
	MessagesBaseStmt( DatabaseManagerCommon& man, const char* table_name ) : DbmStatement( man )
		, tableName( table_name )
		, newFormat( tableName.equals( OLAPMESSAGES_TABLE_NAME ) ) // PYR-51852
	{
		dbmId = man.getDbmId(); // PYR-51852
	}
};

class SaveMessageStmt : public MessagesBaseStmt
{
	void prepareStmt();

public:
	SaveMessageStmt(DatabaseManagerCommon& man, const char* table_name) : MessagesBaseStmt( man, table_name )
	{
		prepareStmt();
	}

	void init( int ordinal_, int fromQueue_, int msgId_, const char * msgBodyStr_, INT16 errCode_, const char * errDescr_, const SrvTime& when_, UINT32 olap_id, UINT16 version_, UINT32 nextCnt_ /*PYR-23301*/);
};

class GetMessageStmt : public MessagesBaseStmt
{
	PSqlInt minOrdinal_param, maxOrdinal_param;
	void prepareStmt();

public:
	GetMessageStmt( DatabaseManagerCommon& man, const char* table_name ) : MessagesBaseStmt( man, table_name )
	{
		prepareStmt();
	}
	void init( UINT32 minOrdinal_, UINT32 maxOrdinal_, UINT32 olap_id);
	void getData( int& ordinal_, int& msgId_, PString& msgBodyStr_, UINT16& version_, UINT32& nextCnt_ /*PYR-23301*/) const;
};

class GetFirstMessageStmt : public MessagesBaseStmt
{
	PSqlIntNullable minOrdinal;
	PSqlInt start;
	void prepareStmt();

public:
	GetFirstMessageStmt( DatabaseManagerCommon& man, const char* table_name ) : MessagesBaseStmt( man, table_name )
	{
		prepareStmt();
	}
	void init( UINT32 start, UINT32 olap_id );
	UINT32 getMinOrdinal() const { return minOrdinal.isNull() ? 0 : minOrdinal.value; }
};


void DbmToOlapClientConn::processGMessage( UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* )
{
	HiresCounter counter;
	UINT32 inQuSize = obj->getCommServerInQueueSize();
	int msgStatus = processGenericMessageNormal;
	obj->processOlapMessage( reqId, msgId, body, this );
	counter.stop();
	UINT32 t, n;
	if( obj->addSpecialTicks( accOlapResponse, t, n, counter, msgStatus ) )
		{
		//do not count overspans here!!!
		PTRACE("}%sX %04d %u (x%d)", obj->getTraceModifier( msgStatus ), msgId, t, n );
		}
	obj->addMessageTicks(counter, msgStatus);
	obj->addInQueueSize( inQuSize );
}

void DbmToOlapClientConn::connected(void) //make a method of OlapStruct
{
	PLog("connected to OLAP %s", olap->name.c_str());
	olap->sendSavedMessages();
}

bool DbmToOlapClientConn::closedOrDisconnected(int errCode, const char * errStr)
{
	PLog("Connection to OLAP %s closed, errCode %d (%s)", olap ? olap->name.c_str() : "", errCode, errStr);
	if(olap && olap->saveMessageOnDisconnect)
	{
		olap->saveQueue();
	}
	return false /*try to reconnect */;
}

UINT32 DbmToOlapClientConn::getOlapId() const
{ 
	return olap ? olap->olapId : 0; 
}

void OlapStruct::sendSavedMessages()
{
	directSendToOlap = true; // Initial state, equivalent to obsolete "saveOlapMessagesToTable=0' parameter
	if (haveOlapDbm() && useOlapDbm() && directSendToOlap)
	{
		bool toBeContinued = false;
		olapServer->processSendSavedMessages(this, toBeContinued);
		if (toBeContinued)
		{
			setDirectSendToOlap(false);
			CommMsgBody body;
			OlapMsgHandler::composeFrameFinishedMsg(body, toBeContinued);
			connToOlapDbm->post(DBM_Q_MESSAGE_FRAME_FINISHED, body);
		}
	}
}

void OlapStruct::saveQueue()
{
	if (!olapServer)
	{
		PLog ("OlapStruct::saveQueue error: !olapServer");
		return;
	}
	if (!olapServer->olapManager)
	{
		PLog ("OlapStruct::saveQueue error: !olapServer->olapManager");
		return;
	}
	olapServer->olapManager->saveQueueToOlap(*this);
}

void OlapStruct::getToOlapQueueRange(CommClientGConnection::const_iterator& begin_, CommClientGConnection::const_iterator& end_) const
{
	begin_ = connToOlapDbm->_begin();
	end_ = connToOlapDbm->_end();
}

void OlapStruct::clearToOlapQueue()
{
	if(connToOlapDbm)
	{
		connToOlapDbm->_clearAll();
	}
}

void OlapStruct::connectToOlap(CommClientNullGuardFactory& guardCliFactory, CommClientConnectionPool& cliPool)
{
	if(0 == connToOlapDbm)
	{
		olapServer->olapInDirectSendMode(olapId, false /*inDirectSend*/);
		connToOlapDbm = new DbmToOlapClientConn(guardCliFactory, olapServer->obj, this);
		cliPool.connect( connToOlapDbm->underlyingConnection(), olapDbmAddress, olapDbmInstance, "oltp" );
	}
}

void OlapStruct::setNewOlapParameters(const unique_ptr<OlapStruct>& other)
{
	if(!olapDbmAddress.equals(other->olapDbmAddress))
	{
		PLog("olap #%u, attempt to change dbmAddress from %s to %s, ignored", 
			olapId,
			olapDbmAddress.c_str(),
			other->olapDbmAddress.c_str()
			);
		return;
	}
	if(!olapDbmInstance.equals(other->olapDbmInstance))
	{
		PLog("olap #%u, attempt to change dbmInstance from %s to %s, ignored", 
			olapId,
			olapDbmInstance.c_str(),
			other->olapDbmInstance.c_str()
			);
		return;
	}
	name = other->name;
	PLog("useOlapDbmPermitted %s to %s", 
		(useOlapDbmPermitted ? "Y" : "N"),
		(other->useOlapDbmPermitted ? "Y" : "N")
		);
	useOlapDbmPermitted = other->useOlapDbmPermitted;

	PLog("saveMessageOnError %s to %s", 
		(saveMessageOnError ? "Y" : "N"),
		(other->saveMessageOnError ? "Y" : "N")
		);
	saveMessageOnError = other->saveMessageOnError;

	PLog("saveMessageOnDisconnect %s to %s", 
		(saveMessageOnDisconnect ? "Y" : "N"),
		(other->saveMessageOnDisconnect ? "Y" : "N")
		);
	saveMessageOnDisconnect = other->saveMessageOnDisconnect;
	// PYR-71763
	PLog( "olapSyncMode %d to %d", olapSyncMode, other->olapSyncMode );
	olapSyncMode = other->olapSyncMode;

	if(!useOlapDbmPermitted)
		clearToOlapQueue();
}

void OlapStruct::trace() const
{
	PLog(
		"trace OLAP:\n"
		"  name = %s\n"
		"  nolapDbmAddress=%s\n"
		"  olapDbmInstance=%s\n"
		"  olapId=%u\n"
		"  useOlapDbmPermitted=%s\n"
		"  saveMessageOnError=%s\n"
		"  saveMessageOnDisconnect=%s\n"
		" syncUsersOnly=%s\n"
		" syncFull=%s\n"
		" syncIce=%s",
		name.c_str(),
		olapDbmAddress.c_str(),
		olapDbmInstance.c_str(),
		olapId,
		(useOlapDbmPermitted ? "YES" : "NO"),
		(saveMessageOnError ? "YES" : "NO"),
		(saveMessageOnDisconnect ? "YES" : "NO"),
		(olapSyncMode == eSyncUsersOnly ? "YES" : "NO"),
		(olapSyncMode == eSyncFull ? "YES" : "NO"),
		(olapSyncMode == eSyncIce ? "YES" : "NO")
		);
}

void OlapStruct::setDirectSendToOlap( bool doSendDirectly )
{
	directSendToOlap = doSendDirectly;
	olapServer->olapInDirectSendMode(olapId, directSendToOlap);
}

eHost OlapStruct::getHostId() const
{
	return olapServer ? olapServer->getHostId() : eHost_Unknown;
}

eHost OlapServer::getHostId() const
{
	return manager ? manager->getHostId() : eHost_Unknown;
}
// PYR-71763
UINT64 OlapServer::getNextUniqueMsgId()
{
	return olapManager->getNextUniqueMsgId();
}

void OlapServer::reportQueue(const HeLogger* heLogger, CommMsgBody& reportBody)
{
	int n = 0;
	for(n = 0; n < olapVector.size(); n++)
	{
		const OlapStruct* olap = olapVector[n];
		if (olap->useOlapDbm())
		{
			PString strOlapQueueSize;
			strOlapQueueSize.appendInt(olap->getQueueSize());
			PString reportId("OlapQueueSize.");
			reportId.append(olap->name);
			heLogger->compose (reportBody, reportId, strOlapQueueSize);
		}
	}
}

void OlapServer::reportSavedMessage(const HeLogger* heLogger, CommMsgBody& reportBody)
{
	UINT32 totalSaved = 0;
	int n = 0;
	for(n = 0; n < olapVector.size(); n++)
	{
		const OlapStruct* olap = olapVector[n];
		if (olap->useOlapDbm())
		{
			PString str;
			totalSaved += olap->numSavedMessagesToOlap;
			str.appendInt(olap->numSavedMessagesToOlap);
			PString reportId("numSavedMessagesTo.");
			reportId.append(olap->name);
			heLogger->compose (reportBody, reportId, str);
		}
	}
	PString str;
	str.appendUint(totalSaved);
	heLogger->compose (reportBody, "totalNumSavedMessages", str);
}

void OlapServer::readFromIniFile(const char* iniFileName)
{
	//disable all existing olaps, all active will be re-enabled after re-reading
	int n = 0;
	for(n = 0; n < olapVector.size(); n++)
	{
		OlapStruct* olap = olapVector[n];
		olap->useOlapDbmPermitted = false;
	}
	PLog("OlapServer::readFromIniFile()");
	PIniFile iniFile( iniFileName );
	const PIniFile::Section* section = iniFile.getSection("SETTINGS");
	if (section && olapManager)
	{
		olapManager->maxMessagesForSynchr = section->getIntProperty("maxMessagesForSynchr", 500);
		PLog("maxMessagesForSynchr %d",	olapManager->maxMessagesForSynchr);

		for(n = 0; n < section->items.size(); n++)
		{
			if( section->items[ n ].name.equalsIgnoreCase( "olap" ) )
			{
				const char* olapSectionName = section->items[ n ].value;
				readOlapFromIniFile(iniFile, olapSectionName);
			}
		}
	}
}

void OlapServer::readOlapFromIniFile(const PIniFile& iniFile, const char* olapSectionName)
{
	const PIniFile::Section* section = iniFile.getSection(olapSectionName);
	if(!section)
	{
		PLog("Not found section [%s] in INI file - olap will not be added", olapSectionName);
		return;
	}
	unique_ptr<OlapStruct> olap( new OlapStruct(this) );
	olap->name = olapSectionName;
	olap->olapDbmAddress				= section->getProperty("olapdbmaddress", "");
	olap->olapDbmInstance			= section->getProperty("olapdbminstance", "");
	int olap_id						= section->getIntProperty("olapid", -1);
	if(!olap->olapDbmAddress.length() || !olap->olapDbmInstance.length() || (olap_id < 0) )
	{
		PLog("Invalid olap section: addr='%s' inst='%s' id=%d",
			olap->olapDbmAddress.c_str(),
			olap->olapDbmInstance.c_str(),
			olap_id
			);
		return;
	}
	olap->olapId = olap_id;
	olap->useOlapDbmPermitted		= 0 != section->getIntProperty("useolapdbm", 0);
	olap->saveMessageOnError			= 0 != section->getIntProperty("saveMessageOnError", 0);
	olap->saveMessageOnDisconnect	= 0 != section->getIntProperty("saveMessageOnDisconnect", 0);
	bool syncUserOnly = 0 != section->getIntProperty("syncUserOnly", 0);
	bool syncFull = 0 != section->getIntProperty("syncFull", 0);
	bool syncIce = 0 != section->getIntProperty("syncIce", 0);
	if((syncUserOnly && syncFull) || (syncIce && syncUserOnly) || (syncIce && syncFull))
	{
		// Misconfiguration
		PLog("PError: Olap in section [%s] is misconfigured: it cannot be a combination of 'syncUserOnly', 'syncFull' and 'syncIce' at the same time. Not included in the list.", olapSectionName);
		return;
	}
	
	if(syncFull)
		olap->olapSyncMode = eSyncFull;
	else
		if(syncUserOnly)
			olap->olapSyncMode = eSyncUsersOnly;
		else
			if(syncIce)
				olap->olapSyncMode = eSyncIce;
			else
				olap->olapSyncMode = eSyncNormal;

	PLog("OlapServer sync mode: %d", olap->olapSyncMode);
	olap->trace();
	addOlap(olap);
}


void OlapServer::addOlap( unique_ptr<OlapStruct>& new_olap )
{
	PLog("OlapServer::addOlap #%u", new_olap->olapId);
	bool olapFound = false;
	for(int n = 0; n < olapVector.size(); n++)
	{
		OlapStruct* olap = olapVector[n];
		if(olap->olapId == new_olap->olapId)
		{
			olapFound = true;
			olap->setNewOlapParameters(new_olap);
			break;
		}
	}
	if(!olapFound)
	{
		PLog("adding a new");
		olapVector.push_back( new_olap.release() );
	}
}


OlapStruct* OlapServer::getOlapById(UINT32 olapId) const
{
	for( int n = 0; n < olapVector.size(); n++ )
	{
		if( olapVector[n]->olapId == olapId )
		{
			return olapVector[n];
		}
	}
	return 0;
}

void OlapServer::postToOlap(UINT32 msgId, const CommMsgBody& body, bool sendAlwaysDirect, bool /*isPlayReplicaSync*/, BYTE msgMask, const OlapMsgSysInfo& msgSysInfo)
{
	// PYR-71763
	OlapMsgSysInfo newMsgSysInfo( msgSysInfo );

	for(int n = 0; n < olapVector.size(); n++)
	{
		OlapStruct* olap = olapVector[n];
		if(!olap->haveOlapDbm())
		{
			continue;
		}
		if(!olap->useOlapDbm())
		{
			continue;
		}
		if(!olap->connToOlapDbm)
		{
			PLog("postToOlap error: !connToOlapDbm in %s", olap->name.c_str());
			continue;
		}
		if( (olap->olapSyncMode != eSyncIce && (msgMask & ICE_MSG_MASK) != 0 && olap->olapSyncMode != eSyncFull) || 
			(olap->olapSyncMode == eSyncIce && (msgMask & ICE_MSG_MASK) == 0) ) // PYR-29926
		{
			continue;
		}
		// PYR-71763
		if( !newMsgSysInfo.containsData( OlapMsgSysInfo::ePresenceUniqueSeqId ) )
		{
			// only adding unique id if it's not already added by some higher-level code
			const UINT64 nextUniqueMsgId = getNextUniqueMsgId();
			newMsgSysInfo.setUniqueSeqId( nextUniqueMsgId );
			PTRACE5( "fid:%llX", nextUniqueMsgId );
		}
		const bool postToOlapNow = olap->directSendToOlap || sendAlwaysDirect;	//	PYR-26279
		if (postToOlapNow)
		{
			newMsgSysInfo.setOltpTimestampToCurrent();		//	PYR-26279
		}

		// PYR-22086
		CommMsgBody msgSysInfoBody;
		newMsgSysInfo.compose(msgSysInfoBody);
		CommMsgBody olapMsg;
		olapMsg
			.composeMsgBody(msgSysInfoBody)
			.merge(body);
		if(postToOlapNow)
		{
			olap->connToOlapDbm->post(msgId, olapMsg);
		}
		else 
		{
			saveMessage(msgId, olapMsg, MSG_IS_NOT_FROM_QUEUE, 0, "", olap->olapId);
		}
	}
}

void OlapServer::postToIce(UINT32 msgId, UINT32 userIntId, const CommMsgBody& body)
{
	PTRACE5(">ICE %u %u", msgId, userIntId);
	OlapMsgSysInfo msgSysInfo(OlapMsgSysInfo::eOlapGroupIce, manager->getHostId(), manager->getDbmId());
	msgSysInfo.setIndexWithinGroup(userIntId);

	postToOlap(msgId, body, false /*sendAlwaysDirect*/, false /*isPlayReplicaSync*/, ICE_MSG_MASK, msgSysInfo);
}

UINT32 OlapServer::connectToOlapAll(CommClientConnectionPool& cliPool, CommClientNullGuardFactory& guardCliFactory)
{
	UINT32 numOlaps = 0;
	for(int n = 0; n < olapVector.size(); n++)
	{
		OlapStruct* olap = olapVector[n];
		if(!olap->useOlapDbmPermitted)
			continue;
		olap->connectToOlap(guardCliFactory, cliPool);
		++numOlaps;
	}
	return numOlaps;
}

void OlapServer::saveQueueToOlapAll()
{
	PLog("Saving queue to %d OLAP(s)...", olapVector.size());
	for(int n = 0; n < olapVector.size(); n++)
	{
		OlapStruct* olap = olapVector[n];
		if(!olap->useOlapDbm())
		{
			PLog(" queue to OLAPs '%s' ignored", olap->name.c_str());
			continue;
		}
		olap->saveQueue();
		PLog(" queue to OLAPs '%s' saved", olap->name.c_str());
	}
}

void OlapServer::init(CommServerObjectWithOlapInteface*	obj_, DatabaseManagerCommonWithOlap* manager_)
{
	obj = obj_;
	manager = manager_;
	olapManager = manager->_getOlapManager();
	if( olapManager )
	{
		olapManager->addUniqueIdGenerator(); // PYR-71763
	}
	else
	{
		// Cannot be!
		PLog( LOG_TRACE_FAULTY_LOGIC ": !olapManager" );
	}
}

//////////////////////////// OLAP - DB

void OlapServer::saveMessage(UINT32 msgId, const CommMsgBody& body, int fromQueue, INT16 errCode, const char * errDescr, UINT32 olapId)
{
	try
	{
		olapManager->saveMessage(msgId, body, fromQueue, errCode, errDescr, olapId);
		OlapStruct* olap = getOlapById(olapId);
		if (olap)
		{
			olap->numSavedMessagesToOlap ++;
		}
	}
	catch(const  PSqlError& er )
	{
		PLog("Message to OLAP was not saved: %s", er.why());
	}
}

//#15262 - no longer throws on longer than 16K messages
INT16 OlapManager::saveMessage(UINT32 msgId, const CommMsgBody& body, int fromQueue, INT16 errCode, const char * errDescr, UINT32 olapId) 
{
	try
	{
		PString msgStr;
		PTRACE5("SM: %u size %u", msgId, body._size());
		CommSSLBase64::encode( msgStr, body._readPtr(), body._size()); // function is now safe

		// This timestamp should NOT be changed to srvObj->getMsgTime(),
		// since it is the time when the message was saved that does not
		// need to be tied to "virtual" timestamps generated by server object
		SrvTime srvNow;
		srvNow.currentLocalTime();

		const UINT32 olapMsgBodyLen = OLAP_MSG_BODY_LEN - 1;
		
		if (msgStr.length() <= olapMsgBodyLen) // 99.999%
		{
			int ordinal = manager->getNextId(manager->getMsgObjectName());
			PTRACE5("OM::sM OrdinalPropName = '%s' olapId = %u -> ordinal = %d", manager->getMsgObjectName(), olapId, ordinal);
	        saveMessageStmt->init(ordinal, fromQueue, msgId, msgStr, errCode, errDescr, srvNow, olapId, olapProtocolVersion, 0 /*numCnt*/);
		    saveMessageStmt->execute();
		}
		else // PYR-23301 - saving in chunks
		{
			int numsegs = msgStr.length() / (olapMsgBodyLen);
			int lastlen = msgStr.length() % (olapMsgBodyLen);
			if (lastlen)
			{
				numsegs += 1;
			}
			else
			{
				lastlen = (olapMsgBodyLen);
			}
			for (int i = 0; i < numsegs; ++i)
			{
				int ordinal = manager->getNextId(manager->getMsgObjectName());
				const char* beg = msgStr.c_str() + i * (olapMsgBodyLen);
				int segnum = numsegs - i - 1;
				int len = segnum ? (olapMsgBodyLen): lastlen;
				PString curr(beg, len);
				saveMessageStmt->init(ordinal, fromQueue, msgId, curr, errCode, errDescr, srvNow, olapId, olapProtocolVersion, segnum /*numCnt*/);
				saveMessageStmt->execute();
				PTRACE5("   S@> msgId=%u nextCnt=%d", msgId, segnum);
			}
		}
        return DBM_NO_ERROR;
    }
	catch(const  PSqlError& er )
	{
		PLog("Message ID=%u to OLAP was not saved: %s", msgId, er.why());
        return DBM_INTERNAL_ERROR;
	}
}

INT16 OlapManager::sendSavedMessagesToOlap(OlapStruct& olap, bool& toBeContinued, PString& errDescr)
{
	PString lastOrdinalOlapIdStr(manager->getOrdinalPropName());
	int lastSentOrdinal = 0;
	lastOrdinalOlapIdStr.appendInt(olap.olapId);
	INT16 err_code = manager->getIntProperty(lastOrdinalOlapIdStr, lastSentOrdinal, errDescr);
	if(err_code)
	{
		PLog("get LastMsgOrdinalOlap error: '%s'", errDescr.c_str());
		PLog("start from 0");
		lastSentOrdinal = 0;
	}
	else
	{
		PTRACE5("OM::sSMTO OrdinalPropName = '%s' olapId = %u fullPropertyName = '%s' -> lastSentOrdinal = %d", manager->getOrdinalPropName(), olap.olapId, lastOrdinalOlapIdStr.c_str(), lastSentOrdinal);
	}

	try
	{
		DbmGenerator& gen = manager->_getGenerator();
		UINT32 lastMessageSavedId = gen.peekNextIdNoInsert(manager->getMsgObjectName());
		UINT32 nextMessageToSent = 0;
		PTRACE5("OM::sSMTO lastMessageSavedId = %u", lastMessageSavedId);
		if( lastSentOrdinal >= lastMessageSavedId - 1 ) //there is nothing to send
		{
			PLog("Nothing to send: last sent = %d, last saved = %d", lastSentOrdinal, lastMessageSavedId - 1);
			olap.setDirectSendToOlap(true);
			toBeContinued = false;
		}
		else
		{	
			// PYR-23301
			const int minOrdinal = lastSentOrdinal + 1;
			int maxOrdinal = minOrdinal + maxMessagesForSynchr; // non-const. gets incremented by the chunked message length
			int lastOrdinal = maxOrdinal;					//if there are nothing in table set counter to the maximum
			if  (lastOrdinal > lastMessageSavedId - 1)		// should not be greater last saved number
			{
				lastOrdinal = lastMessageSavedId - 1;
			}
			PLog("sent from %d to %d", minOrdinal, maxOrdinal);

			getSavedOLAPMessagesStmt->init(minOrdinal, maxOrdinal, olap.olapId);
			getSavedOLAPMessagesStmt->execute();

			PString msgBodyStr = "";
			UINT32 prevNextCnt = 0;
			UINT32 nextCnt = 0;
			bool loopOnceMore = true;
			while( loopOnceMore )
			{
				loopOnceMore = false;
				while (getSavedOLAPMessagesStmt->fetch())
				{
					int msgId;
					PString currStr;
					UINT16 version;
					
					getSavedOLAPMessagesStmt->getData(lastOrdinal, msgId, currStr, version, nextCnt);

					if( nextCnt != 0 && prevNextCnt == 0 )
					{
						maxOrdinal += nextCnt;
						loopOnceMore = true;
						PLog("loading %u chunks. maxOrdinal %d now. will loop again.", nextCnt, maxOrdinal);
					}
					if (lastOrdinal + nextCnt <= maxOrdinal)
					{
						msgBodyStr.append(currStr);
						if (nextCnt) // long message segment
						{
							PTRACE5("   G@> msgId=%d nextCnt=%u", msgId, nextCnt);
							prevNextCnt = nextCnt;
							continue;
						}
						PTRACE5("ord> %d", lastOrdinal);

						PBlock msgBlock;
						CommSSLBase64::decode( msgBlock, msgBodyStr );

						PTRACE5("decoded");

						CommMsgBody msgBody;
						memcpy( msgBody._alloc( msgBlock.size() ), msgBlock.ptr(), msgBlock.size() ); //lint !e864 ... depends on order of evaluation

						// PYR-22086
						CommMsgBody olapMsgBody;
						if(version == eOlapProtocolVersionLegacy)
						{
							PTRACE("MSG %d v0->v1", msgId);
							OlapMsgSysInfo sysInfo(OlapMsgSysInfo::eOlapGroupDefault, manager->getHostId(), manager->getDbmId());
							CommMsgBody sysInfoBody;
							sysInfo.compose(sysInfoBody);
							olapMsgBody
								.composeMsgBody(sysInfoBody)
								.merge(msgBody);
						}
						else
						{
							olapMsgBody.moveFrom(msgBody);
						}
						olap.connToOlapDbm->post(msgId, olapMsgBody);
						msgBodyStr = "";
					}
					else // long message on the boundary: process in next batch
					{
						lastOrdinal -= 1;
						PLog("%d + %d exceeded maxOrdinal. break.", lastOrdinal, nextCnt);
						break;
					}
					prevNextCnt = nextCnt;
				}
				getSavedOLAPMessagesStmt->closeCursor();
				if( loopOnceMore )
				{
					getSavedOLAPMessagesStmt->init(lastOrdinal + 1, maxOrdinal, olap.olapId);
					getSavedOLAPMessagesStmt->execute();
				}
			}
			
			getFirstMessageStmt->init(lastOrdinal, olap.olapId);
			getFirstMessageStmt->execute();
			if (getFirstMessageStmt->fetch())
				nextMessageToSent = getFirstMessageStmt->getMinOrdinal();
			getFirstMessageStmt->closeCursor();
			if (nextMessageToSent) // there is something in the table to the specific OLAP
			{
				PLog("SavedMessageQueue is not empty, come back for more");
				olap.setDirectSendToOlap(false);
				toBeContinued = true;
				manager->saveIntProperty(lastOrdinalOlapIdStr, nextMessageToSent-1, errDescr);
			}
			else
			{
				PLog("SavedMessageQueue is empty, begin send directly to Olap");
				olap.setDirectSendToOlap(true);
				toBeContinued = false;
				olap.numSavedMessagesToOlap = 0;
				manager->saveIntProperty(lastOrdinalOlapIdStr, lastMessageSavedId - 1, errDescr);
			}
		}
		PTRACE5("SSM2O done (nextMessageToSent = %u lastMessageSavedId = %u)", nextMessageToSent, lastMessageSavedId);
		return DBM_NO_ERROR;
	}
	catch(const PSqlError& er )
	{
		PLog("Processing GetMessagesStmt Err# %d : %s", er.code(), er.why());
		olap.setDirectSendToOlap(false);
		toBeContinued = false;
		errDescr = er.why();
		return er.code();
	}
}

// PYR-71763 - add new generator to generate unique ids for each message to OLAP
void OlapManager::addUniqueIdGenerator()
{
	PASSERT( manager );
	uniqueOlapMsgIdObjectName.appendUint( manager->getDbmId() );
	PLog( "uniqueOlapMsgIdObjectName: %s", uniqueOlapMsgIdObjectName.c_str() );
	manager->addObjectNameToGenerator( uniqueOlapMsgIdObjectName, true /*local*/ );
}
UINT64 OlapManager::getNextUniqueMsgId()
{
	return manager->getNextId64( uniqueOlapMsgIdObjectName );
}

void OlapServer::processSendSavedMessages(OlapStruct* olap, bool& toBeContinued)
{
	PString errDescr;
	PASSERT(olap); // Discussed with MG, to silence PC Lint
	olapManager->sendSavedMessagesToOlap(*olap, toBeContinued, errDescr);
}

void OlapServer::olapInDirectSendMode( UINT32 olapId, bool inDirectSend )
{
	PLog(" OLAP #%d is %sin DirectSend mode", olapId, inDirectSend ? "" : "NOT ");
	if(allOlapsInDirectSend)
	{
		if(inDirectSend)
		{
			PLog(" Warning inDirectSend is true when allOlapsInDirectSend is true! Ignored!");
		}
		else
		{
			PLog(" Not all OLAPs are in DirectSend mode");
			allOlapsInDirectSend = false;
			manager->allOlapsInDirectSend(false /*inDirectSend*/);
		}
	}
	else
	{
		if(inDirectSend)
		{
			for(UINT32 i=0; i<olapVector.size(); ++i)
			{
				if(!olapVector[i]->directSendToOlap)
				{
					PLog(" Waiting on OLAP %s (#%d) to switch to DirectSend mode", olapVector[i]->name.c_str(), olapVector[i]->olapId);
					return;
				}
			}
			PLog(" All OLAPs are in DirectSend mode");
			allOlapsInDirectSend = true;
			manager->allOlapsInDirectSend(true /*inDirectSend*/);
		}
	}
	// else allOlapsInDirectSend won't be changed 
}

void OlapManager::saveQueueToOlap(OlapStruct& olap)
{
	if (!olap.useOlapDbm())
	{
		PLog("saveQueueToOlap '%s' ignored", olap.name.c_str());
		return;
	}

    CommClientGConnection::const_iterator begin, end;
	olap.getToOlapQueueRange(begin, end);

	PLog("Start saving messages to Olap %s", olap.name.c_str());
	olap.setDirectSendToOlap(false);

	if (begin == end )
	{
		PLog("queue is empty");
		return;
	}
	
	int counter = 0;
	int totalSaved = 0;
    int totalErrors = 0;
	try
	{ 
		manager->setAutoCommit(false);
		for ( CommClientGConnection::const_iterator it = begin; it != end; ++it)
		{
			UINT32 msgId = CommClientGConnection::_getMsgId( it );
			CommMsgBody body;
			CommClientGConnection::_getMsgBody( it, body);
			INT16 errCode = saveMessage(msgId, body, MSG_IS_FROM_QUEUE, 0, "", olap.olapId);

            if (errCode == DBM_NO_ERROR)
            {
                counter++;
                totalSaved++;
                if (counter > 999) // magic number - write 1000 messages in one transaction
                {
                    manager->commit();
                    counter = 0;
                    PLog("%d messages committed", totalSaved);
                }
            }
            else
            {
                ++totalErrors;
            }
		}
        manager->commit();
        manager->setAutoCommit(true);
	}
	catch (const  PSqlError& er)
	{
		PLog("Message to OLAP was not saved: %s", er.why());
	}
	olap.clearToOlapQueue();
    PLog("Total %d messages saved, %d failed, olap queue cleared", totalSaved, totalErrors);
}

void OlapManager::prepareStatements( DatabaseManagerCommonWithOlap& manager )
{
	saveMessageStmt				= new SaveMessageStmt( manager, manager.getMsgTableName() );
	getSavedOLAPMessagesStmt	= new GetMessageStmt( manager, manager.getMsgTableName() );
	getFirstMessageStmt			= new GetFirstMessageStmt( manager, manager.getMsgTableName() );
}

void OlapManager::deleteStatements()
{
	delete	saveMessageStmt;
	delete	getSavedOLAPMessagesStmt;
	delete	getFirstMessageStmt;
	_zeroStatements();
}

void OlapManager::_zeroStatements()
{
	saveMessageStmt				= 0;
	getSavedOLAPMessagesStmt	= 0;
	getFirstMessageStmt			= 0;
}

void OlapManager::init(DatabaseManagerCommonWithOlap* manager_)
{
	manager = manager_;
}

void SaveMessageStmt::init( int ordinal_, int fromQueue_, int msgId_, const char * msgBodyStr_, INT16 errCode_, const char * errDescr_, const SrvTime& when_, UINT32 olap_id, UINT16 version_, UINT32 nextCnt_ /*PYR-23301*/)
{
	ordinal = ordinal_;
	msgId = msgId_;
	fromQueue = fromQueue_;
	msgBodyStr = msgBodyStr_;
	errCode = errCode_;
	errDescr.initCut( errDescr_ );
	when = when_;
	olapId = olap_id;
	version = version_;
	nextCnt = nextCnt_; // PYR-23301
}

void SaveMessageStmt::prepareStmt()
{
	PString statement("INSERT INTO ");
	// PYR-51852 - new MESSAGES table has different format
	if( newFormat )
	{
		PLog( "New MESSAGES format. DbmId=%d", dbmId.value );
		statement.append( OLAPMESSAGES_TABLE_NAME " (ORDINAL, FROMQUEUE, MSGID, MSGBODYSTR, ERRCODE, ERRDESCR, WHEN, OLAPID, VERSION, NEXTCNT, DBMID) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	}
	else
	{
		statement
			.append(tableName)
			.append(" (ORDINAL, FROMQUEUE, MSGID, MSGBODYSTR, ERRCODE, ERRDESCR, WHEN, OLAPID, VERSION, NEXTCNT) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	}
	prepare(statement);

	bindFirstParam( ordinal );
	bindNextParam( fromQueue );
	bindNextParam( msgId );
	bindNextParam( msgBodyStr );
	bindNextParam( errCode );
	bindNextParam( errDescr );
	bindNextParam( when );
	bindNextParam( olapId );
	bindNextParam( version );
	bindNextParam( nextCnt ); // PYR-23301
	if( newFormat )
	{
		bindNextParam( dbmId ); // PYR-51852
	}
}

void GetMessageStmt::prepareStmt()
{
	PString stmt("SELECT ORDINAL, FROMQUEUE, MSGID, MSGBODYSTR, VERSION, NEXTCNT FROM ");
	// PYR-51852 - new MESSAGES table has different format
	if( newFormat )
	{
		PLog( "New MESSAGES format. DbmId=%d", dbmId.value );
		stmt.append( OLAPMESSAGES_TABLE_NAME " WHERE ORDINAL BETWEEN ? AND ? AND OLAPID = ? AND DBMID = ? ORDER BY ORDINAL" );
	}
	else
	{
		stmt
			.append(tableName)
			.append(" WHERE ORDINAL BETWEEN ? AND ? AND OLAPID = ? ORDER BY ORDINAL");
	}
	prepare(stmt);

	bindFirstCol( ordinal );
	bindNextCol( fromQueue );
	bindNextCol( msgId );
	bindNextCol( msgBodyStr );
	bindNextCol( version );
	bindNextCol( nextCnt ); // PYR-23301
	// WHERE
	bindFirstParam( minOrdinal_param );
	bindNextParam( maxOrdinal_param );
	bindNextParam( olapId );
	if( newFormat )
	{
		bindNextParam( dbmId ); // PYR-51852
	}
}

void GetMessageStmt::getData( int& ordinal_, int& msgId_, PString& msgBodyStr_, UINT16& version_, UINT32& nextCnt_ /*PYR-23301*/) const
{
	ordinal_ = ordinal.value;
	msgId_ = msgId.value;
	msgBodyStr_ = msgBodyStr.c_str();
	version_ = version.value;
	nextCnt_ = nextCnt.value; // PYR-23301
}

void GetMessageStmt::init( UINT32 minOrdinal_, UINT32 maxOrdinal_, UINT32 olap_id)
{
	olapId = olap_id;
	minOrdinal_param = minOrdinal_;
	maxOrdinal_param = maxOrdinal_;
}

void GetFirstMessageStmt::prepareStmt()
{
	PString stmt("SELECT MIN(ORDINAL) FROM ");
	stmt
		.append(tableName)
		.append(" WHERE ORDINAL > ? AND OLAPID = ? ");
	if( newFormat ) // PYR-51852
	{
		PLog( "New MESSAGES format. DbmId=%d", dbmId.value );
		stmt.append( " AND DBMID = ?" );
	}
	prepare(stmt);

	bindFirstCol( minOrdinal );
	// WHERE
	bindFirstParam( start );
	bindNextParam( olapId );
	if( newFormat )
	{
		bindNextParam( dbmId ); // PYR-51852
	}
}

void GetFirstMessageStmt::init( UINT32 start_, UINT32 olap_id)
{		
	olapId = olap_id;
	start = start_;
}

//************ container to keep prepared message during transaction **************
void InTransactionUpdates::insertMessage(const UINT32 msgId, CommMsgBody& body, BYTE msgMask, const OlapMsgSysInfo& sysInfo)
{
	size_t origBodySize = body._size();
#if PTRACELVL >= 5
	int origListSize = preparedMessages.size();
#endif
	LogMessage msg;
	msg.msgId = msgId;
	msg.msgMask = msgMask;
	msg.msgBody.moveFrom( body );
	msg.sysInfo = sysInfo;
	list< LogMessage >::const_iterator it = preparedMessages.insert( preparedMessages.end(), msg ); //-V808
#if PTRACELVL >= 5
	if (origListSize + 1 != preparedMessages.size())
	{
		PLog (LOG_TRACE_FAULTY_LOGIC " !InTransactionUpdates::insertMessage(list.size %d+1!=%zu)", origListSize, preparedMessages.size());
	}
	else if (it == preparedMessages.end())
	{
		PLog (LOG_TRACE_FAULTY_LOGIC " !InTransactionUpdates::insertMessage(%u)", msgId);
	}
	else if (it->msgId != msgId)
	{
		PLog (LOG_TRACE_FAULTY_LOGIC " !InTransactionUpdates::insertMessage(%u != %u)", it->msgId, msgId);
	}
	else if (it->msgBody._size() != origBodySize)
	{
		PLog (LOG_TRACE_FAULTY_LOGIC " !InTransactionUpdates::insertMessage(body size %u != %u)", it->msgBody._size(), origBodySize);
	}
#endif
}

void InTransactionUpdates::commit(CommServerObjectWithOlapInteface*	dbmObj)
{
	// don't need to check if we use OLAP because insertMessage already checked it

	if(dbmObj)
	{
#if PTRACELVL >= 5
		if (preparedMessages.size() > 0)
		{
			PTRACE5("m=%zu", preparedMessages.size());
		}
#endif
		for(
			_List::const_iterator it = preparedMessages.begin(); 
			it != preparedMessages.end(); 
			++it)
		{
			const LogMessage& msg = *it;
			dbmObj->postToOlap(msg.msgId, msg.msgBody, false, false /*isPlayReplicaSync*/, msg.msgMask, msg.sysInfo);
		}
	}
	else
	{
		PLog("Error: !ToSendMessages::dbmObj");
	}
	preparedMessages.clear();
}

void InTransactionUpdates::rollback()
{
	PLog("Prepared messages rollback");
	preparedMessages.clear();
}

void InTransactionUpdates::setSequenceEnd()
{
	for ( _List::iterator it = preparedMessages.begin(); it != preparedMessages.end(); )
	{
		LogMessage& msg = *it;
		++it;
		msg.sysInfo.setSequenceEnd ( it == preparedMessages.end() );
	}
}


//////////////////////////////////////////////////////////////////////////

OlapMessageProcessor::OlapMessageProcessor(const UINT32 *arr, UINT32 sizeArr )
{
	for( UINT32 i = 0; i < sizeArr; ++i )
	{
		allowedMessages.insert( arr[i] );
	}

	allowedMessages.insert( DBM_A_OLAPSAVER_RESULT		);
	allowedMessages.insert( DBM_A_OLAP_INSERT_PROPERTY	);
	allowedMessages.insert( DBM_A_OLAP_UPDATE_PROPERTY	);
	allowedMessages.insert( DBM_A_MESSAGE_FRAME_FINISHED);
	allowedMessages.insert( DBM_A_OLAP_REJECTED_MESSAGE	);
}

void OlapMessageProcessor::processOlapMessage( UINT32 msgId, CommMsgParser &parser, DbmToOlapClientConn* conn, UINT32 reqId, OlapServer& olapServer )
{
	if( allowedMessages.find( msgId ) != allowedMessages.end() )
	{
		INT16 err_code;
		parser.parseINT16(err_code);
		if(err_code)
		{
			const char * errDescr;
			parser.parseString(errDescr);
			if(conn->getOlap() && conn->getOlap()->saveMessageOnError)
			{
				PLog("DBM_A_OLAP #%u message %d - error #%d: '%s'", conn->getOlapId(), msgId, err_code, errDescr);
				// get body of original message to OLAP
				CommClientGConnection::const_iterator it = conn->_getIterator(reqId);
				if (conn->_end() == it)
				{
					PLog("Original message was not saved");	
				}
				else
				{
					CommMsgBody originalBody;
					DbmToOlapClientConn::_getMsgBody( it, originalBody);
					UINT32 originalMsgId = DbmToOlapClientConn::_getMsgId( it);
					olapServer.saveMessage(originalMsgId, originalBody, MSG_IS_NOT_FROM_QUEUE, err_code, errDescr, conn->getOlapId());
				}
			}
		}
	}
	else if( DBM_A_OLAP_UNKNOWN_MESSAGE == msgId )
	{
		UINT32 unknownmsgId;
		parser.parseUINT32(unknownmsgId);
		PLog("DBM_A_OLAP unknown message %d'", unknownmsgId);
	}
	else if( DBM_Q_GET_SAVED_MESSAGES == msgId )
	{
		bool toBeContinued = false;
		olapServer.processSendSavedMessages(conn->getOlap(), toBeContinued);
		CommMsgBody reply;
		OlapMsgHandler::composeFrameFinishedMsg(reply, toBeContinued);
		conn->post(DBM_Q_MESSAGE_FRAME_FINISHED, reply);
	}
	else
	{
		PLog("processOlapMessage error ... unhandled message Id=%d", msgId);
	}
}
