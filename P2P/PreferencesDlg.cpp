// PreferencesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "PreferencesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog


CPreferencesDlg::CPreferencesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nTabViewNum = 0;
	m_pPreferencesTabConnect = NULL;	// 접속
	m_pPreferencesTabDirectory = NULL;	// 디렉토리
	m_pPreferencesTabDisplay = NULL;	// 디스플레이
}




void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDlg)
	DDX_Control(pDX, IDC_TAB_OPTION, m_tabOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OPTION, OnSelchangeTabOption)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg message handlers

BOOL CPreferencesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	

	// 256 칼라 이상 적용(icon) 
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_PREFERENCE_TAB);
	imageList.Create(16, 16, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(255, 255, 255)); 
	//m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // 활성이미지
	m_tabOption.SetImageList(&imageList);
	imageList.Detach();
	bitmap.Detach();
	
	// tab
	m_tabOption.InsertItem(0, "접속", 1);
	m_tabOption.InsertItem(1, "디렉토리", 2);
	m_tabOption.InsertItem(2, "디스플레이", 3);
	
	m_tabOption.SetCurSel(m_nTabViewNum);
	TabView();

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesDlg::TabView()
{
	CRect rect;
	switch(m_nTabViewNum){
		case 0:	// 접속
			m_pPreferencesTabConnect = new CPreferencesTabConnect;	
			m_pPreferencesTabConnect->Create(IDD_TAB_PREFERENCE_CONNECT, &m_tabOption);
			m_pPreferencesTabConnect->GetClientRect(&rect);
			m_pPreferencesTabConnect->MoveWindow(5, 23, rect.Width(), rect.Height());
			m_pPreferencesTabConnect->ShowWindow(SW_SHOW);
			break;
		case 1:	// 디렉토리
			m_pPreferencesTabDirectory = new CPreferencesTabDirectory;	
			m_pPreferencesTabDirectory->Create(IDD_TAB_PREFERENCE_DIRECTORY, &m_tabOption);
			m_pPreferencesTabDirectory->GetClientRect(&rect);
			m_pPreferencesTabDirectory->MoveWindow(5, 23, rect.Width(), rect.Height());
			m_pPreferencesTabDirectory->ShowWindow(SW_SHOW);
			break;
		case 2:	// 디스플레이
			m_pPreferencesTabDisplay = new CPreferencesTabDisplay;	
			m_pPreferencesTabDisplay->Create(IDD_TAB_PREFERENCE_DISPLAY, &m_tabOption);
			m_pPreferencesTabDisplay->GetClientRect(&rect);
			m_pPreferencesTabDisplay->MoveWindow(5, 23, rect.Width(), rect.Height());
			m_pPreferencesTabDisplay->ShowWindow(SW_SHOW);
			break;
	}

	// 선택이 해제된것은 정리
	if(m_nTabViewNum != 0){	// 접속
		if(m_pPreferencesTabConnect != NULL){
			m_pPreferencesTabConnect->ShowWindow(SW_HIDE);
			delete m_pPreferencesTabConnect;
			m_pPreferencesTabConnect = NULL;
		}
	}
	if(m_nTabViewNum != 1){	// 디렉토리
		if(m_pPreferencesTabDirectory != NULL){
			m_pPreferencesTabDirectory->ShowWindow(SW_HIDE);
			delete m_pPreferencesTabDirectory;
			m_pPreferencesTabDirectory = NULL;
		}
	}
	if(m_nTabViewNum != 2){	// 디스플레이
		if(m_pPreferencesTabDisplay != NULL){
			m_pPreferencesTabDisplay->ShowWindow(SW_HIDE);
			delete m_pPreferencesTabDisplay;
			m_pPreferencesTabDisplay = NULL;
		}
	}

}




void CPreferencesDlg::OnSelchangeTabOption(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_nTabViewNum = m_tabOption.GetCurSel();
	TabView();	// 화면전환
	
	/**
	CString temp;
	temp.Format("%d", m_nTabViewNum);
	SetWindowText(temp);
	**/

	*pResult = 0;
}



void CPreferencesDlg::FirstSetCurSel(UINT num)
{
	m_nTabViewNum = num;
}

void CPreferencesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pPreferencesTabConnect != NULL){
		m_pPreferencesTabConnect->ShowWindow(SW_HIDE);
		delete m_pPreferencesTabConnect;
		m_pPreferencesTabConnect = NULL;
	}
	if(m_pPreferencesTabDirectory != NULL){
		m_pPreferencesTabDirectory->ShowWindow(SW_HIDE);
		delete m_pPreferencesTabDirectory;
		m_pPreferencesTabDirectory = NULL;
	}
	if(m_pPreferencesTabDisplay != NULL){
		m_pPreferencesTabDisplay->ShowWindow(SW_HIDE);
		delete m_pPreferencesTabDisplay;
		m_pPreferencesTabDisplay = NULL;
	}	
}
