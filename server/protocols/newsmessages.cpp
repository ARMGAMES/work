#include "newsmessages.h"

void NewsItem::print() const
{
	PTRACE3("version=%d,mask=%d,name=%s",newsVer,changesMask,newsName.c_str());
	PTRACE3("priority=%d,displayType=%d",newsPriority,newsDisplayType);
	PTRACE3("start=%d/%d/%d-%02d:%02d:%02d",newsStartTime._year,newsStartTime._month,newsStartTime._day,newsStartTime._hour,newsStartTime._minute,newsStartTime._second);
	PTRACE3("end=%d/%d/%d-%02d:%02d:%02d",newsEndTime._year,newsEndTime._month,newsEndTime._day,newsEndTime._hour,newsEndTime._minute,newsEndTime._second);
	PTRACE3("criteria=%s",newsCriteria.c_str());
}

UINT32 /*enum ePIPEFlags*/ convertContentPreferenceToPipeFlag( UINT32 contentPreference /*enum ContentPreference*/ )
{
	UINT32 flags = 0;
	if((contentPreference & PokerStarsPref) != 0)
	{
		flags |= ePokerStarsPref;
	}
	if((contentPreference & CasinoPref) != 0)
	{
		flags |= eCasinoPref;
	}
	if((contentPreference & SportsBookPref) != 0)
	{
		flags |= eSportsBookPref;
	}

	return flags;
}