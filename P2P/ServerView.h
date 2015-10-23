#if !defined(AFX_SERVERVIEW_H__DE05A314_B19F_4E65_84F4_8468175AEF43__INCLUDED_)
#define AFX_SERVERVIEW_H__DE05A314_B19F_4E65_84F4_8468175AEF43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


class CServerView : public CFormView
{
public:
	CServerView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CServerView)

// Form Data
public:
	//{{AFX_DATA(CServerView)
	enum { IDD = IDD_SERVER_VIEW };
	CStatic	m_staticState;
	CListCtrl	m_listSelect;
	CListCtrl	m_listServer;
	CString	m_strLog;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void AddServerListItem(CString strSeverName, CString strServerIP, BOOL bViewFolder);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CServerView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERVIEW_H__DE05A314_B19F_4E65_84F4_8468175AEF43__INCLUDED_)
