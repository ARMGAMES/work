#include "ppdir.h"
#include "ppi18n.h"

#include <memory>
#include <errno.h>

#if defined( PWIN32 )
#if !defined( S_ISDIR )
#define S_ISDIR( mode ) (mode & _S_IFDIR)
#endif
// TODO: P_MAC10X is temporary
#elif defined( P_ANDROID ) || defined( LINUX ) || defined( P_MAC10X )
#include <sys/stat.h>
#endif // P_ANDROID || LINUX || P_MAC10X

template <typename T>
struct PathTraits
{
private:
	// Only char and WCHAR are supported
	PathTraits();
};

#if defined( PWIN32 )
template <>
struct PathTraits<WCHAR>
{
	static size_t size( const WCHAR* str ) { return ::wcslen( str ); }
	static WCHAR* getCwd() { return ::_wgetcwd( NULL, 0 ); }
};
// TODO: P_MAC10X is temporary
#elif defined( P_ANDROID ) || defined( LINUX ) || defined( P_MAC10X )
template <>
struct PathTraits<char>
{
	static size_t size( const char* str ) { return ::strlen( str ); }
	static char* getCwd() { return ::getcwd( NULL, 0 ); }
};
#endif

template<typename T>
struct PathGenerator
{
	PathGenerator()
		: path()
		, end( (const T*)path.ptr() )
		, items()
		, currItem( items.cend() )
		, curr()
	{
	}

	int init( const T* const path_, const size_t size );
	void cleanup()
	{
		path.clear();
		end = (const T*)path.ptr();
		items.clear();
		currItem = items.cend();
		curr.clear();
	}

	const T* next();

private:

	struct Item
	{
		const T* value;
		size_t size;
	};

	typedef std::list<Item> ItemsType;

	static const T SLASH = T('/');
	static const T BACKSLASH = T('\\');
	static const T DOT = T('.');
#if defined( PWIN32 )
	static const T SEPARATOR = BACKSLASH;
#else
	static const T SEPARATOR = SLASH;
#endif

	PBlock path;
	const T* end;
	ItemsType items;
	typename ItemsType::const_iterator currItem;
	PBlock curr;

	PathGenerator( const PathGenerator& );
	PathGenerator& operator=( const PathGenerator& );

#if defined( PWIN32 )
	// On Windows delimiter could be mixed
	static bool isDelim( const T c )
	{
		return SLASH == c || BACKSLASH == c;
	}

	static bool isDrive( const T c )
	{
		return T('A') <= c && c <= T('Z') || T('a') <= c && c <= T('z');
	}

	// TODO: Only local paths are supported on Windows
	static bool isAbsolutePath( const T* const path_, size_t size )
	{
		return  ( size > 0 && isDelim( *path_ ) ) ||
				( size > 2 && isDrive( *path_ ) && T(':') == path_[1] && isDelim( path_[2] ) );
	}
// TODO: P_MAC10X is temporary
#elif defined( P_ANDROID ) || defined( LINUX ) || defined( P_MAC10X )
	static bool isDelim( const T c )
	{
		return SLASH == c;
	}

	static bool isAbsolutePath( const T* const path_, size_t size )
	{
		return size > 0 && isDelim( *path_ );
	}
#else
#warning "The methods `isDelim` and `isAbsolutePath` is not supported on current platform"
#endif

	const T* findOf( const T* path_, std::function<bool( const T )> compFunc ) const
	{
		for( ;path_ < end && compFunc( *path_ ); ++path_ );
		return path_;
	}

	const T* skipDelim( const T* path_ ) const
	{
		return findOf( path_, isDelim );
	}

	const T* findDelim( const T* path_ ) const
	{
		// For gcc >= 4.6: Need to capture `this` to call static method?!
		return findOf( path_, [this]( const T c ) { return !PathGenerator<T>::isDelim( c ); } );
	}

	void splitPath();
	void appendCurrent();
};

// TODO: Only local paths are supported on Windows
template<typename T>
int PathGenerator<T>::init( const T* const path_, const size_t size )
{
	if( nullptr == path_ || 0 == size )
	{
		return (-1);
	}

	cleanup();

	if( !isAbsolutePath( path_, size ) )
	{
		std::unique_ptr<T, decltype( &::free )> cwd( PathTraits<T>::getCwd(), &::free );
		if( nullptr == cwd )
		{
			return errno;
		}

		// All optimizations about efficient memory management done by `PBlock` class,
		// reallocation during next `append` is highly improbable.
		const size_t binCwdSize = PathTraits<T>::size( cwd.get() ) * sizeof( T );
		::memcpy( path.append( binCwdSize ), cwd.get(), binCwdSize );
		T& last = *( (T*)path.append( sizeof( T ) ) );
		last = SEPARATOR;
	}

	const size_t binPathSize = size * sizeof( T );
	::memcpy( path.append( binPathSize ), path_, binPathSize );

	end = (const T*)( path.ptr() + path.size() );

	splitPath();

	// Skip root
	currItem = items.cbegin();
	if( items.cend() == currItem )
	{
		return (-1);
	}

	appendCurrent();

	return 0;
}

// Splitting file system path by delimiter(s) and creating index of file system path elements.
// Later that index will be used to generate (see method `next`) path with different depth.
// Paths which are containing `.` and `..` elements will be normalized.
// For example:
// Path: `C:/Users/username/Documents`
// Index: `C:`, `Users`, `username`, `Documents`
// Generator iterations: `C:/Users`, `C:/Users/username`, `C:/Users/username/Documents`
template<typename T>
void PathGenerator<T>::splitPath()
{
	items.clear();
	const auto* ptr = (const T*)path.ptr();
	while( ptr < end )
	{
		const T* const p = findDelim( ptr );
		Item item = { ptr, size_t( p - ptr ) };
		// Checking if path item is have leading dot (`.`) symbol
		if( item.size > 0 && DOT == *(item.value) )
		{
			// If item is just `..` then previous item should be removed from index
			if( item.size == 2 && DOT == item.value[1] )
			{
				// If index only have root item path left splitting/normalization is done
				if( items.size() == 1 )
				{
					return;
				}
				items.pop_back();
			}
			// If item is just `.` then it should be ignored
			else if( 1 != item.size )
			{
				items.push_back( item );
			}
		}
		// Any regular item (not `.` and `..`) will be added to index.
		// Root item could be empty or contain drive letter.
		// In future root item could contain UNC host and share names `\\host\share`
		// or some special path elements, like `\\?`, `\\?\C:`, etc.
		else
		{
			items.push_back( item );
		}

		ptr = skipDelim( p );
	}
}

