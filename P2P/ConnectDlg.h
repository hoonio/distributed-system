#if !defined(AFX_CONNECTDLG_H__3B407B96_5F34_4BFE_80A5_3DAC70C6A6AC__INCLUDED_)
#define AFX_CONNECTDLG_H__3B407B96_5F34_4BFE_80A5_3DAC70C6A6AC__INCLUDED_

#include "DataServerLibrary.h"
#include "MyListCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog

class CConnectDlg : public CDialog
{
// Construction
public:
	void ModifyEdit(UINT n);
	void SaveFile();
	void LoadFile();
	CDataServerLibrary m_cServerInfo;	// 서버목록 데이타
	UINT m_nListIndex;	// 리스트컨트롤에서 현재선택된 번호
	
	// 서버목록을 파일에 저장
	CArchive *m_pARead;
	CArchive *m_pAWrite;
	CFile *m_pFile;

	void AddItem(CString name, CString ip, CString userID, CString view, UINT image);
	CConnectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectDlg)
	enum { IDD = IDD_DIALOG_CONNECT };
	CMyListCtrl	m_listServer;
	CIPAddressCtrl	m_ipAddressServerIP;
	CString	m_strServerName;
	CString	m_strUserID;
	int		m_nClient;
	BOOL	m_bViewFolder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioClient();
	afx_msg void OnRadioServer();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonConnect();
	afx_msg void OnDestroy();
	afx_msg void OnClickListServer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditServername();
	afx_msg void OnChangeEditUserid();
	afx_msg void OnFieldchangedIpaddressServerip(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckViewfolder();
	afx_msg void OnRclickListServer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnServerLibraryDelete();
	afx_msg void OnServerLibraryInsert();
	afx_msg void OnDblclkListServer(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTDLG_H__3B407B96_5F34_4BFE_80A5_3DAC70C6A6AC__INCLUDED_)
