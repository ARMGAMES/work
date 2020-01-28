#ifndef variant_h_included
#define	variant_h_included

#include "ppinclude.h"
#include "commmsgbody.h"
#include "ppstring.h"
#include "i18n.strmap.h"

// the type is unknown, value is undefined
#define VARIANT_UNKNOWN	0

// block types
#define VARIANT_STRING	1
#define VARIANT_PBLOCK	2
#define VARIANT_MSGBODY 3

// integral types
#define VARIANT_BOOLEAN	10
#define VARIANT_BYTE	11
#define VARIANT_UINT8	11
#define VARIANT_INT8	12
#define VARIANT_UINT16	13
#define VARIANT_INT16	14
#define VARIANT_UINT32	15
#define VARIANT_INT32	16
#define VARIANT_UINT64	17
#define VARIANT_INT64	18

// composite types
#define VARIANT_TIME	21
#define VARIANT_I18NSTRING 22

// Variant is a container values of various types
// sizeof(Variant)=9

class Variant
{
public:

	// default constructor
	Variant()
	{
		type = VARIANT_UNKNOWN;
	}

	// copy constructor
	Variant( const Variant& source )
	{
		_assign( source );
	}

	// destructor
	~Variant()
	{
		clear();
	}

	// assignment operator
	Variant& operator=( const Variant& source )
	{
		clear();
		_assign( source );
		return *this;
	}

	// clear the current value
	void clear()
	{
		switch( type )
		{
		case VARIANT_STRING:
			delete valueString;
			valueString = 0;
			break;
		case VARIANT_PBLOCK:
			delete valueBlock;
			valueBlock = 0;
			break;
		case VARIANT_MSGBODY:
			delete valueMsgBody;
			valueMsgBody = 0;
			break;
		case VARIANT_I18NSTRING:
			delete valueI18nString;
			valueI18nString = 0;
			break;
		}
		type = VARIANT_UNKNOWN;
	}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	Variant( Variant&& other )
	{
		moveFrom( other );
	}
	Variant& operator=( Variant&& other )
	{
		moveFrom( other );
		return *this;
	}

	void moveFrom( Variant& other )
	{
		type = other.type;
		other.type = VARIANT_UNKNOWN;
		switch( type )
		{
		case VARIANT_UNKNOWN:
			break;
		case VARIANT_STRING:
			valueString = other.valueString;
			other.valueString = 0;
			break;
		case VARIANT_PBLOCK:
			valueBlock = other.valueBlock;
			other.valueBlock = 0;
			break;
		case VARIANT_MSGBODY:
			valueMsgBody = other.valueMsgBody;
			other.valueMsgBody = 0;
			break;
		case VARIANT_BOOLEAN:
			valueBool = other.valueBool;
			break;
		case VARIANT_BYTE:
			valueBYTE = other.valueBYTE;
			break;
		case VARIANT_INT32:
			valueINT32 = other.valueINT32;
			break;
		case VARIANT_UINT32:
			valueUINT32 = other.valueUINT32;
			break;
		case VARIANT_INT64:
			valueINT64 = other.valueINT64;
			break;
		case VARIANT_UINT64:
			valueUINT64 = other.valueUINT64;
			break;
		case VARIANT_I18NSTRING:
			valueI18nString = other.valueI18nString;
			other.valueI18nString = 0;
			break;
		default:
			PASSERT5( false ); // add your type here
		}
	}
#endif

	BYTE getType() const
	{
		return type;
	}

	// comparison operator
	bool operator==( const Variant& source )
	{
		if( type != source.type )
			return false;
		switch( type )
		{
		case VARIANT_UNKNOWN:
			return true;
		case VARIANT_STRING:
			return PString::compare(valueString->c_str(),source.valueString->c_str())==0;
		case VARIANT_PBLOCK:
			return valueBlock->size() == source.valueBlock->size() &&
				memcmp(valueBlock->ptr(), source.valueBlock->ptr(), valueBlock->size() ) == 0;
		case VARIANT_MSGBODY:
			return false;
		case VARIANT_BOOLEAN:
			return valueBool == source.valueBool;
		case VARIANT_BYTE:
			return valueBYTE == source.valueBYTE;
		case VARIANT_INT32:
			return valueINT32 == source.valueINT32;
		case VARIANT_UINT32:
			return valueUINT32 == source.valueUINT32;
		case VARIANT_INT64:
			return valueINT64 == source.valueINT64;
		case VARIANT_UINT64:
			return valueUINT64 == source.valueUINT64;
		case VARIANT_I18NSTRING:
			return valueI18nString->equals(*source.valueI18nString);
		default:
			PASSERT5( false ); // add your type here
		}
		return false;
	}

	bool operator!=( const Variant& source )
	{
		return !(*this==source);
	}

public: // constructors from values of various types

