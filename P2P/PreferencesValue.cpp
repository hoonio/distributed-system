// PreferencesValue.cpp: implementation of the CPreferencesValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P2p.h"
#include "PreferencesValue.h"

#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPreferencesValue::CPreferencesValue()
{
	/**
	m_nDownloadSpeed = 200;	// 다운로드 속도 (0~ 500)
	m_nUploadSpeed = 200;// 업로드 속도
	m_strFolderTemp = "";	// 임시폴더
	m_strFolderInComing = "";	// 다운로드폴더
	m_strFolderShare = "";	// 공유폴더
	m_bDeleteComplete = 0;	// 다운완료된거 삭제
	m_nPopupTime = 5;	// 팝업 메시지지연속도
	m_bPopupConnect = 1;	// 팝업 연결
	m_bPopupDisconnect = 1;	// 팝업 연결끊김
	m_bPopupCompleteDownload = 1;	// 팝업 다운완료
	**/
	
	// 생성자에서 값을 넣어준다, 여기서 처리를 하면 에러가 남 그래서 접속되는 시점에서 처리한다
	//ConnectLoadFile(&m_nDownloadSpeed, &m_nUploadSpeed);
	//DirectoryLoadFile(&m_strFolderTemp, &m_strFolderInComing, &m_strFolderShare);
	//DisplayLoadFile(&m_bDeleteComplete, &m_nPopupTime, &m_bPopupConnect, &m_bPopupDisconnect, &m_bPopupCompleteDownload);	
}

CPreferencesValue::~CPreferencesValue()
{

}





// CONNECT (SAVE)
void CPreferencesValue::ConnectSaveFile(UINT download, UINT upload)
{
	CString basePath = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;
	CString totalPath;
	totalPath.Format("%s%s", basePath, FILENAME_PREFERENCES);
	
	CString temp;
	temp.Format("%d", download);
	WritePrivateProfileString ("CONNECT", "nDownloadSpeed", temp, totalPath);
	m_nDownloadSpeed = download;

	temp.Format("%d", upload);
	WritePrivateProfileString ("CONNECT", "nUploadSpeed", temp, totalPath);
	m_nUploadSpeed = upload;	
}

// CONNECT (LOAD)
void CPreferencesValue::ConnectLoadFile(UINT *download, UINT *upload)
{
	CString basePath = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;
	CString totalPath;
	totalPath.Format("%s%s", basePath, FILENAME_PREFERENCES);
	
	CString temp;
	const UINT bufsize = 256;
	char buff[bufsize];	

	GetPrivateProfileString("CONNECT", "nDownloadSpeed", "200", buff, BUFSIZ, totalPath);
	*download = atoi(buff);
	m_nDownloadSpeed = *download;

	GetPrivateProfileString("CONNECT", "nUploadSpeed", "200", buff, BUFSIZ, totalPath);
	*upload = atoi(buff);	
	m_nUploadSpeed = *upload;
}


// DIRECTORY (SAVE)
void CPreferencesValue::DirectorySaveFile(CString *tempFolder, CString *inComingFolder, CString *shareFolder)
{
	CString basePath = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;
	CString totalPath;
	totalPath.Format("%s%s", basePath, FILENAME_PREFERENCES);
	
	CString temp;

	WritePrivateProfileString ("DIRECTORY", "strFolderTemp", *tempFolder, totalPath);
	m_strFolderTemp = *tempFolder;

	WritePrivateProfileString ("DIRECTORY", "strFolderInComing", *inComingFolder, totalPath);
	m_strFolderInComing = *inComingFolder;

	WritePrivateProfileString ("DIRECTORY", "strFolderShare", *shareFolder, totalPath);
	m_strFolderShare = *shareFolder;
}

