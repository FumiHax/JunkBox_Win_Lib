//
// ExFrame.cpp : インプリメンテーション ファイル
//

#include  "ExFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CExFrame

IMPLEMENT_DYNCREATE(CExFrame, CMDIChildWnd)


CExFrame::CExFrame()
{
    //DEBUG_INFO("CONSTRUCTOR: CExFrame\n");

    pDoc        = NULL;
    pView       = NULL;
    pApp        = NULL;
    pTempl      = NULL;
    anyData     = NULL;

    toolBar     = NULL;
    vScrollBar  = NULL;
    hScrollBar  = NULL;

    Title       = _T("");
    preTitle    = _T("");
    colorMode   = GRAPH_COLOR_MONO;

    cancelOperation  = false;
    doneErrorMessage = false;

    reSizeMoving     = false;
}


//
CExFrame::~CExFrame()
{
    DEBUG_INFO("DESTRUCTOR: CExFrame\n");

    if (pApp!=NULL) {
        DEBUG_INFO("Call Application FrameDestructor()\n");
        pApp->FrameDestructor(this);    // 上位アプリケーションに通知
    }

    if (!isNull(pView)) pView->pFrame = NULL;
    if (!isNull(pDoc))  pDoc->pFrame  = NULL;
    pView = NULL;
    pDoc  = NULL;
}


//
// メッセージマップ
//
BEGIN_MESSAGE_MAP(CExFrame, CMDIChildWnd)
    //{{AFX_MSG_MAP(CExFrame)
    //}}AFX_MSG_MAP

#ifdef ON_WM_ENTERSIZEMOVE
    ON_WM_ENTERSIZEMOVE()
    ON_WM_EXITSIZEMOVE()
#else
    ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
    ON_MESSAGE(WM_EXITSIZEMOVE,  OnExitSizeMove)
#endif
    ON_WM_ERASEBKGND()
    ON_WM_SIZING()
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CExFrame メッセージ ハンドラ

BOOL  CExFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
    dwStyle = dwStyle | WS_CLIPCHILDREN;
    dwStyle = dwStyle & (~WS_MAXIMIZEBOX);  // 最大化ボタン禁止
    return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}



#ifdef ON_WM_ENTERSIZEMOVE

void  CExFrame::OnEnterSizeMove()
{
    CMDIChildWnd::OnEnterSizeMove();
    reSizeMoving = true;
}


void  CExFrame::OnExitSizeMove()
{
    CMDIChildWnd::OnExitSizeMove();
    reSizeMoving = false;
}

#else

LRESULT  CExFrame::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
    reSizeMoving = true;
    return 0;
}


LRESULT  CExFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
    reSizeMoving = false;
    return 0;
}

#endif



//
BOOL  CExFrame::OnEraseBkgnd(CDC* pDC)
{
    // TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します．
    return CMDIChildWnd::OnEraseBkgnd(pDC);
}


//
void  CExFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
    POINT pt;
    pt.x = pRect->right - pRect->left + 1;
    pt.y = pRect->bottom - pRect->top + 1;

    pt = pView->GetClientSize(pt);
    pt = pView->GetWindowReSize(pt);
    pt = pView->GetWindowSize(pt);

    pRect->right  = pRect->left + pt.x - 1;
    pRect->bottom = pRect->top  + pt.y - 1;
    CMDIChildWnd::OnSizing(fwSide, pRect);

    pView->doReSize = true;
}

