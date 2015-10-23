#if !defined(AFX_DFTRANSFERSTATUSDLG_H__1F4117C4_5C6C_4846_B9FF_E78AF530B7C8__INCLUDED_)
#define AFX_DFTRANSFERSTATUSDLG_H__1F4117C4_5C6C_4846_B9FF_E78AF530B7C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFTransferStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDFTransferStatusDlg dialog
#include "..\DFOpenMindProtocol.h"
#include "DFFileServer.h"

class CDFTransferStatusDlg : public CDialog
{
// Construction
public:
	CDFTransferStatusDlg(CWnd* pParent = NULL);   // standard constructor

	UINT	m_nTimer;

	// 새로 송신과 수신 정보를 얻어온다.
	void	RefreshSendList();
	void	RefreshRecvList();

	void	UpdateSendItem(PXFER_STATUS pXferStatus);
	void	UpdateRecvItem(PXFER_STATUS pXferStatus);

// Dialog Data
	//{{AFX_DATA(CDFTransferStatusDlg)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CListCtrl	m_SendStatusList;
	CListCtrl	m_RecvStatusList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFTransferStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDFTransferStatusDlg)
	afx_msg void OnAbortRecv();
	afx_msg void OnAbortSend();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFTRANSFERSTATUSDLG_H__1F4117C4_5C6C_4846_B9FF_E78AF530B7C8__INCLUDED_)
