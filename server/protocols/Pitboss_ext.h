// ===========================================================
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
// ===========================================================

// Use for interface between Poker modules (eg Admin) and Casino Pitboss

#ifndef Pitboss_ext_h_included
#define Pitboss_ext_h_included

#include "commmsgbody.h"

//////////////////////////////////////////////////
// Alert handling through Admin
#define PITBOSS_Q_ALERTS_SET_PARAMETER		84001
#define PITBOSS_A_ALERTS_SET_PARAMETER		84002

// PYR-31624
struct PBAlertParameter
{
	INT32  userThreshold;
	INT32  gameVarThreshold;

	PBAlertParameter()
	{
		userThreshold = 0;
		gameVarThreshold = 0;
	}
	void parse(CommMsgParser& parser)
	{
		parser.
			parseINT32(userThreshold).
			parseINT32(gameVarThreshold);
	}
	void compose(CommMsgBody& body) const
	{
		body.
			composeINT32(userThreshold).
			composeINT32(gameVarThreshold);
	}
};

#endif
