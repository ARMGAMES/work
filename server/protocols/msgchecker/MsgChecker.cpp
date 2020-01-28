#include "MsgChecker.h"
#include "commmsgbody.h"
#include "dbm.h"
#include "shared.h"
#include "composecolls.h"

#define NO_LIMIT_STRING "*"

void MsgChecker::dynamicInit(const char* initialDir, const char* limitsFileName, const PIniFile& iniFile,  const char* iniSectionName)
{
	PLog("msgChecker: readIni section=%s file=%s", iniSectionName, limitsFileName);
	clear();

	PString limitsFileDir;
	const PIniFile::Section* sec = iniFile.getSection(iniSectionName);
	if (sec)
	{
		checkEnabled = sec->getIntProperty("checkEnabled", DEFAULT_CHECKENABLED) != 0;
		checkWithException = sec->getIntProperty("checkWithException", DEFAULT_CHECKWITHEXCEPTION) != 0;
		checkUnparsedContent = sec->getIntProperty("checkUnparsedContent", DEFAULT_CHECKUNPARSEDCONTENT) != 0;		
		limitsFileDir = sec->getProperty("limitsFileDir", "");
		const char* excludeListStr = sec->getProperty("excludeList", "");
		readPStringSetFromString(excludeListStr, excludedMsgs, ',', true/*toUpper*/, true/*trim*/);
	}
	else
	{
		PLog("msgChecker: readSettings - section [%s] not found", iniSectionName);
	}

	if (checkEnabled)
	{			
		PString fullPath(limitsFileDir.length()? limitsFileDir.c_str() : initialDir);		
		fullPath.append(limitsFileName);
		readLimits(fullPath);
	}	
	PLogOptions();
}

void MsgChecker::readLimits(const char* limitsFileName)
{
	PLog("msgChecker: read limits from '%s'", limitsFileName);
	PIniFile iniFile;
	try
	{
		iniFile.load(limitsFileName);
	}
	catch (const PError& e)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "msgChecker: readSettings - could not load '%s' - %s, limit check is disabled", limitsFileName, e.why());
		checkEnabled = false;
		return;
	}
	const PIniFile::Section* sec = iniFile.getSection("MsgLimits");
	if (!sec)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "msgChecker: readSettings - section [MsgLimits] not found, limit check is disabled");
		checkEnabled = false;
		return;
	}

	for (const auto& iniItem : sec->items)
	{
		Elem elem;
		PString nameUp(iniItem.name);
		nameUp.toUpper();
		vector<PString> strVec;
		readStrVectorFromString(iniItem.value, strVec, ',', false/*keepEmpty*/, true/*trim*/);

		if (strVec.size() >= 1)
		{
			if (!strVec[0].equals(NO_LIMIT_STRING))
			{
				elem.highLimitSet = true;
				elem.highLimit = p_atoi64(strVec[0]);
			}
		}
		if (strVec.size() >= 2)
		{
			if (!strVec[1].equals(NO_LIMIT_STRING))
			{
				elem.lowLimitSet = true;
				elem.lowLimit = p_atoi64(strVec[1]);
			}
		}
		if (elem.highLimitSet && elem.lowLimitSet && elem.lowLimit > elem.highLimit)
		{
			PLog(MSG_CHECKER_ERROR_PREFIX" %s - low limit %" PRId64 " > high limit %" PRId64, iniItem.name.c_str(), elem.lowLimit, elem.highLimit);
		}
		pair< PStringMap<Elem>::iterator, bool > result = limits.insert(nameUp, elem);
		if (!result.second)
		{
			PLog(MSG_CHECKER_ERROR_PREFIX" %s - repeated limit definition", iniItem.name.c_str());
		}
	}
}

void MsgChecker::PLogOptions() const
{
	PString tmpStr;
	PLog("msgChecker: checkEnabled = %s", checkEnabled? "y" : "n");
	PLog("msgChecker: checkWithException = %s", checkWithException? "y" : "n");
	PLog("msgChecker: checkUnparsedContent = %s", checkUnparsedContent? "y" : "n");	
	PLog("msgChecker: excludedMsgs[%zu]: %s", excludedMsgs.size(), strSetToStr(tmpStr, excludedMsgs));


	PLog("msgChecker: %zu limits", limits.size());
	for (const auto& item : limits)
	{
		PString limitStr;
		if (item.second.highLimitSet)
		{
			limitStr.append("high: ");
			limitStr.appendInt64(item.second.highLimit);
		}
		if (item.second.lowLimitSet)
		{
			if (limitStr.length())
			{
				limitStr.append(", ");
			}
			limitStr.append("low: ");
			limitStr.appendInt64(item.second.lowLimit);
		}
		if (!item.second.lowLimitSet && !item.second.highLimitSet)
		{
			limitStr.append("no limits set");
		}
		PLog("msgChecker: limit %s (%s)", item.first, limitStr.c_str());
	}
}

