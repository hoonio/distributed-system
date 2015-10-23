#include "stdafx.h"
#include "DFFileServer.h"
#include "DFHideMainFrame.h"

extern REG_CONFIG_INFO g_Config;
extern DFHideMainFrame* g_pMainFrame;

DWORD WINAPI RecverThread(LPVOID p);
DWORD WINAPI SenderThread(LPVOID p);
DWORD WINAPI ListenThread(LPVOID p);

////////////////////////////////////////////////////////////////////////////////////
DFFileServer::DFFileServer(DWORD dwMaxConn, DWORD dwMaxUser)
{
	m_dwMaxConn = dwMaxConn;
	m_dwMaxUser = dwMaxUser;
	m_Sock		= INVALID_SOCKET;
	m_hThread	= INVALID_HANDLE_VALUE;

	if(!CreateListenThread()) 
		MessageBox(NULL, TEXT("Listen Thread Creation Failure"), TEXT("File Server"), MB_ICONERROR|MB_OK);
}

DFFileServer::~DFFileServer()
{
	if(m_Sock != INVALID_SOCKET) Stop();
}

BOOL DFFileServer::CreateListenThread()
{
	DWORD	dwThreadId = 0;

	m_hThread = BEGINTHREADEX(NULL, 0, ListenThread, this, CREATE_SUSPENDED, &dwThreadId); 
	if(m_hThread)
	{
		// �켱 ������ ���� ���� �Ͽ� ��Ʈ��ũ �޽����� ������ ������ �Ѵ�.
		// ��¥�� ���� ������� ��κ� ���ŷ ���¿� �����Ƿ� ���ɿ� ������ ����.
		SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
		ResumeThread(m_hThread);
		return TRUE;
	}
	return FALSE;
}

DWORD DFFileServer::Listen()
{
	SOCKET	Sock = INVALID_SOCKET;
	SOCKADDR_IN			ChildSock_sin;
	int					iSize = sizeof(struct sockaddr);

	if(!CreateListenSock()) 
	{
		MessageBox(NULL, TEXT("Listen Socket Creation Failure"), TEXT("File Server"), MB_ICONERROR|MB_OK);
		return 0;
	}

	for(;;)
	{
		Sock = WSAAccept(	m_Sock, 
							(struct sockaddr FAR *)&ChildSock_sin, 
							&iSize, 
							NULL, 
							0);
		if(Sock == INVALID_SOCKET)
		{		
			return 0;
		}

#ifdef _DEBUG
//		LOGOUT(TEXT("SERVER: A File Client Connected"));
#endif
		DFFileSender* pFileSender = new DFFileSender(Sock);
		g_pMainFrame->m_UpList.AddTail(pFileSender);	

	}


	return 0;
}

