#include "vppmultipliers.h"

/*static*/ bool VppMultipliers::isValidCurrency( const char* name )
{
	if( !name || !*name )
		return false;

	UINT32 currenciesSize = sizeof( pokerCurrencies ) / sizeof( pokerCurrencies[0] );

	for( int i = 0; i < currenciesSize; ++i )
	{
		if( !PString::compareIgnoreCase( name, pokerCurrencies[i] ) )
			return true;
	}
	return false;
}

const char* VppMultipliers::toString(PString& ret) const
{
	for( const_iterator it = begin(); it != end(); ++it )
	{
		if( !LicenseRules::isValidLicense(it->first) )
		{
			PLog("ERROR: invalid license: %u", it->first);
			continue;
		}
		PString tmp = LicenseRules::getLicenseIdCode(it->first);
		if( it->second.value > VppMultiplier::maxVppMultiplier * 100 )
		{
			PLog( "ERROR: multiplier out of range %d for %s", it->second.value, tmp.c_str() );
			continue;
		}
		tmp.append('-').appendInt(it->second.value).append('-').appendUint(it->second.type);
		if( ret.length() )
		{
			ret.append(';');
		}

		ret.append(tmp);
	}
	return ret;
}

void VppMultipliers::fromString(const char* initStr)
{
	if(!initStr || !*initStr)
		return;

	//reading property string to map "license1-value1-type1;license2-value2-type2:..."
	UINT32 propLength = strlen(initStr);
	const char* licenseStart = initStr;
	const char* valueStart = 0;
	const char* typeStart = 0;
	PString license;
	INT32 value = 0;
	UINT32 type = (UINT32) VppMultiplier::undefined;

	for( int i = 0; i < propLength + 1; ++i )// include null terminator!
	{
		if( *( initStr + i ) == '-' )
		{
			if( !valueStart ) //first dash
			{
				license.assign( licenseStart, initStr + i );
				valueStart = initStr + i + 1;
			}
			else if( !typeStart ) //second dash - type
			{
				value = atoi( valueStart );
				typeStart =  initStr + i + 1;
			}
		}
		else if( *( initStr + i ) == ';' || *( initStr + i ) == 0)
		{
			licenseStart = initStr + i + 1;
			if( typeStart )
			{
				type = atoi( typeStart );
			}

			if( license.length() )
			{
				LicenceIdFlags licenseId; // PYR-109216
				if( LicenseRules::licenseNameToId( license, licenseId ) )
				{
					insert( make_pair( licenseId, VppMultiplier( value, ( VppMultiplier::Type )type ) ) );
				}
				else
				{
					PLog( "VPPM:Wrong licence: '%s'", license.c_str() );
				}
			}

			if( *( initStr + i ) == 0 ) //note that we go to null-terminator
			{
				break;
			}
			else
			{
				license = "";
				value = 0;
				type = VppMultiplier::undefined;
				typeStart = 0;
				valueStart = 0;
			}
		}
	}
}

INT32 VppMultipliers::getMultiplierX100(UINT32 licenseId) const
{
	const_iterator it = find( (LicenceIdFlags)licenseId );
	return ( ( it == end() ) ? 0 : it->second.value );	//x100 value
}

INT32 VppMultipliers::getMaxMultiplierX100() const
{
	INT32 max = 0;

	for( const_iterator it = begin(); it != end(); ++it )
	{
		if( it->second.value > max )
		{
			max = it->second.value;
		}
	}
	
	return max;
}

/*static*/ void VppMultipliers::parseSection( const IniDataFromDbm::Section* section, PStringMap<UINT32>& licenseCurrencyPairToMultiplierMap )
{
	// section format:
	// Name: license_currency Value: <multiplier>x
	// examples com_usd 18x / DK_USD 7.32x / PT_eur 10x
	if( section )
	{
		PLog( "section [default_global_vpp_multipliers]: %zu items", section->items.size() );
		PString err;
		for( int i = 0; i < section->items.size(); ++i )
		{
			if( !isValidLicenseCurrencyPair( section->items[i].name, err ) )
			{
				PLog( "Error when parsing ini data for lobby, section[vppMultipliers]: wrong parameter name encountered '%s'",
					section->items[i].name.c_str() );
				continue;
			}

			UINT32 value = readVppMultiplierFromIniParam( section->items[i].value, err );
			PString name = section->items[i].name;
			if( err.length() )
			{
				value = 0;
				PLog( "Error when parsing ini data for lobby, section[vppMultipliers], parameter %s: %s. Default 0 value will be used."
					, name.c_str(), err.c_str() );
				err = "";
			}
			name.toUpper();
			licenseCurrencyPairToMultiplierMap.insert( make_pair( name, value ) ); //probably should check if unique
			PLog( "%s : %u", name.c_str(), value );
		}
	}
}

