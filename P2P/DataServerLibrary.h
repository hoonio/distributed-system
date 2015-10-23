// DataServerLibrary.h: interface for the CDataServerLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASERVERLIBRARY_H__C2F61C80_A4BF_4130_A768_CBCEBCE31011__INCLUDED_)
#define AFX_DATASERVERLIBRARY_H__C2F61C80_A4BF_4130_A768_CBCEBCE31011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataServerLibrary : public CObject  
{
	DECLARE_SERIAL(CDataServerLibrary)	// Serialize 사용하기 위해 추가
public:
	BOOL m_bIsData;	// 데이타의 갯수인지 아님 데이타 인지
	UINT m_nCount;	// 데이타의 갯수
	CString m_strServerName;	// 서버이름
	CString m_strServerIP;	// 서버아이피
	CString m_strUserID;	// 유저아이디
	CString m_strViewFolder;	// 남이 자신의 폴더를 볼수 있는지

	virtual void Serialize(CArchive& ar);	// 사용자가 추가
	CDataServerLibrary();
	virtual ~CDataServerLibrary();

};

#endif // !defined(AFX_DATASERVERLIBRARY_H__C2F61C80_A4BF_4130_A768_CBCEBCE31011__INCLUDED_)