BOOL DFFileServer::CreateListenSock()
{
	SOCKADDR_IN	si_addr;
	int			nRet;

	// create listen socket.
	m_Sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(m_Sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	// bind listen socket with si_addr struct.
	si_addr.sin_family		= AF_INET;
	si_addr.sin_port		= htons(FILE_XFER_PORT);
	si_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	nRet = bind(m_Sock, (struct sockaddr *) &si_addr, sizeof(si_addr));
	if(nRet == SOCKET_ERROR)
	{	
		return FALSE;
	}

	// start listening..
	nRet = listen(m_Sock, 5);
	if(nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

void DFFileServer::CloseSocket()
{
	if(m_Sock != INVALID_SOCKET) closesocket(m_Sock);
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
DFFileSender::DFFileSender(SOCKET Sock)
{
	m_Sock = Sock;
	m_hThread	= INVALID_HANDLE_VALUE;
	m_hFile		= INVALID_HANDLE_VALUE;

	// ���� ���� ����ü�� �ʱ�ȭ�Ѵ�.
	memset(&m_XferStatus, 0, sizeof(m_XferStatus));
	m_XferStatus.dwXferedSize	= 0;
	m_XferStatus.dwFileSize		= (DWORD)-1;
	m_XferStatus.dwState		= STATE_CONNECTED;

	// ���� ���� ���¸� �����ϵ��� ���� ���̾�α׿� �޽��� ������.
	PostMsgToStatusDlg();

	// ���� �����带 �����Ͽ� ���� �۾��� �����ϵ��� �Ѵ�.
	if(!CreateXferThread()) 
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
	}
}

DFFileSender::~DFFileSender()
{
	if(m_XferStatus.dwState == STATE_CONNECTED || m_XferStatus.dwState == STATE_XFERING)
	{
		Disconnect();
		WaitForSingleObject(m_hThread, INFINITE);
	}
}

BOOL DFFileSender::CreateXferThread()
{
	DWORD	dwThreadId = 0;

	m_hThread = BEGINTHREADEX(NULL, 0, SenderThread, this, CREATE_SUSPENDED, &dwThreadId); 
	if(m_hThread)
	{
		// �켱 ������ ���� ���� �Ͽ� ��Ʈ��ũ �޽����� ������ ������ �Ѵ�.
		// ��¥�� ���� ������� ��κ� ���ŷ ���¿� �����Ƿ� ���ɿ� ������ ����.
		SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
		ResumeThread(m_hThread);
		return TRUE;
	}
	return FALSE;
}

void DFFileSender::CloseSocket(BOOL bGraceful)
{
	if(!bGraceful)
	{
		LINGER LingerStruct;
		LingerStruct.l_onoff  = 1;
		LingerStruct.l_linger = 0;

		setsockopt(m_Sock, SOL_SOCKET, SO_LINGER,
					(char*)&LingerStruct, sizeof(LingerStruct));
	}
	closesocket(m_Sock);
	m_Sock = INVALID_SOCKET;
}

BOOL DFFileSender::RecvFileInfo()
{
	BYTE	MsgBuf[4096] = {0,};
	DWORD	dwMsgSize = 1 + sizeof(TCHAR)*MAX_PATH+13;
	DWORD	dwNumOfByteRead = 0;
	DWORD	dwTotalByteRead = 0;
	DWORD	dwFlag = 0;
	
	WSABUF	WsaBuf;
	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = dwMsgSize;

	do
	{
		int nRet = WSARecv(m_Sock, &WsaBuf, 1, &dwNumOfByteRead, &dwFlag, NULL, NULL);
		if(nRet == SOCKET_ERROR)
		{
			DWORD dwErr = WSAGetLastError();
			if(dwErr != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteRead;
		WsaBuf.len -= dwNumOfByteRead;

		dwTotalByteRead += dwNumOfByteRead;
	} while(dwTotalByteRead < dwMsgSize);

	// �޽��� ����!!
	if(MsgBuf[0] != DFOM_CC_REQUEST)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}
	
	lstrcpy(m_XferStatus.tszFileName, (LPTSTR)MsgBuf+1); 
	lstrcpy(m_XferStatus.tszUserID, (LPTSTR)MsgBuf+1+MAX_PATH);
	PostMsgToStatusDlg();

	// ���ε��� ������ full path�� �ۼ��Ѵ�.
	wsprintf(m_FullPath, TEXT("%s\\%s"), g_Config.tszShareFolder, m_XferStatus.tszFileName);

//#ifdef _DEBUG
//	LOGOUT(TEXT("SENDER: Received File Info Request"));
//#endif

	return TRUE;
}

BOOL DFFileSender::SendResult()
{
	BYTE bRet = 0;

	m_hFile = 
		CreateFile(	m_FullPath, 
					GENERIC_READ, 
					FILE_SHARE_READ, 
					NULL, 
					OPEN_EXISTING, 
					FILE_FLAG_SEQUENTIAL_SCAN, 
					NULL);

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
//#ifdef _DEBUG
//		LOGOUT(TEXT("SERVER : SendFile Open Fail, FullPath: %s, Err:%d"), m_FullPath, GetLastError());
//#endif
		BYTE bRet = 0;
	}
	else bRet = 1;

	// ������ ���� ���ߴٸ� �۽��� �� �����Ƿ� ���� �޽�����, �ƴ� ��� ���� �޽����� �Բ�
	// ���Ͽ� ���� ����(ũ��)�� �����ش�.

	BYTE	MsgBuf[4096] = {0,};
	DWORD	dwMsgSize = 6;
	DWORD	dwNumOfByteSent = 0;
	DWORD	dwTotalByteSent = 0;
	WSABUF	WsaBuf;

	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = dwMsgSize;

	MsgBuf[0] = DFOM_CC_RESPONSE;
	if(bRet)
	{
		MsgBuf[1] = 1;
		m_XferStatus.dwFileSize = GetFileSize(m_hFile, NULL);
		*(DWORD*)(MsgBuf + 2) = m_XferStatus.dwFileSize;
		PostMsgToStatusDlg();
	}
	else
	{
		MsgBuf[1] = 0;
		*(DWORD*)(MsgBuf + 2) = 0;
	}

	// ���� �޽��� �۽�!!
	do
	{
		int nRet = WSASend(m_Sock, &WsaBuf, 1, &dwNumOfByteSent, 0, 0, 0);
		if(nRet == SOCKET_ERROR)
		{
			DWORD dwErr = WSAGetLastError();
			if(dwErr != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteSent;
		WsaBuf.len -= dwNumOfByteSent;

		dwTotalByteSent += dwNumOfByteSent;
	} while(dwTotalByteSent < dwMsgSize);

//#ifdef _DEBUG
//	LOGOUT(TEXT("SENDER: File Request Response Sent"));
//#endif

	// ���� ���� ������ ���� �׳� ������!!
	if(bRet == 0)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}

	return TRUE;
}

BOOL DFFileSender::SendFile()
{
	// ���κ��� ���� �޽����� �ް�, ���� ��� ���.
	// ���� �Ϸ� �޽��� ������ ����~
	BYTE	MsgBuf[4096+2] = {0,};
	DWORD	dwMsgSize = 1 + sizeof(TCHAR)*MAX_PATH;
	DWORD	dwNumOfByteXfered = 0;
	DWORD	dwTotalByteXfered = 0;
	DWORD	dwFlag = 0;
	DWORD	dwTotalFileRead = 0;
	DWORD	dwFileRead = 0;
	int		nRet = 0;
	
	WSABUF	WsaBuf;
	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = dwMsgSize;

	do
	{
		int nRet = WSARecv(m_Sock, &WsaBuf, 1, &dwNumOfByteXfered, &dwFlag, NULL, NULL);
		if(nRet == SOCKET_ERROR)
		{
			DWORD dwErr = WSAGetLastError();
			if(dwErr != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteXfered;
		WsaBuf.len -= dwNumOfByteXfered;

		dwTotalByteXfered += dwNumOfByteXfered;
	} while(dwTotalByteXfered < dwMsgSize);

//#ifdef _DEBUG
//	LOGOUT(TEXT("SENDER: Received Xfer Start Msg"));
//#endif

	// �޽��� ����
	if(MsgBuf[0] != DFOM_CC_XFER_START)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}

	// �� ���� �۽��Ѵ�!!!!
	DWORD dwRemainSize = m_XferStatus.dwFileSize;
	do
	{
		dwTotalByteXfered = 0;
		dwTotalFileRead = 0;

		BYTE* pLoc = MsgBuf;
		DWORD dwReadSize = (dwRemainSize >= 4096) ? 4096 : dwRemainSize;

		do	// ���ۿ� 4096 ����Ʈ�� ä���(���� ���� ����)
		{		
			// ��.. ������ ������ ����!!
			if(!ReadFile(m_hFile, pLoc, dwReadSize, &dwFileRead, NULL))
			{
#ifdef _DEBUG
				DWORD dwErr2 = GetLastError();
//				LOGOUT(TEXT("SENDER: ReadFile Failed!!! : %d"), dwErr2);
#endif
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
			else
			{
				// ������ �� ���� ���!!
				if(dwFileRead == 0)
				{
#ifdef _DEBUG
//					LOGOUT(TEXT("SENDER: ReadFile fin!!"));
#endif
					break;
				}
				else
				{
					dwTotalFileRead += dwFileRead;
					pLoc += dwFileRead;
				}
			}
#ifdef _DEBUG
//			LOGOUT("SENDER: FILE READ ok!!");
#endif
		} while(dwTotalFileRead < dwReadSize);

		// ���� ���� �ʱ�ȭ!!
		WsaBuf.buf = (char*)MsgBuf;
		WsaBuf.len = dwReadSize;

		do
		{
			nRet = WSASend(m_Sock, &WsaBuf, 1, &dwNumOfByteXfered, NULL, NULL, NULL);
			if(nRet == SOCKET_ERROR)
			{
				DWORD dwErr = WSAGetLastError();
				if(dwErr != WSAEINPROGRESS)
				{
#ifdef _DEBUG
					CString strErr;
					strErr.Format(TEXT("ErrorCode : %d"), dwErr);
					MessageBox(NULL, strErr, "Err", MB_OK); 
#endif
					m_XferStatus.dwState = STATE_ERROR;
					PostMsgToStatusDlg();
					return FALSE;
				}
			}

			WsaBuf.buf += dwNumOfByteXfered;
			WsaBuf.len -= dwNumOfByteXfered;

			dwTotalByteXfered += dwNumOfByteXfered;

			dwRemainSize -= dwNumOfByteXfered;

			m_XferStatus.dwState = STATE_XFERING;
			m_XferStatus.dwXferedSize = m_XferStatus.dwFileSize - dwRemainSize;
			PostMsgToStatusDlg();

//#ifdef _DEBUG
//			if(dwRemainSize < 4096) LOGOUT(TEXT("SENDER : Sender EoF"));

//			LOGOUT("RemainSize : %d", dwRemainSize);
//#endif
		} while((dwTotalByteXfered < 4096) && dwRemainSize);

	}while(dwRemainSize);

	// ������ �ݴ´�.
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

	// �Ϸ� �޽��� ������ �Ѵ�.
	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = dwMsgSize;
	dwNumOfByteXfered = 0;

	do
	{
		int nRet = WSARecv(m_Sock, &WsaBuf, 1, &dwNumOfByteXfered, &dwFlag, NULL, NULL);
		if(nRet == SOCKET_ERROR)
		{
			DWORD dwErr3 = WSAGetLastError();
			if(dwErr3 != WSAEINPROGRESS)
			{
//#ifdef _DEBUG
//				LOGOUT(TEXT("SENDER: FIN MSG ERROR"));
//#endif
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteXfered;
		WsaBuf.len -= dwNumOfByteXfered;

		dwTotalByteXfered += dwNumOfByteXfered;
	} while(dwTotalByteXfered < dwMsgSize);

	// �޽��� ����
	if(MsgBuf[0] != DFOM_CC_XFER_FIN)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}
	
//#ifdef _DEBUG
//	LOGOUT(TEXT("SENDER: File Send Completed!!!!"));
//#endif
	m_XferStatus.dwState = STATE_FINISHED;
	PostMsgToStatusDlg();

	return TRUE;
}

void DFFileSender::PostMsgToStatusDlg()
{
	// status dlg�� msg post!!
	COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (NULL, TEXT("DFOpenMind Transfer Status")); 

	cd.cbData = sizeof(m_XferStatus);
	cd.dwData = 1;						// send 1. recv 0.
	cd.lpData = &m_XferStatus;
	PostMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cd);		
}

void DFFileSender::PostMsgToMainframe()
{
	HWND hWnd = ::FindWindow (NULL, TEXT("DFOpenMind")); 

	PostMessage(hWnd, WM_SEND_FIN, (WPARAM)this, NULL);
}

DWORD DFFileSender::Transfer()
{
	__try
	{
		if(!RecvFileInfo())	
			__leave;
		if(!SendResult())
			__leave;
		if(!SendFile())	
			__leave;

		__leave;
	}

	__finally
	{
		if(m_Sock != INVALID_SOCKET) CloseSocket();
		if(m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
		PostMsgToMainframe();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
DFFileRecver::DFFileRecver(DWORD dwIP, LPCTSTR tszFile, LPCTSTR tszUserID)
{
	m_Sock		= INVALID_SOCKET;
	m_hThread	= INVALID_HANDLE_VALUE;
	m_hFile		= INVALID_HANDLE_VALUE;

	// ������ �ּҸ� �����Ѵ�.
	m_Peer_sin.sin_family			= AF_INET;
	m_Peer_sin.sin_port				= htons(FILE_XFER_PORT);
	m_Peer_sin.sin_addr.S_un.S_addr = dwIP;

	// ���� ���� ����ü�� �ʱ�ȭ�Ѵ�.
	memset(&m_XferStatus, 0, sizeof(m_XferStatus));
	lstrcpy(m_XferStatus.tszUserID, tszUserID);
	lstrcpy(m_XferStatus.tszFileName, tszFile);
	m_XferStatus.dwXferedSize	= 0;
	m_XferStatus.dwFileSize		= (DWORD)-1;
	m_XferStatus.dwState		= STATE_CLOSED;

	// �ٿ���� ������ full path�� �ۼ��Ѵ�.
	wsprintf(m_FullPath, TEXT("%s\\%s"), g_Config.tszDownloadFolder, tszFile);

	// ���� ���� ���¸� �����ϵ��� ���� ���̾�α׿� �޽��� ������.
	PostMsgToStatusDlg();

	// ���� �����带 �����Ͽ� ���� �۾��� �����ϵ��� �Ѵ�.
	if(!CreateXferThread()) 
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
	}
}

DFFileRecver::~DFFileRecver()
{
	if(m_XferStatus.dwState == STATE_CONNECTED || m_XferStatus.dwState == STATE_XFERING)
	{
		Disconnect();
		WaitForSingleObject(m_hThread, INFINITE);
	}
}

BOOL DFFileRecver::CreateXferThread()
{
	DWORD	dwThreadId = 0;

	m_hThread = BEGINTHREADEX(NULL, 0, RecverThread, this, CREATE_SUSPENDED, &dwThreadId); 
	if(m_hThread)
	{
		// �켱 ������ ���� ���� �Ͽ� ��Ʈ��ũ �޽����� ������ ������ �Ѵ�.
		// ��¥�� ���� ������� ��κ� ���ŷ ���¿� �����Ƿ� ���ɿ� ������ ����.
		SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
		ResumeThread(m_hThread);
		return TRUE;
	}
	return FALSE;
}

void DFFileRecver::PostMsgToStatusDlg()
{
	// status dlg�� msg post!!
	COPYDATASTRUCT cd; 
    HWND hWnd = ::FindWindow (NULL, TEXT("DFOpenMind Transfer Status")); 

	cd.cbData = sizeof(m_XferStatus);
	cd.dwData = 0;
	cd.lpData = &m_XferStatus;
	PostMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
}

void DFFileRecver::PostMsgToMainframe()
{
	HWND hWnd = ::FindWindow (NULL, TEXT("DFOpenMind")); 

	PostMessage(hWnd, WM_RECV_FIN, (WPARAM)this, NULL);
}

BOOL DFFileRecver::CreateAndConnect()
{
	m_Sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(m_Sock == INVALID_SOCKET) return FALSE;
	
	// ������ ����!
	int nRet = WSAConnect(	m_Sock,	(struct sockaddr FAR *)&m_Peer_sin, 
							sizeof(struct sockaddr), NULL, NULL, NULL, NULL);
//#ifdef _DEBUG
//	if(nRet == SOCKET_ERROR)
//		LOGOUT(TEXT("RECVER: FileRecver Sock Creation Failed"));
//#endif

	return (nRet != SOCKET_ERROR);
}

void DFFileRecver::CloseSocket(BOOL bGraceful)
{
	if(!bGraceful)
	{
		LINGER LingerStruct;
		LingerStruct.l_onoff  = 1;
		LingerStruct.l_linger = 0;

		setsockopt(m_Sock, SOL_SOCKET, SO_LINGER,
					(char*)&LingerStruct, sizeof(LingerStruct));
	}
	closesocket(m_Sock);
	m_Sock = INVALID_SOCKET;
}

BOOL DFFileRecver::SendFileInfo()
{
	BYTE	MsgBuf[4096] = {0,};
	DWORD	dwMsgSize = 1 + sizeof(TCHAR)*MAX_PATH+13;
	DWORD	dwNumOfByteSent = 0;
	DWORD	dwTotalByteSent = 0;
	
	MsgBuf[0] = DFOM_CC_REQUEST;
	lstrcpy((LPTSTR)MsgBuf+1, m_XferStatus.tszFileName); 
	lstrcpy((LPTSTR)MsgBuf+1+MAX_PATH, (LPCTSTR)g_pMainFrame->m_strUserName);

	WSABUF	WsaBuf;
	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = dwMsgSize;

	do
	{
		int nRet = WSASend(m_Sock, &WsaBuf, 1, &dwNumOfByteSent, 0, NULL, NULL); 
		if(nRet == SOCKET_ERROR)
		{
			DWORD dwErr = WSAGetLastError();
			if(dwErr != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteSent;
		WsaBuf.len -= dwNumOfByteSent;

		dwTotalByteSent += dwNumOfByteSent;
	} while(dwTotalByteSent < dwMsgSize);

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER: File Info Request Sent"));
//#endif

	return TRUE;
}

BOOL DFFileRecver::RecvResult()
{
	BYTE	MsgBuf[4096] = {0,};
	DWORD	dwMsgSize = 6;
	DWORD	dwNumOfByteRead = 0;
	DWORD	dwTotalByteRead = 0;
	DWORD	dwFlag = 0;
	WSABUF	WsaBuf;

	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = 4096;

	do
	{
		int nRet = WSARecv(m_Sock, &WsaBuf, 1, &dwNumOfByteRead, &dwFlag, NULL, NULL);
		if(nRet == SOCKET_ERROR)
		{
			DWORD dwErr = WSAGetLastError();
			if(dwErr != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteRead;
		WsaBuf.len -= dwNumOfByteRead;

		dwTotalByteRead += dwNumOfByteRead;
	} while(dwTotalByteRead < dwMsgSize);

	// �޽��� ������ ������. ���� �м��ؼ� ���� �۾��� ������ �غ� �Ѵ�.
	if((MsgBuf[0] != DFOM_CC_RESPONSE) || (MsgBuf[1] != 1))
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER: File Info Response Received"));
//#endif

	m_XferStatus.dwFileSize = *(DWORD*)(MsgBuf+2);
	PostMsgToStatusDlg();

	return TRUE;
}

BOOL DFFileRecver::RecvFile()
{
	// ���� ������ �غ� ������.
	// ���� ������ ��û�ϰ�, �ٷ� ���Ž����ؼ� ���� �Ϸ�Ǹ� ������!!
	BYTE	MsgBuf[4098] = {0,};
	WSABUF	WsaBuf;
	DWORD	dwNumOfByteXfered = 0;
	DWORD	dwTotalByteXfered = 0;
	DWORD	dwWritten = 0;
	DWORD	dwTotalWritten = 0;
	DWORD	dwFlag = 0;
	int		nRet = 0;

	// �켱 ������ ���� ���� ����Ѵ�.
	m_hFile = 
		CreateFile(	m_FullPath, 
					GENERIC_WRITE, 
					0, 
					0, 
					CREATE_NEW, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL);

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER: Download File Opened"));
//#endif

	// ���� ���� ��û �޽��� �۽�!!
	MsgBuf[0] = DFOM_CC_XFER_START;
	lstrcpy((LPTSTR)MsgBuf+1, m_XferStatus.tszFileName);
	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = 1+MAX_PATH;

	do
	{
		nRet = WSASend(m_Sock, &WsaBuf, 1, &dwNumOfByteXfered, 0, NULL, NULL); 
		if(nRet == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteXfered;
		WsaBuf.len -= dwNumOfByteXfered;

		dwTotalByteXfered += dwNumOfByteXfered;
	} while(dwTotalByteXfered < 1+MAX_PATH);

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER: Send Start Msg Sent"));
//#endif

	// �������� ���� ������ �ȴ�.
	DWORD dwRemainSize = m_XferStatus.dwFileSize;
	do
	{
		// ���� ���� �ʱ�ȭ!!
		WsaBuf.buf = (char*)MsgBuf;
		WsaBuf.len = 4096;
		dwTotalByteXfered	= 0;
		dwTotalWritten		= 0;

		do
		{
			nRet = WSARecv(m_Sock, &WsaBuf, 1, &dwNumOfByteXfered, &dwFlag, NULL, NULL);
			if(nRet == SOCKET_ERROR)
			{
				DWORD dwErr = WSAGetLastError();
				if(dwErr != WSAEINPROGRESS)
				{
#ifdef _DEBUG
					CString strErr;
					strErr.Format(TEXT("ErrorCode : %d"), dwErr);
					MessageBox(NULL, strErr, "Err", MB_OK); 
#endif
					m_XferStatus.dwState = STATE_ERROR;
					PostMsgToStatusDlg();
					return FALSE;
				}
			}

			WsaBuf.buf += dwNumOfByteXfered;
			WsaBuf.len -= dwNumOfByteXfered;

			dwTotalByteXfered += dwNumOfByteXfered;

			dwRemainSize -= dwNumOfByteXfered;
//#ifdef _DEBUG
//			if(dwRemainSize < 4096)
//				LOGOUT(TEXT("RECVER : Recver EoF"));
//#endif
		} while((dwTotalByteXfered < 4096) && dwRemainSize);

		// ��.. ������ ���Ͽ��� �������!!
		BYTE* pLoc = MsgBuf;
		do
		{
			if(!WriteFile(m_hFile, pLoc, dwTotalByteXfered, &dwWritten, NULL))
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
			else
			{
				dwTotalWritten += dwWritten;
				pLoc += dwWritten;

				m_XferStatus.dwState = STATE_XFERING;
				m_XferStatus.dwXferedSize = m_XferStatus.dwFileSize - dwRemainSize;
				PostMsgToStatusDlg();
			}
		} while(dwTotalByteXfered > dwTotalWritten);

	}while(dwRemainSize);

	// ������ �ݴ´�.
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER : File Recv FINISHED Perfectly!!!!"));
//#endif

	// 1�ʰ� ó���� ���!
	Sleep(1000);

	// ���� �Ϸ������� �뺸�Ѵ�.
	MsgBuf[0] = DFOM_CC_XFER_FIN;
	lstrcpy((LPTSTR)MsgBuf+1, m_XferStatus.tszFileName);
	WsaBuf.buf = (char*)MsgBuf;
	WsaBuf.len = 1+MAX_PATH;
	dwTotalByteXfered = 0;

	do
	{
		nRet = WSASend(m_Sock, &WsaBuf, 1, &dwNumOfByteXfered, 0, NULL, NULL); 
		if(nRet == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEINPROGRESS)
			{
				m_XferStatus.dwState = STATE_ERROR;
				PostMsgToStatusDlg();
				return FALSE;
			}
		}

		WsaBuf.buf += dwNumOfByteXfered;
		WsaBuf.len -= dwNumOfByteXfered;

		dwTotalByteXfered += dwNumOfByteXfered;
	} while(dwTotalByteXfered < 1+MAX_PATH);

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER : fIN Msg Sent"));
//#endif

	m_XferStatus.dwState = STATE_FINISHED;
	PostMsgToStatusDlg();

	return TRUE;
}

// blocking mode�� �۵��Ѵ�!! ������ �� �ϳ����� �۾��̹Ƿ� blocking ��尡 ����!!
DWORD DFFileRecver::Transfer()
{
	__try
	{
		// ������ �����Ѵ�.
		if(!CreateAndConnect()) 
			__leave;
		if(!SendFileInfo())	
			__leave;
		if(!RecvResult())	
			__leave;
		if(!RecvFile())		
			__leave;

		__leave;
	}

	__finally
	{
		if(m_Sock != INVALID_SOCKET) CloseSocket();
		if(m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
		PostMsgToMainframe();
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// recver Thread Function
DWORD WINAPI RecverThread(LPVOID p)
{
	DFFileRecver*	pDFFileRecver = (DFFileRecver*) p;
	return pDFFileRecver->Transfer();	
}

DWORD WINAPI SenderThread(LPVOID p)
{
	DFFileSender* pDFFileSender = (DFFileSender*) p;
	return pDFFileSender->Transfer();
}

DWORD WINAPI ListenThread(LPVOID p)
{
	DFFileServer* pDFFileServer = (DFFileServer*) p;
	return pDFFileServer->Listen();
}