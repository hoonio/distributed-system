// ConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "ConnectDlg.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog


CConnectDlg::CConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectDlg)
	m_strServerName = _T("");
	m_strUserID = _T("");
	m_nClient = -1;
	m_bViewFolder = FALSE;
	//}}AFX_DATA_INIT

	// UserCode
	m_pARead = NULL;
	m_pAWrite = NULL;
	m_pFile = NULL;
	m_nListIndex = 0;
}


void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	DDX_Control(pDX, IDC_LIST_SERVER, m_listServer);
	DDX_Control(pDX, IDC_IPADDRESS_SERVERIP, m_ipAddressServerIP);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_strServerName);
	DDX_Text(pDX, IDC_EDIT_USERID, m_strUserID);
	DDX_Radio(pDX, IDC_RADIO_CLIENT, m_nClient);
	DDX_Check(pDX, IDC_CHECK_VIEWFOLDER, m_bViewFolder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_BN_CLICKED(IDC_RADIO_CLIENT, OnRadioClient)
	ON_BN_CLICKED(IDC_RADIO_SERVER, OnRadioServer)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_SERVER, OnClickListServer)
	ON_EN_CHANGE(IDC_EDIT_SERVERNAME, OnChangeEditServername)
	ON_EN_CHANGE(IDC_EDIT_USERID, OnChangeEditUserid)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_SERVERIP, OnFieldchangedIpaddressServerip)
	ON_BN_CLICKED(IDC_CHECK_VIEWFOLDER, OnCheckViewfolder)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER, OnRclickListServer)
	ON_COMMAND(ID_SERVER_LIBRARY_DELETE, OnServerLibraryDelete)
	ON_COMMAND(ID_SERVER_LIBRARY_INSERT, OnServerLibraryInsert)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SERVER, OnDblclkListServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

BOOL CConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_listServer.m_nType = CONNECTDLG;	// 리스트가 지신의 기능만을 처리할수 있는 타입값

	m_nClient = 0;	// 라디오박스
	m_bViewFolder = TRUE;	// 폴더공유
	UpdateData(false);
	
	m_listServer.InsertColumn(0, "", LVCFMT_CENTER, 22);	// 이미지
	m_listServer.InsertColumn(1, "Server Name", LVCFMT_LEFT, 116);
	m_listServer.InsertColumn(2, "Server IP", LVCFMT_LEFT, 110);
	m_listServer.InsertColumn(3, "User ID", LVCFMT_LEFT, 100);
	m_listServer.InsertColumn(4, "View Folder", LVCFMT_CENTER, 90);
	m_listServer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// 밑줄나오게 설정

	// 256 칼라 이상 적용(icon) 
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_LIST_LIBRARY_CONNECT);
	imageList.Create(16, 16, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(255, 255, 255)); 
	m_listServer.SetImageList(&imageList, LVSIL_SMALL);
	imageList.Detach();
	bitmap.Detach();
	/**
	// listCtrl 에 아이콘 256 칼라 넣기
	m_imageList.Create(16, 16, ILC_COLOR8 | ILC_MASK, 16, 1);
	HICON hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), 
								 MAKEINTRESOURCE(IDI_ICON_LIST_LIBRARY_CONNECT), 
								 IMAGE_ICON, SM_CXICON, SM_CYICON, LR_SHARED | LR_LOADTRANSPARENT);
	m_imageList.Add(hIcon);
	hIcon = (HICON)::LoadImage(AfxGetInstanceHandle(), 
								 MAKEINTRESOURCE(IDI_ICON_LIST_LIBRARY_DISCONNECT), 
								 IMAGE_ICON, SM_CXICON, SM_CYICON, LR_SHARED | LR_LOADTRANSPARENT);	
	m_imageList.Add(hIcon);
	m_listServer.SetImageList(&m_imageList, LVSIL_SMALL);
	**/

	// 파일로드
	LoadFile();

	// 처음에 데이타가 한개라도 있으면 첫번째 데이타를 보이게 
	if(m_listServer.GetItemCount() > 0){
		m_strServerName = m_listServer.GetItemText(0, 1);
		m_ipAddressServerIP.SetWindowText(m_listServer.GetItemText(0, 2));
		m_strUserID = m_listServer.GetItemText(0, 3);
		m_bViewFolder = (m_listServer.GetItemText(0, 4) == "YES" ? 1 : 0);
		UpdateData(FALSE);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDlg::OnRadioClient()	// 클라이언트
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(true);
	GetDlgItem(IDC_IPADDRESS_SERVERIP)->EnableWindow(true);
	
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
}




