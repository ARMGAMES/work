//------------------------------------------------------------------------------
//	ImageGalleryShared.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2014 PYR Software Ltd. All rights reserved. This document
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

#ifndef ImageGalleryShared_h_included
#define ImageGalleryShared_h_included

#include "pplib.h"
#include "commlib.h"
#include "pyrtime.h"
#include "PBitmask.h"
#include "sitemasks.h"

#define IMAGECATS_TABLE_NAME "IMAGECATS"

#define IMAGE_GALLERY_CATS_AUDIT_TABLE_NAME	"IMAGECATSAUDIT"
#define IMAGE_GALLERY_AUDIT_TABLE_NAME		"IMAGEGALLERYAUDIT"

#define NEXT_IMAGE_GAL_CAT_AUDIT_ID			"NextImageGalCatAuditId"
#define NEXT_IMAGE_GALLERY_AUDIT_ID			"NextImageGalleryAuditId"

#define IMAGE_GALLERY_COMMMENTS_LENGTH (128 + 1)

#define IMAGE_CRITERIA_LENGTH	(3600 + 1)	// PYR-28965

enum eImageGalleryPublishMode
{
	eIGPM_Freeze			= 0,
	eIGPM_FreezeAndClear	= 1,
	eIGPM_Republish			= 2,
	eIGMP_Uninitialized		= 3,
	eIGMP_Waiting			= 4,
};

#define IMAGE_GALLERY_HIDDEN_CATID 0x0FFFFFFF

#define MAX_ALLOWED_CUSTOM_USER_IMAGES 1


class ImageCatItem
{
	enum eImageCatItemFlags   // this definition must be synchronized with the definition in admin java code
	{
		// for some reason, the 1 & 2 are not used in java admin code
		eICF_Normal = 4,   // set and used by admin for display
		eICF_Stealth = 8,  // set and used by admin for display  // PYR-106207
	};
public:
	UINT32 catId;
	UINT32 parentId;
	UINT32 templateId;
	UINT32 flags;  // the flags value is transparent to server code, set and used in java admin page, stored in database IMAGECATS table
	UINT32 siteMaskObsolete; // PYR-100634
	UINT32 displayPriority;
	PString comments;
	UINT32 brandMask;		//	PYR-41324
	PSiteMask siteMask;		// PYR-100634

	ImageCatItem()	: catId(0)
					, parentId(0)
					, templateId(0)
					, flags(0)
					, siteMaskObsolete(0)
					, displayPriority(0)
					, brandMask(0) {}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body
			.composeUINT32( catId )
			.composeUINT32( parentId )
			.composeUINT32( templateId )
			.composeUINT32( flags )
			.composeUINT32( siteMaskObsolete )
			.composeString( comments )
			.composeUINT32( displayPriority )
			.composeUINT32( brandMask )
			.composeObject( siteMask ) // PYR-100634
			;

			return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32( catId )
			.parseUINT32( parentId )
			.parseUINT32( templateId )
			.parseUINT32( flags )
			.parseUINT32( siteMaskObsolete )
			.parseStringP( comments )
			.parseUINT32( displayPriority )
			.parseUINT32( brandMask )
			;

			if( parser.parseEnded() )
			{
				// approved usage of PSiteMask::fromUINT64()
				siteMask = PSiteMask::fromUINT64( siteMaskObsolete ); // Compatibility with the old admin version
			}
			else
			{
				parser.parseObject( siteMask ); // PYR-100634
			}
		
