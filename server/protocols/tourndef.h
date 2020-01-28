#ifndef	tourndef_h_included
#define	tourndef_h_included

#define TOURSCRIPT_FLAG_USETICKETORBUYIN		(1 <<  0)	//0x0001
#define TOURSCRIPT_FLAG_OLYMPICHACK				(1 <<  1)	//0x0002
#define TOURSCRIPT_FLAG_SITANDGO_EXTRA			(1 <<  2)	//0x0004
#define TOURSCRIPT_FLAG_DOUBLESTAKES			(1 <<  3)	//0x0008
#define TOURSCRIPT_FLAG_SITANDGO				(1 <<  4)	//0x0010
#define TOURSCRIPT_FLAG_PPA						(1 <<  5)	//0x0020
#define TOURSCRIPT_FLAG_REGENERATED				(1 <<  6)	//0x0040
#define TOURSCRIPT_FLAG_CHIPSDENOMINATION		(1 <<  7)	//0x0080
#define TOURSCRIPT_FLAG_NOTELIGIBLEBOARD		(1 <<  8)	//0x0100
#define TOURSCRIPT_FLAG_AFTERFINISHED			(1 <<  9)	//0x0200
#define TOURSCRIPT_FLAG_NO_HOMEGAME_STATS		(1 << 10)	//0x0400
#define TOURSCRIPT_FLAG_STOPSATELLITE			(1 << 11)	//0x0800
#define TOURSCRIPT_FLAG_EXTENDEDREGISTRATION	(1 << 12)	//0x1000
#define TOURSCRIPT_FLAG_RECORDING				(1 << 13)	//0x2000
#define TOURSCRIPT_FLAG_RECOPENCARDS			(1 << 14)	//0x4000
#define TOURSCRIPT_FLAG_NOTSAVEHANDSTODBM		(1 << 15)	//0x8000
#define TOURSCRIPT_FLAG_HIGHVOLUMEREG			(1 << 16)	//0x10000
#define TOURSCRIPT_FLAG_REBUYENABLE				(1 << 17)	//0x20000
#define TOURSCRIPT_FLAG_ADDONENABLE				(1 << 18)	//0x40000
#define TOURSCRIPT_FLAG_SATELLITERETURNBUYIN	(1 << 19)	//0x80000
#define TOURSCRIPT_FLAG_DUMMY					(1 << 20)	//0x100000
#define TOURSCRIPT_FLAG_NOOBSERVCHATONTHEEND	(1 << 21)	//0x200000
#define TOURSCRIPT_FLAG_BLOCKCHATALLIN			(1 << 22)	//0x400000
#define TOURSCRIPT_FLAG_BLOCKCHATINHAND			(1 << 23)	//0x800000
#define TOURSCRIPT_FLAG_GIVETARGETTICKET		(1 << 24)	//0x1000000
#define TOURSCRIPT_FLAG_GIVETARGETTICKET_REG	(1 << 25)	//0x2000000
#define TOURSCRIPT_FLAG_CHATMONITOR				(1 << 26)	//0x4000000
//#define TOURSCRIPT_FLAG_TICKETEXCLUSIVE			(1 << 27)	//0x8000000 //PYR-46488 Deprectaed
#define TOURSCRIPT_FLAG_BLINDSREVERSE			(1 << 28)	//0x10000000
#define TOURSCRIPT_FLAG_NO_SATELL_WINNER_UNREG	(1 << 29)	//0x20000000
#define TOURSCRIPT_FLAG_REGISTERTOSIMILAR		(1 << 30)	//0x40000000
//#define TOURSCRIPT_FLAG_END					(1 << 31)	//0x80000000 //The End!

