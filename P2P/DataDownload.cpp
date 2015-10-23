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

IMPLEMENT_SERIAL(CDataDownload, CObject, 1)	// Serialize 사용하기 위해 추가

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
	// 데이타를 파일에 적을때 몇개인지 모르므로 가장앞에 데이타의 갯수를 적는다
	if(m_bIsData){	// 순수한데이타만저장
		if(ar.IsStoring()){
			CString strByte;
			strByte.Format("%d", m_nByte);
			CString strReceiveSize;
			strReceiveSize.Format("%d", m_nReceiveSize);			
			CString strIndex;
			strIndex.Format("%d", m_nIndex);
			ar << m_strFileName << m_strFilePath << strByte << m_strServerName << 
				strReceiveSize << m_strServerIP << m_strStatus << strIndex;	// 송신(쓰기)
		}else{
			CString strByte;
			CString strReceiveSize;						
			CString strIndex;
			ar >> m_strFileName >> m_strFilePath >> strByte >> m_strServerName >> 
				strReceiveSize >> m_strServerIP >> m_strStatus >> strIndex;	// 수신(읽기)
			m_nByte = atoi(strByte);
			m_nReceiveSize = atoi(strReceiveSize);						
			m_nIndex = atoi(strIndex);
		}
	}else{	// 갯수저장
		// Serialize 로저장을 할때는 문자만 가능하다
		CString temp;		
		if(ar.IsStoring()){
			temp.Format("%d", m_nCount);
			ar << temp;	// 송신(쓰기)
		}else{
			ar >> temp;	// 수신(읽기)
			m_nCount = atoi(temp);
		}
	}

}
