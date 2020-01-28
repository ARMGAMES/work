#include "friendserver_atf.h"

//=================================================================
//                    FriendStruct
//=================================================================

friendserver::cli::FriendStruct::FriendStruct()
{
	clear();
}

void friendserver::cli::FriendStruct::clear()
{
	name.clear();
	imageId = 0;
	isOnline = false;
	hasPendingChallenge = false;
	isNewFriend = false;
	hasRMPendingChallenge = false;
	playerFlag = 0;
}

bool friendserver::cli::FriendStruct::equals(const FriendStruct& _o) const
{
	return name.equals(_o.name) &&
		imageId == _o.imageId &&
		isOnline == _o.isOnline &&
		hasPendingChallenge == _o.hasPendingChallenge &&
		isNewFriend == _o.isNewFriend &&
		hasRMPendingChallenge == _o.hasRMPendingChallenge &&
		playerFlag == _o.playerFlag;
}

const char *friendserver::cli::FriendStruct::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("name=");
	_buf.append(name);
	_buf.append(',');
	_buf.append("imageId=");
	_buf.appendUint(imageId);
	_buf.append(',');
	_buf.append("isOnline=");
	_buf.appendUint(isOnline);
	_buf.append(',');
	_buf.append("hasPendingChallenge=");
	_buf.appendUint(hasPendingChallenge);
	_buf.append(',');
	_buf.append("isNewFriend=");
	_buf.appendUint(isNewFriend);
	_buf.append(',');
	_buf.append("hasRMPendingChallenge=");
	_buf.appendUint(hasRMPendingChallenge);
	_buf.append(',');
	_buf.append("playerFlag=");
	_buf.appendUint(playerFlag);
	_buf.append('}');
	return _buf.c_str();
}

const char *friendserver::cli::FriendStruct::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("name", name, _buf);
	Atf::XmlElement::encodeAsXmlElement("imageId", imageId, _buf);
	Atf::XmlElement::encodeAsXmlElement("isOnline", isOnline, _buf);
	Atf::XmlElement::encodeAsXmlElement("hasPendingChallenge", hasPendingChallenge, _buf);
	Atf::XmlElement::encodeAsXmlElement("isNewFriend", isNewFriend, _buf);
	Atf::XmlElement::encodeAsXmlElement("hasRMPendingChallenge", hasRMPendingChallenge, _buf);
	Atf::XmlElement::encodeAsXmlElement("playerFlag", playerFlag, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool friendserver::cli::FriendStruct::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
	{
		if (_element.equals("name"))
		{
			if(!Atf::XmlElement::movePBlockToPString(_value, name)) return false;
		}
		else if (_element.equals("imageId"))
		{
			imageId = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("isOnline"))
		{
			isOnline = (*_value.ptr() == '1');
		}
		else if (_element.equals("hasPendingChallenge"))
		{
			hasPendingChallenge = (*_value.ptr() == '1');
		}
		else if (_element.equals("isNewFriend"))
		{
			isNewFriend = (*_value.ptr() == '1');
		}
		else if (_element.equals("hasRMPendingChallenge"))
		{
			hasRMPendingChallenge = (*_value.ptr() == '1');
		}
		else if (_element.equals("playerFlag"))
		{
			playerFlag = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void friendserver::cli::FriendStruct::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	CommMsgBody _body;
	//if (!equals(FriendStruct())) // not empty
	{
		_body.composeString(name);
		_body.composeUINT32(imageId);
		_body.composeBOOL(isOnline);
		_body.composeBOOL(hasPendingChallenge);
		_body.composeBOOL(isNewFriend);
		_body.composeBOOL(hasRMPendingChallenge);
		_body.composeUINT32(playerFlag);
	}

	_msg.composeMsgBody(_body);
}

void friendserver::cli::FriendStruct::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);
	
	//if(!_body._size())
	//{
	//	return;
	//}

	CommMsgParser _parser0(_body);
	_parser0.parseStringP(name);
	_parser0.parseUINT32(imageId);
	_parser0.parseBOOL(isOnline);
	_parser0.parseBOOL(hasPendingChallenge);
	_parser0.parseBOOL(isNewFriend);
	_parser0.parseBOOL(hasRMPendingChallenge);
	_parser0.parseUINT32(playerFlag);
}

void friendserver::cli::FriendStruct::parseMsg(Atf::MsgParserWithJSON& _parser)
{
	Atf::MsgParserWithJSONForMsgBodyStruct _parser0(_parser);
	_parser0.parse("name", name);
	_parser0.parse("imageId", imageId);
	_parser0.parse("isOnline", isOnline);
	_parser0.parse("hasPendingChallenge", hasPendingChallenge);
	_parser0.parse("isNewFriend", isNewFriend);
	_parser0.parse("hasRMPendingChallenge", hasRMPendingChallenge);
	_parser0.parse("playerFlag", playerFlag);
}

const char *friendserver::cli::FriendStruct::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("name", name);
	_jsonstr.compose("imageId", imageId);
	_jsonstr.compose("isOnline", isOnline);
	_jsonstr.compose("hasPendingChallenge", hasPendingChallenge);
	_jsonstr.compose("isNewFriend", isNewFriend);
	_jsonstr.compose("hasRMPendingChallenge", hasRMPendingChallenge);
	_jsonstr.compose("playerFlag", playerFlag);
	return _buf.c_str();
}

void friendserver::cli::FriendStruct::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("name", name);
	_jparser.parseByNameThrow("imageId", imageId);
	_jparser.parseByNameThrow("isOnline", isOnline);
	_jparser.parseByNameThrow("hasPendingChallenge", hasPendingChallenge);
	_jparser.parseByNameThrow("isNewFriend", isNewFriend);
	_jparser.parseByNameThrow("hasRMPendingChallenge", hasRMPendingChallenge);
	_jparser.parseByNameThrow("playerFlag", playerFlag);
}

//=================================================================
//                    FriendReqStruct
//=================================================================

friendserver::cli::FriendReqStruct::FriendReqStruct()
{
	clear();
}

void friendserver::cli::FriendReqStruct::clear()
{
	name.clear();
	imageId = 0;
	isOnline = false;
	playerFlag = 0;
}

bool friendserver::cli::FriendReqStruct::equals(const FriendReqStruct& _o) const
{
	return name.equals(_o.name) &&
		imageId == _o.imageId &&
		isOnline == _o.isOnline &&
		playerFlag == _o.playerFlag;
}

const char *friendserver::cli::FriendReqStruct::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("name=");
	_buf.append(name);
	_buf.append(',');
	_buf.append("imageId=");
	_buf.appendUint(imageId);
	_buf.append(',');
	_buf.append("isOnline=");
	_buf.appendUint(isOnline);
	_buf.append(',');
	_buf.append("playerFlag=");
	_buf.appendUint(playerFlag);
	_buf.append('}');
	return _buf.c_str();
}

const char *friendserver::cli::FriendReqStruct::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("name", name, _buf);
	Atf::XmlElement::encodeAsXmlElement("imageId", imageId, _buf);
	Atf::XmlElement::encodeAsXmlElement("isOnline", isOnline, _buf);
	Atf::XmlElement::encodeAsXmlElement("playerFlag", playerFlag, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool friendserver::cli::FriendReqStruct::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
	{
		if (_element.equals("name"))
		{
			if(!Atf::XmlElement::movePBlockToPString(_value, name)) return false;
		}
		else if (_element.equals("imageId"))
		{
			imageId = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("isOnline"))
		{
			isOnline = (*_value.ptr() == '1');
		}
		else if (_element.equals("playerFlag"))
		{
			playerFlag = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void friendserver::cli::FriendReqStruct::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	CommMsgBody _body;
	//if (!equals(FriendReqStruct())) // not empty
	{
		_body.composeString(name);
		_body.composeUINT32(imageId);
		_body.composeBOOL(isOnline);
		_body.composeUINT32(playerFlag);
	}

	_msg.composeMsgBody(_body);
}

void friendserver::cli::FriendReqStruct::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);
	
	//if(!_body._size())
	//{
	//	return;
	//}

	CommMsgParser _parser0(_body);
	_parser0.parseStringP(name);
	_parser0.parseUINT32(imageId);
	_parser0.parseBOOL(isOnline);
	_parser0.parseUINT32(playerFlag);
}

void friendserver::cli::FriendReqStruct::parseMsg(Atf::MsgParserWithJSON& _parser)
{
	Atf::MsgParserWithJSONForMsgBodyStruct _parser0(_parser);
	_parser0.parse("name", name);
	_parser0.parse("imageId", imageId);
	_parser0.parse("isOnline", isOnline);
	_parser0.parse("playerFlag", playerFlag);
}

