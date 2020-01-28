/**
 * FppFactor.cpp
 *
 * This file was auto-generated from FppFactor.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor FppFactor.txt
 */
 
#include "FppFactor.h"

//=================================================================
//                    FppFactor
//=================================================================

TournClient::FppFactor::FppFactor()
{
	clear();
}

void TournClient::FppFactor::clear()
{
	fppFactor = 0;
	sites = 0;
	endT.setNull();
}

bool TournClient::FppFactor::equals(const FppFactor& _o) const
{
	return fppFactor == _o.fppFactor &&
		sites == _o.sites &&
		endT.equals(_o.endT);
}

const char *TournClient::FppFactor::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("fppFactor=");
	_buf.appendUint(fppFactor);
	_buf.append(',');
	_buf.append("sites=");
	_buf.appendUint(sites);
	_buf.append(',');
	_buf.append("endT=");
	Atf::AtfTempl<SrvTime>::ToTraceString(_buf, endT);
	_buf.append('}');
	return _buf.c_str();
}

const char *TournClient::FppFactor::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("fppFactor", fppFactor, _buf);
	Atf::XmlElement::encodeAsXmlElement("sites", sites, _buf);
	Atf::AtfTempl<SrvTime>::ToXmlString(_buf, "endT", endT);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool TournClient::FppFactor::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _str.length(), _element, _value, _type))
	{
		if (_element.equals("fppFactor"))
		{
			fppFactor = (UINT16)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("sites"))
		{
			sites = (UINT32)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("endT"))
		{
			Atf::AtfTempl<SrvTime>::FromXmlString(_value, endT);
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void TournClient::FppFactor::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	_msg.composeUINT16(fppFactor);
	_msg.composeUINT32(sites);
	_msg.composeSrvTime(endT);
}

void TournClient::FppFactor::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT16(fppFactor);
	_parser.parseUINT32(sites);
	_parser.parseSrvTime(endT);
}

const char *TournClient::FppFactor::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("fppFactor", fppFactor);
	_jsonstr.compose("sites", sites);
	_jsonstr.compose("endT", endT);
	return _buf.c_str();
}

void TournClient::FppFactor::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("fppFactor", fppFactor);
	_jparser.parseByNameThrow("sites", sites);
	_jparser.parseByNameThrow("endT", endT);
}

/* static */ void TournClient::FppFactor::validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	Atf::JSONStringValidatorWithChecker _jparser(_jsonString, _checker, _descr, _fieldsWithUnparsedContent);
	UINT16 fppFactor; _jparser.validateByNameThrow("fppFactor", fppFactor);
	AtfValidator::validateInt(_descr, "fppFactor", fppFactor, _checker, __FILE__, __LINE__);
	UINT32 sites; _jparser.validateByNameThrow("sites", sites);
	AtfValidator::validateInt(_descr, "sites", sites, _checker, __FILE__, __LINE__);
	SrvTime endT; _jparser.validateByNameThrow("endT", endT);
	AtfValidator::validateSrvDateTime(_descr, "endT", endT, _checker, __FILE__, __LINE__);
}

/*static*/ void TournClient::FppFactor::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	UINT16 fppFactor; _parser.parseUINT16(fppFactor);
	AtfValidator::validateInt(_descr, "fppFactor", fppFactor, _checker, __FILE__, __LINE__);
	UINT32 sites; _parser.parseUINT32(sites);
	AtfValidator::validateInt(_descr, "sites", sites, _checker, __FILE__, __LINE__);
	SrvTime endT; _parser.parseSrvTime(endT);
	AtfValidator::validateSrvDateTime(_descr, "endT", endT, _checker, __FILE__, __LINE__);
}

//=================================================================
//                    FppFactorEx
//=================================================================

TournClient::FppFactorEx::FppFactorEx()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

TournClient::FppFactorEx::FppFactorEx(FppFactorEx&& _o)
	: fppFactor(std::move(_o.fppFactor))
	, sites(std::move(_o.sites))
	, endT(std::move(_o.endT))
{
}

TournClient::FppFactorEx& TournClient::FppFactorEx::operator=(FppFactorEx&& _o)
{
	if(this != &_o)
	{
		fppFactor = std::move(_o.fppFactor);
		sites = std::move(_o.sites);
		endT = std::move(_o.endT);
	}
	return *this;
}

#endif

void TournClient::FppFactorEx::clear()
{
	fppFactor = 0;
	sites.clear();
	endT.setNull();
}

bool TournClient::FppFactorEx::equals(const FppFactorEx& _o) const
{
	return fppFactor == _o.fppFactor &&
		sites.equals(_o.sites) &&
		endT.equals(_o.endT);
}

