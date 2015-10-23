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
	DECLARE_SERIAL(CDataServerLibrary)	// Serialize ����ϱ� ���� �߰�
public:
	BOOL m_bIsData;	// ����Ÿ�� �������� �ƴ� ����Ÿ ����
	UINT m_nCount;	// ����Ÿ�� ����
	CString m_strServerName;	// �����̸�
	CString m_strServerIP;	// ����������
	CString m_strUserID;	// �������̵�
	CString m_strViewFolder;	// ���� �ڽ��� ������ ���� �ִ���

	virtual void Serialize(CArchive& ar);	// ����ڰ� �߰�
	CDataServerLibrary();
	virtual ~CDataServerLibrary();

};

#endif // !defined(AFX_DATASERVERLIBRARY_H__C2F61C80_A4BF_4130_A768_CBCEBCE31011__INCLUDED_)
