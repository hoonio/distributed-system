// DFTransferStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFTransferStatusDlg.h"
#include "DFHideMainFrame.h"

extern DFHideMainFrame* g_pMainFrame;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFTransferStatusDlg dialog


CDFTransferStatusDlg::CDFTransferStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDFTransferStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDFTransferStatusDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDFTransferStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDFTransferStatusDlg)
	DDX_Control(pDX, IDC_SEND_PROGRESS_LIST, m_SendStatusList);
	DDX_Control(pDX, IDC_RECV_PROGRESS_LIST, m_RecvStatusList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDFTransferStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CDFTransferStatusDlg)
	ON_BN_CLICKED(IDC_ABORT_RECV, OnAbortRecv)
	ON_BN_CLICKED(IDC_ABORT_SEND, OnAbortSend)
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFTransferStatusDlg message handlers

void CDFTransferStatusDlg::OnAbortRecv() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_RecvStatusList.GetFirstSelectedItemPosition();
	while(pos)
	{
		CString strItem;
		int nItem = m_RecvStatusList.GetNextSelectedItem(pos);
		strItem = m_RecvStatusList.GetItemText(nItem, 0);
		m_RecvStatusList.DeleteItem(nItem);

		POSITION pos2 = g_pMainFrame->m_DownList.GetHeadPosition();
		while(pos2)
		{
			POSITION temppos = pos2;
			DFFileRecver* pRecver = (DFFileRecver*)g_pMainFrame->m_DownList.GetNext(pos2);
			if(pRecver && lstrcmp(pRecver->m_XferStatus.tszFileName, (LPCTSTR)strItem) == 0)
			{
				g_pMainFrame->m_DownList.RemoveAt(temppos);
				break;
			}
		}
	}	
}

void CDFTransferStatusDlg::OnAbortSend() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_SendStatusList.GetFirstSelectedItemPosition();
	while(pos)
	{
		CString strItem;
		int nItem = m_SendStatusList.GetNextSelectedItem(pos);
		strItem = m_SendStatusList.GetItemText(nItem, 0);
		m_SendStatusList.DeleteItem(nItem);

		POSITION pos2 = g_pMainFrame->m_UpList.GetHeadPosition();
		while(pos2)
		{
			POSITION temppos = pos2;
			DFFileSender* pSender = (DFFileSender*)g_pMainFrame->m_UpList.GetNext(pos2);
			if(pSender && lstrcmp(pSender->m_XferStatus.tszFileName, (LPCTSTR)strItem) == 0)
			{
				g_pMainFrame->m_UpList.RemoveAt(temppos);
				break;
			}
		}
	}
}

