// iceshared.h 
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef iceshared_h_included
#define iceshared_h_included

#include "HgValMap.h"
#include "vectorutils.h"
#include "invlogging.h"
#include "icesharedenums.h"
#include "dbm.h"
#include "composecolls.h"
#include "table.h"
#include "PBitmask.h"

//////////////////////////////////////////////////////////////////////////

class ParseUtils
{
	ParseUtils(){}
public:
	static bool isNumeric( const char* str )
	{
		const INT32 len = strlen( str );

		for( INT32 i = 0; i < len ; ++i )
		{
			if( !isdigit( str[i] ) && !( i == 0 && str[i] == '-' ) )
				return false;
		}
		return true;
	}
	static bool isAlphaNumeric( const char* str )
	{
		const INT32 len = strlen( str );

		for( INT32 i = 0; i < len; ++i )
		{
			if( !isdigit( str[i] ) && !isalpha( str[i] ) )
				return false;
		}
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////

class IceParamData;

class ParamCustomCompareInterface
{
public:
	//
	virtual ~ParamCustomCompareInterface()
	{
	}
	// data type of the event parameter this comparator understands
	virtual INT8 getCompareType() const = 0;
	//
	virtual INT16 getCompareParamId() const = 0;
	// comparison algorithm executed on the event parameter
	virtual int compare( const IceParamData& eventParamData ) const = 0;
	
	// not compare - using an xFilter for a custom action - TODO: make pure = 0 on clean-up
	virtual void* getActionObject( INT32 objectId ) const { return nullptr; };
	//
	virtual bool fromString( const char* str ) = 0;
	//
	virtual const char* toString( PString& str ) const = 0;
	//
	virtual ParamCustomCompareInterface* clone() const = 0;
	//
	virtual bool isInitSucceded() const = 0;
};

class ParamCustomFactoryInterface
{
public:
	virtual ParamCustomCompareInterface* createCustomCompareFilter( const INT16 paramId, const char* str ) const = 0;
	virtual ~ParamCustomFactoryInterface()
	{
	}
};

//////////////////////////////////////////////////////////////////////////
class IceParamData
{
	INT8	type;
	INT16	id;
	bool	isUnique;
	// extended parameter value syntax $<src>.<extended_id>
	// their type is determined by the type of parameter itself
	//
	// if ePt_Extended - similar to flag32
	// type		-	ePt_Extended
	// id		-	describes actual id
	// valStr	-	stores string value from db
	// var32	-	stores ID if the extended parameter
	// maskVal32-	stores extended data source id
	
	union //-V2514
	{
		bool			valBool;
		INT8			val8;
		INT16			val16;
		INT32			val32;
		INT64			val64;
	};
	union //-V2514
	{
		INT8			maskVal8;
		INT16			maskVal16;
		INT32			maskVal32;
		INT64			maskVal64;
	};

	PBitmask								flagsLong; 
	PBitmask								flagsLongMask;
	PString									valStr;
	SrvTime									valTime;
	PAutoPtr<CommMsgBody>					msgBody;
	PAutoPtr<ParamCustomCompareInterface>	objCmp;
	
	void clear()
	{
		valBool			= 0;
		val8			= 0;
		val16			= 0;
		val32			= 0;
		val64			= 0;
		maskVal8		= 0;
		maskVal16		= 0;
		maskVal32		= 0;
		maskVal64		= 0;
		flagsLong		= PBitmask::zero;
		flagsLongMask	= PBitmask::zero;
		valStr.cut(0);
		if( msgBody )
		{
			delete msgBody.extract();
		}
		if( objCmp )
		{
			delete objCmp.extract();
		}
	}
	
public:
	CommMsgBody* detachMsgBody()
	{
		if( type == ePt_MsgBody )
		{
			return msgBody.extract();
		}
		return NULL;
	}
	bool getVal( bool&  val )				const { if( type == ePt_Bool		)	{ val = valBool;	return true; } return false;	}
	bool getVal( INT8&  val )				const { if( type == ePt_INT8		)	{ val = val8;		return true; } return false;	}
	bool getVal( INT16& val )				const { if( type == ePt_INT16		)	{ val = val16;		return true; } return false;	}
	bool getVal( INT32& val )				const { if( type == ePt_INT32		)	{ val = val32;		return true; } return false;	}
	bool getVal( INT64& val )				const { if( type == ePt_INT64		)	{ val = val64;		return true; } return false;	}
	bool getVal( PString& val )				const { if( type == ePt_String		)	{ val = valStr;		return true; } return false;	}
	bool getVal( const char*& val )			const { if( type == ePt_String		)	{ val = valStr;		return true; } return false;	}
	bool getVal( SrvTime& val )				const { if( type == ePt_DateTime	)	{ val = valTime;	return true; } return false;	}
	bool getVal( const CommMsgBody*& val)	const { if( type == ePt_MsgBody		)	{ val = msgBody;	return true; } return false;	}
	bool getVal( CommMsgBody*& val)				  { if( type == ePt_MsgBody		)	{ val = msgBody;	return true; } return false;	}
	
	bool getFlagVal( INT8&  val, INT8&  mask ) const { if( type == ePt_Flags8	)	{ val = val8 ; mask = maskVal8 ; return true; } return false; }
	bool getFlagVal( INT16& val, INT16& mask ) const { if( type == ePt_Flags16	)	{ val = val16; mask = maskVal16; return true; } return false; }
	bool getFlagVal( INT32& val, INT32& mask ) const { if( type == ePt_Flags32	)	{ val = val32; mask = maskVal32; return true; } return false; }
	bool getFlagVal( INT64& val, INT64& mask ) const { if( type == ePt_Flags64	)	{ val = val64; mask = maskVal64; return true; } return false; }
	bool getFlagVal( PBitmask& val, PBitmask& mask )   const { if( type == ePt_FlagsLong)	{ val = flagsLong;mask = flagsLongMask;  return true; } return false; }
	
	//////////////////////////////////////////////////////////////////////////
	static bool getVal( bool&  val					, const IceParamData& ipd ) { if( ipd.type == ePt_Bool		) { val = ipd.valBool	;	return true; } return false; }
	static bool getVal( INT8&  val					, const IceParamData& ipd ) { if( ipd.type == ePt_INT8		) { val = ipd.val8		;	return true; } return false; }
	static bool getVal( INT16& val					, const IceParamData& ipd ) { if( ipd.type == ePt_INT16		) { val = ipd.val16		;	return true; } return false; }
	static bool getVal( INT32& val					, const IceParamData& ipd ) { if( ipd.type == ePt_INT32		) { val = ipd.val32		;	return true; } return false; }
	static bool getVal( INT64& val					, const IceParamData& ipd ) { if( ipd.type == ePt_INT64		) { val = ipd.val64		;	return true; } return false; }
	static bool getVal( PString& val				, const IceParamData& ipd ) { if( ipd.type == ePt_String	) { val = ipd.valStr	;	return true; } return false; }
	static bool getVal( const char*& val			, const IceParamData& ipd ) { if( ipd.type == ePt_String	) { val = ipd.valStr	;	return true; } return false; }
	static bool getVal( SrvTime& val				, const IceParamData& ipd ) { if( ipd.type == ePt_DateTime	) { val = ipd.valTime	;	return true; } return false; }
	static bool getVal( const CommMsgBody*& val		, const IceParamData& ipd ) { if( ipd.type == ePt_MsgBody	) { val = ipd.msgBody	;	return true; } return false; }

	static bool getFlagVal( INT8&  val, INT8&  mask , const IceParamData& ipd ) { if( ipd.type == ePt_Flags8	) { val = ipd.val8	; mask = ipd.maskVal8	; return true; } return false; }
	static bool getFlagVal( INT16& val, INT16& mask , const IceParamData& ipd ) { if( ipd.type == ePt_Flags16	) { val = ipd.val16	; mask = ipd.maskVal16	; return true; } return false; }
	static bool getFlagVal( INT32& val, INT32& mask , const IceParamData& ipd ) { if( ipd.type == ePt_Flags32	) { val = ipd.val32	; mask = ipd.maskVal32	; return true; } return false; }
	static bool getFlagVal( INT64& val, INT64& mask , const IceParamData& ipd ) { if( ipd.type == ePt_Flags64	) { val = ipd.val64	; mask = ipd.maskVal64	; return true; } return false; }
	static bool getFlagVal( PBitmask& val, PBitmask& mask, const IceParamData& ipd ) { if( ipd.type == ePt_FlagsLong)	{ val = ipd.flagsLong; mask = ipd.flagsLongMask;  return true; } return false; }

	//////////////////////////////////////////////////////////////////////////


	static 
	bool isNumeric(INT8 type_){ return type_>= ePt_INT8		&& type_<= ePt_INT64;	}

	bool isNumeric		()	const { return type >= ePt_INT8		&& type <= ePt_INT64;	}
	bool isFlags		()	const { return type >= ePt_Flags8	&& type <= ePt_Flags64;	}
	bool isFlagsLong	()	const { return type == ePt_FlagsLong;	}
	bool isDateTime		()	const { return type == ePt_DateTime;	}
	bool isString		()	const { return type == ePt_String;		}
	bool isMsg			()	const { return type == ePt_MsgBody;		}
	bool isCustomFilter	()	const { return type == ePt_xFilter;		}

	// kludge. should not be used anywhere else except IceParamVector::setUniqueForAll
	void setIsUnique( bool isUnique_ )
	{
		isUnique = isUnique_;
	}

	const ParamCustomCompareInterface* getCustomCompareInterface() const
	{
		return objCmp;
	}
	INT16 getCompareParamId() const
	{
		INT16 retVal = eIceEventParam_Unknown;
		if( isCustomFilter() )
		{
			retVal = objCmp->getCompareParamId();
		}
		return retVal;
	}
	INT8 getCompareParamType() const
	{
		INT8 retVal = ePt_Unknown;
		if( isCustomFilter() )
		{
			retVal = objCmp->getCompareType();
		}
		return retVal;
	}
	INT64 getNumericAs64( INT8& numericTypeFound ) const
	{
		numericTypeFound = ePt_Unknown;
		INT64 val = 0;
		switch( type )
		{
		case ePt_INT8:	val = val8;		numericTypeFound = type; break;
		case ePt_INT16: val = val16;	numericTypeFound = type; break;
		case ePt_INT32:	val = val32;	numericTypeFound = type; break;
		case ePt_INT64:	val = val64;	numericTypeFound = type; break;
		}
		return val;
	}
	const PString& getStringP() const // does no checking will return an empty string if not a string param
	{
		return valStr;
	}
	IceParamData( CommMsgParser& parser )  : type(ePt_Unknown), id(0), isUnique(false)
	{ 
		parse(parser); 
	}
	// special string for constructing inside ice lobby - parameter data may have special objects like objCmp implementing custom filter 
	// which is different from the regular IceParamData::compare by ePt_type
	IceParamData( INT8 type_, INT16 id_, bool isUnique_, const PString& str, const ParamCustomFactoryInterface& cfi, bool& isOk ) : type(type_), id(id_), isUnique(isUnique_)
	{
		isOk = constructFromString( str, cfi );
	}
	// not supposed to be used for anything and initialized immediately. 
	// empty constructor exists only for vector.
	IceParamData() : type(ePt_Unknown), id(0), isUnique(false)
	{
		clear();
	}
	explicit IceParamData( INT16 id_, bool isUnique_, bool  val )			: id(id_), isUnique(isUnique_),	valBool(val){ type = ePt_Bool;	}
																										  
	explicit IceParamData( INT16 id_, bool isUnique_, INT8  val )			: id(id_), isUnique(isUnique_),	val8 (val)					{ type = ePt_INT8;		}
	explicit IceParamData( INT16 id_, bool isUnique_, INT16 val )			: id(id_), isUnique(isUnique_),	val16(val)					{ type = ePt_INT16;		}
	explicit IceParamData( INT16 id_, bool isUnique_, INT32 val )			: id(id_), isUnique(isUnique_),	val32(val)					{ type = ePt_INT32;		}
	explicit IceParamData( INT16 id_, bool isUnique_, INT64 val )			: id(id_), isUnique(isUnique_),	val64(val)					{ type = ePt_INT64;		}
																										  
	explicit IceParamData( INT16 id_, bool isUnique_, INT8 val,  INT8 mask ): id(id_), isUnique(isUnique_),	val8 (val),	maskVal8 (mask)	{ type = ePt_Flags8;	}
	explicit IceParamData( INT16 id_, bool isUnique_, INT16 val, INT16 mask): id(id_), isUnique(isUnique_),	val16(val),	maskVal16(mask)	{ type = ePt_Flags16;	}
	explicit IceParamData( INT16 id_, bool isUnique_, INT32 val, INT32 mask): id(id_), isUnique(isUnique_),	val32(val),	maskVal32(mask)	{ type = ePt_Flags32;	}
	explicit IceParamData( INT16 id_, bool isUnique_, INT64 val, INT64 mask): id(id_), isUnique(isUnique_),	val64(val),	maskVal64(mask)	{ type = ePt_Flags64;	}
																										  
	explicit IceParamData( INT16 id_, bool isUnique_, const char* val )		: id(id_), isUnique(isUnique_),	valStr (val)				{ type = ePt_String;	}
	explicit IceParamData( INT16 id_, bool isUnique_, const SrvTime& val )	: id(id_), isUnique(isUnique_),	valTime(val)				{ type = ePt_DateTime;	}
	explicit IceParamData( INT16 id_, bool isUnique_, CommMsgBody* val )	: id(id_), isUnique(isUnique_), msgBody(val)				{ type = ePt_MsgBody;	}

	explicit IceParamData( INT16 id_, bool isUnique_, const PBitmask& val, const PBitmask& mask)
																			: id(id_), isUnique(isUnique_),	flagsLong(val),	flagsLongMask(mask)	{ type = ePt_FlagsLong;	}

	// need explicitly define and use operator= because of msgBody does not have a copy constructor
	IceParamData( const IceParamData& rhs ) : msgBody( NULL )
	{
		operator=(rhs);
	}
	IceParamData& operator=( const IceParamData& rhs )
	{
		if( this == &rhs )
			return *this;
		
		clear();

		type		= rhs.type;
		id			= rhs.id;
		isUnique	= rhs.isUnique;
		
		switch( type )
		{
		case ePt_Bool:		valBool	= rhs.valBool;									break;

		case ePt_INT8:		val8	= rhs.val8;										break;
		case ePt_INT16:		val16	= rhs.val16;									break;
		case ePt_INT32:		val32	= rhs.val32;									break;
		case ePt_INT64:		val64	= rhs.val64;									break;
		
		case ePt_Flags8:	val8	= rhs.val8;		maskVal8  = rhs.maskVal8;		break;
		case ePt_Flags16:	val16	= rhs.val16;	maskVal16 = rhs.maskVal16;		break;

		case ePt_Extended: // same as flags32
		case ePt_Flags32:	val32	= rhs.val32;	maskVal32 = rhs.maskVal32;		break;
		case ePt_Flags64:	val64	= rhs.val64;	maskVal64 = rhs.maskVal64;		break;

		case ePt_FlagsLong:	flagsLong = rhs.flagsLong; 
							flagsLongMask = rhs.flagsLongMask;						break;

		case ePt_DateTime:	valTime	= rhs.valTime;									break;
		case ePt_String:	valStr	= rhs.valStr;									break;
		case ePt_MsgBody:	
			{
				PAutoPtr<CommMsgBody> tmp = new CommMsgBody();
				if( rhs.msgBody )
				{
					tmp->copyFrom(*rhs.msgBody);
				}
				msgBody.swap(tmp);
				break;
			}
		case ePt_xFilter:
			{
				PAutoPtr<ParamCustomCompareInterface> tmp(  rhs.objCmp->clone() );
				objCmp.swap(tmp);
				break;
			}
		}
		return *this;
	}

	int compare( const IceParamData& rhs ) const
	{
		if( isExtended() || rhs.isExtended() )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": cannot compare extended parameters" );
			print();
			rhs.print();
			PASSERT5(0);
			return -1; // keep compiler happy. it'll break 
		}
		switch( type )
		{
		case ePt_Bool:		return (valBool ? 1 : 0) - ( rhs.valBool ? 1 : 0 );

		case ePt_INT8:		return val8  > rhs.val8		? 1 : val8  < rhs.val8  ? -1 : 0;
		case ePt_INT16:		return val16 > rhs.val16	? 1 : val16 < rhs.val16 ? -1 : 0;
		case ePt_INT32:		return val32 > rhs.val32	? 1 : val32 < rhs.val32 ? -1 : 0;
		case ePt_INT64:		return val64 > rhs.val64	? 1 : val64 < rhs.val64 ? -1 : 0;
		
		// remove bits we're not interested in and compare with the rhs.valX.
		// Anything other than == does no make sense but implement it for consistency
		case ePt_Flags8:	return ( ( val8  & rhs.maskVal8  ) ==  rhs.val8  ) ? 0 : ( val8  & rhs.maskVal8  ) < rhs.val8  ? - 1 : 1;
		case ePt_Flags16:	return ( ( val16 & rhs.maskVal16 ) ==  rhs.val16 ) ? 0 : ( val16 & rhs.maskVal16 ) < rhs.val16 ? - 1 : 1;
		case ePt_Flags32:	return ( ( val32 & rhs.maskVal32 ) ==  rhs.val32 ) ? 0 : ( val32 & rhs.maskVal32 ) < rhs.val32 ? - 1 : 1;
		case ePt_Flags64:	return ( ( val64 & rhs.maskVal64 ) ==  rhs.val64 ) ? 0 : ( val64 & rhs.maskVal64 ) < rhs.val64 ? - 1 : 1;
		
		case ePt_FlagsLong:	
		{
			PBitmask bmkTmp( flagsLong );
			bmkTmp &= rhs.flagsLongMask;
			if( bmkTmp == rhs.flagsLong )
				return 0;
			
			// cannot have a consistent even nonsensical greater or less implementation so always return greater
			// we should not be comparing flags for this anyway and if we run into it it will be a faulty logic
			return 1;
		}


		case ePt_DateTime:	return valTime.compareTo( rhs.valTime );
		case ePt_String	:	return PString::compare( valStr, rhs.valStr );
		case ePt_MsgBody:	return msgBody > rhs.msgBody ? 1 : msgBody < rhs.msgBody ? -1 : 0; // compare pointers - not a real comparison. just in case we store it in a collection
		case ePt_xFilter:
			{
				PLog( LOG_TRACE_FAULTY_LOGIC ": cannot directly invoke IceParamData::compare() for ePt_CustomFilter" );
				return 1; // not equal
			}
		}
		PLog( LOG_TRACE_FAULTY_LOGIC ": unexpected type %d", type );
		PASSERT5(0);
		return -1; // keep compiler happy. it'll break 
	}
	bool operator == ( const IceParamData& rhs ) const { return compare( rhs ) == 0;	}
	bool operator != ( const IceParamData& rhs ) const { return compare( rhs ) != 0;	}
	bool operator <  ( const IceParamData& rhs ) const { return compare( rhs ) <  0;	}
	bool operator >  ( const IceParamData& rhs ) const { return compare( rhs ) >  0;	}
	bool operator <= ( const IceParamData& rhs ) const { return compare( rhs ) <= 0;	}
	bool operator >= ( const IceParamData& rhs ) const { return compare( rhs ) >= 0;	}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body
			.composeINT8	( type	)
			.composeINT16	( id	)
			;
		return composeData( body );
	}
	CommMsgBody& composeData( CommMsgBody& body ) const
	{
		switch( type )
		{
		case ePt_Bool:		return body.composeBOOL		( valBool	);	

		case ePt_INT8:		return body.composeINT8		( val8		);	
		case ePt_INT16:		return body.composeINT16	( val16		);
		case ePt_INT32:		return body.composeINT32	( val32		);
		case ePt_INT64:		return body.composeINT64	( val64		);

		case ePt_Flags8	:	return body.composeINT8		( val8		).composeINT8 ( maskVal8  ); 
		case ePt_Flags16:	return body.composeINT16	( val16		).composeINT16( maskVal16 );
		
		case ePt_Extended: // can still compose extended
		case ePt_Flags32:	return body.composeINT32	( val32		).composeINT32( maskVal32 );
		case ePt_Flags64:	return body.composeINT64	( val64		).composeINT64( maskVal64 );
		case ePt_FlagsLong:	return body.composeObject	( flagsLong	).composeObject( flagsLongMask );

		case ePt_DateTime:	return body.composeSrvTime	( valTime	);
		case ePt_String:	return body.composeString	( valStr	);
		case ePt_MsgBody:	
			{
				if( msgBody )
				{
					return body.composeMsgBody( *msgBody	);
				}
				return body.composeMsgBody( CommMsgBody() );
			}
		case ePt_xFilter:
			{
				PLog( LOG_TRACE_FAULTY_LOGIC ": cannot compose custom filter" );
				print();
				return body;
			}
		}
		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser)
	{
		parser
			.parseINT8	( type	)
			.parseINT16	( id	)
			;
		return parseData( parser );
	}
	CommMsgParser& parseData( CommMsgParser& parser)
	{
		switch( type )
		{
		case ePt_Bool		:	return parser.parseBOOL		( valBool	);

		case ePt_INT8		:	return parser.parseINT8		( val8		);
		case ePt_INT16		:	return parser.parseINT16	( val16		);
		case ePt_INT32		:	return parser.parseINT32	( val32		);
		case ePt_INT64		:	return parser.parseINT64	( val64		);

		case ePt_Flags8		:	return parser.parseINT8		( val8		).parseINT8	( maskVal8	);
		case ePt_Flags16	:	return parser.parseINT16	( val16		).parseINT16( maskVal16 );

		case ePt_Extended: // can still parse extended
		case ePt_Flags32	:	return parser.parseINT32	( val32		).parseINT32( maskVal32 );
		case ePt_Flags64	:	return parser.parseINT64	( val64		).parseINT64( maskVal64 );
		case ePt_FlagsLong	:	return parser.parseObject	( flagsLong	).parseObject( flagsLongMask );

		case ePt_DateTime	:	return parser.parseSrvTime	( valTime	);
		case ePt_String		:	return parser.parseStringP	( valStr	);
		case ePt_MsgBody	:	
			{
				PAutoPtr<CommMsgBody> tmp = new CommMsgBody();
				parser.parseMsgBody( *tmp	);
				msgBody.swap( tmp ); // whatever was in msgBody before swap with tmp will get deallocated by the tmp
				return parser;
			}
		case ePt_xFilter	:
			{
				PLog( LOG_TRACE_FAULTY_LOGIC ": cannot parse custom filter" );
				return parser;
			}
		}
		return parser;
	}
	const char* toString( PString& str ) const
	{
		str.cut(0);
		switch( type )
		{
		case ePt_Bool		:	return str.appendInt	( valBool	);

		case ePt_INT8		:	return str.appendInt	( val8		);
		case ePt_INT16		:	return str.appendInt	( val16		);
		case ePt_INT32		:	return str.appendInt	( val32		);
		case ePt_INT64		:	return str.appendInt64	( val64		);

		case ePt_Flags8		:	return str.appendInt	(val8		).append("/").appendInt	 ( maskVal8	);
		case ePt_Flags16	:	return str.appendInt	(val16		).append("/").appendInt	 ( maskVal16	);

		case ePt_Extended: // can still convert extended
		case ePt_Flags32	:	return str.appendInt	(val32		).append("/").appendInt	 ( maskVal32	);
		case ePt_Flags64	:	return str.appendInt64	(val64		).append("/").appendInt64( maskVal64	);
		case ePt_FlagsLong	:	
		{
			PString sf, sfm;
			return str.append		( flagsLong.toDbString( sf ) ).append( "/" ).append( flagsLongMask.toDbString( sfm ) );
		}

		case ePt_String		:	return str.append( valStr );
		case ePt_DateTime	:	
			{					
				return str
					.appendInt(	valTime._year	).append("/")
					.appendInt(	valTime._month	).append("/")
					.appendInt(	valTime._day	).append("/")
					.appendInt(	valTime._hour	).append("/")
					.appendInt(	valTime._minute	).append("/")
					.appendInt(	valTime._second	).append("/")
					.appendInt( valTime._wday	);
			}
		case ePt_MsgBody	: return str.append("<body>"); // no toString for CommMsgBody
		case ePt_xFilter	:
			{
				objCmp->toString(str);
				return str;
			}
		}
		return str;
	}
	template< class T > 
	void convertFlags( const char* str, T& val, T& mask )
	{
		vector<T> v;
		StringVectorConvert::toNumVector( str, v, "/" );
		val		= v.size() > 0 ? v[0] : 0;
		mask	= v.size() > 1 ? v[1] : 0;
	}
	template<>
	void convertFlags( const char* str, PBitmask& val, PBitmask& mask )
	{
		vector<PString> v;
		StringVectorConvert::toStringVector( str, v, "/" );
		
		if( v.size() > 0 ) 
			val.fromDbString( v[0] ); 
		else 
			val = PBitmask::zero;

		if( v.size() > 1 ) 
			mask.fromDbString( v[1] ); 
		else 
			mask = PBitmask::zero;
	}
	static bool checkExtendedData( const PString& str, PString& strOut, INT32& extendedParamId, INT32& extendedParamSource )
	{
		// check $numeric_src_id.numeric_param_id
		if( str.startsWith( "$" ) )
		{
			const char* pdot = str.find( "." );
			if( pdot && ParseUtils::isNumeric( pdot + 1 ) )
			{
				PString sourceStr( str.c_str() + 1, pdot );
				if( ParseUtils::isNumeric( sourceStr ) )
				{
					extendedParamSource		= atoi( sourceStr );
					extendedParamId			= atoi( pdot + 1 );
					strOut					= str;
					return true;
				}
			}
		}
		return false;
	};
