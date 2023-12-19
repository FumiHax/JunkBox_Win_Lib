// DxPolygonView.cpp : 実装ファイル
//

#include "..\stdafx.h"

#include "DxPolygonView.h"



// DxPolygonView

IMPLEMENT_DYNCREATE(DxPolygonView, CExView)


DxPolygonView::DxPolygonView()
{

}


//
DxPolygonView::~DxPolygonView()
{
}



BEGIN_MESSAGE_MAP(DxPolygonView, CDxBaseView)
END_MESSAGE_MAP()



// DxPolygonView 描画

void DxPolygonView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: 描画コードをここに追加してください。
}


// DxPolygonView 診断

#ifdef _DEBUG
void DxPolygonView::AssertValid() const
{
    CDxBaseView::AssertValid();
}


#ifndef _WIN32_WCE
void DxPolygonView::Dump(CDumpContext& dc) const
{
    CDxBaseView::Dump(dc);
}
#endif
#endif //_DEBUG


// DxPolygonView メッセージ ハンドラ