//UINT64 tournFlags2
#define TOURNSCRIPT_FLAG2_OPTEARLYSTART			(ONE64 <<  0)	//0x0001
#define TOURNSCRIPT_FLAG2_OPTSKIPBREAK			(ONE64 <<  1)	//0x0002
#define TOURNSCRIPT_FLAG2_SYNCBREAKS_EVEN	    (ONE64 <<  2)	//0x0004
#define TOURNSCRIPT_FLAG2_SYNCBREAKS_ODD	    (ONE64 <<  3)	//0x0008
#define TOURNSCRIPT_FLAG2_PACKEDLOSERSOUT		(ONE64 <<  4)	//0x0010
#define TOURNSCRIPT_FLAG2_NSTACK				(ONE64 <<  5)	//0x0020
#define TOURNSCRIPT_FLAG2_MILESTONE				(ONE64 <<  6)	//0x0040
#define TOURNSCRIPT_FLAG2_SPINANDGO				(ONE64 <<  7)	//0x0080
#define TOURNSCRIPT_FLAG2_FLIGHT_ENTRY			(ONE64 <<  8)	//0x0100
#define TOURNSCRIPT_FLAG2_FLIGHT_INTERMEDIATE	(ONE64 <<  9)	//0x0200
#define TOURNSCRIPT_FLAG2_FLIGHT_FINAL			(ONE64 <<  10)	//0x0400
#define TOURNSCRIPT_FLAG2_TABLE_SUBSCRIPTION_OVER_CONNECTION	(ONE64 <<  11)	//0x0800
#define TOURNSCRIPT_FLAG2_BLINDS_INCREASE_INTERVAL_HANDS        (ONE64 <<  12)  //0x1000
#define TOURNSCRIPT_FLAG2_POWERSUP_ENABLED      (ONE64 <<  13)  //0x2000
#define TOURNSCRIPT_FLAG2_HTML5_ALLOWED         (ONE64 <<  14)  //0x4000 // PYR-34553
#define TOURNSCRIPT_FLAG2_CUSTOM_PRIZES         (ONE64 <<  15)  //0x8000 // PYR-33574
#define TOURNSCRIPT_FLAG2_SAG_FREEROLL_PM_DUP   (ONE64 <<  16)  //0x10000 // PYR-38580
#define TOURNSCRIPT_FLAG2_PSNG					(ONE64 <<  17)  //0x20000 // PYR-39272
#define TOURNSCRIPT_FLAG2_KNOCKOUT				(ONE64 <<  18)  //0x40000 (262144) // PYR-39980
#define TOURNSCRIPT_FLAG2_PSNG_REMATCH			(ONE64 <<  19)  //0x80000 (524288) // PYR-39272
#define TOURNSCRIPT_FLAG2_BEAT_THE_CLOCK		(ONE64 <<  20)  //0x100000 (1048576)
#define TOURNSCRIPT_FLAG2_WIN_THE_BUTTON		(ONE64 <<  21)  //0x200000 (2097152) // PYR-32587
#define TOURNSCRIPT_FLAG2_STORMLOBBY			(ONE64 <<  22)	//0x400000 //PYR-44908
#define TOURNSCRIPT_FLAG2_RESTRICTED_TO_NON_QUALIFIERS (ONE64 <<  23)  //0x800000 //PYR-38597
#define TOURNSCRIPT_FLAG2_FORCE_TO_TOP			(ONE64 <<  24)  //0x1000000 //PYR-34048
#define TOURNSCRIPT_FLAG2_FORCE_TO_TOP_STICKY	(ONE64 <<  25)  //0x2000000 //PYR-34048
#define TOURNSCRIPT_FLAG2_SPINANDGOMAX			(ONE64 <<  26)	//0x4000000 //PYR-51857
//#define TOURNSCRIPT_FLAG2_SERVER7			    (ONE64 <<  27)	//0x8000000 //PYR-53828  // retired by PYR-75653 in 2018.Main.07
#define TOURNSCRIPT_FLAG2_EARLY_PAYOUTS		    (ONE64 <<  28)	//0x10000000 //PYR-50812
#define TOURNSCRIPT_FLAG2_STORM_BETATESTER      (ONE64 <<  29)  //0x20000000 //PYR-53549
#define TOURNSCRIPT_FLAG2_FLOPSTARS             (ONE64 <<  30)  //0x40000000 //PYR-56338
#define TOURNSCRIPT_FLAG2_SUPPORTS_INTERNAL_ACCOUNTS (ONE64 <<  31)  //0x80000000 //PYR-64730
//#define TOURNSCRIPT_FLAG2_USE_OLD_TABLE_SERVER  (ONE64 <<  32)	// 0x100000000 //PYR-75653 this flag introduced in 2018.Main.07, retired in 2018.Main.08
#define TOURNSCRIPT_FLAG2_WINNER_ANIMATION	    (ONE64 <<  33)	//0x200000000 //PYR-71948
#define TOURNSCRIPT_FLAG2_INTHEMONEY_ANIMATION	    (ONE64 <<  34)	//0x400000000 //PYR-71953
#define TOURNSCRIPT_FLAG2_BUTTON_BLIND			(ONE64 << 35) // 0x800000000 //PYR-88920 PYR-89287
#define TOURNSCRIPT_FLAG2_FORCE_WINNER_TO_SHOW	(ONE64 << 36) // 0x1000000000  //PYR-73581 PYR-90426
#define TOURNSCRIPT_FLAG2_SAG_FLASH				(ONE64 << 37) // 0x2000000000  //PYR-111934
#define TOURNSCRIPT_FLAG2_TOURNSERVER7			(ONE64 << 38) // 0x4000000000 //PYR-99054
#define TOURNSCRIPT_FLAG2_PROGRESSIVEPLUS		(ONE64 << 39) // 0x8000000000 //PYR-114749

