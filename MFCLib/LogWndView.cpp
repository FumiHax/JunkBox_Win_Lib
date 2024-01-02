//
// LogWndView.cpp : CLogWndView クラスの実装
//

#include  "MFCBase.h"
#include  "LogWndFrame.h"
#include  "LogWndView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace jbxl;
using namespace jbxwl;


IMPLEMENT_DYNCREATE(CLogWndView, CExTextView)


CLogWndView::CLogWndView()
{
    //DEBUG_INFO("コンストラクタ：IN  CLogWndView\n");    

    pFrame        = NULL;
    pDoc          = NULL;
    Title         = _T("");

    m_FontSizeX   = 0;
    m_FontSizeY   = 0;
    m_DocLastPos  = 0;

    m_msx = m_msy = 0;
    m_mex = m_mey = 0;
    m_mpress      = FALSE;
    m_dragsx      = 0;
    m_dragsy      = 0;
    m_dragex      = 0;
    m_dragey      = 0;

    m_copysy      = -1;
    m_copyey      = -1;
    m_locked      = FALSE;

    KanjiCode     = CODE_SJIS;

    //DEBUG_INFO("コンストラクタ：OUT CLogWndView\n");    
}


CLogWndView::~CLogWndView()
{   
    DEBUG_INFO("DESTRUCTOR: CLogWndView: START\n"); 

    while(m_locked) ::Sleep(10);

    DEBUG_INFO("DESTRUCTOR: CLogWndView: END\n");
}


BEGIN_MESSAGE_MAP(CLogWndView, CExTextView)
    ON_WM_CREATE() 
    ON_WM_DESTROY() 
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CLogWndView::PreCreateWindow(CREATESTRUCT& cs)
{
    CSize sizeTotal(100, 100);
    CSize sizeLine(m_FontSizeX, m_FontSizeY);
    CSize sizePage(m_FontSizeX, m_FontSizeY*LINES_PER_PAGE);
    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);
    
    BOOL ret = CScrollView::PreCreateWindow(cs);
    return ret;
}


//
BOOL CLogWndView::OnEraseBkgnd(CDC* pDC)
{
    return CExTextView::OnEraseBkgnd(pDC);
}


void CLogWndView::OnSize(UINT nType, int cx, int cy)
{
    CExTextView::OnSize(nType, cx, cy);
}


void CLogWndView::OnDraw(CDC* pDC)
{
    if (pDoc==NULL) pDoc = GetDocument();
    if (!pDoc || pDoc->ringBuffer==NULL) return;

    CLogRingBuffer* pRB = pDoc->ringBuffer;
    if (pRB->getMaxLineY()==0) return;

    CSingleLock lock(&(pDoc->criticalKey));
    lock.Lock();
    while (!lock.IsLocked()) {
        Sleep(100);
        lock.Lock();
    }

    resetScrollSize();

    CFont* pOldFontX = pDC->SelectObject(&m_ScreenFont);
    CRect  rc;
    pDC->GetClipBox(&rc);

    int nLineBegin = rc.top/m_FontSizeY;
    int nLineEnd   = min((rc.bottom+m_FontSizeY-1)/m_FontSizeY, pRB->getMaxLineY());

    for (int i=nLineBegin; i<nLineEnd; i++) {
        Buffer buf = pRB->getRingBuffer(i);
        if (buf.buf==NULL) break;

        if (i>=m_copysy && i<=m_copyey) {
            pDC->SetBkColor(RGB(200,200,200));
        }
        else {
            pDC->SetBkColor(RGB(255,255,255));
        }

        if      (buf.state==LOG_RB_WARNING) {
            pDC->SetTextColor(RGB(20,20,200));
        }
        else if (buf.state==LOG_RB_ERROR) {
            pDC->SetTextColor(RGB(200,20,20));
        }
        else if (buf.state==LOG_RB_DEBUG) {
            pDC->SetTextColor(RGB(20,150,20));
        }
        else if (buf.state==LOG_RB_INFO) {
            pDC->SetTextColor(RGB(20,150,150));
        }
        else {
            pDC->SetTextColor(RGB(0,0,0));
        }

        //if (isText_Buffer(buf)) kanji_convert_Buffer(&buf);
        CString outtext = mbs2ts((char*)buf.buf);
        pDC->TextOut(DISPLAY_MARGIN, i*m_FontSizeY+DISPLAY_MARGIN, (LPCTSTR)outtext);
        free_Buffer(&buf);
    }

    setScrollPosition(pDC, pRB);

    lock.Unlock();

    pDC->SelectObject(pOldFontX);
}


