/**
 * bduaclientprot_thin.cpp
 *
 * This file was auto-generated from bduaclientprot.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin bduaclientprot.txt
 */
 
#include "bduaclientprot_thin.h"

//=================================================================
//                    BDUAData
//=================================================================

BDUAClient::BDUAData::BDUAData()
{
	clear();
}

void BDUAClient::BDUAData::clear()
{
	bduaType = 0;
	bduaValue = 0;
}

bool BDUAClient::BDUAData::equals(const BDUAData& _o) const
{
	return bduaType == _o.bduaType &&
		bduaValue == _o.bduaValue;
}

const char *BDUAClient::BDUAData::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("bduaType=");
	_buf.appendUint(bduaType);
	_buf.append(',');
	_buf.append("bduaValue=");
	_buf.appendUint(bduaValue);
	_buf.append('}');
	return _buf.c_str();
}

void BDUAClient::BDUAData::composeMsg(CommMsgBody& _msg) const
{
	CommMsgBody _body;
	//if (!equals(BDUAData())) // not empty
	{
		_body.composeUINT32(bduaType);
		_body.composeUINT32(bduaValue);
	}

	_msg.composeMsgBody(_body);
}

void BDUAClient::BDUAData::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);

	CommMsgParser _parser0(_body);
	_parser0.parseUINT32(bduaType);
	_parser0.parseUINT32(bduaValue);
}

/*static*/ void BDUAClient::BDUAData::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	AtfMsgBodyParserView _parser0(_parser);
	_parser.skipMsgBody();
	UINT32 bduaType; _parser0.parseUINT32(bduaType);
	AtfValidator::validateInt(_descr, "bduaType", bduaType, _checker, __FILE__, __LINE__);
	UINT32 bduaValue; _parser0.parseUINT32(bduaValue);
	AtfValidator::validateInt(_descr, "bduaValue", bduaValue, _checker, __FILE__, __LINE__);
	AtfValidator::validateEmbeddedMsgbodyParser(_descr, _parser0, _parser._shift(), __FILE__, __LINE__);
	if(!_parser0.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser0);
}

//=================================================================
//                Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER
//=================================================================

BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER()
{
	clear();
}

void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::clear()
{
}

bool BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::equals(const Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER& _o) const
{
	return true;
}

const char *BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_LOBBYETC_GET_BDUAS_FOR_USER).append(")");
	return _buf.c_str();
}

void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::composeMsg(CommMsgBody& _msg) const
{
}

void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::parseMsg(CommMsgParser& _parser)
{
}

/*static*/ void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "MSG_LOBBYETC_GET_BDUAS_FOR_USER";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY
//=================================================================

BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY(Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY&& _o)
	: errCode(std::move(_o.errCode))
	, errString(std::move(_o.errString))
	, userBDUAs(std::move(_o.userBDUAs))
	, userTier(std::move(_o.userTier))
{
}

BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY& BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::operator=(Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY&& _o)
{
	if(this != &_o)
	{
		errCode = std::move(_o.errCode);
		errString = std::move(_o.errString);
		userBDUAs = std::move(_o.userBDUAs);
		userTier = std::move(_o.userTier);
	}
	return *this;
}

#endif

void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::clear()
{
	errCode = 0;
	errString.clear();
	userBDUAs.clear();
	userTier = 0;
}

bool BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::equals(const Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY& _o) const
{
	return errCode == _o.errCode &&
		errString.equals(_o.errString) &&
		userBDUAs.equals(_o.userBDUAs) &&
		userTier == _o.userTier;
}

const char *BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY).append(")");
	_buf.append(',');
	_buf.append("errCode=");
	_buf.appendInt(errCode);
	if(errCode)
	{
		_buf.append(',');
		_buf.append("errString=");
		_buf.append(errString);
	}
	else
	{
		_buf.append(',');
		_buf.append("userBDUAs=");
		userBDUAs.toTraceString(_buf);
		_buf.append(',');
		_buf.append("userTier=");
		_buf.appendUint(userTier);
	}
	return _buf.c_str();
}

void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeINT16(errCode);
	if(errCode)
	{
		_msg.composeString(errString);
	}
	else
	{
		userBDUAs.composeMsg(_msg);
		_msg.composeUINT32(userTier);
	}
}

void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::parseMsg(CommMsgParser& _parser)
{
	_parser.parseINT16(errCode);
	if(errCode)
	{
		_parser.parseStringP(errString);
	}
	else
	{
		userBDUAs.parseMsg(_parser);
		if(_parser.parseEnded()) return;
		_parser.parseUINT32(userTier);
	}
}

/*static*/ void BDUAClient::Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsWithUnparsedContent)
{
	const char *_descr = "MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY";
	if(_checker)
	{
		if(!_checker->isValidationEnabled(_descr)) return;
		if(!_checker->hasLimitDefined(_descr)) _checker = nullptr;
	}

	const char *_dummy = "";
	INT16 errCode = 0;
	_parser.parseINT16(errCode);
	AtfValidator::validateInt(_descr, "errCode", errCode, _checker, __FILE__, __LINE__);
	if(errCode)
	{
		_parser.parseStringN(_dummy, 0, "errString"); size_t szErrString = strlen(_dummy);
		AtfValidator::validateInt(_descr, "errString", szErrString, _checker, __FILE__, __LINE__);
	}
	else
	{
		PString _descbuf;
		int szUserBDUAs = ThinAtf::LAtfVector< BDUAData, 4 >::validateMsg(_parser, _checker, _descbuf.assign(_descr).append('.').append("userBDUAs"), _fieldsWithUnparsedContent);
		AtfValidator::validateInt(_descr, "userBDUAs", szUserBDUAs, _checker, __FILE__, __LINE__);
		if(_parser.parseEnded()) return;
		UINT32 userTier; _parser.parseUINT32(userTier);
		AtfValidator::validateInt(_descr, "userTier", userTier, _checker, __FILE__, __LINE__);
	}
	if(!_parser.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser);
}

//=================================================================
//                ProtocolFactory
//=================================================================

/* static */ bool BDUAClient::ProtocolFactory::validateProtocolMessage(const char *_nameSpace, UINT32 _msgId, CommMsgParser& _parser, const AtfMsgChecker* _checker, PString& _fieldsNotParseEnded)
{
	if(!strcmp(_nameSpace, ""))
	{
		switch(_msgId)
		{
			case MSG_LOBBYETC_GET_BDUAS_FOR_USER: Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
			case MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY: Protocol_MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY::validateMsg(_parser, _checker, _fieldsNotParseEnded); return true;
		}
	}
	
	return false;
}
