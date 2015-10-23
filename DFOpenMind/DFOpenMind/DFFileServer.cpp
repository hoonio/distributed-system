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
		// 우선 순위를 조금 높게 하여 네트워크 메시지의 지연이 없도록 한다.
		// 어짜피 소켓 쓰레드는 대부분 블로킹 상태에 빠지므로 성능에 영향이 없다.
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

	// 전송 상태 구조체를 초기화한다.
	memset(&m_XferStatus, 0, sizeof(m_XferStatus));
	m_XferStatus.dwXferedSize	= 0;
	m_XferStatus.dwFileSize		= (DWORD)-1;
	m_XferStatus.dwState		= STATE_CONNECTED;

	// 현재 수신 상태를 갱신하도록 상태 다이얼로그에 메시지 날린다.
	PostMsgToStatusDlg();

	// 수신 쓰레드를 생성하여 수신 작업을 시작하도록 한다.
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
		// 우선 순위를 조금 높게 하여 네트워크 메시지의 지연이 없도록 한다.
		// 어짜피 소켓 쓰레드는 대부분 블로킹 상태에 빠지므로 성능에 영향이 없다.
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

	// 메시지 검증!!
	if(MsgBuf[0] != DFOM_CC_REQUEST)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}
	
	lstrcpy(m_XferStatus.tszFileName, (LPTSTR)MsgBuf+1); 
	lstrcpy(m_XferStatus.tszUserID, (LPTSTR)MsgBuf+1+MAX_PATH);
	PostMsgToStatusDlg();

	// 업로드할 파일의 full path를 작성한다.
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

	// 파일을 열지 못했다면 송신할 수 없으므로 실패 메시지를, 아닌 경우 성공 메시지와 함께
	// 파일에 대한 정보(크기)를 보내준다.

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

	// 응답 메시지 송신!!
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

	// 파일 열기 실패한 경우는 그냥 죽이자!!
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
	// 상대로부터 개시 메시지를 받고, 이후 계속 쏜다.
	// 이후 완료 메시지 받으면 종료~
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

	// 메시지 검증
	if(MsgBuf[0] != DFOM_CC_XFER_START)
	{
		m_XferStatus.dwState = STATE_ERROR;
		PostMsgToStatusDlg();
		return FALSE;
	}

	// 자 이제 송신한다!!!!
	DWORD dwRemainSize = m_XferStatus.dwFileSize;
	do
	{
		dwTotalByteXfered = 0;
		dwTotalFileRead = 0;

		BYTE* pLoc = MsgBuf;
		DWORD dwReadSize = (dwRemainSize >= 4096) ? 4096 : dwRemainSize;

		do	// 버퍼에 4096 바이트를 채운다(끝인 경우는 제외)
		{		
			// 자.. 실제로 파일을 읽자!!
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
				// 파일을 다 읽은 경우!!
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

		// 수신 버퍼 초기화!!
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

	// 파일을 닫는다.
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

	// 완료 메시지 수신을 한다.
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

	// 메시지 검증
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
	// status dlg에 msg post!!
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

	// 서버의 주소를 정리한다.
	m_Peer_sin.sin_family			= AF_INET;
	m_Peer_sin.sin_port				= htons(FILE_XFER_PORT);
	m_Peer_sin.sin_addr.S_un.S_addr = dwIP;

	// 전송 상태 구조체를 초기화한다.
	memset(&m_XferStatus, 0, sizeof(m_XferStatus));
	lstrcpy(m_XferStatus.tszUserID, tszUserID);
	lstrcpy(m_XferStatus.tszFileName, tszFile);
	m_XferStatus.dwXferedSize	= 0;
	m_XferStatus.dwFileSize		= (DWORD)-1;
	m_XferStatus.dwState		= STATE_CLOSED;

	// 다운받은 파일의 full path를 작성한다.
	wsprintf(m_FullPath, TEXT("%s\\%s"), g_Config.tszDownloadFolder, tszFile);

	// 현재 수신 상태를 갱신하도록 상태 다이얼로그에 메시지 날린다.
	PostMsgToStatusDlg();

	// 수신 쓰레드를 생성하여 수신 작업을 시작하도록 한다.
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
		// 우선 순위를 조금 높게 하여 네트워크 메시지의 지연이 없도록 한다.
		// 어짜피 소켓 쓰레드는 대부분 블로킹 상태에 빠지므로 성능에 영향이 없다.
		SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
		ResumeThread(m_hThread);
		return TRUE;
	}
	return FALSE;
}

void DFFileRecver::PostMsgToStatusDlg()
{
	// status dlg에 msg post!!
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
	
	// 서버에 연결!
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

	// 메시지 수신이 끝났다. 이제 분석해서 다음 작업을 시작할 준비를 한다.
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
	// 파일 전송의 준비가 끝났다.
	// 전송 시작을 요청하고, 바로 수신시작해서 수신 완료되면 끝내자!!
	BYTE	MsgBuf[4098] = {0,};
	WSABUF	WsaBuf;
	DWORD	dwNumOfByteXfered = 0;
	DWORD	dwTotalByteXfered = 0;
	DWORD	dwWritten = 0;
	DWORD	dwTotalWritten = 0;
	DWORD	dwFlag = 0;
	int		nRet = 0;

	// 우선 파일을 열어 놓고 대기한다.
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

	// 전송 시작 요청 메시지 송신!!
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

	// 이제부터 파일 전송이 된다.
	DWORD dwRemainSize = m_XferStatus.dwFileSize;
	do
	{
		// 수신 버퍼 초기화!!
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

		// 자.. 실제로 파일에다 기록하자!!
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

	// 파일을 닫는다.
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

//#ifdef _DEBUG
//	LOGOUT(TEXT("RECVER : File Recv FINISHED Perfectly!!!!"));
//#endif

	// 1초간 처리를 대기!
	Sleep(1000);

	// 수신 완료했음을 통보한다.
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

// blocking mode로 작동한다!! 쓰레드 당 하나씩의 작업이므로 blocking 모드가 가능!!
DWORD DFFileRecver::Transfer()
{
	__try
	{
		// 소켓을 생성한다.
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