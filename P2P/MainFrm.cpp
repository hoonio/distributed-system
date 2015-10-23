// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "P2p.h"

#include "MainFrm.h"


#include "ConnectDlg.h"	// 서버접속
#include "PreferencesDlg.h"	// 환경설정

#include "SearchView.h"	// 검색
#include "DownloadsView.h"	// 다운로드
#include "UploadsView.h"	// 업로드
#include "ServerView.h"	// 서버




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
	ON_MESSAGE(UM_ICON_NOTIFY, OnTrayNotification)	// 트레이아이콘
	ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_SERVER_CONNECT, ID_MENU_SERVER_DISCONNECT, OnMenuConnectUpdate)	// UPDATE_COMMAND_UI
	ON_MESSAGE(WM_TASKBARNOTIFIERCLICKED,OnTaskbarNotifierClicked)	// 알림창(MSN)
	ON_MESSAGE(UM_SETPANETEXT, OnSetPaneText)	// 메인프레임의 statusbar 수정
	ON_MESSAGE(UM_POPUPMESSAGE, OnPopupMessage)	// 팝업메시지
	ON_MESSAGE(UM_ADDITEMSEARCHFILE, OnAddItemSearchFile)	// IOCP에서 찾은파일을여기서처리
	ON_MESSAGE(UM_DOWNLOAD_COMPLETE, OnDownloadComplete)	// 다운완료
	ON_MESSAGE(UM_UPLOAD_COMPLETE, OnUploadComplete)	// 다운완료
	ON_MESSAGE(UM_ADD_UPLOAD_DATA, OnAddUploadData)	// 업로드데이타추가
	ON_MESSAGE(UM_ADD_DOWNLOAD_DATA, OnAddDownloadData)	// 다운로드데이타추가
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	/**
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	**/
	ID_INDICATOR_EXT,	// 하드남은용량
	ID_INDICATOR_EXT,	// 업로드
	ID_INDICATOR_EXT,	// 다운로드
	ID_INDICATOR_EXT,	// 서버현황
	ID_INDICATOR_EXT	// blank
};


