#ifndef i18n_utils_h_included
#define i18n_utils_h_included

#include "ppi18n.h"
#include "ppunistring.h"

void i18nAppend(PString& trg, const char* src);
void i18nAppendUint(PString& trg, unsigned int val);
void i18nAppendInt(PString& trg, int val);
void i18nAppendUint64(PString& trg, UINT64 val);
void i18nAppendInt64(PString& trg, INT64 val);
void i18nAppendChar(PString& trg, PUNICHAR c);
void i18nAppendChar(PString& trg, char c);
bool i18nCut(PString& inOut, INT32 maxBytes, bool doLogOrigIfCut); // returns true and logs if cut happened // PYR-33629

void i18nPsx_Utf8(PString& ret, const char* in);
void i18nPsx_Utf8NoReserve(PString& ret, const char* in);
void i18nPsx_Utf8_XmlEncoded(PString/*UTF8*/& ret, const char* /*PStringX*/in);
UINT32 i18nPsx_Utf8(PString& ret, const char* in, UINT32 maxLen);
void i18nUtf8_Psx(PString& ret, const char* in);
bool i18nUtf8_Psx_validated(PString& ret, const char* in);
void i18nUtf8_Uni(PUniString& ret, const char* in);
void i18nPsx_Uni(PUniString& ret, const char* in);
void i18nSplitFullNameBySpaces(const char* fullName, vector<PString>& names);
void i18nSplitFullNameByFirstSpace(const char* fullName, PString& firstName, PString& lastName);

class I18nCharacterNormalizer
{
public:
	static const PUNICHAR_RET UNDEFINED_CHAR = -5;
	virtual ~I18nCharacterNormalizer() {}
	// returns non-negative encoding for normalized character 
	virtual PUNICHAR_RET normalize(PUNICHAR_RET c) const = 0;
};
void i18nAppendNormalized(PString/*PStringX*/& trg, const char* /*PStringX*/src, const I18nCharacterNormalizer& normalizer);
bool i18nIsNormalized(const char* /*PStringX*/src, const I18nCharacterNormalizer& normalizer);

#endif // i18n_utils_h_included
