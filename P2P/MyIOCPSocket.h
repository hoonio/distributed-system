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


#define BUFSIZE 8192		// 파일을 받거나 보낼때 사용되는 버퍼 크기, 200보다는 커야함
#define MYPORT 2000
#define LIMITSOCKET 30	// 클라이언트 최대갯수는 30명
#define FILENAME_LENGTH 100	// 파일명

// 패킷헤더
#define P_TEMP					99	// test
#define P_FILE_HEADER			10	// 파일이름, 파일사이즈
#define P_FILE_MAIN				11	// 파일내용
#define P_FILE_TAIL				12	// 파일전송 완료
#define P_FIRST_CONNECT			13	// 자신이 처음 서버에 접속하는것이라고 알림 (서버로부터 다른컴들의 아이피를 받음)
#define P_NOFIRST_CONNECT		14	// 자신이 처음 아니라 다른서버에서 아이피를 받아서 서버에 접속하는것이라고 알림 (다른컴에 접속만 함)
#define P_SEND_ALL_IP			15	// 다른컴들의 아이피를 전송
#define P_SEARCH_FILE			16	// 파일검색
#define P_SEARCH_FILE_LIST		17	// 파일검색후 받을데이타
#define P_DOWNLOAD_FILE_NOTICE	18	// 다운로드받기위해 서버에 전송
#define P_DOWNLOAD_FILE_ERROR	19	// 다운로드파일을 찾을수 없음
#define P_SEND_FILE_READY		20	// 파일전송을하기위해준비(파일을요청했을때 파일이 없을경우에는 P_DOWNLOAD_FILE_ERROR 를 보내기위해)
#define P_SEND_FILE				21	// 파일전송

// 송신인지 수신인지 구분
#define SENDDATA	0	// MY_IO_DATA 에서 사용
#define RECVDATA	1


///////////////////////////////////////////
// file 검색시 사용
typedef struct
{
	CString strFileName;
	CString strFilePath;
	DWORD nByte;
}MY_SEARCH_FILE;











// 클라이언트 정보, CompletionPort 에서 파라미터
typedef struct 
{
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;	
	UINT index;	// 배열로 구조체를 관리할때 index 저장
}MY_CLIENT_DATA;


// 클라이언트 상세정보
typedef struct
{
	MY_CLIENT_DATA *pClntData;
	CHAR clntName[256];	// 서버이름
	BOOL bViewFolder;	// 서버 폴더볼수있는지 여부
}MY_CLIENT_DETAIL_DATA;



typedef struct 
{
	UINT nPacketHeader;
	DWORD nTotalByte;	// 총크기
	char buffer[BUFSIZE + 1];	// 실제 데이타가 들어있는공간, NULL 까지  + 1	
}MY_PACKET;



// 패킷전송시사용(전체데이타)
typedef struct
{
	char *pszFileSearchData;	// 파일검색데이타전송시 사용
	char *pszTotalIPData;	// 처음접속시 모든컴의 아이피에 사용
}MY_SEND_DATA;


// 패킷받을시 사용(전체데이타)
typedef struct
{
	char *pszFileSearchData;	// 파일검색데이타전송시 사용
	char *pszTotalIPData;	// 처음접속시 모든컴의 아이피에 사용
}MY_RECV_DATA;


// 소켓의 버퍼 정보를 구조체화.
typedef struct 
{
	OVERLAPPED overlapped;
	MY_PACKET sendData;	// 한번에 보내지는 패킷
	MY_PACKET recvData;
	WSABUF wsaBuf;			// wsaBuf.buf 에 실제 데이타 주소가 들어가있다( &data)	
	BOOL flag;	// 수신인지 송신인지 구분, GetQueuedCompletionStatus() 에서 수신 송신 모두 반응하므로 	
	DWORD nSendByte;	// 자신이 보낸 데이타양	
	MY_SEND_DATA mySendData;	// 실제 총데이타
	MY_RECV_DATA myRecvData;
	UINT nDownloadFileIndex;	// 파일다운시에만 사용
	UINT nUploadFileIndex;	// 파일업로드시에만 사용
	//DWORD nRecvByte;	// 자신이 받은 데이타양
}MY_IO_DATA;



