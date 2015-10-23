// PreferencesValue.h: interface for the CPreferencesValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCESVALUE_H__B8AC6076_B9A9_4682_9328_7385CF5FDED3__INCLUDED_)
#define AFX_PREFERENCESVALUE_H__B8AC6076_B9A9_4682_9328_7385CF5FDED3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPreferencesValue	// 환경설정 값내용
{
public:
	void DisplaySaveFile(BOOL bDeleteComplete, UINT nPopupTime, BOOL  bPopupConnect, BOOL bPopupDisconnect, BOOL bPopupComplete);
	void DisplayLoadFile(BOOL *bDeleteComplete, UINT *nPopupTime, BOOL *bPopupConnect, BOOL *bPopupDisconnect, BOOL *bPopupCompleteDownload);
	void DirectorySaveFile(CString *tempFolder, CString *inComingFolder, CString *shareFolder);
	void DirectoryLoadFile(CString *tempFolder, CString *inComingFolder, CString *shareFolder);
	void ConnectSaveFile(UINT download, UINT upload);
	void ConnectLoadFile(UINT *download, UINT *upload);
	UINT m_nDownloadSpeed;	// 다운로드 속도
	UINT m_nUploadSpeed;	// 업로드 속도
	CString m_strFolderTemp;	// 임시폴더
	CString m_strFolderInComing;	// 다운로드폴더
	CString m_strFolderShare;	// 공유폴더
	BOOL m_bDeleteComplete;	// 다운완료된거 삭제
	UINT m_nPopupTime;	// 팝업 메시지지연속도
	BOOL m_bPopupConnect;	// 팝업 연결
	BOOL m_bPopupDisconnect;	// 팝업 연결끊김
	BOOL m_bPopupCompleteDownload;	// 팝업 다운완료

	
	CPreferencesValue();
	virtual ~CPreferencesValue();

};

#endif // !defined(AFX_PREFERENCESVALUE_H__B8AC6076_B9A9_4682_9328_7385CF5FDED3__INCLUDED_)
