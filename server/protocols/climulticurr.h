//	climulticurr.h

#ifndef	climulticurr_h_included
#define	climulticurr_h_included

#include "commmsgbody.h"
#include "ppstring.h"

const char* _parseCurrency( CommMsgParser& parser );
PString& _parseCurrency( CommMsgParser& parser, PString& curr );

PString& _fixCurrency( PString& curr );

#endif // climulticurr_h_included
