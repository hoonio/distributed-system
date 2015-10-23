// DataDownload.h: interface for the CDataDownload class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATADOWNLOAD_H__788D96E7_0AB7_4AE0_9DD5_F4667E8A1EB6__INCLUDED_)
#define AFX_DATADOWNLOAD_H__788D96E7_0AB7_4AE0_9DD5_F4667E8A1EB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDataDownload : public CObject  
{
	DECLARE_SERIAL(CDataDownload)	// Serialize 사용하기 위해 추가
public:
	BOOL m_bIsData;	// 데이타의 갯수인지 아님 데이타 인지
	UINT m_nCount;	// 데이타의 갯수
	CString m_strFileName;	// 파일이름
	CString m_strFilePath;	// 파일경로
	DWORD m_nByte;	// 총바이트수
	UINT m_nIndex;
	CString m_strServerName;	// 서버이름
	CString m_strServerIP;	// 서버아이피
	DWORD m_nReceiveSize;
	CString m_strStatus;	// 다운로드중, 다운로드완료, 서버접속해제	



	virtual void Serialize(CArchive& ar);	// 사용자가 추가
	CDataDownload();
	virtual ~CDataDownload();

};



#endif // !defined(AFX_DATADOWNLOAD_H__788D96E7_0AB7_4AE0_9DD5_F4667E8A1EB6__INCLUDED_)