private:
	bool initFromExtendedDataAndChangeType( const PString& str )
	{
		if( isCustomFilter() )
			return false;

		if( checkExtendedData( str, valStr, val32, maskVal32 ) )
		{
			type = ePt_Extended;
			return true;
		}
		return false;
	}
	bool constructFromString( const PString& str, const ParamCustomFactoryInterface& cfi )
	{
		bool ok = true;

		if( !isCustomFilter() && initFromExtendedDataAndChangeType( str ) )
		{
			return ok;
		}
		if( isExtended() )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": cannot construct extended parameter this way" );
			PASSERT5(0);
			return false;
		}

		switch( type )
		{
		case ePt_Bool		:	valBool	= atoi(str) != 0;				break;

		case ePt_INT8		:	val8	= atoi(str);					break;
		case ePt_INT16		:	val16	= atoi(str);					break;
		case ePt_INT32		:	val32	= atoi(str);					break;
		case ePt_INT64		:	val64	= p_atoi64(str);				break;
		
		case ePt_Flags8		:	convertFlags( str, val8	, maskVal8	);	break;
		case ePt_Flags16	:	convertFlags( str, val16, maskVal16 );	break;

		case ePt_Flags32	:	convertFlags( str, val32, maskVal32 );	break;
		case ePt_Flags64	:	convertFlags( str, val64, maskVal64 );	break;
		
		case ePt_FlagsLong	:	convertFlags( str, flagsLong, flagsLongMask);	
																		break;

		case ePt_String		:	valStr	= str;							break;
		case ePt_DateTime	:	
			{
				vector<INT32> v;
				StringVectorConvert::toNumVector( str, v, "/" );
				if( v.size() == 7  )
				{ 
					valTime._year	= v[0];
					valTime._month	= v[1];
					valTime._day	= v[2];
					valTime._hour	= v[3];
					valTime._minute	= v[4];
					valTime._second	= v[5];
					valTime._wday	= v[6];
				}
				else
				{
					PLog( LOG_TRACE_FAULTY_LOGIC ": could not parse DateTime '%s'", str.c_str() );
					valTime.setNull();
				}
				break;
			}
		case ePt_MsgBody	:	
			{
				// currently no fromString for CommMsgBody. Creates an empty msgBody
				PAutoPtr<CommMsgBody> tmp = new CommMsgBody();
				msgBody.swap(tmp);
				break;
			}
		case ePt_xFilter	:
			{
				PAutoPtr<ParamCustomCompareInterface> tmp( cfi.createCustomCompareFilter( id, str ) );
				objCmp.swap(tmp);
				if( objCmp != NULL )
				{
					ok = objCmp->isInitSucceded();
				}
				else
				{
					PLog( "could not create custom filter" );
					ok = false;
				}
				break;
			}
		}
		if( !ok )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": could not initialize parameter:" );
			print();
		}
		return ok;
	}
public:

	void print() const
	{
#if PTRACELVL >= 3
		PString s;
		if( toString( s ) )
		{
			PLog("ed t %d id %d '%s' %c", type, id, s.c_str(), isUnique ? 'u' : 'm' );
		}
		else
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": bad evt: t %d id %d ", type, id);
		}
		