//#define TOURNSCRIPT_FLAG2_END					(ONE64 << 63)		//The End!

#define TOURNSCRIPT_FLAG2_FLIGHT	( TOURNSCRIPT_FLAG2_FLIGHT_ENTRY | TOURNSCRIPT_FLAG2_FLIGHT_INTERMEDIATE | TOURNSCRIPT_FLAG2_FLIGHT_FINAL ) //0x700

#define TOURNPUB_DYNAMIC_FLAG_FORCE_TO_TOP			(ONE64 <<  0)	//0x0001 //PYR-107722
#define TOURNPUB_DYNAMIC_FLAG_FORCE_TO_TOP_STICKY	(ONE64 <<  1)	//0x0002 //PYR-107722


#define PERCENT_NORMAL	1
#define PERCENTx100	    2
#define PERCENTx10000	3 // Bug #4879: Decided to move two grades below
#define PERCENTx10M	    4 // PYR-30338
#define TICKET_INTID	7 //Bug #5881:


enum TournLobbyLayout
{ //lint --e{849} : same enumerator value
	SitGo_All =						0x0001,		//0
	SitGo_1Table =					0x0002,
	SitGo_2Table =					0x0004,
	SitGo_HeadsUp =					0x0008,
	SitGo_Satellites =				0x0010,
	SitGo_PlayMoney =				0x0020,		//5

	Tourney_All =					0x0040,
	Tourney_Regular =				0x0080,
	Tourney_Satellites =			0x0100,
	Tourney_SpecialEvents =			0x0200,
	Tourney_Freerolls =				0x0400,		//10
	Tourney_Private =				0x0800,

	WCOOP_All =						0x1000,		Events_EPT_All =			0x1000,
	WCOOP_MainEvents =				0x2000,		Events_EPT_Qualifiers =		0x2000,
	WCOOP_FppSatellites =			0x4000,		Events_EPT_FppSatellites =	0x4000,
	WCOOP_CashSatellites =			0x8000,		Events_EPT_CashSatellites =	0x8000,		//15
	WCOOP_SnGSatellites =			0x10000,	Events_EPT_SnGSatellites =	0x10000,

