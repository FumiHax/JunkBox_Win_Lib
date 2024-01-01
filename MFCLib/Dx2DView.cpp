//
// CDx2DView.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "Dx2DView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CDx2DView

IMPLEMENT_DYNCREATE(CDx2DView, CExView)


CDx2DView::CDx2DView()
{
    cnstXYRate = true;
}


//
CDx2DView::~CDx2DView()
{
}


BEGIN_MESSAGE_MAP(CDx2DView, CDxBaseView)
    //{{AFX_MSG_MAP(CDx2DView)
        // メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します．
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDx2DView 描画
/*
void CDx2DView::OnDraw(CDC* pDC)
{
    ExecWindowReSize();
    ExecRender();
}
*/



/////////////////////////////////////////////////////////////////////////////
// CDx2DView 診断

#ifdef _DEBUG
void CDx2DView::AssertValid() const
{
    CDxBaseView::AssertValid();
}


//
void CDx2DView::Dump(CDumpContext& dc) const
{
    CDxBaseView::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDx2DView メッセージ ハンドラ

//
//  デバイスを初期化して，サーフェイスに表示データを読み込む
//
void  CDx2DView::OnInitialUpdate() 
{
    cmnHead = pDoc->cmnHead;
    if (isNull(cmnHead.grptr)) return;

    origXSize  = cmnHead.xsize;
    origYSize  = cmnHead.ysize;
    sizeXYRate = (double)cmnHead.ysize/(double)cmnHead.xsize;

    BOOL rslt = InitDevice(cmnHead.xsize, cmnHead.ysize);
    if (!rslt) {
        CString func, mesg;
        mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
        MessageBox(_T("CDx2DView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    // グラフィックデータ
    bufferSize = Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    if (bufferSize.x<0) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CRT_BAKBUF);
        MessageBox(_T("CDx2DView::OnInitialUpdate():\n\n") + mesg);
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    // サーフェイスを作成して表示データを読み込む
    hasViewData = Dx9SurfaceFromMemory(lpD3DDevice, &lpSurface, cmnHead); 
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_SURFACE);
        MessageBox(_T("CDx2DView::OnInitialUpdate():\n\n") + mesg); 
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    SetWindowSize(origXSize, origYSize);
    return;
}


void CDx2DView::ExecRender()
{
    //
    Title = pDoc->preTitle + pDoc->Title + pDoc->pstTitle;
    if (clientRate!=1.0) {
        CString szprcnt;
        szprcnt.Format(_T("  (%d%%)"), (int)(clientRate*100+0.5));
        Title = Title + szprcnt;
    }
    CString mesg1, mesg2;
    mesg1.LoadString(IDS_STR_UNSPT_DATA);
    mesg2.LoadString(IDS_STR_IMP_PROCESS);
    Title = Title + _T("  ") + mesg1 + _T(" [") + mesg2 + _T("]");
    this->SetTitle(Title);

    HRESULT  hr;

    hr = lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_CLR_WND);
        MessageBox(_T("CDx2DView::ExecRender():\n\n") + mesg); 
        return;
    }

    hr = lpD3DDevice->StretchRect(lpSurface, NULL, lpBackBuffer, NULL, D3DTEXF_NONE);
    if (FAILED(hr)) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_TRNS_BAKBUF);
        MessageBox(_T("CDx2DView::ExecRender():\n\n") + mesg); 
        return;
    }

    hr = lpD3DDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        hr = ResetDx9Device(lpD3DDevice, &d3dParam, this);
        if (FAILED(hr)) {
            CString mesg;
            mesg.LoadString(IDS_STR_FAIL_RESTR_DX9DEV);
            MessageBox(_T("CDx2DView::ExecRender():\n\n") + mesg);
            pFrame->SendMessage(WM_CLOSE);
            //GetParentFrame()->DestroyWindow();
        }
        else ExecRender();
    }
}


//
void  CDx2DView::InitObject()
{
    Dx9GetBackBuffer(lpD3DDevice, &lpBackBuffer);
    hasViewData = Dx9SurfaceFromMemory(lpD3DDevice, &lpSurface, cmnHead);
    if (!hasViewData) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_REINIT_OBJ);
        MessageBox(_T("CDx2DView::InitObject():\n\n") + mesg); 
        return;
    }
}
