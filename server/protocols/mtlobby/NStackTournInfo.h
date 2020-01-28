/**
 * NStackTournInfo.h
 *
 * This file was auto-generated from NStackTournInfo.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor NStackTournInfo.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_NSTACKTOURNINFO_H
#define ATF_PROTOCOL_DEFFILE_NSTACKTOURNINFO_H


#include "UserDefStructs.h"

namespace TournClient
{
	//PYR-25226
	struct NStackTournInfoStruct
	{
		// 4444b - initialStacks,unitStackChips,forceReloadAllStacksInLevel,forceReloadAllStacksInMinutes,reloadStacksOnlyWhenEmpty
		INT32 initialStacks;
		INT32 unitStackChips;
		UINT32 forceReloadAllStacksInLevel;
		UINT32 forceReloadAllStacksInMinutes;
		bool reloadStacksOnlyWhenEmpty;

		NStackTournInfoStruct();
		void clear();
		bool equals(const NStackTournInfoStruct&) const;
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

#endif
