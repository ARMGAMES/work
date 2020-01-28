#ifndef i18n_strmap_h_included
#define i18n_strmap_h_included

#include "i18n.locales.h"

#ifdef PROTOCOL_CLIENT_ONLY

class CommMsgParser;
class CommMsgBody;

#define EXPLICIT explicit

#else

#include "commmsgbody.h"
#include "dbm.h"
#include "i18n.utils.h"

#define EXPLICIT

#endif

class I18nPString
{
public:
	I18nPString();
	EXPLICIT I18nPString( PString&& msg, UINT32 locale = LOCALE_DEFAULT );
	EXPLICIT I18nPString( const char* msg, UINT32 locale = LOCALE_DEFAULT ) : I18nPString( PString( msg ), locale ) {}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	I18nPString( I18nPString&& other ) : pStringVector(std::move(other.pStringVector)) {}
	I18nPString& operator=( I18nPString&& other ) { return moveFrom(other); }
	I18nPString( const I18nPString& other ) : pStringVector(other.pStringVector) {}
	I18nPString& operator=( const I18nPString& other ) { pStringVector = other.pStringVector; return *this; }
#endif

	bool isThere(UINT32 locale = LOCALE_DEFAULT) const;
	UINT32 size() const;
	PSTR_SIZE_T maxLength() const;

	PString& getPString(UINT32 locale = LOCALE_DEFAULT);
	PString& getPStringByTable(const PMsgLocaleTable& table, UINT32 locale = LOCALE_DEFAULT);
	const PString& getPString(UINT32 locale = LOCALE_DEFAULT) const;
	const PString& getPStringByTable(const PMsgLocaleTable& table, UINT32 locale = LOCALE_DEFAULT) const;
	bool getPairByInd(UINT32 ind, UINT32& iLocale, PString& pString) const;

	void putPString(const char* msg, UINT32 locale = LOCALE_DEFAULT);
	void appendPString(const char* msg, UINT32 locale = LOCALE_DEFAULT);
//	void putProperty(const PMsgLocaleTable& table, const PIniFile::Section* section, const char* key);

	void compose(CommMsgBody& query) const;
	void parse(CommMsgParser& parser);

	bool remove(UINT32 locale); // will not remove the LOCALE_DEFAULT entry but clean instead
	void clear();
	I18nPString& assign(const I18nPString& from);
	I18nPString& moveFrom(I18nPString& from);

	bool equals(const I18nPString& other) const;

#ifndef PROTOCOL_CLIENT_ONLY
	void psx_Utf8();
	void utf8_Psx();
#endif

	INT32 dbgCalculateTotalBytes() const;

protected:
	typedef pair<UINT32, PString> i18nLangPStringPair;

	vector<i18nLangPStringPair> pStringVector;
};

//-----------------------------------------------------------------------------

#undef EXPLICIT

//-----------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

// PYR-21301: create an I18nPString object given a msgId (the object will contain the translations for all locales)
void assignI18nPString(I18nPString& ret, const PMsgLocaleTable& table, const PMsgId& msgId);

template<size_t N>
class I18nPStringEx
{
	template<size_t N_>
	struct DataType
	{
		PString strings[N_];

		PString& at(size_t idx)
		{
			PASSERT5(idx < N_);
			return strings[idx];
		}

		const PString& at(size_t idx) const
		{
			PASSERT5(idx < N_);
			return strings[idx];
		}
	};

	bool validIndex(size_t idx) const
	{
		if (idx >= N)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ":I18nPStringEx index (%d) out of range (%d)", idx, N);
			PASSERT5(false);
			return false;
		}
		return true;
	}