void  CLogWndView::setScrollPosition(CDC* pDC, CLogRingBuffer* pRB)
{
    int lastPos = pRB->getLastPosition();
    if (m_DocLastPos==lastPos) return;
    m_DocLastPos = lastPos;

    CRect  rc;
    GetClientRect(&rc);     

    POINT  pt;
    pt.x = 0;
    pt.y = (lastPos+1)*m_FontSizeY - rc.bottom + rc.top;
    if (pt.y<0) pt.y = 0;

    ScrollToPosition(pt);
}


void  CLogWndView::resetScrollSize()
{
    if (pDoc==NULL) pDoc = GetDocument();
    if (!pDoc || !pDoc->ringBuffer) return;
    CLogRingBuffer* pRB = pDoc->ringBuffer;
    if (pRB->getMaxLineY()==0 || pRB->getMaxLineX()==0) return;
    
    int scrollSizeX = m_FontSizeX*pRB->getMaxLineX() + DISPLAY_MARGIN;
    int scrollSizeY = m_FontSizeY*pRB->getMaxLineY() + DISPLAY_MARGIN;
    //if (scrollSizeY>SHRT_MAX) scrollSizeY = SHRT_MAX;

    if (scrollSizeX!=m_ScrollSizeX || scrollSizeY!=m_ScrollSizeY) {
        m_ScrollSizeX = scrollSizeX;
        m_ScrollSizeY = scrollSizeY;

        CSize sizeTotal(m_ScrollSizeX, m_ScrollSizeY);
        CSize sizeLine(m_FontSizeX, m_FontSizeY);
        CSize sizePage(m_FontSizeX, m_FontSizeY*LINES_PER_PAGE);

        SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);
    }

    return;
}


void  CLogWndView::OnInitialUpdate()
{
    CExTextView::OnInitialUpdate();

    if (pDoc==NULL) pDoc = GetDocument();
    if (!pDoc || pDoc->ringBuffer==NULL) return;
    CLogRingBuffer* pRB = pDoc->ringBuffer;

    CClientDC dc(this);
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);

    m_FontSizeX   = dc.GetTextExtent(CString('0', 1)).cx;       // フォントの横サイズ
    m_FontSizeY   = tm.tmHeight + tm.tmExternalLeading;         // フォント（含行間）の縦サイズ

    m_ScrollSizeX = m_FontSizeX + DISPLAY_MARGIN;
    m_ScrollSizeY = m_FontSizeY + DISPLAY_MARGIN;
    //if (m_ScrollSizeY>SHRT_MAX) m_ScrollSizeY = SHRT_MAX;

    CSize sizeTotal(m_ScrollSizeX+2, m_ScrollSizeY);
    CSize sizeLine(m_FontSizeX, m_FontSizeY);
    CSize sizePage(m_FontSizeX, m_FontSizeY*LINES_PER_PAGE);

    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);

    return;
}


void  CLogWndView::clearViewDoc()
{
    if (pDoc==NULL) pDoc = GetDocument();
    if (!pDoc || pDoc->ringBuffer==NULL) return;
    CLogRingBuffer* pRB = pDoc->ringBuffer;

    CSingleLock lock(&(pDoc->criticalKey));
    lock.Lock();
    while (!lock.IsLocked()) {
        Sleep(100);
        lock.Lock();
    }

    m_ScrollSizeX = m_FontSizeX + DISPLAY_MARGIN;
    m_ScrollSizeY = m_FontSizeY + DISPLAY_MARGIN;
    //if (m_ScrollSizeY>SHRT_MAX) m_ScrollSizeY = SHRT_MAX;

    CSize sizeTotal(m_ScrollSizeX+2, m_ScrollSizeY);
    CSize sizeLine(m_FontSizeX, m_FontSizeY);
    CSize sizePage(m_FontSizeX, m_FontSizeY*LINES_PER_PAGE);

    SetScrollSizes(MM_TEXT, sizeTotal, sizePage, sizeLine);

    POINT  pt;
    pt.x = 0;
    pt.y = 0;

    ScrollToPosition(pt);

    pDoc->clear();

    InvalidateRect(NULL, TRUE);

    lock.Unlock();
}


