#ifndef olapmsghandler_h_included
#define olapmsghandler_h_included

#include "commmsgbody.h"
#include "dbmolap.h"

#include "pyrtime.h"	//	PYR-26279
#include "licenseid.h"
#include "PBitmask.h"
#include "sitemasks.h"
#include "licensemask.h"

//	PYR-30121, default index for setIndexWithinGroup()
#define DEFAULT_GROUP_INDEX  1

// PYR-22086

struct OlapMsgSysInfo
{
	friend class OlapMsgHandler;

	enum eOlapDataGroup
	{
		eOlapGroupUndefined = -1, // For parsing purposes only!
		eOlapGroupDefault = 0,
		eOlapGroupUsers = 1,
		eOlapGroupTourns = 2,
		eOlapGroupHands = 3,
		eOlapGroupLoginSession = 4,
		eOlapGroupLoginData = 5,
		eOlapGroupAudit = 6, // Append only tables
		eOlapGroupNonOlap = 7,
		eOlapGroupLogoutData = 8,
		eOlapGroupTrackcodes = 9,
		eOlapGroupCashtrans = 10,
		eReportGroupData = 11,
		eOlapGroupMarket = 12,
		eOlapGroupStatsDB = 13, // PYR-29281 append only, NOT for updatable tables
		eOlapGroupIce = 14, // PYR-29926
		eOlapGroupIceReplicate = 15, // PYR-30772 - for updates
		eOlapGroupAux = 16, // PYR-30747
		eOlapGroupTournHands = 17,	//	PYR-32627
		eOlapGroupSportsBook = 18,	//	PYR-36484
		//eOlapGroupDuel = 19, // PYR-35000 // PYR-53789 - obsolete
		//eOlapGroupFriends = 20, // PYR-35600 // PYR-53789 - obsolete
		//eOlapGroupDuelHands = 21, // PYR-37699 // PYR-53789 - obsolete
		eOlapGroupTheDeal = 22, // PYR-37300
		eOlapGroupStarsDraft = 23,	//	PYR-40346
		eOlapGroupPoolBetting = 24,	//	PYR-42410
		eOlapGroupBDUA = 25, // PYR-46610
		eOlapGroupAuxGameSession = 26, // PYR-49105
		eOlapGroupAuxInternal = 27, // PYR-50285
		eOlapGroupAuxAudit = 28, // PYR-50285
		eOlapGroupAuxHands = 29, // PYR-50285
		eOlapGroupAuxBDUA = 30,  // PYR-54580
		eOlapGroupAutomationData = 31, // PYR-55497
		eOlapGroupAuthData = 32,	//	PYR-60681
		eOlapGroupPredictor = 33,	//	PYR-63110
		eOlapGroupAsiaB2B = 34, // PYR-64175
		eOlapGroupCvlReplicate = 35, // PYR-78456
		eOlapGroupIceAudit = 36, // PYR-78456
		eOlapGroupAuxCasinoRace = 37, // PYR-75566
		eOlapGroupSocialCasino = 38, // PYR-77583
		eOlapGroupMarketComplianceAudit = 39, // PYR-79474 for 'append only' tables
		eOlapGroupStarsStore = 40, // PYR-81644
		eOlapGroupAuxNotification = 41, // PYR-117884
		// Add new groups above!
		// Also add to OlapMsgSysInfo::getGroupByName()!
		eOlapGroupMax
	};

	enum eOlapDataPresence
	{
		ePresenceNone 				= 0,
		ePresenceLicenseMask 		= 1 << 0,
		ePresenceSiteMask 			= 1 << 1,
		ePresenceFlagPM 			= 1 << 2,
		ePresenceFlagLiability 		= 1 << 3,
		ePresenceSourceHostId 		= 1 << 4, // PYR-27618
		ePresenceExclusiveHostId	= 1 << 5, // PYR-27618
		ePresenceIdxWithinGroup 	= 1 << 6, // PYR-29174
		ePresenceDbmId 				= 1 << 7, // PYR-30404
		ePresenceMulticast 			= 1 << 8, // PYR-31289
		ePresenceFlagSequenceEnd	= 1 << 9, // PYR-35064
		ePresenceFlagExtendedLiability 	= 1 << 10, // PYR-63828
		ePresenceFranzIndex			= 1 << 11, // PYR-71763
		ePresenceUniqueSeqId		= 1 << 12, // PYR-71763 - unique monotonic id that is set by replication source module to talk to Franz
	};

private:

#define MAX_HOST_ID_SUPPORTED ((UINT32)eHost_Maximum)
	static_assert(MAX_HOST_ID_SUPPORTED<=4, "HostId is only 2 bit value via eOlapDataFlagXXXHostIdMask");
#define MAX_DBM_ID_SUPPORTED 16383 // 14 bit value, 0b11111111111111, via eOlapDataFlagDbmIdMask

