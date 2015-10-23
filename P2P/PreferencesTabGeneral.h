#if !defined(AFX_PREFERENCESTABGENERAL_H__E3525AB7_CEEE_469B_A0A2_1D4BFE3AB5C9__INCLUDED_)
#define AFX_PREFERENCESTABGENERAL_H__E3525AB7_CEEE_469B_A0A2_1D4BFE3AB5C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTabGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabGeneral dialog

class CPreferencesTabGeneral : public CDialog
{
// Construction
public:
	CPreferencesTabGeneral(CWnd* pParent = NULL);   // standard constructor
	~CPreferencesTabGeneral();

// Dialog Data
	//{{AFX_DATA(CPreferencesTabGeneral)
	enum { IDD = IDD_TAB_PREFERENCE_GENERAL };
	CString	m_strUserID;
	BOOL	m_bViewFolder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTabGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesTabGeneral)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTABGENERAL_H__E3525AB7_CEEE_469B_A0A2_1D4BFE3AB5C9__INCLUDED_)
