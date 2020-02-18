//dbmgenerator.h
#ifndef dbmgenerator_h_included
#define dbmgenerator_h_included

#include "pplib.h"
#include "ppcontainer.h"

class	DatabaseManagerCommon;
class	GetNextIdStmt;
class	InsertObjectRowStmt;
class	SetNextIdStmt;
class	SelectAndSetNextIdStmt;
class	CallGetAndUpdateNextIdStmt; 
class	CallInsertIdStmt; 


class InsertIdRangeStmt;
class GetIdRangesStmt;
class GetCurrentIdRangesStmt;
class IdRangeClientConnection;
class CommMsgParser;

#define	OBJECT_NAME_LEN				31 ////moved from "dbm.h"

class DbmGenerator
{
private:
	GetNextIdStmt*			getNextIdStmt;
	GetNextIdStmt*			getNextIdStmtNoForUpdateClause; 
	InsertObjectRowStmt*	insertObjectRowStmt;
	SetNextIdStmt*			setNextIdStmt;
	SelectAndSetNextIdStmt* selectAndSetNextIdStmt; 
	CallGetAndUpdateNextIdStmt* callGetAndUpdateNextIdStmt; 
	CallInsertIdStmt*		callInsertIdStmt; 

	InsertIdRangeStmt*		insertIdRangeStmt; // save the range received from master dbm
	GetIdRangesStmt*		getIdRangesStmt; // called on init to read all available Id ranges received or needed to be received from master dbm
	GetCurrentIdRangesStmt*	getCurrentIdRangesStmt; // called every time the current range is exhausted

	void insertObject( const char* objectName, UINT32 objectId );

	void insertObject64( const char* objectName, UINT64 objectId );
	bool updateObjectRow64( const char* objectName, const bool tryInsert, const UINT64 idIncrement, UINT64& oldObjectId ); // PYR-45721
	bool selectObjectRowWithUpdateLock64( const char* objectName, UINT64& objectId ); // PYR-45721
	bool _callGetAndUpdateNextId( const char* objectName, const UINT64 idIncrement, UINT64& oldObjectId ); // PYR-48098 - helper to be called from updateObjectRow64()

	bool getId64FromTable( const char* objectName, UINT64& id );
	void assertObjectNameValid( const char* objectName, const bool unCached = false /*PYR-40317*/ ) const; // PYR-27418 - PASSERT5 violation happens if not valid

	struct ObjectIdRange
	{
		UINT64 begin;
		UINT64 end;
		bool isEmpty() const { return begin > end; }
		UINT64 idsRemaining() const { return isEmpty() ? 0 : end - begin + 1; } // PYR-38720
		ObjectIdRange() { begin = 0; end = 0; }
	};

	INT32 rangeIncrement;
	PStringMap<ObjectIdRange> rangesMap;
	PStringMap<ObjectIdRange> rangesPrevMap;
	ObjectIdRange* getObjectRange( const char* objectName );
	ObjectIdRange* addObjectRange( const char* objectName );
	void replenishRange( const char* objectName, ObjectIdRange* range );

	// PYR-27418
	struct ObjectIdRangeComp
	{
		bool operator()( const ObjectIdRange& x, const ObjectIdRange& y ) const
		{
			bool isless = false;
			if( x.begin < y.begin )
			{
				isless = true;
			}
			if( ( isless && x.end >= y.begin ) || (!isless && y.end >= x.begin ) )
			{
				PString err;
				err.appendInt64(x.begin).append("-").appendInt64(x.end).append(" and ").appendInt64(y.begin).append("-").appendInt64(y.end);
				PLog( "ERROR: ObjectIdRanges intersect! %s", err.c_str() );
				PASSERT( 0 );
			}
			return isless;
		}
	};
	DatabaseManagerCommon*	dbManagerCommon;
	IdRangeClientConnection* idRangeConn; // connection to master generator
	PStringSet localGenerators; // all object names, which only use local generator table (no ranges to receive);
	PStringSet sharedGenerators; // all object names, which use master generator table from IOM site;
	PStringSet idRangeRequestsSent; // to prevent sending duplicate idrange requests
	INT32 remainingIdsThreshold; // we will ask for a new Id range if remaining number of Ids is less that this value
	INT32 requiredIdsBuffer; // how many Ids we should maintain in the slave generator

