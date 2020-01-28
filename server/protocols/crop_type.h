#ifndef crop_type_h_included
#define crop_type_h_included

enum DbmCropType
{
	//typeLeagueUsers     =-1,
    // do NOT change the exisiting sequence numbers, even if a job is no longer needed !!!
	typeNone            = 0,
	//typePlayMoney       = 1, // obsolete PYR-75994
	//typeRealMoney       = 2, // obsolete PYR-75994
	//typeZeroHandsPlay   = 3, // obsolete PYR-75994
	//typeZeroHandsReal   = 4, // obsolete PYR-75994
	typeHistPro         = 5,
	//typeFreeRoll        = 6, // obsolete PYR-75994
	//typeZeroFreeRoll    = 7, // obsolete PYR-75994
	//typeAudit           = 8, // obsolete PYR-75994
/* obsolete after PYR-26147
	typeCardNumEncrypt  = 9,
	typeCardNumEncrypt1 = 10,
	typeCardNumEncrypt2 = 11,
*/
	typeChatCrop        = 12, // obsolete
	typeDailyUsersStats = 13,
/* obsolete after PYR-26147
	typeCardNumDemask   = 14,
	typeCardNumMask     = 15,
*/
	typeLoggedout       = 16,
	typeTourns          = 17,
	typePMSnGOTourns    = 18,
	typeDailyActivityStats  = 19,
	typeTrackCodes      = 20,
	//typeAllAudit        = 21, // obsolete PYR-75994
	typeCashtransStats  = 22,
	typeProcsStats      = 23,
    typeStaleCleared    = 24,
    typeStaleUnCleared  = 25,
	//typeExpiredFpp      = 26, // obsolete by #17728
    typeCashtransCleared  = 27,
	typeCashtransfers	= 28, // PYR-13857 , PYR-25875
	//typeCardNumHash   = 29, // Obsoleted by PYR-82528 // #17888 
	typeStaleUsers    = 30, // #20399; job=staleusers; cropPortion=1_user
	typeFrBankAccountsDecrypt = 31, // #23636; job=frbankaccountsdecrypt, one time job, 'numDays' value is dummy and ignored
	typeCashConversion = 32, // PYR-25875
	typeCashtrans4Upd = 33, // PYR-26345
	typePayGreekUserDailyTax = 34, // PYR-28936
	typeCashtransPMCleared = 35, //PYR-45574 

