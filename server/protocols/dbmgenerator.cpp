// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//dbmgenerator.cpp
#include	"dbmgenerator.h"
#include	"dbmstmt.h"
#include	"dbm.h"
#include	"dbmgeneratorprepstmt.h" // PYR-24146 - moved class GetNextIdStmt to new file dbmgeneratorprepstmt.h
#include	"commcli.h"
#include	"dbmserverobjectcommon.h"
#include	"sites.h"


DbmGenerator::DbmGenerator()
	: rangeIncrement( 0 )
	, dbManagerCommon( 0 )
	, idRangeConn( 0 )
	, remainingIdsThreshold( 0 )
	, requiredIdsBuffer( 0 )
	, useSelectWithUpdate( true ) 
{
	_zeroStatements();
	PLog("DbmGenerator: useSelectWithUpdate=%s", useSelectWithUpdate ? "yes" : "no");// PYR-47222 #mgrosman - eventually remove
}

void DbmGenerator::prepareStatements( DatabaseManagerCommon& manager )
{
	dbManagerCommon = &manager; // PYR-27418

	getNextIdStmt		= new GetNextIdStmt( manager );
	getNextIdStmtNoForUpdateClause = new GetNextIdStmt( manager, false /*useForUpdateClause*/); // PYR-45721
	setNextIdStmt		= new SetNextIdStmt( manager );
	insertObjectRowStmt	= new InsertObjectRowStmt( manager );
	selectAndSetNextIdStmt = new SelectAndSetNextIdStmt( manager ); // PYR-45721
	insertIdRangeStmt	= new InsertIdRangeStmt( manager );
	getIdRangesStmt		= new GetIdRangesStmt( manager );
	getCurrentIdRangesStmt = new GetCurrentIdRangesStmt( manager );
}

void DbmGenerator::deleteStatements()
{
	delete	insertObjectRowStmt;
	delete	setNextIdStmt;
	delete	getNextIdStmt;
	delete	getNextIdStmtNoForUpdateClause; // PYR-45721
	delete	selectAndSetNextIdStmt; // PYR-45721
	delete	insertIdRangeStmt;
	delete	getIdRangesStmt;
	delete	getCurrentIdRangesStmt;

	_zeroStatements();
}

/*lint -save -e423 */
void DbmGenerator::_zeroStatements()
{
	getNextIdStmt		= 0;
	setNextIdStmt		= 0;
	insertObjectRowStmt	= 0;
	selectAndSetNextIdStmt = nullptr; // PYR-45721
	getNextIdStmtNoForUpdateClause = nullptr; // PYR-45721
	insertIdRangeStmt	= 0;
	getIdRangesStmt		= 0;
	getCurrentIdRangesStmt = 0;
}
/*lint -restore */

DbmGenerator::~DbmGenerator()
{
} //-V773

void DbmGenerator::saveNextId64( const char* objectName, UINT64 id )
{
	PLog("g64=%s", objectName); // log added to help investigating cross-accessed threads deadlock on GENERATOR64 table 
	setNextIdStmt->init( objectName, id );
	if( !setNextIdStmt->execUpdateDelete() )
	{
		char buf[32];
		PLog( LOG_TRACE_FAULTY_LOGIC ": failed to update GENERATOR64: '%s' %s", objectName, p_u64toa( id, buf ) );
	}
}