const char *friendserver::cli::FriendReqStruct::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("name", name);
	_jsonstr.compose("imageId", imageId);
	_jsonstr.compose("isOnline", isOnline);
	_jsonstr.compose("playerFlag", playerFlag);
	return _buf.c_str();
}

void friendserver::cli::FriendReqStruct::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("name", name);
	_jparser.parseByNameThrow("imageId", imageId);
	_jparser.parseByNameThrow("isOnline", isOnline);
	_jparser.parseByNameThrow("playerFlag", playerFlag);
}

//=================================================================
//                    BlockedUserStruct
//=================================================================

friendserver::cli::BlockedUserStruct::BlockedUserStruct()
{
	clear();
}

void friendserver::cli::BlockedUserStruct::clear()
{
	name.clear();
}

bool friendserver::cli::BlockedUserStruct::equals(const BlockedUserStruct& _o) const
{
	return name.equals(_o.name);
}

const char *friendserver::cli::BlockedUserStruct::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("name=");
	_buf.append(name);
	_buf.append('}');
	return _buf.c_str();
}

const char *friendserver::cli::BlockedUserStruct::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("name", name, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool friendserver::cli::BlockedUserStruct::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
	{
		if (_element.equals("name"))
		{
			if(!Atf::XmlElement::movePBlockToPString(_value, name)) return false;
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void friendserver::cli::BlockedUserStruct::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	CommMsgBody _body;
	//if (!equals(BlockedUserStruct())) // not empty
	{
		_body.composeString(name);
	}

	_msg.composeMsgBody(_body);
}

void friendserver::cli::BlockedUserStruct::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);
	
	//if(!_body._size())
	//{
	//	return;
	//}

	CommMsgParser _parser0(_body);
	_parser0.parseStringP(name);
}

void friendserver::cli::BlockedUserStruct::parseMsg(Atf::MsgParserWithJSON& _parser)
{
	Atf::MsgParserWithJSONForMsgBodyStruct _parser0(_parser);
	_parser0.parse("name", name);
}

const char *friendserver::cli::BlockedUserStruct::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("name", name);
	return _buf.c_str();
}

