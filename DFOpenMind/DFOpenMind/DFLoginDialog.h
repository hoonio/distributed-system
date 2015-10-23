#if !defined(AFX_DFLOGINDIALOG_H__7FD7E766_2CFC_490F_9909_0216CAB8983E__INCLUDED_)
#define AFX_DFLOGINDIALOG_H__7FD7E766_2CFC_490F_9909_0216CAB8983E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFLoginDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DFLoginDialog dialog

class DFLoginDialog : public CDialog
{
// Construction
public:
	DFLoginDialog(CWnd* pParent = NULL);   // standard constructor
	DFLoginDialog(CString strUserID, CString strPwd, BOOL bAuto, CWnd* pParent = NULL);


// Dialog Data
	//{{AFX_DATA(DFLoginDialog)
	enum { IDD = IDD_LOGIN_DIALOG };
	BOOL	m_bChecked;
	CString	m_strPassword;
	CString	m_strUserID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFLoginDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DFLoginDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFLOGINDIALOG_H__7FD7E766_2CFC_490F_9909_0216CAB8983E__INCLUDED_)
