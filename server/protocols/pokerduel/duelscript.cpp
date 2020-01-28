
#include "duelscript.h"
#include "PokerDuel.h"
#include "scripts.h"
#include "scriptstringparser.h"

#include "pokerduel\pokerduelprot_thin.h"

#define _HandsPerPlayer			"HandsPerPlayer"
#define _TimeBankMinutes		"TimeBankMinutes"
#define _TimeBankIncrement		"TimeBankIncrement"
#define _TimeoutTimeBankIncrement "TimeoutTimeBankIncrement"
#define _MatchType				"MatchType"
#define _DuelName				"DuelName"

#define _RandomMatchTimeout			"RandomMatchTimeout"
#define _PendingChallengeTimeout	"PendingChallengeTimeout"

//	TODO: create a new file in protocols, scriptUtils.cpp, and move these global function to there, and refactor lobby to use these

//	Copied from scripts.cpp
inline unsigned long xstrtoul( const char* s, const char** end_p, int radix )
{
	char* pp;
	unsigned long ret = strtoul( s, &pp, radix );
	if( end_p )
		*end_p = pp;
	return ret;
}
//	Copied from scripts.cpp
static UINT32 parseAmount( const char* strBuff, const char*& dummy, const char* propName = 0 )
{
	UINT32 amount = 0;
	dummy = 0;
	if( *strBuff )
	{
		strBuff = PString::ltrim( strBuff );
		amount = 100 * xstrtoul( strBuff, &dummy, 10 );

		strBuff = PString::ltrim( dummy );
		if( *strBuff == '.' )
		{
			strBuff = PString::ltrim( strBuff + 1 );
			UINT32 temp = xstrtoul( strBuff, &dummy, 10 );
			if( ( dummy - strBuff ) != 2 )
			{
				PString err;
				if( propName )
					err.assign( "Error in tournament script property '" ).append( propName ).append( "' : " );
				throw PError( err.append( "Scan Error after the dot (nonzero remainder of cents)" ) );
			}

			amount += temp;
		}
	}

	return amount;
}
//	Copied from scripts.cpp
static UINT32 getAmountProperty( const PIniFile::Section* section, const char* propName )
{
	const char* dummy;
	const char* strBuff = section->getProperty( propName, "" );
	return parseAmount( strBuff,  dummy, propName );
}

//	Copied from scripts.cpp
static void putI18nProperty(pokerduel::P_I18nPString& ret, const PMsgLocaleTable& table, const PIniFile::Section* section, const char* key, bool prohibitHtmlChars )	//	throws
{
	for(UINT32 locale = 0; locale < table.max; ++locale)
	{
		PString lkey = key;

		if (locale != LOCALE_DEFAULT) // keep existing keys as is
			lkey.append("_").append(table.locales[locale].fileName);

		// for update, the value could be an empty string
		// in this case, we need to remove
		const char* value = section->getProperty(lkey, NULL);
		if (value)
		{
			if ( prohibitHtmlChars )
			{
				//	don't allow '<' or '>'
				for ( const char* p = value; *p != '\0'; ++p )
					if ( *p == '<' || *p == '>' )
					{
						PString err( "HTML characters not allowed in " );
						err.append( key );
						throw PError( err );
					}
			}

			ret.appendPString(value, locale);
		}
	}
}

//	Copied from tourscript.cpp
static UINT32 _readVppMultiplierFromIniParam( const char* param, PString& err )
{
	const char* strBuff = PString::ltrim( param );
	char* dummy = 0;

	const char* terminator = strBuff + strlen( strBuff ) - 1;

	if(  *terminator != 'x' && *terminator != 'X'  ) 
	{
		err = " has wrong format - no 'x' at the end!";
		return 0;
	}

	if( *strBuff == '-' )// to exclude .-123
	{
		err = " must be positive!";
		return 0;
	}

	UINT32 intPart, fracPart=0;

	intPart = strtoul( strBuff, &dummy, 10 );
	//intPart = atoi( strBuff );

	if( intPart == 0 && *strBuff != '0' && *strBuff != '.' && strBuff != terminator ) 
	{
		err = " has an invalid symbol!";
		return 0;
	}

	if( intPart > VppMultiplier::maxVppMultiplier )
	{
		err = " must not exceed ";
		err.appendUint( VppMultiplier::maxVppMultiplier ).append("x!");
		return 0;
	}

	if( *dummy == '.' )
	{
		strBuff = dummy + 1;
		if( !isdigit( *strBuff ) && strBuff != terminator )// to exclude .-123
		{
			err = " has an invalid symbol after the dot!";
			return 0;
		}

		fracPart = strtoul( strBuff, &dummy, 10 );
		if( dummy != terminator )// to exclude .1 23x
		{
			err = " has an invalid symbol after the dot!";
			return 0;
		}

		if( dummy - strBuff > 2 )
		{
			err = " has more than 2 digits after the dot!" ;
			return 0;
		}
		else if ( dummy - strBuff == 1 )
			fracPart *=10;

	}
	else if( dummy != terminator )
	{
		err = " has invalid symbol!";
		return 0;
	}

	return intPart*100 + fracPart;
}

