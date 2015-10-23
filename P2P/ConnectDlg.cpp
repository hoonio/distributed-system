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
	m_listServer.m_nType = CONNECTDLG;	// ����Ʈ�� ������ ��ɸ��� ó���Ҽ� �ִ� Ÿ�԰�

	m_nClient = 0;	// �����ڽ�
	m_bViewFolder = TRUE;	// ��������
	UpdateData(false);
	
	m_listServer.InsertColumn(0, "", LVCFMT_CENTER, 22);	// �̹���
	m_listServer.InsertColumn(1, "Server Name", LVCFMT_LEFT, 116);
	m_listServer.InsertColumn(2, "Server IP", LVCFMT_LEFT, 110);
	m_listServer.InsertColumn(3, "User ID", LVCFMT_LEFT, 100);
	m_listServer.InsertColumn(4, "View Folder", LVCFMT_CENTER, 90);
	m_listServer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// ���ٳ����� ����

	// 256 Į�� �̻� ����(icon) 
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_LIST_LIBRARY_CONNECT);
	imageList.Create(16, 16, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(255, 255, 255)); 
	m_listServer.SetImageList(&imageList, LVSIL_SMALL);
	imageList.Detach();
	bitmap.Detach();
	/**
	// listCtrl �� ������ 256 Į�� �ֱ�
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

	// ���Ϸε�
	LoadFile();

	// ó���� ����Ÿ�� �Ѱ��� ������ ù��° ����Ÿ�� ���̰� 
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

void CConnectDlg::OnRadioClient()	// Ŭ���̾�Ʈ
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(true);
	GetDlgItem(IDC_IPADDRESS_SERVERIP)->EnableWindow(true);
	
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
}




void CConnectDlg::OnRadioServer()	// ����
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(false);
	GetDlgItem(IDC_IPADDRESS_SERVERIP)->EnableWindow(false);
		
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_USERID)->SetFocus();
}

// ��ϻ��� �߰���
void CConnectDlg::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	AddItem("New Server", "", "", "YES", 1);	// 0���� ���ӵ��̹���	
	m_nListIndex = m_listServer.GetItemCount() - 1;	// ���� �����߰��ߴٸ� ������ ���õ���ġ�� count - 1
	
	// ��Ʈ�ѵ� �ʱ�ȭ
	m_strServerName = "";
	m_ipAddressServerIP.SetWindowText("");
	m_strUserID = "";
	m_bViewFolder = 1;
	UpdateData(FALSE);	
	
	m_listServer.EnsureVisible(m_nListIndex, TRUE);	// new ��ư�� ����� ��ũ���� ��ġ�� ��ũ���� �Ʒ���
	GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
}

// ��������ó��
void CConnectDlg::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString ip;

	if(m_nClient == 0){	// Ŭ���̾�Ʈ
		if(m_strServerName == ""){
			MessageBox("�����̸��� Ȯ���ϼ���!");
			GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
			return;
		}

		m_ipAddressServerIP.GetWindowText(ip);
		if(ip == "0.0.0.0"){
			MessageBox("����IP �� Ȯ���ϼ���!");
			GetDlgItem(IDC_IPADDRESS_SERVERIP)->SetFocus();
			return;
		}
	}

	// ������ �̺κ��� üũ
	if(m_strUserID == ""){
		MessageBox("����� ���̵� Ȯ���ϼ���!");
		GetDlgItem(IDC_EDIT_USERID)->SetFocus();
		return;
	}

	// ���Ͽ���(����ó��)
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_strUserID = m_strUserID;
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_bIsClient = m_nClient;	// Ŭ���̾�Ʈ���� ��������
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_strSevrIP = ip;

	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_bViewFolder = m_bViewFolder;
	((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.InitSock();
	if(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_nErrorCode != 0){	// ���ӽ��н�
		AfxMessageBox("���ӿ� �����߽��ϴ�\n�ٽ� �����Ͻʽÿ�!");
		return;
	}

	// �����Ҷ� ȯ�漳������ �־��ش�
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_cPreferencesValue.DirectoryLoadFile(&(pMain->m_cPreferencesValue.m_strFolderTemp), &(pMain->m_cPreferencesValue.m_strFolderInComing), 
		&(pMain->m_cPreferencesValue.m_strFolderShare));
	pMain->m_cPreferencesValue.DirectoryLoadFile(&(pMain->m_cPreferencesValue.m_strFolderTemp), &(pMain->m_cPreferencesValue.m_strFolderInComing), 
		&(pMain->m_cPreferencesValue.m_strFolderShare));
	pMain->m_cPreferencesValue.DisplayLoadFile(&(pMain->m_cPreferencesValue.m_bDeleteComplete), &(pMain->m_cPreferencesValue.m_nPopupTime), 
		&(pMain->m_cPreferencesValue.m_bPopupConnect), &(pMain->m_cPreferencesValue.m_bPopupDisconnect), &(pMain->m_cPreferencesValue.m_bPopupCompleteDownload));
	
	// �����忡�� ���������� ������ ���ؼ� ���⼭ �����־��ش�
	pMain->m_myIOCPSocket.m_strFolderInComing = pMain->m_cPreferencesValue.m_strFolderInComing;
	pMain->m_myIOCPSocket.m_strFolderTemp = pMain->m_cPreferencesValue.m_strFolderTemp;
	pMain->m_myIOCPSocket.m_strFolderShare = pMain->m_cPreferencesValue.m_strFolderShare;

	// �ϵ��� �뷮�� �˾Ƴ��� status �ٿ� �ִ´�	
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
	strFree = strFree.Mid(0, cnt - 7);	// 8�� �Ⱑ����Ʈ�� ��Ÿ����� �Ҽ����̻��� ��ġ���� 9�ڸ��� �Ⱑ����Ʈ(12.34 GB)
	strFree.Format("(%s:)%s.%s GB Free", strUNC.Left(1), strFree.Left(strFree.GetLength() - 2), strFree.Right(2));
	pMain->m_wndStatusBar.SetPaneText(1, strFree);

	// status ��, upload, download
	CString strStatus;
	pMain->m_cPreferencesValue.ConnectLoadFile(&pMain->m_cPreferencesValue.m_nDownloadSpeed, &pMain->m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d�� - %d kB/s", 0, pMain->m_cPreferencesValue.m_nUploadSpeed);
	pMain->m_wndStatusBar.SetPaneText(2, strStatus);
	strStatus.Format("%d�� - %d kB/s", 0, pMain->m_cPreferencesValue.m_nDownloadSpeed);
	pMain->m_wndStatusBar.SetPaneText(3, strStatus);


	EndDialog(IDYES);
}





void CConnectDlg::AddItem(CString name, CString ip, CString userID, CString view, UINT image)
{
	LV_ITEM a;
	a.iItem = m_listServer.GetItemCount();	// ���� ��ġ
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// ���������� ǥ���ɰ�
	a.iSubItem = 0;	// ���ε���
	a.iImage = image;	// �̹��� �ε���, 0���� ���ӵ��̹���
	a.stateMask = LVIS_STATEIMAGEMASK;	// ���º�ȭ�� Mask ó��
	a.state = INDEXTOSTATEIMAGEMASK(1);	// ��ȿ�� ���� ��Ʈ
	//name.Format(" %s", name);
	a.pszText = "";	//(LPSTR)(LPCTSTR) name;	// ���ڿ� 	
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

	// �ݱ����� ���Ͽ� ����
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

	// ����Ÿ�� ���Ͽ� ������ ����� �𸣹Ƿ� ����տ� ����Ÿ�� ������ ���´�
	m_cServerInfo.m_bIsData = FALSE;	// ����Ÿ�� �ƴ� ���� ���
	try{	// ����Ÿ�� ������ ���â�� �߹Ƿ� ���⼭ �̸� ����ó��
		m_cServerInfo.Serialize(*m_pARead);
	}catch(...){		
		;
	}
	int cnt = m_cServerInfo.m_nCount;

	CString temp;
	temp.Format("%d", cnt);
	//MessageBox(temp);
	
	m_cServerInfo.m_bIsData = TRUE;	// ����Ÿ�� ���
	UINT image;	
	for(int i=0; i<cnt; i++){		
		m_cServerInfo.Serialize(*m_pARead);	
		m_pARead->Flush();
		image = 1;//rand() % 2;	// 0���� ���ӵ��̹���
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
	
	// ����Ÿ�� ���Ͽ� ������ ����� �𸣹Ƿ� ����տ� ����Ÿ�� ������ ���´�
	m_cServerInfo.m_bIsData = FALSE;	// ����Ÿ�� �ƴ� ���� �Է�
	m_cServerInfo.m_nCount = cnt;
	m_cServerInfo.Serialize(*m_pAWrite);

	m_cServerInfo.m_bIsData = TRUE;	// ����Ÿ�Է�
	for(int i=0; i<cnt; i++){	
		m_cServerInfo.m_strServerName = m_listServer.GetItemText(i, 1);
		m_cServerInfo.m_strServerIP = m_listServer.GetItemText(i, 2);
		m_cServerInfo.m_strUserID = m_listServer.GetItemText(i, 3);
		m_cServerInfo.m_strViewFolder = m_listServer.GetItemText(i, 4);
		m_cServerInfo.Serialize(*m_pAWrite);	
	}
	m_pAWrite->Flush();

	// �ݱ�
	m_pAWrite->Close();
	m_pFile->Close();
	delete m_pAWrite;
	delete m_pFile;
	m_pAWrite = NULL;
	m_pFile = NULL;
}


// �����̸� ������
void CConnectDlg::OnChangeEditServername() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	ModifyEdit(1);
	
}

// UserID ������
void CConnectDlg::OnChangeEditUserid() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_strUserID.Find("[", 0) != -1 || m_strUserID.Find("]", 0) != -1){
		AfxMessageBox("'[' ���ڿ� ']' ���ڴ� ����Ҽ� �����ϴ�");
		m_strUserID = "";
		UpdateData(FALSE);
		return;
	}
	ModifyEdit(3);
}

// ������ ������
void CConnectDlg::OnFieldchangedIpaddressServerip(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ModifyEdit(2);
	*pResult = 0;
}

// üũ�ڽ� ������
void CConnectDlg::OnCheckViewfolder() 
{
	// TODO: Add your control notification handler code here
	ModifyEdit(4);
}

// ����Ʈ�ڽ����� ����������
void CConnectDlg::ModifyEdit(UINT n)
{
	UpdateData();

	LV_ITEM a;
	a.iItem = m_nListIndex;
	a.mask = LVIF_TEXT | LVIF_STATE;	// ���������� ǥ���ɰ�
	a.iSubItem = 0;	// ���ε���	
	switch(n){
		case 1:	// �����̸� ������
			m_listServer.SetItemText(a.iItem, n, this->m_strServerName);
			break;
		case 2:	// ���������� ������
			{
				CString temp;
				this->m_ipAddressServerIP.GetWindowText(temp);
				m_listServer.SetItemText(a.iItem, n, temp);
				break;
			}
		case 3:	// ����� ���̵� ������
			m_listServer.SetItemText(a.iItem, n, this->m_strUserID);
			break;
		case 4:	// �������̱� üũ�ڽ� ������		
			char temp[3];
			strcpy(temp, (this->m_bViewFolder == 1 ? "YES" : "NO"));
			m_listServer.SetItemText(a.iItem, n, temp);
			break;			
	}
}

// ����Ʈ�ڽ����� �����ʹ�ư��������
void CConnectDlg::OnRclickListServer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(this->m_nClient == 0){	// Ŭ���̾�Ʈ ���ýÿ���
		NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		int intNum = pNMListView->iItem;
		/**
		if(intNum == -1){
			return;
		}
		**/
		m_nListIndex = intNum;	// ���� ���õǾ��ٸ� ������ ��ġ����

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


