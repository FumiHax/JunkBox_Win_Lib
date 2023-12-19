#pragma once

/**
CDxVScrollView クラス

    対応する Document の cmnHead (pDoc->cmnHead) からデータを受け取り
    縦スクロール付き（画像の伸張なし）の画像を表示する．
    DirectXの機能を使用する．

    cmnHead.zsize にクライアント領域の初期サイズ（Yサイズ）が格納され
    ている必要がある（Zサイズではない）．つまり，
        (cmnHead.xsize, cmnHead.ysize) は全体の画像サイズ
        (cmnHead.xsize, cmnHead.zsize) は表示用サイズ

    縦スクロールだから，Xサイズ（cmnHead.xsize）は同じ

*/


#include  "DxBaseClass.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CDxVScrollView ビュー

class CDxVScrollView : public  CDxBaseView //public CView
{
protected:
    CDxVScrollView();
    DECLARE_DYNCREATE(CDxVScrollView)

// アトリビュート
public:
    RECT   dispRect;        // メモリ上の領域  dispRect = renderRect + vsbpos
    RECT   renderRect;      // サーフェイスの有効領域 または 表示領域の大きさ
    RECT   bufferRect;      // バックバッファの描画領域
    int    maxClientYSize;  // クライアントの最大Yサイズ．メモリを有効利用するために使用．

// オペレーション
public:
    virtual void    InitObject(void);
    virtual void    ExecRender();
    virtual BOOL    SetNewSurface(int ovsbpos);

// オーバーライド
public:
    //{{AFX_VIRTUAL(CDxVScrollView)
    virtual void OnInitialUpdate();
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    virtual ~CDxVScrollView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CDxVScrollView)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
