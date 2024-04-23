//
// ExView.cpp : インプリメンテーション ファイル
//

#include  "ExView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


UINT  jbxwl::TimerID = 1;


/////////////////////////////////////////////////////////////////////////////
// CExView  画像データ用

IMPLEMENT_DYNCREATE(CExView, CView)


CExView::CExView()
{
//  DEBUG_WARN("CONSTRUCT  CExView\n");
    cmnHead.init();
    msGraph.init();

    pDoc        = NULL;     //(CExDocument*)GetDocument();
    pFrame      = NULL;
    pApp        = NULL;
    anyData     = NULL;

    hasViewData = FALSE;    // 有効な表示用データをもっているか
    Title       = _T("");   // ウィンドウタイトル

    timerID     = -1;       // -1 でタイマー未使用．
    sizeXYRate  = 1.0;
    clientRate  = 1.0;
    sizeFac     = 1.0;

    xsize       = 0;
    ysize       = 0;
    zsize       = 1;

    origXSize   = 0;
    origYSize   = 0;
    initXSize   = 0;
    initYSize   = 0;
    prevXSize   = 0;
    prevYSize   = 0;

    cnstSize    = false;
    cnstXSize   = false;
    cnstYSize   = false;
    cnstXYRate  = false;
    maxXSize    = SINTMAX;
    maxYSize    = SINTMAX;

    ctrlMouse   = true;
    doReSize    = false;
    activeWin   = false;

    vSBpos      = 0;
    prevSBpos   = -1;
    vSBmax      = 0;
    vSBntch     = 3;
    vSBctrl     = true; 
    viewMode    = VIEW_ZDIR;
    colorMode   = GRAPH_COLOR_MONO;

    cMax        = 0;
    cMin        = 0;
    vMax        = 0;
    vMin        = 0;

    clientRect.bottom = 0;
    clientRect.top    = 0;
    clientRect.left   = 0;
    clientRect.right  = 0;
}



CExView::~CExView()
{
    DEBUG_INFO("DESTRUCTOR: CExView\n");

    if (pApp!=NULL) {
        DEBUG_INFO("Call Application ViewDestructor()\n");
        pApp->ViewDestructor(this);     // 上位アプリケーションに通知
    }

    // データは対応する Documentのディストラクタで削除
    cmnHead.mfree();
    msGraph.mfree();

    viewData.free();

    if (!isNull(pDoc))   pDoc->pView   = NULL;
    if (!isNull(pFrame)) pFrame->pView = NULL;
    pDoc   = NULL;
    pFrame = NULL;
}



BEGIN_MESSAGE_MAP(CExView, CView)
    //{{AFX_MSG_MAP(CExView)
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
//  ON_WM_TIMER()
    //}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CExView 診断

#ifdef _DEBUG
void CExView::AssertValid() const
{
    CView::AssertValid();
}


void CExView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// Window サイズ

//
// クライアントサイズを指定して，ウィンドを配置する
//
//  cxsize --- クライアント領域の Xサイズ
//  cysize --- クライアント領域の Yサイズ
//
//  sizeXYRate は下位クラスで定義されている必要がある．
//
POINT  CExView::SetWindowSize(int cxs, int cys, BOOL first)
{
    POINT  pc = {-1, -1};

    if (cxs<0 || cys<0) return pc;
    pc.x = cxs;
    pc.y = cys;

    POINT pt = GetWindowSize(pc);
    pFrame->SetWindowPos((CWnd*)&wndTop, 0, 0, pt.x, pt.y, SWP_NOMOVE);
    GetClientRect(&clientRect);

    // サイズが合わない場合
    if (clientRect.right!=pc.x || clientRect.bottom!=pc.y) {
        if (first) {
            double srate = (double)clientRect.bottom/(double)clientRect.right;
            if (sizeXYRate<srate) {
                pc.x = clientRect.right;
                pc.y = (int)(pc.x*sizeXYRate+0.5);
            }
            else {
                pc.y = clientRect.bottom;
                pc.x = (int)(pc.y/sizeXYRate+0.5);
            }
        }
        else {
            pc.x = prevXSize;
            pc.y = prevYSize;
        }
        pt = GetWindowSize(pc);
        pFrame->SetWindowPos((CWnd*)&wndTop, 0, 0, pt.x, pt.y, SWP_NOMOVE);
        GetClientRect(&clientRect);
    }
            
    pc.x = prevXSize  = clientRect.right;
    pc.y = prevYSize  = clientRect.bottom;
    clientRate = (double)clientRect.right/(double)origXSize;

    if (first) {
        initXSize = clientRect.right;
        initYSize = clientRect.bottom;
    }
    return pc;
}


