// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "P2p.h"

#include "MainFrm.h"


#include "ConnectDlg.h"	// ��������
#include "PreferencesDlg.h"	// ȯ�漳��

#include "SearchView.h"	// �˻�
#include "DownloadsView.h"	// �ٿ�ε�
#include "UploadsView.h"	// ���ε�
#include "ServerView.h"	// ����




#define  UM_ICON_NOTIFY  WM_USER+1



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_SERVER_CONNECT, OnServerConnect)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENU_SEARCH, OnMenuSearch)
	ON_COMMAND(ID_MENU_SERVER, OnMenuServer)
	ON_COMMAND(ID_MENU_UPLOADS, OnMenuUploads)
	ON_COMMAND(ID_MENU_DOWNLOADS, OnMenuDownloads)
	ON_COMMAND(ID_MENU_CHAT, OnMenuChat)
	ON_COMMAND(ID_MENU_PREFERENCES, OnMenuPreferences)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_SERVER_CONNECT, OnMenuServerConnect)
	ON_COMMAND(ID_MENU_SERVER_DISCONNECT, OnMenuServerDisconnect)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_ICON_NOTIFY, OnTrayNotification)	// Ʈ���̾�����
	ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_SERVER_CONNECT, ID_MENU_SERVER_DISCONNECT, OnMenuConnectUpdate)	// UPDATE_COMMAND_UI
	ON_MESSAGE(WM_TASKBARNOTIFIERCLICKED,OnTaskbarNotifierClicked)	// �˸�â(MSN)
	ON_MESSAGE(UM_SETPANETEXT, OnSetPaneText)	// ������������ statusbar ����
	ON_MESSAGE(UM_POPUPMESSAGE, OnPopupMessage)	// �˾��޽���
	ON_MESSAGE(UM_ADDITEMSEARCHFILE, OnAddItemSearchFile)	// IOCP���� ã�����������⼭ó��
	ON_MESSAGE(UM_DOWNLOAD_COMPLETE, OnDownloadComplete)	// �ٿ�Ϸ�
	ON_MESSAGE(UM_UPLOAD_COMPLETE, OnUploadComplete)	// �ٿ�Ϸ�
	ON_MESSAGE(UM_ADD_UPLOAD_DATA, OnAddUploadData)	// ���ε嵥��Ÿ�߰�
	ON_MESSAGE(UM_ADD_DOWNLOAD_DATA, OnAddDownloadData)	// �ٿ�ε嵥��Ÿ�߰�
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	/**
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	**/
	ID_INDICATOR_EXT,	// �ϵ峲���뷮
	ID_INDICATOR_EXT,	// ���ε�
	ID_INDICATOR_EXT,	// �ٿ�ε�
	ID_INDICATOR_EXT,	// ������Ȳ
	ID_INDICATOR_EXT	// blank
};


// status�� ������ �ֱ����� �迭
static HICON hIconArray[] =
{
   NULL,	// ����
   NULL,	// ���ε�
   NULL,	// �ٿ�ε�
   NULL,	// �ϵ�
   NULL
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bConnectedFlag = FALSE;	// ������

	// �˻�Ŭ�������� ���(�˻��ѳ����� �ٸ��並 �����ϸ� ����Ÿ ������ �������Ƿ� ���⼭ ����)
	for(int i=0; i<MAX_SEARCH_COUNT; i++){
		m_searchListData[i].nSearchListSize = 0;	
		m_searchListData[i].strSearchListFileName = "";
		for(int k=0; k<MAX_SEARCH_FILE_COUNT; k++){
			m_searchListData[i].strServerIP[k]	= "";
			m_searchListData[i].strServerName[k] = "";
			m_searchListData[i].searchListData[k].nByte = 0;
			m_searchListData[i].searchListData[k].strFileName = "";
			m_searchListData[i].searchListData[k].strFilePath = "";
		}
	}

	// �ٿ�ε���, ���ε��� ����ó��
	m_pARead = NULL;
	m_pAWrite = NULL;
	m_pFile = NULL;
	m_nDownloadMaxIndex = 0;
	m_nUploadMaxIndex = 0;

	m_pMsnDlg = NULL;
}

