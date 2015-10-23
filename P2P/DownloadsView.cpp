// DownloadsView.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "DownloadsView.h"

#include "MainFrm.h"
#include <cmath>	// ceil

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownloadsView

IMPLEMENT_DYNCREATE(CDownloadsView, CFormView)

CDownloadsView::CDownloadsView()
	: CFormView(CDownloadsView::IDD)
{
	//{{AFX_DATA_INIT(CDownloadsView)
	//}}AFX_DATA_INIT
}

CDownloadsView::~CDownloadsView()
{
}

void CDownloadsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadsView)
	DDX_Control(pDX, IDC_LIST_DOWNLOADS, m_listDownloads);
	DDX_Control(pDX, IDC_STATIC_STATE, m_staticState);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownloadsView, CFormView)
	//{{AFX_MSG_MAP(CDownloadsView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DOWNLOADS, OnRclickListDownloads)
	ON_COMMAND(ID_TAB_UPDOWN_DELETE, OnTabUpdownDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadsView diagnostics

#ifdef _DEBUG
void CDownloadsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDownloadsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDownloadsView message handlers

void CDownloadsView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class

	m_listDownloads.m_nType = DOWNLOADVIEW;	// 리스트가 지신의 기능만을 처리할수 있는 타입값

	
	// static 컨트롤 출력
	CString state;
	state.Format("%s", "Downloads (2/10)");
	m_staticState.SetWindowText(state);

	
	

	// listCtrl
	m_listDownloads.InsertColumn(0, "File Name", LVCFMT_CENTER, 300);
	m_listDownloads.InsertColumn(1, "Size", LVCFMT_RIGHT, 110);
	m_listDownloads.InsertColumn(2, "Receive Size", LVCFMT_RIGHT, 130);
	m_listDownloads.InsertColumn(3, "Progress", LVCFMT_RIGHT, 120);
	m_listDownloads.InsertColumn(4, "Status", LVCFMT_CENTER, 110);
	m_listDownloads.InsertColumn(5, "Server Name", LVCFMT_LEFT, 200);	
	m_listDownloads.InsertColumn(6, "Num", LVCFMT_RIGHT, 50);	
	
	m_listDownloads.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// 밑줄나오게 설정

	/**
	// 256 칼라 이상 적용(icon) 
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_LIST_LIBRARY_CONNECT);
	imageList.Create(16, 16, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(255, 255, 255)); 
	m_listDownloads.SetImageList(&imageList, LVSIL_SMALL);
	imageList.Detach();
	bitmap.Detach();
	**/

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	pMain->GetSysImgList(m_listDownloads, m_imgSmallList);	// 시스템아이콘얻기

	// 링크드리스트에 있는 값 리스트컨트롤에 뿌림
	MY_DOWNLOAD_FILE_LIST tempFile;	// 일단은 여기에 값을저장후
	CString serverName;
	POSITION pos;
	pos = pMain->m_downloadFileList.GetHeadPosition();
	while(pos){
		tempFile = pMain->m_downloadFileList.GetAt(pos);
		serverName.Format("%s [%s]", tempFile.strServerName, tempFile.strServerIP);
		AddItem(tempFile.downloadList.strFileName, tempFile.downloadList.nByte, tempFile.nReceiveSize, tempFile.strStatus, 
			serverName,	tempFile.nIndex);
		pMain->m_downloadFileList.GetNext(pos);
	}

	// static 업데이트
	CString strState;
	strState.Format("Downloads (%d/%d)", pMain->m_myIOCPSocket.m_downloadFileDataList.GetCount(), m_listDownloads.GetItemCount());
	m_staticState.SetWindowText(strState);
}

