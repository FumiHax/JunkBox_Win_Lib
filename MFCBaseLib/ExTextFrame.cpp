//
// ExTextFrame.cpp : インプリメンテーション ファイル
//

#include  "ExTextFrame.h"


using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CExTextFrame

IMPLEMENT_DYNCREATE(CExTextFrame, CMDIChildWnd)

CExTextFrame::CExTextFrame()
{
    //DEBUG_INFO("CONSTRUCTOR: CExTextFrame\n");

    pDoc        = NULL;
    pView       = NULL;
    pApp        = NULL;
    pTempl      = NULL;

    toolBar     = NULL;

    Title       = _T("");
    preTitle    = _T("");

    cancelOperation  = false;
    doneErrorMessage = false;

    reSizeMoving     = false;
}


//
CExTextFrame::~CExTextFrame()
{
    DEBUG_INFO("DESTRUCTOR: CExTextFrame: START\n");

    if (pApp!=NULL) {
        DEBUG_INFO("Call Application FrameDestructor()\n");
        pApp->FrameDestructor(this);    // 上位アプリケーションに通知
    }

    if (!isNull(pView)) pView->pFrame = NULL;
    pView = NULL;
    pDoc  = NULL;

    DEBUG_INFO("DESTRUCTOR: CExTextFrame: END\n");
}


//
// メッセージマップ
//

BEGIN_MESSAGE_MAP(CExTextFrame, CMDIChildWnd)
    //{{AFX_MSG_MAP(CExTextFrame)
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
    ON_WM_MOVE()
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CExTextFrame メッセージ ハンドラ

BOOL  CExTextFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{
    dwStyle = dwStyle | WS_CLIPCHILDREN;
    dwStyle = dwStyle & (~WS_MAXIMIZEBOX);  // 最大化ボタン禁止
    return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}



#ifdef ON_WM_ENTERSIZEMOVE

void  CExTextFrame::OnEnterSizeMove()
{
    CMDIChildWnd::OnEnterSizeMove();
    reSizeMoving = true;
}


void  CExTextFrame::OnExitSizeMove()
{
    CMDIChildWnd::OnExitSizeMove();
    reSizeMoving = false;
}


#else


LRESULT CExTextFrame::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
    reSizeMoving = true;
    return 0;
}


LRESULT CExTextFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
    reSizeMoving = false;
    return 0;
}

#endif



BOOL  CExTextFrame::OnEraseBkgnd(CDC* pDC)
{
    // TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します．
    return CMDIChildWnd::OnEraseBkgnd(pDC);
}



void  CExTextFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
    POINT pt;
    pt.x = pRect->right - pRect->left;
    pt.y = pRect->bottom - pRect->top;

    pt = pView->GetClientSize(pt);
    pt = pView->GetWindowReSize(pt);
    pt = pView->GetWindowSize(pt);

    pRect->right  = pRect->left + pt.x;
    pRect->bottom = pRect->top  + pt.y;
    CMDIChildWnd::OnSizing(fwSide, pRect);

    pView->doReSize = true;
}



void  CExTextFrame::OnMove(int x, int y)
{
    CMDIChildWnd::OnMove(x, y);

    // TODO: ここにメッセージ ハンドラー コードを追加します．
    this->OnPaint();
}

