//------------------------------------------------------------------------------
//	Throwables.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2019 PYR Software Ltd. All rights reserved. This document
//	is for the use of licensed users only and is subject to the terms and
//	conditions of the accompanying and/or applicable license between you and
//	PYR Software Ltd, including without limitation those terms and conditions
//	prohibiting unauthorized translation, reverse engineering, decompilation,
//	disassembly, and other inappropriate use of any software provided. Any
//	unauthorized use, copying, modification, distribution, publication or
//	disclosure of information is a violation of copyright laws. No reproduction
//	in whole or in part of this document may be made without express written
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#if !defined( __Throwables_h_included__ )
#define __Throwables_h_included__

//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include "commonmacros.h"

//------------------------------------------------------------------------------
// PYR-98733
//------------------------------------------------------------------------------

namespace Throwable
{

//------------------------------------------------------------------------------

enum Type
{
	Type_First = 1,
	rose = Type_First,
	horseshoe = 2,
	pie = 3,
	fish = 4,

	ENUM_LAST_VALUE( Type )
};

//------------------------------------------------------------------------------

} // namespace Throwable

//------------------------------------------------------------------------------
#endif // !defined( __Throwables_h_included__ )
//------------------------------------------------------------------------------
