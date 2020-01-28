/**
 * NStackTournInfo_thin.cpp
 *
 * This file was auto-generated from NStackTournInfo.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin NStackTournInfo.txt
 */
 
#include "NStackTournInfo_thin.h"

//=================================================================
//                    NStackTournInfoStruct
//=================================================================

TournClientThin::NStackTournInfoStruct::NStackTournInfoStruct()
{
	clear();
}

void TournClientThin::NStackTournInfoStruct::clear()
{
	initialStacks = 0;
	unitStackChips = 0;
	forceReloadAllStacksInLevel = 0;
	forceReloadAllStacksInMinutes = 0;
	reloadStacksOnlyWhenEmpty = false;
}

bool TournClientThin::NStackTournInfoStruct::equals(const NStackTournInfoStruct& _o) const
{
	return initialStacks == _o.initialStacks &&
		unitStackChips == _o.unitStackChips &&
		forceReloadAllStacksInLevel == _o.forceReloadAllStacksInLevel &&
		forceReloadAllStacksInMinutes == _o.forceReloadAllStacksInMinutes &&
		reloadStacksOnlyWhenEmpty == _o.reloadStacksOnlyWhenEmpty;
}

const char *TournClientThin::NStackTournInfoStruct::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("initialStacks=");
	_buf.appendInt(initialStacks);
	_buf.append(',');
	_buf.append("unitStackChips=");
	_buf.appendInt(unitStackChips);
	_buf.append(',');
	_buf.append("forceReloadAllStacksInLevel=");
	_buf.appendUint(forceReloadAllStacksInLevel);
	_buf.append(',');
	_buf.append("forceReloadAllStacksInMinutes=");
	_buf.appendUint(forceReloadAllStacksInMinutes);
	_buf.append(',');
	_buf.append("reloadStacksOnlyWhenEmpty=");
	_buf.appendUint(reloadStacksOnlyWhenEmpty);
	_buf.append('}');
	return _buf.c_str();
}

void TournClientThin::NStackTournInfoStruct::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeINT32(initialStacks);
	_msg.composeINT32(unitStackChips);
	_msg.composeUINT32(forceReloadAllStacksInLevel);
	_msg.composeUINT32(forceReloadAllStacksInMinutes);
	_msg.composeBOOL(reloadStacksOnlyWhenEmpty);
}

void TournClientThin::NStackTournInfoStruct::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT32(initialStacks);
	_parser.parseINT32(unitStackChips);
	_parser.parseUINT32(forceReloadAllStacksInLevel);
	_parser.parseUINT32(forceReloadAllStacksInMinutes);
	_parser.parseBOOL(reloadStacksOnlyWhenEmpty);
}

/*static*/ void TournClientThin::NStackTournInfoStruct::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	INT32 initialStacks; _parser.parseINT32(initialStacks);
	AtfValidator::validateInt(_descr, "initialStacks", initialStacks, _checker, __FILE__, __LINE__);
	INT32 unitStackChips; _parser.parseINT32(unitStackChips);
	AtfValidator::validateInt(_descr, "unitStackChips", unitStackChips, _checker, __FILE__, __LINE__);
	UINT32 forceReloadAllStacksInLevel; _parser.parseUINT32(forceReloadAllStacksInLevel);
	AtfValidator::validateInt(_descr, "forceReloadAllStacksInLevel", forceReloadAllStacksInLevel, _checker, __FILE__, __LINE__);
	UINT32 forceReloadAllStacksInMinutes; _parser.parseUINT32(forceReloadAllStacksInMinutes);
	AtfValidator::validateInt(_descr, "forceReloadAllStacksInMinutes", forceReloadAllStacksInMinutes, _checker, __FILE__, __LINE__);
	bool reloadStacksOnlyWhenEmpty; _parser.parseBOOL(reloadStacksOnlyWhenEmpty);
	AtfValidator::validateInt(_descr, "reloadStacksOnlyWhenEmpty", reloadStacksOnlyWhenEmpty, _checker, __FILE__, __LINE__);
}