static void validateScript( const PIniFile::Section* section )	//	throws
{
	char err[ 256 + TOURN_PROPERTY_LEN ];

	vector<PString> identifiers;
	
	identifiers.push_back( _Site );
	identifiers.push_back( _HandsPerPlayer );
	identifiers.push_back( _SmallBlind );
	identifiers.push_back( _BigBlind );
	identifiers.push_back( _InitialChips );
	//identifiers.push_back( _TimeToActMinutes );
	identifiers.push_back( _TimeBankMinutes );
	identifiers.push_back( _TimeBankIncrement );
	identifiers.push_back( _BuyIn );
	identifiers.push_back( _Rake );
	identifiers.push_back( _PlayMoney );
	identifiers.push_back( _Currency );
	identifiers.push_back( _Game );
	identifiers.push_back( _Structure );
	identifiers.push_back( _DuelName );
	identifiers.push_back( _SpeedDisplay );
	identifiers.push_back( _TicketOption );
	identifiers.push_back( _AdmissionTicket );
	identifiers.push_back( _TicketOption );
	identifiers.push_back( _MatchType );
	identifiers.push_back( _TimeoutTimeBankIncrement );
	identifiers.push_back( _RandomMatchTimeout );
	identifiers.push_back( _PendingChallengeTimeout );

	//i18n
	vector<PString> i18n_identifiers;
	i18n_identifiers.push_back(_DuelName);

	const int i18n_size = i18n_identifiers.size();

	for(int i_i18n_id = 0; i_i18n_id < i18n_size; ++i_i18n_id)
	{
		for(int i_locale = 0; i_locale < LocalesTable.max; ++i_locale)
		{
			if (i_locale == LOCALE_DEFAULT) // keep existing keys as is
				continue;

			PString lkey = i18n_identifiers[i_i18n_id];
			lkey.append("_").append(LocalesTable.locales[i_locale].fileName);
			identifiers.push_back(lkey);
		}
	}

	vector<bool> id_p(identifiers.size(), false);

	for ( int i = 0; i < section->items.size(); ++i )
	{
		const PIniFile::Item& item = section->items[ i ];

		int j;
		for ( j = identifiers.size() - 1; j >= 0 ; j-- )
		{
			if ( item.name.equalsIgnoreCase( identifiers[ j ] ) )
			{
				if ( id_p[ j ] )
				{
					sprintf( err, "Error in script, identifier '%s' in section [startup] found more than once!", identifiers[ j ].c_str() );
					throw PError( err );
				}
				else
					id_p[ j ] = true;

				break;
			}
		}

		if ( j < 0 )
		{
			//bypass for multipliers PYR-21113
			if( !( PString::compareIgnoreCase( item.name, _VPP_Multiplier_, strlen( _VPP_Multiplier_ ) ) ) ) //PYR-25455: startsWith()
			{
				LicenceId licenceId = eLicenceDefault;
				if( !PLicenseMask::licenseNameToId( item.name + strlen( _VPP_Multiplier_ ), licenceId ) )
				{
					sprintf( err, "Error in tourn_Script in section [startup]: '%s' - invalid license!", item.name.c_str() );
					throw PError( err );
				}
				continue;
			}

			sprintf( err, "Error in script in section [startup]: Non-valid identifier found '%s'!", item.name.c_str() );
			throw PError( err );
		}
	}
}

