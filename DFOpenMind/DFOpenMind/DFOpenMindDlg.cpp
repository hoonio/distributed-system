// DFOpenMindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFOpenMindDlg.h"
#include "DFTransferStatusDlg.h"
#include "DFHideMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DFHideMainFrame* g_pMainFrame;
extern REG_CONFIG_INFO g_Config;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindDlg dialog

CDFOpenMindDlg::CDFOpenMindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDFOpenMindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFOpenMindDlg)
	m_MaxDate = COleDateTime::GetCurrentTime();
	m_MinDate = COleDateTime::GetCurrentTime();
	m_strName = _T("");
	m_strExtension = _T("");
	m_strUser = _T("");
	m_bDate = FALSE;
	m_bExtension = FALSE;
	m_bName = TRUE;
	m_bSize = FALSE;
	m_bUser = FALSE;
	m_strMinSize = _T("");
	m_strMaxSize = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDFOpenMindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFOpenMindDlg)
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
	DDX_Control(pDX, IDC_USERTREE, m_UserTree);
	DDX_DateTimeCtrl(pDX, IDC_DATE_MAX, m_MaxDate);
	DDX_DateTimeCtrl(pDX, IDC_DATE_MIN, m_MinDate);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 32);
	DDX_Text(pDX, IDC_EXTENTION, m_strExtension);
	DDV_MaxChars(pDX, m_strExtension, 3);
	DDX_CBString(pDX, IDC_USER, m_strUser);
	DDV_MaxChars(pDX, m_strUser, 12);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bDate);
	DDX_Check(pDX, IDC_CHECK_EXTENTION, m_bExtension);
	DDX_Check(pDX, IDC_CHECK_NAME, m_bName);
	DDX_Check(pDX, IDC_CHECK_SIZE, m_bSize);
	DDX_Check(pDX, IDC_CHECK_USER, m_bUser);
	DDX_Text(pDX, IDC_MINSIZE, m_strMinSize);
	DDV_MaxChars(pDX, m_strMinSize, 7);
	DDX_Text(pDX, IDC_MAXSIZE, m_strMaxSize);
	DDV_MaxChars(pDX, m_strMaxSize, 7);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDFOpenMindDlg, CDialog)
	//{{AFX_MSG_MAP(CDFOpenMindDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_STATUS, OnStatus)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_DOWNLOAD, OnDownload)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_CHECK_EXTENTION, OnCheckExtention)
	ON_BN_CLICKED(IDC_CHECK_NAME, OnCheckName)
	ON_BN_CLICKED(IDC_CHECK_SIZE, OnCheckSize)
	ON_BN_CLICKED(IDC_CHECK_USER, OnCheckUser)
	ON_BN_CLICKED(IDC_RADIO_ACCESS, OnRadioAccess)
	ON_BN_CLICKED(IDC_RADIO_CREATION, OnRadioCreation)
	ON_BN_CLICKED(IDC_RADIO_MODIFY, OnRadioModify)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_NOTIFY(NM_DBLCLK, IDC_USERTREE, OnDblclkUsertree)
//	ON_NOTIFY(HDN_ITEMDBLCLICK, IDC_FILELIST, OnItemdblclickFilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnDblclkFilelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFOpenMindDlg message handlers

BOOL CDFOpenMindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// 다이얼로그가 뜨면서 트리와 리스트컨트롤, 체크 박스, 라디오 버튼, 버튼 등을 초기화 한다.

	m_bRadioSelect = 0;
	
	// 파일 리스트 컨트롤의 컬럼을 세팅한다.
	m_FileList.InsertColumn(0, TEXT("File Name"), LVCFMT_CENTER, 100);
	m_FileList.InsertColumn(1, TEXT("UserID"), LVCFMT_CENTER, 100);
	m_FileList.InsertColumn(2, TEXT("Size"), LVCFMT_CENTER, 80);
	m_FileList.InsertColumn(3, TEXT("Creation Time"), LVCFMT_CENTER, 100);
	m_FileList.InsertColumn(4, TEXT("Last Access Time"), LVCFMT_CENTER, 100);
	m_FileList.InsertColumn(5, TEXT("Last Write Time"), LVCFMT_CENTER, 100);

	// 트리의 최상위 부모 노드를 세팅한다.
	m_hRootItem = m_UserTree.InsertItem(TEXT("Share Folders"));
	m_hMyItem	= m_UserTree.InsertItem(TEXT("My Share"), m_hRootItem);
	m_hOtherItem= m_UserTree.InsertItem(TEXT("Other User's"), m_hRootItem);
	m_UserTree.Expand(m_hRootItem, TVE_EXPAND);

	// Connect 버튼 활성화 여부를 결정한다.
	if(g_pMainFrame->m_bConnected)
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);

	// 라디오 버튼을 세팅한다.
	GetDlgItem(IDC_RADIO_CREATION)->
		CheckRadioButton(IDC_RADIO_CREATION, IDC_RADIO_MODIFY, IDC_RADIO_CREATION);   

	// 유저 tree를 초기화한다. MainFrame의 유저 리스트로부터 받아온다.
	RefleshUserTree();
	RemoveAllList();

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDFOpenMindDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDFOpenMindDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDFOpenMindDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// SHOW -> HIDE 경우 처리.
void CDFOpenMindDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

