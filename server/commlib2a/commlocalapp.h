#ifndef commlocalapp_h_included
#define commlocalapp_h_included

#include "commsrv.h"

int CommLocalAppMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0 );
int CommLocalAppSharedMemMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0 );
int CommLocalAppSharedMemMainElevated( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0, const PStringEncoding* fileNameEncoding = 0 );
int CommMultiLocalAppMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0 );
int CommMultiLocalAppSharedMemMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0 );
int CommLocalTcpAppMain( int argc, char** argv );
int CommLocalTcpAppClientMain( int argc, char** argv );

#ifdef PPOSIX
int CommLocalAppUnixMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0 );
int CommMultiLocalAppUnixMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg = 0 );
#endif

#endif