#ifdef _refactor_this
const char *DuelScript::getProperty( UINT32 id ) const
{
	for( UINT32 i = 0; i < properties.size(); ++i ) 
	{
		if( properties[i].id == id )
			return properties[i].value;
	}
	return 0;
}
void DuelScript::setProperty( UINT32 id, const char *value )
{
	for( UINT32 i = 0; i < properties.size(); ++i ) 
	{
		if( properties[i].id == id )
		{
			properties[i].value.assign( value );
			return;
		}
	}
	properties.push_back( DuelTournProperty( id, value ) );
}

void DuelScript::writeVppMultipliers( const VppMultipliers& inputMap )
{
	PString propStr;
	inputMap.toString(propStr);

	if( propStr.length() )
		setProperty(PROP_DUELTYPE_VPP_MULTIPLIERS, propStr );

	PLog("writeVppMultipliers: %s", propStr.c_str());
}
#endif

static void setProperty( pokerduel::DuelType& script, UINT32 id, const char *value )
{
	for( UINT32 i = 0; i < script.properties.size(); ++i ) 
	{
		if( script.properties[i].id == id )
		{
			script.properties[i].value.assign( value );
			return;
		}
	}

	pokerduel::DuelTypeProperty prop;
	prop.id = id;
	prop.value = value;
	script.properties.push_back( prop );
}

static void writeVppMultipliers( pokerduel::DuelType& script, const VppMultipliers& inputMap )
{
	PString propStr;
	inputMap.toString(propStr);

	if( propStr.length() )
		setProperty(script, PROP_DUELTYPE_VPP_MULTIPLIERS, propStr );

	PLog("writeVppMultipliers: %s", propStr.c_str());
}


static void check( INT32 value, INT32 min, INT32 max, const char* errDesc )
{
	if ( value < min || value > max )
	{
		PString error( "!" );
		error.append( errDesc ).append( " invalid: " ).appendInt( value );
		throw( PError(error) );
	}
}

//	Copied from scripts.cpp
static void _checkOnlyOneBitSet( UINT32 flag )
{
	if( !flag ) return;
	flag &= flag-1; // clear the least significant bit
	if( !flag ) return;

	throw PError( "Segregated site cannot be combined with other sites" );
}

