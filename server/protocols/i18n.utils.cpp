#include "i18n.utils.h"

#include "ppi18n_x.h"
#include "ppi18n_utf8.h"
#include "pplogfile.h"

void i18nAppend(PString& trg, const char* src)
{
	PStringX::StringParser parser;
	parser.init(src, 0);
	for	(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
	{
		PStringX::staticAppend(trg, s);
	}
}

void i18nAppendUint(PString& trg, unsigned int val)
{
	char buf[64];
	i18nAppend(trg, p_utoa(val, buf));
}

void i18nAppendInt(PString& trg, int val)
{
	char buf[64];
	i18nAppend(trg, p_itoa(val, buf));
}

void i18nAppendUint64(PString& trg, UINT64 val)
{
	char buf[64];
	i18nAppend(trg, p_u64toa(val, buf));
}

void i18nAppendInt64(PString& trg, INT64 val)
{
	char buf[64];
	i18nAppend(trg, p_i64toa(val, buf));
}

void i18nAppendChar(PString& trg, PUNICHAR c)
{
	PStringX::staticAppend(trg, c);
}

void i18nAppendChar(PString& trg, char c)
{
	PStringX::staticAppend(trg, (unsigned char)c);
}

bool i18nCut(PString& inOut, INT32 maxBytes, bool doLogIfCut) // returns true if cut
{
	if (!inOut.length() || inOut.length() <= maxBytes)
		return false;
	if (inOut.charAt(0) != _PStringX_Props::_utf8Prefix) // Win1252
	{
		if (doLogIfCut)
		{
			PTRACE("i18nCut %d->%d: orig(1252)='%s'", inOut.length(), maxBytes, inOut.c_str());
		}
		inOut.cut(maxBytes);
		return true;
	}
	// UTF8
	PStringX::StringParser parser;
	parser.init(inOut, 0);
	PString cutStr, testNextCharStr;
	cutStr.reserve(maxBytes);
	testNextCharStr.reserve(maxBytes);
	PUNICHAR_RET s = parser.nextChar();
	for	(; s > 0; s = parser.nextChar())
	{
		PStringX::staticAppend(testNextCharStr, s);
		if (testNextCharStr.length() > maxBytes)
			break;
		PStringX::staticAppend(cutStr, s);
	}
	if (s < 0)
	{
		PTRACE("i18nCut invalid utf8 orig.len=%d, cut.len=%d", inOut.length(), cutStr.length());
	}
	if (doLogIfCut)
	{
		PTRACE("i18nCut %d->%d: orig(utf8)='%s'", inOut.length(), cutStr.length(), inOut.c_str());
	}
	inOut = cutStr;
	return true;
}

void i18nAppendNormalized(PString& trg, const char* src, const I18nCharacterNormalizer& normalizer)
{
	PStringX::StringParser parser;
	parser.init(src, 0);
	for	(PUNICHAR_RET c = parser.nextChar(); c > 0; c = parser.nextChar())
	{
		if (normalizer.normalize(c) > 0)
			PStringX::staticAppend(trg, c);
	}
}

bool i18nIsNormalized(const char* src, const I18nCharacterNormalizer& normalizer)
{
	PStringX::StringParser parser;
	parser.init(src, 0);
	for	(PUNICHAR_RET c = parser.nextChar(); c > 0; c = parser.nextChar())
	{
		if (normalizer.normalize(c) < 0)
			return false;
	}
	return true;
}

// ATTENTION: the converted string is appended to ret
// Translate PStringX into PUtf8String
void i18nPsx_Utf8(PString& ret, const char* in)
{
	if (in == NULL || *in == 0) // PYR-32795
		return;
	ret.reserve(ret.length() + strlen(in) * 2);
	PStringX::StringParser parser;
	parser.init(in, 0);
	for(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
		PUtf8String::staticAppend(ret, s);
}

// ATTENTION: the converted string is appended to ret
// Translate PStringX into PUtf8String
// No premature optimization
void i18nPsx_Utf8NoReserve(PString& ret, const char* in)
{
	if (in == NULL || *in == 0) // PYR-32795
		return;
	PStringX::StringParser parser;
	parser.init(in, 0);
	for(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
		PUtf8String::staticAppend(ret, s);
}

// ATTENTION: the converted string is appended to ret
// Translate PStringX into PUtf8String xml encoded
void i18nPsx_Utf8_XmlEncoded(PString& ret, const char* in)
{
	if (in == NULL || *in == 0) // PYR-32795
		return;
	ret.reserve(ret.length() + strlen(in) * 2);
	PStringX::StringParser parser;
	parser.init(in, 0);
	for(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
	{
		switch( s )
		{
		case '&':
			ret.append( "&amp;" );
			break;
		case '<':
			ret.append( "&lt;" );
			break;
		case '>':
			ret.append( "&gt;" );
			break;
		case '\'':
			ret.append( "&apos;");
			break;
		case '\"':
			ret.append( "&quot;" );
			break;
		default:
			PUtf8String::staticAppend(ret, s);
			break;
		}
	}
}

// ATTENTION: the converted string is appended to ret
UINT32 i18nPsx_Utf8(PString& ret, const char* in, UINT32 maxLen)
{
	PStringX::StringParser parser;
	parser.init(in, 0);
	UINT32 len = 0;
	for(PUNICHAR_RET s = parser.nextChar(); s > 0 && len < maxLen; s = parser.nextChar(), ++len)
		PUtf8String::staticAppend(ret, s);

	return len;
}

// ATTENTION: the converted string is appended to ret
// Translate PUtf8String into PStringX
void i18nUtf8_Psx(PString& ret, const char* in)
{
	if (in == NULL || *in == 0) // PYR-32795
		return;
	ret.reserve(ret.length() + strlen(in) + 1);
	PUtf8String::StringParser parser;
	parser.init(in, 0);
	for(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
		PStringX::staticAppend(ret, s);
}

// ATTENTION: the converted string is appended to ret
bool i18nUtf8_Psx_validated( PString& ret, const char* in )
{
	PUtf8String::StringParser parser;
	parser.init(in, 0);
	PUNICHAR_RET s;
	for(s = parser.nextChar(); s > 0; s = parser.nextChar())
		PStringX::staticAppend(ret, s);
	return (s == 0);
}

// ATTENTION: the converted string is added to ret
// Translate PUtf8String into PUniString
void i18nUtf8_Uni(PUniString& ret, const char* in)
{
	PUtf8String::StringParser parser;
	parser.init(in, 0);
	for(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
		ret.append(static_cast< PUNICHAR >( s ));
}

// ATTENTION: the converted string is added to ret
// Translate PStringX into PUniString
void i18nPsx_Uni(PUniString& ret, const char* in)
{
	PStringX::StringParser parser;
	parser.init(in, 0);
	for(PUNICHAR_RET s = parser.nextChar(); s > 0; s = parser.nextChar())
		ret.append(static_cast< PUNICHAR >( s ));
}

void i18nSplitFullNameBySpaces(const char* fullName, vector<PString>& names)
{
	PStringX::StringParser parser(fullName, 0);
	PString oneName;
	for(;;)
	{
		PUNICHAR_RET s = parser.nextChar();
		if (s <= 0)
		{
			if (oneName.length())
			{
				names.push_back(oneName);
			}
			return;
		}
		if (s == ' ' || s == '\t')
		{
			if (oneName.length())
			{
				names.push_back(oneName);
			}
			oneName = "";
			continue;
		}
		PStringX::staticAppend(oneName, s);
	}
}

void i18nSplitFullNameByFirstSpace(const char* fullName, PString& firstName, PString& lastName)
{
	firstName = "";
	lastName = "";
	vector<PString> names;
	i18nSplitFullNameBySpaces(fullName, names);
	if (names.empty())
		return;
	firstName = names[0];
	for (UINT32 i = 1; i < names.size(); ++i)
	{
		if (lastName.length() > 0)
		{
			i18nAppendChar(lastName, ' ');
		}
		i18nAppend(lastName, names[i]);
	}
}
