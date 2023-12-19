#pragma once

#include  "ExClass.h"
#include  "Dx9.h"

#include  <mmsystem.h>


//
namespace jbxwl {


/*
CDxBaseView クラス

    CExView クラスを継承．
    DirectX を使用した，データを表示するための基本テンプレートクラス．
    2D表示を行う場合は，このクラスを継承するとよい．
    一方，3D表示を行う場合は，CDxVTXBaseView クラスを継承するとよい．
    通常，CExDocument から cmnHeadをもらって，データの表示を行う．（推奨動作）

    制御用変数
        BOOL hasViewData;   // 表示可能な用データを持っているか？ 
                            // hasViewData==TRUE でなければ，データは表示されない．

    このクラスを継承するクラスは，少なくとも以下の関数をオーバーライドする必要がある．
        void    OnInitialUpdate();      // クラスの初期化
        void    ExecRender();           // 表示ルーチン
        void    InitObject(void);       // デバイスリセット時のリソース再獲得関数

    また，必要なら以下の関数もオーバーライドする．
        void  SetParameter(D3DPRESENT_PARAMETERS*, int, int);   // デバイスのパラメータを設定する．
 
    Direct Input のマウスの初期化機能．OnActivateView()

    以下 CExViewクラスから継承した機能
        ウィンドウサイズのジャストフィット機能あり．v(^^
        ウィンドウのリサイズ制御機能あり．bool cnstSize, cnstXSize, cnstYSize, cnstXYRate;  
        タイマ割り込み機能あり．TimerStart() でタイマを始動させる．
        マウス制御機能（簡易版）あり．  GetMousePos() （クライアント領域でのマウスの座標）
      
        垂直スクロールテンプレート機能 ('04 8/28)
            垂直スクロールバーを使用する場合は，vSBmaxを設定し，SetScrollRange(SB_VERT, 0, vSBmax);
            を呼ぶ．スクロールバーの位置は vSBposに設定される．
            スクロールバーを動かした場合は，SetNewSurface(int ovsbpos)をオーバーライドし，サーフェイス
            を更新する．ovsbposはスクロールバーの以前の位置．
*/


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
CDxVTXBaseView クラス

    CDxBaseView クラスを継承．
    DirectX で 3D表示を行うためのテンプレートクラス．
    通常，CExDocument から cmnHeadをもらって，データの表示を行う．（推奨動作）

    制御用変数
        BOOL hasViewData;   // 表示可能な用データを持っているか？ 
                            // hasViewData==TRUE でなければ，データは表示されない．

    このクラスを継承するクラスは，少なくとも以下の関数をオーバーライドする必要がある．
        void    OnInitialUpdate();      // クラスの初期化
        void    ExecRender();           // 表示ルーチン
        void    SetState();             // 環境設定
        BOOL    PrepareVB();            // 頂点バッファのセット

    また，必要なら以下の関数もオーバーライドする．
        void    InitObject(void);   // デバイスリセット時のリソース再獲得関数
        void    ReleaseVB();            // 頂点バッファの開放
        void    ExRotationAngle();      // 回転方向の入れ替え
      
