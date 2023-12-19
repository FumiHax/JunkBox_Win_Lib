#pragma once

#include  "MFCBase.h"

/**

*/

//
namespace jbxwl {

using namespace jbxl;


class    CExDocument;
class    CExTextView;
class    CExTextFrame;
class    CAppCallBack;



/////////////////////////////////////////////////////////////////////////////
// CExTextView ビュー

class CExTextView : public CScrollView
{
protected:
    CExTextView();                  // 動的生成に使用されるプロテクト コンストラクタ
    DECLARE_DYNCREATE(CExTextView)

// アトリビュート
public:

    CDocument*      pDoc;
    CExTextFrame*   pFrame;
    CAppCallBack*   pApp;

    CString         Title;

    RECT            clientRect;     // クライアント領域
    double          sizeXYRate;     // クライアント領域のサイズ(全体)の縦横比率 (Y/X)．下位クラスで定義する必要がある．
    double          clientRate;     // クライアント領域の倍率（X方向を基準にする）

    int             xsize;          //
    int             ysize;          //

    int             origXSize;      // 本来の画像を表示した場合の Xサイズ．下位クラスで定義する必要がある．
    int             origYSize;      // 本来の画像を表示した場合の Yサイズ．下位クラスで定義する必要がある．
    int             initXSize;      // 最初に表示された時のクライアント領域の Xサイズ
    int             initYSize;      // 最初に表示された時のクライアント領域の Yサイズ
    int             prevXSize;      // 直前のクライアント領域の Xサイズ
    int             prevYSize;      // 直前のクライアント領域の Yサイズ

    bool            cnstSize;       // ウィンドウサイズ固定か？ 
    bool            cnstXSize;      // ウィンドウのXサイズ固定か？ 
    bool            cnstYSize;      // ウィンドウのYサイズ固定か？  
    bool            cnstXYRate;     // ウィンドウは sizeXYRate依存 
    int             maxXSize;       // ウィンドウの最大Xサイズ
    int             maxYSize;       // ウィンドウの最大Yサイズ

    bool            doReSize;       // ウィンドウのリサイズ許可

protected:  
    bool            activeWin;      // ウィンドウはアクティブか？

// オペレーション
public:
    POINT           SetWindowSize(int xs, int ys, BOOL first=TRUE); // クライアントサイズを指定して，ウィンドを配置する．
    POINT           GetWindowSize(POINT pt);                        // クライアントサイズから，ウィンドウサイズを得る．
    POINT           GetClientSize(POINT pt);                        // ウィンドウサイズから，クライアントサイズを得る．
    POINT           GetWindowReSize(POINT pt);                      // ウィンドウのサイズを条件に合わせて再計算する
    POINT           ExecWindowReSize(int xs, int ys);               // ウィンドウを条件に合わせてリサイズする
    void            SetTitle(LPCTSTR title); 

    virtual  POINT  GetWindowDisplaySize(POINT pt) { return pt;}    // ウィンドウが画面に収まるように大きさを再計算する
    
    // オーバーライド

    //{{AFX_VIRTUAL(CExTextView)
public:
    virtual  void   OnInitialUpdate();
    virtual  void   OnDraw(CDC* pDC);

protected:
    virtual  void   OnActivateView(BOOL bActivate, CScrollView* pActivateView, CScrollView* pDeactiveView);
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual     ~CExTextView();

protected:
    //{{AFX_MSG(CExTextView)
    afx_msg  void   OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg  BOOL   OnEraseBkgnd(CDC* pDC);
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