// HIDE -> SHOW 경우 처리.
void CDFOpenMindDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void CDFOpenMindDlg::OnExit() 
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("Do You Want To Exit DFOpenMind?", MB_YESNO|MB_DEFBUTTON2) == IDYES)
	{
		EndDialog(IDC_EXIT);
	}
	
}

void CDFOpenMindDlg::OnConfig() 
{
	// TODO: Add your control notification handler code here
	CString strMsg;
	strMsg.Format(TEXT("Configuration is not supported yet"));
	MessageBox(strMsg, TEXT("DFOpenMind Configuration"), MB_OK|MB_ICONINFORMATION);	
}

void CDFOpenMindDlg::OnStatus() 
{
	// TODO: Add your control notification handler code here
	CDFTransferStatusDlg StatusDlg;
	StatusDlg.DoModal();
}

void CDFOpenMindDlg::OnConnect() 
{
	// TODO: Add your control notification handler code here
	g_pMainFrame->ConnectToServer((LPCTSTR)g_pMainFrame->m_strServerIP);
	
}

void CDFOpenMindDlg::RefleshUserTree()
{
	if(m_UserTree.ItemHasChildren(m_hOtherItem))
	{
		HTREEITEM hItem = m_UserTree.GetChildItem(m_hOtherItem);

		while(hItem != NULL)
		{
			TVITEM item;
			TCHAR szText[1024];
			item.hItem = hItem;
			item.mask = TVIF_TEXT | TVIF_HANDLE;
			item.pszText = szText;
			item.cchTextMax = 1024;

			BOOL bWorked = m_UserTree.GetItem(&item);
			
			hItem = m_UserTree.GetNextSiblingItem(hItem);
			m_UserTree.DeleteItem(item.hItem);
		}
	}

	CString				Key;
	PCLIENT_USER_INFO	pUserInfo;
	POSITION pos = g_pMainFrame->m_UserMap.GetStartPosition();
	while(pos)
	{
		g_pMainFrame->m_UserMap.GetNextAssoc(pos, Key, (void*&)pUserInfo);
		if(pUserInfo) AddUser(pUserInfo->tszUserID);	
	}
}

void CDFOpenMindDlg::AddUser(LPCTSTR tszUserID)
{
	m_UserTree.InsertItem(tszUserID, m_hOtherItem);
}

void CDFOpenMindDlg::DeleteUser(LPCTSTR tszUserID)
{
	if(m_UserTree.ItemHasChildren(m_hOtherItem))
	{
		HTREEITEM hItem = m_UserTree.GetChildItem(m_hOtherItem);

		while(hItem != NULL)
		{
			TVITEM item;
			TCHAR szText[1024];
			item.hItem = hItem;
			item.mask = TVIF_TEXT | TVIF_HANDLE;
			item.pszText = szText;
			item.cchTextMax = 1024;

			BOOL bWorked = m_UserTree.GetItem(&item);

			if(bWorked && lstrcmpi(item.pszText, tszUserID) == 0)
			{
				m_UserTree.DeleteItem(item.hItem);
				break;
			}
			hItem = m_UserTree.GetNextSiblingItem(hItem);
		}
	}
}

void CDFOpenMindDlg::RemoveAllList()
{
	m_FileList.DeleteAllItems();
}

void CDFOpenMindDlg::AddFile(PFILE_INFO pFileInfo)
{
	CString strTemp;
	CTime CreationTime(pFileInfo->ftCreationTime);
	CTime AccessTime(pFileInfo->ftLastAccessTime);
	CTime WriteTime(pFileInfo->ftLastWriteTime);

	// 새로운 아이템을 써 넣는다.
	int i = m_FileList.GetItemCount();
	m_FileList.InsertItem(i, pFileInfo->tszFileName);
	m_FileList.SetItemText(i, 1, pFileInfo->tszUserID);
	strTemp.Format(TEXT("%d"), pFileInfo->dwFileSizeLow);
	m_FileList.SetItemText(i, 2, strTemp);
	
	m_FileList.SetItemText(i, 3, CreationTime.Format(TEXT("%y-%m-%d %H:%M:%S")));
	m_FileList.SetItemText(i, 4, AccessTime.Format(TEXT("%y-%m-%d %H:%M:%S")));
	m_FileList.SetItemText(i, 5, WriteTime.Format(TEXT("%y-%m-%d %H:%M:%S")));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDFOpenMindDlg::OnCheckDate() 
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if(m_bDate)
	{
		GetDlgItem(IDC_RADIO_CREATION)->EnableWindow();
		GetDlgItem(IDC_RADIO_ACCESS)->EnableWindow();
		GetDlgItem(IDC_RADIO_MODIFY)->EnableWindow();

		GetDlgItem(IDC_DATE_MIN)->EnableWindow();
		GetDlgItem(IDC_DATE_MAX)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_RADIO_CREATION)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_ACCESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MODIFY)->EnableWindow(FALSE);

		GetDlgItem(IDC_DATE_MIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATE_MAX)->EnableWindow(FALSE);
	}
}