CMainFrame::~CMainFrame()
{
	
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// CBRS_GRIPPER �����ϸ� ��� ������ ������
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		 | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	// 16Į�� �̻� �̹��� ����
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_TOOLBAR_1);
	imageList.Create(72, 52, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(214, 214, 206)); 
	m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // Ȱ���̹���
	imageList.Detach();
	/**	// ���ٰ� ��Ȱ���ϰ�� (����� ��Ȱ��ȭ�ɰ�찡 ����)
	bitmap.Detach();
	bitmap.LoadBitmap(IDB_TOOLBAR_DISABLE);
	imageList.Create(72, 52,  ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(214, 214, 214)); 
	m_wndToolBar.SendMessage(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imageList.m_hImageList);   // ��Ȱ���̹���.. 
	imageList.Detach();
	bitmap.Detach();
	**/


	// Status �� ������ �ֱ�
	HINSTANCE hInst;
	hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ICON_STATUS_SERVERCOUNT),RT_GROUP_ICON);	
	m_hTooltipIcon[3] = (HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON_STATUS_SERVERCOUNT),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	hIconArray[4] = m_hTooltipIcon[3];	
	m_hTooltipIcon[2] = (HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON_STATUS_UPLOADS),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	hIconArray[3] = m_hTooltipIcon[2];
	m_hTooltipIcon[1] = (HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON_STATUS_DOWNLOADS),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	hIconArray[2] = m_hTooltipIcon[1];
	m_hTooltipIcon[0] = (HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON_STATUS_HARD),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	hIconArray[1] = m_hTooltipIcon[0];

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT),indicators,hIconArray))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Ʈ���̾���
	if(!m_trayIcon.Create(this, UM_ICON_NOTIFY, _T("p2peer v1.0(����)"), NULL, IDI_ICON_TRAY_CONNECT)){
		return -1;
	}
	m_trayIcon.SetIcon(IDI_ICON_TRAY_DISCONNECT);	

	SetWindowText("p2peer v1.0");	// Ÿ��Ʋ��

	// �˸�â(MSN)
	m_wndTaskbarNotifier.Create(this);
	m_wndTaskbarNotifier.SetSkin(IDB_SKIN_MSN);
	m_wndTaskbarNotifier.SetTextFont("Arial",90,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE);
 	m_wndTaskbarNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_wndTaskbarNotifier.SetTextRect(CRect(10,40,m_wndTaskbarNotifier.m_nSkinWidth-10,m_wndTaskbarNotifier.m_nSkinHeight-25));
	
	// Status ��
	// �̹� ���¹� ��ü�� �����Ǿ��ְ� ���¹ٵ� ������ �������Ƿ� Pane ���߰��ϸ�ȴ�
	m_wndStatusBar.SetPaneInfo(5, ID_SEPARATOR, SBPS_NORMAL, 30);
	m_wndStatusBar.SetPaneText(5, "");
	m_wndStatusBar.SetPaneInfo(4, ID_SEPARATOR, SBPS_NORMAL, 140);
	m_wndStatusBar.SetPaneText(4, "����ȵ�");
	m_wndStatusBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(3, "0�� - 0 kB/s");
	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(2, "0�� - 0 kB/s");
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 140);
	m_wndStatusBar.SetPaneText(1, "");
	
	
	// ������ ���丮�� ���� ��, �� ���� ���丮�� �ö� �� Program Files ������ �������ָ� ������..
	// Program Files ������ �̸��� �ٲ��� �ʴµ���...Windows ���丮�� ���� ����̺꿡 �����ϱ��..
	char buf[256]; 
	GetWindowsDirectory(buf, 256);
	strcat(buf, "\\..\\Program Files\\p2peer\\");	// C:\WINNT\..\Program Files
	m_bIsFirst = CreateDirectory(buf, NULL);	// ������ ���� (������ ������ �׳� .., 1 ����), ���� ������ ���ٸ�(ó�������ϴ°��̶��) �⺻���� ����		
	m_strFolderPath.Format("%s", buf);	// �ٸ� Ŭ�������� ��θ� ã�� ����
	if(m_bIsFirst){	// ó�����α׷��� �����ٸ� �ٸ� ������ �����
		char temp[256];
		HANDLE file;	// ������ �����ߴٰ� �ٽ� �ݱ����� ����ϴ� �ڵ�

		// ���ϴٿ�ε� �ӽ�����
		strcpy(temp, buf);
		strcat(temp, FOLDERNAME_TEMP);
		CreateDirectory(temp, NULL);	

		// ���ϴٿ�ε� ����
		strcpy(temp, buf);
		strcat(temp, FOLDERNAME_INCOMING);
		CreateDirectory(temp, NULL);	

		// ServerLibrary.dat ����
		strcpy(temp, buf);
		strcat(temp, FILENAME_SERVERLIBRARY);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);

		// DownloadsList.dat ����
		strcpy(temp, buf);
		strcat(temp, FILENAME_DOWNLOADSLIST);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);

		// UploadsList.dat ����
		strcpy(temp, buf);
		strcat(temp, FILENAME_UPLOADSLIST);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);

		// Preferences.ini ����
		strcpy(temp, buf);
		strcat(temp, FILENAME_PREFERENCES);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);
	}	

	// ȯ�漳������ ���Ͽ��� �о��
	//this->m_cPreferencesValue.LoadValue();

	LoadFile(1, m_cDownloadInfo);	// �ٿ�ε��� ����ó��
	LoadFile(0, m_cUploadInfo);	// ���ε��� ����ó��

	SetCheckToolbar(ID_MENU_SEARCH);

	m_nViewNum = 0;
	 
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style ^= FWS_ADDTOTITLE;	// ������� ���ֱ�
	
	