	Events_PSLive_London =			0x20000,	//PYR-24672

	WPT_All =						0x40000,	Events_WCP__USStates =		0x40000,
	WPT_MainEvents =				0x80000,	Events_WCP__CAProvinces =	0x80000,
	WPT_FppSatellites =				0x100000,	Events_WCP__FPPQualifiers =	0x100000,	//20
	WPT_CashSatellites =			0x200000,	Events_WCP__RoundRobin =	0x200000,
	WPT_SnGSatellites =				0x400000,	Events_WCP__OnlineFinals =	0x400000,

	Events_PSLive_Macau =			0x800000,	//PYR-24672

	Events_WSOP_All =				0x1000000,
	Events_WSOP_Qualifiers =		0x2000000,		//25
	Events_WSOP_FppSatellites =		0x4000000,
	Events_WSOP_CashSatellites =	0x8000000,
	Events_WSOP_SnGSatellites =		0x10000000,

	Events_PSLive_Madrid =			0x20000000,	//PYR-24672

	Events_WCOOP_All =				0x40000000,		//30
	Events_WCOOP_MainEvents =		0x80000000,		

	Events_NONE =					0x0
};

enum TournLobbyLayout2
{
	Events_WCOOP_FppSatellites =	0x0001,
	Events_WCOOP_CashSatellites =	0x0002,
	Events_WCOOP_SnGSatellites =	0x0004,

	Events_PSLive_Other =			0x0008,		//PYR-24672		//35

	Events_NAPT_All =				0x0010,
	Events_NAPT_Qualifiers =		0x0020,
	Events_NAPT_FppSatellites =		0x0040,
	Events_NAPT_CashSatellites =	0x0080,
	Events_NAPT_SnGSatellites =		0x0100,		//40
	Auto_Global =					0x0200,			//server internal

	Tourney_Regular_High =			0x0400,
	Tourney_Regular_Medium =		0x0800,
	Tourney_Regular_Low =			0x1000,
	Tourney_Regular_Micro =			0x2000,		//45
	Tourney_Regular_Rebuy =			0x4000,
	Tourney_Regular_Turbo =			0x8000,

	Tourney_Satellites_Cash =		0x10000,
	Tourney_Satellites_FPP =		0x20000,
	Tourney_Satellites_Rebuy =		0x40000,	//50
	Tourney_Satellites_Events =		0x80000,

	SitGo_1Table_High =				0x100000,
	SitGo_1Table_Medium =			0x200000,
	SitGo_1Table_Low =				0x400000,
	SitGo_1Table_Micro =			0x800000,	//55
	SitGo_1Table_Turbo =			0x1000000,

	SitGo_MultiTable_High =			0x2000000,
	SitGo_MultiTable_Medium =		0x4000000,
	SitGo_MultiTable_Low =			0x8000000,
	SitGo_MultiTable_Micro =		0x10000000,	//60
	SitGo_MultiTable_Turbo =		0x20000000,

	SitGo_HeadsUp_High =			0x40000000,
	SitGo_HeadsUp_Medium =			0x80000000,

	Events2_NONE =					0x0
};

enum TournLobbyLayout3
{
	SitGo_HeadsUp_Low =				0x0001,
	SitGo_HeadsUp_Micro =			0x0002,		//65

	SitGo_Satellites_Cash =			0x0004,
	SitGo_Satellites_FPP =			0x0008,
	SitGo_Satellites_Events =		0x0010,

	SitGo_PlayMoney_High =			0x0020,
	SitGo_PlayMoney_Medium =		0x0040,		//70
	SitGo_PlayMoney_Low =			0x0080,

	Events_Special_All =			0x0100,
	Events_Special_Qualifiers =		0x0200,
	Events_Special_FppSatellites =	0x0400,
	Events_Special_CashSatellites =	0x0800,		//75
	Events_Special_SnGSatellites =	0x1000,

