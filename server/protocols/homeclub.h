// Copyright (C) 2010 PYR Software Ltd. All rights reserved
// homeclub.h

#ifndef homeclub_h_included
#define homeclub_h_included

#include "pplib.h"
#include "commmsgbody.h"
#include "pyrtime.h"
#include "composecolls.h"
#include "HgDbmSharedCli.h"
#include "homelobbymsgs.h"

////////////////////////////////////////////////////////////////////////////////////

#define MIN_LENGTH_CLUBNAME 8
#define MAX_LENGTH_CLUBNAME 30

//PYR-19684
#define MIN_LENGTH_INVITATIONCODE 8
#define MAX_LENGTH_INVITATIONCODE 30

struct HgCreateClubParameters
{
	PString clubname;
	PString password;

	HgCreateClubParameters();
	HgCreateClubParameters( const char *clubname_, const char *password_ );

	void clear();
	void compose( CommMsgBody& body ) const;
	void parse( CommMsgParser& parser );
};


/////////////////////////////////////////////////////////////////////////////////////

struct HgAddUserToClubParameters
{
	UINT32 clubId;
	PString clubPassword;
	PString comment;

	HgAddUserToClubParameters();

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

//////////////////////////////////////////////////////////////////////////////////////

struct HgPlayerListElement
{
	PString userId;
	PString city;          
	PString comment;   // nickname
	UINT32 status;     // HomeGamesMembershipStatus, see hgdbmshared.h
	UINT32 role;       // HgMemberRole, see hgdbmshared.h
	SrvTime enrolled;
	UINT32 flags;

	HgPlayerListElement();
	HgPlayerListElement( const char *userId_, const char *city_, const char *comment_, UINT32 status_, UINT32 role_,
						 const SrvTime& enrolled_, const UINT32 flags_ );

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};


struct HgPlayerList
{
	vector<HgPlayerListElement> players;

	HgPlayerList();

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

//////////////////////////////////////////////////////////////////////////////////////

enum HgChangePlayerStatusAction
{
	eHgAcceptPlayer = 0,
	eHgRejectPlayer,
	eHgBlockPlayer,
	eHgUnblockPlayer,
	eHgSuspendPlayer,
	eHgReinstatePlayer,
	eHgRemovePlayer,
	eHgMakePlayerAdmin,
	eHgRevokePlayerAdmin,
	eHgRemoveApplication,    // PYR-17881

	ENUM_LAST_VALUE(HgChangePlayerStatusAction) // HgChangePlayerStatusAction_Last & HgChangePlayerStatusAction_NextAfterLast
};


struct HgChangePlayersStatusParams
{
	UINT32 clubId;
	HgChangePlayerStatusAction action;          
	vector<PString> usersToChange;

	HgChangePlayersStatusParams();
	HgChangePlayersStatusParams( UINT32 clubId_, HgChangePlayerStatusAction action_, const vector<PString>& usersToChange_ );

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};


//////////////////////////////////////////////////////////////////////////////////////////

struct HgRegisterGameParams
{
	UINT32 clubId;
	UINT32 gameId;
	UINT32 buyInRM;

	HgRegisterGameParams();
	HgRegisterGameParams( UINT32 clubId_, UINT32 gameId_, UINT32 buyInRm_ );

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct HgClubNameImageApprovalParams
{
	UINT32 clubId;
	UINT32 approvalFlags;   // HomeGamesClubFlags, see HomeGamesClubFlags in the hgdbmshared.h

	HgClubNameImageApprovalParams();
	HgClubNameImageApprovalParams( UINT32 clubId_, UINT32 approvalFlags_ );

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

struct HgClubNameImageApprovalParamsList
{
	vector< HgClubNameImageApprovalParams > params;

	HgClubNameImageApprovalParamsList();

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

/////////////////////////////////////////////////////////////////////////////////////////

struct ClubInfoPubl
{
	UINT32 clubId;
	UINT32 imageId;

	ClubInfoPubl();
	ClubInfoPubl( UINT32 clubId_, UINT32 imageId_ );

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

/////////////////////////////////////////////////////////////////////////////////////////

enum HgChangeClubStatusAction
{
	eHgSuspendClub = 1,
	eHgCloseClub = 2,
	eHgReinstateClub = 3,
	eHgDeleteClub,
};

struct HgChangeClubStatusParams
{
	UINT32 clubId;
	HgChangeClubStatusAction action;

	HgChangeClubStatusParams();
	HgChangeClubStatusParams( UINT32 clubId_, HgChangeClubStatusAction action_ );

	void clear();
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const;
};

#endif // homeclub_h_included
