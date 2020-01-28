#include "visibility.h"
#include "licenserules.h"
#include "brands.h"
#include "sitemasks.h"

// PYR-32027  Client Visibility parsing moved from lobbydata.h 

void ClientFeaturesVisibility::init( const IniDataFromDbm& iniData )
{
	siteFeaturesVisibilities.clear();
	const IniDataFromDbm::Section *section = iniData.getSection("client_features_visibility");
	if ( !section )
	{
		PLog("client_features_visibility not found in INIDATA");
		return;
	}

	vector<INISiteData> iniSiteData;
	readINIData(section, iniSiteData);

	for (int i=0; i<iniSiteData.size(); ++i)
	{
		if (iniSiteData[i].platformId == 0 && iniSiteData[i].brandId == BrandType_None)
		{
			SiteFeaturesVisibility siteFeatures;

			siteFeatures.brandId = iniSiteData[i].brandId;
			siteFeatures.siteId = iniSiteData[i].siteId;
			//	backward compatibility: for "old" clients the enum ClientFeatures is currently less than 32 bits so the cast here is ok
			siteFeatures.featuresMask = static_cast<UINT32>(iniSiteData[i].featuresMask.flags[0]);
			siteFeatures.originalINIDataFeaturesMask = siteFeatures.featuresMask;

			siteFeaturesVisibilities.push_back(siteFeatures);
			PLog("CLIF VIS site %u=%u", siteFeatures.siteId, siteFeatures.featuresMask);
		}
	}
}

void ClientFeaturesVisibility::compose( CommMsgBody& msg ) const
{
	size_t shift;
	UINT32 count =  0;
	msg.composeVectorSize(count, shift);

	for( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		if( BrandType_None == siteFeaturesVisibilities[i].brandId ) // PYR-38530 
		{
			++count;

			msg.
				composeUINT32(siteFeaturesVisibilities[i].siteId).
				composeUINT32(siteFeaturesVisibilities[i].featuresMask);
		}
	}
	msg.updateVectorSize(count, shift);
}

bool ClientFeaturesVisibility::setFeatureForSite( UINT32 siteId, UINT32 feature, bool enabled )
{
	// feature should only be a single bit
	bool ok = false;
	for( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		if( siteFeaturesVisibilities[i].siteId == siteId )
		{
			if( enabled )
			{
				if ( siteFeaturesVisibilities[i].originalINIDataFeaturesMask & feature )	//	Turn on only if it was originally on
				{
					siteFeaturesVisibilities[i].featuresMask |= feature;
				}
			}
			else
			{
				siteFeaturesVisibilities[i].featuresMask &= ~feature;
			}
			ok = true;
		}
	}
	return ok;
}

bool ClientFeaturesVisibilityBase::isNumericValue(const char* str) 
{
	for (const char* p=str; *p; ++p)
		if (*p < '0' || *p > '9')
			return false;
	return true;
}

void ClientFeaturesVisibilityBase::setVisibilityBit(const char* input, ExtendedFlag& mask) 
{
	if (isNumericValue(input))
	{
		mask.flags[0] |= p_atou64( input ); // backward computability
	}
	else
	{
		ClientVisibilityRules::setClientVisibilityByName(input, mask);
	}
}

//	Convert a string to a UINT64 bitmask, I.e. "1,2" -> 3
void ClientFeaturesVisibilityBase::convertStringToBitmask(const PString& flags, ExtendedFlag& maskOn, ExtendedFlag& maskOff) 
{
	vector<PString> vFlags;

	strTokenize(flags, vFlags, ",");
	for( UINT32 i = 0; i < vFlags.size(); ++i )
	{
		const char* flagValue = vFlags[i].c_str();

		if( !flagValue || !*flagValue)
		{
			PLog( "Error in INIDATA/client_features_visibility: invalid flag '%s'", vFlags[i].c_str() );
			continue;
		}
				
		switch(*flagValue)
		{
		case '+':
			setVisibilityBit(flagValue+1, maskOn);
			break;
		case '-':
			setVisibilityBit(flagValue+1, maskOff);
			break;
		default:
			setVisibilityBit(flagValue, maskOn);
			break;
		}
	}
}

