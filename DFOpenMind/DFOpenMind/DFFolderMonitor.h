#ifndef __DFFOLDERMONITOR_H__
#define __DFFOLDERMONITOR_H__

#pragma once

#include  <process.h> 

typedef unsigned (__stdcall *PTHREAD_START)(void*);
#define BEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadId) \
	((HANDLE)_beginthreadex((void*)(psa), \
	(unsigned)(cbStack), (PTHREAD_START)(pfnStartAddr),\
	(void*)(pvParam), (unsigned)(fdwCreate), (unsigned*)(pdwThreadId)))

class DFFolderMonitor
{
public:
	DFFolderMonitor(LPCTSTR tszFolder);
	~DFFolderMonitor();


	void		OnNotify();
	HANDLE		CreateMonitorThread();
	DWORD		Monitor();

private:
	HANDLE		m_hNotify;
	HANDLE		m_hMonitor;
	HANDLE		m_hStop;
	TCHAR		m_tszFolder[MAX_PATH];
	BOOL		m_bInService;
};



#endif

