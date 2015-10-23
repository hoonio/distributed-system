# Microsoft Developer Studio Project File - Name="P2p" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=P2p - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "P2p.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "P2p.mak" CFG="P2p - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "P2p - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "P2p - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "P2p - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/P2peer.exe"

!ELSEIF  "$(CFG)" == "P2p - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/P2peer.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "P2p - Win32 Release"
# Name "P2p - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConnectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataDownload.cpp
# End Source File
# Begin Source File

SOURCE=.\DataServerLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\DownloadsView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MMStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MsnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MyIOCPSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MyTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\P2p.cpp
# End Source File
# Begin Source File

SOURCE=.\P2p.rc
# End Source File
# Begin Source File

SOURCE=.\P2pDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\P2pView.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesTabConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesTabDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesTabDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\PreferencesValue.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchView.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TaskbarNotifier.cpp
# End Source File
# Begin Source File

SOURCE=.\TrayIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\UploadsView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConnectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataDownload.h
# End Source File
# Begin Source File

SOURCE=.\DataServerLibrary.h
# End Source File
# Begin Source File

SOURCE=.\DownloadsView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MMStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\MsnDlg.h
# End Source File
# Begin Source File

SOURCE=.\MyIOCPSocket.h
# End Source File
# Begin Source File

SOURCE=.\MyListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MyTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\P2p.h
# End Source File
# Begin Source File

SOURCE=.\P2pDoc.h
# End Source File
# Begin Source File

SOURCE=.\P2pView.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesDlg.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesTabConnect.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesTabDirectory.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesTabDisplay.h
# End Source File
# Begin Source File

SOURCE=.\PreferencesValue.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchView.h
# End Source File
# Begin Source File

SOURCE=.\ServerView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskbarNotifier.h
# End Source File
# Begin Source File

SOURCE=.\TrayIcon.h
# End Source File
# Begin Source File

SOURCE=.\UploadsView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ADLSearch.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chat.ico
# End Source File
# Begin Source File

SOURCE=.\res\Connect.ico
# End Source File
# Begin Source File

SOURCE=.\res\connect_title.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ConnectedHighHigh.ico
# End Source File
# Begin Source File

SOURCE=.\res\ConnectedNotNot.ico
# End Source File
# Begin Source File

SOURCE=.\res\connectListIcon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Contact0.ico
# End Source File
# Begin Source File

SOURCE=.\res\Contact4.ico
# End Source File
# Begin Source File

SOURCE=.\res\Disconnect.ico
# End Source File
# Begin Source File

SOURCE=.\res\Download.ico
# End Source File
# Begin Source File

SOURCE=.\res\FinishedDL.ico
# End Source File
# Begin Source File

SOURCE=.\res\FinishedUL.ico
# End Source File
# Begin Source File

SOURCE=.\res\Friends1.ico
# End Source File
# Begin Source File

SOURCE=.\res\HardDisk.ico
# End Source File
# Begin Source File

SOURCE=.\res\message_title.bmp
# End Source File
# Begin Source File

SOURCE=.\res\P2p.ico
# End Source File
# Begin Source File

SOURCE=.\res\P2p.rc2
# End Source File
# Begin Source File

SOURCE=.\res\P2p_tray.ico
# End Source File
# Begin Source File

SOURCE=.\res\P2pDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\p2pToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\p2pToolbar2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\p2pToolbar_1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\p2pToolbar_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\p2pToolbar_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\p2pToolbar_Disconnect.bmp
# End Source File
# Begin Source File

SOURCE=.\res\preference_icon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Preferences_connect.ico
# End Source File
# Begin Source File

SOURCE=.\res\Preferences_directory.ico
# End Source File
# Begin Source File

SOURCE=.\res\Preferences_Display.ico
# End Source File
# Begin Source File

SOURCE=.\res\Preferences_general.ico
# End Source File
# Begin Source File

SOURCE=.\res\Search.ico
# End Source File
# Begin Source File

SOURCE=.\res\searchTabIcon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\serverCount.ico
# End Source File
# Begin Source File

SOURCE=.\res\ServerInfo.ico
# End Source File
# Begin Source File

SOURCE=.\res\skinmsn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TrayConnected.ico
# End Source File
# Begin Source File

SOURCE=.\res\TrayDisconnected.ico
# End Source File
# Begin Source File

SOURCE=.\res\Upload.ico
# End Source File
# Begin Source File

SOURCE=.\res\User.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