bool ClientFeaturesVisibilityBase::analyseSiteName(	const PString& siteEx,
												   UINT32& siteId,
												   UINT32& platformId,
												   UINT32& brandId
												   ) 
{
	//	siteEx format: 	 siteID.ClientPlatform.brandId, Example: "COM", "COM.WIN", COM.WIN.FT"
	//
	vector<PString> siteExTokens;
	strTokenize(siteEx, siteExTokens, ".");

	const INT32 numTokens = siteExTokens.size();
	if (numTokens == 3) // COM.WIN.FT
	{
		siteId = SiteRules::getSiteIdWithNet(siteExTokens[0]);
		if (siteId == PokerStars_SiteUnknown)
		{
			PLog("Error in INIDATA/client_features_visibility: unrecognized site: '%s'", siteExTokens[0].c_str());
			return false;
		}

		siteExTokens[1].toUpper();
		platformId = PlatformRules::getPlatformId(siteExTokens[1]);
		if (platformId == Client_UnknownPlatform)
		{
			PLog("Error in INIDATA/client_features_visibility: unrecognized platform: '%s'", siteExTokens[1].c_str());
			return false;
		}

		// case insensitive
		brandId = getBrandId(siteExTokens[2]);

		if (brandId == BrandType_None)
		{
			PLog("Error in INIDATA/client_features_visibility: unrecognized brand: '%s'", siteExTokens[2].c_str());
			return false;
		}

		return true;
	}

	if (numTokens == 2) // COM.WIN
	{
		// case insensitive
		// can be BrandType_None
		siteId = SiteRules::getSiteIdWithNet(siteExTokens[0]);
		if (siteId == PokerStars_SiteUnknown)
		{
			PLog("Error in INIDATA/client_features_visibility: unrecognized site: '%s'", siteExTokens[0].c_str());
			return false;
		}

		siteExTokens[1].toUpper();
		platformId = PlatformRules::getPlatformId(siteExTokens[1]);
		if (platformId == Client_UnknownPlatform)
		{
			PLog("Error in INIDATA/client_features_visibility: unrecognized platform: '%s'", siteExTokens[1].c_str());
			return false;
		}

		brandId = BrandType_None;

		return true;
	}

	if (numTokens == 1) // only Site COM
	{
		siteId = SiteRules::getSiteIdWithNet(siteExTokens[0]);
		if (siteId == PokerStars_SiteUnknown)
		{
			PLog("Error in INIDATA/client_features_visibility: unrecognized site: '%s'", siteExTokens[0].c_str());
			return false;
		}

		platformId = Client_UnknownPlatform;
		brandId = BrandType_None;

		return true;
	}

	PLog("Error ini INIDATA/client_features_visibility: unrecognized site: '%s'", siteEx.c_str());
	return false;
}

void ClientFeaturesVisibilityBase::readINIData(const IniDataFromDbm::Section *section, vector<INISiteData>& iniSiteData) 
{
	const UINT32 numItems = section->items.size();
	UINT32 siteId;
	UINT32 platformId;
	UINT32 brandId;
	for ( UINT32 i = 0; i < numItems; ++i )
	{
		if (analyseSiteName(section->items[i].name, siteId, platformId, brandId))
		{
			INISiteData siteInfo;
			siteInfo.siteId = siteId;
			siteInfo.platformId = platformId;
			siteInfo.brandId = brandId;
			convertStringToBitmask(section->items[i].value, siteInfo.featuresMask, siteInfo.featuresMaskOff);

			iniSiteData.push_back(siteInfo);
		}
	}
}

//	PYR-27500

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientDeviceSpecificFeature::composeForCompatiblePublication(CommMsgBody& body) const
{
	body
		.composeUINT32(clientDevice)
		.composeUINT64(featuresMask.flags[0])
		;
}

// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientDeviceSpecificFeature::compose(CommMsgBody& body) const
{
	body.composeUINT32(clientDevice);
	featuresMask.compose(body);
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientTypeSpecificFeature::composeForCompatiblePublication(CommMsgBody& body) const
{
	body
		.composeUINT32(clientType)
		.composeUINT64(featuresMask.flags[0])
		;

	body.composeUINT32(clientDeviceSpecificFeatures.size());

	for (int i=0; i<clientDeviceSpecificFeatures.size(); ++i)
	{
		CommMsgBody subBody;
		clientDeviceSpecificFeatures[i].composeForCompatiblePublication(subBody);
		body.composeMsgBody(subBody);
	}
}

// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientTypeSpecificFeature::compose(CommMsgBody& body) const
{
	body.composeUINT32(clientType);

	featuresMask.compose(body);

	body.composeUINT32(clientDeviceSpecificFeatures.size());

	for (int i=0; i<clientDeviceSpecificFeatures.size(); ++i)
	{
		CommMsgBody subBody;
		clientDeviceSpecificFeatures[i].compose(subBody);
		body.composeMsgBody(subBody);
	}
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientPlatformSpecificFeature::composeForCompatiblePublication(CommMsgBody& body) const
{
	body
		.composeUINT32(clientPlatform)
		.composeUINT64(featuresMask.flags[0])
		;

	body.composeUINT32(clientTypeSpecificFeatures.size());

	for (int i=0; i<clientTypeSpecificFeatures.size(); ++i)
	{
		CommMsgBody subBody;
		clientTypeSpecificFeatures[i].composeForCompatiblePublication(subBody);
		body.composeMsgBody(subBody);
	}
}

// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientPlatformSpecificFeature::compose(CommMsgBody& body) const
{
	body.composeUINT32(clientPlatform);
	featuresMask.compose(body);

	body.composeUINT32(clientTypeSpecificFeatures.size());

	for (int i=0; i<clientTypeSpecificFeatures.size(); ++i)
	{
		CommMsgBody subBody;
		clientTypeSpecificFeatures[i].compose(subBody);
		body.composeMsgBody(subBody);
	}
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientPlatformSpecificFeature::setFeatureEx( UINT64 featureBit, INT32 index, bool enabled )
{
	ClientFeaturesVisibilityEx::setFeature(featuresMask.flags[index], featureBit, enabled);
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::ClientPlatformSpecificFeature::setFeature( UINT64 featureBit, bool enabled )
{
	ClientFeaturesVisibilityEx::setFeature(featuresMask.flags[0], originalINIDataFeaturesMask, featureBit, enabled);
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::composeForCompatiblePublication(CommMsgBody& body) const
{
	body
		.composeUINT32(siteId)
		.composeUINT64(clientFeaturesMask.flags[0])
		;

	body.composeUINT32(clientPlatformSpecificFeatures.size());

	for (int i=0; i<clientPlatformSpecificFeatures.size(); ++i)
	{
		CommMsgBody platformBody;
		clientPlatformSpecificFeatures[i].composeForCompatiblePublication(platformBody);
		body.composeMsgBody(platformBody);
	}
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::compose(CommMsgBody& body) const
{
	body.composeUINT32(siteId);
	clientFeaturesMask.compose(body);

	body.composeUINT32(clientPlatformSpecificFeatures.size());

	for (int i=0; i<clientPlatformSpecificFeatures.size(); ++i)
	{
		CommMsgBody platformBody;
		clientPlatformSpecificFeatures[i].compose(platformBody);
		body.composeMsgBody(platformBody);
	}
}

// PYR-38530
// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::composeWithBrandId(CommMsgBody& body) const
{
	body
		.composeUINT32(brandId)
		.composeUINT32(siteId)
		;
	clientFeaturesMask.compose(body);

	body.composeUINT32(clientPlatformSpecificFeatures.size());

	for (int i=0; i<clientPlatformSpecificFeatures.size(); ++i)
	{
		CommMsgBody platformBody;
		clientPlatformSpecificFeatures[i].compose(platformBody);
		body.composeMsgBody(platformBody);
	}
}


void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::getDefaultPlatformFeaturesMaskForSite(const vector<INISiteData>& iniSiteData, ExtendedFlag& mask) const
{
	PASSERT5(siteId != 0);

	mask.clear();

	for (int i=0; i<iniSiteData.size(); ++i)
	{
		if (iniSiteData[i].brandId == BrandType_None && iniSiteData[i].siteId == siteId && iniSiteData[i].platformId == 0)
		{
			mask = iniSiteData[i].featuresMask;
		}
	}
}

INT32 ClientFeaturesVisibilityEx::SiteFeaturesVisibility::findPlatformFeaturesForBrand(UINT32 platformId) const
{
	for (INT32 i=0; i<clientPlatformSpecificFeatures.size(); ++i)
	{
		if(clientPlatformSpecificFeatures[i].clientPlatform == platformId)
		{
			return i;
		}
	}

	return -1;
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::initClientPlatformSpecificFeatures(const vector<INISiteData>& iniSiteData)
{
	PASSERT5(siteId != 0);

	for (int i=0; i<iniSiteData.size(); ++i)
	{
		if ( iniSiteData[i].brandId == BrandType_None && 
			 iniSiteData[i].siteId == siteId && 
			 iniSiteData[i].platformId != 0)
		{
			ClientPlatformSpecificFeature platform;
			platform.clientPlatform = iniSiteData[i].platformId;
			getDefaultPlatformFeaturesMaskForSite(iniSiteData, platform.featuresMask);
			platform.featuresMask.add(iniSiteData[i].featuresMask);
			platform.featuresMask.clear(iniSiteData[i].featuresMaskOff);
			platform.originalINIDataFeaturesMask = platform.featuresMask.flags[0];
			clientPlatformSpecificFeatures.push_back( platform );
		}
	}

	for (int i=0; i<iniSiteData.size(); ++i)
	{
		if (iniSiteData[i].brandId == brandId && 
			iniSiteData[i].siteId == siteId && 
			iniSiteData[i].platformId != 0)
		{
			INT32 ind = findPlatformFeaturesForBrand(iniSiteData[i].platformId); 
			if( ind < 0 )
			{
				ClientPlatformSpecificFeature newPlatform;
				newPlatform.clientPlatform = iniSiteData[i].platformId;
				getDefaultPlatformFeaturesMaskForSite(iniSiteData, newPlatform.featuresMask);

				clientPlatformSpecificFeatures.push_back( newPlatform );
				ind = clientPlatformSpecificFeatures.size() - 1;
			}

			ClientPlatformSpecificFeature& platform = clientPlatformSpecificFeatures[ind];
						
			platform.featuresMask.add(iniSiteData[i].featuresMask);
			platform.featuresMask.clear(iniSiteData[i].featuresMaskOff);
			platform.originalINIDataFeaturesMask = platform.featuresMask.flags[0];
		}
	}
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::setFeatureEx( UINT64 featureBit, INT32 index, bool enabled )
{
	ClientFeaturesVisibilityEx::setFeature(clientFeaturesMask.flags[index], featureBit, enabled);

	for (INT32 i = 0; i < clientPlatformSpecificFeatures.size(); ++i)
	{
		clientPlatformSpecificFeatures[i].setFeatureEx(featureBit, index, enabled);
	}
}

void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::setFeature( UINT64 featureBit, bool enabled )
{
	ClientFeaturesVisibilityEx::setFeature(clientFeaturesMask.flags[0], originalINIDataFeaturesMask, featureBit, enabled);

	for (INT32 i = 0; i < clientPlatformSpecificFeatures.size(); ++i)
	{
		clientPlatformSpecificFeatures[i].setFeature(featureBit, enabled);
	}
}

// if platformID is found in clientPlatformSpecificFeatures, set visibilities accordingly
// otherwise set the visibilities from the no-platform specified case
// visibilities is assumed to already contain the right number of flag elements
void ClientFeaturesVisibilityEx::SiteFeaturesVisibility::getVisibilitiesForPlatform( UINT32 platformId, vector<UINT64>& visibilities ) const //PYR-51551
{
	PASSERT( visibilities.size() == EXTENDED_FLAG_COUNT );
	const ExtendedFlag* extendedFlags;
	extendedFlags = &clientFeaturesMask;

	if( platformId != Client_UnknownPlatform )
	{
		INT32 ind = findPlatformFeaturesForBrand( platformId );
		if( ind >= 0 )
		{
			extendedFlags = &(clientPlatformSpecificFeatures[ind].featuresMask);
		}
	}

	for ( INT32 x = 0; x < EXTENDED_FLAG_COUNT; ++x )
	{
		visibilities[x] = extendedFlags->flags[x];
	}
}

bool ClientFeaturesVisibilityEx::setFeatureForSite( UINT32 siteId, UINT64 feature_, bool enabled )
{
	bool ok = false;
	for ( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		if ( siteFeaturesVisibilities[i].siteId == siteId )
		{
			siteFeaturesVisibilities[i].setFeature(feature_, enabled);
			ok = true;
		}
	}

	return ok;
}

void ClientFeaturesVisibilityEx::setFeatureForSiteCountry( UINT32 siteId, const vector<PString>& countries, UINT64 feature, INT32 index, bool enabled )
{

	if( countries.empty() )
	{
		// set site
		bool found = false;
		for ( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
		{
			SiteFeaturesVisibility&  siteFeatures = siteFeaturesVisibilities[i];
			if ( siteFeatures.siteId == siteId )
			{
				siteFeatures.setFeatureEx(feature, index, enabled);
				found = true;
				PLog( "set site %u.%u=%s", siteFeatures.siteId, siteFeatures.brandId, siteFeatures.clientFeaturesMask.toString().c_str() );
			}
		}

		if( !found )
		{
			UINT32 brandId = BrandType_PokerStars; 
			while(brandId <= BrandType_Last)
			{
				SiteFeaturesVisibility siteFeatures;

				siteFeatures.brandId = brandId;
				siteFeatures.siteId = siteId;

				siteFeatures.setFeatureEx(feature, index, enabled);

				siteFeaturesVisibilities.push_back(siteFeatures);
				brandId = (brandId << 1);

				PLog( "add site %u.%u=%s", siteFeatures.siteId, siteFeatures.brandId, siteFeatures.clientFeaturesMask.toString().c_str() );
			}
		}
	}
	else
	{
		// clear for site
		for ( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
		{
			SiteFeaturesVisibility&  siteFeatures = siteFeaturesVisibilities[i];
			if ( siteFeatures.siteId == siteId )
			{
				siteFeatures.setFeatureEx(feature, index, false/*enabled)*/);
				PLog( "clear site %u.%u=%s", siteFeatures.siteId, siteFeatures.brandId, siteFeatures.clientFeaturesMask.toString().c_str() );
			}
		}

		// and set country exception
		for( const auto& country : countries )
		{
			bool found = false;
			for( auto& ce : countryExceptions )
			{
				if( ce.country.equals(country) )
				{
					ce.setFeature(feature, index, enabled);
					found = true;
					PLog( "set ce '%s'.%u on=%s off=%s", country.c_str(), siteId, ce.featureMaskOn.toString().c_str(), ce.featureMaskOff.toString().c_str() );
				}
			}

			if( !found )
			{
				PSiteMask siteMask;
				SiteRules::maskFromSiteNumber(siteId, siteMask);
				CountryException newCe(country, siteMask, BrandType_ALLMask);
				newCe.setFeature(feature, index, enabled);
				countryExceptions.push_back(newCe);
				PLog( "add ce '%s'.%u on=%s off=%s", country.c_str(), siteId, newCe.featureMaskOn.toString().c_str(), newCe.featureMaskOff.toString().c_str() );
			}
		}
	}
}

//	PYR-29125
void ClientFeaturesVisibilityEx::CountryException::compose(CommMsgBody& body) const
{
	body.composeString(country);
	featureMaskOn.compose(body);
	featureMaskOff.compose(body);
	body.composeUINT32(siteVisibilityMask.getLower32());
	// PYR-99318 - Adding the PBitmask, since the caller will add the resulting body using composeMsgBody().
	siteVisibilityMask.compose(body);
}

void ClientFeaturesVisibilityEx::CountryException::composeForCompatiblePublication(CommMsgBody& body) const
{
	body
		.composeString(country)
		.composeUINT64(featureMaskOn.flags[0])
		.composeUINT64(featureMaskOff.flags[0])
		.composeUINT32(siteVisibilityMask.getLower32())
		;
	// PYR-99318 - Adding the PBitmask, since the caller will add the resulting body using composeMsgBody().
	siteVisibilityMask.compose(body);
}

// PYR-38530
// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ClientFeaturesVisibilityEx::CountryException::composeWithBrandId(CommMsgBody& body) const
{
	body.composeString(country);
	featureMaskOn.compose(body);
	featureMaskOff.compose(body);
	body
		.composeUINT32(brandIdMask)
		.composeUINT32(siteVisibilityMask.getLower32())
		;
	// PYR-99318 - Adding the PBitmask, since the caller will add the resulting body using composeMsgBody().
	siteVisibilityMask.compose(body);
}

//	- remove the leading '-' or '+' sign
//	- return true if the first char is '-'
bool ClientFeaturesVisibilityEx::isTokenNegative(PString& token) const
{
	if (token.length() == 0)
	{
		return false;
	}

	if (token[0] == '-')
	{
		PString newToken = token.c_str() + 1;
		token.assign(newToken);
		return true;
	}

	if (token[0] == '+')
	{
		PString newToken = token.c_str() + 1;
		token.assign(newToken);
	}

	return false;
}

//	Sample input: "1,+2,-4,8"
void ClientFeaturesVisibilityEx::analyseMaskString(const PString& flags, ExtendedFlag& onMask, ExtendedFlag& offMask) const
{
	onMask.clear();
	offMask.clear();

	vector<PString> vFlags;

	strTokenize(flags, vFlags, ",");
	for( UINT32 i = 0; i < vFlags.size(); ++i )
	{
		PString& token = vFlags[i];
		if (token.length() == 0)
		{
			continue;
		}

		const bool isNegative = isTokenNegative(token);
		if (isNegative)
		{
			setVisibilityBit( token.c_str(), offMask );
		}
		else
		{
			setVisibilityBit( token.c_str(), onMask );
		}
	}
}

//	A kind of strTokenize() that ignores delimiters within brackets.
void ClientFeaturesVisibilityEx::tokenizeCountries(const char* input, vector<PString>& countries) const
{
	//	Sample input: "CA(COM,NET),US(COM)"
	//	Out: [0]: CA(COM,NET)
	//		 [1]: US(COM)

	if (input == NULL)
		return;

	const char* start = input;
	bool insideBracket = false;
	while (*input != '\0')
	{
		if (*input == '(')
			insideBracket = true;
		else if (*input == ')')
			insideBracket = false;
		else if (*input == ',')
		{
			if (!insideBracket)
			{
				PString token(start, input);
				PString trimmedToken(PString::ltrim(token), PString::rtrim(token));
				if (trimmedToken.length() > 0)
				{
					trimmedToken.toUpper();
					countries.push_back(trimmedToken);
				}
				start = input + 1;
			}
		}
		++input;
	}

	PString token(start);
	PString trimmedToken(PString::ltrim(token), PString::rtrim(token));
	if (trimmedToken.length() > 0)
	{
		trimmedToken.toUpper();
		countries.push_back(trimmedToken);
	}
}

void ClientFeaturesVisibilityEx::getSiteVisibilityMask(PString& country, // in-out param
													   PSiteMask& siteVisibilityMask) const
{
	//	Example:
	//	input: country = "CA(NET,COM)"
	//	returns:	country = "CA"
	//				siteVisibilityMask = PokerStars_Net | PokerStars_Com

	siteVisibilityMask = SiteBitmask_ALLMask;	//	default is all sites
	const char* openBrace = country.find('(');
	if (openBrace == NULL )
	{
		return;
	}
	const char* closeBrace = country.find(')');
	if (closeBrace == NULL )
	{
		PLog("visibility_country_exceptions syntax error: %s", country.c_str());
		return;
	}
	PString sites(openBrace+1, closeBrace);
	if (sites.length() == 0)
	{
		PLog("visibility_country_exceptions syntax error: %s", country.c_str());
		return;
	}
	PSiteMask proposedSiteVisibilityMask;
	vector<PString> siteV;
	strTokenize(sites, siteV, ",");
	for (int i=0; i<siteV.size(); ++i)
	{
		PSiteMask mask;
		SiteRules::maskFromSiteNumber(SiteRules::getSiteIdWithNet(siteV[i]), mask);
		if (mask == PSiteMask::zero)
		{
			PLog("visibility_country_exceptions unknown site is ignored: %s", siteV[i].c_str());
		}
		proposedSiteVisibilityMask |= mask;
	}
	if (proposedSiteVisibilityMask != PSiteMask::zero)
	{
		siteVisibilityMask = proposedSiteVisibilityMask;
	}
	PString newCountry(country, openBrace);
	country.assign(newCountry);
}

void ClientFeaturesVisibilityEx::analyseCountryString(const PString& inputString,
	vector< pair<PString, CountryMask> >& countrySiteMask) const
{
	vector<PString> tokens;
	tokenizeCountries(inputString.c_str(), tokens);
	//	Sample input: "CA(COM,NET).FT,US(COM)"
	//	Out: [0]: CA(COM,NET).FT
	//		 [1]: US(COM)

	countrySiteMask.reserve(tokens.size());
	
	for (INT32 i=0; i<tokens.size(); ++i)
	{
		PSiteMask siteVisibilityMask;

		vector<PString> brandTokens;
		strTokenize(tokens[i], brandTokens, "."); // Format NL(COM).FT or NL(COM)
		UINT32 brandId = BrandType_None;
		if( 1 == brandTokens.size() )
		{
			// NL(COM) case
			brandId = BrandType_ALLMask; // Brand not specified - any brand
			PString& country = brandTokens[0];
			getSiteVisibilityMask(country, siteVisibilityMask);
			countrySiteMask.push_back( make_pair(country, CountryMask(siteVisibilityMask, brandId ) ) );
		}
		else if( 2 == brandTokens.size() )
		{
			// NL(COM).FT case
			brandId = getBrandId(brandTokens[1]);
			PString& country = brandTokens[0];
			getSiteVisibilityMask(country, siteVisibilityMask);
			countrySiteMask.push_back( make_pair(country, CountryMask(siteVisibilityMask, brandId ) ) );
		}
		else
		{
			PLog("visibility_country_exceptions country syntax error: %s", inputString.c_str());
		}
	}
}

void ClientFeaturesVisibilityEx::init( const IniDataFromDbm& iniData )
{
	siteFeaturesVisibilities.clear();
	countryExceptions.clear();

	const IniDataFromDbm::Section *clientFeaturesSection = iniData.getSection("client_features_visibility");
	if ( !clientFeaturesSection )
	{
		PLog("client_features_visibility not found in INIDATA");
		return;
	}

	vector<INISiteData> iniSiteData;
	readINIData(clientFeaturesSection, iniSiteData);

	for (int i=0; i<iniSiteData.size(); ++i)
	{
		// main branch
		if (iniSiteData[i].platformId == 0 && iniSiteData[i].brandId == BrandType_None)
		{
			UINT32 brandId = BrandType_PokerStars; 
			while(brandId <= BrandType_Last)
			{
				SiteFeaturesVisibility siteFeatures;

				siteFeatures.brandId = brandId;
				siteFeatures.siteId = iniSiteData[i].siteId;
				siteFeatures.clientFeaturesMask = iniSiteData[i].featuresMask;
				siteFeatures.originalINIDataFeaturesMask = siteFeatures.clientFeaturesMask.flags[0]; // compatibility
				siteFeatures.initClientPlatformSpecificFeatures(iniSiteData);

				siteFeaturesVisibilities.push_back(siteFeatures);
				brandId = (brandId << 1);

				PLog( "CLIF VISEx site %u.%u=%s", siteFeatures.siteId, siteFeatures.brandId, siteFeatures.clientFeaturesMask.toString().c_str() );
			}
		}
	}

	//	PYR-29125
	const IniDataFromDbm::Section *countrySection = iniData.getSection("visibility_country_exceptions");
	if ( !countrySection )
	{
		PLog("visibility_country_exceptions not found in INIDATA");
		return;
	}
	const UINT32 numItems = countrySection->items.size();
	countryExceptions.reserve(numItems);
	
	for ( UINT32 i = 0; i < numItems; ++i )
	{
		vector< pair<PString, CountryMask> > countrySiteMaskPairs;
		analyseCountryString(countrySection->items[i].name, countrySiteMaskPairs);

		for (int j = 0; j < countrySiteMaskPairs.size(); ++j)
		{
			CountryException country(countrySiteMaskPairs[j].first, countrySiteMaskPairs[j].second.siteMask, countrySiteMaskPairs[j].second.brandMask);
			analyseMaskString(countrySection->items[i].value, country.featureMaskOn, country.featureMaskOff);
			countryExceptions.push_back(country);
			PString dummy;
			PLog("VisCountryExc: %s=%s,%s,%s,%d", country.country.c_str(), 
				country.featureMaskOn.toString().c_str(), 
				country.featureMaskOff.toString().c_str(), 
				country.siteVisibilityMask.toDbString(dummy),
				country.brandIdMask);
		}
	}
}

//	PYR-27500
void ClientFeaturesVisibilityEx::composeForCompatiblePublication( CommMsgBody& msg ) const
{
	size_t shift;
	UINT32 count =  0;
	msg.composeVectorSize(count, shift);

	for( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		if( BrandType_PokerStars == siteFeaturesVisibilities[i].brandId ) // PYR-38530 
		{
			++count;

			CommMsgBody siteMsgBody;
			siteFeaturesVisibilities[i].composeForCompatiblePublication(siteMsgBody);
			msg.composeMsgBody(siteMsgBody);
		}
	}
	msg.updateVectorSize(count, shift);

	//	PYR-29125
	count =  0;
	msg.composeVectorSize(count, shift);
	for (UINT32 i = 0; i < countryExceptions.size(); ++i )
	{
		if( 0 != (BrandType_PokerStars & countryExceptions[i].brandIdMask)  ) // PYR-38530 
		{
			++count;
			CommMsgBody countryExcpetionMsgBody;
			countryExceptions[i].composeForCompatiblePublication( countryExcpetionMsgBody );
			msg.composeMsgBody( countryExcpetionMsgBody );
		}
	}
	msg.updateVectorSize(count, shift);
}

void ClientFeaturesVisibilityEx::composeWithoutBrand( CommMsgBody& msg ) const
{
	size_t shift;
	UINT32 count =  0;
	msg.composeVectorSize(count, shift);

	for( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		if( BrandType_PokerStars == siteFeaturesVisibilities[i].brandId ) // PYR-38530 
		{
			++count;

			CommMsgBody siteMsgBody;
			siteFeaturesVisibilities[i].compose(siteMsgBody);
			msg.composeMsgBody(siteMsgBody);
		}
	}
	msg.updateVectorSize(count, shift);

	//	PYR-29125
	count =  0;
	msg.composeVectorSize(count, shift);
	for (UINT32 i = 0; i < countryExceptions.size(); ++i )
	{
		if( 0 != (BrandType_PokerStars & countryExceptions[i].brandIdMask)  ) // PYR-38530 
		{
			++count;
			CommMsgBody countryExcpetionMsgBody;
			countryExceptions[i].compose( countryExcpetionMsgBody );
			msg.composeMsgBody( countryExcpetionMsgBody );
		}
	}
	msg.updateVectorSize(count, shift);
}

// PYR-38530
// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ClientFeaturesVisibilityEx::composeWithBrandId(CommMsgBody& msg) const
{
	msg.composeUINT32(siteFeaturesVisibilities.size());

	for( UINT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		CommMsgBody siteMsgBody;
		siteFeaturesVisibilities[i].composeWithBrandId(siteMsgBody);
		msg.composeMsgBody(siteMsgBody);
	}

	//	PYR-29125
	msg.composeUINT32(countryExceptions.size());
	for (UINT32 i = 0; i < countryExceptions.size(); ++i )
	{
		CommMsgBody countryExcpetionMsgBody;
		countryExceptions[i].composeWithBrandId( countryExcpetionMsgBody );
		msg.composeMsgBody( countryExcpetionMsgBody );
	}
}

void ClientFeaturesVisibilityEx::getVisibilities( UINT32 clientSiteId, const char* country, vector<UINT64>& visibilities, UINT32 brandId /*=BrandType_None*/, UINT32 platformId /*=Client_UnknownPlatform*/ ) const
{
	bool found = false;
	visibilities.resize( EXTENDED_FLAG_COUNT, 0 );
	for ( INT32 i = 0; i < siteFeaturesVisibilities.size(); ++i )
	{
		const SiteFeaturesVisibility& visibility = siteFeaturesVisibilities[i];
		if ( visibility.siteId == clientSiteId
			&& (brandId == BrandType_None || visibility.brandId == brandId) )	// if brandId is not specified, just match the first hit (maintains legacy behaviour)
		{
			visibility.getVisibilitiesForPlatform( platformId, visibilities );
			found = true;
			break;
		}
	}

	if ( !found )
	{
		PLog( "!getVisibilities: site %u (brand %u) not found", clientSiteId, brandId );
		return;
	}

	if ( !strlen( country ) )
	{
		return;
	}

	//	Apply country filter
	for ( INT32 i = 0; i < countryExceptions.size(); ++i )
	{
		const CountryException& ce = countryExceptions[i];
		PSiteMask clientSiteMask;
		SiteRules::maskFromSiteNumber(clientSiteId, clientSiteMask);
		if ( ( (ce.siteVisibilityMask & clientSiteMask) != PSiteMask::zero ) && 
				ce.country.equalsIgnoreCase( country ) )
		{
			for ( INT32 x = 0; x < EXTENDED_FLAG_COUNT; ++x )
			{
				visibilities[x] = ( visibilities[x] | ce.featureMaskOn.flags[x] ) & ~ce.featureMaskOff.flags[x];
			}
		}
	}
}

// PYR-52524 Do not change this function without updating GlobalPublisherData::composeClientVisibility() in GlobalPublisherData.cpp
void ExtendedFlag::compose(CommMsgBody& msg) const
{
	CommMsgBody subBody;

	for(INT32 n = 0; n < SZARR(flags); ++n )
	{
		subBody.composeUINT64( flags[n] );
	}

	msg.composeMsgBody(subBody);
}

void ExtendedFlag::composeForPublication( UINT64& featureMask, UINT64& featureMask2 ) const
{
	if( SZARR( flags ) == 2 )
	{
		featureMask	= flags[0];
		featureMask2 = flags[1];
	}
	else
	{
		PLog( LOG_TRACE_FAULTY_LOGIC " <ExtendedFlag::composeForPublication>: unexpected flags size" );
	}
}

ExtendedFlag::ExtendedFlag()
{
	clear();
}

void ExtendedFlag::setBit(INT32 index, UINT64 value)
{
	if( index >= SZARR(flags) )
	{
		PLog(LOG_TRACE_FAULTY_LOGIC " invalid visibility index: %d", index);
		return;
	}

	flags[index] |= value;
}

void ExtendedFlag::clearBit(INT32 index, UINT64 value)
{
	if( index >= SZARR(flags) )
	{
		PLog(LOG_TRACE_FAULTY_LOGIC " invalid visibility index: %d", index);
		return;
	}

	flags[index] &= ~value;
}

PString ExtendedFlag::toString() const
{
	PString rc;
	for(INT32 n = 0; n < SZARR(flags); ++n )
	{
		if( n )
			rc.append('-');

		rc.appendUint64( flags[n] );
	}

	return rc;
}

void ExtendedFlag::clear()
{
	for(INT32 n = 0; n < SZARR(flags); ++n )
	{
		flags[n] = 0;
	}
}

void ExtendedFlag::add(const ExtendedFlag& other)
{
	for(INT32 n = 0; n < SZARR(flags); ++n )
	{
		flags[n] |= other.flags[n];
	}
}

void ExtendedFlag::clear(const ExtendedFlag& other)
{
	for(INT32 n = 0; n < SZARR(flags); ++n )
	{
		flags[n] &= ~(other.flags[n]);
	}
}
