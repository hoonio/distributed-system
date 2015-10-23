// DFOpenMindServiceTestDlg.h : header file
//

#if !defined(AFX_DFOPENMINDSERVICETESTDLG_H__50D13E04_22B7_4506_9267_1063A37E265C__INCLUDED_)
#define AFX_DFOPENMINDSERVICETESTDLG_H__50D13E04_22B7_4506_9267_1063A37E265C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DFOpenMindCore.h"

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindServiceTestDlg dialog

class CDFOpenMindServiceTestDlg : public CDialog
{
// Construction
public:
	CDFOpenMindServiceTestDlg(CWnd* pParent = NULL);	// standard constructor
	DFOpenMindCore	m_OpenMind;
	BOOL			m_bInService;
// Dialog Data
	//{{AFX_DATA(CDFOpenMindServiceTestDlg)
	enum { IDD = IDD_DFOPENMINDSERVICETEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFOpenMindServiceTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDFOpenMindServiceTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnStop();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFOPENMINDSERVICETESTDLG_H__50D13E04_22B7_4506_9267_1063A37E265C__INCLUDED_)
