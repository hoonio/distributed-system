#ifndef __DFGATE_H__
#define __DFGATE_H__

#pragma once

#include "DFCmn.h"

class DFGate
{
public:
	DFGate(BOOL bInitState = TRUE, PCTSTR pszName = NULL)
	{
		m_hEvent = CreateEvent(NULL, FALSE, bInitState, pszName);
		DFASSERT(m_hEvent != NULL);
	}

	~DFGate()
	{
		DFVERIFY(CloseHandle(m_hEvent));
	}

	DWORD WaitToEnterGate(DWORD dwTimeout = INFINITE, BOOL bAlertable = FALSE)
	{
		DFASSERT(this);
		return WaitForSingleObjectEx(m_hEvent, dwTimeout, bAlertable);
	}

	void LiftGate()
	{	
		DFASSERT(this);
		SetEvent(m_hEvent);
	}

private:
	HANDLE	m_hEvent;
};

#endif