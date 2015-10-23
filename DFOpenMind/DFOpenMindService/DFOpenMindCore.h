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
#define CHECK_TIME		10000	// 10초에 한번씩 체크한다.
#define CHECK_COUNT		6		// 1분이 되면, 살아있는 지 문의.
#define NOT_ALIVE		9		// 1분 30초동안 수신된 내용이 없으면 죽인다.
#define MAX_MSGSIZE		4096
#define MAX_USERS		50;



// Structure for user information.
typedef struct
{
	void*		Key;			// 소켓 컨텍스트를 위한 키.
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
	DFMapPtrToPtr		m_KeyUserMap;		// 접속된 모든 유저들의 정보를 관리
	DFMapStringToPtr	m_NameUserMap;		// 역시!!

	DFMsgProcessor*		m_pMsgProcessor;	// 소켓 이벤트와 메시지를 처리
	DFServerSock*		m_pServerSock;		// 소켓 IO를 담당
	DFTimeChecker		m_TimeChecker;		// 타임 체크. 오랜동안 송신이 없는 클라이언트 제거.
	DFFileDB			m_FileDB;			// 클라이언트의 파일 정보들을 관리.
	DFUserDB*			m_pUserDB;			// 이용자 정보를 관리.

	// 소켓 이벤트 처리를 위한 함수들..
	void				OnReceive(void* Key);
	void				OnClose(void* Key);
	void				OnAccept(void* Key);

	void				CloseForcibly(void* Key);
	BOOL				GetRecvedMsg(void* Key, BYTE* MsgBuf, UINT* pMsgSize);
	void				SetRecvCountZero(void* Key);

	// 접속된 모든 유저를 관리하는 함수들.
	BOOL				IsLoggedUser(LPCTSTR tszUserID);
	USER_INFO*			FindUserInfoWithKey(void* Key);
	void				AddKeyUserInfoMap(void* Key, USER_INFO* pUserInfo);
	void				AddNameUserInfoMap(USER_INFO* pUserInfo);
	void				RemoveUserInfoMap(void* Key);
	DWORD				GetLoggedUserCount();
	void				GetLoggedUserNameArray(LPTSTR tszArray);

	// DB에서 유저의 ID와 PASSWORD가 일치하는 지 확인하는 함수.
	BYTE				IsValidUserAndPassword(LPCTSTR tszUsrID, LPCTSTR tszPwd);

	// 송신 관련된 함수들..
	BOOL				Send(void* Key, BYTE* Msg, UINT nMsgSize);	// 실제 메시지 송신하는 함수.
	void				Broadcast(BYTE* Msg, UINT nMsgSize, LPCTSTR tszUserID = NULL);

	void				BroadcastLogout(LPCTSTR tszUserID);
	void				BroadcastLogin(LPCTSTR tszUserID);
	void				SendAliveAsk(void* Key);
	void				SendLoginResponse(void* Key, LPCTSTR tszUserID, BYTE btSuccess, BYTE btErrCode);
	void				SendLoggedUserInfos(void* Key);

	// 수신된 메시지를 실제로 처리하는 함수들..
	void				OnMsgLogin(void* Key, BYTE* pMsg, UINT nMsgSize);
	void				OnMsgLogout(void* Key, BYTE* pMsg, UINT nMsgSize);
	void 				OnMsgNewUser(void* Key, BYTE* pMsg, UINT nMsgSize);
	void				OnMsgSendInfos(void* Key, BYTE* pMsg, UINT nMsgSize);
	void				OnMsgSearch(void* Key, BYTE* pMsg, UINT nMsgSize);
};


#endif