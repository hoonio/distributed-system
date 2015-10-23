// HideMainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFOpenMind.h"
#include "DFHideMainFrame.h"
#include "DFLoginDialog.h"
#include "DFNewUserDlg.h"
#include "..\DFOpenMindProtocol.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// global variables..
REG_CONFIG_INFO g_Config;
DFHideMainFrame* g_pMainFrame = NULL; 


BOOL UpdateConfigRegistry(REG_CONFIG_INFO& ConfigInfo);
BOOL UpdateUserRegistry(REG_USER_INFO& UserInfo);
BOOL RemoveRegistry();
BOOL GetConfigRegistry(REG_CONFIG_INFO& Info);
BOOL GetUserRegistry(REG_USER_INFO& Info);



BOOL UpdateConfigRegistry(REG_CONFIG_INFO& ConfigInfo)
{
	HKEY	hKey;
	LONG	lRet = 0;

	__try
	{
		lRet = RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
								TEXT("SOFTWARE\\DFOpenMind"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_ALL_ACCESS,
								NULL,
								&hKey,
								NULL);
		if(lRet != ERROR_SUCCESS) __leave;

		lRet = RegSetValueEx(	hKey, 
								TEXT("Config"), 
								0, 
								REG_BINARY, 
								(LPBYTE)&ConfigInfo, 
								sizeof(REG_CONFIG_INFO));
		if(lRet != ERROR_SUCCESS) __leave;
	}

	__finally
	{
		RegCloseKey(hKey);
		return (lRet == ERROR_SUCCESS);
	}
}

BOOL UpdateUserRegistry(REG_USER_INFO& UserInfo)
{
	HKEY	hKey;
	LONG	lRet = 0;

	__try
	{
		lRet = RegCreateKeyEx(	HKEY_LOCAL_MACHINE,
								TEXT("SOFTWARE\\DFOpenMind"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_ALL_ACCESS,
								NULL,
								&hKey,
								NULL);
		if(lRet != ERROR_SUCCESS) __leave;

		lRet = RegSetValueEx(	hKey, 
								TEXT("User"), 
								0, 
								REG_BINARY, 
								(LPBYTE)&UserInfo, 
								sizeof(REG_USER_INFO));
		if(lRet != ERROR_SUCCESS) __leave;
	}

	__finally
	{
		RegCloseKey(hKey);
		return (lRet == ERROR_SUCCESS);
	}
}

BOOL RemoveRegistry()
{
	LONG	lRet;
	lRet = RegDeleteKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\DFOpenMind"));
	return (lRet == ERROR_SUCCESS);
}

BOOL GetConfigRegistry(REG_CONFIG_INFO& Info)
{
	HKEY	hKey;
	LONG	lRet;
	
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
						TEXT("SOFTWARE\\DFOpenMind"), 
						0, 
						KEY_ALL_ACCESS, 
						&hKey);

	if(lRet == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(Info);

		lRet = RegQueryValueEx(	hKey,
								TEXT("Config"), 
								0,
								NULL,
								(LPBYTE) &Info,
								&dwSize);

		RegCloseKey(hKey);

		return (ERROR_SUCCESS == lRet); 
	}
	else return FALSE;
}

BOOL GetUserRegistry(REG_USER_INFO& Info)
{
	HKEY	hKey;
	LONG	lRet;
	
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
						TEXT("SOFTWARE\\DFOpenMind"), 
						0, 
						KEY_ALL_ACCESS, 
						&hKey);

	if(lRet == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(Info);

		lRet = RegQueryValueEx(	hKey,
								TEXT("User"), 
								0,
								NULL,
								(LPBYTE) &Info,
								&dwSize);

		RegCloseKey(hKey);

		return (ERROR_SUCCESS == lRet); 
	}
	else return FALSE;
}

