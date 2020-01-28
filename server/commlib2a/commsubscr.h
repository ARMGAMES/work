#ifndef commsubscr_h_included
#define commsubscr_h_included

#include "ppinclude.h"
#include "ppblock.h"
#include "ppthread.h"
#include "pplogfile.h"
#include "commmsgbody.h"

class CommSubscription
{
public:
	enum { _StateSubscrDisc = 0, _StateSubscrPeerAssigned = 1, _StateSubscrRequestSent = 2, _StateSubscrSync = 3, _StateSubscrDesync = 4, _StateSubscrFrozenDesync = 5, _StateSubscrFrozenDisc = 6,_StateSubscrServerFatalError = -1, _StateSubscrUnexpectedError = -2 };
	class OnTheFlyCallback
	{
	public:
		virtual void onlineUpdateAttachment( UINT32 msgId, CommMsgBody& ) {}
	};

public:
	class Path
	{
	private:
		friend class CommSubscription;
		enum{ maxPath = 16 };

		int path_[ maxPath ];
		int pathLen_;

	public:
		Path()
		{
			pathLen_ = 0;
		}

		template< size_t N > Path( const int ( &p )[ N ], size_t len = N )
		{
			PASSERT( ( len <= N ) && ( len <= maxPath ) );
			memmove( path_, p, sizeof( *p ) * len );
			pathLen_ = static_cast< int >( len );
		}

	public:
		const int* path() const
		{
			return path_;
		}
		int pathLen() const
		{
			return pathLen_;
		}

	private:
		bool _canIncPathLen() const
		{
			return pathLen_ < maxPath - 1;
		}
		static bool _isValidPathLen( int len )
		{
			return len <= maxPath;
		}
	};

protected:
	struct _Node;
	struct _DelItem
	{
		BYTE isLeaf;
		UINT32 id;
		UINT32 rev;
		_DelItem* nextInSubscr;//in CommSubscription::subscrDelItems
		_Node* node;
		_DelItem* nextInNode;//in _Node::nodeDelItems
		_DelItem* prevInNode;//in _Node::nodeDelItems
	};
	struct _Node
	{
	public:
		UINT32 id;
		UINT32 rev;
		BYTE isLeaf;
		CommMsgBody leaf;//if isLeaf
		vector< /* new */ _Node* > children;//if !isLeaf
		int nItems;//meaningful if !isLeaf only
		_DelItem* firstNodeDelItems;//sorted by ascending rev (most recent - last)
		_DelItem* lastNodeDelItems;

	public:
		_Node()
		{
			nItems = 0;
			firstNodeDelItems = lastNodeDelItems = 0;
		}
		~_Node()
		{
			_clear();
		}

	private:
		friend class CommSubscription;
		void clear()
		{
			_clear();
			children.clear();
			nItems = 0;
		}

	private:
		void _clear();
	};

protected:
	bool callback;
	_Node root;
	BYTE hdr[ 16 ];

	int percentOfDelItemsToStore;
	_DelItem* subscrDelItems;//sorted by ascending rev (most recent - last)
	_DelItem* lastSubscrDelItems;
	int nSubscrDelItems;
	UINT32 lastGoodDelItemRev;

public:
	CommSubscription( bool callback_ = true );
	virtual ~CommSubscription()
	{
		_clearDelHistory();
	}
	void clear()
	{
		root.clear();
		_clearDelHistory();
	}

public:
	size_t size( const int* path, int pathLen ) const;
	bool isLeaf( const int* path, int pathLen ) const;
	const CommMsgBody& leaf( const int* path, int pathLen ) const;

public:
	void genSyncRequest( CommMsgBody& body ) const;
	void genSyncRequestExPhase1( CommMsgBody& msg ) const;
	void genSyncRequestExPhase2( CommMsgBody& msg ) const;

