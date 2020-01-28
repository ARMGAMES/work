// icestructs.h 
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef icestructs_h_included
#define icestructs_h_included

#include "composecolls.h"
#include "pplib.h"
#include "vectorutils.h"
#include "IceShared.h"
#include "newsmessages.h"

//////////////////////////////////////////////////////////////////////////
// events

class ObjectCounterInterface
{
public:
	virtual void changeCount( const bool isAdded )
	{
		PLog( "oci - no subclass" );
	};
	virtual ~ObjectCounterInterface() {}
};

class IceEvent : public IceParamVector
{
	// user is composed as a part of olap message and is not par of the event itself

	INT32			sourceId	= 0;	// not composed by 'composeForDbm' but composed by 'reComposeForIceOlap'
	INT64			eventId		= 0;	// not composed by 'composeForDbm' but composed by 'reComposeForIceOlap'
	INT32			dbmId		= 0;		// not composed by 'composeForDbm' - composed in olap sysInfo


	UINT32			olapMsgId	= 0;	// composed by 'composeForDbm' but thrown out by 'reComposeForIceOlap'
	INT16			eventType	= 0;	// composed by 'composeForDbm'
	BYTE			version		= 0;	// composed by 'composeForDbm'
	bool			valid		= false;// not composable

	ObjectCounterInterface* oci = nullptr;

public:
	void setObjectCounter( ObjectCounterInterface* oci_ )
	{
		if( oci )
			oci->changeCount( false );

		oci = oci_;

		if( oci )
			oci->changeCount( true );
	}
	IceEvent( UINT32 olapMsgId_, INT16 eventType_ ) 
		: sourceId(0), eventId(0), dbmId(0), olapMsgId(olapMsgId_), eventType(eventType_), version(0), valid(true), oci( nullptr )
	{
	}
	IceEvent( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ ) 
		: sourceId(sourceId_), eventId(eventId_), dbmId(0), olapMsgId(olapMsgId_), eventType(eventType_), version(0), valid(true), oci( nullptr )
	{
	}
	// on parse dbmId must be specified - as we parse it on the ice lobby itself and are expecting to work with it
	// olapMsgId -	now that we can forward an event and because olapMsgId is not composed as a part of the event
	//				we need a way to pass it to the generic constructor from the parser as well
	//
	IceEvent( CommMsgParser& parser, const INT32 dbmId_, const INT32 olapMsgId_ ) : dbmId(dbmId_), olapMsgId(olapMsgId_), valid(true), oci( nullptr )
	{
		parseFromIceOlap(parser);
	}
	IceEvent( const IceEvent& rhs ) : IceParamVector( rhs ), oci( nullptr )
	{
		sourceId	= rhs.sourceId	;
		eventId		= rhs.eventId	;
		dbmId		= rhs.dbmId		;
		olapMsgId	= rhs.olapMsgId	;
		eventType	= rhs.eventType	;
		version		= rhs.version	;
		valid		= rhs.valid		;

		setObjectCounter( rhs.oci )	;
	}
	IceEvent& operator=( const IceEvent& rhs )
	{
		if( this != &rhs )
		{
			IceParamVector::operator=( rhs ); 
			
			sourceId	= rhs.sourceId	;
			eventId		= rhs.eventId	;
			dbmId		= rhs.dbmId		;
			olapMsgId	= rhs.olapMsgId	;
			eventType	= rhs.eventType	;
			version		= rhs.version	;
			valid		= rhs.valid		;

			setObjectCounter( rhs.oci )	;
		}
		return *this;
	}
	const vector<IceParamData>& getParams() const
	{
		return params;
	}
	virtual ~IceEvent()
	{
		if( oci )
		{
			oci->changeCount( false );
		}
	}
protected:
	// composes for dbm
	CommMsgBody& composeForDbm( CommMsgBody& body ) const
	{
		body.composeINT32(olapMsgId)
			// data below will be recomposed by reComposeForIceOlap
			.composeINT16(eventType)
			.composeBYTE(version)
			;
		IceParamVector::compose(body);

		return body;
	}
	// needed only if we want to parse an event as a specific event type in order to add more event parameters
	// along the way traveled by the event
	//
	void parseByDbm( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
	{
		eventId		= eventId_;
		sourceId	= sourceId_;

		INT16 eventTypeTmp;
		BYTE versionTmp;
		parser
			.parseUINT32(olapMsgId)
			.parseINT16(eventTypeTmp)
			.parseBYTE(versionTmp)
			;
		IceParamVector::parse(parser);
		if( eventType == eventTypeTmp && version == versionTmp )
		{
			return;
		}
		PLog(	LOG_TRACE_FAULTY_LOGIC ": IceEvent::parseByDbm expected t %d v %d found t %d v %d", 
				eventType, version, eventTypeTmp, versionTmp );
		
		addParam( IceParamData( eIceEventParam_DbgExpectedEventType	, true, eventType	 ) ); 
		addParam( IceParamData( eIceEventParam_DbgReceivedEventType	, true, eventTypeTmp ) );

		// 
		eventType = 0;
		valid = false;
	}
	// composes as if dbm composed it
	CommMsgBody& composeForIceOlap( CommMsgBody& body ) const
	{
		body.composeINT32(sourceId)
			.composeINT64(eventId)
			.composeINT16(eventType)
			.composeBYTE(version)
			;
		IceParamVector::compose(body);

		return body;
	}
	
public:
	CommMsgBody & composeForForward( CommMsgBody& body ) const
	{
		return composeForIceOlap( body );
	}
	CommMsgParser& parseFromIceOlap( CommMsgParser& parser )
	{

		parser
			// composed by the source (dbm)
			.parseINT32(sourceId)
			.parseINT64(eventId)
			// composed by the originator (where it happened)
			.parseINT16(eventType)
			.parseBYTE(version)
			;
		IceParamVector::parse(parser);

		return parser;
	}
	static void reComposeForIceOlap( const CommMsgBody& bodyIn, CommMsgBody& bodyOut, UINT32& olapMsgId, INT64 eventId, INT32 sourceId )
	{
		CommMsgParser parser(bodyIn);

		// olapMsgId will not be recomposed
		parser.parseUINT32(olapMsgId);

		// compose data from the dbm
		bodyOut
			.composeINT32(sourceId)
			.composeINT64(eventId)
			// merge with remaining data of the event less olapMsgId
			.copyMsgBodyFromParser(parser)
			;

	}
	static void parseEventSourceAndIdFromParserCopy( CommMsgParser parserCopy, INT32& eventSourceId, INT64& eventId, INT16& eventType )
	{
		parserCopy
			// composed by the source (dbm)
			.parseINT32(eventSourceId)
			.parseINT64(eventId)
			// composed by the originator (where it happened)
			.parseINT16(eventType)
			;
	}
	INT16 getType() const
	{
		return eventType;
	}
	INT32 getSourceId() const
	{
		return sourceId;
	}
	INT64 getEventId() const
	{
		return eventId;
	}
	UINT32 getOlapMsgId() const
	{
		return olapMsgId;
	}
	bool isForceOptIn() const
	{
		return eventType == eIceEvent_ForceOptInBypassFilter;
	}
	bool isTournEvent() const 
	{
		return isTournEvent( eventType );
	}
	static bool isTournEvent( INT16 eventType )
	{
		return	eventType == eIceEvent_PlayedInTourn		||
				eventType == eIceEvent_KickedTournPlayer	||
				eventType == eIceEvent_ReceivedTournAward	||
				eventType == eIceEvent_FinishedTourn		||
				eventType == eIceEvent_TournHand;
	}
	bool isRingTableEvent() const 
	{
		return	isRingTableEvent( eventType );
	}
	static bool isRingTableEvent( INT16 eventType ) 
	{
		return	eventType == eIceEvent_RingHandPlayed;
	}
	bool isSelfSufficientEvent() const 
	{ 
		return	!(	eventType == eIceEvent_KickedTournPlayer	||
					eventType == eIceEvent_ReceivedTournAward	||
					eventType == eIceEvent_FinishedTourn		||
					eventType == eIceEvent_TournHand);
	}																				
	static bool isIgnoredWhenUserNotLoaded( INT16 eventType )					
	{																				
		return	eventType == eIceEvent_PrivilegeChange		|| 
				eventType == eIceEvent_UserTrackCodeChange	;
	}
	bool isIgnoredWhenUserNotLoaded() const
	{
		return isIgnoredWhenUserNotLoaded( eventType );
	}
	bool isValid() const
	{
		return valid;
	}
	INT32 getDbmId() const
	{
		return dbmId;
	}
	void print() const
	{
		PLog("e: m %d t %d id %lld s %d v %d d %d", olapMsgId, eventType, eventId, sourceId, version, dbmId );
		IceParamVector::print();
	}
	void printBrief( const INT32 userIntId, const char* strTrace = "" ) const
	{
		PLog("e:\t%d\t%d\t%d\t%d\t%lld %s", userIntId, eventType, sourceId, dbmId, eventId, strTrace );
	}

protected:
	IceParamVector::compose;
};

//////////////////////////////////////////////////////////////////////////

class CardMaskHelper
{
public:
	// rank 2-14 (2,3,4,5,6,7,8,9,10,11(j),12(q),13(k),14(a)
	// suit h,c,s,d
	//
	static bool getMask( const BYTE rank, const BYTE suit, INT64& mask )
	{
		INT8 shift = 0;
		switch( suit )
		{
		case 'h': break;
		case 'c': shift = 13; break;
		case 's': shift = 26; break;
		case 'd': shift = 39; break;
		default:
			PLog( "invalid suit %c", suit );
			return false;
		}
		if( rank > 14 || rank < 2 )
		{
			PLog( "invalid rank %d", rank );
			return false;
		}
		shift += ( rank - 2 );
		mask |= ( (INT64)1 << shift );
		return true;
	}
	static bool addCard( const BYTE rank, const BYTE suit, INT64& val )
	{
		INT64 mask = 0;
		if( !getMask( rank, suit, mask ) )
			return false;
		
		val |= mask;
		return true;
	}
	static bool checkCard( const INT64 val, BYTE rank, BYTE suit)
	{
		INT64 mask = 0;
		if( !getMask( rank, suit, mask ) )
			return false;

		return ( val & mask ) != 0;
	}
};

#endif // icestructs_h_included