//
//  クライアント領域のサイズからウインドウ領域のサイズを計算する．
//
POINT  CExView::GetWindowSize(POINT pt)
{
    RECT  frect, crect;
    POINT pc;

    pFrame->GetWindowRect(&frect);
    GetClientRect(&crect);
    
    pc.x = pt.x + (frect.right -frect.left) - (crect.right -crect.left);
    pc.y = pt.y + (frect.bottom-frect.top)  - (crect.bottom-crect.top); 
    return pc;
}


//
//  ウインドウ領域のサイズからクライアント領域のサイズを計算する．
//
POINT  CExView::GetClientSize(POINT pt)
{
    RECT  frect, crect;
    POINT pc;

    pFrame->GetWindowRect(&frect);
    GetClientRect(&crect);
    
    pc.x = pt.x - (frect.right -frect.left) + (crect.right -crect.left);
    pc.y = pt.y - (frect.bottom-frect.top)  + (crect.bottom-crect.top); 
    return pc;
}


//
POINT  CExView::GetWindowReSize(POINT pt)
{
    int minxs = GetSystemMetrics(SM_CXMINTRACK);
    int minys = GetSystemMetrics(SM_CYMINTRACK);

    if (pt.x<minxs) pt.x = minxs; 
    if (pt.y<minys) pt.y = minys;

    if (cnstSize) {
        pt.x = initXSize;
        pt.y = initYSize;
    }
    else if (cnstXSize || cnstYSize || cnstXYRate) {
        if (cnstXSize && !cnstYSize) {          // Xサイズ固定
            pt.x = initXSize;
            pt.y = Min(pt.y, maxYSize);
        }
        else if (!cnstXSize && cnstYSize) {     // Yサイズ固定
            pt.x = Min(pt.x, maxXSize);
            pt.y = initYSize;
        }
        else if (cnstXYRate) {                  // 縦横比固定
            TCHAR* cur = GetMouseCursorType();
            if (cur==IDC_SIZENS) {
                pt.y = Min(pt.y, maxYSize);
                pt.x = (int)(pt.y/sizeXYRate + 0.5);
            }
            else {
                pt.x = Min(pt.x, maxXSize);
                pt.y = (int)(pt.x*sizeXYRate + 0.5);
            }
        }
    }

    return pt;
}


//
POINT  CExView::ExecWindowReSize(int cxsize, int cysize)
{
    POINT pt;
    pt.x = cxsize;
    pt.y = cysize;

    if (doReSize) {
        pt = GetWindowReSize(pt);
        if (!cnstSize && (cnstXSize || cnstYSize || cnstXYRate)) {
            pt = SetWindowSize(pt.x, pt.y, FALSE);
        }
    }

    doReSize = false;
    return pt;
}




/////////////////////////////////////////////////////////////////////////////
// マウス

//
// クライアント上のマウスカーソルの位置を獲得する．
//     マウスカーソルがクライアントの範囲にない場合は，座標別に負の値を返す．
//
POINT  CExView::GetMousePos()
{
    POINT pt;
    RECT  crect;

    if (m_hWnd==NULL) {
        pt.x = pt.y = -1;
        return pt;
    }

    GetCursorPos(&pt);
    ScreenToClient(&pt);
    this->GetClientRect(&crect);

    if (crect.right-crect.left < pt.x) pt.x = -pt.x;
    if (crect.bottom-crect.top < pt.y) pt.y = -pt.y;

    return pt;
}


