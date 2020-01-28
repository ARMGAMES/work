#include "ppinclude.h"
#include "ppstring.h"
#include "commmsgbody.h"
#include "commsubscr.h"

class _CommSubscrDataError : public PInternalError
{
public:
	_CommSubscrDataError( const char* msg )
		: PInternalError( msg )
	{
	}
};

void CommSubscription::_safeNodeAppended( const Path& path )
{
	try
	{
		nodeAppended( path );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in nodeAppended() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: unknown exception in nodeAppended() caught" );
	}
}

void CommSubscription::_safeLeafAppended( const Path& path, const CommMsgBody& body )
{
	try
	{
		leafAppended( path, body );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in leafAppended() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: unknown exception in leafAppended() caught" );
	}
}

/*
void CommSubscription::_safeNodeInserted( const Path& path, int pos )
{
try
{
nodeInserted( path, pos );
}
catch( PError& err )
{
PLog( "CommSubscription: exception in nodeInserted() caught: %s", err.why() );
}
catch( ... )
{
PLog( "CommSubscription: exception in nodeInserted() caught" );
}
}

void CommSubscription::_safeLeafInserted( const Path& path, int pos, const CommMsgBody& body )
{
try
{
leafInserted( path, pos, body );
}
catch( PError& err )
{
PLog( "CommSubscription: exception in leafInserted() caught: %s", err.why() );
}
catch( ... )
{
PLog( "CommSubscription: exception in leafInserted() caught" );
}
}
*/

void CommSubscription::_safeLeafUpdated( const Path& path, const CommMsgBody& body )
{
	try
	{
		leafUpdated( path, body );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in leafUpdated() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: exception in leafUpdated() caught" );
	}
}

void CommSubscription::_safeBeforeLeafUpdated(const Path& path, const CommMsgBody& oldLeaf, const CommMsgBody& newLeaf)
{
	try
	{
		beforeLeafUpdated(path, oldLeaf, newLeaf);
	}
	catch ( PError& err )
	{
		PLog("CommSubscription: exception in beforeLeafUpdated() caught: %s", err.why());
	}
	catch (...)
	{
		PLog("CommSubscription: exception in beforeLeafUpdated() caught");
	}
}

void CommSubscription::_safeLeafRemoved( const Path& path )
{
	try
	{
		leafRemoved( path );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in leafRemoved() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: exception in leafRemoved() caught" );
	}
}

void CommSubscription::_safeBeforeLeafRemoved(const Path& path)
{
	try
	{
		beforeLeafRemoved(path);
	}
	catch (PError& err)
	{
		PLog("CommSubscription: exception in beforeLeafRemoved() caught: %s", err.why());
	}
	catch (...)
	{
		PLog("CommSubscription: exception in beforeLeafRemoved() caught");
	}
}

void CommSubscription::_safeNodeRemoved( const Path& path )
{
	try
	{
		nodeRemoved( path );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in nodeRemoved() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: exception in nodeRemoved() caught" );
	}
}

void CommSubscription::_safeBeforeNodeRemoved(const Path& path)
{
	try
	{
		beforeNodeRemoved(path);
	}
	catch (PError& err)
	{
		PLog("CommSubscription: exception in beforeNodeRemoved() caught: %s", err.why());
	}
	catch (...)
	{
		PLog("CommSubscription: exception in beforeNodeRemoved() caught");
	}
}

void CommSubscription::_safeNodeChanged( const Path& path )
{
	try
	{
		nodeChanged( path );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in nodeChanged() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: exception in nodeChanged() caught" );
	}
}

void CommSubscription::_safeOnlineUpdateAttachment( UINT32 msgId, CommMsgBody& body )
{
	try
	{
		onlineUpdateAttachment( msgId, body );
	}
	catch( PError& err )
	{
		PLog( "CommSubscription: exception in onlineUpdateAttachment() caught: %s", err.why() );
	}
	catch( ... )
	{
		PLog( "CommSubscription: exception in onlineUpdateAttachment() caught" );
	}
}

void CommSubscription::_Node::_clear()
{
	//PASSERT( _heapchk() );
	for( _DelItem* it = firstNodeDelItems; it ; )
	{
		it->node = 0;
		_DelItem* nextIt = it->nextInNode;
		it->nextInNode = it->prevInNode = 0;
		it = nextIt;
	}
	firstNodeDelItems = lastNodeDelItems = 0;
	//PASSERT( _heapchk() );

	int n = static_cast< int >( children.size() );
	for( int i=0; i < n ; ++i )
	{
		_Node* child = children[ i ];
		delete child;
	}
	nItems = 0;
}

CommSubscription::CommSubscription( bool callback_ )
{
	callback = callback_;

	memset( hdr, 0, sizeof(hdr) );
	root.isLeaf = 0x00;
	root.id = 0;
	root.rev = 0;

	percentOfDelItemsToStore = 0;
	subscrDelItems = lastSubscrDelItems = 0;
	nSubscrDelItems = 0;
	lastGoodDelItemRev = root.rev;
}

void CommSubscription::_setHdr( UINT32 ida, UINT32 idb, UINT32 idc, UINT32 idd )
{
	CommMsgBody::writeUINT32( hdr, ida );
	CommMsgBody::writeUINT32( hdr + 4, idb );
	CommMsgBody::writeUINT32( hdr + 8, idc );
	CommMsgBody::writeUINT32( hdr + 12, idd );
}

inline const CommSubscription::_Node* CommSubscription::_findNode( const int* path, int pathLen ) const
{
	const _Node* node = &root;
	for( int i=0; i < pathLen ; ++i )
	{
		PASSERT3( !node->isLeaf );
		PASSERT3( path[ i ] >= 0 && path[ i ] < node->children.size() );
		node = node->children[ path[ i ] ];
	}
	return node;
}

size_t CommSubscription::size( const int* path, int pathLen ) const
{
	const _Node* node = _findNode( path, pathLen );
	PASSERT3( !node->isLeaf );
	return node->children.size();
}

bool CommSubscription::isLeaf( const int* path, int pathLen ) const
{
	const _Node* node = _findNode( path, pathLen );
	return node->isLeaf != 0;
}

const CommMsgBody& CommSubscription::leaf( const int* path, int pathLen ) const
{
	const _Node* node = _findNode( path, pathLen );
	PASSERT3( node->isLeaf );
	return node->leaf;
}

