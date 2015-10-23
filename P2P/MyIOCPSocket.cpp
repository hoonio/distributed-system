// MyIOCPSocket.cpp: implementation of the CMyIOCPSocket class.
//
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
////
////					[ErrorCode]
////	10061 => 연결시도가 강제적으로 거부됩니다
////	10004 => WSACancelBlockingCall() 함수를 호출해서 블로킹소켓동작을 중지
////
///////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************************
데이타를 보내면 계속보내므로 받는쪽에서 처리도 하기전에 또 받아지기 떄문에 데이타가 엉켜버린다(그냥 내추측)
그래서 Sleep 를주었지만 10번을 보낸다면 시간을 너무 잡아 먹는다(2대의 컴만됨)
버퍼를 크게 잡으면 컴이 많아도 상관업음
********************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////







#include "stdafx.h"
#include "P2p.h"
#include "MyIOCPSocket.h"

#include "MainFrm.h"
#include "SearchView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif




/************** Queue ****************************/
CMyQueue::CMyQueue()
{
	rear=0;
	front=0;
	for(int i=0; i<MAXQUEUE; i++){
		memset(&(queue[i]), 0, sizeof(MY_PACKET));
	}
}

CMyQueue::~CMyQueue()
{

}


int CMyQueue::Push(MY_PACKET data){
	if((rear + 1) % MAXQUEUE == front){  //queue가 꽉찼냐??
		AfxMessageBox("Queue Overflow");
		return 0;
	}	
	rear = (rear + 1) % MAXQUEUE;
	queue[rear] = data;
	return 1;
}

int CMyQueue::Pop(MY_PACKET *data){
	if(front == rear){
		AfxMessageBox("Queue Underflow");
		return 0;
	}
	*data = queue[front+1];
	front = (front + 1) % MAXQUEUE;
	return 1;
}

/******************************************/






//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyIOCPSocket::CMyIOCPSocket()
{
	m_nClntCount = 0;
	m_hServSock = INVALID_SOCKET;
	for(int i=0 ;i<LIMITSOCKET; i++){
		memset(&m_clntSock[i] , 0, sizeof(MY_CLIENT_DETAIL_DATA));
	}
	
	m_pCompletionThread = NULL;	
	m_pAcceptThread = NULL;

	m_bInitSevrFlag = TRUE;
	m_bInitClntFlag = TRUE;
	
	m_nDownloadMaxIndex = 0;
	m_nUploadMaxIndex = 0;
	
	
	
	InitializeCriticalSection(&m_cs);	// 크리티컬섹션 START
}

CMyIOCPSocket::~CMyIOCPSocket()
{
	DeleteCriticalSection(&m_cs);	// 크리티컬섹션 END
}

void CMyIOCPSocket::InitSock()
{
	m_nErrorCode = 0;	//정상코드
	m_nClntCount = 0;
	WSADATA	wsaData;
	SYSTEM_INFO SystemInfo;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){	// Load Winsock 2.2 DLL 
		ErrorHandling(1, "WSAStartup()");
	}

	//1. Completion Port 생성.
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(m_hCompletionPort == NULL){
		ErrorHandling(3, "CreateIoCompletionPort()");
	}
	GetSystemInfo(&SystemInfo);

	//2. Completion Port 에서 입출력 완료를 대기 하는 쓰레드를 CPU 개수 만큼 생성.
	for(int i=0; i<(int)SystemInfo.dwNumberOfProcessors; i++){
		//_beginthreadex(NULL, 0, CompletionThread, (LPVOID)m_hCompletionPort, 0, NULL);
		m_pCompletionThread = AfxBeginThread(CompletionThread, (LPVOID)this);	
	}

	m_pAcceptThread = AfxBeginThread(AcceptThread, (LPVOID)this);	// 서버소켓생성, 클라이언트 접속 대기
	if(m_bIsClient == 0){	// 클라이언트라면
		while(m_bInitClntFlag == TRUE){
		//	Sleep(100);
		}
	}
}




// 서버에서 접속 대기 쓰레드 (클리이언트가 접속시마다 실행 (while 문))
UINT AcceptThread(LPVOID param)
{	
	CMyIOCPSocket *threadObject = (CMyIOCPSocket *)param;	// param 에서 this 를 받아서 클래스 제어

	MY_IO_DATA *pIOData;
	MY_CLIENT_DATA *pClntData;
	DWORD flags = 0;
	DWORD nRecvBytes = 0;
	HANDLE hResult;


	if(threadObject->m_bIsClient == 0){	// 클라이언트라면
		threadObject->ClientConnect(threadObject->m_strSevrIP);	// 자신이 클라이언트로 접속시
	}
	threadObject->ServerConnect();	// 서버소켓 START (서버든 클라이언트든 서버소켓은 가지고 있어야함)	

	SOCKET *pHServSock = &(threadObject->m_hServSock);
	HANDLE *pHCompletionPort = &(threadObject->m_hCompletionPort);	
	
	while(TRUE)
	{			
		SOCKET hClntSock;
		SOCKADDR_IN clntAddr;				
		INT addrLen = sizeof(clntAddr);		
		
		// accept()		
		hClntSock = accept(*pHServSock, (SOCKADDR*)&clntAddr, &addrLen);		
		if(hClntSock == INVALID_SOCKET)
		{
			//int error = WSAGetLastError();
			//char out[256];
			//sprintf(out, "accept() error! [ErrorCode %d]", error);
			//AfxMessageBox(out); 			
			threadObject->ErrorHandling(1, "accept()");
			threadObject->m_pAcceptThread = NULL;
			return FALSE;
			//continue;
		}
		
		// 연결 된 클라이언트의 소켓 핸들 정보와 주소 정보를 설정.
		pClntData = (MY_CLIENT_DATA *)malloc(sizeof(MY_CLIENT_DATA));		
		pClntData->hClntSock = hClntSock;
		memcpy(&(pClntData->clntAddr), &clntAddr, addrLen);
		
		EnterCriticalSection(&(threadObject->m_cs));	// 크리티컬섹션 enter

		threadObject->m_clntSock[threadObject->m_nClntCount].pClntData = pClntData;
		threadObject->m_clntSock[threadObject->m_nClntCount].pClntData->index = threadObject->m_nClntCount;			

		LeaveCriticalSection(&(threadObject->m_cs));	// 크리티컬섹션 leave

		//3. Overlapped 소켓과 CompletionPort의 연결.
		hResult = CreateIoCompletionPort((HANDLE)hClntSock, *pHCompletionPort, (DWORD)pClntData,	// 쓰레드에전달
			0);
		if(hResult == NULL){
			threadObject->ErrorHandling(3, "CreateIoCompletionPort()");
		}
		
		// 연결 된 클라이언트를 위한 버퍼를 설정 하고 OVERLAPPED 구조체 변수 초기화.
		pIOData=(MY_IO_DATA *)malloc(sizeof(MY_IO_DATA));
		memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));	
		memset(&(pIOData->mySendData), 0, sizeof(MY_SEND_DATA));
		memset(&(pIOData->myRecvData), 0, sizeof(MY_RECV_DATA));

		threadObject->m_pClntIOData[threadObject->m_nClntCount] = pIOData;
				
		EnterCriticalSection(&(threadObject->m_cs));	// 크리티컬섹션 enter

		threadObject->m_nClntCount ++;	// 클라이언트수 하나증가
	
		LeaveCriticalSection(&(threadObject->m_cs));	// 크리티컬섹션 leave
		
		// 서버는 시작하자마자 수신대기를 한다(처음 클라이언트가 붙으면 클라이언트는 다른컴의 아이피를 요청할수 있으므로)
		if(threadObject->RecvData(pClntData, pIOData) == FALSE){	// 데이타 WSARecv()
			continue;
		}

		
		// 메인프레임의 statusBar
		char pzClntAddr[20];
		char *strOut;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
		char *strOut2;
		strOut = new char[256];
		strOut2 = new char[256];
		sprintf(strOut, "연결됨 (서버:%d)", threadObject->m_nClntCount);	
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
		strcpy(pzClntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
		sprintf(strOut2, "%s 서버가\n접속되었습니다!", pzClntAddr);
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_SERVER_CONNECT);

