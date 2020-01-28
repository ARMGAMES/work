#ifndef collusions_h_included
#define collusions_h_included

//Error Codes
#define COLLUSION_OK					0x0000
#define COLLUSION_GENERAL_ERROR			0x0001
#define COLLUSION_HAND_PARSE_ERROR		0x0002
#define COLLUSION_DETECT_ERROR			0x0003

#define COLLUSION_DELTA					0x1000

// C- means collusion server, T-means Table Server, A-means Admin, D-means DBM
#define COLLUSION_Q_HAND				0x1001
	//T->C, "817<s4s5>" handId,handType,maxPlayersAtHand<userId,colludeFlags,cards,delta>	

#define COLLUSION_Q_AVAIL_RULES			0x1004
	//A->C
#define COLLUSION_Q_ENABLED_RULES		0x1008
	//A->C
#define COLLUSION_Q_ENABLE_RULE			0x1010
	//A->C "41" Rule#,state
#define COLLUSION_Q_RELOAD_DYNAMIC		0x1020
	//A->C ""
#define COLLUSION_Q_CBMD_SET_PARAMS		0x1041
    //A->C
#define COLLUSION_Q_UPDATE_SEC_ALERT	0x1042
    //A->C 444s4 reqId alertId status comment type
#define COLLUSION_Q_CREATE_SEC_ALERT	0x1043
    //A->C 44ssss444 reqId type user1 user2 parameter comment sourceMask behaveMask score
#define COLLUSION_Q_UPDATE_ALERT_GROUP  0x1044
    //A->C 444s4 reqId groupId status comment type
#define COLLUSION_Q_TSS_SET_PARAMETER   0x1045
    //A->C
#define COLLUSION_Q_ALLOCATE_GROUP      0x1046
    //A->C 44s reqId groupId comment
#define COLLUSION_Q_DEALLOCATE_GROUP    0x1047
    //A->C 44s reqId groupId comment
#define COLLUSION_Q_RUG_SET_PARAMS      0x1048
    //A->C 444444444444bb reqId, RugScores, isUpdate, isPurge
#define COLLUSION_Q_RUG_ADD_EXCLUDED    0x1049
    //A->C 44ss reqId, type, value, comment (see struct RUGExcludeTypes in secdbm.h)
#define COLLUSION_Q_RUG_DEL_EXCLUDED    0x1050
    //A->C 44s reqId, type, value (see struct RUGExcludeTypes in secdbm.h)
#define COLLUSION_Q_TSS_MANAGE_RULE     0x1051
    //A->C
#define COLLUSION_Q_CREATE_RELATED_ALERT		0x1052
    //L->C 444<s> reqId tournId numUsers<userId>
#define COLLUSION_Q_FAILED_VERIFICATION			0x1053
    //G->C 4<s> reqId userId
#define COLLUSION_Q_CDA_SET_PARAMETER			0x1054
    //A->C
#define COLLUSION_Q_CDA_MANAGE_RULE				0x1055
    //A->C
#define COLLUSION_Q_TSS_ADD_HR_CARDS			0x1056
    //A->C 4ssss reqId cardNums, caseId, source, comments
#define COLLUSION_Q_LOGIN_SET_PARAMETER			0x1057
    //A->C
#define COLLUSION_Q_LOGIN_MANAGE_RULE			0x1058
    //A->C
#define COLLUSION_Q_TRAMAIL_SET_PARAMETER		0x1059
    //A->C
#define COLLUSION_Q_TRAMAIL_MANAGE_RULE			0x105A
    //A->C
#define COLLUSION_Q_HSMT_SET_PARAMETER			0x105B
    //A->C
#define COLLUSION_Q_HSMT_MANAGE_RULE			0x105C
    //A->C
#define COLLUSION_Q_SEND_PURCHASE				0x105D 
    //Cashier->C 4<TSSPurchase> reqId TSSPurchase (see secdbm.h)
#define COLLUSION_Q_LOGIN_SET_WHITELIST_PARAM	0x105E
	//A->C 44ssb reqId LoginWhitelistParam
