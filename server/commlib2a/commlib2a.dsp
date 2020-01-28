# Microsoft Developer Studio Project File - Name="commlib2a" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=commlib2a - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "commlib2a.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "commlib2a.mak" CFG="commlib2a - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "commlib2a - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "commlib2a - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "commlib2a - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\plib" /I "openssl\WIN32VC6" /D "NDEBUG" /D PASSERTLVL=4 /D PTRACELVL=3 /D "WIN32" /D "_MBCS" /D "_LIB" /D "__USE_MALLOC" /FD /GF /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "commlib2a - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "." /I "..\plib" /I "openssl\WIN32VC6" /D "_DEBUG" /D PASSERTLVL=5 /D PTRACELVL=5 /D "WIN32" /D "_MBCS" /D "_LIB" /D "__USE_MALLOC" /FR /FD /GZ /GF /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "commlib2a - Win32 Release"
# Name "commlib2a - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\commappfactory.cpp
# End Source File
# Begin Source File

SOURCE=.\commcgi.cpp
# End Source File
# Begin Source File

SOURCE=.\commcliconn.cpp
# End Source File
# Begin Source File

SOURCE=.\commcligconn.cpp
# End Source File
# Begin Source File

SOURCE=.\commcligconnex.cpp
# End Source File
# Begin Source File

SOURCE=.\commclisubscr.cpp
# End Source File
# Begin Source File

SOURCE=.\commconcli.cpp
# End Source File
# Begin Source File

SOURCE=.\commconn.cpp
# End Source File
# Begin Source File

SOURCE=.\commdispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\commdispatchercommon.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdaesencryptedcli.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdaesencryptedroot.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdaesencryptedsrv.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdnullcli.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdnullsrv.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdcli.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdroot.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdsidcli.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdsidroot.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdsidsrv.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdsrv.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdrsacli.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdrsaroot.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdrsasrv.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdtokencli.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdtokenroot.cpp
# End Source File
# Begin Source File

SOURCE=.\commgrdtokensrv.cpp
# End Source File
# Begin Source File

SOURCE=.\commhtml.cpp
# End Source File
# Begin Source File

SOURCE=.\commhtmlex.cpp
# End Source File
# Begin Source File

SOURCE=.\commhtmlparser.cpp
# End Source File
# Begin Source File

SOURCE=.\commhttp.cpp
# End Source File
# Begin Source File

SOURCE=.\commhttpserver.cpp
# End Source File
# Begin Source File

SOURCE=.\commlocalapp.cpp
# End Source File
# Begin Source File

SOURCE=.\commlocalshared.cpp
# End Source File
# Begin Source File

SOURCE=.\commlocalssl.cpp
# End Source File
# Begin Source File

SOURCE=.\commlocalsslclient.cpp
# End Source File
# Begin Source File

SOURCE=.\commlzhl.cpp
# End Source File
# Begin Source File

SOURCE=.\commmail.cpp
# End Source File
# Begin Source File

SOURCE=.\commmailload.cpp
# End Source File
# Begin Source File

SOURCE=.\commmime.cpp
# End Source File
# Begin Source File

SOURCE=.\commmonitorlogger.cpp
# End Source File
# Begin Source File

SOURCE=.\commmsgbody.cpp
# End Source File
# Begin Source File

SOURCE=.\commphyslocal.cpp
# End Source File
# Begin Source File

SOURCE=.\commpop3.cpp
# End Source File
# Begin Source File

SOURCE=.\commqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\commredundant.cpp
# End Source File
# Begin Source File

SOURCE=.\commrtable.cpp
# End Source File
# Begin Source File

SOURCE=.\commsgmlparser.cpp
# End Source File
# Begin Source File

SOURCE=.\commsharedmem.cpp
# End Source File
# Begin Source File

SOURCE=.\commsmtp.cpp
# End Source File
# Begin Source File

SOURCE=.\commsrvcomposite.cpp
# End Source File
# Begin Source File

SOURCE=.\commsrvconn.cpp
# End Source File
# Begin Source File

SOURCE=.\commsrvobj.cpp
# End Source File
# Begin Source File

SOURCE=.\commsrvsubscr.cpp
# End Source File
# Begin Source File

SOURCE=.\commssl.cpp
# End Source File
# Begin Source File

SOURCE=.\commssl0.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslaes.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslbase64.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslbio.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslc.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslchksum.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslcipher.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslcipher3.cpp
# End Source File
# Begin Source File

SOURCE=.\commssldh.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslentropy.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslentropys.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslentropywinc.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslpasswordhash.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslpasswordhashgenerate.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslprivkey.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslrand.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslrsa.cpp
# End Source File
# Begin Source File

SOURCE=.\commssls.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslsign.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslsysinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslthrow.cpp
# End Source File
# Begin Source File

SOURCE=.\commsslverify.cpp
# End Source File
# Begin Source File

SOURCE=.\commsubscr.cpp
# End Source File
# Begin Source File

SOURCE=.\commtcp0.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmcomposite.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmlm.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmlocal.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmmac.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmrw.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmrwp.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmrwpc.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmrwplain.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmrwssl.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmsimple.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmsimpleplain.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmsimplessl.cpp
# End Source File
# Begin Source File