// 업 다운로드 파일의 정보
typedef struct
{
	UINT nIndex;	// 인덱스번호(다운로드뷰에서와의 연동을위ㅐ)
	CString strFileFullName;	// 파일의경로및 이름
	DWORD nFileSize;	// 파일사이즈
	DWORD nReceiveSize;	// 다운받은 사이즈, 업로드한사이즈
	CFile *pFile;	// 파일포인터
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
		CList<MY_FILE_DATA, MY_FILE_DATA&> m_uploadFileDataList;	// 업로드하는 파일의 정보
		CList<MY_FILE_DATA, MY_FILE_DATA&> m_downloadFileDataList;	// 다운받는 파일의 정보
		UINT m_nDownloadMaxIndex;	// 다음인덱스번호(다운로드목록과 실제다운중인파일ㅇ과의 연동) 이번호는 고유한번호로 가장큰번호
		UINT m_nUploadMaxIndex;	// 다음인덱스번호(업로드목록과 실제업로드중인파일ㅇ과의 연동) 이번호는 고유한번호로 가장큰번호
		
		//CMyQueue myQueue;	// 데이타 받아서 큐에서 관리, 현재는 필요없음
		BOOL FindAlltFile(char* pszPath, char *pszName, CList<MY_SEARCH_FILE , MY_SEARCH_FILE& >*pFileList);
		void ErrorHandling(UINT errorCode, CString strPrn);
		CString m_strSevrIP;
		BOOL SendData(MY_CLIENT_DATA *, MY_IO_DATA *, UINT, char *, DWORD nTotalByte, BOOL bFirst);
		BOOL RecvData(MY_CLIENT_DATA *, MY_IO_DATA *);
		void CloseClntSock(MY_CLIENT_DATA *pClntData, MY_IO_DATA *pIOData);	// 클라이언트가 접속해제시
		void CloseSevrSock();	// 자신이 접속해제시
		BOOL ClientConnect(CString strIp);	// 자신이 클라이언트로 접속했을시
		BOOL ServerConnect();	// 자신이 서버로 접속했을시
		
		// Socket
		HANDLE m_hCompletionPort;
		SOCKET m_hServSock;	
		SOCKADDR_IN m_servAddr;
		MY_CLIENT_DETAIL_DATA m_clntSock[LIMITSOCKET];	// 클라이언트정보
		MY_IO_DATA *m_pClntIOData[LIMITSOCKET];
		INT m_nClntCount;	// 클라이언트 갯수
		
		CRITICAL_SECTION m_cs;	// 크리티컬 섹션

		BOOL m_bIsClient;	// 처음에 접속할때 클라이언트인지 서버인지, 클라이언트면 0, 서버면 -1
		UINT m_nErrorCode;	// 접속실패시 에러발생(정상:0, 서버접속에러:1, 클라리언트에러:2, Completion:3)

		// 나중에 쓰레드를 종료하기위해(접속 실패시)
		CWinThread *m_pCompletionThread;	
		CWinThread *m_pAcceptThread;

		BOOL m_bInitSevrFlag;	// InitSock() 함수 호출시 쓰레드로 넘어가서 성공을 했는지 실패를 했는지 확인이 안되서 사용
		BOOL m_bInitClntFlag;

		CString m_strUserID;	// 사용자 ID(자신의 ID)
		BOOL m_bViewFolder;		// 자신의 bViewFolder

		// CMainFrame 의 m_cPreferencesValue 에 접근을하면 값을 얻을수 있지만 쓰레드에서 접근을 하면 에러가나므로 여기서따로 폴더경로를 만들고 접속시 값을넣어줌
		CString m_strFolderTemp;	// 임시폴더
		CString m_strFolderInComing;	//다운폴더
		CString m_strFolderShare;	// 공유폴더
		
		void InitSock();
		CMyIOCPSocket();
		virtual ~CMyIOCPSocket();
};




// Thread
UINT CompletionThread(LPVOID param);	// IOCompletionPort Thread
UINT AcceptThread(LPVOID param);	// AcceptThread Thread




#endif // !defined(AFX_MYIOCPSOCKET_H__18807975_3C06_4757_82F8_8A5E861D64B5__INCLUDED_)