inline /* static */ void CommSubscription::_checkChild( const _Node& node )
{
	int n = static_cast< int >( node.children.size() );

	for( int i=0; i < n-1 ; ++i )
	{
		PASSERT3(node.children[ i ]->id < node.children[ i+1 ]->id);
	}
}

inline /* static */ int CommSubscription::_findChild( const _Node& node, UINT32 id )
{
	PASSERT3( !node.isLeaf );
	
/*	int n = node.children.size();
	for( int i=0; i < n ; ++i )
	{
		const _Node* child = node.children[ i ];
		if( child->id == id )
			return i;
	}
	return -1;
*/
	int n = static_cast< int >( node.children.size() );
	if (!n)
		return -1;
	int rbound = n - 1;
	int lbound = 0;
	while(1)
	{
		if (rbound == lbound)
			return node.children[ rbound ]->id == id ? rbound : -1; 
		int c = lbound + (rbound - lbound)/2;
		if (node.children[ c ]->id == id)
			return c;
		else
		{
			if ( node.children[ c ]->id < id)
			{
				lbound = c + 1;
				if (lbound > rbound)
					lbound = rbound;
			}
			else
			{
				rbound = c - 1;
				if (lbound > rbound)
					rbound = lbound;
			}
		}
	}
	return -1;
}

/* static */ void CommSubscription::_saveNode( CommMsgBody& msg, const _Node& node )//recursive
{
	PASSERT3( !node.isLeaf );
	PASSERT3( node.children.size() < 65535 );
	UINT16 n = (UINT16)node.children.size();
	msg.composeUINT16( n );
	for( int i=0; i < n ; ++i )
	{
		_Node* child = node.children[ i ];
		PASSERT3( child->isLeaf == 0x00 || child->isLeaf == 0xFF );
		if( child->isLeaf )
			msg.composeBYTE( child->isLeaf ).composeUINT32( child->id ).composeUINT32( child->rev ).composeMsgBody( child->leaf );
		else
		{
			msg.composeBYTE( child->isLeaf ).composeUINT32( child->id ).composeUINT32( child->rev );
			_saveNode( msg, *child );
		}
	}
}

/* static */ void CommSubscription::_loadNode( _Node& node, CommMsgParser& parser )//recursive
{
	PASSERT3( !node.isLeaf );
	UINT16 n;
	parser.parseUINT16( n );

	PASSERT3( node.children.size() == 0 );
	PASSERT3( node.nItems == 0 );
	//supposedly no need to deal with delItems - as _loadNode _always_ called for empty nodes only

	node.clear();//just in case
	node.children.reserve( n );
	for( int i=0; i < n ; ++i )
	{
		BYTE _isLeaf;
		UINT32 id, rev;
		parser.parseBYTE( _isLeaf ).parseUINT32( id ).parseUINT32( rev );
		if( _isLeaf != 0x00 && _isLeaf != 0xFF )
			throw PInternalError( "CommSubscription::load(): unexpected isLeaf flag value" );//format error - not a _CommSubscrDataError

		_Node* child = new _Node();
		child->isLeaf = _isLeaf;
		child->id = id;
		child->rev = rev;
		node.children.push_back( child );

		if( _isLeaf )
		{
			parser.parseMsgBody( child->leaf );
			++node.nItems;
		}
		else
		{
			_loadNode( *child, parser );
			node.nItems += child->nItems;
		}
	}
}

void CommSubscription::_loadNodeEx( CommSubscription::Path& path, _Node& node, CommMsgParser& parser )//recursive; same as _loadNode but may call callbacks
{
	PASSERT3( !node.isLeaf );
	UINT16 n;
	parser.parseUINT16( n );

	PASSERT3( node.children.size() == 0 );
	PASSERT3( node.nItems == 0 );
	//supposedly no need to deal with delItems - as _loadNode _always_ called for empty nodes only

	node.clear();//just in case
	node.children.reserve( n );
	for( int i=0; i < n ; ++i )
	{
		BYTE _isLeaf;
		UINT32 id, rev;
		parser.parseBYTE( _isLeaf ).parseUINT32( id ).parseUINT32( rev );
		if( _isLeaf != 0x00 && _isLeaf != 0xFF )
			throw PInternalError( "CommSubscription::load(): unexpected isLeaf flag value" );//format error - not a _CommSubscrDataError

		_Node* child = new _Node();
		child->isLeaf = _isLeaf;
		child->id = id;
		child->rev = rev;
		node.children.push_back( child );

		if( _isLeaf )
		{
			parser.parseMsgBody( child->leaf );
			if( callback )
				_safeLeafAppended( path, child->leaf );
			++node.nItems;
		}
		else
		{
			if( callback )
				_safeNodeAppended( path );

			PASSERT3( path._canIncPathLen() );
			path.path_[ path.pathLen_++ ] = i;
			_loadNodeEx( path, *child, parser );
			--path.pathLen_;

			node.nItems += child->nItems;
		}
	}
}

/* static */ void CommSubscription::_genNodeSyncRequest( CommMsgBody& msg, const _Node& node )//recursive
{
	PASSERT3( !node.isLeaf );
	PASSERT3( node.children.size() < 65535 );
	UINT16 n = (UINT16)node.children.size();
	msg.composeUINT16( n );
	for( int i=0; i < n ; ++i )
	{
		_Node* child = node.children[ i ];
		PASSERT3( child->isLeaf == 0x00 || child->isLeaf == 0xFF );
		msg.composeBYTE( child->isLeaf ).composeUINT32( child->id ).composeUINT32( child->rev );
		if( !child->isLeaf )
			_genNodeSyncRequest( msg, *child );
	}
}