void friendserver::cli::BlockedUserStruct::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("name", name);
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_FRIEND_LIST
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::Protocol_MSG_FRDSRV_GET_FRIEND_LIST()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::clear()
{
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::equals(const Protocol_MSG_FRDSRV_GET_FRIEND_LIST& _o) const
{
	return true;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_FRIEND_LIST != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_FRIEND_LIST*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_FRIEND_LIST).append(")");
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_FRIEND_LIST).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::parseMsg(CommMsgParser& _parser)
{
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	friends.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::equals(const Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		friends.equals(_o.friends);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_FRIEND_LIST_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_FRIEND_LIST_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("friends=");
		friends.toTraceString(_buf);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_FRIEND_LIST_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		friends.toXmlString("friends", _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("friends"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< FriendStruct, 4 > >::FromXmlString(_value, friends)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		friends.composeMsg(_msg, _ignoreJSON);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		friends.parseMsg(_parser);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("friends", friends);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("friends", friends);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_REMOVE_FRIEND
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::Protocol_MSG_FRDSRV_REMOVE_FRIEND()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::clear()
{
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::equals(const Protocol_MSG_FRDSRV_REMOVE_FRIEND& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_REMOVE_FRIEND != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_REMOVE_FRIEND*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_REMOVE_FRIEND).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_REMOVE_FRIEND).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::equals(const Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_REMOVE_FRIEND_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_REMOVE_FRIEND_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_REMOVE_FRIEND_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_BLOCKED_LIST
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::clear()
{
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::equals(const Protocol_MSG_FRDSRV_GET_BLOCKED_LIST& _o) const
{
	return true;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_BLOCKED_LIST != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_BLOCKED_LIST*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_BLOCKED_LIST).append(")");
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_BLOCKED_LIST).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::parseMsg(CommMsgParser& _parser)
{
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	friends.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::equals(const Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		friends.equals(_o.friends);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_BLOCKED_LIST_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_BLOCKED_LIST_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("friends=");
		friends.toTraceString(_buf);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_BLOCKED_LIST_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		friends.toXmlString("friends", _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("friends"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< BlockedUserStruct, 4 > >::FromXmlString(_value, friends)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		friends.composeMsg(_msg, _ignoreJSON);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		friends.parseMsg(_parser);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("friends", friends);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("friends", friends);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_UNBLOCK_USER
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::Protocol_MSG_FRDSRV_UNBLOCK_USER()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::clear()
{
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::equals(const Protocol_MSG_FRDSRV_UNBLOCK_USER& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_UNBLOCK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_UNBLOCK_USER*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_UNBLOCK_USER).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_UNBLOCK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::equals(const Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_UNBLOCK_USER_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_UNBLOCK_USER_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_UNBLOCK_USER_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::clear()
{
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::equals(const Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST& _o) const
{
	return true;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_FRIEND_REQ_LIST != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_FRIEND_REQ_LIST).append(")");
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_FRIEND_REQ_LIST).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::parseMsg(CommMsgParser& _parser)
{
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	friends.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::equals(const Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		friends.equals(_o.friends);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("friends=");
		friends.toTraceString(_buf);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		friends.toXmlString("friends", _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("friends"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< FriendReqStruct, 4 > >::FromXmlString(_value, friends)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		friends.composeMsg(_msg, _ignoreJSON);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		friends.parseMsg(_parser);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("friends", friends);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("friends", friends);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_ADD_FRIEND_REQ
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::clear()
{
	userId.clear();
	throughUserSearch = false;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::equals(const Protocol_MSG_FRDSRV_ADD_FRIEND_REQ& _o) const
{
	return userId.equals(_o.userId) &&
		throughUserSearch == _o.throughUserSearch;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_ADD_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_ADD_FRIEND_REQ*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_ADD_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	_buf.append(',');
	_buf.append("throughUserSearch=");
	_buf.appendUint(throughUserSearch);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_ADD_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	Atf::XmlElement::encodeAsXmlElement("throughUserSearch", throughUserSearch, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else if (_element.equals("throughUserSearch"))
			{
				throughUserSearch = (*_value.ptr() == '1');
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
	_msg.composeBOOL(throughUserSearch);
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
	if(_parser.parseEnded()) return;
	_parser.parseBOOL(throughUserSearch);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	_jsonstr.compose("throughUserSearch", throughUserSearch);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	if(_jparser.parseEnded()) return;
	_jparser.parseByNameThrow("throughUserSearch", throughUserSearch);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::equals(const Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_ADD_FRIEND_REQ_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_ADD_FRIEND_REQ_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_ADD_FRIEND_REQ_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DENY_FRIEND_REQ
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::clear()
{
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::equals(const Protocol_MSG_FRDSRV_DENY_FRIEND_REQ& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DENY_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DENY_FRIEND_REQ*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DENY_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DENY_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::equals(const Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DENY_FRIEND_REQ_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DENY_FRIEND_REQ_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DENY_FRIEND_REQ_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::clear()
{
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::equals(const Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_ACCEPT_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_ACCEPT_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_ACCEPT_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::equals(const Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_BLOCK_USER
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::Protocol_MSG_FRDSRV_BLOCK_USER()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::clear()
{
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::equals(const Protocol_MSG_FRDSRV_BLOCK_USER& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_BLOCK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_BLOCK_USER*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_BLOCK_USER).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_BLOCK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_BLOCK_USER_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::equals(const Protocol_MSG_FRDSRV_BLOCK_USER_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_BLOCK_USER_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_BLOCK_USER_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_BLOCK_USER_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_BLOCK_USER_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_CHECK_USER
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::Protocol_MSG_FRDSRV_CHECK_USER()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::clear()
{
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::equals(const Protocol_MSG_FRDSRV_CHECK_USER& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_CHECK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_CHECK_USER*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_CHECK_USER).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_CHECK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_CHECK_USER_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::Protocol_MSG_FRDSRV_CHECK_USER_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	isOnline = false;
	relation = 0;
	imageId = 0;
	originalUserId.clear();
	addFrdNotAllowed = false;
	playerFlag = 0;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::equals(const Protocol_MSG_FRDSRV_CHECK_USER_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		isOnline == _o.isOnline &&
		relation == _o.relation &&
		imageId == _o.imageId &&
		originalUserId.equals(_o.originalUserId) &&
		addFrdNotAllowed == _o.addFrdNotAllowed &&
		playerFlag == _o.playerFlag;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_CHECK_USER_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_CHECK_USER_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_CHECK_USER_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("isOnline=");
		_buf.appendUint(isOnline);
		_buf.append(',');
		_buf.append("relation=");
		_buf.appendUint(relation);
		_buf.append(',');
		_buf.append("imageId=");
		_buf.appendUint(imageId);
		_buf.append(',');
		_buf.append("originalUserId=");
		_buf.append(originalUserId);
		_buf.append(',');
		_buf.append("addFrdNotAllowed=");
		_buf.appendUint(addFrdNotAllowed);
		_buf.append(',');
		_buf.append("playerFlag=");
		_buf.appendUint(playerFlag);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_CHECK_USER_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("isOnline", isOnline, _buf);
		Atf::XmlElement::encodeAsXmlElement("relation", relation, _buf);
		Atf::XmlElement::encodeAsXmlElement("imageId", imageId, _buf);
		Atf::XmlElement::encodeAsXmlElement("originalUserId", originalUserId, _buf);
		Atf::XmlElement::encodeAsXmlElement("addFrdNotAllowed", addFrdNotAllowed, _buf);
		Atf::XmlElement::encodeAsXmlElement("playerFlag", playerFlag, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("isOnline"))
			{
				isOnline = (*_value.ptr() == '1');
			}
			else if (_element.equals("relation"))
			{
				relation = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("imageId"))
			{
				imageId = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("originalUserId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, originalUserId)) return false;
			}
			else if (_element.equals("addFrdNotAllowed"))
			{
				addFrdNotAllowed = (*_value.ptr() == '1');
			}
			else if (_element.equals("playerFlag"))
			{
				playerFlag = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeBOOL(isOnline);
		_msg.composeUINT32(relation);
		_msg.composeUINT32(imageId);
		_msg.composeString(originalUserId);
		_msg.composeBOOL(addFrdNotAllowed);
		_msg.composeUINT32(playerFlag);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseBOOL(isOnline);
		_parser.parseUINT32(relation);
		_parser.parseUINT32(imageId);
		_parser.parseStringP(originalUserId);
		_parser.parseBOOL(addFrdNotAllowed);
		_parser.parseUINT32(playerFlag);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("isOnline", isOnline);
		_jsonstr.compose("relation", relation);
		_jsonstr.compose("imageId", imageId);
		_jsonstr.compose("originalUserId", originalUserId);
		_jsonstr.compose("addFrdNotAllowed", addFrdNotAllowed);
		_jsonstr.compose("playerFlag", playerFlag);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("isOnline", isOnline);
		_jparser.parseByNameThrow("relation", relation);
		_jparser.parseByNameThrow("imageId", imageId);
		_jparser.parseByNameThrow("originalUserId", originalUserId);
		_jparser.parseByNameThrow("addFrdNotAllowed", addFrdNotAllowed);
		_jparser.parseByNameThrow("playerFlag", playerFlag);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::clear()
{
	setNotRemove = false;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::equals(const Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ& _o) const
{
	return setNotRemove == _o.setNotRemove;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ).append(")");
	_buf.append(',');
	_buf.append("setNotRemove=");
	_buf.appendUint(setNotRemove);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("setNotRemove", setNotRemove, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("setNotRemove"))
			{
				setNotRemove = (*_value.ptr() == '1');
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeBOOL(setNotRemove);
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseBOOL(setNotRemove);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("setNotRemove", setNotRemove);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("setNotRemove", setNotRemove);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::equals(const Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                    RecentOppStruct
//=================================================================

friendserver::cli::RecentOppStruct::RecentOppStruct()
{
	clear();
}

void friendserver::cli::RecentOppStruct::clear()
{
	name.clear();
	imageId = 0;
	isOnline = false;
	frdRelation = eFrdRelation_none;
	playerFlag = 0;
}

bool friendserver::cli::RecentOppStruct::equals(const RecentOppStruct& _o) const
{
	return name.equals(_o.name) &&
		imageId == _o.imageId &&
		isOnline == _o.isOnline &&
		frdRelation == _o.frdRelation &&
		playerFlag == _o.playerFlag;
}

const char *friendserver::cli::RecentOppStruct::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("name=");
	_buf.append(name);
	_buf.append(',');
	_buf.append("imageId=");
	_buf.appendUint(imageId);
	_buf.append(',');
	_buf.append("isOnline=");
	_buf.appendUint(isOnline);
	_buf.append(',');
	_buf.append("frdRelation=");
	_buf.appendUint(frdRelation);
	_buf.append(',');
	_buf.append("playerFlag=");
	_buf.appendUint(playerFlag);
	_buf.append('}');
	return _buf.c_str();
}

const char *friendserver::cli::RecentOppStruct::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("name", name, _buf);
	Atf::XmlElement::encodeAsXmlElement("imageId", imageId, _buf);
	Atf::XmlElement::encodeAsXmlElement("isOnline", isOnline, _buf);
	Atf::XmlElement::encodeAsXmlElement("frdRelation", frdRelation, _buf);
	Atf::XmlElement::encodeAsXmlElement("playerFlag", playerFlag, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool friendserver::cli::RecentOppStruct::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
	{
		if (_element.equals("name"))
		{
			if(!Atf::XmlElement::movePBlockToPString(_value, name)) return false;
		}
		else if (_element.equals("imageId"))
		{
			imageId = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("isOnline"))
		{
			isOnline = (*_value.ptr() == '1');
		}
		else if (_element.equals("frdRelation"))
		{
			frdRelation = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("playerFlag"))
		{
			playerFlag = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void friendserver::cli::RecentOppStruct::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	CommMsgBody _body;
	//if (!equals(RecentOppStruct())) // not empty
	{
		_body.composeString(name);
		_body.composeUINT32(imageId);
		_body.composeBOOL(isOnline);
		_body.composeUINT32(frdRelation);
		_body.composeUINT32(playerFlag);
	}

	_msg.composeMsgBody(_body);
}

void friendserver::cli::RecentOppStruct::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);
	
	//if(!_body._size())
	//{
	//	return;
	//}

	CommMsgParser _parser0(_body);
	_parser0.parseStringP(name);
	_parser0.parseUINT32(imageId);
	_parser0.parseBOOL(isOnline);
	_parser0.parseUINT32(frdRelation);
	_parser0.parseUINT32(playerFlag);
}

void friendserver::cli::RecentOppStruct::parseMsg(Atf::MsgParserWithJSON& _parser)
{
	Atf::MsgParserWithJSONForMsgBodyStruct _parser0(_parser);
	_parser0.parse("name", name);
	_parser0.parse("imageId", imageId);
	_parser0.parse("isOnline", isOnline);
	_parser0.parse("frdRelation", frdRelation);
	_parser0.parse("playerFlag", playerFlag);
}

const char *friendserver::cli::RecentOppStruct::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("name", name);
	_jsonstr.compose("imageId", imageId);
	_jsonstr.compose("isOnline", isOnline);
	_jsonstr.compose("frdRelation", frdRelation);
	_jsonstr.compose("playerFlag", playerFlag);
	return _buf.c_str();
}

void friendserver::cli::RecentOppStruct::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("name", name);
	_jparser.parseByNameThrow("imageId", imageId);
	_jparser.parseByNameThrow("isOnline", isOnline);
	_jparser.parseByNameThrow("frdRelation", frdRelation);
	_jparser.parseByNameThrow("playerFlag", playerFlag);
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::clear()
{
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::equals(const Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST& _o) const
{
	return true;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_RECENT_PLAYED_LIST != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_RECENT_PLAYED_LIST).append(")");
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_RECENT_PLAYED_LIST).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::parseMsg(CommMsgParser& _parser)
{
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	recentPlayedUsers.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::equals(const Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		recentPlayedUsers.equals(_o.recentPlayedUsers);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("recentPlayedUsers=");
		recentPlayedUsers.toTraceString(_buf);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		recentPlayedUsers.toXmlString("recentPlayedUsers", _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("recentPlayedUsers"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< RecentOppStruct, 4 > >::FromXmlString(_value, recentPlayedUsers)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		recentPlayedUsers.composeMsg(_msg, _ignoreJSON);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		recentPlayedUsers.parseMsg(_parser);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("recentPlayedUsers", recentPlayedUsers);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("recentPlayedUsers", recentPlayedUsers);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::clear()
{
	token.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::equals(const Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN& _o) const
{
	return token.equals(_o.token);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_USE_FRD_SHARE_TOKEN != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_USE_FRD_SHARE_TOKEN).append(")");
	_buf.append(',');
	_buf.append("token=");
	_buf.append(token);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_USE_FRD_SHARE_TOKEN).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("token", token, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("token"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, token)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(token);
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(token);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("token", token);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("token", token);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	tokenUserId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::equals(const Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		tokenUserId.equals(_o.tokenUserId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("tokenUserId=");
		_buf.append(tokenUserId);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("tokenUserId", tokenUserId, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("tokenUserId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, tokenUserId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeString(tokenUserId);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseStringP(tokenUserId);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("tokenUserId", tokenUserId);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("tokenUserId", tokenUserId);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::clear()
{
	friendUpdateType = 0;
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::equals(const Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE& _o) const
{
	return friendUpdateType == _o.friendUpdateType &&
		userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE).append(")");
	_buf.append(',');
	_buf.append("friendUpdateType=");
	_buf.appendUint(friendUpdateType);
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("friendUpdateType", friendUpdateType, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("friendUpdateType"))
			{
				friendUpdateType = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeUINT32(friendUpdateType);
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(friendUpdateType);
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("friendUpdateType", friendUpdateType);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("friendUpdateType", friendUpdateType);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE
//=================================================================

friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::clear()
{
	addOrDrop = false;
	userId.clear();
}

bool friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::equals(const Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE& _o) const
{
	return addOrDrop == _o.addOrDrop &&
		userId.equals(_o.userId);
}

bool friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE).append(")");
	_buf.append(',');
	_buf.append("addOrDrop=");
	_buf.appendUint(addOrDrop);
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("addOrDrop", addOrDrop, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("addOrDrop"))
			{
				addOrDrop = (*_value.ptr() == '1');
			}
			else if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeBOOL(addOrDrop);
	_msg.composeString(userId);
}

void friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseBOOL(addOrDrop);
	_parser.parseStringP(userId);
}

const char *friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("addOrDrop", addOrDrop);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("addOrDrop", addOrDrop);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_SHARE_URL
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::Protocol_MSG_FRDSRV_GET_SHARE_URL()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::clear()
{
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::equals(const Protocol_MSG_FRDSRV_GET_SHARE_URL& _o) const
{
	return true;
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_SHARE_URL != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_SHARE_URL*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_SHARE_URL).append(")");
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_SHARE_URL).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::parseMsg(CommMsgParser& _parser)
{
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY
//=================================================================

friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY()
{
	clear();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	url.clear();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::equals(const Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		url.equals(_o.url);
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_GET_SHARE_URL_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY*)_other));
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_GET_SHARE_URL_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("url=");
		_buf.append(url);
	}
	return _buf.c_str();
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_GET_SHARE_URL_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("url", url, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("url"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, url)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeString(url);
	}
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseStringP(url);
	}
}

const char *friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("url", url);
	}
	return _buf.c_str();
}

void friendserver::cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("url", url);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                    UserId_UserIntId
//=================================================================

friendserver::rodbm::UserId_UserIntId::UserId_UserIntId()
{
	clear();
}

void friendserver::rodbm::UserId_UserIntId::clear()
{
	userId.clear();
	userIntId = 0;
}

bool friendserver::rodbm::UserId_UserIntId::equals(const UserId_UserIntId& _o) const
{
	return userId.equals(_o.userId) &&
		userIntId == _o.userIntId;
}

const char *friendserver::rodbm::UserId_UserIntId::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("userId=");
	_buf.append(userId);
	_buf.append(',');
	_buf.append("userIntId=");
	_buf.appendUint(userIntId);
	_buf.append('}');
	return _buf.c_str();
}

const char *friendserver::rodbm::UserId_UserIntId::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId", userIntId, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool friendserver::rodbm::UserId_UserIntId::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
	{
		if (_element.equals("userId"))
		{
			if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
		}
		else if (_element.equals("userIntId"))
		{
			userIntId = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void friendserver::rodbm::UserId_UserIntId::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
	_msg.composeUINT32(userIntId);
}

void friendserver::rodbm::UserId_UserIntId::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
	_parser.parseUINT32(userIntId);
}

void friendserver::rodbm::UserId_UserIntId::parseMsg(Atf::MsgParserWithJSON& _parser)
{
	_parser.parse("userId", userId);
	_parser.parse("userIntId", userIntId);
}

const char *friendserver::rodbm::UserId_UserIntId::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	_jsonstr.compose("userIntId", userIntId);
	return _buf.c_str();
}

void friendserver::rodbm::UserId_UserIntId::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	_jparser.parseByNameThrow("userIntId", userIntId);
}

//=================================================================
//                Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID
//=================================================================

friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID()
{
	clear();
}

void friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::clear()
{
	userId.clear();
}

bool friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::equals(const Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_GET_USERINTID_BY_NORMALIZED_ID != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID*)_other));
}

const char *friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_GET_USERINTID_BY_NORMALIZED_ID).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_GET_USERINTID_BY_NORMALIZED_ID).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID
//=================================================================

friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID()
{
	clear();
}

void friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::clear()
{
	errCode = 0;
	errMsg.clear();
	userIntId = 0;
	originalUserId.clear();
}

bool friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::equals(const Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		userIntId == _o.userIntId &&
		originalUserId.equals(_o.originalUserId);
}

bool friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_GET_USERINTID_BY_NORMALIZED_ID != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID*)_other));
}

const char *friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_GET_USERINTID_BY_NORMALIZED_ID).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("userIntId=");
		_buf.appendUint(userIntId);
		_buf.append(',');
		_buf.append("originalUserId=");
		_buf.append(originalUserId);
	}
	return _buf.c_str();
}

const char *friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_GET_USERINTID_BY_NORMALIZED_ID).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("userIntId", userIntId, _buf);
		Atf::XmlElement::encodeAsXmlElement("originalUserId", originalUserId, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("userIntId"))
			{
				userIntId = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("originalUserId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, originalUserId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeUINT32(userIntId);
		_msg.composeString(originalUserId);
	}
}

void friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseUINT32(userIntId);
		_parser.parseStringP(originalUserId);
	}
}

const char *friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("userIntId", userIntId);
		_jsonstr.compose("originalUserId", originalUserId);
	}
	return _buf.c_str();
}

void friendserver::rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("userIntId", userIntId);
		_jparser.parseByNameThrow("originalUserId", originalUserId);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                    FriendStruct
//=================================================================

friendserver::frienddbm::FriendStruct::FriendStruct()
{
	clear();
}

void friendserver::frienddbm::FriendStruct::clear()
{
	name.clear();
	hasNewFriendPendingAction = false;
}

bool friendserver::frienddbm::FriendStruct::equals(const FriendStruct& _o) const
{
	return name.equals(_o.name) &&
		hasNewFriendPendingAction == _o.hasNewFriendPendingAction;
}

const char *friendserver::frienddbm::FriendStruct::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("name=");
	_buf.append(name);
	_buf.append(',');
	_buf.append("hasNewFriendPendingAction=");
	_buf.appendUint(hasNewFriendPendingAction);
	_buf.append('}');
	return _buf.c_str();
}

const char *friendserver::frienddbm::FriendStruct::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("name", name, _buf);
	Atf::XmlElement::encodeAsXmlElement("hasNewFriendPendingAction", hasNewFriendPendingAction, _buf);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool friendserver::frienddbm::FriendStruct::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
	{
		if (_element.equals("name"))
		{
			if(!Atf::XmlElement::movePBlockToPString(_value, name)) return false;
		}
		else if (_element.equals("hasNewFriendPendingAction"))
		{
			hasNewFriendPendingAction = (*_value.ptr() == '1');
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void friendserver::frienddbm::FriendStruct::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(name);
	_msg.composeBOOL(hasNewFriendPendingAction);
}

void friendserver::frienddbm::FriendStruct::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(name);
	_parser.parseBOOL(hasNewFriendPendingAction);
}

void friendserver::frienddbm::FriendStruct::parseMsg(Atf::MsgParserWithJSON& _parser)
{
	_parser.parse("name", name);
	_parser.parse("hasNewFriendPendingAction", hasNewFriendPendingAction);
}

const char *friendserver::frienddbm::FriendStruct::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("name", name);
	_jsonstr.compose("hasNewFriendPendingAction", hasNewFriendPendingAction);
	return _buf.c_str();
}

void friendserver::frienddbm::FriendStruct::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("name", name);
	_jparser.parseByNameThrow("hasNewFriendPendingAction", hasNewFriendPendingAction);
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::clear()
{
	userIntId = 0;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::equals(const Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA& _o) const
{
	return userIntId == _o.userIntId;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_LOAD_USER_DATA != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_LOAD_USER_DATA).append(")");
	_buf.append(',');
	_buf.append("userIntId=");
	_buf.appendUint(userIntId);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_LOAD_USER_DATA).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userIntId", userIntId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userIntId"))
			{
				userIntId = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeUINT32(userIntId);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(userIntId);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userIntId", userIntId);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userIntId", userIntId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_LOAD_USER_DATA
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::clear()
{
	errCode = 0;
	errMsg.clear();
	friendUsers.clear();
	friendReqUserIds.clear();
	blockedUserIds.clear();
	flags = 0;
	hasScore = false;
	score = 0;
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::equals(const Protocol_DBM_A_FRDSRV_LOAD_USER_DATA& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		friendUsers.equals(_o.friendUsers) &&
		friendReqUserIds.equals(_o.friendReqUserIds) &&
		blockedUserIds.equals(_o.blockedUserIds) &&
		flags == _o.flags &&
		hasScore == _o.hasScore &&
		score == _o.score;
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_LOAD_USER_DATA != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_LOAD_USER_DATA*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_LOAD_USER_DATA).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("friendUsers=");
		friendUsers.toTraceString(_buf);
		_buf.append(',');
		_buf.append("friendReqUserIds=");
		friendReqUserIds.toTraceString(_buf);
		_buf.append(',');
		_buf.append("blockedUserIds=");
		blockedUserIds.toTraceString(_buf);
		_buf.append(',');
		_buf.append("flags=");
		_buf.appendUint(flags);
		_buf.append(',');
		_buf.append("hasScore=");
		_buf.appendUint(hasScore);
		_buf.append(',');
		_buf.append("score=");
		_buf.appendUint(score);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_LOAD_USER_DATA).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		friendUsers.toXmlString("friendUsers", _buf);
		friendReqUserIds.toXmlString("friendReqUserIds", _buf);
		blockedUserIds.toXmlString("blockedUserIds", _buf);
		Atf::XmlElement::encodeAsXmlElement("flags", flags, _buf);
		Atf::XmlElement::encodeAsXmlElement("hasScore", hasScore, _buf);
		Atf::XmlElement::encodeAsXmlElement("score", score, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("friendUsers"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< FriendStruct, 4 > >::FromXmlString(_value, friendUsers)) return false;
			}
			else if (_element.equals("friendReqUserIds"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< PString, 4 > >::FromXmlString(_value, friendReqUserIds)) return false;
			}
			else if (_element.equals("blockedUserIds"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< PString, 4 > >::FromXmlString(_value, blockedUserIds)) return false;
			}
			else if (_element.equals("flags"))
			{
				flags = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("hasScore"))
			{
				hasScore = (*_value.ptr() == '1');
			}
			else if (_element.equals("score"))
			{
				score = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		friendUsers.composeMsg(_msg, _ignoreJSON);
		friendReqUserIds.composeMsg(_msg, _ignoreJSON);
		blockedUserIds.composeMsg(_msg, _ignoreJSON);
		_msg.composeUINT32(flags);
		_msg.composeBOOL(hasScore);
		_msg.composeUINT32(score);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		friendUsers.parseMsg(_parser);
		friendReqUserIds.parseMsg(_parser);
		blockedUserIds.parseMsg(_parser);
		_parser.parseUINT32(flags);
		_parser.parseBOOL(hasScore);
		_parser.parseUINT32(score);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("friendUsers", friendUsers);
		_jsonstr.compose("friendReqUserIds", friendReqUserIds);
		_jsonstr.compose("blockedUserIds", blockedUserIds);
		_jsonstr.compose("flags", flags);
		_jsonstr.compose("hasScore", hasScore);
		_jsonstr.compose("score", score);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("friendUsers", friendUsers);
		_jparser.parseByNameThrow("friendReqUserIds", friendReqUserIds);
		_jparser.parseByNameThrow("blockedUserIds", blockedUserIds);
		_jparser.parseByNameThrow("flags", flags);
		_jparser.parseByNameThrow("hasScore", hasScore);
		_jparser.parseByNameThrow("score", score);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::clear()
{
	userIntId1 = 0;
	userId2.clear();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::equals(const Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND& _o) const
{
	return userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2);
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_REMOVE_FRIEND != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_REMOVE_FRIEND).append(")");
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_REMOVE_FRIEND).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_REMOVE_FRIEND
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::equals(const Protocol_DBM_A_FRDSRV_REMOVE_FRIEND& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_REMOVE_FRIEND != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_REMOVE_FRIEND*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_REMOVE_FRIEND).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_REMOVE_FRIEND).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_UNBLOCK_USER
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::clear()
{
	userIntId1 = 0;
	userId2.clear();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::equals(const Protocol_DBM_Q_FRDSRV_UNBLOCK_USER& _o) const
{
	return userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2);
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_UNBLOCK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_UNBLOCK_USER*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_UNBLOCK_USER).append(")");
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_UNBLOCK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_UNBLOCK_USER
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::Protocol_DBM_A_FRDSRV_UNBLOCK_USER()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::equals(const Protocol_DBM_A_FRDSRV_UNBLOCK_USER& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_UNBLOCK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_UNBLOCK_USER*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_UNBLOCK_USER).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_UNBLOCK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::clear()
{
	userId1.clear();
	userIntId1 = 0;
	userId2.clear();
	userIntId2 = 0;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::equals(const Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ& _o) const
{
	return userId1.equals(_o.userId1) &&
		userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2) &&
		userIntId2 == _o.userIntId2;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_ADD_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_ADD_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	_buf.append(',');
	_buf.append("userIntId2=");
	_buf.appendUint(userIntId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_ADD_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId2", userIntId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else if (_element.equals("userIntId2"))
			{
				userIntId2 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
	_msg.composeUINT32(userIntId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
	_parser.parseUINT32(userIntId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	_jsonstr.compose("userIntId2", userIntId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	_jparser.parseByNameThrow("userIntId2", userIntId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::equals(const Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_ADD_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_ADD_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_ADD_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::clear()
{
	userId1.clear();
	userIntId1 = 0;
	userId2.clear();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::equals(const Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ& _o) const
{
	return userId1.equals(_o.userId1) &&
		userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2);
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_DENY_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_DENY_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_DENY_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::equals(const Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_DENY_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_DENY_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_DENY_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::clear()
{
	userId1.clear();
	userIntId1 = 0;
	userId2.clear();
	userIntId2 = 0;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::equals(const Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ& _o) const
{
	return userId1.equals(_o.userId1) &&
		userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2) &&
		userIntId2 == _o.userIntId2;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	_buf.append(',');
	_buf.append("userIntId2=");
	_buf.appendUint(userIntId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId2", userIntId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else if (_element.equals("userIntId2"))
			{
				userIntId2 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
	_msg.composeUINT32(userIntId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
	_parser.parseUINT32(userIntId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	_jsonstr.compose("userIntId2", userIntId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	_jparser.parseByNameThrow("userIntId2", userIntId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::equals(const Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_ACCEPT_FRIEND_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_ACCEPT_FRIEND_REQ).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_ACCEPT_FRIEND_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_BLOCK_USER
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::Protocol_DBM_Q_FRDSRV_BLOCK_USER()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::clear()
{
	userId1.clear();
	userIntId1 = 0;
	userId2.clear();
	userIntId2 = 0;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::equals(const Protocol_DBM_Q_FRDSRV_BLOCK_USER& _o) const
{
	return userId1.equals(_o.userId1) &&
		userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2) &&
		userIntId2 == _o.userIntId2;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_BLOCK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_BLOCK_USER*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_BLOCK_USER).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	_buf.append(',');
	_buf.append("userIntId2=");
	_buf.appendUint(userIntId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_BLOCK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId2", userIntId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else if (_element.equals("userIntId2"))
			{
				userIntId2 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
	_msg.composeUINT32(userIntId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
	_parser.parseUINT32(userIntId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	_jsonstr.compose("userIntId2", userIntId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	_jparser.parseByNameThrow("userIntId2", userIntId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_BLOCK_USER
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::Protocol_DBM_A_FRDSRV_BLOCK_USER()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::equals(const Protocol_DBM_A_FRDSRV_BLOCK_USER& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_BLOCK_USER != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_BLOCK_USER*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_BLOCK_USER).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_BLOCK_USER).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::clear()
{
	userId.clear();
	userIntId = 0;
	updateType = 0;
	score = 0;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::equals(const Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE& _o) const
{
	return userId.equals(_o.userId) &&
		userIntId == _o.userIntId &&
		updateType == _o.updateType &&
		score == _o.score;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	_buf.append(',');
	_buf.append("userIntId=");
	_buf.appendUint(userIntId);
	_buf.append(',');
	_buf.append("updateType=");
	_buf.appendUint(updateType);
	_buf.append(',');
	_buf.append("score=");
	_buf.appendInt(score);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId", userIntId, _buf);
	Atf::XmlElement::encodeAsXmlElement("updateType", updateType, _buf);
	Atf::XmlElement::encodeAsXmlElement("score", score, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else if (_element.equals("userIntId"))
			{
				userIntId = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("updateType"))
			{
				updateType = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("score"))
			{
				score = (INT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
	_msg.composeUINT32(userIntId);
	_msg.composeUINT32(updateType);
	_msg.composeINT32(score);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
	_parser.parseUINT32(userIntId);
	_parser.parseUINT32(updateType);
	_parser.parseINT32(score);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	_jsonstr.compose("userIntId", userIntId);
	_jsonstr.compose("updateType", updateType);
	_jsonstr.compose("score", score);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	_jparser.parseByNameThrow("userIntId", userIntId);
	_jparser.parseByNameThrow("updateType", updateType);
	_jparser.parseByNameThrow("score", score);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::equals(const Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_SET_USER_FRIEND_SCORE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_SET_USER_FRIEND_SCORE).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_SET_USER_FRIEND_SCORE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::clear()
{
	userId.clear();
	userIntId = 0;
	setNotRemove = false;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::equals(const Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ& _o) const
{
	return userId.equals(_o.userId) &&
		userIntId == _o.userIntId &&
		setNotRemove == _o.setNotRemove;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	_buf.append(',');
	_buf.append("userIntId=");
	_buf.appendUint(userIntId);
	_buf.append(',');
	_buf.append("setNotRemove=");
	_buf.appendUint(setNotRemove);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId", userIntId, _buf);
	Atf::XmlElement::encodeAsXmlElement("setNotRemove", setNotRemove, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else if (_element.equals("userIntId"))
			{
				userIntId = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("setNotRemove"))
			{
				setNotRemove = (*_value.ptr() == '1');
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
	_msg.composeUINT32(userIntId);
	_msg.composeBOOL(setNotRemove);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
	_parser.parseUINT32(userIntId);
	_parser.parseBOOL(setNotRemove);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	_jsonstr.compose("userIntId", userIntId);
	_jsonstr.compose("setNotRemove", setNotRemove);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	_jparser.parseByNameThrow("userIntId", userIntId);
	_jparser.parseByNameThrow("setNotRemove", setNotRemove);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::equals(const Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::clear()
{
	userId1.clear();
	userIntId1 = 0;
	userIds.clear();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::equals(const Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION& _o) const
{
	return userId1.equals(_o.userId1) &&
		userIntId1 == _o.userIntId1 &&
		userIds.equals(_o.userIds);
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userIds=");
	userIds.toTraceString(_buf);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	userIds.toXmlString("userIds", _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userIds"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< PString, 4 > >::FromXmlString(_value, userIds)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeUINT32(userIntId1);
	userIds.composeMsg(_msg, _ignoreJSON);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseUINT32(userIntId1);
	userIds.parseMsg(_parser);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userIds", userIds);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userIds", userIds);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::equals(const Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::clear()
{
	userId_sender.clear();
	userIntId_sender = 0;
	userIds_receiver.clear();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::equals(const Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO& _o) const
{
	return userId_sender.equals(_o.userId_sender) &&
		userIntId_sender == _o.userIntId_sender &&
		userIds_receiver.equals(_o.userIds_receiver);
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_GET_FRDREQ_INFO != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_GET_FRDREQ_INFO).append(")");
	_buf.append(',');
	_buf.append("userId_sender=");
	_buf.append(userId_sender);
	_buf.append(',');
	_buf.append("userIntId_sender=");
	_buf.appendUint(userIntId_sender);
	_buf.append(',');
	_buf.append("userIds_receiver=");
	userIds_receiver.toTraceString(_buf);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_GET_FRDREQ_INFO).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId_sender", userId_sender, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId_sender", userIntId_sender, _buf);
	userIds_receiver.toXmlString("userIds_receiver", _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId_sender"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId_sender)) return false;
			}
			else if (_element.equals("userIntId_sender"))
			{
				userIntId_sender = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userIds_receiver"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< PString, 4 > >::FromXmlString(_value, userIds_receiver)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId_sender);
	_msg.composeUINT32(userIntId_sender);
	userIds_receiver.composeMsg(_msg, _ignoreJSON);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId_sender);
	_parser.parseUINT32(userIntId_sender);
	userIds_receiver.parseMsg(_parser);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId_sender", userId_sender);
	_jsonstr.compose("userIntId_sender", userIntId_sender);
	_jsonstr.compose("userIds_receiver", userIds_receiver);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId_sender", userId_sender);
	_jparser.parseByNameThrow("userIntId_sender", userIntId_sender);
	_jparser.parseByNameThrow("userIds_receiver", userIds_receiver);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::clear()
{
	errCode = 0;
	errMsg.clear();
	vecReqSent.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::equals(const Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		vecReqSent.equals(_o.vecReqSent);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_GET_FRDREQ_INFO != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_GET_FRDREQ_INFO).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("vecReqSent=");
		vecReqSent.toTraceString(_buf);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_GET_FRDREQ_INFO).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		vecReqSent.toXmlString("vecReqSent", _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("vecReqSent"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< UINT32, 4 > >::FromXmlString(_value, vecReqSent)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		vecReqSent.composeMsg(_msg, _ignoreJSON);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		vecReqSent.parseMsg(_parser);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("vecReqSent", vecReqSent);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("vecReqSent", vecReqSent);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP
//=================================================================

friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::clear()
{
	userId1.clear();
	userIntId1 = 0;
	userId2.clear();
	userIntId2 = 0;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::equals(const Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP& _o) const
{
	return userId1.equals(_o.userId1) &&
		userIntId1 == _o.userIntId1 &&
		userId2.equals(_o.userId2) &&
		userIntId2 == _o.userIntId2;
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userIntId1=");
	_buf.appendUint(userIntId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	_buf.append(',');
	_buf.append("userIntId2=");
	_buf.appendUint(userIntId2);
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId1", userIntId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId2", userIntId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userIntId1"))
			{
				userIntId1 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else if (_element.equals("userIntId2"))
			{
				userIntId2 = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeUINT32(userIntId1);
	_msg.composeString(userId2);
	_msg.composeUINT32(userIntId2);
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseUINT32(userIntId1);
	_parser.parseStringP(userId2);
	_parser.parseUINT32(userIntId2);
}

const char *friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userIntId1", userIntId1);
	_jsonstr.compose("userId2", userId2);
	_jsonstr.compose("userIntId2", userIntId2);
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userIntId1", userIntId1);
	_jparser.parseByNameThrow("userId2", userId2);
	_jparser.parseByNameThrow("userIntId2", userIntId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP
//=================================================================

friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP()
{
	clear();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::equals(const Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP*)_other));
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if(errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if(errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if(errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if(errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS
//=================================================================

friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS()
{
	clear();
}

void friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::clear()
{
	userId.clear();
	userIntId = 0;
	maxOpponents = 0;
}

bool friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::equals(const Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS& _o) const
{
	return userId.equals(_o.userId) &&
		userIntId == _o.userIntId &&
		maxOpponents == _o.maxOpponents;
}

bool friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_Q_PDUEL_GET_RECENT_OPPONENTS != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS*)_other));
}

const char *friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_Q_PDUEL_GET_RECENT_OPPONENTS).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	_buf.append(',');
	_buf.append("userIntId=");
	_buf.appendUint(userIntId);
	_buf.append(',');
	_buf.append("maxOpponents=");
	_buf.appendInt(maxOpponents);
	return _buf.c_str();
}

const char *friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_Q_PDUEL_GET_RECENT_OPPONENTS).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	Atf::XmlElement::encodeAsXmlElement("userIntId", userIntId, _buf);
	Atf::XmlElement::encodeAsXmlElement("maxOpponents", maxOpponents, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else if (_element.equals("userIntId"))
			{
				userIntId = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("maxOpponents"))
			{
				maxOpponents = (INT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
	_msg.composeUINT32(userIntId);
	_msg.composeINT32(maxOpponents);
}

void friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
	_parser.parseUINT32(userIntId);
	_parser.parseINT32(maxOpponents);
}

const char *friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	_jsonstr.compose("userIntId", userIntId);
	_jsonstr.compose("maxOpponents", maxOpponents);
	return _buf.c_str();
}

void friendserver::olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	_jparser.parseByNameThrow("userIntId", userIntId);
	_jparser.parseByNameThrow("maxOpponents", maxOpponents);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS
//=================================================================

friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS()
{
	clear();
}

void friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::clear()
{
	errCode = 0;
	errMsg.clear();
	recentPlayedUsers.clear();
}

bool friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::equals(const Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		recentPlayedUsers.equals(_o.recentPlayedUsers);
}

bool friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::equals(Atf::MessageProtocol* _other) const
{
	if (DBM_A_PDUEL_GET_RECENT_OPPONENTS != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS*)_other));
}

const char *friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(DBM_A_PDUEL_GET_RECENT_OPPONENTS).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("recentPlayedUsers=");
		recentPlayedUsers.toTraceString(_buf);
	}
	return _buf.c_str();
}

const char *friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(DBM_A_PDUEL_GET_RECENT_OPPONENTS).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		recentPlayedUsers.toXmlString("recentPlayedUsers", _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("recentPlayedUsers"))
			{
				if(!Atf::AtfTempl< Atf::LAtfVector< PString, 4 > >::FromXmlString(_value, recentPlayedUsers)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		recentPlayedUsers.composeMsg(_msg, _ignoreJSON);
	}
}

void friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		recentPlayedUsers.parseMsg(_parser);
	}
}

const char *friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("recentPlayedUsers", recentPlayedUsers);
	}
	return _buf.c_str();
}

void friendserver::olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("recentPlayedUsers", recentPlayedUsers);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::clear()
{
	userId1.clear();
	userId2.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::equals(const Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP& _o) const
{
	return userId1.equals(_o.userId1) &&
		userId2.equals(_o.userId2);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeString(userId2);
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	isFriend = false;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::equals(const Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		isFriend == _o.isFriend;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("isFriend=");
		_buf.appendUint(isFriend);
	}
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("isFriend", isFriend, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("isFriend"))
			{
				isFriend = (*_value.ptr() == '1');
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeBOOL(isFriend);
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseBOOL(isFriend);
	}
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("isFriend", isFriend);
	}
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("isFriend", isFriend);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::clear()
{
	userId.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE& _o) const
{
	return userId.equals(_o.userId);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE).append(")");
	_buf.append(',');
	_buf.append("userId=");
	_buf.append(userId);
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId", userId, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId);
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId", userId);
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId", userId);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	numFriends = 0;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		numFriends == _o.numFriends;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("numFriends=");
		_buf.appendInt(numFriends);
	}
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("numFriends", numFriends, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("numFriends"))
			{
				numFriends = (INT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeINT32(numFriends);
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseINT32(numFriends);
	}
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("numFriends", numFriends);
	}
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("numFriends", numFriends);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::clear()
{
	userId1.clear();
	userId2.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE& _o) const
{
	return userId1.equals(_o.userId1) &&
		userId2.equals(_o.userId2);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeString(userId2);
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
	numFriends = 0;
	isOnline = false;
	frdRelation = 0;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::equals(const Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg) &&
		numFriends == _o.numFriends &&
		isOnline == _o.isOnline &&
		frdRelation == _o.frdRelation;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	else
	{
		_buf.append(',');
		_buf.append("numFriends=");
		_buf.appendUint(numFriends);
		_buf.append(',');
		_buf.append("isOnline=");
		_buf.appendUint(isOnline);
		_buf.append(',');
		_buf.append("frdRelation=");
		_buf.appendUint(frdRelation);
	}
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	else
	{
		Atf::XmlElement::encodeAsXmlElement("numFriends", numFriends, _buf);
		Atf::XmlElement::encodeAsXmlElement("isOnline", isOnline, _buf);
		Atf::XmlElement::encodeAsXmlElement("frdRelation", frdRelation, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else if (_element.equals("numFriends"))
			{
				numFriends = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("isOnline"))
			{
				isOnline = (*_value.ptr() == '1');
			}
			else if (_element.equals("frdRelation"))
			{
				frdRelation = (UINT32)p_atoi64((const char *)_value.ptr());
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
	else
	{
		_msg.composeUINT32(numFriends);
		_msg.composeBOOL(isOnline);
		_msg.composeUINT32(frdRelation);
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
	else
	{
		_parser.parseUINT32(numFriends);
		_parser.parseBOOL(isOnline);
		_parser.parseUINT32(frdRelation);
	}
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	else
	{
		_jsonstr.compose("numFriends", numFriends);
		_jsonstr.compose("isOnline", isOnline);
		_jsonstr.compose("frdRelation", frdRelation);
	}
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	else
	{
		_jparser.parseByNameThrow("numFriends", numFriends);
		_jparser.parseByNameThrow("isOnline", isOnline);
		_jparser.parseByNameThrow("frdRelation", frdRelation);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::clear()
{
	userId1.clear();
	userId2.clear();
	addOrRemove = false;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::equals(const Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE& _o) const
{
	return userId1.equals(_o.userId1) &&
		userId2.equals(_o.userId2) &&
		addOrRemove == _o.addOrRemove;
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	_buf.append(',');
	_buf.append("addOrRemove=");
	_buf.appendUint(addOrRemove);
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	Atf::XmlElement::encodeAsXmlElement("addOrRemove", addOrRemove, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else if (_element.equals("addOrRemove"))
			{
				addOrRemove = (*_value.ptr() == '1');
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeString(userId2);
	_msg.composeBOOL(addOrRemove);
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseStringP(userId2);
	_parser.parseBOOL(addOrRemove);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userId2", userId2);
	_jsonstr.compose("addOrRemove", addOrRemove);
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userId2", userId2);
	_jparser.parseByNameThrow("addOrRemove", addOrRemove);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::clear()
{
	userId1.clear();
	userId2.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED& _o) const
{
	return userId1.equals(_o.userId1) &&
		userId2.equals(_o.userId2);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeString(userId2);
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::clear()
{
	userId1.clear();
	userId2.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT& _o) const
{
	return userId1.equals(_o.userId1) &&
		userId2.equals(_o.userId2);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_CHALLENGE_SENT != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_SENT).append(")");
	_buf.append(',');
	_buf.append("userId1=");
	_buf.append(userId1);
	_buf.append(',');
	_buf.append("userId2=");
	_buf.append(userId2);
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_SENT).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("userId1", userId1, _buf);
	Atf::XmlElement::encodeAsXmlElement("userId2", userId2, _buf);
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("userId1"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId1)) return false;
			}
			else if (_element.equals("userId2"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, userId2)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeString(userId1);
	_msg.composeString(userId2);
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(userId1);
	_parser.parseStringP(userId2);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("userId1", userId1);
	_jsonstr.compose("userId2", userId2);
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("userId1", userId1);
	_jparser.parseByNameThrow("userId2", userId2);
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY
//=================================================================

friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY()
{
	clear();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::clear()
{
	errCode = 0;
	errMsg.clear();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::equals(const Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errMsg.equals(_o.errMsg);
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::equals(Atf::MessageProtocol* _other) const
{
	if (MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY != _other->getMsgId() || strcmp(getMsgIdString(), _other->getMsgIdString()))
	{
		return false;
	}
   
 	return equals(*((Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY*)_other));
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if (errCode != 0)
	{
		_buf.append(',');
		_buf.append("errMsg=");
		_buf.append(errMsg);
	}
	return _buf.c_str();
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::asXmlString(PString& _buf, bool _logTimeStamp) const
{
	UINT64 _timeTicks64 = 0;
	UINT32 _msgTime = 0;
		
	if(_logTimeStamp)
	{
		INT32 _msec;
		UINT32 _sec = LONGSYSTEMTICKS(&_msec);
		_timeTicks64 = (UINT64)_sec * 1000 + _msec;
		_msgTime = (UINT32)plib_time(0);
	}
		
	return asXmlString(_buf, _timeTicks64, _msgTime);
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::asXmlString(PString& _buf, UINT64 _timestampTicks, UINT32 _msgTime) const
{	
	_buf.cut(0);
	_buf.append("<message>");
	_buf.append("<msgid name='").append(getMsgIdString()).append("'>").appendUint(MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY).append("</msgid>");
	if(_timestampTicks)
		_buf.append("<ticks>").appendUint64(_timestampTicks).append("</ticks>");
	if(_msgTime)	
		_buf.append("<mtime>").appendUint(_msgTime).append("</mtime>");
	_buf.append("<struct>");
	Atf::XmlElement::encodeAsXmlElement("errCode", errCode, _buf);
	if (errCode != 0)
	{
		Atf::XmlElement::encodeAsXmlElement("errMsg", errMsg, _buf);
	}
	_buf.append("</struct>");
	_buf.append("</message>");
	return _buf.c_str();
}

bool friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::loadFromXmlString(const PString& _xmlString)
{
	const char *_xml = _xmlString.c_str();
	PString _str;
   
	if (Atf::XmlElement::getElementValueAsString("/message/struct", _xml, _str))
	{
		_xml = _str.c_str();

		PString _element;
		PBlock _value;
		PString _type;     
		
        while (Atf::XmlElement::decodeNextXmlElement(_xml, _element, _value, _type))
        {
			if (_element.equals("errCode"))
			{
				errCode = (INT16)p_atoi64((const char *)_value.ptr());
			}
			else if (_element.equals("errMsg"))
			{
				if(!Atf::XmlElement::movePBlockToPString(_value, errMsg)) return false;
			}
			else
			{
				return false;  // unknown instance variable name
			}
		}

		Atf::XmlElement::skipSpaces(_xml);
		return (*_xml == 0);  // reach the end of string, nothing unknown!
	}
	else 
	{
		CommMsgBody _body;
		if (Atf::XmlElement::getLeafElementAsMsgBody("/message/body", _xml, _body))
		{
			parseMessage(_body);
			return true;
		}
		else
		{
			return false;
		}
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeINT16(errCode);
	if (errCode != 0)
	{
		_msg.composeString(errMsg);
	}
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if (errCode != 0)
	{
		_parser.parseStringP(errMsg);
	}
}

const char *friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::asJSONString(PString& _buf) const
{
	_buf.cut(0);
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("errCode", errCode);
	if (errCode != 0)
	{
		_jsonstr.compose("errMsg", errMsg);
	}
	return _buf.c_str();
}

void friendserver::dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY::loadFromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("errCode", errCode);
	if (errCode != 0)
	{
		_jparser.parseByNameThrow("errMsg", errMsg);
	}
	PASSERT(_jparser.parseEnded());
}

//=================================================================
//                ProtocolFactory
//=================================================================

Atf::MessageProtocol* friendserver::ProtocolFactory::createMessageProtocolObject(const char *_nameSpace, UINT32 _msgId)
{
	Atf::MessageProtocol *_obj = NULL;
	
	if (!strcmp(_nameSpace, 
#ifdef cli_STRING
		cli_STRING
#else
		"cli"
#endif
		))
	{
		switch(_msgId)
		{
			case MSG_FRDSRV_ACCEPT_FRIEND_REQ: _obj = new cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ(); break;
			case MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY(); break;
			case MSG_FRDSRV_ADD_FRIEND_REQ: _obj = new cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ(); break;
			case MSG_FRDSRV_ADD_FRIEND_REQ_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_ADD_FRIEND_REQ_REPLY(); break;
			case MSG_FRDSRV_BLOCK_USER: _obj = new cli::Protocol_MSG_FRDSRV_BLOCK_USER(); break;
			case MSG_FRDSRV_BLOCK_USER_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_BLOCK_USER_REPLY(); break;
			case MSG_FRDSRV_CHECK_USER: _obj = new cli::Protocol_MSG_FRDSRV_CHECK_USER(); break;
			case MSG_FRDSRV_CHECK_USER_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_CHECK_USER_REPLY(); break;
			case MSG_FRDSRV_DENY_FRIEND_REQ: _obj = new cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ(); break;
			case MSG_FRDSRV_DENY_FRIEND_REQ_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_DENY_FRIEND_REQ_REPLY(); break;
			case MSG_FRDSRV_GET_BLOCKED_LIST: _obj = new cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST(); break;
			case MSG_FRDSRV_GET_BLOCKED_LIST_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_GET_BLOCKED_LIST_REPLY(); break;
			case MSG_FRDSRV_GET_FRIEND_LIST: _obj = new cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST(); break;
			case MSG_FRDSRV_GET_FRIEND_LIST_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_GET_FRIEND_LIST_REPLY(); break;
			case MSG_FRDSRV_GET_FRIEND_REQ_LIST: _obj = new cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST(); break;
			case MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY(); break;
			case MSG_FRDSRV_GET_RECENT_PLAYED_LIST: _obj = new cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST(); break;
			case MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY(); break;
			case MSG_FRDSRV_GET_SHARE_URL: _obj = new cli::Protocol_MSG_FRDSRV_GET_SHARE_URL(); break;
			case MSG_FRDSRV_GET_SHARE_URL_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_GET_SHARE_URL_REPLY(); break;
			case MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE: _obj = new cli::Protocol_MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE(); break;
			case MSG_FRDSRV_REMOVE_FRIEND: _obj = new cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND(); break;
			case MSG_FRDSRV_REMOVE_FRIEND_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_REMOVE_FRIEND_REPLY(); break;
			case MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ: _obj = new cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ(); break;
			case MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY(); break;
			case MSG_FRDSRV_UNBLOCK_USER: _obj = new cli::Protocol_MSG_FRDSRV_UNBLOCK_USER(); break;
			case MSG_FRDSRV_UNBLOCK_USER_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_UNBLOCK_USER_REPLY(); break;
			case MSG_FRDSRV_USE_FRD_SHARE_TOKEN: _obj = new cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN(); break;
			case MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY: _obj = new cli::Protocol_MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY(); break;
			case MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE: _obj = new cli::Protocol_MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE(); break;
		}
	}
	else if (!strcmp(_nameSpace, 
#ifdef dlobby_STRING
		dlobby_STRING
#else
		"dlobby"
#endif
		))
	{
		switch(_msgId)
		{
			case MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED(); break;
			case MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY(); break;
			case MSG_FRDSRV_DLOBBY_CHALLENGE_SENT: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT(); break;
			case MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY(); break;
			case MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP(); break;
			case MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY(); break;
			case MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE(); break;
			case MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY(); break;
			case MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE(); break;
			case MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY(); break;
			case MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE: _obj = new dlobby::Protocol_MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE(); break;
		}
	}
	else if (!strcmp(_nameSpace, 
#ifdef frienddbm_STRING
		frienddbm_STRING
#else
		"frienddbm"
#endif
		))
	{
		switch(_msgId)
		{
			case DBM_A_FRDSRV_ACCEPT_FRIEND_REQ: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_ACCEPT_FRIEND_REQ(); break;
			case DBM_A_FRDSRV_ADD_FRIEND_REQ: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_ADD_FRIEND_REQ(); break;
			case DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP(); break;
			case DBM_A_FRDSRV_BLOCK_USER: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_BLOCK_USER(); break;
			case DBM_A_FRDSRV_DENY_FRIEND_REQ: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_DENY_FRIEND_REQ(); break;
			case DBM_A_FRDSRV_GET_FRDREQ_INFO: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_GET_FRDREQ_INFO(); break;
			case DBM_A_FRDSRV_LOAD_USER_DATA: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_LOAD_USER_DATA(); break;
			case DBM_A_FRDSRV_REMOVE_FRIEND: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_FRIEND(); break;
			case DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION(); break;
			case DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ(); break;
			case DBM_A_FRDSRV_SET_USER_FRIEND_SCORE: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_SET_USER_FRIEND_SCORE(); break;
			case DBM_A_FRDSRV_UNBLOCK_USER: _obj = new frienddbm::Protocol_DBM_A_FRDSRV_UNBLOCK_USER(); break;
			case DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ(); break;
			case DBM_Q_FRDSRV_ADD_FRIEND_REQ: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_ADD_FRIEND_REQ(); break;
			case DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP(); break;
			case DBM_Q_FRDSRV_BLOCK_USER: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_BLOCK_USER(); break;
			case DBM_Q_FRDSRV_DENY_FRIEND_REQ: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_DENY_FRIEND_REQ(); break;
			case DBM_Q_FRDSRV_GET_FRDREQ_INFO: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_GET_FRDREQ_INFO(); break;
			case DBM_Q_FRDSRV_LOAD_USER_DATA: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_LOAD_USER_DATA(); break;
			case DBM_Q_FRDSRV_REMOVE_FRIEND: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_FRIEND(); break;
			case DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION(); break;
			case DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ(); break;
			case DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE(); break;
			case DBM_Q_FRDSRV_UNBLOCK_USER: _obj = new frienddbm::Protocol_DBM_Q_FRDSRV_UNBLOCK_USER(); break;
		}
	}
	else if (!strcmp(_nameSpace, 
#ifdef olap_STRING
		olap_STRING
#else
		"olap"
#endif
		))
	{
		switch(_msgId)
		{
			case DBM_A_PDUEL_GET_RECENT_OPPONENTS: _obj = new olap::Protocol_DBM_A_PDUEL_GET_RECENT_OPPONENTS(); break;
			case DBM_Q_PDUEL_GET_RECENT_OPPONENTS: _obj = new olap::Protocol_DBM_Q_PDUEL_GET_RECENT_OPPONENTS(); break;
		}
	}
	else if (!strcmp(_nameSpace, 
#ifdef rodbm_STRING
		rodbm_STRING
#else
		"rodbm"
#endif
		))
	{
		switch(_msgId)
		{
			case DBM_A_GET_USERINTID_BY_NORMALIZED_ID: _obj = new rodbm::Protocol_DBM_A_GET_USERINTID_BY_NORMALIZED_ID(); break;
			case DBM_Q_GET_USERINTID_BY_NORMALIZED_ID: _obj = new rodbm::Protocol_DBM_Q_GET_USERINTID_BY_NORMALIZED_ID(); break;
		}
	}
	
	if (!_obj)
	{
		Atf::UnimplementedProtocol *_o = new Atf::UnimplementedProtocol();
		_o->msgId = _msgId; 
		_obj = _o; 
	}
	
	return _obj;
}

