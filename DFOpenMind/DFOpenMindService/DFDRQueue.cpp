#include "stdafx.h"
#include "DFDRQueue.h"


DFDRQueue::DFDRQueue(UINT nObjectSize, UINT nBase, UINT nMax, UINT nInc)
{
	DFASSERT(nBase != 0 && nBase <= nMax);
	DFASSERT(nObjectSize > 0);

	m_BaseObjectSize= nObjectSize;
	m_BaseNodeCount = nBase;		
	m_MaxNodeCount	= nMax;			
	m_IncNodeCount	= nInc;

	m_pDataHead		= NULL;				
	m_pDataTail		= NULL;				
	m_pTotalHead	= NULL;				
	m_pTotalTail	= NULL;

	m_TotalNodeCount= 0;			
	m_DataNodeCount = 0;

	InitializeCriticalSection(&m_CriticalSection);

	CreateNodes(nBase);
}

DFDRQueue::~DFDRQueue()
{
	DeleteAllNodes();
	DeleteCriticalSection(&m_CriticalSection);
}

////////////////////////////////////////////////////////////////////////////////
BOOL DFDRQueue::AddToQueue(BYTE* pObject, UINT nSize) 
{
	BOOL bRet = TRUE;

	DFASSERT(pObject && nSize && nSize <= m_BaseObjectSize);

	EnterCriticalSection(&m_CriticalSection);

	// 여유 노드가 없는 상황이라면 새로이 노드를 생성한다.
	if(IsFull())
	{
		if(m_TotalNodeCount + m_IncNodeCount <= m_MaxNodeCount)
		{
			CreateNodes(m_IncNodeCount);
		}
		else
		{
			bRet = FALSE;
		}
	}
	
	// 여유 있는 노드가 존재한다면 데이터노드의 테일 뒤에 붙인다.
	if(bRet)
	{
		if(m_pDataTail)
			m_pDataTail = m_pDataTail->pDownLink;
		else
			m_pDataHead = m_pDataTail = m_pTotalHead;

		memcpy(m_pDataTail->pObject, pObject, nSize);
		m_pDataTail->nSize = nSize;

		m_DataNodeCount++;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return bRet;
}

BOOL DFDRQueue::GetFromQueue(BYTE* pObject, UINT* pSize)
{
	BOOL bRet = TRUE;

	EnterCriticalSection(&m_CriticalSection);

	if(m_pDataHead)
	{
		DFASSERT(pObject && !IsBadWritePtr(pObject, m_pDataHead->nSize));
		memcpy(pObject, m_pDataHead->pObject, m_pDataHead->nSize);
		*pSize = m_pDataHead->nSize;

		m_pDataHead->nSize = 0;

		QueueNode* pTempNode = m_pDataHead;
		DFASSERT(m_DataNodeCount > 0);
		m_DataNodeCount--;

		if(m_DataNodeCount)
		{	
			m_pDataHead = m_pDataHead->pDownLink;
			m_pDataHead->pUpLink = NULL;
			m_pTotalHead = m_pDataHead;
			m_pTotalTail->pDownLink = pTempNode;
			pTempNode->pUpLink = m_pTotalTail;
			pTempNode->pDownLink = NULL;

			m_pTotalTail = pTempNode;
		}
		else
		{
			m_pDataHead = m_pDataTail = NULL;
		}

		// 여유 노드 수를 비교한다. inc의 2배가 넘는다면 inc만큼 줄인다
//		if((m_TotalNodeCount - m_DataNodeCount) > m_IncNodeCount * 2)
//			DeleteNodes(m_IncNodeCount);
	}
	else
	{
		*pSize = 0;
		bRet = FALSE;
	}

	LeaveCriticalSection(&m_CriticalSection);
	
	return bRet;
}

UINT DFDRQueue::GetNodeCount()
{
	EnterCriticalSection(&m_CriticalSection);
	UINT nRet = m_DataNodeCount;
	LeaveCriticalSection(&m_CriticalSection);

	return nRet;
}

BOOL DFDRQueue::IsEmpty()
{
	BOOL bRet = TRUE;

	EnterCriticalSection(&m_CriticalSection);
	if(m_DataNodeCount)
		bRet = FALSE;
	LeaveCriticalSection(&m_CriticalSection);

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////////////


BOOL DFDRQueue::IsFull()
{
	if(m_TotalNodeCount == m_DataNodeCount)
		return TRUE;
	else 
		return FALSE;
}

void DFDRQueue::CreateNodes(UINT nAmount)
{
	QueueNode*	pNewNode = NULL;
	UINT		nCAmount = nAmount;

	while(nCAmount--)
	{
		pNewNode = new QueueNode;
		DFASSERT(pNewNode);
		
		BYTE* pObject = new BYTE[m_BaseObjectSize];
		DFASSERT(pObject);

		pNewNode->pObject	= pObject;
		pNewNode->nSize		= 0;
		pNewNode->pDownLink = NULL;
		pNewNode->pUpLink	= NULL;

		if(m_pTotalTail)
		{
			m_pTotalTail->pDownLink = pNewNode;
			pNewNode->pUpLink = m_pTotalTail;
			m_pTotalTail = pNewNode;
		}
		else	// 최초 생성시에..
		{
			m_pTotalTail = m_pTotalHead = pNewNode;
		}

		m_TotalNodeCount++;
	}
}

void DFDRQueue::DeleteNodes(UINT nAmount)
{
	QueueNode*	pTgtNode = NULL; 

	while(nAmount--)
	{	
		DFASSERT(m_pTotalTail);

		pTgtNode = m_pTotalTail;
		if(m_pTotalTail->pUpLink)
		{
			m_pTotalTail = m_pTotalTail->pUpLink;
			m_pTotalTail->pDownLink = NULL;	
		}
		else
		{
			m_pTotalTail = m_pTotalHead = NULL;
		}
	
		DFASSERT(pTgtNode != NULL && !IsBadWritePtr(pTgtNode, sizeof(QueueNode)));
		DFASSERT(pTgtNode->pObject != NULL && !IsBadWritePtr(pTgtNode->pObject, m_BaseObjectSize));
		
		delete [] pTgtNode->pObject;
		delete pTgtNode;

		DFASSERT(m_TotalNodeCount > 0);
		m_TotalNodeCount--;
	}
}

void DFDRQueue::DeleteAllNodes()
{
	EnterCriticalSection(&m_CriticalSection);

	QueueNode* pTailNode = m_pTotalTail;
	while(pTailNode)
	{
		QueueNode* pTgtNode = pTailNode;
		pTailNode = pTailNode->pUpLink;

		DFASSERT(pTgtNode);
		DFASSERT(pTgtNode->pObject);

		delete [] pTgtNode->pObject;
		delete pTgtNode;
	}
	m_TotalNodeCount = 0;
	m_DataNodeCount = 0;

	LeaveCriticalSection(&m_CriticalSection);
}


