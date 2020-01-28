// FppBundlesCli.h
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef FppBundlesCli_h_included
#define FppBundlesCli_h_included

enum FppBundleTargetObjective
{
	eFBTO_Unknown	= 0,
	eFBTO_KeepVip	= 1<<0,
	eFBTO_NextVip	= 1<<1,
	eFBTO_NoAction	= 1<<2,
	eFBTO_Milestone	= 1<<3,
};

class UserFppBundleSingleLevel
{
public:
	UserFppBundleSingleLevel()	:	accVpp		( 0 )
								,	fppReward	( 0 )
								,	targetType	( 0 )
								,	penalty		( 0 )
								,	vipOrd		( 0 ) {}
	
	UserFppBundleSingleLevel( INT32	accVpp_		
							, INT32	fppReward_
							, INT16	targetType_	
							, INT16	penalty_
							, INT32	vipOrd_	)		
							:	
								accVpp		( accVpp_		)
							,	fppReward	( fppReward_	)
							,	targetType	( targetType_	)
							,	penalty		( penalty_		)
							,	vipOrd		( vipOrd_		) {}

	UserFppBundleSingleLevel( CommMsgParser& parser )
	{
		parse( parser );
	}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeINT32	(	accVpp		)
			.composeINT32	(	fppReward	)
			.composeINT16	(	targetType	)
			.composeINT16	(	penalty		)
			.composeINT32	(	vipOrd		)
			;

		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseINT32	(	accVpp		)
			.parseINT32	(	fppReward	)
			.parseINT16	(	targetType	)
			.parseINT16	(	penalty		)
			.parseINT32	(	vipOrd		)
			;
		return parser;
	}
	void print() const
	{
		PLog( "bundle: vpp %d fpp %d %c p %d o %d"
			,accVpp, fppReward, targetType == eFBTO_KeepVip ? 'k' : 'n', penalty, vipOrd );
	}
public:

	INT32	accVpp		;
	INT32	fppReward	;
	INT16	targetType	;	// eFppBundleTargetType - keep existing VIP, achieve next, etc
	INT16	penalty		;	// 0 - 100 percent
	INT32	vipOrd		;	// Redundant information for bundle order within a level. Supplied by parser.
};

class UserFppBundleVppNotifyData
{
public:
	enum eActionFlags
	{
		eActionFlag_Unknown			= 0,
		eActionFlag_BundleChange	= 1<<0,
		eActionFlag_FppChange		= 1<<1,
		eActionFlag_VipChange		= 1<<2,
		eActionFlag_VppChange		= 1<<3,
	};
	UserFppBundleVppNotifyData	(	UINT64	vppMonthly_
								,	UINT64	vppYearly_
								,	INT16	vipStatus_
								,	INT32	fpp_
								,	BYTE	actionFlags_ 
								,	INT32	bundleChangeFpp_ 
								,	INT32	vppMonthlyCents_ 
								,	INT32	vppYearlyCents_ 
								,	INT32	fppBonus_ 
								)
								:	
									vppMonthly		(	vppMonthly_	)
								,	vppYearly		(	vppYearly_	)
								,	vipStatus		(	vipStatus_	)
								,	fpp				(	fpp_		)
								,	actionFlags		(	actionFlags_)
								,	bundleChangeFpp	(	bundleChangeFpp_)
								,	vppMonthlyCentsOnly	(	vppMonthlyCents_)
								,	vppYearlyCentsOnly	(	vppYearlyCents_)
								,	fppBonus		(	fppBonus_)
	{
	}
	UserFppBundleVppNotifyData( CommMsgParser& parser )
	{
		parse( parser );
	}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeUINT64	(	vppMonthly	)
			.composeUINT64	(	vppYearly	)
			.composeINT16	(	vipStatus	)
			.composeINT32	(	fpp			)
			.composeBYTE	(	actionFlags	)
			.composeINT32	(	bundleChangeFpp )
			.composeINT32	(	vppMonthlyCentsOnly )
			.composeINT32	(	vppYearlyCentsOnly )
			.composeINT32	(	fppBonus )
			;

		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT64(	vppMonthly	)
			.parseUINT64(	vppYearly	)
			.parseINT16	(	vipStatus	)
			.parseINT32	(	fpp			)
			.parseBYTE	(	actionFlags	)
			.parseINT32	(	bundleChangeFpp )
			.parseINT32	(	vppMonthlyCentsOnly )
			.parseINT32	(	vppYearlyCentsOnly )
			.parseINT32	(	fppBonus )
			;
		return parser;
	}
	void print() const
	{
		PLog( "bundle: vm %" PRId64 " vy %" PRId64 " fpp %d vl %d a %d ch %d vmc %d vyc %d b %d"
			,vppMonthly, vppYearly, fpp, vipStatus, actionFlags, bundleChangeFpp, vppMonthlyCentsOnly, vppYearlyCentsOnly, fppBonus );
	}
	UINT64	vppMonthly	; // total monthly VPP (not including cents)
	UINT64	vppYearly	; // total yearly VPP (not including cents)
	INT16	vipStatus	; // current VIP status
	INT32	fpp			; // total fpp awarded
	BYTE	actionFlags	; // event type mask ()
	INT32	bundleChangeFpp; // fpp awarded since the last nudge (=delta)
	INT32	vppMonthlyCentsOnly; // total monthly VPP cents only par
	INT32	vppYearlyCentsOnly; // total yearly VPP cents only part
	INT32	fppBonus	; // fpp bonus paid on special occasion 
};

