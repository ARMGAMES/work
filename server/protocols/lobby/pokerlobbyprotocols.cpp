/**
 * pokerlobbyprotocols.cpp
 *
 * This file was auto-generated from pokerlobbyprotocols.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: pokerlobbyprotocols.txt
 */
 
#include "pokerlobbyprotocols.h"

//=================================================================
//                    TableCaps
//=================================================================

PokerLobbyMsgs::PokerLobbyServerData::TableCaps::TableCaps()
{
	clear();
}

void PokerLobbyMsgs::PokerLobbyServerData::TableCaps::clear()
{
	tableCapTotal = 0;
	tableCapZoom = 0;
}

bool PokerLobbyMsgs::PokerLobbyServerData::TableCaps::equals(const TableCaps& _o) const
{
	return tableCapTotal == _o.tableCapTotal &&
		tableCapZoom == _o.tableCapZoom;
}

const char *PokerLobbyMsgs::PokerLobbyServerData::TableCaps::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("tableCapTotal=");
	_buf.appendInt(tableCapTotal);
	_buf.append(',');
	_buf.append("tableCapZoom=");
	_buf.appendInt(tableCapZoom);
	_buf.append('}');
	return _buf.c_str();
}

const char *PokerLobbyMsgs::PokerLobbyServerData::TableCaps::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("tableCapTotal", tableCapTotal, _buf);
	Atf::XmlElement::encodeAsXmlElement("tableCapZoom", tableCapZoom, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool PokerLobbyMsgs::PokerLobbyServerData::TableCaps::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _str.length(), _element, _value, _type))
	{
		if (_element.equals("tableCapTotal"))
		{
			tableCapTotal = (INT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("tableCapZoom"))
		{
			tableCapZoom = (INT32)p_atoi64((const char *)_value.ptr());
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void PokerLobbyMsgs::PokerLobbyServerData::TableCaps::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT32(tableCapTotal);
	_msg.composeINT32(tableCapZoom);
}

void PokerLobbyMsgs::PokerLobbyServerData::TableCaps::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT32(tableCapTotal);
	_parser.parseINT32(tableCapZoom);
}

const char *PokerLobbyMsgs::PokerLobbyServerData::TableCaps::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("tableCapTotal", tableCapTotal);
	_jsonstr.compose("tableCapZoom", tableCapZoom);
	return _buf.c_str();
}

void PokerLobbyMsgs::PokerLobbyServerData::TableCaps::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("tableCapTotal", tableCapTotal);
	_jparser.parseByNameThrow("tableCapZoom", tableCapZoom);
}

/* static */ void PokerLobbyMsgs::PokerLobbyServerData::TableCaps::validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	Atf::JSONStringValidatorWithChecker _jparser(_jsonString, _checker, _descr, _fieldsWithUnparsedContent);
	INT32 tableCapTotal; _jparser.validateByNameThrow("tableCapTotal", tableCapTotal);
	AtfValidator::validateInt(_descr, "tableCapTotal", tableCapTotal, _checker, __FILE__, __LINE__);
	INT32 tableCapZoom; _jparser.validateByNameThrow("tableCapZoom", tableCapZoom);
	AtfValidator::validateInt(_descr, "tableCapZoom", tableCapZoom, _checker, __FILE__, __LINE__);
}

/*static*/ void PokerLobbyMsgs::PokerLobbyServerData::TableCaps::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	INT32 tableCapTotal; _parser.parseINT32(tableCapTotal);
	AtfValidator::validateInt(_descr, "tableCapTotal", tableCapTotal, _checker, __FILE__, __LINE__);
	INT32 tableCapZoom; _parser.parseINT32(tableCapZoom);
	AtfValidator::validateInt(_descr, "tableCapZoom", tableCapZoom, _checker, __FILE__, __LINE__);
}

//=================================================================
//                    PlayerStats
//=================================================================

PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::PlayerStats()
{
	clear();
}

void PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::clear()
{
	numTables = 0;
	numPlayers = 0;
	numTourn = 0;
	numBlitz = 0;
}