// status에 아이콘 넣기위한 배열
static HICON hIconArray[] =
{
   NULL,	// 서버
   NULL,	// 업로드
   NULL,	// 다운로드
   NULL,	// 하드
   NULL
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bConnectedFlag = FALSE;	// 비접속

	// 검색클래스에서 사용(검색한내용이 다른뷰를 선택하면 데이타 내용이 없어지므로 여기서 관리)
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

	// 다운로드목록, 업로드목록 파일처리
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
	
	// CBRS_GRIPPER 삭제하면 쿨바 왼쪽의 선없앰
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		 | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	// 16칼라 이상 이미지 적용
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_TOOLBAR_1);
	imageList.Create(72, 52, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(214, 214, 206)); 
	m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // 활성이미지
	imageList.Detach();
	/**	// 툴바가 비활성일경우 (현재는 비활성화될경우가 없음)
	bitmap.Detach();
	bitmap.LoadBitmap(IDB_TOOLBAR_DISABLE);
	imageList.Create(72, 52,  ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(214, 214, 214)); 
	m_wndToolBar.SendMessage(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imageList.m_hImageList);   // 비활성이미지.. 
	imageList.Detach();
	bitmap.Detach();
	**/


	// Status 바 아이콘 넣기
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

	// 트레이아콘
	if(!m_trayIcon.Create(this, UM_ICON_NOTIFY, _T("p2peer v1.0(끊김)"), NULL, IDI_ICON_TRAY_CONNECT)){
		return -1;
	}
	m_trayIcon.SetIcon(IDI_ICON_TRAY_DISCONNECT);	

	SetWindowText("p2peer v1.0");	// 타이틀바

	// 알림창(MSN)
	m_wndTaskbarNotifier.Create(this);
	m_wndTaskbarNotifier.SetSkin(IDB_SKIN_MSN);
	m_wndTaskbarNotifier.SetTextFont("Arial",90,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE);
 	m_wndTaskbarNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_wndTaskbarNotifier.SetTextRect(CRect(10,40,m_wndTaskbarNotifier.m_nSkinWidth-10,m_wndTaskbarNotifier.m_nSkinHeight-25));
	
	// Status 바
	// 이미 상태바 객체가 생성되어있고 상태바도 설정이 되있으므로 Pane 만추가하면된다
	m_wndStatusBar.SetPaneInfo(5, ID_SEPARATOR, SBPS_NORMAL, 30);
	m_wndStatusBar.SetPaneText(5, "");
	m_wndStatusBar.SetPaneInfo(4, ID_SEPARATOR, SBPS_NORMAL, 140);
	m_wndStatusBar.SetPaneText(4, "연결안됨");
	m_wndStatusBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(3, "0개 - 0 kB/s");
	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneText(2, "0개 - 0 kB/s");
	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 140);
	m_wndStatusBar.SetPaneText(1, "");
	
	
	// 윈도우 디렉토리를 구한 후, 그 상위 디렉토리로 올라간 후 Program Files 폴더로 접근해주면 되지요..
	// Program Files 폴더는 이름이 바뀌지 않는데다...Windows 디렉토리와 같은 드라이브에 있으니까요..
	char buf[256]; 
	GetWindowsDirectory(buf, 256);
	strcat(buf, "\\..\\Program Files\\p2peer\\");	// C:\WINNT\..\Program Files
	m_bIsFirst = CreateDirectory(buf, NULL);	// 폴더를 생성 (폴더가 있으면 그냥 .., 1 리턴), 만약 폴더가 없다면(처음실행하는것이라면) 기본적인 설정		
	m_strFolderPath.Format("%s", buf);	// 다른 클래스에서 경로를 찾기 쉽게
	if(m_bIsFirst){	// 처음프로그램을 열었다면 다른 폴더도 만든다
		char temp[256];
		HANDLE file;	// 파일을 생성했다가 다시 닫기위해 사용하는 핸들

		// 파일다운로드 임시폴더
		strcpy(temp, buf);
		strcat(temp, FOLDERNAME_TEMP);
		CreateDirectory(temp, NULL);	

		// 파일다운로드 폴더
		strcpy(temp, buf);
		strcat(temp, FOLDERNAME_INCOMING);
		CreateDirectory(temp, NULL);	

		// ServerLibrary.dat 파일
		strcpy(temp, buf);
		strcat(temp, FILENAME_SERVERLIBRARY);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);

		// DownloadsList.dat 파일
		strcpy(temp, buf);
		strcat(temp, FILENAME_DOWNLOADSLIST);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);

		// UploadsList.dat 파일
		strcpy(temp, buf);
		strcat(temp, FILENAME_UPLOADSLIST);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);

		// Preferences.ini 파일
		strcpy(temp, buf);
		strcat(temp, FILENAME_PREFERENCES);
		file = (HANDLE)CreateFile( temp, GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
		    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
		CloseHandle(file);
	}	

	// 환경설정값을 파일에서 읽어옴
	//this->m_cPreferencesValue.LoadValue();

	LoadFile(1, m_cDownloadInfo);	// 다운로드목록 파일처리
	LoadFile(0, m_cUploadInfo);	// 업로드목록 파일처리

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
	cs.style ^= FWS_ADDTOTITLE;	// 제목없음 없애기
	
	

//SendMessage(WM_SYSCOMMAND, (WPARAM)SC_MAXIMIZE); 
//	ShowWindow(SW_MAXIMIZE);
	return TRUE;
}


LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	CMenu menu, *pSubMenu;
	// 오른쪽마우스눌렀을경우
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
		// 컨텍스트 메뉴출력
		pSubMenu->TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
		menu.DestroyMenu();
	}else if(LOWORD(lParam) == WM_LBUTTONDBLCLK){	// 왼쪽 버튼더블클릭		
		if(!IsWindowVisible()){	// 보이지 않을때만
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
	if(m_bConnectedFlag == FALSE){	// 비접속이라면 
		if(m_bIsFirst){	// 처음실행하는것이라면 1,  기본적인 설정하기
			//MessageBox("프로그램을 처음 실행하는것이므로 \n다운로드, 업로드 등 기본적인설정");
			CPreferencesDlg dlg;
			dlg.m_nTabViewNum = 2;
			dlg.FirstSetCurSel(2);
			if(dlg.DoModal() == IDCANCEL){
				AfxMessageBox("처음 한번은 디렉토리 설정을 하셔야합니다!");
				return;	
			}
		}

		CConnectDlg dlg;
		if(dlg.DoModal() == IDCANCEL){
			return;
		}

	

		m_bConnectedFlag = TRUE;	// 접속	
		//SetWindowText("p2peer (접속중..)");	//타이틀바
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetTooltipText("p2peer v1.0(연결됨)");	// 트레이아이콘
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetIcon(IDI_ICON_TRAY_CONNECT);	// 트레이아이콘

		// 16칼라 이상 이미지 적용, 툴바이미지 교체
		CImageList imageList;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TOOLBAR_2);
		imageList.Create(72, 52, ILC_COLORDDB  | ILC_MASK, 16, 1);
		imageList.Add(&bitmap, RGB(214, 214, 206)); 
		m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // 활성이미지
		imageList.Detach();
	}else{	// 현재 접속중이라면
		// TODO: Add your command handler code here

		// 소켓해제
		m_myIOCPSocket.CloseSevrSock();		

		m_bConnectedFlag = FALSE;	// 비접속	
		//SetWindowText("p2peer (접속해제상태..)");	// 타이틀바
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetTooltipText("p2peer v1.0(끊김)");	// 트레이아이콘
		((CMainFrame *)AfxGetMainWnd())->m_trayIcon.SetIcon(IDI_ICON_TRAY_DISCONNECT);	// 트레이아이콘

		// 16칼라 이상 이미지 적용, 툴바이미지 교체
		CImageList imageList;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TOOLBAR_1);
		imageList.Create(72, 52, ILC_COLORDDB  | ILC_MASK, 16, 1);
		imageList.Add(&bitmap, RGB(214, 214, 206)); 
		m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // 활성이미지
		imageList.Detach();

		// 접속해제시 status 바의 하드용량 내용삭제
		m_wndStatusBar.SetPaneText(1, "");
	}
}




// Message
void CMainFrame::OnMenuConnectUpdate(CCmdUI *pCmdUI)	// 접속버튼눌렀을때 다른버튼은 비활성화(UPDATE_COMMAND_UI)
{
	switch(pCmdUI->m_nID){
		case ID_MENU_SERVER_CONNECT:
			pCmdUI->Enable(!m_bConnectedFlag);	// 접속했을결우 m_bConnectedFlag = 1
			break;
		case ID_MENU_SERVER_DISCONNECT:
			pCmdUI->Enable(m_bConnectedFlag);			
			break;
	}
	
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	// 서브클래싱 처리 (프로그램이 종료되는게 아니라 안보이게
	// 디버깅일때만 안되게
//	#ifndef MYDEBUG
		ShowWindow(SW_HIDE);
		return;
//	#endif

	// 소켓을 종료안하고 프로그램을 닫을때
	if(m_myIOCPSocket.m_hServSock != INVALID_SOCKET){
		m_myIOCPSocket.CloseSevrSock();
	}

	CFrameWnd::OnClose();
}

void CMainFrame::OnMenuSearch() 
{
	// TODO: Add your command handler code here
	if(SetCheckToolbar(ID_MENU_SEARCH)){	// 툴바가 눌러지는효과
		return;
	}
	ShowView(0);
}

void CMainFrame::OnMenuServer() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("준비중입니다");
	return;

	if(SetCheckToolbar(ID_MENU_SERVER)){	// 툴바가 눌러지는효과
		return;
	}
	ShowView(3);
}


void CMainFrame::OnMenuUploads() 
{
	// TODO: Add your command handler code here
	if(SetCheckToolbar(ID_MENU_UPLOADS)){	// 툴바가 눌러지는효과
		return;
	}
	ShowView(2);
}

void CMainFrame::OnMenuDownloads() 
{
	// TODO: Add your command handler code here
	if(SetCheckToolbar(ID_MENU_DOWNLOADS)){	// 툴바가 눌러지는효과
		return;
	}
	ShowView(1);
}