#endif
	}
	void trace5( const char* prefix ) const
	{
#if PTRACELVL >= 5
		PString s;
		if( toString( s ) )
		{
			PLog("%s ed t %d id %d '%s' %c", prefix, type, id, s.c_str(), isUnique ? 'u' : 'm' );
		}
		else
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": bad evt: t %d id %d ", type, id);
		}

#endif
	}

	INT16 getId()					const { return id; }
	eIceParamTypes getType()		const { return (eIceParamTypes)type; }
	bool getIsUnique()				const { return isUnique; }

	inline bool isExtended()				const { return type == ePt_Extended; }
	inline INT32 getExtendedParamSource()	const { return isExtended() ? maskVal32	: 0; }
	inline INT32 getExtendedParamId()		const { return isExtended() ? val32		: 0; }


	bool isEqualType( const IceParamData& rhs, const INT64 overrideRhsParamId )					const 
	{ 
		if( overrideRhsParamId )
			return type == rhs.type && id == overrideRhsParamId; 

		return type == rhs.type && id == rhs.id; 
	}
	bool isEqualType( const INT8 rhsType, const INT16 rhsId )	const 
	{ 
		return type == rhsType	&& id == rhsId;  
	}

};

//////////////////////////////////////////////////////////////////////////

class IceParamVector
{
protected:
	vector<IceParamData> params;

public:
	virtual ~IceParamVector() {}

	CommMsgParser& parse( CommMsgParser& parser )
	{
		parseVectorOfParsableValues(params, parser);
		return parser;
	}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		composeVectorOfComposableValues(params, body);
		return body;
	}
	const IceParamData* findParam( INT16 id, INT16 idx = 0 ) const
	{
		INT16 curIdx = 0;
		for( int i = 0; i < params.size(); ++i )
		{
			if( params[i].getId() == id )
			{
				if( idx == curIdx )
				{
					return &params[i];
				}
				++curIdx;
			}
		}
		return NULL;
	}
	IceParamData* findParam( INT16 id, INT16 idx = 0 )
	{
		INT16 curIdx = 0;
		for( int i = 0; i < params.size(); ++i )
		{
			if( params[i].getId() == id )
			{
				if( idx == curIdx )
				{
					return &params[i];
				}
				++curIdx;
			}
		}
		return NULL;
	}
	// kludge until I figure out a better way to handle it
	// in general I don't need to always compose isUnique with every event
	// but sometimes I really need it, so for now I just have a way of setting it
	// especially in cases where I need it it's always the same
	//
	void setUniqueForAll( const bool isUnique )
	{
		for( int i = 0; i < params.size(); ++i )
		{
			params[i].setIsUnique( isUnique );
		}
	}
	template<class T>
	bool getParamValue( const INT16 paramId, T& val, INT16 idx = 0 ) const
	{
		const IceParamData* pd = findParam( paramId, idx );
		if( pd )
		{
			return pd->getVal(val);
		}
		return false;
	}
	template<class T_new, class T_old>
	bool getParamValueCompat( const INT16 paramId, T_new& val, INT16 idx = 0 ) const
	{
		const IceParamData* pd = findParam( paramId, idx );
		if( pd )
		{
			if( pd->getVal(val) )
			{
				return true;
			}
			// new type incompatible - try old type
			T_old valOld;
			if( pd->getVal( valOld ) )
			{
				// found old type - cast into new
				val = (T_new)valOld;
				return true;
			}
		}
		return false;
	}
	template<class T>
	bool getParamFlagValues( const INT16 paramId, INT16 idx, T& val ) const
	{
		const IceParamData* pd = findParam( paramId, idx );
		if( !pd )
			return false;
		if( !( pd->isFlags() ) )
			return false;
		
		T mask;
		return pd->getFlagVal(val, mask);
	}
	INT64 getNumericParamValueAs64( const INT16 paramId, INT16 idx, INT8& numericTypeFound ) const
	{
		const IceParamData* pd = findParam( paramId, idx );
		if( pd )
		{
			return pd->getNumericAs64( numericTypeFound );
		}
		return 0;
	}
	bool addParam( const IceParamData& param, const bool writeOver = true )
	{
		IceParamData* existing = findParam( param.getId(), 0 /*idx*/ ); // write over will overwrite the first parameter with idx 0
		if( existing )
		{
			if( param.getIsUnique() )
			{
				if( writeOver )
				{
					*existing = param;
					return true;
				}
				return false;
			}
		}
		PTRACE5( "pne+ %d %c", param.getId(), param.getIsUnique() ? 'u' : 'm');
		params.push_back( param );
		return true;
	}
	INT32 mergeWith( const vector<IceParamData>& paramsFrom )
	{
		PTRACE5( "mrg+ %d", (INT32)paramsFrom.size() );

		INT32 numAdded = 0;
		for( int i = 0; i < paramsFrom.size(); ++i )
		{
			if( addParam( paramsFrom[i], false /*writeOver*/ ) )
			{
				++numAdded;
			}
		}
		return numAdded;
	}
	void print() const
	{
		PLog("pms:");
		InvLogging::printSimplePrintableColl( params.begin(), params.end() );
	}
};


//////////////////////////////////////////////////////////////////////////
// when admin sends a parameter - check if the data is correct

class DummyArrayIdValidatorInterface
{
public:
	virtual bool isSupportsArray( INT16 paramId ) const 
	{ 
		return false; 
	}
	virtual ~DummyArrayIdValidatorInterface(){}
};

class IceParamsTypesValidator : public DummyArrayIdValidatorInterface
{
	set<INT32> autoEnrolPermittedEventTypes;

	class FlagDescr
	{
	public:
		INT64	flagVal;
		PString flagName;

		FlagDescr() : flagVal( 0 )
		{
		}
		FlagDescr( INT64 flagVal_, const char* flagName_ ) : flagVal( flagVal_ ), flagName( flagName_ )
		{
		}
		CommMsgBody& compose( CommMsgBody& body ) const
		{
			return
				body.composeINT64	( flagVal )
					.composeString	( flagName)
					;
		}
		CommMsgParser& parse( CommMsgParser& parser )
		{
			return
				parser.parseINT64	( flagVal )
					.parseStringP	( flagName)
					;
		}
	};
	class FlagLongDescr
	{
	public:
		PBitmask	flagVal;
		PString		flagName;

		FlagLongDescr() : flagVal( PBitmask::zero )
		{
		}
		FlagLongDescr( const PBitmask& flagVal_, const char* flagName_ ) : flagVal( flagVal_ ), flagName( flagName_ )
		{
		}
		CommMsgBody& compose( CommMsgBody& body ) const
		{
			return
				body.composeObject	( flagVal )
				.composeString		( flagName )
				;
		}
		CommMsgParser& parse( CommMsgParser& parser )
		{
			return
				parser.parseObject	( flagVal )
				.parseStringP		( flagName )
				;
		}
	};
public:
	class PDim : public vector<INT8>
	{
	public:
		
		PDim(){}
		PDim( INT8 d1					) { push_back( d1 ); }
		PDim( INT8 d1, INT8 d2			) { push_back( d1 ); push_back( d2 ); }
		PDim( INT8 d1, INT8 d2, INT8 d3 ) { push_back( d1 ); push_back( d2 ); push_back( d3 );}
	};
	class ParamType
	{
	public:
		INT16	id;
		INT8	type;
		PString name;
		vector<FlagDescr>		flags;
		vector<FlagLongDescr>	flagsLong;
		PDim	dimensions;

		ParamType( INT16 id_, INT8 type_, const char* name_, const PDim& dimensions_ = PDim() ) 
			: id(id_), type(type_), name(name_), dimensions( dimensions_ )
		{
		}
		ParamType( INT16 id_, INT8 type_, const char* name_, const vector<FlagDescr>& flags_, const PDim& dimensions_ = PDim() ) 
			: id(id_), type(type_), name(name_), flags(flags_), dimensions( dimensions_ )
		{
		}
		ParamType( INT16 id_, INT8 type_, const char* name_, const vector<FlagLongDescr>& flags_, const PDim& dimensions_ = PDim() )
			: id( id_ ), type( type_ ), name( name_ ), flagsLong( flags_ ), dimensions( dimensions_ )
		{
		}
		CommMsgBody& compose( CommMsgBody& body ) const
		{
			body.composeINT16	( id )
				.composeINT8	(type)
				.composeString	(name)
				;
			composeVectorOfComposableValues( flags		, body );
			composeVectorOfComposableValues( flagsLong	, body );

			return body;
		}
		CommMsgParser& parse( CommMsgParser& parser )
		{
			parser.parseINT16	( id )
				.parseINT8		(type)
				.parseStringP	(name)
				;
			parseVectorOfParsableValues( flags		, parser );
			parseVectorOfParsableValues( flagsLong	, parser );

			return parser;
		}
		bool isArray() const
		{
			return dimensions.size() > 0;
		}
		// empty idx universally converted to linear index 0
		INT16 getLinearIndex( const vector<INT8>& idx, PString& errDescr ) const
		{
			if( !isArray() )
			{
				errDescr
					.assign( "param ").appendInt( id )
					.append( " " ).append( name )
					.append( " is not an array" )
					;
				return -1;
			}
			if( idx.size() != dimensions.size() )
			{
				errDescr
					.assign( "param ").appendInt( id )
					.append( " " ).append( name )
					.append( " has " ).appendInt( dimensions.size() )
					.append( " [] dimensions and not " ).appendInt( idx.size() );
				return -1;
			}

			INT16 linearIndex = 0;
			vector<INT16> prevDimPlaneSize( idx.size() );

			for( INT8 i = 0; i < idx.size() ; ++i )
			{
				if( idx[i] < 0 )
				{
					// internally we work with 0 based indexes but the script works with 1 based indexes so 
					// in order to communicate it to the user we must use + 1 for diagnostic output
					//
					errDescr
						.assign( "param ").appendInt( id ).append( " " ).append( name )
						.append( " index for [] dimension " ).appendInt( i + 1 ).append( " is " ).appendInt( idx[i] + 1 ).append( " < 1 " );
					return -1;
				}
				if( dimensions[i] == length_is_variable && i != idx.size() - 1 )
				{
					errDescr
						.assign( "parser implementation error: param ").appendInt( id ).append( " " ).append( name )
						.append( " only last dimension can be variable" );	
					return -1;
				}
				if( dimensions[i] != 0 && idx[i] >= dimensions[i] && dimensions[i] != length_is_variable )
				{
					// internally we work with 0 based indexes but the script works with 1 based indexes so 
					// in order to communicate it to the user we must use + 1 for diagnostic output
					//
					errDescr
						.assign( "param ").appendInt( id ).append( " " ).append( name )
						.append( " index for [] dimension " ).appendInt( i + 1 ).append( " is " ).appendInt( idx[i] + 1 ).append( " > " ).appendInt( dimensions[i] );
					return -1;
				}
				if( i > 0 )
				{
					// i = 0 - vector, 1 - matrix, 2 - 3d array so each dimension size would be
					// the size of the previous dimension * limit of the current dimension
					//
					prevDimPlaneSize[i] = prevDimPlaneSize[i-1] * dimensions[i-1];
				}
				else
				{
					// previous dimension size is 0 - no an array
					prevDimPlaneSize[i] = 1;
				}
				linearIndex += prevDimPlaneSize[i] * idx[i];
			}
			return linearIndex;
		}
	};
private:
	
