#ifndef __DFFILESERVER_H__
#define __DFFILESERVER_H__

#pragma once

#include "..\DFOpenMindProtocol.h"
#include "winsock2.h"

#define STATE_CLOSED		0	// 연결되지 않은 상태. 초기값.
#define STATE_CONNECTED		1	// 연결된 상태.
#define STATE_XFERING		2	// 전송중인 상태
#define STATE_FINISHED		3	// 전송이 끝난 상태
#define STATE_ERROR			4	// 에러 발생.

#define FILE_XFER_PORT		8000

// 송수신 상태를 표현할 구조체.
typedef struct
{
	DWORD	dwState;
	TCHAR	tszUserID[USER_ID_LENGTH+1];
	TCHAR	tszFileName[MAX_PATH];
	DWORD	dwFileSize;
	DWORD	dwXferedSize;
} XFER_STATUS, *PXFER_STATUS;

// 송신을 위해 connect의 accept를 하는 클래스.
class DFFileServer
{
public:
	DFFileServer(DWORD dwMaxConn, DWORD dwMaxUser);
	~DFFileServer();

	DWORD	Listen();
	void	Stop(){CloseSocket();};
private:
	BOOL	CreateListenSock();
	BOOL	CreateListenThread();
	void	CloseSocket();

	SOCKET	m_Sock;
	HANDLE	m_hThread;
	DWORD	m_dwMaxConn;
	DWORD	m_dwMaxUser;
};


// 송수신 마다 전송상태 다이얼로그에 WM_COPYDATA 메시지를 쏜다.
// 송수신 취소를 위해서는 클래스를 삭제하면 된다. 삭제를 위해 FileServer객체는
// Sender와 Recver의 리스트 혹은 Map을 가진다.

// FileSender/FileRecver class(use overlapped io with completion routine)
class DFFileSender
{
public:
	DFFileSender(SOCKET Sock);
	~DFFileSender();

	void			Disconnect(){CloseSocket(TRUE);};
	DWORD			Transfer();

	XFER_STATUS		m_XferStatus;
private:
	void			CloseSocket(BOOL bGraceful = FALSE);
	BOOL			RecvFileInfo();
	BOOL			SendResult();
	BOOL			SendFile();
	BOOL			CreateXferThread();
	void			PostMsgToStatusDlg();
	void			PostMsgToMainframe();

	SOCKET			m_Sock;
	HANDLE			m_hThread;
	HANDLE			m_hFile;

	TCHAR			m_FullPath[MAX_PATH];
};

class DFFileRecver
{
public:
	DFFileRecver(DWORD dwPeerIP, LPCTSTR tszFile, LPCTSTR tszUserID);
	~DFFileRecver();

	void			Disconnect(){CloseSocket(TRUE);};
	DWORD			Transfer();

	XFER_STATUS		m_XferStatus;
private:
	BOOL			CreateAndConnect();
	void			CloseSocket(BOOL bGraceful = FALSE);
	BOOL			SendFileInfo();
	BOOL			RecvResult();
	BOOL			RecvFile();
	BOOL			CreateXferThread();
	void			PostMsgToStatusDlg();
	void			PostMsgToMainframe();

	SOCKADDR_IN		m_Peer_sin;	
	SOCKET			m_Sock;
	HANDLE			m_hThread;
	HANDLE			m_hFile;

	TCHAR			m_FullPath[MAX_PATH];
};

#endif


