// P2pView.h : interface of the CP2pView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2PVIEW_H__3F4A975D_11E0_494B_8B3E_CDD46DA8C642__INCLUDED_)
#define AFX_P2PVIEW_H__3F4A975D_11E0_494B_8B3E_CDD46DA8C642__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CP2pView : public CFormView
{
protected: // create from serialization only
	CP2pView();
	DECLARE_DYNCREATE(CP2pView)

public:
	//{{AFX_DATA(CP2pView)
	enum{ IDD = IDD_P2P_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CP2pDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP2pView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CP2pView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CP2pView)
	afx_msg void OnButtonTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in P2pView.cpp
inline CP2pDoc* CP2pView::GetDocument()
   { return (CP2pDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_P2PVIEW_H__3F4A975D_11E0_494B_8B3E_CDD46DA8C642__INCLUDED_)
