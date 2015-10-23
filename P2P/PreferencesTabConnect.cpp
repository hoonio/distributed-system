// PreferencesTabConnect.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "PreferencesTabConnect.h"

#include "PreferencesValue.h"	// 환경설정 값
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabConnect dialog


CPreferencesTabConnect::CPreferencesTabConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesTabConnect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesTabConnect)
	m_nDownload = 0;
	m_nUpload = 0;
	m_strStaticDownload = _T("");
	m_strStaticUpload = _T("");
	//}}AFX_DATA_INIT
}

// Destroy 에서 처리가 안되서 소멸자에서 처리
CPreferencesTabConnect::~CPreferencesTabConnect()
{
	// 환경설정을 파일에 쓰기
	UpdateData(TRUE);
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->ConnectSaveFile(m_nDownload, m_nUpload);
}

void CPreferencesTabConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTabConnect)
	DDX_Control(pDX, IDC_SLIDER_UPLOAD, m_SliderUpload);
	DDX_Control(pDX, IDC_SLIDER_DOWNLOAD, m_SliderDownload);
	DDX_Slider(pDX, IDC_SLIDER_DOWNLOAD, m_nDownload);
	DDX_Slider(pDX, IDC_SLIDER_UPLOAD, m_nUpload);
	DDX_Text(pDX, IDC_STATIC_DOWNLOAD, m_strStaticDownload);
	DDX_Text(pDX, IDC_STATIC_UPLOAD, m_strStaticUpload);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesTabConnect, CDialog)
	//{{AFX_MSG_MAP(CPreferencesTabConnect)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DOWNLOAD, OnCustomdrawSliderDownload)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_UPLOAD, OnCustomdrawSliderUpload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabConnect message handlers



BOOL CPreferencesTabConnect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// 환경설정값 파일에서 로드
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->ConnectLoadFile((UINT *)&m_nDownload, (UINT *)&m_nUpload);
	UpdateData(FALSE);

	m_SliderDownload.SetRange(0, 500);
	m_SliderDownload.SetPos(m_nDownload);
	m_strStaticDownload.Format("%d kb/s", m_nDownload);

	m_SliderUpload.SetRange(0, 500);
	m_SliderUpload.SetPos(m_nUpload);
	m_strStaticUpload.Format("%d kb/s", m_nUpload);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CPreferencesTabConnect::OnCustomdrawSliderDownload(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_nDownload = m_SliderDownload.GetPos();
	m_strStaticDownload.Format("%d kb/s", m_nDownload);
	UpdateData(FALSE);	
	*pResult = 0;
}

void CPreferencesTabConnect::OnCustomdrawSliderUpload(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_nUpload = m_SliderUpload.GetPos();
	m_strStaticUpload.Format("%d kb/s", m_nUpload);
	UpdateData(FALSE);	
	*pResult = 0;
}