	void processSyncRequest( CommMsgBody& reply, CommMsgParser& parser ) const;
	void processSyncRequestExPhase1( CommMsgBody& reply, CommMsgParser& parser ) const;
	void processSyncRequestExPhase2( CommMsgBody& reply, CommMsgParser& parser ) const;
	void processSyncResponse( CommMsgParser& parser );
	bool processSyncResponseEx( CommMsgParser& parser );//returns false if 2nd-phase requestEx is necessary
	void processOnlineUpdate( CommMsgParser& parser );
	static void parseMessageOnTheFly( OnTheFlyCallback& callback, CommMsgParser& parser );

public://overridables
	virtual void nodeAppended( const Path& path ) {}
	virtual void leafAppended( const Path& path, const CommMsgBody& body ) {}
	//virtual void nodeInserted( const Path& path, int pos ) {}
	//virtual void leafInserted( const Path& path, int pos, const CommMsgBody& body ) {}
	virtual void leafUpdated( const Path& path, const CommMsgBody& body ) {}
	virtual void leafRemoved( const Path& ) {}
	virtual void nodeRemoved( const Path& ) {}

	virtual void beforeLeafUpdated(const Path& path, const CommMsgBody& oldLeaf, const CommMsgBody& newLeaf) {}
	virtual void beforeLeafRemoved(const Path& path) {}
	virtual void beforeNodeRemoved(const Path& path) {}

	virtual void nodeChanged( const Path& path ) {}//also called with pathLen() == 0 for 'global update'

	virtual void onlineUpdateAttachment( UINT32 msgId, CommMsgBody& ) {}

protected:
	void _setHdr( UINT32 ida, UINT32 idb, UINT32 idc, UINT32 idd );
	/* inline */ const _Node* _findNode( const int* path, int pathLen ) const;
	_Node* _findNodeOnline( CommMsgParser& parser, UINT32 rev, Path& path );
	static void _skipNodeOnline( CommMsgParser& parser );
	static void _saveNode( CommMsgBody& msg, const _Node& node );//recursive
	static void _loadNode( _Node& node, CommMsgParser& parser );//recursive
	void _loadNodeEx( CommSubscription::Path& path, _Node& node, CommMsgParser& parser );//recursive; same as _loadNode but may call callbacks

	void _clearDelHistory();//also resyncs lastGoodDelItemRev

private:
	/* inline */ static int _findChild( const _Node& node, UINT32 id );
	/*inline */  static void _checkChild( const _Node& node );


	static void _genNodeSyncRequest( CommMsgBody& msg, const _Node& node );//recursive
//	static void _processNodeSyncRequest( CommMsgBody& reply, const _Node& node, CommMsgParser& parser );//recursive
	static void _processNodeSyncRequestExPhase1( CommMsgBody& reply, const _Node& node, UINT32 rev );//recursive
	static void _processNodeSyncRequestExPhase2( CommMsgBody& reply, const _Node& node, CommMsgParser& parser );//recursive
	void _genZeroSyncResponse( CommMsgBody& reply, UINT32 reqRootRev ) const;
	static void _skipNodeSyncRequest( CommMsgParser& parser );//recursive
	void _processNodeSyncResponse( _Node& node, CommMsgParser& parser, Path& path );//recursive
	void _processNodeSyncResponseEx( _Node& node, CommMsgParser& parser, Path& path );//recursive

private://safety wrappers
	void _safeNodeAppended( const Path& path );
	void _safeLeafAppended( const Path& path, const CommMsgBody& body );
	//void _safeNodeInserted( const Path& path, int pos );
	//void _safeLeafInserted( const Path& path, int pos, const CommMsgBody& body );
	void _safeLeafUpdated( const Path& path, const CommMsgBody& body );
	void _safeBeforeLeafUpdated( const Path& path, const CommMsgBody& oldLeaf, const CommMsgBody& newLeaf );
	void _safeLeafRemoved( const Path& );
	void _safeBeforeLeafRemoved( const Path& );
	void _safeNodeRemoved( const Path& );
	void _safeBeforeNodeRemoved( const Path& );
	void _safeNodeChanged( const Path& path );
	void _safeOnlineUpdateAttachment( UINT32 msgId, CommMsgBody& );

