// DFLoginDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFLoginDialog.h"
#include "DFHideMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DFLoginDialog dialog


DFLoginDialog::DFLoginDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DFLoginDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DFLoginDialog)
	m_bChecked = FALSE;
	m_strPassword = _T("");
	m_strUserID = _T("");
	//}}AFX_DATA_INIT
}


DFLoginDialog::DFLoginDialog(CString strUserID, CString strPwd, BOOL bAuto, CWnd* pParent)
	: CDialog(DFLoginDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DFLoginDialog)
	m_bChecked = bAuto;
	m_strPassword = strPwd;//_T("");
	m_strUserID = strUserID; //_T("");
	//}}AFX_DATA_INIT
}


void DFLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFLoginDialog)
	DDX_Check(pDX, IDC_CHECK_REG, m_bChecked);
	DDX_Text(pDX, IDC_LOGIN_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 8);
	DDX_Text(pDX, IDC_LOGIN_USER, m_strUserID);
	DDV_MaxChars(pDX, m_strUserID, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DFLoginDialog, CDialog)
	//{{AFX_MSG_MAP(DFLoginDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DFLoginDialog message handlers

void DFLoginDialog::OnOK() 
{
	// TODO: Add extra validation here
//	((DFHideMainFrame*)(((CDFOpenMindApp*)AfxGetApp())->m_pMainWnd))->
//		SendLoginMsg(m_strUserID, m_strPassword);
	CDialog::OnOK();
}

void DFLoginDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}


BOOL DFLoginDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_bChecked)
		OnOK();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
