// DFOpenMindDlg.h : header file
//

#if !defined(AFX_DFOPENMINDDLG_H__B2F69ADF_C739_4017_A53E_268B8CFBCF15__INCLUDED_)
#define AFX_DFOPENMINDDLG_H__B2F69ADF_C739_4017_A53E_268B8CFBCF15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindDlg dialog
#include "resource.h"
#include "..\DFOpenMindProtocol.h"

class CDFOpenMindDlg : public CDialog
{
// Construction
public:
	CDFOpenMindDlg(CWnd* pParent = NULL);	// standard constructor
	
	BYTE		m_bRadioSelect;
	HTREEITEM	m_hRootItem;
	HTREEITEM	m_hMyItem;
	HTREEITEM	m_hOtherItem;

	void		RefleshUserTree();
	void		DeleteUser(LPCTSTR tszUserID);
	void		AddUser(LPCTSTR tszUserID);
	void		RemoveAllList();
	void		AddFile(PFILE_INFO pFileInfo);

	void		ConvertTimeToFileTime(COleDateTime Time, FILETIME& FileTime);
	void		Download(CString strUserID, CString strFileName);

// Dialog Data
	//{{AFX_DATA(CDFOpenMindDlg)
	enum { IDD = IDD_DFOPENMIND_DIALOG };
	CListCtrl	m_FileList;
	CTreeCtrl	m_UserTree;
	COleDateTime	m_MaxDate;
	COleDateTime	m_MinDate;
	CString	m_strName;
	CString	m_strExtension;
	CString	m_strUser;
	BOOL	m_bDate;
	BOOL	m_bExtension;
	BOOL	m_bName;
	BOOL	m_bSize;
	BOOL	m_bUser;
	CString	m_strMinSize;
	CString	m_strMaxSize;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFOpenMindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDFOpenMindDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnExit();
	afx_msg void OnConfig();
	afx_msg void OnStatus();
	afx_msg void OnSearch();
	afx_msg void OnDownload();
	afx_msg void OnCheckDate();
	afx_msg void OnCheckExtention();
	afx_msg void OnCheckName();
	afx_msg void OnCheckSize();
	afx_msg void OnCheckUser();
	afx_msg void OnRadioAccess();
	afx_msg void OnRadioCreation();
	afx_msg void OnRadioModify();
	afx_msg void OnConnect();
	afx_msg void OnDblclkUsertree(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnItemdblclickFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFOPENMINDDLG_H__B2F69ADF_C739_4017_A53E_268B8CFBCF15__INCLUDED_)
