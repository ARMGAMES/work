#ifndef gtest_bindings_h_included
#define gtest_bindings_h_included

// The logic to:
// * Make it possible to print the contents of `PString`-s and `PBlock`-s from the unit tests, in human-readable, decorated ways.
// * Make it possible to compare `PString`-s to other `PString`-s and to regular C strings, also for the unit test.
// * Make it posisble to compare `PString`-s to `PBlock`-s, again, for the unit testing purposes.

#pragma warning(disable:5045)  // NOTE(dimak): `warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified`.

#include "ppstring.h"
#include "ppblock.h"

#include "../current/3rdparty/gtest/gtest-custom-comparators.h"

inline void PrintTo(PString const& s, std::ostream* os) { *os << "PString(\"" << s.c_str() << "\", length=" << s.length() << ")"; }

inline void PrintTo(PBlock const& b, std::ostream* os)
{
	*os << "PBlock(";
	bool ascii = true;
	BYTE const* ptr = b.ptr();
	for (size_t i = 0u; i < b.size() && ascii; ++i) {
		if (!(ptr[i] >= 32 && ptr[i] < 128)) {
			ascii = false;
		}
	}
	if (ascii) {
		*os << '"' << std::string(ptr, ptr + b.size()) << '"';
	}
	else {
		*os << "{ ";
		for (size_t i = 0u; i < b.size(); ++i) {
			if (i > 0) {
				*os << ", ";
			}
			static char buf[5];
			sprintf(buf, "0x%02x", ptr[i]);
			*os << buf;
			if (ptr[i] >= 32 && ptr[i] < 128) {
				*os << "=\'" << static_cast<char>(ptr[i]) << '\'';
			}
		}
		*os << " }";
	}
	*os << ", size = " << b.size() << ")";
}

namespace gtest {
	template <>
	struct CustomComparator<PString, PString> {
		static bool EXPECT_EQ_IMPL(PString const& lhs, PString const& rhs) { return lhs.length() == rhs.length() && 0 == std::memcmp(lhs.c_str(), rhs.c_str(), lhs.length()); }
		static bool EXPECT_NE_IMPL(PString const& lhs, PString const& rhs) { return !EXPECT_EQ_IMPL(lhs, rhs); }
	};
	template <>
	struct CustomComparator<PString, char*> {
		static bool EXPECT_EQ_IMPL(PString const& s, char const* c) { return std::strlen(c) == s.length() && 0 == std::memcmp(s.c_str(), c, s.length() + 1u); }
		static bool EXPECT_NE_IMPL(PString const& s, char const* c) { return !EXPECT_EQ_IMPL(s, c); }
	};
	template <>
	struct CustomComparator<char*, PString> {
		static bool EXPECT_EQ_IMPL(char const* c, PString const& s) { return CustomComparator<PString, char*>::EXPECT_EQ_IMPL(s, c); }
		static bool EXPECT_NE_IMPL(char const* c, PString const& s) { return CustomComparator<PString, char*>::EXPECT_NE_IMPL(s, c); }
	};
	// NOTE(dimak): This is a unittest-only comparison; obviously, we're not going to compare `PBlock`-s to `char` arrays in the real life.
	template <>
	struct CustomComparator<PBlock, char*> {
		static bool EXPECT_EQ_IMPL(PBlock const& b, char const* c) { return b.size() == std::strlen(c) && 0 == std::memcmp(b.ptr(), c, b.size()); }
		static bool EXPECT_NE_IMPL(PBlock const& b, char const* c) { return !EXPECT_EQ_IMPL(b, c); }
	};
	template <>
	struct CustomComparator<char*, PBlock> {
		static bool EXPECT_EQ_IMPL(char const* c, PBlock const& b) { return CustomComparator<PBlock, char*>::EXPECT_EQ_IMPL(b, c); }
		static bool EXPECT_NE_IMPL(char const* c, PBlock const& b) { return CustomComparator<PBlock, char*>::EXPECT_NE_IMPL(b, c); }
	};
	template <>
	struct CustomComparator<PBlock, PBlock> {
		static bool EXPECT_EQ_IMPL(PBlock const& b1, PBlock const& b2) { return b1.size() == b2.size() && 0 == std::memcmp(b1.ptr(), b2.ptr(), b1.size()); }
		static bool EXPECT_NE_IMPL(PBlock const& b1, PBlock const& b2) { return !EXPECT_EQ_IMPL(b1, b2); }
	};
	// NOTE(dimak): Comparisons between `PString` and `PBlock` are specifically to test the base64 encoder/decoder.
	template <>
	struct CustomComparator<PBlock, PString> {
		static bool EXPECT_EQ_IMPL(PBlock const& b, PString const& s) { return b.size() == s.length() && 0 == std::memcmp(b.ptr(), s.c_str(), b.size()); }
		static bool EXPECT_NE_IMPL(PBlock const& b, PString const& s) { return !EXPECT_EQ_IMPL(b, s); }
	};
	template <>
	struct CustomComparator<PString, PBlock> {
		static bool EXPECT_EQ_IMPL(PString const& s, PBlock const& b) { return CustomComparator<PBlock, PString>::EXPECT_EQ_IMPL(b, s); }
		static bool EXPECT_NE_IMPL(PString const& s, PBlock const& b) { return CustomComparator<PBlock, PString>::EXPECT_NE_IMPL(b, s); }
	};

}  // namespace gtest

#endif  // gtest_bindings_h_included