void VppMultipliers::fillFromIniSection( const IniDataFromDbm::Section* section, const char* currencyToFill )
{
	clear();

	PStringMap<UINT32> licenseCurrencyPairToMultiplierMap;
	parseSection( section, licenseCurrencyPairToMultiplierMap );

	PString license, currency;

	for( PStringMap<UINT32>::iterator it = licenseCurrencyPairToMultiplierMap.begin(); it != licenseCurrencyPairToMultiplierMap.end(); ++it )
	{
		if( !(it->first) ) //just in case
			continue;

		const char* del = strchr( it->first, '_' );
		if( !del )
		{
			PLog( "fillFromIniSection error: wrong format: %s", it->first );
			continue;
		}

		currency.assign( del + 1 );
		if( !isValidCurrency( currency ) )
		{
			PLog( "fillFromIniSection error: invalid currency in lobby list: %s", currency.c_str() );
			continue;
		}

		//skip if not the currency we are looking for
		if( !currency.equalsIgnoreCase( currencyToFill ) )
			continue;

		license.assign( it->first, del - it->first );

		LicenceId licenseId; // PYR-109216
		if( !LicenseRules::licenseNameToId( license, licenseId ) )
			continue;

		if( it->second > VppMultiplier::maxVppMultiplier * 100 )
		{
			PLog( "fillFromIniSection error: invalid license: %s", license.c_str() );
			continue;
		}

		insert( make_pair( licenseId, VppMultiplier( it->second, VppMultiplier::global ) ) );
	}
}

//PYR-21113
//validates the parameter name used in dbm ini data, ex. "COM-USD"
/*static*/ bool VppMultipliers::isValidLicenseCurrencyPair( const char* name, PString& err )
{
	if( !name || !*name )
	{
		err = "Empty parameter name";
		return false;
	}

	const char* del = strchr( name, '_' );

	if( !del || !(del == name + 2 || del == name + 3 || del == name + 4) )
	{
		err = "Invalid parameter name format, should be LICENSE_CURRENCY (e.g. 'COM_USD'), uppercase please!";
		return false;
	}

	PString license;
	license.assign( name, del - name );

	LicenceIdFlags dummy;
	if( !LicenseRules::licenseNameToId( license, dummy ) )
	{
		err = "Invalid licence part of parameter name";
		return false;
	}

	if( !isValidCurrency( del + 1 ) )
	{
		err = "Invalid currency part of parameter name";
		return false;
	}
	err = "";
	return true;
}

//PYR-21113
//reads VPP_Multiplier_XXX parameter value, validates and return multiplier value in percents
//used in lobby, dbm and tourscript
/*static*/ UINT32 VppMultipliers::readVppMultiplierFromIniParam( const char* param, PString& err )
{
	if( param == nullptr )
	{
		err = " is null!";
		return 0;
	}

	const char* strBuff = PString::ltrim( param );
	char* dummy = 0;

	if( strlen( strBuff ) == 0 ) //-V805
	{
		err = " has wrong format - is length 0!";
		return 0;
	}

	const char* terminator = strBuff + strlen( strBuff ) - 1;

	if( *terminator != 'x' && *terminator != 'X' )
	{
		err = " has wrong format - no 'x' at the end!";
		return 0;
	}

	if( *strBuff == '-' )// to exclude .-123
	{
		err = " must be positive!";
		return 0;
	}

	UINT32 intPart, fracPart = 0;

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
		err.appendUint( VppMultiplier::maxVppMultiplier ).append( "x!" );
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
			err = " has more than 2 digits after the dot!";
			return 0;
		}
		else if( dummy - strBuff == 1 )
			fracPart *= 10;

	}
	else if( dummy != terminator )
	{
		err = " has invalid symbol!";
		return 0;
	}

	return intPart * 100 + fracPart;
}