	enum eOlapDataFlags
	{
		eOlapDataFlagPM = 1 << 0,
		eOlapDataFlagLiability = 1 << 1,
		eOlapDataFlagSourceHostIdMask	= (1 << 2) | (1 << 3), // PYR-27618
		eOlapDataFlagExclusiveHostIdMask= (1 << 4) | (1 << 5), // PYR-27618
		eOlapDataFlagDbmIdMask			= (MAX_DBM_ID_SUPPORTED << 6), // PYR-30404
		eOlapDataFlagMulticast			= 1 << 20, // PYR-31289
		eOlapDataFlagSequenceEnd		= 1 << 21, // PYR-35064
		eOlapDataFlagExtLiability		= 1 << 22, // PYR-63828
	};

	mutable CommSrvTime oltpTimestamp;		//	PYR-26279, timestamp of when the message was posted to OLAP
	CommUtcTime utcTimeForOlap;				// PYR-27214, timestamp for synchronizing accounting events across dbm's

	// Data group
	BYTE dataGroup; // eOlapDataGroup
	// Data presence indicator
	UINT32 dataPresenceMask; // eOlapDataPresence
	// Data containers
	PLicenseMask licenseMask;
	PSiteMask siteMask;
	UINT32 flags; // eOlapDataFlags
	UINT32 idxWithinGroup;		//	PYR-29174
	INT64 franzIndex = 0; // PYR-71763 - set by OS2 from uniqueSeqId and replaced by CS after renumeration
	INT64 uniqueSeqId = 0; // PYR-71763 - original from the source module

public:
	OlapMsgSysInfo(eOlapDataGroup dataGroup, eHost srcHost, UINT32 dbmId) 
		: dataGroup(dataGroup)
		, dataPresenceMask(ePresenceNone)
		, flags(0)
		, idxWithinGroup(0)
	{
		if(srcHost != eHost_Unknown)
		{
			setSourceHostId(srcHost);
		}
		setDbmId(dbmId);
	}

	void clearDynamicData() // PYR-28804
	{
		// The following data is considered dynamic: PlayMoney
		resetPlayMoney();
		resetIndexWithinGroup();		//	PYR-29174
		utcTimeForOlap.setNull();
		// SourceHostId and ExclusiveHostId are usually initialized in the DbmStatementWithLog-derived constructors and considered static data
	}

	void compose(CommMsgBody& body) const
	{
		UINT32 tmpLicenseMask = licenseMask.getLower32();
		UINT32 tmpSiteMask = siteMask.getLower32();
		body
			.composeBYTE(dataGroup)
			.composeUINT32(dataPresenceMask)
			.composeUINT32(tmpLicenseMask)
			.composeUINT32(tmpSiteMask)
			.composeUINT32(flags)
			.composeSrvTime(oltpTimestamp);

		utcTimeForOlap.compose(body); // PYR-27214
		body
			.composeUINT32(idxWithinGroup)
			.composeINT64(franzIndex) // PYR-71763
			.composeINT64(uniqueSeqId) // PYR-71763
			;
		licenseMask.compose(body);
		siteMask.compose(body);
	}

