//
// DxSRenderView.cpp : インプリメンテーション ファイル
//

#include  "MFCBase.h"
#include  "DxSRenderView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;


/**
CDxSRenderView クラス （サーフェイスレンダリング）

    CDxVTXBaseViewクラスからの継承クラス
    対応する Document の msGraph (pDoc->msGraph) からデータを受け取り（cmnHeadではないので注意），
    （簡易）サーフェイスレンダリングを行い，DirectXの機能を使って 3D表示する．

    3D表示する場合の色は，ボクセルの輝度値(16bit)に対して，ARGB16(各4bit)
    と解釈する．輝度値(16bit)の設定には
        uWord  ARGB2Word(uWord a, uWord r, uWord g, uWord b)    (要 #include "Gdata.h")
        uWord  RGBA2Word(uWord r, uWord g, uWord b, uWord a)    (要 #include "Gdata.h")
        uWord  RGB2Word (uWord r, uWord g, uWord b)             (要 #include "Gdata.h")
    を使用すると良い．因みに逆の関数は
        D3DCOLOR  Dx9Word2RGBA(uWord pp)                        (要 #include "Dx9.h")
        D3DCOLOR  Dx9Word2ARGB(uWord pp)                        (要 #include "Dx9.h")
        D3DCOLOR  Dx9Word2RGB (uWord pp)                        (要 #include "Dx9.h")
*/



/////////////////////////////////////////////////////////////////////////////
// CDxSRenderView

IMPLEMENT_DYNCREATE(CDxSRenderView, CExView)


CDxSRenderView::CDxSRenderView()
{
    cnstXYRate = true;
//  rotation = true;
    pCounter  = NULL;
    volumeColor = TRUE;
}



CDxSRenderView::~CDxSRenderView()
{
}



BEGIN_MESSAGE_MAP(CDxSRenderView, CDxVTXBaseView)
    //{{AFX_MSG_MAP(CDxSRenderView)
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDxSRenderView 描画
/*
void CDxSRenderView::OnDraw(CDC* pDC)
{   
    //CDocument* pDoc = GetDocument();
    // TODO: この位置に描画用のコードを追加してください
    //ExecWindowReSize();
    //ExecRender();
}
*/



/////////////////////////////////////////////////////////////////////////////
// CDxSRenderView 診断

#ifdef _DEBUG
void CDxSRenderView::AssertValid() const
{
    CDxVTXBaseView::AssertValid();
}