template<typename T>
void PathGenerator<T>::appendCurrent()
{
	const Item& item = *currItem;

	const size_t binSize = item.size * sizeof( T );
	auto* ptr = (T*)curr.append( binSize + sizeof( T ) );
	::memcpy( ptr, item.value, binSize );
	ptr[item.size] = 0;

	++currItem;
}

template<typename T>
const T* PathGenerator<T>::next()
{
	if( curr.size() < sizeof( T ) || items.cend() == currItem )
	{
		return nullptr;
	}

	T& last = *( (T*)( curr.ptr() + curr.size() ) - 1 );
	last = SEPARATOR;
	appendCurrent();

	return (const T*)curr.ptr();
}

template<typename T>
static int _tryMakeDir( std::function<int()> mkdir, std::function<int( T& )> stat )
{
	int rc = mkdir();
	if( EEXIST == rc )
	{
		T info = { 0 };
		if( 0 != stat( info ) )
		{
			return (-1);
		}

		if( !S_ISDIR( info.st_mode ) )
		{
			return (-1);
		}
	}
	return rc;
}

// `errno` thread-safety
// For Unix/Linux:
// https://stackoverflow.com/questions/1694164/is-errno-thread-safe#1694170
// http://www.unix.org/whitepapers/reentrant.html
// http://linux.die.net/man/3/errno
// For Windows: https://stackoverflow.com/questions/6413052/msvc-errno-thread-safety
static inline int _getErrno( int rc )
{
	return (-1) == rc ? errno : 0;
}

template<typename T>
static bool _createPath( const T* path, bool createPath, std::function<int( const T* )> makeDir )
{
	if( nullptr == path || T('\0') == *path )
	{
		return false;
	}

	int irc = makeDir( path );
	if( !createPath || ENOENT != irc )
	{
		return 0 == irc;
	}

	PathGenerator<T> generator;
	if( 0 != generator.init( path, PathTraits<T>::size( path ) ) )
	{
		return false;
	}

	const T* p = generator.next();
	if( nullptr == p )
	{
		return false;
	}

	do
	{
		irc = makeDir( p );
		if( 0 != irc && EEXIST != irc )
		{
			return false;
		}
	}
	while( nullptr != ( p = generator.next() ) );

	return true;
}

#if defined( PWIN32 )
PDirectory::PDirectory( const PStringEncoding* pathEnc_ /* = NULL */ )
{
	pathEnc = pathEnc_;
	handle = INVALID_HANDLE_VALUE;
#if !defined( PWIDE_ONLY )
	isNT = _PWinIsUnicode();
	handleIsW = false;
#endif
}

inline void PDirectory::_del()
{
	if( handle != INVALID_HANDLE_VALUE )
	{
		::FindClose( handle );
		handle = INVALID_HANDLE_VALUE;
	}
}

bool PDirectory::_skipDots()
{
#if !defined( PWIDE_ONLY )
	if( !handleIsW )
	{
		while( strcmp( dataA.cFileName, "." ) == 0 ||
			strcmp( dataA.cFileName, ".." ) == 0 )
		{
			BOOL Ok = ::FindNextFileA( handle, &dataA );
			if( !Ok )
				return false;
		}
	}
	else
#endif
	{
		while( wcscmp( dataW.cFileName, L"." ) == 0 ||
			wcscmp( dataW.cFileName, L".." ) == 0 )
		{
			BOOL Ok = ::FindNextFileW( handle, &dataW );
			if( !Ok )
				return false;
		}
	}
	return true;
}

bool PDirectory::file( const char* file )
{
	_del();

#if !defined( PWIDE_ONLY )
	if( pathEnc == NULL )
	{
		handle = ::FindFirstFileA( file, &dataA );
		handleIsW = false;
	}
	else
	{
#endif
		PBlock path;
		_PWin32AppendWStringFromPStringEncoding( path, file, pathEnc );
		_PWin32NullTerminateWString( path );

#if !defined( PWIDE_ONLY )
		if( isNT )
		{
#endif
			handle = ::FindFirstFileW( (const WCHAR*)path.ptr(), &dataW );
#if !defined( PWIDE_ONLY )
			handleIsW = true;
		}
		else
		{
			PBlock aPath;
			_PWin32WStringToA( aPath, (const WCHAR*)path.ptr() );
			handle = ::FindFirstFileA( (const char*)aPath.ptr(), &dataA );
			handleIsW = false;
		}
	}
#endif

	if( handle == INVALID_HANDLE_VALUE )
		return false;

	return _skipDots();
}

bool PDirectory::first( const char* dir )
{
	PString fileMask;
	if( dir && *dir )
	{
		if( pathEnc )
		{
			fileMask.assign( dir );
			pathEnc->append( fileMask, "\\*" );
		}
		else
		{
			fileMask.assign( dir );
			fileMask.append( "\\*" );
		}
	}
	else
	{
		if( pathEnc )
		{
			pathEnc->append( fileMask, "*" );
		}
		else
		{
			fileMask.append( "*" );
		}
	}

	return file( fileMask );
}

bool PDirectory::firstFile( const char* dir, const char* prefix, const char* ext )
{
	PString fileMask;
	if( dir && *dir )
	{
		if( pathEnc )
		{
			fileMask.assign( dir );
			pathEnc->append( fileMask, "\\" );
		}
		else
		{
			fileMask.assign( dir );
			fileMask.append( "\\" );
		}
	}
	if( prefix && *prefix )
	{
		if( pathEnc )
		{
			pathEnc->append( fileMask, prefix );
		}
		else
		{
			fileMask.append( prefix );
		}
	}
	if( ext && *ext )
	{
		if( pathEnc )
		{
			pathEnc->append( fileMask, "*." );
			pathEnc->append( fileMask, ext );
		}
		else
		{
			fileMask.append( "*." );
			fileMask.append( ext );
		}
	}
	else
	{
		if( pathEnc )
		{
			pathEnc->append( fileMask, "*.*" );
		}
		else
		{
			fileMask.append( "*.*" );
		}
	}

	return file( fileMask );
}

