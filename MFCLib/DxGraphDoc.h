#pragma once


/**
CDxGraphDoc クラス
    
    CExDocumentクラスからの継承クラス．
    DirectXの機能を利用して，読み込んだ画像のフォーマットを解析する．

    画像データは cmnHead, msGraph メンバに格納される．ただし，画像ファイル
    のフォーマットが未知の場合は，cmnHead.kind==UN_KNOWN となり，msGraphには
    画像は格納されない．画像ファイルのフォーマットが未知の場合（UN_KNOWN）は，
    DirectXの機能を使用して，フォーマット解析を行う（msGraphにはデータは無し）．
    
    ここで言う「画像データのフォーマットが未知」とは作者が画像フォーマットを
    知らないということ．jpeg, bmp等も含む．

    画像ファイルのフォーマットが既知の場合（CT_DATA, CT_3DM, RAS_DATAなど），
    画像データは msGraphにもコピーされ，cmnHeadの画像データ自体は 8bit化
    される．

    通常は cmnHead が Viewにわたされる．

        OnOpenDocument(LPCTSTR) -> ReadDataFile(LPCTSTR);
                                -> MakeViewData(void);
*/

#include  "DxBaseClass.h"
#include  "Gio.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CDxGraphDoc ドキュメント

class CDxGraphDoc : public CExDocument  //CDocument
{
protected:
    CDxGraphDoc(); 
    DECLARE_DYNCREATE(CDxGraphDoc)

// アトリビュート
public:

// オペレーション
public:
    BOOL    MakeViewData();
    BOOL    ReadDataFile(LPCTSTR fname);


// オーバーライド
public:
    //{{AFX_VIRTUAL(CDxGraphDoc)
    virtual void Serialize(CArchive& ar); 
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual ~CDxGraphDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CDxGraphDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}


}       // namespace
