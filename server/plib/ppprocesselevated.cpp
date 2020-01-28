#include "ppinclude.h"
#include "pperror.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppprocess.h"
#include "ppi18n_utf8.h"

#ifdef PWIN32

#include "ppwinwnd.h"

void PProcess::startElevated( const WCHAR* exeName, int nParms, const char* const* parms, bool forceElevated )
{
	unsigned short globalCounterCopy;
	{
		PLock lock(cs);
		globalCounterCopy=++globalCounter;
	}
	char eventName[256];
	sprintf( eventName,"pyrsftevnt%08lX-%04X-%08lX", ::GetCurrentProcessId(), globalCounterCopy, ::GetTickCount() );
	event.create( false, eventName, true );
	PString cmdLine;
	cmdLine.append( '\"' ).append( eventName ).append( '\"' );
	for( int i=0; i < nParms ; ++i )
		cmdLine.append( " \"" ).append( parms[ i ] ).append( '\"' );

#ifndef PWIDE_ONLY
	bool useShellEx = _PWinIsUnicode();
#else
	bool useShellEx = true;
#endif

	if( useShellEx )
	{
		OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
		if( ::GetVersionEx( &osvi ) )
			if( osvi.dwPlatformId != VER_PLATFORM_WIN32_NT || osvi.dwMajorVersion < 6  )
				useShellEx = false;
	}
	PLog( "useShellEx = %d", useShellEx ? 1 : 0 );

	if( useShellEx )
	{
		typedef BOOL (WINAPI *SHELLEXECUTEEXW)(LPSHELLEXECUTEINFOW);
		if( PAutoProcPtr< SHELLEXECUTEEXW > _ShellExecuteExW = PAutoProcPtr< SHELLEXECUTEEXW >( "ShellExecuteExW", "shell32" ) )
		{
			PBlock wCmdLine;
			_PWin32AStringToW( wCmdLine, cmdLine );

			SHELLEXECUTEINFOW sei;
			memset( &sei, 0, sizeof(sei) );
			sei.cbSize = sizeof(sei);
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			sei.lpFile = exeName;
			sei.lpParameters = (WCHAR*)wCmdLine.ptr();
			sei.nShow = SW_HIDE;

			if( forceElevated )
				sei.lpVerb = L"runas";

			if( !_ShellExecuteExW( &sei ) )
			{
				DWORD err = ::GetLastError();
				event.close();
				throw PSystemError( "ShellExecuteEx error", err );
			}

			handle = sei.hProcess;
		}
		else
		{
			useShellEx = false;
		}
	}

	if( !useShellEx )
	{
		const PStringEncodingUtf8 utf8StrEnc;

		PString buf;
		utf8StrEnc.append( buf, '"' );
		_PWin32WStringToPStringEncoding( buf, exeName, &utf8StrEnc );
		utf8StrEnc.append( buf, '"' );
		utf8StrEnc.append( buf, ' ' );
		utf8StrEnc.append( buf, cmdLine );

		PROCESS_INFORMATION pi;
		if( !_PWin32CreateProcess( NULL, buf, NULL, &utf8StrEnc, &pi ) )
		{
			DWORD err = ::GetLastError();
			event.close();
			throw PSystemError( "CreateProcess error", err );
		}

		::CloseHandle( pi.hThread );
		handle = pi.hProcess;
	}
}

#elif defined ( P_MAC10X )

#include <Security/Security.h>
#include <signal.h>
#include <sys/stat.h>

