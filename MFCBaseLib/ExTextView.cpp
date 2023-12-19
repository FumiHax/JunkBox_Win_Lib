//
// ExView.cpp : インプリメンテーション ファイル
//


#include  "ExTextView.h"


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CExTextView  画像データ用

IMPLEMENT_DYNCREATE(CExTextView, CScrollView)


CExTextView::CExTextView()
{
    pDoc        = NULL;     //(CExDocument*)GetDocument();
    pFrame      = NULL;
    pApp        = NULL;

    Title       = _T("");   // ウィンドウタイトル

    sizeXYRate  = 1.0;
    clientRate  = 1.0;

    xsize       = 0;
    ysize       = 0;

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

    doReSize    = false;
    activeWin   = false;

    clientRect.bottom = 0;
    clientRect.top    = 0;
    clientRect.left   = 0;
    clientRect.right  = 0;
}



CExTextView::~CExTextView()
{
    DEBUG_INFO("DESTRUCTOR: CExTextView: START\n");

    if (pApp!=NULL) {
        DEBUG_INFO("Call Application ViewDestructor()\n");
        pApp->ViewDestructor(this);     // 上位アプリケーションに通知
    }

    if (!isNull(pFrame)) pFrame->pView = NULL;
    pDoc   = NULL;
    pFrame = NULL;

    DEBUG_INFO("DESTRUCTOR: CExTextView: END\n");
}



BEGIN_MESSAGE_MAP(CExTextView, CScrollView)
    //{{AFX_MSG_MAP(CExTextView)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



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
POINT  CExTextView::SetWindowSize(int cxs, int cys, BOOL first)
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
POINT  CExTextView::GetWindowSize(POINT pt)
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
POINT  CExTextView::GetClientSize(POINT pt)
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
POINT  CExTextView::GetWindowReSize(POINT pt)
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
POINT  CExTextView::ExecWindowReSize(int cxsize, int cysize)
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
// CExTextView ツール

void CExTextView::SetTitle(LPCTSTR title) 
{
    this->Title   = title;
    pFrame->Title = title;
    pFrame->SetTitle(title);        // ルート＆カレントウィンドウ
    pFrame->SetWindowText(title);   // カレントウィンドウ
    pDoc->SetTitle(title);          // ?

    return;
}




/////////////////////////////////////////////////////////////////////////////
// CExTextView メッセージ ハンドラ

void CExTextView::OnInitialUpdate() 
{
    CScrollView::OnInitialUpdate();

    if ((cnstXSize&&cnstYSize) || (cnstXSize&&cnstXYRate) || (cnstYSize&&cnstXYRate )) {
        cnstSize = true;
        cnstXSize = cnstYSize = cnstXYRate = false;
    }
}


//
void CExTextView::OnDraw(CDC* pDC)
{
    ExecWindowReSize(clientRect.right, clientRect.bottom);
}


//
void CExTextView::OnActivateView(BOOL bActivate, CScrollView* pActivateView, CScrollView* pDeactiveView) 
{
    activeWin = false;
    if (bActivate && pActivateView==this)  {
        activeWin = true;
    }

    CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


//
void CExTextView::OnSize(UINT nType, int cx, int cy) 
{
    CScrollView::OnSize(nType, cx, cy);

    doReSize = true;
    if (pFrame!=NULL) GetClientRect(&clientRect);
}


// 
BOOL CExTextView::OnEraseBkgnd(CDC* pDC)
{
    return CScrollView::OnEraseBkgnd(pDC);
}