	typeTlbPoints = 100,
	//typeBOPTlbPoints = 101,
    typeAffiliateStats = 102,
	typeAffiliateBackdateStats = 103,
	typeHandsStats = 104,
	typeFppDeductionStats = 105,
	typeSpecialBonusesStats = 106,
    typeAffiliateSummary = 107,
    typeRecalculateAffiliateStats = 108,
	typeDailyGameStats = 109,
	typeMonthlyGameStats = 110,
	typeTournStats = 111,
	typeTournUserStats = 112,
	typePersistence = 113,
	typeTlbEvents = 114,
	typeCrmTourns = 115,
	typeTournHandsStats = 116,
	typeTournAAMS = 117,
	typeTableAAMS = 118,
	typeReportUserSummaryAAMS = 119,
	typeDailyFppBalance = 120,
	typeTournResultsStats = 121,
	typeExpiredTournTicketCrop = 122,
	typeObsoleteTournTicketCrop = 123,
	typeUserTax = 124,
	typeTlbPointsItalian = 125,	
	typeClickStats = 126,
	typeTlbPointsFrench = 127,	
	typeRakeStats = 128,
	typeHomeGamesTournResultStats = 129,
	typeTempHands = 130,
	typeUserEvents0 = 131,
	typeAffUserEvents = 132,
	typeInboundMailStats = 133,
	typeInboundCsrMailStats = 134,
	typeOutboundCsrMailStats = 135,
    typeDeleteLeague = 136, // PYR-16389
	typeHomeGamesSeasonEndStats = 137, // PYR-17081
	typeTournsRootAllNotNormalOrShowing = 138, // PYR-18137
	typeAAMS830 = 139, // PYR-18850
	typeAAMS830Tables = 140, // PYR-18850
	typeAAMSResubmit790 = 141,
	typePersistence2 = 142, // fix for PYR-14870; DAEMONLOG.CROPTYPE is not initialized - CropJobHandler::saveCropDate is never called for this handler
	typeAAMSCollectTableTicketTimeInfo = 143, //PYR-20074
	typeExpiredTemplates = 144, // PYR-19875
	typeTableAAMS_2 = 145,
	typeUserGroupVpp = 146, // PYR-20709
	typeWindowsInfoUser = 147, // PYR-20205
	typeAAMSResubmit790_2 = 148,
	typeExtLbtyUsrMoneyOnTblRecur = 149,
	typeMsgTriggerRecur = 150,
	typeTournSumStats = 151,
	typeCashoutStats = 152,
    typeCleanUsersScreenFlagCropper = 153, // PYR-16913
	typeEETournSummary = 154,
	typeRGRingVpp = 155,
	typeTableAAMS_1=156,
	// typeTableAAMS_2 - defined above
	typeTableAAMS_3=157,
	typeTableAAMS_4=158,
	typeTableAAMS_5=159,
	typeAAMSResubmit790_1=160,
	// typeAAMSResubmit790_2 - defined above
	typeAAMSResubmit790_3=161,
	typeAAMSResubmit790_4=162,
	typeAAMSResubmit790_5=163,
	typeTableAAMSBlitz_1=164,
	typeTableAAMSBlitz_2=165,
	typeAffiliateCounters=166,
	typePlayerPmStats = 167,
	typePlayerPmActivityStats = 168,
	typeExtHandsPm = 169,
	typePlayerPmBalanceStats = 170,
	typeFTTransfers = 171,
	typeFTBalances = 172,
	typeMonthLTFPPBalances = 173,
	typeHomeGamesSetArchivePending = 174,		//	PYR-24340
	typeHomeGamesArchiveClubs = 175,			//	PYR-24340
	typeRMOKInit = 176,			                //	PYR-25735
	typeFTTaskForce = 177,			            //	PYR-25698
	typeAffiliateStatsFT = 178,                 //  PYR-25706
	typeAffiliateSummaryFT = 179,               //  PYR-25706
	typeRecalculateAffiliateStatsFT = 180,      //  PYR-25706 
    typeRecalculateAffiliateStatsAms2 = 181,    //  PYR-25706 
	typeTrackCodes_UseTCFlags = 182,			//	PYR-25971
	typeFlightSummaryStats = 183,			    //	PYR-25579
	typeFlightDetailStats = 184,			    //	PYR-25579
	typeSngVpp = 185,						    //	PYR-26496
	typeTournProps = 186,						//	PYR-26801
	typeTlbPointsUSNJ = 187,                    //  PYR-27899
	typeUserDailyNJ = 188,                      //  PYR-28306
	typeTournTaxNJ = 189,                    
	typeLimitedEntryTourns = 190,				//	PYR-28521

	typeRecryptCardNums = 191,                  //  PYR-28860
	typeRecryptCardInfo = 192,                  //  PYR-28860
	typeRecryptUserFiscalEncr = 193,            //  PYR-28860
	typeRecryptGcDebitData = 194,               //  PYR-28860

	typeBGStats            = 195,               //  PYR-30954
	typeCasinoStats        = 196,               //  PYR-30650
	typeDailyCPPBalances   = 197,               //  PYR-30650
	typeMonthCPPBalances   = 198,               //  PYR-30650
	typeDailyCPPBalancesAmsst = 199,            //  PYR-30650
	typeMonthCPPBalancesAmsst = 200,            //  PYR-30650
	typeBGRingBets = 201,                       //  PYR-31293
	typeBGTournBets = 202,                      //  PYR-31293
	typeExtHandsNJ = 203,
	typeExtHandsPmNJ = 204,
	type_PS_Poker_Player_Stats     = 205, //PYR-31733
	type_PS_Poker_Affiliate_Stats  = 206, //PYR-31733
	type_PS_Poker_Stats_Recalc     = 207, //PYR-31733
	type_FT_Poker_Player_Stats     = 208, //PYR-31733
	type_FT_Poker_Affiliate_Stats  = 209, //PYR-31733
	type_FT_Poker_Stats_Recalc     = 210, //PYR-31733
	type_PS_Casino_Player_Stats    = 211, //PYR-31733
	type_PS_Casino_Affiliate_Stats = 212, //PYR-31733
	type_PS_Casino_Stats_Recalc    = 213, //PYR-31733
	type_FT_Casino_Player_Stats    = 214, //PYR-31733
	type_FT_Casino_Affiliate_Stats = 215, //PYR-31733
	type_FT_Casino_Stats_Recalc    = 216, //PYR-31733
	typeSagStatus				   = 217, //PYR-33150
	typeIceTasksCropper			   = 218, // PYR-31374
	typeExtLbtyUsrMoneyOnXCGame		= 219,	// PYR-34834
	typeExtLbtyUsrMoneyOnSportsBook	= 220,	// PYR-34834
	typeExtLbtyPendingCashOuts		= 221,	// PYR-33749
	typeExtLbtyPendingCashSnap		= 222,	// PYR-33749
	typeExtLbtyUsrDailyChips		= 223,	// PYR-33749
	typeIceBoardSeasonCropper		= 224, // PYR-37462

