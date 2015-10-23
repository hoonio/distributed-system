// DataDownload.cpp: implementation of the CDataDownload class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P2p.h"
#include "DataDownload.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CDataDownload, CObject, 1)	// Serialize ����ϱ� ���� �߰�

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataDownload::CDataDownload()
{
	m_nReceiveSize = 0;
	m_nIndex = 0;
	m_nCount = 0;
	
}

CDataDownload::~CDataDownload()
{

}

void CDataDownload::Serialize(CArchive &ar)
{
	// ����Ÿ�� ���Ͽ� ������ ����� �𸣹Ƿ� ����տ� ����Ÿ�� ������ ���´�
	if(m_bIsData){	// �����ѵ���Ÿ������
		if(ar.IsStoring()){
			CString strByte;
			strByte.Format("%d", m_nByte);
			CString strReceiveSize;
			strReceiveSize.Format("%d", m_nReceiveSize);			
			CString strIndex;
			strIndex.Format("%d", m_nIndex);
			ar << m_strFileName << m_strFilePath << strByte << m_strServerName << 
				strReceiveSize << m_strServerIP << m_strStatus << strIndex;	// �۽�(����)
		}else{
			CString strByte;
			CString strReceiveSize;						
			CString strIndex;
			ar >> m_strFileName >> m_strFilePath >> strByte >> m_strServerName >> 
				strReceiveSize >> m_strServerIP >> m_strStatus >> strIndex;	// ����(�б�)
			m_nByte = atoi(strByte);
			m_nReceiveSize = atoi(strReceiveSize);						
			m_nIndex = atoi(strIndex);
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
