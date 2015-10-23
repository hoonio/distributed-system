; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CUploadsView
LastTemplate=CTabCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "P2p.h"
LastPage=0

ClassCount=18
Class1=CP2pApp
Class2=CP2pDoc
Class3=CP2pView
Class4=CMainFrame

ResourceCount=21
Resource1=IDR_MENU_TRAY
Resource2=IDR_MENU_SERVER_LIBRARY
Class5=CAboutDlg
Resource3=IDD_SERVER_VIEW
Resource4=IDD_FORMVIEW (English (U.S.))
Class6=CPreferencesDlg
Resource5=IDD_DIALOG_MSN
Class7=CConnectDlg
Resource6=IDD_TAB_PREFERENCE_CONNECT
Class8=CSearchView
Resource7=IDD_UPLOADS_VIEW
Class9=CDownloadsView
Resource8=IDD_P2P_FORM
Resource9=IDD_OLE_PROPPAGE_LARGE (English (U.S.))
Class10=CUploadsView
Resource10=IDD_TAB_SEARCH_VIEW_DETAIL
Class11=CServerView
Resource11=IDD_DIALOG_CONNECT
Class12=CChatView
Resource12=IDR_MENU_TAB_UPDOWN
Class13=CMsnDlg
Resource13=IDD_SEARCH_VIEW
Resource14=IDD_DOWNLOADS_VIEW
Class14=CMyListCtrl
Resource15=IDR_MAINFRAME
Resource16=IDR_MENU_TAB_SEARCH
Resource17=IDD_ABOUTBOX
Class15=CPreferencesTabConnect
Class16=CPreferencesTabDirectory
Class17=CPreferencesTabDisplay
Class18=CMyTabCtrl
Resource18=IDD_DIALOG_PREFERENCES
Resource19=IDR_MENU1
Resource20=IDD_TAB_PREFERENCE_DIRECTORY
Resource21=IDD_TAB_PREFERENCE_DISPLAY

[CLS:CP2pApp]
Type=0
HeaderFile=P2p.h
ImplementationFile=P2p.cpp
Filter=N
LastObject=CP2pApp

[CLS:CP2pDoc]
Type=0
HeaderFile=P2pDoc.h
ImplementationFile=P2pDoc.cpp
Filter=N
LastObject=CP2pDoc

[CLS:CP2pView]
Type=0
HeaderFile=P2pView.h
ImplementationFile=P2pView.cpp
Filter=D
LastObject=CP2pView
BaseClass=CFormView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=W
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=P2p.cpp
ImplementationFile=P2p.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_MENU_SERVER_CONNECT
Command2=ID_MENU_SERVER_DISCONNECT
Command3=ID_APP_EXIT
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_VIEW_TOOLBAR
Command9=ID_VIEW_STATUS_BAR
Command10=ID_APP_ABOUT
Command11=ID_MENU_PREFERENCES
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_P2P_FORM]
Type=1
Class=CP2pView
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_BUTTON_TEST,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_SERVER_CONNECT
Command2=ID_MENU_SEARCH
Command3=ID_MENU_DOWNLOADS
Command4=ID_MENU_UPLOADS
Command5=ID_MENU_SERVER
Command6=ID_MENU_CHAT
Command7=ID_MENU_PREFERENCES
Command8=ID_APP_ABOUT
Command9=ID_MENU_TEMP
CommandCount=9

[MNU:IDR_MENU_TRAY]
Type=1
Class=?
Command1=ID_MENU_SERVER_CONNECT
Command2=ID_MENU_SERVER_DISCONNECT
Command3=ID_MENU_PREFERENCES
Command4=ID_APP_ABOUT
Command5=ID_APP_EXIT
CommandCount=5

