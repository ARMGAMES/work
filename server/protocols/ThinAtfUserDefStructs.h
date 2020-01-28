#ifndef PYR_PROTOCOLS_THIN_ATF_USER_DEFINED_STRUCTS_H
#define PYR_PROTOCOLS_THIN_ATF_USER_DEFINED_STRUCTS_H

#include "ppinclude.h"
#include "pperror.h"
#include "ppcontainer.h"
#include "commmsgbody.h"
#include "pyrtime.h"
#include "i18n.strmap.h"
#include "Ref.h"
#include "AtfValidator.h"


// for VS2017 C++ complier
#if ( _MSC_VER > 1700 ) && !defined( ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR )
#define ATF_GENERATE_DEFAULT_COPYCONSTRUCTOR
#endif


namespace ThinAtf
{

	// ==========================================================
	//               template definitions
	// ==========================================================

	template<typename T>
	class AtfTempl
	{
	public:
		static void clear(T& _t)
		{
			_t.clear();
		}
		static void clear(T& _t, const T& _default)
		{
			_t = _default;
		}
		static bool Equals(const T& _t1, const T& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const T& _t)
		{
			_t.toTraceString(_buf);			
		}
		static void ComposeMessage(CommMsgBody& _body, const T& _t)
		{
			_t.composeMsg(_body);
		}
		static void ParseMessage(CommMsgParser& _parser, T& _t)
		{
			_t.parseMsg(_parser);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			T::validateMsg(_parser, _checker, _fieldDesc, _fieldsWithUnparsedContent);
		}
	};

