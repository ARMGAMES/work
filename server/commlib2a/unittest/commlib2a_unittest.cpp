#pragma warning(disable:5045)  // NOTE(dimak): `warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified`.

#include "../current/3rdparty/gtest/gtest-main-with-dflags.h"

#include "../commssl.h"
#include "../../plib/gtest_bindings.h"

#ifdef _DEBUG
constexpr static uint64_t kDefaultLargeInputSize = static_cast<uint64_t>(1e5);
#else
constexpr static uint64_t kDefaultLargeInputSize = static_cast<uint64_t>(1e6);
#endif

DEFINE_uint64(large_input_size, kDefaultLargeInputSize, "The number of entries to run the large test against.");
DEFINE_uint64(random_seed, 19830816, "The random seed to use for the large test.");
DEFINE_double(empty_input_probability, 0.03, "The probabilty that each randomly generated string will be of zero length.");
DEFINE_uint32(random_input_min_length, 450, "For non-empty input strings, the shortest length.");
DEFINE_uint32(random_input_max_length, 550, "For non-empty input strings, the longest length.");

TEST(CommLib2a, Base64SmokeTest)
{
	PBlock input;
	PString output;

	input.cut(0);
	std::memcpy(input.append(3), "foo", 3);
	CommSSLBase64::encode(output, input.ptr(), input.size());
	EXPECT_EQ("Zm9v", output);
	PBlock result;
	CommSSLBase64::decode(result, output.c_str());
	EXPECT_EQ("foo", result);
}

TEST(CommLib2a, Base64RegressionTest)
{
#define BASE64_ASCII_TEST_CASE(encoded, original) \
	do { \
		PString encoded_via_code; \
		CommSSLBase64::encode(encoded_via_code, reinterpret_cast<BYTE const*>(original), std::strlen(original)); \
		EXPECT_EQ(encoded, encoded_via_code) << "For input `" << original << "`."; \
		PBlock decoded; \
		CommSSLBase64::decode(decoded, encoded_via_code); \
		EXPECT_EQ(decoded, original) << "For encoded `" << encoded_via_code << "`.'"; \
	} while (false)
#define BASE64_BINARY_TEST_CASE(encoded, ...) \
	do { \
		std::vector<BYTE> original_vector({__VA_ARGS__}); \
		PString encoded_via_code; \
		CommSSLBase64::encode(encoded_via_code, &original_vector[0], original_vector.size()); \
		EXPECT_EQ(encoded, encoded_via_code); \
		PBlock original; \
		std::memcpy(original.ensure(original_vector.size()), &original_vector[0], original_vector.size()); \
		PBlock decoded_via_code; \
		CommSSLBase64::decode(decoded_via_code, encoded); \
		EXPECT_EQ(decoded_via_code, original) << "For encoded `" << encoded << "`.'"; \
	} while (false)
#include "base64_test_cases.inl"
#undef BASE64_ASCII_TEST_CASE
#undef BASE64_BINARY_TEST_CASE
}

TEST(CommLib2a, Base64FastImplSmokeTest)
{
	PBlock input;
	PBlock output;

	input.cut(0);
	std::memcpy(input.append(3), "foo", 3);
	CommSSLBase64::encodeOverwritingIntoBlock(output, input.ptr(), input.size());
	EXPECT_EQ("Zm9v", output);
	PBlock result;
	CommSSLBase64::decodeOverwriting(result, reinterpret_cast<const char*>(output.ptr()), output.size());
	EXPECT_EQ("foo", result);
}