// ����Ʈ ��Ʈ�ѿ��� ��ϼ��ý�
void CConnectDlg::OnClickListServer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(this->m_nClient == 0){	// Ŭ���̾�Ʈ ���ýÿ���
		NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		int intNum = pNMListView->iItem;
		if(intNum == -1){
			return;
		}

		m_nListIndex = intNum;	// ���� ���õǾ��ٸ� ������ ��ġ����

		m_strServerName = m_listServer.GetItemText(intNum, 1);
		m_ipAddressServerIP.SetWindowText(m_listServer.GetItemText(intNum, 2));
		m_strUserID = m_listServer.GetItemText(intNum, 3);
		m_bViewFolder = (m_listServer.GetItemText(intNum, 4) == "YES" ? 1 : 0);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

// ����Ʈ��Ʈ�� ����Ŭ���� ����
void CConnectDlg::OnDblclkListServer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(this->m_nClient == 0){	// Ŭ���̾�Ʈ ���ýÿ���
		NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		int intNum = pNMListView->iItem;
		if(intNum == -1){
			return;
		}

		m_nListIndex = intNum;	// ���� ���õǾ��ٸ� ������ ��ġ����

		m_strServerName = m_listServer.GetItemText(intNum, 1);
		m_ipAddressServerIP.SetWindowText(m_listServer.GetItemText(intNum, 2));
		m_strUserID = m_listServer.GetItemText(intNum, 3);
		m_bViewFolder = (m_listServer.GetItemText(intNum, 4) == "YES" ? 1 : 0);
		UpdateData(FALSE);
		this->OnButtonConnect();	// ����ó��
	}
	*pResult = 0;
}

/**
// ������ ���������� Ȯ������ ���⼭ �̹����� ��ü
void CConnectDlg::OnButtonTest() 
{
	// TODO: Add your control notification handler code here
	int cnt = m_listServer.GetItemCount();
	LV_ITEM a;
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;
	a.state = INDEXTOSTATEIMAGEMASK(1);
	a.stateMask = LVIS_STATEIMAGEMASK;
	a.iSubItem = 0;	// ���ε���
	for(int i=0; i<cnt; i++){
		a.iItem = i;
		m_listServer.GetItem(&a);
		a.iImage = 1;
		a.pszText = "";	
		m_listServer.SetItem(&a);	
	}
}
**/
