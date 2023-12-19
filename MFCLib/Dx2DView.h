#pragma once

/**
CDx2DView クラス

    CDxBaseViewクラスからの継承クラス．
    対応する Document の cmnHead (pDoc->cmnHead) からデータを受け取り
    DirectXの機能を使って 2D画像表示を行う．静止画用．

    cmnHead.kind==UN_KNOWN の場合は，ExDocumentクラスで DirectXの機能を
    使用して，画像フォーマットの解析を行っておかなければならない．

    また，cmnHead.kind が既知（CT_ADAT, CT_3DX, RAS_DATA）の場合に対しては，
    depthが 8bitにしか対応していないので，Documentクラスで 8bit化して
    おく必要がある．

*/

#include  "DxBaseClass.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CDx2DView ビュー

class CDx2DView : public CDxBaseView  //public CView
{
protected:
    CDx2DView(); 
    DECLARE_DYNCREATE(CDx2DView)

// アトリビュート
public:

// オペレーション
public:
    virtual void    ExecRender();
    virtual void    InitObject(void);

// オーバーライド
    //{{AFX_VIRTUAL(CDx2DView)
public:
    virtual void OnInitialUpdate();
protected:
//  virtual void OnDraw(CDC* pDC);
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    virtual ~CDx2DView();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CDx2DView)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
