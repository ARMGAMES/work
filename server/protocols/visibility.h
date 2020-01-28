////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef	visibility_h_included
#define	visibility_h_included

#include "CliFeatureVisibility.h"

#ifndef	PROTOCOL_CLIENT_ONLY

#include "shared.h"
#include "PBitmask.h"
#include "sitemasks.h"

// PYR-37929
// Do not change this size without updating ExtendedFlag::composeForPublication() and updating the ATF GlobalPublisherClient::ATFExtendedFlag definition in protocols/globalpublisher/globalpublisherclientprot.txt
static const size_t EXTENDED_FLAG_COUNT = 2;

struct ExtendedFlag
{
	UINT64 flags[EXTENDED_FLAG_COUNT];

	ExtendedFlag();
	void setBit(INT32 index, UINT64 value);
	void clearBit(INT32 index, UINT64 value);
	void compose(CommMsgBody& msg) const;
	void composeForPublication( UINT64& featureMask, UINT64& featureMask2 ) const;
	void clear();
	void add(const ExtendedFlag& other); 
	void clear(const ExtendedFlag& other); 
	PString toString() const;
};

// PYR-32027  Client Visibility parsing moved from lobbydata.h 

//	PYR-29125
class ClientVisibilityRules
{
	struct Description
	{
		INT8 index; // QWORD index in the ExtendedFlag
		UINT64 feature;
		const char* desc;
	};

public:

