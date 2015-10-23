#if !defined(AFX_PREFERENCESTABDISPLAY_H__5F08D44A_7245_4EFB_87DF_82500CA46873__INCLUDED_)
#define AFX_PREFERENCESTABDISPLAY_H__5F08D44A_7245_4EFB_87DF_82500CA46873__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTabDisplay.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabDisplay dialog

class CPreferencesTabDisplay : public CDialog
{
// Construction
public:
	CPreferencesTabDisplay(CWnd* pParent = NULL);   // standard constructor
	~CPreferencesTabDisplay();

// Dialog Data
	//{{AFX_DATA(CPreferencesTabDisplay)
	enum { IDD = IDD_TAB_PREFERENCE_DISPLAY };
	BOOL	m_bDeleteComplete;
	UINT	m_nPopupTime;
	BOOL	m_bPopupConnect;
	BOOL	m_bPopupDisconnect;
	BOOL	m_bPopupCompleteDownload;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTabDisplay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesTabDisplay)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTABDISPLAY_H__5F08D44A_7245_4EFB_87DF_82500CA46873__INCLUDED_)
