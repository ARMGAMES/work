#ifndef MsgChecker_h
#define MsgChecker_h

#include "ppcontainer.h"
#include "ppinifile.h"
#include "commmsgbody.h"
#include "commmonitorlogger.h"
#include "UserDefStructs.h"
#include "AtfValidator.h"
#include "AtfCommPlayerConns.h"

/* MsgChecker is a framework to do sanity check of client provided contents (currently on the range of integer parameters only).
   It can be easily incorporated into existing server modules by following the following steps. See PYR-52360 for details.

1. Add MsgChecker.h/.cpp to the project, and modify build file (*.files) if applicable. You might need to add shared.cpp if it is not here.
	file=../protocols/msgchecker/MsgChecker.cpp

2. #include "msgchecker/MsgChecker.h"

3. Add the following to the server object:
	
	MsgChecker msgChecker; // PYR-52356

4. Add the following to dynamicInit() of the server object:

	msgChecker.dynamicInit(initialDir, "sanity.<your-server-name>.limit", iniFile); // PYR-52356

5. Add the following to reportPerformance() of the server object:

	msgChecker.reportPerformance(heLogger, reportBody); // PYR-52356

6. Normally limits are defined in the ATF message formats, but it can also be defined in sanity.<your-server-name>.limit file (see below for 
   explanations of the file) as dynamically reloadable limits with higher precedence. The overriding should only happen in rare situations
   in production.   

=============================================
*For ini file, the format is the following:
=============================================
- Production/QA setup (Document these additions in deployment notes of the JIRA)
[MsgLimitOptions]
; 1 (on), 0 (off)
checkenabled=1

; 1 (on - throw exception), 0 (off)
checkWithException=0

; comma separated list of messages for which verification is ignored even if requested by checkLimit call
excludeList=

; specify the folder where limit file is located (if set empty or not set, it is the the ini file folder)
limitsFileDir=

- Developer's local setup as an example:
[MsgLimitOptions]
; 1 (on), 0 (off)
checkenabled=1

; 1 (on - throw exception), 0 (off)
checkWithException=1     

; comma separated list of messages for which verification is ignored even if requested by checkLimit call
excludeList=MSG_LOBBY_TOURN_STATS,MSG_LOBBY_FIND_PLAYER2

; specify the folder where limit file is located (if set empty or not set, it is the the ini file folder)
limitsFileDir=

=============================================
*For limit file, the format is the following:
=============================================
[MsgLimits]
; individual limits for a particular message (MSG_LOBBY_USER_STATS) and particular param name (numHands). 
; first integer is max limit, second integer is min limit. Use * (star) if you want to skip any of the values (then a check for that limit will not be done)
; you may also just omit second limit or even all the limits
; both positive and negative values may be used, the numbers are read in INT64 format
MSG_LOBBYETC_CLIENT_HISTORY_REQUEST.type=1000
MSG_LOBBYETC_CLIENT_HISTORY_REQUEST.handIds.size=1000,0
MSG_LOBBY_USER_STATS.numHands=2000,0
MSG_LOBBY_SOME_OTHER_MESSAGE.param1=*,-1  
MSG_LOBBY_SOME_OTHER_MESSAGE.param2=3  
MSG_LOBBY_SOME_OTHER_MESSAGE.param3=

Note: you need to create sanity.<your-server-name>.limit file in the source directory of your server if it is not there
and commit to GIT. Use lobby2/sanity.lobby.limit as a reference.

=============================================
*See lobbyobject.cpp code as an example
=============================================

*/

#define MSG_CHECKER_ERROR_PREFIX	"INVALID_CLI_INPUT."
#define DEFAULT_CHECKENABLED		1
#define DEFAULT_CHECKWITHEXCEPTION	0
#define DEFAULT_CHECKUNPARSEDCONTENT 1

class MsgChecker : public AtfMsgChecker
{
	void reactOnLimitViolation(PString& errLine, const char* file, INT32 line) const;
	void readLimits(const char* limitsFileName);	

	// return value: true - limit checked, false - limit not checked
	bool checkLimitRaw(INT64 value, const char* fieldKey, bool fromATF, const char* file, INT32 line) const;

public:
	struct Elem
	{
		bool highLimitSet;
		bool lowLimitSet;
		INT64 highLimit;
		INT64 lowLimit;
		Elem()
			: highLimitSet(false)
			, lowLimitSet(false)
			, highLimit(PINT64_MAX)
			, lowLimit(PINT64_MIN)
		{ }			
	};

	// configs
	bool checkEnabled;
	bool checkWithException;
	bool checkUnparsedContent;
	PStringMap<Elem> limits;
	PStringSet excludedMsgs;
	mutable INT32 numMsgCheckErrors;

public: // interface
	bool isValidationEnabled(const char* msgName) const override
	{
		return isEligible(msgName);
	}
	bool hasLimitDefined(const char* /*msgName*/) const override
	{
		// this function is for performance purpose only - it does not have to be accurate, as long as it doesn't return false if you have the limit for the message.
		return limits.size() != 0;
	}
	bool checkParamLimit(INT64 value, const char* fieldKey, const char* file /*__FILE__*/, INT32 line /*__LINE__*/) const override
	{
		return checkLimitRaw(value, fieldKey, true/*fromATF*/, file, line);
	}
	bool checkParamLimitU(UINT64 value, const char* fieldKey, const char* file /*__FILE__*/, INT32 line /*__LINE__*/) const override
	{	
		// to improve if really needed
		return checkParamLimit((INT64)value, fieldKey, file, line);
	}

public:
	MsgChecker()
	{
		clear();
	}
	void clear()
	{
		checkEnabled = (bool)DEFAULT_CHECKENABLED;
		checkWithException = (bool)DEFAULT_CHECKWITHEXCEPTION;
		checkUnparsedContent = (bool)DEFAULT_CHECKUNPARSEDCONTENT;
		limits.clear();
		excludedMsgs.clear();
		numMsgCheckErrors = 0;
	}