	static void setClientVisibilityByName(const char* name, ExtendedFlag& flags)
	{
		static const Description descriptions[] = 
		{
			{ 0, eSocialMediaUser,							"SocialMediaUser"							},
			{ 0, eViewLinkCode,								"ViewLinkCode"								},
			{ 0, eFullTiltLink,								"FullTiltLink"								},
			{ 0, eFullTiltTransfer,							"FullTiltTransfer"							},
			{ 0, eFutureStarCode,							"FutureStarCode"							},
			{ 0, eNoSupernovaElite,							"NoSupernovaElite"							}, // PYR-38773
			{ 0, eHallOfFameLevels,							"HallOfFameLevels"							},
			{ 0, eRealMoneyFormDepositAssist,				"RealMoneyFormDepositAssist"				}, // PYR-30039
			{ 0, eBrowserEmbedded,							"BrowserEmbedded"							},
			{ 0, eCashierBrowserEmbedded,					"CashierBrowserEmbedded"					},
			{ 0, eFreemium,									"Freemium"									},
			{ 0, eFreemiumMobile,							"FreemiumMobile"							},
			{ 0, eNetverifyAllowAlternative,				"NetverifyAllowAlternative"					},
			{ 0, eCashierCloseDepositAssist,				"CashierCloseDepositAssist"					},
			{ 0, eAppCloseDepositAssist,					"AppCloseDepositAssist"						},
			{ 0, eFreerollDepositAssist,					"FreerollDepositAssist"						},
			{ 0, eWebContactSupport,						"WebContactSupport"							},
			{ 0, eLobbyEventTabSupport,						"LobbyEventTabSupport"						},
			//{ 0, eGuestAccountEnabled,						"GuestAccountEnabled"						}, // obsolete by PYR-91342
			{ 0, eShowAffiliateTrackOnCreate,				"ShowAffiliateTrackOnCreate"				},
			{ 0, eLoginViaEmailEnabled,						"LoginViaEmailEnabled"						},
			{ 0, eWebTokenCustomerSupportBrowserEmbedded,	"CustomerSupportBrowserEmbedded"			},
			{ 0, eDisplayClientDevice,						"DisplayClientDevice"						},
			{ 0, eWebTokenFreemiumBrowserEmbedded,			"FreemiumBrowserEmbedded"					},
			{ 0, eHelpSupportPortal,						"HelpSupportPortal"							},
			{ 0, eMobileAdXSupport,							"MobileAdXSupport"							},
			{ 0, eExchangeRateLabel,						"ExchangeRateLabel"							},
			{ 0, eSpinAndGo,								"SpinAndGo"									}, // PYR-28752
			//{ 0, eInClientPasswordReset,					"InClientPasswordReset"						}, // obsolete with PYR-52410
			{ 0, eIceFeatureEnabled,						"IceFeatureEnabled"							}, // PYR-30268
			{ 0, eCasinoNonAuth,							"CasinoNonAuth"								}, // PYR-29533
			{ 0, eCasinoAuth,								"CasinoAuth"								}, // PYR-29533
			{ 0, eCasinoLimits,								"CasinoLimits"								}, // PYR-29581
			{ 0, eMobileV2Support,							"MobileV2Support"							}, // PYR-29579
			{ 0, ePlayFulltiltPoker,						"PlayFulltiltPoker"							}, // PYR-30114
			{ 0, eGeoAssist,								"GeoAssist"									},
			{ 0, eDeclineDepositAssist,						"decline-deposit-assist"					}, // PYR-30985
			{ 0, eSpinGoNewFlagShown,						"SpinGoNewFlagShown"						}, // PYR-31618
			{ 0, eRealMoneyFormCloseDepositAssist,			"RealMoneyFormCloseDepositAssist"			}, // PYR-32416
			{ 0, eLandingScreenTest,						"LandingScreenTest"							}, // PYR-32613
			{ 0, eSportsBookNonAuth,						"SportsBookNonAuth"							}, // PYR-32578
			{ 0, eSportsBookAuth,							"SportsBookAuth"							}, // PYR-32578
			{ 0, eSportsBookLimits,							"SportsBookLimits"							}, // PYR-32578
			{ 0, eSportsBookOptions,						"SportsBookOptions"							}, // PYR-32734
			{ 0, eCashierDepositAssist,						"Deposit-Assistance-Main-Cashier"			}, // PYR-32704
			{ 0, eCashierTimeoutDepositAssist,				"CashierTimeoutDepositAssist"				}, // PYR-33064
			{ 0, eClientTimeoutDepositAssist,				"ClientTimeoutDepositAssist"				}, // PYR-33064
			{ 0, eSportsBookInBrowser,						"SportsBookInBrowser"						}, // PYR-32578
			{ 0, eVIPStoreMobile,							"VIPStoreMobile"							}, // PYR-28871
			{ 0, eCasinoLiveGames,							"CasinoLiveGames"							}, // PYR-33609
			{ 0, eIceLeaderboard,							"IceLeaderboard"							}, // PYR-33416
			{ 0, eShowSentForProcessingNoRMOK,				"ShowSentForProcessingNoRMOK"				}, // PYR-33814
			{ 0, eFrRequireIBAN,							"FrRequireIBAN"								}, // PYR-33448
			{ 0, eInstantBonuses,							"InstantBonuses"							}, // PYR-31838
			{ 0, eBotpLeaderBoard,							"BotpLeaderBoard"							}, // PYR-34826
			{ 0, e3DSecureBrowserEmbedded,					"3DSecureBrowserEmbedded"					}, // PYR-34066
			{ 0, eFppBundles,								"FppBundles"								}, // PYR-32027
			{ 0, eSendDocumentsVisible,						"SendDocumentsVisible"						}, // PYR-34619
			{ 0, eDFSNonAuth,								"DFSNonAuth"								}, // PYR-36466
			{ 0, eDFSAuth,									"DFSAuth"									}, // PYR-36466
			{ 0, eSpainVault2,								"SpainVault2"								}, // PYR-36688 - can be reused in Y2016
			{ 0, eWebRealMoneyForm,							"WebRealMoneyForm"							}, // PYR-36624
			{ 0, eAccountCreationDisabled,					"AccountCreationDisabled"					}, // PYR-36811

			{ 1, eKOTabQSSNK,								"KOTabQSSNK"								}, // PYR-40292
			{ 1, eKOTabsDefaultLoad,						"KOTabsDefaultLoad"							}, // PYR-39944
			{ 1, eWebCashier2,								"WebCashier2"								}, // PYR-40292
			{ 1, eLiveChatSupport,							"LiveChatSupport"							}, // PYR-39787
			{ 1, eKOTabQSScheduled,							"KOTabQSScheduled"							}, // PYR-40292
			{ 1, eKOTabAGSNK,								"KOTabAGSNK"								}, // PYR-40292
			{ 1, eKOTabAGScheduled,							"KOTabAGScheduled"							}, // PYR-40292
			{ 1, eWebPasswordReset,							"WebPasswordReset"							}, // PYR-41017
			{ 1, eWebCashier2deposit,						"WebCashier2deposit"						}, // PYR-40292
			{ 1, eZoomTabDisable,							"ZoomTabDisable"							}, // PYR-43016
			{ 1, eVegasNonAuth,								"VegasNonAuth"								}, // PYR-42295
			{ 1, eVegasAuth,								"VegasAuth"									}, // PYR-42295
			{ 1, eEasyLobby,								"EasyLobby"									}, // PYR-40531
			{ 1, eBTCTabQSSNG,								"BTCTabQSSNG"								}, // PYR-43017
			{ 1, eBTCTabQSMTT,								"BTCTabQSMTT"								}, // PYR-43017
			{ 1, eBTCTabAllSNG,								"BTCTabAllSNG"								}, // PYR-43017
			{ 1, eBTCTabAllMTT,								"BTCTabAllMTT"								}, // PYR-43017
			{ 1, eBTCTabsDefaultLoad,						"BTCTabsDefaultLoad"						}, // PYR-43017
			{ 1, ePublicLiveChatSupport,					"PublicLiveChatSupport"						}, // PYR-45817
			{ 1, eFreeSpinsBonuses,							"FreeSpinsBonuses"							}, // PYR-44841
			{ 1, eMiniGameTickets,							"MiniGameTickets"							}, // PYR-46074
			{ 1, eCvl,										"CVL"										}, // PYR-45659
			{ 1, eSpinAndGoOmaha,							"SpinAndGoOmaha"							}, // PYR-50211
			{ 1, eSpinAndGoMax,								"SpinAndGoMax"								}, // PYR-51311
			{ 1, ePowerUpNewFlagShown,						"PowerUpNewFlagShown"						}, // PYR-52434
			{ 1, eTempPinPerLogin,							"TempPinPerLogin"							}, // PYR-76895
// 			{ 1, eSkybetSportsTab,							"SkybetSportsTab"							}, // PYR-92704  - Can be reused in the future. Business' decision.
			{ 1, eAllInCashOut,								"AllInCashOut"								}, // PYR-79935
			// NOTE: Please add new client features below here
		};

		for ( INT32 i = 0; i < SZARR(descriptions); ++i )
		{
			if (PString::compareIgnoreCase(name, descriptions[i].desc) == 0)
			{
				flags.setBit(descriptions[i].index, descriptions[i].feature);
				return;
			}
		}

		PLog("Error: unknown client visibility: '%s'", name);
	}
};