	PStringSet monotonicByUserIdGenerators; // PYR-40317
	bool useSelectWithUpdate; // PYR-45721 - use new way
	bool useAutonomousTransaction; // PYR-48098 - DB2 specific only NOT ready for other DBs

	void _zeroStatements();

	DbmGenerator(const DbmGenerator& other);
	const DbmGenerator& operator =(const DbmGenerator& other);

public:
	struct Generator // PYR-27418
	{
		const char* objectName;
		bool local;
	};
	
	class DbmGeneratorCallback // PYR-27418 - to be overriden in server object to do some tasks after generator is initialized (received all ranges)
	{
	protected:
		DbmGeneratorCallback() {}

	public:
		virtual ~DbmGeneratorCallback() {}
		virtual void process() = 0;
	};

	DbmGenerator();
	bool init( INT32 remainingIdsThreshold_, INT32 requiredIdsBuffer_, DbmGeneratorCallback* callback ); // PYR-27418
	void setIdRangeConn( IdRangeClientConnection* conn ) { idRangeConn = conn; } // PYR-27418 - called for slave generator
	void addObjectName( const char* objectName, bool local ); // PYR-27418 - to add generators dynamically
	bool useSharedGenerators() const { return sharedGenerators.size() > 0; } // PYR-27418
	INT16 getIdRangeForSlave( const char* objectName, INT32 rangeSize, UINT64& startId, UINT64& endId, PString& sqlErr ); // PYR-27418
	void processReceiveRangesRequest( CommMsgParser& parser, PStringSet& requestsSent, DbmGeneratorCallback* callback ); // PYR-27418
	void addMonotonicObjectName( const char* objectName ); // PYR-40317
	bool isMonotonicByUserId( const char* objectName ) const { return monotonicByUserIdGenerators.find( objectName ) != monotonicByUserIdGenerators.end(); } // PYR-40317

	void setRangeIncrement(UINT32 newIncrement) 
	{
		PLog("DbmGenerator: rangeIncrement=%u", newIncrement);
		rangeIncrement = newIncrement;
	}
	UINT32 getRangeIncrement() const
	{
		return rangeIncrement;
	}
	//void _init( DatabaseManagerCommon& dbmanager );
	void prepareStatements( DatabaseManagerCommon& dbmanager );
	void deleteStatements();
	~DbmGenerator();
	UINT64 getNextId64( const char* objectName );
	UINT64 getNextId64UnCached( const char* objectName );
	void getNext2Ids64( const char* objectName, UINT64& id1, UINT64& id2 ); // PYR-38720

	static const char* getHandCounterName( UINT32 siteId ); // #24146

	UINT64 peekNextHandId64( UINT32 siteId );
	UINT32 peekNextIdNoInsert( const char* objectName );
	UINT64 peekNextId64NoInsert( const char* objectName, bool unCached = false /*PYR-27418*/ );
	void saveNextId64( const char* objectName, UINT64 id ); // #mgrosman eventually remove - only used for the old way of updating GENERATOR64 table
	// PYR-27418 - this method must be called before saveNextId64 to ensure that the Id to be saved is not outside the current Ids range
	UINT64 checkAndAdjustNextId( const char* objectName, UINT64& currentId, INT32& increment /*in-out param*/ );
	bool usingSlaveGenerator( const char* objectName ) const { return idRangeConn && sharedGenerators.find( objectName ) != sharedGenerators.end(); }
	void setUseSelectWithUpdate( const bool useSelectWithUpdate_ ) // PYR-45721
	{
		useSelectWithUpdate = useSelectWithUpdate_;
		PLog("DbmGenerator: useSelectWithUpdate=%s", useSelectWithUpdate ? "yes" : "no");
	}
	void setUseAutonomousTransaction( const bool useAutonomousTransaction_ ) // PYR-48098
	{
		useAutonomousTransaction = useAutonomousTransaction_;
		PLog( "DbmGenerator: useAutonomousTransaction=%s", useAutonomousTransaction ? "yes" : "no" );
	}

	void commit();
	void rollback();
	void clearPrevMap();
};

#endif	//dbmgenerator_h_included
