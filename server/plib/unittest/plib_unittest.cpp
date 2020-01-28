#pragma warning(disable:5045)  // NOTE(dimak): `warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified`.

#include "../current/3rdparty/gtest/gtest-main-with-dflags.h"
#include "../gtest_bindings.h"

DEFINE_bool(fail, false, "Set to true to fail the test and see the error messages it outputs to the terminal.");

#include <cmath>  // For `atan()`, to compute PI.

// Basic `PString` usage.
TEST(PLib, PStringSmoke)
{
	PString s;
	EXPECT_EQ(0u, s.length());
	s.append("Hello");
	EXPECT_EQ(5u, s.length());
	EXPECT_EQ("Hello", s);
	s.append(", ");
	EXPECT_EQ(7u, s.length());
	EXPECT_EQ("Hello, ", s);
	PString s2 = "world";
	EXPECT_EQ("world", s2);
	s.append(s2);
	EXPECT_EQ(12u, s.length());
	EXPECT_EQ("Hello, world", s);
	s.append('!');
	EXPECT_EQ(13u, s.length());
	EXPECT_EQ("Hello, world!", s);
	s.cut(4);
	EXPECT_EQ(4u, s.length());
	EXPECT_EQ("Hell", s);
}

// `PString` memory allocation with `.cut()` and `.reserve()`.
TEST(PLib, PStringBuffer)
{
	PString s1;
	PString s2;
	PString s3;
	EXPECT_EQ(0u, s1.length());
	EXPECT_EQ(0u, s2.length());
	EXPECT_EQ(0u, s3.length());
	// Empty, uninitialized `PString`-s should return the pointers to the same empty string to `.c_str()`.
	EXPECT_TRUE(s1.c_str() == s2.c_str());
	EXPECT_TRUE(s1.c_str() == s3.c_str());

	s2 = "test";
	EXPECT_EQ(4u, s2.length());
	char const* s2_ptr = s2.c_str();
	EXPECT_FALSE(s2_ptr == s1.c_str());  // Now that `s2` was allocated, the pointer it returns is different from the original empty string one.
	s2.cut(0);
	EXPECT_EQ(0u, s2.length());
	EXPECT_EQ("", s2);
	EXPECT_TRUE(s2_ptr == s2.c_str());
	EXPECT_FALSE(s2_ptr == s1.c_str());

	s2 = "";
	EXPECT_EQ(0u, s2.length());
	EXPECT_EQ("", s2);
	// EXPECT_TRUE(s2_ptr == s2.c_str()); -- This would fail, as `s2 = ""` results in `s2` allocating a new buffer for itself.
	EXPECT_FALSE(s2_ptr == s1.c_str());

	EXPECT_EQ(0u, s3.length());
	EXPECT_TRUE(s1.c_str() == s3.c_str());
	s3.reserve(3u);
	EXPECT_EQ(0u, s3.length());
	EXPECT_FALSE(s1.c_str() == s3.c_str());  // Now that `s3` was allocated, its pointer changes too.
}

// Illustrate by example what do `PString::append*()` do.
TEST(PLib, PStringAppend)
{
	EXPECT_EQ("0", PString().appendInt(0));
	EXPECT_EQ("42", PString().appendInt(42));
	EXPECT_EQ("-1", PString().appendInt(-1));
	EXPECT_EQ("0", PString().appendHexInt(0));
	EXPECT_EQ("20", PString().appendHexInt(32));
	EXPECT_EQ("101", PString().appendHexInt(257));
	EXPECT_EQ("FFFFFEFF", PString().appendHexInt(-257));  // NOTE(dimak): This is not what I expected, but consistent with the code.
	EXPECT_EQ("-0.50", PString().appendDouble(-0.5));
	EXPECT_EQ("3.14", PString().appendDouble(std::atan(1) * 4));
}

