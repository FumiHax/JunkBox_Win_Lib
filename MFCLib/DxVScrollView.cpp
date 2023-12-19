//
// CDxVScrollView.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "DxVScrollView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CDxVScrollView

IMPLEMENT_DYNCREATE(CDxVScrollView, CExView)


CDxVScrollView::CDxVScrollView()
{   
    cnstXSize = true;
    activeWin = true;

    // メモリ上の領域  dispRect = renderRect + vSBpos
    dispRect.top      = 0;  // 表示するメモリの先頭（Y座標）
    dispRect.bottom   = 0;  // 表示するメモリの最後（Y座標）
    dispRect.right    = 0;  //
    dispRect.left     = 0;  //

    // サーフェイスの有効領域 または 表示領域の大きさ
    renderRect.top    = 0;  //
    renderRect.bottom = 0;  //
    renderRect.right  = 0;  //
    renderRect.left   = 0;  //

    // バックバッファの描画領域
    bufferRect.top    = 0;  //
    bufferRect.bottom = 0;  //  
    bufferRect.right  = 0;  //
    bufferRect.left   = 0;  //

    maxClientYSize    = 0;
    vSBntch           = 10;
}



CDxVScrollView::~CDxVScrollView()
{
}



BEGIN_MESSAGE_MAP(CDxVScrollView, CDxBaseView)
    //{{AFX_MSG_MAP(CDxVScrollView)
    ON_WM_SIZE()
    ON_WM_LBUTTONDBLCLK()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDxVScrollView 描画
/*
void  CDxVScrollView::OnDraw(CDC* pDC)
{
    ExecWindowReSize();
    ExecRender();
}
*/



/////////////////////////////////////////////////////////////////////////////
// CDxVScrollView 診断

#ifdef _DEBUG
void  CDxVScrollView::AssertValid() const
{
    CDxBaseView::AssertValid();
}


//
void  CDxVScrollView::Dump(CDumpContext& dc) const
{
    CDxBaseView::Dump(dc);
}
#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// CDxVScrollView メッセージ ハンドラ

void  CDxVScrollView::OnInitialUpdate() 
{
    cmnHead = pDoc->cmnHead;
    if (isNull(cmnHead.grptr)) return;

    maxClientYSize = Min(cmnHead.ysize, this->maxYSize);
    BOOL rslt = InitDevice(cmnHead.xsize, maxClientYSize);
    if (!rslt) {
        // 失敗した場合は，領域が大きすぎる可能性がある
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
        MessageBox(_T("CDxVScrollView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    dispRect.right    = cmnHead.xsize - 1;  // dispRect.left = 0;
    renderRect.right  = cmnHead.xsize - 1;
    dispRect.bottom   = Min(cmnHead.ysize-1, clientRect.bottom);
    renderRect.bottom = dispRect.bottom;

    vSBpos = 0;
    vSBmax = cmnHead.ysize - cmnHead.zsize; //(clientRect.bottom - clientRect.top + 1);
    if (vSBmax<0) vSBmax = 0;
    SetScrollRange(SB_VERT, 0, vSBmax);

    // グラフィックデータ用バックバッファ
    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CRT_BAKBUF);
        MessageBox(_T("CDxVScrollView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    origXSize  = cmnHead.xsize;
    origYSize  = cmnHead.zsize;
    sizeXYRate = (float)cmnHead.zsize/(float)cmnHead.xsize;

    //DEBUG_INFO("CDxVScrollView::OnInitialUpdate():\n");
    SetWindowSize(cmnHead.xsize, cmnHead.zsize);
    hasViewData = TRUE;

    Title = pDoc->preTitle + pDoc->Title + pDoc->pstTitle;
    this->SetTitle(Title);

    return;
}


//
void  CDxVScrollView::ExecRender()
{
    HRESULT  hr;

    // クライアント領域がメモリ領域より大きい場合の転送領域の計算
    if (clientRect.bottom==0) bufferRect.bottom = 0;
    else  bufferRect.bottom = (int)(renderRect.bottom*bufferSize.y/(float)clientRect.bottom);
    bufferRect.right = bufferSize.x;

    hr = lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CLR_WND);
        MessageBox(_T("CDxVScrollView::ExecRender():\n\n") + mesg);
        return;
    }

    hr = lpD3DDevice->StretchRect(lpSurface, &renderRect, lpBackBuffer, &bufferRect, D3DTEXF_NONE);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_BAKBUF);
        MessageBox(_T("CDxVScrollView::ExecRender():\n\n") + mesg);
        return;
    }

    hr = lpD3DDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        //DEBUG_INFO("CDxVScrollView::ExecRender(): Resetting Device ...\n"); 
        hr = ResetDx9Device(lpD3DDevice, &d3dParam, this);
        if (FAILED(hr)) {
            CString mesg;
            mesg.LoadString(IDS_STR_FAIL_RESTR_DX9DEV);
            MessageBox(_T("CDxVScrollView::ExecRender():\n\n") + mesg);
            pFrame->SendMessage(WM_CLOSE);
            //GetParentFrame()->DestroyWindow();
        }
        else  ExecRender();
    }
}


//
void  CDxVScrollView::OnSize(UINT nType, int cx, int cy) 
{
    CDxBaseView::OnSize(nType, cx, cy);
    
    if (pFrame!=NULL) {
        GetClientRect(&clientRect);

        vSBmax = cmnHead.ysize - Min(cmnHead.ysize-1, clientRect.bottom) - 1;
        if (vSBmax<0)      vSBmax = 0;
        if (vSBpos>vSBmax) vSBpos = vSBmax;
        SetScrollRange(SB_VERT, 0, vSBmax);

        if (lpSurface!=NULL) DXRELEASE(lpSurface);

        HRESULT hr;
        hr = lpD3DDevice->CreateOffscreenPlainSurface(cmnHead.xsize, clientRect.bottom+1, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &lpSurface, NULL);
        if (FAILED(hr)) return;

        hr = SetNewSurface(vSBpos); 
    }
}


//
void  CDxVScrollView::InitObject()
{
    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_REGET_BAKBUF);
        MessageBox(_T("CDxVScrollView::InitObject():\n\n") + mesg);
        return;
    }

    GetClientRect(&clientRect);
    HRESULT hr = lpD3DDevice->CreateOffscreenPlainSurface(cmnHead.xsize, clientRect.bottom+1, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &lpSurface, NULL);
    if (FAILED(hr)) { 
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_REGET_SURFACE);
        MessageBox(_T("CDxVScrollView::InitObject():\n\n") + mesg);
        return;
    }

    hasViewData = SetNewSurface(vSBpos);
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_RETRNS_SURFACE);
        MessageBox(_T("CDxVScrollView::InitObject():\n\n") + mesg);
        return;
    }
    return;
}



BOOL  CDxVScrollView::SetNewSurface(int ovsbpos)
{
    renderRect.bottom = Min(cmnHead.ysize-1, clientRect.bottom);
    dispRect.bottom   = renderRect.bottom + vSBpos;
    dispRect.top      = vSBpos;
    uByte* ptr = (uByte*)&(CmnHeadBytePoint(cmnHead, 0, vSBpos));
    HRESULT hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, ptr, D3DFMT_L8,  cmnHead.xsize, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    if (FAILED(hr)) return FALSE;

    ScrollWindow(0, ovsbpos-vSBpos);
    return TRUE;
}


//
void  CDxVScrollView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{   
    pFrame->onLButtonDBLClick();
}
