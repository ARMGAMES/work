#ifndef tabledef_h_included
#define tabledef_h_included

enum
{
	AllInDisconnAllow = 0x01,
	AllInShowAllow_obsoleted = 0x02,
	PlayerAllIn = 0x04,
	PlayerCapped = 0x08,
	//PlayerRITEnabled = 0x10  // this flag is no longer used any more in table server 7
};


enum // Sent as BYTE, max flag value is 0x80
{
	ClientPublPropTeamPro			= 0x01,
	ClientPublPropFixedBounty		= 0x02,
	ClientPublPropTeamProBounty		= 0x04,
	ClientPublPropShowNoChatToolTip	= 0x08, // Bug #15760: priv2ShowNoChatTooltip
	ClientPublPropProportionalPrize	= 0x10,
	ClientPublPropPspcDisplay       = 0x20, // PYR-118097
};

#endif