/*
void CommSubscription::_processNodeSyncRequest( CommMsgBody& reply, const _Node& node, CommMsgParser& parser )//recursive
{
	PASSERT3( !node.isLeaf );
	UINT16 n;
	parser.parseUINT16( n );

	PASSERT3( node.children.size() < 65535 );
	UINT16 m = node.children.size();
	int mUsed = 0;

	//usedStatic is an optimization to avoid calling malloc() in 99.99% cases
#define USEDSTATIC_SIZE 16 //16 - why not?
	bool usedStatic[ USEDSTATIC_SIZE ];
	PArrayAutoPtr< bool > usedAllocated( m <= USEDSTATIC_SIZE ? 0 : new bool[ m ] );

	bool* used;
	if( m <= USEDSTATIC_SIZE )
		used = usedStatic;
	else
		used = usedAllocated;

	int i;
	for( i=0; i < m ; ++i )
		used[ i ] = false;

	bool orderChanged = false;
	int lookAt = 0;
	for( i=0; i < n ; ++i )
	{
		UINT32 id, rev;
		BYTE _isLeaf;
		parser.parseBYTE( _isLeaf ).parseUINT32( id ).parseUINT32( rev );
		if( _isLeaf != 0x00 && _isLeaf != 0xFF )
			throw PInternalError( "CommSubscription::processSyncRequest(): unexpected isLeaf flag value" );//format error - not a _CommSubscrDataError

		const _Node* child = 0;
		//int childIdx = _findChild( node, id );
		int childIdx = -1;
		if( lookAt < node.children.size() && node.children[ lookAt ]->id == id )
		{
			childIdx = lookAt;
			++lookAt;
		}
		else
		{
			childIdx = _findChild( node, id );
			if( childIdx >= 0 )
				PLog( "_findChild(!)" );
		}

		if( childIdx >= 0 )
		{
			child = node.children[ childIdx ];
			used[ childIdx ] = true;
			++mUsed;
		}
		else
			orderChanged = true;

		if( _isLeaf )
		{
			if( child != 0 )
			{
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncRequest(): isLeaf flags does not match" );

				if( rev != child->rev )
					reply.composeBYTE( 'u' ).composeUINT32( id ).composeUINT32( child->rev ).composeMsgBody( child->leaf );
			}
		}
		else
		{
			if( child == 0 )
				_skipNodeSyncRequest( parser );
			else
			{
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncRequest(): isLeaf flags does not match" );

				if( rev == child->rev )
					_skipNodeSyncRequest( parser );
				else
				{
					reply.composeBYTE( 'U' ).composeUINT32( id ).composeUINT32( child->rev );
					_processNodeSyncRequest( reply, *child, parser );
				}
			}
		}
	}

	if( orderChanged )
	{
		reply.composeBYTE( 'R' ).composeUINT16( m );
		for( i=0; i < m ; ++i )
			reply.composeUINT32( node.children[ i ]->id );
	}

	int theoryErr = 0;
	int theoryErrCount = 0;
	if( mUsed != m )
	{
		for( i=0; i < m ; ++i )
		{
			if( !used[ i ] )
			{
				if( i < lookAt )
				{
					theoryErr |= 4;
					++theoryErrCount;
				}
				const _Node* child = node.children[ i ];
				if( child->isLeaf )
					reply.composeBYTE( 'a' ).composeUINT32( child->id ).composeUINT32( child->rev ).composeMsgBody( child->leaf );
				else
				{
					reply.composeBYTE( 'A' ).composeUINT32( child->id ).composeUINT32( child->rev );
					_saveNode( reply, *child );
				}
			}
			else
			{
				if( i >= lookAt )
				{
					theoryErr |= 2;
					++theoryErrCount;
				}
			}
		}
	}
	else
	{
		if( lookAt != node.children.size() )
			theoryErr |= 1;
	}

	if( theoryErr )
		PLog( "theoryErr=%d count=%d", theoryErr, theoryErrCount );

	reply.composeBYTE( '0' );
}

*/




/* static */ void CommSubscription::_processNodeSyncRequestExPhase2( CommMsgBody& reply, const _Node& node, CommMsgParser& parser )//recursive
{
	PASSERT3( !node.isLeaf );
	UINT16 n;
	parser.parseUINT16( n );

	PASSERT3( node.children.size() < 65535 );
	UINT16 m = static_cast< UINT16 >( node.children.size() );

	int i;
	int lookAt = 0;
	for( i=0; i < n ; ++i )
	{
		UINT32 id, rev;
		BYTE _isLeaf;
		parser.parseBYTE( _isLeaf ).parseUINT32( id ).parseUINT32( rev );
		if( _isLeaf != 0x00 && _isLeaf != 0xFF )
			throw PInternalError( "CommSubscription::processSyncRequest2(): unexpected isLeaf flag value" );//format error - not a _CommSubscrDataError

		const _Node* child = 0;
		//int childIdx = _findChild( node, id );
		int childIdx = -1;
		if( lookAt < node.children.size() && node.children[ lookAt ]->id == id )
		{
			childIdx = lookAt;
			++lookAt;
		}
		//		else
		//			{
		//			childIdx = _findChild( node, id );
		//			PASSERT3( childIdx < 0 );
		//			}

		if( childIdx >= 0 )
		{
			child = node.children[ childIdx ];
		}
		else
		{
			reply.composeBYTE( _isLeaf ? 'd' : 'D' ).composeUINT32( id );
		}

		if( _isLeaf )
		{
			if( child != 0 )
			{
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncRequest2(): isLeaf flags does not match" );

				if( rev != child->rev )
					reply.composeBYTE( 'u' ).composeUINT32( id ).composeUINT32( child->rev ).composeMsgBody( child->leaf );
			}
		}
		else
		{
			if( child == 0 )
				_skipNodeSyncRequest( parser );
			else
			{
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncRequest2(): isLeaf flags does not match" );

				if( rev == child->rev )
					_skipNodeSyncRequest( parser );
				else
				{
					reply.composeBYTE( 'U' ).composeUINT32( id ).composeUINT32( child->rev );
					_processNodeSyncRequestExPhase2( reply, *child, parser );
				}
			}
		}
	}

	for( i=lookAt; i < m ; ++i )
	{
		const _Node* child = node.children[ i ];
		if( child->isLeaf )
			reply.composeBYTE( 'a' ).composeUINT32( child->id ).composeUINT32( child->rev ).composeMsgBody( child->leaf );
		else
		{
			reply.composeBYTE( 'A' ).composeUINT32( child->id ).composeUINT32( child->rev );
			_saveNode( reply, *child );
		}
	}

	reply.composeBYTE( '0' );
}

/* static */ void CommSubscription::_skipNodeSyncRequest( CommMsgParser& parser )//recursive
{
	UINT16 n;
	parser.parseUINT16( n );

	for( int i=0; i < n ; ++i )
	{
		UINT32 id, rev;
		BYTE _isLeaf;
		parser.parseBYTE( _isLeaf ).parseUINT32( id ).parseUINT32( rev );
		if( _isLeaf != 0x00 && _isLeaf != 0xFF )
			throw PInternalError( "CommSubscription::processSyncRequest(): unexpected isLeaf flag value" );//format error - not a _CommSubscrDataError

		if( !_isLeaf )
			_skipNodeSyncRequest( parser );
	}
}

