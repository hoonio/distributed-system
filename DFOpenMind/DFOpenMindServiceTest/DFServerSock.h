#ifndef __DFSERVERSOCK_H__
#define __DFSERVERSOCK_H__

#pragma once

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#define FD_SETSIZE	40960	// definition of maximum sockets to 4096.
#endif


#define MAX_WORKER_THREADS		128+1	// 4 * CPU + 1
#define WS_VERS					MAKEWORD(2,2)

// Inform ServerSock Events to Dispatchers.
#ifndef INFORM_CODE
// notification code.
#define INFORM_ACCEPT			1		// Connection Established.
#define INFORM_RECV				2		// Some Message Received.
#define INFORM_CLOSE			3		// Connection Closed by Client
#define INFORM_KILLED			4		// Connection Closed by Server Forcibly.
#define INFORM_TERMINATE		5		// Service Is Terminated.
#endif


#include "DFCmn.h"
#include "DFDRQueue.h"
#include "DFMapPtrToPtr.h"
#include "DFIOCP.h"
#include <winsock2.h>


// definition of context that is needed for socket
typedef struct _PER_IO_CONTEXT		
{
	WSAOVERLAPPED			Overlapped;
	BYTE*					pBuf;			// Send/Recv Buffer.
	WSABUF					WsaBuf;
	int						nTotalBytes;	
	int						nTransferred;
	DFDRQueue*				pMsgQueue;
	BOOL					bReady;			// 송신 가능함을 나타냄.
} PER_IO_CONTEXT, *PPER_IO_CONTEXT;

typedef struct _PER_SOCKET_CONTEXT	
{
	SOCKET					Socket;
	BOOL					bConnected;			// 연결이 종료되었음을 나타냄.
	int						iUseCount;			// 현재 펜딩된 IO의 수.
	PPER_IO_CONTEXT			pRecvIOContext;		// for the Recv Operation.
	PPER_IO_CONTEXT			pSendIOContext;		// for the Send Operation.
} PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;

////////////////////////////////////////////////////////////////////////////////////////
class DFServerSock
{
public:
	DFServerSock(DFIOCP* pEventIOCP, UINT nMaxMsgSize = BASE_OBJECT_SIZE);
	~DFServerSock();

	BOOL				Initialize( unsigned short usPort, BOOL bSetBuffer = TRUE, 
									UINT nSendSize = 0, UINT nRecvSize = 0);

	BOOL				StartService();
	void				StopService();

	// closing client connection... and more.
	void				CloseClient(PPER_SOCKET_CONTEXT lpPerSocketContext, BOOL bGraceful);
	void				CloseClientForcibly(PPER_SOCKET_CONTEXT lpPerSocketContext);
	PPER_SOCKET_CONTEXT FindSocketContext(void* Key);
	void				GetClientIP(PPER_SOCKET_CONTEXT lpPerSocketContext, TCHAR* szClientIP);
	DWORD				GetClientIP(PPER_SOCKET_CONTEXT lpPerSocketContext);
	DFIOCP*				GetEventIOCP(){return m_pEventIOCP;};
	BOOL				IsRunning();

	// send and recv functions.
	BOOL				SendMsg(void* Key, BYTE* pBuf, UINT nSize);
	BOOL				RecvMsg(void* Key, BYTE* pBuf, UINT* pSize);

	// functions that threads call.
	DWORD				Worker();
	DWORD				Manager();

private:
	// functions that related to creating/destroying.
	BOOL				CreateListenSock();
	BOOL				CreateWorkerThreads();
	BOOL				CreateManagerThread();
	void				GetProperThreadsCount();
	void				ForceThreadKill(HANDLE hThread);
	
	// create/destroy socket and IO context.
	PPER_IO_CONTEXT		CreateIOContext();
	PPER_SOCKET_CONTEXT	CreateSocketContext(SOCKET Sock);
	void				DeleteIOContext(PPER_IO_CONTEXT lpIOContext);
	void				DeleteSocketContext(PPER_SOCKET_CONTEXT lpPerSocketContext);

	// for the socket context map.
	void				AddContextToMap(PPER_SOCKET_CONTEXT lpPerSocketContext);
	void				RemoveContextFromMap(PPER_SOCKET_CONTEXT lpPerSocketContext);
	void				RemoveAllContext();

	// functions that related recv/send 
	BOOL				PostInitialRecv(PPER_SOCKET_CONTEXT lpPerSocketContext);
	BOOL				CheckAndPostNextSend(PPER_SOCKET_CONTEXT lpPerSocketContext);
	BOOL				PostNextSendDirect(PPER_SOCKET_CONTEXT lpPerSocketContext, BYTE* pBuf, UINT nSize);
	void				ProcessRecv(PPER_SOCKET_CONTEXT lpPerSocketContext);
	void				ProcessSend(PPER_SOCKET_CONTEXT lpPerSocketContext);

	// other methods...
	BOOL				SetBufSize(SOCKET Sock, UINT nSendSize, UINT nRecvSize);
	void				SetRunningFlag(BOOL bFlag);

	// private member variables.. 
	// Data and flag members
	SOCKET				m_ListenSock;
	unsigned short		m_usPort;
	UINT				m_nMaxMsgSize;
	
	DWORD				m_dwWorkerThreadCount;

	BOOL				m_bSetBufferSize;
	unsigned int		m_nSendBufSize;
	unsigned int		m_nRecvBufSize;
	BOOL				m_bRunning;

	// thread handles.
	HANDLE				m_hWorkerThread[MAX_WORKER_THREADS];
	HANDLE				m_hManagerThread;

	DFIOCP*				m_pSockIOCP;	// 소켓 처리를 위한 iocp
	DFIOCP*				m_pEventIOCP;	// Dispatcher에게 알려주기 위한 iocp.
	
	// Event handles.
	HANDLE				m_hStop;		// indicate that i wanna stop service;

	CRITICAL_SECTION	m_ContextMapCS;
	DFMapPtrToPtr		m_SocketContextMap;
};

#endif