BOOL GetServerIPRegistry(LPTSTR tszServerIP)
{
	HKEY	hKey;
	LONG	lRet;
	
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
						TEXT("SOFTWARE\\DFOpenMind"), 
						0, 
						KEY_ALL_ACCESS, 
						&hKey);

	if(lRet == ERROR_SUCCESS)
	{
		DWORD dwSize = 32;
		DWORD dwType = REG_SZ;

		lRet = RegQueryValueEx(	hKey,
								TEXT("ServerIP"), 
								0,
								&dwType,
								(LPBYTE) tszServerIP,
								&dwSize);

		RegCloseKey(hKey);

		return (ERROR_SUCCESS == lRet); 
	}
	else return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// DFHideMainFrame

IMPLEMENT_DYNCREATE(DFHideMainFrame, CFrameWnd)

DFHideMainFrame::DFHideMainFrame() : m_ClientSock(m_hWnd)
{
	m_strUserName = "";
	m_pFolderMonitor = NULL;
	m_pFileServer	 = NULL;
	Create(NULL,"DFOpenMind", WS_OVERLAPPED);
}

DFHideMainFrame::~DFHideMainFrame()
{
}

BEGIN_MESSAGE_MAP(DFHideMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CHideMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_SHOWDIALOG, OnShowdialog)
	ON_WM_DESTROY()

	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_TRAYICON_NOTIFY, OnTrayIconNotify)
	ON_MESSAGE(WM_FOLDERCHANGED, OnFolderChanged)
	ON_MESSAGE(WM_SEND_FIN, OnSendFin)
	ON_MESSAGE(WM_RECV_FIN, OnRecvFin)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHideMainFrame message handlers

int DFHideMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	g_pMainFrame = this;
	m_bConnected = FALSE;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	NOTIFYICONDATA data;

	data.cbSize				= sizeof(NOTIFYICONDATA);
	data.hWnd				= m_hWnd;
	data.uID				= IDR_MAINFRAME;
	data.uFlags				= NIF_ICON | NIF_MESSAGE | NIF_TIP;
	data.uCallbackMessage	= ID_TRAYICON_NOTIFY;

	data.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	lstrcpy(data.szTip, TEXT("DFOpenMind"));
	
	Shell_NotifyIcon(NIM_ADD, &data);
	
	m_pDlg	= NULL;
	m_pMenu	= new CMenu;
	m_pMenu->LoadMenu(IDR_MENU1);

	m_bDeleted = FALSE;

	// 서버의 IP 정보를 로드한다.
	TCHAR	tszServerIP[32];
	if(!GetServerIPRegistry((LPTSTR)tszServerIP))
	{
		m_strServerIP = SERVER_IP;
	}
	else
	{
		m_strServerIP = tszServerIP;
	}

	// 설정 정보를 로드한다.
	if(!GetConfigRegistry(g_Config))
	{
		TCHAR	tszCurrentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, tszCurrentDir);

		wsprintf(g_Config.tszDownloadFolder, TEXT("%s\\Downloads"), tszCurrentDir); 
		wsprintf(g_Config.tszShareFolder, TEXT("%s\\Share"), tszCurrentDir);

		CreateDirectory(g_Config.tszDownloadFolder, NULL);
		CreateDirectory(g_Config.tszShareFolder, NULL);

		g_Config.btMaxDownloads = 30;
		g_Config.btMaxUploads = 30;
		g_Config.btMaxUser = 10;
		
		UpdateConfigRegistry(g_Config);
	}

	// 서버에 연결한다.
	if(!ConnectToServer((LPCTSTR)m_strServerIP)) 
	{
		MessageBox("Connection To Server Failed", "Connect", MB_OK|MB_ICONERROR);
		PostMessage(WM_CLOSE, 0, 0);	// 바로 가차없이 죽인다.
		return 0;
	}

	m_bConnected = TRUE;
	return 0;
}

void DFHideMainFrame::OnClose() 
{
	NOTIFYICONDATA data;

	data.cbSize = sizeof(NOTIFYICONDATA);
	data.hWnd = m_hWnd;
	data.uID = IDR_MAINFRAME;
	
	Shell_NotifyIcon(NIM_DELETE, &data);

	m_bDeleted = TRUE;

	CFrameWnd::OnClose();
}