	Auto_Tourney_Regular =			0x2000,			//server internal
	Auto_Tourney_Satellites	=		0x4000,			//server internal
	Auto_SitGo_1Table = 			0x8000,			//server internal
	Auto_SitGo_MultiTable =			0x10000,		//server internal		//80
	Auto_SitGo_HeadsUp =			0x20000,		//server internal
	Auto_SitGo_Satellites =			0x40000,		//server internal
	Auto_SitGo_PlayMoney =			0x80000,		//server internal

	Tourney_VIP_All =				0x100000,
	Tourney_VIP_Silver =			0x200000,	//85
	Tourney_VIP_Gold =				0x400000,
	Tourney_VIP_Platinum =			0x800000,
	Tourney_VIP_SuperNova =			0x1000000,

	Events_LAPT_All =				0x2000000,
	Events_LAPT_Qualifiers =		0x4000000,	//90
	Events_LAPT_FppSatellites =		0x8000000,
	Events_LAPT_CashSatellites =	0x10000000,
	Events_LAPT_SnGSatellites =		0x20000000,

	Events_PSM_All =				0x40000000,

	Events3_SPECIAL =				0x80000000,	//95
	Events3_NONE =					0x0
};

enum TournLobbyLayout4
{
	Events_PSM_Qualifiers =			0x0001,
	Events_PSM_FppSatellites =		0x0002,
	Events_PSM_CashSatellites =		0x0004,
	Events_PSM_SnGSatellites =		0x0008,

	Tourney_VIP_Bronze =			0x0010,		//100

	Tourney_Regional =				0x0020,

	Events_WSOP_Steps =				0x0040,
	Events_WCOOP_Steps =			0x0080,
	Events_LAPT_Steps =				0x0100,
	Events_NAPT_Steps =				0x0200,		//105
	Events_EPT_Steps =				0x0400,
	Events_PSM_Steps =				0x0800,

	SitGo_Satellites_Steps =		0x1000,

	Events_APPT_All =				0x2000,
	Events_APPT_Qualifiers =		0x4000,		//110
	Events_APPT_FppSatellites =		0x8000,
	Events_APPT_CashSatellites =	0x10000,
	Events_APPT_SnGSatellites =		0x20000,
	Events_APPT_Steps =				0x40000,

	Hidden_Tourn_Banner =			0x80000,	//115
	HomeGame_Tourn_FromAdmin =		0x100000,	// PYR-17835		//server internal

	SitGo_1Table_Hyper =			0x200000,	// PYR-19546

	Events_More_All =				0x400000,	// PYR-21291
	Events_More_UKIPT =				0x800000,
	Events_More_RPS =				0x1000000,	//120
	Events_More_Eureka =			0x2000000,
	Events_More_Estrellas =			0x4000000,
	Events_More_FPS =				0x8000000,
	Events_More_IPT =				0x10000000,
	Events_More_ANZPT =				0x20000000,	//125
	Events_More_BPT =				0x40000000,

	Tourney_VIP_Chrome =			0x80000000,

	Events4_NONE =					0x0
};

//PYR-31037
enum eTournLobbyLayout5
{
	eLayout5_NONE = 0,

	eStreamO1 =  (1 << 0),		//128
	eStreamO2 =  (1 << 1),
	eStreamO3 =  (1 << 2),		//130
	eStreamO4 =  (1 << 3),
	eStreamO5 =  (1 << 4),
	eStreamO6 =  (1 << 5),
	eStreamO7 =  (1 << 6),
	eStreamO8 =  (1 << 7),		//135
	eStreamO9 =  (1 << 8),
	eStream10 =  (1 << 9),
	eStream11 =  (1 << 10),
	eStream12 =  (1 << 11),
	eStream13 =  (1 << 12),		//140
	eStream14 =  (1 << 13),
	eStream15 =  (1 << 14),
	eStream16 =  (1 << 15),
	eStream17 =  (1 << 16),
	eStream18 =  (1 << 17),		//145
	eStream19 =  (1 << 18),
	eStream20 =  (1 << 19),
	eStream21 =  (1 << 20),
	eStream22 =  (1 << 21),
	eStream23 =  (1 << 22),		//150
	eStream24 =  (1 << 23),
	eStream25 =  (1 << 24),
	eStream26 =  (1 << 25),
	eStream27 =  (1 << 26),
	eStream28 =  (1 << 27),		//155
	eStream29 =  (1 << 28),
	eStream30 =  (1 << 29),
	eStream31 =  (1 << 30),
	eStream32 =  (1 << 31), //lint !e648
};

