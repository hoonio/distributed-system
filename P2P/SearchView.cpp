// SearchView.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "SearchView.h"

#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchView

IMPLEMENT_DYNCREATE(CSearchView, CFormView)

CSearchView::CSearchView()
	: CFormView(CSearchView::IDD)
{
	//{{AFX_DATA_INIT(CSearchView)
	m_strKey = _T("");
	//}}AFX_DATA_INIT

	// User Code
	
}


CSearchView::~CSearchView()
{

}

void CSearchView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchView)
	DDX_Control(pDX, IDC_LIST_SEARCH, m_listSearch);
	DDX_Control(pDX, IDC_COMBO_SERVERFILTER, m_comboFilter);
	DDX_Control(pDX, IDC_TAB_SEARCH, m_tabSearch);
	DDX_Text(pDX, IDC_EDIT_SEARCHKEY, m_strKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchView, CFormView)
	//{{AFX_MSG_MAP(CSearchView)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SEARCH, OnSelchangeTabSearch)
	ON_NOTIFY(NM_RCLICK, IDC_TAB_SEARCH, OnRclickTabSearch)
	ON_COMMAND(ID_TAB_SEARCH_DELETE, OnTabSearchDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCH, OnDblclkListSearch)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchView diagnostics

#ifdef _DEBUG
void CSearchView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSearchView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSearchView message handlers

UINT CSearchView::m_nTabCnt = 0;	// (갯수가 계속남아야하므로 static)


