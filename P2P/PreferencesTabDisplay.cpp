// PreferencesTabDisplay.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "PreferencesTabDisplay.h"

#include "PreferencesValue.h"	// ȯ�漳�� ��
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabDisplay dialog


CPreferencesTabDisplay::CPreferencesTabDisplay(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesTabDisplay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesTabDisplay)
	m_bDeleteComplete = FALSE;
	m_nPopupTime = 0;
	m_bPopupConnect = FALSE;
	m_bPopupDisconnect = FALSE;
	m_bPopupCompleteDownload = FALSE;
	//}}AFX_DATA_INIT
}


// Destroy ���� ó���� �ȵǼ� �Ҹ��ڿ��� ó��
CPreferencesTabDisplay::~CPreferencesTabDisplay()
{
	// ȯ�漳���� ���Ͽ� ����
	UpdateData(TRUE);
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->DisplaySaveFile(m_bDeleteComplete, m_nPopupTime, m_bPopupConnect, m_bPopupDisconnect, m_bPopupCompleteDownload);
}


void CPreferencesTabDisplay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTabDisplay)
	DDX_Check(pDX, IDC_CHECK_DELETE_COMPLETE, m_bDeleteComplete);
	DDX_Text(pDX, IDC_EDIT_POPUPTIME, m_nPopupTime);
	DDX_Check(pDX, IDC_CHECK_CONNECT, m_bPopupConnect);
	DDX_Check(pDX, IDC_CHECK_DISCONNECT, m_bPopupDisconnect);
	DDX_Check(pDX, IDC_CHECK_DOWNLOAD, m_bPopupCompleteDownload);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesTabDisplay, CDialog)
	//{{AFX_MSG_MAP(CPreferencesTabDisplay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabDisplay message handlers

BOOL CPreferencesTabDisplay::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// ȯ�漳���� ���Ͽ��� �ε�
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->DisplayLoadFile(&m_bDeleteComplete, &m_nPopupTime, &m_bPopupConnect, &m_bPopupDisconnect, &m_bPopupCompleteDownload);
	UpdateData(FALSE);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




