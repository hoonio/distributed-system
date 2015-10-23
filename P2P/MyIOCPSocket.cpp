// MyIOCPSocket.cpp: implementation of the CMyIOCPSocket class.
//
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
////
////					[ErrorCode]
////	10061 => ����õ��� ���������� �źε˴ϴ�
////	10004 => WSACancelBlockingCall() �Լ��� ȣ���ؼ� ���ŷ���ϵ����� ����
////
///////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************************
����Ÿ�� ������ ��Ӻ����Ƿ� �޴��ʿ��� ó���� �ϱ����� �� �޾����� ������ ����Ÿ�� ���ѹ�����(�׳� ������)
�׷��� Sleep ���־����� 10���� �����ٸ� �ð��� �ʹ� ��� �Դ´�(2���� �ĸ���)
���۸� ũ�� ������ ���� ���Ƶ� �������
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
	if((rear + 1) % MAXQUEUE == front){  //queue�� ��á��??
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
	
	
	
	InitializeCriticalSection(&m_cs);	// ũ��Ƽ�ü��� START
}

CMyIOCPSocket::~CMyIOCPSocket()
{
	DeleteCriticalSection(&m_cs);	// ũ��Ƽ�ü��� END
}

void CMyIOCPSocket::InitSock()
{
	m_nErrorCode = 0;	//�����ڵ�
	m_nClntCount = 0;
	WSADATA	wsaData;
	SYSTEM_INFO SystemInfo;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){	// Load Winsock 2.2 DLL 
		ErrorHandling(1, "WSAStartup()");
	}

	//1. Completion Port ����.
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(m_hCompletionPort == NULL){
		ErrorHandling(3, "CreateIoCompletionPort()");
	}
	GetSystemInfo(&SystemInfo);

	//2. Completion Port ���� ����� �ϷḦ ��� �ϴ� �����带 CPU ���� ��ŭ ����.
	for(int i=0; i<(int)SystemInfo.dwNumberOfProcessors; i++){
		//_beginthreadex(NULL, 0, CompletionThread, (LPVOID)m_hCompletionPort, 0, NULL);
		m_pCompletionThread = AfxBeginThread(CompletionThread, (LPVOID)this);	
	}

	m_pAcceptThread = AfxBeginThread(AcceptThread, (LPVOID)this);	// �������ϻ���, Ŭ���̾�Ʈ ���� ���
	if(m_bIsClient == 0){	// Ŭ���̾�Ʈ���
		while(m_bInitClntFlag == TRUE){
		//	Sleep(100);
		}
	}
}




// �������� ���� ��� ������ (Ŭ���̾�Ʈ�� ���ӽø��� ���� (while ��))
UINT AcceptThread(LPVOID param)
{	
	CMyIOCPSocket *threadObject = (CMyIOCPSocket *)param;	// param ���� this �� �޾Ƽ� Ŭ���� ����

	MY_IO_DATA *pIOData;
	MY_CLIENT_DATA *pClntData;
	DWORD flags = 0;
	DWORD nRecvBytes = 0;
	HANDLE hResult;


	if(threadObject->m_bIsClient == 0){	// Ŭ���̾�Ʈ���
		threadObject->ClientConnect(threadObject->m_strSevrIP);	// �ڽ��� Ŭ���̾�Ʈ�� ���ӽ�
	}
	threadObject->ServerConnect();	// �������� START (������ Ŭ���̾�Ʈ�� ���������� ������ �־����)	

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
		
		// ���� �� Ŭ���̾�Ʈ�� ���� �ڵ� ������ �ּ� ������ ����.
		pClntData = (MY_CLIENT_DATA *)malloc(sizeof(MY_CLIENT_DATA));		
		pClntData->hClntSock = hClntSock;
		memcpy(&(pClntData->clntAddr), &clntAddr, addrLen);
		
		EnterCriticalSection(&(threadObject->m_cs));	// ũ��Ƽ�ü��� enter

		threadObject->m_clntSock[threadObject->m_nClntCount].pClntData = pClntData;
		threadObject->m_clntSock[threadObject->m_nClntCount].pClntData->index = threadObject->m_nClntCount;			

		LeaveCriticalSection(&(threadObject->m_cs));	// ũ��Ƽ�ü��� leave

		//3. Overlapped ���ϰ� CompletionPort�� ����.
		hResult = CreateIoCompletionPort((HANDLE)hClntSock, *pHCompletionPort, (DWORD)pClntData,	// �����忡����
			0);
		if(hResult == NULL){
			threadObject->ErrorHandling(3, "CreateIoCompletionPort()");
		}
		
		// ���� �� Ŭ���̾�Ʈ�� ���� ���۸� ���� �ϰ� OVERLAPPED ����ü ���� �ʱ�ȭ.
		pIOData=(MY_IO_DATA *)malloc(sizeof(MY_IO_DATA));
		memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));	
		memset(&(pIOData->mySendData), 0, sizeof(MY_SEND_DATA));
		memset(&(pIOData->myRecvData), 0, sizeof(MY_RECV_DATA));

		threadObject->m_pClntIOData[threadObject->m_nClntCount] = pIOData;
				
		EnterCriticalSection(&(threadObject->m_cs));	// ũ��Ƽ�ü��� enter

		threadObject->m_nClntCount ++;	// Ŭ���̾�Ʈ�� �ϳ�����
	
		LeaveCriticalSection(&(threadObject->m_cs));	// ũ��Ƽ�ü��� leave
		
		// ������ �������ڸ��� ���Ŵ�⸦ �Ѵ�(ó�� Ŭ���̾�Ʈ�� ������ Ŭ���̾�Ʈ�� �ٸ����� �����Ǹ� ��û�Ҽ� �����Ƿ�)
		if(threadObject->RecvData(pClntData, pIOData) == FALSE){	// ����Ÿ WSARecv()
			continue;
		}

		
		// ������������ statusBar
		char pzClntAddr[20];
		char *strOut;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
		char *strOut2;
		strOut = new char[256];
		strOut2 = new char[256];
		sprintf(strOut, "����� (����:%d)", threadObject->m_nClntCount);	
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
		strcpy(pzClntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
		sprintf(strOut2, "%s ������\n���ӵǾ����ϴ�!", pzClntAddr);
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_SERVER_CONNECT);

#ifdef MYSOCKETDEBUG
	char temp2[20];
	strcpy(temp2, inet_ntoa(clntAddr.sin_addr));
	char out[256];
	sprintf(out, "%s Ŭ���̾�Ʈ ���ӵ�! \nŬ���̾�Ʈ ��(�ڽ�������) : %d\n", temp2, threadObject->m_nClntCount);
	AfxMessageBox(out);
#endif
	}
}



