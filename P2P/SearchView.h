#if !defined(AFX_SEARCHVIEW_H__995B8DD1_7242_4BF5_82BA_0BE8845BDF4A__INCLUDED_)
#define AFX_SEARCHVIEW_H__995B8DD1_7242_4BF5_82BA_0BE8845BDF4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#include "MyIOCPSocket.h"	// Added by ClassView
#include "MyListCtrl.h"

#define MAX_SEARCH_COUNT 20	// �˻��ִ��
#define MAX_SEARCH_FILE_COUNT 100	// �˻��Ǽ������������� ��

class CSearchViewDetail;



/** MyIOCPSocket.h �� ���ǵ�����
typedef struct
{
	CString strFileName;
	CString strFilePath;
	DWORD nByte;
}SEARCH_FILE;
**/


// �˻��� ����� ������ ����
typedef struct
{
	MY_SEARCH_FILE searchListData[MAX_SEARCH_FILE_COUNT];
	CString strServerName[MAX_SEARCH_FILE_COUNT];	// �����̸�
	CString strServerIP[MAX_SEARCH_FILE_COUNT];	// ����������
	UINT nSearchListSize;	// �˻��ȼ�
	CString strSearchListFileName;	// �˻��̸�
}MY_SEARCH_FILE_DETAIL_LIST;


class CSearchView : public CFormView
{
public:
	CSearchView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSearchView)

// Form Data
public:
	//{{AFX_DATA(CSearchView)
	enum { IDD = IDD_SEARCH_VIEW };
	CMyListCtrl	m_listSearch;
	CComboBox	m_comboFilter;
	CTabCtrl	m_tabSearch;
	CString	m_strKey;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	CImageList m_imgSmallList;	// �ý��۾�����
	UINT m_nSearchFileCnt;	// �ϳ��� �ǿ��� �˻��� �����Ǽ��� �����Ѵ�
	void AddItemSearchFile(char *pszData, UINT cnt);	
	void CreateTab(CString searchName, UINT cnt, UINT nImage);	// �������� �ǻ���
	void AddItem(CString fileName, UINT size, CString serverName, BOOL flag);
	void InsertColumn();
	static UINT m_nTabCnt;	// ������ ���� ���� (������ ��ӳ��ƾ��ϹǷ� static)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSearchView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSearchView)
	afx_msg void OnButtonSearch();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeTabSearch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTabSearch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabSearchDelete();
	afx_msg void OnDblclkListSearch(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHVIEW_H__995B8DD1_7242_4BF5_82BA_0BE8845BDF4A__INCLUDED_)
