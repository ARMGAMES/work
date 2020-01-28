#include "i18n.locales.h"
#include "ppi18n_x.h"
#include "pplogfile.h"
#include "ppcheck.h"
#include "sites.h"

//
// WHEN ADDING NEW LANGUAGE ALSO CHECK i18nFormatLocaleName()!
// Language codes: ISO 639.1
// Country codes: ISO 3166.1
//

static const PStreamEncodingWin1252		file1252;
static const PStreamEncodingUtf8		fileUTF8;

//enum from i18n.localedef.h
CLIENT_LOCALES_ORDER

//array of structs from i18n.localedef.h
SUPPORTED_LOCALES_TABLE

static const PStringEncodingPStringX encoding;

PMsgLocaleTable LocalesTable = 
{
	supportedLocales,
	sizeof(supportedLocales) / sizeof(supportedLocales[0]) - 1,
	&encoding
};

class BufferedStream 
{
private:
	char* buf;
	const char* cur;
	const char* end;
	size_t size;
	PInputStream* stream;
	// prohibited
	BufferedStream(const BufferedStream&);
	BufferedStream& operator=(const BufferedStream&);

public:
	BufferedStream(PInputStream* stream_, size_t size_ = 4096);
	~BufferedStream();
	size_t readLine(PString& out);
};

BufferedStream::BufferedStream(PInputStream* stream_, size_t size_)
{
	stream = stream_;
	buf = new char[size = size_];
	cur = end = buf;
}

BufferedStream::~BufferedStream()
{
	delete[] buf;
	cur = end = 0;
}

size_t BufferedStream::readLine(PString& out)
{
	out.assign("");

	for( ;; )
	{
		if(end == buf)	//nothing in the buffer - read new portion
		{
			end += stream->read((unsigned char*)buf, size);

			if(end <= buf)	//EOF or error from read(2)
				return out.length();
		}

		const char* beg = cur;

		for(; cur < end && *cur != '\n' && *cur != '\r'; ++cur)
			;

		out.append(beg, cur);

		if(cur < end)
		{
			++cur;
			if(out.length() > 0)
				return out.length();
		}
		else
		{
			cur = end = buf;
		}
	}
	// never reached
}

static int getLocaleIdx(const PMsgLocaleTable& table, const char* name)
{
	for (unsigned int i = 0; i < table.max; ++i)
		if (strcmp(name, table.locales[i].fileName) == 0)
			return i;
	return -1;
}

static int parseStream(const PMsgLocaleTable& table, const PString& lineBuf, PMsgTableImage& tableImage, PString& err)
{
	const char* t = lineBuf.c_str();
	while (*t == ' ' || *t == '\t')
		++t;
	if (*t == '#' || !*t) // comments and empty lines allowed
		return 0;
	const char* loc = t;
	while (*t && *t != ' ' && *t != '\t')
		++t;
	if (!*t)
	{
		err = "unexpected EOL";
		return -1;
	}
	const char* loc_e = t++;
	while (*t == ' ' || *t == '\t')
		++t;
	if (!*t)
	{
		err = "unexpected EOL";
		return -1;
	}
	const char* tag = t;
	while (*t && *t != ' ' && *t != '\t')
		++t;
	if (!*t)
	{
		err = "unexpected EOL";
		return -1;
	}
	const char* tag_e = t++;
	while (*t == ' ' || *t == '\t')
		++t;
	if (!*t)
	{
		err = "unexpected EOL";
		return -1;
	}
	const char* flags = t;
	while (*t && *t != ' ' && *t != '\t')
		++t;
	if (!*t)
	{
		err = "unexpected EOL";
		return -1;
	}
	t++;
	// leading white spaces preserved
	if (!*t)
	{
		err = "unexpected EOL";
		return -1;
	}
	const char* msg = t;
	tableImage.localeId = getLocaleIdx(table, PString(loc, loc_e));
	if (tableImage.localeId < 0) 
	{
		// skip unknown locale - quite possible on updates
		return 0;
	}
	tableImage.flags = atoi(flags);
	tableImage.tag.assign(tag, tag_e);
	tableImage.msg.assign(msg);

	return 1;
}

