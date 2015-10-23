// UploadsView.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "UploadsView.h"

#include "MainFrm.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUploadsView

IMPLEMENT_DYNCREATE(CUploadsView, CFormView)

CUploadsView::CUploadsView()
	: CFormView(CUploadsView::IDD)
{
	//{{AFX_DATA_INIT(CUploadsView)
	//}}AFX_DATA_INIT
}

CUploadsView::~CUploadsView()
{
}

void CUploadsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUploadsView)
	DDX_Control(pDX, IDC_LIST_UPLOADS, m_listUploads);
	DDX_Control(pDX, IDC_STATIC_STATE, m_staticState);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUploadsView, CFormView)
	//{{AFX_MSG_MAP(CUploadsView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_UPLOADS, OnRclickListUploads)
	ON_COMMAND(ID_TAB_UPDOWN_DELETE, OnTabUpdownDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUploadsView diagnostics

#ifdef _DEBUG
void CUploadsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CUploadsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUploadsView message handlers

void CUploadsView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	m_listUploads.m_nType = UPLOADVIEW;	// 리스트가 지신의 기능만을 처리할수 있는 타입값

	// static 컨트롤 출력
	CString state;
	state.Format("%s", "Uploads (5/8)");
	m_staticState.SetWindowText(state);

	// listCtrl
	m_listUploads.InsertColumn(0, "File Name", LVCFMT_CENTER, 300);
	m_listUploads.InsertColumn(1, "Size", LVCFMT_RIGHT, 110);
	m_listUploads.InsertColumn(2, "Send Size", LVCFMT_RIGHT, 130);
	m_listUploads.InsertColumn(3, "Progress", LVCFMT_RIGHT, 120);
	m_listUploads.InsertColumn(4, "Status", LVCFMT_CENTER, 110);
	m_listUploads.InsertColumn(5, "Client Name", LVCFMT_LEFT, 200);	
	m_listUploads.InsertColumn(6, "Num", LVCFMT_RIGHT, 50);	

	m_listUploads.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// 밑줄나오게 설정
	
	/**
	// 샘플 데이타
	CString fileName[] = {"Directory.ico", "toolbar20.bmp", "따라해보는후킹구현.zip"};
	UINT size[] = {123, 1200, 91210};
	CString serverName[] = {"권용재 [123.122.322.124]", "전지현 [125.32.22.124]", "자우림빠른서버 [23.242.322.24]"};
	UINT speed[] = {1231, 22, 911};
	for(int i=0; i<3; i++){		
		AddItem(fileName[i], size[i], serverName[i], speed[i], "testFolder");
	}
	**/

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	pMain->GetSysImgList(m_listUploads, m_imgSmallList);	// 시스템아이콘얻기

	// 링크드리스트에 있는 값 리스트컨트롤에 뿌림
	MY_DOWNLOAD_FILE_LIST tempFile;	// 일단은 여기에 값을저장후
	CString serverName;
	POSITION pos;
	pos = pMain->m_uploadFileList.GetHeadPosition();
	while(pos){
		tempFile = pMain->m_uploadFileList.GetAt(pos);
		serverName.Format("%s [%s]", tempFile.strServerName, tempFile.strServerIP);
		AddItem(tempFile.downloadList.strFileName, tempFile.downloadList.nByte, tempFile.nReceiveSize, tempFile.strStatus, 
			serverName,	tempFile.nIndex);
		pMain->m_uploadFileList.GetNext(pos);
	}

	// static 업데이트
	CString strState;
	strState.Format("Uploads (%d/%d)", pMain->m_myIOCPSocket.m_uploadFileDataList.GetCount(), m_listUploads.GetItemCount());
	m_staticState.SetWindowText(strState);

	
	
}