void CConnectDlg::OnRadioServer()	// 서버
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(false);
	GetDlgItem(IDC_IPADDRESS_SERVERIP)->EnableWindow(false);
		
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_USERID)->SetFocus();
}

// 목록새로 추가시
void CConnectDlg::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	AddItem("New Server", "", "", "YES", 1);	// 0번이 접속된이미지	
	m_nListIndex = m_listServer.GetItemCount() - 1;	// 현재 새로추가했다면 무조건 선택된위치는 count - 1
	
	// 컨트롤들 초기화
	m_strServerName = "";
	m_ipAddressServerIP.SetWindowText("");
	m_strUserID = "";
	m_bViewFolder = 1;
	UpdateData(FALSE);	
	
	m_listServer.EnsureVisible(m_nListIndex, TRUE);	// new 버튼을 누룰시 스크롤이 넘치면 스크롤을 아래로
	GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
}

// 서버접속처리
void CConnectDlg::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString ip;

	if(m_nClient == 0){	// 클라이언트
		if(m_strServerName == ""){
			MessageBox("서버이름을 확인하세요!");
			GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
			return;
		}

		m_ipAddressServerIP.GetWindowText(ip);
		if(ip == "0.0.0.0"){
			MessageBox("서버IP 를 확인하세요!");
			GetDlgItem(IDC_IPADDRESS_SERVERIP)->SetFocus();
			return;
		}
	}

	// 서버라도 이부분을 체크
	if(m_strUserID == ""){
		MessageBox("사용자 아이디를 확인하세요!");
		GetDlgItem(IDC_EDIT_USERID)->SetFocus();
		return;
	}

	// 소켓연결(접속처리)
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_strUserID = m_strUserID;
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_bIsClient = m_nClient;	// 클라이언트인지 서버인지
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_strSevrIP = ip;

	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_bViewFolder = m_bViewFolder;
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.InitSock();
	if(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_nErrorCode != 0){	// 접속실패시
		AfxMessageBox("접속에 실패했습니다\n다시 접속하십시요!");
		return;
	}

	// 접속할때 환경설정값을 넣어준다
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_cPreferencesValue.DirectoryLoadFile(&(pMain->m_cPreferencesValue.m_strFolderTemp), &(pMain->m_cPreferencesValue.m_strFolderInComing), 
		&(pMain->m_cPreferencesValue.m_strFolderShare));
	pMain->m_cPreferencesValue.DirectoryLoadFile(&(pMain->m_cPreferencesValue.m_strFolderTemp), &(pMain->m_cPreferencesValue.m_strFolderInComing), 
		&(pMain->m_cPreferencesValue.m_strFolderShare));
	pMain->m_cPreferencesValue.DisplayLoadFile(&(pMain->m_cPreferencesValue.m_bDeleteComplete), &(pMain->m_cPreferencesValue.m_nPopupTime), 
		&(pMain->m_cPreferencesValue.m_bPopupConnect), &(pMain->m_cPreferencesValue.m_bPopupDisconnect), &(pMain->m_cPreferencesValue.m_bPopupCompleteDownload));
	
	// 쓰레드에서 이폴더값을 접근을 못해서 여기서 값을넣어준다
	pMain->m_myIOCPSocket.m_strFolderInComing = pMain->m_cPreferencesValue.m_strFolderInComing;
	pMain->m_myIOCPSocket.m_strFolderTemp = pMain->m_cPreferencesValue.m_strFolderTemp;
	pMain->m_myIOCPSocket.m_strFolderShare = pMain->m_cPreferencesValue.m_strFolderShare;

	// 하드의 용량을 알아내서 status 바에 넣는다	
	CString strUNC;
	pMain->m_cPreferencesValue.DirectoryLoadFile(&pMain->m_cPreferencesValue.m_strFolderTemp, &pMain->m_cPreferencesValue.m_strFolderInComing, &pMain->m_cPreferencesValue.m_strFolderShare);
	strUNC.Format(pMain->m_cPreferencesValue.m_strFolderInComing.Left(1));
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
	strFree.Format("(%s:)%s.%s GB Free", strUNC.Left(1), strFree.Left(strFree.GetLength() - 2), strFree.Right(2));
	pMain->m_wndStatusBar.SetPaneText(1, strFree);

	// status 바, upload, download
	CString strStatus;
	pMain->m_cPreferencesValue.ConnectLoadFile(&pMain->m_cPreferencesValue.m_nDownloadSpeed, &pMain->m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d개 - %d kB/s", 0, pMain->m_cPreferencesValue.m_nUploadSpeed);
	pMain->m_wndStatusBar.SetPaneText(2, strStatus);
	strStatus.Format("%d개 - %d kB/s", 0, pMain->m_cPreferencesValue.m_nDownloadSpeed);
	pMain->m_wndStatusBar.SetPaneText(3, strStatus);


	EndDialog(IDYES);
}





