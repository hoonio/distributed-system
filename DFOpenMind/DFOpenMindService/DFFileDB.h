#ifndef __DFFILEDB_H__
#define __DFFILEDB_H__

#pragma once

#include "DFCmn.h"
#include "..\DFOpenMindProtocol.h"
#include "DFMapPtrToPtr.h"



// 각 이용자의 공유 폴더의 파일 정보를 담고 있는 컬렉션 클래스.
class DFFileDB
{
public:
	DFFileDB();
	~DFFileDB();

	void		AddNewFileInfo(PFILE_INFO pFileInfo);	// 새로운 파일 정보를 추가한다.
	void		RemoveFileInfo(PFILE_INFO pFileInfo);	// 파일 정보를 하나 제거한다.
	void		RemoveFileInfoAll(LPCTSTR tszUserID);	// 해당 유저의 모든 파일 정보를 제거한다.

	// 조건을 만족하는 파일의 정보 구조체의 배열을 리턴하며, 구조체의 갯수를 구한다.
	PFILE_INFO	FindFilesWithRequirement(PFILE_FIND_INFO pFindFileInfo, UINT nMaxItem, UINT& nCount);
private:
	BOOL		CheckFileWithRequirement(PFILE_FIND_INFO pFindFileInfo, PFILE_INFO2 pFileInfo2);

	CRITICAL_SECTION	m_CS;
	DFMapStringToPtr	m_FileWithUserMap;	// 유저 ID로 키를 삼는 파일 맵의 맵.
	DFMapStringToPtr	m_FileMap;			// 유저 ID + 파일 이름을 키로 삼는다.
};

#endif