//�� ��� �Ϸῡ ���� �������� �ൿ ���� 
UINT CompletionThread(LPVOID param)
{	
	CMyIOCPSocket *threadObject = (CMyIOCPSocket *)param;	// param ���� this �� �޾Ƽ� Ŭ���� ����
	HANDLE *pHCompletionPort = &(threadObject->m_hCompletionPort);
	DWORD BytesTransferred;
	MY_CLIENT_DATA *pClntData;
	MY_IO_DATA *pIOData;
	DWORD nRecvBytes = 0;	// ���� ����Ÿ��
	DWORD nSendBytes = 0;
	DWORD flags = 0;
	BOOL bIsRecvData = TRUE;	// recv �� �Ұ�����?
	//CFile *pFile;	
	

	while(1)
	{
		bIsRecvData = TRUE;
		// 5. ������� �Ϸ� �� ������ ���� ����. 
		GetQueuedCompletionStatus(*pHCompletionPort,			 // Completion Port
								  &BytesTransferred,		 // ����� �������� ���� �� ����Ʈ ��
								  (LPDWORD)&pClntData,		// CreateIoCompletionPort() ���� ������ ����
								  (LPOVERLAPPED*)&pIOData, // OVERLAPPED ����ü ������.
								  INFINITE);
		
		if(pIOData->flag != SENDDATA  &&  pIOData->flag != RECVDATA){	// �������������� ���� recv�� 2���̶�� ����... �׷��� �̹� free �� pIOData�� ���� ���ϰ�
			continue;
		}
		if(BytesTransferred == 0){
			threadObject->CloseClntSock(pClntData, pIOData);	// Ŭ���̾�Ʈ ������ ���涧 (���� ������ ����������)
			continue;
		}
	



		//UINT aa = pIOData->wsaBuf.buf[0] + sizeof(BUFSIZE + 1);
		//DWORD bb = pIOData->wsaBuf.buf[0] + sizeof(BUFSIZE + 1) + sizeof(UINT);
		

		//////////////////////////////////////////////// �۽��� �Ϸ�Ǿ�����///////////////////////////////////////////////
		if(pIOData->flag == SENDDATA){	
			char szMessage[BUFSIZE];	
			DWORD nTotalByte = pIOData->sendData.nTotalByte;	// ���� �� ����Ʈ
			UINT nPacketHeader = pIOData->sendData.nPacketHeader;	// �ٸ��Ŀ��� ��������ŸȮ��, (����Ȯ�ν� ���)
			DWORD nSendByte = pIOData->nSendByte;	// ��������Ʈ
			strcpy(szMessage, pIOData->sendData.buffer);	// szMessage �������� �޽�

			//CString strTrace;
			//strTrace.Format("========��������Ÿ START========\n %s \n========��������Ÿ END========\n", szMessage);
			//TRACE(strTrace);	// trace �� �������ڴ� ��¸���


			//	�ڱ������� �Ϸ�Ǿ��ٰ� �˷��ָ鼭 ����������Ÿ���ִ��� Ȯ��
			switch(nPacketHeader){
				case P_FIRST_CONNECT:	// ó�����ӽ� ����
					pIOData->flag = RECVDATA;	// ��ٷ� ����Ÿ�� �޾ƾ��ϹǷ� flag ����
					break;
				case P_NOFIRST_CONNECT:	// �ٸ� ������ ���ӽ� ����
					pIOData->flag = RECVDATA;	// ��ٷ� ����Ÿ�� �޾ƾ��ϹǷ� flag ����
					break;	
				case P_SEND_ALL_IP:	// ��ó�� ���ӽ� �ٸ���� �ĵ��� �����Ǹ� ����
					{
						if(nTotalByte > nSendByte){	// ������ ����Ÿ�� �ִٸ�							
							char szSend[BUFSIZE + 1];	// ���������� �������� ����Ÿ
							CString strData;
							strData.Format("%s", pIOData->mySendData.pszTotalIPData);
							strncpy(szSend, strData.Mid(BUFSIZE * (nSendByte / BUFSIZE), BUFSIZE), BUFSIZE);							
							szSend[BUFSIZE] = '\0';
							//Sleep(200);	// ��ٷ� send �� �ϸ� �޴��ʿ��� ����Ÿ�� �ٲ�Ƿ� �ణ�ǽð�����
							if(threadObject->SendData(pClntData, pIOData, P_SEND_ALL_IP, szSend, strData.GetLength(), FALSE) == FALSE){	// ��� ���� �����Ǹ� ���������� Ŭ���̾�Ʈ���� ����(������ Ŭ���̾�Ʈ�� ������ �󳻿��� ����)
								continue;
							}							
							bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
						}else{
							delete []pIOData->mySendData.pszTotalIPData;
							pIOData->mySendData.pszTotalIPData = NULL;
						}						
					}
					//bIsRecvData = FALSE;	// TEST
					break;
				case P_SEARCH_FILE:	// �ڽ��� �˻���û��
					bIsRecvData = FALSE;	// �̹� recv �ѹ� �Ȼ���
					pIOData->flag = RECVDATA;	// ��ٷ� ����Ÿ�� �޾ƾ��ϹǷ� flag ����
					break;
				case P_SEARCH_FILE_LIST:	// �˻������ϸ�� 
					{											
						if(nTotalByte > nSendByte){	// ������ ����Ÿ�� �ִٸ�							
							char szSend[BUFSIZE + 1];	// ���������� �������� ����Ÿ
							CString strFileSearchData;
							strFileSearchData.Format("%s", pIOData->mySendData.pszFileSearchData);
							strncpy(szSend, strFileSearchData.Mid(BUFSIZE * (nSendByte / BUFSIZE), BUFSIZE), BUFSIZE);							
							szSend[BUFSIZE] = '\0';							
							if(threadObject->SendData(pClntData, pIOData, P_SEARCH_FILE_LIST, szSend, strFileSearchData.GetLength(), FALSE) == FALSE){	// ��� ���� �����Ǹ� ���������� Ŭ���̾�Ʈ���� ����(������ Ŭ���̾�Ʈ�� ������ �󳻿��� ����)
								continue;
							}							
							bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
						}else{
							delete []pIOData->mySendData.pszFileSearchData;
							pIOData->mySendData.pszFileSearchData = NULL;
						}						
					}
					//bIsRecvData = FALSE;
					break;
				case P_DOWNLOAD_FILE_NOTICE:	// ������ �ٿ�ε�ޱ����� ������ ���ϸ�, ��� ����
					bIsRecvData = FALSE;	// �̹� recv �ѹ� �Ȼ���
					pIOData->flag = RECVDATA;	// ��ٷ� ����Ÿ�� �޾ƾ��ϹǷ� flag ����
					break;		
				case P_SEND_FILE_READY:	// ������ �ִٴ� �޼����� ������ Ȯ���ϸ鼭 �������� ����
					{
						// ���ϵ���Ÿ ����
						POSITION pos;
						MY_FILE_DATA *tempData;
						pos = threadObject->m_uploadFileDataList.GetHeadPosition();
						while(pos){	// �۾��ϰ� �ִ�����ã��
							tempData = &(threadObject->m_uploadFileDataList.GetAt(pos));
							if(tempData->nIndex == pIOData->nUploadFileIndex){
								break;
							}
							threadObject->m_uploadFileDataList.GetNext(pos);
						}


						char szSendFileData[BUFSIZE + 1];
						tempData->pFile->Read(szSendFileData, BUFSIZE);

						if(nTotalByte <= BUFSIZE){	// ������ ���� NULL ó���� ���ϹǷ� ���⼭ ����� �ؼ� ���������� NULL ���
							szSendFileData[nTotalByte % BUFSIZE] = '\0';															
							tempData->nReceiveSize += nTotalByte % BUFSIZE;	// ��������Ÿ
						}else{								
							szSendFileData[BUFSIZE] = '\0';												
							tempData->nReceiveSize += BUFSIZE;	// ��������Ÿ
						}
						


						/* ������ �����Ҷ��� ���̳ʸ� ���� �����ҋ� sendData() �Լ��� ȣ���ϸ� ���̳ʸ����� NULL �������ϹǷ� ������ ����Ƿ� ���⼭ �Լ��κ��� �����ؼ� ó�� */
						BOOL bFirst = TRUE;
						int result;
						DWORD nSendBytes  = 0;				
						memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
						memset(&(pIOData->sendData), 0, sizeof(MY_PACKET));
						pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
						pIOData->wsaBuf.buf = (char *)&(pIOData->sendData);			// buf �� pIOData->data.buffer �� �ְ� �Ǹ� buffer�� �r�� ������ ������ nPacketHeader�� ���� ���� ���Ѵ� �׷��� data ����ü�� �ּҸ� �����ؼ� ������ ��ü�� �޾Ƽ� ����ؼ� ����Ÿ�� ������� ��´�
						pIOData->flag = SENDDATA;	// �۽�
						pIOData->sendData.nPacketHeader = P_SEND_FILE;	// ��Ŷ���
						pIOData->sendData.nTotalByte = tempData->pFile->GetLength();	// ��Ŷ �ѱ���
						//strncpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
						memcpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
						pIOData->sendData.buffer[BUFSIZE] = '\0';
						if(bFirst == TRUE){	// ó�� ��������Ÿ���
							pIOData->nSendByte = strlen(szSendFileData);
						}else{
							pIOData->nSendByte += strlen(szSendFileData);	//ó�������� ����Ÿ�� �ƴϸ� �󸶳� ������ �ִ��� ����Ÿ�� ���Ѵ�
						}
						result = WSASend(pClntData->hClntSock, &pIOData->wsaBuf, 1, &nSendBytes, 0, &pIOData->overlapped, NULL);
						pIOData->sendData.nTotalByte = nTotalByte;	// ��Ŷ �ѱ���	
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


						//tempData->nReceiveSize += strlen(szSendFileData);	// ��������Ÿ
						/**
						if(threadObject->SendData(pClntData, pIOData, P_SEND_FILE, szSendFileData, tempData->pFile->GetLength(), TRUE) == FALSE){	// ��������
							continue;
						}					
						**/
						bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
						
					}
					break;
				case P_DOWNLOAD_FILE_ERROR:	// �ٿ�ε� ������ ã���� ���ٰ� �޽����� ����
					pIOData->flag = RECVDATA;	// ��ٷ� ����Ÿ�� �޾ƾ��ϹǷ� flag ����
					break;
				case P_SEND_FILE:	// ��������
					{
						POSITION pos;
						MY_FILE_DATA *tempData;
						pos = threadObject->m_uploadFileDataList.GetHeadPosition();
						while(pos){	// �۾��ϰ� �ִ�����ã��
							tempData = &(threadObject->m_uploadFileDataList.GetAt(pos));
							if(tempData->nIndex == pIOData->nUploadFileIndex){
								break;
							}
							threadObject->m_uploadFileDataList.GetNext(pos);
						}
						
						if(nTotalByte > tempData->nReceiveSize){	// ������ ����Ÿ�� �ִٸ�
							// ���ϵ���Ÿ ����
							char szSendFileData[BUFSIZE + 1];	// ���������� �������� ����Ÿ								
							szSendFileData[0] = '\0';
							tempData->pFile->Read(szSendFileData, BUFSIZE);
							
							if(nTotalByte <= nSendByte + BUFSIZE){	// ������ ���� NULL ó���� ���ϹǷ� ���⼭ ����� �ؼ� ���������� NULL ���
								szSendFileData[nTotalByte % BUFSIZE] = '\0';
								tempData->nReceiveSize += nTotalByte % BUFSIZE;	// ��������Ÿ
							}else{								
								szSendFileData[BUFSIZE] = '\0';								
								tempData->nReceiveSize += BUFSIZE;	// ��������Ÿ
							}
							Sleep(30);	// ���⼭ sleep�� ���ָ� ����Ÿ�� ū���� ������ �޴��� ť ���۰� ���ڶ� �׷��� ����� ������ ����
							
							//tempData->nReceiveSize += strlen(szSendFileData);	// ��������Ÿ

							/* ������ �����Ҷ��� ���̳ʸ� ���� �����ҋ� sendData() �Լ��� ȣ���ϸ� ���̳ʸ����� NULL �������ϹǷ� ������ ����Ƿ� ���⼭ �Լ��κ��� �����ؼ� ó�� */
							BOOL bFirst = FALSE;
							int result;
							DWORD nSendBytes  = 0;				
							memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
							memset(&(pIOData->sendData), 0, sizeof(MY_PACKET));
							pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
							pIOData->wsaBuf.buf = (char *)&(pIOData->sendData);			// buf �� pIOData->data.buffer �� �ְ� �Ǹ� buffer�� �r�� ������ ������ nPacketHeader�� ���� ���� ���Ѵ� �׷��� data ����ü�� �ּҸ� �����ؼ� ������ ��ü�� �޾Ƽ� ����ؼ� ����Ÿ�� ������� ��´�
							pIOData->flag = SENDDATA;	// �۽�
							pIOData->sendData.nPacketHeader = P_SEND_FILE;	// ��Ŷ���
							pIOData->sendData.nTotalByte = tempData->pFile->GetLength();	// ��Ŷ �ѱ���
							//strncpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
							memcpy(pIOData->sendData.buffer, szSendFileData, BUFSIZE);
							pIOData->sendData.buffer[BUFSIZE] = '\0';
							if(bFirst == TRUE){	// ó�� ��������Ÿ���
								pIOData->nSendByte = strlen(szSendFileData);
							}else{
								pIOData->nSendByte += strlen(szSendFileData);	//ó�������� ����Ÿ�� �ƴϸ� �󸶳� ������ �ִ��� ����Ÿ�� ���Ѵ�
							}
							result = WSASend(pClntData->hClntSock, &pIOData->wsaBuf, 1, &nSendBytes, 0, &pIOData->overlapped, NULL);
							pIOData->sendData.nTotalByte = nTotalByte;	// ��Ŷ �ѱ���	
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
							if(threadObject->SendData(pClntData, pIOData, P_SEND_FILE, szSendFileData, tempData->pFile->GetLength(), FALSE) == FALSE){	// ��������
								continue;
							}
							**/
							bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
							//AfxMessageBox(szSendFileData);
						}else{
							// ���信������ ����Ʈ�� ���ε�ƴٴ°����˸�
							((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_UPLOAD_COMPLETE, (WPARAM)tempData->nIndex, (LPARAM)0);
						
							tempData->nReceiveSize = tempData->pFile->GetLength();

							tempData->pFile->Close();	// ������ �� �����Ŀ� ������ �ݴ���	
							delete tempData->pFile;
							threadObject->m_uploadFileDataList.RemoveAt(pos);	// ����Ʈ���� ����	
							//AfxMessageBox("�������ۿϷ�");							
							bIsRecvData = TRUE;
						}
					}
					break;
			}	// if(pIOData->flag == SENDDATA)	
		///////////////////////////////////// �ڽſ��� �޽����� ������ ó��(�޽��� ���ŵ�) ///////////////////////////////////////////
		}else if(pIOData->flag == RECVDATA){	
			char szMessage[BUFSIZE];	
			DWORD nTotalByte = pIOData->recvData.nTotalByte;	// ���� �� ����Ʈ
			UINT nPacketHeader = pIOData->recvData.nPacketHeader;	// �ٸ��Ŀ��� ��������ŸȮ��, (����Ȯ�ν� ���)
			DWORD nSendByte = pIOData->nSendByte;	// ��������Ʈ
			strcpy(szMessage, pIOData->recvData.buffer);	// szMessage �������� �޽�

			//CString strTrace;
			//strTrace.Format("========��������Ÿ START========\n %s \n========��������Ÿ END========\n", szMessage);
			//TRACE(strTrace);	// trace �� �������ڴ� ��¸���


			switch(nPacketHeader){	// ����� üũ				
				case P_FIRST_CONNECT:	// Ŭ���̾�Ʈ�� ó�� ���ӽ� �ٸ��ĵ��� �����Ǹ� ��û, // �ٸ��ĵ��� �����Ǹ� ����
					{						
						TRACE("P_FIRST_CONNECT �����ø� �޾Ƽ� �ٽ� ip �� ����");

						EnterCriticalSection(&(threadObject->m_cs));	// ũ��Ƽ�ü��� enter

						// ���� ���̵�� nViewFolder ������,
						CString strMessage;
						strMessage.Format("%s", szMessage);
						int cnt = strMessage.Find('?', 0);	
						int oldCnt = 0;
						strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(0, cnt));	// Ŭ���̾�Ʈ�ʿ������� ID ������
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						threadObject->m_clntSock[threadObject->m_nClntCount - 1].bViewFolder = (strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1) == "1" ? 1 : 0);
	
						//////////////////////////////// �����޽��� ���� // message => ex) 2?122.233.43.212?123.21.223.221?
						CString strTotalData;	// �ѵ���Ÿ�� ����ٰ� �������� ������ ���̸� ��ġ�� �ɰ��� ������
						int len = 0;
						CString strNum;						
						
						// Ŭ���̾�Ʈ�� ����
						strNum.Format("%d", threadObject->m_nClntCount - 1);						
						strTotalData += strNum;
						strTotalData += "?";						

						// �ڽ��� �̸��� viewFolder, �ڽ��� �����Ǵ� �̹̾˰� �����Ƿ� ip �� �Ⱥ���						
						strTotalData += threadObject->m_strUserID;
						strTotalData += "?";
						strTotalData += (threadObject->m_bViewFolder == 1 ? "1" : "0");
						strTotalData += "?";						

						// �ٸ����� ip �� �̸�, viewFolder �� ����
						for(int i=0; i<threadObject->m_nClntCount-1; i++){	// ������ Ŭ���̾�Ʈ�� �޽����� �޴� Ŭ���̾�Ʈ�̹Ƿ� -1
							// ip
							strTotalData += inet_ntoa(threadObject->m_clntSock[i].pClntData->clntAddr.sin_addr);
							strTotalData += "?";

							// �̸�
							strTotalData += threadObject->m_clntSock[i].clntName;							
							strTotalData += "?";

							// nViewFolder
							strTotalData += (threadObject->m_clntSock[i].bViewFolder == 1 ? "1" : "0");		
							strTotalData += "?";
						}
					
						pIOData->mySendData.pszTotalIPData = new char[strTotalData.GetLength() + 1];	// ����Ÿ�� �ٺ����� ���� �ƴϹǷ� ��������
						strcpy(pIOData->mySendData.pszTotalIPData, strTotalData);

						char szSendip[BUFSIZE + 1];	// ���������� �������� ����Ÿ

						LeaveCriticalSection(&(threadObject->m_cs));	// ũ��Ƽ�ü��� leave

						strncpy(szSendip, strTotalData.Mid(0, BUFSIZE), BUFSIZE);							
						szSendip[BUFSIZE] = '\0';
						//Sleep(100);
						if(threadObject->SendData(pClntData, pIOData, P_SEND_ALL_IP, szSendip, strTotalData.GetLength(), TRUE) == FALSE){	// ��� ���� �����Ǹ� ���������� Ŭ���̾�Ʈ���� ����(������ Ŭ���̾�Ʈ�� ������ �󳻿��� ����)
							continue;
						}
						//AfxMessageBox(strTotalData + "  \n����");
						bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
					}		
					break;
				case P_NOFIRST_CONNECT:	// Ŭ���̾�Ʈ�� �̹� �������� �����Ǹ� �޾Ƽ� ���ӿ�û�̹Ƿ� �׳� ���ӿ�û�� ����
					{
						// Ŭ���̾�Ʈ�ʿ������� ID�� bViewFolder ������						
						int cnt = 0;
						int oldCnt = 0;
						CString strMessage;
						strMessage.Format("%s", szMessage);
						cnt = strMessage.Find('?', 0);						
						strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(0, cnt));

						// Ŭ���̾�Ʈ�������� viewFolder ����
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
						if(nTotalByte > strlen(pIOData->myRecvData.pszTotalIPData)){	// BytesTransferred �� BUFSIZE �� ũ����
							break;	// ����Ÿ�� ���ޱ����� break�� �Ѵ�
						}
						
						INT cnt;
						CString strMessage;
						strMessage.Format("%s", pIOData->myRecvData.pszTotalIPData);
						cnt = strMessage.Find('?', 0);		

						//AfxMessageBox(strMessage + "  \n����");
						
						// �������� ������ �ɰ���
						char comCnt[3];
						if(cnt == 1){	// �ļ��� ���ڸ��϶�
							comCnt[0] = pIOData->myRecvData.pszTotalIPData[0];
							comCnt[1] = '\0';
						}else if(cnt == 2){	// �ļ��� ���ڸ��϶�(2�ڸ� �̻��� ����)
							comCnt[0] = pIOData->myRecvData.pszTotalIPData[0];
							comCnt[1] = pIOData->myRecvData.pszTotalIPData[1];		
							comCnt[2] = '\0';
						}

						// Ŭ���̾�Ʈ�ʿ������� ID ������
						int oldCnt = 0;
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						strcpy(threadObject->m_clntSock[threadObject->m_nClntCount - 1].clntName, strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1));

						// Ŭ���̾�Ʈ�������� viewFolder ����
						oldCnt = cnt;
						cnt = strMessage.Find('?', oldCnt + 1);
						threadObject->m_clntSock[threadObject->m_nClntCount - 1].bViewFolder = (strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1) == "1" ? 1 : 0);						
											
						CString clntSock[LIMITSOCKET];
						for(int i=0;i<atoi(comCnt); i++){	// ������ Ŭ���̾�Ʈ �ۿ� ������ comCnt �� 0 , �׷��� ó������
							// ip
							oldCnt = cnt;
							cnt = strMessage.Find('?', oldCnt + 1);							
							clntSock[i] = strMessage.Mid(oldCnt + 1, cnt - oldCnt - 1);	
							threadObject->ClientConnect(clntSock[i]);	// ���� ������ŭ ���ӿ�û							

							// �̸�
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
						//temp.Format("%s\nP_SEND_ALL_IP �����ø� ����(���� ������ ó��)", szMessage);
						//TRACE(temp);

						break;		
					}			
				case P_SEARCH_FILE:	// ���ϰ˻���
					{	
						char szShareFolder[256];
						sprintf(szShareFolder, "%s", threadObject->m_strFolderShare);
						CList<MY_SEARCH_FILE , MY_SEARCH_FILE&>listSearchFile;	// ������ �˻��ؼ� ����	
						threadObject->FindAlltFile(szShareFolder, szMessage, &listSearchFile);	// �������������� �˻�
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
		
						pIOData->mySendData.pszFileSearchData = new char[strFileSearchData.GetLength() + 1];	// ����Ÿ�� �ٺ����� ���� �ƴϹǷ� ��������
						strcpy(pIOData->mySendData.pszFileSearchData, strFileSearchData);

						char szSend[BUFSIZE + 1];	// ���������� �������� ����Ÿ
						strncpy(szSend, strFileSearchData.Mid(0, BUFSIZE), BUFSIZE);
						szSend[BUFSIZE] = '\0';						
						if(threadObject->SendData(pClntData, pIOData, P_SEARCH_FILE_LIST, szSend, strFileSearchData.GetLength(), TRUE) == FALSE){	// ��� ���� �����Ǹ� ���������� Ŭ���̾�Ʈ���� ����(������ Ŭ���̾�Ʈ�� ������ �󳻿��� ����)
							continue;
						}

						bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
					}
					break;
				case P_SEARCH_FILE_LIST:	// ���ϰ˻��� ����� ����
					{
						if(pIOData->myRecvData.pszFileSearchData == NULL){
							pIOData->myRecvData.pszFileSearchData = new char[nTotalByte + 1];
							memset(pIOData->myRecvData.pszFileSearchData, 0, nTotalByte + 1);							
						}
						strncat(pIOData->myRecvData.pszFileSearchData, szMessage, BUFSIZE);
						//static DWORD nRecvByte = 0;	// ��������Ÿ���� �״´�
						//nRecvByte += BytesTransferred - sizeof(UINT) - sizeof(DWORD);	// ��������Ÿ ����ü���� nPacketHeader �� nTotalByte�� ������
						if(nTotalByte > strlen(pIOData->myRecvData.pszFileSearchData)){	// BytesTransferred �� BUFSIZE �� ũ����
							break;	// ����Ÿ�� ���ޱ����� break�� �Ѵ�
						}
						//nRecvByte = 0;						

						//CString temp;
						//temp.Format("%s \n���ϸ�� ����", pIOData->myRecvData.pszFileSearchData);
						//AfxMessageBox(temp);

						char *pszFileSearchData;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
						pszFileSearchData = new char[strlen(pIOData->myRecvData.pszFileSearchData) + 1];						
						strcpy(pszFileSearchData, pIOData->myRecvData.pszFileSearchData);	
						CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
						pMain->PostMessage(UM_ADDITEMSEARCHFILE, (WPARAM)pszFileSearchData, (LPARAM)pClntData->index);	// LPARAM �� ������ �ε�����ȣ

						delete [] pIOData->myRecvData.pszFileSearchData;
						pIOData->myRecvData.pszFileSearchData = NULL;
						
						break;
					}
				case P_DOWNLOAD_FILE_NOTICE:	// �����������ִ��� ������ Ȯ��
					{
						MY_FILE_DATA tempData;
						tempData.strFileFullName = szMessage;
						tempData.pFile = new CFile;						
						tempData.pFile->Open(szMessage, CFile::modeRead);	// CFile::typeBinary
						tempData.nFileSize = tempData.pFile->GetLength();
						if(tempData.pFile->m_hFile == CFile::hFileNull){	// ������ ã���� ������ ���â ó���� �ϸ� �ȵǰ� �ٽ� ã���� ���ٰ� �޽����� ��������
							if(threadObject->SendData(pClntData, pIOData, P_DOWNLOAD_FILE_ERROR, szMessage, strlen(szMessage), TRUE) == FALSE){	// ������ ã���� ���ٰ� �˸�
								continue;
							}			
							delete tempData.pFile;
							bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
							break;
						}
						
						tempData.nIndex = threadObject->m_nUploadMaxIndex;
						pIOData->nUploadFileIndex = threadObject->m_nUploadMaxIndex;

						tempData.nReceiveSize = 0;
												
						threadObject->m_uploadFileDataList.AddTail(tempData);	// ����Ʈ�� ����Ÿ ����

						// ������������ �޽����� ���� ���ε帮��Ʈ�� �߰�
						// �����̸�, ���ϰ��, ����Ʈ��, ����������, �����̸�
						CString strMessage;
						CString strTemp;
						strMessage.Format("%s", tempData.strFileFullName);
						INT cnt = strMessage.ReverseFind('\\');
						strMessage.Format("%s", strMessage.Right(strMessage.GetLength() - cnt - 1));	// �����̸�
						strMessage += "?";
						strMessage += tempData.strFileFullName.Left(cnt + 1);	// ���ϰ��
						strMessage += "?";
						strTemp.Format("%d", tempData.nFileSize);	// ����Ʈ��
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
						pszMessage = new char[strMessage.GetLength() + 1];	// ���⼭ �������λ����� �޴��ʿ��� delete
						strcpy(pszMessage, strMessage);						
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_ADD_UPLOAD_DATA, (WPARAM)pszMessage, (LPARAM)0);	// ���ε����ϸ���߰�
						

						if(threadObject->SendData(pClntData, pIOData, P_SEND_FILE_READY, szMessage, tempData.pFile->GetLength(), TRUE) == FALSE){	// ��������
							continue;
						}					
						//file.Close();	// ��������Ȯ���ϴºκп��� ������ ���ϰ� close��
						bIsRecvData = FALSE;	// ����Ÿ�� send �ϰ� ��ٷ� recv �� �ϸ� flag ���� ���ϹǷ� �ڽ��� ���� ����Ÿ�� �ڽ��� �޾ƺ���
						break;
					}
				case P_SEND_FILE_READY:	// �������ޱ����� ����������� �غ�
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
						
						// �ٿ�ε尡 ���۵ȴٰ� �˸�
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_ADD_DOWNLOAD_DATA, (WPARAM)tempData.nIndex, (LPARAM)0);
					}
					break;
				case P_DOWNLOAD_FILE_ERROR:	// �ٿ�ε������� ��û������ ã���� ���ٰ� �޽�������
					{
						CString strMessage;
						strMessage.Format("%s", szMessage);
						INT cnt = strMessage.ReverseFind('\\');
						strMessage.Format("%s ������ ã���� �����ϴ�!", strMessage.Right(strMessage.GetLength() - cnt - 1));
						AfxMessageBox(strMessage);
					}
					break;
				case P_SEND_FILE:	// �������۹���
					{
						POSITION pos;
						MY_FILE_DATA *tempData = NULL;
						pos = threadObject->m_downloadFileDataList.GetHeadPosition();
						while(pos){	// �۾��ϰ� �ִ�����ã��
							tempData = &(threadObject->m_downloadFileDataList.GetAt(pos));
							if(tempData->nIndex == pIOData->nDownloadFileIndex){
								break;
							}
							threadObject->m_downloadFileDataList.GetNext(pos);
						}
						
						// �̺κ��������� ��� �Ǵµ� ����Ÿ�޴ºκ��� �ణ �ν��ؼ� ����Ÿ�� �ٹ޾Ҵµ��� �� �޴� ó���κ��� ����Ǽ� �ٹ޾Ҵٸ� pass �ϱ�����
						if(tempData == NULL){	// �̹� m_downloadFileDataList �� �ִµ���Ÿ�� ���������, tempData ���� ����Ÿ�� ����
							break;
						}
						
						if(nTotalByte <= BUFSIZE){	// ����Ÿ�� �ѹ��� ������ɶ�
							tempData->nReceiveSize += nTotalByte % BUFSIZE;	// ��������Ÿ
							tempData->pFile->Write(pIOData->recvData.buffer, nTotalByte % BUFSIZE);	
						}else{	// ������ ������
							if(nTotalByte <= tempData->nReceiveSize + BUFSIZE){	// �������϶�
								tempData->pFile->Write(pIOData->recvData.buffer, nTotalByte - tempData->nReceiveSize);	// ���̷��������� ����Ÿ�� ���� �˼��� ���� �׷��� �̷��� ���
								tempData->nReceiveSize += nTotalByte - tempData->nReceiveSize;	// ��������Ÿ								
							}else{	// ��������Ÿ�� ���޾ƾ��Ҷ�
								tempData->pFile->Write(pIOData->recvData.buffer, BytesTransferred - sizeof(DWORD) - sizeof(UINT));
								tempData->nReceiveSize += BytesTransferred - sizeof(DWORD) - sizeof(UINT);	// ��������Ÿ								
							}							
						}

						//tempData->pFile->Write(pIOData->recvData.buffer, strlen(pIOData->recvData.buffer));
						//tempData->nReceiveSize += strlen(pIOData->recvData.buffer);
						

						if(nTotalByte > tempData->nReceiveSize){	// BytesTransferred �� BUFSIZE �� ũ����
							break;	// ����Ÿ�� ���ޱ����� break�� �Ѵ�
						}

						tempData->nReceiveSize = nTotalByte;	// �ٿ�ε��Ͽ��� ���̱� ����

						// ������������ popup
						char *strOut;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
						strOut = new char[256];						
						CString strFileName;
						strFileName.Format("%s", tempData->strFileFullName);
						INT cnt = strFileName.ReverseFind('\\');
						sprintf(strOut, "%s\n���ϴٿ�ε�Ϸ�!", strFileName.Right(strFileName.GetLength() - cnt - 1));
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut, (LPARAM)POPUP_MESSEGE_DOWNLOADCOMPLETE);

						// �ٿ�ε�信������ ����Ʈ�� �ٿ�ƴٴ°����˸�
						((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_DOWNLOAD_COMPLETE, (WPARAM)tempData->nIndex, (LPARAM)0);
						//AfxMessageBox("�������ۿϷ�!");

											
						tempData->pFile->Flush();
						tempData->pFile->Close();
						
						// ������ �ٹ����� ������������ incoming ������ �̵�						
						CString strNewFile;
						strNewFile.Format("%s%s", threadObject->m_strFolderInComing, strFileName.Right(strFileName.GetLength() - cnt - 1));
						MoveFile(tempData->strFileFullName, strNewFile);

						delete tempData->pFile;
						threadObject->m_downloadFileDataList.RemoveAt(pos);	// ����Ʈ���� ����							
					}
					break;
			} // switch()				
		} // if(pIOData->flag == SENDDATA)	
		if(bIsRecvData == TRUE){
			if(threadObject->RecvData(pClntData, pIOData) == FALSE){	// ����Ÿ WSARecv()
				continue;
			}
			
		}
	} // while(1)
	return 0;
}





