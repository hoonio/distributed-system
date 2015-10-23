// DFOpenMind.h : main header file for the DFOPENMIND application
//

#if !defined(AFX_DFOPENMIND_H__A9B248C2_196F_40F8_9C8E_369868A2A25C__INCLUDED_)
#define AFX_DFOPENMIND_H__A9B248C2_196F_40F8_9C8E_369868A2A25C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindApp:
// See DFOpenMind.cpp for the implementation of this class
//

class CDFOpenMindApp : public CWinApp
{
public:
	CDFOpenMindApp();

	HANDLE		m_hEvent;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFOpenMindApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDFOpenMindApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFOPENMIND_H__A9B248C2_196F_40F8_9C8E_369868A2A25C__INCLUDED_)
