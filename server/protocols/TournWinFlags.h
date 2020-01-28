#ifndef TournWinFlags_h_included
#define TournWinFlags_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

// [AO 2009-09-11] #12002 - For encoding different data in the dynamic leaf
enum TournWinFlags // 16 bits
{
	WIN_TOURN_TICKET		= (1 << 0),
	WIN_TOURN_ENTRY			= (1 << 1),
	WIN_TOURN_PLAY_MONEY	= (1 << 2),
	WIN_TOURN_FPPS			= (1 << 3),
	WIN_TOURN_DEAL			= (1 << 4),	// [AO 2010-04-19] #13832 - User participated in deal
	WIN_TOURN_NEXT_FLIGHT   = (1 << 5), // #19167
};

////////////////////////////////////////////////////////////////////////////////

#endif // TournWinFlags_h_included
