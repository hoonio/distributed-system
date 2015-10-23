#ifndef __DFFILESERVER_H__
#define __DFFILESERVER_H__

#pragma once

#include "..\DFOpenMindProtocol.h"
#include "winsock2.h"

#define STATE_CLOSED		0	// ������� ���� ����. �ʱⰪ.
#define STATE_CONNECTED		1	// ����� ����.
#define STATE_XFERING		2	// �������� ����
#define STATE_FINISHED		3	// ������ ���� ����
#define STATE_ERROR			4	// ���� �߻�.

#define FILE_XFER_PORT		8000

// �ۼ��� ���¸� ǥ���� ����ü.
typedef struct
{
	DWORD	dwState;
	TCHAR	tszUserID[USER_ID_LENGTH+1];
	TCHAR	tszFileName[MAX_PATH];
	DWORD	dwFileSize;
	DWORD	dwXferedSize;
} XFER_STATUS, *PXFER_STATUS;

// �۽��� ���� connect�� accept�� �ϴ� Ŭ����.
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


// �ۼ��� ���� ���ۻ��� ���̾�α׿� WM_COPYDATA �޽����� ���.
// �ۼ��� ��Ҹ� ���ؼ��� Ŭ������ �����ϸ� �ȴ�. ������ ���� FileServer��ü��
// Sender�� Recver�� ����Ʈ Ȥ�� Map�� ������.

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


