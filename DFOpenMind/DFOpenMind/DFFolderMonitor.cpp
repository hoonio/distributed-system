#include "stdafx.h"
#include "DFFolderMonitor.h"
#include "DFHideMainFrame.h"

DWORD WINAPI DFMonitorThread(LPVOID p);

////////////////////////////////////////
extern DFHideMainFrame* g_pMainFrame;
////////////////////////////////////////

DFFolderMonitor::DFFolderMonitor(LPCTSTR tszFolder)
{
	ASSERT(tszFolder != NULL && lstrlen(tszFolder) != 0);

	lstrcpy(m_tszFolder, tszFolder);
	m_hStop = CreateEvent(0,0,0,0);
	m_hMonitor = CreateMonitorThread();
}

DFFolderMonitor::~DFFolderMonitor()
{
	if(m_bInService) SetEvent(m_hStop);
	WaitForSingleObject(m_hMonitor, INFINITE);
	CloseHandle(m_hMonitor);
	CloseHandle(m_hStop);
}

HANDLE DFFolderMonitor::CreateMonitorThread()
{
	DWORD	dwThreadId;

	// create a dispatcher thread.
	HANDLE hMonitor = BEGINTHREADEX(NULL, 0, DFMonitorThread, this, CREATE_SUSPENDED, &dwThreadId);
	if(hMonitor != NULL)
	{
		SetThreadPriority(hMonitor, THREAD_PRIORITY_BELOW_NORMAL);
		ResumeThread(hMonitor);
	}

	return hMonitor;
}

void DFFolderMonitor::OnNotify()
{
	////////////////////////////////////
	g_pMainFrame->RefleshLocalFiles();
	////////////////////////////////////
}

DWORD DFFolderMonitor::Monitor()
{
	DWORD	dwWaitStatus; 
	HANDLE	hWait[2];

	m_hNotify = 
		FindFirstChangeNotification(m_tszFolder, 
									FALSE,
									FILE_NOTIFY_CHANGE_FILE_NAME /*| 
									FILE_NOTIFY_CHANGE_SIZE | 
									FILE_NOTIFY_CHANGE_LAST_WRITE*/);

	if(m_hNotify == INVALID_HANDLE_VALUE) 
		return 0;

	m_bInService = TRUE;
	hWait[0] = m_hStop;
	hWait[1] = m_hNotify;

	while(TRUE) 
	{ 
		// Wait for notification.

		dwWaitStatus = WaitForMultipleObjects(2, hWait, FALSE, INFINITE); 

		switch(dwWaitStatus) 
		{ 
		case WAIT_OBJECT_0: 
			FindCloseChangeNotification(m_hNotify);
			m_bInService = FALSE;
			return 1;

		case WAIT_OBJECT_0 + 1: 
			OnNotify();
			if(FindNextChangeNotification(hWait[1]) == FALSE) 
			{
				m_bInService = FALSE;
				return 0; 
			}
			break; 
		} 
	}
} 


/////////////////////////////////////////////////////////////////////////////
DWORD WINAPI DFMonitorThread(LPVOID p)
{
	DFFolderMonitor* pFolderMonitor = (DFFolderMonitor*)p;
	return pFolderMonitor->Monitor();
}