LONG DFHideMainFrame::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	POINT point;
	CMenu *sMenu;
	switch(lParam)
	{
		//좌측 마우스 버튼을 클릭하면 대화상자가 실행된다.
	case WM_LBUTTONDOWN :	
		//현재 대화상자가 실행되면 아무일도 하지 않는다.
		if(m_pDlg)
		{
			m_pDlg->SetFocus();
			break;
		}
		m_pDlg= new CDFOpenMindDlg;
		//대화상자에서 EXIT버튼을 클릭하면 종료한다.
		if(m_pDlg->DoModal() == IDC_EXIT)
		{
			SendMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}
		//대화상자 삭제
		if(m_pDlg && !m_bDeleted)
		{
			delete m_pDlg;
			m_pDlg = NULL;
		}
		break;

		//우측 마우스 버튼을 클릭하면
	case WM_RBUTTONDOWN:
		//pMenu의 서브 메뉴를 얻고
		sMenu = m_pMenu->GetSubMenu(0);
		//마우스 위치를 얻는다.
		GetCursorPos(&point);
		//TrackPopupMenu함수를 실행하여 팝업 메뉴를 구동한다.
		sMenu->TrackPopupMenu(	TPM_RIGHTBUTTON|TPM_LEFTALIGN,
								point.x,
								point.y,
								this,
								CRect(point.x - 100, point.y - 100, point.x, point.y));
		break;
	}
	return TRUE;
}

//컨텍스트메뉴에서 "끝내기" 선택
void DFHideMainFrame::OnExit() 
{
	if(AfxMessageBox("Do You Want To Exit DFOpenMind?", MB_YESNO|MB_DEFBUTTON2) == IDYES)
	{
		if(m_pFolderMonitor)
		{
			delete m_pFolderMonitor; 
			m_pFolderMonitor = NULL;
		}

		// 유저 정보 제거한다.
		CString Key;
		PCLIENT_USER_INFO pUserInfo;

		POSITION pos = m_UserMap.GetStartPosition();
		while(pos)
		{
			m_UserMap.GetNextAssoc(pos, Key, (void*&)pUserInfo);
			if(pUserInfo) delete pUserInfo;
		}

		m_UserMap.RemoveAll();

		// 파일 서버 제거한다.
		delete m_pFileServer; m_pFileServer = NULL;

		// 현재 송수신 중인 객체를 제거한다.

		pos = m_UpList.GetHeadPosition();
		while(pos)
		{
			DFFileSender* pSender = (DFFileSender*)m_UpList.GetNext(pos);
			delete pSender;
		}
		m_UpList.RemoveAll();

		pos = m_DownList.GetHeadPosition();
		while(pos)
		{
			DFFileRecver* pRecver = (DFFileRecver*)m_DownList.GetNext(pos);
			delete pRecver;
		}
		m_DownList.RemoveAll();
		

		PostMessage(WM_CLOSE, 0, 0);
	}
}

//컨텍스트 메뉴에서 "대화상자" 선택
void DFHideMainFrame::OnShowdialog() 
{
	if(m_pDlg)
	{
		m_pDlg->SetFocus();
		return;
	}
	m_pDlg = new CDFOpenMindDlg;
	if(m_pDlg->DoModal() == IDC_EXIT)
	{
		SendMessage(WM_CLOSE, 0, 0);
		return;
	}

	if(m_pDlg)
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}	
}

void DFHideMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
	if(m_pMenu)
	{
		m_pMenu->DestroyMenu();
		delete m_pMenu;
		m_pMenu = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL DFHideMainFrame::ConnectToServer(LPCTSTR tszServerIP)
{
	if(m_ClientSock.Create())
	{
		if(m_ClientSock.AsyncSelect(FD_READ|FD_WRITE|FD_CLOSE|FD_CONNECT))
		{
			if(m_ClientSock.Connect(tszServerIP, SERVER_PORT))
				return TRUE;
			else if(m_ClientSock.GetLastError() != WSAEWOULDBLOCK)
			{
				m_ClientSock.Close();
				return FALSE;
			}
			else return TRUE;
		}
	}			

	return FALSE;
}

void DFHideMainFrame::OnCloseSock()
{
	if(m_pDlg)
	{
		m_pDlg->GetDlgItem(IDC_CONNECT)->EnableWindow();
	}
}

void DFHideMainFrame::CloseConnection()
{
	m_ClientSock.Close();
}

void DFHideMainFrame::LogonProcess()
{
	if(m_pDlg)
	{
		m_pDlg->GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
	}

	// 저장되어 있는 레지스트리를 확인한다.
	REG_USER_INFO Info;
	if(!GetUserRegistry(Info))
	{
		// 저장된 값이 없다면 새로운 유저 생성 다이얼로그를 띄운다.
		DFNewUserDlg* pNewUserDlg = new DFNewUserDlg;
		if(pNewUserDlg->DoModal() == IDOK)
		{
			SendNewUserMsg(	pNewUserDlg->m_strID,
							pNewUserDlg->m_strPwd,
							pNewUserDlg->m_strAddr,
							pNewUserDlg->m_strPhone,
							pNewUserDlg->m_strMail);

			// 레지스트리를 업데이트한다.
			lstrcpy(Info.tszUserID, pNewUserDlg->m_strID);
			lstrcpy(Info.tszPassword, pNewUserDlg->m_strPwd);
			Info.bAutoLogin = FALSE;
			UpdateUserRegistry(Info);
		}
		delete pNewUserDlg;
	}
	else
	{
		// 로긴 다이얼로그를 띄워 서버에 로긴한다.
		DFLoginDialog* pLoginDlg = new DFLoginDialog(Info.tszUserID, Info.tszPassword, Info.bAutoLogin);

		if(pLoginDlg->DoModal() == IDOK)	// 로긴 시도한 경우!!
		{
			m_strUserName = Info.tszUserID;

			//ID와 PWD를 저장하는 경우.
			if(pLoginDlg->m_bChecked == TRUE)
			{
				REG_USER_INFO UserInfo;
				UserInfo.bAutoLogin = TRUE;
				lstrcpy(UserInfo.tszUserID, (LPCTSTR)pLoginDlg->m_strUserID);
				lstrcpy(UserInfo.tszPassword, (LPCTSTR)pLoginDlg->m_strPassword);

				UpdateUserRegistry(UserInfo);
			}
			else
			{
				REG_USER_INFO UserInfo;
				UserInfo.bAutoLogin = FALSE;
				lstrcpy(UserInfo.tszUserID, TEXT(""));
				lstrcpy(UserInfo.tszPassword, TEXT(""));

				UpdateUserRegistry(UserInfo);
			}

			// 실제 로긴 메시지를 송신한다!
			SendLoginMsg(pLoginDlg->m_strUserID, pLoginDlg->m_strPassword);
		}
		else
		{
			m_ClientSock.Close();
		}
		
		delete pLoginDlg;
	}
}

// 수신 처리!!
void DFHideMainFrame::RecvMsg(BYTE* pBuf, UINT nSize)
{
	switch(pBuf[0])
	{
	case DFOM_SC_LOGIN :
		OnRecvLogin(pBuf+1, nSize-1);
		break;
	case DFOM_SC_LOGINUSERS :
		OnRecvLoginUsers(pBuf+1, nSize-1);
		break;
	case DFOM_SC_LOGOUT :
		OnRecvLogOut(pBuf+1, nSize-1);
		break;
	case DFOM_SC_NEWUSER :
		OnRecvNewUser(pBuf+1, nSize-1);
		break;
	case DFOM_SC_SEARCH_RESULTS :
		OnRecvSearchResult(pBuf+1, nSize-1);
		break;
	case DFOM_SC_ALIVE :
		SendHeartBeat();
		break;
	default:
		break;
	}
}

inline int DFHideMainFrame::SendMsg(BYTE* pBuf, UINT nSize)
{
	return m_ClientSock.SendMsg(pBuf, nSize);
}

// 메시지 송신에 관한 함수들...
int DFHideMainFrame::SendLoginMsg(CString strUserID, CString strPassword)
{
	BYTE	Msg[4096] = {0,};

	Msg[0] = DFOM_CS_LOGIN;
	lstrcpyn((LPTSTR)(Msg + 1), (LPCTSTR)strUserID, 12);
	lstrcpyn((LPTSTR)(Msg + 1 + sizeof(TCHAR)*13), (LPCTSTR) strPassword, 8);

	return SendMsg(Msg, sizeof(TCHAR)*22+1);
}

int DFHideMainFrame::SendNewUserMsg(CString strID, CString strPwd, CString strAddr, 
									CString strPhone, CString strMail)
{
	BYTE	Msg[4096] = {0,};

	Msg[0] = DFOM_CS_NEWUSER;
	lstrcpyn((LPTSTR)(Msg+1), (LPCTSTR)strID, 12);
	lstrcpyn((LPTSTR)(Msg+1+sizeof(TCHAR)* 13), (LPCTSTR)strPwd, 8);
	lstrcpyn((LPTSTR)(Msg+1+sizeof(TCHAR)* 22), (LPCTSTR)strAddr, 60);
	lstrcpyn((LPTSTR)(Msg+1+sizeof(TCHAR)* 83), (LPCTSTR)strPhone, 16);
	lstrcpyn((LPTSTR)(Msg+1+sizeof(TCHAR)* 100), (LPCTSTR)strMail, 40);

	return SendMsg(Msg, 1+sizeof(TCHAR)* 141);
}

int DFHideMainFrame::SendFileInfoMsg(FILE_INFO* pFileInfo, UINT nAmount)
{
	BYTE	Msg[4096] = {0,};

	Msg[0] = DFOM_CS_SENDINFOS;
	memcpy(Msg + 1, pFileInfo, nAmount*sizeof(FILE_INFO));

	return SendMsg(Msg, 1+nAmount*sizeof(FILE_INFO));
}

int DFHideMainFrame::SendSearchMsg(FILE_FIND_INFO& FileFindInfo)
{
	BYTE	Msg[4096] = {0,};

	Msg[0] = DFOM_CS_SEARCH;
	*(WORD*)(Msg+1) = 100;		// 100개 까지 한번에 검색하도록..
	memcpy(Msg+3, &FileFindInfo, sizeof(FILE_FIND_INFO));

	return SendMsg(Msg, sizeof(FILE_FIND_INFO)+3);
}

int DFHideMainFrame::SendHeartBeat()
{
	BYTE	Msg = DFOM_CS_ALIVE;
	return SendMsg(&Msg, 1);
}


void DFHideMainFrame::CollectShareFileInfo(LPCTSTR tszUserID)
{
	CFileFind finder;
	PFILE_INFO pFileInfo = NULL;
	CTime	Time;

	// 공유 디렉토리로 현재 디렉토리의 위치를 변경한다.
	
	SetCurrentDirectory(g_Config.tszShareFolder);

	BOOL bWorking = finder.FindFile("*.*");
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		if(!finder.IsDirectory() && !finder.IsDots())
		{
			pFileInfo = new FILE_INFO;

			lstrcpy(pFileInfo->tszUserID, tszUserID);
			lstrcpy(pFileInfo->tszFileName, (LPCTSTR) finder.GetFileName());
			finder.GetLastWriteTime(&pFileInfo->ftLastWriteTime);
			finder.GetCreationTime(&pFileInfo->ftCreationTime);
			finder.GetLastAccessTime(&pFileInfo->ftLastAccessTime);
			pFileInfo->dwFileSizeHigh	= 0;
			pFileInfo->dwFileSizeLow	= finder.GetLength();
			
			m_LocalFileList.AddTail(pFileInfo);
		}
	}

	// share 폴더를 감시하기 시작한다.
	m_pFolderMonitor = new DFFolderMonitor(g_Config.tszShareFolder);

	// app가 있는 디렉토리로 현재 디렉토리를 변경한다.
	SetCurrentDirectory(TEXT(".."));
}

