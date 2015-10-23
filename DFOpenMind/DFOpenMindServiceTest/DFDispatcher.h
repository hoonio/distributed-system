#include "DFIOCP.h"
#include "DFServerSock.h"


#pragma once

#ifndef __DFDISPATCHER_H__
#define __DFDISPATCHER_H__

#define MAX_DISPATCHERS		128+1
#define DISPATCHER_RATIO		2

// Inform ServerSock Events to Dispatchers.
#ifndef INFORM_CODE
// notification code.
#define INFORM_ACCEPT			1		// Connection Established.
#define INFORM_RECV				2		// Some Message Received.
#define INFORM_CLOSE			3		// Connection Closed by Client
#define INFORM_KILLED			4		// Connection Closed by Server Forcibly.
#define INFORM_TERMINATE		5		// Service Is Terminated.
#endif

typedef void (__cdecl *PRCSSFUNC)(void* Key, DWORD EventType);


class DFMsgProcessor
{
public:
	DFMsgProcessor(DFIOCP* pIOCP, PRCSSFUNC pfnProcess);
	~DFMsgProcessor();

	// Dispatcher work function.
	DWORD				DispatchAndExecute();	// �Լ� �����͸� �̿�, ���� �۾��� �ǽ��Ѵ�.
private:
	BOOL				CreateDispatchersInitially();
	void				GetProperDispatcherCount();
	HANDLE				CreateDispatcher();

	PRCSSFUNC			m_ProcessFunc;			// ���� ó���۾��� �Լ� ������.
	DFIOCP*				m_pIOCP;
	DWORD				m_dwDispatchers;		// ������ ������ ��.
	HANDLE				m_hDispatchers[MAX_DISPATCHERS];
};




#endif