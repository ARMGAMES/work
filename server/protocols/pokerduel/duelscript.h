//	duelscript.h

#ifndef	duelscript_h_included
#define	duelscript_h_included

#include "pyrtime.h"
#include "commmsgbody.h"
#include "ppinifile.h"
#include "i18n.strmap.h"
#include "licenseid.h"
#include "tourscript.h"



void parseDuelScript ( const PIniFile& iniFile, vector<PString>& errors, CommMsgBody& dataObjectsBodies );

#endif	//duelscript_h_included
