// P2pDoc.h : interface of the CP2pDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_P2PDOC_H__2CD7BCD8_FB32_478E_AFFB_BD5C11C0CAB9__INCLUDED_)
#define AFX_P2PDOC_H__2CD7BCD8_FB32_478E_AFFB_BD5C11C0CAB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CP2pDoc : public CDocument
{
protected: // create from serialization only
	CP2pDoc();
	DECLARE_DYNCREATE(CP2pDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP2pDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CP2pDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CP2pDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_P2PDOC_H__2CD7BCD8_FB32_478E_AFFB_BD5C11C0CAB9__INCLUDED_)
