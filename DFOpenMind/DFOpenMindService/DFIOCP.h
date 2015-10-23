#ifndef __DFIOCP_H__
#define __DFIOCP_H__

#pragma once

#include "winsock2.h"
#include "DFCmn.h"


//th
//#define ULONG_PTR unsigned long

class DFIOCP
{
public:
	DFIOCP(int nMaxConcurrency = -1)
	{
		m_hIOCP = NULL;
		if(nMaxConcurrency != -1)
			CreateIOCP(nMaxConcurrency);
	}

	~DFIOCP()
	{
		if(m_hIOCP != NULL)
			DFVERIFY(CloseHandle(m_hIOCP));
	}

	BOOL CreateIOCP(int nMaxConcurrency = 0)
	{
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
		DFASSERT(m_hIOCP != NULL);
		return (m_hIOCP != NULL);
	}

	BOOL UpdateIOCP(HANDLE hDevice, LPVOID CompletionKey)
	{
		BOOL bOK = 
			(m_hIOCP == CreateIoCompletionPort(hDevice, m_hIOCP, (ULONG_PTR)CompletionKey, 0));
		DFASSERT(bOK);
		return bOK;
	}

	BOOL UpdateIOCP(SOCKET Sock, LPVOID CompletionKey)
	{
		BOOL bOK = 
			(m_hIOCP == CreateIoCompletionPort((HANDLE)Sock, m_hIOCP, (ULONG_PTR)CompletionKey, 0));
		DFASSERT(bOK);
		return bOK;
	}
	
	BOOL PostStatus(LPVOID CompletionKey, DWORD dwNumBytes = 0, OVERLAPPED* pOverlapped = NULL)
	{
		BOOL bOK = 
			PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, (ULONG_PTR)CompletionKey, pOverlapped);
		DFASSERT(bOK);
		return bOK;
	}
	
	BOOL PostStatus(LPVOID CompletionKey, LPVOID dwNumBytes = NULL, OVERLAPPED* pOverlapped = NULL)
	{
		BOOL bOK = 
			PostQueuedCompletionStatus(m_hIOCP, (DWORD)dwNumBytes, (ULONG_PTR)CompletionKey, pOverlapped);
		DFASSERT(bOK);
		return bOK;
	}

	BOOL GetStatus(LPVOID* pCompletionKey, PDWORD pdwNumBytes, OVERLAPPED** ppOverlapped, DWORD dwMilliseconds = INFINITE)
	{
		return (GetQueuedCompletionStatus(	m_hIOCP, pdwNumBytes, (ULONG_PTR*)pCompletionKey, 
											ppOverlapped, dwMilliseconds));
	}

	BOOL GetStatus(LPVOID* pCompletionKey, LPVOID* pdwNumBytes, OVERLAPPED** ppOverlapped, DWORD dwMilliseconds = INFINITE)
	{
		return (GetQueuedCompletionStatus(	m_hIOCP, (PDWORD)pdwNumBytes, (ULONG_PTR*)pCompletionKey, 
											ppOverlapped, dwMilliseconds));
	}

private:
	HANDLE			m_hIOCP;
};

#endif