//SendMessage(WM_SYSCOMMAND, (WPARAM)SC_MAXIMIZE); 
//	ShowWindow(SW_MAXIMIZE);
	return TRUE;
}


LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	CMenu menu, *pSubMenu;
	// �����ʸ��콺���������
	if(LOWORD(lParam) == WM_RBUTTONUP){
		if(!menu.LoadMenu(IDR_MENU_TRAY)){
			return 0;
		}
		if(!(pSubMenu = menu.GetSubMenu(0))){
			return 0;
		}
		CPoint pos;
		GetCursorPos(&pos);
		SetForegroundWindow();
		// ���ؽ�Ʈ �޴����
		pSubMenu->TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
		menu.DestroyMenu();
	}else if(LOWORD(lParam) == WM_LBUTTONDBLCLK){	// ���� ��ư����Ŭ��		
		if(!IsWindowVisible()){	// ������ ��������
			SendMessage(WM_SYSCOMMAND, (WPARAM)SC_RESTORE); 
			ShowWindow(SW_SHOW);
			SetForegroundWindow();
		}else{
			return 1;
		}
		
	}
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnServerConnect() 
{
	// TODO: Add your command handler code here
	if(m_bConnectedFlag == FALSE){	// �������̶�� 
		if(m_bIsFirst){	// ó�������ϴ°��̶�� 1,  �⺻���� �����ϱ�
			//MessageBox("���α׷��� ó�� �����ϴ°��̹Ƿ� \n�ٿ�ε�, ���ε� �� �⺻���μ���");
			CPreferencesDlg dlg;
			dlg.m_nTabViewNum = 2;
			dlg.FirstSetCurSel(2);
			if(dlg.DoModal() == IDCANCEL){
				AfxMessageBox("ó�� �ѹ��� ���丮 ������ �ϼž��մϴ�!");
				return;	
			}
		}

		CConnectDlg dlg;
		if(dlg.DoModal() == IDCANCEL){
			return;
		}

	

		m_bConnectedFlag = TRUE;	// ����	
		//SetWindowText("p2peer (������..)");	//Ÿ��Ʋ��
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetTooltipText("p2peer v1.0(�����)");	// Ʈ���̾�����
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetIcon(IDI_ICON_TRAY_CONNECT);	// Ʈ���̾�����

		// 16Į�� �̻� �̹��� ����, �����̹��� ��ü
		CImageList imageList;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TOOLBAR_2);
		imageList.Create(72, 52, ILC_COLORDDB  | ILC_MASK, 16, 1);
		imageList.Add(&bitmap, RGB(214, 214, 206)); 
		m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // Ȱ���̹���
		imageList.Detach();
	}else{	// ���� �������̶��
		// TODO: Add your command handler code here

		// ��������
		m_myIOCPSocket.CloseSevrSock();		

		m_bConnectedFlag = FALSE;	// ������	
		//SetWindowText("p2peer (������������..)");	// Ÿ��Ʋ��
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetTooltipText("p2peer v1.0(����)");	// Ʈ���̾�����
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetIcon(IDI_ICON_TRAY_DISCONNECT);	// Ʈ���̾�����

		// 16Į�� �̻� �̹��� ����, �����̹��� ��ü
		CImageList imageList;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TOOLBAR_1);
		imageList.Create(72, 52, ILC_COLORDDB  | ILC_MASK, 16, 1);
		imageList.Add(&bitmap, RGB(214, 214, 206)); 
		m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // Ȱ���̹���
		imageList.Detach();

		// ���������� status ���� �ϵ�뷮 �������
		m_wndStatusBar.SetPaneText(1, "");
	}
}




// Message
void CMainFrame::OnMenuConnectUpdate(CCmdUI *pCmdUI)	// ���ӹ�ư�������� �ٸ���ư�� ��Ȱ��ȭ(UPDATE_COMMAND_UI)
{
	switch(pCmdUI->m_nID){
		case ID_MENU_SERVER_CONNECT:
			pCmdUI->Enable(!m_bConnectedFlag);	// ����������� m_bConnectedFlag = 1
			break;
		case ID_MENU_SERVER_DISCONNECT:
			pCmdUI->Enable(m_bConnectedFlag);			
			break;
	}
	
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	// ����Ŭ���� ó�� (���α׷��� ����Ǵ°� �ƴ϶� �Ⱥ��̰�
	// ������϶��� �ȵǰ�
//	#ifndef MYDEBUG
		ShowWindow(SW_HIDE);
		return;
//	#endif

	// ������ ������ϰ� ���α׷��� ������
	if(m_myIOCPSocket.m_hServSock != INVALID_SOCKET){
		m_myIOCPSocket.CloseSevrSock();
	}

	CFrameWnd::OnClose();
}