#define COLLUSION_Q_NJ_MAA						0x105F
	//Csh->C 4ssds  reqId, userId, lastName, DOB, SSN_last4
#define COLLUSION_Q_ALLOCATE_ATASTRANS			0x1060  // PYR-27329
	//A->C 44s reqId transId comment
#define COLLUSION_Q_DEALLOCATE_ATASTRANS		0x1061
	//A->C 44s reqId transId comment
#define COLLUSION_Q_FREEMIUM_SET_PARAMETER		0x1062												// PYR-29348
#define COLLUSION_Q_FREEMIUM_MANAGE_RULE		0x1063
#define REGULATOR_Q_COLLUSION_REVAD_RESULT		0x1064												// PYR-31219	// PYR-28735
#define COLLUSION_Q_CREATE_CAS_ALERT			0x1065												// PYR-31624
	//PB->C 44444ss reqId type userIntId gameType variantType parameter comment
#define COLLUSION_Q_STR_SET_PARAMETER			0x1066												// PYR-24460
	//A->C 444444444444444444 reqId STRParameter
#define COLLUSION_Q_STR_SET_PSRULESCORES		0x1067												// PYR-24460
	//A->C 44<224> reqId numScores<STRRuleScore>
#define COLLUSION_Q_STR_GENERATE_ALERTS			0x1068												// PYR-24460
	//A->C //44<8>4444b reqId nTour<tournId> minPlace maxPlace minPrize minPrizePerBuyin chkSupRule
#define COLLUSION_Q_MANUAL_ACTIONS				0x1069												// PYR-26684
	//A->C See struct ManualActions
#define COLLUSION_Q_STR_MANUAL_ALERT_REPORT		0x106A												// PYR-36618
	//A->C //4tt44<8> reqId timeFrom timeTo reqStatus nTour<tournId>
#define COLLUSION_Q_ACCOUNT_SELF_LOCKING		0x1070												// PYR-34643
	//WS->C
#define HOLMES_Q_ALERT_EVENT					0x1071												// PYR-36683
	//C->Holmes
#define COLLUSION_Q_ALERT_EVENT			        0x1072												// PYR-37179
	//Cron->Collusions  4 AlertChangeEvent
#define CESAR_Q_ALERT_EVENT				        0x1073												// PYR-34256
	//Collusoin->Cesar reqId CesarAlertEvent
#define COLLUSION_Q_CREATE_CROSS_SITE_MAA       0x1074												// PYR-42376
    //IOM Collusion -> NJ Colluson reqId, CrossSiteMAAAlerts
#define COLLUSION_Q_DELETE_SEC_ALERT            0x1075												// PYR-42939
    //CRON -> Colussion
#define COLLUSION_Q_CREATE_FREUD_RULE			0x1076												// PYR-51363
	// Admin -> Collusion 4* - reqId FreudRule
#define COLLUSION_Q_UPDATE_FREUD_RULE			0x1077												// PYR-51363
	// Admin -> Collusion 4* - reqId FreudRule
#define COLLUSION_Q_FREUD_PREDICTION			0x1078												// PYR-51363
	// Freud -> Collusion 44* - reqId RuleId FreudPredictionEvent
#define COLLUSION_Q_REORDER_FREUD_RULES			0x1079												// PYR-51363
	// Admin -> Collusion 444<44> - reqId type num<ruleid, orderid> 
#define COLLUSION_Q_CRM_OFFER_APPLIED			0x107A	
	// lobbyetc -> Collusion 4ss - reqId userId offerCode
#define COLLUSION_Q_CREATE_BIOMAA_ALERT			0x107B												// PYR-58263
	//ApalBioMAA->C 4* reqId AlertGroupStruct
#define COLLUSION_Q_CREATE_HDWMAA_ALERT			0x107C												// PYR-58616
//ApalHdwMAA->C 4* reqId AlertGroupStruct
#define COLLUSION_Q_GEOLOC_ADD_RULE				0x107D												// PYR-66110
	// Admin -> Collusion 4* - reqId GeolocAlertRule
