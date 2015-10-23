#include "stdafx.h"
#include "DFServerSock.h"

DWORD WINAPI DFSSWorkerThread(LPVOID p);
DWORD WINAPI DFSSManagerThread(LPVOID p);


DFServerSock::DFServerSock(DFIOCP* pEventIOCP, UINT nMaxMsgSize)
{
	// create events.
	m_hStop		= CreateEvent(NULL, FALSE, FALSE, NULL);
	DFASSERT(m_hStop != NULL);

	// create critical sections.
	::InitializeCriticalSection(&m_ContextMapCS);

	// get proper threads count.
	GetProperThreadsCount();

	// initialize thread handles to NULL.
	for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
		m_hWorkerThread[i] = NULL;
	m_hManagerThread = NULL;

	// initialize other member variables.
	m_usPort		= 0;
	m_ListenSock	= INVALID_SOCKET;
	m_bRunning		= FALSE;
	m_bSetBufferSize= FALSE;
	m_nMaxMsgSize	= nMaxMsgSize;

	m_pSockIOCP		= NULL;
	m_pEventIOCP	= pEventIOCP;
}

DFServerSock::~DFServerSock()
{
	if(IsRunning())
	{
		StopService();
	}

	// Close All clients and remove all context in map.
	RemoveAllContext();

	// delete critical sections.
	::DeleteCriticalSection(&m_ContextMapCS);

	DFVERIFY(CloseHandle(m_hStop));

	WSACleanup();
}

BOOL DFServerSock::Initialize(unsigned short usPort, BOOL bSetBuffer, UINT nSendSize, UINT nRecvSize)
{
	WSADATA		WsaData;

	// socket dll initialize.
	int nRet = WSAStartup(WS_VERS, &WsaData);
	if(!nRet)
	{
		// socket port register. 
		m_usPort		= usPort;
		m_bSetBufferSize= bSetBuffer;
		m_nSendBufSize	= nSendSize;
		m_nRecvBufSize	= nRecvSize;

		return TRUE;
	}

	return FALSE;
}

BOOL DFServerSock::StartService()
{
	if(IsRunning())
		return FALSE;

	// create manager thread.
	if(m_hManagerThread == NULL)
	{
		m_pSockIOCP = new DFIOCP;
		DFASSERT(m_pSockIOCP != NULL);
		if(!m_pSockIOCP->CreateIOCP())
			return FALSE;

		if(!CreateManagerThread())
			return FALSE;
	}
	return TRUE;
}

void DFServerSock::StopService()
{
	if(IsRunning())		// if aleady running.
	{	
		// set stop event and close listen socket.
		DFVERIFY(SetEvent(m_hStop));
		SOCKET SockTemp = m_ListenSock;
		m_ListenSock = INVALID_SOCKET;
		if(SockTemp != INVALID_SOCKET)
		{
#ifdef _DEBUG
			int iRet = closesocket(SockTemp);
			DFASSERT(iRet == 0);
#else 
			closesocket(SockTemp);
#endif
		}
		// wait for manager termination.
		WaitForSingleObject(m_hManagerThread, INFINITE);

		DFVERIFY(CloseHandle(m_hManagerThread));
		m_hManagerThread = NULL;
		DFVERIFY(ResetEvent(m_hStop));

		m_pEventIOCP->PostStatus(0, INFORM_TERMINATE);
	}
}

inline BOOL DFServerSock::IsRunning()
{
	return (BOOL)InterlockedExchangeAdd ((LONG*)&m_bRunning, 0);
}

inline void DFServerSock::SetRunningFlag(BOOL bFlag)
{
	InterlockedExchange((LPLONG)&m_bRunning, (LONG)bFlag);
}

