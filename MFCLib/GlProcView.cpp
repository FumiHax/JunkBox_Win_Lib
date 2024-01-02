//
// GlProcView.cpp : 実装ファイル
//
//
// Document はダミーとなる
//

#include  "MFCBase.h"
#include  "GlProcView.h"


#ifdef  ENABLE_OPENGL


using namespace jbxl;
using namespace jbxwl;


// GlProcView

IMPLEMENT_DYNCREATE(CGlProcView, CExView)


CGlProcView::CGlProcView()
{
    hasViewData = TRUE;
    //
    initFunc = NULL;
    dispFunc = NULL;
    postFunc = NULL;
}


CGlProcView::~CGlProcView()
{
}


// GlProcView 描画

void CGlProcView::ExecRender()
{
    if (!WGLGetCurrent(this->m_pDC, this->m_hRC)) return;

//  CDocument* pDoc = GetDocument();

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // 描画モード
//  ::glMatrixMode(GL_MODELVIEW);
//  ::glLoadIdentity();

    //
    ::glEnable(GL_NORMALIZE);
    ::glEnable(GL_AUTO_NORMAL);
    ::glPushMatrix();

    if (dispFunc!=NULL) (*dispFunc)();

    ::glPopMatrix();
    ::glDisable(GL_AUTO_NORMAL);
    ::glDisable(GL_NORMALIZE);

    //::glFinish();
    ::glFlush();

    SwapBuffers(m_pDC->GetSafeHdc());
    if (postFunc!=NULL) (*postFunc)();
}



/////////////////////////////////////////////////////////////////////////////
// CGlProcView メッセージ ハンドラ

//
// called from ExecDocFrmView();
//
void CGlProcView::OnInitialUpdate() 
{
    //DEBUG_INFO("CGlProcView::OnInitialUpdate()\n");

    // ウィンドウサイズ　xsize, ysize の4バイト境界の問題は呼び出し側の責任
    if (xsize<=0) xsize = 400;
    if (ysize<=0) ysize = 400;

    //
    origXSize  = xsize;
    origYSize  = ysize;
    sizeXYRate = (float)ysize/(float)xsize;

    //
    InitDevice(xsize, ysize);
    InitObject();
    if (initFunc!=NULL) {
        WGLGetCurrent(this->m_pDC, this->m_hRC);
        (*initFunc)();
    }

    return;
}



//////////////////////////////////////////////////////////////////////////////
//

BEGIN_MESSAGE_MAP(CGlProcView, CGlBaseView)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


void  CGlProcView::OnSize(UINT nType, int cx, int cy)
{
    if (!WGLGetCurrent(this->m_pDC, this->m_hRC)) return;

    CGlBaseView::OnSize(nType, cx, cy);

    //
    if(0>=cx || 0>=cy) return;

    ::glViewport(0, 0, cx, cy);
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();
    ::gluPerspective(40.0f, (GLfloat)cx/(GLfloat)cy, 0.1f, 20.0f);
    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();

    //
    ::glTranslatef( 0.0f, 0.0f, -5.0f );
    ::glRotatef( 0.0f, 0.0f, 0.0f, 0.0f );
}


int  CGlProcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    //DEBUG_INFO("CGlProcView::OnCreate()\n");

    if (CGlBaseView::OnCreate(lpCreateStruct)==-1) return -1;

    return 0;
}


void  CGlProcView::OnDestroy()
{
    CGlBaseView::OnDestroy();
}


// GlProcView 診断

#ifdef _DEBUG
void CGlProcView::AssertValid() const
{
    CGlBaseView::AssertValid();
}


#ifndef _WIN32_WCE
void CGlProcView::Dump(CDumpContext& dc) const
{
    CGlBaseView::Dump(dc);
}
#endif
#endif //_DEBUG


#endif      // ENABLE_OPENGL