#ifdef MYSOCKETDEBUG
	char temp2[20];
	strcpy(temp2, inet_ntoa(clntAddr.sin_addr));
	char out[256];
	sprintf(out, "%s 클라이언트 접속됨! \n클라이언트 수(자신은제외) : %d\n", temp2, threadObject->m_nClntCount);
	AfxMessageBox(out);
#endif
	}
}



//입 출력 완료에 따른 쓰레드의 행동 정의 
UINT CompletionThread(LPVOID param)
{	
	CMyIOCPSocket *threadObject = (CMyIOCPSocket *)param;	// param 에서 this 를 받아서 클래스 제어
	HANDLE *pHCompletionPort = &(threadObject->m_hCompletionPort);
	DWORD BytesTransferred;
	MY_CLIENT_DATA *pClntData;
	MY_IO_DATA *pIOData;
	DWORD nRecvBytes = 0;	// 받은 데이타양
	DWORD nSendBytes = 0;
	DWORD flags = 0;
	BOOL bIsRecvData = TRUE;	// recv 를 할것인지?
	//CFile *pFile;	
	

	while(1)
	{
		bIsRecvData = TRUE;
		// 5. 입출력이 완료 된 소켓의 정보 얻음. 
		GetQueuedCompletionStatus(*pHCompletionPort,			 // Completion Port
								  &BytesTransferred,		 // 입출력 과정에서 전송 된 바이트 수
								  (LPDWORD)&pClntData,		// CreateIoCompletionPort() 에서 보내온 인자
								  (LPOVERLAPPED*)&pIOData, // OVERLAPPED 구조체 포인터.
								  INFINITE);
		
		if(pIOData->flag != SENDDATA  &&  pIOData->flag != RECVDATA){	// 남이접속해제시 만약 recv가 2번이라면 에러... 그래서 이미 free 한 pIOData는 접근 못하게
			continue;
		}
		if(BytesTransferred == 0){
			threadObject->CloseClntSock(pClntData, pIOData);	// 클라이언트 접속이 끊길때 (남이 접속을 해제했을시)
			continue;
		}
	



		//UINT aa = pIOData->wsaBuf.buf[0] + sizeof(BUFSIZE + 1);
		//DWORD bb = pIOData->wsaBuf.buf[0] + sizeof(BUFSIZE + 1) + sizeof(UINT);
		

		//////////////////////////////////////////////// 송신이 완료되었을때///////////////////////////////////////////////
		if(pIOData->flag == SENDDATA){	
			char szMessage[BUFSIZE];	
			DWORD nTotalByte = pIOData->sendData.nTotalByte;	// 보낸 총 바이트
			UINT nPacketHeader = pIOData->sendData.nPacketHeader;	// 다른컴에서 받은데이타확인, (수신확인시 사용)
			DWORD nSendByte = pIOData->nSendByte;	// 보낸바이트
			strcpy(szMessage, pIOData->sendData.buffer);	// szMessage 실질적인 메시

			//CString strTrace;
			//strTrace.Format("========보낸데이타 START========\n %s \n========보낸데이타 END========\n", szMessage);
			//TRACE(strTrace);	// trace 는 많은문자는 출력못함


			//	자기전송이 완료되었다고 알렸주면서 못보낸데이타가있는지 확인
			switch(nPacketHeader){
				case P_FIRST_CONNECT:	// 처음접속시 보냄
					pIOData->flag = RECVDATA;	// 곧바로 데이타를 받아야하므로 flag 변경
					break;
				case P_NOFIRST_CONNECT:	// 다른 서버에 접속시 보냄
					pIOData->flag = RECVDATA;	// 곧바로 데이타를 받아야하므로 flag 변경
					break;	
				case P_SEND_ALL_IP:	// 컴처음 접속시 다른모든 컴들의 아이피를 전송
					{
						if(nTotalByte > nSendByte){	// 더보낼 데이타가 있다면							
							char szSend[BUFSIZE + 1];	// 실제적으로 보내지는 데이타
							CString strData;
							strData.Format("%s", pIOData->mySendData.pszTotalIPData);
							strncpy(szSend, strData.Mid(BUFSIZE * (nSendByte / BUFSIZE), BUFSIZE), BUFSIZE);							
							szSend[BUFSIZE] = '\0';
							//Sleep(200);	// 곧바로 send 를 하면 받는쪽에서 데이타가 바뀌므로 약간의시간을줌
							if(threadObject->SendData(pClntData, pIOData, P_SEND_ALL_IP, szSend, strData.GetLength(), FALSE) == FALSE){	// 모든 컴의 아이피를 지금접속한 클라이언트에세 전송(서버와 클라이언트만 있으면 빈내용을 전송)
								continue;
							}							
							bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
						}else{
							delete []pIOData->mySendData.pszTotalIPData;
							pIOData->mySendData.pszTotalIPData = NULL;
						}						
					}
					//bIsRecvData = FALSE;	// TEST
					break;
				case P_SEARCH_FILE:	// 자신이 검색요청시
					bIsRecvData = FALSE;	// 이미 recv 한번 된상태
					pIOData->flag = RECVDATA;	// 곧바로 데이타를 받아야하므로 flag 변경
					break;
				case P_SEARCH_FILE_LIST:	// 검색한파일목록 
					{											
						if(nTotalByte > nSendByte){	// 더보낼 데이타가 있다면							
							char szSend[BUFSIZE + 1];	// 실제적으로 보내지는 데이타
							CString strFileSearchData;
							strFileSearchData.Format("%s", pIOData->mySendData.pszFileSearchData);
							strncpy(szSend, strFileSearchData.Mid(BUFSIZE * (nSendByte / BUFSIZE), BUFSIZE), BUFSIZE);							
							szSend[BUFSIZE] = '\0';							
							if(threadObject->SendData(pClntData, pIOData, P_SEARCH_FILE_LIST, szSend, strFileSearchData.GetLength(), FALSE) == FALSE){	// 모든 컴의 아이피를 지금접속한 클라이언트에세 전송(서버와 클라이언트만 있으면 빈내용을 전송)
								continue;
							}							
							bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
						}else{
							delete []pIOData->mySendData.pszFileSearchData;
							pIOData->mySendData.pszFileSearchData = NULL;
						}						
					}
					//bIsRecvData = FALSE;
					break;
				case P_DOWNLOAD_FILE_NOTICE:	// 파일을 다운로드받기위해 서버로 파일명, 경로 전송
					bIsRecvData = FALSE;	// 이미 recv 한번 된상태
					pIOData->flag = RECVDATA;	// 곧바로 데이타를 받아야하므로 flag 변경
					break;		
				case P_SEND_FILE_READY:	// 파일이 있다는 메세지가 간것을 확인하면서 파일전송 시작
					{
						// 파일데이타 전송
						POSITION pos;
						MY_FILE_DATA *tempData;
						pos = threadObject->m_uploadFileDataList.GetHeadPosition();
						while(pos){	// 작업하고 있는파일찾기
							tempData = &(threadObject->m_uploadFileDataList.GetAt(pos));
							if(tempData->nIndex == pIOData->nUploadFileIndex){
								break;
							}
							threadObject->m_uploadFileDataList.GetNext(pos);
						}


						char szSendFileData[BUFSIZE + 1];
						tempData->pFile->Read(szSendFileData, BUFSIZE);

						if(nTotalByte <= BUFSIZE){	// 파일을 열면 NULL 처리를 못하므로 여기서 계산을 해서 마지막열때 NULL 계산
							szSendFileData[nTotalByte % BUFSIZE] = '\0';															
							tempData->nReceiveSize += nTotalByte % BUFSIZE;	// 보낸데이타
						}else{								
							szSendFileData[BUFSIZE] = '\0';												
							tempData->nReceiveSize += BUFSIZE;	// 보낸데이타
						}
						


						/* 파일을 전송할때는 바이너리 값을 전송할떄 sendData() 함수로 호출하면 바이너리에는 NULL 을포함하므로 문제가 생기므로 여기서 함수부분을 복사해서 처리 */
						BOOL bFirst = TRUE;
						int result;
						DWORD nSendBytes  = 0;				
						memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
						memset(&(pIOData->sendData), 0, sizeof(MY_PACKET));
						pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
						pIOData->wsaBuf.buf = (char *)&(pIOData->sendData);			// buf 에 pIOData->data.buffer 를 넣게 되면 buffer은 걊을 받을수 있지만 nPacketHeader은 값을 받지 못한다 그래서 data 구조체의 주소를 대입해서 포인터 자체를 받아서 계산해서 데이타와 헤더까지 얻는다
						pIOData->flag = SENDDATA;	// 송신
						pIOData->sendData.nPacketHeader = P_SEND_FILE;	// 패킷헤더
						pIOData->sendData.nTotalByte = tempData->pFile->GetLength();	// 패킷 총길이
						//strncpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
						memcpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
						pIOData->sendData.buffer[BUFSIZE] = '\0';
						if(bFirst == TRUE){	// 처음 보낸데이타라면
							pIOData->nSendByte = strlen(szSendFileData);
						}else{
							pIOData->nSendByte += strlen(szSendFileData);	//처음보내는 데이타가 아니면 얼마나 보내고 있는지 데이타를 더한다
						}
						result = WSASend(pClntData->hClntSock, &pIOData->wsaBuf, 1, &nSendBytes, 0, &pIOData->overlapped, NULL);
						pIOData->sendData.nTotalByte = nTotalByte;	// 패킷 총길이	
						if(result == SOCKET_ERROR){		
							if(WSAGetLastError() != WSA_IO_PENDING){
								int error = WSAGetLastError();
								char out[256];
								sprintf(out, "WSASend() error! [ErrorCode %d]", error);
								AfxMessageBox(out); 
								//return FALSE;
								break;
							}
						}
						/* END */


						//tempData->nReceiveSize += strlen(szSendFileData);	// 보낸데이타
						/**
						if(threadObject->SendData(pClntData, pIOData, P_SEND_FILE, szSendFileData, tempData->pFile->GetLength(), TRUE) == FALSE){	// 파일전송
							continue;
						}					
						**/
						bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
						
					}
					break;
				case P_DOWNLOAD_FILE_ERROR:	// 다운로드 파일을 찾을수 없다고 메시지를 보냄
					pIOData->flag = RECVDATA;	// 곧바로 데이타를 받아야하므로 flag 변경
					break;
				case P_SEND_FILE:	// 파일전송
					{
						POSITION pos;
						MY_FILE_DATA *tempData;
						pos = threadObject->m_uploadFileDataList.GetHeadPosition();
						while(pos){	// 작업하고 있는파일찾기
							tempData = &(threadObject->m_uploadFileDataList.GetAt(pos));
							if(tempData->nIndex == pIOData->nUploadFileIndex){
								break;
							}
							threadObject->m_uploadFileDataList.GetNext(pos);
						}
						
						if(nTotalByte > tempData->nReceiveSize){	// 더보낼 데이타가 있다면
							// 파일데이타 전송
							char szSendFileData[BUFSIZE + 1];	// 실제적으로 보내지는 데이타								
							szSendFileData[0] = '\0';
							tempData->pFile->Read(szSendFileData, BUFSIZE);
							
							if(nTotalByte <= nSendByte + BUFSIZE){	// 파일을 열면 NULL 처리를 못하므로 여기서 계산을 해서 마지막열때 NULL 계산
								szSendFileData[nTotalByte % BUFSIZE] = '\0';
								tempData->nReceiveSize += nTotalByte % BUFSIZE;	// 보낸데이타
							}else{								
								szSendFileData[BUFSIZE] = '\0';								
								tempData->nReceiveSize += BUFSIZE;	// 보낸데이타
							}
							Sleep(30);	// 여기서 sleep를 안주면 데이타가 큰것을 보낼때 받는쪽 큐 버퍼가 모자라서 그런지 제대로 받지를 못함
							
							//tempData->nReceiveSize += strlen(szSendFileData);	// 보낸데이타

							/* 파일을 전송할때는 바이너리 값을 전송할떄 sendData() 함수로 호출하면 바이너리에는 NULL 을포함하므로 문제가 생기므로 여기서 함수부분을 복사해서 처리 */
							BOOL bFirst = FALSE;
							int result;
							DWORD nSendBytes  = 0;				
							memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
							memset(&(pIOData->sendData), 0, sizeof(MY_PACKET));
							pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
							pIOData->wsaBuf.buf = (char *)&(pIOData->sendData);			// buf 에 pIOData->data.buffer 를 넣게 되면 buffer은 걊을 받을수 있지만 nPacketHeader은 값을 받지 못한다 그래서 data 구조체의 주소를 대입해서 포인터 자체를 받아서 계산해서 데이타와 헤더까지 얻는다
							pIOData->flag = SENDDATA;	// 송신
							pIOData->sendData.nPacketHeader = P_SEND_FILE;	// 패킷헤더
							pIOData->sendData.nTotalByte = tempData->pFile->GetLength();	// 패킷 총길이
							//strncpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
							memcpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
							pIOData->sendData.buffer[BUFSIZE] = '\0';
							if(bFirst == TRUE){	// 처음 보낸데이타라면
								pIOData->nSendByte = strlen(szSendFileData);
							}else{
								pIOData->nSendByte += strlen(szSendFileData);	//처음보내는 데이타가 아니면 얼마나 보내고 있는지 데이타를 더한다
							}
							result = WSASend(pClntData->hClntSock, &pIOData->wsaBuf, 1, &nSendBytes, 0, &pIOData->overlapped, NULL);
							pIOData->sendData.nTotalByte = nTotalByte;	// 패킷 총길이	
							if(result == SOCKET_ERROR){		
								if(WSAGetLastError() != WSA_IO_PENDING){
									int error = WSAGetLastError();
									char out[256];
									sprintf(out, "WSASend() error! [ErrorCode %d]", error);
									AfxMessageBox(out); 
									//return FALSE;
									break;
								}
							}
							/* END */

							/**
							if(threadObject->SendData(pClntData, pIOData, P_SEND_FILE, szSendFileData, tempData->pFile->GetLength(), FALSE) == FALSE){	// 파일전송
								continue;
							}
							**/
							bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
							//AfxMessageBox(szSendFileData);
						}else{
							// 업뷰에나오는 리스트에 업로드됐다는것을알림
							((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_UPLOAD_COMPLETE, (WPARAM)tempData->nIndex, (LPARAM)0);
						
							tempData->nReceiveSize = tempData->pFile->GetLength();

							tempData->pFile->Close();	// 파일을 다 전송후에 파일을 닫느다	
							delete tempData->pFile;
							threadObject->m_uploadFileDataList.RemoveAt(pos);	// 리스트에서 삭제	
							//AfxMessageBox("파일전송완료");							
							bIsRecvData = TRUE;
						}
					}
					break;
			}	// if(pIOData->flag == SENDDATA)	
		///////////////////////////////////// 자신에게 메시지가 왔을때 처리(메시지 수신됨) ///////////////////////////////////////////
		}else if(pIOData->flag == RECVDATA){	
			char szMessage[BUFSIZE];	
			DWORD nTotalByte = pIOData->recvData.nTotalByte;	// 보낸 총 바이트
			UINT nPacketHeader = pIOData->recvData.nPacketHeader;	// 다른컴에서 받은데이타확인, (수신확인시 사용)
			DWORD nSendByte = pIOData->nSendByte;	// 보낸바이트
			strcpy(szMessage, pIOData->recvData.buffer);	// szMessage 실질적인 메시

			//CString strTrace;
			//strTrace.Format("========받은데이타 START========\n %s \n========받은데이타 END========\n", szMessage);
			//TRACE(strTrace);	// trace 는 많은문자는 출력못함


			switch(nPacketHeader){	// 헤더로 체크				
				case P_FIRST_CONNECT:	// 클라이언트가 처음 접속시 다른컴들의 아이피를 요청, // 다른컴들의 아이피를 전송
					{						
						TRACE("P_FIRST_CONNECT 메지시를 받아서 다시 ip 를 전송");

						EnterCriticalSection(&(threadObject->m_cs));	// 크리티컬섹션 enter

						// 받은 아이디와 nViewFolder 값정리,
						CString strMessage;
						strMessage.Format("%s", szMessage);
						int cnt = strMessage.Find('?', 0);	
						int oldCnt = 0;
						strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(0, cnt));	// 클라이언트쪽에서보낸 ID 를저장
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						threadObject->m_clntSock[threadObject->m_nClntCount - 1].bViewFolder = (strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1) == "1" ? 1 : 0);
	
						//////////////////////////////// 보낼메시지 정리 // message => ex) 2?122.233.43.212?123.21.223.221?
						CString strTotalData;	// 총데이타를 여기다가 저장한후 버퍼의 길이를 넘치면 쪼개서 보낸다
						int len = 0;
						CString strNum;						
						
						// 클라이언트의 갯수
						strNum.Format("%d", threadObject->m_nClntCount - 1);						
						strTotalData += strNum;
						strTotalData += "?";						

						// 자신의 이름과 viewFolder, 자신의 아이피는 이미알고 있으므로 ip 는 안보냄						
						strTotalData += threadObject->m_strUserID;
						strTotalData += "?";
						strTotalData += (threadObject->m_bViewFolder == 1 ? "1" : "0");
						strTotalData += "?";						

						// 다른컴의 ip 와 이름, viewFolder 을 보냄
						for(int i=0; i<threadObject->m_nClntCount-1; i++){	// 마지막 클라이언트는 메시지를 받는 클라이언트이므로 -1
							// ip
							strTotalData += inet_ntoa(threadObject->m_clntSock[i].pClntData->clntAddr.sin_addr);
							strTotalData += "?";

							// 이름
							strTotalData += threadObject->m_clntSock[i].clntName;							
							strTotalData += "?";

							// nViewFolder
							strTotalData += (threadObject->m_clntSock[i].bViewFolder == 1 ? "1" : "0");		
							strTotalData += "?";
						}
					
						pIOData->mySendData.pszTotalIPData = new char[strTotalData.GetLength() + 1];	// 데이타를 다보내는 것이 아니므로 동적생성
						strcpy(pIOData->mySendData.pszTotalIPData, strTotalData);

						char szSendip[BUFSIZE + 1];	// 실제적으로 보내지는 데이타

						LeaveCriticalSection(&(threadObject->m_cs));	// 크리티컬섹션 leave

						strncpy(szSendip, strTotalData.Mid(0, BUFSIZE), BUFSIZE);							
						szSendip[BUFSIZE] = '\0';
						//Sleep(100);
						if(threadObject->SendData(pClntData, pIOData, P_SEND_ALL_IP, szSendip, strTotalData.GetLength(), TRUE) == FALSE){	// 모든 컴의 아이피를 지금접속한 클라이언트에세 전송(서버와 클라이언트만 있으면 빈내용을 전송)
							continue;
						}
						//AfxMessageBox(strTotalData + "  \n보냄");
						bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
					}		
					break;
				case P_NOFIRST_CONNECT:	// 클라이언트가 이미 서버에서 아이피를 받아서 접속요청이므로 그냥 접속요청만 받음
					{
						// 클라이언트쪽에서보낸 ID와 bViewFolder 를저장						
						int cnt = 0;
						int oldCnt = 0;
						CString strMessage;
						strMessage.Format("%s", szMessage);
						cnt = strMessage.Find('?', 0);						
						strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(0, cnt));

						// 클라이언트에서보낸 viewFolder 정리
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						threadObject->m_clntSock[threadObject->m_nClntCount - 1].bViewFolder = (strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1) == "1" ? 1 : 0);	
						break;
					}
				case P_SEND_ALL_IP:
					{
						if(pIOData->myRecvData.pszTotalIPData == NULL){
							pIOData->myRecvData.pszTotalIPData = new char[nTotalByte + 1];
							memset(pIOData->myRecvData.pszTotalIPData, 0, nTotalByte + 1);	
						}

						strncat(pIOData->myRecvData.pszTotalIPData, szMessage, BUFSIZE);
						if(nTotalByte > strlen(pIOData->myRecvData.pszTotalIPData)){	// BytesTransferred 는 BUFSIZE 의 크기임
							break;	// 데이타는 더받기위해 break를 한다
						}
						
						INT cnt;
						CString strMessage;
						strMessage.Format("%s", pIOData->myRecvData.pszTotalIPData);
						cnt = strMessage.Find('?', 0);		

						//AfxMessageBox(strMessage + "  \n받음");
						
						// 여러개의 정보를 쪼갠다
						char comCnt[3];
						if(cnt == 1){	// 컴수가 한자리일때
							comCnt[0] = pIOData->myRecvData.pszTotalIPData[0];
							comCnt[1] = '\0';
						}else if(cnt == 2){	// 컴수가 두자리일때(2자리 이상은 없음)
							comCnt[0] = pIOData->myRecvData.pszTotalIPData[0];
							comCnt[1] = pIOData->myRecvData.pszTotalIPData[1];		
							comCnt[2] = '\0';
						}

						// 클라이언트쪽에서보낸 ID 를저장
						int oldCnt = 0;
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1));

						// 클라이언트에서보낸 viewFolder 정리
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						threadObject->m_clntSock[threadObject->m_nClntCount - 1].bViewFolder = (strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1) == "1" ? 1 : 0);						
											
						CString clntSock[LIMITSOCKET];
						for(int i=0;i<atoi(comCnt); i++){	// 서버와 클라이언트 밖에 없으면 comCnt 는 0 , 그럴땐 처리않함
							// ip
							oldCnt = cnt;
							cnt = strMessage.Find('?', oldCnt + 1);							
							clntSock[i] = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);	
							threadObject->ClientConnect(clntSock[i]);	// 받은 서버만큼 접속요청							

							// 이름
							oldCnt = cnt;
							cnt = strMessage.Find('?', oldCnt + 1);
							strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1));

							// nViewFolder
							oldCnt = cnt;
							cnt = strMessage.Find('?', oldCnt + 1);
							threadObject->m_clntSock[threadObject->m_nClntCount - 1].bViewFolder = (strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1) == "1" ? 1 : 0);							
						}			
						
						delete [] pIOData->myRecvData.pszTotalIPData;
						pIOData->myRecvData.pszTotalIPData = NULL;
						
						//CString temp;
						//temp.Format("%s\nP_SEND_ALL_IP 메지시를 받음(받은 아이피 처리)", szMessage);
						//TRACE(temp);

						break;		
					}			
				case P_SEARCH_FILE:	// 파일검색시
					{	
						char szShareFolder[256];
						sprintf(szShareFolder, "%s", threadObject->m_strFolderShare);
						CList<MY_SEARCH_FILE , MY_SEARCH_FILE&>listSearchFile;	// 파일을 검색해서 저장	
						threadObject->FindAlltFile(szShareFolder, szMessage, &listSearchFile);	// 공유된폴더에서 검색
						int cnt = listSearchFile.GetCount();
						CString strFileSearchData;
						CString strFileName;
						CString strFilePath;
						MY_SEARCH_FILE searchFile;
						POSITION pos;
						CString temp;
						temp.Format("%d", cnt);
						strFileSearchData = temp;
						strFileSearchData += "?";
						pos = listSearchFile.GetHeadPosition();
						while(pos){
							searchFile = listSearchFile.GetAt(pos);
							listSearchFile.GetNext(pos);
							strFileSearchData += searchFile.strFileName;
							strFileSearchData += "?";
							strFileSearchData += searchFile.strFilePath;
							strFileSearchData += "?";
							temp.Format("%d", searchFile.nByte);
							strFileSearchData += temp;
							strFileSearchData += "?";							
						}
		
						pIOData->mySendData.pszFileSearchData = new char[strFileSearchData.GetLength() + 1];	// 데이타를 다보내는 것이 아니므로 동적생성
						strcpy(pIOData->mySendData.pszFileSearchData, strFileSearchData);

						char szSend[BUFSIZE + 1];	// 실제적으로 보내지는 데이타
						strncpy(szSend, strFileSearchData.Mid(0, BUFSIZE), BUFSIZE);
						szSend[BUFSIZE] = '\0';						
						if(threadObject->SendData(pClntData, pIOData, P_SEARCH_FILE_LIST, szSend, strFileSearchData.GetLength(), TRUE) == FALSE){	// 모든 컴의 아이피를 지금접속한 클라이언트에세 전송(서버와 클라이언트만 있으면 빈내용을 전송)
							continue;
						}

						bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
					}
					break;
				case P_SEARCH_FILE_LIST:	// 파일검색후 목록을 받음
					{
						if(pIOData->myRecvData.pszFileSearchData == NULL){
							pIOData->myRecvData.pszFileSearchData = new char[nTotalByte + 1];
							memset(pIOData->myRecvData.pszFileSearchData, 0, nTotalByte + 1);							
						}
						strncat(pIOData->myRecvData.pszFileSearchData, szMessage, BUFSIZE);
						//static DWORD nRecvByte = 0;	// 받은데이타양을 쌓는다
						//nRecvByte += BytesTransferred - sizeof(UINT) - sizeof(DWORD);	// 실제데이타 구조체에서 nPacketHeader 과 nTotalByte를 뺴야함
						if(nTotalByte > strlen(pIOData->myRecvData.pszFileSearchData)){	// BytesTransferred 는 BUFSIZE 의 크기임
							break;	// 데이타는 더받기위해 break를 한다
						}
						//nRecvByte = 0;						

						//CString temp;
						//temp.Format("%s \n파일목록 받음", pIOData->myRecvData.pszFileSearchData);
						//AfxMessageBox(temp);

						char *pszFileSearchData;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
						pszFileSearchData = new char[strlen(pIOData->myRecvData.pszFileSearchData) + 1];						
						strcpy(pszFileSearchData, pIOData->myRecvData.pszFileSearchData);	
						CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
						pMain->PostMessage(UM_ADDITEMSEARCHFILE, (WPARAM)pszFileSearchData, (LPARAM)pClntData->index);	// LPARAM 은 서버의 인덱스번호

						delete [] pIOData->myRecvData.pszFileSearchData;
						pIOData->myRecvData.pszFileSearchData = NULL;
						
						break;
					}
				case P_DOWNLOAD_FILE_NOTICE:	// 보낼파일이있는지 없는지 확인
					{
						MY_FILE_DATA tempData;
						tempData.strFileFullName = szMessage;
						tempData.pFile = new CFile;						
						tempData.pFile->Open(szMessage, CFile::modeRead);	// CFile::typeBinary
						tempData.nFileSize = tempData.pFile->GetLength();
						if(tempData.pFile->m_hFile == CFile::hFileNull){	// 파일을 찾을수 없을시 경고창 처리를 하면 안되고 다시 찾을수 없다고 메시지를 보내야함
							if(threadObject->SendData(pClntData, pIOData, P_DOWNLOAD_FILE_ERROR, szMessage, strlen(szMessage), TRUE) == FALSE){	// 파일을 찾을수 없다고 알림
								continue;
							}			
							delete tempData.pFile;
							bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
							break;
						}
						
						tempData.nIndex = threadObject->m_nUploadMaxIndex;
						pIOData->nUploadFileIndex = threadObject->m_nUploadMaxIndex;

						tempData.nReceiveSize = 0;
												
						threadObject->m_uploadFileDataList.AddTail(tempData);	// 리스트에 데이타 저장

						// 파일의정보를 메시지로 보내 업로드리스트에 추가
						// 파일이름, 파일경로, 바이트수, 서버아이피, 서버이름
						CString strMessage;
						CString strTemp;
						strMessage.Format("%s", tempData.strFileFullName);
						INT cnt = strMessage.ReverseFind('\\');
						strMessage.Format("%s", strMessage.Right(strMessage.GetLength() - cnt - 1));	// 파일이름
						strMessage += "?";
						strMessage += tempData.strFileFullName.Left(cnt + 1);	// 파일경로
						strMessage += "?";
						strTemp.Format("%d", tempData.nFileSize);	// 바이트수
						strMessage += strTemp;
						strMessage += "?";
						char pzClntAddr[20];
						strcpy(pzClntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
						strTemp.Format("%s", pzClntAddr);
						strMessage += strTemp;
						strMessage += "?";
						for(int i=0; i<threadObject->m_nClntCount; i++){
							if(pClntData->index == threadObject->m_clntSock[i].pClntData->index){
								strMessage += threadObject->m_clntSock[i].clntName;
								break;
							}							
						}
						strMessage += "?";						
						char *pszMessage;
						pszMessage = new char[strMessage.GetLength() + 1];	// 여기서 동적으로생성후 받는쪽에서 delete
						strcpy(pszMessage, strMessage);						
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_ADD_UPLOAD_DATA, (WPARAM)pszMessage, (LPARAM)0);	// 업로드파일목록추가
						

						if(threadObject->SendData(pClntData, pIOData, P_SEND_FILE_READY, szMessage, tempData.pFile->GetLength(), TRUE) == FALSE){	// 파일전송
							continue;
						}					
						//file.Close();	// 파일전송확인하는부분에서 전송을 다하고 close함
						bIsRecvData = FALSE;	// 데이타를 send 하고 곧바로 recv 를 하면 flag 값이 변하므로 자신이 보낸 데이타를 자신이 받아본다
						break;
					}
				case P_SEND_FILE_READY:	// 파일을받기위해 파일을만들고 준비
					{
						MY_FILE_DATA tempData;
						CString strMessage;
						strMessage.Format("%s", szMessage);
						INT cnt = strMessage.ReverseFind('\\');
						tempData.strFileFullName.Format("%s\\%s", threadObject->m_strFolderTemp, strMessage.Right(strMessage.GetLength() - cnt - 1));
						tempData.nFileSize = nTotalByte;
						tempData.nReceiveSize = 0;
						tempData.nIndex = threadObject->m_nDownloadMaxIndex;
						pIOData->nDownloadFileIndex = threadObject->m_nDownloadMaxIndex;
						tempData.pFile = new CFile;
						tempData.pFile->Open(tempData.strFileFullName, CFile::modeCreate | CFile::modeWrite);							
						threadObject->m_downloadFileDataList.AddTail(tempData);	
						
						// 다운로드가 시작된다고 알림
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_ADD_DOWNLOAD_DATA, (WPARAM)tempData.nIndex, (LPARAM)0);
					}
					break;
				case P_DOWNLOAD_FILE_ERROR:	// 다운로드파일을 요청했지만 찾을수 없다고 메시지가옴
					{
						CString strMessage;
						strMessage.Format("%s", szMessage);
						INT cnt = strMessage.ReverseFind('\\');
						strMessage.Format("%s 파일을 찾을수 없습니다!", strMessage.Right(strMessage.GetLength() - cnt - 1));
						AfxMessageBox(strMessage);
					}
					break;
				case P_SEND_FILE:	// 파일전송받음
					{
						POSITION pos;
						MY_FILE_DATA *tempData = NULL;
						pos = threadObject->m_downloadFileDataList.GetHeadPosition();
						while(pos){	// 작업하고 있는파일찾기
							tempData = &(threadObject->m_downloadFileDataList.GetAt(pos));
							if(tempData->nIndex == pIOData->nDownloadFileIndex){
								break;
							}
							threadObject->m_downloadFileDataList.GetNext(pos);
						}
						
						// 이부분은원래는 없어도 되는데 데이타받는부분이 약간 부실해서 데이타를 다받았는데도 또 받는 처리부분이 실행되서 다받았다면 pass 하기위해
						if(tempData == NULL){	// 이미 m_downloadFileDataList 에 있는데이타를 지웠을경우, tempData 에는 데이타가 없음
							break;
						}
						
						if(nTotalByte <= BUFSIZE){	// 데이타를 한번만 받으면될때
							tempData->nReceiveSize += nTotalByte % BUFSIZE;	// 보낸데이타
							tempData->pFile->Write(pIOData->recvData.buffer, nTotalByte % BUFSIZE);	
						}else{	// 여러번 받을때
							if(nTotalByte <= tempData->nReceiveSize + BUFSIZE){	// 마지막일때
								tempData->pFile->Write(pIOData->recvData.buffer, nTotalByte - tempData->nReceiveSize);	// 바이러니파일은 데이타의 양을 알수가 없다 그래서 이렇게 계산
								tempData->nReceiveSize += nTotalByte - tempData->nReceiveSize;	// 보낸데이타								
							}else{	// 아직데이타를 더받아야할때
								tempData->pFile->Write(pIOData->recvData.buffer, BytesTransferred - sizeof(DWORD) - sizeof(UINT));
								tempData->nReceiveSize += BytesTransferred - sizeof(DWORD) - sizeof(UINT);	// 받은데이타								
							}							
						}

						//tempData->pFile->Write(pIOData->recvData.buffer, strlen(pIOData->recvData.buffer));
						//tempData->nReceiveSize += strlen(pIOData->recvData.buffer);
						

						if(nTotalByte > tempData->nReceiveSize){	// BytesTransferred 는 BUFSIZE 의 크기임
							break;	// 데이타는 더받기위해 break를 한다
						}

						tempData->nReceiveSize = nTotalByte;	// 다운로드목록에서 보이기 위해

						// 메인프레임의 popup
						char *strOut;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
						strOut = new char[256];						
						CString strFileName;
						strFileName.Format("%s", tempData->strFileFullName);
						INT cnt = strFileName.ReverseFind('\\');
						sprintf(strOut, "%s\n파일다운로드완료!", strFileName.Right(strFileName.GetLength() - cnt - 1));
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut, (LPARAM)POPUP_MESSEGE_DOWNLOADCOMPLETE);

						// 다운로드뷰에나오는 리스트에 다운됐다는것을알림
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_DOWNLOAD_COMPLETE, (WPARAM)tempData->nIndex, (LPARAM)0);
						//AfxMessageBox("파일전송완료!");

											
						tempData->pFile->Flush();
						tempData->pFile->Close();
						
						// 파일을 다받으면 템프폴더에서 incoming 폴더로 이동						
						CString strNewFile;
						strNewFile.Format("%s%s", threadObject->m_strFolderInComing, strFileName.Right(strFileName.GetLength() - cnt - 1));
						MoveFile(tempData->strFileFullName, strNewFile);

						delete tempData->pFile;
						threadObject->m_downloadFileDataList.RemoveAt(pos);	// 리스트에서 삭제							
					}
					break;
			} // switch()				
		} // if(pIOData->flag == SENDDATA)	
		if(bIsRecvData == TRUE){
			if(threadObject->RecvData(pClntData, pIOData) == FALSE){	// 데이타 WSARecv()
				continue;
			}
			
		}
	} // while(1)
	return 0;
}





