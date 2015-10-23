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

UINT CSearchView::m_nTabCnt = 0;	// (������ ��ӳ��ƾ��ϹǷ� static)


void CSearchView::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here
	// Tab
	if(((CMainFrame *)AfxGetMainWnd())->m_bConnectedFlag == TRUE){	// ������ �������϶���
		if(m_nTabCnt + 1 > MAX_SEARCH_COUNT){
			AfxMessageBox("���̻� �˻��Ҽ� �����ϴ�!\n�˻��� ����� ����ʽÿ�!");
			return;
		}
		int aa = ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_nClntCount;
		if(aa == 0){
			AfxMessageBox("���ӵ��ִ� ������ �����ϴ�!");
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
			AfxMessageBox("*.* ������ �˻��Ҽ� �����ϴ�");
			m_strKey = "";
			UpdateData(false);
			return;
		}
		m_nSearchFileCnt = 0;	// ���� ���� �߰��ǹǷ� �˻��� �����Ǽ��� 0
		m_nTabCnt++;

		CreateTab(m_strKey, 0, 0);	// �������� ���߰�, ��ư���� �����ɶ��� �ι�° �Ķ���Ͱ� �ʿ����	

		// ������ �޽��� ����
		CMyIOCPSocket *pSock = &(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket);
		UINT curSel = m_comboFilter.GetCurSel();
		if(curSel == 0){	// ��ü�˻�
			for(int i=0; i<pSock->m_nClntCount; i++){
				if(pSock->SendData(pSock->m_clntSock[i].pClntData, pSock->m_pClntIOData[i], P_SEARCH_FILE, szFileName, strlen(szFileName), TRUE) == FALSE){
					((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.ErrorHandling(2, "SendData()");		
				}
			}
		}else{	// ���ϳ��˻�
			CString strIP;
			m_comboFilter.GetLBText(curSel, strIP);
			UINT nStart = strIP.Find("[", 0);
			UINT nEnd = strIP.Find("]", 0);
			strIP = strIP.Mid(nStart + 1, nEnd - nStart - 1);
			//AfxMessageBox(strIP);
			for(int i=0; i<pSock->m_nClntCount; i++){
				if(strcmp(strIP, inet_ntoa(pSock->m_clntSock[i].pClntData->clntAddr.sin_addr)) == 0){	// ������ ��
					if(pSock->SendData(pSock->m_clntSock[i].pClntData, pSock->m_pClntIOData[i], P_SEARCH_FILE, szFileName, strlen(szFileName), TRUE) == FALSE){
						((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.ErrorHandling(2, "SendData()");		
					}
					break;
				}
			}
			
		}

		m_listSearch.DeleteAllItems();	// ����Ʈ�� ����� ����

		CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
		pMain->m_searchListData[m_nTabCnt - 1].strSearchListFileName = m_strKey;	// �˻��� �����̸� ����

		
		m_strKey = "";
		UpdateData(false);
		GetDlgItem(IDC_EDIT_SEARCHKEY)->SetFocus();
	}else{
		AfxMessageBox("������ ���� �����Ͻʽÿ�!");
		m_strKey = "";
		UpdateData(false);
	}
}

void CSearchView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class

	m_listSearch.m_nType = SEARCHVIEW;	// ����Ʈ�� ������ ��ɸ��� ó���Ҽ� �ִ� Ÿ�԰�

	// ���ӵǾ��ִ� Ŭ���̾�Ʈ�� ����� �߰�
	m_comboFilter.AddString("��ü");
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

	InsertColumn();	// �÷� �߰�

	// ó���� �ǳ����� �ϳ��� ������ ����Ʈ���� �Ⱥ��̰�(ó������ ���̸� ����� �̻�����)
	if(m_nTabCnt == 0){
		m_tabSearch.ShowWindow(FALSE);
	}

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	pMain->GetSysImgList(m_listSearch, m_imgSmallList);	// �ý��۾����ܾ��


	// �ٸ��޴��� �����ߴٰ� �ٽ� �̺並 �����ϸ� �ٽ� �ʱ�ȭ�� �ǹǷ� �����Ǿ��� ���� ����	
	if(m_nTabCnt != 0){
		
		UINT cnt = 0;	
		CString strName;	// �˻��� �����̸�
		UINT nFileCnt;	// �˻��� ���ϼ�
		UINT nImage;	// ������ �̹���
		while(cnt != m_nTabCnt){		
			strName = pMain->m_searchListData[cnt].strSearchListFileName;
			nFileCnt = pMain->m_searchListData[cnt].nSearchListSize;
			nImage = 0;
			if(nFileCnt > 5){	// �˻��Ȱ� ������ �������� �ٲ�
				nImage = 1;
			}
			CreateTab(strName, nFileCnt, nImage);	// �������� ���߰�, �ι�° �Ķ���ͷ� ������ �����Ǵ°͸� ����Ʈ�� ����Ÿ�� �ִ´�		
			cnt ++;
		}
		CString strFileName;	// ���ϸ�
		UINT nSize;		// ���ϻ�����
		CString strServerName;	// �����̸�
		CString strServerIP;	// ����������
		CString strServerInfo;	// ��������
		for(UINT i=0; i<nFileCnt; i++){	// ����Ʈ ����
			strFileName = pMain->m_searchListData[m_nTabCnt - 1].searchListData[i].strFileName;
			nSize = pMain->m_searchListData[m_nTabCnt - 1].searchListData[i].nByte;
			strServerName.Format("%s", pMain->m_searchListData[m_nTabCnt - 1].strServerName[i]);	// �����̸�
			strServerIP.Format("%s", pMain->m_searchListData[m_nTabCnt - 1].strServerIP[i]);	// ����������
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
	m_listSearch.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// ���ٳ����� ����
}


void CSearchView::AddItem(CString fileName, UINT size, CString serverName, BOOL flag)
{
	if(flag == TRUE){	// �˻��ؼ� �߰��Ҷ�(OnInitialUpdate ���� �߰��Ҷ��� �ƴ�)
		m_nSearchFileCnt++;	// �˻��� �����Ǽ��� ����
		CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
		pMain->m_searchListData[m_nTabCnt - 1].nSearchListSize = m_nSearchFileCnt;	// �˻������ϰ�������, m_tabSearch �� �̹� ���� �ϳ�����������
		// �˻��ȸ���� ������		
		TCITEM tc;
		tc.mask = TCIF_TEXT;
		char szTitle[256];
		sprintf(szTitle, "%s [%d]", pMain->m_searchListData[m_nTabCnt - 1].strSearchListFileName, m_nSearchFileCnt);
		tc.pszText = szTitle;
		m_tabSearch.SetItem(m_nTabCnt - 1, &tc) ;
		if(m_nSearchFileCnt > 5){	// �˻��Ȱ� ������ �������� �ٲ�
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
	a.iItem = m_listSearch.GetItemCount();	// ���� ��ġ
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// ���������� ǥ���ɰ�
	a.iSubItem = 0;	// ���ε���
	a.iImage = img;//shinfo.iIcon;//shInfo.iIcon;//GetIconIndex(fileName);//image;	// �̹��� �ε���
	a.stateMask = LVIS_STATEIMAGEMASK;	// ���º�ȭ�� Mask ó��
	a.state = INDEXTOSTATEIMAGEMASK(1);	// ��ȿ�� ���� ��Ʈ
	fileName.Format(" %s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// ���ڿ� 
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
	// 256 Į�� �̻� ����(icon) 
	CImageList imageList;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_SEARCH_TAB);
	imageList.Create(16, 16, ILC_COLORDDB  | ILC_MASK, 16, 1);
	imageList.Add(&bitmap, RGB(255, 255, 255)); 
	//m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);  // Ȱ���̹���
	m_tabSearch.SetImageList(&imageList);
	imageList.Detach();
	bitmap.Detach();

	// ó���� �ǳ����� �ϳ��� ������ ����Ʈ���� �Ⱥ��̰� ���������Ƿ� ����Ʈ���� ���̰� (ó������ ���̸� ����� �̻�����)
	m_tabSearch.ShowWindow(TRUE);
	
	
	CString temp;
	temp.Format("%s [%d]", searchName, cnt);
	m_tabSearch.InsertItem(m_nTabCnt, temp, nImage);	
	m_tabSearch.SetCurSel(m_nTabCnt - 1);

	if(cnt == 0  ||  cnt == m_nTabCnt){	// ��ưŬ������ �����ǰų�, OnInitialUpdate���� �������͸�
		
	}
}



void CSearchView::OnSelchangeTabSearch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int cur = m_tabSearch.GetCurSel();
	*pResult = 0;

	m_listSearch.DeleteAllItems();	// ����Ʈ�� ����� ����

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	UINT nFileCnt = pMain->m_searchListData[cur].nSearchListSize;
	CString strFileName;	// ���ϸ�
	UINT nSize;		// ���ϻ�����
	CString strServerName;	// �����̸�
	CString strServerIP;	// ����������
	CString strServerInfo;	// ��������
	for(UINT i=0; i<nFileCnt; i++){	// ����Ʈ ����
		strFileName = pMain->m_searchListData[cur].searchListData[i].strFileName;
		nSize = pMain->m_searchListData[cur].searchListData[i].nByte;
		strServerName.Format("%s", pMain->m_searchListData[cur].strServerName[i]);	// �����̸�
		strServerIP.Format("%s", pMain->m_searchListData[cur].strServerIP[i]);	// ����������
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

	// ������ �ϴµ� ���콺�����ʹ�ưŬ���Ѱ��� �����ؾ��ϴµ� �ε�����ȣ�������¹������� ���缱�õȰ��� ������, �ε����� ó���ϸ� ����Ÿ�����ϴºκ� �����ؾ���
	int cur = m_tabSearch.GetCurSel();
	m_tabSearch.DeleteItem(cur);

	// �����س�������Ÿ ����
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	for(UINT i=cur; i<m_nTabCnt-1; i++){
		pMain->m_searchListData[i] = pMain->m_searchListData[i + 1];	// �������ǳ����� �����
	}	
	m_listSearch.DeleteAllItems();	// ����Ʈ�� ����� ����
	UINT nFileCnt = pMain->m_searchListData[cur].nSearchListSize;
	CString strFileName;	// ���ϸ�
	UINT nSize;		// ���ϻ�����
	CString strServerName;	// �����̸�
	CString strServerIP;	// ����������
	CString strServerInfo;	// ��������
	for(i=0; i<nFileCnt; i++){	// ����Ʈ ����
		strFileName = pMain->m_searchListData[m_nTabCnt - 2].searchListData[i].strFileName;
		nSize = pMain->m_searchListData[m_nTabCnt - 2].searchListData[i].nByte;
		strServerName.Format("%s", pMain->m_searchListData[m_nTabCnt - 2].strServerName[i]);	// �����̸�
		strServerIP.Format("%s", pMain->m_searchListData[m_nTabCnt - 2].strServerIP[i]);	// ����������
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
	
	////// ���ڿ��� �ɰ��� ó��
	UINT cnt;
	CString strMessage;
	strMessage.Format("%s", szOut);
	cnt = strMessage.Find('?', 0);		
	
	// �������� ������ �ɰ���
	char comCnt[4];
	if(cnt == 1){	// ���ϰ� ���ڸ��϶�
		comCnt[0] = szOut[0];
		comCnt[1] = '\0';
	}else if(cnt == 2){	// ���ϰ� ���ڸ��϶�
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
			out.Format("%d�� �̻��� ������ �˻��Ҽ� �����ϴ�!", MAX_SEARCH_FILE_COUNT);
			AfxMessageBox(out);
			break;
		}
		// ���ϸ�
		oldCnt = cnt;
		cnt = strMessage.Find('?', oldCnt + 1);						
		searchFile.strFileName = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);

		// ���ϰ��
		oldCnt = cnt;
		cnt = strMessage.Find('?', oldCnt + 1);						
		searchFile.strFilePath = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);

		// �뷮
		oldCnt = cnt;
		cnt = strMessage.Find('?', oldCnt + 1);						
		searchFile.nByte = atoi(strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1));
		
		// �˻������ϳ��� ����, �˻��� ���ϸ��� OnButtonSearch()�� ó��, �˻��ȼ��� AddItem() ���� ó��
		pMain->m_searchListData[m_nTabCnt - 1].searchListData[m_nSearchFileCnt] = searchFile;	
		pMain->m_searchListData[m_nTabCnt - 1].strServerName[m_nSearchFileCnt].Format("%s", szClntName);	// �����̸�
		pMain->m_searchListData[m_nTabCnt - 1].strServerIP[m_nSearchFileCnt].Format("%s", szClntIP);	// ����������
		AddItem(searchFile.strFileName, searchFile.nByte, szClntInfo, TRUE);
	}

	delete szOut;	// ������ �ʿ��� �������� �����ϹǷ� delete

	// ����Ŀ��� �˻��� ������ ������ ���;��ϴµ� �ϳ��� ���̶� �˻��Ȱ� ������ ����
	//if(atoi(comCnt) == 0){
	//	AfxMessageBox("�˻��� ������ �����ϴ�!");
	//}
}

void CSearchView::OnDblclkListSearch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	int intNum = pNMListView->iItem;
	if(m_listSearch.GetItemText(intNum, 0) == ""){	// ����Ÿ�� ���°��� ����Ŭ����
		return;
	}

	UINT cur = m_tabSearch.GetCurSel();
	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
	CMyIOCPSocket *pSock = &(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket);


	CString strIP;
	strIP = pMain->m_searchListData[cur].strServerIP[intNum];
	for(int i=0; i<pSock->m_nClntCount; i++){
		if(strcmp(strIP, inet_ntoa(pSock->m_clntSock[i].pClntData->clntAddr.sin_addr)) == 0){	// ������ ��
			break;
		}
	}
	int cnt = i;	// m_clntSock �� Ŭ���̾�Ʈ ��ȣ
	


	if(pSock->m_nClntCount == cnt){
		AfxMessageBox("������ ã���� �����ϴ�!");
		return;
	}
	
	// �ϴ��� ���⿡ ���������� ��ũ�帮��Ʈ������
	MY_DOWNLOAD_FILE_LIST tempFile;	
	tempFile.downloadList = pMain->m_searchListData[cur].searchListData[intNum];
	tempFile.nReceiveSize = 0;
	tempFile.nSpeed = 0;
	tempFile.strStatus = "�ٿ�ε���";
	tempFile.strServerIP = pMain->m_searchListData[cur].strServerIP[intNum];
	tempFile.strServerName = pMain->m_searchListData[cur].strServerName[intNum];	
	tempFile.nIndex = pMain->m_nDownloadMaxIndex;
	pMain->m_myIOCPSocket.m_nDownloadMaxIndex = pMain->m_nDownloadMaxIndex;	// �׻��� ����ȭ��Ų��
	pMain->m_nDownloadMaxIndex++;
	pMain->m_downloadFileList.AddHead(tempFile);	// ��ũ�帮��Ʈ�� ������

	// IOCP ����
	char szMessage[256];
	sprintf(szMessage, "%s%s", pMain->m_searchListData[cur].searchListData[intNum].strFilePath, pMain->m_searchListData[cur].searchListData[intNum].strFileName);
	if(pSock->SendData(pSock->m_clntSock[cnt].pClntData, pSock->m_pClntIOData[cnt], P_DOWNLOAD_FILE_NOTICE, szMessage, strlen(szMessage), TRUE) == FALSE){
		((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.ErrorHandling(2, "SendData()");		
	}
	

	*pResult = 0;
}