bool i18nDynamicLocaleFilter(const PMsgLocaleTable& table, UINT32 localeId, const PMsgData* msgData, int dataSize, const PMsgTableImage& tableImage, int msgId)
{
	if( tableImage.flags & TFLAG_MULTI )
		return true; 

	for ( ; ; )
	{
		if( msgData[ localeId * dataSize + msgId ].msg )
			return false;

		if( static_cast< int >( localeId ) < tableImage.localeId )
			return false;

		if( localeId == tableImage.localeId )
			return true;

		localeId = table.locales[ localeId ].parentId;
	}

	// never reached
}

static void translateMessage(char* p)
{
	//
	// Bug #5694
	// Only \r and \n will be translated *AS IS* in the .txt file
	// all other escapes should be already translated or taken verbatim
	// 
	char* s = p;
	while (*p) 
	{
		if (*p == '\\' && *(p + 1) == 'n') 
		{
			*s++ = '\n';
			++p;
			++p;
		}
		else if (*p == '\\' && *(p + 1) == 'r') 
		{
			*s++ = '\r';
			++p;
			++p;
		}
		else
		{
			*s++ = *p++;
		}
	}
	*s = *p;
}

static int loadMessage(const PMsgLocaleTable& table, PMsgData* msgData, const PMsgStrTable& strTable, const PMsgTableImage& tableImage, PMsgLocaleFilter localeFilter, UINT32 locale)
{
	const PMsgStrCData* strData = strTable.strData;
	unsigned int dataSize = strTable.dataSize;
	PMsgStrCData comp = {0, 0, tableImage.tag};
	_DEBUG_CHECK_TABLE_ORDER(strData, strData + dataSize);
	const PMsgStrCData* it = lower_bound(strData, strData + dataSize, comp);
	if (it == strData + dataSize || strcmp((*it).msgStr, tableImage.tag) != 0)
	{
		// skip unknown tag - quite possible on the update
		return 0;
	}

	const int msgId = it->msgId;

	if (localeFilter && !localeFilter(table, locale, msgData, dataSize, tableImage, msgId))
	{
		// single-language loading
		return 0;
	}

	const int offset = tableImage.localeId * dataSize + msgId;
	msgData[offset].msg = new char[tableImage.msg.length() + 1];
	// literal copy - the buffer contains well-formed PStringX
	strcpy(const_cast<char*>(msgData[offset].msg), tableImage.msg.c_str());
	msgData[offset].flags = tableImage.flags;
	// ensure correct flags in the preferred locale in single-language mode
	if (localeFilter && locale != tableImage.localeId && !(tableImage.flags & TFLAG_MULTI)) 
	{
		const int localeOffset = locale * dataSize + msgId;
		msgData[localeOffset].flags = tableImage.flags;
	}
	// should multi-language be covered, too???

	translateMessage(const_cast<char*>(msgData[offset].msg));

	return 1;
}

static int loadDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, const PMsgStrTable& strTable, PInputStream* txtStream, PString& err, PMsgLocaleFilter localeFilter, UINT32 locale)
{
	PASSERT5(locale < table.max); // debug only
	if (table.max <= locale) //-V547
		locale = 0;

	int ret = 0;
	int lineCount = 0;
	BufferedStream lineStream(txtStream);
	PString lineBuf;

	PMsgData* msgData;
	msgData = new PMsgData[strTable.dataSize * table.max];

	while (lineStream.readLine(lineBuf))
	{
		PMsgTableImage tableImage;
		++lineCount;
		ret = parseStream(table, lineBuf, tableImage, err);
		if (ret < 0) 
		{
			err.append(" on line ").appendInt(lineCount);
			break;
		}
		if (ret == 0)
		{
			continue;
		}
		ret = loadMessage(table, msgData, strTable, tableImage, localeFilter, locale);
		if (ret < 0)  //-V547 // note that loadMessage used to return negative in the past, but not now
		{
			err.append(" on line ").appendInt(lineCount);
			break;
		}
	}

	if (ret < 0)
	{
		PLog("i18nLoadDynamicTable aborted: %s", err.c_str());
		delete [] msgData;
		return ret;
	}

	i18nFreeDynamicTable(table, dataTable, strTable);

	dataTable.msgData = msgData;
	dataTable.allocated = true;

	return lineCount;
}

