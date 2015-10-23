// P2pView.cpp : implementation of the CP2pView class
//

#include "stdafx.h"
#include "P2p.h"

#include "P2pDoc.h"
#include "P2pView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP2pView

IMPLEMENT_DYNCREATE(CP2pView, CFormView)

BEGIN_MESSAGE_MAP(CP2pView, CFormView)
	//{{AFX_MSG_MAP(CP2pView)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP2pView construction/destruction

CP2pView::CP2pView()
	: CFormView(CP2pView::IDD)
{
	//{{AFX_DATA_INIT(CP2pView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CP2pView::~CP2pView()
{
}

void CP2pView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CP2pView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CP2pView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}
#include "MainFrm.h"
void CP2pView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();


	//
	//((CMainFrame *)AfxGetMainWnd())->ShowView(0);	// 검색이 처음보이게

}

/////////////////////////////////////////////////////////////////////////////
// CP2pView diagnostics

#ifdef _DEBUG
void CP2pView::AssertValid() const
{
	CFormView::AssertValid();
}

void CP2pView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CP2pDoc* CP2pView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CP2pDoc)));
	return (CP2pDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CP2pView message handlers


#include "MainFrm.h"

void CP2pView::OnButtonTest() 
{
	// TODO: Add your control notification handler code here

	((CMainFrame *)AfxGetMainWnd())->m_wndTaskbarNotifier.Show("str");
}
