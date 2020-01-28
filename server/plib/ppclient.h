#ifndef ppclient_h_included
#define ppclient_h_included

#include "ppincludebase.h"
class PStringEncoding;

long POpenFile( const char* path, const PStringEncoding* encString = NULL );
long POpenFolder( const char* path, const PStringEncoding* encString = NULL );
long POpenBrowserWindow( const char* url, const PStringEncoding* encString = NULL );
void PStrCopyToClipboard( const char* str, const PStringEncoding* encString = NULL );

#endif