SOURCE=.\commtmwin.cpp
# End Source File
# Begin Source File

SOURCE=.\commtok822.cpp
# End Source File
# Begin Source File

SOURCE=.\commtthread.cpp
# End Source File
# Begin Source File

SOURCE=.\communix.cpp
# End Source File
# Begin Source File

SOURCE=.\commurl.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhl\Huff.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhl1\huff1.cpp
# End Source File
# Begin Source File

SOURCE=.\i18ncommhtml.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhl\Lz.cpp
# End Source File
# Begin Source File

SOURCE=.\lzhl1\lz1.cpp
# End Source File
# Begin Source File

SOURCE=.\tok822_parse.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_commcomm.h
# End Source File
# Begin Source File

SOURCE=.\_commcomp.h
# End Source File
# Begin Source File

SOURCE=.\_commmime.h
# End Source File
# Begin Source File

SOURCE=.\_commssl.h
# End Source File
# Begin Source File

SOURCE=.\_commtcp0.h
# End Source File
# Begin Source File

SOURCE=.\_commtmlm.h
# End Source File
# Begin Source File

SOURCE=.\_commtmrw.h
# End Source File
# Begin Source File

SOURCE=.\_commtmrwp.h
# End Source File
# Begin Source File

SOURCE=.\_commtmrwpc.h
# End Source File
# Begin Source File

SOURCE=.\_commtmsimple.h
# End Source File
# Begin Source File

SOURCE=.\commappfactory.h
# End Source File
# Begin Source File

SOURCE=.\commappfactoryinterface.h
# End Source File
# Begin Source File

SOURCE=.\commcgi.h
# End Source File
# Begin Source File

SOURCE=.\commcli.h
# End Source File
# Begin Source File

SOURCE=.\commconcli.h
# End Source File
# Begin Source File

SOURCE=.\commconn.h
# End Source File
# Begin Source File

SOURCE=.\commdispatcher.h
# End Source File
# Begin Source File

SOURCE=.\commgrdaesencrypted.h
# End Source File
# Begin Source File

SOURCE=.\commgrdnull.h
# End Source File
# Begin Source File

SOURCE=.\commgrdppwd.h
# End Source File
# Begin Source File

SOURCE=.\commgrdppwdsid.h
# End Source File
# Begin Source File

SOURCE=.\commgrdrsa.h
# End Source File
# Begin Source File

SOURCE=.\commgrdtoken.h
# End Source File
# Begin Source File

SOURCE=.\commhercules.h
# End Source File
# Begin Source File

SOURCE=.\commhtml.h
# End Source File
# Begin Source File

SOURCE=.\commhttp.h
# End Source File
# Begin Source File

SOURCE=.\commidmap.h
# End Source File
# Begin Source File

SOURCE=.\commlib.h
# End Source File
# Begin Source File

SOURCE=.\commlocalapp.h
# End Source File
# Begin Source File

SOURCE=.\commlzhl.h
# End Source File
# Begin Source File

SOURCE=.\commmail.h
# End Source File
# Begin Source File

SOURCE=.\commmonitorlogger.h
# End Source File
# Begin Source File

SOURCE=.\commmsgbody.h
# End Source File
# Begin Source File

SOURCE=.\commpop3.h
# End Source File
# Begin Source File

SOURCE=.\commproto.h
# End Source File
# Begin Source File

SOURCE=.\commqueue.h
# End Source File
# Begin Source File

SOURCE=.\commredundant.h
# End Source File
# Begin Source File

SOURCE=.\commrtable.h
# End Source File
# Begin Source File

SOURCE=.\commsgml.h
# End Source File
# Begin Source File

SOURCE=.\commsharedmem.h
# End Source File
# Begin Source File

SOURCE=.\commsmtp.h
# End Source File
# Begin Source File

SOURCE=.\commsrv.h
# End Source File
# Begin Source File

SOURCE=.\commsrvcomposite.h
# End Source File
# Begin Source File

SOURCE=.\commssl.h
# End Source File
# Begin Source File

SOURCE=.\commsubscr.h
# End Source File
# Begin Source File

SOURCE=.\commtime.h
# End Source File
# Begin Source File

SOURCE=.\commtmcomposite.h
# End Source File
# Begin Source File

SOURCE=.\commtmlm.h
# End Source File
# Begin Source File

SOURCE=.\commtmlocal.h
# End Source File
# Begin Source File

SOURCE=.\commtmmac.h
# End Source File
# Begin Source File

SOURCE=.\commtmrw.h
# End Source File
# Begin Source File

SOURCE=.\commtmsimple.h
# End Source File
# Begin Source File

SOURCE=.\commtmwin.h
# End Source File
# Begin Source File

SOURCE=.\commtok822.h
# End Source File
# Begin Source File

SOURCE=.\commtthread.h
# End Source File
# Begin Source File

SOURCE=.\commurl.h
# End Source File
# Begin Source File

SOURCE=.\tok822.h
# End Source File
# End Group
# End Target
# End Project
