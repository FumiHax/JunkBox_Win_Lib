//
// DxPolygonDoc.cpp : 実装ファイル
//

#include "..\stdafx.h"

#include "DxPolygonDoc.h"


// DxPolygonDoc

IMPLEMENT_DYNCREATE(DxPolygonDoc, CExDocument)

//
DxPolygonDoc::DxPolygonDoc()
{
}


//
BOOL DxPolygonDoc::OnNewDocument()
{
    if (!CExDocument::OnNewDocument()) return FALSE;
    return TRUE;
}


//
DxPolygonDoc::~DxPolygonDoc()
{
}


BEGIN_MESSAGE_MAP(DxPolygonDoc, CExDocument)
END_MESSAGE_MAP()



// DxPolygonDoc 診断

#ifdef _DEBUG
void DxPolygonDoc::AssertValid() const
{
    CExDocument::AssertValid();
}


#ifndef _WIN32_WCE
void DxPolygonDoc::Dump(CDumpContext& dc) const
{
    CExDocument::Dump(dc);
}
#endif
#endif //_DEBUG


#ifndef _WIN32_WCE
// DxPolygonDoc シリアル化

void DxPolygonDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring()) {
        // TODO: 格納するコードをここに追加してください。
    }
    else {
        // TODO: 読み込むコードをここに追加してください。
    }
}
#endif


// DxPolygonDoc コマンド
