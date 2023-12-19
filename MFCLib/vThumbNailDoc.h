#pragma once

/**
CvThumbNailDoc クラス

    CRwGRDocクラスからの継承クラス．
    複数または 3Dのファイルを読み込んで，２列の縦スクロール用サムネイル画像
    のデータを作り出す．UN_KNOWNデータは処理できない．

    読み込んだ元データは msGraphに格納され，サムネイルデータは cmnHeadに格納
    され，8bit化される．

    通常は DxVScrollView で表示される．

    サムネイル，ウィンドウの大きさは，OnOpenDocument(LPCTSTR) を呼び出す前なら
        SetSize(int txs, int tys, int bds=4, int vrs=4, int hrs=2); 
            txs: サムネイルの Xサイズ
            tys: サムネイルの Yサイズ
            bds: サムネイル間の境界のサイズ
            vrs: ウィンドウのクライアント領域の Xサイズ．(txs+bds)の倍率で指定
            hrs: ウィンドウのクライアント領域の Yサイズ．(tys+bds)の倍率で指定
    の関数で変える事ができる．
    
    cmnHeadを Viewが使用する場合のために，cmnHead.zsize にクライアント領域の初期
    サイズ（Yサイズ）が格納される（Zサイズではない）．
*/

#include  "RwGRDoc.h"


#define  THUMBNAIL_SIZE  128


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CvThumbNailDoc ドキュメント

class CvThumbNailDoc : public CRwGRDoc  //public CDocument
{
protected:
    CvThumbNailDoc();           // 動的生成に使用されるプロテクト コンストラクタ．
    DECLARE_DYNCREATE(CvThumbNailDoc)

// アトリビュート
public:
    int  tNailBorder;       // サムネイル間のボーダサイズ（ピクセル）

    int  xTNailSize;        // サムネイルの Xサイズ（ピクセル）
    int  yTNailSize;        // サムネイルの Yサイズ（ピクセル）
    
    int  xTNailOrder;       // ウィンドウの初期 Xサイズ（サムネイルの個数で指定）
    int  yTNailOrder;       // ウィンドウの初期 Yサイズ（サムネイルの個数で指定）
    
    int  xClientSize;       // クライントウィンドウの初期 Xサイズ（ピクセル）
    int  yClientSize;       // クライントウィンドウの初期 Yサイズ（ピクセル）

    int  yMaxOrder;         // クライントウィンドウの最大 Yサイズ（サムネイルの個数で指定）
    int  yMaxClientSize;    // クライントウィンドウの最大 Yサイズ（ピクセル）．作業メモリに関係する．

// オペレーション
public:
//  virtual BOOL    MakeViewData();
    virtual CmnHead TranslateData();

    void    SetSize(int txs, int tys, int bds, int vrs, int hrs, int ymx);
    int     GetTNailNum(POINT pt);

//  virtual void  MouseCommand(POINT pt);

// オーバーライド
    //{{AFX_VIRTUAL(CvThumbNailDoc)
    public:
    virtual void Serialize(CArchive& ar);
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual ~CvThumbNailDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CvThumbNailDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:

};


//{{AFX_INSERT_LOCATION}}


}       // namespace