// 자신이 서버로 접속했을시
BOOL CMyIOCPSocket::ServerConnect()
{	
	//SOCKADDR_IN servAddr;
	m_hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(m_hServSock == INVALID_SOCKET){
		ErrorHandling(1, "WSASocket()");
		return FALSE;
	}
	
	m_servAddr.sin_family=AF_INET;
	m_servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	m_servAddr.sin_port=htons(MYPORT);

	// bind()
	if(bind(m_hServSock, (SOCKADDR*)&m_servAddr, sizeof(m_servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling(1, "bind()");
		return FALSE;
	}

	// listen()
	if(listen(m_hServSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling(1, "listen()");
		return FALSE;
	}
	

	// 메인프레임의 statusBar
	if(m_bIsClient != 0){	// 서버로 접속시에만
		char *strOut;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
		char *strOut2;
		strOut = new char[256];
		strOut2 = new char[256];
		sprintf(strOut, "%s", "대기상태");	
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);	
		sprintf(strOut2, "%s", "접속 대기상태에 \n들어갑니다!");			
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_CONNECT);
	}


	return TRUE;
}

// 자신이 클라이언트로 접속했을시
BOOL CMyIOCPSocket::ClientConnect(CString strIp)
{
	SOCKET hClntSock;	
	SOCKADDR_IN clntAddr;
	MY_IO_DATA *pIOData;
	MY_CLIENT_DATA *pClntData;
	DWORD nRecvBytes = 0;	// 받은 데이타양
	DWORD flags = 0;
//AfxMessageBox("0");
	hClntSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(hClntSock == INVALID_SOCKET){
		ErrorHandling(2, "WSASocket()");
		return FALSE;
	}

	memset(&clntAddr, 0, sizeof(clntAddr));
	clntAddr.sin_family = AF_INET;
	clntAddr.sin_addr.s_addr = inet_addr(strIp);
	clntAddr.sin_port = htons(MYPORT);
//AfxMessageBox("1");
	// connect()
	if(connect(hClntSock, (SOCKADDR*)&clntAddr, sizeof(clntAddr)) == INVALID_SOCKET)
	{
		ErrorHandling(2, "connect()");		
		return FALSE;
	}
//AfxMessageBox("2");
		
	// 연결 된 클라이언트의 소켓 핸들 정보와 주소 정보를 설정.
	pClntData = (MY_CLIENT_DATA *)malloc(sizeof(MY_CLIENT_DATA));		
	pClntData->hClntSock = hClntSock;
	memcpy(&(pClntData->clntAddr), &clntAddr, sizeof(clntAddr));

	EnterCriticalSection(&m_cs);	// 크리티컬섹션 enter

	// 클라이언트정보 멤버변수 저장		
	// hClntSock 소켓이 지역변수이여서소멸되지만  맵버에 저장해도 계속사용할수 있음
	m_clntSock[m_nClntCount].pClntData = pClntData;	// 클라이언트로 접속을 할경우는 서버에 연결을 하는 소켓이 데이타 소켓이 된다
	m_clntSock[m_nClntCount].pClntData->index = m_nClntCount;
	m_nClntCount ++;	// 클라이언트수 하나증가

	LeaveCriticalSection(&m_cs);	// 크리티컬섹션 leave

	// Overlapped 소켓과 CompletionPort의 연결.
	CreateIoCompletionPort((HANDLE)hClntSock, this->m_hCompletionPort, (DWORD)pClntData,	// 쓰레드에전달
		0);	

	// 연결 된 클라이언트를 위한 버퍼를 설정 하고 OVERLAPPED 구조체 변수 초기화.
	pIOData=(MY_IO_DATA *)malloc(sizeof(MY_IO_DATA));
	memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));	
	memset(&(pIOData->mySendData), 0, sizeof(MY_SEND_DATA));
	memset(&(pIOData->myRecvData), 0, sizeof(MY_RECV_DATA));

	m_pClntIOData[m_nClntCount - 1] = pIOData;	
	
	EnterCriticalSection(&m_cs);	// 크리티컬섹션 enter

	// 자신의 이름과 폴더볼수있는지여부를 같이 보냄
	CString strTotalData;
	char szUserID[256];
	strTotalData += m_strUserID;
	strTotalData += "?";
	strTotalData += m_bViewFolder == 1 ? "1" : "0";
	strTotalData += "?";				
	sprintf(szUserID, "%s", strTotalData);
	DWORD nLength = strlen(szUserID);
		
	if(m_nClntCount == 1){	// 처음접속하므로 서버로 부터 다른컴들의 아이피를 받음
		// 클라이언트는 처음 서버에 접속할때 자신이 처음이라고 알리면 다른 컴들의 아이피를 받는다
		if(SendData(pClntData, pIOData, P_FIRST_CONNECT, szUserID, nLength, TRUE) == FALSE){
			ErrorHandling(2, "SendData()");		
		}
	}else{	// 처음이 아니라 이미서버로부터 다른컴들의 아이피를 받았으므로 접속만 요청
		if(SendData(pClntData, pIOData, P_NOFIRST_CONNECT, szUserID, nLength, TRUE) == FALSE){
			ErrorHandling(2, "SendData()");		
		}
	}

	LeaveCriticalSection(&m_cs);	// 크리티컬섹션 leave

	// 메인프레임의 statusBar
	char szClntAddr[20];
	char *strOut;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
	char *strOut2;
	strOut = new char[256];
	strOut2 = new char[256];
	sprintf(strOut, "연결됨 (서버:%d)", m_nClntCount);			
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
	strcpy(szClntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
	sprintf(strOut2, "%s 서버에\n접속되었습니다!", szClntAddr);			
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_CONNECT);


