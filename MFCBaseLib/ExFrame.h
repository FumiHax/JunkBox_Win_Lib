#pragma once

/**
CExFrame クラス

    CExView とCExDocument を繋ぐためのフレームテンプレートクラス．

    マウス制御機能（簡易版）あり．  GetMousePos() （クライアント領域でのマウスの座標）
    デフォルトで最大化ボックスは無効である．
*/

#include  "ExToolBar.h"


//
namespace jbxwl {


class   CExDocument;
class   CExView;
class   CExFrame;
class   CExToolBar;
class   CAppCallBack;



/////////////////////////////////////////////////////////////////////////////
// CExFrame フレーム

class CExFrame : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CExFrame)

protected:
    CExFrame(); 


// アトリビュート
public:
    CString         Title;
    CString         preTitle;
    int             colorMode;          // データのカラータイプ（Doc<->View 受け渡し用）

    CExDocument*    pDoc;
    CExView*        pView;
    CAppCallBack*   pApp;
    CMultiDocTemplate*  pTempl;         // 自分を作り出したテンプレートへのポインタ．
    void*           anyData;

    CExToolBar*     toolBar;
    CScrollBar*     hScrollBar;
    CScrollBar*     vScrollBar;

    bool            doneErrorMessage;   // 既にこのテンプレートの，（致命的な）エラーメッセージの処理は行われている．
    bool            cancelOperation;    // オペレータにより，処理がキャンセルされた

    bool            reSizeMoving;       // ウィンドウはリサイズまたは移動中


// オペレーション
public:
    virtual void onLButtonDBLClick() {}


// オーバーライド
public:
    //{{AFX_VIRTUAL(CExFrame)
        virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle=WS_CHILD|WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, 
                                const RECT& rect=rectDefault, CMDIFrameWnd* pParentWnd=NULL, CCreateContext* pContext=NULL);
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual ~CExFrame();

protected:
    //{{AFX_MSG(CExFrame)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:

#ifdef ON_WM_ENTERSIZEMOVE
    afx_msg void OnEnterSizeMove();                                 // for VS2008 over
    afx_msg void OnExitSizeMove();
#else
    afx_msg LRESULT OnEnterSizeMove(WPARAM wParam, LPARAM lParam);  // for VS2005
    afx_msg LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
#endif
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
