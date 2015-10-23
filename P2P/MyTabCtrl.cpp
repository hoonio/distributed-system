// MyTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "MyTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl

CMyTabCtrl::CMyTabCtrl()
{
}

CMyTabCtrl::~CMyTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CMyTabCtrl)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl message handlers

void CMyTabCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar == VK_ESCAPE){
		return;
	}

	CTabCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