#define COLLUSION_Q_REORDER_GEOLOC_RULES		0x107E												// PYR-66110
	// Admin -> Collusion 44<s2> - reqId num<name, ordinal> 
#define COLLUSION_Q_GET_ALERT_BY_TYPE			0x107F												// PYR-58616
	// anyone to Collusion
#define COLLUSION_Q_CREATE_RUG_ALERT			0x1080												// PYR-58616
	//ApalHdwMAA->C
#define COLLUSION_Q_GET_USER_IN_PAIR_ALERTS_BY_TYPE		0x1081										// PYR-72656
	// anyone to Collusion
#define COLLUSION_Q_UPDATE_USER_RG				0x1082												// PYR-75546
	// CRM->C 44s444bb - reqId, userIntId, userId, oldAdminRiskLevel, oldSuggestedRiskLevel, newRiskLevel, isAdmin, isForced
#define COLLUSION_Q_MSTR_CREATE_CONFIG			0x1083												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_MSTR_MODIFY_CONFIG			0x1084												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_MSTR_ACTIVATE_CONFIG		0x1085												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_MSTR_DELETE_CONFIG			0x1086												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_MSTR_SET_PSRSCORES			0x1087												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_MSTR_GENERATE_ALERTS		0x1088												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_MSTR_MANUAL_ALERT_REPORT	0x1089												// PYR-50464
	// Admin -> Collusion
#define COLLUSION_Q_CLOSE_BATCH					0x1090												// PYR-64085
	// Security modules -> Collusion
#define COLLUSION_Q_TRA_SET_PARAMETER			0x1091												// PYR-70751
	//A->C
#define COLLUSION_Q_TRA_MANAGE_RULE				0x1092												// PYR-70751
	//A->C
#define COLLUSION_Q_CREATE_SEC_ALERT_GROUP		0x1093												// PYR-84380
	//CCDA->C

#define COLLUSION_A_HAND				0x2001
	//C->T, "41{s}" handId,err_code{err_dscp}	
#define COLLUSION_A_AVAIL_RULES			0x2004
	//C->A	"4<4ss1>" Rules Num<Rule#,Name,description,code>
#define COLLUSION_A_ENABLED_RULES		0x2008
	//C->A	"4<4ss1>" Rules Num<Rule#,Name,description,code>
#define COLLUSION_A_ENABLE_RULE			0x2010
	//C->A "411{s}" Rule#,state,err_code{err_descp}
#define COLLUSION_A_RELOAD_DYNAMIC		0x2020
	//C->A "1{s}" err_code{err_descp}
#define COLLUSION_A_CBMD_SET_PARAMS		0x2041
    //C->A
#define COLLUSION_A_UPDATE_SEC_ALERT	0x2042
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_CREATE_SEC_ALERT	0x2043
    //C->A 6[s|4] errCode [errStr]
#define COLLUSION_A_UPDATE_ALERT_GROUP  0x2044
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_TSS_SET_PARAMETER   0x2045
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_ALLOCATE_GROUP      0x2046
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_DEALLOCATE_GROUP    0x2047
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_RUG_SET_PARAMS      0x2048
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_RUG_ADD_EXCLUDED    0x2049
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_RUG_DEL_EXCLUDED    0x2050
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_TSS_MANAGE_RULE     0x2051
    //C->A
#define COLLUSION_A_CREATE_RELATED_ALERT		0x2052
    //L->C 6[s] errCode [errStr]
#define COLLUSION_A_FAILED_VERIFICATION			0x2053
    //C->G 6[s] errCode [errStr]
#define COLLUSION_A_CDA_SET_PARAMETER			0x2054
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_CDA_MANAGE_RULE				0x2055
    //C->A
#define COLLUSION_A_TSS_ADD_HR_CARDS			0x2056
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_LOGIN_SET_PARAMETER			0x2057
    //C->A 6[s] errCode [errStr]
