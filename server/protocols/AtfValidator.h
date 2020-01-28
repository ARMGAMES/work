#ifndef PYR_ATF_FIELD_VALIDATOR_H
#define PYR_ATF_FIELD_VALIDATOR_H

#include "commmsgbody.h"
#include "commssl.h"
#include "commtime.h"

#define MAX_UNPARSEDFIELDINFO_BUFFERSIZE   2000000    // around 2 MB


//==============================================================================================
//                               AtfMsgChecker
//==============================================================================================

class AtfMsgChecker
{
public:
	virtual ~AtfMsgChecker() {}
	virtual bool isValidationEnabled(const char* msgName) const = 0;   // if not enabled, no any validation will be taken on this message.
	virtual bool hasLimitDefined(const char* msgName) const = 0;       // if false, only perform the normal atf limit check.
	// the following two functions are used for overriding atf embedded limit check.
	// the returned boolean value indicates whether the limit check has been performed from this function.
	// it does *not* mean whether the limit check is passed or not.
	// return true when the limit check is overriden, no further atf limit check on this field. 
	// return false when no check is performed in this function, continue atf limit check.
	virtual bool checkParamLimit(INT64 value, const char* fieldKey, const char* file /*__FILE__*/, INT32 line /*__LINE__*/) const = 0;
	virtual bool checkParamLimitU(UINT64 value, const char* fieldKey, const char* file /*__FILE__*/, INT32 line /*__LINE__*/) const = 0;
};


//==============================================================================================
//                               AtfMsgBodyParserView
//==============================================================================================

class AtfMsgBodyParserView : public CommMsgParser
{
	size_t startOffset;
	size_t endOffset;
public:
	AtfMsgBodyParserView(const CommMsgParser& parser_) : CommMsgParser(parser_)
	{
		size_t sz;
		skipMsgBody(sz);
		endOffset = CommMsgParser::_shift();
		startOffset = endOffset - sz;
		PASSERT(endOffset >= startOffset);
		shiftBack(sz);
	}
	size_t shiftInParent() const
	{
		return CommMsgParser::_shift();
	}
	bool parseEnded() const
	{
		return shiftInParent() == endOffset;
	}
	size_t msgBodyViewSize() const
	{
		return endOffset - startOffset;
	}
	size_t shiftInView() const
	{
		return shiftInParent() - startOffset;
	}
	bool isInViewScope() const
	{
		size_t t = shiftInParent();
		return t >= startOffset && t <= endOffset;  // when parsing ends, it moves to endOffset (still think it's in valid range)
	}
private:
	using CommMsgParser::getMsgBody;  // prohibited
	using CommMsgParser::_shift;      // prohibited, please explicitly use shiftInView() or shiftInParent()
};


//==============================================================================================
//                               AtfValidator
//==============================================================================================