	typeDailyDeposits			    = 226, // PYR-36662
	//typeDuelStats			        = 227, // PYR-37105 // PYR-53789 - obsolete
	type_PS_Sports_Player_Stats     = 228, //PYR-37007
	type_PS_Sports_Affiliate_Stats  = 229, //PYR-37007
	typeSportsMoneyStats            = 230, //PYR-37007
	typeROStats                     = 231, //PYR-39611
	typeEESportsMoney               = 232, //PYR-40414
	type_NJ_RIBR_Stats              = 233, //PYR-40552
	typeSportBets                   = 234, // PYR-40503
	typeSportRejects                = 235, // PYR-40503
	typeSportEvents                 = 236, // PYR-40503
	typeSportMarkets                = 237, // PYR-40503
	typeSportCampaigns              = 238, // PYR-40503
	typeSportIModeCash              = 239, // PYR-40503
	typeSportIModeSpp               = 240, // PYR-40503
	typeSportExpiredFreeBets        = 241, // PYR-40503
	typeMiniGamesPlaysCleanup       = 242, // PYR-41592
	typeDailyFppBalanceBrand        = 243, //PYR-42348
	typeDailyCPPBalancesBrand       = 244, //PYR-42348
	typeDailyDepositsBrand          = 245, //PYR-42348
	typeXcTransInterrupted          = 246, //PYR-44085
    typeCasinoInterruptedStats      = 247, //PYR-44082
	typeW2GCasino                   = 248, //PYR-43432
	typeIceDoneTaskInstanceCropper	= 249, //PYR-41747
	typeDailyCPPBalancesProduct     = 250, //PYR-44604
	typeROOperatorFinAdjustment     = 251, //PYR-45085
	typeROFailedLogins              = 252, //PYR-45085
	typeROPlayerExclusions          = 253, //PYR-45085	
	typeROUserDaily                 = 254, //PYR-44913

	typeAffiliationReset			= 255, // PYR-36714	

	typeROTransOverEuroThreshold    = 256, //PYR-45085
	typeROBannedAccounts            = 257, //PYR-45085*
	typeROGameTypeCasino            = 258, //PYR-45085
	typeROGameTypeSports            = 259, //PYR-45085
	typeROGameRevenue               = 260, //PYR-45085
	typeROExcessiveWinnings         = 261, //PYR-45085
	typeROGameSessionsGeneral       = 262, //PYR-45085
	typeROGameSessionsSports        = 263, //PYR-45085
	typeROPlayerStatus              = 264, //PYR-45085
	typeCZTournSummary              = 265, //PYR-48613
	typeLTCasino                    = 266, //PYR-50204
	typeLTSports                    = 267, //PYR-50204
	typeAAMS831						= 268, // PYR-49537
	typeRingGamesAAMS				= 269, // PYR-63224
	typeExtendedTournUniversalLiab  = 270, // PYR-60173 - not a deamon job - just a marker
	typeSBInitialEvents             = 271, // PYR-69656
	typeSBDailyActivity             = 272, // PYR-76538
	typeROJackpotConfig             = 273, // PYR-56444
	typeROJackpotWinnings           = 274, // PYR-56444
	typeROJackpotSummary            = 275, // PYR-56444
	typeSportsW2G                   = 276, // PYR-76562
	typeAffiliateCleanup            = 277, // PYR-76617
	typeSEWinLoss                   = 278, // PYR-82034
	typeUK_SE_Csv1                  = 279, // PYR-82502
	typeUK_SE_Csv2                  = 280, // PYR-82502
	typeUK_SE_Csv3                  = 281, // PYR-82502
	typeUserDaily                   = 282, // PYR-99782
	typeXCJPDeposit                 = 283, // PYR-103523
	typeXCJPPayout                  = 284, // PYR-103523
	typeFullTiltMigrUsersGDPR		= 285, // PYR-84803
	typePACAsinoMoneyStats          = 286, // PYR-111423
	typePACAsinoMoneyStatsCarry     = 287, // PYR-111423
	typePAHandsStats                = 288, // PYR-111423
	typePARibrStats                 = 289, // PYR-111423
	typePASportsWsr                 = 290, // PYR-111423
	typePATournStats                = 291, // PYR-111423
	typePamUserMatchingBiomaa       = 292, // PYR-121466
	typePamUserMatchingSimple       = 293, // PYR-121466