void CSearchView::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here
	// Tab
	if(((CMainFrame *)AfxGetMainWnd())->m_bConnectedFlag == TRUE){	// 서버에 접속중일때만
		if(m_nTabCnt + 1 > MAX_SEARCH_COUNT){
			AfxMessageBox("더이상 검색할수 없습니다!\n검색된 목록을 지우십시요!");
			return;
		}
		int aa = ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_nClntCount;
		if(aa == 0){
			AfxMessageBox("접속되있는 서버가 없습니다!");
			m_strKey = "";
			UpdateData(false);
			return;
		}	
		
		UpdateData(true);
		if(m_strKey == ""){
			GetDlgItem(IDC_EDIT_SEARCHKEY)->SetFocus();
			return;
		}
						
		char szFileName[256];
		sprintf(szFileName, "%s", m_strKey);
		if(strcmp(szFileName, "*.*") == 0){
			AfxMessageBox("*.* 파일은 검색할수 없습니다");
			m_strKey = "";
			UpdateData(false);
			return;
		}
		m_nSearchFileCnt = 0;	// 새로 탭이 추가되므로 검색된 파일의수도 0
		m_nTabCnt++;

		CreateTab(m_strKey, 0, 0);	// 동적으로 탭추가, 버튼으로 생성될때는 두번째 파라미터가 필요없음	

		// 서버에 메시지 전송
		CMyIOCPSocket *pSock = &(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket);
		UINT curSel = m_comboFilter.GetCurSel();
		if(curSel == 0){	// 전체검색
			for(int i=0; i<pSock->m_nClntCount; i++){
				if(pSock->SendData(pSock->m_clntSock[i].pClntData, pSock->m_pClntIOData[i], P_SEARCH_FILE, szFileName, strlen(szFileName), TRUE) == FALSE){
					((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.ErrorHandling(2, "SendData()");		
				}
			}
		}else{	// 컴하나검색
			CString strIP;
			m_comboFilter.GetLBText(curSel, strIP);
			UINT nStart = strIP.Find("[", 0);
			UINT nEnd = strIP.Find("]", 0);
			strIP = strIP.Mid(nStart + 1, nEnd - nStart - 1);
			//AfxMessageBox(strIP);
			for(int i=0; i<pSock->m_nClntCount; i++){
				if(strcmp(strIP, inet_ntoa(pSock->m_clntSock[i].pClntData->clntAddr.sin_addr)) == 0){	// 아이피 비교
					if(pSock->SendData(pSock->m_clntSock[i].pClntData, pSock->m_pClntIOData[i], P_SEARCH_FILE, szFileName, strlen(szFileName), TRUE) == FALSE){
						((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.ErrorHandling(2, "SendData()");		
					}
					break;
				}
			}
			
		}

		m_listSearch.DeleteAllItems();	// 리스트의 모든목록 삭제

		CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
		pMain->m_searchListData[m_nTabCnt - 1].strSearchListFileName = m_strKey;	// 검색된 파일이름 저장

		
		m_strKey = "";
		UpdateData(false);
		GetDlgItem(IDC_EDIT_SEARCHKEY)->SetFocus();
	}else{
		AfxMessageBox("서버에 먼저 접속하십시요!");
		m_strKey = "";
		UpdateData(false);
	}
}

void CSearchView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class

	m_listSearch.m_nType = SEARCHVIEW;	// 리스트가 지신의 기능만을 처리할수 있는 타입값

	// 접속되어있는 클라이언트의 목록을 추가
	m_comboFilter.AddString("전체");
	int count = ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_nClntCount;
	char szClntName[256];
	char szClntIP[20];
	for(int i=0; i<count; i++){
		strcpy(szClntName, ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[i].clntName);
		strcpy(szClntIP, inet_ntoa(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[i].pClntData->clntAddr.sin_addr));
		sprintf(szClntName, "%s [%s]", szClntName, szClntIP);
		m_comboFilter.AddString(szClntName);
	}
	
	m_comboFilter.SetCurSel(0);

	InsertColumn();	// 컬럼 추가

	// 처음에 탭내용이 하나도 없으면 탭컨트롤이 안보이게(처음부터 보이면 모양이 이상해짐)
	if(m_nTabCnt == 0){
		m_tabSearch.ShowWindow(FALSE);
	}

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	pMain->GetSysImgList(m_listSearch, m_imgSmallList);	// 시스템아이콘얻기


	// 다른메뉴를 선택했다가 다시 이뷰를 선택하면 다시 초기화가 되므로 생성되었던 탭을 생성	
	if(m_nTabCnt != 0){
		
		UINT cnt = 0;	
		CString strName;	// 검색된 파일이름
		UINT nFileCnt;	// 검색된 파일수
		UINT nImage;	// 아이콘 이미지
		while(cnt != m_nTabCnt){		
			strName = pMain->m_searchListData[cnt].strSearchListFileName;
			nFileCnt = pMain->m_searchListData[cnt].nSearchListSize;
			nImage = 0;
			if(nFileCnt > 5){	// 검색된게 많으면 아이콘을 바꿈
				nImage = 1;
			}
			CreateTab(strName, nFileCnt, nImage);	// 동적으로 탭추가, 두번째 파라미터로 마지막 생성되는것만 리스트에 데이타를 넣는다		
			cnt ++;
		}
		CString strFileName;	// 파일명
		UINT nSize;		// 파일사이즈
		CString strServerName;	// 서버이름
		CString strServerIP;	// 서버아이피
		CString strServerInfo;	// 서버정보
		for(UINT i=0; i<nFileCnt; i++){	// 리스트 내용
			strFileName = pMain->m_searchListData[m_nTabCnt - 1].searchListData[i].strFileName;
			nSize = pMain->m_searchListData[m_nTabCnt - 1].searchListData[i].nByte;
			strServerName.Format("%s", pMain->m_searchListData[m_nTabCnt - 1].strServerName[i]);	// 서버이름
			strServerIP.Format("%s", pMain->m_searchListData[m_nTabCnt - 1].strServerIP[i]);	// 서버아이피
			strServerInfo.Format("%s [%s]", strServerName, strServerIP);
			AddItem(strFileName, nSize, strServerInfo, FALSE);
		}
	}
	
	GetDlgItem(IDC_EDIT_SEARCHKEY)->SetFocus();

	
}



void CSearchView::InsertColumn()
{
	m_listSearch.InsertColumn(0, "File Name", LVCFMT_CENTER, 400);
	m_listSearch.InsertColumn(1, "Size", LVCFMT_RIGHT, 150);
	m_listSearch.InsertColumn(2, "Server IP", LVCFMT_LEFT, 200);
	m_listSearch.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// 밑줄나오게 설정
}


void CSearchView::AddItem(CString fileName, UINT size, CString serverName, BOOL flag)
{
	if(flag == TRUE){	// 검색해서 추가할때(OnInitialUpdate 에서 추가할때는 아님)
		m_nSearchFileCnt++;	// 검색됨 파일의수를 증가
		CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
		pMain->m_searchListData[m_nTabCnt - 1].nSearchListSize = m_nSearchFileCnt;	// 검색된파일갯수저장, m_tabSearch 를 이미 전에 하나증가시켰음
		// 검색된목록의 수변경		
		TCITEM tc;
		tc.mask = TCIF_TEXT;
		char szTitle[256];
		sprintf(szTitle, "%s [%d]", pMain->m_searchListData[m_nTabCnt - 1].strSearchListFileName, m_nSearchFileCnt);
		tc.pszText = szTitle;
		m_tabSearch.SetItem(m_nTabCnt - 1, &tc) ;
		if(m_nSearchFileCnt > 5){	// 검색된게 많으면 아이콘을 바꿈
			TCITEM tc2;
			tc2.mask = TCIF_IMAGE;
			tc2.iImage = 1;
			m_tabSearch.SetItem(m_nTabCnt - 1, &tc2) ;
		}
	}

	SHFILEINFO sfi;
    SHGetFileInfo(fileName, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON );
	int img = sfi.iIcon;

	LV_ITEM a;
	a.iItem = m_listSearch.GetItemCount();	// 삽입 위치
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// 실직적으로 표현될값
	a.iSubItem = 0;	// 열인덱스
	a.iImage = img;//shinfo.iIcon;//shInfo.iIcon;//GetIconIndex(fileName);//image;	// 이미지 인덱스
	a.stateMask = LVIS_STATEIMAGEMASK;	// 상태변화를 Mask 처리
	a.state = INDEXTOSTATEIMAGEMASK(1);	// 유효한 상태 비트
	fileName.Format(" %s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// 문자열 
	CString temp;
	m_listSearch.InsertItem(&a);	
	
	temp.Format("%s KB", ((CMainFrame *)AfxGetMainWnd())->ChangeComma(size));
	m_listSearch.SetItemText(a.iItem, 1, temp);
	m_listSearch.SetItemText(a.iItem, 2, serverName);
}

void CSearchView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	m_listSearch.SetImageList(NULL, LVSIL_SMALL);
	

}

void CSearchView::CreateTab(CString searchName, UINT cnt, UINT nImage)
{
	// 256 칼라 이상 적용(icon) 
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_SEARCH_TAB);
	imageList.Create(16, 16, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(255, 255, 255)); 
	//m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // 활성이미지
	m_tabSearch.SetImageList(&imageList);
	imageList.Detach();
	bitmap.Detach();

	// 처음에 탭내용이 하나도 없으면 탭컨트롤이 안보이게 설정했으므로 탭컨트롤을 보이게 (처음부터 보이면 모양이 이상해짐)
	m_tabSearch.ShowWindow(TRUE);
	
	
	CString temp;
	temp.Format("%s [%d]", searchName, cnt);
	m_tabSearch.InsertItem(m_nTabCnt, temp, nImage);	
	m_tabSearch.SetCurSel(m_nTabCnt - 1);

	if(cnt == 0  ||  cnt == m_nTabCnt){	// 버튼클릭으로 생성되거나, OnInitialUpdate에서 마지막것만
		
	}
}



void CSearchView::OnSelchangeTabSearch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int cur = m_tabSearch.GetCurSel();
	*pResult = 0;

	m_listSearch.DeleteAllItems();	// 리스트의 모든목록 삭제

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	UINT nFileCnt = pMain->m_searchListData[cur].nSearchListSize;
	CString strFileName;	// 파일명
	UINT nSize;		// 파일사이즈
	CString strServerName;	// 서버이름
	CString strServerIP;	// 서버아이피
	CString strServerInfo;	// 서버정보
	for(UINT i=0; i<nFileCnt; i++){	// 리스트 내용
		strFileName = pMain->m_searchListData[cur].searchListData[i].strFileName;
		nSize = pMain->m_searchListData[cur].searchListData[i].nByte;
		strServerName.Format("%s", pMain->m_searchListData[cur].strServerName[i]);	// 서버이름
		strServerIP.Format("%s", pMain->m_searchListData[cur].strServerIP[i]);	// 서버아이피
		strServerInfo.Format("%s [%s]", strServerName, strServerIP);
		AddItem(strFileName, nSize, strServerInfo, FALSE);
	}
}