#ifdef MYSOCKETDEBUG		
	char temp2[20];
	strcpy(temp2, inet_ntoa(clntAddr.sin_addr));
	char out[256];
	sprintf(out, "%s 서버에 접속됨! \n클라이언트 수(자신은제외) : %d\n", temp2, m_nClntCount);
	//AfxMessageBox(out);
#endif

	m_bInitClntFlag = FALSE;	// InitSocket() 함수를 리턴시키기 위해

	return TRUE;
}

// 자신이 접속종료 소켓 종료
void CMyIOCPSocket::CloseSevrSock()
{
	if(m_hServSock != INVALID_SOCKET){
		closesocket(m_hServSock);
		m_hServSock = INVALID_SOCKET;
	}
	
	WSACleanup();

	DWORD dwExitCode;
	// CompletionThread 중지
	if(m_pCompletionThread != NULL){		
		GetExitCodeThread(m_pCompletionThread->m_hThread, &dwExitCode);
		TerminateThread(m_pCompletionThread->m_hThread, dwExitCode);
		CloseHandle(m_pCompletionThread->m_hThread);
		m_pCompletionThread = NULL;
	}

	// AcceptThread 중지		
	if(m_pAcceptThread != NULL){
		GetExitCodeThread(m_pAcceptThread->m_hThread, &dwExitCode);
		TerminateThread(m_pAcceptThread->m_hThread, dwExitCode);
		CloseHandle(m_pAcceptThread->m_hThread);	
		m_pAcceptThread = NULL;
	}


	m_nClntCount = 0;

	// 메인프레임의 statusBar
	char *strOut;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
	char *strOut2;
	strOut = new char[256];
	strOut2 = new char[256];
	sprintf(strOut, "%s", "연결안됨");	
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
	sprintf(strOut2, "%s", "접속해제되었습니다");
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_DISCONNECT);

