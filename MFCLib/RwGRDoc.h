#pragma once

/**
CRwGRDoc クラス

    画像ファイルを読み込む．CExDocumentクラスを継承．

    予め cmnHeadにデータがある場合は，hasReadData=TRUE にする．
    multiSliceData==TRUE の場合は，マルチスライス読み込みモードになり3Dデータとして読み込む．

    画像データは 最初 msGraphに読み込まれ, MakeViewData()により cmnHeadへコピーされ（別の
    データ領域） 8bit化される．
    ただしメモリを節約したい場合，nonCmanHead==TRUEにすれば，データはcmnHeadにはコピーされない．
    また，MakeViewData()をオーバーライドしてもよい．

    通常は cmnHead が Viewにわたされる．
*/

#include  "ExClass.h"


//
namespace jbxwl {



/////////////////////////////////////////////////////////////////////////////
// CRwGRDoc ドキュメント

class CRwGRDoc : public CExDocument  //CDocument
{
protected:
    CRwGRDoc(); 
    DECLARE_DYNCREATE(CRwGRDoc)

// アトリビュート
public:
//  BOOL    multiSliceData; // TRUE ならマルチスライス読み込みモード
//  BOOL    nonCmnHead;     // TRUE なら cmnHeadにデータを入れない．MakeViewData()では何もしない．

// オペレーション
public:
    virtual BOOL    ReadDataFile(LPCTSTR str);
    virtual BOOL    MakeViewData();
    virtual CmnHead TranslateData();

// オーバーライド
    //{{AFX_VIRTUAL(CRwGRDoc)
    public:
    virtual void Serialize(CArchive& ar); 
    //}}AFX_VIRTUAL

// インプリメンテーション
public:
    virtual ~CRwGRDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    //{{AFX_MSG(CRwGRDoc)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:

};



//{{AFX_INSERT_LOCATION}}


}       // namespace
