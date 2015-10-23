#ifndef __DFUSERDB_H__
#define __DFUSERDB_H__

#pragma once

#include "DFMapPtrToPtr.h"
#include "..\\DFOpenMindProtocol.h"

#define USERDB_FILE		TEXT("DFOMUser.db")

typedef struct
{
	TCHAR		tszUserID[13];
	TCHAR		tszPassword[9];
	TCHAR		tszAddress[61];
	TCHAR		tszPhone[17];
	TCHAR		tszEMail[41];
} USER_RECORD, *PUSER_RECORD;

// 유저 데이터베이스(file db)
class DFUserDB
{
public:
	DFUserDB();
	~DFUserDB();

	BOOL				CreateOrOpenDBFile();
	BOOL				CloseDBFile();
	BYTE				AddNewUser(PUSER_RECORD pUserRecord);
	BYTE				CheckUser(LPCTSTR tszUserID, LPCTSTR tszPwd);

private:
	DWORD				m_dwUserCount;
	HANDLE				m_hDBFile;
	DFMapStringToPtr	m_UserMap;
	CRITICAL_SECTION	m_CS;
};

#endif