void CDownloadsView::AddItem(CString fileName, DWORD size, DWORD nReceiveSize, CString strStatus, CString serverName, UINT nNum)
{
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	SHFILEINFO sfi;
    SHGetFileInfo(fileName, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON );
	int img = sfi.iIcon;

	LV_ITEM a;
	a.iItem = m_listDownloads.GetItemCount();	// 삽입 위치
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// 실직적으로 표현될값
	a.iSubItem = 0;	// 열인덱스
	a.iImage = img;	// 이미지 인덱스
	a.stateMask = LVIS_STATEIMAGEMASK;	// 상태변화를 Mask 처리
	a.state = INDEXTOSTATEIMAGEMASK(1);	// 유효한 상태 비트
	fileName.Format("%s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// 문자열 
	CString temp;
	m_listDownloads.InsertItem(&a);	
	temp.Format("%s KB", pMain->ChangeComma(size));
	m_listDownloads.SetItemText(a.iItem, 1, temp);
	temp.Format("%s KB", pMain->ChangeComma(nReceiveSize));
	m_listDownloads.SetItemText(a.iItem, 2, temp);

	char szReceiveSize[256];
	float total = (float)size;
	float recv = (float)nReceiveSize;
	int pu = (int)ceil((recv / total) * 100);
	sprintf(szReceiveSize, "[ %d%% ]", pu);		
	m_listDownloads.SetItemText(a.iItem, 3, szReceiveSize);

	m_listDownloads.SetItemText(a.iItem, 4, strStatus);
	m_listDownloads.SetItemText(a.iItem, 5, serverName);
	temp.Format("%d", nNum);
	m_listDownloads.SetItemText(a.iItem, 6, temp);	// 의미는 없지만 조금더 편하게 하기위해번호를 부여(실제다운받는것고연동을위해)
}


void CDownloadsView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(pMain->m_myIOCPSocket.m_downloadFileDataList.GetCount() > 0){	// 다운로드중일때만
		// 다운로드되는목록과 실제다운되고있는목록을 비교 (중첩 while)
		// 실제다운되고있는것
		POSITION posIOCP;	
		posIOCP = pMain->m_myIOCPSocket.m_downloadFileDataList.GetHeadPosition();
		MY_FILE_DATA *pIOCP;

		// 다운로드목록데이타
		POSITION posData;	
		posData = pMain->m_downloadFileList.GetHeadPosition();
		MY_DOWNLOAD_FILE_LIST *pData;
		
		BOOL bOut = TRUE;
		while(posIOCP){	
			bOut = TRUE;
			pIOCP = &(pMain->m_myIOCPSocket.m_downloadFileDataList.GetAt(posIOCP));
			while(posData){
				pData = &(pMain->m_downloadFileList.GetAt(posData));
				if(pIOCP->nIndex == pData->nIndex){
					for(int i=0; i<m_listDownloads.GetItemCount(); i++){	// 리스트목록과 번호비교
						char szNum[5];
						m_listDownloads.GetItemText(i, 6, szNum, 5);						
						if(pIOCP->nIndex == (UINT)atoi(szNum)){
							// 받은데이타양		
							char szReceiveSize[256];
							sprintf(szReceiveSize, "%s KB", pMain->ChangeComma(pIOCP->nReceiveSize));
							m_listDownloads.SetItemText(i, 2, szReceiveSize);
							
							float total = (float)pIOCP->nFileSize;
							float recv = (float)pIOCP->nReceiveSize;
							int pu = (int)ceil((recv / total) * 100);
							sprintf(szReceiveSize, "[ %d%% ]", pu);
							m_listDownloads.SetItemText(i, 3, szReceiveSize);
						//	pIOCP->nReceiveSize = pData->nReceiveSize;
							break;
							bOut = FALSE;
						}						
					}
				}
				if(bOut == FALSE){
					break;
				}
				pMain->m_downloadFileList.GetNext(posData);
			}			
			pMain->m_myIOCPSocket.m_downloadFileDataList.GetNext(posIOCP);			
		}
	}
	CFormView::OnTimer(nIDEvent);
}

int CDownloadsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetTimer(0, 500, NULL);

	return 0;
}

void CDownloadsView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	m_listDownloads.SetImageList(NULL, LVSIL_SMALL);
	KillTimer(0);	
}


void CDownloadsView::OnRclickListDownloads(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	int intNum = pNMListView->iItem;
	if(0 < m_listDownloads.GetSelectedCount()){	// 선택을했을시만
		// TrackPopupMenu
		CMenu obMenu;
		obMenu.LoadMenu(IDR_MENU_TAB_UPDOWN);
		CMenu *pPopupMenu = obMenu.GetSubMenu(0);
		ASSERT(pPopupMenu);
		CPoint obCursorPoint = (0, 0);
		GetCursorPos(&obCursorPoint);
		//if(0 >= m_listServer.GetSelectedCount()){
		//	pPopupMenu->EnableMenuItem(ID_SERVER_LIBRARY_DELETE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED); 
		//}else{
		//	pPopupMenu->EnableMenuItem(ID_SERVER_LIBRARY_DELETE, MF_BYCOMMAND | MF_ENABLED);
		//}
		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, obCursorPoint.x, obCursorPoint.y, this);
	}
	
	*pResult = 0;
}

void CDownloadsView::OnTabUpdownDelete() 
{
	// TODO: Add your command handler code here
	m_listDownloads.MyDeleteItem();
}