int i18nLoadDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, const PMsgStrTable& strTable, PInputStream* txtStream, PString& err)
{
	return loadDynamicTable(table, dataTable, strTable, txtStream, err, 0, 0);
}

int i18nLoadFilteredDynamicTable(const PMsgLocaleTable& table, UINT32 locale, PMsgDataTable& dataTable, const PMsgStrTable& strTable, PInputStream* txtStream, PString& err, PMsgLocaleFilter localeFilter)
{
	return loadDynamicTable(table, dataTable, strTable, txtStream, err, localeFilter, locale);
}

void i18nFreeDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, const PMsgStrTable& strTable)
{
	i18nFreeDynamicTable( table, dataTable, strTable.dataSize );
}

void i18nFreeDynamicTable(const PMsgLocaleTable& table, PMsgDataTable& dataTable, unsigned int dataSize)
{
	if (dataTable.allocated)
	{
		for (unsigned int i = 0; i < dataSize * table.max; ++i)
		{
			delete [] const_cast<char*>(dataTable.msgData[i].msg);
		}
		delete [] dataTable.msgData;
	}
	dataTable.msgData = NULL;
	dataTable.allocated = false;
}

void i18nGetProperty(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const PIniFile::Section* section, const char* key)
{
	PASSERT5(localeId < table.max);
	if (table.max <= localeId) //-V547
		localeId = LOCALE_DEFAULT;
	const char* value = 0;
	for ( ; ; )
	{
		PString lkey = key;
		if (localeId != LOCALE_DEFAULT) // keep existing keys as is
			lkey.append("_").append(table.locales[localeId].fileName);
		value = section->getProperty(lkey);
		if (value && value[0])
			break;
		if (localeId == LOCALE_DEFAULT)
			break;
		localeId = table.locales[localeId].parentId;
	}
	ret = value;
}

unsigned int i18nGetLocaleId(const PMsgLocaleTable& table, unsigned int localeId, const vector<unsigned int>& locales)
{
	PASSERT5(localeId < table.max);

	if (table.max <= localeId) //-V547
		localeId = LOCALE_DEFAULT;

	for ( ; ; )
	{
		vector<unsigned int>::const_iterator it = find(locales.begin(), locales.end(), localeId);
		if (it != locales.end() && *it == localeId)
			break;
		if (localeId == LOCALE_DEFAULT)
			break;
		localeId = table.locales[localeId].parentId;
	}

	return localeId;
}

bool i18nFindId(PMsgId& id, const PMsgLocaleTable& localeTable, const PMsgDataTable& dataTable, const PString& str)
{
	int size = localeTable.max * dataTable.rowSize;
	for (int i = 0; i < size; ++i)
	{
		if (strcmp(str, dataTable.msgData[i].msg) == 0)
		{
			id.msgTable = &dataTable;
			id.msgId = i % dataTable.rowSize;
			return true;
		}
	}
	id.msgTable = 0;
	id.msgId = 0;
	return false;
}

UINT32 i18nGetLocale(const char * language)
{
	for ( int i = 0; i < sizeof(supportedLocales)/sizeof(supportedLocales[0])-1; i++ )
	{
		if (!stricmp(supportedLocales[i].fileName,language))
			return i;
	}


	return -1;
}

void i18nGetLanguage(UINT32 locale, PString & res)
{
	if (locale < sizeof(supportedLocales)/sizeof(supportedLocales[0])-1)
		res.append(supportedLocales[locale].fileName);
}
