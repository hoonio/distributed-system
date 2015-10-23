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

	// ���� ��尡 ���� ��Ȳ�̶�� ������ ��带 �����Ѵ�.
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
	
	// ���� �ִ� ��尡 �����Ѵٸ� �����ͳ���� ���� �ڿ� ���δ�.
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

		// ���� ��� ���� ���Ѵ�. inc�� 2�谡 �Ѵ´ٸ� inc��ŭ ���δ�
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
		else	// ���� �����ÿ�..
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