void CUploadsView::AddItem(CString fileName, DWORD size, DWORD nReceiveSize, CString strStatus, CString serverName, UINT nNum)
{
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	SHFILEINFO sfi;
    SHGetFileInfo(fileName, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON );
	int img = sfi.iIcon;

	LV_ITEM a;
	a.iItem = m_listUploads.GetItemCount();	// 삽입 위치
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// 실직적으로 표현될값
	a.iSubItem = 0;	// 열인덱스
	a.iImage = img;	// 이미지 인덱스
	a.stateMask = LVIS_STATEIMAGEMASK;	// 상태변화를 Mask 처리
	a.state = INDEXTOSTATEIMAGEMASK(1);	// 유효한 상태 비트
	fileName.Format("%s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// 문자열 
	CString temp;
	m_listUploads.InsertItem(&a);	
	temp.Format("%s KB", pMain->ChangeComma(size));
	m_listUploads.SetItemText(a.iItem, 1, temp);
	temp.Format("%s KB", pMain->ChangeComma(nReceiveSize));
	m_listUploads.SetItemText(a.iItem, 2, temp);

	char szReceiveSize[256];
	float total = (float)size;
	float recv = (float)nReceiveSize;
	int pu = (int)ceil((recv / total) * 100);
	sprintf(szReceiveSize, "[ %d%% ]", pu);		
	m_listUploads.SetItemText(a.iItem, 3, szReceiveSize);

	m_listUploads.SetItemText(a.iItem, 4, strStatus);
	m_listUploads.SetItemText(a.iItem, 5, serverName);
	temp.Format("%d", nNum);
	m_listUploads.SetItemText(a.iItem, 6, temp);	// 의미는 없지만 조금더 편하게 하기위해번호를 부여(실제다운받는것고연동을위해)

/**
	LV_ITEM a;
	a.iItem = m_listUploads.GetItemCount();	// 삽입 위치
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// 실직적으로 표현될값
	a.iSubItem = 0;	// 열인덱스
//	a.iImage = image;	// 이미지 인덱스
	a.stateMask = LVIS_STATEIMAGEMASK;	// 상태변화를 Mask 처리
	a.state = INDEXTOSTATEIMAGEMASK(1);	// 유효한 상태 비트
	fileName.Format("[icon] %s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// 문자열 
	CString temp;
	m_listUploads.InsertItem(&a);	
	temp.Format("%d KB", size);
	m_listUploads.SetItemText(a.iItem, 1, temp);
	temp.Format("%d KB", rand() % 100);
	m_listUploads.SetItemText(a.iItem, 2, temp);
	m_listUploads.SetItemText(a.iItem, 3, "5% 막대");
	m_listUploads.SetItemText(a.iItem, 4, "상태아이콘");
	m_listUploads.SetItemText(a.iItem, 5, serverName);
	temp.Format("%d KB/s", speed);
	m_listUploads.SetItemText(a.iItem, 6, temp);
	m_listUploads.SetItemText(a.iItem, 7, "19 m");
	m_listUploads.SetItemText(a.iItem, 8, "c:\\it\\test");
	**/
}



void CUploadsView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	m_listUploads.SetImageList(NULL, LVSIL_SMALL);
	KillTimer(0);
}

void CUploadsView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(pMain->m_myIOCPSocket.m_uploadFileDataList.GetCount() > 0){	// 다운로드중일때만
		// 다운로드되는목록과 실제다운되고있는목록을 비교 (중첩 while)
		// 실제다운되고있는것
		POSITION posIOCP;	
		posIOCP = pMain->m_myIOCPSocket.m_uploadFileDataList.GetHeadPosition();
		MY_FILE_DATA *pIOCP;

		// 다운로드목록데이타
		POSITION posData;	
		posData = pMain->m_uploadFileList.GetHeadPosition();
		MY_DOWNLOAD_FILE_LIST *pData;
		
		BOOL bOut = TRUE;
		while(posIOCP){	
			bOut = TRUE;
			pIOCP = &(pMain->m_myIOCPSocket.m_uploadFileDataList.GetAt(posIOCP));
			while(posData){
				pData = &(pMain->m_uploadFileList.GetAt(posData));
				if(pIOCP->nIndex == pData->nIndex){
					for(int i=0; i<m_listUploads.GetItemCount(); i++){	// 리스트목록과 번호비교
						char szNum[5];
						m_listUploads.GetItemText(i, 6, szNum, 5);						
						if(pIOCP->nIndex == (UINT)atoi(szNum)){
							// 받은데이타양		
							char szReceiveSize[256];
							sprintf(szReceiveSize, "%s KB", pMain->ChangeComma(pIOCP->nReceiveSize));
							m_listUploads.SetItemText(i, 2, szReceiveSize);
							
							float total = (float)pIOCP->nFileSize;
							float recv = (float)pIOCP->nReceiveSize;
							int pu = (int)ceil((recv / total) * 100);
							sprintf(szReceiveSize, "[ %d%% ]", pu);
							m_listUploads.SetItemText(i, 3, szReceiveSize);
							//pIOCP->nReceiveSize = pData->nReceiveSize;
							break;
							bOut = FALSE;
						}						
					}
				}
				if(bOut == FALSE){
					break;
				}
				pMain->m_uploadFileList.GetNext(posData);
			}			
			pMain->m_myIOCPSocket.m_uploadFileDataList.GetNext(posIOCP);			
		}
	}
	CFormView::OnTimer(nIDEvent);
}

int CUploadsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetTimer(0, 500, NULL);
	
	return 0;
}

void CUploadsView::OnRclickListUploads(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	int intNum = pNMListView->iItem;
	if(0 < m_listUploads.GetSelectedCount()){	// 선택을했을시만
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

void CUploadsView::OnTabUpdownDelete() 
{
	// TODO: Add your command handler code here
	m_listUploads.MyDeleteItem();
}