BOOL CDFTransferStatusDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ASSERT(this);

	m_SendStatusList.InsertColumn(0, TEXT("FileName"), LVCFMT_CENTER, 100);
	m_SendStatusList.InsertColumn(1, TEXT("State"), LVCFMT_CENTER, 70);
	m_SendStatusList.InsertColumn(2, TEXT("UserID"), LVCFMT_CENTER, 100);
	m_SendStatusList.InsertColumn(3, TEXT("Total Size"), LVCFMT_CENTER, 80);
	m_SendStatusList.InsertColumn(4, TEXT("Xfered Size"), LVCFMT_CENTER, 80);
	m_SendStatusList.InsertColumn(5, TEXT("Ratio(%)"), LVCFMT_CENTER, 60);

	m_RecvStatusList.InsertColumn(0, TEXT("FileName"), LVCFMT_CENTER, 100);
	m_RecvStatusList.InsertColumn(1, TEXT("State"), LVCFMT_CENTER, 70);
	m_RecvStatusList.InsertColumn(2, TEXT("UserID"), LVCFMT_CENTER, 100);
	m_RecvStatusList.InsertColumn(3, TEXT("Total Size"), LVCFMT_CENTER, 80);
	m_RecvStatusList.InsertColumn(4, TEXT("Xfered Size"), LVCFMT_CENTER, 80);
	m_RecvStatusList.InsertColumn(5, TEXT("Ratio(%)"), LVCFMT_CENTER, 60);

	RefreshSendList();
	RefreshRecvList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDFTransferStatusDlg::RefreshSendList()
{
	m_SendStatusList.DeleteAllItems();

	if(g_pMainFrame->m_UpList.IsEmpty()) 
	{
		GetDlgItem(IDC_ABORT_SEND)->EnableWindow(FALSE);
		return;
	}
	else 
	{
		GetDlgItem(IDC_ABORT_SEND)->EnableWindow();	
	}

	POSITION pos = g_pMainFrame->m_UpList.GetHeadPosition();
	while(pos)
	{
		PXFER_STATUS pStatus = (PXFER_STATUS)g_pMainFrame->m_UpList.GetNext(pos);
		if(pStatus)
		{
			CString strState;
			CString strSize, strXferSize, strRatio;

			switch(pStatus->dwState)
			{
			case STATE_CLOSED:
				strState.Format(TEXT("NotConnected"));break;
			case STATE_CONNECTED:
				strState.Format(TEXT("Connected"));break;
			case STATE_XFERING:
				strState.Format(TEXT("Xfering"));break;
			case STATE_FINISHED:	
				strState.Format(TEXT("Finished"));break;
			case STATE_ERROR:		
				strState.Format(TEXT("ERROR"));break;
			}

			strSize.Format(TEXT("%d"), pStatus->dwFileSize);
			strXferSize.Format(TEXT("%d"), pStatus->dwXferedSize);
			strRatio.Format(TEXT("%d"), (pStatus->dwXferedSize*100 / pStatus->dwFileSize));

			int i = m_SendStatusList.GetItemCount();
			m_SendStatusList.InsertItem(i, pStatus->tszFileName);
			m_SendStatusList.SetItemText(i, 1, strState);
			m_SendStatusList.SetItemText(i, 2, pStatus->tszUserID);
			m_SendStatusList.SetItemText(i, 3, strSize);
			m_SendStatusList.SetItemText(i, 4, strXferSize);
			m_SendStatusList.SetItemText(i, 5, strRatio);
		}
	}
}


void CDFTransferStatusDlg::RefreshRecvList()
{
	m_RecvStatusList.DeleteAllItems();

	if(g_pMainFrame->m_DownList.IsEmpty()) 
	{
		GetDlgItem(IDC_ABORT_RECV)->EnableWindow(FALSE);
		return;
	}
	else 
	{
		GetDlgItem(IDC_ABORT_RECV)->EnableWindow();	
	}

	POSITION pos = g_pMainFrame->m_DownList.GetHeadPosition();
	while(pos)
	{
		PXFER_STATUS pStatus = (PXFER_STATUS)g_pMainFrame->m_DownList.GetNext(pos);
		if(pStatus)
		{
			CString strState;
			CString strSize, strXferSize, strRatio;

			switch(pStatus->dwState)
			{
			case STATE_CLOSED:
				strState.Format(TEXT("NotConnected"));break;
			case STATE_CONNECTED:
				strState.Format(TEXT("Connected"));break;
			case STATE_XFERING:
				strState.Format(TEXT("Xfering"));break;
			case STATE_FINISHED:	
				strState.Format(TEXT("Finished"));break;
			case STATE_ERROR:		
				strState.Format(TEXT("ERROR"));break;
			}

			strSize.Format(TEXT("%d"), pStatus->dwFileSize);
			strXferSize.Format(TEXT("%d"), pStatus->dwXferedSize);
			ASSERT(pStatus->dwFileSize != 0);
			strRatio.Format(TEXT("%d"), (pStatus->dwXferedSize*100 / pStatus->dwFileSize));

			int i = m_RecvStatusList.GetItemCount();
			m_RecvStatusList.InsertItem(i, pStatus->tszFileName);
			m_RecvStatusList.SetItemText(i, 1, strState);
			m_RecvStatusList.SetItemText(i, 2, pStatus->tszUserID);
			m_RecvStatusList.SetItemText(i, 3, strSize);
			m_RecvStatusList.SetItemText(i, 4, strXferSize);
			m_RecvStatusList.SetItemText(i, 5, strRatio);
		}
	}
}