#define COLLUSION_A_LOGIN_MANAGE_RULE			0x2058
    //C->A
#define COLLUSION_A_TRAMAIL_SET_PARAMETER		0x2059
    //A->C
#define COLLUSION_A_TRAMAIL_MANAGE_RULE			0x205A
    //A->C
#define COLLUSION_A_HSMT_SET_PARAMETER			0x205B
    //A->C
#define COLLUSION_A_HSMT_MANAGE_RULE			0x205C
    //A->C
#define COLLUSION_A_SEND_PURCHASE				0x205D 
    //C->Cashier 6[s] errCode [errStr]
#define COLLUSION_A_LOGIN_SET_WHITELIST_PARAM	0x205E
	//C->A 6[s] errCode [errStr]
#define COLLUSION_A_NJ_MAA						0x205F
	//C->Csh 2[s|4]  errCode [errDescr|matchMask]
#define COLLUSION_A_ALLOCATE_ATASTRANS			0x2060												// PYR-27329
	//C->A 6[s] errCode [errStr]
#define COLLUSION_A_DEALLOCATE_ATASTRANS		0x2061
	//C->A 6[s] errCode [errStr]
#define COLLUSION_A_FREEMIUM_SET_PARAMETER		0x2062												// PYR-29348
#define COLLUSION_A_FREEMIUM_MANAGE_RULE		0x2063
#define REGULATOR_A_COLLUSION_REVAD_RESULT		0x2064												// PYR-31219 // PYR-28735
#define COLLUSION_A_CREATE_CAS_ALERT			0x2065												// PYR-31624
	//C->PB 6[s|4] errCode [errStr|alertId]
#define COLLUSION_A_STR_SET_PARAMETER			0x2066												// PYR-24460
	//C->A 46[s] reqId errCode [errStr]
#define COLLUSION_A_STR_SET_PSRULESCORES		0x2067												// PYR-24460
	//C->A 42[s] reqId errCode [errString]
#define COLLUSION_A_STR_GENERATE_ALERTS			0x2068												// PYR-24460
	//C->A 42[s] reqId errCode [errString]
#define COLLUSION_A_MANUAL_ACTIONS				0x2069												// PYR-26684
	//C->A 42[s|s] (error code never returned)
#define COLLUSION_A_STR_MANUAL_ALERT_REPORT		0x206A												// PYR-36618
	//C->A 42[s|4<44444BB4s44tt>] reqId errCode [errString|numrows<STRManualOutputReport>]
#define COLLUSION_A_ACCOUNT_SELF_LOCKING		0x2070												// PYR-34643
	//C->WS
#define HOLMES_A_ALERT_EVENT					0x2071												// PYR-36683
	//Holmes->C
#define COLLUSION_A_ALERT_EVENT					0x2072												// PYR-37179
	//Collusions->Cron
#define CESAR_A_ALERT_EVENT				        0x2073												// PYR-34256
	//Cesar -> Collusion
#define COLLUSION_A_CREATE_CROSS_SITE_MAA       0x2074												// PYR-42376
	// NJ Collusion -> IOM Collusion
#define COLLUSION_A_DELETE_SEC_ALERT            0x2075												// PYR-42939
    // Colussion -> Cron
#define COLLUSION_A_CREATE_FREUD_RULE			0x2076												// PYR-51363
	// Collusion -> Admin 42[s|4] reqId errCode [errString|ruleId]
#define COLLUSION_A_UPDATE_FREUD_RULE			0x2077												// PYR-51363					
	// Collusion -> Admin 42[s] reqId errCode [errString]
#define COLLUSION_A_FREUD_PREDICTION			0x2078												// PYR-51363
	// Collusion -> Freud 42[s|4] reqId errCode [errString|ruleId]
#define COLLUSION_A_REORDER_FREUD_RULES			0x2079												// PYR-51363
	// Collusion -> Admin 42[s] reqId errCode [errString]
