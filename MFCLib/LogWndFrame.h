#pragma once


/**
CLogWndFrame クラス

    CDxMGRView クラス用フレーム


*/

#include  "ExTextFrame.h"
#include  "LogWndDoc.h"
//#include  "LogWndView.h"


//
namespace jbxwl {


/////////////////////////////////////////////////////////////////////////////
// CLogWndFrame フレーム

class CLogWndFrame : public CExTextFrame
{
    DECLARE_DYNCREATE(CLogWndFrame)
protected:
    CLogWndFrame(); 

// アトリビュート
public:
    CLogWndDoc*   pDoc;
    CLogWndView*  pView;

// オペレーション
public:

// オーバーライド
    //{{AFX_VIRTUAL(CLogWndFrame)
    //}}AFX_VIRTUAL

// インプリメンテーション
protected:
    virtual ~CLogWndFrame();

    //{{AFX_MSG(CLogWndFrame)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLogCopy();
    afx_msg void OnLogSave();
    afx_msg void OnLogClear();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

public:
//  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}



/////////////////////////////////////////////////////////////////////////////
//

CLogWndFrame*  ExecLogWnd(CMultiDocTemplate* pDocTempl, LPCTSTR title, CAppCallBack* app);


}       // namespace