bool DbmGenerator::getId64FromTable( const char* objectName, UINT64& id )
{
	bool found = false;
	// PYR-45721
	UINT64 origId = 0;
	if( useSelectWithUpdate)
	{
		// For cases where slave generator is not used we can directly increment the value in the GENERATOR64 table.
		if( !usingSlaveGenerator( objectName ) )
		{
			return updateObjectRow64( objectName, false /*tryInsert*/, 1 /*idIncrement*/, id );
		}
		// When slave generator is used we must first extract the value, adjust and then increment using the adjusted value.
		found = selectObjectRowWithUpdateLock64( objectName, origId );
		if( found )
		{
			id = origId;
		}
	}
	else // old way - #mgrosman eventually remove
	{
		getNextIdStmt->setObjectName( objectName );
		getNextIdStmt->execute();
		if( getNextIdStmt->fetch() )
		{
			found = true;
			id = getNextIdStmt->getObjectId();
		}
		getNextIdStmt->closeCursor();
	}
	//no such object found in table
	if ( !found )
	{
		return false;
	}
	//object found, increase it's id value
	// PYR-27418
	INT32 increment = 1;
	UINT64 nextIdToSave;
	if( !useSelectWithUpdate || usingSlaveGenerator( objectName ) ) // PYR-45721 - call check And Adjust NextId only for the old case or when using slave generator
	{
		nextIdToSave = checkAndAdjustNextId( objectName, id, increment );
	}
	else // PYR-45721 - we already incremented generator value by 1
	{
		nextIdToSave = id + 1;
	}

	if( !useSelectWithUpdate ) // old way - #mgrosman eventually remove
	{
		saveNextId64( objectName, nextIdToSave );
	}
	else if( usingSlaveGenerator( objectName ) )
	{
		// in case of slave generator the nextIdToSave could be != id + increment,
		// so we cannot pass increment directly as a parameter to updateObjectRow64()
		UINT64 currentIdDummy;
		PASSERT( nextIdToSave > origId );
		if( !updateObjectRow64( objectName, false /*tryInsert*/, nextIdToSave - origId /*idIncrement*/, currentIdDummy ) )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": getId64FromTable(%s) - failed to update GENERATOR64: %" PRIu64, objectName, nextIdToSave );
		}
		else if( currentIdDummy != origId )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": getId64FromTable(%s) - origId != currentIdDummy (%" PRIu64 "!=%" PRIu64 ")", objectName, origId, currentIdDummy );
		}
	}
	return true;
}

void	DbmGenerator::insertObject( const char* objectName, UINT32 objectId )
{
/*lint -save -e747 */
	insertObject64(objectName, objectId);
/*lint -restore */
}

void	DbmGenerator::insertObject64( const char* objectName, UINT64 objectId )
{
	insertObjectRowStmt->init(objectName, objectId);
	insertObjectRowStmt->execute();
}

// PYR-24146
/*static*/ const char* DbmGenerator::getHandCounterName( UINT32 siteId )
{
	const char* objectName = ACTIVEHANDS_TABLE_NAME; // default one - for HandId
	switch( siteId )
	{
//	<<== @site2-hands-generator: Hands generators for segregated sites @ ====
	case PokerStars_Ita:
		objectName = "HandsCounterIT";
		break;
	case PokerStars_Fra:
		objectName = "HandsCounterFR";
		break;
	case PokerStars_Spa:
		objectName = "HandsCounterES";
		break;
	case PokerStars_US_NJ:	// PYR-27418
		objectName = "HandsCounterUSNJ";
		break;
	case PokerStars_Por:	// PYR-43995
		objectName = "HandsCounterPT";
		break;
	case PokerStars_Ind:	// PYR-43995
		objectName = "HandsCounterIN";
		break;
	case PokerStars_US_PA:	// PYR-70714
		objectName = "HandsCounterUSPA";
		break;
	case PokerStars_US_CO:	// PYR-124292
		objectName = "HandsCounterUSCO";
		break;
	case PokerStars_DE:	// PYR-124555
		objectName = "HandsCounterDE";
		break;
//	==== @site2-hands-generator: Hands generators for segregated sites @ ==>>
	}
	return objectName;
}
//what id will be returned by next call to getNextId()?
// PYR-24146 - will return a site specific hand counter if siteId is segregated
UINT64 DbmGenerator::peekNextHandId64( UINT32 siteId )
{
	const char* objectName = getHandCounterName( siteId ); // #24146
	// PYR-27418 - handIds must be in shared space between NJ and IOM sites
	if( siteId == PokerStars_Com && sharedGenerators.find( objectName ) == sharedGenerators.end() )
	{
		PString errMsg = objectName;
		throw PError( errMsg.append( " - missing from shared generators list" ) );
	}
	if( useSelectWithUpdate ) // PYR-45721
	{
		UINT64 objectId = 0;
		updateObjectRow64( objectName, true /*tryInsert*/, 0 /*idIncrement*/, objectId );
		return objectId;
	}
	// old way - #mgrosman eventually remove
	getNextIdStmt->setObjectName( objectName );
	getNextIdStmt->execute();
	bool found = getNextIdStmt->fetch();
	getNextIdStmt->closeCursor();
	//no such object found in table, will return 1 from getNextId
	if ( !found )
	{
		PASSERT( !usingSlaveGenerator( objectName ) ); // PYR-27418 - only possible for master generator or if slave then it should be local
		insertObject( objectName, 1 );
		return 1;
	}
	return getNextIdStmt->getObjectId();
}

