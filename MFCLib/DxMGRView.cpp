//
// DxMGRView.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "DxMGRView.h"
#include  "RwGRDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CDxMGRView

IMPLEMENT_DYNCREATE(CDxMGRView, CExView)


CDxMGRView::CDxMGRView()
{
    cnstXYRate = true;

    renderRect.left   = 0;
    renderRect.top    = 0;
    renderRect.right  = 0;
    renderRect.bottom = 0;
    
    cMax = VIEW_8_MAX;
    cMin = 0;

    vSBntch = 1;            // スクロールピッチ
}


//
CDxMGRView::~CDxMGRView()
{

}


BEGIN_MESSAGE_MAP(CDxMGRView, CDxBaseView)
    //{{AFX_MSG_MAP(CDxMGRView)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDxMGRView 描画
/*
void CDxMGRView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: この位置に描画用のコードを追加してください
}
*/



/////////////////////////////////////////////////////////////////////////////
// CDxMGRView 診断

#ifdef _DEBUG
void CDxMGRView::AssertValid() const
{
    CDxBaseView::AssertValid();
}

//
void CDxMGRView::Dump(CDumpContext& dc) const
{
    CDxBaseView::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDxMGRView メッセージ ハンドラ

// 現在は カラーについては Planeデータ専用
// Planeデータで無い場合のカラー処理は処理はまだ実装していない．

// 
// pDoc->cmnHead, pDoc->vMax, pDoc->vMin, pDoc->cMax, pDoc->cMin が必要．
//
void CDxMGRView::OnInitialUpdate() 
{
    cmnHead = pDoc->cmnHead;
    if (isNull(cmnHead.grptr)) return;

    colorMode = pDoc->colorMode & GRAPH_COLOR_MASK;
    if (pDoc->colorMode & GRAPH_COLOR_USE_BANK) useBank = true; 

    if (useBank) {
        if      (colorMode==GRAPH_COLOR_RGB  && cmnHead.zsize!=3) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_BGR  && cmnHead.zsize!=3) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_ARGB && cmnHead.zsize!=4) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_ABGR && cmnHead.zsize!=4) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_RGBA && cmnHead.zsize!=4) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_BGRA && cmnHead.zsize!=4) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_XRGB && cmnHead.zsize!=4) colorMode = GRAPH_COLOR_MONO;
        else if (colorMode==GRAPH_COLOR_XBGR && cmnHead.zsize!=4) colorMode = GRAPH_COLOR_MONO;
    }
    if (colorMode==GRAPH_COLOR_MONO) useBank = false;

    //
    xsize = cmnHead.xsize;
    ysize = cmnHead.ysize;
    
    // 表示モード
    if (colorMode==GRAPH_COLOR_RGB  || colorMode==GRAPH_COLOR_BGR  || 
        colorMode==GRAPH_COLOR_XRGB || colorMode==GRAPH_COLOR_ARGB || colorMode==GRAPH_COLOR_RGBA  ||
        colorMode==GRAPH_COLOR_XBGR || colorMode==GRAPH_COLOR_ABGR || colorMode==GRAPH_COLOR_BGRA) {
        if (viewMode==VIEW_ZDIR) {      // Z
            xsize = cmnHead.xsize;
            ysize = cmnHead.ysize;
            zsize = 1;
        }
        else {
            CString mesg;
            mesg.LoadString(IDS_STR_UNSPT_VWMODE);
            MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
            pFrame->doneErrorMessage = TRUE;
            return;
        }
    }

    else {
        if (viewMode==VIEW_ZDIR) {      // Z
            xsize = cmnHead.xsize;
            ysize = cmnHead.ysize;
            zsize = cmnHead.zsize;
        }
        else if (viewMode==VIEW_XDIR) { // X
            xsize = cmnHead.ysize;
            ysize = cmnHead.zsize;
            zsize = cmnHead.xsize;
            if (ysize<=1) {
                CString mesg;
                mesg.LoadString(IDS_STR_INVLD_VWMODE);
                MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
                pFrame->doneErrorMessage = TRUE;
                return;
            }
        }
        else if (viewMode==VIEW_YDIR) { // Y
            xsize = cmnHead.xsize;
            ysize = cmnHead.zsize;
            zsize = cmnHead.ysize;
            if (ysize<=1) {
                CString mesg;
                mesg.LoadString(IDS_STR_INVLD_VWMODE);
                MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
                pFrame->doneErrorMessage = TRUE;
                return;
            }

        }
        else {
            CString mesg;
            mesg.LoadString(IDS_STR_UNKWN_VWMODE);
            MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
            pFrame->doneErrorMessage = TRUE;
            return;
        }
    }

    //
    cMax = pDoc->cMax;
    cMin = pDoc->cMin;
    vMax = pDoc->vMax;
    vMin = pDoc->vMin;

    //
    D3DFORMAT format;

    // RGB 8bit/plane
    // なんで xsize, ysize は偶数なんだっけ！？
    if (colorMode==GRAPH_COLOR_RGB || colorMode==GRAPH_COLOR_BGR) {
        //xsize = (((xsize*3+3)/4)*4+2)/3;
        //ysize = (((ysize*3+3)/4)*4+2)/3;
        viewData.set(xsize, ysize, 1, 24);
        format = D3DFMT_X8R8G8B8;
        vMax = cMax = VIEW_8_MAX;
        vMin = cMin = VIEW_8_MIN;
    }
    // ARGB/RGBA 8bit/plane
    else if (colorMode==GRAPH_COLOR_ARGB || colorMode==GRAPH_COLOR_RGBA) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_A8R8G8B8;
        vMax = cMax = VIEW_8_MAX;
        vMin = cMin = VIEW_8_MIN;
    }
    // XRGB 8bit/plane
    else if (colorMode==GRAPH_COLOR_XRGB) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_X8R8G8B8;
        vMax = cMax = VIEW_8_MAX;
        vMin = cMin = VIEW_8_MIN;
    }
    // ABGR/BGRA 8bit/plane
    else if (colorMode==GRAPH_COLOR_ABGR || colorMode==GRAPH_COLOR_BGRA) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_A8R8G8B8;
        vMax = cMax = VIEW_8_MAX;
        vMin = cMin = VIEW_8_MIN;
    }
    // XBGR 8bit/plane
    else if (colorMode==GRAPH_COLOR_XBGR) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_X8R8G8B8;
        vMax = cMax = VIEW_8_MAX;
        vMin = cMin = VIEW_8_MIN;
    }
    // ARGB16/RGBA16 16bit/plane
    else if (colorMode==GRAPH_COLOR_ARGB16 || colorMode==GRAPH_COLOR_RGBA16) {
        //xsize += xsize%2;
        //ysize += ysize%2;
        viewData.set(xsize, ysize, 1, 16);
        format = D3DFMT_A4R4G4B4;
        vMax = cMax = SWORDMAX;
        vMin = cMin = SWORDMIN;
    }
    // RGB16 16bit/plane
    else if (colorMode==GRAPH_COLOR_RGB16) {
        //xsize += xsize%2;
        //ysize += ysize%2;
        viewData.set(xsize, ysize, 1, 16);
        format = D3DFMT_R5G6B5;
        vMax = cMax = SWORDMAX;
        vMin = cMin = SWORDMIN;
    }
    // GRAPH_COLOR_MONO, MONO16  8,16bit/plane (MONO16は MONOに変換されて表示される)
    else {
        //xsize = ((xsize+3)/4)*4;
        //ysize = ((ysize+3)/4)*4;
        viewData.set(xsize, ysize);
        format = D3DFMT_R5G6B5;
    }

    //
    origXSize  = xsize;
    origYSize  = ysize;
    sizeXYRate = (double)ysize/(double)xsize;

    renderRect.top    = 0;
    renderRect.left   = 0;
    renderRect.right  = xsize;
    renderRect.bottom = ysize;
    vSBmax            = zsize - 1;

    SetScrollRange(SB_VERT, 0, vSBmax);
    SetScrollPos(SB_VERT, vSBpos);

    //
    // DX9
    //
    BOOL rslt = InitDevice(xsize, ysize);
    if (!rslt) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
        MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CRT_BAKBUF);
        MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
        return;
    }

    HRESULT hr = lpD3DDevice->CreateOffscreenPlainSurface(xsize, ysize, format, D3DPOOL_DEFAULT, &lpSurface, NULL);
    if (FAILED(hr)) { 
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CRT_SURFACE);
        MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    hasViewData = SetNewSurface(vSBpos);
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_SURFACE);
        MessageBox(_T("CDxMGRView::OnInitialUpdate():\n\n") + mesg); 
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    //
    SetWindowSize(viewData.xsize, viewData.ysize);
    return;
}


