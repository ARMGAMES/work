#ifndef chatfilter_h_included
#define chatfilter_h_included

#include "ppinclude.h"
#include "ppunistring.h"
#include "sitemasks.h"
#include "table.h"

#define WHITELISTSEPARATOR	"*****"

class ChatFilterHelper
{
	bool isDirty;
	vector<PUniString> myPatternUni;
	size_t blackListSize;

	// No implementation
	ChatFilterHelper(const ChatFilterHelper&);
	ChatFilterHelper& operator=(const ChatFilterHelper&);

#if PTRACELVL >= 5
	void PUniString2PStringX(PString& ret, const PUniString& s);
	void dump();
#endif

public:
	ChatFilterHelper() : isDirty(true), blackListSize(0) {}
	void setDirty() { isDirty = true; }
	const vector<PUniString>& convertToPUniString(const vector<PString>& myPattern,  size_t& blackListSize_);
	static ChatCategory getChatCategoryFromSite( const PSiteMask& siteMask );
};

bool _filterChat( PString& ret, const vector< PString >& myPattern, ChatFilterHelper* pChatFilterHelper = NULL );
void removeInvalidSymbolsFromChatString( PString& ret, const char* chatInput, bool noExtendedAsciiCode = false );

#endif
