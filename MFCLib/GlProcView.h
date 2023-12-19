#pragma once

#ifdef  ENABLE_OPENGL


#include  "GlBaseClass.h"

#include  <gl/glew.h>
#include  <gl/glut.h>
#include  <gl/glext.h>
#include  <gl/wglew.h>
//#include  <GL/glaux.h>


typedef   void (*GL_PPROC_VIEW_FUNC)(void); 


//
namespace jbxwl {


class CGlProcView : public CGlBaseView
{
    DECLARE_DYNCREATE(CGlProcView)

protected:
    CGlProcView();           
    virtual ~CGlProcView();

public:
//  RECT            renderRect;
    //
    GL_PPROC_VIEW_FUNC  initFunc;
    GL_PPROC_VIEW_FUNC  dispFunc;
    GL_PPROC_VIEW_FUNC  postFunc;

    void    SetInitFunc(GL_PPROC_VIEW_FUNC func) { initFunc = func;}
    void    SetDispFunc(GL_PPROC_VIEW_FUNC func) { dispFunc = func;}
    void    SetPostFunc(GL_PPROC_VIEW_FUNC func) { postFunc = func;}

// オペレーション
public:
//  virtual void    ClearObject(void);
    virtual void    SetState(void) {}
    virtual void    InitObject(void) { SetState();}
    virtual void    ExecRender(void);
    virtual void    OnDraw(CDC* pDC) { CGlBaseView::OnDraw(pDC);}

// オーバーライド

    //{{AFX_VIRTUAL(CGlProcView)
    public:
    virtual void    OnInitialUpdate();
    //}}AFX_VIRTUAL

#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
};


}       // namespace


#endif      // ENABLE_OPENGL