bool PDirectory::next()
{
	BOOL Ok;
#if !defined( PWIDE_ONLY )
	if( ! handleIsW )
		Ok = ::FindNextFileA( handle, &dataA );
	else
#endif
		Ok = ::FindNextFileW( handle, &dataW );
	if( !Ok )
		return false;
	return _skipDots();
}

void PDirectory::currentFileName( PString& fileName ) const
{
#if !defined( PWIDE_ONLY )
	if( ! handleIsW )
	{
		if( pathEnc == NULL )
		{
			fileName = dataA.cFileName;
			return;
		}
		else
		{
			PASSERT4( !isNT );
			PBlock wFileName;
			_PWin32AStringToW( wFileName, dataA.cFileName );
			_PWin32WStringToPStringEncoding( fileName, (const WCHAR*)wFileName.ptr(), pathEnc );
		}
	}
	else
#endif
	{
#if !defined( PWIDE_ONLY )
		PASSERT4( pathEnc != NULL );
#endif
		_PWin32WStringToPStringEncoding( fileName, dataW.cFileName, pathEnc );
	}
}

static time_t _PFileTimeToTimet( const FILETIME* pft )
{
	unsigned __int64 tt = ( ((unsigned __int64)pft->dwHighDateTime) << 32 ) | (unsigned __int64)pft->dwLowDateTime;
	if( tt < 116444736000000000 )
		return 0;
	return (time_t) ((tt - 116444736000000000)/10000000);
}

bool PDirectory::currentFileModificationTime( struct tm* tm ) const // prohibited for business logic - returns struct tm in system time
{
#if !defined( PWIDE_ONLY )
	const FILETIME* ft = handleIsW ? &dataW.ftLastWriteTime : &dataA.ftLastWriteTime;
#else
	const FILETIME* ft = &dataW.ftLastWriteTime;
#endif
	if( ft->dwLowDateTime == 0 && ft->dwHighDateTime == 0 )
		return false;
	SYSTEMTIME systime;
	if( !::FileTimeToSystemTime( ft, &systime ) )
		return false;

	_PWin32SystemTimeToStructTm( &systime, tm );
	return true;
}

time_t PDirectory::currentFileModificationTime() const
{
#if !defined( PWIDE_ONLY )
	const FILETIME* ft = handleIsW ? &dataW.ftLastWriteTime : &dataA.ftLastWriteTime;
#else
	const FILETIME* ft = &dataW.ftLastWriteTime;
#endif
	if( ft->dwLowDateTime == 0 && ft->dwHighDateTime == 0 )
		return 0;
	return _PFileTimeToTimet( ft );
}

time_t PDirectory::currentFileCreationTime() const
{
#if !defined( PWIDE_ONLY )
	const FILETIME* ft = handleIsW ? &dataW.ftCreationTime : &dataA.ftCreationTime;
#else
	const FILETIME* ft = &dataW.ftCreationTime;
#endif
	if( ft->dwLowDateTime == 0 && ft->dwHighDateTime == 0 )
		return 0;
	return _PFileTimeToTimet( ft );
}

UINT64 PDirectory::currentFileSize() const
{
#if !defined( PWIDE_ONLY )
	return handleIsW ? ( ((unsigned __int64)dataW.nFileSizeHigh) << 32 ) | (unsigned __int64)dataW.nFileSizeLow : ( ((unsigned __int64)dataA.nFileSizeHigh) << 32 ) | (unsigned __int64)dataA.nFileSizeLow;
#else
	return ( ((unsigned __int64)dataW.nFileSizeHigh) << 32 ) | (unsigned __int64)dataW.nFileSizeLow;
#endif
}

bool PDirectory::currentFileIsDir() const
{
#if !defined( PWIDE_ONLY )
	return handleIsW ? ( dataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 : ( dataA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
#else
	return ( dataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
#endif
}

PDirectory::~PDirectory()
{
	_del();
}

/* static */ bool PDirectory::makeDir( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	int err;
#if !defined( PWIDE_ONLY )
	if( pathEnc == NULL )
		err = ::mkdir( path );
	else
#endif
	{
		PBlock wPath;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wPath, path, pathEnc );
		_PWin32NullTerminateWString( wPath );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aPath;
			_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
			err = ::mkdir( (const char*)aPath.ptr() );
		}
		else
#endif
		{
			err = ::_wmkdir( (const WCHAR*)wPath.ptr() );
		}
	}

	if( err == 0 )
		return true;
	if( errno != EEXIST )
		throw PInternalError( PString( "Unable to create directory: " ).appendUint( errno ) );
	return false;
}

/* static */ bool PDirectory::removeFile( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	int err;
#if !defined( PWIDE_ONLY )
	if( pathEnc == NULL )
		err = ::unlink( path );
	else
#endif
	{
		PBlock wPath;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wPath, path, pathEnc );
		_PWin32NullTerminateWString( wPath );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aPath;
			_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
			err = ::unlink( (const char*)aPath.ptr() );
		}
		else
#endif
		{
			err = ::_wunlink( (const WCHAR*)wPath.ptr() );
		}
	}

	return err == 0;
}

/* static */ bool PDirectory::removeDir( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	int err;
#if !defined( PWIDE_ONLY )
	if( pathEnc == NULL )
		err = ::rmdir( path );
	else
#endif
	{
		PBlock wPath;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wPath, path, pathEnc );
		_PWin32NullTerminateWString( wPath );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aPath;
			_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
			err = ::rmdir( (const char*)aPath.ptr() );
		}
		else
#endif
		{
			err = ::_wrmdir( (const WCHAR*)wPath.ptr() );
		}
	}

	return err == 0;
}

/* static */ void PDirectory::copyFile( const char* from, const char* to, const PStringEncoding* enc /* = NULL */ )
{
	BOOL Ok;
#if !defined( PWIDE_ONLY )
	if( enc == NULL )
	{
		Ok = ::CopyFileA( from, to, FALSE );
	}
	else
#endif
	{
		PBlock wFrom;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wFrom, from, enc );
		_PWin32NullTerminateWString( wFrom );
		PBlock wTo;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wTo, to, enc );
		_PWin32NullTerminateWString( wTo );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aFrom;
			_PWin32WStringToA( aFrom, (const WCHAR*)wFrom.ptr() );
			PBlock aTo;
			_PWin32WStringToA( aTo, (const WCHAR*)wTo.ptr() );
			Ok = ::CopyFileA( (const char*)aFrom.ptr(), (const char*)aTo.ptr(), FALSE );
		}
		else
