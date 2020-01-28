/**
 * upservercommonprot.h
 *
 * This file was auto-generated from upservercommonprot.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor upservercommonprot.txt
 */

#ifndef ATF_PROTOCOL_DEFFILE_UPSERVERCOMMONPROT_H
#define ATF_PROTOCOL_DEFFILE_UPSERVERCOMMONPROT_H

#include "UserDefStructs.h"
#include "upserver/UPServer.h"


namespace UPServerCommon
{
	struct Property
	{
		//! M(44 <FIXME>CommUtcTime</FIXME> M()) - M(propertyTypeId,value, <FIXME>validUntil</FIXME> ,M())
		INT32 propertyTypeId;	// please see UserPropertyType enum above
		INT32 value;	// so far this is a free format value
		CommUtcTime validUntil;	// set it to 12/31/9999 for never expiring properties
		Atf::CopyableMsgBody propertyAttributes;

		Property();
		void clear();
		bool equals(const Property&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		Property(Property&&);
		Property& operator=(Property&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		Property(const Property&) = default;
		Property& operator=(const Property&) = default;
#endif
#endif
	};

	struct PropertyForClient
	{
		// M(44) - M(propertyTypeId,value)
		INT32 propertyTypeId;	// please see UserPropertyType enum above
		INT32 value;	// so far this is a free format value

		PropertyForClient();
		void clear();
		bool equals(const PropertyForClient&) const;
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

	struct UserPropertyLeaf
	{
		// 4<M(44)> - numProperties<M(propertyTypeId,value)>
		Atf::LAtfVector< PropertyForClient, 4 > properties;

		UserPropertyLeaf();
		void clear();
		bool equals(const UserPropertyLeaf&) const;
		const char *toTraceString(PString& _str) const;
		const char *toXmlString(const char *_element, PString& _str) const;
		bool fromXmlString(const PString& _str);
		void composeMsg(CommMsgBody& _msg, bool _ignoreJSON = COMPOSEMSG_IGNORE_JSON_DEFAULT) const;
		void parseMsg(CommMsgParser& _parser);
		const char *toJSONString(PString& _str) const;
		void fromJSONString(const PString& _str);
		static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		UserPropertyLeaf(UserPropertyLeaf&&);
		UserPropertyLeaf& operator=(UserPropertyLeaf&&);
#ifdef ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
		UserPropertyLeaf(const UserPropertyLeaf&) = default;
		UserPropertyLeaf& operator=(const UserPropertyLeaf&) = default;
#endif
#endif
	};

	struct CardMatch_Winner_Prop
	{
		// 184 - gameType,gameId,entryId
		BYTE gameType;	// enum MiniGameCommon::EntryType
		UINT64 gameId;	// TableId or BlitzId or TournId
		UINT32 entryId;	// for Zoom

		CardMatch_Winner_Prop();
		void clear();
		bool equals(const CardMatch_Winner_Prop&) const;
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

	struct CardMatch_Entry_Prop
	{
		// 1848 - gameType,gameId,entryId,subGameId
		BYTE gameType;	// enum MiniGameCommon::EntryType
		UINT64 gameId;	// TableId or BlitzId or TournId
		UINT32 entryId;	// for Zoom
		UINT64 subGameId;	// id of a child object of gameId, e.g., tableId of table in a tournament

		CardMatch_Entry_Prop();
		void clear();
		bool equals(const CardMatch_Entry_Prop&) const;
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
