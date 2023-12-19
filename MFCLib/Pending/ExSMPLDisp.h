#pragma once

/**
ExSMPLDisp.h : ヘッダー ファイル

    簡易画像表示
*/

#include  "ExClass.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CExSMPLDisp フレーム

class CExSMPLDisp : public CExFrame, public MSGraphDisp<sWord>
{
    DECLARE_DYNCREATE(CExSMPLDisp)

protected:
//  CExSMPLDisp();

// アトリビュート
public:
    CAppCallBack*   pApp;
    CExFrame*       pFrame;
    MSGraph<sWord>  msGraph;

// オペレーション
public:
    CExSMPLDisp(CAppCallBack* app=NULL) { pApp=app; pFrame=NULL; msGraph.init();}
    virtual  void  displayNew (MSGraph<sWord> vp);
    virtual  void  displayNext(MSGraph<sWord> vp);

// オーバーライド

    //{{AFX_VIRTUAL(CExSMPLDisp)
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    virtual ~CExSMPLDisp();

    //{{AFX_MSG(CExSMPLDisp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
