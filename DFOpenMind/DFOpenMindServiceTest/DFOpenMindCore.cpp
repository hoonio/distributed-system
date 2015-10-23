#include "stdafx.h"
#include "DFOpenMindCore.h"

DFOpenMindCore* g_pOpenMindCore = NULL;
void MsgProcessFunc(void* Key, DWORD EventType);


DFOpenMindCore::DFOpenMindCore() : m_IOCP(0), m_TimeChecker(CHECK_TIME)
{
	g_pOpenMindCore = this;
	m_bInService	= FALSE;
	m_pMsgProcessor = NULL;
	m_pServerSock	= NULL;
	m_pUserDB		= NULL;
	m_btCurrentUsers= 0;

	InitializeCriticalSection(&m_CS);
}

DFOpenMindCore::~DFOpenMindCore()
{
	if(m_bInService) StopService();
	DeleteCriticalSection(&m_CS);
}

BOOL DFOpenMindCore::StartService()
{
	if(!m_bInService)
	{
		
		m_pUserDB = new DFUserDB;
		m_pUserDB->CreateOrOpenDBFile();
		m_pMsgProcessor = new DFMsgProcessor(&m_IOCP, (PRCSSFUNC)MsgProcessFunc);
		m_pServerSock = new DFServerSock(&m_IOCP, MAX_MSGSIZE);
		m_pServerSock->Initialize(SERVICE_PORT);
		
		if(m_pServerSock->StartService())
		{
			if(m_TimeChecker.Start())
			{
				m_bInService = TRUE;
				return TRUE;
			}
		}
		return FALSE;
	}
	return TRUE;
}

void DFOpenMindCore::StopService()
{
	if(m_bInService)
	{
		m_TimeChecker.Stop();
		m_pServerSock->StopService();
		delete m_pServerSock; m_pServerSock = NULL;
		delete m_pMsgProcessor; m_pMsgProcessor = NULL;
		delete m_pUserDB; m_pUserDB = NULL;
		m_bInService = FALSE;
	}
}

void DFOpenMindCore::CloseForcibly(void* Key)
{
	m_pServerSock->CloseClientForcibly((PPER_SOCKET_CONTEXT)Key);
}

// 소켓을 통해 수신 받은 메시지를 읽어온다.
inline BOOL DFOpenMindCore::GetRecvedMsg(void* Key, BYTE* MsgBuf, UINT* pMsgSize)
{
	return m_pServerSock->RecvMsg(Key, MsgBuf, pMsgSize);
}

BOOL DFOpenMindCore::Send(void* Key, BYTE* Msg, UINT nMsgSize)
{
	return m_pServerSock->SendMsg(Key, Msg, nMsgSize);
}

void DFOpenMindCore::Broadcast(BYTE* Msg, UINT nMsgSize, LPCTSTR tszUserID)
{
	void*		Key = NULL;
	USER_INFO*	pUserInfo = NULL;

	EnterCriticalSection(&m_CS);

	// 연결된 모든 유저에게 메시지를 날린다.
	POSITION pos = m_KeyUserMap.GetStartPosition();
	while(pos)
	{
		m_KeyUserMap.GetNextAssoc(pos, (void*&)Key, (void*&)pUserInfo);
		if(Key && pUserInfo)
		{
			if(tszUserID == NULL || lstrcmpi(pUserInfo->UserId, tszUserID) != 0)
				Send(Key, Msg, nMsgSize);
		}
	}

	LeaveCriticalSection(&m_CS);
}


// 새로 접속한 유저에게 기존 접속 유저들의 리스트를 송신.
void DFOpenMindCore::SendLoggedUserInfos(void* Key)
{
	void*		Key2 = NULL;
	USER_INFO*	pUserInfo = NULL;
	BYTE		MsgBuf[4096] = {0,};
	BYTE		nUsers = 0;
	BYTE*		pLoc = MsgBuf+3;

	EnterCriticalSection(&m_CS);

	nUsers = (BYTE)m_KeyUserMap.GetCount();

	// 연결된 모든 유저의 정보를 날린다.
	POSITION pos = m_KeyUserMap.GetStartPosition();
	while(pos)
	{
		m_KeyUserMap.GetNextAssoc(pos, (void*&)Key2, (void*&)pUserInfo);
		if(Key2 && pUserInfo)
		{
			memcpy(pLoc, &pUserInfo->dwIP, sizeof(DWORD));
			pLoc += sizeof(DWORD);
			lstrcpy((TCHAR*)pLoc, pUserInfo->UserId);
			pLoc += (USER_ID_LENGTH+1);
#ifdef _DEBUG
//			LOGOUT("LoggedUser!!, UserID : %s", pUserInfo->UserId);
#endif
		}
	}

	LeaveCriticalSection(&m_CS);

	MsgBuf[0] = DFOM_SC_LOGINUSERS;
	MsgBuf[1] = nUsers;
	MsgBuf[2] = 0;

	Send(Key, MsgBuf, 3+(sizeof(TCHAR)+4)*((USER_ID_LENGTH+1)*nUsers));
}

