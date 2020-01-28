#include "i18n.utils.h"
#include "i18n.strmap.h"

#ifdef PROTOCOL_CLIENT_ONLY
#include "commmsgbody.h"
#endif

I18nPString::I18nPString()
{
	pStringVector.emplace_back( LOCALE_DEFAULT, "" );
}

I18nPString::I18nPString( PString&& msg, UINT32 locale )
{
	if( locale != LOCALE_DEFAULT )
		pStringVector.emplace_back( LOCALE_DEFAULT, "" );

	pStringVector.emplace_back( locale, move( msg ) );
}

bool I18nPString::isThere(UINT32 locale) const
{
	for(vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if((*iter).first == locale)
			return true;
	}

	return false;
}

UINT32 I18nPString::size() const
{
	return static_cast< UINT32 >( pStringVector.size() );
}

PSTR_SIZE_T I18nPString::maxLength() const
{
	PSTR_SIZE_T length = 0;

	for(vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if(length < (*iter).second.length())
			length = (*iter).second.length();
	}

	return length;
}

PString& I18nPString::getPString(UINT32 locale)
{
	return getPStringByTable(LocalesTable, locale);
}

PString& I18nPString::getPStringByTable(const PMsgLocaleTable& table, UINT32 locale)
{
	for(vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if((*iter).first == locale)
			return (*iter).second;
	}

	PASSERT5(locale != LOCALE_DEFAULT);	//there is no LOCALE_DEFAULT message! - shouldn't happen

	if(locale > MAX_LOCALES)
		locale = LOCALE_DEFAULT;

	return getPStringByTable(table, table.locales[locale].parentId);
}

const PString& I18nPString::getPString(UINT32 locale) const
{
	return getPStringByTable(LocalesTable, locale);
}

const PString& I18nPString::getPStringByTable(const PMsgLocaleTable& table, UINT32 locale) const
{
	for(vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if((*iter).first == locale)
			return (*iter).second;
	}

	PASSERT5(locale != LOCALE_DEFAULT);	//there is no LOCALE_DEFAULT message! - shouldn't happen

	if(locale > MAX_LOCALES)
		locale = LOCALE_DEFAULT;

	return getPStringByTable(table, table.locales[locale].parentId);
}

bool I18nPString::getPairByInd(UINT32 ind, UINT32& iLocale, PString& pString) const
{
	if(ind >= pStringVector.size())
	{
		iLocale = LOCALE_DEFAULT;
		pString = "";
		return false;
	}
	iLocale = pStringVector[ind].first;
	pString = pStringVector[ind].second;
	return true;
}

void I18nPString::putPString(const char* msg, UINT32 locale)
{
	for(vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if((*iter).first == locale)
		{
			(*iter).second = msg;
			return;
		}
	}

	pStringVector.emplace_back( locale, msg );
}

void I18nPString::appendPString(const char* msg, UINT32 locale)
{
	for(vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if((*iter).first == locale)
		{
			(*iter).second.append(msg);
			return;
		}
	}

	pStringVector.emplace_back( locale, msg );
}

void I18nPString::compose(CommMsgBody& query) const
{
	query.
		composeUINT32( static_cast< UINT32 >( pStringVector.size() ) );

	for(vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		query.
			composeUINT32((*iter).first).
			composeString((*iter).second);
	}
}

void I18nPString::parse(CommMsgParser& parser)
{
	clear(); // !!!

	UINT32 m_size;

	parser.
		parseUINT32(m_size);

	for(UINT32 i_count = 0; i_count < m_size; ++i_count)
	{
		UINT32 locale; 
		const char* m_str;

		parser.
			parseUINT32(locale).
			parseString(m_str);

		putPString(m_str, locale);
	}
}

bool I18nPString::remove(UINT32 locale)
{
	for(vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		if((*iter).first == locale)
		{
			if(LOCALE_DEFAULT == locale)
			{
				(*iter).second = "";
			}
			else
			{
				pStringVector.erase(iter);
			}
			return true;
		}
	}

	return false;
}
void I18nPString::clear()
{
	pStringVector.clear();
	pStringVector.emplace_back( LOCALE_DEFAULT, "" );
}

I18nPString& I18nPString::assign(const I18nPString& from)
{
//	clear();

	pStringVector = from.pStringVector;

	return *this;
}

I18nPString& I18nPString::moveFrom(I18nPString& from)
{
//	clear();

	pStringVector.swap(from.pStringVector);

	return *this;
}

bool I18nPString::equals(const I18nPString& other) const
{
	if(pStringVector.size() != other.pStringVector.size())
		return false;

	for( auto&& p : pStringVector )
	{
		if( !p.second.equals( other.getPString( p.first ) ) )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

void I18nPString::psx_Utf8()
{
	for(vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		PString utf8String;
		i18nPsx_Utf8(utf8String, (*iter).second);
		(*iter).second.moveFrom(utf8String);
	}
}

void I18nPString::utf8_Psx()
{
	for(vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();
		iter != pStringVector.end();
		++iter)
	{
		PString psxString;
		i18nUtf8_Psx(psxString, (*iter).second);
		(*iter).second.moveFrom(psxString);
	}
}

void assignI18nPString(I18nPString& ret, const PMsgLocaleTable& table, const PMsgId& msgId) // PYR-21301
{
	ret.clear();
	for(UINT32 locale = 0; locale < table.max; ++locale)
	{
		PString str;
		PMsg(str, table, locale, msgId);
		if (str.length())
			ret.putPString(str, locale);
	}
}

//-----------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

INT32 I18nPString::dbgCalculateTotalBytes() const
{
	INT32 retVal = 0;
	for( auto&& p : pStringVector )
	{
		retVal += static_cast< INT32 >( sizeof(p) + p.second.length() );
	}
	return retVal;
}
