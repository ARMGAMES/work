#ifndef dbmlicenserules_h_included
#define dbmlicenserules_h_included

#include "licenserules.h"
#include "dbmstmtcommon.h"

class DbmLicenseRules
{
public:
	// PYR-96119, does not throw
	// returns false if LicenseRulesConfig was already initialized
	// returns false if SQL statement failed
	static bool readAndPrintLicenseHostConfig(DatabaseManagerCommon& man, LicenseRulesConfig& licenseRulesConfig)
	{
		PString logStr;

		if (!licenseRulesConfig._peripheralLicenseToHostMap.empty())
		{
			PLog("LicenseRulesConfig: re-reading attempt ignored, current peripheralLicenseToHostMap=%s",
				licenseRulesConfig.toString(logStr));
			return false;
		}

		try
		{
			GetCfgLicenseHostStmt stmt(man);
			stmt.execute();
			while (stmt.fetch())
			{
				licenseRulesConfig._addLicenseHost(stmt.getLicenseId(), stmt.getHostId());
			}
			stmt.closeCursor();
		}
		catch (const PSqlError& er)
		{
			PLog("LicenseRulesConfig: %d: '%s' while reading from CFG_LICENSEHOST; skipping init", er.code(), er.why());
			return false;
		}

		if (licenseRulesConfig._peripheralLicenseToHostMap.empty())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig: table CFG_LICENSEHOST is empty");
		}

		PLog("LicenseRulesConfig: %s", licenseRulesConfig.toString(logStr));
		return true;
	}
};

#endif // #define dbmlicenserules_h_included