//PYR-31037
enum eTournLobbyLayout6
{
	eLayout6_NONE = 0,

	eStream33 =  (1 << 0),		//160
	eStream34 =  (1 << 1),
	eStream35 =  (1 << 2),
	eStream36 =  (1 << 3),
	eStream37 =  (1 << 4),
	eStream38 =  (1 << 5),		//165
	eStream39 =  (1 << 6),
	eStream40 =  (1 << 7),
	eStream41 =  (1 << 8),
	eStream42 =  (1 << 9),
	eStream43 =  (1 << 10),		//170
	eStream44 =  (1 << 11),
	eStream45 =  (1 << 12),
	eStream46 =  (1 << 13),
	eStream47 =  (1 << 14),
	eStream48 =  (1 << 15),		//175
	eStream49 =  (1 << 16),
	eStream50 =  (1 << 17),
	eStream51 =  (1 << 18),
	eStream52 =  (1 << 19),
	eStream53 =  (1 << 20),		//180
	eStream54 =  (1 << 21),
	eStream55 =  (1 << 22),
	eStream56 =  (1 << 23),
	eStream57 =  (1 << 24),
	eStream58 =  (1 << 25),		//185
	eStream59 =  (1 << 26),
	eStream60 =  (1 << 27),
	eStream61 =  (1 << 28),
	eStream62 =  (1 << 29),
	eStream63 =  (1 << 30),		//190
	eStream64 =  (1 << 31), //lint !e648
};
enum TournProp2
{
	FastDealingTourn = 1
};

enum TableLogos
{
	Logo_WCOOP = 1,
	Logo_SM,
	Logo_WCP,
	Logo_WM,
	Logo_E1,
	Logo_E2,
	Logo_E3,
	Logo_E4,
	Logo_E5,
	Logo_E6,
	Logo_E7,
	Logo_E8,
	Logo_E9,
	Logo_E10,
	Logo_E11,
	Logo_E12
};