	// add all types before this
	// a JIRA number in comment is a must
	//types between 500 and 2999 are reserved by DBAs
	typeReserved555 = 555,
	typeReserved556 = 556,
	typeReplicationHandsToItaly = 557,
	typeReserved559 = 559,
    typeFTTableLoad = 560, //PYR-25706 to amsst.daemonlog
	typeMissingTournProps = 570, //PYR-29320
	typeReserved571 = 571, 
	typeReserved572 = 572, 
	//Bulgaria export
	typeReserved573 = 573, 
	typeReserved574 = 574, 
	typeReserved575 = 575, 
	typeReserved576 = 576, 
	typeReserved577 = 577, 
	typeReserved578 = 578, 
	typeReserved579 = 579, 
	typeReserved580 = 580, 
	typeReserved581 = 581, //typeBGRingBets  #31293
	typeReserved582 = 582, //typeBGTournBets #31293	
	//end Bulgaria export
	typeReservedLiabilityReplica583 = 583,
	typeReservedCasinoReplicaMal = 584, //#32691
	typeReservedGeoIpReplica = 585, 
	typeReservedGeoCountryChange = 586, 
	typeReservedCasinoReplicaIta = 587,
	typeReserved588 = 588, //#41280 flightuserstatus replica to usersbg
	typeReserved589 = 589, //#48465 casino replica to usersbg
	typeReserved590 = 590, //#48465 sb replica to usersbg
	typeReserved591 = 591, //#48465 cz_tourn_stats replica to usersbg
	typeReserved592 = 592, //#52813 india replica, batchid
	typeReserved593 = 593, //#52813 tournresultsstats
	typeReserved594 = 594, //#52813 playermoneystats
	typeReserved595 = 595, //#52813 cashtransstats
	typeReserved596 = 596, //#52813 playertournhands
	typeReserved597 = 597, //#52813 tournstats
	typeReserved598 = 598, //#52813 handsstats
	typeReserved599 = 599, //#56089 bg-cz handssharedstats replica
	typeReserved600 = 600, //#78766 nj replica poker, job
	typeReserved601 = 601, //#78766 nj replica sports_wsr
	typeReserved602 = 602, //#78766 nj replica handsstats
	typeReserved603 = 603, //#78766 nj replica tournstats
	typeReserved604 = 604, //#78766 nj replica casinomoneystats
	typeReserved605 = 605, //#78766 nj replica njuserdaily
	typeReserved606 = 606, //#78766 nj replica sports time
	typeReserved607 = 607, //#78766 nj replica casino time
	typeReserved608 = 608, //#78766 nj replica playermoneystats
	typeReserved609 = 609, //#78766 nj replica cashtransstats
	typeReserved610 = 610, //#78766 nj replica tournresultsstats
    //
	typeReserved700 = 700, //#72928 pa replica poker, job
	typeReserved701 = 701, //#72928 pa replica sports_wsr
	typeReserved702 = 702, //#72928 pa replica handsstats
	typeReserved703 = 703, //#72928 pa replica tournstats
	typeReserved704 = 704, //#72928 pa replica casinomoneystats
	typeReserved705 = 705, //#72928 pa replica njuserdaily
	typeReserved706 = 706, //#72928 pa replica sports time
	typeReserved707 = 707, //#72928 pa replica casino time
	typeReserved708 = 708, //#72928 pa replica playermoneystats
	typeReserved709 = 709, //#72928 pa replica cashtransstats
	typeReserved710 = 710, //#72928 pa replica tournresultsstats
    //
	typeReservedLiabilityReplicaPA = 783,
	//
	typeReserved1013 = 1013, //be replica playermoneystats
	typeReserved1014 = 1014,
	typeDynCZNonTournamentActivity = 1015, //#56089
	typeDynHandsSharedStats = 1016, //#56089
	typeReserved1022 = 1022, //be replica cashtransstats
	typeReserved1104 = 1104, //be replica handsstats
	typeReserved1111 = 1111, //be replica tournstats
	typeReserved1112 = 1112, //be replica playertournhands
	typeTournResultsToMaltaMigration = 2121,	
};

#endif // crop_type_h_included