	Variant( bool value )
	{
		type = VARIANT_BOOLEAN;
		valueBool = value;
	}

	Variant( BYTE value )
	{
		type = VARIANT_BYTE;
		valueBYTE = value;
	}

	Variant( UINT32 value )
	{
		type = VARIANT_UINT32;
		valueUINT32 = value;
	}

	Variant( INT32 value )
	{
		type = VARIANT_INT32;
		valueINT32 = value;
	}

	Variant( UINT64 value )
	{
		type = VARIANT_UINT64;
		valueUINT64 = value;
	}

	Variant( INT64 value )
	{
		type = VARIANT_INT64;
		valueINT64 = value;
	}

	Variant( const char* value )
	{
		type = VARIANT_STRING;
		valueString = new PString( value );
	}
	Variant( const char* begin, const char* end )
	{
		type = VARIANT_STRING;
		valueString = new PString( begin, end );
	}
	Variant( const PString& value )
	{
		type = VARIANT_STRING;
		valueString = new PString( value );
	}

	Variant( const CommMsgBody& value )
	{
		type = VARIANT_MSGBODY;
		valueMsgBody = new CommMsgBody;
		valueMsgBody->copyFrom( value );
	}

	Variant( const I18nPString& value )
	{
		type = VARIANT_I18NSTRING;
		valueI18nString = new I18nPString;
		valueI18nString->assign( value );
	}

public: // access operators (with type verification)

	const char* c_str() const
	{
		PASSERT( type == VARIANT_STRING );
		return valueString->c_str();
	}

	bool& getBool()
	{
		PASSERT( type == VARIANT_BOOLEAN );
		return valueBool;
	}

	bool c_bool() const
	{
		PASSERT( type == VARIANT_BOOLEAN );
		return valueBool;
	}

	BYTE c_byte() const
	{
		PASSERT( type == VARIANT_BYTE );
		return valueBYTE;
	}

	PString& getPString()
	{
		PASSERT( type == VARIANT_STRING );
		return *valueString;
	}

	const PString& getPString() const
	{
		PASSERT( type == VARIANT_STRING );
		return *valueString;
	}

	PBlock& getPBlock()
	{
		PASSERT( type == VARIANT_PBLOCK );
		return *valueBlock;
	}

	const PBlock& getPBlock() const
	{
		PASSERT( type == VARIANT_PBLOCK );
		return *valueBlock;
	}

	const CommMsgBody& getMsgBody() const
	{
		PASSERT( type == VARIANT_MSGBODY);
		return *valueMsgBody;
	}

	UINT32& getUINT32()
	{
		PASSERT( type == VARIANT_UINT32 );
		return valueUINT32;
	}

	INT32& getINT32()
	{
		PASSERT( type == VARIANT_INT32 );
		return valueINT32;
	}

	UINT32 c_uint32() const
	{
		PASSERT( type == VARIANT_UINT32 );
		return valueUINT32;
	}

	INT32 c_int32() const
	{
		PASSERT( type == VARIANT_INT32 );
		return valueINT32;
	}

	UINT64& getUINT64()
	{
		PASSERT( type == VARIANT_UINT64 );
		return valueUINT64;
	}

	INT64& getINT64()
	{
		PASSERT( type == VARIANT_INT64 );
		return valueINT64;
	}

	UINT64 c_uint64() const
	{
		PASSERT( type == VARIANT_UINT64 );
		return valueUINT64;
	}

	INT64 c_int64() const
	{
		PASSERT( type == VARIANT_INT64 );
		return valueINT64;
	}

	PString& getPStringByLocale(UINT32 locale)
	{
		PASSERT( type == VARIANT_I18NSTRING );
		return valueI18nString->getPString( locale );
	}

	const PString& getPStringByLocale(UINT32 locale) const
	{
		PASSERT( type == VARIANT_I18NSTRING );
		return valueI18nString->getPString( locale );
	}

	const I18nPString& getI18nString() const
	{
		PASSERT( type == VARIANT_I18NSTRING );
		return *valueI18nString;
	}

public: // converters

	PString& toString( PString& result ) const
	{
		result.cut( 0 );
		switch( type )
		{
		case VARIANT_UNKNOWN:
			return result;
		case VARIANT_STRING:
			return result.assign( *valueString );
		case VARIANT_BOOLEAN:
			return result.assign( valueBool? "true": "false" );
		case VARIANT_BYTE:
			return result.appendUint( valueBYTE );
		case VARIANT_INT32:
			return result.appendInt( valueINT32 );
		case VARIANT_UINT32:
			return result.appendUint( valueUINT32 );
		case VARIANT_INT64:
			return result.appendInt64( valueINT64 );
		case VARIANT_UINT64:
			return result.appendUint64( valueUINT64 );
		default:
			return result;
		}
	}