void DFHideMainFrame::SendLocalShareFileInfo()
{
	PFILE_INFO pFileInfo = NULL;
	POSITION pos = m_LocalFileList.GetHeadPosition();
	while(pos)
	{
		pFileInfo = (PFILE_INFO)m_LocalFileList.GetNext(pos);
		SendFileInfoMsg(pFileInfo, 1);		// 일단 하나씩 보내자.. ㅠㅜ
	}
}

void DFHideMainFrame::OnRecvLogin(BYTE* pBuf, UINT nSize)
{
	if(pBuf[0] != 1)
	{
		switch(pBuf[1])
		{
		case FAIL_USER_ALREADY_LOGIN:
			MessageBox(TEXT("Already Logged User!"), TEXT("Login Fail"), MB_OK|MB_ICONERROR);
			break;
		case FAIL_ID_NOT_EXIST :
			MessageBox(TEXT("User ID not Exist"), TEXT("Login Fail"), MB_OK|MB_ICONERROR);
			break;
		case FAIL_PASSWORD_MISMATCH:
			MessageBox(TEXT("Password Mismatch"), TEXT("Login Fail"), MB_OK|MB_ICONERROR);
			break;
		default :
			break;
		}
	}
	else
	{
		m_strUserName = (LPTSTR)pBuf+2;

		//MessageBox(TEXT("Successfully Logged"), TEXT("Login"), MB_OK|MB_ICONINFORMATION);
//		LOGOUT("CLIENT: Successfully Logged");
		// 이 클라이언트가 공유한 파일의 리스트를 작성한다.
		CollectShareFileInfo((LPTSTR)pBuf+2);

		m_pFileServer = new DFFileServer(g_Config.btMaxUploads, g_Config.btMaxUser);
		
		// 이 클라이언트가 공유한 파일의 리스트를 송신한다.
		SendLocalShareFileInfo();
	}
}