void CMainFrame::OnMenuSearch() 
{
	// TODO: Add your command handler code here
	if(SetCheckToolbar(ID_MENU_SEARCH)){	// ���ٰ� ��������ȿ��
		return;
	}
	ShowView(0);
}

void CMainFrame::OnMenuServer() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("�غ����Դϴ�");
	return;

	if(SetCheckToolbar(ID_MENU_SERVER)){	// ���ٰ� ��������ȿ��
		return;
	}
	ShowView(3);
}


void CMainFrame::OnMenuUploads() 
{
	// TODO: Add your command handler code here
	if(SetCheckToolbar(ID_MENU_UPLOADS)){	// ���ٰ� ��������ȿ��
		return;
	}
	ShowView(2);
}

void CMainFrame::OnMenuDownloads() 
{
	// TODO: Add your command handler code here
	if(SetCheckToolbar(ID_MENU_DOWNLOADS)){	// ���ٰ� ��������ȿ��
		return;
	}
	ShowView(1);
}

void CMainFrame::OnMenuChat() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("�غ����Դϴ�");
	return;

	// ä���ϴ� ���̾�α�(Test), ���߿� ��ũ�帮��Ʈ�� �������� �����ְ� ��������
	if(m_pMsnDlg == NULL){
		m_pMsnDlg = new CMsnDlg;
		m_pMsnDlg->Create(IDD_DIALOG_MSN, GetDesktopWindow( ));
	}
	m_pMsnDlg->ShowWindow(SW_SHOW);
}


void CMainFrame::OnMenuPreferences() 
{
	// TODO: Add your command handler code here
	CPreferencesDlg dlg;
	dlg.DoModal();


//	if(SetCheckToolbar(ID_MENU_PREFERENCES)){	// ���ٰ� ��������ȿ��
//		return;
//	}
}



BOOL CMainFrame::SetCheckToolbar(UINT uID)	// ���ٸ޴��� �������� ���������� ����
{
	if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(uID)){	// �̹� ���õ��ִٸ�
		return TRUE;
	}
	UINT arrToolbar[] = {
		ID_MENU_SEARCH, ID_MENU_DOWNLOADS, ID_MENU_UPLOADS, 
		ID_MENU_SERVER, ID_MENU_CHAT, ID_MENU_PREFERENCES
	};
	int size = sizeof(arrToolbar)/sizeof(UINT);
	for(int i=0; i<size; i++){	
		if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(arrToolbar[i])){	// �̹� ���õ��ִٸ�
			m_wndToolBar.GetToolBarCtrl().CheckButton(arrToolbar[i], 0);	// ����
		}
	}
	m_wndToolBar.GetToolBarCtrl().CheckButton(uID, 1);
	return FALSE;
}


LRESULT CMainFrame::OnTaskbarNotifierClicked(WPARAM wParam,LPARAM lParam)
{
	//MessageBox("A Taskbar Notifier was clicked!!","Hi",MB_OK);
	return 0;
}

void CMainFrame::OnAppExit() 
{
	// TODO: Add your command handler code here
	CFrameWnd::OnClose();	// Exit ��ư�� ���������� ���� �ݱ�
	//PostQuitMessage(0);
}









void CMainFrame::ShowView(UINT no)
{
	m_nViewNum = no;
	CView *oldView = GetActiveView();
	//CView *m_pNewView;
	switch(no){
		case 0:	// �˻�
			m_pNewView = new CSearchView;
			break;
		case 1:	// �ٿ�ε�
			m_pNewView = new CDownloadsView;	
			break;
		case 2:	// ���ε�
			m_pNewView = new CUploadsView;
			break;
		case 3:	// ����
			m_pNewView = new CServerView;
			break;
	}
	m_pNewView->Create(NULL, "", WS_CHILD | WS_VISIBLE | WS_BORDER,
		this->rectDefault, this, AFX_IDW_PANE_FIRST, NULL);
	m_pNewView->ShowWindow(SW_SHOW);
	CDocument *pDoc = oldView->GetDocument();
	pDoc->AddView(m_pNewView);
	pDoc->RemoveView(oldView);
	oldView->DestroyWindow();
	SetActiveView(m_pNewView);
	this->RecalcLayout();
	m_pNewView->OnInitialUpdate();
	m_pNewView->Invalidate();
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here	
	SaveFile(1, m_cDownloadInfo);	// �ٿ�ε��� ����ó��
	SaveFile(0, m_cUploadInfo);	// ���ε��� ����ó��
}

