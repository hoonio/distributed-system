// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "P2p.h"
#include "MyListCtrl.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers

void CMyListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_nType == CONNECTDLG  ||  m_nType == UPLOADVIEW  ||  m_nType == DOWNLOADVIEW){	// ���� ���̾�α�, ���ε�ٿ�ε��
		if(nChar == VK_DELETE){
			MyDeleteItem();	
		}
	}
	
	if(m_nType == SEARCHVIEW){	// �˻���
		if(nChar == VK_RETURN){
			MySelectItem();	
		}
	}
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ������ ���� ó��
void CMyListCtrl::MyDeleteItem()
{
	LV_ITEM a;
	int Count = this->GetItemCount();
	int i;
	a.mask = LVIF_STATE;
	for(i=Count - 1; i>=0; i--){	// ���õȰ��� �������� ���´�
		a.iItem = i;
		if(this->GetItemState(i, LVIS_SELECTED) != 0){
			if(MessageBox("���� �����Ͻðڽ��ϱ�?          \n", NULL, MB_YESNO | MB_ICONEXCLAMATION) == IDYES){
				break;
			}else{
				return;
			}
		}
	}		
	if(m_nType == CONNECTDLG){	// ���Ӵ��̾�α�
		for(i=Count - 1; i>=0; i--){
			a.iItem = i;
			if(this->GetItemState(i, LVIS_SELECTED) != 0){
				this->DeleteItem(i);
			}
		}	
	}else if(m_nType == UPLOADVIEW  ||  m_nType == DOWNLOADVIEW){	// ���ε� �ٿ�ε��(��ũ�帮��Ʈ�� ���� �����ؾ��Ѵ�)
		CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
		MY_DOWNLOAD_FILE_LIST tempFile;	// �ϴ��� ���⿡ ����������
		POSITION pos;
		
		char szNum[5];
		char szStatus[255];
		for(i=Count - 1; i>=0; i--){
			a.iItem = i;
			if(this->GetItemState(i, LVIS_SELECTED) != 0){
				this->GetItemText(i, 4, szStatus, 255);
				if(strcmp(szStatus, "���ε���") == 0  ||  strcmp(szStatus, "�ٿ�ε���") == 0){	// ���ε����ΰų� �ٿ�ε����ΰ��� ���� �Ұ�
					continue;
				}
				this->GetItemText(i, 6, szNum, 5);
				if(m_nType == UPLOADVIEW){	// ���ε��ϋ�
					pos = pMain->m_uploadFileList.GetHeadPosition();
				}else if(m_nType == DOWNLOADVIEW){	//�ٿ�ε��϶�
					pos = pMain->m_downloadFileList.GetHeadPosition();
				}
				while(pos){
					if(m_nType == UPLOADVIEW){	// ���ε��϶�
						tempFile = pMain->m_uploadFileList.GetAt(pos);
						if(tempFile.nIndex == (UINT)atoi(szNum)){
							pMain->m_uploadFileList.RemoveAt(pos);	// CList ����
							break;
						}
						pMain->m_uploadFileList.GetNext(pos);
					}else if(m_nType == DOWNLOADVIEW){	// �ٿ�ε��϶�
						tempFile = pMain->m_downloadFileList.GetAt(pos);						
						if(tempFile.nIndex == (UINT)atoi(szNum)){
							pMain->m_downloadFileList.RemoveAt(pos);	// CList ����
							break;
						}
						pMain->m_downloadFileList.GetNext(pos);
					}
				}				
				this->DeleteItem(i);	// ����Ʈ��Ʈ�� ����
			}
		}// for()

		/**	 �ð��� ��� �պ��� ���� , ����Ʈ ������ ����ƽ��Ʈ���� ���뵵 �ٲ��
		// static ������Ʈ
		CString strState;
		if(m_nType == UPLOADVIEW){	// ���ε��϶�			
			strState.Format("Uploads (%d/%d)", pMain->m_myIOCPSocket.m_uploadFileDataList.GetCount(), pMain->GetActiveView()->m_listUploads.GetItemCount());
			pMain->GetActiveView()->m_staticState.SetWindowText(strState);
		}else if(m_nType == DOWNLOADVIEW){	// �ٿ�ε��϶�
			strState.Format("Downloads (%d/%d)", m_myIOCPSocket.m_DownloadFileDataList.GetCount(), ((CDownloadsView *)m_pNewView)->m_listDownloads.GetItemCount());
			((CDownloadsView *)m_pNewView)->m_staticState.SetWindowText(strState);
		}
		**/
	}



	
	





}



// �˻����� ����Ʈ����� �ٿ�ε������� ����
void CMyListCtrl::MySelectItem()
{
	AfxMessageBox("�̺κп��� �ٿ�ε������� ����");
}
