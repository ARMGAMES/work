#include "i18ncommhtml.h"

class i18nCommHtmlProxy: public CommHtmlTemplateEx
{
public:
	CommHtmlTemplateEx* instance;
	void spell(PString& buf, const char* name)
	{
		instance->spell(buf, name);
	}
	i18nCommHtmlProxy(const PStringEncoding* enc): CommHtmlTemplateEx(enc), instance(0)
	{
	}
};

void i18nCommHtmlManager::file2str(const char* path, PString& str)
{
	FILE* file = fopen(path, "r");
	if (!file)
		return;

	char buf[2048];
	while (fgets(buf, sizeof(buf), file))
	{
		str.append(buf);
	}
	fclose(file);
}

i18nCommHtmlProxy* i18nCommHtmlManager::file2body(const char* path, const PStringEncoding* encoding, const PStreamEncoding* fileEncoding)
{
	FILE* file = fopen(path, "r");
	if (!file)
		return 0;
	PFileInputStream fstream(file);
	i18nCommHtmlProxy* body = new i18nCommHtmlProxy(encoding);
	body->load(fstream, *fileEncoding);
	fclose(file);

	return body;
}

i18nCommHtmlProxy* i18nCommHtmlManager::str2body(const char* str, const PStringEncoding* encoding, const PStreamEncoding* fileEncoding)
{
	if (!str[0])
		return 0;
	int len = strlen(str);
	PBlock block;
	BYTE* beg = block.alloc(len);
	memcpy(beg, str, len);
	PMemInputStream mstream(block);
	i18nCommHtmlProxy* body = new i18nCommHtmlProxy(encoding);
	body->load(mstream, *fileEncoding);

	return body;
}

void i18nCommHtmlManager::freeBodies()
{
	for (int i = 0; i < bodies.size(); ++i)
		delete bodies[i];
	bodies.clear();
}

void i18nCommHtmlManager::load(const char* filePath, const char* fileName)
{
	const char* dot = strchr(fileName, '.');
	if (!dot) 
		dot = &fileName[strlen(fileName)];

	int cnt = 0;
	freeBodies();

	bodies.resize(table->max);
	for (int i = 0; i < table->max; ++i)
	{
		PString path(filePath);
		try 
		{
			path
				.append(fileName, dot)
				.append('_')
				.append(table->locales[i].fileName)
				.append(dot);
			bodies[i] = file2body(path, table->encoding, table->locales[i].encoding);
			// fallback for default locale if the file is not renamed
			if (i == 0 && bodies[i] == 0)
			{
				path.assign(filePath);
				path.append(fileName);
				bodies[i] = file2body(path, table->encoding, table->locales[0].encoding);
			}
			if (bodies[i])
				++cnt;
		}
		catch (PError& er)
		{
			// do NOT re-throw!
			bodies[i] = NULL;
			PLog("i18nCommHtmlManager: failed to process '%s' (localeId = %d): %s. File skipped.", path.c_str(), i, er.why());
		}
	}
	PLog("i18nCommHtmlManager: loaded %d out of %d templates", cnt, table->max);
}

void i18nCommHtmlManager::load(const CommMsgBody& msgBody)
{
	int cnt = 0;
	freeBodies();

	CommMsgParser parser(msgBody);
	bodies.resize(table->max);
	for (int i = 0; i < table->max; ++i)
	{
		try 
		{
			const char* content;
			parser.parseString(content);
			bodies[i] = str2body(content, table->encoding, table->locales[i].encoding);
			if (bodies[i])
				++cnt;
		}
		catch (PError& er)
		{
			// do NOT re-throw!
			bodies[i] = NULL;
			PLog("i18nCommHtmlManager: failed to process msgBody for localeId = %d: %s. File skipped.", i, er.why());
		}
	}
	PLog("i18nCommHtmlManager: loaded %d out of %d templates", cnt, table->max);
}

void i18nCommHtmlManager::prepareMsgBody(const PMsgLocaleTable* table, const char* filePath, const char* fileName, CommMsgBody& body)
{
	const char* dot = strchr(fileName, '.');
	if (!dot) 
		dot = &fileName[strlen(fileName)];

	for (int i = 0; i < table->max; ++i)
	{
		PString curr;
		PString path(filePath);
		path
			.append(fileName, dot)
			.append('_')
			.append(table->locales[i].fileName)
			.append(dot);
		file2str(path, curr);
		// fallback for default locale if the file is not renamed
		if (i == 0 && curr.length() == 0)
		{
			path.assign(filePath);
			path.append(fileName);
			file2str(path, curr);
		}
		body.composeString(curr);
	}
}

// Returns true if name was found.
bool i18nCommHtmlTemplate::magic(PString& buf, const char* name)
{
	PASSERT3(manager);
	PASSERT5(locale < manager->table->max); // debug only
	int cloc = locale;
	for ( ; ; )
	{
		if (manager->table->max <= cloc)
			cloc = 0;
		i18nCommHtmlProxy* body = manager->bodies[cloc];
		if (body) // language file found
		{
			body->instance = this; // the currently constructed derived class
			if (body->magic(buf, name))
				return true;
		}
		if (cloc == 0) // root
			break;
		cloc = manager->table->locales[cloc].parentId;
	}
	return false;
}
