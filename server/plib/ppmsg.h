#ifndef ppmsg_h_included
#define ppmsg_h_included

#include "ppincludebase.h"
#include "ppstring.h"
#include "ppi18n.h"

// 
// see i18n.locales.{cpp,h} for flag definitions and instantiations
//
struct PMsgLocale
{
	int						parentId;
	const char*				displayName;
	const char*				fileName;
	const char*				emailCharset;
	const PStreamEncoding*	encoding;
	UINT32					displayOrder;
	UINT32					flags;
};

struct PMsgLocaleTable
{
	const PMsgLocale*		locales;
	unsigned int			max;
	const PStringEncoding*	encoding;
};

//------------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//------------------------------------------------------------------------------

struct PMsgCData
{
	const char* 	msg;
	UINT32          flags;
#if _MSC_VER < 1400
	~PMsgCData()
	{
		// don't even think of using free[] here!
	}
#endif
};

struct PMsgData : public PMsgCData
{
	PMsgData()
	{
		msg = 0;
		flags = 0;
	}
	PMsgData(const char* msg_)
	{
		msg = msg_;
		flags = 0;
	}
	PMsgData(const char* msg_, unsigned int flags_)
	{
		msg = msg_;
		flags = flags_;
	}
};

struct PMsgDataTable
{
	const PMsgCData*	msgData;
	unsigned int		rowSize;
	bool				allocated;
};

struct PMsgId
{
	const PMsgDataTable*	msgTable;
	unsigned int			msgId;
	PMsgId(): msgTable(0), msgId(0)
	{
	}
	PMsgId(const PMsgDataTable* msgTable_, unsigned int msgId_):
	msgTable(msgTable_), msgId(msgId_)
	{
	}
	bool isNull() const
	{
		return msgTable == 0 || msgId == 0;
	}
	void setNull()
	{
		msgTable = 0;
		msgId = 0;
	}
	bool isValid() const
	{
		if (isNull())
			return true;
		PASSERT5(msgId < msgTable->rowSize); // debug
		return msgId < msgTable->rowSize;
	}
	bool equals(const PMsgId& other) const
	{
		if (isNull() && other.isNull())
			return true;
		return (msgTable == other.msgTable) && (msgId == other.msgId);
	}
};

#define P_TXT( msgId )	( msgId )

struct PMsgStrCData
{
	const PMsgDataTable*	msgTable;
	unsigned int			msgId;
	const char*				msgStr;
	bool operator<(const PMsgStrCData& other) const
	{
		return(strcmp(msgStr, other.msgStr) < 0);
	}
};

struct PMsgStrData : public PMsgStrCData
{
	PMsgStrData(const char* msgStr_)
	{
		msgTable = 0;
		msgId = 0;
		msgStr = msgStr_;
	}
	PMsgStrData(const PMsgDataTable* msgTable_, unsigned int msgId_, const char* msgStr_)
	{
		msgTable = msgTable_;
		msgId = msgId_;
		msgStr = msgStr_;
	}
};

struct PMsgStrTable
{
	const PMsgStrCData*	strData;
	unsigned int		dataSize;
};

PMsgId PMsgIdFromMsgStr(const PMsgStrTable& strTable, const char* msgStr);

/* flags */ UINT32 PMsgStr(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* strId);
/* flags */ UINT32 PMsgStr1(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* strId, const char* p0);
/* flags */ UINT32 PMsgStr2(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* strId, const char* p0, const char* p1);
/* flags */ UINT32 PMsgStr3(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* strId, const char* p0, const char* p1, const char* p2);
/* flags */ UINT32 PMsgStr4(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* strId, const char* p0, const char* p1, const char* p2, const char* p3);
/* flags */ UINT32 PMsgStr5(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgStrTable& strTable, const char* strId, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4);
/* flags */ UINT32 PMsgFlags(unsigned int localeId, const PMsgId& msgId);

// deprecated functionality
void _PXMsg( PString& ret, const char* format, int nArgs, const char* args[] );
void PXMsg( PString& ret, const char* format );
void PXMsg1( PString& ret, const char* format, const char* p0 );
void PXMsg2( PString& ret, const char* format, const char* p0, const char* p1 );
void PXMsg3( PString& ret, const char* format, const char* p0, const char* p1, const char* p2 );
void PXMsg4( PString& ret, const char* format, const char* p0, const char* p1, const char* p2, const char* p3 );
void PXMsg5( PString& ret, const char* format, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4 );

//------------------------------------------------------------------------------
#else // PROTOCOL_CLIENT_ONLY
//------------------------------------------------------------------------------

struct _PMsgStr
{
	const char* const msgStr;

	explicit _PMsgStr( const char* msgStr_ )
		: msgStr( msgStr_ )
	{
	}

	bool isNull() const
	{
		return !msgStr || !*msgStr;
	}

private: // prohibit default assignment operator
	_PMsgStr& operator=( const _PMsgStr& other );
};

//------------------------------------------------------------------------------

class PMsgId
{
	friend class PMsgTable;

private:
	unsigned int msgId;

private:
	void _lookup( _PMsgStr&& msgStr );

public:
	PMsgId( nullptr_t = nullptr )
		: msgId( 0 )
	{
	}

	explicit PMsgId( const char* msgStr )
	{
		_lookup( _PMsgStr( msgStr ) );
	}

	PMsgId( _PMsgStr&& msgStr )
	{
		_lookup( std::move( msgStr ) );
	}

	PMsgId( const PMsgId& other )
		: msgId( other.msgId )
	{
	}

public:
	bool isNull() const
	{
		return !msgId;
	}

	explicit operator bool() const
	{
		return !isNull();
	}

public:
	bool equals( const PMsgId& other ) const
	{
		return msgId == other.msgId;
	}

	bool operator==( const PMsgId& other ) const
	{
		return equals( other );
	}

	bool operator!=( const PMsgId& other ) const
	{
		return !equals( other );
	}

public:
	UINT32 flags() const;

public:
	PMsgId& operator=( _PMsgStr&& msgStr )
	{
		_lookup( std::move( msgStr ) );
		return *this;
	}

	PMsgId& operator=( const PMsgId& other )
	{
		msgId = other.msgId;
		return *this;
	}

public:
	void setNull()
	{
		msgId = 0;
	}
};

//------------------------------------------------------------------------------

#define P_TXT( msgId )	_PMsgStr( #msgId )
#define P_NULL_TXT		_PMsgStr( nullptr )

//------------------------------------------------------------------------------

struct PMsgCliStrData
{
	UINT32 flags;
	const char* msgStr;

	bool operator<( const PMsgCliStrData& other ) const
	{
		return PString::compare( msgStr, other.msgStr ) < 0;
	}
};

//------------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//------------------------------------------------------------------------------

// i18n functionality
void _PMsg(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, int nArgs, const char* args[]);
void PMsg(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId);
void PMsg1(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0);
void PMsg2(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1);
void PMsg3(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1, const char* p2);
void PMsg4(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1, const char* p2, const char* p3);
void PMsg5(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const PMsgId& msgId, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4);

// debug-only functionality
void _PMsg(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, int nArgs, const char* args[]);
void PMsg(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg);
void PMsg1(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0);
void PMsg2(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1);
void PMsg3(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1, const char* p2);
void PMsg4(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1, const char* p2, const char* p3);
void PMsg5(PString& ret, const PMsgLocaleTable& localeTable, unsigned int localeId, const char* msg, const char* p0, const char* p1, const char* p2, const char* p3, const char* p4);

#endif