#ifdef MYSOCKETDEBUG
	//AfxMessageBox("접속해제되었습니다");
#endif
	
}
 
	 
// 남이접속을 해제시.
void CMyIOCPSocket::CloseClntSock(MY_CLIENT_DATA *pClntData, MY_IO_DATA *pIOData)
{
	int index = pClntData->index;

	EnterCriticalSection(&m_cs);	// 크리티컬섹션 enter

	for(int i=index; i<m_nClntCount-1; i++){
		m_clntSock[i] = m_clntSock[i + 1];	
		m_pClntIOData[i] = m_pClntIOData[i + 1];
	}
	memset(&m_clntSock[m_nClntCount - 1] , 0, sizeof(MY_CLIENT_DETAIL_DATA));
	//memset(&m_pClntIOData[m_nClntCount - 1] , 0, sizeof(MY_IO_DATA));
	m_nClntCount --;	// 클라이언트수 하나감소

	LeaveCriticalSection(&m_cs);	// 크리티컬섹션 leave

	
	// 메인프레임의 statusBar
	char clntAddr[20];
	char *strOut;	// 메인쓰레드로 메시지를 보내야하는데 그냥보내면 메세지가 사라지는것같아서 동적으로 생성하고 메인프레임쪽에서 delete
	char *strOut2;
	strOut = new char[256];
	strOut2 = new char[256];
	strcpy(clntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
	if(m_nClntCount == 0){
		sprintf(strOut, "%s", "대기상태");	
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
		sprintf(strOut2, "%s 서버가\n접속해제되었습니다\n(접속된 서버없음)", clntAddr);			
	}else{
		sprintf(strOut, "연결됨 (서버:%d)", m_nClntCount);			
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
		sprintf(strOut2, "%s 서버가\n접속해제되었습니다", clntAddr);			
	}	
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_SERVER_DISCONNECT); 
	

#ifdef MYSOCKETDEBUG
	char temp2[20];
	CString temp;	
	strcpy(temp2, inet_ntoa(pClntData->clntAddr.sin_addr));
	temp.Format("%s 클라이언트 접속해제!\n 클라이언트 수(자신은제외) : %d", temp2, m_nClntCount);	// inet_ntoa() IP로변환			
	AfxMessageBox(temp);
	if(m_nClntCount == 0){
		AfxMessageBox("더이상 접속되있는 서버가 없습니다!");
	}	
#endif				

	closesocket(pClntData->hClntSock);
	free(pClntData);
	free(pIOData);
	pClntData = NULL;
	pIOData = NULL;
}





