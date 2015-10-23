// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__DD06A073_DB4B_4FFD_ABC1_0EBCD2C20CF4__INCLUDED_)
#define AFX_MAINFRM_H__DD06A073_DB4B_4FFD_ABC1_0EBCD2C20CF4__INCLUDED_

#include "TrayIcon.h"	// Ʈ���̾�����
#include "TaskbarNotifier.h"	// �˸�â(MSN)
#include "MMStatusBar.h"	// status ������
#include "PreferencesValue.h"	// Added by ClassView
#include "MyIOCPSocket.h"	// Added by ClassView
#include "SearchView.h"	// Added by ClassView
#include "DownloadsView.h"	// Added by ClassView
#include "DataDownload.h"	// Added by ClassView
#include "MsnDlg.h"	// ä��

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
	CView *m_pNewView;	// ��
	UINT m_nViewNum;	// ������ȣ
	// �ü���������ϴ� �����ܾ��
	void GetSysImgList(CListCtrl &listCtrl, CImageList &imageList);

	// �ٿ�ε�, ���ε� ����� ���Ͽ� ����, �ε�
	CDataDownload m_cDownloadInfo;	// �ٿ�ε����� ��������
	CDataDownload m_cUploadInfo;	// ���ε����� ��������
	CArchive *m_pARead;
	CArchive *m_pAWrite;
	CFile *m_pFile;
	void LoadFile(BOOL nFlag, CDataDownload &cData);	// 1�̸� �ٿ�ε�0�̸� ���ε�
	void SaveFile(BOOL nFlag, CDataDownload &cData);	// 1�̸� �ٿ�ε�0�̸� ���ε�
	CList<MY_DOWNLOAD_FILE_LIST, MY_DOWNLOAD_FILE_LIST&> m_downloadFileList;	// �ٿ�ε���(�ٸ��並 �����ϸ� ����Ÿ ������ �������Ƿ� ���⼭ ����)
	CList<MY_DOWNLOAD_FILE_LIST, MY_DOWNLOAD_FILE_LIST&> m_uploadFileList;	// �����(�ٸ��並 �����ϸ� ����Ÿ ������ �������Ƿ� ���⼭ ����)
	UINT m_nDownloadMaxIndex;	// �����ε�����ȣ(�ٿ�ε��ϰ� �����ٿ��������Ϥ����� ����) �̹�ȣ�� �����ѹ�ȣ�� ����ū��ȣ
	UINT m_nUploadMaxIndex;	// �����ε�����ȣ(���ε��ϰ� �������ε��������Ϥ����� ����) �̹�ȣ�� �����ѹ�ȣ�� ����ū��ȣ

	MY_SEARCH_FILE_DETAIL_LIST m_searchListData[MAX_SEARCH_COUNT];	// �˻�Ŭ�������� ���(�˻��ѳ����� �ٸ��並 �����ϸ� ����Ÿ ������ �������Ƿ� ���⼭ ����)
	CString ChangeComma(UINT);
	CMyIOCPSocket m_myIOCPSocket;	// IOCP ����
	CPreferencesValue m_cPreferencesValue;	// ȯ�漳�� ������
	CString m_strFolderPath;	// ���α׷� �������
	BOOL m_bIsFirst;	// ���α׷��� ó������Ǵ°�����.
	HICON m_hTooltipIcon[5];	// status ������
	void ShowView(UINT no);
	BOOL SetCheckToolbar(UINT uID);	// ���ٹ�ư ������ȿ��
	BOOL m_bConnectedFlag;	// ����������(1�̸� ����)
	CTrayIcon m_trayIcon;	// Ʈ���̾�����
	
	CMsnDlg *m_pMsnDlg;	// ä���ϴ� ���̾�α�

	CTaskbarNotifier m_wndTaskbarNotifier;	// �˸�â(MSN)
	virtual ~CMainFrame();

	//CStatusBar  m_wndStatusBar;	// ��������� Ŭ������ �������
	MMStatusBar  m_wndStatusBar;	// ��������� status
	
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
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);	// Ʈ���̾�����
	afx_msg void OnMenuConnectUpdate(CCmdUI *pCmdUI);
	afx_msg LRESULT OnTaskbarNotifierClicked(WPARAM wParam,LPARAM lParam);	// �˸�â(MSN)
	afx_msg LRESULT OnSetPaneText(WPARAM wParam, LPARAM lParam);	// ������������ statusbar ����
	afx_msg LRESULT OnPopupMessage(WPARAM wParam, LPARAM lParam);	// �˾��޽���
	afx_msg LRESULT OnAddItemSearchFile(WPARAM wParam, LPARAM lParam);	// IOCP���� ã�����������⼭ó��
	afx_msg LRESULT OnDownloadComplete(WPARAM wParam, LPARAM lParam);	// �ٿ�Ϸ�
	afx_msg LRESULT OnUploadComplete(WPARAM wParam, LPARAM lParam);	// ���ε�Ϸ�
	afx_msg LRESULT OnAddUploadData(WPARAM wParam, LPARAM lParam);	// ���ε嵥��Ÿ�߰�
	afx_msg LRESULT OnAddDownloadData(WPARAM wParam, LPARAM lParam);	// �ٿ�ε嵥��Ÿ�߰�
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__DD06A073_DB4B_4FFD_ABC1_0EBCD2C20CF4__INCLUDED_)
