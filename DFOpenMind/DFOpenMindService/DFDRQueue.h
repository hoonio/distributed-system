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
	
	UINT				m_BaseObjectSize;			// ������ ������Ʈ�� �ִ� ������.
	UINT				m_BaseNodeCount;			// ����(�ּ�) ��� ��
	UINT				m_MaxNodeCount;				// �ִ� ��� ��
	UINT				m_IncNodeCount;				// �ѹ��� ������ų ��� ��

	UINT				m_TotalNodeCount;			// ���� �� ��� ��
	UINT				m_DataNodeCount;			// ���� ������ ��� ��

	QueueNode*			m_pDataHead;				// ������ ����� ���
	QueueNode*			m_pDataTail;				// ������ ����� ����
	QueueNode*			m_pTotalHead;				// �� ����� ���
	QueueNode*			m_pTotalTail;				// �� ����� ����

	CRITICAL_SECTION	m_CriticalSection;
};



#endif