void CSearchView::OnRclickTabSearch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	// TrackPopupMenu
	CMenu obMenu;
	obMenu.LoadMenu(IDR_MENU_TAB_SEARCH);
	CMenu *pPopupMenu = obMenu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	CPoint obCursorPoint = (0, 0);
	GetCursorPos(&obCursorPoint);
	/**
	if(0 >= m_listServer.GetSelectedCount()){
		pPopupMenu->EnableMenuItem(ID_SERVER_LIBRARY_DELETE, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED); 
	}else{
		pPopupMenu->EnableMenuItem(ID_SERVER_LIBRARY_DELETE, MF_BYCOMMAND | MF_ENABLED);
	}
	**/
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, obCursorPoint.x, obCursorPoint.y, this);


	*pResult = 0;
}


void CSearchView::OnTabSearchDelete() 
{
	// TODO: Add your command handler code here

	// 삭제를 하는데 마우스오른쪽버튼클린한것을 삭제해야하는데 인덱스번호가져오는법을몰라 현재선택된것을 삭제함, 인덱스로 처리하면 데이타정리하는부분 수정해야함
	int cur = m_tabSearch.GetCurSel();
	m_tabSearch.DeleteItem(cur);

	// 저장해놓은데이타 정리
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	for(UINT i=cur; i<m_nTabCnt-1; i++){
		pMain->m_searchListData[i] = pMain->m_searchListData[i + 1];	// 지운탭의내용은 지운다
	}	
	m_listSearch.DeleteAllItems();	// 리스트의 모든목록 삭제
	UINT nFileCnt = pMain->m_searchListData[cur].nSearchListSize;
	CString strFileName;	// 파일명
	UINT nSize;		// 파일사이즈
	CString strServerName;	// 서버이름
	CString strServerIP;	// 서버아이피
	CString strServerInfo;	// 서버정보
	for(i=0; i<nFileCnt; i++){	// 리스트 내용
		strFileName = pMain->m_searchListData[m_nTabCnt - 2].searchListData[i].strFileName;
		nSize = pMain->m_searchListData[m_nTabCnt - 2].searchListData[i].nByte;
		strServerName.Format("%s", pMain->m_searchListData[m_nTabCnt - 2].strServerName[i]);	// 서버이름
		strServerIP.Format("%s", pMain->m_searchListData[m_nTabCnt - 2].strServerIP[i]);	// 서버아이피
		strServerInfo.Format("%s [%s]", strServerName, strServerIP);
		AddItem(strFileName, nSize, strServerInfo, FALSE);
	}
		
	m_nTabCnt--;
	m_tabSearch.SetCurSel(m_nTabCnt - 1);
}