// 접속한 유저가 로그 아웃했음을 다른 유저들에게 브로드캐스트.
void DFOpenMindCore::BroadcastLogout(LPCTSTR tszUserID)
{
	BYTE	MsgBuf[4096] = {0,};

	MsgBuf[0] = DFOM_SC_LOGOUT;
	lstrcpy((TCHAR*)MsgBuf+1, tszUserID);

	Broadcast(MsgBuf, 1+sizeof(TCHAR)*(USER_ID_LENGTH+1));
}

// 새로운 유저가 접속했음을 다른 유저들에게 브로드캐스트.
void DFOpenMindCore::BroadcastLogin(LPCTSTR tszUserID)
{
	BYTE	MsgBuf[4096] = {0,};

	MsgBuf[0] = DFOM_SC_LOGINUSERS;
	MsgBuf[1] = 1;
	MsgBuf[2] = 1;
	lstrcpy((TCHAR*)MsgBuf+3, tszUserID);

	Broadcast(MsgBuf, 3+sizeof(TCHAR)*(USER_ID_LENGTH+1), tszUserID);
}

// 로그인을 시도한 클라이언트에게 로긴 결과를 통보.
void DFOpenMindCore::SendLoginResponse(void* Key, LPCTSTR tszUserID, BYTE btSuccess, BYTE btErrCode)
{
	BYTE	MsgBuf[4096];

	MsgBuf[0] = DFOM_SC_LOGIN;
	MsgBuf[1] = btSuccess;
	MsgBuf[2] = btErrCode;
	lstrcpy((TCHAR*)MsgBuf+3, tszUserID);

	Send(Key, MsgBuf, 3+sizeof(TCHAR)*(USER_ID_LENGTH+1));
}

// 한 유저에게 HeartBeat를 송신하라고 요청한다.
void DFOpenMindCore::SendAliveAsk(void* Key)
{
	BYTE MsgType = DFOM_SC_ALIVE;
	Send(Key, &MsgType, 1);
}

// 수신 처리되었을 경우 타임아웃에 안걸리게 0으로 만듭니다.
void DFOpenMindCore::SetRecvCountZero(void* Key)
{
	if(USER_INFO* pUserInfo = FindUserInfoWithKey(Key))
	{
		InterlockedExchange((LPLONG)&pUserInfo->nNotRecvedCount, 0);
	}
}

BOOL DFOpenMindCore::IsLoggedUser(LPCTSTR tszUserID)
{
	USER_INFO* pUserInfo;
	BOOL bRet = FALSE;

	EnterCriticalSection(&m_CS);
	
	if(m_NameUserMap.Lookup(tszUserID, (void*&)pUserInfo))
		bRet = TRUE;

	LeaveCriticalSection(&m_CS);

	return bRet;
}

BYTE DFOpenMindCore::IsValidUserAndPassword(LPCTSTR tszUsrID, LPCTSTR tszPwd)
{
	return m_pUserDB->CheckUser(tszUsrID, tszPwd);
}

USER_INFO* DFOpenMindCore::FindUserInfoWithKey(void* Key)
{
	USER_INFO* pUserInfo = NULL;

	EnterCriticalSection(&m_CS);
	m_KeyUserMap.Lookup(Key, (void*&)pUserInfo);
	LeaveCriticalSection(&m_CS);

	return pUserInfo;
}

