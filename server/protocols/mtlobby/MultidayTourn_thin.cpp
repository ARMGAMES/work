/**
 * MultidayTourn_thin.cpp
 *
 * This file was auto-generated from MultidayTourn.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -genMoveConstructor -thin MultidayTourn.txt
 */
 
#include "MultidayTourn_thin.h"

//=================================================================
//                    MultidayTourn
//=================================================================

TournClientThin::MultidayTourn::MultidayTourn()
{
	clear();
}

void TournClientThin::MultidayTourn::clear()
{
	props = 0;
	dayOneStopValue = 0;
	dayTwoStartMinutes = 0;
	dayTwoStart.setNull();
}

bool TournClientThin::MultidayTourn::equals(const MultidayTourn& _o) const
{
	return props == _o.props &&
		dayOneStopValue == _o.dayOneStopValue &&
		dayTwoStartMinutes == _o.dayTwoStartMinutes &&
		dayTwoStart.equals(_o.dayTwoStart);
}

const char *TournClientThin::MultidayTourn::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("props=");
	_buf.appendUint(props);
	_buf.append(',');
	_buf.append("dayOneStopValue=");
	_buf.appendUint(dayOneStopValue);
	_buf.append(',');
	_buf.append("dayTwoStartMinutes=");
	_buf.appendUint(dayTwoStartMinutes);
	_buf.append(',');
	_buf.append("dayTwoStart=");
	ThinAtf::AtfTempl<CommSrvTime>::ToTraceString(_buf, dayTwoStart);
	_buf.append('}');
	return _buf.c_str();
}

void TournClientThin::MultidayTourn::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeUINT32(props);
	_msg.composeUINT32(dayOneStopValue);
	_msg.composeUINT32(dayTwoStartMinutes);
	_msg.composeSrvTime(dayTwoStart);
}

void TournClientThin::MultidayTourn::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT32(props);
	_parser.parseUINT32(dayOneStopValue);
	_parser.parseUINT32(dayTwoStartMinutes);
	_parser.parseSrvTime(dayTwoStart);
}

/*static*/ void TournClientThin::MultidayTourn::validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char* _descr, PString& _fieldsWithUnparsedContent)
{
	UINT32 props; _parser.parseUINT32(props);
	AtfValidator::validateInt(_descr, "props", props, _checker, __FILE__, __LINE__);
	UINT32 dayOneStopValue; _parser.parseUINT32(dayOneStopValue);
	AtfValidator::validateInt(_descr, "dayOneStopValue", dayOneStopValue, _checker, __FILE__, __LINE__);
	UINT32 dayTwoStartMinutes; _parser.parseUINT32(dayTwoStartMinutes);
	AtfValidator::validateInt(_descr, "dayTwoStartMinutes", dayTwoStartMinutes, _checker, __FILE__, __LINE__);
	CommSrvTime dayTwoStart; _parser.parseSrvTime(dayTwoStart);
	AtfValidator::validateSrvDateTime(_descr, "dayTwoStart", dayTwoStart, _checker, __FILE__, __LINE__);
}

