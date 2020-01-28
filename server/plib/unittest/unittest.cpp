#include "../../unittestpp/src/UnitTest++.h"
#include "ppinclude.h"
#include "ppstring.h"

TEST(PString_AssignFromConstChar)
{
	{
		const char* p = "12345678901234567890";
		PString s;

		// s is empty
		s = p;
		CHECK_EQUAL(strlen(p), s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s has size smaller than p
		s = "123456789";
		s = p;
		CHECK_EQUAL(strlen(p), s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s has size larger than p
		s = "1234567890123456789012345678901234567890";
		s = p;
		CHECK_EQUAL(strlen(p), s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s has size the same as p
		s = "12345678901234567891";
		s = p;
		CHECK_EQUAL(strlen(p), s.length());
		CHECK_EQUAL(true, s.equals(p));

	}

	{
		const char* p = NULL; // p is null
		PString s;

		// s is empty
		s = p;
		CHECK_EQUAL(0, s.length());
		CHECK_EQUAL(true, s.equals(""));

		// s is not empty
		s = "1234567890123456789012345678901234567890";
		s = p;
		CHECK_EQUAL(0, s.length());
		CHECK_EQUAL(true, s.equals(""));
	}	

	{
		const char* p = ""; // p is empty
		PString s;

		// s is empty
		s = p;
		CHECK_EQUAL(strlen(p), s.length());
		CHECK_EQUAL(0, s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s is not empty
		s = "1234567890123456789012345678901234567890";
		s = p;
		CHECK_EQUAL(strlen(p), s.length());
		CHECK_EQUAL(0, s.length());
		CHECK_EQUAL(true, s.equals(p));
	}	
}

TEST(PString_AssignFromString)
{
	{
		PString p = "12345678901234567890";
		PString s;
		
		// s is empty
		s = p;
		CHECK_EQUAL(p.length(), s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s has size smaller than p
		s = "123456789";
		s = p;
		CHECK_EQUAL(p.length(), s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s has size larger than p
		s = "1234567890123456789012345678901234567890";
		s = p;
		CHECK_EQUAL(p.length(), s.length());
		CHECK_EQUAL(true, s.equals(p));
	}

	{
		PString p; // p is empty
		PString s;

		// s is empty
		s = p;
		CHECK_EQUAL(p.length(), s.length());
		CHECK_EQUAL(true, s.equals(p));

		// s is not empty
		s = "1234567890123456789012345678901234567890";
		s = p;
		CHECK_EQUAL(p.length(), s.length());
		CHECK_EQUAL(true, s.equals(p));
	}	
}

int main(int argc, char** argv)
{
	return UnitTest::RunTestCommandLine(argc, argv);
}