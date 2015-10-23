// MsnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "MsnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsnDlg dialog


CMsnDlg::CMsnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsnDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMsnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsnDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsnDlg, CDialog)
	//{{AFX_MSG_MAP(CMsnDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsnDlg message handlers

BOOL CMsnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON_VIEW_CHAT);
	ASSERT(hIcon);
	SetIcon(hIcon, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