	template<>
	class AtfTempl<bool>
	{
	public:
		static void clear(bool& _b, bool _v = false)
		{
			_b = _v;
		}
		static bool Equals(const bool& i1, const bool& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const bool& c)
		{
			_buf.appendUint(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const bool& _t)
		{
			_body.composeBOOL(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, bool& _t)
		{
			_parser.parseBOOL(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			//_parser.skip(1);
			bool _t; _parser.parseBOOL(_t);
		}
	};

	template<>
	class AtfTempl<char>
	{
	public:
		static void clear(char& _i, char _v = '0')
		{
			_i = _v;
		}
		static bool Equals(const char& i1, const char& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const char& c)
		{
			_buf.append(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const char& _t)
		{
			_body.composeINT8(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, char& _t)
		{
			INT8 _n;
			_parser.parseINT8(_n);
			_t = _n;
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(1);
		}
	};

	template<>
	class AtfTempl<INT8>
	{
	public:
		static void clear(INT8& _i, INT8 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const char& i1, const INT8& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const INT8& _t)
		{
			_buf.appendInt(_t);
		}
		static void ComposeMessage(CommMsgBody& _body, const INT8& _t)
		{
			_body.composeINT8(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, INT8& _t)
		{
			_parser.parseINT8(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(1);
		}
	};

	template<>
	class AtfTempl<BYTE>
	{
	public:
		static void clear(BYTE& _i, BYTE _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const BYTE& i1, const BYTE& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const BYTE& c)
		{
			_buf.appendUint(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const BYTE& _t)
		{
			_body.composeBYTE(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, BYTE& _t)
		{
			_parser.parseBYTE(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(1);
		}
	};

	template<>
	class AtfTempl<INT16>
	{
	public:
		static void clear(INT16& _i, INT16 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const INT16& i1, const INT16& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const INT16& c)
		{
			_buf.appendInt(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const INT16& _t)
		{
			_body.composeINT16(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, INT16& _t)
		{
			_parser.parseINT16(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(2);
		}
	};

	template<>
	class AtfTempl<UINT16>
	{
	public:
		static void clear(UINT16& _i, UINT16 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const UINT16& i1, const UINT16& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const UINT16& c)
		{
			_buf.appendUint(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const UINT16& _t)
		{
			_body.composeUINT16(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, UINT16& _t)
		{
			_parser.parseUINT16(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(2);
		}
	};

	template<>
	class AtfTempl<INT32>
	{
	public:
		static void clear(INT32& _i, INT32 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const INT32& i1, const INT32& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const INT32& c)
		{
			_buf.appendInt(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const INT32& _t)
		{
			_body.composeINT32(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, INT32& _t)
		{
			_parser.parseINT32(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(4);
		}
	};

	template<>
	class AtfTempl<UINT32>
	{
	public:
		static void clear(UINT32& _i, UINT32 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const UINT32& i1, const UINT32& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const UINT32& c)
		{
			_buf.appendUint(c);
		}
		static void ComposeMessage(CommMsgBody& _body, const UINT32& _t)
		{
			_body.composeUINT32(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, UINT32& _t)
		{
			_parser.parseUINT32(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(4);
		}
	};

	template<>
	class AtfTempl<INT64>
	{
	public:
		static void clear(INT64& _i, INT64 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const INT64& i1, const INT64& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const INT64& u)
		{
			_buf.appendInt64(u);
		}
		static void ComposeMessage(CommMsgBody& _body, const INT64& _t)
		{
			_body.composeINT64(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, INT64& _t)
		{
			_parser.parseINT64(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(8);
		}
	};

	template<>
	class AtfTempl<UINT64>
	{
	public:
		static void clear(UINT64& _i, UINT64 _v = 0)
		{
			_i = _v;
		}
		static bool Equals(const UINT64& i1, const UINT64& i2)
		{
			return i1 == i2;
		}
		static void ToTraceString(PString& _buf, const UINT64& u)
		{
			_buf.appendUint64(u);
		}
		static void ComposeMessage(CommMsgBody& _body, const UINT64& _t)
		{
			_body.composeUINT64(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, UINT64& _t)
		{
			_parser.parseUINT64(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(8);
		}
	};


	template<>
	class AtfTempl<PString>
	{
	public:
		static void clear(PString& _s)
		{
			_s.clear();
		}
		static void clear(PString& _s, const PString& _defValue)
		{
			_s = _defValue;
		}
		static bool Equals(const PString& _t1, const PString& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const PString& s)
		{
			_buf.append('\"').append(s).append('\"');
		}
		static void ComposeMessage(CommMsgBody& _body, const PString& _t)
		{
			_body.composeString(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, PString& _t)
		{
			_parser.parseStringP(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			const char *_s;
			_parser.parseString(_s);
		}
	};


	template<>
	class AtfTempl<PBlock>
	{
	public:
		static void clear(PBlock& _p)
		{
			_p.clear();
		}
		static bool Equals(const PBlock& _t1, const PBlock& _t2)
		{
			return _t1.size() == _t2.size() && memcmp(_t1.ptr(), _t2.ptr(), _t1.size()) == 0;
		}
		static void ToTraceString(PString& _buf, const PBlock& b)
		{
			_buf.append("{...}"); 
		}
		static void ComposeMessage(CommMsgBody& _body, const PBlock& _t)
		{
			_body._composeVarBlock(_t.ptr(), _t.size());
		}
		static void ParseMessage(CommMsgParser& _parser, PBlock& _t)
		{
			_parser.parsePBlock(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			const BYTE* _p;
			size_t _sz;
			_parser._parseVarBlock(_p, _sz);
		}
	};


	template<>
	class AtfTempl<CommSrvTime>
	{
	public:
		static void clear(CommSrvTime& _t)
		{
			_t.setNull();
		}
		static bool Equals(const CommSrvTime& _t1, const CommSrvTime& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const CommSrvTime& _t)
		{
			// trace it even if _t.isNull()
			_buf.append('{')
				.appendUint(_t._year)
				.append('-')
				.appendUint(_t._month)
				.append('-')
				.appendUint(_t._day)
				.append(' ')
				.appendUint(_t._hour)
				.append(':')
				.appendUint(_t._minute)
				.append(':')
				.appendUint(_t._second)
				//.append(" wday=")
				//.appendUint(_t._wday)
				.append('}')
				;
		}
		static void ComposeMessage(CommMsgBody& _body, const CommSrvTime& _t)
		{
			_body.composeSrvTime(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, CommSrvTime& _t)
		{
			_parser.parseSrvTime(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(8);
		}
	};

	template<>
	class AtfTempl<CommSrvDate>
	{
	public:
		static void clear(CommSrvDate& _t)
		{
			_t.setNull();
		}
		static bool Equals(const CommSrvDate& _t1, const CommSrvDate& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const CommSrvDate& _t)
		{
			// trace it even if _t.isNull()
			_buf.append('{')
				.appendUint(_t._year)
				.append('-')
				.appendUint(_t._month)
				.append('-')
				.appendUint(_t._day)
				.append('}')
				;
		}
		static void ComposeMessage(CommMsgBody& _body, const CommSrvDate& _t)
		{
			_body.composeSrvDate(_t);
		}
		static void ParseMessage(CommMsgParser& _parser, CommSrvDate& _t)
		{
			_parser.parseSrvDate(_t);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(4);
		}
	};


	template<>
	class AtfTempl<CommUtcTime>
	{
	public:
		static void clear(CommUtcTime& _t)
		{
			_t.setNull();
		}
		static bool Equals(const CommUtcTime& _t1, const CommUtcTime& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const CommUtcTime& _t)
		{
			// trace it even if _t.isNull()
			_buf.append('{')
				.appendUint(_t._year)
				.append('-')
				.appendUint(_t._month)
				.append('-')
				.appendUint(_t._day)
				.append(' ')
				.appendUint(_t._hour)
				.append(':')
				.appendUint(_t._minute)
				.append(':')
				.appendUint(_t._second)
				//.append(" wday=")
				//.appendUint(_t._wday)
				.append('}')
				;
		}
		static void ComposeMessage(CommMsgBody& _body, const CommUtcTime& _t)
		{
			_t.compose(_body);
		}
		static void ParseMessage(CommMsgParser& _parser, CommUtcTime& _t)
		{
			_t.parse(_parser);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(8);
		}
	};

	template<>
	class AtfTempl<CommUtcDate>
	{
	public:
		static void clear(CommUtcDate& _t)
		{
			_t.setNull();
		}
		static bool Equals(const CommUtcDate& _t1, const CommUtcDate& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const CommUtcDate& _t)
		{
			// trace it even if _t.isNull()
			_buf.append('{')
				.appendUint(_t._year)
				.append('-')
				.appendUint(_t._month)
				.append('-')
				.appendUint(_t._day)
				.append('}')
				;
		}
		static void ComposeMessage(CommMsgBody& _body, const CommUtcDate& _t)
		{
			_t.compose(_body);
		}
		static void ParseMessage(CommMsgParser& _parser, CommUtcDate& _t)
		{
			_t.parse(_parser);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			_parser.skip(4);
		}
	};

	template<>
	class AtfTempl<I18nPString>
	{
	public:
		static void clear(I18nPString& _t)
		{
			_t.clear();
		}
		static bool Equals(const I18nPString& _t1, const I18nPString& _t2)
		{
			return _t1.equals(_t2);
		}
		static void ToTraceString(PString& _buf, const I18nPString& _t)
		{
			_buf.append("{i18n=[");

			for (UINT32 i = 0; i < _t.size(); ++i)
			{
				if (i > 0)	_buf.append(',');

				UINT32 locale;
				PString msg;
				_t.getPairByInd(i, locale, msg);

				_buf.append('{');
				_buf.append("locale=");
				_buf.appendUint(locale);
				_buf.append(',');
				_buf.append("msg=");
				_buf.append(msg);
				_buf.append('}');
			}

			_buf.append("]}");
		}
		static void ComposeMessage(CommMsgBody& _body, const I18nPString& _t)
		{
			_t.compose(_body);
		}
		static void ParseMessage(CommMsgParser& _parser, I18nPString& _t)
		{
			_t.parse(_parser);
		}
		static void ValidateMessage(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)
		{
			I18nPString _t;
			_t.parse(_parser);
		}
	};


	// ============== AtfVectors ==============
	template <typename T>
	class AtfVectorBase : public std::vector<T>
	{
	typedef std::vector<T> Super;
	public:
		AtfVectorBase()
		{
		}

		AtfVectorBase(const AtfVectorBase& _other)
			: Super(_other)
		{
		}

		AtfVectorBase& operator=(const AtfVectorBase& _other)
		{
			if(this != &_other)
			{
				Super::operator=(_other);
			}
			return *this;
		}

		AtfVectorBase& operator=(const vector<T>& v)
		{
			if(this != &v)
			{
				Super::operator=(v);
			}
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		AtfVectorBase(AtfVectorBase&& _other)
			: Super(std::move(_other))
		{
		}

		AtfVectorBase& operator=(AtfVectorBase&& _other)
		{
			if(this != &_other)
			{
				Super::operator=(std::move(_other));
			}
			return *this;
		}

		AtfVectorBase& operator=(vector<T>&& v)
		{
			if(this != &v)
			{
				Super::operator=(std::move(v));
			}
			return *this;
		}
#endif

		bool equals(const AtfVectorBase& _other) const
		{
			if(Super::size() != _other.size())
			{
				return false;
			}
			for (UINT32 _i=0; _i < Super::size(); ++_i)
			{
				if (! AtfTempl< T >::Equals(Super::at(_i), _other[_i]))
					return false;
			}
			return true;
		}

		void toTraceString(PString& _buf) const
		{
			_buf.append('[');

			if(Super::size())
			{
				for (UINT32 _i=0; _i<Super::size(); ++_i)
				{
					if (_i > 0)	_buf.append(',');
					AtfTempl< T >::ToTraceString(_buf, Super::at(_i));
				}
			}
			_buf.append(']');
		}
	};

	template <typename T, int k>   // k is bytesOfSize
	class LAtfVector : public AtfVectorBase<T>
	{
		typedef AtfVectorBase<T> Super;
	public:	
		LAtfVector()  // L<bytes>Vector
		{
			PASSERT(k == 1 || k == 2 || k == 4);
		}

		LAtfVector(const LAtfVector& _other)
			: Super(_other)
		{
		}

		LAtfVector& operator=(const LAtfVector& _other)
		{
			Super::operator=(_other);
			return *this;
		}

		LAtfVector& operator=(const vector<T>& v)
		{
			Super::operator=(v);
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		LAtfVector(LAtfVector&& _other)
			: Super(std::move(_other))
		{
		}

		LAtfVector& operator=(LAtfVector&& _other)
		{
			Super::operator=(std::move(_other));
			return *this;
		}

		LAtfVector& operator=(vector<T>&& v)
		{
			Super::operator=(std::move(v));
			return *this;
		}
#endif

		void composeMsg(CommMsgBody& _body) const
		{
			switch(k)
			{
				case 1: PASSERT5(Super::size() <= 0xFF); _body.composeBYTE((BYTE)Super::size()); break;
				case 2: PASSERT5(Super::size() <= 0xFFFF); _body.composeUINT16((UINT16)Super::size()); break;
				case 4: _body.composeUINT32(static_cast<UINT32>(Super::size())); break;
				default: PASSERT(0);
			}
			for (UINT32 _i=0; _i< Super::size(); ++_i)
			{
				AtfTempl< T >::ComposeMessage(_body, Super::at(_i));
			}
		}

		void parseMsgN(CommMsgParser& _parser, size_t maxSizeAllowed, const char* fieldDesc = "")
		{
			Super::clear();

			UINT32 _n = 0;	
			switch(k)
			{
				case 1: { BYTE _i; _parser.parseBYTE(_i); _n = _i; } break;
				case 2: { UINT16 _i; _parser.parseUINT16(_i); _n = _i; } break;
				case 4: { UINT32 _i; _parser.parseUINT32(_i); _n = _i; } break;
				default: PASSERT(0);
			}

			if(maxSizeAllowed > 0 && _n > maxSizeAllowed)
			{
				PString err(fieldDesc);
				err.append(" vector size is too big: sz=").appendUint(_n).append(", maxsize=").appendUint(static_cast<UINT32>(maxSizeAllowed));
				throw PParserError(err);
			}

			Super::reserve(_n > 8192 ? 8192 : _n);  // in case huge n consume too much memory here!
			
			for (UINT32 _i=0; _i<_n; ++_i)
			{
				T _r;
				AtfTempl< T >::ParseMessage(_parser, _r);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
				Super::push_back(std::move(_r));
#else
				Super::push_back(_r);
#endif
			}
		}

		void parseMsg(CommMsgParser& _parser)
		{
			parseMsgN(_parser, 0, "");
		}

		static int validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)  // return vector size
		{
			UINT32 _n = 0;	
			switch(k)
			{
				case 1: { BYTE _i; _parser.parseBYTE(_i); _n = _i; } break;
				case 2: { UINT16 _i; _parser.parseUINT16(_i); _n = _i; } break;
				case 4: { UINT32 _i; _parser.parseUINT32(_i); _n = _i; } break;
				default: PASSERT(0);
			}
			for (UINT32 _i=0; _i<_n; ++_i)
			{
				AtfTempl< T >::ValidateMessage(_parser, _checker, _fieldDesc, _fieldsWithUnparsedContent);
			}
			return _n;
		}
	};


	template <typename T, int k>   // k is numOfNullBytes
	class AtfVector0 : public AtfVectorBase<T>
	{
		typedef AtfVectorBase<T> Super;
	public:	
		AtfVector0()     // Vector0
		{
			PASSERT(k > 0);
		}

		AtfVector0(const AtfVector0& _other)
			: Super(_other)
		{
		}

		AtfVector0& operator=(const AtfVector0& _other)
		{
			Super::operator=(_other);
			return *this;
		}

		AtfVector0& operator=(const vector<T>& v)
		{
			Super::operator=(v);
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		AtfVector0(AtfVector0&& _other)
			: Super(std::move(_other))
		{
		}

		AtfVector0& operator=(AtfVector0&& _other)
		{
			Super::operator=(std::move(_other));
			return *this;
		}

		AtfVector0& operator=(vector<T>&& v)
		{
			Super::operator=(std::move(v));
			return *this;
		}
#endif

		void composeMsg(CommMsgBody& _body) const
		{
			for (UINT32 _i=0; _i< Super::size(); ++_i)
			{
				AtfTempl< T >::ComposeMessage(_body,  Super::at(_i));
			}		
			for (BYTE _n = 0; _n < k; ++_n)
			{
				_body.composeBYTE(0);
			}
		}

		void parseMsgN(CommMsgParser& _parser, size_t maxSizeAllowed, const char* fieldDesc = "")
		{
			Super::clear();

			for(;;) 
			{
				// check whether the terminator has reached
				const BYTE *_p = _parser.getMsgBody()._readPtr() + _parser._shift();
				const BYTE *_last = _parser.getMsgBody()._readPtr() + _parser.getMsgBody()._size();

				bool _end = true;
				for (BYTE _n = 0; _n < k; ++_n)
				{
					if ((_p + _n) < _last && *(_p + _n))
					{
						_end = false;
						break;
					}
				}

				if (_end) 
				{
					for (BYTE _n = 0; _n < k; ++_n)
					{
						BYTE _t;
						_parser.parseBYTE(_t);
					}
					return;
				}

				T _r;
				AtfTempl< T >::ParseMessage(_parser, _r);

				if(_parser.parseEnded())
				{
					throw PParserError("vector0ParseMessage: invalid message format!");
				}

				if(maxSizeAllowed > 0 && Super::size() >= maxSizeAllowed)
				{
					PString err(fieldDesc);
					err.append(" vector size is too big: maxsize=").appendUint(static_cast<UINT32>(maxSizeAllowed));
					throw PParserError(err);
				}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
				Super::push_back(std::move(_r));
#else
				Super::push_back(_r);
#endif
			}
		}

		void parseMsg(CommMsgParser& _parser)
		{
			parseMsgN(_parser, 0, "");
		}

		static int validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)  // return vector size
		{
			int _sz = 0;

			for(;;) 
			{
				// check whether the terminator has reached
				const BYTE *_p = _parser.getMsgBody()._readPtr() + _parser._shift();
				const BYTE *_last = _parser.getMsgBody()._readPtr() + _parser.getMsgBody()._size();

				bool _end = true;
				for (BYTE _n = 0; _n < k; ++_n)
				{
					if ((_p + _n) < _last && *(_p + _n))
					{
						_end = false;
						break;
					}
				}

				if (_end) 
				{
					_parser.skip(k);
					return _sz;
				}

				AtfTempl< T >::ValidateMessage(_parser, _checker, _fieldDesc, _fieldsWithUnparsedContent);
				++_sz;

				if(_parser.parseEnded())
				{
					throw PParserError("vector0ValidateMessage: invalid message format!");
				}
			}
		}

	};

	template <typename T>
	class FixedAtfVector : public AtfVectorBase<T>
	{
		typedef AtfVectorBase<T> Super;
		int fixedArraySize;
		T dummy;
	public:
		FixedAtfVector(int arraySize_)
		{
			PASSERT(arraySize_ > 0);
			fixedArraySize = arraySize_;
		}

		FixedAtfVector(const FixedAtfVector& _other)
			: Super(_other), fixedArraySize(_other.fixedArraySize)
		{
		}

		FixedAtfVector& operator=(const FixedAtfVector& _other)
		{
			PASSERT(fixedArraySize == _other.fixedArraySize);
			Super::operator=(_other);
			return *this;
		}

		FixedAtfVector& operator=(const vector<T>& v)
		{
			Super::operator=(v);
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		FixedAtfVector(FixedAtfVector&& _other)
			: Super(std::move(_other)), fixedArraySize(_other.fixedArraySize)
		{
		}

		FixedAtfVector& operator=(FixedAtfVector&& _other)
		{
			PASSERT(fixedArraySize == _other.fixedArraySize);
			Super::operator=(std::move(_other));
			return *this;
		}

		FixedAtfVector& operator=(vector<T>&& v)
		{
			Super::operator=(std::move(v));
			return *this;
		}
#endif

		T& operator[]( size_t pos )
		{
			return at( pos );
		}

		const T& operator[]( size_t pos ) const
		{
			return at( pos );
		}

		T& at( size_t pos )
		{
			if( pos >= Super::size() && pos < fixedArraySize )
			{
				return dummy;
			}
			else
			{
				return Super::at( pos );
			}
		}

		const T& at( size_t pos ) const
		{
			if( pos >= Super::size() && pos < fixedArraySize )
			{
				return dummy;
			}
			else
			{
				return Super::at( pos );
			}
		}

		size_t size() const
		{
			PASSERT( Super::size() <= fixedArraySize );
			return fixedArraySize;
		}

		void composeMsg(CommMsgBody& _body) const
		{
			PASSERT( Super::size() <= fixedArraySize );

			for ( UINT32 _i = 0; _i < fixedArraySize; ++_i )
			{
				AtfTempl< T >::ComposeMessage(_body, at(_i));
			}
		}

		void parseMsg(CommMsgParser& _parser)
		{
			Super::clear();

			Super::reserve(fixedArraySize);

			for ( UINT32 _i = 0; _i < fixedArraySize; ++_i )
			{
				T _r;
				AtfTempl< T >::ParseMessage(_parser, _r);

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
				Super::push_back(std::move(_r));
#else
				Super::push_back(_r);
#endif
			}
		}

		static int validateMsg(CommMsgParser& _parser, int _fixedArraySize, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)  // return vector size
		{
			for ( UINT32 _i = 0; _i < _fixedArraySize; ++_i )
			{
				AtfTempl< T >::ValidateMessage(_parser, _checker, _fieldDesc, _fieldsWithUnparsedContent);
			}
			return _fixedArraySize;
		}
	};


	template <typename T>
	class AtfVectorWithSpecialTerminator : public AtfVectorBase<T>  // Vector.._xx
	{
		typedef AtfVectorBase<T> Super;
		PString terminator;
	public:
		AtfVectorWithSpecialTerminator(const char* terminator_)
		{
			PASSERT(terminator_ && *terminator_ && !strchr(terminator_, ','));
			terminator = terminator_;
		}

		AtfVectorWithSpecialTerminator(const AtfVectorWithSpecialTerminator& _other)
			: Super(_other), terminator(_other.terminator)
		{
		}

		AtfVectorWithSpecialTerminator& operator=(const AtfVectorWithSpecialTerminator& _other)
		{
			PASSERT(terminator.equals(_other.terminator));
			Super::operator=(_other);
			return *this;
		}

		AtfVectorWithSpecialTerminator& operator=(const vector<T>& v)
		{
			Super::operator=(v);
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		AtfVectorWithSpecialTerminator(AtfVectorWithSpecialTerminator&& _other)
			: Super(std::move(_other)), terminator(std::move(_other.terminator))
		{
		}

		AtfVectorWithSpecialTerminator& operator=(AtfVectorWithSpecialTerminator&& _other)
		{
			PASSERT(terminator.equals(_other.terminator));
			Super::operator=(std::move(_other));
			return *this;
		}

		AtfVectorWithSpecialTerminator& operator=(vector<T>&& v)
		{
			Super::operator=(std::move(v));
			return *this;
		}
#endif

		void composeMsg(CommMsgBody& _body) const
		{
			for (UINT32 _i=0; _i< Super::size(); ++_i)
			{
				AtfTempl< T >::ComposeMessage(_body,  Super::at(_i));
			}
			PBlock _block;
			if(!convertTerminator(terminator, _block))
			{
				PASSERT(false);   // wrong terminator format
			}
			memcpy(_body._append(_block.size()), _block.ptr(), _block.size());
		}

		void parseMsgN(CommMsgParser& _parser, size_t maxSizeAllowed, const char* fieldDesc = "")
		{
			Super::clear();

			PBlock _block;
			if(!convertTerminator(terminator, _block))
			{
				PASSERT(false);   // wrong terminator format
			}
			const BYTE *_term = _block.ptr();
			size_t _termLen = _block.size();

			for(;;) 
			{
				// check whether the terminator has reached
				if (memcmp(_parser.getMsgBody()._readPtr() + _parser._shift(), _term, _termLen) == 0)
				{
					// consume the terminator bytes
					for (BYTE _n = 0; _n < _termLen; ++_n)
					{
						BYTE _t;
						_parser.parseBYTE(_t);
					}
					return;
				}

				T _r;
				AtfTempl< T >::ParseMessage(_parser, _r);

				if(_parser.parseEnded())
				{
					throw PParserError("vectorWithSpecialTerminatorParseMessage: invalid message format!");
				}

				if(maxSizeAllowed > 0 && Super::size() >= maxSizeAllowed)
				{
					PString err(fieldDesc);
					err.append(" vector size is too big: maxsize=").appendUint(static_cast<UINT32>(maxSizeAllowed));
					throw PParserError(err);
				}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
				Super::push_back(std::move(_r));
#else
				Super::push_back(_r);
#endif
			}
		}

		void parseMsg(CommMsgParser& _parser)
		{
			parseMsgN(_parser, 0, "");
		}
		
		static int validateMsg(CommMsgParser& _parser, const char* _terminator, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)  // return vector size
		{
			int _sz = 0;

			PBlock _block;
			if(!convertTerminator(_terminator, _block))
			{
				PASSERT(false);   // wrong terminator format
			}
			const BYTE *_term = _block.ptr();
			size_t _termLen = _block.size();

			for(;;) 
			{
				// check whether the terminator has reached
				if (memcmp(_parser.getMsgBody()._readPtr() + _parser._shift(), _term, _termLen) == 0)
				{
					// consume the terminator bytes
					_parser.skip(_termLen);
					return _sz;
				}

				AtfTempl< T >::ValidateMessage(_parser, _checker, _fieldDesc, _fieldsWithUnparsedContent);
				++_sz;

				if(_parser.parseEnded())
				{
					throw PParserError("vectorWithSpecialTerminatorValidateMessage: invalid message format!");
				}
			}
		}

		static bool convertTerminator(const char* _terminator, PBlock& block)
		{
			while(*_terminator && *(_terminator+1))
			{
				BYTE result = 0;

				char b1 = *_terminator;
				if (b1 >= '0' && b1 <= '9')
				{
					result = b1 - '0';
				}
				else if (b1 >= 'A' && b1 <= 'F')
				{
					result = b1 - 'A' + 10;
				}
				else if (b1 >= 'a' && b1 <= 'f')
				{
					result = b1 - 'a' + 10;
				}
				else
				{
					return false;
				}
				
				++_terminator;
				result <<= 4;

				char b2 = *_terminator;

				if (b2 >= '0' && b2 <= '9')
				{
					result += b2 - '0';
				}
				else if (b2 >= 'A' && b2 <= 'F')
				{
					result += b2 - 'A' + 10;
				}
				else if (b2 >= 'a' && b2 <= 'f')
				{
					result += b2 - 'a' + 10;
				}
				else
				{
					return false;
				}

				++_terminator;
				*block.append(1) = result;
			}

			//*block.append(1) = '\0';
			return *_terminator == 0;
		}
	};


	// MsgBodyWrappedAtfVector
	// there is no length or terminator characters composed in the msg body,
	// instead, the vector boundary is wrapped by a msg body
	template <typename T>
	class MsgBodyWrappedAtfVector : public AtfVectorBase<T>
	{
		typedef AtfVectorBase<T> Super;
	public:
		MsgBodyWrappedAtfVector()
		{
		}

		MsgBodyWrappedAtfVector(const MsgBodyWrappedAtfVector& _other)
			: Super(_other)
		{
		}

		MsgBodyWrappedAtfVector& operator=(const MsgBodyWrappedAtfVector& _other)
		{
			Super::operator=(_other);
			return *this;
		}

		MsgBodyWrappedAtfVector& operator=(const vector<T>& v)
		{
			Super::operator=(v);
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		MsgBodyWrappedAtfVector(MsgBodyWrappedAtfVector&& _other)
			: Super(std::move(_other))
		{
		}

		MsgBodyWrappedAtfVector& operator=(MsgBodyWrappedAtfVector&& _other)
		{
			Super::operator=(std::move(_other));
			return *this;
		}

		MsgBodyWrappedAtfVector& operator=(vector<T>&& v)
		{
			Super::operator=(std::move(v));
			return *this;
		}
#endif

		void composeMsg(CommMsgBody& _body) const
		{
			CommMsgBody _body0;
			for (UINT32 _i=0; _i< Super::size(); ++_i)
			{
				AtfTempl< T >::ComposeMessage(_body0, Super::at(_i));
			}
			_body.composeMsgBody(_body0);
		}

		void parseMsgN(CommMsgParser& _parser, size_t maxSizeAllowed, const char* fieldDesc = "")
		{
			Super::clear();

			CommMsgBody _body0;
			_parser.parseMsgBody(_body0);
			CommMsgParser _parser0(_body0);

			while(!_parser0.parseEnded())
			{
				T _r;
				AtfTempl< T >::ParseMessage(_parser0, _r);

				if(maxSizeAllowed > 0 && Super::size() >= maxSizeAllowed)
				{
					PString err(fieldDesc);
					err.append(" vector size is too big: maxsize=").appendUint(maxSizeAllowed);
					throw PParserError(err);
				}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
				Super::push_back(std::move(_r));
#else
				Super::push_back(_r);
#endif
			}
		}

		void parseMsg(CommMsgParser& _parser)
		{
			parseMsgN(_parser, 0, "");
		}

		static int validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent)  // return vector size
		{
			CommMsgBody _body0;
			_parser.parseMsgBody(_body0);
			CommMsgParser _parser0(_body0);

			int _sz = 0;
			while(!_parser0.parseEnded())
			{
				AtfTempl< T >::ValidateMessage(_parser0, _checker, _fieldDesc, _fieldsWithUnparsedContent);
				++_sz;
			}
			return _sz;
		}
	};


	// AtfVectorM class is to wrap each vector item inside a msgbody inside the message composing and parsing.
	// For JSON and XML format serialization/deserialization, there is no extra change (same as vector<T>)
	template <typename T>
	class AtfVectorM : public AtfVectorBase<T>
	{
		typedef AtfVectorBase<T> Super;
	public:
		AtfVectorM()
		{
		}

		AtfVectorM( const AtfVectorM& _other )
			: Super( _other )
		{
		}

		AtfVectorM& operator=( const AtfVectorM& _other )
		{
			Super::operator=( _other );
			return *this;
		}

		AtfVectorM& operator=( const vector<T>& v )
		{
			Super::operator=( v );
			return *this;
		}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		AtfVectorM( AtfVectorM&& _other )
			: Super( std::move( _other ) )
		{
		}

		AtfVectorM& operator=( AtfVectorM&& _other )
		{
			Super::operator=( std::move( _other ) );
			return *this;
		}

		AtfVectorM& operator=( vector<T>&& v )
		{
			Super::operator=( std::move( v ) );
			return *this;
		}
#endif

		void composeMsg( CommMsgBody& _body ) const
		{
			_body.composeUINT32( static_cast<UINT32>( Super::size() ) );

			for( UINT32 _i = 0; _i < Super::size(); ++_i )
			{
				CommMsgBody _body0;
				AtfTempl< T >::ComposeMessage( _body0, Super::at( _i ) );
				_body.composeMsgBody( _body0 );
			}
		}

		void parseMsgN( CommMsgParser& _parser, size_t maxSizeAllowed, const char* fieldDesc )
		{
			Super::clear();

			UINT32 _n = 0;
			_parser.parseUINT32( _n );

			if( maxSizeAllowed > 0 && _n > maxSizeAllowed )
			{
				PString err( fieldDesc );
				err.append( " vector size is too big: sz=" ).appendUint( _n ).append( ", maxsize=" )
					.appendUint( static_cast<UINT32>( maxSizeAllowed ) );
				throw PParserError( err );
			}

			Super::reserve( _n > 8192 ? 8192 : _n );  // in case huge n consume too much memory here! //-V547

			for( UINT32 _i = 0; _i < _n; ++_i )
			{
				CommMsgBody _body0;
				_parser.parseMsgBody( _body0 );
				CommMsgParser _parser0( _body0 );

				T _r;
				AtfTempl< T >::ParseMessage( _parser0, _r );

				if( maxSizeAllowed > 0 && Super::size() >= maxSizeAllowed )
				{
					PString err( fieldDesc );
					err.append( " vector size is too big: maxsize=" ).appendUint( maxSizeAllowed );
					throw PParserError( err );
				}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
				Super::push_back( std::move( _r ) );
#else
				Super::push_back( _r );
#endif
			}
		}

		void parseMsg( CommMsgParser& _parser )
		{
			parseMsgN( _parser, 0, "" );
		}

		static int validateMsg( CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent )  // return vector size
		{
			UINT32 _n = 0;
			_parser.parseUINT32( _n );

			for( UINT32 _i = 0; _i < _n; ++_i )
			{
				CommMsgBody _body0;
				_parser.parseMsgBody( _body0 );
				CommMsgParser _parser0( _body0 );

				AtfTempl< T >::ValidateMessage( _parser0, _checker, _fieldDesc, _fieldsWithUnparsedContent );
			}

			return _n;
		}
	};



	// CopyableMsgBody: only for small size CommMsgBody, 
	// if the body size is too big, please use MoveableMsgBody or BigSharedMsgBody type

	class CopyableMsgBody : public CommMsgBody
	{
		static const int smallCopyableMsgSizeMax = 8192;
	public:
		CopyableMsgBody() {}
		CopyableMsgBody(const CopyableMsgBody& _o) { copyFrom(_o); }
		CopyableMsgBody(const CommMsgBody& _o) { copyFrom(_o); }
		CopyableMsgBody(CommMsgBody& _o, bool) { moveFrom(_o); }

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		CopyableMsgBody( CopyableMsgBody&& _o ) { moveFrom(_o); }
		CopyableMsgBody( CommMsgBody&& _o ) { moveFrom(_o); }
		CopyableMsgBody& operator=( CopyableMsgBody&& _o ) { if(this != &_o) moveFrom( _o ); return *this; }
		CopyableMsgBody& operator=( CommMsgBody&& _o ) { if(this != &_o) moveFrom( _o ); return *this; }
#endif

		CopyableMsgBody& operator=( const CopyableMsgBody& _o ) { if(this != &_o) copyFrom( _o ); return *this; }
		CopyableMsgBody& operator=( const CommMsgBody& _o) { if(this != &_o) copyFrom(_o); return *this; }

		bool equals(const CommMsgBody& _o) const { return msgBodyContentEquals(*this, _o); }
		const char *toTraceString(PString& _buf) const { return _buf.append("{CommMsgBody:").appendUint(static_cast<UINT32>(_size())).append("}"); }
		void composeMsg(CommMsgBody& body) const { body.composeMsgBody(*this); }
		void parseMsg(CommMsgParser& parser) { parser.parseMsgBody(*this); }
		void parseMsgN(CommMsgParser& parser, size_t maxSizeAllowed, const char* fieldDesc = "") { parser.parseMsgBodyN(*this, maxSizeAllowed, fieldDesc); }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent) { _parser.skipMsgBody(); }

		static bool msgBodyContentEquals(const CommMsgBody& _a, const CommMsgBody& _b)
		{
			return _a._size() == _b._size() && memcmp(_a._readPtr(), _b._readPtr(), _a._size()) == 0;
		}
	};



	// move only no copy version of CommMsgBody, no restriction for body size
	class MoveableMsgBody : public CommMsgBody
	{
	public:
		MoveableMsgBody() {}
		MoveableMsgBody(CommMsgBody& _o, bool) { moveFrom(_o); }

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		MoveableMsgBody( MoveableMsgBody&& _o ) { moveFrom(_o); }
		MoveableMsgBody( CommMsgBody&& _o ) { moveFrom(_o); }
		MoveableMsgBody& operator=( MoveableMsgBody&& _o ) { if(this != &_o) moveFrom( _o ); return *this; }
		MoveableMsgBody& operator=( CommMsgBody&& _o ) { if(this != &_o) moveFrom( _o ); return *this; }
#endif

		bool equals(const CommMsgBody& _o) const { return CopyableMsgBody::msgBodyContentEquals(*this, _o); }
		const char *toTraceString(PString& _buf) const { return _buf.append("{CommMsgBody:").appendUint(static_cast<UINT32>(_size())).append("}"); }
		void composeMsg(CommMsgBody& body) const { body.composeMsgBody(*this); }
		void parseMsg(CommMsgParser& parser) { parser.parseMsgBody(*this); }
		void parseMsgN(CommMsgParser& parser, size_t maxSizeAllowed, const char* fieldDesc = "") { parser.parseMsgBodyN(*this, maxSizeAllowed, fieldDesc); }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent) { _parser.skipMsgBody(); }

	private:
		MoveableMsgBody(const CommMsgBody& _o);  // prohibit
		MoveableMsgBody(const MoveableMsgBody& _o);  // prohibit
		MoveableMsgBody& operator=( const CommMsgBody& _o);  // prohibit
		MoveableMsgBody& operator=( const MoveableMsgBody& _o);  // prohibit
	};



	// if the msgBody size is too big but still want to put it into a vector or other std container,
	// you could consider to use BigSharedMsgBody type.
	class BigSharedMsgBody
	{
		struct CommMsgBodyRefCounted : public RefCounted { CommMsgBody body; };
		TRef< CommMsgBodyRefCounted > bodyPtr;

	public:
		BigSharedMsgBody() : bodyPtr(new CommMsgBodyRefCounted()) {}
		BigSharedMsgBody(const BigSharedMsgBody& _o) { bodyPtr = _o.bodyPtr; }
		BigSharedMsgBody(const CommMsgBody& _o) { copyFrom(_o); }
		BigSharedMsgBody(CommMsgBody& _o, bool) { moveFrom(_o); }

		BigSharedMsgBody& operator=(const CommMsgBody& _o) { copyFrom(_o); return *this; }
		BigSharedMsgBody& operator=(const BigSharedMsgBody& _o) { if(this != &_o) bodyPtr = _o.bodyPtr; return *this; }

		bool equals(const BigSharedMsgBody& _o) const { return bodyPtr.ptr() == _o.bodyPtr.ptr() || CopyableMsgBody::msgBodyContentEquals(*this, _o); }
		const char *toTraceString(PString& _buf) const { return _buf.append("{BigSharedMsgBody:").appendUint(static_cast<UINT32>(_size())).append("}"); }

		void composeMsg(CommMsgBody& body) const { body.composeMsgBody(bodyPtr->body); }
		void parseMsg(CommMsgParser& parser) { parser.parseMsgBody(bodyPtr->body); }
		void parseMsgN(CommMsgParser& parser, size_t maxSizeAllowed, const char* fieldDesc = "") { parser.parseMsgBodyN(bodyPtr->body, maxSizeAllowed, fieldDesc); }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent) { _parser.skipMsgBody(); }

		operator const CommMsgBody&() const { return bodyPtr->body; }
		operator CommMsgBody&() { return bodyPtr->body; }

	public: // interfaces copied from CommMsgBody
		void moveFrom( CommMsgBody& other ) { if(&bodyPtr->body != &other) bodyPtr->body.moveFrom(other); }
		void moveFrom( PBlock& other ) { bodyPtr->body.moveFrom(other); }
		void copyFrom( const CommMsgBody& other ) { if(&bodyPtr->body != &other) bodyPtr->body.copyFrom(other); }
		void copyFromSkipReqId( const CommMsgBody& other ) { bodyPtr->body.copyFromSkipReqId(other); }

		size_t _size() const { return bodyPtr->body._size(); }

		void _skipPrefix( size_t prefixSz ) { bodyPtr->body._skipPrefix(prefixSz); }
		void _shift( size_t offset_ ) { bodyPtr->body._shift(offset_); }

		void clear() { bodyPtr->body.clear(); }
		void merge( const CommMsgBody& other ) { bodyPtr->body.merge(other); }
		void mergeSkipReqId( const CommMsgBody& other ) { bodyPtr->body.mergeSkipReqId(other); }

		BigSharedMsgBody& composeINT8( INT8 b ) { bodyPtr->body.composeINT8(b); return *this; }
		BigSharedMsgBody& composeBYTE( BYTE b ) { bodyPtr->body.composeBYTE(b); return *this; }
		BigSharedMsgBody& composeBOOL( bool b ) { bodyPtr->body.composeBOOL(b); return *this; }
		BigSharedMsgBody& composeINT16( INT16 i ) { bodyPtr->body.composeINT16(i); return *this; }
		BigSharedMsgBody& composeUINT16( UINT16 i ) { bodyPtr->body.composeUINT16(i); return *this; }
		BigSharedMsgBody& composeINT32( INT32 i ) { bodyPtr->body.composeINT32(i); return *this; }
		BigSharedMsgBody& composeUINT32( UINT32 i ) { bodyPtr->body.composeUINT32(i); return *this; }
		BigSharedMsgBody& composeINT64( INT64 i ) { bodyPtr->body.composeINT64(i); return *this; }
		BigSharedMsgBody& composeUINT64( UINT64 i ) { bodyPtr->body.composeUINT64(i); return *this; }
		BigSharedMsgBody& composeString( const char* s ) { bodyPtr->body.composeString(s); return *this; }
		BigSharedMsgBody& composeMsgBody( const CommMsgBody& m ) { bodyPtr->body.composeMsgBody(m); return *this; }
		BigSharedMsgBody& composeMsgBodySkipReqId( const CommMsgBody& m ) { bodyPtr->body.composeMsgBodySkipReqId(m); return *this; }
		BigSharedMsgBody& _composeVarBlock( const BYTE* ptr, size_t sz ) { bodyPtr->body._composeVarBlock(ptr, sz); return *this; }
		BigSharedMsgBody& composePBlock( const PBlock& block ) { bodyPtr->body.composePBlock(block); return *this; }
		BigSharedMsgBody& composeSrvTime( const CommSrvTime& srvTime ) { bodyPtr->body.composeSrvTime(srvTime); return *this; }
		BigSharedMsgBody& composeSrvDate( const CommSrvDate& srvDate ) { bodyPtr->body.composeSrvDate(srvDate); return *this; }
		BigSharedMsgBody& copyMsgBodyFromParser( const CommMsgParser& parser ) { bodyPtr->body.copyMsgBodyFromParser(parser); return *this; }

		BigSharedMsgBody& composeVectorSize( UINT32 i, size_t & shift ) { bodyPtr->body.composeVectorSize(i, shift); return *this; }
		void updateVectorSize( UINT32 i, size_t shift ) { bodyPtr->body.updateVectorSize(i,shift); }

		template< class T >
		BigSharedMsgBody& composeObject( const T& obj ) { obj.compose( bodyPtr->body ); return *this; }
	};


	class MsgIdBodyPair  // the format of the msgBody is identified by msgId. 
	{
	public:
		UINT32 msgId;
		CopyableMsgBody msgBody;

		MsgIdBodyPair() { clear(); }
		virtual ~MsgIdBodyPair() {}

		MsgIdBodyPair(const MsgIdBodyPair& _o) { msgId = _o.msgId; msgBody.copyFrom(_o.msgBody); }
		MsgIdBodyPair& operator=(const MsgIdBodyPair& _o) { if(this != &_o) { msgId = _o.msgId; msgBody.copyFrom(_o.msgBody); } return *this; }

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
		MsgIdBodyPair(MsgIdBodyPair&& _o) { msgId = _o.msgId; msgBody.moveFrom(_o.msgBody); }
		MsgIdBodyPair& operator=(MsgIdBodyPair&& _o) { if(this != &_o) { msgId = _o.msgId; msgBody.moveFrom(_o.msgBody); } return *this; }
#endif

		void clear() { msgId = 0; msgBody.clear(); }
		bool equals(const MsgIdBodyPair& _o) const { return msgId == _o.msgId && msgBody.equals(_o.msgBody); }
		const char *toTraceString(PString& _buf) const { return _buf.append("{msgId=").appendUint(msgId).append(",msgBody={CommMsgBody:").appendUint(static_cast<UINT32>(msgBody._size())).append("}}"); }
		void composeMsg(CommMsgBody& _msg) const { _msg.composeUINT32(msgId); _msg.composeMsgBody(msgBody); }
		void parseMsg(CommMsgParser& _parser) { _parser.parseUINT32(msgId); _parser.parseMsgBody(msgBody); }
		void parseMsgN(CommMsgParser& _parser, size_t maxSizeAllowed, const char* fieldDesc = "") {  _parser.parseUINT32(msgId); _parser.parseMsgBodyN(msgBody, maxSizeAllowed, fieldDesc); }
		static void validateMsg(CommMsgParser& _parser, const AtfMsgChecker* _checker, const char *_fieldDesc, PString& _fieldsWithUnparsedContent) { _parser.skip(4).skipMsgBody(); }
	};

};



#endif