// Basic `PBlock` usage.
TEST(PLib, PBlockSmoke)
{
	PBlock b;
	EXPECT_EQ(0u, b.size());
	std::memcpy(b.append(5), "Hello", 5);
	EXPECT_EQ(5u, b.size());
	EXPECT_EQ("Hello", b);
	std::memcpy(b.append(8), ", world!", 8);
	EXPECT_EQ(13u, b.size());
	EXPECT_EQ("Hello, world!", b);
	EXPECT_NE("", b);
	EXPECT_NE("Hell", b);
	EXPECT_NE("Hello, world!!", b);
	b.cut(3);
	EXPECT_EQ(3u, b.size());
	EXPECT_EQ("Hel", b);
}

// `PString` memory allocation with `.cut()` and `.reserve()`.
TEST(PLib, PBlockBuffer)
{
	PBlock s1;
	PBlock s2;
	PBlock s3;
	EXPECT_EQ(0u, s1.size());
	EXPECT_EQ(0u, s2.size());
	EXPECT_EQ(0u, s3.size());
	// Empty, uninitialized `PString`-s should return the pointers to the same empty string to `.ptr()`.
	EXPECT_TRUE(s1.ptr() == s2.ptr());
	EXPECT_TRUE(s1.ptr() == s3.ptr());

	PBlock block_test;
	std::memcpy(block_test.append(4), "Test", 4);
	s2 = block_test;
	EXPECT_EQ(4u, s2.size());
	EXPECT_EQ("Test", s2);
	BYTE const* s2_ptr = s2.ptr();
	EXPECT_FALSE(s2_ptr == s1.ptr());  // Now that `s2` was allocated, the pointer it returns is different from the original empty block one.
	s2.cut(2u);
	EXPECT_EQ(2u, s2.size());
	EXPECT_EQ("Te", s2);
	EXPECT_TRUE(s2_ptr == s2.ptr());
	EXPECT_FALSE(s2_ptr == s1.ptr());
	s2.cut(0u);
	EXPECT_EQ(0u, s2.size());
	EXPECT_EQ("", s2);
	EXPECT_TRUE(s2_ptr == s2.ptr());
	EXPECT_FALSE(s2_ptr == s1.ptr());

	s2 = PBlock();
	EXPECT_EQ(0u, s2.size());
	EXPECT_EQ("", s2);
	// EXPECT_TRUE(s2_ptr == s2.ptr()); -- This would fail, as `s2 = ""` results in `s2` allocating a new buffer for itself.
	EXPECT_FALSE(s2_ptr == s1.ptr());

	EXPECT_EQ(0u, s3.size());
	EXPECT_TRUE(s1.ptr() == s3.ptr());
	s3.reserve(3u);
	EXPECT_EQ(0u, s3.size());
	EXPECT_FALSE(s1.ptr() == s3.ptr());  // Now that `s3` was allocated, its pointer changes too.
}

// Custom comparisons: `PString` to C-strings.
TEST(PLib, PStringToStringComparison)
{
	EXPECT_EQ("foo", PString("foo"));
	EXPECT_EQ(PString("foo"), "foo");
	EXPECT_NE("foo", PString("foo1"));
	EXPECT_NE("foo", PString("fo"));
	EXPECT_NE(PString("foo1"), "foo");
	EXPECT_NE(PString("fo"), "foo");
}

// Custom comparisons: `PBlock` to C-strings and `PString`-s.
TEST(PLib, PStringToPBlockComparison)
{
	PBlock foo_block;
	std::memcpy(foo_block.append(3), "foo", 3);
	EXPECT_EQ(foo_block, "foo");
	EXPECT_EQ("foo", foo_block);
	EXPECT_NE(foo_block, "bar");
	EXPECT_NE("meh", foo_block);
	EXPECT_EQ(foo_block, PString("foo"));
	EXPECT_EQ(PString("foo"), foo_block);
	EXPECT_NE(foo_block, PString("bar"));
	EXPECT_NE(PString("meh"), foo_block);
}

// This test will fail if `--fail` is provided in the command line. It is to show how does the output look like.
TEST(PLib, CustomPrinterFailureTest)
{
	if (FLAGS_fail)
	{
		EXPECT_EQ(PString("foo"), PString("bar"));

		PBlock b;
		BYTE* ptr = b.append(4);
		ptr[0] = 'A';
		ptr[1] = '\0';
		ptr[2] = 0x80;
		ptr[3] = 0xff;
		EXPECT_EQ(b, "blah");
	}
}