static void loadScript( const PIniFile& iniFile, pokerduel::DuelType& script )		//	throws
{
	const PIniFile::Section* sec = iniFile.getSection( "startup" );
	if ( !sec )
	{
		throw( PError( "No [startup] section found in script!" ) );
	}

	script.duelTypeId = 0;
	script.status = DuelTypeStatus_Active;

	validateScript( sec );

	script.numHands = sec->getIntProperty( _HandsPerPlayer, 0 );
	check( script.numHands, 1, 100, _HandsPerPlayer );

	//	Hands must be even
	if ( script.numHands & 0x0001 ) 
	{
		PString error( "!" );
		error.append( _HandsPerPlayer ).append( " must be even: " ).appendInt( script.numHands );
		throw( PError(error) );
	}

	script.smallBlind = sec->getIntProperty( _SmallBlind, 0 );
	check( script.smallBlind, 1, INT_MAX, _SmallBlind );

	script.bigBlind = sec->getIntProperty( _BigBlind, 0 );
	check( script.bigBlind, 1, INT_MAX, _BigBlind );

	script.startingChips = sec->getIntProperty( _InitialChips, 0 );

	const INT32 minStartingChips =  script.bigBlind * 10;
	if ( script.startingChips < minStartingChips )
	{
		PString error( "!Initial chips must be at least 10 x BB" );
		throw( PError( error ) );
	}
	
	//script.timeToAct = sec->getIntProperty( _TimeToActMinutes, 0 );
	//check( script.timeToAct, 1, INT_MAX, _TimeToActMinutes );
	
	script.timerBank = sec->getIntProperty( _TimeBankMinutes, 0 );
	check( script.timerBank, 1, INT_MAX, _TimeBankMinutes );

	script.timeBankIncrement = sec->getIntProperty( _TimeBankIncrement, 0 );
	check( script.timeBankIncrement, 0, 1440, _TimeBankIncrement );

	script.timeoutTimeBankIncrement = sec->getIntProperty( _TimeoutTimeBankIncrement, 0 );
	check( script.timeoutTimeBankIncrement, 0, 1440, _TimeoutTimeBankIncrement );

	script.buyIn = getAmountProperty( sec, _BuyIn);
	script.rake = getAmountProperty( sec, _Rake );

	const char* isPm = sec->getProperty( _PlayMoney, "No" );
	if ( !PString::compareIgnoreCase( isPm, "yes", 3 ) )
		script.isPlayMoney = 1;
	else if ( !PString::compareIgnoreCase( isPm, "no", 2 ) )
		script.isPlayMoney = 0;
	else 
		throw PError( "Error in script in section [startup]: Non valid identifier found after 'PlayMoney='!" );

	script.currency = sec->getProperty( _Currency, "" );
	if ( script.currency.length() )
	{
		const int sz = sizeof( currencies ) / sizeof( currencies[ 0 ] );
		int i;
		for ( i = sz - 1; i >= 0; i-- )
		{
			if ( !PString::compareIgnoreCase( script.currency, currencies[ i ], 3 ) )
			{
				script.currency.assign( currencies[ i ] );
				break;
			}
		}

		if ( i < 0 )
		{
			throw PError( "Error in script in section [startup]: Non valid identifier found after 'Currency='! parameter" );
		}
	}

	//PYR-29907
	if ( !script.isPlayMoney && !*script.currency )	//RM with empty currency - set USD
	{
		script.currency.assign("USD");
	}

	//	Site mask
	script.siteMask = 0;
	const char* siteProperty = sec->getProperty( _Site, "" );
	if ( *siteProperty )
	{
		TScriptStringParser	parser( siteProperty );
		vector<PString> sites;
		while ( !parser.parseEnded() )
		{
			PString buff;
			sites.push_back( parser.parseNextValueAsString(buff) );
		}   

		for ( int siteIdx = 0; siteIdx < sites.size(); ++siteIdx )
		{
			for ( int i = NUM_SITES - 2; i >= 0; i-- )
			{
				int len = strlen( Sites[ i ].name );
				if ( !(PString::compareIgnoreCase( sites[siteIdx], Sites[ i ].name, len ) ) )
				{
					script.siteMask |= Sites[ i ].code;
				}
			}
		}
	}

	//	PYR-37104. Do not allow a real money .NET site.
	if ( !script.isPlayMoney && ( script.siteMask & PokerStars_PM_only ) != 0 )
	{
		throw PError( "Real money .NET site not allowed." );
	}

	//	AQA-862. Segregated sites cannot be combined with other sites. (Copied from scripts.cpp)
	if ( !script.isPlayMoney )
	{
		if ( script.siteMask & (PokerStars_SharedLiquidity|PokerStars_PM_only) )
		{
			if ( script.siteMask & PokerStars_Segregated )
			{
				throw PError( "Segregated site cannot be combined with other sites" );
			}
		}
		else
		{
			// only one segregated site can be set
			_checkOnlyOneBitSet( script.siteMask );
		}
	}
	
	if ( script.siteMask == 0 )	//	AQA-830
	{
		script.siteMask = PokerStars_SharedLiquidity;
	}

	//	Game
	const char* gameString = sec->getProperty( _Game, "Holdem" );
	if ( PString::compareIgnoreCase( gameString, "Holdem") )
	{
		PString err( "Invalid game type: ");
		err.append( gameString );
		throw PError( err );
	}
	script.game = HandTypeHoldem;

	//	Structure
	const char* structureString = sec->getProperty( _Structure, "NoLimit" );
	if ( PString::compareIgnoreCase( structureString, "NoLimit"))
	{
		PString err( "Invalid structure: ");
		err.append( structureString );
		throw PError( err );
	}
	script.structure = StructNoLimit;

	putI18nProperty(script.name, LocalesTable, sec, _DuelName, true /* prohibitHtmlChars */ );

	//	Speed display
	const char* speedDisplayString = sec->getProperty( _SpeedDisplay, "" );
	if ( speedDisplayString && *speedDisplayString )
	{
		if( !PString::compareIgnoreCase( speedDisplayString, "Regular", 7 ) )
		{
			script.speedDisplay = SpeedDisplayRegular;
		}
		else if( !PString::compareIgnoreCase( speedDisplayString, "Turbo", 5 ) )
		{
			script.speedDisplay = SpeedDisplayTurbo;
		}
		else
		{
			throw PError( "Invalid value for 'SpeedDisplay=', expecting one of the following values: 'Regular', 'Turbo'" );
		}
	}

	script.tournFlags = 0;
	const char* ticketOptionString = sec->getProperty( _TicketOption, "" );
	if ( ticketOptionString != NULL && *ticketOptionString )
	{
		if( !PString::compareIgnoreCase( ticketOptionString, "TicketOrBuyIn", 13 ) )
			script.tournFlags |= TOURSCRIPT_FLAG_USETICKETORBUYIN;
		//else if( !PString::compareIgnoreCase( ticketOptionString, "TicketAndBuyIn", 14 ) )
		//	script.tournFlags &= ~TOURSCRIPT_FLAG_USETICKETORBUYIN;
		else
			throw PError( "Error in tourn_Script in section [startup]: Non valid identifier found after 'TicketOption=' !" );
	}

	//	Admissions
	const char* admissionString = sec->getProperty( _AdmissionTicket, "" );
	if ( admissionString && *admissionString )
	{
		const char* str_p = PString::ltrim( admissionString );
		if( *str_p == '(' )
		{
			str_p = PString::ltrim( str_p + 1 );
			while( *str_p && *str_p != ')' )
			{
				admissionString = strchr( str_p, '"' );

				if( admissionString )
					admissionString++;
				else
					throw PError( "No double quotes after 'AdmissionTicket=' and left bracket found!" );

				str_p = strchr( admissionString, '"' );
				if( str_p )
					script.admissions.insert( script.admissions.end(), PString( admissionString, str_p - admissionString ) );
				else
					throw PError( "No second double quotes after 'AdmissionTicket=(' found!" );

				str_p = PString::ltrim( str_p + 1 );

				if( *str_p == ',' ) 
				{
					str_p = PString::ltrim( str_p + 1 );
					continue;
				}
				else if ( *str_p != ')' )
					throw PError( "No comma or right bracket after 'AdmissionTicket=(' found !" );
			}
		}
		else			
		{// only single admission
			script.admissions.insert( script.admissions.end(), str_p );
		}
	}

	//	AQA-1816
	if ( ( script.tournFlags & TOURSCRIPT_FLAG_USETICKETORBUYIN ) != 0 )
	{
		if ( !( script.admissions.size() ) )
			throw PError( "TicketOrBuyIn option specified without 'AdmissionTicket=' parameter!" );
	}

	//	
	const char* matchTypeString = sec->getProperty( _MatchType, "" );
	if ( *matchTypeString )
	{
		script.matchType = 0;

		if ( !PString::compareIgnoreCase( matchTypeString, "Random") )
		{
			script.matchType = DuelMatchType_Random;
		}
		else if ( !PString::compareIgnoreCase( matchTypeString, "Challenge"))
		{
			script.matchType = DuelMatchType_FriendChallenge;
		}
		else
		{
			if ( !PString::compareIgnoreCase( matchTypeString, "Challenge,Random") ||
				 !PString::compareIgnoreCase( matchTypeString, "Random,Challenge" ) )
			{
				script.matchType = DuelMatchType_All;
			}
			else
			{
				PString err( _MatchType );
				err.append( ": Invalid format" );
				throw PError( err );
			}
		}
	}

	//	VPP (Copied from scripts.cpp. TODO: refactor into a procedure, and call this proc from here and from scripts.cpp)
	const UINT32 vppSubstrLength = strlen(_VPP_Multiplier_);
	VppMultipliers multipliersMap;

	for( int i=0; i < sec->items.size(); ++i  )
	{
		const PIniFile::Item& item = sec->items[i];

		if ( !( PString::compareIgnoreCase( item.name, _VPP_Multiplier_, strlen( _VPP_Multiplier_ ) ) ) )
		{
			PString errMsg;

			UINT32 value = _readVppMultiplierFromIniParam( item.value, errMsg );
			if( errMsg.length() )
			{
				PString err( "Error in tourn_Script in section [startup]: parameter '" );
				err.append( item.name ).append( "'" ).append( errMsg );
				throw PError( err );
			}
			PString license =  item.name.c_str() + vppSubstrLength;
			LicenceId licenseId = eLicenceDefault;
			if ( !PLicenseMask::licenseNameToId( license, licenseId ) ) //invalid
			{
				PString err( "Error in tourn_Script in section [startup]: parameter '" );
				err.append( item.name ).append( "' invalid license" );
				throw PError( err );
			}
			multipliersMap.insert( make_pair( licenseId, VppMultiplier( value, VppMultiplier::local ) ) );
		} //starts with 
	} //for items
	if ( !multipliersMap.empty() )
	{
		writeVppMultipliers( script, multipliersMap );
	}

	//	PYR-37120
	script.randomMatchTimeout = sec->getIntProperty( _RandomMatchTimeout, 60 );
	check( script.randomMatchTimeout, 1, 10080, _RandomMatchTimeout );			//	10080 == minutes in a week
	
	script.pendingChallengeTimeout = sec->getIntProperty( _PendingChallengeTimeout, 2 * MINUTES_IN_A_DAY );
	check( script.pendingChallengeTimeout, 1, 10080, _PendingChallengeTimeout );	//	10080 == minutes in a week

	SrvTime now;
	now.currentLocalTime();
	const PIniFile::Section* secPokerDuel = iniFile.getSection( "_PokerDuel" );
	if ( secPokerDuel )
	{

		const char* t = secPokerDuel->getProperty( "fromDate" );
		if( t )
		{
			if( ! strToSrvTime( t, script.fromDate ) )
				throw PError( "Wrong fromData format" );
		}

		t = secPokerDuel->getProperty( "toDate" );
		if( t )
		{
			if( ! strToSrvTime( t, script.toDate ) )
				throw PError( "Wrong toDate format" );
		}

		if( script.toDate.compareTo( script.fromDate ) <= 0 )
		{
			throw PError( "toDate must be later than fromDate" );
		}

		if( script.toDate.compareTo( now ) <= 0 )
		{
			throw PError( "toDate must be later than now" );
		}
	}
}

