////////////////////////////////////////////////////////////////////
///
///		2005-07-26
///		IOCP SOCKET
///		Kwon Yong Jae / amadeusj@hanmail.net
///
////////////////////////////////////////////////////////////////////


// MyIOCPSocket.h: interface for the CMyIOCPSocket class.
//
//////////////////////////////////////////////////////////////////////

///// ws2_32.lib => link

#if !defined(AFX_MYIOCPSOCKET_H__18807975_3C06_4757_82F8_8A5E861D64B5__INCLUDED_)
#define AFX_MYIOCPSOCKET_H__18807975_3C06_4757_82F8_8A5E861D64B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <winsock2.h>
#include <Afxtempl.h>	// CList


#define BUFSIZE 8192		// ������ �ްų� ������ ���Ǵ� ���� ũ��, 200���ٴ� Ŀ����
#define MYPORT 2000
#define LIMITSOCKET 30	// Ŭ���̾�Ʈ �ִ밹���� 30��
#define FILENAME_LENGTH 100	// ���ϸ�

// ��Ŷ���
#define P_TEMP					99	// test
#define P_FILE_HEADER			10	// �����̸�, ���ϻ�����
#define P_FILE_MAIN				11	// ���ϳ���
#define P_FILE_TAIL				12	// �������� �Ϸ�
#define P_FIRST_CONNECT			13	// �ڽ��� ó�� ������ �����ϴ°��̶�� �˸� (�����κ��� �ٸ��ĵ��� �����Ǹ� ����)
#define P_NOFIRST_CONNECT		14	// �ڽ��� ó�� �ƴ϶� �ٸ��������� �����Ǹ� �޾Ƽ� ������ �����ϴ°��̶�� �˸� (�ٸ��Ŀ� ���Ӹ� ��)
#define P_SEND_ALL_IP			15	// �ٸ��ĵ��� �����Ǹ� ����
#define P_SEARCH_FILE			16	// ���ϰ˻�
#define P_SEARCH_FILE_LIST		17	// ���ϰ˻��� ��������Ÿ
#define P_DOWNLOAD_FILE_NOTICE	18	// �ٿ�ε�ޱ����� ������ ����
#define P_DOWNLOAD_FILE_ERROR	19	// �ٿ�ε������� ã���� ����
#define P_SEND_FILE_READY		20	// �����������ϱ������غ�(��������û������ ������ ������쿡�� P_DOWNLOAD_FILE_ERROR �� ����������)
#define P_SEND_FILE				21	// ��������

// �۽����� �������� ����
#define SENDDATA	0	// MY_IO_DATA ���� ���
#define RECVDATA	1


///////////////////////////////////////////
// file �˻��� ���
typedef struct
{
	CString strFileName;
	CString strFilePath;
	DWORD nByte;
}MY_SEARCH_FILE;











// Ŭ���̾�Ʈ ����, CompletionPort ���� �Ķ����
typedef struct 
{
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;	
	UINT index;	// �迭�� ����ü�� �����Ҷ� index ����
}MY_CLIENT_DATA;


// Ŭ���̾�Ʈ ������
typedef struct
{
	MY_CLIENT_DATA *pClntData;
	CHAR clntName[256];	// �����̸�
	BOOL bViewFolder;	// ���� ���������ִ��� ����
}MY_CLIENT_DETAIL_DATA;



typedef struct 
{
	UINT nPacketHeader;
	DWORD nTotalByte;	// ��ũ��
	char buffer[BUFSIZE + 1];	// ���� ����Ÿ�� ����ִ°���, NULL ����  + 1	
}MY_PACKET;



// ��Ŷ���۽û��(��ü����Ÿ)
typedef struct
{
	char *pszFileSearchData;	// ���ϰ˻�����Ÿ���۽� ���
	char *pszTotalIPData;	// ó�����ӽ� ������� �����ǿ� ���
}MY_SEND_DATA;


// ��Ŷ������ ���(��ü����Ÿ)
typedef struct
{
	char *pszFileSearchData;	// ���ϰ˻�����Ÿ���۽� ���
	char *pszTotalIPData;	// ó�����ӽ� ������� �����ǿ� ���
}MY_RECV_DATA;


// ������ ���� ������ ����üȭ.
typedef struct 
{
	OVERLAPPED overlapped;
	MY_PACKET sendData;	// �ѹ��� �������� ��Ŷ
	MY_PACKET recvData;
	WSABUF wsaBuf;			// wsaBuf.buf �� ���� ����Ÿ �ּҰ� ���ִ�( &data)	
	BOOL flag;	// �������� �۽����� ����, GetQueuedCompletionStatus() ���� ���� �۽� ��� �����ϹǷ� 	
	DWORD nSendByte;	// �ڽ��� ���� ����Ÿ��	
	MY_SEND_DATA mySendData;	// ���� �ѵ���Ÿ
	MY_RECV_DATA myRecvData;
	UINT nDownloadFileIndex;	// ���ϴٿ�ÿ��� ���
	UINT nUploadFileIndex;	// ���Ͼ��ε�ÿ��� ���
	//DWORD nRecvByte;	// �ڽ��� ���� ����Ÿ��
}MY_IO_DATA;