void DFOpenMindCore::AddKeyUserInfoMap(void* Key, USER_INFO* pUserInfo)
{
	EnterCriticalSection(&m_CS);

	m_KeyUserMap[Key] = pUserInfo;
	if(lstrcmpi(pUserInfo->UserId, TEXT("")) != 0) 
		m_NameUserMap[pUserInfo->UserId] = pUserInfo;

	LeaveCriticalSection(&m_CS);
}

void DFOpenMindCore::AddNameUserInfoMap(USER_INFO* pUserInfo)
{
	EnterCriticalSection(&m_CS);

	m_NameUserMap[pUserInfo->UserId] = pUserInfo;

	LeaveCriticalSection(&m_CS);
}

void DFOpenMindCore::RemoveUserInfoMap(void* Key)
{
	USER_INFO* pUserInfo = NULL;

	EnterCriticalSection(&m_CS);

	m_KeyUserMap.Lookup(Key, (void*&)pUserInfo);
	m_KeyUserMap.RemoveKey(Key);
	m_NameUserMap.RemoveKey(pUserInfo->UserId);

	LeaveCriticalSection(&m_CS);
}

DWORD DFOpenMindCore::GetLoggedUserCount()
{
	EnterCriticalSection(&m_CS);

	DWORD dwCount  = m_NameUserMap.GetCount();

	LeaveCriticalSection(&m_CS);

	return dwCount;
}

void DFOpenMindCore::GetLoggedUserNameArray(LPTSTR tszArray)
{
	USER_INFO* pUserInfo = NULL;
	TCHAR Key[255];
	int i = 0;

	EnterCriticalSection(&m_CS);
	POSITION pos = m_NameUserMap.GetStartPosition();
	while(pos)
	{
		m_NameUserMap.GetNextAssoc(pos, (LPTSTR&)Key, (void*&)pUserInfo);
		if(pUserInfo)
		{
			// 이 코드가 맞는 지.. 12인지 13인지 헤깔린다.. 헐헐..
			lstrcpyn(tszArray + (i++ * 13), pUserInfo->UserId, 12);
		}
	}

	LeaveCriticalSection(&m_CS);
}

void DFOpenMindCore::ProcessMessage(void* Key, DWORD EventType)
{
	DFASSERT(Key);

	if(Key)
	{
		switch(EventType)
		{
		case INFORM_ACCEPT :
			OnAccept(Key);
			break;
		case INFORM_RECV :
			OnReceive(Key);
			break;
		case INFORM_CLOSE :
			OnClose(Key);
			break;
		default:
			break;
		}
	}
}

void DFOpenMindCore::OnReceive(void* Key)
{
	BYTE				MsgBuf[MAX_MSGSIZE];
	UINT				nMsgSize = 0;

	if(GetRecvedMsg(Key, MsgBuf, &nMsgSize))
	{
		// 일단 짤리지 않게 TIMEOUT을 0으로 세팅한다~!
		SetRecvCountZero(Key);

		// 메시지 타입에 따라 작업 실시!
		switch(MsgBuf[0])
		{
		case DFOM_CS_LOGIN		:
			OnMsgLogin(Key, MsgBuf+1, nMsgSize-1);
			break;
		case DFOM_CS_LOGOUT		:
			OnMsgLogout(Key, MsgBuf+1, nMsgSize-1);
			break;
		case DFOM_CS_NEWUSER	:
			OnMsgNewUser(Key, MsgBuf+1, nMsgSize-1);
			break;
		case DFOM_CS_SENDINFOS	:
			OnMsgSendInfos(Key, MsgBuf+1, nMsgSize-1);
			break;
		case DFOM_CS_SEARCH		:
			OnMsgSearch(Key, MsgBuf+1, nMsgSize-1);
			break;
		case DFOM_CS_ALIVE		:
			break;
		default:
			break;
		}
	}
}

//  연결이 종료된 경우 처리.
void DFOpenMindCore::OnClose(void* Key)
{
	USER_INFO*			pUserInfo = NULL;

	pUserInfo = FindUserInfoWithKey(Key);
	if(pUserInfo)
	{
		// 모든 사용자에게 사용자 로그아웃울 알린다.
		if(lstrlen(pUserInfo->UserId))
			BroadcastLogout(pUserInfo->UserId);

		// 해당 유저의 공유 파일 정보를 제거한다.
		m_FileDB.RemoveFileInfoAll(pUserInfo->UserId);
		
		// 유저 컨텍스트를 날린다.
		RemoveUserInfoMap(Key);
		delete pUserInfo; pUserInfo = NULL;	
	}
}

