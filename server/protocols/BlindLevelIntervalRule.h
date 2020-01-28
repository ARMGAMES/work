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

#ifndef blind_level_interval_h_included
#define blind_level_interval_h_included

#include "tourndef.h"
#include "UserDefStructs.h"

struct BlindLevelIntervalRule
{
	UINT32 levelType;
	INT32 threshold;
	vector<INT32> handsOrMins;

	BlindLevelIntervalRule()
	{
		clear();
	}

	void clear()
	{
		levelType = BlindLevelIntervalRuleType_None;
		threshold = 0;
		handsOrMins.clear();
	}

	const char* toTraceString(PString& _buf) const
	{
		_buf.append('{');
		_buf.append("levelType=");
		_buf.appendInt(levelType);
		_buf.append(',');
		_buf.append("threshold=");
		_buf.appendInt(threshold);
		_buf.append(',');
		_buf.append("prizes=");
		_buf.append('[');
		for ( size_t _i = 0; _i < handsOrMins.size(); ++_i )
		{
			if ( _i > 0 )	_buf.append(',');
			_buf.appendInt(handsOrMins[_i]);
		}
		_buf.append(']');
		_buf.append('}');
		return _buf.c_str();
	}

	void compose(CommMsgBody &body) const
	{
		CommMsgBody myBody;
		myBody.composeUINT32(levelType)
			.composeINT32(threshold)
			.composeINT32(handsOrMins.size());

		for( size_t i = 0; i < handsOrMins.size(); ++i )
		{
			CommMsgBody tempBody;
			tempBody.composeINT32(handsOrMins[i]);
			myBody.composeMsgBody(tempBody);
		}

		body.composeMsgBody(myBody);
	}

	void parse(CommMsgParser &parser)
	{
		CommMsgBody body;
		parser.parseMsgBody(body);
		CommMsgParser myParser(body);
		UINT32 myType;
		INT32 numValues;

		myParser.parseUINT32(myType)
				.parseINT32(threshold)
				.parseINT32(numValues);

		for( int i = 0; i < numValues; ++i )
		{
			INT32 value;
			CommMsgBody valueBody;
			myParser.parseMsgBody(valueBody);
			CommMsgParser valueParser(valueBody);
			valueParser.parseINT32(value);
			handsOrMins.push_back(value);
		}

		levelType = myType;
	}

	void composeMsg(CommMsgBody& body, bool _ignoreJSON = false) const { compose(body); }
	void parseMsg(CommMsgParser& parser) { parse(parser); }

	bool equals(const BlindLevelIntervalRule& other) const
	{
		return levelType == other.levelType &&
			threshold == other.threshold &&
			handsOrMins == other.handsOrMins;
	}

	static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
	{
		UINT32 levelType; _parser.parseUINT32(levelType);
		AtfValidator::validateInt(_descr, "levelType", levelType, _checker, __FILE__, __LINE__);
		INT32 threshold; _parser.parseINT32(threshold);
		AtfValidator::validateInt(_descr, "threshold", threshold, _checker, __FILE__, __LINE__);
		
		// TODO is this validation for array ok?
		UINT32 _n = 0;
		_parser.parseUINT32(_n);
		for ( UINT32 _i = 0; _i < _n; ++_i )
		{
			INT32 handsOrMin; _parser.parseINT32(handsOrMin);
			AtfValidator::validateInt(_descr, "handsOrMin", handsOrMin, _checker, __FILE__, __LINE__);
		}
	}

	// placeholders
	const char *toXmlString(const char *_element, PString& _str) const { return _str; }
	bool fromXmlString(const PString& _str) { return true; }
	const char *toJSONString(PString& _str) const { return _str; }
	void fromJSONString(const PString& _str) {}
	static void validateJSONString(const PString& _str, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent) {}
};


#endif // blind_level_interval_h_included

