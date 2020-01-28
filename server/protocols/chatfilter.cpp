#include "chatfilter.h"
#include "ppi18n_x.h"
#if PTRACELVL >= 5
 #include "i18n.utils.h"
 #include "pplogfile.h"
#endif

/*---------------------------------------------------------------------------------------------
				normalizeUniString (by SK) based on Win-1252
---------------------------------------------------------------------------------------------*/
static void normalizeUniString( PUniString& ret )
{
	PUniString buf;
	for( const PUNICHAR* p = ret; *p; ++p )
	{
		switch( *p )
		{
			case 0x00A0: //NBSP
				buf.append( ' ' );
				break;

			case 0x00A1: //INVERTED EXCLAMATION MARK
			case 0x00CC: //CAPITAL I WITH GRAVE
			case 0x00CD: //CAPITAL I WITH ACUTE
			case 0x00CE: //CAPITAL I WITH CIRCUMFLEX
			case 0x00CF: //CAPITAL I WITH DIAERESIS
			case 0x00EC: //SMALL i WITH GRAVE
			case 0x00ED: //SMALL i WITH ACUTE
			case 0x00EE: //SMALL i WITH CIRCUMFLEX
			case 0x00EF: //SMALL i WITH DIAERESIS
				buf.append( 'i' );
				break;

			case 0x00A2: //CENT SIGN
			case 0x00C7: //CAPITAL C WITH CEDILLA
			case 0x00E7: //SMALL c WITH CEDILLA
				buf.append( 'c' );
				break;

			case 0x00A3: //POUND SIGN
				buf.append( 'f' );
				break;

			case 0x0024: //DOLLAR SIGN
			case 0x00A7: //SECTION SIGN
				buf.append( 's' );
				break;

			case 0x00B5: //MICRO SIGN
				buf.append( 'm' );
				break;

			case 0x00C0: //CAPITAL A WITH GRAVE
			case 0x00C1: //CAPITAL A WITH ACUTE
			case 0x00C2: //CAPITAL A WITH CIRCUMFLEX
			case 0x00C3: //CAPITAL A WITH TILDE
			case 0x00C4: //CAPITAL A WITH DIAERESIS
			case 0x00C5: //CAPITAL A WITH RING ABOVE
			case 0x00E0: //SMALL a WITH GRAVE
			case 0x00E1: //SMALL a WITH ACUTE
			case 0x00E2: //SMALL a WITH CIRCUMFLEX
			case 0x00E3: //SMALL a WITH TILDE
			case 0x00E4: //SMALL a WITH DIAERESIS
			case 0x00E5: //SMALL a WITH RING ABOVE
			case 0x0410: // Russian capital A
			case 0x0430: // Russian small a
				buf.append( 'a' );
				break;

			case 0x00C8: //CAPITAL E WITH GRAVE
			case 0x00C9: //CAPITAL E WITH ACUTE
			case 0x00CA: //CAPITAL E WITH CIRCUMFLEX
			case 0x00CB: //CAPITAL E WITH DIAERESIS
			case 0x00E8: //SMALL e WITH GRAVE
			case 0x00E9: //SMALL e WITH ACUTE
			case 0x00EA: //SMALL e WITH CIRCUMFLEX
			case 0x00EB: //SMALL e WITH DIAERESIS
			case 0x0415: // Russian capital E
			case 0x0435: // Russian small e
				buf.append( 'e' );
				break;

			case 0x00D0: //CAPITAL LETTER ETH (D)
			case 0x00F0: //SMALL LETTER ETH (d)
				buf.append( 'd' );
				break;

			case 0x00D1: //CAPITAL N WITH TILDE
			case 0x00F1: //SMALL n WITH TILDE
				buf.append( 'n' );
				break;

			case 0x00A4: //CURRENCY SIGN
			case 0x00D2: //CAPITAL O WITH GRAVE
			case 0x00D3: //CAPITAL O WITH ACUTE
			case 0x00D4: //CAPITAL O WITH CIRCUMFLEX
			case 0x00D5: //CAPITAL O WITH TILDE
			case 0x00D6: //CAPITAL O WITH DIAERESIS
			case 0x00D8: //CAPITAL O WITH STROKE
			case 0x00F2: //SMALL o WITH GRAVE
			case 0x00F3: //SMALL o WITH ACUTE
			case 0x00F4: //SMALL o WITH CIRCUMFLEX
			case 0x00F5: //SMALL o WITH TILDE
			case 0x00F6: //SMALL o WITH DIAERESIS
			case 0x00F8: //SMALL o WITH STROKE
			case 0x041E: // Russian capital O
			case 0x043E: // Russian small o
				buf.append( 'o' );
				break;

			case 0x00D7: //MULTIPLICATION SIGN
				buf.append( 'x' );
				break;

			case 0x00D9: //CAPITAL U WITH GRAVE
			case 0x00DA: //CAPITAL U WITH ACUTE
			case 0x00DB: //CAPITAL U WITH CIRCUMFLEX
			case 0x00DC: //CAPITAL U WITH DIAERESIS
			case 0x00F9: //SMALL u WITH GRAVE
			case 0x00FA: //SMALL u WITH ACUTE
			case 0x00FB: //SMALL u WITH CIRCUMFLEX
			case 0x00FC: //SMALL u WITH DIAERESIS
				buf.append( 'u' );
				break;

			case 0x00A5: //YEN SIGN
			case 0x00DD: //CAPITAL Y WITH ACUTE
			case 0x00FD: //SMALL y WITH ACUTE
			case 0x00FF: //SMALL y WITH DIAERESIS
				buf.append( 'y' );
				break;

			case 0x00DE: //CAPITAL LETTER THORN
			case 0x00FE: //SMALL LETTER THORN
				buf.append( 'p' );
				break;

			case 0x00DF: //SMALL SHARP S (BETA)
				buf.append( 'b' );
				break;

			//Questionable:	//??
			//case 0x00A9: //COPYRIGHT SIGN					(o|c)
			//case 0x00AA: //FEMININE ORDINAL INDICATOR		(a)
			//case 0x00BA: //MASCULINE ORDINAL INDICATOR	(o)
			//case 0x00AE: //REGISTERED SIGN				(r)
			//case 0x00B0: //DEGREE SIGN					(o)
			//case 0x00B6: //PILCROW SIGN					(p)
			//case 0x00C6: //ASH /LATIN CAPITAL LETTER AE	(a|e)
			//case 0x00E6: //ASH /LATIN SMALL LETTER ae		(a|e)

			default:
			{
				//buf.append( PUniString::_toLower( *p ) );
				if( *p >= 'A' && *p <= 'Z' )
					buf.append( *p + 'a' - 'A' );
				else
				buf.append( *p );
			}
		}
	}

	ret.moveFrom( buf );
}