#define COLLUSION_A_CRM_OFFER_APPLIED			0x207A	
	// Collusion -> lobbyetc 42[s] reqId errCode [errString]
#define COLLUSION_A_CREATE_BIOMAA_ALERT			0x207B												// PYR-58263
	// Collusion -> ApalBioMAA 42[s|44<*>] reqId errCode [errString|groupId nAlerts<AlertRecord>]
#define COLLUSION_A_CREATE_HDWMAA_ALERT			/*0x207C*/ (COLLUSION_Q_CREATE_HDWMAA_ALERT + COLLUSION_DELTA)								// PYR-58616
// Collusion -> ApalHdwMAA 42[s|44<*>] reqId errCode [errString|groupId nAlerts<AlertRecord>]
#define COLLUSION_A_GEOLOC_ADD_RULE				/*0x207D*/ (COLLUSION_Q_GEOLOC_ADD_RULE + COLLUSION_DELTA)									// PYR-66110
	// Collusion -> Admin 6[s] errCode [errStr]
#define COLLUSION_A_REORDER_GEOLOC_RULES		/*0x207E*/ (COLLUSION_Q_REORDER_GEOLOC_RULES + COLLUSION_DELTA)								// PYR-66110
	// Collusion -> Admin 6[s] errCode [errStr]
#define COLLUSION_A_GET_ALERT_BY_TYPE			/*0x207F*/ (COLLUSION_Q_GET_ALERT_BY_TYPE + COLLUSION_DELTA)

#define COLLUSION_A_CREATE_RUG_ALERT			/*0x2080*/ (COLLUSION_Q_CREATE_RUG_ALERT + COLLUSION_DELTA)									// PYR-58616

#define COLLUSION_A_GET_USER_IN_PAIR_ALERTS_BY_TYPE		/*0x2081*/ (COLLUSION_Q_GET_USER_IN_PAIR_ALERTS_BY_TYPE + COLLUSION_DELTA)			// PYR-72656

#define COLLUSION_A_UPDATE_USER_RG				/*0x2082*/ (COLLUSION_Q_UPDATE_USER_RG + COLLUSION_DELTA)									// PYR-75546
	// CRM->C 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_CREATE_CONFIG			/*0x2083*/ (COLLUSION_Q_MSTR_CREATE_CONFIG + COLLUSION_DELTA)								// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_MODIFY_CONFIG			/*0x2084*/ (COLLUSION_Q_MSTR_MODIFY_CONFIG + COLLUSION_DELTA)								// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_ACTIVATE_CONFIG		/*0x2085*/ (COLLUSION_Q_MSTR_ACTIVATE_CONFIG + COLLUSION_DELTA)								// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_DELETE_CONFIG			/*0x2086*/ (COLLUSION_Q_MSTR_DELETE_CONFIG + COLLUSION_DELTA)								// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_SET_PSRSCORES			/*0x2087*/ (COLLUSION_Q_MSTR_SET_PSRSCORES + COLLUSION_DELTA)								// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_GENERATE_ALERTS		/*0x2088*/ (COLLUSION_Q_MSTR_GENERATE_ALERTS + COLLUSION_DELTA)								// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_MSTR_MANUAL_ALERT_REPORT	/*0x2089*/ (COLLUSION_Q_MSTR_MANUAL_ALERT_REPORT + COLLUSION_DELTA)							// PYR-50464
	// Collusion -> Admin 42[s] reqId, errCode [errString]
#define COLLUSION_A_CLOSE_BATCH					/*0x2090*/ (COLLUSION_Q_CLOSE_BATCH + COLLUSION_DELTA)										// PYR-64085
	// Collusion -> Security modules 42[s] reqId, errCode [errString]
#define COLLUSION_A_TRA_SET_PARAMETER			/*0x2091*/ (COLLUSION_Q_TRA_SET_PARAMETER + COLLUSION_DELTA)								// PYR-70751
	//C->A 2[s] errCode [errStr]