// �ڽ��� ������ ����������
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
	

	// ������������ statusBar
	if(m_bIsClient != 0){	// ������ ���ӽÿ���
		char *strOut;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
		char *strOut2;
		strOut = new char[256];
		strOut2 = new char[256];
		sprintf(strOut, "%s", "������");	
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);	
		sprintf(strOut2, "%s", "���� �����¿� \n���ϴ�!");			
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_CONNECT);
	}


	return TRUE;
}

// �ڽ��� Ŭ���̾�Ʈ�� ����������
BOOL CMyIOCPSocket::ClientConnect(CString strIp)
{
	SOCKET hClntSock;	
	SOCKADDR_IN clntAddr;
	MY_IO_DATA *pIOData;
	MY_CLIENT_DATA *pClntData;
	DWORD nRecvBytes = 0;	// ���� ����Ÿ��
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
		
	// ���� �� Ŭ���̾�Ʈ�� ���� �ڵ� ������ �ּ� ������ ����.
	pClntData = (MY_CLIENT_DATA *)malloc(sizeof(MY_CLIENT_DATA));		
	pClntData->hClntSock = hClntSock;
	memcpy(&(pClntData->clntAddr), &clntAddr, sizeof(clntAddr));

	EnterCriticalSection(&m_cs);	// ũ��Ƽ�ü��� enter

	// Ŭ���̾�Ʈ���� ������� ����		
	// hClntSock ������ ���������̿����Ҹ������  �ʹ��� �����ص� ��ӻ���Ҽ� ����
	m_clntSock[m_nClntCount].pClntData = pClntData;	// Ŭ���̾�Ʈ�� ������ �Ұ��� ������ ������ �ϴ� ������ ����Ÿ ������ �ȴ�
	m_clntSock[m_nClntCount].pClntData->index = m_nClntCount;
	m_nClntCount ++;	// Ŭ���̾�Ʈ�� �ϳ�����

	LeaveCriticalSection(&m_cs);	// ũ��Ƽ�ü��� leave

	// Overlapped ���ϰ� CompletionPort�� ����.
	CreateIoCompletionPort((HANDLE)hClntSock, this->m_hCompletionPort, (DWORD)pClntData,	// �����忡����
		0);	

	// ���� �� Ŭ���̾�Ʈ�� ���� ���۸� ���� �ϰ� OVERLAPPED ����ü ���� �ʱ�ȭ.
	pIOData=(MY_IO_DATA *)malloc(sizeof(MY_IO_DATA));
	memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));	
	memset(&(pIOData->mySendData), 0, sizeof(MY_SEND_DATA));
	memset(&(pIOData->myRecvData), 0, sizeof(MY_RECV_DATA));

	m_pClntIOData[m_nClntCount - 1] = pIOData;	
	
	EnterCriticalSection(&m_cs);	// ũ��Ƽ�ü��� enter

	// �ڽ��� �̸��� ���������ִ������θ� ���� ����
	CString strTotalData;
	char szUserID[256];
	strTotalData += m_strUserID;
	strTotalData += "?";
	strTotalData += m_bViewFolder == 1 ? "1" : "0";
	strTotalData += "?";				
	sprintf(szUserID, "%s", strTotalData);
	DWORD nLength = strlen(szUserID);
		
	if(m_nClntCount == 1){	// ó�������ϹǷ� ������ ���� �ٸ��ĵ��� �����Ǹ� ����
		// Ŭ���̾�Ʈ�� ó�� ������ �����Ҷ� �ڽ��� ó���̶�� �˸��� �ٸ� �ĵ��� �����Ǹ� �޴´�
		if(SendData(pClntData, pIOData, P_FIRST_CONNECT, szUserID, nLength, TRUE) == FALSE){
			ErrorHandling(2, "SendData()");		
		}
	}else{	// ó���� �ƴ϶� �̹̼����κ��� �ٸ��ĵ��� �����Ǹ� �޾����Ƿ� ���Ӹ� ��û
		if(SendData(pClntData, pIOData, P_NOFIRST_CONNECT, szUserID, nLength, TRUE) == FALSE){
			ErrorHandling(2, "SendData()");		
		}
	}

	LeaveCriticalSection(&m_cs);	// ũ��Ƽ�ü��� leave

	// ������������ statusBar
	char szClntAddr[20];
	char *strOut;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
	char *strOut2;
	strOut = new char[256];
	strOut2 = new char[256];
	sprintf(strOut, "����� (����:%d)", m_nClntCount);			
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
	strcpy(szClntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
	sprintf(strOut2, "%s ������\n���ӵǾ����ϴ�!", szClntAddr);			
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_CONNECT);