//	PYR-27500
class ClientFeaturesVisibilityBase
{
	static bool analyseSiteName(const PString& siteEx, UINT32& siteId, UINT32& platformId, UINT32& brandId);
	static void convertStringToBitmask(const PString& flags, ExtendedFlag& maskOn, ExtendedFlag& maskOff);
	static bool isNumericValue(const char* str);

protected:

	struct INISiteData
	{
		UINT32 siteId;
		UINT32 platformId;
		UINT32 brandId;
		ExtendedFlag featuresMask;
		ExtendedFlag featuresMaskOff;
	};

	static void setVisibilityBit(const char* input, ExtendedFlag& mask);
	static void readINIData(const IniDataFromDbm::Section *section, vector<INISiteData>& iniSiteData);
};

// PYR-24031
class ClientFeaturesVisibility: protected ClientFeaturesVisibilityBase
{
	struct SiteFeaturesVisibility
	{
		UINT32 brandId;
		UINT32 siteId;
		UINT32 featuresMask;
		UINT32 originalINIDataFeaturesMask;
	};
	vector<SiteFeaturesVisibility> siteFeaturesVisibilities;

public:
	void init(const IniDataFromDbm& iniData);
	void compose(CommMsgBody& msg) const;
	bool setFeatureForSite( UINT32 siteId, UINT32 feature, bool enabled );
};

//	PYR-27500.  This class supersedes ClientFeaturesVisibility.
//	ClientFeaturesVisibility will eventually be removed.
class ClientFeaturesVisibilityEx: protected ClientFeaturesVisibilityBase
{
	static void setFeature(UINT64& featuresMask, UINT64 originalINIDataFeaturesMask, UINT64 featureBit, bool enabled)
	{
		if ( enabled )
		{
			if ( originalINIDataFeaturesMask & featureBit )	//	Turn on only if it was originally on
			{
				//	Make sure we only turn on bits that were present in the original mask
				featuresMask |= (featureBit & originalINIDataFeaturesMask);
			}
		}
		else
		{
			featuresMask &= ~featureBit;
		}
	}

	static void setFeature(UINT64& featuresMask, UINT64 featureBit, bool enabled)
	{
		if ( enabled )
		{
			featuresMask |= featureBit;
		}
		else
		{
			featuresMask &= ~featureBit;
		}
	}

	class SiteFeaturesVisibility
	{
		struct ClientDeviceSpecificFeature
		{
			UINT32 clientDevice;	//	enum ClientDevice
			ExtendedFlag featuresMask;

			void composeForCompatiblePublication(CommMsgBody& body) const;
			void compose(CommMsgBody& body) const;
		};

		struct ClientTypeSpecificFeature
		{
			UINT32 clientType;	//	enum ClientType
			ExtendedFlag featuresMask;
			vector<ClientDeviceSpecificFeature> clientDeviceSpecificFeatures;

			void composeForCompatiblePublication(CommMsgBody& body) const;
			void compose(CommMsgBody& body) const;
		};

		struct ClientPlatformSpecificFeature
		{
			UINT32 clientPlatform;	//	enum ClientPlatform
			ExtendedFlag featuresMask;
			UINT64 originalINIDataFeaturesMask;
			vector<ClientTypeSpecificFeature> clientTypeSpecificFeatures;

