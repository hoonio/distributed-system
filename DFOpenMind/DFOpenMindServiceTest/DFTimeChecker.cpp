#include "stdafx.h"
#include "DFTimeChecker.h"
#include "DFOpenMindCore.h"

extern DFOpenMindCore* g_pOpenMindCore;
DWORD WINAPI TimeCheckerThread(LPVOID p);



DFTimeChecker::DFTimeChecker(UINT nTime)
{
	m_hChecker	= NULL;
	m_nTime		= nTime;
	m_hEvent	= CreateEvent(NULL, 0, 0, NULL);
	DFASSERT(m_hEvent != NULL);
	m_bInService = FALSE;
}

DFTimeChecker::~DFTimeChecker()
{
	if(m_bInService) Stop();
	CloseHandle(m_hEvent);
	m_hEvent = NULL;
}

BOOL DFTimeChecker::Start()
{
	m_hChecker = CreateTimeChecker();
	if(!m_hChecker) return FALSE;

	m_bInService = TRUE;
	return TRUE;
}

void DFTimeChecker::Stop()
{
	SetEvent(m_hEvent);

	DWORD dwWait = WaitForSingleObject(m_hChecker, 3000);
	if(dwWait == WAIT_TIMEOUT)
	{
		DWORD dwExitCode;

		GetExitCodeThread(m_hChecker, &dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
			TerminateThread(m_hChecker , 0);

	}
	m_hChecker = NULL;
}

HANDLE DFTimeChecker::CreateTimeChecker()
{
	DWORD	dwThreadId;

	// create a dispatcher thread.
	HANDLE hChecker = BEGINTHREADEX(NULL, 0, TimeCheckerThread, this, CREATE_SUSPENDED, &dwThreadId);
	if(hChecker != NULL)
	{
		SetThreadPriority(hChecker, THREAD_PRIORITY_BELOW_NORMAL);
		ResumeThread(hChecker);
	}

	return hChecker;
}

DWORD DFTimeChecker::TimeCheck()
{
	for(;;)
	{
		DWORD dwWait = WaitForSingleObject(m_hEvent, m_nTime);
		if(dwWait == WAIT_OBJECT_0)
		{
			return 1;
		}
		else if(dwWait == WAIT_TIMEOUT)
		{
			CheckAndDrop();
		}
	}
}

// 실제 작업을 해주는 함수~!
void DFTimeChecker::CheckAndDrop()
{
	g_pOpenMindCore->CheckAndKillConnection();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI TimeCheckerThread(LPVOID p)
{
	DFTimeChecker* pTimeChecker = (DFTimeChecker*) p;
	return pTimeChecker->TimeCheck();
}