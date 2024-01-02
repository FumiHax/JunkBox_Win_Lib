//
// DxDirectView.cpp : 実装ファイル
//

#include  "MFCBase.h"
#include  "DxDirectView.h"


using namespace jbxl;
using namespace jbxwl;


// DxDirectView

IMPLEMENT_DYNCREATE(CDxDirectView, CExView)

//
CDxDirectView::CDxDirectView()
{
}


//
CDxDirectView::~CDxDirectView()
{
}



BEGIN_MESSAGE_MAP(CDxDirectView, CDxBaseView)
END_MESSAGE_MAP()


// DxDirectView 描画

void CDxDirectView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();

    // TODO: 描画コードをここに追加してください．
}


// DxDirectView 診断

#ifdef _DEBUG
void CDxDirectView::AssertValid() const
{
    CDxBaseView::AssertValid();
}


#ifndef _WIN32_WCE
void CDxDirectView::Dump(CDumpContext& dc) const
{
    CDxBaseView::Dump(dc);
}
#endif
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDxDirectView メッセージ ハンドラ

void CDxDirectView::OnInitialUpdate() 
{
    // xsize, ysize の4バイト境界の問題は呼び出し側の責任
    if (xsize<=0 || ysize<=0) return;

    //
    D3DFORMAT format;

    // RGB 8bit/plane
    if (colorMode==GRAPH_COLOR_RGB || colorMode==GRAPH_COLOR_BGR) {
        viewData.set(xsize, ysize, 1, 24);
        format = D3DFMT_X8R8G8B8;
    }
    // ARGB/RGBA 8bit/plane
    else if (colorMode==GRAPH_COLOR_ARGB || colorMode==GRAPH_COLOR_RGBA) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_A8R8G8B8;
    }
    // XRGB 8bit/plane
    else if (colorMode==GRAPH_COLOR_XRGB) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_X8R8G8B8;
    }
    // ABGR/BGRA 8bit/plane
    else if (colorMode==GRAPH_COLOR_ABGR || colorMode==GRAPH_COLOR_BGRA) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_A8R8G8B8;
    }
    // XBGR 8bit/plane
    else if (colorMode==GRAPH_COLOR_XBGR) {
        viewData.set(xsize, ysize, 1, 32);
        format = D3DFMT_X8R8G8B8;
    }
    // ARGB16/RGBA16 16bit/plane
    else if (colorMode==GRAPH_COLOR_ARGB16 || colorMode==GRAPH_COLOR_RGBA16) {
        viewData.set(xsize, ysize, 1, 16);
        format = D3DFMT_A4R4G4B4;
    }
    // RGB16 16bit/plane
    else if (colorMode==GRAPH_COLOR_RGB16) {
        viewData.set(xsize, ysize, 1, 16);
        format = D3DFMT_R5G6B5;
    }
    // GRAPH_COLOR_MONO, MONO16  8,16bit/plane (MONO16は MONOに変換されて表示される)
    else {
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

    //
    // DX9
    //
    BOOL rslt = InitDevice(xsize, ysize);
    if (!rslt) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
        MessageBox(_T("CDxDirectView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CRT_BAKBUF);
        MessageBox(_T("CDxDirectView::OnInitialUpdate():\n\n") + mesg);
        return;
    }

    HRESULT hr = lpD3DDevice->CreateOffscreenPlainSurface(xsize, ysize, format, D3DPOOL_DEFAULT, &lpSurface, NULL);
    if (FAILED(hr)) { 
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CRT_SURFACE);
        MessageBox(_T("CDxDirectView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }
    
    hasViewData = SetNewSurface(0);
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_SURFACE);
        MessageBox(_T("CDxDirectView::OnInitialUpdate():\n\n") + mesg); 
        pFrame->doneErrorMessage = TRUE;
        return;
    }
    
    SetWindowSize(viewData.xsize, viewData.ysize);
    return;
}


BOOL CDxDirectView::SetNewSurface(int dummy)
{
    if (isNull(viewData.grptr)) {
        CString mesg, noti;
        mesg.LoadString(IDS_STR_DLTD_DATA);
        noti.LoadString(IDS_STR_ERROR);
        MessageBox(_T("CDxDirectView::SetNewSurface():\n\n") + mesg, noti, MB_OK);
        return FALSE;
    }

    HRESULT hr;

    // RGB 8bit/plane
    if (colorMode==GRAPH_COLOR_RGB) {
        int srcpitch = viewData.xsize*3;
        ntoh_data(viewData.grptr, viewData.lsize, 3);
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // BGR 8bit/plane
    else if (colorMode==GRAPH_COLOR_BGR) {
        int srcpitch = viewData.xsize*3;
        if (isBigEndian) swap_byte((void*)viewData.grptr, viewData.lsize, 3);
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // XRGB/ARGB 8bit/plane
    else if (colorMode==GRAPH_COLOR_XRGB || colorMode==GRAPH_COLOR_ARGB) {
        int srcpitch = viewData.xsize*4;
        ntoh_data(viewData.grptr, viewData.lsize, 4);
        if (colorMode==GRAPH_COLOR_XRGB) {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_X8R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
        else {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
    }

    // RBGA 8bit/plane
    else if (colorMode==GRAPH_COLOR_RGBA) {
        int srcpitch = viewData.xsize*4;
        if (isBigEndian) swap_byte((void*)viewData.grptr, viewData.lsize, 4);
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8B8G8R8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // XBGR/ABGR 8bit/plane
    else if (colorMode==GRAPH_COLOR_XBGR || colorMode==GRAPH_COLOR_ABGR) {
        int srcpitch = viewData.xsize*4;
        ntoh_data(viewData.grptr, viewData.lsize, 4);
        if (colorMode==GRAPH_COLOR_XBGR) {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_X8B8G8R8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
        else {
            hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8B8G8R8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
        }
    }

    // BGRA 8bit/plane
    else if (colorMode==GRAPH_COLOR_BGRA) {
        int srcpitch = viewData.xsize*4;
        if (isBigEndian) swap_byte((void*)viewData.grptr, viewData.lsize, 4);
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A8R8G8B8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // RGB16 16bit/plane
    else if (colorMode==GRAPH_COLOR_RGB16) {
        int srcpitch = viewData.xsize*2;
        ntoh_data(viewData.grptr, viewData.lsize, 2);
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_R5G6B5, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // ARGB16 16bit/plane
    else if (colorMode==GRAPH_COLOR_ARGB16) {
        int srcpitch = viewData.xsize*2;
        ntoh_data(viewData.grptr, viewData.lsize, 2);
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_A4R4G4B4, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    // RGBA16 16bit/plane
    else if (colorMode==GRAPH_COLOR_RGBA16) {
        int srcpitch = viewData.xsize*2;
        ntoh_data(viewData.grptr, viewData.lsize, 2);
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

    // Monochrome 8,16bit/plane
    else if (colorMode==GRAPH_COLOR_MONO || colorMode==GRAPH_COLOR_MONO16) {
        // GRAPH_COLOR_MONO16 は 8bitに変換されている
        int srcpitch = viewData.xsize;
        hr = D3DXLoadSurfaceFromMemory(lpSurface, NULL, NULL, viewData.grptr, D3DFMT_L8, srcpitch, NULL, &renderRect, D3DX_FILTER_NONE, 0);
    }

    if (FAILED(hr)) return FALSE;

    return TRUE;
}


void  CDxDirectView::ExecRender()
{
    if (isNull(lpD3DDevice)) return;

    HRESULT hr = lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CLR_WND);
        MessageBox(_T("CDxDirectView::ExecRender():\n\n") + mesg);
        return;
    }

    hr = lpD3DDevice->StretchRect(lpSurface, NULL, lpBackBuffer, NULL, D3DTEXF_NONE);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_BAKBUF);
        MessageBox(_T("CDxDirectView::ExecRender():\n\n") + mesg); 
        return;
    }

    hr = lpD3DDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        hr = ResetDx9Device(lpD3DDevice, &d3dParam, this);
        if (FAILED(hr)) {
            DEBUG_INFO("CDxDirectView::ExecRender(): %s (%s)\n", DXGetErrorString(hr), DXGetErrorDescription(hr));
            CString mesg;
            mesg.LoadString(IDS_STR_FAIL_RESTR_DX9DEV);
            MessageBox(_T("CDxDirectView::ExecRender():\n\n") + mesg + _T("\n") + DXGetErrorString(hr));
            pFrame->SendMessage(WM_CLOSE);
        }
        else  ExecRender();
    }
}


void  CDxDirectView::InitObject()
{
    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_REGET_BAKBUF);
        MessageBox(_T("CDxDirectView::InitObject():\n\n") + mesg);
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
        MessageBox(_T("CDxDirectView::InitObject():\n\n") + mesg);
        return;
    }

    hasViewData = SetNewSurface(0);
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_RETRNS_SURFACE);
        MessageBox(_T("CDxDirectView::InitObject():\n\n") + mesg);
        return;
    }
    return;
}