#define COLLUSION_A_TRA_MANAGE_RULE				/*0x2092*/ (COLLUSION_Q_TRA_MANAGE_RULE + COLLUSION_DELTA)									// PYR-70751
	//C->A 2[s] errCode [errStr]
#define COLLUSION_A_CREATE_SEC_ALERT_GROUP		/*0x2093*/ (COLLUSION_Q_CREATE_SEC_ALERT_GROUP + COLLUSION_DELTA)							// PYR-84380
	//C -> CCDA

// SECURITYALERTS.ALERTTYPE (#7093, 7094, 7897...)
enum eSecurityAlertType
{
	eSecurityAlertNONE = 0,
	eSecurityAlertCBMD = 1,									// not in use, but alerts in DB may exist
	eSecurityAlertMAA = 2,
	eSecurityAlertManual = 3,
	eSecurityAlertTSS = 4,
	eSecurityAlertRUG = 5,
	eSecurityAlertMCA = 6,									// #14195
	eSecurityAlertSuperRelated = 7,							// #14307 - reserved for future
	eSecurityAlertCompliance = 8,							// #12772
	eSecurityAlertTRA = 9,									// #14378
	eSecurityAlertTRA2 = 10,								// #14377
	eSecurityAlertCDA = 11,									// #16033
	eSecurityAlertHackAttempt = 12,							// #17069
	eSecurityAlertLogin = 13,								// #20301
	eSecurityAlertVelocityIP = 14,							// #19439
	eSecurityAlertTrueIP = 15,								// #21034
	eSecurityAlertGQMismatch = 16,							// #22414
	eSecurityAlertHSMT = 17,								// #16557
	eSecurityAlertCDA2 = 18,								// #24957
	eSecurityAlertLockout = 19,								// #24890
	eSecurityAlertPausaOutliers = 20,						// PYR-26196
	eSecurityAlertPaymentSecurity = 21,						// PYR-26196
	eSecurityAlertGameIntegrity = 22,						// PYR-26196
	eSecurityAlertResponsibleGaming = 23,					// PYR-26196
	eSecurityAlertPossibleBlackList = 24,   				// PYR-27614
	eSecurityAlertNJGeoIP = 25,   							// PYR-27243
	eSecurityAlertGreekTax = 26,   							// PYR-29250
	eSecurityAlertFreemium = 27,   							// PYR-29348
	eSecurityAlertPMCDADF = 28,								// PYR-29530 Frank. PlayMoney. // PYR-33477	
	eSecurityAlertPMCDASSG = 29,							// PYR-29530 Frank.
	eSecurityAlertAuthenticationFailure = 30,				// PYR-28735
	eSecurityAlertLoginAfterDormancy = 31,					// PYR-30455 PYR-30909
	eSecurityAlertCasinoHighWins = 32,						// PYR-31624
	eSecurityAlertCDADF = 33,								// PYR-29530 Frank. Real money. "Dump folds" alert
	eSecurityAlertCDASSG = 34,								// PYR-29530 Frank. Real money. "Suspicious stack grow" alert
	eSecurityAlertSTR = 35,									// PYR-24460
	eSecurityAlertSelfLocking = 36,							// PYR-34643
	eSecurityAlertFailedLoginFA = 37,						// PYR-35428
	eSecurityAlertHolmesSTR = 38,							// PYR-36683
	eSecurityAlertCrossSiteMAA = 39,						// PYR-42376
	eSecurityAlertFTBio = 40,								// PYR-42107
	eSecurityAlertMLV = 41,									// PYR-43027
	eSecurityAlertNJRejoinTournAfterGeo = 42,				// PYR-43576
	eSecurityAlertAuthFailureAPP = 43,						// PYR-38917
	eSecurityAlertMassFailedLogin = 44,						// PYR-47163
	eSecurityAlertCustomerDueDiligence = 45,				// PYR-47597
	eSecurityAlertFreudFraud = 46,							// PYR-51363
	eSecurityAlertFreudCRM = 47,							// PYR-51363
	eSecurityAlertScorecard = 48,							// PYR-51233
	eSecurityAlertPANVerification = 49,						// PYR-53001
	eSecurityAlertBioMAA = 50,								// PYR-55171
	eSecurityAlertPTBlackList = 51,							// PYR-52669 raised as result of Levenshtein match of Doc ID
	eSecurityAlertNJPossibleBlackList = 52,					// PYR-58755
	eSecurityAlertColCDA = 53,								// PYR-64013
	eSecurityAlertHdwMAA = 54,								// PYR-58616
	eSecurityAlertGeolocMismatch = 55,						// PYR-66110
	eSecurityAlertAsiaCDD = 56,								// PYR-64085
	eSecurityAlertResponsibleGaming2 = 57,					// PYR-75546
	eSecurityAlertTrackingcode = 58,						// PYR-67877
	eSecurityAlertFreudDeposit = 59,						// PYR-77260
	eSecurityAlertFRDocMismatch = 60,						// PYR-43398
	eSecurityAlertUSPABlacklist = 61,						// PYR-112094
	eSecurityAlertNewDevice3DSTrigger = 62,					// PYR-104952
};

