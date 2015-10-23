// ServerView.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "ServerView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerView

IMPLEMENT_DYNCREATE(CServerView, CFormView)

CServerView::CServerView()
	: CFormView(CServerView::IDD)
{
	//{{AFX_DATA_INIT(CServerView)
	m_strLog = _T("");
	//}}AFX_DATA_INIT
}

CServerView::~CServerView()
{
}

void CServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerView)
	DDX_Control(pDX, IDC_STATIC_STATE, m_staticState);
	DDX_Control(pDX, IDC_LIST_SELECT, m_listSelect);
	DDX_Control(pDX, IDC_LIST_SERVER, m_listServer);
	DDX_Text(pDX, IDC_EDIT_LOG, m_strLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerView, CFormView)
	//{{AFX_MSG_MAP(CServerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CServerView message handlers

void CServerView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CString state;
	state.Format("%s", "Server (2/10)");
	m_staticState.SetWindowText(state);

	// listCtrl, �������
	m_listServer.InsertColumn(0, "Server Name", LVCFMT_CENTER, 100);
	m_listServer.InsertColumn(1, "Server IP", LVCFMT_LEFT, 100);
	m_listServer.InsertColumn(2, "View Folder", LVCFMT_LEFT, 83);
	//m_listServer.InsertColumn(3, "Progress", LVCFMT_LEFT, 150);
	m_listServer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// ���ٳ����� ����

	// listCtrl, �����Ѹ��
	m_listSelect.InsertColumn(0, "File Name", LVCFMT_CENTER, 267);
	m_listSelect.InsertColumn(1, "Size", LVCFMT_LEFT, 90);	
	m_listSelect.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	// ���ٳ����� ����

	// test Log
	m_strLog = "12:53:26: DonkeyServer No6�� ���� �� (62.241.53.15:4242)...";
	m_strLog += "\r\n";
	m_strLog += "12:53:26: !-= www.FreeOsex.com =-!   �� ���� �� (83.149.117.56:4321)...";
	m_strLog += "\r\n";
	m_strLog += "12:53:42: !-= www.FreeOsex.com =-!    (83.149.117.56:4321)�� �����, �α��� ��û�� �����ϴ�";
	m_strLog += "\r\n";
	m_strLog += "12:53:43: ���ӵ�: !-= www.FreeOsex.com =-!   ";
	m_strLog += "\r\n";
	UpdateData(false);

	
	// ���ӵǾ��ִ� Ŭ���̾�Ʈ�� ����� �߰�
	int count = ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_nClntCount;
	char szClntName[256];
	char szClntIP[20];
	for(int i=0; i<count; i++){
		strcpy(szClntName, ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[i].clntName);
		strcpy(szClntIP, inet_ntoa(((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[i].pClntData->clntAddr.sin_addr));
		AddServerListItem(szClntName, szClntIP, ((CMainFrame *)AfxGetMainWnd())->m_myIOCPSocket.m_clntSock[i].bViewFolder);
	}
}

void CServerView::AddServerListItem(CString strSeverName, CString strServerIP, BOOL bViewFolder)
{
	LV_ITEM a;
	a.iItem = m_listServer.GetItemCount();	// ���� ��ġ
	a.mask = LVIF_TEXT | LVIF_IMAGE  | LVIF_STATE;	// ���������� ǥ���ɰ�
	a.iSubItem = 0;	// ���ε���
	//a.iImage = image;	// �̹��� �ε���, 0���� ���ӵ��̹���
	a.stateMask = LVIS_STATEIMAGEMASK;	// ���º�ȭ�� Mask ó��
	a.state = INDEXTOSTATEIMAGEMASK(1);	// ��ȿ�� ���� ��Ʈ
	//name.Format(" %s", name);
	a.pszText = (LPSTR)(LPCTSTR)strSeverName;	//(LPSTR)(LPCTSTR) name;	// ���ڿ� 	
	m_listServer.InsertItem(&a);	
	m_listServer.SetItemText(a.iItem, 1, strServerIP);
	CString temp;
	temp.Format("%s", (bViewFolder == 1 ? "YES" : "NO"));
	m_listServer.SetItemText(a.iItem, 2, temp);
}