void CConnectDlg::AddItem(CString name, CString ip, CString userID, CString view, UINT image)
{
	LV_ITEM a;
	a.iItem = m_listServer.GetItemCount();	// 삽입 위치
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// 실직적으로 표현될값
	a.iSubItem = 0;	// 열인덱스
	a.iImage = image;	// 이미지 인덱스, 0번이 접속된이미지
	a.stateMask = LVIS_STATEIMAGEMASK;	// 상태변화를 Mask 처리
	a.state = INDEXTOSTATEIMAGEMASK(1);	// 유효한 상태 비트
	//name.Format(" %s", name);
	a.pszText = "";	//(LPSTR)(LPCTSTR) name;	// 문자열 	
	m_listServer.InsertItem(&a);	
	m_listServer.SetItemText(a.iItem, 1, name);
	m_listServer.SetItemText(a.iItem, 2, ip);
	m_listServer.SetItemText(a.iItem, 3, userID);
	m_listServer.SetItemText(a.iItem, 4, view);
}

void CConnectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here

	// 닫기전에 파일에 저장
	SaveFile();

}

void CConnectDlg::LoadFile()
{
	CString path = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;	
	m_pFile = new CFile;
	CString tempPath;
	tempPath.Format("%s%s", path, FILENAME_SERVERLIBRARY);
	m_pFile->Open(tempPath, CFile::modeRead);
	m_pARead = new CArchive(m_pFile, CArchive::load);	

	// 데이타를 파일에 적을때 몇개인지 모르므로 가장앞에 데이타의 갯수를 적는다
	m_cServerInfo.m_bIsData = FALSE;	// 데이타가 아닌 숫자 출력
	try{	// 데이타가 없음시 경고창이 뜨므로 여기서 미리 예외처리
		m_cServerInfo.Serialize(*m_pARead);
	}catch(...){		
		;
	}
	int cnt = m_cServerInfo.m_nCount;

	CString temp;
	temp.Format("%d", cnt);
	//MessageBox(temp);
	
	m_cServerInfo.m_bIsData = TRUE;	// 데이타가 출력
	UINT image;	
	for(int i=0; i<cnt; i++){		
		m_cServerInfo.Serialize(*m_pARead);	
		m_pARead->Flush();
		image = 1;//rand() % 2;	// 0번이 접속된이미지
		AddItem(m_cServerInfo.m_strServerName, m_cServerInfo.m_strServerIP, 
			m_cServerInfo.m_strUserID, m_cServerInfo.m_strViewFolder, image);
	}

	m_pARead->Close();
	m_pFile->Close();
	delete m_pARead;
	delete m_pFile;
	m_pARead = NULL;
	m_pFile = NULL;
}

