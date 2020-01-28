// Copyright (C) 2001-2011 PYR Software Ltd.  All rights reserved.

#ifndef serverreplay_h_included
#define serverreplay_h_included

#define TIMER_REPLAY_MSG                   1
#define TIMER_REPLAY_SHUTDOWN_MSG          2
#define TIMER_REPORT_STATS_TO_LOBBY        3

#define MSG_REPLAY_FIRST /* ........... */ 0x4800

#define MSG_REPLAY_PAUSE /* ........... */ 0x4801
#define MSG_REPLAY_PAUSE_REPLY /* ..... */ 0x4802

#define MSG_REPLAY_RESUME /* .......... */ 0x4803
#define MSG_REPLAY_RESUME_REPLY /* .... */ 0x4804

#define MSG_REPLAY_NEXT_HAND /* ....... */ 0x4805
#define MSG_REPLAY_NEXT_HAND_REPLY /* . */ 0x4806

#define MSG_REPLAY_PREV_HAND /* ....... */ 0x4807
#define MSG_REPLAY_PREV_HAND_REPLY /* . */ 0x4808

#define MSG_REPLAY_HANDS_QUANTITY /* .. */ 0x4809
#define MSG_REPLAY_HANDS_QUANTITY_REPLY    0x480A

#define MSG_REPLAY_CURR_HAND_INFO /* .. */ 0x480B
#define MSG_REPLAY_CURR_HAND_INFO_REPLY    0x480C

#define MSG_REPLAY_GOTO_I_TH_HAND /* .. */ 0x480D
#define MSG_REPLAY_GOTO_I_TH_HAND_REPLY    0x480E

#define MSG_REPLAY_WAIT /* ............ */ 0x480F

#define MSG_REPLAY_TIMER_ERROR /* ..... */ 0x4810

#define MSG_REPLAY_HAND_IDS /* ........ */ 0x4811
#define MSG_REPLAY_HAND_IDS_REPLY /* .. */ 0x4812

#define MSG_REPLAY_GOTO_HAND_ID /* .... */ 0x4813
#define MSG_REPLAY_GOTO_HAND_ID_REPLY      0x4814

#define MSG_REPLAY_ADD_BOOKMARK /* .... */ 0x4815
#define MSG_REPLAY_ADD_BOOKMARK_REPLY      0x4816

#define MSG_REPLAY_DEL_BOOKMARK /* .... */ 0x4817
#define MSG_REPLAY_DEL_BOOKMARK_REPLY      0x4818

#define MSG_REPLAY_HAND_BOOKMARKS /* .. */ 0x4819
#define MSG_REPLAY_HAND_BOOKMARKS_REPLY    0x481A

#define MSG_REPLAY_GOTO_BOOKMARK /* ... */ 0x481B
#define MSG_REPLAY_GOTO_BOOKMARK_REPLY     0x481C

#define MSG_REPLAY_CUT_BOOKMARKS /* ... */ 0x481D	// Cut hands between two bookmarks
#define MSG_REPLAY_CUT_BOOKMARKS_REPLY     0x481E

// ....................................... 0x481F

// ....................................... 0x4820

#define MSG_REPLAY_ALL_BOOKMARKS /* ... */ 0x4821
#define MSG_REPLAY_ALL_BOOKMARKS_REPLY     0x4822

#define MSG_REPLAY_PAUSE_AFTER_HAND        0x4823
#define MSG_REPLAY_PAUSE_AFTER_HAND_REPLY  0x4824

#define MSG_REPLAY_STARTED /* ......... */ 0x4825
#define MSG_REPLAY_STARTED_REPLY /* ... */ 0x4826

#define MSG_REPLAY_FINISHED /* ........ */ 0x4827
#define MSG_REPLAY_FINISHED_REPLY /* .. */ 0x4828

#define MSG_REPLAY_ANNOUNCED /* ....... */ 0x4829
#define MSG_REPLAY_ANNOUNCED_REPLY /* . */ 0x482A

#define MSG_REPLAY_HAND_IDS_64 /* ..... */ 0x482B
#define MSG_REPLAY_HAND_IDS_64_REPLY       0x482C

#define MSG_REPLAY_CURR_HAND_INFO_64       0x482D
#define MSG_REPLAY_CURR_HAND_INFO_64_REPLY 0x482E

// ....................................... 0x482F

// ....................................... 0x4830

#define MSG_REPLAY_GOTO_HAND_ID_64 /* . */ 0x4831
#define MSG_REPLAY_GOTO_HAND_ID_64_REPLY   0x4832

#define MSG_REPLAY_HANDS_QUANTITY_64       0x4833
#define MSG_REPLAY_HANDS_QUANTITY_64_REPLY 0x4834

#define MSG_REPLAY_GOTO_I_TH_HAND_64       0x4835
#define MSG_REPLAY_GOTO_I_TH_HAND_64_REPLY 0x4836

#define MSG_HERCULES_LAST /* .......... */ 0x48FF

#define REPLAY_STARTED_AUTOMATICALLY	0
#define REPLAY_STARTED_MANUALLY			1

#define TIMER_LOBBY_MSGCHEKER_STATS_INTERVAL   60000

#endif
