
#include "fiscalcodevalidation.h"
#include "ppstring.h"
#include "shared.h"

// Consider to express validation parsing using regular expressions
bool FiscalCodeValidation::isValid(const char* fiscalCode_, const char* country)
{
	PString fc(PString::ltrim(fiscalCode_), PString::rtrim(fiscalCode_));
	// Remove delimiters
	PString fiscalCode;
	for (int i = 0; i < fc.length(); ++i)
	{
		switch (fc[i])
		{
		case '.':
		case '-':
			continue;
		default:
			fiscalCode.append(fc[i]);
		}
	}
	if (PString::compareIgnoreCase(country, "AR") == 0)
	{
		return validArgentinaCUIT(fiscalCode);
	}
	else if (PString::compareIgnoreCase(country, "BR") == 0)
	{
		return validBrazilCPF(fiscalCode);
	}
	else if (PString::compareIgnoreCase(country, "CL") == 0)
	{
		return validChileRUT(fiscalCode);
	}
	else if (PString::compareIgnoreCase(country, "NO") == 0) // PYR-24107
	{
		return validNorwayTaxId(fiscalCode);
	}
	else if (PString::compareIgnoreCase(country, "TR") == 0) // PYR-24107
	{
		return validTurkeyTaxId(fiscalCode);
	}
	return true; // not validated
}

// Argentina CUIT is [20|23|24|27|30|33|34]-99999999-9
bool FiscalCodeValidation::validArgentinaCUIT(const char* fiscalCode)
{
	size_t len = PString::length(fiscalCode);
	if (len != 11)
	{
		return false;
	}
	PString str; // digital code without delimiters used for calculation
	for (size_t i = 0; i < len; ++i)
	{
		if (i == 0 && fiscalCode[i] != '2'/*&& fiscalCode[i] != '3'*/) // 3 for business tax id
		{
			return false;
		}
		if (i == 1 && fiscalCode[i] != '0' && fiscalCode[i] != '3' && fiscalCode[i] != '4' && fiscalCode[i] != '7')
		{
			return false;
		}
		if (i <= 9 && (fiscalCode[i] < '0' || fiscalCode[i] > '9'))
		{
			return false;
		}
		if (i == 10)
		{
			 if (fiscalCode[i] < '0' || fiscalCode[i] > '9')
			 {
				 return false;
			 }
			 continue;
		}
		str.append(fiscalCode[i]);
	}
	PASSERT5(str.length() == 10);
	UINT32 total = 0;
	int coefs[10] = {5,4,3,2,7,6,5,4,3,2};
	for (int i = 0; i < 10; ++i)
	{
		total += (str[i] - '0') * coefs[i];
	}
	int rest = 11 - (total % 11);
	if (rest == 11)
	{
		rest = 0;
	}
	if (rest == 10)
	{
		rest = 9;
	}
	return ('0' + rest == fiscalCode[10]);
}

// Brazilian CPF is 999.999.999-99
bool FiscalCodeValidation::validBrazilCPF(const char* fiscalCode)
{
	size_t len = PString::length(fiscalCode);
	if (len != 11)
	{
		return false;
	}
	PString str; // digital code without delimiters used for calculation
	for (size_t i = 0; i < len; ++i)
	{
		if (i <= 8 && (fiscalCode[i] < '0' || fiscalCode[i] > '9'))
		{
			return false;
		}
		if (i == 9 || i == 10)
		{
			if (fiscalCode[i] < '0' || fiscalCode[i] > '9')
			{
				return false;
			}
			continue; // exclude from calculation
		}
		str.append(fiscalCode[i]);
	}
	PASSERT5(str.length() == 9);
	UINT32 total1 = 0, total2 = 0;
	for (int i = 0; i < 9; ++i)
	{
		int d = str[i] - '0';
		total1 += d * (10 - i);
		total2 += d * (11 - i);
	}
	int rest1 = 11 - (total1 % 11);
	if (rest1 >= 10)
	{
		rest1 = 0;
	}
	total2 += (rest1 * 2);
	int rest2 = 11 - (total2 % 11);
	if (rest2 >= 10)
	{
		rest2 = 0;
	}
	return (fiscalCode[9] == '0' + rest1 && fiscalCode[10] == '0' + rest2 /*&& rest1 != rest2 / * PYR-23125 * / */ );
}

