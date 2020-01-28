#ifndef getfileinterface_h_included
#define getfileinterface_h_included

#define MSG_UPDATE_START					0x4701
//s4<s4<ss>> 4<s4<ss>> - rootDir iniFile0 iniFile1
#define MSG_UPDATE_START_REPLY					0x4702
//2s errCode errStr
#define MSG_UPDATE_STOP						0x4703
// - none
#define MSG_UPDATE_STOP_REPLY					0x4704
//2s errCode errStr
#define MSG_UPDATE_NOTIFY_COMPLETED				0x4705
//55 update* error*
#define MSG_UPDATE_NOTIFY_PROGRESS				0x4706
//1 %%
#define MSG_UPDATE_NOTIFY_ERROR					0x4707
//55 update* error*
#define MSG_UPDATE_SHUTDOWN					0x4708

#define MSG_GET_FILE_CHUNK					0x47A1
//s4 path offset
#define MSG_GET_FILE_CHUNK_REPLY				0x47A2
//2[41B] errCode [ fullFileSz isLast varBlock ]

#endif
