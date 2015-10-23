#ifndef __DFDRQUEUE_H__
#define __DFDRQUEUE_H__

#pragma once

#include "winsock2.h"
#include "DFCmn.h"


#define BASE_OBJECT_SIZE	4096
#define BASE_NODE			16
#define MAX_NODE			1024
#define	INC_NODE			4


class DFDRQueue
{
public:

	typedef struct _QUEUENODE
	{
		struct _QUEUENODE*	pUpLink;
		struct _QUEUENODE*	pDownLink;
		BYTE*				pObject;
		UINT				nSize;
	} QueueNode;

	DFDRQueue(	UINT nObjectSize = BASE_OBJECT_SIZE, UINT nBase = BASE_NODE, 
				UINT nMax = MAX_NODE, UINT nInc = INC_NODE);
	~DFDRQueue();

	BOOL				AddToQueue(BYTE* pObject, UINT nSize);
	BOOL				GetFromQueue(BYTE* pObject, UINT* pSize);
	BOOL				IsEmpty();
	UINT				GetNodeCount();

private:
	void				CreateNodes(UINT nAmount);
	void				DeleteNodes(UINT nAmount);
	void				DeleteAllNodes();
	BOOL				IsFull();
	
	UINT				m_BaseObjectSize;			// 데이터 오브젝트의 최대 사이즈.
	UINT				m_BaseNodeCount;			// 최초(최소) 노드 수
	UINT				m_MaxNodeCount;				// 최대 노드 수
	UINT				m_IncNodeCount;				// 한번에 증가시킬 노드 수

	UINT				m_TotalNodeCount;			// 현재 총 노드 수
	UINT				m_DataNodeCount;			// 현재 데이터 노드 수

	QueueNode*			m_pDataHead;				// 데이터 노드의 헤드
	QueueNode*			m_pDataTail;				// 데이터 노드의 테일
	QueueNode*			m_pTotalHead;				// 총 노드의 헤드
	QueueNode*			m_pTotalTail;				// 총 노드의 테일

	CRITICAL_SECTION	m_CriticalSection;
};



#endif
