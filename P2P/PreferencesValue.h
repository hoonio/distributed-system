// PreferencesValue.h: interface for the CPreferencesValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCESVALUE_H__B8AC6076_B9A9_4682_9328_7385CF5FDED3__INCLUDED_)
#define AFX_PREFERENCESVALUE_H__B8AC6076_B9A9_4682_9328_7385CF5FDED3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPreferencesValue	// ȯ�漳�� ������
{
public:
	void DisplaySaveFile(BOOL bDeleteComplete, UINT nPopupTime, BOOL  bPopupConnect, BOOL bPopupDisconnect, BOOL bPopupComplete);
	void DisplayLoadFile(BOOL *bDeleteComplete, UINT *nPopupTime, BOOL *bPopupConnect, BOOL *bPopupDisconnect, BOOL *bPopupCompleteDownload);
	void DirectorySaveFile(CString *tempFolder, CString *inComingFolder, CString *shareFolder);
	void DirectoryLoadFile(CString *tempFolder, CString *inComingFolder, CString *shareFolder);
	void ConnectSaveFile(UINT download, UINT upload);
	void ConnectLoadFile(UINT *download, UINT *upload);
	UINT m_nDownloadSpeed;	// �ٿ�ε� �ӵ�
	UINT m_nUploadSpeed;	// ���ε� �ӵ�
	CString m_strFolderTemp;	// �ӽ�����
	CString m_strFolderInComing;	// �ٿ�ε�����
	CString m_strFolderShare;	// ��������
	BOOL m_bDeleteComplete;	// �ٿ�Ϸ�Ȱ� ����
	UINT m_nPopupTime;	// �˾� �޽��������ӵ�
	BOOL m_bPopupConnect;	// �˾� ����
	BOOL m_bPopupDisconnect;	// �˾� �������
	BOOL m_bPopupCompleteDownload;	// �˾� �ٿ�Ϸ�

	
	CPreferencesValue();
	virtual ~CPreferencesValue();

};

#endif // !defined(AFX_PREFERENCESVALUE_H__B8AC6076_B9A9_4682_9328_7385CF5FDED3__INCLUDED_)
