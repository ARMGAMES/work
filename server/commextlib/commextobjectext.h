#ifndef commextobjectext_h_included
#define commextobjectext_h_included
#include "commext.h"
#include "commextobject.h"



class CommExtObjectEx : public CommExtObject
{

public:
	struct CommExtObjectMember
	{
		size_t offset_value;
		virtual void parse(void * _this,CommMsgParser & parser) const = 0;
		virtual void compose(const void * _this,CommMsgBody & body) const = 0;
		CommExtObjectMember(size_t offset_value_)
		{
			offset_value = offset_value_;
		}
	};
	typedef PStringMap <CommExtObjectMember*> MemberMap;

	struct CommExtObjectItemVector 
	{
		size_t offset_value;
		void parse(void * _this,CommMsgParser & parser) const 
		{
			UINT32 n;
			parser.parseUINT32(n);
			for (UINT32 i=0; i<n; i++)
			{
				UINT32 id;
				parser.parseUINT32(id);
				((vector<UINT32> *)((BYTE *)_this + offset_value))->push_back(id);
			}

		}
		void compose(const void * _this,CommMsgBody & body) const
		{
			body.composeUINT32( ((vector<UINT32> *)((BYTE *)_this + offset_value))->size() );
			for (UINT32 i=0; i<((vector<UINT32> *)((BYTE *)_this + offset_value))->size(); i++)
			{
				body.composeUINT32( (*(vector<UINT32> *)((BYTE *)_this + offset_value))[i]);
			}
		}
		const vector<UINT32> & get(const void * _this) const
		{
			return *((vector<UINT32> *)((BYTE *)_this + offset_value));
		}