void CDxSRenderView::Dump(CDumpContext& dc) const
{
    CDxVTXBaseView::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CDxSRenderView メッセージ ハンドラ


void  CDxSRenderView::OnInitialUpdate() 
{
//  CDxVTXBaseView::OnInitialUpdate();

    msGraph = pDoc->msGraph;
    if (msGraph.isNull()) return;

    double rzxy = msGraph.RZxy;
    if (rzxy==0.0) rzxy = 1.0;

    Rbound.xmax = msGraph.xs;
    Rbound.ymax = msGraph.ys;
    Rbound.zmax = (int)(msGraph.zs/rzxy+0.5);
    Rbound.xmin = Rbound.ymin = Rbound.zmin = 0;

    center = D3DXVECTOR3((Rbound.xmax + Rbound.xmin)/2.f,
                         (Rbound.ymax + Rbound.ymin)/2.f,
                         (Rbound.zmax + Rbound.zmin)/2.f);
    xsize = Rbound.xmax - Rbound.xmin + 1;
    ysize = Rbound.ymax - Rbound.ymin + 1;
    zsize = Rbound.zmax - Rbound.zmin + 1;
    msize = Max(xsize, ysize);
    msize = Max(msize, zsize);

    sizeFac  = Min(sizeFac, 1.0/msize);

    int dxsize = msize;
    if (dxsize<MSG_DEFAULT_WINDOW_SIZE) dxsize = MSG_DEFAULT_WINDOW_SIZE;
    if (Dx9ReverseZMode) reverseZ = true;
    else                 reverseZ = false;
    
    BOOL rslt = InitDevice(dxsize, dxsize);
    if (!rslt) {
        CString mesg;
        mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
        MessageBox(_T("CDxSRenderView::OnInitialUpdate():\n\n") + mesg); 
        pFrame->doneErrorMessage = TRUE;
        return;
    }

    if (!PrepareVB()) return;
    SetState();

    origXSize  = MSG_DEFAULT_WINDOW_SIZE;
    origYSize  = MSG_DEFAULT_WINDOW_SIZE;
    sizeXYRate = origYSize/(double)origXSize;

    //DEBUG_INFO("CDxSRenderView::OnInitialUpdate():\n");
    SetWindowSize(MSG_DEFAULT_WINDOW_SIZE, MSG_DEFAULT_WINDOW_SIZE);
    hasViewData = TRUE;

    Title = pDoc->preTitle + pDoc->Title + pDoc->pstTitle;
    this->SetTitle(Title);

    return;
}



void  CDxSRenderView::SetState()
{
    D3DLIGHT9    m_light;
    D3DXVECTOR3  mVecDir;
//  D3DMATERIAL9 m_material;
    D3DCAPS9 caps;

    lpD3DDevice->GetDeviceCaps(&caps);
//  if (!(caps.FVFCaps & D3DFVFCAPS_PSIZE)) {
//      MessageBox("CDxSRenderView::SetState\n\nD3DFVF_PSIZE をサポートしていない");
//      double pointsize = 3.0;
//      double pointsize = 4.0;
//      lpD3DDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pointsize));
//  }

    // Z バッファ有効化
    lpD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    // アルファブレンディング無効化
    lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // ポリゴンの両面を描画
    lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // フィルタ設定
    lpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

    // レンダリング時の色の計算方法の設定
    lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

    // ライト
    lpD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    ZeroMemory(&m_light, sizeof(D3DLIGHT9));
    m_light.Type = D3DLIGHT_DIRECTIONAL;
    m_light.Diffuse.r = 1.0f;
    m_light.Diffuse.g = 1.0f;
    m_light.Diffuse.b = 1.0f;

    // ライト0 の方向
//  mVecDir = D3DXVECTOR3(1.0f, -1.0f, -2.0f);
    mVecDir = D3DXVECTOR3(1.0f, -2.0f, -1.0f);
//  mVecDir = D3DXVECTOR3(1.0f,  2.0f, 1.0f);
    D3DXVec3Normalize((D3DXVECTOR3*)&m_light.Direction, &mVecDir);
    lpD3DDevice->SetLight(0, &m_light);
    lpD3DDevice->LightEnable(0, TRUE);

    // ライト1 の方向
//  mVecDir = D3DXVECTOR3(1.0, 1.0, 1.0);
//  D3DXVec3Normalize((D3DXVECTOR3*)&m_light.Direction, &mVecDir);
//  lpD3DDevice->SetLight(1, &m_light);
//  lpD3DDevice->LightEnable(1, TRUE);
//
    // ライト2 の方向
//  mVecDir = D3DXVECTOR3(-1.0, 1.0, -1.0);
//  D3DXVec3Normalize((D3DXVECTOR3*)&m_light.Direction, &mVecDir);
//  lpD3DDevice->SetLight(2, &m_light);
//  lpD3DDevice->LightEnable(2, TRUE);

    // プロジェクション行列を設定: 視界 60度，近くの限界 0.1，遠くの限界 10*msize*sizeFac
    //D3DXMatrixPerspectiveFovLH(&matProj, (float)(60.0*PI/180.0), 1.0f, 0.1f, (float)(10.0*msize*sizeFac));
    D3DXMatrixPerspectiveFovLH(&matProj, (float)(PI/3.0), 1.0f, 0.1f, (float)(10.0*msize*sizeFac));
    lpD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    // カメラの制御:  カメラの位置，物体の位置，カメラの上方向の傾き()
//  D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0,0,1.5f*msize*sizeFac), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
//  D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0,-1.5f*msize*sizeFac,0), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,-1));
//  lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);
    if (mWheelAc==0.0) mWheelAc = 1.5*msize*sizeFac;
    ExMouseWheel();

    // 頂点フォーマット設定
    lpD3DDevice->SetFVF(FVF_SVERTEXV);
    
    D3DXMatrixTranslation(&matTrans, -(float)(center.x*sizeFac), -(float)(center.y*sizeFac), (float)(center.z*sizeFac));
    lpD3DDevice->SetTransform(D3DTS_WORLD, &matTrans);
}


// 
void  CDxSRenderView::ExMouseWheel()
{
    if (Xabs(mWheelAc)<1.0) mWheelAc = Sign(mWheelAc);

    // カメラの制御:  カメラの位置，物体の位置，カメラの上方向の傾き()
    D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, (float)mWheelAc, 0.0f), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1));
    lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);

    double pointsize = sizeFac/Xabs(mWheelAc)*1000.0;
    lpD3DDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pointsize));
}

        
//    
void  CDxSRenderView::ExecRender()
{
    HRESULT hr;

    // 画面のクリア
    hr = lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,96), 1.0f, 0);
    if (FAILED(hr)) { DEBUG_ERROR("CDxSRenderView::ExecRender(): ERROR: 画面クリアに失敗しました\n"); return;}
    
    // シーンの描画開始
    hr = lpD3DDevice->BeginScene();
    if (FAILED(hr)) { DEBUG_ERROR("CDxSRenderView::ExecRender(): ERROR: シーンの開始に失敗しました\n"); return;}

    hr = lpD3DDevice->SetStreamSource(0, vb, 0, sizeof(SVERTEXV));
    if (FAILED(hr)) { DEBUG_ERROR("CDxSRenderView::ExecRender(): ERROR: ストリームの設定に失敗しました\n"); lpD3DDevice->EndScene(); return;}
    
    hr = Dx9DrawPrimitive(lpD3DDevice, D3DPT_POINTLIST, 1, datano);
    if (FAILED(hr)) { DEBUG_ERROR("CDxSRenderView::ExecRender(): ERROR: プリミティブ描画に失敗しました\n"); lpD3DDevice->EndScene(); return;}
    
    hr = lpD3DDevice->EndScene();   
    if (FAILED(hr)) { DEBUG_ERROR("CDxSRenderView::ExecRender(): ERROR: シーンの終了に失敗しました\n"); return;}
    // シーンの描画終了

    // 画面表示
    hr = lpD3DDevice->Present(NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {   // デバイスチェック
//      DEBUG_INFO("CDxSRenderView:ExecRender(): Resetting Device ....\n"); 
        hr = ResetDx9Device(lpD3DDevice, &d3dParam, this);
        if (FAILED(hr)) {
            CString mesg;
            mesg.LoadString(IDS_STR_FAIL_RESTR_DX9DEV);
            MessageBox(_T("CDxSRenderView::ExecRender():\n\n") + mesg);
            pFrame->SendMessage(WM_CLOSE);
            //GetParentFrame()->DestroyWindow();
        }
    }
}



