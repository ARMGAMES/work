// ===========================================================
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef PBitmask_h_included
#define PBitmask_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "commmsgbody.h"
#include <array>

#define PBITMASK_TOTAL_BITS 256

template<size_t NUM_OF_BITS>
class PBitmaskImpl
{
	static constexpr size_t dataBitsSize = (NUM_OF_BITS + 63) / 64;
	array<UINT64, dataBitsSize>	dataBits = {0};

	void shiftLeft64Chunk(INT32 n)
	{
		// Verified by caller n <= 64
		UINT64 shiftedOut = 0;
		for (size_t i = 0; i < dataBits.size(); ++i)
		{
			UINT64 newShiftedOut = dataBits[i] >> (64 - n);
			dataBits[i] <<= n;
			dataBits[i] |= shiftedOut;
			shiftedOut = newShiftedOut;
		}
	}
	void shiftRight64Chunk(INT32 n)
	{
		// Verified by caller n <= 64
		UINT64 shiftedOut = 0;
		for (auto iter = dataBits.rbegin(); iter != dataBits.rend(); ++iter)
		{
			UINT64 newShiftedOut = *iter << (64 - n);
			*iter >>= n;
			*iter |= shiftedOut;
			shiftedOut = newShiftedOut;
		}
	}

	const char* appendHexLeadingZeros(PString& ret, UINT64 n) const
	{
		PString tmp;
		tmp.appendHexInt(n);

		auto padZeros = 16 - tmp.length();
		while (padZeros > 0)
		{
			ret.append('0');
			--padZeros;
		}
		ret.append(tmp);

		return ret.c_str();
	}

protected:
	void setFromUINT64(UINT64 n)
	{
		dataBits.fill(0);
		dataBits[0] = n;
	}

public:

	static const PBitmaskImpl<NUM_OF_BITS> zero;

	PBitmaskImpl() {}

	PBitmaskImpl(int) = delete;
	explicit PBitmaskImpl(const char* str)
	{
		fromDbString(str);
	}

	PBitmaskImpl(const PBitmaskImpl<NUM_OF_BITS>& other)
		: dataBits(other.dataBits)
	{}

	virtual ~PBitmaskImpl() {}
	PBitmaskImpl& operator=(const PBitmaskImpl<NUM_OF_BITS>& other)
	{
		if (this != &other)
		{
			dataBits = other.dataBits;
		}
		return *this;
	}

	PBitmaskImpl(PBitmaskImpl<NUM_OF_BITS>&& other)
		: dataBits(std::move(other.dataBits)) 
	{}

	PBitmaskImpl& operator=(PBitmaskImpl<NUM_OF_BITS>&& other)
	{
		if (this != &other)
		{
			dataBits = std::move(other.dataBits);
		}
		return *this;
	}


	virtual const char* toDbString(PString& hexStr) const
	{
		hexStr.cut(0); // PYR-122198
		//	Most significant UINT64 is the last array element
		bool isFirst = true;
		for (auto iter = dataBits.crbegin(); iter != dataBits.crend(); ++iter)
		{
			if (isFirst && !*iter)
				continue;
			if (isFirst)
			{
				hexStr.appendHexInt(*iter);
				isFirst = false;
			}
			else
			{
				appendHexLeadingZeros(hexStr, *iter);
			}
		}

		if (hexStr.length() == 0)
		{
			hexStr.append('0');
		}
		return hexStr.c_str();
	}

	virtual void fromDbString(const char* hexStr)	//	throws
	{
		static const size_t MAX_HEX_CHARS = dataBits.size() * sizeof(UINT64) * 2;

		if (hexStr == nullptr || *hexStr == '\0')
		{
			throw PError("PBitmask::fromDbString null/empty input string");
		}

		const auto len = strlen(hexStr);
		if (len > MAX_HEX_CHARS)
		{
			PString err("PBitmask::fromDbString input string too long: ");
			err.appendInt(len).append(" > ").appendInt(MAX_HEX_CHARS);
			throw PError(err);
		}

		dataBits.fill(0U);
		const char* end = &hexStr[len - 1];

		size_t idx = 0;
		while ((end - 15) > hexStr)
		{
			dataBits[idx++] = atohex64(end - 15, end);
			end -= 16;
		}
		dataBits[idx] = atohex64(hexStr, end);
	}

