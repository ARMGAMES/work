/**
 * protocols\PoolBetting\PB_Server_WAPI_thin.cpp
 *
 * This file was auto-generated from protocols\PoolBetting\PB_Server_WAPI.txt by ATF protocol generator. 
 * Do not edit this file directly!
 *
 * Generator used the following keys: -thin -genMoveConstructor -dontGenValidationFuncs protocols\PoolBetting\PB_Server_WAPI.txt
 */
 
#include "PB_Server_WAPI_thin.h"

//=================================================================
//                    Balance
//=================================================================

PB::Balance::Balance()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::Balance::Balance(Balance&& _o)
	: currency(std::move(_o.currency))
	, amount(std::move(_o.amount))
{
}

PB::Balance& PB::Balance::operator=(Balance&& _o)
{
	if(this != &_o)
	{
		currency = std::move(_o.currency);
		amount = std::move(_o.amount);
	}
	return *this;
}

#endif

void PB::Balance::clear()
{
	currency.clear();
	amount = 0;
}

bool PB::Balance::equals(const Balance& _o) const
{
	return currency.equals(_o.currency) &&
		amount == _o.amount;
}

const char *PB::Balance::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("amount=");
	_buf.appendInt(amount);
	_buf.append('}');
	return _buf.c_str();
}

void PB::Balance::composeMsg(CommMsgBody& _msg) const
{
	CommMsgBody _body;
	//if (!equals(Balance())) // not empty
	{
		_body.composeString(currency);
		_body.composeINT32(amount);
	}

	_msg.composeMsgBody(_body);
}

void PB::Balance::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);

	CommMsgParser _parser0(_body);
	_parser0.parseStringP(currency);
	_parser0.parseINT32(amount);
}

//=================================================================
//                    Warning
//=================================================================

PB::Warning::Warning()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::Warning::Warning(Warning&& _o)
	: code(std::move(_o.code))
	, description(std::move(_o.description))
{
}

PB::Warning& PB::Warning::operator=(Warning&& _o)
{
	if(this != &_o)
	{
		code = std::move(_o.code);
		description = std::move(_o.description);
	}
	return *this;
}

#endif

void PB::Warning::clear()
{
	code = 0;
	description.clear();
}

bool PB::Warning::equals(const Warning& _o) const
{
	return code == _o.code &&
		description.equals(_o.description);
}

const char *PB::Warning::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("code=");
	_buf.appendInt(code);
	_buf.append(',');
	_buf.append("description=");
	_buf.append(description);
	_buf.append('}');
	return _buf.c_str();
}

void PB::Warning::composeMsg(CommMsgBody& _msg) const
{
	CommMsgBody _body;
	//if (!equals(Warning())) // not empty
	{
		_body.composeINT16(code);
		_body.composeString(description);
	}

	_msg.composeMsgBody(_body);
}

void PB::Warning::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);

	CommMsgParser _parser0(_body);
	_parser0.parseINT16(code);
	_parser0.parseStringP(description);
}

//=================================================================
//                    StakeLimit
//=================================================================

PB::StakeLimit::StakeLimit()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::StakeLimit::StakeLimit(StakeLimit&& _o)
	: currency(std::move(_o.currency))
	, amount(std::move(_o.amount))
{
}

PB::StakeLimit& PB::StakeLimit::operator=(StakeLimit&& _o)
{
	if(this != &_o)
	{
		currency = std::move(_o.currency);
		amount = std::move(_o.amount);
	}
	return *this;
}

#endif

void PB::StakeLimit::clear()
{
	currency.clear();
	amount = 0;
}

bool PB::StakeLimit::equals(const StakeLimit& _o) const
{
	return currency.equals(_o.currency) &&
		amount == _o.amount;
}

const char *PB::StakeLimit::toTraceString(PString& _buf) const
{
	_buf.append('{');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("amount=");
	_buf.appendInt(amount);
	_buf.append('}');
	return _buf.c_str();
}