void DFHideMainFrame::OnRecvLoginUsers(BYTE* pBuf, UINT nSize)
{
	BYTE*	pLoc = pBuf + 2;

	for(int i = 0; i < (int)pBuf[0]; i++)
	{
		PCLIENT_USER_INFO pUserInfo = new CLIENT_USER_INFO;
		memcpy(&pUserInfo->dwIP, pLoc, sizeof(DWORD));
		pLoc += sizeof(DWORD);
		lstrcpy(pUserInfo->tszUserID, (TCHAR*)pLoc);
		pLoc += sizeof(TCHAR)*(USER_ID_LENGTH+1);

//		LOGOUT("CLIENT : Logged User  %s", pUserInfo->tszUserID);
		
		m_UserMap[pUserInfo->tszUserID] = pUserInfo;

		if(m_pDlg)	// 다이얼로그가 활성화되어 있다면... 트리에 아이디를 넣는다.
		{
			m_pDlg->AddUser(pUserInfo->tszUserID);
		}
	}
}

void DFHideMainFrame::OnRecvLogOut(BYTE* pBuf, UINT nSize)
{
	PCLIENT_USER_INFO pUserInfo = NULL;

	m_UserMap.Lookup((LPCTSTR)pBuf, (void*&)pUserInfo);
	if(pUserInfo)
	{
		if(m_pDlg)
		{
			m_pDlg->DeleteUser(pUserInfo->tszUserID);
		}

		m_UserMap.RemoveKey(pUserInfo->tszUserID);
		delete pUserInfo;
	}
}