BOOL CDxMGRView::SetNewSurface(int dummy)
{
    int     point, n;
    double  ca, cb;
    HRESULT hr;

    if (isNull(viewData.grptr)) {
        CString mesg, noti;
        mesg.LoadString(IDS_STR_DLTD_DATA);
        noti.LoadString(IDS_STR_ERROR);
        MessageBox(_T("CDxMGRView::SetNewSurface():\n\n") + mesg, noti, MB_OK);
        return FALSE;
    }

    // 
    check_endian();
    MakeSetTitle();

    // コントラスト調整
    //   cMax, cMin: 可変　　vMax, vMin:コントラストの上限と下限（固定）
    if (cMax==cMin) cMax = cMin + 1;
    ca = ((double)(vMax-vMin))/((double)(cMax-cMin));
    cb = ((double)(vMin-cMin))/((double)(cMax-cMin));

    // RGB 8bit/plane
    if (colorMode==GRAPH_COLOR_RGB) {
        for (int k=0; k<3; k++) {
            if (HostEndian==BIG_ENDIAN) n = k;
            else                        n = 2 - k;
            for (int j=0; j<cmnHead.ysize; j++) {
                for (int i=0; i<cmnHead.xsize; i++) {
                    uByte* ptr = &viewData.point(i, j);
                    point  = (int)(cmnHead.point(i, j, k)*ca + VIEW_8_MAX*cb + 0.5);
                    point  = Min(point, VIEW_8_MAX);
                    point  = Max(point, VIEW_8_MIN);
                    ptr[n] = point;
                }
            }
        }

        int srcpitch = viewData.xsize*3;
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // BGR 8bit/plane
    else if (colorMode==GRAPH_COLOR_BGR) {
        for (int k=0; k<3; k++) {
            if (HostEndian==BIG_ENDIAN) n = 2 - k;
            else                        n = k;
            for (int j=0; j<cmnHead.ysize; j++) {
                for (int i=0; i<cmnHead.xsize; i++) {
                    uByte* ptr = &viewData.point(i, j);
                    point  = (int)(cmnHead.point(i, j, k)*ca + VIEW_8_MAX*cb + 0.5);
                    point  = Min(point, VIEW_8_MAX);
                    point  = Max(point, VIEW_8_MIN);
                    ptr[n] = point;
                }
            }
        }

        int srcpitch = viewData.xsize*3;
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // XRGB/ARGB 8bit/plane
    else if (colorMode==GRAPH_COLOR_XRGB || colorMode==GRAPH_COLOR_ARGB) {
        for (int k=0; k<4; k++) {
            if (HostEndian==BIG_ENDIAN) n = k;
            else                        n = 3 - k;
            for (int j=0; j<cmnHead.ysize; j++) {
                for (int i=0; i<cmnHead.xsize; i++) {
                    uByte* ptr = &viewData.point(i, j);
                    point  = (int)(cmnHead.point(i, j, k)*ca + VIEW_8_MAX*cb + 0.5);
                    point  = Min(point, VIEW_8_MAX);
                    point  = Max(point, VIEW_8_MIN);
                    ptr[n] = point;
                }
            }
        }

        int srcpitch = viewData.xsize*4;
        if (colorMode==GRAPH_COLOR_XRGB) {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_X8R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);   
        }
        else {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);   
        }
    }

    // RGBA 8bit/plane
    else if (colorMode==GRAPH_COLOR_RGBA) {
        for (int k=0; k<4; k++) {
            if (HostEndian==BIG_ENDIAN) n = 3 - k;
            else                        n = k;
            for (int j=0; j<cmnHead.ysize; j++) {
                for (int i=0; i<cmnHead.xsize; i++) {
                    uByte* ptr = &viewData.point(i, j);
                    point  = (int)(cmnHead.point(i, j, k)*ca + VIEW_8_MAX*cb + 0.5);
                    point  = Min(point, VIEW_8_MAX);
                    point  = Max(point, VIEW_8_MIN);
                    ptr[n] = point;
                }
            }
        }

        int srcpitch = viewData.xsize*4;
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8B8G8R8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // XBGR/ABGR 8bit/plane
    else if (colorMode==GRAPH_COLOR_XBGR || colorMode==GRAPH_COLOR_ABGR) {
        for (int k=0; k<4; k++) {
            if (HostEndian==BIG_ENDIAN) n = k;
            else                        n = 3 - k;
            for (int j=0; j<cmnHead.ysize; j++) {
                for (int i=0; i<cmnHead.xsize; i++) {
                    uByte* ptr = &viewData.point(i, j);
                    point  = (int)(cmnHead.point(i, j, k)*ca + VIEW_8_MAX*cb + 0.5);
                    point  = Min(point, VIEW_8_MAX);
                    point  = Max(point, VIEW_8_MIN);
                    ptr[n] = point;
                }
            }
        }

        int srcpitch = viewData.xsize*4;
        if (colorMode==GRAPH_COLOR_XBGR) {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_X8B8G8R8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
        else {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8B8G8R8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
    }

    // BGRA 8bit/plane
    else if (colorMode==GRAPH_COLOR_BGRA) {
        for (int k=0; k<4; k++) {
            if (HostEndian==BIG_ENDIAN) n = 3 - k;
            else                        n = k;
            for (int j=0; j<cmnHead.ysize; j++) {
                for (int i=0; i<cmnHead.xsize; i++) {
                    uByte* ptr = &viewData.point(i, j);
                    point  = (int)(cmnHead.point(i, j, k)*ca + VIEW_8_MAX*cb + 0.5);
                    point  = Min(point, VIEW_8_MAX);
                    point  = Max(point, VIEW_8_MIN);
                    ptr[n] = point;
                }
            }
        }

        int srcpitch = viewData.xsize*4;
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // RGB16/ARGB16/RGBA16 16bit/plane
    else if (colorMode==GRAPH_COLOR_RGB16 || colorMode==GRAPH_COLOR_ARGB16 || colorMode==GRAPH_COLOR_RGBA16) {
        uWord* src = NULL;
        uWord* dst = NULL;

        if (prevSBpos!=vSBpos) {
            if (viewMode==VIEW_ZDIR) {
                for (int j=0; j<cmnHead.ysize; j++) {
                    for (int i=0; i<cmnHead.xsize; i++) {
                        src = (uWord*)&cmnHead.point(i, j, vSBpos);
                        dst = (uWord*)&viewData.point(i, j);
                        point = (int)(*src*ca + vMax*cb + 0.5);
                        //point = Min(point, vMax);
                        //point = Max(point, vMin);
                        *dst  = point;
                    }
                }
            }
            else if (viewMode==VIEW_XDIR) {
                for (int j=0; j<cmnHead.zsize; j++) {
                    for (int i=0; i<cmnHead.ysize; i++) {
                        src = (uWord*)&cmnHead.point(vSBpos, i, j);
                        dst = (uWord*)&viewData.point(i, j);
                        point = (int)(*src*ca + vMax*cb + 0.5);
                        //point = Min(point, vMax);
                        //point = Max(point, vMin);
                        *dst  = point;
                    }
                }
            }
            else if (viewMode==VIEW_YDIR) {
                for (int j=0; j<cmnHead.zsize; j++) {
                    for (int i=0; i<cmnHead.xsize; i++) {
                        src = (uWord*)&cmnHead.point(i, cmnHead.ysize-1-vSBpos, j);
                        dst = (uWord*)&viewData.point(i, j);
                        point = (int)(*src*ca + vMax*cb + 0.5);
                        //point = Min(point, vMax);
                        //point = Max(point, vMin);
                        *dst  = point;
                    }
                }
            }
            else  return FALSE;
        }

        ntoh_data(viewData.grptr, viewData.lsize, 2);
        int srcpitch = viewData.xsize*2;
        if (colorMode==GRAPH_COLOR_RGB16) {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_R5G6B5, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
        else if (colorMode==GRAPH_COLOR_ARGB16) {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A4R4G4B4, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
        else {  // GRAPH_COLOR_RGBA16
            // R4G4B4A4 -> A4R4G4B4
            for (int i=0; i<viewData.xsize*viewData.ysize*2; i+=2) {
                int r = (viewData.grptr[i]  &0xf0)>>4;
                int g = (viewData.grptr[i]  &0x0f)<<4;
                int b = (viewData.grptr[i+1]&0xf0)>>4;
                int a = (viewData.grptr[i+1]&0x0f)<<4;
                viewData.grptr[i]   = (uByte)(a + r);
                viewData.grptr[i+1] = (uByte)(g + b);
            }
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A4R4G4B4, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
    }

    // Monochrome 8,16bit/plane    cmnHead は8bitに変換済み
    else if (colorMode==GRAPH_COLOR_MONO || colorMode==GRAPH_COLOR_MONO16) {
        if (prevSBpos!=vSBpos) {
            if (viewMode==VIEW_ZDIR) {
                for (int j=0; j<cmnHead.ysize; j++) {
                    for (int i=0; i<cmnHead.xsize; i++) {
                        point = (int)(cmnHead.point(i, j, vSBpos)*ca + VIEW_8_MAX*cb + 0.5);
                        point = Min(point, VIEW_8_MAX);
                        point = Max(point, 0);
                        viewData.point(i, j) = point;
                    }
                }
            }
            else if (viewMode==VIEW_XDIR) {
                for (int j=0; j<cmnHead.zsize; j++) {
                    for (int i=0; i<cmnHead.ysize; i++) {
                        point = (int)(cmnHead.point(vSBpos, i, j)*ca + VIEW_8_MAX*cb + 0.5);
                        point = Min(point, VIEW_8_MAX);
                        point = Max(point, 0);
                        viewData.point(i, j) = point;
                    }
                }
            }
            else if (viewMode==VIEW_YDIR) {
                for (int j=0; j<cmnHead.zsize; j++) {
                    for (int i=0; i<cmnHead.xsize; i++) {
                        point = (int)(cmnHead.point(i, cmnHead.ysize-1-vSBpos, j)*ca + VIEW_8_MAX*cb + 0.5);
                        point = Min(point, VIEW_8_MAX);
                        point = Max(point, 0);
                        viewData.point(i, j) = point;
                    }
                }
            }
            else  return FALSE;
        }

        // GRAPH_COLOR_MONO16 は 8bitに変換されている
        int srcpitch = viewData.xsize;
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_L8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    if (FAILED(hr)) return FALSE;

    return TRUE;
}


void  CDxMGRView::ExecRender()
{
    HRESULT  hr;

    MakeSetTitle();

    hr = lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CLR_WND);
        MessageBox(_T("CDxMGRView::ExecRender():\n\n") + mesg);
        return;
    }

    hr = lpD3DDevice->StretchRect(lpSurface, NULL, lpBackBuffer, NULL, D3DTEXF_NONE);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_BAKBUF);
        MessageBox(_T("CDxMGRView::ExecRender():\n\n") + mesg); 
        return;
    }

    hr = lpD3DDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        hr = ResetDx9Device(lpD3DDevice, &d3dParam, this);
        if (FAILED(hr)) {
            CString mesg;
            mesg.LoadString(IDS_STR_FAIL_RESTR_DX9DEV);
            MessageBox(_T("CDxMGRView::ExecRender():\n\n") + mesg);
            pFrame->SendMessage(WM_CLOSE);
            //GetParentFrame()->DestroyWindow();
        }
        else  ExecRender();
    }
}


void  CDxMGRView::InitObject()
{
    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_REGET_BAKBUF);
        MessageBox(_T("CDxMGRView::InitObject():\n\n") + mesg);
        return;
    }

    HRESULT hr;
    if (colorMode==GRAPH_COLOR_RGB || colorMode==GRAPH_COLOR_BGR) {
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &lpSurface, NULL);
    }
    else if (colorMode==GRAPH_COLOR_ARGB || colorMode==GRAPH_COLOR_RGBA) {
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpSurface, NULL);
    }
    else if (colorMode==GRAPH_COLOR_XRGB) {
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &lpSurface, NULL);
    }
    else if (colorMode==GRAPH_COLOR_ABGR || colorMode==GRAPH_COLOR_BGRA) {
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpSurface, NULL);
    }
    else if (colorMode==GRAPH_COLOR_XBGR) {
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &lpSurface, NULL);
    }
    else if (colorMode==GRAPH_COLOR_ARGB16 || colorMode==GRAPH_COLOR_RGBA16) {
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_A4R4G4B4, D3DPOOL_DEFAULT, &lpSurface, NULL);
    }
    else {  // GRAPH_COLOR_MONO, MONO16, RGB16
        hr = lpD3DDevice->CreateOffscreenPlainSurface(viewData.xsize, viewData.ysize, D3DFMT_R5G6B5,   D3DPOOL_DEFAULT, &lpSurface, NULL);
    }

    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_REGET_SURFACE);
        MessageBox(_T("CDxMGRView::InitObject():\n\n") + mesg);
        return;
    }

    hasViewData = SetNewSurface(vSBpos);
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_RETRNS_SURFACE);
        MessageBox(_T("CDxMGRView::InitObject():\n\n") + mesg);
        return;
    }
    return;
}


void  CDxMGRView::MakeSetTitle()
{
    Title = pDoc->preTitle + pDoc->Title + pDoc->pstTitle;
    if (vSBmax>=1) {
        CString mesg;
        mesg.LoadString(IDS_STR_IMG_NUM);
        mesg = _T("  ") + mesg + _T(" %d");
        int no = pDoc->startNo + vSBpos*Sign(pDoc->endNo - pDoc->startNo);

        Title += numbering_name_t((LPCTSTR)mesg, no);
    }

    if (clientRate!=1.0) {
        CString szprcnt;
        szprcnt.Format(_T("  (%d%%)"), (int)(clientRate*100+0.5));
        Title = Title + szprcnt;
    }
    this->SetTitle(Title);
}
