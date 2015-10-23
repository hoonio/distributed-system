// DFNewUserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFNewUserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DFNewUserDlg dialog


DFNewUserDlg::DFNewUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DFNewUserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DFNewUserDlg)
	m_strAddr = _T("");
	m_strID = _T("");
	m_strMail = _T("");
	m_strPhone = _T("");
	m_strPwd = _T("");
	//}}AFX_DATA_INIT
}


void DFNewUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFNewUserDlg)
	DDX_Text(pDX, IDC_NEWUSER_ADDR, m_strAddr);
	DDV_MaxChars(pDX, m_strAddr, 60);
	DDX_Text(pDX, IDC_NEWUSER_ID, m_strID);
	DDV_MaxChars(pDX, m_strID, 12);
	DDX_Text(pDX, IDC_NEWUSER_MAIL, m_strMail);
	DDV_MaxChars(pDX, m_strMail, 40);
	DDX_Text(pDX, IDC_NEWUSER_PHONE, m_strPhone);
	DDV_MaxChars(pDX, m_strPhone, 16);
	DDX_Text(pDX, IDC_NEWUSER_PWD, m_strPwd);
	DDV_MaxChars(pDX, m_strPwd, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DFNewUserDlg, CDialog)
	//{{AFX_MSG_MAP(DFNewUserDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DFNewUserDlg message handlers



void DFNewUserDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	
	CDialog::OnOK();
}

void DFNewUserDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_strAddr = _T("");
	m_strID = _T("");
	m_strMail = _T("");
	m_strPhone = _T("");
	m_strPwd = _T("");

	CDialog::OnCancel();
}
