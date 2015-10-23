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
	DECLARE_SERIAL(CDataDownload)	// Serialize ����ϱ� ���� �߰�
public:
	BOOL m_bIsData;	// ����Ÿ�� �������� �ƴ� ����Ÿ ����
	UINT m_nCount;	// ����Ÿ�� ����
	CString m_strFileName;	// �����̸�
	CString m_strFilePath;	// ���ϰ��
	DWORD m_nByte;	// �ѹ���Ʈ��
	UINT m_nIndex;
	CString m_strServerName;	// �����̸�
	CString m_strServerIP;	// ����������
	DWORD m_nReceiveSize;
	CString m_strStatus;	// �ٿ�ε���, �ٿ�ε�Ϸ�, ������������	



	virtual void Serialize(CArchive& ar);	// ����ڰ� �߰�
	CDataDownload();
	virtual ~CDataDownload();

};



#endif // !defined(AFX_DATADOWNLOAD_H__788D96E7_0AB7_4AE0_9DD5_F4667E8A1EB6__INCLUDED_)