BOOL CMyIOCPSocket::SendData(MY_CLIENT_DATA *pClntData, MY_IO_DATA *pIOData, UINT nPacketHeader, char *message, DWORD nTotalByte, BOOL bFirst)
{	
	int result;
	DWORD nSendBytes  = 0;				
	memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
	memset(&(pIOData->sendData), 0, sizeof(MY_PACKET));
	pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
	pIOData->wsaBuf.buf = (char *)&(pIOData->sendData);			// buf 에 pIOData->data.buffer 를 넣게 되면 buffer은 걊을 받을수 있지만 nPacketHeader은 값을 받지 못한다 그래서 data 구조체의 주소를 대입해서 포인터 자체를 받아서 계산해서 데이타와 헤더까지 얻는다
	pIOData->flag = SENDDATA;	// 송신
	pIOData->sendData.nPacketHeader = nPacketHeader;	// 패킷헤더
	pIOData->sendData.nTotalByte = nTotalByte;	// 패킷 총길이
	
	strncpy(pIOData->sendData.buffer, message, BUFSIZE);
	pIOData->sendData.buffer[BUFSIZE] = '\0';
	
	if(bFirst == TRUE){	// 처음 보낸데이타라면
		pIOData->nSendByte = strlen(message);
	}else{
		pIOData->nSendByte += strlen(message);	//처음보내는 데이타가 아니면 얼마나 보내고 있는지 데이타를 더한다
	}

	result = WSASend(pClntData->hClntSock, &pIOData->wsaBuf, 1, &nSendBytes, 0, &pIOData->overlapped, NULL);
	pIOData->sendData.nTotalByte = nTotalByte;	// 패킷 총길이	
	if(result == SOCKET_ERROR){		
		if(WSAGetLastError() != WSA_IO_PENDING){
			int error = WSAGetLastError();
			char out[256];
			sprintf(out, "WSASend() error! [ErrorCode %d]", error);
			AfxMessageBox(out); 
			return FALSE;
		}
	}
	return TRUE;
}