void PB::StakeLimit::composeMsg(CommMsgBody& _msg) const
{
	CommMsgBody _body;
	//if (!equals(StakeLimit())) // not empty
	{
		_body.composeString(currency);
		_body.composeINT32(amount);
	}

	_msg.composeMsgBody(_body);
}

void PB::StakeLimit::parseMsg(CommMsgParser& _parser)
{
	CommMsgBody _body;
	_parser.parseMsgBody(_body);

	CommMsgParser _parser0(_body);
	_parser0.parseStringP(currency);
	_parser0.parseINT32(amount);
}

//=================================================================
//                Protocol_Q_GET_USER_INFO
//=================================================================

PB::PB_AuthWAPI::Protocol_Q_GET_USER_INFO::Protocol_Q_GET_USER_INFO()
{
	clear();
}

void PB::PB_AuthWAPI::Protocol_Q_GET_USER_INFO::clear()
{
}

bool PB::PB_AuthWAPI::Protocol_Q_GET_USER_INFO::equals(const Protocol_Q_GET_USER_INFO& _o) const
{
	return true;
}

const char *PB::PB_AuthWAPI::Protocol_Q_GET_USER_INFO::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(Q_GET_USER_INFO).append(")");
	return _buf.c_str();
}

void PB::PB_AuthWAPI::Protocol_Q_GET_USER_INFO::composeMsg(CommMsgBody& _msg) const
{
}

void PB::PB_AuthWAPI::Protocol_Q_GET_USER_INFO::parseMsg(CommMsgParser& _parser)
{
}

//=================================================================
//                Protocol_A_GET_USER_INFO
//=================================================================

PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::Protocol_A_GET_USER_INFO()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::Protocol_A_GET_USER_INFO(Protocol_A_GET_USER_INFO&& _o)
	: webId(std::move(_o.webId))
	, defaultCurrency(std::move(_o.defaultCurrency))
	, balances(std::move(_o.balances))
	, timezone(std::move(_o.timezone))
	, site(std::move(_o.site))
	, language(std::move(_o.language))
	, warnings(std::move(_o.warnings))
	, stakeLimits(std::move(_o.stakeLimits))
	, country(std::move(_o.country))
{
}

PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO& PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::operator=(Protocol_A_GET_USER_INFO&& _o)
{
	if(this != &_o)
	{
		webId = std::move(_o.webId);
		defaultCurrency = std::move(_o.defaultCurrency);
		balances = std::move(_o.balances);
		timezone = std::move(_o.timezone);
		site = std::move(_o.site);
		language = std::move(_o.language);
		warnings = std::move(_o.warnings);
		stakeLimits = std::move(_o.stakeLimits);
		country = std::move(_o.country);
	}
	return *this;
}

#endif

void PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::clear()
{
	webId.clear();
	defaultCurrency.clear();
	balances.clear();
	timezone.clear();
	site.clear();
	language.clear();
	warnings.clear();
	stakeLimits.clear();
	country.clear();
}

bool PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::equals(const Protocol_A_GET_USER_INFO& _o) const
{
	return webId.equals(_o.webId) &&
		defaultCurrency.equals(_o.defaultCurrency) &&
		balances.equals(_o.balances) &&
		timezone.equals(_o.timezone) &&
		site.equals(_o.site) &&
		language.equals(_o.language) &&
		warnings.equals(_o.warnings) &&
		stakeLimits.equals(_o.stakeLimits) &&
		country.equals(_o.country);
}

const char *PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(A_GET_USER_INFO).append(")");
	_buf.append(',');
	_buf.append("webId=");
	_buf.append(webId);
	_buf.append(',');
	_buf.append("defaultCurrency=");
	_buf.append(defaultCurrency);
	_buf.append(',');
	_buf.append("balances=");
	balances.toTraceString(_buf);
	_buf.append(',');
	_buf.append("timezone=");
	_buf.append(timezone);
	_buf.append(',');
	_buf.append("site=");
	_buf.append(site);
	_buf.append(',');
	_buf.append("language=");
	_buf.append(language);
	_buf.append(',');
	_buf.append("warnings=");
	warnings.toTraceString(_buf);
	_buf.append(',');
	_buf.append("stakeLimits=");
	stakeLimits.toTraceString(_buf);
	_buf.append(',');
	_buf.append("country=");
	_buf.append(country);
	return _buf.c_str();
}

void PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeString(webId);
	_msg.composeString(defaultCurrency);
	balances.composeMsg(_msg);
	_msg.composeString(timezone);
	_msg.composeString(site);
	_msg.composeString(language);
	warnings.composeMsg(_msg);
	stakeLimits.composeMsg(_msg);
	_msg.composeString(country);
}

void PB::PB_AuthWAPI::Protocol_A_GET_USER_INFO::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(webId);
	_parser.parseStringP(defaultCurrency);
	balances.parseMsg(_parser);
	_parser.parseStringP(timezone);
	_parser.parseStringP(site);
	_parser.parseStringP(language);
	warnings.parseMsg(_parser);
	stakeLimits.parseMsg(_parser);
	if(_parser.parseEnded()) return;
	_parser.parseStringP(country);
}

//=================================================================
//                Protocol_Q_PLACE_TICKET
//=================================================================

PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::Protocol_Q_PLACE_TICKET()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::Protocol_Q_PLACE_TICKET(Protocol_Q_PLACE_TICKET&& _o)
	: transactionId(std::move(_o.transactionId))
	, ticketId(std::move(_o.ticketId))
	, currency(std::move(_o.currency))
	, cashAmount(std::move(_o.cashAmount))
	, description(std::move(_o.description))
{
}

PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET& PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::operator=(Protocol_Q_PLACE_TICKET&& _o)
{
	if(this != &_o)
	{
		transactionId = std::move(_o.transactionId);
		ticketId = std::move(_o.ticketId);
		currency = std::move(_o.currency);
		cashAmount = std::move(_o.cashAmount);
		description = std::move(_o.description);
	}
	return *this;
}

#endif

void PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::clear()
{
	transactionId = 0;
	ticketId = 0;
	currency.clear();
	cashAmount = 0;
	description.clear();
}

bool PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::equals(const Protocol_Q_PLACE_TICKET& _o) const
{
	return transactionId == _o.transactionId &&
		ticketId == _o.ticketId &&
		currency.equals(_o.currency) &&
		cashAmount == _o.cashAmount &&
		description.equals(_o.description);
}

const char *PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(Q_PLACE_TICKET).append(")");
	_buf.append(',');
	_buf.append("transactionId=");
	_buf.appendUint64(transactionId);
	_buf.append(',');
	_buf.append("ticketId=");
	_buf.appendUint64(ticketId);
	_buf.append(',');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("cashAmount=");
	_buf.appendInt(cashAmount);
	_buf.append(',');
	_buf.append("description=");
	_buf.append(description);
	return _buf.c_str();
}

void PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeUINT64(transactionId);
	_msg.composeUINT64(ticketId);
	_msg.composeString(currency);
	_msg.composeINT32(cashAmount);
	_msg.composeString(description);
}

void PB::PB_AuthWAPI::Protocol_Q_PLACE_TICKET::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT64(transactionId);
	_parser.parseUINT64(ticketId);
	_parser.parseStringP(currency);
	_parser.parseINT32(cashAmount);
	_parser.parseStringP(description);
}

//=================================================================
//                Protocol_A_PLACE_TICKET
//=================================================================

PB::PB_AuthWAPI::Protocol_A_PLACE_TICKET::Protocol_A_PLACE_TICKET()
{
	clear();
}

void PB::PB_AuthWAPI::Protocol_A_PLACE_TICKET::clear()
{
}

bool PB::PB_AuthWAPI::Protocol_A_PLACE_TICKET::equals(const Protocol_A_PLACE_TICKET& _o) const
{
	return true;
}

const char *PB::PB_AuthWAPI::Protocol_A_PLACE_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(A_PLACE_TICKET).append(")");
	return _buf.c_str();
}

void PB::PB_AuthWAPI::Protocol_A_PLACE_TICKET::composeMsg(CommMsgBody& _msg) const
{
}