static void _normlizeAbusiveWords( const vector< PString >& myPattern, vector< PUniString >& myPatternUni, size_t& blackListSize )
{
	blackListSize = myPattern.size();
	myPatternUni.clear();
	myPatternUni.reserve(myPattern.size());
	for( size_t i = 0; i < myPattern.size() ; ++i )
	{
		if (myPattern[ i ].equals(WHITELISTSEPARATOR))
		{
			if (blackListSize == myPattern.size())
			{
				blackListSize = i;
			}
			continue;
		}

		PStringX::StringParser parser;
		parser.init( myPattern[ i ] );

		PUniString buf;
		while( PUNICHAR_RET c = parser.nextChar() )
		{
			if( 0 < c )
				buf.append( c );
			else
				break;
		}

		if( buf.length() )
		{
			if (i < blackListSize)
			{
				normalizeUniString( buf );
			}
			//PYR-41470, remove the leading and ending white spaces
			myPatternUni.push_back(PUniString(buf.ltrim(), buf.rtrim()));
		}
	} // for
}

#if PTRACELVL >= 5
// Returns number of bytes written to utf8 or -1 if error. Adds a zero byte to
// the end of the string. Code points above U+FFFF will not be processed.
// Ignore surrogate code points. utf8 buffer must be at least 4 byte size.
static int ucs2Char2utf8Char(PUNICHAR ucs2, unsigned char* utf8)
{
	if (ucs2 < 0x80)
	{
		utf8[0] = ucs2;
		utf8[1] = 0;
		return 1;
	}
	if (ucs2 >= 0x80 && ucs2 < 0x800) //-V560
	{
		utf8[0] = (ucs2 >> 6)   | 0xC0;
		utf8[1] = (ucs2 & 0x3F) | 0x80;
		utf8[2] = 0;
		return 2;
	}
	if (ucs2 >= 0x800 && ucs2 < 0xFFFF) //-V560
	{
		utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
		utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
		utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
		utf8[3] = 0;
		return 3;
	}
	return -1;
}

