// DFOpenMindService.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "stdlib.h"
#include <TCHAR.H>
#include "DFServiceStatus.h"
#include "DFIOCP.H"
#include "DFOpenMindCore.h"


#ifndef __COMPLETION_KEY
	#define CK_SERVICE_CONTROL		1
	#define CK_USERDATA_RECVED		2
#endif

TCHAR g_szServiceName[] = TEXT("DFOpenMindService");
DFServiceStatus g_ServiceStatus;


DWORD WINAPI DFOpenMindServiceHandlerEx(	DWORD dwControl, DWORD /*dwEventType*/, 
											PVOID /*pvEventData*/, PVOID pvContext)
{
	DWORD dwReturn = ERROR_CALL_NOT_IMPLEMENTED;
	BOOL bPostControlToServiceThread = FALSE;

	switch(dwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		g_ServiceStatus.SetUltimateState(SERVICE_STOPPED, 2000);
		bPostControlToServiceThread = TRUE;
		break;

	case SERVICE_CONTROL_PAUSE:
		g_ServiceStatus.SetUltimateState(SERVICE_PAUSED, 2000);
		bPostControlToServiceThread = TRUE;
		break;
	case SERVICE_CONTROL_CONTINUE:
		g_ServiceStatus.SetUltimateState(SERVICE_RUNNING, 2000);
		bPostControlToServiceThread = TRUE;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		g_ServiceStatus.ReportStatus();
		break;
	default:
		break;
	}

	if(bPostControlToServiceThread)
	{
		DFIOCP* pIOCP = (DFIOCP*)pvContext;
		pIOCP->PostStatus((LPVOID)CK_SERVICE_CONTROL, dwControl);
		dwReturn = NO_ERROR;
	}

	return dwReturn;
}

void WINAPI OpenMindServiceMain(DWORD /*dwArgc*/, PTSTR* /*pszArg*/)
{
	ULONG_PTR		CompKey = CK_SERVICE_CONTROL;				// 현재 콘트롤인지 수신인지 판별.
	DWORD			dwControl = SERVICE_CONTROL_CONTINUE;		// 콘트롤의 종류를 판별.
	DFIOCP			IOCP(0);									// 신호를 받을 IOCP 객체.
	OVERLAPPED*		pO = NULL;
	DFOpenMindCore	OpenMind;

	DFVERIFY(g_ServiceStatus.Initialize(	g_szServiceName, 
											DFOpenMindServiceHandlerEx,
											(PVOID) &IOCP,
											FALSE));
	g_ServiceStatus.AcceptControls(	SERVICE_ACCEPT_STOP |
									SERVICE_ACCEPT_SHUTDOWN |
									SERVICE_ACCEPT_PAUSE_CONTINUE);

	do
	{
		switch(CompKey)
		{
		case CK_SERVICE_CONTROL:
			switch(dwControl)
			{
			case SERVICE_CONTROL_CONTINUE:				
				DFVERIFY(OpenMind.StartService());
				DFVERIFY(g_ServiceStatus.ReportUltimateState());
				break;

			case SERVICE_CONTROL_PAUSE:
			case SERVICE_CONTROL_STOP:
			case SERVICE_CONTROL_SHUTDOWN:
				OpenMind.StopService();
				DFVERIFY(g_ServiceStatus.ReportUltimateState());
				break;
			default:
				break;
			}
			break;
		case CK_USERDATA_RECVED:
				break;
		default:
			break;
		}
		IOCP.GetStatus((void**)&CompKey, &dwControl, &pO);

	} while(g_ServiceStatus.GetCurrentState() != SERVICE_STOPPED);
}


void InstallService()
{
	SC_HANDLE hSCM = 
		OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	DFASSERT(hSCM != NULL);

	TCHAR szModulePathName[_MAX_PATH * 2];
	DFVERIFY(GetModuleFileName(	NULL, 
								szModulePathName, 
								sizeof(szModulePathName)/sizeof(szModulePathName[0])));
	lstrcat(szModulePathName, TEXT(" /service"));
	
	SC_HANDLE hService = 
		CreateService(	hSCM, 
						g_szServiceName, 
						g_szServiceName, 
						SERVICE_CHANGE_CONFIG,
						SERVICE_WIN32_OWN_PROCESS, 
						SERVICE_AUTO_START, 
						SERVICE_ERROR_NORMAL, 
						szModulePathName, 
						NULL, NULL, NULL, NULL, NULL);

	DFASSERT(hService != NULL);

	SERVICE_DESCRIPTION sd = 
	{
		TEXT("DFOpenMindService For DFOpenMind Service")
		TEXT("Mail: deepdark@archblitz.com)")
	};

	ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);

	DFVERIFY(CloseServiceHandle(hService));
	DFVERIFY(CloseServiceHandle(hSCM));
}

void RemoveService()
{
	SC_HANDLE hSCM = 
		OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	DFASSERT(hSCM != NULL);

	SC_HANDLE hService = 
		OpenService(hSCM, g_szServiceName, DELETE);

	DFASSERT(hService != NULL);

	DFVERIFY(DeleteService(hService));

	DFVERIFY(CloseServiceHandle(hService));
	DFVERIFY(CloseServiceHandle(hSCM));
}

int APIENTRY _tWinMain(HINSTANCE /*hInstance*/,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     /*lpCmdLine*/,
                     int       /*nCmdShow*/)
{
 	// TODO: Place code here.
	int nArgc; 

#ifdef _UNICODE
	PCTSTR* ppArgv = (PCTSTR*)CommandLineToArgvW((LPCWSTR)GetCommandLine(), &nArgc);
#else
	LPCTSTR* ppArgv = (LPCTSTR*) __argv;
	nArgc = __argc;
#endif	

	if(nArgc < 2)
	{
		MessageBox(	NULL,
					TEXT("DFOpenMindService - DFOpenMind Server Service.\n")
					TEXT("Author: deepdark@archblitz.com[Hongik Univ. Computer Eng.] \n\n")
					TEXT("Usage: DFOpenMindService.exe [/install] [/remove] ")
					TEXT("[/service]\n\n")
					TEXT("/install\t\tInstalls the service in the SCM's database.\n")
					TEXT("/remove\t\tRemoves the service from the SCM's database.\n")
//					TEXT("/debug\t\tRuns the service as a normal process for debugging.\n")
					TEXT("/service\t\tRuns the process as a service ")
					TEXT("(should only be set in SCM's database)."),
					g_szServiceName, MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		for(int i = 1; i < nArgc; i++)
		{
			if((ppArgv[i][0] == TEXT('-')) || (ppArgv[i][0] == TEXT('/')))
			{
				if(lstrcmpi(&ppArgv[i][1], TEXT("install")) == 0)
				{
					InstallService();
					MessageBox(NULL, TEXT("Service Installed"), g_szServiceName, MB_OK|MB_ICONINFORMATION);
				}
				if(lstrcmpi(&ppArgv[i][1], TEXT("remove")) == 0)
				{
					RemoveService();
					MessageBox(NULL, TEXT("Service Removed"), g_szServiceName, MB_OK|MB_ICONINFORMATION);
				}
				if(lstrcmpi(&ppArgv[i][1], TEXT("service")) == 0)
				{
					SERVICE_TABLE_ENTRY ServiceTable[] = 
					{
						{ g_szServiceName, OpenMindServiceMain },
						{ NULL, NULL }
					};

					DFVERIFY(StartServiceCtrlDispatcher(ServiceTable));
				}
			}
		}
	}

#ifdef _UNICODE
	HeapFree(GetProcessHeap(), 0, (LPVOID)ppArgv);
#endif

	return 0;
}



