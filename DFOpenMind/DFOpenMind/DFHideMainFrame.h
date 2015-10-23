
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

// ������Ʈ���� ����� ����ü.
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
	
	// Ŭ���̾�Ʈ ����!!
	DFClientSock		m_ClientSock;			// ������ �����Ͽ� ����� ���� Ŭ����.
	CMapStringToPtr		m_UserMap;				// ������ ���� ���۹��� ����� ������ �̸��� ����.
	CPtrList			m_FileInfoList;			// �˻� ����� �����Ѵ�!!
	CPtrList			m_LocalFileList;		// ���� ���� ���� ������ �����Ѵ�.
	DFFolderMonitor*	m_pFolderMonitor; 
	DFFileServer*		m_pFileServer;			// File Send ���� Listenner.
	CString				m_strUserName;
	CString				m_strServerIP;

	CPtrList			m_DownList;
	CPtrList			m_UpList;

	// ������ ����/����
	BOOL			ConnectToServer(LPCTSTR tszServerIP);
	void			CloseConnection();
	void			OnCloseSock();

	// ������ �޽��� �۽�/����!
	void			RecvMsg(BYTE* pBuf, UINT nSize);
	int				SendMsg(BYTE* pBuf, UINT nSize);
	void			LogonProcess();
	void			SendLocalShareFileInfo();
	void			CollectShareFileInfo(LPCTSTR tszUserID);

	// ��û �޽����� �ۼ��ϴ� �Լ�,
	int				SendLoginMsg(CString strUserID, CString strPassword);
	int				SendNewUserMsg(	CString strID, CString strPwd, CString strAddr, 
									CString strPhone, CString strMail);

	int				SendFileInfoMsg(FILE_INFO* pFileInfo, UINT nAmount);
	int				SendSearchMsg(FILE_FIND_INFO& FileFindInfo);
	int				SendHeartBeat();

	// ���� �޽��� ó���Լ�.
	void			OnRecvLogin(BYTE* pBuf, UINT nSize);
	void			OnRecvLoginUsers(BYTE* pBuf, UINT nSize);
	void			OnRecvLogOut(BYTE* pBuf, UINT nSize);
	void			OnRecvNewUser(BYTE* pBuf, UINT nSize);
	void			OnRecvSearchResult(BYTE* pBuf, UINT nSize);

	// ���� ���� ��ȭ ��/./
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
