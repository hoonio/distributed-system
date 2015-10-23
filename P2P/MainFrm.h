// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__DD06A073_DB4B_4FFD_ABC1_0EBCD2C20CF4__INCLUDED_)
#define AFX_MAINFRM_H__DD06A073_DB4B_4FFD_ABC1_0EBCD2C20CF4__INCLUDED_

#include "TrayIcon.h"	// 트레이아이콘
#include "TaskbarNotifier.h"	// 알림창(MSN)
#include "MMStatusBar.h"	// status 아이콘
#include "PreferencesValue.h"	// Added by ClassView
#include "MyIOCPSocket.h"	// Added by ClassView
#include "SearchView.h"	// Added by ClassView
#include "DownloadsView.h"	// Added by ClassView
#include "DataDownload.h"	// Added by ClassView
#include "MsnDlg.h"	// 채팅

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations	
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:	
	CView *m_pNewView;	// 뷰
	UINT m_nViewNum;	// 현재뷰번호
	// 운영체제가관리하는 아이콘얻기
	void GetSysImgList(CListCtrl &listCtrl, CImageList &imageList);

	// 다운로드, 업로드 목록을 파일에 저장, 로드
	CDataDownload m_cDownloadInfo;	// 다운로드목록을 파일저장
	CDataDownload m_cUploadInfo;	// 업로드목록을 파일저장
	CArchive *m_pARead;
	CArchive *m_pAWrite;
	CFile *m_pFile;
	void LoadFile(BOOL nFlag, CDataDownload &cData);	// 1이면 다운로드0이면 업로드
	void SaveFile(BOOL nFlag, CDataDownload &cData);	// 1이면 다운로드0이면 업로드
	CList<MY_DOWNLOAD_FILE_LIST, MY_DOWNLOAD_FILE_LIST&> m_downloadFileList;	// 다운로드목록(다른뷰를 선택하면 데이타 내용이 없어지므로 여기서 관리)
	CList<MY_DOWNLOAD_FILE_LIST, MY_DOWNLOAD_FILE_LIST&> m_uploadFileList;	// 업목록(다른뷰를 선택하면 데이타 내용이 없어지므로 여기서 관리)
	UINT m_nDownloadMaxIndex;	// 다음인덱스번호(다운로드목록과 실제다운중인파일ㅇ과의 연동) 이번호는 고유한번호로 가장큰번호
	UINT m_nUploadMaxIndex;	// 다음인덱스번호(업로드목록과 실제업로드중인파일ㅇ과의 연동) 이번호는 고유한번호로 가장큰번호

	MY_SEARCH_FILE_DETAIL_LIST m_searchListData[MAX_SEARCH_COUNT];	// 검색클래스에서 사용(검색한내용이 다른뷰를 선택하면 데이타 내용이 없어지므로 여기서 관리)
	CString ChangeComma(UINT);
	CMyIOCPSocket m_myIOCPSocket;	// IOCP 소켓
	CPreferencesValue m_cPreferencesValue;	// 환경설정 값내용
	CString m_strFolderPath;	// 프로그램 폴더경로
	BOOL m_bIsFirst;	// 프로그램이 처음실행되는것인지.
	HICON m_hTooltipIcon[5];	// status 아이콘
	void ShowView(UINT no);
	BOOL SetCheckToolbar(UINT uID);	// 툴바버튼 눌러진효과
	BOOL m_bConnectedFlag;	// 접속중인지(1이면 접속)
	CTrayIcon m_trayIcon;	// 트레이아이콘
	
	CMsnDlg *m_pMsnDlg;	// 채팅하는 다이얼로그

	CTaskbarNotifier m_wndTaskbarNotifier;	// 알림창(MSN)
	virtual ~CMainFrame();

	//CStatusBar  m_wndStatusBar;	// 사용자정의 클래스로 만들었음
	MMStatusBar  m_wndStatusBar;	// 사용자정의 status
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar    m_wndToolBar;

// Generated message map functions

protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnServerConnect();
	afx_msg void OnClose();
	afx_msg void OnMenuSearch();
	afx_msg void OnMenuServer();
	afx_msg void OnMenuUploads();
	afx_msg void OnMenuDownloads();
	afx_msg void OnMenuChat();
	afx_msg void OnMenuPreferences();
	afx_msg void OnAppExit();
	afx_msg void OnDestroy();
	afx_msg void OnMenuServerConnect();
	afx_msg void OnMenuServerDisconnect();
	//}}AFX_MSG
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);	// 트레이아이콘
	afx_msg void OnMenuConnectUpdate(CCmdUI *pCmdUI);
	afx_msg LRESULT OnTaskbarNotifierClicked(WPARAM wParam,LPARAM lParam);	// 알림창(MSN)
	afx_msg LRESULT OnSetPaneText(WPARAM wParam, LPARAM lParam);	// 메인프레임의 statusbar 수정
	afx_msg LRESULT OnPopupMessage(WPARAM wParam, LPARAM lParam);	// 팝업메시지
	afx_msg LRESULT OnAddItemSearchFile(WPARAM wParam, LPARAM lParam);	// IOCP에서 찾은파일을여기서처리
	afx_msg LRESULT OnDownloadComplete(WPARAM wParam, LPARAM lParam);	// 다운완료
	afx_msg LRESULT OnUploadComplete(WPARAM wParam, LPARAM lParam);	// 업로드완료
	afx_msg LRESULT OnAddUploadData(WPARAM wParam, LPARAM lParam);	// 업로드데이타추가
	afx_msg LRESULT OnAddDownloadData(WPARAM wParam, LPARAM lParam);	// 다운로드데이타추가
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__DD06A073_DB4B_4FFD_ABC1_0EBCD2C20CF4__INCLUDED_)