// PYR-51363
enum FreudGameType
{
	eGameType_NONE = 0,
	ePOKER_RM_CASH = 1 << 0,  //1
    ePOKER_RM_TOURN = 1 << 1, //2,
};

// PYR-51363
enum FreudPredictionType
{
	ePredictionType_NONE = 0,
	eFirstPokerHand = 1,
	eFreudDepositAlert = 2			// PYR-77260
};

// PYR-51363
struct FreudPredictionEvent
{
	UINT32 userIntId;
	PString userId;
	UINT32 predictionType;
	UINT32 predictionScore;
	PString description;
	UINT32 firstDepositAmount;
	PString firstDepositCurrency;
	UINT32 stake;
	UINT32 gameType;
	PString gameCurrency;
	UINT64 transId;					// PYR-77260

	FreudPredictionEvent()
	{
		userIntId = 0;
		predictionType = 0;
		predictionScore = 0;
		firstDepositAmount = 0;
		stake = 0;
		gameType = 0;
		transId = 0;
	}

	void parse(CommMsgParser& parser)
	{
		parser.
			parseUINT32(userIntId).
			parseStringP(userId).
			parseUINT32(predictionType).
			parseUINT32(predictionScore).
			parseStringP(description).
			parseUINT32(firstDepositAmount).
			parseStringP(firstDepositCurrency).
			parseUINT32(stake).
			parseUINT32(gameType).
			parseStringP(gameCurrency)
			;
		if (!parser.parseEnded())
		{
			parser.parseUINT64(transId);
		}
	}

	void compose(CommMsgBody& body) const
	{
		body.
			composeUINT32(userIntId).
			composeString(userId).
			composeUINT32(predictionType).
			composeUINT32(predictionScore).
			composeString(description).
			composeUINT32(firstDepositAmount).
			composeString(firstDepositCurrency).
			composeUINT32(stake).
			composeUINT32(gameType).
			composeString(gameCurrency).
			composeUINT64(transId)
		    ;
	}

	void log() const
	{
		PLog("FreudPreEvent:u=%s,uid=%u,type=%u,sc=%u,fd=%u,stake=%u,gty=%u,gc=%s,tid=%llu", 
			userId.c_str(), userIntId, predictionType, predictionScore, firstDepositAmount, stake, gameType, gameCurrency.c_str(), transId);
	}
};

// PYR-54633
enum CRMOfferType
{
	eCRMOffer_StarCode = 0,
	eCRMOffer_OfferCode = 1,
	eCRMOffer_ControlGroup = 2,
};

/*---- Connection types ----*/
#define ASIA_CDD_TO_COLLUSIONS_CONN_TYPE					"asiacdd"
#define CRM_TO_COLLUSIONS_CONN_TYPE							"crmserver"
#define SECPORTAL_TO_COLLUSIONS_CONN_TYPE					"secportal"

#endif // collusions_h_included