	static const INT8 length_is_variable = 0;

	
	typedef map< INT16 /*id*/, ParamType* > IceParamsTypesMapType_;
	typedef PtrMapOwner< IceParamsTypesMapType_ > IceParamsTypesMap_;
	IceParamsTypesMap_ paramMap;

public:
	IceParamsTypesValidator()
	{
#define ADD_ICE_PARAM( id_, type_, name_ ) paramMap.updateValue( id_, new ParamType( id_, type_, name_ ) )
#define ADD_ICE_PARAM_FLAGS( id_, type_, name_, flags_ ) paramMap.updateValue( id_, new ParamType( id_, type_, name_, flags_ ) )
#define ADD_ICE_PARAM_DIM( id_, type_, name_, dim_ ) paramMap.updateValue( id_, new ParamType( id_, type_, name_, dim_ ) )
#define ADD_ICE_PARAM_FLAGS_DIM( id_, type_, name_, flags_, dim_ ) paramMap.updateValue( id_, new ParamType( id_, type_, name_, flags_, dim_ ) )

		//////////////////////////////////////////////////////////////////////////
		// all flag params
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_GameFlags8_Bits_IsPlayMoney	,"Play Money") );
			descr.push_back( FlagDescr( eIceEventParam_GameFlags8_Bits_IsZoom		,"Zoom"		) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_GameFlags8			,ePt_Flags8		,"Game Flags", descr	);
		}
		{
			// PYR-84639: To be deprecated
 			vector<FlagDescr> descr;
 			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags8_Bits_SmallBlind	,"SmallBlind") );
 			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags8_Bits_BigBlind		,"BigBlind"	 ) );
 			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags8_Bits_Button		,"Button"	 ) );
 			ADD_ICE_PARAM_FLAGS( eIceEventParam_SeatPositionFlags8_Deprecated,ePt_Flags8	,"SeatPositionFlags8", descr	);
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_SmallBlind		,"SmallBlind") );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_BigBlind		,"BigBlind"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button			,"Button"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_1	,"Button-1"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_2	,"Button-2"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_3	,"Button-3"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_4	,"Button-4"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_5	,"Button-5"	 ) );
			descr.push_back( FlagDescr( eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_6	,"Button-6"	 ) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SeatPositionFlags16		,ePt_Flags16		,"SeatPositionFlags16", descr	);
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_RingHandFlags8_Bits_RakeTaken	,"Rake Taken"	) );
			descr.push_back( FlagDescr( eIceEventParam_RingHandFlags8_Bits_Cashout		,"Cashout"		) );
			descr.push_back( FlagDescr( eIceEventParam_RingHandFlags8_Bits_SeatFinder	,"SeatFinder"	) );
			descr.push_back( FlagDescr( eIceEventParam_RingHandFlags8_Bits_HiLo			,"HiLo"			) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_RingHandFlags8		,ePt_Flags8		,"RingHandFlags8"	, descr	);
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventTournResultFlags_Win				,	"Win flag") );
			descr.push_back( FlagDescr( eIceEventTournResultFlags_FinalTable		,	"Final Table") ); // PYR-33583

			ADD_ICE_PARAM_FLAGS( eIceEventParam_Tourn_ResultFlags8	,ePt_Flags8		,"Tourn_ResultFlags8", descr	);
		}
		{
			// PYR-33583
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventTournResultFlags_Cash				,	"Cash"					) );
			descr.push_back( FlagDescr( eIceEventTournResultFlags_TCash				,	"TCash"					) );
			descr.push_back( FlagDescr( eIceEventTournResultFlags_FPP				,	"FPP"					) );
			descr.push_back( FlagDescr( eIceEventTournResultFlags_TournTicket		,	"TournTicket"			) );
			descr.push_back( FlagDescr( eIceEventTournResultFlags_TournRegistration	,	"TournRegistration"		) );
			descr.push_back( FlagDescr( eIceEventTournResultFlags_None				,	"None"					) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_Tourn_WinType	,ePt_Flags8		,"Tourn_ResultFlags8", descr	);
		}

		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_PrivilegeFlags32_Bits_BoardParticipateOff,	"LeaderBoardDefaultOff") );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_Tourn_ResultFlags8	,ePt_Flags8		,"PrivilegeFlags32", descr	);
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_SNG			, "SNG"							) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_SAG			, "SAG"							) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Timed			, "Timed"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Reentry		, "Reentry"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Flighted		, "Flighted"					) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Rebuy			, "Rebuy"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Addon			, "Addon"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Bounty		, "Knockout"					) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Shootout		, "Shootout"					) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_HiLo			, "HiLo"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Powerup		, "Powerup"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Satellite		, "Satellite"					) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_WinButton		, "WinButton"					) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_NStack		, "NStack"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_Fifty50		, "Fifty50"						) );
			descr.push_back( FlagDescr( eIceEventParam_Tourn_Flags16_Bits_ProgressPlus	, "ProgressPlus"				) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_Tourn_Flags16		,ePt_Flags16	, "Tourn Flags", descr			);
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_PrivilegeFlags32_Bits_BoardParticipateOff, "LeaderBoardDefaultOff"	) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_PrivilegeFlags32	,ePt_Flags32				,"PrivilegeFlags", descr	);
		}
		// 
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_VppsFlags8_FppBundleUser			, "FppBundleUser"			) );
			descr.push_back( FlagDescr( eIceEventParam_VppsFlags8_FppBundleAdminUpdate	, "FppBundleAdminUpdate"	) );
			descr.push_back( FlagDescr( eIceEventParam_VppsFlags8_FppBundleVppAwarded	, "FppBundleVppAwarded"		) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_VppsFlags8	,ePt_Flags8					, "VppFlags", descr			);
		}
		{
			vector<FlagDescr> descr;
			// add flags when they get defined
			ADD_ICE_PARAM_FLAGS( eIceEventParam_CppsFlags8	,ePt_Flags8					, "CppFlags", descr			);
		}
		{
			vector<FlagDescr> descr;
			// add flags when they get defined
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SppsFlags8	,ePt_Flags8					, "SppFlags", descr			);
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_DuelPlayerFlags8_Bits_First		, "First"					) );
			descr.push_back( FlagDescr( eIceEventParam_DuelPlayerFlags8_Bits_Friend		, "Friend"					) );
			descr.push_back( FlagDescr( eIceEventParam_DuelPlayerFlags8_Bits_Challenge	, "Challenge"				) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_DuelPlayerFlags8	,ePt_Flags8			, "DuelPlayer", descr		);
		}
		
		// PYR-34347
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_BlackjackFlags8_Bits_PlayerBlackjack		, "PlayerBlackjack"		) );
			descr.push_back( FlagDescr( eIceEventParam_BlackjackFlags8_Bits_DealerBlackjack		, "DealerBlackjack"		) );
			descr.push_back( FlagDescr( eIceEventParam_BlackjackFlags8_Bits_Insurance			, "Insurance"			) );
			descr.push_back( FlagDescr( eIceEventParam_BlackjackFlags8_Bits_Surrender			, "Surrender"			) );
			descr.push_back( FlagDescr( eIceEventParam_BlackjackFlags8_Bits_InsuranceOffered	, "InsuranceOffered"	) );
			descr.push_back( FlagDescr( eIceEventParam_BlackjackFlags8_Bits_SurrenderOffered	, "SurrenderOffered"	) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BlackjackFlags8	,ePt_Flags8		, "BlackjackFlags8", descr	);
		}
		
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_SportBetResultFlag8_Win			, "Win"						) );
			descr.push_back( FlagDescr( eIceEventParam_SportBetResultFlag8_Loss			, "Loss"					) );
			descr.push_back( FlagDescr( eIceEventParam_SportBetResultFlag8_Push			, "Push"					) );

			// PYR-37407
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SportsBetResultFlags8	,ePt_Flags8		, "SportsBetResult"			, descr	);

			// PYR-34347
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BlackjackResult			,ePt_Flags8		, "BlackjackResult"			, descr	);

			// PYR-43854
			ADD_ICE_PARAM_FLAGS( eIceEventParam_RouletteBetResult		,ePt_Flags8		, "RouletteBetResult"		, descr	);

			// PYR-59835
			ADD_ICE_PARAM_FLAGS( eIceEventParam_DreamCatcherBetResult	,ePt_Flags8		, "DreamCatcherBetResult"	, descr	);

			// PYR-67812
			ADD_ICE_PARAM_FLAGS( eIceEventParam_TexasHoldemBonusResult	,ePt_Flags8		, "TexasHoldemBonusResult"	, descr );

			// PYR-66738
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BaccaratBetResult		,ePt_Flags8		, "BaccaratBetResult"		, descr );

			// PYR-66224
			ADD_ICE_PARAM_FLAGS( eIceEventParam_DragonTigerBetResult	,ePt_Flags8		, "DragonTigerBetResult"	, descr );
		}
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_SportBetFlag8_BetType_Single		, "BetType_Single"			) );
			descr.push_back( FlagDescr( eIceEventParam_SportBetFlag8_BetType_Multiple	, "BetType_Multiple"		) );
			descr.push_back( FlagDescr( eIceEventParam_SportBetFlag8_BetType_System		, "BetType_System"			) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SportsBetFlags8	,ePt_Flags8				, "SportsBetTypeFlags8", descr	);
		}

		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_SportBetFlag8_Group_BuildABet	, "BuildABet"				) );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SportsBetFlags8Group	,ePt_Flags8		, "SportsBetGlags8Group", descr	);
		}

		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_AmericanFootball	, "AmericanFootball") );		
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Archery			, "Archery"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_AussieRules		, "AussieRules"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Badminton		, "Badminton"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Baseball			, "Baseball"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Basketball		, "Basketball"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_BeachSoccer		, "BeachSoccer"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_BeachVolley		, "BeachVolley"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Boxing			, "Boxing"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Bowling			, "Bowling"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Canoeing			, "Canoeing"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Chess			, "Chess"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Cricket			, "Cricket"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Curling			, "Curling"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Cycling			, "Cycling"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Darts			, "Darts"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Esports			, "Esports"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_F1				, "F1"				) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Fencing			, "Fencing"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_FieldHockey		, "FieldHockey"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_FigureSkating	, "FigureSkating"	) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Floorball		, "Floorball"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Football			, "Football"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Futsal			, "Futsal"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_GaelicSports		, "GaelicSports"	) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Golf				, "Golf"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Gymnastics		, "Gymnastics"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Handball			, "Handball"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_HorseRacing		, "HorseRacing"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_IceHockey		, "IceHockey"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Judo				, "Judo"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Lacrosse			, "Lacrosse"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_MotorBikes		, "MotorBikes"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Netball			, "Netball"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Pesapallo		, "Pesapallo"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Poker			, "Poker"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Pool				, "Pool"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Rowing			, "Rowing"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Rugby			, "Rugby"			) );					
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_RugbyLeague		, "RugbyLeague"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_RugbyUnion		, "RugbyUnion"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Sailing			, "Sailing"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Snooker			, "Snooker"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Softball			, "Softball"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Squash			, "Squash"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Swimming			, "Swimming"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_TableTennis		, "TableTennis"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Taekwando		, "Taekwando"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Tennis			, "Tennis"			) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Volleyball		, "Volleyball"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Waterpolo		, "Waterpolo"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Weightlifting	, "Weightlifting"	) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Wrestling		, "Wrestling"		) );	
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Bowls			, "Bowls"			) );
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_MultiSport		, "MultiSport"		) );			
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved1		, "Reserved1"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved2		, "Reserved2"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved3		, "Reserved3"		) );
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved4		, "Reserved4"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved5		, "Reserved5"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved6		, "Reserved6"		) );
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved7		, "Reserved7"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved8		, "Reserved8"		) );				
			descr.push_back( FlagDescr( eIceEventParam_SportFlag64_Reserved9		, "Reserved9"		) );	
			
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SportsKindFlag64	,ePt_Flags64	, "SportsKind", descr	);
		}

		{
			SportsFlagsLong sfl;
			vector<FlagLongDescr> descr;
			
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_AmericanFootball	, "AmericanFootball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Archery			, "Archery" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_AussieRules		, "AussieRules" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Badminton			, "Badminton" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Baseball			, "Baseball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Basketball			, "Basketball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_BeachSoccer		, "BeachSoccer" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_BeachVolley		, "BeachVolley" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Boxing				, "Boxing" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Bowling			, "Bowling" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Canoeing			, "Canoeing" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Chess				, "Chess" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Cricket			, "Cricket" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Curling			, "Curling" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Cycling			, "Cycling" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Darts				, "Darts" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Esports			, "Esports" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_F1					, "F1" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Fencing			, "Fencing" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_FieldHockey		, "FieldHockey" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_FigureSkating		, "FigureSkating" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Floorball			, "Floorball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Football			, "Football" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Futsal				, "Futsal" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_GaelicSports		, "GaelicSports" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Golf				, "Golf" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Gymnastics			, "Gymnastics" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Handball			, "Handball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_HorseRacing		, "HorseRacing" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_IceHockey			, "IceHockey" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Judo				, "Judo" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Lacrosse			, "Lacrosse" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_MotorBikes			, "MotorBikes" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Netball			, "Netball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Pesapallo			, "Pesapallo" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Poker				, "Poker" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Pool				, "Pool" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Rowing				, "Rowing" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Rugby				, "Rugby" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_RugbyLeague		, "RugbyLeague" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_RugbyUnion			, "RugbyUnion" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Sailing			, "Sailing" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Snooker			, "Snooker" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Softball			, "Softball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Squash				, "Squash" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Swimming			, "Swimming" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_TableTennis		, "TableTennis" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Taekwando			, "Taekwando" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Tennis				, "Tennis" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Volleyball			, "Volleyball" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Waterpolo			, "Waterpolo" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Weightlifting		, "Weightlifting" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Wrestling			, "Wrestling" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Bowls				, "Bowls" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_MultiSport			, "MultiSport" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved1			, "Reserved1" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved2			, "Reserved2" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved3			, "Reserved3" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved4			, "Reserved4" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved5			, "Reserved5" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved6			, "Reserved6" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved7			, "Reserved7" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved8			, "Reserved8" ) );
			descr.push_back( FlagLongDescr( sfl.eIceEventParam_SportFlagLong_Reserved9			, "Reserved9" ) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_SportsKindFlagLong, ePt_FlagsLong, "SportsKindLong", descr );
		}

		// PYR-75461
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Basketball		, "VirtualBasketball"	) );			
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Boxing			, "VirtualBoxing"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Cricket			, "VirtualCricket"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Cycling			, "VirtualCycling"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Darts				, "VirtualDarts"		) );					
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Dogs				, "VirtualDogs"			) );					
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_F1				, "VirtualF1"			) );					
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Football			, "VirtualFootball"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Horses			, "VirtualHorses"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_MatchDay			, "VirtualMatchDay"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Numbers			, "VirtualNumbers"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_RushFootball		, "VirtualRushFootball"	) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Speedway			, "VirtualSpeedway"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Tennis			, "VirtualTennis"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_Trotting			, "VirtualTrotting"		) );				
			descr.push_back( FlagDescr( eIceEventParam_VirtualSportFlag64_MultiSport		, "MultiVirtualSport"	) );	
			
			ADD_ICE_PARAM_FLAGS( eIceEventParam_SportsVirtualKindFlag64	,ePt_Flags64		, "VirtualSport", descr	);
		}

		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_10h	, "10h"	) );	
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_9h	, "9h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_8h	, "8h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_7h	, "7h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_6h	, "6h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_5h	, "5h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_4h	, "4h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_3h	, "3h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_2h	, "2h"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Ah	, "Ah"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Kh	, "Kh"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Qh	, "Qh"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Jh	, "Jh"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_10c	, "10c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_9c	, "9c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_8c	, "8c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_7c	, "7c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_6c	, "6c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_5c	, "5c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_4c	, "4c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_3c	, "3c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_2c	, "2c"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Ac	, "Ac"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Kc	, "Kc"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Qc	, "Qc"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Jc	, "Jc"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_10s	, "10s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_9s	, "9s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_8s	, "8s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_7s	, "7s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_6s	, "6s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_5s	, "5s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_4s	, "4s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_3s	, "3s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_2s	, "2s"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_As	, "As"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Ks	, "Ks"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Qs	, "Qs"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Js	, "Js"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_10d	, "10d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_9d	, "9d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_8d	, "8d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_7d	, "7d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_6d	, "6d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_5d	, "5d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_4d	, "4d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_3d	, "3d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_2d	, "2d"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Ad	, "Ad"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Kd	, "Kd"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Qd	, "Qd"	) );
			descr.push_back( FlagDescr( eIceEventParam_Card_Bits64_Jd	, "Jd"	) );

			// PYR-42615
			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandHoleCards			,ePt_Flags64	, "HandHoleCards"			, descr	);
			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandBoardCards			,ePt_Flags64	, "RingBoardCards"			, descr	);
			// PYR-39372
			ADD_ICE_PARAM_FLAGS( eIceEventParam_RingHoleAndFlopCards	,ePt_Flags64	, "RingHoleAndFlopCards"	, descr	);
			
			// PYR-34347
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BlackjackPlayerCards	,ePt_Flags64	, "BlackjackPlayerCards"	, descr	);  
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BlackjackDealerCards	,ePt_Flags64	, "BlackjackDealerCards"	, descr	);
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BlackjackFirstTwoCards	,ePt_Flags64	, "BlackjackFirstTwoCards"	, descr	);  

			// PYR-67812
			ADD_ICE_PARAM_FLAGS( eIceEventParam_TexasHoldemBonusPlayerCards, ePt_Flags64, "TexasHoldemBonusPlayerCards", descr );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_TexasHoldemBonusDealerCards, ePt_Flags64, "TexasHoldemBonusDealerCards", descr );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_TexasHoldemBonusBoardCards , ePt_Flags64, "TexasHoldemBonusBoardCards" , descr );

			// PYR-66738
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BaccaratPlayerCard		, ePt_Flags64	, "BaccaratPlayerCard"		, descr );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_BaccaratBankerCard		, ePt_Flags64	, "BaccaratBankerCard"		, descr );

			// PYR-66224
			ADD_ICE_PARAM_FLAGS( eIceEventParam_DragonCard				, ePt_Flags64	, "DragonCard"				, descr );
			ADD_ICE_PARAM_FLAGS( eIceEventParam_TigerCard				, ePt_Flags64	, "TigerCard"				, descr );
		}

		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_StraightBet		, "StraightBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_SplitBet			, "SplitBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_CornerBet		, "CornerBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_StreetBet		, "StreetBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_LineBet			, "LineBet"				) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_RedBlackBet		, "RedBlackBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_OddEvenBet		, "OddEvenBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_HighLowBet		, "HighLowBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_DozensBet		, "DozensBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_ColumnsBet		, "ColumnsBet"			) );
			descr.push_back( FlagDescr( eIceEventParam_RouletteBetTypeFlags16_Bits_FiveBet			, "FiveBet"				) );
			
			ADD_ICE_PARAM_FLAGS( eIceEventParam_RouletteTypeBetsPlaced	,ePt_Flags16	, "RouletteTypeBetsPlaced"	, descr	);
			ADD_ICE_PARAM_FLAGS( eIceEventParam_RouletteBetType			,ePt_Flags16	, "RouletteBetType"			, descr	);
		}

		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_EmailValidated		, "EmailValidated"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_RealMoneyOK		, "RealMoneyOK"			) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_Authenticated		, "Authenticated"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_AgeVerified		, "AgeVerified"			) );

			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_RMPokerPlayer		, "RMPokerPlayer"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_RMCasinoPlayer		, "RMCasinoPlayer"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_RMSportsPlayer		, "RMSportsPlayer"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_StarsRewardsOptIn	, "StarsRewardsOptedIn"	) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_FraudulentAccount	, "FraudulentAccount"	) );

			// PYR-74170
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_DoNotPayBonus		, "DoNotPayBonus"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_NoSportsBonus		, "NoSportsBonus"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_NoCasinoPlay		, "NoCasinoPlay"		) );
			descr.push_back( FlagDescr( eIceEventParam_AccountValidationTypeFlags16_Bits_NoSportsPlay		, "NoSportsPlay"		) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_AccountValidationType	,ePt_Flags16	, "ValidationType"	, descr	);
		}

		// PYR-47136
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceDepositFlags8_IgnoreBySignals				, "IgnoredBySignals"			) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_RmDepositFlags8	,ePt_Flags8				, "DepositFlags", descr			);
		}
		// PYR-49457
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerReload-1 )		, "Reload"			) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerEMP-1 )			, "EMP"				) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerDisintegrate-1 )	, "Disintegrate"	) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerXRay-1 )			, "Xray"			) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerEngineer-1 )		, "Engineer"		) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerScanner-1 )		, "Scanner"			) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerIntel-1 )			, "Intel"			) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerUpgrade-1 )		, "Upgrade"			) );
			descr.push_back( FlagDescr( (INT64)1<<( PokerGamePowerClone-1 )			, "Clone"			) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandPowerPlayedFlags64	,ePt_Flags64		, "HandPowerPlayed",	descr	);
		}

		// PYR-59835
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_DreamCatcherNumber_Flags8_Bits_1		, "1"	) );
			descr.push_back( FlagDescr( eIceEventParam_DreamCatcherNumber_Flags8_Bits_2		, "2"	) );
			descr.push_back( FlagDescr( eIceEventParam_DreamCatcherNumber_Flags8_Bits_5		, "5"	) );
			descr.push_back( FlagDescr( eIceEventParam_DreamCatcherNumber_Flags8_Bits_10	, "10"	) );
			descr.push_back( FlagDescr( eIceEventParam_DreamCatcherNumber_Flags8_Bits_20	, "20"	) );
			descr.push_back( FlagDescr( eIceEventParam_DreamCatcherNumber_Flags8_Bits_40	, "40"	) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_DreamCatcherBetNumber	,ePt_Flags8	, "DreamCatcherBetNumber"	, descr	);
		}

		// PYR-67812
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Ante		, "Ante"	) );
			descr.push_back( FlagDescr( eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Bonus		, "Bonus"	) );
			descr.push_back( FlagDescr( eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Flop		, "Flop"	) );
			descr.push_back( FlagDescr( eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Turn		, "Turn"	) );
			descr.push_back( FlagDescr( eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_River		, "River"	) );
			descr.push_back( FlagDescr( eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Jackpot	, "Jackpot"	) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_TexasHoldemBonusBetType	,ePt_Flags8	, "TexasHoldemBonusBetType"	, descr	);
		}

		// PYR-66738
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_Player		, "Player"		) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_Banker		, "Banker"		) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_Tie			, "Tie"			) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_BankerPair	, "BankerPair"	) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_PlayerPair	, "PlayerPair"	) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_PerfectPair	, "PerfectPair"	) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_EitherPair	, "EitherPair"	) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_PlayerBonus	, "PlayerBonus" ) );
			descr.push_back( FlagDescr( eIceEventParam_BaccaratBetTypeFlags16_Bits_BankerBonus	, "BankerBonus" ) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_BaccaratBetType	,ePt_Flags16	, "BaccaratBetType"	, descr	);
		}

		// PYR-6624
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_DragonTigerBetTypeFlags8_Bits_Dragon		, "Dragon"		) );
			descr.push_back( FlagDescr( eIceEventParam_DragonTigerBetTypeFlags8_Bits_Tiger		, "Tiger"		) );
			descr.push_back( FlagDescr( eIceEventParam_DragonTigerBetTypeFlags8_Bits_Tie		, "Tie"			) );
			descr.push_back( FlagDescr( eIceEventParam_DragonTigerBetTypeFlags8_Bits_SuitedTie	, "SuitedTie"	) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_DragonTigerBetType	,ePt_Flags8	, "DragonTigerBetType"	, descr	);
		}

		// PYR-63483
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_HandActionFlags8_Bits_Fold				, "Fold"		) );
			descr.push_back( FlagDescr( eIceEventParam_HandActionFlags8_Bits_Check				, "Check"		) );
			descr.push_back( FlagDescr( eIceEventParam_HandActionFlags8_Bits_Call				, "Call"		) );
			descr.push_back( FlagDescr( eIceEventParam_HandActionFlags8_Bits_Bet				, "Bet"			) );
			descr.push_back( FlagDescr( eIceEventParam_HandActionFlags8_Bits_Raise				, "Raise"		) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandPreFlopAction	,ePt_Flags8		, "PreFlopAction"	, descr	);
			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandFlopAction		,ePt_Flags8		, "FlopAction"		, descr	);
			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandTurnAction		,ePt_Flags8		, "TurnAction"		, descr	);
			ADD_ICE_PARAM_FLAGS( eIceEventParam_HandRiverAction		,ePt_Flags8		, "RiverAction"		, descr	);
		}

		// PYR-123925
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr(eIceEventParam_Tourn_PPP_Flags8_Bits_Entry		, "Entry"		) );
			descr.push_back( FlagDescr(eIceEventParam_Tourn_PPP_Flags8_Bits_MidJourney	, "MidJourney"	) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_Tourn_PPPFlags	, ePt_Flags8		, "TournPPPFlags", descr );
		}

		//////////////////////////////////////////////////////////////////////////
		// all non-flag params
		ADD_ICE_PARAM( eIceEventParam_EventType					,ePt_INT16			,"Event Type"					);
		ADD_ICE_PARAM( eIceEventParam_Platform					,ePt_INT32			,"Platform"						);
		ADD_ICE_PARAM( eIceEventParam_Brand						,ePt_INT32			,"Brand"						); // PYR-40455
		ADD_ICE_PARAM( eIceEventParam_Site						,ePt_Flags32		,"Site32"						); // PYR-84447
		ADD_ICE_PARAM( eIceEventParam_SiteLong					,ePt_FlagsLong		,"SiteLong"						); // PYR-100633 	


		ADD_ICE_PARAM( eIceEventParam_RingGameType				,ePt_INT8			,"Ring Game Type"				);
		ADD_ICE_PARAM( eIceEventParam_Structure					,ePt_INT8			,"Structure"					);
		ADD_ICE_PARAM( eIceEventParam_PlayersPerTable			,ePt_INT8			,"PlayersPerTable"				);
		ADD_ICE_PARAM( eIceEventParam_RingLowStake				,ePt_INT32			,"RingLowStake"					); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_RingCap					,ePt_INT32			,"RingCap"						); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_RingAnte					,ePt_INT32			,"RingAnte"						); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_RingCurrency				,ePt_String			,"RingCurrency"					); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_RingMultiGameType			,ePt_INT32			,"Ring Multi Game Type"			); // PYR-66899
		

		ADD_ICE_PARAM( eIceEventParam_TableId					,ePt_INT64			,"tableId"						);
		ADD_ICE_PARAM( eIceEventParam_MergedTableId				,ePt_INT64			,"mergedTableId"				);
		ADD_ICE_PARAM( eIceEventParam_TableSize					,ePt_INT8			,"TableSize"					);
		ADD_ICE_PARAM( eIceEventParam_HandResult				,ePt_INT8			,"HandResult"					);
		ADD_ICE_PARAM( eIceEventParam_FinishedHandAt			,ePt_INT8			,"FinishedHandAt"				);
		ADD_ICE_PARAM( eIceEventParam_RingHighStake				,ePt_INT32			,"RingHighStake"				);
		ADD_ICE_PARAM( eIceEventParam_RingBuyInTime				,ePt_DateTime		,"RingBuyInTime"				);
		ADD_ICE_PARAM( eIceEventParam_RingHandNumberOfSwaps		,ePt_INT8			,"RingHandNumberOfSwaps"		);

		ADD_ICE_PARAM( eIceEventParam_RingNumPlayersDealtIn		,ePt_INT8			,"RingNumPlayersDealtIn"		);

		// PYR-71993
		ADD_ICE_PARAM( eIceEventParam_RingHandNetLossAmount		,ePt_INT64			,"RingNetLossAmount"			);
		ADD_ICE_PARAM( eIceEventParam_RingHandNetWinAmount		,ePt_INT64			,"RingNetWinAmount"				);
		ADD_ICE_PARAM( eIceEventParam_RingHandGrossWinAmount	,ePt_INT64			,"RingGrossWinAmount"			);
		ADD_ICE_PARAM( eIceEventParam_RingHandBetAmount			,ePt_INT64			,"RingBetAmount"				);

		// PYR-33924
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsSuitCount	,ePt_INT8			,"HoleCardsSuitCount"			);
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsHighValue	,ePt_INT8			,"HoleCardsHighValue"			);
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsHighSuit		,ePt_INT8			,"HoleCardsHighSuit"			);
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsLowValue		,ePt_INT8			,"HoleCardsLowValue"			);
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsLowSuit		,ePt_INT8			,"HoleCardsLowSuit"				);
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsGap			,ePt_INT8			,"HoleCardsGap"					);
		
		// PYR-33084
		ADD_ICE_PARAM( eIceEventParam_HandStrengthRank			,ePt_INT8			,"HandStrengthRank"				);
		ADD_ICE_PARAM( eIceEventParam_HandStrengthSuit			,ePt_INT8			,"HandStrengthSuit"				);
		ADD_ICE_PARAM( eIceEventParam_HandStrengthHigh			,ePt_INT8			,"HandStrengthHigh"				);
		ADD_ICE_PARAM( eIceEventParam_HandStrengthLow			,ePt_INT8			,"HandStrengthLow"				);

		// PYR-100104
		ADD_ICE_PARAM( eIceEventParam_PlayersStacked			,ePt_INT16			,"PlayersStacked"				);

		// PYR-106565
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetAmount		,ePt_INT64,			"PokerSideBetAmount"			);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetWinAmount		,ePt_INT64,			"PokerSideBetWinAmount"			);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetNetWinAmount	,ePt_INT64,			"PokerSideBetNetWinAmount"		);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetNetLossAmount	,ePt_INT64,			"PokerSideBetNetLossAmount"		);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetCurrency		,ePt_String,		"PokerSideBetCurrency"			);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetReturn		,ePt_INT32,			"PokerSideBetReturn"			);
		{
			vector<FlagDescr> descr;
			descr.push_back( FlagDescr( eIceEventParam_PokerSideBetType_Flags8_HoleCards, "HoleCards" ) );
			descr.push_back( FlagDescr( eIceEventParam_PokerSideBetType_Flags8_FlopCards, "FlopCards" ) );

			ADD_ICE_PARAM_FLAGS( eIceEventParam_PokerSideBetType, ePt_Flags8, "PokerSideBetResult", descr );
		}
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetPayoutHoleCards	,ePt_INT8,		"PokerSideBetPayoutHoleCards"	);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetPayoutFlopBet		,ePt_INT8,		"PokerSideBetPayoutFlopBet"		);
		ADD_ICE_PARAM( eIceEventParam_PokerSideBetResult			,ePt_INT8,		"PokerSideBetResult"			);
		
		// PYR-72156
		ADD_ICE_PARAM( eIceEventParam_UnfoldAntesReturned		,ePt_INT8			,"UnfoldAntesReturned"			);
		ADD_ICE_PARAM( eIceEventParam_UnfoldResult				,ePt_INT8			,"UnfoldResult"					);
		ADD_ICE_PARAM( eIceEventParam_UnfoldBet					,ePt_INT8			,"UnfoldBet"					);
		ADD_ICE_PARAM( eIceEventParam_UnfoldFinish				,ePt_INT8			,"UnfoldFinish"					);
		ADD_ICE_PARAM( eIceEventParam_UnfoldHandStrengthRank	,ePt_INT8			,"UnfoldHandStrengthRank"		);

		// PYR-63482
		ADD_ICE_PARAM( eIceEventParam_HandPotSizeAmountBB		,ePt_INT64			,"PotSizeAmountBB"				);
		ADD_ICE_PARAM( eIceEventParam_HandPotSizeAmount			,ePt_INT64			,"PotSizeAmount"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_PotSizeAmount		,ePt_INT64			,"TournPotSizeAmount"			);
		ADD_ICE_PARAM( eIceEventParam_Tourn_HandBetAmount		,ePt_INT64			,"TournHandBetAmount"			);

		//we will no allow to create this parameter directly in ice lobby it can only be delivered via event
		//ADD_ICE_PARAM( eIceEventParam_HandHoleCards			,ePt_String			,"HandHoleCards"				);
		ADD_ICE_PARAM( eIceEventParam_HandHoleCardsFilter		,ePt_xFilter		,"HandHoleCardsFilter"			);

		ADD_ICE_PARAM( eIceEventParam_TournId					,ePt_INT64			,"TournId"						);
		ADD_ICE_PARAM( eIceEventParam_Tourn_Group				,ePt_String			,"Tourn_Group"					);
		ADD_ICE_PARAM( eIceEventParam_Tourn_MaxPlayers			,ePt_INT32			,"Tourn_MaxPlayers"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_HandType			,ePt_INT8			,"Tourn_HandType"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_InternalRef			,ePt_String			,"Tourn_InternalRef"			); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_Tourn_PlayedTime			,ePt_DateTime		,"Tourn_PlayedTime"				); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_Tourn_Currency			,ePt_String			,"Tourn_Currency"				); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_Tourn_MinPlayers			,ePt_INT32			,"Tourn_MinPlayers"				); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_Tourn_TargetRef			,ePt_String			,"Tourn_TargetRef"				); // PYR-66899
		ADD_ICE_PARAM( eIceEventParam_Tourn_PPPBuyIn			,ePt_INT64			,"Tourn_PPPBuyIn"				); // PYR-123925
		ADD_ICE_PARAM( eIceEventParam_ProgressivePlusBuyIn		,ePt_INT64			,"ProgPlusPoolBuyIn"			); // PYR-125878
		ADD_ICE_PARAM( eIceEventParam_ProgressivePlusWinAmount	,ePt_INT64			,"ProgPlusPoolWinAmount"		); // PYR-125878
		ADD_ICE_PARAM( eIceEventParam_Tourn_TotalPrizePool		,ePt_INT64			,"TournTotalPrizPool"			); // PYR-124673
		ADD_ICE_PARAM( eIceEventParam_Tourn_BountiesWon			,ePt_INT32			,"TournBountiesWon"				); // PYR-124673

		ADD_ICE_PARAM( eIceEventParam_Tourn_BuyIn				,ePt_INT64			,"Tourn_BuyIn"					);
		ADD_ICE_PARAM( eIceEventParam_Tourn_WinAmount			,ePt_INT64			,"Tourn_WinAmount"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_TargetTourn			,ePt_INT32			,"Tourn_TargetTourn"			);
		ADD_ICE_PARAM( eIceEventParam_Tourn_NumKicked			,ePt_INT8			,"Tourn_NumKicked"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_NumPlayers			,ePt_INT32			,"Tourn_NumPlayers"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_Rank				,ePt_INT32			,"Tourn_Rank"					);
		ADD_ICE_PARAM( eIceEventParam_Tourn_RankPercentile		,ePt_INT16			,"Tourn_RankPercentile"			);
		ADD_ICE_PARAM( eIceEventParam_Tourn_Stake				,ePt_INT64			,"Tourn_Stake"					);

		ADD_ICE_PARAM( eIceEventParam_Tourn_SpnGoMltplier		,ePt_INT32			, "Tourn_SpnGoMltplierOld"		); // obsoleted by PYR-108380
		ADD_ICE_PARAM( eIceEventParam_Tourn_SpnGoType			,ePt_INT8			, "Tourn_SpinGoType"			);
		ADD_ICE_PARAM( eIceEventParam_Tourn_SpnGoJckptLvl		,ePt_INT8			, "Tourn_SpinGoJackpotLevel"	);
		ADD_ICE_PARAM( eIceEventParam_Tourn_Speed				,ePt_INT8			, "Tourn_Speed"					);
		ADD_ICE_PARAM( eIceEventParam_VppsAmount				,ePt_INT64			, "VppsAmount"					);
		ADD_ICE_PARAM( eIceEventParam_TaskGroupId				,ePt_INT32			, "TaskGroupId"					);
		ADD_ICE_PARAM( eIceEventParam_TaskSuccessCount			,ePt_INT8			, "TaskSuccessCount"			);
		ADD_ICE_PARAM( eIceEventParam_TaskFailCount				,ePt_INT8			, "TaskFailCount"				);
		ADD_ICE_PARAM( eIceEventParam_Tourn_SpnGoMltprDec		,ePt_INT32			, "Tourn_SpnGoMltplier"			); // PYR-108380

		ADD_ICE_PARAM( eIceEventParam_TaskRefName				,ePt_String			, "QuestRef"					);
		ADD_ICE_PARAM( eIceEventParam_TaskId					,ePt_INT32			, "TaskId"						);
		ADD_ICE_PARAM( eIceEventParam_StarCodeName				,ePt_String			, "StarCodeName"				);
		ADD_ICE_PARAM( eIceEventParam_CrmOfferRef				,ePt_String			, "crmOfferRefName"				);
		ADD_ICE_PARAM( eIceEventParam_BoardId					,ePt_INT32			, "LeaderBoardId"				);
		ADD_ICE_PARAM( eIceEventParam_BoardSeasonId				,ePt_INT32			, "LeaderBoardSeasonId"			);
		ADD_ICE_PARAM( eIceEventParam_BoardSeasonRank			,ePt_INT32			, "LeaderBoardSeasonRank"		);
		ADD_ICE_PARAM( eIceEventParam_BoardSeasonScore			,ePt_INT64			, "LeaderBoardSeasonScore"		);
		ADD_ICE_PARAM( eIceEventParam_BoardSeasonRankPercentile	,ePt_INT16			, "LeaderBoardSeasonRankPctile"	);

		
		ADD_ICE_PARAM( eIceEventParam_TournTicketTypeId			,ePt_INT32			, "TournTicketTypeId"			);
		ADD_ICE_PARAM( eIceEventParam_RmDepositAmount			,ePt_INT32			, "RmDepositAmount"				);
		ADD_ICE_PARAM( eIceEventParam_RmDepositTransId			,ePt_INT32			, "RmDepositTransId"			);
		ADD_ICE_PARAM( eIceEventParam_RmDepositCurrency			,ePt_String			, "RmDepositCurrency"			);
		ADD_ICE_PARAM( eIceEventParam_RmDepositPaySystemName	,ePt_String			, "RmDepositPaysystem"			);
		ADD_ICE_PARAM( eIceEventParam_Special_FilterMatchTime	,ePt_DateTime		, "FilterMatchTime"				);	
		ADD_ICE_PARAM( eIceEventParam_Special_JoinedQuestTime	,ePt_DateTime		, "JoinedQuestTime"				);	
		ADD_ICE_PARAM( eIceEventParam_Special_TaskCreatedTime	,ePt_DateTime		, "TaskCreatedTime"				);	
		ADD_ICE_PARAM( eIceEventParam_Special_TaskExpireTime	,ePt_DateTime		, "TaskExpireTime"				);	
		ADD_ICE_PARAM( eIceEventParam_Special_TaskActivateTime	,ePt_DateTime		, "TaskActivateTime"			);	
		ADD_ICE_PARAM( eIceEventParam_RmDepositFirst			,ePt_INT8			, "DepositFirst"				);
		ADD_ICE_PARAM( eIceEventParam_VppsVipLevel				,ePt_INT8			, "VppsVipLevel"				);
		ADD_ICE_PARAM( eIceEventParam_VppsMonthly				,ePt_INT64			, "VppsMonthly"					);
		ADD_ICE_PARAM( eIceEventParam_VppsYearly				,ePt_INT64			, "VppsYearly"					);
		ADD_ICE_PARAM( eIceEventParam_FppAmount64				,ePt_INT64			, "FppAmount"					);
		ADD_ICE_PARAM( eIceEventParam_VppsMonthlyBefore			,ePt_INT64			, "VppsMonthlyBefore"			);
		ADD_ICE_PARAM( eIceEventParam_VppsYearlyBefore			,ePt_INT64			, "VppsYearlyBefore"			);
		ADD_ICE_PARAM( eIceEventParam_VppsVipLevelBefore		,ePt_INT8			, "VipLevelBefore"				);
		ADD_ICE_PARAM( eIceEventParam_VppsSource        		,ePt_INT8			, "VppSource"					);
		ADD_ICE_PARAM( eIceEventParam_CvlSchema					,ePt_INT32			, "CvlSchema"					);
		ADD_ICE_PARAM( eIceEventParam_TaskInstanceStateValue1	,ePt_INT64			, "TaskInstanceStateValue1"		);
		ADD_ICE_PARAM( eIceEventParam_TaskInstanceStateValue2	,ePt_INT64			, "TaskInstanceStateValue2"		);
		ADD_ICE_PARAM( eIceEventParam_TaskInstanceStateValue3	,ePt_INT64			, "TaskInstanceStateValue3"		);
		ADD_ICE_PARAM( eIceEventParam_TaskInstanceStateValue4	,ePt_INT64			, "TaskInstanceStateValue4"		);
		ADD_ICE_PARAM( eIceEventParam_TaskInstanceStateValue5	,ePt_INT64			, "TaskInstanceStateValue5"		);
		ADD_ICE_PARAM( eIceEventParam_RmDepositFlags8			,ePt_Flags8			, "DepositFlags"				);
		ADD_ICE_PARAM( eIceEventParam_CppsAmount				,ePt_INT64			, "CppsAmount"					);
		ADD_ICE_PARAM( eIceEventParam_CppsMonthly				,ePt_INT64			, "CppsMonthly"					);
		ADD_ICE_PARAM( eIceEventParam_CppsYearly				,ePt_INT64			, "CppsYearly"					);
		ADD_ICE_PARAM( eIceEventParam_CppsLifetime				,ePt_INT64			, "CppsLifetime"				);
		ADD_ICE_PARAM( eIceEventParam_CppsSource				,ePt_INT8			, "CppsSource"					);
		ADD_ICE_PARAM( eIceEventParam_SppsAmount				,ePt_INT64			, "SppsAmount"					);
		ADD_ICE_PARAM( eIceEventParam_SppsMonthly				,ePt_INT64			, "SppsMonthly"					);
		ADD_ICE_PARAM( eIceEventParam_SppsYearly				,ePt_INT64			, "SppsYearly"					);
		ADD_ICE_PARAM( eIceEventParam_SppsLifetime				,ePt_INT64			, "SppsLifetime"				);
		ADD_ICE_PARAM( eIceEventParam_SppsSource				,ePt_INT8			, "SppsSource"					);
		ADD_ICE_PARAM( eIceEventParam_FirstTwitchTimeLink		,ePt_INT8			, "FirstTimeLink"				); // PYR-112222
		ADD_ICE_PARAM(eIceEventParam_RmDepositTransId64			,ePt_INT64			, "RmDepositTransId64"			); // PYR-116913

		ADD_ICE_PARAM( eIceEventParam_FilterMatchParam			,ePt_INT64			, "FilterMatch"					);
		
		ADD_ICE_PARAM( eIceEventParam_DuelTypeId				,ePt_INT32			, "DuelTypeID"					);
		ADD_ICE_PARAM( eIceEventParam_DuelBuyIn					,ePt_INT64			, "DuelBuyIn"					);
		ADD_ICE_PARAM( eIceEventParam_DuelNumHands				,ePt_INT32			, "DuelNumHands"				);
		ADD_ICE_PARAM( eIceEventParam_DuelNumHandsWon			,ePt_INT32			, "DuelNumHandsWon"				);
		ADD_ICE_PARAM( eIceEventParam_DuelResult				,ePt_INT8			, "DuelResult"					);
		ADD_ICE_PARAM( eIceEventParam_DuelScoreAmount			,ePt_INT32			, "DuelScoreAmount"				);
		ADD_ICE_PARAM( eIceEventParam_DuelScoreMargin			,ePt_INT32			, "DuelScoreMargin"				);
		ADD_ICE_PARAM( eIceEventParam_DuelScorePercent			,ePt_INT16			, "DuelScorePercent"			);
		ADD_ICE_PARAM( eIceEventParam_DuelWinAmount				,ePt_INT64			, "DuelWinAmount"				);
		ADD_ICE_PARAM( eIceEventParam_DuelHandAllin				,ePt_INT8			, "DuelHandAllin"				);	
		ADD_ICE_PARAM( eIceEventParam_DuelHandScoreAmount		,ePt_INT32			, "DuelHandScoreAmount"			);	
		ADD_ICE_PARAM( eIceEventParam_DuelHandFinish			,ePt_INT8			, "DuelHandFinish"				);	
		ADD_ICE_PARAM( eIceEventParam_DuelHoleCardsSuitCount	,ePt_INT8			, "DuelHandHoleCardsSuitCount"	);	
		ADD_ICE_PARAM( eIceEventParam_DuelHoleCardsHighValue	,ePt_INT8			, "DuelHandHoleCardsHighValue"	);	
		ADD_ICE_PARAM( eIceEventParam_DuelHoleCardsHighSuit		,ePt_INT8			, "DuelHandHoleCardsHighSuit"	);
		ADD_ICE_PARAM( eIceEventParam_DuelHoleCardsLowValue		,ePt_INT8			, "DuelHandHoleCardsLowValue"	);
		ADD_ICE_PARAM( eIceEventParam_DuelHoleCardsLowSuit		,ePt_INT8			, "DuelHandHoleCardsLowSuit"	);
		ADD_ICE_PARAM( eIceEventParam_DuelHoleCardsGap			,ePt_INT8			, "DuelHandHoleCardsGap"		);
		ADD_ICE_PARAM( eIceEventParam_DuelHandStrength			,ePt_INT64			, "DuelHandStrength"			);
		ADD_ICE_PARAM( eIceEventParam_DuelHandResult			,ePt_INT8			, "DuelHandResult"				);
		ADD_ICE_PARAM( eIceEventParam_DuelOpponentIntId			,ePt_INT32			, "DuelOpponentIntId"			);	
		ADD_ICE_PARAM( eIceEventParam_DuelScoreMaxAvailable		,ePt_INT64			, "DuelScoreMaxAvailable"		);	

		// PYR-32027
		ADD_ICE_PARAM( eIceEventParam_CountryBefore				,ePt_String			, "CountryBefore"				);
		ADD_ICE_PARAM( eIceEventParam_CountryAfter				,ePt_String			, "CountryAfter"				);
		ADD_ICE_PARAM( eIceEventParam_LicenseBefore				,ePt_INT32			, "LicenseBefore"				);
		ADD_ICE_PARAM( eIceEventParam_LicenseAfter				,ePt_INT32			, "LicenseAfter"				);
		
		// PYR-34344
		ADD_ICE_PARAM( eIceEventParam_CasinoGameCategory		,ePt_INT32			, "CasinoGameCategory"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoGameType			,ePt_INT32			, "CasinoGameType"				);
		ADD_ICE_PARAM( eIceEventParam_CasinoGameVariant			,ePt_INT32			, "CasinoGameVariant"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoGameVendor			,ePt_INT32			, "CasinoGameVendor"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoGameMode			,ePt_INT8			, "CasinoGameMode"				);
		ADD_ICE_PARAM( eIceEventParam_CPPAmount					,ePt_INT64			, "CPPAmount"					);
		ADD_ICE_PARAM( eIceEventParam_CasinoBetAmount			,ePt_INT64			, "CasinoBetAmount"				);
		ADD_ICE_PARAM( eIceEventParam_CasinoGrossWinAmount		,ePt_INT64			, "CasinoGrossWinAmount"		);
		ADD_ICE_PARAM( eIceEventParam_CasinoNetWinAmount		,ePt_INT64			, "CasinoNetWinAmount"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoNetLossAmount		,ePt_INT64			, "CasinoNetLossAmount"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoBetFundSource		,ePt_INT8			, "CasinoBetFundSource"			);
		// PYR-69879
		ADD_ICE_PARAM( eIceEventParam_CasinoBetFundSourceBonusId,ePt_INT32			, "CasinoBetFundSourceBonusId"	);
		// PYR-70794
		ADD_ICE_PARAM( eIceEventParam_CasinoBetReturn			,ePt_INT32			, "CasinoBetReturn"				);

		// PYR-84989 EventCasinoRaceFinish
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceID				, ePt_INT64			, "CasinoRaceID"				);
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceGameTemplateID	, ePt_INT32			, "CasinoRaceGameTemplateID"	);
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceNumPlayers		, ePt_INT32			, "CasinoRaceNumPlayers"		);
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceRank			, ePt_INT32			, "CasinoRaceRank"				);
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceRankPercentile	, ePt_INT32			, "CasinoRaceRankPercentile"	);
		ADD_ICE_PARAM( eIceEventParam_CasinoRacePoints			, ePt_INT64			, "CasinoRacePoints"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceWinAmount		, ePt_INT64			, "CasinoRaceWinAmount"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoRacePrizeType		, ePt_INT8			, "CasinoRacePrizeType"			);
		ADD_ICE_PARAM( eIceEventParam_CasinoRaceSpinCount		, ePt_INT32			, "CasinoRaceSpinCount"			);


		//  PYR-34347 
		ADD_ICE_PARAM( eIceEventParam_BlackjackBetCurrency      ,ePt_String			, "BlackjackBetCurrency"      	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackInitialBetAmount ,ePt_INT64			, "BlackjackInitialBetAmount" 	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackTotalBetAmount   ,ePt_INT64			, "BlackjackTotalBetAmount"   	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackGrossWinAmount   ,ePt_INT64			, "BlackjackGrossWinAmount"   	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackNetWinAmount     ,ePt_INT64			, "BlackjackNetWinAmount"     	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackNumCards         ,ePt_INT16			, "BlackjackNumCards"         	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackDealerTotal      ,ePt_INT64			, "BlackjackDealerTotal"      	);
		ADD_ICE_PARAM( eIceEventParam_BlackjackSeatNumSplits	,ePt_INT16			, "BlackjackSeatNumSplits"		);
		ADD_ICE_PARAM( eIceEventParam_BlackjackNumDoubleDowns	,ePt_INT16			, "BlackjackNumDoubleDowns"		);
		ADD_ICE_PARAM( eIceEventParam_BlackjackPlayerTotal		,ePt_INT8			, "BlackjackPlayerTotal"		);

		// PYR-35623
		ADD_ICE_PARAM( eIceEventParam_NewAccountPromoCode		,ePt_String			, "NewAccountPromoCode"			);
		ADD_ICE_PARAM( eIceEventParam_NewAccountMktgPromoCode	,ePt_String			, "NewAccountMarketingPromoCode");
		ADD_ICE_PARAM( eIceEventParam_NewAccountRefDomain		,ePt_String			, "NewAccountRefDomain"			);
		ADD_ICE_PARAM( eIceEventParam_NewAccountInstallId		,ePt_String			, "NewAccountInstallId"			);
		ADD_ICE_PARAM( eIceEventParam_NewAccountClientPlatform	,ePt_INT32			, "NewAccountClientPlatform"	);

		// PYR-96173
		ADD_ICE_PARAM( eIceEventParam_GameAnalyticsProgramId	,ePt_INT32			, "GameAnalyticsProgramId"		);
		ADD_ICE_PARAM( eIceEventParam_GameAnalyticsProgramName	,ePt_String			, "GameAnalyticsProgramName"	);
		
		// PYR-47136
		ADD_ICE_PARAM( eIceEventParam_NewAccountTinfo			,ePt_String			, "NewAccountTINFO"				);
		// PYR-49359
		ADD_ICE_PARAM( eIceEventParam_NewAccountSiteId			,ePt_INT32			, "NewAccountSiteId"			);
		ADD_ICE_PARAM( eIceEventParam_NewAccountCountry			,ePt_String			, "NewAccountCountry"			);
		ADD_ICE_PARAM( eIceEventParam_NewAccountEmail			,ePt_String			, "NewAccountEmail"				);
		//PYR-51989
		ADD_ICE_PARAM( eIceEventParam_NewAccountAffiliateTracker,ePt_String			, "AffiliateTracker"			);
		ADD_ICE_PARAM( eIceEventParam_DefaultCurrency			,ePt_String			, "DefaultCurrency"				);
		ADD_ICE_PARAM( eIceEventParam_UserStateTrackCode		,ePt_INT32			, "UserTrackCode"				);

		// PYR-35734
		ADD_ICE_PARAM( eIceEventParam_BonusId					,ePt_INT32			, "BonusId"						);
		// PYR-40096
		ADD_ICE_PARAM( eIceEventParam_FirstLTPointsType			,ePt_INT8			, "FirstLTPointsType"			);
		// PYR-39764
		ADD_ICE_PARAM( eIceEventParam_AppLoginId				,ePt_INT64			, "AppLoginId"					);
		// PYR-31472
		ADD_ICE_PARAM( eIceEventParam_TrackCode					,ePt_String			, "TrackCode"					);
		ADD_ICE_PARAM( eIceEventParam_TrackCodeChange			,ePt_INT8			, "TrackCodeChange"				);

		// PYR-37407
		ADD_ICE_PARAM( eIceEventParam_SportsBetAmount			,ePt_INT64			, "SportsBetAmount"				);
		ADD_ICE_PARAM( eIceEventParam_SportsBetCurrency			,ePt_String			, "SportsBetCurrency"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetOdds				,ePt_INT32			, "SportsBetOdds"				);
		ADD_ICE_PARAM( eIceEventParam_SportsEventID				,ePt_INT64			, "SportsEventID"				);
		ADD_ICE_PARAM( eIceEventParam_SportsWinAmount			,ePt_INT64			, "SportsReturnAmount"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetReturn			,ePt_INT32			, "SportsBetReturn"				);
		ADD_ICE_PARAM( eIceEventParam_SportsBetTmngInPlayBets	,ePt_INT16			, "SportsBetTmngInPlayBets"		);
		ADD_ICE_PARAM( eIceEventParam_SportsBetTmngPreMtchBts	,ePt_INT16			, "SportsBetTmngPreMtchBts"		);
		ADD_ICE_PARAM( eIceEventParam_SportsSpinBet				,ePt_INT8			, "SportsSpinBet"				);
		ADD_ICE_PARAM( eIceEventParam_SportsSpinBetMultiplier	,ePt_INT32			, "SportsSpinBetMultiplier"		);
		ADD_ICE_PARAM( eIceEventParam_SportsCompetition			,ePt_String			, "SportsCompetition"			);
		ADD_ICE_PARAM( eIceEventParam_SportsMarketId			,ePt_INT32			, "SportsMarketID"				);
		ADD_ICE_PARAM( eIceEventParam_SportsMarketName			,ePt_String			, "SportsMarketName"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetFundSource		,ePt_INT8			, "SportsBetFundSource"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetSelection		,ePt_String			, "SportsBetSelection"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetSelectionCode	,ePt_String			, "SportsBetSelectionCode"		);
		ADD_ICE_PARAM( eIceEventParam_SportsBetSelectionId		,ePt_INT64			, "SportsBetSelectionID"		);
		ADD_ICE_PARAM( eIceEventParam_SportsBetMarketCode		,ePt_String			, "SportsBetMarketCode"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetMultipleNumLegs	,ePt_INT32			, "SportsBetMultipleNumberLegs"	);
		ADD_ICE_PARAM( eIceEventParam_SportsBetMultipleLegsWon	,ePt_INT32			, "SportsBetMultipleLegsWon"	);
		ADD_ICE_PARAM( eIceEventParam_SportsBetNetWin			,ePt_INT64			, "SportsBetNetWin"				);
		ADD_ICE_PARAM( eIceEventParam_SportsBetNetLoss			,ePt_INT64			, "SportsBetNetLoss"			);
		ADD_ICE_PARAM( eIceEventParam_SportsBetComposition		,ePt_INT8			, "MultipleSportComposition"	);
		ADD_ICE_PARAM( eIceEventParam_SportsBetFundSrcCampgnID	,ePt_INT64			, "SportsBetSourceCampaingId"	);
		
		// PYR-43429
		ADD_ICE_PARAM( eIceEventParam_MiniGameRef				,ePt_String			, "MiniGameRef"					);
		ADD_ICE_PARAM( eIceEventParam_MiniGameResult			,ePt_INT8			, "MiniGameResult"				);
		// PYR-52970
		ADD_ICE_PARAM( eIceEventParam_MiniGamePrizeType			,ePt_INT32			, "MiniGamePrizeType"			);
		ADD_ICE_PARAM( eIceEventParam_MiniGamePrizeAmount		,ePt_INT64			, "MiniGamePrizeAmount"			);
		ADD_ICE_PARAM( eIceEventParam_MiniGamePrizeRefId		,ePt_INT64			, "MiniGamePrizeRefID"			);
		ADD_ICE_PARAM( eIceEventParam_MiniGamePlayId			,ePt_INT64			, "MiniGamePlayID"				);

		
		// PYR-43854
		ADD_ICE_PARAM( eIceEventParam_RouletteBetAmount			,ePt_INT64			, "RouletteBetAmount"			);
		ADD_ICE_PARAM( eIceEventParam_RouletteBetCurrency		,ePt_String			, "RouletteBetCurrency"			);
		ADD_ICE_PARAM( eIceEventParam_RouletteGrossWinAmount	,ePt_INT64			, "RouletteGrossWinAmount"		);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumberSpun		,ePt_INT8			, "RouletteNumberSpun"			);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumBetsPlaced		,ePt_INT16			, "RouletteNumBetsPlaced"		);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumBetsWon		,ePt_INT16			, "RouletteNumBetsWon"			);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumStraightBets	,ePt_INT8			, "RouletteNumStraightBetsPlaced");
		ADD_ICE_PARAM( eIceEventParam_RouletteNumSplitBets		,ePt_INT8			, "RouletteNumSplitBetsPlaced"	);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumCornerBets		,ePt_INT8			, "RouletteNumCornerBetsPlaced"	);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumStreetBets		,ePt_INT8			, "RouletteNumStreetBetsPlaced"	);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumLineBets		,ePt_INT8			, "RouletteNumLineBetsPlaced"	);
		ADD_ICE_PARAM( eIceEventParam_RouletteBetNumber			,ePt_INT8			, "RouletteBetNumber"			);
		ADD_ICE_PARAM( eIceEventParam_RouletteBetSubNumber		,ePt_INT8			, "RouletteBetSubNumber"		);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumTypeBetsPlaced	,ePt_INT16			, "RouletteNumTypeBetsPlaced"	);
		// PYR-55908
		ADD_ICE_PARAM( eIceEventParam_RouletteNumRedBlackBetsPlaced	,ePt_INT8		, "RouletteNumRedBlackBetsPlaced"	);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumOddEvenBetPlaced	,ePt_INT8		, "RouletteNumOddEvenBetPlaced"		);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumHighLowBetPlaced	,ePt_INT8		, "RouletteNumHighLowBetPlaced"		);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumDozensBetPlaced	,ePt_INT8		, "RouletteNumDozensBetPlaced"		);
		ADD_ICE_PARAM( eIceEventParam_RouletteNumColumnBetPlaced	,ePt_INT8		, "RouletteNumColumnBetPlaced"		);
		// PYR-65905
		ADD_ICE_PARAM( eIceEventParam_RouletteSpinReturn		,ePt_INT32			, "RouletteSpinReturn"			);
		ADD_ICE_PARAM( eIceEventParam_RouletteBetReturn			,ePt_INT32			, "RouletteBetReturn"			);

		// PYR-45971
		ADD_ICE_PARAM( eIceEventParam_SlotLinesPlayed			,ePt_INT8			, "SlotLinesPlayed"				);
		ADD_ICE_PARAM( eIceEventParam_SlotLinesWon				,ePt_INT8			, "SlotLinesWon"				);
		ADD_ICE_PARAM( eIceEventParam_SlotNumDifferentSymbols	,ePt_INT8			, "SlotNumDifferentSymbols"		);
		ADD_ICE_PARAM( eIceEventParam_SlotBonusGameGrossWinAmount,ePt_INT64			, "SlotBonusGameGrossWinAmount"	);
		ADD_ICE_PARAM( eIceEventParam_SlotBonusGameType			,ePt_INT8			, "SlotBonusGameType"			);

		ADD_ICE_PARAM_DIM( eIceEventParam_SlotNumSymbol			,ePt_INT8			, "SlotNumSymbol"	, PDim(15)	); // vector of 15
		ADD_ICE_PARAM_DIM( eIceEventParam_SlotLocation			,ePt_INT8			, "SlotLocation"	, PDim(5,3)	); // matrix 5 x 3

		// PYR-39805
		ADD_ICE_PARAM( eIceEventParam_FreemiumPurchaseAmount	,ePt_INT64			, "FreemiumPurchaseAmount"		);
		ADD_ICE_PARAM( eIceEventParam_FreemiumPurchaseFirst		,ePt_INT8			, "FreemiumPurchaseFirst"		);

		// PYR-56058
		ADD_ICE_PARAM( eIceEventParam_ChestTier					,ePt_INT8			, "ChestTier"					);
		ADD_ICE_PARAM( eIceEventParam_ChestLevelUp				,ePt_INT8			, "ChestLevelUp"				);
		ADD_ICE_PARAM( eIceEventParam_ChestTypeId				,ePt_INT32			, "ChestTypeId"					);
		ADD_ICE_PARAM( eIceEventParam_ChestIssuerType			,ePt_INT8			, "ChestIssuer"					);
		ADD_ICE_PARAM( eIceEventParam_ChestExchangeType			,ePt_INT8			, "ChestExchangeType"			);

		// PYR-59835
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherBetAmount			,ePt_INT64		, "DreamCatcherBetAmount"		);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherBetCurrency		,ePt_String		, "DreamCatcherBetCurrency"		);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherGrossWinAmount	,ePt_INT64		, "DreamCatcherGrossWinAmount"	);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherNetWinAmount		,ePt_INT64		, "DreamCatcherNetWinAmount"	);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherNetLossAmount		,ePt_INT64		, "DreamCatcherNetLossAmount"	);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherNumberSpun		,ePt_INT8		, "DreamCatcherNumberSpun"		);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherNumBetsPlaced		,ePt_INT16		, "DreamCatcherNumBetsPlaced"	);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherSpinReturn		,ePt_INT32		, "DreamCatcherSpinReturn"		);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherBetReturn			,ePt_INT32		, "DreamCatcherBetReturn"		);
		ADD_ICE_PARAM( eIceEventParam_DreamCatcherMultiplier		,ePt_INT8		, "DreamCatcherMultiplier"		);

		// PYR-67812
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusBetAmount			,ePt_INT64		, "TexasHoldemBonusBetAmount"				);
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusBetCurrency		,ePt_String		, "TexasHoldemBonusBetCurrency"				);
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusGrossWinAmount	,ePt_INT64		, "TexasHoldemBonusGrossWinAmount"			);
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusNetWinAmount		,ePt_INT64		, "TexasHoldemBonusNetWinAmount"			);
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusNetLossAmount		,ePt_INT64		, "TexasHoldemBonusNetLossAmount"			);
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusHandStrengthRank	,ePt_INT8		, "TexasHoldemBonusPlayerHandStrengthRank"	);
		ADD_ICE_PARAM( eIceEventParam_TexasHoldemBonusBetReturn			,ePt_INT32		, "TexasHoldemBonusBetReturn"				);

		// PYR-66738
		ADD_ICE_PARAM( eIceEventParam_BaccaratBetAmount				,ePt_INT64		, "BaccaratBetAmount"			);
		ADD_ICE_PARAM( eIceEventParam_BaccaratBetCurrency			,ePt_String		, "BaccaratBetCurrency"			);
		ADD_ICE_PARAM( eIceEventParam_BaccaratGrossWinAmount		,ePt_INT64		, "BaccaratGrossWinAmount"		);
		ADD_ICE_PARAM( eIceEventParam_BaccaratNetWinAmount			,ePt_INT64		, "BaccaratNetWinAmount"		);
		ADD_ICE_PARAM( eIceEventParam_BaccaratNetLossAmount			,ePt_INT64		, "BaccaratNetLossAmount"		);
		ADD_ICE_PARAM( eIceEventParam_BaccaratNumBetsPlaced			,ePt_INT16		, "BaccaratNumBetsPlaced"		);
		ADD_ICE_PARAM( eIceEventParam_BaccaratPlayerTotal			,ePt_INT8		, "BaccaratPlayerTotal"			);
		ADD_ICE_PARAM( eIceEventParam_BaccaratBankerTotal			,ePt_INT8		, "BaccaratBankerTotal"			);

		// PYR-66224
		ADD_ICE_PARAM( eIceEventParam_DragonTigerBetAmount			,ePt_INT64		, "DragonTigerBetAmount"		);
		ADD_ICE_PARAM( eIceEventParam_DragonTigerBetCurrency		,ePt_String		, "DragonTigerBetCurrency"		);
		ADD_ICE_PARAM( eIceEventParam_DragonTigerGrossWinAmount		,ePt_INT64		, "DragonTigerGrossWinAmount"	);
		ADD_ICE_PARAM( eIceEventParam_DragonTigerNetWinAmount		,ePt_INT64		, "DragonTigerNetWinAmount"		);
		ADD_ICE_PARAM( eIceEventParam_DragonTigerNetLossAmount		,ePt_INT64		, "DragonTigerNetLossAmount"	);
		ADD_ICE_PARAM( eIceEventParam_DragonTigerNumBetsPlaced		,ePt_INT16		, "DragonTigerNumBetsPlaced"	);

		// PYR-82915
		ADD_ICE_PARAM( eIceEventParam_JackpotWinType				,ePt_INT8		, "JackpotWinType"				);
		ADD_ICE_PARAM( eIceEventParam_JackpotType					,ePt_INT8		, "JackpotType"					);
		ADD_ICE_PARAM( eIceEventParam_JackpotWinAmount				,ePt_INT64		, "JackpotWinAmount"			);

		// PYR-92702
		ADD_ICE_PARAM( eIceEventParam_KYCDocType					,ePt_INT8		, "KYCDocType"					);
		ADD_ICE_PARAM( eIceEventParam_KYCDocSubCategory				,ePt_INT8		, "KYCDocSubCategory"			);

		// PYR-54691
		ADD_ICE_PARAM( eIceEventParam_RewardPointAmount				,ePt_INT32		, "RewardPointAmount"			);
		ADD_ICE_PARAM( eIceEventParam_RewardPointSource				,ePt_INT8		, "RewardPointSource"			);

		// PYR-82915
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTribeSelection			,ePt_INT8	, "VictoryTribesTribeSelection"			);	
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesLevel					,ePt_INT8	, "VictoryTribesLevel"					);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesBonusTournType			,ePt_INT8	, "VictoryTribesBonusTournType"			);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTournNumPlayers			,ePt_INT32	, "VictoryTribesTournNumPlayers"		);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTournamentCoins			,ePt_INT64	, "VictoryTribesTournamentCoins"		);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTournRank				,ePt_INT32	, "VictoryTribesTournRank"				);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTournRankPercentile		,ePt_INT16	, "VictoryTribesTournRankPercentile"	);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTournPrizeAmount			,ePt_INT64	, "VictoryTribesTournPrizeAmount"		);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesLevelUp					,ePt_INT8	, "VictoryTribesLevelUp"				);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesLevelDown				,ePt_INT8	, "VictoryTribesLevelDown"				);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesFreeSpinsTrigger			,ePt_INT8	, "VictoryTribesFreeSpinsTrigger"		);
		ADD_ICE_PARAM( eIceEventParam_VictoryTribesTribalFreeSpinsTrigger	,ePt_INT8	, "VictoryTribesTribalFreeSpinsTrigger"	);
		
		// special parameter - appears as a string in the parser but loaded as xFilter
		ADD_ICE_PARAM( eIceEventParam_SetVar						,ePt_xFilter	, "SetVar"						);
		for( INT32 i = eIceEventParam_StateFilterFirst; i < eIceEventParam_StateFilterLast; ++i )
		{
			ADD_ICE_PARAM( i,	ePt_INT64, PString( "StateFilter" ).appendInt( i )	);
		}
		// another special parameter which will not occur in events and does not actually get added to parameters
		// but we still need it in the validator because it gets loaded from DB and gets translated to a flag on an
		// instance filter collection
		//
		ADD_ICE_PARAM( eIceEventParam_EventFlowControl			,ePt_INT32			, "EventFlowControl"			);

