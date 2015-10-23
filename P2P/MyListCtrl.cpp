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
	if(m_nType == CONNECTDLG  ||  m_nType == UPLOADVIEW  ||  m_nType == DOWNLOADVIEW){	// 접속 다이얼로그, 업로드다운로드뷰
		if(nChar == VK_DELETE){
			MyDeleteItem();	
		}
	}
	
	if(m_nType == SEARCHVIEW){	// 검색뷰
		if(nChar == VK_RETURN){
			MySelectItem();	
		}
	}
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 아이템 삭제 처리
void CMyListCtrl::MyDeleteItem()
{
	LV_ITEM a;
	int Count = this->GetItemCount();
	int i;
	a.mask = LVIF_STATE;
	for(i=Count - 1; i>=0; i--){	// 선택된것이 있을때만 묻는다
		a.iItem = i;
		if(this->GetItemState(i, LVIS_SELECTED) != 0){
			if(MessageBox("정말 삭제하시겠습니까?          \n", NULL, MB_YESNO | MB_ICONEXCLAMATION) == IDYES){
				break;
			}else{
				return;
			}
		}
	}		
	if(m_nType == CONNECTDLG){	// 접속다이얼로그
		for(i=Count - 1; i>=0; i--){
			a.iItem = i;
			if(this->GetItemState(i, LVIS_SELECTED) != 0){
				this->DeleteItem(i);
			}
		}	
	}else if(m_nType == UPLOADVIEW  ||  m_nType == DOWNLOADVIEW){	// 업로드 다운로드뷰(링크드리스트의 값도 삭제해야한다)
		CMainFrame *pMain = ((CMainFrame *)AfxGetMainWnd());
		MY_DOWNLOAD_FILE_LIST tempFile;	// 일단은 여기에 값을저장후
		POSITION pos;
		
		char szNum[5];
		char szStatus[255];
		for(i=Count - 1; i>=0; i--){
			a.iItem = i;
			if(this->GetItemState(i, LVIS_SELECTED) != 0){
				this->GetItemText(i, 4, szStatus, 255);
				if(strcmp(szStatus, "업로드중") == 0  ||  strcmp(szStatus, "다운로드중") == 0){	// 업로드중인거나 다운로드중인것은 삭제 불가
					continue;
				}
				this->GetItemText(i, 6, szNum, 5);
				if(m_nType == UPLOADVIEW){	// 업로드일떄
					pos = pMain->m_uploadFileList.GetHeadPosition();
				}else if(m_nType == DOWNLOADVIEW){	//다운로드일때
					pos = pMain->m_downloadFileList.GetHeadPosition();
				}
				while(pos){
					if(m_nType == UPLOADVIEW){	// 업로드일때
						tempFile = pMain->m_uploadFileList.GetAt(pos);
						if(tempFile.nIndex == (UINT)atoi(szNum)){
							pMain->m_uploadFileList.RemoveAt(pos);	// CList 삭제
							break;
						}
						pMain->m_uploadFileList.GetNext(pos);
					}else if(m_nType == DOWNLOADVIEW){	// 다운로드일때
						tempFile = pMain->m_downloadFileList.GetAt(pos);						
						if(tempFile.nIndex == (UINT)atoi(szNum)){
							pMain->m_downloadFileList.RemoveAt(pos);	// CList 삭제
							break;
						}
						pMain->m_downloadFileList.GetNext(pos);
					}
				}				
				this->DeleteItem(i);	// 리스트컨트롤 삭제
			}
		}// for()

		/**	 시간이 없어서 손보지 못함 , 리스트 삭제시 스테틱컨트롤의 내용도 바뀌게
		// static 업데이트
		CString strState;
		if(m_nType == UPLOADVIEW){	// 업로드일때			
			strState.Format("Uploads (%d/%d)", pMain->m_myIOCPSocket.m_uploadFileDataList.GetCount(), pMain->GetActiveView()->m_listUploads.GetItemCount());
			pMain->GetActiveView()->m_staticState.SetWindowText(strState);
		}else if(m_nType == DOWNLOADVIEW){	// 다운로드일때
			strState.Format("Downloads (%d/%d)", m_myIOCPSocket.m_DownloadFileDataList.GetCount(), ((CDownloadsView *)m_pNewView)->m_listDownloads.GetItemCount());
			((CDownloadsView *)m_pNewView)->m_staticState.SetWindowText(strState);
		}
		**/
	}



	
	





}



// 검색에서 리스트목록을 다운로드목록으로 전송
void CMyListCtrl::MySelectItem()
{
	AfxMessageBox("이부분에서 다운로드목록으로 전송");
}