	void parse(CommMsgParser& parser)
	{
		UINT32 tmpLicenseMask, tmpSiteMask;
		parser
			.parseBYTE(dataGroup)
			.parseUINT32(dataPresenceMask)
			.parseUINT32(tmpLicenseMask)
			.parseUINT32(tmpSiteMask)
			.parseUINT32(flags)
			.parseSrvTime(oltpTimestamp);
		// PYR-27214 
		utcTimeForOlap.parse(parser);
		parser.parseUINT32(idxWithinGroup); //PYR-29174
		if (!parser.parseEnded())
		{
			parser.parseINT64(franzIndex); // PYR-71763
		}
		else
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "parse ended while attempting to parse franzIndex for OlapMsgSysInfo");
		}
		if (!parser.parseEnded())
		{
			parser.parseINT64(uniqueSeqId); // PYR-71763
		}
		else
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "parse ended while attempting to parse uniqueSeqId for OlapMsgSysInfo");
		}
			
		if ( !parser.parseEnded() ) // PYR-101329 TODO VG remove after 2019.Main.05 is released on BOTH NJ & PA
		{
			licenseMask.parse(parser);
			siteMask.parse(parser);
		}
		else
		{
			licenseMask = PLicenseMask::fromUINT64(tmpLicenseMask); // approved usage of PLicenseMask::fromUINT64()
			siteMask = PSiteMask::fromUINT64(tmpSiteMask); // approved usage of PSiteMask::fromUINT64()
		}
	}

	// Data access

	eOlapDataGroup getDataGroup() const { return (eOlapDataGroup)dataGroup; }

	bool containsData(eOlapDataPresence dataType) const
	{
		return (dataPresenceMask & dataType) != 0;
	}

	// License mask
	[[deprecated("use setLicenseMask(const PLicenseMask&) instead")]]
	void setLicenseMask(UINT32 licenseMask_)
	{
		licenseMask = PLicenseMask::fromUINT64(licenseMask_); // approved usage of PLicenseMask::fromUINT64()
		dataPresenceMask |= ePresenceLicenseMask;
	}

	[[deprecated("use retrieveLicenseMask(PLicenseMask&) instead")]]
	bool retrieveLicenseMask(UINT32& licenseMask_) const
	{
		bool contains = containsData(ePresenceLicenseMask);
		if(contains)
		{
			licenseMask_ = licenseMask.getLower32();
		}
		return contains;
	}

	void setLicenseMask(const PLicenseMask& licenseMask_)
	{
		licenseMask = licenseMask_;
		dataPresenceMask |= ePresenceLicenseMask;
	}

	bool retrieveLicenseMask(PLicenseMask& licenseMask_) const
	{
		bool contains = containsData(ePresenceLicenseMask);
		if (contains)
		{
			licenseMask_ = licenseMask;
		}
		return contains;
	}

	// Site mask
	[[deprecated("use setSiteMask(const PSiteMask&) instead")]]
	void setSiteMask(UINT32 siteMask_)
	{
		siteMask = PSiteMask::fromUINT64(siteMask_); // TODO: PBITMASK should not be called
		dataPresenceMask |= ePresenceSiteMask;
	}

	[[deprecated("use retrieveSiteMask(PSiteMask&) instead")]]
	bool retrieveSiteMask(UINT32& siteMask_) const
	{
		bool contains = containsData(ePresenceSiteMask);
		if(contains)
		{
			siteMask_ = siteMask.getLower32();
		}
		return contains;
	}

	void setSiteMask(const PSiteMask& siteMask_)
	{
		siteMask = siteMask_;
		dataPresenceMask |= ePresenceSiteMask;
	}

	bool retrieveSiteMask(PSiteMask& siteMask_) const
	{
		bool contains = containsData(ePresenceSiteMask);
		if (contains)
		{
			siteMask_ = siteMask;
		}
		return contains;
	}

	// Play money
	void setPlayMoney(bool isPlayMoney)
	{
		if(isPlayMoney)
		{
			flags |= eOlapDataFlagPM;
		}
		else
		{
			flags &= ~eOlapDataFlagPM;
		}
		dataPresenceMask |= ePresenceFlagPM;
	}

	bool retrieveIsPlayMoney(bool& isPlayMoney) const
	{
		bool contains = containsData(ePresenceFlagPM);
		if(contains)
		{
			isPlayMoney = (flags & eOlapDataFlagPM) != 0;
		}
		return contains;
	}

	void resetPlayMoney()
	{
		dataPresenceMask &= ~ePresenceFlagPM;
	}

	// Liability
	void setLiability(bool isLiability)
	{
		if(isLiability)
		{
			flags |= eOlapDataFlagLiability;
		}
		else
		{
			flags &= ~eOlapDataFlagLiability;
		}
		dataPresenceMask |= ePresenceFlagLiability;
	}

	bool retrieveLiability(bool& isLiability) const
	{
		bool contains = containsData(ePresenceFlagLiability);
		if(contains)
		{
			isLiability = (flags & eOlapDataFlagLiability) != 0;
		}
		return contains;
	}
	// PYR-63828
	// Extended Liability
	void setExtendedLiability(bool isExtLiability)
	{
		if(isExtLiability)
		{
			flags |= eOlapDataFlagExtLiability;
		}
		else
		{
			flags &= ~eOlapDataFlagExtLiability;
		}
		dataPresenceMask |= ePresenceFlagExtendedLiability;
	}

	bool retrieveExtendedLiability(bool& isExtLiability) const
	{
		bool contains = containsData(ePresenceFlagExtendedLiability);
		if(contains)
		{
			isExtLiability = (flags & eOlapDataFlagExtLiability) != 0;
		}
		return contains;
	}

	void setFranzIndex(INT64 franzIndex_)
	{
		franzIndex = franzIndex_;
		dataPresenceMask |= ePresenceFranzIndex;
	}

	bool retrieveFranzIndex(INT64& franzIndex_) const
	{
		bool contains = containsData(ePresenceFranzIndex);
		if(contains)
		{
			franzIndex_ = franzIndex;
		}
		return contains;
	}
	void setUniqueSeqId(INT64 uniqueSeqId_)
	{
		uniqueSeqId = uniqueSeqId_;
		dataPresenceMask |= ePresenceUniqueSeqId;
	}
	void resetUniqueSeqId() // PYR-71763
	{
		dataPresenceMask &= ~ePresenceUniqueSeqId;
	}
	bool retrieveUniqueSeqId(INT64& uniqueSeqId_) const
	{
		bool contains = containsData(ePresenceUniqueSeqId);
		if(contains)
		{
			uniqueSeqId_ = uniqueSeqId;
		}
		return contains;
	}

	//	PYR-26279
	void setOltpTimestampToCurrent() const
	{
		oltpTimestamp.currentLocalTime();
	}
	const CommSrvTime& getOltpMsgTime() const
	{
		return oltpTimestamp;
	}
	void setUtcTimeForOlap( const CommUtcTime& utcTime )
	{
		utcTimeForOlap = utcTime;
	}
	const CommUtcTime& getUtcTimeForOlap() const
	{
		return utcTimeForOlap;
	}

	// HostId, PYR-27618
	void setSourceHostId(UINT32 hostId)
	{
		if(hostId < eHost_Central || hostId > MAX_HOST_ID_SUPPORTED)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " SSHI: Invalid hostId %u not set for OlapMsgSysInfo", hostId);
			PASSERT5(0);
			return;
		}

		flags &= ~eOlapDataFlagSourceHostIdMask;
		flags |= (((hostId-eHost_Central) << 2) & eOlapDataFlagSourceHostIdMask);
		dataPresenceMask |= ePresenceSourceHostId;
	}

	bool retrieveSourceHostId(UINT32& hostId) const
	{
		bool contains = containsData(ePresenceSourceHostId);
		if(contains)
		{
			hostId = ((flags & eOlapDataFlagSourceHostIdMask) >> 2) + eHost_Central;
		}
		return contains;
	}

	void setExclusiveHostId(UINT32 hostId)
	{
		if(hostId < eHost_Central || hostId > MAX_HOST_ID_SUPPORTED)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " SEHI: Invalid hostId %u not set for OlapMsgSysInfo", hostId);
			PASSERT5(0);
			return;
		}

		flags &= ~eOlapDataFlagExclusiveHostIdMask;
		flags |= (((hostId-eHost_Central) << 4) & eOlapDataFlagExclusiveHostIdMask);
		dataPresenceMask |= ePresenceExclusiveHostId;
	}

	bool retrieveExclusiveHostId(UINT32& hostId) const
	{
		bool contains = containsData(ePresenceExclusiveHostId);
		if(contains)
		{
			hostId = ((flags & eOlapDataFlagExclusiveHostIdMask) >> 4) + eHost_Central;
		}
		return contains;
	}

	// PYR-29174
	void setIndexWithinGroup(UINT32 idx)
	{
		dataPresenceMask |= ePresenceIdxWithinGroup;
		idxWithinGroup = idx;
	}

	bool retrieveIndexWithinGroup(UINT32& idx) const
	{
		bool contains = containsData(ePresenceIdxWithinGroup);
		if(contains)
		{
			idx = idxWithinGroup;
		}
		return contains;
	}	
	void resetIndexWithinGroup()
	{
		dataPresenceMask &= ~ePresenceIdxWithinGroup;
	}

	// PYR-30404
	void setDbmId(UINT32 dbmId)
	{
		if(dbmId > MAX_DBM_ID_SUPPORTED)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " SDI: Invalid dbmId %u not set for OlapMsgSysInfo", dbmId);
			PASSERT5(0);
			return;
		}

		flags &= ~eOlapDataFlagDbmIdMask;

		flags |= ((dbmId << 6) & eOlapDataFlagDbmIdMask);
		dataPresenceMask |= ePresenceDbmId;
	}

	bool retrieveDbmId(UINT32& dbmId) const
	{
		bool contains = containsData(ePresenceDbmId);
		if(contains)
		{
			dbmId = (flags & eOlapDataFlagDbmIdMask) >> 6;
		}
		return contains;
	}	

	// Multicast
	void setMulticast(bool isMulticast)
	{
		if(isMulticast)
		{
			flags |= eOlapDataFlagMulticast;
		}
		else
		{
			flags &= ~eOlapDataFlagMulticast;
		}
		dataPresenceMask |= ePresenceMulticast;
	}

	bool retrieveMulticast(bool& isMulticast) const
	{
		bool contains = containsData(ePresenceMulticast);
		if(contains)
		{
			isMulticast = (flags & eOlapDataFlagMulticast) != 0;
		}
		return contains;
	}

	//PYR-23220
	static UINT32 getGroupByName(const char* name)
	{
		struct nameVal_
		{
			const char* name;
			UINT32 val;
		};
		static const nameVal_ nameval[] = 
		{
			{ "DEFAULT",       eOlapGroupDefault      },
			{ "USERS",         eOlapGroupUsers        },
			{ "TOURNS",        eOlapGroupTourns       },
			{ "HANDS",         eOlapGroupHands        },
			{ "LOGINSESSION",  eOlapGroupLoginSession },
			{ "LOGINDATA",     eOlapGroupLoginData    },
			{ "AUDIT",         eOlapGroupAudit        },
			{ "NONOLAP",       eOlapGroupNonOlap      },
			{ "LOGOUTDATA",    eOlapGroupLogoutData   },
			{ "TRACKCODES",    eOlapGroupTrackcodes   },
			{ "CASHTRANS",     eOlapGroupCashtrans    },
			{ "STATS",         eReportGroupData       },
			{ "MARKET",		   eOlapGroupMarket       },
			{ "STATSDB",       eOlapGroupStatsDB      },
			{ "ICE",	       eOlapGroupIce	      },
			{ "ICEREP",	       eOlapGroupIceReplicate },
			{ "AUX",           eOlapGroupAux          },
			{ "TOURNHANDS",	   eOlapGroupTournHands   },
			{ "SPORTSBOOK",	   eOlapGroupSportsBook   },
			{ "THEDEAL",       eOlapGroupTheDeal      }, // PYR-37300  (GiuseppeG: This was mistakenly removed in a commit for PYR-40346)
			{ "STARSDRAFT",    eOlapGroupStarsDraft   }, // PYR-40346
			{ "POOLBETTING",   eOlapGroupPoolBetting  }, // PYR-42410
			{ "BDUA",          eOlapGroupBDUA         }, // PYR-46610
			{ "AUXGAMESESSION", eOlapGroupAuxGameSession }, // PYR-49105
			{ "AUXINTERNAL",	eOlapGroupAuxInternal }, // PYR-50285
			{ "AUXAUDIT",		eOlapGroupAuxAudit }, // PYR-50285
			{ "AUXHANDS",		eOlapGroupAuxHands }, // PYR-50285
			{ "AUXBDUA",        eOlapGroupAuxBDUA  }, // PYR-54580
			{ "AUTODATA",       eOlapGroupAutomationData  }, // PYR-55497
			{ "AUTHDATA",		eOlapGroupAuthData  },	// PYR-60681
			{ "PREDICTOR",      eOlapGroupPredictor  }, // PYR-63110
			{ "ASIABB",         eOlapGroupAsiaB2B }, // PYR-64175 (2018-05-08 GiuseppeG: digits inside the text seem to create issues to Majordomo)
			{ "CVL",			eOlapGroupCvlReplicate }, // PYR-78456 Change group for some CVL replication messages 
			{ "ICEAUDIT",		eOlapGroupIceAudit }, // PYR-78456 Change group for some ICE Audit replication messages 
			{ "AUXCASINORACE",	eOlapGroupAuxCasinoRace }, // PYR-75566
			{ "SOCIALCASINO",	eOlapGroupSocialCasino }, // PYR-77583
			{ "MARKETCMPLAUDIT", eOlapGroupMarketComplianceAudit }, // PYR-79474
			{ "STARSSTORE",     eOlapGroupStarsStore  }, // PYR-81644
			{ "AUXNOTIFICATION", eOlapGroupAuxNotification }, // PYR-117884

			{ "",              eOlapGroupMax          },
		};

		for (int i = 0; nameval[i].val != eOlapGroupMax; ++i)
		{
			if (strcmp(name, nameval[i].name) == 0)
				return nameval[i].val;
		}
		return eOlapGroupMax;
	}

	// Sequence End	- PYR-35064
	void setSequenceEnd(bool isEndOfSequence)
	{
		if(isEndOfSequence)
		{
			flags |= eOlapDataFlagSequenceEnd;
		}
		else
		{
			flags &= ~eOlapDataFlagSequenceEnd;
		}
		dataPresenceMask |= ePresenceFlagSequenceEnd;
	}

	bool retrieveIsSequenceEnd(bool& isEndOfSequence) const
	{
		bool contains = containsData(ePresenceFlagSequenceEnd);
		if(contains)
		{
			isEndOfSequence = (flags & eOlapDataFlagSequenceEnd) != 0;
		}
		return contains;
	}
	// PYR-49534
	static UINT32 getOlapIndexFrom64( UINT64 id64 )
	{
		if ( id64 & (UINT64)0x00000000ffffffff )
		{
			return (UINT32)id64;	//	Use lower 32 bits
		}
		return ( id64 >> 32 );		//	Use upper 32 bits
	}
};

