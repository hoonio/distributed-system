
#if !defined(AFX_DFHIDEMAINFRAME_H__E1EAEB1F_3DAA_11D2_8B94_006097AD9DDC__INCLUDED_)
#define AFX_DFHIDEMAINFRAME_H__E1EAEB1F_3DAA_11D2_8B94_006097AD9DDC__INCLUDED_
#include "DFOpenMindDlg.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFHideMainFrame.h : header file
//
#include "..\DFOpenMindProtocol.h"
#include "DFFolderMonitor.h"
#include "DFFileServer.h"

/////////////////////////////////////////////////////////////////////////////
// CHideMainFrame frame
#define ID_TRAYICON_NOTIFY	WM_USER+100
#define WM_FOLDERCHANGED	WM_USER+101
#define WM_RECV_FIN			WM_USER+102
#define WM_SEND_FIN			WM_USER+103
#define SERVER_IP			TEXT("211.41.177.108")
#define SERVER_PORT			7777

// 레지스트리에 저장될 구조체.
typedef struct 
{
	BYTE	btMaxUser;
	BYTE	btMaxUploads;
	BYTE	btMaxDownloads;
	TCHAR	tszShareFolder[MAX_PATH];
	TCHAR	tszDownloadFolder[MAX_PATH];
} REG_CONFIG_INFO;

typedef struct
{
	BOOL	bAutoLogin;
	TCHAR	tszUserID[13];
	TCHAR	tszPassword[9];
} REG_USER_INFO;

typedef struct
{
	DWORD	dwIP;
	TCHAR	tszUserID[13];
} CLIENT_USER_INFO, *PCLIENT_USER_INFO;

#include "DFClientSock.h"


class DFHideMainFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(DFHideMainFrame)

protected:

// Attributes
public:
	DFHideMainFrame();           // protected constructor used by dynamic creation
	CDFOpenMindDlg*			m_pDlg;
	CMenu*					m_pMenu;
	BOOL					m_bDeleted;
	BOOL					m_bConnected;
	
	// 클라이언트 소켓!!
	DFClientSock		m_ClientSock;			// 서버와 연결하여 통신할 소켓 클래스.
	CMapStringToPtr		m_UserMap;				// 서버로 부터 전송받은 연결된 유저의 이름을 관리.
	CPtrList			m_FileInfoList;			// 검색 결과를 보관한다!!
	CPtrList			m_LocalFileList;		// 로컬 공유 파일 정보를 보관한다.
	DFFolderMonitor*	m_pFolderMonitor; 
	DFFileServer*		m_pFileServer;			// File Send 위한 Listenner.
	CString				m_strUserName;
	CString				m_strServerIP;

	CPtrList			m_DownList;
	CPtrList			m_UpList;

	// 서버와 연결/종료
	BOOL			ConnectToServer(LPCTSTR tszServerIP);
	void			CloseConnection();
	void			OnCloseSock();

	// 서버와 메시지 송신/수신!
	void			RecvMsg(BYTE* pBuf, UINT nSize);
	int				SendMsg(BYTE* pBuf, UINT nSize);
	void			LogonProcess();
	void			SendLocalShareFileInfo();
	void			CollectShareFileInfo(LPCTSTR tszUserID);

	// 요청 메시지를 작성하는 함수,
	int				SendLoginMsg(CString strUserID, CString strPassword);
	int				SendNewUserMsg(	CString strID, CString strPwd, CString strAddr, 
									CString strPhone, CString strMail);

	int				SendFileInfoMsg(FILE_INFO* pFileInfo, UINT nAmount);
	int				SendSearchMsg(FILE_FIND_INFO& FileFindInfo);
	int				SendHeartBeat();

	// 수신 메시지 처리함수.
	void			OnRecvLogin(BYTE* pBuf, UINT nSize);
	void			OnRecvLoginUsers(BYTE* pBuf, UINT nSize);
	void			OnRecvLogOut(BYTE* pBuf, UINT nSize);
	void			OnRecvNewUser(BYTE* pBuf, UINT nSize);
	void			OnRecvSearchResult(BYTE* pBuf, UINT nSize);

	// 공유 폴더 변화 시/./
	void			RefleshLocalFiles();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHideMainFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~DFHideMainFrame();

	// Generated message map functions
	//{{AFX_MSG(CHideMainFrame)

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnExit();
	afx_msg void OnShowdialog();
	afx_msg void OnDestroy();

	//}}AFX_MSG
	afx_msg LONG OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFolderChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRecvFin(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSendFin(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFHIDEMAINFRAME_H__E1EAEB1F_3DAA_11D2_8B94_006097AD9DDC__INCLUDED_)
