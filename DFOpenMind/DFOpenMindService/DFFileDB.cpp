#include "stdafx.h"
#include "DFFileDB.h"
#include <stdio.h>
#include "string.h"
#include "TCHAR.H"

//////////////////////////////////////////////////////////////////////////////////////////////////
DFFileDB::DFFileDB()
{
	InitializeCriticalSection(&m_CS);
}

DFFileDB::~DFFileDB()
{
	DeleteCriticalSection(&m_CS);
}

void DFFileDB::AddNewFileInfo(PFILE_INFO pFileInfo)
{
	DFMapStringToPtr*	pFileInfoMap = NULL;

	// 서버에서 관리하는 구조체로 변경한다.
	PFILE_INFO2	pFileInfo2 = new FILE_INFO2;
	wsprintf(pFileInfo2->tszFileName, TEXT("%s\\\\%s"), pFileInfo->tszUserID, pFileInfo->tszFileName);
//	lstrcpy(pFileInfo2->tszAlternateFileName, pFileInfo->tszAlternateFileName);
	pFileInfo2->ftLastWriteTime = pFileInfo->ftLastWriteTime;
	pFileInfo2->ftLastAccessTime= pFileInfo->ftLastAccessTime;
	pFileInfo2->ftCreationTime	= pFileInfo->ftCreationTime;
	pFileInfo2->dwFileSizeLow	= pFileInfo->dwFileSizeLow;
	pFileInfo2->dwFileSizeHigh	= pFileInfo->dwFileSizeHigh;

	EnterCriticalSection(&m_CS);

	m_FileMap.SetAt(pFileInfo2->tszFileName, (void*)pFileInfo2);

	m_FileWithUserMap.Lookup(pFileInfo->tszUserID, (void*&)pFileInfoMap);
	if(!pFileInfoMap)
	{
		pFileInfoMap = new DFMapStringToPtr;
		m_FileWithUserMap.SetAt(pFileInfo->tszUserID, (void*)pFileInfoMap);
	}

	pFileInfoMap->SetAt(pFileInfo->tszFileName, (void*)pFileInfo2);

	LeaveCriticalSection(&m_CS);
}

void DFFileDB::RemoveFileInfo(PFILE_INFO pFileInfo)
{
	DFMapStringToPtr*	pFileInfoMap = NULL;
	TCHAR				tszFileName[MAX_PATH+USER_ID_LENGTH+3];
	PFILE_INFO2			pFileInfo2 = NULL;

	wsprintf(tszFileName, TEXT("%s\\\\%s"), pFileInfo->tszUserID, pFileInfo->tszFileName);

	EnterCriticalSection(&m_CS);

	m_FileMap.Lookup(tszFileName, (void*&)pFileInfo2);
	if(pFileInfo2)
	{
		m_FileMap.RemoveKey(tszFileName);
		delete pFileInfo2;
	}

	m_FileWithUserMap.Lookup(pFileInfo->tszUserID, (void*&)pFileInfoMap);
	if(!pFileInfoMap) return;

	pFileInfoMap->RemoveKey(pFileInfo->tszFileName);
	if(pFileInfoMap->GetCount() == 0)
	{
		m_FileWithUserMap.RemoveKey(pFileInfo->tszUserID);
		delete pFileInfoMap;
	}

	LeaveCriticalSection(&m_CS);
}