const char *TournClient::FppFactorEx::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("fppFactor=");
	_buf.appendUint(fppFactor);
	_buf.append(',');
	_buf.append("sites=");
	sites.toTraceString(_buf);
	_buf.append(',');
	_buf.append("endT=");
	Atf::AtfTempl<SrvTime>::ToTraceString(_buf, endT);
	_buf.append('}');
	return _buf.c_str();
}

const char *TournClient::FppFactorEx::toXmlString(const char *_element, PString& _buf) const
{
	_buf.append('<').append(_element).append('>');
	Atf::XmlElement::encodeAsXmlElement("fppFactor", fppFactor, _buf);
	sites.toXmlString("sites", _buf);
	Atf::AtfTempl<SrvTime>::ToXmlString(_buf, "endT", endT);
	Atf::XmlElement::appendElementCloseTag(_element, _buf);
	return _buf.c_str();
}

bool TournClient::FppFactorEx::fromXmlString(const PString& _str)
{
	PString _element;
	PBlock _value;
	PString _type;
	const char* _xml = _str.c_str();
	
	while (Atf::XmlElement::decodeNextXmlElement(_xml, _str.length(), _element, _value, _type))
	{
		if (_element.equals("fppFactor"))
		{
			fppFactor = (UINT16)p_atoi64((const char *)_value.ptr());
		}
		else if (_element.equals("sites"))
		{
			if(!Atf::AtfTempl< PSiteMask >::FromXmlString(_value, sites)) return false;
		}
		else if (_element.equals("endT"))
		{
			Atf::AtfTempl<SrvTime>::FromXmlString(_value, endT);
		}
		else
		{
			return false;  // unknown instance variable name
		}
	}
	return true;
}

void TournClient::FppFactorEx::composeMsg(CommMsgBody& _msg, bool _ignoreJSON) const
{
	CommMsgBody _body;
	//if (!equals(FppFactorEx())) // not empty
	{
		_body.composeUINT16(fppFactor);
		sites.composeMsg(_body, _ignoreJSON);
		_body.composeSrvTime(endT);
	}

	_msg.composeMsgBody(_body);
}

void TournClient::FppFactorEx::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);

	CommMsgParser _parser0(_body);
	_parser0.parseUINT16(fppFactor);
	sites.parseMsg(_parser0);
	_parser0.parseSrvTime(endT);
}

const char *TournClient::FppFactorEx::toJSONString(PString& _buf) const
{
	Atf::JSONStringComposer _jsonstr(_buf);
	_jsonstr.compose("fppFactor", fppFactor);
	_jsonstr.compose("sites", sites);
	_jsonstr.compose("endT", endT);
	return _buf.c_str();
}

void TournClient::FppFactorEx::fromJSONString(const PString& _jsonString)
{
	Atf::JSONStringParser _jparser(_jsonString);
	_jparser.parseByNameThrow("fppFactor", fppFactor);
	_jparser.parseByNameThrow("sites", sites);
	_jparser.parseByNameThrow("endT", endT);
}

/* static */ void TournClient::FppFactorEx::validateJSONString(const PString& _jsonString, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	Atf::JSONStringValidatorWithChecker _jparser(_jsonString, _checker, _descr, _fieldsWithUnparsedContent);
	UINT16 fppFactor; _jparser.validateByNameThrow("fppFactor", fppFactor);
	AtfValidator::validateInt(_descr, "fppFactor", fppFactor, _checker, __FILE__, __LINE__);
	PSiteMask sites; _jparser.validateByNameThrow("sites", sites);
	SrvTime endT; _jparser.validateByNameThrow("endT", endT);
	AtfValidator::validateSrvDateTime(_descr, "endT", endT, _checker, __FILE__, __LINE__);
}

/*static*/ void TournClient::FppFactorEx::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	AtfMsgBodyParserView _parser0(_parser);
	_parser.skipMsgBody();
	UINT16 fppFactor; _parser0.parseUINT16(fppFactor);
	AtfValidator::validateInt(_descr, "fppFactor", fppFactor, _checker, __FILE__, __LINE__);
	PString _descbuf;
	PSiteMask::validateMsg(_parser0, _checker, _descbuf.assign(_descr).append('.').append("sites"), _fieldsWithUnparsedContent);
	SrvTime endT; _parser0.parseSrvTime(endT);
	AtfValidator::validateSrvDateTime(_descr, "endT", endT, _checker, __FILE__, __LINE__);
	AtfValidator::validateEmbeddedMsgbodyParser(_descr, _parser0, _parser._shift(), __FILE__, __LINE__);
	if(!_parser0.parseEnded()) AtfValidator::logUnparsedInfo(_fieldsWithUnparsedContent, _descr, _parser0);
}
