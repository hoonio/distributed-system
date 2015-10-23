#if !defined(AFX_DOWNLOADSVIEW_H__18FDDC84_693C_4C66_BFD9_C7A89C68D126__INCLUDED_)
#define AFX_DOWNLOADSVIEW_H__18FDDC84_693C_4C66_BFD9_C7A89C68D126__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DownloadsView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDownloadsView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "MyIOCPSocket.h"	// Added by ClassView
#include "MyListCtrl.h"


/** MyIOCPSocket.h 에 정의되있음
typedef struct
{
	CString strFileName;
	CString strFilePath;
	DWORD nByte;
}SEARCH_FILE;
**/


// 다운로드목록, (업로드에서도 사용)
typedef struct
{
	MY_SEARCH_FILE downloadList;	// 파일이름, 사이즈, 경로
	UINT nIndex;	// 인덱스번호(다운로드뷰에서와의 연동을위ㅐ)
	CString strServerName;	// 서버이름
	CString strServerIP;	// 서버아이피
	DWORD nReceiveSize;
	CString strStatus;	// 1이면 접속, 0이면 비접속
	DWORD nSpeed;
}MY_DOWNLOAD_FILE_LIST;






class CDownloadsView : public CFormView
{
public:
	CImageList m_imgSmallList;	// 시스템아이콘
	CDownloadsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDownloadsView)

// Form Data
public:
	//{{AFX_DATA(CDownloadsView)
	enum { IDD = IDD_DOWNLOADS_VIEW };
	CMyListCtrl m_listDownloads;
	CStatic	m_staticState;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void AddItem(CString fileName, DWORD size, DWORD nReceiveSize, CString strStatus, CString serverName, UINT nNum);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDownloadsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDownloadsView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRclickListDownloads(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabUpdownDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOWNLOADSVIEW_H__18FDDC84_693C_4C66_BFD9_C7A89C68D126__INCLUDED_)
