#ifndef dbmmsg_h_included
#define dbmmsg_h_included

//////////////////////////////////////////////////////////////////////////////
//
// HOW TO ADD NEW MESSAGES:
// 1. define new message ID above "Add new messages above this comment!!!" comment
// 2. add reply in the new format e.g. (DBM_DELTA + DBM_Q_YOUR_MSG)
// 3. do not forget to add numeric replyID in the comment 
//
/////////////////////////////////////////////////////////////////////////////

#define DBM_DELTA           1000

//////////////////////////////////////Communication protocol Ids:
#define	DBM_Q_KILL_TABLES	108
#define	DBM_A_KILL_TABLES	109


//Unsolicited messages sent as notifications from dbm to other processes:

#define DBM_U_USER_BONUS_EXERCISED			500	//4ss4ss44 reqId=0, userId, email, bonusAmount, bonusName, bonusUrl, privileges, emLocale
//new best hand occured
#define DBM_U_NEW_BEST_HAND					501 //4111<ss8> reqId = 0, indicator(bit), rank, numUsers<combination, userId, handId>
#define DBM_U_ADMIN_REVOKED					502 //4ss4 reqId=0 userId numEmails<email> adminLoginId
#define DBM_U_ACCOUNT_CLOSED				503 //4ss4b488488 - reqId=0, userId, email, emLocale, authFailed, correctedFlags, correctedFlags2, correctedFlags3, effectivePrivileges, effectivePrivileges2, effectivePrivileges3
#define DBM_U_USER_VIP_STATUS				505 //4s444s444424b reqId=0, userId, newVipStatus, locale, licenseId, name, minFpp, minFppNext, fppBonusRate, vipToKeepYearlyStatus, lastMonthForYearly, missableMonthsYearly, doNotSendPopUp, country
#define DBM_U_ACH_RETURN					506 //4ss444sT4s4D reqId=0, userId, email, newFlags, transId, amount, currency, completed, authCode, authMsg, locale, representmentDate
#define DBM_U_USERPRIVS_CHANGED				507 //4ss4848 reqId=0, userId, email, newPrivs, newPrivs2, suspendRmAction, newPrivs3
#define DBM_U_USER_CHAT_RESTORED			508 //4ss44TT488 reqId=0, userId, email, newPrivs, newFlags, timeWhenRevoked, timeRevokedUntil, locale, newPrivs2, privs3
#define DBM_U_USER_EXCLUSION_EXPIRED		509 //4ss444TT4b48 reqId=0,userId,email,newPrivs,newFlags,numDays,timeFrom,timeUntil,locale,notifyAAMS,userIntId,flags2
#define DBM_U_CHARGEBACK					510 //44ss44ss4T4[b] reqId=0, locale, userId, email, amountUSD, purchaseTransId, cardNum, paySystem amountOwed purchaseCompleted timezone,isPM
#define DBM_U_USER_GAME_LIMIT_APPOVED		511 // 4s448bbbbb reqId=0, userId, newPrivs, newFlags, newFlags2, approveTableLimit, approveTournLimit, approveBuyInLimit, approvePlayingTimeLimit, approveCasinoLimit
#define DBM_U_UNCLEARED_CLEARED				513 // 4ss44s4 reqId=0, userId, email, emLocale, transId, paySystem, amount

// PYR-68134 DBM_U_USERFLAGS2_CHANGED has been superseded by DBM_U_USER_FLAGS_PRIVS_CHANGED
//#define DBM_U_USERFLAGS2_CHANGED			514 // [AO 2009-03-06] #10684: 448 reqId=0, userId, flags2
#define DBM_U_USER_RMOK_CHANGED				515 // 4b4448T rmfRaised, updateId, updateType, adminIntId, appLoginId, newUser
#define DBM_U_TO_CASHIER_DBMINI_UPDATED		516 //
#define DBM_U_USER_OWEDCHIPS_LIQUIDATED		517 // 4ss44s4s reqid=0, userid, email, emLocale, amountFrom, currencyFrom, amountTo, currencyTo
#define DBM_U_USER_BALANCE_BECAME_NEGATIVE	518 // 4ss4444   reqid=0, userid, email, locale, emLocale, vipStatus, licenseId
#define DBM_U_REWARD_PAYMENT				519 // 4s44s4444444 reqid=0, userid, amount, tAmount, currency, vppLevel, nextAmount, nextTAmount, nextVppLevel, currentCounterInCents, totalNum, totalAwardFor1stTime
#define DBM_U_CASHERRORS_ALERT              520 // 4b4ssss reqid=0, isCashErrorsInserted, groupId, gateway, paysystem, transtype, errCode [PYR-13041]
#define DBM_U_USER_VPPS_CHANGED             521 // 4s44444 reqId=0, userId, fpps, monthlyVpps, yearlyVpps, vipStatus [PYR-13431], dailyVpps [PYR-17290]
#define DBM_U_VIP_REWARDS_UPDATED			522 // 44<444>4<4> - reqId=0, numItems<groupId, vppThreshold, amount>, numItems<vppThreshold>
#define DBM_U_CASHTRANS_APPROVED			523 // 4(Trn)Ds - req=0, CashTrans, mailDate, wayBill
#define DBM_U_EXCLUDED_AND_BLOCKED_STATES_UPDATED		524 // 4B - 0 BlockedCountries
#define DBM_U_CASHERRORS_UPDATED			525 // 41ssss4 - reqId=0 updateNotDelete gateway paysystem transType cashErrorCode cashErrorGroupId
#define DBM_U_USER_OWEDCHIPS_TOGGLED		526 // 4s4411 reqid=0, userid, transType, objectId, hasOwedChipsBefore, hasOwedChipsNow
// PYR-15418
#define DBM_U_DO_TABLEDBM_LIABILITIES_REG	527	// 4t4 - reqId = 0, srvTime, licenseId = 0

#define DBM_U_DO_TABLEDBM_LIABILITIES_EXT_USR_MONEYONTBL	528	// 4t4 - reqId = 0, srvTime, licenseId

// [AN 2010-08-19] PYR-15236 Estonia Global Black List
#define DBM_U_ADMIN_MASS_UNREGISTER_USER	529 // 4sstr - reqId = 0, adminUserId, userId, timeExpires, reason
#define DBM_U_BINDETAILS_UPDATED			530 // 44<ssbD4> reqId=0 numBins<bin country isHostile hostileExpiry flags>
// PYR-17774
#define DBM_U_SYSTEM_RESTART_TIME			531 // 4t - reqId=0, restratTime

// PYR-18621
#define DBM_U_IT_USER_DOCUMENTS_REQUESTED		532 // 4s reqId=0,userId

//PYR-19418
#define DBM_U_IT_ACCOUNT_STATUS_CHANGE		533 //4s481s - reqId=0,userId,userIntId,flags2,reason,status

// PYR-18694
#define DBM_U_ORIGINAL_MOBILE				534 // 4ss4 - reqId=0,userId,origMobile,locale
// #19893 dbm->cashier
#define DBM_U_USER_DEPOSIT_LIMIT_FINALIZED	535 // 4s144 reqId=0 userId status oldLimit requestedLimit

// PYR-20730
#define DBM_U_USER_DOCUMENTS_REQUESTED		536 // 4s reqId=0,userId
#define DBM_U_TO_REGULATOR_INI_UPDATED		537
// dbm->lobby
#define DBM_U_MAIN_DBM_INI_UPDATED			538

// PYR-22522 dbm->lobby
#define DBM_U_REWARD_PAYMENT_1ST_FPP		539 // 4s reqId=0,userId

//PYR-24396 dbm->lobby
#define DBM_U_TOURNHAPPYHOURS				540 //44<rec> - reqId=0,numRecords,<TournHappyHours>

// PYR-23062
#define DBM_U_USER_NEW_HALL_OF_FAME_LEVEL	541 //4s444 - reqId=0, userId, ltvpp, levelId, flags

#define DBM_U_USER_CARDINFOS_UPDATED		542 // 4s44<> reqId=0, userId, type, numRecords, <SavedCardInfo>

//PYR-24382 dbm->lobby
//#define DBM_U_GOLDENSNG_OBSOLETE						543 //44<rec> - reqId=0,numRecords,<GoldenSnG>
//PYR-24840 dbm->cashier
#define DBM_U_AWARDS_UPDATED				544 //44<rec> - reqId=0,numRecords,<Awards>

//PYR-27214
#define DBM_U_OLTP_TIME_UPDATE				545	//4t - reqId = 0, srvUtcTime

#define DBM_U_GREEK_TAXES_PAID				546

//PYR-28690 dbm->lobby
//#define DBM_U_MILESTONEGOLDENSNG_OBSOLETE			547 //44<rec> - reqId=0,numRecords,<MilestoneGoldenSnG>

//DBM_Q_AVAILABLE_MESSAGE
//#define DBM_U_BOPMAPS_UPDATED				548 //44<44<s4<t4<4444>>>> - reqId=0,numSites<siteId,numCurrencies<currency,numTimes<effectiveFrom,numItems<mapId,from,to,tlbBoard>>>>

#define DBM_U_TO_AUXHUB_INI_UPDATED			549 // XcSettingsCommon
// dbm->cashier
#define DBM_U_FIRST_LICENSED_DEPOSIT        550 // PYR-30476 Castrans + User

// PYR-29583 dbm->auxhub
#define DBM_U_LANG_PATTERNS                 551

// PYR-31006
#define DBM_U_DEGAS_INTERVAL				552 // t - UTC time of the first moment of last period (eg 11:00:00), and sysInfo.utcServerTime contains the first moment of new time period 

//#define DBM_U_USERROLL_CHANGED              553 // PYR-31829 dbm->auxhub : 4-reqId=0, 4-userIntId, UserRoll
#define DBM_U_ROLL_CHANGED                  554 // PYR-31829 dbm->auxhub & cashier : 4-reqId=0, Roll
//#define DBM_U_USERROLL_DELETED              555 // PYR-31841 dbm->auxhub : 4-reqId=0, 4-userIntId, 4-userRollId

#define DBM_U_DO_SPORTSBOOK_LIABILITIES_EXT	556	// PYR-34302 dbm->sportsbook : 4t4<4> - reqId = 0, srvTime, n<licenseId>
#define DBM_U_DO_SPORTSBOOK_LIABILITIES_REG	557	// PYR-34302 dbm->sportsbook : 4t4<4> - reqId = 0, srvTime, n<licenseId> = empty vector

// PYR-33233
#define DBM_U_TO_SPORTSBOOK_INI_UPDATED     558 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

#define DBM_U_USERROLLS_CHANGED             559 // PYR-31829 dbm->cashier and auxhub : 4-reqId=0, 4-userIntId, s-userId, 4-locale, UserRolls, ui32vec-ins, ui32vec-del
#define DBM_U_USERROLL_CLEARED             560 // PYR-31838 dbm->cashier : 4-reqId=0, 4-userIntId, UserRoll, 4-amtToPay, b-redemptionLimitReached

// PYR-32027
#define DBM_U_TO_LOBBYETC_INI_UPDATED		561 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

// PYR-36465, PYR-48971 renamed to DBM_U_TO_DFSUS_INI_UPDATED
#define DBM_U_TO_DFSUS_INI_UPDATED        562 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

// PYR-32356 dbm->logindbm
#define DBM_U_GET_USERS_SNAPSHOT			563 // logindbm will reply with DBM_Q_SUBMIT_USERS_SNAPSHOT message

// PYR-35364
#define DBM_U_USERPROP_CHANGED             564 // 4-reqId=0, 4-userIntId, s-userId, userProp, 4-recordId

// PYR-40680
#define DBM_U_USERBRANDIMAGE_CHANGED       565 // 4-reqId=0, 4-userIntId, s-userId, 4-brandId, 4-imageId

#define DBM_U_CONVRATES_CHANGED            566 // reqId=0, vector<CurrencyRate>, Margins // PYR-36381
#define DBM_U_USER_BIGWIN                  567 // 4-reqId=0,4-userIntId,s-userId,8-transId,4-transType,8-objectId,4-amount,s-currency // PYR-44105

// PYR-42410
#define DBM_U_TO_POOLBETTING_INI_UPDATED     568 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

#define DBM_U_GAMETICKETTYPE_CHANGED         569 // PYR-46074 dbm->cashier : 4-reqId=0, GameTicketType

//	PYR-44063
#define DBM_U_DO_POOLBETTING_LIABILITIES_REG 570	// dbm->poolBetting : 4t4<4> - reqId = 0, srvTime, n<licenseId> = empty vector

// PYR-36465 PYR-48971
#define DBM_U_TO_DFSINTL_INI_UPDATED     571 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

// PYR-44386
#define DBM_U_TO_REGULATOR_MODIFY_USERS_TRACKCODE 572 // 44s4s4s4 reqId=0 userActionId,trackcode,acctionType,userId,userIntId,adminId,adminIntId

// PYR-47909, dbm->cashier
#define DBM_U_USER_COUNTRY_CHANGED       573 // 4ss4 reqId=0,'UserCountryChangeNotification'

// PYR-50736 dbm->lobbyEtc
#define DBM_U_LICENSECHANGED			 574 // 4444 reqId=0,userIntId,oldLicense,newLicense PYR-50736 dbm->lobbyEtc

#define DBM_U_GAMETICKETT_ISSUED		 575 // PYR-47665 dbm->cashier: 4-reqId=0, 4-userIntId, s-userId, 4-locale, GameTicket

#define DBM_U_SELFEXCLUSION_CHANGED      576 // 4b48T reqId=0,setNotExpire,daysOfSelfExclusion,appLoginId, <User> dbm->cashier // PYR-51733
// obsolete by PYR-77865 #define DBM_U_USER_FLAGS_CHANGED		577 // 444844T reqId=0,oldFlags,newFlags,appLoginId,updateType,updateId,<User>

// PYR-50049
#define DBM_U_TO_REGULATOR_SEND_MSG_TO_USER 578 // 4<T><T> reqId=0, <UserPendingAction>, <User>

#define DBM_U_CASHOUT_REFUND            579 // 4844T reqId=0, auditId64, originalCashoutParentTransId, refundAmount, <User> dbm->cashier // PYR-30309
#define DBM_U_CASHTRANS_CHANGED			580 // inserted or updated, PYR-59401, see dbm code for compose format

// PYR-63110
#define DBM_U_TO_PREDICTOR_INI_UPDATED  581 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

//	PYR-64085
#define DBM_U_ASIAGAMINGGATEWAY_INI_UPDATED 582 // 4<s6> - size<operatorId,OpId>

// PYR-74890
#define DBM_U_DO_SPORTSBOOK_LIABILITIES_EXT_BET 583	// dbm->sportsbook : 4t4<4> - reqId = 0, srvTime, n<licenseId>

#define DBM_U_TAX_PAID 584	// dbm->cashier: User, etc

// PYR-81644
#define DBM_U_TO_STARSSTORE_INI_UPDATED  585 // uses IniRulesMap::compose/parse 	  4<s8>  - numItems<name,licenseFlags>

// PYR-76141
#define DBM_U_SUGGESTEDDEPOSITS_UPDATED		586 //44<rec> - reqId=0,numRecords,<SuggestedDepositCampaign>

#define DBM_U_NEW_USER_INSERTED				587 // 4-reqId=0,User // PYR-91134

// PYR-114224
#define DBM_U_TO_BLITZ_LOBBY_INI_UPDATED  588 

// PYR-114342
#define DBM_U_DEPOSIT_LIMIT_CHANGED			589 // dbm->cashier: 4m(O)4448T - reqId=0, m(User), limitRequestId, limitType, limitAmount, appLoginid, msgTime
#define DBM_U_DEPOSIT_APPROVED				590 // dbm->cashier: 4m(O)m(O)T - reqId=0, m(User), m(CashTransDataSimple), msgTime

// PYR-88481
#define DBM_U_TO_SOCIAL_CASINO_INI_UPDATED	591 // dbm->socialcasino 44<s8>  reqId=0, numItems<name,licenseFlags>

// PYR-122184
#define DBM_U_VERIFIED_CARD_UPDATED			592 // dbm->cashier: 4s44 - reqId=0, userId, cc_id, type

// PYR-124654
#define DBM_U_TO_REGULATOR_USERS_TO_REMOVE_PRIV_DUPLICATE 593 // 44<s> - reqId=0,numUsers,<userId>

//
// DBM REQUESTS AND REPLIES
//
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_USER_AUTH                                             1001  // 4sssss714m4 - q_id, usernick, macAddr, volumeLabel, installId, ipAddr, port, objectType, objectId(see OBJ_TYPE_* constants), messagebody, platformId
#define DBM_A_USER_AUTH                                             (DBM_DELTA + DBM_Q_USER_AUTH) // 2001	//	46[M()4s4s4ssM()4<s>4444444844t848tssM(448)44sM(t8448sM(8M()44)m)s4s8ss4866t44s|sM()4] - q_id, err_code
																											//	[authBody,privs,city,flags,email,userIntId,country,state,M(-empty-),rightsSize<right>,depositMask,ltFpp,vipStatus,locale,fpp,vipCounter1,vipCounter2,
																											//	priv2,emLocale,timezone,gracePeriodUntil,flags2,originalUserIntId,depositMask,lastLogin,countryByIp,stateByIp,M(depMask,depMask2,depMask3)
																											//	vipCounter3,licenseId,fiscalCode,M(lastLogoutTime,lastLoginId,lastLogoutReason,visitedMask,currentLoginId,lastLogoutIp,M(appSessionId,M(mappedInfoBody),sessionFlags,parentAppTypeId),visitedMaskEx),
																											//	promoImageCode,authFlags,frArjelUserHash,privileges3,userId,sex,imageId,flags3,vipCounter1CentsOnly,vipCounter2CentsOnly,
																											//	registered,targetRewardPoints,pokerRPReward,defaultCurrency
																											//	|errDescr [authBody,userLocale]] 

// PYR-20837
#define DBM_Q_GET_ES_USER_LAST_LOGIN_DETAILS                        1002  // 448ttt - q_id, userIntId, loginId, lastrLogin, lastLogout, currentLogin
#define DBM_A_GET_ES_USER_LAST_LOGIN_DETAILS                        (DBM_DELTA + DBM_Q_GET_ES_USER_LAST_LOGIN_DETAILS) // 2002  //46[s|tt4884<4>4<4s>] - q_id, err [errDescr| lastLogin, lastLogout, logoutReason, totalPlayed,totalWon,numTourns<tournId>numHands<handId,tableName>] 

#define DBM_QS_USER_ADD                                              1003  //4ss4sssssssss44s4Dsssssbss4b4M
#define DBM_AS_USER_ADD                                              (DBM_DELTA + DBM_QS_USER_ADD) // 2003  //46[s|M()4s4b] q_id, err [errDescr|M(userDataBody),numFppForQuestionnaire,promoDescr,validationCode,emailUniqueCode] 

//q_id,id,pwd,playChips,fullName,email,addr_1,addr_2,city,state,country,zipCode, userInstallId, siteId, questInt, questStr, questFPP, DOB, trackingVer, trackingGUID, manualPromoCode, trackingCode, trackingInfo, lastAttempt, firstName, lastName, licenseId, mailList, imageId (see IMAGE_TYPE_* constants for details), similarUsersBody(#20399)


#define DBM_Q_HAND                                                  1004  //4M(...)M(empty or 4<4M(...)>) - reqId,M(see Hand::parseFromTable),M(empty or IceBody - numIceEvents<userIntId,M(userIceBody)>)
#define DBM_A_HAND                                                  (DBM_DELTA + DBM_Q_HAND) // 2004  //46[s] q_id, err [errDescr] 

#define DBM_Q_MTL_EARLY_PAYOUT										1005
#define DBM_A_MTL_EARLY_PAYOUT										(DBM_DELTA + DBM_Q_MTL_EARLY_PAYOUT) //2005

#define DBM_Q_HAND_ID                                               1006  //481//q_id, table_id, chips_type (0=real 1=play 2=tourn)
#define DBM_A_HAND_ID                                               (DBM_DELTA + DBM_Q_HAND_ID) // 2006  //46[44|s] q_id, err, [handId, created|errDescr] 

#define DBM_Q_TABLE_NEW                                             1007  //44s444s - q_id, table_type, assignedName, typeFlags, clubId, callFlags, effCreator
#define DBM_A_TABLE_NEW                                             (DBM_DELTA + DBM_Q_TABLE_NEW) // 2007  //46[44s4s|s] q_id, err, [tableType,tableId,TableName,clubId,effCreator|errDescr] 

#define DBM_Q_GET_VIPSTATUS_INFO_RO									1008 // see DBM_Q_GET_VIPSTATUS_INFO (without MessageInfoStruct)
#define DBM_A_GET_VIPSTATUS_INFO_RO									(DBM_DELTA + DBM_Q_GET_VIPSTATUS_INFO_RO) // 2008 - see DBM_A_GET_VIPSTATUS_INFO

#define DBM_Q_USER_IN                                               1009  //48s11444sM(cc)141ss14M([rec])68 - reqId,tableId,userId,pos,isPlay,chips,hiBetFL,hiBetNL,currency,{currencyContext},maxPlayers,siteId,inGame,country,state,isPLO,scalePM,{[struct RatholeManTableData]},gameType,appSessionId
#define DBM_A_USER_IN                                               (DBM_DELTA + DBM_Q_USER_IN) // 2009  //46[ss4847M(BrandImages)|s[4]] q_id, err [, city, email, imageId, transIdBuyIn, hallOfFameLevelId, chipsAfter, M(BrandImages) | errDescr ] 

#define DBM_Q_USER_OUT                                              1010  //48s14s4188 - reqId,tableId,userId,isPlay,chips,currency,scalePM, forcedLeave, currHandId, appSessionId
#define DBM_A_USER_OUT                                              (DBM_DELTA + DBM_Q_USER_OUT) // 2010  //46[s] q_id, err [, errDescr] 

#define DBM_Q_BL_SAVE_STATS                                         1011  //44t4...4 - reqId,blitzId,timeStamp,timeInterval(sec),timerResolution(ms),hands,pot,rake,numEntries,numSittingOut,numPlayers,playersToSeat,playersSeated,numPostponedPlayers,numWaitingPlayers1,...,numWaitingPlayers10
#define DBM_A_BL_SAVE_STATS                                         (DBM_DELTA + DBM_Q_BL_SAVE_STATS) // 2011  //46[s] - reqId,errCode,[errDescr] 

#define DBM_Q_NEED_DATA                                             1012  //4 q_id
#define DBM_A_NEED_DATA                                             (DBM_DELTA + DBM_Q_NEED_DATA) // 2012  //46[s|...data...] q_id, err [, errDescr | gametypes+tabletypes+tables ] 

#define DBM_Q_USER_CHIPS_QUERY                                      1013  // 4s1s4b441M(M(41144bbs)448) - reqId,userId,isPlay,gameCurrency,minDebitAmount,needRelated,clientPlatform,clientVersion,gameObjectType,PokerGameInstanceInfoForRoll=M(M(41144bbs)448)
#define DBM_A_USER_CHIPS_QUERY                                      (DBM_DELTA + DBM_Q_USER_CHIPS_QUERY) // 2013  // 46[sb|41484bMM4<s>bM] - reqId,errCode,[errDescr,enoughOnAllAccounts|chips,playAddedMask,playAddedAmount,playChips64,userRollId,enoughOnAllAccounts,M(currencyContext),M(currencyContextCli),numGroups<group>,useAutoConvertBuyIn,M(clientCurrencyContextShort)]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_IMAGE                                                 1014  //44 req_id, imageId
#define DBM_A_IMAGE                                                 (DBM_DELTA + DBM_Q_IMAGE) // 2014  //46[s|B44] req_id, err_code, [errDescr|block,height,width] 

#define DBM_Q_CHIPS_FULL_INFO                                       1015  //4s req_id, userId	
#define DBM_A_CHIPS_FULL_INFO                                       (DBM_DELTA + DBM_Q_CHIPS_FULL_INFO) // 2015  //46[s|44444ssssssss1s] req_id, err_code, [errDescr|chips, playchips, allocChips, allocPlayChips, fppAmount, email, adr1, adr2, city, state, country, zip, fullName, realMoneyOk, category] 

#define DBM_Q_CREATE_TOURNAMENT                                     1016  //4s4<s>s typeId, startDateTime, numPreregistered<userName>, effCreator
#define DBM_A_CREATE_TOURNAMENT                                     (DBM_DELTA + DBM_Q_CREATE_TOURNAMENT) // 2016  //46[s|444] req_id, err_code, [errDescr|tourn_id, start_time,] 

#define DBM_Q_HAND_HISTORY                                          1017  //s14[<8>]1	userId, requestType, requestContents[<handId>], outOfHandHistory 
#define DBM_A_HAND_HISTORY                                          (DBM_DELTA + DBM_Q_HAND_HISTORY) // 2017  //46[s|ss184<m>] req_id, err_code[errDescr|userId, email, requestType, requestContents, numHands<HandBody>] 

#define DBM_Q_USER_ADDCHIPS                                         1018  //48s1144sM448 q_id, table_id, userId, pos, is_play_chips, chips, chipsStart, currency,currencyContext,scalePM,itLastAddonId,appSessionId
#define DBM_A_USER_ADDCHIPS                                         (DBM_DELTA + DBM_Q_USER_ADDCHIPS) // 2018  //46[s|48] q_id, err [errDescr|amountAdded,transIdAddOn] 

// obsolete by PYR-89086
//#define DBM_Q_ARCH_CHECK_USER_UPDATE                                1019  // (regulator->logindbm) 4s4s reqId, userId, licenseId, email 
//#define DBM_A_ARCH_CHECK_USER_UPDATE                                (DBM_DELTA + DBM_Q_ARCH_CHECK_USER_UPDATE) // 2019  //  

#define DBM_Q_USER_INFO                                             1020  //4s reqId, userId
#define DBM_A_USER_INFO                                             (DBM_DELTA + DBM_Q_USER_INFO) // 2020  //46[s|ssssssssB4444[sss4244<ssT>]D,8,8, 4TT]ssss -  


// if BIT_ADDRESS		ssssss		addr_1, addr_2, city, state, country, zipCode
// if BIT_EMAIL		s			new_email
// if BIT_PASSWORD		s			new_pwd
// if BIT_IMAGE		1{.|4|B}	imageType, {nothing OR image(private) OR imageId(public)}
// if BIT_FULLNAME		s newFullName
//PYR-21469
#define DBM_Q_GET_SHARED_CC_RUG                                     1021     //4ss reqId userId1 userId2
#define DBM_A_GET_SHARED_CC_RUG                                     (DBM_DELTA + DBM_Q_GET_SHARED_CC_RUG) // 2021   //46[s|44] reqId, errCode, [errDescr|nSharedCC nRUGs] 

// PYR-32356 logindbm->dbm(oltp rw)
#define DBM_Q_SUBMIT_USERS_SNAPSHOT									1022 //44<M(...)> - reqId, size<M(User)>
#define DBM_A_SUBMIT_USERS_SNAPSHOT									(DBM_DELTA + DBM_Q_SUBMIT_USERS_SNAPSHOT) // 2022 //4

#define DBM_Q_LOBBY_SETTINGS                                        1023 	//// 4s{TableTypeScript} - reqId, adminId
#define DBM_A_LOBBY_SETTINGS                                        (DBM_DELTA + DBM_Q_LOBBY_SETTINGS) // 2023   //46[s|4<4>M] q_id, err [errDescr|numIds<typeId>tableDbmData] - tableDbmData needs to be sent to table DBM 

// TableTypeScript = 1111112 444 44444 
		// game, hilo, structur, playmoney, oneOnOne, maxpertable, maxtables, 
		// minChipsLimit, minStartChips, rake, 
		// ante, smallBlind, loBet, hiBet, bringin

#define DBM_Q_ACTIVATE_ACCOUNT                                      1024 	// s44s8 userId,unique,siteId,promoName,appLoginId
#define DBM_A_ACTIVATE_ACCOUNT                                      (DBM_DELTA + DBM_Q_ACTIVATE_ACCOUNT) // 2024   //46[s|44s] q_id, err [errDescr|firstTime,siteId,promoName]  

#define DBM_Q_PENDING_USERS                                         1025 	// 4 req_id ?
#define DBM_A_PENDING_USERS                                         (DBM_DELTA + DBM_Q_PENDING_USERS) // 2025 	// 46[s|4<s44>] req_id err_code[errDescr|numUsers<userName, imageId, flags>] 

#define DBM_Q_APPROVE_USER                                          1026 	// 4s4s1s4 req_id, adminId, adminIntId, userName, approval, comments, imageId
#define DBM_A_APPROVE_USER                                          (DBM_DELTA + DBM_Q_APPROVE_USER) // 2026 	// 46[s|s444s8484M]  req_id, errCode, [errDescr | userId, imageId, effectivePrivs, flags, email, effectivePrivs2, apprivedImageId, effectivePrivs3, deletedImageId, activatedImageBody ] 

#define DBM_Q_SET_PRIVILEGES                                        1027 	// 4s4s4888884 - req_id, userId, new_priv, adminUserId, prev_priv, new_priv2, prev_priv2, new_priv3, prev_priv3, appLoginId, eUpdateType
#define DBM_A_SET_PRIVILEGES                                        (DBM_DELTA + DBM_Q_SET_PRIVILEGES) // 2027 	// 46[s|s44] q_id, err [errDescr|userId, newEffectivePrivileges, flags]  

#define DBM_Q_BL_MOVE_USER_ALLOC_VPP                                1028  // 44s84<44> - reqId, blitzId, userId, appSessionId, numTables<vppInCents,entryId>
#define DBM_A_BL_MOVE_USER_ALLOC_VPP                                (DBM_DELTA + DBM_Q_BL_MOVE_USER_ALLOC_VPP) // 2028  // 46[s] - reqId, errCode[errDescr] 

//	PYR-25971, Daemon (dbd) to dbm
#define DBM_Q_REPORTONLY_TRACKCODES_TCFLAGS                         1029 	// 4 - reqId
#define DBM_A_REPORTONLY_TRACKCODES_TCFLAGS                         (DBM_DELTA + DBM_Q_REPORTONLY_TRACKCODES_TCFLAGS) // 2029 	//44<4<4>> reqId, numTrackCodes, <numIds,<userIntId>> 

#define DBM_Q_USER_STATS                                            1030 	// s4 userId, numHands
#define DBM_A_USER_STATS                                            (DBM_DELTA + DBM_Q_USER_STATS) // 2030 	// 46[s|ss4S] q_id, err [errDescr|userId, email, numHandsRequested, Statistics]  

#define DBM_Q_TOURN_STATS                                           1031 	// 1s4 queryType(see TOURN_STATS_*), userId, tournId
#define DBM_A_TOURN_STATS                                           (DBM_DELTA + DBM_Q_TOURN_STATS) // 2031 	// 46[s|ss144{81HSV}] q_id, err [errDescr| userNAme, userEmail, requestType, tournIdOrNumTournsRequested, numTournsReturned {lastHandID, flags, TournHeader, Statistics, vector<TournParticipant>}]  

#define DBM_Q_TOURN_CLOSED                                          1032 	// 4T1s tournId, timeClosed, status, effCanceller
#define DBM_A_TOURN_CLOSED                                          (DBM_DELTA + DBM_Q_TOURN_CLOSED) // 2032 	// 46[s] q_id, err [errDescr]  

#define DBM_Q_CHANGE_USER_CHIPS                                     1033 	// 4s84644 req_id, userName, playchips64, fppAmount, fppAmountCents, transType, budgetCodeId
#define DBM_A_CHANGE_USER_CHIPS                                     (DBM_DELTA + DBM_Q_CHANGE_USER_CHIPS) // 2033 	// 46[s] q_id, err [errDescr]  

#define DBM_Q_UPDATE_USER_PWD                                       1034 	// 4ss reqId, userId, newPwd
#define DBM_A_UPDATE_USER_PWD                                       (DBM_DELTA + DBM_Q_UPDATE_USER_PWD) // 2034 	// 46[s] q_id, err [errDescr]  

#define DBM_Q_SAVE_HISTORY_ITEMS                                    1035 	//4814, reqId, tableId, chipsType, handFlags (then see handhistory starting from numFrames)
#define DBM_A_SAVE_HISTORY_ITEMS                                    (DBM_DELTA + DBM_Q_SAVE_HISTORY_ITEMS) // 2035  //46[s] q_id, err [errDescr]  

#define DBM_Q_USER_EMAIL                                            1036 	//41[4||s|4<s>] reqId, requestType[ tournId | (nothing) | userId | numUsers<userId> ]
#define DBM_A_USER_EMAIL                                            (DBM_DELTA + DBM_Q_USER_EMAIL) // 2036  //46[4<sss44>|s] reqId, errCode, [numEmail<userId, fullName, email, effectivePrivs, flags>|errDescr] 

												//EMAIL_REQ_TYPE_TOURNAMENT		0 4 tournId
												//EMAIL_REQ_TYPE_ALLUSERS		1 () - nothing
												//EMAIL_REQ_TYPE_ONEUSER		2 s userId
												//EMAIL_REQ_TYPE_USERS_LIST		3 4<s> numUsers<userId>
												//EMAIL_REQ_TYPE_TRACKCODE		s trackCode
#define DBM_Q_SET_USER_COMMENT                                      1037 	//4ss[s4] reqId, userId, commentText [,adminId, adminIntId] <- depends on connection
#define DBM_A_SET_USER_COMMENT                                      (DBM_DELTA + DBM_Q_SET_USER_COMMENT) // 2037  //46[s] reqId, errCode, [errDescr] 

#define DBM_Q_TOURN_REG_INFO                                        1038 	//4s414<s>4<4>4<s>7fssf4 reqId, userId, tournId, checkLevel, (restrictToTrackCodes), (restrictToAffiliates), (restrictToPromoCodes),numOfSagTourns,isPsng,countryByIp,stateByIp,includeDoNotPay,TournRegInfoFlags
#define DBM_A_TOURN_REG_INFO                                        (DBM_DELTA + DBM_Q_TOURN_REG_INFO) // 2038  //46[sb|444s11484444bmm] reqId, err_code, [errDescr, enoughOnAllAccounts | buyIn, rake, fpp, admissionId, isPlayMoney, isPwdProtected, userChips, userPlayChips64, userFpp, numTickets, tChips, wChips, enoughOnAllAccounts, currencyContext, clientCurrencyContext] 

#define DBM_Q_ADD_APPROVED_TRANS                                    1039 	//4s1s44ss4s44s4s8s44 reqId, userId, transtype ('P' or 'R') paySystem, amountBefore, amount, comment, xTransStr, parentId, accountCurrency purchaseTransId converted, currency, convrate, status, ctFlagsToRaise, productId, transType, budgetCodeId
#define DBM_A_ADD_APPROVED_TRANS                                    (DBM_DELTA + DBM_Q_ADD_APPROVED_TRANS) // 2039  //46[s|4ss4444] q_id, err [errDescr|bonusId, bonusName, bonusDescr, bonusPercent, bonusMax, bonusAmount, fppAmount]  

#define DBM_Q_LOBBY_REPORT_INSTALL                                  1040 	//4ssT reqId installId promo whenSrvTime
#define DBM_A_LOBBY_REPORT_INSTALL                                  (DBM_DELTA + DBM_Q_LOBBY_REPORT_INSTALL) // 2040 	//46[s] q_id, err [errDescr]  

#define DBM_Q_USER_ALL_EMAILS                                       1041 	// identical to DBM_Q_USER_EMAIL
#define DBM_A_USER_ALL_EMAILS                                       (DBM_DELTA + DBM_Q_USER_ALL_EMAILS) // 2041 	// identical to DBM_A_USER_EMAIL 

#define DBM_Q_SAVE_GLOBAL_EVENT                                     1042 	// 414444s(T) reqId infoType int1 int2 int3 int4 adminUserId when
#define DBM_A_SAVE_GLOBAL_EVENT                                     (DBM_DELTA + DBM_Q_SAVE_GLOBAL_EVENT) // 2042 	// 46[s] q_id, err [errDescr]  

// PYR-87736
#define DBM_Q_GET_SE_USERS_FOR_BLACKLIST_CHECK						1043 // 4 reqId
#define DBM_A_GET_SE_USERS_FOR_BLACKLIST_CHECK						(DBM_DELTA + DBM_Q_GET_SE_USERS_FOR_BLACKLIST_CHECK) // 2043 // 

#define DBM_Q_LOGIN_CONFIRM                                         1044 	// 4s48 reqId, userId,loginAuthMethod,appSessionId
#define DBM_A_LOGIN_CONFIRM                                         (DBM_DELTA + DBM_Q_LOGIN_CONFIRM) // 2044 	// 4s444<M>8s444444M88 - reqId, userId, correctedFlags, effectivePrivs, numBannedUsers<bannedUser>, effectivePrivs2, balanceCurrency, chips, playChips, tChips, wChips, owedChips, flags2, licenseSpecificBody, priveleges3, flags3

#define DBM_Q_RESET_PWD                                             1045 	// 4sssb4b4 reqId userId newPwd mobile isFacebookFirstLogin adminIntId doClientChecks USERCHANGES.UPDATETYPE
#define DBM_A_RESET_PWD                                             (DBM_DELTA + DBM_Q_RESET_PWD) // 2045 	// 46[s|s] reqId, err_code [errDescr|email] 

#define DBM_Q_USER_LIST_EMAILS                                      1046 	// identical to DBM_Q_USER_EMAIL
#define DBM_A_USER_LIST_EMAILS                                      (DBM_DELTA + DBM_Q_USER_LIST_EMAILS) // 2046 	// identical to DBM_A_USER_EMAIL 

#define DBM_Q_RESET_BOUNCED                                         1047 	// 4s8 reqId, userId, appLoginId	
#define DBM_A_RESET_BOUNCED                                         (DBM_DELTA + DBM_Q_RESET_BOUNCED) // 2047 	// 46[s|6] req_id, err_code [errDescr|unique] 

#define DBM_Q_BOT_USER_DETECTED                                     1048 	// 4s44s reqId, userID , code, severity, comment
#define DBM_A_BOT_USER_DETECTED                                     (DBM_DELTA + DBM_Q_BOT_USER_DETECTED) // 2048 	// 46[s] req_id, err_code [errDescr] 

#define DBM_Q_USER_TABLE_AUTH                                       1049 	// 4sssss714m - q_id, usernick, macAddr, volumeLabel, installId, ipAddr, port, objectType, objectId(see OBJ_TYPE_* constants), messagebody
#define DBM_A_USER_TABLE_AUTH                                       (DBM_DELTA + DBM_Q_USER_TABLE_AUTH) // 2049  //46[B4s4s|sB4] q_id, err_code, [messagebody, privs, city, flags, email, fpp, lt_fpp, chips |errDescr [authBody userLocale]] 

#define DBM_Q_UPDATE_SAGMAX_AWARD                                   1050 	//
#define DBM_A_UPDATE_SAGMAX_AWARD                                   (DBM_DELTA + DBM_Q_UPDATE_SAGMAX_AWARD) // 2050 	

#define DBM_Q_GET_CASHTRANS_MAX_TIMESTAMP                           1051
#define DBM_A_GET_CASHTRANS_MAX_TIMESTAMP                           (DBM_DELTA + DBM_Q_GET_CASHTRANS_MAX_TIMESTAMP) // 2051

#define DBM_Q_USER_TRACKCODE_EMAILS                                 1052 	//44s req_id, requestType, trackCode
#define DBM_A_USER_TRACKCODE_EMAILS                                 (DBM_DELTA + DBM_Q_USER_TRACKCODE_EMAILS) // 2052 	// identical to DBM_A_USER_EMAIL 

// From DBD (#6859)
#define DBM_Q_FIND_CLEARED_BY_ADMIN                                 1053     //4D reqId dateToCheck
#define DBM_A_FIND_CLEARED_BY_ADMIN                                 (DBM_DELTA + DBM_Q_FIND_CLEARED_BY_ADMIN) // 2053     //46[s|4<4>] reqId, errCode [errDescr|numRecords<recId>] 

#define DBM_Q_TRIM_CLEARED_BY_ADMIN                                 1054     //44<4> reqId numRecords<recId>
#define DBM_A_TRIM_CLEARED_BY_ADMIN                                 (DBM_DELTA + DBM_Q_TRIM_CLEARED_BY_ADMIN) // 2054     //46[s|4] reqId, errCode [errDescr|totalTrimmed]  

#define DBM_Q_FIND_STALE_UNCLEARED                                  1055     //4D reqId dateToCheck
#define DBM_A_FIND_STALE_UNCLEARED                                  (DBM_DELTA + DBM_Q_FIND_STALE_UNCLEARED) // 2055     //46[s|4<4>] reqId, errCode [errDescr|numUsers<userIntId>] 

#define DBM_Q_TRIM_STALE_UNCLEARED                                  1056     //44<4> reqId numUsers<userIntId>
#define DBM_A_TRIM_STALE_UNCLEARED                                  (DBM_DELTA + DBM_Q_TRIM_STALE_UNCLEARED) // 2056     //46[s|4] reqId, errCode [errDescr|totalTrimmed] 

#define DBM_Q_CLOSE_MASS_ACCOUNT                                    1057     //4sb7s4<s> reqId, commentProps, keepPrivileges, negativeCategory, commentUsers, nUsers<userId>
#define DBM_A_CLOSE_MASS_ACCOUNT                                    (DBM_DELTA + DBM_Q_CLOSE_MASS_ACCOUNT) // 2057     // 46[s|4<s6s>] req_id, err_code [errDescr|numUsers<UserReply>] 

#define DBM_Q_COMMENT_MASS_ACCOUNT                                  1058     //4s4<s> reqId, commentUsers, nUsers<userId>
#define DBM_A_COMMENT_MASS_ACCOUNT                                  (DBM_DELTA + DBM_Q_COMMENT_MASS_ACCOUNT) // 2058     // 46[s|4<s6s>] req_id, err_code [errDescr|numUsers<UserReply>] 

// Security alerts
#define DBM_Q_MAA_GET_USERS_BY_INTID                                1059    //44<4> reqId numUsers<userIntId>
#define DBM_A_MAA_GET_USERS_BY_INTID                                (DBM_DELTA + DBM_Q_MAA_GET_USERS_BY_INTID) // 2059    //46[s|4<User>] reqId errCode [errStr | numUsers<User>] 

// PYR-23479
#define DBM_Q_ATAS_GET_TCHIP_DATA                                   1060     //4AtasTChipData  reqId struct AtasTChipData
#define DBM_A_ATAS_GET_TCHIP_DATA                                   (DBM_DELTA + DBM_Q_ATAS_GET_TCHIP_DATA) // 2060     //46[s|b] reqId errCode [errStr | ruleFires] 

#define DBM_Q_CREATE_SEC_ALERT                                      1061     //44ssss444 reqId type user1 user2 parameter comment sourceMask behaveMask score
#define DBM_A_CREATE_SEC_ALERT                                      (DBM_DELTA + DBM_Q_CREATE_SEC_ALERT) // 2061     //46[s] reqId errCode [errStr] 

#define DBM_Q_UPDATE_SEC_ALERT                                      1062     //44s4 reqId alertId comment status
#define DBM_A_UPDATE_SEC_ALERT                                      (DBM_DELTA + DBM_Q_UPDATE_SEC_ALERT) // 2062     //46[s] reqId errCode [errStr] 

#define DBM_Q_GET_HSMT_TOURN_DATA                                   1063     //44 reqId tournId
#define DBM_A_GET_HSMT_TOURN_DATA                                   (DBM_DELTA + DBM_Q_GET_HSMT_TOURN_DATA) // 2063     //46[s|444<s>] reqId errCode [errStr | gameType,buyIn,nPlayers<userId>] 

#define DBM_Q_TSS_GET_CARDMATCH                                     1064     //4s reqId userId
#define DBM_A_TSS_GET_CARDMATCH                                     (DBM_DELTA + DBM_Q_TSS_GET_CARDMATCH) // 2064     //46[s|4<sss>] reqId errCode [errStr|numRec<userId paysystem cardNum>] 

#define DBM_Q_ATAS_GET_TRANSINFO                                    1065     //4s4 reqId userId
#define DBM_A_ATAS_GET_TRANSINFO                                    (DBM_DELTA + DBM_Q_ATAS_GET_TRANSINFO) // 2065     //46[s|4] reqId errCode [errStr|transCount <transData>] 

#define DBM_Q_ATAS_GET_HACKINFO                                     1066     //4s44444 reqId userId installCheckDays passwordCheckDays addressCheckDays emailCheckDays loginCheckDays
#define DBM_A_ATAS_GET_HACKINFO                                     (DBM_DELTA + DBM_Q_ATAS_GET_HACKINFO) // 2066     //46[s|44444] reqId errCode [errStr|numInstall numPassword numAddress numEmail numLogin] 

#define DBM_Q_MAA_GET_USERS                                         1067     //44<s> reqId numUsers<userId>
#define DBM_A_MAA_GET_USERS                                         (DBM_DELTA + DBM_Q_MAA_GET_USERS) // 2067  //46[s|4<User>] reqId errCode [errStr | numUsers<User>] 

#define DBM_Q_CHECK_PENDING_ALERTS                                  1068     //44<s> reqId numUsers<userId>
#define DBM_A_CHECK_PENDING_ALERTS                                  (DBM_DELTA + DBM_Q_CHECK_PENDING_ALERTS) // 2068     //46[s|4<4>] reqId numUsers<numAlerts> 

// PYR-21685
#define DBM_Q_GET_SELF_EXCLUDED_PARAMS                              1069  //4s reqId, userId
#define DBM_A_GET_SELF_EXCLUDED_PARAMS                              (DBM_DELTA + DBM_Q_GET_SELF_EXCLUDED_PARAMS) // 2069  //46[s | B(4bbbbbbtt4b)] reqId, err [, errDescr | B(locale,bannedES,bannedDefault,exclusionEE,exclusionAge,selfExclusion,durationExclusion,srvFrom,srvUntil,timezone,indefinite)] 

#define DBM_Q_USER_BY_INTID                                         1070     //44<4> reqId numIds<userIntId>
#define DBM_A_USER_BY_INTID                                         (DBM_DELTA + DBM_Q_USER_BY_INTID) // 2070     //46[s|4<User>] reqId errCode[errStr|numUsers<User>] 

#define DBM_Q_GET_FAKEMACS                                          1071     //4 reqId
#define DBM_A_GET_FAKEMACS                                          (DBM_DELTA + DBM_Q_GET_FAKEMACS) // 2071     //46[s|4<s>] reqId errCode [errStr | numMacs<macId>] 

#define DBM_Q_GET_TRACKCODES                                        1072    // 4 - reqId
#define DBM_A_GET_TRACKCODES                                        (DBM_DELTA + DBM_Q_GET_TRACKCODES) // 2072  //46[s|4<s444>] - reqId, errCode[errDescr|numCodes<trackcode, status, userTrackCodeDb, trackCodeFlags>] 

#define DBM_Q_UPDATE_HOLD                                           1073 	// 4[s4]4<41sss4>  reqId [adminId adminIntId] num_updates<transId, newstatus, xTransStr, errorInfo, errCode, eCashtransTableId>
#define DBM_A_UPDATE_HOLD                                           (DBM_DELTA + DBM_Q_UPDATE_HOLD) // 2073     // 46[s|4<46s1>]  reqId errCode[errDescr|num_updates<transId, err_code, err_descr, newStatus>] 

// PSIP gateway to OLTP
#define DBM_Q_CHANGE_IMAGES                                         1074     // 44<s4> - reqId, numUsers<userId, imageId>
#define DBM_A_CHANGE_IMAGES                                         (DBM_DELTA + DBM_Q_CHANGE_IMAGES) // 2074     // 46[s|4<s6s>] - reqId, errCode [errDescr|numUsers<userId, errCode, errDescr>] 

// Lobby to DBM
#define DBM_Q_UPDATE_FLYER                                          1075     // 41444s[4] - reqId, action, attrib, flags, locale, text, siteId
#define DBM_A_UPDATE_FLYER                                          (DBM_DELTA + DBM_Q_UPDATE_FLYER) // 2075     // 46[s] - reqId, errCode [errDescr] 

// Security alerts: collusion to OLAP
#define DBM_Q_MAA_DEEP_RESEARCH                                     1076     // 444<s> - reqId, checkMask, numUsers<userId>
#define DBM_A_MAA_DEEP_RESEARCH                                     (DBM_DELTA + DBM_Q_MAA_DEEP_RESEARCH) // 2076     // 46[s|sssssssD44<4sssssssD4>] reqId errCode[errDescr|originalUser numUsers <associatedUser alertId>] 

// TODO: UG delete this message when web stops to send it
#define DBM_Q_GET_BOP_TLB                                           1077  // 44T4 - reqId, intervalType, when, maxRecords
#define DBM_A_GET_BOP_TLB                                           (DBM_DELTA + DBM_Q_GET_BOP_TLB) // 2077  // 46[s|TT4<444<s4>>] reqId, errCode[errDescr | srvFrom, srvTo, numIntervals<planetKey, volumeType, numPoints<userId, points>>] 

// ATAS to OLAP
#define DBM_Q_ATAS_GET_USER_INFO                                    1078     // 4s - reqId, userId
#define DBM_A_ATAS_GET_USER_INFO                                    (DBM_DELTA + DBM_Q_ATAS_GET_USER_INFO) // 2078     // 46[s|User] reqId, errCode[errDescr|User] 

#define DBM_Q_DOUBLE_USER2                                          1079  //4s4ss4 reqId, adminId, adminIntId, oldUserId, newUserId, newUserIntId
#define DBM_A_DOUBLE_USER2                                          (DBM_DELTA + DBM_Q_DOUBLE_USER2) // 2079

#define DBM_Q_USER_PENDING_CASHOUTS                                 1080     //4s reqId userId
#define DBM_A_USER_PENDING_CASHOUTS                                 (DBM_DELTA + DBM_Q_USER_PENDING_CASHOUTS) // 2080     //46[s|4<B>] reqId errCode [errDescr|numTrans<msgBody> //each msgBody is one CashTrans (user ParseFromDbmFull()) 

#define DBM_Q_CANCEL_PENDING                                        1081 	//4s4<4> reqId userId numTrans<transId>
#define DBM_A_CANCEL_PENDING                                        (DBM_DELTA + DBM_Q_CANCEL_PENDING) // 2081 	//46[s|4<46[s|s4s4]> reqId errCode[errDescr|numTrans<transId errCode[errDescr|paysystem amount currency converted]>] 

#define DBM_Q_AGE_VERIFIED_192                                      1082  //444<s,s>(I)8 reqId, userId, status, numPairs, <name, value>, AgeVerifyInfo, appLoginId
#define DBM_A_AGE_VERIFIED_192                                      (DBM_DELTA + DBM_Q_AGE_VERIFIED_192) // 2082  //46[s|44] reqId errCode [errStr|flags,updateId] 

// Template Server to read-only OLAP
#define DBM_Q_GET_USERS_INT_IDS_BY_TRACKCODE                        1083 	// 4s - reqId, trackCode
#define DBM_A_GET_USERS_INT_IDS_BY_TRACKCODE                        (DBM_DELTA + DBM_Q_GET_USERS_INT_IDS_BY_TRACKCODE) // 2083 	// 46[s|4<4>] - reqId, errCode [errDescr|numUsers<userIntId>] 

// Admin to DBM
#define DBM_Q_READ_VELOCITY_CONTROL                                 1084  //4 reqId
#define DBM_A_READ_VELOCITY_CONTROL                                 (DBM_DELTA + DBM_Q_READ_VELOCITY_CONTROL) // 2084  //46[s] - reqId,errCode[errDescr] 

//cashier to read-only dbm
#define DBM_Q_RESEARCH_USER_ACCT                                    1085 //4sssb reqId userId paySystem cardNum productId isCashoutAction
#define DBM_A_RESEARCH_USER_ACCT                                    (DBM_DELTA + DBM_Q_RESEARCH_USER_ACCT) // 2085  //4s444<4>4<s> reqId ResearchUserCardData 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SET_USER_PRIV2                                        1086  //4s8b reqId userId priv2ToSet action(0=clear,1=set)
#define DBM_A_SET_USER_PRIV2                                        (DBM_DELTA + DBM_Q_SET_USER_PRIV2) // 2086  //46[s!8] - reqId,errCode[errDescr!userpriv2] 

#define DBM_Q_GET_COMPLETED_TIME                                    1087 	//44 reqId transId
#define DBM_A_GET_COMPLETED_TIME                                    (DBM_DELTA + DBM_Q_GET_COMPLETED_TIME) // 2087 	//446[s|T] reqId transId errorCode[errDescr|srvTimeCompleted] 

#define DBM_Q_SAVE_ZOOM_CHAT										1088   //4446<86<4ss>> reqId blitzId replayerId n<handId n<when user, chat> >
#define DBM_A_SAVE_ZOOM_CHAT										(DBM_DELTA + DBM_Q_SAVE_ZOOM_CHAT)  // 2088 // 42[s] reqId err [errStr]

// PYR-90560
#define DBM_Q_SAVE_AGE_VERIF_URU_RESULT								1089 	//4oo8 reqId UruResults AgeVerifyInfo appLoginId
#define DBM_A_SAVE_AGE_VERIF_URU_RESULT								(DBM_DELTA + DBM_Q_SAVE_AGE_VERIF_URU_RESULT) // 2089 	//42[s|4] reqId err_code[errDescr|resultId] 

#define DBM_Q_UPDATE_WAITING                                        1090 // same as DBM_Q_UPDATE_HOLD
#define DBM_A_UPDATE_WAITING                                        (DBM_DELTA + DBM_Q_UPDATE_WAITING) // 2090     // 46[s|4<46s1>]  reqId errCode[errDescr|num_updates<transId, err_code, err_descr, newStatus>] 

#define DBM_Q_PROCESS_PASSWORDS                                     1091 	// 444<ss> - reqId, action, numUsers<userId, currentUserPwd>
#define DBM_A_PROCESS_PASSWORDS                                     (DBM_DELTA + DBM_Q_PROCESS_PASSWORDS) // 2091 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_EXCLUDEDOBJECTS                                   1092     // 4 reqId 
#define DBM_A_GET_EXCLUDEDOBJECTS                                   (DBM_DELTA + DBM_Q_GET_EXCLUDEDOBJECTS) // 2092     // 46[s|4<4s4ss>] reqId errCode[errDescr|numObjects<type strVal numVal adminId comment>] 

#define DBM_Q_ADD_EXCLUDEDOBJECTS                                   1093     // 44s4ss reqId type strVal numVal adminId comment
#define DBM_A_ADD_EXCLUDEDOBJECTS                                   (DBM_DELTA + DBM_Q_ADD_EXCLUDEDOBJECTS) // 2093     // 46[s] reqId errCode[errDescr] 

#define DBM_Q_UPDATE_SELFEXCLUSION                                  1094 	//4[s]s44444 reqID, [adminId, adminIntId], userId, newDaysActual, expireAction, flags, refId, refType !!! NOTE: HACK adminId, adminIntId sent only for NON admin connections
#define DBM_A_UPDATE_SELFEXCLUSION                                  (DBM_DELTA + DBM_Q_UPDATE_SELFEXCLUSION) // 2094 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_BIN                                               1095     // 44 reqId cardId
#define DBM_A_GET_BIN                                               (DBM_DELTA + DBM_Q_GET_BIN) // 2095     // 46[s|s] - reqId, errCode[errDescr|bin] 

#define DBM_Q_UPDATE_USER_PROMO                                     1096 	//4ss reqId userId oldPromoName newPromoname
#define DBM_A_UPDATE_USER_PROMO                                     (DBM_DELTA + DBM_Q_UPDATE_USER_PROMO) // 2096 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_STALE_CASHTRANS                                   1097     // 4D4 - reqId, filterDate, cropType
#define DBM_A_GET_STALE_CASHTRANS                                   (DBM_DELTA + DBM_Q_GET_STALE_CASHTRANS) // 2097     // 46[s|4<4>] - reqId, errCode[errDescr|numCashtrans<transId>] 

#define DBM_Q_DELETE_STALE_CASHTRANS                                1098     // 44<41BS> - reqId, numCashtrans<StaleCashtransData>
#define DBM_A_DELETE_STALE_CASHTRANS                                (DBM_DELTA + DBM_Q_DELETE_STALE_CASHTRANS) // 2098     // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_MTL_SET_DAY_TWO                                       1099     // 44T reqID, tournId, SrvTime 
#define DBM_A_MTL_SET_DAY_TWO                                       (DBM_DELTA + DBM_Q_MTL_SET_DAY_TWO) // 2099     // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SAVE_CASHTRANS_PROP                                   1100 	//4M(s4)44s8 reqId M(adminId adminIntId) transId propType propStr propInt !!! NOTE: admin info sent for all cashier->dbm connections
#define DBM_A_SAVE_CASHTRANS_PROP                                   (DBM_DELTA + DBM_Q_SAVE_CASHTRANS_PROP) // 2100 	//46[s] - reqId, errCode[errDescr] 

// Lobby to DBM
#define DBM_Q_SET_GAME_LIMIT                                        1101  // 4ss44448 - reqId, userId, currency, limit1, limitType1, limit2, limitType2, appLoginId
#define DBM_A_SET_GAME_LIMIT                                        (DBM_DELTA + DBM_Q_SET_GAME_LIMIT) // 2101  // 46[s|b44444]6[s|b44444] - reqId, errCode1[errDescr1|effectiveNow1, limit1, flags1, limitType1, autoLimitHour1, waitingIncreaseLimitDays1], errCode2[errDescr2|effectiveNow2, limit2, flags2, limitType2, autoLimitHour2, waitingIncreaseLimitDays2] 

#define DBM_Q_GET_TABLE_LIMIT                                       1102  // 4s4 - reqId, userId
#define DBM_A_GET_TABLE_LIMIT                                       (DBM_DELTA + DBM_Q_GET_TABLE_LIMIT) // 2102  // 46[s|4s4s4<s>4b44] - reqId, errCode[errDescr|limitNL, limitCurrencyNL, limitFL, limitCurrencyNF, numBalances<balanceCurrency>, autoLimitHour, hadPrevTableLimitUK, waitingIncreaseNLLimitDays, waitingIncreaseFLLimitDays] 

#define DBM_Q_GET_SCHEDULED_GAME_LIMITS                             1103  // 4 - reqId
#define DBM_A_GET_SCHEDULED_GAME_LIMITS                             (DBM_DELTA + DBM_Q_GET_SCHEDULED_GAME_LIMITS) // 2103  // 46[s|4<s44>] - reqId, errCode[errDescr|numLimits<userId, limitId, limitType>] 

#define DBM_Q_PROCESS_GAME_LIMITS                                   1104  // 4s44 - reqId, userId, limitId, limitType
#define DBM_A_PROCESS_GAME_LIMITS                                   (DBM_DELTA + DBM_Q_PROCESS_GAME_LIMITS) // 2104  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_WIRE_ACCOUNT                                      1105  //4s4 - reqId, userId
#define DBM_A_GET_WIRE_ACCOUNT                                      (DBM_DELTA + DBM_Q_GET_WIRE_ACCOUNT) // 2105  // 46[s|WireAccountInfo] - reqId, errCode[errDescr|WireAccountInfo (see shared.h)] 

#define DBM_Q_SET_USER_TIMEZONE                                     1106  //4s4 reqId userId timezone
#define DBM_A_SET_USER_TIMEZONE                                     (DBM_DELTA + DBM_Q_SET_USER_TIMEZONE) // 2106  //46[s|4] reqId errCode[errDescr|timezone] 

#define DBM_Q_SET_HOLDCASHOUT_SUSPENDRM_REVALID_BATCH               1107     //4s4<sb>4<sb>4<sb>s reqId adminId numHCOUsers<userId setNotRemove> numSuspRMUsers<userId setNotRemove> numRavlidUsers<userId setNotRemove> comment
#define DBM_A_SET_HOLDCASHOUT_SUSPENDRM_REVALID_BATCH               (DBM_DELTA + DBM_Q_SET_HOLDCASHOUT_SUSPENDRM_REVALID_BATCH) // 2107     //46[s|4<SecUserReply>4<SecUserReply>4<SecUserReply>] reqId errCode[errDescr|numUsers<SecUserReply> numUsers<SecUserReply> numUsers<SecUserReply>] 

#define DBM_Q_SET_AUTH_FLAGS                                        1108  // 4s48 - reqId, userId, eAuthFlagsAction, appLoginId
#define DBM_A_SET_AUTH_FLAGS                                        (DBM_DELTA + DBM_Q_SET_AUTH_FLAGS) // 2108  // 46[s] - reqId, errCode[errDescr] 

#define DBM_QS_CHECK_USERID                                          1109  // 4sb - reqId, userId, doReturnUserIfExists
#define DBM_AS_CHECK_USERID                                          (DBM_DELTA + DBM_QS_CHECK_USERID) // 2109  // 46[s|s4<s>]M - reqId, errCode[errDescr|userId numIds<suggestedUserId>] M(userIfRequestedAndFound_orEmptyBody)

// PYR-34533
#define DBM_Q_GET_ES_USER_WINNINGS									1110  // 44t - q_id, userIntId, loginTime
#define DBM_A_GET_ES_USER_WINNINGS									(DBM_DELTA + DBM_Q_GET_ES_USER_WINNINGS) // 2110  //46[s|88] - q_id, err [errDescr| totalPlayed,totalWon] 

#define DBM_Q_LDBM_COUNTRY_BY_IP                                    1111  //4s reqId, userIp
#define DBM_A_LDBM_COUNTRY_BY_IP                                    (DBM_DELTA + DBM_Q_LDBM_COUNTRY_BY_IP) // 2111  //46[s|sbs] - reqId, errCode[errDescr|countryCode wasOverriden stateByIp] 

#define DBM_Q_LDBM_GET_IP_OVERRIDES                                 1112  //4 reqId
#define DBM_A_LDBM_GET_IP_OVERRIDES                                 (DBM_DELTA + DBM_Q_LDBM_GET_IP_OVERRIDES) // 2112  //44<sssssTs> reqId numRanges<ipFrom ipTo countryOverriden> 

#define DBM_Q_LDBM_ADD_IP_OVERRIDE                                  1113  // 4sssss - reqId ipFrom ipTo country state comment
#define DBM_A_LDBM_ADD_IP_OVERRIDE                                  (DBM_DELTA + DBM_Q_LDBM_ADD_IP_OVERRIDE) // 2113  //46[s] 

#define DBM_Q_LDBM_DELETE_IP_OVERRIDE                               1114  //4 reqId ipFrom ipTo comment
#define DBM_A_LDBM_DELETE_IP_OVERRIDE                               (DBM_DELTA + DBM_Q_LDBM_DELETE_IP_OVERRIDE) // 2114  //46[s] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_LDBM_REREAD_GEO_IP                                    1115  //4
#define DBM_A_LDBM_REREAD_GEO_IP                                    (DBM_DELTA + DBM_Q_LDBM_REREAD_GEO_IP) // 2115  //46[s] 

#define DBM_Q_GET_TSS_DATA                                          1116  //4 reqId
#define DBM_A_GET_TSS_DATA                                          (DBM_DELTA + DBM_Q_GET_TSS_DATA) // 2116  //46[s|4<s>] reqId numPaysystems<paysystemName> 


// Logical equivalent of DBM_Q_USER_UPDATE with new data composing
// For updateType see eUserUpdateType in shared.h
// Update body format:
// eUserUpdateTypeAddress: ssssss - addr_1, addr_2, city, state, country, zipCode
// eUserUpdateTypeEmail: s - email
// eUserUpdateTypePassword: s - pwd
// eUserUpdateTypeImage: 1B - imageType, ImageVarBlock
// eUserUpdateTypeFullName: s - fullName
// eUserUpdateTypePhone: s - phone
// eUserUpdateTypeRealMoney: empty
// eUserUpdateTypeFirstNameLastName: ss - firstName, lastName
// eUserUpdateTypeFiscalCode: s - fiscalCode
// eUserUpdateTypeProperties: 4<44sT> - propType, propInt, propStr, propWhen
// eUserUpdateTypeBirthday: D - birthDate
// eUserUpdateTypeImageId: 4 - imageId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_USER_UPDATE2                                          1117  // 4s4<4M>4 - reqId, userId, numUpdates<updateType, updateBody>, eUpdateType(optional)
#define DBM_A_USER_UPDATE2                                          (DBM_DELTA + DBM_Q_USER_UPDATE2) // 2117  // 46[s|s4444sssssss8bb8] - reqId, errCode[errDescr|userId, unique, imageId, effectivePrivs, flags, fullName, addr_1, addr_2, city, state, country, zipCode, effectivePrivs2, notifyItalianClient, userIsPlayMoney, effectivePrivs3] 

#define DBM_Q_GET_TOURNS_INFO                                       1118  // 44<4> - reqId, numTourns<tournId>
#define DBM_A_GET_TOURNS_INFO                                       (DBM_DELTA + DBM_Q_GET_TOURNS_INFO) // 2118  // 46[s|4<4b[bTT4s]>] reqId, errCode[errDescr|numTourns<tournId, found[isPlayMoney, startDateTime, closed, tournSites, currency]>] 

#define DBM_Q_ISSUE_LEAGUE_PRIZES                                   1119  // 44<s444s>b - reqId, numPrizes<userId, leagueId, amount, ticketTypeId, comment, currency>, isTMoney
#define DBM_A_ISSUE_LEAGUE_PRIZES                                   (DBM_DELTA + DBM_Q_ISSUE_LEAGUE_PRIZES) // 2119  // 46[s|4<s46s>] - reqId, errCode[errDescr|numErrors<userId, leagueId, errCode, errDescr>] 

#define DBM_Q_ESMIGRATION2_GET_INFO                                  1120  // 4s - reqId, userId
#define DBM_A_ESMIGRATION2_GET_INFO                                  (DBM_DELTA + DBM_Q_ESMIGRATION2_GET_INFO) // 2220

#define DBM_Q_GET_BIRTH_USER_PROPS                                  1121  // 44 - reqId, userIntId
#define DBM_A_GET_BIRTH_USER_PROPS                                  (DBM_DELTA + DBM_Q_GET_BIRTH_USER_PROPS) // 2121  // 46[s|4s..] - reqId, errCode, [errDescr|propsVector] 

// Auth Server to RO DBM
#define DBM_Q_VERIFY_PWD_SID                                        1122  // 4ssss - reqId, userId, pwdEncr, sid, ssn4
#define DBM_A_VERIFY_PWD_SID                                        (DBM_DELTA + DBM_Q_VERIFY_PWD_SID) // 2122  // 46[s] - reqId, errCode[errDescr] 

// RO request via "util" and "lobby" connections
#define DBM_Q_GET_ORIGINAL_USERINTID                                1123  // 4s - reqId, userId
#define DBM_A_GET_ORIGINAL_USERINTID                                (DBM_DELTA + DBM_Q_GET_ORIGINAL_USERINTID) // 2123  // 46[s|4] - reqId, errCode[errDescr|originalUserIntId] 

#define DBM_Q_GET_CLIENTINFO_DATA                                   1124  // 4s4 - reqId, userId, numbDaysBack
#define DBM_A_GET_CLIENTINFO_DATA                                   (DBM_DELTA + DBM_Q_GET_CLIENTINFO_DATA) // 2124  // 46[s|4<...>] - reqId, errCode[errDescr|numbEntries<MONETWININFO>] 

// admin to RW dbm
#define DBM_Q_ECHECK_DELETE_SSN                                     1125  //4s1 reqId userId clearSsnNotCardnum
#define DBM_A_ECHECK_DELETE_SSN                                     (DBM_DELTA + DBM_Q_ECHECK_DELETE_SSN) // 2125  // 46[s] - reqId, errCode[errDescr] 

// AuthServer to Login DBM
#define DBM_Q_LDBM_FREEZE_INSTALLID                                 1126  // 4ss - reqId, userId, comment
#define DBM_A_LDBM_FREEZE_INSTALLID                                 (DBM_DELTA + DBM_Q_LDBM_FREEZE_INSTALLID) // 2126  // 46[s] - reqId, errCode[errDescr] 

// Lobby to Login DBM. RO.
#define DBM_Q_LDBM_GET_USER_LOGINS                                  1127  // 4ss - reqId, userId
#define DBM_A_LDBM_GET_USER_LOGINS                                  (DBM_DELTA + DBM_Q_LDBM_GET_USER_LOGINS) // 2127  // 46[s|4<18s>f] - reqId, errCode[errDescr|numLogins<objectType, objectId, ipAddr>,allowPlayRM] 

#define DBM_Q_MIGRATION_SILENT_TO_EU_PERFORM                        1128
#define DBM_A_MIGRATION_SILENT_TO_EU_PERFORM                        (DBM_DELTA + DBM_Q_MIGRATION_SILENT_TO_EU_PERFORM) // 2128

#define DBM_Q_SAVE_USER_PROPERTY                                    1129  // 44<..>s8  - reqId, userIntId, UserProperty, audit comment, appLoginId
#define DBM_A_SAVE_USER_PROPERTY                                    (DBM_DELTA + DBM_Q_SAVE_USER_PROPERTY) // 2129  // 46[s|b]   - reqId, errCode[errDescr|found] 

#define DBM_Q_READ_PROPERTIES                                       1130  // 4s      - reqId, propertyIdlike
#define DBM_A_READ_PROPERTIES                                       (DBM_DELTA + DBM_Q_READ_PROPERTIES) // 2130  // 46[s|4..] - reqId, errCode[errDescr|size [Property1, ....]] 

#define DBM_Q_SAVE_PROPERTY                                         1131  // 4<..>   - reqId, Property
#define DBM_A_SAVE_PROPERTY                                         (DBM_DELTA + DBM_Q_SAVE_PROPERTY) // 2131  // 46[s]     - reqId, errCode[errDescr] 

// #21732
#define DBM_Q_GET_BANNED_USER_LOGINS                                1132  // 4s4<s>4<s> - reqId, userId, <installIds>, <macIds>
#define DBM_A_GET_BANNED_USER_LOGINS                                (DBM_DELTA + DBM_Q_GET_BANNED_USER_LOGINS) // 2132  // 46[s|4<M({})>4<M({})>] - reqId, errCode, [errDescr | nInstallIds<M({LoginStats})>, nMacs<M({LoginStats})>] 

#define DBM_Q_NEED_TABLE_DATA                                       1133  // 4 - reqId
#define DBM_A_NEED_TABLE_DATA                                       (DBM_DELTA + DBM_Q_NEED_TABLE_DATA) // 2133  // 4 

// From Lobby
#define DBM_Q_GET_TABLE_NAMES                                       1134  // 4 - reqId
#define DBM_A_GET_TABLE_NAMES                                       (DBM_DELTA + DBM_Q_GET_TABLE_NAMES) // 2134  // 46[s|4<4s>] - reqId, errCode[errDescr|numNames<nameId, name>] 

#define DBM_Q_ESMIGRATION2_COLLECT_AND_CASHOUT                      1135  //
#define DBM_A_ESMIGRATION2_COLLECT_AND_CASHOUT                      (DBM_DELTA + DBM_Q_ESMIGRATION2_COLLECT_AND_CASHOUT) // 2135  // 

// From TableDBM to RO DBM
#define DBM_Q_ALLOWED_AT_TABLE                                      1136  // ... - see userInData
#define DBM_A_ALLOWED_AT_TABLE                                      (DBM_DELTA + DBM_Q_ALLOWED_AT_TABLE) // 2136  // 46[s]ss484474M(BrandImages) - reqId, errCode[errDescr], city, userEmail, imageId, privileges2, vipStatus, userIntId, maxTablesLimit, hallOfFameLevelId, M(BrandImages)

// From TableDBM to OLTP DBM
#define DBM_Q_UPDATE_PLAYCHIPS                                      1137  // 4M(s)s84848 - reqId, M(userId), userId, playChips64, transType, objectId, adminIntId, appLoginId
#define DBM_A_UPDATE_PLAYCHIPS                                      (DBM_DELTA + DBM_Q_UPDATE_PLAYCHIPS) // 2137  // 46[s|8s4] - reqId, errCode[errDescr|auditId64,userId,userIntId] 

#define DBM_Q_ADD_MORE_PLAYCHIPS                                    1138  // 4M(s)s4b484487 - reqId, M(userId), userId, playChips, autoRebuy, chipsQuantum, objectId, transType, scalePM,appSessionId,minAddAmount
#define DBM_A_ADD_MORE_PLAYCHIPS                                    (DBM_DELTA + DBM_Q_ADD_MORE_PLAYCHIPS) // 2138  // 46[s|88] - reqId, errCode[errDescr|chipsActual, auditId64] 

#define DBM_Q_GET_ID_RANGE                                          1139  //	4s4 - reqId, tableName, range
#define DBM_A_GET_ID_RANGE                                          (DBM_DELTA + DBM_Q_GET_ID_RANGE) // 2139  // 46[s|44] - reqId, errCode[errDescr|begin,end] 

// TableDBM messages
#define DBM_Q_TDBM_LOBBY_SETTINGS                                   1140  // 4M - reqId, tableDbmData
#define DBM_A_TDBM_LOBBY_SETTINGS                                   (DBM_DELTA + DBM_Q_TDBM_LOBBY_SETTINGS) // 2140  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_TDBM_TABLE_CLOSED                                     1141  // 44 - reqId, tableId
#define DBM_A_TDBM_TABLE_CLOSED                                     (DBM_DELTA + DBM_Q_TDBM_TABLE_CLOSED) // 2141  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_USER_DATA                                         1142  // 44{s,...} - reqId, numUsers, {userId,...}
#define DBM_A_GET_USER_DATA                                         (DBM_DELTA + DBM_Q_GET_USER_DATA) // 2142  // 44{s4m,..} reqId, numUsers {{userid,errCode,msgData}...} 

#define DBM_Q_ESMIGRATION2_PERFORM                                  1143  //
#define DBM_A_ESMIGRATION2_PERFORM                                  (DBM_DELTA + DBM_Q_ESMIGRATION2_PERFORM) // 2143  // 

#define DBM_Q_FULL_ACCOUNT_TRANSFER                                 1144  // 4ss UserBalanceDatas s UserBalanceData UserBalanceData - reqId,userFrom,commentUserFrom,UserFromBalanceData,userTo,commentsUserTo,userToBalanceData,transferBalanceData
#define DBM_A_FULL_ACCOUNT_TRANSFER                                 (DBM_DELTA + DBM_Q_FULL_ACCOUNT_TRANSFER) // 2144  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_MTL_MASS_REGISTER_READDBM                             1146  // 
#define DBM_A_MTL_MASS_REGISTER_READDBM                             (DBM_DELTA + DBM_Q_MTL_MASS_REGISTER_READDBM) // 2146  //  

#define DBM_Q_MTL_MASS_UNREGISTER_READDBM                           1147  // 
#define DBM_A_MTL_MASS_UNREGISTER_READDBM                           (DBM_DELTA + DBM_Q_MTL_MASS_UNREGISTER_READDBM) // 2147  //  

#define DBM_Q_MTL_MASS_REGISTER_WRITEDBM                            1148  // 
#define DBM_A_MTL_MASS_REGISTER_WRITEDBM                            (DBM_DELTA + DBM_Q_MTL_MASS_REGISTER_WRITEDBM) // 2148  //  

#define DBM_Q_MTL_MASS_UNREGISTER_WRITEDBM                          1149  // 
#define DBM_A_MTL_MASS_UNREGISTER_WRITEDBM                          (DBM_DELTA + DBM_Q_MTL_MASS_UNREGISTER_WRITEDBM) // 2149  //  

#define DBM_Q_MTL_REGISTER_USER_READDBM                             1150 
#define DBM_A_MTL_REGISTER_USER_READDBM                             (DBM_DELTA + DBM_Q_MTL_REGISTER_USER_READDBM) // 2150  

#define DBM_Q_MTL_REGISTER_USER_WRITEDBM                            1151 
#define DBM_A_MTL_REGISTER_USER_WRITEDBM                            (DBM_DELTA + DBM_Q_MTL_REGISTER_USER_WRITEDBM) // 2151  

#define DBM_Q_MTL_UNREGISTER_USER_READDBM                           1152 
#define DBM_A_MTL_UNREGISTER_USER_READDBM                           (DBM_DELTA + DBM_Q_MTL_UNREGISTER_USER_READDBM) // 2152  

#define DBM_Q_MTL_UNREGISTER_USER_WRITEDBM                          1153 
#define DBM_A_MTL_UNREGISTER_USER_WRITEDBM                          (DBM_DELTA + DBM_Q_MTL_UNREGISTER_USER_WRITEDBM) // 2153  

#define DBM_Q_MTL_UNREGISTER_ALL_USER_WRITEDBM                      1154 
#define DBM_A_MTL_UNREGISTER_ALL_USER_WRITEDBM                      (DBM_DELTA + DBM_Q_MTL_UNREGISTER_ALL_USER_WRITEDBM) // 2154  

// TableDBM message
#define DBM_Q_TDBM_GET_TABTYPES                                     1155  // 4 - reqId
#define DBM_A_TDBM_GET_TABTYPES                                     (DBM_DELTA + DBM_Q_TDBM_GET_TABTYPES) // 2155  // 46[s|4...] - reqId, errCode[errDescr|tableData] 

#define DBM_Q_MTL_USERS_OUT_WRITEDBM                                1156 
#define DBM_A_MTL_USERS_OUT_WRITEDBM                                (DBM_DELTA + DBM_Q_MTL_USERS_OUT_WRITEDBM) // 2156  

#define DBM_Q_TOURN_REG_INFO_READDBM                                1157 
#define DBM_A_TOURN_REG_INFO_READDBM                                (DBM_DELTA + DBM_Q_TOURN_REG_INFO_READDBM) // 2157  

#define DBM_Q_INSERT_PGADTRANSACT                                   1158 // PYR-23644: 4 Body : reqId PgadTransactEssential
#define DBM_A_INSERT_PGADTRANSACT			                        (DBM_DELTA + DBM_Q_INSERT_PGADTRANSACT) // 2158

#define DBM_Q_MTL_CANCEL_TOURNAMENT_WRITEDBM                        1159 
#define DBM_A_MTL_CANCEL_TOURNAMENT_WRITEDBM                        (DBM_DELTA + DBM_Q_MTL_CANCEL_TOURNAMENT_WRITEDBM) // 2159  

#define DBM_Q_MTL_REGISTER_SAT_WINNERS_WRITEDBM                     1160 
#define DBM_A_MTL_REGISTER_SAT_WINNERS_WRITEDBM                     (DBM_DELTA + DBM_Q_MTL_REGISTER_SAT_WINNERS_WRITEDBM) // 2160  

// PYR-78011
#define DBM_Q_SE_GET_SPORTSBOOK_DATA_DAILY                          1161  // 4tt - reqId, start, end(inclusive)
#define DBM_A_SE_GET_SPORTSBOOK_DATA_DAILY                          (DBM_DELTA + DBM_Q_SE_GET_SPORTSBOOK_DATA_DAILY) // 2161  // 46[s|O] - reqId, errCode, [errDescr|SwedishCoreGateway::SpbPlayerGameRecord]

#define DBM_Q_LAST_HAND_ID                                          1162  // 4bbbbb reqId inPromotion.COM,inPromotion.IT,inPromotion.FR,inPromotion.ES,inPromotion.USNJ
#define DBM_A_LAST_HAND_ID                                          (DBM_DELTA + DBM_Q_LAST_HAND_ID) // 2162  // 46[s|8888] reqId, errCode [errDescr|lastHandId,lastCounter0,lastCounter1,lastCounter2,lastCounter3] 

#define DBM_Q_UPDATE_USER_PROMO_PAID                                1163 	//4ss reqId userId oldPromoName newPromoname
#define DBM_A_UPDATE_USER_PROMO_PAID                                (DBM_DELTA + DBM_Q_UPDATE_USER_PROMO_PAID) // 2163  // 46[s|8] reqId, errCode [errDescr|lastHandId] 

// TableDBM message
#define DBM_Q_TDBM_COUNT_USER_AT_TABLES                             1164  // 4sbb4 - reqId, userId, isPlayMoney, isOneOnOne, tableType
#define DBM_A_TDBM_COUNT_USER_AT_TABLES                             (DBM_DELTA + DBM_Q_TDBM_COUNT_USER_AT_TABLES) // 2164  // 46[s|7777] - reqId, errCode[errDescr|count, countHU, countHTML5, countBlitz] 

#define DBM_Q_ATAS_GET_RELATED                                      1165  // 4s - reqId, userId
#define DBM_A_ATAS_GET_RELATED                                      (DBM_DELTA + DBM_Q_ATAS_GET_RELATED) // 2165  // 46[s|4<s>] reqId, errCode [errDescr|numUsers<userId>] 

#define DBM_Q_HOLMES_CHECK                                          1166  // 4ss - reqId, userId1, userId2
#define DBM_A_HOLMES_CHECK                                          (DBM_DELTA + DBM_Q_HOLMES_CHECK) // 2166  // 46[s|44] - reqId, errCode [errDescr| CC-matched RMT-matched] 

// TableDBM message
#define DBM_Q_NEED_TOURNS_DATA                                      1167  // 4 - reqId
#define DBM_A_NEED_TOURNS_DATA                                      (DBM_DELTA + DBM_Q_NEED_TOURNS_DATA) // 2167  // 4 

// TableDBM messages
#define DBM_Q_TDBM_MTL_TABLE_CLOSED                                 1168  //48 - q_id, table_id
#define DBM_A_TDBM_MTL_TABLE_CLOSED                                 (DBM_DELTA + DBM_Q_TDBM_MTL_TABLE_CLOSED) // 2168  //46[s] q_id, err [, errDescr]

#define DBM_Q_TDBM_CLOSE_TABLES                                     1169  // 44<8> - reqId, numTables<tableId>
#define DBM_A_TDBM_CLOSE_TABLES                                     (DBM_DELTA + DBM_Q_TDBM_CLOSE_TABLES) // 2169  // 46[s] - regId, errCode[errDescr]

// PYR-82746
#define DBM_Q_PT_GET_USER_UPDATES                                   1170  //4TT - reqId,startTime,endTime
#define DBM_A_PT_GET_USER_UPDATES                                   (DBM_DELTA + DBM_Q_PT_GET_USER_UPDATES) // 2170

#define DBM_Q_TDBM_SET_TOURN_DEAD                                   1171  //447 - reqId, tournId, status
#define DBM_A_TDBM_SET_TOURN_DEAD                                   (DBM_DELTA + DBM_Q_TDBM_SET_TOURN_DEAD) // 2171  //46[s] q_id, err [, errDescr]

#define DBM_Q_TDBM_RECREATE_TOURN                                   1172  //44(script) - reqId, tournId, TournScript
#define DBM_A_TDBM_RECREATE_TOURN                                   (DBM_DELTA + DBM_Q_TDBM_RECREATE_TOURN) // 2172  //46s447 req_id, err_code, errDescr, tourn_id, start_time, publishTime 

// Obsolete after 2019.Build.10, PYR-104996
// DBM_Q_AVAILABLE_MESSAGE 
//#define DBM_Q_UPDATE_BLOCKED_CARD_OWNER                             1173  // 444 - reqId, usercardId, owner(0/1)
//#define DBM_A_UPDATE_BLOCKED_CARD_OWNER                             (DBM_DELTA + DBM_A_UPDATE_BLOCKED_CARD_OWNER) // 2173  // 46[s] reqId, errCode [errDescr] 

// TournDBM to Login DBM
#define DBM_Q_LDBM_LOGOUT_FROM_OBJECTS                              1174  // 4144<8> - reqId, objectType, logoutType, numObjects<objectId>
#define DBM_A_LDBM_LOGOUT_FROM_OBJECTS                              (DBM_DELTA + DBM_Q_LDBM_LOGOUT_FROM_OBJECTS) // 2174  // 46[s] - reqId, errCode[errDescr] 

// TableDBM message
#define DBM_Q_MTL_PREREGISTER_USER									1175 // 4... - reqId, MessageInfoStructBody, tournScript, creatorUserId, userId, userIndex
#define DBM_A_MTL_PREREGISTER_USER									(DBM_DELTA + DBM_Q_MTL_PREREGISTER_USER) // 2175  446[s|4] - reqId, userIndex, errCode[errStr|userIntId]

#define DBM_Q_MTL_REGISTER_USER_CHECK								1176
#define DBM_A_MTL_REGISTER_USER_CHECK								(DBM_DELTA + DBM_Q_MTL_REGISTER_USER_CHECK) // 2176

#define DBM_Q_MTL_RECREATE_TOURNAMENT_READDBM                       1177 
#define DBM_A_MTL_RECREATE_TOURNAMENT_READDBM                       (DBM_DELTA + DBM_Q_MTL_RECREATE_TOURNAMENT_READDBM) // 2177  

#define DBM_Q_SET_RSA_ACTIVATED                                     1178  // 4s1s8 - reqId, userId, rsaActivated, adminId, appLoginId
#define DBM_A_SET_RSA_ACTIVATED                                     (DBM_DELTA + DBM_Q_SET_RSA_ACTIVATED) // 2178  // 46[s] reqId, errCode [errDescr] 

// Collusion/TSS
#define DBM_Q_TSS_GET_FRAUDULENT                                    1179  // 4s - reqId, ipAddr
#define DBM_A_TSS_GET_FRAUDULENT                                    (DBM_DELTA + DBM_Q_TSS_GET_FRAUDULENT) // 2179  // 46[s|4<s>] reqId, errCode [errDescr|numUsers<userId>] 

#define DBM_Q_TSS_GET_TOTALDEPOSIT                                  1180  // 4s - reqId, userId
#define DBM_A_TSS_GET_TOTALDEPOSIT                                  (DBM_DELTA + DBM_Q_TSS_GET_TOTALDEPOSIT) // 2180  // 46[s|4] reqId, errCode [errDescr|totalDeposit] 

//variant of DBM_Q_NEW_CHQS_UPDATE_REJECT for admin, see #8878
#define DBM_Q_NEW_CHQS_UPDATE_REJECT_WITH_ERRCODE                   1181  //44<411ss(D)ss>  reqId num_cheqs<transId, oldstatus, newstatus, xTransStr, errorInfo, whenMailed wayBill errorCode>
#define DBM_A_NEW_CHQS_UPDATE_REJECT_WITH_ERRCODE                   (DBM_DELTA + DBM_Q_NEW_CHQS_UPDATE_REJECT_WITH_ERRCODE) // 2181  //identical to DBM_A_NEW_CHQS_UPDATE_REJECT 

#define DBM_Q_ADMIN_UPDATE_CASHTRANS_FLAGS                          1182  // 4488s4s - reqId, transId, flagsOld, flagsNew, comment, eCashtransTableId, userId
#define DBM_A_ADMIN_UPDATE_CASHTRANS_FLAGS                          (DBM_DELTA + DBM_Q_ADMIN_UPDATE_CASHTRANS_FLAGS) // 2182  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_SET_CASHTRANS_FLAGS_VEC                               1183  // 44<M(4884s)> reqId numTrans<M(transId flagsOld flagsNew eCashtransTableId userId)>
#define DBM_A_SET_CASHTRANS_FLAGS_VEC                               (DBM_DELTA + DBM_Q_SET_CASHTRANS_FLAGS_VEC) // 2183  // 46[s|4<46s>] reqId errCode [errDescr | numErrors<transId errCode errDescr>] 

#define DBM_Q_SET_USER_ADMINFLAGS_VEC                               1184  // 44<{s44}> reqId numUsers<{userId adminFlagsOld adminFlagsNew}>
#define DBM_A_SET_USER_ADMINFLAGS_VEC                               (DBM_DELTA + DBM_Q_SET_USER_ADMINFLAGS_VEC) // 2184  // 46[s|4<s6s>] reqId errCode [errDescr | numErrors<userId  errCode errDescr>] 

#define DBM_Q_SET_CASHTRANS_FLAGS_MASK                              1185  // 448bss48b - reqId, transId, flagsMask, setNotDrop, comment, userId, eCashtransTableId, flags2Mask, setNotDrop2
#define DBM_A_SET_CASHTRANS_FLAGS_MASK                              (DBM_DELTA + DBM_Q_SET_CASHTRANS_FLAGS_MASK) // 2185  // 446[s] - reqId, transId, errCode [errDescr]

// [AO 2009-03-27] VipStore messages
#define DBM_Q_VIPSTORE_GET_CURRENCIES                               1186 
#define DBM_A_VIPSTORE_GET_CURRENCIES                               (DBM_DELTA + DBM_Q_VIPSTORE_GET_CURRENCIES) // 2186  

#define DBM_Q_VIPSTORE_CHECK_DEAL                                   1187 
#define DBM_A_VIPSTORE_CHECK_DEAL                                   (DBM_DELTA + DBM_Q_VIPSTORE_CHECK_DEAL) // 2187  

#define DBM_Q_VIPSTORE_CHECK_ITEM                                   1188 
#define DBM_A_VIPSTORE_CHECK_ITEM                                   (DBM_DELTA + DBM_Q_VIPSTORE_CHECK_ITEM) // 2188  

#define DBM_Q_VIPSTORE_PURCHASE                                     1189 
#define DBM_A_VIPSTORE_PURCHASE                                     (DBM_DELTA + DBM_Q_VIPSTORE_PURCHASE) // 2189  

#define DBM_Q_VIPSTORE_CANCEL_ORDERS                                1190 // 4ss44<44s48> - reqId, userId, adminId, adminIntId, num <orderId, price, comments, flags, appSessionId>
#define DBM_A_VIPSTORE_CANCEL_ORDERS                                (DBM_DELTA + DBM_Q_VIPSTORE_CANCEL_ORDERS) // 2190  

//#11236
#define DBM_Q_SET_ROLE_RELATIONS                                    1191  // 44<ss> reqId numElems<RoleRelation>
#define DBM_A_SET_ROLE_RELATIONS                                    (DBM_DELTA + DBM_Q_SET_ROLE_RELATIONS) // 2191  // 46[s] reqId, errCode [errStr] 

#define DBM_Q_SET_ROLE_HIERARCHY                                    1192  // 44<ss4> reqId numElems<RoleHierarchy>
#define DBM_A_SET_ROLE_HIERARCHY                                    (DBM_DELTA + DBM_Q_SET_ROLE_HIERARCHY) // 2192  // 46[s] reqId, errCode [errStr] 

//automation server to dbm (part of #9866)
#define DBM_Q_AUTOMATION_SET_USER                                   1193  //4s reqId userId
#define DBM_A_AUTOMATION_SET_USER                                   (DBM_DELTA + DBM_Q_AUTOMATION_SET_USER) // 2193  // 46[s] reqId, errCode [errStr] 

//#9866
#define DBM_Q_GET_TRACK_CODE_USERS                                  1194  //4s reqId trackCode
#define DBM_A_GET_TRACK_CODE_USERS                                  (DBM_DELTA + DBM_Q_GET_TRACK_CODE_USERS) // 2194  //46[s|4<s>] reqId errCode [errStr|numUsers<userId>] 

// CRM server to OLAP #10845
#define DBM_Q_GET_PLAYING_HISTORY                                   1195  // 4sTT - reqId, userId, from, to
#define DBM_A_GET_PLAYING_HISTORY                                   (DBM_DELTA + DBM_Q_GET_PLAYING_HISTORY) // 2195  // 46[s|444<T44s44444444444sss44b4444s>] - reqId, errCode[errDescr|licenseId, localeId, numRecords<when, transType, objectId, currency, chips, baseFpp, fpp, tChips, wChips, chipsAfter, vpp1After, vpp2After, fppAfter, tChipsAfter, wChipsAfter, tableName, tournName, otherUserId, game, structure, isOneToOne, smallBlind, lowBet, highBet, aamsTicketId>] 

// MTLobby to OLTP
#define DBM_Q_ROLLBACK_REBUY_ADDON                                  1196  // 4s448444444s4 - reqId, userId, eRefundType, tournId, tableId, tableChips, realMoney, playMoney, fpp, tChips, wChips, tournCurrency, scalePM
#define DBM_A_ROLLBACK_REBUY_ADDON                                  (DBM_DELTA + DBM_Q_ROLLBACK_REBUY_ADDON) // 2196  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_UPDATE_CONVMARGIN                                     1197  // 44ss14ss - reqId, actionType, curSrc, curDst, updateNotDelete, SIGNED margin x MARGIN_MULTIPLIER, adminId, comment
#define DBM_A_UPDATE_CONVMARGIN                                     (DBM_DELTA + DBM_Q_UPDATE_CONVMARGIN) // 2197  // 46[s] - reqId, errCode [errStr] 

#define DBM_Q_SET_DEFAULT_ACCOUNT                                   1198  // 4ss8 - reqId, userId, defaultAccountCurrency, appLoginId
#define DBM_A_SET_DEFAULT_ACCOUNT                                   (DBM_DELTA + DBM_Q_SET_DEFAULT_ACCOUNT) // 2198  // 46[s] reqId, errCode [errStr] 

#define DBM_Q_NEW_CURRENCY_ACCOUNT                                  1199  // 4ss48 - reqId, userId, newAccountCurrency, userIntId, appLoginId
#define DBM_A_NEW_CURRENCY_ACCOUNT                                  (DBM_DELTA + DBM_Q_NEW_CURRENCY_ACCOUNT) // 2199  // 46[s] reqId, errCode [errStr] 

#define DBM_Q_CURRENCY_EXCHANGE                                     1200  // 44ss41 - reqId, ConversionActionType, curSrc, curDst, amtSrc, doBackwardMargin
#define DBM_A_CURRENCY_EXCHANGE                                     (DBM_DELTA + DBM_Q_CURRENCY_EXCHANGE) // 2200  // 46[s|44] - reqId, errCode, [errStr | amtDstWithMargin, amtDestZeroMargin] 

#define DBM_Q_CREATE_BATCH                                          1201  // 4ss reqId gatewayName paySystemName
#define DBM_A_CREATE_BATCH                                          (DBM_DELTA + DBM_Q_CREATE_BATCH) // 2201     //46[s|4] reqId errorCode[errorDescr|batchId] 

#define DBM_Q_COMPLETE_BATCH                                        1202  // 444<4>s reqId batchId numTrans<transId>fileName
#define DBM_A_COMPLETE_BATCH                                        (DBM_DELTA + DBM_Q_COMPLETE_BATCH) // 2202     //46[s|4<46s>] reqId errorCode[errDescr|numTransWithErrors<transId errorCode errDescr> 

#define DBM_Q_UPDATE_BATCH_FILESTATUS                               1203  // 444 reqId batchId newFileStatus
#define DBM_A_UPDATE_BATCH_FILESTATUS                               (DBM_DELTA + DBM_Q_UPDATE_BATCH_FILESTATUS) // 2203     //46[s] reqId errorCode[errorDescr] 

// Lobby to RO DBM
#define DBM_Q_MIGRATION_GET_INFO                                    1204 	// 4s4 - reqId, userId, eMigrationType
#define DBM_A_MIGRATION_GET_INFO                                    (DBM_DELTA + DBM_Q_MIGRATION_GET_INFO) // 2204 	// 46[s|ss4444444444bD] - reqId, errCode[errStr|linkedUserId, linkedUserCity, cashBalance, tBalance, wBalance, fppBalance, monthlyVpp, yearlyVpp, tickets, bonuses, exchangeRate, eMigrationType, bonusEligible, birthdate] 

// Lobby to OLTP DBM
#define DBM_Q_MIGRATION_LINK                                        1205 	// 4s4sD - reqId, userId, eMigrationType, linkedUserId, birthday
#define DBM_A_MIGRATION_LINK                                        (DBM_DELTA + DBM_Q_MIGRATION_LINK) // 2205 	// Same as DBM_A_MIGRATION_GET_INFO. See errors 205, 206 

#define DBM_Q_MIGRATION_TRANSFER                                    1206 	// 4s444448 - reqId, userId, cashAmt, tAmt, wAmt, fppAmt, exchangeRate, appLoginId
#define DBM_A_MIGRATION_TRANSFER                                    (DBM_DELTA + DBM_Q_MIGRATION_TRANSFER) // 2206 	// 46[s|44ss] - reqId, errCode[errStr|cashAmtEuro, fppAmt, linkedUserId, city] 

#define DBM_Q_MIGRATION_CONVERT                                     1207 	// 4s44<44>8 - reqId, userId, exchangeRate, numTickets<ticketId, ticketValue>, appLoginId
#define DBM_A_MIGRATION_CONVERT                                     (DBM_DELTA + DBM_Q_MIGRATION_CONVERT) // 2207 	// Same as DBM_A_MIGRATION_GET_INFO + cashAmtEuro, vipStatus (44) 

// Admin to OLTP DBM
#define DBM_Q_ANNUL_MIGRATION_LINK                                  1208  // 4s4s - reqId, userId, migrationId, comment
#define DBM_A_ANNUL_MIGRATION_LINK                                  (DBM_DELTA + DBM_Q_ANNUL_MIGRATION_LINK) // 2208 	// 46[s] - reqId, errCode[errStr] 

// Lobby to RO DBM
#define DBM_Q_GET_VALID_TOURNTICKETS                                1209  // 4s - reqId, userId
#define DBM_A_GET_VALID_TOURNTICKETS                                (DBM_DELTA + DBM_Q_GET_VALID_TOURNTICKETS) // 2209 	// 46[s|4<44>] - reqId, errCode[errStr|numTickets<ticketId, ticketValue>] 

#define DBM_Q_DELETE_CURRENCY_ACCOUNT                               1210  // 4ss8 - reqId, userId, currency, appLoginId
#define DBM_A_DELETE_CURRENCY_ACCOUNT                               (DBM_DELTA + DBM_Q_DELETE_CURRENCY_ACCOUNT) // 2210 	// 46[s] - reqId, errCode [errStr] 

#define DBM_Q_CURRENCY_ACCOUNT_TRANSFER                             1211  // 4s4ss44bbb - reqId, userId, amount, currencySrc, currencyDst, chipsToCheckFrom, chipsToCheckTo, useZeroMargin, realChipsNotTChips, amountIsFromNotTo
#define DBM_A_CURRENCY_ACCOUNT_TRANSFER                             (DBM_DELTA + DBM_Q_CURRENCY_ACCOUNT_TRANSFER) // 2211 	// 46[s|4s4s] - reqId, errCode [errStr | amountTo, currencyTo, amountFrom, currencyFrom] 

// PYR-26990
#define DBM_Q_GET_SAG_DATA											1212 // 444 - reqId, tournId, scriptId
#define DBM_A_GET_SAG_DATA											(DBM_DELTA + DBM_Q_GET_SAG_DATA) // 2212 // 46[s|] - reqId, errCode[errDescr|]

#define DBM_Q_ESMIGRATION2_INIT_PURCHASE                            1213  //
#define DBM_A_ESMIGRATION2_INIT_PURCHASE                            (DBM_DELTA + DBM_Q_ESMIGRATION2_INIT_PURCHASE) // 2213  // 

//admin to dbm #11773
#define DBM_Q_CLEAN_USER_CACHE                                      1214  //44 - reqId, numSeconds: 0 means use ini file value
#define DBM_A_CLEAN_USER_CACHE                                      (DBM_DELTA + DBM_Q_CLEAN_USER_CACHE) // 2214 	// 46[s] - reqId, errCode[errStr] 

//#11389
#define DBM_Q_UPDATE_CARDBIN_DETAILS                                1215  //44<MsgBody{binDetail}> reqId numBins binDetail
#define DBM_A_UPDATE_CARDBIN_DETAILS                                (DBM_DELTA + DBM_Q_UPDATE_CARDBIN_DETAILS) // 2215 	// 46[s|4<s6s>] - reqId globalErrCode[globalErrStr|numBinErrors <bin errCode errString>] // normal reply: reqId 0 0 

#define DBM_Q_GET_MONET_VMINFO                                      1216  // 4s reqId, userId
#define DBM_A_GET_MONET_VMINFO                                      (DBM_DELTA + DBM_Q_GET_MONET_VMINFO) // 2216     // 2[s|4t] errCode, [errStr|(VM_usage_mask, VM_last updated time)] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_REFUND_SAT_WINNERS                                    1217  // 44bs444b4s4<4> - reqId, tournId, isPlayMoney, currency, buyIn, fppBuyIn, scalePM, isFromSatellite, satelliteId, targetReferemce, numUsers<userIntId>
#define DBM_A_REFUND_SAT_WINNERS                                    (DBM_DELTA + DBM_Q_REFUND_SAT_WINNERS) // 2217     // 46[s|44<s844s4>] - reqId, errCode[errStr|tournId,numUsers<userId, pmRefund, fppRefund, tChipsRefund, refundCurrency, returnedTicketId>] 

#define DBM_Q_GET_CONVRATES_TO_USD                                  1218  // 4 reqId
#define DBM_A_GET_CONVRATES_TO_USD                                  (DBM_DELTA + DBM_Q_GET_CONVRATES_TO_USD) // 2218 	// 44<s4> - reqId, size <currency, rateToUsd*CONV_RATE_BASE> 

#define DBM_Q_ADD_GROUPMONEY_CATEGORY                               1219  //44s reqId catId groupName
#define DBM_A_ADD_GROUPMONEY_CATEGORY                               (DBM_DELTA + DBM_Q_ADD_GROUPMONEY_CATEGORY) // 2219 	//46[s] reqId errCode[errDescr] 

#define DBM_Q_DEL_GROUPMONEY_CATEGORY                               1220  //44s reqId catId groupName
#define DBM_A_DEL_GROUPMONEY_CATEGORY                               (DBM_DELTA + DBM_Q_DEL_GROUPMONEY_CATEGORY) // 2220 	//46[s] reqId errCode[errDescr] 

#define DBM_Q_ESMIGRATION2_COMPLETE_PURCHASE                        1221  //
#define DBM_A_ESMIGRATION2_COMPLETE_PURCHASE                        (DBM_DELTA + DBM_Q_ESMIGRATION2_COMPLETE_PURCHASE) // 2221  // 

#define DBM_Q_DEAD_ACCOUNT_TRANSFER                                 1222  // 4s - reqId, userId
#define DBM_A_DEAD_ACCOUNT_TRANSFER                                 (DBM_DELTA + DBM_Q_DEAD_ACCOUNT_TRANSFER) // 2222     //46[s] reqId errCode[errDescr] 

#define DBM_Q_DELETE_CURRENCY_ACCOUNT_CHECK                         1223  // 4ssb - reqId, userId, currency, disregradBonuses
#define DBM_A_DELETE_CURRENCY_ACCOUNT_CHECK                         (DBM_DELTA + DBM_Q_DELETE_CURRENCY_ACCOUNT_CHECK) // 2223  // 46[s] - reqId, errCode [errStr] 

#define DBM_Q_GET_PERSISTENCE_RECORDS                               1224 	// 4D - reqId, filterDate
#define DBM_A_GET_PERSISTENCE_RECORDS                               (DBM_DELTA + DBM_Q_GET_PERSISTENCE_RECORDS) // 2224 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DELETE_PERSISTENCE_RECORDS                            1225 	// 44<8> - reqId, numIds<id>
#define DBM_A_DELETE_PERSISTENCE_RECORDS                            (DBM_DELTA + DBM_Q_DELETE_PERSISTENCE_RECORDS) // 2225 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_HOLDCASHOUT_AND_CLOSE_BATCH                           1226 	// 4ss4<s>4<s> - reqId, adminId, comment, numHold<userId>, numClose<userId>
#define DBM_A_HOLDCASHOUT_AND_CLOSE_BATCH                           (DBM_DELTA + DBM_Q_HOLDCASHOUT_AND_CLOSE_BATCH) // 2226 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SET_UNLIMITED_OBJECT                                  1227 	// 4b4ss - reqId, setNotRemove, type, value, comments
#define DBM_A_SET_UNLIMITED_OBJECT                                  (DBM_DELTA + DBM_Q_SET_UNLIMITED_OBJECT) // 2227 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SET_USER_ADMINFLAGS                                   1228 	// 4s44 reqId, userId, adminFlagsOld, adminFlagsNew
#define DBM_A_SET_USER_ADMINFLAGS                                   (DBM_DELTA + DBM_Q_SET_USER_ADMINFLAGS) // 2228 	// 46[s] - reqId, errCode[errDescr]

#define DBM_Q_SET_RELATED_GROUP                                     1229 	// 44<sb> - reqId, adminId, numUsers<userId, setNotRemove>
#define DBM_A_SET_RELATED_GROUP                                     (DBM_DELTA + DBM_Q_SET_RELATED_GROUP) // 2229 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_CURRENCY_MULTI_ACCOUNT_TRANSFER                       1230  //4s4<4s4s> - reqId, userId, numTransfers<amountSrc, currencySrc, amountDst, currencyDst>
#define DBM_A_CURRENCY_MULTI_ACCOUNT_TRANSFER                       (DBM_DELTA + DBM_Q_CURRENCY_MULTI_ACCOUNT_TRANSFER) // 2230  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_ADMIN_USER_RIGHTS                                     1231     //4s reqId userId
#define DBM_A_ADMIN_USER_RIGHTS                                     (DBM_DELTA + DBM_Q_ADMIN_USER_RIGHTS) // 2231 	// 46[s|<User>4<s>] - reqId, errCode[errDescr|cashedUser numRights<rightStr>] 

#define DBM_Q_ADMIN_UPDATE_DESCRIPTORS                              1232 	//4s4<4> reqId numTransacts newDescriptor <transId>
#define DBM_A_ADMIN_UPDATE_DESCRIPTORS                              (DBM_DELTA + DBM_Q_ADMIN_UPDATE_DESCRIPTORS) // 2232 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SAVE_DYNAMIC_DESCRIPTOR                               1233 	//44ss reqId transId descriptor gatewayName
#define DBM_A_SAVE_DYNAMIC_DESCRIPTOR                               (DBM_DELTA + DBM_Q_SAVE_DYNAMIC_DESCRIPTOR) // 2233 	// 46[s|14] - reqId, errCode[errDescr|reportDescriptor descriptorFlags] 

#define DBM_Q_UPDATE_GATEWAY_DESCRIPTOR                             1234 	//4ss44 reqId gateway descriptor oldFlags newFlags
#define DBM_A_UPDATE_GATEWAY_DESCRIPTOR                             (DBM_DELTA + DBM_Q_UPDATE_GATEWAY_DESCRIPTOR) // 2234 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_CREATE_CASHERRORGROUP                                 1235 	//4ss4 reqId groupName description flags
#define DBM_A_CREATE_CASHERRORGROUP                                 (DBM_DELTA + DBM_Q_CREATE_CASHERRORGROUP) // 2235 	// 46[s|4] - reqId, errCode[errDescr|groupId] 

#define DBM_Q_UPDATE_CASHERRORGROUP                                 1236 	//44ss4 reqId groupId groupName description flags
#define DBM_A_UPDATE_CASHERRORGROUP                                 (DBM_DELTA + DBM_Q_UPDATE_CASHERRORGROUP) // 2236 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SAVE_CASHERROR                                        1237 	//4sss4 reqId gateway paysystem transType gatewayCode groupId
#define DBM_A_SAVE_CASHERROR                                        (DBM_DELTA + DBM_Q_SAVE_CASHERROR) // 2237 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DELETE_CASHERROR                                      1238 	//4sss4 reqId gateway paysystem transType gatewayCode 
#define DBM_A_DELETE_CASHERROR                                      (DBM_DELTA + DBM_Q_DELETE_CASHERROR) // 2238 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_REVOKE_ADMIN_PRIV                                     1239 	// 4s44 - reqId, adminId, adminLoginId, eUpdateType
#define DBM_A_REVOKE_ADMIN_PRIV                                     (DBM_DELTA + DBM_Q_REVOKE_ADMIN_PRIV) // 2239 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_USERS_PEND_CASHOUTS_IN_STATUS                     1240 	//4s4<s> reqId status numUsers<userId>
#define DBM_A_GET_USERS_PEND_CASHOUTS_IN_STATUS                     (DBM_DELTA + DBM_Q_GET_USERS_PEND_CASHOUTS_IN_STATUS) // 2240 	// 46[s|4< s4<4> >] reqId errCode[ errDescr | numUsers<userId numTrans<transId> >] 

#define DBM_Q_SET_STATUS_FOR_TRANSIDS                               1241 	//4ss4<4> reqId oldStatus newStatus numTrans<transId>
#define DBM_A_SET_STATUS_FOR_TRANSIDS                               (DBM_DELTA + DBM_Q_SET_STATUS_FOR_TRANSIDS) // 2241 	// 46[s|4<46s>] reqId errCode[errDescr|errCount<transId transErrCode transErrDescr> normally should be reqId 0 0  

// PYR-122184
#define DBM_Q_GET_VERIFIED_CARDS									1242 // 4s reqId userId
#define DBM_A_GET_VERIFIED_CARDS									(DBM_DELTA + DBM_Q_GET_VERIFIED_CARDS) // 2242 - 42[s|s4<4>] - reqId errCode[errDescr|userId count<ccId>]

// PYR-54466 - Auth Server to RO DBM
#define DBM_Q_VERIFY_SID                                            1243  // 4ss - reqId, userId, sid
#define DBM_A_VERIFY_SID                                            (DBM_DELTA + DBM_Q_VERIFY_SID) // 2243  // 46[s] - reqId, errCode[errDescr] 

// Admin to OLTP
#define DBM_Q_ADD_ADMINRIGHT                                        1244  // 44sss - reqId, righttype, rightName, rightdisplayname, rightdescription
#define DBM_A_ADD_ADMINRIGHT                                        (DBM_DELTA + DBM_Q_ADD_ADMINRIGHT) // 2244  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_UPDATE_ADMINRIGHT                                     1245  // 44sss - reqId, righttype, rightName, rightdisplayname, rightdescription
#define DBM_A_UPDATE_ADMINRIGHT                                     (DBM_DELTA + DBM_Q_UPDATE_ADMINRIGHT) // 2245  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DEL_ADMINRIGHT                                        1246  // 4s - reqId, rightName 
#define DBM_A_DEL_ADMINRIGHT                                        (DBM_DELTA + DBM_Q_DEL_ADMINRIGHT) // 2246  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_ADD_GROUPOWNER                                        1247  // 4ss - reqId, groupName, ownerid 
#define DBM_A_ADD_GROUPOWNER                                        (DBM_DELTA + DBM_Q_ADD_GROUPOWNER) // 2247  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DEL_GROUPOWNER                                        1248  // 4ss - reqId, groupName, ownerid 
#define DBM_A_DEL_GROUPOWNER                                        (DBM_DELTA + DBM_Q_DEL_GROUPOWNER) // 2248  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_ADD_RIGHTOWNER                                        1249  // 4ss - reqId, rightName, ownerid 
#define DBM_A_ADD_RIGHTOWNER                                        (DBM_DELTA + DBM_Q_ADD_RIGHTOWNER) // 2249  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DEL_RIGHTOWNER                                        1250  // 4ss - reqId, rightName, ownerid 
#define DBM_A_DEL_RIGHTOWNER                                        (DBM_DELTA + DBM_Q_DEL_RIGHTOWNER) // 2250  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SAVE_USERCARDNUM                                      1251  //44ssssss28 reqId userIntId paysystem cardnumPlain cardnumEnc (empty if not encr) bin ("" or 00+binPlain) issueNum issueDate propertyType appLoginId
#define DBM_A_SAVE_USERCARDNUM                                      (DBM_DELTA + DBM_Q_SAVE_USERCARDNUM) // 2251  // 46[s|4b] - reqId, errCode[errDescr|cc_id,isBlocked]

#define DBM_Q_DELETE_USERCARDNUM                                    1252  //4244s - reqId, propertyType, userintid, cardnumId, comment
#define DBM_A_DELETE_USERCARDNUM                                    (DBM_DELTA + DBM_Q_DELETE_USERCARDNUM) // 2252  // 46[s|4] - reqId, errCode[errDescr|cc_id] 

// PYR-21877
#define DBM_Q_UPDATE_TABLE_LOGLOCATION                              1253  // 48s - reqId, tableId, logLocation
#define DBM_A_UPDATE_TABLE_LOGLOCATION                              (DBM_DELTA + DBM_Q_UPDATE_TABLE_LOGLOCATION) // 2253  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_UPDATE_ADMINGROUP                                     1254  // 4sss - reqId, groupName, groupDisplayName, groupDescription
#define DBM_A_UPDATE_ADMINGROUP                                     (DBM_DELTA + DBM_Q_UPDATE_ADMINGROUP) // 2254  // 46[s] - reqId, errCode[errDescr] 
// #22870: to RO dbm
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_USER_UPDATE2_CHECK                                    1255  // same as DBM_Q_USER_UPDATE2
#define DBM_A_USER_UPDATE2_CHECK                                    (DBM_DELTA + DBM_Q_USER_UPDATE2_CHECK) // 2255

// cashier to RO dbm
#define DBM_Q_CURRENCY_MULTI_ACCOUNT_TRANSFER_CHECK                 1256  //4s4<4s4s> - reqId, userId, numTransfers<amountSrc, currencySrc, amountDst, currencyDst>
#define DBM_A_CURRENCY_MULTI_ACCOUNT_TRANSFER_CHECK                 (DBM_DELTA + DBM_Q_CURRENCY_MULTI_ACCOUNT_TRANSFER_CHECK) // 2256  // 46[s] - reqId, errCode[errDescr,errBody] 

// DRC to Table DBM
#define DBM_Q_GET_ALLOCATED_MC                                      1257  // 4s - reqId, userId
#define DBM_A_GET_ALLOCATED_MC                                      (DBM_DELTA + DBM_Q_GET_ALLOCATED_MC) // 2257  // 46[s|48(see AmountsInCurrencies.compose())] - reqId, errCode[errDescr|allocatedFppInCents, allocatedPlayChips, AmountsInCurrencies] 

// Cashier to DBM, DRC
#define DBM_Q_CHIPS_FULL_INFO_CASHIER                               1258  // 4s - reqId, userId	
#define DBM_A_CHIPS_FULL_INFO_CASHIER                               (DBM_DELTA + DBM_Q_CHIPS_FULL_INFO_CASHIER) // 2258  //46[s|...] - reqId, errCode[errDescr|...] 

#define DBM_Q_CHECK_CASHIN_LIMITS                                   1259  //4sss4 reqId, userId, paysystem, accountCurrency, transId(or 0)
#define DBM_A_CHECK_CASHIN_LIMITS                                   (DBM_DELTA + DBM_Q_CHECK_CASHIN_LIMITS) // 2259  //46[s|444] - reqId, errCode [errStr|usedLimit day, week, month] 

#define DBM_Q_RESEARCH_USER_PLI                                     1260  //4s reqId, userId
#define DBM_A_RESEARCH_USER_PLI                                     (DBM_DELTA + DBM_Q_RESEARCH_USER_PLI) // 2260  //46[s|struct CashinLimit] 

#define DBM_Q_PURCHASE_LIMIT_INCREASE                               1261  //4ss48 reqId, userId, paysystem, nextLevelId, appLoginId
#define DBM_A_PURCHASE_LIMIT_INCREASE                               (DBM_DELTA + DBM_Q_PURCHASE_LIMIT_INCREASE) // 2261  //46[s] - reqId, errCode [errStr] 

#define DBM_Q_READ_USER_PROPERTY                                    1262  // 444 - reqId, userIntId, propType
#define DBM_A_READ_USER_PROPERTY                                    (DBM_DELTA + DBM_Q_READ_USER_PROPERTY) // 2262  // 46[s|b..] - reqId, errCode[errDescr|found [UserProperty]] 

// Lobby to main OLTP
#define DBM_Q_SET_AUTOCONVERSION_FLAGS                              1263  // 4sbb8 - reqId, userId, doConvertBuyIn, doConvertWinnings appLoginId
#define DBM_A_SET_AUTOCONVERSION_FLAGS                              (DBM_DELTA + DBM_Q_SET_AUTOCONVERSION_FLAGS) // 2263  // 46[s|4848] - reqId, errCode[errDescr|flags, flags2, privs, privs2] 

#define DBM_Q_READ_CURRENCIES                                       1264  // 4 - reqId
#define DBM_A_READ_CURRENCIES                                       (DBM_DELTA + DBM_Q_READ_CURRENCIES) // 2264  // 46[s] - reqId, errCode [errStr] 

#define DBM_QS_RETRIEVE_USERID                                       1265  // 4ssbsss - reqId email ipaddr checkChallengeAnswers answer1 answer2 ssn4 
#define DBM_AS_RETRIEVE_USERID                                       (DBM_DELTA + DBM_QS_RETRIEVE_USERID) // 2265  // 46[s|4<M>] - reqId, errCode [errStr|count<messageBody>] 

#define DBM_Q_UPDATE_CASHOUTROUTINGRULES                            1266  // 4sss - reqId gateway paysystem xmldata  #11207
#define DBM_A_UPDATE_CASHOUTROUTINGRULES                            (DBM_DELTA + DBM_Q_UPDATE_CASHOUTROUTINGRULES) // 2266  // 46[s] - reqId errCode [errStr] 

#define DBM_Q_DELETE_CASHOUTROUTINGRULES                            1267  // 4ss - reqId gateway paysystem           #11207
#define DBM_A_DELETE_CASHOUTROUTINGRULES                            (DBM_DELTA + DBM_Q_DELETE_CASHOUTROUTINGRULES) // 2267  // 46[s] - reqId errCode [errStr] 

// ... and REUSED: PYR-22031!!!
#define DBM_Q_GET_USERS_IPS                                         1268  // 44<s> reqId numUsers<userId>
#define DBM_A_GET_USERS_IPS                                         (DBM_DELTA + DBM_Q_GET_USERS_IPS) // 2268  // 46[s|4<ss>] reqId errCode [errStr|numUsers<userId ipAddr>] 

// [JV 2010-02-01] PYR 12882: Tournament Script Update
#define DBM_Q_FIND_TOURNAMENT_SCRIPT_BY_ID                          1269  // 44<4> - reqId size [tourn id...]
#define DBM_A_FIND_TOURNAMENT_SCRIPT_BY_ID                          (DBM_DELTA + DBM_Q_FIND_TOURNAMENT_SCRIPT_BY_ID) // 2269  // 446[s|4<46[s|m]> - reqId dbmId errCode [ errDesc | size < tournId errCode [errDesc |TournamentScript messageBody]>] 

// Table to RW DBM
#define DBM_Q_USER_RM_CHIPS_RETURN                                  1270  // 48s4s8 - reqId, tableId, userId, chips, currency, appSessionId
#define DBM_A_USER_RM_CHIPS_RETURN                                  (DBM_DELTA + DBM_Q_USER_RM_CHIPS_RETURN) // 2270  // 46[s] - reqId, errCode[errDescr] 

// AAMS cleanup
#define DBM_Q_GET_CLOSED_TABLES_FROM_LIST                           1271  // 44<8> - reqId,numTables,<tableId>
#define DBM_A_GET_CLOSED_TABLES_FROM_LIST                           (DBM_DELTA + DBM_Q_GET_CLOSED_TABLES_FROM_LIST) // 2271  // 44<8> - reqId,numTables,<tableId> 

#define DBM_Q_BMTL_UPDATE_USER_CHIPS                               1272
#define DBM_A_BMTL_UPDATE_USER_CHIPS                               (DBM_DELTA + DBM_Q_BMTL_UPDATE_USER_CHIPS) // 2272

//#define DBM_Q_TGM_REPORT_DK_FLIGHT_SAT_WINNER                      1273
//#define DBM_A_TGM_REPORT_DK_FLIGHT_SAT_WINNER                      (DBM_DELTA + DBM_Q_TGM_REPORT_DK_FLIGHT_SAT_WINNER) // 2273

#define DBM_Q_REREAD_INIDATA_FROMDB                                 1274  // 4 - reqId
#define DBM_A_REREAD_INIDATA_FROMDB                                 (DBM_DELTA + DBM_Q_REREAD_INIDATA_FROMDB) // 2274  // 46[s] - reqId, errCode[errDescr] 

// Lobby and MT Lobby to RO DBM
#define DBM_Q_GET_RELATED_USERS_GROUPS                              1275  // 44<s> - reqId, numUsers<userId>
#define DBM_A_GET_RELATED_USERS_GROUPS                              (DBM_DELTA + DBM_Q_GET_RELATED_USERS_GROUPS) // 2275  // 46[s|4<ssb>] - reqId, errCode[errDescr|numUsersGroups<userId, groupId, isSuperRelated] 

// Lobby to RO OLTP DBM
#define DBM_Q_GET_TABLE_TYPES                                       1276  // 44<4> - reqId, numTypes<tableTypeId>
#define DBM_A_GET_TABLE_TYPES                                       (DBM_DELTA + DBM_Q_GET_TABLE_TYPES) // 2276  // 46[s|4<4tabType>] - reqId, errCode[errDescr|numTypes<tableTypeId,tabType>]

// [AO 2010-04-01] #14033
#define DBM_Q_GET_TOURN_LIMIT                                       1277  // 4s4 - reqId, userId
#define DBM_A_GET_TOURN_LIMIT                                       (DBM_DELTA + DBM_Q_GET_TOURN_LIMIT) // 2277  // 46[s|4s4s4<s>4b44] - reqId, errCode[errDescr|limitTourn, limitCurrencyTourn, limitSNG, limitCurrencySNG, numBalances<balanceCurrency>, autoLimitHour, hadPrevTournLimitUK, waitingIncreaseTournLimitDays, waitingIncreaseSNGLimitDays] 

// [AN 2010-04-01] #14409
#define DBM_Q_GET_CARD_VELOCITY_DATA                                1278  // 4s4bs - reqId, userId, numMinutes, excPrePaidBin, bin
#define DBM_A_GET_CARD_VELOCITY_DATA                                (DBM_DELTA + DBM_Q_GET_CARD_VELOCITY_DATA) // 2278  // 46[s44] - reqId, errCode[errDescr|numCards|numBins] 

#define DBM_Q_OVERCREDIT_REJECT_DUP                                 1279  // 4M(s4)4sss4s1 - reqId, M(adminId, adminIntId), transid, gateway, errCodeStr, errInfo, rec, newGateway, transCounter !!! NOTE: admin info sent for all cashier->dbm connections
#define DBM_A_OVERCREDIT_REJECT_DUP                                 (DBM_DELTA + DBM_Q_OVERCREDIT_REJECT_DUP) // 2279  // 46{s|4} - reqId, errCode, {errDescr,newTransid} 

// [AO 2010-04-20] #13832
#define DBM_Q_PERSIST_MTL_TOURN_DEAL                                1280  // 44ss - reqId, tournUd, encodedPlaceDeals, encodedPersonalDeals
#define DBM_A_PERSIST_MTL_TOURN_DEAL                                (DBM_DELTA + DBM_Q_PERSIST_MTL_TOURN_DEAL) // 2280  // 46{s} - reqId, errCode, {errDescr} 

#define DBM_Q_PRE_CONVERT_ALL_USER_ASSETS                           1281  // 4s4 - reqId, userId, conversionFlags
#define DBM_A_PRE_CONVERT_ALL_USER_ASSETS                           (DBM_DELTA + DBM_Q_PRE_CONVERT_ALL_USER_ASSETS) // 2281  // 46[s|1M] reqId, errCode [errDescr|assetsConversionNeeded, preConversionContext] 

#define DBM_Q_CONVERT_ALL_USER_ASSETS                               1282  // 4s4sM - reqId, userId, conversionFlags, adminId, preConversionContext
#define DBM_A_CONVERT_ALL_USER_ASSETS                               (DBM_DELTA + DBM_Q_CONVERT_ALL_USER_ASSETS) // 2282  // 46[s|BB ] reqId, errCode [errDescr|UserAllAssetsConversion, AmountsInCurrencies] 

#define DBM_Q_TRA_LOGIN_INFO                                        1283  // 44 - reqId, userIntId
#define DBM_A_TRA_LOGIN_INFO                                        (DBM_DELTA + DBM_Q_TRA_LOGIN_INFO) // 2283  // 46[s|sssss] reqId, errCode [errStr|<TRALoginRecord>] 

// DBD to RO OLTP instance
#define DBM_Q_GET_GT_CROP_INFO                                      1284  // 4 - reqId
#define DBM_A_GET_GT_CROP_INFO                                      (DBM_DELTA + DBM_Q_GET_GT_CROP_INFO) // 2284  // 46[s|bs4] - reqId, errCode[errDescr|doRecreate, tableName, planSize] 

// DBD to special RW OLTP instance
#define DBM_Q_PROCESS_GT_CROP                                       1285  // 4bs4 - reqId, doRecreate, tableName, planSize
#define DBM_A_PROCESS_GT_CROP                                       (DBM_DELTA + DBM_Q_PROCESS_GT_CROP) // 2285  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_USERFRBANKACCOUNT                                 1286  // 4s - reqId, userId
#define DBM_A_GET_USERFRBANKACCOUNT                                 (DBM_DELTA + DBM_Q_GET_USERFRBANKACCOUNT) // 2286  // 46[s|UserFrBankAccount] - reqId, errCode[errDescr|struct UserFrBankAccount] 

#define DBM_Q_SET_USERFRBANKACCOUNT                                 1287  // 4sssbss - reqId, userId, UserFrBankAccount(see struct UserFrBankAccount in shared.h), comment
#define DBM_A_SET_USERFRBANKACCOUNT                                 (DBM_DELTA + DBM_Q_SET_USERFRBANKACCOUNT) // 2287  // 46[s|b] - reqId, errCode[errDescr|is1stTime] 

#define DBM_Q_GET_USER_TRA                                          1288  // 444444T - reqId, userIntId, tournId, award, minPlayers, minAward, eliminated
#define DBM_A_GET_USER_TRA                                          (DBM_DELTA + DBM_Q_GET_USER_TRA) // 2288  // 46[s|4<s>] - reqId, errCode[errDesc|userId awardUSD numIp<ipAddr>] 

// Lobby to Login DBM
#define DBM_Q_SET_USER_PROMO_IMAGE                                  1289  // 4sbs - reqId, userId, doSet, promoImageCode
#define DBM_A_SET_USER_PROMO_IMAGE                                  (DBM_DELTA + DBM_Q_SET_USER_PROMO_IMAGE) // 2289  // 46[s] - reqId, errCode[errDesc] 

// cashier->dbm
#define DBM_Q_SET_FR_USER_SPENDING_LIMIT                            1290  // 4s4 - reqId, userId, frSpendingLimit
#define DBM_A_SET_FR_USER_SPENDING_LIMIT                            (DBM_DELTA + DBM_Q_SET_FR_USER_SPENDING_LIMIT) // 2290  // 46[s] - reqId, errCode[errDesc] 

// Lobby to Login DBM
#define DBM_Q_GET_USER_PROMO_IMAGE                                  1291  // 4s - reqId, userId
#define DBM_A_GET_USER_PROMO_IMAGE                                  (DBM_DELTA + DBM_Q_GET_USER_PROMO_IMAGE) // 2291  // 46[s|s] - reqId, errCode[errDesc|promoImageCode] 

// cashier to DBM
#define DBM_Q_FRMIGRATION_GET_INFO                                  1292  // 4s4s - reqId, userId, eMigrationType, linkedUserId
#define DBM_A_FRMIGRATION_GET_INFO                                  (DBM_DELTA + DBM_Q_FRMIGRATION_GET_INFO) // 2292  // 

#define DBM_Q_FRMIGRATION_CHECK                                     1293  // 4sM  - reqId, userId, RO context
#define DBM_A_FRMIGRATION_CHECK                                     (DBM_DELTA + DBM_Q_FRMIGRATION_CHECK) // 2293  // 

#define DBM_Q_FRMIGRATION_RW_STEP1                                  1294  //
#define DBM_A_FRMIGRATION_RW_STEP1                                  (DBM_DELTA + DBM_Q_FRMIGRATION_RW_STEP1) // 2294  // 

#define DBM_Q_FRMIGRATION_RW_STEP2                                  1295  //
#define DBM_A_FRMIGRATION_RW_STEP2                                  (DBM_DELTA + DBM_Q_FRMIGRATION_RW_STEP2) // 2295  // 

#define DBM_Q_FRMIGRATION_COMPLETE_PURCHASE                         1296  //
#define DBM_A_FRMIGRATION_COMPLETE_PURCHASE                         (DBM_DELTA + DBM_Q_FRMIGRATION_COMPLETE_PURCHASE) // 2296  // 

// PYR-14428 Admin to DBM
#define DBM_Q_MARK_AVC_SENT                                         1297  // 44sbs	- reqId,userId,sent/not sent,comment
#define DBM_A_MARK_AVC_SENT                                         (DBM_DELTA + DBM_Q_MARK_AVC_SENT) // 2297  // 46[s] - reqId, errCode[errDesc] 

#define DBM_Q_USER_DOCUMENTS_VALIDATED                              1298  // 44ss	- reqId,userId,action,comment
#define DBM_A_USER_DOCUMENTS_VALIDATED                              (DBM_DELTA + DBM_Q_USER_DOCUMENTS_VALIDATED) // 2298  // 46[s] - reqId, errCode[errDesc] 

// VIP Store to RO DBM
#define DBM_Q_GET_MIGRATED_USER_INFO                                1299  // 4s - reqId, userId
#define DBM_A_GET_MIGRATED_USER_INFO                                (DBM_DELTA + DBM_Q_GET_MIGRATED_USER_INFO) // 2299  // 46[s|s4bt] - reqId, errCode[errDesc|migratedFromUserId, migratedFromUserIntId, isConvertedToFrench, convertedToFrenchTime] 

// #14235 - ARJEL Events (Lobby to DBM)
#define DBM_Q_GET_USER_TRANSACT_LASTREQUESTED                       1300 
#define DBM_A_GET_USER_TRANSACT_LASTREQUESTED                       (DBM_DELTA + DBM_Q_GET_USER_TRANSACT_LASTREQUESTED) // 2300  

#define DBM_Q_SET_USER_TRANSACT_LASTREQUESTED                       1301 
#define DBM_A_SET_USER_TRANSACT_LASTREQUESTED                       (DBM_DELTA + DBM_Q_SET_USER_TRANSACT_LASTREQUESTED) // 2301  

#define DBM_Q_GET_USER_TRANSACT_FOR_ARJEL                           1302 
#define DBM_A_GET_USER_TRANSACT_FOR_ARJEL                           (DBM_DELTA + DBM_Q_GET_USER_TRANSACT_FOR_ARJEL) // 2302  

//PYR-14427, lobby to RW OLTP
#define DBM_Q_FR_AVC_VALIDATE                                       1303  //4s48 - reqId,userId,avc, appLoginId
#define DBM_A_FR_AVC_VALIDATE                                       (DBM_DELTA + DBM_Q_FR_AVC_VALIDATE) // 2303  //46[s|bb] - reqId,errCode,[errDesc|fullyValidated,is1stTime] 

#define DBM_Q_FR_AVC_RESEND                                         1304  //4s - reqId,userId
#define DBM_A_FR_AVC_RESEND                                         (DBM_DELTA + DBM_Q_FR_AVC_RESEND) // 2304  //46[s] - reqId,errCode,[errDesc] 

// [AO 2010-06-20] #15030
#define DBM_Q_SET_USER_CASH_BANNED                                  1305  // 444<s>4<s>4<s>4<s> - reqId, adminIntId, numUsersToBan<userIds>, numUsersToUnban<userIds>, numUsersSetSafeMode<userIds>, numUsersClearSafeMode<userIds>
#define DBM_A_SET_USER_CASH_BANNED                                  (DBM_DELTA + DBM_Q_SET_USER_CASH_BANNED) // 2305  

#define DBM_Q_FRMIGRATION_REJECT_OVERCREDIT                         1306 
#define DBM_A_FRMIGRATION_REJECT_OVERCREDIT                         (DBM_DELTA + DBM_Q_FRMIGRATION_REJECT_OVERCREDIT) // 2306  

//PYR-14427, lobby to RO DBM
#define DBM_Q_GET_FR_ACC_VALIDATION                                 1307  //4s - reqId,userId
#define DBM_A_GET_FR_ACC_VALIDATION                                 (DBM_DELTA + DBM_Q_GET_FR_ACC_VALIDATION) // 2307  //46[s|<rec>] - reqId,errCode,[errDesc|<FrAccValidation record>] 

//PYR-14427, lobby to RW OLTP
#define DBM_Q_FR_ACC_VALIDATION_DEACTIVATE                          1308  //4s4 - reqId,userId,actionMask
#define DBM_A_FR_ACC_VALIDATION_DEACTIVATE                          (DBM_DELTA + DBM_Q_FR_ACC_VALIDATION_DEACTIVATE) // 2308  //46[s] - reqId,errCode,[errDesc] 

// #14430, Lobby to RO DBM
#define DBM_Q_GET_BUYIN_LIMIT                                       1309  // 4s - reqId, userId
#define DBM_A_GET_BUYIN_LIMIT                                       (DBM_DELTA + DBM_Q_GET_BUYIN_LIMIT) // 2309  // 46[s|4,4] - reqId, errCode[errDesc|limit,autoLimitHour] 

// Lobby to RW OLTP
#define DBM_Q_SET_BUYIN_LIMIT                                       1310  // 4s48 - reqId, userId, limit, appLoginId
#define DBM_A_SET_BUYIN_LIMIT                                       (DBM_DELTA + DBM_Q_SET_BUYIN_LIMIT) // 2310  // 46[s|b] - reqId, errCode[errDesc|effectiveNow] 

// Cashier to DBM
#define DBM_Q_FRMIGRATION_INIT_PURCHASE                             1311  //
#define DBM_A_FRMIGRATION_INIT_PURCHASE                             (DBM_DELTA + DBM_Q_FRMIGRATION_INIT_PURCHASE) // 2311  // 

//PYR-15205, Matching SSN
#define DBM_Q_GET_RELATED_SECNUM                                    1312  // 4ss - reqId, userId, paysystem
#define DBM_A_GET_RELATED_SECNUM                                    (DBM_DELTA + DBM_Q_GET_RELATED_SECNUM) // 2312  // 

//PYR-15339
#define DBM_Q_CLEANUP_DEAD_TABLE                                    1313  //484 - reqId,tableId,scalePM
#define DBM_A_CLEANUP_DEAD_TABLE                                    (DBM_DELTA + DBM_Q_CLEANUP_DEAD_TABLE) // 2313  //46[s] - reqId,errCode,[errDesc] 

// PYR-14750
#define DBM_Q_GET_USER_TOURNTICKETS2                                1315 	//4s reqId,userId 
#define DBM_A_GET_USER_TOURNTICKETS2                                (DBM_DELTA + DBM_Q_GET_USER_TOURNTICKETS2) // 2315 	//46[s|4<4sb4t>] reqId, err_code[errDescr| numTickets,<M(ticketd, description, multitourn, price, issued, exp)>] 

#define DBM_Q_GET_TICKET_TOURNIDS                                   1316 	//44ss4 reqId,ticketId,admission,currency,flags
#define DBM_A_GET_TICKET_TOURNIDS                                   (DBM_DELTA + DBM_Q_GET_TICKET_TOURNIDS) // 2316 	//46[s|4<4>[M(ss4)]] reqId, err_code[errDescr| numTourns<tournId>, [M(admission,currency,flags)]] - the last body only composed by dbms, but not by DRC since Lobby doesn't need it

// PYR-14700
#define DBM_Q_GET_EXPIRED_TICKETS                                   1317 	// 4D	- reqId, filterDate 
#define DBM_A_GET_EXPIRED_TICKETS                                   (DBM_DELTA + DBM_Q_GET_EXPIRED_TICKETS) // 2317 	// 46[s|4<4>]	- reqId, errCode[errDescr|numTickets<ticketId>] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_DELETE_EXPIRED_TOURN_TICKET                          1318 	// 4o- reqId, TournTicketToDelete
#define DBM_A_DELETE_EXPIRED_TOURN_TICKET                          (DBM_DELTA + DBM_Q_DELETE_EXPIRED_TOURN_TICKET) // 2318 	// 46[s]		- reqId, errCode[errDescr] 

#define DBM_Q_GET_OBSOLETE_TICKETS_TO_CLEANUP                       1319 	// 44b	- reqId, ticketType (0 if asking to find which type needs clean-up), markAsBeingCleanedUp 
#define DBM_A_GET_OBSOLETE_TICKETS_TO_CLEANUP                       (DBM_DELTA + DBM_Q_GET_OBSOLETE_TICKETS_TO_CLEANUP) // 2319 	// 46[s|44<4>]	- reqId, errCode[errDescr|ticketType, numTickets<ticketId>] 

#define DBM_Q_MARK_TICKET_TYPE_FOR_DELETION                         1320 	// 441	- reqId, ticketType, isScheduledForDeletion
#define DBM_A_MARK_TICKET_TYPE_FOR_DELETION                         (DBM_DELTA + DBM_Q_MARK_TICKET_TYPE_FOR_DELETION) // 2320 	// 46[s]		- reqId, errCode 

//PYR-15603, Possible hacking alert
#define DBM_Q_GET_POSSIBLE_HACKING                                  1321  // 4ss4b44 - reqId, userId, paysystem, cardId, chkCardFirstUse, hrsSinceNewInstall, hrsSincePwdChange
#define DBM_A_GET_POSSIBLE_HACKING                                  (DBM_DELTA + DBM_Q_GET_POSSIBLE_HACKING) // 2321  // 46[s|s] - reqId, errCode, [errDesc|paysystem] 

#define DBM_Q_SAVE_VALIDATEFAILS                                    1322 // 4ss44 - reqId,userid,paysystem,digits,attempts
#define DBM_A_SAVE_VALIDATEFAILS                                    (DBM_DELTA + DBM_Q_SAVE_VALIDATEFAILS) // 2322  //46[s] - reqId,errCode,[errDesc]  

// PYR-15565
#define DBM_Q_FIRSTUSE_CARD_ACC                                     1323  // 4s4 - reqId, userId, cardId
#define DBM_A_FIRSTUSE_CARD_ACC                                     (DBM_DELTA + DBM_Q_FIRSTUSE_CARD_ACC) // 2323  // 46[s|b] - reqId, errCode, [errDesc|firstUseCardAcc] 

//PYR-12297
#define DBM_Q_TSS_GET_FAILURE_COUNT                                 1324  // 4s4 - reqId, userId, minNumbFails
#define DBM_A_TSS_GET_FAILURE_COUNT                                 (DBM_DELTA + DBM_Q_TSS_GET_FAILURE_COUNT) // 2324  // 46[s|s44] - reqId, errCode, [errDesc|userid, minNumbFail,numFailures] 

//PYR-15236
#define DBM_Q_AUTOMATION_UNREGISTER_USER                            1325  // 4st4 - reqId, userId, timeExpires, reason
#define DBM_A_AUTOMATION_UNREGISTER_USER                            (DBM_DELTA + DBM_Q_AUTOMATION_UNREGISTER_USER) // 2325  // 46[s] - reqId, errCode, [errDesc] 

//PYR-15438
#define DBM_Q_GET_FIRSTDEPOSITS_DATA                                1326  // 4ss4444 - reqId, userId, paysystem, ruleType, transId, minAmount, minutesSince1stDeposit
#define DBM_A_GET_FIRSTDEPOSITS_DATA                                (DBM_DELTA + DBM_Q_GET_FIRSTDEPOSITS_DATA) // 2326  // 46[s|444] - reqId, errCode, [errDesc|ruleType,minAmount,AccAmount] 

#define DBM_Q_GET_DEPOSIT_VELOCITY_DATA                             1327  // 4ssb4 - reqId, userId, paysystem, approve, numbHours
#define DBM_A_GET_DEPOSIT_VELOCITY_DATA                             (DBM_DELTA + DBM_Q_GET_DEPOSIT_VELOCITY_DATA) // 2327  // 46[s|444] - reqId, errCode, [errDesc|nDeposits,nMinutes,amount] 

//PYR-15751
#define DBM_Q_EEMIGRATION_PERFORM                                   1328  // 4s84 - reqId, userId, appLoginEssentials
#define DBM_A_EEMIGRATION_PERFORM                                   (DBM_DELTA + DBM_Q_EEMIGRATION_PERFORM) // 2328  // 4s - reqId, userId 

// [AO 2010-09-08] #16063
//PYR-44179 - Changed bool to INT32 for severity
#define DBM_Q_ADD_RELATEDUSERS                                      1329  // 44<s>s4s - reqId, numUsers <userId>, groupId, severity, comment
#define DBM_A_ADD_RELATEDUSERS                                      (DBM_DELTA + DBM_Q_ADD_RELATEDUSERS) // 2329  // 46[s|4<s6s>] - reqId, errCode, [errDesc] 

// PYR-26990
#define DBM_Q_FIND_TOURNAMENTS_BY_ID								1330 // 44<4> - reqId, numTourns<tournId>
#define DBM_A_FIND_TOURNAMENTS_BY_ID								(DBM_DELTA + DBM_Q_FIND_TOURNAMENTS_BY_ID) // 2330 // 46[s|4<M(O(44st7777))>] - reqId, errCode[errDescr|numFoundTourns<M(DrcTournInfo(tournId, tournFlags, currency, startDateTime, buyIn, rake, fppBuyIn, status))>]

// PYR-15984: admin message to check if sid is expired
#define DBM_Q_ADMIN_EXPIRED_SID                                     1331  // 4 - reqID
#define DBM_A_ADMIN_EXPIRED_SID                                     (DBM_DELTA + DBM_Q_ADMIN_EXPIRED_SID) // 2331  // 46<s> - reqId, errCode, [errDesc] 

#define DBM_Q_ADMIN_RESET_SID                                       1332  // 4 - reqID
#define DBM_A_ADMIN_RESET_SID                                       (DBM_DELTA + DBM_Q_ADMIN_RESET_SID) // 2332  // 46[s|s] - reqId, errCode, [errDesc|new SID] 

// Lobby to Main and Table DBMs
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_KILL_TABLE                                            1333  // 484 - reqId, tableId, scalePM
#define DBM_A_KILL_TABLE                                            (DBM_DELTA + DBM_Q_KILL_TABLE) // 2333  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SET_USER_COMMENT_WITH_ADMINID                         1334  // 4sss reqId, userId, commentText, adminId
#define DBM_A_SET_USER_COMMENT_WITH_ADMINID                         (DBM_DELTA + DBM_Q_SET_USER_COMMENT_WITH_ADMINID) // 2334  // 46[s] - reqId, errCode[errDescr] 

//	PYR-26801
#define DBM_Q_DELETE_TOURNPROPS										1335	//	44<444> - reqId,vectorSize<tournId,locale,proptype>
#define DBM_A_DELETE_TOURNPROPS										(DBM_DELTA + DBM_Q_DELETE_TOURNPROPS)	//	2335	//	46[s] - reqId, errCode[errDescr] 

// HgDbm to OLAP
#define DBM_Q_GET_HGAME_TOURN_STATS                                 1336  // 4tt4<4> - reqId, timeFrom, timeTo, numClubs<clubId>
#define DBM_A_GET_HGAME_TOURN_STATS                                 (DBM_DELTA + DBM_Q_GET_HGAME_TOURN_STATS) // 2336  // 6[s]4<o> - reqId, errCode[errDescr], numTourns<HgTournStats> 

// PYR-15466: Blank Out User Data
#define DBM_Q_ADMIN_SET_RETAIN_USER_DATA                            1337  // 4s4s - reqID, userId, retainFlag, comment
#define DBM_A_ADMIN_SET_RETAIN_USER_DATA                            (DBM_DELTA + DBM_Q_ADMIN_SET_RETAIN_USER_DATA) // 2337  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_GET_NEXT_CROPTIME_COMMON                              1338  // 44 - reqId, cropType
#define DBM_A_GET_NEXT_CROPTIME_COMMON                              (DBM_DELTA + DBM_Q_GET_NEXT_CROPTIME_COMMON) // 2338  // 46[s]4t - reqId, errCode [errDescr | cropType, cropTime ] 

#define DBM_Q_SAVE_LAST_CROPPED_TIME_COMMON                         1339  // 44t - reqId, cropType, cropTime, index
#define DBM_A_SAVE_LAST_CROPPED_TIME_COMMON                         (DBM_DELTA + DBM_Q_SAVE_LAST_CROPPED_TIME_COMMON) // 2339  // 46[s]4t - reqId, errCode [errDescr | cropType, index ] 

// Admin to Main RW DBM
#define DBM_Q_DELETE_USER_PROPERTY                                  1340 	// 4448 - reqId, userIntId, propertyType, appLoginId
#define DBM_A_DELETE_USER_PROPERTY                                  (DBM_DELTA + DBM_Q_DELETE_USER_PROPERTY) // 2340 	// 46[s] - reqId, errCode[errDesc] 

// #15250 , Lobby to RO DBM
#define DBM_Q_GET_PLAYING_TIME_LIMIT                                1341  // 4s8 - reqId, userId, appLoginId
#define DBM_A_GET_PLAYING_TIME_LIMIT                                (DBM_DELTA + DBM_Q_GET_PLAYING_TIME_LIMIT) // 2341  // 46[s|4,4] - reqId, errCode[errDesc|limit,autoLimitHour] 

// Lobby to RW OLTP
#define DBM_Q_SET_PLAYING_TIME_LIMIT                                1342  // 4s48 - reqId, userId, limit (in minutes), appLoginId
#define DBM_A_SET_PLAYING_TIME_LIMIT                                (DBM_DELTA + DBM_Q_SET_PLAYING_TIME_LIMIT) // 2342  // 46[s|b] - reqId, errCode[errDesc|effectiveNow] 

// PYR-16277
#define DBM_Q_FETCH_CHALLENGE_QUESTIONS                             1343 	// 4s4sb - reqId, userid, userIntId, email, verifyEmail
#define DBM_A_FETCH_CHALLENGE_QUESTIONS                             (DBM_DELTA + DBM_Q_FETCH_CHALLENGE_QUESTIONS) // 2343 	// 46[s|b2<o>] - reqId, errCode[errDes| isRM, size, <userproperty vector>] 

// PYR-15453
#define DBM_Q_SAVE_OLYMPIC_SNAPSHOT                                 1344  // 444<4844> - reqId, tournId, numTables <tableIndex, tableId, player0, player1>
#define DBM_A_SAVE_OLYMPIC_SNAPSHOT                                 (DBM_DELTA + DBM_Q_SAVE_OLYMPIC_SNAPSHOT) // 2344  // 46[s] - reqId, errCode[errDesc] 

#define DBM_Q_LOAD_OLYMPIC_SNAPSHOT                                 1345  // 44 - reqId, tournId
#define DBM_A_LOAD_OLYMPIC_SNAPSHOT                                 (DBM_DELTA + DBM_Q_LOAD_OLYMPIC_SNAPSHOT) // 2345  // 46[s|4<4844>] - reqId, errCode[errDesc|numTables<tableIndex,tableId,player0,player1>] 

#define DBM_Q_GET_TSALES_EXPIRED                                    1346  // 4 - reqId
#define DBM_A_GET_TSALES_EXPIRED                                    (DBM_DELTA + DBM_Q_GET_TSALES_EXPIRED) // 2346  // 46[s|4<4ssb44s>] saleId sellerId buyerId isStartedByBuyer tChips Cash Currency 

#define DBM_Q_SET_TSALES_EXPIRED                                    1347  // 44<4> - reqId size<saleIds>
#define DBM_A_SET_TSALES_EXPIRED                                    (DBM_DELTA + DBM_Q_SET_TSALES_EXPIRED) // 2347  // 46[s] 

#define DBM_Q_TSALE_TO_PSTARS                                       1348  // 4s44s - reqId, sellerId, tChips, cash, currency
#define DBM_A_TSALE_TO_PSTARS                                       (DBM_DELTA + DBM_Q_TSALE_TO_PSTARS) // 2348  // 46[s|s] - reqId errCode [errDescr | sellerIdExact] 

#define DBM_Q_TCHIPS_SALE_CHECK                                     1349  // 4ss1444s reqId seller buyer isBuyer tAmt, wAmt, amt, currency
#define DBM_A_TCHIPS_SALE_CHECK                                     (DBM_DELTA + DBM_Q_TCHIPS_SALE_CHECK) // 2349  // 

#define DBM_Q_TSALE_TO_PSTARS_CHECK                                 1350  // 4s44s - reqId, sellerId, tChips, cash, currency
#define DBM_A_TSALE_TO_PSTARS_CHECK                                 (DBM_DELTA + DBM_Q_TSALE_TO_PSTARS_CHECK) // 2350  // 46[s] 

// #16182
#define DBM_Q_MARK_HG_PLAYER                                        1351  // 44<sb> - reqId, numUsers<userId, isHgPlayer>
#define DBM_A_MARK_HG_PLAYER                                        (DBM_DELTA + DBM_Q_MARK_HG_PLAYER) // 2351  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_USER_ACTIVATION_INFO                              1352  // 4s - reqId, userId
#define DBM_A_GET_USER_ACTIVATION_INFO                              (DBM_DELTA + DBM_Q_GET_USER_ACTIVATION_INFO) // 2352  // 46[s,s44s4s] - reqId, errCode[errDescr,email,unique,emLocale,country,siteId,promoName] 

// PYR-83113
#define DBM_Q_MIGRATION_IOM_TO_EU_PERFORM							1353
#define DBM_A_MIGRATION_IOM_TO_EU_PERFORM							(DBM_DELTA + DBM_Q_MIGRATION_IOM_TO_EU_PERFORM) // 2353

// #15466
#define DBM_Q_GET_CASHTRANS_TO_BLANK_FOR_USER                       1354  // 4s - reqId, userId
#define DBM_A_GET_CASHTRANS_TO_BLANK_FOR_USER                       (DBM_DELTA + DBM_Q_GET_CASHTRANS_TO_BLANK_FOR_USER) // 2354  // 42[s|4<4>4<4>] - reqId, errCode, [errDesc | size <transid> size <cc ids>] 

#define DBM_Q_BLANKOUT_CASHTRANS_USERDATA                           1355  // 44<4> - reqId, size <transId>
#define DBM_A_BLANKOUT_CASHTRANS_USERDATA                           (DBM_DELTA + DBM_Q_BLANKOUT_CASHTRANS_USERDATA) // 2355  // 42[s|4<4>] - reqId, errCode, [errDesc] 

#define DBM_Q_BLANKOUT_USERS_USERDATA                               1356  // 4s - reqId, userId
#define DBM_A_BLANKOUT_USERS_USERDATA                               (DBM_DELTA + DBM_Q_BLANKOUT_USERS_USERDATA) // 2356  // 42[s|4<4>] - reqId, errCode, [errDesc] 

// #17203
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_MTL_TOURN_BOUNTY                                      1357  // 44bso488OM[IceEvent] - reqId, tournId, isPlayMoney, tournCurrency, BountyEntries,scalePM,tournFlags2,finalFlightId,BountyEntries2,M[IceEvent]
#define DBM_A_MTL_TOURN_BOUNTY                                      (DBM_DELTA + DBM_Q_MTL_TOURN_BOUNTY) // 2357  // 46[s] - reqId, errCode[errDescr] 

// #17173
#define DBM_Q_ATAS_GET_HGCLUB_ITNTL_INFO                            1358  // 4s - reqId, userId
#define DBM_A_ATAS_GET_HGCLUB_ITNTL_INFO                            (DBM_DELTA + DBM_Q_ATAS_GET_HGCLUB_ITNTL_INFO) // 2358  // 46[s|b] - reqId, errCode[errDescr | bIntntlClub] 

// #16914
#define DBM_Q_UPDATE_LANG_PATTERNS                                  1359  // 444<s>b - reqId, locale, numPatterns<pattern>, whiteList
#define DBM_A_UPDATE_LANG_PATTERNS                                  (DBM_DELTA + DBM_Q_UPDATE_LANG_PATTERNS) // 2359  // 46[s] - reqId, errCode[errDescr] 

// #17049
#define DBM_Q_GET_ENTITY_VELOCITY_DATA                              1360  // 4s4ss4<...> - reqId, IP, ccid, zip, email, nEVsets<entityVelData>
#define DBM_A_GET_ENTITY_VELOCITY_DATA                              (DBM_DELTA + DBM_Q_GET_ENTITY_VELOCITY_DATA) // 2360  // 46[s|4<4(...)>] - reqId, errCode[errDescr|nSets<numb (EntityVelData)>] 

#define DBM_Q_CREATE_TGM_WRITEDBM									1361 //	4 reqId
#define DBM_A_CREATE_TGM_WRITEDBM									(DBM_DELTA + DBM_Q_CREATE_TGM_WRITEDBM) // 2361 //46 [s|4] -reqId,errCode[errMsg|tgmId]

//#
#define DBM_Q_GET_USER_RUG											1362
#define DBM_A_GET_USER_RUG											(DBM_DELTA + DBM_Q_GET_USER_RUG) //2362

// #15466
#define DBM_Q_GET_CASHTRANS_FOR_CCID                                1363  // 44s - reqId, ccid, userid
#define DBM_A_GET_CASHTRANS_FOR_CCID                                (DBM_DELTA + DBM_Q_GET_CASHTRANS_FOR_CCID) // 2363  // 46[s|b4<4>] - reqId, errCode [errDescr | shared size <transid>] 

#define DBM_Q_BLANKOUT_CARDNUMS                                     1364  // 44<4> - reqId, size <cardnumid>
#define DBM_A_BLANKOUT_CARDNUMS                                     (DBM_DELTA + DBM_Q_BLANKOUT_CARDNUMS) // 2364  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_BLANKOUT_CARDNUM_UPDATE_CASHTRANS                     1365  // 444<4> - reqId, ccid, size <transid>
#define DBM_A_BLANKOUT_CARDNUM_UPDATE_CASHTRANS                     (DBM_DELTA + DBM_Q_BLANKOUT_CARDNUM_UPDATE_CASHTRANS) // 2365  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_ACHBATCH_COMPLETED                                    1366  // 4ssssss transId, Gateway, xTransStr, status, productId, errorInfo, Descriptor 
#define DBM_A_ACHBATCH_COMPLETED                                    (DBM_DELTA + DBM_Q_ACHBATCH_COMPLETED) // 2366  // 46[s] 

// #17032
#define DBM_Q_GET_CDA_GAME_TOURNS_DATA                              1367  // 44 - reqId, userIntId
#define DBM_A_GET_CDA_GAME_TOURNS_DATA                              (DBM_DELTA + DBM_Q_GET_CDA_GAME_TOURNS_DATA) // 2367  // 46[s|s4s114444b]] - reqId, errCode [errDescr | userId ltFpp country gameType limitType smallBlind bigBlind maxNPlayers buyIn isNoTour] 

// #17382
#define DBM_Q_ATAS_GET_TRANSFER_AFFILIATE                           1368  // 4s - reqId, userId
#define DBM_A_ATAS_GET_TRANSFER_AFFILIATE                           (DBM_DELTA + DBM_Q_ATAS_GET_TRANSFER_AFFILIATE) // 2368  // 46[s|b] - reqId, errCode [errDescr | isTransferAffiliate] 

// #17767
#define DBM_Q_SET_NO_HG_EMAILS_PRIVILEGE_WITH_COMMENT               1369  // 41s4<ss> - reqId, removeNotSet, adminId, numUsers<userId, comment>
#define DBM_A_SET_NO_HG_EMAILS_PRIVILEGE_WITH_COMMENT               (DBM_DELTA + DBM_Q_SET_NO_HG_EMAILS_PRIVILEGE_WITH_COMMENT) // 2369  // 44<6s> - reqId, numUsers<errCode, errDescr> 

// #17825
#define DBM_Q_NEG_BAL_INFO                                          1370  // 44<s> - reqId, nRelated <relatedId>
#define DBM_A_NEG_BAL_INFO                                          (DBM_DELTA + DBM_Q_NEG_BAL_INFO) // 2370  // 46[s|4] - reqId, errCode [errDescr | numbNegBal] 

// PYR-92966
#define DBM_Q_ARCH_GET_RELATEDUSERS                                 1371  // protocols/ArchiveManager/ArchiveManagerProt.txt
#define DBM_A_ARCH_GET_RELATEDUSERS                                 (DBM_DELTA + DBM_Q_ARCH_GET_RELATEDUSERS) // 2371  // protocols/ArchiveManager/ArchiveManagerProt.txt

// PYR-92966
#define DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS                         1372  // protocols/ArchiveManager/ArchiveManagerProt.txt
#define DBM_A_UNARCHIVE_INSERT_RELATEDUSERS                         (DBM_DELTA + DBM_Q_UNARCHIVE_INSERT_RELATEDUSERS) // 2372  // protocols/ArchiveManager/ArchiveManagerProt.txt

// #13934 - from lobby, change jubilee hands
#define DBM_Q_SET_JUBILEE_HANDS                                     1373     // 42<42<88844>> - reqId, #sites< siteId, #handIds<jubileeHandStops, fromHandId, toHandId, vppMultiplier, minPrize> >
#define DBM_A_SET_JUBILEE_HANDS                                     (DBM_DELTA + DBM_Q_SET_JUBILEE_HANDS) // 2373     // 46[s] - reqId errCode [errDescr] 

// #23642
#define DBM_Q_ATAS_GET_TRACK_CODES                                  1374  // 4s - reqId, userId
#define DBM_A_ATAS_GET_TRACK_CODES                                  (DBM_DELTA + DBM_Q_ATAS_GET_TRACK_CODES) // 2374  // 46[s|4<s>] - reqId, errCode [errDescr | nCodes<trackCode>] 

// #17712
#define DBM_Q_GET_PREPAID_BIN                                       1375  // 4s - reqId, bin
#define DBM_A_GET_PREPAID_BIN                                       (DBM_DELTA + DBM_Q_GET_PREPAID_BIN) // 2375  // 46[s|b] - reqId, errCode [errDescr | isPrePaidBin] 

#define DBM_Q_GCWIRE_NOTIFY_WRITE                                   1376  // 4s44<M> - reqId, adminId, adminIntId, count<WireNotifyStruct>
#define DBM_A_GCWIRE_NOTIFY_WRITE                                   (DBM_DELTA + DBM_Q_GCWIRE_NOTIFY_WRITE) // 2376  // 44<M> - reqId, count<WireNotifyStruct> 

// PYR-24812, PYR-84524
#define DBM_Q_SET_BLOCKING_CODE_ONE_USER							1377  // 44ss - reqId, blockingCode, userId, comments
#define DBM_A_SET_BLOCKING_CODE_ONE_USER							(DBM_DELTA + DBM_Q_SET_BLOCKING_CODE_ONE_USER) // 2377  // 46[s] - reqId,errCode,[errDesc]

// PYR-17774
#define DBM_Q_SET_SYSTEM_RESTART_TIME                               1378  // 4t - reqId, restartTime
#define DBM_A_SET_SYSTEM_RESTART_TIME                               (DBM_DELTA + DBM_Q_SET_SYSTEM_RESTART_TIME) // 2378  // 46[s] - reqId, errCode [errDescr] 

// PYR-14242
#define DBM_Q_VALIDATE_MOBILE                                       1379  // s48 - userId,unique,appLoginId
#define DBM_A_VALIDATE_MOBILE                                       (DBM_DELTA + DBM_Q_VALIDATE_MOBILE) // 2379   //46[s|44s] - reqId, err [errDescr]  

// #18174 - Fast Deposit
#define DBM_Q_SAVE_FAST_DEPOSIT_INFO                                1380  // 4sbM8 - req userId hasActiveFastDepState fdUserProperty appLoginId
#define DBM_A_SAVE_FAST_DEPOSIT_INFO                                (DBM_DELTA + DBM_Q_SAVE_FAST_DEPOSIT_INFO) // 2380  // 46[s] 

// PYR-14242
#define DBM_Q_RECREATE_MOBILE_VALIDATION_INFO                       1381  // 4s418 - reqId, userId, adminIntId, newInstallId, appLoginId
#define DBM_A_RECREATE_MOBILE_VALIDATION_INFO                       (DBM_DELTA + DBM_Q_RECREATE_MOBILE_VALIDATION_INFO) // 2381  // 46[s|s4] - reqId, err [errDescr|mobile,unique] 


#define DBM_Q_DELETE_USER_PROPERTY_2                                1382  // 4s4 - req userId propType
#define DBM_A_DELETE_USER_PROPERTY_2                                (DBM_DELTA + DBM_Q_DELETE_USER_PROPERTY_2) // 2382  // 46[s] 

// PYR-18024
#define DBM_Q_CREATE_RING_GAME_REPLAY                               1383  // 4soB - reqId, adminId, RingGameReplayScript(parser), script
#define DBM_A_CREATE_RING_GAME_REPLAY                               (DBM_DELTA + DBM_Q_CREATE_RING_GAME_REPLAY) // 2383  // 46[s|4] - reqId, errCode, [errDescr | replayId] 

#define DBM_Q_DELETE_FAST_DEPOSIT_INFO                              1384  // 4ss8 - req userId paySystem appLoginId
#define DBM_A_DELETE_FAST_DEPOSIT_INFO                              (DBM_DELTA + DBM_Q_DELETE_FAST_DEPOSIT_INFO) // 2384  // 46[s] 

#define DBM_Q_CHECK_CASHIN_LIMITS_HIT                               1385  // 4ss4s4 - req userId paysystem procAmount procCurrency transId(or 0)
#define DBM_A_CHECK_CASHIN_LIMITS_HIT                               (DBM_DELTA + DBM_Q_CHECK_CASHIN_LIMITS_HIT) // 2385  // 46[s[M]] reqId errCode [errDescrOrInfo [limitsBody]] 

//PYR-18397
#define DBM_Q_BEMIGRATION_PERFORM                                   1386  // 4s84 - reqId, userId, appLoginEssentials
#define DBM_A_BEMIGRATION_PERFORM                                   (DBM_DELTA + DBM_Q_BEMIGRATION_PERFORM) // 2386  // 4s - reqId, userId 

// #18676 LimitController and Admin -> OLTP
#define DBM_Q_UPDATE_BE_USER_LOSS_LIMIT                             1387  // 4s4b48 - req userId beLossLimit setNotIncrement auditFlags userLoginId
#define DBM_A_UPDATE_BE_USER_LOSS_LIMIT                             (DBM_DELTA + DBM_Q_UPDATE_BE_USER_LOSS_LIMIT) // 2387  // 46[s] 

// #18502
#define DBM_Q_GET_CARD_HASH                                         1388  // 44 - reqId, cardId
#define DBM_A_GET_CARD_HASH                                         (DBM_DELTA + DBM_Q_GET_CARD_HASH) // 2388  // 46[s|s] - reqId, errCode [errDescr | cardHash] 

#define DBM_Q_GET_HRCARD_ID_HASH                                    1389  // 44<ss> - reqId, nCards<cardNum, paysystem>
#define DBM_A_GET_HRCARD_ID_HASH                                    (DBM_DELTA + DBM_Q_GET_HRCARD_ID_HASH) // 2389  // 46[s|4<ss>4<4ss>] - reqId, errCode [errDescr | nCards<cardN,Hash> nMatch<cardId,uid,cardnum>] 

// #18708
#define DBM_Q_SET_USER_PRIV                                         1390  // 4s4b - reqId, userId, privToSet, action(false=clear, true=set)
#define DBM_A_SET_USER_PRIV                                         (DBM_DELTA + DBM_Q_SET_USER_PRIV) // 2390  // 46[s|4] - reqId, errCode[errDescr|userPriv] 

//PYR-18568
#define DBM_Q_BE_AVC_VALIDATE                                       1391  //4s48 - reqId,userId,avc,appLoginid
#define DBM_A_BE_AVC_VALIDATE                                       (DBM_DELTA + DBM_Q_BE_AVC_VALIDATE) // 2391  //46[s|444] - reqId,errCode,[errDesc|flags,privs,oldActionMask] 

#define DBM_Q_BE_AVC_REGENERATE                                     1392  //4s - reqId,userId
#define DBM_A_BE_AVC_REGENERATE                                     (DBM_DELTA + DBM_Q_BE_AVC_REGENERATE) // 2392  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_GET_BE_ACC_VERIFICATION                               1393  //4s - reqId,userId
#define DBM_A_GET_BE_ACC_VERIFICATION                               (DBM_DELTA + DBM_Q_GET_BE_ACC_VERIFICATION) // 2393  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_BE_ACC_VERIFICATION_DEACTIVATE                        1394  //4s4s444t - reqId,userId,actionMask,comment,snapDeposits,snapCashouts,snapDays,snapTime
#define DBM_A_BE_ACC_VERIFICATION_DEACTIVATE                        (DBM_DELTA + DBM_Q_BE_ACC_VERIFICATION_DEACTIVATE) // 2394  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_BE_AVC_MARKSENT                                       1395  //4ss1 - reqId,userId,comment,setNotClear
#define DBM_A_BE_AVC_MARKSENT                                       (DBM_DELTA + DBM_Q_BE_AVC_MARKSENT) // 2395  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_UPDATE_ITALIAN_SPECIAL_COUNTERS                       1396  // 4sb44 req,userId,nacNotBonus,oldValue,newValue
#define DBM_A_UPDATE_ITALIAN_SPECIAL_COUNTERS                       (DBM_DELTA + DBM_Q_UPDATE_ITALIAN_SPECIAL_COUNTERS) // 2396  //46[s] 

// #18129
#define DBM_Q_MTL_CHANGE_DUMMY_TOURN                                1397  // 4sT7 reqId, adminId, startTime, maxPlayers
#define DBM_A_MTL_CHANGE_DUMMY_TOURN                                (DBM_DELTA + DBM_Q_MTL_CHANGE_DUMMY_TOURN) // 2397  // 46[s] - reqId,errCode,[errDesc] 

// PYR-24812
#define DBM_Q_SET_BLOCKING_CODE										1398  // 444<s>s - reqId, blockingCode, numUsers<userId>, comments
#define DBM_A_SET_BLOCKING_CODE										(DBM_DELTA + DBM_Q_SET_BLOCKING_CODE) // 2397  // 46[s|4<s6s>] - reqId,errCode,[errDesc | numFailed<userId,errCode,errDescr>] 

//PYR-24396
#define DBM_Q_SAVE_TOURNHAPPYHOURS									1399  //4<rec> - reqId,<struct TournHappyHours>
#define DBM_A_SAVE_TOURNHAPPYHOURS									(DBM_DELTA + DBM_Q_SAVE_TOURNHAPPYHOURS) // 2399  //46[s] - reqId,errCode,[errDesc]

// PYR-18621, lobby to RW OLTP
#define DBM_Q_GET_IT_ACC_VALIDATION                                 1400  //4s - reqId,userId
#define DBM_A_GET_IT_ACC_VALIDATION                                 (DBM_DELTA + DBM_Q_GET_IT_ACC_VALIDATION) // 2400  //46[s|<rec>] - reqId,errCode,[errDesc|<ItAccVerification record>] 

// lobby & automation to RW OLTP
#define DBM_Q_IT_ACC_VALIDATION_DEACTIVATE                          1401  //4s4s - reqId,userId,actionMask,comment
#define DBM_A_IT_ACC_VALIDATION_DEACTIVATE                          (DBM_DELTA + DBM_Q_IT_ACC_VALIDATION_DEACTIVATE) // 2401  //46[s] - reqId,errCode,[errDesc] 

//#24825
#define DBM_Q_IT_LV_GET_USERS                                    1402 // 44 - reqId,type(0-blocked,1-unprocessed)
#define DBM_A_IT_LV_GET_USERS                                    (DBM_DELTA + DBM_Q_IT_LV_GET_USERS) // 46[s|4<s4s???> - numUsers<userId,userIntId,fiscalCode,????>

//#24825
#define DBM_Q_IT_LV_SET_USER_STATUS                              1403 // 444sb - reqId,userIntId,status,comment
#define DBM_A_IT_LV_SET_USER_STATUS                              (DBM_DELTA + DBM_Q_IT_LV_SET_USER_STATUS) // 46[s]

// PYR-19282 - message to get users info by vector of userIntIds
#define DBM_Q_GET_USERS                                             1404  // 44<4> - reqId, numUsers<userIntId>
#define DBM_A_GET_USERS                                             (DBM_DELTA + DBM_Q_GET_USERS) // 2404  // 46[s] - reqId, errCode[errDescr|numUsers<(User)>]. User - userIntId, userId, firstName, lastName, fiscalCode 

#define DBM_Q_USER_BLACKLISTED                                      1405  // 4s4 - reqId, userId, licenseId
#define DBM_A_USER_BLACKLISTED                                      (DBM_DELTA + DBM_Q_USER_BLACKLISTED) // 2405  // 46[s] - reqId, errCode[errDescr] 

// PYR-38100
#define DBM_Q_GET_REGISTRATION_LOGIN_BRAND							1406 // 4ssbb - reqId, userId, userId2, needRegBrand, needLoginBrand
#define DBM_A_GET_REGISTRATION_LOGIN_BRAND							(DBM_DELTA + DBM_Q_GET_REGISTRATION_LOGIN_BRAND) // 2406 // 42[s|4444] - reqId,errCode [errDescr|regBrandId,regBrandId2,loginBrandId,loginBrandId2]

// PYR-92966
#define DBM_Q_ARCH_DELETE_RELATEDUSERS                              1407  //4s reqId userId
#define DBM_A_ARCH_DELETE_RELATEDUSERS                              (DBM_DELTA + DBM_Q_ARCH_DELETE_RELATEDUSERS) // 2407  //46[s] reqId  errCode[errDescr]

// PYR-18621
// lobby to DBM
#define DBM_Q_IT_ACC_VALIDATION_START                               1408  //4sf8 reqId,userId,notifyClient,appLoginId
#define DBM_A_IT_ACC_VALIDATION_START                               (DBM_DELTA + DBM_Q_IT_ACC_VALIDATION_START) // 2408  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_SET_USER_FLAGS2                                       1409  //4s8b448 reqId userId flags2_Mask setNotRemove eUpdateType adminIntid appLoginId
#define DBM_A_SET_USER_FLAGS2                                       (DBM_DELTA + DBM_Q_SET_USER_FLAGS2) // 2409  //46[s] - reqId, errCode[errDescr] 

//PYR-19409, lobby to RW OLTP
#define DBM_Q_FR_ACC_VALIDATION_APPEND                              1410  //4s8 - reqId,userId,appLoginId
#define DBM_A_FR_ACC_VALIDATION_APPEND                              (DBM_DELTA + DBM_Q_FR_ACC_VALIDATION_APPEND) // 2410  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_CALCULATE_USER_TTICKETS_TOTAL                         1411  //4s4 - reqId,userId,userIntId
#define DBM_A_CALCULATE_USER_TTICKETS_TOTAL                         (DBM_DELTA + DBM_Q_CALCULATE_USER_TTICKETS_TOTAL) // 2411  //46[s|4] - reqId,errCode[errDescr|TTicketsTotal]

#define DBM_Q_UPDATE_ONE_CASHTRANS_GATEWAY                          1412  //44s - reqId,transId,newGateway
#define DBM_A_UPDATE_ONE_CASHTRANS_GATEWAY                          (DBM_DELTA + DBM_Q_UPDATE_ONE_CASHTRANS_GATEWAY) // 2412  //46[s] - reqId,errCode[errDescr] 

// #19236
#define DBM_Q_GET_FIRST3_DEPOSITS                                   1413  // 4ss - reqId,userId,paysystem
#define DBM_A_GET_FIRST3_DEPOSITS                                   (DBM_DELTA + DBM_Q_GET_FIRST3_DEPOSITS) // 2413  //46[s|444444] - reqId,errCode[errDescr|daylim,amt1,amt2,amt3,t12,t23] 

// PYR-17950
#define DBM_Q_CLIENT_DOWNLOAD_STATS                                 1414  // 44<D444> - reqId, numRecords, <date, siteId, platformId, downloadCount>
#define DBM_A_CLIENT_DOWNLOAD_STATS                                 (DBM_DELTA + DBM_Q_CLIENT_DOWNLOAD_STATS) // 2414  // 46[s] q_id, err [errDescr]  

#define DBM_Q_LOBBY_REPORT_NEW_INSTALL                              1415  // 4s44T - reqId, installId, siteId, platformId, whenSrvTime
#define DBM_A_LOBBY_REPORT_NEW_INSTALL                              (DBM_DELTA + DBM_Q_LOBBY_REPORT_NEW_INSTALL) // 2415  // 46[s] q_id, err [errDescr]  

#define DBM_Q_LOBBY_REPORT_FIRST_LOGIN                              1416  // 4ss44T - reqId, installId, userId, siteId, platformId whenSrvTime
#define DBM_A_LOBBY_REPORT_FIRST_LOGIN                              (DBM_DELTA + DBM_Q_LOBBY_REPORT_FIRST_LOGIN) // 2416  // 46[s] q_id, err [errDescr]  

#define DBM_Q_REPORT_CREATE_ACCOUNT_GIVE_UP                         1417  // 4s44s - reqId, installId, siteId, platformId, accountWizardPageName
#define DBM_A_REPORT_CREATE_ACCOUNT_GIVE_UP                         (DBM_DELTA + DBM_Q_REPORT_CREATE_ACCOUNT_GIVE_UP) // 2417  // 46[s] q_id, err [errDescr] 

// #19167
#define DBM_Q_MTL_UPDATE_FLIGHT_TOURN_STATS                         1418  // 44oo - reqId, tournId, FlightTournInfo(parser), newFlightTournInfo(or dummy for MainDbm)
#define DBM_A_MTL_UPDATE_FLIGHT_TOURN_STATS                         (DBM_DELTA + DBM_Q_MTL_UPDATE_FLIGHT_TOURN_STATS) // 2418  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_MTL_ALREADY_ELIMINATED_USERS_OUT                      1419  // See compose/parse code
#define DBM_A_MTL_ALREADY_ELIMINATED_USERS_OUT                      (DBM_DELTA + DBM_Q_MTL_ALREADY_ELIMINATED_USERS_OUT) // 2419  // //46[s]4<s1444(T)> req_id, err_code [errDescr], numWinners <userId registrationType registrationValue0 registrationValue1 tournFlags startTime> 

// PYR-18694 to Template Server
#define DBM_Q_GET_INIDATA2                                          1420  // 4s - reqId,object
#define DBM_A_GET_INIDATA2                                          (DBM_DELTA + DBM_Q_GET_INIDATA2) // 2420  // 46[s|4<s4<s4<s,s,4>>> - reqId,errCode[errDescr|IniDataFromDbm] 

// Blitz Lobby to Table DBM
#define DBM_Q_BL_NEED_DATA                                          1421  // 44 - reqId, blitzId
#define DBM_A_BL_NEED_DATA                                          (DBM_DELTA + DBM_Q_BL_NEED_DATA) // 2421  //42[s|...] - reqId,errCode,[errDescr|...] 

#define DBM_Q_BL_USER_IN                                            1422  // 44sb4sM4 - reqId, blitzId, userId, isPlayMoney, buyIn, currency, currencyContext, entryCount
#define DBM_A_BL_USER_IN                                            (DBM_DELTA + DBM_Q_BL_USER_IN) // 2422  //42[s|44744<s>47] - reqId,errCode[errDescr|entryNum, imageId, specialAmount, hallOfFameLevelId, numGroups<relatedGroup>, rhObligationId, chipsAfter] 

// #19562
#define DBM_Q_MTL_HGTOURN_KNOCKOUTS                                 1423  // 44b4<o>... - reqId, tournId, isPlayMoney, numKnockouts<HgKnockoutEntry>
#define DBM_A_MTL_HGTOURN_KNOCKOUTS                                 (DBM_DELTA + DBM_Q_MTL_HGTOURN_KNOCKOUTS) // 2423  //42[s|...] - reqId,errCode,[errDescr|...] 

// #19216
#define DBM_Q_ATAS_GET_INSTALLID                                    1424  // 44 - reqId,userIntId
#define DBM_A_ATAS_GET_INSTALLID                                    (DBM_DELTA + DBM_Q_ATAS_GET_INSTALLID) // 2424  //42[s|s] - reqId,errCode,[errDescr|installId] 

#define DBM_Q_ATAS_GET_MAC_INSTALLID                                1425  // 4s - reqId,userId
#define DBM_A_ATAS_GET_MAC_INSTALLID                                (DBM_DELTA + DBM_Q_ATAS_GET_MAC_INSTALLID) // 2425  //42[s|bsssss] - reqId,errCode,[errDescr|currLoggedIn <TRALoginRecord>]

// PYR-20074
#define DBM_Q_GET_AAMS_USER_TABLE_TRANSACTS                         1426  // 44s8tt - reqId, ticketWindowSizeSeconds, userId, tableId, startTime, endTime
#define DBM_A_GET_AAMS_USER_TABLE_TRANSACTS                         (DBM_DELTA + DBM_Q_GET_AAMS_USER_TABLE_TRANSACTS) // 2426  // 42[s|4<84t>] - reqId, errCode, [errDescr|numTrans, <transId,transType,timeWhen>] 

// #19752
// Blitz Lobby to RO OLTP DBM
#define DBM_Q_BL_GET_TABLE_TYPE                                     1427  // 44 - reqId, tableTypeId
#define DBM_A_BL_GET_TABLE_TYPE                                     (DBM_DELTA + DBM_Q_BL_GET_TABLE_TYPE) // 2427  // 46[s|tabType] - reqId, errCode[errDescr|tabType] 

// Blitz Lobby to Table DBM
#define DBM_Q_BL_USER_OUT                                           1428  // 44s44bs4 - reqId, blitzId, userId, entryId, chips, isPlayMoney, currency, fppInCents
#define DBM_A_BL_USER_OUT                                           (DBM_DELTA + DBM_Q_BL_USER_OUT) // 2428  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_BL_ADD_CHIPS                                          1429  // 44s44bs - reqId, blitzId, userId, entryId, chips, isPlayMoney, currency
#define DBM_A_BL_ADD_CHIPS                                          (DBM_DELTA + DBM_Q_BL_ADD_CHIPS) // 2429  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_BL_UPDATE_BALANCES                                    1430  // 444<s44bs4> - reqId, blitzId, numUsers<userId, entryId, chips, isPlayMoney, currency, fppInCents>
#define DBM_A_BL_UPDATE_BALANCES                                    (DBM_DELTA + DBM_Q_BL_UPDATE_BALANCES) // 2430  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_BL_UPDATE_STATS                                       1431  // 44s4M - reqId, blitzId, userId, entryId, statsBody
#define DBM_A_BL_UPDATE_STATS                                       (DBM_DELTA + DBM_Q_BL_UPDATE_STATS) // 2431  // 46[s] - reqId, errCode[errDescr] 

// Lobby to Table DBM
#define DBM_Q_BL_CREATE_BLITZ                                       1432  // 444s - reqId, tableTypeId, maxPlayers, creator
#define DBM_A_BL_CREATE_BLITZ                                       (DBM_DELTA + DBM_Q_BL_CREATE_BLITZ) // 2432  // 46[s|4] - reqId, errCode[errDescr|blitzId] 

// Blitz Lobby to Table DBM
#define DBM_Q_BL_CREATE_BLITZ_TABLES                                1433  // 44444<s> - reqId, blitzId, tableTypeId, licenseId, numTables<tableName>
#define DBM_A_BL_CREATE_BLITZ_TABLES                                (DBM_DELTA + DBM_Q_BL_CREATE_BLITZ_TABLES) // 2433  // 46[s|4<4>] - reqId, errCode[errDescr|numTables<tableId>] 

// Lobby to Table DBM
#define DBM_Q_BL_DELETE_BLITZ                                       1434  // 44sb4 - reqId, blitzId, deletedBy, creditStuckUsers, scalePM
#define DBM_A_BL_DELETE_BLITZ                                       (DBM_DELTA + DBM_Q_BL_DELETE_BLITZ) // 2434  // 46[s] - reqId, errCode[errDescr] 

// Blitz Lobby to Table DBM
#define DBM_Q_BL_DELETE_BLITZ_TABLE                                 1435  // 448 - reqId, blitzId, tableId
#define DBM_A_BL_DELETE_BLITZ_TABLE                                 (DBM_DELTA + DBM_Q_BL_DELETE_BLITZ_TABLE) // 2435  // 46[s] - reqId, errCode[errDescr] 

// Lobby to RW OLTP DBM
#define DBM_Q_BL_SAVE_TABLE_TYPE                                    1436  // 4s... - reqId, adminId, TableScript::Table
#define DBM_A_BL_SAVE_TABLE_TYPE                                    (DBM_DELTA + DBM_Q_BL_SAVE_TABLE_TYPE) // 2436  // 46[s|4] - reqId, errCode[errDescr|tableTypeId] 

// DBD to TemplateServer (PYR-19875)
#define DBM_Q_EXPIRED_ACTIVE_TARGET_NEWS_IDS                        1437 	//4 reqId
#define DBM_A_EXPIRED_ACTIVE_TARGET_NEWS_IDS                        (DBM_DELTA + DBM_Q_EXPIRED_ACTIVE_TARGET_NEWS_IDS) // 2437  //46[s|4<4>] reqId, errCode[errDescr|numRecords<templateId>] 

#define DBM_Q_TEMPLATE_EXPIRED                                      1438 	//44<4> reqId, numRecords<templateId>
#define DBM_A_TEMPLATE_EXPIRED                                      (DBM_DELTA + DBM_Q_TEMPLATE_EXPIRED) // 2438  //46[s] - reqId, errCode[errDescr] 

// Lobby to Table DBM
#define DBM_Q_NEED_BLITZ_DATA                                       1439  // 4 - reqId
#define DBM_A_NEED_BLITZ_DATA                                       (DBM_DELTA + DBM_Q_NEED_BLITZ_DATA) // 2439  // 46[s|4<444>] - reqId, errCode[errDescr|numSessions<blitzId, tableTypeId, maxPlayers>] 

// From TableDBM to RO DBM
#define DBM_Q_ALLOWED_AT_BLITZ                                      1440  // ... - see BlitzUserInR in blitz.h
#define DBM_A_ALLOWED_AT_BLITZ                                      (DBM_DELTA + DBM_Q_ALLOWED_AT_BLITZ) // 2440  // 46[s|4] - reqId, errCode[errDescr|userIntId] 

// #20301
#define DBM_Q_GET_LOGINDATA_FLAGS                                   1441  // 4s - reqId,userId
#define DBM_A_GET_LOGINDATA_FLAGS                                   (DBM_DELTA + DBM_Q_GET_LOGINDATA_FLAGS) // 2441  // 46[s|8] - reqId, errCode[errDescr|flags] 

// #19157
#define DBM_Q_GET_ALL_HG_PLAYER_DETAILS                             1442  // 4 - reqId
#define DBM_A_GET_ALL_HG_PLAYER_DETAILS                             (DBM_DELTA + DBM_Q_GET_ALL_HG_PLAYER_DETAILS) // 2442  // 46[s|4<o>] - reqId, numUsers, <HgUserDetails> 

#define DBM_Q_GET_HG_PLAYER_DETAILS                                 1443  // 44<4> - reqId, numUsers, <userIntId>
#define DBM_A_GET_HG_PLAYER_DETAILS                                 (DBM_DELTA + DBM_Q_GET_HG_PLAYER_DETAILS) // 2443  // 46[s|4<o>] - reqId, numUsers, <HgUserDetails> 

#define DBM_Q_GET_CANCELLED_TRANSACTION                             1444  // 48b reqId TRANSACTS.transId includePgad // PYR-23644
#define DBM_A_GET_CANCELLED_TRANSACTION                             (DBM_DELTA + DBM_Q_GET_CANCELLED_TRANSACTION) // 2444 // 46[s|Body Body b[Body]] reqId errCode [errDescr | RmTransactEssentials CashTransDataSimple hasPgadTransact [PgadTransactEssential]]

// #26453 - Collusions to OLAP
#define DBM_Q_INFO_PREV_DEPOSIT										1445  // 4s4 - reqId, userId, transId
#define DBM_A_INFO_PREV_DEPOSIT										(DBM_DELTA + DBM_Q_INFO_PREV_DEPOSIT) // 2445 // 46[s|4<s>4<s>] - reqId, errCode[errDescr|numIpAddresses<ipAddresses> numInstallIds<installIds>]

// #20399
#define DBM_Q_GET_USERS_FOR_ARCHIVING                               1446  // daemon->olap:     4D - reqId lastLoginDate
#define DBM_A_GET_USERS_FOR_ARCHIVING                               (DBM_DELTA + DBM_Q_GET_USERS_FOR_ARCHIVING) // 2446  // 46[s|4<s448484>] - reqId errCode[errDescr|count<userId userIntId priv priv2 flags flags2 license>] 

// obsolete by PYR-89086
//#define DBM_Q_ARCHIVE_USER                                          1447  // daemon->logindbm: 4s - reqId userId
//#define DBM_A_ARCHIVE_USER                                          (DBM_DELTA + DBM_Q_ARCHIVE_USER) // 2447  // 46[s] - reqId errCode[errDescr] 

#define DBM_Q_UNARCHIVE_USER                                        1448  // admin ->logindbm: 4sb - reqId userId unarchiveIsTemporary
#define DBM_A_UNARCHIVE_USER                                        (DBM_DELTA + DBM_Q_UNARCHIVE_USER) // 2448  // 46[s] - reqId errCode[errDescr] 

// #20134
#define DBM_Q_SAVE_VIP_LEVEL                                        1449  // 44D44s444444 - reqId, levelId (0 - insert, non-0 - update), validFrom, licenseId, statusId, statusName, type, minVppMonthly, minVppYearly, fppRate, lastMonthYearly, missableMonthsYearly
#define DBM_A_SAVE_VIP_LEVEL                                        (DBM_DELTA + DBM_Q_SAVE_VIP_LEVEL) // 2449  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DELETE_VIP_LEVEL                                      1450  // 44 - reqId, levelId
#define DBM_A_DELETE_VIP_LEVEL                                      (DBM_DELTA + DBM_Q_DELETE_VIP_LEVEL) // 2450  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_REREAD_VIP_LEVELS                                     1451  // 4 - reqId
#define DBM_A_REREAD_VIP_LEVELS                                     (DBM_DELTA + DBM_Q_REREAD_VIP_LEVELS) // 2451  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_USER_WITH_ALL_PROPS                               1452  // regulator or logindbm ->dbmRO : 4s4  - reqId userId userIntId
#define DBM_A_GET_USER_WITH_ALL_PROPS                               (DBM_DELTA + DBM_Q_GET_USER_WITH_ALL_PROPS) // 2452  // 46[s|B] - reqId errCode[errDescr | UserWithProperties] 

#define DBM_Q_ARCH_INSERT_USER_WITH_PROPS                           1453  // logindbm->oltp  : 4sBB1 - reqId adminId userBody propsBody unarchiveIsTemporary
#define DBM_A_ARCH_INSERT_USER_WITH_PROPS                           (DBM_DELTA + DBM_Q_ARCH_INSERT_USER_WITH_PROPS) // 2453  // 46[s] 

#define DBM_Q_ARCH_DELETE_USER_WITH_PROPS                           1454  // logindbm->oltp  : 4sb - reqId userId hasUserPorperties
#define DBM_A_ARCH_DELETE_USER_WITH_PROPS                           (DBM_DELTA + DBM_Q_ARCH_DELETE_USER_WITH_PROPS) // 2454  // 46[s] 

// obsolete by PYR-89086
//#define DBM_Q_ARCH_CHECK_NEW_USER                                   1455  // lobby->logindbm = DBM_QS_USER_ADD without last SimilarUsersBody; it is parsed by the same parser as DBM_QS_USER_ADD
//#define DBM_A_ARCH_CHECK_NEW_USER                                   (DBM_DELTA + DBM_Q_ARCH_CHECK_NEW_USER) // 2455  // 46[s|B] - reqId errCode[errDescr | SimilarUsersBody(tunnels then to OLTP)] 

// PYR-18101
#define DBM_Q_SAVE_USER_INSTRUCTIONS                                1456  // 44<..>s  - reqId, userIntId, UserSpecialInstructions, audit comment
#define DBM_A_SAVE_USER_INSTRUCTIONS                                (DBM_DELTA + DBM_Q_SAVE_USER_INSTRUCTIONS) // 2456  // 46[s|b]   - reqId, errCode[errDescr|found] 

#define DBM_Q_READ_USER_INSTRUCTIONS                                1457  // 444 - reqId, userIntId, depatmentId
#define DBM_A_READ_USER_INSTRUCTIONS                                (DBM_DELTA + DBM_Q_READ_USER_INSTRUCTIONS) // 2457  // 46[s|b..] - reqId, errCode[errDescr|found [UserInstructions]] 

#define DBM_Q_READ_ALL_USER_INSTRUCTIONS                            1458  // 444 - reqId, userIntId
#define DBM_A_READ_ALL_USER_INSTRUCTIONS                            (DBM_DELTA + DBM_Q_READ_ALL_USER_INSTRUCTIONS) // 2458  // 46[s|4<..>] - reqId, errCode[errDescr|numOfInstructions, <UserInstructions>] 

#define DBM_Q_MIGRATION_ES_CHECK                                    1459 
#define DBM_A_MIGRATION_ES_CHECK                                    (DBM_DELTA + DBM_Q_MIGRATION_ES_CHECK) // 2459  

#define DBM_Q_MIGRATION_ES_PERFORM                                  1460 
#define DBM_A_MIGRATION_ES_PERFORM                                  (DBM_DELTA + DBM_Q_MIGRATION_ES_PERFORM) // 2460  

// #20731
#define DBM_Q_GET_SEQ_FIRST_TRANS                                   1461  // 4s4 - reqId,userId,maxHour
#define DBM_A_GET_SEQ_FIRST_TRANS                                   (DBM_DELTA + DBM_Q_GET_SEQ_FIRST_TRANS) // 2461  // 46[s|4<..>] - reqId, errCode[errDescr|numOfTrans<paysys,amt,stat,hours>] 

// PYR-20730 & PYR-20734
// lobby to RW OLTP
#define DBM_Q_ES_GET_ACC_VALIDATION                                 1462  //4s - reqId,userId
#define DBM_A_ES_GET_ACC_VALIDATION                                 (DBM_DELTA + DBM_Q_ES_GET_ACC_VALIDATION) // 2462  //46[s|<rec>,b] - reqId,errCode,[errDesc|<AccVerification record>,hasSpanishId] 

// lobby & automation to RW OLTP
#define DBM_Q_ES_ACC_VALIDATION_DEACTIVATE                          1463  //4s4s - reqId,userId,actionMask,comment
#define DBM_A_ES_ACC_VALIDATION_DEACTIVATE                          (DBM_DELTA + DBM_Q_ES_ACC_VALIDATION_DEACTIVATE) // 2463  //46[s] - reqId,errCode,[errDesc] 

// Admin (CDMS) to DBM
#define DBM_Q_ES_USER_DOCUMENTS_VALIDATED                           1464  // 4s4s - reqId,userId,action(eDocsValidatedAction),comment
#define DBM_A_ES_USER_DOCUMENTS_VALIDATED                           (DBM_DELTA + DBM_Q_ES_USER_DOCUMENTS_VALIDATED) // 2464  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_ES_ACC_VALIDATION_START                               1465  //4sb8 reqId,userId,notifyClent,appLoginId
#define DBM_A_ES_ACC_VALIDATION_START                               (DBM_DELTA + DBM_Q_ES_ACC_VALIDATION_START) // 2465  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_SELF_DEPOSIT_LIMITS                               1466  // UINT32 reqId,PString userid
#define DBM_A_GET_SELF_DEPOSIT_LIMITS                               (DBM_DELTA + DBM_Q_GET_SELF_DEPOSIT_LIMITS) // 2466  // 

#define DBM_Q_SET_SELF_DEPOSIT_LIMIT_EX                             1467  // UINT32 reqId,PString userid,INT32 newDailyLimit,INT32 newWeeklyLimit,INT32 newMonthlyLimit, UINT64 appLoginId
#define DBM_A_SET_SELF_DEPOSIT_LIMIT_EX                             (DBM_DELTA + DBM_Q_SET_SELF_DEPOSIT_LIMIT_EX) // 2467  // 

// cashier to OLAP - do not send it to ROOT - there is no index on USERS.FISCALCODE
#define DBM_Q_GET_USERS_BY_FISCALCODE                               1468  // should be sent only to OLAP !! 4<s> size<fiscalCode>
#define DBM_A_GET_USERS_BY_FISCALCODE                               (DBM_DELTA + DBM_Q_GET_USERS_BY_FISCALCODE) // 2468  // 4[6|4< s4<UserEssentials> >] 

// PYR-16740
// lobby to RW OLTP
#define DBM_Q_DK_GET_ACC_VALIDATION                                 1469  //4s - reqId,userId
#define DBM_A_DK_GET_ACC_VALIDATION                                 (DBM_DELTA + DBM_Q_DK_GET_ACC_VALIDATION) // 2469  //46[s|<rec>,b] - reqId,errCode,[errDesc|<AccVerification record>] 

// lobby & automation to RW OLTP
#define DBM_Q_DK_ACC_VALIDATION_DEACTIVATE                          1470  //4s4s - reqId,userId,actionMask,comment
#define DBM_A_DK_ACC_VALIDATION_DEACTIVATE                          (DBM_DELTA + DBM_Q_DK_ACC_VALIDATION_DEACTIVATE) // 2470  //46[s] - reqId,errCode,[errDesc] 

// Admin (CDMS) to DBM
#define DBM_Q_DK_USER_DOCUMENTS_VALIDATED                           1471  // 4s4s - reqId,userId,action(eDocsValidatedAction),comment
#define DBM_A_DK_USER_DOCUMENTS_VALIDATED                           (DBM_DELTA + DBM_Q_DK_USER_DOCUMENTS_VALIDATED) // 2471  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_DK_ACC_VALIDATION_START                               1472  //4sb8 reqId,userId,notifyClent,appLoginId
#define DBM_A_DK_ACC_VALIDATION_START                               (DBM_DELTA + DBM_Q_DK_ACC_VALIDATION_START) // 2472  //46[s] - reqId, errCode[errDescr] 

// #19439
#define DBM_Q_GET_IP_SPEED_DATA                                     1473  // 4s - reqId,userId
#define DBM_A_GET_IP_SPEED_DATA                                     (DBM_DELTA + DBM_Q_GET_IP_SPEED_DATA) // 2473  // 46[s|sst] - reqId,errCode[errDescr|ipAddr,installId,logouttime] 

// #20924, #16996
#define DBM_Q_GET_TOURN_CLOSED_DETAILS                              1474  // 444 - reqId, tournId, licenseId
#define DBM_A_GET_TOURN_CLOSED_DETAILS                              (DBM_DELTA + DBM_Q_GET_TOURN_CLOSED_DETAILS) // 2474  // 46[s|m] - reqId,errCode[errDescr|(licenseSpecificBody)] 

// #16706
// Auth Server to Login DBM
// renamed from LDBM_Q to DBM_Q
#define DBM_Q_DK_AUTH_RESULT          			                    1475 // 4s6[s] - reqId, userId, errCode[errDescr]
#define DBM_A_DK_AUTH_RESULT                                        (DBM_DELTA + DBM_Q_DK_AUTH_RESULT) // 2475

// PYR-21231: retrieve a list of reports the current CSR has access to
#define DBM_Q_ADMIN_GET_ALLOWED_REPORTNAMES                         1476  // 4 - reqId
#define DBM_A_ADMIN_GET_ALLOWED_REPORTNAMES                         (DBM_DELTA + DBM_Q_ADMIN_GET_ALLOWED_REPORTNAMES) // 2476  // 46[s|4<s>] - reqId,errCode[errDescr|numReports<reportName>] 

// PYR-20924 - retrieve userintids by userids
#define DBM_Q_GET_USERS_BY_IDS                                      1477  // 44<s> - reqId, numUsers<userId>
#define DBM_A_GET_USERS_BY_IDS                                      (DBM_DELTA + DBM_Q_GET_USERS_BY_IDS) // 2477  // 46[s|4<s4>] - reqId,errCode[errDescr|numUsers<userId,userIntId>] 

// #20318 cashier->dbm
#define DBM_Q_GET_SCHEDULED_PENDING_DEPOSIT_LIMITS                  1478  // 4 reqId
#define DBM_A_GET_SCHEDULED_PENDING_DEPOSIT_LIMITS                  (DBM_DELTA + DBM_Q_GET_SCHEDULED_PENDING_DEPOSIT_LIMITS) // 2478  // 46[s|4<4>] reqId,errCode[descr|map<userIntId,userId>]

#define DBM_Q_PROCESS_USER_PENDING_DEPOSIT_LIMIT                    1479  // 44 reqId, userIntId
#define DBM_A_PROCESS_USER_PENDING_DEPOSIT_LIMIT                    (DBM_DELTA + DBM_Q_PROCESS_USER_PENDING_DEPOSIT_LIMIT) // 2479  // 46[s] 

#define DBM_Q_MIGRATION_DK_CHECK                                    1480 
#define DBM_A_MIGRATION_DK_CHECK                                    (DBM_DELTA + DBM_Q_MIGRATION_DK_CHECK) // 2480  

#define DBM_Q_MIGRATION_DK_PERFORM                                  1481 
#define DBM_A_MIGRATION_DK_PERFORM                                  (DBM_DELTA + DBM_Q_MIGRATION_DK_PERFORM) // 2481  

// spain replicator->OLAP
// PYR-20924 - calculate Spanish users report
#define DBM_Q_GET_SPAIN_USERS_REPORT_FOR_RU                         1482  // 4ftt - reqId,isDaily,startTime,endTime
#define DBM_A_GET_SPAIN_USERS_REPORT_FOR_RU                         (DBM_DELTA + DBM_Q_GET_SPAIN_USERS_REPORT_FOR_RU) // 2482  // 46[s|44<O>4] - reqId,errCode[errDescr|numActive,numReported<SpainUserData>,numTotal] 

// PYR-71878
#define DBM_Q_GET_ALL_XC_JPSETTINGS                                 1483  // 4 - reqId
#define DBM_A_GET_ALL_XC_JPSETTINGS                                 (DBM_DELTA + DBM_Q_GET_ALL_XC_JPSETTINGS) // 2483  // 46[s|4<M>] - reqId, errCode[errDesc|<num<MsgBody>]

// PYR-114882
#define DBM_Q_DELETE_ONE_STALE_CASHTRANS                                1484     // 44(41bs) - reqId, croptype, {StaleCashtransData}
#define DBM_A_DELETE_ONE_STALE_CASHTRANS                                (DBM_DELTA + DBM_Q_DELETE_ONE_STALE_CASHTRANS) // 2484     // 46[s] - reqId, errCode[errDescr] 

// #DBM_Q_AVAILABLE_MESSAGE
// spain replicator->STATS
// #define DBM_Q_SPAIN_GET_MONTHLY_CJ_DATA_OLD                         1485  // 4d - reqId, end_date
// #define DBM_A_SPAIN_GET_MONTHLY_CJ_DATA_OLD                         (DBM_DELTA + DBM_Q_SPAIN_GET_MONTHLY_CJ_DATA_OLD) // 2485  // 46[s|O] - reqId,errCode[errDescr|SpainCjReport]

#define DBM_Q_MTL_SET_TOURN_PROPERTY                                1486 	//444s - reqId,tournId,propId,propValue
#define DBM_A_MTL_SET_TOURN_PROPERTY                                (DBM_DELTA + DBM_Q_MTL_SET_TOURN_PROPERTY) // 2486 	//46[s] - reqId,errCode,[errDescr] 

// PYR-16996
#define DBM_Q_DK_GET_TOURN_INFO                                     1487  // 44 - reqId, tournId
#define DBM_A_DK_GET_TOURN_INFO                                     (DBM_DELTA + DBM_Q_DK_GET_TOURN_INFO) // 2487  // 46[s|DKTournInfo] 

#define DBM_Q_ADMIN_UPDATE_BANBYREGULATOR                           1488  // 4sb - reqId, userId, {set-true|remove-false}
#define DBM_A_ADMIN_UPDATE_BANBYREGULATOR                           (DBM_DELTA + DBM_Q_ADMIN_UPDATE_BANBYREGULATOR) // 2488  // 46[s] - reqId,errCode,[errDescr] 

// PYR-109066
#define DBM_Q_GET_ALL_BLACKLIST_MONITORS                            1489  // 4 - reqId
#define DBM_A_GET_ALL_BLACKLIST_MONITORS                            (DBM_DELTA + DBM_Q_GET_ALL_BLACKLIST_MONITORS) // 2489 // 46[s|4<O>] - reqId,errCode,[errDescr|size<BlacklistMonitorData>]

// PYR-20849
#define DBM_Q_ADMIN_CLEAR_SECURITY_ALERT_FLAG                       1490  //4s8s4 reqId,userId,flagsMask,comments,userIntId
#define DBM_A_ADMIN_CLEAR_SECURITY_ALERT_FLAG                       (DBM_DELTA + DBM_Q_ADMIN_CLEAR_SECURITY_ALERT_FLAG) // 2490  // 46[s] - reqId,errCode,[errDescr] 

#define DBM_Q_REGULATOR_NEED_DATA                                   1491  // 4 reqId
#define DBM_A_REGULATOR_NEED_DATA                                   (DBM_DELTA + DBM_Q_REGULATOR_NEED_DATA) // 2491  // 

// PYR-21342
#define DBM_Q_SAVE_AAMS_TRANS_TICKET                                1492  //4O8s - reqId, MessageInfoStruct, transId, aamsTicketId
#define DBM_A_SAVE_AAMS_TRANS_TICKET                                (DBM_DELTA + DBM_Q_SAVE_AAMS_TRANS_TICKET) // 2492  //46[s]  - reqId,errCode,[errDescr] 

#define DBM_Q_MIGRATION_EU_CHECK                                    1493 
#define DBM_A_MIGRATION_EU_CHECK                                    (DBM_DELTA + DBM_Q_MIGRATION_EU_CHECK) // 2493  

#define DBM_Q_MIGRATION_EU_PERFORM                                  1494 
#define DBM_A_MIGRATION_EU_PERFORM                                  (DBM_DELTA + DBM_Q_MIGRATION_EU_PERFORM) // 2494  

// PYR-21399
#define DBM_Q_USERID_BY_USERINTID                                   1495  // 44 - reqId, userIntId
#define DBM_A_USERID_BY_USERINTID                                   (DBM_DELTA + DBM_Q_USERID_BY_USERINTID) // 2495  // 46[s|s] - reqId,errCode,[errDescr | userId] 

// PYR-70381
#define DBM_Q_GET_DK_USERS_FOR_MARKETING_COMM_BLACKLIST_CHECK       1496 // 4 - reqId
#define DBM_A_GET_DK_USERS_FOR_MARKETING_COMM_BLACKLIST_CHECK       (DBM_DELTA + DBM_Q_GET_DK_USERS_FOR_MARKETING_COMM_BLACKLIST_CHECK) // 2496 // 46[s|4<Obj>] - reqId,errCode,[errDescr|numOfUsers<DkUserInfo>]

// DBM_Q_AVAILABLE_MESSAGE 1497
/*
// PYR-16913: DBD to DBM OLTP -  set a flag in flags2, but do not automatically load the user into cache
#define DBM_Q_SET_USER_FLAGS2_NO_CACHE_AUTO_LOAD                    1497     //4s488b4 reqId userId userIntId flags2 flags2_Mask setNotRemove eUpdateType 
#define DBM_A_SET_USER_FLAGS2_NO_CACHE_AUTO_LOAD                    (DBM_DELTA + DBM_Q_SET_USER_FLAGS2_NO_CACHE_AUTO_LOAD) // 2497  // 46[s] - reqId,errCode,[errDescr] 
*/
#define DBM_Q_BL_RECONCILE                                          1498 	//44s4 - reqId, blitzId, userId, chips, isPlayMoney, currency, vppInCents
#define DBM_A_BL_RECONCILE                                          (DBM_DELTA + DBM_Q_BL_RECONCILE) // 2498  // 46[s] - reqId,errCode,[errDescr] 

#define DBM_Q_RECONCILE_BLITZ                                       1499 	//44s4 - reqId, blitzId, userId, chips, isPlayMoney, currency, vppInCents
#define DBM_A_RECONCILE_BLITZ                                       (DBM_DELTA + DBM_Q_RECONCILE_BLITZ) // 2499  // 46[s|8] - reqId,errCode,[errDescr|auditId] 

//PYR-18691
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_ADMIN_SET_USER_LIMITS                                 1500 	//4s44s - reqId, userId,limitType,limitVal,currency
#define DBM_A_ADMIN_SET_USER_LIMITS                                 (DBM_DELTA + DBM_Q_ADMIN_SET_USER_LIMITS) // 2500  // 46[s] - reqId,errCode,[errDescr] 

#define DBM_Q_MTL_REGISTER_USER                                     1502  //44s44441s4<s>41s444bsM4sbs4<44>ss4<s>4<4>4<s>bbb444M(44424444)48fff7 - 
																		  // reqId, tournId, userId, buyInPlay, buyInFpp, buyInRM, buyInTChips, useTicket, adminId,
																		  // numAdmissions<admission>, tournFlags, isPlayMoney, currency, vipStatus, initChips, registrationSites, 
																		  // isDkPlayerRegistered, sex, M(currencyContext), creditAmount, creditCurrency, isCreditTMoney, creditComment, 
																		  // num<country, state>, countryByIp, stateByIp, n<trackingCode>,n<affiliate>,n<promoCode>, isHeadsup, isTurboHyper, isBlitz, scalePM, finalFlightId, clientDevice
																		  // M(entryNum, prevMoney, prevTChips, prevAddons, prevFpp, prevTlb, prevChips, prevFppReward)
																		  // brandId,appLoginId,includeDoNotPay,reentryTourn,bountyTourn,minPlayers
#define DBM_A_MTL_REGISTER_USER                                     (DBM_DELTA + DBM_Q_MTL_REGISTER_USER) // 2502  //46[s|4s44sss44s44t484884] req_id, err_code [errDescr|userIntId, userId, totalInitChips, buyIn, city, country, email, imageId, effectivePrivileges, fullName, cameFromTourn, locale, registerTime, emLocale, effectivePrivileges2, vipStatus, userFlags2, transIdRegister, hallOfFameLevelId] 

#define DBM_Q_MTL_UNREGISTER_USER                                   1503  //44ss11s1444 req_id, tourn_id, userId, adminId, unregType(eMtLobbyUnregType), debitPlayerIfDummy, extraComment, isBlitz, scalePM, finalFlightTournId, clientDevice
#define DBM_A_MTL_UNREGISTER_USER                                   (DBM_DELTA + DBM_Q_MTL_UNREGISTER_USER) // 2503  //46s[48444ss] req_id, err_code, userName|errDescr [refund, refundPlay, refundFpp, refundTChips, refundWChips, tournamentCurrency, refundCurrency] 
//#22691
#define DBM_Q_ADD_TGM_TOURN                                         1504 //444 reqId, tgmId, tournId
#define DBM_A_ADD_TGM_TOURN                                         (DBM_DELTA + DBM_Q_ADD_TGM_TOURN) //46[s]
//#24146
#define DBM_Q_SAVE_MILESTONE_HAND									1505 //4488 - reqId, siteId, handId, handNumber
#define DBM_A_SAVE_MILESTONE_HAND									(DBM_DELTA + DBM_Q_SAVE_MILESTONE_HAND) // 2505 //46[s] reqId, errCode[errDescr]

#define DBM_Q_MTL_KILLTABLES                                        1506  //44	req_id, tournId
#define DBM_A_MTL_KILLTABLES                                        (DBM_DELTA + DBM_Q_MTL_KILLTABLES) // 2506  //never sent 

#define DBM_Q_MTL_ALLOCATE_USERS                                    1507  //44f4<8s14477> req_id, tourn_id, isBlitzSwitching, numUsers<tableId, userName, position, chips, userIntId, unusedStacks, ordinal>
#define DBM_A_MTL_ALLOCATE_USERS                                    (DBM_DELTA + DBM_Q_MTL_ALLOCATE_USERS) // 2507  //46[s] req_id, err_code [errDescr] 

#define DBM_Q_MTL_RESEAT_USERS                                      1508  //481<s81> req_id, oldTableId, numUsers< userId, newTableId, newPosition >
#define DBM_A_MTL_RESEAT_USERS                                      (DBM_DELTA + DBM_Q_MTL_RESEAT_USERS) // 2508  //46[s] req_id, err_code [errDescr] 

#define DBM_Q_MTL_USERS_OUT                                         1509  // See compose/parse code
#define DBM_A_MTL_USERS_OUT                                         (DBM_DELTA + DBM_Q_MTL_USERS_OUT) // 2509  //46[s]4<s1444(T)> req_id, err_code [errDescr], numWinners <userId registrationType registrationValue0 registrationValue1 tournFlags startTime> 

#define DBM_Q_MTL_ALIVE                                             1510  //44724 req_id, tournId, seconds, curOlympicLevel, secondsInLevel
#define DBM_A_MTL_ALIVE                                             (DBM_DELTA + DBM_Q_MTL_ALIVE) // 2510  //46[s] req_id, err_code [errDescr] 

#define DBM_Q_GET_HISTORICAL_CONVRATES_IN_INTERVAL                  1511  // 4tt4<s> reqId startTime endTime vec<currencies>
#define DBM_A_GET_HISTORICAL_CONVRATES_IN_INTERVAL                  (DBM_DELTA + DBM_Q_GET_HISTORICAL_CONVRATES_IN_INTERVAL) // 2511 46[s|STRUCT] - reqId, errCode[errDescr|struct ConvRater]

#define DBM_Q_MTL_MSG_TEXT                                          1512  // 44S q_id, tourn_id, i18n_messageBoardText
#define DBM_A_MTL_MSG_TEXT                                          (DBM_DELTA + DBM_Q_MTL_MSG_TEXT) // 2512  //46[s] q_id, err [, errDescr] 

#define DBM_Q_MTL_PRIZE_ADDON                                       1513  //444 reqId, tournId, prizeAddon
#define DBM_A_MTL_PRIZE_ADDON                                       (DBM_DELTA + DBM_Q_MTL_PRIZE_ADDON) // 2513  //46[s] q_id, err [, errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_USER_LOGOUT                                           1514  //418s484 reqId, objType, objId, userName, logoutType, appSessionId, clientType
#define DBM_A_USER_LOGOUT                                           (DBM_DELTA + DBM_Q_USER_LOGOUT) // 2514  //46[s] q_id, err [, errDescr] 

#define DBM_Q_SAVE_BLACKLIST_MONITOR								1515 // 4O - reqId,BlacklistMonitorData
#define DBM_A_SAVE_BLACKLIST_MONITOR								(DBM_DELTA + DBM_Q_SAVE_BLACKLIST_MONITOR) // 46[s] - reqId,errCode[errDescr]

#define DBM_Q_CASHOUT_COLLECT_DATA							        1516  //
#define DBM_A_CASHOUT_COLLECT_DATA									(DBM_DELTA + DBM_Q_CASHOUT_COLLECT_DATA) // 2516

#define DBM_Q_INCOMING_EMAILS                                       1517  //4 reqId
#define DBM_A_INCOMING_EMAILS                                       (DBM_DELTA + DBM_Q_INCOMING_EMAILS) // 2517  //46[s|4<441ssssss>] reqId, err, [errDescr|numEmails<emailId, received, status, from, mailbox, subj, user, assigned, inReplyTo>] 

#define DBM_Q_ONE_EMAIL                                             1518  //441 reqId, emailId, needGuessUser
#define DBM_A_ONE_EMAIL                                             (DBM_DELTA + DBM_Q_ONE_EMAIL) // 2518  //46[s|b41ssssss4<s1>] reqId, err, [errDescr|emailBlock, emailId, received, status, from, mailbox, subj, user, assigned, inReplyTo, numSuggestedUsers<userId, basedOn>] 

#define DBM_Q_UPDATE_EMAIL                                          1519  //441[1|s] reqId, emailId, reqType[status|(user|assigned)]
#define DBM_A_UPDATE_EMAIL                                          (DBM_DELTA + DBM_Q_UPDATE_EMAIL) // 2519  //46[s]b41ssssss4<s1>] reqId, err, [errDescr|emailBlock, emailId, received, status, from, mailbox, subj, user, assigned, inReplyTo, numSuggestedUsers<userId, basedOn>] 

#define DBM_Q_USERS_BY_EMAIL_ADDR                                   1520  //4s reqId, email
#define DBM_A_USERS_BY_EMAIL_ADDR                                   (DBM_DELTA + DBM_Q_USERS_BY_EMAIL_ADDR) // 2520  //46[s|4<s>] reqId, err_code, [errDescr|numUsers<userId>] 

#define DBM_Q_DOUBLE_USER                                           1521  //4ss reqId, oldUserId, newUserId
#define DBM_A_DOUBLE_USER                                           (DBM_DELTA + DBM_Q_DOUBLE_USER) // 2521  //46[s] reqId, err_code, [errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_MTL_UNREG_USERS                                       1522  //44s4B4<M>Bff4<s4> reqId, tournId, adminId, adminIntId, isBlitz, finalFlightTournId, tgmUserBody, needRefundInfo, isBounty, isReentry, numUsers<userId, userIntId, licenseId>
#define DBM_A_MTL_UNREG_USERS                                       (DBM_DELTA + DBM_Q_MTL_UNREG_USERS) // 2522  //444<UserRefundInfo> reqId, numUsersUnRegistered, refundCount<UserRefundInfo> 

#define DBM_Q_MTL_REBUY                                             1523  //448s44444fs44f4M([4]) reqId, tournId, tableId, userId, rebuyAmount, rebuyAmountPM, fppAmount, chipsToAdd, userIntId, isPM, currency, numRebuysAddons, scalePM, isBlitz, finalFlightId, M([overrideReqId])
#define DBM_A_MTL_REBUY                                             (DBM_DELTA + DBM_Q_MTL_REBUY) // 2523  //46[s|4444877] reqId, err_code, [errDescr | chips, playChips, fpp, tChips, transId, moneyAfter, tChipsAfter]

#define DBM_Q_MTL_ADDON                                             1524  //448s44444fs44f4M([4]) reqId, tournId, tableId, userId, rebuyAmount, rebuyAmountPM, fppAmount, chipsToAdd, userIntId, isPM, currency, numRebuysAddons, scalePM, isBlitz, finalFlightId, M([overrideReqId])
#define DBM_A_MTL_ADDON                                             (DBM_DELTA + DBM_Q_MTL_ADDON) // 2524  //46[s|4444877] reqId, err_code, [errDescr | chips, playChips, fpp, tChips, transId, moneyAfter, tChipsAfter]

#define DBM_Q_MTL_USERS_AWARDS                                      1525  // see DBM_Q_MTL_USERS_OUT
#define DBM_A_MTL_USERS_AWARDS                                      (DBM_DELTA + DBM_Q_MTL_USERS_AWARDS) // 2525  // see DBM_A_MTL_USERS_OUT 

#define DBM_Q_MTL_CREATE_INITIAL_TABLES                             1526  //444<s>4s reqId, tournId, numTables<tableName>, clubId, effCreator
#define DBM_A_MTL_CREATE_INITIAL_TABLES                             (DBM_DELTA + DBM_Q_MTL_CREATE_INITIAL_TABLES) // 2526  //44<4> reqId numtables<tableId> 

#define DBM_Q_UPDATE_PAYSYSTEM_MESSAGE                              1527  //41ss reqId isForClosedSystem paySystem message
#define DBM_A_UPDATE_PAYSYSTEM_MESSAGE                              (DBM_DELTA + DBM_Q_UPDATE_PAYSYSTEM_MESSAGE) // 2527  //46[s] reqId, err_code, [errDescr] 

#define DBM_Q_UPDATE_ELV_RETURN_STATUS                              1528  //444 reqId, gcDebitHistId, newStatus
#define DBM_A_UPDATE_ELV_RETURN_STATUS                              (DBM_DELTA + DBM_Q_UPDATE_ELV_RETURN_STATUS) // 2528  //46[s] reqId, err_code, [errDescr] 

#define DBM_Q_GET_CASHTRANS                                         1529  //44 reqId, transId
#define DBM_A_GET_CASHTRANS                                         (DBM_DELTA + DBM_Q_GET_CASHTRANS) // 2529  //46[s] reqId, err_code, [errDescr] 

// PYR-41319
#define DBM_Q_GET_USER_PERSONAL_INFO								1530 //44<s>bb reqId, numUsers<userId>, needLoginBrand, needRegBrand
#define DBM_A_GET_USER_PERSONAL_INFO								(DBM_DELTA + DBM_Q_GET_USER_PERSONAL_INFO) // 2530  //46[s|4<M>] reqId err_code [errDescr|numusers<_UserPersonalInfo>] 

#define DBM_Q_UPDATE_PSTARS_TEAM                                    1531  //4s4<ss4> reqId adminId numUsers<userId, displayName, siteMask>
#define DBM_A_UPDATE_PSTARS_TEAM                                    (DBM_DELTA + DBM_Q_UPDATE_PSTARS_TEAM) // 2531  //46[s|4<ss4s>] reqId, err_code, [errDescr|numUsers<userId, displayName, siteMask, country>] 

#define DBM_Q_GCDEBIT_UPDATE_STATUSES                               1532  //44<4>11 reqId, numAccounts<accountId> oldStatus newStatus
#define DBM_A_GCDEBIT_UPDATE_STATUSES                               (DBM_DELTA + DBM_Q_GCDEBIT_UPDATE_STATUSES) // 2532  //46[s|4<46s>] reqId err_code [errDescr|numAccounts<gcDebitId errCode errDescr>] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SET_TOURNTICKETS_EXPDATE                              1533  //4T4<4> reqId, expDate, numTickets <ticketId>
#define DBM_A_SET_TOURNTICKETS_EXPDATE                              (DBM_DELTA + DBM_Q_SET_TOURNTICKETS_EXPDATE) // 2533  //46[s|4<s6s>] reqId err_code [errDescr|numtickets<ticketId err_code errDescr>] 

#define DBM_Q_AGE_VERIFIED_ARIST                                    1534 	//4sTsssssssssssDsss4s44sb8 reqId userId when fullName first middle last gender addr city state zip county country DOB phone SSN4 id idType xTransStr matchCode errCode errDescr passed appLoginId
#define DBM_A_AGE_VERIFIED_ARIST                                    (DBM_DELTA + DBM_Q_AGE_VERIFIED_ARIST) // 2534  //46[s|4] reqId, err_code, [errDescr|resultId] 

#define DBM_Q_GET_AGE_VERIFY_SETTINGS                               1535  //4 reqId
#define DBM_A_GET_AGE_VERIFY_SETTINGS                               (DBM_DELTA + DBM_Q_GET_AGE_VERIFY_SETTINGS) // 2535  //46[s|A] reqId, err_code, [errDescr|AgeVerifySettings] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SELF_EXCLUDE_USER                                     1536 	//4s4f8 reqId userId numDays,appSessionId
#define DBM_A_SELF_EXCLUDE_USER                                     (DBM_DELTA + DBM_Q_SELF_EXCLUDE_USER) // 2536  //46[s|4TM] reqId,errCode[errDescr|numDays,expiryDate,UserMsgBody] // for UserMsgBody format see DBM_A_BUILD_USER_BAN_CASE 

#define DBM_Q_USER_UPCOMING_TOURNS                                  1537 	//4s reqId userId
#define DBM_A_USER_UPCOMING_TOURNS                                  (DBM_DELTA + DBM_Q_USER_UPCOMING_TOURNS) // 2537 	//46[s|4<4>] reqId err_code[errDescr|numTourns<tournId>] 

#define DBM_Q_REREAD_PAYSYSTEMS_COUNTRIES                           1538  //4 reqId
#define DBM_A_REREAD_PAYSYSTEMS_COUNTRIES                           (DBM_DELTA + DBM_Q_REREAD_PAYSYSTEMS_COUNTRIES) // 2538  //4{(4,...)|6s} reqId {PaySystemsWithCountries.size, ... | err_code, err_descr} 

#define DBM_Q_GET_EXPIRED_EXCLUSIONS                                1539  //4 reqId
#define DBM_A_GET_EXPIRED_EXCLUSIONS                                (DBM_DELTA + DBM_Q_GET_EXPIRED_EXCLUSIONS) // 2539  //46[s|4<s>] reqId numUsers[userId] 

#define DBM_Q_UPDATE_EXPIRED                                        1540  //4s reqId userId
#define DBM_A_UPDATE_EXPIRED                                        (DBM_DELTA + DBM_Q_UPDATE_EXPIRED) // 2540  //46[s] reqId errCode[errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_EXPIRE_SELFEXCLUSION                                  1541  //4s reqID, userId
#define DBM_A_EXPIRE_SELFEXCLUSION                                  (DBM_DELTA + DBM_Q_EXPIRE_SELFEXCLUSION) // 2541  //46[s] reqId errCode[errDescr] 

// Blitz Lobby to Table DBM
#define DBM_Q_BL_USER_RM_CHIPS_RETURN                               1542  // 44s44s48 - reqId, blitzId, userId, entryNum, chips, currency, specialAmount, staked
#define DBM_A_BL_USER_RM_CHIPS_RETURN                               (DBM_DELTA + DBM_Q_BL_USER_RM_CHIPS_RETURN) // 2542  // 46[s|4] - reqId, errCode[errDescr|specialAmountDelta]

// #23636 2 pairs below: dbd<->ro/rw dbm
#define DBM_Q_GET_CRYPTED_FRBANKACCOUNTS                            1543 // 4 req
#define DBM_A_GET_CRYPTED_FRBANKACCOUNTS                            (DBM_DELTA + DBM_Q_GET_CRYPTED_FRBANKACCOUNTS) // 2543 // 4<4s> req count <userIntId acctCrypted>

#define DBM_Q_UPDATE_FRBANKACCOUNTS                                 1544 // 44<4ss> req count <userIntId oldAcct newAcct>
#define DBM_A_UPDATE_FRBANKACCOUNTS                                 (DBM_DELTA + DBM_Q_UPDATE_FRBANKACCOUNTS) // 2544 	// 46[s] - reqId errCode [errDescr] 

// #22093
// Obsolete after PYR-50253
// DBM_Q_AVAILABLE_MESSAGE
// #define DBM_Q_INSERT_ES_BOT_REPORT_DAILY							1545  //4fd88888888 reqId CnjBotReport
// #define DBM_A_INSERT_ES_BOT_REPORT_DAILY							(DBM_DELTA + DBM_Q_INSERT_ES_BOT_REPORT_DAILY) // 2545 //46[s]
// #define DBM_Q_GET_ES_BOT_REPORT_MONTHLY								1546  //4dd reqId periodStart periodEnd
// #define DBM_A_GET_ES_BOT_REPORT_MONTHLY								(DBM_DELTA + DBM_Q_GET_ES_BOT_REPORT_MONTHLY) // 2546 //46[s|fd88888888] reqId errCode [errDescr | CnjBotReport]


#define DBM_Q_UPDATE_MATCHCODE_PASSED                               1547  //44b reqId MatchCode Passed
#define DBM_A_UPDATE_MATCHCODE_PASSED                               (DBM_DELTA + DBM_Q_UPDATE_MATCHCODE_PASSED) // 2547  //46[s] reqId errCode[errDescr] 

// PYR-89627
#define DBM_Q_GET_RECENT_CASHTRANS_ESSENTIALS						1548 // 4s4 reqId,userID, secondsBack
#define DBM_A_GET_RECENT_CASHTRANS_ESSENTIALS						(DBM_DELTA + DBM_Q_GET_RECENT_CASHTRANS_ESSENTIALS) // 2548 // 42[s|4<4sssss44s4stt48s84>] reqId, errCode[errDescr|size<CashtransEssentials>]

// PYR-91944
#define DBM_Q_SAVE_AGE_VERIF_ARISTOTLE_RESULT						1549
#define DBM_A_SAVE_AGE_VERIF_ARISTOTLE_RESULT						(DBM_DELTA + DBM_Q_SAVE_AGE_VERIF_ARISTOTLE_RESULT) // 2549

#define DBM_Q_SET_USER_LOCALE                                       1550  //4s4 reqId userId localeId
#define DBM_A_SET_USER_LOCALE                                       (DBM_DELTA + DBM_Q_SET_USER_LOCALE) // 2550  //46[s|4444444m] reqId errCode[errDescr|locale, emLocale, userIntId, emLocale, platformUsed, siteVisited, brandUsed, visitedEx] 

#define DBM_Q_GET_USER_TNEWS_DATA                                   1551  //4s reqId userId
#define DBM_A_GET_USER_TNEWS_DATA                                   (DBM_DELTA + DBM_Q_GET_USER_TNEWS_DATA) // 2551  //46[s/4d4<s>s] reqId errCode[errDescr|balanceUSD regDate numberCodes<code>,sex] 

// PYR-21829 
#define DBM_Q_MOBILE_CHANGED										1552 // 4s reqId userId 
#define DBM_A_MOBILE_CHANGED										(DBM_DELTA + DBM_Q_MOBILE_CHANGED) // 2552 //46[s] reqId errCode[errDescr] 

#define DBM_Q_GET_BLITZ_HANDS										1553 //4t4<48> - reqId,timeStart,timeEnd,numSessions<sessionId,lastHandId>
#define DBM_A_GET_BLITZ_HANDS										(DBM_DELTA + DBM_Q_GET_BLITZ_HANDS) //46[s|4<44<8>>] - reqId, errCode[ errDescr | numSessions<sessionId,numHands<handId>>]

// Blitz Lobby to Table DBM
#define DBM_Q_BL_UPDATE_BLITZ										1554  // 4444 - reqId, blitzId, tableTypeId, maxPlayers
#define DBM_A_BL_UPDATE_BLITZ										(DBM_DELTA + DBM_Q_BL_UPDATE_BLITZ) // 2542  // 46[s] - reqId, errCode[errDescr]

#define DBM_Q_SAVE_WIRE_CASHOUT_INFO                                1555  //4(W)8 see WireAccountInfo::compose()
#define DBM_A_SAVE_WIRE_CASHOUT_INFO                                (DBM_DELTA + DBM_Q_SAVE_WIRE_CASHOUT_INFO) // 2555  //46[s] reqId errCode[errDescr] 

#define DBM_Q_GET_USERS_LOCALE                                      1556  //44<s> reqId numUsers <userId>
#define DBM_A_GET_USERS_LOCALE                                      (DBM_DELTA + DBM_Q_GET_USERS_LOCALE) // 2556  //46[s/4<s644>] reqId errCode[errDescr/numUsers<userid errcode userintid locale>] 

#define DBM_Q_INVALIDATE_WIREACCOUNT                                1557  //44s8s reqId accountId userId appLoginId extraComment
#define DBM_A_INVALIDATE_WIREACCOUNT                                (DBM_DELTA + DBM_Q_INVALIDATE_WIREACCOUNT) // 2557  //46[s] reqId errCode[errDescr] 

#define DBM_Q_CASHOUT_INIT2                                         1558
#define DBM_A_CASHOUT_INIT2                                         (DBM_DELTA + DBM_Q_CASHOUT_INIT2) // 2558

#define DBM_Q_GET_HELD_FLAGGED_DEPOSITS								1559 // 48 reqId cashtransFlagMask (ROOT.CashTrans in 'N' status + FLAGS)
#define DBM_A_GET_HELD_FLAGGED_DEPOSITS								(DBM_DELTA + DBM_Q_GET_HELD_FLAGGED_DEPOSITS) // 2559

// PYR-26453: Collusions to OLAP
#define DBM_Q_FIRSTUSE_PAYSYSTEM									1560  // 4ss - reqId, userId, paysystem
#define DBM_A_FIRSTUSE_PAYSYSTEM									(DBM_DELTA + DBM_Q_FIRSTUSE_PAYSYSTEM) // 2560  // 46[s|t] - reqId, errCode[errDescr|firstUseTime]

#define DBM_Q_VALIDATE_NEWINSTALL									1561  //4ss4 reqId, userId, installId, code // #21829
#define DBM_A_VALIDATE_NEWINSTALL									(DBM_DELTA + DBM_Q_VALIDATE_NEWINSTALL) // 2561

#define DBM_Q_GET_USER_DEPOSIT_LIMIT                                1562  //4s reqId userId
#define DBM_A_GET_USER_DEPOSIT_LIMIT                                (DBM_DELTA + DBM_Q_GET_USER_DEPOSIT_LIMIT) // 2562  //46[s|4T4] reqId err_code [errDescr|weeklyLimit enforcedUntil requestId]

#define DBM_Q_SET_USER_DEPOSIT_LIMIT                                1563  //4s4[s4]8 reqId userId newLimit [adminComment adminIntId] appLoginId
#define DBM_A_SET_USER_DEPOSIT_LIMIT                                (DBM_DELTA + DBM_Q_SET_USER_DEPOSIT_LIMIT) // 2563  //46[s|8T] reqId err_code [errDescr|weeklyLimit enforcedUntil] 

#define DBM_Q_COMPLETE_LIMITREQUEST                                 1564  //4s418s reqId userId limit_request_id status newLimit comments
#define DBM_A_COMPLETE_LIMITREQUEST                                 (DBM_DELTA + DBM_Q_COMPLETE_LIMITREQUEST) // 2564  //46[s|8T] reqId err_code [errDescr|weeklyLimit enforcedUntil] 

#define DBM_Q_UPDATE_CARDNUM_EX                                     1565  // 444ssss reqId, transId, ctTableId, cardNumPlain, cardNumEncr, cardNumMasked, custId, bin ("" or 00+binPlain)
#define DBM_A_UPDATE_CARDNUM_EX                                     (DBM_DELTA + DBM_Q_UPDATE_CARDNUM_EX) // 2565  //46[s|4b] reqId errCode[errDescr|cc_id isBlocked] 

// PYR-121189
#define DBM_Q_CHECK_CARD_VERIFIED_RO                                1566 //444 reqId, userIntId, cardNumId
#define DBM_A_CHECK_CARD_VERIFIED_RO                                (DBM_DELTA + DBM_Q_CHECK_CARD_VERIFIED_RO) // 2566 // 46[s|b] reqId errCode[errDescr|isVerified]

/* obsolete by PYR-45748
#define DBM_Q_CHANGE_WUTRANS_STATUS                                 1569  //4s44s reqId, wuTransId, newStatus, comment
#define DBM_A_CHANGE_WUTRANS_STATUS                                 (DBM_DELTA + DBM_Q_CHANGE_WUTRANS_STATUS) // 2569  //46[s|s] reqId errCode[errDescr] 
*/
// #define DBM_Q_AVAILABLE_MESSAGE	1569

#define DBM_Q_UPDATE_CARDNUM                                        1570  // 44ss reqId, transId, cardNumPlain, cardNumEncr
#define DBM_A_UPDATE_CARDNUM                                        (DBM_DELTA + DBM_Q_UPDATE_CARDNUM) // 2570  //46[s|4b] reqId errCode[errDescr|cc_id isBlocked] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_ADMIN_SAVE_TICKETTYPE                                 1571 	//4(ticketType) reqId, ticketType (see struct TicketType in shared.h)
#define DBM_A_ADMIN_SAVE_TICKETTYPE                                 (DBM_DELTA + DBM_Q_ADMIN_SAVE_TICKETTYPE) // 2571 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_UNCLEARED_DEPOSITS                                    1572 	//4s reqId userId
#define DBM_A_UNCLEARED_DEPOSITS                                    (DBM_DELTA + DBM_Q_UNCLEARED_DEPOSITS) // 2572 	//46[s|4<4s4D>]44 reqId, errCode [errDescr|numUncleared<transId paysystem amountUSD clearDateTime>] maxnum maxsum 

//lobby to dbm - user is registered on IntelliPoker site
#define DBM_Q_INTELLIPOKER_REGISTRATION                             1573 	//4sb reqId userId onOff
#define DBM_A_INTELLIPOKER_REGISTRATION                             (DBM_DELTA + DBM_Q_INTELLIPOKER_REGISTRATION) // 2573 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_SET_CONVERSION_RATE2                                  1574 	// 4s4 reqId, currencyCode, newRate
#define DBM_A_SET_CONVERSION_RATE2                                  (DBM_DELTA + DBM_Q_SET_CONVERSION_RATE2) // 2574 	//46[s] reqId, err_code[errDescr] 

//PS-IP gateway to dbm
#define DBM_Q_CHANGED_IP_USERS_SINCE_TIME                           1575  //4t  reqId timeSince
#define DBM_A_CHANGED_IP_USERS_SINCE_TIME                           (DBM_DELTA + DBM_Q_CHANGED_IP_USERS_SINCE_TIME) // 2575  //46[s|t4<4bb>] reqId err_code[errDescr|endTime numUsers<userIntId, imageChanged, deposited>]  

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_IP_USERS_BY_INTIDS                                    1576  //44<4> reqId numUsers <userIntId>
#define DBM_A_IP_USERS_BY_INTIDS                                    (DBM_DELTA + DBM_Q_IP_USERS_BY_INTIDS) // 2576  //46[s|4(4s4ssbs444TT4s4(448)88)] reqId err_code[errDescr|numUsers(userIntId, userId, imageId, country, city, isPlayMoney, email, vipstatus, flags, privileges, registered, firstRMTransfer, livenseId, sex, visited, (depositMask, depositMask2, depositMask3), priveleges2, flags2)] 

#define DBM_Q_CREDIT_VPP_USERS                                      1577  //44<s46>s4 reqId numusers<userId amount amountCents>comment ePlayerPointsType
#define DBM_A_CREDIT_VPP_USERS                                      (DBM_DELTA + DBM_Q_CREDIT_VPP_USERS) // 2577     //46[s|4<s6s>] reqId err_code [errDescr|numusers<userId err_code errDescr>] 

// Daemon (dbd) to dbm
#define DBM_Q_REPORTONLY_TRACKCODES_DATE                            1578 	// 4D - reqId, date
#define DBM_A_REPORTONLY_TRACKCODES_DATE                            (DBM_DELTA + DBM_Q_REPORTONLY_TRACKCODES_DATE) // 2578 	//44<4<4>> reqId, numTrackCodes, <numIds,<userIntId>> 

#define DBM_Q_PROCESS_REPORTONLY_TRACKCODES                         1579 	// 4sb4<4> - reqId, trackCode, markCropped, numUsers<userIntId>
#define DBM_A_PROCESS_REPORTONLY_TRACKCODES                         (DBM_DELTA + DBM_Q_PROCESS_REPORTONLY_TRACKCODES) // 2579 	//46[s] reqId, errCode[errDescr] 

// Lobby to dbm
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_USER_IMAGE_ID_FLAGS                                   1580 	// 4s - reqId, userId
#define DBM_A_USER_IMAGE_ID_FLAGS                                   (DBM_DELTA + DBM_Q_USER_IMAGE_ID_FLAGS) // 2580 	//46[s|4444sTT44s4M48448m] reqId, errCode[errDescr|imageId, flags, vipStatus, privileges, promoName, registered, firstDeposit, licenseId, prevVpp, sex, visited, depositMask128, userIntId, privileges2, maxTablesLimitPM, maxTablesLimitRM, flags2, visitedEx] 

#define DBM_Q_GET_USER_FCP_INFO                                     1581  //4s reqId userId
#define DBM_A_GET_USER_FCP_INFO                                     (DBM_DELTA + DBM_Q_GET_USER_FCP_INFO) // 2581     //46[s<fcpinfo>] reqId err_code[errDescr/id fullName email addr_1 addr_2 city state country zipCode flags userIntId phone registered birthDate] 

//PYR-75994: obsolete
// #define DBM_Q_ALLAUDIT_IDS_DATE                                     1582 	//4(D) reqId, date
// #define DBM_A_ALLAUDIT_IDS_DATE                                     (DBM_DELTA + DBM_Q_ALLAUDIT_IDS_DATE) // 2582     //44<4> reqId, numIds<auditId> 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_MTL_REGISTER_SAT_WINNERS                              1583 	// 444bs44bb4b448ff77s4<4> - tournId, registrationLimit, chips, isPlayMoney, currency, buyIn, fppBuyIn, isFlight, isDkRegistered, scalePM, isBlitz, finalFlightId, tournFlags, tournFlags2,hasBounties,isReentry,initialChips,minPlayers, admission, num<userIntIds>
#define DBM_A_MTL_REGISTER_SAT_WINNERS                              (DBM_DELTA + DBM_Q_MTL_REGISTER_SAT_WINNERS) // 2583     //46[s|4<4sss44s44t484ss44>f4<s4s>] reqId, errCode, [errDescr|numUsers<userIntId,userId,city,email,imageId,priv,fullName,satelliteId,locale,registerTime,emLocale,effectivePrivileges2,vipStatus,state,ipAddress,flags,originalUserIntId>,isFirstDanish,numDanishUsers<userId,userIntId,ipAddress>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_ADMIN_CHANGE_INIDATA                                  1584  //414ssss4ss reqId,reqType[I,U,D],recordId,object,section,itemname,itemvalue,flags,descr,comment
#define DBM_A_ADMIN_CHANGE_INIDATA                                  (DBM_DELTA + DBM_Q_ADMIN_CHANGE_INIDATA) // 2584 	//46[s] reqId, errCode [errDescr] 

#define DBM_Q_GET_INIDATA                                           1585  //4s reqId
#define DBM_A_GET_INIDATA                                           (DBM_DELTA + DBM_Q_GET_INIDATA) // 2585     //46[44ss...,...|s] reqId, errCode [errDescr|countSections,countItems,value,name ...(see IniDataFromDbm)] 

// Admin to dbm
#define DBM_Q_ADMIN_SAVE_VENDOR                                     1586  // 44ss - regId, vendorId, name, descr
#define DBM_A_ADMIN_SAVE_VENDOR                                     (DBM_DELTA + DBM_Q_ADMIN_SAVE_VENDOR) // 2586 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_ADMIN_SAVE_PRODUCT                                    1587  // 444s4ss - regId, productId, vendorId, descr, price, currency, allowedCountries
#define DBM_A_ADMIN_SAVE_PRODUCT                                    (DBM_DELTA + DBM_Q_ADMIN_SAVE_PRODUCT) // 2587 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_ADMIN_SAVE_CLAIMCODES                                 1588  // 44<M> - regId, numCodes<ClaimCodeMsgBody> ClaimCodeMsgBody: 4sD - productId, claimCode, ExpDate
#define DBM_A_ADMIN_SAVE_CLAIMCODES                                 (DBM_DELTA + DBM_Q_ADMIN_SAVE_CLAIMCODES) // 2588 	// 46[s|4<s6s>] reqId, errCode [errDescr|numCodes<claimCode, errCode, errDescr>] 

#define DBM_Q_ADMIN_CANCEL_CLAIMCODE                                1589  // 44 - regId, claimCodeId
#define DBM_A_ADMIN_CANCEL_CLAIMCODE                                (DBM_DELTA + DBM_Q_ADMIN_CANCEL_CLAIMCODE) // 2589 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_GET_CASHTRANS_LIST                                    1590 	//44<4> reqId listSize<transId>
#define DBM_A_GET_CASHTRANS_LIST                                    (DBM_DELTA + DBM_Q_GET_CASHTRANS_LIST) // 2590 	//44<46[s|(Tr)> reqId listSize<transId errCode[errDecr|Transaction]> - see CashTransaction::parseFromDbmFull 

#define DBM_Q_INSERT_PAYSYSTEM_GROUP                                1591 	/// 4s44 reqId name flags defaultLevel
#define DBM_A_INSERT_PAYSYSTEM_GROUP                                (DBM_DELTA + DBM_Q_INSERT_PAYSYSTEM_GROUP) // 2591 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_UPDATE_PAYSYSTEM_GROUP                                1592 	/// 44s44 reqId groupId name flags defaultLevel
#define DBM_A_UPDATE_PAYSYSTEM_GROUP                                (DBM_DELTA + DBM_Q_UPDATE_PAYSYSTEM_GROUP) // 2592 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_INSERT_DEPOSIT_LEVEL                                  1593 	/// 4s44444 reqId name order minDeposit maxDay, maxWeek maxMonth
#define DBM_A_INSERT_DEPOSIT_LEVEL                                  (DBM_DELTA + DBM_Q_INSERT_DEPOSIT_LEVEL) // 2593 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_UPDATE_DEPOSIT_LEVEL                                  1594 	/// 44s44444 reqId levelId name order minDeposit maxDay, maxWeek maxMonth
#define DBM_A_UPDATE_DEPOSIT_LEVEL                                  (DBM_DELTA + DBM_Q_UPDATE_DEPOSIT_LEVEL) // 2594 	// 46[s] reqId, errCode [errDescr] 

#define DBM_Q_INSERT_USERGROUP_LEVEL                                1595 	/// 44<s44> reqId size<userId groupId levelId> (existing combination of userId + groupId will be ignored, error code sent back!)
#define DBM_A_INSERT_USERGROUP_LEVEL                                (DBM_DELTA + DBM_Q_INSERT_USERGROUP_LEVEL) // 2595 	// 46[s|4<s6[s]>] reqId, errCode[errDescr|size<userId errCode [errDescr]>] 

#define DBM_Q_UPDATE_USERGROUP_LEVEL                                1596 	/// 44<s44> reqId size<userId groupId levelId> (only level value can be changed!)
#define DBM_A_UPDATE_USERGROUP_LEVEL                                (DBM_DELTA + DBM_Q_UPDATE_USERGROUP_LEVEL) // 2596 	// 46[s|4<s6[s]>] reqId, errCode[errDescr|size<userId errCode [errDescr]>] 

// PYR-38811
#define DBM_Q_GET_USERS_REGISTRATION_BRAND_BY_USERID				1597	/// 44<s> reqId, size<userId>
#define DBM_A_GET_USERS_REGISTRATION_BRAND_BY_USERID				(DBM_DELTA + DBM_Q_GET_USERS_REGISTRATION_BRAND_BY_USERID) // 2597	// 46[S|4<4>] reqId, errCode[errDescr|size<brandId>] must contain exact size, using default value, and order

// PYR-38811
#define DBM_Q_GET_USERS_REGISTRATION_BRAND_BY_USERINTID				1598	/// 44<4> reqId, size<userIntId>
#define DBM_A_GET_USERS_REGISTRATION_BRAND_BY_USERINTID				(DBM_DELTA + DBM_Q_GET_USERS_REGISTRATION_BRAND_BY_USERINTID) // 2598	// 46[S|4<4>] reqId, errCode[errDescr|size<brandId>] must contain exact size, using default value, and order

#define DBM_Q_UPDATE_TRANS_STATUS									1599 // 44ss reqId,transId,oldStatus,newStatus,comments
#define DBM_A_UPDATE_TRANS_STATUS									(DBM_DELTA + DBM_Q_UPDATE_TRANS_STATUS) // 46[s] reqId, errCode[errDescr]

#define DBM_Q_ADMIN_MANAGE_INSTALLID								1600 //4ssb reqId, userId, installId, validate/invalidate // #21829
#define DBM_A_ADMIN_MANAGE_INSTALLID								(DBM_DELTA + DBM_Q_ADMIN_MANAGE_INSTALLID) // 2600 //46[s] reqId errCode[errDescr] 

#define DBM_Q_UPDATE_TRANS_GATEWAY                                  1601 	//44<4s> reqId listSize <transId gatewayName>
#define DBM_A_UPDATE_TRANS_GATEWAY                                  (DBM_DELTA + DBM_Q_UPDATE_TRANS_GATEWAY) // 2601  //46[s|4<46[s]>] reqId err_code [errDescr|listSize <transId errCode[errDescr]>] 

#define DBM_Q_MOVE_USER_ALLOC_FPP                                   1602 	//4s8 reqId userId, appSessionId
#define DBM_A_MOVE_USER_ALLOC_FPP                                   (DBM_DELTA + DBM_Q_MOVE_USER_ALLOC_FPP) // 2602  //46[s] reqId, errCode [errDescr] 

#define DBM_Q_SAVE_UNIQUE_INI_PARAM                                 1603 	//4sssss reqId adminId sectionName paramName paramValue comment 
#define DBM_A_SAVE_UNIQUE_INI_PARAM                                 (DBM_DELTA + DBM_Q_SAVE_UNIQUE_INI_PARAM) // 2603 	//46[s] reqId, errCode [errDescr] 

#define DBM_Q_SAVE_NONUNIQUE_INI_PARAM                              1604 	//4sssss reqId adminId sectionName paramName paramValue comment 
#define DBM_A_SAVE_NONUNIQUE_INI_PARAM                              (DBM_DELTA + DBM_Q_SAVE_NONUNIQUE_INI_PARAM) // 2604 	//46[s] reqId, errCode [errDescr] 

// This message is meant for read-only DBM. May take long time to process.
#define DBM_Q_BUILD_USER_BAN_CASE                                   1605 	// 4s44<4> - regId, userId, maxGenerations, numExcludedCards<excludedCardId>
#define DBM_A_BUILD_USER_BAN_CASE                                   (DBM_DELTA + DBM_Q_BUILD_USER_BAN_CASE) // 2605  

#define DBM_Q_CREATE_BAN_CASE                                       1606 	// 444s - regId, reasonCode, growthLimit, comment
#define DBM_A_CREATE_BAN_CASE                                       (DBM_DELTA + DBM_Q_CREATE_BAN_CASE) // 2606  // 46[s|4] - regId, errCode[errDescr | caseId] 

#define DBM_Q_ADD_USERS_TO_BAN_CASE                                 1607 	// 444<M> - regId, caseId, numUsers<UserMsgBody> // for UserMsgBody format see DBM_A_BUILD_USER_BAN_CASE
#define DBM_A_ADD_USERS_TO_BAN_CASE                                 (DBM_DELTA + DBM_Q_ADD_USERS_TO_BAN_CASE) // 2607  // 46[s] - regId, errCode[errDescr] 

#define DBM_Q_ADMIN_CHANGE_REPRESENTMENT_DATE                       1608  // 4M(s4)44Ds reqId, M(adminId, adminIntId), transId, ordinalDate(1 - the 1st representment date, 2 - the second one), SrvDate, comment !!! NOTE: admin info sent for all cashier->dbm connections
#define DBM_A_ADMIN_CHANGE_REPRESENTMENT_DATE                       (DBM_DELTA + DBM_Q_ADMIN_CHANGE_REPRESENTMENT_DATE) // 2608  // 46[s] regId, errCode[errDescr] 

#define DBM_Q_ADMIN_CLOSE_REPRESENTMENT                             1609  // 4M(s4)44s  reqId, M(adminId, adminIntId), transId, status, comment !!! NOTE: admin info sent for all cashier->dbm connections
#define DBM_A_ADMIN_CLOSE_REPRESENTMENT                             (DBM_DELTA + DBM_Q_ADMIN_CLOSE_REPRESENTMENT) // 2609  // 46[s] regId, errCode[errDescr] 

#define DBM_Q_GET_NEWINSTALL_VALIDATIONCODE							1610 // 4ss reqId, userId, installId	// #21829
#define DBM_A_GET_NEWINSTALL_VALIDATIONCODE							(DBM_DELTA + DBM_Q_GET_NEWINSTALL_VALIDATIONCODE) //4s4b reqId, mobile, unique, new

#define DBM_Q_PROCESS_BAN_CASE_USERS                                1611  // 4s4<M> - regId, comment, numUsers<UserMsgBody> // for UserMsgBody format see DBM_A_BUILD_USER_BAN_CASE
#define DBM_A_PROCESS_BAN_CASE_USERS                                (DBM_DELTA + DBM_Q_PROCESS_BAN_CASE_USERS) // 2611  // 46[s|4<s>] - regId, errCode[errDescr|numUsers<userId>] 

#define DBM_Q_MANAGE_BAN_CASE                                       1612  // 44s44 - regId, caseId, startUser, startGeneration, banCaseStatus
#define DBM_A_MANAGE_BAN_CASE                                       (DBM_DELTA + DBM_Q_MANAGE_BAN_CASE) // 2612  // 46[s] - regId, errCode[errDescr | numUsers<UserMsgBody>] 

#define DBM_Q_PROCESS_BAN_CASE_CRITERIA                             1613  // 444<M>s - regId, caseId, numObjects<MsgBody>, comment MsgBody: s44s4 - userId, objectType, intObjectValue, strObjectValue, status
#define DBM_A_PROCESS_BAN_CASE_CRITERIA                             (DBM_DELTA + DBM_Q_PROCESS_BAN_CASE_CRITERIA) // 2613  // 46[s] - regId, errCode[errDescr] 

#define DBM_Q_GET_RELATED_BAN_CASE_USERS                            1614  // 4s4<4b>b - regId, userId, numCases<caseId, includeChildren>, forUnban
#define DBM_A_GET_RELATED_BAN_CASE_USERS                            (DBM_DELTA + DBM_Q_GET_RELATED_BAN_CASE_USERS) // 2614  // 46[s] - regId, errCode[errDescr | numUsers<UserMsgBody>] 

//cashtrans
#define DBM_Q_GET_CASHTRANS_STATS                                   1615  // 4T - reqId, dateWhen
#define DBM_A_GET_CASHTRANS_STATS                                   (DBM_DELTA + DBM_Q_GET_CASHTRANS_STATS) // 2615  // 46[s|4<M>] - reqId, errCode, [errDescr|numRecords<MsgBody>] MsgBody: 4444444444 - userIntId, depositMask, sumDeposits, sumCashouts, sumChargeBacks, sumBonuses, sumTransferSent, sumTransferReceived, adminAdjustmentSum, otherSum 

#define DBM_Q_SAVE_CASHTRANS_STATS                                  1616  // 4T4<M> - reqId, dateWhen, numRecords<MsgBody> MsgBody: 4444444444 - userIntId, depositMask, sumDeposits, sumCashouts, sumChargeBacks, sumBonuses, sumTransferSent, sumTransferReceived, adminAdjustmentSum, otherSum
#define DBM_A_SAVE_CASHTRANS_STATS                                  (DBM_DELTA + DBM_Q_SAVE_CASHTRANS_STATS) // 2616  // 46[s|4] - reqId, errCode, [errDescr|numRecords] 

#define DBM_Q_MANAGE_BAN_CASE_HOLD                                  1617  // 44s4 - regId, caseId, startUser, startGeneration
#define DBM_A_MANAGE_BAN_CASE_HOLD                                  (DBM_DELTA + DBM_Q_MANAGE_BAN_CASE_HOLD) // 2617  // 46[s | 44<M>] - regId, errCode[errDescr | caseId, numObjects<MsgBody>] MsgBody: s44s4 - userId, objectType, intObjectValue, strObjectValue, status 

#define DBM_Q_GET_USER_CONSENTS                                     1618  // 4s - regId userId
#define DBM_A_GET_USER_CONSENTS                                     (DBM_DELTA + DBM_Q_GET_USER_CONSENTS) // 2618  // 46[s | 4 <MSG(44ss)>] - regId errCode[errDescr | consent Number <consentId, status, title, text>] 

#define DBM_Q_SAVE_USER_CONSENT                                     1619  // 4s44 - regId userId ConsentId status
#define DBM_A_SAVE_USER_CONSENT                                     (DBM_DELTA + DBM_Q_SAVE_USER_CONSENT) // 2619  // 46[s] regId errCode[errDescr] 

//PS-IP gateway to dbm
#define DBM_Q_IP_GET_ALL_USERS_IDS                                  1620  // 4 - reqId
#define DBM_A_IP_GET_ALL_USERS_IDS                                  (DBM_DELTA + DBM_Q_IP_GET_ALL_USERS_IDS) // 2620  //46[s|t4<4>] - reqId, err_code[errDescr|endTime, numUsers<userIntId>]  

#define DBM_Q_IP_USERS_BY_IDS                                       1621  // 44<s> - reqId, numUsers<userId>
#define DBM_A_IP_USERS_BY_IDS                                       (DBM_DELTA + DBM_Q_IP_USERS_BY_IDS) // 2621  //46[s|4<s4>] - reqId, err_code[errDescr|numUsers<userId, userIntId>] 

#define DBM_Q_GET_USERS_BY_PROP_TYPE_STR							1622 // OLAP only! // 4:reqId, 4:eUserPropertyType, s:UserProperties.propStr
#define DBM_A_GET_USERS_BY_PROP_TYPE_STR							(DBM_DELTA + DBM_Q_GET_USERS_BY_PROP_TYPE_STR) // 2622 //46[s|4<UserWithProperties>]

#define DBM_Q_SET_SMSVALIDATION_FLAG								1623  // 4bs4 - reqId, enable, userId, userIntId
#define DBM_A_SET_SMSVALIDATION_FLAG								(DBM_DELTA + DBM_Q_SET_SMSVALIDATION_FLAG) //  2623 #21829

// Admin to DBM
#define DBM_Q_CLEAR_UNCLEARED_RECORD                                1624  //44s - reqId, userId, unclearedId, comment
#define DBM_A_CLEAR_UNCLEARED_RECORD                                (DBM_DELTA + DBM_Q_CLEAR_UNCLEARED_RECORD) // 2624  //46[s] - regId, errCode[errDescr] 

// Cashier to DBM
#define DBM_Q_REQUEST_REAL_TRANSFER                                 1625  // 4ss4ss4<> - reqId, userFrom, userTo, amountFrom, currencyFrom, comment, transferProcessing, appLoginEssentials
#define DBM_A_REQUEST_REAL_TRANSFER                                 (DBM_DELTA + DBM_Q_REQUEST_REAL_TRANSFER) // 2625  // 46[s|44bM] - regId, errCode[errDescr|transFromId, transToId, okForAutoTransfer, cashTransBody]

// Cashier or Admin to DBM
#define DBM_Q_COMPLETE_REAL_TRANSFER                                1626  // 4ss444sb4 - reqId, userFrom, userTo, transFromId, transToId, transferAmount, comment, awareOfBan, transferProcessing
#define DBM_A_COMPLETE_REAL_TRANSFER                                (DBM_DELTA + DBM_Q_COMPLETE_REAL_TRANSFER) // 2626  // 46[s|ss4484s] - regId, errCode[errDescr|dummy1 dummy2 dummy3 dummy4 convRate amountTo currencyTo] 

// Admin to DBM
#define DBM_Q_REJECT_REAL_TRANSFER                                  1627  // 4ss444ss - reqId, userFrom, userTo, transFromId, transToId, transferAmount, comment, errorCode
#define DBM_A_REJECT_REAL_TRANSFER                                  (DBM_DELTA + DBM_Q_REJECT_REAL_TRANSFER) // 2627  // 46[s|ss44]    - regId, errCode[errDescr|dummy1 dummy2 dummy3 dummy4] 

// Admin to DBM - LEVELPERMISSIONS
#define DBM_Q_ADD_LEVELPERMISSION                                   1628  //4444 - reqId, groupId, levelId, rank
#define DBM_A_ADD_LEVELPERMISSION                                   (DBM_DELTA + DBM_Q_ADD_LEVELPERMISSION) // 2628  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_UPD_LEVELPERMISSION                                   1629  //4444 - reqId, groupId, levelId, rank
#define DBM_A_UPD_LEVELPERMISSION                                   (DBM_DELTA + DBM_Q_UPD_LEVELPERMISSION) // 2629  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_DEL_LEVELPERMISSION                                   1630  //444  - reqId, groupId, levelId
#define DBM_A_DEL_LEVELPERMISSION                                   (DBM_DELTA + DBM_Q_DEL_LEVELPERMISSION) // 2630  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_CASHTRANS_ALL                                     1631  //44 - reqId, transId
#define DBM_A_GET_CASHTRANS_ALL                                     (DBM_DELTA + DBM_Q_GET_CASHTRANS_ALL) // 2631  //46[s|4...] reqId errCode [errDescr|size, numTrans<CashTrans>] 

#define DBM_Q_PROCESS_PAYMENT_NOTIFICATION                          1632  //44s4s4s44 - reqId purchaseId transType amount paySystem actionMask errorInfo userChips userChipsOwed
#define DBM_A_PROCESS_PAYMENT_NOTIFICATION                          (DBM_DELTA + DBM_Q_PROCESS_PAYMENT_NOTIFICATION) // 2632 	//46[s|4s4444M] reqId errCode [errDescr|transId, userId, userChips, userOwedChips, successActions, licenseId, UserMsgBody] 

// Admin to DBM
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_FREEZE_OBJECT                                         1633  // 44sds - reqId, objectType, objectValue, expDate, comment
#define DBM_A_FREEZE_OBJECT                                         (DBM_DELTA + DBM_Q_FREEZE_OBJECT) // 2633  // 46[s4|4] - reqId, errCode[errDescr, existingObjectId|objectId] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_EXPIRE_FROZEN_OBJECT                                  1634  // 444ss - reqId, objectType, objectId, objectValue, comment
#define DBM_A_EXPIRE_FROZEN_OBJECT                                  (DBM_DELTA + DBM_Q_EXPIRE_FROZEN_OBJECT) // 2634  // 46[s] - reqId, errCode[errDescr] 

// Admin to DBM
#define DBM_Q_UPDATE_REAL_TRANSFER_COMMENT                          1635  // 4ss444s - reqId, userFrom, userTo, transFromId, transToId, transferAmount, comment
#define DBM_A_UPDATE_REAL_TRANSFER_COMMENT                          (DBM_DELTA + DBM_Q_UPDATE_REAL_TRANSFER_COMMENT) // 2635  // 46[s|ss44] - regId, errCode[errDescr|dummy1 dummy2 dummy3 dummy4] 

#define DBM_Q_CREDIT_USERS_BONUS                                    1636  //444<s44s>s reqId, bonusId, numUsers<userId, amount, fpp, accountCurrency>, comment
#define DBM_A_CREDIT_USERS_BONUS                                    (DBM_DELTA + DBM_Q_CREDIT_USERS_BONUS) // 2636  //46[s|4<s6s>] reqId, err_code[errDescr | numusers<userId, err_code, errDescr>] 

// Cashier to DBM
#define DBM_Q_SAVE_LIVE_SUPPORT_REQUEST                             1637  // 4s484s - reqId, userId, supportType, objectId, requestType, comments
#define DBM_A_SAVE_LIVE_SUPPORT_REQUEST                             (DBM_DELTA + DBM_Q_SAVE_LIVE_SUPPORT_REQUEST) // 2637 // 46[s] - reqId, errCode[errDescr] 

// Lobby to DBM (read-only)
#define DBM_Q_GET_USER_BALANCE                                      1638  // 4ss4 - reqId, userId, currency, otherAllocated
#define DBM_A_GET_USER_BALANCE                                      (DBM_DELTA + DBM_Q_GET_USER_BALANCE) // 2638  // 46[s|b444444] - reqId, err_code[errDescr | hasAccountInCurrency, availableChips, totalChipsInCurrency, availableTChips, totalTChipsInCurrency, fpp, ltFpp] 

// PS-IP gateway to DBM
#define DBM_Q_PSIP_FPP_TRANSFER_REQUEST                             1639  // 4ss4bs - reqId, userId, fppTransactId, amount, fppNotVpp, comments
#define DBM_A_PSIP_FPP_TRANSFER_REQUEST                             (DBM_DELTA + DBM_Q_PSIP_FPP_TRANSFER_REQUEST) // 2639  // 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_VALIDATED_INSTALLIDS								1640  // 4s - reqId, userId
#define DBM_A_GET_VALIDATED_INSTALLIDS								(DBM_DELTA + DBM_Q_GET_VALIDATED_INSTALLIDS) // 2640 #21829

#define DBM_Q_SAVE_APPROVED_PURCHASE                                1641  //4sss4ssssss444ss8[44]s reqId gateway paySystem userId amount currency bonusCode xTransStr cardNum adminUserId productId daysToClear fppToClear unclearedFlags status errorInfo cashtrFlagsToRaise [userBalanceFromAdmin adminControlFlags] errCodeStr
#define DBM_A_SAVE_APPROVED_PURCHASE                                (DBM_DELTA + DBM_Q_SAVE_APPROVED_PURCHASE) // 2641  //46[s|4s444s4O] - reqId, errCode[errDescr|purchaseTransId userEmail userEmLocale licenseId fxReturnAmt balanceCurrency userIntId BonusExtraAttributes] 

#define DBM_Q_VIEW_USER_INFO                                        1642  //4s reqId userId
#define DBM_A_VIEW_USER_INFO                                        (DBM_DELTA + DBM_Q_VIEW_USER_INFO) // 2642  //46[s] - reqId errCode[errDescr] 

#define DBM_Q_USER_ACCOUNTS                                         1643  //4ss4 reqId userId paysystem daysBack returns.
#define DBM_A_USER_ACCOUNTS                                         (DBM_DELTA + DBM_Q_USER_ACCOUNTS) // 2643  //46[s|4<4T>] reqId errCode [errDescr | numAccounts <cc_id, lastPurchaseTime> 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_MODIFY_AUTH_VERIFIED                                  1644  //exactly as DBM_Q_MODIFY_AGE_VERIFIED:
#define DBM_A_MODIFY_AUTH_VERIFIED                                  (DBM_DELTA + DBM_Q_MODIFY_AUTH_VERIFIED) // 2644  //exactly as DBM_A_MODIFY_AGE_VERIFIED: 

// PYR-18383
#define DBM_Q_ADMIN_CLEAR_FISCALCODE								1645  //4ss reqId userId comment 
#define DBM_A_ADMIN_CLEAR_FISCALCODE								(DBM_DELTA + DBM_Q_ADMIN_CLEAR_FISCALCODE) // 2645 // 46[s] - reqId errCode[errDescr

// PYR-26990
#define DBM_Q_MOVE_SAT_WINNERS_RECORDS								1646 //44444f4<4> reqId,tournId,satelliteId,dbmIdFrom,dbmIdTo,realDbmId,numUsers<userIntId>
#define DBM_A_MOVE_SAT_WINNERS_RECORDS								(DBM_DELTA + DBM_Q_MOVE_SAT_WINNERS_RECORDS) // 2646 // 46[s] - reqId,errCode[errDescr]

#define DBM_Q_EXPIRE_FPP                                            1647 	// 4ss8 - reqId, reason, userId, fppBalance
#define DBM_A_EXPIRE_FPP                                            (DBM_DELTA + DBM_Q_EXPIRE_FPP) // 2647 	// 46[s] - reqId errCode[errDescr] 

// Admin to RW DBM
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_RESTORE_FPP                                           1648 	// 4s448s - reqId, userId, currentFppBalanceInCents, fppToRestoreInCents, transId, comment
#define DBM_A_RESTORE_FPP                                           (DBM_DELTA + DBM_Q_RESTORE_FPP) // 2648 	// 46[s] - reqId errCode[errDescr] 

#define DBM_Q_ADD_APPROVED_TRANS_EX                                 1649  //4s1s4ss4ss44 reqId, userId, transtype ('P' or 'R') paySystem, amount, comment, xTransStr, parentId, adminId, accountCurrency convRate originalTransId
#define DBM_A_ADD_APPROVED_TRANS_EX                                 (DBM_DELTA + DBM_Q_ADD_APPROVED_TRANS_EX) // 2649  //46[s|4s44] reqId errCode[errDescr|newTransId userEmail userLocale timezone licenseId] 

// PS-IP gateway to OLAP
#define DBM_Q_GET_ACTIVE_TRACKCODES_COUNT                           1650  // 4 - reqId
#define DBM_A_GET_ACTIVE_TRACKCODES_COUNT                           (DBM_DELTA + DBM_Q_GET_ACTIVE_TRACKCODES_COUNT) // 2650  // 46[s|numTrackCodes] - reqId errCode[errDescr] 

// PS-IP gateway to OLTP
#define DBM_Q_ASSIGN_TRACKCODE                                      1651  // 4ss - reqId, userId, trackCode
#define DBM_A_ASSIGN_TRACKCODE                                      (DBM_DELTA + DBM_Q_ASSIGN_TRACKCODE) // 2651  // 46[s|4<s6s>] - reqId errCode[errDescr|numErrors<userId, errCode, errDescr>] 

#define DBM_Q_ISSUE_TOURNAMENT_TICKETS                              1652  // 4s44 - reqId, userId, ticketTypeId, numTickets
#define DBM_A_ISSUE_TOURNAMENT_TICKETS                              (DBM_DELTA + DBM_Q_ISSUE_TOURNAMENT_TICKETS) // 2652  // 46[s] - reqId errCode[errDescr] 

#define DBM_Q_SET_USER_EMLOCALE                                     1653  //4s4 reqId userId localeId
#define DBM_A_SET_USER_EMLOCALE                                     (DBM_DELTA + DBM_Q_SET_USER_EMLOCALE) // 2653  //46[s|44] reqId errCode[errDescr|locale, emLocale] 

#define DBM_Q_GET_USERS_EMLOCALE                                    1654  //44<s> reqId numUsers <userId>
#define DBM_A_GET_USERS_EMLOCALE                                    (DBM_DELTA + DBM_Q_GET_USERS_EMLOCALE) // 2654  //46[s/4<s644>] reqId errCode[errDescr/numUsers<userid errcode userintid locale>] 

//obsolete, DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_TLB_ALL_BOP                                           1655  // 4 - reqId
//#define DBM_A_TLB_ALL_BOP                                           (DBM_DELTA + DBM_Q_TLB_ALL_BOP) // 2655  // 46[s|44<444TT4<s44>>] reqId, errCode[errDescr | numIntervals<type, planetKey, volumeType, srvFrom, srvTo, numPoints<userId, userIntId, points>>] 

//obsolete, DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_ISSUE_BOP_PRIZES                                      1656  // 44<s44444s> - reqId, numPrizes<userId, intervalId, division, subdivision, amount, ticketTypeId, comment, currency>
//#define DBM_A_ISSUE_BOP_PRIZES                                      (DBM_DELTA + DBM_Q_ISSUE_BOP_PRIZES) // 2656  // 46[s|4<s46s>] - reqId errCode[errDescr|numErrors<userId, intervalId, errCode, errDescr>] 

//obsolete, DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_TLB_USER_BOP_POINTS                                   1657  // 4s - reqId, userId
//#define DBM_A_TLB_USER_BOP_POINTS                                   (DBM_DELTA + DBM_Q_TLB_USER_BOP_POINTS) // 2657  // 46[s|4<44444444>] reqId, errCode[errDescr | numIntervals<planetKey, gamesPlayed, currentScoreLV, gamesToCompleteLVB, bestCompletedScoreLV, currentScoreHV, gamesToCompleteHVB, bestCompletedScoreHV>] 

//Cashier to dbm
#define DBM_Q_USER_AFFILIATION                                      1658  // 4ssb reqId userId affiliateId checkAffiliation
#define DBM_A_USER_AFFILIATION                                      (DBM_DELTA + DBM_Q_USER_AFFILIATION) // 2658  // 46[s] - reqId errCode[errDescr|balanceCurrency flags privs] 

#define DBM_Q_CASHIN_CC_INIT_RO                                     1659  // 
#define DBM_A_CASHIN_CC_INIT_RO                                     (DBM_DELTA + DBM_Q_CASHIN_CC_INIT_RO) // 2659  //  

//PYR-26251
#define DBM_QG_DEBIT_FUNDS											1660
#define DBM_AG_DEBIT_FUNDS											(DBM_DELTA + DBM_QG_DEBIT_FUNDS) // 2660

#define DBM_Q_CASHIN_CC_INIT_RW                                     1661  // 
#define DBM_A_CASHIN_CC_INIT_RW                                     (DBM_DELTA + DBM_Q_CASHIN_CC_INIT_RW) // 2661  //  

#define DBM_Q_GET_CASHTRANS_ALL_MULTISTAGE                          1662
#define DBM_A_GET_CASHTRANS_ALL_MULTISTAGE                          (DBM_DELTA + DBM_Q_GET_CASHTRANS_ALL_MULTISTAGE) // 2662

// from login dbm
#define DBM_Q_GET_USER                                              1663  //4s reqId userId
#define DBM_A_GET_USER                                              (DBM_DELTA + DBM_Q_GET_USER) // 2663  //46[s|(U)] reqId errCode[errDescr|User] 

// PYR-19436 - DBM_Q_SET_USER_FLAGS also sent from Cashier for blacklisted Belgium players
#define DBM_Q_SET_USER_FLAGS                                        1664  //4s4b448 reqId userId flagsMask setNotRemove eUpdateType adminIntId appLoginId
#define DBM_A_SET_USER_FLAGS                                        (DBM_DELTA + DBM_Q_SET_USER_FLAGS) // 2664  //46[s|4] reqId errCode[errDescr|newUserFlags] 

#define DBM_Q_USER_LOGGED_IN                                        1665  //4Os84ss4s448 reqId MessageInfoStruct userId loginId visitedFlags countryByIp stateByIp beUserLossLimitInit IP platformUsed brandsUsed appSessionId
#define DBM_A_USER_LOGGED_IN                                        (DBM_DELTA + DBM_Q_USER_LOGGED_IN) // 2665  //46[s|4m] reqId errCode[errDescr|newVisitedFlags,newVisitedFlagsEx] 

#define DBM_Q_USER_LOGGED_OUT                                       1666  //4Os8 reqId MessageInfoStruct userId loginId
#define DBM_A_USER_LOGGED_OUT                                       (DBM_DELTA + DBM_Q_USER_LOGGED_OUT) // 2666  //46[s] reqId errCode[errDescr] 

#define DBM_Q_ADMIN_RIGHTS                                          1667  //4s reqId userId
#define DBM_A_ADMIN_RIGHTS                                          (DBM_DELTA + DBM_Q_ADMIN_RIGHTS) // 2667  //46[s|2<s> reqId errCode[errDescr|numRights<adminRight>] 

#define DBM_Q_CHANGE_USER_PLAY_MONEY								1668 // 44844 reqId userIntId signedPlayMoneyDelta transType objectId
#define DBM_A_CHANGE_USER_PLAY_MONEY								(DBM_DELTA + DBM_Q_CHANGE_USER_PLAY_MONEY) // 2668 // 46[s|8] reqId errCode[errDescr | transactId]

#define DBM_Q_GET_USERS_PLAY_MONEY_AND_LOGIN						1669 // 44<4> reqId size<userIntId>
#define DBM_A_GET_USERS_PLAY_MONEY_AND_LOGIN						(DBM_DELTA + DBM_Q_GET_USERS_PLAY_MONEY_AND_LOGIN) // 2669 // 46[s|4<46[s|8b]>] reqId errCode[errDescr | size<userIntId userErrCode [userErrDescr | playMoney, isLoggedIn]>]

#define DBM_Q_GET_TRANSACT_BY_TYPE_AND_OBJECT						1670 // 444 reqId transType objectId
#define DBM_A_GET_TRANSACT_BY_TYPE_AND_OBJECT						(DBM_DELTA + DBM_Q_GET_TRANSACT_BY_TYPE_AND_OBJECT) // 2670 // 46[s|8s4T8] reqId errCode[errDescr | transId userId userIntId when playMoneyDelta]

#define DBM_Q_NEED_TABLE_DATA_FOR_RESURRECTION						1671 
#define DBM_A_NEED_TABLE_DATA_FOR_RESURRECTION						(DBM_DELTA + DBM_Q_NEED_TABLE_DATA_FOR_RESURRECTION) // 2671

#define DBM_Q_RESET_ALLOCATED_HAS_LEFT								1672 //48 reqId, tableid
#define DBM_A_RESET_ALLOCATED_HAS_LEFT								(DBM_DELTA + DBM_Q_RESET_ALLOCATED_HAS_LEFT) // 2672
// PYR-112271
#define DBM_Q_MTL_UPDATE_TOURN_PROPERTY								1673 //4o - srvshared/mtlobby/mtlobbyserverprot.txt
#define DBM_A_MTL_UPDATE_TOURN_PROPERTY								(DBM_DELTA + DBM_Q_MTL_UPDATE_TOURN_PROPERTY) // 2673

// PYR-28503 - now switched to cancellation in user batches
#define DBM_Q_CANCEL_RUNNING_TOURN_BATCH                            1674 // 44<484f77777744474447>4<8>44sf447s4f - reqId, numUsers<intId, tableId, tournChips, notEliminated, refund, refundT, refundFpp, refundPlay, refundRake, refundRakeT, ticketId, place, placeTo, tlbPoints, tuFlags, fromSatellite, flightFlags, newNumReentries>, numBlitzTables<tableId>, tournId, formula, tournCurrency, isBlitz, tournFlags, scalePM, awardCurrencyType, admission, targetId, finalBatch
#define DBM_A_CANCEL_RUNNING_TOURN_BATCH                            (DBM_DELTA + DBM_Q_CANCEL_RUNNING_TOURN_BATCH) // 2674  //46[s|4<4s>] - reqId, errCode[errDescr|numUsers<userIntId, userId>]

#define DBM_Q_PAM_GET_TARGET_USERS                                  1675 // 4444 - reqId,uploadId,batchSize,lastSeqId
#define DBM_A_PAM_GET_TARGET_USERS                                  (DBM_DELTA + DBM_Q_PAM_GET_TARGET_USERS) // 2675 // 46[s|4<M(44sssdt)>] - reqId,errCode[errDesc|numUsers<M(seqId,extUserIntId,email,fullName,phoneNumber,dateOfBirth,processedTime)>]

//#define DBM_Q_AVAILABLE_MESSAGE                                     1676 

#define DBM_Q_UPDATE_REPLAY_STATUS_NL                               1677 
#define DBM_A_UPDATE_REPLAY_STATUS_NL                               (DBM_DELTA + DBM_Q_UPDATE_REPLAY_STATUS_NL) // 2677  
// end _NL

#define DBM_Q_GET_TOURN_LOC_NAME                                    1678  //4s reqId tournId, userId1, userId2
#define DBM_A_GET_TOURN_LOC_NAME                                    (DBM_DELTA + DBM_Q_GET_TOURN_LOC_NAME) // 2678 //46[s|ss] - reqId, errCode[errDescr|locTournName1 locTournName1]

#define DBM_Q_CHECK_LOGIN_TRIGGERS                                  1679  //4s reqId userId
#define DBM_A_CHECK_LOGIN_TRIGGERS                                  (DBM_DELTA + DBM_Q_CHECK_LOGIN_TRIGGERS) // 2679  //46[s|44m] - reqId, errCode[errDescr|flags privs DBM_U_USER_EXCLUSION_EXPIRED_reply_body] 

#define DBM_Q_PROCESS_BAN_CASE_LOGINS                               1680  // Identical to DBM_Q_PROCESS_BAN_CASE_USERS
#define DBM_A_PROCESS_BAN_CASE_LOGINS                               (DBM_DELTA + DBM_Q_PROCESS_BAN_CASE_LOGINS) // 2680  // Identical to DBM_A_PROCESS_BAN_CASE_USERS 

#define DBM_Q_PROCESS_BAN_CASE_LOGIN_CRITERIA                       1681  // Identical to DBM_Q_PROCESS_BAN_CASE_CRITERIA
#define DBM_A_PROCESS_BAN_CASE_LOGIN_CRITERIA                       (DBM_DELTA + DBM_Q_PROCESS_BAN_CASE_LOGIN_CRITERIA) // 2681  // Identical to DBM_A_PROCESS_BAN_CASE_CRITERIA 

#define DBM_Q_ADD_LOGINS_TO_BAN_CASE                                1682  // Identical to DBM_Q_ADD_USERS_TO_BAN_CASE
#define DBM_A_ADD_LOGINS_TO_BAN_CASE                                (DBM_DELTA + DBM_Q_ADD_LOGINS_TO_BAN_CASE) // 2682  // Identical to DBM_A_ADD_USERS_TO_BAN_CASE 

#define DBM_Q_ADMIN_INSERT_PAYSYSTEM2                               1683  //4... reqId,(see Paysystem struct in dbmstruct.h)
#define DBM_A_ADMIN_INSERT_PAYSYSTEM2                               (DBM_DELTA + DBM_Q_ADMIN_INSERT_PAYSYSTEM2) // 2683  //46[s] reqId, errCode [errDescr] 

#define DBM_Q_ADMIN_UPDATE_PAYSYSTEM2                               1684  //4s... reqId, prevAllowedCountries, (see Paysystem struct in dbmstruct.h)
#define DBM_A_ADMIN_UPDATE_PAYSYSTEM2                               (DBM_DELTA + DBM_Q_ADMIN_UPDATE_PAYSYSTEM2) // 2684  //46[s] reqId, errCode [errDescr] 

#define DBM_Q_ADMIN_DELETE_PAYSYSTEM2                               1685  //4ss reqId, paysystemName, prevAllowedCountries
#define DBM_A_ADMIN_DELETE_PAYSYSTEM2                               (DBM_DELTA + DBM_Q_ADMIN_DELETE_PAYSYSTEM2) // 2685  //46[s] reqId, errCode [errDescr] 

// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_ADMIN_EXCLUDE_BOP_PLAYER                              1686  // 4ss - reqId, userId, reason
//#define DBM_A_ADMIN_EXCLUDE_BOP_PLAYER                              (DBM_DELTA + DBM_Q_ADMIN_EXCLUDE_BOP_PLAYER) // 2686  // 46[s|d] - reqId, errCode [errDescr|excludedFrom] 

// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_ADMIN_REINSTATE_BOP_PLAYER                            1687  // 4ss - reqId, userId, reason
//#define DBM_A_ADMIN_REINSTATE_BOP_PLAYER                            (DBM_DELTA + DBM_Q_ADMIN_REINSTATE_BOP_PLAYER) // 2687  // 46[s|d] - reqId, errCode [errDescr|reinstatedOn] 

#define DBM_Q_REFRESH_LOGIN_SETTINGS                                1688  //4 reqId
#define DBM_A_REFRESH_LOGIN_SETTINGS                                (DBM_DELTA + DBM_Q_REFRESH_LOGIN_SETTINGS) // 2688  //46[s] reqId, errCode [errDescr] 

#define DBM_Q_REFRESH_OLAP_SETTINGS                                 1689  //4 reqId
#define DBM_A_REFRESH_OLAP_SETTINGS                                 (DBM_DELTA + DBM_Q_REFRESH_OLAP_SETTINGS) // 2689  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_REFRESH_OLAP_LOGIN_SETTINGS                           1690  //4 reqId
#define DBM_A_REFRESH_OLAP_LOGIN_SETTINGS                           (DBM_DELTA + DBM_Q_REFRESH_OLAP_LOGIN_SETTINGS) // 2690  // 46[s] - reqId, errCode [errDescr] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_REMOVE_FROM_CACHE_LDBM                                1691  //4s reqId userId
#define DBM_A_REMOVE_FROM_CACHE_LDBM                                (DBM_DELTA + DBM_Q_REMOVE_FROM_CACHE_LDBM) // 2691  //identical to DBM_A_REMOVE_FROM_CACHE 

#define DBM_Q_CLEAR_CACHE_LDBM                                      1692  //4 reqId
#define DBM_A_CLEAR_CACHE_LDBM                                      (DBM_DELTA + DBM_Q_CLEAR_CACHE_LDBM) // 2692  //identical to DBM_A_CLEAR_CACHE 

#define DBM_Q_GET_SESSION_INFO                                      1693  //4s reqId userId
#define DBM_A_GET_SESSION_INFO                                      (DBM_DELTA + DBM_Q_GET_SESSION_INFO) // 2693  // 46[s|(B)t8] - reqId, errCode [errDescr|messageBody,lastLogin,loginFlags] 

// Lobby to DBM
#define DBM_Q_VERIFY_TRANSFER_CONDITIONS                            1694  // 4ss4s - reqId, userFrom, userTo, transferAmount, comment
#define DBM_A_VERIFY_TRANSFER_CONDITIONS                            (DBM_DELTA + DBM_Q_VERIFY_TRANSFER_CONDITIONS) // 2694  // 46s4b[M]b[M] regId, errCode, errDescr, transferProcessing, hasUserDataFrom, userDataFrom, hasUserDataTo, userDataTo

#define DBM_Q_BMTL_DELETE_TABLE                                     1695 //48 reqId,tableId
#define DBM_A_BMTL_DELETE_TABLE                                     (DBM_DELTA + DBM_Q_BMTL_DELETE_TABLE) //2695

// Admin to DBM
#define DBM_Q_STORE_ADMIN_DOC_CHUNK                                 1696  // 4444s - reqId, docId, version, chunkId, data
#define DBM_A_STORE_ADMIN_DOC_CHUNK                                 (DBM_DELTA + DBM_Q_STORE_ADMIN_DOC_CHUNK) // 2696  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_DELETE_ADMIN_DOC_CHUNKS                               1697  // 44444 - reqId, docId, version, fromChunk, toChunk
#define DBM_A_DELETE_ADMIN_DOC_CHUNKS                               (DBM_DELTA + DBM_Q_DELETE_ADMIN_DOC_CHUNKS) // 2697  // 46[s] - reqId, errCode [errDescr] 

#define DBM_Q_GET_SELF_EXCLUSION_BAN_CASE                           1698  // 4s - regId, userId
#define DBM_A_GET_SELF_EXCLUSION_BAN_CASE                           (DBM_DELTA + DBM_Q_GET_SELF_EXCLUSION_BAN_CASE) // 2698  // 46[s] - regId, errCode[errDescr | numUsers<UserMsgBody>] 

// DBD to Login DBM
#define DBM_Q_LDBM_CLEANUP_USER_CACHE                               1699  // 4 - regId
#define DBM_A_LDBM_CLEANUP_USER_CACHE                               (DBM_DELTA + DBM_Q_LDBM_CLEANUP_USER_CACHE) // 2699  // 46[s] - regId, errCode[errDescr] 

//from cashier to dbm - user wants to cash-in using credit card
#define DBM_Q_CASHIN_REPRESENTMENT_INIT                             1700  //4s4s12 reqId, userId, amount(cents), creditcardNum, expMonth, expYear ????
#define DBM_A_CASHIN_REPRESENTMENT_INIT                             (DBM_DELTA + DBM_Q_CASHIN_REPRESENTMENT_INIT) // 2700  //464[s|4442<D>] reqId, errCode, allowedAmount, [errDescr|transId parentTransId, amountCancelled numDetails<detail>] 

//from cashier to dbm - answer recieved from gateway
#define DBM_Q_CASHIN_CC_COMPLETE                                    1701  //44s41s reqId, transId, newStatus, amount, errorInfo
#define DBM_A_CASHIN_CC_COMPLETE                                    (DBM_DELTA + DBM_Q_CASHIN_CC_COMPLETE) // 2701  //46[s] reqId, errCode, [errDescr] 

//from cashier to dbm - give me all incomplete transactions
#define DBM_Q_CASHIER_GET_INCOMPLETE                                1702  //4 reqId
#define DBM_A_CASHIER_GET_INCOMPLETE                                (DBM_DELTA + DBM_Q_CASHIER_GET_INCOMPLETE) // 2702  //46[s|4<inTrans>4<outTrans>] reqId, errCode, [errDescr|numInTrans<...>numOutTrans<...>] 

//from cashier to dbm - give me all transactions of a user
#define DBM_Q_CASHIER_TRANS_HISTORY                                 1703  //4s4 reqId, userId, lastHours
#define DBM_A_CASHIER_TRANS_HISTORY                                 (DBM_DELTA + DBM_Q_CASHIER_TRANS_HISTORY) // 2703  //46[s|4<inTrans>4<outTrans>] reqId, errCode, [errDescr|num] 

#define DBM_Q_GET_USER_PROPERTIES_BY_USERID                         1704  //4s4<4> reqId, userId vecSize<userPropId>
#define DBM_A_GET_USER_PROPERTIES_BY_USERID                         (DBM_DELTA + DBM_Q_GET_USER_PROPERTIES_BY_USERID) // 2704  //46[s|vecSize<UserProp> + userRelatedInfo] 

#define DBM_Q_SET_USER_FACEBOOKINTID		                        1705  //448 reqId userIntId facebookIntId
#define DBM_A_SET_USER_FACEBOOKINTID			                    (DBM_DELTA + DBM_Q_SET_USER_FACEBOOKINTID) // 2705  //46[s]

/* removed with PYR-25629
// DBM_Q_AVAILABLE_MESSAGE
//from admin to dbm - give me "manual pending" transactions
#define DBM_Q_CASHIER_PENDING_MANUAL                                1706  //4s1 reqId, paySystem, transType([R]edeem or [P]urchase)
#define DBM_A_CASHIER_PENDING_MANUAL                                (DBM_DELTA + DBM_Q_CASHIER_PENDING_MANUAL) // 2706  // 46[s|4<Trans>] reqId, errCode, [errDescr|numInTrans<...>] 
*/
// PYR-22184 from Lobby to LoginDBM
#define DBM_Q_SMS_RESET_PWD                                         1707 	// 4ssssbsbsssbs reqId userId zip address newPwd verifyEmail email useQuestion question1 question2 mobile isFacebookFirstLogin ssn4
#define DBM_A_SMS_RESET_PWD                                         (DBM_DELTA + DBM_Q_SMS_RESET_PWD) // 2707 	// 46[s|ss4ss] reqId, err_code [errDescr|email, userId, locale, mobile, country] 

#define DBM_Q_US_MIGRATION_READ_IOM_ASSETS                          1708
#define DBM_A_US_MIGRATION_READ_IOM_ASSETS                          (DBM_DELTA + DBM_Q_US_MIGRATION_READ_IOM_ASSETS) // 2708

#define DBM_Q_NJ_MIGRATION_CONSOLIDATE_IOM_ASSETS					1709
#define DBM_A_NJ_MIGRATION_CONSOLIDATE_IOM_ASSETS                   (DBM_DELTA + DBM_Q_NJ_MIGRATION_CONSOLIDATE_IOM_ASSETS) // 2709

#define DBM_Q_PLAYMONEY_USER_MIGRATION_PERFORM                      1710 // 4s484 reqId userId targetLicenseId appLoginEssentials
#define DBM_A_PLAYMONEY_USER_MIGRATION_PERFORM                      (DBM_DELTA + DBM_Q_PLAYMONEY_USER_MIGRATION_PERFORM) // 2710

/* obsolete, DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_CASHIER_SET_BONUS                                     1711  //4222 reqId, percent, fppPercent, hands
#define DBM_A_CASHIER_SET_BONUS                                     (DBM_DELTA + DBM_Q_CASHIER_SET_BONUS) // 2711  //46[s] reqId, err_code[errDescr] 

#define DBM_Q_CASHIER_GET_BONUS                                     1712  //4 reqId
#define DBM_A_CASHIER_GET_BONUS                                     (DBM_DELTA + DBM_Q_CASHIER_GET_BONUS) // 2712  //46[s|222] reqId, err_Code [errDescr|percent, fppPercent, hands] 

#define DBM_Q_CASHIER_CANCEL_BONUS                                  1713  //4 reqId
#define DBM_A_CASHIER_CANCEL_BONUS                                  (DBM_DELTA + DBM_Q_CASHIER_CANCEL_BONUS) // 2713  //46[s|222] reqId, err_Code [errDescr|percent, fppPercent, hands] 
*/
#define DBM_Q_HOLD_USER_CASH_FUNDS                                  1714
#define DBM_A_HOLD_USER_CASH_FUNDS                                  (DBM_DELTA + DBM_Q_HOLD_USER_CASH_FUNDS) // 2714

#define DBM_Q_SAVE_BAD_CASHTRANS                                    1715  // see format of DBM_Q_CASHIN_CC_INIT
#define DBM_A_SAVE_BAD_CASHTRANS                                    (DBM_DELTA + DBM_Q_SAVE_BAD_CASHTRANS) // 2715  //46[s] reqId, err_Code [errDescr] 

//PYR-26251
#define DBM_QG_CREDIT_FUNDS											1716
#define DBM_AG_CREDIT_FUNDS											(DBM_DELTA + DBM_QG_CREDIT_FUNDS) // 2716

#define DBM_Q_GET_CASHTRANS_MULTISTAGE                              1717
#define DBM_A_GET_CASHTRANS_MULTISTAGE								(DBM_DELTA + DBM_Q_GET_CASHTRANS_MULTISTAGE) // 2717

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_FULLTILT_SET_PAIRING                                  1718  //
//#define DBM_A_FULLTILT_SET_PAIRING                                  (DBM_DELTA + DBM_Q_FULLTILT_SET_PAIRING) // 2718

// PYR-45667 message was left for the sake of NJ migration
#define DBM_Q_FULLTILT_DELETE_PAIRING                               1719  // 444s8 reqId psUserIntId ftUserIntId comment appLoginId
#define DBM_A_FULLTILT_DELETE_PAIRING                               (DBM_DELTA + DBM_Q_FULLTILT_DELETE_PAIRING) // 2719

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
// #define DBM_Q_FULLTILT_MIGRATION_PERFORM                            1720  //
// #define DBM_A_FULLTILT_MIGRATION_PERFORM                            (DBM_DELTA + DBM_Q_FULLTILT_MIGRATION_PERFORM) // 2720

// obsolete
//#define DBM_Q_FULLTILT_DEPOSIT_CHECK                                1721  //
//#define DBM_A_FULLTILT_DEPOSIT_CHECK                                (DBM_DELTA + DBM_Q_FULLTILT_DEPOSIT_CHECK) // 2721

// obsolete
//#define DBM_Q_FULLTILT_DEPOSIT_PERFORM                              1722  //
//#define DBM_A_FULLTILT_DEPOSIT_PERFORM                              (DBM_DELTA + DBM_Q_FULLTILT_DEPOSIT_PERFORM) // 2722

#define DBM_Q_CLOSE_ACCOUNT                                         1723  //4ssb4 reqId, userId, comment, keepPrivileges, negativeCategory
#define DBM_A_CLOSE_ACCOUNT                                         (DBM_DELTA + DBM_Q_CLOSE_ACCOUNT) // 2723  //46[s] reqId, err_code [errDescr] 

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_FULLTILT_CHECK_PAIRING                                1724  //
//#define DBM_A_FULLTILT_CHECK_PAIRING                                (DBM_DELTA + DBM_Q_FULLTILT_CHECK_PAIRING) // 2724

//#define DBM_Q_FULLTILT_SAVE_FT_MESSAGE						        1725  //
//#define DBM_A_FULLTILT_SAVE_FT_MESSAGE								(DBM_DELTA + DBM_Q_FULLTILT_SAVE_FT_MESSAGE) // 2725

#define DBM_Q_REOPEN_ACCOUNT                                        1726  //4ss[4] reqId, userId, comment, positiveCategory
#define DBM_A_REOPEN_ACCOUNT                                        (DBM_DELTA + DBM_Q_REOPEN_ACCOUNT) // 2726  //46[s] reqId, err_code [errDescr] 

// obsolete, TBD: AlexeyB delete when Admin is updated
#define DBM_Q_TRANSFER_FUNDS                                        1727  //4s4s4s11 reqId, userFrom, userFromBankroll, userTo, transferAmount, comment, awareOfBan, adminInitiated
#define DBM_A_TRANSFER_FUNDS                                        (DBM_DELTA + DBM_Q_TRANSFER_FUNDS) // 2727  //46[s|ss84s44] reqId, err_code [errDescr|dummy1 dummy2 convRate amountTo currencyTo transFromId transToId] 

#define DBM_Q_TRANSFER_FUNDS_BY_ADMIN                                  1727  //4s4s4s11s reqId, userFrom, userFromBankroll, userTo, transferAmount, comment, awareOfBan, dummy, currencyFrom
#define DBM_A_TRANSFER_FUNDS_BY_ADMIN                                  (DBM_DELTA + DBM_Q_TRANSFER_FUNDS_BY_ADMIN) // 2727  //46[s|ss84s44] reqId, err_code [errDescr|dummy1 dummy2 convRate amountTo currencyTo transFromId transToId]

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_FULLTILT_MIGRATION_CHECK                              1728  //
//#define DBM_A_FULLTILT_MIGRATION_CHECK                              (DBM_DELTA + DBM_Q_FULLTILT_MIGRATION_CHECK) // 2728

/// request from SQL client
#define DBM_Q_EXECUTE_STMT                                          1729  //4s reqId, statement
#define DBM_A_EXECUTE_STMT                                          (DBM_DELTA + DBM_Q_EXECUTE_STMT) // 2729  //46[s] reqId, err_code [errDescr] 
/* obsolete, DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_PLAY_TRANSFER                                         1730
#define DBM_A_PLAY_TRANSFER
*/
#define DBM_Q_SET_USER_CATEGORY                                     1731  //4ss reqId, user, category
#define DBM_A_SET_USER_CATEGORY                                     (DBM_DELTA + DBM_Q_SET_USER_CATEGORY) // 2731  //46[s] reqId, err_code [errDescr] 
/* obsolete, DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_CASHOUTS_DURING_BONUS                                 1732  //4s reqId, userId
#define DBM_A_CASHOUTS_DURING_BONUS                                 (DBM_DELTA + DBM_Q_CASHOUTS_DURING_BONUS) // 2732  //46[s|1] reqId, err_code[errDescr|answerByte] 
*/
#define DBM_Q_GET_TGM_TOURNS										1733  //44bbt reqId, tgmId, withUsers, activeOnly, periodStartTime
#define DBM_A_GET_TGM_TOURNS										(DBM_DELTA + DBM_Q_GET_TGM_TOURNS) // 2733  6[s|4<M(6[s|4<4tt4s44<444>>])>] reqId, errCode[ errMsg | msgCount < M( 6 [ s | tournCount <tournId, start, closed, status, name, dbmId, numUsers < userIntId , numReentries, tuflags > > ] )> ]
/* obsolete, DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_EXERCISE_BONUS                                        1734  //4s4 reqId, userId, amount
#define DBM_A_EXERCISE_BONUS                                        (DBM_DELTA + DBM_Q_EXERCISE_BONUS) // 2734  //46[s] reqId, err_code  
*/
#define DBM_Q_PRE_CASHIN                                            1735  //4ss4T reqId, userId, paySystem, amount, bonusSince
#define DBM_A_PRE_CASHIN                                            (DBM_DELTA + DBM_Q_PRE_CASHIN) // 2735  //46[s|4 BonusRefusalStruct 4<CashtransEssentials>]  reqId, err_code [errDescr|availAmount, bonus, vec_size<CashtransEssentials>] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_CREATE_TICKETS                                        1736  //4s4<s4>1T4s4[s] reqId, admission, numUsers<userId, addChips>,isMulti, expDate, ticketTypeId, comment, sendMask [,adminId]
#define DBM_A_CREATE_TICKETS                                        (DBM_DELTA + DBM_Q_CREATE_TICKETS) // 2736  //46[s6<s6s>] - reqId, err_code [errDescr, numUsers<userId, errCode, errDescr>] 

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_GET_FULLTILT_PAIR_SCORES                              1737 //4s reqId userId
//#define DBM_A_GET_FULLTILT_PAIR_SCORES                              (DBM_DELTA + DBM_Q_GET_FULLTILT_PAIR_SCORES) // 2737 46[s|{UserScore}] = reqId, err_code [errDescr|UserScore]

#define DBM_Q_UPDATE_FULLTILT_PAIR_SCORES                           1738 //44{UserScore} - reqId, userIntId, UserScore
#define DBM_A_UPDATE_FULLTILT_PAIR_SCORES                           (DBM_DELTA + DBM_Q_UPDATE_FULLTILT_PAIR_SCORES) // 2738 46[s]

// PYR-22691
#define DBM_Q_TGM_CLOSE												1739 //44 reqId, tgmId
#define DBM_A_TGM_CLOSE												(DBM_DELTA + DBM_Q_TGM_CLOSE) // 2739 46[s]

#define DBM_Q_MTL_RESEAT_USERS_EX                                   1740  //481<s814> req_id, oldTableId, numUsers< userId, newTableId, newPosition, newChips >
#define DBM_A_MTL_RESEAT_USERS_EX                                   (DBM_DELTA + DBM_Q_MTL_RESEAT_USERS_EX) // 2740  //46[s] req_id, err_code [errDescr] 

//#define DBM_Q_AVAILABLE_MESSAGE                                      1741

#define DBM_Q_SAVE_ADMINCHAT                                        1742  //4sss1t reqId adminId, userId, message, type, timestamp
#define DBM_A_SAVE_ADMINCHAT                                        (DBM_DELTA + DBM_Q_SAVE_ADMINCHAT) // 2742  //46[s] req_id, err_code [errDescr] 

// PYR-21124
#define DBM_Q_GET_ADMINCHAT_SESSIONID								1743	//	4 reqId
#define DBM_A_GET_ADMINCHAT_SESSIONID								(DBM_DELTA + DBM_Q_GET_ADMINCHAT_SESSIONID)	//	2743	//	46[s|4], reqId, errCode[errDescr | sessionId]

#define DBM_Q_GET_RELATEDUSERS                                      1744  //4
#define DBM_A_GET_RELATEDUSERS                                      (DBM_DELTA + DBM_Q_GET_RELATEDUSERS) // 2744  //46[s|4<ss>] reqId, errCode [errStr|num<userId, groupId>] 

//PYR-44179 - Changed bool to INT32 for severity
#define DBM_Q_ADD_RELATEDUSER                                       1745  //4sss4 reqId, userId, groupId, comment, severity
#define DBM_A_ADD_RELATEDUSER                                       (DBM_DELTA + DBM_Q_ADD_RELATEDUSER) // 2745  //46[s] req_id, err_code [errDescr] 

//PYR-41075 - changed format for new requirements
#define DBM_Q_DELETE_RELATEDUSER                                    1746  //4Oss4s reqId, MessageInfoStruct, userId, groupId, severity, comment
#define DBM_A_DELETE_RELATEDUSER                                    (DBM_DELTA + DBM_Q_DELETE_RELATEDUSER) // 2746  //46[s] req_id, err_code [errDescr] 
/* obsolete
// DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_PENDING_CHEQUES                                       1747  //41[TT] reqId reqType[dateFrom dateTo] 
#define DBM_A_PENDING_CHEQUES                                       (DBM_DELTA + DBM_Q_PENDING_CHEQUES) // 2747  //46[s|4<414T4sss411D>]  

#define DBM_Q_UPDATE_PENDING_CHEQUES                                1748  ////44<44s11sD> reqId numCheques<transId ordinal chqNum oldStatus newStatus, comment, mailedDate>
#define DBM_A_UPDATE_PENDING_CHEQUES                                (DBM_DELTA + DBM_Q_UPDATE_PENDING_CHEQUES) // 2748  //44<6s> reqId, numUpdates<err_code, errDescr> 

#define DBM_Q_CHEQUE_HISTORY                                        1749  ////444 reqId, transId, ordinal
#define DBM_A_CHEQUE_HISTORY                                        (DBM_DELTA + DBM_Q_CHEQUE_HISTORY) // 2749  ////46[s|4<Ts1ss>] reqId, err_code [errDescr | numItems<when, admin, what, newValue, comments> ] 
*/
// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_FULLTILT_MIGRATION_FAILED                             1750  //
//#define DBM_A_FULLTILT_MIGRATION_FAILED                             (DBM_DELTA + DBM_Q_FULLTILT_MIGRATION_FAILED) // 2750

#define DBM_Q_CHIPS_INFO_WITH_OPTIONS								1751  // 4s444<4> - reqId, userId userIntId eChipsInfoRequests size<userPropIds>
#define DBM_A_CHIPS_INFO_WITH_OPTIONS					            (DBM_DELTA + DBM_Q_CHIPS_INFO_WITH_OPTIONS) // 2751 46[s|UserChipsInfo,bool hasUserRolls [M{UserRolls}], bool hasCashinLimits [M{CashinLimits}], bool hasBonusData [M{BonusData}]]

//	PYR-24084: Request from admin CGI to DRC (to dmb and tabledmb) for chip details
#define DBM_Q_GET_ALLOCATED_CHIP_DETAILS							1752	//	4sb - reqId, userId, tournDbm
#define DBM_A_GET_ALLOCATED_CHIP_DETAILS							(DBM_DELTA + DBM_Q_GET_ALLOCATED_CHIP_DETAILS) // 2752	//	46[s|4<s4444s4sb>] reqId, err_code[errDescr|arrayCount<tableName,tableType,tournId,chips,chipsType,currency,blitzId,blitzSessionName,blitzIsPlayMoney>]

#define DBM_Q_GET_USER_AWARDS										1753	//	44 - reqId, userIntId
#define DBM_A_GET_USER_AWARDS										(DBM_DELTA + DBM_Q_GET_USER_AWARDS) // 2753

//cashier to dbm
#define DBM_Q_ROLLBACK_COMPLETE                                     1754  //441ss reqId, transId, status, xTransStr, errInfo
#define DBM_A_ROLLBACK_COMPLETE                                     (DBM_DELTA + DBM_Q_ROLLBACK_COMPLETE) // 2754  //46[s|14] reqId, err_code[ errDescr | parentTransFinished, parentTransId ] 


#define DBM_Q_CASHOUT_CHQ_COMPLETE                                  1755  //44s41s reqId, transId, newStatus, amount, errorInfo
#define DBM_A_CASHOUT_CHQ_COMPLETE                                  (DBM_DELTA + DBM_Q_CASHOUT_CHQ_COMPLETE) // 2755  //46[s] 
/* obsolete
// DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_USER_PENDING_CHEQUES                                  1756  //4s reqId, userId
#define DBM_A_USER_PENDING_CHEQUES                                  (DBM_DELTA + DBM_Q_USER_PENDING_CHEQUES) // 2756  //46[s|444] reqId, err_code[ errDescr | amountPendingExternal, userFlags, userPrivs] 
*/

#define DBM_Q_SET_TOURN_START                                       1757  //44T44s - reqId, tournId, startdatetime, tlbBoard, tournFee, currency
#define DBM_A_SET_TOURN_START                                       (DBM_DELTA + DBM_Q_SET_TOURN_START) // 2757  //46[s|4] reqId errCode[errDescr|tlbBoard] 

/* obsolete
// DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_USER_PENDING_CHEQUES_PP                               1758  //see DBM_Q_USER_PENDING_CHEQUES
#define DBM_A_USER_PENDING_CHEQUES_PP                               (DBM_DELTA + DBM_Q_USER_PENDING_CHEQUES_PP) // 2758  //see DBM_A_USER_PENDING_CHEQUES 
*/
#define DBM_Q_REMOVE_TGM_TOURN                                      1759 //444 reqId, tgmId, tournId
#define DBM_A_REMOVE_TGM_TOURN                                      (DBM_DELTA + DBM_Q_REMOVE_TGM_TOURN) // 2759   46[s] 

#define DBM_Q_LDBM_SET_USER_LIMITED_MODE							1760 //4sb4 - reqId, userId, setNotDrop, limitedModeTypeMask
#define DBM_A_LDBM_SET_USER_LIMITED_MODE							(DBM_DELTA + DBM_Q_LDBM_SET_USER_LIMITED_MODE) // 2760   46[s] 

#define DBM_Q_GET_VERIFIED_CASHTRANS                                1761
#define DBM_A_GET_VERIFIED_CASHTRANS                                (DBM_DELTA + DBM_Q_GET_VERIFIED_CASHTRANS) // 2761

// PYR-26990
#define DBM_Q_GET_SAT_WINNERS_RECORDS								1762 //47M([444<4>]) - reqId,op,M([tournId,satelliteId,num<userIntId>])
#define DBM_A_GET_SAT_WINNERS_RECORDS								(DBM_DELTA + DBM_Q_GET_SAT_WINNERS_RECORDS) // 2762 // 46[s|4<444s74s744>] - reqId,errCode[errDescr|num<tournId,userIntId,fromSat,userId,chips,ticketId,ipAddr,numReentries,dbmId,flags>]

#define DBM_Q_UPDATE_SAT_WINNERS_RECORDS							1763 //474<M(444[s74s744])> - reqId,op,num<M(tournId,userIntId,fromSat[userId,chips,tournTicket,ipAddr,numReentries,dbmId,flags])>
#define DBM_A_UPDATE_SAT_WINNERS_RECORDS							(DBM_DELTA + DBM_Q_UPDATE_SAT_WINNERS_RECORDS) // 2763 //46[s] - reqId,errCode[errDescr]

#define DBM_Q_CASHIN_NT_COMPLETE                                    1764  // 414s4s reqId status retAmount errorInfo systemFee xTransStr
#define DBM_A_CASHIN_NT_COMPLETE                                    (DBM_DELTA + DBM_Q_CASHIN_NT_COMPLETE) // 2764  ////46[s] reqId, err_code [errDescr] 

/* obsolete PYR-61156
#define DBM_Q_GET_APPROVED_CASHOUTS									1765 // 4st reqId, paysystem, fromtime
#define DBM_A_GET_APPROVED_CASHOUTS									(DBM_DELTA + DBM_Q_GET_APPROVED_CASHOUTS) //46{s|4,...} reqId, errCode, {errDescr|size,cashtransData,..}
*/

//from admin - cheques and neteller redeems by status(es)
#define DBM_Q_NEW_CHQS_BY_STATUS                                    1766  // 41ss  reqId gateway(N or Q), gateway, status(es)
#define DBM_A_NEW_CHQS_BY_STATUS                                    (DBM_DELTA + DBM_Q_NEW_CHQS_BY_STATUS) // 2766  // 46[s|4<(Trn)(D)ss>] reqId, err_code[errDescr|numTrans<(CashTransaction)whenMailed wayBill phone>] 

//from admin - update cheques and neteller redeems
#define DBM_Q_NEW_CHQS_UPDATE                                       1767  // 44<411ss(D)s>  reqId num_cheqs<transId, oldstatus, newstatus, xTransStr, errorInfo, whenMailed wayBill>
#define DBM_A_NEW_CHQS_UPDATE                                       (DBM_DELTA + DBM_Q_NEW_CHQS_UPDATE) // 2767  // 46[s|4<46s[(Trn)s]>]  reqId err_code[errDescr| num_cheqs<transId, err_code, err_descr[CashTrans phone]> ] 

#define DBM_Q_NEW_CHQ_BY_ID                                         1768  // 44  reqId transId
#define DBM_A_NEW_CHQ_BY_ID                                         (DBM_DELTA + DBM_Q_NEW_CHQ_BY_ID) // 2768  // 46[s|(Trn)(D)s] reqId, err_code[errDescr|CashTransaction whenMailed wayBill] 

//from admin - reread settings from ini file
#define DBM_Q_REFRESH_SETTINGS                                      1769  // 4b  reqId, doBroadcast
#define DBM_A_REFRESH_SETTINGS                                      (DBM_DELTA + DBM_Q_REFRESH_SETTINGS) // 2769  // 46 reqId errCode (0) 

/* obsolete, DBM_Q_AVAILABLE_MESSAGE
#define DBM_Q_ADMIN_ADD_BONUS                                       1770  //4sTTT44s44ss reqId name announsed start end percent max conditions fppPerDollar flags url description
#define DBM_A_ADMIN_ADD_BONUS                                       (DBM_DELTA + DBM_Q_ADMIN_ADD_BONUS) // 2770  //46[s] reqId, err_code [errDescr|bonusId] 

#define DBM_Q_ADMIN_CANCEL_BONUS                                    1771  //44 reqId bonusId
#define DBM_A_ADMIN_CANCEL_BONUS                                    (DBM_DELTA + DBM_Q_ADMIN_CANCEL_BONUS) // 2771  //46[s] reqId, err_code [errDescr] 
*/
#define DBM_Q_GET_USER_BONUSHIST                                    1772  //4s reqId userId
#define DBM_A_GET_USER_BONUSHIST                                    (DBM_DELTA + DBM_Q_GET_USER_BONUSHIST) // 2772  ////46[s|2<sT444444>] reqId errCode[errDescr | numItems<bonusName when transId amount fppToCollect fppCollecred amountCredited creditTransId bonusType>] 

#define DBM_Q_PRE_CASHOUT                                           1773  //4s4s[4841] reqId userId amount balanceCurrency [maxPeriod appSessionId brandId userRollbackOption - from cashier]
#define DBM_A_PRE_CASHOUT                                           (DBM_DELTA + DBM_Q_PRE_CASHOUT) // 2773  //46[s|1ss44s42<1Tss44s8444b4s>] reqId err_code[errDescr | needsBonusWarning bonusName bonusUrl bonusPercent, bonusMaxAmount userEmail estimatedRollback numFinalRedeemTypes<cashoutOption>] 
// cashoutOption - cashinByte srvWhen reference encCardNum limitAmount isAllowed currCode convRate64 maxNum maxSum period accountNotProcessorLimit cardId productId


// PYR-84874 NOTE: if you modify contents of the message in the part before userId please change AdminBrokerProtocols in srvshared also
#define DBM_Q_ADMIN_ADD_USERBONUS                                   1774  //444sss4s[s] reqId bonusId pseudoPurchaseAmount userId comment code fppCollected accountCurrency [userComment]
#define DBM_A_ADMIN_ADD_USERBONUS                                   (DBM_DELTA + DBM_Q_ADMIN_ADD_USERBONUS) // 2774  //46[s] reqId err_code[errDescr] 

#define DBM_Q_GET_CASHOUT_WO_DEPOSIT                                1775
#define DBM_A_GET_CASHOUT_WO_DEPOSIT                                (DBM_DELTA + DBM_Q_GET_CASHOUT_WO_DEPOSIT) // 2775

#define DBM_Q_BLOCK_USER_CARD                                       1776  // 4ss1ss4484 reqId, userId, cardNum, blockNotUnblock comment paySystem cc_id owner appLoginId flags
#define DBM_A_BLOCK_USER_CARD                                       (DBM_DELTA + DBM_Q_BLOCK_USER_CARD) // 2776  // 46[s|4b] reqId err_code[errDescr | userCardId InsertedNotUpdated]

// from admin - list of purchases with "Unknown" status ('U')
#define DBM_Q_PURCHASES_UNKNOWN                                     1777  //4 reqId
#define DBM_A_PURCHASES_UNKNOWN                                     (DBM_DELTA + DBM_Q_PURCHASES_UNKNOWN) // 2777  // 46[s|4<(Trn)>] reqId err_code[errDescr|numTrans<(CashTransaction)>] 

#define DBM_Q_UPDATE_UNKNOWN                                        1778  // same as DBM_Q_UPDATE_HOLD
#define DBM_A_UPDATE_UNKNOWN                                        (DBM_DELTA + DBM_Q_UPDATE_UNKNOWN) // 2778  // 46[s|4<46s1>]  reqId errCode[errDescr | num_cheqs<transId, err_code, err_descr, newStatus>] 

#define DBM_Q_ADMIN_DELETE_USERBONUS                                1779  //4444ss reqId userBonusId bonusId amount userId comment //bonusId and userId are redundant and sent for self-control
#define DBM_A_ADMIN_DELETE_USERBONUS                                (DBM_DELTA + DBM_Q_ADMIN_DELETE_USERBONUS) // 2779  //46[s] reqId err_code[errDescr] 

#define DBM_Q_SAVE_ADMINCHAT_OK                                     1780  //4s reqId userId
#define DBM_A_SAVE_ADMINCHAT_OK                                     (DBM_DELTA + DBM_Q_SAVE_ADMINCHAT_OK) // 2780  //46[s] reqId err_code[errDescr] 

#define DBM_Q_MTL_SET_FINAL_FLIGHT_END_DATE                         1781 //44
#define DBM_A_MTL_SET_FINAL_FLIGHT_END_DATE                         (DBM_DELTA + DBM_Q_MTL_SET_FINAL_FLIGHT_END_DATE) //2781 46[s]

//PYR-25825
#define DBM_Q_GET_FIRSTDEP_MAA_DATA                                 1782  // 4ss444<s> - reqId, userId, paysystem, transId, minutesSince1stDeposit nRelated <relatedId>
#define DBM_A_GET_FIRSTDEP_MAA_DATA                                 (DBM_DELTA + DBM_Q_GET_FIRSTDEP_MAA_DATA) // 2782  // 46[s|444] - reqId, errCode, [errDesc|ruleType,minAmount,AccAmount] 

#define DBM_Q_SET_NOMAIL_PRIVILEGES                                 1783  //414<s> reqId removeNotSet numUsersr<userId>
#define DBM_A_SET_NOMAIL_PRIVILEGES                                 (DBM_DELTA + DBM_Q_SET_NOMAIL_PRIVILEGES) // 2783  //44<6s> reqId numUsersr<errCode errDescr> 

#define DBM_Q_UPDATE_TOURN_LEADERS                                  1784 	//44 reqId, tournId
#define DBM_A_UPDATE_TOURN_LEADERS                                  (DBM_DELTA + DBM_Q_UPDATE_TOURN_LEADERS) // 2784  //46[s|44] reqId err_code[errDescr|numTotal, numTlb] 

#define DBM_Q_SET_CREDITBACKABLE                                    1785 	//441ss reqId, transId, isCreditbackable userId, comment
#define DBM_A_SET_CREDITBACKABLE                                    (DBM_DELTA + DBM_Q_SET_CREDITBACKABLE) // 2785  //46[s] reqId err_code[errDescr] 

// #define DBM_Q_AVAILABLE_MESSAGE                                     1786 	//44<4> reqId, numHands<handId>

#define DBM_Q_SAVE_AWARD					                        1787 	//4 Award  reqId [Award - optional for 2013-07 build]
#define DBM_A_SAVE_AWARD						                    (DBM_DELTA + DBM_Q_SAVE_AWARD) // 2787  //46[s|4] reqId err_code[errDescr | awardId] 

#define DBM_Q_USER_TLB                                              1788 	//4sT reqId userId startAfter
#define DBM_A_USER_TLB                                              (DBM_DELTA + DBM_Q_USER_TLB) // 2788 	//46[s|ss4<444>] reqId err_code[errDescr|userId, email numRecords<tournId, place, points>] 

// #define DBM_Q_AVAILABLE_MESSAGE                                             1789 	// empty

/// from sqlcli - ids of tables closed n days ago
#define DBM_Q_CLOSED_TABLES                                         1790 	// 14 isPlayMoney daysAgo
#define DBM_A_CLOSED_TABLES                                         (DBM_DELTA + DBM_Q_CLOSED_TABLES) // 2790 	// 46[s|4<4>] errCode [errDescr|numTables<tableId>] 

// #define DBM_Q_AVAILABLE_MESSAGE                                 1791      // 44<8> reqId, numTables<tableId>

// from admin - export critical info from tables to text files
#define DBM_Q_SAVE_CRITICAL                                         1792 	//4 reqId
#define DBM_A_SAVE_CRITICAL                                         (DBM_DELTA + DBM_Q_SAVE_CRITICAL) // 2792 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_TLB_ALL                                               1793 	//41<1(T)(T)>4 reqId, numIntervals<type, srvFrom, srvTo>, tlbBoard {eKeyRegularTlb (default if not specified) or eKeyItRegularTlb}
#define DBM_A_TLB_ALL                                               (DBM_DELTA + DBM_Q_TLB_ALL) // 2793 	//46[s4|4<pnt>4<pnt>4<pnt>1<(T)(T)4<pnt>>4] - reqId,errCode,[errDescr,tlbBoard|numIntervals,<type,srvFrom,srvTo,numPoints,<intervalTlb>>,tlbBoard] 

#define DBM_Q_ADD_ADMINGROUP                                        1794 	//4sss - reqId, groupName, groupDisplayName, groupDescription
#define DBM_A_ADD_ADMINGROUP                                        (DBM_DELTA + DBM_Q_ADD_ADMINGROUP) // 2794 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_DELETE_ADMINGROUP                                      1795 	//4s reqId, groupName
#define DBM_A_DELETE_ADMINGROUP                                     (DBM_DELTA + DBM_Q_DELETE_ADMINGROUP) // 2795 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_ADD_GROUPRIGHT                                        1796 	//4ss reqId, groupName, rightName
#define DBM_A_ADD_GROUPRIGHT                                        (DBM_DELTA + DBM_Q_ADD_GROUPRIGHT) // 2796 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_DELETE_GROUPRIGHT                                     1797 	//4ss reqId, groupName, rightName
#define DBM_A_DELETE_GROUPRIGHT                                     (DBM_DELTA + DBM_Q_DELETE_GROUPRIGHT) // 2797 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_ADD_GROUPUSER                                         1798 	//4ss reqId, groupName, userName
#define DBM_A_ADD_GROUPUSER                                         (DBM_DELTA + DBM_Q_ADD_GROUPUSER) // 2798 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_DELETE_GROUPUSER                                      1799 	//4ss reqId, groupName, userName
#define DBM_A_DELETE_GROUPUSER                                      (DBM_DELTA + DBM_Q_DELETE_GROUPUSER) // 2799 	//46[s] reqId errCode [errDescr] 

#define DBM_Q_GET_NEXT_CROPDATE                                     1800 	//444 reqId, cropType, dependsOnCropType
#define DBM_A_GET_NEXT_CROPDATE                                     (DBM_DELTA + DBM_Q_GET_NEXT_CROPDATE) // 2800 	//46[s|4(D)} reqId err_code[errDescr | type date ] 

#define DBM_Q_SAVE_LAST_CROPPED                                     1801 	//44(D) reqId, cropType, dateCropped
#define DBM_A_SAVE_LAST_CROPPED                                     (DBM_DELTA + DBM_Q_SAVE_LAST_CROPPED) // 2801 	//46[s] reqId errCode [errDescr] 

// #define DBM_Q_AVAILABLE_MESSAGE                                 1802 	//41(D)  reqId, chipsType, date

// #define DBM_Q_AVAILABLE_MESSAGE                                    1803     //14 (D) isPlayMoney date

//from admin - give me info about active tables
// Obsolete
#define DBM_Q_ACTIVE_TABLES_SIDE                                    1804 	//4 reqId
#define DBM_A_ACTIVE_TABLES_SIDE                                    (DBM_DELTA + DBM_Q_ACTIVE_TABLES_SIDE) // 2804 	//46[s|4<4s1111141<s4>>] reqId err_code[errDescr|numTables< tableId, tableName, game structure, hilo playMoney oneOnOne handId numUsers<userId chips>>] 

#define DBM_Q_ACTIVE_TABLES_TOURN                                   1805 	//4 reqId
#define DBM_A_ACTIVE_TABLES_TOURN                                   (DBM_DELTA + DBM_Q_ACTIVE_TABLES_TOURN) // 2805 	//46[s|4<4s1114141<s4>>] reqId err_code[errDescr|numTables<tableId tableName game structure hilo buyIn maxPerTable activeHandId numUsers<userId chips>>] 

#define DBM_Q_DELETE_CARDINFO_BY_INFOID                             1806 	// 44ss8 requestId CARDINFO.infoId verifUserId verifPaysystem appLoginId
#define DBM_A_DELETE_CARDINFO_BY_INFOID                             (DBM_DELTA + DBM_Q_DELETE_CARDINFO_BY_INFOID) // 2806

#define DBM_Q_GET_USER_CARDINFOS                                    1807 	// 4s reqId userId
#define DBM_A_GET_USER_CARDINFOS                                    (DBM_DELTA + DBM_Q_GET_USER_CARDINFOS) // 2807

#define DBM_Q_COLLECT_CARDINFOS_DATA                                1808
#define DBM_A_COLLECT_CARDINFOS_DATA                                (DBM_DELTA + DBM_Q_COLLECT_CARDINFOS_DATA) // 2808

#define DBM_Q_UPDATE_CARDINFOS_DATA                                 1809
#define DBM_A_UPDATE_CARDINFOS_DATA                                 (DBM_DELTA + DBM_Q_UPDATE_CARDINFOS_DATA) // 2809

// PYR-25162 from CRMServer to OLTP
#define DBM_Q_REMOVE_USER_TRACKCODE									1810	// 4ss4<4> reqId trackcode adminId userid, userintid>
#define DBM_A_REMOVE_USER_TRACKCODE									(DBM_DELTA + DBM_Q_REMOVE_USER_TRACKCODE)	// 2810 46[s|4m] reqId err_code [errDescr|numUsers message(<userId err_code errDescr>)]

// #define DBM_Q_AVAILABLE_MESSAGE											1811	//4b4<s> - reqId,optedOut,numNames,<goldenSnGName>

#define DBM_Q_GET_LIABILITY                                         1812 	// 4 reqId
#define DBM_A_GET_LIABILITY                                         (DBM_DELTA + DBM_Q_GET_LIABILITY) // 2812 	// 4(L) reqId(liability) 

#define DBM_Q_GET_EXT_LIABILITY_USR_MONEYONTBL                      1813 	// 44t4<4>4 reqId, licenseId, timeTaken, numLics<licId>, dailyTournLiabilityType
#define DBM_A_GET_EXT_LIABILITY_USR_MONEYONTBL                      (DBM_DELTA + DBM_Q_GET_EXT_LIABILITY_USR_MONEYONTBL) // 2813 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_MTL_MASS_REGISTER                                     1814 	//44s4ff444<s>  reqId, tournId, adminId, registrationSites, isDKplayerRegistered, isBlitzMode, brandId, numUsers<userId>
#define DBM_A_MTL_MASS_REGISTER                                     (DBM_DELTA + DBM_Q_MTL_MASS_REGISTER) // 2814 	//46[s|4<4s44ss44s44t48484M(BrandImages)>4<>] reqId, err_code[errDescr|numUsers<userIntId, userId, initChips, buyIn, city, email, imageId, effectivePrivileges, fullName, cameFromTourn, locale, registerTime,emLocale,effPriv2,vipStatus,flags2,hallOfFameLevel,M(BrandImages)>4<rejectedUsers>] 

#define DBM_Q_MTL_MASS_UNREGISTER                                   1815 	//44s44<s>  reqId, tournId, adminId, registrationSites, numUsers<userId>
#define DBM_A_MTL_MASS_UNREGISTER                                   (DBM_DELTA + DBM_Q_MTL_MASS_UNREGISTER) // 2815 	//46[s] reqId, err_code[errDescr] 

//#define DBM_Q_AVAILABLE_MESSAGE                                           1816 	//41s4 reqId awareOfRestriction userId receiverIntId

//#define DBM_Q_AVAILABLE_MESSAGE                                           1817 	//4144(T)sssss84 reqId awareOfRestriction wuTransId amount received wuCtrlNum crCtrlNum crWuOperator comment cashtrStatus cashtrFlagsToRaise wuOfficeId

// PYR-58575
#define DBM_Q_SET_GDPR_READY                                        1816  //4s reqId, userId
#define DBM_A_SET_GDPR_READY                                        (DBM_DELTA + DBM_Q_SET_GDPR_READY) // 2816  //46[s|b] reqId, err_code [errDescr|needCloseAccount] 

//#define DBM_Q_AVAILABLE_MESSAGE									1817

// PYR-24630
#define DBM_Q_MTL_SAVE_TOURN_ITID									1818	//44s reqId,tournId,aamsSessionId
#define DBM_A_MTL_SAVE_TOURN_ITID									(DBM_DELTA + DBM_Q_MTL_SAVE_TOURN_ITID) // 2818	// 46[s] reqId, err_code[errDescr]

#define DBM_Q_SAVE_TABLE_ITID										1819	//48s reqId,tableId,aamsSessionId
#define DBM_A_SAVE_TABLE_ITID										(DBM_DELTA + DBM_Q_SAVE_TABLE_ITID) // 2819	// 46[s] reqId, err_code[errDescr]

#define DBM_Q_BL_SAVE_BLITZ_ITID									1820	//44s reqId,blitzId,aamsSessionId
#define DBM_A_BL_SAVE_BLITZ_ITID									(DBM_DELTA + DBM_Q_BL_SAVE_BLITZ_ITID) // 2820	// 46[s] reqId, err_code[errDescr]

//DBM_Q_AVAILABLE_MESSAGE
//	PYR-25263, Guest Account.  LobbyEtc to DBM RO.
//#define DBM_Q_GET_GUEST_POOL										1821	//	4	reqId //PYR-91342, deprecated
//#define DBM_A_GET_GUEST_POOL										(DBM_DELTA + DBM_Q_GET_GUEST_POOL)	//	2821	//	46[s|4<s4>]	reqId, err_code[errDescr|numRecords<userid,licenseId>] //PYR-91342, deprecated

//DBM_Q_AVAILABLE_MESSAGE
//	PYR-25263, Guest Account.  LobbyEtc to DBM RW.
//#define DBM_Q_INIT_GUEST_ACCOUNT									1822	//	4ss448	reqId,userId,password,locale,timezone,appLoginId //PYR-91342, deprecated
//#define DBM_A_INIT_GUEST_ACCOUNT									(DBM_DELTA + DBM_Q_INIT_GUEST_ACCOUNT)	//	2822	//	46[s|8] reqId, err_code[errDescr|startChips] //PYR-91342, deprecated


// PYR-24001 LobbyEtc to DBM RW.
#define DBM_Q_SET_USER_EMLOCALES                                    1823  //4s444 reqId userId emLocale, emLocale2, emLocale3
#define DBM_A_SET_USER_EMLOCALES                                    (DBM_DELTA + DBM_Q_SET_USER_EMLOCALES) // 2823  //46[s|444444444m] reqId errCode[errDescr|locale, emLocale, emLocale2, emLocale3, userIntId, emLocaleOrg, platformUsed, siteVisited, brandUsed, visitedEx] 

// PYR-24001 LobbyEtc to DBM RO.
#define DBM_Q_GET_USER_EMLOCALES                                    1824  //4s reqId userId
#define DBM_A_GET_USER_EMLOCALES                                    (DBM_DELTA + DBM_Q_GET_USER_EMLOCALES) // 2824  //46[s|4444] reqId errCode[errDescr|locale, emLocale, emLocale2, emLocale3] 

#define DBM_Q_US_MIGRATION_READ_IOM_USER                            1825
#define DBM_A_US_MIGRATION_READ_IOM_USER                            (DBM_DELTA + DBM_Q_US_MIGRATION_READ_IOM_USER) // 2825

#define DBM_Q_SAVE_IMAGE                                            1826 	//4B reqId varBlock
#define DBM_A_SAVE_IMAGE                                            (DBM_DELTA + DBM_Q_SAVE_IMAGE) // 2826 	//46[s|4]reqId err_code[ errDescr|imageId ] 

//#define DBM_Q_AVAILABLE_MESSAGE                                             1827     //41s44(T)sssss84 reqId awareOfRestriction userId receiverIntId amount received wuCtrlNum crCtrlNum crWuOperator comment cashtrStatus cashtrFlagsToRaise wuOfficeId

//#define DBM_Q_AVAILABLE_MESSAGE							1827  //4s4ssb4 reqId, adminId, adminIntId, userId, comment, keepPrivileges, negativeCategory

#define DBM_Q_TCHIPS_CREDIT                                         1828 	// 4s4747ss4 reqId, userId, tChipsBefore, amountTChips, wChipsBefore, amountWChips, comment, accountCurrency, budgetCodeId
#define DBM_A_TCHIPS_CREDIT                                         (DBM_DELTA + DBM_Q_TCHIPS_CREDIT) // 2828 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_TCHIPS_TRANSFER                                       1829 	// 4s4s441s1s reqId, userFrom, userFromWChips, userTo, wChipsTransferAmount, money, awareOfBanOrClosed, comment, isTChips, currencyFrom
#define DBM_A_TCHIPS_TRANSFER                                       (DBM_DELTA + DBM_Q_TCHIPS_TRANSFER) // 2829 	//46[s|ss] reqId, err_code[errDescr|userFromEmail, userToEmail] 

#define DBM_Q_MAIL_BOUNCED                                          1830 	// 4ss reqId, email, reason
#define DBM_A_MAIL_BOUNCED                                          (DBM_DELTA + DBM_Q_MAIL_BOUNCED) // 2830 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ADMIN_FORCE_USER_LOGOUT                               1831 	// 4s reqId, userId
#define DBM_A_ADMIN_FORCE_USER_LOGOUT                               (DBM_DELTA + DBM_Q_ADMIN_FORCE_USER_LOGOUT) // 2831 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_SET_CONVERSION_RATE                                   1832 	// 4s4 reqId, currencyCode, newRate
#define DBM_A_SET_CONVERSION_RATE                                   (DBM_DELTA + DBM_Q_SET_CONVERSION_RATE) // 2832 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_EXPORT_ZIP_HIST                                       1833 	// 4444 reqId, fromHandId, toHandId, by
#define DBM_A_EXPORT_ZIP_HIST                                       (DBM_DELTA + DBM_Q_EXPORT_ZIP_HIST) // 2833 	//46[s] reqId, err_code[errDescr] 

//	PYR-58575. LoginDbm -> Dbm
#define DBM_Q_CLOSE_ARCHIVED_ACCOUNT								1834	//	4M488484tsb4b - reqId,M(user),oldPriv,oldPriv2,oldPriv3,oldFlags,oldFlags3,adminIntId,when,comment,removePrivs,negativeCat,saveUserPropAudit
#define DBM_A_CLOSE_ARCHIVED_ACCOUNT								(DBM_DELTA + DBM_Q_CLOSE_ARCHIVED_ACCOUNT)	//	2834	//	46[s] - reqId, err_code[errDescr]

#define DBM_Q_VERIFY_REAL_TRANSFER                                  1835 	// 4ss4 reqId, userFrom, userTo, amount
#define DBM_A_VERIFY_REAL_TRANSFER                                  (DBM_DELTA + DBM_Q_VERIFY_REAL_TRANSFER) // 2835 	//46[s|ss4] reqId, err_code [errDescr | emailFrom, emailTo, transId] 

// USNJ dbm -> IOM dbm
#define DBM_Q_INSERT_EXTERNAL_USER									1836
#define DBM_A_INSERT_EXTERNAL_USER                                  (DBM_DELTA + DBM_Q_INSERT_EXTERNAL_USER) // 2836

//from admin
#define DBM_Q_ADD_AFFILIATE                                         1837 	// 44s4 reqId accountId promoName qualPropId
#define DBM_A_ADD_AFFILIATE                                         (DBM_DELTA + DBM_Q_ADD_AFFILIATE) // 2837 	//46[s] reqId, err_code[errDescr] 

//#define DBM_Q_AVAILABLE_MESSAGE									1838

#define DBM_Q_PREVERIFY_REAL_TRANSFER                               1839     // 4ss4s reqId, userFrom, userTo, amountFrom, currencyFrom
#define DBM_A_PREVERIFY_REAL_TRANSFER                               (DBM_DELTA + DBM_Q_PREVERIFY_REAL_TRANSFER) // 2839     //46[s|ssss4] reqId, err_code [errDescr | userToRealUserId userToCity userToState userToCountry licenseId] 

#define DBM_Q_CLEAR_CACHE                                           1840 	//4 reqId
#define DBM_A_CLEAR_CACHE                                           (DBM_DELTA + DBM_Q_CLEAR_CACHE) // 2840 	//46[s] reqId, err_code[errDescr] 

//PYR-25814 table <--> DRC
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_DRC_RH_USER_OUT										1841	//4{rec} - reqId,{struct RatholeManTableData}
#define DBM_A_DRC_RH_USER_OUT										(DBM_DELTA + DBM_Q_DRC_RH_USER_OUT)	// 2841 	//46 - reqId,errCode=0

//tableDBM <--> DRC
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_DRC_CHECK_USER_BEFORE_SITTING							1842	//4sbb4{[rec]}4 - reqId,userId,isPlayMoney,isOneOnOne,tableType,{[struct RatholeManTableData]},maxTablesAllowed
#define DBM_A_DRC_CHECK_USER_BEFORE_SITTING							(DBM_DELTA + DBM_Q_DRC_CHECK_USER_BEFORE_SITTING)	// 2842  //46[s|77774] - reqId,errCode,[errDescr|count, countHU, countHTML5, countBlitz, rhObl]

#define DBM_Q_CREATE_REPLAY                                         1843 	//4TssssB8 reqId startTime, name, description, filename, adminId, script, tournId
#define DBM_A_CREATE_REPLAY                                         (DBM_DELTA + DBM_Q_CREATE_REPLAY) // 2843 	//46[s|4] reqId, err_code[errDescr|replayId] 

#define DBM_Q_UPDATE_REPLAY_STATUS                                  1844 	//441 reqId replayId status
#define DBM_A_UPDATE_REPLAY_STATUS                                  (DBM_DELTA + DBM_Q_UPDATE_REPLAY_STATUS) // 2844 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_FINALIZE_CCOIN                                        1845 	//44<4> reqId, numTrans<transId>
#define DBM_A_FINALIZE_CCOIN                                        (DBM_DELTA + DBM_Q_FINALIZE_CCOIN) // 2845 	//44<Trn> reqId, numTrans<transaction> 

#define DBM_Q_OVERCREDIT_COMPLETE                                   1846 	//441ss reqId, transId, status, xTransStr, errInfo
#define DBM_A_OVERCREDIT_COMPLETE                                   (DBM_DELTA + DBM_Q_OVERCREDIT_COMPLETE) // 2846 	//46[s] reqId, err_code[errDescr] ?? 

// PYR-26954
#define DBM_Q_USER_PLATFORM_CHANGED									1847	//4s48 reqId, userId, platformMask, appSessionId
#define DBM_A_USER_PLATFORM_CHANGED                                 (DBM_DELTA + DBM_Q_USER_PLATFORM_CHANGED) // 2847 	//46[s] reqId, err_code[errDescr]

// #define DBM_Q_AVAILABLE_MESSAGE                                    1848 	//4(D) reqId, date
 
// #define DBM_Q_AVAILABLE_MESSAGE                                     1849 	//44<4> reqId, numRecords<auditId>

#define DBM_Q_MANAGE_REPORTRIGHTS                                   1850 	//41ss reqId type reportName rightName 
#define DBM_A_MANAGE_REPORTRIGHTS                                   (DBM_DELTA + DBM_Q_MANAGE_REPORTRIGHTS) // 2850 	//46[s] reqId, err_code[errDescr] 

					///type: 1 = add; 2 = delete, see reportManageType* constants below

#define DBM_Q_REFRESH_FAKEMACS                                      1851 	//4 reqId
#define DBM_A_REFRESH_FAKEMACS                                      (DBM_DELTA + DBM_Q_REFRESH_FAKEMACS) // 2851 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ACTIVE_JUBILEE_HAND                                   1852 	//444 reqId jubileeHandIdOrNumber siteId
#define DBM_A_ACTIVE_JUBILEE_HAND                                   (DBM_DELTA + DBM_Q_ACTIVE_JUBILEE_HAND) // 2852 	//46[s8|1[8s11111444s8|8]] reqId err_code [ errDescr realMilestoneHandId | jubileeHandGiven [tableId tableName game structure hiLo isPlay isOneOnOne smallBlind loBet hiBet currency lastHandId | lastHandIdOrCounter]] 

// from cahier
#define DBM_Q_SET_UNKNOWN_STATUS                                    1853 	//4<4> numTransId<auditId>
#define DBM_A_SET_UNKNOWN_STATUS                                    (DBM_DELTA + DBM_Q_SET_UNKNOWN_STATUS) // 2853 	//46[s] reqId, err_code[errDescr]  

//#define DBM_Q_AVAILABLE_MESSAGE
// PYR-25597 - obsolete PYR-113567
//#define DBM_Q_GET_OFFLINE_HAND_IDS									1854	//4s4tt - reqId, userId, timeFrom, timeTo
//#define DBM_A_GET_OFFLINE_HAND_IDS                                  (DBM_DELTA + DBM_Q_GET_OFFLINE_HAND_IDS) // 2854	//46[s|4<8>] reqId, err_code[errDescr|numHands<handId64>]

//#define DBM_Q_AVAILABLE_MESSAGE
// PYR-25597 - obsolete
//#define DBM_Q_GET_OFFLINE_HAND_IDS									1854	//4s4tt - reqId, userId, timeFrom, timeTo
//#define DBM_A_GET_OFFLINE_HAND_IDS                                  (DBM_DELTA + DBM_Q_GET_OFFLINE_HAND_IDS) // 2854	//46[s|4<8>] reqId, err_code[errDescr|numHands<handId64>]

// PYR-24890
// login dbm -> sec server
#define DBM_Q_SEC_LOGIN_FAILED										1856	//4s444b - reqId, installId, userid, licenseId, logoutTypeOrError, isFrozen
#define DBM_A_SEC_LOGIN_FAILED										(DBM_DELTA + DBM_Q_SEC_LOGIN_FAILED) // 2856	//46[s] reqId, err_code[errDescr]

// sec server -> login dbm
#define DBM_Q_SEC_LOCKOUT_INSTALLID									1857	//4bs4s - reqId, lock, installId, lockType, comment
#define DBM_A_SEC_LOCKOUT_INSTALLID									(DBM_DELTA + DBM_Q_SEC_LOCKOUT_INSTALLID) // 2857	//46[s] reqId, err_code[errDescr]

#define DBM_Q_SAVE_NOGW_CASHTRANS                                   1858 	// see format of DBM_Q_CASHIN_CC_INIT
#define DBM_A_SAVE_NOGW_CASHTRANS                                   (DBM_DELTA + DBM_Q_SAVE_NOGW_CASHTRANS) // 2858 	//46[s] reqId, err_Code [errDescr] 

// #22082
#define DBM_Q_REGISTER_ADMIN_LOGIN                                  1859 // 4s6Tb8 - reqId, adminId, remoteIpAddr, result, loginTime, isLogin, sessionId
#define DBM_A_REGISTER_ADMIN_LOGIN                                  (DBM_DELTA + DBM_Q_REGISTER_ADMIN_LOGIN) // 2859 // 46[s|sb8b] - reqId, errCode[errDescr|sessionId, isRevoked] 

//	PYR-58575. LoginDbm->Dbm
#define DBM_Q_INSERT_USERCHANGES_ARCH_USER							1860	//	4s4444884t - reqId,userId,userIntId,licenseId,updateField,updateType,oldValue,newValue,adminIntId,when
#define DBM_A_INSERT_USERCHANGES_ARCH_USER							(DBM_DELTA + DBM_Q_INSERT_USERCHANGES_ARCH_USER)	//	2860	// 46[s] reqId, err_Code [errDescr]

//#define DBM_Q_AVAILABLE_MESSAGE                                     1861 	// 44s11 reId recvrIntId comment office terminate

#define DBM_Q_CANCEL_RUNNING_TOURN                                  1862    // 44441M([4<44>])M(b[O])44 - reqId,tournId,compensationFormula,knockout,award_currency_type,M([numAwards<awardAmount nextTournId>]),M(isBOP[bopTable]),flightTournFlags,scalePM
#define DBM_A_CANCEL_RUNNING_TOURN                                  (DBM_DELTA + DBM_Q_CANCEL_RUNNING_TOURN) // 2862    // 46[s|44444<s4444444>] - reqId, err_Code[errDescr|formula,totalChipsCount,equalShareSize,remainingAwardPool(0 if full rollback),remainingNumUsers<userId,money,play,fpp,tChips,tournChipsStanding,place,placeTo>] 

#define DBM_Q_ADMIN_LOGIN                                           1863 	//4s reqId, remoteIpAddr
#define DBM_A_ADMIN_LOGIN                                           (DBM_DELTA + DBM_Q_ADMIN_LOGIN) // 2863 	//46[s|2<s>s] reqId err_code[errDescr|numRighs<rightName>, tokenStr]
//PYR-25970
#define DBM_Q_CREATE_TGM											1864	//	4o - reqId, CreateTgmRequest
#define DBM_A_CREATE_TGM											(DBM_DELTA + DBM_Q_CREATE_TGM) //2864 //46[s|4] - reqId, errCode [errMsg|tgmId]
//PYR-25970
#define DBM_Q_GET_TGMS												1865	//44b reqId, type, activeOnly
#define DBM_A_GET_TGMS												(DBM_DELTA + DBM_Q_GET_TGMS) //2865 //46[s| 4<44stt4st>] - reqId, errCode [errMsg| num<tgmId, type, groupName, start, end,maxEntry, adminId, created>]

#define DBM_Q_INSERT_CHARGEBACK                                     1866 	//44Ds442224s[s] reqId, transId, postDate relatedUser, caseId, amount, reason , status, state, controlflags, xtransstr, reasonStr
#define DBM_A_INSERT_CHARGEBACK                                     (DBM_DELTA + DBM_Q_INSERT_CHARGEBACK) // 2866 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_UPDATE_CHARGEBACK                                     1867 	//44s4222d4b reqId, cargeBackId, newRelatedUser, newCaseId, newReason , newStatus, newState, postedDate, reversalAmount, isPM
#define DBM_A_UPDATE_CHARGEBACK                                     (DBM_DELTA + DBM_Q_UPDATE_CHARGEBACK) // 2867 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ADMIN_REVOKE_CHAT                                     1868 	//4s4s1ssT reqId, adminUserId, userId, bitFlags(see RevokeChatFlags enum), strComment, strMessage, timeUntil
#define DBM_A_ADMIN_REVOKE_CHAT                                     (DBM_DELTA + DBM_Q_ADMIN_REVOKE_CHAT) // 2868 	//46[s|sss4488>] reqId, err_code, [errr_descr | userId, userId, email, strMessage, flags, priv, priv2, priv3]

//#define DBM_Q_AVAILABLE_MESSAGE                                     1869 	//TBD

#define DBM_Q_AFFILIATE_REFERRALS                                   1870 	// 4sTT reqId, affilUserId, srvTimeFrom srvTimeTo
#define DBM_A_AFFILIATE_REFERRALS                                   (DBM_DELTA + DBM_Q_AFFILIATE_REFERRALS) // 2870 	// 46[s|4<s444>] reqId, err_code, [errr_descr | numPromos<promoName totalRegistered realMoney, qualified (>= 100 LT_FPP)> 
//PYR-25970
#define DBM_Q_UPDATE_TGM											1871	// 4o reqId, UpdateTgmRequest
#define DBM_A_UPDATE_TGM											(DBM_DELTA + DBM_Q_UPDATE_TGM) //2871	//46[s] - reqId, errCode [errMsg]

#define DBM_Q_USER_QUESTIONNAIRE                                    1872 	//4s4s reqId, userId, questInt, questStr
#define DBM_A_USER_QUESTIONNAIRE                                    (DBM_DELTA + DBM_Q_USER_QUESTIONNAIRE) // 2872 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ADD_AFFILIATE_PAYMENTS                                1873     // 44<4s4<44>> reqId numPayments<accountId, currency, numPayItems<paymentItemId amount>>
#define DBM_A_ADD_AFFILIATE_PAYMENTS                                (DBM_DELTA + DBM_Q_ADD_AFFILIATE_PAYMENTS) // 2873     //46[s|4<46s4>] reqId, err_code[errDescr|numAccounts<accountId, errCode, errDescr, paymentTransId>] 

#define DBM_Q_DELETE_AFFILIATE                                      1874 	//44s reqId accountId promoName 
#define DBM_A_DELETE_AFFILIATE                                      (DBM_DELTA + DBM_Q_DELETE_AFFILIATE) // 2874 	//46[s] reqId, err_code[errDescr] 
//PYR-25970
#define DBM_Q_DELETE_TGM											1875	//444 reqId, tgmId, type
#define DBM_A_DELETE_TGM											(DBM_DELTA + DBM_Q_DELETE_TGM) //2875	//46[s] - reqId, errCode [errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_EXECUTE_USER_STMT                                     1876     //
#define DBM_A_EXECUTE_USER_STMT                                     (DBM_DELTA + DBM_Q_EXECUTE_USER_STMT) // 2876     //46[s] reqId, err_code[errDescr] 

#define DBM_Q_CACHE_USERS                                           1877 	//44<4> reqId numUsers<userIntId>
#define DBM_A_CACHE_USERS                                           (DBM_DELTA + DBM_Q_CACHE_USERS) // 2877 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_SAVE_RECENT_CACHE                                     1878 	//44 reqId ticksLimit
#define DBM_A_SAVE_RECENT_CACHE                                     (DBM_DELTA + DBM_Q_SAVE_RECENT_CACHE) // 2878 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_GET_SAVED_CACHE                                       1879 	//4 reqId
#define DBM_A_GET_SAVED_CACHE                                       (DBM_DELTA + DBM_Q_GET_SAVED_CACHE) // 2879 	//46[s|4<4>] reqId, err_code[errDescr|numIds<intId>] 

#define DBM_Q_DELETE_CHARGEBACK                                     1880 	//44 reqId cbackId
#define DBM_A_DELETE_CHARGEBACK                                     (DBM_DELTA + DBM_Q_DELETE_CHARGEBACK) // 2880 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_TOURN_DEAL                                            1881 	//444<s4> reqId, tournId numUsers<userId newAward>
#define DBM_A_TOURN_DEAL                                            (DBM_DELTA + DBM_Q_TOURN_DEAL) // 2881 	//46[s|4<s6s>] reqId, err_code[errDescr|numUsers<userId err_code err_descr>] 

#define DBM_Q_AFFPROPS                                              1882 	//4<script> reqId, AffCategoryScript
#define DBM_A_AFFPROPS                                              (DBM_DELTA + DBM_Q_AFFPROPS) // 2882 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_CREATE_AFFACCOUNT                                     1883 	//4ss<4s4>4 reqId, userId, payprop, <numPromo, promoName, qualPropId>, status
#define DBM_A_CREATE_AFFACCOUNT                                     (DBM_DELTA + DBM_Q_CREATE_AFFACCOUNT) // 2883 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_UPDATE_AFFACCOUNT                                     1884 	//44s4 reqId, AccountId, payprop, status
#define DBM_A_UPDATE_AFFACCOUNT                                     (DBM_DELTA + DBM_Q_UPDATE_AFFACCOUNT) // 2884 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_CREDIT_USERS                                          1885   //44<s46>sss44 reqId numusers<userId amount amountCents>comment currency, paysystem, auditTransType, budgetCodeId
#define DBM_A_CREDIT_USERS                                          (DBM_DELTA + DBM_Q_CREDIT_USERS) // 2885     //46[s|4<s6s>] reqId err_code [errDescr|numusers<userId err_code errDescr>] 

#define DBM_Q_CREDIT_USERS_FPP                                      1886   //44<s46>s4 reqId numusers<userId amount amountCents>comment budgetCodeId
#define DBM_A_CREDIT_USERS_FPP                                      (DBM_DELTA + DBM_Q_CREDIT_USERS_FPP) // 2886 	// see 2885 

#define DBM_Q_REDEEMS_UNKNOWN                                       1887 	//see DBM_Q_PURCHASES_UNKNOWN
#define DBM_A_REDEEMS_UNKNOWN                                       (DBM_DELTA + DBM_Q_REDEEMS_UNKNOWN) // 2887 	//see DBM_A_PURCHASES_UNKNOWN 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_REMOVE_FROM_CACHE                                     1888 	//4s reqId userId
#define DBM_A_REMOVE_FROM_CACHE                                     (DBM_DELTA + DBM_Q_REMOVE_FROM_CACHE) // 2888 	//46[s] reqId, err_code[errDescr] 

//	PYR-27579, CRM Server to Olap
#define DBM_Q_RESPONSIBLE_GAMING_AUDIT								1889	// 4sTT - reqId, userId, from, to
#define DBM_A_RESPONSIBLE_GAMING_AUDIT								(DBM_DELTA + DBM_Q_RESPONSIBLE_GAMING_AUDIT)	//	2889

#define DBM_Q_USER_DONT_SHOW_WHERE                                  1890 	//4sb reqId userId set_not_remove
#define DBM_A_USER_DONT_SHOW_WHERE                                  (DBM_DELTA + DBM_Q_USER_DONT_SHOW_WHERE) // 2890 	//46[s|s44] reqId, err_code[errDescr|userId new_privs flags] 

#define DBM_Q_ASSIGN_USER_PROMO                                     1891 	//4ss reqId userId promoName
#define DBM_A_ASSIGN_USER_PROMO                                     (DBM_DELTA + DBM_Q_ASSIGN_USER_PROMO) // 2891 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ASSIGN_USER_SID                                       1892 	//4ss8 reqId userId securId appLoginId
#define DBM_A_ASSIGN_USER_SID                                       (DBM_DELTA + DBM_Q_ASSIGN_USER_SID) // 2892 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_CLOSED_TABLES_ORD_DATE                                1893     //14 (D) date
#define DBM_A_CLOSED_TABLES_ORD_DATE                                (DBM_DELTA + DBM_Q_CLOSED_TABLES_ORD_DATE) // 2893     // 46[s|4<844>] errCode [errDescr|numTables<tableId,minordinal,maxordinal>] 

#define DBM_Q_PROCESS_CHAT_CLOSED_TABLES                            1894     //44<844> reqId, numTables<tableId,minOrdinal,maxOrdinal>
#define DBM_A_PROCESS_CHAT_CLOSED_TABLES                            (DBM_DELTA + DBM_Q_PROCESS_CHAT_CLOSED_TABLES) // 2894    // 46[s|4] reqId, err_code[errDescr|rowsDeleted] 

#define DBM_Q_REMIND_SID                                            1895 	//4s reqId userId
#define DBM_A_REMIND_SID                                            (DBM_DELTA + DBM_Q_REMIND_SID) // 2895 	//46[s|bss4s448] reqId err_code [errDescr|viaSMS[mobile|email],sid,locale,country,userIntId,flags,flags3] 

#define DBM_Q_US_MIGRATION_CLOSE_IOM_USER                           1896
#define DBM_A_US_MIGRATION_CLOSE_IOM_USER                           (DBM_DELTA + DBM_Q_US_MIGRATION_CLOSE_IOM_USER) // 2896

#define DBM_QS_US_MIGRATION_INSERT_US_USER                          1897
#define DBM_AS_US_MIGRATION_INSERT_US_USER                          (DBM_DELTA + DBM_QS_US_MIGRATION_INSERT_US_USER) // 2897

#define DBM_Q_GET_LAST_TIME_RMOK_FLAG_SET							1898 //#24151 44 reqId userIntId
#define DBM_A_GET_LAST_TIME_RMOK_FLAG_SET							(DBM_DELTA + DBM_Q_GET_LAST_TIME_RMOK_FLAG_SET) // 46[s|t]

#define DBM_Q_RECALC_PREV_TLB                                       1899 	//4 reqId
#define DBM_A_RECALC_PREV_TLB                                       (DBM_DELTA + DBM_Q_RECALC_PREV_TLB) // 2899 	// 46[s] reqId, err_code[errDescr] 

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_DAD_TRANSACT_ASSIGN                                 1900 	//4 reqIds userId 
//#define DBM_A_DAD_TRANSACT_ASSIGN                                 (DBM_DELTA + DBM_Q_DAD_TRANSACT_ASSIGN) // 2900 	// 46[s] reqId, err_code[errDescr] 

// obsolete
// DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_DAD_TRANSACT_CLOSE                                  1901 	//4 reqIdsss4 userId,comment, dadpaysystem, connecttype 
//#define DBM_A_DAD_TRANSACT_CLOSE                                  (DBM_DELTA + DBM_Q_DAD_TRANSACT_CLOSE) // 2901 	// 46[s] reqId, err_code[errDescr] 

//#define DBM_Q_AVAILABLE_MESSAGE                                   1902 	//4s reqId userId

//#define DBM_Q_AVAILABLE_MESSAGE                                   1903 	//4s	

//#define DBM_Q_AVAILABLE_MESSAGE                                   1904 	//4ssss4s4 reqId, userId, MTCN, userAmount, receiverName, officeId, bonusCode, wuGatewayId

//#define DBM_Q_AVAILABLE_MESSAGE                                   1905 	//44444sssss84 reqId wuTransId newStatus amount receiverIntId wuCtrlNum crCtrlNum crWuOperator comment cashtrStatus cashtrFlagsToRaise wuOfficeId

#define DBM_Q_USER_ADD_CHECK                                        1906  // same as DBM_QS_USER_ADD - should be processed in RW dbm - logs the failure
#define DBM_A_USER_ADD_CHECK                                        (DBM_DELTA + DBM_Q_USER_ADD_CHECK) // 2906

#define DBM_Q_HAND_COLLUSION                                        1907 	//C->D, 42<21<s>>" handId,collusions_size<code,players_size<userId>>
#define DBM_A_HAND_COLLUSION                                        (DBM_DELTA + DBM_Q_HAND_COLLUSION) // 2907 	//D->C, "46{s}" handId,err_code{err_dscp}	 

//#define DBM_Q_AVAILABLE_MESSAGE									1908

//#define DBM_Q_AVAILABLE_MESSAGE                                   1909

//#define DBM_Q_AVAILABLE_MESSAGE                            1910 	//444444 reqId groupId minDep maxDay maxWeek maxMonth

//#define DBM_Q_AVAILABLE_MESSAGE                                      1911 	//44s44441<s> reqId categoryId, name minDep maxDay maxWeek maxMonth numSystems<paySystem>

//#define DBM_Q_AVAILABLE_MESSAGE                            1912 	//441<s> reqId groupId numSystems<paysystem>

//#define DBM_Q_AVAILABLE_MESSAGE                                   1913 	//44 reqId groupId

//#define DBM_Q_AVAILABLE_MESSAGE                                   1914 	//4ss4444 reqId name dispName minDep maxDay maxWeek maxMonth

//#define DBM_Q_AVAILABLE_MESSAGE                                  1915 	//44 reqId catId dispName minDep maxDay maxWeek maxMonth

#define DBM_Q_CREDIT_USERS_FPP_LT                                   1916 	//see 1886
#define DBM_A_CREDIT_USERS_FPP_LT                                   (DBM_DELTA + DBM_Q_CREDIT_USERS_FPP_LT) // 2916 	//see 2886

#define DBM_Q_REFRESH_PROMOTIONS                                    1917     // 4  reqId 
#define DBM_A_REFRESH_PROMOTIONS                                    (DBM_DELTA + DBM_Q_REFRESH_PROMOTIONS) // 2917     // 4  reqId  

// 1918, 1919 moved to reportdbm.h

#define DBM_Q_UPDATE_TICKET_STATUS                                  1920 	//44bool reqId, ticketID, setUsed 
#define DBM_A_UPDATE_TICKET_STATUS                                  (DBM_DELTA + DBM_Q_UPDATE_TICKET_STATUS) // 2920 	// 46[s] reqId, err_code[errDescr] 

// PYR-27418
#define DBM_Q_GET_GENERATOR_ID_RANGES								1921	//47<s7> - reqId, num<objectName,rangeSize>
#define DBM_A_GET_GENERATOR_ID_RANGES								(DBM_DELTA + DBM_Q_GET_GENERATOR_ID_RANGES) // 2921 //46[s|7<s88>] reqId, errCode[errDescr|numRanges<objectName,startId,endId>]

// PYR-102906 deprecated
#define DBM_Q_UPDATE_MASKED_CARDNUMS                                1922 	//44<4s> reqId numTrans<transId cardNum>
#define DBM_A_UPDATE_MASKED_CARDNUMS                                (DBM_DELTA + DBM_Q_UPDATE_MASKED_CARDNUMS) // 2922 	//44<4s> reqId numTrans<transId cardNum> 

//tableDBM <--> DRC PYR-25814
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_DRC_USER_CHIPS_QUERY                                  1923  //4s18s4bM(4S8B44444)M(s48444) - reqId,userId,isPlay,allocatedPM,currency,minBuyIn,needRelated,{struct RatholeManTableData},{struct PoolBlockTableData} - each message body can be empty
#define DBM_A_DRC_USER_CHIPS_QUERY                                  (DBM_DELTA + DBM_Q_DRC_USER_CHIPS_QUERY) // 2923  //46[sb|41484bMM4<s>4f] - reqId,errCode,[errDescr,[enoughOnAllAccounts|maxTables]|chips,playAddedMask,playAddedAmount,playChips64,userRollId,enoughOnAllAccounts,M(currencyContext),M(currencyContextCli),numGroups,<group>,rhManMinBuyin,eligibleForPoolBlockPenalty] //see PLAYADDMASK_* constants 

#define DBM_Q_ADD_ADMIN_MONEY_LIMITS                                1924 	//44s44  reqId catName maxTransfer maxCredit rank
#define DBM_A_ADD_ADMIN_MONEY_LIMITS                                (DBM_DELTA + DBM_Q_ADD_ADMIN_MONEY_LIMITS) // 2924 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_EDIT_ADMIN_MONEY_LIMITS                               1925 	//44s444 reqId catId catName maxTransfer maxCredit rank
#define DBM_A_EDIT_ADMIN_MONEY_LIMITS                               (DBM_DELTA + DBM_Q_EDIT_ADMIN_MONEY_LIMITS) // 2925 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_DELETE_ADMIN_MONEY_LIMITS                             1926 	//44	reqId catId 
#define DBM_A_DELETE_ADMIN_MONEY_LIMITS                             (DBM_DELTA + DBM_Q_DELETE_ADMIN_MONEY_LIMITS) // 2926 	//46[s] reqId, err_code[errDescr]  

// PYR-84874 NOTE: if you modify contents of the message in the part before userId please change AdminBrokerProtocols in srvshared also
#define DBM_Q_ADD_ADMIN_MONEY_RIGHTS                                1927 	//4s4 reqId userId catId
#define DBM_A_ADD_ADMIN_MONEY_RIGHTS                                (DBM_DELTA + DBM_Q_ADD_ADMIN_MONEY_RIGHTS) // 2927 	//46[s] reqId, err_code[errDescr] 

// PYR-84874 NOTE: if you modify contents of the message in the part before userId please change AdminBrokerProtocols in srvshared also
#define DBM_Q_EDIT_ADMIN_MONEY_RIGHTS                               1928 	//4s4 reqId userId catId
#define DBM_A_EDIT_ADMIN_MONEY_RIGHTS                               (DBM_DELTA + DBM_Q_EDIT_ADMIN_MONEY_RIGHTS) // 2928 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_DELETE_ADMIN_MONEY_RIGHTS                             1929 	//4s4 reqId userId 
#define DBM_A_DELETE_ADMIN_MONEY_RIGHTS                             (DBM_DELTA + DBM_Q_DELETE_ADMIN_MONEY_RIGHTS) // 2929 	//46[s] reqId, err_code[errDescr]  

// #artakk #TODO needs further discussion in order to make this generic for US
#define DBM_QS_DELETE_NJ_USER_FROM_IOM								1930 	//
#define DBM_AS_DELETE_NJ_USER_FROM_IOM								(DBM_DELTA + DBM_QS_DELETE_NJ_USER_FROM_IOM) // 2930

#define DBM_Q_TCHIPS_SALE                                           1931 	//4ss1444s reqId seller buyer isBuyer tAmt, wAmt, amt, currency
#define DBM_A_TCHIPS_SALE                                           (DBM_DELTA + DBM_Q_TCHIPS_SALE) // 2931 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_USER_ROLLBACKS                                        1932 	//4s4s1 reqId, userId, maxCashoutAmount, cashoutBalanceCurrency, userRollbackOption
#define DBM_A_USER_ROLLBACKS                                        (DBM_DELTA + DBM_Q_USER_ROLLBACKS) // 2932 	//46[s|4<4ssT4s4>2<ss4s4>] recId errorcode [errDescr|numRollbacks<purchaseId, gateway, paysystem, completed, amount, currency, converted> numCardstoRollback<cardNum paySystem filteredAmount, currency, filteredConverted>] 

#define DBM_Q_GET_USER_PENDING_DEPOSIT_LIMITS                       1933  // 44 reqId, userIntId
#define DBM_A_GET_USER_PENDING_DEPOSIT_LIMITS                       (DBM_DELTA + DBM_Q_GET_USER_PENDING_DEPOSIT_LIMITS) // 2933  2[s|4<4444>] errorcode[errDescr|numLimits<limitType, limitId, oldValue, newValue>]

#define DBM_Q_PRE_TSALE                                             1934 	//4s reqId userId
#define DBM_A_PRE_TSALE                                             (DBM_DELTA + DBM_Q_PRE_TSALE) // 2934 	//46[s|sss4] reqId, err_code[errDescr|realUserId userCity userCountry userLicense] 

#define DBM_Q_PRECANCEL_TSALES                                      1935 	//4s reqId userId
#define DBM_A_PRECANCEL_TSALES                                      (DBM_DELTA + DBM_Q_PRECANCEL_TSALES) // 2935 	//46 [s|44<{s4441Ts}>] reqId err_code[errDescr| Zero Count <MsgBody{otherUser T$ W$ money isStartedByBuyer validUntil currency}>] 

#define DBM_Q_CANCEL_TSALE_ID                                       1936 	//4s4<4> reqId userId numSales<saleId>
#define DBM_A_CANCEL_TSALE_ID                                       (DBM_DELTA + DBM_Q_CANCEL_TSALE_ID) // 2936 	//46[s|4<6[s|s4441]>] reqId, err_code[errDescr | numSales <errCode[errDescr|tChips wChips money isBuyer]>] 

#define DBM_Q_LOGIN_IDS_DATE                                        1937 	//4(D) reqId, date
#define DBM_A_LOGIN_IDS_DATE                                        (DBM_DELTA + DBM_Q_LOGIN_IDS_DATE) // 2937 	//44<4> reqId, numIds<loginId> 

#define DBM_Q_CROP_LOGGEDOUT                                        1938 	//44<4> reqId, numRecords<loginidId>
#define DBM_A_CROP_LOGGEDOUT                                        (DBM_DELTA + DBM_Q_CROP_LOGGEDOUT) // 2938 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_CLEAR_USER_PROMO                                      1939 	//4ss reqId userId promoName
#define DBM_A_CLEAR_USER_PROMO                                      (DBM_DELTA + DBM_Q_CLEAR_USER_PROMO) // 2939 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_CLOSED_TOURNS_DATE                                    1940 	//4(D)4 reqId, date, eTournsByDateQueryType
#define DBM_A_CLOSED_TOURNS_DATE                                    (DBM_DELTA + DBM_Q_CLOSED_TOURNS_DATE) // 2940 	//44<4<4>> reqId, numTourns, <numIds,<userIntId>> 

#define DBM_Q_PROCESS_CLOSED_TOURNS                                 1941 	//444<4> reqId, tournId, numRecords<UserIntId>
#define DBM_A_PROCESS_CLOSED_TOURNS                                 (DBM_DELTA + DBM_Q_PROCESS_CLOSED_TOURNS) // 2941 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_GET_VIPSTATUS_INFO                                    1942 	// 4Os8 - reqId, MessageInfoStruct, userid, appLoginId
#define DBM_A_GET_VIPSTATUS_INFO                                    (DBM_DELTA + DBM_Q_GET_VIPSTATUS_INFO) // 2942 	//446[s|4D444D444D664<444>4<4444>4[4,4,8]] reqId, err_code[errDescr|currentStatus validUntil monthly.counter monthly.fppToNext monthly.nextStatus monthly.timeToEarn yearly.counter yearly.fppToNext yearly.nextStatus yearly.timeToEarn monthly.counterCents yearly.counterCents numBonusLevels<groupId vppThreshold amount> numVipLevels<statusId minFpp fppBonusRate vipToKeepYearlyStatus> lastMonthForYearly [previousYearVPP, dailyVpp, ltVppCents64]] 

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SET_VIPSTATUS_INFO                                    1943 	//4s4444446666b reqId userid oldstatus oldcounter1 oldcounter2 status counter1 counter2 oldCounter1Cents oldCounter2Cents counter1Cents counter2Cents keepVipStatusForNextMonth
#define DBM_A_SET_VIPSTATUS_INFO                                    (DBM_DELTA + DBM_Q_SET_VIPSTATUS_INFO) // 2943 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ADD_CANNED_EMAIL                                      1944 	//4ss44 reqId emailTitle emailText emailType locale
#define DBM_A_ADD_CANNED_EMAIL                                      (DBM_DELTA + DBM_Q_ADD_CANNED_EMAIL) // 2944 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_EDIT_CANNNED_EMAIL                                    1945 	//44s4 reqId emailId newEmailText locale
#define DBM_A_EDIT_CANNNED_EMAIL                                    (DBM_DELTA + DBM_Q_EDIT_CANNNED_EMAIL) // 2945 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_GET_CANNED_EMAIL                                      1946 	//444 reqId, emailId, locale
#define DBM_A_GET_CANNED_EMAIL                                      (DBM_DELTA + DBM_Q_GET_CANNED_EMAIL) // 2946 	//46[s|s] reqId, err_code[errDescr|emailBodyText] 

#define DBM_Q_REINSTATE_CHAT                                        1947 	//4ss reqId userId adminId
#define DBM_A_REINSTATE_CHAT                                        (DBM_DELTA + DBM_Q_REINSTATE_CHAT) // 2947 	//46[s|s4488] reqId, err_code[errDescr|userId, flags, privs, privs2, privs3] 

#define DBM_Q_GET_SAVED_CARD                                        1948 	//4ss reqId userId paySystem
#define DBM_A_GET_SAVED_CARD                                        (DBM_DELTA + DBM_Q_GET_SAVED_CARD) // 2948 	//46[s|ss(...)] reqId, err_code[errDescr|userId paySystem (SavedCardInfo)]  

#define DBM_Q_SAVE_CARDINFO                                         1949 	//4(...)8 reqId(cardInfo) appLoginId
#define DBM_A_SAVE_CARDINFO                                         (DBM_DELTA + DBM_Q_SAVE_CARDINFO) // 2949 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_GET_USER_PENDING_GAME_LIMITS							1950  // 44 reqId, userIntId   // PYR-27779
#define DBM_A_GET_USER_PENDING_GAME_LIMITS							(DBM_DELTA + DBM_Q_GET_USER_PENDING_GAME_LIMITS) // 2950 2[s|4<4444>] errorcode[errDescr|numLimits<limitType, limitId, oldValue, newValue>]

//#define DBM_Q_AVAILABLE_MESSAGE                              1951 	//44<44s> reqId num<officeId percentage forbiddenCountries>

#define DBM_Q_TLB_PERIOD                                            1952 	//444TT4 - reqId, numEvents, numUsers, timeFrom, timeTo, tlbBoard {eKeyRegularTlb (default if not specified) or eKeyItRegularTlb}
#define DBM_A_TLB_PERIOD                                            (DBM_DELTA + DBM_Q_TLB_PERIOD) // 2952 	//46[s] reqId, err_code[errDescr!<type, srvFrom srvTo numPoints<intervalTlb>>] 

#define DBM_Q_GET_SPECIALEMAIL_LIST                                 1953 	//44 reqId emailType
#define DBM_A_GET_SPECIALEMAIL_LIST                                 (DBM_DELTA + DBM_Q_GET_SPECIALEMAIL_LIST) // 2953 	//46[s] 4<4s> reqId, err_code[errDescr!numEmails (emailId emailTitle)] 

#define DBM_Q_DELETE_SAVED_CARDINFO                                 1954 	//4ss8 requestId userId paySystem appLoginId
#define DBM_A_DELETE_SAVED_CARDINFO                                 (DBM_DELTA + DBM_Q_DELETE_SAVED_CARDINFO) // 2954 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_PAYMENT_GFSL_NOTIF                                    1955 	//44s1ss4414s requestId transId xTransStr transType('P'/'R') currency(USD/CAD) maskedAcctNum amtRequested amtPaid status('A'/'R') authCode authMsg
#define DBM_A_PAYMENT_GFSL_NOTIF                                    (DBM_DELTA + DBM_Q_PAYMENT_GFSL_NOTIF) // 2955 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_USER_ADDCHIPS_AUTO                                    1956 	// identical to DBM_A_USER_ADDCHIPS until chipsStart /..plus 7s7487 quantum, currency, scalePM, itLastAddonId,appSessionId,minRebuyAmount
#define DBM_A_USER_ADDCHIPS_AUTO                                    (DBM_DELTA + DBM_Q_USER_ADDCHIPS_AUTO) // 2956 	// identical to DBM_A_USER_ADDCHIPS 

#define DBM_Q_DONT_SEND_TOURNEMAIL                                  1957 	//4sb reqId userId set_not_remove
#define DBM_A_DONT_SEND_TOURNEMAIL                                  (DBM_DELTA + DBM_Q_DONT_SEND_TOURNEMAIL) // 2957 	//46[s|s44] reqId, err_code[errDescr|userId new_privs flags] 

#define DBM_Q_UPDATE_ACH_CBACK_STATUS                               1958 	//44<41> reqId, numReturns<achCBackId, csrAction>
#define DBM_A_UPDATE_ACH_CBACK_STATUS                               (DBM_DELTA + DBM_Q_UPDATE_ACH_CBACK_STATUS) // 2958 	//44<46s> reqId, numReturns<cbackId, errCode, errDescr> 

#define DBM_Q_NJ_BLACKLIST_CHECK				                    1959 	// 4ssdss reqId, userId, ssn4, dob, firstName, lastName
#define DBM_A_NJ_BLACKLIST_CHECK						            (DBM_DELTA + DBM_Q_NJ_BLACKLIST_CHECK) // 2959 6[s]4<old NjBlacklistRecord>

#define DBM_Q_SET_NOMAIL_PRIVILEGES_WITH_COMMENT                    1960  ////41s4<ss> reqId removeNotSet adminId numUsersr<userId comment>
#define DBM_A_SET_NOMAIL_PRIVILEGES_WITH_COMMENT                    (DBM_DELTA + DBM_Q_SET_NOMAIL_PRIVILEGES_WITH_COMMENT) // 2960  //44<6s> reqId numUsersr<errCode errDescr> 

#define DBM_Q_ADMIN_SAVE_BONUS                                      1961 	//4(bonus) reqId, bonus  
#define DBM_A_ADMIN_SAVE_BONUS                                      (DBM_DELTA + DBM_Q_ADMIN_SAVE_BONUS) // 2961 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ADMIN_GET_BONUSES                                     1962 	//4 reqId
#define DBM_A_ADMIN_GET_BONUSES                                     (DBM_DELTA + DBM_Q_ADMIN_GET_BONUSES) // 2962 	//46([s]/4<bonus> reqId, err_code[errDescr]/numBonuses <bonus> 

// 1963, 1964 moved to reportdbm.h

#define DBM_Q_REVOKE_USER_CHAT                                      1965 	//4sbTs reqId userId revokeNotReinstate timeUntil comment
#define DBM_A_REVOKE_USER_CHAT                                      (DBM_DELTA + DBM_Q_REVOKE_USER_CHAT) // 2965 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_ADD_TRACKCODE                                         1966 	//4ss444s reqId trackCode TrackDesc status trackType trackCodeDb adminUserId 
#define DBM_A_ADD_TRACKCODE                                         (DBM_DELTA + DBM_Q_ADD_TRACKCODE) // 2966 	//46[s] reqId, err_code[errDescr|actualTrackCodeAdded] 

#define DBM_Q_MODIFY_USERS_TRACKCODE                                1967 	//4s1b4<s>s reqId trackCode mode (0 add 1 delete) overLimited numUsers <userId> adminId
#define DBM_A_MODIFY_USERS_TRACKCODE                                (DBM_DELTA + DBM_Q_MODIFY_USERS_TRACKCODE) // 2967 	//46[s] reqId, err_code[errDescr|numusers<userId err_code errDescr>] 

#define DBM_Q_GET_GATEWAY_SETTINGS                                  1968 	//4 reqId 
#define DBM_A_GET_GATEWAY_SETTINGS                                  (DBM_DELTA + DBM_Q_GET_GATEWAY_SETTINGS) // 2968 	//46[s|2<GatewaySetting>] reqId, err_code[errDescr|numEntries <GatewaySetting>] 

#define DBM_Q_UPDATE_GATEWAY_SETTINGS                               1969 	//4s42<GatewaySetting>s reqId adminId adminIntId numUpdates <GatewaySetting> comment
#define DBM_A_UPDATE_GATEWAY_SETTINGS                               (DBM_DELTA + DBM_Q_UPDATE_GATEWAY_SETTINGS) // 2969 	//46[s] reqId, err_code[errDescr] 

#define DBM_Q_GET_CARDBINS                                          1970 	//4 reqId
#define DBM_A_GET_CARDBINS                                          (DBM_DELTA + DBM_Q_GET_CARDBINS) // 2970 	//46[s|4<ssbD4>] reqId numBins<bin country isHostile hostileExpiry flags> 

#define DBM_Q_LOBBY_SET_OPTIONS                                     1971 	//44sT reqId, optionsUINT, optionsStr, optionsTime
#define DBM_A_LOBBY_SET_OPTIONS                                     (DBM_DELTA + DBM_Q_LOBBY_SET_OPTIONS) // 2971 	//46[s] 

#define DBM_Q_SET_XTRANSSTR                                         1972 	//4M(s4)4s4 reqId, M(adminId, adminIntId), transId, xTransStr, ctTableId(enum eCashtransTableId) !!! NOTE: admin info sent for all cashier->dbm connections
#define DBM_A_SET_XTRANSSTR                                         (DBM_DELTA + DBM_Q_SET_XTRANSSTR) // 2972 	//46[s] 

#define DBM_Q_GCWIRE_INIT                                           1973 	//4s4sss1 reqId userId amount currency bonusCode productId isWirecardNotGcwire
#define DBM_A_GCWIRE_INIT                                           (DBM_DELTA + DBM_Q_GCWIRE_INIT) // 2973 	//46[s|4] reqId [errDescr|orderId] 

#define DBM_Q_GCWIRE_UPDATE                                         1974 	//44s reqId orderId paymentRef
#define DBM_A_GCWIRE_UPDATE                                         (DBM_DELTA + DBM_Q_GCWIRE_UPDATE) // 2974 	//46[s] 

#define DBM_Q_SET_WIRE_UNKNOWN                                      1975 	// see DBM_Q_SET_UNKNOWN_STATUS
#define DBM_A_SET_WIRE_UNKNOWN                                      (DBM_DELTA + DBM_Q_SET_WIRE_UNKNOWN) // 2975 	// see DBM_A_SET_UNKNOWN_STATUS 

#define DBM_Q_GCWIRE_NOTIFY_CHECK                                   1976 	//41s4<s4sTs4s> reqId isWirecardNotGcwire adminId numItems<paymentRef amount currency timeCompleted bankUniqueId orderId errorInfo>
#define DBM_A_GCWIRE_NOTIFY_CHECK                                   (DBM_DELTA + DBM_Q_GCWIRE_NOTIFY_CHECK) // 2976 	//44<M> - reqId, count<WireNotifyStruct> 

// [vladimirk]  RegActions PYR-39488 removed
#define DBM_Q_INSERT_REGACTION                                      1977 	//44TTss reqId actionType,actionStart,actionEnd,paramstr,desc
#define DBM_A_INSERT_REGACTION                                      (DBM_DELTA + DBM_Q_INSERT_REGACTION) // 2977 	//46[s] 

#define DBM_Q_UPDATE_REGACTION                                      1978 	//444TTss reqId actionId, cancelled, actionStart,actionEnd,paramstr,desc
#define DBM_A_UPDATE_REGACTION                                      (DBM_DELTA + DBM_Q_UPDATE_REGACTION) // 2978 	//46[s] 

#define DBM_Q_GET_EXCLUDED_STATES                                   1979 	//4 reqId 
#define DBM_A_GET_EXCLUDED_STATES                                   (DBM_DELTA + DBM_Q_GET_EXCLUDED_STATES) // 2979 	//46[s|2<s2<s2<s>>>] reqId err_code [errStr | numPaysystems <paysystem numCountries <country numStates <state>>>] 

#define DBM_Q_UPDATE_EXCLUDED_STATES                                1980 	//41sss2<s2<s>> reqId addNotRemove adminId comment paysystem numCountries <country numStates <state>>
#define DBM_A_UPDATE_EXCLUDED_STATES                                (DBM_DELTA + DBM_Q_UPDATE_EXCLUDED_STATES) // 2980 	//46[s] reqId err_code[errDescr] 

// from URU via cashier
#define DBM_Q_AGE_VERIFIED_URU                                      1981 	//4s(I)8 reqId userId AgeVerifyInfo appLoginId
#define DBM_A_AGE_VERIFIED_URU                                      (DBM_DELTA + DBM_Q_AGE_VERIFIED_URU) // 2981 	//46[s|44] reqId err_code[errDescr|userFlags, resultId] 

// from admin forced set/remove
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_MODIFY_AGE_VERIFIED                                   1982 	//4s1s reqId userId setNotRemove comment
#define DBM_A_MODIFY_AGE_VERIFIED                                   (DBM_DELTA + DBM_Q_MODIFY_AGE_VERIFIED) // 2982 	//46[s] reqId err_code[errDescr] 

#define DBM_Q_GCDEBIT_GET_DEPOSITS                                  1983 	//4 reqId
#define DBM_A_GCDEBIT_GET_DEPOSITS                                  (DBM_DELTA + DBM_Q_GCDEBIT_GET_DEPOSITS) // 2983 	//46[s|4<GCDebitData>] reqId err_code[errDescr | numRecords <GCDebitData> ] 

#define DBM_Q_GCDEBIT_UPDATE_DEPOSITED                              1984 	//44<4> reqId numAccounts <userId>
#define DBM_A_GCDEBIT_UPDATE_DEPOSITED                              (DBM_DELTA + DBM_Q_GCDEBIT_UPDATE_DEPOSITED) // 2984 	//46[s] reqId err_code[errDescr] 

#define DBM_Q_MODIFY_USER_TRACKCODE                                 1985 	//4s1bss4 reqId, trackCode, mode(0-add,1-delete), overLimited, userId, adminId, adminIntId
#define DBM_A_MODIFY_USER_TRACKCODE                                 (DBM_DELTA + DBM_Q_MODIFY_USER_TRACKCODE) // 2985 	//46[s] reqId, err_code[errDescr>]

#define DBM_Q_GCDEBIT_UPDATE_ACCOUNT                                1986 	//4s1 reqId userId status
#define DBM_A_GCDEBIT_UPDATE_ACCOUNT                                (DBM_DELTA + DBM_Q_GCDEBIT_UPDATE_ACCOUNT) // 2986 	//46[s] reqId err_code[errDescr | GCDebitData] 

//#define DBM_Q_AVAILABLE_MESSAGE                                   1987 	//4s reqId userId

#define DBM_Q_MONET_USER_INFO                                       1988 	//4s reqId userId
#define DBM_A_MONET_USER_INFO                                       (DBM_DELTA + DBM_Q_MONET_USER_INFO) // 2988 	//46[s|444] reqId err_code[errDescr | flags privileges userintid email lt_fpp lt_fpp_cents vipstatus vipcounter1 vipcounter1_cents vipcounter2 vipcounter2_cents emlocale vipstatusset] 

#define DBM_Q_RSA_DATA_MOVEOUT										1989	//4s reqId userId
#define DBM_A_RSA_DATA_MOVEOUT										(DBM_DELTA + DBM_Q_RSA_DATA_MOVEOUT)	//46[s|b] reqId err_code[errDescr | body]

#define DBM_Q_RSA_DATA_MOVEIN										1990	//4sb reqId userId body
#define DBM_A_RSA_DATA_MOVEIN										(DBM_DELTA + DBM_Q_RSA_DATA_MOVEIN)	//46[s] reqId err_code[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SET_USER_DOB                                          1991 	//4sDb reqId userId dateOfBirth awareOfUnder18
#define DBM_A_SET_USER_DOB                                          (DBM_DELTA + DBM_Q_SET_USER_DOB) // 2991 	//46[s] reqId err_code[errDescr] 

#define DBM_Q_UPDATE_TRACKCODE                                      1992 	//4ss44 reqId trackCode TrackDesc status TrackType
#define DBM_A_UPDATE_TRACKCODE                                      (DBM_DELTA + DBM_Q_UPDATE_TRACKCODE) // 2992 	//46[s] reqId err_code[errDescr] 

#define DBM_Q_DELETE_TRACKCODE                                      1993 	//4ss reqId trackCode
#define DBM_A_DELETE_TRACKCODE                                      (DBM_DELTA + DBM_Q_DELETE_TRACKCODE) // 2993 	//46[s] reqId err_code[errDescr] 

#define DBM_Q_GCDEBIT_NOTIFY                                        1994 	//44<44s4s4DDsssss> reqId numRec <GCDebitNotification>
#define DBM_A_GCDEBIT_NOTIFY                                        (DBM_DELTA + DBM_Q_GCDEBIT_NOTIFY) // 2994 	//46[s|4<ss4 GCDebitNotification>] reqId err_code[errDescr|numUsers<userId email locale GCDebitNotification>] 

#define DBM_Q_ADMIN_ADD_FPPSTORE_USERBONUS                          1995 	//44ss444 reqId bonusId userId comment chipsAwarded fppCollected
#define DBM_A_ADMIN_ADD_FPPSTORE_USERBONUS                          (DBM_DELTA + DBM_Q_ADMIN_ADD_FPPSTORE_USERBONUS) // 2995 	//46[s] reqId err_code[errDescr] 

#define DBM_Q_READ_CACHIN_LIMITS                                    1996 	//4 reqId
#define DBM_A_READ_CACHIN_LIMITS                                    (DBM_DELTA + DBM_Q_READ_CACHIN_LIMITS) // 2996 	//46[s] reqId err_code[errDescr] 

// PYR-23435 automation -> CRMServer
#define DBM_Q_GET_TRACKCODE_DB										1997	//4s reqId trackCode
#define DBM_A_GET_TRACKCODE_DB                                      (DBM_DELTA + DBM_Q_GET_TRACKCODE_DB) // 2997 	//46[s]|44 reqId err_code [errDescr]|db status 

//#define DBM_Q_AVAILABLE_MESSAGE											1998

#define DBM_Q_RELEASE_USERBONUS                                     1999 	//4444ss reqId userBonusId comment 
#define DBM_A_RELEASE_USERBONUS                                     (DBM_DELTA + DBM_Q_RELEASE_USERBONUS) // 2999 	//46[s] reqId err_code[errDescr]
// ! STOP !
// Do NOT insert new messages here
// A new range started at 7000!!!
#define DBM_Q_PROCESS_USER_CONFIRMED_DEPOSIT_LIMITS                 7001    // 484<44b44s> userIntId,appLoginId, numLimits<limitType,limitId,isApprove,prevLimit,limit,currency> PYR-27795
#define DBM_A_PROCESS_USER_CONFIRMED_DEPOSIT_LIMITS                 (DBM_DELTA + DBM_Q_PROCESS_USER_CONFIRMED_DEPOSIT_LIMITS) // 8001  2[s|4<2s44b44s> errCode[errDescr|numLimits<errCode,errDescr,limitType,limitId,approve,prevLimit,limit,currency>]

#define DBM_Q_PROCESS_USER_CONFIRMED_GAME_LIMITS					7002    // 484<44b44s> userIntId,appLoginId, numLimits<limitType,limitId,isApprove,prevLimit,limit,currency> PYR-27795
#define DBM_A_PROCESS_USER_CONFIRMED_GAME_LIMITS					(DBM_DELTA + DBM_Q_PROCESS_USER_CONFIRMED_GAME_LIMITS)    // 8002  2[s|4<2s44b44s> errCode[errDescr|numLimits<errCode,errDescr,limitType,limitId,approve,prevLimit,limit,currency>]

// PYR-27243: LobbyEtc->LDBM
#define DBM_Q_LDBM_UPDATE_LIMITEDMODE_GEOIP							7003  //4s4ss - reqId,userId,status,countryByIp,regionByIp
#define DBM_A_LDBM_UPDATE_LIMITEDMODE_GEOIP							(DBM_DELTA + DBM_Q_LDBM_UPDATE_LIMITEDMODE_GEOIP) // 8003  //46[s] - reqId, errCode[errDescr]

// #27257 #PYR-111146 #todo obsolete after build 08 admin deployed on nj host
#define DBM_Q_SAVE_NJBLACKLIST                                      7004 // 4o reqId, old NjBlacklistRecord
#define DBM_A_SAVE_NJBLACKLIST                                      (DBM_DELTA + DBM_Q_SAVE_NJBLACKLIST) // 6[s|old NjBlacklistRecord]

#define DBM_Q_DELETE_USBLACKLIST                                    7005 // 44s - reqId,blacklistId,comment
#define DBM_A_DELETE_USBLACKLIST                                    (DBM_DELTA + DBM_Q_DELETE_USBLACKLIST) // 6[s]

// PYR-27741: LoginDbm -> OLTP
#define DBM_Q_UPDATE_USER_LOGINTIME_STATUS							7006	// 4sbb44t2244844 - reqId, userId, time_limit_reached_or_reseted, sendlimitHitDatatoDB, hitId, userIntId, when, hitType, limitCategory, limitType, limitRefId, hitRefId, limitAmount, hitAmount
#define DBM_A_UPDATE_USER_LOGINTIME_STATUS							(DBM_DELTA + DBM_Q_UPDATE_USER_LOGINTIME_STATUS) // 8006  //46[s] - reqId, errCode[errDescr]	// 4s - reqId, userId

// PYR-27243: LobbyEtc->DBMRO
#define DBM_Q_GET_USER_MOBILE_PHONE_NUMBER							7007 // 4s - reqId,userId
#define DBM_A_GET_USER_MOBILE_PHONE_NUMBER							(DBM_DELTA + DBM_Q_GET_USER_MOBILE_PHONE_NUMBER) // 8007  //46[s|s4] - reqId, errCode[errDescr|mobilePhoneNum,unique]

// PYR-27262
#define DBM_Q_SAVE_LIFETIME_LIMIT_CONFIRMED							7008 // 4s8 - reqId,userId,appLoginId
#define DBM_A_SAVE_LIFETIME_LIMIT_CONFIRMED							(DBM_DELTA + DBM_Q_SAVE_LIFETIME_LIMIT_CONFIRMED) // 8008  //2[s] - reqId, errCode[errDescr]

// PYR-28123
#define DBM_Q_CRM_ASSIGN_TRACKCODE									7009 // 4ssb - reqId, userId, trackCode, intellipoker
#define DBM_A_CRM_ASSIGN_TRACKCODE									(DBM_DELTA + DBM_Q_CRM_ASSIGN_TRACKCODE) // 8009  //2[s] - reqId, errCode[errDescr]

//	PYR-27258: Regulator -> OLTP
#define DBM_Q_CLEAR_STRONG_AUTH_REQD								7010 // ssss - userId, answer1, answer2, ssn4
#define DBM_A_CLEAR_STRONG_AUTH_REQD								(DBM_DELTA + DBM_Q_CLEAR_STRONG_AUTH_REQD)	//	8010	// 46[s]4s - reqId, errCode[ErrDescr], locale, userId

// PYR-24156
#define DBM_Q_GET_USER_PWD_UPDATE_TIME								7011  //44 req_id, userIntId
#define DBM_A_GET_USER_PWD_UPDATE_TIME								(DBM_DELTA + DBM_Q_GET_USER_PWD_UPDATE_TIME) // 8011  //46[s|T] req_id, err_code, [errDescr|pswUpdate] 

//#define DBM_Q_AVAILABLE_MESSAGE                  7012 //4 reqId

//PYR-27249, Lobby to RO DBM
#define DBM_Q_GET_BUYIN_LIMIT_EX									7013	//4s - reqId,userId
#define DBM_A_GET_BUYIN_LIMIT_EX									(DBM_DELTA + DBM_Q_GET_BUYIN_LIMIT_EX)	// 8013	//46[s|6<44>] - reqId,errCode,[errDescr|numLimits,<limitType,amount>]

//Lobby to RW OLTP
#define DBM_Q_SET_BUYIN_LIMIT_EX									7014	//4s86<44> - reqId,userId,appLoginId,numLimits,<limitType,amount>
#define DBM_A_SET_BUYIN_LIMIT_EX									(DBM_DELTA + DBM_Q_SET_BUYIN_LIMIT_EX) // 8014	//46[s|6<46[s|4b]>] - reqId,errCode,[errDescr|numLimits,<limitType,errCode,[eddDescr|amount,effectiveNow]>]

// PYR-28269 Pausa to RO DBM
#define DBM_Q_PAUSA_GET_STATUS										7015 // 44 - reqId, transId
#define DBM_A_PAUSA_GET_STATUS										(DBM_DELTA + DBM_Q_PAUSA_GET_STATUS) // 8015  //2[s] - reqId, errCode[errDescr|status]

#define DBM_Q_GET_USER_PAST_VIPSTATUS								7016 // 44T - reqId userIntId srvTime
#define DBM_A_GET_USER_PAST_VIPSTATUS								(DBM_DELTA + DBM_Q_GET_USER_PAST_VIPSTATUS) // 8016  // 46[s|4] reqId errCode[errDescr|vipStatus]

#define DBM_Q_DRC_PROXY_REQUEST 									7017 // 44bb4B - reqId, dbmId, isReadOnly, isTournDbm, msgId, nsgBody
#define DBM_A_DRC_PROXY_REQUEST  									(DBM_DELTA + DBM_Q_DRC_PROXY_REQUEST) // 8017  //46[s|B] - reqId, errCode[errDescr | msgBody]

#define DBM_Q_GET_USERS_BY_EMAIL 									7018 // 4s - reqId, email
#define DBM_A_GET_USERS_BY_EMAIL  									(DBM_DELTA + DBM_Q_GET_USERS_BY_EMAIL) // 8018

// image gallery PYR-26568
#define DBM_Q_SAVE_USER_IMAGE_APPROVAL								7019 // 4O4 - reqId, ImageApprovalItem, adminIntId
#define DBM_A_SAVE_USER_IMAGE_APPROVAL								(DBM_DELTA + DBM_Q_SAVE_USER_IMAGE_APPROVAL) // 8019 46[s] - reqId, errCode[errDescr]

#define DBM_Q_DELETE_USER_IMAGE_APPROVAL							7020 // 4444 - reqId, imageId, userIntId, adminIntId
#define DBM_A_DELETE_USER_IMAGE_APPROVAL							(DBM_DELTA + DBM_Q_DELETE_USER_IMAGE_APPROVAL) // 8020 46[s] - reqId, errCode[errDescr]

#define DBM_Q_GET_IMAGE_USER_APPROVALS								7021 // 4 - reqId, userIntId
#define DBM_A_GET_IMAGE_USER_APPROVALS								(DBM_DELTA + DBM_Q_GET_IMAGE_USER_APPROVALS) // 8021 46[s|4<O>] - reqId, errCode[errDescr|numItems<M(ImageApprovalItem)>]

#define DBM_Q_GET_IMAGE_ONE_USER_APPROVAL							7022 // 444 - reqId, imgId, userIntId
#define DBM_A_GET_IMAGE_ONE_USER_APPROVAL							(DBM_DELTA + DBM_Q_GET_IMAGE_ONE_USER_APPROVAL) // 8022 46[s|4<O>] - reqId, errCode[errDescr|ImageApprovalItem]

// PYR-28287 from PSIP to OLAP
#define DBM_Q_GET_PSIP_VMINFO										7023 // 4s - reqId, userId
#define DBM_A_GET_PSIP_VMINFO										(DBM_DELTA + DBM_Q_GET_PSIP_VMINFO) // 8023 // 42[s|s] - reqId, errCode[errDescr|vmwtype]

// PYR-27224
#define DBM_QS_DIRECT_ASSIGN_TRACKCODE								7024 // 4s4 - reqId, trackCode, userIntId
#define DBM_AS_DIRECT_ASSIGN_TRACKCODE								(DBM_DELTA + DBM_QS_DIRECT_ASSIGN_TRACKCODE) // 8024  //2[s] - reqId, errCode[errDescr]

//PYR-28135
#define DBM_Q_ALLOW_EXTRA_ENTRY_TO_LE_TOURN							7025	//	444<4s44> - reqId, tgmId, numUsers <userIntId, userId, numExtraEntries, flags >
#define DBM_A_ALLOW_EXTRA_ENTRY_TO_LE_TOURN							(DBM_DELTA + DBM_Q_ALLOW_EXTRA_ENTRY_TO_LE_TOURN) //8025	//	46[s|4<4s44>]> - reqId, errCode [ errMsg | numUsers <userIntId, userId, numExtraEntries, flags> ]
#define DBM_Q_GET_TGM_DATA											7026  //44bb reqId, tgmId
#define DBM_A_GET_TGM_DATA											(DBM_DELTA + DBM_Q_GET_TGM_DATA) // 8026  46[s|4<444>] reqId, errCode[ errMsg | numUsers < userIntId, numEntries, flags > ]

// PYR-26639
#define DBM_Q_CLOSE_BATCH											7027
#define DBM_A_CLOSE_BATCH											(DBM_DELTA + DBM_Q_CLOSE_BATCH)		// 8027

// PYR-27889
#define DBM_Q_TOURN_STARTED											7028 // 44t4<4> - reqId, tournamentId, timestamp, userCount, <userIntId>
#define DBM_A_TOURN_STARTED											(DBM_DELTA + DBM_Q_TOURN_STARTED) // 8028 46[s]

//	PYR-28552
#define DBM_Q_ADMIN_SET_STRONG_AUTH_REQD							7029	//	4sbs reqId,userId,setOrClearFlag,comment
#define DBM_A_ADMIN_SET_STRONG_AUTH_REQD							(DBM_DELTA + DBM_Q_ADMIN_SET_STRONG_AUTH_REQD)	//	8029  // 46[s] - reqId,errCode[errDescr] 


//	PYR-28611
#define DBM_Q_AUDIT_RETRIEVE_USERID									7030	// 4ss4<s> - reqId,email,ipaddr,size<userIds>
#define DBM_A_AUDIT_RETRIEVE_USERID									(DBM_DELTA + DBM_Q_AUDIT_RETRIEVE_USERID)	//	8030	// 46[s] - reqId,errCode[errStr]

// PYR-28691 Admin - LoginDbm
#define DBM_Q_UPDATE_IT_USER_LOGIN_STATUS							7031	// 4b reqId, login enabled/disabled
#define DBM_A_UPDATE_IT_USER_LOGIN_STATUS							(DBM_DELTA + DBM_Q_UPDATE_IT_USER_LOGIN_STATUS)	//	8031  // 46[s] - reqId,errCode[errDescr] 

// PYR-27251
#define DBM_Q_NEED_RESURRECTION_TABLE_DATA                          7032  // 44<8> - reqId, <tableId>
#define DBM_A_NEED_RESURRECTION_TABLE_DATA                          (DBM_DELTA + DBM_Q_NEED_RESURRECTION_TABLE_DATA) //

#define DBM_Q_LOBBY_SET_TOURN_PROPERTY                              7033 	//44s4<4> - reqId,propId,propValue,<tournId>
#define DBM_A_LOBBY_SET_TOURN_PROPERTY                              (DBM_DELTA + DBM_Q_LOBBY_SET_TOURN_PROPERTY) // 2486 	//46[s] - reqId,errCode,[errDescr] 

#define DBM_Q_LOBBY_DELETE_TOURN_PROPERTY                           7034 	//444<4> - reqId,propId,<tournId>
#define DBM_A_LOBBY_DELETE_TOURN_PROPERTY                           (DBM_DELTA + DBM_Q_LOBBY_DELETE_TOURN_PROPERTY) // 2486 	//46[s] - reqId,errCode,[errDescr] 

#define DBM_Q_EXPIRED_LIMITED_ENTRY_TGMS_DATE						7035	//4d
#define DBM_A_EXPIRED_LIMITED_ENTRY_TGMS_DATE						(DBM_DELTA + DBM_Q_EXPIRED_LIMITED_ENTRY_TGMS_DATE)//8035	46[s|4<4>]

// PYR-26665
#define DBM_Q_CHECK_CAN_UPLOAD_CUSTOM_IMAGE							7036 //44 - reqId,userIntId
#define DBM_A_CHECK_CAN_UPLOAD_CUSTOM_IMAGE							(DBM_DELTA + DBM_Q_CHECK_CAN_UPLOAD_CUSTOM_IMAGE)	// 8036 46[s|bt] - reqId,errCode[errDescr|canUpload,whenCan]
// PYR-28898 - added debug message just for testing
#ifdef _DEBUG
#define DBM_Q_DEBUG													7037	//44M - reqId, debugOp, M(...)
#define DBM_A_DEBUG													(DBM_DELTA + DBM_Q_DEBUG) // 8037 // 46[s] - reqId, errCode[errDescr]
#endif // #ifdef _DEBUG

#define DBM_Q_PAY_GREEK_USER_DAILY_TAX								7038 //44 - reqId,userIntId
#define DBM_A_PAY_GREEK_USER_DAILY_TAX								(DBM_DELTA + DBM_Q_PAY_GREEK_USER_DAILY_TAX) // 8038

#define DBM_Q_GET_GREEK_USERS_FOR_DAILY_TAX							7039 //4
#define DBM_A_GET_GREEK_USERS_FOR_DAILY_TAX							(DBM_DELTA + DBM_Q_GET_GREEK_USERS_FOR_DAILY_TAX) // 8039 // 44<4> reqId, vecSize <UserIntId>

// PYR-28731, PYR-38265
#define DBM_Q_GET_NEW_INSTRUMENT_DATA								7040 //4bs4444 - reqId,newIssuerOnly,bin,cc_id,transId,minAmt,minutesFirstDep
#define DBM_A_GET_NEW_INSTRUMENT_DATA								(DBM_DELTA + DBM_Q_GET_NEW_INSTRUMENT_DATA)	// 8040 46[s|4] - reqId,errCode[errDescr|nFI]

// PYR-28860 begin
#define DBM_Q_GET_FOR_RECRYPT_CARDNUMS				7041 // 44s - reqId, portionSize, prefix
#define DBM_A_GET_FOR_RECRYPT_CARDNUMS				(DBM_DELTA + DBM_Q_GET_FOR_RECRYPT_CARDNUMS) // 8041 6[s|4<4s>] - errCode [errDescr | n <cardNumId, enc_cc>]

#define DBM_Q_UPDATE_RECRYPTED_CARDNUMS				7042 // 44<4s> - reqId, n <cardNumId, enc_cc>
#define DBM_A_UPDATE_RECRYPTED_CARDNUMS				(DBM_DELTA + DBM_Q_UPDATE_RECRYPTED_CARDNUMS) // 8042 6[s]

#define DBM_Q_GET_FOR_RECRYPT_CARDINFO				7043 // 44s - reqId, portionSize, prefix
#define DBM_A_GET_FOR_RECRYPT_CARDINFO				(DBM_DELTA + DBM_Q_GET_FOR_RECRYPT_CARDINFO) // 8043 6[s|4<4sss>] - errCode [errDescr | n <infoId, cardNum, dLicense, secNum>]

#define DBM_Q_UPDATE_RECRYPTED_CARDINFO				7044 // 44<4sss> - reqId, n <infoId, cardNum, dLicense, secNum, oldCN, oldDL, oldSN>
#define DBM_A_UPDATE_RECRYPTED_CARDINFO				(DBM_DELTA + DBM_Q_UPDATE_RECRYPTED_CARDINFO) // 8044 6[s]

//#define DBM_Q_AVAILABLE_MESSAGE			7045 // 44s - reqId, portionSize, prefix

//#define DBM_Q_AVAILABLE_MESSAGE			7046 // 44<4sss> - reqId, n <debitId, bank, acc, holder>

#define DBM_Q_GET_FOR_RECRYPT_USERFISCALENCR		7047 // 44s - reqId, portionSize, prefix
#define DBM_A_GET_FOR_RECRYPT_USERFISCALENCR		(DBM_DELTA + DBM_Q_GET_FOR_RECRYPT_USERFISCALENCR) // 8047 6[s|4<4s>] - errCode [errDesc | n <userIntId, fiscalEncr>]

#define DBM_Q_UPDATE_RECRYPTED_USERFISCALENCR		7048 // 44<4s> - reqId, n <userIntId, fiscalEncr>
#define DBM_A_UPDATE_RECRYPTED_USERFISCALENCR		(DBM_DELTA + DBM_Q_UPDATE_RECRYPTED_USERFISCALENCR) // 8048 6[s]
// PYR-28860 end

// MONET -> DBMRO
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	DBM_Q_MONET_GET_USER_TABLE_CAP				7049	//44<4> reqId, numofUsers, userIntIds	PYR-29283
#define DBM_A_MONET_GET_USER_TABLE_CAP				(DBM_DELTA + DBM_Q_MONET_GET_USER_TABLE_CAP)	// 46[s|4<M(464)>] reqId, errCode, [errMsg|numofUsers, Msg(userintId, errCode, usertableCap)]

#define DBM_Q_GET_HELD_CASHTRANS_DEPOSIT            7050 // PYR-29092
#define DBM_A_GET_HELD_CASHTRANS_DEPOSIT            (DBM_DELTA + DBM_Q_GET_HELD_CASHTRANS_DEPOSIT) // 8050

#define DBM_Q_SELECT_ESUSERS_FOR_CHECK				7051 // PYR-29281 4tt - reqId rmOkBefore notCheckedAfter
#define DBM_A_SELECT_ESUSERS_FOR_CHECK				(DBM_DELTA + DBM_Q_SELECT_ESUSERS_FOR_CHECK) // 8051 6[s|4<{}>] - errCode [errDescr | n< {EsUsersIdAndBanCheck} >]

#define DBM_Q_INSERT_ESUSER_CHECK_RESULT			7052 // PYR-29281 4{} - reqId {EsUsersIdAndBanCheck}
#define DBM_A_INSERT_ESUSER_CHECK_RESULT			(DBM_DELTA + DBM_Q_INSERT_ESUSER_CHECK_RESULT) // 8052 6[s]

//PYR-28998
#define DBM_Q_MTL_USERS_OUT_NO_AWARDS				7053	//4<rec>4<rec> - reqId,<tournIngo>,numUsers,<userInfo>
#define DBM_A_MTL_USERS_OUT_NO_AWARDS				(DBM_DELTA + DBM_Q_MTL_USERS_OUT_NO_AWARDS)	// 8053 //46[s] - reqId,errCode,[errDescr]

// PYR-28503
#define DBM_Q_GET_CANCEL_RUNNING_TOURN_DATA			7054 //44f4 - reqId, tournId, isBlitz, flightTournFlags
#define DBM_A_GET_CANCEL_RUNNING_TOURN_DATA			(DBM_DELTA + DBM_Q_GET_CANCEL_RUNNING_TOURN_DATA) // 8054 //46[s|...] - reqId, errCode[errDescr|...]

//#define DBM_Q_AVAILABLE_MESSAGE					7055

#define DBM_Q_CASHIN_PM_INIT_RW						7056
#define DBM_A_CASHIN_PM_INIT_RW						(DBM_DELTA + DBM_Q_CASHIN_PM_INIT_RW) // 8056

// PYR-25226
#define DBM_Q_UPDATE_ALLOCATED_UNUSEDSTACKS			7057
#define DBM_A_UPDATE_ALLOCATED_UNUSEDSTACKS			(DBM_DELTA + DBM_Q_UPDATE_ALLOCATED_UNUSEDSTACKS)  // 8057

#define DBM_Q_UPDATE_ALLOCATED_UNUSEDSTACKS_BATCH   7058
#define DBM_A_UPDATE_ALLOCATED_UNUSEDSTACKS_BATCH	(DBM_DELTA + DBM_Q_UPDATE_ALLOCATED_UNUSEDSTACKS_BATCH)  // 8058

#define DBM_Q_SET_USER_IDEXTENSION		            7059  // PYR-29384
#define DBM_A_SET_USER_IDEXTENSION			        (DBM_DELTA + DBM_Q_SET_USER_IDEXTENSION) // 8059

//#define DBM_Q_AVAILABLE_MESSAGE			7060	//4st - reqId,name,time

//#define DBM_Q_AVAILABLE_MESSAGE					7061	//4s - reqId,name

//#define DBM_Q_AVAILABLE_MESSAGE			7062	//4<rec> - reqId,<MilestoneGoldenSnG>

//#define DBM_Q_AVAILABLE_MESSAGE			7063	//44 - reqId,id

//#define DBM_Q_AVAILABLE_MESSAGE	    7064  // PYR-29386

#define DBM_Q_GET_ONE_USER_IP					7065  // PYR-29386 4s - reqId, userId
#define DBM_A_GET_ONE_USER_IP					(DBM_DELTA + DBM_Q_GET_ONE_USER_IP) // 8065 // 46[s|ipAddress]

#define DBM_Q_GET_ARJELUSERIDS					7066	//44<s> - reqId size <userIds>
#define DBM_A_GET_ARJELUSERIDS					(DBM_DELTA + DBM_Q_GET_ARJELUSERIDS) // 8066 //6[s|4<ss>] - errCode [errDescr | size <userId arjelUserId>

// PYR-29348
#define DBM_Q_FIRSTUSE_CARD_ACC_PM				7067  // 4s4 - reqId, userId, cardId
#define DBM_A_FIRSTUSE_CARD_ACC_PM				(DBM_DELTA + DBM_Q_FIRSTUSE_CARD_ACC_PM) // 8067  // 42[s|b] - reqId, errCode, [errDesc|firstUseCardAcc] 
// PYR-28728
#define DBM_Q_MTL_NEED_TABLE_DATA				7068	//44 - reqId, tournId
#define DBM_A_MTL_NEED_TABLE_DATA				(DBM_DELTA + DBM_Q_MTL_NEED_TABLE_DATA) // 8068 //46[s|4<O(8s4451<s1417><774>)>4<8>4] - reqId, errCode[errDescr|numTables<AllocTable(tableId,tableName,created,lastOrd,dealerPos,numUsers<name,pos,chips,hasLeft,unusedStacks><addedChips,state,reqId,ordinal>)>,numTablesToClose<tabledId>,dbmId]

#define DBM_Q_MTL_UPDATE_ALLOCATED_ADDCHIPS		7069	//44<8s7774> - reqId,numUsers<tableId,userId,deltaChips,newAddedChips,newState,newReqId>
#define DBM_A_MTL_UPDATE_ALLOCATED_ADDCHIPS		(DBM_DELTA + DBM_Q_MTL_UPDATE_ALLOCATED_ADDCHIPS) // 8069 //46[s] - reqId,errCode[errDescr]

#define DBM_Q_MTL_DELETE_FROM_ALLOCATED			7070	//44<8s> - reqId, numUsers<tableId, userId>
#define DBM_A_MTL_DELETE_FROM_ALLOCATED			(DBM_DELTA + DBM_Q_MTL_DELETE_FROM_ALLOCATED) // 8070 //46[s] - reqId, errCode[errDescr]

#define DBM_Q_GET_CANCEL_RUNNING_TOURN_TABLE_DATA	7071	//44 - reqId, tournId
#define DBM_A_GET_CANCEL_RUNNING_TOURN_TABLE_DATA	(DBM_DELTA + DBM_Q_GET_CANCEL_RUNNING_TOURN_TABLE_DATA) // 8071 //46[s|4<s487>] - reqId, errCode[errDescr|numUsers<userId, userIntId, tableId, tournChips>]

#define DBM_Q_DELETE_ALLOCATED_USERS_AND_TABLE	7072	//44<8> - reqId, numTables<tableId>
#define DBM_A_DELETE_ALLOCATED_USERS_AND_TABLE	(DBM_DELTA + DBM_Q_DELETE_ALLOCATED_USERS_AND_TABLE) // 8072 //46[s] - reqId, errCode[errDescr]

// PYR-29441
#define DBM_Q_GET_USERS_TO_CLOSE_IN_ARJEL		7073  //
#define DBM_A_GET_USERS_TO_CLOSE_IN_ARJEL		(DBM_DELTA + DBM_Q_GET_USERS_TO_CLOSE_IN_ARJEL) // 8073

// PYR-28689
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_CREDIT_TOURN_MILESTONE_WINNING_TABLE		7074	//4sf8s4O4<s>s8888 - reqId,currency,isPM,tableId,tableSeqIdStr,msEventTimeSecInScript,TournMileStonePrize,num<userId>,userComment,appLoginId,finalFlightId,buyInTransId,tournFlags2
#define DBM_A_CREDIT_TOURN_MILESTONE_WINNING_TABLE		(DBM_DELTA + DBM_Q_CREDIT_TOURN_MILESTONE_WINNING_TABLE)  // 8074 46[s|userId]

#define DBM_Q_MTL_CREDIT_PM_TOURN_MILESTONE_WINNERS		7075
#define DBM_A_MTL_CREDIT_PM_TOURN_MILESTONE_WINNERS		(DBM_DELTA + DBM_Q_MTL_CREDIT_PM_TOURN_MILESTONE_WINNERS)  // 8075

// PYR-29348
#define DBM_Q_GET_NEW_INSTRUMENT_DATA_PM		7076 //4bss4444t - reqId,newIssuerOnly,bin,userId,cc_id,transId,minAmt,minutesFirstDep,startCheckingTime
#define DBM_A_GET_NEW_INSTRUMENT_DATA_PM		(DBM_DELTA + DBM_Q_GET_NEW_INSTRUMENT_DATA_PM)	// 8076 46[s|4] - reqId,errCode[errDescr|nFI]

// PYR-29348
#define DBM_Q_GET_BUNDLE_VELOCITY_DATA_PM		7077 // 4s44<4444<4s>> - reqId, userId, transIdToIgnore, n, <PMBundleVelocityItem>
#define DBM_A_GET_BUNDLE_VELOCITY_DATA_PM		(DBM_DELTA + DBM_Q_GET_BUNDLE_VELOCITY_DATA_PM) // 8077 // 42[s|4<4444<4s>>] - reqId, errCode, [errDesc|n, <PMBundleVelocityItem>]

//PYR-29348
#define DBM_Q_GET_FIRSTDEPOSITS_DATA_PM         7078  // 4ss4444 - reqId, userId, paysystem, ruleType, transId, minAmount, minutesSince1stDeposit
#define DBM_A_GET_FIRSTDEPOSITS_DATA_PM         (DBM_DELTA + DBM_Q_GET_FIRSTDEPOSITS_DATA_PM) // 8078  // 46[s|444] - reqId, errCode, [errDesc|ruleType,minAmount,AccAmount] 

//PYR-29348
#define DBM_Q_GET_CARD_VELOCITY_DATA_PM         7079  // 4s4bs - reqId, userId, numMinutes, excPrePaidBin, bin
#define DBM_A_GET_CARD_VELOCITY_DATA_PM         (DBM_DELTA + DBM_Q_GET_CARD_VELOCITY_DATA_PM) // 8079  // 46[s44] - reqId, errCode[errDescr|numCards|numBins] 

#define DBM_Q_ADD_APPROVED_PM_PURCHASE			7080  // 4M - reqId, CashTransDataSimple
#define DBM_A_ADD_APPROVED_PM_PURCHASE			(DBM_DELTA + DBM_Q_ADD_APPROVED_PM_PURCHASE) // 8080

#define DBM_Q_CHECK_CASHIN_PM_LIMITS			7081  //4s reqId, userId
#define DBM_A_CHECK_CASHIN_PM_LIMITS            (DBM_DELTA + DBM_Q_CHECK_CASHIN_PM_LIMITS) // 8081


#define DBM_Q_SAG_SAVE_PAYOUTS						7082 // 44<o>	-reqId, numPayouts<SagPayoutTable>
#define DBM_A_SAG_SAVE_PAYOUTS						(DBM_DELTA + DBM_Q_SAG_SAVE_PAYOUTS)	// 8082 46[s|4<44s>] - reqId,errCode[errDescr|numPayouts<SagPayoutVersion>]

#define DBM_Q_SAG_GET_PAYOUTS						7083 // 44<o>	-reqId, numPayouts<SagPayoutTable>
#define DBM_A_SAG_GET_PAYOUTS						(DBM_DELTA + DBM_Q_SAG_GET_PAYOUTS)	// 8083 46[s|4<44s>] - reqId,errCode[errDescr|numPayouts<SagPayoutVersion>]

// PYR-29441
#define DBM_Q_GET_SELF_EXCL_USERS_FR		    7084  //
#define DBM_A_GET_SELF_EXCL_USERS_FR		    (DBM_DELTA + DBM_Q_GET_SELF_EXCL_USERS_FR) // 8084

//	PYR-28733
#define DBM_Q_GET_INSTALLID_INFO				7085		//	4ss  - reqId,userId,installId
#define DBM_A_GET_INSTALLID_INFO				(DBM_DELTA + DBM_Q_GET_INSTALLID_INFO)		// 8085		46[s|b4] - reqId,errCode[errDescr|installIdExistsForUser,nonMatchingUserIdCount]

// PYR-29441
#define DBM_Q_GET_USERS_FOR_INACTIVITY_CHECK_FR	7086  //
#define DBM_A_GET_USERS_FOR_INACTIVITY_CHECK_FR	(DBM_DELTA + DBM_Q_GET_USERS_FOR_INACTIVITY_CHECK_FR) // 8086

// PYR-29441
#define DBM_Q_GET_INACTIVE_USERS_FR         	7087  //
#define DBM_A_GET_INACTIVE_USERS_FR				(DBM_DELTA + DBM_Q_GET_INACTIVE_USERS_FR) // 8087

#define DBM_Q_UPDATE_SAG_TOURN		         	7088  //
#define DBM_A_UPDATE_SAG_TOURN					(DBM_DELTA + DBM_Q_UPDATE_SAG_TOURN) // 8088

// PYR-29441
#define DBM_Q_GET_CLOSED_BY_DOCS_USERS_FR		7089  //
#define DBM_A_GET_CLOSED_BY_DOCS_USERS_FR		(DBM_DELTA + DBM_Q_GET_CLOSED_BY_DOCS_USERS_FR) // 8089

// PYR-28728
#define DBM_Q_MTL_ALLOCATE_USERS_EX				7090  //484<8s14477> req_id, tableIdFrom, numUsers<tableId, userName, position, chips, userIntId, unusedStacks, ordinal>
#define DBM_A_MTL_ALLOCATE_USERS_EX				(DBM_DELTA + DBM_Q_MTL_ALLOCATE_USERS_EX) // 8090  //46[s|4<4>] req_id, err_code [errDescr|numUpdated<userIntId>]

//#define DBM_Q_AVAILABLE_MESSAGE					7091

//DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_REFRESH_BOPMAPS					7092 //4O - reqId, ...(see DBM_U_BOPMAPS_UPDATED)
//#define DBM_A_REFRESH_BOPMAPS					(DBM_DELTA + DBM_Q_REFRESH_BOPMAPS) // 8092

//DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_GET_BOPMAPS						7093 //4 - reqId
//#define DBM_A_GET_BOPMAPS						(DBM_DELTA + DBM_Q_GET_BOPMAPS) // 8093 //46[s|O] - reqId,errCode[errDescr|BopMaps] (see DBM_U_BOPMAPS_UPDATED)

#define DBM_Q_XC_START_GAME						7094 // 4O8422bs444884 reqId XcRequestCommon xcLoginId tableTypeId gameTypeId variantType isPlayMoney currency minBet maxBet selfRmLimit restoreGameId (trail is not saved) restoreHandId restoreUnresolvedBet
#define DBM_A_XC_START_GAME						(DBM_DELTA + DBM_Q_XC_START_GAME)  // 8094

#define DBM_Q_XC_TRANSACTION					7095 // 4O82884444 reqId XcRequestCommon xcTransId eXcTransType(B/R/H) handId deltaGame64(B+,H-) deltaReserved(+/-) finalReserved(>=0) unresolvedBet(>0 only for eXcTransFlag_InterruptedHand) moneyAtRisk(on HResult)
#define DBM_A_XC_TRANSACTION					(DBM_DELTA + DBM_Q_XC_TRANSACTION) // 8095

#define DBM_Q_XC_GAME_OVER						7096 // 4O2448 reqId XcRequestCommon termCode totalReservedToClear unresolvedBet totalMoneyAtRisk
#define DBM_A_XC_GAME_OVER						(DBM_DELTA + DBM_Q_XC_GAME_OVER)   // 8096

#define DBM_Q_XC_REMOVE_GAME					7097 // 4O44 reqId XcRequestCommon adminIntId unresolvedBetToCreditBack
#define DBM_A_XC_REMOVE_GAME					(DBM_DELTA + DBM_Q_XC_REMOVE_GAME) // 8097

#define DBM_Q_XCHUB_NEED_DATA					7098 // 4 reqId
#define DBM_A_XCHUB_NEED_DATA					(DBM_DELTA + DBM_Q_XCHUB_NEED_DATA)  // 8098 46[s|...] reqId errCode [errDescr|see dbm code for details]

// PYR-30183
#define DBM_Q_EXECUTE_USER_INTID_STMT			7099 // 4sb4 - reqId, statement, sendToOlap, userIntId
#define DBM_A_EXECUTE_USER_INTID_STMT			(DBM_DELTA + DBM_Q_EXECUTE_USER_INTID_STMT) // 8099 46[s] reqId, errCode[errDescr] 

// PYR-30164 - ice - special message to generate an event on the first hand played in the tournament
#define DBM_Q_MTL_USER_PLAYED_FIRST_HAND		7100 // 4O - reqId,O(see MtUserPlayedHand)
#define DBM_A_MTL_USER_PLAYED_FIRST_HAND		(DBM_DELTA + DBM_Q_MTL_USER_PLAYED_FIRST_HAND) // 8100 46[s] - reqId,errCode[errDescr]

// PYR-29926 - ice
#define DBM_Q_GET_LAST_PLATFORM					7101 // 44 - reqId,userIntId)
#define DBM_A_GET_LAST_PLATFORM					(DBM_DELTA + DBM_Q_GET_LAST_PLATFORM) // 8101 46[s|4] - reqId,errCode[errDescr|platformId]

// PYR-29926 - ice
#define DBM_Q_ICE_AWARD_TICKET					7102 // 44 - reqId,userIntId,ticketTypeId)
#define DBM_A_ICE_AWARD_TICKET					(DBM_DELTA + DBM_Q_ICE_AWARD_TICKET) // 8102 46[s|4] - reqId,errCode[errDescr]

// PYR-29926 - ice
#define DBM_Q_GET_ICE_TOURNAMENT_DATA			7103 // 48 - reqId,tournId
#define DBM_A_GET_ICE_TOURNAMENT_DATA			(DBM_DELTA + DBM_Q_GET_ICE_TOURNAMENT_DATA) // 8103 46[s|o] - reqId,errCode[errDescr|IceEvent]

// PYR-30160
#define DBM_Q_MTL_REBUY_ADDON_READDBM			7104 // 44779s - reqId,userIntId,priceRM,priceFpp,pricePM,currency
#define DBM_A_MTL_REBUY_ADDON_READDBM			(DBM_DELTA + DBM_Q_MTL_REBUY_ADDON_READDBM) // 8104 46[s] - reqId,errCode[errDescr]

#define DBM_Q_TOURN_CLOSED_BATCH				7105 // 4t14<4> - reqId, closed, status, numTourns< tournIds >
#define DBM_A_TOURN_CLOSED_BATCH				(DBM_DELTA + DBM_Q_TOURN_CLOSED_BATCH) // 8105 46[s] - reqId,errCode[errDescr]
// AuxHub->RW DBM (not RO)
#define DBM_Q_XC_GET_MAX_REQID					7106 // 4 reqId
#define DBM_A_XC_GET_MAX_REQID					(DBM_DELTA + DBM_Q_XC_GET_MAX_REQID) // 8106 46[s|8] reqId,errCode[errDescr|maxXcReqId]

//#define DBM_Q_AVAILABLE_MESSAGE			7107 //4sstt - reqId, userId, installId, begin time, end time

#define DBM_Q_GET_APPROVED_GAME_LIMIT           7108 // 444s - reqId, userIntId eTableLimitType gameCurrency
#define DBM_A_GET_APPROVED_GAME_LIMIT           (DBM_DELTA + DBM_Q_GET_APPROVED_GAME_LIMIT) // 8108 // 46[s|4s4s] - reqId, errCode[errDescr| dbLimitValue, dbLimitCurrency, gameLimit, gameCurrency] 

// PYR-30258
#define DBM_Q_GET_DEPOSIT_VELOCITY_DATA_PM		7109  // 4ssb4 - reqId, userId, paysystem, approve, numbHours
#define DBM_A_GET_DEPOSIT_VELOCITY_DATA_PM		(DBM_DELTA + DBM_Q_GET_DEPOSIT_VELOCITY_DATA_PM) // 8109  // 46[s|444] - reqId, errCode, [errDesc|nDeposits,nMinutes,amount] 

//PYR-29769
#define DBM_Q_MTL_REFUND_PM_TOURN_SAT_WINNERS	7110
#define DBM_A_MTL_REFUND_PM_TOURN_SAT_WINNERS	(DBM_DELTA + DBM_Q_MTL_REFUND_PM_TOURN_SAT_WINNERS)  // 8110

// PYR-29620
#define DBM_Q_GET_FULL_USER                     7111
#define DBM_A_GET_FULL_USER	                    (DBM_DELTA + DBM_Q_GET_FULL_USER)  // 8111

//	PYR-29811
#define DBM_Q_SET_USER_MANUAL_PROMO				7112 	//4ss8 - reqId userId promoName appLoginId
#define DBM_A_SET_USER_MANUAL_PROMO				(DBM_DELTA + DBM_Q_SET_USER_MANUAL_PROMO) // 8112 	// 46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_ALL_APPROVED_GAME_LIMITS      7113 // 444s - reqId, userIntId
#define DBM_A_GET_ALL_APPROVED_GAME_LIMITS      (DBM_DELTA + DBM_Q_GET_ALL_APPROVED_GAME_LIMITS) // 8113 // 46[s|4<44s44bt>] - reqId, errCode[errDescr| size<eTableLimitType AmountInCurrency autoApproveHours waitingIncreaseLimitDays hadPrevLimitUK grantedTime] 

#define DBM_Q_SET_GAME_LIMITS2                  7114 // 4s844<44s> - reqId userId appLoginId,adminIntId sz<limitType, limit, currency>
#define DBM_A_SET_GAME_LIMITS2                  (DBM_DELTA + DBM_Q_SET_GAME_LIMITS2) // 8114  // 46[s|O4<6|b44>] - reqId, globalErrCode[globalErrDescr|userChipsInfo sz<limType errCode[errDescr|effectiveNow1 autoLimitHour waitingIncreaseLimitDays]>] 

#define DBM_Q_GET_USER_TRACKCODES               7115 	//44 reqId, userIntId
#define DBM_A_GET_USER_TRACKCODES               (DBM_DELTA + DBM_Q_GET_USER_TRACKCODES) // 8115 	//46[s|4<s>] reqId, err_code[errDescr|numCodes<trackCode>]

// PYR-30588
#define DBM_Q_SET_USER_PRIV3                    7116  //4s8b reqId userId priv3ToSet action(0=clear,1=set)
#define DBM_A_SET_USER_PRIV3                    (DBM_DELTA + DBM_Q_SET_USER_PRIV3) // 8116  //46[s!8] - reqId,errCode[errDescr!userpriv3] 

// PYR-30471
#define DBM_Q_MIGRATION_UK_CHECK                7117 
#define DBM_A_MIGRATION_UK_CHECK                (DBM_DELTA + DBM_Q_MIGRATION_UK_CHECK) // 8117  

#define DBM_Q_MIGRATION_UK_PERFORM              7118 
#define DBM_A_MIGRATION_UK_PERFORM              (DBM_DELTA + DBM_Q_MIGRATION_UK_PERFORM) // 8118

//PYR-30733
#define DBM_Q_DRC_RH_USER_UNSIT					7119	//4M(rec)4 - reqId,M(struct RatholeManTableData),obligationId
#define DBM_A_DRC_RH_USER_UNSIT					(DBM_DELTA + DBM_Q_DRC_RH_USER_UNSIT)	// 8119 	//46 - reqId,errCode=0

// PYR-30661
#define DBM_Q_MIGRATION_BG_CHECK                7120 
#define DBM_A_MIGRATION_BG_CHECK                (DBM_DELTA + DBM_Q_MIGRATION_BG_CHECK) // 8120

#define DBM_Q_MIGRATION_BG_PERFORM              7121 
#define DBM_A_MIGRATION_BG_PERFORM              (DBM_DELTA + DBM_Q_MIGRATION_BG_PERFORM) // 8121

//	PYR-28732
#define DBM_Q_DELETE_CHALLENGE_QUESTIONS		7122	//	4448 - reqId, userIntId, admintIntId, appLoginId
#define DBM_A_DELETE_CHALLENGE_QUESTIONS		(DBM_DELTA + DBM_Q_DELETE_CHALLENGE_QUESTIONS)	//	8122	//	46 - reqId,errCode

// PYR-28735
#define DBM_Q_GET_REAUTH_LOGIN_DATA				7124	// 4s48 - reqId, userId, thresholdLoginId
#define DBM_A_GET_REAUTH_LOGIN_DATA				(DBM_DELTA + DBM_Q_GET_REAUTH_LOGIN_DATA)	//	8124	//	42[s|s4<s>4<s>4<s>4<s>] - reqId,errCode,[errDescr|userId, size<ipAddress>, size<installId>, size<macAddr>, size<routerAddr>]

// PYR-26675: itAcctGateway <-> logindbm
#define DBM_Q_LDBM_UPDATE_PGAD_DOWN_STATUS		7125	//4b - reqId, isPGADDown
#define DBM_A_LDBM_UPDATE_PGAD_DOWN_STATUS		(DBM_DELTA + DBM_Q_LDBM_UPDATE_PGAD_DOWN_STATUS)	// 8125 	//4 - reqId

// PYR-28735
#define DBM_Q_EVAL_USER_REAUTH_METHODS			7126	// 4s - reqId, userId
#define DBM_A_EVAL_USER_REAUTH_METHODS			(DBM_DELTA + DBM_Q_EVAL_USER_REAUTH_METHODS)	// 8126 // 42[s|bbb] - reqId,errCode,[errDescr|existsChallengeQuestions, existsSMSValidation, exists4CardDigits]

// PYR-30831
#define DBM_Q_GET_USER_PWD_CHANGE_INFO			7127	// 44 - reqId, userIntId
#define DBM_A_GET_USER_PWD_CHANGE_INFO			(DBM_DELTA + DBM_Q_GET_USER_PWD_CHANGE_INFO)	// 8127 // 42[s|{PasswordChangeInfo}] - reqId,errCode,[errDescr|{PasswordChangeInfo}]

// PYR-30473
#define DBM_Q_GET_CUMULATIVE_DEPOSIT			7128  // 44ss4 - reqId, transId, userId, paysystem, numbHours
#define DBM_A_GET_CUMULATIVE_DEPOSIT			(DBM_DELTA + DBM_Q_GET_CUMULATIVE_DEPOSIT)  // 8128  // 46[s|4] - reqId, errCode, [errDesc|amount] 

// PYR-30475
#define DBM_Q_GET_SELFEXCLUDED_INFO  			7129  // 44 - reqId, userIntId
#define DBM_A_GET_SELFEXCLUDED_INFO				(DBM_DELTA + DBM_Q_GET_SELFEXCLUDED_INFO)    // 8129

// PYR-30831
#define DBM_Q_GET_USER_CHIPS_INFO				7130	// 44 - reqId, userIntId
#define DBM_A_GET_USER_CHIPS_INFO				(DBM_DELTA + DBM_Q_GET_USER_CHIPS_INFO)			// 8130 // 42[s|{UserChipsInfo}] - reqId,errCode,[errDescr|{UserChipsInfo}]

// PYR-30831
#define DBM_Q_GET_USER_CQ_CHANGE_INFO			7131	// 44 - reqId, userIntId
#define DBM_A_GET_USER_CQ_CHANGE_INFO			(DBM_DELTA + DBM_Q_GET_USER_CQ_CHANGE_INFO)		// 8131 // 42[s|tt] - reqId,errCode,[errDescr|userRegisteredTime,reminderRefusedTime]

// PYR-30475
#define DBM_Q_CONVERT_FUNDS_TO_ONE_ACCT			7132	// 4s - reqId, userId
#define DBM_A_CONVERT_FUNDS_TO_ONE_ACCT			(DBM_DELTA + DBM_Q_CONVERT_FUNDS_TO_ONE_ACCT)	// 8132

// mtobby to RO DBM
#define DBM_Q_GET_RELATED_USERS_GROUPS_BY_USERINTID	7133  // 44<4> - reqId, numUsers<userIntId>
#define DBM_A_GET_RELATED_USERS_GROUPS_BY_USERINTID	(DBM_DELTA + DBM_Q_GET_RELATED_USERS_GROUPS_BY_USERINTID) // 8133  // 46[s|4<ss4>] - reqId, errCode[errDescr|numUsersGroups< userId, groupId, severity] 

#define DBM_Q_SET_CARDINFO_FLAGS_MASK			7134	// 444 - reqId, infoId, flagsToRaise
#define DBM_A_SET_CARDINFO_FLAGS_MASK			(DBM_DELTA + DBM_Q_SET_CARDINFO_FLAGS_MASK)		// 8134

//PYR-26837
#define DBM_Q_GET_CASHTRANS_BY_XTRANSSTR        7135  //444ss reqId, userIntId, secondsAgo, xTransStr, paysystem
#define DBM_A_GET_CASHTRANS_BY_XTRANSSTR        (DBM_DELTA + DBM_Q_GET_CASHTRANS_BY_XTRANSSTR) // 8135  //46[s|m] reqId, err_code, [errDescr|transbody] 


// PYR-30436
#define DBM_Q_MTL_UPDATE_FLIGHT_STATS_PROP		7136 // 44O - reqId, tournId, O(flightInfo)
#define DBM_A_MTL_UPDATE_FLIGHT_STATS_PROP		(DBM_DELTA + DBM_Q_MTL_UPDATE_FLIGHT_STATS_PROP) // 8136 // 46[s|O] - reqId, errCode[errDescr|O(newFlightInfo)]

// PYR-30436
#define DBM_Q_GET_FTM_DATA						7137 // 44 - reqId, tournId
#define DBM_A_GET_FTM_DATA						(DBM_DELTA + DBM_Q_GET_FTM_DATA) // 8137 // 46[s|7] - reqId, errCode[errDescr|prizePool]

// PYR-30436
#define DBM_Q_GET_TARGET_USER_REENTRIES			7138 // 44 - reqId, tournId, targetId
#define DBM_A_GET_TARGET_USER_REENTRIES			(DBM_DELTA + DBM_Q_GET_TARGET_USER_REENTRIES) // 8138 // 46[s|O] - reqId, errCode[errDescr|O(targetReentries)]

// PYR-30436
#define DBM_Q_UPDATE_FLIGHT_TARGET_CANCEL		7139 // 444<447> - reqId, targetId, numUsers<userIntId, flightFlags, newNumReentries>
#define DBM_A_UPDATE_FLIGHT_TARGET_CANCEL		(DBM_DELTA + DBM_Q_UPDATE_FLIGHT_TARGET_CANCEL) // 8139 // 46[s] - reqId, errCode[errDescr]

// PYR-30443
#define DBM_Q_MTL_SET_ACTIVE_TOURN_PROPERTY     7140 	//444s - reqId,tournId,propId,propValue
#define DBM_A_MTL_SET_ACTIVE_TOURN_PROPERTY     (DBM_DELTA + DBM_Q_MTL_SET_ACTIVE_TOURN_PROPERTY) // 8140 	//46[s] - reqId,errCode,[errDescr] 

// PYR-28388 
#define DBM_Q_ADD_USBLACKLIST_RECORD_CHECK      7141 	//
#define DBM_A_ADD_USBLACKLIST_RECORD_CHECK      (DBM_DELTA + DBM_Q_ADD_USBLACKLIST_RECORD_CHECK)      // 8141

#define DBM_Q_ADD_USBLACKLIST_RECORD_PERFORM    7142 	//
#define DBM_A_ADD_USBLACKLIST_RECORD_PERFORM    (DBM_DELTA + DBM_Q_ADD_USBLACKLIST_RECORD_PERFORM)    // 8142

#define DBM_Q_DELETE_NJBLACKLIST_RECORD_CHECK   7143 	//
#define DBM_A_DELETE_NJBLACKLIST_RECORD_CHECK   (DBM_DELTA + DBM_Q_DELETE_NJBLACKLIST_RECORD_CHECK)   // 8143

#define DBM_Q_DELETE_NJBLACKLIST_RECORD_PERFORM 7144 	//
#define DBM_A_DELETE_NJBLACKLIST_RECORD_PERFORM (DBM_DELTA + DBM_Q_DELETE_NJBLACKLIST_RECORD_PERFORM) // 8144

// PYR-27243
#define DBM_Q_USER_UPDATE_COUNTRY_STATE_BY_IP     7145 	//4sss - reqId, userId, countryByIp, stateByIp
#define DBM_A_USER_UPDATE_COUNTRY_STATE_BY_IP     (DBM_DELTA + DBM_Q_USER_UPDATE_COUNTRY_STATE_BY_IP) // 8145 	//46[s] - reqId,errCode,[errDescr]

// PYR-30463
#define DBM_Q_GET_FULL_USER_EX                  7146
#define DBM_A_GET_FULL_USER_EX	                (DBM_DELTA + DBM_Q_GET_FULL_USER_EX)  // 8146

#define DBM_Q_GET_USER_FISCAL_ENCR              7147 // 4s - req, userId
#define DBM_A_GET_USER_FISCAL_ENCR	            (DBM_DELTA + DBM_Q_GET_USER_FISCAL_ENCR)  // 8147 46[s|s]  req errCode [errDescr | fiscalEncr]

// PYR-31577
#define DBM_Q_GET_CUMULATIVE_DEPOSIT_PM			7148  // 44ss4 - reqId, transId, userId, paysystem, numbHours
#define DBM_A_GET_CUMULATIVE_DEPOSIT_PM			(DBM_DELTA + DBM_Q_GET_CUMULATIVE_DEPOSIT_PM)  // 8128  // 46[s|4] - reqId, errCode, [errDesc|amount] 

// PYR-31379 - ice
#define DBM_Q_ICE_AWARD_CASH					7149 // 4484s4 - reqId,userIntId,acceptedInstanceId,cashAmount,currency,budgetCodeId
#define DBM_A_ICE_AWARD_CASH					(DBM_DELTA + DBM_Q_ICE_AWARD_CASH) // 8149 46[s|84] - reqId,errCode[errDescr|transId,cashTransId]

//#define DBM_Q_AVAILABLE_MESSAGE      7150 //

//#define DBM_Q_AVAILABLE_MESSAGE    7151 //

// PYR-31811
#define DBM_Q_GET_USERS_RECENT_TRANSACTS_INFO   7152 // 444<4>4<4> reqId scanIntervalInSeconds numUsers<userIntId> numUsPropTypes<eUserPropertyType>
#define DBM_A_GET_USERS_RECENT_TRANSACTS_INFO   (DBM_DELTA + DBM_Q_GET_USERS_RECENT_TRANSACTS_INFO)  // 8152 46[s|4:numUsers <4:userIntId 6:err[s:errDescr|M:UserWithProperties M:RmTransactEssentials(may be empty) M:ContextBody>]]

//#define DBM_Q_AVAILABLE_MESSAGE      7153 //

//	PYR-28735.  From admin.  Set/clear USERS.FLAGS2 accountRevalidationReqdFlag2 bit
#define DBM_Q_SET_ACCT_REVALIDATION				7154	//	4sbs - reqId,userId,setOrClearFlag,comment
#define DBM_A_SET_ACCT_REVALIDATION				(DBM_DELTA + DBM_Q_SET_ACCT_REVALIDATION)	//	8154	46[s] - reqId,errCode[errDescr]

// PYR-28673
#define DBM_Q_SAVE_ACM_CHATVIOLATIONS           7155
#define DBM_A_SAVE_ACM_CHATVIOLATIONS           (DBM_DELTA + DBM_Q_SAVE_ACM_CHATVIOLATIONS)  // 8155

//	PYR-30714.  From amsAdmin.  Set/clear USERS.FLAG isAffiliate bit
#define DBM_Q_SET_AFFILIATE_FLAG				7156	//	4sb - reqId,userId,setOrClearFlag
#define DBM_A_SET_AFFILIATE_FLAG				(DBM_DELTA + DBM_Q_SET_AFFILIATE_FLAG)		//	8156	46[s]

// PYR-31811
#define DBM_Q_SURRENDER_IT_USER_BALANCE         7157 // 44M reqId userIntId ContextBody(from DBM_A_GET_USERS_RECENT_TRANSACTS_INFO)
#define DBM_A_SURRENDER_IT_USER_BALANCE         (DBM_DELTA + DBM_Q_SURRENDER_IT_USER_BALANCE)  // 8157 46[s|48] req errCode[errDescr | amount transId]

#define DBM_Q_SAVE_ROLL							7158 // 4B - reqId, Roll
#define DBM_A_SAVE_ROLL							(DBM_DELTA + DBM_Q_SAVE_ROLL)	// 8158 46[s|4] - reqId,errCode[errDescr|rollId]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
// PYR-84874 NOTE: if you modify contents of the message in the part before userId please change AdminBrokerProtocols in srvshared also
#define DBM_Q_ISSUE_USERROLL                    7159 // 444s44448s4s - reqId, eRewardIssuer, oldIssuerId32, userId, userIntId, rollId, amount, eUserRollIssueFlags, IssuerId, rewardCurrency("" if RollCurrency), adminIntId, adminId
#define DBM_A_ISSUE_USERROLL					(DBM_DELTA + DBM_Q_ISSUE_USERROLL)	// 8159 46[s|48MM] - reqId,errCode[errDescr|UserRolls.userRollId Transacts.transId M(UserRoll) M(Roll)]

#define DBM_Q_GET_USER_DEPOSIT_CARDS			7160 // 4s4 reqId, userid, userIntId
#define DBM_A_GET_USER_DEPOSIT_CARDS			(DBM_DELTA + DBM_Q_GET_USER_DEPOSIT_CARDS) // 8160 46[4{ss},...] reqId, errCode[erDescr|numRecords,{paysystem,encryptedCardNum}]

// PYR-31590
#define DBM_Q_GET_SINGLE_USER_DATA_BY_INTID     7161  // 44 - reqId, userIntId
#define DBM_A_GET_SINGLE_USER_DATA_BY_INTID		(DBM_DELTA + DBM_Q_GET_SINGLE_USER_DATA_BY_INTID) // 8161  // 44m reqId, errCode, msgData

#define DBM_Q_UPDATE_USERROLL_STATUS			7162 //4444tM(4t) - reqId, userIntId, userRollId, eUserRollAction, newExpiration, MsgBody(oldStatus, oldExpiration)
#define DBM_A_UPDATE_USERROLL_STATUS			(DBM_DELTA + DBM_Q_UPDATE_USERROLL_STATUS)	// 8162 46[s|8] - reqId,errCode[errDescr|Transacts.transId]

// PYR-28673
#define LDBM_Q_UPDATE_USER_CHATBANBYACM			7163 // 441 - reqId, userIntId, chatBanOrNot
#define LDBM_A_UPDATE_USER_CHATBANBYACM			(DBM_DELTA + LDBM_Q_UPDATE_USER_CHATBANBYACM) // 8163 46[s|s4488] - reqId, errCode[erDescr|userId, priv, flags, priv2, priv3]

// PYR-24460
#define DBM_Q_GET_STR_TOURN_DATA				7164 	//44<4> reqId nTourns<tournId> 
#define DBM_A_GET_STR_TOURN_DATA				(DBM_DELTA + DBM_Q_GET_STR_TOURN_DATA) // 8164	//46[s|4<44444444TT>] reqId err_code[errDescr|nTourns<tournId buyIn nPlay addOn nAddOn reBuy nRebuy effAddon startTime closeTime>]

#define DBM_Q_GET_STR_SUSP_PLAYERS				7165 	//444444<4> reqId minPlace maxPlace minPrize minPrizePerBuyin nTour<tournId>
#define DBM_A_GET_STR_SUSP_PLAYERS				(DBM_DELTA + DBM_Q_GET_STR_SUSP_PLAYERS) // 8165	//46[s|4<44<s44T>>] reqId err_code[errDescr|nTourn<tournId nPlay<userId award place regTime>>]

// PYR-32776
#define DBM_Q_UPDATE_TOURNUSER_TUFLAGS			7166	//4444 reqId tournId userIntId tuFlagsBit
#define DBM_A_UPDATE_TOURNUSER_TUFLAGS			(DBM_DELTA + DBM_Q_UPDATE_TOURNUSER_TUFLAGS) // 8166 //42s reqId errcode errstr

//#define DBM_Q_AVAILABLE_MESSAGE			7167 //

// PYR-24460
#define DBM_Q_GET_FIRST_TIME_RM					7168	//44 reqId userIntId
#define DBM_A_GET_FIRST_TIME_RM					(DBM_DELTA + DBM_Q_GET_FIRST_TIME_RM) // 8168 //42[s|T] reqId errcode [errstr|TimeofFirstRM]

#define DBM_Q_GET_EXPIRED_USERROLLS 			7169 // 4 - reqId
#define DBM_A_GET_EXPIRED_USERROLLS 			(DBM_DELTA + DBM_Q_GET_EXPIRED_USERROLLS)	// 8169 46[s|4<UserRollDbEx>] - reqId,errCode[errDescr|vec_size<UserRollDbEx>]

#define DBM_Q_UPDATE_CASHTRANS_CUSTID	 		7170 // 444s - reqId, transId, ctTableId, custId
#define DBM_A_UPDATE_CASHTRANS_CUSTID 			(DBM_DELTA + DBM_Q_UPDATE_CASHTRANS_CUSTID)	// 8170 46[s] - reqId,errCode[errDescr]

// PYR-29932: regulator <-> OLTP
#define DBM_Q_REPORT_USER_EMAIL_UNAUTHORIZED	7171 // 4s reqId, userId
#define DBM_A_REPORT_USER_EMAIL_UNAUTHORIZED 	(DBM_DELTA + DBM_Q_REPORT_USER_EMAIL_UNAUTHORIZED)	// 8171 46[s] - reqId,errCode[errDescr]

// PYR-24460
#define DBM_Q_GET_STR_LOGIN_DATA				7172	//44TTbb reqId, userId, startTime, elimTime, needsPrimaryInstall, needsInstallsIPs 
#define DBM_A_GET_STR_LOGIN_DATA				(DBM_DELTA + DBM_Q_GET_STR_LOGIN_DATA) // 8172 //42[s|4ss4<s>4<s>] reqId errcode [errstr|NLoginsPrim,PrimInstall,SecInstall,nInstalls<IID>,nIPs<IP>]

// PYR-18558
#define DBM_Q_REFILL_USER_PLAY_MONEY			7173
#define DBM_A_REFILL_USER_PLAY_MONEY			(DBM_DELTA + DBM_Q_REFILL_USER_PLAY_MONEY) // 8173

// PYR-24460
#define DBM_Q_GET_RELATED_IID_ACC_CLOSED		7174	//4ss reqId, userId, primInstallId 
#define DBM_A_GET_RELATED_IID_ACC_CLOSED		(DBM_DELTA + DBM_Q_GET_RELATED_IID_ACC_CLOSED) // 8174 //42[s|4<s4>] reqId errcode [errstr|nAccs<userId, closedCategory>]

#define DBM_Q_GET_SHARED_IID_STATUS				7175	//4s4<s> reqId, userId, nInstallIds<installId> 
#define DBM_A_GET_SHARED_IID_STATUS				(DBM_DELTA + DBM_Q_GET_SHARED_IID_STATUS) // 8175 //42[s|b] reqId errcode [errstr|isThereAnyIIDshared]

#define DBM_Q_GET_NPLAYERS_MTT_SHARE_IP_IID		7176	//4s44<s>4<s> reqId, userId, tournId, nIPs<IP>, nInst<installId>
#define DBM_A_GET_NPLAYERS_MTT_SHARE_IP_IID		(DBM_DELTA + DBM_Q_GET_NPLAYERS_MTT_SHARE_IP_IID) // 8176 //42[s|44] reqId errcode [errstr|NSharedIPs NSharedInstalls]

#define DBM_Q_GET_NUMB_SECUSER_LOGINS			7177	//4ss reqId, userId, primInstallId 
#define DBM_A_GET_NUMB_SECUSER_LOGINS			(DBM_DELTA + DBM_Q_GET_NUMB_SECUSER_LOGINS) // 8177 //42[s|s4] reqId errcode [errstr|secUserName nSecUserLogins]

// PYR-33444
#define DBM_Q_SET_TGM_PROPERTY					7178	//4 reqId 
#define DBM_A_SET_TGM_PROPERTY					(DBM_DELTA + DBM_Q_SET_TGM_PROPERTY) // 8178 //42[s|] reqId errcode [errstr|]

// PYR-33549
#define DBM_Q_GET_CLIENTINFO_FOR_PERIOD			7179 // 444s - reqId, daysFrom, daysTo, userId
#define DBM_A_GET_CLIENTINFO_FOR_PERIOD			(DBM_DELTA + DBM_Q_GET_CLIENTINFO_FOR_PERIOD)	// 8179 42[s] - reqId,errCode[errDescr]

#define DBM_QG_CREDIT_SPP						7180 // QG_common:4-req,8-transId,s-obj,t-time + bisness part: 4888 - userIntId, deltaSpp, deltaFpp, objectId
#define DBM_AG_CREDIT_SPP						(DBM_DELTA + DBM_QG_CREDIT_SPP) // 8180 46[s|8] reqId errCode[errDescr | transId]

//	PYR-29932
#define DBM_Q_SET_EMAIL_VALIDATION_FLAGS		7181	//	4sbbbb - reqId,userId,emailVerified,emailAutoValidatedFlag2,emailAlwaysValidateFlag2,emailUnauthorizedFlag2
#define DBM_A_SET_EMAIL_VALIDATION_FLAGS		(DBM_DELTA + DBM_Q_SET_EMAIL_VALIDATION_FLAGS)		//	8181	46[s] reqId errcode[errDescr]

// PYR-30855
#define DBM_Q_GET_FIRSTDEP_MAA_DATA_PM          7182  // 4ss444<s> - reqId, userId, paysystem, transId, minutesSince1stDeposit nRelated <relatedId>
#define DBM_A_GET_FIRSTDEP_MAA_DATA_PM          (DBM_DELTA + DBM_Q_GET_FIRSTDEP_MAA_DATA_PM) // 8182  // 46[s|444] - reqId, errCode, [errDesc|ruleType,minAmount,AccAmount] 

#define DBM_Q_GET_HELD_DEPOSITS_BY_USER         7183  //4s reqId userId
#define DBM_A_GET_HELD_DEPOSITS_BY_USER         (DBM_DELTA + DBM_Q_GET_HELD_DEPOSITS_BY_USER) // 8183  //4[s|4<B>] reqId  errCode[errDescr|numTrans<CashtransEssential>] 

// PYR-33448
#define DBM_Q_GET_FR_USER_LAST_ACTIVITY_DATE    7184	//	
#define DBM_A_GET_FR_USER_LAST_ACTIVITY_DATE	(DBM_DELTA + DBM_Q_GET_FR_USER_LAST_ACTIVITY_DATE) //	8184

#define DBM_Q_CHECK_FR_USER_RECENT_ACTIVITY	    7185	//	
#define DBM_A_CHECK_FR_USER_RECENT_ACTIVITY		(DBM_DELTA + DBM_Q_CHECK_FR_USER_RECENT_ACTIVITY)  //	8185

// PYR-33374
#define DBM_Q_GET_USERS_FOR_INACTIVITY_CHECK_IT	7186  //
#define DBM_A_GET_USERS_FOR_INACTIVITY_CHECK_IT	(DBM_DELTA + DBM_Q_GET_USERS_FOR_INACTIVITY_CHECK_IT) // 8186

//PYR-33857
#define DBM_Q_DK_CLEAR_FLIGHT_CACHE				7187  //44 reqId, tournId
#define DBM_A_DK_CLEAR_FLIGHT_CACHE				(DBM_DELTA + DBM_Q_DK_CLEAR_FLIGHT_CACHE) // 8187

// PYR-33233
#define DBM_Q_SPORTSBOOK_NEED_DATA				7188 // 4 reqId
#define DBM_A_SPORTSBOOK_NEED_DATA				(DBM_DELTA + DBM_Q_SPORTSBOOK_NEED_DATA)  // 8187 46[s|4<s8>]  -  reqId errCode [errDescr|numItems<name,licenseFlags>] uses IniRulesMap::compose/parse

//	PYR-33959
#define DBM_Q_DELETE_USERCONSENTS				7189 //	44 reqid userIntId
#define DBM_A_DELETE_USERCONSENTS				(DBM_DELTA + DBM_Q_DELETE_USERCONSENTS)		//	8189	//	46[s] - reqId errCode[errDescr]

// PYR-38255
#define DBM_Q_ICE_AWARD_FPP						7190 // 44844 - reqId,userIntId,acceptedInstanceId,fppAmount,budgetCodeId
#define DBM_A_ICE_AWARD_FPP						(DBM_DELTA + DBM_Q_ICE_AWARD_FPP) // 8190 46[s|8] - reqId,errCode[errDescr|transId]

#define DBM_Q_ICE_AWARD_PLAYMONEY				7191 // 4488 - reqId,userIntId,acceptedInstanceId,playMoneyAmountInCents
#define DBM_A_ICE_AWARD_PLAYMONEY				(DBM_DELTA + DBM_Q_ICE_AWARD_PLAYMONEY) // 8191 46[s|8] - reqId,errCode[errDescr|transId]


#define DBM_Q_XC_BUYIN						7192 // 4O488 reqId XcRequestCommon amount(buyin>0, cancel<0) xcTransId origXcTransId(!=0 for cancel)
#define DBM_A_XC_BUYIN						(DBM_DELTA + DBM_Q_XC_BUYIN) // 8192 same as DBM_A_XC_START_GAME

// PYR-26675: itAcctGateway <-> logindbm
#define DBM_Q_LDBM_PGAD_TIMEOUT_USER		7193	//44 - reqId, userIntId
#define DBM_A_LDBM_PGAD_TIMEOUT_USER		(DBM_DELTA + DBM_Q_LDBM_PGAD_TIMEOUT_USER)	// 8193 	//	46[s] - reqId errCode[errDescr]

// PYR-31838
#define DBM_Q_GET_USERROLLS_BY_USER			7194 // 44b - reqId,userIntId,includeFinished
#define DBM_A_GET_USERROLLS_BY_USER			(DBM_DELTA + DBM_Q_GET_USERROLLS_BY_USER) // 8194 46[s|UserRolls] - reqId,errCode[errDescr|UserRolls]

//PYR-32003 
#define DBM_Q_MOVE_FLIGHT_USER					7195 //4444 reqId, userIntId, srcTournId
#define DBM_A_MOVE_FLIGHT_USER					(DBM_DELTA + DBM_Q_MOVE_FLIGHT_USER) //8195 46[s] - reqId errCode[errDescr]

//PYR-32003 
#define DBM_Q_GET_STUCK_FLIGHT_USER_DESTINATION	7196 //444<4s> reqId, tournId, numUsers<userIntId,userId>
#define DBM_A_GET_STUCK_FLIGHT_USER_DESTINATION	(DBM_DELTA + DBM_Q_GET_STUCK_FLIGHT_USER_DESTINATION) //8196 46[s|4<4s4>] - reqId errCode[errDescr|numUsers<userIntId,userId,destId>]
//
#define DBM_Q_GET_USERROLLS_ESSENTIALS			7197 // 44 - reqId,userIntId
#define DBM_A_GET_USERROLLS_ESSENTIALS			(DBM_DELTA + DBM_Q_GET_USERROLLS_ESSENTIALS) // 8197 46[s|4<M>] - reqId,errCode[errDescr|size<M(UserRollEssentials)>]

// PYR-42474
#define DBM_Q_MTL_UPDATE_TOURN_DYNAMIC_FLAGS 7198 //4444 - reqId,tournId,flagsToSet,flagsToClear
#define DBM_A_MTL_UPDATE_TOURN_DYNAMIC_FLAGS (DBM_DELTA + DBM_Q_MTL_UPDATE_TOURN_DYNAMIC_FLAGS) // 8198 	//46[s|444] - reqId,errCode,[errDescr|tournId,flagsToSet,flagsToClear] 

//PYR-32003 
#define DBM_Q_UPDATE_NUM_FLIGHT_LEVELS			7199 //444 reqId, tgmId, numLevels
#define DBM_A_UPDATE_NUM_FLIGHT_LEVELS			(DBM_DELTA + DBM_Q_UPDATE_NUM_FLIGHT_LEVELS) //8199 46[s] - reqId errCode[errDescr]

//PYR-34786
#define DBM_Q_CHECK_MIGRATE_TO_DOTCOM           7200 // 4s reqId userId
#define DBM_A_CHECK_MIGRATE_TO_DOTCOM			(DBM_DELTA + DBM_Q_CHECK_MIGRATE_TO_DOTCOM) // 8200
#define DBM_Q_MIGRATE_TO_DOTCOM                 7201 // 4s4 reqId userId eUserMigrationFlags 
#define DBM_A_MIGRATE_TO_DOTCOM					(DBM_DELTA + DBM_Q_MIGRATE_TO_DOTCOM) // 8201

// #define DBM_Q_AVAILABLE_MESSAGE	7202 // 4ttt reqId, startTime, endTime, serverTakenTime

//PYR-33749
#define DBM_Q_SPAIN_GET_SPORTSBOOK_JUA			7203 // 4ttt reqId, startTime, endTime, serverTakenTime
#define DBM_A_SPAIN_GET_SPORTSBOOK_JUA			(DBM_DELTA + DBM_Q_SPAIN_GET_SPORTSBOOK_JUA) // 8203	42[s|vector<{CnjSportsbookJUA}>]
#define DBM_Q_SPAIN_GET_SPORTSBOOK_CEV			7204 // 4ttt reqId, startTime, endTime, serverTakenTime
#define DBM_A_SPAIN_GET_SPORTSBOOK_CEV			(DBM_DELTA + DBM_Q_SPAIN_GET_SPORTSBOOK_CEV) // 8204	42[s|vector<{CnjSportsbookCEV}>]
// for CJ report
#define DBM_Q_SPAIN_GET_USER_SPORTSBOOK_DAILY	7205 // 4ttt reqId, startTime, endTime, serverTakenTime
#define DBM_A_SPAIN_GET_USER_SPORTSBOOK_DAILY	(DBM_DELTA + DBM_Q_SPAIN_GET_USER_SPORTSBOOK_DAILY) // 8205		42[s|vector<{CnjSportsbookCJD}>]

//PYR 33564
#define DBM_Q_MIGRATION_DE_SH_CHECK             7206 // 4s reqId userId
#define DBM_A_MIGRATION_DE_SH_CHECK		        (DBM_DELTA + DBM_Q_MIGRATION_DE_SH_CHECK) // 8206
#define DBM_Q_MIGRATION_DE_SH_PERFORM           7207 // 4s reqId userId 
#define DBM_A_MIGRATION_DE_SH_PERFORM			(DBM_DELTA + DBM_Q_MIGRATION_DE_SH_PERFORM) // 8207

//PYR-33749
#define DBM_Q_CHECK_LIABILITY_COMPLETION		7208 // 4t4 reqId, serverTakenTime, licenseId
#define DBM_A_CHECK_LIABILITY_COMPLETION		(DBM_DELTA + DBM_Q_CHECK_LIABILITY_COMPLETION) // 8208 42[s|4] reqId errCode[errDescr|liabilityTypesCompletedMask]

//#define DBM_Q_AVAILABLE_MESSAGE				7209

//#define DBM_Q_AVAILABLE_MESSAGE				7210

//#define DBM_Q_AVAILABLE_MESSAGE				7211

//	PYR-33647
#define DBM_Q_GET_ACCOUNT_HISTORY				7212	//	44d4s reqId, userIntId, lastLoginDate, days back, cash tran currency
#define DBM_A_GET_ACCOUNT_HISTORY				(DBM_DELTA + DBM_Q_GET_ACCOUNT_HISTORY)	//	8212	42[s|4<8s>4<8s>4<8s>4<8s>] - reqId,errCode[errDescr|buyInCount<amount,currency>,winLossCount<amount,currency>,depositCount<totalDeposits,currency>,cashOutCount<totalCashouts,currency>

//PYR-35064
#define DBM_Q_DESH_GET_USER_INFO				7213 // 444<4> reqId, userIntId, size<userProperty>
#define DBM_A_DESH_GET_USER_INFO				(DBM_DELTA + DBM_Q_DESH_GET_USER_INFO) // 8213  42[s|{DeshOlapUserInfo}] - reqId,errCode[errDescr|DeshOlapUserInfo]

// PYR-35203
#define DBM_Q_UPDATE_USERROLLS_ORDER			7214 //4s444<M(4)> - reqId, adminId, adminIntId, userIntId, numids<M(userRollId)>
#define DBM_A_UPDATE_USERROLLS_ORDER			(DBM_DELTA + DBM_Q_UPDATE_USERROLLS_ORDER)	// 8214 42[s] - reqId,errCode[errDescr]

// PYR-33648
// lobby to RW OLTP
#define DBM_Q_DESH_GET_ACC_VALIDATION                                 7215  //4s - reqId,userId
#define DBM_A_DESH_GET_ACC_VALIDATION                                 (DBM_DELTA + DBM_Q_DESH_GET_ACC_VALIDATION) // 8215  //46[s|<rec>,b] - reqId,errCode,[errDesc|<AccVerification record>] 

// lobby & automation to RW OLTP
#define DBM_Q_DESH_ACC_VALIDATION_DEACTIVATE                          7216  //4s4s - reqId,userId,actionMask,comment
#define DBM_A_DESH_ACC_VALIDATION_DEACTIVATE                          (DBM_DELTA + DBM_Q_DESH_ACC_VALIDATION_DEACTIVATE) // 8216  //46[s] - reqId,errCode,[errDesc] 

// Admin (CDMS) to DBM
#define DBM_Q_DESH_USER_DOCUMENTS_VALIDATED                           7217  // 4s4s - reqId,userId,action(eDocsValidatedAction),comment
#define DBM_A_DESH_USER_DOCUMENTS_VALIDATED                           (DBM_DELTA + DBM_Q_DESH_USER_DOCUMENTS_VALIDATED) // 8217  //46[s] - reqId,errCode,[errDesc] 

#define DBM_Q_DESH_ACC_VALIDATION_START                               7218  //4sb8 reqId,userId,notifyClent,appLoginId
#define DBM_A_DESH_ACC_VALIDATION_START                               (DBM_DELTA + DBM_Q_DESH_ACC_VALIDATION_START) // 8218  //46[s] - reqId, errCode[errDescr] 

#define DBM_Q_GET_ICE_USER_TRACKCODES								7219 // 44 reqId, userIntId
#define DBM_A_GET_ICE_USER_TRACKCODES								(DBM_DELTA + DBM_Q_GET_ICE_USER_TRACKCODES) // 8219  //46[s] - reqId, errCode[errDescr|numCodes<iceTrackCodeId>] 

// PYR-55245
#define DBM_Q_GET_ICE_ISSUED_ACTIVE_TRACKCODES						7220 // 4 reqId
#define DBM_A_GET_ICE_ISSUED_ACTIVE_TRACKCODES						(DBM_DELTA + DBM_Q_GET_ICE_ISSUED_ACTIVE_TRACKCODES) // 8220 // 46[s|4] - reqId, errCode[errDescr|numIssued]

//#define DBM_Q_AVAILABLE_MESSAGE			7221

// PYR-35634, PYR-59489
#define DBM_Q_GET_SINGLE_CURRENCY_LICENSE_HISTORICAL_BALANCE			7222	//	44ts reqId, userIntId, fromTime, currency
#define DBM_A_GET_SINGLE_CURRENCY_LICENSE_HISTORICAL_BALANCE			(DBM_DELTA + DBM_Q_GET_SINGLE_CURRENCY_LICENSE_HISTORICAL_BALANCE)	//	8222	42[s|4] - reqId,errCode[errDescr|balance]  may send errCode=NO_HISTORICAL_BALANCE

//	PYR-33647
#define DBM_Q_INSERT_ACCOUNT_SUMMARY			7223 //44d4<s4444> - reqId, user int id, taken, number of record<currency, buyins, winloss, deposits, cashouts>
#define DBM_A_INSERT_ACCOUNT_SUMMARY			(DBM_DELTA + DBM_Q_INSERT_ACCOUNT_SUMMARY)	// 8223 42[s] - reqId,errCode[errDescr]

//#define DBM_Q_AVAILABLE_MESSAGE			7224

// PYR-35428 login dbm -> sec server
#define DBM_Q_SEC_LOGIN_FAILED_FA				7225	//4s4 - reqId, userid, licenseId
#define DBM_A_SEC_LOGIN_FAILED_FA				(DBM_DELTA + DBM_Q_SEC_LOGIN_FAILED_FA) // 8225	//46[s] reqId, err_code[errDescr]

//#define DBM_Q_AVAILABLE_MESSAGE			7226

//#define DBM_Q_AVAILABLE_MESSAGE			7227

//#define DBM_Q_AVAILABLE_MESSAGE			7228

// PYR-32027
#define DBM_Q_LOBBYETC_NEED_DATA				7229  // 4 reqId
#define DBM_A_LOBBYETC_NEED_DATA				(DBM_DELTA + DBM_Q_LOBBYETC_NEED_DATA) // 8229  //

// PYR-36172
#define DBM_Q_GET_RELATED_IID_ACC_SELFEXCLUDED	7230		// 4ss - reqId, userId, installId
#define DBM_A_GET_RELATED_IID_ACC_SELFEXCLUDED	(DBM_DELTA + DBM_Q_GET_RELATED_IID_ACC_SELFEXCLUDED) // 8230	//42[s|4<s>] reqId, err_code[errDescr|numSelfExcludedUserId<selfExcludedUserId>]

// PYR-36282
#define DBM_Q_GET_EVER_RMOK_USERINTIDS			7231  // 44 - reqId,licenseId
#define DBM_A_GET_EVER_RMOK_USERINTIDS			(DBM_DELTA + DBM_Q_GET_EVER_RMOK_USERINTIDS) // 8231  // 42[s|4<4>] - reqId,errCode[errDesc|size<userIntId>]

#define DBM_Q_UPDATE_SELFEXCLUSION_FR           7232 	//4s444 reqID, userId, newDaysActual, exclusionFlags, seFlags
#define DBM_A_UPDATE_SELFEXCLUSION_FR           (DBM_DELTA + DBM_Q_UPDATE_SELFEXCLUSION_FR) // 8232 	// 46[s] - reqId, errCode[errDescr] 

//#define DBM_Q_AVAILABLE_MESSAGE				7233

//#define DBM_Q_AVAILABLE_MESSAGE				7234

//#define DBM_Q_AVAILABLE_MESSAGE				7235

//#define DBM_Q_AVAILABLE_MESSAGE				7236

//PYR-36282
#define DBM_Q_DESH_GET_MULTIPLE_USERS_INFO			7237 // 44<4>4<4> reqId, size<userIntId>, size<userProperty>
#define DBM_A_DESH_GET_MULTIPLE_USERS_INFO			(DBM_DELTA + DBM_Q_DESH_GET_MULTIPLE_USERS_INFO) // 8237  424<M(2[s|{DeshOlapUserInfo}])> - reqId,errCode=0,size<M(errCodeUser[errDescrUser|DeshOlapUserInfo])>

//#define DBM_Q_AVAILABLE_MESSAGE				7238

// PYR-35097
#define DBM_Q_CASHOUT_AND_SURRENDER_FR_CHECK	7239
#define DBM_A_CASHOUT_AND_SURRENDER_FR_CHECK	(DBM_DELTA + DBM_Q_CASHOUT_AND_SURRENDER_FR_CHECK) // 8239

#define DBM_Q_CASHOUT_AND_SURRENDER_FR			7240
#define DBM_A_CASHOUT_AND_SURRENDER_FR	        (DBM_DELTA + DBM_Q_CASHOUT_AND_SURRENDER_FR) // 8240

#define DBM_Q_GET_AAMS_SPORTSBOOK_HISTORY		7241 // 4s - regId, userId // read only
#define DBM_A_GET_AAMS_SPORTSBOOK_HISTORY		(DBM_DELTA + DBM_Q_GET_AAMS_SPORTSBOOK_HISTORY) // 8241  46[s|4<4M>] - reqId, errCode[errDescr|nTourns<tournId, infoBody>] where infoBody: ss444<M> - AAMSID, AAMSTicket, cost, gwStatus, numReacquisitions<reacquisitionBody> where reacquisitionBody: sT44 - AAMSTicket, when, type, cost

// PYR-81727 tournDBM --> dbmRO
#define DBM_Q_MTL_CREATE_TOURNAMENT_CHECKS		7242 // 4... - reqId, tournScript
#define DBM_A_MTL_CREATE_TOURNAMENT_CHECKS		(DBM_DELTA + DBM_Q_MTL_CREATE_TOURNAMENT_CHECKS) // 8242  46[s|(Script)4<s>4<4>4<4>] - reqId, errCode[errDescr|tournScript, size<obsoleteAdmission>, size<obsoleteAdmissionId>, size<restrictedToAffiliateIntId>]

#define DBM_Q_TOURN_ID							7243 // 4 - reqId
#define DBM_A_TOURN_ID							(DBM_DELTA + DBM_Q_TOURN_ID) // 8243  46[s|4] - reqId, errCode[errDescr|tournId]

// PYR-53789 - nobody sends this message, but it's still here for the future replacement of similar DBM_Q_ALLOWED_AT_TABLE and DBM_Q_ALLOWED_AT_BLITZ messages
#define DBM_Q_ALLOWED_AT_PLAY					7244 // 47M(...) - reqId,type,M(...)
#define DBM_A_ALLOWED_AT_PLAY					(DBM_DELTA + DBM_Q_ALLOWED_AT_PLAY) // 8244 - 46[s...|...] - reqId,errCode[errDescr,...|...]

// PYR-114342 cashier -> dbmRW
#define DBM_Q_INSERT_LIMIT_HIT					7245 // 4o - reqId, LimitHitData
#define DBM_A_INSERT_LIMIT_HIT					(DBM_DELTA + DBM_Q_INSERT_LIMIT_HIT) // 8245 - 42[s] - reqId, errCode[errDesc]

//#define DBM_Q_AVAILABLE_MESSAGE				7246

//#define DBM_Q_AVAILABLE_MESSAGE				7247

//#define DBM_Q_AVAILABLE_MESSAGE				7248

//#define DBM_Q_AVAILABLE_MESSAGE				7249

//PYR-36002 Arjel --> Market Dbm
#define DBM_Q_REPORT_ARJEL_EVENT				7250											// 4TTs4ss4s2 - reqId, reportTime, eventTime, frUserId, userIntId, eventType, confirmationId, flags, eventXml, arjelInstId
#define DBM_A_REPORT_ARJEL_EVENT				(DBM_DELTA + DBM_Q_REPORT_ARJEL_EVENT)			// 8250		// 42[s] - reqId, errCode[errString]

// PYR-104815 Cashier to Dbm
#define DBM_Q_GET_USERID_BY_CHARGEBACKID				7251 // 44 - reqId, cBackId
#define DBM_A_GET_USERID_BY_CHARGEBACKID				(DBM_DELTA + DBM_Q_GET_USERID_BY_CHARGEBACKID)		// 8251 // 42[s|s44] - reqId,err_code[errDescr|userId,userIntId,cashTransId]

#define DBM_Q_GET_ELIGIBLE_BONUSES				7252											// 4s - reqId, userId
#define DBM_A_GET_ELIGIBLE_BONUSES				(DBM_DELTA + DBM_Q_GET_ELIGIBLE_BONUSES)		// 8252

//#define DBM_Q_AVAILABLE_MESSAGE					7253                                         // 4tt - reqId, start_timestamp, end_timestamp //not used

#define DBM_Q_SPAIN_GET_SPORTSBOOK_BOT_MONTHLY 7254 // 4ttt reqId, startTime, endTime, serverTakenTime
#define DBM_A_SPAIN_GET_SPORTSBOOK_BOT_MONTHLY (DBM_DELTA + DBM_Q_SPAIN_GET_SPORTSBOOK_BOT_MONTHLY) // 8254

//#define DBM_Q_AVAILABLE_MESSAGE			7255						// 4ttt - reqId, start_timestamp, end_timestamp, takenTime

//#define DBM_Q_AVAILABLE_MESSAGE			7256  //4bd888888888888888888888888 reqId CnjBotReport

#define DBM_Q_GET_ES_BOT_REPORT_MONTHLY_HISTORY	 7257  //4dd reqId periodStart periodEnd
#define DBM_A_GET_ES_BOT_REPORT_MONTHLY_HISTORY	 (DBM_DELTA + DBM_Q_GET_ES_BOT_REPORT_MONTHLY_HISTORY) // 8257 //46[s|4<bd888888888888888888888888>] reqId errCode [errDescr | numOfReports<CnjBotReport>]

//#define DBM_Q_AVAILABLE_MESSAGE			7258

// PYR-36687
#define DBM_Q_SEND_ICE_EVENTS					7259	// 44<4m> - reqId, numUsers<userIntId,icebody>
#define DBM_A_SEND_ICE_EVENTS (DBM_DELTA + DBM_Q_SEND_ICE_EVENTS) // 8259

#define DBM_Q_ADMIN_ADD_USERBONUS_OLAPCHECK     7260 // identical to DBM_Q_ADMIN_ADD_USERBONUS
#define DBM_A_ADMIN_ADD_USERBONUS_OLAPCHECK     (DBM_DELTA + DBM_Q_ADMIN_ADD_USERBONUS_OLAPCHECK) // 8260 identical to DBM_A_ADMIN_ADD_USERBONUS

// PYR-35275
#define DBM_Q_GET_ES_ACCOUNT_HISTORY			7261 // 44TT - reqId, userIntId, from, to
#define DBM_A_GET_ES_ACCOUNT_HISTORY			(DBM_DELTA + DBM_Q_GET_ES_ACCOUNT_HISTORY) // 8261 // 46[s|4s88888888] - reqId errCode [errDescr | locale, fiscalCode, totalDeposits, totalWithdrawal, totalCasinoSportsBets, totalStakedPokerCash, totalTournEntries, totalBonuses, totalWinnings, balance]

// PYR-34163
#define DBM_Q_GET_REWARD_DETAILS_FOR_ICE_LOBBY	7262 // 4 reqId
#define DBM_A_GET_REWARD_DETAILS_FOR_ICE_LOBBY	(DBM_DELTA + DBM_Q_GET_REWARD_DETAILS_FOR_ICE_LOBBY)  // 8262 42[s|4<M(4s444stt4m)>] reqId errCode [errDescr|size<M(rollId,rollName,status,flags,issuersMask,rollCurrency,validFrom,validUntil,gameMinBet,licensesMaskEx)>]

// PYR-35600 - retrieve userintid by userid (search in normalized cache)
#define DBM_Q_GET_USERINTID_BY_NORMALIZED_ID    7263  // 44<s> - reqId, userId
#define DBM_A_GET_USERINTID_BY_NORMALIZED_ID    (DBM_DELTA + DBM_Q_GET_USERINTID_BY_NORMALIZED_ID) // 8263  // 46[s|4s] - reqId,errCode[errDescr|userIntId, originalUserId>] 

//#define DBM_Q_AVAILABLE_MESSAGE		7264

//#define DBM_Q_AVAILABLE_MESSAGE		7265

// PYR-37213
#define DBM_Q_GET_DEPOSIT_LIMIT_INCREASE		7266 // 4s4 - reqId, userId, daysBack
#define DBM_A_GET_DEPOSIT_LIMIT_INCREASE		(DBM_DELTA + DBM_Q_GET_DEPOSIT_LIMIT_INCREASE) // 8266  // 46[s|4] - reqId errCode[errDescr|depLimitIncrease]

// PYR-36574
#define DBM_Q_SPAIN_GET_SPORTSBOOK_JUT_JUD		7267 // 4{m} - reqId, {Sportsbook SBDB_Q_OLAP_BET_CONCLUSION_INTERCEPT OLAP message}
#define DBM_A_SPAIN_GET_SPORTSBOOK_JUT_JUD		(DBM_DELTA + DBM_Q_SPAIN_GET_SPORTSBOOK_JUT_JUD) // 8267  // 46[s|b[{CnjSportsbookJUT}{CnjSportsbookJUD}]] - reqId, errCode [errDescr | hasReport [{CnjSportsbookJUT} {CnjSportsbookJUD}]]

// PYR-584524
#define DBM_Q_SET_ONE_BATCH_USER_PRIVILEGES		7268 	// 4b4884 - req_id, toSet, new_priv, new_priv2, new_priv3, userId
#define DBM_A_SET_ONE_BATCH_USER_PRIVILEGES		(DBM_DELTA + DBM_Q_SET_ONE_BATCH_USER_PRIVILEGES) // 8268 	// 46[s] - req_id, errCode, [errDescr]

// PYR-32027
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SET_BATCH_PRIVILEGES				7269 	// 4b4884<s> - req_id, toSet, new_priv, new_priv2, new_priv3, numUsers<userId>
#define DBM_A_SET_BATCH_PRIVILEGES				(DBM_DELTA + DBM_Q_SET_BATCH_PRIVILEGES) // 8269 	// 464<6,[s]> - req_id, errCode, numUsers<errDescr, [errDescr]>

// PYR-37510
#define DBM_Q_SET_DFS_FIRST_USAGE				7270 // 4st - reqId, userId, timeStamp
#define DBM_A_SET_DFS_FIRST_USAGE				(DBM_DELTA + DBM_Q_SET_DFS_FIRST_USAGE) // 8270 	// 46[s] reqId, err [errDescr]

// PYR-37487
#define DBM_Q_INSERT_DESH_VAULT_REPORT			7271 //
#define DBM_A_INSERT_DESH_VAULT_REPORT		    (DBM_DELTA + DBM_Q_INSERT_DESH_VAULT_REPORT) // 8271

#define DBM_Q_GET_DESH_VAULT_REPORT				7272 //
#define DBM_A_GET_DESH_VAULT_REPORT				(DBM_DELTA + DBM_Q_GET_DESH_VAULT_REPORT) // 8272

#define DBM_Q_GET_DESH_REPORT_FOR_IMPORT		7273 //
#define DBM_A_GET_DESH_REPORT_FOR_IMPORT		(DBM_DELTA + DBM_Q_GET_DESH_REPORT_FOR_IMPORT) // 8273

#define DBM_Q_FINISH_DESH_REPORT				7274 //
#define DBM_A_FINISH_DESH_REPORT				(DBM_DELTA + DBM_Q_FINISH_DESH_REPORT) // 8274

#define DBM_Q_DESHGW_NEED_DATA					7275 //
#define DBM_A_DESHGW_NEED_DATA					(DBM_DELTA + DBM_Q_DESHGW_NEED_DATA) // 8275

#define DBM_Q_GET_DESH_FILEID					7276 //
#define DBM_A_GET_DESH_FILEID					(DBM_DELTA + DBM_Q_GET_DESH_FILEID) // 8276

//#define DBM_Q_AVAILABLE_MESSAGE			7277 

//#define DBM_Q_AVAILABLE_MESSAGE			7278

//#define DBM_Q_AVAILABLE_MESSAGE			7279

// PYR-37487
#define DBM_Q_GET_VAULT_REPORTING_STATUS		7280 //
#define DBM_A_GET_VAULT_REPORTING_STATUS		(DBM_DELTA + DBM_Q_GET_VAULT_REPORTING_STATUS) // 8280

#define DBM_Q_DESH_VALIDATION_FAILED			7281 //
#define DBM_A_DESH_VALIDATION_FAILED			(DBM_DELTA + DBM_Q_DESH_VALIDATION_FAILED) // 8281

//PYR-35216
#define DBM_Q_GET_LOGIN_ID						7282										// 4s - reqId userId
#define DBM_A_GET_LOGIN_ID						(DBM_DELTA + DBM_Q_GET_LOGIN_ID)			// 8282  // 42[s|8] - reqId errCode [errString|loginId]

// PYR-37487
#define DBM_Q_DESH_UPDATE_XML_REPORT			7283 //
#define DBM_A_DESH_UPDATE_XML_REPORT			(DBM_DELTA + DBM_Q_DESH_UPDATE_XML_REPORT) // 8283

#define DBM_Q_DESH_UPDATE_VAULTIDS				7284 //
#define DBM_A_DESH_UPDATE_VAULTIDS				(DBM_DELTA + DBM_Q_DESH_UPDATE_VAULTIDS) // 8284

#define DBM_Q_POPULATE_DESH_REPORT				7285 //
#define DBM_A_POPULATE_DESH_REPORT				(DBM_DELTA + DBM_Q_POPULATE_DESH_REPORT) // 8285

//#define DBM_Q_AVAILABLE_MESSAGE				7286

// PYR-36189
#define DBM_Q_SET_USER_FLAGS3                   7287  //4s8b4s reqId userId flags3_Mask setNotRemove eUpdateType adminId
#define DBM_A_SET_USER_FLAGS3                   (DBM_DELTA + DBM_Q_SET_USER_FLAGS3) // 8287  //46[s] - reqId, errCode[errDescr] 

//	PYR-37959
#define DBM_Q_ATAS_GET_CANCELLED_CASHOUTS		7288 //	4s4 - reqId userId daysAgo
#define DBM_A_ATAS_GET_CANCELLED_CASHOUTS		(DBM_DELTA + DBM_Q_ATAS_GET_CANCELLED_CASHOUTS)	//	8288	//	46[s|4<4>] - reqId, err [errDescr|numbTran<transId>]

//#define DBM_Q_AVAILABLE_MESSAGE	7289

// PYR-34659
#define DBM_Q_ARJEL_GET_SPORTSBOOK_OFFLINE_EVENTS		7290	// 44tt - reqId, userIntId, startTimeSrv, endTimeSrv
#define DBM_A_ARJEL_GET_SPORTSBOOK_OFFLINE_EVENTS		(DBM_DELTA + DBM_Q_ARJEL_GET_SPORTSBOOK_OFFLINE_EVENTS)		// 8290	// 46[s|4<{m}>] - reqId, errCode [errDescr | count<{ArjelSportsbookOfflineEvent}>]

#define DBM_Q_GET_BG_REPORTABLE_USERS           7291                                     // 44 - reqId LicenseId     PYR-37704
#define DBM_A_GET_BG_REPORTABLE_USERS           (DBM_DELTA + DBM_Q_GET_BG_REPORTABLE_USERS) // 8291  

//	PYR-38019
#define DBM_Q_GET_TOTAL_DEPOSIT					7292 //	4s4 - reqId userId hoursBack
#define DBM_A_GET_TOTAL_DEPOSIT					(DBM_DELTA + DBM_Q_GET_TOTAL_DEPOSIT)	// 8292  // 46[s|4] - reqId, errCode, [errDesc|amount] 

// PYR-34152
#define DBM_Q_GET_PLAYER_POINTS                 7293                                     // 4s - reqId userId
#define DBM_A_GET_PLAYER_POINTS                 (DBM_DELTA + DBM_Q_GET_PLAYER_POINTS)    // 8293 // 42[s|PPP], where P is 'PlayerPointsProgram', so PPP is 'vpp, cpp, spp'

// PYR-35645
#define DBM_Q_GET_DK_SPORSBOOK_REPORT           7294                                        // 4tt - reqId,startTime,endTime
#define DBM_A_GET_DK_SPORSBOOK_REPORT           (DBM_DELTA + DBM_Q_GET_DK_SPORSBOOK_REPORT) // 8294 // 42[s|{m}] - - reqId, errCode [errDescr | {DKSportsBookStatsForPeriod}] 

// PYR-35645 PYR-123065
#define DBM_Q_DK_BET_CONCLUSION                 7295 // 4{m} - reqId, {Sportsbook SBDB_Q_OLAP_BET_CONCLUSION_INTERCEPT OLAP message}
#define DBM_A_DK_BET_CONCLUSION                 (DBM_DELTA + DBM_Q_DK_BET_CONCLUSION) // 8295  // 46[s|4<{DkFastOddsSStruk}>4<{DkFastOddsTStruk}> 8] - reqId, errCode [errDescr |  count<{DkFastOddsSStruk}>, count<{DkFastOddsTStruk}>, TranId]

// PYR-35645 PYR-123065
#define DBM_Q_DK_BET_PLACEMENT                  7296 // 4{m} - reqId, {Sportsbook SBDB_Q_OLAP_BET_PLACEMENT_INTERCEPT OLAP message}
#define DBM_A_DK_BET_PLACEMENT                  (DBM_DELTA + DBM_Q_DK_BET_PLACEMENT) // 8296  // 46[s|4<{DkFastOddsTStruk}> 8] - reqId, errCode [errDescr | count<{DkFastOddsTStruk}>, TranId ]

//#define DBM_Q_AVAILABLE_MESSAGE	7297

//	PYR-38019
#define DBM_Q_GET_RELATED_IID_NEG_BAL			7298 //	4ss ? reqId userId InstallId
#define DBM_A_GET_RELATED_IID_NEG_BAL			(DBM_DELTA + DBM_Q_GET_RELATED_IID_NEG_BAL)	// 8298  //42[s|4<s4>] reqId errcode [errstr|nUsers<userId userIntId>]

// PYR-35734
#define DBM_Q_GET_BONUSES						7299 //	44<4> - reqId, numItemms<bonusId>
#define DBM_A_GET_BONUSES						(DBM_DELTA + DBM_Q_GET_BONUSES)	// 8299 //	 46[s|4<o>] - reqId, errCode [errDescr | count<Bonus>]

// PYR-38831
#define DBM_QS_GET_USER_WITH_PROPS              7300  // 4s44<4>  - reqId userId userIntId vecSize<userPropId>
#define DBM_AS_GET_USER_WITH_PROPS              (DBM_DELTA + DBM_QS_GET_USER_WITH_PROPS) // 8300  // 46[s|B] - reqId errCode[errDescr | UserWithProperties] 

//PYR 38555
#define DBM_Q_MIGRATION_PT_CHECK                7301 // 4s reqId userId
#define DBM_A_MIGRATION_PT_CHECK		        (DBM_DELTA + DBM_Q_MIGRATION_PT_CHECK) // 8301
#define DBM_Q_MIGRATION_PT_PERFORM              7302 // 4sM84 reqId userId migrContextBody appLoginEssentials
#define DBM_A_MIGRATION_PT_PERFORM   			(DBM_DELTA + DBM_Q_MIGRATION_PT_PERFORM) // 8302

// PYR-38554
// regulator to OLAP - do not send it to ROOT - there is no index on USERPROPERTIES.PROPRTYSTR
#define DBM_Q_GET_USERS_BY_PROPERTYSTR          7303  // should be sent only to OLAP !!
#define DBM_A_GET_USERS_BY_PROPERTYSTR          (DBM_DELTA + DBM_Q_GET_USERS_BY_PROPERTYSTR) // 8303

//#define DBM_Q_AVAILABLE_MESSAGE				7304

// PYR-38296
#define DBM_Q_REMOVE_USER_APP_SESSION           7305 // 4s84 - reqId, userId, appSessionId, logoutReason
#define DBM_A_REMOVE_USER_APP_SESSION           (DBM_DELTA + DBM_Q_REMOVE_USER_APP_SESSION)	// 8305 // 42[s] - reqId errCode [errString]

// PYR-39019
#define DBM_Q_UPDATE_USER_NAC					7306  // 4s44 req,userId,oldValue,newValue
#define DBM_A_UPDATE_USER_NAC					(DBM_DELTA + DBM_Q_UPDATE_USER_NAC) // 8306  //46[s]

#define DBM_Q_SAVE_NOCHIPS_TRANSACT		7307  // 4s448sbs8 req,userId,userIntId,transType,objId,comment,isCurValid,currency,objectId2
#define DBM_A_SAVE_NOCHIPS_TRANSACT		(DBM_DELTA + DBM_Q_SAVE_NOCHIPS_TRANSACT) // 8307  //46[s|8] req,errCode[errDescr|transId]

// PYR-38561
#define DBM_Q_GET_PT_MONEY_MOVEMENTS				7308 //
#define DBM_A_GET_PT_MONEY_MOVEMENTS				(DBM_DELTA + DBM_Q_GET_PT_MONEY_MOVEMENTS) // 8308

#define DBM_Q_GET_PT_EXTENDED_LIABILITY			7309 //
#define DBM_A_GET_PT_EXTENDED_LIABILITY			(DBM_DELTA + DBM_Q_GET_PT_EXTENDED_LIABILITY) // 8309

// PYR-39823
#define DBM_Q_GET_CARD_FIRST_USAGE_INFO         7310 // 4s4tt - reqId, userId, userIntId, timeFrom, timeTo
#define DBM_A_GET_CARD_FIRST_USAGE_INFO         (DBM_DELTA + DBM_Q_GET_CARD_FIRST_USAGE_INFO) // 8310 // 46[s|4<sst>]  reqId errCode[errDescr|size<CardFirstUsageInfoData>]

//#define DBM_Q_AVAILABLE_MESSAGE				7311

//	PYR-40426
#define DBM_Q_MG_GET_INIT_DATA				    7312  // 44<4> - reqId,numTypeFilter<typeFilter>
#define DBM_A_MG_GET_INIT_DATA					(DBM_DELTA + DBM_Q_MG_GET_INIT_DATA)  // 8312	// 42[s|4<M(4s48sb44<M(48)>444tts4<4>444)>4<M(42sbb8s)>4<M(4sssssM(44s8)484stt8s4<s>t4t4sM(4s4444)ss)>4<M(4s4444)>4<M(448tt)>] - errCode[errString|numMinigameDefs<M(minigameId,minigameName,minigameType,feeAmount,feeCurrency,isPlayMoney,feeFPPcents,numAccountAdjustments<M(accountId,amount)>,siteMask,brandMask,licenseMask,timeAvailableFrom,timeAvailableTo,minigameDef,numValidGameTicketTypes<validGameTicketTypes>,flags,scriptId,budgetCodeId)>,numAccounts<M(accountId,poolType,currency,isPlayMoney,isFPPs,amountInMillicents,accountName)>,numCatalogPrizes<M(prizeId,catalogName,prizeName,comment,nameResourceId,descriptionResourceId,M(prizeType,referenceId,currency,amount),licenseMask,affinitiesMask,tiersMask,preferredCurrency,startTime,endTime,value,valueCurrency,numCategories<categories>,created,creatorAdminIntId,expired,expiredAdminIntId,assetName,M(prizeLimitId,prizeLimitName,playerLimitAmount,playerLimitDuration,globalLimitAmount,globalLimitDuration),permittedCountries,excludedCountries)>,numPrizeLimits<M(prizeLimitId,prizeLimitName,playerLimitAmount,playerLimitDuration,globalLimitAmount,globalLimitDuration)>,numPrizeLimitsUsage<M(prizeLimitId,userIntId,minigamePlayId,whenAwarded,whenLimitExpires)>]

// PYR-39772, PYR-90648: auxhub->LDBM
#define DBM_Q_LDBM_AUXHUB_RESTART_REQ_DATA		7313  //44<s> - reqId,num <userId>
#define DBM_A_LDBM_AUXHUB_RESTART_REQ_DATA		(DBM_DELTA + DBM_Q_LDBM_AUXHUB_RESTART_REQ_DATA) // 8313  //46[s | 4<M(s4b)>] - reqId, errCode[errDescr | num<M(userId, authFlags, timeLimitReached)>]

// PYR-38803
#define DBM_Q_GET_FULL_USER_LOGIN_INFO        	7314  // 448 - reqId,userIntId,loginId
#define DBM_A_GET_FULL_USER_LOGIN_INFO			(DBM_DELTA + DBM_Q_GET_FULL_USER_LOGIN_INFO) // 8314  //46[s|O] - reqId, errCode[errDescr|FullUserLoginInfo]

// PYR-38726
#define DBM_Q_CREDIT_DEBIT_USER_TCHIPS			7315   // 4s4s4s reqId,userId,amount,currency,budgetCodeId,comment
#define DBM_A_CREDIT_DEBIT_USER_TCHIPS			(DBM_DELTA + DBM_Q_CREDIT_DEBIT_USER_TCHIPS) // 8315  // 46[s] - reqId,errCode,[errDesc]

//#define DBM_Q_AVAILABLE_MESSAGE				7316	//	4bsb4<s> - reqId, isPlaymoney, currency, allowFPPTickets,numAdmissions<admission>

// PYR-38803
#define DBM_Q_GET_DK_CASINO_GAMES_REPORT        7317  // 4tt - reqId,startTime,endTime
#define DBM_A_GET_DK_CASINO_GAMES_REPORT        (DBM_DELTA + DBM_Q_GET_DK_CASINO_GAMES_REPORT) // 8317  // 46[s|O] - reqId, errCode[errDesc|DKCasinoGamesStatsForPeriod] 

// PYR-40504
#define DBM_Q_GET_TICKET						7318   // 44 - reqId,ticketId
#define DBM_A_GET_TICKET						(DBM_DELTA + DBM_Q_GET_TICKET) // 8318  // 46[s|bts4] - reqId,errCode,[errDesc|isUsed,expDate,admissionId,ticketTypeFlags]

// PYR-43886
#define DBM_Q_CONVERT_USER_FPP					7319   // 4s4 reqId,userId,rate
#define DBM_A_CONVERT_USER_FPP					(DBM_DELTA + DBM_Q_CONVERT_USER_FPP) // 8319  // 46[s|84] - reqId,errCode,[errDesc|fpp,chps]

// PYR-99890
#define DBM_Q_GET_SEC_CONFIG					7320   // 4s4 reqId,userId,rate
#define DBM_A_GET_SEC_CONFIG					(DBM_DELTA + DBM_Q_GET_SEC_CONFIG) // 8320

//PYR-88293
#define DBM_Q_MG_INSERT_FIXED_PRIZE_DRAW		7321  // 484444 - reqId,minigamePlayId,minigameId,prizeNum,prizeIndex,drawNum
#define DBM_A_MG_INSERT_FIXED_PRIZE_DRAW		(DBM_DELTA + DBM_Q_MG_INSERT_FIXED_PRIZE_DRAW) // 8321  // 42[s] - reqId,errCode[errString]

// PYR-37300
#define DBM_Q_MG_GET_HISTORICAL_PLAYS			7322	// 4tt - reqId,startTime,endTime
#define DBM_A_MG_GET_HISTORICAL_PLAYS			(DBM_DELTA + DBM_Q_MG_GET_HISTORICAL_PLAYS) // 8322	// 46[s|4<M(84s48ttsM(74s9)f849)>] - reqId,errCode[errString|numMinigamePlays<M(minigamePlayId,userIntId,userId,minigameId,minigamePlayId_Source,minigameStartTime,minigameEndTime,playHistory,M(prizeType,referenceId,currency,amount),isPlayMoney,appLoginId,brandId,jackpotCurrentTotal)>]

// PYR-37300
#define DBM_Q_MG_ADD_ACCOUNT					7323	// 446sfft4 - reqId,accountId,poolType,currency,isPlayMoney,isFPPs,timeCreated,adminIntId
#define DBM_A_MG_ADD_ACCOUNT					(DBM_DELTA + DBM_Q_MG_ADD_ACCOUNT)	//8323	// 46[s] - reqId,errCode[errString]

// PYR-37300
#define DBM_Q_MG_GET_JACKPOT_SHARE_WINNERS		7324	// 48 - reqId,minigamePlayId
#define DBM_A_MG_GET_JACKPOT_SHARE_WINNERS		(DBM_DELTA + DBM_Q_MG_GET_JACKPOT_SHARE_WINNERS)	// 8324	// 46[s|4<M(84tM(74s9))>] - reqId,errCode[errString|numJackpotShares<M(minigamePlayId,userIntId,jackpotTime,M(prizeType,referenceId,currency,amount))>]

// PYR-37300
#define DBM_Q_MG_START_GAME						7325	// 484s48t84s9fffsM(74s9)94<M(49)>6864 - reqId,minigamePlayId,userIntId,userId,minigameId,minigamePlayId_source,minigameStartTime,appLoginId,brandId,feeCurrency,feeAmount,isPlayMoney,isFPPs,requestBrandImages,playHistory,M(prizeType,referenceId,currency,amount),jackpotCurrentTotal,numAccountAdjustments<M(accountId,amount)>,status,gameId,gameIdType,entryNum
#define DBM_A_MG_START_GAME						(DBM_DELTA + DBM_Q_MG_START_GAME)	// 8325	// 46[s|M(4<M(44)>)] - reqId,errCode[errString|M(numImages<M(brand,imageId)>)]

// PYR-37300
#define DBM_Q_MG_END_GAME						7326	// 48tsM(44s8)b4<M(48)>4<M(48)>24s8 - reqId,minigamePlayId,minigameEndTime,playHistory,M(prizeType,referenceId,currency,amount),payPrize,numPaymentAccountAdjustments<M(accountId,amount)>,numOtherAccountAdjustments<M(accountId,amount)>,status,minigameType,minigameName,transId
#define DBM_A_MG_END_GAME						(DBM_DELTA + DBM_Q_MG_END_GAME)	// 8326	// 46[s] - reqId,errCode[errString]

// PYR-37300
#define DBM_Q_MG_PAY_JACKPOT_SHARE				7327	// 484stM(74s9)64<M(49)> - reqId,minigamePlayId,userIntId,userId,jackpotTime,M(prizeType,referenceId,currency,amount),prizeReason,numAccountAdjustments<M(accountId,amount)>
#define DBM_A_MG_PAY_JACKPOT_SHARE				(DBM_DELTA + DBM_Q_MG_PAY_JACKPOT_SHARE)	// 8327	// 46[s] - reqId,errCode[errString]

// PYR-37300
#define DBM_Q_MG_ADD_MINIGAME					7328	// 4t4M(4s48sb44<M(48)>444tts4<4>444) - timeCreated,adminIntId,M(minigameId,minigameName,minigameType,feeAmount,feeCurrency,isPlayMoney,feeFPPcents,numAccountAdjustments<M(accountId,amount)>,siteMask,brandMask,licenseMask,timeAvailableFrom,timeAvailableTo,minigameDef,numValidGameTicketTypes<validGameTicketTypes>,flags,scriptId,budgetCodeId)
#define DBM_A_MG_ADD_MINIGAME					(DBM_DELTA + DBM_Q_MG_ADD_MINIGAME)	// 8328	// 46[s|4] - reqId,errCode[errString|minigameId]

// PYR-37300
#define DBM_Q_MG_UPDATE_MINIGAME				7329	// 4t4M(4s48sb44<M(48)>444tts4<4>444) - timeUpdated,adminIntId,M(minigameId,minigameName,minigameType,feeAmount,feeCurrency,isPlayMoney,feeFPPcents,numAccountAdjustments<M(accountId,amount)>,siteMask,brandMask,licenseMask,timeAvailableFrom,timeAvailableTo,minigameDef,numValidGameTicketTypes<validGameTicketTypes>,flags,scriptId,budgetCodeId)
#define DBM_A_MG_UPDATE_MINIGAME				(DBM_DELTA + DBM_Q_MG_UPDATE_MINIGAME)	// 8329	// 46[s] - reqId,errCode[errString]

// PYR-37300
#define DBM_Q_MG_ADJUST_ACCOUNTS				7330	// 446ffst44<M(49)> - reqId,transId,actionType,isPlayMoney,isFPPs,currency,transferTime,adminIntId,numAccountAdjustments<M(accountId,amount)>
#define DBM_A_MG_ADJUST_ACCOUNTS				(DBM_DELTA + DBM_Q_MG_ADJUST_ACCOUNTS)	// 8330	// 46[s] - reqId,errCode[errString]

// PYR-38474
#define DBM_Q_GET_USER_SURRENDER_ASSETS			7331 // 4s4 - reqId,userId,userIntId
#define DBM_A_GET_USER_SURRENDER_ASSETS			(DBM_DELTA + DBM_Q_GET_USER_SURRENDER_ASSETS) // 8331 // 46[s|MM]bs - reqId,errCode[errDescr|M(tickets&bonuses)M(User),bSurrenderFlag,assetComment]
                                                                                              // User::parseUserFromMaster(M(User)) -> User
// PYR-38474
#define DBM_Q_SURRENDER_USER_ASSETS				7332 // 4s4sMs - reqId,userId,userIntId,transferToId,M(ticket&bonuses),comment // 'M(tickets&bonuses)' comes from 'DBM_A_GET_USER_SURRENDER_ASSETS'
#define DBM_A_SURRENDER_USER_ASSETS				(DBM_DELTA + DBM_Q_SURRENDER_USER_ASSETS)	// 8332 // 46[s|]

#define DBM_Q_CANCEL_USER_PENDING_LIMITS		7333 // pending (loose) requests will be cancelled
#define DBM_A_CANCEL_USER_PENDING_LIMITS		(DBM_DELTA + DBM_Q_CANCEL_USER_PENDING_LIMITS) // 8333 // 46[s|]

// PYR-38561
#define DBM_Q_GET_DATA_FOR_SESSION_REPORT		7334	// 4TT4 - reqId,startTime,endTime,licenseId
#define DBM_A_GET_DATA_FOR_SESSION_REPORT		(DBM_DELTA + DBM_Q_GET_DATA_FOR_SESSION_REPORT) // 8334 // 46[s|4<48T44>] - reqId,errCode[errDescr|numLogEntries<userIntId,loginId,when,logType,devType>]

//#define DBM_Q_AVAILABLE_MESSAGE	7335	// 4TT4 - reqId,startTime,endTime,licenseId

#define DBM_Q_GET_USER_DATA_FOR_FT_CONVERSION   7336  // regulator -> dbmRO : 44s  - reqId userIntId email
#define DBM_A_GET_USER_DATA_FOR_FT_CONVERSION   (DBM_DELTA + DBM_Q_GET_USER_DATA_FOR_FT_CONVERSION) // 8336  // 46[s|BB4<B> ] - reqId errCode[errDescr | UserWithProperties, UserSelfExcludedInfo, <Users by email>] 

// PYR-37300
#define DBM_Q_MG_ALLOCATE_NEXT_PLAYID			7337	// 4 - reqId
#define DBM_A_MG_ALLOCATE_NEXT_PLAYID			(DBM_DELTA + DBM_Q_MG_ALLOCATE_NEXT_PLAYID) // 8337	// 46[s|8] - reqId,errCode[errString|minigamePlayId]

// PYR-37300
#define DBM_Q_MG_ALLOCATE_NEXT_MINIGAMEID		7338	// 4 - reqId
#define DBM_A_MG_ALLOCATE_NEXT_MINIGAMEID		(DBM_DELTA + DBM_Q_MG_ALLOCATE_NEXT_MINIGAMEID) // 8338	// 46[s|4] - reqId,errCode[errString|minigameId]

// PYR-37300
#define DBM_Q_MG_ALLOCATE_NEXT_ACCOUNT_TRANSID	7339	// 4 - reqId
#define DBM_A_MG_ALLOCATE_NEXT_ACCOUNT_TRANSID	(DBM_DELTA + DBM_Q_MG_ALLOCATE_NEXT_ACCOUNT_TRANSID) // 8339	// 46[s|4] - reqId,errCode[errString|transId]

// PYR-40997
#define DBM_Q_LDBM_CHECK_FROZEN_OBJECT			7340	// 44<4s> - reqId,vectorSize<eFrozenObjectType,objectValue>
#define DBM_A_LDBM_CHECK_FROZEN_OBJECT			(DBM_DELTA + DBM_Q_LDBM_CHECK_FROZEN_OBJECT) // 8340 // 42[s]  reqId, errcode [ errstr ]

// PYR-38561
#define DBM_Q_GET_DATA_FOR_SELF_EXCLUDED_REPORT	7341	// 4TT4 - reqId,startTime,endTime,licenseId
#define DBM_A_GET_DATA_FOR_SELF_EXCLUDED_REPORT	(DBM_DELTA + DBM_Q_GET_DATA_FOR_SELF_EXCLUDED_REPORT) // 8341 //

// PYR-39878
#define DBM_Q_GET_FULLTILT_CONVERSION_DATA_BY_INTID		7342 // 44 - reqId, ftPlayerIx
#define DBM_A_GET_FULLTILT_CONVERSION_DATA_BY_INTID		(DBM_DELTA + DBM_Q_GET_FULLTILT_CONVERSION_DATA_BY_INTID) // 8342 46[s| ] - reqId,errCode[errDescr|FullTiltConversionData size<FullTiltMigrationUserTicket> size<FullTiltMigrationUserBonus> FullTiltMigrationPreconsolidatedUserBalance]
#define DBM_Q_GET_FULLTILT_CONVERSION_DATA_BY_STRID		7343 // 4s  - reqId, ftPlayerStrId | e-mail
#define DBM_A_GET_FULLTILT_CONVERSION_DATA_BY_STRID		(DBM_DELTA + DBM_Q_GET_FULLTILT_CONVERSION_DATA_BY_STRID) // 8343 46[s|s44b4s] - reqId,errCode[errDescr|pwdHash salt ftPlayerUserIntId hasMigrated psUserIntId ftUserId]
#define DBM_Q_FULLTILT_CONVERSION_PERFORM				7344 // 444 - reqId,playerix,userIntId
#define DBM_A_FULLTILT_CONVERSION_PERFORM				(DBM_DELTA + DBM_Q_FULLTILT_CONVERSION_PERFORM) // 8344 46[s] - reqId,errCode[errDescr]

// PYR-38484
#define DBM_Q_GET_LOGIN_IP_HISTORY						7345 // 4s4 - reqId, userId, days
#define DBM_A_GET_LOGIN_IP_HISTORY						(DBM_DELTA + DBM_Q_GET_LOGIN_IP_HISTORY) //8345 46[s|4<tss>] - reqId,errCode[errDescr| numOfItems<logingtime, country, ipAddress>]

// PYR-41600
#define DBM_Q_FULLTILT_BLOCK_USER						7346 // 44<4s> - reqId size <ftUserIntId blkcomment>
#define DBM_A_FULLTILT_BLOCK_USER						(DBM_DELTA + DBM_Q_FULLTILT_BLOCK_USER) // 8346 46[s|4<46s>] - reqId errCode[errDescr|numErrors <ftUserIntId errCode errDescr>]  
#define DBM_Q_FULLTILT_UNBLOCK_USER						7347 // 4s4<4> - reqId unblkcomment size <ftUserIntId>
#define DBM_A_FULLTILT_UNBLOCK_USER						(DBM_DELTA + DBM_Q_FULLTILT_UNBLOCK_USER) // 8347 46[s|4<46s>] - reqId errCode[errDescr|numErrors <ftUserIntId errCode errDescr>]

// PYR-39878
#define DBM_Q_GET_MATCHING_USERS_FOR_FT_CONVERSION      7348  // regulator -> dbmRO : 44ss  - reqId pairedUserIntId fiscalcode email
#define DBM_A_GET_MATCHING_USERS_FOR_FT_CONVERSION      (DBM_DELTA + DBM_Q_GET_MATCHING_USERS_FOR_FT_CONVERSION) // 8348  // 46[s|bM4<M>4<M> ] - reqId errCode[errDescr | isPaired, User, size<Users by fiscalCode>, size<Users by email>]

// PYR-43398 regulator to RW OLTP
#define DBM_Q_FR_ACC_VALIDATION_PDC_UPDATE              7349  //4s4s4444s - reqId,userId,userIntId,adminId,adminIntId,eFrAccValPdcTimerAction,pdcActionMaskSet,pdcActionMaskClear,comment
#define DBM_A_FR_ACC_VALIDATION_PDC_UPDATE              (DBM_DELTA + DBM_Q_FR_ACC_VALIDATION_PDC_UPDATE) // 8349  //46[s|O] - reqId,errCode,[errDesc|FrAccountValidation(after update)]

// PYR-41491
#define DBM_Q_LDBM_UPDATE_LOGINDATA_FLAGS               7350  //4s8bss reqId,userId,flagsMask,addOrDrop,adminUserId,comments
#define DBM_A_LDBM_UPDATE_LOGINDATA_FLAGS               (DBM_DELTA + DBM_Q_LDBM_UPDATE_LOGINDATA_FLAGS) // 8350  // 46[s] - reqId,errCode,[errDescr] 

// PYR-38054 dbd -> dbmRO
#define DBM_Q_GET_USER_TOURNTICKETS_ALL                 7351 //4s reqId, userId
#define DBM_A_GET_USER_TOURNTICKETS_ALL                 (DBM_DELTA + DBM_Q_GET_USER_TOURNTICKETS_ALL) // 8351  46[s|bM(4<TournTicketWithTime>)] - reqId errCode[errDescr|hasTickets, M(4<T>)]

// PYR-38054 dbd -> dbm
#define DBM_Q_DELETE_USER_TOURNTICKETS                  7352 //4s4<T> reqId, MessageInfoStruct, userId, M(4<TournTicketWithTime>)
#define DBM_A_DELETE_USER_TOURNTICKETS                  (DBM_DELTA + DBM_Q_DELETE_USER_TOURNTICKETS) // 8352  46[s|]

// PYR-42171
#define DBM_Q_MTL_USERS_OUT_NO_AWARDS_ALREADY_ELIMINATED 7353 //4<rec>4<rec> - reqId,<tournIngo>,numUsers,<userInfo>
#define DBM_A_MTL_USERS_OUT_NO_AWARDS_ALREADY_ELIMINATED (DBM_DELTA + DBM_Q_MTL_USERS_OUT_NO_AWARDS_ALREADY_ELIMINATED) // 8353 //46[s] - reqId,errCode,[errDescr]

// PYR-42449
#define DBM_Q_FULLTILT_REJECT_CONVERSION				7354 // 4444 - reqId ftUserIntId adminIntId errCode
#define DBM_A_FULLTILT_REJECT_CONVERSION				(DBM_DELTA + DBM_Q_FULLTILT_REJECT_CONVERSION) // 8354 46[s] - reqId errCode[errDescr]  

// PYR-41592
#define DBM_Q_GET_MGPLAYS_RECORDS                       7355 	// 4d - reqId, filterDate
#define DBM_A_GET_MGPLAYS_RECORDS                       (DBM_DELTA + DBM_Q_GET_MGPLAYS_RECORDS) // 8355 	// 46[s|4<8>] - reqId, errCode[errDescr] 

#define DBM_Q_DELETE_MGPLAYS_RECORDS                    7356 	// 44<8> - reqId, numIds<id>
#define DBM_A_DELETE_MGPLAYS_RECORDS                    (DBM_DELTA + DBM_Q_DELETE_MGPLAYS_RECORDS) // 8356 	// 46[s] - reqId, errCode[errDescr] 

// PYR-41760   CRM server to OLAP
#define DBM_Q_GET_FPP_HISTORY                           7357  // 4sTT - reqId, userId, from, to
#define DBM_A_GET_FPP_HISTORY                           (DBM_DELTA + DBM_Q_GET_FPP_HISTORY) // 8357  // 46[s|444<T8484444>] - reqId, errCode, 
																     // [errDescr | licenseId, locale, numRecords<when, transId, transType, objectId, fpp, fppAfter, vpp1After, vpp2After>]

// PYR-42121
#define DBM_Q_ATAS_GET_NUM_INSTALLID_MAC                7358  // 4sss4 - reqId,userId,installId,mac,hoursBack
#define DBM_A_ATAS_GET_NUM_INSTALLID_MAC                (DBM_DELTA + DBM_Q_ATAS_GET_NUM_INSTALLID_MAC) // 8358  //42[s|4] - reqId,errCode,[errDescr|count]

// PYR-41350
#define DBM_Q_ADD_CASINO_HANDOVER_RECORD				7359 //
#define DBM_A_ADD_CASINO_HANDOVER_RECORD				(DBM_DELTA + DBM_Q_ADD_CASINO_HANDOVER_RECORD) // 8359

// PYR-38561
#define DBM_Q_INSERT_PT_MONEY_MOVEMENTS					7360 //
#define DBM_A_INSERT_PT_MONEY_MOVEMENTS					(DBM_DELTA + DBM_Q_INSERT_PT_MONEY_MOVEMENTS) // 8360

//	PYR-41017. Regulator to Olap
#define DBM_Q_GET_FULLTILT_EMAIL_INFO					7361	//	4sb - reqId, ftUserIdOrEmail,searchByNameNotEmail
#define DBM_A_GET_FULLTILT_EMAIL_INFO					(DBM_DELTA + DBM_Q_GET_FULLTILT_EMAIL_INFO)		//	8361	//	46[s|bss44s4ssd] - reqId, errCode[errDesc|hasMigrated,userId,email,locale,license,country,ftPlayerIx,firstName,lastName,DOB]

//	PYR-41017. Regulator to Oltp
#define DBM_Q_SET_NON_MIGR_FULLTILT_PWD					7362	//	444s - reqId, ftPlayerIx,pwdSalt,pwdHash
#define DBM_A_SET_NON_MIGR_FULLTILT_PWD					(DBM_DELTA + DBM_Q_SET_NON_MIGR_FULLTILT_PWD)	//	8362	//	46[s] - reqId, errCode[errDesc]

// PYR-41350
#define DBM_Q_GET_CASINO_HANDOVER_RECORDS				7363 //
#define DBM_A_GET_CASINO_HANDOVER_RECORDS				(DBM_DELTA + DBM_Q_GET_CASINO_HANDOVER_RECORDS) // 8363

#define DBM_Q_CHECK_CSR_MONEYRIGHT					7364 // 44s - reqId, amountInCents, currency
#define DBM_A_CHECK_CSR_MONEYRIGHT					(DBM_DELTA + DBM_Q_CHECK_CSR_MONEYRIGHT) // 8364 // 46[s] - reqId, errCode[errDesc]

// PYR-42171
#define DBM_Q_MTL_NEED_DATA_HEADER                      7365                                                   // 44 - q_id, tourn_id
#define DBM_A_MTL_NEED_DATA_HEADER                      (DBM_DELTA + DBM_Q_MTL_NEED_DATA_HEADER)  // 8365      // 46[s|4464(TournamentScript)4<4>(TablesBody)b(I18nPString)4<(I18nPString)>44<4>4T]  - reqId, errCod, [errDescr|tournId,...] 

#define DBM_Q_MTL_NEED_DATA_USERS                       7366                                                   // 444418 - q_id, tourn_id, firstUserIntIdOfRequest, chunkUsersNum, isPlayMoney, tournFlags2
#define DBM_A_MTL_NEED_DATA_USERS                       (DBM_DELTA + DBM_Q_MTL_NEED_DATA_USERS)  // 8366       // 46[s|144<4s44sss44444444s4664T4>]  - reqId, errCod, [errDescr|isLastChunk,nextUserIntIdToRequest,usersNum<...user...>] 

#define DBM_Q_MTL_NEED_DATA_HEADER_TABLEDBM             7367                                                  //4s4<4>b4<s> - reqId,currency,numTickets,<ticketTypeId>,isPlayMoney,numAdmissions,<admission>
#define DBM_A_MTL_NEED_DATA_HEADER_TABLEDBM             (DBM_DELTA + DBM_Q_MTL_NEED_DATA_HEADER_TABLEDBM) // 8367  

#define DBM_Q_MTL_NEED_DATA_USERS_TABLEDBM              7368                                                  //44<4> - reqId,numUsers,<userIntId>
#define DBM_A_MTL_NEED_DATA_USERS_TABLEDBM              (DBM_DELTA + DBM_Q_MTL_NEED_DATA_USERS_TABLEDBM)  // 8368  

// PYR-41650
#define DBM_Q_GET_PT_POKER_DATA							7369 //
#define DBM_A_GET_PT_POKER_DATA							(DBM_DELTA + DBM_Q_GET_PT_POKER_DATA) // 8369

// PYR-41472
#define DBM_Q_GET_NON_DORMANT_ACCOUNTS					7370                                                   // 444<s> - reqId prevDays nUsers<userId>
#define DBM_A_GET_NON_DORMANT_ACCOUNTS					(DBM_DELTA + DBM_Q_GET_NON_DORMANT_ACCOUNTS) // 8370   // 46[s|4<s>] - reqId errCode [errDesc|nUsers<userId>]

// PYR-39894 Regulator -> Olap
#define DBM_Q_GET_USERS_REGISTRATION_BRAND				7371 // 44<4> - reqId, numIds<userIntId>
#define DBM_A_GET_USERS_REGISTRATION_BRAND				(DBM_DELTA + DBM_Q_GET_USERS_REGISTRATION_BRAND) // 8371 // 46[s|4<UserBrandPair>] - reqId, errCode[errDesc|size<UserBrandPair>]

//#define DBM_Q_AVAILABLE_MESSAGE						7373
// Obsolete from PYR-124664
//#define DBM_Q_INSERT_PWD_RESET_AUDIT					7373	//	4 - reqId,struct PwdResetAudit
//#define DBM_A_INSERT_PWD_RESET_AUDIT					(DBM_DELTA + DBM_Q_INSERT_PWD_RESET_AUDIT)	//	8373	//	46[s] - errCode[errDesc]

// PYR-51363 Collusion -> Olap
#define DBM_Q_GET_FREUD_TOURN_USER_DATA 				7374 // reqId tournId userIntId
#define DBM_A_GET_FREUD_TOURN_USER_DATA					(DBM_DELTA + DBM_Q_GET_FREUD_TOURN_USER_DATA) // 8374  // 42[s|FreudTournUserData] reqId, errCode[errDescr|FreudTournUserData]

// PYR-42977
#define DBM_Q_GET_TICKET_TYPES							7375 //44<4> - reqId, recordCount<ticketTypeId>
#define DBM_A_GET_TICKET_TYPES							(DBM_DELTA + DBM_Q_GET_TICKET_TYPES) // 8375 //46[s|4<42>]  - reqId, errCode, [errDescr | recordCount<ticketTypeId, availability>] ;availability=0 - available, other values indicate error codes

// PYR-42977
#define DBM_Q_ADMIN_FINALIZE_TICKETTYPE                 7376 //44 - reqId, ticketTypeId (see struct TicketType in shared.h)
#define DBM_A_ADMIN_FINALIZE_TICKETTYPE                 (DBM_DELTA + DBM_Q_ADMIN_FINALIZE_TICKETTYPE) // 8376 	// 46[s] reqId, errCode [errDescr] 

//PYR-42971
#define DBM_Q_DRC_USER_SITOUT_POOLBLOCK_UPDATE			7377 //4s84444444 - reqId, userId, tableId, blockMinHands, handsPlayed, licenseId, vipStat, minBet, maxBet, BB
#define DBM_A_DRC_USER_SITOUT_POOLBLOCK_UPDATE			(DBM_DELTA + DBM_Q_DRC_USER_SITOUT_POOLBLOCK_UPDATE) // 8377 //46[s] - reqId, errCode, [errDescr]

// PYR-40575
#define DBM_Q_MIGRATION_ROMANIA_CHECK					7378 
#define DBM_A_MIGRATION_ROMANIA_CHECK					(DBM_DELTA + DBM_Q_MIGRATION_ROMANIA_CHECK) // 8378  

#define DBM_Q_MIGRATION_ROMANIA_PERFORM					7379 
#define DBM_A_MIGRATION_ROMANIA_PERFORM					(DBM_DELTA + DBM_Q_MIGRATION_ROMANIA_PERFORM) // 8379

// PYR-43408
#define DBM_Q_XC_HAND_RESTORE							7380
#define DBM_A_XC_HAND_RESTORE							(DBM_DELTA + DBM_Q_XC_HAND_RESTORE) // 8380

// PYR-43027 sec server -> login dbm
#define DBM_Q_SEC_FREEZE_OBJECT                         7381    //44sts4 - reqId, objectType, objectValue, expTime, comment, reason
#define DBM_A_SEC_FREEZE_OBJECT                         (DBM_DELTA + DBM_Q_SEC_FREEZE_OBJECT) // 8381    //46[s|4] reqId, err_code[errDescr|objectId] 

// PYR-41648
#define DBM_Q_GET_PT_SPORTSBOOK_EVENTS				    7382 // 4tt - reqId,startTime,endTime
#define DBM_A_GET_PT_SPORTSBOOK_EVENTS                  (DBM_DELTA + DBM_Q_GET_PT_SPORTSBOOK_EVENTS) // 8382 // 42[s|4<M{PtSportsBookEventMsg}>] - reqId, errCode [errDescr | vector<M{PtSportsBookEventMsg}>]

// PYR-43576 login dbm -> sec server
#define DBM_Q_SEC_NJ_GEOLOC_TOURN						7383	//44484s - reqId, eventType, tournId, tableId, userIntId, userId
#define DBM_A_SEC_NJ_GEOLOC_TOURN						(DBM_DELTA + DBM_Q_SEC_NJ_GEOLOC_TOURN) // 8383	//46[s] reqId, err_code[errDescr]

// PYR-43027 sec server (MLV) -> OLAP
#define DBM_Q_GET_MLV_OVERRIDE_FREEZE					7384	//44ssss - reqId, MLV::eLoginObjectType, installId, IP address, MAC, RouterMAC
#define DBM_A_GET_MLV_OVERRIDE_FREEZE					(DBM_DELTA + DBM_Q_GET_MLV_OVERRIDE_FREEZE) // 42[s|1] reqId, errCode[errDescr|shouldOverride]

// PYR-43576 sec server -> OLAP
#define DBM_Q_GET_TOURN_FROM_TABLE						7385	//44 - reqId, tableId
#define DBM_A_GET_TOURN_FROM_TABLE						(DBM_DELTA + DBM_Q_GET_TOURN_FROM_TABLE) // 8385	//46[s] reqId, err_code[errDescr|tournId]

//	PYR-43867
#define DBM_Q_GET_PT_USER_WINNINGS						7386  // 44t - reqId, userIntId, loginTime
#define DBM_A_GET_PT_USER_WINNINGS						(DBM_DELTA + DBM_Q_GET_PT_USER_WINNINGS) // 8386  //46[s|88] - reqId, err [errDescr| totalPlayed,totalWon] 

// PYR-39403 MGLobby -> TableDBM
#define DBM_Q_MG_UPDATE_GAME							7387	// 48s242M(44s8)b4t8 - reqId,minigamePlayId,playHistory,status,minigameType,prizeNum,M(prizeType,referenceId,currency,amount),payPrize,prizeId,updateTime,transId
#define DBM_A_MG_UPDATE_GAME							(DBM_DELTA + DBM_Q_MG_UPDATE_GAME)	// 8387	// 46[s] - reqId,errCode[errString]

// PYR-43140 lobby -> RO DBM
#define DBM_Q_ROM_ACC_VALIDATION_START					7388  //4sb8 reqId,userId,notifyClent,appLoginId
#define DBM_A_ROM_ACC_VALIDATION_START					(DBM_DELTA + DBM_Q_ROM_ACC_VALIDATION_START) // 8388  //46[s] - reqId, errCode[errDescr]

// PYR-43140 lobby & automation -> RW DBM
#define DBM_Q_ROM_ACC_VALIDATION_DEACTIVATE				7389  //4s4s - reqId,userId,actionMask,comment
#define DBM_A_ROM_ACC_VALIDATION_DEACTIVATE				(DBM_DELTA + DBM_Q_ROM_ACC_VALIDATION_DEACTIVATE) // 8389  //46[s] - reqId,errCode,[errDesc]

// PYR-43140  lobby -> RW DBM
#define DBM_Q_ROM_GET_ACC_VALIDATION					7390  //4s - reqId,userId
#define DBM_A_ROM_GET_ACC_VALIDATION					(DBM_DELTA + DBM_Q_ROM_GET_ACC_VALIDATION) // 8390  //46[s|<rec>,b] - reqId,errCode,[errDesc|<AccVerification record>]

// PYR-42399 to OLAP, not to RO dbm
#define DBM_Q_CHECK_NEW_USER_EMAIL                      7391 // 4s4s - reqId, email, targetLicenseId, installId
#define DBM_A_CHECK_NEW_USER_EMAIL                      (DBM_DELTA + DBM_Q_CHECK_NEW_USER_EMAIL) // 8391  // 46[s|b44] - reqId, errCode[errDescr|duplicateEmailFound,brandId,userIntIdWithDupEmail] 

// PYR-37639
#define DBM_Q_INSERT_NOTCASHINS							7392 // 4s - reqId, paySystem
#define DBM_A_INSERT_NOTCASHINS							(DBM_DELTA + DBM_Q_INSERT_NOTCASHINS) // 8392

#define DBM_Q_DELETE_NOTCASHINS							7393 // 4s - reqId, paySystem
#define DBM_A_DELETE_NOTCASHINS							(DBM_DELTA + DBM_Q_DELETE_NOTCASHINS) // 8393

// PYR-45244
#define DBM_Q_GET_CDA_TOURN_DATA						7394 // reqId tournId
#define DBM_A_GET_CDA_TOURN_DATA						(DBM_DELTA + DBM_Q_GET_CDA_TOURN_DATA) // 8394  // 42[s|CDATournInfo] reqId, errCode[errDescr|CDATournInfo]

// PYR-39446
#define DBM_Q_CREDIT_USERS_WITH_LICENCE                 7395  //44<M(s46)>sss444m reqId numusers<M(userId amount amountCents)>comment currency, paysystem, auditTransType, budgetCodeId, allowedLicenceMask, allowedLicenceMaskEx
#define DBM_A_CREDIT_USERS_WITH_LICENCE                 (DBM_DELTA + DBM_Q_CREDIT_USERS_WITH_LICENCE) // 8395     //46[s|4<M(s46s)>t] reqId err_code [errDescr|numusers<M(userId licenceId err_code errDescr)> time] 

// PYR-43072 to OLAP
#define DBM_Q_XC_JACKPOT_CREATED						7396 // 84s2s844sbb - xcReqId,jackpotKey,name,vendorId,vendorJackpotId,reset,progressionRate,resetRate,currency,isPlayMoney,useResetValue
#define DBM_A_XC_JACKPOT_CREATED						(DBM_DELTA + DBM_Q_XC_JACKPOT_CREATED) // 8396

// PYR-43072 to OLAP
#define DBM_Q_XC_JACKPOT_RESEEDED						7397 // 848sb - xcReqId,jackpotKey,amountReseed,currency,isPlayMoney
#define DBM_A_XC_JACKPOT_RESEEDED						(DBM_DELTA + DBM_Q_XC_JACKPOT_RESEEDED) // 8397

// PYR-45089 jobCtrl -> SPB OLAP
#define DBM_Q_CHECK_FR_USER_SPB_ACTIVITY_FOR_PERIOD     7398 // 44tt - reqId, userIntId, timeFrom, timeTo
#define DBM_A_CHECK_FR_USER_SPB_ACTIVITY_FOR_PERIOD     (DBM_DELTA + DBM_Q_CHECK_FR_USER_SPB_ACTIVITY_FOR_PERIOD) // 8398 46[s|T] reqId, errCode[errDescr|lastActivityTimestamp]

//	PYR-44432. Regulator to Olap
#define DBM_Q_GET_LASTLOGINS							7399 //	44<s> - reqId,numUsers<userId>
#define DBM_A_GET_LASTLOGINS							(DBM_DELTA + DBM_Q_GET_LASTLOGINS)	//	8399	//	46[s|4<st>] - reqId errCode[errDesc|arrSize<userId,lastLogin>]

// PYR-45645 
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_SAVE_GAMETICKETTYPE						7400 // 4B - reqId, GameTicketType
#define DBM_A_SAVE_GAMETICKETTYPE						(DBM_DELTA + DBM_Q_SAVE_GAMETICKETTYPE)	// 8400 46[s|4] - reqId,errCode[errDescr|gameTicketTypeId]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define DBM_Q_ISSUE_GAMETICKET							7401 // 44s444844s - reqId, eRewardIssuer, userId, userIntId, ticketTypeId, eGameTicketIssueFlags, IssuerId, numPlays, adminIntId, adminId
#define DBM_A_ISSUE_GAMETICKET							(DBM_DELTA + DBM_Q_ISSUE_GAMETICKET) // 8401 46[s|88M] - reqId,errCode[errDescr|GameTicket.gameTicketId Transacts.transId M(GameTicket)]

#define DBM_Q_UPDATE_GAMETICKET_STATUS					7402 //4484844 - reqId, userIntId, appSessionId, brandId, gameTicketId, eGameTicketAction, oldStatus
#define DBM_A_UPDATE_GAMETICKET_STATUS					(DBM_DELTA + DBM_Q_UPDATE_GAMETICKET_STATUS) // 8402 46[s|8] - reqId,errCode[errDescr|Transacts.transId]

#define DBM_Q_GET_EXPIRED_GAMETICKETS 					7403 // 4 - reqId
#define DBM_A_GET_EXPIRED_GAMETICKETS 					(DBM_DELTA + DBM_Q_GET_EXPIRED_GAMETICKETS)	// 8403 46[s|4<GameTicketEx>] - reqId,errCode[errDescr|vec_size<GameTicketEx>]

#define DBM_Q_GET_GAMETICKETS_BY_USER					7404 // 44 - reqId,userIntId
#define DBM_A_GET_GAMETICKETS_BY_USER					(DBM_DELTA + DBM_Q_GET_GAMETICKETS_BY_USER) // 8404 46[s|GameTickets] - reqId,errCode[errDescr|GameTickets]

// PYR-44656
#define DBM_Q_XC_TURN_ON_OFF_AUTOCONVERSION				7405
#define DBM_A_XC_TURN_ON_OFF_AUTOCONVERSION				(DBM_DELTA + DBM_Q_XC_TURN_ON_OFF_AUTOCONVERSION) // 8405

// PYR-32587
#define DBM_Q_MTL_WINTHEBUTTON_MODEOVER					7406 // 44 - reqId, tournId
#define DBM_A_MTL_WINTHEBUTTON_MODEOVER					(DBM_DELTA + DBM_Q_MTL_WINTHEBUTTON_MODEOVER) // 8406 46[s] - reqId, errCode[errDescr]

// PYR-44656
#define DBM_Q_XC_CONVERT_FUNDS							7407
#define DBM_A_XC_CONVERT_FUNDS							(DBM_DELTA + DBM_Q_XC_CONVERT_FUNDS) // 8407

// PYR-37705
#define DBM_Q_BG_NEED_PYRDOC_DATA						7408 //
#define DBM_A_BG_NEED_PYRDOC_DATA						(DBM_DELTA + DBM_Q_BG_NEED_PYRDOC_DATA) // 8408

// PYR-45617
#define DBM_Q_MG_VALIDATE_SIDEGAME_ENTRY				7409 // 4s184 - reqId, userId, entryType, gameId, entryId
#define DBM_A_MG_VALIDATE_SIDEGAME_ENTRY				(DBM_DELTA + DBM_Q_MG_VALIDATE_SIDEGAME_ENTRY) // 8409  // 46[s|b] - reqId, errCode[errDescr|success]

// PYR-29824
#define DBM_Q_INSERT_CHARGEBACKPM                       7411 //44Ds442224s[s] reqId, transId, postDate, relatedUser, caseId, amount, reason , status, state, controlflags, xtransstr, reasonStr
#define DBM_A_INSERT_CHARGEBACKPM                      (DBM_DELTA + DBM_Q_INSERT_CHARGEBACKPM) // 8411

// PYR-37705
#define DBM_Q_BG_SAVE_MESSAGE							7412 //
#define DBM_A_BG_SAVE_MESSAGE							(DBM_DELTA + DBM_Q_BG_SAVE_MESSAGE) // 8412

#define DBM_Q_BG_GET_UNREPORTED_USER_REQUESTS			7413 //
#define DBM_A_BG_GET_UNREPORTED_USER_REQUESTS			(DBM_DELTA + DBM_Q_BG_GET_UNREPORTED_USER_REQUESTS) // 8413

#define DBM_Q_BG_SAVE_USER_RESPONSE						7414 //
#define DBM_A_BG_SAVE_USER_RESPONSE						(DBM_DELTA + DBM_Q_BG_SAVE_USER_RESPONSE) // 8414

#define DBM_Q_CHECK_UNIQUE_USERBONUS					7415 // OLAP 44444<4> - reqId, userIntId, migratedFromUserIntId, bonusId, numOfAliases <aliasBonusId>
#define DBM_A_CHECK_UNIQUE_USERBONUS					(DBM_DELTA + DBM_Q_CHECK_UNIQUE_USERBONUS) // 8415 46[s] - reqId, errCode[errDescr]

// PYR-45760
// request from SQL client
#define DBM_Q_SEND_TEXT_FILE							7416 //
#define DBM_A_SEND_TEXT_FILE							(DBM_DELTA + DBM_Q_SEND_TEXT_FILE) // 8416

// PYR-37705
#define DBM_Q_BG_GET_CASINO_DATA						7417 // 4tt - reqId,startTime,endTime
#define DBM_A_BG_GET_CASINO_DATA						(DBM_DELTA + DBM_Q_BG_GET_CASINO_DATA) // 8417 46[s|4<O>] - reqId, errCode[errDescr|size<BgCasinoHandResultMsg>]

#define DBM_Q_BG_GET_POKER_DATA							7418 //
#define DBM_A_BG_GET_POKER_DATA							(DBM_DELTA + DBM_Q_BG_GET_POKER_DATA) // 8418

#define DBM_Q_BG_GET_USER_CHANGES						7419 //
#define DBM_A_BG_GET_USER_CHANGES						(DBM_DELTA + DBM_Q_BG_GET_USER_CHANGES) // 8419

#define DBM_Q_BG_SAVE_USER_REQUESTS						7420 //
#define DBM_A_BG_SAVE_USER_REQUESTS						(DBM_DELTA + DBM_Q_BG_SAVE_USER_REQUESTS) // 8420

// PYR-44930
#define DBM_Q_SAVE_SCHED_BROADCAST_MSG					7421 // 444STSI44b4<4>4<4>4<4>44<S>4<T> - reqId, adminIntId, broadcastID, createdBy, createdTime, lastModifiedBy, message, flags, sites, numTourns<TournID>, numHandTypes<HandType>, numBetStructs<BetStruct>, stakesMask, numCountries<Country>, numDatetimes<datetime>
#define DBM_A_SAVE_SCHED_BROADCAST_MSG					(DBM_DELTA + DBM_Q_SAVE_SCHED_BROADCAST_MSG) // 8421 // 46[s|4T4] - reqId, errCode[errDescr|broadcastId,createdTime,versionNum]

#define DBM_Q_GET_ALL_SCHED_BROADCAST_MSG				7422 // 4 - reqId
#define DBM_A_GET_ALL_SCHED_BROADCAST_MSG				(DBM_DELTA + DBM_Q_GET_ALL_SCHED_BROADCAST_MSG) // 8422 // 46[s|4<M(4STSI44b4<4>4<4>4<4>44<S>4<T>)>] - reqId, errCode[errDescr|numBroadcasts<M(broadcastID, createdBy, createdTime, lastModifiedBy, message, flags, sites, numTourns<TournID>, numHandTypes<HandType>, numBetStructs<BetStruct>, stakesMask, numCountries<Country>, numDatetimes<datetime>)>]

#define DBM_Q_UPDATE_SCHED_BROADCAST_MSG_STATUS			7423 // 442 - reqId, adminIntId, broadcastId, status
#define DBM_A_UPDATE_SCHED_BROADCAST_MSG_STATUS			(DBM_DELTA + DBM_Q_UPDATE_SCHED_BROADCAST_MSG_STATUS) // 8423 // 46[s] - reqId, errCode[errDescr]

// PYR-45808
#define DBM_Q_GET_HISTORICAL_CONVRATES                  7424  // 4t reqId timestamp
#define DBM_A_GET_HISTORICAL_CONVRATES                  (DBM_DELTA + DBM_Q_GET_HISTORICAL_CONVRATES) // 8424 46[s|4<s4>] - reqId, errCode[errDescr|size<currency, rateToUsd*CONV_RATE_BASE>]

// PYR-45888 sec server (MLV) -> OLAP
#define DBM_Q_MLV_GET_ALERT_DATA						7425 // 4* - reqId, {MLV::GetOlapAlertDataInputs}
#define DBM_A_MLV_GET_ALERT_DATA						(DBM_DELTA + DBM_Q_MLV_GET_ALERT_DATA) // 42[s|111444] reqId, errCode[errDescr|shouldOverride isExcludedObject isFrozenObject numOpenAccounts numClosedAccounts numNewAccounts]

// PYR-35827
#define DBM_Q_CANCEL_AVC_CHECK							7426 //4s4 - reqId, userId, userIntId
#define DBM_A_CANCEL_AVC_CHECK							(DBM_DELTA + DBM_Q_CANCEL_AVC_CHECK) // 8426 42[s] reqId, errCode[errDescr]

// PYR-37705
#define DBM_Q_BG_GET_UNREPORTED_GAME_REQUESTS			7427 //
#define DBM_A_BG_GET_UNREPORTED_GAME_REQUESTS			(DBM_DELTA + DBM_Q_BG_GET_UNREPORTED_GAME_REQUESTS) // 8427

#define DBM_Q_BG_SAVE_GAME_RESPONSE						7428 //
#define DBM_A_BG_SAVE_GAME_RESPONSE						(DBM_DELTA + DBM_Q_BG_SAVE_GAME_RESPONSE) // 8428

#define DBM_Q_BG_SAVE_GAME_REQUESTS						7429 //
#define DBM_A_BG_SAVE_GAME_REQUESTS						(DBM_DELTA + DBM_Q_BG_SAVE_GAME_REQUESTS) // 8429

#define DBM_Q_BG_NEED_MARKET_DATA						7430 //
#define DBM_A_BG_NEED_MARKET_DATA						(DBM_DELTA + DBM_Q_BG_NEED_MARKET_DATA) // 8430

// PYR-46739
#define DBM_Q_CASHIER_NEED_DATA_RW                      7431 	// 4 reqId;
#define DBM_A_CASHIER_NEED_DATA_RW                      (DBM_DELTA + DBM_Q_CASHIER_NEED_DATA_RW) // 8431 	// 

// PYR-46739
#define DBM_Q_CASHIER_NEED_DATA_RO                      7432 	// 4 reqId;
#define DBM_A_CASHIER_NEED_DATA_RO                      (DBM_DELTA + DBM_Q_CASHIER_NEED_DATA_RO) // 8432 	// 

// PYR-45645 
#define DBM_Q_GET_GAMETICKETTYPEBYID					7433 // 44 - reqId, GameTicketTypeId
#define DBM_A_GET_GAMETICKETTYPEBYID					(DBM_DELTA + DBM_Q_GET_GAMETICKETTYPEBYID)	// 8433 46[s|M] - reqId,errCode[errDescr|M(GameTicketType)]

// PYR-44166
#define DBM_Q_HAS_ROLLBACK_HISTORY                      7434 	// 4s - reqId userId
#define DBM_A_HAS_ROLLBACK_HISTORY                      (DBM_DELTA + DBM_Q_HAS_ROLLBACK_HISTORY) // 8434 46[s|4] - reqId, errCode[errDescr|firstRollbackTransId]

// PYR-37705
#define DBM_Q_BG_SAVE_USER_REQ_RESP						7435 //
#define DBM_A_BG_SAVE_USER_REQ_RESP						(DBM_DELTA + DBM_Q_BG_SAVE_USER_REQ_RESP) // 8435

// PYR-45645 
#define DBM_Q_SET_GAMETICKETTYPE_BOUND_TO_GAME  		7436 // 444 - reqId, GameTicketTypeId, adminIntId
#define DBM_A_SET_GAMETICKETTYPE_BOUND_TO_GAME  		(DBM_DELTA + DBM_Q_SET_GAMETICKETTYPE_BOUND_TO_GAME  )	// 8436 46[s] - reqId,errCode[errDescr]

// PYR-40872
#define DBM_Q_GET_ROLLS_ESSENTIALS						7437 // 4 reqId
#define DBM_A_GET_ROLLS_ESSENTIALS						(DBM_DELTA + DBM_Q_GET_ROLLS_ESSENTIALS)  // 8437 42[s|4<M(4s444stt4m)>] reqId errCode [errDescr|size<M(rollId,rollName,status,flags,issuersMask,rollCurrency,validFrom,validUntil,gameMinBet,licensesMaskEx)>]

// PYR-45631
#define DBM_Q_GET_BG_USERS_AWATING_REGISTRATION			7438 // 4 - reqId
#define DBM_A_GET_BG_USERS_AWATING_REGISTRATION 		(DBM_DELTA + DBM_Q_GET_BG_USERS_AWATING_REGISTRATION) // 8438 46[s|4<4>] - reqId,errCode[errDescr|vec_size<userIntId>]

#define DBM_Q_GET_GAMETICKETSWITHTYPE_BY_USER			7439 // 44 - reqId,userIntId
#define DBM_A_GET_GAMETICKETSWITHTYPE_BY_USER			(DBM_DELTA + DBM_Q_GET_GAMETICKETSWITHTYPE_BY_USER) // 8439 46[s|4<GameTicketWithType>] - reqId,errCode[errDescr|<GameTicketWithType>]

// PYR-46651
#define DBM_Q_UPDATE_NEXT_HAND_BUTTON_FOR_WINTHEBUTTONTOURN 7440 // 481 - reqId, tableId, buttonPos
#define DBM_A_UPDATE_NEXT_HAND_BUTTON_FOR_WINTHEBUTTONTOURN (DBM_DELTA + DBM_Q_UPDATE_NEXT_HAND_BUTTON_FOR_WINTHEBUTTONTOURN) // 8440 46[s] - reqId,errCode[errDescr]

// PYR-47103
#define DBM_Q_SAVE_PT_XML_REPORT						7441 // 4tsB8d - reqId, creationTime, fileName, compressedXml, originalSize, reportingDate
#define DBM_A_SAVE_PT_XML_REPORT						(DBM_DELTA + DBM_Q_SAVE_PT_XML_REPORT) // 8441

// PYR-47103 PtGW -> MarketDBM
#define DBM_Q_GET_PT_FILEIDS_BY_TIMESTAMP				7442 // 4t - reqId, creationTime
#define DBM_A_GET_PT_FILEIDS_BY_TIMESTAMP				(DBM_DELTA + DBM_Q_GET_PT_FILEIDS_BY_TIMESTAMP) // 8442 46[s|4<4>] - reqId,errCode[errDescr|vec_size<FileID>]

// PYR-47103 PtGW -> MarketDBM
#define DBM_Q_GET_PT_REPORT_BY_FILEID					7443 // 44 - reqId, fileID
#define DBM_A_GET_PT_REPORT_BY_FILEID					(DBM_DELTA + DBM_Q_GET_PT_REPORT_BY_FILEID) // 8443 46[s|ts4B8d] - reqId,errCode[errDescr|creationTime,fileName,version,compressedXml,originalSize,reportingDate]

// PYR-47103 PtGW -> MarketDBM
#define DBM_Q_UPDATE_PT_REPORT							7444 // 4tsB8 - reqId, creationTime, fileName, compressedXml, originalSize
#define DBM_A_UPDATE_PT_REPORT							(DBM_DELTA + DBM_Q_UPDATE_PT_REPORT) // 8444 46[s|B48] - reqId,errCode[errDescr|]

// PYR-47455 PtGW -> ComplianceDBM (RO)
#define DBM_Q_GET_PT_VER_STATUS_FOR_USERS				7445 //
#define DBM_A_GET_PT_VER_STATUS_FOR_USERS				(DBM_DELTA + DBM_Q_GET_PT_VER_STATUS_FOR_USERS) // 8445

// PYR-47543 Admin -> dbm RW
#define DBM_Q_MODIFY_USER_DUE_DILIGENCE                 7446 // 4s4bs reqId userId userIntId setNotRemove comment
#define DBM_A_MODIFY_USER_DUE_DILIGENCE                 (DBM_DELTA + DBM_Q_MODIFY_USER_DUE_DILIGENCE) // 8446 46[s]

#define DBM_Q_CLEANUP_DEAD_TOURN						7447 //44t1sbb - reqId, tournid, closed, status, canceller, isMultiDbm, cleanupAllocated
#define DBM_A_CLEANUP_DEAD_TOURN						(DBM_DELTA + DBM_Q_CLEANUP_DEAD_TOURN) // 8447

#define DBM_Q_CLEANUP_ALLOCATED_FOR_TOURN				7448 //444 - reqId,tournId,dbmId
#define DBM_A_CLEANUP_ALLOCATED_FOR_TOURN				(DBM_DELTA + DBM_Q_CLEANUP_ALLOCATED_FOR_TOURN) // 8448

#define DBM_Q_CLEANUP_DEAD_ZOOM							7449  //447fs7s - reqId,blitzId,scalePM,creditStuckUsers,adminId,handType,blitzName
#define DBM_A_CLEANUP_DEAD_ZOOM							(DBM_DELTA + DBM_Q_CLEANUP_DEAD_ZOOM) // 8449  //46[s] - reqId,errCode,[errDesc] 

// PYR-38597
#define DBM_Q_USER_REGISTERED_IN_SATELLITE_TARGET		7450 // 484 - reqId, tournId, userIntId
#define DBM_A_USER_REGISTERED_IN_SATELLITE_TARGET		(DBM_DELTA + DBM_Q_USER_REGISTERED_IN_SATELLITE_TARGET) // 8450  // 46[s|b] - reqId, errCode[errDescr|isRegistered]

//	PYR-47887. mtLobby -> TournDbm
#define DBM_Q_MTL_DEGAS_TOURN_FINISHED						7451	//	48tO - reqId,tournId,lastEliminated,OlapUtcTime
#define DBM_A_MTL_DEGAS_TOURN_FINISHED						(DBM_DELTA + DBM_Q_MTL_DEGAS_TOURN_FINISHED)	//	8451	//	46[s] - reqId,errCode,[errDesc] 

// PYR-47982
#define DBM_Q_TRY_CREATE_CURRENCY_ACCOUNT				7452  // 44s48 - reqId, userIntId, newAccountCurrency, transType, appLoginId
#define DBM_A_TRY_CREATE_CURRENCY_ACCOUNT				(DBM_DELTA + DBM_Q_TRY_CREATE_CURRENCY_ACCOUNT) // 8452  // 42[s] reqId, errCode [errStr] 

// PYR-48107
#define DBM_Q_FETCH_CASHTRANS4UPD						7453  // 44<4>s - reqId, size<purchaseId>,userId
#define DBM_A_FETCH_CASHTRANS4UPD						(DBM_DELTA + DBM_Q_FETCH_CASHTRANS4UPD) // 8453  // 42[s] reqId, errCode [errStr]

// PYR-47587 lobby -> RO DBM
#define DBM_Q_CZ_ACC_VALIDATION_START					7454  //4sb8 reqId,userId,notifyClent,appLoginId
#define DBM_A_CZ_ACC_VALIDATION_START					(DBM_DELTA + DBM_Q_CZ_ACC_VALIDATION_START) // 8454  //46[s] - reqId, errCode[errDescr]

// PYR-47587 lobby & automation -> RW DBM
#define DBM_Q_CZ_ACC_VALIDATION_DEACTIVATE				7455  //4s4s - reqId,userId,actionMask,comment
#define DBM_A_CZ_ACC_VALIDATION_DEACTIVATE				(DBM_DELTA + DBM_Q_CZ_ACC_VALIDATION_DEACTIVATE) // 8455  //46[s] - reqId,errCode,[errDesc]

// PYR-47587  lobby -> RW DBM
#define DBM_Q_CZ_GET_ACC_VALIDATION						7456  //4s - reqId,userId
#define DBM_A_CZ_GET_ACC_VALIDATION						(DBM_DELTA + DBM_Q_CZ_GET_ACC_VALIDATION) // 8456  //46[s|<rec>,b] - reqId,errCode,[errDesc|<AccVerification record>]

// PYR-47567
#define DBM_Q_MIGRATION_CZECH_CHECK						7457 // 4s - reqId, userId
#define DBM_A_MIGRATION_CZECH_CHECK						(DBM_DELTA + DBM_Q_MIGRATION_CZECH_CHECK) // 8457 //46[s] - reqId,errCode,[errDesc|B<userConext>]
						
#define DBM_Q_MIGRATION_CZECH_PERFORM					7458 // 4s - reqId, userId, B<AppLoginEssentials>
#define DBM_A_MIGRATION_CZECH_PERFORM					(DBM_DELTA + DBM_Q_MIGRATION_CZECH_PERFORM) // 8458  //46[s] - reqId,errCode,[errDesc]

// PYR-45097
#define DBM_Q_PM_WATCH2EARN_START						7459 //
#define DBM_A_PM_WATCH2EARN_START						(DBM_DELTA + DBM_Q_PM_WATCH2EARN_START) // 8459 //

// PYR-45097
#define DBM_Q_PM_WATCH2EARN_FINISH						7460 //
#define DBM_A_PM_WATCH2EARN_FINISH						(DBM_DELTA + DBM_Q_PM_WATCH2EARN_FINISH) // 8460 //

// PYR-48393
#define DBM_Q_VALIDATE_USERFRBANKACCOUNT                7461 // 44bss4 reqId userIntId setValid comment adminId adminIntId
#define DBM_A_VALIDATE_USERFRBANKACCOUNT                (DBM_DELTA + DBM_Q_VALIDATE_USERFRBANKACCOUNT) // 8461 	// 46[s] - reqId errCode [errDescr]

// PYR-44657
#define DBM_Q_TURN_ON_OFF_GAMEIN_AUTOCONVERSION			7462 // 4Osb48 reqId,MessageInfoStruct,userId,setNotRemove,updateType,appLoginId
#define DBM_A_TURN_ON_OFF_GAMEIN_AUTOCONVERSION			(DBM_DELTA + DBM_Q_TURN_ON_OFF_GAMEIN_AUTOCONVERSION) // 8462	// 46[s] - reqId errCode [errDescr]

// PYR-44657
#define DBM_Q_CONVERT_FOR_TARGET_AMOUNT 				7463 // 4Os4s8 reqId,MessageInfoStruct,userId,totalAmountNeeded,currency,appLoginId
#define DBM_A_CONVERT_FOR_TARGET_AMOUNT 				(DBM_DELTA + DBM_Q_CONVERT_FOR_TARGET_AMOUNT) // 8463	// 46[s|4] - reqId errCode [errDescr|amountAfter]

// PYR-47572 Cashier->RW Dbm
#define DBM_Q_VERIFY_CARD								7464 // 4s44s44 - reqId, userId, userIntId, CC_ID, paysystem, adminIntId, refId
#define DBM_A_VERIFY_CARD								(DBM_DELTA + DBM_Q_VERIFY_CARD) // 8464	// 46[s|4] - reqId errCode [errDescr]

// PYR-47770 : LobbyEtc->LDBM
#define DBM_Q_LDBM_GET_LOGIN_LIMIT_INFO					7465  //4s - reqId,userId
#define DBM_A_LDBM_GET_LOGIN_LIMIT_INFO					(DBM_DELTA + DBM_Q_LDBM_GET_LOGIN_LIMIT_INFO) // 8465  //46[s|4444t] - reqId, errCode[errDescr|loginTotal, loginTotalLimit, loginTimeTotalSec, loginTimeLimitMin, userSessionStartTime]

//	PYR-47770 LobbyEtc->RO Dbm
#define DBM_Q_GET_DAILY_BUYIN_ACCUMULATED_TOTALS		7466	// 4s44<4D> - reqId,userId,userIntId,numLimitType<limitType,date>
#define DBM_A_GET_DAILY_BUYIN_ACCUMULATED_TOTALS		(DBM_DELTA + DBM_Q_GET_DAILY_BUYIN_ACCUMULATED_TOTALS)	//	8466	//	46[s|4<44t>] - reqId, errCode[errDescr|numLimits<limitType,accumulatedTotal,scheduledTime>]

// PYR-48809 Cashier->RW Dbm
#define DBM_Q_UNVERIFY_CARD								7467 // 44ss44 - reqId, UnverifyAccountParams as (accountId, paysystem, userId, userIntId, adminIntId, refId)
#define DBM_A_UNVERIFY_CARD								(DBM_DELTA + DBM_Q_UNVERIFY_CARD) // 8467	// 46[s|4] - reqId errCode [errDescr]

// PYR-48559
#define DBM_Q_XC_PURGE_GAME								7468
#define DBM_A_XC_PURGE_GAME								(DBM_DELTA + DBM_Q_XC_PURGE_GAME) // 8468

// PYR-48153
#define DBM_Q_CHECK_USER_FIRST_TIME_EVENT               7469	//444 reqId, userIntId, licenseId
#define DBM_A_CHECK_USER_FIRST_TIME_EVENT               (DBM_DELTA + DBM_Q_CHECK_USER_FIRST_TIME_EVENT) // 8469 42[s|b] reqId, errCode[errDescr|found]

// PYR-49230
#define DBM_Q_GET_LAST_PT_REPORT_DATA_END_PERIOD		7470 //
#define DBM_A_GET_LAST_PT_REPORT_DATA_END_PERIOD		(DBM_DELTA + DBM_Q_GET_LAST_PT_REPORT_DATA_END_PERIOD) // 8470

// PYR-49230
#define DBM_Q_INSERT_PT_REPORT_RAW_DATA					7471 //
#define DBM_A_INSERT_PT_REPORT_RAW_DATA					(DBM_DELTA + DBM_Q_INSERT_PT_REPORT_RAW_DATA) // 8471

// PYR-49230
#define DBM_Q_GET_PT_REPORT_RAW_DATA					7472 //
#define DBM_A_GET_PT_REPORT_RAW_DATA					(DBM_DELTA + DBM_Q_GET_PT_REPORT_RAW_DATA) // 8472

// PYR-45046
#define DBM_Q_INSERT_ACCT_CREATION_FAILURE				7473 // 44{m} - reqId, errCode, {RegisterNewUserRequest}
#define DBM_A_INSERT_ACCT_CREATION_FAILURE				(DBM_DELTA + DBM_Q_INSERT_ACCT_CREATION_FAILURE) // 8473 46[s] - reqId errCode [errDescr]

// PYR-49605
#define DBM_Q_GET_PT_RMOK_USERS_INFO					7474 //
#define DBM_A_GET_PT_RMOK_USERS_INFO					(DBM_DELTA + DBM_Q_GET_PT_RMOK_USERS_INFO) // 8474

// PYR-44386
#define DBM_Q_INSERT_USER_PENDING_ACTION				7475 // 4<UserPendingAction> - reqId, <UserPendingAction>
#define DBM_A_INSERT_USER_PENDING_ACTION				(DBM_DELTA + DBM_Q_INSERT_USER_PENDING_ACTION) // 8475 46[s|4] - reqId,errCode[errDescr|pendingActionId]

// PYR-50042
#define DBM_QS_GET_USERS_BY_EMAIL 						7476 // 4s - reqId, email
#define DBM_AS_GET_USERS_BY_EMAIL  						(DBM_DELTA + DBM_QS_GET_USERS_BY_EMAIL) // 8476

// PYR-44386
#define DBM_Q_GET_EXPIRED_USERPENDINGACTIONS			7477 //4 - reqId
#define DBM_A_GET_EXPIRED_USERPENDINGACTIONS			(DBM_DELTA + DBM_Q_GET_EXPIRED_USERPENDINGACTIONS) // 8477 46[s|4<4>] - reqId,errCode[errDescr|vec_size,<UserActionId>]

#define DBM_Q_GET_USER_PENDING_ACTIONS					7478 //44 - reqId, userIntId
#define DBM_A_GET_USER_PENDING_ACTIONS					(DBM_DELTA + DBM_Q_GET_USER_PENDING_ACTIONS) // 8478 46[s|4<UserPendingAction>] - reqId,errCode[errDescr|vec_size,<UserPendingAction>]

// PYR-45550
#define DBM_Q_MG_ALLOCATE_NEXT_CATALOG_PRIZEID			7479 // 4 - reqId
#define DBM_A_MG_ALLOCATE_NEXT_CATALOG_PRIZEID			(DBM_DELTA + DBM_Q_MG_ALLOCATE_NEXT_CATALOG_PRIZEID) // 8479 46[s|4] - reqId,errCode[errString|prizeId]

// PYR-45550
#define DBM_Q_MG_ADD_CATALOG_PRIZE						7480 // 44M(4sssssM(44s8)484stt8s4<s>t4t4s) - reqId,expirePrizeId,M(prizeId,catalogName,prizeName,comment,nameResourceId,descriptionResourceId,M(prizeType,referenceId,currency,amount),licenseMask,affinitiesMask,tiersMask,preferredCurrency,startTime,endTime,value,valueCurrency,numCategories<categories>,created,creatorAdminIntId,expired,expiredAdminIntId,assetName)
#define DBM_A_MG_ADD_CATALOG_PRIZE						(DBM_DELTA + DBM_Q_MG_ADD_CATALOG_PRIZE) // 8480 46[s] - errCode[errString]

// PYR-45550
#define DBM_Q_MG_EXPIRE_CATALOG_PRIZE					7481 // 44t4 - reqId,expirePrizeId,timeExpired,adminIntId
#define DBM_A_MG_EXPIRE_CATALOG_PRIZE					(DBM_DELTA + DBM_Q_MG_EXPIRE_CATALOG_PRIZE) // 8481 46[s] - errCode[errString]

// PYR-44386
#define DBM_Q_FINALIZE_USER_PENDING_ACTION				7482 //442244 - reqId,userActionId,status,issueFailCode,userIntId,adminIntId
#define DBM_A_FINALIZE_USER_PENDING_ACTION				(DBM_DELTA + DBM_Q_FINALIZE_USER_PENDING_ACTION) // 8482 46[s|4<4>] - reqId,errCode[errDescr]

// PYR-49826
#define DBM_Q_MTL_TOURN_DEAL_AVAILABILITY               7483 // 48ts7574 reqId, tournId, timestamp, currency, numEntries, dealPauseOption, dealExclusion, flags
#define DBM_A_MTL_TOURN_DEAL_AVAILABILITY               (DBM_DELTA + DBM_Q_MTL_TOURN_DEAL_AVAILABILITY) // 8483     // 46[s] - reqId, errCode[errDescr] 

// PYR-49826
#define DBM_Q_MTL_TOURN_DEAL_ATTEMPT	                7484 // 487555tt94<7>4<4>4<8>4<6> reqId, tournId, dealGeneration, numPlayers, agreedDealType, dealFailureState, startTime, endTime, totalToAward, numPreDealPayouts<preDealPayouts>, numUserIntIds<userIntIds>, numAppLoginIds<appLoginIds>, numUserActions<userActions>
#define DBM_A_MTL_TOURN_DEAL_ATTEMPT		            (DBM_DELTA + DBM_Q_MTL_TOURN_DEAL_ATTEMPT) // 8484     // 46[s] - reqId, errCode[errDescr] 

// PYR-33529
#define DBM_Q_RESET_USER_NAC2                           7485 // 4s4 - reqId,userId,userIntId ; userId or userIntId must be set
#define DBM_A_RESET_USER_NAC2				            (DBM_DELTA + DBM_Q_RESET_USER_NAC2) // 8485 // 46[s] - reqId, errCode[errDescr] 

// PYR-50042
#define DBM_QS_US_MIGRATION_READ_IOM_USER               7486
#define DBM_AS_US_MIGRATION_READ_IOM_USER               (DBM_DELTA + DBM_QS_US_MIGRATION_READ_IOM_USER) // 8486

// PYR-50042
#define DBM_QS_US_MIGRATION_INSERT_US_USER_2            7487
#define DBM_AS_US_MIGRATION_INSERT_US_USER_2            (DBM_DELTA + DBM_QS_US_MIGRATION_INSERT_US_USER_2) // 8487

// PYR-46650
#define DBM_Q_GET_USER_AVAIL_FOR_CASHOUT				7488 // 4s4 - reqId,userId,userIntId ; userId or userIntId must be set
#define DBM_A_GET_USER_AVAIL_FOR_CASHOUT            	(DBM_DELTA + DBM_Q_GET_USER_AVAIL_FOR_CASHOUT) // 8488    // 46[s|4<4s>,4s] = 46[s|AmountsInCurrencies,AmountInCurrency] - reqId, errCode[errDescr|amounts, availableTotal]

// PYR-49061: special interface to remove all user's hanging logins within a time range. Use it only to remove massive hanging logins in production.
#define DBM_Q_GET_USERS_WITH_LOGIN_IN_TIME_RANGE        7489  //418tt4 reqId, objType, objId, timeFrom, timeTo, maxNum
#define DBM_A_GET_USERS_WITH_LOGIN_IN_TIME_RANGE        (DBM_DELTA + DBM_Q_GET_USERS_WITH_LOGIN_IN_TIME_RANGE) // 8489  //46[s|4<s>] reqId, err [errDescr|num<userId>] 

// PYR-49061: special interface to remove a user's hanging logins within a time range. Use it only to remove massive hanging logins in production.
#define DBM_Q_REMOVE_USER_LOGIN_IN_TIME_RANGE			7490  //4s18tt4 reqId, userName, objType, objId, timeFrom, timeTo, maxNumToRemove
#define DBM_A_REMOVE_USER_LOGIN_IN_TIME_RANGE			(DBM_DELTA + DBM_Q_REMOVE_USER_LOGIN_IN_TIME_RANGE) // 8490  //46[s|4] reqId, err [errDescr | numLoginsRemoved] 

// PYR-50210
#define DBM_Q_GET_PT_DISABLED_ACCOUNTS_STATUS			7491
#define DBM_A_GET_PT_DISABLED_ACCOUNTS_STATUS			(DBM_DELTA + DBM_Q_GET_PT_DISABLED_ACCOUNTS_STATUS) // 8491

// PYR-64459
#define DBM_Q_GET_FIRST_TAKEN_IN_RANGE					7492 // 4tt4 -reqId, startTime, endTime, licenseId
#define DBM_A_GET_FIRST_TAKEN_IN_RANGE					(DBM_DELTA + DBM_Q_GET_FIRST_TAKEN_IN_RANGE) // 8492 // 4t - reqId, startTaken

// PYR-50622
#define DBM_Q_SET_ROLL_BOUND_TO_GAME					7493 // 44b - reqId, rollId, setNotRemove
#define DBM_A_SET_ROLL_BOUND_TO_GAME					(DBM_DELTA + DBM_Q_SET_ROLL_BOUND_TO_GAME)	// 8493 46[s] - reqId,errCode[errDescr]

// PYR-50624
#define DBM_Q_GET_ROLLS									7494 // 4 reqId
#define DBM_A_GET_ROLLS									(DBM_DELTA + DBM_Q_GET_ROLLS)  // 8494 42[s|4<Roll>] reqId errCode [errDescr|size<Roll>]

// PYR-49208
#define DBM_Q_XC_JACKPOT_USERBET							7495
#define DBM_A_XC_JACKPOT_USERBET							(DBM_DELTA + DBM_Q_XC_JACKPOT_USERBET) // 8495

// PYR-49208
#define DBM_Q_XC_JACKPOT_WITHDRAWAL							7496
#define DBM_A_XC_JACKPOT_WITHDRAWAL							(DBM_DELTA + DBM_Q_XC_JACKPOT_WITHDRAWAL) // 8496

// PYR-50017
#define DBM_Q_GET_ALL_OPEN_IT_TABLES					7497
#define DBM_A_GET_ALL_OPEN_IT_TABLES					(DBM_DELTA + DBM_Q_GET_ALL_OPEN_IT_TABLES) // 8497

// PYR-50357
#define DBM_Q_SET_USER_COMMENT_EX                       7498 	//4ss4 reqId, userId, commentText, flags
#define DBM_A_SET_USER_COMMENT_EX                       (DBM_DELTA + DBM_Q_SET_USER_COMMENT_EX) // 8498  //46[s] reqId, errCode, [errDescr]

// PYR-52085
#define DBM_Q_GET_PT_PS_INTERNAL_USERS					7499 // 4 reqId
#define DBM_A_GET_PT_PS_INTERNAL_USERS					(DBM_DELTA + DBM_Q_GET_PT_PS_INTERNAL_USERS) // 8499 // 46[s|4<4>] reqId, errCode, [errDescr|size<userIntId>]

// PYR-51670
#define DBM_Q_LDBM_UPDATE_USER_BAR						7500 // 44 - reqId, userIntId, targetRps
#define DBM_A_LDBM_UPDATE_USER_BAR						(DBM_DELTA + DBM_Q_LDBM_UPDATE_USER_BAR) // 8500 // 46[s] reqId, errCode, [errDescr]

//#define DBM_Q_AVAILABLE_MESSAGE         7501 	//44O reqId, arjelReportType, TransactionInfo

// PYR-73376 regulator->loginDbm
// instead of DBM_Q_GET_ARJEL_TRANSACTION_REPORT_DATA

//#define DBM_Q_AVAILABLE_MESSAGE		7501  //4sbbb reqId, userId, pokerPref, casinoPref, sportsPref

//PYR-51427
#define DBM_Q_MG_ENSURE_PRIZE_CATEGORIES_EXIST   7502 //44<s> - reqId, numCategories<categories>
#define DBM_A_MG_ENSURE_PRIZE_CATEGORIES_EXIST   (DBM_DELTA + DBM_Q_MG_ENSURE_PRIZE_CATEGORIES_EXIST) // 8502 46[s] - reqId, errCode[errString]

// PYR-33629 RMNOK PayPal
#define DBM_Q_SAVE_RMNOK_USER_MISSING_PERSONAL_INFO		7503 // based on External (gw=PayPal) data
#define DBM_A_SAVE_RMNOK_USER_MISSING_PERSONAL_INFO		(DBM_DELTA + DBM_Q_SAVE_RMNOK_USER_MISSING_PERSONAL_INFO) // 8503

#define DBM_Q_COPY_CASHTRANS_PRE						7504 //
#define DBM_A_COPY_CASHTRANS_PRE						(DBM_DELTA + DBM_Q_COPY_CASHTRANS_PRE) // 8504

// PYR-50292
#define DBM_Q_GET_USERS_INFO_FOR_UA					7505	// 4tt - reqId,from,until
#define DBM_A_GET_USERS_INFO_FOR_UA					(DBM_DELTA + DBM_Q_GET_USERS_INFO_FOR_UA) // 8505	// 46[s|4<4444>] - reqId,errCode,[errDescr|numUsers<userIntId,platformUsed,brandUsed,siteVisited>]

// PYR-51233
#define DBM_Q_GET_SC_LOGGEDAPP_DATA					7506 // 44 - reqId, userIntId
#define DBM_A_GET_SC_LOGGEDAPP_DATA					(DBM_DELTA + DBM_Q_GET_SC_LOGGEDAPP_DATA) // 8506	// 46[s|s4sss44s4444s] - reqId,errCode,[errDesc|SCLoginAppData]

#define DBM_Q_GET_SC_EMAIL_ALIAS_HANDLE				7507 // 4ss44 - reqId, userId, emHandleAlias, siteId, loopbackHours
#define DBM_A_GET_SC_EMAIL_ALIAS_HANDLE				(DBM_DELTA + DBM_Q_GET_SC_EMAIL_ALIAS_HANDLE) // 8507	// 46[s|b] - reqId,errCode,[errDesc|aliasFound]

// PYR-52861: logindbm <-> oltp
#define DBM_Q_UPDATE_USER_IP_INFO					7508 // 4ssss  reqId, userId, ipAddr, countryByIp, stateByIp
#define DBM_A_UPDATE_USER_IP_INFO					(DBM_DELTA + DBM_Q_UPDATE_USER_IP_INFO) // 8508 // 46[s] reqId, errCode, [errDescr]

// PYR-51233
#define DBM_Q_GET_SC_POSTAL_CODE_VELOCITY			7509 // 4s4 - reqId, zip, nPrevDays
#define DBM_A_GET_SC_POSTAL_CODE_VELOCITY			(DBM_DELTA + DBM_Q_GET_SC_POSTAL_CODE_VELOCITY) // 8509	// 46[s|4] - reqId,errCode,[errDesc|numZips]

#define DBM_Q_GET_SC_IP_VELOCITY					7510 // 4s4 - reqId, ipAddress, nPrevDays
#define DBM_A_GET_SC_IP_VELOCITY					(DBM_DELTA + DBM_Q_GET_SC_IP_VELOCITY) // 8510	// 46[s|4] - reqId,errCode,[errDesc|numAccounts]

// PYR-51733
#define DBM_Q_GET_ALL_CCIDS_BY_USER					7511 // 4s - reqId, userId
#define DBM_A_GET_ALL_CCIDS_BY_USER					(DBM_DELTA + DBM_Q_GET_ALL_CCIDS_BY_USER) // 8511	// 46[s|4<4>] - reqId,errCode,[errDesc|numCardIds<cardId>]

#define DBM_Q_GET_USERBLOCKEDCARDS					7512 // 4s - reqId, userIntId
#define DBM_A_GET_USERBLOCKEDCARDS					(DBM_DELTA + DBM_Q_GET_USERBLOCKEDCARDS) // 8512	// 46[s|4<T>] - reqId,errCode,[errDesc|vectorSize<UserBlockedCard>]

// PYR-50742
#define DBM_Q_INSERT_RESOLVERLOG					7513 // 4ss4s - reqId, link, site, brand, resolverId
#define DBM_A_INSERT_RESOLVERLOG					(DBM_DELTA + DBM_Q_INSERT_RESOLVERLOG) // 8513	// 46[s] - reqId,errCode,[errDesc]

// PYR-50742
#define DBM_Q_UPDATE_RESOLVERLOG					7514 // 4ss4s - reqId, link, site, brand, resolverId
#define DBM_A_UPDATE_RESOLVERLOG					(DBM_DELTA + DBM_Q_INSERT_RESOLVERLOG) // 8514	// 46[s] - reqId,errCode,[errDesc]

#define DBM_Q_SELECT_RESOLVERLOG					7515 // 4s - reqId, resolverId
#define DBM_A_SELECT_RESOLVERLOG					(DBM_DELTA + DBM_Q_SELECT_RESOLVERLOG) // 8515	// 46[s|4<ss4>] - reqId,errCode,[errDesc|num<link,site,brand>]

// PYR-51233
#define DBM_Q_GET_SC_EMAIL_HANDLE_USAGE				7516 // 4ss44 - reqId, userId, emHandleAlias, siteId, loopbackHours
#define DBM_A_GET_SC_EMAIL_HANDLE_USAGE				(DBM_DELTA + DBM_Q_GET_SC_EMAIL_HANDLE_USAGE) // 8516	// 46[s|b] - reqId,errCode,[errDesc|countExceeded]

// PYR-50202
#define DBM_Q_INSERT_DORMANTUSERCHECK               7517 // 4O - reqId, DormantUserCheck
#define DBM_A_INSERT_DORMANTUSERCHECK               (DBM_DELTA + DBM_Q_INSERT_DORMANTUSERCHECK) // 8517  // 46[s] - reqId,errCode,[errDesc]

// PYR-51733
#define DBM_Q_UPDATE_BLOCKED_CARD_FLAGS				7518  //4444b reqId userCardId ccid newBlkCardFlags setNotRemove
#define DBM_A_UPDATE_BLOCKED_CARD_FLAGS				(DBM_DELTA + DBM_Q_UPDATE_BLOCKED_CARD_FLAGS) // 8518  //46[s] - reqId, errCode[errDescr] 

// PYR-44386
#define DBM_Q_MODIFY_ARCHIVED_USERS_TRACKCODE        7519 	//4s1b4<s>s reqId trackCode mode (0 add 1 delete) overLimited numUsers <userId> adminId
#define DBM_A_MODIFY_ARCHIVED_USERS_TRACKCODE        (DBM_DELTA + DBM_Q_MODIFY_ARCHIVED_USERS_TRACKCODE) // 8519 	//46[s] reqId, err_code[errDescr|numusers<userId err_code errDescr>] 

// PYR-50202
#define DBM_Q_JOBSCTRL_NEED_DATA                     7520 	// 4 - reqId
#define DBM_A_JOBSCTRL_NEED_DATA                     (DBM_DELTA + DBM_Q_JOBSCTRL_NEED_DATA) // 8520  //46[s|4<O>] - reqId, errCode[errDescr|size<JobsCtrlLog>]

#define DBM_Q_CREATE_JOBSCTRLLOG                     7521 	// 4O - reqId, JobsCtrlLog
#define DBM_A_CREATE_JOBSCTRLLOG                     (DBM_DELTA + DBM_Q_CREATE_JOBSCTRLLOG) // 8521  //46[s|4] - reqId, errCode[errDescr|jobId]

#define DBM_Q_UPDATE_JOBSCTRLLOG                     7522 	// 4O - reqId, JobsCtrlLog
#define DBM_A_UPDATE_JOBSCTRLLOG                     (DBM_DELTA + DBM_Q_UPDATE_JOBSCTRLLOG) // 8522  //46[s] - reqId, errCode[errDescr]

#define DBM_Q_SAVE_JOBSCTRLRESULTS                   7523 	// 44<O> - reqId, size<JobsCtrlResult>
#define DBM_A_SAVE_JOBSCTRLRESULTS                   (DBM_DELTA + DBM_Q_SAVE_JOBSCTRLRESULTS) // 8523  //46[s] - reqId, errCode[errDescr]

#define DBM_Q_SAVE_TRANS_PAYWALL	                 7524 	// PYR-48920 cashier->mainDbm
#define DBM_A_SAVE_TRANS_PAYWALL		             (DBM_DELTA + DBM_Q_SAVE_TRANS_PAYWALL) // 8524  //46[s] - reqId, errCode[errDescr]

// PYR-49193
#define DBM_Q_GET_CASINO_SPORT_WAGERING              7525	// 4ttt4<s4>  -  reqId ProductType timeFrom timeTo nUsers<userId userIntId>
#define DBM_A_GET_CASINO_SPORT_WAGERING              (DBM_DELTA + DBM_Q_GET_CASINO_SPORT_WAGERING) // 42[s|4<s4<s8>4<s8>>] - reqId errCode [errStr|nUsers<userId numCurrCas<curr Amount> numCurrSports<curr Amount>>]

// PYR-52678
#define DBM_Q_GET_ARJEL_USER_REPORT_EXTRA_DATA		 7526 	//44 reqId, userIntId
#define DBM_A_GET_ARJEL_USER_REPORT_EXTRA_DATA       (DBM_DELTA + DBM_Q_GET_ARJEL_USER_REPORT_EXTRA_DATA) // 8526  //46[s|4<6[s|s8] reqId, errCode, [errDescr|lastKnownIpAddress,lastLoginId]

// PYR-52678
#define DBM_Q_GET_ARJEL_HAND_REPORT_EXTRA_DATA   	 7527 	//484<4> reqId, tournId, nUsers<userIntId>
#define DBM_A_GET_ARJEL_HAND_REPORT_EXTRA_DATA       (DBM_DELTA + DBM_Q_GET_ARJEL_HAND_REPORT_EXTRA_DATA) // 8527  //46[s|b[|O]4<46[s|s]>] reqId, errCode, [errDescr|tournInfofound[|ArjelTournInfo],nUsers<userIntId,errCode[errDescr|lastKnownIpAddress]>]

// PYR-54303
#define DBM_Q_UPDATE_USER_USEDBALANCE				 7528 // 4s44 req,userId,oldValue,newValue
#define DBM_A_UPDATE_USER_USEDBALANCE				 (DBM_DELTA + DBM_Q_UPDATE_USER_USEDBALANCE) // 8528  //46[s]

// PYR-53001
#define DBM_Q_MODIFY_FISCAL_CODE_VERIFIED            7529 // 4sbs reqId userId setNotRemove comment
#define DBM_A_MODIFY_FISCAL_CODE_VERIFIED            (DBM_DELTA + DBM_Q_MODIFY_FISCAL_CODE_VERIFIED ) // 8529  //46[s] reqId err_code[errDescr] 

// PYR-30309 cashier -> RO dbm
#define DBM_Q_GET_IND_USER_DAILY_TAXABLE_INFO        7530 // 4s reqId userId
#define DBM_A_GET_IND_USER_DAILY_TAXABLE_INFO        (DBM_DELTA + DBM_Q_GET_IND_USER_DAILY_TAXABLE_INFO ) // 8530 //46[s|44] reqId,errCode[errDescr|dailyTaxableCashout,dailyTaxPaid]

// PYR-51560
#define DBM_Q_GET_ARCHIVED_SCHED_BROADCAST_MSG       7531	// 4  -  reqId
#define DBM_A_GET_ARCHIVED_SCHED_BROADCAST_MSG       (DBM_DELTA + DBM_Q_GET_ARCHIVED_SCHED_BROADCAST_MSG) // 8531 // 46[s|4<M(4STSI44b4<4>4<4>4<4>44<S>4<T>)>] - reqId, errCode[errDescr|numBroadcasts<M(broadcastID, createdBy, createdTime, lastModifiedBy, message, flags, sites, numTourns<TournID>, numHandTypes<HandType>, numBetStructs<BetStruct>, stakesMask, numCountries<Country>, numDatetimes<datetime>)>]

// PYR-52524
#define DBM_Q_GET_GLOBALPUBLISHER_INITDATA           7532	// 4 -  reqId
#define DBM_A_GET_GLOBALPUBLISHER_INITDATA           (DBM_DELTA + DBM_Q_GET_GLOBALPUBLISHER_INITDATA) // 8532 // 42[s|4<s4<ss4>>] - reqId errCode [errStr|numSections<name,numItems<name,value,type>>cvldata]

// PYR-52524
#define DBM_Q_GET_LOBBY_OPTIONS						 7533	// 4 -  reqId
#define DBM_A_GET_LOBBY_OPTIONS						 (DBM_DELTA + DBM_Q_GET_LOBBY_OPTIONS) // 8533 // 42[s|4<s4st>] - reqId errCode [errStr|numParams<propId, propInt, propStr, when>]

// PYR-52669
#define DBM_Q_EXECUTE_LEVENSHTEIN_ON_DOCS			 7534	// 444M - reqId, threshold1, threshold2, <RegBlackListItem>
#define DBM_A_EXECUTE_LEVENSHTEIN_ON_DOCS	 		 (DBM_DELTA + DBM_Q_EXECUTE_LEVENSHTEIN_ON_DOCS) // 8534 // 42[s|4<PtBlackListCheckItem>] - reqId errCode [errStr|vectorSize<PtBlackListCheckItem>]

// PYR-52678
#define DBM_Q_GET_USER_INFO_FOR_GAME_EVENT    	     7535 	//44<4> reqId, numUsers<userIntId>
#define DBM_A_GET_USER_INFO_FOR_GAME_EVENT           (DBM_DELTA + DBM_Q_GET_USER_INFO_FOR_GAME_EVENT) // 8535  //44<6[s|M]> reqId, numUsers<6[s|MsgBody]>

//	PYR-51853
#define DBM_Q_LDBM_ADMIN_CLEAR_TEMP_PIN				 7536	//	44s - reqId, userIntId, userId
#define DBM_A_LDBM_ADMIN_CLEAR_TEMP_PIN				 (DBM_DELTA + DBM_Q_LDBM_ADMIN_CLEAR_TEMP_PIN)	//	8536	//	46[s] - reqId, errCode[errDescr]

//	PYR-51853
#define DBM_Q_LDBM_GET_TEMP_PIN						 7537	//	44 - reqId, userIntId
#define DBM_A_LDBM_GET_TEMP_PIN						(DBM_DELTA + DBM_Q_LDBM_GET_TEMP_PIN)	//	8537	//	46[s|s] - reqId, errCode[errDescr|tempPin]

//	PYR-56094
#define DBM_Q_DRC_NOTIFY_DISASTER_STATE_TABLE		 7538	// 484ff - reqId,tableId,factoryId,disasterStateEnabled,lobbyDisconnectWarningEnabled
#define DBM_A_DRC_NOTIFY_DISASTER_STATE_TABLE		 (DBM_DELTA + DBM_Q_DRC_NOTIFY_DISASTER_STATE_TABLE)	// 8538 	// 46[s] - reqId, errCode[errDescr]

#define DBM_Q_DRC_NOTIFY_DISASTER_STATE_TOURN		 7539	// 484ff - reqId,tournId,factoryId,disasterStateEnabled,lobbyDisconnectWarningEnabled
#define DBM_A_DRC_NOTIFY_DISASTER_STATE_TOURN		 (DBM_DELTA + DBM_Q_DRC_NOTIFY_DISASTER_STATE_TOURN)	// 8539 	// 46[s] - reqId, errCode[errDescr]

#define DBM_Q_DRC_NOTIFY_DISASTER_STATE_BLITZ		 7540	// 444ff - reqId,blitzId,factoryId,disasterStateEnabled,lobbyDisconnectWarningEnabled
#define DBM_A_DRC_NOTIFY_DISASTER_STATE_BLITZ		 (DBM_DELTA + DBM_Q_DRC_NOTIFY_DISASTER_STATE_BLITZ)	// 8540 	// 46[s] - reqId, errCode[errDescr]

// PYR-54689
#define DBM_Q_SET_GAME_AVATAR						 7541 // 44444 - reqId,userIntId,brandId,avatarGameType,imageId
#define DBM_A_SET_GAME_AVATAR						 (DBM_DELTA + DBM_Q_SET_GAME_AVATAR)					// 8541 	// 46[s] - reqId, errCode[errDescr]

// PYR-57450
#define DBM_Q_DESH_GET_FILE_IDS_IN_INTERVAL			 7542 // 4tt - reqId,timeFrom,timeTo
#define DBM_A_DESH_GET_FILE_IDS_IN_INTERVAL			 (DBM_DELTA + DBM_Q_DESH_GET_FILE_IDS_IN_INTERVAL)      // 8542     // 46[s|4<44>] - reqId, errCode[errDescr | filesCount<fileId,version>]

// PYR-50066
#define DBM_Q_XC_HAND_DETAILS						 7543
#define DBM_A_XC_HAND_DETAILS						 (DBM_DELTA + DBM_Q_XC_HAND_DETAILS) // 8543

// PYR-50066
#define DBM_Q_ADD_CASINO_HAND_DETAILS				 7544
#define DBM_A_ADD_CASINO_HAND_DETAILS				 (DBM_DELTA + DBM_Q_ADD_CASINO_HAND_DETAILS) // 8544

// PYR-55898
#define DBM_Q_MG_GET_ACTIVE_CATALOG_PRIZES			 7545 // 4st - reqId,catalogName,time
#define DBM_A_MG_GET_ACTIVE_CATALOG_PRIZES			 (DBM_DELTA + DBM_Q_MG_GET_ACTIVE_CATALOG_PRIZES) // 8545			// 42[s|4<M(4sssssM(44s8)484stt8s4<s>t4t4sM(4s4444))>] - reqId,errCode[errString|numPrizes<M(prizeId,catalogName,prizeName,comment,nameResourceId,descriptionResourceId,M(prizeType,referenceId,currency,amount),licenseMask,affinitiesMask,tiersMask,preferredCurrency,startTime,endTime,value,valueCurrency,numCategories<categories>,created,creatorAdminIntId,expired,expiredAdminIntId,assetName,M(prizeLimitId,prizeLimitName,playerLimitAmount,playerLimitDuration,globalLimitAmount,globalLimitDuration))>]

// PYR-55194 Collusion -> Olap
#define DBM_Q_GET_FREUD_USER_LOGIN_DATA 			 7546 // reqId userId
#define DBM_A_GET_FREUD_USER_LOGIN_DATA				 (DBM_DELTA + DBM_Q_GET_FREUD_USER_LOGIN_DATA) // 8546  // 42[s|FreudUserLoginData] reqId, errCode[errDescr|FreudUserLoginData]

// PYR-53503
#define DBM_Q_MG_SAVE_EV_DATA						 7547	// 4M - reqId,StarsRewardsEVData
#define DBM_A_MG_SAVE_EV_DATA						 (DBM_DELTA + DBM_Q_MG_SAVE_EV_DATA) // 8547	// 46[s] - reqId, errCode[errDescr]

// PYR-56207
#define DBM_Q_SET_OPTIONS_MADE_BY_USER				 7548 // 4s4s48888444ss - reqId,userId,updateType,adminId,adminIntId,appLoginId,prevPrivileges3,privs3MaskToSet,privs3MaskToClear,prevOptions,maskSet,maskClear,casinoUserComment,sportsUserComment
#define DBM_A_SET_OPTIONS_MADE_BY_USER				 (DBM_DELTA + DBM_Q_SET_OPTIONS_MADE_BY_USER) // 8548				// 46[s] - reqId, errCode[errDescr]

// PYR-54454, not used after the JIRA is suspended
#define DBM_Q_GET_SPAIN_BOT_TOURN_MONTHLY			 7549 // 4tt reqId, startTaken, endTaken
#define DBM_A_GET_SPAIN_BOT_TOURN_MONTHLY			 (DBM_DELTA + DBM_Q_GET_SPAIN_BOT_TOURN_MONTHLY) // 8549 // 42[s|8888] - reqId, errCode[errDescr|initialBalance,movementsDebit,movementsCredit,finalBalance]

// PYR-52613
#define DBM_Q_CHECK_BONUS							 7550 // 4s reqId, paysystem, bonusCode, acctAmount, acctCurrency
#define DBM_A_CHECK_BONUS							 (DBM_DELTA + DBM_Q_CHECK_BONUS) // 8550

// PYR-55543
#define DBM_Q_GET_USERROLLS_BY_USER_PLUS_BOUND_EXPIRED	7551 // 44 - reqId,userIntId
#define DBM_A_GET_USERROLLS_BY_USER_PLUS_BOUND_EXPIRED	(DBM_DELTA + DBM_Q_GET_USERROLLS_BY_USER_PLUS_BOUND_EXPIRED) // 8551 42[s|UserRolls,4<4>] - reqId,errCode[errDescr|UserRolls,boundExpiredCount<boundExpiredURId>]

// PYR-37902
#define DBM_Q_BG_GET_SPORTSBOOK_DATA                 7552 // 4tt - reqId,startTime,endTime
#define DBM_A_BG_GET_SPORTSBOOK_DATA                 (DBM_DELTA + DBM_Q_BG_GET_SPORTSBOOK_DATA) // 8552 42[s|4<M(O)>] - reqId, errCode [errDescr | size<M(BgSportsbookBetResultMsg)>]

// PYR-49647
#define DBM_Q_SET_TOURN_PUBLISHED_STATUS			7553 // 44f - reqId,tournId,isPublished
#define DBM_A_SET_TOURN_PUBLISHED_STATUS			(DBM_DELTA + DBM_Q_SET_TOURN_PUBLISHED_STATUS) // 8553 	// 46[s] - reqId,errCode,[errDescr] 

// PYR-55334
#define DBM_Q_GET_SPAIN_BOT_JACKPOTS_INFO			7554 // 4tt reqId, startTaken, endTaken
#define DBM_A_GET_SPAIN_BOT_JACKPOTS_INFO			(DBM_DELTA + DBM_Q_GET_SPAIN_BOT_JACKPOTS_INFO) // 8554 // 42[s|4<MsgBody>] - reqId, errCode[errDescr|jackpotsNum<MsgBody{CnjBotReportV2::JackpotData}>]

//	PYR-61912
#define DBM_Q_USER_AUTH_ADMINRO						7555	//	same format as DBM_Q_USER_AUTH
#define DBM_A_USER_AUTH_ADMINRO						(DBM_DELTA + DBM_Q_USER_AUTH_ADMINRO)	//	8555	// same format as DBM_A_USER_AUTH

// PYR-51402 Cashier->DBM
#define DBM_Q_APPEND_CASHTRANS_ERROR_INFO			7556 // 444s - reqId, transId, cashtransTableId (see enum eCashtransTableId), strToAppend
#define DBM_A_APPEND_CASHTRANS_ERROR_INFO			(DBM_DELTA + DBM_Q_APPEND_CASHTRANS_ERROR_INFO)	// 8556 42[s] - reqId,errCode[errDescr]

// PYR-62364 - retrieve email data by userids
#define DBM_Q_GET_USERS_EMAIL_DATA					7557  // 44<s> - reqId, numUsers<userId>
#define DBM_A_GET_USERS_EMAIL_DATA					(DBM_DELTA + DBM_Q_GET_USERS_EMAIL_DATA) // 8557  // 46[s|4<ss44>] - reqId,errCode[errDescr|numUsers<userId,email,locale,licenseId>] 

// PYR-54304 Cashier -> DBM
#define DBM_Q_PAY_INDIA_USER_ANNUAL_TAX				7558 //4Os84 - reqId, MessageInfoStruct, userId, appLoginId, brandId
#define DBM_A_PAY_INDIA_USER_ANNUAL_TAX				(DBM_DELTA + DBM_Q_PAY_INDIA_USER_ANNUAL_TAX) // 8558 42[s|4] - reqId,errCode[errDescr|taxPaid]

// PYR-54304 Cashier -> OLAP
#define DBM_Q_GET_INDIA_USERS_TO_PAY_ANNUAL_TAXES	7559 // 4 - reqId
#define DBM_A_GET_INDIA_USERS_TO_PAY_ANNUAL_TAXES 	(DBM_DELTA + DBM_Q_GET_INDIA_USERS_TO_PAY_ANNUAL_TAXES)	// 8559 46[s|4<M>] - reqId,errCode[errDescr|vec_size<IndiaUserInfo]

// PYR-62719
#define DBM_Q_GET_RECENT_SUCCESSFUL_DEPOSITS		7560 // 4s4 reqId, userId, secondsAgo
#define DBM_A_GET_RECENT_SUCCESSFUL_DEPOSITS		(DBM_DELTA + DBM_Q_GET_RECENT_SUCCESSFUL_DEPOSITS) // 8560 46[s|4<O>] - reqId,errCode[errDesc|size<CashTransDataSimple>]

// PYR-60134
#define DBM_Q_CONVERTER_NEED_RO_DATA                7561  // 4 reqId
#define DBM_A_CONVERTER_NEED_RO_DATA                (DBM_DELTA + DBM_Q_CONVERTER_NEED_RO_DATA) // 8561  // 

// PYR-60134
#define DBM_Q_CONVERTER_NEED_OLAP_DATA              7562  // 4 reqId
#define DBM_A_CONVERTER_NEED_OLAP_DATA              (DBM_DELTA + DBM_Q_CONVERTER_NEED_OLAP_DATA) // 8562  // 

// PYR-59305
#define DBM_Q_GET_NET_REPORT_BRIEF                  7563 // 4s reqId, userId
#define DBM_A_GET_NET_REPORT_BRIEF                  (DBM_DELTA + DBM_Q_GET_NET_REPORT_BRIEF) // 8563 6[s|T4<O>] reqId,errCode[errDesc|timeSince,size<CashTransDataSimple>]

// PYR-65082: APAL BioMAA -> OLAP
#define DBM_Q_CLOSED_ACCOUNT_CATEGORY_BY_INTID      7564     //44<4> reqId numIds<userIntId>
#define DBM_A_CLOSED_ACCOUNT_CATEGORY_BY_INTID      (DBM_DELTA + DBM_Q_CLOSED_ACCOUNT_CATEGORY_BY_INTID) // 8564     //46[s|4<42>] reqId errCode[errStr|numUsers<userIntId,Category>]

// PYR-64479 Admin -> RW DBM
#define DBM_Q_SURRENDER_USER_ASSETS_GDPR			7565 // 4s4sMs - reqId,userId,userIntId,M(tickets&bonuses) // 'M(tickets&bonuses)' comes from 'DBM_A_GET_USER_SURRENDER_ASSETS'
#define DBM_A_SURRENDER_USER_ASSETS_GDPR			(DBM_DELTA + DBM_Q_SURRENDER_USER_ASSETS_GDPR) // 8565 // 46[s|]

// PYR-54559
#define DBM_Q_GET_TOURN_CLOSED_DETAILS_BY_ASSETS	7566  // 484 - reqId, tournId, licenseId
#define DBM_A_GET_TOURN_CLOSED_DETAILS_BY_ASSETS	(DBM_DELTA + DBM_Q_GET_TOURN_CLOSED_DETAILS_BY_ASSETS) // 8566  // 46[s|m] - reqId,errCode[errDescr|(licenseSpecificBody)]

// PYR-65782
#define DBM_Q_GET_USERS_FOR_CALC_TOURN_TICKET_TOTALS    7567  // 44 - reqId, licenseId
#define DBM_A_GET_USERS_FOR_CALC_TOURN_TICKET_TOTALS    (DBM_DELTA + DBM_Q_GET_USERS_FOR_CALC_TOURN_TICKET_TOTALS) // 8567  // 46[s|4<4>] - reqId,errCode[errDescr|size<userIntId>]

// PYR-54559 spain replicator->OLAP
#define DBM_Q_SPAIN_GET_DAILY_CJ_DATA                 7568  // 4ttt - reqId, start_timestamp, end_timestamp, takenTime
#define DBM_A_SPAIN_GET_DAILY_CJ_DATA                 (DBM_DELTA + DBM_Q_SPAIN_GET_DAILY_CJ_DATA) // 8568  // 46[s|ttO] - reqid,errCode[errDescr|startTaken,endTaken,EsCjReportData]

// PYR-54559 spain replicator->OLAP
#define DBM_Q_SPAIN_GET_ADDITIONAL_MONTHLY_CJ_OP_DATA 7569  // 4ttt - reqId, start_timestamp, end_timestamp, takenTime
#define DBM_A_SPAIN_GET_ADDITIONAL_MONTHLY_CJ_OP_DATA (DBM_DELTA + DBM_Q_SPAIN_GET_ADDITIONAL_MONTHLY_CJ_OP_DATA) // 8569 // 46[s|O] - reqid,errCode[errDescr|EsAdditionalOpData]

// PYR-65396
#define DBM_Q_SET_CVLUSERTIER						7570  //444 reqId userIntId tier
#define DBM_A_SET_CVLUSERTIER						(DBM_DELTA + DBM_Q_SET_CVLUSERTIER) // 8570  //46[s] - reqId, errCode[errDescr] 

//	PYR-52518
#define DBM_Q_GET_EXTERNAL_USER_INFO_BY_OPERATOR	7571	//	46s - reqId, opId, operatorUserId
#define DBM_A_GET_EXTERNAL_USER_INFO_BY_OPERATOR	(DBM_DELTA + DBM_Q_GET_EXTERNAL_USER_INFO_BY_OPERATOR)		//	8571	//	46[s|s4M4<o>] - reqId,errCode[errDescr|psUserId,userIntId,M(User),vecSize<UserProperty>]

// PYR-64479 Admin -> RO DBM
#define DBM_Q_GET_USER_SURRENDER_ASSETS_GDPR		7572 // 4s4 - reqId,userId,userIntId
#define DBM_A_GET_USER_SURRENDER_ASSETS_GDPR		(DBM_DELTA + DBM_Q_GET_USER_SURRENDER_ASSETS_GDPR) // 8572 // 46[s|MM]bs - reqId,errCode[errDescr|M(tickets&bonuses)M(User),bSurrenderFlag,assetComment]

// PYR-94187 lobbyEtc -> RO DBM
#define DBM_Q_GET_USERS_ATTRS_FOR_UA				7573	// 44 - reqId,userIntId
#define DBM_A_GET_USERS_ATTRS_FOR_UA				(DBM_DELTA + DBM_Q_GET_USERS_ATTRS_FOR_UA) // 8573	// 46[s|444m] - reqId,errCode,[errDescr|platformUsed,brandUsed,siteVisited,visitedEx]

// PYR-64516 Asia replicator -> RO DBM
#define DBM_Q_GET_EXTERNAL_USER_BY_USERINTID        7574 // 44 - reqId, userIntId
#define DBM_A_GET_EXTERNAL_USER_BY_USERINTID        (DBM_DELTA + DBM_Q_GET_EXTERNAL_USER_BY_USERINTID) // 8574 // 46[s|2ss] - reqId,errCode[errDescr|opId,opUserId,operatorIdStr]

// Asia replicator -> OLAP
#define DBM_Q_GET_ASIA_REPORT_EVENTS_ADDITIONAL_DATA	7575 // 4tt - reqId, startTimeUtc endTimeUtc(inclusive)
#define DBM_A_GET_ASIA_REPORT_EVENTS_ADDITIONAL_DATA	(DBM_DELTA + DBM_Q_GET_ASIA_REPORT_EVENTS_ADDITIONAL_DATA)	// 8575 // // 46[s|4<8T4O>] - reqId,errCode[errDescr|size<transId,heartbeat,gameRefType,gameDataStruct>]
// End PYR-64516

//	PYR-69655
#define DBM_Q_FWD_LOGMONITOR_ISSUE					 7576	// 46ss6ss - reqId,eLogMonitorIssueType,formatStr,logStr,errCode,sourceData,sourceLogPrefix
#define DBM_A_FWD_LOGMONITOR_ISSUE					 (DBM_DELTA + DBM_Q_FWD_LOGMONITOR_ISSUE)	// 8576 	// 46[s] - reqId, errCode[errDescr]

// PYR-52007 asiareplicator->OLAP
#define DBM_Q_GET_ASIA_REPORT_EVENTS_FOR_PERIOD    7577  // 4tt - reqId startTimeUtc endTimeUtc(inclusive)
#define DBM_A_GET_ASIA_REPORT_EVENTS_FOR_PERIOD    (DBM_DELTA + DBM_Q_GET_ASIA_REPORT_EVENTS_FOR_PERIOD) // 8577  // 42[s|4<M>]

// PYR-67700
#define DBM_Q_GET_USERS_FOR_IT_REPORT_DEPOSIT_LIMITS 7578  // 4 - reqId
#define DBM_A_GET_USERS_FOR_IT_REPORT_DEPOSIT_LIMITS (DBM_DELTA + DBM_Q_GET_USERS_FOR_IT_REPORT_DEPOSIT_LIMITS) // 8578  // 42[s|4<???>] TBD

// PYR-67720
#define DBM_Q_GET_USERESSENTIALS_BY_USERINTID		 7579  // 44<4> - reqId, vectorSize, <userIntId>
#define DBM_A_GET_USERESSENTIALS_BY_USERINTID		 (DBM_DELTA + DBM_Q_GET_USERESSENTIALS_BY_USERINTID) // 8579  // 42[s|4<UserEssentials>]

//	PYR-64085
#define DBM_Q_ASIAGAMINGGATEWAY_NEED_DATA			 7580   // 4 - reqId
#define DBM_A_ASIAGAMINGGATEWAY_NEED_DATA			 (DBM_DELTA + DBM_Q_ASIAGAMINGGATEWAY_NEED_DATA)	//	8580 // 4[2|4<s6>] - reqId,errCode[errDesc|size<operatorId,opId>]

// PYR-69864
#define DBM_Q_SET_USER_COMMENT_ARCHIVED				 7581 	//4sss44 reqId, userId, commentText, adminId, adminIntId, userIntId
#define DBM_A_SET_USER_COMMENT_ARCHIVED				 (DBM_DELTA + DBM_Q_SET_USER_COMMENT_ARCHIVED) // 8581  //46[s] reqId, errCode, [errDescr] 

// PYR-64730 MtLobby -> TournDbm
#define DBM_Q_MTL_PLAY_MONEY_TOURN_FINISHED			 7582	//	48 - reqId,tournId
#define DBM_A_MTL_PLAY_MONEY_TOURN_FINISHED			 (DBM_DELTA + DBM_Q_MTL_PLAY_MONEY_TOURN_FINISHED)	//	8582	//	46[s] - reqId,errCode,[errDesc] 

// PYR-64085: Asia CDD -> OLAP
#define DBM_Q_GET_USER_CASHOUTS_ASIA				 7583     // 4st - reqId, userId, startedFrom
#define DBM_A_GET_USER_CASHOUTS_ASIA				 (DBM_DELTA + DBM_Q_GET_USER_CASHOUTS_ASIA) // 8583	// 42[s|4<UserAsiaCashouts::Case>]

// PYR-69503 Admin -> MainDBM
#define DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG			 7584 // 4O44s - reqId,MessageInfoStruct,userIntId,reasons,comment
#define DBM_A_SET_FRAUDULENT_ACCOUNT_FLAG			 (DBM_DELTA + DBM_Q_SET_FRAUDULENT_ACCOUNT_FLAG) // 8584 // 46[s] - reqId,errCode,[errDesc]
#define DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG			 7585 // 4O4s - reqId,MessageInfoStruct,userIntId,comment
#define DBM_A_CLEAR_FRAUDULENT_ACCOUNT_FLAG			 (DBM_DELTA + DBM_Q_CLEAR_FRAUDULENT_ACCOUNT_FLAG) // 8585 // 46[s] - reqId,errCode,[errDesc]

//	PYR-72821
#define DBM_Q_LDBM_ADMIN_REMOVE_HANGING_LOGINS		7586 // 448btt - reqId,objectType,ObjectId,allObjectIds,startTime,endTime
#define DBM_A_LDBM_ADMIN_REMOVE_HANGING_LOGINS		(DBM_DELTA + DBM_Q_LDBM_ADMIN_REMOVE_HANGING_LOGINS)	// 8586 // 46[s|4] - reqId,errCode,[errDesc|numUsersToProcess]

//	PYR-68134, Admin->LoginDbm
#define DBM_Q_LDBM_ADMIN_ASSIGN_RANDOM_PIN			7587 // 4sb4s - reqId,userId,isTempNotPermanent,emailTemplateId,userComment
#define DBM_A_LDBM_ADMIN_ASSIGN_RANDOM_PIN			(DBM_DELTA + DBM_Q_LDBM_ADMIN_ASSIGN_RANDOM_PIN)		//	8587 // 46[s|4] - reqId,errCode,[errDesc]

//	PYR-68134, LoginDbm->Dbm
#define DBM_Q_DBM_ASSIGN_SID_AUTH_FLAGS				7588 //	4ssss4s - reqId,userId,secureId,adminId,adminIntId,comment
#define DBM_A_DBM_ASSIGN_SID_AUTH_FLAGS				(DBM_DELTA + DBM_Q_DBM_ASSIGN_SID_AUTH_FLAGS)			//	8588 // 46[s|4] - reqId,errCode,[errDesc]
// PYR-70835 Cashier -> OLAP
#define DBM_Q_SAVE_PAYSYSTEM_CONSENT			 7589	//	444s8 - reqId,userIntId,consentValue,Paysystem,appLoginId
#define DBM_A_SAVE_PAYSYSTEM_CONSENT			 (DBM_DELTA + DBM_Q_SAVE_PAYSYSTEM_CONSENT)	// 8589 //	46[s|4] - reqId,errCode,[errDesc|consentId]

//	PYR-74340
#define DBM_Q_DRC_NOTIFY_LONG_RUNNING_SERVICE		 7590	// 484 - reqId,tableId,numLongRunningSevices;
#define DBM_A_DRC_NOTIFY_LONG_RUNNING_SERVICE		 (DBM_DELTA + DBM_Q_DRC_NOTIFY_LONG_RUNNING_SERVICE)	// 8590 	// 46[s] - reqId, errCode[errDescr]

//#define DBM_Q_AVAILABLE_MESSAGE	7591  //4s2 reqId, userId, indiaCurrentTaxYear

// PYR-64373 LoginDbm->Dbm
#define DBM_Q_INSERT_INDIA_ARCH_USER_ZERO_ANNUAL_TAX	7592 // 44s244 - reqId,userIntId,fiscalCode,indiaUserCurrentTaxYear,balanceBeforeTaxation,taxableAmount
#define DBM_A_INSERT_INDIA_ARCH_USER_ZERO_ANNUAL_TAX	(DBM_DELTA + DBM_Q_INSERT_INDIA_ARCH_USER_ZERO_ANNUAL_TAX) // 8592 // 46[s] reqId, err_Code [errDescr]

// PYR-66028 JobsController -> Dbm
#define DBM_Q_GET_IT_ACCOUNTS_FOR_STATUS_CHECK 		7593 // 4 - reqId
#define DBM_A_GET_IT_ACCOUNTS_FOR_STATUS_CHECK 		(DBM_DELTA + DBM_Q_GET_IT_ACCOUNTS_FOR_STATUS_CHECK) // 8593 46[s|4<ItAccountToCheck>] - reqId,errCode[errDescr|vec_size<ItAccountToCheck>]

// PYR-66028 admin -> OLAP
#define DBM_Q_GET_PGAD_ACCT_STATUS			 		7594 // 4 - reqId,userIntId
#define DBM_A_GET_PGAD_ACCT_STATUS 					(DBM_DELTA + DBM_Q_GET_PGAD_ACCT_STATUS) // 8594 46[s|M<PgadTransactEssential>] - reqId,errCode[errDescr|<PgadTransactEssential>]

// PYR-69310 cashier -> RW DBM
#define DBM_Q_UPDATE_REFCONVRATES					7595 // 44<RefConvRateItem> reqId, vectorSize, <RefConvRateItem>
#define DBM_A_UPDATE_REFCONVRATES					(DBM_DELTA + DBM_Q_UPDATE_REFCONVRATES) // 8595 //46[s] reqId, errCode[errDescr] 

// PYR-71294 crmdbm -> RW DBM
#define DBM_Q_SET_USER_RG_RISK_LEVEL				7596 // 4s44b4bs44 488488
#define DBM_A_SET_USER_RG_RISK_LEVEL				(DBM_DELTA + DBM_Q_SET_USER_RG_RISK_LEVEL) // 8596 //46[s] reqId, errCode[errDescr] 

// PYR-67877 Collusion -> Olap
#define DBM_Q_GET_USERS_BY_TRACKCODES				7597 // 44<s> - reqId, nTcode<trackCode>
#define DBM_A_GET_USERS_BY_TRACKCODES				(DBM_DELTA + DBM_Q_GET_USERS_BY_TRACKCODES) // 8597 //46[s|4<s4<4>>] reqId, errCode[errDescr|nTC<trckCode nUids<uid>>] 

//	PYR-77065
#define DBM_Q_GET_SE_USER_WINNINGS					7598  // 44t - reqId, userIntId, loginTime
#define DBM_A_GET_SE_USER_WINNINGS					(DBM_DELTA + DBM_Q_GET_SE_USER_WINNINGS) // 8598  //46[s|8] - reqId, err [errDescr| totalWon] 

// PYR-61891 JobsController -> Dbm
#define DBM_Q_GET_USERS_FOR_ARJEL_REPORT_PENDING_OFFLINE_MESSAGES 7599  // 4 - reqId
#define DBM_A_GET_USERS_FOR_ARJEL_REPORT_PENDING_OFFLINE_MESSAGES (DBM_DELTA + DBM_Q_GET_USERS_FOR_ARJEL_REPORT_PENDING_OFFLINE_MESSAGES) // 8599  46[s|4<4>] - reqId,errCode[errDescr|vec_size<userIntId>]

// PYR-63165 Regulator -> Olap
#define DBM_Q_GET_BE_UNVALIDATED_USERS				7600 // 4 - reqId
#define DBM_A_GET_BE_UNVALIDATED_USERS				(DBM_DELTA + DBM_Q_GET_BE_UNVALIDATED_USERS) // 8600 //46[s|4<s4<4>>] reqId, errCode[errDescr|numUserProperties] 

// PYR-70907 JobsController -> Olap
#define DBM_Q_GET_BE_USERS_BY_TOKEN_MIGRATION_DATA	7601 // 4s - reqId,oldToken
#define DBM_A_GET_BE_USERS_BY_TOKEN_MIGRATION_DATA	(DBM_DELTA + DBM_Q_GET_BE_USERS_BY_TOKEN_MIGRATION_DATA) // 8601 //46[s|4<>] reqId, errCode[errDescr|vec_size<UserWithProperties>] 

// PYR-78900 swedishgateway -> OLAP
#define DBM_Q_SE_NON_FINANCIAL_REPORTS_DATA			7602  // 4tt - reqId startTime endTime(inclusive)
#define DBM_A_SE_NON_FINANCIAL_REPORTS_DATA			(DBM_DELTA + DBM_Q_SE_NON_FINANCIAL_REPORTS_DATA) // 8602  // 42[s|4<M>]

//PYR-78149, regulator to RO DBM
#define DBM_Q_GET_GENERIC_ACC_VALIDATION                                 7603  //4s - reqId,userId
#define DBM_A_GET_GENERIC_ACC_VALIDATION                                 (DBM_DELTA + DBM_Q_GET_GENERIC_ACC_VALIDATION) // 8603 //46[s|<rec>,b] - reqId,errCode,[errDesc|<AccVerification record>]

// PYR-77866 regulator,admin,... -> RW DBM
#define DBM_Q_UPDATE_BANBYREGULATOR					7604  // From Regulator: 4s4s4444 - reqId, adminId, adminIntId, userId, userIntId, eBanByRegulatorReasons to set, eBanByRegulatorReasons to remove, eUpdateType. From Admin: 4s4444 - reqId, userId, userIntId, eBanByRegulatorReasons to set, eBanByRegulatorReasons to remove, eUpdateType
#define DBM_A_UPDATE_BANBYREGULATOR					(DBM_DELTA + DBM_Q_UPDATE_BANBYREGULATOR) // 8604  // 42[s|b4] resultBannedByRegulatorFlag, resultBanByRegulatorReasons

// PYR-76937 replays improvements
#define DBM_Q_GET_TOURNSWITHREPLAYS                 7605 	//4 - reqId
#define DBM_A_GET_TOURNSWITHREPLAYS                 (DBM_DELTA + DBM_Q_GET_TOURNSWITHREPLAYS) // 8605 	//46[s|4<8sTTb>] - reqId, err_code[errDescr|vec_size<tournId, tournName, tournStartTime, tournEndTime>] 

#define DBM_Q_GET_REPLAY_BY_ID                      7606 	//48 reqId, tournId
#define DBM_A_GET_REPLAY_BY_ID                      (DBM_DELTA + DBM_Q_GET_REPLAY_BY_ID) // 8606 	//46[s|4B4<4<SS>>] - reqId, err_code[errDescr|replayId, script, iniFileSize<sectionSize<item, name>>] 

// PYR-78227
#define DBM_Q_MIGRATION_SWEDEN_CHECK				7607
#define DBM_A_MIGRATION_SWEDEN_CHECK				(DBM_DELTA + DBM_Q_MIGRATION_SWEDEN_CHECK) // 8607

// PYR-78227
#define DBM_Q_MIGRATION_SWEDEN_PERFORM				7608
#define DBM_A_MIGRATION_SWEDEN_PERFORM				(DBM_DELTA + DBM_Q_MIGRATION_SWEDEN_PERFORM) // 8608

#define DBM_Q_GET_APPROVED_GAME_LIMITS_AND_TIMES    7609 // 4s4<4> - reqId, userIntId, size<val>
#define DBM_A_GET_APPROVED_GAME_LIMITS_AND_TIMES    (DBM_DELTA + DBM_Q_GET_APPROVED_GAME_LIMITS_AND_TIMES) // 8609 // 46[s|4<b44s44bt>] - reqId, errCode[errDescr| size<limitExists eTableLimitType AmountInCurrency autoApproveHours waitingIncreaseLimitDays hadPrevLimitUK grantedTime] 

// PYR-81917
#define DBM_Q_GET_CDH_USERINFO						7610 // 44<M(4s)> - reqId, sz, <M(userIntId, userId)>
#define DBM_A_GET_CDH_USERINFO						(DBM_DELTA + DBM_Q_GET_CDH_USERINFO) // 8610 // 42[s|4<M(4s44)>] - reqId, errCode[errDescr|sz<M(userIntId, userId, accountAgeSecs, LtFpp)>]

// PYR-78504 swedishgateway -> OLAP
#define DBM_Q_SE_FINANCIAL_REPORTS_DATA				7611  // 4tt - reqId startTime endTime(inclusive)
#define DBM_A_SE_FINANCIAL_REPORTS_DATA				(DBM_DELTA + DBM_Q_SE_FINANCIAL_REPORTS_DATA) // 8611  // 42[s|4<M>]

// PYR-77215
#define DBM_Q_SE_GET_REPORT_CONFIG_DATA				7612 // 4 - reqId
#define DBM_A_SE_GET_REPORT_CONFIG_DATA				(DBM_DELTA + DBM_Q_SE_GET_REPORT_CONFIG_DATA) // 8612

// PYR-80333
#define DBM_Q_LDBM_VIEW_USER_INFO_NO_USER			7613 // 4s44<s>s - reqId, adminUserId, adminUserIntId, size<adminRights>, userId
#define DBM_A_LDBM_VIEW_USER_INFO_NO_USER			(DBM_DELTA + DBM_Q_LDBM_VIEW_USER_INFO_NO_USER) // 8613  // 42[s] - reqId, err_code[errDescr]

// PYR-80333
#define DBM_Q_LDBM_UPDATE_USER_PWD					7614 // 4s44<s>s - reqId, adminUserId, adminUserIntId, size<adminRights>, userId
#define DBM_A_LDBM_UPDATE_USER_PWD					(DBM_DELTA + DBM_Q_LDBM_UPDATE_USER_PWD) // 8614 // 42[s] - reqId, err_code[errDescr]

// PYR-83160 casino races
#define DBM_Q_XC_RACE_RESULT						7615
#define DBM_A_XC_RACE_RESULT						(DBM_DELTA + DBM_Q_XC_RACE_RESULT) // 8615

// PYR-83111
#define DBM_Q_MTL_UPDATE_FLIGHT_INTERNAL_ACCOUNTS	7616 // 48b48o - reqId, tournId, isTargetTourn, maxVPPMultiplierX100, fromTournRake, fromFlightInfo
#define DBM_A_MTL_UPDATE_FLIGHT_INTERNAL_ACCOUNTS	(DBM_DELTA + DBM_Q_MTL_UPDATE_FLIGHT_INTERNAL_ACCOUNTS) // 8616 // 46[s|8] - reqId, errCode[errDescr|fromTournRake]

// PYR-83998
#define DBM_Q_SPAIN_GET_USERS_LOGIN_DATA				7617 // 44<s4> - reqId,size<UserIdPair(userId,userIntId)>
#define DBM_A_SPAIN_GET_USERS_LOGIN_DATA				(DBM_DELTA + DBM_Q_SPAIN_GET_USERS_LOGIN_DATA) // 8617 // 42[s|4<4O>] req,errCode[errDescr|size<userIntId,EsLoginData>]

// PYR-83111
#define DBM_Q_MTL_MOVE_USER_FLIGHT_INTERNAL_ACCOUNTS	7618 // 4844 - reqId, tournId, deltaRM, deltaRake
#define DBM_A_MTL_MOVE_USER_FLIGHT_INTERNAL_ACCOUNTS	(DBM_DELTA + DBM_Q_MTL_MOVE_USER_FLIGHT_INTERNAL_ACCOUNTS) // 8618 // 46[s] - reqId, errCode[errDescr]

// PYR-79883
#define DBM_Q_GET_NEXT_ID							7619 // 4ss - reqId, objectName, userId
#define DBM_A_GET_NEXT_ID							(DBM_DELTA + DBM_Q_GET_NEXT_ID) // 8619 // 46[s|8tt] - reqId, errCode[errDescr | newId,dbmTime,dbmHearbeatUtc]

// PYR-79883
#define DBM_Q_REQUEST_REAL_TRANSFER_PAR             7622  // 4s4ss4<>4M - reqId, userIdFrom, amountFrom, currencyFrom, comment, transferProcessing, appLoginEssentials, transToId, userDataTo
#define DBM_A_REQUEST_REAL_TRANSFER_PAR             (DBM_DELTA + DBM_Q_REQUEST_REAL_TRANSFER_PAR) // 8622 // 46[s|4bM] - regId, errCode[errDescr|transFromId, okForAutoTransfer, cashTransBody]

// PYR-81664
#define DBM_Q_STARSSTORE_NEED_DATA					7623 // 4 reqId
#define DBM_A_STARSSTORE_NEED_DATA					(DBM_DELTA + DBM_Q_STARSSTORE_NEED_DATA)  // 8623 46[s|4<s8>]  -  reqId errCode [errDescr|numItems<name,licenseFlags>] uses IniRulesMap::compose/parse

// PYR-76141
#define DBM_Q_SAVE_SUGGESTEDDEPOSIT					7624 // 44S2SM(4488)4<S>4<S>4S4<S>4444<S4<4>>4B4<S4<4>>B4<S4<4>>4B44 - reqId SuggestedDepositCampaign
#define DBM_A_SAVE_SUGGESTEDDEPOSIT					(DBM_DELTA + DBM_Q_SAVE_SUGGESTEDDEPOSIT) // 8624 // 46[S|4] - reqId, errcode, [errMsg|suggestedDepositId]

#define DBM_Q_GET_ALL_ACTIVE_SUGGESTEDDEPOSITS		7625 // 4 - reqId
#define DBM_A_GET_ALL_ACTIVE_SUGGESTEDDEPOSITS		(DBM_DELTA + DBM_Q_GET_ALL_ACTIVE_SUGGESTEDDEPOSITS) // 8625 46[s|4<O>] - reqId, errCode, [errMsg | numRecords <SuggestedDepositCampaign>]

// PYR-89650
#define DBM_Q_MIGRATION_SWISS_CHECK					7626 // 4 - reqId
#define DBM_A_MIGRATION_SWISS_CHECK					(DBM_DELTA + DBM_Q_MIGRATION_SWISS_CHECK) // 8626 46[S] - reqId, errcode, [errMsg]

// PYR-89650
#define DBM_Q_MIGRATION_SWISS_PERFORM				7627 // 4 - reqId
#define DBM_A_MIGRATION_SWISS_PERFORM				(DBM_DELTA + DBM_Q_MIGRATION_SWISS_PERFORM) // 8627 46[S] - reqId, errcode, [errMsg]

// PYR-80956
#define DBM_Q_RG_SUSPEND_RM							7628 // 4ss - reqId, userId, quote_and_timestamp
#define DBM_A_RG_SUSPEND_RM							(DBM_DELTA + DBM_Q_RG_SUSPEND_RM) // 8628 46[S] - reqId, errcode, [errMsg]

// PYR-91513
#define DBM_Q_CZ_GET_LAST_REPORT_TS_CET				7629 // 4 - reqId
#define DBM_A_CZ_GET_LAST_REPORT_TS_CET				(DBM_DELTA + DBM_Q_CZ_GET_LAST_REPORT_TS_CET) // 8629 46[S|t] - reqId, errcode, [errMsg|SrvTime]

// PYR-91513
#define DBM_Q_CZ_UPDATE_LAST_REPORT_TS_CET			7630 // 4t - reqId, SrvTime
#define DBM_A_CZ_UPDATE_LAST_REPORT_TS_CET			(DBM_DELTA + DBM_Q_CZ_UPDATE_LAST_REPORT_TS_CET) // 8630 46[S] - reqId, errcode, [errMsg]

// PYR-92307
#define DBM_Q_CZ_GET_SPORTSBOOK_DATA				7631 // 4tt - reqId, startTime, endTime(noninclusive)
#define DBM_A_CZ_GET_SPORTSBOOK_DATA				(DBM_DELTA + DBM_Q_CZ_GET_SPORTSBOOK_DATA) // 8631 46[s|O] - reqId, errCode, [errDescr|CzCoreGateway::SpbPlayerGameRecord]

// PYR-92307
#define DBM_Q_CZ_POKER_GAMES_REPORTS_DATA			7632  // 4tt - reqId, startTime, endTime(noninclusive)
#define DBM_A_CZ_POKER_GAMES_REPORTS_DATA			(DBM_DELTA + DBM_Q_CZ_POKER_GAMES_REPORTS_DATA) // 8632  // 42[s|4<M>]

// PYR-91513
#define DBM_Q_CZ_GET_USER_ACCOUNTS					7633 // 4t - reqId, endTime(noninclusive)
#define DBM_A_CZ_GET_USER_ACCOUNTS					(DBM_DELTA + DBM_Q_CZ_GET_USER_ACCOUNTS) // 8633  // 42[s|4<O>] reqId, errCode, [errDescr|size<CzCoreGateway::UserAccountRecord>]

// PYR-92307
#define DBM_Q_CZ_POKER_TOURNS_REPORTS_DATA			7634  // 4tt - reqId, startTime, endTime(inclusive)
#define DBM_A_CZ_POKER_TOURNS_REPORTS_DATA			(DBM_DELTA + DBM_Q_CZ_POKER_TOURNS_REPORTS_DATA) // 8634  // 42[s|4<M>]

// PYR-91513
#define DBM_Q_CZ_GET_UPDATED_USER_ACCOUNTS 			7635 // 4tt - reqId, startTime, endTime(noninclusive)
#define DBM_A_CZ_GET_UPDATED_USER_ACCOUNTS			(DBM_DELTA + DBM_Q_CZ_GET_UPDATED_USER_ACCOUNTS) // 8635  // 42[s|4<O>] reqId, errCode, [errDescr|size<CzCoreGateway::UpdatedUserAccountRecord >]

// PYR-92506
#define DBM_Q_CZ_GET_POKER_GAME_SESSION_DATA		7636 // 4tt - reqId, startTime, endTime(noninclusive)
#define DBM_A_CZ_GET_POKER_GAME_SESSION_DATA		(DBM_DELTA + DBM_Q_CZ_GET_POKER_GAME_SESSION_DATA) // 8636  // 42[s|4<O>] reqId, errCode, [errDescr|size<CzCoreGateway::PokerGameRecord>]

// PYR-92506
#define DBM_Q_CZ_GET_POKER_TOURN_SESSION_DATA		7637 // 4tt - reqId, startTime, endTime(noninclusive)
#define DBM_A_CZ_GET_POKER_TOURN_SESSION_DATA		(DBM_DELTA + DBM_Q_CZ_GET_POKER_TOURN_SESSION_DATA) // 8637  // 42[s|4<O>] reqId, errCode, [errDescr|size<CzCoreGateway::PokerTournRecord>]

// PYR-97438
#define DBM_Q_CHECK_NEW_DEVICE_PAYMENT				7638 // 4 - reqId
#define DBM_A_CHECK_NEW_DEVICE_PAYMENT				(DBM_DELTA + DBM_Q_CHECK_NEW_DEVICE_PAYMENT) // 8627 46[S] - reqId, errcode, [errMsg]

//PYR-97209
#define DRC_Q_TABLE_RESERVATION_RECORDS				7639 //484<48822> - reqId,tableId,size<userId,start,end,code,servCode>
#define DRC_A_TABLE_RESERVATION_RECORDS				(DBM_DELTA + DRC_Q_TABLE_RESERVATION_RECORDS)       // 8639  // 46[S] - reqId, errcode, [errMsg]

//PYR-99198
#define DBM_Q_SAVE_TGM_FLIGHT_USER_INFO			7640 // 44<O> - reqId, numData<FlightUserBuyInInfo>
#define DBM_A_SAVE_TGM_FLIGHT_USER_INFO			(DBM_DELTA + DBM_Q_SAVE_TGM_FLIGHT_USER_INFO) // 8640  // 42[s] reqId, errCode, [errDescr]

//PYR-99198
#define DBM_Q_DELETE_TGM_FLIGHT_USER_INFO			7641 // 44<8> - reqId, numData<buyInTransId>
#define DBM_A_DELETE_TGM_FLIGHT_USER_INFO			(DBM_DELTA + DBM_Q_DELETE_TGM_FLIGHT_USER_INFO) // 8641  // 42[s] reqId, errCode, [errDescr]

// PYR-92627
#define DBM_Q_CZ_GET_MONEY_MOVEMENTS				7642 // 4tt - reqId, startTime, endTime(inclusive)
#define DBM_A_CZ_GET_MONEY_MOVEMENTS				(DBM_DELTA + DBM_Q_CZ_GET_MONEY_MOVEMENTS) // 8642  // 42[s|4<O>] reqId, errCode, [errDescr|size<CzCoreGateway::MoneyMovementRecord>]

// PYR-94417
#define DBM_Q_CDMS_ICE_EVENT						7643 // 4s11 - reqId, userId, docType, docSubCat
#define DBM_A_CDMS_ICE_EVENT						(DBM_DELTA + DBM_Q_CDMS_ICE_EVENT) // 8643 46[s] - reqId, errcode, [errMsg]

//PYR-99198
#define DBM_Q_UPDATE_TGM_FLIGHT_USER_INFO			7644 // 44<84> - reqId, numData<buyInTransId, awarded>
#define DBM_A_UPDATE_TGM_FLIGHT_USER_INFO			(DBM_DELTA + DBM_Q_UPDATE_TGM_FLIGHT_USER_INFO) // 8644  // 42[s] reqId, errCode, [errDescr]

// PYR-100733
#define DBM_Q_SAVE_THROWABLE_EVENTS					7645 // 4814<M(8444)> - reqId, tableId, handType, numEvts< M(when, sourceUserIntId, targetUserIntId, throwableTypeId) >
#define DBM_A_SAVE_THROWABLE_EVENTS					(DBM_DELTA + DBM_Q_SAVE_THROWABLE_EVENTS) // 8645  // 42[s] reqId, errCode, [errDescr]

// PYR-96138
#define DBM_Q_CZ_GET_REPORT_SELF_LIMITS_DATA		7646 // 4tt - reqId, startTime, endTime
#define DBM_A_CZ_GET_REPORT_SELF_LIMITS_DATA		(DBM_DELTA + DBM_Q_CZ_GET_REPORT_SELF_LIMITS_DATA) // 8646  // 42[s|4<O>] reqId, errCode, [errDescr|size<CzCoreGateway::TableLimitsRecord>]

// PYR-102238
#define DBM_Q_MODIFY_AGE_VERIFIED_WITH_ADMIN		7647 // 4s1ss4 reqId userId setNotRemove comment adminId adminIntId
#define DBM_A_MODIFY_AGE_VERIFIED_WITH_ADMIN		(DBM_DELTA + DBM_Q_MODIFY_AGE_VERIFIED_WITH_ADMIN) // 8647  // 42[s] reqId err_code [errDescr]

// PYR-102238
#define DBM_Q_MODIFY_AUTH_VERIFIED_WITH_ADMIN		7648 // 4s1ss4 reqId userId setNotRemove comment adminId adminIntId
#define DBM_A_MODIFY_AUTH_VERIFIED_WITH_ADMIN		(DBM_DELTA + DBM_Q_MODIFY_AUTH_VERIFIED_WITH_ADMIN) // 8648 // 42[s] reqId err_code [errDescr]

//PYR-95387
#define DBM_Q_CZ_GET_ONLINE_TOURNAMENTS				7650 //4tt - reqId, startTime, endTime (non-inclusive)
#define DBM_A_CZ_GET_ONLINE_TOURNAMENTS				(DBM_DELTA + DBM_Q_CZ_GET_ONLINE_TOURNAMENTS) // 8650 // 42[s|4<O>] reqId err_code [errDescr|size<CzCoreGateway::OnlineTournamentInfo>]

// PYR-95672
#define DBM_Q_CZ_GET_USER_SESSION_DATA				7651 //4tt - reqId, startTime, endTime (non-inclusive)
#define DBM_A_CZ_GET_USER_SESSION_DATA				(DBM_DELTA + DBM_Q_CZ_GET_USER_SESSION_DATA) // 8651 // 42[s|4<O>] reqId err_code [errDescr|size<CzCoreGateway::UserSessionRecord>]

// PYR-101785
#define DBM_Q_BL_RECONCILE_LOAN						7652	//44s4s4b48 - reqId, blitzId, userId, entryNum, currency, chipsLoan, isPlayMoney, scalePM, appSessionId
#define DBM_A_BL_RECONCILE_LOAN						(DBM_DELTA + DBM_Q_BL_RECONCILE_LOAN) // 8652 // 42[s|] reqId err_code [errDescr|]

// PYR-106112
#define DBM_Q_GENERATE_USER_ALIASES					7653 //44 - reqId, numAliases
#define DBM_A_GENERATE_USER_ALIASES					(DBM_DELTA + DBM_Q_GENERATE_USER_ALIASES) // 8653 // 42[s|4<s>] reqId err_code [errDescr|size<alias>]

// PYR-106112
#define DBM_Q_SET_USER_ALIASES						7654 //44484st - reqId,tableTypeId,blitzId,tableId,userIntId,alias,assigned
#define DBM_A_SET_USER_ALIASES						(DBM_DELTA + DBM_Q_SET_USER_ALIASES) // 8654 // 42[s] reqId err_code [errDescr]

// PYR-106112
#define DBM_Q_ADD_ALIAS_BLOCKS						7655 //44<M(2s)> - reqId, numBlocks<M(blockNum, block)>
#define DBM_A_ADD_ALIAS_BLOCKS						(DBM_DELTA + DBM_Q_ADD_ALIAS_BLOCKS) // 8655 // 42[s]4<ss> - reqId,err_code[errDescr]

//PYR-103380
#define DRC_Q_TABLE_WAITLIST_RECORDS				7656 // see srvshared/drcserverprot.txt
#define DRC_A_TABLE_WAITLIST_RECORDS				(DBM_DELTA + DRC_Q_TABLE_WAITLIST_RECORDS)       // see srvshared/drcserverprot.txt

// PYR-106112
#define DBM_Q_DEL_ALIAS_BLOCK						7657 //42s - reqId, blockNum, block
#define DBM_A_DEL_ALIAS_BLOCK						(DBM_DELTA + DBM_Q_DEL_ALIAS_BLOCK) // 8657 // 42[s] - reqId,err_code[errDescr]

#define DBM_Q_FORFEIT_FLIGHT_USER_STACK				7658 //4O reqId, FlightUserStackForfeitDbmInfo
#define DBM_A_FORFEIT_FLIGHT_USER_STACK				(DBM_DELTA + DBM_Q_FORFEIT_FLIGHT_USER_STACK) // 8658 // 42[s|] reqId err_code [errDescr|]

#define DBM_Q_GET_PENDING_FLIGHT_STACKS				7659 // 44<8> reqId, num<tournId>
#define DBM_A_GET_PENDING_FLIGHT_STACKS				(DBM_DELTA + DBM_Q_GET_PENDING_FLIGHT_STACKS) // 8659 // 42[s|4<O>] reqId err_code [errDescr| num<FlightUserStackForfeitDbmInfo>]

// PYR-105214 Cashier to Dbm
#define DBM_Q_APPROVE_TRANSFER_CHECK_RO				7660 // 4ss444sb4 - reqId, userFrom, userTo, transFromId, transToId, transferAmount, comment, awareOfBan, transferProcessing (same as DBM_Q_COMPLETE_REAL_TRANSFER)
#define DBM_A_APPROVE_TRANSFER_CHECK_RO				(DBM_DELTA + DBM_Q_APPROVE_TRANSFER_CHECK_RO)	// 8660 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_APPROVE_TRANSFER_PART1_TO				7661 // 4O - reqId, TransferActionContext
#define DBM_A_APPROVE_TRANSFER_PART1_TO				(DBM_DELTA + DBM_Q_APPROVE_TRANSFER_PART1_TO)	// 8661 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_APPROVE_TRANSFER_PART2_FROM			7662 // 4O - reqId, TransferActionContext
#define DBM_A_APPROVE_TRANSFER_PART2_FROM			(DBM_DELTA + DBM_Q_APPROVE_TRANSFER_PART2_FROM)	// 8662 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_GET_BE_APPROVED_LIMITS_AUDIT			7663 // 4 - reqId
#define DBM_A_GET_BE_APPROVED_LIMITS_AUDIT			(DBM_DELTA + DBM_Q_GET_BE_APPROVED_LIMITS_AUDIT) // 8663 // 42[s|44<O,4<O>>] reqId err_code [errDescr| oldRegulatoryLimit size< UserWithProperties,size<LimitsAuditData>>]

// PYR-102057 Cashier to Dbm
#define DBM_Q_REJECT_TRANSFER_CHECK_RO				7664  // 4ss444ss - reqId, userFrom, userTo, transFromId, transToId, transferAmount, comment, errorCode (same as DBM_Q_REJECT_REAL_TRANSFER)
#define DBM_A_REJECT_TRANSFER_CHECK_RO				(DBM_DELTA + DBM_Q_REJECT_TRANSFER_CHECK_RO)	// 8664 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_REJECT_TRANSFER_PART1_FROM			7665 // 4O - reqId, TransferActionContext
#define DBM_A_REJECT_TRANSFER_PART1_FROM			(DBM_DELTA + DBM_Q_REJECT_TRANSFER_PART1_FROM)	// 8665 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_REJECT_TRANSFER_PART2_TO				7666 // 4O - reqId, TransferActionContext
#define DBM_A_REJECT_TRANSFER_PART2_TO				(DBM_DELTA + DBM_Q_REJECT_TRANSFER_PART2_TO)	// 8666 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_CLOSE_ONE_ACCOUNT						7667     //4sb7ss reqId, commentProps, keepPrivileges, negativeCategory, commentUsers, userId
#define DBM_A_CLOSE_ONE_ACCOUNT                     (DBM_DELTA + DBM_Q_CLOSE_ONE_ACCOUNT) // 8667     // 46[s] req_id, err_code [errDescr]

#define DBM_Q_COMMENT_ONE_ACCOUNT                   7668     //4ss reqId, commentUsers, userId
#define DBM_A_COMMENT_ONE_ACCOUNT                   (DBM_DELTA + DBM_Q_COMMENT_ONE_ACCOUNT) // 8668     // 46[s] req_id, err_code [errDescr]

//obsolete, DBM_Q_AVAILABLE_MESSAGE
//#define DBM_Q_ISSUE_BOP_PRIZES_ONE_USER             7669     // 4s44444s - reqId, userId, intervalId, division, subdivision, amount, ticketTypeId, comment, currency
//#define DBM_A_ISSUE_BOP_PRIZES_ONE_USER             (DBM_DELTA + DBM_Q_ISSUE_BOP_PRIZES_ONE_USER) // 8669     // 46[s|4<s46s>] - reqId errCode[errDescr|numErrors<userId, intervalId, errCode, errDescr>] 

#define DBM_Q_ISSUE_LEAGUE_PRIZES_ONE_USER          7670  // 4s444sb - reqId, userId, leagueId, amount, ticketTypeId, comment, currency, isTMoney
#define DBM_A_ISSUE_LEAGUE_PRIZES_ONE_USER          (DBM_DELTA + DBM_Q_ISSUE_LEAGUE_PRIZES_ONE_USER) // 8670  // 46[s|4<s46s>] - reqId, errCode[errDescr|numErrors<userId, leagueId, errCode, errDescr>] 

#define DBM_Q_GET_USERID_BY_AFFILIATE_ACCTID        7671  // 44 - reqId, accountId
#define DBM_A_GET_USERID_BY_AFFILIATE_ACCTID        (DBM_DELTA + DBM_Q_GET_USERID_BY_AFFILIATE_ACCTID) // 8671  // 46[s|s] - reqId,errCode,[errDescr | userId]

#define DBM_Q_USER_UPDATE2_WITH_ADMINID             7672  // 4s4<4M>44s - reqId, userId, numUpdates<updateType, updateBody>, eUpdateType adminIntId adminId 
#define DBM_A_USER_UPDATE2_WITH_ADMINID             (DBM_DELTA + DBM_Q_USER_UPDATE2_WITH_ADMINID) // 8672  // 46[s|s4444sssssss8bb8] - reqId, errCode[errDescr|userId, unique, imageId, effectivePrivs, flags, fullName, addr_1, addr_2, city, state, country, zipCode, effectivePrivs2, notifyItalianClient, userIsPlayMoney, effectivePrivs3] 

// PYR-84803
#define DBM_Q_GET_FT_GDPR_CANDIDATES				7673 // 4 - reqId
#define DBM_A_GET_FT_GDPR_CANDIDATES				(DBM_DELTA + DBM_Q_GET_FT_GDPR_CANDIDATES) // 8673  // 46[s|4<4>] - reqId, errCode[errDescr|size<playerIX>]

// PYR-84803
#define DBM_Q_ANONYMIZE_FT_USERS					7674 // 44<4> - reqId, size<playerIX>
#define DBM_A_ANONYMIZE_FT_USERS					(DBM_DELTA + DBM_Q_ANONYMIZE_FT_USERS) // 8674  // 46[s] - reqId, errCode[errDescr]

// PYR-79883 Cashier to Dbm
#define DBM_Q_INIT_TRANSFER_CHECK_RO				7675  // 4ss4ss4O - reqId, userFrom, userTo, amountFrom, currencyFrom, comment, transferProcessing, senderAppLogin:AppLoginEssentials (same as DBM_Q_REQUEST_REAL_TRANSFER)
#define DBM_A_INIT_TRANSFER_CHECK_RO				(DBM_DELTA + DBM_Q_INIT_TRANSFER_CHECK_RO)		// 8675 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_INIT_TRANSFER_PART1_FROM				7676 // 4O - reqId, TransferActionContext
#define DBM_A_INIT_TRANSFER_PART1_FROM				(DBM_DELTA + DBM_Q_INIT_TRANSFER_PART1_FROM)	// 8676 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_INIT_TRANSFER_PART2_TO				7677 // 4O - reqId, TransferActionContext
#define DBM_A_INIT_TRANSFER_PART2_TO				(DBM_DELTA + DBM_Q_INIT_TRANSFER_PART2_TO)		// 8677 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

// PYR-109834 Cashier to Dbm
#define DBM_Q_GET_USERID_BY_TRANSID					7678 // 44 - reqId, transId, eCashtransTableId
#define DBM_A_GET_USERID_BY_TRANSID					(DBM_DELTA + DBM_Q_GET_USERID_BY_TRANSID)		// 8678 // 42[s|s] - reqId,err_code[errDescr|userId]

// PYR-105425 Cashier to Dbm
#define DBM_Q_TRANSFER_DEAD_BALANCE_CHECK_RO		7679  // 4s4s - reqId, adminId, adminIntId, userId
#define DBM_A_TRANSFER_DEAD_BALANCE_CHECK_RO		(DBM_DELTA + DBM_Q_TRANSFER_DEAD_BALANCE_CHECK_RO)   // 8679 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_TRANSFER_DEAD_BALANCE_PART1_FROM		7680 // 4O - reqId, TransferActionContext
#define DBM_A_TRANSFER_DEAD_BALANCE_PART1_FROM		(DBM_DELTA + DBM_Q_TRANSFER_DEAD_BALANCE_PART1_FROM) // 8680 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_TRANSFER_DEAD_BALANCE_PART2_TO		7681 // 4O - reqId, TransferActionContext
#define DBM_A_TRANSFER_DEAD_BALANCE_PART2_TO		(DBM_DELTA + DBM_Q_TRANSFER_DEAD_BALANCE_PART2_TO)   // 8681 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

// PYR-109834 Cashier to Dbm
#define DBM_Q_SET_TOURNTICKET_EXPDATE_ONE_USER      7682  //4T4 reqId, expDate, ticketId
#define DBM_A_SET_TOURNTICKET_EXPDATE_ONE_USER      (DBM_DELTA + DBM_Q_SET_TOURNTICKET_EXPDATE_ONE_USER) // 8682  //46[s|s6s] reqId err_code [errDescr|ticketId err_code errDescr] 

#define DBM_Q_GET_USERID_BY_TICKET_ID				7683  // 44 - reqId, ticketId
#define DBM_A_GET_USERID_BY_TICKET_ID				(DBM_DELTA + DBM_Q_GET_USERID_BY_TICKET_ID) // 8683  // 46[s|s] - reqId,errCode,[errDescr | userId]

// PYR-90691 Cashier to Dbm
#define DBM_Q_INIT_TRANSFER_BY_ADMIN_CHECK_RO		7684  //4s4s4s11s reqId, userFrom, userFromBankroll, userTo, transferAmount, comment, awareOfBan, dummy, currencyFrom (same as DBM_Q_TRANSFER_FUNDS_BY_ADMIN)
#define DBM_A_INIT_TRANSFER_BY_ADMIN_CHECK_RO		(DBM_DELTA + DBM_Q_INIT_TRANSFER_BY_ADMIN_CHECK_RO)	// 8684 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_INIT_TRANSFER_BY_ADMIN_PART1_FROM		7685 // 4O - reqId, TransferActionContext
#define DBM_A_INIT_TRANSFER_BY_ADMIN_PART1_FROM		(DBM_DELTA + DBM_Q_INIT_TRANSFER_BY_ADMIN_PART1_FROM)	// 8685 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_INIT_TRANSFER_BY_ADMIN_PART2_TO		7686 // 4O - reqId, TransferActionContext
#define DBM_A_INIT_TRANSFER_BY_ADMIN_PART2_TO		(DBM_DELTA + DBM_Q_INIT_TRANSFER_BY_ADMIN_PART2_TO)	// 8686 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

// PYR-106819 CRM server to OLAP
#define DBM_Q_GET_MG_SIDEBET_HISTORY                7687  // 4sTT - reqId, userId, from, to
#define DBM_A_GET_MG_SIDEBET_HISTORY                (DBM_DELTA + DBM_Q_GET_MG_SIDEBET_HISTORY) // 8687  // 46[s|444<o>] - reqId, errCode, licenseId, locale, vector<MiniGameServer::MiniGameSideBetData>]

// PYR-109267
#define DRC_Q_GET_TOURN_TABLE_OBJECTS				7688 // see srvshared/drc/drcserverprot.txt
#define DRC_A_GET_TOURN_TABLE_OBJECTS				(DBM_DELTA + DRC_Q_GET_TOURN_TABLE_OBJECTS) // 8688  // see srvshared/drc/drcserverprot.txt

// PYR-111060
#define DBM_Q_BG_GET_POKER_SIDEBET_DATA             7689 // 4tt - reqId, startTime, endTime
#define DBM_A_BG_GET_POKER_SIDEBET_DATA             (DBM_DELTA + DBM_Q_BG_GET_POKER_SIDEBET_DATA) // 8689 46[s|4<O>] - reqId, errCode[errDescr|size<BgPokerSideBetResultMsg>]

#define DBM_Q_UPDATE_PAYSYSTEM_DATA					7690 // PYR-112209: 4<o> - reqId, vector<PaySystemDescriptors::PaySystemData>
#define DBM_A_UPDATE_PAYSYSTEM_DATA					(DBM_DELTA + DBM_Q_UPDATE_PAYSYSTEM_DATA) // 8690  // 42[s|] - reqId, errCode[errDescr|]

// PYR-102906 - Cashier to RW DBM
#define DBM_Q_FINALIZE_CCOIN_TRANS					7691 //4s4O - reqId, adminId, adminIntId, TransToFinalize
#define DBM_A_FINALIZE_CCOIN_TRANS					(DBM_DELTA + DBM_Q_FINALIZE_CCOIN_TRANS) // 8691 //442[s|O] - reqId, transId,err_code[errDescr|CashTransData]

#define DBM_Q_UPDATE_MASKED_CARDNUM                 7692 	//44s reqId transId cardNum
#define DBM_A_UPDATE_MASKED_CARDNUM                 (DBM_DELTA + DBM_Q_UPDATE_MASKED_CARDNUM) // 8692 	//42[s] reqId errCode [errDescr]

// PYR-105411 Cashier to RW DBM
#define DBM_Q_UPDATE_HOLD_UNKNOWN_WAITING_TRANS     7693   //4s441sss4 - reqId adminId adminIntId transId newstatus xTransStr errorInfo errCode eCashtransTableId
#define DBM_A_UPDATE_HOLD_UNKNOWN_WAITING_TRANS     (DBM_DELTA + DBM_Q_UPDATE_HOLD_UNKNOWN_WAITING_TRANS) // 8693     // 46[s|46s1] - reqId errCode[errDescr|transId err_code err_descr newStatus]

// PYR-114342
#define DBM_Q_GET_DEPOSIT_LIMIT_HIT_INFO			7694	//4s reqId userId
#define DBM_A_GET_DEPOSIT_LIMIT_HIT_INFO			(DBM_DELTA + DBM_Q_GET_DEPOSIT_LIMIT_HIT_INFO) // 8694	//42[s|O] reqId errCode [errDescr|DepositLimitHitInfo]

// PYR-105411 Cashier to RW DBM
#define DBM_Q_UPDATE_HOLD_BULK_FOR_USER             7695 	// 4s4s4<41sss4> - reqId adminId adminIntId userId num_updates<transId, newstatus, xTransStr, errorInfo, errCode, eCashtransTableId>
#define DBM_A_UPDATE_HOLD_BULK_FOR_USER             (DBM_DELTA + DBM_Q_UPDATE_HOLD_BULK_FOR_USER) // 8695     // 46[s|4<46s1>] - reqId errCode[errDescr|num_updates<transId, err_code, err_descr, newStatus>]

// PYR-105415 - Admin to RW Dbm
#define DBM_Q_SET_CASHTRANS_FLAGS                   7696  // 44884s - reqId transId flagsOld flagsNew eCashtransTableId userId
#define DBM_A_SET_CASHTRANS_FLAGS                   (DBM_DELTA + DBM_Q_SET_CASHTRANS_FLAGS) // 8696  // 442[s] reqId transId errCode errDescr

// PYR-104952 Collusion -> OLAP
#define DBM_Q_GET_LOGINAPP_DATA						7697  // 48 - reqId appLoginId
#define DBM_A_GET_LOGINAPP_DATA                    (DBM_DELTA + DBM_Q_GET_LOGINAPP_DATA) // 8697  // 46[s|s] reqId errCode [errDescr|installId]

// #PYR-111146 Admin -> RW Dbm
#define DBM_Q_SAVE_USBLACKLIST                      7698 // 4o reqId, UsBlacklistRecord
#define DBM_A_SAVE_USBLACKLIST                      (DBM_DELTA + DBM_Q_SAVE_USBLACKLIST) // 8698 6[s|UsBlacklistRecord]

// #PYR-111146 regulator -> RO Dbm
#define DBM_Q_US_BLACKLIST_CHECK				    7699 // 4ssdss reqId, userId, ssn4, dob, firstName, lastName
#define DBM_A_US_BLACKLIST_CHECK					(DBM_DELTA + DBM_Q_US_BLACKLIST_CHECK) // 8699 6[s]4<UsBlacklistRecord>

//PYR-88481
#define DBM_Q_SOCIAL_CASINO_NEED_DATA				7700 // 4 reqId
#define DBM_A_SOCIAL_CASINO_NEED_DATA				(DBM_DELTA + DBM_Q_SOCIAL_CASINO_NEED_DATA) // 8700 46[s|4<s8>] reqId errCode [errDescr|numItems<name,licenseFlags>]

// PYR-91999 - AB to RW DBM
#define DBM_Q_UPDATE_USERGROUP_LEVEL_USER           7701 	// 4s44 - reqId userId groupId levelId (only level value can be changed!)
#define DBM_A_UPDATE_USERGROUP_LEVEL_USER           (DBM_DELTA + DBM_Q_UPDATE_USERGROUP_LEVEL_USER) // 8701 	// 46[s] - reqId errCode[errDescr]

// PYR-91999 - AB to RW DBM
#define DBM_Q_CREDIT_VPP_USER                       7702  //4s46s4 - reqId userId amount amountCents comment ePlayerPointsType
#define DBM_A_CREDIT_VPP_USER                       (DBM_DELTA + DBM_Q_CREDIT_VPP_USER) // 8702     //46[s] - reqId errCode[errDescr]

// PYR-91999 - AB to RW DBM
#define DBM_Q_CREDIT_USER_BONUS                     7703  //44s44ss reqId bonusId userId amount fpp accountCurrency comment
#define DBM_A_CREDIT_USER_BONUS                     (DBM_DELTA + DBM_Q_CREDIT_USER_BONUS) // 8703  //46[s] - reqId errCode[errDescr]

// PYR-120795 Cashier to RO Dbm
#define DBM_Q_GET_USERID_BY_TRANSID_VEC             7704 // 444<4> - reqId, eCashtransTableId, numTrans<transId>
#define DBM_A_GET_USERID_BY_TRANSID_VEC             (DBM_DELTA + DBM_Q_GET_USERID_BY_TRANSID_VEC)		// 8704 // 42[s|4<4s>] - reqId,err_code[errDescr|numTrans<transId,userId>]

// PYR-120795 Cashier to RW Dbm
#define DBM_Q_UPDATE_OVERCREDIT_GATEWAY             7705 // 444<4> - reqId, transId, gatewayName
#define DBM_A_UPDATE_OVERCREDIT_GATEWAY             (DBM_DELTA + DBM_Q_UPDATE_OVERCREDIT_GATEWAY)		// 8705 // 442[s] - reqId,transId,err_code[errDescr]

// PYR-105425 Cashier to Dbm
#define DBM_Q_TRANSFER_ALL_ASSETS_CHECK_RO          7706  // 4s4sssM - reqId, adminId, adminIntId, userFrom, userTo, comment, M(tickets&bonuses)' comes from 'DBM_A_GET_USER_SURRENDER_ASSETS'
#define DBM_A_TRANSFER_ALL_ASSETS_CHECK_RO          (DBM_DELTA + DBM_Q_TRANSFER_ALL_ASSETS_CHECK_RO)   // 8706 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_TRANSFER_ALL_ASSETS_PART1_FROM        7707 // 4O - reqId, TransferActionContext
#define DBM_A_TRANSFER_ALL_ASSETS_PART1_FROM        (DBM_DELTA + DBM_Q_TRANSFER_ALL_ASSETS_PART1_FROM) // 8707 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_TRANSFER_ALL_ASSETS_PART2_TO          7708 // 4O - reqId, TransferActionContext
#define DBM_A_TRANSFER_ALL_ASSETS_PART2_TO          (DBM_DELTA + DBM_Q_TRANSFER_ALL_ASSETS_PART2_TO)   // 8708 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

// PYR-105425 Cashier to Dbm
#define DBM_Q_GET_NEXT_ID_VEC                       7709 // 4ss4 - reqId, objectName, userId, count
#define DBM_A_GET_NEXT_ID_VEC                       (DBM_DELTA + DBM_Q_GET_NEXT_ID_VEC) // 8709 // 46[s|4<8>tt] - reqId, errCode[errDescr | count<newId>,dbmTime,dbmHearbeatUtc]

// PYR-119671 CSI to DBM
#define	DBM_Q_GET_USER_CASH_TRANS					7710	// 4ssst - reqId, userId, transType, statuses, startedFrom
#define	DBM_A_GET_USER_CASH_TRANS					(DBM_DELTA + DBM_Q_GET_USER_CASH_TRANS)			// 8710 // 42[s|4<o>] - reqId, err_code[errDescr|numTrans<TransData>]

// PYR-118031 Regulator -> DBM RO
#define DBM_Q_REVERSE_MIGRATION_EU_CHECK			7711 // 4s - reqId, userId
#define DBM_A_REVERSE_MIGRATION_EU_CHECK			(DBM_DELTA + DBM_Q_REVERSE_MIGRATION_EU_CHECK) // 8711 // 42[s] - reqId, errCode[errDesc]

// PYR-118031 Regulator -> DBM RW
#define DBM_Q_REVERSE_MIGRATION_EU_PERFORM			7712 // 4s4 - reqId, userId, eUserMigrationFlags
#define DBM_A_REVERSE_MIGRATION_EU_PERFORM			(DBM_DELTA + DBM_Q_REVERSE_MIGRATION_EU_PERFORM) // 8712 // 42[s] - reqId, errCode[errDesc]

// PYR-120470
#define DBM_Q_MG_SET_GAME_PROP						7713	// 4M(444s) - reqId,M(minigameId,propType,propId,propValue)
#define DBM_A_MG_SET_GAME_PROP						(DBM_DELTA + DBM_Q_MG_SET_GAME_PROP)	// 8713	// 46[s| - reqId,errCode[errString]

#define DBM_Q_MG_GET_GAME_PROP						7714	// 4444 - reqId,minigameId,propType,propId
#define DBM_A_MG_GET_GAME_PROP						(DBM_DELTA + DBM_Q_MG_GET_GAME_PROP)	// 8714	// 42[s|M(444s)] - reqId,errCode[errString|M(minigameId,propType,propId,propValue)]

#define DBM_Q_MG_GET_GAME_PROPS						7715	// 4444 - reqId,minigameId,propType,propId
#define DBM_A_MG_GET_GAME_PROPS						(DBM_DELTA + DBM_Q_MG_GET_GAME_PROPS)	// 8715	// 42[s|4<M(444s)>] - reqId,errCode[errString|numProperties<M(minigameId,propType,propId,propValue)>]

#define DBM_Q_MG_DELETE_GAME_PROP					7716	// 4444 - reqId,minigameId,propType,propId
#define DBM_A_MG_DELETE_GAME_PROP					(DBM_DELTA + DBM_Q_MG_DELETE_GAME_PROP)	// 8716	// 46[s] - reqId,errCode[errString]

// PYR-109327
#define DBM_Q_PPP_REGISTER							7717 // 48s4484484s14<s>1s8t48 - pppId,entryId,userId,userIntId,buyInRM,buyInPM,buyInTChips,rakeRM,rakePM,rakeTChips,adminId,useTicket,numAdmissions<admissions>,isPlayMoney,currency,appLoginId,buyinTime,status,initialValue
#define DBM_A_PPP_REGISTER							(DBM_DELTA + DBM_Q_PPP_REGISTER) // 8717 // 2[s] - errCode[errString]

#define DBM_Q_PPP_ALLOCATE_ENTRYID					7718 // empty message body
#define DBM_A_PPP_ALLOCATE_ENTRYID					(DBM_DELTA + DBM_Q_PPP_ALLOCATE_ENTRYID) //8718 // 2[s|8] - errCode[errString|entryId]

#define DBM_Q_PPP_MATCH_PLAYERS						7719 // 44<8>8 - pppId,numEntryIds<entryIds>,tournId
#define DBM_A_PPP_MATCH_PLAYERS						(DBM_DELTA + DBM_Q_PPP_MATCH_PLAYERS) // 8719 // 2[s4<8>] - errCode[errString,numFailedEntries<failedEntries>]

#define DBM_Q_PPP_NEED_DATA							7720 // empty message body
#define DBM_A_PPP_NEED_DATA							(DBM_DELTA + DBM_Q_PPP_NEED_DATA) // 8720 // 2[s|4<M(48s4s18888848t484444t)>] - errCode[errString|numEntries<M(pppId,entryId,userId,userIntId,currency,isPlayMoney,value,buyinValue,buyinRake,buyinTChipsValue,buyinTChipsRake,buyinTicketId,buyInTransId,buyinTime,status,tournId,numPlayed,numEliminations,earned,lastBountyEventId,lastActive)>]

#define DBM_Q_PPP_UPDATE_ENTRY_STATUS				7721 // 484 - pppId,entryId,status
#define DBM_A_PPP_UPDATE_ENTRY_STATUS				(DBM_DELTA + DBM_Q_PPP_UPDATE_ENTRY_STATUS) // 8721 // 2[s4<8>] - errCode[errString,numFailedEntries<failedEntries>]

#define DBM_Q_PPP_ELIMINATE_PLAYERS					7722 // 484<M(44848444)>b - pppId,tournId,numKnockouts<M(bountyEventId,eliminatorUserIntId,eliminatorEntryId,eliminatedUserIntId,eliminatedEntryId,amountPaid,amountToHead,knockoutSpinSeedAmount)>,isFinal
#define DBM_A_PPP_ELIMINATE_PLAYERS					(DBM_DELTA + DBM_Q_PPP_ELIMINATE_PLAYERS) // 8722 // 2[s4<8>] - errCode[errString,numFailedEntries<failedEntries>]

#define DBM_Q_PPP_REG_INFO							7723 // s4m4<s>44441s14<s>4<4>4<s>b8s2<ss>ssb4 - userId,tournFlags,registrationSites,numAdmissions<admissions>,sagFactor,buyIn,rake,fppBuyIn,isPlayMoney,currency,needRelated,numRestrictedToTrackingCode<restrictedToTrackingCode>,numRestrictedToAffiliate<restrictedToAffiliate>,numRestrictedToPromoCode<restrictedToPromoCode>,earlyUserCheck,tournFlags2,sex,numRegistrationBlockingOverride<country,state>,countryByIp,stateByIp,includeDoNotPay,flags
#define DBM_A_PPP_REG_INFO							(DBM_DELTA + DBM_Q_PPP_REG_INFO) // 8723 // 2[sb|4<s44>448444b444M()M()M()bM()] - errCode[errString,enoughOnAllAccounts|numEffectiveAdmissions<admissionId,totalCount,reserved>,vipStatus,availableChips,availablePlayChips,availableFpp,availableTChips,locale,enoughOnAllAccounts,effectiveBuyIn,effectiveRake,effectiveFppBuyIn,M(),M(),M(),useAutoConvertBuyIn,M()]

#define DBM_Q_PPP_UNREGISTER						7724 // 48s8 - pppId,entryId,userId,appLoginId
#define DBM_A_PPP_UNREGISTER						(DBM_DELTA + DBM_Q_PPP_UNREGISTER) // 8724 // 2[s|448s] - errCode[errString|refundChips,refundTChips,refundPlayChips,currency]

#define DBM_Q_PPP_CASHOUT							7725 // 48s82s4 - pppId,entryId,userId,appLoginId,cashoutType,adminId,adminIntId
#define DBM_A_PPP_CASHOUT							(DBM_DELTA + DBM_Q_PPP_CASHOUT) // 8725 // 2[s|448s] - errCode[errString|cashoutChips,cashoutTChips,cashoutPlayChips,currency]

#define DBM_Q_PPP_SCRIPT_AUDIT						7726 // 44tm - pppId,scriptId,when,siteMask
#define DBM_A_PPP_SCRIPT_AUDIT						(DBM_DELTA + DBM_Q_PPP_SCRIPT_AUDIT) // 8726 // 2[s] - errCode[errString]

#define DBM_Q_CHECK_ADMISSIONS						7727 // 4<s8>sb - numAdmissions<name,value>,currency,isPM
#define DBM_A_CHECK_ADMISSIONS						(DBM_DELTA + DBM_Q_CHECK_ADMISSIONS) // 8727 // 2[s] - errCode[errString]

#define DBM_Q_PPP_CANCEL_RUNNING_TOURN				7728 // 48b4<M(4884)> - pppId,tournId,finalizeMode,numRemainingUsers<M(userIntId,entryId,newValue,refund)>
#define DBM_A_PPP_CANCEL_RUNNING_TOURN				(DBM_DELTA + DBM_Q_PPP_CANCEL_RUNNING_TOURN) // 8728 // 2[s] - errCode[errString]

#define DBM_Q_PPP_REGISTER_READDBM					7729 // s444s - userId,tournFlags,buyInRM,buyInTChips,currency 
#define DBM_A_PPP_REGISTER_READDBM					(DBM_DELTA + DBM_Q_PPP_REGISTER_READDBM) // 8729 // 2[s] - errCode[errString]

#define DBM_Q_PPP_UPDATE_ENTRY_STATUS_READDBM		7730 // s - userId 
#define DBM_A_PPP_UPDATE_ENTRY_STATUS_READDBM		(DBM_DELTA + DBM_Q_PPP_UPDATE_ENTRY_STATUS_READDBM) // 8730 // 2[s] - errCode[errString] 

#define DBM_Q_REVOKE_SELFEXCLUSION					7731 // 4s4s44 - reqID, adminId, adminIntId, userId, currExcludedId, newSeDays
#define DBM_A_REVOKE_SELFEXCLUSION					(DBM_DELTA + DBM_Q_REVOKE_SELFEXCLUSION) // 8731 // 46[s] - reqId, errCode[errDesc]

// PYR-121466
#define DBM_Q_PAM_GET_NONMATCHING_SET				7732 // 4 - uploadId
#define DBM_A_PAM_GET_NONMATCHING_SET				(DBM_DELTA + DBM_Q_PAM_GET_NONMATCHING_SET) // 8732  // 2[s|4<4>] - errCode[errDesc|numNonmatchingSeqIds<nonmatchingSeqIds>]

// PYR-121097
#define DBM_Q_MIGRATION_GREEK_CHECK					7733 // 4s - reqId, userId
#define DBM_A_MIGRATION_GREEK_CHECK					(DBM_DELTA + DBM_Q_MIGRATION_GREEK_CHECK) // 8733 // 42[s|M(oo)] - reqId, errCode[errDesc|MsgBody(UserTournTickets, UserBonusesToVoid)]

// PYR-121097
#define DBM_Q_MIGRATION_GREEK_PERFORM				7734 // 4sM(oo)o - reqId, userId, MsgBody(UserTournTickets, UserBonusesToVoid), AppLoginEssentials
#define DBM_A_MIGRATION_GREEK_PERFORM				(DBM_DELTA + DBM_Q_MIGRATION_GREEK_PERFORM) // 8734 // 42[s] - reqId, errCode[errDesc]

// PYR-119847 Cashier to Dbm
#define DBM_Q_TRANSFER_FULL_ACCOUNT_CHECK_RO        7735  // 4s4ssOsOO - reqId,adminId,adminIntId,userFrom,commentUserFrom,UserFromBalanceData,userTo,commentsUserTo,userToBalanceData,transferBalanceData
#define DBM_A_TRANSFER_FULL_ACCOUNT_CHECK_RO        (DBM_DELTA + DBM_Q_TRANSFER_FULL_ACCOUNT_CHECK_RO)   // 8735 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_TRANSFER_FULL_ACCOUNT_PART1_FROM      7736 // 4O - reqId, TransferActionContext
#define DBM_A_TRANSFER_FULL_ACCOUNT_PART1_FROM      (DBM_DELTA + DBM_Q_TRANSFER_FULL_ACCOUNT_PART1_FROM) // 8736 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_TRANSFER_FULL_ACCOUNT_PART2_TO        7737 // 4O - reqId, TransferActionContext
#define DBM_A_TRANSFER_FULL_ACCOUNT_PART2_TO        (DBM_DELTA + DBM_Q_TRANSFER_FULL_ACCOUNT_PART2_TO)   // 8737 // 42[s|O] - reqId,err_code[errDescr|TransferActionContext]

#define DBM_Q_PPP_REGISTER_WRITEDBM					7738 // 4s481s4<s> - reqId, userId, pppId, apploginId, useTicket, currency, num<admissions>
#define DBM_A_PPP_REGISTER_WRITEDBM					(DBM_DELTA + DBM_Q_PPP_REGISTER_WRITEDBM)   // 8738 42[s|84] - reqId, errCode [errStr|transId,ticketId]

#define DBM_Q_PPP_UNREGISTER_WRITEDBM		        7739 // 4s484 - reqId, userId, pppId, appLoginId, ticketId
#define DBM_A_PPP_UNREGISTER_WRITEDBM				(DBM_DELTA + DBM_Q_PPP_UNREGISTER_WRITEDBM) // 8739 42[s|8] - reqId, errCode [errStr|transId]

// PYR-117466 Cashier to Main Dbm
#define DBM_Q_NEW_CHQS_UPDATE_ONE_TRANS             7740  // 4411ss(D)s  reqId transId, oldstatus, newstatus, xTransStr, errorInfo, whenMailed wayBill
#define DBM_A_NEW_CHQS_UPDATE_ONE_TRANS             (DBM_DELTA + DBM_Q_NEW_CHQS_UPDATE_ONE_TRANS) // 8740  // 46[s|46s[(Trn)s>]  reqId err_code[errDescr| transId, err_code, err_descr[CashTrans phone]]

// PYR-124662
#define DBM_QG_VIPSTORE_CANCEL_ORDERS               7741
#define DBM_AG_VIPSTORE_CANCEL_ORDERS               (DBM_DELTA + DBM_QG_VIPSTORE_CANCEL_ORDERS) // 8741

// PYR-117466 Cashier to Main Dbm
#define DBM_Q_NEW_CHQS_UPDATE_REJECT_WITH_ERRCODE_ONE_TRANS 7742  //4411ss(D)ss  reqId transId, oldstatus, newstatus, xTransStr, errorInfo, whenMailed wayBill errorCode
#define DBM_A_NEW_CHQS_UPDATE_REJECT_WITH_ERRCODE_ONE_TRANS (DBM_DELTA + DBM_Q_NEW_CHQS_UPDATE_REJECT_WITH_ERRCODE_ONE_TRANS) // 8742


///
// Add new messages above this comment!!!
// Also remember to re-use msgIds marked with DBM_Q_AVAILABLE_MESSAGE
//

#define DBM_Q_OLAP_GET_USERS			21700	//
#define DBM_Q_OLAP_PROCESS_SAVED_QUEUE	21705	//
#define DBM_Q_EXPORT_ZIP_HIST_0			21701	//4ss4 reqId, from time, to time, by
//#define DBM_Q_WORKER					40000


///// notification messages (dbm to dbm2, do not require any answer)
// USER slaves - slave DBMs connected via "dbmslave" connection type
// all slaves - slave DBMs connected via either "dbmslave" or "dbmslave2" connection type
#define DBM_N_USER_CACHE				10001 //s(U) userId, (User) 	// Sent to USER slaves only, Login DBM
#define DBM_N_BONUSES					10002 // empty					// Sent to all slaves, OLAP
#define DBM_N_CASHIN_LIMITS				10003 // empty					// Sent to all slaves, OLAP
#define DBM_N_REREAD_BLOCKED_STATES		10004 // empty					// Sent to all slaves, OLAP
#define DBM_N_ACTIVE_TRACK_CODES		10005 // empty					// Sent to all slaves, OLAP
#define DBM_N_USER_TRACK_CODES			10006 // s userId				// Sent to USER slaves only
#define DBM_N_USER_PROMONAME			10007 // ss userId, promoName	// Sent to USER slaves only
#define DBM_N_READ_VELOCITYCONTROL		10008 // empty					// Sent to all slaves, OLAP
// obsoleted by #18664 #define DBM_N_CLOSE_ROLLBACKS			10009 // sbss paySystem closeNotOpen adminId comment
// obsolete by PYR-26128  #define DBM_N_READ_NOTCASHINS			10010 // can be reused after April 2012 build
#define DBM_N_REREAD_CURRENCIES			10011 // empty					// Sent to all slaves, OLAP
#define DBM_N_REREAD_CONVMARGINS		10012 // empty					// Sent to all slaves, OLAP
#define DBM_N_REREAD_SETTINGS           10013 // empty					// Sent to all slaves, OLAP
#define DBM_N_REREAD_INIDATA            10014 // empty					// Sent to all slaves, OLAP
#define DBM_N_PLAYING_TIME_LIMIT		10015 // s44 - userId, limit, limitType		// Sent to Login DBM
// obsoleted by PYR-45735 #define DBM_N_REREAD_WUOFFICES          10016 // empty #18179			// Sent to all slaves, OLAP
#define DBM_N_PLAYER_TOKEN				10017 // ss - userId, token		// Sent to Login DBM
#define DBM_N_TO_LOGINDBM_INI_UPDATED	10018 // M - see composeDbmIniFileForLoginDbm() // Sent to Login DBM
#define DBM_N_REREAD_VIP_LEVELS			10019 // empty					// Sent to all slaves, OLAP
#define DBM_N_FR_PLACE_OF_BIRTH			10020 // PYR-23674: sbsbs - userId, countryChanged, countryOfBirth, cityChanged, cityOfBirth // Sent to Login DBM
#define DBM_N_UPDATE_TICKET_CACHE		10021 // PYR-25202: empty 		// Sent to all slaves only (To send to OLAP, eOlapGroupTourns must be used!)
// obsolete by PYR-42099  #define DBM_N_UPDATE_LAST_ANNUAL_ACK	10022 // PYR-27262: bool - annualAckRequired // Sent to Login DBM
#define DBM_N_TO_COLLUSION_INI_UPDATED	10024 // PYR-28732:				//	Sent to OLAP only, see composeDbmIniFileForCollusion
#define DBM_N_UPDATE_ROLLS				10025 // PYR-31829: empty
#define DBM_N_LOGIN_COOL_OFF			10027 // s4 - userId, limit		// Sent to Login DBM //PYR-105467
#define DBM_N_LOGIN_TOTAL_LIMIT		    10028 // s4 - userId, limit		// Sent to Login DBM // PYR-47770 
#define DBM_U_CZ_SELFEXCLUSION			10029 // PYR-47770: 4T-reqId=0, 4-userId,exclusionTime // Sent to lobby
#define DBM_U_AUTH_CHANGED				10030 // PYR-55375: 4S4 - reqId=0, userId, flags // Sent to lobby
#define DBM_U_USER_FLAGS_PRIVS_CHANGED	10031 // PYR-60282 4-reqId=0, 4-userIntId, S-usrId, M(UserOld), M(UserNew) // Sent to lobbyEtc when flags/privileges changed
#define DBM_N_PLAYER_TOKEN_NG			10032 // PYR-63165: ss - userId, tokenNg // Sent to Login DBM, TODO: #dmitryst - remove after migration to new BE token complete
#define DBM_N_UPDATE_PAYSYSTEM_DATA		10033 // PYR-112209: <o> - vector<PaySystemDescriptors::PaySystemData>
#define DBM_U_USER_EMAIL_CHANGED		10034 // PYR-117612 PYR-117618: 4oo - reqId=0, oldUser, newUser

//	PYR-68899 Obsoleted.
// Notification message from Login DBM to lobbyetc for currently logged in users only (no longer sent to cashier after PYR-36189)
//#define LDBM_N_USER_FLAGS				10101	// s4884884s - userId,flags,flags2,flags3,privileges,privileges2,privileges3,imageId,country

// Notification message from Login DBM to Lobby
#define LDBM_N_PLAYING_TIME_LIMIT		10102	// s4444 - userId, remainingMinutes, offendingLimitType, offendingTimeLimit,licenseId
#define LDBM_N_USER_NOT_ALLOWED_TO_PLAY	10103	// sD444 - userId, until, timeLimitType, playingTimeLimit, license
// #define DBM_Q_AVAILABLE_MESSAGE		10104
#define LDBM_N_USER_BLACKLISTED			10105	// s4 - userId, licenseId
#define LDBM_N_USER_PLAYING_TIME_RESET	10106	// sb4 - userId, isNewDay, playingTimeLimit // PYR-27741, PYR-47770
// PYR-26675: logindbm -> lobby
#define LDBM_N_LOBBY_PGAD_IS_BACK		10107   // 4 reqId

// PYR-34698: logindbm -> server modules
#define LDBM_N_SERVER_PGAD_TIMEOUT_USER	10108   // 4 reqId

// PYR-39772: logindbm -> auxhub
#define LDBM_N_AUXHUB_NJ_GEOIP_STATUS_CHANGE 10109   // 44 reqId, userId, authFlags

//	PYR-52886: LoginDbm -> LobbyEtc. Logout of the Authenticated Poker Connection
#define LDBM_N_LOGOUT_APC				10110		//	4s - reqId, userId

// PYR-50999: logindbm -> regulator
#define LDBM_N_LONG_USER_SESSION		10111		// 44st84s - reqId, userIntId, userId, loginTime, appLoginId, licenseId, ipAddr

#define DBM_A_OLAP_GET_USERS			22700	//446[s] num_users, err_descr
#define DBM_A_EXPORT_ZIP_HIST_0			22701	//46[s] reqId, err_code[errDescr] 
#define DBM_A_OLAP_PROCESS_SAVED_QUEUE	22705	//46[s] reqId, err_code[errDescr] 


//OLAPSaver can not save message
#define DBM_A_OLAPSAVER_RESULT			23000	//46[s] reqId, err_code[errDescr] 

#define DBM_A_WORKER					41000
//#define DBM_A_WORKER_DUMMY				41001

#endif

