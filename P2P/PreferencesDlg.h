#if !defined(AFX_PREFERENCESDLG_H__F7EF4A7E_F092_4C3B_9687_8E628F641629__INCLUDED_)
#define AFX_PREFERENCESDLG_H__F7EF4A7E_F092_4C3B_9687_8E628F641629__INCLUDED_

#include "PreferencesTabDirectory.h"	// Added by ClassView
#include "PreferencesTabConnect.h"	// Added by ClassView
#include "PreferencesTabDisplay.h"	// Added by ClassView
#include "MyTabCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

class CPreferencesDlg : public CDialog
{
// Construction
public:
	void FirstSetCurSel(UINT num);	
	CPreferencesTabConnect *m_pPreferencesTabConnect;	// ����
	CPreferencesTabDirectory *m_pPreferencesTabDirectory;	// ���丮
	CPreferencesTabDisplay *m_pPreferencesTabDisplay;	// ���÷���
	

	UINT m_nTabViewNum;	// ����� ���̰� �ִ���
	void TabView();
	CPreferencesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	enum { IDD = IDD_DIALOG_PREFERENCES };
	CMyTabCtrl	m_tabOption;	// esc ��ư�� ����� �ǳ����� �������Ƿ� ����Ŭ��������ó��
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabOption(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDLG_H__F7EF4A7E_F092_4C3B_9687_8E628F641629__INCLUDED_)