enum TournFlags_ServInt // tournament parameters for mt lobby and client
{ // see also tournscript.h:: 'enum TournamentServiceFlags'
	TOURNFLAG_SERVINT_PLAYERLIST_MASK	= 0x0003,
	TOURNFLAG_SERVINT_PLAYERLIST_NO		= 0x0001,
	TOURNFLAG_SERVINT_PLAYERLIST_DIALUP	= 0x0002,
	TOURNFLAG_SERVINT_BYES_NO			= 0x0004,
	TOURNFLAG_SERVINT_AUTOREBUY			= 0x0008,
	TOURNFLAG_SERVINT_FINTABLE_BCKGRND	= 0x0010,
	TOURNFLAG_SERVINT_TCHIPS_AWARD		= 0x0020,
	TOURNFLAG_SERVINT_AFFILIATE_FUNDED	= 0x0040,
	TOURNFLAG_SERVINT_REMATCH			= 0x0080,
	TOURNFLAG_SERVINT_EXPERIMENTAL3		= 0x0100,
	TOURNFLAG_SERVINT_SYNCBREAKS		= 0x0200,
	TOURNFLAG_SERVINT_REDUCED_PUBLISH	= 0x0400,
	TOURNFLAG_SERVINT_EXPERIMENTAL4		= 0x0800,
	TOURNFLAG_SERVINT_GENDERMALE		= 0x1000,
	TOURNFLAG_SERVINT_GENDERFEMALE		= 0x2000,
	TOURNFLAG_SERVINT_RETAINBOUNTIES	= 0x4000,   // [AO 2010-05-06] #14621
	TOURNFLAG_SERVINT_DISPLAYHUBRACKET	= 0x8000,   // [YL 2010-10-29] #15453
	TOURNFLAG_SERVINT_RECURRING_TOURN   = 0x10000,  // [MJ 2011-02-27] #17813
	TOURNFLAG_SERVINT_TARGET_FLIGHT		= 0x20000,	// [MJ 2011-06-13] #19167
	TOURNFLAG_SERVINT_VOICE_TOURN		= 0x40000,	// [AN 2011-12-09] #18513
	TOURNFLAG_SERVINT_CHATMON_FINALTBL_YES	= 0x80000,	// PYR-20838
	TOURNFLAG_SERVINT_CHATMON_FINALTBL_NO	= 0x100000,	// PYR-20838
	TOURNFLAG_SERVINT_BLITZ				= 0x200000,	// PYR-22885
	TOURNFLAG_SERVINT_EXPERIMENTAL5		= 0x400000,	// PYR-26694 // TODO: remove if/when it is safe to
	TOURNFLAG_SERVINT_CHIPGRAPH			= 0x400000,	// PYR-26694 / PYR-31659
	TOURNFLAG_SERVINT_MULTIDBM			= 0x800000, // PYR-28728
	TOURNFLAG_SERVINT_ALLWINNERS		= 0x1000000, // PYR-55505 indicates that there are pay-tiers with n=all

	//TOURNFLAG_SERVINT_WINNER_REG_FINAL
	//TOURNFLAG_SERVINT_RESERVED		= 0x0..00,
									/*
	last one:							0x80000000
									*/
};

// #18129
#define MAX_PLAYERS_UNCHANGED (-1)

// #18129
enum DummyTournsStatus
{
	eDummyTournStatusRegistering = 1,
	eDummyTournStatusRestricted = 2,
};

//PYR-34230
#define MAX_SUBSCRIPTION_CAPACITY			65533
//#define MAX_SUBSCRIPTION_CAPACITY			100		//256 for QA testing
//PYR-33545
#ifdef _DEBUG
#define ALLINSHOOTOUT_THROTTLING_TABLES		0x10	//16 for QA testing
#else
#define ALLINSHOOTOUT_THROTTLING_TABLES		0x1000	//4K
#endif

// PYR-42474 Tournament's dynamic flag set
enum eTournPropDyncamicFlags
{
	eTournPropDyncamicFlags_NONE						= 0,
	eTournPropDyncamicFlags_STARTED						= (1 << 0),
	eTournPropDyncamicFlags_PRIVATE_TABLES_MADE_PUBLIC	= (1 << 1), // 0x2 PYR-39535
	eTournPropDyncamicFlags_PSNG_SAMPLE					= (1 << 2), // 0x4 PYR-39272
	eTournPropDyncamicFlags_FORCE_TO_TOP				= (1 << 3), // 0x8 PYR-107722
	eTournPropDyncamicFlags_FORCE_TO_TOP_STICKY			= (1 << 4)  // 0x16 PYR-107722
};

// PYR-32616
enum BlindLevelIntervalRuleTypes
{
	BlindLevelIntervalRuleType_None = 0,
	BlindLevelIntervalRuleType_PercentPlayers = 1,
	BlindLevelIntervalRuleType_Bubble = 2,
	BlindLevelIntervalRuleType_NumPlayers = 3
};

enum eMixTournSatellite
{
	eMixTournSatellite_nonMix = 0,
	eMixTournSatellite_mixNonSatellite = 1,
	eMixTournSatellite_mixSatellite = 2,
};

#endif
