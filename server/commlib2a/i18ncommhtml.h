#ifndef i18ncommhtml_h_included
#define i18ncommhtml_h_included

// i18n wrapper for CommHtmlTemplate when dealing with e-mail files
// questions: igors@pyrsoftware.com

#include "ppmsg.h"
#include "commhtml.h"
#include "commmsgbody.h"

class i18nCommHtmlProxy;
class i18nCommHtmlTemplate;

// initialize once in the server object
class i18nCommHtmlManager
{
friend class i18nCommHtmlTemplate;
private:
	vector<i18nCommHtmlProxy*> bodies;
	const PMsgLocaleTable* table;

private:
	i18nCommHtmlProxy* file2body(const char* path, const PStringEncoding* encoding, const PStreamEncoding* fileEncoding);
	i18nCommHtmlProxy* str2body(const char* str, const PStringEncoding* encoding, const PStreamEncoding* fileEncoding);
	static void file2str(const char* path, PString& str);
	void freeBodies();

public:
	static void prepareMsgBody(const PMsgLocaleTable* table_, const char* filePath, const char* fileName, CommMsgBody& fileVector);
	void load(const char* filePath, const char* fileName);
	void load(const CommMsgBody& fileVector);
	i18nCommHtmlManager(): table(0)
	{
	}
	void init(const PMsgLocaleTable* table_)
	{
		table = table_;
	}
	i18nCommHtmlManager( const PMsgLocaleTable* table_)
	{
		init(table_);
	}
	~i18nCommHtmlManager()
	{
		freeBodies();
	}
};

// derive your email class from this one instead of CommHtmlTemplate
class i18nCommHtmlTemplate : public CommHtmlTemplateEx
{
private:
	i18nCommHtmlManager* manager;
	UINT32 locale;

public:
	i18nCommHtmlTemplate() :
		manager(0), locale(0)
	{
	}
	void init(i18nCommHtmlManager* manager_, UINT32 locale_)
	{
		PASSERT3(manager_);
		CommHtmlTemplateEx::init(manager_->table->encoding);
		manager = manager_;
		locale = locale_;
	}
	i18nCommHtmlTemplate(i18nCommHtmlManager* manager_, UINT32 locale_)
	{
		init(manager_, locale_);
	}
	// Returns true if name was found.
	bool magic(PString& buf, const char* name);
	virtual void spell(PString& buf, const char* name) {}
	UINT32 getLocale() const { return locale; }
};

#endif
