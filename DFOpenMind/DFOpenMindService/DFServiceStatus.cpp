#include "stdafx.h"
#include "dfservicestatus.h"

DFServiceStatus::DFServiceStatus()
{
	ZeroMemory(this, sizeof(SERVICE_STATUS));
	m_hServiceStatus = NULL;
}

DFServiceStatus::~DFServiceStatus()
{

}

BOOL DFServiceStatus::Initialize(PCTSTR szServiceName, LPHANDLER_FUNCTION_EX pFnHandler,
									PVOID pvContext, BOOL bOwnProcess, BOOL bInteractWithDesktop)
{
	m_hServiceStatus =
		RegisterServiceCtrlHandlerEx(szServiceName, pFnHandler, pvContext);

	DFASSERT(m_hServiceStatus != NULL);

	dwServiceType = bOwnProcess ? SERVICE_WIN32_OWN_PROCESS : SERVICE_WIN32_SHARE_PROCESS;

	if(bInteractWithDesktop)
	{
		dwServiceType |= SERVICE_INTERACTIVE_PROCESS;
	}

	dwCurrentState				= SERVICE_START_PENDING;
	dwControlsAccepted			= 0;
	dwWin32ExitCode				= NO_ERROR;
	dwServiceSpecificExitCode	= 0;
	dwCheckPoint				= 0;
	dwWaitHint					= 2000;
	
	return (m_hServiceStatus != NULL);
}

void DFServiceStatus::AcceptControls(DWORD dwFlags, BOOL bAccept)
{
	if(bAccept)
		dwControlsAccepted |= dwFlags;
	else
		dwControlsAccepted &= ~dwFlags;
}

BOOL DFServiceStatus::ReportStatus()
{
	BOOL bRet = SetServiceStatus(m_hServiceStatus, this);
	DFASSERT(bRet);

	return bRet;
}

BOOL DFServiceStatus::ReportWin32Error(DWORD dwError)
{
	dwWin32ExitCode = dwError;
	dwServiceSpecificExitCode = 0;
	return (ReportStatus());
}

BOOL DFServiceStatus::ReportServiceSpecificError(DWORD dwError)
{
	dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
	dwServiceSpecificExitCode = dwError;
	return (ReportStatus());
}

BOOL DFServiceStatus::SetUltimateState(DWORD dwState, DWORD dwWaitHintArg)
{
	DWORD dwPendingState = 0;

	switch(dwState)
	{
	case SERVICE_STOPPED:
		dwPendingState = SERVICE_STOP_PENDING;
		break;
	case SERVICE_RUNNING:
		dwPendingState = 
			(dwCurrentState == SERVICE_PAUSED) ? SERVICE_CONTINUE_PENDING : SERVICE_START_PENDING;
		break;
	case SERVICE_PAUSED:
		dwPendingState = SERVICE_PAUSE_PENDING;
		break;
	default:
		DFASSERT(dwState != 0);
		break;
	}

	dwCheckPoint = 1;
	dwWaitHint = dwWaitHintArg;

	dwWin32ExitCode = NO_ERROR;
	dwServiceSpecificExitCode = 0;

	BOOL bRet = FALSE;
	if(dwPendingState != 0)
	{
		m_Gate.WaitToEnterGate();

		dwCurrentState = dwPendingState;

		bRet = (dwWaitHintArg != 0) ? ReportStatus() : ReportUltimateState();
	}
	return bRet;
}

BOOL DFServiceStatus::AdvanceState(DWORD dwWaitHintArg, DWORD dwCheckPointArg)
{
	dwCheckPoint = (dwCheckPointArg == 0) ? dwCheckPoint + 1 : dwCheckPoint;
	dwWaitHint = dwWaitHintArg;

	dwWin32ExitCode = NO_ERROR;
	dwServiceSpecificExitCode = 0;
	
	return (ReportStatus());
}

BOOL DFServiceStatus::ReportUltimateState()
{
	DWORD dwUltimateState = 0;

	switch(dwCurrentState)
	{
	case SERVICE_START_PENDING:
	case SERVICE_CONTINUE_PENDING:
		dwUltimateState = SERVICE_RUNNING;
		break;
	case SERVICE_STOP_PENDING:
		dwUltimateState = SERVICE_STOPPED;
		break;
	case SERVICE_PAUSE_PENDING:
		dwUltimateState = SERVICE_PAUSED;
		break;
	}

	dwCheckPoint = dwWaitHint = 0;

	dwWin32ExitCode = NO_ERROR;
	dwServiceSpecificExitCode = 0;

	BOOL bRet = FALSE;
	if(dwUltimateState != 0)
	{
		dwCurrentState = dwUltimateState;
		bRet = ReportStatus();

		m_Gate.LiftGate();
	}

	return bRet;
}