#endif
		{
			Ok = ::CopyFileW( (const WCHAR*)wFrom.ptr(), (const WCHAR*)wTo.ptr(), FALSE );
		}
	}

	if( !Ok )
	{
		DWORD err = GetLastError();
		PString errStr( "Unable to copy file: '" );
		errStr.append( from );//+++
		errStr.append( "' err=" );
		errStr.appendInt( err );
		throw PInternalError( errStr );
	}
}

/* static */ bool PDirectory::renameFile( const char* from, const char* to, const PStringEncoding* enc /* = NULL */ )
{
	BOOL Ok;
#if !defined( PWIDE_ONLY )
	if( enc == NULL )
		Ok = ::MoveFileA( from, to );
	else
#endif
	{
		PBlock wFrom;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wFrom, from, enc );
		_PWin32NullTerminateWString( wFrom );
		PBlock wTo;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wTo, to, enc );
		_PWin32NullTerminateWString( wTo );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aFrom;
			_PWin32WStringToA( aFrom, (const WCHAR*)wFrom.ptr() );
			PBlock aTo;
			_PWin32WStringToA( aTo, (const WCHAR*)wTo.ptr() );
			Ok = ::MoveFileA( (const char*)aFrom.ptr(), (const char*)aTo.ptr() );
		}
		else
#endif
		{
			Ok = ::MoveFileW( (const WCHAR*)wFrom.ptr(), (const WCHAR*)wTo.ptr() );
		}
	}
	return Ok != 0;
}

/* static */ bool PDirectory::fileExists( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
#if !defined( PWIDE_ONLY )
	if( pathEnc == NULL )
		return ::GetFileAttributesA( path ) != INVALID_FILE_ATTRIBUTES;
	else
#endif
	{
		PBlock wPath;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wPath, path, pathEnc );
		_PWin32NullTerminateWString( wPath );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aPath;
			_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
			return ::GetFileAttributesA( (const char*)aPath.ptr() ) != INVALID_FILE_ATTRIBUTES;
		}
		else
#endif
		{
			return ::GetFileAttributesW( (const WCHAR*)wPath.ptr() ) != INVALID_FILE_ATTRIBUTES;
		}
	}
}

/* static */ time_t PDirectory::fileModificationTime( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
#if !defined( PWIDE_ONLY )
	if( pathEnc == NULL )
	{
		struct _stat stat;
		if( ::_stat( path, &stat ) == 0 )
			return stat.st_mtime;
	}
	else
#endif
	{
		PBlock wPath;//of WCHAR
		_PWin32AppendWStringFromPStringEncoding( wPath, path, pathEnc );
		_PWin32NullTerminateWString( wPath );

#if !defined( PWIDE_ONLY )
		if( ! _PWinIsUnicode() )
		{
			PBlock aPath;
			_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
			struct _stat stat;
			if( ::_stat( (const char*)aPath.ptr(), &stat ) == 0 )
				return stat.st_mtime;
		}
		else
#endif
		{
			struct _stat stat;
			if( ::_wstat( (const WCHAR*)wPath.ptr(), &stat ) == 0 )
				return stat.st_mtime;
		}
	}
	PASSERT4( 0 );
	return 0;
}

static int _osMakeDir( const WCHAR* path )
{
	// Windows specific `_stat` structure
	return _tryMakeDir<struct _stat>(
		[path] { return _getErrno( ::_wmkdir( path ) ); },
		[path]( struct _stat& info ) { return ::_wstat( path, &info ); } );
}

/* static */ bool PDirectory::makePath( const char* path, bool createPath /* = true */, const PStringEncoding* pathEnc /* = NULL */ )
{
	PBlock ePath;
	_PWin32AppendWStringFromPStringEncoding( ePath, path, pathEnc );
	_PWin32NullTerminateWString( ePath );
	return _createPath<WCHAR>( (const WCHAR*)ePath.ptr(), createPath, _osMakeDir );
}

#elif defined(P_MAC10X)

#pragma mark Mac Implementation

#ifndef CFSafeRelease   
#define CFSafeRelease(_cf)                  \
	do                                      \
	{										\
		if ((_cf) != NULL)                  \
		{									\
			CFRelease((CFTypeRef) _cf);     \
			_cf = NULL;                     \
		}									\
	}			                            \
	while (0)
#endif // CFSafeRelease

#ifdef COCOA_TOUCH

#include "mixed.h"

PDirectory::PDirectory( const PStringEncoding* pathEnc_ /* = NULL */ )
: pathEnc( pathEnc_ ), maskPrefix(NULL), maskExt(NULL)//, fsIter(NULL)
{
}

bool PDirectory::next()
{
	printf("next:NI\n");
	return false;
}

inline void PDirectory::_del()
{
	printf("_del:NI\n");
}

PDirectory::~PDirectory()
{
	printf("~PDirectory:NI\n");
	_del();
}

bool PDirectory::firstFile( const char* dir, const char* prefix, const char* ext )
{
	printf("firstFile:NI\n");
	bool result = false;
	return result;
}

/* static */ bool PDirectory::removeFile( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	bool result = false;
	printf("removeFile:NI\n");
	return result;
}

void PDirectory::currentFileName( PString& fileName ) const
{
	printf("currentFileName:NI\n");
}

/* static */ bool PDirectory::fileExists( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	return CTFileExists(path);
}

/* static */ time_t PDirectory::fileModificationTime( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	printf("fileModificationTime:NI\n");
	time_t ret = 0;
	return ret;
}


#else

static bool _isSystemTiger()
{
	static SInt32 _sysVers = 0;
	if( _sysVers == 0 )
		Gestalt( gestaltSystemVersion, &_sysVers );
	return ( _sysVers >= 0x1040 );
}

