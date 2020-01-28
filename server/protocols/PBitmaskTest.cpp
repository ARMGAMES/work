#include "PBitmask.h"
#include "pperror.h"
#include "commmsgbody.h"
#include <random>
#include <functional>
#include <iostream>
#include <memory>

using namespace std;

namespace
{

typedef unsigned char(*bitwiseFunc)(unsigned char, unsigned char);

unsigned char bitwiseAnd(unsigned char l1, unsigned char l2)
{
	return (l1 & l2);
}

unsigned char bitwiseAndNot(unsigned char l1, unsigned char l2)
{
	return (l1 & ~l2);
}

unsigned char bitwiseOr(unsigned char l1, unsigned char l2)
{
	return (l1 | l2);
}

unsigned char bitwiseXor(unsigned char l1, unsigned char l2)
{
	return (l1 ^ l2);
}

unsigned char hexDigitToByte(char c, short* isNull)
{
	if ('0' <= c && c <= '9')
	{
		return c - '0';
	}
	if ('A' <= c && c <= 'F')
	{
		return 10 + c - 'A';
	}
	if ('a' <= c && c <= 'f')
	{
		return 10 + c - 'a';
	}
	if (c)
	{
		*isNull = -1;
	}
	return 0;
}

char byteToHexDigit(unsigned char c)
{
	unsigned char c1 = (unsigned char)(c & (unsigned char)0x0F);
	return (0 <= c1 && c1 <= 9 ? '0' + c1 : 'A' + c1 - 10);
}

unsigned char shiftHelper(char p, char c, int numBits, short multiDigit, short* isNull)
{
	if (numBits > 0)
	{
		return (hexDigitToByte(c, isNull) >> numBits) | (unsigned char)((((~((unsigned char)0xF << numBits) & 0xF) & hexDigitToByte(p, isNull)) << (4 - numBits)) & 0xF);
	}
	if (numBits < 0)
	{
		numBits *= -1;
		unsigned char hb = (multiDigit ? ((hexDigitToByte(c, isNull) << numBits) & (unsigned char)0xF0) : 0);
		return (hb | (((hexDigitToByte(c, isNull) << numBits) & 0xF) | (((((unsigned char)0xF << (4 - numBits)) & 0xF) & hexDigitToByte(p, isNull)) >> (4 - numBits))));
	}
	return hexDigitToByte(c, isNull);
}

void bitmask_ShiftLeft(const char* bitmaskStr, int* numBits, char* res)
{
	size_t i = 0;
	int bitsToShift, digitsToAdd, hasNonZeroDigit = 0;
	short resIsNull = 0;
	if (!bitmaskStr || !numBits || !res ||  *numBits < 0)
	{
		return;
	}
	digitsToAdd = *numBits / 4;
	bitsToShift = -1 * (*numBits % 4);
	while (*bitmaskStr)
	{
		unsigned char b = shiftHelper(*(bitmaskStr + 1), *bitmaskStr, bitsToShift, !i, &resIsNull);
		if (resIsNull)
		{
			return;
		}
		if (!i)
		{
			char ch = byteToHexDigit((b & (unsigned char)0xF0) >> 4);
			if (ch != '0')
			{
				*res++ = ch;
				hasNonZeroDigit = 1;
			}
			b &= (unsigned char)0xF;
		}
		*res = byteToHexDigit(b);
		if (*res != '0' || hasNonZeroDigit)
		{
			++res;
			hasNonZeroDigit = 1;
		}
		++bitmaskStr;
		++i;
	}
	if (hasNonZeroDigit)
	{
		for (i = 0; i < digitsToAdd; ++i)
		{
			*res++ = '0';
		}
	}
	else
	{
		*res++ = '0';
	}
	if (*res == '0')
	{
		++res;
	}
	*res = 0;
}

void bitmask_ShiftRight(const char* bitmaskStr, int* numBits, char* res)
{
	size_t i = 0, n, len;
	int bitsToShift, digitsToDrop, hasNonZeroDigit = 0;
	char p = 0;
	short resIsNull = 0;
	if (!bitmaskStr || !numBits || !res || *numBits < 0)
	{
		return;
	}
	digitsToDrop = *numBits / 4;
	len = strlen(bitmaskStr);
	n = (digitsToDrop < len ? len - digitsToDrop : 0);
	bitsToShift = *numBits % 4;
	while (i++ < n && *bitmaskStr)
	{
		unsigned char b = shiftHelper(p, *bitmaskStr, bitsToShift, 0, &resIsNull);
		if (resIsNull)
		{
			return;
		}
		*res = byteToHexDigit(b);
		if (*res != '0' || hasNonZeroDigit)
		{
			++res;
			hasNonZeroDigit = 1;
		}
		p = *bitmaskStr++;
	}
	if (n)
	{
		if (*res == '0')
		{
			++res;
		}
	}
	else
	{
		*res++ = '0';
	}
	*res = 0;
}

void bitMaskHelper(const char* s1, const char* s2, char* res, bitwiseFunc bfunc, int resultSizeIsMaxOfArgs)
{
	size_t len1, len2, len, i;
	unsigned char l1, l2, l;
	short hasNonZeroDigit = 0;
	short resIsNull = 0;
	const char *origS1 = s1, *origS2 = s2;
	if (!s1 || !s2)
	{
		return;
	}
	while (*s1 == '0')
	{
		++s1;
	}
	if (!*s1 && s1 != origS1)
	{
		--s1;
	}
	while (*s2 == '0')
	{
		++s2;
	}
	if (!*s2 && s2 != origS2)
	{
		--s2;
	}
	len1 = strlen(s1);
	len2 = strlen(s2);
	if (resultSizeIsMaxOfArgs)
	{
		if (len1 >= len2)
		{
			len = len2;
			for (size_t i = 0; i < len1 - len2; ++i, ++s1)
			{
				if (*s1)
				{
					*res++ = *s1;
					hasNonZeroDigit = 1;
				}
			}
		}
		else
		{
			len = len1;
			if (resultSizeIsMaxOfArgs == 1)
			{
				for (size_t i = 0; i < len2 - len1; ++i, ++s2)
				{
					if (*s2)
					{
						*res++ = *s2;
						hasNonZeroDigit = 1;
					}
				}
			}
			else
			{
				s2 += (len2 - len1);
			}
		}
	}
	else
	{
		if (len1 >= len2)
		{
			len = len2;
			s1 += (len1 - len2);
		}
		else
		{
			len = len1;
			s2 += (len2 - len1);
		}
	}
	for (i = 0; i < len; ++i, ++s1, ++s2)
	{
		l1 = hexDigitToByte(*s1, &resIsNull);
		if (resIsNull)
		{
			return;
		}
		l2 = hexDigitToByte(*s2, &resIsNull);
		if (resIsNull)
		{
			return;
		}
		l = bfunc(l1, l2);
		*res = byteToHexDigit(l);
		if (*res != '0' || hasNonZeroDigit)
		{
			++res;
			hasNonZeroDigit = 1;
		}
	}
	if (!hasNonZeroDigit)
	{
		*res++ = '0';
	}
	*res = 0;
}

void bitmask_And(const char* bitmaskStr1, const char* bitmaskStr2, char* res)
{
	bitMaskHelper(bitmaskStr1, bitmaskStr2, res, bitwiseAnd, 0);
}

void bitmask_AndNot(const char* bitmaskStr1, const char* bitmaskStr2, char* res)
{
	bitMaskHelper(bitmaskStr1, bitmaskStr2, res, bitwiseAndNot, 2);
}

void bitmask_Or(const char* bitmaskStr1, const char* bitmaskStr2, char* res)
{
	bitMaskHelper(bitmaskStr1, bitmaskStr2, res, bitwiseOr, 1);
}

void bitmask_Xor(const char* bitmaskStr1, const char* bitmaskStr2, char* res)
{
	bitMaskHelper(bitmaskStr1, bitmaskStr2, res, bitwiseXor, 1);
}

template<size_t NUM_OF_BITS>
class PBitmaskUnitTest
{
	static constexpr size_t totalLength = NUM_OF_BITS / 64;