void DFHideMainFrame::OnRecvNewUser(BYTE* pBuf, UINT nSize)
{
	if(pBuf[0] == 1)
	{
		LogonProcess();
	}
	else
	{
		switch(pBuf[1])
		{
		case FAIL_MAX_USERS :
			MessageBox(TEXT("Over Maximum Users!"), TEXT("New User Fail"), MB_OK|MB_ICONERROR);
			break;
		case FAIL_ID_ALREADY_EXIST :
			MessageBox(TEXT("User Exists!"), TEXT("New User Fail"), MB_OK|MB_ICONERROR);
			break;
		default:
			break;
		}
	}
}

void DFHideMainFrame::OnRecvSearchResult(BYTE* pBuf, UINT nSize)
{
	if(*(WORD*)pBuf == 0)
	{
		MessageBox(TEXT("No Matching Files"), TEXT("Search"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	PFILE_INFO pFileInfo = new FILE_INFO;
	memcpy(pFileInfo, pBuf+4, sizeof(FILE_INFO));

	// 첫 메시지.. 리스트 컨트롤을 지우고 새로 넣는다.
	if(*(WORD*)(pBuf+2) == 1)
	{
		while(m_FileInfoList.GetCount())
		{
			PFILE_INFO pTemp = (PFILE_INFO)m_FileInfoList.RemoveHead();
			delete pTemp;
		}

		if(m_pDlg)
		{
			m_pDlg->RemoveAllList();
		}
	}

	
	m_FileInfoList.AddTail(pFileInfo);
	if(m_pDlg)
	{
		m_pDlg->AddFile(pFileInfo);
#ifdef _DEBUG
//	LOGOUT("CLIENT : SEARCH RESULT!!");
	
#endif
	}
}

void DFHideMainFrame::OnFolderChanged(WPARAM wParam, LPARAM lParam)
{
	CFileFind finder;
	PFILE_INFO pFileInfo = NULL;
	CTime	Time;

	// 기존의 폴더 정보를 제거한다.
	POSITION pos = m_LocalFileList.GetHeadPosition();
	while(pos)
	{
		pFileInfo = (PFILE_INFO)m_LocalFileList.GetNext(pos);
		if(pFileInfo) delete pFileInfo;
	}

	m_LocalFileList.RemoveAll();

	// 공유 디렉토리로 현재 디렉토리의 위치를 변경한다.
	
	SetCurrentDirectory(g_Config.tszShareFolder);

	BOOL bWorking = finder.FindFile("*.*");
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		if(!finder.IsDirectory() && !finder.IsDots())
		{
			pFileInfo = new FILE_INFO;

			lstrcpy(pFileInfo->tszUserID, (LPCTSTR)m_strUserName);
			lstrcpy(pFileInfo->tszFileName, (LPCTSTR) finder.GetFileName());
			finder.GetLastWriteTime(&pFileInfo->ftLastWriteTime);
			finder.GetCreationTime(&pFileInfo->ftCreationTime);
			finder.GetLastAccessTime(&pFileInfo->ftLastAccessTime);
			pFileInfo->dwFileSizeHigh	= 0;
			pFileInfo->dwFileSizeLow	= finder.GetLength();
			
			m_LocalFileList.AddTail(pFileInfo);
		}
	}

	// app가 있는 디렉토리로 현재 디렉토리를 변경한다.
	SetCurrentDirectory(TEXT(".."));
}

void DFHideMainFrame::RefleshLocalFiles()
{
	PostMessage(WM_FOLDERCHANGED, 0, 0);
}

// 작업이 끝난 경우 리스트에서 제거한다.
void DFHideMainFrame::OnRecvFin(WPARAM wParam, LPARAM lParam)
{
	DFFileRecver* pRecver = (DFFileRecver*)wParam;
	POSITION pos = m_DownList.Find(pRecver);
	m_DownList.RemoveAt(pos);
	delete pRecver;
}

void DFHideMainFrame::OnSendFin(WPARAM wParam, LPARAM lParam)
{
	DFFileSender* pSender = (DFFileSender*)wParam;
	POSITION pos = m_UpList.Find(pSender);
	m_UpList.RemoveAt(pos);
	delete pSender;
}