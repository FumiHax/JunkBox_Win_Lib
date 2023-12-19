//
// OpenGL用ライブラリ
//


#include  "GlBaseClass.h"


#ifdef  ENABLE_OPENGL


using namespace jbxwl;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGlBaseView for OpenGL

CGlBaseView::CGlBaseView()
{
    m_hRC = NULL;
    m_pDC = NULL;

    ZeroMemory(&glParam, sizeof(PIXELFORMATDESCRIPTOR));
}


//
CGlBaseView::~CGlBaseView()
{
    DEBUG_INFO("DESTRUCTOR: CGlBaseView\n");
    if (pApp!=NULL) pApp->ViewDestructor(this);     // 上位アプリケーションに通知
}


//
//  デバイスのパラメータを設定する．オーバーロード可能．
//
void  CGlBaseView::SetParameter(PIXELFORMATDESCRIPTOR* param)
{
    //DEBUG_ERROR("CGlBaseView::SetParameter\n");
    //
    ZeroMemory(param, sizeof(PIXELFORMATDESCRIPTOR));
    param->nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    param->nVersion   = 1;
    param->dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    param->iPixelType = PFD_TYPE_RGBA;
    param->cColorBits = 24;
    param->cDepthBits = 16;
    param->iLayerType = PFD_MAIN_PLANE;
    //
    colorMode = GRAPH_COLOR_RGBA;
}


//
BOOL  CGlBaseView::InitDevice(int xsize, int ysize, CWnd* cwnd)
{
    ::glDepthFunc(GL_LEQUAL);
    ::glEnable(GL_DEPTH_TEST);

    ::glShadeModel(GL_SMOOTH);

    // Lighting
//  ::glEnable(GL_LIGHTING); 
//  ::glEnable(GL_LIGHT0);
//  ::glEnable(GL_LIGHT1);

    SetWindowSize(xsize, ysize);

    return TRUE; 
}


//
// ウィンドウが画面に収まるように大きさを再計算する
//
POINT  CGlBaseView::GetWindowDisplaySize(POINT pt)
{
    int rx = GetSystemMetrics(SM_CXSCREEN);         // X方向の解像度
    int ry = GetSystemMetrics(SM_CYSCREEN) - 14;    // Y方向の解像度（-ツールバー）

    int scrSize = Min(rx, ry)*2/3;
    int sz = Max(pt.x, pt.y);
    if (sz<=0) return pt; 
    
    double rs = 1.0;
    if (sz>scrSize) {
        rs   = (double)scrSize/sz;
        pt.x = (int)(pt.x*rs);
        pt.y = (int)(pt.y*rs);
    }

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
*/
int   CGlBaseView::GetMouseButton() 
{
    return  0;
}


//
void  CGlBaseView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
    CExView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}




/////////////////////////////////////////////////////////////////////////////////////////////
//

BEGIN_MESSAGE_MAP(jbxwl::CGlBaseView, CExView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()



int  CGlBaseView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CExView::OnCreate(lpCreateStruct)==-1) return -1;

    m_pDC = new CClientDC(this);
    if (m_pDC==NULL) return -1;

    SetParameter(&glParam);

    m_hRC = WGLCreateContext(m_pDC, &glParam);
    if (m_hRC==NULL) {
        delete m_pDC;
        m_pDC = NULL;
        return -1;
    }
    
    return 0;
}


//
void  CGlBaseView::OnDestroy()
{
    CExView::OnDestroy();

    WGLDeleteContext(m_hRC);

    if (m_pDC) {
        delete m_pDC;
        m_pDC = NULL;
    }
}


//
void  CGlBaseView::OnSize(UINT nType, int cx, int cy)
{
    CExView::OnSize(nType, cx, cy);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGlVTXBaseView クラス
//
/*

BEGIN_MESSAGE_MAP(jbxwl::CGlVTXBaseView, CGlBaseView)
    ON_WM_TIMER()
END_MESSAGE_MAP()


CGlVTXBaseView::CGlVTXBaseView()
{
    angleX = angleY = angleZ = 0.0;
    angle  = 0.0;
    rTime  = gTime = 0.0;
    lTime  = 1.0;
    mWheelAc = 0.0;
    mWheelNotch = 1.0;

    startTime = (double)timeGetTime();
}



CGlVTXBaseView::~CGlVTXBaseView()
{
    //DEBUG_INFO("DESTRUCTOR: CGlVTXBaseView\n");
    if (pApp!=NULL) pApp->ViewDestructor(this);     // 上位アプリケーションに通知

    ReleaseVB();
}



void  CGlVTXBaseView::SwitchRotation() 
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



// 回転処理
//      この関数の外で，matTrans（平行移動）が定義されていること．
//      DirectInput は挙動が怪しいので使いたくない....
//
void  CGlVTXBaseView::ExecRotation() 
{
    rTime = (double)timeGetTime() - startTime;

    if (rotation) {
        angle = angle + (rTime - gTime);
    }

    else {
        double  mwheel = 0.0;

        if (mwheel!=0.0) {
            mWheelAc = mWheelAc - mWheelNotch*mwheel/2000.f;
            ExMouseWheel();
        }

        angle = 0.0;
    }
    gTime = rTime;

}



/////////////////////////////////////////////////////////////////
// 描画

void CGlVTXBaseView::OnTimer(UINT_PTR nIDEvent)
{
    if (hasViewData) {
        if (doReSize) ExecWindowReSize(clientRect.right, clientRect.bottom);
        else          ExecRotation();
        ExecRender();
    }
    else  DEBUG_ERROR("CGlVTXBaseView::OnTimer(): ERROR: 表示可能なデータがありません\n");
    CGlBaseView::OnTimer(nIDEvent);
}



void  CGlVTXBaseView::OnDraw(CDC* pDC)
{
    if (hasViewData && timerID==-1) {
        if (doReSize) ExecWindowReSize(clientRect.right, clientRect.bottom);
        else          ExecRotation();
        ExecRender();
    }
    else if (!hasViewData) DEBUG_ERROR("CGlVTXBaseView::OnDraw(): ERROR: 表示可能なデータがありません\n");
}

*/


#endif      // ENABLE_OPENGL