UINT32 DbmGenerator::peekNextIdNoInsert( const char* objectName )
{
	return (UINT32) peekNextId64NoInsert(objectName);
}

UINT64 DbmGenerator::peekNextId64NoInsert( const char* objectName, bool unCached /*=false*/ )
{
	// PYR-27418
	assertObjectNameValid( objectName, unCached /*PYR-40317*/ );

	//////////////// #4903 ////////////////
	// PYR-27418 - only ask cache if unCached param is false, otherwise ask DB directly
	if(!unCached && rangeIncrement > 1 //we use new "cached id" functionality
		)
	{
		ObjectIdRange* range = getObjectRange(objectName);
		if(range && !range->isEmpty())
		{
			return range->begin;
		}
		//else read it from table, see below
	}
	//////////////// #4903 end ////////////////
	// PYR-45721 - since this can be called on RO dbm, we cannot use the new "select with update" statement
	// and should not use the old "Select ... FOR UPDATE WITH RS" statement.
	getNextIdStmtNoForUpdateClause->setObjectName( objectName );
	getNextIdStmtNoForUpdateClause->execute();
	bool found = getNextIdStmtNoForUpdateClause->fetch();
	getNextIdStmtNoForUpdateClause->closeCursor();
	PASSERT5(found || !usingSlaveGenerator( objectName )); // PYR-27418 - "not found" for shared generator can only be possible for master generator
	return found ? getNextIdStmtNoForUpdateClause->getObjectId() : 1;
}

UINT64 DbmGenerator::getNextId64UnCached( const char* objectName )
{
	// PYR-27418
	assertObjectNameValid( objectName, true /*PYR-40317*/ );

	UINT64 nextId = 0;
	if ( getId64FromTable( objectName, nextId ) )
	{
		return nextId;
	}
	PASSERT5( !usingSlaveGenerator( objectName ) ); // PYR-27418 - only possible for master generator or if slave it must local
	insertObject( objectName, 2 );
	return 1;
}

UINT64 DbmGenerator::getNextId64( const char* objectName )
{
	// PYR-27418
	assertObjectNameValid( objectName );

	if( rangeIncrement < 2 //old functionality
		)
	{
		return getNextId64UnCached(objectName);
	}
	/// new "cached id ranges" functionality
	ObjectIdRange* range = getObjectRange(objectName);
	if(!range) //not found
	{
		// PYR-27418 - can only happen in the master generator or slave in local mode
		PASSERT5( !usingSlaveGenerator( objectName ) );
		range = addObjectRange(objectName);
	}
	else
	{
		if(range->isEmpty())
		{
			replenishRange(objectName, range);
		}
	}
	return range->begin++;
}
// PYR-38720
void DbmGenerator::getNext2Ids64( const char* objectName, UINT64& id1, UINT64& id2 )
{
	// PYR-27418
	assertObjectNameValid( objectName );

	if( rangeIncrement < 2 //old functionality
		)
	{
		id1 = getNextId64UnCached( objectName );
		id2 = getNextId64UnCached( objectName );
		return;
	}
	/// new "cached id ranges" functionality
	ObjectIdRange* range = getObjectRange( objectName );
	if( !range ) //not found
	{
		// PYR-27418 - can only happen in the master generator or slave in local mode
		if( idRangeConn && sharedGenerators.find( objectName ) != sharedGenerators.end() )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": !range for slave dbm shared generator '%s'!", objectName );
		}
		range = addObjectRange( objectName );
		// We can get at least two ids out of the new range, since rangeIncrement >= 2
	}
	else
	{
		if( range->idsRemaining() < 2 ) // not enough left in the current range to produce two consecutive ids
		{
			replenishRange( objectName, range );
		}
	}
	id1 = range->begin++;
	id2 = range->begin++;
}