// 
BOOL  CExView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    if (vSBmax!=0 && vSBctrl) {
        int ovsbpos = vSBpos;

        vSBpos = vSBpos - zDelta/30*vSBntch;
        if (vSBpos<0)      vSBpos = 0;
        if (vSBpos>vSBmax) vSBpos = vSBmax;
        SetScrollPos(SB_VERT, vSBpos);

        HRESULT hr = SetNewSurface(ovsbpos);
        OnDraw(NULL);
    }

    return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
// CExView ツール

void CExView::SetTitle(LPCTSTR title) 
{
    this->Title   = title;
    pFrame->Title = title;
    pFrame->SetTitle(title);        // ルート＆カレントウィンドウ
    pFrame->SetWindowText(title);   // カレントウィンドウ
//  if (pDoc!=NULL) pDoc->SetTitle(title);          // ?

    return;
}




/////////////////////////////////////////////////////////////////////////////
// CExView メッセージ ハンドラ

void CExView::OnInitialUpdate() 
{
    CView::OnInitialUpdate();

    if (pDoc!=NULL) {
        cmnHead = pDoc->cmnHead;
        msGraph = pDoc->msGraph;
        xsize   = cmnHead.xsize;
        ysize   = cmnHead.ysize;
        zsize   = cmnHead.zsize;
    }

    if ((cnstXSize&&cnstYSize) || (cnstXSize&&cnstXYRate) || (cnstYSize&&cnstXYRate )) {
        cnstSize = true;
        cnstXSize = cnstYSize = cnstXYRate = false;
    }
}


//
void CExView::OnDraw(CDC* pDC)
{
    if (hasViewData && timerID==-1) {
        ExecWindowReSize(clientRect.right, clientRect.bottom);
        ExecRender();
    }
    else if (!hasViewData) DEBUG_ERROR("CExView::OnDraw(): ERROR: 表示可能なデータがありません\n");
}


//
void CExView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
    activeWin = false;
    ctrlMouse = false;
    if (bActivate && pActivateView==this)  {
        activeWin = true;
        ctrlMouse = true;
    }

    CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


//
void CExView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);

    doReSize = true;
    if (pFrame!=NULL) GetClientRect(&clientRect);
}


// リサイズ時のちらつき防止
BOOL CExView::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}


//
void CExView::OnTimer(UINT_PTR nIDEvent)
{
    if (hasViewData) {
        ExecWindowReSize(clientRect.right, clientRect.bottom);
        ExecRender();
    }
    else  DEBUG_ERROR("CExView::OnTimer(): ERROR: 表示可能なデータがありません\n");

    CView::OnTimer(nIDEvent);
}


//
void  CExView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (!activeWin || vSBmax==0) return;
    if (pScrollBar!=NULL || !vSBctrl) return;
    
    int ovsbpos = vSBpos;

    switch (nSBCode) {
        case SB_LINEDOWN:
            vSBpos += vSBntch;
            if (vSBpos>vSBmax) vSBpos = vSBmax;
            break;
        case SB_LINEUP:
            vSBpos -= vSBntch;
            if (vSBpos<0) vSBpos = 0;
            break;
        case SB_THUMBPOSITION:
            vSBpos = nPos;
            break;
        case SB_THUMBTRACK:
            vSBpos = nPos;
            break;
        case SB_PAGEDOWN:
            vSBpos += vSBntch*2;
            if (vSBpos>vSBmax) vSBpos = vSBmax;
            break;
        case SB_PAGEUP:
            vSBpos -= vSBntch*2;
            if (vSBpos<0) vSBpos = 0;
            break;
    }
    SetScrollPos(SB_VERT, vSBpos);

    HRESULT hr = SetNewSurface(ovsbpos);
    OnDraw(NULL);

    return;
}

