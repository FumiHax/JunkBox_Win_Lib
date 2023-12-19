#pragma once

/**
CDxDirectView クラス

　Viewの viewDataを直接操作して 2D画像を更新するためのクラス．
　16/24/32bit カラー対応．
　動画用．
*/

#include  "DxBaseClass.h"


//
namespace jbxwl {


// DxDirectView ビュー

class CDxDirectView : public CDxBaseView
{
    DECLARE_DYNCREATE(CDxDirectView)

protected:
    CDxDirectView();           
    virtual ~CDxDirectView();

public:
    RECT            renderRect;

// オペレーション
public:
//  virtual void    ClearObject(void);
    virtual void    InitObject(void);
    virtual void    ExecRender(void);
    virtual BOOL    SetNewSurface(int ovsbpos=0);

    virtual void    OnDraw(CDC* pDC);

// オーバーライド
public:
    //{{AFX_VIRTUAL(CDxDirectView)
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
};



}       // namespace
