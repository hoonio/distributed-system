#include "Stdafx.h"
#include "DFUserDB.h"

DFUserDB::DFUserDB()
{
	m_dwUserCount = 0;
	m_hDBFile = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&m_CS);
}

DFUserDB::~DFUserDB()
{
	if(m_hDBFile) CloseDBFile();
	DeleteCriticalSection(&m_CS);
}

BOOL DFUserDB::CreateOrOpenDBFile()
{
	m_hDBFile = CreateFile(	USERDB_FILE, 
							GENERIC_READ|GENERIC_WRITE,
							0, 
							NULL, 
							OPEN_ALWAYS, 
							FILE_ATTRIBUTE_HIDDEN, 
							NULL);

	if(m_hDBFile == INVALID_HANDLE_VALUE) return FALSE;

	DWORD dwFileSize = GetFileSize(m_hDBFile, NULL);
	
	// 유저 레코드의 수를 읽어온다.
	m_dwUserCount = dwFileSize / sizeof(USER_RECORD);

	DWORD dwBytesRead = 0;
	// 각 유저 레코드를 메모리에 로드 한다.
	for(DWORD i = 0; i < m_dwUserCount; i++)
	{
		PUSER_RECORD pUserRecord = new USER_RECORD;
		if(!ReadFile(m_hDBFile, pUserRecord, sizeof(USER_RECORD), &dwBytesRead, NULL))
			return FALSE;

		m_UserMap.SetAt(pUserRecord->tszUserID, pUserRecord);
	}

	return TRUE;
}

BOOL DFUserDB::CloseDBFile()
{
	TCHAR	Key[255];
	PUSER_RECORD pUserRecord;
	DWORD	dwBytesWritten = 0;

	POSITION pos = m_UserMap.GetStartPosition();
	while(pos)
	{
		m_UserMap.GetNextAssoc(pos, (LPTSTR&)Key, (void*&)pUserRecord);
		if(Key && pUserRecord)
		{
			if(!WriteFile(m_hDBFile, pUserRecord, sizeof(USER_RECORD), &dwBytesWritten, NULL))
				return FALSE;
			delete pUserRecord;
		}
	}

	m_UserMap.RemoveAll();

	return TRUE;
}

BYTE DFUserDB::AddNewUser(PUSER_RECORD pUserRecord)
{
	void* Value;
	BYTE btRet;

	EnterCriticalSection(&m_CS);

	if(m_UserMap.Lookup(pUserRecord->tszUserID, (void*&) Value)) 
	{
		btRet = FAIL_ID_ALREADY_EXIST;
	}
	else
	{
		m_UserMap[pUserRecord->tszUserID]  = pUserRecord;
		m_dwUserCount++;
		btRet = FAIL_SUCCESS;
	}

	LeaveCriticalSection(&m_CS);
	
	return btRet;
}

BYTE DFUserDB::CheckUser(LPCTSTR tszUserID, LPCTSTR tszPwd)
{
	PUSER_RECORD pUserRecord;
	BYTE  btRet;

	ASSERT(lstrlen(tszUserID) != 0);
	EnterCriticalSection(&m_CS);

	if(!m_UserMap.Lookup(tszUserID, (void*&) pUserRecord)) 
	{
		btRet = FAIL_ID_NOT_EXIST;
	}
	else if(lstrcmp(tszPwd, pUserRecord->tszPassword) != 0)
	{
		btRet = FAIL_PASSWORD_MISMATCH;
	}
	else btRet = FAIL_SUCCESS;

	LeaveCriticalSection(&m_CS);

	return btRet;
}