BOOL CMyIOCPSocket::RecvData(MY_CLIENT_DATA *pClntData, MY_IO_DATA *pIOData)
{
	DWORD nRecvBytes = 0;	// 받은 데이타양
	DWORD flags = 0;

	// RECEIVE AGAIN, 계속해서 데이타를 입력받음					
	memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
//	memset(&(pIOData->recvData), 0, sizeof(MY_PACKET));	// 초기화를 해주면 안됨, 다못받은 데이타에 쓰레기값들어감
	memset(&(pIOData->recvData.buffer), 0, BUFSIZE + 1);	// 버퍼만 초기화
	pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
	pIOData->wsaBuf.buf = (char *)&(pIOData->recvData);		// buf 에 pIOData->data.buffer 를 넣게 되면 buffer은 걊을 받을수 있지만 nPacketHeader은 값을 받지 못한다 그래서 data 구조체의 주소를 대입해서 포인터 자체를 받아서 계산해서 데이타와 헤더까지 얻는다
	pIOData->flag = RECVDATA;	


	static DWORD nOldTotalByte;
	//static UINT nOldPacketHeader = 0;
	if(pIOData->recvData.nPacketHeader > 21){	// 파일전송		
		nOldTotalByte = nOldTotalByte;
		//nOldPacketHeader = 21;
	}else{
		nOldTotalByte = pIOData->recvData.nTotalByte;
	}
	
	
	int result = WSARecv(pClntData->hClntSock, 	// 데이터 입력 소켓.
			&(pIOData->wsaBuf),		// 데이터 입력 버퍼 포인터.
			1,						// 데이터 입력 버퍼의 수.
			&nRecvBytes,					// 전송된 바이트 수를 저장하기 위한 포인터
			&flags, 
			&(pIOData->overlapped),	// OVERLAPPED 구조체 포인터.
			NULL
			);
	if(result == SOCKET_ERROR){	
		if(WSAGetLastError() != ERROR_IO_PENDING){
			int error = WSAGetLastError();
			char out[256];
			sprintf(out, "WSARecv() error! [ErrorCode %d]", error);
			AfxMessageBox(out); 
			return FALSE;
		}
	}

	if(pIOData->recvData.nPacketHeader > 21){	// 파일전송
		pIOData->recvData.nPacketHeader = 21;
		pIOData->recvData.nTotalByte = nOldTotalByte;
	}






	return TRUE;
}

