#include "ppinclude.h"
#include "ppstring.h"
#include "ppi18n.h"
#include "ppi18n_codepage.h"

static const PUNICHAR _win1252Table[] = { //see http://www.microsoft.com/typography/unicode/1252.htm
	//IMPORTANT: MUST re-generate _win1252RTable if changing _win1252Table
	//           x0      x1      x2      x3      x4      x5      x6      x7      x8      x9      xA      xB      xC      xD      xE      xF
	/* 8x */ 0x20AC, 0x0000, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x017D, 0x0000, 
	/* 9x */ 0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0000, 0x017E, 0x0178,
	/* Ax */ 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 
	/* Bx */ 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 
	/* Cx */ 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 
	/* Dx */ 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 
	/* Ex */ 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 
	/* Fx */ 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

static const PStreamEncodingCodePage::ReverseTableItem _win1252RTable[] = {
	//AUTO-GENERATED from _win1252Table using _ppcodepage.cpp
	//MUST be re-generated if _win1252Table changes
	//{ *********************** Win1252-R **********************
	{ 0x00A0, 0xA0 },
	{ 0x00A1, 0xA1 },
	{ 0x00A2, 0xA2 },
	{ 0x00A3, 0xA3 },
	{ 0x00A4, 0xA4 },
	{ 0x00A5, 0xA5 },
	{ 0x00A6, 0xA6 },
	{ 0x00A7, 0xA7 },
	{ 0x00A8, 0xA8 },
	{ 0x00A9, 0xA9 },
	{ 0x00AA, 0xAA },
	{ 0x00AB, 0xAB },
	{ 0x00AC, 0xAC },
	{ 0x00AD, 0xAD },
	{ 0x00AE, 0xAE },
	{ 0x00AF, 0xAF },
	{ 0x00B0, 0xB0 },
	{ 0x00B1, 0xB1 },
	{ 0x00B2, 0xB2 },
	{ 0x00B3, 0xB3 },
	{ 0x00B4, 0xB4 },
	{ 0x00B5, 0xB5 },
	{ 0x00B6, 0xB6 },
	{ 0x00B7, 0xB7 },
	{ 0x00B8, 0xB8 },
	{ 0x00B9, 0xB9 },
	{ 0x00BA, 0xBA },
	{ 0x00BB, 0xBB },
	{ 0x00BC, 0xBC },
	{ 0x00BD, 0xBD },
	{ 0x00BE, 0xBE },
	{ 0x00BF, 0xBF },
	{ 0x00C0, 0xC0 },
	{ 0x00C1, 0xC1 },
	{ 0x00C2, 0xC2 },
	{ 0x00C3, 0xC3 },
	{ 0x00C4, 0xC4 },
	{ 0x00C5, 0xC5 },
	{ 0x00C6, 0xC6 },
	{ 0x00C7, 0xC7 },
	{ 0x00C8, 0xC8 },
	{ 0x00C9, 0xC9 },
	{ 0x00CA, 0xCA },
	{ 0x00CB, 0xCB },
	{ 0x00CC, 0xCC },
	{ 0x00CD, 0xCD },
	{ 0x00CE, 0xCE },
	{ 0x00CF, 0xCF },
	{ 0x00D0, 0xD0 },
	{ 0x00D1, 0xD1 },
	{ 0x00D2, 0xD2 },
	{ 0x00D3, 0xD3 },
	{ 0x00D4, 0xD4 },
	{ 0x00D5, 0xD5 },
	{ 0x00D6, 0xD6 },
	{ 0x00D7, 0xD7 },
	{ 0x00D8, 0xD8 },
	{ 0x00D9, 0xD9 },
	{ 0x00DA, 0xDA },
	{ 0x00DB, 0xDB },
	{ 0x00DC, 0xDC },
	{ 0x00DD, 0xDD },
	{ 0x00DE, 0xDE },
	{ 0x00DF, 0xDF },
	{ 0x00E0, 0xE0 },
	{ 0x00E1, 0xE1 },
	{ 0x00E2, 0xE2 },
	{ 0x00E3, 0xE3 },
	{ 0x00E4, 0xE4 },
	{ 0x00E5, 0xE5 },
	{ 0x00E6, 0xE6 },
	{ 0x00E7, 0xE7 },
	{ 0x00E8, 0xE8 },
	{ 0x00E9, 0xE9 },
	{ 0x00EA, 0xEA },
	{ 0x00EB, 0xEB },
	{ 0x00EC, 0xEC },
	{ 0x00ED, 0xED },
	{ 0x00EE, 0xEE },
	{ 0x00EF, 0xEF },
	{ 0x00F0, 0xF0 },
	{ 0x00F1, 0xF1 },
	{ 0x00F2, 0xF2 },
	{ 0x00F3, 0xF3 },
	{ 0x00F4, 0xF4 },
	{ 0x00F5, 0xF5 },
	{ 0x00F6, 0xF6 },
	{ 0x00F7, 0xF7 },
	{ 0x00F8, 0xF8 },
	{ 0x00F9, 0xF9 },
	{ 0x00FA, 0xFA },
	{ 0x00FB, 0xFB },
	{ 0x00FC, 0xFC },
	{ 0x00FD, 0xFD },
	{ 0x00FE, 0xFE },
	{ 0x00FF, 0xFF },
	{ 0x0152, 0x8C },
	{ 0x0153, 0x9C },
	{ 0x0160, 0x8A },
	{ 0x0161, 0x9A },
	{ 0x0178, 0x9F },
	{ 0x017D, 0x8E },
	{ 0x017E, 0x9E },
	{ 0x0192, 0x83 },
	{ 0x02C6, 0x88 },
	{ 0x02DC, 0x98 },
	{ 0x2013, 0x96 },
	{ 0x2014, 0x97 },
	{ 0x2018, 0x91 },
	{ 0x2019, 0x92 },
	{ 0x201A, 0x82 },
	{ 0x201C, 0x93 },
	{ 0x201D, 0x94 },
	{ 0x201E, 0x84 },
	{ 0x2020, 0x86 },
	{ 0x2021, 0x87 },
	{ 0x2022, 0x95 },
	{ 0x2026, 0x85 },
	{ 0x2030, 0x89 },
	{ 0x2039, 0x8B },
	{ 0x203A, 0x9B },
	{ 0x20AC, 0x80 },
	{ 0x2122, 0x99 },
	//} *********************** Win1252-R **********************
};
static const PStreamEncodingCodePage::ReverseTableItem* _win1252RTableEnd = _win1252RTable + sizeof(_win1252RTable)/sizeof(_win1252RTable[ 0 ]);

/* static */ const PUNICHAR* PStreamEncodingWin1252::table = _win1252Table;

/* static */ int PStreamEncodingWin1252::_staticReadChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd )
{
	return _Parent::_staticReadChars( table, dst, dstEnd, src, srcEnd );
}

/* virtual */ int PStreamEncodingWin1252::_readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd ) const
{
	return _staticReadChars( dst, dstEnd, src, srcEnd );
}

/* static */ int PStreamEncodingWin1252::_staticWriteChar( PUNICHAR_32 c )
{
	return _Parent::_staticWriteChar( _win1252RTable, _win1252RTableEnd, c );
}

///* static */ PStreamEncodingWin1252 PStreamEncodingWin1252::instance;

//****************************************************************************

PStringEncodingWin1252::PStringEncodingWin1252( BYTE flags_, PUNICHAR defaultChar_ )
: PStringEncodingCodePage( _win1252Table, _win1252RTable, _win1252RTableEnd, flags_, defaultChar_ )
{
}

///* static */ PStringEncodingWin1252 PStringEncodingWin1252::instance;
