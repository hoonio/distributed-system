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


IMPLEMENT_SERIAL(CDataServerLibrary, CObject, 1)	// Serialize 사용하기 위해 추가

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
	// 데이타를 파일에 적을때 몇개인지 모르므로 가장앞에 데이타의 갯수를 적는다
	if(m_bIsData){	// 순수한데이타만저장
		if(ar.IsStoring()){
			ar << m_strServerName << m_strServerIP << m_strUserID << m_strViewFolder;	// 송신(쓰기)
		}else{
			ar >> m_strServerName >> m_strServerIP >> m_strUserID >> m_strViewFolder;	// 수신(읽기)
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