#undef ADD_ICE_PARAM
#undef ADD_ICE_PARAM_FLAGS

	}
	bool isValid( const IceParamData& param ) const
	{
		const ParamType* pt = paramMap.findValue( param.getId() );
		if( !pt )
		{
			PLog("Bad event id %d", param.getId());
			return false;
		}
		if( pt->type != param.getType() )
		{
			PLog("event type %d != %u '%s'", param.getType(), pt->type, pt->name.c_str());
			return false;
		}
		return true;
	}
	bool getParamType( const INT16 paramId,  INT8& typeId, PString& errDescr ) const
	{
		const ParamType* pt = paramMap.findValue( paramId );
		if( !pt )
		{
			errDescr.assign( "Unknown parameter id:" ).appendInt( paramId );
			PLog( LOG_TRACE_FAULTY_LOGIC ": paramId: %d no type", paramId );
			typeId = ePt_Unknown;
			return false; 
		}
		typeId = pt->type;
		return true;
	}
	const ParamType* getParamTypePtr( const INT16 paramId, PString& errDescr ) const
	{
		const ParamType* pt = paramMap.findValue( paramId );
		if( !pt )
		{
			errDescr.assign( "Unknown parameter id:" ).appendInt( paramId );
			PLog( LOG_TRACE_FAULTY_LOGIC ": paramId: %d no type", paramId );
			return nullptr; 
		}
		return pt;
	}
	bool isSupportsArray( INT16 paramId ) const override
	{
		if( !paramId )
			return false;

		PString errDescrUnused;
		const ParamType* pt = getParamTypePtr( paramId, errDescrUnused );
		if( pt )
			return pt->dimensions.size() > 0;

		return false;
	}
	bool isAutoEnollPermitted( const INT32 eventType )
	{
		return autoEnrolPermittedEventTypes.find( eventType ) != autoEnrolPermittedEventTypes.end();
	}
	void setAutoEnrollPermitted( const vector<INT32>& v )
	{
		autoEnrolPermittedEventTypes.clear();
		copy( v.begin(), v.end(), inserter( autoEnrolPermittedEventTypes, autoEnrolPermittedEventTypes.begin() ) );
	}
};