void CMyIOCPSocket::ErrorHandling(UINT errorCode, CString strPrn)
{
	m_nErrorCode = errorCode;	// 접속실패시 에러발생(정상:0, 서버접속에러:1, 클라리언트에러:2, Completion:3)

	
	if(m_bIsClient == 0){	// 클라이언트라면
		m_bInitClntFlag = FALSE;	// InitSocket() 함수를 리턴시키기 위해
	}
	
	int error = WSAGetLastError();
	char out[256];
	sprintf(out, "%s error! [ErrorCode %d]", strPrn, error);
	AfxMessageBox(out); 			
	
	// 다시 InitSocket() 함수를 호출하므로 초기화
	m_bInitClntFlag = TRUE;	// InitSocket() 함수를 리턴시키기 위해

	// 소켓 삭제
	for(int i=0; i<LIMITSOCKET; i++){	// 클라이언트 소켓은 있을수도 있고 없을수도 있다
		if(m_clntSock[i].pClntData != NULL){
			closesocket(m_clntSock[i].pClntData->hClntSock);
		}
	}
	if(m_hServSock != INVALID_SOCKET){
		closesocket(m_hServSock);
		m_hServSock = INVALID_SOCKET;
	}
	
	WSACleanup();

	DWORD dwExitCode;
	// CompletionThread 중지
	if(m_pCompletionThread != NULL){		
		GetExitCodeThread(m_pCompletionThread->m_hThread, &dwExitCode);
		TerminateThread(m_pCompletionThread->m_hThread, dwExitCode);
		CloseHandle(m_pCompletionThread->m_hThread);
		m_pCompletionThread = NULL;
	}

	// AcceptThread 중지		
	if(m_pAcceptThread != NULL){
		GetExitCodeThread(m_pAcceptThread->m_hThread, &dwExitCode);
		TerminateThread(m_pAcceptThread->m_hThread, dwExitCode);
		CloseHandle(m_pAcceptThread->m_hThread);	
		m_pAcceptThread = NULL;
	}

	
}

// 파일검색
BOOL CMyIOCPSocket::FindAlltFile(char* pszPath, char *pszName, CList<MY_SEARCH_FILE , MY_SEARCH_FILE& >*pFileList)
{
	UINT nCnt = 0;
    char szPath[256];
    memset(szPath, 0, sizeof(szPath));
    strcpy(szPath, pszPath);
    if(strcmp(&szPath[strlen(szPath)-1],  "\\") != 0) {		
        strcat(szPath, "\\");
   }
    strcat(szPath, "*.*");

	char szName[256];	// 파일명
	char szExt[3];	// 화장자
	CString strName;
	strName.Format("%s", pszName);
	int result = strName.Find('.', 0);
	if(result == -1){	// 화장자가 없을경우
		strcpy(szName, pszName);
		strcpy(szExt, "*");		
	}else{
		strcpy(szExt, strName.Mid(result + 1, strName.GetLength() - result - 1));
		strcpy(szName, strName.Left(result));		
	}
	if(strcmp(szExt, "*") == 0  &&  strcmp(szName, "*") == 0){	// *.* 은 검색안됨
		return FALSE;
	}

    CFileFind ff;
	CString strByte;
	CString strFilename;
	CString strFilePath;
	int nR;
	MY_SEARCH_FILE tempFile;
	CString strTempPath;
    BOOL bWorking = ff.FindFile(szPath);
    while(bWorking) {
		bWorking = ff.FindNextFile();
        strFilename = ff.GetFileName();            // Filename + Ext
        strFilePath = ff.GetFilePath();            // FullPath + Filename + Ext
        if(ff.IsDirectory()){	// 폴더라면
            if(!ff.IsDots()){
				FindAlltFile(strFilePath.GetBuffer(0), pszName, pFileList);	// 하위 폴더의 내용도 검색하기 위해 자기 자신을 호출한다.				
            }
        }else{	// 파일이라면
			CString strName = ff.GetFileName();
            result = strName.Find('.');
            if(result >= 0){	// 확장가자 있는것만 검색
				if(strcmp(szExt, "*") == 0){	// 확장자가 * 라면
					nR = strnicmp(strName.Left(result), szName, strlen(szName));
					if(stricmp(strName.Left(result), szName) == 0 || nR == 0){	// 파일명이 같거나 앞에서부터 검색글자만큼 같다면						
						strTempPath = strFilePath;						
						strTempPath = strTempPath.Left(strTempPath.Find(strFilename, 0));						

						tempFile.nByte = ff.GetLength();
						tempFile.strFileName = strFilename;
						tempFile.strFilePath = strTempPath;						

						pFileList->AddTail( tempFile );		
						//strByte.Format("%d", ff.GetLength());
						//TRACE(strFilePath + "\t\t" + strByte + "Byte" + "\n");
					}
				}else if(strcmp(szName, "*") == 0){	// 파일명이 * 이라면
					if(stricmp(strName.Mid(result + 1, strName.GetLength() - result - 1), szExt) == 0){	// 확장자가 같으면
						//strByte.Format("%d", ff.GetLength());						
						//TRACE(strFilePath + "\t\t" + strByte + "Byte" + "\n");						
						strTempPath = strFilePath;						
						strTempPath = strTempPath.Left(strTempPath.Find(strFilename, 0));						

						tempFile.nByte = ff.GetLength();
						tempFile.strFileName = strFilename;
						tempFile.strFilePath = strTempPath;						

						pFileList->AddTail( tempFile );						
					}
				}else if(strName.Mid(result + 1).CompareNoCase(szExt) == 0){	// 파일명과 확장자가 같다면
					nR = strnicmp(strName.Left(result), szName, strlen(szName));
                    if(stricmp(strName.Left(result), szName) == 0  ||  nR == 0){	// 파일명이 같거나 앞에서부터 검색글자만큼 같다면
						if(stricmp(strName.Mid(result + 1, strName.GetLength() - result - 1), szExt) == 0){	// 확장자가 같으면
							//strByte.Format("%d", ff.GetLength());
							//TRACE(strFilePath + "\t\t" + strByte + "Byte" + "\n");
							strTempPath = strFilePath;						
							strTempPath = strTempPath.Left(strTempPath.Find(strFilename, 0));						

							tempFile.nByte = ff.GetLength();
							tempFile.strFileName = strFilename;
							tempFile.strFilePath = strTempPath;						

							pFileList->AddTail( tempFile );		
						}
					}
                }
            }			
		}
    }
	return TRUE;
}