void PB::PB_AuthWAPI::Protocol_A_PLACE_TICKET::parseMsg(CommMsgParser& _parser)
{
}

//=================================================================
//                Protocol_Q_CASHOUT_TICKET
//=================================================================

PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::Protocol_Q_CASHOUT_TICKET()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::Protocol_Q_CASHOUT_TICKET(Protocol_Q_CASHOUT_TICKET&& _o)
	: transactionId(std::move(_o.transactionId))
	, ticketId(std::move(_o.ticketId))
	, currency(std::move(_o.currency))
	, cashAmount(std::move(_o.cashAmount))
	, remainingStake(std::move(_o.remainingStake))
	, cashoutMargin(std::move(_o.cashoutMargin))
	, description(std::move(_o.description))
{
}

PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET& PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::operator=(Protocol_Q_CASHOUT_TICKET&& _o)
{
	if(this != &_o)
	{
		transactionId = std::move(_o.transactionId);
		ticketId = std::move(_o.ticketId);
		currency = std::move(_o.currency);
		cashAmount = std::move(_o.cashAmount);
		remainingStake = std::move(_o.remainingStake);
		cashoutMargin = std::move(_o.cashoutMargin);
		description = std::move(_o.description);
	}
	return *this;
}

#endif

void PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::clear()
{
	transactionId = 0;
	ticketId = 0;
	currency.clear();
	cashAmount = 0;
	remainingStake = 0;
	cashoutMargin = 0;
	description.clear();
}

bool PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::equals(const Protocol_Q_CASHOUT_TICKET& _o) const
{
	return transactionId == _o.transactionId &&
		ticketId == _o.ticketId &&
		currency.equals(_o.currency) &&
		cashAmount == _o.cashAmount &&
		remainingStake == _o.remainingStake &&
		cashoutMargin == _o.cashoutMargin &&
		description.equals(_o.description);
}

const char *PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(Q_CASHOUT_TICKET).append(")");
	_buf.append(',');
	_buf.append("transactionId=");
	_buf.appendUint64(transactionId);
	_buf.append(',');
	_buf.append("ticketId=");
	_buf.appendUint64(ticketId);
	_buf.append(',');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("cashAmount=");
	_buf.appendInt(cashAmount);
	_buf.append(',');
	_buf.append("remainingStake=");
	_buf.appendInt(remainingStake);
	_buf.append(',');
	_buf.append("cashoutMargin=");
	_buf.appendInt(cashoutMargin);
	_buf.append(',');
	_buf.append("description=");
	_buf.append(description);
	return _buf.c_str();
}

void PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeUINT64(transactionId);
	_msg.composeUINT64(ticketId);
	_msg.composeString(currency);
	_msg.composeINT32(cashAmount);
	_msg.composeINT32(remainingStake);
	_msg.composeINT32(cashoutMargin);
	_msg.composeString(description);
}

void PB::PB_AuthWAPI::Protocol_Q_CASHOUT_TICKET::parseMsg(CommMsgParser& _parser)
{
	_parser.parseUINT64(transactionId);
	_parser.parseUINT64(ticketId);
	_parser.parseStringP(currency);
	_parser.parseINT32(cashAmount);
	_parser.parseINT32(remainingStake);
	_parser.parseINT32(cashoutMargin);
	_parser.parseStringP(description);
}

//=================================================================
//                Protocol_A_CASHOUT_TICKET
//=================================================================

PB::PB_AuthWAPI::Protocol_A_CASHOUT_TICKET::Protocol_A_CASHOUT_TICKET()
{
	clear();
}

void PB::PB_AuthWAPI::Protocol_A_CASHOUT_TICKET::clear()
{
}

bool PB::PB_AuthWAPI::Protocol_A_CASHOUT_TICKET::equals(const Protocol_A_CASHOUT_TICKET& _o) const
{
	return true;
}

const char *PB::PB_AuthWAPI::Protocol_A_CASHOUT_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(A_CASHOUT_TICKET).append(")");
	return _buf.c_str();
}