TEST(CommLib2a, Base64FastImplRegressionTest)
{
#define BASE64_ASCII_TEST_CASE(encoded, original) \
	do { \
		PString encoded_as_pstring(encoded); \
		PString original_as_pstring(original); \
		PBlock encoded_via_code; \
		CommSSLBase64::encodeOverwritingIntoBlock(encoded_via_code, original); \
		EXPECT_EQ(encoded_as_pstring, encoded_via_code) << "For input `" << original << "`."; \
		PBlock decoded; \
		CommSSLBase64::decodeOverwriting(decoded, encoded_via_code); \
		EXPECT_EQ(decoded, original_as_pstring) << "For encoded `" << encoded << "`.'"; \
	} while (false)
#define BASE64_BINARY_TEST_CASE(encoded, ...) \
	do { \
		PString encoded_as_pstring(encoded); \
		std::vector<BYTE> original_vector({__VA_ARGS__}); \
		PBlock original_as_pblock; \
		std::memcpy(original_as_pblock.ensure(original_vector.size()), &original_vector[0], original_vector.size()); \
		PBlock encoded_via_code; \
		CommSSLBase64::encodeOverwritingIntoBlock(encoded_via_code, original_as_pblock); \
		EXPECT_EQ(encoded_as_pstring, encoded_via_code); \
		PBlock decoded_via_code; \
		CommSSLBase64::decodeOverwriting(decoded_via_code, encoded_via_code); \
		EXPECT_EQ(decoded_via_code, original_as_pblock) << "For encoded `" << encoded << "`.'"; \
	} while (false)

#include "base64_test_cases.inl"
#undef BASE64_ASCII_TEST_CASE
#undef BASE64_BINARY_TEST_CASE
}

struct Base64TestLargeInput final {
	struct GoldenEntry {
		PBlock original;
		PString encoded;
	};
	std::vector<GoldenEntry> entries;
	static std::vector<GoldenEntry> GenEntries() {
		std::vector<GoldenEntry> entries(FLAGS_large_input_size);
		srand(FLAGS_random_seed);
		constexpr static int cutoff_p_mask = 0xffff;
		static_assert((cutoff_p_mask & (cutoff_p_mask + 1)) == 0, "Must be a power of two minus one.");
		int const cutoff_p = FLAGS_empty_input_probability * cutoff_p_mask;
		for (GoldenEntry& e : entries) {
			if ((rand() & cutoff_p_mask) >= cutoff_p) {
				size_t const sz = FLAGS_random_input_min_length + (rand() % (FLAGS_random_input_max_length - FLAGS_random_input_min_length + 1));
				BYTE* ptr = e.original.ensure(sz);
				for (size_t i = 0; i < sz; ++i) {
					ptr[i] = static_cast<BYTE>(rand() & 0xff);
				}
				CommSSLBase64::encode(e.encoded, ptr, sz);
			}
		}
		return entries;
	}
	Base64TestLargeInput() : entries(GenEntries()) {}
};

TEST(CommLib2a, Base64PrepareLargeInput)
{
	current::Singleton<Base64TestLargeInput>();
}

TEST(CommLib2a, Base64TestEncodeAgainstLargeInput)
{
	for (auto const& e : current::Singleton<Base64TestLargeInput>().entries) {
		PString encoded_via_code;
		CommSSLBase64::encode(encoded_via_code, e.original.ptr(), e.original.size());
		EXPECT_EQ(e.encoded, encoded_via_code);
	}
}

TEST(CommLib2a, Base64TestDecodeAgainstLargeInput)
{
	for (auto const& e : current::Singleton<Base64TestLargeInput>().entries) {
		PBlock decoded_via_code;
		CommSSLBase64::decode(decoded_via_code, e.encoded.c_str(), e.encoded.length());
		EXPECT_EQ(e.original, decoded_via_code);
	}
}

TEST(CommLib2a, Base64FastImplTestEncodeAgainstLargeInput)
{
	for (auto const& e : current::Singleton<Base64TestLargeInput>().entries) {
		static PBlock encoded_via_code;
		CommSSLBase64::encodeOverwritingIntoBlock(encoded_via_code, e.original);
		EXPECT_EQ(e.encoded, encoded_via_code);
	}
}

TEST(CommLib2a, Base64FastImplTestDecodeAgainstLargeInput)
{
	for (auto const& e : current::Singleton<Base64TestLargeInput>().entries) {
		static PBlock decoded_via_code;
		CommSSLBase64::decodeOverwriting(decoded_via_code, e.encoded);
		EXPECT_EQ(e.original, decoded_via_code);
	}
}