DbmGenerator::ObjectIdRange* DbmGenerator::getObjectRange( const char* objectName )
{
	PStringMap<ObjectIdRange>::iterator it = rangesMap.find( objectName ); 
	if(rangesMap.end() == it) //it's not in the map
	{
		UINT64 currentId = 0;
		bool found = false;
		// PYR-45721
		UINT64 origId = 0;
		if( useSelectWithUpdate )
		{
			// For cases where slave generator is not used we can directly increment the value in the GENERATOR64 table.
			if( !usingSlaveGenerator( objectName ) )
			{
				found = updateObjectRow64( objectName, false /*tryInsert*/, rangeIncrement, origId );
			}
			else
			{
				// When slave generator is used we must first extract the value, adjust and then increment using the adjusted value.
				// Using new select with update lock
				found = selectObjectRowWithUpdateLock64( objectName, origId );
			}
			if( found )
			{
				currentId = origId;
			}
		}
		else // old way - #mgrosman eventually remove
		{
			getNextIdStmt->setObjectName( objectName ); //does it exist in the table?
			getNextIdStmt->execute();
			found = getNextIdStmt->fetch();
			getNextIdStmt->closeCursor();
			if( found )
			{
				currentId = getNextIdStmt->getObjectId();
			}
		}
		if( !found )
		{
			return nullptr; //not in the table, either
		}
		//range exists in the table
		// PYR-27418 - check if rangeIncrement fits into the current range for slave generator
		INT32 increment = rangeIncrement;
		UINT64 nextIdToSave;
		if( !useSelectWithUpdate || usingSlaveGenerator( objectName ) ) // PYR-45721 - call check And Adjust NextId only for the old case or when using slave generator
		{
			nextIdToSave = checkAndAdjustNextId( objectName, currentId, increment );
		}
		else // PYR-45721 - we already incremented generator value by rangeIncrement
		{
			nextIdToSave = currentId + rangeIncrement;
		}
		ObjectIdRange newRange;
		newRange.begin = currentId;
		newRange.end = newRange.begin + increment - 1;
		// PYR-45721
		if( !useSelectWithUpdate ) // old way - #mgrosman eventually remove
		{
			saveNextId64( objectName, nextIdToSave );
		}
		else if( usingSlaveGenerator( objectName ) )
		{
			// in case of slave generator the nextIdToSave could be != currentId + increment,
			// so we cannot pass increment directly as a parameter to updateObjectRow64()
			UINT64 currentIdDummy;
			PASSERT( nextIdToSave > origId );
			if( !updateObjectRow64( objectName, false /*tryInsert*/, nextIdToSave - origId /*idIncrement*/, currentIdDummy ) )
			{
				PLog( LOG_TRACE_FAULTY_LOGIC ": getObjectRange(%s) - failed to update GENERATOR64: %" PRIu64, objectName, nextIdToSave );
			}
			else if( currentIdDummy != origId )
			{
				PLog( LOG_TRACE_FAULTY_LOGIC ": getObjectRange(%s) - origId != currentIdDummy (%" PRIu64 "!=%" PRIu64 ")", objectName, origId, currentIdDummy );
			}
		}
		it = rangesMap.insert(PStringMap<ObjectIdRange>::value_type(objectName, newRange)).first;
		
		ObjectIdRange dummyRange;
		dummyRange.begin = 1; /// so it will be deleted in generator.rollback()
		dummyRange.end = rangeIncrement;
		// saving previous values of range in rangesPrevMap
		pair< PStringMap<ObjectIdRange>::iterator, bool > res = rangesPrevMap.insert(PStringMap<ObjectIdRange>::value_type(objectName, dummyRange));
		if (!res.second)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": failed to insert dummyRange into rangesPrevMap: '%s'", objectName);
		}
	}
	return &(*it).second;
}

