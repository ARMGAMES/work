
#ifndef licenseutils_h_included
#define licenseutils_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "vectorutils.h"
#include "licenseid.h"
#include "licenserules.h"
#include "licensemask.h"

class LicenseUtils
{
public:
	// deprecated by PYR-100636
	static UINT32 licenseListToMask( const char* licenseList ) // PYR-74558 (moved as is from dbmanager.ini)
	{
		UINT32 licenceMask = 0;
		vector<PString> vtmp;
		StringVectorConvert::toStringVector( licenseList, vtmp );

		for( int i = 0; i < vtmp.size(); ++i )
		{
			const char*  strLicenseId = vtmp[i];

			LicenceId licenseId;
			if( !LicenseRules::licenseNameToId(strLicenseId, licenseId) )
				continue;
			if( licenseId == eLicenceDefault )
				licenseId = _LicenceDefaultBit_ForMaskUseOnly;

			licenceMask |= licenseId;
			PLog( "'%s'->%u", strLicenseId, licenseId );
		}
		return licenceMask;
	}

	// deprecated by PYR-100636
	static void createLicenseListFromMask( UINT32 licenseMask, vector<UINT32>& licenseIds )
	{
		for( INT32 i = 0; i < 32; ++i )
		{
			UINT32 licenseId = 1 << i;
			
			if( ( licenseId & licenseMask ) != 0 )
			{
				if( licenseId == _LicenceDefaultBit_ForMaskUseOnly )
				{
					licenseIds.push_back( eLicenceDefault );
				}
				else if( LicenseRules::isValidLicense( licenseId ) )
				{
					licenseIds.push_back( licenseId );
				}
			}
			if( licenseId == eLicenceLast )
			{
				break;
			}
		}
	}

	[[deprecated("use PLicenseMask::fromLicenseListStr() instead")]]
	static void licenseListToMask( const char* licenseList, PBitmask& licenseMask ) // PYR-100636
	{
		PLicenseMask licMask;
		licenseListToMask(licenseList, licMask);
		licenseMask = std::move(licMask);
	}

	[[deprecated("use PLicenseMask::fromLicenseListStr() instead")]]
	static void licenseListToMask( const char* licenseList, PLicenseMask& licenseMask ) // PYR-100636
	{
		licenseMask = PBitmask::zero;
		vector<PString> vtmp;
		StringVectorConvert::toStringVector( licenseList, vtmp );

		for( int i = 0; i < vtmp.size(); ++i )
		{
			const char*  strLicenseId = vtmp[i];

			LicenceId licenseId = eLicenceDefault;
			if( !PLicenseMask::licenseNameToId( strLicenseId, licenseId ) )
			{
				PLog( "cannot process '%s' - unknown licence name", strLicenseId );
				continue;
			}
			if( !LicenseRules::addToMask(licenseId, licenseMask) )
			{
				PLog( "cannot process '%s'", strLicenseId );
				continue;
			}
			PLog( "'%s'->%u", strLicenseId, licenseId );
		}
	}
		
	// NOTE: the function adds to the vector eLicenceDefault = 0 value in case of the default license 
	static void createLicenseListFromMask( const PLicenseMask& licenseMask, vector<UINT32>& licenseIds ) // PYR-100636
	{
		PLicenseMask licenseIdMask = LicenceBitmask_Default;
		
		while( true )
		{
			if( (licenseIdMask & licenseMask) != PBitmask::zero )
			{
				// NOTE licenseId should not exceeds UINT32
				UINT32 licenseId;
				LicenseRules::licenseIdFromMask( licenseIdMask, licenseId );

				// NOTE eLicenceDefault = 0 is used for LicenceBitmask_Default
				if( LicenseRules::isValidLicense( licenseId ) )
				{
					licenseIds.push_back( licenseId );
				}
			}

			if( licenseIdMask == LicenceBitmask_Last )
			{
				break;
			}

			licenseIdMask <<= 1;
		}
	}
};

#endif // #define licenseutils_h_included