class UserFppBundlesForVipLevel
{
public:
	enum eSupernovaFlags
	{
		eSupernovaFlags_YearlyLevel = 1<<0,
		eSupernovaFlags_YearlyUser	= 1<<1,
	};

	bool isUseYearlyVpps() const
	{
		if( vipStatus == eVipSuperNova )
		{
			return 0 != ( supernovaFlags & eSupernovaFlags_YearlyLevel );
		}
		return vipStatus == eVipEliteSuperNova;
	}
	bool isSupernovaYearlyUser() const
	{
		return ( vipStatus == eVipSuperNova && 0 != ( supernovaFlags & eSupernovaFlags_YearlyUser ) );
	}
	UserFppBundlesForVipLevel(	UINT64	vppMonthly_
							 ,	UINT64	vppYearly_
							 ,	INT16	vipStatus_
							 ,	INT32	vppMonthlyCentsOnly_ 
							 ,	INT32	vppYearlyCentsOnly_ 
							 ,	INT32	fpp_ 
							 ,	INT8	supernovaFlags_
							 )
							 :	
							 	vppMonthly	(	vppMonthly_	)
							 ,	vppYearly	(	vppYearly_	)
							 ,	vppMonthlyCentsOnly	(	vppMonthlyCentsOnly_	)
							 ,	vppYearlyCentsOnly	(	vppYearlyCentsOnly_	)
							 ,	vipStatus	(	vipStatus_	)
							 ,	fpp			(	fpp_		)
							 ,	warningDays			( 2	)
							 ,	rewardPercent		( 0	)
							 ,	notificationPercent	( 0	)
							 ,	supernovaFlags	( vipStatus_ == eVipSuperNova ? supernovaFlags_ : 0 )
							 ,	nextLevelRewardPercent( 0 )
	{
	}
	UserFppBundlesForVipLevel( CommMsgParser& parser )
	{
		parse( parser );
	}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeUINT64	(	vppMonthly	)
			.composeUINT64	(	vppYearly	)
			.composeINT16	(	vipStatus	)
			.composeUINT32	(	fpp			)
			.composeINT32	(	warningDays	)
			.composeINT32	(	rewardPercent )
			.composeINT32	(	notificationPercent	)
			;
		composeVectorOfMessageBodiesOfComposableValues( vipLevelBundles, body );
		body
			.composeINT32	(	nextLevelRewardPercent	)
			.composeINT32	(	vppMonthlyCentsOnly	)
			.composeINT32	(	vppYearlyCentsOnly	)
			.composeINT8	(	supernovaFlags	)
			;

		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT64(	vppMonthly	)
			.parseUINT64(	vppYearly	)
			.parseINT16	(	vipStatus	)
			.parseINT32 (	fpp			)
			.parseINT32	(	warningDays	)
			.parseINT32	(	rewardPercent )
			.parseINT32	(	notificationPercent	)
			;
		parseVectorOfParsableMessageBodies( vipLevelBundles, parser );

		parser
			.parseINT32 ( nextLevelRewardPercent )
			.parseINT32 ( vppMonthlyCentsOnly )
			.parseINT32 ( vppYearlyCentsOnly )
			;
		if( !parser.parseEnded() ) // TODO: remove condition after Dec 2016
		{
			parser.parseINT8( supernovaFlags );
		}
		return parser;
	}
	UINT64	vppMonthly	; // total monthly fpp (not including cents)
	UINT64	vppYearly	; // total yearly fpp (not including cents)
	INT32	vppMonthlyCentsOnly	; // total monthly fpp cents part only
	INT32	vppYearlyCentsOnly	; // total yearly fpp cents part only
	INT16	vipStatus	;
	INT32	fpp			;
	
	INT32 warningDays;
	INT32 rewardPercent;
	INT32 notificationPercent;
	INT8  supernovaFlags;
	
	vector<UserFppBundleSingleLevel> vipLevelBundles;
	INT32 nextLevelRewardPercent;
};

#endif // FppBundlesCli_h_included