/* static */ void CommSubscription::_processNodeSyncResponse( _Node& node, CommMsgParser& parser, Path& path )//recursive
{
	bool changed = false;
	for(;;)
	{
		BYTE cmd;
		parser.parseBYTE( cmd );
		switch( cmd )
		{
		case 'D':
			//_processNodeSyncRequest2 only
			{
				UINT32 id;
				parser.parseUINT32( id );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node to be deleted not found" );
				_Node* child = node.children[ childIdx ];
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf expected" );

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
					_safeBeforeNodeRemoved( tmpPath );
				}

				delete child;
				node.children.erase( node.children.begin() + childIdx );

				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = childIdx;
					_safeNodeRemoved( path );
					--path.pathLen_;
				}
			}
			break;
		case 'd':
			//_processNodeSyncRequest2 only
			{
				UINT32 id;
				parser.parseUINT32( id );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf to be deleted not found" );
				_Node* child = node.children[ childIdx ];
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node expected" );

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
					_safeBeforeLeafRemoved( tmpPath );
				}

				delete child;
				node.children.erase( node.children.begin() + childIdx );

				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = childIdx;
					_safeLeafRemoved( path );
					--path.pathLen_;
				}
			}
			break;
		case 'U':
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node to be updated not found" );
				_Node* child = node.children[ childIdx ];
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf cannot be changed to node in update" );
				child->rev = rev;

				PASSERT3( path._canIncPathLen() );
				path.path_[ path.pathLen_++ ] = childIdx;
				_processNodeSyncResponse( *child, parser, path );
				--path.pathLen_;
			}
			break;
		case 'u':
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf to be updated not found" );
				_Node* child = node.children[ childIdx ];
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node cannot be changed to leaf in update" );

				CommMsgBody newLeafBody;
				parser.parseMsgBody(newLeafBody);

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
					_safeBeforeLeafUpdated( tmpPath, child->leaf, newLeafBody );
				}
				
				child->rev = rev;
				child->leaf.moveFrom(newLeafBody);

				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = childIdx;
					_safeLeafUpdated( path, child->leaf );
					--path.pathLen_;
				}
			}
			break;
		case 'R':
			////_processNodeSyncRequest (not ...Request2) only
			{
				changed = true;

				UINT16 n;
				parser.parseUINT16( n );
				vector< _Node* > newChildren;
				newChildren.reserve( n );
				newChildren.insert( newChildren.begin(), n, 0 );

				UINT16 m = static_cast< UINT16 >( node.children.size() );
				int mUsed = 0;
				PArrayAutoPtr< bool > used( new bool[ m ] );

				int i;
				for( i=0; i < m ; ++i )
					used[ i ] = false;

				try
				{
					for( i=0; i < n ; ++i )
					{
						UINT32 id;
						parser.parseUINT32( id );
						int childIdx = _findChild( node, id );
						if( childIdx < 0 )
						{
							_Node* newChild = new _Node();
							newChild->isLeaf = 0xAA;//temporary
							newChild->id = id;
							newChildren[ i ] = newChild;
						}
						else
						{
							newChildren[ i ] = node.children[ childIdx ];
							used[ childIdx ] = true;
							++mUsed;
						}
					}

					newChildren.swap( node.children );//!!! - newChildren.size becomes m, node.children.size becomes n

					if( mUsed != m )
					{
						for( i=0; i < m ; ++i )
							if( !used[ i ] )
								delete newChildren[ i ];
					}
				}
				catch( ... )
				{//deleting temporary nodes on failure
					for( i=0; i < n ; ++i )
					{
						_Node* child = newChildren[ i ];
						if( child->isLeaf == 0xAA )
							delete child;
					}
					throw;
				}
			}
			break;
		case 'A':
			{
				changed = true;

				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				_Node* child = 0;
				if( childIdx >= 0 )
				{
					child = node.children[ childIdx ];
					if( child->isLeaf != 0xAA )
						throw PInternalError( "CommSubscription::processSyncResponse: node to be added is not a new one" );//format error, not a _CommSubscrDataError
				}
				else
				{
					child = new _Node();
					child->id = id;
					node.children.push_back( child );
				}
				child->rev = rev;
				child->isLeaf = 0x00;
				_loadNode( *child, parser );
			}
			break;
		case 'a':
			{
				changed = true;

				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				_Node* child = 0;
				if( childIdx >= 0 )
				{
					child = node.children[ childIdx ];
					if( child->isLeaf != 0xAA )
						throw PInternalError( "CommSubscription::processSyncResponse: leaf to be added is not a new one" );//format error, not a _CommSubscrDataError
				}
				else
				{
					child = new _Node();
					child->id = id;
					node.children.push_back( child );
				}
				child->rev = rev;
				child->isLeaf = 0xFF;
				parser.parseMsgBody( child->leaf );
			}
			break;
		case '0':
			{
				int n = static_cast< int >( node.children.size() );
				int i;
				int nItems = 0;
				for( i=0; i < n ; ++i )
				{
					const _Node* child = node.children[ i ];
					if( child->isLeaf != 0x00 && child->isLeaf != 0xFF )
						throw PInternalError( "CommSubscription::processSyncResponse: new node was not added" );//format error, not a _CommSubscrDataError
					nItems += child->isLeaf ? 1 : child->nItems;
				}
				node.nItems = nItems;
				if( callback && changed )
					_safeNodeChanged( path );
			}
			return;
		default:
			throw PInternalError( "CommSubscription::processSyncResponse: Unknown syncResponse command" );//format error, not a _CommSubscrDataError
		}
	}
}