			void setFeature( UINT64 featureBit, bool enabled );
			void setFeatureEx( UINT64 featureBit, INT32 index, bool enabled );
			void composeForCompatiblePublication(CommMsgBody& body) const;
			void compose(CommMsgBody& body) const;
		};

		vector<ClientPlatformSpecificFeature> clientPlatformSpecificFeatures;

		void getDefaultPlatformFeaturesMaskForSite(const vector<INISiteData>& iniSiteData, ExtendedFlag& mask) const;
		INT32 findPlatformFeaturesForBrand(UINT32 platformId) const;

	public:

		SiteFeaturesVisibility()
			: brandId(0)
			, siteId(0)
			, originalINIDataFeaturesMask(0)
		{
		}

		void composeForCompatiblePublication(CommMsgBody& body) const;
		void compose(CommMsgBody& body) const;
		void composeWithBrandId(CommMsgBody& body) const; // PYR-38530
		void initClientPlatformSpecificFeatures(const vector<INISiteData>& iniSiteData);
		void setFeature( UINT64 featureBit, bool enabled );
		void setFeatureEx( UINT64 featureBit, INT32 index, bool enabled );
		void getVisibilitiesForPlatform( UINT32 platformId, vector<UINT64>& visibilities ) const; //PYR-51551
		const vector<ClientPlatformSpecificFeature>& getClientPlatformSpecificFeatures() const { return clientPlatformSpecificFeatures; }

		UINT32 brandId;	// PYR-38530
		UINT32 siteId;	//	enum Sites
		ExtendedFlag clientFeaturesMask;
		UINT64 originalINIDataFeaturesMask;
	};

	struct CountryException	//	PYR-29125
	{
		PString country;
		ExtendedFlag featureMaskOn;
		ExtendedFlag featureMaskOff;
		PSiteMask siteVisibilityMask;
		UINT32 brandIdMask;

		CountryException(const char* country_, const PSiteMask& siteVisibilityMask_, UINT32 brandIdMask_)
			: country(country_)
			, siteVisibilityMask(siteVisibilityMask_)
			, brandIdMask(brandIdMask_)
		{
		}
		CountryException() : 
			brandIdMask(0)
		{
		}

		void setFeature(UINT64 feature, INT32 index, bool enabled)
		{
			if( enabled )
			{
				featureMaskOn.setBit(index, feature);
				featureMaskOff.clearBit(index, feature);
			}
			else
			{
				featureMaskOn.clearBit(index, feature);
				featureMaskOff.setBit(index, feature);
			}
		}
		void composeForCompatiblePublication(CommMsgBody& body) const;
		void compose(CommMsgBody& body) const;
		void composeWithBrandId(CommMsgBody& body) const; // PYR-38530
	};

	struct CountryMask
	{
		CountryMask(const PSiteMask& siteMask_, UINT32 brandMask_) : 
			siteMask(siteMask_), 
			brandMask(brandMask_) 
		{}
		PSiteMask siteMask;
		UINT32 brandMask;
	};

	void analyseMaskString(const PString& flags, ExtendedFlag& onMask, ExtendedFlag& offMask) const;
	bool isTokenNegative(PString& token) const;
	void getSiteVisibilityMask(PString& country, PSiteMask& siteVisibilityMask) const;
	void analyseCountryString(const PString& inputString, vector< pair<PString, CountryMask> >& countrySiteMask) const;
	void tokenizeCountries(const char* input, vector<PString>& countries) const;

	vector<SiteFeaturesVisibility> siteFeaturesVisibilities;
	vector< CountryException >     countryExceptions;			//	PYR-29125

public:

	void init(const IniDataFromDbm& iniData);
	void composeWithoutBrand(CommMsgBody& msg) const;
	void composeWithBrandId(CommMsgBody& msg) const; // PYR-38530
	void composeForCompatiblePublication(CommMsgBody& msg) const;
	bool setFeatureForSite( UINT32 siteId, UINT64 feature, bool enabled );
	void getVisibilities( UINT32 clientSiteId, const char* country, vector<UINT64>& visibilities, UINT32 brandId=BrandType_None, UINT32 platformId=Client_UnknownPlatform ) const;	//	PYR-41017
	void setFeatureForSiteCountry( UINT32 siteId, const vector<PString>& countries, UINT64 feature, INT32 index, bool enabled );
	const vector<SiteFeaturesVisibility>& getSiteFeaturesVisibilities() const { return siteFeaturesVisibilities; }
	const vector<CountryException>& getCountryExceptions() const { return countryExceptions; }
};

#endif	//PROTOCOL_CLIENT_ONLY

#endif	//visibility_h_included
