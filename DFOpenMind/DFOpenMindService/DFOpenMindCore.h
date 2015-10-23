#ifndef __DFOPENMINDCORE_H__
#define __DFOPENMINDCORE_H__

#pragma once

#include "DFIOCP.h"
#include "DFMapPtrToPtr.h"
#include "DFDispatcher.h"
#include "DFTimeChecker.h"
#include "DFFileDB.h"
#include "DFUserDB.H"



#define	SERVICE_PORT	7777
#define CHECK_TIME		10000	// 10�ʿ� �ѹ��� üũ�Ѵ�.
#define CHECK_COUNT		6		// 1���� �Ǹ�, ����ִ� �� ����.
#define NOT_ALIVE		9		// 1�� 30�ʵ��� ���ŵ� ������ ������ ���δ�.
#define MAX_MSGSIZE		4096
#define MAX_USERS		50;



// Structure for user information.
typedef struct
{
	void*		Key;			// ���� ���ؽ�Ʈ�� ���� Ű.
	TCHAR		UserId[USER_ID_LENGTH+1];
	UINT		nNotRecvedCount;
	DWORD		dwIP;
} USER_INFO;



// main class for DFOpenMind Service
class DFOpenMindCore
{
public:
	DFOpenMindCore();
	~DFOpenMindCore();

	BOOL				StartService();
	void				StopService();

	void				ProcessMessage(void* Key, DWORD EventType);
	void				CheckAndKillConnection();

private:
	BOOL				m_bInService;
	DFIOCP				m_IOCP;
	BYTE				m_btCurrentUsers;
	
	CRITICAL_SECTION	m_CS;
	DFMapPtrToPtr		m_KeyUserMap;		// ���ӵ� ��� �������� ������ ����
	DFMapStringToPtr	m_NameUserMap;		// ����!!

	DFMsgProcessor*		m_pMsgProcessor;	// ���� �̺�Ʈ�� �޽����� ó��
	DFServerSock*		m_pServerSock;		// ���� IO�� ���
	DFTimeChecker		m_TimeChecker;		// Ÿ�� üũ. �������� �۽��� ���� Ŭ���̾�Ʈ ����.
	DFFileDB			m_FileDB;			// Ŭ���̾�Ʈ�� ���� �������� ����.
	DFUserDB*			m_pUserDB;			// �̿��� ������ ����.

	// ���� �̺�Ʈ ó���� ���� �Լ���..
	void				OnReceive(void* Key);
	void				OnClose(void* Key);
	void				OnAccept(void* Key);

	void				CloseForcibly(void* Key);
	BOOL				GetRecvedMsg(void* Key, BYTE* MsgBuf, UINT* pMsgSize);
	void				SetRecvCountZero(void* Key);

	// ���ӵ� ��� ������ �����ϴ� �Լ���.
	BOOL				IsLoggedUser(LPCTSTR tszUserID);
	USER_INFO*			FindUserInfoWithKey(void* Key);
	void				AddKeyUserInfoMap(void* Key, USER_INFO* pUserInfo);
	void				AddNameUserInfoMap(USER_INFO* pUserInfo);
	void				RemoveUserInfoMap(void* Key);
	DWORD				GetLoggedUserCount();
	void				GetLoggedUserNameArray(LPTSTR tszArray);

	// DB���� ������ ID�� PASSWORD�� ��ġ�ϴ� �� Ȯ���ϴ� �Լ�.
	BYTE				IsValidUserAndPassword(LPCTSTR tszUsrID, LPCTSTR tszPwd);

	// �۽� ���õ� �Լ���..
	BOOL				Send(void* Key, BYTE* Msg, UINT nMsgSize);	// ���� �޽��� �۽��ϴ� �Լ�.
	void				Broadcast(BYTE* Msg, UINT nMsgSize, LPCTSTR tszUserID = NULL);

	void				BroadcastLogout(LPCTSTR tszUserID);
	void				BroadcastLogin(LPCTSTR tszUserID);
	void				SendAliveAsk(void* Key);
	void				SendLoginResponse(void* Key, LPCTSTR tszUserID, BYTE btSuccess, BYTE btErrCode);
	void				SendLoggedUserInfos(void* Key);

	// ���ŵ� �޽����� ������ ó���ϴ� �Լ���..
	void				OnMsgLogin(void* Key, BYTE* pMsg, UINT nMsgSize);
	void				OnMsgLogout(void* Key, BYTE* pMsg, UINT nMsgSize);
	void 				OnMsgNewUser(void* Key, BYTE* pMsg, UINT nMsgSize);
	void				OnMsgSendInfos(void* Key, BYTE* pMsg, UINT nMsgSize);
	void				OnMsgSearch(void* Key, BYTE* pMsg, UINT nMsgSize);
};


#endif