		CommExtObjectItemVector(size_t offset_value_)
		{
			offset_value = offset_value_;
		}
	};
	typedef PStringMap <CommExtObjectItemVector*> ItemMap;


private:
	struct CommExtObjectMemberUINT32 : public CommExtObjectMember
	{
		void parse(void * _this, CommMsgParser & parser) const override
		{
			parser.parseUINT32(*(UINT32*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this, CommMsgBody & body) const override
		{
			body.composeUINT32(*(const UINT32*)((const BYTE *)_this + offset_value));
		}
		CommExtObjectMemberUINT32(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	struct CommExtObjectMemberString : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			parser.parseStringP(*(PString*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeString(*(const PString*)((const BYTE *)_this + offset_value));
		}
		CommExtObjectMemberString(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	struct CommExtObjectMemberUINT64 : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser)const  override
		{
			parser.parseUINT64(*(UINT64*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeUINT64(*(const UINT64*)((const BYTE *)_this + offset_value));
		}
		CommExtObjectMemberUINT64(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	struct CommExtObjectMemberBlock : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			parser.parsePBlock(*(PBlock*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body._composeVarBlock( ((const PBlock*)((const BYTE *)_this + offset_value))->ptr(), ((const PBlock*)((const BYTE *)_this + offset_value))->size());
		}
		CommExtObjectMemberBlock(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	struct CommExtObjectMemberDate : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			parser.parseSrvDate(*(CommSrvDate*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeSrvDate(*(const CommSrvDate*)((const BYTE *)_this + offset_value));
		}
		CommExtObjectMemberDate(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	struct CommExtObjectMemberTime : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			parser.parseSrvTime(*(CommSrvTime*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeSrvTime(*(const CommSrvTime*)((const BYTE *)_this + offset_value));
		}
		CommExtObjectMemberTime(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	template< class T >
	struct CommExtObjectMemberTemplate : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			parser.parseObject<T>(*(T*)((BYTE *)_this + offset_value));
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeObject<T>(*(const T*)((const BYTE *)_this + offset_value));
		}
		CommExtObjectMemberTemplate(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	template< class T >
	struct CommExtObjectMemberVectorType : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			UINT32 n;
			parser.parseUINT32(n);
			for (UINT32 i=0; i<n; i++)
			{
				T & t = * ((vector<T> *)((BYTE *)_this + offset_value)) ->insert(((vector<T> *)((BYTE *)_this + offset_value))->end(),T());
				t.parse(parser);
			}
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeUINT32( ((vector<T> *)((BYTE *)_this + offset_value))->size() );
			for (UINT32 i=0; i<((vector<T> *)((BYTE *)_this + offset_value))->size(); i++)
			{
				(*((const vector<T> *)((const BYTE *)_this + offset_value)))[i].compose(body);
			}
		}
		CommExtObjectMemberVectorType(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};

	class _UINT32Composer
	{
	public:
		const UINT32 & v;
		void compose(CommMsgBody & body) const 
		{
			body.composeUINT32(v);
		}
		_UINT32Composer(const UINT32 &v_) : v(v_)
		{
		}
	};
	class _UINT32Parser
	{
	public:
		UINT32 & v;
		void parse(CommMsgParser &parser)
		{
			parser.parseUINT32(v);
		}
		_UINT32Parser(UINT32 &v_) : v(v_)
		{
		}
	};

	class _StringComposer
	{
	public:
		const PString & v;
		void compose(CommMsgBody & body)const 
		{
			body.composeString(v);
		}
		_StringComposer(const PString &v_) : v(v_)
		{
		}
	};
	class _StringParser
	{
	public:
		PString & v;
		void parse(CommMsgParser &parser)
		{
			parser.parseStringP(v);
		}
		_StringParser(PString &v_) : v(v_)
		{
		}
	};


	template< class T1, class T2, class T3 >
	struct CommExtObjectMemberVector : public CommExtObjectMember
	{
		void parse(void * _this,CommMsgParser & parser) const override
		{
			UINT32 n;
			parser.parseUINT32(n);
			for (UINT32 i=0; i<n; i++)
			{
				T1 & t = * ((vector<T1> *)((BYTE *)_this + offset_value))->insert( ((vector<T1> *)((BYTE *)_this + offset_value))->end(),T1());
				T2 t1(t);
				t1.parse(parser);
			}
		}
		void compose(const void * _this,CommMsgBody & body) const override
		{
			body.composeUINT32( ((const vector<T1> *)((const BYTE *)_this + offset_value))->size() );
			for (UINT32 i=0; i<((const vector<T1> *)((const BYTE *)_this + offset_value))->size(); i++)
			{
				T3 t((*((const vector<T1> *)((const BYTE *)_this + offset_value)))[i]);
				t.compose(body);
			}
		}
		CommExtObjectMemberVector(size_t offset_value_) : CommExtObjectMember(offset_value_)
		{
		}
	};


	struct _CommExtObjectUnknownMember
	{
		PString name;
		virtual void parse(CommMsgParser & parser) = 0;
		virtual void compose(CommMsgBody & body) const = 0;
	};
	struct _CommExtObjectUnknownMemberUINT32 : public _CommExtObjectUnknownMember
	{
		UINT32 value;
		void parse(CommMsgParser & parser) override
		{
			parser.parseUINT32(value);
		}
		void compose(CommMsgBody & body) const override
		{
			body.composeUINT32(value);
		}
	};
	struct _CommExtObjectUnknownMemberString : public _CommExtObjectUnknownMember
	{
		PString value;
		void parse(CommMsgParser & parser) override
		{
			parser.parseStringP(value);
		}
		void compose(CommMsgBody & body) const override
		{
			body.composeString(value);
		}
	};
	struct _CommExtObjectUnknownMemberUINT64 : public _CommExtObjectUnknownMember
	{
		UINT64 value;
		void parse(CommMsgParser & parser) override
		{
			parser.parseUINT64(value);
		}
		void compose(CommMsgBody & body) const override
		{
			body.composeUINT64(value);
		}
	};
	struct _CommExtObjectUnknownMemberBlock : public _CommExtObjectUnknownMember
	{
		PBlock value;
		void parse(CommMsgParser & parser) override
		{
			parser.parsePBlock(value);
		}
		void compose(CommMsgBody & body) const override
		{
			body._composeVarBlock(value.ptr(),value.size());
		}
	};

	struct _CommExtObjectUnknownMemberDate : public _CommExtObjectUnknownMember
	{
		CommSrvDate value;
		void parse(CommMsgParser & parser) override
		{
			parser.parseSrvDate(value);
		}
		void compose(CommMsgBody & body) const override
		{
			body.composeSrvDate(value);
		}
	};
	struct _CommExtObjectUnknownMemberTime : public _CommExtObjectUnknownMember
	{
		CommSrvTime value;
		void parse(CommMsgParser & parser) override
		{
			parser.parseSrvTime(value);
		}
		void compose(CommMsgBody & body) const override
		{
			body.composeSrvTime(value);
		}
	};

	list <_CommExtObjectUnknownMember *> unknownMembers;



public:
//	UINT32 objectId;

	static void declareUINT32(const char * name, size_t v, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberUINT32(v)));
	}
	static void declareUINT64(const char * name, size_t v, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberUINT64(v)));
	}
	static void declareBlock(const char * name, size_t v, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberBlock(v)));
	}
	static void declareString(const char * name, size_t v, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberString(v)));
	}
	static void declareDate(const char * name, size_t v, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberDate(v)));
	}
	static void declareTime(const char * name, size_t v, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberTime(v)));
	}
	static void declareObjectMember(const char * name, CommExtObjectMember * m, MemberMap & memberMap)
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, m));
	}

	template< class T > static
	void declareObject( const char * name, size_t v, MemberMap & memberMap )
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberTemplate<T>(v)));
	}
	template< class T > static
	void declareVector( const char * name, size_t v, MemberMap & memberMap )
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberVectorType<T>(v)));
	}

	static void declareUINT32Vector( const char * name, size_t v, MemberMap & memberMap )
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberVector<UINT32,_UINT32Parser,_UINT32Composer>(v)));
	}
	static void declareStringVector( const char * name, size_t v, MemberMap & memberMap )
	{
		PASSERT3(strcmp(name,"idObject"));
		memberMap.insert( MemberMap::value_type( name, new CommExtObjectMemberVector<PString,_StringParser,_StringComposer>(v)));
	}

	CommExtObjectEx()
	{
//		objectId = 0;
	}
	
	static void declareItemVector( const char * name, size_t v, ItemMap & itemMap )
	{
		itemMap.insert( ItemMap::value_type( name, new CommExtObjectItemVector(v)));
	}



	virtual MemberMap * getMap() const 
	{
		return 0;
	}
	virtual ItemMap * getItemMap() const 
	{
		return 0;
	}
	virtual void parse(CommMsgParser & parser);
	virtual void compose(CommMsgBody & body) const;
	virtual ~CommExtObjectEx();
};


#endif
