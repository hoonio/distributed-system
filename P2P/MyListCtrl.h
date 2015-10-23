#if !defined(AFX_MYLISTCTRL_H__39FEDC6A_DDB1_4445_8F37_3305A910ECFC__INCLUDED_)
#define AFX_MYLISTCTRL_H__39FEDC6A_DDB1_4445_8F37_3305A910ECFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

#define CONNECTDLG 1
#define SEARCHVIEW 2
#define UPLOADVIEW 3
#define DOWNLOADVIEW 4

class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void MySelectItem();
	UINT m_nType;	// 검색에서도 사용하고 접속리스트에도 사용하므로 서로 기능이 틀리므로 여기값으로 각각기능 처리
	void MyDeleteItem();
	virtual ~CMyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__39FEDC6A_DDB1_4445_8F37_3305A910ECFC__INCLUDED_)
