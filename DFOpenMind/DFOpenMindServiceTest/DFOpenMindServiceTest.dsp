# Microsoft Developer Studio Project File - Name="DFOpenMindServiceTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DFOpenMindServiceTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DFOpenMindServiceTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DFOpenMindServiceTest.mak" CFG="DFOpenMindServiceTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DFOpenMindServiceTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DFOpenMindServiceTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DFOpenMindServiceTest - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MFC_USED" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "DFOpenMindServiceTest - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MFC_USED" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DFOpenMindServiceTest - Win32 Release"
# Name "DFOpenMindServiceTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DFDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\DFDRQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\DFFileDB.cpp
# End Source File
# Begin Source File

SOURCE=.\DFMapPtrToPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindCore.cpp
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindServiceTest.cpp
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindServiceTest.rc
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindServiceTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DFServerSock.cpp
# End Source File
# Begin Source File

SOURCE=.\DFTimeChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\DFUserDB.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DFCmn.h
# End Source File
# Begin Source File

SOURCE=.\DFDispatcher.h
# End Source File
# Begin Source File

SOURCE=.\DFDRQueue.h
# End Source File
# Begin Source File

SOURCE=.\DFFileDB.h
# End Source File
# Begin Source File

SOURCE=.\DFIOCP.h
# End Source File
# Begin Source File

SOURCE=.\DFMapPtrToPtr.h
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindCore.h
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindServiceTest.h
# End Source File
# Begin Source File

SOURCE=.\DFOpenMindServiceTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\DFServerSock.h
# End Source File
# Begin Source File

SOURCE=.\DFTimeChecker.h
# End Source File
# Begin Source File

SOURCE=.\DFUserDB.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DFOpenMindServiceTest.ico
# End Source File
# Begin Source File

SOURCE=.\res\DFOpenMindServiceTest.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
