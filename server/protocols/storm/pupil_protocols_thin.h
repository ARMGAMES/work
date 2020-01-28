/**
 * pupil_protocols_thin.h
 *
 * This file was auto-generated from pupil_protocols.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin pupil_protocols.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_PUPIL_PROTOCOLS_THIN_H
#define ATF_PROTOCOL_DEFFILE_PUPIL_PROTOCOLS_THIN_H

//------------------------------------------------------------------------------
//	Pupil - Power Up Poker in Lobby - Protocols
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2016 PYR Software Ltd. All rights reserved. This document
//	is for the use of licensed users only and is subject to the terms and
//	conditions of the accompanying and/or applicable license between you and
//	PYR Software Ltd, including without limitation those terms and conditions
//	prohibiting unauthorized translation, reverse engineering, decompilation,
//	disassembly, and other inappropriate use of any software provided. Any
//	unauthorized use, copying, modification, distribution, publication or
//	disclosure of information is a violation of copyright laws. No reproduction
//	in whole or in part of this document may be made without express written
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#include "ThinAtfUserDefStructs.h"
#include "ppcontainer.h"
#include "PBitmask.h"
#include "sitemasks.h"
#include "licensemask.h"


namespace _PupilThin
{
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	// Pupil Publication
	//------------------------------------------------------------------------------
	//
	// Node { 0 } - list of games
	//

	//------------------------------------------------------------------------------
	// Pupil Publication: Game Leaf
	//------------------------------------------------------------------------------

	struct GameStaticLeaf	// Leaf { 0, n, 0 }
	{
		struct _VisibilityType
		{
			UINT32 brand;	// visibility mask
			UINT32 siteObsolete;	// visibility mask 32 bits

			bool excludeCountries;
			ThinAtf::LAtfVector< PString , 4 > countries;	// country inlcusion/exclusion list

			PString criteria;	// criteria name to look up in Help Support Portal
			PSiteMask siteMask;	// visibility mask
			mutable bool _is_empty_internal;

			_VisibilityType();
			void clear();
			bool isEmptyMsg() const { if(_is_empty_internal) _is_empty_internal = equals(_VisibilityType()); return _is_empty_internal; }
			bool equals(const _VisibilityType&) const;
			const char *toTraceString(PString& _str) const;
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			_VisibilityType(_VisibilityType&&);
			_VisibilityType& operator=(_VisibilityType&&);
		private:
			_VisibilityType(const _VisibilityType&);  // prohibit
			_VisibilityType& operator=(const _VisibilityType&);  // prohibit
#endif

		private: // helper functions
			void parseAnonymousMsgBody0(CommMsgParser& _parser);
			static void validateAnonymousMsgBody0(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		};

		struct _SampleType
		{
			UINT32 tournId;
			PString serverAddr;
			PString serverObject;
			mutable bool _is_empty_internal;

			_SampleType();
			void clear();
			bool isEmptyMsg() const { if(_is_empty_internal) _is_empty_internal = equals(_SampleType()); return _is_empty_internal; }
			bool equals(const _SampleType&) const;
			const char *toTraceString(PString& _str) const;
			void composeMsg(CommMsgBody& _msg) const;
			void parseMsg(CommMsgParser& _parser);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
			_SampleType(_SampleType&&);
			_SampleType& operator=(_SampleType&&);
		private:
			_SampleType(const _SampleType&);  // prohibit
			_SampleType& operator=(const _SampleType&);  // prohibit
#endif
		};

		// b[b4M(44M(b4<s>)sm)M(4ss)I11b1bs88818141I484] - visible[regOpen,gameId,M(brand,siteObsolete,M(excludeCountries,numCountries<countries>),criteria,siteMask),M(tournId,serverAddr,serverObject),name,handType,structure,isHiLo,speedDisplay,isPlayMoney,currency,buyIn,rake,prizePool,prizePoolUnit,prizePoolMoneyValue,playerPerTable,minPlayers,maxUserRequests,customSpeedDisplayName,tournFlags,tournFlags2,tournFlagsServInt]
		bool visible;

		bool regOpen;

		UINT32 gameId;
		_VisibilityType visibility;
		_SampleType sample;

		I18nPString name;

		BYTE handType;
		BYTE structure;
		bool isHiLo;

		BYTE speedDisplay;

		bool isPlayMoney;
		PString currency;
		INT64 buyIn;
		INT64 rake;

		INT64 prizePool;
		BYTE prizePoolUnit;
		INT64 prizePoolMoneyValue;

		BYTE playerPerTable;
		UINT32 minPlayers;

		BYTE maxUserRequests;

		I18nPString customSpeedDisplayName;

		UINT32 tournFlags;
		UINT64 tournFlags2;
		UINT32 tournFlagsServInt;

		GameStaticLeaf();
		void clear();
		bool equals(const GameStaticLeaf&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		GameStaticLeaf(GameStaticLeaf&&);
		GameStaticLeaf& operator=(GameStaticLeaf&&);
	private:
		GameStaticLeaf(const GameStaticLeaf&);  // prohibit
		GameStaticLeaf& operator=(const GameStaticLeaf&);  // prohibit
#endif
	};

	struct GameDynamicLeaf	// Leaf { 0, n, 1 }
	{
		// 44444 - numUsersWaiting,numUsersPlaying,numUsersRegistered,numDistinctUsersWaiting,numActiveGames
		UINT32 numUsersWaiting;
		UINT32 numUsersPlaying;
		UINT32 numUsersRegistered;
		UINT32 numDistinctUsersWaiting;
		UINT32 numActiveGames;

		GameDynamicLeaf();
		void clear();
		bool equals(const GameDynamicLeaf&) const;
		const char *toTraceString(PString& _str) const;
		void composeMsg(CommMsgBody& _msg) const;
		void parseMsg(CommMsgParser& _parser);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		GameDynamicLeaf(GameDynamicLeaf&&);
		GameDynamicLeaf& operator=(GameDynamicLeaf&&);
	private:
		GameDynamicLeaf(const GameDynamicLeaf&);  // prohibit
		GameDynamicLeaf& operator=(const GameDynamicLeaf&);  // prohibit
#endif
	};
};

#endif
