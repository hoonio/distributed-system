#ifndef __DFFILEDB_H__
#define __DFFILEDB_H__

#pragma once

#include "DFCmn.h"
#include "..\DFOpenMindProtocol.h"
#include "DFMapPtrToPtr.h"



// �� �̿����� ���� ������ ���� ������ ��� �ִ� �÷��� Ŭ����.
class DFFileDB
{
public:
	DFFileDB();
	~DFFileDB();

	void		AddNewFileInfo(PFILE_INFO pFileInfo);	// ���ο� ���� ������ �߰��Ѵ�.
	void		RemoveFileInfo(PFILE_INFO pFileInfo);	// ���� ������ �ϳ� �����Ѵ�.
	void		RemoveFileInfoAll(LPCTSTR tszUserID);	// �ش� ������ ��� ���� ������ �����Ѵ�.

	// ������ �����ϴ� ������ ���� ����ü�� �迭�� �����ϸ�, ����ü�� ������ ���Ѵ�.
	PFILE_INFO	FindFilesWithRequirement(PFILE_FIND_INFO pFindFileInfo, UINT nMaxItem, UINT& nCount);
private:
	BOOL		CheckFileWithRequirement(PFILE_FIND_INFO pFindFileInfo, PFILE_INFO2 pFileInfo2);

	CRITICAL_SECTION	m_CS;
	DFMapStringToPtr	m_FileWithUserMap;	// ���� ID�� Ű�� ��� ���� ���� ��.
	DFMapStringToPtr	m_FileMap;			// ���� ID + ���� �̸��� Ű�� ��´�.
};

#endif
