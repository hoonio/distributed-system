// P2pDoc.cpp : implementation of the CP2pDoc class
//

#include "stdafx.h"
#include "P2p.h"

#include "P2pDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP2pDoc

IMPLEMENT_DYNCREATE(CP2pDoc, CDocument)

BEGIN_MESSAGE_MAP(CP2pDoc, CDocument)
	//{{AFX_MSG_MAP(CP2pDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP2pDoc construction/destruction

CP2pDoc::CP2pDoc()
{
	// TODO: add one-time construction code here

}

CP2pDoc::~CP2pDoc()
{
}

BOOL CP2pDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CP2pDoc serialization

void CP2pDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CP2pDoc diagnostics

#ifdef _DEBUG
void CP2pDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CP2pDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CP2pDoc commands
