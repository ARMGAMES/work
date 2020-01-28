#ifndef commtok822_h_included
#define commtok822_h_included

void CommAddressNormalize(vector<PString> &addr, vector<PString> & addrn, const char * addressString);
void CommAddressNormalize(PString &addr, PString &addrn, const char * addressString);

class CommMailAddressParseData
{
public:
	int nSubDomains;
	int nDomainLiterals;
	PString lastSubDomain;

public:
	CommMailAddressParseData()
	{
		nSubDomains = 0;
		nDomainLiterals = 0;
	}
};

const int CommMailAddressFlagSpaceAsAddressDelimiterAllowed = 0x01;//not applicable to single address parsing
const int CommMailAddressFlagBareRouteAddrDisallowed = 0x02;//address in form "<abc@def.com>" (RFC822 requires something before it: "abc <abc@def,com>", but it was changed by RFC1123)
const int CommMailAddressFlagNormalizeQuotedLocalPart = 0x04;//normalize "abc"@def.com to abc@def.com

void CommMailAddressNormalize( vector<PString> &addr, vector<PString> & addrn, const char * addressString, int flags = 0 );
void CommMailAddressNormalize( PString &addr, PString &addrn, const char * addressString, CommMailAddressParseData* retParseData = 0, int flags = 0 );

#endif
