// ===========================================================
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef	commonmacros_h_included
#define	commonmacros_h_included

// moved from protocols/shared.h
#define SZARR(a) (sizeof(a)/sizeof(*(a)))

// moved from protocols/shared.h
#define DEFAULT_VECTOR_RESERVE_MAXSIZE 1024

// When used at the end of enum the compiler will generate enum last and last + 1 values as:
// <enum name>_Last and <enum name>_NextAfterLast. If you need to insert after that you need
// to start with comma. For example:
//enum HgChangePlayerStatusAction
//{
//	eHgAcceptPlayer = 0,
//	eHgRejectPlayer,
//	eHgBlockPlayer,
//	eHgUnblockPlayer,
//	eHgSuspendPlayer,
//	eHgReinstatePlayer,
//	eHgRemovePlayer,
//	eHgMakePlayerAdmin,
//	eHgRevokePlayerAdmin,
//	eHgRemoveApplication,
//
//	ENUM_LAST_VALUE(HgChangePlayerStatusAction) // HgChangePlayerStatusAction_Last & HgChangePlayerStatusAction_NextAfterLast
//};
#define ENUM_LAST_VALUE(name) name ## _NextAfterLast, name ## _Last = name ## _NextAfterLast - 1

#endif // commonmacros_h_included
