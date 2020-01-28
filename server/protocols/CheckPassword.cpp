// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "i18n.shared.h"
#include "ppi18n_x.h"
#include "licenseid.h"

#include "CheckPassword.h"

#define TEMP_PWD_PREFIX	"tmp"

enum ePwdCheckRules : UINT32
{
	ePwdCheckRules_firstNameNotAllowed		= 1,
	ePwdCheckRules_lastNameNotAllowed		= 1 << 1,	//	2
	ePwdCheckRules_dobNotAllowed			= 1 << 2,	//	4
	ePwdCheckRules_mustStartWithAlpha		= 1 << 3,	//	8
	ePwdCheckRules_mustContainOneDigit		= 1 << 4,	//	16,		should be set for non-ES license
	ePwdCheckRules_secondLastNameNotAllowed = 1 << 5,	//	32,	
};

static const char specialChars[] = { ' ', '-', '\'' };

//	Copied from client/client_shared/i18n.cpp
template< typename _char >
static inline bool _is_ascii_char( _char c )
{
	return (0 < c) && (c < 0x80);
}

//	Copied from client/gui/DialogHelpers.h
inline bool _isPasswordChar( PUNICHAR c )
{
	return _is_ascii_char( c );
}

//	Copied from client/gui/DialogHelpers.h
inline bool _isPasswordSymbolChar( PUNICHAR c )
{
	if (!_isPasswordChar( c ))
		return false;

	// no numbers
	if (('0' <= c) && (c <= '9'))
		return false;

	// no digits
	if (('A' <= c) && (c <= 'Z'))
		return false;
	if (('a' <= c) && (c <= 'z'))
		return false;

	return true;
}

//	Copied from client/gui/ustring.h
inline PUNICHAR _upr( PUNICHAR c )
{
	return i18nCharToUpperCS( c );
}

//	Copied from client/gui/ustring.h
inline const PUNICHAR* _stristr( const PUNICHAR* s1, const PUNICHAR* s2 )
{
	if (!*s2)
		return s1;
	while (*s1)
	{
		const PUNICHAR* p1 = s1;
		const PUNICHAR* p2 = s2;
		for (; *p1 && *p2 && (_upr( *p1 ) == _upr( *p2 )); ++p1, ++p2)
			;
		if (!*p2)
			return s1;
		++s1;
	}
	return 0;
}

//	Copied from client/gui/ustring.h
class PUniStringEx : public PUniString
{
public:

	PUniStringEx( const char* in_ )
	{
		if (in_ == nullptr)
		{
			in_ = "";
		}

		PStringX::StringParser parser;
		parser.init( in_ );

		while (PUNICHAR_RET c = parser.nextChar())
		{
			if (0 < c)
				append( c );
			else
				break;
		}
	}
	PUniStringEx(const PUNICHAR* s)
		: PUniString(s)
	{}
	PUniStringEx(const PUNICHAR* begin, const PUNICHAR* end)
		: PUniString(begin, end)
	{}

	~PUniStringEx()
	{
		clear();
	}

	bool containsIgnoreCase( const PUniStringEx& in_ ) const
	{
		return _stristr( c_str(), in_.c_str() ) != nullptr;
	}
};

static void removeSpecialChars(const PString& orig, PString& fixed)
{
	fixed.cut(0);
	for (int i = 0; i < orig.length(); ++i)
	{
		bool isSpecChar = false;
		for (auto ch : specialChars)
		{
			if (orig[i] == ch)
			{
				isSpecChar = true;
				break;
			}
		}

		if (!isSpecChar)
		{
			fixed.append(orig[i]);
		}
	}
}

static bool hasNamePattern(const char* pwd, const char* name)
{
	if (i18n_isNull(name))
	{
		return false;
	}

	const PUniStringEx uPassword(pwd);

	PString strName(name);
	i18nTrim(strName);

	if (strName.length() == 0)
	{
		return false;
	}

	PUniStringEx origFullName(strName.c_str());
	if (uPassword.containsIgnoreCase(origFullName))
	{
		return true;
	}
	
	// remove all special chars from name
	PString fixedName;
	removeSpecialChars(strName, fixedName);
	PUniStringEx uName(fixedName.c_str());
	if (uName.length() > 0 && uPassword.containsIgnoreCase(uName))
	{
		return true;
	}

	// remove all special chars from password
	PString fixedPwd;
	removeSpecialChars(pwd, fixedPwd);
	const PUniStringEx uFixedPassword(fixedPwd);
	if (uName.length() > 0 && uFixedPassword.containsIgnoreCase(uName))
	{
		return true;
	}

	return false;
}

