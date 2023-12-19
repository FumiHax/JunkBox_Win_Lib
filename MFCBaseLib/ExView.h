#pragma once

#include  "MFCBase.h"


/**
CExView クラス

データを表示するためのテンプレートクラス．
通常，CExDocument から cmnHeadをもらって，データの表示を行う．（推奨動作）
cmnHead, msGraph がメモリ管理無しの場合は強制free を行う．
    
制御用変数
    BOOL hasViewData;   // 表示可能な用データを持っているか？ 
                        // hasViewData==TRUE でなければ，データは表示されない．

このクラスを継承するクラスは，少なくとも以下の関数をオーバーライドする必要がある．
    void    OnInitialUpdate();      // クラスの初期化
    void    ExecRender();           // 表示ルーチン
        
ウィンドウサイズのジャストフィット機能あり．SetWindowSize()
ウィンドウのリサイズ制御機能あり．bool cnstSize, cnstXSize, cnstYSize, cnstXYRate;  ExecWindowReSize()
タイマ割り込み機能あり．TimerStart() でタイマを始動させる．TimerStop()で停止．
    タイマ制御中は OnDraw()は無効にする．
マウス制御機能（簡易版）あり．  GetMousePos() （クライアント領域でのマウスの座標）
      
垂直スクロールテンプレート機能 ('04 8/28)
    垂直スクロールバーを使用する場合は，vSBmaxを設定し，SetScrollRange(SB_VERT, 0, vSBmax);
    を呼ぶ．スクロールバーの位置は vSBposに設定される．
    スクロールバーを動かした場合は，SetNewSurface(int ovsbpos)をオーバーライドし，サーフェイス
    を更新する．ovsbposはスクロールバーの以前の位置．
    スクロールの刻みは vSBntc で指定．

@attention
    xsize, ysize は4バイト境界に注意．
*/


#define   VIEW_8_MAX    255
#define   VIEW_8_MIN    0

#define   VIEW_ZDIR     0
#define   VIEW_XDIR     1
#define   VIEW_YDIR     2


//
namespace jbxwl {

using namespace jbxl;


extern   UINT   TimerID;

class    CExDocument;
class    CExView;
class    CExFrame;
class    CAppCallBack;


/////////////////////////////////////////////////////////////////////////////
// CExView ビュー

class CExView : public CView
{
protected:
    CExView();                      // 動的生成に使用されるプロテクト コンストラクタ
    DECLARE_DYNCREATE(CExView)

// アトリビュート
public:
    ExCmnHead        viewData;      // 表示用．メモリ管理無しでの使用が前提．xsize, ysizeは偶数にすべき（どうしてだっけ？）．
    ExCmnHead        cmnHead;       // pDoc->cmnHead への参照用．
    ExMSGraph<sWord> msGraph;       // pDoc->msGraph への参照用．

    CExDocument*     pDoc;
    CExFrame*        pFrame;
    CAppCallBack*    pApp;
    void*            anyData;

    BOOL            hasViewData;

    CString         Title;

    RECT            clientRect;     // クライアント領域
    double          sizeXYRate;     // クライアント領域のサイズ(全体)の縦横比率 (Y/X)．下位クラスで定義する必要がある．
    double          clientRate;     // クライアント領域の倍率（X方向を基準にする）
    double          sizeFac;        // オブジェクトのサイズの倍率 (3D Renderで使用)

    int             xsize;          // 表示用 Xサイズ．偶数にすべき（？）．
    int             ysize;          // 表示用 Yサイズ．偶数にすべき（？）．
    int             zsize;          // 表示用 Zサイズ．

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

    int             vSBpos;         // Vert Scroll の位置
    int             prevSBpos;      // Vert Scroll の直前の位置
    int             vSBmax;         // Vert Scroll の最大値
    int             vSBntch;        // Vert Scroll の刻み幅
    bool            vSBctrl;        // Vert Scroll のコントロール．スクロールを許可するか？
    int             viewMode;       // Z, X, Y方向から見る
    int             colorMode;      // カラーモード
    bool            usePlane;       // Plane モード

    int             cMax;           // コントラストの最大値（可変）
    int             cMin;           // コントラストの最小値（可変）
    int             vMax;           // 源データの最大値（固定）
    int             vMin;           // 源データの最小値（固定）

    bool            doReSize;       // ウィンドウのリサイズ許可

protected:  
    int             timerID;        // タイマー識別子
    bool            ctrlMouse;      // マウスで制御可能か
    bool            activeWin;      // ウィンドウはアクティブか？

// オペレーション
public:
    POINT           GetMousePos();
    virtual int     GetMouseButton(){return 0;} // CDxBaseClassで実装．ここで実装できたら，CDxBaseClassでの実装を削除して virtual をはずす．

    POINT           SetWindowSize(int xs, int ys, BOOL first=TRUE); // クライアントサイズを指定して，ウィンドを配置する．
    POINT           GetWindowSize(POINT pt);                        // クライアントサイズから，ウィンドウサイズを得る．
    POINT           GetClientSize(POINT pt);                        // ウィンドウサイズから，クライアントサイズを得る．
    POINT           GetWindowReSize(POINT pt);                      // ウィンドウのサイズを条件に合わせて再計算する
    POINT           ExecWindowReSize(int xs, int ys);               // ウィンドウを条件に合わせてリサイズする
    void            SetTitle(LPCTSTR title); 

    virtual  POINT  GetWindowDisplaySize(POINT pt) { return pt;}    // ウィンドウが画面に収まるように大きさを再計算する
    virtual  BOOL   SetNewSurface(int ovsbpos=0) { return TRUE;}    // OnVScroll(), OnMouseWheel()の中でサーフェイスを獲得し直す．
    virtual  void   TimerStart(int tm=40) { timerID = TimerID++; SetTimer(timerID, tm, NULL);}
    virtual  void   TimerStop()  { KillTimer(timerID); timerID = -1; }
    virtual  void   ExecRender() {}
    
    // オーバーライド

    //{{AFX_VIRTUAL(CExView)
public:
    virtual  void   OnInitialUpdate();
    virtual  void   OnDraw(CDC* pDC);

protected:
    virtual  void   OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual     ~CExView();

protected:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CExView)
    afx_msg  void   OnSize(UINT nType, int cx, int cy);
    afx_msg  void   OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg  BOOL   OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg  BOOL   OnEraseBkgnd(CDC* pDC);
    afx_msg  void   OnTimer(UINT_PTR nIDEvent);
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