// �޴����� �����ϱ�
void CMainFrame::OnMenuServerConnect() 
{
	// TODO: Add your command handler code here
	OnServerConnect();
}

// �޴����� ��������
void CMainFrame::OnMenuServerDisconnect() 
{
	// TODO: Add your command handler code here
	OnServerConnect();
}

// ������������ statusbar ����
LRESULT CMainFrame::OnSetPaneText(WPARAM wParam, LPARAM lParam)
{
	//CString strOut;
	//strOut.Format("%s", wParam);
	char *szOut;
	szOut = (char *)wParam;
	m_wndStatusBar.SetPaneText(4, szOut);		
	delete szOut;	// ������ �ʿ��� �������� �����ϹǷ� delete
	return TRUE;	
}


// �˾��޽���
LRESULT CMainFrame::OnPopupMessage(WPARAM wParam, LPARAM lParam)
{
	if(lParam == POPUP_MESSEGE_DOWNLOADCOMPLETE  &&  this->m_cPreferencesValue.m_bPopupCompleteDownload != 1){	// �ٿ�ε�Ϸ��
		return FALSE;
	}
	if(lParam == POPUP_MESSEGE_SERVER_CONNECT  &&  this->m_cPreferencesValue.m_bPopupConnect != 1){	// ���ο� ������ �����
		return FALSE;
	}
	if(lParam == POPUP_MESSEGE_SERVER_DISCONNECT  &&  this->m_cPreferencesValue.m_bPopupDisconnect != 1){	// ���ο� ������ �����
		return FALSE;
	}

	char *szOut;
	szOut = (char *)wParam;
	m_wndTaskbarNotifier.Show(szOut, 300, m_cPreferencesValue.m_nPopupTime * 1000, 300);	// �ð��� 1 / 1000 ��
	delete szOut;	// ������ �ʿ��� �������� �����ϹǷ� delete
	return TRUE;
}

// IOCP���� ã�� ������ IOCPŬ�������� �˻� Ŭ������ ������ �ϸ� ��������(?) �׷��� ���⸦ ���ļ���
LRESULT CMainFrame::OnAddItemSearchFile(WPARAM wParam, LPARAM lParam)	
{
	((CSearchView *)GetActiveView())->AddItemSearchFile((char *)wParam, lParam);
	//char *szOut;
	//szOut = (char *)wParam;
	return TRUE;
}

// ���� �־ , �� ����
CString CMainFrame::ChangeComma(UINT data)
{
	CString strData;
	strData.Format("%d", data);
	if(strData.GetLength() <= 3){	// 1kb �� �ȵǸ�
		switch(strData.GetLength()){
			case 1:
				strData.Format("0.00%d", data);
				break;
			case 2:
				strData.Format("0.0%d", data);
				break;
			case 3:
				strData.Format("0.%d", data);
				break;
		}
		return strData;
	}
	strData = strData.Left(strData.GetLength() - 3);
	for(int a=strData.GetLength()-3; a>0; a-=3){
			strData.Insert(a,",");		
	}
	return strData;
}