	static void fitString(char* res)
	{
		INT32 totalPBitmaskBytes = PBitmaskImpl<NUM_OF_BITS>::size() / 8;
		size_t strLen = strlen(res);
		// check if total bytes required for string representation not greater than total bytes in PBitmaskImpl<NUM_OF_BITS>
		if (((strLen + 1) / 2) > totalPBitmaskBytes)
		{
			INT32 totalStrLen = totalPBitmaskBytes * 2;
			strcpy(res, &res[strLen - totalStrLen]);
		}
	}

	static PString longestRepresentableString(const char* s)
	{
		char buf[4096];
		assert(strlen(s) < sizeof(buf));
		strcpy(buf, s);
		fitString(buf);
		return buf;
	}

	static bool testDefCtor()
	{
		UINT64 zero64 = 0;
		PBitmaskImpl<NUM_OF_BITS> b;
		PString hexStr;
		b.toDbString(hexStr);
		return hexStr.equalsIgnoreCase("0");
	}

	static bool testUintCtor()
	{
		UINT64 u = 17;
		PBitmaskImpl<NUM_OF_BITS> b = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(u);
		PString hexStr;
		b.toDbString(hexStr);
		char buf[33];
		snprintf(buf, sizeof(buf), "%llx", u);
		return hexStr.equalsIgnoreCase(buf);
	}

	static bool testAndNot()
	{
		PString s1 = longestRepresentableString("1234567890ABCDEF"), s2 = longestRepresentableString("FEDCBA0987654321");
		PBitmaskImpl<NUM_OF_BITS> b1(s1), b2(s2);
		PBitmaskImpl<NUM_OF_BITS> b = andNot(b1, b2);
		char res[1024];
		memset(res, 0, sizeof(res));
		bitmask_AndNot(s1, s2, res);
		PBitmaskImpl<NUM_OF_BITS> eb(res);
		if (b != eb)
		{
			return false;
		}

		s2 = longestRepresentableString("FEDCBA0987654321FEDCBA0987654321");
		b1 = PBitmaskImpl<NUM_OF_BITS>(s1);
		b2 = PBitmaskImpl<NUM_OF_BITS>(s2);
		b = andNot(b1, b2);
		memset(res, 0, sizeof(res));
		bitmask_AndNot(s1, s2, res);
		eb = PBitmaskImpl<NUM_OF_BITS>(res);
		if (b != eb)
		{
			return false;
		}

		swap(s1, s2);
		b1 = PBitmaskImpl<NUM_OF_BITS>(s1);
		b2 = PBitmaskImpl<NUM_OF_BITS>(s2);
		b = andNot(b1, b2);
		memset(res, 0, sizeof(res));
		bitmask_AndNot(s1, s2, res);
		eb = PBitmaskImpl<NUM_OF_BITS>(res);
		if (b != eb)
		{
			return false;
		}
		return true;
	}