bool PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::equals(const PlayerStats& _o) const
{
	return numTables == _o.numTables &&
		numPlayers == _o.numPlayers &&
		numTourn == _o.numTourn &&
		numBlitz == _o.numBlitz;
}

const char *PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("numTables=");
	_buf.appendUint(numTables);
	_buf.append(',');
	_buf.append("numPlayers=");
	_buf.appendUint(numPlayers);
	_buf.append(',');
	_buf.append("numTourn=");
	_buf.appendUint(numTourn);
	_buf.append(',');
	_buf.append("numBlitz=");
	_buf.appendUint(numBlitz);
	_buf.append('}');
	return _buf.c_str();
}

const char *PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("numTables", numTables, _buf);
	Atf::XmlElement::encodeAsXmlElement("numPlayers", numPlayers, _buf);
	Atf::XmlElement::encodeAsXmlElement("numTourn", numTourn, _buf);
	Atf::XmlElement::encodeAsXmlElement("numBlitz", numBlitz, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _str.length(), _element, _value, _type))
	{
		if (_element.equals("numTables"))
		{
			numTables = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("numPlayers"))
		{
			numPlayers = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("numTourn"))
		{
			numTourn = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("numBlitz"))
		{
			numBlitz = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeUINT32(numTables);
	_msg.composeUINT32(numPlayers);
	_msg.composeUINT32(numTourn);
	_msg.composeUINT32(numBlitz);
}

void PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(numTables);
	_parser.parseUINT32(numPlayers);
	_parser.parseUINT32(numTourn);
	_parser.parseUINT32(numBlitz);
}

const char *PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("numTables", numTables);
	_jsonstr.compose("numPlayers", numPlayers);
	_jsonstr.compose("numTourn", numTourn);
	_jsonstr.compose("numBlitz", numBlitz);
	return _buf.c_str();
}

void PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("numTables", numTables);
	_jparser.parseByNameThrow("numPlayers", numPlayers);
	_jparser.parseByNameThrow("numTourn", numTourn);
	_jparser.parseByNameThrow("numBlitz", numBlitz);
}

/* static */ void PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	Atf::JSONStringValidatorWithChecker _jparser(_jsonString, _checker, _descr, _fieldsWithUnparsedContent);
	UINT32 numTables; _jparser.validateByNameThrow("numTables", numTables);
	AtfValidator::validateInt(_descr, "numTables", numTables, _checker, __FILE__, __LINE__);
	UINT32 numPlayers; _jparser.validateByNameThrow("numPlayers", numPlayers);
	AtfValidator::validateInt(_descr, "numPlayers", numPlayers, _checker, __FILE__, __LINE__);
	UINT32 numTourn; _jparser.validateByNameThrow("numTourn", numTourn);
	AtfValidator::validateInt(_descr, "numTourn", numTourn, _checker, __FILE__, __LINE__);
	UINT32 numBlitz; _jparser.validateByNameThrow("numBlitz", numBlitz);
	AtfValidator::validateInt(_descr, "numBlitz", numBlitz, _checker, __FILE__, __LINE__);
}

/*static*/ void PokerLobbyMsgs::PokerLobbyServerData::PlayerStats::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	UINT32 numTables; _parser.parseUINT32(numTables);
	AtfValidator::validateInt(_descr, "numTables", numTables, _checker, __FILE__, __LINE__);
	UINT32 numPlayers; _parser.parseUINT32(numPlayers);
	AtfValidator::validateInt(_descr, "numPlayers", numPlayers, _checker, __FILE__, __LINE__);
	UINT32 numTourn; _parser.parseUINT32(numTourn);
	AtfValidator::validateInt(_descr, "numTourn", numTourn, _checker, __FILE__, __LINE__);
	UINT32 numBlitz; _parser.parseUINT32(numBlitz);
	AtfValidator::validateInt(_descr, "numBlitz", numBlitz, _checker, __FILE__, __LINE__);
}
