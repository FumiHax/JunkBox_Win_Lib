#pragma once

/**
CDxMGRView クラス

    CDxBaseViewクラスからの継承クラス．マルチ画像表示．16/24/32bit カラー対応.

    対応する Document の cmnHead (pDoc->cmnHead) からデータ（3Dデータ可）を受け取り
    DirectXの機能を使って 2D表示を行う．対直スクロールバー付き．

    現在サポートしているカラー処理は Planeモード (Z方向をRGBの各色に割り当てるモード)のみ
*/

#include  "DxBaseClass.h"

//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CDxMGRView ビュー

class CDxMGRView : public CDxBaseView
{
protected:
    CDxMGRView();
    DECLARE_DYNCREATE(CDxMGRView)

// アトリビュート
public:
    RECT            renderRect;
    void            MakeSetTitle();

// オペレーション
public:
    virtual void    InitObject(void);
    virtual void    ExecRender(void);
    virtual BOOL    SetNewSurface(int ovsbpos=0);

// オーバーライド
public:
    //{{AFX_VIRTUAL(CDxMGRView)
    virtual void OnInitialUpdate();
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    virtual ~CDxMGRView();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CDxMGRView)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