// 頂点バッファの準備
BOOL   CDxSRenderView::PrepareVB()
{
    int  i,  j,  k;
    int  x,  y,  z;
    int ii, jj, kk;
    int is, js, ks;
    int ie, je, ke;
    double rzxy;

    // 新しいカウンタ
    pCounter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE);
    if (pCounter!=NULL) {
        CString mesg;
        mesg.LoadString(IDS_STR_PROC_VERTEX_BUF);
        char* mbstr = ts2mbs(mesg);
        pCounter->Start(msGraph.xs*msGraph.ys*msGraph.zs*2, mbstr);
        ::free(mbstr);
    }

    datano = GetValidVoxelNum();
    if (datano<=0) {
        if (pCounter!=NULL) {
            pCounter->Stop();
            deleteNull(pCounter);
            pFrame->cancelOperation  = TRUE;
            pFrame->doneErrorMessage = TRUE;
        }
        CString mesg, noti;
        if (datano==-1) {
            mesg.LoadString(IDS_STR_CANCEL_VERTEX_BUF_SET1);
            noti.LoadString(IDS_STR_CANCEL);
            MessageBox(_T("CDxSRenderView::PrepareVB():\n\n") + mesg, noti);
        }
        else if (datano==0) {
            mesg.LoadString(IDS_STR_INFO_VERTEX_ZERO);
            noti.LoadString(IDS_STR_INFO);
            MessageBox(_T("CDxSRenderView::PrepareVB():\n\n") + mesg, noti);
        }
        else {
            mesg.LoadString(IDS_STR_ERR_VERTEX_NUM);
            noti.LoadString(IDS_STR_ERROR);
            MessageBox(_T("CDxSRenderView::PrepareVB():\n\n") + mesg, noti);
        }
        return FALSE;
    }

    lpD3DDevice->CreateVertexBuffer(sizeof(SVERTEXV)*datano, 0, FVF_SVERTEXV, D3DPOOL_MANAGED, &vb, NULL);
    if (vb==NULL) {
        if (pCounter!=NULL) {
            pCounter->Stop();
            deleteNull(pCounter);
        }
        CString mesg, noti;
        mesg.LoadString(IDS_STR_FAIL_GET_VERTEX_BUF);
        noti.LoadString(IDS_STR_ERROR);
        MessageBox(_T("CDxSRenderView::PrepareVB():\n\n") + mesg, noti);
        return FALSE;
    }

    SVERTEXV *v;
    vb->Lock(0, 0, (void **)&v, 0);

    rzxy = msGraph.RZxy;
    if (rzxy==0.0) rzxy = 1.0;

    int cnt= 0;
    for (k=0; k<msGraph.zs; k++) {
        for (j=0; j<msGraph.ys; j++) {
            for (i=0; i<msGraph.xs; i++) {
                x = y = z = 0;
                bool chk = false;
                if (msGraph.point(i, j, k)!=0) {
                    is = js = ks = -1;
                    ie = je = ke =  1;

                    // 境界
                    if (i==0)            { is = 0; x = -10; chk = true;}    // -9-1
                    if (i==msGraph.xs-1) { ie = 0; x =  10; chk = true;}    //  9+1
                    if (j==0)            { js = 0; y = -10; chk = true;}
                    if (j==msGraph.ys-1) { je = 0; y =  10; chk = true;}
                    if (k==0)            { ks = 0; z = -10; chk = true;}
                    if (k==msGraph.zs-1) { ke = 0; z =  10; chk = true; if (msGraph.zs==1) z = -10;}

                    // サーフェイスの判定
                    for (kk=ks; kk<=ke; kk++) 
                    for (jj=js; jj<=je; jj++) 
                    for (ii=is; ii<=ie; ii++) {
                        if (msGraph.point(i+ii, j+jj, k+kk)==0) { 
                            x   = x  + ii;
                            y   = y  + jj;
                            z   = z  + kk;
                            chk = true;
                        }
                    }

                    // サーフェイス
                    if (chk) { 
                        //
                        v[cnt].ps =  3.5f;
                        v[cnt].x  =  i*(float)sizeFac;
                        v[cnt].y  =  j*(float)sizeFac;
                        
                        if (reverseZ) v[cnt].z  = -(msGraph.zs-1-k)*(float)(sizeFac/rzxy);   // Z軸の反転
                        else          v[cnt].z  = -k*(float)(sizeFac/rzxy);

                        int n2 = x*x + y*y + z*z;
                        if (n2!=0) {
                            float nn = (float)sqrt((double)n2);
                    
                            v[cnt].nx = x/nn;
                            v[cnt].ny = y/nn;

                            if (reverseZ) v[cnt].nz =  z/nn;    // Z軸の反転
                            else          v[cnt].nz = -z/nn;
                        }
                        //
                        else {
                            v[cnt].nx = -1.0f;
                            v[cnt].ny =  1.0f;
                            v[cnt].nz =  1.0f;
                        }

                        if (volumeColor) {
                            if (msGraph.color == GRAPH_COLOR_R4G4B4A4 || msGraph.color == GRAPH_COLOR_RGBA16) {
                                v[cnt].color = Dx9Word2RGBA((uWord)msGraph.point(i, j, k));
                            }
                            else if (msGraph.color == GRAPH_COLOR_A4R4G4B4 || msGraph.color == GRAPH_COLOR_ARGB16) {
                                v[cnt].color = Dx9Word2ARGB((uWord)msGraph.point(i, j, k));
                            }
                            else {
                                v[cnt].color = Dx9Word2RGB((uWord)msGraph.point(i, j, k));
                            }
                        }
                        else {
                            v[cnt].color = D3DCOLOR_RGBA(192, 192, 192, 192);
                        }
                        cnt++;
                    }
                }
            }
        }

        // カウンタ
        if (pCounter!=NULL) {
            pCounter->SetPos(msGraph.xs*msGraph.ys*msGraph.zs + (k+1)*msGraph.xs*msGraph.ys);
            if (pCounter->isCanceled()) {
                pCounter->Stop();
                deleteNull(pCounter);
                vb->Unlock();
                DXRELEASE(vb);

                CString mesg, noti;
                mesg.LoadString(IDS_STR_CANCEL_VERTEX_BUF_SET2);
                mesg.LoadString(IDS_STR_CANCEL);
                MessageBox(_T("CDxSRenderView::PrepareVB():\n\n") + mesg, noti);

                pFrame->cancelOperation  = TRUE;
                pFrame->doneErrorMessage = TRUE;
                return FALSE;
            }
        }
    }

    datano = cnt;
    vb->Unlock();

    if (pCounter!=NULL) {
        pCounter->Stop();
        deleteNull(pCounter);
    }
    return TRUE;
}