void CDFOpenMindDlg::OnCheckExtention() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_bExtension)
	{
		GetDlgItem(IDC_EXTENTION)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_EXTENTION)->EnableWindow(FALSE);
	}
}

void CDFOpenMindDlg::OnCheckName() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_bName)
	{
		GetDlgItem(IDC_NAME)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_NAME)->EnableWindow(FALSE);
	}	
}

void CDFOpenMindDlg::OnCheckSize() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_bSize)
	{
		GetDlgItem(IDC_MINSIZE)->EnableWindow();
		GetDlgItem(IDC_MAXSIZE)->EnableWindow();
		GetDlgItem(IDC_MINSIZE)->SetWindowText(TEXT("0"));
		GetDlgItem(IDC_MAXSIZE)->SetWindowText(TEXT("0"));
	}
	else
	{
		GetDlgItem(IDC_MINSIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MAXSIZE)->EnableWindow(FALSE);
	}	
}

void CDFOpenMindDlg::OnCheckUser() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_bUser)
	{
		GetDlgItem(IDC_USER)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_USER)->EnableWindow(FALSE);
	}
}

void CDFOpenMindDlg::OnRadioAccess() 
{
	// TODO: Add your control notification handler code here
	m_bRadioSelect = FIND_ACCESS_TIME;
	
}

void CDFOpenMindDlg::OnRadioCreation() 
{
	// TODO: Add your control notification handler code here
	m_bRadioSelect = FIND_CREATION_TIME;
}

void CDFOpenMindDlg::OnRadioModify() 
{
	// TODO: Add your control notification handler code here
	m_bRadioSelect = FIND_MODIFY_TIME;
}