void DbmGenerator::replenishRange(const char* objectName, ObjectIdRange* range)
{
	bool found = false;
	UINT64 id = 0;
	UINT64 origId = 0;
	if( useSelectWithUpdate ) // PYR-45721
	{
		// For cases where slave generator is not used we can directly increment the value in the GENERATOR64 table.
		if( !usingSlaveGenerator( objectName ) )
		{
			found = updateObjectRow64( objectName, false /*tryInsert*/, rangeIncrement, origId );
		}
		else
		{
			// When slave generator is used we must first extract the value, adjust and then increment using the adjusted value.
			// Using new select with update lock
			found = selectObjectRowWithUpdateLock64( objectName, origId );
		}
		if( found )
		{
			id = origId;
		}
	}
	else // old way - #mgrosman eventually remove
	{
		getNextIdStmt->setObjectName( objectName );
		getNextIdStmt->execute();
		if( getNextIdStmt->fetch() )
		{
			found = true;
			id = getNextIdStmt->getObjectId();
		}
		getNextIdStmt->closeCursor();
	}
	if(!found)
	{
        // #15229 - this situation may happen in case of physical rollback not
        // followed by logical rollback if PSqlDeadlock is not caught in the
        // caller's code. In such case we simply want to substitute the id with
        // the end of the existing range that was just exhausted, and hope
        // that on some n-th attempt table update will not cause another 
        // deadlock exception (IS)
        char buf[64];
        id = range->end + 1;
		PLog("replenishRange error: objectName='%s' is not found, recovering with objectId=%s", objectName, p_u64toa(id, buf));
		// PYR-27418 - can only happen in the master generator or slave in local mode
		PASSERT5( !usingSlaveGenerator( objectName ) );
	}

	rangesPrevMap.insert(PStringMap<ObjectIdRange>::value_type(objectName, *range));
	
	// PYR-27418 - check if rangeIncrement fits into the current range for slave generator
	INT32 increment = rangeIncrement;
	UINT64 nextIdToSave;
	if( !useSelectWithUpdate || usingSlaveGenerator( objectName ) ) // PYR-45721 - call check And Adjust NextId only for the old case or when using slave generator
	{
		nextIdToSave = checkAndAdjustNextId( objectName, id, increment );
	}
	else // PYR-45721 - we already incremented generator value by rangeIncrement
	{
		nextIdToSave = id + rangeIncrement;
	}
	range->begin = id;
	range->end = id + increment - 1;
	if( !useSelectWithUpdate ) // old way - #mgrosman eventually remove
	{
		saveNextId64( objectName, nextIdToSave );
	}
	else if( usingSlaveGenerator( objectName ) ) // PYR-45721
	{
		// in case of slave generator the nextIdToSave could be != id + increment,
		// so we cannot pass increment directly as a parameter to updateObjectRow64()
		UINT64 currentIdDummy;
		PASSERT( nextIdToSave > origId );
		if( !updateObjectRow64( objectName, false /*tryInsert*/, nextIdToSave - origId /*idIncrement*/, currentIdDummy ) )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": replenishRange(%s) - failed to update GENERATOR64: %" PRIu64, objectName, nextIdToSave );
		}
		else if( currentIdDummy != origId )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": replenishRange(%s) - origId != currentIdDummy (%" PRIu64 "!=%" PRIu64 ")", objectName, origId, currentIdDummy );
		}
	}
}

DbmGenerator::ObjectIdRange* DbmGenerator::addObjectRange(const char* objectName)
{
	ObjectIdRange newRange;
	newRange.begin = 1;
	newRange.end = rangeIncrement;

	insertObject64( objectName, (UINT64)rangeIncrement + 1 );

	// saving previous values of range in rangesPrevMap
	rangesPrevMap.insert(PStringMap<ObjectIdRange>::value_type(objectName, newRange));
	
	PStringMap<ObjectIdRange>::iterator it = rangesMap.insert(PStringMap<ObjectIdRange>::value_type(objectName, newRange)).first;
	return &(*it).second;
}

void DbmGenerator::commit()
{
	clearPrevMap();
}

void DbmGenerator::clearPrevMap()
{
	rangesPrevMap.clear();
}

void DbmGenerator::rollback()
{
	PLog("DbmGenerator::rollback rangesPrevMap.size=%d", rangesPrevMap.size());

	for(PStringMap<ObjectIdRange>::const_iterator it = rangesPrevMap.begin(); rangesPrevMap.end() != it; ++it)
	{
		const ObjectIdRange& prev_range = (*it).second;
		PStringMap<ObjectIdRange>::iterator it_curr = rangesMap.find((*it).first);
		if(rangesMap.end() != it_curr)
		{
			if(1 == prev_range.begin)
			{
				PLog("erasing object %s", (*it_curr).first);
				rangesMap.erase(it_curr);
			}
			else
			{
				PLog("restoring object %s begin=%" PRIu64 " end=%" PRIu64, (*it_curr).first, prev_range.begin, prev_range.end);
				(*it_curr).second.begin = prev_range.begin;
				(*it_curr).second.end   = prev_range.end;
			}

		}
	}
	rangesPrevMap.clear();
}

// PYR-27418
void DbmGenerator::addObjectName( const char* objectName, bool local )
{
#if PTRACELVL >= 5
	if( localGenerators.find( objectName ) != localGenerators.end() || sharedGenerators.find( objectName ) != sharedGenerators.end() )
	{
		PTRACE5( "'%s' is already in generators list", objectName );
	}
#endif
	PLog( "Generator '%s' added (%s)", objectName, local ? "local" : "shared" );
	if( local )
	{
		localGenerators.insert( objectName );
	}
	else
	{
		sharedGenerators.insert( objectName );
	}
}

