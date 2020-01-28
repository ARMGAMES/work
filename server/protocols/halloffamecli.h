// halloffamecli.h
// PYR-23062
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef halloffamecli_h_included
#define	halloffamecli_h_included

#include "pplib.h"
#include "composecolls.h"

enum HallOfFameLevelIds
{
	eHOF_LID_100 = 100,
	eHOF_LID_200 = 200,
	eHOF_LID_300 = 300,
};

enum HallOfFameFlags
{
	eHOF_Email_User			= 1<<0,
	eHOF_Email_CSR			= 1<<1,
	eHOF_HallOfFame_Level	= 1<<2,
};

class HallOfFameLevel
{
public:
	UINT32	levelId;
	UINT32	ltVppThreshold;
	BYTE	flags;


	HallOfFameLevel( UINT32 levelId_, UINT32 ltVppThreshold_, BYTE emailFlags_) 
		: 
	levelId( levelId_ ), 
	ltVppThreshold( ltVppThreshold_ ), 
	flags( emailFlags_ )
	{
	}
	HallOfFameLevel() : levelId( 0 ), ltVppThreshold( 0 ), flags( 0 )
	{
	}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		CommMsgBody msgBody;
		msgBody
			.composeUINT32( levelId )
			.composeUINT32( ltVppThreshold )
			.composeBYTE( flags );

		body.composeMsgBody( msgBody );
		
return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		CommMsgBody msgBody;
		parser.parseMsgBody( msgBody );
		CommMsgParser parserBody( msgBody );
		
		parserBody
			.parseUINT32( levelId )
			.parseUINT32( ltVppThreshold )
			.parseBYTE( flags );

		return parser;
	}
	const bool operator <(const HallOfFameLevel& rhs) const
	{
		return levelId < rhs.levelId;
	}
	void log() const
	{
		PLog( "levelId: %u threshold: %u flags: %u", levelId, ltVppThreshold, flags );
	}
};

struct HofLevelsForLicense
{
	typedef set< HallOfFameLevel > HofLevels; // want them ordered
	HofLevels levels;
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		CommMsgBody msgBodyLevels;
		composeSetOfComposableObjects( msgBodyLevels, levels );
		body.composeMsgBody( msgBodyLevels );
		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		CommMsgBody msgBodyLevels;
		parser.parseMsgBody( msgBodyLevels );
		CommMsgParser parserLevels( msgBodyLevels );
		parseSetOfParsableObjects( parserLevels, levels );
		return parser;
	}
	UINT32 getHofLevelIdByVpp( const UINT32 ltVpp ) const
	{
		const HallOfFameLevel* unused;

		return getHofLevelIdByVpp( ltVpp, unused );
	}
	UINT32 getHofLevelIdByVpp( const UINT32 ltVpp, const HallOfFameLevel*& level ) const
	{
		level = NULL;
		for( HofLevels::const_iterator it = levels.begin(); it != levels.end(); ++it )
		{
			const HallOfFameLevel& hl = *it;
			if( hl.ltVppThreshold <= ltVpp && ( level == NULL || level->levelId < hl.levelId ) )
			{
				level = &hl;
			}
		}
		return level == NULL ? 0 : level->levelId;
	}
	void log() const
	{
		for( HofLevels::const_iterator it = levels.begin(); it != levels.end(); ++it )
		{
			it->log();
		}
	}
	UINT32 getHofTresholdByLevelId( const UINT32 levelId ) const
	{
		const HallOfFameLevel* unused;

		return getHofTresholdByLevelId( levelId, unused );
	}
	UINT32 getHofTresholdByLevelId( const UINT32 levelId, const HallOfFameLevel*& level ) const
	{
		level = NULL;
		for( HofLevels::const_iterator it = levels.begin(); it != levels.end(); ++it )
		{
			const HallOfFameLevel& hl = *it;
			if( hl.levelId == levelId )
			{
				level = &hl;
			}
		}
		return level == NULL ? 0 : level->ltVppThreshold;
	}
};

#endif // halloffamecli_h_included