void PProcess::startElevated( const char* exeName, int nParms, const char* const* parms )
{
	// Check for executable is owned by root and has setuid
	bool rootExe = false;
	struct stat st;
	if( ::stat( exeName, &st ) == 0 )
	{
		if( (st.st_uid == 0) && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH | S_ISUID) != 0) )
			rootExe = true;
	}
	//	UniString_Mac path( exeName, &i18n_str_enc );
	//	CFURLRef url = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, path,
	//												kCFURLPOSIXPathStyle, false );
	//	FSRef ref;
	//	if( CFURLGetFSRef( url, &ref ) )
	//	{
	//		FSCatalogInfo info;
	//		err = FSGetCatalogInfo( &ref, kFSCatInfoPermissions, &info, NULL, NULL, NULL );
	//		if( noErr == err )
	//		{
	//			FSPermissionInfo *perms = (FSPermissionInfo*)&info.permissions;
	//			UInt16 exeMode = S_IXUSR | S_IXGRP | S_IXOTH;
	//			if( (perms->mode & exeMode) != 0 ) // Not executable
	//				rootExe;
	//			else if( rExe )
	//			{
	//				if( perms->userID != 0 ) // root
	//					fix = true;
	//				else if( (perms->mode & S_ISUID) == 0 ) // Not setuid
	//					fix = true;
	//			}
	//			CFSafeRelease( perms->fileSec );
	//		}
	//	}
	//	CFSafeRelease( url );

	if( rootExe )
	{
		if( !(processPid = ::fork()) )
		{
			const char* msg = parms[nParms - 1];
			char* envName = "PokerStars.msg.value";
			setenv( envName, msg );
			char* argv[nParms+2];
			argv[0] = (char*)exeName;
			for( int i = 1; i < nParms; ++i )
				argv[i] = (char*)parms[i - 1];
			argv[nParms] = envName;
			argv[nParms + 1] = NULL;
			if( ::execv( exeName, argv ) == -1 )
				::exit( -1 );
			//			unsetenv( envName );
		}
	}
	else // If not, fall back to Auth services
	{
		AuthorizationRef authRef;
		OSStatus err = AuthorizationCreate( NULL, kAuthorizationEmptyEnvironment,
			kAuthorizationFlagDefaults,
			&authRef );
		if( errAuthorizationSuccess == err )
		{
			const char* prompt = "Enter your computer Administrator username/password to complete update.\n\n"
				"Note: Do NOT use your Stars Account id and password.\n\n";
			AuthorizationItem envItems = { kAuthorizationEnvironmentPrompt, strlen(prompt), (void*)prompt, 0 };
			AuthorizationEnvironment env = { 1, &envItems };
			AuthorizationItem rightItems = { kAuthorizationRightExecute, 0, NULL, 0 };
			AuthorizationRights rights = { 1, &rightItems };
			AuthorizationFlags flags = kAuthorizationFlagDefaults |
				kAuthorizationFlagInteractionAllowed |
				kAuthorizationFlagPreAuthorize |
				kAuthorizationFlagExtendRights;
			err = AuthorizationCopyRights( authRef, &rights, &env, flags, NULL );

			if( errAuthorizationSuccess == err )
			{
				const char* msg = parms[nParms - 1];
				char* envName = "PokerStars.msg.value";
				setenv( envName, msg );
				char* argv[nParms+1];
				for( int i = 0; i < nParms - 1; ++i )
					argv[i] = (char*)parms[i];
				argv[nParms - 1] = envName;
				argv[nParms] = NULL;

				err = AuthorizationExecuteWithPrivileges( authRef, exeName, kAuthorizationFlagDefaults,
					argv, NULL );
				if( errAuthorizationSuccess != err )
				{
					switch( err )
					{
					case errAuthorizationDenied: // User failed to authenticate
						break;
					case errAuthorizationCanceled: // User cancelled authentication
						break;
					}
					AuthorizationFree( authRef, kAuthorizationFlagDefaults );
					PLog( "AuthorizationExecuteWithPrivileges error -> %lu\n", err );
					throw PSystemError( "AuthorizationExecuteWithPrivileges error", err );
				}
				//				unsetenv( envName );
			}
			else if( errAuthorizationCanceled == err )
			{
				AuthorizationFree( authRef, kAuthorizationFlagDefaults );
				PLog( "AuthorizationCopyRights error -> %lu\n", err );
				throw PSystemError( "AuthorizationCopyRights error", err );
			}
			AuthorizationFree( authRef, kAuthorizationFlagDefaults );
		}
		else
		{
			PLog( "AuthorizationCreate error -> %lu\n", err );
			throw PSystemError( "AuthorizationCreate error", err );
		}
	}
}

#elif defined ( PPOSIX )

void PProcess::startElevated( const char* exeName, int nParms, const char* const* parms )
{
	if (!(processPid = ::fork()))
	{
		char * * argv = new char * [nParms + 2];
		argv[0]=(char *) exeName;
		for( int i=1; i <= nParms ; ++i )
		{
			argv[i] = (char *)parms[i-1];
		}
		argv[nParms+1]=0;
		if (::execv(exeName,argv) == -1)
		{
			::exit(-1);
		}
		delete [] argv;
	}
}

#endif