void CConnectDlg::SaveFile()
{
	CString path = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;	
	m_pFile = new CFile;
	CString tempPath;
	tempPath.Format("%s%s", path, FILENAME_SERVERLIBRARY);
	m_pFile->Open(tempPath, CFile::modeCreate | CFile::modeWrite);
	m_pAWrite = new CArchive(m_pFile, CArchive::store);
	int cnt = m_listServer.GetItemCount();
	
	// 데이타를 파일에 적을때 몇개인지 모르므로 가장앞에 데이타의 갯수를 적는다
	m_cServerInfo.m_bIsData = FALSE;	// 데이타가 아닌 숫자 입력
	m_cServerInfo.m_nCount = cnt;
	m_cServerInfo.Serialize(*m_pAWrite);

	m_cServerInfo.m_bIsData = TRUE;	// 데이타입력
	for(int i=0; i<cnt; i++){	
		m_cServerInfo.m_strServerName = m_listServer.GetItemText(i, 1);
		m_cServerInfo.m_strServerIP = m_listServer.GetItemText(i, 2);
		m_cServerInfo.m_strUserID = m_listServer.GetItemText(i, 3);
		m_cServerInfo.m_strViewFolder = m_listServer.GetItemText(i, 4);
		m_cServerInfo.Serialize(*m_pAWrite);	
	}
	m_pAWrite->Flush();

	// 닫기
	m_pAWrite->Close();
	m_pFile->Close();
	delete m_pAWrite;
	delete m_pFile;
	m_pAWrite = NULL;
	m_pFile = NULL;
}


// 서버이름 수정시
void CConnectDlg::OnChangeEditServername() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	ModifyEdit(1);
	
}

// UserID 수정시
void CConnectDlg::OnChangeEditUserid() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_strUserID.Find("[", 0) != -1 || m_strUserID.Find("]", 0) != -1){
		AfxMessageBox("'[' 문자와 ']' 문자는 사용할수 없습니다");
		m_strUserID = "";
		UpdateData(FALSE);
		return;
	}
	ModifyEdit(3);
}

// 아이피 수정시
void CConnectDlg::OnFieldchangedIpaddressServerip(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ModifyEdit(2);
	*pResult = 0;
}

// 체크박스 수정시
void CConnectDlg::OnCheckViewfolder() 
{
	// TODO: Add your control notification handler code here
	ModifyEdit(4);
}

// 데이트박스에서 수정했을시
void CConnectDlg::ModifyEdit(UINT n)
{
	UpdateData();

	LV_ITEM a;
	a.iItem = m_nListIndex;
	a.mask = LVIF_TEXT | LVIF_STATE;	// 실직적으로 표현될값
	a.iSubItem = 0;	// 열인덱스	
	switch(n){
		case 1:	// 서버이름 수정시
			m_listServer.SetItemText(a.iItem, n, this->m_strServerName);
			break;
		case 2:	// 서버아이피 수정시
			{
				CString temp;
				this->m_ipAddressServerIP.GetWindowText(temp);
				m_listServer.SetItemText(a.iItem, n, temp);
				break;
			}
		case 3:	// 사용자 아이디 수정시
			m_listServer.SetItemText(a.iItem, n, this->m_strUserID);
			break;
		case 4:	// 폴더보이기 체크박스 수정시		
			char temp[3];
			strcpy(temp, (this->m_bViewFolder == 1 ? "YES" : "NO"));
			m_listServer.SetItemText(a.iItem, n, temp);
			break;			
	}
}