// Chilean RUT is [9]9.999.999-X where 9 - is [0-9], X is [0-9] or K
bool FiscalCodeValidation::validChileRUT(const char* fiscalCode_)
{
	PString fiscalCode(fiscalCode_);
	while (fiscalCode.length() < 9)
	{
		fiscalCode = PString("0").append(fiscalCode);
	}
	if (fiscalCode.length() != 9 || fiscalCode.equals("000000000"))
	{
		return false;
	}

	PString str; // digital code without delimiters used for calculation
	for (int i = 0; i < fiscalCode.length(); ++i)
	{
		if (i <= 7 && (fiscalCode[i] < '0' || fiscalCode[i] > '9'))
		{
			return false;
		}
		if (i == 8)
		{
			if (fiscalCode[i] >= '0' && fiscalCode[i] <= '9' || fiscalCode[i] == 'K' || fiscalCode[i] == 'k')
			{
				continue; // exclude from calculation
			}
			return false;
		}
		str.append(fiscalCode[i]);
	}
	PASSERT5(str.length() == 8);
	UINT32 total = 0;
	int coefs[8] = {3,2,7,6,5,4,3,2};
	for (int i = 0; i < 8; ++i)
	{
		total += (str[i] - '0') * coefs[i];
	}

	int rest = 11 - (total % 11);
	if (rest == 11)
	{
		return (fiscalCode[8] == '0');
	}
	else if (rest == 10)
	{
		return (fiscalCode[8] == 'K' || fiscalCode[8] == 'k');
	}
	return ('0' + rest == fiscalCode[8]);
}

// PYR-24107
// NOTE: Algorithm provided by Harald Hanche-Olsen (han...@imf.unit.no) 
// AUTHOR: Stein J. Rypern <stei...@oslonett.no> 
bool FiscalCodeValidation::validNorwayTaxId(const char* fiscalCode)
{
	if (PString::length(fiscalCode) != 11)
	{
		return false;
	}
	for (int i = 0; i < 11; ++i)
	{
		if (fiscalCode[i] < '0' || fiscalCode[i] > '9')
		{
			return false;
		}
	}

	long sum1 = 0, sum2 = 0;  //-V126
	int weights1[] = {3, 7, 6, 1, 8, 9, 4, 5, 2, 1, 0}; 
	int weights2[] = {5, 4, 3, 2, 7, 6, 5, 4, 3, 2, 1}; 
	int digit; 
	for (int i = 0; i < 11; ++i) 
	{
		digit = (int)(fiscalCode[i] - '0'); 
		sum1 += digit * weights1[i]; 
		sum2 += digit * weights2[i]; 
	} 
	return (((sum1 % 11) == 0) && ((sum2 % 11) == 0)); 
}

// PYR-24107
bool FiscalCodeValidation::validTurkeyTaxId(const char* fiscalCode)
{
	if (PString::length(fiscalCode) != 11)
	{
		return false;
	}
	if (fiscalCode[0] < '1' || fiscalCode[0] > '9')
	{
		return false;
	}
	for (int i = 1; i < 11; ++i)
	{
		if (fiscalCode[i] < '0' || fiscalCode[i] > '9')
		{
			return false;
		}
	}
	int digit[9];
	for (int i = 0; i < 9; ++i)
	{
		digit[i] = (int)(fiscalCode[i] - '0'); 
	}

	int x10 = 3 * (digit[0] + digit[2] + digit[4] + digit[6] + digit[8]) + digit[1] + digit[3] + digit[5] + digit[7];
	x10 = (10 - (x10 % 10)) % 10;
	if (x10 + '0' != fiscalCode[9])
	{
		return false;
	}
	int x11 = digit[0] + digit[2] + digit[4] + digit[6] + digit[8] + 3 * (digit[1] + digit[3] + digit[5] + digit[7] + x10);
	x11 = (10 - (x11 % 10)) % 10;
	return (x11 + '0' == fiscalCode[10]);
}

