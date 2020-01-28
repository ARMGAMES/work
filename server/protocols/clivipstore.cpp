/*******************************************************************************
  clivipstore.h
********************************************************************************

  Copyright (c) 2001-2011 PYR Software Ltd. All rights reserved. This document
  is for the use of licensed users only and is subject to the terms and
  conditions of the accompanying and/or applicable license between you and
  PYR Software Ltd, including without limitation those terms and conditions
  prohibiting unauthorized translation, reverse engineering, decompilation,
  disassembly, and other inappropriate use of any software provided. Any
  unauthorized use, copying, modification, distribution, publication or
  disclosure of information is a violation of copyright laws. No reproduction
  in whole or in part of this document may be made without express written
  consent of PYR Software Ltd.

*******************************************************************************/

#include "clivipstore.h"
#include "climulticurr.h"
#include "VipStoreItemProps.h"

void VIPStoreData::synchronized()
{
	int path[] = { 0, 0 };
	int num = size( path, 1 );
	int& i = path[1];
	items.resize( num );
	for ( i = 0 ; i < num; ++i )
	{
		CommMsgParser parser( leaf( path, 2 ) );
		items[i].parse( parser );
	}
}

void VIPStoreData::VIPStoreItem::parse( CommMsgParser& parser )
{
	UINT32 vipStatusLevel;
	UINT32 numProps;

	// Parse the static fields
	parser.parseUINT32( itemId )
		.parseStringP( name )
		.parseUINT32( price )
		.parseUINT32( imageId )
		.parseStringP( url )
		.parseUINT32( vipStatusLevel )
		.parseUINT32( specialItem )
		.parseUINT32( siteId )
		.parseStringP( currency )
		.parseStringP( longDesc )
		.parseUINT32( numProps );

	vipLevel = ( VipStatusLevel )vipStatusLevel;

	sortOrder = 0;
	minYearVpp = 0;
	yearlyVppYear = 0;
	emailDelivery = false;
	orderLimit = 0;

	props.clear();
	props.reserve( numProps );

	// Parse the properties
	for( size_t i = 0; i < numProps; ++i )
	{
		VIPStoreItem::Property prop;

		UINT32 numValues;
		parser.parseStringP( prop.name ).
			parseUINT32( numValues );

		// Parse the property values
		prop.values.resize( numValues );
		for( size_t j = 0; j < numValues; ++j )
			parser.parseStringP( prop.values[ j ] );

		if( prop.name.equals( FPP_ITEM_PROP_CATEGORY ) )
		{
			if( prop.values.size() )
				category = prop.values.front();
		}
		else if( prop.name.equals( FPP_ITEM_PROP_SUBCATEGORY ) )
		{
			if( prop.values.size() )
				subcategory = prop.values.front();
		}
		else if( prop.name.equals( FPP_ITEM_PROP_SORT_ORDER ) )
		{
			if( prop.values.size() )
				sortOrder = strtoul( prop.values.front(), NULL, 10 );
		}
		else if( prop.name.equals( FPP_ITEM_PROP_MINIMAL_YEAR_VPP ) )
		{
			if( prop.values.size() )
				minYearVpp = strtoul( prop.values.front(), NULL, 10 );
		}
		else if( prop.name.equals( FPP_ITEM_PROP_EMAIL_DELIVERY ) )
		{
			if( prop.values.size() )
				emailDelivery = PString::compareIgnoreCase( prop.values.front(), "yes" ) == 0;
		}
		else if( prop.name.equals( FPP_ITEM_PROP_UNIQUE_BONUS ) )
		{
			if( prop.values.size() )
				orderLimit = strtoul( prop.values.front(), NULL, 10 );
		}
		else if( prop.name.equals( FPP_ITEM_PROP_UNIQUE_ROLL ) )
		{
			if( prop.values.size() )
				orderLimit = strtoul( prop.values.front(), NULL, 10 );
		}
		else if( prop.name.equals( FPP_ITEM_PROP_LIMIT_ORDERS ) )
		{
			if( prop.values.size() && (orderLimit == 0) )
				orderLimit = strtoul( prop.values.front(), NULL, 10 );
		}
		else if( prop.name.equals( FPP_ITEM_PROP_YEARLY_VPP_YEAR ) )
		{
			if( prop.values.size() )
				yearlyVppYear = strtoul( prop.values.front(), NULL, 10 );
		}
		else
		{
			props.push_back( prop );
		}
	}

	UINT32 sz;
	parser.parseUINT32( sz );
	countries.clear();
	countries.reserve( sz );
	for( size_t i = 0; i < sz; ++i )
	{
		const char* oneString;
		parser.parseString( oneString );
		countries.push_back( oneString );
	}

	if( !parser.parseEnded() )	// PYR-24493
	{
		i18nName.parse( parser );
		i18nDesc.parse( parser );

		// Client will not support multiple items' categories and subcategories -
		// there is no proper relationship introduced in the interface.
		parser.parseUINT32( sz );
		for( size_t i = 0; i < sz; ++i )
			i18nCategory.parse( parser );
		parser.parseUINT32( sz );
		for( size_t i = 0; i < sz; ++i )
			i18nSubCategory.parse( parser );
	}
}

const char* VIPStoreData::VIPStoreItem::getName( int locale ) const
{
	const char* ret = i18nName.getPString( locale ).c_str();
	if( ret && *ret )
		return ret;
	return name.c_str();
}

const char* VIPStoreData::VIPStoreItem::getDesc( int locale ) const
{
	const char* ret = i18nDesc.getPString( locale ).c_str();
	if( ret && *ret )
		return ret;
	return longDesc.c_str();
}

const char* VIPStoreData::VIPStoreItem::getCategory( int locale ) const
{
	const char* ret = i18nCategory.getPString( locale ).c_str();
	if( ret && *ret )
		return ret;
	return category.c_str();
}

const char* VIPStoreData::VIPStoreItem::getSubCategory( int locale ) const
{
	const char* ret = i18nSubCategory.getPString( locale ).c_str();
	if( ret && *ret )
		return ret;
	return subcategory.c_str();
}