void CLogWndView::SetTitle(LPCTSTR title) 
{
    this->Title   = title;
    pFrame->Title = title;
    pFrame->SetTitle(title);        // ルート＆カレントウィンドウ
    pFrame->SetWindowText(title);   // カレントウィンドウ
    pDoc->SetTitle(title);          // ?

    return;
}


#ifdef _DEBUG

void CLogWndView::AssertValid() const
{
    CScrollView::AssertValid();
}


//
void CLogWndView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}


//
CLogWndDoc* CLogWndView::GetDocument() const // デバッグ以外のバージョンはインラインです．
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogWndDoc)));
    return (CLogWndDoc*)m_pDocument;
}


#endif //_DEBUG



///////////////////////////////////////////////////////////////////////////////
// CLogWndView メッセージ ハンドラ
// フォントの設定

int CLogWndView::OnCreate(LPCREATESTRUCT lpcs)
{
    if (CScrollView::OnCreate(lpcs)==-1) return -1;

    CString fname, fsize;
    fname.LoadString(IDS_STR_FONT_GOTHIC);      // ＭＳ ゴシック
    fsize.LoadString(IDS_STR_FONT_SIZE);        // 120
    int sz = ttoi(fsize);

    CClientDC dc(this);
    m_ScreenFont.CreatePointFont(sz, (LPCTSTR)fname, &dc);

    //
    OnInitialUpdate();
    return 0;
}


void CLogWndView::OnDestroy(void)
{
    m_ScreenFont.DeleteObject();
    CScrollView::OnDestroy();
}



////////////////////////////////////////////////////////////////////////////////////////////
// Copy (& Paste)
// 
void  CLogWndView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint scrlpos = GetScrollPosition();
    m_msx    = point.x + scrlpos.x;
    m_msy    = point.y + scrlpos.y;

    m_dragsx = m_msx/m_FontSizeX;
    m_dragsy = m_msy/m_FontSizeY;
    m_mpress = TRUE;

    // Window内をクリック
    CRect  rc;
    GetClientRect(&rc);     
    if (point.x>=rc.left && point.x<=rc.right && point.y>rc.top && point.y<=rc.bottom) {
        m_copysy = -1;
        m_copyey = -1;
        InvalidateRect(NULL, FALSE);
    }

    CScrollView::OnLButtonDown(nFlags, point);
}


void  CLogWndView::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_mpress = FALSE;

    CScrollView::OnLButtonUp(nFlags, point);
}


void  CLogWndView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_mpress) {
        CPoint scrlpos = GetScrollPosition();
        m_mex = point.x + scrlpos.x;
        m_mey = point.y + scrlpos.y;
        m_dragex = m_mex/m_FontSizeX;
        m_dragey = m_mey/m_FontSizeY;

        if (m_dragsx!=m_dragex || m_dragsy!=m_dragey) {
            m_copysy = m_dragsy;
            m_copyey = m_dragey;
            InvalidateRect(NULL, FALSE);
        }
    }

    CScrollView::OnMouseMove(nFlags, point);
}


CString  CLogWndView::getCopyData(void)
{
    CString ret = _T("");

    if (m_copysy < 0 || m_copysy > m_copyey) return ret;

    CLogWndDoc* pdoc = GetDocument();
    if (!pdoc || !pdoc->ringBuffer) return ret;
    CLogRingBuffer* pRB = pdoc->ringBuffer;

    for (int i=m_copysy; i<=m_copyey; i++) {
        Buffer buf = dup_Buffer(pRB->pBuf[i]);
        if (pRB->getKindData(i)!=LOG_RB_BINARY_DATA) {
            if (buf.buf[buf.vldsz-1]!='\n') cat_s2Buffer("\n", &buf);
        }

        ret = ret + mbs2ts((char*)buf.buf);
        free_Buffer(&buf);
    }

    return ret;
}


//
// CScrollView が 32767pixel を越える場合の対応
//
BOOL  CLogWndView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask  = SIF_TRACKPOS;

    if (LOBYTE(nScrollCode)==SB_THUMBTRACK)
    {
        GetScrollInfo(SB_HORZ, &info);
        nPos = info.nTrackPos;
    }

    if (HIBYTE(nScrollCode)==SB_THUMBTRACK)
    {
        GetScrollInfo(SB_VERT, &info);
        nPos = info.nTrackPos;
    }

    return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}
