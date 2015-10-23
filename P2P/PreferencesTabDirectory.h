#if !defined(AFX_PREFERENCESTABDIRECTORY_H__4AF02A92_561E_4124_B9B7_21BF3CA004EF__INCLUDED_)
#define AFX_PREFERENCESTABDIRECTORY_H__4AF02A92_561E_4124_B9B7_21BF3CA004EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTabDirectory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabDirectory dialog

class CPreferencesTabDirectory : public CDialog
{
// Construction
public:
	void SelectDirectory(CString *pStrEdit);
	CPreferencesTabDirectory(CWnd* pParent = NULL);   // standard constructor
	~CPreferencesTabDirectory();

// Dialog Data
	//{{AFX_DATA(CPreferencesTabDirectory)
	enum { IDD = IDD_TAB_PREFERENCE_DIRECTORY };
	CString	m_strInComingFolder;
	CString	m_strShareFolder;
	CString	m_strTempFolder;
	CString	m_staticFree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTabDirectory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesTabDirectory)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTemp();
	afx_msg void OnButtonIncoming();
	afx_msg void OnButtonSharefolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTABDIRECTORY_H__4AF02A92_561E_4124_B9B7_21BF3CA004EF__INCLUDED_)
