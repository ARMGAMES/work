#ifndef ppdir_h_included
#define ppdir_h_included

#if defined( P_MAC10X )
#if defined( P_IOS ) || defined( MAC_COCOA )
#include <CoreFoundation/CoreFoundation.h>
#else
	#include <Carbon/Carbon.h>
#endif // defined( P_IOS )
#endif // defined( P_MAC10X )

#include "ppinclude.h"
#include "pperror.h"
#include "ppstring.h"

#if defined( PWIN64 )
#define PWIDE_ONLY
#endif

class PDirectory
{
private:
	const PStringEncoding* pathEnc;

#ifdef PWIN32

	union
	{
#if !defined( PWIDE_ONLY )
		WIN32_FIND_DATAA dataA;
#endif
		WIN32_FIND_DATAW dataW;
	};
	HANDLE handle;
#ifndef PWIDE_ONLY
	bool isNT;
	bool handleIsW;
#endif

#elif defined( P_MAC10X )

#if defined( P_IOS ) || defined( MAC_COCOA )

	class PItemInfo
	{
	public:
		bool isDir;
		time_t modTime;
		time_t createTime;
		PString name;
		UInt64 size;
		PItemInfo();
		PItemInfo( bool dir_, time_t& mod_, time_t& create_, const PString& name_, UInt64 size_ );
		PItemInfo( const PItemInfo& orig );
		PItemInfo& operator=( const PItemInfo& rhs );
		~PItemInfo();
		bool isValid() const { return (name.length() > 0); }
	};
	deque<PItemInfo> dirContents;
	PItemInfo curr;

	void getContents( const char *dir );

#else

	class PItemInfo
	{
	public:
		bool isDir;
		UTCDateTime modTime;
		UTCDateTime createTime;
		CFStringRef name;
		UInt64 size;
		PItemInfo();
		PItemInfo( bool dir_, UTCDateTime& mod_, UTCDateTime& create_, CFStringRef name_, UInt64 size_ );
		PItemInfo( const PItemInfo& orig );
		PItemInfo& operator=( const PItemInfo& rhs );
		~PItemInfo();
		bool isValid() const { return name != NULL; }
	};
	deque<PItemInfo> dirContents;
	PItemInfo curr;

	void getContents( CFURLRef url );

#endif // defined( P_IOS )

	CFStringRef maskPrefix;
	CFStringRef maskExt;

	//	FSIterator fsIter;
	//	FSCatalogInfo currInfo;
	//	HFSUniStr255 currName;
	//	bool _createIter( CFURLRef url );
#endif

#if defined(P_ANDROID) || defined(LINUX)

	PString directoryPath;
	vector<PString> directoryFiles;
	int currentFile;

#endif	// P_ANDROID || LINUX

public:
	PDirectory( const PStringEncoding* pathEnc_ = NULL );

	bool file( const char* file );
	bool first( const char* dir );
	bool firstFile( const char* dir, const char* prefix, const char* ext );
	bool next();
	bool currentFileIsDir() const;
	void currentFileName( PString& fileName ) const;
	bool currentFileModificationTime( struct tm* ret ) const; // prohibited for business logic - returns struct tm in system time
	time_t currentFileModificationTime() const;
	time_t currentFileCreationTime() const;
	UINT64 currentFileSize() const;

	~PDirectory();

public:
	static bool makeDir( const char* path, const PStringEncoding* pathEnc = NULL );
	static bool removeFile( const char* path, const PStringEncoding* pathEnc = NULL );
	static bool removeDir( const char* path, const PStringEncoding* pathEnc = NULL );
	static void copyFile( const char* from, const char* to, const PStringEncoding* enc = NULL );
	static bool renameFile( const char* from, const char* to, const PStringEncoding* enc = NULL );
	static bool fileExists( const char* path, const PStringEncoding* pathEnc = NULL );
	static time_t fileModificationTime( const char* path, const PStringEncoding* pathEnc = NULL );
	static bool makePath( const char* path, bool createPath = true, const PStringEncoding* pathEnc = NULL );

private:
	/* inline */ void _del();
	bool _skipDots();
};

#if defined(P_ANDROID) || defined(LINUX)

void _convertPStringArbitraryToUtf8(const char *src, PStringEncoding const *to, PString &dst);
void _convertPStringUtf8ToArbitrary(const char *src, PStringEncoding const *from, PString &dst);

#endif // P_ANDROID || LINUX

#endif