void CDFOpenMindDlg::OnSearch() 
{
	// TODO: Add your control notification handler code here
	FILE_FIND_INFO FindInfo;

	memset(&FindInfo, 0, sizeof(FindInfo));

	// 파일 정보 리스트를 초기화한다!!
	RemoveAllList();

	// 구조체를 세팅한다.
	UpdateData();

	if(!(m_bUser || m_bName || m_bExtension || m_bDate || m_bSize))
	{
		MessageBox(TEXT("Must Select Search Condition."), TEXT("Search Error"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	// 탐색 조건을 명시한다.
	if(m_bUser) 
	{
		if(m_strUser.GetLength() == 0)
		{
			MessageBox(TEXT("Illegal UserID"), TEXT("Search Error"), MB_OK|MB_ICONINFORMATION);
			return;
		}
		lstrcpy(FindInfo.tszUserID, m_strUser);
		FindInfo.Methods.base_methods |= FIND_USER_ID;
	}
	if(m_bName) 
	{
		if(m_strName.GetLength() == 0)
		{
			MessageBox(TEXT("Illegal FileName Portion"), TEXT("Search Error"), MB_OK|MB_ICONINFORMATION);
			return;
		}
		lstrcpy(FindInfo.tszFileNamePortion, m_strName);
		FindInfo.Methods.base_methods |= FIND_NAME;
	}
	if(m_bExtension) 
	{
		lstrcpy(FindInfo.tszExtention, m_strExtension);
		FindInfo.Methods.base_methods |= FIND_EXTENSION;
	}
	if(m_bDate)
	{
		COleDateTime Cur(COleDateTime::GetCurrentTime());
		
		if((m_MinDate > m_MaxDate) || (m_MaxDate > Cur))
		{
			MessageBox(TEXT("Illegal DateTime"), TEXT("Search Error"), MB_ICONINFORMATION|MB_OK);
			return;
		}
		FindInfo.Methods.base_methods |= FIND_TIME;
		FindInfo.Methods.valid_check |= FIND_MIN_TIME;
		FindInfo.Methods.valid_check |= FIND_MAX_TIME;
		FindInfo.Methods.time_methods = m_bRadioSelect;

		ConvertTimeToFileTime(m_MaxDate, FindInfo.ftMaxTime);
		ConvertTimeToFileTime(m_MinDate, FindInfo.ftMinTime);
	}
	if(m_bSize)
	{
		if(	(atol((LPCTSTR)m_strMinSize) > atol((LPCTSTR)m_strMaxSize)) ||
			(atol((LPCTSTR)m_strMinSize) == 0 && atol((LPCTSTR)m_strMaxSize) ==0))
		{
			MessageBox(TEXT("Illegal File Size"), TEXT("Search Error"), MB_ICONINFORMATION|MB_OK);
			return;
		}

		FindInfo.Methods.base_methods |= FIND_SIZE;
		if(atol((LPCTSTR)m_strMinSize) != 0)
			FindInfo.Methods.valid_check |= FIND_MIN_SIZE;
		if(atol((LPCTSTR)m_strMaxSize) != 0)
			FindInfo.Methods.valid_check |= FIND_MAX_SIZE;

		FindInfo.dwMinFileSizeLow = atol((LPCTSTR)m_strMinSize);
		FindInfo.dwMaxFileSizeLow = atol((LPCTSTR)m_strMaxSize);
	}

	// 탐색 메시지 송신!!
	g_pMainFrame->SendSearchMsg(FindInfo);
}

void CDFOpenMindDlg::ConvertTimeToFileTime(COleDateTime Time, FILETIME& FileTime)
{
	// Convert CTime to FILETIME
	SYSTEMTIME timeDest;
	Time.GetAsSystemTime(timeDest);
	::SystemTimeToFileTime(&timeDest, &FileTime);
}


void CDFOpenMindDlg::OnDownload() 
{
	// TODO: Add your control notification handler code here
	// File List에서 선택된 리스트를 읽어온다.
	// 이후 파일 수신 클라이언트를 선택된 수만큼 생성하며, 
	// 다른 클라이언트와 통신하도록 한다.

	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = m_FileList.GetNextSelectedItem(pos);
		
		// 유저와 파일이름만 알면 다운로드 가능하다.
		CString strFileName = m_FileList.GetItemText(nItem, 0);
		CString strUserID = m_FileList.GetItemText(nItem, 1);

		// 자신의 것은 다운로드 불가..
		if(strUserID == g_pMainFrame->m_strUserName) 
		{
			MessageBox(TEXT("Local File Download!!"));
			return;
		}

		Download(strUserID, strFileName);
	}
	
}

void CDFOpenMindDlg::OnDblclkUsertree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	// The pointer to my tree control.

	HTREEITEM hItem = m_UserTree.GetSelectedItem();

	if(hItem != NULL) 
	{
		CString strText = m_UserTree.GetItemText(hItem);
		if(lstrcmp(strText, TEXT("My Share")) == 0)
		{
			// 현재 유저 id로 공유된 폴더의 파일 정보를 뿌린다.
			PFILE_INFO pFileInfo;

			RemoveAllList();
			
			POSITION pos = g_pMainFrame->m_LocalFileList.GetHeadPosition();
			while(pos)
			{
				pFileInfo = (PFILE_INFO)g_pMainFrame->m_LocalFileList.GetNext(pos);
				AddFile(pFileInfo);
			}
		}
		else if(lstrcmp(strText, TEXT("Share Folders")) != 0 && 
				lstrcmp(strText, TEXT("Other User's")) != 0)
		{
			// 선택된 유저의 모든 공유 파일 정보 송신을 요청한다.
			RemoveAllList();
			FILE_FIND_INFO FindInfo;

			memset(&FindInfo, 0, sizeof(FindInfo));
			lstrcpy(FindInfo.tszUserID, strText);
			FindInfo.Methods.base_methods |= FIND_USER_ID;

			// 탐색 메시지 송신!!
			g_pMainFrame->SendSearchMsg(FindInfo);
		}
	}
	
	*pResult = 0;
}

void CDFOpenMindDlg::Download(CString strUserID, CString strFileName)
{
	// strUserID로 해당 클라이언트의 IP를 알아낸다.
	// 이후 접속하여 strFileName인 파일을 다운로드 받는다.
	PCLIENT_USER_INFO pUserInfo = NULL;

	g_pMainFrame->m_UserMap.Lookup(strUserID, (void*&)pUserInfo);
	if(pUserInfo)
	{
		if(g_Config.btMaxDownloads < g_pMainFrame->m_DownList.GetCount() + 1) return;

		DFFileRecver* pRecver = new DFFileRecver(pUserInfo->dwIP, (LPCTSTR)strFileName, (LPCTSTR)strUserID);
		g_pMainFrame->m_DownList.AddTail(pRecver);
	}
}

void CDFOpenMindDlg::OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnDownload();
	*pResult = 0;
}