#ifdef MYSOCKETDEBUG		
	char temp2[20];
	strcpy(temp2, inet_ntoa(clntAddr.sin_addr));
	char out[256];
	sprintf(out, "%s ������ ���ӵ�! \nŬ���̾�Ʈ ��(�ڽ�������) : %d\n", temp2, m_nClntCount);
	//AfxMessageBox(out);
#endif

	m_bInitClntFlag = FALSE;	// InitSocket() �Լ��� ���Ͻ�Ű�� ����

	return TRUE;
}

// �ڽ��� �������� ���� ����
void CMyIOCPSocket::CloseSevrSock()
{
	if(m_hServSock != INVALID_SOCKET){
		closesocket(m_hServSock);
		m_hServSock = INVALID_SOCKET;
	}
	
	WSACleanup();

	DWORD dwExitCode;
	// CompletionThread ����
	if(m_pCompletionThread != NULL){		
		GetExitCodeThread(m_pCompletionThread->m_hThread, &dwExitCode);
		TerminateThread(m_pCompletionThread->m_hThread, dwExitCode);
		CloseHandle(m_pCompletionThread->m_hThread);
		m_pCompletionThread = NULL;
	}

	// AcceptThread ����		
	if(m_pAcceptThread != NULL){
		GetExitCodeThread(m_pAcceptThread->m_hThread, &dwExitCode);
		TerminateThread(m_pAcceptThread->m_hThread, dwExitCode);
		CloseHandle(m_pAcceptThread->m_hThread);	
		m_pAcceptThread = NULL;
	}


	m_nClntCount = 0;

	// ������������ statusBar
	char *strOut;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
	char *strOut2;
	strOut = new char[256];
	strOut2 = new char[256];
	sprintf(strOut, "%s", "����ȵ�");	
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
	sprintf(strOut2, "%s", "���������Ǿ����ϴ�");
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_DISCONNECT);