//	Copied (and re-formatted) from client/gui/DialogHelpers.cpp
EnumPasswordCheckRes checkPassword( UINT32 licenseId,
									const char* userId,
									const char* password,
									const char* firstName,
									const char* lastName,
									const char* secondLastName,
									const SrvDate& dateOfBirth ) // firstName, lastName and dateOfBirth are enforced if present
{
	static const INT32 minPasswordLen = 8;		//	currently the same for all licenses
	UINT32 ruleMask;
	INT32 minNumCharTypes;
	if (licenseId == eLicenceSpain)
	{
		ruleMask = ePwdCheckRules_firstNameNotAllowed | ePwdCheckRules_lastNameNotAllowed | ePwdCheckRules_dobNotAllowed | ePwdCheckRules_secondLastNameNotAllowed;
		minNumCharTypes = 3;
	}
	else
	{
		ruleMask = ePwdCheckRules_mustContainOneDigit;
		minNumCharTypes = 1;
	}

	PUniStringEx uPassword( password );
	static PUniStringEx tmpPwdPrefix( TEMP_PWD_PREFIX );

	if (uPassword.startsWith( tmpPwdPrefix ))
	{
		return pcrErrorIsTemporaryPassword;
	}

	const size_t passwordLen = i18n_strlen( password );

	if (passwordLen < minPasswordLen)
	{
		return pcrErrorTooShort;
	}

	if (passwordLen) //-V547
	{
		// check user id is part of password
		if (!i18n_isNull( userId ))
		{
			PUniStringEx uUserId(userId);
			if (uPassword.containsIgnoreCase( uUserId ))
			{
				return pcrErrorContainsUserID;
			}
		}

		// check first name is part of password
		if ((ruleMask & ePwdCheckRules_firstNameNotAllowed) != 0)
		{
			if (hasNamePattern(password, firstName))
			{
				return pcrErrorContainsFirstName;
			}
		}

		if ((ruleMask & ePwdCheckRules_lastNameNotAllowed) != 0)
		{
			// check last name is part of password
			if (hasNamePattern(password, lastName))
			{
				return pcrErrorContainsLastName;
			}
		}

		if ((ruleMask & ePwdCheckRules_secondLastNameNotAllowed) != 0)
		{
			// check last name is part of password
			if (hasNamePattern(password, secondLastName))
			{
				return pcrErrorContainsSecondLastName;
			}
		}

		if ((ruleMask & ePwdCheckRules_dobNotAllowed) != 0)
		{
			// check date of birth is part of password
			if (!dateOfBirth.isNull())
			{
				char year[8], month[8], day[8];
				sprintf( day, "%02u", dateOfBirth._day );
				sprintf( month, "%02u", dateOfBirth._month );
				sprintf( year, "%02u", dateOfBirth._year % 100 );

				// check ddmmyy
				PString dateOfBirthStr;
				dateOfBirthStr.append( day ).append( month ).append( year );

				PUniStringEx uDOB( dateOfBirthStr );
				if (uPassword.containsIgnoreCase(uDOB))
				{
					return pcrErrorContainsDateOfBirth;
				}
			}
		}

		if ((ruleMask & ePwdCheckRules_mustStartWithAlpha) != 0)
		{
			if (!('A' <= uPassword[0] && uPassword[0] <= 'Z')
				&& !('a' <= uPassword[0] && uPassword[0] <= 'z'))
			{
				return pcrErrorNotStartingWithAlphaChar;
			}
		}

		int requirementsMet = 0;
		for (const PUNICHAR* u = uPassword; *u != 0; ++u)
		{
			if ('0' <= *u && *u <= '9')
			{
				requirementsMet++;
				break;
			}
		}

		if ((ruleMask & ePwdCheckRules_mustContainOneDigit) != 0)
		{
			if (requirementsMet == 0)
			{
				return pcrErrorNotEnoughCharacterTypes;
			}
		}

		// check at least one upper case
		for (const PUNICHAR* u = uPassword; *u != 0; ++u)
		{
			if ('A' <= *u && *u <= 'Z')
			{
				requirementsMet++;
				break;
			}
		}

		// check at least one lower case
		for (const PUNICHAR* u = uPassword; *u != 0; ++u)
		{
			if ('a' <= *u && *u <= 'z')
			{
				requirementsMet++;
				break;
			}
		}

		// check at least one symbol
		for (const PUNICHAR* u = uPassword; *u != 0; ++u)
		{
			if (_isPasswordSymbolChar( *u ))
			{
				requirementsMet++;
				break;
			}
		}

		if (requirementsMet < minNumCharTypes)
		{
			return pcrErrorNotEnoughCharacterTypes;
		}
	}

	return pcrSucess;
}

