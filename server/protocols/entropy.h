#ifndef entropy_h_included
#define entropy_h_included

#define MSG_ENTROPY								0x7890
	//42<B>2<4> reqId, n<entropyBlock>, n<rnd_max>
#define MSG_ENTROPY_REPLY						0x7891
	//42<4> reqId, n<rnd>

#define MSG_ENTROPY_REFRESH_SETTINGS			0x7892
	//4 <reqId>
#define MSG_ENTROPY_REFRESH_SETTINGS_REPLY		0x7893
	//42[s] <reqId> errCode [errStr]

#define MSG_CHECK_ENTROPY_OBSOLETE				0x7894 // still used by Casino, so not obsolete actually
	//4[4] <reqId> [<siteIds>]
#define MSG_CHECK_ENTROPY_OBSOLETE_REPLY		0x7895
	//4bool <reqId> isReady

#define MSG_ENTROPY64							0x7896
	//42<B>2<8> reqId, n<entropyBlock>, n<rnd_max>
#define MSG_ENTROPY64_REPLY						0x7897
	//42<8> reqId, n<rnd>

// PYR-44606
#define MSG_SITE_ENTROPY						0x7898 // OBSOLETE, use MSG_SITE_ENTROPY2
                                                       //442<B>2<4> reqId, sites, n<entropyBlock>, n<rnd_max>
#define MSG_SITE_ENTROPY_REPLY					0x7899 //42<4> reqId, n<rnd>

#define MSG_SITE_ENTROPY64						0x7900 // OBSOLETE, use MSG_SITE_ENTROPY64_2
													   //442<B>2<8> reqId, sites, n<entropyBlock>, n<rnd_max>
#define MSG_SITE_ENTROPY64_REPLY				0x7901 //42<8> reqId, n<rnd>

// PYR-108004 - sites PBitmask
#define MSG_CHECK_ENTROPY2						0x7902 //4[m] <reqId> [<siteIds>]
#define MSG_CHECK_ENTROPY2_REPLY				0x7903 //4bool <reqId> isReady

#define MSG_SITE_ENTROPY2						0x7904 //4m2<B>2<4> reqId, sites, n<entropyBlock>, n<rnd_max>
#define MSG_SITE_ENTROPY2_REPLY				    0x7905 //42<4> reqId, n<rnd>

#define MSG_SITE_ENTROPY64_2					0x7906 //4m2<B>2<8> reqId, sites, n<entropyBlock>, n<rnd_max>
#define MSG_SITE_ENTROPY64_2_REPLY				0x7907 //42<8> reqId, n<rnd>

#endif