void parseDuelScript ( const PIniFile& iniFile, vector<PString>& errors, CommMsgBody& dataObjectsBodies )
{
	errors.clear();

	const PIniFile::Section* sec = iniFile.getSection( "PokerDuels" );
	if ( !sec )
	{
		errors.push_back( "Invalid/missing [PokerDuels] section in uploaded file." );
		return;
	}
	
	vector< PIniFile > iniFiles;
	const int numItems = sec->items.size();
	for ( int i = 0; i < numItems ; ++i )
	{
		if ( !sec->items[ i ].name.equals( "PokerDuel" ) )
			continue;

		const PString& tournName = sec->items[ i ].value;
		PIniFile& out = *iniFiles.insert( iniFiles.end(), PIniFile() );
		PLog( "Tournament '%s' -> script %d", tournName.c_str(), iniFiles.size() - 1 );
		const int m = iniFile.sections.size();
		PString tournNameSlash = tournName;
		tournNameSlash.append( '/' );
		for( int j=0; j < m ; ++j )
		{
			if( iniFile.sections[ j ].name.startsWith( tournNameSlash ) )
			{
				const char* secName = iniFile.sections[ j ].name + strlen( tournNameSlash );

				PIniFile::Section* outSec = out.addSection( secName );
				outSec->items = iniFile.sections[ j ].items;
			}
		}
	}

	if ( iniFiles.size() == 0 )
	{
		PLog( "No scripts found" );
		return;
	}

	//	4<M(script)> 

	int numValidScripts = 0;
	size_t shift;
	dataObjectsBodies.composeVectorSize( 0, shift );
	for ( int i = 0; i < iniFiles.size(); ++i )
	{
		try
		{
			pokerduel::DuelType script;
			loadScript( iniFiles[i], script );
			script.composeMsg( dataObjectsBodies );

			++numValidScripts;
		}
		catch ( PError e)
		{
			PString errMsg( "Script ");
			errMsg.appendInt( i ).append( " err: ").append( e.why() );
			errors.push_back( errMsg );
		}
	}

	dataObjectsBodies.updateVectorSize( numValidScripts, shift );
}