void DFFileDB::RemoveFileInfoAll(LPCTSTR tszUserID)
{
	DFMapStringToPtr*	pFileInfoMap = NULL;
	PFILE_INFO2			pFileInfo2 = NULL;
	TCHAR				tszKey[255];
	TCHAR				tszTemp[MAX_PATH+USER_ID_LENGTH+3];

	DFASSERT(tszUserID != NULL);

	EnterCriticalSection(&m_CS);

	wsprintf(tszTemp, TEXT("%s\\\\"), tszUserID);

	POSITION pos = m_FileMap.GetStartPosition();
	while(pos)
	{
		m_FileMap.GetNextAssoc(pos, (LPTSTR&)tszKey, (void*&) pFileInfo2);
		if(pFileInfo2)
		{
			if(_tcsnicmp(pFileInfo2->tszFileName, tszTemp, lstrlen(tszTemp)) == 0)
			{
				m_FileMap.RemoveKey(pFileInfo2->tszFileName);
				delete pFileInfo2;
			}
		}
	}

	m_FileWithUserMap.Lookup(tszUserID, (void*&)pFileInfoMap);
	if(!pFileInfoMap) return;

	pFileInfoMap->RemoveAll();

	m_FileWithUserMap.RemoveKey(tszUserID);
	delete pFileInfoMap;

	LeaveCriticalSection(&m_CS);
}

PFILE_INFO DFFileDB::FindFilesWithRequirement(PFILE_FIND_INFO pFindFileInfo, UINT nMaxItem, UINT& nCount)
{
	TCHAR				tszKey[255];
	DFMapStringToPtr*	pFileInfoMap = NULL;
	PFILE_INFO2			pFileInfo2 = NULL;
	FILE_INFO			FileInfo;
	POSITION			pos = NULL;
	TCHAR*				pOc = NULL;

	DFASSERT(nMaxItem < 1000);

	PFILE_INFO pFileInfoArray = new FILE_INFO[nMaxItem];

	EnterCriticalSection(&m_CS);

#ifdef _DEBUG
//	LOGOUT("SERVER: SEARCH METHOD(%d)", SEARCH_USER_ID(pFindFileInfo->Methods));
#endif
	// 유저 ID로 검색하는 조건이 들어간 경우,
	if(SEARCH_USER_ID(pFindFileInfo->Methods))
	{
		m_FileWithUserMap.Lookup(pFindFileInfo->tszUserID, (void*&) pFileInfoMap);
		if(pFileInfoMap)
		{
			pos = pFileInfoMap->GetStartPosition();
			while(pos)
			{
				pFileInfoMap->GetNextAssoc(pos, (LPTSTR&)tszKey, (void*&) pFileInfo2);
				if(pFileInfo2)
				{
					if(CheckFileWithRequirement(pFindFileInfo, pFileInfo2))
					{
						pOc = _tcsstr(pFileInfo2->tszFileName, TEXT("\\\\"));
						DFASSERT(pOc != NULL);
						int nLen = pOc - pFileInfo2->tszFileName + 1;
						
						lstrcpy(FileInfo.tszFileName, pOc+2);
						lstrcpyn(FileInfo.tszUserID, pFileInfo2->tszFileName, nLen);
						FileInfo.ftLastWriteTime	= pFileInfo2->ftLastWriteTime;
						FileInfo.ftLastAccessTime	= pFileInfo2->ftLastAccessTime;
						FileInfo.ftCreationTime		= pFileInfo2->ftCreationTime;
						FileInfo.dwFileSizeLow		= pFileInfo2->dwFileSizeLow;
						FileInfo.dwFileSizeHigh		= pFileInfo2->dwFileSizeHigh;

						// 리턴할 자료를 채워넣는다.
						pFileInfoArray[nCount++] = FileInfo;
						if(nCount == nMaxItem) break;
					}
				}
			}
		}
	}
	// 유저 ID로 검색하는 조건이 없는 경우.
	else
	{
		pos = m_FileMap.GetStartPosition();
		while(pos)
		{
			m_FileMap.GetNextAssoc(pos, (LPTSTR&)tszKey, (void*&) pFileInfo2);
			if(pFileInfo2)
			{
				if(CheckFileWithRequirement(pFindFileInfo, pFileInfo2))
				{
					pOc = _tcsstr(pFileInfo2->tszFileName, TEXT("\\\\"));
					DFASSERT(pOc != NULL);
					int nLen = pOc - pFileInfo2->tszFileName + 1;
					
					lstrcpy(FileInfo.tszFileName, pOc+2);
					lstrcpyn(FileInfo.tszUserID, pFileInfo2->tszFileName, nLen);
					FileInfo.ftLastWriteTime	= pFileInfo2->ftLastWriteTime;
					FileInfo.ftLastAccessTime	= pFileInfo2->ftLastAccessTime;
					FileInfo.ftCreationTime		= pFileInfo2->ftCreationTime;
					FileInfo.dwFileSizeLow		= pFileInfo2->dwFileSizeLow;
					FileInfo.dwFileSizeHigh		= pFileInfo2->dwFileSizeHigh;

					// 리턴할 자료를 채워넣는다.
					pFileInfoArray[nCount++] = FileInfo;
					if(nCount == nMaxItem) break;
				}
			}
		}
	}

	LeaveCriticalSection(&m_CS);

	if(nCount == 0)
	{
		delete [] pFileInfoArray;
		return NULL;
	}
	else
	{
		return pFileInfoArray;
	}
}

