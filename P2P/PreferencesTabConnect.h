#if !defined(AFX_PREFERENCESTABCONNECT_H__B718C7A2_C673_4F35_84BD_0018D0740E41__INCLUDED_)
#define AFX_PREFERENCESTABCONNECT_H__B718C7A2_C673_4F35_84BD_0018D0740E41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTabConnect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabConnect dialog

class CPreferencesTabConnect : public CDialog
{
// Construction
public:
	CPreferencesTabConnect(CWnd* pParent = NULL);   // standard constructor
	~CPreferencesTabConnect();

// Dialog Data
	//{{AFX_DATA(CPreferencesTabConnect)
	enum { IDD = IDD_TAB_PREFERENCE_CONNECT };
	CSliderCtrl	m_SliderUpload;
	CSliderCtrl	m_SliderDownload;
	int		m_nDownload;
	int		m_nUpload;
	CString	m_strStaticDownload;
	CString	m_strStaticUpload;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTabConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesTabConnect)
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawSliderDownload(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderUpload(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTABCONNECT_H__B718C7A2_C673_4F35_84BD_0018D0740E41__INCLUDED_)