/* static */ void CommSubscription::_processNodeSyncResponseEx( _Node& node, CommMsgParser& parser, Path& path )//recursive
{
	for(;;)
	{
		BYTE cmd;
		parser.parseBYTE( cmd );
		switch( cmd )
		{
		case 'D':
			//_processNodeSyncRequestEx only
			{
				UINT32 id;
				parser.parseUINT32( id );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node to be deleted not found" );
				_Node* child = node.children[ childIdx ];
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node expected" );

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
					_safeBeforeNodeRemoved( tmpPath );
				}

				delete child;
				node.children.erase( node.children.begin() + childIdx );

				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = childIdx;
					_safeNodeRemoved( path );
					--path.pathLen_;
				}
			}
			break;
		case 'd':
			//_processNodeSyncRequestEx only
			{
				UINT32 id;
				parser.parseUINT32( id );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf to be deleted not found" );
				_Node* child = node.children[ childIdx ];
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf expected" );

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
					_safeBeforeLeafRemoved( tmpPath );
				}

				delete child;
				node.children.erase( node.children.begin() + childIdx );

				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = childIdx;
					_safeLeafRemoved( path );
					--path.pathLen_;
				}
			}
			break;
		case 'U':
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node to be updated not found" );
				_Node* child = node.children[ childIdx ];
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf cannot be changed to node in update" );
				child->rev = rev;

				PASSERT3( path._canIncPathLen() );
				path.path_[ path.pathLen_++ ] = childIdx;
				_processNodeSyncResponseEx( *child, parser, path );
				--path.pathLen_;
			}
			break;
		case 'u':
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				if( childIdx < 0 )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf to be updated not found" );
				_Node* child = node.children[ childIdx ];
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node cannot be changed to leaf in update" );

				CommMsgBody newLeafBody;
				parser.parseMsgBody(newLeafBody);
				
				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
					_safeBeforeLeafUpdated( tmpPath, child->leaf, newLeafBody );
				}
				
				child->rev = rev;
				child->leaf.moveFrom(newLeafBody);

				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = childIdx;
					_safeLeafUpdated( path, child->leaf );
					--path.pathLen_;
				}
			}
			break;
		case 'V'://_processNodeSyncRequestExPhase1 only
			//hybrid of 'U' and 'A'
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				_Node* child = 0;
				bool newNode = false;
				if( childIdx < 0 )
				{
					child = new _Node();
					child->id = id;
					child->isLeaf = 0x00;
					node.children.push_back( child );
					newNode = true;
				}
				else
					child = node.children[ childIdx ];

				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: leaf cannot be changed to node in update" );
				child->rev = rev;

				if( callback && newNode )
				{
					//PASSERT3( path._canIncPathLen() );
					//path.path_[ path.pathLen_++ ] = childIdx;
					_safeNodeAppended( path );
					//--path.pathLen_;
				}

				PASSERT3( path._canIncPathLen() );
				path.path_[ path.pathLen_++ ] = childIdx;
				_processNodeSyncResponseEx( *child, parser, path );
				--path.pathLen_;
			}
			break;
		case 'v'://_processNodeSyncRequestExPhase1 only
			//hybrid of 'u' and 'a'
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				_Node* child = 0;
				bool newLeaf = false;
				if( childIdx < 0 )
				{
					child = new _Node();
					child->id = id;
					child->isLeaf = 0xFF;
					node.children.push_back( child );
					newLeaf = true;
				}
				else
					child = node.children[ childIdx ];
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processSyncResponse: node cannot be changed to leaf in update" );

				CommMsgBody newLeafBody;
				parser.parseMsgBody(newLeafBody);
				
				if( callback )
				{
					if( !newLeaf )
					{
						Path tmpPath = path;
						tmpPath.path_[ tmpPath.pathLen_++ ] = childIdx;
						_safeBeforeLeafUpdated( tmpPath, child->leaf, newLeafBody );
					}
				}
				
				child->rev = rev;
				child->leaf.moveFrom(newLeafBody);

				if( callback )
				{
					if( newLeaf )
					{
						_safeLeafAppended( path, child->leaf );
					}
					else
					{
						PASSERT3( path._canIncPathLen() );
						path.path_[ path.pathLen_++ ] = childIdx;
						_safeLeafUpdated( path, child->leaf );
						--path.pathLen_;
					}
				}
			}
			break;
		case 'A':
			{
				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				PASSERT3( childIdx < 0 );
				_Node* child = new _Node();
				child->id = id;
				node.children.push_back( child );

				child->rev = rev;
				child->isLeaf = 0x00;
				if( callback )
				{
					_safeNodeAppended( path );
				}

				PASSERT3( path._canIncPathLen() );
				path.path_[ path.pathLen_++ ] = childIdx;
				_loadNodeEx( path, *child, parser );
				--path.pathLen_;
			}
			break;
		case 'a':
			{
				//				changed = true;

				UINT32 id, rev;
				parser.parseUINT32( id ).parseUINT32( rev );
				int childIdx = _findChild( node, id );
				PASSERT3( childIdx < 0 );
				_Node* child = new _Node();
				child->id = id;
				node.children.push_back( child );

				child->rev = rev;
				child->isLeaf = 0xFF;
				parser.parseMsgBody( child->leaf );

				if( callback )
				{
					_safeLeafAppended( path, child->leaf );
				}
			}
			break;
		case '0':
			{
				int n = static_cast< int >( node.children.size() );
				int i;
				int nItems = 0;
				for( i=0; i < n ; ++i )
				{
					const _Node* child = node.children[ i ];
					if( child->isLeaf != 0x00 && child->isLeaf != 0xFF )
						throw PInternalError( "CommSubscription::processSyncResponse: new node was not added" );//format error, not a _CommSubscrDataError
					nItems += child->isLeaf ? 1 : child->nItems;
				}
				node.nItems = nItems;
			}
			return;
		default:
			throw PInternalError( "CommSubscription::processSyncResponse: Unknown syncResponse command" );//format error, not a _CommSubscrDataError
		}
	}
}

void CommSubscription::genSyncRequest( CommMsgBody& msg ) const
{
	msg.composeUINT32( root.rev )._composeFixedBlock( hdr, 16 );
	_genNodeSyncRequest( msg, root );
}

void CommSubscription::genSyncRequestExPhase1( CommMsgBody& msg ) const
{
	msg.composeUINT32( root.rev )._composeFixedBlock( hdr, 16 );
}

void CommSubscription::genSyncRequestExPhase2( CommMsgBody& msg ) const
{
	msg.composeUINT32( root.rev )._composeFixedBlock( hdr, 16 );
	_genNodeSyncRequest( msg, root );
}

void CommSubscription::_genZeroSyncResponse( CommMsgBody& reply, UINT32 reqRootRev ) const
{
	reply.composeUINT32( reqRootRev ).composeUINT32( root.rev ).composeBYTE( '0' )._composeFixedBlock( hdr, 16 );
	_saveNode( reply, root );
}