//////////////////////////////////////////////////////////////////////////

class IceParamComparator
{
public:
	static bool compareParams( const IceParamData& lhs, const IceParamData& rhs, const INT32 action, bool& isMatch, const INT16 overrideRhsParamId = 0  )
	{
		lhs.trace5( "lhs" );
		rhs.trace5( "rhs" );

		isMatch = false;

		if( rhs.isCustomFilter() )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": compare params rhs is custom" );
			return false;
		}
		// lhs custom filter processing - cannot handle both as custom filters at the same time
		// if rhs is a custom - .compare() will fail with a faulty logic
		if( lhs.isCustomFilter() )
		{
			const ParamCustomCompareInterface* cciLHS = lhs.getCustomCompareInterface();
			if( !cciLHS || !rhs.isEqualType( cciLHS->getCompareType(), cciLHS->getCompareParamId() ) )
			{
				return false;
			}
			switch( action )
			{
			case eIceParamComparator_Less:				isMatch = cciLHS->compare( rhs ) <  0;	return true;
			case eIceParamComparator_LessOrEqual:		isMatch = cciLHS->compare( rhs ) <= 0;	return true;
			case eIceParamComparator_Equal:				isMatch = cciLHS->compare( rhs ) == 0;	return true;
			case eIceParamComparator_GreaterOrEqual:	isMatch = cciLHS->compare( rhs ) >= 0;	return true;
			case eIceParamComparator_Greater:			isMatch = cciLHS->compare( rhs ) >  0;	return true;
			case eIceParamComparator_NotEqual:			isMatch = cciLHS->compare( rhs ) != 0;	return true;
			};
			return false;
		}
		// compare regular parameters (non-custom)
		if( !lhs.isEqualType(rhs, overrideRhsParamId) )
		{
			return false;
		}
		switch( action )
		{
		case eIceParamComparator_Less:				isMatch = lhs.compare( rhs ) <  0;	return true;
		case eIceParamComparator_LessOrEqual:		isMatch = lhs.compare( rhs ) <= 0;	return true;
		case eIceParamComparator_Equal:				isMatch = lhs.compare( rhs ) == 0;	return true;
		case eIceParamComparator_GreaterOrEqual:	isMatch = lhs.compare( rhs ) >= 0;	return true;
		case eIceParamComparator_Greater:			isMatch = lhs.compare( rhs ) >  0;	return true;
		case eIceParamComparator_NotEqual:			isMatch = lhs.compare( rhs ) != 0;	return true;
		};
		return false;
	}
	static void compareNumericValue64( const INT64 lhs, const INT64 rhs, const INT32 action, bool& isMatch )
	{
		switch( action )
		{
		case eIceParamComparator_Less:				isMatch = lhs <  rhs;	break;
		case eIceParamComparator_LessOrEqual:		isMatch = lhs <= rhs;	break;
		case eIceParamComparator_Equal:				isMatch = lhs == rhs;	break;
		case eIceParamComparator_GreaterOrEqual:	isMatch = lhs >= rhs;	break;
		case eIceParamComparator_Greater:			isMatch = lhs >  rhs;	break;
		case eIceParamComparator_NotEqual:			isMatch = lhs != rhs;	break;
		};
	}
	static const char* compareActionCode2String( const INT32 action )
	{
		switch( action )
		{
		case eIceParamComparator_Less:				return "<"	;
		case eIceParamComparator_LessOrEqual:		return "<="	;
		case eIceParamComparator_Equal:				return "=="	;
		case eIceParamComparator_GreaterOrEqual:	return ">="	;
		case eIceParamComparator_Greater:			return ">"	;
		case eIceParamComparator_NotEqual:			return "!="	;
		};
		return "<unknown action>";
	}
	static bool compareWithCustomRHS( const IceParamData& eventDataLHS, const ParamCustomCompareInterface& cciRHS, const INT32 action, bool& isMatch )
	{
#if PTRACELVL >= 5
		eventDataLHS.trace5( "lhs" );
		PString str5;
		cciRHS.toString( str5 );
		PLog( "cciRHS: '%s'", str5.c_str() );
#endif

		isMatch = false;
		if( !eventDataLHS.isEqualType( cciRHS.getCompareType(), cciRHS.getCompareParamId() ) )
		{
			return false;
		}
		switch( action )
		{
			// *** 0 appears on the left in the equation because it compares to the rhs.compare( lhs ) as custom filter is always an rhs operand
		case eIceParamComparator_Less:				isMatch = ( 0 <  cciRHS.compare( eventDataLHS ) );	return true;
		case eIceParamComparator_LessOrEqual:		isMatch = ( 0 <= cciRHS.compare( eventDataLHS ) );	return true;
		case eIceParamComparator_Equal:				isMatch = ( 0 == cciRHS.compare( eventDataLHS ) );	return true;
		case eIceParamComparator_GreaterOrEqual:	isMatch = ( 0 >= cciRHS.compare( eventDataLHS ) );	return true;
		case eIceParamComparator_Greater:			isMatch = ( 0 >  cciRHS.compare( eventDataLHS ) );	return true;
		case eIceParamComparator_NotEqual:			isMatch = ( 0 != cciRHS.compare( eventDataLHS ) );	return true;
		};
		return false;
	}
};


class IceParamUtils
{
public:
	static INT64 combineBlinds( const INT32 smallBlind, const INT32 bigBlind )
	{
		return ( (INT64)smallBlind | (((INT64)bigBlind)<<32) );
	}
};
//////////////////////////////////////////////////////////////////////////



#endif // iceshared_h_included
