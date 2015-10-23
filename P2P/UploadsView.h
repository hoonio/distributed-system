#if !defined(AFX_UPLOADSVIEW_H__54D7EAA3_B740_444B_800C_1237763A21BA__INCLUDED_)
#define AFX_UPLOADSVIEW_H__54D7EAA3_B740_444B_800C_1237763A21BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UploadsView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUploadsView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "MyListCtrl.h"


class CUploadsView : public CFormView
{
public:
	CImageList m_imgSmallList;	// 시스템아이콘
	CUploadsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CUploadsView)

// Form Data
public:
	//{{AFX_DATA(CUploadsView)
	enum { IDD = IDD_UPLOADS_VIEW };
	CMyListCtrl	m_listUploads;
	CStatic	m_staticState;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void AddItem(CString fileName, DWORD size, DWORD nReceiveSize, CString strStatus, CString serverName, UINT nNum);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUploadsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CUploadsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CUploadsView)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRclickListUploads(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabUpdownDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPLOADSVIEW_H__54D7EAA3_B740_444B_800C_1237763A21BA__INCLUDED_)