[DLG:IDD_DIALOG_CONNECT]
Type=1
Class=CConnectDlg
ControlCount=16
Control1=IDC_RADIO_CLIENT,button,1342308361
Control2=IDC_RADIO_SERVER,button,1342177289
Control3=IDC_EDIT_SERVERNAME,edit,1350631552
Control4=IDC_IPADDRESS_SERVERIP,SysIPAddress32,1342242816
Control5=IDC_EDIT_USERID,edit,1350631552
Control6=IDC_CHECK_VIEWFOLDER,button,1342373891
Control7=IDC_LIST_SERVER,SysListView32,1350631433
Control8=IDC_BUTTON_CONNECT,button,1342242817
Control9=IDC_BUTTON_NEW,button,1342242816
Control10=IDCANCEL,button,1342242816
Control11=IDC_STATIC,button,1342177287
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342177294

[DLG:IDD_DIALOG_PREFERENCES]
Type=1
Class=CPreferencesDlg
ControlCount=4
Control1=IDC_TAB_OPTION,SysTabControl32,1342177280
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342177294

[CLS:CPreferencesDlg]
Type=0
HeaderFile=PreferencesDlg.h
ImplementationFile=PreferencesDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CPreferencesDlg
VirtualFilter=dWC

[CLS:CConnectDlg]
Type=0
HeaderFile=ConnectDlg.h
ImplementationFile=ConnectDlg.cpp
BaseClass=CDialog
Filter=W
VirtualFilter=dWC
LastObject=ID_SERVER_LIBRARY_DELETE

[DLG:IDD_SEARCH_VIEW]
Type=1
Class=CSearchView
ControlCount=7
Control1=IDC_LIST_SEARCH,SysListView32,1350631433
Control2=IDC_TAB_SEARCH,SysTabControl32,1342177280
Control3=IDC_EDIT_SEARCHKEY,edit,1350631552
Control4=IDC_COMBO_SERVERFILTER,combobox,1344339971
Control5=IDC_BUTTON_SEARCH,button,1342242817
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342179843

[CLS:CSearchView]
Type=0
HeaderFile=SearchView.h
ImplementationFile=SearchView.cpp
BaseClass=CFormView
Filter=W
LastObject=CSearchView
VirtualFilter=VWC

[DLG:IDD_DOWNLOADS_VIEW]
Type=1
Class=CDownloadsView
ControlCount=4
Control1=IDC_LIST_DOWNLOADS,SysListView32,1350631433
Control2=IDC_STATIC_STATE,static,1342308352
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342179843

[CLS:CDownloadsView]
Type=0
HeaderFile=DownloadsView.h
ImplementationFile=DownloadsView.cpp
BaseClass=CFormView
Filter=W
LastObject=CDownloadsView
VirtualFilter=VWC

