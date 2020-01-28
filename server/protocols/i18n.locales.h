#ifndef i18n_locales_h_included
#define i18n_locales_h_included

#include "ppmsg.h"
#include "ppinifile.h"

#include "i18n.localedef.h"

//
// Language flags
#define LFLAG_HIDDEN			0x0			// DEBUG client only
#define LFLAG_PRODUCTION		0x4			// Show on Production

// Beta flags
#define LFLAG_FUTURE_SUPPORT	(0x1 << 3)	// Future support
#define LFLAG_NO_SUPPORT		(0x3 << 3)	// CSR doesn't provide support in this language
#define LFLAG_LIMITED_SUPPORT	(0x5 << 3)	// CSR provides limited support in this language
#define LFLAG_FULL_SUPPORT		(0x7 << 3)	// CSR provides full support in this language
#define LMASK_SUPPORT			(0x7 << 3)

#define LFLAG_UTF8				(0x1 << 6)	// Not a 1252 codepage

#define LFLAG_LOGOGRAPHIC		(0x1 << 7)	// Character set contains hieroglyphs

//
// Tag flags, specify as "FLG(HTML|MULTI|CONST) your message", get back from PMsgId
#define TFLAG_HTML						0x1 // client hint - html follows
#define TFLAG_MULTI						0x2 // i18n library hint - keep every language
#define TFLAG_CONST						0x4 // i18n scripts hint - do not overwrite with translations


extern PMsgLocaleTable LocalesTable;		// Pass to PMsg()

//-----------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

extern void i18nGetProperty(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const PIniFile::Section* section, const char* key);
extern unsigned int i18nGetLocaleId(const PMsgLocaleTable& table, unsigned int localeId, const vector<unsigned int>& locales);

// the search is linear/slow, please let know igorS if this function is planned for frequent use, and/or on the client table
extern bool i18nFindId(PMsgId& id, const PMsgLocaleTable& LocaleTable, const PMsgDataTable& dataTable, const PString& str);

struct PMsgTableImage
{
	int				localeId;
	unsigned int	flags;
	PString			tag;
	PString			msg;
};

typedef bool (*PMsgLocaleFilter)(const PMsgLocaleTable& table, UINT32 localeId, const PMsgData* msgData, int dataSize, const PMsgTableImage& tableImage, int msgId);
extern bool i18nDynamicLocaleFilter(const PMsgLocaleTable& table, UINT32 localeId, const PMsgData* msgData, int dataSize, const PMsgTableImage& tableImage, int msgId);

extern int i18nLoadDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, const PMsgStrTable& strTable, PInputStream* txtStream, PString& err);
extern int i18nLoadFilteredDynamicTable(const PMsgLocaleTable& table, UINT32 locale, PMsgDataTable& dataTable, const PMsgStrTable& strTable,
	PInputStream* txtStream, PString& err, PMsgLocaleFilter localeFilter = i18nDynamicLocaleFilter);
extern void i18nFreeDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, const PMsgStrTable& strTable);
extern void i18nFreeDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, unsigned int dataSize);
extern UINT32 i18nGetLocale(const char * language);
extern void i18nGetLanguage(UINT32 locale, PString & res);

//-----------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

#endif // i18n_locales_h_included
