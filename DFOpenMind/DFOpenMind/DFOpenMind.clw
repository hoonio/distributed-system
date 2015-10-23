; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDFOpenMindDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "dfopenmind.h"
LastPage=0

ClassCount=8
Class1=DFClientSock
Class2=CHideMainFrame
Class3=DFLoginDialog
Class4=DFNewUserDlg
Class5=CDFOpenMindApp
Class6=CAboutDlg
Class7=CDFOpenMindDlg
Class8=CDFTransferStatusDlg

ResourceCount=12
Resource1=IDD_ABOUTBOX
Resource2=IDD_LOGIN_DIALOG
Resource3=IDD_NEWUSER_DIALOG
Resource4=IDD_DFOPENMIND_DIALOG
Resource5=IDD_PROGRESS_DIALOG
Resource6=IDR_MENU1
Resource7=IDD_DFOPENMIND_DIALOG (Korean)
Resource8=IDD_PROGRESS_DIALOG (Korean)
Resource9=IDD_NEWUSER_DIALOG (Korean)
Resource10=IDD_LOGIN_DIALOG (Korean)
Resource11=IDD_ABOUTBOX (Korean)
Resource12=IDR_MENU1 (Korean)

[CLS:DFClientSock]
Type=0
BaseClass=CAsyncSocket
HeaderFile=DFClientSock.h
ImplementationFile=DFClientSock.cpp
LastObject=DFClientSock

[CLS:CHideMainFrame]
Type=0
HeaderFile=DFHideMainFrame.h
ImplementationFile=DFHideMainFrame.cpp
LastObject=CHideMainFrame

[CLS:DFLoginDialog]
Type=0
BaseClass=CDialog
HeaderFile=DFLoginDialog.h
ImplementationFile=DFLoginDialog.cpp

[CLS:DFNewUserDlg]
Type=0
BaseClass=CDialog
HeaderFile=DFNewUserDlg.h
ImplementationFile=DFNewUserDlg.cpp
LastObject=IDCANCEL
Filter=D
VirtualFilter=dWC

[CLS:CDFOpenMindApp]
Type=0
BaseClass=CWinApp
HeaderFile=DFOpenMind.h
ImplementationFile=DFOpenMind.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=DFOpenMindDlg.cpp
ImplementationFile=DFOpenMindDlg.cpp
LastObject=CAboutDlg

[CLS:CDFOpenMindDlg]
Type=0
BaseClass=CDialog
HeaderFile=DFOpenMindDlg.h
ImplementationFile=DFOpenMindDlg.cpp
LastObject=IDC_FILELIST
Filter=D
VirtualFilter=dWC

[CLS:CDFTransferStatusDlg]
Type=0
BaseClass=CDialog
HeaderFile=DFTransferStatusDlg.h
ImplementationFile=DFTransferStatusDlg.cpp
LastObject=CDFTransferStatusDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_LOGIN_DIALOG]
Type=1
Class=DFLoginDialog
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC,static,1342308865
Control5=IDC_LOGIN_USER,edit,1350631552
Control6=IDC_LOGIN_PASSWORD,edit,1350631584
Control7=IDC_CHECK_REG,button,1342242819

[DLG:IDD_NEWUSER_DIALOG]
Type=1
Class=DFNewUserDlg
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC,static,1342308865
Control5=IDC_STATIC,static,1342308865
Control6=IDC_STATIC,static,1342308865
Control7=IDC_STATIC,static,1342308865
Control8=IDC_NEWUSER_ID,edit,1350631552
Control9=IDC_NEWUSER_PWD,edit,1350631584
Control10=IDC_NEWUSER_ADDR,edit,1350631552
Control11=IDC_NEWUSER_PHONE,edit,1350631552
Control12=IDC_NEWUSER_MAIL,edit,1350631552

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DFOPENMIND_DIALOG]
Type=1
Class=CDFOpenMindDlg
ControlCount=30
Control1=IDC_SEARCH,button,1342242817
Control2=IDC_EXIT,button,1342242816
Control3=IDC_USERTREE,SysTreeView32,1350631431
Control4=IDC_FILELIST,SysListView32,1350631425
Control5=IDC_STATIC,button,1342177287
Control6=IDC_DOWNLOAD,button,1342242816
Control7=IDC_CONFIG,button,1342242816
Control8=IDC_USER,combobox,1478557954
Control9=IDC_NAME,edit,1350631552
Control10=IDC_CHECK_NAME,button,1342242819
Control11=IDC_CHECK_USER,button,1342242819
Control12=IDC_CHECK_DATE,button,1342242819
Control13=IDC_CHECK_SIZE,button,1342242819
Control14=IDC_CHECK_EXTENTION,button,1342242819
Control15=IDC_DATE_MIN,SysDateTimePick32,1476460576
Control16=IDC_MINSIZE,edit,1484849280
Control17=IDC_MAXSIZE,edit,1484849280
Control18=IDC_STATIC,static,1342308865
Control19=IDC_STATIC,static,1342308865
Control20=IDC_STATIC,static,1342308865
Control21=IDC_EXTENTION,edit,1484849280
Control22=IDC_DATE_MAX,SysDateTimePick32,1476460576
Control23=IDC_STATIC,static,1342308865
Control24=IDC_RADIO_CREATION,button,1476395017
Control25=IDC_STATIC,button,1342177287
Control26=IDC_RADIO_ACCESS,button,1476395017
Control27=IDC_RADIO_MODIFY,button,1476395017
Control28=IDC_STATUS,button,1342242816
Control29=IDC_CONNECT,button,1342242816
Control30=IDC_STATIC,static,1342308865

