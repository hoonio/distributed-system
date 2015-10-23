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
	m_listUploads.m_nType = UPLOADVIEW;	// ����Ʈ�� ������ ��ɸ��� ó���Ҽ� �ִ� Ÿ�԰�

	// static ��Ʈ�� ���
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

	m_listUploads.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// ���ٳ����� ����
	
	/**
	// ���� ����Ÿ
	CString fileName[] = {"Directory.ico", "toolbar20.bmp", "�����غ�����ŷ����.zip"};
	UINT size[] = {123, 1200, 91210};
	CString serverName[] = {"�ǿ��� [123.122.322.124]", "������ [125.32.22.124]", "�ڿ츲�������� [23.242.322.24]"};
	UINT speed[] = {1231, 22, 911};
	for(int i=0; i<3; i++){		
		AddItem(fileName[i], size[i], serverName[i], speed[i], "testFolder");
	}
	**/

	CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());

	pMain->GetSysImgList(m_listUploads, m_imgSmallList);	// �ý��۾����ܾ��

	// ��ũ�帮��Ʈ�� �ִ� �� ����Ʈ��Ʈ�ѿ� �Ѹ�
	MY_DOWNLOAD_FILE_LIST tempFile;	// �ϴ��� ���⿡ ����������
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

	// static ������Ʈ
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
	a.iItem = m_listUploads.GetItemCount();	// ���� ��ġ
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// ���������� ǥ���ɰ�
	a.iSubItem = 0;	// ���ε���
	a.iImage = img;	// �̹��� �ε���
	a.stateMask = LVIS_STATEIMAGEMASK;	// ���º�ȭ�� Mask ó��
	a.state = INDEXTOSTATEIMAGEMASK(1);	// ��ȿ�� ���� ��Ʈ
	fileName.Format("%s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// ���ڿ� 
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
	m_listUploads.SetItemText(a.iItem, 6, temp);	// �ǹ̴� ������ ���ݴ� ���ϰ� �ϱ����ع�ȣ�� �ο�(�����ٿ�޴°Ͱ���������)

/**
	LV_ITEM a;
	a.iItem = m_listUploads.GetItemCount();	// ���� ��ġ
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// ���������� ǥ���ɰ�
	a.iSubItem = 0;	// ���ε���
//	a.iImage = image;	// �̹��� �ε���
	a.stateMask = LVIS_STATEIMAGEMASK;	// ���º�ȭ�� Mask ó��
	a.state = INDEXTOSTATEIMAGEMASK(1);	// ��ȿ�� ���� ��Ʈ
	fileName.Format("[icon] %s", fileName);
	a.pszText = (LPSTR)(LPCTSTR) fileName;	// ���ڿ� 
	CString temp;
	m_listUploads.InsertItem(&a);	
	temp.Format("%d KB", size);
	m_listUploads.SetItemText(a.iItem, 1, temp);
	temp.Format("%d KB", rand() % 100);
	m_listUploads.SetItemText(a.iItem, 2, temp);
	m_listUploads.SetItemText(a.iItem, 3, "5% ����");
	m_listUploads.SetItemText(a.iItem, 4, "���¾�����");
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
	if(pMain->m_myIOCPSocket.m_uploadFileDataList.GetCount() > 0){	// �ٿ�ε����϶���
		// �ٿ�ε�Ǵ¸�ϰ� �����ٿ�ǰ��ִ¸���� �� (��ø while)
		// �����ٿ�ǰ��ִ°�
		POSITION posIOCP;	
		posIOCP = pMain->m_myIOCPSocket.m_uploadFileDataList.GetHeadPosition();
		MY_FILE_DATA *pIOCP;

		// �ٿ�ε��ϵ���Ÿ
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
					for(int i=0; i<m_listUploads.GetItemCount(); i++){	// ����Ʈ��ϰ� ��ȣ��
						char szNum[5];
						m_listUploads.GetItemText(i, 6, szNum, 5);						
						if(pIOCP->nIndex == (UINT)atoi(szNum)){
							// ��������Ÿ��		
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
	if(0 < m_listUploads.GetSelectedCount()){	// �����������ø�
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