void CDFTransferStatusDlg::UpdateSendItem(PXFER_STATUS pStatus)
{
	// 해당 정보를 갖고 있는 아이템을 리스트컨트롤에서 찾아서 갱신한다.
	LVFINDINFO info;
	int nIndex;
	CString strState;
	CString strSize, strXferSize, strRatio;

	info.flags = LVFI_STRING;
	info.psz = pStatus->tszFileName;
	
	nIndex = m_SendStatusList.FindItem(&info);
	if(nIndex != -1)
	{
		switch(pStatus->dwState)
		{
		case STATE_CLOSED:
			strState.Format(TEXT("NotConnected"));break;
		case STATE_CONNECTED:
			strState.Format(TEXT("Connected"));break;
		case STATE_XFERING:
			strState.Format(TEXT("Xfering"));break;
		case STATE_FINISHED:	
			strState.Format(TEXT("Finished"));break;
		case STATE_ERROR:		
			strState.Format(TEXT("ERROR"));break;
		}

		strSize.Format(TEXT("%d"), pStatus->dwFileSize);
		strXferSize.Format(TEXT("%d"), pStatus->dwXferedSize);
		strRatio.Format(TEXT("%d"), (pStatus->dwXferedSize*100 / pStatus->dwFileSize));
		m_SendStatusList.SetItemText(nIndex, 1, strState);
		m_SendStatusList.SetItemText(nIndex, 2, pStatus->tszUserID);
		m_SendStatusList.SetItemText(nIndex, 3, strSize);
		m_SendStatusList.SetItemText(nIndex, 4, strXferSize);
		m_SendStatusList.SetItemText(nIndex, 5, strRatio);
	}
}

void CDFTransferStatusDlg::UpdateRecvItem(PXFER_STATUS pStatus)
{
	// 해당 정보를 갖고 있는 아이템을 리스트컨트롤에서 찾아서 갱신한다.
	LVFINDINFO info;
	int nIndex;
	CString strState;
	CString strSize, strXferSize, strRatio;

	info.flags = LVFI_STRING;
	info.psz = pStatus->tszFileName;
	
	nIndex = m_RecvStatusList.FindItem(&info);
	if(nIndex != -1)
	{
		switch(pStatus->dwState)
		{
		case STATE_CLOSED:
			strState.Format(TEXT("NotConnected"));break;
		case STATE_CONNECTED:
			strState.Format(TEXT("Connected"));break;
		case STATE_XFERING:
			strState.Format(TEXT("Xfering"));break;
		case STATE_FINISHED:	
			strState.Format(TEXT("Finished"));break;
		case STATE_ERROR:		
			strState.Format(TEXT("ERROR"));break;
		}

		strSize.Format(TEXT("%d"), pStatus->dwFileSize);
		strXferSize.Format(TEXT("%d"), pStatus->dwXferedSize);
		strRatio.Format(TEXT("%d"), (pStatus->dwXferedSize*100 / pStatus->dwFileSize));
		m_RecvStatusList.SetItemText(nIndex, 1, strState);
		m_RecvStatusList.SetItemText(nIndex, 2, pStatus->tszUserID);
		m_RecvStatusList.SetItemText(nIndex, 3, strSize);
		m_RecvStatusList.SetItemText(nIndex, 4, strXferSize);
		m_RecvStatusList.SetItemText(nIndex, 5, strRatio);
	}
}

BOOL CDFTransferStatusDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	// TODO: Add your message handler code here and/or call default
	// 송수신에 대한 정보가 새로 왔다. 업데이트 시키자!!!

	switch(pCopyDataStruct->dwData)
	{
	case 0:
		UpdateRecvItem((PXFER_STATUS)pCopyDataStruct->lpData);
		break;
	case 1:
		UpdateSendItem((PXFER_STATUS)pCopyDataStruct->lpData);
		break;
	default:
		break;
	}
	
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}
