#if !defined(BASE64_ASCII_TEST_CASE) || !defined(BASE64_BINARY_TEST_CASE)
#error "Must define `BASE64_ASCII_TEST_CASE` and `BASE64_BINARY_TEST_CASE` before `#include`-ing this `.inl` file."
#endif

// NOTE(dimak): ASCII tests added via https://github.com/dkorolev/Current/blob/61f80ba67a4c4e796cfb05e0ad303d132b391b4b/bricks/util/test.cc#L250-L274.
BASE64_ASCII_TEST_CASE("", "");
BASE64_ASCII_TEST_CASE("Zg==", "f");
BASE64_ASCII_TEST_CASE("Zm8=", "fo");
BASE64_ASCII_TEST_CASE("Zm9v", "foo");
BASE64_ASCII_TEST_CASE("Zm9vYg==", "foob");
BASE64_ASCII_TEST_CASE("Zm9vYmE=", "fooba");
BASE64_ASCII_TEST_CASE("Zm9vYmFy", "foobar");
BASE64_ASCII_TEST_CASE("MDw+", "0<>");  // The '+' is the standard, non-URL, piece.

// NOTE(dimak): Binary tests added manually and validated with `echo -n '...' | base64 --decode | hexdump' (or `| hexdump -b` to ensure output bytes count).
BASE64_BINARY_TEST_CASE("fw==", 0x7f);
BASE64_BINARY_TEST_CASE("gA==", 0x80);
BASE64_BINARY_TEST_CASE("AA==", 0x00);
BASE64_BINARY_TEST_CASE("/w==", 0xff);
BASE64_BINARY_TEST_CASE("AP8=", 0x00, 0xff);
BASE64_BINARY_TEST_CASE("AP8A", 0x00, 0xff, 0x00);
BASE64_BINARY_TEST_CASE("AP8A/w==", 0x00, 0xff, 0x00, 0xff);
BASE64_BINARY_TEST_CASE("/wA=", 0xff, 0x00);
BASE64_BINARY_TEST_CASE("/wD/", 0xff, 0x00, 0xff);
BASE64_BINARY_TEST_CASE("/wD/AA==", 0xff, 0x00, 0xff, 0x00);
BASE64_BINARY_TEST_CASE("qlWqVao=", 0xaa, 0x55, 0xaa, 0x55, 0xaa);
BASE64_BINARY_TEST_CASE("VapVqlWq", 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa);
