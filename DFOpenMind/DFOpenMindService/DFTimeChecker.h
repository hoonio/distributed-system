#ifndef __DFTIMECHECKER_H__
#define __DFTIMECHECKER_H__

#pragma once

#include "DFCmn.h"

class DFTimeChecker
{
public:
	DFTimeChecker(UINT nTime = 1000);
	~DFTimeChecker();

	BOOL		Start();
	void		Stop();

	HANDLE		CreateTimeChecker();
	DWORD		TimeCheck();

	void		CheckAndDrop();

private:
	HANDLE		m_hChecker;
	HANDLE		m_hEvent;
	UINT		m_nTime;
	BOOL		m_bInService;
};




#endif