public:

	I18nPStringEx<N>()
	{
		DataType<N> emptyStrings;
		pStringVector.push_back(i18nLangPStringPair(LOCALE_DEFAULT, emptyStrings));
	}

	bool isThere(UINT32 locale = LOCALE_DEFAULT) const
	{
		for(typename vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin(); iter != pStringVector.end(); ++iter)
		{
			if((*iter).first == locale)
				return true;
		}
		return false;
	}
	UINT32 size() const
	{
		return pStringVector.size();
	}
	PSTR_SIZE_T maxLength(size_t idx) const
	{
		if (!validIndex(idx))
		{
			return 0;
		}
		PSTR_SIZE_T length = 0;
		for(typename vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin(); iter != pStringVector.end(); ++iter)
		{
			if(length < (*iter).second.at(idx).length())
				length = (*iter).second.at(idx).length();
		}
		return length;
	}
	void getPString(size_t idx, PString& ret, UINT32 locale = LOCALE_DEFAULT) const
	{
		getPStringByTable(idx, LocalesTable, ret, locale);
	}
	void getPStringByTable(size_t idx, const PMsgLocaleTable& table, PString& ret, UINT32 locale = LOCALE_DEFAULT) const
	{
		if (!validIndex(idx))
		{
			ret = "";
			return;
		}
		for(typename vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin();	iter != pStringVector.end(); ++iter)
		{
			if((*iter).first == locale)
			{
				ret = (*iter).second.at(idx);
				return;
			}
		}

		PASSERT5(locale != LOCALE_DEFAULT);	//there is no LOCALE_DEFAULT message! - shouldn't happen

		if(locale > MAX_LOCALES)
			locale = LOCALE_DEFAULT;
		getPStringByTable(idx, table, ret, table.locales[locale].parentId);
	}

	bool getPairByInd(size_t idx, UINT32 ind, UINT32& iLocale, PString& pString) const
	{
		if(ind >= pStringVector.size() || idx >= N)
		{
			iLocale = LOCALE_DEFAULT;
			pString = "";
			return false;
		}
		iLocale = pStringVector[ind].first;
		pString = pStringVector[ind].second.at(idx);
		return true;
	}

	void putPString(size_t idx, const char* msg, UINT32 locale = LOCALE_DEFAULT)
	{
		if (!validIndex(idx))
		{
			return;
		}
		for(typename vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();	iter != pStringVector.end(); ++iter)
		{
			if((*iter).first == locale)
			{
				(*iter).second.at(idx) = msg;
				return;
			}
		}
		DataType<N> emptyStrings;
		emptyStrings.at(idx) = msg;
		pStringVector.push_back(i18nLangPStringPair(locale, emptyStrings));
	}
	void appendPString(size_t idx, const char* msg, UINT32 locale = LOCALE_DEFAULT)
	{
		if (!validIndex(idx))
		{
			return;
		}
		for(typename vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();	iter != pStringVector.end(); ++iter)
		{
			if((*iter).first == locale)
			{
				(*iter).second.at(idx).append(msg);
				return;
			}
		}
		DataType<N> emptyStrings;
		emptyStrings.at(idx) = msg;
		pStringVector.push_back(i18nLangPStringPair(locale, emptyStrings));
	}

	void compose(CommMsgBody& query) const
	{
		query.composeUINT32(pStringVector.size());
		for(typename vector<i18nLangPStringPair>::const_iterator iter = pStringVector.begin(); iter != pStringVector.end(); ++iter)
		{
			query.composeUINT32((*iter).first);
			for (size_t i = 0; i < N; ++i)
			{
				query.composeString((*iter).second.at(i));
			}
		}
	}
	void parse(CommMsgParser& parser)
	{
		clear();
		UINT32 size;
		parser.parseUINT32(size);
		for(int i_count = 0; i_count < size; ++i_count)
		{
			UINT32 locale; 
			const char* str;
			parser.parseUINT32(locale);
			for (size_t i = 0; i < N; ++i)
			{
				parser.parseString(str);
				putPString(i, str, locale);
			}
		}
	}

	void clear()
	{
		pStringVector.clear();
		DataType<N> emptyStrings;
		pStringVector.push_back(i18nLangPStringPair(LOCALE_DEFAULT, emptyStrings));
	}
	I18nPStringEx<N>& assign(const I18nPStringEx<N>& from)
	{
		pStringVector = from.pStringVector;
		return *this;
	}
	I18nPStringEx<N>& moveFrom(I18nPStringEx<N>& from)
	{
		pStringVector.swap(from.pStringVector);
		return *this;
	}
	bool equals(const I18nPStringEx<N>& other) const
	{
		if (pStringVector.size() != other.pStringVector.size())
			return false;

		for(int ind = 0; ind < pStringVector.size(); ++ind)
		{
			if(!pStringVector[ind].second.equals(other.getPString(pStringVector[ind].first).c_str()))
				return false;
		}
		return true;
	}

	void psx_Utf8()
	{
		for(typename vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();	iter != pStringVector.end(); ++iter)
		{
			for (size_t i = 0; i < N; ++i)
			{
				PString utf8String;
				i18nPsx_Utf8(utf8String, (*iter).second.at(i));
				(*iter).second.at(i).assign(utf8String);
			}
		}
	}
	void utf8_Psx()
	{
		for(typename vector<i18nLangPStringPair>::iterator iter = pStringVector.begin();	iter != pStringVector.end(); ++iter)
		{
			for (size_t i = 0; i < N; ++i)
			{
				PString psxString;
				i18nUtf8_Psx(psxString, (*iter).second.at(i));
				(*iter).second.at(i).assign(psxString);
			}
		}
	}

private:
	typedef pair<UINT32, DataType<N> > i18nLangPStringPair;
	vector<i18nLangPStringPair> pStringVector;
};

//-----------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

#endif // i18n_strmap_h_included