void CommSubscription::processSyncRequest( CommMsgBody& reply, CommMsgParser& parser ) const
{
	const BYTE* reqHdr;
	UINT32 reqRootRev;
	parser.parseUINT32( reqRootRev )._parseFixedBlock( reqHdr, (size_t) 16 );
	if( memcmp( hdr, reqHdr, 16 ) != 0 || reqRootRev > root.rev )
		_genZeroSyncResponse( reply, reqRootRev );
	else
	{
		try
		{
			reply.composeUINT32( reqRootRev ).composeUINT32( root.rev ).composeBYTE( 'U' );
//			_processNodeSyncRequest( reply, root, parser );
			_processNodeSyncRequestExPhase2( reply, root, parser );
		}
		catch( _CommSubscrDataError& err )
		{
			PTRACE3( "Warning: exception in CommSubscription::processSyncRequest() - full update will be sent: '%s'", err.why() );
			reply.clear();
			_genZeroSyncResponse( reply, reqRootRev );
		}
	}
}

void CommSubscription::processSyncRequestExPhase2( CommMsgBody& reply, CommMsgParser& parser ) const
{
	const BYTE* reqHdr;
	UINT32 reqRootRev;
	parser.parseUINT32( reqRootRev )._parseFixedBlock( reqHdr, (size_t) 16 );
	if( memcmp( hdr, reqHdr, 16 ) != 0 || reqRootRev > root.rev )
		_genZeroSyncResponse( reply, reqRootRev );
	else
	{
		try
		{
			reply.composeUINT32( reqRootRev ).composeUINT32( root.rev ).composeBYTE( 'U' );
			_processNodeSyncRequestExPhase2( reply, root, parser );
		}
		catch( _CommSubscrDataError& err )
		{
			PTRACE3( "Warning: exception in CommSubscription::processSyncRequestExPhase2() - full update will be sent: '%s'", err.why() );
			reply.clear();
			_genZeroSyncResponse( reply, reqRootRev );
		}
	}
}

void CommSubscription::processSyncResponse( CommMsgParser& parser )
{
	UINT32 reqRootRev;
	UINT32 newRootRev;
	BYTE cmd;
	parser.parseUINT32( reqRootRev ).parseUINT32( newRootRev ).parseBYTE( cmd );
	if( reqRootRev != root.rev )
		throw _CommSubscrDataError( "CommSubscription: unexpected root revision in syncResponse" );

	root.rev = newRootRev;

	switch( cmd )
	{
	case 'U':
		{
			Path path;
			_clearDelHistory();
			_processNodeSyncResponse( root, parser, path );
			_clearDelHistory();//called 2nd time to resync lastGoodDelItemRev
		}
		break;

	case '0':
		{
			const BYTE* hdr_;
			parser._parseFixedBlock( hdr_, (size_t)16 );
			memcpy( hdr, hdr_, 16 );
			root.clear();
			_clearDelHistory();
			_loadNode( root, parser );
			_clearDelHistory();//called 2nd time to resync lastGoodDelItemRev
			if( callback )
			{
				Path path;
				_safeNodeChanged( path );
			}
		}
		break;

	default:
		throw PInternalError( "CommSubscription::processSyncResponse: Unknown command" );//format error, not a _CommSubscrDataError
	}
}

bool CommSubscription::processSyncResponseEx( CommMsgParser& parser )
{
	UINT32 reqRootRev;
	UINT32 newRootRev;
	BYTE cmd;
	parser.parseUINT32( reqRootRev ).parseUINT32( newRootRev ).parseBYTE( cmd );
	if( reqRootRev != root.rev )
		throw _CommSubscrDataError( "CommSubscription: unexpected root revision in syncResponse" );

	switch( cmd )
	{
	case 'U':
		{
			Path path;
			root.rev = newRootRev;
			_clearDelHistory();
			_processNodeSyncResponseEx( root, parser, path );
			_clearDelHistory();//called 2nd time to resync lastGoodDelItemRev
		}
		break;

	case 'X':
		return false;

	case '0':
		{
			const BYTE* hdr_;
			root.rev = newRootRev;
			parser._parseFixedBlock( hdr_, (size_t)16 );
			memcpy( hdr, hdr_, 16 );
			root.clear();
			_clearDelHistory();
			Path path;
			_loadNodeEx( path, root, parser );
			_clearDelHistory();//called 2nd time to resync lastGoodDelItemRev
		}
		break;

	default:
		throw PInternalError( "CommSubscription::processSyncResponse: Unknown command" );//format error, not a _CommSubscrDataError
	}

	return true;
}

CommSubscription::_Node* CommSubscription::_findNodeOnline( CommMsgParser& parser, UINT32 rev, Path& path )
{
	root.rev = rev;//!!!
	_Node* node = &root;
	UINT16 pathLen;
	parser.parseUINT16( pathLen );

	PASSERT3( path._isValidPathLen( pathLen ) );
	path.pathLen_ = pathLen;

	for( int i=0; i < pathLen ; ++i )
	{
		if( node->isLeaf )
			throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );

		UINT16 idx;
		parser.parseUINT16( idx );
		if( idx >= node->children.size() )
			throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: invalid node index" );
		node = node->children[ idx ];
		node->rev = rev;//!!!

		path.path_[ i ] = idx;
	}
	return node;
}