inline BOOL DFFileDB::CheckFileWithRequirement(PFILE_FIND_INFO pFindFileInfo, PFILE_INFO2 pFileInfo2)
{
	// 파일 정보와 찾기 정보가 일치하는 지 확인한다.

//	if(SEARCH_USER_ID(pFindFileInfo->Methods))
//	{
//
//	}
	if(SEARCH_FILE_NAME(pFindFileInfo->Methods))
	{
		// 해당 문자열이 파일 이름에 존재하는 지 검사.
		TCHAR* pTemp = _tcsstr(pFileInfo2->tszFileName, pFindFileInfo->tszFileNamePortion);
		if(pTemp == NULL) return FALSE;
	}
	if(SEARCH_FILE_EXTENSION(pFindFileInfo->Methods))
	{
		int nLen = lstrlen(pFileInfo2->tszFileName);
		if(0 != lstrcmpi(pFileInfo2->tszFileName + nLen - 3, pFindFileInfo->tszExtention))
			return FALSE;
	}
	if(SEARCH_SIZE(pFindFileInfo->Methods))
	{
		if(VALID_MIN_SIZE(pFindFileInfo->Methods))
		{
			if(	(pFindFileInfo->dwMinFileSizeHigh > pFileInfo2->dwFileSizeHigh) ||
				(	(pFindFileInfo->dwMinFileSizeHigh == pFileInfo2->dwFileSizeHigh) &&
					(pFindFileInfo->dwMinFileSizeLow > pFileInfo2->dwFileSizeLow)))
					return FALSE;
		}
		if(VALID_MAX_SIZE(pFindFileInfo->Methods))
		{
			if(	(pFindFileInfo->dwMaxFileSizeHigh < pFileInfo2->dwFileSizeHigh) ||
				(	(pFindFileInfo->dwMaxFileSizeHigh == pFileInfo2->dwFileSizeHigh) &&
					(pFindFileInfo->dwMaxFileSizeLow < pFileInfo2->dwFileSizeLow)))
					return FALSE;
		}
	}
	if(SEARCH_FILE_TIME(pFindFileInfo->Methods))
	{
		FILETIME	ftTime;				

		if(SEARCH_CREATION_TIME(pFindFileInfo->Methods))
		{
			ftTime = pFileInfo2->ftCreationTime;
		}
		else if(SEARCH_ACCESS_TIME(pFindFileInfo->Methods))
		{
			ftTime = pFileInfo2->ftLastAccessTime;
		}
		else if(SEARCH_MODIFY_TIME(pFindFileInfo->Methods))
		{
			ftTime = pFileInfo2->ftLastWriteTime;
		}
		else return FALSE;

		if(VALID_MIN_TIME(pFindFileInfo->Methods))
		{
			if(CompareFileTime(&ftTime, &pFindFileInfo->ftMinTime) != 1)
				return FALSE;
		}
		if(VALID_MAX_TIME(pFindFileInfo->Methods))
		{
			if(CompareFileTime(&ftTime, &pFindFileInfo->ftMaxTime) != -1)
				return FALSE;
		}
	}

	return TRUE;
}