// 리스트박스에서 오른쪽버튼눌렀을시
void CConnectDlg::OnRclickListServer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(this->m_nClient == 0){	// 클라이언트 선택시에만
		NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		int intNum = pNMListView->iItem;
		/**
		if(intNum == -1){
			return;
		}
		**/
		m_nListIndex = intNum;	// 현재 선택되었다면 현재의 위치저장

		// TrackPopupMenu
		CMenu obMenu;
		obMenu.LoadMenu(IDR_MENU_SERVER_LIBRARY);
		CMenu *pPopupMenu = obMenu.GetSubMenu(0);
		ASSERT(pPopupMenu);
		CPoint obCursorPoint = (0, 0);
		GetCursorPos(&obCursorPoint);
		if(0 >= m_listServer.GetSelectedCount()){
			pPopupMenu->EnableMenuItem(ID_SERVER_LIBRARY_DELETE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED); 
		}else{
			pPopupMenu->EnableMenuItem(ID_SERVER_LIBRARY_DELETE, MF_BYCOMMAND | MF_ENABLED);
		}
		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, obCursorPoint.x, obCursorPoint.y, this);
		
	}
	*pResult = 0;
}

// TrackPopupMenu
void CConnectDlg::OnServerLibraryDelete() 
{
	// TODO: Add your command handler code here
	m_listServer.MyDeleteItem();

}

// TrackPopupMenu
void CConnectDlg::OnServerLibraryInsert() 
{
	// TODO: Add your command handler code here
	OnButtonNew();
}


// 리스트 컨트롤에서 목록선택시
void CConnectDlg::OnClickListServer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(this->m_nClient == 0){	// 클라이언트 선택시에만
		NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		int intNum = pNMListView->iItem;
		if(intNum == -1){
			return;
		}

		m_nListIndex = intNum;	// 현재 선택되었다면 현재의 위치저장

		m_strServerName = m_listServer.GetItemText(intNum, 1);
		m_ipAddressServerIP.SetWindowText(m_listServer.GetItemText(intNum, 2));
		m_strUserID = m_listServer.GetItemText(intNum, 3);
		m_bViewFolder = (m_listServer.GetItemText(intNum, 4) == "YES" ? 1 : 0);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

// 리스트컨트롤 더블클릭시 접속
void CConnectDlg::OnDblclkListServer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(this->m_nClient == 0){	// 클라이언트 선택시에만
		NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		int intNum = pNMListView->iItem;
		if(intNum == -1){
			return;
		}

		m_nListIndex = intNum;	// 현재 선택되었다면 현재의 위치저장

		m_strServerName = m_listServer.GetItemText(intNum, 1);
		m_ipAddressServerIP.SetWindowText(m_listServer.GetItemText(intNum, 2));
		m_strUserID = m_listServer.GetItemText(intNum, 3);
		m_bViewFolder = (m_listServer.GetItemText(intNum, 4) == "YES" ? 1 : 0);
		UpdateData(FALSE);
		this->OnButtonConnect();	// 접속처리
	}
	*pResult = 0;
}

/**
// 서버가 접속중인지 확인한후 여기서 이미지를 교체
void CConnectDlg::OnButtonTest() 
{
	// TODO: Add your control notification handler code here
	int cnt = m_listServer.GetItemCount();
	LV_ITEM a;
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;
	a.state = INDEXTOSTATEIMAGEMASK(1);
	a.stateMask = LVIS_STATEIMAGEMASK;
	a.iSubItem = 0;	// 열인덱스
	for(int i=0; i<cnt; i++){
		a.iItem = i;
		m_listServer.GetItem(&a);
		a.iImage = 1;
		a.pszText = "";	
		m_listServer.SetItem(&a);	
	}
}
**/
