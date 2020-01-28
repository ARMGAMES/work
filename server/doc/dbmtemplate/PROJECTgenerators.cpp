// ===========================================================
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "PROJECTmanager.h"
#include "commonmacros.h"

// PYR-27418 - "TableName1" generator is local (doesn't share ID space between NJ and IOM site)
// "TableName2" generator is shared (NJ site will ask IOM site for IDs)
const DbmGenerator::Generator* PROJECTManager::getGenerators( size_t& generatorsSize ) const // override
{
	static const DbmGenerator::Generator generators[] =
	{
		//  objectName  			local
		//	--------------			-----------
		{ "TableName1",				true },
		{ "TableName2",				false },
		// NB: add a line here for each new ID generator
	};
	generatorsSize = SZARR( generators );
	return generators;
}
