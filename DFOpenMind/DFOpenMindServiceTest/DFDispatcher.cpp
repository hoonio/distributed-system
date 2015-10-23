#include "stdafx.h"
#include "dfdispatcher.h"


DWORD WINAPI DFDispatcherThread(LPVOID p);


DFMsgProcessor::DFMsgProcessor(DFIOCP* pIOCP, PRCSSFUNC pfnProcess)
{
	DFASSERT(pIOCP != NULL);
	m_pIOCP			= pIOCP;
	m_ProcessFunc	= pfnProcess;

	GetProperDispatcherCount();
	DFVERIFY(CreateDispatchersInitially());
}

DFMsgProcessor::~DFMsgProcessor()
{
	for(DWORD i = 0; i < m_dwDispatchers; i++)
	{
		m_pIOCP->PostStatus(NULL, (DWORD)0);
		Sleep(0);
	}

	DWORD dwWait = WaitForMultipleObjects(m_dwDispatchers, m_hDispatchers, TRUE, 3000);
	if(dwWait == WAIT_TIMEOUT)
	{
		for(DWORD i = 0; i < m_dwDispatchers; i++)
		{
			DWORD dwExitCode;

			GetExitCodeThread(m_hDispatchers[i], &dwExitCode);
			if(dwExitCode == STILL_ACTIVE)
				TerminateThread(m_hDispatchers[i] , 0);
		}
	}
}

// get Min/Max Dispatcher counts by systeminfo.
void DFMsgProcessor::GetProperDispatcherCount()
{
	DFSystemInfo	SystemInfo;
	m_dwDispatchers = SystemInfo.dwNumberOfProcessors * DISPATCHER_RATIO;
}

BOOL DFMsgProcessor::CreateDispatchersInitially()
{
	for(DWORD i = 0; i < m_dwDispatchers; i++)
	{
		m_hDispatchers[i] = CreateDispatcher();
		if(m_hDispatchers[i] == NULL) 
		{
			m_dwDispatchers = i+1;
			return FALSE;
		}
	}
	return TRUE;
}

HANDLE DFMsgProcessor::CreateDispatcher()
{
	DWORD	dwThreadId;

	// create a dispatcher thread.
	HANDLE hDispatcher = BEGINTHREADEX(NULL, 0, DFDispatcherThread, this, CREATE_SUSPENDED, &dwThreadId);
	if(hDispatcher != NULL)
	{
		SetThreadPriority(hDispatcher, THREAD_PRIORITY_BELOW_NORMAL);
		ResumeThread(hDispatcher);
	}

	return hDispatcher;
}

DWORD DFMsgProcessor::DispatchAndExecute()
{
	BOOL				bSuccess = FALSE;
	void*				Key = NULL;
	DWORD				EventType = 0;
	LPOVERLAPPED		lpTempOv = NULL;

	for(BOOL bStay = TRUE; bStay; )
	{
		bSuccess = m_pIOCP->GetStatus(&Key, &EventType, &lpTempOv, INFINITE);

		// 이제 실제 iocp 패킷을 분석하자..
		if(bSuccess)
		{
			if(Key == NULL)		// 쓰레드 종료 명령.
			{
				bStay = FALSE;
			}
			else 
			{
				// 뭔가 메시지가 제대로 들어 왔다.
				// 작업 실시~!! 함수 포인터 이용~!
				(*m_ProcessFunc)(Key, EventType);
			}
		}
	}	
	return 0;
}

DWORD WINAPI DFDispatcherThread(LPVOID p)
{
	DFMsgProcessor* pDFMsgProcessor = (DFMsgProcessor*) p;
	return pDFMsgProcessor->DispatchAndExecute();
}
