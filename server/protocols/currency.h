#ifndef	currency_h_included
#define	currency_h_included

//
// ISO 4217 currency code to numeric codes convertor 
// See http://www.xe.com/iso4217.htm
// ONLY PARTIAL LIST!!! Add new currencies to currency.cpp.
//

extern int CurrencyCode2number(const char* code);
extern const char* CurrencyNumber2code(int number);

inline unsigned char getEuroSign_1252()
{
	return 0x80;
}

inline unsigned short getEuroSignUni()
{
	return 0x20AC;
}

inline unsigned short getPoundSignUni()
{
	return 0x00A3;
}

inline unsigned short getINRSignUni()
{
	return 0x20B9;
}

#endif
