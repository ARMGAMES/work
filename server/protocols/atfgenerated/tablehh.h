/**
 * tablehh.h
 *
 * This file was auto-generated from tablehh.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: tablehh.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_TABLEHH_H
#define ATF_PROTOCOL_DEFFILE_TABLEHH_H

#include "../../atf/UserDefStructs.h"


namespace HandHist
{
	namespace content
	{
		/*
		 * since all the following data structs go to database for hand history persistence,
		 * all future fields must be added in the end with ParseEndOptB4Me modifier.
		 */

		struct PlayerAndBounty
		{
			// M(s88s4) - M(userId,bounty,tournId,ticketAdmission,tchips)
			PString userId;
			INT64 bounty;
			UINT64 tournId;
			PString ticketAdmission;
			UINT32 tchips;

			PlayerAndBounty();
			void clear();
			bool equals(const PlayerAndBounty&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		};

		struct BountyWon
		{
			// 4<M(s88s4)> - numBountyWinners<M(userId,bounty,tournId,ticketAdmission,tchips)>
			Atf::LAtfVector< PlayerAndBounty , 4 > bountyWinners;

			BountyWon();
			void clear();
			bool equals(const BountyWon&) const;
			const char *toTraceString(PString& _str) const;
			const char *toXmlString(const char *_element, PString& _str) const;
			bool fromXmlString(const PString& _str);
			void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
			void parseMsg(CommMsgParser& _parser);
			const char *toJSONString(PString& _str) const;
			void fromJSONString(const PString& _str);
			static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
			static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		};

	};
};

#endif
