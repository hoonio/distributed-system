// DFClientSock.cpp : implementation file
//

#include "stdafx.h"
#include "DFOpenMind.h"
#include "DFClientSock.h"
#include "DFHideMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DFClientSock

DFClientSock::DFClientSock()
{
	m_hWnd = NULL;
}

DFClientSock::~DFClientSock()
{

}

DFClientSock::DFClientSock(HWND hWnd)
{
	m_hWnd = hWnd;
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(DFClientSock, CAsyncSocket)
	//{{AFX_MSG_MAP(DFClientSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// DFClientSock member functions

void DFClientSock::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	MessageBox(m_hWnd, TEXT("Connection Closed"), TEXT("Connection"), MB_OK|MB_ICONINFORMATION);
	((DFHideMainFrame*)(((CDFOpenMindApp*)AfxGetApp())->m_pMainWnd))->OnCloseSock();
	CAsyncSocket::OnClose(nErrorCode);
}

void DFClientSock::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	int iZero = 2048;
	SetSockOpt(SO_RCVBUF, &iZero, sizeof(iZero));

	((DFHideMainFrame*)(((CDFOpenMindApp*)AfxGetApp())->m_pMainWnd))->LogonProcess();
	CAsyncSocket::OnConnect(nErrorCode);
}

void DFClientSock::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	static BYTE MsgBuf[40960] = {0,};
	static BYTE* pLoc = MsgBuf;
	static UINT nRecvBufSize = 40960;
	static UINT nTotalRead = 0;

	DWORD dwRead = Receive(pLoc, nRecvBufSize);
	if(dwRead == SOCKET_ERROR)
	{
		if(GetLastError() != WSAEWOULDBLOCK) 
		{
			Close();
			return ;
		}
	}

	pLoc += dwRead;
	nRecvBufSize -= dwRead;
	nTotalRead += dwRead;

	if(nTotalRead >= 2)
	{
		WORD wdMsgSize = *(WORD*)MsgBuf;
		if(nTotalRead >= (DWORD)wdMsgSize+2)
		{
//			LOGOUT("CLIENT: 1 Msg Recv Complete, Process It!! Type : %d", MsgBuf[2]);

			// 하나의 메시지를 처리한다!!
			((DFHideMainFrame*)(((CDFOpenMindApp*)AfxGetApp())->m_pMainWnd))
				->RecvMsg(MsgBuf+2, wdMsgSize);
			
			// 버퍼 이동~!
			nTotalRead -= (wdMsgSize + 2);
			pLoc -= (wdMsgSize + 2);
			nRecvBufSize += (wdMsgSize + 2);
			memmove(MsgBuf, MsgBuf+wdMsgSize+2, nTotalRead);
		}
	}	
	CAsyncSocket::OnReceive(nErrorCode);
}

void DFClientSock::OnSend(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bCanSend = TRUE;
	SendMsg(NULL, 0, FALSE);
	CAsyncSocket::OnSend(nErrorCode);
}

int DFClientSock::SendMsg(BYTE* pBuf, UINT nSize, BOOL bNew)
{
	static BYTE MsgBuf[40960] = {0,};
	static BYTE* pLoc = MsgBuf;
	static UINT	nSendSize = 0;
	
	if(bNew)
	{
		// 버퍼를 넘어서는 경우 ㅡㅜ
		if(MsgBuf+40960 <= pLoc+nSize) return 0;

		*(pLoc)		= LOBYTE(nSize);
		*(pLoc+1)	= HIBYTE(nSize);

		memcpy(pLoc+2, pBuf, nSize);
		pLoc += (nSize+2);
		nSendSize += (nSize+2);
	}

	ASSERT(this);

	if(m_bCanSend)
	{
		while(nSendSize != 0)
		{
			DWORD dwSent = Send(MsgBuf, nSendSize);
			if(dwSent == SOCKET_ERROR)
			{
				if(GetLastError() != WSAEWOULDBLOCK) 
				{
					Close();
					return FALSE;
				}
				m_bCanSend = FALSE;

				pLoc += dwSent; nSendSize -= dwSent;
				memmove(MsgBuf, pLoc, nSendSize);
				pLoc = MsgBuf; 
				break;
			}
			else
			{
				pLoc += dwSent;
				nSendSize -= dwSent;
			}
		}

		// 송신 완료 했으므로 버퍼와 사이즈를 초기화한다.
		pLoc = MsgBuf;
		nSendSize = 0;
	}
	
	return 1;
}

