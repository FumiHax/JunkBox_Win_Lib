
#include  "DxBaseClass.h"


using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDxBaseView for DirectX

BEGIN_MESSAGE_MAP(jbxwl::CDxBaseView, CExView)
END_MESSAGE_MAP()


CDxBaseView::CDxBaseView()
{
    lpD3D        = NULL;
    lpD3DDevice  = NULL;    // DirectX9 3Dデバイス
    lpBackBuffer = NULL;    // バックバッファ
    lpSurface    = NULL;    // サーフェース
    lpTexture    = NULL;    // テクスチャ

    lpDInput     = NULL;
    lpDMouse     = NULL;    // マウスデバイス
    lpDKeyBoard  = NULL;    // キーボードバイス

    bufferSize.x = bufferSize.y = 0;
}



CDxBaseView::~CDxBaseView()
{
    //DEBUG_INFO("DESTRUCTOR: CDxBaseView\n");
    if (pApp!=NULL) pApp->ViewDestructor(this);     // 上位アプリケーションに通知

//  DXRELEASE(lpDMouse);        // リリース禁止
//  DXRELEASE(lpDKeyBoard);     // リリース禁止
    DXRELEASE(lpTexture);
    DXRELEASE(lpSurface); 
    DXRELEASE(lpBackBuffer);
    DXRELEASE(lpD3DDevice);
}


/**
    機能：  DirectX9のデバイスを作る．
    引数： int cxs, cys ウィンドウの初期サイズ，省略した場合は画像データ(cmnHead)に合わせられる．
    戻値：  成功：TRUE, 失敗：FALSE
    説明：  
*/
BOOL  CDxBaseView::InitDevice(int xsize, int ysize, CWnd* cwnd)
{
    lpD3D = GpD3D;
    if (cwnd ==NULL) cwnd = this;
    SetParameter(&d3dParam, xsize, ysize);

    lpD3DDevice = Dx9CreateGraphic(lpD3D, &d3dParam, cwnd->m_hWnd);
    if (lpD3DDevice==NULL) return FALSE;
    
    // インプットデバイス
    lpDMouse    = GpDMouse;
    lpDKeyBoard = GpDKeyBoard;
    
    return TRUE; 
}


//
void  CDxBaseView::ClearObject()
{
    hasViewData = FALSE;
    DXRELEASE(lpTexture);
    DXRELEASE(lpBackBuffer);
    DXRELEASE(lpSurface);
}


//
//  デバイスのパラメータを設定する．オーバーロード可能．
//
void   CDxBaseView::SetParameter(D3DPRESENT_PARAMETERS* d3dParam, int xsize, int ysize)
{
    RECT rect;
        
    if (xsize>0 && ysize>0) {
        rect.right  = xsize;
        rect.bottom = ysize;
    }
    else {
        this->GetClientRect(&rect);
    }
    rect.right  = (int)(rect.right*1.5);
    rect.bottom = (int)(rect.bottom*1.5);

    ZeroMemory(d3dParam, sizeof(D3DPRESENT_PARAMETERS));
    d3dParam->BackBufferWidth        = rect.right; 
    d3dParam->BackBufferHeight       = rect.bottom;  
    d3dParam->BackBufferCount        = 1; 
    d3dParam->Windowed               = TRUE;
    d3dParam->SwapEffect             = D3DSWAPEFFECT_DISCARD;
//  d3dParam->SwapEffect             = D3DSWAPEFFECT_FLIP;
    d3dParam->BackBufferFormat       = D3DFMT_UNKNOWN;
    d3dParam->EnableAutoDepthStencil = TRUE;
    d3dParam->AutoDepthStencilFormat = D3DFMT_D16;
}


//
// ウィンドウが画面に収まるように大きさを再計算する
//
POINT  CDxBaseView::GetWindowDisplaySize(POINT pt)
{
    D3DDISPLAYMODE disp;

    lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &disp);    // ルートウィンドウのサイズ
    int scrSize = Min(disp.Width, disp.Height)*2/3;

    int sz = Max(pt.x, pt.y);
    if (sz<=0) return pt; 
    
    double rs = 1.0;
    if (sz>scrSize) {
        rs   = scrSize/sz;
        pt.x = (int)(pt.x*rs);
        pt.y = (int)(pt.y*rs);
    }

    //wRateSize.Format("  (%3d%%)", (int)(rs*100+0.5));
    return pt;
}