void PB::PB_AuthWAPI::Protocol_A_CASHOUT_TICKET::composeMsg(CommMsgBody& _msg) const
{
}

void PB::PB_AuthWAPI::Protocol_A_CASHOUT_TICKET::parseMsg(CommMsgParser& _parser)
{
}

//=================================================================
//                Protocol_Q_SETTLE_TICKET
//=================================================================

PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::Protocol_Q_SETTLE_TICKET()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::Protocol_Q_SETTLE_TICKET(Protocol_Q_SETTLE_TICKET&& _o)
	: webId(std::move(_o.webId))
	, transactionId(std::move(_o.transactionId))
	, ticketId(std::move(_o.ticketId))
	, currency(std::move(_o.currency))
	, cashAmount(std::move(_o.cashAmount))
	, description(std::move(_o.description))
{
}

PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET& PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::operator=(Protocol_Q_SETTLE_TICKET&& _o)
{
	if(this != &_o)
	{
		webId = std::move(_o.webId);
		transactionId = std::move(_o.transactionId);
		ticketId = std::move(_o.ticketId);
		currency = std::move(_o.currency);
		cashAmount = std::move(_o.cashAmount);
		description = std::move(_o.description);
	}
	return *this;
}

#endif

void PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::clear()
{
	webId.clear();
	transactionId = 0;
	ticketId = 0;
	currency.clear();
	cashAmount = 0;
	description.clear();
}

bool PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::equals(const Protocol_Q_SETTLE_TICKET& _o) const
{
	return webId.equals(_o.webId) &&
		transactionId == _o.transactionId &&
		ticketId == _o.ticketId &&
		currency.equals(_o.currency) &&
		cashAmount == _o.cashAmount &&
		description.equals(_o.description);
}

const char *PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(Q_SETTLE_TICKET).append(")");
	_buf.append(',');
	_buf.append("webId=");
	_buf.append(webId);
	_buf.append(',');
	_buf.append("transactionId=");
	_buf.appendUint64(transactionId);
	_buf.append(',');
	_buf.append("ticketId=");
	_buf.appendUint64(ticketId);
	_buf.append(',');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("cashAmount=");
	_buf.appendInt(cashAmount);
	_buf.append(',');
	_buf.append("description=");
	_buf.append(description);
	return _buf.c_str();
}

void PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeString(webId);
	_msg.composeUINT64(transactionId);
	_msg.composeUINT64(ticketId);
	_msg.composeString(currency);
	_msg.composeINT32(cashAmount);
	_msg.composeString(description);
}

void PB::PB_PlainWAPI::Protocol_Q_SETTLE_TICKET::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(webId);
	_parser.parseUINT64(transactionId);
	_parser.parseUINT64(ticketId);
	_parser.parseStringP(currency);
	_parser.parseINT32(cashAmount);
	_parser.parseStringP(description);
}

//=================================================================
//                Protocol_A_SETTLE_TICKET
//=================================================================

PB::PB_PlainWAPI::Protocol_A_SETTLE_TICKET::Protocol_A_SETTLE_TICKET()
{
	clear();
}

void PB::PB_PlainWAPI::Protocol_A_SETTLE_TICKET::clear()
{
}

bool PB::PB_PlainWAPI::Protocol_A_SETTLE_TICKET::equals(const Protocol_A_SETTLE_TICKET& _o) const
{
	return true;
}

const char *PB::PB_PlainWAPI::Protocol_A_SETTLE_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(A_SETTLE_TICKET).append(")");
	return _buf.c_str();
}

void PB::PB_PlainWAPI::Protocol_A_SETTLE_TICKET::composeMsg(CommMsgBody& _msg) const
{
}

void PB::PB_PlainWAPI::Protocol_A_SETTLE_TICKET::parseMsg(CommMsgParser& _parser)
{
}

//=================================================================
//                Protocol_Q_VOID_TICKET
//=================================================================

PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::Protocol_Q_VOID_TICKET()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::Protocol_Q_VOID_TICKET(Protocol_Q_VOID_TICKET&& _o)
	: webId(std::move(_o.webId))
	, transactionId(std::move(_o.transactionId))
	, ticketId(std::move(_o.ticketId))
	, currency(std::move(_o.currency))
	, cashAmount(std::move(_o.cashAmount))
	, description(std::move(_o.description))
{
}

PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET& PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::operator=(Protocol_Q_VOID_TICKET&& _o)
{
	if(this != &_o)
	{
		webId = std::move(_o.webId);
		transactionId = std::move(_o.transactionId);
		ticketId = std::move(_o.ticketId);
		currency = std::move(_o.currency);
		cashAmount = std::move(_o.cashAmount);
		description = std::move(_o.description);
	}
	return *this;
}

#endif

void PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::clear()
{
	webId.clear();
	transactionId = 0;
	ticketId = 0;
	currency.clear();
	cashAmount = 0;
	description.clear();
}

bool PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::equals(const Protocol_Q_VOID_TICKET& _o) const
{
	return webId.equals(_o.webId) &&
		transactionId == _o.transactionId &&
		ticketId == _o.ticketId &&
		currency.equals(_o.currency) &&
		cashAmount == _o.cashAmount &&
		description.equals(_o.description);
}

const char *PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(Q_VOID_TICKET).append(")");
	_buf.append(',');
	_buf.append("webId=");
	_buf.append(webId);
	_buf.append(',');
	_buf.append("transactionId=");
	_buf.appendUint64(transactionId);
	_buf.append(',');
	_buf.append("ticketId=");
	_buf.appendUint64(ticketId);
	_buf.append(',');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("cashAmount=");
	_buf.appendInt(cashAmount);
	_buf.append(',');
	_buf.append("description=");
	_buf.append(description);
	return _buf.c_str();
}

void PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeString(webId);
	_msg.composeUINT64(transactionId);
	_msg.composeUINT64(ticketId);
	_msg.composeString(currency);
	_msg.composeINT32(cashAmount);
	_msg.composeString(description);
}

void PB::PB_PlainWAPI::Protocol_Q_VOID_TICKET::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(webId);
	_parser.parseUINT64(transactionId);
	_parser.parseUINT64(ticketId);
	_parser.parseStringP(currency);
	_parser.parseINT32(cashAmount);
	_parser.parseStringP(description);
}

//=================================================================
//                Protocol_A_VOID_TICKET
//=================================================================

PB::PB_PlainWAPI::Protocol_A_VOID_TICKET::Protocol_A_VOID_TICKET()
{
	clear();
}

void PB::PB_PlainWAPI::Protocol_A_VOID_TICKET::clear()
{
}

bool PB::PB_PlainWAPI::Protocol_A_VOID_TICKET::equals(const Protocol_A_VOID_TICKET& _o) const
{
	return true;
}

const char *PB::PB_PlainWAPI::Protocol_A_VOID_TICKET::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(A_VOID_TICKET).append(")");
	return _buf.c_str();
}

void PB::PB_PlainWAPI::Protocol_A_VOID_TICKET::composeMsg(CommMsgBody& _msg) const
{
}

void PB::PB_PlainWAPI::Protocol_A_VOID_TICKET::parseMsg(CommMsgParser& _parser)
{
}

//=================================================================
//                Protocol_Q_CASHOUT_TICKET2
//=================================================================

PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::Protocol_Q_CASHOUT_TICKET2()
{
	clear();
}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED

PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::Protocol_Q_CASHOUT_TICKET2(Protocol_Q_CASHOUT_TICKET2&& _o)
	: webId(std::move(_o.webId))
	, transactionId(std::move(_o.transactionId))
	, ticketId(std::move(_o.ticketId))
	, currency(std::move(_o.currency))
	, cashAmount(std::move(_o.cashAmount))
	, remainingStake(std::move(_o.remainingStake))
	, cashoutMargin(std::move(_o.cashoutMargin))
	, description(std::move(_o.description))
{
}

PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2& PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::operator=(Protocol_Q_CASHOUT_TICKET2&& _o)
{
	if(this != &_o)
	{
		webId = std::move(_o.webId);
		transactionId = std::move(_o.transactionId);
		ticketId = std::move(_o.ticketId);
		currency = std::move(_o.currency);
		cashAmount = std::move(_o.cashAmount);
		remainingStake = std::move(_o.remainingStake);
		cashoutMargin = std::move(_o.cashoutMargin);
		description = std::move(_o.description);
	}
	return *this;
}

#endif

void PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::clear()
{
	webId.clear();
	transactionId = 0;
	ticketId = 0;
	currency.clear();
	cashAmount = 0;
	remainingStake = 0;
	cashoutMargin = 0;
	description.clear();
}

bool PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::equals(const Protocol_Q_CASHOUT_TICKET2& _o) const
{
	return webId.equals(_o.webId) &&
		transactionId == _o.transactionId &&
		ticketId == _o.ticketId &&
		currency.equals(_o.currency) &&
		cashAmount == _o.cashAmount &&
		remainingStake == _o.remainingStake &&
		cashoutMargin == _o.cashoutMargin &&
		description.equals(_o.description);
}

const char *PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(Q_CASHOUT_TICKET2).append(")");
	_buf.append(',');
	_buf.append("webId=");
	_buf.append(webId);
	_buf.append(',');
	_buf.append("transactionId=");
	_buf.appendUint64(transactionId);
	_buf.append(',');
	_buf.append("ticketId=");
	_buf.appendUint64(ticketId);
	_buf.append(',');
	_buf.append("currency=");
	_buf.append(currency);
	_buf.append(',');
	_buf.append("cashAmount=");
	_buf.appendInt(cashAmount);
	_buf.append(',');
	_buf.append("remainingStake=");
	_buf.appendInt(remainingStake);
	_buf.append(',');
	_buf.append("cashoutMargin=");
	_buf.appendInt(cashoutMargin);
	_buf.append(',');
	_buf.append("description=");
	_buf.append(description);
	return _buf.c_str();
}

void PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::composeMsg(CommMsgBody& _msg) const
{
	_msg.composeString(webId);
	_msg.composeUINT64(transactionId);
	_msg.composeUINT64(ticketId);
	_msg.composeString(currency);
	_msg.composeINT32(cashAmount);
	_msg.composeINT32(remainingStake);
	_msg.composeINT32(cashoutMargin);
	_msg.composeString(description);
}

void PB::PB_PlainWAPI::Protocol_Q_CASHOUT_TICKET2::parseMsg(CommMsgParser& _parser)
{
	_parser.parseStringP(webId);
	_parser.parseUINT64(transactionId);
	_parser.parseUINT64(ticketId);
	_parser.parseStringP(currency);
	_parser.parseINT32(cashAmount);
	_parser.parseINT32(remainingStake);
	_parser.parseINT32(cashoutMargin);
	_parser.parseStringP(description);
}

//=================================================================
//                Protocol_A_CASHOUT_TICKET2
//=================================================================

PB::PB_PlainWAPI::Protocol_A_CASHOUT_TICKET2::Protocol_A_CASHOUT_TICKET2()
{
	clear();
}

void PB::PB_PlainWAPI::Protocol_A_CASHOUT_TICKET2::clear()
{
}

bool PB::PB_PlainWAPI::Protocol_A_CASHOUT_TICKET2::equals(const Protocol_A_CASHOUT_TICKET2& _o) const
{
	return true;
}

const char *PB::PB_PlainWAPI::Protocol_A_CASHOUT_TICKET2::asTraceString(PString& _buf) const
{
	_buf.cut(0);
	_buf.append("msgid=").append(getMsgIdString()).append("(").appendUint(A_CASHOUT_TICKET2).append(")");
	return _buf.c_str();
}

void PB::PB_PlainWAPI::Protocol_A_CASHOUT_TICKET2::composeMsg(CommMsgBody& _msg) const
{
}

void PB::PB_PlainWAPI::Protocol_A_CASHOUT_TICKET2::parseMsg(CommMsgParser& _parser)
{
}