static CFStringRef _createCFString( const char* s, const PStringEncoding* encString_ = NULL )
{
	CFStringRef str = NULL;
	if( s && *s )
	{
		if( encString_ )
		{
			PStringEncoding::StringParser parser;
			encString_->createNewParser( parser );
			parser.init( s );

			int len = encString_->charLength( s );
			PBlock b;
			b.alloc( len * sizeof(UniChar) );
			UniChar* buf = (UniChar*)b.ptr();
			for( UniCharCount i = 0; i < len; ++i )
			{
				PUNICHAR_RET ch = parser.nextChar();
				PASSERT4( ch > 0 );
				if( 0 < ch )
					*buf++ = ch;
			}
			str = CFStringCreateWithCharacters( kCFAllocatorDefault, (UniChar*)b.ptr(), len );
		}
		else
		{
			str = CFStringCreateWithFileSystemRepresentation( kCFAllocatorDefault, s );
		}
	}
	return str;
}

static CFURLRef _createCFURL( const char* s, const PStringEncoding* encString_ = NULL )
{
	CFStringRef str = _createCFString( s, encString_ );
	CFURLRef url = NULL;
	if( str )
		url = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, str, kCFURLPOSIXPathStyle, false );
	CFSafeRelease( str );
	return url;
}

static bool _createSplitDir( const char* s, const PStringEncoding* enc,
							CFURLRef& parent, CFStringRef& file )
{
	bool result = false;
	CFURLRef url = _createCFURL( s, enc );
	if( url )
	{
		file = CFURLCopyLastPathComponent( url );
		parent = CFURLCreateCopyDeletingLastPathComponent( kCFAllocatorDefault, url );
		if( file && parent )
			result = true;
	}
	CFSafeRelease( url );
	return result;
}

static bool _createSplitDir( const char* s, const PStringEncoding* enc,
							FSRef& parentRef, CFStringRef& file )
{
	bool result = false;
	CFURLRef parent = NULL;
	if( _createSplitDir( s, enc, parent, file ) && CFURLGetFSRef( parent, &parentRef ) )
		result = true;
	CFSafeRelease( parent );
	return result;
}

static bool _createCopyMoveRefs( const char* from, const char* to, const PStringEncoding* enc,
								FSRef& srcRef, FSRef& parentRef, CFStringRef& file )
{
	bool result = false;
	CFURLRef src = _createCFURL( from, enc );
	if( src && CFURLGetFSRef( src, &srcRef ) && _createSplitDir( to, enc, parentRef, file ) )
		result = true;
	CFSafeRelease( src );
	return result;
}

static bool _setTime( const UTCDateTime& dt, struct tm* tm )
{
	CFAbsoluteTime aTime;
	if( noErr == UCConvertUTCDateTimeToCFAbsoluteTime( &dt, &aTime ) )
	{
		CFTimeZoneRef tz = CFTimeZoneCopySystem();
		CFGregorianDate dt = CFAbsoluteTimeGetGregorianDate( aTime, tz );
		tm->tm_year = dt.year - 1900;
		tm->tm_mon = dt.month - 1;
		tm->tm_mday = dt.day;
		tm->tm_hour = dt.hour;
		tm->tm_min = dt.minute;
		tm->tm_sec = (int)dt.second;
		tm->tm_wday = CFAbsoluteTimeGetDayOfWeek( aTime, tz );
		tm->tm_yday = CFAbsoluteTimeGetDayOfYear( aTime, tz );
		tm->tm_isdst = -1;
		CFSafeRelease( tz );
		return true;
	}
	return false;
}

static time_t _getTimeT( const UTCDateTime& dt )
{
	struct tm time;
	if( _setTime( dt, &time ) )
		return mktime( &time );
	else
		return 0;
}

PDirectory::PItemInfo::PItemInfo( bool dir_, UTCDateTime& mod_, UTCDateTime& create_, CFStringRef name_, UInt64 size_ )
: isDir(dir_), name(NULL), size(size_)
{
	modTime.highSeconds = mod_.highSeconds;
	modTime.lowSeconds = mod_.lowSeconds;
	modTime.fraction = mod_.fraction;
	createTime.highSeconds = create_.highSeconds;
	createTime.lowSeconds = create_.lowSeconds;
	createTime.fraction = create_.fraction;
	if( name_ )
		name = CFStringCreateCopy( kCFAllocatorDefault, name_ );
}

PDirectory::PItemInfo::PItemInfo()
: isDir(false), name(NULL), size(0)
{
	modTime.highSeconds = modTime.lowSeconds = modTime.fraction = 0;
	createTime.highSeconds = createTime.lowSeconds = createTime.fraction = 0;
}

PDirectory::PItemInfo::PItemInfo( const PItemInfo& orig )
: isDir(orig.isDir), name(NULL), size(orig.size)
{
	modTime.highSeconds = orig.modTime.highSeconds;
	modTime.lowSeconds = orig.modTime.lowSeconds;
	modTime.fraction = orig.modTime.fraction;
	createTime.highSeconds = orig.createTime.highSeconds;
	createTime.lowSeconds = orig.createTime.lowSeconds;
	createTime.fraction = orig.createTime.fraction;
	if( orig.name )
		name = CFStringCreateCopy( kCFAllocatorDefault, orig.name );
}

PDirectory::PItemInfo::~PItemInfo()
{
	CFSafeRelease( name );
}

PDirectory::PItemInfo& PDirectory::PItemInfo::operator=( const PItemInfo& rhs )
{
	isDir = rhs.isDir;
	modTime.highSeconds = rhs.modTime.highSeconds;
	modTime.lowSeconds = rhs.modTime.lowSeconds;
	modTime.fraction = rhs.modTime.fraction;
	createTime.highSeconds = rhs.createTime.highSeconds;
	createTime.lowSeconds = rhs.createTime.lowSeconds;
	createTime.fraction = rhs.createTime.fraction;
	CFSafeRelease( name );
	if( rhs.name )
		name = CFStringCreateCopy( kCFAllocatorDefault, rhs.name );
	size = rhs.size;
	return *this;
}

PDirectory::PDirectory( const PStringEncoding* pathEnc_ /* = NULL */ )
: pathEnc( pathEnc_ ), maskPrefix(NULL), maskExt(NULL)//, fsIter(NULL)
{
}

void PDirectory::_del() // Clear find
{
	//		if( fsIter )
	//		{
	//			FSCloseIterator( fsIter );
	//			fsIter = NULL;
	//			// Clean curr items
	//		}
	dirContents.clear();
	curr = PItemInfo();
	CFSafeRelease( maskPrefix );
	CFSafeRelease( maskExt );
}