[DLG:IDD_OLE_PROPPAGE_LARGE (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_UPLOADS_VIEW]
Type=1
Class=CUploadsView
ControlCount=4
Control1=IDC_LIST_UPLOADS,SysListView32,1350631433
Control2=IDC_STATIC_STATE,static,1342308352
Control3=IDC_STATIC,static,1342179843
Control4=IDC_STATIC,button,1342177287

[CLS:CUploadsView]
Type=0
HeaderFile=UploadsView.h
ImplementationFile=UploadsView.cpp
BaseClass=CFormView
Filter=W
LastObject=CUploadsView
VirtualFilter=VWC

[DLG:IDD_SERVER_VIEW]
Type=1
Class=CServerView
ControlCount=9
Control1=IDC_LIST_SERVER,SysListView32,1350631433
Control2=IDC_STATIC_STATE,static,1342308352
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342179843
Control5=IDC_TREE_EXPLORER,SysTreeView32,1350631431
Control6=IDC_LIST_SELECT,SysListView32,1350631433
Control7=IDC_EDIT_LOG,edit,1353777348
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,button,1342177287

[CLS:CServerView]
Type=0
HeaderFile=ServerView.h
ImplementationFile=ServerView.cpp
BaseClass=CFormView
Filter=D
LastObject=CServerView
VirtualFilter=VWC

[CLS:CChatView]
Type=0
HeaderFile=ChatView.h
ImplementationFile=ChatView.cpp
BaseClass=CFormView
Filter=D
LastObject=CChatView
VirtualFilter=VWC

[DLG:IDD_DIALOG_MSN]
Type=1
Class=CMsnDlg
ControlCount=6
Control1=IDC_EDIT_SEND,edit,1350631552
Control2=IDC_BUTTON_SEND,button,1342242817
Control3=IDC_EDIT_CHAT,edit,1352728644
Control4=IDC_LIST_SERVER,SysListView32,1350631427
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342177294

[CLS:CMsnDlg]
Type=0
HeaderFile=MsnDlg.h
ImplementationFile=MsnDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CMsnDlg
VirtualFilter=dWC

[DLG:IDD_TAB_SEARCH_VIEW_DETAIL]
Type=1
Class=?
ControlCount=1
Control1=IDC_LIST_SEARCH,SysListView32,1350631433

[MNU:IDR_MENU_SERVER_LIBRARY]
Type=1
Class=?
Command1=ID_SERVER_LIBRARY_INSERT
Command2=ID_SERVER_LIBRARY_DELETE
CommandCount=2

[CLS:CMyListCtrl]
Type=0
HeaderFile=MyListCtrl.h
ImplementationFile=MyListCtrl.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC
LastObject=CMyListCtrl

[DLG:IDD_TAB_PREFERENCE_CONNECT]
Type=1
Class=CPreferencesTabConnect
ControlCount=7
Control1=IDC_SLIDER_DOWNLOAD,msctls_trackbar32,1342242837
Control2=IDC_SLIDER_UPLOAD,msctls_trackbar32,1342242837
Control3=IDC_STATIC_DOWNLOAD,static,1342308352
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC_UPLOAD,static,1342308352
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_TAB_PREFERENCE_DIRECTORY]
Type=1
Class=CPreferencesTabDirectory
ControlCount=12
Control1=IDC_EDIT_TEMP,edit,1350631552
Control2=IDC_BUTTON_TEMP,button,1342242817
Control3=IDC_EDIT_INCOMING,edit,1350631552
Control4=IDC_BUTTON_INCOMING,button,1342242817
Control5=IDC_EDIT_SHAREFOLDER,edit,1350631552
Control6=IDC_BUTTON_SHAREFOLDER,button,1342242817
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC_FREE,static,1342308352
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352

[DLG:IDD_TAB_PREFERENCE_DISPLAY]
Type=1
Class=CPreferencesTabDisplay
ControlCount=8
Control1=IDC_CHECK_DELETE_COMPLETE,button,1342242819
Control2=IDC_EDIT_POPUPTIME,edit,1350631552
Control3=IDC_CHECK_CONNECT,button,1342242819
Control4=IDC_CHECK_DOWNLOAD,button,1342242819
Control5=IDC_CHECK_DISCONNECT,button,1342242819
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,button,1342177287

[CLS:CPreferencesTabConnect]
Type=0
HeaderFile=PreferencesTabConnect.h
ImplementationFile=PreferencesTabConnect.cpp
BaseClass=CDialog
Filter=D
LastObject=CPreferencesTabConnect
VirtualFilter=dWC

[CLS:CPreferencesTabDirectory]
Type=0
HeaderFile=PreferencesTabDirectory.h
ImplementationFile=PreferencesTabDirectory.cpp
BaseClass=CDialog
Filter=D
LastObject=CPreferencesTabDirectory
VirtualFilter=dWC

[CLS:CPreferencesTabDisplay]
Type=0
HeaderFile=PreferencesTabDisplay.h
ImplementationFile=PreferencesTabDisplay.cpp
BaseClass=CDialog
Filter=D
LastObject=CPreferencesTabDisplay
VirtualFilter=dWC

[CLS:CMyTabCtrl]
Type=0
HeaderFile=MyTabCtrl.h
ImplementationFile=MyTabCtrl.cpp
BaseClass=CTabCtrl
Filter=W
VirtualFilter=UWC
LastObject=CMyTabCtrl

[MNU:IDR_MENU_TAB_SEARCH]
Type=1
Class=?
Command1=ID_TAB_SEARCH_DELETE
CommandCount=1

[MNU:IDR_MENU1]
Type=1
CommandCount=0

[MNU:IDR_MENU_TAB_UPDOWN]
Type=1
Class=?
Command1=ID_TAB_UPDOWN_DELETE
CommandCount=1

[DLG:IDD_FORMVIEW (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

