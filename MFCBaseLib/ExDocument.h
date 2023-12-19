#pragma once

/**
CExDocument クラス

    ファイルを読み込むためのテンプレートクラス．

    制御用変数
        BOOL hasReadData;   // 読み込みデータを持っているか？
        BOOL hasViewData;   // 表示用データに加工済みか？

    予め cmnHeadにデータがある場合は，hasReadData=TRUE にしてOnNewDocument(void)を呼び出す．
    データを読み込む場合は OnOpenDocument(LPCTSTR) を呼ぶ．

    OnOpenDocument(LPCTSTR) で実際にデータを読み込むためには ReadDataFile(LPCTSTR) をオーバーライドする．
    この際データは msGraph, cmnHeadの両方（別々の領域）に読み込むようにすると良い．

    データ加工用の関数は MakeData(void) である．これをオーバーライドして，cmnHeadを表示用データ
    に加工し，CExViewに渡す．

    データの書き込みは  未実装．
*/

#include  "MFCBase.h"
#include  "Gio.h"


//
namespace jbxwl {

using namespace jbxl;


class   CExDocument;
class   CExView;
class   CExFrame;
class   CAppCallBack;


/////////////////////////////////////////////////////////////////////////////
// CExDocument ドキュメント (画像データ用)

class CExDocument : public CDocument
{
protected:
    CExDocument();
    DECLARE_DYNCREATE(CExDocument)

// アトリビュート
public:
    CmnHead          infHead;       // 読み込みファイルのヘッダ情報（読み込みの前準備）
    ExMSGraph<sWord> msGraph;       // 読み込みデータ
    ExCmnHead        cmnHead;       // 表示用データ

    CExView*        pView;
    CExFrame*       pFrame;
    CAppCallBack*   pApp;
    void*           anyData;

    CString         Title;
    CString         preTitle;
    CString         pstTitle;

    int             vMax;           // データの最大値
    int             vMin;           // データの最小値
    int             cMax;           // コントラストの最大値
    int             cMin;           // コントラストの最小値

    int             base;           // データのベース値（0以下にならないように底上げした場合の値）
    int             colorMode;      // データのカラータイプ
    
    int             startNo;        // マルチスライス読み込み時の開始番号．
    int             endNo;          // マルチスライス読み込み時の終了番号．  startNo > endNo でも可．
    
    BOOL            multiSliceData; // TRUE ならマルチスライス読み込みモード
    BOOL            hasReadData;    // ファイルデータを正常に msGraph に読み込み，保持しているか．          
    BOOL            hasViewData;    // 読み込んだデータ msGraph から表示用データ cmnHead を作成したか． 
    BOOL            ctrlCntrst;     // 表示用データ作成時に（画素値が255以下でも）強制的にコントラストを調整を行う．

    BOOL            notFreeMSGraph; // メモリ管理無しの場合，msGraph のメモリ開放を禁止（msGraphは外部からの持込み）
    BOOL            notFreeCmnHead; // メモリ管理無しの場合，cmnHead のメモリ開放を禁止（cmnHeadは外部からの持込み）

// オペレーション
public:
    CExView*        GetView();              // 関連付けられた Viewクラスへのポインタを得る．

    virtual  BOOL   ReadDataFile(LPCTSTR);  // ファイルデータを msGraphへ読み込み
    virtual  BOOL   WriteDataFile(LPCTSTR); // データをファイルへ書き出し
    virtual  BOOL   MakeViewData();         // 読み込みデータ msGraph から表示用データ cmnHeadを作成する．

// オーバーライド
public:
    //{{AFX_VIRTUAL(CExDocument)
    virtual  void   Serialize(CArchive& ar);
    virtual  BOOL   OnNewDocument();
    virtual  BOOL   OnOpenDocument(LPCTSTR lpszPathName);
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual ~CExDocument();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CExDocument)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


}       // namespace