		return parser;
	}
	ImageCatItem( CommMsgParser& parser )
	{
		parse( parser );
	}
	void print() const
	{
		PString str;
		PLog("ImageCatItem: catId=%u parentId=%u templateId=%u flags=%u siteMask=0x%s brandMask=%u", catId, parentId, templateId, flags, siteMask.toDbString(str), brandMask);
	}
	bool isHidden() const
	{
		return isHidden( catId );
	}
	static bool isHidden( UINT32 id )
	{
		return ( id == IMAGE_GALLERY_HIDDEN_CATID );
	}
	bool equalsForPublish( const ImageCatItem& rhs ) const
	{
		return    catId			== rhs.catId
			   && parentId		== rhs.parentId
			   && siteMask		== rhs.siteMask
			   && brandMask     == rhs.brandMask		//	PYR-41324
			   // && displayPriority == rhs.displayPriority // *** essential for publication but only when republishing
			   // && templateId	== rhs.templateId	// not essential for publication
			   // && flags			== rhs.flags	// not essential for publication
			   // && comments.equals(rhs.comments)	// not essential for publication
			   ;
	}
	bool isStealthImageCategory() const  // PYR-106207
	{
		return ( flags & eICF_Stealth ) != 0;
	}
};


class ImageGalleryItem
{

public:
	enum eImageGalleryItemFlags
	{
		eIGIF_Inactive				= (1<<0),
		eIGIF_Banned				= (1<<1),
		eIGIF_InVisibleForCategories= (1<<2), // used only by the admin. if flag is set image cannot be added to categories
	};
	enum 
	{
		unknownLeafId = 0xFFFFFFFF,
	};

	UINT32 imageId;
	UINT32 catId;
	UINT32 templateId;
	UINT32 flags;
	PString comments;
	UINT32 displayPriority;
	UINT32 leafId; // non-persistable

	ImageGalleryItem()	: imageId(0)
						, catId(0)
						, templateId(0)
						, flags(0)
						, displayPriority(0)
						, leafId(unknownLeafId)
	{
	}
	ImageGalleryItem( UINT32 imageId_, UINT32 catId_, UINT32 templateId_, UINT32 flags_, const char* comments_, UINT32 displayPriority_ )
		: 
		  imageId(imageId_)
		, catId(catId_)
		, templateId(templateId_)
		, flags(flags_)
		, comments( comments_ )
		, displayPriority( displayPriority_ )
		, leafId(unknownLeafId)
	{
	}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return body
			.composeUINT32( imageId )
			.composeUINT32( catId )
			.composeUINT32( templateId )
			.composeUINT32( flags )
			.composeString( comments )
			.composeUINT32( displayPriority )
			;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32( imageId )
			.parseUINT32( catId )
			.parseUINT32( templateId )
			.parseUINT32( flags )
			.parseStringP( comments )
			;
		if( !parser.parseEnded() ) // TODO: remove before build - for QA compatibility only
		{
			parser.parseUINT32(displayPriority);
		}
		else
		{
			displayPriority = 0;
		}
		return parser;
	}
	ImageGalleryItem( CommMsgParser& parser ) : leafId(unknownLeafId)
	{
		parse( parser );
	}
	bool canPublish() const
	{
		return ( flags & ( eIGIF_Inactive | eIGIF_Banned ) ) == 0;
	}
	bool equals( const ImageGalleryItem& rhs ) const
	{
		if( &rhs == this )
			return true;
		
		// not comparing leaf id
		return (	( imageId == rhs.imageId )
				&&	( catId == rhs.catId )
				&&	( flags == rhs.flags )
				&&	( templateId == rhs.templateId )
				&&	( displayPriority == rhs.displayPriority )
				&&	  comments.equals( rhs.comments ) );
	}
	bool isPublished() const
	{
		return (leafId != unknownLeafId);
	}
	void setUnpublished()
	{
		leafId = unknownLeafId;
	}
	void print() const
	{
		PLog("ImageGalleryItem: imageId=%u, catId=%u, templateId=%u, flags=%u, p=%u", imageId, catId, templateId, flags, displayPriority);
	}
};

enum eImageApprovalFlags
{
	eIAF_Approved	= (1<<0),
};

class ImageApprovalItem
{
public:
	UINT32 imgId;
	UINT32 userIntId;
	UINT32 flags;
	SrvTime whenApproved;	// PYR-29014, not included in parse/compose until we need it.

