#pragma once

/**
    CExToolBar

    ウィンドウのリサイズ時，移動時のちらつき防止効果付きツールバー

*/

#include  "MFCBase.h"


namespace jbxwl {


class CExFrame;


// CExToolBar

class CExToolBar : public CToolBar
{
    DECLARE_DYNAMIC(CExToolBar)

public:
    //CExFrame* pFrame;
    UINT        rsrcID;

public:
    //CExToolBar(CExFrame* pfrm=NULL);
    CExToolBar(void);
    virtual ~CExToolBar();

    BOOL  LoadToolBar(UINT id);

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


}       // namespace