bool PDirectory::_skipDots()
{
	//		if( currName.length && (currName.unicode[0] == '.') )
	//			return next();
	//		else
	return true;
}

//bool PDirectory::_createIter( CFURLRef url )
//	{
//		FSRef ref;
//		if( CFURLGetFSRef( url, &ref ) )
//		{
//			OSStatus err = FSOpenIterator( &ref, kFSIterateFlat, &fsIter );
//			if( noErr == err )
//				return next();
//		}
//		return false;
//	}

void PDirectory::getContents( CFURLRef url )
{
	FSRef ref;
	if( CFURLGetFSRef( url, &ref ) )
	{
		FSIterator iter;
		OSStatus err = FSOpenIterator( &ref, kFSIterateFlat, &iter );
		if( noErr == err )
		{
			FSCatalogInfoBitmap whichInfo = kFSCatInfoNodeFlags |
				kFSCatInfoCreateDate |
				kFSCatInfoContentMod |
				kFSCatInfoDataSizes;
			const UInt32 cnt = 24;
			FSCatalogInfo infos[cnt];
			HFSUniStr255 names[cnt];
			ItemCount items = cnt;
			Boolean didChange; // Ignore for now
			OSStatus err = noErr;
			while( noErr == err )
			{
				err = FSGetCatalogInfoBulk( iter, items, &items, &didChange, whichInfo, infos,
					NULL, NULL, names );
				if( (noErr == err) || (errFSNoMoreItems == err) )
				{
					for( int i = 0; i < items; ++i )
					{
						// Handle mask
						bool use = true;
						CFStringRef name = CFStringCreateWithCharacters( kCFAllocatorDefault,
							names[i].unicode, names[i].length );
						if( maskPrefix )
						{
							CFRange found = CFStringFind( name, maskPrefix, kCFCompareCaseInsensitive );
							if( found.location != 0 )
								use = false;
						}
						if( use && maskExt )
						{
							CFIndex len = CFStringGetLength( name ),
								loc = len - CFStringGetLength( maskExt );
							CFRange found = CFStringFind( name, maskExt, kCFCompareCaseInsensitive | kCFCompareBackwards );
							if( found.location != loc )
								use = false;
						}
						if( use )
						{
							bool dir = ((infos[i].nodeFlags & kFSNodeIsDirectoryMask) != 0);
							PItemInfo p( dir, infos[i].contentModDate, infos[i].createDate,
								name, dir ? 0 : infos[i].dataLogicalSize );
							dirContents.push_back( p );
						}
						CFSafeRelease( name );
					}
				}
			}
			FSCloseIterator( iter );
		}
	}
}

bool firstFile( const char* dir, const char* prefix, const char* ext );

bool PDirectory::first( const char* dir )
{
	bool result = false;
	_del();
	CFURLRef url = _createCFURL( dir, pathEnc );
	if( url )
	{
		getContents( url);
		result = next();// get first
		//			result = _createIter( url );
	}
	CFSafeRelease( url );
	return result;
}

bool PDirectory::file( const char* file )
{
	bool result = false;
	_del();
	CFURLRef url = _createCFURL( file, pathEnc );
	if( url )
	{
		FSRef ref;
		FSCatalogInfoBitmap whichInfo = kFSCatInfoNodeFlags | kFSCatInfoCreateDate |
			kFSCatInfoContentMod | kFSCatInfoDataSizes;
		FSCatalogInfo info;
		HFSUniStr255 name;
		if( CFURLGetFSRef( url, &ref ) &&
			(FSGetCatalogInfo( &ref, whichInfo, &info, &name, NULL, NULL ) == noErr) )
		{
			CFStringRef nameS = CFStringCreateWithCharacters( kCFAllocatorDefault,
				name.unicode, name.length );
			bool dir = ((info.nodeFlags & kFSNodeIsDirectoryMask) != 0);
			curr = PItemInfo( dir, info.contentModDate, info.createDate,
				nameS, dir ? 0 : info.dataLogicalSize );
			CFSafeRelease( nameS );
			result = true;
		}
	}
	CFSafeRelease( url );
	return result;
}

bool PDirectory::firstFile( const char* dir, const char* prefix, const char* ext )
{
	bool result = false;
	_del();
	CFURLRef parent = _createCFURL( dir, pathEnc );
	if( parent )
	{
		CFSafeRelease( maskPrefix );
		maskPrefix = _createCFString( prefix, pathEnc );
		CFSafeRelease( maskExt );
		maskExt = _createCFString( ext, pathEnc );

		getContents( parent );
		result = next(); // get first
		//			result = _createIter( parent );
	}
	CFSafeRelease( parent );
	return result;
}

bool PDirectory::next()
{
	if( dirContents.size() )
	{
		curr = dirContents.front();
		dirContents.pop_front();
		return true;
	}
	else
		return false;
	//		if( fsIter )
	//		{
	//			FSCatalogInfoBitmap whichInfo = kFSCatInfoNodeFlags |
	//											kFSCatInfoCreateDate |
	//											kFSCatInfoContentMod |
	//											kFSCatInfoDataSizes;
	//			ItemCount items = 1;
	//			Boolean didChange;
	//			FSRef ref;
	//			OSStatus err = FSGetCatalogInfoBulk( fsIter, items, &items, &didChange, whichInfo, &currInfo,
	//										&ref, NULL, &currName );
	//			if( noErr == err )
	//				if( _skipDots() )
	//				{
	//					// Handle mask
	//					bool result = true;
	//					CFStringRef name = CFStringCreateWithCharacters( kCFAllocatorDefault,
	//										currName.unicode, currName.length );
	//					if( maskPrefix )
	//					{
	//						CFRange found = CFStringFind( name, maskPrefix, kCFCompareCaseInsensitive );
	//						if( found.location != 0 )
	//						{
	//							result = next();
	//							CFSafeRelease( name );
	//							name = CFStringCreateWithCharacters( kCFAllocatorDefault,
	//											currName.unicode, currName.length );
	//						}
	//					}
	//					if( maskExt )
	//					{
	//						CFIndex len = CFStringGetLength( name ),
	//								loc = len - CFStringGetLength( maskExt );
	//						CFRange found = CFStringFind( name, maskExt, kCFCompareCaseInsensitive | kCFCompareBackwards );
	//						if( found.location != loc )
	//							result = next();
	//					}
	//					CFSafeRelease( name );
	//					return result;
	//				}
	//		}
	return false;
}