/// PYR-43111, check validity of CNP number, see BRD for details
static bool validRomanianCNP(const char* fiscalCode)
{
	if (PString::length(fiscalCode) != 13)
	{
		return false;
	}

	const int DIGITS_TO_MUL = 12;
	static const INT8 mulDigits[DIGITS_TO_MUL] = {2,7,9,1,4,6,3,5,8,2,7,9};

	int totalSum = 0;
	for (int i = 0; i < DIGITS_TO_MUL; i++)
	{
		int currentDigit = fiscalCode[i] - '0';
		if (currentDigit < 0 || currentDigit > 9)
			return false;

		totalSum += currentDigit * mulDigits[i];
	}

	int checkDigit = totalSum % 11;
	if (checkDigit == 10)
	{
		checkDigit = 1;
	}

	int lastDigit = fiscalCode[DIGITS_TO_MUL] - '0';
	if (lastDigit < 0 || lastDigit > 9)
		return false;

	return lastDigit == checkDigit;
}

static inline char digitToChar(UINT32 digit)
{
	return char(digit + '0');
}

// PYR-43111, check if Romanian fiscalCode matches DateOfBirth
bool FiscalCodeValidation::validRomanianCNPwithDOB(const char* romanianFiscalCode, const SrvDate& DOB)
{
	const char* cnp = romanianFiscalCode;
	if (DOB.isNull())
	{
		return false;
	}

	if (!validRomanianCNP(cnp))
	{
		return false;
	}

	// 'romanianFiscalCode' is 13 digits string here
	char YY = digitToChar((DOB._year % 100) / 10); // first digit of 2-digit year number
	char Y  = digitToChar(DOB._year  % 10);        // last digit of 2-digit year number

	char MM = digitToChar((DOB._month % 100) / 10);// first digit of 2-digit month number
	char M  = digitToChar(DOB._month  % 10);       // last digit of 2-digit month number

	char DD = digitToChar((DOB._day % 100) / 10);  // first digit of 2-digit month number
	char D  = digitToChar(DOB._day  % 10);         // last digit of 2-digit month number

	bool yOk = (YY == cnp[1]) && (Y == cnp[2]);
	bool mOk = (MM == cnp[3]) && (M == cnp[4]);
	bool dOk = (DD == cnp[5]) && (D == cnp[6]);

	return yOk && mOk && dOk;
}

bool FiscalCodeValidation::verifyBGUCN(const char* code, const SrvDate& DOB)
{
	if (DOB.isNull() || !code || strlen(code) != 10)
	{
		PLog("!UCN Code or DOB");
		return false;
	}

	// The UCN digits in order of 
	// 1, 2, 3, 4, 5, 6, 7, 8, 9, respectively, are multiplied by 
	// 2, 4, 8, 5, 10, 9, 7, 3, 6, 
	// the products are summed 
	const int weightsMap[] = {2, 4, 8, 5, 10, 9, 7, 3, 6, 0}; // The weights are the powers of 2, modulo 11; last 0 added for convenience - see loop below
	int sumSpecIndexProds = 0;	
	for(int i = 0; i < 10; ++i)
	{
		if(!isdigit(code[i]))
		{
			PLog("!UCN NonDigit");
			return false;
		}
		sumSpecIndexProds += (code[i] - '0') * weightsMap[i];
	}

	// the result is divided by 11. If the remainder = 10 or 0 - the check (10th) digit is 0, otherwise it is equal to the remainder.
	if(sumSpecIndexProds % 11 % 10 != (code[9] - '0'))
	{
		PLog("!UCN Checksum");
		return false;
	}

	// Digits 1, 2 -> DOB Year, last two digits 
	int yearLast2 = DOB._year % 100; // last 2 digits
	int yearUCN = (code[0] - '0') * 10 + (code[1] - '0');
	if(yearLast2 != yearUCN)
	{
		PLog("!UCN Year");
		return false;
	}

	// Digits 3, 4 -> DOB month. For post 31.12.1999 it is incremented by 40; for DOB in 1800-1899 it is incremented by 20
	int monthUCN = (code[2] - '0') * 10 + (code[3] - '0');
	if(DOB._year < 1900)
	{
		monthUCN -= 20;
	}
	else if(DOB._year > 1999) 
	{
		monthUCN -= 40;
	}
	if(DOB._month != monthUCN)
	{
		PLog("!UCN Month");
		return false;
	}

	// Digits 5 6 -> DOB month date
	int dayUCN = (code[4] - '0') * 10 + (code[5] - '0');
	if(DOB._day != dayUCN)
	{
		PLog("!UCN Day");
		return false;
	}
	return true;
}
	