void DFOpenMindCore::OnAccept(void* Key)
{
	// 새로운 유저 객체 생성
	USER_INFO* pUserInfo = new USER_INFO;

	pUserInfo->Key				= Key;
	pUserInfo->nNotRecvedCount	= 0;
	lstrcpy(pUserInfo->UserId, TEXT(""));
	pUserInfo->dwIP = m_pServerSock->GetClientIP((PPER_SOCKET_CONTEXT) Key);

	// 전체 유저를 관리하는 맵에 첨가.
	AddKeyUserInfoMap(Key, pUserInfo);
}

// 타임아웃되었는 지 체크후 타임아웃된 클라이언트는 죽인다!!
void DFOpenMindCore::CheckAndKillConnection()
{
	void*		Key = NULL;
	USER_INFO*	pUserInfo = NULL;

	EnterCriticalSection(&m_CS);

	// 일단 연결된 모든 유저에게 메시지를 날린다.
	POSITION pos = m_KeyUserMap.GetStartPosition();
	while(pos)
	{
		m_KeyUserMap.GetNextAssoc(pos, (void*&)Key, (void*&)pUserInfo);
		if(Key && pUserInfo)
		{
			if(pUserInfo->nNotRecvedCount > CHECK_COUNT)
			{
				// 살아있는 지 문의한다.
				SendAliveAsk(Key);
			}

			// 타임 아웃을 체크하고 넘어선 유저를 죽인다.
			if(pUserInfo->nNotRecvedCount > NOT_ALIVE)
			{
				CloseForcibly(Key);
			}
			else 
			{
				InterlockedIncrement((LPLONG)&pUserInfo->nNotRecvedCount);
			}
		}
	}

	LeaveCriticalSection(&m_CS);
}


// 클라이언트의 로긴 요청 메시지 수신시 처리.
void DFOpenMindCore::OnMsgLogin(void* Key, BYTE* pMsg, UINT /*nMsgSize*/)
{
	BYTE	Msg[4096];
	BYTE	btRet;
	USER_INFO* pUserInfo = FindUserInfoWithKey(Key);

	DFASSERT(pUserInfo != NULL);

#ifdef _DEBUG
//	LOGOUT("SERVER: LOGIN MSG(ID : %s, Pwd: %s)", pMsg, (LPCTSTR)pMsg+sizeof(TCHAR)*13);
#endif
	
	// 행여 같은 id가 이미 서버에 로그인되어 있는 지 확인한다.
	if(!IsLoggedUser((LPCTSTR)pMsg))
	{
		// 해당 클라이언트가 DB에 존재하고, Password가 맞는 지 확인한다.
		btRet = IsValidUserAndPassword((LPCTSTR)pMsg, (LPCTSTR)pMsg+sizeof(TCHAR)*13);
	}
	else btRet = FAIL_USER_ALREADY_LOGIN;

	// 메시지 송신 관련~!!
	if(btRet == FAIL_SUCCESS)
	{
		// 문제가 없으면, 유저를 등록한다.
		lstrcpy(pUserInfo->UserId, (LPCTSTR)pMsg);
		AddNameUserInfoMap(pUserInfo);

		// 로긴 결과를 응답합니다.~!
		Msg[0] = DFOM_SC_LOGIN;
		Msg[1] = 1;
		Msg[2] = btRet;
		lstrcpyn((LPTSTR)Msg+3, (LPCTSTR)pMsg, 12);
		Send(Key, Msg, 3 + sizeof(TCHAR)*13);
#ifdef _DEBUG
//	LOGOUT("SERVER: LOG RESULT(%d, %d)", Msg[1], Msg[2]);
#endif

		// 이 클라이언트에게 현재 로긴된 사용자 정보를 송신합니다.
		SendLoggedUserInfos(Key);

		// 다른 클라이언트에게 이 클라이언트의 로긴을 알림.
		Msg[0] = DFOM_SC_LOGINUSERS;
		Msg[1] = 1;
		Msg[2] = 1;
		memcpy(Msg+3, &pUserInfo->dwIP, 4);
		lstrcpy((LPTSTR)Msg+7, (LPCTSTR)pMsg);

		Broadcast(Msg, 7 + sizeof(TCHAR)*13, pUserInfo->UserId);
		
	}
	else
	{
		// 로그인 실패 메시지..
		Msg[0] = DFOM_SC_LOGIN;
		Msg[1] = 0;
		Msg[2] = btRet;
		lstrcpyn((LPTSTR)Msg+3, (LPCTSTR)pMsg, 12);

		Send(Key, Msg, 3 + sizeof(TCHAR)*13);

#ifdef _DEBUG
//		LOGOUT("SERVER: LOG RESULT(%d, %d)", Msg[1], Msg[2]);
#endif
	}	
}

