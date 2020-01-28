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

#ifndef	clivipstore_h_included
#define	clivipstore_h_included

#include "dbm.h"
#include "commcli.h"
#include "VipStoreInventorySharedCli.h"
#include "i18n.strmap.h"

class VIPStoreData : public CommClientSubscriber
{
public:
	struct VIPStoreItem
	{
		struct Property
		{
			PString name;
			vector< PString > values;
			PString userValue;	// result of choice
		};

		PString name;
		UINT32 sortOrder;
		PString url;
		UINT32 itemId;
		UINT32 imageId;
		UINT32 price;
		VipStatusLevel vipLevel;
		UINT32 specialItem;
		UINT32 siteId;
		PString longDesc;
		PString currency;
		vector< Property > props;
		vector< PString > countries;

		// Special Properties
		PString category;
		PString subcategory;
		UINT32 minYearVpp;
		UINT32 yearlyVppYear;
		bool emailDelivery;
		UINT32 orderLimit;

		PString coupon;
		UINT32 couponValue;
		UINT32 couponRemainder;

		// PYR-24493
		I18nPString i18nName;
		I18nPString i18nDesc;
		I18nPString i18nCategory;
		I18nPString i18nSubCategory;

		void parse( CommMsgParser& parser );

		const char* getName( int locale ) const;
		const char* getDesc( int locale ) const;
		const char* getCategory( int locale ) const;
		const char* getSubCategory( int locale ) const;
	};

	vector< VIPStoreItem > items;

protected:
	void synchronized() override;
};

#endif // clivipstore_h_included