	static bool testGetLower32()
	{
		PBitmaskImpl<NUM_OF_BITS> b1 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(17);
		if (b1.getLower32() != 17)
		{
			return false;
		}

		const UINT64 max64 = static_cast<UINT64>(~0);
		const UINT32 max32 = static_cast<UINT32>(~0);
		PBitmaskImpl<NUM_OF_BITS> b2 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(max64);
		if (b2.getLower32() != max32)
		{
			return false;
		}

		return true;
	}

	static bool testGetLower64()
	{
		PBitmaskImpl<NUM_OF_BITS> b1 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(17);
		if (b1.getLower64() != 17)
		{
			return false;
		}

		const UINT64 max64 = static_cast<UINT64>(~0);
		PBitmaskImpl<NUM_OF_BITS> b2 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(max64);
		if (b2.getLower64() != max64)
		{
			return false;
		}

		return true;
	}

	static bool testComposeParse()
	{
		CommMsgBody body;
		PBitmaskImpl<NUM_OF_BITS> b;
		body.composeObject(b);
		CommMsgParser parser(body);
		PBitmaskImpl<NUM_OF_BITS> b1;
		parser.parseObject(b1);
		if (b != b1)
		{
			return false;
		}

		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			vector<UINT64> nums(l, 0);
			for (UINT64 j = 0; j < l; ++j)
			{
				nums[j] = mtRand();
			}
			PASSERT(nums.size() == l);
			UINT64 bufSize = 16 * l + 1;
			unique_ptr<char[]> buf(new char[bufSize]);
			int nw = 0;
			for (UINT64 j = 0; j < l; ++j)
			{
				if (j)
				{
					nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
				}
				else
				{
					nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
				}
			}
			PBitmaskImpl<NUM_OF_BITS> b2(buf.get());
			CommMsgBody body;
			body.composeObject(b2);
			CommMsgParser parser(body);
			PBitmaskImpl<NUM_OF_BITS> b3;
			parser.parseObject(b3);
			if (b2 != b3)
			{
				return false;
			}
		}

		return true;
	}