bool FiscalCodeValidation::verifyBGPNF(const char* code)
{
	if(!code || strlen(code) != 10)
	{
		PLog("!PNF Code");
		return false;
	}

	// The digits in order of 
	// 1, 2, 3, 4, 5, 6, 7, 8, 9 are multiplied respectively 
	// 21, 19, 17, 13, 11, 9, 7, 3, 1, 
	// the products are summed
	const int weightsMap[] = {21, 19, 17, 13, 11, 9, 7, 3, 1, 0}; // hardcoded numbers, last 0 added for convenience
	int sumSpecIndexProds = 0;	
	for(int i = 0; i < 10; ++i)
	{
		if(!isdigit(code[i]))
		{
			PLog("!PNF Code");
			return false;
		}
		sumSpecIndexProds += (code[i] - '0') * weightsMap[i];
	}

	// the result is divided by 10. The remainder goes to the the check (10th) digit.
	if(sumSpecIndexProds % 10 != (code[9] - '0'))
	{
		PLog("!PNF Code");
		return false;
	}
	return true;
}

bool FiscalCodeValidation::verifyBGPin(const char* code)
{
	if(!code || strlen(code) != 10)
	{
		PLog("!PIN Code");
		return false;
	}
	for(int i = 0; i < 10; ++i)
	{
		if(!isdigit(code[i]))
		{
			PLog("!PIN Code");
			return false;
		}
	}
	return true;
}

bool FiscalCodeValidation::validBulgarianFC(INT32 bgFiscalCodeType, const char* bulgarianFiscalCode, const SrvDate& DOB)
{
	switch(bgFiscalCodeType)
	{
	case 1: // UCN/EGN - citizens or permanent residents
		return verifyBGUCN(bulgarianFiscalCode, DOB);
	case 2: // PNF - foreigners
		return verifyBGPNF(bulgarianFiscalCode);
	case 3: // PIN
		return verifyBGPin(bulgarianFiscalCode);
	default:
		{
			PLog("!FC Type");
			return false;
		}
	}
}

bool FiscalCodeValidation::validPortugalFC(const char* fiscalCode)
{
	if (fiscalCode && *fiscalCode && (fiscalCode[0] == '1' || fiscalCode[0] == '2' || fiscalCode[0] == '3')) // PYR-114837: added fiscalCode '3'
		return true;
	PLog("!valid PT FC");
	return false;
}

// NOTE: function expects PAN in upper case
FiscalCodeValidation::ePanValidationError FiscalCodeValidation::getIndianPanValidationError(const char* pan, const char* lastName) // PYR-53001
{
	int panLen = pan ? strlen(pan) : 0;
	if(!pan || panLen != 10)
	{
		PLog("pan.len=%d!=10", panLen);
		return ePanValidationError_Length;
	}
	if(!lastName || !*lastName)
	{
		PLog("!lastName");
		return ePanValidationError_LastNameMissing;
	}

	const char* cardholderTypes = "ABCFGHLJPTK";
	bool allDigitsAreZero = true;
	for (int i = 0; i < 10; ++i)
	{
		char panChar = pan[i];
		if (i < 3 && (panChar < 'A' || panChar > 'Z')) // first three letters are sequence of alphabets from AAA to ZZZ
		{
			return ePanValidationError_FirstThreeChars;
		}
		else if (i == 3 && !PString::find(cardholderTypes, panChar)) // fourth character informs about the type of holder of the card. Must be one from "ABCFGHLJPTK"
		{
			return ePanValidationError_WrongCardHolderType;
		}
		else if (i == 4 && panChar != PString::toUpper(lastName[0])) // The fifth character of the PAN must be equal to the first character of last name
		{
			return ePanValidationError_FirstCharOfLastName;
		}
		else if (i >=5 && i <= 8) // not a number
		{
			if (!isdigit(panChar))
				return ePanValidationError_NotADigit;
			if (panChar != '0')
				allDigitsAreZero = false;
		}
		else if (i == 9 && (panChar < 'A' || panChar > 'Z')) // The last character is an alphabetic check digit.
		{
			return ePanValidationError_WrongCheckDigit;
		}
	}
	if (allDigitsAreZero)
		return ePanValidationError_AllDigitsAreZero;
	return ePanValidationError_None;
}

