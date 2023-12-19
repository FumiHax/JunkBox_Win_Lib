#pragma once

/**
CvThumbNailFrame クラス

    縦スクロールサムネイル画像の表示用フレーム

    vThumbNailDoc と DxVScrollView を繋ぐ
    
*/

#include  "appCallBack.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CvThumbNailFrame フレーム

class CvThumbNailFrame : public CExFrame
{
    DECLARE_DYNCREATE(CvThumbNailFrame)

protected:
    CvThumbNailFrame();

// アトリビュート
public:

// オペレーション
public:
    virtual void onLButtonDBLClick();   

// オーバーライド

    //{{AFX_VIRTUAL(CvThumbNailFrame)
    //}}AFX_VIRTUAL

protected:
    virtual ~CvThumbNailFrame();

    //{{AFX_MSG(CvThumbNailFrame)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}


}       // namespace