	const char* toStringWithType(PString& res) const //only integral and string types are supported
	{// <type>=<value>
		switch(type)
		{
		case VARIANT_STRING:	res.append("s=").append(valueString->c_str());	break;
		case VARIANT_BOOLEAN:	res.append("b=").append(valueBool ? '1' : '0');	break;
		case VARIANT_BYTE:		res.append("1=").appendUint(valueBYTE);			break;
		case VARIANT_INT32:		res.append("7=").appendInt(valueINT32);			break;
		case VARIANT_UINT32:	res.append("4=").appendUint(valueUINT32);		break;
		case VARIANT_INT64:		res.append("9=").appendInt64(valueINT64);		break;
		case VARIANT_UINT64:	res.append("8=").appendUint64(valueUINT64);		break;
		default:	break;
		}
		return res.c_str();
	}

public: // serialization

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeBYTE( type );
		switch( type )
		{
			case VARIANT_UNKNOWN:
				return body;
			case VARIANT_PBLOCK:
				return body._composeVarBlock( valueBlock->ptr(), valueBlock->size() );
			case VARIANT_STRING:
				return body.composeString( valueString->c_str() );
			case VARIANT_MSGBODY:
				return body.composeMsgBody(*valueMsgBody);
			case VARIANT_BOOLEAN:
				return body.composeBOOL( valueBool );
			case VARIANT_BYTE:
				return body.composeBYTE( valueBYTE );
			case VARIANT_INT32:
				return body.composeINT32( valueINT32 );
			case VARIANT_UINT32:
				return body.composeUINT32( valueUINT32 );
			case VARIANT_INT64:
				return body.composeINT64( valueINT64 );
			case VARIANT_UINT64:
				return body.composeUINT64( valueUINT64 );
			case VARIANT_I18NSTRING:
				valueI18nString->compose( body );
				return body;
			default:
				PASSERT( false ); // add composing for new types
		}
		return body;
	}

	CommMsgParser& parse( CommMsgParser& parser )
	{
		clear();
		parser.parseBYTE( type );
		switch( type )
		{
		case VARIANT_UNKNOWN:
			return parser;
		case VARIANT_PBLOCK:
			valueBlock = new PBlock();
			return parser.parsePBlock( *valueBlock );
		case VARIANT_STRING:
			valueString = new PString();
			return parser.parseStringP( *valueString );
		case VARIANT_MSGBODY:
			valueMsgBody = new CommMsgBody();
			return parser.parseMsgBody(*valueMsgBody);
		case VARIANT_BOOLEAN:
			return parser.parseBOOL( valueBool );
		case VARIANT_BYTE:
			return parser.parseBYTE( valueBYTE );
		case VARIANT_INT32:
			return parser.parseINT32( valueINT32 );
		case VARIANT_UINT32:
			return parser.parseUINT32( valueUINT32 );
		case VARIANT_INT64:
			return parser.parseINT64( valueINT64 );
		case VARIANT_UINT64:
			return parser.parseUINT64( valueUINT64 );
		case VARIANT_I18NSTRING:
			valueI18nString = new I18nPString;
			valueI18nString->parse( parser );
			return parser;
		default:
			PASSERT( false ); // add parsing for new types
		}
		return parser;
	}

private:

	BYTE type;

	union //-V2514
	{
		bool valueBool;
		BYTE valueBYTE;
		UINT32 valueUINT32;
		INT32 valueINT32;
		UINT64 valueUINT64;
		INT64 valueINT64;
		PString* valueString;          //-V117
		PBlock* valueBlock;            //-V117
		CommMsgBody* valueMsgBody;     //-V117
		I18nPString* valueI18nString;  //-V117
	};

	void _assign( const Variant& source )
	{
		type = source.type;
		switch( type )
		{
		case VARIANT_UNKNOWN:
			break;
		case VARIANT_STRING:
			valueString = new PString( *source.valueString );
			break;
		case VARIANT_PBLOCK:
			valueBlock = new PBlock( *source.valueBlock );
			break;
		case VARIANT_MSGBODY:
			valueMsgBody = new CommMsgBody();
			valueMsgBody->copyFrom( *source.valueMsgBody );
			break;
		case VARIANT_BOOLEAN:
			valueBool = source.valueBool;
			break;
		case VARIANT_BYTE:
			valueBYTE = source.valueBYTE;
			break;
		case VARIANT_INT32:
			valueINT32 = source.valueINT32;
			break;
		case VARIANT_UINT32:
			valueUINT32 = source.valueUINT32;
			break;
		case VARIANT_INT64:
			valueINT64 = source.valueINT64;
			break;
		case VARIANT_UINT64:
			valueUINT64 = source.valueUINT64;
			break;
		case VARIANT_I18NSTRING:
			valueI18nString = new I18nPString;
			valueI18nString->assign( *source.valueI18nString );
			break;
		default:
			PASSERT5( false ); // add your type here
		}
	}
};

#endif //variant_h_included