	ImageApprovalItem( UINT32 imgId_, UINT32 userIntId_, UINT32 flags_ ) 
		: imgId(imgId_), userIntId(userIntId_), flags(flags_)
	{
	}
	ImageApprovalItem() : imgId(0)
						, userIntId(0)
						, flags(0){}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return body
			.composeUINT32( imgId )
			.composeUINT32( userIntId )
			.composeUINT32( flags )
			;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		return parser
			.parseUINT32( imgId )
			.parseUINT32( userIntId )
			.parseUINT32( flags )
			;
	}
	ImageApprovalItem( CommMsgParser& parser )
	{
		parse( parser );
	}
	bool isApproved() const
	{
		return isApproved( flags );
	}
	static bool isApproved( UINT32 flags_ )
	{
		return (flags_ & eIAF_Approved) != 0;
	}
};

class ImageGalleryCriterion
{
	void clear()
	{
		intVal = 0;
		type = eCriterionTypeUndefined;
		strVal.cut(0);
	}

public:
	enum eCriterionType // sequential numbers 0 - 0xFFFE
	{
		eCriterionTypeUndefined	= 0,
		eCriterionTypeCountry	= 1,

		// insert all criteria before this line. they must be sequential
		eCriterionTypeEmpty		= 0xFFFF		
	};
	PString			strVal;
	INT32			intVal;
	UINT32			type; // non-composable - as per strong Client insistence. inferred from the order of compose

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return body
			.composeString( strVal )
			.composeINT32( intVal )
			;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		if( parser.parseEnded() )
		{
			// an empty message body signifies empty criterion
			clear();
			type = eCriterionTypeEmpty;
			return parser;
		}

		return parser
			.parseStringP( strVal )
			.parseINT32( intVal )
			;
	}
	ImageGalleryCriterion( CommMsgParser& parser )
	{
		parse( parser );
	}
	ImageGalleryCriterion() : intVal(0), type(eCriterionTypeUndefined)
	{
	}
	bool isEmpty() const 
	{
		return type == eCriterionTypeEmpty;
	}
	void print() const
	{
		PLog( "IGC %u: '%s' %u", type, strVal.c_str(), intVal );
	}
};

class ImageGalleryTag
{
public:
	INT32			tagId;
	INT32			imageId;
	PString			strVal;
	INT32			intVal;
	
	
	
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return body
			.composeINT32( tagId )
			.composeINT32( imageId )
			.composeString( strVal )
			.composeINT32( intVal )
			;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		return parser
			.parseINT32( tagId )
			.parseINT32( imageId )
			.parseStringP( strVal )
			.parseINT32( intVal )
			;
	}
	ImageGalleryTag( CommMsgParser& parser )
	{
		parse( parser );
	}
	ImageGalleryTag( INT32 tagId_, INT32 imageId_, PString strVal_, INT32 intVal_ ) : tagId(tagId_), imageId(imageId_), strVal(strVal_), intVal(intVal_)
	{
	}
	ImageGalleryTag() : tagId(0), imageId(0), intVal(0)
	{
	}
	void print() const
	{
		PLog( "IGT: t=%u i=%u '%s' %u", tagId, imageId, strVal.c_str(), intVal );
	}
};

class ImageCriteriaItem	// PYR-28965
{
public:
	UINT32 imageId;
	PString criteria;

	ImageCriteriaItem()	: imageId( 0 )
	{
	}

	ImageCriteriaItem( UINT32 imageId_, const PString& criteria_ )	: imageId( imageId_ ), criteria( criteria_ )
	{
	}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return body
			.composeUINT32( imageId )
			.composeString( criteria )
			;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32( imageId )
			.parseStringP( criteria )
			;
		
		return parser;
	}

	ImageCriteriaItem( CommMsgParser& parser ) 
	{
		parse( parser );
	}

	void print() const
	{
		PLog("ImageTagItem: imageId=%u, criteria='%s'", imageId, criteria.c_str());
	}
};

#endif // ImageGalleryShared_h_included
