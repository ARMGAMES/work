#ifndef basegatewaystructs_h_included
#define basegatewaystructs_h_included

#include "commcli.h"
#include "ppinifile.h"
#include "licenseid.h"
#include "timezone.h"

#define AAMS_PROTOCOL_ERROR 50
#define AAMS_PROTOCOL_PKCS7_ERROR 55
#define AAMS_PROTOCOL_NOT_AVAILABLE 200

enum eServerConnectionType
{
    AAMSDbmServer                                   = 1,
    AAMSDaemonServer                                = 2,
    AAMSTableServer                                 = 3,
    AAMSGamingAccountServer                         = 4,
    AAMSMajordomo                                   = 5,
    CoreGatewayServerConnectionType_TrustedAdmin    = 6,
    SocialPsapiServer                               = 7,
    BelgiumEpisServer                               = 9,
    DanishSpmServer                                 = 10,
    SpainCnjServer                                  = 11,
	DeshGWServer                                    = 12,
	DeshGWImporter                                  = 13,
	PtGWServer                                      = 14,
	BgGWServer                                      = 15,
	AAMSAutomationServer                            = 16,
	SwedishGWServer                                 = 17,
	CzCoreGWServer                                  = 18,
	LobbyEtcServer									= 19,					
};


class UserLicenseCodes
{
	vector<UINT32> userLicenseCodes; // #19372 Array of concessionary codes mapped to licenseVersionMask (USERS.FLAGS2).

public:
	void init(const PIniFile::Section* section, UINT32 defaultCode, const char* propertyName = "userLicenseCodes");
	UINT32 getCode(UINT64 flags2) const;
	int getCodeIdx(UINT32 code) const; // PYR-50515
	INT16 getCodeByIdx(int idx, UINT32& code) const; // PYR-50515
};


class AsyncCallRequestId : public CommClientGConnection::AsyncCall
{
public:
	virtual UINT32 getConnId() const = 0; // PYR-26992
	virtual UINT32 getReqId() const = 0;
	virtual UINT32 getTransportKey() const = 0;
	virtual AsyncCallRequestId* clone() const = 0; // consider in overrides to return the covariant actual types - check (just in case) that AIX compiler would support it
	virtual ~AsyncCallRequestId(){}
};

// Create a directory and if necessary all missing parent directories
void mkdirRecursive( const char* dir );

inline void convertUtcTimeToEtTime(const CommUtcTime& utcTime, SrvTime& etTime)
{
	tzUtcToLocalSrvTime(utcTime.encode(), etTime, eTzET);
}
inline void convertEtTimetoUtcTime(const SrvTime& etTime, CommUtcTime& utcTime)
{
	utcTime.decode(tzSrvTimeToUtc(etTime,eTzET));
}

#endif // basegatewaystructs_h_included
