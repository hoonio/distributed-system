// DFOpenMindServiceTest.h : main header file for the DFOPENMINDSERVICETEST application
//

#if !defined(AFX_DFOPENMINDSERVICETEST_H__D0FCC176_5998_4D1B_8353_65A7EF8D0ED7__INCLUDED_)
#define AFX_DFOPENMINDSERVICETEST_H__D0FCC176_5998_4D1B_8353_65A7EF8D0ED7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindServiceTestApp:
// See DFOpenMindServiceTest.cpp for the implementation of this class
//

class CDFOpenMindServiceTestApp : public CWinApp
{
public:
	CDFOpenMindServiceTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFOpenMindServiceTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDFOpenMindServiceTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFOPENMINDSERVICETEST_H__D0FCC176_5998_4D1B_8353_65A7EF8D0ED7__INCLUDED_)