    回転制御機能あり
    Direct Input のマウスの初期化機能．OnActivateView()  (CDxBaseViewクラスより）

    以下 CExViewクラスから継承した機能
        ウィンドウサイズのジャストフィット機能あり．v(^^
        ウィンドウのリサイズ制御機能あり．bool cnstSize, cnstXSize, cnstYSize, cnstXYRate;  
        タイマ割り込み機能あり．TimerStart() でタイマを始動させる．
        マウス制御機能（簡易版）あり．  GetMousePos() （クライアント領域でのマウスの座標）
      
        垂直スクロールテンプレート機能 ('04 8/28)
            垂直スクロールバーを使用する場合は，vSBmaxを設定し，SetScrollRange(SB_VERT, 0, vSBmax);
            を呼ぶ．スクロールバーの位置は vSBposに設定される．
            スクロールバーを動かした場合は，SetNewSurface(int ovsbpos)をオーバーライドし，サーフェイス
            を更新する．ovsbposはスクロールバーの以前の位置．
*/


class  CDxBaseView;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
HRESULT  ResetDx9Device(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*, CDxBaseView* pviw=NULL);

    機能：  DirectX9のデバイスをリセットする．

    引数： lpD3DDevice     リセットするDirectX9デバイス 
            *d3dParam       デバイスリセットのためのパラメータ．作成時に使用したものを指定する．
            *pviw           このデバイスを持っている CDxBaseViewクラスを指定する．省略可能（NULL）  

    戻値：  リセットの HRESULT

    説明：  D3DPOOL_DEFAULT管理のリソースなどは，デバイスのリセットを行う前にそれを開放し，
            リセット後に再び獲得しなければならない．pviwに CDxBaseViewクラスのポインタを設定
            した場合，解放関数として pviw->ClearObject() が呼び出され，再獲得関数として 
            pviw->InitObject()が呼び出される．この２つの関数は，CDxBaseViewクラスの継承クラス
            の中でオーバーライドされなければならない．pviw に NULLを指定した場合は，リソース
            の開放・再獲得は行われない．

            CDxBaseView* を引数に持つので，Dx9.cpp へは入れられない．
*/

HRESULT  ResetDx9Device(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*, CDxBaseView* pviw=NULL);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DirectX Viewクラス(基本形)
//
class   CDxBaseView : public CExView  
{
public:
    LPDIRECT3D9             lpD3D;          // Dx9         リリース禁止
    LPDIRECT3DDEVICE9       lpD3DDevice;    // Dx9 3Dデバイスインターフェイス
    LPDIRECT3DSURFACE9      lpBackBuffer;   // バックバッファ
    LPDIRECT3DSURFACE9      lpSurface;      // サーフェース
    LPDIRECT3DTEXTURE9      lpTexture;      // テクスチャ
    D3DPRESENT_PARAMETERS   d3dParam;       // 3Dデバイスパラメータ

    LPDIRECTINPUT8          lpDInput;       // Dx Input                      リリース禁止
    LPDIRECTINPUTDEVICE8    lpDMouse;       // マウスのインターフェイス      リリース禁止
    LPDIRECTINPUTDEVICE8    lpDKeyBoard;    // キーボードのインターフェイス  リリース禁止

    POINT                   bufferSize;

public:
    CDxBaseView();
    virtual ~CDxBaseView();

protected:
    virtual void  OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
    virtual void  OnInitialUpdate(void) {}          // 初期化
    virtual void  SetState(void) {}                 // 環境設定
    virtual void  ExecRender(void) {}               // 描画
    virtual void  ClearObject(void);                // デバイスリセット時のリソース開放関数
    virtual void  InitObject(void) { SetState();}   // デバイスリセット時のリソース再獲得関数
    virtual void  SetParameter(D3DPRESENT_PARAMETERS* d3dParam, int xsize, int ysize);  // デバイスのパラメータを設定する．

    POINT GetWindowDisplaySize(POINT pt);           // ウィンドウが画面に収まるように大きさを再計算する
    BOOL  InitDevice(int xsize=0, int ysize=0, CWnd* cwnd=NULL);    // DirectX9のデバイスを作る

    virtual int   GetMouseButton();
    DECLARE_MESSAGE_MAP()
};



////////////////////////////////////////////////////////////////////////
// DirectX Vertex View クラス
//

class  CDxVTXBaseView : public CDxBaseView
{
public:
    LPDIRECT3DVERTEXBUFFER9 vb, lb;     // 頂点バッファへのポインタ

    D3DXMATRIX  matWorld,  matView,  matProj;
    D3DXMATRIX  matTrans,  matTemp;
    D3DXMATRIX  matRotate, matRotateX, matRotateY, matRotateZ;
    D3DXVECTOR3 center;

    double  rTime, gTime, lTime, startTime;     // lTime: スピード調節用
    double  angle, angleX, angleY, angleZ;      // 回転角
    double  mWheelAc;
    double  mWheelNotch;
    bool    rotation;
    bool    reverseZ;

public:
    CDxVTXBaseView();
    virtual ~CDxVTXBaseView();

protected:
    virtual void  OnDraw(CDC* pDC);

public:
    virtual void  OnInitialUpdate(void) {}          // 初期化
    virtual void  SetState(void)   {}               // 環境設定
    virtual void  ExecRender(void) {}               // 描画
    virtual void  InitObject(void) {SetState();}    // デバイスリセット時のリソース再獲得関数

    virtual BOOL  PrepareVB(void) {return TRUE;}                    // 頂点バッファのセット
    virtual void  ReleaseVB(void) {DXRELEASE(vb); DXRELEASE(lb);}   // 頂点バッファの開放
    
    virtual void  SwitchRotation(void);             // 回転モードの切り替え
    virtual void  ExecRotation(void);               // 回転処理  ExRotation()を使用して回転方向を決める．

    virtual void  ExMouseWheel(void) {}             // マウスホイールによる操作の切り替え
    virtual void  ExRotationAngle(void) {}          // 回転方向の入れ替え

    DECLARE_MESSAGE_MAP()
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};


}       // namespace
