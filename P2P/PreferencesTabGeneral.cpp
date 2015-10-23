// PreferencesTabGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "PreferencesTabGeneral.h"

#include "PreferencesValue.h"	// ȯ�漳�� ��
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabGeneral dialog


CPreferencesTabGeneral::CPreferencesTabGeneral(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesTabGeneral::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesTabGeneral)
	m_strUserID = _T("");
	m_bViewFolder = FALSE;
	//}}AFX_DATA_INIT
}

// Destroy ���� ó���� �ȵǼ� �Ҹ��ڿ��� ó��
CPreferencesTabGeneral::~CPreferencesTabGeneral()
{
	// ȯ�漳���� ���Ͽ� ����
	UpdateData(TRUE);
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->GeneralSaveFile(&m_strUserID, m_bViewFolder);
}


void CPreferencesTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTabGeneral)
	DDX_Text(pDX, IDC_EDIT_USERID, m_strUserID);
	DDX_Check(pDX, IDC_CHECK_SHAREFOLDER, m_bViewFolder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesTabGeneral, CDialog)
	//{{AFX_MSG_MAP(CPreferencesTabGeneral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabGeneral message handlers


BOOL CPreferencesTabGeneral::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// ȯ�漳���� ���Ͽ��� �ε�
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->GeneralLoadFile(&m_strUserID, &m_bViewFolder);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