// �� �ٿ�ε� ������ ����
typedef struct
{
	UINT nIndex;	// �ε�����ȣ(�ٿ�ε�信������ ����������)
	CString strFileFullName;	// �����ǰ�ι� �̸�
	DWORD nFileSize;	// ���ϻ�����
	DWORD nReceiveSize;	// �ٿ���� ������, ���ε��ѻ�����
	CFile *pFile;	// ����������
}MY_FILE_DATA;



/////////////////////////////////////////////





/************** Queue ********************/
#define MAXQUEUE 50

class CMyQueue  
{
public:
	CMyQueue();
	virtual ~CMyQueue();
	int CMyQueue::Push(MY_PACKET data);
	int CMyQueue::Pop(MY_PACKET *data);

	MY_PACKET queue[MAXQUEUE];
	char rear;
	char front;

};
/***********************************/






class CMyIOCPSocket
{
	public:
		CList<MY_FILE_DATA, MY_FILE_DATA&> m_uploadFileDataList;	// ���ε��ϴ� ������ ����
		CList<MY_FILE_DATA, MY_FILE_DATA&> m_downloadFileDataList;	// �ٿ�޴� ������ ����
		UINT m_nDownloadMaxIndex;	// �����ε�����ȣ(�ٿ�ε��ϰ� �����ٿ��������Ϥ����� ����) �̹�ȣ�� �����ѹ�ȣ�� ����ū��ȣ
		UINT m_nUploadMaxIndex;	// �����ε�����ȣ(���ε��ϰ� �������ε��������Ϥ����� ����) �̹�ȣ�� �����ѹ�ȣ�� ����ū��ȣ
		
		//CMyQueue myQueue;	// ����Ÿ �޾Ƽ� ť���� ����, ����� �ʿ����
		BOOL FindAlltFile(char* pszPath, char *pszName, CList<MY_SEARCH_FILE , MY_SEARCH_FILE& >*pFileList);
		void ErrorHandling(UINT errorCode, CString strPrn);
		CString m_strSevrIP;
		BOOL SendData(MY_CLIENT_DATA *, MY_IO_DATA *, UINT, char *, DWORD nTotalByte, BOOL bFirst);
		BOOL RecvData(MY_CLIENT_DATA *, MY_IO_DATA *);
		void CloseClntSock(MY_CLIENT_DATA *pClntData, MY_IO_DATA *pIOData);	// Ŭ���̾�Ʈ�� ����������
		void CloseSevrSock();	// �ڽ��� ����������
		BOOL ClientConnect(CString strIp);	// �ڽ��� Ŭ���̾�Ʈ�� ����������
		BOOL ServerConnect();	// �ڽ��� ������ ����������
		
		// Socket
		HANDLE m_hCompletionPort;
		SOCKET m_hServSock;	
		SOCKADDR_IN m_servAddr;
		MY_CLIENT_DETAIL_DATA m_clntSock[LIMITSOCKET];	// Ŭ���̾�Ʈ����
		MY_IO_DATA *m_pClntIOData[LIMITSOCKET];
		INT m_nClntCount;	// Ŭ���̾�Ʈ ����
		
		CRITICAL_SECTION m_cs;	// ũ��Ƽ�� ����

		BOOL m_bIsClient;	// ó���� �����Ҷ� Ŭ���̾�Ʈ���� ��������, Ŭ���̾�Ʈ�� 0, ������ -1
		UINT m_nErrorCode;	// ���ӽ��н� �����߻�(����:0, �������ӿ���:1, Ŭ�󸮾�Ʈ����:2, Completion:3)

		// ���߿� �����带 �����ϱ�����(���� ���н�)
		CWinThread *m_pCompletionThread;	
		CWinThread *m_pAcceptThread;

		BOOL m_bInitSevrFlag;	// InitSock() �Լ� ȣ��� ������� �Ѿ�� ������ �ߴ��� ���и� �ߴ��� Ȯ���� �ȵǼ� ���
		BOOL m_bInitClntFlag;

		CString m_strUserID;	// ����� ID(�ڽ��� ID)
		BOOL m_bViewFolder;		// �ڽ��� bViewFolder

		// CMainFrame �� m_cPreferencesValue �� �������ϸ� ���� ������ ������ �����忡�� ������ �ϸ� ���������Ƿ� ���⼭���� ������θ� ����� ���ӽ� �����־���
		CString m_strFolderTemp;	// �ӽ�����
		CString m_strFolderInComing;	//�ٿ�����
		CString m_strFolderShare;	// ��������
		
		void InitSock();
		CMyIOCPSocket();
		virtual ~CMyIOCPSocket();
};




// Thread
UINT CompletionThread(LPVOID param);	// IOCompletionPort Thread
UINT AcceptThread(LPVOID param);	// AcceptThread Thread




#endif // !defined(AFX_MYIOCPSOCKET_H__18807975_3C06_4757_82F8_8A5E861D64B5__INCLUDED_)