class OlapMsgHandler
{
public:

	// PYR-29157: same as extractMsgData, but starts AFTER reqId
	static void extractMsgDataNoReqId(
		UINT32 msgId,		// OLAP message ID.
		CommMsgParser& parser,		// In/out parser. Passed in pointing at the beginning of the whole OLAP message.
						// Points at the first data member after service info extraction.
		OlapMsgSysInfo* sysInfo = 0	// Optional pointer on the OlapMsgSysInfo structure.
		)
	{
		if(msgId == DBM_Q_GET_SAVED_MESSAGES)
		{
			if(sysInfo)
			{
				sysInfo->dataGroup = OlapMsgSysInfo::eOlapGroupNonOlap;
			}
			return;
		}

		CommMsgBody sysInfoBody;
		parser.parseMsgBody(sysInfoBody);
		if(sysInfo)
		{
			CommMsgParser sysInfoParser(sysInfoBody);
			sysInfo->parse(sysInfoParser);
		}
	}

	static void extractMsgData(
		UINT32 msgId,		// OLAP message ID.
		CommMsgParser& parser,		// In/out parser. Passed in pointing at the beginning of the whole OLAP message.
						// Points at the first data member after service info extraction.
		UINT32& reqId,			// Request ID to reply back with OLAP answer message.
		OlapMsgSysInfo* sysInfo = 0	// Optional pointer on the OlapMsgSysInfo structure.
		)
	{
		parser.parseUINT32(reqId);
		extractMsgDataNoReqId(msgId, parser, sysInfo);
	}

	static void composeFrameFinishedMsg(CommMsgBody& msgBody, bool toBeContinued)
	{
		OlapMsgSysInfo sysInfo(OlapMsgSysInfo::eOlapGroupDefault, eHost_Unknown, MAX_DBM_ID_SUPPORTED);
		CommMsgBody sysInfoBody;
		sysInfo.compose(sysInfoBody);

		msgBody
			.composeMsgBody(sysInfoBody)
			.composeBYTE((BYTE)toBeContinued);
	}
};

#endif // olapmsghandler_h_included