/**
マウスボタンチェック
    マウスの押されたボタンをチェック．

    戻り値
        0 : ボタンは押されていない．
        1 : 左ボタン
        2 : 右ボタン
        3 : 中ボタン

    実装に DirectInput を使用．(~~;
*/
int   CDxBaseView::GetMouseButton() 
{
    int  ret = 0;
    DIMOUSESTATE dims;

    if (lpDMouse!=NULL && activeWin) {
        lpDMouse->Acquire();
        HRESULT hr = lpDMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims);
        if (SUCCEEDED(hr)) {
            if      (dims.rgbButtons[0]==0x80) ret = 1;             
            else if (dims.rgbButtons[1]==0x80) ret = 2;             
            else if (dims.rgbButtons[2]==0x80) ret = 3;                             
        }
//      lpDMouse->Unacquire();
    }
    return  ret;
}


void  CDxBaseView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
    if (lpDMouse!=NULL) {   
        lpDMouse->Unacquire();
        if (bActivate && pActivateView==this) {
            lpDMouse->Acquire();
        }
    }

    CExView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDxVTXBaseView クラス
//

BEGIN_MESSAGE_MAP(jbxwl::CDxVTXBaseView, CDxBaseView)
    ON_WM_TIMER()
END_MESSAGE_MAP()


CDxVTXBaseView::CDxVTXBaseView()
{
    vb = lb = NULL;

    rotation  = false;
    ctrlMouse = true;
    reverseZ  = false;

    angleX = angleY = angleZ = 0.0;
    angle = 0.0;
    rTime = gTime = 0.0;
    lTime = 1.0;
    mWheelAc = 0.0;
    mWheelNotch = 1.0;

    startTime = (double)timeGetTime();

    D3DXMatrixIdentity(&matRotate); 
    D3DXMatrixIdentity(&matTrans);  
    D3DXMatrixIdentity(&matTemp);
}


//
CDxVTXBaseView::~CDxVTXBaseView()
{
    //DEBUG_INFO("DESTRUCTOR: CDxVTXBaseView\n");
    if (pApp!=NULL) pApp->ViewDestructor(this);     // 上位アプリケーションに通知

    ReleaseVB();
}


//
void  CDxVTXBaseView::SwitchRotation() 
{
    if (rotation) {
        rotation = false;
    }
    else {
        gTime = rTime = (double)timeGetTime() - startTime;
        rotation = true;
    }
    angleX = angleY = angleZ = 0.0;
}


/**
回転処理
    この関数の外で，matTrans（平行移動）が定義されていること．
    DirectInput は挙動が怪しいので使いたくない....
*/
void  CDxVTXBaseView::ExecRotation() 
{
    HRESULT  hr;

    rTime = (double)timeGetTime() - startTime;

    if (rotation) {
        angle = angle + (rTime - gTime);
        D3DXMatrixRotationY(&matRotateY, (float)angle/(200.0f*10));
        D3DXMatrixRotationX(&matRotateX, (float)angle/(100.0f*10));
        matRotate = matTemp*matRotateX*matRotateY;
    }

    else {
        DIMOUSESTATE dims;
        double  mwheel = 0.0;

        if (lpDMouse!=NULL && activeWin) {
            lpDMouse->Acquire();
            hr = lpDMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims);
            if (SUCCEEDED(hr)) {
                if (ctrlMouse) {    // ctrlMouse: マウスによる操作が可能かどうか．
                    mwheel = (double)dims.lZ;           // マウス ホイール移動量
                    if (dims.rgbButtons[0]==0x80) { 
                        POINT pt = GetMousePos();
                        if (pt.x>=0 && pt.y>=0) {
                            angleX = (double)dims.lX;   // マウス X移動量
                            angleY = (double)dims.lY;   // マウス Y移動量
                            ExRotationAngle();          // 呼び出し側の都合により角度を入れ替える．
                            lTime  = rTime - gTime;
                        }
                        else ctrlMouse = false;
                    }
                }
                if (dims.rgbButtons[0]!=0x80 && !ctrlMouse) {
                    POINT pt = GetMousePos();                       // クライアント領域内で左クリックすると
                    if (pt.x>=0 && pt.y>=0) ctrlMouse = true;       // マウスによる操作が可能に．
                }                                            
            }
        }
        if (mwheel!=0.0) {
            mWheelAc = mWheelAc - mWheelNotch*mwheel/2000.f;
            ExMouseWheel();
        }

        D3DXMatrixRotationX(&matRotateX, (float)(angleX/150.0*((rTime-gTime)/lTime)));
        D3DXMatrixRotationY(&matRotateY, (float)(angleY/150.0*((rTime-gTime)/lTime)));
        D3DXMatrixRotationZ(&matRotateZ, (float)(angleZ/150.0*((rTime-gTime)/lTime)));
        matRotate = matTemp = matRotate*matRotateX*matRotateY*matRotateZ;
        angle = 0.0;
    }
    gTime = rTime;

    matWorld = matTrans*matRotate;
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
}



