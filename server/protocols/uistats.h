// uistats.h
#ifndef uistats_h_included
#define uistats_h_included

#define	MSG_UISTATS_SAVE_USER_STATS				1000 // s
#define	MSG_UISTATS_SAVE_USER_STATS_REPLY		1001 // 6[s]

//	PYR-24587
//	Message from the mobile device - application was launched and a settings change has been detected.
//	The user has NOT logged in.  Unauthenticated message.
#define MSG_UISTATS_MOBILE_DEVICE_REGISTRATION			1100	//	4ms44444[s] deviceId,deviceTokenMsgBody,installId,platformId,siteId,notificationType,deviceLocale,brand[,appKey]
																//		For deviceTokenMsgBody format, B=binary device token data
#define MSG_UISTATS_MOBILE_DEVICE_REGISTRATION_REPLY    1101	//	6[s|4] errCode [errDescr|deviceId]

//	PYR-24587
//	Message from the mobile device - user has logged in and a settings change has been detected.  Authenticated message.
#define MSG_UISTATS_MOBILE_DEVICE_OWNERSHIP				1102	//	4m444 deviceId,deviceTokenMsgBody,oldDeviceId,notificationType,deviceLocale
																//		For deviceTokenMsgBody format, B=binary device token data
#define MSG_UISTATS_MOBILE_DEVICE_OWNERSHIP_REPLY		1103	//	6[s] errCode [errDescr]


//	PYR-24967
//	Message from the mobile device - user has logged in and a settings change has been detected.  Authenticated message.
#define MSG_UISTATS_MOBILE_DEVICE_OWNERSHIP_2				1104	//	4m4444 deviceId,deviceTokenMsgBody,oldDeviceId,notificationType,deviceLocale,platformId
//		For deviceTokenMsgBody format, B=binary device token data
#define MSG_UISTATS_MOBILE_DEVICE_OWNERSHIP_2_REPLY			1105	//	6[s] errCode [errDescr]

//	PYR-34818 authenticated client -> uistats: obsolete see PYR-52384
//#define MSG_UISTATS_LOG_TEST_RESULTS						1106	//	4s4ss - timestamp, installId, siteId, test version, test results
//#define MSG_UISTATS_LOG_TEST_RESULTS_REPLY					1107	//	6[s] - errCode[errDesc]

// PYR-36560
// Message from the mobile device - user has logged in to play Poker Duel. Authenticated message.
#define MSG_UISTATS_MOBILE_DEVICE_LAST_ACCESSED				1108	//	44 - deviceId,platformId
#define MSG_UISTATS_MOBILE_DEVICE_LAST_ACCESSED_REPLY		1109	//	6[s] - errCode[errDesc]

//	PYR-35266
#define MSG_UISTATS_MOBILE_RATE_ME							1110	//	4 - rating
#define MSG_UISTATS_MOBILE_RATE_ME_REPLY					1111	//	6[s] - errCode[errDesc]

// PYR-49177
#define MSG_UISTATS_SECURITY								1112 // 4s
#define MSG_UISTATS_SECURITY2								1113 // 44s code, severity, description
#define MSG_UISTATS_SEND_CLIENT_VC							1114 // uistats->monet

#endif //uistats_h_included