void ChatFilterHelper::PUniString2PStringX(PString& ret, const PUniString& s)
{
	ret.cut(0);
	int l = s.length();
	char buf[16];
	for (int j = 0; j < l; ++j)
	{
		if (ucs2Char2utf8Char(s[j], (unsigned char*)buf) > 0)
		{
			i18nAppend(ret, buf);
		}
		else
		{
			i18nAppend(ret, "...");
			break;
		}
	}
}

void ChatFilterHelper::dump()
{
	PLog("===black list===");
	for (size_t i = 0; i < blackListSize; ++i)
	{
		PString ret;
		PUniString2PStringX(ret, myPatternUni[i]);
		PLog("%s", ret.c_str());
	}
	PLog("===white list===");
	for (size_t i = blackListSize; i < myPatternUni.size(); ++i)
	{
		PString ret;
		PUniString2PStringX(ret, myPatternUni[i]);
		PLog("%s", ret.c_str());
	}
	PLog("===end====");
}
#endif

struct PUinStringCmpIgnoreCase : public binary_function< PUniString, PUniString, bool >
{
public:
	bool operator() ( const PUniString& a, const PUniString& b ) const
	{
		return PUniString::compareIgnoreCase(a.c_str(), b.c_str()) < 0;
	}
};

const vector<PUniString>& ChatFilterHelper::convertToPUniString(const vector<PString>& myPattern,  size_t& blackListSize_)
{
	if (isDirty)
	{
#if PTRACELVL >= 5
		PTRACE5("convertToPUniString: isDirty");
#endif
		_normlizeAbusiveWords(myPattern, myPatternUni, blackListSize);
		// sort white list
		sort(myPatternUni.begin() + blackListSize, myPatternUni.end(), PUinStringCmpIgnoreCase());
		isDirty = false;
//#if PTRACELVL >= 5
//		if (myPatternUni.size())
//			dump();
//#endif
	}
	blackListSize_ = blackListSize;
	return myPatternUni;
}

/*static*/ ChatCategory ChatFilterHelper::getChatCategoryFromSite( const PSiteMask& siteMask )
{
	if( siteMask == SiteBitmask_ALLMask ) return ChatBroadcastAll;
	if( siteMask & SiteBitmask_Com ) return ChatBroadcastCom;
	if( siteMask & SiteBitmask_Net ) return ChatBroadcastNet;
	if( siteMask & SiteBitmask_Ita ) return ChatBroadcastIta;
	if( siteMask & SiteBitmask_Fra ) return ChatBroadcastFra;
	if( siteMask & SiteBitmask_Est ) return ChatBroadcastEst;
	if( siteMask & SiteBitmask_Bel ) return ChatBroadcastBel;
	if( siteMask & SiteBitmask_Spa ) return ChatBroadcastSpa;
	if( siteMask & SiteBitmask_Den ) return ChatBroadcastDen;
	if( siteMask & SiteBitmask_EU )  return ChatBroadcastEU;
	if( siteMask & SiteBitmask_US_NJ ) return ChatBroadcastUS_NJ;
	if( siteMask & SiteBitmask_UK )  return ChatBroadcastUK;
	if( siteMask & SiteBitmask_Bul ) return ChatBroadcastBul;
	if( siteMask & SiteBitmask_DE_SH ) return ChatBroadcastDE_SH;
	if( siteMask & SiteBitmask_Por ) return ChatBroadcastPor;
	if( siteMask & SiteBitmask_Rom ) return ChatBroadcastRom;
	if( siteMask & SiteBitmask_Cze ) return ChatBroadcastCz;
	if( siteMask & SiteBitmask_RUSO ) return ChatBroadcastRuSo;
	if( siteMask & SiteBitmask_RUSOPM ) return ChatBroadcastRuSoPM;
	if( siteMask & SiteBitmask_Ind ) return ChatBroadcastInd;
	if( siteMask & SiteBitmask_US_PA ) return ChatBroadcastUS_PA;
	if( siteMask & SiteBitmask_Asia ) return ChatBroadcastAsia;
	if( siteMask & SiteBitmask_Swe ) return ChatBroadcastSwe;
	if( siteMask & SiteBitmask_CH ) return ChatBroadcastCH;
	if( siteMask & SiteBitmask_ARBA ) return ChatBroadcastARBA;
	if( siteMask & SiteBitmask_GR ) return ChatBroadcastGR; // PYR-120936
	return ChatError;
}

inline static bool isExcemptedWord(const PUniString& word, const vector< PUniString >& myPatternUni, size_t blackListSize)
{
	return (word.startsWith((PUNICHAR*)L"*") || binary_search(myPatternUni.begin() + blackListSize, myPatternUni.end(), word, PUinStringCmpIgnoreCase()));
}