bool MsgChecker::isEligible(const char* msgName) const
{
	if (!checkEnabled)
	{
		return false;
	}
	PString key(msgName);
	key.toUpper();
	return (excludedMsgs.find(key) == excludedMsgs.end());
}

bool MsgChecker::checkLimitRaw(INT64 value, const char* fieldKey, bool fromATF, const char* file, INT32 line) const
{       
	PString key(fieldKey);
	key.toUpper();
	auto it = limits.find(key);
	if (it == limits.end())
	{
		if (!fromATF)
		{
			PLog(MSG_CHECKER_ERROR_PREFIX"%s: undefined limit", key.c_str());
		}
		return false; // no limit found
	}

	const Elem& elem = it->second;
	bool lowLimitReached = (elem.lowLimitSet && value < elem.lowLimit);
	bool highLimitReached = (elem.highLimitSet && value > elem.highLimit);
	if (!lowLimitReached && !highLimitReached)
	{
		return true; // no limit violation
	}

	// create error string
	PString strErr(MSG_CHECKER_ERROR_PREFIX);
	strErr.append(key).append(": ").appendInt64(value);
	if (lowLimitReached)
	{
		strErr.append(" < ").appendInt64(elem.lowLimit);
	}
	else
	{
		strErr.append(" > ").appendInt64(elem.highLimit);
	}
	if (file != nullptr)
	{
		strErr.append(" : ").append(file);
	}
	if (line != PINT32_MIN)
	{
		strErr.append(" : ").appendInt(line);
	}

	if (fromATF)
	{
		throw PError(strErr); 
	}
	else
	{
		++numMsgCheckErrors;
		if (checkWithException)
		{
			throw PError(strErr); 
		}
		else
		{
			PLog("%s", strErr.c_str());
		}
	}
	return true;
}

void MsgChecker::checkLimit(INT64 value, const char* msgName, const char* paramName, const char* file, INT32 line) const	
{
	if (!isEligible(msgName))
	{
		return; // msg excluded from limit check
	}

	PString key(msgName);
	key.append('.').append(paramName);
	checkLimitRaw(value, key, false/*fromATF*/, file, line);
}

void MsgChecker::compose(CommMsgBody& msg) const
{
	msg
		.composeBOOL(checkEnabled)
		.composeBOOL(checkWithException)
		.composeBOOL(checkUnparsedContent)
		.composeINT32((INT32)excludedMsgs.size())
		;
	for (const char* item : excludedMsgs)
	{
		msg.composeString(item);
	}
	msg.composeINT32(limits.size());
	for (const auto& item : limits)
	{
		CommMsgBody msg1;
		msg1.composeString(item.first);
		msg1.composeBOOL(item.second.lowLimitSet);
		msg1.composeINT64(item.second.lowLimit);
		msg1.composeBOOL(item.second.highLimitSet);
		msg1.composeINT64(item.second.highLimit);
		msg.composeMsgBody(msg1);
	}
}
void MsgChecker::parse(CommMsgParser& parser)
{
	clear();	
	parser
		.parseBOOL(checkEnabled)
		.parseBOOL(checkWithException)
		.parseBOOL(checkUnparsedContent)
		;
	INT32 excludedMsgSize;
	parser.parseINT32(excludedMsgSize);
	for (int i = 0; i < excludedMsgSize; ++i)
	{
		const char* str;
		parser.parseString(str);
		excludedMsgs.insert(str);
	}
	INT32 size;
	parser.parseINT32(size);
	for (INT32 i = 0; i < size; ++i)
	{
		CommMsgBody msg;
		parser.parseMsgBody(msg);
		CommMsgParser parser2(msg);
		const char *elemName;
		Elem elem;
		parser2.parseString(elemName);
		parser2.parseBOOL(elem.lowLimitSet);
		parser2.parseINT64(elem.lowLimit);
		parser2.parseBOOL(elem.highLimitSet);
		parser2.parseINT64(elem.highLimit);
		limits.insert(elemName, elem);		
	}
#ifdef _DEBUG
	PLogOptions();
#endif
}

