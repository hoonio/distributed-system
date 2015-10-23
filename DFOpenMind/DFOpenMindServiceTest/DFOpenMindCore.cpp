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

// ������ ���� ���� ���� �޽����� �о�´�.
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

	// ����� ��� �������� �޽����� ������.
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


// ���� ������ �������� ���� ���� �������� ����Ʈ�� �۽�.
void DFOpenMindCore::SendLoggedUserInfos(void* Key)
{
	void*		Key2 = NULL;
	USER_INFO*	pUserInfo = NULL;
	BYTE		MsgBuf[4096] = {0,};
	BYTE		nUsers = 0;
	BYTE*		pLoc = MsgBuf+3;

	EnterCriticalSection(&m_CS);

	nUsers = (BYTE)m_KeyUserMap.GetCount();

	// ����� ��� ������ ������ ������.
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

// ������ ������ �α� �ƿ������� �ٸ� �����鿡�� ��ε�ĳ��Ʈ.
void DFOpenMindCore::BroadcastLogout(LPCTSTR tszUserID)
{
	BYTE	MsgBuf[4096] = {0,};

	MsgBuf[0] = DFOM_SC_LOGOUT;
	lstrcpy((TCHAR*)MsgBuf+1, tszUserID);

	Broadcast(MsgBuf, 1+sizeof(TCHAR)*(USER_ID_LENGTH+1));
}

// ���ο� ������ ���������� �ٸ� �����鿡�� ��ε�ĳ��Ʈ.
void DFOpenMindCore::BroadcastLogin(LPCTSTR tszUserID)
{
	BYTE	MsgBuf[4096] = {0,};

	MsgBuf[0] = DFOM_SC_LOGINUSERS;
	MsgBuf[1] = 1;
	MsgBuf[2] = 1;
	lstrcpy((TCHAR*)MsgBuf+3, tszUserID);

	Broadcast(MsgBuf, 3+sizeof(TCHAR)*(USER_ID_LENGTH+1), tszUserID);
}

// �α����� �õ��� Ŭ���̾�Ʈ���� �α� ����� �뺸.
void DFOpenMindCore::SendLoginResponse(void* Key, LPCTSTR tszUserID, BYTE btSuccess, BYTE btErrCode)
{
	BYTE	MsgBuf[4096];

	MsgBuf[0] = DFOM_SC_LOGIN;
	MsgBuf[1] = btSuccess;
	MsgBuf[2] = btErrCode;
	lstrcpy((TCHAR*)MsgBuf+3, tszUserID);

	Send(Key, MsgBuf, 3+sizeof(TCHAR)*(USER_ID_LENGTH+1));
}

// �� �������� HeartBeat�� �۽��϶�� ��û�Ѵ�.
void DFOpenMindCore::SendAliveAsk(void* Key)
{
	BYTE MsgType = DFOM_SC_ALIVE;
	Send(Key, &MsgType, 1);
}

// ���� ó���Ǿ��� ��� Ÿ�Ӿƿ��� �Ȱɸ��� 0���� ����ϴ�.
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
			// �� �ڵ尡 �´� ��.. 12���� 13���� ��򸰴�.. ����..
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
		// �ϴ� ©���� �ʰ� TIMEOUT�� 0���� �����Ѵ�~!
		SetRecvCountZero(Key);

		// �޽��� Ÿ�Կ� ���� �۾� �ǽ�!
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

//  ������ ����� ��� ó��.
void DFOpenMindCore::OnClose(void* Key)
{
	USER_INFO*			pUserInfo = NULL;

	pUserInfo = FindUserInfoWithKey(Key);
	if(pUserInfo)
	{
		// ��� ����ڿ��� ����� �α׾ƿ��� �˸���.
		if(lstrlen(pUserInfo->UserId))
			BroadcastLogout(pUserInfo->UserId);

		// �ش� ������ ���� ���� ������ �����Ѵ�.
		m_FileDB.RemoveFileInfoAll(pUserInfo->UserId);
		
		// ���� ���ؽ�Ʈ�� ������.
		RemoveUserInfoMap(Key);
		delete pUserInfo; pUserInfo = NULL;	
	}
}

void DFOpenMindCore::OnAccept(void* Key)
{
	// ���ο� ���� ��ü ����
	USER_INFO* pUserInfo = new USER_INFO;

	pUserInfo->Key				= Key;
	pUserInfo->nNotRecvedCount	= 0;
	lstrcpy(pUserInfo->UserId, TEXT(""));
	pUserInfo->dwIP = m_pServerSock->GetClientIP((PPER_SOCKET_CONTEXT) Key);

	// ��ü ������ �����ϴ� �ʿ� ÷��.
	AddKeyUserInfoMap(Key, pUserInfo);
}

// Ÿ�Ӿƿ��Ǿ��� �� üũ�� Ÿ�Ӿƿ��� Ŭ���̾�Ʈ�� ���δ�!!
void DFOpenMindCore::CheckAndKillConnection()
{
	void*		Key = NULL;
	USER_INFO*	pUserInfo = NULL;

	EnterCriticalSection(&m_CS);

	// �ϴ� ����� ��� �������� �޽����� ������.
	POSITION pos = m_KeyUserMap.GetStartPosition();
	while(pos)
	{
		m_KeyUserMap.GetNextAssoc(pos, (void*&)Key, (void*&)pUserInfo);
		if(Key && pUserInfo)
		{
			if(pUserInfo->nNotRecvedCount > CHECK_COUNT)
			{
				// ����ִ� �� �����Ѵ�.
				SendAliveAsk(Key);
			}

			// Ÿ�� �ƿ��� üũ�ϰ� �Ѿ ������ ���δ�.
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


// Ŭ���̾�Ʈ�� �α� ��û �޽��� ���Ž� ó��.
void DFOpenMindCore::OnMsgLogin(void* Key, BYTE* pMsg, UINT /*nMsgSize*/)
{
	BYTE	Msg[4096];
	BYTE	btRet;
	USER_INFO* pUserInfo = FindUserInfoWithKey(Key);

	DFASSERT(pUserInfo != NULL);

#ifdef _DEBUG
//	LOGOUT("SERVER: LOGIN MSG(ID : %s, Pwd: %s)", pMsg, (LPCTSTR)pMsg+sizeof(TCHAR)*13);
#endif
	
	// �࿩ ���� id�� �̹� ������ �α��εǾ� �ִ� �� Ȯ���Ѵ�.
	if(!IsLoggedUser((LPCTSTR)pMsg))
	{
		// �ش� Ŭ���̾�Ʈ�� DB�� �����ϰ�, Password�� �´� �� Ȯ���Ѵ�.
		btRet = IsValidUserAndPassword((LPCTSTR)pMsg, (LPCTSTR)pMsg+sizeof(TCHAR)*13);
	}
	else btRet = FAIL_USER_ALREADY_LOGIN;

	// �޽��� �۽� ����~!!
	if(btRet == FAIL_SUCCESS)
	{
		// ������ ������, ������ ����Ѵ�.
		lstrcpy(pUserInfo->UserId, (LPCTSTR)pMsg);
		AddNameUserInfoMap(pUserInfo);

		// �α� ����� �����մϴ�.~!
		Msg[0] = DFOM_SC_LOGIN;
		Msg[1] = 1;
		Msg[2] = btRet;
		lstrcpyn((LPTSTR)Msg+3, (LPCTSTR)pMsg, 12);
		Send(Key, Msg, 3 + sizeof(TCHAR)*13);
#ifdef _DEBUG
//	LOGOUT("SERVER: LOG RESULT(%d, %d)", Msg[1], Msg[2]);
#endif

		// �� Ŭ���̾�Ʈ���� ���� �α�� ����� ������ �۽��մϴ�.
		SendLoggedUserInfos(Key);

		// �ٸ� Ŭ���̾�Ʈ���� �� Ŭ���̾�Ʈ�� �α��� �˸�.
		Msg[0] = DFOM_SC_LOGINUSERS;
		Msg[1] = 1;
		Msg[2] = 1;
		memcpy(Msg+3, &pUserInfo->dwIP, 4);
		lstrcpy((LPTSTR)Msg+7, (LPCTSTR)pMsg);

		Broadcast(Msg, 7 + sizeof(TCHAR)*13, pUserInfo->UserId);
		
	}
	else
	{
		// �α��� ���� �޽���..
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

// Ŭ���̾�Ʈ�� �α� �ƿ� �޽��� ���Ž� ó��.
void DFOpenMindCore::OnMsgLogout(void* /*Key*/, BYTE* /*pMsg*/, UINT /*nMsgSize*/)
{
	// �ϴ� �α׾ƿ� �޽����� �Ⱦ��� �ɷ� ����.!!
}

// Ŭ���̾�Ʈ�� ���ο� ���� ���� �޽��� ���Ž� ó��.
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

// Ŭ���̾�Ʈ�� �ڽ��� ���� ���� ����Ʈ �޽����� ���Ž� ó��.
void DFOpenMindCore::OnMsgSendInfos(void* /*Key*/, BYTE* pMsg, UINT nMsgSize)
{
	DWORD dwFileInfoCount = nMsgSize / sizeof(FILE_INFO);
	for(DWORD i = 0; i < dwFileInfoCount; i++)
	{
		m_FileDB.AddNewFileInfo((PFILE_INFO)(pMsg+i*sizeof(FILE_INFO)));
	}
}

// Ŭ���̾�Ʈ�� ���� �˻� �޽��� ���Ž� ó��.
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

	// �˻��� ����� ������ ���� �޽����� �۽��Ѵ�.
	Msg[0] = DFOM_SC_SEARCH_RESULTS;
	*(WORD*)(Msg+1) = (WORD)nCount;
	
	// �˻� ���н�.
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