// DIRECTORY (LOAD)
void CPreferencesValue::DirectoryLoadFile(CString *tempFolder, CString *inComingFolder, CString *shareFolder)
{
	CString basePath = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;
	CString totalPath;
	totalPath.Format("%s%s", basePath, FILENAME_PREFERENCES);

	CString temp;
	const UINT bufsize = 256;
	char buff[bufsize];	
	CString tempPath;

	tempPath.Format("%s%s", basePath, FOLDERNAME_TEMP);
	GetPrivateProfileString("DIRECTORY", "strFolderTemp", tempPath, buff, BUFSIZ, totalPath);
	tempFolder->Format("%s", buff);
	m_strFolderTemp = *tempFolder;

	tempPath.Format("%s%s", basePath, FOLDERNAME_INCOMING);
	GetPrivateProfileString("DIRECTORY", "strFolderInComing", tempPath, buff, BUFSIZ, totalPath);
	inComingFolder->Format("%s", buff);
	m_strFolderInComing = *inComingFolder;

	tempPath.Format("%s%s", basePath, FOLDERNAME_INCOMING);
	GetPrivateProfileString("DIRECTORY", "strFolderShare", tempPath, buff, BUFSIZ, totalPath);
	shareFolder->Format("%s", buff);
	m_strFolderShare = *shareFolder;	
}

// DISPLAY (SAVE)
void CPreferencesValue::DisplaySaveFile(BOOL bDeleteComplete, UINT nPopupTime, BOOL bPopupConnect, BOOL bPopupDisconnect, BOOL bPopupCompleteDownload)
{
	CString basePath = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;
	CString totalPath;
	totalPath.Format("%s%s", basePath, FILENAME_PREFERENCES);
	
	CString temp;
	temp.Format("%s", (bDeleteComplete == TRUE ? "TRUE" : "FALSE"));
	WritePrivateProfileString ("DISPLAY", "bDeleteComplete", temp, totalPath);
	m_bDeleteComplete = bDeleteComplete;

	temp.Format("%d", nPopupTime);
	WritePrivateProfileString ("DISPLAY", "nPopupTime", temp, totalPath);
	m_nPopupTime = nPopupTime;

	temp.Format("%s", (bPopupConnect == TRUE ? "TRUE" : "FALSE"));
	WritePrivateProfileString ("DISPLAY", "bPopupConnect", temp, totalPath);
	m_bPopupConnect = bPopupConnect;

	temp.Format("%s", (bPopupDisconnect == TRUE ? "TRUE" : "FALSE"));
	WritePrivateProfileString ("DISPLAY", "bPopupDisconnect", temp, totalPath);
	m_bPopupDisconnect = bPopupDisconnect;

	temp.Format("%s", (bPopupCompleteDownload == TRUE ? "TRUE" : "FALSE"));
	WritePrivateProfileString ("DISPLAY", "bPopupCompleteDownload", temp, totalPath);
	m_bPopupCompleteDownload = bPopupCompleteDownload;

	

}

// DISPLAY (LOAD)
void CPreferencesValue::DisplayLoadFile(BOOL *bDeleteComplete, UINT *nPopupTime, BOOL *bPopupConnect, BOOL *bPopupDisconnect, BOOL *bPopupCompleteDownload)
{
	CString basePath = ((CMainFrame *)AfxGetMainWnd())->m_strFolderPath;
	CString totalPath;
	totalPath.Format("%s%s", basePath, FILENAME_PREFERENCES);

	CString temp;
	const UINT bufsize = 256;
	char buff[bufsize];	
	GetPrivateProfileString("DISPLAY", "bDeleteComplete", "FALSE", buff, BUFSIZ, totalPath);
	*bDeleteComplete = (strcmp(buff, "TRUE") == 0 ? 1 : 0);	
	m_bDeleteComplete = *bDeleteComplete;

	GetPrivateProfileString("DISPLAY", "nPopupTime", "5", buff, BUFSIZ, totalPath);
	*nPopupTime = atoi(buff);
	m_nPopupTime = *nPopupTime;

	GetPrivateProfileString("DISPLAY", "bPopupConnect", "TRUE", buff, BUFSIZ, totalPath);
	*bPopupConnect = (strcmp(buff, "TRUE") == 0 ? 1 : 0);	
	m_bPopupConnect = *bPopupConnect;

	GetPrivateProfileString("DISPLAY", "bPopupDisconnect", "TRUE", buff, BUFSIZ, totalPath);
	*bPopupDisconnect = (strcmp(buff, "TRUE") == 0 ? 1 : 0);	
	m_bPopupDisconnect = *bPopupDisconnect;

	GetPrivateProfileString("DISPLAY", "bPopupCompleteDownload", "TRUE", buff, BUFSIZ, totalPath);
	*bPopupCompleteDownload = (strcmp(buff, "TRUE") == 0 ? 1 : 0);	
	m_bPopupCompleteDownload = *bPopupCompleteDownload;
}