	// Initialize from dbString if not empty, otherwise initialize from integerMaskFallback
	void fromDbString(const char* dbString, UINT64 integerMaskFallback) // no argument based throw
	{
		if (dbString && *dbString)
		{
			fromDbString(dbString);
		}
		else
		{
			*this = PBitmaskImpl<NUM_OF_BITS>::fromUINT64(integerMaskFallback);
		}
	}

	static PBitmaskImpl<NUM_OF_BITS> fromDbStringS(const char* hexStr)
	{
		return PBitmaskImpl<NUM_OF_BITS>(hexStr);
	}

	PBitmaskImpl<NUM_OF_BITS>& operator|=(const PBitmaskImpl<NUM_OF_BITS>& other)
	{
		for (size_t i = 0U; i < dataBits.size(); ++i)
		{
			dataBits[i] |= other.dataBits[i];
		}

		return *this;
	}

	PBitmaskImpl<NUM_OF_BITS>& operator^=(const PBitmaskImpl<NUM_OF_BITS>& other)
	{
		for (size_t i = 0U; i < dataBits.size(); ++i)
		{
			dataBits[i] ^= other.dataBits[i];
		}
		return *this;
	}

	PBitmaskImpl<NUM_OF_BITS>& operator&=(const PBitmaskImpl<NUM_OF_BITS>& other)
	{
		for (size_t i = 0U; i < dataBits.size(); ++i)
		{
			dataBits[i] &= other.dataBits[i];
		}
		return *this;
	}

	PBitmaskImpl<NUM_OF_BITS>& operator<<=(INT32 n)
	{
		if (n == 0)
			return *this;

		if (n < 0)
		{
			throw PError("PBitmask operator<<= invalid arg");
		}

		if (n <= 64)	//	Most use-case shortcut
		{
			shiftLeft64Chunk(n);
			return *this;
		}

		const INT32 numChunks = n / 64;
		const INT32 remainder = n % 64;
		for (INT32 i = 0; i < numChunks; ++i)
		{
			shiftLeft64Chunk(64);
		}
		if (remainder)
			shiftLeft64Chunk(remainder);

		return *this;
	}

	PBitmaskImpl<NUM_OF_BITS>& operator>>=(INT32 n)
	{
		if (n == 0)
			return *this;

		if (n < 0)
		{
			throw PError("operator>>= invalid arg");
		}

		if (n <= 64)	//	Most use-case shortcut
		{
			shiftRight64Chunk(n);
			return *this;
		}

		const INT32 numChunks = n / 64;
		const INT32 remainder = n % 64;
		for (INT32 i = 0; i < numChunks; ++i)
		{
			shiftRight64Chunk(64);
		}
		if (remainder)
			shiftRight64Chunk(remainder);

		return *this;
	}

	bool operator==(const PBitmaskImpl<NUM_OF_BITS>& other) const
	{
		return dataBits == other.dataBits;
	}

	bool operator!=(const PBitmaskImpl<NUM_OF_BITS>& other) const
	{
		return !(*this == other);
	}

	bool intersects(const PBitmaskImpl<NUM_OF_BITS>& other) const
	{
		for (size_t i = 0; i < dataBits.size(); ++i)
		{
			if (dataBits[i] & other.dataBits[i])
			{
				return true;
			}
		}
		return false;
	}

	PBitmaskImpl<NUM_OF_BITS>& andNot(const PBitmaskImpl<NUM_OF_BITS>& other)
	{
		for (size_t i = 0; i < dataBits.size(); ++i)
		{
			dataBits[i] &= ~other.dataBits[i];
		}
		return *this;
	}

	virtual void compose(CommMsgBody& body) const
	{
		body.composeINT32(static_cast<INT32>(dataBits.size()));
		for (auto d : dataBits)
		{
			body.composeUINT64(d);
		}
	}

	virtual void parse(CommMsgParser& parser)
	{
		dataBits.fill(0U);

		INT32 size;
		parser.parseINT32(size);

		if (size > static_cast<INT32>(dataBits.size()))
		{
			INT32 i = 0;
			for (; i < static_cast<INT32>(dataBits.size()); ++i)
			{
				parser.parseUINT64(dataBits[i]);
			}
			for (; i < size; ++i)
				parser.skip(sizeof(UINT64));
			return;
		}

		// size <= dataBits.size()
		for (INT32 i = 0; i < size; ++i)
		{
			parser.parseUINT64(dataBits[i]);
		}
	}