	bool isCheckEnabled() const { return checkEnabled; }
	bool isExceptionEnabled() const { return checkWithException; }
	bool isCheckUnparsedContentEnabled() const { return checkUnparsedContent; }
	bool isEligible(const char* msgName) const;	
	void PLogOptions() const;

	// initialDir & limitsFileName -> full path of limit file
	// iniFile & iniSectionName -> option parameters in the ini file
	void dynamicInit(const char* initialDir, const char* limitsFileName, const PIniFile& iniFile,  const char* iniSectionName="MsgLimitOptions");	
	void reportPerformance(CommMonitorLoggerObject* heLogger, CommMsgBody& reportBody, const char* prefix="") const
	{
		if (heLogger)
		{
			heLogger->compose(reportBody, PString(prefix).append("numMsgCheckErrors"), PString().appendInt(numMsgCheckErrors));			
		}
		numMsgCheckErrors = 0;
	}
	void reportPerformance(std::vector<PNameValue>& data) const
	{
		data.emplace_back("numMsgCheckErrors", PString().appendInt(numMsgCheckErrors));
		numMsgCheckErrors = 0;
	}
	
	void checkLimit(INT64 value, const char* msgName, const char* paramName, const char* file = nullptr, INT32 line = PINT32_MIN) const;

public:
	void compose(CommMsgBody& msg) const;
	void parse(CommMsgParser& parser);
};

struct MsgValidator
{	
private:	
	typedef std::function<bool(PString& unparsedFields)> validateFunc;

	static void validateMsg(const char* ns, const UINT32 msgId, const MsgChecker& checker, bool logUnknownMsg, const CommMsgBody& body/*debugging only*/, const validateFunc& func)
	{
		if(!checker.isCheckEnabled())
		{
			return;
		}
		try
		{
			PString unparsedFields;
			bool checked = func(unparsedFields);
			// isValidationEnabled() is added to enable suppressing error messages through reloadable configurations
			if (!checked && logUnknownMsg && checker.isValidationEnabled(PString().appendUint(msgId)))
			{
				PLog(MSG_CHECKER_ERROR_PREFIX "msgId %u not checked", msgId);
			}
			if (checker.isCheckUnparsedContentEnabled() && unparsedFields.length() > 0)
			{
				throw PError(PString("unparsed content: ").append(unparsedFields));
			}
			if (checked)
			{
				PTRACE5("msgValidated: %s:%u", ns, msgId);
			}			
		}
		catch(const PError& e)
		{
#ifdef _DEBUG
			// dump msg in log as base64 string
			PString bodyInBase64;
			CommSSLBase64::encode(bodyInBase64, body._readPtr(), body._size());
			PLog("msgdump: %s", bodyInBase64.c_str());
#endif // _DEBUG

			++checker.numMsgCheckErrors;
			PString errMsg(MSG_CHECKER_ERROR_PREFIX);
			errMsg				
				.append(" msgId: ")
				.appendUint(msgId)
				.append(", reason: ")
				.append(e.why());

			if(checker.isExceptionEnabled())
			{
				throw PError(errMsg);
			}
			else
			{
				PLog("%s", errMsg.c_str());
			}
		}
		catch(...)
		{
			++checker.numMsgCheckErrors;
			if(checker.isExceptionEnabled())
			{
				throw;
			}
			else
			{
				PLog(MSG_CHECKER_ERROR_PREFIX "msgId: %u", msgId);
			}
		}
	}

public:
	template<typename F>
	static void validateMsg(const char* ns, const UINT32 msgId, const CommMsgParser& parser_, const MsgChecker& checker, bool logUnknownMsg)
	{
		validateMsg(ns, msgId, checker, logUnknownMsg, parser_.getMsgBody(), [&] (PString& unparsedFields) {
			CommMsgParser parser(parser_);
			return F::validateProtocolMessage(ns, msgId, parser, &checker, unparsedFields);
		});
	}

	template<typename F>
	static void validateMsg(const char* ns, const UINT32 msgId, const CommMsgBody& body, const MsgChecker& checker, bool logUnknownMsg)
	{
		validateMsg<F>(ns, msgId, CommMsgParser(body), checker, logUnknownMsg);
	}

	template<typename F>
	static void validateJsonMsg(const char* ns, const UINT32 msgId, const CommMsgBody& body, const MsgChecker& checker, bool logUnknownMsg)
	{
		validateMsg(ns, msgId, checker, logUnknownMsg, body, [&] (PString& unparsedFields) {
			PString _jsonStr;
			Atf::_AtfCipwaConnection::_unwrapJSonStringFromMsgBody(body, _jsonStr);
			return F::validateProtocolMessageFromJSONString(ns, msgId, _jsonStr, &checker, unparsedFields);
		});
	}

#ifdef _DEBUG
	template<typename F>
	static void debugMsg(const char* ns, const UINT32 msgId, const char* inputInBase64, const MsgChecker& checker)
	{
		PBlock msgContent;
		CommSSLBase64::decode(msgContent, inputInBase64);
		CommMsgBody body;
		body.moveFrom(msgContent);
		validateMsg<F>(ns, msgId, body, checker, true/*logUnknownMsg*/);
	}
#endif // _DEBUG
};

#endif