// 클라이언트의 로그 아웃 메시지 수신시 처리.
void DFOpenMindCore::OnMsgLogout(void* /*Key*/, BYTE* /*pMsg*/, UINT /*nMsgSize*/)
{
	// 일단 로그아웃 메시지는 안쓰는 걸로 하자.!!
}

// 클라이언트의 새로운 유저 생성 메시지 수신시 처리.
void DFOpenMindCore::OnMsgNewUser(void* Key, BYTE* pMsg, UINT nMsgSize)
{
	BYTE	Msg[256];

	PUSER_RECORD pUserRecord = new USER_RECORD;
	memcpy(pUserRecord, pMsg, nMsgSize);
	
	BYTE btRet = m_pUserDB->AddNewUser(pUserRecord);

	Msg[0] = DFOM_SC_NEWUSER;
	Msg[1] = (btRet == FAIL_SUCCESS) ?  (BYTE)1 : (BYTE)0;
	Msg[2] = btRet;
	lstrcpyn((LPTSTR)Msg+3, pUserRecord->tszUserID, 12);

#ifdef _DEBUG

//	LOGOUT("SERVER: NEWUSER(%s, %s)", pUserRecord->tszUserID, pUserRecord->tszPassword);
#endif

	Send(Key, Msg, 3 + sizeof(TCHAR)*13);
}

// 클라이언트의 자신의 공유 파일 리스트 메시지를 수신시 처리.
void DFOpenMindCore::OnMsgSendInfos(void* /*Key*/, BYTE* pMsg, UINT nMsgSize)
{
	DWORD dwFileInfoCount = nMsgSize / sizeof(FILE_INFO);
	for(DWORD i = 0; i < dwFileInfoCount; i++)
	{
		m_FileDB.AddNewFileInfo((PFILE_INFO)(pMsg+i*sizeof(FILE_INFO)));
	}
}

// 클라이언트의 파일 검색 메시지 수신시 처리.
void DFOpenMindCore::OnMsgSearch(void* Key, BYTE* pMsg, UINT /*nMsgSize*/)
{
	WORD	i = 0;
	BYTE	Msg[4096];
	UINT	nCount = 0;
	WORD	wdMaxResult = *(WORD*)pMsg;
	PFILE_INFO pInfoArray = 
		m_FileDB.FindFilesWithRequirement((PFILE_FIND_INFO)(pMsg+2), wdMaxResult, nCount);
//TH
//#ifdef _DEBUG
//	PFILE_FIND_INFO pFind = (PFILE_FIND_INFO)(pMsg+2);
//	LOGOUT("SERVER: SEARCH REQUEST(wdMaxResult : %d, UserID: %s, Result Set : %d)",wdMaxResult, pFind->tszUserID, nCount);
//#endif

	// 검색된 결과를 가지고 응답 메시지를 송신한다.
	Msg[0] = DFOM_SC_SEARCH_RESULTS;
	*(WORD*)(Msg+1) = (WORD)nCount;
	
	// 검색 실패시.
	if(nCount == 0)
	{
		*(WORD*)(Msg+3) = 0;
		Send(Key, Msg, 5);
	}
	else
	{
		while(i < nCount)
		{
			*(WORD*)(Msg+3) = (WORD)(i+1);
			memcpy(Msg+5, pInfoArray+(i++), sizeof(FILE_INFO));
			Send(Key, Msg, 5+sizeof(FILE_INFO));
//TH
//#ifdef _DEBUG
//			LOGOUT("sERVER: A SEARCH RESULT SEND!!!");
//#endif
		}
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void MsgProcessFunc(void* Key, DWORD EventType)
{
	g_pOpenMindCore->ProcessMessage(Key, EventType);
}