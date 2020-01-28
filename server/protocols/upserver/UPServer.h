// UPServer.h
#ifndef UPServer_h_included
#define UPServer_h_included

namespace UPServerCommon
{
	enum UserPropertyType
	{
		UserPropertyType_Glow	= 1001,
		UserPropertyType_CardMatch	= 1002,
		UserPropertyType_CardMatch_Winner = 1003,
	};

	enum UserGlowType
	{
		UserGlowType_None		= -1,
		UserGlowType_TheDeal	= 0,
	};

	enum OneTimeNotificationTypes
	{
		OTN_ClientAnimation = 1,
	};

	enum ClientAnimationTypes
	{
		CAT_CardMatchWinner = 1,
	};

}


#endif //UPServer_h_included




