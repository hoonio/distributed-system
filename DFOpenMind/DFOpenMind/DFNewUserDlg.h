#if !defined(AFX_DFNEWUSERDLG_H__BC79951F_0767_40FA_B264_8A03FCF5F78D__INCLUDED_)
#define AFX_DFNEWUSERDLG_H__BC79951F_0767_40FA_B264_8A03FCF5F78D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFNewUserDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DFNewUserDlg dialog

class DFNewUserDlg : public CDialog
{
// Construction
public:
	DFNewUserDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DFNewUserDlg)
	enum { IDD = IDD_NEWUSER_DIALOG };
	CString	m_strAddr;
	CString	m_strID;
	CString	m_strMail;
	CString	m_strPhone;
	CString	m_strPwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFNewUserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DFNewUserDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFNEWUSERDLG_H__BC79951F_0767_40FA_B264_8A03FCF5F78D__INCLUDED_)
