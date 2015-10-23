// PreferencesTabDirectory.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "PreferencesTabDirectory.h"

#include "PreferencesValue.h"	// 환경설정 값
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabDirectory dialog


CPreferencesTabDirectory::CPreferencesTabDirectory(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesTabDirectory::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesTabDirectory)
	m_strInComingFolder = _T("");
	m_strShareFolder = _T("");
	m_strTempFolder = _T("");
	m_staticFree = _T("");
	//}}AFX_DATA_INIT
}

// Destroy 에서 처리가 안되서 소멸자에서 처리
CPreferencesTabDirectory::~CPreferencesTabDirectory()
{
	// 환경설정을 파일에 쓰기
	UpdateData(TRUE);
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->DirectorySaveFile(&m_strTempFolder, &m_strInComingFolder, &m_strShareFolder);
}


void CPreferencesTabDirectory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTabDirectory)
	DDX_Text(pDX, IDC_EDIT_INCOMING, m_strInComingFolder);
	DDX_Text(pDX, IDC_EDIT_SHAREFOLDER, m_strShareFolder);
	DDX_Text(pDX, IDC_EDIT_TEMP, m_strTempFolder);
	DDX_Text(pDX, IDC_STATIC_FREE, m_staticFree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesTabDirectory, CDialog)
	//{{AFX_MSG_MAP(CPreferencesTabDirectory)
	ON_BN_CLICKED(IDC_BUTTON_TEMP, OnButtonTemp)
	ON_BN_CLICKED(IDC_BUTTON_INCOMING, OnButtonIncoming)
	ON_BN_CLICKED(IDC_BUTTON_SHAREFOLDER, OnButtonSharefolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTabDirectory message handlers



BOOL CPreferencesTabDirectory::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// 환경설정값 파일에서 로드
	CPreferencesValue *pPreferencesValue = &(((CMainFrame *)AfxGetMainWnd())->m_cPreferencesValue);		
	pPreferencesValue->DirectoryLoadFile(&m_strTempFolder, &m_strInComingFolder, &m_strShareFolder);
	UpdateData(FALSE);

	
	// 하드의 용량을 알아내서 status 바에 넣는다	
	UpdateData(TRUE);
	CString strUNC;
	strUNC.Format(m_strInComingFolder.Left(1));
	strUNC = strUNC + ":\\";
	ULARGE_INTEGER ulAvailable, ulTotal, ulFree;
	GetDiskFreeSpaceEx((LPCTSTR)strUNC, &ulAvailable, &ulTotal, &ulFree);
	double nFree, nTotal;
	if(ulAvailable.QuadPart){	
		nFree = ulAvailable.LowPart+(double)ulAvailable.HighPart*(double)4294967296;
		nTotal = ulTotal.LowPart+(double)ulTotal.HighPart*(double)4294967296;
	}
	CString strFree;
	strFree.Format("%f", nFree);
 	int cnt = strFree.Find(".", 0);
	strFree = strFree.Mid(0, cnt - 7);	// 8은 기가바이트로 나타낼경우 소수점이상의 수치에서 9자리가 기가바이트(12.34 GB)

	CString strTotal;
	strTotal.Format("%f", nTotal);
 	cnt = strTotal.Find(".", 0);
	strTotal = strTotal.Mid(0, cnt - 7);	// 8은 기가바이트로 나타낼경우 소수점이상의 수치에서 9자리가 기가바이트(12.34 GB)

	m_staticFree.Format("(%s:)%s.%s / %s.%s GB", strUNC.Left(1), strFree.Left(strFree.GetLength() - 2), strFree.Right(2), strTotal.Left(strTotal.GetLength() - 2), strTotal.Right(2));
	UpdateData(FALSE);



	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTabDirectory::OnButtonTemp() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(pMain->m_bConnectedFlag == 1){
		AfxMessageBox("접속중에는 변경할수 없습니다!\n접속해제후 변경하십시요!");
		return;
	}
	SelectDirectory(&m_strTempFolder);
}

void CPreferencesTabDirectory::OnButtonIncoming() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(pMain->m_bConnectedFlag == 1){
		AfxMessageBox("접속중에는 변경할수 없습니다!\n접속해제후 변경하십시요!");
		return;
	}
	SelectDirectory(&m_strInComingFolder);
}

void CPreferencesTabDirectory::OnButtonSharefolder() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(pMain->m_bConnectedFlag == 1){
		AfxMessageBox("접속중에는 변경할수 없습니다!\n접속해제후 변경하십시요!");
		return;
	}
	SelectDirectory(&m_strShareFolder);
}

void CPreferencesTabDirectory::SelectDirectory(CString *pStrEdit)
{
	BROWSEINFO bi;
	bi.hwndOwner = NULL;//m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = "Select Folder";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	char pszDisplayName[256];
	LPITEMIDLIST lpID = SHBrowseForFolder(&bi);		// ->다이얼로그박스 띠우는거
	if(lpID != NULL){
		CString temp="A:\\";
		if(SHGetPathFromIDList(lpID,pszDisplayName)){			
			if(pszDisplayName == temp){	
				AfxMessageBox("A:드라이브는 루트폴더로 사용할수 없습니다.다시 설정!");
			    return;
			}
			pStrEdit->Format("%s", pszDisplayName);
			UpdateData(FALSE);
		}		
	}	
}