bool FiscalCodeValidation::validIndianPAN(const char* pan, const char* lastName) // PYR-53001
{
	return (getIndianPanValidationError(pan, lastName) == ePanValidationError_None);
}

bool FiscalCodeValidation::validateSwedishPersonnummer(const char* personNumber) // assumes 12 digits
{
	if (strlen(personNumber) != 12)
	{
		PLog("!len");
		return false;
	}
	const int weightsMap[] = { 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2 };
	int sumProducts = 0;
	for (int i = 0; i <= 10; ++i)
	{
		if (!isdigit(personNumber[i]))
		{
			PLog("!digit");
			return false;
		}
		int curProduct = (personNumber[i] - '0') * weightsMap[i];
		if (curProduct > 9)
			curProduct = curProduct / 10 + curProduct % 10;
		sumProducts += curProduct;
	}
	int checkSum = (10 - (sumProducts % 10)) % 10;
	if (checkSum != (personNumber[11] - '0'))
	{
		PLog("!chkSum");
		return false;
	}
	return true;
}

bool FiscalCodeValidation::getSexFromSwedishPersonnummer(const char* fullPersonNumber, PString& sex)
{
	if (strlen (fullPersonNumber) != 12)
		return false;
	int genderNum = (fullPersonNumber[10] - '0') % 2;
	if (genderNum == 0)
	{
		sex = "F";
	}
	else
	{
		sex = "M";
	}
	return true;
}

bool FiscalCodeValidation::validateSwedishPersonnummerWithDOB(const char* personNumber, const SrvDate& DOB, PString& personNumber12digits, PString& sex)
{
	personNumber12digits = "";

	if (DOB.isNull())
	{
		PLog("dob is null");
		return false;
	}

	char YYYY = digitToChar((DOB._year % 10000) / 1000); // first digit of 4-digit year number
	char YYY = digitToChar((DOB._year % 1000) / 100);        // second digit of 2-digit year number
	char YY = digitToChar((DOB._year % 100) / 10); // first digit of 2-digit year number, third digit of 4-digit year number 
	char Y = digitToChar(DOB._year % 10);        // last digit of 4- and 2-digit year number

	char MM = digitToChar((DOB._month % 100) / 10);// first digit of 2-digit month number
	char M = digitToChar(DOB._month % 10);       // last digit of 2-digit month number

	char DD = digitToChar((DOB._day % 100) / 10);  // first digit of 2-digit month number
	char D = digitToChar(DOB._day % 10);         // last digit of 2-digit month number

	PString fullPersonNumber = "";
	if (strlen (personNumber) == 10) // only 2 digits for year
	{
		fullPersonNumber.assign("").append(YYYY).append(YYY).append(personNumber);
	}
	else
	{
		fullPersonNumber = personNumber;
	}

	if (!validateSwedishPersonnummer(fullPersonNumber))
		return false;

	bool yOk = (YYYY == fullPersonNumber[0]) && (YYY == fullPersonNumber[1]) && (YY == fullPersonNumber[2]) && (Y == fullPersonNumber[3]);
	bool mOk = (MM == fullPersonNumber[4]) && (M == fullPersonNumber[5]);
	bool dOk = (DD == fullPersonNumber[6]) && (D == fullPersonNumber[7]);

	if (!yOk || !mOk || !dOk)
	{
		PLog("!dob");
		return false;
	}

	personNumber12digits = fullPersonNumber;
	return getSexFromSwedishPersonnummer(fullPersonNumber, sex);
}