//
//  (angleX, angleY -> angle?, angle?)
// 
void  CDxSRenderView::ExRotationAngle()
{
    angleZ = -angleX;
    angleX = -angleY;
    angleY =  0.0;
}


//
void  CDxSRenderView::ClearObject()
{
    hasViewData = FALSE;
//  TimerStop();
}


//
void  CDxSRenderView::InitObject()
{
    SetState();
//  TimerStart();
    hasViewData = TRUE;
}


//
int  CDxSRenderView::GetValidVoxelNum()
{
    int  i,  j,  k;
    int ii, jj, kk;
    int is, js, ks;
    int ie, je, ke;
    int cnt = 0;

    for (k=0; k<msGraph.zs; k++) {
        for (j=0; j<msGraph.ys; j++) {
            for (i=0; i<msGraph.xs; i++) {
                bool chk = false;
                if (msGraph.point(i, j, k)!=0) {
                    is = js = ks = -1;
                    ie = je = ke =  1;

                    if (i==0 || i==msGraph.xs-1 ||
                        j==0 || j==msGraph.ys-1 || 
                        k==0 || k==msGraph.zs-1    ) chk = true;
                    else {
                        for (kk=ks; kk<=ke; kk++) 
                        for (jj=js; jj<=je; jj++) 
                        for (ii=is; ii<=ie; ii++) {  
                            if (msGraph.point(i+ii, j+jj, k+kk)==0) chk = true;
                        }
                    }
                }
                if (chk) cnt++;
            }
        }
        if (pCounter!=NULL) {
            pCounter->SetPos((k+1)*msGraph.xs*msGraph.ys);
            if (pCounter->isCanceled()) return -1;
        }
    }
    return cnt;
}


