#ifndef fiscal_code_validation_h_included
#define fiscal_code_validation_h_included

#include "pyrtime.h"

class FiscalCodeValidation
{
private:
	static bool validArgentinaCUIT(const char* fiscalCode);
	static bool validBrazilCPF(const char* fiscalCode);
	static bool validChileRUT(const char* fiscalCode);
	// PYR-PYR-24107
	static bool validNorwayTaxId(const char* fiscalCode);
	static bool validTurkeyTaxId(const char* fiscalCode);

	// PYR-37984
	static bool verifyBGUCN(const char* code, const SrvDate& DOB);
	static bool verifyBGPNF(const char* code);
	static bool verifyBGPin(const char* code);

public:
	enum ePanValidationError
	{
		ePanValidationError_None = 0,
		ePanValidationError_Length = 1, // PAN length != 10
		ePanValidationError_LastNameMissing = 2, // User's last name is missing
		ePanValidationError_FirstThreeChars = 3, // first 3 chars of pan is not alphabetic
		ePanValidationError_WrongCardHolderType = 4, // wrong fourth char (cardholder type)
		ePanValidationError_FirstCharOfLastName = 5, // Pan's fifth char != Last Name's fist char
		ePanValidationError_NotADigit = 6, // chars form [6..9] must be digits
		ePanValidationError_AllDigitsAreZero = 7, // at list one char form [6..9] must be not Zero
		ePanValidationError_WrongCheckDigit = 8, // last char must be alphabetic
	};

	// If country requires fiscal code and in your case fiscalCode is optional
	// then check it for empty string and do not call this function.
	static bool isValid(const char* fiscalCode, const char* country);

	// PYR-43111, check if Romanian fiscalCode matches DateOfBirth
	static bool validRomanianCNPwithDOB(const char* romanianFiscalCode, const SrvDate& DOB); // license-based, only for eLicenceRomania

	// PYR-37984, check if Bulgarian fiscal code is valid
	static bool validBulgarianFC(INT32 bgFiscalCodeType, const char* bulgarianFiscalCode, const SrvDate& DOB); // license-based, only for eLicenceBG

	// PYR-50483, check if Portugal fiscal code (NIF id) is valid
	static bool validPortugalFC(const char* fiscalCode); // license-based, only for eLicencePortugal

	// PYR-53001, check if Indian PAN is valid
	static bool validIndianPAN(const char* pan, const char* lastName);
	static ePanValidationError getIndianPanValidationError(const char* pan, const char* lastName); // returns enum ePanValidationError

	// PYR-75125
	static bool validateSwedishPersonnummer(const char* personNumber);
	static bool getSexFromSwedishPersonnummer(const char* fullPersonNumber, PString& sex);
	static bool validateSwedishPersonnummerWithDOB(const char* personNumber, const SrvDate& DOB, PString& personNumber12digits, PString& sex);
};

#endif // fiscal_code_validation_h_included