	static bool testToStringFromString()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			vector<UINT64> nums(l, 0);
			for (UINT64 j = 0; j < l; ++j)
			{
				nums[j] = mtRand();
			}
			PASSERT(nums.size() == l);
			UINT64 bufSize = 16 * l + 1;
			unique_ptr<char[]> buf(new char[bufSize]);
			int nw = 0;
			for (UINT64 j = 0; j < l; ++j)
			{
				if (j)
				{
					nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
				}
				else
				{
					nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
				}
			}
			PBitmaskImpl<NUM_OF_BITS> b1(buf.get());
			PString hexStr;
			b1.toDbString(hexStr);
			PBitmaskImpl<NUM_OF_BITS> b2(hexStr.c_str());
			if (b1 != b2)
			{
				return false;
			}
		}
		return true;
	}

	static bool testNullStrCtor()
	{
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b(nullptr);
			return false;
		}
		catch (const PError&)
		{
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	static bool testEmptyStrCtor()
	{
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b("");
			return false;
		}
		catch (const PError&)
		{
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	static bool testValidStrCtor()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b(buf.get());
				PString hexStr;
				b.toDbString(hexStr);
				if (!hexStr.equalsIgnoreCase(buf.get()))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testValidStrAssign()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b;
				b.fromDbString(buf.get());
				PString hexStr;
				b.toDbString(hexStr);
				if (!hexStr.equalsIgnoreCase(buf.get()))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testEmptyStrAssign()
	{
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString("");
			return false;
		}
		catch (const PError&)
		{
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	static bool testNullStrAssign()
	{
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString(nullptr);
			return false;
		}
		catch (const PError&)
		{
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	static bool testValidStrFromString()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b;
				b.fromDbString(buf.get());
				PString hexStr;
				b.toDbString(hexStr);
				if (!hexStr.equalsIgnoreCase(buf.get()))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testEmptyStrFromString()
	{
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString("");
			return false;
		}
		catch (const PError&)
		{
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	static bool testNullStrFromString()
	{
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString(nullptr);
			return false;
		}
		catch (const PError&)
		{
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	static bool testInvalidStrCtor()
	{
		bool ok = true;
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b("G234567890ABCDEF1234567890ABCDEF");
			ok = false;
		}
		catch (const PError&)
		{
		}

		try
		{
			PBitmaskImpl<NUM_OF_BITS> b("23456789GCDEF");
			ok = false;
		}
		catch (const PError&)
		{
		}

		try
		{
			PBitmaskImpl<NUM_OF_BITS> b("23456789GCDEG");
			ok = false;
		}
		catch (const PError&)
		{
		}
		return ok;
	}

	static bool testInvalidStrFromString()
	{
		bool ok = true;
		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString("G234567890ABCDEF1234567890ABCDEF");
			ok = false;
		}
		catch (const PError&)
		{
		}

		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString("23456789GCDEF");
			ok = false;
		}
		catch (const PError&)
		{
		}

		try
		{
			PBitmaskImpl<NUM_OF_BITS> b;
			b.fromDbString("23456789GCDEG");
			ok = false;
		}
		catch (const PError&)
		{
		}
		return ok;
	}

	static bool testCopyCtor()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b1(buf.get());
				PBitmaskImpl<NUM_OF_BITS> b2(b1);
				if (b1 != b2)
				{
					return false;
				}
				PString hexStr1, hexStr2;
				b1.toDbString(hexStr1);
				b2.toDbString(hexStr2);
				if (!hexStr1.equalsIgnoreCase(hexStr2))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testMoveCtor()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b1(buf.get());
				PBitmaskImpl<NUM_OF_BITS> b2(move(b1));
				PString hexStr;
				b2.toDbString(hexStr);
				if (!hexStr.equalsIgnoreCase(buf.get()))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testAssignmentOperator()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b1(buf.get()), b2;
				b2 = b1;
				if (b1 != b2)
				{
					return false;
				}
				PString hexStr1, hexStr2;
				b1.toDbString(hexStr1);
				b2.toDbString(hexStr2);
				if (!hexStr1.equalsIgnoreCase(hexStr2))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testMoveAssignmentOperator()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (UINT64 i = 0; i < (UINT64)100; ++i)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b1(buf.get()), b2;
				b2 = move(b1);
				PString hexStr;
				b2.toDbString(hexStr);
				if (!hexStr.equalsIgnoreCase(buf.get()))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testBitwiseOperatorHelperInternal2(const PBitmaskImpl<NUM_OF_BITS>& b, const vector<UINT64>& nums1, const vector<UINT64>& nums2, const function<UINT64(UINT64, UINT64)>& bitwiseFunc)
	{
		UINT64 l = max(nums1.size(), nums2.size());
		vector<UINT64> nums(l, 0);
		auto rit1 = nums1.crbegin(), rit2 = nums2.crbegin();
		for (UINT64 j = nums.size() - 1; (rit1 != nums1.crend() || rit2 != nums2.crend()); --j)
		{
			UINT64 n1 = 0, n2 = 0;
			if (rit1 != nums1.crend())
			{
				n1 = *rit1;
				++rit1;
			}
			if (rit2 != nums2.crend())
			{
				n2 = *rit2;
				++rit2;
			}
			nums[j] = bitwiseFunc(n1, n2);
		}

		UINT64 ebufSize = 16 * l + 1;
		unique_ptr<char[]> ebuf(new char[ebufSize]);
		int nw = 0;
		for (UINT64 j = 0; j < l; ++j)
		{
			if (j)
			{
				nw += snprintf(ebuf.get() + nw, ebufSize - nw, "%016llx", nums[j]);
			}
			else
			{
				nw += snprintf(ebuf.get() + nw, ebufSize - nw, "%llx", nums[j]);
			}
		}
		PBitmaskImpl<NUM_OF_BITS> eb(ebuf.get());
		return (eb == b);
	}

	static bool testBitwiseOperatorHelperInternal(bool useMemberOperator, const PBitmaskImpl<NUM_OF_BITS>& b1, const PBitmaskImpl<NUM_OF_BITS>& b2, const vector<UINT64>& nums1, const vector<UINT64>& nums2, const function<UINT64(UINT64, UINT64)>& bitwiseFunc, const function<PBitmaskImpl<NUM_OF_BITS>(const PBitmaskImpl<NUM_OF_BITS>&, const PBitmaskImpl<NUM_OF_BITS>&)>& bitmaskFunc)
	{
		PBitmaskImpl<NUM_OF_BITS> b;
		if (useMemberOperator)
		{
			b = b1;
			bitmaskFunc(b, b2);
		}
		else
		{
			b = bitmaskFunc(b1, b2);
		}
		return testBitwiseOperatorHelperInternal2(b, nums1, nums2, bitwiseFunc);
	}

	static bool testBitwiseOperatorHelper(bool useMemberOperator, const function<UINT64(UINT64, UINT64)>& bitwiseFunc, const function<PBitmaskImpl<NUM_OF_BITS>(const PBitmaskImpl<NUM_OF_BITS>&, const PBitmaskImpl<NUM_OF_BITS>&)>& bitmaskFunc)
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l1 = 1; l1 <= totalLength; ++l1)
		{
			for (UINT64 l2 = 1; l2 < l1; ++l2)
			{
				vector<UINT64> nums1(l1, 0);
				for (UINT64 j = 0; j < l1; ++j)
				{
					nums1[j] = mtRand();
				}
				PASSERT(nums1.size() == l1);
				UINT64 bufSize1 = 16 * l1 + 1;
				unique_ptr<char[]> buf1(new char[bufSize1]);
				int nw = 0;
				for (UINT64 j = 0; j < l1; ++j)
				{
					if (j)
					{
						nw += snprintf(buf1.get() + nw, bufSize1 - nw, "%016llx", nums1[j]);
					}
					else
					{
						nw += snprintf(buf1.get() + nw, bufSize1 - nw, "%llx", nums1[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b1(buf1.get());

				vector<UINT64> nums2(l2, 0);
				for (UINT64 j = 0; j < l2; ++j)
				{
					nums2[j] = mtRand();
				}
				PASSERT(nums2.size() == l2);
				UINT64 bufSize2 = 16 * l2 + 1;
				unique_ptr<char[]> buf2(new char[bufSize2]);
				buf2[0] = 0;
				nw = 0;
				for (UINT64 j = 0; j < l2; ++j)
				{
					if (j)
					{
						nw += snprintf(buf2.get() + nw, bufSize2 - nw, "%016llx", nums2[j]);
					}
					else
					{
						nw += snprintf(buf2.get() + nw, bufSize2 - nw, "%llx", nums2[j]);
					}
				}
				PBitmaskImpl<NUM_OF_BITS> b2(buf2.get());

				if (!testBitwiseOperatorHelperInternal(useMemberOperator, b1, b2, nums1, nums2, bitwiseFunc, bitmaskFunc))
				{
					return false;
				}

				if (!testBitwiseOperatorHelperInternal(useMemberOperator, b2, b1, nums2, nums1, bitwiseFunc, bitmaskFunc))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testMemberBitwiseOrEqualOperator()
	{
		return testBitwiseOperatorHelper(true, [](UINT64 n1, UINT64 n2) {return n1 | n2;}, [](const PBitmaskImpl<NUM_OF_BITS>& res, const PBitmaskImpl<NUM_OF_BITS>& op) {const_cast<PBitmaskImpl<NUM_OF_BITS>&>(res) |= op; return res;});
	}

	static bool testMemberBitwiseXorEqualOperator()
	{
		return testBitwiseOperatorHelper(true, [](UINT64 n1, UINT64 n2) {return n1 ^ n2;}, [](const PBitmaskImpl<NUM_OF_BITS>& res, const PBitmaskImpl<NUM_OF_BITS>& op) {const_cast<PBitmaskImpl<NUM_OF_BITS>&>(res) ^= op; return res;});
	}

	static bool testMemberBitwiseAndEqualOperator()
	{
		return testBitwiseOperatorHelper(true, [](UINT64 n1, UINT64 n2) {return n1 & n2;}, [](const PBitmaskImpl<NUM_OF_BITS>& res, const PBitmaskImpl<NUM_OF_BITS>& op) {const_cast<PBitmaskImpl<NUM_OF_BITS>&>(res) &= op; return res;});
	}

	static bool testGlobalBitwiseOrOperator()
	{
		return testBitwiseOperatorHelper(false, [](UINT64 n1, UINT64 n2) {return n1 | n2;}, [](const PBitmaskImpl<NUM_OF_BITS>& b1, const PBitmaskImpl<NUM_OF_BITS>& b2) {return b1 | b2;});
	}

	static bool testGlobalBitwiseXorOperator()
	{
		return testBitwiseOperatorHelper(false, [](UINT64 n1, UINT64 n2) {return n1 ^ n2;}, [](const PBitmaskImpl<NUM_OF_BITS>& b1, const PBitmaskImpl<NUM_OF_BITS>& b2) {return b1 ^ b2;});
	}

	static bool testGlobalBitwiseAndOperator()
	{
		return testBitwiseOperatorHelper(false, [](UINT64 n1, UINT64 n2) {return n1 & n2;}, [](const PBitmaskImpl<NUM_OF_BITS>& b1, const PBitmaskImpl<NUM_OF_BITS>& b2) {return b1 & b2;});
	}

	static bool testEqualityOperatorInternal(bool testEquality)
	{
		PString in1Str = longestRepresentableString("FEDCBA9876543210");
		PString in2Str = longestRepresentableString("1234567890ABCDEF1234567890ABCDEF");
		PString in3Str = longestRepresentableString("FEDCBA9876543210FEDCBA9876543210");
		if (in1Str.length() >= in2Str.length())
		{
			in1Str.cut(in1Str.length() - 1);
		}

		PBitmaskImpl<NUM_OF_BITS> in1(in1Str);
		PBitmaskImpl<NUM_OF_BITS> in2(in2Str);
		PBitmaskImpl<NUM_OF_BITS> in3(in3Str);
		bool tb1 = (in1 < in2);
		if (!tb1) return false;
		bool tb2 = (in2 < in1);
		if (tb2) return false;
		bool tb3 = (in2 < in3);
		if (!tb3) return false;
		bool tb4 = (in3 < in2);
		if (tb4) return false;
		bool tb5 = (in1 < in3);
		if (!tb5) return false;
		bool tb6 = (in3 < in1);
		if (tb6) return false;

		PBitmaskImpl<NUM_OF_BITS> b1;
		if (testEquality)
		{
			if (!(b1 == b1))
			{
				return false;
			}
		}
		else
		{
			if (b1 != b1)
			{
				return false;
			}
		}

		PBitmaskImpl<NUM_OF_BITS> b4("1"), b5 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(1);
		if (testEquality)
		{
			if (!(b4 == b5) || !(b4 == b4) || !(b5 == b5))
			{
				return false;
			}
		}
		else
		{
			if (b4 != b5 || b4 != b4 || b5 != b5)
			{
				return false;
			}
		}

		static const PString b6Str = longestRepresentableString("1234567890ABCDEF");
		static const UINT64 b6Uint = strtoull(b6Str, nullptr, 16);
		PBitmaskImpl<NUM_OF_BITS> b6(b6Str), b7 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(b6Uint);
		if (testEquality)
		{
			if (!(b6 == b7) || !(b6 == b6) || !(b7 == b7))
			{
				return false;
			}
		}
		else
		{
			if (b6 != b7 || b6 != b6 || b7 != b7)
			{
				return false;
			}
		}

		static const PString b8Str = longestRepresentableString("0001234567890ABCDEF");
		static const UINT64 b8Uint = strtoull(b8Str, nullptr, 16);
		PBitmaskImpl<NUM_OF_BITS> b8(b8Str), b9 = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(b8Uint);
		if (testEquality)
		{
			if (!(b8 == b9) || !(b8 == b8) || !(b9 == b9))
			{
				return false;
			}
		}
		else
		{
			if (b8 != b9 || b8 != b8 || b9 != b9)
			{
				return false;
			}
		}

		if (testEquality)
		{
			if (b1 == b4 || b1 == b5 || b1 == b6 || b1 == b7 || b1 == b8 || b1 == b9)
			{
				return false;
			}
		}
		else
		{
			if (!(b1 != b4) || !(b1 != b5) || !(b1 != b6) || !(b1 != b7) || !(b1 != b8) || !(b1 != b9))
			{
				return false;
			}
		}

		if (testEquality)
		{
			if (b4 == b6 || b4 == b7 || b4 == b8 || b4 == b9)
			{
				return false;
			}
		}
		else
		{
			if (!(b4 != b6) || !(b4 != b7) || !(b4 != b8) || !(b4 != b9))
			{
				return false;
			}
		}

		if (testEquality)
		{
			if (b5 == b6 || b5 == b7 || b5 == b8 || b5 == b9)
			{
				return false;
			}
		}
		else
		{
			if (!(b5 != b6) || !(b5 != b7) || !(b5 != b8) || !(b5 != b9))
			{
				return false;
			}
		}

		return true;
	}

	static bool testEqualityOperator()
	{
		return testEqualityOperatorInternal(true);
	}

	static bool testInequalityOperator()
	{
		return testEqualityOperatorInternal(false);
	}

	static bool expectedCarryOver(const char* buf, INT32 bitsToShift)
	{
		PString tmpStr;
		size_t len = strlen(buf);
		INT32 r = len % 16;
		if (r)
		{
			INT32 padding = 16 - r;
			for (int i = 0; i < padding; ++i)
			{
				tmpStr.append("0");
			}
			tmpStr.append(buf);
		}
		else
		{
			tmpStr.assign(buf);
		}
		len = tmpStr.length();

		char str[17] = { 0 };
		strncpy(str, tmpStr.c_str(), 16);

		INT32 timesCanBeShifted = (PBITMASK_TOTAL_BITS / 64) - (len / 16);

		UINT64 num = PBitmaskImpl<NUM_OF_BITS>::atohex64(str, &str[15]);

		const INT32 numChunks = bitsToShift / 64;
		const INT32 remainder = bitsToShift % 64;

		if (numChunks > timesCanBeShifted)
		{
			return true;
		}

		UINT64 newShiftedOut = 0;
		if (numChunks == timesCanBeShifted)
		{
			newShiftedOut = num >> (64 - remainder);
		}

		return newShiftedOut != 0;
	}

	static bool testMemberShiftOperatorHelperInternal(bool useMemberOperator, const char* buf, INT32 s, const function<PBitmaskImpl<NUM_OF_BITS>(const PBitmaskImpl<NUM_OF_BITS>&, INT32)>& bitwiseFunc, const function<void(const char*, int*, char*)>& bitmaskFunc)
	{
		PBitmaskImpl<NUM_OF_BITS> b;
		bool carryOverExpected = expectedCarryOver(buf, s);
		try
		{
			if (useMemberOperator)
			{
				b.fromDbString(buf);
				bitwiseFunc(b, s);

			}
			else
			{
				PBitmaskImpl<NUM_OF_BITS> b1(buf);
				b = bitwiseFunc(b1, s);
			}
		}
		catch (const PError&)
		{
			if (carryOverExpected)
			{
				return true;
			}
			return false;
		}

		char ebuf[4096];
		memset(ebuf, 0, sizeof(ebuf));
		bitmaskFunc(buf, &s, ebuf);
		fitString(ebuf);

		try
		{
			PBitmaskImpl<NUM_OF_BITS> eb(ebuf);
			return (b == eb);
		}
		catch (const PError&)
		{
			return false;
		}
	}

	static bool testMemberShiftOperatorHelper(bool useMemberOperator, const function<PBitmaskImpl<NUM_OF_BITS>(const PBitmaskImpl<NUM_OF_BITS>&, INT32)>& bitwiseFunc, const function<void(const char*, int*, char*)>& bitmaskFunc)
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			for (INT32 s = 0; s <= PBitmaskImpl<NUM_OF_BITS>::size(); ++s)
			{
				vector<UINT64> nums(l, 0);
				for (UINT64 j = 0; j < l; ++j)
				{
					nums[j] = mtRand();
				}
				PASSERT(nums.size() == l);
				UINT64 bufSize = 16 * l + 1;
				unique_ptr<char[]> buf(new char[bufSize]);
				int nw = 0;
				for (UINT64 j = 0; j < l; ++j)
				{
					if (j)
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
					}
					else
					{
						nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
					}
				}

				if (!testMemberShiftOperatorHelperInternal(useMemberOperator, buf.get(), s, bitwiseFunc, bitmaskFunc))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool testMemberShiftLeftOperator()
	{
		return testMemberShiftOperatorHelper(true, [](const PBitmaskImpl<NUM_OF_BITS>& b, INT32 s) {const_cast<PBitmaskImpl<NUM_OF_BITS>&>(b) <<= s; return b;}, [](const char* buf, int* s, char* ebuf) {bitmask_ShiftLeft(buf, s, ebuf);});
	}

	static bool testMemberShiftRightOperator()
	{
		return testMemberShiftOperatorHelper(true, [](const PBitmaskImpl<NUM_OF_BITS>& b, INT32 s) {const_cast<PBitmaskImpl<NUM_OF_BITS>&>(b) >>= s; return b;}, [](const char* buf, int* s, char* ebuf) {bitmask_ShiftRight(buf, s, ebuf);});
	}

	static bool testGlobalShiftLeftOperator()
	{
		return testMemberShiftOperatorHelper(false, [](const PBitmaskImpl<NUM_OF_BITS>& b, INT32 s) {return b << s;}, [](const char* buf, int* s, char* ebuf) {bitmask_ShiftLeft(buf, s, ebuf);});
	}

	static bool testGlobalShiftRightOperator()
	{
		return testMemberShiftOperatorHelper(false, [](const PBitmaskImpl<NUM_OF_BITS>& b, INT32 s) {return b >> s;}, [](const char* buf, int* s, char* ebuf) {bitmask_ShiftRight(buf, s, ebuf);});
	}

	static bool testMixingOperators()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			vector<UINT64> nums(l, 0);
			for (UINT64 j = 0; j < l; ++j)
			{
				nums[j] = mtRand();
			}
			PASSERT(nums.size() == l);
			UINT64 bufSize = 16 * l + 1;
			unique_ptr<char[]> buf(new char[bufSize]);
			int nw = 0;
			for (UINT64 j = 0; j < l; ++j)
			{
				if (j)
				{
					nw += snprintf(buf.get() + nw, bufSize - nw, "%016llx", nums[j]);
				}
				else
				{
					nw += snprintf(buf.get() + nw, bufSize - nw, "%llx", nums[j]);
				}
			}
			PBitmaskImpl<NUM_OF_BITS> b(buf.get());

			for (UINT64 i = 0; i < 100; ++i)
			{
				UINT32 op = mtRand() % 6;
				switch (op)
				{
					case 0:
					case 1:
					case 2:
					case 3:
					{
						if (l >= 2)
						{
							UINT64 l1 = (mtRand() % l) + 1;
							vector<UINT64> nums1(l1, 0);
							for (UINT64 j = 0; j < l1; ++j)
							{
								nums1[j] = mtRand();
							}
							PASSERT(nums1.size() == l1);
							UINT64 bufSize1 = 16 * l1 + 1;
							unique_ptr<char[]> buf1(new char[bufSize1]);
							int nw = 0;
							for (UINT64 j = 0; j < l1; ++j)
							{
								if (j)
								{
									nw += snprintf(buf1.get() + nw, bufSize1 - nw, "%016llx", nums1[j]);
								}
								else
								{
									nw += snprintf(buf1.get() + nw, bufSize1 - nw, "%llx", nums1[j]);
								}
							}
							PBitmaskImpl<NUM_OF_BITS> b1(buf1.get());
							char* res = new char[4096];
							memset(res, 0, 4096);
							if (op == 0)
							{
								b.andNot(b1);
								bitmask_AndNot(buf.get(), buf1.get(), res);
							}
							else if (op == 1)
							{
								b |= b1;
								bitmask_Or(buf.get(), buf1.get(), res);
							}
							else if (op == 2)
							{
								b ^= b1;
								bitmask_Xor(buf.get(), buf1.get(), res);
							}
							else
							{
								b &= b1;
								bitmask_And(buf.get(), buf1.get(), res);
							}

							fitString(res);
							buf.reset(res);
							try
							{
								PBitmaskImpl<NUM_OF_BITS> eb(res);
								if (b != eb)
								{
									return false;
								}
							}
							catch (const PError&)
							{
								return false;
							}
						}
					}
					break;

					case 4:
					case 5:
					{
						INT32 s = mtRand() % 65;
						char* res = new char[4096];
						memset(res, 0, 4096);
						if (op == 4)
						{
							b <<= s;
							bitmask_ShiftLeft(buf.get(), &s, res);
							fitString(res);
						}
						else
						{
							b >>= s;
							bitmask_ShiftRight(buf.get(), &s, res);
							fitString(res);
						}

						buf.reset(res);
						PBitmaskImpl<NUM_OF_BITS> eb;
						try
						{
							if (*res)
							{
								eb.fromDbString(res);
							}
						}
						catch (const PError&)
						{
							return false;
						}
						if (b != eb)
						{
							return false;
						}
					}
					break;
				}
			}
		}
		return true;
	}

	static bool testOperatorLess()
	{
		mt19937_64 mtRand(time(0));
		for (UINT64 l = 1; l <= totalLength; ++l)
		{
			vector<UINT64> nums1(l, 0);
			for (UINT64 j = 0; j < l; ++j)
			{
				nums1[j] = mtRand();
			}
			PASSERT(nums1.size() == l);
			UINT64 bufSize1 = 16 * l + 1;
			unique_ptr<char[]> buf1(new char[bufSize1]);
			int nw = 0;
			for (UINT64 j = 0; j < l; ++j)
			{
				nw += snprintf(buf1.get() + nw, bufSize1 - nw, "%016llx", nums1[j]);
			}
			PBitmaskImpl<NUM_OF_BITS> b1(buf1.get());

			vector<UINT64> nums2(l, 0);
			for (UINT64 j = 0; j < l; ++j)
			{
				nums2[j] = mtRand();
			}
			PASSERT(nums2.size() == l);
			UINT64 bufSize2 = 16 * l + 1;
			unique_ptr<char[]> buf2(new char[bufSize2]);
			nw = 0;
			for (UINT64 j = 0; j < l; ++j)
			{
				nw += snprintf(buf2.get() + nw, bufSize2 - nw, "%016llx", nums2[j]);
			}
			PBitmaskImpl<NUM_OF_BITS> b2(buf2.get());

			assert(strlen(buf1.get()) == strlen(buf2.get()));

			bool isLess = (b1 < b2);
			bool expectedIsLess = lexicographical_compare(buf1.get(), buf1.get() + bufSize1 - 1, buf2.get(), buf2.get() + bufSize2 - 1, [](const char c1, const char c2){return hexDigitToByte(c1, nullptr) < hexDigitToByte(c2, nullptr);});
			if (isLess != expectedIsLess)
			{
				return false;
			}
		}
		return true;
	}

	static bool testSetBitAndTestBit()
	{
		PBitmaskImpl<NUM_OF_BITS> test;
		test.setBit(0, true);

		PString hexStr;
		test.toDbString(hexStr);
		if (!PString::equals(hexStr, "1"))
		{
			return false;
		}

		test.setBit(NUM_OF_BITS - 1, true);
		hexStr = "";
		test.toDbString(hexStr);

		static constexpr size_t expectedStrLen = NUM_OF_BITS / 4;
		char expectedStr[expectedStrLen + 1];
		int bitsToShift = NUM_OF_BITS - 1;
		bitmask_ShiftLeft("1", &bitsToShift, expectedStr);
		expectedStr[expectedStrLen - 1] = '1';
		if (!PString::equals(hexStr, expectedStr))
		{
			return false;
		}

		try
		{
			test.setBit(NUM_OF_BITS * 2, true);
			return false;
		}
		catch (...)
		{
		}

		test.setBit(NUM_OF_BITS - 1, false);
		hexStr = "";
		test.toDbString(hexStr);
		if (!PString::equals(hexStr, "1"))
		{
			return false;
		}
		if (!test.testBit(0) || test.testBit(NUM_OF_BITS - 1))
		{
			return false;
		}
		return true;
	}

public:
	static bool run()
	{
		cout << "running PBitmaskUnitTest for PBitmaskImpl<" << NUM_OF_BITS << ">" << endl;
		static constexpr size_t N = 100;
		for (size_t i = 0; i < N; ++i)
		{
			cout << "running iteration " << (i + 1) << " / " << N << endl;
			bool ok = true;
			ok &= testDefCtor();
			ok &= testUintCtor();
			ok &= testNullStrCtor();
			ok &= testEmptyStrCtor();
			ok &= testValidStrCtor();
			ok &= testInvalidStrCtor();
			ok &= testCopyCtor();
			ok &= testMoveCtor();
			ok &= testAssignmentOperator();
			ok &= testMoveAssignmentOperator();
			ok &= testValidStrAssign();
			ok &= testEmptyStrAssign();
			ok &= testNullStrAssign();
			ok &= testValidStrFromString();
			ok &= testEmptyStrFromString();
			ok &= testNullStrFromString();
			ok &= testInvalidStrFromString();
			ok &= testAndNot();
			ok &= testGetLower32();
			ok &= testGetLower64();
			ok &= testComposeParse();
			ok &= testToStringFromString();
			ok &= testMemberBitwiseOrEqualOperator();
			ok &= testMemberBitwiseXorEqualOperator();
			ok &= testMemberBitwiseAndEqualOperator();
			ok &= testGlobalBitwiseOrOperator();
			ok &= testGlobalBitwiseXorOperator();
			ok &= testGlobalBitwiseAndOperator();
			ok &= testEqualityOperator();
			ok &= testInequalityOperator();
			ok &= testMemberShiftLeftOperator();
			ok &= testMemberShiftRightOperator();
			ok &= testGlobalShiftLeftOperator();
			ok &= testGlobalShiftRightOperator();
			ok &= testMixingOperators();
			ok &= testSetBitAndTestBit();
			ok &= testOperatorLess();
			if (!ok)
			{
				return false;
			}
		}
		return true;
	}
};

} // unnamed namespace

int main()
{
	bool ok;
	ok = PBitmaskUnitTest<64>::run();
	if (!ok)
	{
		cout << "test failed" << endl;
		return 1;
	}
	ok = PBitmaskUnitTest<128>::run();
	if (!ok)
	{
		cout << "test failed" << endl;
		return 1;
	}
	ok = PBitmaskUnitTest<256>::run();
	if (!ok)
	{
		cout << "test failed" << endl;
		return 1;
	}
	ok = PBitmaskUnitTest<512>::run();
	if (!ok)
	{
		cout << "test failed" << endl;
		return 1;
	}
	ok = PBitmaskUnitTest<1024>::run();
	if (!ok)
	{
		cout << "test failed" << endl;
		return 1;
	}
	cout << "test passed" << endl;
	return 0;
}
