// DFOpenMind.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFOpenMindDlg.h"
#include "DFHideMainFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindApp

BEGIN_MESSAGE_MAP(CDFOpenMindApp, CWinApp)
	//{{AFX_MSG_MAP(CDFOpenMindApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindApp construction

CDFOpenMindApp::CDFOpenMindApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDFOpenMindApp object

CDFOpenMindApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindApp initialization

BOOL CDFOpenMindApp::InitInstance()
{
	// 이미 DFOpenMind가 존재한다면 종료한다.
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("c7179d3a-9b54-4778-af2e-933c27fe8bb2"));
	if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(m_hEvent);
		return FALSE;
	}
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(TEXT("Winsock Initialization Fail"));
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	m_pMainWnd = new DFHideMainFrame;
	m_nCmdShow = SW_SHOWMINIMIZED;			
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();			
	HideApplication();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

int CDFOpenMindApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	CloseHandle(m_hEvent);
	m_hEvent = INVALID_HANDLE_VALUE;
	return CWinApp::ExitInstance();
}
