#ifndef __DFSERVICESTATUS_H__
#define __DFSERVICESTATUS_H__

#pragma once

#include "DFCmn.h"
#include "DFGate.h"

class DFServiceStatus : public SERVICE_STATUS
{
public:
	DFServiceStatus();
	~DFServiceStatus();

	// initialize service control handler.
	BOOL	Initialize(PCTSTR szServiceName, LPHANDLER_FUNCTION_EX pFnHandler,
		PVOID pvContext, BOOL bOwnProcess, BOOL bInteractWithDesktop = FALSE);

	// accept or de-accept new service controls - like stop, start.. 
	void	AcceptControls(DWORD dwFlags, BOOL bAccept = TRUE);

	// report current service_status to SCM
	BOOL	ReportStatus();

	// set next state with synchronization.
	BOOL	SetUltimateState(DWORD dwState, DWORD dwWaitHintArg = 0);

	// report to SCM that service is progressing.
	BOOL	AdvanceState(DWORD dwWaitHintArg, DWORD dwCheckPointArg = 0);

	// report changed state to SCM
	BOOL	ReportUltimateState();

	BOOL	ReportWin32Error(DWORD dwError);
	BOOL	ReportServiceSpecificError(DWORD dwError);

	DWORD	GetCurrentState(){return dwCurrentState;};
	
private:
	SERVICE_STATUS_HANDLE	m_hServiceStatus;
	DFGate					m_Gate;
};

#endif