	UINT32 getLower32() const
	{
		return static_cast<UINT32>(getLower64());
	}

	UINT64 getLower64() const
	{
		return dataBits[0];
	}

	void setBit(INT32 bitPos, bool setNotClear)
	{
		if (bitPos < 0)
		{
			throw PError("PBitmask::setBit invalid bitPos");
		}
		UINT32 idx = bitPos / 64;
		UINT32 offset = bitPos % 64;
		if (idx >= dataBits.size())
		{
			PString err("setBit: bitPos ");
			err.appendInt(bitPos).append(" out of range");
			throw PError(err);
		}
		if (setNotClear)
		{
			dataBits[idx] |= UINT64(1) << offset;
		}
		else
		{
			dataBits[idx] &= ~(UINT64(1) << offset);
		}
	}

	bool testBit(INT32 bitPos) const
	{
		if (bitPos < 0)
		{
			throw PError("testBit invalid bitPos");
		}
		UINT32 idx = bitPos / 64;
		UINT32 offset = bitPos % 64;
		if (idx >= dataBits.size())
		{
			return false;
		}
		return ((dataBits[idx] >> offset) & 1U) != 0;
	}

	bool isOnlyOneBitSet() const
	{
		bool onlyOneSet = false;
		for (auto d : dataBits)
		{
			if (d > 0)
			{
				if (!(d & (d - 1)))
				{
					if (onlyOneSet)
						return false;

					onlyOneSet = true;
				}
				else
					return false;
			}
		}
		return onlyOneSet;
	}

	// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	static INT32 countBits(UINT64 v)
	{
		v = v - ((v >> 1) & (UINT64)~(UINT64)0 / 3);
		v = (v & (UINT64)~(UINT64)0 / 15 * 3) + ((v >> 2) & (UINT64)~(UINT64)0 / 15 * 3);
		v = (v + (v >> 4)) & (UINT64)~(UINT64)0 / 255 * 15;
		return (UINT64)(v * ((UINT64)~(UINT64)0 / 255)) >> (sizeof(UINT64) - 1) * CHAR_BIT;         //-V634
	}

	INT32 numBitsSet() const
	{
		INT32 count = 0;
		for (auto d : dataBits)
		{
			count += countBits(d);
		}
		return count;
	}

	// PYR-109751 - returns the current number of bits stored.
	static constexpr size_t size()
	{
		return NUM_OF_BITS;
	}


	static PBitmaskImpl<NUM_OF_BITS> fromUINT64(UINT64 n)
	{
		PBitmaskImpl<NUM_OF_BITS> b;
		b.setFromUINT64(n);
		return b;
	}

	static PBitmaskImpl fromINT64(INT64 n) { return fromUINT64(static_cast<UINT64>(n)); }

	//	Modified version of p_hex64toa which takes an end ptr.
	static UINT64 atohex64(_In_opt_z_ const char * start, _In_opt_z_ const char* end) //	throws
	{
		UINT64 result64 = 0;

		while (start <= end)
		{
			if (*start >= '0' && *start <= '9')
				result64 = (result64 << 4) | (*start - '0');
			else if (*start >= 'A' && *start <= 'F')
				result64 = (result64 << 4) | (*start - 'A' + 10);
			else if (*start >= 'a' && *start <= 'f')
				result64 = (result64 << 4) | (*start - 'a' + 10);
			else
				throw PError("PBitmask: invalid hex char");

			++start;
		}
		return result64;
	}

	friend bool operator< (const PBitmaskImpl<NUM_OF_BITS>& pb1, const PBitmaskImpl<NUM_OF_BITS>& pb2)
	{
		return lexicographical_compare(pb1.dataBits.crbegin(), pb1.dataBits.crend(), pb2.dataBits.crbegin(), pb2.dataBits.crend());
	}

	explicit operator bool() const
	{
		return *this != zero;
	}


	// ========== ATF support functions ==========

	const char *toJSONString( PString& buf ) const
	{
		PString tmpStr;
		buf.append( "\"" );
		toDbString( tmpStr );
		buf.append( tmpStr ).append( "\"" );
		return buf.c_str();
	}