bool DbmGenerator::init( INT32 remainingIdsThreshold_, INT32 requiredIdsBuffer_, DbmGeneratorCallback* initFinishedCallback )
{
	// nothing to do for master generator or no new functionality
	if( !useSharedGenerators() || !idRangeConn )
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": !useSharedGenerators() || !idRangeConn");
		PASSERT(0);
	}
	remainingIdsThreshold = remainingIdsThreshold_;
	requiredIdsBuffer = requiredIdsBuffer_;

	PLog( "Reading existing Id ranges" );
	PStringMap<INT64> currentRangesMap;
	getIdRangesStmt->execute();
	while( getIdRangesStmt->fetch() )
	{
		const char* objectName = getIdRangesStmt->getObjectName();
		if( sharedGenerators.find( objectName ) == sharedGenerators.end() )
		{
			// It's not the shared Id range this dbm controls, so skip it
			PLog( " skipping '%s' - not mine or not shared", objectName );
			continue;
		}
		const UINT64 idRangeStart = getIdRangesStmt->getStartRange();
		const UINT64 idRangeEnd = getIdRangesStmt->getEndRange();
		INT64 idsLeft = idRangeEnd - idRangeStart + 1;
		if( getIdRangesStmt->getObjectId() > idRangeStart ) // current Id is inside this range
		{
			idsLeft = idRangeEnd - getIdRangesStmt->getObjectId() + 1;
		}
		pair< PStringMap<INT64>::iterator, bool > result = currentRangesMap.insert( make_pair( objectName, idsLeft ) );
		if( !result.second )
		{
			result.first->second += idsLeft; // we accumulate the number of unused Ids
		}
		char s1[32], s2[32], s3[32];
		PLog( "Id range '%s' initialized: %s - %s (id=%s)", objectName, p_u64toa( getIdRangesStmt->getStartRange(), s1 ), p_u64toa( getIdRangesStmt->getEndRange(), s2 ), p_u64toa( getIdRangesStmt->getObjectId(), s3 ) );
	}
	getIdRangesStmt->closeCursor();
	PLog( "Finished reading %d ranges", (int)currentRangesMap.size() );
	// Now request all the missing ranges
	INT32 numObjects = 0;
	size_t shift = 0;
	CommMsgBody request;
	PStringSet* requestsSent = new PStringSet;
	request.composeVectorSize( numObjects, shift );
	for( PStringSet::const_iterator it = sharedGenerators.begin(); it != sharedGenerators.end(); ++it )
	{
		INT32 idsToAcquire = requiredIdsBuffer;
		const char* const objectName = *it;
		PStringMap<INT64>::const_iterator currentRangesIt = currentRangesMap.find( objectName );
		if( currentRangesIt != currentRangesMap.end() )
		{
			const INT64 idsLeft = currentRangesIt->second;
			// skip where range already exists and there's enough Ids left
			if( idsLeft >= remainingIdsThreshold )
			{
				continue;
			}
			idsToAcquire -= idsLeft;
		}
		PLog( "Acquiring %d Ids for '%s'", idsToAcquire, objectName );
		//44<s9> - reqId, num<objectName,rangeSize>
		request
			.composeString( objectName )
			.composeINT32( idsToAcquire )
			;
		requestsSent->insert( objectName );
		idRangeRequestsSent.insert( objectName );
		++numObjects;
	}

	delete requestsSent;
	delete initFinishedCallback;
	return true;
}