void PDirectory::currentFileName( PString& fileName ) const
{
	fileName = 0; // Clear first
	if( curr.isValid() )
	{
		if( pathEnc )
		{
			CFIndex len = CFStringGetLength( curr.name );
			const UniChar* buf = CFStringGetCharactersPtr( curr.name );
			UniChar* uBuf = NULL;
			if( buf == NULL )
			{
				uBuf = (UniChar*)malloc( (len + 1) * sizeof(UniChar) );
				CFStringGetCharacters( curr.name, CFRangeMake(0,len), uBuf );
				uBuf[len] = 0;
				buf = uBuf;
			}
			for( ; len > 0; len-- )
				pathEnc->append( fileName, *buf++ );
			if( uBuf )
				free( uBuf );
		}
		else
		{
			const char* buf = CFStringGetCStringPtr( curr.name, kCFStringEncodingUTF8 );
			char uBuf[256];
			if( (buf == NULL) && CFStringGetCString( curr.name, uBuf, 256, kCFStringEncodingUTF8 ) )
				buf = uBuf;
			if( buf )
				fileName = buf;
		}
	}
}

bool PDirectory::currentFileModificationTime( struct tm* tm ) const // prohibited for business logic - returns struct tm in system time
{
	if( curr.isValid() )
		return _setTime( curr.modTime, tm );
	else
		return false;
}

time_t PDirectory::currentFileModificationTime() const
{
	if( curr.isValid() )
		return _getTimeT( curr.modTime );
	else
		return false;
}

time_t PDirectory::currentFileCreationTime() const
{
	if( curr.isValid() )
		return _getTimeT( curr.createTime );
	else
		return false;
}

UINT64 PDirectory::currentFileSize() const
{
	if( curr.isValid() )
		return curr.size;
	else
		return 0;
}

bool PDirectory::currentFileIsDir() const
{
	if( curr.isValid() )
		return curr.isDir;
	else
		return false;
}

PDirectory::~PDirectory()
{
	_del();
}

/* static */ bool PDirectory::makeDir( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	bool result = false;
	FSRef parent;
	CFStringRef dir = NULL;
	if( _createSplitDir( path, pathEnc, parent, dir ) )
	{
		CFIndex len = CFStringGetLength( dir );
		UniChar* name = (UniChar*)malloc( len * sizeof(UniChar) );
		CFStringGetCharacters( dir, CFRangeMake(0,len), name );
		OSStatus err = FSCreateDirectoryUnicode( &parent, len, name, kFSCatInfoNone,
			NULL, NULL, NULL, NULL );
		free( name );
		if( noErr == err )
			result = true;
	}
	CFSafeRelease( dir );
	return result;
}

/* static */ bool PDirectory::removeFile( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	bool result = false;
	CFURLRef url = _createCFURL( path, pathEnc );
	if( url )
	{
		FSRef ref;
		if( CFURLGetFSRef( url, &ref ) && (noErr == FSDeleteObject( &ref )) )
			result = true;
	}
	CFSafeRelease( url );
	return result;
}

/* static */ bool PDirectory::removeDir( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	return removeFile( path, pathEnc );
}

/* static */ void PDirectory::copyFile( const char* from, const char* to, const PStringEncoding* enc /* = NULL */ )
{
	FSRef srcRef;
	FSRef parentRef;
	CFStringRef file = NULL;
	if( _createCopyMoveRefs( from, to, enc, srcRef, parentRef, file ) )
	{
		OSStatus err = noErr;
		if( _isSystemTiger() )
			err = FSCopyObjectSync( &srcRef, &parentRef, file, NULL, kFSFileOperationOverwrite );
		else
		{
			PASSERT( false );
			//				FSRef newRef;
			//				err = FSMoveObject( &srcRef, &parentRef, &newRef );
			//				if( noErr == err )
			//				{
			//					UniCharCount len = CFStringGetLength( file );
			//					UniChar* name = (UniChar*)malloc( len * sizeof(UniChar) );
			//					CFStringGetCharacters( file, CFRangeMake(0,len), name);
			//					err = FSRenameUnicode( &newRef, len, name, kTextEncodingUnknown, NULL );
			//					free( name );
			//				}
		}
	}
	CFSafeRelease( file );
}

/* static */ bool PDirectory::renameFile( const char* from, const char* to, const PStringEncoding* enc /* = NULL */ )
{
	bool result = false;
	FSRef srcRef;
	FSRef parentRef;
	CFStringRef file = NULL;
	if( _createCopyMoveRefs( from, to, enc, srcRef, parentRef, file ) )
	{
		OSStatus err = noErr;
		if( _isSystemTiger() )
			err = FSMoveObjectSync( &srcRef, &parentRef, file, NULL, kFSFileOperationOverwrite );
		else
		{
			FSRef newRef;
			err = FSMoveObject( &srcRef, &parentRef, &newRef );
			if( noErr == err )
			{
				UniCharCount len = CFStringGetLength( file );
				UniChar* name = (UniChar*)malloc( len * sizeof(UniChar) );
				CFStringGetCharacters( file, CFRangeMake(0,len), name);
				err = FSRenameUnicode( &newRef, len, name, kTextEncodingUnknown, NULL );
				free( name );
			}
		}
		if( noErr == err )
			result = true;
	}
	CFSafeRelease( file );
	return result;
}


/* static */ bool PDirectory::fileExists( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	bool result = false;
	CFURLRef url = _createCFURL( path, pathEnc );
	if( url )
	{
		FSRef ref;
		result = CFURLGetFSRef( url, &ref );
	}
	CFSafeRelease( url );
	return result;
}

#include <sys/stat.h>
/* static */ time_t PDirectory::fileModificationTime( const char* path, const PStringEncoding* pathEnc /* = NULL */ )
{
	CFStringRef pathS = _createCFString( path, pathEnc );
	time_t ret = 0;
	if( pathS )
	{
		char buf[1024];
		if( CFStringGetFileSystemRepresentation( pathS, buf,  1024 ) )
		{
			struct stat stat_;
			if( ::stat( buf, &stat_ ) == 0 )
				ret = stat_.st_mtime;
		}
	}
	CFSafeRelease( pathS );
	return ret;
}