// set socket buffer size.
BOOL DFServerSock::SetBufSize(SOCKET Sock, UINT nSendSize, UINT nRecvSize)
{
	int nRet1 = setsockopt(	Sock, SOL_SOCKET, SO_SNDBUF, (char*) &nSendSize, sizeof(nSendSize));
	int nRet2 = setsockopt(	Sock, SOL_SOCKET, SO_RCVBUF, (char*) &nRecvSize, sizeof(nRecvSize));

	if((nRet1 == SOCKET_ERROR) || (nRet2 == SOCKET_ERROR))
		return FALSE;
	else
		return TRUE;
}

void DFServerSock::GetProperThreadsCount()
{
	SYSTEM_INFO		SystemInfo;
	DWORD			ProperCount = 0;

	// proper threads number is cpu number * 2 + 1.
	GetSystemInfo(&SystemInfo);
	ProperCount		= SystemInfo.dwNumberOfProcessors * 2 + 1;
	
	if(ProperCount > MAX_WORKER_THREADS)
		ProperCount = MAX_WORKER_THREADS;

	m_dwWorkerThreadCount = ProperCount;
}

BOOL DFServerSock::CreateListenSock()
{
	SOCKADDR_IN	si_addr;
	int			nRet;
	int			nZero = 0;
	LINGER		LingerStruct;

	// create listen socket.
	m_ListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(m_ListenSock == INVALID_SOCKET)
	{
		return FALSE;
	}

	// bind listen socket with si_addr struct.
	si_addr.sin_family		= AF_INET;
	si_addr.sin_port		= htons(m_usPort);
	si_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	nRet = bind(m_ListenSock, (struct sockaddr *) &si_addr, sizeof(si_addr));
	if(nRet == SOCKET_ERROR)
	{	
		return FALSE;
	}

	// start listening..
	nRet = listen(m_ListenSock, 5);
	if(nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	// disable send buffering on listen socket.
	nZero = 0;
	nRet = setsockopt(m_ListenSock, SOL_SOCKET, SO_SNDBUF, (char*) &nZero, sizeof(nZero));
	if(nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	// disable recv buffering on listen socket.
	nZero = 0;
	nRet = setsockopt(m_ListenSock, SOL_SOCKET, SO_RCVBUF, (char*) &nZero, sizeof(nZero));
	if(nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	// set linger on listen socket(for hard closing).
	nZero = 0;
	LingerStruct.l_onoff  = 1;
	LingerStruct.l_linger = 0;

	nRet = setsockopt(	m_ListenSock, SOL_SOCKET, SO_LINGER, 
						(char*) &LingerStruct, sizeof(LingerStruct));
	if(nRet == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL DFServerSock::CreateManagerThread()
{
	DWORD	dwThreadId = 0;

	// create manager thread. 
	m_hManagerThread = BEGINTHREADEX(NULL, 0, DFSSManagerThread, this, CREATE_SUSPENDED, &dwThreadId);
	if(m_hManagerThread == NULL)
	{	
		return FALSE;
	}

	SetThreadPriority(m_hManagerThread, THREAD_PRIORITY_BELOW_NORMAL);
	ResumeThread(m_hManagerThread);

	return TRUE;
}

BOOL DFServerSock::CreateWorkerThreads()
{
	DWORD	dwThreadId;

	// create worker thread.
	for(DWORD dwCount = 0; dwCount < m_dwWorkerThreadCount; dwCount++)
	{
		m_hWorkerThread[dwCount] = 
			BEGINTHREADEX(NULL, 0, DFSSWorkerThread, this, CREATE_SUSPENDED, &dwThreadId);
		if(m_hWorkerThread[dwCount] == NULL)
		{
			return FALSE;
		}

		SetThreadPriority(m_hWorkerThread[dwCount], THREAD_PRIORITY_BELOW_NORMAL);
		ResumeThread(m_hWorkerThread[dwCount]);
	}

	return TRUE;
}

void DFServerSock::ForceThreadKill(HANDLE hThread)
{
	DWORD dwExitCode;

	DFVERIFY(GetExitCodeThread(hThread, &dwExitCode));
	if(dwExitCode == STILL_ACTIVE)
		TerminateThread(hThread , 0);
}


PPER_SOCKET_CONTEXT DFServerSock::CreateSocketContext(SOCKET Sock)
{
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL;
	PPER_IO_CONTEXT		lpRecvIOContext = CreateIOContext();
	PPER_IO_CONTEXT		lpSendIOContext = CreateIOContext();

	lpPerSocketContext = new PER_SOCKET_CONTEXT;
	DFASSERT(lpPerSocketContext != NULL && !IsBadReadPtr(lpPerSocketContext, sizeof(PER_SOCKET_CONTEXT)));

	lpPerSocketContext->Socket			= Sock;
	lpPerSocketContext->iUseCount		= 1;
	lpPerSocketContext->bConnected		= 1;
	lpPerSocketContext->pRecvIOContext  = lpRecvIOContext;
	lpPerSocketContext->pSendIOContext  = lpSendIOContext;

	return lpPerSocketContext;
}

PPER_IO_CONTEXT DFServerSock::CreateIOContext()
{
	PPER_IO_CONTEXT lpPerIoContext = NULL;

	BYTE*		pBuffer = new BYTE[m_nMaxMsgSize+2];
	DFASSERT(pBuffer != NULL && !IsBadReadPtr(pBuffer, m_nMaxMsgSize+2));

	DFDRQueue*	pQueue = new DFDRQueue(m_nMaxMsgSize);
	DFASSERT(pQueue != NULL && !IsBadReadPtr(pQueue, sizeof(DFDRQueue)));

	lpPerIoContext = new PER_IO_CONTEXT;
	DFASSERT(lpPerIoContext != NULL && !IsBadReadPtr(lpPerIoContext, sizeof(PER_IO_CONTEXT)));

	lpPerIoContext->Overlapped.Internal		= 0;
	lpPerIoContext->Overlapped.InternalHigh = 0;
	lpPerIoContext->Overlapped.Offset		= 0;
	lpPerIoContext->Overlapped.OffsetHigh	= 0;
	lpPerIoContext->Overlapped.hEvent		= NULL;
	lpPerIoContext->nTotalBytes				= 0;
	lpPerIoContext->nTransferred			= 0;
	lpPerIoContext->pBuf					= pBuffer;
	lpPerIoContext->WsaBuf.buf				= (char*) pBuffer;
	lpPerIoContext->WsaBuf.len				= m_nMaxMsgSize+2;
	lpPerIoContext->pMsgQueue				= pQueue;
	lpPerIoContext->bReady					= TRUE;

	return lpPerIoContext;
}

void DFServerSock::DeleteSocketContext(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	DFASSERT(lpPerSocketContext && !IsBadWritePtr(lpPerSocketContext, sizeof(PER_SOCKET_CONTEXT)));
	DFASSERT(lpPerSocketContext->pRecvIOContext != NULL);
	DFASSERT(lpPerSocketContext->pSendIOContext != NULL);

	DeleteIOContext(lpPerSocketContext->pRecvIOContext);
	DeleteIOContext(lpPerSocketContext->pSendIOContext);

	delete lpPerSocketContext;
}

void DFServerSock::DeleteIOContext(PPER_IO_CONTEXT lpIOContext)
{
	DFASSERT(!IsBadWritePtr(lpIOContext, sizeof(PER_IO_CONTEXT)));
	DFASSERT(!IsBadWritePtr(lpIOContext->pBuf, m_nMaxMsgSize+2));
	DFASSERT(!IsBadWritePtr(lpIOContext->pMsgQueue, sizeof(DFDRQueue)));

	delete [] lpIOContext->pBuf;
	delete lpIOContext->pMsgQueue;
	delete lpIOContext;
}

//-------------------------------------------------------------------------------
BOOL DFServerSock::PostInitialRecv(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	int					nRet = 0;
	DWORD				dwFlag = 0;
	DWORD				dwRecvNumBytes = 0;
	PPER_IO_CONTEXT		lpIOContext = lpPerSocketContext->pRecvIOContext;
	
	if((BOOL)InterlockedExchangeAdd ((LONG*)&lpPerSocketContext->bConnected, 0))
	{
		nRet = WSARecv(	lpPerSocketContext->Socket,
						&(lpIOContext->WsaBuf),
						1,
						&dwRecvNumBytes,
						&dwFlag,
						&(lpIOContext->Overlapped),
						NULL);

		if((nRet == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			CloseClient(lpPerSocketContext, FALSE);
			return FALSE;
		}
		InterlockedIncrement((LONG*)&lpPerSocketContext->iUseCount);
		return TRUE;
	}
	return FALSE;
}

BOOL DFServerSock::PostNextSendDirect(PPER_SOCKET_CONTEXT lpPerSocketContext, BYTE* pBuf, UINT nSize)
{
	int				nRet = 0;
	DWORD			dwFlag = 0;
	DWORD			nTransferred = 0;
	PPER_IO_CONTEXT lpIOContext = lpPerSocketContext->pSendIOContext;

	if(lpPerSocketContext->bConnected)
	{
		memcpy(lpIOContext->pBuf + 2, pBuf, nSize);

		*(lpIOContext->pBuf)			= HIBYTE(nSize);
		*(lpIOContext->pBuf + 1)		= LOBYTE(nSize);
		lpIOContext->WsaBuf.buf			= (char*)lpIOContext->pBuf;
		lpIOContext->WsaBuf.len			= nSize + 2;
		lpIOContext->nTotalBytes		= nSize + 2;
		lpIOContext->nTransferred		= 0;

		nRet = WSASend(lpPerSocketContext->Socket,
						&(lpIOContext->WsaBuf),
						1,
						&nTransferred,
						dwFlag,
						&(lpIOContext->Overlapped),
						NULL);
		if((nRet == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			CloseClient(lpPerSocketContext, FALSE);
			return FALSE;
		}
		InterlockedIncrement((LONG*)&lpPerSocketContext->iUseCount);
		return TRUE;
	}
	return FALSE;
}

BOOL DFServerSock::CheckAndPostNextSend(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	int				nRet = 0;
	DWORD			dwFlag = 0;
	DWORD			nTransferred = 0;
	UINT			nSize = 0;
	PPER_IO_CONTEXT lpIOContext = lpPerSocketContext->pSendIOContext;

	if(lpPerSocketContext->bConnected)
	{
		// 현재 송신 중인 내용이 없을 경우,
		if(InterlockedCompareExchange((LPLONG)&lpIOContext->bReady, FALSE, TRUE) == TRUE)
		{
			if(lpIOContext->pMsgQueue->GetFromQueue(lpIOContext->pBuf + 2, &nSize))
			{			
				*(lpIOContext->pBuf)			= LOBYTE(nSize);
				*(lpIOContext->pBuf + 1)		= HIBYTE(nSize);
				lpIOContext->WsaBuf.buf			= (char*)lpIOContext->pBuf;
				lpIOContext->WsaBuf.len			= nSize + 2;
				lpIOContext->nTotalBytes		= nSize + 2;
				lpIOContext->nTransferred		= 0;

				nRet = WSASend(lpPerSocketContext->Socket,
								&(lpIOContext->WsaBuf),
								1,
								&nTransferred,
								dwFlag,
								&(lpIOContext->Overlapped),
								NULL);
				if((nRet == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
				{
					CloseClient(lpPerSocketContext, FALSE);
					return FALSE;
				}
				InterlockedIncrement((LONG*)&lpPerSocketContext->iUseCount);
				return TRUE;
			}
			else
				InterlockedExchange((LPLONG)&lpIOContext->bReady, TRUE);
		}		
	}
	return FALSE;
}

void DFServerSock::ProcessRecv(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	PPER_IO_CONTEXT	lpIOContext = lpPerSocketContext->pRecvIOContext;

	if((BOOL)InterlockedExchangeAdd ((LONG*)&lpPerSocketContext->bConnected, 0))
	{
		if(lpIOContext)
		{
			if(lpIOContext->nTransferred >= 2)		// 메시지 길이를 알 수 있다~!
			{
				lpIOContext->nTotalBytes = MAKEWORD(*(lpIOContext->pBuf), *(lpIOContext->pBuf + 1)) + 2;
				
				if(lpIOContext->nTotalBytes == lpIOContext->nTransferred)		// 뒷메시지가 없고 딱 맞는 경우..
				{
					lpIOContext->pMsgQueue->AddToQueue(lpIOContext->pBuf + 2, lpIOContext->nTotalBytes - 2);
					m_pEventIOCP->PostStatus(lpPerSocketContext, INFORM_RECV);

					lpIOContext->WsaBuf.buf		= (char*)lpIOContext->pBuf;
					lpIOContext->WsaBuf.len		= m_nMaxMsgSize+2;
					lpIOContext->nTransferred	= 0;
					lpIOContext->nTotalBytes	= 0;

					PostInitialRecv(lpPerSocketContext);
				}
				else if(lpIOContext->nTotalBytes < lpIOContext->nTransferred)	// 뒷 메시지가 존재하는 경우
				{
					lpIOContext->pMsgQueue->AddToQueue(lpIOContext->pBuf + 2, lpIOContext->nTotalBytes - 2);
					m_pEventIOCP->PostStatus(lpPerSocketContext, INFORM_RECV);

					lpIOContext->nTransferred -= lpIOContext->nTotalBytes;
					memmove(lpIOContext->pBuf, 
							lpIOContext->pBuf + lpIOContext->nTotalBytes,
							lpIOContext->nTransferred);

					lpIOContext->WsaBuf.buf		= (char*)lpIOContext->pBuf + lpIOContext->nTransferred;
					lpIOContext->WsaBuf.len		= m_nMaxMsgSize+2 - lpIOContext->nTransferred;
					lpIOContext->nTotalBytes	= 0;

					ProcessRecv(lpPerSocketContext);
				}
				else	// 길이는 알지만 수신이 안끝난 경우.
				{
					PostInitialRecv(lpPerSocketContext);
				}
			}
			else		// 아직 길이를 알지 못하는 경우.,
			{
				PostInitialRecv(lpPerSocketContext);
			}
		}
	}
}

void DFServerSock::ProcessSend(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	DWORD	nTransferred = 0;
	DWORD	dwFlag = 0;
	int		nRet = 0;

	PPER_IO_CONTEXT lpIOContext = lpPerSocketContext->pSendIOContext;

	if(lpPerSocketContext->bConnected)
	{
		if(lpIOContext->nTransferred == lpIOContext->nTotalBytes)	// 송신 완료된 경우..
		{
			InterlockedExchange((LPLONG)&lpIOContext->bReady, TRUE);
			CheckAndPostNextSend(lpPerSocketContext);
		}
		else	// 송신 완료되지 못한 상황이라면,.
		{
			nRet = WSASend( lpPerSocketContext->Socket,
							&(lpIOContext->WsaBuf),
							1,
							&nTransferred,
							dwFlag,
							&(lpIOContext->Overlapped),
							NULL);

			if((nRet == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
			{
				CloseClient(lpPerSocketContext, FALSE);
				return;
			}
			InterlockedIncrement((LONG*)&lpPerSocketContext->iUseCount);
		}
	}
	else
	{
		CloseClient(lpPerSocketContext, FALSE);
	}
}

BOOL DFServerSock::SendMsg(void* Key, BYTE* pBuf, UINT nSize)
{
	__try
	{
		PPER_SOCKET_CONTEXT lpPerSocketContext = FindSocketContext(Key);
		if(lpPerSocketContext)
		{
			if(nSize > m_nMaxMsgSize)
				return FALSE;

			if(lpPerSocketContext->pSendIOContext->pMsgQueue->AddToQueue(pBuf, nSize))
			{
				return CheckAndPostNextSend(lpPerSocketContext);
			}

		/*	// 바로 송신 가능하다면 큐에 쓰지 않고 바로 송신한다.
			if(InterlockedCompareExchange((LPLONG)lpPerSocketContext->pSendIOContext->bReady, FALSE, TRUE) == TRUE)
			{
				return PostNextSendDirect(lpPerSocketContext, pBuf, nSize);
			}
			else //if(lpPerSocketContext->pSendIOContext->pMsgQueue->AddToQueue(pBuf, nSize))
			{
				return lpPerSocketContext->pSendIOContext->pMsgQueue->AddToQueue(pBuf, nSize);
				//return CheckAndPostNextSend(lpPerSocketContext);
			}
		*/
		}
	}
	// 이미 제거된 소켓 컨텍스트에 접근하는 경우.. 무시한다.
	__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
	{
	//	return FALSE;
	}

	return FALSE;
}

BOOL DFServerSock::RecvMsg(void* Key, BYTE* pBuf, UINT* pSize)
{
	__try
	{
		PPER_SOCKET_CONTEXT lpPerSocketContext = FindSocketContext(Key);

		if(lpPerSocketContext)
		{
			if(lpPerSocketContext->pRecvIOContext->pMsgQueue->GetFromQueue(pBuf, pSize))
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	// 이미 제거된 소켓 컨텍스트에 접근하는 경우.. 무시한다.
	__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
	{
	//	return FALSE;
	}

	return FALSE;
}

void DFServerSock::AddContextToMap(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	EnterCriticalSection(&m_ContextMapCS);

	m_SocketContextMap[(void*)lpPerSocketContext] = lpPerSocketContext;

	LeaveCriticalSection(&m_ContextMapCS);
}


void DFServerSock::RemoveContextFromMap(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	DFASSERT(lpPerSocketContext != NULL);

	EnterCriticalSection(&m_ContextMapCS);
	m_SocketContextMap.RemoveKey((void*)lpPerSocketContext);
	LeaveCriticalSection(&m_ContextMapCS);
	
	// remove socket context.
	DeleteSocketContext(lpPerSocketContext);
}

void DFServerSock::RemoveAllContext()
{
	POSITION			pos = NULL;
	void*				Key = NULL;
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL;

	EnterCriticalSection(&m_ContextMapCS);

	pos = m_SocketContextMap.GetStartPosition();

	while(pos)
	{
		m_SocketContextMap.GetNextAssoc(pos, Key, (void*&)lpPerSocketContext);

		DFASSERT(lpPerSocketContext != NULL); 

		// Make app to notify Connection Closing.
		m_pEventIOCP->PostStatus(lpPerSocketContext, INFORM_CLOSE);

		// close socket.
		closesocket(lpPerSocketContext->Socket);

		// remove socket context.
		DeleteSocketContext(lpPerSocketContext);
	}

	m_SocketContextMap.RemoveAll();

	LeaveCriticalSection(&m_ContextMapCS);
}

PPER_SOCKET_CONTEXT DFServerSock::FindSocketContext(void* Key)
{
	PPER_SOCKET_CONTEXT	lpPerSocketContext = NULL;

	if(Key)
	{
		EnterCriticalSection(&m_ContextMapCS);

		m_SocketContextMap.Lookup((void*)Key, (void*&)lpPerSocketContext);

		LeaveCriticalSection(&m_ContextMapCS);
	}

	return lpPerSocketContext;
}

void DFServerSock::CloseClientForcibly(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	// 큐에 내용이 없을 때까지!!, 송신 중인 메시지가 없을 때까지.
//	if(	!lpPerSocketContext->pSendIOContext->pMsgQueue->IsEmpty() &&
//		!lpPerSocketContext->pSendIOContext->bReady) Sleep(10);

	LINGER LingerStruct;
	LingerStruct.l_onoff  = 1;
	LingerStruct.l_linger = 0;
	setsockopt(lpPerSocketContext->Socket, SOL_SOCKET, SO_LINGER,
				(char*)&LingerStruct, sizeof(LingerStruct));			

	closesocket(lpPerSocketContext->Socket);

	InterlockedExchange((LONG*)&lpPerSocketContext->bConnected, 0);

//	RemoveContextFromMap(lpPerSocketContext);

	// Make app to notify Connection Closing.
	//m_pEventIOCP->PostStatus(lpPerSocketContext, INFORM_KILLED);
}

void DFServerSock::CloseClient(PPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful)
{
	DFASSERT(lpPerSocketContext && !IsBadReadPtr(lpPerSocketContext, sizeof(PER_SOCKET_CONTEXT)));

	// indicate that this Socket is closed!!
	// in later, no IO will be pending by this "bConnected" flag. 
	InterlockedExchange((LONG*)&lpPerSocketContext->bConnected, 0);

	if(InterlockedCompareExchange((LONG*)&lpPerSocketContext->iUseCount, 0, 1) == 1)
	{
		if(!bGraceful)
		{
			LINGER LingerStruct;
			LingerStruct.l_onoff  = 1;
			LingerStruct.l_linger = 0;
			setsockopt(lpPerSocketContext->Socket, SOL_SOCKET, SO_LINGER,
						(char*)&LingerStruct, sizeof(LingerStruct));			
		}

		closesocket(lpPerSocketContext->Socket);

		RemoveContextFromMap(lpPerSocketContext);

		// Make app to notify Connection Closing.
		m_pEventIOCP->PostStatus(lpPerSocketContext, INFORM_CLOSE);
	}
}

DWORD DFServerSock::GetClientIP(PPER_SOCKET_CONTEXT lpPerSocketContext)
{
	SOCKADDR_IN ChildSock_sin;
	int Size = sizeof(struct sockaddr);

	int nRet = getpeername(lpPerSocketContext->Socket, (struct sockaddr FAR *)&ChildSock_sin, &Size);
	
	return (nRet == 0) ? ChildSock_sin.sin_addr.S_un.S_addr : 0;
}

void DFServerSock::GetClientIP(PPER_SOCKET_CONTEXT lpPerSocketContext, TCHAR* szClientIP)
{
	SOCKADDR_IN ChildSock_sin;
	int Size = sizeof(struct sockaddr);

	int nRet = getpeername(lpPerSocketContext->Socket, (struct sockaddr FAR *)&ChildSock_sin, &Size);
	
	(nRet == 0) ? 
		wsprintf(szClientIP, TEXT("%s"), inet_ntoa(ChildSock_sin.sin_addr)) : szClientIP[0] = NULL;

}


//////////////////////////////////////////////////////////////////////////////////////////////
// Functions that are called by Thread Function.
DWORD DFServerSock::Manager()
{
	SOCKET				Sock = INVALID_SOCKET;
	PPER_SOCKET_CONTEXT lpPerSocketContext = NULL;

	__try
	{
		if(!CreateWorkerThreads())
			__leave;
		if(!CreateListenSock())
			__leave;

		SetRunningFlag(TRUE); 

		while(WaitForSingleObject(m_hStop, 0) != WAIT_OBJECT_0)
		{
			Sock = WSAAccept(m_ListenSock, NULL, NULL, NULL, 0);
			if(Sock == INVALID_SOCKET)
			{		
				__leave;
			}
			else if(m_bSetBufferSize)	// if socket BUFFER setting defined.
			{
				DFVERIFY(SetBufSize(Sock, m_nSendBufSize, m_nRecvBufSize));
			}
			
			lpPerSocketContext = CreateSocketContext(Sock);
			DFASSERT(lpPerSocketContext != NULL);

			AddContextToMap(lpPerSocketContext);
			DFVERIFY(m_pSockIOCP->UpdateIOCP(Sock, (void*)lpPerSocketContext));

			m_pEventIOCP->PostStatus(lpPerSocketContext, INFORM_ACCEPT);
			PostInitialRecv(lpPerSocketContext);
		}
		
		__leave;
	}

	__finally	
	{
		SetRunningFlag(FALSE);

		// Kill all worker threads.
		if(m_pSockIOCP != NULL)
		{
			for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
			{
				// this will end worker threads
				m_pSockIOCP->PostStatus(NULL, (DWORD)0);
			}
		}

		// wait for worker terminations.
		WaitForMultipleObjects(m_dwWorkerThreadCount, m_hWorkerThread, TRUE, INFINITE);
		for(DWORD k = 0; k < m_dwWorkerThreadCount; k++)
		{
			DFVERIFY(CloseHandle(m_hWorkerThread[k]));
			m_hWorkerThread[k] = NULL;
		}	

		// remove all socket contexts in map.
		RemoveAllContext();

		// release IOCP.
		if(m_pSockIOCP)
		{
			delete m_pSockIOCP;
			m_pSockIOCP = NULL;
		}

		// closing listen socket.
		if(m_ListenSock != INVALID_SOCKET)
		{
#ifdef _DEBUG
			int iRet = closesocket(m_ListenSock);
			DFASSERT(iRet ==0);
#else
			closesocket(m_ListenSock);
#endif
			m_ListenSock = INVALID_SOCKET;
		}	

		return 0;
	}
}

DWORD DFServerSock::Worker()
{
	BOOL				bSuccess = FALSE;
	LPOVERLAPPED		lpOverlapped = NULL;
	PPER_SOCKET_CONTEXT	lpPerSocketContext = NULL;
	DWORD				dwIoSize = 0;

	for(;;)
	{
		bSuccess = m_pSockIOCP->GetStatus(	(void**)&lpPerSocketContext,	// Key
											&dwIoSize,						// IO Size 
											&lpOverlapped,					// Overlapped Structure **
											INFINITE);						// Wait Time

		// lpPerSocketContext is NULL means return - End Thread.
		if(lpPerSocketContext == NULL)	
		{
			return 0;
		}

		// error occur in this socket context. maybe connection closed.
		if(!bSuccess || (bSuccess && (dwIoSize == 0)))
		{
			InterlockedDecrement((LONG*)&lpPerSocketContext->iUseCount);

			CloseClient(lpPerSocketContext, FALSE);
		}	
		else	// Works!!!!!
		{
			InterlockedDecrement((LONG*)&lpPerSocketContext->iUseCount);

			// process recv completion..
			if(lpOverlapped == (LPOVERLAPPED)lpPerSocketContext->pRecvIOContext)
			{
				lpPerSocketContext->pRecvIOContext->nTransferred += dwIoSize;
				lpPerSocketContext->pRecvIOContext->WsaBuf.buf += dwIoSize;
				lpPerSocketContext->pRecvIOContext->WsaBuf.len -= dwIoSize;

				ProcessRecv(lpPerSocketContext);
			}
			// process send completion..
			else if(lpOverlapped == (LPOVERLAPPED)lpPerSocketContext->pSendIOContext)
			{
				lpPerSocketContext->pSendIOContext->nTransferred += dwIoSize;
				lpPerSocketContext->pSendIOContext->WsaBuf.buf += dwIoSize;
				lpPerSocketContext->pSendIOContext->WsaBuf.len -= dwIoSize;

				ProcessSend(lpPerSocketContext);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// Thread Functions.
DWORD WINAPI DFSSWorkerThread(LPVOID p)
{
	DFServerSock* pServerSock = (DFServerSock*) p;
	return pServerSock->Worker();
}

DWORD WINAPI DFSSManagerThread(LPVOID p)
{
	DFServerSock* pServerSock = (DFServerSock*) p;
	return pServerSock->Manager();
}