void CommSubscription::processOnlineUpdate( CommMsgParser& parser )
{
	UINT32 rootRev;
	parser.parseUINT32( rootRev );
	if( rootRev != root.rev )
		throw _CommSubscrDataError( PString( "CommSubscription::processOnlineUpdate: Unexpected root revision, expected " ).appendHexInt( root.rev ).append( ", got " ).appendHexInt( rootRev ) );

	for(;;)
	{
		BYTE cmd;
		parser.parseBYTE( cmd );
		switch( cmd )
		{
		case 'A':
			{
				Path path;
				_Node* node = _findNodeOnline( parser, ++rootRev, path );
				if( node->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );
				_Node* newNode = new _Node();
				newNode->isLeaf = 0x00;
				newNode->id = newNode->rev = rootRev;
				node->children.push_back( newNode );
				if( callback )
					_safeNodeAppended( path );
			}
			break;

			/*
			case 'I':
			{
			Path path;
			_Node* node = _findNodeOnline( parser, ++rootRev, path );
			if( node->isLeaf )
			throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );
			UINT16 pos;
			parser.parseUINT16( pos );
			if( pos >= node->children.size() )
			throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: invalid node index" );
			_Node* newNode = new _Node();
			newNode->isLeaf = 0x00;
			newNode->id = newNode->rev = rootRev;
			node->children.insert( node->children.begin() + pos, newNode );

			if( callback )
			_safeNodeInserted( path, pos );
			}
			break;
			*/
		case 'a':
			{
				Path path;
				_Node* node = _findNodeOnline( parser, ++rootRev, path );
				if( node->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );
				_Node* newNode = new _Node();
				newNode->isLeaf = 0xFF;
				newNode->id = newNode->rev = rootRev;
				node->children.push_back( newNode );
				parser.parseMsgBody( newNode->leaf );
				if( callback )
					_safeLeafAppended( path, newNode->leaf );
			}
			break;
			/*
			case 'i':
			{
			Path path;
			_Node* node = _findNodeOnline( parser, ++rootRev, path );
			if( node->isLeaf )
			throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );
			UINT16 pos;
			parser.parseUINT16( pos );
			if( pos >= node->children.size() )
			throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: invalid node index" );
			_Node* newNode = new _Node();
			newNode->isLeaf = 0xFF;
			newNode->id = newNode->rev = rootRev;
			node->children.insert( node->children.begin() + pos, newNode );
			parser.parseMsgBody( newNode->leaf )

			if( callback )
			_safeLeafInserted( path, pos, newNode->leaf );
			}
			break;
			*/
		case 'u':
			{
				Path path;
				_Node* node = _findNodeOnline( parser, ++rootRev, path );
				if( !node->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Leaf expected" );

				CommMsgBody newLeafBody;
				parser.parseMsgBody(newLeafBody);

				if( callback )
				{
					_safeBeforeLeafUpdated( path, node->leaf, newLeafBody );
				}

				node->leaf.moveFrom(newLeafBody);
				//no node->rev update needed as it is done by _findNode

				if( callback )
					_safeLeafUpdated( path, node->leaf );
			}
			break;

		case 'd':
			{
				Path path;
				_Node* node = _findNodeOnline( parser, ++rootRev, path );
				if( node->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );
				UINT16 pos;
				parser.parseUINT16( pos );
				if( pos >= node->children.size() )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: invalid node index" );
				_Node* child = node->children[ pos ];
				if( !child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Leaf expected" );

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = pos;
					_safeBeforeLeafRemoved( tmpPath );
				}

				_childToBeDeleted( node, child, rootRev );
				delete child;
				node->children.erase( node->children.begin() + pos );
				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = pos;
					_safeLeafRemoved( path );
					//not necessary to --callback->pathLen_;
				}
			}
			break;

		case 'D':
			{
				Path path;
				_Node* node = _findNodeOnline( parser, ++rootRev, path );
				if( node->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );
				UINT16 pos;
				parser.parseUINT16( pos );
				if( pos >= node->children.size() )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: invalid node index" );
				_Node* child = node->children[ pos ];
				if( child->isLeaf )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Node expected" );

				if( callback )
				{
					Path tmpPath = path;
					tmpPath.path_[ tmpPath.pathLen_++ ] = pos;
					_safeBeforeNodeRemoved( tmpPath );
				}

				_childToBeDeleted( node, child, rootRev );
				delete child;
				node->children.erase( node->children.begin() + pos );
				if( callback )
				{
					PASSERT3( path._canIncPathLen() );
					path.path_[ path.pathLen_++ ] = pos;
					_safeNodeRemoved( path );
					//not necessary to --callback->pathLen_;
				}
			}
			break;

		case '!':
			{
				UINT32 attachId;
				CommMsgBody attachment;
				parser.parseUINT32( attachId ).parseMsgBody( attachment );
				if( callback )
					_safeOnlineUpdateAttachment( attachId, attachment );
			}
			break;

		case '0':
			{
				PASSERT3( rootRev == root.rev );
				UINT32 newRootRev;
				parser.parseUINT32( newRootRev );
				if( newRootRev != root.rev )
					throw _CommSubscrDataError( "CommSubscription::processOnlineUpdate: Invalid post-update root revision" );
			}
			return;

		default:
			throw PInternalError( "CommSubscription::processOnlineUpdate: Unknown command" );//format error - not a _CommSubscrDataError
		}
	}
}

/* static */ void CommSubscription::_skipNodeOnline( CommMsgParser& parser )
{
	UINT16 pathLen;
	parser.parseUINT16( pathLen );
	for( int i=0; i < pathLen ; ++i )
	{
		UINT16 idx;
		parser.parseUINT16( idx );
	}
}

/* static */ void CommSubscription::parseMessageOnTheFly( OnTheFlyCallback& callback, CommMsgParser& parser )
{
	UINT32 rootRev;
	parser.parseUINT32( rootRev );//skip

	for(;;)
	{
		BYTE cmd;
		parser.parseBYTE( cmd );
		switch( cmd )
		{
		case 'A':
			_skipNodeOnline( parser );
			break;

			/*
			case 'I':
			{
			_skipNodeOnline( parser );
			UINT16 pos;
			parser.parseUINT16( pos );
			}
			break;
			*/
		case 'a':
			{
				_skipNodeOnline( parser );
				CommMsgBody leaf;
				parser.parseMsgBody( leaf );
			}
			break;
			/*
			case 'i':
			{
			_skipNodeOnline( parser );
			CommMsgBody leaf;
			UINT16 pos;
			parser.parseUINT16( pos ).parseMsgBody( leaf );
			}
			break;
			*/
		case 'u':
			{
				_skipNodeOnline( parser );
				CommMsgBody leaf;
				parser.parseMsgBody( leaf );
			}
			break;

		case 'd':
			{
				_skipNodeOnline( parser );
				UINT16 pos;
				parser.parseUINT16( pos );
			}
			break;

		case 'D':
			{
				_skipNodeOnline( parser );
				UINT16 pos;
				parser.parseUINT16( pos );
			}
			break;

		case '!'://the only meaningful operation here
			{
				UINT32 attachId;
				CommMsgBody attachment;
				parser.parseUINT32( attachId ).parseMsgBody( attachment );
				callback.onlineUpdateAttachment( attachId, attachment );
			}
			break;

		case '0':
			{
				UINT32 newRootRev;
				parser.parseUINT32( newRootRev );
			}
			return;

		default:
			throw PInternalError( "CommSubscription::parseMessageOnTheFly: Unknown command" );//format error - not a _CommSubscrDataError
			break;
		}
	}
}