#endif
#endif // COCOA_TOUCH

#if defined(P_ANDROID) || defined(LINUX)

#include <sys/stat.h>
#include <dirent.h>
#include <regex.h>

bool PDirectory::removeDir(const char* path, const PStringEncoding* pathEnc)
{
	PString utf8Path;
	_convertPStringArbitraryToUtf8(path, pathEnc, utf8Path);
	return !rmdir(utf8Path.c_str());
}

bool PDirectory::removeFile(const char* path, const PStringEncoding* pathEnc)
{
	PString utf8Path;
	_convertPStringArbitraryToUtf8(path, pathEnc, utf8Path);
	return !unlink(utf8Path.c_str());
}

bool PDirectory::fileExists(const char* path, const PStringEncoding* pathEnc)
{
	PString utf8Path;
	_convertPStringArbitraryToUtf8(path, pathEnc, utf8Path);
	struct stat st;
	return !stat(utf8Path.c_str(), &st);
}

time_t PDirectory::fileModificationTime(const char* path, const PStringEncoding* pathEnc)
{
	PString utf8Path;
	_convertPStringArbitraryToUtf8(path, pathEnc, utf8Path);
	struct stat st;
	return !stat(utf8Path.c_str(), &st) ? st.st_mtime : 0;
}

bool PDirectory::makeDir( const char* path, const PStringEncoding* pathEnc)
{
	PString utf8Path;
	_convertPStringArbitraryToUtf8(path, pathEnc, utf8Path);
	return !mkdir(utf8Path, S_IRWXU | S_IRWXG | S_IRWXO );
}

bool PDirectory::renameFile( const char* from, const char* to, const PStringEncoding* pathEnc)
{
	PString utf8From;
	_convertPStringArbitraryToUtf8(from, pathEnc, utf8From);
	PString utf8To;
	_convertPStringArbitraryToUtf8(to, pathEnc, utf8To);
	return rename(utf8From, utf8To);
}

// Instance

PDirectory::PDirectory(const PStringEncoding* pathEnc)
{
	this->pathEnc = pathEnc;
	currentFile = -1;
}

bool PDirectory::first(const char* dir)
{
	PString fileMask;
	if (dir && *dir)
	{
		fileMask.append(dir);
		fileMask.append("/*");
	}
	else
	{
		fileMask.append("*");
	}

	return file(fileMask);
}



bool PDirectory::file(const char* path)
{
	directoryPath.cut(0);
	directoryFiles.clear();
	currentFile = -1;

	PString utf8Path;
	_convertPStringArbitraryToUtf8(path, pathEnc, utf8Path);

	const char *delimiter = strrchr(utf8Path, '/');
	if (!delimiter)
	{
		return false;
	}

	directoryPath.assign(utf8Path, delimiter - utf8Path);

	DIR *dp;
	if (!(dp = opendir(directoryPath)))
	{
		return false;
	}

	PString pattern;

	pattern.append('^');

	while (*++delimiter)
	{
		switch (*delimiter)
		{
		case '*':
			pattern.append(".*");
			break;
		case '?':
			pattern.append(".");
			break;
		case '.':
			pattern.append("\\.");
			break;
		default:
			pattern.append(*delimiter);
			break;
		}
	}

	pattern.append('$');

	regex_t re;
	if (regcomp(&re, pattern, REG_NOSUB) != 0)
	{
		closedir(dp);
		return false;
	}

	struct dirent *dirp;
	while ((dirp = readdir(dp)) != NULL)
	{
#if !defined(P_ANDROID) // !P_ANDROID
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
		{
			continue;
		}
#endif // !P_ANDROID
		if (regexec(&re, dirp->d_name, (size_t)0, NULL, 0))
		{
			continue;
		}
		PString encodeFile;
		_convertPStringUtf8ToArbitrary(dirp->d_name, pathEnc, encodeFile);
		directoryFiles.push_back(PString(encodeFile));
	}

	closedir(dp);

	regfree(&re);

	if (!directoryFiles.size())
	{
		return false;
	}

	currentFile = 0;

	return true;
}

bool PDirectory::firstFile(const char* dir, const char* prefix, const char *ext)
{
	PString path;

	path.append(dir);
	path.append("/");
	path.append(prefix);
	path.append(".");
	path.append(ext);

	return file(path);
}

void PDirectory::currentFileName(PString &fileName) const
{

	if (currentFile >= 0)
	{
		fileName = directoryFiles.at(currentFile);
	}
}

bool PDirectory::next()
{

	if (currentFile < 0)
	{
		return false;
	}

	currentFile++;

	if (currentFile < directoryFiles.size())
	{
		return true;
	}

	directoryFiles.clear();
	currentFile = -1;

	return false;
}

UINT64 PDirectory::currentFileSize() const
{
	if (currentFile < 0)
	{
		return 0;
	}

	PString file;
	file.assign(directoryPath);
	file.append('/');
	file.append(directoryFiles.at(currentFile));

	struct stat st;

	if (stat(file, &st))
	{
		return 0;
	}

	return st.st_size;
}

bool PDirectory::currentFileIsDir() const
{
	if (currentFile < 0)
	{
		return 0;
	}

	PString file;
	file.assign(directoryPath);
	file.append('/');
	file.append(directoryFiles.at(currentFile));

	struct stat st;

	if (stat(file, &st))
	{
		return 0;
	}

	return S_ISDIR(st.st_mode);
}

PDirectory::~PDirectory()
{
}

static int _osMakeDir( const char* path )
{
	return _tryMakeDir<struct stat>(
		[path] { return _getErrno( ::mkdir( path, S_IRWXU | S_IRWXG | S_IRWXO ) ); },
		[path]( struct stat& info ) { return ::stat( path, &info ); } );
}

/* static */ bool PDirectory::makePath( const char* path, bool createPath /* = true */, const PStringEncoding* pathEnc /* = NULL */ )
{
	PString ePath;
	_convertPStringArbitraryToUtf8( path, pathEnc, ePath );
	return _createPath<char>( ePath.c_str(), createPath, _osMakeDir );
}
#endif	// P_ANDROID

