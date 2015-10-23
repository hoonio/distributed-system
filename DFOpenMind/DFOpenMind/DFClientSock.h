#if !defined(AFX_DFCLIENTSOCK_H__780611A3_515E_4DB5_84E7_8CDF1162B0F4__INCLUDED_)
#define AFX_DFCLIENTSOCK_H__780611A3_515E_4DB5_84E7_8CDF1162B0F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFClientSock.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// DFClientSock command target

class DFClientSock : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	DFClientSock();
	DFClientSock(HWND hWnd);
	virtual ~DFClientSock();

	HWND	m_hWnd;
	BOOL	m_bCanSend;

	int		SendMsg(BYTE* pBuf, UINT nSize, BOOL bNew = TRUE);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFClientSock)
	public:
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(DFClientSock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFCLIENTSOCK_H__780611A3_515E_4DB5_84E7_8CDF1162B0F4__INCLUDED_)
