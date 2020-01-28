/**
 * crossgamebuttonsserverprot_thin.cpp
 *
 * This file was auto-generated from crossgamebuttonsserverprot.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin crossgamebuttonsserverprot.txt
 */
 
#include "crossgamebuttonsserverprot_thin.h"

//=================================================================
//                Protocol_CRM_Q_GET_CROSSLINK_BUTTONS
//=================================================================

CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS()
{
	clear();
}

void CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::clear()
{
}

bool CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::equals(const Protocol_CRM_Q_GET_CROSSLINK_BUTTONS& _o) const
{
	return true;
}

const char *CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(CRM_Q_GET_CROSSLINK_BUTTONS).append(")");
	return _buf.c_str();
}

void CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::composeMsg(CommMsgBody& _msg) const
{
}

void CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::parseMsg(CommMsgParser& _parser)
{
}

/*static*/ void CrossGameLinkServer::Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "CRM_Q_GET_CROSSLINK_BUTTONS";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                Protocol_CRM_A_GET_CROSSLINK_BUTTONS
//=================================================================

CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::Protocol_CRM_A_GET_CROSSLINK_BUTTONS()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::Protocol_CRM_A_GET_CROSSLINK_BUTTONS(Protocol_CRM_A_GET_CROSSLINK_BUTTONS&& _o)
	: errCode(std::move(_o.errCode))
	, errStr(std::move(_o.errStr))
	, buttons(std::move(_o.buttons))
{
}

CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS& CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::operator=(Protocol_CRM_A_GET_CROSSLINK_BUTTONS&& _o)
{
	if(this != &_o)
	{
		errCode = std::move(_o.errCode);
		errStr = std::move(_o.errStr);
		buttons = std::move(_o.buttons);
	}
	return *this;
}

#endif

void CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::clear()
{
	errCode = 0;
	errStr.clear();
	buttons.clear();
}

bool CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::equals(const Protocol_CRM_A_GET_CROSSLINK_BUTTONS& _o) const
{
	return errCode == _o.errCode &&
		errStr.equals(_o.errStr) &&
		buttons.equals(_o.buttons);
}

const char *CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(CRM_A_GET_CROSSLINK_BUTTONS).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if( errCode )
	{
		_buf.append(',');
		_buf.append("errStr=");
		_buf.append(errStr);
	}
	else
	{
		_buf.append(',');
		_buf.append("buttons=");
		buttons.toTraceString(_buf);
	}
	return _buf.c_str();
}

void CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeINT16(errCode);
	if( errCode )
	{
		_msg.composeString(errStr);
	}
	else
	{
		buttons.composeMsg(_msg);
	}
}

void CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if( errCode )
	{
		_parser.parseStringP(errStr);
	}
	else
	{
		buttons.parseMsg(_parser);
	}
}

/*static*/ void CrossGameLinkServer::Protocol_CRM_A_GET_CROSSLINK_BUTTONS::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "CRM_A_GET_CROSSLINK_BUTTONS";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	const char *_dummy = "";
	INT16 errCode = 0;
	_parser.parseINT16(errCode);
	AtfValidator::validateInt(_descr, "errCode", errCode, _checker, __FILE__, __LINE__);
	if( errCode )
	{
		_parser.parseStringN(_dummy, 0, "errStr"); size_t szErrStr = strlen(_dummy);
		AtfValidator::validateInt(_descr, "errStr", szErrStr, _checker, __FILE__, __LINE__);
	}
	else
	{
		PString _descbuf;
		int szButtons = ThinAtf::LAtfVector< CrossGameLinkCommon::CrossGameLinkButtonData, 4 >::validateMsg(_parser, _checker, _descbuf.assign(_descr).append('.').append("buttons"), _fieldsWithUnparsedContent);
		AtfValidator::validateInt(_descr, "buttons", szButtons, _checker, __FILE__, __LINE__);
	}
	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE
//=================================================================

CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE(Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE&& _o)
	: adminIntId(std::move(_o.adminIntId))
	, buttons(std::move(_o.buttons))
{
}

CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE& CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::operator=(Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE&& _o)
{
	if(this != &_o)
	{
		adminIntId = std::move(_o.adminIntId);
		buttons = std::move(_o.buttons);
	}
	return *this;
}

#endif

void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::clear()
{
	adminIntId = 0;
	buttons.clear();
}

bool CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::equals(const Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE& _o) const
{
	return adminIntId == _o.adminIntId &&
		buttons.equals(_o.buttons);
}

const char *CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE).append(")");
	_buf.append(',');
	_buf.append("adminIntId=");
	_buf.appendUint(adminIntId);
	_buf.append(',');
	_buf.append("buttons=");
	buttons.toTraceString(_buf);
	return _buf.c_str();
}

void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeUINT32(adminIntId);
	buttons.composeMsg(_msg);
}

void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(adminIntId);
	buttons.parseMsg(_parser);
}

/*static*/ void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	UINT32 adminIntId; _parser.parseUINT32(adminIntId);
	AtfValidator::validateInt(_descr, "adminIntId", adminIntId, _checker, __FILE__, __LINE__);
	PString _descbuf;
	int szButtons = ThinAtf::LAtfVector< CrossGameLinkCommon::CrossGameLinkButtonData, 4 >::validateMsg(_parser, _checker, _descbuf.assign(_descr).append('.').append("buttons"), _fieldsWithUnparsedContent);
	AtfValidator::validateInt(_descr, "buttons", szButtons, _checker, __FILE__, __LINE__);
	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY
//=================================================================

CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY(Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY&& _o)
	: errCode(std::move(_o.errCode))
	, errStr(std::move(_o.errStr))
{
}

CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY& CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::operator=(Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY&& _o)
{
	if(this != &_o)
	{
		errCode = std::move(_o.errCode);
		errStr = std::move(_o.errStr);
	}
	return *this;
}

#endif

void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::clear()
{
	errCode = 0;
	errStr.clear();
}

bool CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::equals(const Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errStr.equals(_o.errStr);
}

const char *CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if( errCode )
	{
		_buf.append(',');
		_buf.append("errStr=");
		_buf.append(errStr);
	}
	return _buf.c_str();
}

void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeINT16(errCode);
	if( errCode )
	{
		_msg.composeString(errStr);
	}
}

void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if( errCode )
	{
		_parser.parseStringP(errStr);
	}
}

/*static*/ void CrossGameLinkServer::Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	const char *_dummy = "";
	INT16 errCode = 0;
	_parser.parseINT16(errCode);
	AtfValidator::validateInt(_descr, "errCode", errCode, _checker, __FILE__, __LINE__);
	if( errCode )
	{
		_parser.parseStringN(_dummy, 0, "errStr"); size_t szErrStr = strlen(_dummy);
		AtfValidator::validateInt(_descr, "errStr", szErrStr, _checker, __FILE__, __LINE__);
	}
	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS
//=================================================================

CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS(Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS&& _o)
	: adminIntId(std::move(_o.adminIntId))
	, buttons(std::move(_o.buttons))
{
}

CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS& CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::operator=(Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS&& _o)
{
	if(this != &_o)
	{
		adminIntId = std::move(_o.adminIntId);
		buttons = std::move(_o.buttons);
	}
	return *this;
}

#endif

void CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::clear()
{
	adminIntId = 0;
	buttons.clear();
}

bool CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::equals(const Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS& _o) const
{
	return adminIntId == _o.adminIntId &&
		buttons.equals(_o.buttons);
}

const char *CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(CRM_Q_CREATE_CROSSLINK_BUTTONS).append(")");
	_buf.append(',');
	_buf.append("adminIntId=");
	_buf.appendUint(adminIntId);
	_buf.append(',');
	_buf.append("buttons=");
	buttons.toTraceString(_buf);
	return _buf.c_str();
}

void CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeUINT32(adminIntId);
	buttons.composeMsg(_msg);
}

void CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(adminIntId);
	buttons.parseMsg(_parser);
}

/*static*/ void CrossGameLinkServer::Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "CRM_Q_CREATE_CROSSLINK_BUTTONS";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	UINT32 adminIntId; _parser.parseUINT32(adminIntId);
	AtfValidator::validateInt(_descr, "adminIntId", adminIntId, _checker, __FILE__, __LINE__);
	PString _descbuf;
	int szButtons = ThinAtf::LAtfVector< CrossGameLinkCommon::CrossGameLinkButtonData, 4 >::validateMsg(_parser, _checker, _descbuf.assign(_descr).append('.').append("buttons"), _fieldsWithUnparsedContent);
	AtfValidator::validateInt(_descr, "buttons", szButtons, _checker, __FILE__, __LINE__);
	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS
//=================================================================

CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS(Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS&& _o)
	: errCode(std::move(_o.errCode))
	, errStr(std::move(_o.errStr))
{
}

CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS& CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::operator=(Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS&& _o)
{
	if(this != &_o)
	{
		errCode = std::move(_o.errCode);
		errStr = std::move(_o.errStr);
	}
	return *this;
}

#endif

void CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::clear()
{
	errCode = 0;
	errStr.clear();
}

bool CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::equals(const Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS& _o) const
{
	return errCode == _o.errCode &&
		errStr.equals(_o.errStr);
}

const char *CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(CRM_A_CREATE_CROSSLINK_BUTTONS).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if( errCode )
	{
		_buf.append(',');
		_buf.append("errStr=");
		_buf.append(errStr);
	}
	return _buf.c_str();
}

void CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeINT16(errCode);
	if( errCode )
	{
		_msg.composeString(errStr);
	}
}

void CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if( errCode )
	{
		_parser.parseStringP(errStr);
	}
}

/*static*/ void CrossGameLinkServer::Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "CRM_A_CREATE_CROSSLINK_BUTTONS";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	const char *_dummy = "";
	INT16 errCode = 0;
	_parser.parseINT16(errCode);
	AtfValidator::validateInt(_descr, "errCode", errCode, _checker, __FILE__, __LINE__);
	if( errCode )
	{
		_parser.parseStringN(_dummy, 0, "errStr"); size_t szErrStr = strlen(_dummy);
		AtfValidator::validateInt(_descr, "errStr", szErrStr, _checker, __FILE__, __LINE__);
	}
	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                ProtocolFactory
//=================================================================

/* static */ bool CrossGameLinkServer::ProtocolFactory::validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded)
{
	if(!strcmp(_nameSpace, ""))
	{
		switch(_msgId)
		{
			case CRM_A_CREATE_CROSSLINK_BUTTONS: Protocol_CRM_A_CREATE_CROSSLINK_BUTTONS::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
			case CRM_A_GET_CROSSLINK_BUTTONS: Protocol_CRM_A_GET_CROSSLINK_BUTTONS::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
			case CRM_Q_CREATE_CROSSLINK_BUTTONS: Protocol_CRM_Q_CREATE_CROSSLINK_BUTTONS::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
			case CRM_Q_GET_CROSSLINK_BUTTONS: Protocol_CRM_Q_GET_CROSSLINK_BUTTONS::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
			case MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE: Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
			case MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY: Protocol_MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
		}
	}
	
	return false;
}