	void fromJSONString( const PString& buf )
	{
		INT64 sz = buf.length();
		if( sz >= 2 && buf[ 0 ] == '"' && buf[ sz - 1 ] == '"' )
		{
			// remove the leading and trailing quotation sign
			fromDbString( PString( buf.c_str() + 1, buf.c_str() + sz - 1 ) );
		}
		else
		{
			fromDbString( buf );
		}
	}

	void composeMsg( CommMsgBody& body, bool ignoreJSON = false ) const
	{
		compose( body );
	}

	void parseMsg( CommMsgParser& parser )
	{
		parse( parser );
	}

	void clear()
	{
		dataBits.fill(0);
	}

	bool equals( const PBitmaskImpl<NUM_OF_BITS>& other ) const
	{
		return *this == other;
	}

	void toTraceString( PString& buff ) const
	{
		PString s;
		toDbString( s );
		buff.append( s );
	}

	void toXmlString( const char *element, PString& buf ) const
	{
		PASSERT( element && *element );   // element is not empty and should not contain white space
		PString s;
		buf.append( '<' ).append( element ).append( '>' ).
			append(toDbString( s )).
			append( "</" ).append( element ).append( '>' );
	}

	bool fromXmlString( const PString& s )
	{
		fromDbString( s );
		return true;
	}

	static void validateJSONString( const PString& jsonString, const /*AtfMsgChecker*/void* /*checker*/, const char* descr, PString& fieldsWithUnparsedContent )
	{
		PBitmaskImpl<NUM_OF_BITS> b;
		b.fromJSONString( jsonString );
	}

	static void validateMsg( CommMsgParser& parser, const /*AtfMsgChecker*/void* /*checker*/, const char* descr, PString& fieldsWithUnparsedContent )
	{
		PBitmaskImpl<NUM_OF_BITS> b;
		b.parseMsg(parser);
	}

};

typedef PBitmaskImpl<PBITMASK_TOTAL_BITS> PBitmask;

template <size_t NUM_OF_BITS>
const PBitmaskImpl<NUM_OF_BITS> PBitmaskImpl<NUM_OF_BITS>::zero;

template <size_t NUM_OF_BITS>
inline PBitmaskImpl<NUM_OF_BITS> operator|(const PBitmaskImpl<NUM_OF_BITS>& left, const PBitmaskImpl<NUM_OF_BITS>& right)
{
	PBitmaskImpl<NUM_OF_BITS> answer = left;
	return answer.operator|=(right);
}

template <size_t NUM_OF_BITS>
inline PBitmaskImpl<NUM_OF_BITS> operator&(const PBitmaskImpl<NUM_OF_BITS>& left, const PBitmaskImpl<NUM_OF_BITS>& right)
{
	PBitmaskImpl<NUM_OF_BITS> answer = left;
	return answer.operator&=(right);
}

template <size_t NUM_OF_BITS>
inline PBitmaskImpl<NUM_OF_BITS> operator^(const PBitmaskImpl<NUM_OF_BITS>& left, const PBitmaskImpl<NUM_OF_BITS>& right)
{
	PBitmaskImpl<NUM_OF_BITS> answer = left;
	return answer.operator^=(right);
}

template <size_t NUM_OF_BITS>
inline PBitmaskImpl<NUM_OF_BITS> operator>>(const PBitmaskImpl<NUM_OF_BITS>& left, INT32 shift)
{
	PBitmaskImpl<NUM_OF_BITS> answer = left;
	return answer.operator>>=(shift);
}

template <size_t NUM_OF_BITS>
inline PBitmaskImpl<NUM_OF_BITS> operator<<(const PBitmaskImpl<NUM_OF_BITS>& left, INT32 shift)
{
	PBitmaskImpl<NUM_OF_BITS> answer = left;
	return answer.operator<<=(shift);
}

template <size_t NUM_OF_BITS>
inline PBitmaskImpl<NUM_OF_BITS> andNot(const PBitmaskImpl<NUM_OF_BITS>& left, const PBitmaskImpl<NUM_OF_BITS>& right)
{
	PBitmaskImpl<NUM_OF_BITS> answer = left;
	return answer.andNot(right);
}

#endif	//	PBitmask_h_included