#ifdef MYSOCKETDEBUG
	//AfxMessageBox("���������Ǿ����ϴ�");
#endif
	
}
 
	 
// ���������� ������.
void CMyIOCPSocket::CloseClntSock(MY_CLIENT_DATA *pClntData, MY_IO_DATA *pIOData)
{
	int index = pClntData->index;

	EnterCriticalSection(&m_cs);	// ũ��Ƽ�ü��� enter

	for(int i=index; i<m_nClntCount-1; i++){
		m_clntSock[i] = m_clntSock[i + 1];	
		m_pClntIOData[i] = m_pClntIOData[i + 1];
	}
	memset(&m_clntSock[m_nClntCount - 1] , 0, sizeof(MY_CLIENT_DETAIL_DATA));
	//memset(&m_pClntIOData[m_nClntCount - 1] , 0, sizeof(MY_IO_DATA));
	m_nClntCount --;	// Ŭ���̾�Ʈ�� �ϳ�����

	LeaveCriticalSection(&m_cs);	// ũ��Ƽ�ü��� leave

	
	// ������������ statusBar
	char clntAddr[20];
	char *strOut;	// ���ξ������ �޽����� �������ϴµ� �׳ɺ����� �޼����� ������°Ͱ��Ƽ� �������� �����ϰ� �����������ʿ��� delete
	char *strOut2;
	strOut = new char[256];
	strOut2 = new char[256];
	strcpy(clntAddr, inet_ntoa(pClntData->clntAddr.sin_addr));
	if(m_nClntCount == 0){
		sprintf(strOut, "%s", "������");	
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
		sprintf(strOut2, "%s ������\n���������Ǿ����ϴ�\n(���ӵ� ��������)", clntAddr);			
	}else{
		sprintf(strOut, "����� (����:%d)", m_nClntCount);			
		((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_SETPANETEXT, (WPARAM)strOut, (LPARAM)0);
		sprintf(strOut2, "%s ������\n���������Ǿ����ϴ�", clntAddr);			
	}	
	((CMainFrame *)AfxGetMainWnd())->PostMessage(UM_POPUPMESSAGE, (WPARAM)strOut2, (LPARAM)POPUP_MESSEGE_SERVER_DISCONNECT); 
	

#ifdef MYSOCKETDEBUG
	char temp2[20];
	CString temp;	
	strcpy(temp2, inet_ntoa(pClntData->clntAddr.sin_addr));
	temp.Format("%s Ŭ���̾�Ʈ ��������!\n Ŭ���̾�Ʈ ��(�ڽ�������) : %d", temp2, m_nClntCount);	// inet_ntoa() IP�κ�ȯ			
	AfxMessageBox(temp);
	if(m_nClntCount == 0){
		AfxMessageBox("���̻� ���ӵ��ִ� ������ �����ϴ�!");
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
	pIOData->wsaBuf.buf = (char *)&(pIOData->sendData);			// buf �� pIOData->data.buffer �� �ְ� �Ǹ� buffer�� �r�� ������ ������ nPacketHeader�� ���� ���� ���Ѵ� �׷��� data ����ü�� �ּҸ� �����ؼ� ������ ��ü�� �޾Ƽ� ����ؼ� ����Ÿ�� ������� ��´�
	pIOData->flag = SENDDATA;	// �۽�
	pIOData->sendData.nPacketHeader = nPacketHeader;	// ��Ŷ���
	pIOData->sendData.nTotalByte = nTotalByte;	// ��Ŷ �ѱ���
	
	strncpy(pIOData->sendData.buffer, message, BUFSIZE);
	pIOData->sendData.buffer[BUFSIZE] = '\0';
	
	if(bFirst == TRUE){	// ó�� ��������Ÿ���
		pIOData->nSendByte = strlen(message);
	}else{
		pIOData->nSendByte += strlen(message);	//ó�������� ����Ÿ�� �ƴϸ� �󸶳� ������ �ִ��� ����Ÿ�� ���Ѵ�
	}

	result = WSASend(pClntData->hClntSock, &pIOData->wsaBuf, 1, &nSendBytes, 0, &pIOData->overlapped, NULL);
	pIOData->sendData.nTotalByte = nTotalByte;	// ��Ŷ �ѱ���	
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
	DWORD nRecvBytes = 0;	// ���� ����Ÿ��
	DWORD flags = 0;

	// RECEIVE AGAIN, ����ؼ� ����Ÿ�� �Է¹���					
	memset(&(pIOData->overlapped), 0, sizeof(OVERLAPPED));
//	memset(&(pIOData->recvData), 0, sizeof(MY_PACKET));	// �ʱ�ȭ�� ���ָ� �ȵ�, �ٸ����� ����Ÿ�� �����Ⱚ��
	memset(&(pIOData->recvData.buffer), 0, BUFSIZE + 1);	// ���۸� �ʱ�ȭ
	pIOData->wsaBuf.len = BUFSIZE + sizeof(UINT) + sizeof(DWORD);//sizeof(pIOData->sendData);
	pIOData->wsaBuf.buf = (char *)&(pIOData->recvData);		// buf �� pIOData->data.buffer �� �ְ� �Ǹ� buffer�� �r�� ������ ������ nPacketHeader�� ���� ���� ���Ѵ� �׷��� data ����ü�� �ּҸ� �����ؼ� ������ ��ü�� �޾Ƽ� ����ؼ� ����Ÿ�� ������� ��´�
	pIOData->flag = RECVDATA;	


	static DWORD nOldTotalByte;
	//static UINT nOldPacketHeader = 0;
	if(pIOData->recvData.nPacketHeader > 21){	// ��������		
		nOldTotalByte = nOldTotalByte;
		//nOldPacketHeader = 21;
	}else{
		nOldTotalByte = pIOData->recvData.nTotalByte;
	}
	
	
	int result = WSARecv(pClntData->hClntSock, 	// ������ �Է� ����.
			&(pIOData->wsaBuf),		// ������ �Է� ���� ������.
			1,						// ������ �Է� ������ ��.
			&nRecvBytes,					// ���۵� ����Ʈ ���� �����ϱ� ���� ������
			&flags, 
			&(pIOData->overlapped),	// OVERLAPPED ����ü ������.
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

	if(pIOData->recvData.nPacketHeader > 21){	// ��������
		pIOData->recvData.nPacketHeader = 21;
		pIOData->recvData.nTotalByte = nOldTotalByte;
	}






	return TRUE;
}

void CMyIOCPSocket::ErrorHandling(UINT errorCode, CString strPrn)
{
	m_nErrorCode = errorCode;	// ���ӽ��н� �����߻�(����:0, �������ӿ���:1, Ŭ�󸮾�Ʈ����:2, Completion:3)

	
	if(m_bIsClient == 0){	// Ŭ���̾�Ʈ���
		m_bInitClntFlag = FALSE;	// InitSocket() �Լ��� ���Ͻ�Ű�� ����
	}
	
	int error = WSAGetLastError();
	char out[256];
	sprintf(out, "%s error! [ErrorCode %d]", strPrn, error);
	AfxMessageBox(out); 			
	
	// �ٽ� InitSocket() �Լ��� ȣ���ϹǷ� �ʱ�ȭ
	m_bInitClntFlag = TRUE;	// InitSocket() �Լ��� ���Ͻ�Ű�� ����

	// ���� ����
	for(int i=0; i<LIMITSOCKET; i++){	// Ŭ���̾�Ʈ ������ �������� �ְ� �������� �ִ�
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
	// CompletionThread ����
	if(m_pCompletionThread != NULL){		
		GetExitCodeThread(m_pCompletionThread->m_hThread, &dwExitCode);
		TerminateThread(m_pCompletionThread->m_hThread, dwExitCode);
		CloseHandle(m_pCompletionThread->m_hThread);
		m_pCompletionThread = NULL;
	}

	// AcceptThread ����		
	if(m_pAcceptThread != NULL){
		GetExitCodeThread(m_pAcceptThread->m_hThread, &dwExitCode);
		TerminateThread(m_pAcceptThread->m_hThread, dwExitCode);
		CloseHandle(m_pAcceptThread->m_hThread);	
		m_pAcceptThread = NULL;
	}

	
}

// ���ϰ˻�
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

	char szName[256];	// ���ϸ�
	char szExt[3];	// ȭ����
	CString strName;
	strName.Format("%s", pszName);
	int result = strName.Find('.', 0);
	if(result == -1){	// ȭ���ڰ� �������
		strcpy(szName, pszName);
		strcpy(szExt, "*");		
	}else{
		strcpy(szExt, strName.Mid(result + 1, strName.GetLength() - result - 1));
		strcpy(szName, strName.Left(result));		
	}
	if(strcmp(szExt, "*") == 0  &&  strcmp(szName, "*") == 0){	// *.* �� �˻��ȵ�
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
        if(ff.IsDirectory()){	// �������
            if(!ff.IsDots()){
				FindAlltFile(strFilePath.GetBuffer(0), pszName, pFileList);	// ���� ������ ���뵵 �˻��ϱ� ���� �ڱ� �ڽ��� ȣ���Ѵ�.				
            }
        }else{	// �����̶��
			CString strName = ff.GetFileName();
            result = strName.Find('.');
            if(result >= 0){	// Ȯ�尡�� �ִ°͸� �˻�
				if(strcmp(szExt, "*") == 0){	// Ȯ���ڰ� * ���
					nR = strnicmp(strName.Left(result), szName, strlen(szName));
					if(stricmp(strName.Left(result), szName) == 0 || nR == 0){	// ���ϸ��� ���ų� �տ������� �˻����ڸ�ŭ ���ٸ�						
						strTempPath = strFilePath;						
						strTempPath = strTempPath.Left(strTempPath.Find(strFilename, 0));						

						tempFile.nByte = ff.GetLength();
						tempFile.strFileName = strFilename;
						tempFile.strFilePath = strTempPath;						

						pFileList->AddTail( tempFile );		
						//strByte.Format("%d", ff.GetLength());
						//TRACE(strFilePath + "\t\t" + strByte + "Byte" + "\n");
					}
				}else if(strcmp(szName, "*") == 0){	// ���ϸ��� * �̶��
					if(stricmp(strName.Mid(result + 1, strName.GetLength() - result - 1), szExt) == 0){	// Ȯ���ڰ� ������
						//strByte.Format("%d", ff.GetLength());						
						//TRACE(strFilePath + "\t\t" + strByte + "Byte" + "\n");						
						strTempPath = strFilePath;						
						strTempPath = strTempPath.Left(strTempPath.Find(strFilename, 0));						

						tempFile.nByte = ff.GetLength();
						tempFile.strFileName = strFilename;
						tempFile.strFilePath = strTempPath;						

						pFileList->AddTail( tempFile );						
					}
				}else if(strName.Mid(result + 1).CompareNoCase(szExt) == 0){	// ���ϸ�� Ȯ���ڰ� ���ٸ�
					nR = strnicmp(strName.Left(result), szName, strlen(szName));
                    if(stricmp(strName.Left(result), szName) == 0  ||  nR == 0){	// ���ϸ��� ���ų� �տ������� �˻����ڸ�ŭ ���ٸ�
						if(stricmp(strName.Mid(result + 1, strName.GetLength() - result - 1), szExt) == 0){	// Ȯ���ڰ� ������
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