INT16 DbmGenerator::getIdRangeForSlave( const char* objectName, INT32 rangeSize, UINT64& startId, UINT64& endId, PString& sqlErr )
{
	if( idRangeConn ) // must only be called in the master generator
	{
		sqlErr = "idRangeConn != 0";
		PLog( LOG_TRACE_FAULTY_LOGIC ": %s", sqlErr.c_str() );
		PASSERT5(0);
		return INTERNAL_LOGICAL_ERROR;
	}
	if( sharedGenerators.find( objectName ) == sharedGenerators.end() )
	{
		sqlErr.assign("'").append(objectName).append("' is not in the list of shared generators");
		PLog( LOG_TRACE_FAULTY_LOGIC ": %s", sqlErr.c_str() );
		PASSERT5(0);
		return INCONSISTENT_DATA;
	}
	try
	{
		if( useSelectWithUpdate )
		{
			// PYR-45721
			if( !updateObjectRow64( objectName, true /*tryInsert*/, rangeSize, startId ) )
			{
				sqlErr.assign( "updateObjectRow64(" ).append( objectName ).append( "," ).appendInt64( rangeSize );
				PLog( LOG_TRACE_FAULTY_LOGIC ": %s", sqlErr.c_str() );
				return INTERNAL_LOGICAL_ERROR;
			}
			endId = startId + rangeSize - 1;
			return DBM_NO_ERROR;
		}
		// use the old way - #mgrosman eventually remove
		getNextIdStmt->setObjectName( objectName ); //does it exist in the table?
		getNextIdStmt->execute();
		bool found = getNextIdStmt->fetch();
		getNextIdStmt->closeCursor();
		if( !found )
		{
			startId = 1;
			endId = rangeSize;
			insertObject64( objectName, rangeSize + 1 );
		}
		else
		{
			startId = getNextIdStmt->getObjectId();
			endId = startId + rangeSize - 1;
			setNextIdStmt->init( objectName, endId + 1 );
			setNextIdStmt->execUpdateDelete();
		}

		return DBM_NO_ERROR;
	}
	catch( const PSqlError& er )
	{
		sqlErr = er.why();
		return er.code();
	}
}

// Check and adjust the next Id from received ranges for slave generator
UINT64 DbmGenerator::checkAndAdjustNextId( const char* objectName, UINT64& currentId, INT32& increment )
{
	PASSERT( increment > 0 );
	UINT64 nextIdToSave = currentId + increment;
	// if not using slave generator received ranges - do nothing
	if( !usingSlaveGenerator( objectName ) )
	{
		return nextIdToSave;
	}

	// get all current ranges
	getCurrentIdRangesStmt->init( objectName, currentId );
	getCurrentIdRangesStmt->execute();
	list<ObjectIdRange> rangeList;
	while( getCurrentIdRangesStmt->fetch() )
	{
		rangeList.push_back( ObjectIdRange() );
		ObjectIdRange& range = rangeList.back();
		range.begin = getCurrentIdRangesStmt->getStartRange();
		range.end = getCurrentIdRangesStmt->getEndRange();
	}
	getCurrentIdRangesStmt->closeCursor();
	rangeList.sort( ObjectIdRangeComp() );
	if( rangeList.empty() )
	{
		char buf[32];
		PLog( LOG_TRACE_FAULTY_LOGIC ": no received ID ranges for '%s' id=%s", objectName, p_u64toa( currentId, buf ) );
		throw PSqlError( "No received ID ranges" );
	}

	char s1[32], s2[32], s3[32]; //-V808
	const UINT64 firstRangeBegin = rangeList.front().begin;
	const UINT64 firstRangeEnd = rangeList.front().end;
	if( currentId < firstRangeBegin )
	{
		// invalid value in GENERATOR64 - currentId must always be inside the first range
		PLog( LOG_TRACE_FAULTY_LOGIC ": currentId for '%s' is not inside the first range (%s < %s) - correcting. Somebody manually corrected GENERATOR64 table?", objectName, p_u64toa( currentId, s1 ), p_u64toa( firstRangeBegin, s2 ) );
		currentId = firstRangeBegin; // let's correct it
		nextIdToSave = currentId + increment; // also correct the last element
	}
	if( nextIdToSave > firstRangeEnd ) // this range is exhausted?
	{
		PTRACE5("Range for '%s' exhausted: %s > %s-%s", objectName, p_u64toa( nextIdToSave, s1 ), p_u64toa( firstRangeBegin, s2 ), p_u64toa( firstRangeEnd, s3 ) );

		increment = firstRangeEnd - currentId + 1; // adjust the increment to fit into the current range
		//get the next range
		list<ObjectIdRange>::const_iterator rangeIt = rangeList.begin();
		if( ++rangeIt == rangeList.end() )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": not enough received ID ranges for '%s'", objectName );
			// Let's try to correct this problem. It can only happen in some edge cases,
			//  e.g. after restart when only one range exists and rangeIncrement is greater or equal the remaining part of that range,
			//  or when IOM site was not available for a long time.
			if( increment == 1 )
			{
				// Cannot correct as only 1 Id left.
				throw PSqlError( "Not enough received ID ranges" );
			}
			// Leave 1 Id in the current range
			--increment;
			nextIdToSave = firstRangeEnd;
		}
		else
		{
			nextIdToSave = rangeIt->begin;
		}
	}
	// calculate how many Ids we have left and send request to master generator for more if necessary
	if( idRangeRequestsSent.find( objectName ) == idRangeRequestsSent.end() ) // request not sent yet
	{
		list<ObjectIdRange>::const_iterator rangeIt = rangeList.begin();
		// the next id is inside the first range or the start of the next range
		INT64 idsLeft = rangeIt->end - nextIdToSave + 1;
		if( idsLeft < 0 ) // nextIdToSave is the start of the next range
		{
			idsLeft = 0;
		}

		while( ++rangeIt != rangeList.end() )
		{
			idsLeft += rangeIt->end - rangeIt->begin + 1;
		}

	}
	return nextIdToSave;
}