class AtfValidator
{
public:
	static void validateInt(const char *structName, const char *fieldName, INT64 v, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		// no atf field restriction defined
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			checker->checkParamLimit(v, fieldKey, file, line);
		}
	}

	static void validateIntMin(const char *structName, const char *fieldName, INT64 v, INT64 min, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimit(v, fieldKey, file, line))
			{
				return;
			}
		}
		if(v < min)
		{
			PString err;
			err.append(structName).append('.').append(fieldName).append(" ").appendInt64(v).append(" is less than min value ").appendInt64(min).append(" {").append(file).append(" line ").appendInt(line).append("}");
			throw PParserError(err);
		}
	}

	static void validateIntMax(const char *structName, const char *fieldName, INT64 v, INT64 max, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimit(v, fieldKey, file, line))
			{
				return;
			}
		}
		if(v > max)
		{
			PString err;
			err.append(structName).append('.').append(fieldName).append(" ").appendInt64(v).append(" is greater than max value ").appendInt64(max).append(" {").append(file).append(" line ").appendInt(line).append("}");
			throw PParserError(err);
		}
	}

	static void validateIntRange(const char *structName, const char *fieldName, INT64 v, INT64 min, INT64 max, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimit(v, fieldKey, file, line))
			{
				return;
			}
		}
		if(v < min || v > max)
		{
			PString err;
			err.append(structName).append('.').append(fieldName).append(" ").appendInt64(v).append(" is not in range [").appendInt64(min).append(',').appendInt64(max).append(']').append(" {").append(file).append(" line ").appendInt(line).append("}");
			throw PParserError(err);
		}
	}

	static void validateUint(const char *structName, const char *fieldName, UINT64 v, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		// no atf field restriction defined
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			checker->checkParamLimitU(v, fieldKey, file, line);
		}
	}

	static void validateUintMin(const char *structName, const char *fieldName, UINT64 v, UINT64 min, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimitU(v, fieldKey, file, line))
			{
				return;
			}
		}
		if(v < min)
		{
			PString err;
			err.append(structName).append('.').append(fieldName).append(" ").appendUint64(v).append(" is less than min value ").appendUint64(min).append(" {").append(file).append(" line ").appendInt(line).append("}");
			throw PParserError(err);
		}
	}

	static void validateUintMax(const char *structName, const char *fieldName, UINT64 v, UINT64 max, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimitU(v, fieldKey, file, line))
			{
				return;
			}
		}
		if(v > max)
		{
			PString err;
			err.append(structName).append('.').append(fieldName).append(" ").appendUint64(v).append(" is greater than max value ").appendUint64(max).append(" {").append(file).append(" line ").appendInt(line).append("}");
			throw PParserError(err);
		}
	}

	static void validateUintRange(const char *structName, const char *fieldName, UINT64 v, UINT64 min, UINT64 max, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimitU(v, fieldKey, file, line))
			{
				return;
			}
		}
		if(v < min || v > max)
		{
			PString err;
			err.append(structName).append('.').append(fieldName).append(" ").appendUint64(v).append(" is not in range [").appendUint64(min).append(',').appendUint64(max).append(']').append(" {").append(file).append(" line ").appendInt(line).append("}");
			throw PParserError(err);
		}
	}


	static bool _checkDate(int year, int month, int day)
	{
		if(year == 0 || month == 0 || day == 0)
		{
			return true;   // usually init value
		}

		if(year < 1900 || year > 2099 ||
			month < 1 || month > 12 ||   // valid month 1 - 12
			day < 1 || day > 31)         // valid day 1 - 31
		{
			return false;
		}

		if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		{
			return day >= 1 && day <= 31; //-V560
		}
		else if(month == 4 || month == 6 || month == 9 || month == 11)
		{
			return day >= 1 && day <= 30; //-V560
		}

		if(month != 2 || day < 1 || day > 29)  // must be February //-V560
		{
			return false;
		}
		// check leap year
		if((year % 4) != 0)  // not divisible by 4, common year
		{
			return day <= 28;
		}
		else if((year % 100) != 0)  // divisible by 4, but not 100: leap year
		{
			return day <= 29; //-V547 // always true
		}
		else if((year % 400) != 0)  // divisible by 100, but not 400: common year
		{
			return day <= 28;
		}
		else  // leap year
		{
			return day <= 29; //-V547 // always true 
		}
	};


	static void validateSrvDateTime(const char *structName, const char *fieldName, const CommSrvDate& v, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(v.isNull() || _checkDate(v._year, v._month, v._day))
		{
			return;  // valid
		}
		// invalid value goes here
		char dateStrBuf[64];
		sprintf(dateStrBuf, "%04d-%02d-%02d", v._year, v._month, v._day);
		PString err;
		err.append(structName).append('.').append(fieldName).append(" ").append(dateStrBuf).append(" is not a valid SrvDate value").append(" {").append(file).append(" line ").appendInt(line).append("}");
		throw PParserError(err);
	}

	static void validateSrvDateTime(const char *structName, const char *fieldName, const CommSrvTime& v, const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(v.isNull())
		{
			return;  // null is a valid value
		}
		if(_checkDate(v._year, v._month, v._day) &&
			(v._hour >= 0 && v._hour <= 23) && //-V560
			(v._minute >= 0 && v._minute <= 59) && //-V560
			(v._second >= 0 && v._second <= 61) //&&  // C++ tm struct manual says it could be leap seconds, so the value is up to 61 //-V560
			//(v._wday >= 0 && v._wday <= 6) //-V560  // PYR-84084 comment out this line (not all protocols fill this field properly)
			)
		{
			return;  // valid
		}
		// invalid value goes here
		char dateStrBuf[64];
		sprintf(dateStrBuf, "%04d-%02d-%02d %02d:%02d:%02d w%d", v._year, v._month, v._day, v._hour, v._minute, v._second, v._wday);
		PString err;
		err.append(structName).append('.').append(fieldName).append(" ").append(dateStrBuf).append(" is not a valid SrvTime value").append(" {").append(file).append(" line ").appendInt(line).append("}");
		throw PParserError(err);
	}


	template<typename T, size_t SIZE>
	static void validateEnum(const char *structName, const char *fieldName, T v, const T (&enums)[SIZE], const AtfMsgChecker* checker, const char* file, INT32 line)
	{
		if(checker)
		{
			PString fieldKey;
			fieldKey.append(structName).append('.').append(fieldName);

			if(checker->checkParamLimit(v, fieldKey, file, line))
			{
				return;
			}
		}

		for(size_t i = 0; i < SIZE; ++i)  // linear search, assuming SIZE is a small number
		{
			if(enums[i] == v)
			{
				return;
			}
		}

		PString err;
		err.append(structName).append('.').append(fieldName).append(" ").appendInt64(v).append(" is not in the value list").append(" {").append(file).append(" line ").appendInt(line).append("}");
		throw PParserError(err);
	}

	static void validateEmbeddedMsgbodyParser(const char *desc, const AtfMsgBodyParserView& parser, size_t shiftExpectedInParent, const char* file, INT32 line)
	{
		size_t msgbodySize = parser.msgBodyViewSize();
		size_t shiftParsed = parser.shiftInParent();

		if(shiftParsed > shiftExpectedInParent)
		{
			PString err;
			err.append(desc).append(" ").append(" parse error: insufficient space: msgbody size=").appendUint64(msgbodySize).append(", absolute shift=").appendUint64(shiftParsed).append(", expected max shift=").appendUint64(shiftExpectedInParent).append(" {").append(file).append(" line ").appendInt(line).append("}");;
			throw PParserError(err);
		}
	}

	static void logUnparsedInfo(PString& buf, const char *descr, size_t unparsedBytes)
	{
		if(buf.length() < MAX_UNPARSEDFIELDINFO_BUFFERSIZE)
		{
			buf.append(descr).append(" (").appendUint64(unparsedBytes).append(" bytes) ");
		}
		else if(buf[buf.length() - 1] != '.')
		{
			buf.append("...");  // the last byte is '.'
		}
	}

	static void logUnparsedInfo(PString& buf, const char *descr, const CommMsgParser& parser)
	{
		logUnparsedInfo(buf, descr, parser.getMsgBody()._size() - parser._shift());
	}

	static void logUnparsedInfo(PString& buf, const char *descr, const AtfMsgBodyParserView& parser)
	{
		logUnparsedInfo(buf, descr, parser.msgBodyViewSize() - parser.shiftInView());
	}
};


class AtfCheckSum
{
public:
	static void makeChecksum(const PString& inStr, PString& outStr)
	{
		CommSSLChksumSHA256 sumSha256;
		sumSha256.addData((const BYTE*)inStr.c_str(), inStr.length());
		PBlock Sha256Chksum;
		sumSha256.chksum(Sha256Chksum);

		const BYTE *p = Sha256Chksum.ptr();
		for (size_t i = 0; i < Sha256Chksum.size() && i < 16; ++i, ++p)  // limit 32 bytes output
		{
			if (*p <= 0xF)
			{
				outStr.append('0');
			}
			outStr.appendHexInt(*p);
		}
	}
};


#endif