void CSearchView::AddItemSearchFile(char *szOut, UINT clntCnt)
{
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	
	char szClntName[256];
	char szClntIP[20];
	char szClntInfo[256];
	
	strcpy(szClntName, ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[clntCnt].clntName);
	strcpy(szClntIP, inet_ntoa(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[clntCnt].pClntData->clntAddr.sin_addr));
	sprintf(szClntInfo, "%s [%s]", szClntName, szClntIP);	
	
	////// 문자열을 쪼개서 처리
	UINT cnt;
	CString strMessage;
	strMessage.Format("%s", szOut);
	cnt = strMessage.Find('?', 0);		
	
	// 여러개의 정보를 쪼갠다
	char comCnt[4];
	if(cnt == 1){	// 파일가 한자리일때
		comCnt[0] = szOut[0];
		comCnt[1] = '\0';
	}else if(cnt == 2){	// 파일가 두자리일때
		comCnt[0] = szOut[0];
		comCnt[1] = szOut[1];		
		comCnt[2] = '\0';
	}else if(cnt == 3){	
		comCnt[0] = szOut[0];
		comCnt[1] = szOut[1];		
		comCnt[2] = szOut[2];		
		comCnt[3] = '\0';
	}
	MY_SEARCH_FILE searchFile;
	int oldCnt = 0;	
	for(int i=0; i<atoi(comCnt); i++){
		if(m_nSearchFileCnt + 1 > MAX_SEARCH_FILE_COUNT){
			CString out;
			out.Format("%d개 이상의 파일은 검색할수 없습니다!", MAX_SEARCH_FILE_COUNT);
			AfxMessageBox(out);
			break;
		}
		// 파일명
		oldCnt = cnt;
		cnt = strMessage.Find('?', oldCnt + 1);						
		searchFile.strFileName = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);

		// 파일경로
		oldCnt = cnt;
		cnt = strMessage.Find('?', oldCnt + 1);						
		searchFile.strFilePath = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);

		// 용량
		oldCnt = cnt;
		cnt = strMessage.Find('?', oldCnt + 1);						
		searchFile.nByte = atoi(strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1));
		
		// 검색된파일내용 저장, 검색된 파일명은 OnButtonSearch()서 처리, 검색된수는 AddItem() 에서 처리
		pMain->m_searchListData[m_nTabCnt - 1].searchListData[m_nSearchFileCnt] = searchFile;	
		pMain->m_searchListData[m_nTabCnt - 1].strServerName[m_nSearchFileCnt].Format("%s", szClntName);	// 서버이름
		pMain->m_searchListData[m_nTabCnt - 1].strServerIP[m_nSearchFileCnt].Format("%s", szClntIP);	// 서버아이피
		AddItem(searchFile.strFileName, searchFile.nByte, szClntInfo, TRUE);
	}

	delete szOut;	// 보내는 쪽에서 동적으로 생성하므로 delete

	// 모든컴에서 검색된 파일이 없을때 나와야하는데 하나의 컴이라도 검색된게 없으면 나옴
	//if(atoi(comCnt) == 0){
	//	AfxMessageBox("검색된 파일이 없습니다!");
	//}
}