// �ٿ�ε���, ���ε��� ��������
void CMainFrame::SaveFile(BOOL nFlag, CDataDownload &cData)	// nFlag 1�̸� �ٿ�ε� 0 �̸� ���ε�
{
	CString arrFileName[] = {FILENAME_UPLOADSLIST, FILENAME_DOWNLOADSLIST};

	m_pFile = new CFile;
	CString tempPath;
	tempPath.Format("%s%s", m_strFolderPath, arrFileName[nFlag]);
	m_pFile->Open(tempPath, CFile::modeCreate | CFile::modeWrite);
	m_pAWrite = new CArchive(m_pFile, CArchive::store);
	int cnt;
	if(nFlag == 1){	// �ٿ�
		cnt = m_downloadFileList.GetCount();
	}else if(nFlag == 0){	// ��
		cnt = m_uploadFileList.GetCount();
	}
	
	// ����Ÿ�� ���Ͽ� ������ ����� �𸣹Ƿ� ����տ� ����Ÿ�� ������ ���´�
	cData.m_bIsData = FALSE;	// ����Ÿ�� �ƴ� ���� �Է�
	cData.m_nCount = cnt;
	cData.Serialize(*m_pAWrite);

	cData.m_bIsData = TRUE;	// ����Ÿ�Է�
	MY_DOWNLOAD_FILE_LIST tempFile;
	POSITION pos;
	if(nFlag == 1){	// �ٿ�
		pos = m_downloadFileList.GetHeadPosition();
	}else if(nFlag == 0){	// ��
		pos = m_uploadFileList.GetHeadPosition();
	}
	while(pos){
		if(nFlag == 1){	// �ٿ�
			tempFile = m_downloadFileList.GetAt(pos);
		}else if(nFlag == 0){	// ��
			tempFile = m_uploadFileList.GetAt(pos);
		}
		cData.m_strFileName = tempFile.downloadList.strFileName;	// �����̸�
		cData.m_strFilePath = tempFile.downloadList.strFilePath;	// ���ϰ��
		cData.m_nByte = tempFile.downloadList.nByte;	// �ѹ���Ʈ��
		cData.m_nIndex = tempFile.nIndex;
		cData.m_strServerName = tempFile.strServerName;	// �����̸�
		cData.m_strServerIP = tempFile.strServerIP;	// ����������
		cData.m_nReceiveSize = tempFile.nReceiveSize;
		cData.m_strStatus = tempFile.strStatus;	// 1�̸� ����, 0�̸� ������
		cData.Serialize(*m_pAWrite);	
		if(nFlag == 1){	// �ٿ�
			m_downloadFileList.GetNext(pos);
		}else if(nFlag == 0){	// ��
			m_uploadFileList.GetNext(pos);
		}
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

// �ٿ�ε���, ���ε��� ���Ϸε�
void CMainFrame::LoadFile(BOOL nFlag, CDataDownload &cData)	// nFlag 1�̸� �ٿ�ε� 0 �̸� ���ε�
{
	CString arrFileName[] = {FILENAME_UPLOADSLIST, FILENAME_DOWNLOADSLIST};
	m_pFile = new CFile;
	CString tempPath;
	tempPath.Format("%s%s", m_strFolderPath, arrFileName[nFlag]);
	m_pFile->Open(tempPath, CFile::modeRead);
	m_pARead = new CArchive(m_pFile, CArchive::load);	

	// ����Ÿ�� ���Ͽ� ������ ����� �𸣹Ƿ� ����տ� ����Ÿ�� ������ ���´�
	cData.m_bIsData = FALSE;	// ����Ÿ�� �ƴ� ���� ���
	try{	// ����Ÿ�� ������ ���â�� �߹Ƿ� ���⼭ �̸� ����ó��
		cData.Serialize(*m_pARead);
	}catch(...){		
		;
	}
	int cnt = cData.m_nCount;

	CString temp;
	temp.Format("%d", cnt);
	//MessageBox(temp);
	
	cData.m_bIsData = TRUE;	// ����Ÿ�� ���
	MY_DOWNLOAD_FILE_LIST tempFile;
	UINT nMaxNum = cnt - 1;
	for(int i=0; i<cnt; i++){		
		cData.Serialize(*m_pARead);	
		m_pARead->Flush();
		tempFile.downloadList.strFileName = cData.m_strFileName;	// �����̸�
		tempFile.downloadList.strFilePath = cData.m_strFilePath;	// ���ϰ��
		tempFile.downloadList.nByte = cData.m_nByte;	// �ѹ���Ʈ��
		tempFile.nIndex = nMaxNum;
		tempFile.strServerName = cData.m_strServerName;	// �����̸�
		tempFile.strServerIP = cData.m_strServerIP;	// ����������
		tempFile.nReceiveSize = cData.m_nReceiveSize;
		if(cData.m_strStatus ==  "�ٿ�ε���"  ||  cData.m_strStatus ==  "���ε���"){
			tempFile.strStatus = "";
		}else{
			tempFile.strStatus = cData.m_strStatus;
		}
		if(nFlag == 1){	// �ٿ�
			m_downloadFileList.AddTail(tempFile);
		}else if(nFlag == 0){	// ��
			m_uploadFileList.AddTail(tempFile);
		}
	
		if(nFlag == 1){	// �ٿ�
			m_nDownloadMaxIndex = m_nDownloadMaxIndex + 1;
			m_myIOCPSocket.m_nDownloadMaxIndex = m_nDownloadMaxIndex;	// �׻��� ����ȭ��Ų��
		}else if(nFlag == 0){	// ��
			m_nUploadMaxIndex = m_nUploadMaxIndex + 1;
			m_myIOCPSocket.m_nUploadMaxIndex = m_nUploadMaxIndex;	// �׻��� ����ȭ��Ų��
		}			
		nMaxNum--;	// �ε�������
		
	}
	
	m_pARead->Close();
	m_pFile->Close();
	delete m_pARead;
	delete m_pFile;
	m_pARead = NULL;
	m_pFile = NULL;
}

// �ٿ�ε�Ϸ�Ǹ� �ٿ�ε���� ����������
LRESULT CMainFrame::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	POSITION posData;	
	posData = m_downloadFileList.GetHeadPosition();
	MY_DOWNLOAD_FILE_LIST *pData;	

	while(posData){	
		pData = &(m_downloadFileList.GetAt(posData));
		if(pData->nIndex == (UINT)wParam){
			pData->nReceiveSize = pData->downloadList.nByte;
			pData->strStatus = "�ٿ�ε�Ϸ�";
			break;
		}
		m_downloadFileList.GetNext(posData);			
	}

	// ����䰡 �ٿ�ε��϶� �ٿ�ε��� ������Ʈ
	if(m_nViewNum == 1){	
		char szNum[5];
		CListCtrl *pList = &(((CDownloadsView *)m_pNewView)->m_listDownloads);
		for(int i=0; i<pList->GetItemCount(); i++){
			pList->GetItemText(i, 6, szNum, 5);						
			if((UINT)wParam == (UINT)atoi(szNum)){
				char szReceiveSize[256];
				sprintf(szReceiveSize, "%s KB", ChangeComma(pData->nReceiveSize));
				pList->SetItemText(i, 2, szReceiveSize);
				sprintf(szReceiveSize, "[ %d%% ]", 100);
				pList->SetItemText(i, 3, szReceiveSize);
				pList->SetItemText(i, 4, "�ٿ�ε�Ϸ�");					
				break;
			}
		}

		// static ������Ʈ
		CString strState;
		INT cnt;
		if(this->m_myIOCPSocket.m_downloadFileDataList.GetCount() - 1 == -1){
			cnt = 0;
		}else{
			cnt = this->m_myIOCPSocket.m_downloadFileDataList.GetCount() - 1;
		}
		strState.Format("Downloads (%d/%d)", cnt, ((CDownloadsView *)m_pNewView)->m_listDownloads.GetItemCount());
		((CDownloadsView *)m_pNewView)->m_staticState.SetWindowText(strState);
	}	

	// status ��, download
	CString strStatus;
	INT cnt;
	if(this->m_myIOCPSocket.m_downloadFileDataList.GetCount() - 1 == -1){
		cnt = 0;
	}else{
		cnt = this->m_myIOCPSocket.m_downloadFileDataList.GetCount() - 1;
	}		
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d�� - %d kB/s", cnt, m_cPreferencesValue.m_nDownloadSpeed);
	m_wndStatusBar.SetPaneText(3, strStatus);

	return TRUE;
}


// ���Ϸ�Ǹ� ���ε���� ����������
LRESULT CMainFrame::OnUploadComplete(WPARAM wParam, LPARAM lParam)
{
	POSITION posData;	
	posData = m_uploadFileList.GetHeadPosition();
	MY_DOWNLOAD_FILE_LIST *pData;	

	while(posData){	
		pData = &(m_uploadFileList.GetAt(posData));
		if(pData->nIndex == (UINT)wParam){
			pData->nReceiveSize = pData->downloadList.nByte;
			pData->strStatus = "���ε�Ϸ�";
			break;
		}
		m_uploadFileList.GetNext(posData);			
	}

	// ����䰡 ���ε� ���ε��� ������Ʈ
	if(m_nViewNum == 2){	
		char szNum[5];
		CListCtrl *pList = &(((CUploadsView *)m_pNewView)->m_listUploads);
		for(int i=0; i<pList->GetItemCount(); i++){
			pList->GetItemText(i, 6, szNum, 5);						
			if((UINT)wParam == (UINT)atoi(szNum)){
				char szReceiveSize[256];
				sprintf(szReceiveSize, "%s KB", ChangeComma(pData->nReceiveSize));
				pList->SetItemText(i, 2, szReceiveSize);
				sprintf(szReceiveSize, "[ %d%% ]", 100);
				pList->SetItemText(i, 3, szReceiveSize);
				pList->SetItemText(i, 4, "���ε�Ϸ�");					
				break;
			}
		}

		// static ������Ʈ
		CString strState;
		INT cnt;
		if(this->m_myIOCPSocket.m_uploadFileDataList.GetCount() - 1 == -1){
			cnt = 0;
		}else{
			cnt = this->m_myIOCPSocket.m_uploadFileDataList.GetCount() - 1;
		}		
		strState.Format("Uploads (%d/%d)", cnt, ((CUploadsView *)m_pNewView)->m_listUploads.GetItemCount());
		((CUploadsView *)m_pNewView)->m_staticState.SetWindowText(strState);
	}	

	// status ��, upload
	CString strStatus;
	INT cnt;
	if(this->m_myIOCPSocket.m_uploadFileDataList.GetCount() - 1 == -1){
		cnt = 0;
	}else{
		cnt = this->m_myIOCPSocket.m_uploadFileDataList.GetCount() - 1;
	}		
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d�� - %d kB/s", cnt, m_cPreferencesValue.m_nUploadSpeed);
	m_wndStatusBar.SetPaneText(2, strStatus);
	return TRUE;
}


// �ٿ�ε��� �ϳ��߰�
LRESULT CMainFrame::OnAddDownloadData(WPARAM wParam, LPARAM lParam)
{
	// status ��, download
	CString strStatus;
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d�� - %d kB/s", m_myIOCPSocket.m_downloadFileDataList.GetCount(), m_cPreferencesValue.m_nDownloadSpeed);
	m_wndStatusBar.SetPaneText(3, strStatus);

	return TRUE;
}


// ���ε��� �ϳ��߰�
LRESULT CMainFrame::OnAddUploadData(WPARAM wParam, LPARAM lParam)
{
	char *szOut;
	szOut = (char *)wParam;
	
	INT cnt;
	CString strMessage;
	strMessage.Format("%s", szOut);
	cnt = strMessage.Find('?', 0);		
	int oldCnt = 0;	

	// �ϴ��� ���⿡ ���������� ��ũ�帮��Ʈ������
	MY_DOWNLOAD_FILE_LIST tempFile;	
	tempFile.downloadList.strFileName = strMessage.Mid(oldCnt, cnt);
	
	oldCnt = cnt;
	cnt = strMessage.Find('?', oldCnt + 1);
	tempFile.downloadList.strFilePath = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);
	
	oldCnt = cnt;
	cnt = strMessage.Find('?', oldCnt + 1);
	tempFile.downloadList.nByte = atoi(strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1));

	tempFile.nReceiveSize = 0;
	tempFile.nSpeed = 0;
	tempFile.strStatus = "���ε���";

	oldCnt = cnt;
	cnt = strMessage.Find('?', oldCnt + 1);
	tempFile.strServerIP = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);
	
	oldCnt = cnt;
	cnt = strMessage.Find('?', oldCnt + 1);
	tempFile.strServerName = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);
	 
	tempFile.nIndex = m_nUploadMaxIndex;
	m_nUploadMaxIndex++;
	m_myIOCPSocket.m_nUploadMaxIndex = m_nUploadMaxIndex;	// �׻��� ����ȭ��Ų��

	m_uploadFileList.AddHead(tempFile);	// ��ũ�帮��Ʈ�� ������

	delete szOut;	// ������ �ʿ��� �������� �����ϹǷ� delete

	// ����䰡 ���ε� ���ε��� ������Ʈ
	if(m_nViewNum == 2){	
		((CUploadsView *)m_pNewView)->m_listUploads.DeleteAllItems();
		MY_DOWNLOAD_FILE_LIST tempFile;	// �ϴ��� ���⿡ ����������
		CString serverName;
		POSITION pos;
		pos = m_uploadFileList.GetHeadPosition();
		while(pos){
			tempFile = m_uploadFileList.GetAt(pos);
			serverName.Format("%s [%s]", tempFile.strServerName, tempFile.strServerIP);
			((CUploadsView *)m_pNewView)->AddItem(tempFile.downloadList.strFileName, tempFile.downloadList.nByte, tempFile.nReceiveSize, tempFile.strStatus, 
				serverName,	tempFile.nIndex);
			m_uploadFileList.GetNext(pos);
		}
		
		// static ������Ʈ
		CString strState;
		strState.Format("Uploads (%d/%d)", m_myIOCPSocket.m_uploadFileDataList.GetCount(), ((CUploadsView *)m_pNewView)->m_listUploads.GetItemCount());
		((CUploadsView *)m_pNewView)->m_staticState.SetWindowText(strState);
	}	

	// status ��, upload
	CString strStatus;
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d�� - %d kB/s", m_myIOCPSocket.m_uploadFileDataList.GetCount(), m_cPreferencesValue.m_nUploadSpeed);
	m_wndStatusBar.SetPaneText(2, strStatus);

	return TRUE;
	
}

// �ü���������ϴ� �����ܾ��
void CMainFrame::GetSysImgList(CListCtrl &listCtrl, CImageList &imageList)
{
	// �ü���� �����ϴ� ���� ������ ��������
    HIMAGELIST  hSystemImgList;
    SHFILEINFO  info;
 
    hSystemImgList = (HIMAGELIST)::SHGetFileInfo(NULL, 0, &info, 
		   sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    
	imageList.Attach(hSystemImgList);    
	
    // ������ ����
    listCtrl.SetImageList(&imageList, LVSIL_SMALL);
	imageList.Detach();
}


