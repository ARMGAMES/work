#include "basestructs.h"
#include "dbm.h"
#include "vectorutils.h"
#include "ppdir.h"

void UserLicenseCodes::init(const PIniFile::Section* section, UINT32 defaultCode, const char* propertyName)
{
	if(!section)
	{
		PLog("Warning: UserLicenseCodes::init IGNORED due to !section");
		return;
	}

	userLicenseCodes.clear();
	const char* userLicenseCodes_ = section->getProperty(propertyName, "");
	StringVectorConvert::toNumVector(userLicenseCodes_, userLicenseCodes);
	if(userLicenseCodes.size() == 0)
	{
		userLicenseCodes.push_back(defaultCode);
	}
	userLicenseCodes.resize(4, 0);
	PString strValue;
	PLog(" %s={%s}", propertyName, VectorStringConvert::vectorToString(userLicenseCodes, VectorStringConvert::UINT32ToString(), strValue, ","));
}

UINT32 UserLicenseCodes::getCode(UINT64 flags2) const
{
	UINT32 licenseVersion = (UINT32)( (flags2 & licenseVersionMask) >> licenseVersionShift );
	PASSERT(licenseVersion < 4);
	return userLicenseCodes[licenseVersion];
}

int UserLicenseCodes::getCodeIdx(UINT32 code) const
{
	for(int i = 0; i < userLicenseCodes.size(); ++i)
	{
		if (userLicenseCodes[i] == code)
			return i;
	}
	return -1;
}

INT16 UserLicenseCodes::getCodeByIdx(int idx, UINT32& code) const
{
	if (idx < 0 || idx >= userLicenseCodes.size())
		return INCONSISTENT_DATA;
	code = userLicenseCodes[idx];
	return DBM_NO_ERROR;
}

// PYR-20482 & PYR-20450
void mkdirRecursive( const char* dir )
{
    size_t len = strlen( dir );
    PBlock buf( len + 1 );
    memcpy( buf.ptr(), dir, buf.size() ); // copy original string to buf including terminating null

    const BYTE* const ptr = buf.ptr();
    BYTE* const endp = buf.ptr() + len;
    // find the parent that exists
    BYTE* endp2;
    for( endp2 = endp - 1; endp2 > ptr; --endp2 )
    {
        if( PDirectory::fileExists( (const char*)ptr ) )
        {
            break;
        }
        // find separator closest to the end
        for( ;*endp2 != '/' && *endp2 != '\\' && endp2 > ptr; --endp2 ) //lint !e445 // reuse of endp2 variable)
        {
        }

        if( *endp2 == '/' || *endp2 == '\\' )
        {
            *endp2 = '\0';
        }
		if( endp2 == ptr )
		{
			break;
		}
    }
    // now create the rest of the folder tree
    while( endp2 < endp ) //lint !e850 // endp2 is modified inside the above "for" loop)
    {
        // find next 0 to be replaced with path separator
        endp2 = (BYTE*)memchr( endp2, 0, endp - endp2 );
        if( endp2 )
        {
			// put back the path separator
			*endp2 = FILESEPARATORCHAR;    
        }
        PDirectory::makeDir( (const char*)ptr );

		if( !endp2 )
		{
			break;
		}
        ++endp2;
    }
}
