//
// ExToolBar.cpp : 実装ファイル
//

#include  "ExToolBar.h"


using namespace jbxwl;


/**
    ちらつき防止用

*/


// CExToolBar

IMPLEMENT_DYNAMIC(CExToolBar, CToolBar)


//CExToolBar::CExToolBar(CExFrame* pfrm)
CExToolBar::CExToolBar()
{
    //pFrame    = pfrm;
    rsrcID  = 0;
}



CExToolBar::~CExToolBar()
{
}



BOOL  CExToolBar::LoadToolBar(UINT id)
{
    rsrcID = id;

    return CToolBar::LoadToolBar(id);
}



BEGIN_MESSAGE_MAP(CExToolBar, CToolBar)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CExToolBar メッセージ ハンドラ

BOOL CExToolBar::OnEraseBkgnd(CDC* pDC)
{
    // ちらつき防止
    ::InvalidateRect(this->m_hWnd, NULL, FALSE);
    ::UpdateWindow(this->m_hWnd);

    return CToolBar::OnEraseBkgnd(pDC);
}