static void getExpandedUnNormilizedWord(const PUNICHAR* pBegin, const PUNICHAR* pEnd, const PUNICHAR* p, PUniString& unNormalizedWord)
{
	// expands word to the left
	while (pBegin > p && *(pBegin - 1) != ' ') --pBegin;
	// expands word to the right
	while (*pEnd && *pEnd != ' ') ++pEnd;
	unNormalizedWord.assign(pBegin, pEnd);
}

/*---------------------------------------------------------------------------------------------
				filterChat (NEW by VG)
---------------------------------------------------------------------------------------------*/
bool _filterChat( PString& ret, const vector< PString >& myPattern, ChatFilterHelper* pChatFilterHelper )
{
	bool filtered = false;

	if( ret.length() == 0 || pChatFilterHelper == NULL || myPattern.size() == 0)
		return filtered;

	size_t blackListSize = 0;
	const vector<PUniString>& myPatternUni = pChatFilterHelper->convertToPUniString(myPattern, blackListSize);

	PStringX::StringParser parser;
	parser.init( ret );

	PUniString buf;
	buf.append( ' ' );
	while( PUNICHAR_RET c = parser.nextChar() )
	{
		if( 0 < c )
			buf.append( c );
		else
			break; // PYR-23604
	}
	buf.append( ' ' );

	ret.cut( 0 );

	if( buf.length() <= 2 )
		return filtered;

	PBlock r( ( buf.length() + 1 ) * sizeof(PUNICHAR) );  // PYR-41470, also include the ending zero
	memcpy( r.ptr(), buf, r.size() );

	normalizeUniString( buf );

	PUNICHAR* p = (PUNICHAR*)r.ptr();
	for( size_t i = 0; i < blackListSize; ++i )
	{
		if( myPatternUni[ i ].length() )
		{
			const PUNICHAR* pBuf = buf;
			while( const PUNICHAR* pp = (PUNICHAR*)wcsstr( (wchar_t*)pBuf, (wchar_t*)myPatternUni[ i ].c_str()) )
			{
				PUNICHAR* _p = p + ( pp - buf );
				PUNICHAR* _pEnd = _p + myPatternUni[ i ].length();
				PUniString unNormalizedWord, unNormalizedWord2(_p, _pEnd);
				getExpandedUnNormilizedWord(_p, _pEnd, p, unNormalizedWord);
				if (!isExcemptedWord(unNormalizedWord, myPatternUni, blackListSize) && !isExcemptedWord(unNormalizedWord2, myPatternUni, blackListSize))
				{
					for( ; _p < _pEnd; ++_p )
					{
						if( *_p != ' ' )
							*_p = '*';
					}
					filtered = true;
				}
				pBuf = pp + myPatternUni[ i ].length();
			} // while
		}
	} // for

	static const PStringEncodingPStringX encStr;
	encStr.append( ret, p + 1, p + buf.length() - 1 );

	return filtered;
}

// PYR-32480
static void clearStringX( PString& ret, const char* str, const vector< pair<PUNICHAR, PUNICHAR> >& validRanges )
{
	ret.cut( 0 );

	PStringX::StringParser parser;
	parser.init( str );

	UINT32 numRanges = validRanges.size();
	PUNICHAR_RET c;

	while( ( c = parser.nextChar() ) > 0 )
	{
		bool valid = false;

		for(UINT32 i = 0; i < numRanges; ++i)
		{
			if(validRanges[ i ].first <= c && c <= validRanges[ i ].second)
			{
				valid = true;
				break;  // for numRanges
			}
		}

		if( valid )
		{
			static const PStringEncodingPStringX encStr;
			encStr.append( ret, c );
		}
	}
}

// PYR-32480
void removeInvalidSymbolsFromChatString( PString& ret, const char* chatInput, bool noExtendedAsciiCode )
{
	if(!chatInput || !*chatInput)
	{
		return;
	}

	vector< pair<PUNICHAR, PUNICHAR> > validRanges;
	if(noExtendedAsciiCode)
	{
		validRanges.push_back( make_pair( 0x20, 0x7F ) );
	}
	else
	{
		validRanges.push_back( make_pair( 0x20, 0xFF ) );
	}

	// for other languages
	validRanges.push_back( make_pair( 0x4E00, 0x9FFF ) );  // CJK common

	clearStringX( ret, chatInput, validRanges );
}