void CSearchView::OnDblclkListSearch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	int intNum = pNMListView->iItem;
	if(m_listSearch.GetItemText(intNum, 0) == ""){	// 데이타가 없는곳을 더블클릭시
		return;
	}

	UINT cur = m_tabSearch.GetCurSel();
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	CMyIOCPSocket *pSock = &(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket);


	CString strIP;
	strIP = pMain->m_searchListData[cur].strServerIP[intNum];
	for(int i=0; i<pSock->m_nClntCount; i++){
		if(strcmp(strIP, inet_ntoa(pSock->m_clntSock[i].pClntData->clntAddr.sin_addr)) == 0){	// 아이피 비교
			break;
		}
	}
	int cnt = i;	// m_clntSock 의 클라이언트 번호
	


	if(pSock->m_nClntCount == cnt){
		AfxMessageBox("서버를 찾을수 없습니다!");
		return;
	}
	
	// 일단은 여기에 값을저장후 링크드리스트에저장
	MY_DOWNLOAD_FILE_LIST tempFile;	
	tempFile.downloadList = pMain->m_searchListData[cur].searchListData[intNum];
	tempFile.nReceiveSize = 0;
	tempFile.nSpeed = 0;
	tempFile.strStatus = "다운로드중";
	tempFile.strServerIP = pMain->m_searchListData[cur].strServerIP[intNum];
	tempFile.strServerName = pMain->m_searchListData[cur].strServerName[intNum];	
	tempFile.nIndex = pMain->m_nDownloadMaxIndex;
	pMain->m_myIOCPSocket.m_nDownloadMaxIndex = pMain->m_nDownloadMaxIndex;	// 항상값을 동기화시킨다
	pMain->m_nDownloadMaxIndex++;
	pMain->m_downloadFileList.AddHead(tempFile);	// 링크드리스트에 값저장

	// IOCP 전송
	char szMessage[256];
	sprintf(szMessage, "%s%s", pMain->m_searchListData[cur].searchListData[intNum].strFilePath, pMain->m_searchListData[cur].searchListData[intNum].strFileName);
	if(pSock->SendData(pSock->m_clntSock[cnt].pClntData, pSock->m_pClntIOData[cnt], P_DOWNLOAD_FILE_NOTICE, szMessage, strlen(szMessage), TRUE) == FALSE){
		((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.ErrorHandling(2, "SendData()");		
	}
	

	*pResult = 0;
}