void CMainFrame::OnMenuChat() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("준비중입니다");
	return;

	// 채팅하는 다이얼로그(Test), 나중에 링크드리스트로 여러개를 띄울수있게 만들어야함
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


//	if(SetCheckToolbar(ID_MENU_PREFERENCES)){	// 툴바가 눌러지는효과
//		return;
//	}
}



BOOL CMainFrame::SetCheckToolbar(UINT uID)	// 툴바메뉴를 눌렀을때 눌러진것은 들어가게
{
	if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(uID)){	// 이미 선택되있다면
		return TRUE;
	}
	UINT arrToolbar[] = {
		ID_MENU_SEARCH, ID_MENU_DOWNLOADS, ID_MENU_UPLOADS, 
		ID_MENU_SERVER, ID_MENU_CHAT, ID_MENU_PREFERENCES
	};
	int size = sizeof(arrToolbar)/sizeof(UINT);
	for(int i=0; i<size; i++){	
		if(m_wndToolBar.GetToolBarCtrl().IsButtonChecked(arrToolbar[i])){	// 이미 선택되있다면
			m_wndToolBar.GetToolBarCtrl().CheckButton(arrToolbar[i], 0);	// 복귀
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
	CFrameWnd::OnClose();	// Exit 버튼을 눌렀을때만 완전 닫기
	//PostQuitMessage(0);
}









void CMainFrame::ShowView(UINT no)
{
	m_nViewNum = no;
	CView *oldView = GetActiveView();
	//CView *m_pNewView;
	switch(no){
		case 0:	// 검색
			m_pNewView = new CSearchView;
			break;
		case 1:	// 다운로드
			m_pNewView = new CDownloadsView;	
			break;
		case 2:	// 업로드
			m_pNewView = new CUploadsView;
			break;
		case 3:	// 서버
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
	SaveFile(1, m_cDownloadInfo);	// 다운로드목록 파일처리
	SaveFile(0, m_cUploadInfo);	// 업로드목록 파일처리
}

// 메뉴에서 접속하기
void CMainFrame::OnMenuServerConnect() 
{
	// TODO: Add your command handler code here
	OnServerConnect();
}

// 메뉴에서 접속해제
void CMainFrame::OnMenuServerDisconnect() 
{
	// TODO: Add your command handler code here
	OnServerConnect();
}

// 메인프레임의 statusbar 수정
LRESULT CMainFrame::OnSetPaneText(WPARAM wParam, LPARAM lParam)
{
	//CString strOut;
	//strOut.Format("%s", wParam);
	char *szOut;
	szOut = (char *)wParam;
	m_wndStatusBar.SetPaneText(4, szOut);		
	delete szOut;	// 보내는 쪽에서 동적으로 생성하므로 delete
	return TRUE;	
}


// 팝업메시지
LRESULT CMainFrame::OnPopupMessage(WPARAM wParam, LPARAM lParam)
{
	if(lParam == POPUP_MESSEGE_DOWNLOADCOMPLETE  &&  this->m_cPreferencesValue.m_bPopupCompleteDownload != 1){	// 다운로드완료됨
		return FALSE;
	}
	if(lParam == POPUP_MESSEGE_SERVER_CONNECT  &&  this->m_cPreferencesValue.m_bPopupConnect != 1){	// 새로운 서버에 연결됨
		return FALSE;
	}
	if(lParam == POPUP_MESSEGE_SERVER_DISCONNECT  &&  this->m_cPreferencesValue.m_bPopupDisconnect != 1){	// 새로운 서버에 연결됨
		return FALSE;
	}

	char *szOut;
	szOut = (char *)wParam;
	m_wndTaskbarNotifier.Show(szOut, 300, m_cPreferencesValue.m_nPopupTime * 1000, 300);	// 시간은 1 / 1000 초
	delete szOut;	// 보내는 쪽에서 동적으로 생성하므로 delete
	return TRUE;
}

// IOCP에서 찾은 파일을 IOCP클래스에서 검색 클래스로 접근을 하면 에러가남(?) 그래서 여기를 거쳐서감
LRESULT CMainFrame::OnAddItemSearchFile(WPARAM wParam, LPARAM lParam)	
{
	((CSearchView *)GetActiveView())->AddItemSearchFile((char *)wParam, lParam);
	//char *szOut;
	//szOut = (char *)wParam;
	return TRUE;
}

// 숫자 넣어서 , 로 변경
CString CMainFrame::ChangeComma(UINT data)
{
	CString strData;
	strData.Format("%d", data);
	if(strData.GetLength() <= 3){	// 1kb 가 안되면
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

// 다운로드목록, 업로드목록 파일저장
void CMainFrame::SaveFile(BOOL nFlag, CDataDownload &cData)	// nFlag 1이면 다운로드 0 이면 업로드
{
	CString arrFileName[] = {FILENAME_UPLOADSLIST, FILENAME_DOWNLOADSLIST};

	m_pFile = new CFile;
	CString tempPath;
	tempPath.Format("%s%s", m_strFolderPath, arrFileName[nFlag]);
	m_pFile->Open(tempPath, CFile::modeCreate | CFile::modeWrite);
	m_pAWrite = new CArchive(m_pFile, CArchive::store);
	int cnt;
	if(nFlag == 1){	// 다운
		cnt = m_downloadFileList.GetCount();
	}else if(nFlag == 0){	// 업
		cnt = m_uploadFileList.GetCount();
	}
	
	// 데이타를 파일에 적을때 몇개인지 모르므로 가장앞에 데이타의 갯수를 적는다
	cData.m_bIsData = FALSE;	// 데이타가 아닌 숫자 입력
	cData.m_nCount = cnt;
	cData.Serialize(*m_pAWrite);

	cData.m_bIsData = TRUE;	// 데이타입력
	MY_DOWNLOAD_FILE_LIST tempFile;
	POSITION pos;
	if(nFlag == 1){	// 다운
		pos = m_downloadFileList.GetHeadPosition();
	}else if(nFlag == 0){	// 업
		pos = m_uploadFileList.GetHeadPosition();
	}
	while(pos){
		if(nFlag == 1){	// 다운
			tempFile = m_downloadFileList.GetAt(pos);
		}else if(nFlag == 0){	// 업
			tempFile = m_uploadFileList.GetAt(pos);
		}
		cData.m_strFileName = tempFile.downloadList.strFileName;	// 파일이름
		cData.m_strFilePath = tempFile.downloadList.strFilePath;	// 파일경로
		cData.m_nByte = tempFile.downloadList.nByte;	// 총바이트수
		cData.m_nIndex = tempFile.nIndex;
		cData.m_strServerName = tempFile.strServerName;	// 서버이름
		cData.m_strServerIP = tempFile.strServerIP;	// 서버아이피
		cData.m_nReceiveSize = tempFile.nReceiveSize;
		cData.m_strStatus = tempFile.strStatus;	// 1이면 접속, 0이면 비접속
		cData.Serialize(*m_pAWrite);	
		if(nFlag == 1){	// 다운
			m_downloadFileList.GetNext(pos);
		}else if(nFlag == 0){	// 업
			m_uploadFileList.GetNext(pos);
		}
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

// 다운로드목록, 업로드목록 파일로드
void CMainFrame::LoadFile(BOOL nFlag, CDataDownload &cData)	// nFlag 1이면 다운로드 0 이면 업로드
{
	CString arrFileName[] = {FILENAME_UPLOADSLIST, FILENAME_DOWNLOADSLIST};
	m_pFile = new CFile;
	CString tempPath;
	tempPath.Format("%s%s", m_strFolderPath, arrFileName[nFlag]);
	m_pFile->Open(tempPath, CFile::modeRead);
	m_pARead = new CArchive(m_pFile, CArchive::load);	

	// 데이타를 파일에 적을때 몇개인지 모르므로 가장앞에 데이타의 갯수를 적는다
	cData.m_bIsData = FALSE;	// 데이타가 아닌 숫자 출력
	try{	// 데이타가 없음시 경고창이 뜨므로 여기서 미리 예외처리
		cData.Serialize(*m_pARead);
	}catch(...){		
		;
	}
	int cnt = cData.m_nCount;

	CString temp;
	temp.Format("%d", cnt);
	//MessageBox(temp);
	
	cData.m_bIsData = TRUE;	// 데이타가 출력
	MY_DOWNLOAD_FILE_LIST tempFile;
	UINT nMaxNum = cnt - 1;
	for(int i=0; i<cnt; i++){		
		cData.Serialize(*m_pARead);	
		m_pARead->Flush();
		tempFile.downloadList.strFileName = cData.m_strFileName;	// 파일이름
		tempFile.downloadList.strFilePath = cData.m_strFilePath;	// 파일경로
		tempFile.downloadList.nByte = cData.m_nByte;	// 총바이트수
		tempFile.nIndex = nMaxNum;
		tempFile.strServerName = cData.m_strServerName;	// 서버이름
		tempFile.strServerIP = cData.m_strServerIP;	// 서버아이피
		tempFile.nReceiveSize = cData.m_nReceiveSize;
		if(cData.m_strStatus ==  "다운로드중"  ||  cData.m_strStatus ==  "업로드중"){
			tempFile.strStatus = "";
		}else{
			tempFile.strStatus = cData.m_strStatus;
		}
		if(nFlag == 1){	// 다운
			m_downloadFileList.AddTail(tempFile);
		}else if(nFlag == 0){	// 업
			m_uploadFileList.AddTail(tempFile);
		}
	
		if(nFlag == 1){	// 다운
			m_nDownloadMaxIndex = m_nDownloadMaxIndex + 1;
			m_myIOCPSocket.m_nDownloadMaxIndex = m_nDownloadMaxIndex;	// 항상값을 동기화시킨다
		}else if(nFlag == 0){	// 업
			m_nUploadMaxIndex = m_nUploadMaxIndex + 1;
			m_myIOCPSocket.m_nUploadMaxIndex = m_nUploadMaxIndex;	// 항상값을 동기화시킨다
		}			
		nMaxNum--;	// 인덱스증가
		
	}
	
	m_pARead->Close();
	m_pFile->Close();
	delete m_pARead;
	delete m_pFile;
	m_pARead = NULL;
	m_pFile = NULL;
}

// 다운로드완료되면 다운로드뷰의 내용을변경
LRESULT CMainFrame::OnDownloadComplete(WPARAM wParam, LPARAM lParam)
{
	POSITION posData;	
	posData = m_downloadFileList.GetHeadPosition();
	MY_DOWNLOAD_FILE_LIST *pData;	

	while(posData){	
		pData = &(m_downloadFileList.GetAt(posData));
		if(pData->nIndex == (UINT)wParam){
			pData->nReceiveSize = pData->downloadList.nByte;
			pData->strStatus = "다운로드완료";
			break;
		}
		m_downloadFileList.GetNext(posData);			
	}

	// 현재뷰가 다운로드일때 다운로드목록 업데이트
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
				pList->SetItemText(i, 4, "다운로드완료");					
				break;
			}
		}

		// static 업데이트
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

	// status 바, download
	CString strStatus;
	INT cnt;
	if(this->m_myIOCPSocket.m_downloadFileDataList.GetCount() - 1 == -1){
		cnt = 0;
	}else{
		cnt = this->m_myIOCPSocket.m_downloadFileDataList.GetCount() - 1;
	}		
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d개 - %d kB/s", cnt, m_cPreferencesValue.m_nDownloadSpeed);
	m_wndStatusBar.SetPaneText(3, strStatus);

	return TRUE;
}


// 업완료되면 업로드뷰의 내용을변경
LRESULT CMainFrame::OnUploadComplete(WPARAM wParam, LPARAM lParam)
{
	POSITION posData;	
	posData = m_uploadFileList.GetHeadPosition();
	MY_DOWNLOAD_FILE_LIST *pData;	

	while(posData){	
		pData = &(m_uploadFileList.GetAt(posData));
		if(pData->nIndex == (UINT)wParam){
			pData->nReceiveSize = pData->downloadList.nByte;
			pData->strStatus = "업로드완료";
			break;
		}
		m_uploadFileList.GetNext(posData);			
	}

	// 현재뷰가 업로드 업로드목록 업데이트
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
				pList->SetItemText(i, 4, "업로드완료");					
				break;
			}
		}

		// static 업데이트
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

	// status 바, upload
	CString strStatus;
	INT cnt;
	if(this->m_myIOCPSocket.m_uploadFileDataList.GetCount() - 1 == -1){
		cnt = 0;
	}else{
		cnt = this->m_myIOCPSocket.m_uploadFileDataList.GetCount() - 1;
	}		
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d개 - %d kB/s", cnt, m_cPreferencesValue.m_nUploadSpeed);
	m_wndStatusBar.SetPaneText(2, strStatus);
	return TRUE;
}


// 다운로드목록 하나추가
LRESULT CMainFrame::OnAddDownloadData(WPARAM wParam, LPARAM lParam)
{
	// status 바, download
	CString strStatus;
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d개 - %d kB/s", m_myIOCPSocket.m_downloadFileDataList.GetCount(), m_cPreferencesValue.m_nDownloadSpeed);
	m_wndStatusBar.SetPaneText(3, strStatus);

	return TRUE;
}


// 업로드목록 하나추가
LRESULT CMainFrame::OnAddUploadData(WPARAM wParam, LPARAM lParam)
{
	char *szOut;
	szOut = (char *)wParam;
	
	INT cnt;
	CString strMessage;
	strMessage.Format("%s", szOut);
	cnt = strMessage.Find('?', 0);		
	int oldCnt = 0;	

	// 일단은 여기에 값을저장후 링크드리스트에저장
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
	tempFile.strStatus = "업로드중";

	oldCnt = cnt;
	cnt = strMessage.Find('?', oldCnt + 1);
	tempFile.strServerIP = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);
	
	oldCnt = cnt;
	cnt = strMessage.Find('?', oldCnt + 1);
	tempFile.strServerName = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);
	 
	tempFile.nIndex = m_nUploadMaxIndex;
	m_nUploadMaxIndex++;
	m_myIOCPSocket.m_nUploadMaxIndex = m_nUploadMaxIndex;	// 항상값을 동기화시킨다

	m_uploadFileList.AddHead(tempFile);	// 링크드리스트에 값저장

	delete szOut;	// 보내는 쪽에서 동적으로 생성하므로 delete

	// 현재뷰가 업로드 업로드목록 업데이트
	if(m_nViewNum == 2){	
		((CUploadsView *)m_pNewView)->m_listUploads.DeleteAllItems();
		MY_DOWNLOAD_FILE_LIST tempFile;	// 일단은 여기에 값을저장후
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
		
		// static 업데이트
		CString strState;
		strState.Format("Uploads (%d/%d)", m_myIOCPSocket.m_uploadFileDataList.GetCount(), ((CUploadsView *)m_pNewView)->m_listUploads.GetItemCount());
		((CUploadsView *)m_pNewView)->m_staticState.SetWindowText(strState);
	}	

	// status 바, upload
	CString strStatus;
	m_cPreferencesValue.ConnectLoadFile(&m_cPreferencesValue.m_nDownloadSpeed, &m_cPreferencesValue.m_nUploadSpeed);
	strStatus.Format("%d개 - %d kB/s", m_myIOCPSocket.m_uploadFileDataList.GetCount(), m_cPreferencesValue.m_nUploadSpeed);
	m_wndStatusBar.SetPaneText(2, strStatus);

	return TRUE;
	
}

// 운영체제가관리하는 아이콘얻기
void CMainFrame::GetSysImgList(CListCtrl &listCtrl, CImageList &imageList)
{
	// 운영체제가 관리하는 작은 아이콘 가져오기
    HIMAGELIST  hSystemImgList;
    SHFILEINFO  info;
 
    hSystemImgList = (HIMAGELIST)::SHGetFileInfo(NULL, 0, &info, 
		   sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    
	imageList.Attach(hSystemImgList);    
	
    // 아이콘 연결
    listCtrl.SetImageList(&imageList, LVSIL_SMALL);
	imageList.Detach();
}


