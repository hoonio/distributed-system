// P2p.h : main header file for the P2P application
//

#if !defined(AFX_P2P_H__683DE54C_485E_4D69_A1D3_496FC1E362D1__INCLUDED_)
#define AFX_P2P_H__683DE54C_485E_4D69_A1D3_496FC1E362D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CP2pApp:
// See P2p.cpp for the implementation of this class
//

class CP2pApp : public CWinApp
{
public:
	CP2pApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP2pApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CP2pApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_P2P_H__683DE54C_485E_4D69_A1D3_496FC1E362D1__INCLUDED_)
