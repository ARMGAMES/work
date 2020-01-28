//
//  PlayChipsDefs.h
//  PokerStars
//
//

#ifndef PlayChipsDefs_h
#define PlayChipsDefs_h

struct PlayChipsConfigStore
{
	UINT32 configId;
	UINT32 storeId;
	PString productId;
	
	PlayChipsConfigStore()
	: configId(0)
	, storeId(0)
	{}
	
	void parse(CommMsgParser& parser){
		
		const char * tmp;
		
		parser
		.parseUINT32(configId)
		.parseUINT32(storeId)
		.parseString(tmp);
		
		productId.assign(tmp);
	}
	
	void compose(CommMsgBody& body) const {
		body
		.composeUINT32(configId)
		.composeUINT32(storeId)
		.composeString(productId.c_str());
	}
};

// PYR-26426
struct PlayChipsConfig
{
	UINT32 configId;
	INT64 amount;
	INT64 playChipsAmount;
	UINT32 payTier;
	SrvTime whenCreated;
	UINT32 createdBy;
	SrvTime whenObsoleted;
	UINT32 obsoletedBy;
	UINT32 platform;
	UINT16 popular;
	vector<PlayChipsConfigStore> storeConfigs;
	
	PlayChipsConfig()
	: configId(0)
	, amount(0)
	, playChipsAmount(0)
	, payTier(0)
	, createdBy(0)
	, obsoletedBy(0)
	, platform(Client_UnknownPlatform)
	, popular(0)
	{}
	
	void parse(CommMsgParser& parser)
	{
		UINT32 numStores;
		parser
		.parseUINT32(configId)
		.parseINT64(amount)
		.parseINT64(playChipsAmount)
		.parseUINT32(payTier)
		.parseSrvTime(whenCreated)
		.parseUINT32(createdBy)
		.parseSrvTime(whenObsoleted)
		.parseUINT32(obsoletedBy)
		.parseUINT32(platform)
		.parseUINT16(popular)
		.parseUINT32(numStores)
		;
		int i=0;
		for(i=0; i < numStores; i++){
			PlayChipsConfigStore configStore;
			configStore.parse(parser);
			storeConfigs.push_back(configStore);
		}
	}
	void compose(CommMsgBody& body) const
	{
		body
		.composeUINT32(configId)
		.composeINT64(amount)
		.composeINT64(playChipsAmount)
		.composeUINT32(payTier)
		.composeSrvTime(whenCreated)
		.composeUINT32(createdBy)
		.composeSrvTime(whenObsoleted)
		.composeUINT32(obsoletedBy)
		.composeUINT32(platform)
		.composeUINT16(popular)
		.composeUINT32(storeConfigs.size())
		;
		int i=0;
		for(i=0; i < storeConfigs.size(); i++)
			storeConfigs[i].compose(body);
	}
};

// PYR-26426
struct PlayChipsTrans
{
	UINT32 purchaseId;
	UINT32 parentId;
	UINT64  amount;
	UINT64  playChipsAmount;
	PString gateway;
	PString status;
	PString errCode;
	PString errInfo;
	PString extRefId;
	SrvTime started;
	SrvTime completed;
	PString userId;
	UINT64  auditId;
	UINT32 userIntId;
	UINT32 paywallId;		// PYR-48920
	INT16 paywallVersionId;	// PYR-48920
	UINT32 segmentId;		// PYR-48920
	INT16 segmentVersionId;	// PYR-48920
	UINT32 bundleId;		// PYR-48920
	INT16 bundleVersionId;	// PYR-48920

	
	PlayChipsTrans()
	: purchaseId(0)
	, parentId(0)
	, amount(0)
	, playChipsAmount(0)
	, auditId(0)
	, userIntId(0)
	, paywallId(0)			// PYR-48920
	, paywallVersionId(0)	// PYR-48920
	, segmentId(0)			// PYR-48920
	, segmentVersionId(0)	// PYR-48920
	, bundleId(0)			// PYR-48920
	, bundleVersionId(0)	// PYR-48920
	{}
	void parse(CommMsgParser& parser)
	{
		parser
		.parseUINT32(purchaseId)
		.parseUINT32(parentId)
		.parseUINT64(amount)
		.parseUINT64(playChipsAmount)
		.parseStringP(gateway)
		.parseStringP(status)
		.parseStringP(errCode)
		.parseStringP(errInfo)
		.parseStringP(extRefId)
		.parseSrvTime(started)
		.parseSrvTime(completed)
		.parseStringP(userId)
		.parseUINT64(auditId)
		.parseUINT32(userIntId)
		;

		if( !parser.parseEnded() )
		{
			parser
				.parseUINT32(paywallId)
				.parseINT16(paywallVersionId)
				.parseUINT32(segmentId)
				.parseINT16(segmentVersionId)
				.parseUINT32(bundleId)
				.parseINT16(bundleVersionId)
				;
		}
		else
		{
			PLog("PlayChipsTrans::parse old client, no paywall info");
		}
	}
	void compose(CommMsgBody& body) const
	{
		body
		.composeUINT32(purchaseId)
		.composeUINT32(parentId)
		.composeUINT64(amount)
		.composeUINT64(playChipsAmount)
		.composeString(gateway)
		.composeString(status)
		.composeString(errCode)
		.composeString(errInfo)
		.composeString(extRefId)
		.composeSrvTime(started)
		.composeSrvTime(completed)
		.composeString(userId)
		.composeUINT64(auditId)
		.composeUINT32(userIntId)
		.composeUINT32(paywallId)		// PYR-48920
		.composeINT16(paywallVersionId)	// PYR-48920
		.composeUINT32(segmentId)		// PYR-48920
		.composeINT16(segmentVersionId)	// PYR-48920
		.composeUINT32(bundleId)		// PYR-48920
		.composeINT16(bundleVersionId)	// PYR-48920
		;
		;
	}
};

#endif /* PlayChipsDefs_h */
