// DataServerLibrary.cpp: implementation of the CDataServerLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P2p.h"
#include "DataServerLibrary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(CDataServerLibrary, CObject, 1)	// Serialize ����ϱ� ���� �߰�

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataServerLibrary::CDataServerLibrary()
{
	m_bIsData = FALSE;
	m_nCount = 0;
}

CDataServerLibrary::~CDataServerLibrary()
{

}


void CDataServerLibrary::Serialize(CArchive &ar)
{
	// ����Ÿ�� ���Ͽ� ������ ����� �𸣹Ƿ� ����տ� ����Ÿ�� ������ ���´�
	if(m_bIsData){	// �����ѵ���Ÿ������
		if(ar.IsStoring()){
			ar << m_strServerName << m_strServerIP << m_strUserID << m_strViewFolder;	// �۽�(����)
		}else{
			ar >> m_strServerName >> m_strServerIP >> m_strUserID >> m_strViewFolder;	// ����(�б�)
		}
	}else{	// ��������
		// Serialize �������� �Ҷ��� ���ڸ� �����ϴ�
		CString temp;		
		if(ar.IsStoring()){
			temp.Format("%d", m_nCount);
			ar << temp;	// �۽�(����)
		}else{
			ar >> temp;	// ����(�б�)
			m_nCount = atoi(temp);
		}
	}
}