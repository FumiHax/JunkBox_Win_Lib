#pragma once

/**
OpenGL用ライブラリ

*/

#include  "WGL.h"
#include  "ExClass.h"
#include  <mmsystem.h>


#ifdef  ENABLE_OPENGL


//
namespace jbxwl {
 

////////////////////////////////////////////////////////////////////////
//  CGlBaseViewクラス
//
class   CGlBaseView : public CExView  
{
public:
        HGLRC       m_hRC;
        CDC*        m_pDC;

        PIXELFORMATDESCRIPTOR glParam;
//
public:
    CGlBaseView();
    virtual ~CGlBaseView();

protected:
    virtual void  OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
    virtual void  OnInitialUpdate(void) {}  // 初期化
    virtual void  SetState(void) {}         // 環境設定
    virtual void  ExecRender(void) {}       // 描画

//  virtual void  ClearObject(void){hasViewData = FALSE;}   // デバイスリセット時のリソース開放関数
    virtual void  InitObject(void) { SetState();}           // デバイスリセット時のリソース再獲得関数
    virtual void  SetParameter(PIXELFORMATDESCRIPTOR* pfd); // デバイスのパラメータを設定する．

    POINT GetWindowDisplaySize(POINT pt);   // ウィンドウが画面に収まるように大きさを再計算する
    BOOL  InitDevice(int xsize=0, int ysize=0, CWnd* cwnd=NULL);

    virtual int   GetMouseButton(void);

    DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    virtual void OnDraw(CDC* pDC) { CExView::OnDraw(pDC);}
};



/*
class  CGlVTXBaseView : public CGlBaseView
{
    //
    double  rTime, gTime, lTime, startTime;     // lTime: スピード調節用
    double  angle, angleX, angleY, angleZ;      // 回転角
    double  mWheelAc;
    double  mWheelNotch;
    bool    rotation;
    bool    reverseZ;

public:
    CGlVTXBaseView();
    virtual ~CGlVTXBaseView();

protected:
    virtual void  OnDraw(CDC* pDC);

public:
    virtual void  OnInitialUpdate() {}              // 初期化
    virtual void  SetState()   {}                   // 環境設定
    virtual void  ExecRender() {}                   // 描画
    virtual void  InitObject(void) { SetState();}   // デバイスリセット時のリソース再獲得関数

    virtual BOOL  PrepareVB() {return TRUE;}                    // 頂点バッファのセット
    virtual void  ReleaseVB() {DXRELEASE(vb); DXRELEASE(lb); }  // 頂点バッファの開放
    
    virtual void  SwitchRotation();                 // 回転モードの切り替え
    virtual void  ExecRotation();                   // 回転処理  ExRotation()を使用して回転方向を決める．

    virtual void  ExMouseWheel() {}                 // マウスホイールによる操作の切り替え
    virtual void  ExRotationAngle() {}              // 回転方向の入れ替え

    DECLARE_MESSAGE_MAP()
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};

*/


}       // namespace




#endif      // ENABLE_OPENGL
