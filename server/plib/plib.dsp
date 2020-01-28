# Microsoft Developer Studio Project File - Name="plib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=plib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plib.mak" CFG="plib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "plib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /O2 /I ".\\" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D PASSERTLVL=4 /D PTRACELVL=3 /D "__USE_MALLOC" /D "WIN32" /FD /GF /GF /GF /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "plib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "plib___Win32_FastRelease"
# PROP BASE Intermediate_Dir "plib___Win32_FastRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NDEBUG" /D PDEBUG=4 /Yu"stdafx.h" /FD /GZ /GZ /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D PASSERTLVL=5 /D PTRACELVL=5 /D PASSERTLVL=4 /D "__USE_MALLOC" /FR /FD /GZ /GF /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "NDEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "plib - Win32 Release"
# Name "plib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\zip\adler32.c
# End Source File
# Begin Source File

SOURCE=.\zip\crc32.c
# End Source File
# Begin Source File

SOURCE=.\zip\deflate.c
# End Source File
# Begin Source File

SOURCE=.\ungif\dgif_lib.c
# End Source File
# Begin Source File

SOURCE=.\ungif\gif_err.c
# End Source File
# Begin Source File

SOURCE=.\ungif\gif_hash.c
# End Source File
# Begin Source File

SOURCE=.\ungif\gifalloc.c
# End Source File
# Begin Source File

SOURCE=.\zip\gzio2.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\infblock.c
# End Source File
# Begin Source File

SOURCE=.\zip\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\zip\inffast.c
# End Source File
# Begin Source File

SOURCE=.\zip\inflate.c
# End Source File
# Begin Source File

SOURCE=.\zip\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\zip\infutil.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=.\jpeg\jutils.c
# End Source File
# Begin Source File

SOURCE=.\ppblock.cpp
# End Source File
# Begin Source File

SOURCE=.\ppclient.cpp
# End Source File
# Begin Source File

SOURCE=.\ppconapp.cpp
# End Source File
# Begin Source File

SOURCE=.\ppcontainer.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\ppdeflate.cpp
# End Source File
# Begin Source File

SOURCE=.\ppdir.cpp
# End Source File
# Begin Source File

SOURCE=.\pperror.cpp
# End Source File
# Begin Source File

SOURCE=.\ppfstream.cpp
# End Source File
# Begin Source File

SOURCE=.\ungif\ppgif.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\ppgzip.cpp
# End Source File
# Begin Source File

SOURCE=.\ppi18n_1252.cpp
# End Source File
# Begin Source File

SOURCE=.\ppi18n_ascii.cpp
# End Source File
# Begin Source File

SOURCE=.\ppi18n_codepage.cpp
# End Source File
# Begin Source File

SOURCE=.\ppi18n_stream.cpp
# End Source File
# Begin Source File

SOURCE=.\ppimage.cpp
# End Source File
# Begin Source File

SOURCE=.\ppincludebase.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\ppinflate.cpp
# End Source File
# Begin Source File

SOURCE=.\ppinifile.cpp
# End Source File
# Begin Source File

SOURCE=.\ppinifileex.cpp
# End Source File
# Begin Source File

SOURCE=.\ppiofile.cpp
# End Source File
# Begin Source File

SOURCE=.\ppipc.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\ppjerror.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\ppjpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\ppjpegout.cpp
# End Source File
# Begin Source File

SOURCE=.\pplogfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ppmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\ppmstream.cpp
# End Source File
# Begin Source File

SOURCE=.\ppprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\ppprocesselevated.cpp
# End Source File
# Begin Source File

SOURCE=.\ppprocesskill.cpp
# End Source File
# Begin Source File

SOURCE=.\pprandom.cpp
# End Source File
# Begin Source File

SOURCE=.\ppsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ppsocketc.cpp
# End Source File
# Begin Source File

SOURCE=.\ppsockets.cpp
# End Source File
# Begin Source File

SOURCE=.\ppstring.cpp
# End Source File
# Begin Source File

SOURCE=.\ppstringutf8.cpp
# End Source File
# Begin Source File

SOURCE=.\ppstringw.cpp
# End Source File
# Begin Source File

SOURCE=.\ppstringx.cpp
# End Source File
# Begin Source File

SOURCE=.\ppsysinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ppthread.cpp
# End Source File
# Begin Source File

SOURCE=.\ppunistring.cpp
# End Source File
# Begin Source File

SOURCE=.\ppwinsvc.cpp
# End Source File
# Begin Source File

SOURCE=.\ppwinwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\ppzzip.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\trees.c
# End Source File
# Begin Source File

SOURCE=.\zip\unzip.c
# End Source File
# Begin Source File

SOURCE=.\zip\zip.c
# End Source File
# Begin Source File

SOURCE=.\zip\zip2.cpp
# End Source File
# Begin Source File

SOURCE=.\zip\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\jpeg\jchuff.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdct.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdhuff.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jerror.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jversion.h
# End Source File
# Begin Source File

SOURCE=.\ppblock.h
# End Source File
# Begin Source File

SOURCE=.\ppclient.h
# End Source File
# Begin Source File

SOURCE=.\ppdaemon.h
# End Source File
# Begin Source File

SOURCE=.\ppdir.h
# End Source File
# Begin Source File

SOURCE=.\pperror.h
# End Source File
# Begin Source File

SOURCE=.\ppi18n.h
# End Source File
# Begin Source File

SOURCE=.\ppimage.h
# End Source File
# Begin Source File

SOURCE=.\ppinclude.h
# End Source File
# Begin Source File

SOURCE=.\ppincludebase.h
# End Source File
# Begin Source File

SOURCE=.\ppinifile.h
# End Source File
# Begin Source File

SOURCE=.\ppiofile.h
# End Source File
# Begin Source File

SOURCE=.\ppipc.h
# End Source File
# Begin Source File

SOURCE=.\pplogfile.h
# End Source File
# Begin Source File

SOURCE=.\ppmsg.h
# End Source File
# Begin Source File

SOURCE=.\ppprocess.h
# End Source File
# Begin Source File

SOURCE=.\pprandom.h
# End Source File
# Begin Source File

SOURCE=.\ppsocket.h
# End Source File
# Begin Source File

SOURCE=.\ppstream.h
# End Source File
# Begin Source File

SOURCE=.\ppstring.h
# End Source File
# Begin Source File

SOURCE=.\ppsystem.h
# End Source File
# Begin Source File

SOURCE=.\ppthread.h
# End Source File
# Begin Source File

SOURCE=.\pptime.h
# End Source File
# Begin Source File

SOURCE=.\ppunistring.h
# End Source File
# Begin Source File

SOURCE=.\ppwinwnd.h
# End Source File
# Begin Source File

SOURCE=.\ppzip.h
# End Source File
# End Group
# End Target
# End Project