void CommSubscription::_childToBeDeleted( _Node* node, _Node* child, UINT32 rev )
{
	_DelItem* it = 0;
	if( percentOfDelItemsToStore != 0 )//redundant; just to be 200% sure
	{
		int nAllowed = percentOfDelItemsToStore == 0 ? 0 : root.nItems * percentOfDelItemsToStore / 100;
		while( nSubscrDelItems > nAllowed )
		{//delete
			delete it;//if any

			it = subscrDelItems;
			PASSERT3( subscrDelItems );
			subscrDelItems = it->nextInSubscr;
			if( subscrDelItems == 0 )//deleted last one
				lastSubscrDelItems = 0;
			--nSubscrDelItems;

			_Node* itNode = it->node;
			if( itNode )
			{
				//PASSERT( _heapchk() );
				PASSERT3( !itNode->isLeaf );
				PASSERT3( itNode->firstNodeDelItems );
				PASSERT3( itNode->firstNodeDelItems == it );
				itNode->firstNodeDelItems = it->nextInNode;
				if( itNode->firstNodeDelItems == 0 )//deleted last one
					itNode->lastNodeDelItems = 0;
				else
					itNode->firstNodeDelItems->prevInNode = 0;
				//PASSERT( _heapchk() );
			}

			PASSERT3( lastGoodDelItemRev <= it->rev );
			lastGoodDelItemRev = it->rev;
		}
	}

	if( percentOfDelItemsToStore == 0 )
	{
		delete it;//if any
		PASSERT3( lastGoodDelItemRev <= rev );
		lastGoodDelItemRev = rev;
		return;
	}
	//if percentOfDelItemsToStore != 0, we'll allow at least one item - not too big deal

	if( it == 0 )
		it = new _DelItem();

	it->id = child->id;
	it->rev = rev;
	it->isLeaf = child->isLeaf;
	it->node = node;

	//PASSERT( _heapchk() );

	it->nextInNode = 0;
	if( node->lastNodeDelItems )
	{
		PASSERT3( node->firstNodeDelItems != 0 );
		PASSERT3( node->lastNodeDelItems->nextInNode == 0 );
		node->lastNodeDelItems->nextInNode = it;
		it->prevInNode = node->lastNodeDelItems;
		node->lastNodeDelItems = it;
	}
	else
	{
		PASSERT3( node->firstNodeDelItems == 0 );
		node->firstNodeDelItems = node->lastNodeDelItems = it;
		it->prevInNode = 0;
	}

	it->nextInSubscr = 0;
	if( lastSubscrDelItems )
	{
		PASSERT3( subscrDelItems != 0 );
		lastSubscrDelItems->nextInSubscr = it;
		lastSubscrDelItems = it;
	}
	else
	{
		PASSERT3( subscrDelItems == 0 );
		subscrDelItems = lastSubscrDelItems = it;
	}

	//PASSERT( _heapchk() );
	++nSubscrDelItems;
}

void CommSubscription::_clearDelHistory()
{
	//PASSERT( _heapchk() );
	for( _DelItem* it = subscrDelItems; it ; )
	{
		_DelItem* it1 = it->nextInSubscr;
		if( it->node )
			it->node->firstNodeDelItems = it->node->lastNodeDelItems = 0;
		delete it;
		it = it1;
	}
	//PASSERT( _heapchk() );
	subscrDelItems = 0;
	lastSubscrDelItems = 0;
	nSubscrDelItems=0;
	lastGoodDelItemRev = root.rev;
}

/* static */ void CommSubscription::_processNodeSyncRequestExPhase1( CommMsgBody& reply, const _Node& node, UINT32 rev )//recursive
{
	PASSERT3( !node.isLeaf );
	int n = static_cast< int >( node.children.size() );
	PASSERT3( n < 65535 );
	for( int i=0; i < n ; ++i )
	{
		_Node* child = node.children[ i ];
		PASSERT3( child->isLeaf == 0x00 || child->isLeaf == 0xFF );
		if( child->rev > rev )
		{
			if( child->isLeaf )
			{
				reply.composeBYTE( 'v' ).composeUINT32( child->id ).composeUINT32( child->rev ).composeMsgBody( child->leaf );
			}
			else
			{
				reply.composeBYTE( 'V' ).composeUINT32( child->id ).composeUINT32( child->rev );
				_processNodeSyncRequestExPhase1( reply, *child, rev );
			}
		}
	}

	for( _DelItem* it = node.lastNodeDelItems ; it ; it = it->prevInNode )
	{
		if( it->rev > rev )
		{
			PASSERT3( it->isLeaf == 0x00 || it->isLeaf == 0xFF );
			if( it->id <= rev )//was created before or on rev, so still exists in client subscription and should be deleted
				reply.composeBYTE( it->isLeaf ? 'd' : 'D' ).composeUINT32( it->id );
		}
		else
			break;//for( it )
	}
	reply.composeBYTE('0');
}

void CommSubscription::processSyncRequestExPhase1( CommMsgBody& reply, CommMsgParser& parser ) const
{
	const BYTE* reqHdr;
	UINT32 reqRootRev;
	parser.parseUINT32( reqRootRev )._parseFixedBlock( reqHdr, (size_t) 16 );
	if( memcmp( hdr, reqHdr, 16 ) != 0 || reqRootRev > root.rev )
		_genZeroSyncResponse( reply, reqRootRev );
	else
	{
		if( reqRootRev < lastGoodDelItemRev )
		{
			reply.composeUINT32( reqRootRev ).composeUINT32( root.rev ).composeBYTE( 'X' );
		}
		else
		{
			try
			{
				reply.composeUINT32( reqRootRev ).composeUINT32( root.rev ).composeBYTE( 'U' );
				_processNodeSyncRequestExPhase1( reply, root, reqRootRev );
			}
			catch( _CommSubscrDataError& err )
			{
				PTRACE3( "Warning: exception in CommSubscription::processSyncRequestExPhase1() - full update will be sent: '%s'", err.why() );
				reply.clear();
				_genZeroSyncResponse( reply, reqRootRev );
			}
		}
	}
}