/////////////////////////////////////////////////////////////////
// 描画

void CDxVTXBaseView::OnTimer(UINT_PTR nIDEvent)
{
    if (hasViewData) {
        if (doReSize) ExecWindowReSize(clientRect.right, clientRect.bottom);
        else          ExecRotation();
        ExecRender();
    }
    else  DEBUG_ERROR("CDxVTXBaseView::OnTimer(): ERROR: 表示可能なデータがありません\n");
    CDxBaseView::OnTimer(nIDEvent);
}


//
void  CDxVTXBaseView::OnDraw(CDC* pDC)
{
    if (hasViewData && timerID==-1) {
        if (doReSize) ExecWindowReSize(clientRect.right, clientRect.bottom);
        else          ExecRotation();
        ExecRender();
    }
    else if (!hasViewData) DEBUG_ERROR("CDxVTXBaseView::OnDraw(): ERROR: 表示可能なデータがありません\n");
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    機能：  DirectX9のデバイスをリセットする．

    引数： lpD3DDevice     リセットするDirectX9デバイス 
            *d3dParam       デバイスリセットのためのパラメータ．作成時に使用したものを指定する．
            *pviw           このデバイスを持っている CDxBaseViewクラスを指定する．省略可能（NULL）  

    戻値：  リセットの HRESULT

    説明：  D3DPOOL_DEFAULT管理のリソースなどは，デバイスのリセットを行う前にそれを開放し，
            リセット後に再び獲得しなければならない．pviwに CDxBaseViewクラスのポインタを設定
            した場合，解放関数として pviw->ClearObject() が呼び出され，再獲得関数として 
            pviw->InitObject()が呼び出される．この２つの関数は，CDxBaseViewクラスの継承クラス
            の中でオーバーライドされなければならない．pviw に NULLを指定した場合は，リソース
            の開放・再獲得は行われない．

            CDxBaseView* を引数に持つので，Dx9.cpp へは入れられない．
*/
HRESULT  jbxwl::ResetDx9Device(LPDIRECT3DDEVICE9 lpD3DDevice, D3DPRESENT_PARAMETERS* pd3dParam, CDxBaseView* pviw)
{
    HRESULT hr = lpD3DDevice->TestCooperativeLevel();
    if (hr==D3D_OK) return hr;

    do {
        Sleep(100);
        hr = lpD3DDevice->TestCooperativeLevel();
    } while (hr==D3DERR_DEVICELOST);        // デバイスがリセットできるようになるまで待つ．

    if (hr==D3DERR_DEVICENOTRESET) {        // リセット可能
        if (pviw!=NULL) pviw->ClearObject();

        hr = lpD3DDevice->Reset(pd3dParam);
        if (hr==D3D_OK) {                   // リセット成功
            DEBUG_INFO("jbxwl::ResetDx9Device(): Reseted Dx9 Device.\n");
            if (pviw!=NULL) pviw->InitObject();
        }
        else {
            DEBUG_ERROR("jbxwl::ResetDx9Device(): Failed to Reset.\n");
        }
    }
    return hr;  
}