[DLG:IDD_PROGRESS_DIALOG]
Type=1
Class=CDFTransferStatusDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_SEND_PROGRESS_LIST,SysListView32,1350631425
Control3=IDC_RECV_PROGRESS_LIST,SysListView32,1350631425
Control4=IDC_STATIC,static,1342308865
Control5=IDC_STATIC,static,1342308865
Control6=IDC_ABORT_SEND,button,1476460544
Control7=IDC_ABORT_RECV,button,1476460544
Control8=IDC_STATIC,static,1342308865

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=ID_SHOWDIALOG
Command2=ID_EXIT
CommandCount=2

[DLG:IDD_ABOUTBOX (Korean)]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DFOPENMIND_DIALOG (Korean)]
Type=1
Class=?
ControlCount=30
Control1=IDC_SEARCH,button,1342242817
Control2=IDC_EXIT,button,1342242816
Control3=IDC_USERTREE,SysTreeView32,1350631431
Control4=IDC_FILELIST,SysListView32,1350631425
Control5=IDC_STATIC,button,1342177287
Control6=IDC_DOWNLOAD,button,1342242816
Control7=IDC_CONFIG,button,1342242816
Control8=IDC_USER,combobox,1478557954
Control9=IDC_NAME,edit,1350631552
Control10=IDC_CHECK_NAME,button,1342242819
Control11=IDC_CHECK_USER,button,1342242819
Control12=IDC_CHECK_DATE,button,1342242819
Control13=IDC_CHECK_SIZE,button,1342242819
Control14=IDC_CHECK_EXTENTION,button,1342242819
Control15=IDC_DATE_MIN,SysDateTimePick32,1476460576
Control16=IDC_MINSIZE,edit,1484849280
Control17=IDC_MAXSIZE,edit,1484849280
Control18=IDC_STATIC,static,1342308865
Control19=IDC_STATIC,static,1342308865
Control20=IDC_STATIC,static,1342308865
Control21=IDC_EXTENTION,edit,1484849280
Control22=IDC_DATE_MAX,SysDateTimePick32,1476460576
Control23=IDC_STATIC,static,1342308865
Control24=IDC_RADIO_CREATION,button,1476395017
Control25=IDC_STATIC,button,1342177287
Control26=IDC_RADIO_ACCESS,button,1476395017
Control27=IDC_RADIO_MODIFY,button,1476395017
Control28=IDC_STATUS,button,1342242816
Control29=IDC_CONNECT,button,1342242816
Control30=IDC_STATIC,static,1342308865

[DLG:IDD_PROGRESS_DIALOG (Korean)]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_SEND_PROGRESS_LIST,SysListView32,1350631425
Control3=IDC_RECV_PROGRESS_LIST,SysListView32,1350631425
Control4=IDC_STATIC,static,1342308865
Control5=IDC_STATIC,static,1342308865
Control6=IDC_ABORT_SEND,button,1476460544
Control7=IDC_ABORT_RECV,button,1476460544
Control8=IDC_STATIC,static,1342308865

[DLG:IDD_NEWUSER_DIALOG (Korean)]
Type=1
Class=?
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC,static,1342308865
Control5=IDC_STATIC,static,1342308865
Control6=IDC_STATIC,static,1342308865
Control7=IDC_STATIC,static,1342308865
Control8=IDC_NEWUSER_ID,edit,1350631552
Control9=IDC_NEWUSER_PWD,edit,1350631584
Control10=IDC_NEWUSER_ADDR,edit,1350631552
Control11=IDC_NEWUSER_PHONE,edit,1350631552
Control12=IDC_NEWUSER_MAIL,edit,1350631552

[DLG:IDD_LOGIN_DIALOG (Korean)]
Type=1
Class=?
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC,static,1342308865
Control5=IDC_LOGIN_USER,edit,1350631552
Control6=IDC_LOGIN_PASSWORD,edit,1350631584
Control7=IDC_CHECK_REG,button,1342242819

[MNU:IDR_MENU1 (Korean)]
Type=1
Class=?
Command1=ID_SHOWDIALOG
Command2=ID_EXIT
CommandCount=2