void DbmGenerator::assertObjectNameValid( const char* objectName, const bool unCached /*=false, PYR-40317*/ ) const
{
	// If new functionality is used, make sure that objectName is correct -
	// meaning that it must be either shared or local generator.
	if( useSharedGenerators()
		&& sharedGenerators.find( objectName ) == sharedGenerators.end()
		&& localGenerators.find( objectName ) == localGenerators.end()
		)
	{
		PLog( LOG_TRACE_FAULTY_LOGIC ": '%s' is not in the list of generators", objectName );
		PASSERT5( 0 );
	}
	// PYR-40317
	if( !unCached && dbManagerCommon->isMainThreadManager() && isMonotonicByUserId( objectName ) )
	{
		PLog( LOG_TRACE_FAULTY_LOGIC ": !userId for monotonic id '%s'. Next id might not be greater than the previous one!", objectName );
	}
}
// PYR-40317
void DbmGenerator::addMonotonicObjectName( const char* objectName )
{
#if PTRACELVL >= 5
	if( monotonicByUserIdGenerators.find( objectName ) != monotonicByUserIdGenerators.end() )
	{
		PTRACE5( "'%s' is already in monotonic generators list", objectName );
	}
#endif
	PLog( "Generator '%s' added (monotonic)", objectName );
	monotonicByUserIdGenerators.insert( objectName );
}

// PYR-48098 - refactored common functionality from updateObjectRow64()
bool DbmGenerator::_callGetAndUpdateNextId( const char* objectName, const UINT64 idIncrement, UINT64& oldObjectId )
{
	bool found;

	selectAndSetNextIdStmt->init(objectName, idIncrement);
	selectAndSetNextIdStmt->execUpdateDelete();
	found = selectAndSetNextIdStmt->fetch();
	selectAndSetNextIdStmt->closeCursor();
	if (found)
	{
		oldObjectId = selectAndSetNextIdStmt->getObjectId();
	}

	return found;
}

// PYR-45721 - select with update for GENERATOR64 table.
// If row not found it attempts to insert one and if that fails the exception is thrown.
bool DbmGenerator::updateObjectRow64( const char* objectName, const bool tryInsert, const UINT64 idIncrement, UINT64& oldObjectId )
{
	if (idIncrement > 1)
	{
		PLog("G64=%s", objectName); // log added to help investigating cross-accessed threads deadlock on GENERATOR64 table 
	}
	// PYR-45721 - optimistic scenario - assume the row exists
	// PYR-48098 - refactored common functionality to be used in catch() clause
	const bool found = _callGetAndUpdateNextId( objectName, idIncrement, oldObjectId );
	if( found )
	{
		return true;
	}
	else if( !tryInsert )
	{
		return false;
	}

	try
	{
		oldObjectId = 1;
		insertObject64( objectName, idIncrement + 1 );
	}
	catch( const PSqlError& er )
	{
		if( !er.sqlState.equals( DUPLICATE_KEY_ERROR_STR ) )
		{
			throw;
		}
		// assume a repeated request to insert the same record - try update again
		const bool found = _callGetAndUpdateNextId( objectName, idIncrement, oldObjectId );
		if( !found )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": failed select with update for '%s', %" PRIu64 "!", objectName, idIncrement );
			throw;
		}
	}
	return true;
}

bool DbmGenerator::selectObjectRowWithUpdateLock64( const char* objectName, UINT64& objectId )
{
	return updateObjectRow64( objectName, false /*tryInsert*/, 0 /*idIncrement*/, objectId );
}