	void _childToBeDeleted( _Node* node, _Node* child, UINT32 rev );

private://prohibited
	CommSubscription( const CommSubscription& );
	CommSubscription& operator =( const CommSubscription& );
};

#if 0
//****************************************************************************

struct CommSubscriptionMapItem
{
public:
	const char* server;
	const char* serverObject;
	const char* subscrChannel;

public:
	CommSubscriptionMapItem( const char* server_, const char* serverObject_, const char* subscrChannel_ )
	{
		server = server_;
		serverObject = serverObject_;
		subscrChannel = subscrChannel_;
	}
};

struct _CommSubscriptionMapCmp : public binary_function< const CommSubscriptionMapItem&, const CommSubscriptionMapItem&, bool >
{
public:
	bool operator() ( const CommSubscriptionMapItem& a, const CommSubscriptionMapItem& b ) const
	{//comparing subscrChannel first because expecting better selectivity
		int cmp = strcmp( a.subscrChannel, b.subscrChannel );
		if( cmp )
			return cmp < 0;
		cmp = strcmp( a.serverObject, b.serverObject );
		if( cmp )
			return cmp < 0;
		return strcmp( a.server, b.server ) < 0;
	}
};

//Warning: implementation of the CommSubscriptionMap is tricky, designed to avoid creating PString object(s) for each find
//CommSubscriptionMap implementation is similar to PStringMap
template< class T >
class CommSubscriptionMap : private map< CommSubscriptionMapItem, T, _CommSubscriptionMapCmp >
{
private:
	typedef map< CommSubscriptionMapItem, T, _CommSubscriptionMapCmp > _Parent;
	//in fact, CommSubscriptionMapItem should contain char*, but it will create problems with types for find etc.

public:
	typedef _Parent::key_type key_type;
	typedef _Parent::value_type value_type;
	typedef _Parent::iterator iterator;
	typedef _Parent::const_iterator const_iterator;

public:
	CommSubscriptionMap()
	{
	}
	~CommSubscriptionMap()
	{
		_clear();
	}

public:
	pair< iterator, bool > insert( const _Parent::value_type& x )
	{
		char* key1 = _dup( x.first.server );
		char* key2 = _dup( x.first.serverObject );
		char* key3 = _dup( x.first.subscrChannel );
		pair< iterator, bool > ret = _Parent::insert( _Parent::value_type( CommSubscriptionMapItem( key1, key2, key3 ), x.second ) );
		if( !ret.second )
		{
			_free( key3 );
			_free( key2 );
			_free( key1 );
		}
		return ret;
	}
	void erase( iterator pos )
	{
		char* key1 = (char*)(*pos).first.server;//see comment re. _Parent
		char* key2 = (char*)(*pos).first.serverObject;//see comment re. _Parent
		char* key3 = (char*)(*pos).first.subscrChannel;//see comment re. _Parent
		_Parent::erase( pos );
		_free( key3 );
		_free( key2 );
		_free( key1 );
	}
	void clear()
	{
		_clear();
	}

public:
	using _Parent::begin;
	using _Parent::end;
	using _Parent::size;
	using _Parent::empty;

	using _Parent::find;
	using _Parent::lower_bound;
	using _Parent::upper_bound;

private:
	static char* _dup( const char* s )
	{
		return s ? strdup( s ) : 0;
	}
	static void _free( char* s )
	{
		if( s )
			free( s );
	}
	void _clear();

private://prohibited
	CommSubscriptionMap( const CommSubscriptionMap< T >& other );
	CommSubscriptionMap& operator =( const CommSubscriptionMap< T >& other );
};

template< class T >
void CommSubscriptionMap< T >::_clear()
{
	for( iterator iter=begin(); iter != end() ; )
	{
		iterator oldIter = iter;
		++iter;
		erase( oldIter );
	}
}
#endif

#endif
