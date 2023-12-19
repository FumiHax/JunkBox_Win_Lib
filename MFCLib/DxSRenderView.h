#pragma once

/**
CDxSRenderView クラス （サーフェイスレンダリング）

    CDxVTXBaseViewクラスからの継承クラス
    対応する Document の msGraph (pDoc->msGraph) からデータを受け取り
    （cmnHeadではないので注意），（簡易）サーフェイスレンダリングを行い，
    DirectXの機能を使って 3D表示する．

    3D表示する場合の色は，ボクセルの輝度値(16bit)に対して，ARGB16(各4bit)
    と解釈する．輝度値(16bit)の設定には
        uWord  ARGB2Word(uWord a, uWord r, uWord g, uWord b)    (要 #include "Gdata.h")
        uWord  RGBA2Word(uWord r, uWord g, uWord b, uWord a)    (要 #include "Gdata.h")
        uWord  RGB2Word (uWord r, uWord g, uWord b)             (要 #include "Gdata.h")
    を使用すると良い．因みに逆の関数は
        D3DCOLOR  Dx9Word2RGBA(uWord pp)                        (要 #include "Dx9.h")
        D3DCOLOR  Dx9Word2ARGB(uWord pp)                        (要 #include "Dx9.h")
        D3DCOLOR  Dx9Word2RGB (uWord pp)                        (要 #include "Dx9.h")                  
*/


#include  "DxBaseClass.h"
#include  "ProgressBarDLG.h"


//
namespace jbxwl {


typedef struct _SVERTEXV
{
    float    x,  y,  z;     // D3DXVECTOR3  頂点座標
    float    nx, ny, nz;    // D3DXVECTOR3  法線ベクトル
    float    ps;            // ピクセルサイズ
    D3DCOLOR color;         // DWORD        ディフューズ色
//  float    tu, tv;        // D3DXVECTOR2  テクスチャ座標
} SVERTEXV;



//#define    FVF_SVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
//#define    FVF_SVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define  FVF_SVERTEXV (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_PSIZE | D3DFVF_DIFFUSE)
//#define    FVF_SVERTEXV (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)


/////////////////////////////////////////////////////////////////////////////
// CDxSRenderView ビュー

class  CDxSRenderView : public CDxVTXBaseView
{
protected:
    CDxSRenderView();           // 動的生成に使用されるプロテクト コンストラクタ
    DECLARE_DYNCREATE(CDxSRenderView)

// アトリビュート
public:
    RBound<int>  Rbound;
    CProgressBarDLG*  pCounter;

    int     xsize;
    int     ysize;
    int     zsize;
    int     msize;

    int     datano;

    BOOL    volumeColor;

// オペレーション
public:
//  CDxGRDoc* GetDocument() { return (CDxGRDoc*)m_pDocument;}

    virtual void    SetState();
    virtual BOOL    PrepareVB();

    virtual void    ExecRender();
    virtual void    ClearObject();
    virtual void    InitObject();

    virtual void    ExRotationAngle();
    virtual void    ExMouseWheel();

    int     GetValidVoxelNum();

    // オーバーライド

    //{{AFX_VIRTUAL(CDxSRenderView)
public:
    virtual void OnInitialUpdate();
protected:
    virtual void OnDraw(CDC* pDC) {}    
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    virtual ~CDxSRenderView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif


protected:
    //{{AFX_MSG(CDxSRenderView)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
